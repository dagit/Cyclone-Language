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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f1;void*f2;};
# 108 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 110
void*Cyc_Core_snd(struct _tuple0*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 251
extern int Cyc_List_forall(int(*)(void*),struct Cyc_List_List*);
# 258
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 134 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 156
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 160
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 165
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;};struct _tuple4{enum Cyc_Absyn_AggrKind f1;struct _tuple2*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 292
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 453 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 460
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 478
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};
# 653 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 841
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 844
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 847
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 854
void*Cyc_Absyn_compress(void*);
# 867
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 879
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 883
extern void*Cyc_Absyn_false_type;
# 885
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_rgn_handle_type(void*);
# 902
void*Cyc_Absyn_exn_type (void);
# 924
void*Cyc_Absyn_at_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 941
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 954
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 964
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 971
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1026
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1039
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1048
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1053
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1057
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple2*,void*,struct Cyc_Absyn_Exp*);
# 1094
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);struct Cyc_RgnOrder_RgnPO;
# 36 "tcutil.h"
int Cyc_Tcutil_is_void_type(void*);
# 38
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 106
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 146
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 150
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 214
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 224
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
# 30
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
# 40
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 67
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 62
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 70
void Cyc_Atts_check_fndecl_atts(unsigned,struct Cyc_List_List*,int);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 71 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned,struct Cyc_Tcenv_Fenv*,struct Cyc_Absyn_Fndecl*);
# 97
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 101
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 103
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Switch_clause*);
# 107
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 125
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple14{struct Cyc_Absyn_Switch_clause*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 128
const struct _tuple14*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 131
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_outlives_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 138
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 140
struct Cyc_RgnOrder_RgnPO*Cyc_Tcenv_curr_rgnpo(struct Cyc_Tcenv_Tenv*);
# 142
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int,int);
# 153
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
# 35 "tctyp.h"
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 44 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr);
void Cyc_Tcexp_check_consume(unsigned,void*,struct Cyc_Absyn_Exp*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_bounds_opt;struct Cyc_List_List*patvars;};
# 53 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,struct Cyc_List_List*);
# 105
void Cyc_Tcpat_check_switch_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 107
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**);
# 109
void Cyc_Tcpat_check_catch_overlap(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*);
# 42 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==5)switch((int)((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2){case Cyc_Absyn_PostInc: _Tmp1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
({void*_Tmp2=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_Tmp3->tag=5,_Tmp3->f1=e1,_Tmp3->f2=0U;_Tmp3;});e->r=_Tmp2;});goto _LL0;}case Cyc_Absyn_PostDec: _Tmp1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
({void*_Tmp2=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_Tmp3->tag=5,_Tmp3->f1=e1,_Tmp3->f2=2U;_Tmp3;});e->r=_Tmp2;});goto _LL0;}default: goto _LL5;}else{_LL5:
 goto _LL0;}_LL0:;}
# 50
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var (void){
static int counter=0;
return({struct _tuple2*_Tmp0=_cycalloc(sizeof(struct _tuple2));_Tmp0->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="_tmpS%X";_tag_fat(_Tmp6,sizeof(char),8U);}),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f2=_Tmp1;});_Tmp0;});}
# 57
static int Cyc_Tcstmt_make_var_exp(struct Cyc_Absyn_Exp**eptr,struct Cyc_Absyn_Stmt*s0){
struct Cyc_Absyn_Exp*e2=*eptr;
int done=0;
while(!done){
void*_Tmp0=e2->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 return 0;case 12: _Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e3=_Tmp1;
_Tmp1=e3;goto _LL6;}case 13: _Tmp1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL6: {struct Cyc_Absyn_Exp*e3=_Tmp1;
e2=e3;continue;}default:
 done=1;};}{
# 67
struct Cyc_Absyn_Exp*e=*eptr;
struct Cyc_Absyn_Vardecl*vd=({struct _tuple2*_Tmp0=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp0,Cyc_Absyn_new_evar(0,0),0);});
struct Cyc_Absyn_Decl*d=({struct Cyc_Absyn_Pat*_Tmp0=({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp2->tag=1,_Tmp2->f1=vd,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,e->loc);});struct Cyc_Absyn_Exp*_Tmp1=e;Cyc_Absyn_let_decl(_Tmp0,_Tmp1,s0->loc);});
# 71
({void*_Tmp0=({struct Cyc_Absyn_Decl*_Tmp1=d;struct Cyc_Absyn_Stmt*_Tmp2=Cyc_Absyn_new_stmt(s0->r,s0->loc);Cyc_Absyn_decl_stmt(_Tmp1,_Tmp2,s0->loc);})->r;s0->r=_Tmp0;});
({struct Cyc_Absyn_Exp*_Tmp0=({void*_Tmp1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp2->tag=4,_Tmp2->f1=vd;_Tmp2;});Cyc_Absyn_varb_exp(_Tmp1,e->loc);});*eptr=_Tmp0;});
return 1;}}struct _tuple15{struct Cyc_Absyn_Tvar*f1;int f2;};struct _tuple16{struct Cyc_Absyn_Vardecl**f1;struct Cyc_Absyn_Exp*f2;};
# 77
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 79
void*_Tmp0;void*_Tmp1;_Tmp1=pat_res.tvars_and_bounds_opt;_Tmp0=pat_res.patvars;{struct _tuple1*p=_Tmp1;struct Cyc_List_List*vse=_Tmp0;
struct Cyc_List_List*vs=({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*))Cyc_Core_fst;}),vse);
struct Cyc_List_List*tvs=p==0?0:({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst;}),(*p).f1);
struct Cyc_List_List*bds=p==0?0:(*p).f2;
struct Cyc_List_List*rgns=p==0?0:({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst;}),({(struct Cyc_List_List*(*)(int(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_filter;})(({(int(*)(struct _tuple15*))Cyc_Core_snd;}),(*p).f1));
te=Cyc_Tcenv_add_type_vars(loc,te,tvs);
for(1;rgns!=0;rgns=rgns->tl){
te=({struct Cyc_Tcenv_Tenv*_Tmp2=te;Cyc_Tcenv_add_region(_Tmp2,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp3->tag=2,_Tmp3->f1=(struct Cyc_Absyn_Tvar*)rgns->hd;_Tmp3;}),0,0);});}
te=Cyc_Tcenv_new_outlives_constraints(te,bds,loc);
if(new_block)
te=Cyc_Tcenv_new_block(loc,te);{
void*ropt=Cyc_Tcenv_curr_rgn(te);
for(1;vs!=0;vs=vs->tl){
if((struct Cyc_Absyn_Vardecl**)vs->hd!=0)
(*_check_null((struct Cyc_Absyn_Vardecl**)vs->hd))->rgn=ropt;}
# 95
if(where_opt!=0){
Cyc_Tcexp_tcTest(te,where_opt,({const char*_Tmp2="switch clause guard";_tag_fat(_Tmp2,sizeof(char),20U);}));
if(!Cyc_Absyn_no_side_effects_exp(where_opt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=({const char*_Tmp4="cannot show &&-clause in pattern has no effects";_tag_fat(_Tmp4,sizeof(char),48U);});_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_err2(where_opt->loc,_tag_fat(_Tmp3,sizeof(void*),1));});}
# 100
Cyc_Tcstmt_tcStmt(te,s,0);}}}
# 103
static int Cyc_Tcstmt_is_case_default(struct Cyc_Absyn_Switch_clause*c){
void*_Tmp0=c->pattern->r;if(*((int*)_Tmp0)==0)
return 1;else{
return 0;};}
# 118 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 120
void*_Tmp0=s0->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 0:
# 122
 return;case 1: _Tmp6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 125
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(e);
return;}case 2: _Tmp6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_Absyn_Stmt*s2=_Tmp5;
# 131
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3: _Tmp6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp6;
# 136
void*t=Cyc_Tcenv_return_typ(te);
if(eopt==0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="should return a value of type ";_tag_fat(_Tmp9,sizeof(char),31U);});_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_warn2(s0->loc,_tag_fat(_Tmp9,sizeof(void*),2));});else{
# 142
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="must return a value of type ";_tag_fat(_Tmp9,sizeof(char),29U);});_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err2(s0->loc,_tag_fat(_Tmp9,sizeof(void*),2));});}}
# 144
return;}
# 147
Cyc_Tcexp_tcExp(te,& t,eopt);
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp7=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_Tmp8=eopt;Cyc_Tcutil_coerce_arg(_Tmp7,_Tmp8,t,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="returns value of type ";_tag_fat(_Tmp9,sizeof(char),23U);});_Tmp8;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp9;_Tmp9.tag=3,_Tmp9.f1=(void*)eopt->topt;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB=" but requires ";_tag_fat(_TmpB,sizeof(char),15U);});_TmpA;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpB;_TmpB.tag=2,_TmpB.f1=(void*)t;_TmpB;});void*_TmpB[4];_TmpB[0]=& _Tmp7,_TmpB[1]=& _Tmp8,_TmpB[2]=& _Tmp9,_TmpB[3]=& _TmpA;Cyc_Warn_err2(s0->loc,_tag_fat(_TmpB,sizeof(void*),4));});
Cyc_Unify_explain_failure();}
# 152
Cyc_Tcexp_check_consume(eopt->loc,t,eopt);
return;}case 4: _Tmp6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 156
Cyc_Tcexp_tcTest(te,e,({const char*_Tmp7="if statement";_tag_fat(_Tmp7,sizeof(char),13U);}));
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5: _Tmp6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f1;_Tmp5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f2;_Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*cont_s=_Tmp5;struct Cyc_Absyn_Stmt*s=_Tmp4;
# 162
Cyc_Tcexp_tcTest(te,e,({const char*_Tmp7="while loop";_tag_fat(_Tmp7,sizeof(char),11U);}));
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9: _Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f2;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f2;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Stmt*guard_s=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;struct Cyc_Absyn_Stmt*cont_s=_Tmp2;struct Cyc_Absyn_Stmt*s=_Tmp1;
# 167
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_tcTest(te,e2,({const char*_Tmp7="for loop";_tag_fat(_Tmp7,sizeof(char),9U);}));
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14: _Tmp6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f2;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp5;struct Cyc_Absyn_Stmt*cont_s=_Tmp4;
# 175
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcTest(te,e,({const char*_Tmp7="do loop";_tag_fat(_Tmp7,sizeof(char),8U);}));
return;}case 6:
# 179
 goto _LL14;case 7: _LL14:
 goto _LL16;case 8: _LL16:
 return;case 13: _Tmp6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*l=_Tmp6;struct Cyc_Absyn_Stmt*s=_Tmp5;
# 186
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=*l;_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_aprintf(({const char*_TmpC="`%s";_tag_fat(_TmpC,sizeof(char),4U);}),_tag_fat(_TmpB,sizeof(void*),1));});*_Tmp8=_Tmp9;});_Tmp8;});tv->name=_Tmp7;}),({
int _Tmp7=Cyc_Tcutil_new_tvar_id();tv->identity=_Tmp7;}),({void*_Tmp7=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_rk);tv->kind=_Tmp7;});
({struct Cyc_Tcenv_Tenv*_Tmp7=Cyc_Tcenv_new_named_block(s0->loc,te,tv);Cyc_Tcstmt_tcStmt(_Tmp7,s,0);});
return;}case 11: _Tmp6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp6;struct Cyc_Absyn_Switch_clause***clauseopt=_Tmp5;
# 192
const struct _tuple14*trip_opt=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);
if(trip_opt==(const struct _tuple14*)0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="fallthru not in a non-last case";_tag_fat(_Tmp9,sizeof(char),32U);});_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(s0->loc,_tag_fat(_Tmp8,sizeof(void*),1));});
return;}{
# 197
struct _tuple14 _Tmp7=*trip_opt;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f2;_Tmp8=_Tmp7.f3;{struct Cyc_List_List*tvs=_Tmp9;struct Cyc_List_List*ts_orig=_Tmp8;
struct Cyc_List_List*instantiation=({struct Cyc_List_List*_TmpA=Cyc_Tcenv_lookup_type_vars(te);({(struct Cyc_List_List*(*)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_make_inst_var,_TmpA,tvs);});
struct Cyc_List_List*ts=({(struct Cyc_List_List*(*)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_Tcutil_substitute,instantiation,ts_orig);
for(1;ts!=0 && es!=0;(ts=ts->tl,es=es->tl)){
# 202
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);
if(!({struct Cyc_RgnOrder_RgnPO*_TmpA=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_TmpB=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_TmpA,_TmpB,(void*)ts->hd,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="fallthru argument has type ";_tag_fat(_TmpC,sizeof(char),28U);});_TmpB;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpB=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpC;_TmpC.tag=3,_TmpC.f1=(void*)((struct Cyc_Absyn_Exp*)es->hd)->topt;_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=({const char*_TmpE="but pattern variable has type ";_tag_fat(_TmpE,sizeof(char),31U);});_TmpD;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpD=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpE;_TmpE.tag=2,_TmpE.f1=(void*)ts->hd;_TmpE;});void*_TmpE[4];_TmpE[0]=& _TmpA,_TmpE[1]=& _TmpB,_TmpE[2]=& _TmpC,_TmpE[3]=& _TmpD;Cyc_Warn_err2(s0->loc,_tag_fat(_TmpE,sizeof(void*),4));});
# 206
Cyc_Unify_explain_failure();}
# 209
Cyc_Tcexp_check_consume(((struct Cyc_Absyn_Exp*)es->hd)->loc,(void*)ts->hd,(struct Cyc_Absyn_Exp*)es->hd);}
# 211
if(es!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="too many arguments in explicit fallthru";_tag_fat(_TmpC,sizeof(char),40U);});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(s0->loc,_tag_fat(_TmpB,sizeof(void*),1));});
if(ts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC="too few arguments in explicit fallthru";_tag_fat(_TmpC,sizeof(char),39U);});_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(s0->loc,_tag_fat(_TmpB,sizeof(void*),1));});
return;}}}case 10: _Tmp6=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp**exp=_Tmp6;struct Cyc_List_List*scs0=_Tmp5;void**dtp=_Tmp4;
# 219
if(Cyc_Tcstmt_make_var_exp(exp,s0)){
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 224
struct Cyc_Absyn_Exp*e=*exp;
void*t=Cyc_Tcexp_tcExp(te,0,e);
# 227
Cyc_Tcexp_check_consume(e->loc,t,e);{
# 231
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*scs=scs0;for(0;scs!=0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& t,e);}else{
# 239
this_pattern=*_check_null(next_pattern);}
# 241
if(scs->tl!=0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_Tmp7=_cycalloc(sizeof(struct Cyc_Tcpat_TcPatResult));({struct Cyc_Tcpat_TcPatResult _Tmp8=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_check_null(scs->tl)->hd)->pattern,& t,e);*_Tmp7=_Tmp8;});_Tmp7;});
if(next_pattern->tvars_and_bounds_opt!=0 &&(*next_pattern->tvars_and_bounds_opt).f2!=0)
# 245
te=Cyc_Tcenv_clear_fallthru(te);else{
# 247
struct Cyc_List_List*vs=Cyc_List_imp_rev(Cyc_Tcutil_filter_nulls(Cyc_List_split(next_pattern->patvars).f1));
struct Cyc_List_List*new_tvs=next_pattern->tvars_and_bounds_opt==0?0:
({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst;}),(*next_pattern->tvars_and_bounds_opt).f1);
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)_check_null(scs->tl)->hd);}}else{
# 253
te=Cyc_Tcenv_clear_fallthru(te);}{
# 255
struct Cyc_Absyn_Pat*p=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
if(!Cyc_Unify_unify(_check_null(p->topt),t)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="switch on type ";_tag_fat(_Tmp9,sizeof(char),16U);});_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB=" but case expects type ";_tag_fat(_TmpB,sizeof(char),24U);});_TmpA;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpA=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpB;_TmpB.tag=3,_TmpB.f1=(void*)p->topt;_TmpB;});void*_TmpB[4];_TmpB[0]=& _Tmp7,_TmpB[1]=& _Tmp8,_TmpB[2]=& _Tmp9,_TmpB[3]=& _TmpA;Cyc_Warn_err2(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tag_fat(_TmpB,sizeof(void*),4));});
# 259
Cyc_Unify_explain_failure();}else{
# 262
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 264
({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp8->v=this_pattern.patvars;_Tmp8;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_Tmp7;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 268
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,scs0,dtp);
return;}}}case 15: _Tmp6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_List_List**scs0=_Tmp5;void**dtp=(void**)_Tmp4;
# 276
({void*_Tmp7=({void*_Tmp8=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_Tmp9->tag=2,({struct Cyc_Absyn_Stmt*_TmpA=Cyc_Absyn_new_stmt(s->r,s->loc);_Tmp9->f1=_TmpA;}),({struct Cyc_Absyn_Stmt*_TmpA=Cyc_Absyn_skip_stmt(s->loc);_Tmp9->f2=_TmpA;});_Tmp9;});Cyc_Absyn_new_stmt(_Tmp8,s->loc);})->r;s->r=_Tmp7;});
# 278
Cyc_Tcstmt_tcStmt(te,s,1);
# 280
if(!({(int(*)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))Cyc_List_exists;})(Cyc_Tcstmt_is_case_default,*scs0)){
# 282
struct _tuple2*def_v;def_v=_cycalloc(sizeof(struct _tuple2)),def_v->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));*_Tmp8=({const char*_Tmp9="exn";_tag_fat(_Tmp9,sizeof(char),4U);});_Tmp8;});def_v->f2=_Tmp7;});{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0);
struct Cyc_Absyn_Pat*def_pat=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp7->tag=1,_Tmp7->f1=vd,({struct Cyc_Absyn_Pat*_Tmp8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_Tmp7->f2=_Tmp8;});_Tmp7;}),0U);
struct Cyc_Absyn_Stmt*def_stmt=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct));_Tmp7->tag=11,({struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp9->tag=4,_Tmp9->f1=vd;_Tmp9;}),0U);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=1;_Tmp7;}),0U),0U);
struct Cyc_Absyn_Switch_clause*def_clause;def_clause=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause)),def_clause->pattern=def_pat,def_clause->pat_vars=0,def_clause->where_clause=0,def_clause->body=def_stmt,def_clause->loc=0U;
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=*scs0;Cyc_List_append(_Tmp8,({struct Cyc_Absyn_Switch_clause*_Tmp9[1];_Tmp9[0]=def_clause;Cyc_List_list(_tag_fat(_Tmp9,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));});*scs0=_Tmp7;});}}{
# 294
void*exception_type=Cyc_Absyn_exn_type();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*scs=*scs0;for(0;scs!=0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& exception_type,0);}else{
# 303
this_pattern=*_check_null(next_pattern);}
# 305
if(scs->tl!=0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_Tmp7=_cycalloc(sizeof(struct Cyc_Tcpat_TcPatResult));({struct Cyc_Tcpat_TcPatResult _Tmp8=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)_check_null(scs->tl)->hd)->pattern,& exception_type,0);*_Tmp7=_Tmp8;});_Tmp7;});
if(next_pattern->tvars_and_bounds_opt!=0 &&(*next_pattern->tvars_and_bounds_opt).f2!=0)
# 309
te=Cyc_Tcenv_clear_fallthru(te);else{
# 311
struct Cyc_List_List*vs=Cyc_List_imp_rev(Cyc_Tcutil_filter_nulls(Cyc_List_split(next_pattern->patvars).f1));
struct Cyc_List_List*new_tvs=next_pattern->tvars_and_bounds_opt==0?0:
({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst;}),(*next_pattern->tvars_and_bounds_opt).f1);
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)_check_null(scs->tl)->hd);}}else{
# 317
te=Cyc_Tcenv_clear_fallthru(te);}{
# 319
struct Cyc_Absyn_Pat*p=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
if(!Cyc_Unify_unify(_check_null(p->topt),exception_type)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=({const char*_Tmp9="switch on type ";_tag_fat(_Tmp9,sizeof(char),16U);});_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)exception_type;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=({const char*_TmpB=" but case expects type ";_tag_fat(_TmpB,sizeof(char),24U);});_TmpA;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpA=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpB;_TmpB.tag=3,_TmpB.f1=(void*)p->topt;_TmpB;});void*_TmpB[4];_TmpB[0]=& _Tmp7,_TmpB[1]=& _Tmp8,_TmpB[2]=& _Tmp9,_TmpB[3]=& _TmpA;Cyc_Warn_err2(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tag_fat(_TmpB,sizeof(void*),4));});
# 323
Cyc_Unify_explain_failure();}else{
# 326
Cyc_Tcpat_check_pat_regions(te,p,this_pattern.patvars);}
# 328
({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp8->v=this_pattern.patvars;_Tmp8;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_Tmp7;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 332
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*scs0,dtp);
return;}}default: _Tmp6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp6;struct Cyc_Absyn_Stmt*s=_Tmp5;
# 336
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_Tmp7=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;struct Cyc_Tcenv_Tenv*te=_Tmp7;
{void*_Tmp8=d->r;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;switch(*((int*)_Tmp8)){case 0: _TmpC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;{struct Cyc_Absyn_Vardecl*vd=_TmpC;
# 340
void*_TmpD;void*_TmpE;void*_TmpF;struct Cyc_Absyn_Tqual _Tmp10;enum Cyc_Absyn_Scope _Tmp11;_Tmp11=vd->sc;_Tmp10=vd->tq;_TmpF=vd->type;_TmpE=vd->initializer;_TmpD=(void**)& vd->rgn;{enum Cyc_Absyn_Scope sc=_Tmp11;struct Cyc_Absyn_Tqual tq=_Tmp10;void*t=_TmpF;struct Cyc_Absyn_Exp*initializer=_TmpE;void**rgn_ptr=(void**)_TmpD;
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
int is_local;
switch((int)sc){case Cyc_Absyn_Static:
 goto _LL4E;case Cyc_Absyn_Extern: _LL4E:
 goto _LL50;case Cyc_Absyn_ExternC: _LL50:
 vd->escapes=1;is_local=0;goto _LL4A;case Cyc_Absyn_Abstract:
# 348
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=({const char*_Tmp14="bad abstract scope for local variable";_tag_fat(_Tmp14,sizeof(char),38U);});_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp13,sizeof(void*),1));});
goto _LL54;case Cyc_Absyn_Register: _LL54:
 goto _LL56;case Cyc_Absyn_Public: _LL56:
 goto _LL58;default: _LL58:
 is_local=1;goto _LL4A;}_LL4A:;
# 355
*rgn_ptr=is_local?curr_bl: Cyc_Absyn_heap_rgn_type;
{void*_Tmp12=Cyc_Absyn_compress(t);struct Cyc_Absyn_ArrayInfo _Tmp13;switch(*((int*)_Tmp12)){case 5: if(is_local){
# 359
vd->escapes=1;
sc=3U;
is_local=0;
goto _LL59;}else{goto _LL5E;}case 4: _Tmp13=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp12)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp13;
# 365
if(vd->initializer==0 || ai.num_elts!=0)
goto _LL59;{
struct Cyc_Absyn_Exp*len;
{void*_Tmp14=_check_null(vd->initializer)->r;void*_Tmp15;struct _fat_ptr _Tmp16;switch(*((int*)_Tmp14)){case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp14)->f1.Wstring_c.tag){case 8: _Tmp16=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp14)->f1.String_c.val;{struct _fat_ptr s=_Tmp16;
len=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);goto _LL60;}case 9: _Tmp16=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp14)->f1.Wstring_c.val;{struct _fat_ptr s=_Tmp16;
len=Cyc_Absyn_uint_exp(1U,0U);goto _LL60;}default: goto _LL6D;}case 27: _Tmp15=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp14)->f2;{struct Cyc_Absyn_Exp*e=_Tmp15;
_Tmp15=e;goto _LL68;}case 28: _Tmp15=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp14)->f1;_LL68: {struct Cyc_Absyn_Exp*e=_Tmp15;
len=e;goto _LL60;}case 36: _Tmp15=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp14)->f2;{struct Cyc_List_List*es=_Tmp15;
_Tmp15=es;goto _LL6C;}case 26: _Tmp15=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp14)->f1;_LL6C: {struct Cyc_List_List*es=_Tmp15;
len=Cyc_Absyn_uint_exp((unsigned)Cyc_List_length(es),0U);goto _LL60;}default: _LL6D:
 len=0;goto _LL60;}_LL60:;}
# 377
ai.num_elts=len;
t=({void*_Tmp14=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_Tmp15->tag=4,_Tmp15->f1=ai;_Tmp15;});vd->type=_Tmp14;});
goto _LL59;}}default: _LL5E:
 goto _LL59;}_LL59:;}{
# 385
struct Cyc_List_List*bound_vars=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);
int allow_evars=is_local;
Cyc_Tctyp_check_type(s0->loc,te,bound_vars,& Cyc_Kinds_tmk,allow_evars,1,t);
({int _Tmp12=Cyc_Tcutil_extract_const_from_typedef(s0->loc,vd->tq.print_const,t);vd->tq.real_const=_Tmp12;});
# 390
if((int)sc==3 ||(int)sc==4)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=({const char*_Tmp14="extern declarations are not yet supported within functions";_tag_fat(_Tmp14,sizeof(char),59U);});_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp13,sizeof(void*),1));});
if(initializer!=0){
Cyc_Tcexp_tcExpInitializer(te,& t,initializer);
# 395
if(!is_local && !Cyc_Tcutil_is_const_exp(initializer))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=({const char*_Tmp14="initializer for static variable needs to be a constant expression";_tag_fat(_Tmp14,sizeof(char),66U);});_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp13,sizeof(void*),1));});
if(!({struct Cyc_RgnOrder_RgnPO*_Tmp12=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_Tmp13=initializer;Cyc_Tcutil_coerce_assign(_Tmp12,_Tmp13,t);})){
({struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp13;_Tmp13.tag=11,_Tmp13.f1=vd;_Tmp13;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp14;_Tmp14.tag=0,_Tmp14.f1=({const char*_Tmp15=" was declared with type ";_tag_fat(_Tmp15,sizeof(char),25U);});_Tmp14;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp14=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp15;_Tmp15.tag=2,_Tmp15.f1=(void*)t;_Tmp15;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=({const char*_Tmp17=" but initialized with type ";_tag_fat(_Tmp17,sizeof(char),28U);});_Tmp16;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp17;_Tmp17.tag=3,_Tmp17.f1=(void*)initializer->topt;_Tmp17;});void*_Tmp17[5];_Tmp17[0]=& _Tmp12,_Tmp17[1]=& _Tmp13,_Tmp17[2]=& _Tmp14,_Tmp17[3]=& _Tmp15,_Tmp17[4]=& _Tmp16;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp17,sizeof(void*),5));});
# 400
Cyc_Unify_unify(t,_check_null(initializer->topt));
Cyc_Unify_explain_failure();}}
# 404
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}case 2: _TmpC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;_TmpB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp8)->f2;_TmpA=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp8)->f3;_Tmp9=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp8)->f4;{struct Cyc_Absyn_Pat*p=_TmpC;struct Cyc_Core_Opt**vds=_TmpB;struct Cyc_Absyn_Exp**exp=_TmpA;void**dtp=(void**)_Tmp9;
# 409
{void*_TmpD=p->r;switch(*((int*)_TmpD)){case 1:
 goto _LL73;case 2: _LL73:
 goto _LL75;case 15: _LL75:
 goto _LL6F;default:
# 414
 if(Cyc_Tcstmt_make_var_exp(exp,s0)){
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}}_LL6F:;}{
# 420
struct Cyc_Absyn_Exp*e=*exp;
Cyc_Tcexp_tcExpInitializer(te,0,e);{
# 423
void*pat_type=_check_null(e->topt);
# 425
struct Cyc_Tcpat_TcPatResult pat_res=Cyc_Tcpat_tcPat(te,p,& pat_type,e);
({struct Cyc_Core_Opt*_TmpD=({struct Cyc_Core_Opt*_TmpE=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpE->v=pat_res.patvars;_TmpE;});*vds=_TmpD;});{
void*pt=_check_null(p->topt);
void*et=_check_null(e->topt);
if(!Cyc_Unify_unify(pt,et)&& !({struct Cyc_RgnOrder_RgnPO*_TmpD=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_TmpE=e;Cyc_Tcutil_coerce_assign(_TmpD,_TmpE,pt);})){
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF="pattern type ";_tag_fat(_TmpF,sizeof(char),14U);});_TmpE;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpF;_TmpF.tag=2,_TmpF.f1=(void*)pt;_TmpF;});struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=({const char*_Tmp11=" does not match definition type ";_tag_fat(_Tmp11,sizeof(char),33U);});_Tmp10;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp11;_Tmp11.tag=2,_Tmp11.f1=(void*)et;_Tmp11;});void*_Tmp11[4];_Tmp11[0]=& _TmpD,_Tmp11[1]=& _TmpE,_Tmp11[2]=& _TmpF,_Tmp11[3]=& _Tmp10;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp11,sizeof(void*),4));});
# 432
Cyc_Unify_unify(pt,et);
Cyc_Unify_explain_failure();}else{
# 436
Cyc_Tcpat_check_pat_regions(te,p,pat_res.patvars);}
Cyc_Tcpat_check_let_pat_exhaustive(p->loc,te,p,dtp);
Cyc_Tcstmt_pattern_synth(s0->loc,te,pat_res,s,0,0);
return;}}}}case 3: _TmpC=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;{struct Cyc_List_List*vds=_TmpC;
# 442
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
for(1;vds!=0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=curr_bl;
({unsigned _TmpD=s0->loc;struct Cyc_Tcenv_Tenv*_TmpE=te;struct Cyc_List_List*_TmpF=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_TmpD,_TmpE,_TmpF,& Cyc_Kinds_tmk,1,1,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);});}
# 448
Cyc_Tcstmt_tcStmt(te,s,0);
return;}case 4: _TmpC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;_TmpB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp8)->f2;_TmpA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp8)->f3;{struct Cyc_Absyn_Tvar*tv=_TmpC;struct Cyc_Absyn_Vardecl*vd=_TmpB;struct Cyc_Absyn_Exp*open_exp_opt=_TmpA;
# 454
({void*_TmpD=({void*_TmpE=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_TmpF->tag=2,({struct Cyc_Absyn_Stmt*_Tmp10=Cyc_Absyn_new_stmt(s->r,s->loc);_TmpF->f1=_Tmp10;}),({struct Cyc_Absyn_Stmt*_Tmp10=Cyc_Absyn_skip_stmt(s->loc);_TmpF->f2=_Tmp10;});_TmpF;});Cyc_Absyn_new_stmt(_TmpE,s->loc);})->r;s->r=_TmpD;});{
# 456
struct Cyc_Tcenv_Tenv*te2=te;
void*_TmpD;void*_TmpE;_TmpE=(void**)& vd->type;_TmpD=(void**)& vd->rgn;{void**t=(void**)_TmpE;void**rgn_ptr=(void**)_TmpD;
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
*rgn_ptr=curr_bl;{
void*rgn_typ;
if((unsigned)open_exp_opt){
# 464
struct _tuple2*drname;drname=_cycalloc(sizeof(struct _tuple2)),({union Cyc_Absyn_Nmspace _TmpF=Cyc_Absyn_Abs_n(({struct _fat_ptr*_Tmp10[1];({struct _fat_ptr*_Tmp11=({struct _fat_ptr*_Tmp12=_cycalloc(sizeof(struct _fat_ptr));*_Tmp12=({const char*_Tmp13="Core";_tag_fat(_Tmp13,sizeof(char),5U);});_Tmp12;});_Tmp10[0]=_Tmp11;});Cyc_List_list(_tag_fat(_Tmp10,sizeof(struct _fat_ptr*),1));}),0);drname->f1=_TmpF;}),({struct _fat_ptr*_TmpF=({struct _fat_ptr*_Tmp10=_cycalloc(sizeof(struct _fat_ptr));*_Tmp10=({const char*_Tmp11="DynamicRegion";_tag_fat(_Tmp11,sizeof(char),14U);});_Tmp10;});drname->f2=_TmpF;});{
void*outer_rgn=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_TmpF=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpF->v=& Cyc_Kinds_trk;_TmpF;}),0);
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_TmpF=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpF->v=& Cyc_Kinds_rk;_TmpF;}),0);{
void*dr_type=({union Cyc_Absyn_AggrInfo _TmpF=Cyc_Absyn_UnknownAggr(0U,drname,0);Cyc_Absyn_aggr_type(_TmpF,({void*_Tmp10[1];_Tmp10[0]=rgn_typ;Cyc_List_list(_tag_fat(_Tmp10,sizeof(void*),1));}));});
void*exp_type=({void*_TmpF=dr_type;void*_Tmp10=outer_rgn;struct Cyc_Absyn_Tqual _Tmp11=Cyc_Absyn_empty_tqual(0U);void*_Tmp12=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_TmpF,_Tmp10,_Tmp11,_Tmp12,Cyc_Absyn_false_type);});
({unsigned _TmpF=s0->loc;struct Cyc_Tcenv_Tenv*_Tmp10=te;struct Cyc_List_List*_Tmp11=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_TmpF,_Tmp10,_Tmp11,& Cyc_Kinds_tmk,1,0,exp_type);});{
# 471
void*key_typ=Cyc_Tcexp_tcExp(te,& exp_type,open_exp_opt);
if(!Cyc_Unify_unify(exp_type,key_typ)&& !({
struct Cyc_RgnOrder_RgnPO*_TmpF=Cyc_Tcenv_curr_rgnpo(te);struct Cyc_Absyn_Exp*_Tmp10=open_exp_opt;Cyc_Tcutil_coerce_assign(_TmpF,_Tmp10,exp_type);}))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=({const char*_Tmp11="expected ";_tag_fat(_Tmp11,sizeof(char),10U);});_Tmp10;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp11;_Tmp11.tag=2,_Tmp11.f1=(void*)exp_type;_Tmp11;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=({const char*_Tmp13=" but found ";_tag_fat(_Tmp13,sizeof(char),12U);});_Tmp12;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp13;_Tmp13.tag=2,_Tmp13.f1=(void*)key_typ;_Tmp13;});void*_Tmp13[4];_Tmp13[0]=& _TmpF,_Tmp13[1]=& _Tmp10,_Tmp13[2]=& _Tmp11,_Tmp13[3]=& _Tmp12;Cyc_Warn_err2(s0->loc,_tag_fat(_Tmp13,sizeof(void*),4));});
if(!Cyc_Unify_unify(outer_rgn,Cyc_Absyn_unique_rgn_type)&& !
Cyc_Unify_unify(outer_rgn,Cyc_Absyn_refcnt_rgn_type))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=({const char*_Tmp11="open is only allowed on unique or reference-counted keys";_tag_fat(_Tmp11,sizeof(char),57U);});_Tmp10;});void*_Tmp10[1];_Tmp10[0]=& _TmpF;Cyc_Warn_err2(s0->loc,_tag_fat(_Tmp10,sizeof(void*),1));});
({void*_TmpF=Cyc_Absyn_rgn_handle_type(rgn_typ);*t=_TmpF;});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1,0);}}}}else{
# 481
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_TmpF->tag=2,_TmpF->f1=tv;_TmpF;});
te2=({unsigned _TmpF=s0->loc;struct Cyc_Tcenv_Tenv*_Tmp10=te2;Cyc_Tcenv_add_type_vars(_TmpF,_Tmp10,({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));_Tmp11->hd=tv,_Tmp11->tl=0;_Tmp11;}));});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0,1);}
# 485
({unsigned _TmpF=s0->loc;struct Cyc_Tcenv_Tenv*_Tmp10=te;struct Cyc_List_List*_Tmp11=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_TmpF,_Tmp10,_Tmp11,& Cyc_Kinds_bk,1,0,*t);});
if(!({void*_TmpF=*t;Cyc_Unify_unify(_TmpF,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_TmpF="region stmt: type of region handle is wrong!";_tag_fat(_TmpF,sizeof(char),45U);}),_tag_fat(0U,sizeof(void*),0));
# 489
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1: _TmpC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;{struct Cyc_Absyn_Fndecl*fd=_TmpC;
# 493
if((int)fd->sc!=2)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=({const char*_TmpF="bad storage class for inner function";_tag_fat(_TmpF,sizeof(char),37U);});_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;Cyc_Warn_err2(d->loc,_tag_fat(_TmpE,sizeof(void*),1));});
Cyc_Atts_check_fndecl_atts(d->loc,fd->i.attributes,1);
Cyc_Tctyp_check_fndecl_valid_type(d->loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
({struct Cyc_List_List*_TmpD=Cyc_Atts_transfer_fn_type_atts(t,fd->i.attributes);fd->i.attributes=_TmpD;});{
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
struct Cyc_Absyn_Vardecl*vd;vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),vd->sc=fd->sc,vd->name=fd->name,vd->varloc=0U,({struct Cyc_Absyn_Tqual _TmpD=Cyc_Absyn_const_tqual(0U);vd->tq=_TmpD;}),({
void*_TmpD=({void*_TmpE=t;void*_TmpF=curr_bl;struct Cyc_Absyn_Tqual _Tmp10=Cyc_Absyn_empty_tqual(0U);void*_Tmp11=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_TmpE,_TmpF,_Tmp10,_Tmp11,Cyc_Absyn_false_type);});vd->type=_TmpD;}),vd->initializer=0,vd->rgn=curr_bl,vd->attributes=0,vd->escapes=0,vd->is_proto=fd->body!=0;
# 505
fd->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=_check_null(te->le);
({struct Cyc_Tcenv_Fenv*_TmpD=Cyc_Tcenv_nested_fenv(d->loc,old_fenv,fd);te->le=_TmpD;});
Cyc_Tcstmt_tcStmt(te,fd->body,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
te->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9: _TmpC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;_TmpB=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp8)->f2;{struct _fat_ptr*n=_TmpC;struct Cyc_List_List*tds=_TmpB;
unimp_msg_part=({const char*_TmpD="namespace";_tag_fat(_TmpD,sizeof(char),10U);});goto _LL24;}case 10: _TmpC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;_TmpB=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp8)->f2;{struct _tuple2*q=_TmpC;struct Cyc_List_List*tds=_TmpB;
unimp_msg_part=({const char*_TmpD="using";_tag_fat(_TmpD,sizeof(char),6U);});goto _LL24;}case 5:
 unimp_msg_part=({const char*_TmpD="type";_tag_fat(_TmpD,sizeof(char),5U);});goto _LL24;case 6:
 unimp_msg_part=({const char*_TmpD="datatype";_tag_fat(_TmpD,sizeof(char),9U);});goto _LL24;case 7:
 unimp_msg_part=({const char*_TmpD="enum";_tag_fat(_TmpD,sizeof(char),5U);});goto _LL24;case 8:
 unimp_msg_part=({const char*_TmpD="typedef";_tag_fat(_TmpD,sizeof(char),8U);});goto _LL24;case 11:
 unimp_msg_part=({const char*_TmpD="extern \"C\"";_tag_fat(_TmpD,sizeof(char),11U);});goto _LL24;case 12:
 unimp_msg_part=({const char*_TmpD="extern \"C include\"";_tag_fat(_TmpD,sizeof(char),19U);});goto _LL24;case 13:
 unimp_msg_part=({const char*_TmpD="__cyclone_port_on__";_tag_fat(_TmpD,sizeof(char),20U);});goto _LL24;case 14:
 unimp_msg_part=({const char*_TmpD="__cyclone_port_off__";_tag_fat(_TmpD,sizeof(char),21U);});goto _LL24;case 15:
 unimp_msg_part=({const char*_TmpD="__tempest_on__";_tag_fat(_TmpD,sizeof(char),15U);});goto _LL24;default:
 unimp_msg_part=({const char*_TmpD="__tempest_off__";_tag_fat(_TmpD,sizeof(char),16U);});goto _LL24;}_LL24:;}
# 527
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=({const char*_TmpA="tcStmt: nested ";_tag_fat(_TmpA,sizeof(char),16U);});_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=unimp_msg_part;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=({const char*_TmpC=" declarations not allowed";_tag_fat(_TmpC,sizeof(char),26U);});_TmpB;});void*_TmpB[3];_TmpB[0]=& _Tmp8,_TmpB[1]=& _Tmp9,_TmpB[2]=& _TmpA;({(int(*)(struct _fat_ptr))Cyc_Warn_impos2;})(_tag_fat(_TmpB,sizeof(void*),3));});}};}
# 539 "tcstmt.cyc"
static int Cyc_Tcstmt_check_catchall_handler(struct Cyc_Absyn_Stmt*b){
void*_Tmp0=b->r;void*_Tmp1;if(*((int*)_Tmp0)==15){_Tmp1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*case_list=_Tmp1;
# 542
int found_wild=0;
while((unsigned)case_list){
struct Cyc_Absyn_Switch_clause*clause=(struct Cyc_Absyn_Switch_clause*)case_list->hd;
{void*_Tmp2=clause->pattern->r;if(*((int*)_Tmp2)==0){
# 547
found_wild=1;
goto _LL5;}else{
# 550
goto _LL5;}_LL5:;}
# 552
if(!Cyc_Tcstmt_ensure_no_throw_stmt(clause->body))
return 0;
case_list=case_list->tl;}
# 556
return found_wild;}}else{
# 558
return 0;};}struct _tuple17{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 562
static int Cyc_Tcstmt_ensure_no_throw_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;enum Cyc_Absyn_Coercion _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0:
# 565
 return 1;case 3: _Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp5;
# 568
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;})(Cyc_Tcstmt_ensure_no_throw_exp,es);}case 7: _Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL8;}case 8: _Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LLA;}case 35: _Tmp5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LLC;}case 9: _Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
# 573
return Cyc_Tcstmt_ensure_no_throw_exp(e1)&& Cyc_Tcstmt_ensure_no_throw_exp(e2);}case 10: _Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*fname=_Tmp5;struct Cyc_List_List*args=_Tmp4;struct Cyc_Absyn_VarargCallInfo*vargs=_Tmp3;int res=_Tmp2;
# 575
if(!res)
return 0;{
int fname_ok=0;
{void*_Tmp6=fname->r;void*_Tmp7;void*_Tmp8;switch(*((int*)_Tmp6)){case 13: _Tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp6)->f1;_Tmp7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp6)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_List_List*ts=_Tmp7;
# 580
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(e1);
goto _LL1D;}case 1: _Tmp8=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp6)->f1;{void*b=_Tmp8;
# 583
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(fname);
goto _LL1D;}default:
# 586
 goto _LL1D;}_LL1D:;}
# 588
return fname_ok &&({(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;})(Cyc_Tcstmt_ensure_no_throw_exp,args);}}case 1: _Tmp5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp5;
# 590
void*_Tmp6;switch(*((int*)b)){case 2: _Tmp6=((struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp6;
# 592
void*_Tmp7=fd->cached_type;struct Cyc_Absyn_FnInfo _Tmp8;if(_Tmp7!=0){if(*((int*)_Tmp7)==5){_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp7)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp8;
# 594
return Cyc_List_mem(Cyc_Atts_attribute_cmp,fi.attributes,(void*)& Cyc_Atts_No_throw_att_val);}}else{
# 602
return 0;}}else{
# 598
return Cyc_List_mem(Cyc_Atts_attribute_cmp,fd->i.attributes,(void*)& Cyc_Atts_No_throw_att_val);};}case 1: _Tmp6=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)b)->f1;{struct Cyc_Absyn_Vardecl*fvard=_Tmp6;
# 606
void*_Tmp7=fvard->type;struct Cyc_Absyn_FnInfo _Tmp8;if(*((int*)_Tmp7)==5){_Tmp8=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_Tmp7)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp8;
# 608
return Cyc_List_mem(Cyc_Atts_attribute_cmp,fi.attributes,(void*)& Cyc_Atts_No_throw_att_val)||
# 611
 Cyc_List_mem(Cyc_Atts_attribute_cmp,fvard->attributes,(void*)& Cyc_Atts_No_throw_att_val);}}else{
# 615
return 0;};}case 3:
# 618
 goto _LL2C;case 4: _LL2C:
# 620
 return 1;default:
# 622
 return 0;};}case 15: _Tmp5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 625
return Cyc_Tcstmt_ensure_no_throw_exp(e1);}case 29: _Tmp5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*exps=_Tmp5;
# 627
return({(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;})(Cyc_Tcstmt_ensure_no_throw_exp,
({(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;})(({(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd;}),exps));}case 14: _Tmp5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*ct=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp4;int b=_Tmp2;enum Cyc_Absyn_Coercion c=_Tmp1;
# 631
return !b && Cyc_Tcstmt_ensure_no_throw_exp(e1);}case 20: _Tmp5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL1A;}case 22: _Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp5;
# 634
void*_Tmp6=e1->topt;void*_Tmp7;if(_Tmp6!=0){if(*((int*)_Tmp6)==3){_Tmp7=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1;{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_Tmp7;
# 636
return !((unsigned)pi->ptr_atts.nullable);}}else{goto _LL3E;}}else{_LL3E:
# 638
 return 0;};}default:
# 642
 return 0;};}
# 655 "tcstmt.cyc"
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*s){
void*_Tmp0=s->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 6:
 goto _LL4;case 7: _LL4:
# 659
 return 1;case 15:
# 661
 return Cyc_Tcstmt_check_catchall_handler(s);case 2: _Tmp3=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 663
return Cyc_Tcstmt_ensure_no_throw_stmt(s1)&& Cyc_Tcstmt_ensure_no_throw_stmt(s2);}case 1: _Tmp3=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 665
return Cyc_Tcstmt_ensure_no_throw_exp(e);}case 3: _Tmp3=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
# 667
if((unsigned)e)
return Cyc_Tcstmt_ensure_no_throw_exp(e);
# 670
return 1;}case 12: _Tmp3=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp3;struct Cyc_Absyn_Stmt*s2=_Tmp2;
# 672
int init_okay=0;
{void*_Tmp4=d->r;void*_Tmp5;switch(*((int*)_Tmp4)){case 0: _Tmp5=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp4)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;
# 675
init_okay=(unsigned)vd->initializer?Cyc_Tcstmt_ensure_no_throw_exp(_check_null(vd->initializer)): 1;
goto _LL15;}case 2: _Tmp5=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp4)->f3;{struct Cyc_Absyn_Exp*i=_Tmp5;
# 678
init_okay=Cyc_Tcstmt_ensure_no_throw_exp(i);
goto _LL15;}default:
# 681
 return 0;}_LL15:;}
# 683
return init_okay && Cyc_Tcstmt_ensure_no_throw_stmt(s2);}case 13: _Tmp3=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*l=_Tmp3;struct Cyc_Absyn_Stmt*s1=_Tmp2;
# 685
return Cyc_Tcstmt_ensure_no_throw_stmt(s1);}case 4: _Tmp3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp1=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e1=_Tmp3;struct Cyc_Absyn_Stmt*s1=_Tmp2;struct Cyc_Absyn_Stmt*s2=_Tmp1;
# 687
return(Cyc_Tcstmt_ensure_no_throw_exp(e1)&& Cyc_Tcstmt_ensure_no_throw_stmt(s1))&& Cyc_Tcstmt_ensure_no_throw_stmt(s2);}default:
# 689
 return 0;};}
