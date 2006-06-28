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
 struct Cyc_Core_Opt{void*v;};struct _tuple0{void*f0;void*f1;};
# 113 "core.h"
void*Cyc_Core_fst(struct _tuple0*);
# 115
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
extern int Cyc_List_exists(int(*)(void*),struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 294
extern struct _tuple1 Cyc_List_split(struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 332
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 526 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 533
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 551
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Subset_coercion =3U,Cyc_Absyn_Other_coercion =4U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};
# 734 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 929
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
# 932
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 935
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 942
void*Cyc_Absyn_compress(void*);
# 955
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 969
extern void*Cyc_Absyn_heap_rgn_type;
# 971
extern void*Cyc_Absyn_al_qual_type;
# 975
extern void*Cyc_Absyn_false_type;
# 977
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_rgn_handle_type(void*);
# 1000
void*Cyc_Absyn_exn_type (void);
# 1026
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1043
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1068
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1075
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1140
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1153
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1162
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1167
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1171
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple2*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1215
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 1244
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
# 48 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);
# 30 "tcutil.h"
int Cyc_Tcutil_is_void_type(void*);
# 32
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 105
int Cyc_Tcutil_coerce_arg(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
# 131 "tcutil.h"
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 159
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 163
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 217
int Cyc_Tcutil_new_tvar_id (void);
# 226
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 232
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 242
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 272
int Cyc_Tcutil_is_aliasable_qual(void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
# 36
extern struct Cyc_Absyn_Kind Cyc_Kinds_aqk;
# 77 "kinds.h"
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 62
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 70
void Cyc_Atts_check_fndecl_atts(unsigned,struct Cyc_List_List*,int);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 70 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_nested_fenv(unsigned,struct Cyc_Tcenv_Fenv*,struct Cyc_Absyn_Fndecl*);
# 96
void*Cyc_Tcenv_return_typ(struct Cyc_Tcenv_Tenv*);
# 98
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
# 100
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_type_vars(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);
# 102
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_set_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_Absyn_Switch_clause*);
# 106
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_fallthru(struct Cyc_Tcenv_Tenv*);
# 124
int Cyc_Tcenv_in_stmt_exp(struct Cyc_Tcenv_Tenv*);struct _tuple14{struct Cyc_Absyn_Switch_clause*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 127
const struct _tuple14*const Cyc_Tcenv_process_fallthru(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,struct Cyc_Absyn_Switch_clause***);
# 130
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_block(unsigned,struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_named_block(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Tvar*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_new_effect_constraints(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,unsigned);
# 139
void*Cyc_Tcenv_curr_rgn(struct Cyc_Tcenv_Tenv*);
# 143
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_aquals_bound(struct Cyc_Tcenv_Tenv*,void*,void*);
# 146
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_add_region(struct Cyc_Tcenv_Tenv*,void*,int,int);
# 154
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
# 35 "tctyp.h"
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 47 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcexp_tcTest(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,struct _fat_ptr);
void Cyc_Tcexp_check_consume(unsigned,struct Cyc_List_List*,void*,struct Cyc_Absyn_Exp*);struct Cyc_Tcpat_TcPatResult{struct _tuple1*tvars_and_effconstr_opt;struct Cyc_List_List*patvars;struct Cyc_List_List*aquals_bounds;};
# 58 "tcpat.h"
struct Cyc_Tcpat_TcPatResult Cyc_Tcpat_tcPat(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**,struct Cyc_Absyn_Exp*);
void Cyc_Tcpat_check_pat_regions(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,struct Cyc_Tcpat_TcPatResult*);
# 110
void Cyc_Tcpat_check_switch_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 112
int Cyc_Tcpat_check_let_pat_exhaustive(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Pat*,void**);
# 114
void Cyc_Tcpat_check_catch_overlap(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,void**);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*);
# 11 "subtype.h"
enum Cyc_Absyn_Coercion Cyc_Subtype_coercible_exact(unsigned,void*,void*);
# 42 "tcstmt.cyc"
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T3;enum Cyc_Absyn_Incrementor _T4;int _T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_T9;_T0=e;{
void*_TA=_T0->r;struct Cyc_Absyn_Exp*_TB;_T1=(int*)_TA;_T2=*_T1;if(_T2!=5)goto _TL0;_T3=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_TA;_T4=_T3->f2;_T5=(int)_T4;switch(_T5){case Cyc_Absyn_PostInc:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_TC=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_TA;_TB=_TC->f1;}{struct Cyc_Absyn_Exp*e1=_TB;_T6=e;{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_TC=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_TC->tag=5;
_TC->f1=e1;_TC->f2=0U;_T7=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_TC;}_T6->r=(void*)_T7;goto _LL0;}case Cyc_Absyn_PostDec:{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_TC=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_TA;_TB=_TC->f1;}{struct Cyc_Absyn_Exp*e1=_TB;_T8=e;{struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_TC=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_TC->tag=5;
_TC->f1=e1;_TC->f2=2U;_T9=(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_TC;}_T8->r=(void*)_T9;goto _LL0;}default: goto _LL5;}goto _TL1;_TL0: _LL5: goto _LL0;_TL1: _LL0:;}}
# 50
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var (void){struct _tuple2*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct Cyc_Int_pa_PrintArg_struct _T3;int _T4;int _T5;struct _fat_ptr _T6;struct _fat_ptr _T7;
static int counter=0;{struct _tuple2*_T8=_cycalloc(sizeof(struct _tuple2));
_T8->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_T9=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _TA;_TA.tag=1;_T4=counter;counter=_T4 + 1;_T5=_T4;_TA.f1=(unsigned)_T5;_T3=_TA;}{struct Cyc_Int_pa_PrintArg_struct _TA=_T3;void*_TB[1];_TB[0]=& _TA;_T6=_tag_fat("_tmpS%X",sizeof(char),8U);_T7=_tag_fat(_TB,sizeof(void*),1);_T2=Cyc_aprintf(_T6,_T7);}*_T9=_T2;_T1=(struct _fat_ptr*)_T9;}_T8->f1=_T1;_T0=(struct _tuple2*)_T8;}return _T0;}
# 57
static int Cyc_Tcstmt_make_var_exp(struct Cyc_Absyn_Exp**eptr,struct Cyc_Absyn_Stmt*s0){struct Cyc_Absyn_Exp**_T0;int _T1;struct Cyc_Absyn_Exp*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Exp**_T5;struct _tuple2*_T6;void*_T7;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T8;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T9;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_TA;void*_TB;struct Cyc_Absyn_Exp*_TC;unsigned _TD;void*_TE;struct Cyc_Absyn_Exp*_TF;unsigned _T10;struct Cyc_Absyn_Pat*_T11;struct Cyc_Absyn_Exp*_T12;struct Cyc_Absyn_Stmt*_T13;unsigned _T14;struct Cyc_Absyn_Stmt*_T15;struct Cyc_Absyn_Decl*_T16;struct Cyc_Absyn_Stmt*_T17;void*_T18;struct Cyc_Absyn_Stmt*_T19;unsigned _T1A;struct Cyc_Absyn_Stmt*_T1B;struct Cyc_Absyn_Stmt*_T1C;unsigned _T1D;struct Cyc_Absyn_Stmt*_T1E;struct Cyc_Absyn_Exp**_T1F;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T20;void*_T21;struct Cyc_Absyn_Exp*_T22;unsigned _T23;_T0=eptr;{
struct Cyc_Absyn_Exp*e2=*_T0;
int done=0;
_TL3: _T1=done;if(_T1)goto _TL5;else{goto _TL4;}
_TL4: _T2=e2;{void*_T24=_T2->r;struct Cyc_Absyn_Exp*_T25;_T3=(int*)_T24;_T4=*_T3;switch(_T4){case 1:
 return 0;case 12:{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T26=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_T24;_T25=_T26->f1;}{struct Cyc_Absyn_Exp*e3=_T25;_T25=e3;goto _LL6;}case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T26=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T24;_T25=_T26->f1;}_LL6: {struct Cyc_Absyn_Exp*e3=_T25;
# 64
e2=e3;goto _TL3;}default:
 done=1;};}goto _TL3;_TL5: _T5=eptr;{
# 67
struct Cyc_Absyn_Exp*e=*_T5;_T6=
Cyc_Tcstmt_stmt_temp_var();_T7=Cyc_Absyn_new_evar(0,0);{struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,_T6,_T7,0,0);{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T24=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T24->tag=1;
_T24->f1=vd;_T9=& Cyc_Absyn_Wild_p_val;_TA=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T9;_TB=(void*)_TA;_TC=e;_TD=_TC->loc;_T24->f2=Cyc_Absyn_new_pat(_TB,_TD);_T8=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T24;}_TE=(void*)_T8;_TF=e;_T10=_TF->loc;_T11=Cyc_Absyn_new_pat(_TE,_T10);_T12=e;_T13=s0;_T14=_T13->loc;{struct Cyc_Absyn_Decl*d=Cyc_Absyn_let_decl(_T11,_T12,_T14);_T15=s0;_T16=d;_T17=s0;_T18=_T17->r;_T19=s0;_T1A=_T19->loc;_T1B=
# 71
Cyc_Absyn_new_stmt(_T18,_T1A);_T1C=s0;_T1D=_T1C->loc;_T1E=Cyc_Absyn_decl_stmt(_T16,_T1B,_T1D);_T15->r=_T1E->r;_T1F=eptr;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T24=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T24->tag=4;
_T24->f1=vd;_T20=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T24;}_T21=(void*)_T20;_T22=e;_T23=_T22->loc;*_T1F=Cyc_Absyn_varb_exp(_T21,_T23);
return 1;}}}}}struct _tuple15{struct Cyc_Absyn_Tvar*f0;int f1;};struct _tuple16{struct Cyc_Absyn_Vardecl**f0;struct Cyc_Absyn_Exp*f1;};
# 77
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){struct Cyc_Tcpat_TcPatResult _T0;struct Cyc_Tcpat_TcPatResult _T1;struct Cyc_Tcpat_TcPatResult _T2;struct Cyc_List_List*(*_T3)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*);struct Cyc_List_List*(*_T4)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Vardecl**(*_T5)(struct _tuple16*);void*(*_T6)(struct _tuple0*);struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*(*_T9)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_TA)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_TB)(struct _tuple15*);void*(*_TC)(struct _tuple0*);struct _tuple1*_TD;struct _tuple1 _TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct _tuple1*_T11;struct _tuple1 _T12;struct Cyc_List_List*_T13;struct Cyc_List_List*(*_T14)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_T15)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_T16)(struct _tuple15*);void*(*_T17)(struct _tuple0*);struct Cyc_List_List*(*_T18)(int(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_T19)(int(*)(void*),struct Cyc_List_List*);int(*_T1A)(struct _tuple15*);void*(*_T1B)(struct _tuple0*);struct _tuple1*_T1C;struct _tuple1 _T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_Tcenv_Tenv*_T20;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T21;struct Cyc_List_List*_T22;void*_T23;void*_T24;struct Cyc_List_List*_T25;int _T26;struct Cyc_List_List*_T27;void*_T28;struct Cyc_Absyn_Vardecl**_T29;struct Cyc_List_List*_T2A;void*_T2B;struct Cyc_Absyn_Vardecl**_T2C;struct Cyc_Absyn_Vardecl*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;void*_T30;int*_T31;int _T32;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T33;void*_T34;int*_T35;int _T36;struct Cyc_List_List*_T37;struct Cyc_Tcenv_Tenv*_T38;struct Cyc_Absyn_Exp*_T39;struct _fat_ptr _T3A;int _T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;struct Cyc_Absyn_Exp*_T3D;unsigned _T3E;struct _fat_ptr _T3F;struct Cyc_List_List*_T40;struct Cyc_List_List*_T41;struct _tuple1*_T42;_T0=pat_res;_T42=_T0.tvars_and_effconstr_opt;_T1=pat_res;_T41=_T1.patvars;_T2=pat_res;_T40=_T2.aquals_bounds;{struct _tuple1*p=_T42;struct Cyc_List_List*vse=_T41;struct Cyc_List_List*aqb=_T40;_T4=Cyc_List_map;{
# 80
struct Cyc_List_List*(*_T43)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*))_T4;_T3=_T43;}_T6=Cyc_Core_fst;{struct Cyc_Absyn_Vardecl**(*_T43)(struct _tuple16*)=(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*))_T6;_T5=_T43;}_T7=vse;{struct Cyc_List_List*vs=_T3(_T5,_T7);
if(p!=0)goto _TL7;_T8=0;goto _TL8;_TL7: _TA=Cyc_List_map;{struct Cyc_List_List*(*_T43)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))_TA;_T9=_T43;}_TC=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T43)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))_TC;_TB=_T43;}_TD=p;_TE=*_TD;_TF=_TE.f0;_T8=_T9(_TB,_TF);_TL8: {struct Cyc_List_List*tvs=_T8;
if(p!=0)goto _TL9;_T10=0;goto _TLA;_TL9: _T11=p;_T12=*_T11;_T10=_T12.f1;_TLA: {struct Cyc_List_List*ecs=_T10;
if(p!=0)goto _TLB;_T13=0;goto _TLC;_TLB: _T15=Cyc_List_map;{struct Cyc_List_List*(*_T43)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))_T15;_T14=_T43;}_T17=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T43)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))_T17;_T16=_T43;}_T19=Cyc_List_filter;{struct Cyc_List_List*(*_T43)(int(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple15*),struct Cyc_List_List*))_T19;_T18=_T43;}_T1B=Cyc_Core_snd;{int(*_T43)(struct _tuple15*)=(int(*)(struct _tuple15*))_T1B;_T1A=_T43;}_T1C=p;_T1D=*_T1C;_T1E=_T1D.f0;_T1F=_T18(_T1A,_T1E);_T13=_T14(_T16,_T1F);_TLC: {struct Cyc_List_List*rgns=_T13;
te=Cyc_Tcenv_add_type_vars(loc,te,tvs);
_TL10: if(rgns!=0)goto _TLE;else{goto _TLF;}
_TLE: _T20=te;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T43=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T43->tag=2;_T22=rgns;_T23=_T22->hd;_T43->f1=(struct Cyc_Absyn_Tvar*)_T23;_T21=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T43;}_T24=(void*)_T21;te=Cyc_Tcenv_add_region(_T20,_T24,0,0);_T25=rgns;
# 85
rgns=_T25->tl;goto _TL10;_TLF:
# 87
 te=Cyc_Tcenv_new_effect_constraints(te,ecs,loc);_T26=new_block;
# 89
if(!_T26)goto _TL11;
te=Cyc_Tcenv_new_block(loc,te);goto _TL12;_TL11: _TL12: {
void*ropt=Cyc_Tcenv_curr_rgn(te);
_TL16: if(vs!=0)goto _TL14;else{goto _TL15;}
_TL14: _T27=vs;_T28=_T27->hd;_T29=(struct Cyc_Absyn_Vardecl**)_T28;if(_T29==0)goto _TL17;_T2A=vs;_T2B=_T2A->hd;_T2C=(struct Cyc_Absyn_Vardecl**)_T2B;_T2D=*_T2C;
_T2D->rgn=ropt;goto _TL18;_TL17: _TL18: _T2E=vs;
# 92
vs=_T2E->tl;goto _TL16;_TL15:
# 95
 _TL1C: if(aqb!=0)goto _TL1A;else{goto _TL1B;}
_TL1A: _T2F=aqb;_T30=_T2F->hd;{struct _tuple0*_T43=(struct _tuple0*)_T30;void*_T44;void*_T45;{struct _tuple0 _T46=*_T43;_T45=_T46.f0;_T44=_T46.f1;}{void*tv=_T45;void*b=_T44;
void*_T46=Cyc_Absyn_compress(tv);_T31=(int*)_T46;_T32=*_T31;if(_T32!=0)goto _TL1D;_T33=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T46;_T34=_T33->f1;_T35=(int*)_T34;_T36=*_T35;if(_T36!=15)goto _TL1F;
# 99
te=Cyc_Tcenv_add_aquals_bound(te,tv,b);goto _LL6;_TL1F: goto _LL9;_TL1D: _LL9: goto _LL6;_LL6:;}}_T37=aqb;
# 95
aqb=_T37->tl;goto _TL1C;_TL1B:
# 105
 if(where_opt==0)goto _TL21;_T38=te;_T39=where_opt;_T3A=
_tag_fat("switch clause guard",sizeof(char),20U);Cyc_Tcexp_tcTest(_T38,_T39,_T3A);_T3B=
Cyc_Absyn_no_side_effects_exp(where_opt);if(_T3B)goto _TL23;else{goto _TL25;}
_TL25:{struct Cyc_Warn_String_Warn_Warg_struct _T43;_T43.tag=0;_T43.f1=_tag_fat("cannot show &&-clause in pattern has no effects",sizeof(char),48U);_T3C=_T43;}{struct Cyc_Warn_String_Warn_Warg_struct _T43=_T3C;void*_T44[1];_T44[0]=& _T43;_T3D=where_opt;_T3E=_T3D->loc;_T3F=_tag_fat(_T44,sizeof(void*),1);Cyc_Warn_err2(_T3E,_T3F);}goto _TL24;_TL23: _TL24: goto _TL22;_TL21: _TL22:
# 110
 Cyc_Tcstmt_tcStmt(te,s,0);}}}}}}}
# 113
static int Cyc_Tcstmt_is_case_default(struct Cyc_Absyn_Switch_clause*c){struct Cyc_Absyn_Switch_clause*_T0;struct Cyc_Absyn_Pat*_T1;int*_T2;unsigned _T3;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T4;struct Cyc_Absyn_Pat*_T5;struct Cyc_Absyn_Pat*_T6;void*_T7;int*_T8;int _T9;_T0=c;_T1=_T0->pattern;{
void*_TA=_T1->r;_T2=(int*)_TA;_T3=*_T2;switch(_T3){case 0:
 return 1;case 1: _T4=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_TA;_T5=_T4->f2;_T6=(struct Cyc_Absyn_Pat*)_T5;_T7=_T6->r;_T8=(int*)_T7;_T9=*_T8;if(_T9!=0)goto _TL27;
return 1;_TL27: goto _LL5;default: _LL5:
 return 0;};}}
# 129 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;int _T3;int _T4;int _T5;int _T6;struct Cyc_Warn_String_Warn_Warg_struct _T7;struct Cyc_Warn_Typ_Warn_Warg_struct _T8;struct Cyc_Absyn_Stmt*_T9;unsigned _TA;struct _fat_ptr _TB;struct Cyc_Warn_String_Warn_Warg_struct _TC;struct Cyc_Warn_Typ_Warn_Warg_struct _TD;struct Cyc_Absyn_Stmt*_TE;unsigned _TF;struct _fat_ptr _T10;struct Cyc_Tcenv_Tenv*_T11;void**_T12;struct Cyc_Absyn_Exp*_T13;struct Cyc_List_List*_T14;struct Cyc_Absyn_Exp*_T15;void*_T16;int _T17;struct Cyc_Warn_String_Warn_Warg_struct _T18;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Warn_String_Warn_Warg_struct _T1B;struct Cyc_Warn_Typ_Warn_Warg_struct _T1C;struct Cyc_Absyn_Stmt*_T1D;unsigned _T1E;struct _fat_ptr _T1F;struct Cyc_Absyn_Exp*_T20;unsigned _T21;struct Cyc_List_List*_T22;void*_T23;struct Cyc_Absyn_Exp*_T24;struct Cyc_Tcenv_Tenv*_T25;struct Cyc_Absyn_Exp*_T26;struct _fat_ptr _T27;struct _tuple11 _T28;struct _tuple11 _T29;struct Cyc_Tcenv_Tenv*_T2A;struct Cyc_Absyn_Exp*_T2B;struct _fat_ptr _T2C;struct _tuple11 _T2D;struct _tuple11 _T2E;struct _tuple11 _T2F;struct _tuple11 _T30;struct Cyc_Tcenv_Tenv*_T31;struct Cyc_Absyn_Exp*_T32;struct _fat_ptr _T33;struct _tuple11 _T34;struct _tuple11 _T35;struct Cyc_Tcenv_Tenv*_T36;struct Cyc_Absyn_Exp*_T37;struct _fat_ptr _T38;struct Cyc_Absyn_Tvar*_T39;struct _fat_ptr*_T3A;struct _fat_ptr _T3B;struct Cyc_String_pa_PrintArg_struct _T3C;struct _fat_ptr*_T3D;struct _fat_ptr _T3E;struct _fat_ptr _T3F;struct Cyc_Absyn_Tvar*_T40;struct Cyc_Absyn_Tvar*_T41;struct Cyc_Absyn_Kind*_T42;struct Cyc_Absyn_Kind*_T43;struct Cyc_Absyn_Tvar*_T44;struct Cyc_Absyn_Stmt*_T45;unsigned _T46;struct Cyc_Tcenv_Tenv*_T47;struct Cyc_Absyn_Tvar*_T48;struct Cyc_Tcenv_Tenv*_T49;struct Cyc_Absyn_Stmt*_T4A;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T4B;struct Cyc_Absyn_Switch_clause***_T4C;struct Cyc_Warn_String_Warn_Warg_struct _T4D;struct Cyc_Absyn_Stmt*_T4E;unsigned _T4F;struct _fat_ptr _T50;const struct _tuple14*_T51;struct Cyc_List_List*(*_T52)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*(*_T53)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple13*(*_T54)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T55;struct Cyc_List_List*_T56;struct Cyc_List_List*(*_T57)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*(*_T58)(void*(*)(void*,void*),void*,struct Cyc_List_List*);void*(*_T59)(struct Cyc_List_List*,void*);struct Cyc_List_List*_T5A;struct Cyc_List_List*_T5B;struct Cyc_Tcenv_Tenv*_T5C;struct Cyc_List_List*_T5D;void*_T5E;struct Cyc_Absyn_Exp*_T5F;struct Cyc_List_List*_T60;struct Cyc_List_List*_T61;void*_T62;struct Cyc_Absyn_Exp*_T63;struct Cyc_List_List*_T64;void*_T65;int _T66;struct Cyc_Warn_String_Warn_Warg_struct _T67;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T68;struct Cyc_List_List*_T69;void*_T6A;struct Cyc_Absyn_Exp*_T6B;struct Cyc_Warn_String_Warn_Warg_struct _T6C;struct Cyc_Warn_Typ_Warn_Warg_struct _T6D;struct Cyc_List_List*_T6E;struct Cyc_Absyn_Stmt*_T6F;unsigned _T70;struct _fat_ptr _T71;struct Cyc_List_List*_T72;void*_T73;struct Cyc_Absyn_Exp*_T74;unsigned _T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;void*_T78;struct Cyc_List_List*_T79;void*_T7A;struct Cyc_Absyn_Exp*_T7B;struct Cyc_List_List*_T7C;struct Cyc_List_List*_T7D;struct Cyc_Warn_String_Warn_Warg_struct _T7E;struct Cyc_Absyn_Stmt*_T7F;unsigned _T80;struct _fat_ptr _T81;struct Cyc_Warn_String_Warn_Warg_struct _T82;struct Cyc_Absyn_Stmt*_T83;unsigned _T84;struct _fat_ptr _T85;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T86;struct Cyc_Absyn_Exp**_T87;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T88;void**_T89;int _T8A;struct Cyc_Absyn_Exp**_T8B;struct Cyc_Absyn_Exp*_T8C;unsigned _T8D;struct Cyc_List_List*_T8E;void*_T8F;struct Cyc_Absyn_Exp*_T90;int _T91;struct Cyc_Tcenv_Tenv*_T92;struct Cyc_List_List*_T93;void*_T94;struct Cyc_Absyn_Switch_clause*_T95;struct Cyc_Absyn_Pat*_T96;void**_T97;struct Cyc_Absyn_Exp*_T98;struct Cyc_Tcpat_TcPatResult*_T99;struct Cyc_List_List*_T9A;struct Cyc_List_List*_T9B;struct Cyc_Tcpat_TcPatResult*_T9C;struct Cyc_Tcenv_Tenv*_T9D;struct Cyc_List_List*_T9E;struct Cyc_List_List*_T9F;void*_TA0;struct Cyc_Absyn_Switch_clause*_TA1;struct Cyc_Absyn_Pat*_TA2;void**_TA3;struct Cyc_Absyn_Exp*_TA4;struct Cyc_Tcpat_TcPatResult*_TA5;struct _tuple1*_TA6;struct Cyc_Tcpat_TcPatResult*_TA7;struct _tuple1*_TA8;struct _tuple1 _TA9;struct Cyc_List_List*_TAA;struct Cyc_Tcpat_TcPatResult*_TAB;struct Cyc_List_List*_TAC;struct _tuple1 _TAD;struct Cyc_List_List*_TAE;struct Cyc_List_List*_TAF;struct Cyc_List_List*_TB0;struct Cyc_Tcpat_TcPatResult*_TB1;struct _tuple1*_TB2;struct Cyc_List_List*(*_TB3)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_TB4)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_TB5)(struct _tuple15*);void*(*_TB6)(struct _tuple0*);struct Cyc_Tcpat_TcPatResult*_TB7;struct _tuple1*_TB8;struct _tuple1 _TB9;struct Cyc_List_List*_TBA;struct Cyc_Tcenv_Tenv*_TBB;struct Cyc_List_List*_TBC;struct Cyc_List_List*_TBD;struct Cyc_List_List*_TBE;struct Cyc_List_List*_TBF;struct Cyc_List_List*_TC0;void*_TC1;struct Cyc_Absyn_Switch_clause*_TC2;struct Cyc_List_List*_TC3;void*_TC4;struct Cyc_Absyn_Switch_clause*_TC5;struct Cyc_List_List*_TC6;void*_TC7;struct Cyc_Absyn_Switch_clause*_TC8;unsigned _TC9;void*_TCA;struct Cyc_Absyn_Pat*_TCB;void*_TCC;void*_TCD;enum Cyc_Absyn_Coercion _TCE;int _TCF;struct Cyc_Warn_String_Warn_Warg_struct _TD0;struct Cyc_Warn_Typ_Warn_Warg_struct _TD1;struct Cyc_Warn_String_Warn_Warg_struct _TD2;struct Cyc_Warn_TypOpt_Warn_Warg_struct _TD3;struct Cyc_Absyn_Pat*_TD4;struct Cyc_List_List*_TD5;void*_TD6;struct Cyc_Absyn_Switch_clause*_TD7;unsigned _TD8;struct _fat_ptr _TD9;struct Cyc_Tcenv_Tenv*_TDA;struct Cyc_Absyn_Pat*_TDB;struct Cyc_Tcpat_TcPatResult*_TDC;struct Cyc_Tcpat_TcPatResult*_TDD;struct Cyc_List_List*_TDE;void*_TDF;struct Cyc_Absyn_Switch_clause*_TE0;struct Cyc_Core_Opt*_TE1;struct Cyc_Tcpat_TcPatResult _TE2;struct Cyc_List_List*_TE3;void*_TE4;struct Cyc_Absyn_Switch_clause*_TE5;unsigned _TE6;struct Cyc_Tcenv_Tenv*_TE7;struct Cyc_Tcpat_TcPatResult _TE8;struct Cyc_List_List*_TE9;void*_TEA;struct Cyc_Absyn_Switch_clause*_TEB;struct Cyc_Absyn_Stmt*_TEC;struct Cyc_List_List*_TED;void*_TEE;struct Cyc_Absyn_Switch_clause*_TEF;struct Cyc_Absyn_Exp*_TF0;struct Cyc_List_List*_TF1;struct Cyc_Absyn_Stmt*_TF2;unsigned _TF3;struct Cyc_Tcenv_Tenv*_TF4;struct Cyc_List_List*_TF5;void**_TF6;struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_TF7;struct Cyc_List_List**_TF8;struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_TF9;void**_TFA;struct Cyc_Absyn_Stmt*_TFB;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_TFC;struct Cyc_Absyn_Stmt*_TFD;void*_TFE;struct Cyc_Absyn_Stmt*_TFF;unsigned _T100;struct Cyc_Absyn_Stmt*_T101;unsigned _T102;void*_T103;struct Cyc_Absyn_Stmt*_T104;unsigned _T105;struct Cyc_Absyn_Stmt*_T106;int(*_T107)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*);int(*_T108)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List**_T109;struct Cyc_List_List*_T10A;int _T10B;struct _tuple2*_T10C;struct _tuple2*_T10D;struct _fat_ptr*_T10E;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T10F;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T110;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T111;void*_T112;void*_T113;struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T114;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T115;void*_T116;void*_T117;struct Cyc_Absyn_Exp*_T118;struct Cyc_Absyn_Switch_clause*_T119;struct Cyc_Absyn_Switch_clause*_T11A;struct Cyc_Absyn_Switch_clause*_T11B;struct Cyc_Absyn_Switch_clause*_T11C;struct Cyc_Absyn_Switch_clause*_T11D;struct Cyc_List_List**_T11E;struct Cyc_List_List**_T11F;struct Cyc_List_List*_T120;struct Cyc_List_List*_T121;struct _fat_ptr _T122;struct Cyc_List_List**_T123;int _T124;struct Cyc_Tcenv_Tenv*_T125;struct Cyc_List_List*_T126;void*_T127;struct Cyc_Absyn_Switch_clause*_T128;struct Cyc_Absyn_Pat*_T129;void**_T12A;struct Cyc_Tcpat_TcPatResult*_T12B;struct Cyc_List_List*_T12C;struct Cyc_List_List*_T12D;struct Cyc_Tcpat_TcPatResult*_T12E;struct Cyc_Tcenv_Tenv*_T12F;struct Cyc_List_List*_T130;struct Cyc_List_List*_T131;void*_T132;struct Cyc_Absyn_Switch_clause*_T133;struct Cyc_Absyn_Pat*_T134;void**_T135;struct Cyc_Tcpat_TcPatResult*_T136;struct _tuple1*_T137;struct Cyc_Tcpat_TcPatResult*_T138;struct _tuple1*_T139;struct _tuple1 _T13A;struct Cyc_List_List*_T13B;struct Cyc_Tcpat_TcPatResult*_T13C;struct Cyc_List_List*_T13D;struct _tuple1 _T13E;struct Cyc_List_List*_T13F;struct Cyc_List_List*_T140;struct Cyc_List_List*_T141;struct Cyc_Tcpat_TcPatResult*_T142;struct _tuple1*_T143;struct Cyc_List_List*(*_T144)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_T145)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_T146)(struct _tuple15*);void*(*_T147)(struct _tuple0*);struct Cyc_Tcpat_TcPatResult*_T148;struct _tuple1*_T149;struct _tuple1 _T14A;struct Cyc_List_List*_T14B;struct Cyc_Tcenv_Tenv*_T14C;struct Cyc_List_List*_T14D;struct Cyc_List_List*_T14E;struct Cyc_List_List*_T14F;struct Cyc_List_List*_T150;struct Cyc_List_List*_T151;void*_T152;struct Cyc_Absyn_Switch_clause*_T153;struct Cyc_List_List*_T154;void*_T155;struct Cyc_Absyn_Switch_clause*_T156;struct Cyc_List_List*_T157;void*_T158;struct Cyc_Absyn_Switch_clause*_T159;unsigned _T15A;void*_T15B;struct Cyc_Absyn_Pat*_T15C;void*_T15D;void*_T15E;enum Cyc_Absyn_Coercion _T15F;int _T160;struct Cyc_Warn_String_Warn_Warg_struct _T161;struct Cyc_Warn_Typ_Warn_Warg_struct _T162;struct Cyc_Warn_String_Warn_Warg_struct _T163;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T164;struct Cyc_Absyn_Pat*_T165;struct Cyc_List_List*_T166;void*_T167;struct Cyc_Absyn_Switch_clause*_T168;unsigned _T169;struct _fat_ptr _T16A;struct Cyc_Tcenv_Tenv*_T16B;struct Cyc_Absyn_Pat*_T16C;struct Cyc_Tcpat_TcPatResult*_T16D;struct Cyc_Tcpat_TcPatResult*_T16E;struct Cyc_List_List*_T16F;void*_T170;struct Cyc_Absyn_Switch_clause*_T171;struct Cyc_Core_Opt*_T172;struct Cyc_Tcpat_TcPatResult _T173;struct Cyc_List_List*_T174;void*_T175;struct Cyc_Absyn_Switch_clause*_T176;unsigned _T177;struct Cyc_Tcenv_Tenv*_T178;struct Cyc_Tcpat_TcPatResult _T179;struct Cyc_List_List*_T17A;void*_T17B;struct Cyc_Absyn_Switch_clause*_T17C;struct Cyc_Absyn_Stmt*_T17D;struct Cyc_List_List*_T17E;void*_T17F;struct Cyc_Absyn_Switch_clause*_T180;struct Cyc_Absyn_Exp*_T181;struct Cyc_List_List*_T182;struct Cyc_Absyn_Stmt*_T183;unsigned _T184;struct Cyc_Tcenv_Tenv*_T185;struct Cyc_List_List**_T186;struct Cyc_List_List*_T187;void**_T188;struct Cyc_Tcenv_Tenv*_T189;int _T18A;struct Cyc_Absyn_Stmt*_T18B;unsigned _T18C;struct Cyc_Tcenv_Tenv*_T18D;struct Cyc_Absyn_Decl*_T18E;int*_T18F;unsigned _T190;struct Cyc_Absyn_Vardecl*_T191;struct Cyc_Absyn_Vardecl*_T192;void**_T193;enum Cyc_Absyn_Scope _T194;int _T195;struct Cyc_Absyn_Vardecl*_T196;struct Cyc_Warn_String_Warn_Warg_struct _T197;struct Cyc_Absyn_Decl*_T198;unsigned _T199;struct _fat_ptr _T19A;void**_T19B;void*_T19C;int _T19D;int*_T19E;unsigned _T19F;int _T1A0;struct Cyc_Absyn_Vardecl*_T1A1;struct Cyc_Absyn_Vardecl*_T1A2;struct Cyc_Absyn_Exp*_T1A3;struct Cyc_Absyn_ArrayInfo _T1A4;struct Cyc_Absyn_Exp*_T1A5;struct Cyc_Absyn_Vardecl*_T1A6;struct Cyc_Absyn_Exp*_T1A7;int*_T1A8;unsigned _T1A9;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1AA;union Cyc_Absyn_Cnst _T1AB;struct _union_Cnst_Wstring_c _T1AC;unsigned _T1AD;union Cyc_Absyn_Cnst _T1AE;struct _union_Cnst_String_c _T1AF;struct _fat_ptr _T1B0;unsigned _T1B1;union Cyc_Absyn_Cnst _T1B2;struct _union_Cnst_Wstring_c _T1B3;int _T1B4;unsigned _T1B5;struct Cyc_Absyn_Vardecl*_T1B6;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T1B7;struct Cyc_List_List*_T1B8;int _T1B9;struct Cyc_Absyn_Stmt*_T1BA;unsigned _T1BB;struct Cyc_Tcenv_Tenv*_T1BC;struct Cyc_List_List*_T1BD;struct Cyc_Absyn_Kind*_T1BE;struct Cyc_Absyn_Kind*_T1BF;int _T1C0;void*_T1C1;struct Cyc_Absyn_Vardecl*_T1C2;struct Cyc_Absyn_Stmt*_T1C3;unsigned _T1C4;struct Cyc_Absyn_Vardecl*_T1C5;struct Cyc_Absyn_Tqual _T1C6;int _T1C7;void*_T1C8;enum Cyc_Absyn_Scope _T1C9;int _T1CA;enum Cyc_Absyn_Scope _T1CB;int _T1CC;struct Cyc_Warn_String_Warn_Warg_struct _T1CD;struct Cyc_Absyn_Decl*_T1CE;unsigned _T1CF;struct _fat_ptr _T1D0;struct Cyc_Tcenv_Tenv*_T1D1;void**_T1D2;struct Cyc_Absyn_Exp*_T1D3;int _T1D4;int _T1D5;struct Cyc_Warn_String_Warn_Warg_struct _T1D6;struct Cyc_Absyn_Decl*_T1D7;unsigned _T1D8;struct _fat_ptr _T1D9;struct Cyc_List_List*_T1DA;struct Cyc_Absyn_Exp*_T1DB;void*_T1DC;int _T1DD;struct Cyc_Warn_Vardecl_Warn_Warg_struct _T1DE;struct Cyc_Warn_String_Warn_Warg_struct _T1DF;struct Cyc_Warn_Typ_Warn_Warg_struct _T1E0;struct Cyc_Warn_String_Warn_Warg_struct _T1E1;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1E2;struct Cyc_Absyn_Exp*_T1E3;struct Cyc_Absyn_Decl*_T1E4;unsigned _T1E5;struct _fat_ptr _T1E6;void*_T1E7;struct Cyc_Absyn_Exp*_T1E8;void*_T1E9;void*_T1EA;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T1EB;struct Cyc_Core_Opt**_T1EC;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T1ED;struct Cyc_Absyn_Exp**_T1EE;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T1EF;void**_T1F0;struct Cyc_Absyn_Pat*_T1F1;int*_T1F2;unsigned _T1F3;int _T1F4;struct Cyc_Absyn_Exp**_T1F5;struct Cyc_Absyn_Exp*_T1F6;void*_T1F7;struct Cyc_Tcenv_Tenv*_T1F8;struct Cyc_Absyn_Pat*_T1F9;void**_T1FA;struct Cyc_Absyn_Exp*_T1FB;struct Cyc_Core_Opt**_T1FC;struct Cyc_Core_Opt*_T1FD;struct Cyc_Tcpat_TcPatResult _T1FE;struct Cyc_Absyn_Pat*_T1FF;void*_T200;struct Cyc_Absyn_Exp*_T201;void*_T202;int _T203;struct Cyc_List_List*_T204;struct Cyc_Absyn_Exp*_T205;void*_T206;int _T207;struct Cyc_Warn_String_Warn_Warg_struct _T208;struct Cyc_Warn_Typ_Warn_Warg_struct _T209;struct Cyc_Warn_String_Warn_Warg_struct _T20A;struct Cyc_Warn_Typ_Warn_Warg_struct _T20B;struct Cyc_Absyn_Decl*_T20C;unsigned _T20D;struct _fat_ptr _T20E;struct Cyc_Tcenv_Tenv*_T20F;struct Cyc_Absyn_Pat*_T210;struct Cyc_Tcpat_TcPatResult*_T211;struct Cyc_Tcpat_TcPatResult*_T212;struct Cyc_Absyn_Pat*_T213;unsigned _T214;struct Cyc_Tcenv_Tenv*_T215;struct Cyc_Absyn_Pat*_T216;void**_T217;struct Cyc_Absyn_Stmt*_T218;unsigned _T219;struct Cyc_Tcenv_Tenv*_T21A;struct Cyc_Tcpat_TcPatResult _T21B;struct Cyc_Absyn_Stmt*_T21C;struct Cyc_List_List*_T21D;void*_T21E;struct Cyc_Absyn_Vardecl*_T21F;struct Cyc_Absyn_Stmt*_T220;unsigned _T221;struct Cyc_Tcenv_Tenv*_T222;struct Cyc_List_List*_T223;struct Cyc_Absyn_Kind*_T224;struct Cyc_Absyn_Kind*_T225;struct Cyc_List_List*_T226;void*_T227;struct Cyc_Absyn_Vardecl*_T228;void*_T229;struct Cyc_List_List*_T22A;struct Cyc_Absyn_Stmt*_T22B;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T22C;struct Cyc_Absyn_Stmt*_T22D;void*_T22E;struct Cyc_Absyn_Stmt*_T22F;unsigned _T230;struct Cyc_Absyn_Stmt*_T231;unsigned _T232;void*_T233;struct Cyc_Absyn_Stmt*_T234;unsigned _T235;struct Cyc_Absyn_Stmt*_T236;struct Cyc_Absyn_Vardecl*_T237;struct Cyc_Absyn_Vardecl*_T238;void**_T239;struct Cyc_Absyn_Vardecl*_T23A;void**_T23B;void**_T23C;struct Cyc_Absyn_Exp*_T23D;unsigned _T23E;struct _tuple2*_T23F;struct Cyc_List_List*_T240;struct _fat_ptr*_T241;struct _fat_ptr _T242;struct _tuple2*_T243;struct _fat_ptr*_T244;struct Cyc_Core_Opt*_T245;struct Cyc_Absyn_Kind*_T246;struct Cyc_Core_Opt*_T247;struct Cyc_Absyn_Kind*_T248;struct Cyc_Core_Opt*_T249;struct Cyc_Absyn_Kind*_T24A;union Cyc_Absyn_AggrInfo _T24B;struct Cyc_List_List*_T24C;struct _fat_ptr _T24D;void*_T24E;void*_T24F;void*_T250;struct Cyc_Absyn_Tqual _T251;void*_T252;void*_T253;struct Cyc_Absyn_Stmt*_T254;unsigned _T255;struct Cyc_Tcenv_Tenv*_T256;struct Cyc_List_List*_T257;struct Cyc_Absyn_Kind*_T258;struct Cyc_Absyn_Kind*_T259;void*_T25A;struct Cyc_Tcenv_Tenv*_T25B;void**_T25C;struct Cyc_Absyn_Exp*_T25D;int _T25E;struct Cyc_List_List*_T25F;struct Cyc_Absyn_Exp*_T260;void*_T261;int _T262;struct Cyc_Warn_String_Warn_Warg_struct _T263;struct Cyc_Warn_Typ_Warn_Warg_struct _T264;struct Cyc_Warn_String_Warn_Warg_struct _T265;struct Cyc_Warn_Typ_Warn_Warg_struct _T266;struct Cyc_Absyn_Stmt*_T267;unsigned _T268;struct _fat_ptr _T269;int _T26A;struct Cyc_Warn_String_Warn_Warg_struct _T26B;struct Cyc_Absyn_Stmt*_T26C;unsigned _T26D;struct _fat_ptr _T26E;void**_T26F;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T270;struct Cyc_Absyn_Stmt*_T271;unsigned _T272;struct Cyc_Tcenv_Tenv*_T273;struct Cyc_List_List*_T274;struct Cyc_Absyn_Stmt*_T275;unsigned _T276;struct Cyc_Tcenv_Tenv*_T277;struct Cyc_List_List*_T278;struct Cyc_Absyn_Kind*_T279;struct Cyc_Absyn_Kind*_T27A;void**_T27B;void*_T27C;void**_T27D;void*_T27E;void*_T27F;int _T280;int(*_T281)(struct _fat_ptr,struct _fat_ptr);void*(*_T282)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T283;struct _fat_ptr _T284;struct Cyc_Absyn_Fndecl*_T285;enum Cyc_Absyn_Scope _T286;int _T287;struct Cyc_Warn_String_Warn_Warg_struct _T288;struct Cyc_Absyn_Decl*_T289;unsigned _T28A;struct _fat_ptr _T28B;struct Cyc_Absyn_Decl*_T28C;unsigned _T28D;struct Cyc_Absyn_Fndecl*_T28E;struct Cyc_Absyn_FnInfo _T28F;struct Cyc_List_List*_T290;struct Cyc_Absyn_Decl*_T291;unsigned _T292;struct Cyc_Tcenv_Tenv*_T293;struct Cyc_Absyn_Fndecl*_T294;struct Cyc_Absyn_Fndecl*_T295;void*_T296;struct Cyc_Absyn_Fndecl*_T297;struct Cyc_Absyn_FnInfo _T298;struct Cyc_List_List*_T299;struct Cyc_Absyn_Vardecl*_T29A;struct Cyc_Absyn_Fndecl*_T29B;struct Cyc_Absyn_Vardecl*_T29C;struct Cyc_Absyn_Fndecl*_T29D;struct Cyc_Absyn_Vardecl*_T29E;struct Cyc_Absyn_Vardecl*_T29F;struct Cyc_Absyn_Vardecl*_T2A0;void*_T2A1;void*_T2A2;void*_T2A3;struct Cyc_Absyn_Tqual _T2A4;void*_T2A5;void*_T2A6;struct Cyc_Absyn_Vardecl*_T2A7;struct Cyc_Absyn_Vardecl*_T2A8;struct Cyc_Absyn_Vardecl*_T2A9;struct Cyc_Absyn_Vardecl*_T2AA;struct Cyc_Absyn_Vardecl*_T2AB;struct Cyc_Absyn_Fndecl*_T2AC;struct Cyc_Absyn_Stmt*_T2AD;struct Cyc_Absyn_Vardecl*_T2AE;struct Cyc_Absyn_Fndecl*_T2AF;struct Cyc_Tcenv_Tenv*_T2B0;struct Cyc_Tcenv_Fenv*_T2B1;struct Cyc_Tcenv_Tenv*_T2B2;struct Cyc_Absyn_Decl*_T2B3;unsigned _T2B4;struct Cyc_Tcenv_Fenv*_T2B5;struct Cyc_Absyn_Fndecl*_T2B6;struct Cyc_Tcenv_Tenv*_T2B7;struct Cyc_Absyn_Fndecl*_T2B8;struct Cyc_Absyn_Stmt*_T2B9;struct Cyc_Tcenv_Tenv*_T2BA;struct Cyc_Warn_String_Warn_Warg_struct _T2BB;struct Cyc_Warn_String_Warn_Warg_struct _T2BC;struct Cyc_Warn_String_Warn_Warg_struct _T2BD;int(*_T2BE)(struct _fat_ptr);void*(*_T2BF)(struct _fat_ptr);struct _fat_ptr _T2C0;_T0=s0;{
# 131
void*_T2C1=_T0->r;struct Cyc_Absyn_Decl*_T2C2;struct Cyc_Absyn_Stmt*_T2C3;struct Cyc_Absyn_Stmt*_T2C4;struct Cyc_Absyn_Exp*_T2C5;void*_T2C6;void*_T2C7;void*_T2C8;_T1=(int*)_T2C1;_T2=*_T1;switch(_T2){case 0:
# 133
 return;case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C8=_T2C9->f1;}{struct Cyc_Absyn_Exp*e=_T2C8;
# 136
Cyc_Tcexp_tcExp(te,0,e);_T3=
Cyc_Tcenv_in_stmt_exp(te);if(_T3)goto _TL2A;else{goto _TL2C;}
_TL2C: Cyc_Tcstmt_simplify_unused_result_exp(e);goto _TL2B;_TL2A: _TL2B:
 return;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C8=_T2C9->f1;_T2C7=_T2C9->f2;}{struct Cyc_Absyn_Stmt*s1=_T2C8;struct Cyc_Absyn_Stmt*s2=_T2C7;
# 142
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C8=_T2C9->f1;}{struct Cyc_Absyn_Exp*eopt=_T2C8;
# 147
void*t=Cyc_Tcenv_return_typ(te);
if(eopt!=0)goto _TL2D;_T4=
Cyc_Tcutil_is_void_type(t);if(_T4)goto _TL2F;else{goto _TL31;}
_TL31: _T5=Cyc_Tcutil_is_any_float_type(t);if(_T5)goto _TL34;else{goto _TL35;}_TL35: _T6=Cyc_Tcutil_is_any_int_type(t);if(_T6)goto _TL34;else{goto _TL32;}
_TL34:{struct Cyc_Warn_String_Warn_Warg_struct _T2C9;_T2C9.tag=0;_T2C9.f1=_tag_fat("should return a value of type ",sizeof(char),31U);_T7=_T2C9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C9=_T7;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CA;_T2CA.tag=2;_T2CA.f1=t;_T8=_T2CA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CA=_T8;void*_T2CB[2];_T2CB[0]=& _T2C9;_T2CB[1]=& _T2CA;_T9=s0;_TA=_T9->loc;_TB=_tag_fat(_T2CB,sizeof(void*),2);Cyc_Warn_warn2(_TA,_TB);}}goto _TL33;
# 153
_TL32:{struct Cyc_Warn_String_Warn_Warg_struct _T2C9;_T2C9.tag=0;_T2C9.f1=_tag_fat("must return a value of type ",sizeof(char),29U);_TC=_T2C9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C9=_TC;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CA;_T2CA.tag=2;_T2CA.f1=t;_TD=_T2CA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CA=_TD;void*_T2CB[2];_T2CB[0]=& _T2C9;_T2CB[1]=& _T2CA;_TE=s0;_TF=_TE->loc;_T10=_tag_fat(_T2CB,sizeof(void*),2);Cyc_Warn_err2(_TF,_T10);}}_TL33: goto _TL30;_TL2F: _TL30:
# 155
 return;_TL2D: _T11=te;_T12=& t;_T13=eopt;
# 158
Cyc_Tcexp_tcExp(_T11,_T12,_T13);_T14=
Cyc_Tcenv_curr_aquals_bounds(te);_T15=eopt;_T16=t;_T17=Cyc_Tcutil_coerce_arg(_T14,_T15,_T16,0);if(_T17)goto _TL36;else{goto _TL38;}
_TL38:{struct Cyc_Warn_String_Warn_Warg_struct _T2C9;_T2C9.tag=0;_T2C9.f1=_tag_fat("returns value of type ",sizeof(char),23U);_T18=_T2C9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C9=_T18;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2CA;_T2CA.tag=3;_T1A=eopt;_T2CA.f1=_T1A->topt;_T19=_T2CA;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2CA=_T19;{struct Cyc_Warn_String_Warn_Warg_struct _T2CB;_T2CB.tag=0;_T2CB.f1=_tag_fat(" but requires ",sizeof(char),15U);_T1B=_T2CB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CB=_T1B;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CC;_T2CC.tag=2;_T2CC.f1=t;_T1C=_T2CC;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CC=_T1C;void*_T2CD[4];_T2CD[0]=& _T2C9;_T2CD[1]=& _T2CA;_T2CD[2]=& _T2CB;_T2CD[3]=& _T2CC;_T1D=s0;_T1E=_T1D->loc;_T1F=_tag_fat(_T2CD,sizeof(void*),4);Cyc_Warn_err2(_T1E,_T1F);}}}}
Cyc_Unify_explain_failure();goto _TL37;_TL36: _TL37: _T20=eopt;_T21=_T20->loc;_T22=
# 163
Cyc_Tcenv_curr_aquals_bounds(te);_T23=t;_T24=eopt;Cyc_Tcexp_check_consume(_T21,_T22,_T23,_T24);
return;}case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C8=_T2C9->f1;_T2C7=_T2C9->f2;_T2C6=_T2C9->f3;}{struct Cyc_Absyn_Exp*e=_T2C8;struct Cyc_Absyn_Stmt*s1=_T2C7;struct Cyc_Absyn_Stmt*s2=_T2C6;_T25=te;_T26=e;_T27=
# 167
_tag_fat("if statement",sizeof(char),13U);Cyc_Tcexp_tcTest(_T25,_T26,_T27);
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T2C1;_T28=_T2C9->f1;_T2C8=_T28.f0;_T29=_T2C9->f1;_T2C7=_T29.f1;_T2C6=_T2C9->f2;}{struct Cyc_Absyn_Exp*e=_T2C8;struct Cyc_Absyn_Stmt*cont_s=_T2C7;struct Cyc_Absyn_Stmt*s=_T2C6;_T2A=te;_T2B=e;_T2C=
# 173
_tag_fat("while loop",sizeof(char),11U);Cyc_Tcexp_tcTest(_T2A,_T2B,_T2C);
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C8=_T2C9->f1;_T2D=_T2C9->f2;_T2C7=_T2D.f0;_T2E=_T2C9->f2;_T2C6=_T2E.f1;_T2F=_T2C9->f3;_T2C5=_T2F.f0;_T30=_T2C9->f3;_T2C4=_T30.f1;_T2C3=_T2C9->f4;}{struct Cyc_Absyn_Exp*e1=_T2C8;struct Cyc_Absyn_Exp*e2=_T2C7;struct Cyc_Absyn_Stmt*guard_s=_T2C6;struct Cyc_Absyn_Exp*e3=_T2C5;struct Cyc_Absyn_Stmt*cont_s=_T2C4;struct Cyc_Absyn_Stmt*s=_T2C3;
# 178
Cyc_Tcexp_tcExp(te,0,e1);_T31=te;_T32=e2;_T33=
_tag_fat("for loop",sizeof(char),9U);Cyc_Tcexp_tcTest(_T31,_T32,_T33);
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C8=_T2C9->f1;_T34=_T2C9->f2;_T2C7=_T34.f0;_T35=_T2C9->f2;_T2C6=_T35.f1;}{struct Cyc_Absyn_Stmt*s=_T2C8;struct Cyc_Absyn_Exp*e=_T2C7;struct Cyc_Absyn_Stmt*cont_s=_T2C6;
# 186
Cyc_Tcstmt_tcStmt(te,s,1);_T36=te;_T37=e;_T38=
_tag_fat("do loop",sizeof(char),8U);Cyc_Tcexp_tcTest(_T36,_T37,_T38);
return;}case 6: goto _LL14;case 7: _LL14: goto _LL16;case 8: _LL16:
# 192
 return;case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C8=_T2C9->f1;_T2C7=_T2C9->f2;}{struct _fat_ptr*l=_T2C8;struct Cyc_Absyn_Stmt*s=_T2C7;
# 197
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T39=tv;{struct _fat_ptr*_T2C9=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T2CA;_T2CA.tag=0;_T3D=l;_T2CA.f1=*_T3D;_T3C=_T2CA;}{struct Cyc_String_pa_PrintArg_struct _T2CA=_T3C;void*_T2CB[1];_T2CB[0]=& _T2CA;_T3E=_tag_fat("`%s",sizeof(char),4U);_T3F=_tag_fat(_T2CB,sizeof(void*),1);_T3B=Cyc_aprintf(_T3E,_T3F);}*_T2C9=_T3B;_T3A=(struct _fat_ptr*)_T2C9;}_T39->name=_T3A;_T40=tv;
_T40->identity=Cyc_Tcutil_new_tvar_id();_T41=tv;_T42=& Cyc_Kinds_ek;_T43=(struct Cyc_Absyn_Kind*)_T42;_T41->kind=Cyc_Kinds_kind_to_bound(_T43);_T44=tv;_T44->aquals_bound=0;_T45=s0;_T46=_T45->loc;_T47=te;_T48=tv;_T49=
Cyc_Tcenv_new_named_block(_T46,_T47,_T48);_T4A=s;Cyc_Tcstmt_tcStmt(_T49,_T4A,0);
return;}case 11:{struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C8=_T2C9->f1;_T4B=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T2C1;_T4C=& _T4B->f2;_T2C7=(struct Cyc_Absyn_Switch_clause***)_T4C;}{struct Cyc_List_List*es=_T2C8;struct Cyc_Absyn_Switch_clause***clauseopt=_T2C7;
# 203
const struct _tuple14*trip_opt=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);
if(trip_opt!=0)goto _TL39;{struct Cyc_Warn_String_Warn_Warg_struct _T2C9;_T2C9.tag=0;
_T2C9.f1=_tag_fat("fallthru in a last case",sizeof(char),24U);_T4D=_T2C9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C9=_T4D;void*_T2CA[1];_T2CA[0]=& _T2C9;_T4E=s0;_T4F=_T4E->loc;_T50=_tag_fat(_T2CA,sizeof(void*),1);Cyc_Warn_err2(_T4F,_T50);}
return;_TL39: _T51=trip_opt;{
# 208
struct _tuple14 _T2C9=*_T51;struct Cyc_List_List*_T2CA;struct Cyc_List_List*_T2CB;_T2CB=_T2C9.f1;_T2CA=_T2C9.f2;{struct Cyc_List_List*tvs=_T2CB;struct Cyc_List_List*ts_orig=_T2CA;_T53=Cyc_List_map_c;{
struct Cyc_List_List*(*_T2CC)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*))_T53;_T52=_T2CC;}_T54=Cyc_Tcutil_make_inst_var;_T55=Cyc_Tcenv_lookup_type_vars(te);_T56=tvs;{struct Cyc_List_List*instantiation=_T52(_T54,_T55,_T56);_T58=Cyc_List_map_c;{
struct Cyc_List_List*(*_T2CC)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*))_T58;_T57=_T2CC;}_T59=Cyc_Tcutil_substitute;_T5A=instantiation;_T5B=ts_orig;{struct Cyc_List_List*ts=_T57(_T59,_T5A,_T5B);
_TL3E: if(ts!=0)goto _TL3F;else{goto _TL3D;}_TL3F: if(es!=0)goto _TL3C;else{goto _TL3D;}
# 213
_TL3C: _T5C=te;_T5D=es;_T5E=_T5D->hd;_T5F=(struct Cyc_Absyn_Exp*)_T5E;Cyc_Tcexp_tcExp(_T5C,0,_T5F);_T60=
Cyc_Tcenv_curr_aquals_bounds(te);_T61=es;_T62=_T61->hd;_T63=(struct Cyc_Absyn_Exp*)_T62;_T64=ts;_T65=_T64->hd;_T66=Cyc_Tcutil_coerce_arg(_T60,_T63,_T65,0);if(_T66)goto _TL40;else{goto _TL42;}
_TL42:{struct Cyc_Warn_String_Warn_Warg_struct _T2CC;_T2CC.tag=0;_T2CC.f1=_tag_fat("fallthru argument has type ",sizeof(char),28U);_T67=_T2CC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CC=_T67;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2CD;_T2CD.tag=3;_T69=es;_T6A=_T69->hd;_T6B=(struct Cyc_Absyn_Exp*)_T6A;_T2CD.f1=_T6B->topt;_T68=_T2CD;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2CD=_T68;{struct Cyc_Warn_String_Warn_Warg_struct _T2CE;_T2CE.tag=0;
_T2CE.f1=_tag_fat("but pattern variable has type ",sizeof(char),31U);_T6C=_T2CE;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CE=_T6C;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CF;_T2CF.tag=2;_T6E=ts;_T2CF.f1=_T6E->hd;_T6D=_T2CF;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CF=_T6D;void*_T2D0[4];_T2D0[0]=& _T2CC;_T2D0[1]=& _T2CD;_T2D0[2]=& _T2CE;_T2D0[3]=& _T2CF;_T6F=s0;_T70=_T6F->loc;_T71=_tag_fat(_T2D0,sizeof(void*),4);Cyc_Warn_err2(_T70,_T71);}}}}
Cyc_Unify_explain_failure();goto _TL41;_TL40: _TL41: _T72=es;_T73=_T72->hd;_T74=(struct Cyc_Absyn_Exp*)_T73;_T75=_T74->loc;_T76=
# 219
Cyc_Tcenv_curr_aquals_bounds(te);_T77=ts;_T78=_T77->hd;_T79=es;_T7A=_T79->hd;_T7B=(struct Cyc_Absyn_Exp*)_T7A;Cyc_Tcexp_check_consume(_T75,_T76,_T78,_T7B);_T7C=ts;
# 211
ts=_T7C->tl;_T7D=es;es=_T7D->tl;goto _TL3E;_TL3D:
# 221
 if(es==0)goto _TL43;{struct Cyc_Warn_String_Warn_Warg_struct _T2CC;_T2CC.tag=0;
_T2CC.f1=_tag_fat("too many arguments in explicit fallthru",sizeof(char),40U);_T7E=_T2CC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CC=_T7E;void*_T2CD[1];_T2CD[0]=& _T2CC;_T7F=s0;_T80=_T7F->loc;_T81=_tag_fat(_T2CD,sizeof(void*),1);Cyc_Warn_err2(_T80,_T81);}goto _TL44;_TL43: _TL44:
 if(ts==0)goto _TL45;{struct Cyc_Warn_String_Warn_Warg_struct _T2CC;_T2CC.tag=0;
_T2CC.f1=_tag_fat("too few arguments in explicit fallthru",sizeof(char),39U);_T82=_T2CC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CC=_T82;void*_T2CD[1];_T2CD[0]=& _T2CC;_T83=s0;_T84=_T83->loc;_T85=_tag_fat(_T2CD,sizeof(void*),1);Cyc_Warn_err2(_T84,_T85);}goto _TL46;_TL45: _TL46:
 return;}}}}}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T2C1;_T86=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T2C1;_T87=& _T86->f1;_T2C8=(struct Cyc_Absyn_Exp**)_T87;_T2C7=_T2C9->f2;_T88=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T2C1;_T89=& _T88->f3;_T2C6=(void**)_T89;}{struct Cyc_Absyn_Exp**exp=_T2C8;struct Cyc_List_List*scs0=_T2C7;void**dtp=(void**)_T2C6;_T8A=
# 229
Cyc_Tcstmt_make_var_exp(exp,s0);if(!_T8A)goto _TL47;
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;_TL47: _T8B=exp;{
# 234
struct Cyc_Absyn_Exp*e=*_T8B;
void*t=Cyc_Tcexp_tcExp(te,0,e);_T8C=e;_T8D=_T8C->loc;_T8E=
# 237
Cyc_Tcenv_curr_aquals_bounds(te);_T8F=t;_T90=e;Cyc_Tcexp_check_consume(_T8D,_T8E,_T8F,_T90);{
# 241
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;{
struct Cyc_List_List*scs=scs0;_TL4C: if(scs!=0)goto _TL4A;else{goto _TL4B;}
_TL4A: _T91=first_case;if(!_T91)goto _TL4D;
first_case=0;_T92=te;_T93=scs;_T94=_T93->hd;_T95=(struct Cyc_Absyn_Switch_clause*)_T94;_T96=_T95->pattern;_T97=& t;_T98=e;
this_pattern=Cyc_Tcpat_tcPat(_T92,_T96,_T97,_T98);goto _TL4E;
# 249
_TL4D: _T99=_check_null(next_pattern);this_pattern=*_T99;_TL4E: _T9A=scs;_T9B=_T9A->tl;
# 251
if(_T9B==0)goto _TL4F;{struct Cyc_Tcpat_TcPatResult*_T2C9=_cycalloc(sizeof(struct Cyc_Tcpat_TcPatResult));_T9D=te;_T9E=scs;_T9F=_T9E->tl;_TA0=_T9F->hd;_TA1=(struct Cyc_Absyn_Switch_clause*)_TA0;_TA2=_TA1->pattern;_TA3=& t;_TA4=e;
*_T2C9=Cyc_Tcpat_tcPat(_T9D,_TA2,_TA3,_TA4);_T9C=(struct Cyc_Tcpat_TcPatResult*)_T2C9;}next_pattern=_T9C;_TA5=next_pattern;_TA6=_TA5->tvars_and_effconstr_opt;
if(_TA6==0)goto _TL51;_TA7=next_pattern;_TA8=_TA7->tvars_and_effconstr_opt;_TA9=*_TA8;_TAA=_TA9.f1;if(_TAA==0)goto _TL51;
# 255
te=Cyc_Tcenv_clear_fallthru(te);goto _TL52;
# 257
_TL51: _TAB=next_pattern;_TAC=_TAB->patvars;_TAD=Cyc_List_split(_TAC);_TAE=_TAD.f0;_TAF=Cyc_Tcutil_filter_nulls(_TAE);{struct Cyc_List_List*vs=Cyc_List_imp_rev(_TAF);_TB1=next_pattern;_TB2=_TB1->tvars_and_effconstr_opt;
if(_TB2!=0)goto _TL53;_TB0=0;goto _TL54;_TL53: _TB4=Cyc_List_map;{
struct Cyc_List_List*(*_T2C9)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))_TB4;_TB3=_T2C9;}_TB6=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T2C9)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))_TB6;_TB5=_T2C9;}_TB7=next_pattern;_TB8=_TB7->tvars_and_effconstr_opt;_TB9=*_TB8;_TBA=_TB9.f0;_TB0=_TB3(_TB5,_TBA);_TL54: {
# 258
struct Cyc_List_List*new_tvs=_TB0;_TBB=te;_TBC=new_tvs;_TBD=vs;_TBE=scs;_TBF=_TBE->tl;_TC0=
# 260
_check_null(_TBF);_TC1=_TC0->hd;_TC2=(struct Cyc_Absyn_Switch_clause*)_TC1;te=Cyc_Tcenv_set_fallthru(_TBB,_TBC,_TBD,_TC2);}}_TL52: goto _TL50;
# 263
_TL4F: te=Cyc_Tcenv_clear_fallthru(te);_TL50: _TC3=scs;_TC4=_TC3->hd;_TC5=(struct Cyc_Absyn_Switch_clause*)_TC4;{
# 265
struct Cyc_Absyn_Pat*p=_TC5->pattern;_TC6=scs;_TC7=_TC6->hd;_TC8=(struct Cyc_Absyn_Switch_clause*)_TC7;_TC9=_TC8->loc;_TCA=t;_TCB=p;_TCC=_TCB->topt;_TCD=
_check_null(_TCC);{enum Cyc_Absyn_Coercion _c=Cyc_Subtype_coercible_exact(_TC9,_TCA,_TCD);_TCE=_c;_TCF=(int)_TCE;
if(_TCF!=0)goto _TL55;{struct Cyc_Warn_String_Warn_Warg_struct _T2C9;_T2C9.tag=0;
# 269
_T2C9.f1=_tag_fat("switch on type ",sizeof(char),16U);_TD0=_T2C9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C9=_TD0;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CA;_T2CA.tag=2;_T2CA.f1=t;_TD1=_T2CA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CA=_TD1;{struct Cyc_Warn_String_Warn_Warg_struct _T2CB;_T2CB.tag=0;_T2CB.f1=_tag_fat(" but case expects type ",sizeof(char),24U);_TD2=_T2CB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CB=_TD2;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2CC;_T2CC.tag=3;_TD4=p;_T2CC.f1=_TD4->topt;_TD3=_T2CC;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2CC=_TD3;void*_T2CD[4];_T2CD[0]=& _T2C9;_T2CD[1]=& _T2CA;_T2CD[2]=& _T2CB;_T2CD[3]=& _T2CC;_TD5=scs;_TD6=_TD5->hd;_TD7=(struct Cyc_Absyn_Switch_clause*)_TD6;_TD8=_TD7->loc;_TD9=_tag_fat(_T2CD,sizeof(void*),4);Cyc_Warn_err2(_TD8,_TD9);}}}}
Cyc_Unify_explain_failure();goto _TL56;
# 273
_TL55: _TDA=te;_TDB=p;_TDC=& this_pattern;_TDD=(struct Cyc_Tcpat_TcPatResult*)_TDC;Cyc_Tcpat_check_pat_regions(_TDA,_TDB,_TDD);_TL56: _TDE=scs;_TDF=_TDE->hd;_TE0=(struct Cyc_Absyn_Switch_clause*)_TDF;{struct Cyc_Core_Opt*_T2C9=_cycalloc(sizeof(struct Cyc_Core_Opt));_TE2=this_pattern;
# 275
_T2C9->v=_TE2.patvars;_TE1=(struct Cyc_Core_Opt*)_T2C9;}_TE0->pat_vars=_TE1;_TE3=scs;_TE4=_TE3->hd;_TE5=(struct Cyc_Absyn_Switch_clause*)_TE4;_TE6=_TE5->loc;_TE7=te;_TE8=this_pattern;_TE9=scs;_TEA=_TE9->hd;_TEB=(struct Cyc_Absyn_Switch_clause*)_TEA;_TEC=_TEB->body;_TED=scs;_TEE=_TED->hd;_TEF=(struct Cyc_Absyn_Switch_clause*)_TEE;_TF0=_TEF->where_clause;
Cyc_Tcstmt_pattern_synth(_TE6,_TE7,_TE8,_TEC,_TF0,1);}}_TF1=scs;
# 244
scs=_TF1->tl;goto _TL4C;_TL4B:;}_TF2=s0;_TF3=_TF2->loc;_TF4=te;_TF5=scs0;_TF6=dtp;
# 279
Cyc_Tcpat_check_switch_exhaustive(_TF3,_TF4,_TF5,_TF6);
return;}}}case 15:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C4=_T2C9->f1;_TF7=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T2C1;_TF8=& _TF7->f2;_T2C8=(struct Cyc_List_List**)_TF8;_TF9=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T2C1;_TFA=& _TF9->f3;_T2C7=(void**)_TFA;}{struct Cyc_Absyn_Stmt*s=_T2C4;struct Cyc_List_List**scs0=(struct Cyc_List_List**)_T2C8;void**dtp=(void**)_T2C7;_TFB=s;{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T2C9=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_T2C9->tag=2;_TFD=s;_TFE=_TFD->r;_TFF=s;_T100=_TFF->loc;
# 287
_T2C9->f1=Cyc_Absyn_new_stmt(_TFE,_T100);_T101=s;_T102=_T101->loc;_T2C9->f2=Cyc_Absyn_skip_stmt(_T102);_TFC=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T2C9;}_T103=(void*)_TFC;_T104=s;_T105=_T104->loc;_T106=Cyc_Absyn_new_stmt(_T103,_T105);_TFB->r=_T106->r;
# 289
Cyc_Tcstmt_tcStmt(te,s,1);_T108=Cyc_List_exists;{
# 291
int(*_T2C9)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))_T108;_T107=_T2C9;}_T109=scs0;_T10A=*_T109;_T10B=_T107(Cyc_Tcstmt_is_case_default,_T10A);if(_T10B)goto _TL57;else{goto _TL59;}
# 293
_TL59:{struct _tuple2*def_v;def_v=_cycalloc(sizeof(struct _tuple2));_T10C=def_v;_T10C->f0=Cyc_Absyn_Loc_n();_T10D=def_v;{struct _fat_ptr*_T2C9=_cycalloc(sizeof(struct _fat_ptr));*_T2C9=_tag_fat("exn",sizeof(char),4U);_T10E=(struct _fat_ptr*)_T2C9;}_T10D->f1=_T10E;{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0,0);{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T2C9=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T2C9->tag=1;
_T2C9->f1=vd;_T110=& Cyc_Absyn_Wild_p_val;_T111=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T110;_T112=(void*)_T111;_T2C9->f2=Cyc_Absyn_new_pat(_T112,0U);_T10F=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T2C9;}_T113=(void*)_T10F;{struct Cyc_Absyn_Pat*def_pat=Cyc_Absyn_new_pat(_T113,0U);{struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T2C9=_cycalloc(sizeof(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct));_T2C9->tag=11;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T2CA=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T2CA->tag=4;
# 297
_T2CA->f1=vd;_T115=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T2CA;}_T116=(void*)_T115;_T2C9->f1=Cyc_Absyn_varb_exp(_T116,0U);_T2C9->f2=1;_T114=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_T2C9;}_T117=(void*)_T114;_T118=Cyc_Absyn_new_exp(_T117,0U);{
# 296
struct Cyc_Absyn_Stmt*def_stmt=
Cyc_Absyn_exp_stmt(_T118,0U);
struct Cyc_Absyn_Switch_clause*def_clause;def_clause=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_T119=def_clause;_T119->pattern=def_pat;_T11A=def_clause;_T11A->pat_vars=0;_T11B=def_clause;_T11B->where_clause=0;_T11C=def_clause;_T11C->body=def_stmt;_T11D=def_clause;_T11D->loc=0U;_T11E=scs0;_T11F=scs0;_T120=*_T11F;{struct Cyc_Absyn_Switch_clause*_T2C9[1];_T2C9[0]=def_clause;_T122=_tag_fat(_T2C9,sizeof(struct Cyc_Absyn_Switch_clause*),1);_T121=Cyc_List_list(_T122);}
*_T11E=Cyc_List_append(_T120,_T121);}}}}goto _TL58;_TL57: _TL58: {
# 305
void*exception_type=Cyc_Absyn_exn_type();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;_T123=scs0;{
struct Cyc_List_List*scs=*_T123;_TL5D: if(scs!=0)goto _TL5B;else{goto _TL5C;}
_TL5B: _T124=first_case;if(!_T124)goto _TL5E;
first_case=0;_T125=te;_T126=scs;_T127=_T126->hd;_T128=(struct Cyc_Absyn_Switch_clause*)_T127;_T129=_T128->pattern;_T12A=& exception_type;
this_pattern=Cyc_Tcpat_tcPat(_T125,_T129,_T12A,0);goto _TL5F;
# 314
_TL5E: _T12B=_check_null(next_pattern);this_pattern=*_T12B;_TL5F: _T12C=scs;_T12D=_T12C->tl;
# 316
if(_T12D==0)goto _TL60;{struct Cyc_Tcpat_TcPatResult*_T2C9=_cycalloc(sizeof(struct Cyc_Tcpat_TcPatResult));_T12F=te;_T130=scs;_T131=_T130->tl;_T132=_T131->hd;_T133=(struct Cyc_Absyn_Switch_clause*)_T132;_T134=_T133->pattern;_T135=& exception_type;
*_T2C9=Cyc_Tcpat_tcPat(_T12F,_T134,_T135,0);_T12E=(struct Cyc_Tcpat_TcPatResult*)_T2C9;}next_pattern=_T12E;_T136=next_pattern;_T137=_T136->tvars_and_effconstr_opt;
if(_T137==0)goto _TL62;_T138=next_pattern;_T139=_T138->tvars_and_effconstr_opt;_T13A=*_T139;_T13B=_T13A.f1;if(_T13B==0)goto _TL62;
# 320
te=Cyc_Tcenv_clear_fallthru(te);goto _TL63;
# 322
_TL62: _T13C=next_pattern;_T13D=_T13C->patvars;_T13E=Cyc_List_split(_T13D);_T13F=_T13E.f0;_T140=Cyc_Tcutil_filter_nulls(_T13F);{struct Cyc_List_List*vs=Cyc_List_imp_rev(_T140);_T142=next_pattern;_T143=_T142->tvars_and_effconstr_opt;
if(_T143!=0)goto _TL64;_T141=0;goto _TL65;_TL64: _T145=Cyc_List_map;{
struct Cyc_List_List*(*_T2C9)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))_T145;_T144=_T2C9;}_T147=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T2C9)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))_T147;_T146=_T2C9;}_T148=next_pattern;_T149=_T148->tvars_and_effconstr_opt;_T14A=*_T149;_T14B=_T14A.f0;_T141=_T144(_T146,_T14B);_TL65: {
# 323
struct Cyc_List_List*new_tvs=_T141;_T14C=te;_T14D=new_tvs;_T14E=vs;_T14F=scs;_T150=_T14F->tl;_T151=
# 325
_check_null(_T150);_T152=_T151->hd;_T153=(struct Cyc_Absyn_Switch_clause*)_T152;te=Cyc_Tcenv_set_fallthru(_T14C,_T14D,_T14E,_T153);}}_TL63: goto _TL61;
# 328
_TL60: te=Cyc_Tcenv_clear_fallthru(te);_TL61: _T154=scs;_T155=_T154->hd;_T156=(struct Cyc_Absyn_Switch_clause*)_T155;{
# 330
struct Cyc_Absyn_Pat*p=_T156->pattern;_T157=scs;_T158=_T157->hd;_T159=(struct Cyc_Absyn_Switch_clause*)_T158;_T15A=_T159->loc;_T15B=exception_type;_T15C=p;_T15D=_T15C->topt;_T15E=
_check_null(_T15D);{enum Cyc_Absyn_Coercion _c=Cyc_Subtype_coercible_exact(_T15A,_T15B,_T15E);_T15F=_c;_T160=(int)_T15F;
if(_T160!=0)goto _TL66;{struct Cyc_Warn_String_Warn_Warg_struct _T2C9;_T2C9.tag=0;
_T2C9.f1=_tag_fat("switch on type ",sizeof(char),16U);_T161=_T2C9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2C9=_T161;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CA;_T2CA.tag=2;_T2CA.f1=exception_type;_T162=_T2CA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2CA=_T162;{struct Cyc_Warn_String_Warn_Warg_struct _T2CB;_T2CB.tag=0;
_T2CB.f1=_tag_fat(" but case expects type ",sizeof(char),24U);_T163=_T2CB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CB=_T163;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2CC;_T2CC.tag=3;_T165=p;_T2CC.f1=_T165->topt;_T164=_T2CC;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2CC=_T164;void*_T2CD[4];_T2CD[0]=& _T2C9;_T2CD[1]=& _T2CA;_T2CD[2]=& _T2CB;_T2CD[3]=& _T2CC;_T166=scs;_T167=_T166->hd;_T168=(struct Cyc_Absyn_Switch_clause*)_T167;_T169=_T168->loc;_T16A=_tag_fat(_T2CD,sizeof(void*),4);Cyc_Warn_err2(_T169,_T16A);}}}}
Cyc_Unify_explain_failure();goto _TL67;
# 338
_TL66: _T16B=te;_T16C=p;_T16D=& this_pattern;_T16E=(struct Cyc_Tcpat_TcPatResult*)_T16D;Cyc_Tcpat_check_pat_regions(_T16B,_T16C,_T16E);_TL67: _T16F=scs;_T170=_T16F->hd;_T171=(struct Cyc_Absyn_Switch_clause*)_T170;{struct Cyc_Core_Opt*_T2C9=_cycalloc(sizeof(struct Cyc_Core_Opt));_T173=this_pattern;
# 340
_T2C9->v=_T173.patvars;_T172=(struct Cyc_Core_Opt*)_T2C9;}_T171->pat_vars=_T172;_T174=scs;_T175=_T174->hd;_T176=(struct Cyc_Absyn_Switch_clause*)_T175;_T177=_T176->loc;_T178=te;_T179=this_pattern;_T17A=scs;_T17B=_T17A->hd;_T17C=(struct Cyc_Absyn_Switch_clause*)_T17B;_T17D=_T17C->body;_T17E=scs;_T17F=_T17E->hd;_T180=(struct Cyc_Absyn_Switch_clause*)_T17F;_T181=_T180->where_clause;
Cyc_Tcstmt_pattern_synth(_T177,_T178,_T179,_T17D,_T181,1);}}_T182=scs;
# 309
scs=_T182->tl;goto _TL5D;_TL5C:;}_T183=s0;_T184=_T183->loc;_T185=te;_T186=scs0;_T187=*_T186;_T188=dtp;
# 344
Cyc_Tcpat_check_catch_overlap(_T184,_T185,_T187,_T188);
return;}}default:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T2C9=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T2C1;_T2C2=_T2C9->f1;_T2C4=_T2C9->f2;}{struct Cyc_Absyn_Decl*d=_T2C2;struct Cyc_Absyn_Stmt*s=_T2C4;
# 348
struct _fat_ptr unimp_msg_part;_T18A=new_block;
if(!_T18A)goto _TL68;_T18B=s0;_T18C=_T18B->loc;_T18D=te;_T189=Cyc_Tcenv_new_block(_T18C,_T18D);goto _TL69;_TL68: _T189=te;_TL69: {struct Cyc_Tcenv_Tenv*_T2C9=_T189;struct Cyc_Tcenv_Tenv*te=_T2C9;_T18E=d;{
void*_T2CA=_T18E->r;struct _tuple2*_T2CB;struct _fat_ptr*_T2CC;struct Cyc_Absyn_Fndecl*_T2CD;struct Cyc_Absyn_Exp*_T2CE;struct Cyc_Absyn_Vardecl*_T2CF;struct Cyc_Absyn_Tvar*_T2D0;struct Cyc_List_List*_T2D1;void*_T2D2;void*_T2D3;struct Cyc_Absyn_Pat*_T2D4;void*_T2D5;_T18F=(int*)_T2CA;_T190=*_T18F;switch(_T190){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T2D6=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2CA;_T2D5=_T2D6->f1;}{struct Cyc_Absyn_Vardecl*vd=_T2D5;void*_T2D6;struct Cyc_Absyn_Exp*_T2D7;void*_T2D8;struct Cyc_Absyn_Tqual _T2D9;enum Cyc_Absyn_Scope _T2DA;_T191=vd;{struct Cyc_Absyn_Vardecl _T2DB=*_T191;_T2DA=_T2DB.sc;_T2D9=_T2DB.tq;_T2D8=_T2DB.type;_T2D7=_T2DB.initializer;_T192=vd;_T193=& _T192->rgn;_T2D6=(void**)_T193;}{enum Cyc_Absyn_Scope sc=_T2DA;struct Cyc_Absyn_Tqual tq=_T2D9;void*t=_T2D8;struct Cyc_Absyn_Exp*initializer=_T2D7;void**rgn_ptr=(void**)_T2D6;
# 353
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
int is_local;_T194=sc;_T195=(int)_T194;switch(_T195){case Cyc_Absyn_Static: goto _LL4E;case Cyc_Absyn_Extern: _LL4E: goto _LL50;case Cyc_Absyn_ExternC: _LL50: _T196=vd;
# 358
_T196->escapes=1;is_local=0;goto _LL4A;case Cyc_Absyn_Abstract:{struct Cyc_Warn_String_Warn_Warg_struct _T2DB;_T2DB.tag=0;
# 360
_T2DB.f1=_tag_fat("bad abstract scope for local variable",sizeof(char),38U);_T197=_T2DB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2DB=_T197;void*_T2DC[1];_T2DC[0]=& _T2DB;_T198=d;_T199=_T198->loc;_T19A=_tag_fat(_T2DC,sizeof(void*),1);Cyc_Warn_err2(_T199,_T19A);}goto _LL54;case Cyc_Absyn_Register: _LL54: goto _LL56;case Cyc_Absyn_Public: _LL56: goto _LL58;default: _LL58:
# 364
 is_local=1;goto _LL4A;}_LL4A: _T19B=rgn_ptr;_T19D=is_local;
# 367
if(!_T19D)goto _TL6C;_T19C=curr_bl;goto _TL6D;_TL6C: _T19C=Cyc_Absyn_heap_rgn_type;_TL6D:*_T19B=_T19C;{
void*_T2DB=Cyc_Absyn_compress(t);struct Cyc_Absyn_ArrayInfo _T2DC;_T19E=(int*)_T2DB;_T19F=*_T19E;switch(_T19F){case 6: _T1A0=is_local;if(!_T1A0)goto _TL6F;_T1A1=vd;
# 371
_T1A1->escapes=1;
sc=3U;
is_local=0;goto _LL59;_TL6F: goto _LL5E;case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T2DD=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T2DB;_T2DC=_T2DD->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T2DC;_T1A2=vd;_T1A3=_T1A2->initializer;
# 377
if(_T1A3==0)goto _TL73;else{goto _TL74;}_TL74: _T1A4=ai;_T1A5=_T1A4.num_elts;if(_T1A5!=0)goto _TL73;else{goto _TL71;}
_TL73: goto _LL59;_TL71: {
struct Cyc_Absyn_Exp*len;_T1A6=vd;_T1A7=_T1A6->initializer;{
void*_T2DD=_T1A7->r;struct Cyc_List_List*_T2DE;struct Cyc_Absyn_Exp*_T2DF;struct _fat_ptr _T2E0;_T1A8=(int*)_T2DD;_T1A9=*_T1A8;switch(_T1A9){case 0: _T1AA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T2DD;_T1AB=_T1AA->f1;_T1AC=_T1AB.Wstring_c;_T1AD=_T1AC.tag;switch(_T1AD){case 8:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T2E1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T2DD;_T1AE=_T2E1->f1;_T1AF=_T1AE.String_c;_T2E0=_T1AF.val;}{struct _fat_ptr s=_T2E0;_T1B0=s;_T1B1=
_get_fat_size(_T1B0,sizeof(char));len=Cyc_Absyn_uint_exp(_T1B1,0U);goto _LL60;}case 9:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T2E1=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T2DD;_T1B2=_T2E1->f1;_T1B3=_T1B2.Wstring_c;_T2E0=_T1B3.val;}{struct _fat_ptr s=_T2E0;
len=Cyc_Absyn_uint_exp(1U,0U);goto _LL60;}default: goto _LL6D;};case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T2E1=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T2DD;_T2DF=_T2E1->f2;}{struct Cyc_Absyn_Exp*e=_T2DF;_T2DF=e;goto _LL68;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T2E1=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T2DD;_T2DF=_T2E1->f1;}_LL68: {struct Cyc_Absyn_Exp*e=_T2DF;
# 384
len=e;goto _LL60;}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T2E1=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T2DD;_T2DE=_T2E1->f2;}{struct Cyc_List_List*es=_T2DE;_T2DE=es;goto _LL6C;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T2E1=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T2DD;_T2DE=_T2E1->f1;}_LL6C: {struct Cyc_List_List*es=_T2DE;_T1B4=
# 386
Cyc_List_length(es);_T1B5=(unsigned)_T1B4;len=Cyc_Absyn_uint_exp(_T1B5,0U);goto _LL60;}default: _LL6D:
 len=0;goto _LL60;}_LL60:;}
# 389
ai.num_elts=len;_T1B6=vd;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T2DD=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_T2DD->tag=5;
_T2DD->f1=ai;_T1B7=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T2DD;}_T1B6->type=(void*)_T1B7;t=_T1B6->type;goto _LL59;}}default: _LL5E: goto _LL59;}_LL59:;}_T1B9=is_local;
# 397
if(_T1B9)goto _TL77;else{goto _TL79;}_TL79: _T1B8=0;goto _TL78;_TL77: _T1B8=Cyc_Tcenv_lookup_type_vars(te);_TL78: {struct Cyc_List_List*bound_vars=_T1B8;
int allow_evars=is_local;_T1BA=s0;_T1BB=_T1BA->loc;_T1BC=te;_T1BD=bound_vars;_T1BE=& Cyc_Kinds_mk;_T1BF=(struct Cyc_Absyn_Kind*)_T1BE;_T1C0=allow_evars;_T1C1=t;
Cyc_Tctyp_check_type(_T1BB,_T1BC,_T1BD,_T1BF,_T1C0,1,_T1C1);_T1C2=vd;_T1C3=s0;_T1C4=_T1C3->loc;_T1C5=vd;_T1C6=_T1C5->tq;_T1C7=_T1C6.print_const;_T1C8=t;
_T1C2->tq.real_const=Cyc_Tcutil_extract_const_from_typedef(_T1C4,_T1C7,_T1C8);_T1C9=sc;_T1CA=(int)_T1C9;
# 402
if(_T1CA==3)goto _TL7C;else{goto _TL7D;}_TL7D: _T1CB=sc;_T1CC=(int)_T1CB;if(_T1CC==4)goto _TL7C;else{goto _TL7A;}
_TL7C:{struct Cyc_Warn_String_Warn_Warg_struct _T2DB;_T2DB.tag=0;_T2DB.f1=_tag_fat("extern declarations are not yet supported within functions",sizeof(char),59U);_T1CD=_T2DB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2DB=_T1CD;void*_T2DC[1];_T2DC[0]=& _T2DB;_T1CE=d;_T1CF=_T1CE->loc;_T1D0=_tag_fat(_T2DC,sizeof(void*),1);Cyc_Warn_err2(_T1CF,_T1D0);}goto _TL7B;_TL7A: _TL7B:
 if(initializer==0)goto _TL7E;_T1D1=te;_T1D2=& t;_T1D3=initializer;
Cyc_Tcexp_tcExpInitializer(_T1D1,_T1D2,_T1D3);_T1D4=is_local;
# 407
if(_T1D4)goto _TL80;else{goto _TL82;}_TL82: _T1D5=Cyc_Tcutil_is_const_exp(initializer);if(_T1D5)goto _TL80;else{goto _TL83;}
_TL83:{struct Cyc_Warn_String_Warn_Warg_struct _T2DB;_T2DB.tag=0;_T2DB.f1=_tag_fat("initializer for static variable needs to be a constant expression",sizeof(char),66U);_T1D6=_T2DB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2DB=_T1D6;void*_T2DC[1];_T2DC[0]=& _T2DB;_T1D7=d;_T1D8=_T1D7->loc;_T1D9=_tag_fat(_T2DC,sizeof(void*),1);Cyc_Warn_err2(_T1D8,_T1D9);}goto _TL81;_TL80: _TL81: _T1DA=
Cyc_Tcenv_curr_aquals_bounds(te);_T1DB=initializer;_T1DC=t;_T1DD=Cyc_Tcutil_coerce_assign(_T1DA,_T1DB,_T1DC);if(_T1DD)goto _TL84;else{goto _TL86;}
_TL86:{struct Cyc_Warn_Vardecl_Warn_Warg_struct _T2DB;_T2DB.tag=11;_T2DB.f1=vd;_T1DE=_T2DB;}{struct Cyc_Warn_Vardecl_Warn_Warg_struct _T2DB=_T1DE;{struct Cyc_Warn_String_Warn_Warg_struct _T2DC;_T2DC.tag=0;_T2DC.f1=_tag_fat(" was declared with type ",sizeof(char),25U);_T1DF=_T2DC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2DC=_T1DF;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2DD;_T2DD.tag=2;_T2DD.f1=t;_T1E0=_T2DD;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2DD=_T1E0;{struct Cyc_Warn_String_Warn_Warg_struct _T2DE;_T2DE.tag=0;
_T2DE.f1=_tag_fat(" but initialized with type ",sizeof(char),28U);_T1E1=_T2DE;}{struct Cyc_Warn_String_Warn_Warg_struct _T2DE=_T1E1;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2DF;_T2DF.tag=3;_T1E3=initializer;_T2DF.f1=_T1E3->topt;_T1E2=_T2DF;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2DF=_T1E2;void*_T2E0[5];_T2E0[0]=& _T2DB;_T2E0[1]=& _T2DC;_T2E0[2]=& _T2DD;_T2E0[3]=& _T2DE;_T2E0[4]=& _T2DF;_T1E4=d;_T1E5=_T1E4->loc;_T1E6=_tag_fat(_T2E0,sizeof(void*),5);Cyc_Warn_err2(_T1E5,_T1E6);}}}}}_T1E7=t;_T1E8=initializer;_T1E9=_T1E8->topt;_T1EA=
_check_null(_T1E9);Cyc_Unify_unify(_T1E7,_T1EA);
Cyc_Unify_explain_failure();goto _TL85;_TL84: _TL85: goto _TL7F;_TL7E: _TL7F:
# 416
 Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T2D6=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T2CA;_T2D4=_T2D6->f1;_T1EB=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T2CA;_T1EC=& _T1EB->f2;_T2D5=(struct Cyc_Core_Opt**)_T1EC;_T1ED=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T2CA;_T1EE=& _T1ED->f3;_T2D3=(struct Cyc_Absyn_Exp**)_T1EE;_T1EF=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T2CA;_T1F0=& _T1EF->f4;_T2D2=(void**)_T1F0;}{struct Cyc_Absyn_Pat*p=_T2D4;struct Cyc_Core_Opt**vds=(struct Cyc_Core_Opt**)_T2D5;struct Cyc_Absyn_Exp**exp=(struct Cyc_Absyn_Exp**)_T2D3;void**dtp=(void**)_T2D2;_T1F1=p;{
# 421
void*_T2D6=_T1F1->r;_T1F2=(int*)_T2D6;_T1F3=*_T1F2;switch(_T1F3){case 1: goto _LL73;case 2: _LL73: goto _LL75;case 14: _LL75: goto _LL6F;default: _T1F4=
# 426
Cyc_Tcstmt_make_var_exp(exp,s0);if(!_T1F4)goto _TL88;
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;_TL88:;}_LL6F:;}_T1F5=exp;{
# 432
struct Cyc_Absyn_Exp*e=*_T1F5;
Cyc_Tcexp_tcExpInitializer(te,0,e);_T1F6=e;_T1F7=_T1F6->topt;{
# 435
void*pat_type=_check_null(_T1F7);_T1F8=te;_T1F9=p;_T1FA=& pat_type;_T1FB=e;{
# 437
struct Cyc_Tcpat_TcPatResult pat_res=Cyc_Tcpat_tcPat(_T1F8,_T1F9,_T1FA,_T1FB);_T1FC=vds;{struct Cyc_Core_Opt*_T2D6=_cycalloc(sizeof(struct Cyc_Core_Opt));_T1FE=pat_res;
_T2D6->v=_T1FE.patvars;_T1FD=(struct Cyc_Core_Opt*)_T2D6;}*_T1FC=_T1FD;_T1FF=p;_T200=_T1FF->topt;{
void*pt=_check_null(_T200);_T201=e;_T202=_T201->topt;{
void*et=_check_null(_T202);_T203=
Cyc_Unify_unify(pt,et);if(_T203)goto _TL8A;else{goto _TL8C;}_TL8C: _T204=Cyc_Tcenv_curr_aquals_bounds(te);_T205=e;_T206=pt;_T207=Cyc_Tcutil_coerce_assign(_T204,_T205,_T206);if(_T207)goto _TL8A;else{goto _TL8D;}
_TL8D:{struct Cyc_Warn_String_Warn_Warg_struct _T2D6;_T2D6.tag=0;
_T2D6.f1=_tag_fat("pattern type ",sizeof(char),14U);_T208=_T2D6;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D6=_T208;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2D7;_T2D7.tag=2;_T2D7.f1=pt;_T209=_T2D7;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2D7=_T209;{struct Cyc_Warn_String_Warn_Warg_struct _T2D8;_T2D8.tag=0;_T2D8.f1=_tag_fat(" does not match definition type ",sizeof(char),33U);_T20A=_T2D8;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D8=_T20A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2D9;_T2D9.tag=2;_T2D9.f1=et;_T20B=_T2D9;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2D9=_T20B;void*_T2DA[4];_T2DA[0]=& _T2D6;_T2DA[1]=& _T2D7;_T2DA[2]=& _T2D8;_T2DA[3]=& _T2D9;_T20C=d;_T20D=_T20C->loc;_T20E=_tag_fat(_T2DA,sizeof(void*),4);Cyc_Warn_err2(_T20D,_T20E);}}}}
Cyc_Unify_unify(pt,et);
Cyc_Unify_explain_failure();goto _TL8B;
# 448
_TL8A: _T20F=te;_T210=p;_T211=& pat_res;_T212=(struct Cyc_Tcpat_TcPatResult*)_T211;Cyc_Tcpat_check_pat_regions(_T20F,_T210,_T212);_TL8B: _T213=p;_T214=_T213->loc;_T215=te;_T216=p;_T217=dtp;
Cyc_Tcpat_check_let_pat_exhaustive(_T214,_T215,_T216,_T217);_T218=s0;_T219=_T218->loc;_T21A=te;_T21B=pat_res;_T21C=s;
Cyc_Tcstmt_pattern_synth(_T219,_T21A,_T21B,_T21C,0,0);
return;}}}}}}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T2D6=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T2CA;_T2D1=_T2D6->f1;}{struct Cyc_List_List*vds=_T2D1;
# 454
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
_TL91: if(vds!=0)goto _TL8F;else{goto _TL90;}
_TL8F: _T21D=vds;_T21E=_T21D->hd;_T21F=(struct Cyc_Absyn_Vardecl*)_T21E;_T21F->rgn=curr_bl;_T220=s0;_T221=_T220->loc;_T222=te;_T223=
Cyc_Tcenv_lookup_type_vars(te);_T224=& Cyc_Kinds_mk;_T225=(struct Cyc_Absyn_Kind*)_T224;_T226=vds;_T227=_T226->hd;_T228=(struct Cyc_Absyn_Vardecl*)_T227;_T229=_T228->type;Cyc_Tctyp_check_type(_T221,_T222,_T223,_T225,1,1,_T229);_T22A=vds;
# 455
vds=_T22A->tl;goto _TL91;_TL90:
# 460
 Cyc_Tcstmt_tcStmt(te,s,0);
return;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T2D6=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T2CA;_T2D0=_T2D6->f1;_T2CF=_T2D6->f2;_T2CE=_T2D6->f3;}{struct Cyc_Absyn_Tvar*tv=_T2D0;struct Cyc_Absyn_Vardecl*vd=_T2CF;struct Cyc_Absyn_Exp*open_exp_opt=_T2CE;_T22B=s;{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T2D6=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_T2D6->tag=2;_T22D=s;_T22E=_T22D->r;_T22F=s;_T230=_T22F->loc;
# 466
_T2D6->f1=Cyc_Absyn_new_stmt(_T22E,_T230);_T231=s;_T232=_T231->loc;_T2D6->f2=Cyc_Absyn_skip_stmt(_T232);_T22C=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T2D6;}_T233=(void*)_T22C;_T234=s;_T235=_T234->loc;_T236=Cyc_Absyn_new_stmt(_T233,_T235);_T22B->r=_T236->r;{
# 468
struct Cyc_Tcenv_Tenv*te2=te;void*_T2D6;void*_T2D7;_T237=vd;{struct Cyc_Absyn_Vardecl _T2D8=*_T237;_T238=vd;_T239=& _T238->type;_T2D7=(void**)_T239;_T23A=vd;_T23B=& _T23A->rgn;_T2D6=(void**)_T23B;}{void**t=(void**)_T2D7;void**rgn_ptr=(void**)_T2D6;
# 470
void*curr_bl=Cyc_Tcenv_curr_rgn(te);_T23C=rgn_ptr;
*_T23C=curr_bl;{
void*rgn_typ;_T23D=open_exp_opt;_T23E=(unsigned)_T23D;
if(!_T23E)goto _TL92;{
# 476
struct _tuple2*drname;drname=_cycalloc(sizeof(struct _tuple2));_T23F=drname;{struct _fat_ptr*_T2D8[1];{struct _fat_ptr*_T2D9=_cycalloc(sizeof(struct _fat_ptr));*_T2D9=_tag_fat("Core",sizeof(char),5U);_T241=(struct _fat_ptr*)_T2D9;}_T2D8[0]=_T241;_T242=_tag_fat(_T2D8,sizeof(struct _fat_ptr*),1);_T240=Cyc_List_list(_T242);}_T23F->f0=Cyc_Absyn_Abs_n(_T240,0);_T243=drname;{struct _fat_ptr*_T2D8=_cycalloc(sizeof(struct _fat_ptr));*_T2D8=_tag_fat("DynamicRegion",sizeof(char),14U);_T244=(struct _fat_ptr*)_T2D8;}_T243->f1=_T244;{struct Cyc_Core_Opt*_T2D8=_cycalloc(sizeof(struct Cyc_Core_Opt));_T246=& Cyc_Kinds_ek;
_T2D8->v=(struct Cyc_Absyn_Kind*)_T246;_T245=(struct Cyc_Core_Opt*)_T2D8;}{void*outer_rgn=Cyc_Absyn_new_evar(_T245,0);{struct Cyc_Core_Opt*_T2D8=_cycalloc(sizeof(struct Cyc_Core_Opt));_T248=& Cyc_Kinds_ek;
_T2D8->v=(struct Cyc_Absyn_Kind*)_T248;_T247=(struct Cyc_Core_Opt*)_T2D8;}rgn_typ=Cyc_Absyn_new_evar(_T247,0);{struct Cyc_Core_Opt*_T2D8=_cycalloc(sizeof(struct Cyc_Core_Opt));_T24A=& Cyc_Kinds_aqk;
_T2D8->v=(struct Cyc_Absyn_Kind*)_T24A;_T249=(struct Cyc_Core_Opt*)_T2D8;}{void*aq=Cyc_Absyn_new_evar(_T249,0);_T24B=
Cyc_Absyn_UnknownAggr(0U,drname,0);{void*_T2D8[1];_T2D8[0]=rgn_typ;_T24D=_tag_fat(_T2D8,sizeof(void*),1);_T24C=Cyc_List_list(_T24D);}{void*dr_type=Cyc_Absyn_aggr_type(_T24B,_T24C);_T24E=dr_type;_T24F=outer_rgn;_T250=aq;_T251=
Cyc_Absyn_empty_tqual(0U);_T252=Cyc_Absyn_false_type;_T253=Cyc_Absyn_false_type;{void*exp_type=Cyc_Absyn_at_type(_T24E,_T24F,_T250,_T251,_T252,_T253);_T254=s0;_T255=_T254->loc;_T256=te;_T257=
Cyc_Tcenv_lookup_type_vars(te);_T258=& Cyc_Kinds_mk;_T259=(struct Cyc_Absyn_Kind*)_T258;_T25A=exp_type;Cyc_Tctyp_check_type(_T255,_T256,_T257,_T259,1,0,_T25A);_T25B=te;_T25C=& exp_type;_T25D=open_exp_opt;{
# 484
void*key_typ=Cyc_Tcexp_tcExp(_T25B,_T25C,_T25D);_T25E=
Cyc_Unify_unify(exp_type,key_typ);if(_T25E)goto _TL94;else{goto _TL96;}_TL96: _T25F=
Cyc_Tcenv_curr_aquals_bounds(te);_T260=open_exp_opt;_T261=exp_type;_T262=Cyc_Tcutil_coerce_assign(_T25F,_T260,_T261);
# 485
if(_T262)goto _TL94;else{goto _TL97;}
# 487
_TL97:{struct Cyc_Warn_String_Warn_Warg_struct _T2D8;_T2D8.tag=0;_T2D8.f1=_tag_fat("expected ",sizeof(char),10U);_T263=_T2D8;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D8=_T263;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2D9;_T2D9.tag=2;_T2D9.f1=exp_type;_T264=_T2D9;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2D9=_T264;{struct Cyc_Warn_String_Warn_Warg_struct _T2DA;_T2DA.tag=0;_T2DA.f1=_tag_fat(" but found ",sizeof(char),12U);_T265=_T2DA;}{struct Cyc_Warn_String_Warn_Warg_struct _T2DA=_T265;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2DB;_T2DB.tag=2;_T2DB.f1=key_typ;_T266=_T2DB;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2DB=_T266;void*_T2DC[4];_T2DC[0]=& _T2D8;_T2DC[1]=& _T2D9;_T2DC[2]=& _T2DA;_T2DC[3]=& _T2DB;_T267=s0;_T268=_T267->loc;_T269=_tag_fat(_T2DC,sizeof(void*),4);Cyc_Warn_err2(_T268,_T269);}}}}goto _TL95;_TL94: _TL95: _T26A=
# 490
Cyc_Tcutil_is_aliasable_qual(aq);if(!_T26A)goto _TL98;{struct Cyc_Warn_String_Warn_Warg_struct _T2D8;_T2D8.tag=0;
_T2D8.f1=_tag_fat("open is allowed only on unique or reference-counted keys",sizeof(char),57U);_T26B=_T2D8;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D8=_T26B;void*_T2D9[1];_T2D9[0]=& _T2D8;_T26C=s0;_T26D=_T26C->loc;_T26E=_tag_fat(_T2D9,sizeof(void*),1);Cyc_Warn_err2(_T26D,_T26E);}goto _TL99;_TL98: _TL99: _T26F=t;
*_T26F=Cyc_Absyn_rgn_handle_type(rgn_typ);
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1,0);}}}}}}goto _TL93;
# 495
_TL92:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T2D8=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T2D8->tag=2;_T2D8->f1=tv;_T270=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T2D8;}rgn_typ=(void*)_T270;_T271=s0;_T272=_T271->loc;_T273=te2;{struct Cyc_List_List*_T2D8=_cycalloc(sizeof(struct Cyc_List_List));
_T2D8->hd=tv;_T2D8->tl=0;_T274=(struct Cyc_List_List*)_T2D8;}te2=Cyc_Tcenv_add_type_vars(_T272,_T273,_T274);
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0,1);_TL93: _T275=s0;_T276=_T275->loc;_T277=te;_T278=
# 499
Cyc_Tcenv_lookup_type_vars(te2);_T279=& Cyc_Kinds_bk;_T27A=(struct Cyc_Absyn_Kind*)_T279;_T27B=t;_T27C=*_T27B;Cyc_Tctyp_check_type(_T276,_T277,_T278,_T27A,1,0,_T27C);_T27D=t;_T27E=*_T27D;_T27F=
Cyc_Absyn_rgn_handle_type(rgn_typ);_T280=Cyc_Unify_unify(_T27E,_T27F);if(_T280)goto _TL9A;else{goto _TL9C;}
_TL9C: _T282=Cyc_Warn_impos;{int(*_T2D8)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T282;_T281=_T2D8;}_T283=_tag_fat("region stmt: type of region handle is wrong!",sizeof(char),45U);_T284=_tag_fat(0U,sizeof(void*),0);_T281(_T283,_T284);goto _TL9B;_TL9A: _TL9B:
 Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T2D6=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T2CA;_T2CD=_T2D6->f1;}{struct Cyc_Absyn_Fndecl*fd=_T2CD;_T285=fd;_T286=_T285->sc;_T287=(int)_T286;
# 506
if(_T287==2)goto _TL9D;{struct Cyc_Warn_String_Warn_Warg_struct _T2D6;_T2D6.tag=0;
_T2D6.f1=_tag_fat("bad storage class for inner function",sizeof(char),37U);_T288=_T2D6;}{struct Cyc_Warn_String_Warn_Warg_struct _T2D6=_T288;void*_T2D7[1];_T2D7[0]=& _T2D6;_T289=d;_T28A=_T289->loc;_T28B=_tag_fat(_T2D7,sizeof(void*),1);Cyc_Warn_err2(_T28A,_T28B);}goto _TL9E;_TL9D: _TL9E: _T28C=d;_T28D=_T28C->loc;_T28E=fd;_T28F=_T28E->i;_T290=_T28F.attributes;
Cyc_Atts_check_fndecl_atts(_T28D,_T290,1);_T291=d;_T292=_T291->loc;_T293=te;_T294=fd;
Cyc_Tctyp_check_fndecl_valid_type(_T292,_T293,_T294);{
void*t=Cyc_Tcutil_fndecl2type(fd);_T295=fd;_T296=t;_T297=fd;_T298=_T297->i;_T299=_T298.attributes;
_T295->i.attributes=Cyc_Atts_transfer_fn_type_atts(_T296,_T299);{
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
struct Cyc_Absyn_Vardecl*vd;vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T29A=vd;_T29B=fd;_T29A->sc=_T29B->sc;_T29C=vd;_T29D=fd;_T29C->name=_T29D->name;_T29E=vd;_T29E->varloc=0U;_T29F=vd;_T29F->tq=Cyc_Absyn_const_tqual(0U);_T2A0=vd;_T2A1=t;_T2A2=curr_bl;_T2A3=Cyc_Absyn_al_qual_type;_T2A4=
Cyc_Absyn_empty_tqual(0U);_T2A5=Cyc_Absyn_false_type;_T2A6=Cyc_Absyn_false_type;_T2A0->type=Cyc_Absyn_at_type(_T2A1,_T2A2,_T2A3,_T2A4,_T2A5,_T2A6);_T2A7=vd;
# 516
_T2A7->initializer=0;_T2A8=vd;_T2A8->rgn=curr_bl;_T2A9=vd;_T2A9->attributes=0;_T2AA=vd;_T2AA->escapes=0;_T2AB=vd;_T2AC=fd;_T2AD=_T2AC->body;
_T2AB->is_proto=_T2AD!=0;_T2AE=vd;
_T2AE->rename=0;_T2AF=fd;
_T2AF->fn_vardecl=vd;_T2B0=te;_T2B1=_T2B0->le;{
struct Cyc_Tcenv_Fenv*old_fenv=_check_null(_T2B1);_T2B2=te;_T2B3=d;_T2B4=_T2B3->loc;_T2B5=old_fenv;_T2B6=fd;
_T2B2->le=Cyc_Tcenv_nested_fenv(_T2B4,_T2B5,_T2B6);_T2B7=te;_T2B8=fd;_T2B9=_T2B8->body;
Cyc_Tcstmt_tcStmt(_T2B7,_T2B9,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);_T2BA=te;
_T2BA->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T2D6=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T2CA;_T2CC=_T2D6->f1;_T2D1=_T2D6->f2;}{struct _fat_ptr*n=_T2CC;struct Cyc_List_List*tds=_T2D1;
unimp_msg_part=_tag_fat("namespace",sizeof(char),10U);goto _LL24;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T2D6=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T2CA;_T2CB=_T2D6->f1;_T2D1=_T2D6->f2;}{struct _tuple2*q=_T2CB;struct Cyc_List_List*tds=_T2D1;
unimp_msg_part=_tag_fat("using",sizeof(char),6U);goto _LL24;}case 5:
 unimp_msg_part=_tag_fat("type",sizeof(char),5U);goto _LL24;case 6:
 unimp_msg_part=_tag_fat("datatype",sizeof(char),9U);goto _LL24;case 7:
 unimp_msg_part=_tag_fat("enum",sizeof(char),5U);goto _LL24;case 8:
 unimp_msg_part=_tag_fat("typedef",sizeof(char),8U);goto _LL24;case 11:
 unimp_msg_part=_tag_fat("extern \"C\"",sizeof(char),11U);goto _LL24;case 12:
 unimp_msg_part=_tag_fat("extern \"C include\"",sizeof(char),19U);goto _LL24;case 13:
 unimp_msg_part=_tag_fat("__cyclone_port_on__",sizeof(char),20U);goto _LL24;case 14:
 unimp_msg_part=_tag_fat("__cyclone_port_off__",sizeof(char),21U);goto _LL24;case 15:
 unimp_msg_part=_tag_fat("__tempest_on__",sizeof(char),15U);goto _LL24;default:
 unimp_msg_part=_tag_fat("__tempest_off__",sizeof(char),16U);goto _LL24;}_LL24:;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CA;_T2CA.tag=0;
# 541
_T2CA.f1=_tag_fat("tcStmt: nested ",sizeof(char),16U);_T2BB=_T2CA;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CA=_T2BB;{struct Cyc_Warn_String_Warn_Warg_struct _T2CB;_T2CB.tag=0;_T2CB.f1=unimp_msg_part;_T2BC=_T2CB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CB=_T2BC;{struct Cyc_Warn_String_Warn_Warg_struct _T2CC;_T2CC.tag=0;_T2CC.f1=_tag_fat(" declarations not allowed",sizeof(char),26U);_T2BD=_T2CC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2CC=_T2BD;void*_T2CD[3];_T2CD[0]=& _T2CA;_T2CD[1]=& _T2CB;_T2CD[2]=& _T2CC;_T2BF=Cyc_Warn_impos2;{int(*_T2CE)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T2BF;_T2BE=_T2CE;}_T2C0=_tag_fat(_T2CD,sizeof(void*),3);_T2BE(_T2C0);}}}}}};}}
# 552 "tcstmt.cyc"
static int Cyc_Tcstmt_check_catchall_handler(struct Cyc_Absyn_Stmt*b){struct Cyc_Absyn_Stmt*_T0;int*_T1;int _T2;struct Cyc_List_List*_T3;unsigned _T4;struct Cyc_List_List*_T5;void*_T6;struct Cyc_Absyn_Switch_clause*_T7;struct Cyc_Absyn_Pat*_T8;int*_T9;int _TA;struct Cyc_Absyn_Switch_clause*_TB;struct Cyc_Absyn_Stmt*_TC;int _TD;struct Cyc_List_List*_TE;int _TF;_T0=b;{
void*_T10=_T0->r;struct Cyc_List_List*_T11;_T1=(int*)_T10;_T2=*_T1;if(_T2!=15)goto _TL9F;{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T12=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T10;_T11=_T12->f2;}{struct Cyc_List_List*case_list=_T11;
# 555
int found_wild=0;
_TLA4: _T3=case_list;_T4=(unsigned)_T3;if(_T4)goto _TLA2;else{goto _TLA3;}
_TLA2: _T5=case_list;_T6=_T5->hd;{struct Cyc_Absyn_Switch_clause*clause=(struct Cyc_Absyn_Switch_clause*)_T6;_T7=clause;_T8=_T7->pattern;{
void*_T12=_T8->r;_T9=(int*)_T12;_TA=*_T9;if(_TA!=0)goto _TLA5;
found_wild=1;goto _LL5;_TLA5: goto _LL5;_LL5:;}_TB=clause;_TC=_TB->body;_TD=
# 562
Cyc_Tcstmt_ensure_no_throw_stmt(_TC);if(_TD)goto _TLA7;else{goto _TLA9;}
_TLA9: return 0;_TLA7:;}_TE=case_list;
# 556
case_list=_TE->tl;goto _TLA4;_TLA3: _TF=found_wild;
# 565
return _TF;}_TL9F:
 return 0;;}}struct _tuple17{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 569
static int Cyc_Tcstmt_ensure_no_throw_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;int(*_T3)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*);int(*_T4)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T5;int _T6;int _T7;int _T8;int _T9;struct Cyc_Absyn_Exp*_TA;int*_TB;unsigned _TC;void*_TD;int _TE;struct Cyc_Absyn_Exp*_TF;int*_T10;int _T11;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T12;struct Cyc_Absyn_PtrInfo _T13;void*_T14;int*_T15;int _T16;struct Cyc_Absyn_PtrInfo _T17;void*_T18;int(*_T19)(void*,void*);struct Cyc_Absyn_FnInfo _T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_T1C;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_T1D;void*_T1E;struct Cyc_Warn_String_Warn_Warg_struct _T1F;int(*_T20)(struct _fat_ptr);void*(*_T21)(struct _fat_ptr);struct _fat_ptr _T22;int _T23;int _T24;int(*_T25)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*);int(*_T26)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T27;void*_T28;void*_T29;int*_T2A;unsigned _T2B;int _T2C;int(*_T2D)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*);int(*_T2E)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*(*_T2F)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*);struct Cyc_List_List*(*_T30)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Exp*(*_T31)(struct _tuple17*);void*(*_T32)(struct _tuple0*);struct Cyc_List_List*_T33;struct Cyc_List_List*_T34;int _T35;void*_T36;int _T37;int _T38;struct Cyc_Absyn_Exp*_T39;int*_T3A;int _T3B;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3C;struct Cyc_Absyn_PtrInfo*_T3D;struct Cyc_Absyn_PtrInfo*_T3E;struct Cyc_Absyn_PtrAtts _T3F;void*_T40;unsigned _T41;int _T42;_T0=e;{
void*_T43=_T0->r;enum Cyc_Absyn_Coercion _T44;int _T45;struct Cyc_Absyn_VarargCallInfo*_T46;struct Cyc_List_List*_T47;struct Cyc_Absyn_Exp*_T48;void*_T49;_T1=(int*)_T43;_T2=*_T1;switch(_T2){case 0:
 return 1;case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T43;_T49=_T4A->f2;}{struct Cyc_List_List*es=_T49;_T4=Cyc_List_forall;{
# 573
int(*_T4A)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))_T4;_T3=_T4A;}_T5=es;_T6=_T3(Cyc_Tcstmt_ensure_no_throw_exp,_T5);return _T6;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T43;_T49=_T4A->f1;_T48=_T4A->f2;}{struct Cyc_Absyn_Exp*e1=_T49;struct Cyc_Absyn_Exp*e2=_T48;_T49=e1;_T48=e2;goto _LL8;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T43;_T49=_T4A->f1;_T48=_T4A->f2;}_LL8: {struct Cyc_Absyn_Exp*e1=_T49;struct Cyc_Absyn_Exp*e2=_T48;_T49=e1;_T48=e2;goto _LLA;}case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_T43;_T49=_T4A->f1;_T48=_T4A->f2;}_LLA: {struct Cyc_Absyn_Exp*e1=_T49;struct Cyc_Absyn_Exp*e2=_T48;_T49=e1;_T48=e2;goto _LLC;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T43;_T49=_T4A->f1;_T48=_T4A->f2;}_LLC: {struct Cyc_Absyn_Exp*e1=_T49;struct Cyc_Absyn_Exp*e2=_T48;_T8=
# 577
Cyc_Tcstmt_ensure_no_throw_exp(e1);if(!_T8)goto _TLAB;_T7=Cyc_Tcstmt_ensure_no_throw_exp(e2);goto _TLAC;_TLAB: _T7=0;_TLAC: return _T7;}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T43;_T49=_T4A->f1;_T47=_T4A->f2;_T46=_T4A->f3;_T45=_T4A->f4;}{struct Cyc_Absyn_Exp*fname=_T49;struct Cyc_List_List*args=_T47;struct Cyc_Absyn_VarargCallInfo*vargs=_T46;int res=_T45;_T9=res;
# 579
if(_T9)goto _TLAD;else{goto _TLAF;}
_TLAF: return 0;_TLAD: {
int fname_ok=0;_TA=fname;{
void*_T4A=_TA->r;struct Cyc_List_List*_T4B;void*_T4C;_TB=(int*)_T4A;_TC=*_TB;switch(_TC){case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T4D=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T4A;_T4C=_T4D->f1;_T4B=_T4D->f2;}{struct Cyc_Absyn_Exp*e1=_T4C;struct Cyc_List_List*ts=_T4B;
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(e1);goto _LL1D;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T4D=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T4A;_TD=_T4D->f1;_T4C=(void*)_TD;}{void*b=_T4C;
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(fname);goto _LL1D;}default: goto _LL1D;}_LL1D:;}_TE=fname_ok;
# 587
if(!_TE)goto _TLB1;_TF=fname;{
void*_T4A=_TF->topt;struct Cyc_Absyn_FnInfo _T4B;if(_T4A==0)goto _TLB3;_T10=(int*)_T4A;_T11=*_T10;if(_T11!=4)goto _TLB5;_T12=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4A;_T13=_T12->f1;_T14=_T13.elt_type;_T15=(int*)_T14;_T16=*_T15;if(_T16!=6)goto _TLB7;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4A;_T17=_T4C->f1;_T18=_T17.elt_type;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4D=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T18;_T4B=_T4D->f1;}}{struct Cyc_Absyn_FnInfo fi=_T4B;_T19=Cyc_Atts_attribute_cmp;_T1A=fi;_T1B=_T1A.attributes;_T1C=& Cyc_Atts_No_throw_att_val;_T1D=(struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*)_T1C;_T1E=(void*)_T1D;
# 590
fname_ok=Cyc_List_mem(_T19,_T1B,_T1E);goto _LL24;}_TLB7: goto _LL27;_TLB5: goto _LL27;_TLB3: _LL27:{struct Cyc_Warn_String_Warn_Warg_struct _T4C;_T4C.tag=0;
# 593
_T4C.f1=_tag_fat("Expected function pointer type",sizeof(char),31U);_T1F=_T4C;}{struct Cyc_Warn_String_Warn_Warg_struct _T4C=_T1F;void*_T4D[1];_T4D[0]=& _T4C;_T21=Cyc_Warn_impos2;{int(*_T4E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T21;_T20=_T4E;}_T22=_tag_fat(_T4D,sizeof(void*),1);_T20(_T22);}_LL24:;}goto _TLB2;_TLB1: _TLB2: _T24=fname_ok;
# 595
if(!_T24)goto _TLB9;_T26=Cyc_List_forall;{int(*_T4A)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))_T26;_T25=_T4A;}_T27=args;_T23=_T25(Cyc_Tcstmt_ensure_no_throw_exp,_T27);goto _TLBA;_TLB9: _T23=0;_TLBA: return _T23;}}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T43;_T28=_T4A->f1;_T49=(void*)_T28;}{void*b=_T49;_T29=b;_T2A=(int*)_T29;_T2B=*_T2A;switch(_T2B){case 3: goto _LL2D;case 4: _LL2D:
# 620 "tcstmt.cyc"
 return 1;default:
 return 0;};}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T43;_T49=_T4A->f1;}{struct Cyc_Absyn_Exp*e1=_T49;_T2C=
# 623
Cyc_Tcstmt_ensure_no_throw_exp(e1);return _T2C;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T43;_T49=_T4A->f3;}{struct Cyc_List_List*exps=_T49;_T2E=Cyc_List_forall;{
# 625
int(*_T4A)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))_T2E;_T2D=_T4A;}_T30=Cyc_List_map;{struct Cyc_List_List*(*_T4A)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*))_T30;_T2F=_T4A;}_T32=Cyc_Core_snd;{struct Cyc_Absyn_Exp*(*_T4A)(struct _tuple17*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))_T32;_T31=_T4A;}_T33=exps;_T34=_T2F(_T31,_T33);_T35=_T2D(Cyc_Tcstmt_ensure_no_throw_exp,_T34);return _T35;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T43;_T36=_T4A->f1;_T49=(void*)_T36;_T48=_T4A->f2;_T45=_T4A->f3;_T44=_T4A->f4;}{void*ct=_T49;struct Cyc_Absyn_Exp*e1=_T48;int b=_T45;enum Cyc_Absyn_Coercion c=_T44;_T38=b;
# 627
if(_T38)goto _TLBC;else{goto _TLBE;}_TLBE: _T37=Cyc_Tcstmt_ensure_no_throw_exp(e1);goto _TLBD;_TLBC: _T37=0;_TLBD: return _T37;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T43;_T48=_T4A->f1;}{struct Cyc_Absyn_Exp*e1=_T48;_T48=e1;goto _LL1A;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T4A=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T43;_T48=_T4A->f1;}_LL1A: {struct Cyc_Absyn_Exp*e1=_T48;_T39=e1;{
# 630
void*_T4A=_T39->topt;void*_T4B;if(_T4A==0)goto _TLBF;_T3A=(int*)_T4A;_T3B=*_T3A;if(_T3B!=4)goto _TLC1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4A;_T3C=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4A;_T3D=& _T3C->f1;_T4B=(struct Cyc_Absyn_PtrInfo*)_T3D;}{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_T4B;_T3E=pi;_T3F=_T3E->ptr_atts;_T40=_T3F.nullable;_T41=(unsigned)_T40;_T42=!_T41;
return _T42;}_TLC1: goto _LL33;_TLBF: _LL33:
 return 0;;}}default:
# 634
 return 0;};}}
# 646 "tcstmt.cyc"
static int Cyc_Tcstmt_nothrow_visit_f1(int*env,struct Cyc_Absyn_Exp*e){int*_T0;_T0=env;
*_T0=Cyc_Tcstmt_ensure_no_throw_exp(e);
return 0;}
# 650
static int Cyc_Tcstmt_nothrow_visit_f2(int*env,struct Cyc_Absyn_Stmt*s){struct Cyc_Absyn_Stmt*_T0;int*_T1;int _T2;int*_T3;int _T4;int*_T5;int _T6;_T0=s;{
# 652
void*_T7=_T0->r;_T1=(int*)_T7;_T2=*_T1;if(_T2!=15)goto _TLC3;_T3=env;_T5=env;_T6=*_T5;
# 654
if(!_T6)goto _TLC5;_T4=Cyc_Tcstmt_check_catchall_handler(s);goto _TLC6;_TLC5: _T4=0;_TLC6:*_T3=_T4;return 0;_TLC3:
 return 1;;}}
# 658
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*s){void(*_T0)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Stmt*);void(*_T1)(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);int*_T2;struct Cyc_Absyn_Stmt*_T3;int _T4;
int env=1;_T1=Cyc_Absyn_visit_stmt;{
void(*_T5)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Stmt*))_T1;_T0=_T5;}_T2=& env;_T3=s;_T0(Cyc_Tcstmt_nothrow_visit_f1,Cyc_Tcstmt_nothrow_visit_f2,_T2,_T3);_T4=env;
return _T4;}
