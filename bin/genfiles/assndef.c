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
extern void*Cyc_List_nth(struct Cyc_List_List*,int);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_AssnMap*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_AssnMap*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_AssnMap*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};
# 905 "absyn.h"
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 911
void*Cyc_Absyn_compress(void*);
# 929
extern void*Cyc_Absyn_uint_type;
# 931
extern void*Cyc_Absyn_sint_type;
# 938
extern void*Cyc_Absyn_heap_rgn_type;
# 940
extern void*Cyc_Absyn_al_qual_type;
# 944
extern void*Cyc_Absyn_false_type;
# 969
void*Cyc_Absyn_exn_type (void);
# 991
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 72
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);struct _tuple11{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 35 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 41
int Cyc_Tcutil_is_pointer_type(void*);
# 59
void*Cyc_Tcutil_pointer_elt_type(void*);
# 125
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
# 102
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 116
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;void*f2;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Select_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Update_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Aggr_AssnDef_Term_struct{int tag;int f1;unsigned f2;struct Cyc_List_List*f3;void*f4;};struct Cyc_AssnDef_Proj_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Alloc_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;void*f4;void*f5;};struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Offseti_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Tagof_AssnDef_Term_struct{int tag;void*f1;};
# 148 "assndef.h"
extern struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory;
# 166
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
# 168
void*Cyc_AssnDef_aggr(int,unsigned,struct Cyc_List_List*,void*);
# 170
void*Cyc_AssnDef_proj(void*,unsigned,void*);
# 174
void*Cyc_AssnDef_offsetf(void*,unsigned,void*);
# 179
void*Cyc_AssnDef_plus(void*,void*,void*);struct _tuple12{void*f0;struct Cyc_List_List*f1;};
# 199
void*Cyc_AssnDef_get_term_type(void*);
# 212 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 224
int Cyc_AssnDef_assncmp(void*,void*);
# 228
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 233
void*Cyc_AssnDef_not(void*);struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};struct _tuple13{void*f0;void*f1;struct Cyc_Dict_Dict f2;};
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _fat_ptr tab;};
# 71 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_typopt2string(void*tp){
if(tp==0)return _tag_fat("NULL",sizeof(char),5U);
return Cyc_Absynpp_typ2string(tp);}static char _TmpG0[2U]=",";static char _TmpG1[2U]="}";static char _TmpG2[2U]="{";
# 76
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
struct _fat_ptr res;
{enum Cyc_Absyn_Primop _Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("0x%x",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_exp2string(e);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)((int)e);_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s[%d]",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 15: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("tagof_tm(%s)",sizeof(char),13U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 6: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*tp=_Tmp2;
# 83
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_typopt2string(tp);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Sel(%s,%s,%s)",sizeof(char),14U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 7: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;
# 86
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_term2string(t3);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Upd(%s,%s,%s)",sizeof(char),14U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 2: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1==0){_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{int i=_Tmp1;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("_X%d",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}}else{_Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp1;
# 90
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("_%s%d",sizeof(char),6U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}}case 3: _Tmp0=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;
# 92
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_prim2string(p);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s(%s)",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 4: _Tmp0=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;void*t2=_Tmp3;
# 94
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_Absynpp_prim2string(p);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("(%s%s%s)",sizeof(char),9U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp4;void*tm=_Tmp3;
# 98
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_typ2string(tp);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(tm);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("(%s)%s",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 8: _Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{int is_union=_Tmp1;unsigned tag=_Tmp5;struct Cyc_List_List*tms=_Tmp4;
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
if(tag!=4294967295U)res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)tag);_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=(struct _fat_ptr)res;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("tag(%d)%s",sizeof(char),10U),_tag_fat(_Tmp8,sizeof(void*),2));});
if(is_union)res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)res;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("union%s",sizeof(char),8U),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL0;}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp4;unsigned i=_Tmp5;void*tp=_Tmp3;
# 116
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)i);_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("#%d(%s)",sizeof(char),8U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}case 10: _Tmp4=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*a=_Tmp4;unsigned i=_Tmp5;void*v=_Tmp3;
# 119
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(a);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)((int)i);_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_term2string(v);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Aupd(%s,%d,%s)",sizeof(char),15U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 11: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;void*invoke=_Tmp3;
# 122
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(invoke);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned)vd;_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("&%s_%s_%x",sizeof(char),10U),_tag_fat(_Tmp9,sizeof(void*),3));});
# 125
goto _LL0;}case 12: _Tmp4=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;{struct Cyc_Absyn_Exp*e=_Tmp4;int id=_Tmp1;void*nelts=_Tmp3;void*invoke=_Tmp2;
# 127
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)e);_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)id;_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_term2string(nelts);_Tmp9.f1=_TmpA;});_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,({struct _fat_ptr _TmpB=(struct _fat_ptr)Cyc_AssnDef_term2string(invoke);_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[4];_TmpA[0]=& _Tmp6,_TmpA[1]=& _Tmp7,_TmpA[2]=& _Tmp8,_TmpA[3]=& _Tmp9;Cyc_aprintf(_tag_fat("Alloc(%d,%d,%s,%s)",sizeof(char),19U),_tag_fat(_TmpA,sizeof(void*),4));});
goto _LL0;}case 13: _Tmp4=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp4;unsigned i=_Tmp5;void*tp=_Tmp3;
# 130
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)((int)i);_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_typopt2string(tp);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Offsetf(%s,%d,%s)",sizeof(char),18U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}default: _Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp3;
# 133
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s.+.%s",sizeof(char),8U),_tag_fat(_Tmp8,sizeof(void*),2));});
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
return(int)(((unsigned)t >> 4U)+ 741U);}};}struct _tuple14{void*f0;void*f1;};
# 205
int Cyc_AssnDef_termcmp(void*_t1,void*_t2){
void*t1=_t1;
void*t2=_t2;
LOOP: {
int c=(int)(*((const unsigned*)t1)- *((const unsigned*)t2));
if(c!=0)return c;
if(t1==t2)return 0;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;int _Tmp3;int _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;enum Cyc_Absyn_Primop _Tmp8;void*_Tmp9;void*_TmpA;void*_TmpB;enum Cyc_Absyn_Primop _TmpC;switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_TmpC=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp9=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp8=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f4;{enum Cyc_Absyn_Primop p1=_TmpC;void*t11=_TmpB;void*t12=_TmpA;void*tp1=_Tmp9;enum Cyc_Absyn_Primop p2=_Tmp8;void*t21=_Tmp7;void*t22=_Tmp6;void*tp2=_Tmp5;
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
{void*_Tmp0=Cyc_Absyn_compress(tp);if(*((int*)_Tmp0)==4)
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
LOOP: {
void*_Tmp0;void*_Tmp1;if(*((int*)t1)==7){_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t1)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t1)->f2;if((void*)_Tmp0==t2){void*m=_Tmp1;void*t=_Tmp0;
# 484
t1=m;
goto LOOP;}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 488
struct Cyc_AssnDef_Update_AssnDef_Term_struct t=({struct Cyc_AssnDef_Update_AssnDef_Term_struct _Tmp0;_Tmp0.tag=7,_Tmp0.f1=t1,_Tmp0.f2=t2,_Tmp0.f3=t3;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 492
void*Cyc_AssnDef_aggr_update(void*t1,unsigned i,void*t2){
LOOP: {
void*_Tmp0;int _Tmp1;unsigned _Tmp2;void*_Tmp3;switch(*((int*)t1)){case 10: _Tmp3=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f1;_Tmp2=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f2;if(i==(unsigned)_Tmp2){void*t=_Tmp3;unsigned j=_Tmp2;
# 496
t1=t;
goto LOOP;}else{goto _LL5;}case 8: _Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f1;_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;_Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f4;{int is_union=_Tmp1;unsigned tag=_Tmp2;struct Cyc_List_List*ts=_Tmp3;void*tp=_Tmp0;
# 500
if(is_union){int _Tmp4=is_union;unsigned _Tmp5=i;struct Cyc_List_List*_Tmp6=({void*_Tmp7[1];_Tmp7[0]=t2;Cyc_List_list(_tag_fat(_Tmp7,sizeof(void*),1));});return Cyc_AssnDef_aggr(_Tmp4,_Tmp5,_Tmp6,tp);}{
# 502
struct Cyc_List_List*revnewts=0;
for(1;i!=0U;-- i){
if(ts==0)goto _LL6;
revnewts=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=(void*)ts->hd,_Tmp4->tl=revnewts;_Tmp4;});
ts=ts->tl;}
# 508
if(ts==0)goto _LL6;{
struct Cyc_List_List*newts=({struct Cyc_List_List*_Tmp4=revnewts;Cyc_List_revappend(_Tmp4,({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=t2,_Tmp5->tl=ts->tl;_Tmp5;}));});
void*res=Cyc_AssnDef_aggr(is_union,tag,newts,tp);
return res;}}}default: _LL5: _LL6: {
# 513
struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct t=({struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct _Tmp4;_Tmp4.tag=10,_Tmp4.f1=t1,_Tmp4.f2=i,_Tmp4.f3=t2;_Tmp4;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}};}}
# 518
static int Cyc_AssnDef_address_disjoint(void*,void*);
# 521
static int Cyc_AssnDef_term_neq(void*t1,void*t2){
LOOP: {
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;unsigned _Tmp6;switch(*((int*)_Tmp0.f0)){case 0: switch(*((int*)_Tmp0.f1)){case 0: _Tmp6=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp6;unsigned j=_Tmp5;
return i!=j;}case 4: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Plus)goto _LL7;else{goto _LLD;}case 3: if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}default: goto _LLD;}case 4: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1==Cyc_Absyn_Plus){if(*((int*)_Tmp0.f1)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Plus){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((void*)_Tmp4==(void*)_Tmp3){void*t11=_Tmp4;void*t12=_Tmp3;void*t21=_Tmp2;void*t22=_Tmp1;
# 527
t1=t12;
t2=t22;
goto LOOP;}else{goto _LL5;}}else{goto _LL5;}}else{_LL5: _Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp2=_Tmp0.f1;if((void*)_Tmp4==(void*)_Tmp2){void*t11=_Tmp4;void*t12=_Tmp3;void*t21=_Tmp2;
# 531
t1=t12;
t2=Cyc_AssnDef_zero();
goto LOOP;}else{if(*((int*)_Tmp0.f1)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}}else{goto _LLD;}}}}else{switch(*((int*)_Tmp0.f1)){case 4: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Plus)goto _LL7;else{goto _LLD;}case 3: if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}default: goto _LLD;}}default: if(*((int*)_Tmp0.f1)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Plus){_LL7: _Tmp4=_Tmp0.f0;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;if((void*)_Tmp4==(void*)_Tmp3){void*t11=_Tmp4;void*t21=_Tmp3;void*t22=_Tmp2;
# 535
t1=Cyc_AssnDef_zero();
t2=t22;
goto LOOP;}else{if(*((int*)_Tmp0.f0)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1==Cyc_Absyn_Bitnot)goto _LL9;else{goto _LLD;}}else{goto _LLD;}}}else{if(*((int*)_Tmp0.f0)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1==Cyc_Absyn_Bitnot)goto _LL9;else{goto _LLD;}}else{goto _LLD;}}}else{if(*((int*)_Tmp0.f0)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1==Cyc_Absyn_Bitnot){_LL9: _Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=_Tmp0.f1;if((void*)_Tmp4==(void*)_Tmp3){void*t1=_Tmp4;void*t2=_Tmp3;
return 1;}else{if(*((int*)_Tmp0.f1)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}}else{goto _LLD;}}}else{if(*((int*)_Tmp0.f1)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot)goto _LLB;else{goto _LLD;}}else{goto _LLD;}}}else{if(*((int*)_Tmp0.f1)==3){if(((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1==Cyc_Absyn_Bitnot){_LLB: _Tmp4=_Tmp0.f0;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;if((void*)_Tmp4==(void*)_Tmp3){void*t1=_Tmp4;void*t2=_Tmp3;
return 1;}else{goto _LLD;}}else{goto _LLD;}}else{_LLD:
# 542
 return Cyc_AssnDef_address_disjoint(t1,t2);}}}};}}
# 548
static int Cyc_AssnDef_address_disjoint(void*t1,void*t2){
LOOP:
 if(t1==t2)return 0;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;void*_Tmp9;void*_TmpA;switch(*((int*)_Tmp0.f0)){case 11: switch(*((int*)_Tmp0.f1)){case 11: _TmpA=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*x=_TmpA;void*nv1=_Tmp9;struct Cyc_Absyn_Vardecl*y=_Tmp8;void*nv2=_Tmp7;
# 554
return x!=y;}case 12:
 return 1;case 13: goto _LLF;case 14: goto _LL13;default: goto _LL15;}case 12: switch(*((int*)_Tmp0.f1)){case 11:
 return 1;case 12: _TmpA=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp6=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp8=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp7=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp3=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f4;{struct Cyc_Absyn_Exp*exp1=_TmpA;int i1=_Tmp6;void*tm1=_Tmp9;void*nv1=_Tmp8;struct Cyc_Absyn_Exp*exp2=_Tmp7;int i2=_Tmp5;void*tm2=_Tmp4;void*nv2=_Tmp3;
# 561
return exp1!=exp2 || i1!=i2;}case 13: goto _LLF;case 14: goto _LL13;default: goto _LL15;}case 13: if(*((int*)_Tmp0.f1)==13){_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tm1=_TmpA;unsigned i=_Tmp2;void*tm2=_Tmp9;unsigned j=_Tmp1;
# 565
if(tm1==tm2 && i!=j)return 1;
t1=tm1;
t2=tm2;
goto LOOP;}}else{_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;{void*tm1=_TmpA;
# 577
t1=tm1;
goto LOOP;}}case 14: switch(*((int*)_Tmp0.f1)){case 14: _TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp8=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tm1=_TmpA;void*i1=_Tmp9;void*tm2=_Tmp8;void*i2=_Tmp7;
# 570
if(tm1==tm2 && Cyc_AssnDef_term_neq(i1,i2))return 1;
t1=tm1;
t2=tm2;
goto LOOP;}case 13: goto _LLF;default: _TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;{void*tm1=_TmpA;
# 583
t1=tm1;
goto LOOP;}}default: switch(*((int*)_Tmp0.f1)){case 13: _LLF: _TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*tm2=_TmpA;
# 580
t2=tm2;
goto LOOP;}case 14: _LL13: _TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*tm2=_TmpA;
# 586
t2=tm2;
goto LOOP;}default: _LL15:
 return 0;}};}}struct _tuple15{int f0;void*f1;};
# 592
struct _tuple12 Cyc_AssnDef_split_addr(void*a){
struct Cyc_List_List*fields=0;
while(1){
{void*_Tmp0;unsigned _Tmp1;void*_Tmp2;if(*((int*)a)==13){_Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)a)->f3;{void*r=_Tmp2;unsigned i=_Tmp1;void*tp=_Tmp0;
# 597
fields=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp4=({struct _tuple15*_Tmp5=_cycalloc(sizeof(struct _tuple15));_Tmp5->f0=(int)i,_Tmp5->f1=tp;_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=fields;_Tmp3;});
a=r;
continue;}}else{
# 601
struct _tuple12 _Tmp3;_Tmp3.f0=a,_Tmp3.f1=fields;return _Tmp3;};}
# 595
1U;}}
# 606
void*Cyc_AssnDef_apply_aggr_update(void*a,struct Cyc_List_List*fields,void*v){
if(fields==0)return v;{
struct _tuple15*_Tmp0=(struct _tuple15*)fields->hd;void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{int i=_Tmp2;void*tpopt=_Tmp1;
void*tp=0;
if(tpopt!=0 && Cyc_Tcutil_is_pointer_type(tpopt))
tp=Cyc_Tcutil_pointer_elt_type(tpopt);{
# 613
void*res=({void*_Tmp3=a;unsigned _Tmp4=(unsigned)i;Cyc_AssnDef_aggr_update(_Tmp3,_Tmp4,({
void*_Tmp5=Cyc_AssnDef_proj(a,(unsigned)i,tp);struct Cyc_List_List*_Tmp6=fields->tl;Cyc_AssnDef_apply_aggr_update(_Tmp5,_Tmp6,v);}));});
return res;}}}}
# 618
static void*Cyc_AssnDef_proj_fields(void*v,struct Cyc_List_List*fs){
for(1;fs!=0;fs=fs->tl){
struct _tuple15*_Tmp0=(struct _tuple15*)fs->hd;void*_Tmp1;int _Tmp2;_Tmp2=_Tmp0->f0;_Tmp1=_Tmp0->f1;{int i=_Tmp2;void*tpopt=_Tmp1;
if(tpopt!=0 && Cyc_Tcutil_is_pointer_type(tpopt))
v=({void*_Tmp3=v;unsigned _Tmp4=(unsigned)i;Cyc_AssnDef_proj(_Tmp3,_Tmp4,Cyc_Tcutil_pointer_elt_type(tpopt));});else{
# 624
v=Cyc_AssnDef_proj(v,(unsigned)i,0);}}}
# 626
return v;}
# 629
void*Cyc_AssnDef_select(void*t1,void*t2,void*tp){
LOOP: {
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==7){_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=_Tmp0.f1;{void*m=_Tmp4;void*a1=_Tmp3;void*v=_Tmp2;void*a2=_Tmp1;
# 633
if(a1==a2)return v;
if(Cyc_AssnDef_address_disjoint(a1,a2)){
t1=m;
goto LOOP;}{
# 642
struct _tuple12 _Tmp5=Cyc_AssnDef_split_addr(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{void*root=_Tmp7;struct Cyc_List_List*fields=_Tmp6;
if(a1==root)
return Cyc_AssnDef_proj_fields(v,fields);
goto _LL4;}}}}else{_LL4: {
# 647
struct Cyc_AssnDef_Select_AssnDef_Term_struct t=({struct Cyc_AssnDef_Select_AssnDef_Term_struct _Tmp5;_Tmp5.tag=6,_Tmp5.f1=t1,_Tmp5.f2=t2,_Tmp5.f3=tp;_Tmp5;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}};}}
# 659 "assndef.cyc"
void*Cyc_AssnDef_proj(void*t1,unsigned i,void*tp){
LOOP: {
void*_Tmp0;void*_Tmp1;void*_Tmp2;unsigned _Tmp3;int _Tmp4;switch(*((int*)t1)){case 8: _Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f1;_Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f3;{int is_union=_Tmp4;unsigned tag=_Tmp3;struct Cyc_List_List*ts=_Tmp2;
# 663
int n=Cyc_List_length(ts);
if(!is_union && i < (unsigned)n)
return Cyc_List_nth(ts,(int)i);else{
if(tag==i && n >= 1)
return(void*)_check_null(ts)->hd;else{
goto _LL0;}}}case 10: _Tmp2=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f1;_Tmp3=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f3;{void*t=_Tmp2;unsigned j=_Tmp3;void*t2=_Tmp1;
# 670
if(i==j)return t2;{
# 673
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt==0)goto NOREDUCE;
{void*_Tmp5=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _Tmp6;switch(*((int*)_Tmp5)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)==24){_Tmp6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp5)->f1)->f1;{union Cyc_Absyn_AggrInfo ai=_Tmp6;
# 677
{void*_Tmp7;if(ai.KnownAggr.tag==2){_Tmp7=*ai.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp7;
# 679
if((int)ad->kind==1)goto NOREDUCE;goto _LL10;}}else{if(ai.UnknownAggr.val.f0==Cyc_Absyn_UnionA)
goto NOREDUCE;else{
goto _LL10;}}_LL10:;}
# 683
goto _LL9;}}else{goto _LLE;}case 6: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp5)->f1==Cyc_Absyn_UnionA)
goto NOREDUCE;else{goto _LLE;}default: _LLE:
 goto _LL9;}_LL9:;}
# 687
t1=t;
goto LOOP;}}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t1)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t1)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t1)->f3;{void*m=_Tmp2;void*a=_Tmp1;void*tp2=_Tmp0;
void*_Tmp5=m;void*_Tmp6=Cyc_AssnDef_offsetf(a,i,tp2);return Cyc_AssnDef_select(_Tmp5,_Tmp6,tp);}default:
 goto _LL0;}_LL0:;}
# 692
NOREDUCE: {
struct Cyc_AssnDef_Proj_AssnDef_Term_struct t=({struct Cyc_AssnDef_Proj_AssnDef_Term_struct _Tmp0;_Tmp0.tag=9,_Tmp0.f1=t1,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 697
void*Cyc_AssnDef_aggr(int is_union,unsigned tag,struct Cyc_List_List*ts,void*tp){
# 700
struct Cyc_AssnDef_Aggr_AssnDef_Term_struct t=({struct Cyc_AssnDef_Aggr_AssnDef_Term_struct _Tmp0;_Tmp0.tag=8,_Tmp0.f1=is_union,_Tmp0.f2=tag,_Tmp0.f3=ts,_Tmp0.f4=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 703
void*Cyc_AssnDef_struct_aggr(struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(0,-1,ts,tp);}
# 706
void*Cyc_AssnDef_datatype_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(0,tag,ts,tp);}
# 709
void*Cyc_AssnDef_union_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(1,tag,ts,tp);}
# 712
void*Cyc_AssnDef_tagof_tm(void*t1){
{void*_Tmp0;unsigned _Tmp1;switch(*((int*)t1)){case 8: _Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;{unsigned tag=_Tmp1;
return Cyc_AssnDef_uint(tag);}case 10: _Tmp0=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f1;_Tmp1=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t1)->f2;{void*t=_Tmp0;unsigned j=_Tmp1;
# 717
void*topt=Cyc_AssnDef_get_term_type(t1);
if(topt==0)goto _LL0;
{void*_Tmp2=Cyc_Absyn_compress(topt);union Cyc_Absyn_AggrInfo _Tmp3;switch(*((int*)_Tmp2)){case 0: if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)==24){_Tmp3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp2)->f1)->f1;{union Cyc_Absyn_AggrInfo ai=_Tmp3;
# 721
{void*_Tmp4;if(ai.KnownAggr.tag==2){_Tmp4=*ai.KnownAggr.val;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 723
if((int)ad->kind==1)return Cyc_AssnDef_uint(j);
goto _LLE;}}else{if(ai.UnknownAggr.val.f0==Cyc_Absyn_UnionA)
return Cyc_AssnDef_uint(j);else{
goto _LLE;}}_LLE:;}
# 728
goto _LL7;}}else{goto _LLC;}case 6: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_Tmp2)->f1==Cyc_Absyn_UnionA)
return Cyc_AssnDef_uint(j);else{goto _LLC;}default: _LLC:
 goto _LL7;}_LL7:;}
# 732
goto _LL0;}default:
 goto _LL0;}_LL0:;}{
# 735
struct Cyc_AssnDef_Tagof_AssnDef_Term_struct t=({struct Cyc_AssnDef_Tagof_AssnDef_Term_struct _Tmp0;_Tmp0.tag=15,_Tmp0.f1=t1;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 739
void*Cyc_AssnDef_cast(void*tp,void*tm){
struct Cyc_AssnDef_Cast_AssnDef_Term_struct t=({struct Cyc_AssnDef_Cast_AssnDef_Term_struct _Tmp0;_Tmp0.tag=5,_Tmp0.f1=tp,_Tmp0.f2=tm;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 744
void*Cyc_AssnDef_alloc(struct Cyc_Absyn_Exp*e,void*nelts,void*invok,void*tp){
static int counter=0;
struct Cyc_AssnDef_Alloc_AssnDef_Term_struct t=({struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _Tmp0;_Tmp0.tag=12,_Tmp0.f1=e,_Tmp0.f2=counter,_Tmp0.f3=nelts,_Tmp0.f4=invok,_Tmp0.f5=tp;_Tmp0;});
++ counter;
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 751
void*Cyc_AssnDef_offsetf(void*tm,unsigned i,void*tp){
struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct t=({struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct _Tmp0;_Tmp0.tag=13,_Tmp0.f1=tm,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 756
void*Cyc_AssnDef_offseti(void*tm,void*i,void*tp){
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=tm,_Tmp1.f1=i;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;if(*((int*)_Tmp0.f1)==0){if(((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1==0)
return tm;else{if(*((int*)_Tmp0.f0)==14)goto _LL3;else{goto _LL5;}}}else{if(*((int*)_Tmp0.f0)==14){_LL3: _Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f3;{void*t=_Tmp3;void*j=_Tmp2;void*tp2=_Tmp1;
# 760
if((tp!=0 && tp2!=0)&& Cyc_Unify_unify(tp,tp2)){
void*_Tmp4=t;void*_Tmp5=Cyc_AssnDef_plus(i,j,Cyc_Absyn_sint_type);return Cyc_AssnDef_offseti(_Tmp4,_Tmp5,tp);}
goto _LL6;}}else{_LL5: _LL6: {
# 764
struct Cyc_AssnDef_Offseti_AssnDef_Term_struct t=({struct Cyc_AssnDef_Offseti_AssnDef_Term_struct _Tmp4;_Tmp4.tag=14,_Tmp4.f1=tm,_Tmp4.f2=i,_Tmp4.f3=tp;_Tmp4;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}};}
# 768
void*Cyc_AssnDef_fresh_var(void*tp){
static int counter=0;
void*t=Cyc_AssnDef_logicvar(0,counter,tp);
++ counter;
return t;}
# 775
void*Cyc_AssnDef_plus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(0U,t1,t2,tp);}
# 778
void*Cyc_AssnDef_minus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(2U,t1,t2,tp);}
# 782
void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop p,void*t,void*tp){
# 784
if((int)p==0)return t;
# 786
if((int)p==2){void*_Tmp0=t;void*_Tmp1=Cyc_AssnDef_uint(-1);return Cyc_AssnDef_binop(1U,_Tmp0,_Tmp1,tp);}{
struct Cyc_AssnDef_Unop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Unop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=3,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& ptr);}}
# 791
void*Cyc_AssnDef_numelts_term(void*t){
return Cyc_AssnDef_unop(18U,t,Cyc_Absyn_uint_type);}static char _TmpG3[5U]="#Mem";
# 797
static struct _fat_ptr Cyc_AssnDef_memory_var={_TmpG3,_TmpG3,_TmpG3 + 5U};
static struct _tuple0 Cyc_AssnDef_memvar={.f0={.Loc_n={4,0}},.f1=& Cyc_AssnDef_memory_var};
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_AssnDef_void_con={0};
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_AssnDef_vd_type={0,(void*)& Cyc_AssnDef_void_con,0};
# 802
struct Cyc_Absyn_Vardecl Cyc_AssnDef_memory_struct={Cyc_Absyn_Public,& Cyc_AssnDef_memvar,0U,{0,0,0,0,0U},(void*)& Cyc_AssnDef_vd_type,0,0,0,0,0};
# 814
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory=& Cyc_AssnDef_memory_struct;static char _TmpG4[5U]="#Exn";
# 816
static struct _fat_ptr Cyc_AssnDef_exception_var={_TmpG4,_TmpG4,_TmpG4 + 5U};
static struct _tuple0 Cyc_AssnDef_exnvar={.f0={.Loc_n={4,0}},.f1=& Cyc_AssnDef_exception_var};
# 819
struct Cyc_Absyn_Vardecl Cyc_AssnDef_exception_struct={Cyc_Absyn_Public,& Cyc_AssnDef_exnvar,0U,{0,0,0,0,0U},(void*)& Cyc_AssnDef_vd_type,0,0,0,0,0};
# 832
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_exception_vardecl (void){
struct Cyc_Absyn_Vardecl*vd=& Cyc_AssnDef_exception_struct;
({void*_Tmp0=Cyc_Absyn_exn_type();vd->type=_Tmp0;});
return vd;}struct _tuple16{struct Cyc_List_List*f0;int f1;};
# 849
struct _tuple16 Cyc_AssnDef_get_coefficient_term_list(void*t){
struct Cyc_List_List*coefficient_term_list=0;
int res=0;
LOOP: {
void*_Tmp0;void*_Tmp1;unsigned _Tmp2;switch(*((int*)t)){case 0: _Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp2;
res=(int)i;goto _LL0;}case 4: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned i=_Tmp2;
# 856
res=(int)i;
t=t1;
goto LOOP;}}else{_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 860
{unsigned _Tmp3;void*_Tmp4;if(*((int*)t2)==4)switch((int)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f1){case Cyc_Absyn_Times: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3)==0){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3)->f1;{void*t21=_Tmp4;unsigned c2=_Tmp3;
# 862
coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_cycalloc(sizeof(struct _tuple15));_Tmp7->f0=(int)c2,_Tmp7->f1=t21;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL9;}}else{goto _LLE;}case Cyc_Absyn_Plus:
# 865
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("Plus in the right branch, not a normal form",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
goto _LL9;default: goto _LLE;}else{_LLE:
# 868
 coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_cycalloc(sizeof(struct _tuple15));_Tmp7->f0=1,_Tmp7->f1=t2;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL9;}_LL9:;}
# 871
t=t1;
goto LOOP;}}}else{goto _LL7;}default: _LL7:
# 874
{unsigned _Tmp3;void*_Tmp4;if(*((int*)t)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Times){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t21=_Tmp4;unsigned c2=_Tmp3;
# 876
coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_cycalloc(sizeof(struct _tuple15));_Tmp7->f0=(int)c2,_Tmp7->f1=t21;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL10;}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13:
# 879
 coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_cycalloc(sizeof(struct _tuple15));_Tmp7->f0=1,_Tmp7->f1=t;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL10;}_LL10:;}
# 882
goto _LL0;}_LL0:;}{
# 884
struct _tuple16 _Tmp0;_Tmp0.f0=coefficient_term_list,_Tmp0.f1=res;return _Tmp0;}}
# 894 "assndef.cyc"
static struct Cyc_List_List*Cyc_AssnDef_imp_merge_coefficient_term_list(int(*less_eq)(void*,void*),struct Cyc_List_List*termlist1,struct Cyc_List_List*termlist2){
# 898
struct Cyc_List_List*res_tail=0;
struct Cyc_List_List*res=0;
if(termlist1==0)return termlist2;
if(termlist2==0)return termlist1;{
int cmp_result=less_eq(((struct _tuple15*)termlist1->hd)->f1,((struct _tuple15*)termlist2->hd)->f1);
while(termlist1!=0 && termlist2!=0){
{int cmp_result=less_eq(((struct _tuple15*)termlist1->hd)->f1,((struct _tuple15*)termlist2->hd)->f1);
if(cmp_result < 0){
if(res_tail==0){
res_tail=termlist1;
termlist1=termlist1->tl;
res=res_tail;}else{
# 911
res_tail->tl=termlist1;
res_tail=termlist1;
termlist1=termlist1->tl;}}else{
# 915
if(cmp_result > 0){
if(res_tail==0){
res_tail=termlist2;
termlist2=termlist2->tl;
res=res_tail;}else{
# 921
res_tail->tl=termlist2;
res_tail=termlist2;
termlist2=termlist2->tl;}}else{
# 927
int i=((struct _tuple15*)termlist1->hd)->f0 + ((struct _tuple15*)termlist2->hd)->f0;
if(i!=0){
((struct _tuple15*)termlist1->hd)->f0=i;
if(res_tail==0){
res_tail=termlist1;
res=res_tail;}else{
# 934
res_tail->tl=termlist1;
res_tail=termlist1;}}
# 938
termlist1=termlist1->tl;
termlist2=termlist2->tl;}}}
# 904
1U;}
# 942
if(res_tail==0){
res_tail=termlist1==0?termlist2: termlist1;
res=res_tail;}else{
# 946
res_tail->tl=termlist1==0?termlist2: termlist1;}
# 948
return res;}}
# 954
inline static unsigned Cyc_AssnDef_id_of_primop(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 return 0U;case Cyc_Absyn_Times:
 return 1U;case Cyc_Absyn_Bitand:
 return 4294967295U;case Cyc_Absyn_Bitor:
 return 0U;case Cyc_Absyn_Bitxor:
 return 0U;default:
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_Absynpp_prim2string(p);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("no id for primop %s",sizeof(char),20U),_tag_fat(_Tmp1,sizeof(void*),1));});};}
# 968
static void*Cyc_AssnDef_termlist2term(struct Cyc_List_List*termlist,int newc,enum Cyc_Absyn_Primop p,void*tp){
unsigned id=Cyc_AssnDef_id_of_primop(p);
if(termlist==0)return Cyc_AssnDef_uint((unsigned)newc);{
void*t=(void*)termlist->hd;
termlist=termlist->tl;
while(termlist!=0){
{struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=4,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=(void*)termlist->hd,_Tmp0.f4=tp;_Tmp0;});
t=Cyc_AssnDef_hash_cons_term((void*)& newterm);
termlist=termlist->tl;}
# 974
1U;}
# 978
if(id==(unsigned)newc)return t;else{
# 992 "assndef.cyc"
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=4,_Tmp0.f1=p,_Tmp0.f2=t,({void*_Tmp1=Cyc_AssnDef_uint((unsigned)newc);_Tmp0.f3=_Tmp1;}),_Tmp0.f4=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& newterm);}}}
# 999
static void*Cyc_AssnDef_compute_factor(void*tp,struct _tuple15*coefficient_term){
void*_Tmp0;int _Tmp1;_Tmp1=coefficient_term->f0;_Tmp0=coefficient_term->f1;{int c=_Tmp1;void*t=_Tmp0;
if(c==1)return t;else{
# 1003
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newfactor=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=1U,_Tmp2.f2=t,({void*_Tmp3=Cyc_AssnDef_uint((unsigned)c);_Tmp2.f3=_Tmp3;}),_Tmp2.f4=tp;_Tmp2;});
return Cyc_AssnDef_hash_cons_term((void*)& newfactor);}}}
# 1008
static int Cyc_AssnDef_inv_termcmp(void*t1,void*t2){
return - Cyc_AssnDef_termcmp(t1,t2);}
# 1019 "assndef.cyc"
static void*Cyc_AssnDef_normalize_plus(void*t1,void*t2,void*tp){
struct _tuple16 _Tmp0=Cyc_AssnDef_get_coefficient_term_list(t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*coefficient_term_list1=_Tmp2;int i1=_Tmp1;
struct _tuple16 _Tmp3=Cyc_AssnDef_get_coefficient_term_list(t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*coefficient_term_list2=_Tmp5;int i2=_Tmp4;
struct Cyc_List_List*coefficient_term_list=
Cyc_AssnDef_imp_merge_coefficient_term_list(Cyc_AssnDef_inv_termcmp,coefficient_term_list1,coefficient_term_list2);
# 1027
struct Cyc_List_List*term_list=({struct Cyc_List_List*(*_Tmp6)(void*(*)(void*,struct _tuple15*),void*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(void*,struct _tuple15*),void*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp6;})(Cyc_AssnDef_compute_factor,tp,coefficient_term_list);
void*res=Cyc_AssnDef_termlist2term(term_list,i1 + i2,0U,tp);
return res;}}}
# 1037
static struct _tuple16 Cyc_AssnDef_term2termlist(enum Cyc_Absyn_Primop p,void*t){
struct Cyc_List_List*termlist=0;
int res=(int)Cyc_AssnDef_id_of_primop(p);
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;unsigned _Tmp3;switch(*((int*)t)){case 0: _Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp3;
res=(int)i;goto _LL0;}case 4: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;unsigned i=_Tmp3;
# 1044
res=(int)i;
t=t1;
goto LOOP;}else{goto _LL5;}}else{_LL5: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;void*t2=_Tmp0;
# 1048
termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t2,_Tmp4->tl=termlist;_Tmp4;});
t=t1;
goto LOOP;}else{goto _LL7;}}default: _LL7:
# 1052
 termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t,_Tmp4->tl=termlist;_Tmp4;});
goto _LL0;}_LL0:;}{
# 1055
struct _tuple16 _Tmp0;_Tmp0.f0=termlist,_Tmp0.f1=res;return _Tmp0;}}
# 1059
inline static unsigned Cyc_AssnDef_eval_binop(enum Cyc_Absyn_Primop p,unsigned i,unsigned j){
switch((int)p){case Cyc_Absyn_Plus:
 return i + j;case Cyc_Absyn_Times:
 return i * j;case Cyc_Absyn_Bitand:
 return i & j;case Cyc_Absyn_Bitor:
 return i | j;case Cyc_Absyn_Bitxor:
 return i ^ j;default:
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Error in evaluating binop",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}
# 1071
static void*Cyc_AssnDef_normalize_assoc_commu(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
struct _tuple16 _Tmp0=Cyc_AssnDef_term2termlist(p,t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*term_list1=_Tmp2;int c1=_Tmp1;
struct _tuple16 _Tmp3=Cyc_AssnDef_term2termlist(p,t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*term_list2=_Tmp5;int c2=_Tmp4;
struct Cyc_List_List*term_list=Cyc_List_imp_merge(Cyc_AssnDef_inv_termcmp,term_list1,term_list2);
unsigned c=Cyc_AssnDef_eval_binop(p,(unsigned)c1,(unsigned)c2);
return Cyc_AssnDef_termlist2term(term_list,(int)c,p,tp);}}}
# 1079
inline static int Cyc_AssnDef_is_nonassociative_commutative(enum Cyc_Absyn_Primop p,void*tp){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
# 1083
 if(tp!=0){
void*_Tmp0=Cyc_Absyn_compress(tp);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==2)
return 1;else{goto _LLE;}}else{_LLE:
 return 0;};}
# 1089
return 0;case Cyc_Absyn_Eq:
 goto _LL8;case Cyc_Absyn_Neq: _LL8:
 return 1;default:
 return 0;};}
# 1096
inline static int Cyc_AssnDef_is_associative_commutative(enum Cyc_Absyn_Primop p,void*tp){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
# 1100
 if(tp!=0){
void*_Tmp0=Cyc_Absyn_compress(tp);if(*((int*)_Tmp0)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp0)->f1)==1)
# 1103
return 1;else{goto _LL10;}}else{_LL10:
 return 0;};}
# 1107
return 0;case Cyc_Absyn_Bitand:
 goto _LL8;case Cyc_Absyn_Bitor: _LL8:
 goto _LLA;case Cyc_Absyn_Bitxor: _LLA:
 return 1;default:
 return 0;};}struct _tuple17{enum Cyc_Absyn_Primop f0;void*f1;void*f2;};
# 1116
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
{struct _tuple17 _Tmp0=({struct _tuple17 _Tmp1;_Tmp1.f0=p,_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;unsigned _Tmp3;unsigned _Tmp4;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned i=_Tmp4;unsigned j=_Tmp3;
# 1120
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_AssnDef_uint(i + j);case Cyc_Absyn_Minus:
 return Cyc_AssnDef_uint(i - j);case Cyc_Absyn_Bitand:
 return Cyc_AssnDef_uint(i & j);case Cyc_Absyn_Bitor:
 return Cyc_AssnDef_uint(i | j);case Cyc_Absyn_Bitxor:
 return Cyc_AssnDef_uint(i ^ j);case Cyc_Absyn_Bitlshift:
 return Cyc_AssnDef_uint(i << j);case Cyc_Absyn_Bitlrshift:
 return Cyc_AssnDef_uint(i >> j);default:
 goto _LL7;}_LL7:;
# 1130
goto _LL0;}}else{if(_Tmp0.f0==Cyc_Absyn_Minus)goto _LL3;else{goto _LL5;}}}else{if(_Tmp0.f0==Cyc_Absyn_Minus){_LL3: _Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp2;void*t2=_Tmp1;
# 1132
if(tp!=0){if(*((int*)tp)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)tp)->f1)==1){
# 1135
if(Cyc_AssnDef_termcmp(t1,t2)==0)return Cyc_AssnDef_uint(0U);
# 1137
t2=({void*_Tmp5=t2;void*_Tmp6=Cyc_AssnDef_uint(-1);Cyc_AssnDef_binop(1U,_Tmp5,_Tmp6,tp);});
return Cyc_AssnDef_binop(0U,t1,t2,tp);}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B:
 goto _LL18;}_LL18:;
# 1141
goto _LL0;}}else{_LL5:
# 1146
 if(Cyc_AssnDef_is_associative_commutative(p,tp)){
# 1151
if((int)p==0)return Cyc_AssnDef_normalize_plus(t1,t2,tp);else{
return Cyc_AssnDef_normalize_assoc_commu(p,t1,t2,tp);}}
# 1154
if(Cyc_AssnDef_is_nonassociative_commutative(p,tp)){
if(Cyc_AssnDef_termcmp(t1,t2)> 0){
void*tmpt=t1;
t1=t2;
t2=tmpt;}}else{
# 1160
if((int)p==0){
struct _tuple14 _Tmp5=({struct _tuple14 _Tmp6;_Tmp6.f0=t1,_Tmp6.f1=t2;_Tmp6;});unsigned _Tmp6;void*_Tmp7;unsigned _Tmp8;void*_Tmp9;if(*((int*)_Tmp5.f0)==4){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f3)==0){if(*((int*)_Tmp5.f1)==0){_Tmp9=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f2;_Tmp8=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f3)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp5.f0)->f4;_Tmp6=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp5.f1)->f1;{void*t1=_Tmp9;unsigned i=_Tmp8;void*tp2=_Tmp7;unsigned j=_Tmp6;
# 1163
if((tp!=0 && tp2!=0)&& Cyc_Unify_unify(tp,tp2)){
void*_TmpA=t1;void*_TmpB=Cyc_AssnDef_uint(i + j);return Cyc_AssnDef_plus(_TmpA,_TmpB,tp);}
goto _LL1D;}}else{goto _LL20;}}else{goto _LL20;}}else{goto _LL20;}}else{_LL20:
 goto _LL1D;}_LL1D:;}}
# 1169
goto _LL0;}}_LL0:;}{
# 1171
struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=p,_Tmp1.f2=t1,_Tmp1.f3=t2,_Tmp1.f4=tp;_Tmp1;});struct Cyc_AssnDef_Binop_AssnDef_Term_struct p=_Tmp0;
return Cyc_AssnDef_hash_cons_term((void*)& p);}}
# 1175
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
# 1197
static void*Cyc_AssnDef_subst_term(struct Cyc_Dict_Dict dict,void*t,struct Cyc_Hashtable_Table*term_h){
# 1199
void**t1=Cyc_Hashtable_lookup_opt(term_h,t);
if(t1!=0)
# 1205
return*t1;{
# 1207
void*res;
{unsigned _Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
# 1211
 res=t;goto _LL0;case 11: _Tmp6=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp6;void*t1=_Tmp5;void*tp=_Tmp4;
# 1213
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
struct Cyc_AssnDef_Addr_AssnDef_Term_struct addr_term=({struct Cyc_AssnDef_Addr_AssnDef_Term_struct _Tmp7;_Tmp7.tag=11,_Tmp7.f1=vd,_Tmp7.f2=s1,_Tmp7.f3=tp;_Tmp7;});
res=Cyc_AssnDef_hash_cons_term((void*)& addr_term);
goto _LL0;}case 12: _Tmp6=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;_Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f5;{struct Cyc_Absyn_Exp*e1=_Tmp6;int i=_Tmp3;void*t1=_Tmp5;void*t2=_Tmp4;void*tp=_Tmp2;
# 1218
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
struct Cyc_AssnDef_Alloc_AssnDef_Term_struct alloc_term=({struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _Tmp7;_Tmp7.tag=12,_Tmp7.f1=e1,_Tmp7.f2=i,_Tmp7.f3=s1,_Tmp7.f4=s2,_Tmp7.f5=tp;_Tmp7;});
res=Cyc_AssnDef_hash_cons_term((void*)& alloc_term);
goto _LL0;}case 6: _Tmp6=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 1224
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_select(s1,s2,tp);
goto _LL0;}case 7: _Tmp6=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*t3=_Tmp4;
# 1229
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
void*s3=Cyc_AssnDef_subst_term(dict,t3,term_h);
res=Cyc_AssnDef_update(s1,s2,s3);
goto _LL0;}case 2:  {
# 1235
void**t1=Cyc_Dict_lookup_opt(dict,t);
if(t1!=0)res=*t1;else{
res=t;}
goto _LL0;}case 3: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp6;void*tp=_Tmp5;
# 1240
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_unop(p,s1,tp);
goto _LL0;}case 4: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 1244
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_binop(p,s1,s2,tp);
goto _LL0;}case 5: _Tmp6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp6;void*t1=_Tmp5;
# 1249
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_cast(tp,s1);
goto _LL0;}case 8: _Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp6=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int b=_Tmp3;unsigned tag=_Tmp0;struct Cyc_List_List*tlist=_Tmp6;void*tp=_Tmp5;
# 1253
struct Cyc_List_List*slist=0;
for(1;tlist!=0;tlist=tlist->tl){
slist=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp8=Cyc_AssnDef_subst_term(dict,(void*)tlist->hd,term_h);_Tmp7->hd=_Tmp8;}),_Tmp7->tl=slist;_Tmp7;});}
# 1257
Cyc_List_imp_rev(slist);
res=Cyc_AssnDef_aggr(b,tag,slist,tp);
goto _LL0;}case 9: _Tmp6=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*tp=_Tmp5;
# 1261
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_proj(s1,i,tp);
goto _LL0;}case 10: _Tmp6=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_AggrUpdate_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*t2=_Tmp5;
# 1265
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_aggr_update(s1,i,s2);
goto _LL0;}case 13: _Tmp6=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;unsigned i=_Tmp0;void*tp=_Tmp5;
# 1270
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_offsetf(s1,i,tp);
goto _LL0;}case 14: _Tmp6=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp6;void*t2=_Tmp5;void*tp=_Tmp4;
# 1274
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_offseti(s1,s2,tp);
goto _LL0;}default: _Tmp6=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp6;
# 1279
res=Cyc_AssnDef_tagof_tm(Cyc_AssnDef_subst_term(dict,t1,term_h));
goto _LL0;}}_LL0:;}
# 1282
Cyc_Hashtable_insert(term_h,t,res);
return res;}}
# 1286
static int Cyc_AssnDef_hash_ptr(void*s){return(int)s;}
# 1289
void*Cyc_AssnDef_subst_t(struct Cyc_Dict_Dict dict,void*t){
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
return Cyc_AssnDef_subst_term(dict,t,term_h);}struct _tuple18{struct _fat_ptr f0;unsigned f1;};
# 1296
static struct _tuple18 Cyc_AssnDef_a2dag(void*a,int*ctr,struct Cyc_Hashtable_Table*t,struct Cyc_List_List**decls){
# 1300
struct _tuple18**resopt=({struct _tuple18**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct _tuple18**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(t,a);
if(resopt!=0)
return*(*resopt);{
# 1304
int c=*ctr;
*ctr=c + 1;{
struct _fat_ptr name=({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)c;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("A%d",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});
struct _fat_ptr s;
unsigned size=1U;
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 s=_tag_fat("true",sizeof(char),5U);goto _LL0;case 1:
 s=_tag_fat("false",sizeof(char),6U);goto _LL0;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1313
struct _fat_ptr ps;
switch((int)p){case Cyc_AssnDef_Eq:
 ps=_tag_fat("==",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LLB;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LLB;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("assn2string primop",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));}_LLB:;
# 1323
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)ps;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_aprintf(_tag_fat("%s%s%s",sizeof(char),7U),_tag_fat(_Tmp6,sizeof(void*),3));});
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1326
struct _tuple18 _Tmp3=Cyc_AssnDef_a2dag(a1,ctr,t,decls);unsigned _Tmp4;struct _fat_ptr _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct _fat_ptr s1=_Tmp5;unsigned n1=_Tmp4;
struct _tuple18 _Tmp6=Cyc_AssnDef_a2dag(a2,ctr,t,decls);unsigned _Tmp7;struct _fat_ptr _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{struct _fat_ptr s2=_Tmp8;unsigned n2=_Tmp7;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=(struct _fat_ptr)s1;_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=(struct _fat_ptr)s2;_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_TmpB,sizeof(void*),2));});
size +=n1 + n2;
goto _LL0;}}}default: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1332
struct _tuple18 _Tmp3=Cyc_AssnDef_a2dag(a1,ctr,t,decls);unsigned _Tmp4;struct _fat_ptr _Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct _fat_ptr s1=_Tmp5;unsigned n1=_Tmp4;
struct _tuple18 _Tmp6=Cyc_AssnDef_a2dag(a2,ctr,t,decls);unsigned _Tmp7;struct _fat_ptr _Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{struct _fat_ptr s2=_Tmp8;unsigned n2=_Tmp7;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=(struct _fat_ptr)s1;_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=(struct _fat_ptr)s2;_TmpB;});void*_TmpB[2];_TmpB[0]=& _Tmp9,_TmpB[1]=& _TmpA;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_TmpB,sizeof(void*),2));});
size +=n1 + n2;
goto _LL0;}}}}_LL0:;}
# 1338
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp2=({struct _fat_ptr*_Tmp3=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp4=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)name;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)s;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(_tag_fat("%s = %s\n",sizeof(char),9U),_tag_fat(_Tmp7,sizeof(void*),2));});*_Tmp3=_Tmp4;});_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*decls;_Tmp1;});*decls=_Tmp0;});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct _tuple18*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct _tuple18*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple18*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=t;void*_Tmp2=a;_Tmp0(_Tmp1,_Tmp2,({struct _tuple18*_Tmp3=_cycalloc(sizeof(struct _tuple18));_Tmp3->f0=name,_Tmp3->f1=size;_Tmp3;}));});{
struct _tuple18 _Tmp0;_Tmp0.f0=name,_Tmp0.f1=size;return _Tmp0;}}}}
# 1343
int Cyc_AssnDef_assnhash(void*);
# 1345
struct _fat_ptr Cyc_AssnDef_assn2dag(void*a){
struct Cyc_List_List*decls=0;
struct Cyc_Hashtable_Table*t=
Cyc_Hashtable_create(221,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
int ctr=0;
struct _tuple18 _Tmp0=Cyc_AssnDef_a2dag(a,& ctr,t,& decls);unsigned _Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct _fat_ptr s=_Tmp2;unsigned n=_Tmp1;
decls=Cyc_List_imp_rev(({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp4=({struct _fat_ptr*_Tmp5=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp6=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=(struct _fat_ptr)s;_Tmp8;});struct Cyc_Int_pa_PrintArg_struct _Tmp8=({struct Cyc_Int_pa_PrintArg_struct _Tmp9;_Tmp9.tag=1,_Tmp9.f1=(unsigned long)((int)n);_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_aprintf(_tag_fat("in %s (%d nodes as tree)\n",sizeof(char),26U),_tag_fat(_Tmp9,sizeof(void*),2));});*_Tmp5=_Tmp6;});_Tmp5;});_Tmp3->hd=_Tmp4;}),_Tmp3->tl=decls;_Tmp3;}));
return Cyc_strconcat_l(decls);}}
# 1358
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int inprec){
int myprec=10;
struct _fat_ptr s;
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 s=_tag_fat("true",sizeof(char),5U);goto _LL0;case 1:
 s=_tag_fat("false",sizeof(char),6U);goto _LL0;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1365
struct _fat_ptr ps;
switch((int)p){case Cyc_AssnDef_Eq:
 ps=_tag_fat("==",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LLB;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LLB;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("assn2string primop",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));}_LLB:;
# 1375
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)ps;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_aprintf(_tag_fat("%s%s%s",sizeof(char),7U),_tag_fat(_Tmp6,sizeof(void*),3));});
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1378
myprec=5;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_a2string(a1,myprec);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_AssnDef_a2string(a2,myprec);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL0;}default: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1382
myprec=10;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_a2string(a1,10);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_AssnDef_a2string(a2,10);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL0;}}_LL0:;}
# 1386
if(myprec < inprec)
s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("(%s)",sizeof(char),5U),_tag_fat(_Tmp1,sizeof(void*),1));});
return s;}
# 1392
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
return Cyc_AssnDef_a2string(a,10);}
# 1404 "assndef.cyc"
int Cyc_AssnDef_assnhash(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 1:
 return 0;case 0:
 return 1;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1409
int _Tmp3=({int _Tmp4=Cyc_AssnDef_termhash(t1);_Tmp4 + (int)p;});return _Tmp3 + Cyc_AssnDef_termhash(t2);}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1411
return(int)(((unsigned)a1 + (unsigned)a2 >> 4U)+ 3U);}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1413
return(int)(((unsigned)a2 + (unsigned)a1 >> 4U)+ 5U);}};}
# 1419
int Cyc_AssnDef_assncmp(void*_a1,void*_a2){
void*a1=_a1;
void*a2=_a2;
LOOP:
 if(a1==a2)return 0;{
int c=(int)(*((const unsigned*)a2)- *((const unsigned*)a1));
if(c!=0)return c;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});enum Cyc_AssnDef_Primreln _Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0.f0)){case 3: if(*((int*)_Tmp0.f1)==3){_Tmp6=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f2;{void*a11=_Tmp6;void*a12=_Tmp5;void*a21=_Tmp4;void*a22=_Tmp3;
_Tmp6=a11;_Tmp5=a12;_Tmp4=a21;_Tmp3=a22;goto _LL4;}}else{goto _LL7;}case 4: if(*((int*)_Tmp0.f1)==4){_Tmp6=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_LL4: {void*a11=_Tmp6;void*a12=_Tmp5;void*a21=_Tmp4;void*a22=_Tmp3;
# 1429
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c!=0)return c;
a1=a12;
a2=a22;
goto LOOP;}}else{goto _LL7;}case 2: if(*((int*)_Tmp0.f1)==2){_Tmp6=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f3;{void*t11=_Tmp6;enum Cyc_AssnDef_Primreln p1=_Tmp2;void*t12=_Tmp5;void*t21=_Tmp4;enum Cyc_AssnDef_Primreln p2=_Tmp1;void*t22=_Tmp3;
# 1435
int c=(int)p1 - (int)p2;
if(c!=0)return c;
c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_termcmp(t12,t22);}}else{goto _LL7;}default: _LL7:
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("assncmp:  tagof failure",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));};}}}
# 1447
static void*Cyc_AssnDef_copy_assn(void*a){
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
# 1450
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("True assertion should be in the table",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));case 1:
# 1452
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("False assertion should be in the table",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp3->tag=3,_Tmp3->f1=a1,_Tmp3->f2=a2;_Tmp3;});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp3->tag=4,_Tmp3->f1=a1,_Tmp3->f2=a2;_Tmp3;});}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=p,_Tmp3->f3=t2;_Tmp3;});}};}
# 1461
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 1464
static void*Cyc_AssnDef_neg_assn(void*a){
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp3->tag=4,({void*_Tmp4=Cyc_AssnDef_not(a1);_Tmp3->f1=_Tmp4;}),({void*_Tmp4=Cyc_AssnDef_not(a2);_Tmp3->f2=_Tmp4;});_Tmp3;});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp3->tag=3,({void*_Tmp4=Cyc_AssnDef_not(a1);_Tmp3->f1=_Tmp4;}),({void*_Tmp4=Cyc_AssnDef_not(a2);_Tmp3->f2=_Tmp4;});_Tmp3;});}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1471
switch((int)p){case Cyc_AssnDef_Eq:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=1U,_Tmp3->f3=t2;_Tmp3;});case Cyc_AssnDef_Neq:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=0U,_Tmp3->f3=t2;_Tmp3;});case Cyc_AssnDef_SLt:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=3U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_SLte:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=2U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_ULt:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=5U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_ULte:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=4U,_Tmp3->f3=t1;_Tmp3;});default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("undefined prim relation",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));};}};}struct Cyc_AssnDef_AssnHashedInfo{void*assn;void*negation;struct Cyc_Set_Set**widened_set;struct Cyc_Set_Set**free_logicvar_set;};
# 1494
static struct Cyc_Hashtable_Table*Cyc_AssnDef_assn_hash_cons_table;
# 1496
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_assn_hash_cons_table (void){
# 1498
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_assn_hash_cons_table==0){
h=Cyc_Hashtable_create(221,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_assn_hash_cons_table=h;{
struct Cyc_Set_Set*ast=Cyc_Set_empty(Cyc_AssnDef_assncmp);
struct Cyc_Set_Set*tst=Cyc_AssnDef_empty_term_set();
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_true_assn,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp2->assn=(void*)& Cyc_AssnDef_true_assn,_Tmp2->negation=(void*)& Cyc_AssnDef_false_assn,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=ast;_Tmp4;});_Tmp2->widened_set=_Tmp3;}),({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_false_assn,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp2->assn=(void*)& Cyc_AssnDef_false_assn,_Tmp2->negation=(void*)& Cyc_AssnDef_true_assn,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=0;_Tmp4;});_Tmp2->widened_set=_Tmp3;}),({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});}}else{
# 1507
h=Cyc_AssnDef_assn_hash_cons_table;}
# 1509
return h;}
# 1511
static void*Cyc_AssnDef_hash_cons_assn(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**resopt=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*))=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;_Tmp0;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
if(resopt==0){
# 1517
void*a2=Cyc_AssnDef_copy_assn(a);
void*nega2=Cyc_AssnDef_neg_assn(a);
# 1520
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;void*_Tmp2=a2;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp3->assn=a2,_Tmp3->negation=nega2,_Tmp3->widened_set=0,_Tmp3->free_logicvar_set=0;_Tmp3;}));});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;void*_Tmp2=nega2;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp3->assn=nega2,_Tmp3->negation=a2,_Tmp3->widened_set=0,_Tmp3->free_logicvar_set=0;_Tmp3;}));});
# 1524
return a2;}else{
# 1526
return(*resopt)->assn;}}
# 1529
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a2;else{if(*((int*)_Tmp0.f1)==0)
return a1;else{if(*((int*)_Tmp0.f0)==1)
return a1;else{if(*((int*)_Tmp0.f1)==1)
return a2;else{
# 1539
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=3,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}}};}}
# 1544
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1==a2)return a1;{
# 1547
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a1;else{if(*((int*)_Tmp0.f1)==0)
return a2;else{if(*((int*)_Tmp0.f0)==1)
return a2;else{if(*((int*)_Tmp0.f1)==1)
return a1;else{
# 1555
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}}};}}
# 1569 "assndef.cyc"
static void*Cyc_AssnDef_reinsert(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1575
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a1)==0)
a1=Cyc_AssnDef_reinsert(a1);
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a2)==0)
a2=Cyc_AssnDef_reinsert(a2);{
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=a1,_Tmp2.f2=a2;_Tmp2;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1582
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a1)==0)
a1=Cyc_AssnDef_reinsert(a1);
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a2)==0)
a2=Cyc_AssnDef_reinsert(a2);{
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=a1,_Tmp2.f2=a2;_Tmp2;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}default:
# 1589
 return Cyc_AssnDef_hash_cons_assn(a);};}
# 1593
struct Cyc_Set_Set*Cyc_AssnDef_assn_fr_logicvar(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1600
if((*_check_null(ahinfo))->free_logicvar_set!=0)
return*(*ahinfo)->free_logicvar_set;{
struct Cyc_Set_Set*res;
{void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
# 1606
 res=Cyc_AssnDef_empty_term_set();
goto _LL0;case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1609
_Tmp1=a1;_Tmp0=a2;goto _LL8;}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;_LL8: {void*a1=_Tmp1;void*a2=_Tmp0;
# 1611
res=({struct Cyc_Set_Set*_Tmp2=Cyc_AssnDef_assn_fr_logicvar(a1);Cyc_Set_union_two(_Tmp2,Cyc_AssnDef_assn_fr_logicvar(a2));});
goto _LL0;}default: _Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 1614
res=({struct Cyc_Set_Set*_Tmp2=Cyc_AssnDef_term_fr_logicvar(t1);Cyc_Set_union_two(_Tmp2,Cyc_AssnDef_term_fr_logicvar(t2));});
goto _LL0;}}_LL0:;}
# 1617
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=res;_Tmp1;});(*ahinfo)->free_logicvar_set=_Tmp0;});
return res;}}
# 1621
static int Cyc_AssnDef_logicvar_in_assn(void*lvar,void*a){
struct Cyc_Set_Set*lvset=Cyc_AssnDef_assn_fr_logicvar(a);
return Cyc_Set_member(lvset,lvar);}
# 1628
void*Cyc_AssnDef_not(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1635
return(*_check_null(ahinfo))->negation;}
# 1638
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=t1,_Tmp0.f2=p,_Tmp0.f3=t2;_Tmp0;});
return Cyc_AssnDef_hash_cons_assn((void*)& ptr);}
# 1648
void*Cyc_AssnDef_eq(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_true_assn;
if(Cyc_AssnDef_term_neq(t1,t2))return(void*)& Cyc_AssnDef_false_assn;
if(Cyc_AssnDef_termcmp(t1,t2)> 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 1654
return Cyc_AssnDef_prim(t1,0U,t2);}
# 1657
void*Cyc_AssnDef_slt(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_false_assn;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp2;unsigned j=_Tmp1;
# 1661
if((int)i < (int)j)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}}}else{goto _LL3;}}else{_LL3:
 return Cyc_AssnDef_prim(t1,2U,t2);};}}
# 1666
void*Cyc_AssnDef_slte(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_true_assn;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp2;unsigned j=_Tmp1;
# 1670
if((int)i <= (int)j)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}}}else{goto _LL3;}}else{_LL3:
 return Cyc_AssnDef_prim(t1,3U,t2);};}}
# 1675
void*Cyc_AssnDef_ult(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_false_assn;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp2;unsigned j=_Tmp1;
# 1679
if(i < j)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}}}else{goto _LL3;}}else{_LL3:
 return Cyc_AssnDef_prim(t1,4U,t2);};}}
# 1684
void*Cyc_AssnDef_ulte(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_true_assn;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;unsigned _Tmp2;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i=_Tmp2;unsigned j=_Tmp1;
# 1688
if(i <= j)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}}}else{goto _LL3;}}else{_LL3:
 return Cyc_AssnDef_prim(t1,5U,t2);};}}
# 1693
void*Cyc_AssnDef_neq(void*t1,void*t2){
if(t1==t2)return(void*)& Cyc_AssnDef_false_assn;
if(Cyc_AssnDef_term_neq(t1,t2))return(void*)& Cyc_AssnDef_true_assn;{
struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});unsigned _Tmp1;if(*((int*)_Tmp0.f0)==0){_Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;if((unsigned)_Tmp1==0U){unsigned i=_Tmp1;
# 1698
void*_Tmp2=Cyc_AssnDef_zero();return Cyc_AssnDef_ult(_Tmp2,t2);}else{if(*((int*)_Tmp0.f1)==0)goto _LL3;else{goto _LL5;}}}else{if(*((int*)_Tmp0.f1)==0){_LL3: _Tmp1=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;if((unsigned)_Tmp1==0U){unsigned j=_Tmp1;
# 1700
void*_Tmp2=Cyc_AssnDef_zero();return Cyc_AssnDef_ult(_Tmp2,t1);}else{goto _LL5;}}else{_LL5:
# 1702
 if(Cyc_AssnDef_termcmp(t1,t2)> 0)
({void*_Tmp2=t1;void*_Tmp3=t2;t1=_Tmp3;t2=_Tmp2;});
# 1705
return Cyc_AssnDef_prim(t1,1U,t2);}};}}
# 1709
struct Cyc_Set_Set*Cyc_AssnDef_widen_it(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1716
if((*_check_null(ahinfo))->widened_set!=0)return*(*ahinfo)->widened_set;{
struct Cyc_Set_Set*s;
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
# 1721
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("true and false should have widened_set",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1723
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0 || s2==0)
s=0;else{
# 1728
s=Cyc_Set_union_two(s1,s2);}
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1731
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0)
s=s2;else{
if(s2==0)
s=s1;else{
# 1738
s=Cyc_Set_intersect(s1,s2);}}
goto _LL0;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1741
s=Cyc_Set_singleton(Cyc_AssnDef_assncmp,a);
switch((int)p){case Cyc_AssnDef_Eq:  {
# 1747
void*topt1=Cyc_AssnDef_get_term_type(t1);
void*topt2=Cyc_AssnDef_get_term_type(t2);
if(topt1!=0 && Cyc_Tcutil_is_integral_type(topt1)||
 topt2!=0 && Cyc_Tcutil_is_integral_type(topt2)){
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t2,t1));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t2,t1));});}
# 1756
goto _LLB;}case Cyc_AssnDef_SLt:
# 1758
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
goto _LLB;case Cyc_AssnDef_ULt:
# 1761
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
goto _LLB;default:
 goto _LLB;}_LLB:;
# 1765
goto _LL0;}}_LL0:;}
# 1767
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=s;_Tmp1;});(*ahinfo)->widened_set=_Tmp0;});
return s;}}
# 1772
void*Cyc_AssnDef_widen(void*a){
# 1774
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(a);
if(sopt==0)return(void*)& Cyc_AssnDef_false_assn;{
void*res=Cyc_Set_fold(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);
return res;}}
# 1780
static void*Cyc_AssnDef_subst_assn(struct Cyc_Dict_Dict dict,void*a,struct Cyc_Hashtable_Table*assn_h,struct Cyc_Hashtable_Table*term_h){
# 1783
void**a1=Cyc_Hashtable_lookup_opt(assn_h,a);
if(a1!=0)
# 1789
return*a1;{
# 1791
void*res;
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 res=a;goto _LL0;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1796
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_prim(s1,p,s2);
goto _LL0;}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1801
void*b1=Cyc_AssnDef_subst_assn(dict,a1,assn_h,term_h);
void*b2=Cyc_AssnDef_subst_assn(dict,a2,assn_h,term_h);
res=Cyc_AssnDef_and(b1,b2);
goto _LL0;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1806
void*b1=Cyc_AssnDef_subst_assn(dict,a1,assn_h,term_h);
void*b2=Cyc_AssnDef_subst_assn(dict,a2,assn_h,term_h);
res=Cyc_AssnDef_or(b1,b2);
goto _LL0;}}_LL0:;}
# 1811
Cyc_Hashtable_insert(assn_h,a,res);
return res;}}
# 1815
void*Cyc_AssnDef_subst_a(struct Cyc_Dict_Dict dict,void*a){
# 1818
struct Cyc_Hashtable_Table*assn_h=Cyc_Hashtable_create(107,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
return Cyc_AssnDef_subst_assn(dict,a,assn_h,term_h);}
# 1835 "assndef.cyc"
static int Cyc_AssnDef_simple_prv(struct Cyc_Set_Set*ctxt,void*a){
while(1){
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1839
if(Cyc_Set_member(ctxt,a))return 1;
# 1846
switch((int)p){case Cyc_AssnDef_Neq:
# 1848
 return((({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t2,t1));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t2,t1));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));});case Cyc_AssnDef_SLte:
# 1853
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});case Cyc_AssnDef_ULte:
# 1856
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});default:
 return 0;};}case 0:
# 1860
 return 1;case 1:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1863
if(!Cyc_AssnDef_simple_prv(ctxt,a1))
return 0;
# 1869
a=a2;
continue;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1872
if(Cyc_AssnDef_simple_prv(ctxt,a1))return 1;
a=a2;
continue;}};}
# 1837
1U;}}
# 1882
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
if(ctxt==a)return 1;{
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(ctxt);
if(sopt==0)return 1;
return Cyc_AssnDef_simple_prv(sopt,a);}}
# 1891
static struct Cyc_List_List*Cyc_AssnDef_m2string(struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_List_List*stringlist){
struct _fat_ptr res=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_AssnDef_term2string(value);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("%s == %s",sizeof(char),9U),_tag_fat(_Tmp2,sizeof(void*),2));});
struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=res;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=stringlist;return _Tmp0;}
# 1896
struct _fat_ptr Cyc_AssnDef_map2string(struct Cyc_Dict_Dict m){
struct Cyc_List_List*stringlist=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_m2string,m,0);
struct _fat_ptr s;
if(stringlist==0)return Cyc_aprintf(_tag_fat(" ",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));else{
s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)*((struct _fat_ptr*)stringlist->hd);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});}
while(stringlist->tl!=0){
stringlist=stringlist->tl;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)s;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)*((struct _fat_ptr*)stringlist->hd);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp2,sizeof(void*),2));});
# 1902
1U;}
# 1905
return s;}
# 1909
struct _fat_ptr Cyc_AssnDef_assnmap2string(struct Cyc_AssnDef_AssnMap am){
return({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_AssnDef_map2string(am.map);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_AssnDef_assn2string(am.assn);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("{%s}&&\n%s",sizeof(char),10U),_tag_fat(_Tmp2,sizeof(void*),2));});}
# 1913
struct _fat_ptr Cyc_AssnDef_assnmap2dag(struct Cyc_AssnDef_AssnMap am){
return({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_AssnDef_map2string(am.map);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_AssnDef_assn2dag(am.assn);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("{%s}\n%s",sizeof(char),8U),_tag_fat(_Tmp2,sizeof(void*),2));});}
# 1917
static struct Cyc_Dict_Dict Cyc_AssnDef_empty_map (void){
static struct Cyc_Dict_Dict*mt=0;
if(mt==0){
struct Cyc_Dict_Dict d=({struct Cyc_Dict_Dict(*_Tmp0)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))=(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty;_Tmp0;})(({int(*_Tmp0)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)=(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp;_Tmp0;}));
mt=({struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));*_Tmp0=d;_Tmp0;});
return d;}else{
# 1924
return*mt;}}
# 1927
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_false_assnmap (void){
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=(void*)& Cyc_AssnDef_false_assn,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.map=_Tmp1;});return _Tmp0;}
# 1931
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_true_assnmap (void){
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=(void*)& Cyc_AssnDef_true_assn,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.map=_Tmp1;});return _Tmp0;}struct _tuple19{void*f0;struct Cyc_AssnDef_AssnMap f1;};
# 1938
struct _tuple19 Cyc_AssnDef_lookup_var_map(struct Cyc_Absyn_Vardecl*vd,struct Cyc_AssnDef_AssnMap am){
void**t=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,vd);
if(t!=0){struct _tuple19 _Tmp0;_Tmp0.f0=*t,_Tmp0.f1=am;return _Tmp0;}{
void*newt=Cyc_AssnDef_fresh_var(vd->type);
struct Cyc_Dict_Dict newm=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(am.map,vd,newt);
struct _tuple19 _Tmp0;_Tmp0.f0=newt,_Tmp0.f1.assn=am.assn,_Tmp0.f1.map=newm;return _Tmp0;}}
# 1948
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_update_var_map(struct Cyc_Absyn_Vardecl*vd,void*newt,struct Cyc_AssnDef_AssnMap am){
struct Cyc_Dict_Dict newm=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(am.map,vd,newt);
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=am.assn,_Tmp0.map=newm;return _Tmp0;}
# 1954
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_widen_assnmap(struct Cyc_AssnDef_AssnMap am){
struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_widen(am.assn);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;return _Tmp0;}
# 1959
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assn(struct Cyc_AssnDef_AssnMap am,void*a){
struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_or(am.assn,a);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;return _Tmp0;}
# 1964
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_and_assnmap_assn(struct Cyc_AssnDef_AssnMap am,void*a){
struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_and(am.assn,a);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;return _Tmp0;}
# 1968
void*Cyc_AssnDef_merge(struct Cyc_Absyn_Vardecl*vd,void*t1,void*t2){
if(t1==t2)return t1;else{
return Cyc_AssnDef_fresh_var(vd->type);}}struct _tuple20{struct Cyc_Hashtable_Table*f0;struct Cyc_Dict_Dict f1;};
# 1973
static void*Cyc_AssnDef_subst_term_internal(struct _tuple20*env,void*t){
return Cyc_AssnDef_subst_term(env->f1,t,env->f0);}
# 1977
static struct Cyc_AssnDef_AssnMap Cyc_AssnDef_subst_am(struct Cyc_Dict_Dict dict,struct Cyc_AssnDef_AssnMap am){
# 1980
struct Cyc_Hashtable_Table*assn_h=Cyc_Hashtable_create(107,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
void*assn=Cyc_AssnDef_subst_assn(dict,am.assn,assn_h,term_h);
struct Cyc_Dict_Dict map=({struct Cyc_Dict_Dict(*_Tmp0)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict)=({struct Cyc_Dict_Dict(*_Tmp1)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(struct _tuple20*,void*),struct _tuple20*,struct Cyc_Dict_Dict))Cyc_Dict_map_c;_Tmp1;});struct _tuple20*_Tmp1=({struct _tuple20*_Tmp2=_cycalloc(sizeof(struct _tuple20));_Tmp2->f0=term_h,_Tmp2->f1=dict;_Tmp2;});_Tmp0(Cyc_AssnDef_subst_term_internal,_Tmp1,am.map);});
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=assn,_Tmp0.map=map;return _Tmp0;}struct Cyc_AssnDef_Accum{void*a1;void*a2;struct Cyc_Dict_Dict m1;struct Cyc_Dict_Dict m2;struct Cyc_Dict_Dict m;};
# 1994
static int Cyc_AssnDef_logicvar_in_map_internal(void*lvar,struct Cyc_Absyn_Vardecl*dummy,void*value,int in_map){
return in_map || Cyc_AssnDef_logicvar_in_term(lvar,value);}
# 1998
static int Cyc_AssnDef_logicvar_in_map(void*lv,struct Cyc_Dict_Dict m){
return({int(*_Tmp0)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int)=(int(*)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int))Cyc_Dict_fold_c;_Tmp0;})(Cyc_AssnDef_logicvar_in_map_internal,lv,m,0);}
# 2005
static struct Cyc_AssnDef_Accum*Cyc_AssnDef_foldm1(struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_AssnDef_Accum*accum){
void**t2=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(accum->m2,vd);
# 2009
if(t2==0){
if(*((int*)t)==2){
# 2012
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,t);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,t);accum->m=_Tmp0;});
return accum;}
# 2017
goto _LL0;}else{
# 2019
goto _LL0;}_LL0:;{
# 2021
void*v=Cyc_AssnDef_fresh_var(vd->type);
({void*_Tmp0=({void*_Tmp1=accum->a1;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,t));});accum->a1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m1,vd,v);accum->m1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,v);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,v);accum->m=_Tmp0;});
return accum;}}
# 2029
if(t==*t2){
# 2032
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,t);accum->m=_Tmp0;});
return accum;}
# 2037
{struct _tuple14 _Tmp0=({struct _tuple14 _Tmp1;_Tmp1.f0=t,_Tmp1.f1=*t2;_Tmp1;});if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2){
# 2039
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
# 2041
({void*_Tmp1=({void*_Tmp2=Cyc_AssnDef_eq(t,*t2);Cyc_AssnDef_and(_Tmp2,accum->a2);});accum->a2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m2,vd,t);accum->m2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,t);accum->m=_Tmp1;});
return accum;}else{
if(!Cyc_AssnDef_logicvar_in_assn(*t2,accum->a1)&& !Cyc_AssnDef_logicvar_in_map(*t2,accum->m1)){
# 2047
({void*_Tmp1=({void*_Tmp2=Cyc_AssnDef_eq(t,*t2);Cyc_AssnDef_and(_Tmp2,accum->a1);});accum->a1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m1,vd,*t2);accum->m1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,*t2);accum->m=_Tmp1;});}}
# 2051
goto _LL5;}else{
# 2053
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
# 2055
({void*_Tmp1=({void*_Tmp2=Cyc_AssnDef_eq(t,*t2);Cyc_AssnDef_and(_Tmp2,accum->a2);});accum->a2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m2,vd,t);accum->m2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,t);accum->m=_Tmp1;});
return accum;}
# 2060
goto _LL5;}}else{if(*((int*)_Tmp0.f1)==2){
# 2062
if(!Cyc_AssnDef_logicvar_in_assn(*t2,accum->a1)&& !Cyc_AssnDef_logicvar_in_map(*t2,accum->m1)){
# 2064
({void*_Tmp1=({void*_Tmp2=Cyc_AssnDef_eq(t,*t2);Cyc_AssnDef_and(_Tmp2,accum->a1);});accum->a1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m1,vd,*t2);accum->m1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,*t2);accum->m=_Tmp1;});}
# 2068
goto _LL5;}else{
# 2070
goto _LL5;}}_LL5:;}{
# 2072
void*v=Cyc_AssnDef_fresh_var(vd->type);
({void*_Tmp0=({void*_Tmp1=Cyc_AssnDef_eq(v,t);Cyc_AssnDef_and(_Tmp1,accum->a1);});accum->a1=_Tmp0;});
({void*_Tmp0=({void*_Tmp1=Cyc_AssnDef_eq(v,*t2);Cyc_AssnDef_and(_Tmp1,accum->a2);});accum->a2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m1,vd,v);accum->m1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,v);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,v);accum->m=_Tmp0;});
return accum;}}
# 2085
static struct Cyc_AssnDef_Accum*Cyc_AssnDef_foldm2(struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_AssnDef_Accum*accum){
{
# 2091
void*v=Cyc_AssnDef_fresh_var(vd->type);
({void*_Tmp0=({void*_Tmp1=Cyc_AssnDef_eq(v,t);Cyc_AssnDef_and(_Tmp1,accum->a2);});accum->a2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,v);accum->m=_Tmp0;});
return accum;};}
# 2098
struct Cyc_Dict_Dict Cyc_AssnDef_empty_term_dict (void){
static struct Cyc_Dict_Dict*term_dict=0;
if(term_dict==0)
term_dict=({struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=Cyc_Dict_empty(Cyc_Core_ptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
# 2103
return*term_dict;}
# 2111
struct _tuple13 Cyc_AssnDef_unify_var_maps(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
if(am1.assn==(void*)& Cyc_AssnDef_false_assn){struct _tuple13 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am2.map;return _Tmp0;}
if(am2.assn==(void*)& Cyc_AssnDef_false_assn){struct _tuple13 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am1.map;return _Tmp0;}{
struct Cyc_AssnDef_Accum accum=({struct Cyc_AssnDef_Accum _Tmp0;_Tmp0.a1=am1.assn,_Tmp0.a2=am2.assn,_Tmp0.m1=am1.map,_Tmp0.m2=am2.map,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.m=_Tmp1;});_Tmp0;});
({struct Cyc_AssnDef_Accum*(*_Tmp0)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=(struct Cyc_AssnDef_Accum*(*)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_foldm1,am1.map,& accum);
({struct Cyc_AssnDef_Accum*(*_Tmp0)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=({struct Cyc_AssnDef_Accum*(*_Tmp1)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=(struct Cyc_AssnDef_Accum*(*)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*))Cyc_Dict_fold;_Tmp1;});_Tmp0(Cyc_AssnDef_foldm2,Cyc_Dict_difference(am2.map,am1.map),& accum);});{
struct _tuple13 _Tmp0;_Tmp0.f0=accum.a1,_Tmp0.f1=accum.a2,_Tmp0.f2=accum.m;return _Tmp0;}}}
# 2121
static struct Cyc_Dict_Dict*Cyc_AssnDef_collect_subst(struct Cyc_Dict_Dict*map2,struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_Dict_Dict*dict){
if(*((int*)value)==2){
# 2127
void**v=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(*map2,vd);
# 2130
if(Cyc_Dict_lookup_opt(*dict,value)==0){
if(v!=0 &&*v==value){
struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=Cyc_Dict_insert(*dict,value,*v);*_Tmp0=_Tmp1;});return _Tmp0;}{
struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict _Tmp2=*dict;void*_Tmp3=value;Cyc_Dict_insert(_Tmp2,_Tmp3,Cyc_AssnDef_fresh_var(vd->type));});*_Tmp0=_Tmp1;});return _Tmp0;}}
# 2135
goto _LL0;}else{
# 2137
goto _LL0;}_LL0:;
# 2139
return dict;}
# 2149 "assndef.cyc"
struct _tuple13 Cyc_AssnDef_unify_var_maps_subst(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
if(am1.assn==(void*)& Cyc_AssnDef_false_assn){struct _tuple13 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am2.map;return _Tmp0;}
if(am2.assn==(void*)& Cyc_AssnDef_false_assn){struct _tuple13 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am1.map;return _Tmp0;}{
# 2155
struct Cyc_Dict_Dict subst_dict=*({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=({struct Cyc_Dict_Dict*(*_Tmp1)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_Tmp1;});struct Cyc_Dict_Dict*_Tmp1=& am2.map;struct Cyc_Dict_Dict _Tmp2=am1.map;_Tmp0(Cyc_AssnDef_collect_subst,_Tmp1,_Tmp2,({struct Cyc_Dict_Dict*_Tmp3=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp4=Cyc_AssnDef_empty_term_dict();*_Tmp3=_Tmp4;});_Tmp3;}));});
struct Cyc_AssnDef_AssnMap _Tmp0=Cyc_AssnDef_subst_am(subst_dict,am2);struct Cyc_AssnDef_AssnMap am2=_Tmp0;
return Cyc_AssnDef_unify_var_maps(am1,am2);}}
# 2160
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assnmap(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
struct _tuple13 _Tmp0=Cyc_AssnDef_unify_var_maps(am1,am2);struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*a1=_Tmp3;void*a2=_Tmp2;struct Cyc_Dict_Dict map=_Tmp1;
struct Cyc_AssnDef_AssnMap _Tmp4;({void*_Tmp5=Cyc_AssnDef_or(a1,a2);_Tmp4.assn=_Tmp5;}),_Tmp4.map=map;return _Tmp4;}}struct Cyc_AssnDef_Canonical_am_accum{void*assn;struct Cyc_Dict_Dict map;struct Cyc_Set_Set*used_lvars;};
# 2172
static struct Cyc_AssnDef_Canonical_am_accum*Cyc_AssnDef_fold_canonical(struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_AssnDef_Canonical_am_accum*accum){
if(*((int*)value)==2){
# 2175
if(!Cyc_Set_member(accum->used_lvars,value)){
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(accum->used_lvars,value);accum->used_lvars=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->map,vd,value);accum->map=_Tmp0;});
return accum;}
# 2180
goto _LL0;}else{
# 2182
goto _LL0;}_LL0:;{
# 2184
void*v=Cyc_AssnDef_fresh_var(vd->type);
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(accum->used_lvars,v);accum->used_lvars=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->map,vd,v);accum->map=_Tmp0;});
({void*_Tmp0=({void*_Tmp1=accum->assn;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,value));});accum->assn=_Tmp0;});
return accum;}}
# 2195
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_canonical_assnmap(struct Cyc_AssnDef_AssnMap am){
struct Cyc_AssnDef_Canonical_am_accum*accum;accum=_cycalloc(sizeof(struct Cyc_AssnDef_Canonical_am_accum)),accum->assn=am.assn,({struct Cyc_Dict_Dict _Tmp0=Cyc_AssnDef_empty_map();accum->map=_Tmp0;}),({struct Cyc_Set_Set*_Tmp0=Cyc_Set_empty(Cyc_Core_ptrcmp);accum->used_lvars=_Tmp0;});
({struct Cyc_AssnDef_Canonical_am_accum*(*_Tmp0)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*)=(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_fold_canonical,am.map,accum);{
struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=accum->assn,_Tmp0.map=accum->map;return _Tmp0;}}
# 2208 "assndef.cyc"
void*Cyc_AssnDef_merge_assnmaps(struct Cyc_List_List*vds1,struct Cyc_AssnDef_AssnMap a1,struct Cyc_List_List*vds2,struct Cyc_AssnDef_AssnMap a2){
# 2210
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
# 2212
void**v1=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a1.map,Cyc_AssnDef_memory);
void**v2=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a2.map,Cyc_AssnDef_memory);
if(v1!=0 && v2!=0)
dict=Cyc_Dict_insert(dict,*v2,*v1);
# 2217
for(1;vds1!=0 && vds2!=0;(vds1=vds1->tl,vds2=vds2->tl)){
if((struct Cyc_Absyn_Vardecl*)vds1->hd!=0 &&(struct Cyc_Absyn_Vardecl*)vds2->hd!=0){
void**v1=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a1.map,(struct Cyc_Absyn_Vardecl*)vds1->hd);
void**v2=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a2.map,_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd));
if(v1!=0 && v2!=0)
dict=Cyc_Dict_insert(dict,*v2,*v1);}}
# 2232
if(Cyc_Dict_is_empty(dict))return a2.assn;else{
return Cyc_AssnDef_subst_a(dict,a2.assn);}}
# 2244 "assndef.cyc"
extern struct Cyc_AssnDef_AssnMap*Cyc_AssnDef_subst_vardecls(struct Cyc_List_List*vds1,struct Cyc_List_List*vds2,struct Cyc_AssnDef_AssnMap*a){
# 2247
if(a==0)return 0;{
struct Cyc_Dict_Dict newmap=Cyc_AssnDef_empty_map();
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
# 2251
void**v=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a->map,Cyc_AssnDef_memory);
if(v!=0){
void*u=Cyc_AssnDef_fresh_var(0);
newmap=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(newmap,Cyc_AssnDef_memory,u);
dict=Cyc_Dict_insert(dict,*v,u);}
# 2257
for(1;vds1!=0 && vds2!=0;(vds1=vds1->tl,vds2=vds2->tl)){
if((struct Cyc_Absyn_Vardecl*)vds1->hd!=0 &&(struct Cyc_Absyn_Vardecl*)vds2->hd!=0){
void**v=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a->map,(struct Cyc_Absyn_Vardecl*)vds1->hd);
if(v!=0){
void*u=Cyc_AssnDef_fresh_var(_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd)->type);
# 2263
newmap=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(newmap,_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd),u);
# 2265
dict=Cyc_Dict_insert(dict,*v,u);}}}{
# 2270
void*newa=Cyc_AssnDef_subst_a(dict,a->assn);
struct Cyc_AssnDef_AssnMap*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));_Tmp0->assn=newa,_Tmp0->map=newmap;return _Tmp0;}}}
# 2279
void*Cyc_AssnDef_subst_args(struct Cyc_List_List*args,struct Cyc_Absyn_Vardecl*res,struct Cyc_List_List*actual_args,void*actual_res,void*mem,struct Cyc_AssnDef_AssnMap am){
# 2283
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
void**mem_val=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,Cyc_AssnDef_memory);
if(mem_val!=0)
dict=Cyc_Dict_insert(dict,*mem_val,mem);
if(res!=0 && actual_res!=0){
void**res_val=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,res);
if(res_val!=0)
dict=Cyc_Dict_insert(dict,*res_val,actual_res);}
# 2292
for(1;args!=0 && actual_args!=0;(args=args->tl,actual_args=actual_args->tl)){
if((struct Cyc_Absyn_Vardecl*)args->hd!=0 &&(void*)actual_args->hd!=0){
void**arg_val=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,(struct Cyc_Absyn_Vardecl*)args->hd);
if(arg_val!=0)
dict=Cyc_Dict_insert(dict,*arg_val,(void*)actual_args->hd);}}
# 2299
return Cyc_AssnDef_subst_a(dict,am.assn);}
# 2302
void Cyc_AssnDef_reset_hash_cons_table (void){
# 2305
Cyc_AssnDef_term_hash_cons_table=0;
Cyc_AssnDef_assn_hash_cons_table=0;}
