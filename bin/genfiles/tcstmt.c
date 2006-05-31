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
# 524 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 531
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};
# 731 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 926
union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n (void);
# 929
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 932
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 939
void*Cyc_Absyn_compress(void*);
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 966
extern void*Cyc_Absyn_heap_rgn_type;
# 968
extern void*Cyc_Absyn_al_qual_type;
# 972
extern void*Cyc_Absyn_false_type;
# 974
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_rgn_handle_type(void*);
# 997
void*Cyc_Absyn_exn_type (void);
# 1023
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1040
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1072
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1137
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1150
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1159
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1164
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1168
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple2*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1212
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 1241
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
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var (void){struct _tuple2*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;struct Cyc_Int_pa_PrintArg_struct _T3;int _T4;int _T5;void**_T6;struct _fat_ptr _T7;struct _fat_ptr _T8;
static int counter=0;{struct _tuple2*_T9=_cycalloc(sizeof(struct _tuple2));
_T9->f0=Cyc_Absyn_Loc_n();{struct _fat_ptr*_TA=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_Int_pa_PrintArg_struct _TB;_TB.tag=1;_T4=counter;counter=_T4 + 1;_T5=_T4;_TB.f1=(unsigned)_T5;_T3=_TB;}{struct Cyc_Int_pa_PrintArg_struct _TB=_T3;void*_TC[1];_T6=_TC + 0;*_T6=& _TB;_T7=_tag_fat("_tmpS%X",sizeof(char),8U);_T8=_tag_fat(_TC,sizeof(void*),1);_T2=Cyc_aprintf(_T7,_T8);}*_TA=_T2;_T1=(struct _fat_ptr*)_TA;}_T9->f1=_T1;_T0=(struct _tuple2*)_T9;}return _T0;}
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
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){struct Cyc_Tcpat_TcPatResult _T0;struct Cyc_Tcpat_TcPatResult _T1;struct Cyc_Tcpat_TcPatResult _T2;struct Cyc_List_List*(*_T3)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*);struct Cyc_List_List*(*_T4)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Vardecl**(*_T5)(struct _tuple16*);void*(*_T6)(struct _tuple0*);struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*(*_T9)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_TA)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_TB)(struct _tuple15*);void*(*_TC)(struct _tuple0*);struct _tuple1*_TD;struct _tuple1 _TE;struct Cyc_List_List*_TF;struct Cyc_List_List*_T10;struct _tuple1*_T11;struct _tuple1 _T12;struct Cyc_List_List*_T13;struct Cyc_List_List*(*_T14)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_T15)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_T16)(struct _tuple15*);void*(*_T17)(struct _tuple0*);struct Cyc_List_List*(*_T18)(int(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_T19)(int(*)(void*),struct Cyc_List_List*);int(*_T1A)(struct _tuple15*);void*(*_T1B)(struct _tuple0*);struct _tuple1*_T1C;struct _tuple1 _T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;struct Cyc_Tcenv_Tenv*_T20;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T21;struct Cyc_List_List*_T22;void*_T23;void*_T24;struct Cyc_List_List*_T25;int _T26;struct Cyc_List_List*_T27;void*_T28;struct Cyc_Absyn_Vardecl**_T29;struct Cyc_List_List*_T2A;void*_T2B;struct Cyc_Absyn_Vardecl**_T2C;struct Cyc_Absyn_Vardecl*_T2D;struct Cyc_List_List*_T2E;struct Cyc_List_List*_T2F;void*_T30;int*_T31;int _T32;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T33;void*_T34;int*_T35;int _T36;struct Cyc_List_List*_T37;struct Cyc_Tcenv_Tenv*_T38;struct Cyc_Absyn_Exp*_T39;struct _fat_ptr _T3A;int _T3B;struct Cyc_Warn_String_Warn_Warg_struct _T3C;void**_T3D;struct Cyc_Absyn_Exp*_T3E;unsigned _T3F;struct _fat_ptr _T40;struct Cyc_List_List*_T41;struct Cyc_List_List*_T42;struct _tuple1*_T43;_T0=pat_res;_T43=_T0.tvars_and_effconstr_opt;_T1=pat_res;_T42=_T1.patvars;_T2=pat_res;_T41=_T2.aquals_bounds;{struct _tuple1*p=_T43;struct Cyc_List_List*vse=_T42;struct Cyc_List_List*aqb=_T41;_T4=Cyc_List_map;{
# 80
struct Cyc_List_List*(*_T44)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*))_T4;_T3=_T44;}_T6=Cyc_Core_fst;{struct Cyc_Absyn_Vardecl**(*_T44)(struct _tuple16*)=(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*))_T6;_T5=_T44;}_T7=vse;{struct Cyc_List_List*vs=_T3(_T5,_T7);
if(p!=0)goto _TL7;_T8=0;goto _TL8;_TL7: _TA=Cyc_List_map;{struct Cyc_List_List*(*_T44)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))_TA;_T9=_T44;}_TC=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T44)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))_TC;_TB=_T44;}_TD=p;_TE=*_TD;_TF=_TE.f0;_T8=_T9(_TB,_TF);_TL8: {struct Cyc_List_List*tvs=_T8;
if(p!=0)goto _TL9;_T10=0;goto _TLA;_TL9: _T11=p;_T12=*_T11;_T10=_T12.f1;_TLA: {struct Cyc_List_List*ecs=_T10;
if(p!=0)goto _TLB;_T13=0;goto _TLC;_TLB: _T15=Cyc_List_map;{struct Cyc_List_List*(*_T44)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))_T15;_T14=_T44;}_T17=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T44)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))_T17;_T16=_T44;}_T19=Cyc_List_filter;{struct Cyc_List_List*(*_T44)(int(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple15*),struct Cyc_List_List*))_T19;_T18=_T44;}_T1B=Cyc_Core_snd;{int(*_T44)(struct _tuple15*)=(int(*)(struct _tuple15*))_T1B;_T1A=_T44;}_T1C=p;_T1D=*_T1C;_T1E=_T1D.f0;_T1F=_T18(_T1A,_T1E);_T13=_T14(_T16,_T1F);_TLC: {struct Cyc_List_List*rgns=_T13;
te=Cyc_Tcenv_add_type_vars(loc,te,tvs);
_TL10: if(rgns!=0)goto _TLE;else{goto _TLF;}
_TLE: _T20=te;{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T44=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T44->tag=2;_T22=rgns;_T23=_T22->hd;_T44->f1=(struct Cyc_Absyn_Tvar*)_T23;_T21=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T44;}_T24=(void*)_T21;te=Cyc_Tcenv_add_region(_T20,_T24,0,0);_T25=rgns;
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
_TL1A: _T2F=aqb;_T30=_T2F->hd;{struct _tuple0*_T44=(struct _tuple0*)_T30;void*_T45;void*_T46;{struct _tuple0 _T47=*_T44;_T46=_T47.f0;_T45=_T47.f1;}{void*tv=_T46;void*b=_T45;
void*_T47=Cyc_Absyn_compress(tv);_T31=(int*)_T47;_T32=*_T31;if(_T32!=0)goto _TL1D;_T33=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T47;_T34=_T33->f1;_T35=(int*)_T34;_T36=*_T35;if(_T36!=15)goto _TL1F;
# 99
te=Cyc_Tcenv_add_aquals_bound(te,tv,b);goto _LL6;_TL1F: goto _LL9;_TL1D: _LL9: goto _LL6;_LL6:;}}_T37=aqb;
# 95
aqb=_T37->tl;goto _TL1C;_TL1B:
# 105
 if(where_opt==0)goto _TL21;_T38=te;_T39=where_opt;_T3A=
_tag_fat("switch clause guard",sizeof(char),20U);Cyc_Tcexp_tcTest(_T38,_T39,_T3A);_T3B=
Cyc_Absyn_no_side_effects_exp(where_opt);if(_T3B)goto _TL23;else{goto _TL25;}
_TL25:{struct Cyc_Warn_String_Warn_Warg_struct _T44;_T44.tag=0;_T44.f1=_tag_fat("cannot show &&-clause in pattern has no effects",sizeof(char),48U);_T3C=_T44;}{struct Cyc_Warn_String_Warn_Warg_struct _T44=_T3C;void*_T45[1];_T3D=_T45 + 0;*_T3D=& _T44;_T3E=where_opt;_T3F=_T3E->loc;_T40=_tag_fat(_T45,sizeof(void*),1);Cyc_Warn_err2(_T3F,_T40);}goto _TL24;_TL23: _TL24: goto _TL22;_TL21: _TL22:
# 110
 Cyc_Tcstmt_tcStmt(te,s,0);}}}}}}}
# 113
static int Cyc_Tcstmt_is_case_default(struct Cyc_Absyn_Switch_clause*c){struct Cyc_Absyn_Switch_clause*_T0;struct Cyc_Absyn_Pat*_T1;int*_T2;unsigned _T3;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T4;struct Cyc_Absyn_Pat*_T5;struct Cyc_Absyn_Pat*_T6;void*_T7;int*_T8;int _T9;_T0=c;_T1=_T0->pattern;{
void*_TA=_T1->r;_T2=(int*)_TA;_T3=*_T2;switch(_T3){case 0:
 return 1;case 1: _T4=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_TA;_T5=_T4->f2;_T6=(struct Cyc_Absyn_Pat*)_T5;_T7=_T6->r;_T8=(int*)_T7;_T9=*_T8;if(_T9!=0)goto _TL27;
return 1;_TL27: goto _LL5;default: _LL5:
 return 0;};}}
# 129 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){struct Cyc_Absyn_Stmt*_T0;int*_T1;unsigned _T2;int _T3;int _T4;int _T5;int _T6;struct Cyc_Warn_String_Warn_Warg_struct _T7;struct Cyc_Warn_Typ_Warn_Warg_struct _T8;void**_T9;void**_TA;struct Cyc_Absyn_Stmt*_TB;unsigned _TC;struct _fat_ptr _TD;struct Cyc_Warn_String_Warn_Warg_struct _TE;struct Cyc_Warn_Typ_Warn_Warg_struct _TF;void**_T10;void**_T11;struct Cyc_Absyn_Stmt*_T12;unsigned _T13;struct _fat_ptr _T14;struct Cyc_Tcenv_Tenv*_T15;void**_T16;struct Cyc_Absyn_Exp*_T17;struct Cyc_List_List*_T18;struct Cyc_Absyn_Exp*_T19;void*_T1A;int _T1B;struct Cyc_Warn_String_Warn_Warg_struct _T1C;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1D;struct Cyc_Absyn_Exp*_T1E;struct Cyc_Warn_String_Warn_Warg_struct _T1F;struct Cyc_Warn_Typ_Warn_Warg_struct _T20;void**_T21;void**_T22;void**_T23;void**_T24;struct Cyc_Absyn_Stmt*_T25;unsigned _T26;struct _fat_ptr _T27;struct Cyc_Absyn_Exp*_T28;unsigned _T29;struct Cyc_List_List*_T2A;void*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Tcenv_Tenv*_T2D;struct Cyc_Absyn_Exp*_T2E;struct _fat_ptr _T2F;struct _tuple11 _T30;struct _tuple11 _T31;struct Cyc_Tcenv_Tenv*_T32;struct Cyc_Absyn_Exp*_T33;struct _fat_ptr _T34;struct _tuple11 _T35;struct _tuple11 _T36;struct _tuple11 _T37;struct _tuple11 _T38;struct Cyc_Tcenv_Tenv*_T39;struct Cyc_Absyn_Exp*_T3A;struct _fat_ptr _T3B;struct _tuple11 _T3C;struct _tuple11 _T3D;struct Cyc_Tcenv_Tenv*_T3E;struct Cyc_Absyn_Exp*_T3F;struct _fat_ptr _T40;struct Cyc_Absyn_Tvar*_T41;struct _fat_ptr*_T42;struct _fat_ptr _T43;struct Cyc_String_pa_PrintArg_struct _T44;struct _fat_ptr*_T45;void**_T46;struct _fat_ptr _T47;struct _fat_ptr _T48;struct Cyc_Absyn_Tvar*_T49;struct Cyc_Absyn_Tvar*_T4A;struct Cyc_Absyn_Kind*_T4B;struct Cyc_Absyn_Kind*_T4C;struct Cyc_Absyn_Tvar*_T4D;struct Cyc_Absyn_Stmt*_T4E;unsigned _T4F;struct Cyc_Tcenv_Tenv*_T50;struct Cyc_Absyn_Tvar*_T51;struct Cyc_Tcenv_Tenv*_T52;struct Cyc_Absyn_Stmt*_T53;struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T54;struct Cyc_Absyn_Switch_clause***_T55;struct Cyc_Warn_String_Warn_Warg_struct _T56;void**_T57;struct Cyc_Absyn_Stmt*_T58;unsigned _T59;struct _fat_ptr _T5A;const struct _tuple14*_T5B;struct Cyc_List_List*(*_T5C)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*(*_T5D)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple13*(*_T5E)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T5F;struct Cyc_List_List*_T60;struct Cyc_List_List*(*_T61)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*(*_T62)(void*(*)(void*,void*),void*,struct Cyc_List_List*);void*(*_T63)(struct Cyc_List_List*,void*);struct Cyc_List_List*_T64;struct Cyc_List_List*_T65;struct Cyc_Tcenv_Tenv*_T66;struct Cyc_List_List*_T67;void*_T68;struct Cyc_Absyn_Exp*_T69;struct Cyc_List_List*_T6A;struct Cyc_List_List*_T6B;void*_T6C;struct Cyc_Absyn_Exp*_T6D;struct Cyc_List_List*_T6E;void*_T6F;int _T70;struct Cyc_Warn_String_Warn_Warg_struct _T71;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T72;struct Cyc_List_List*_T73;void*_T74;struct Cyc_Absyn_Exp*_T75;struct Cyc_Warn_String_Warn_Warg_struct _T76;struct Cyc_Warn_Typ_Warn_Warg_struct _T77;struct Cyc_List_List*_T78;void**_T79;void**_T7A;void**_T7B;void**_T7C;struct Cyc_Absyn_Stmt*_T7D;unsigned _T7E;struct _fat_ptr _T7F;struct Cyc_List_List*_T80;void*_T81;struct Cyc_Absyn_Exp*_T82;unsigned _T83;struct Cyc_List_List*_T84;struct Cyc_List_List*_T85;void*_T86;struct Cyc_List_List*_T87;void*_T88;struct Cyc_Absyn_Exp*_T89;struct Cyc_List_List*_T8A;struct Cyc_List_List*_T8B;struct Cyc_Warn_String_Warn_Warg_struct _T8C;void**_T8D;struct Cyc_Absyn_Stmt*_T8E;unsigned _T8F;struct _fat_ptr _T90;struct Cyc_Warn_String_Warn_Warg_struct _T91;void**_T92;struct Cyc_Absyn_Stmt*_T93;unsigned _T94;struct _fat_ptr _T95;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T96;struct Cyc_Absyn_Exp**_T97;struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T98;void**_T99;int _T9A;struct Cyc_Absyn_Exp**_T9B;struct Cyc_Absyn_Exp*_T9C;unsigned _T9D;struct Cyc_List_List*_T9E;void*_T9F;struct Cyc_Absyn_Exp*_TA0;int _TA1;struct Cyc_Tcenv_Tenv*_TA2;struct Cyc_List_List*_TA3;void*_TA4;struct Cyc_Absyn_Switch_clause*_TA5;struct Cyc_Absyn_Pat*_TA6;void**_TA7;struct Cyc_Absyn_Exp*_TA8;struct Cyc_Tcpat_TcPatResult*_TA9;struct Cyc_List_List*_TAA;struct Cyc_List_List*_TAB;struct Cyc_Tcpat_TcPatResult*_TAC;struct Cyc_Tcenv_Tenv*_TAD;struct Cyc_List_List*_TAE;struct Cyc_List_List*_TAF;void*_TB0;struct Cyc_Absyn_Switch_clause*_TB1;struct Cyc_Absyn_Pat*_TB2;void**_TB3;struct Cyc_Absyn_Exp*_TB4;struct Cyc_Tcpat_TcPatResult*_TB5;struct _tuple1*_TB6;struct Cyc_Tcpat_TcPatResult*_TB7;struct _tuple1*_TB8;struct _tuple1 _TB9;struct Cyc_List_List*_TBA;struct Cyc_Tcpat_TcPatResult*_TBB;struct Cyc_List_List*_TBC;struct _tuple1 _TBD;struct Cyc_List_List*_TBE;struct Cyc_List_List*_TBF;struct Cyc_List_List*_TC0;struct Cyc_Tcpat_TcPatResult*_TC1;struct _tuple1*_TC2;struct Cyc_List_List*(*_TC3)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_TC4)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_TC5)(struct _tuple15*);void*(*_TC6)(struct _tuple0*);struct Cyc_Tcpat_TcPatResult*_TC7;struct _tuple1*_TC8;struct _tuple1 _TC9;struct Cyc_List_List*_TCA;struct Cyc_Tcenv_Tenv*_TCB;struct Cyc_List_List*_TCC;struct Cyc_List_List*_TCD;struct Cyc_List_List*_TCE;struct Cyc_List_List*_TCF;struct Cyc_List_List*_TD0;void*_TD1;struct Cyc_Absyn_Switch_clause*_TD2;struct Cyc_List_List*_TD3;void*_TD4;struct Cyc_Absyn_Switch_clause*_TD5;struct Cyc_List_List*_TD6;void*_TD7;struct Cyc_Absyn_Switch_clause*_TD8;unsigned _TD9;void*_TDA;struct Cyc_Absyn_Pat*_TDB;void*_TDC;void*_TDD;enum Cyc_Absyn_Coercion _TDE;int _TDF;struct Cyc_Warn_String_Warn_Warg_struct _TE0;struct Cyc_Warn_Typ_Warn_Warg_struct _TE1;struct Cyc_Warn_String_Warn_Warg_struct _TE2;struct Cyc_Warn_TypOpt_Warn_Warg_struct _TE3;struct Cyc_Absyn_Pat*_TE4;void**_TE5;void**_TE6;void**_TE7;void**_TE8;struct Cyc_List_List*_TE9;void*_TEA;struct Cyc_Absyn_Switch_clause*_TEB;unsigned _TEC;struct _fat_ptr _TED;struct Cyc_Tcenv_Tenv*_TEE;struct Cyc_Absyn_Pat*_TEF;struct Cyc_Tcpat_TcPatResult*_TF0;struct Cyc_Tcpat_TcPatResult*_TF1;struct Cyc_List_List*_TF2;void*_TF3;struct Cyc_Absyn_Switch_clause*_TF4;struct Cyc_Core_Opt*_TF5;struct Cyc_Tcpat_TcPatResult _TF6;struct Cyc_List_List*_TF7;void*_TF8;struct Cyc_Absyn_Switch_clause*_TF9;unsigned _TFA;struct Cyc_Tcenv_Tenv*_TFB;struct Cyc_Tcpat_TcPatResult _TFC;struct Cyc_List_List*_TFD;void*_TFE;struct Cyc_Absyn_Switch_clause*_TFF;struct Cyc_Absyn_Stmt*_T100;struct Cyc_List_List*_T101;void*_T102;struct Cyc_Absyn_Switch_clause*_T103;struct Cyc_Absyn_Exp*_T104;struct Cyc_List_List*_T105;struct Cyc_Absyn_Stmt*_T106;unsigned _T107;struct Cyc_Tcenv_Tenv*_T108;struct Cyc_List_List*_T109;void**_T10A;struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T10B;struct Cyc_List_List**_T10C;struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T10D;void**_T10E;struct Cyc_Absyn_Stmt*_T10F;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T110;struct Cyc_Absyn_Stmt*_T111;void*_T112;struct Cyc_Absyn_Stmt*_T113;unsigned _T114;struct Cyc_Absyn_Stmt*_T115;unsigned _T116;void*_T117;struct Cyc_Absyn_Stmt*_T118;unsigned _T119;struct Cyc_Absyn_Stmt*_T11A;int(*_T11B)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*);int(*_T11C)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List**_T11D;struct Cyc_List_List*_T11E;int _T11F;struct _tuple2*_T120;struct _tuple2*_T121;struct _fat_ptr*_T122;struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T123;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T124;struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*_T125;void*_T126;void*_T127;struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T128;struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T129;void*_T12A;void*_T12B;struct Cyc_Absyn_Exp*_T12C;struct Cyc_Absyn_Switch_clause*_T12D;struct Cyc_Absyn_Switch_clause*_T12E;struct Cyc_Absyn_Switch_clause*_T12F;struct Cyc_Absyn_Switch_clause*_T130;struct Cyc_Absyn_Switch_clause*_T131;struct Cyc_List_List**_T132;struct Cyc_List_List**_T133;struct Cyc_List_List*_T134;struct Cyc_List_List*_T135;struct Cyc_Absyn_Switch_clause**_T136;struct _fat_ptr _T137;struct Cyc_List_List**_T138;int _T139;struct Cyc_Tcenv_Tenv*_T13A;struct Cyc_List_List*_T13B;void*_T13C;struct Cyc_Absyn_Switch_clause*_T13D;struct Cyc_Absyn_Pat*_T13E;void**_T13F;struct Cyc_Tcpat_TcPatResult*_T140;struct Cyc_List_List*_T141;struct Cyc_List_List*_T142;struct Cyc_Tcpat_TcPatResult*_T143;struct Cyc_Tcenv_Tenv*_T144;struct Cyc_List_List*_T145;struct Cyc_List_List*_T146;void*_T147;struct Cyc_Absyn_Switch_clause*_T148;struct Cyc_Absyn_Pat*_T149;void**_T14A;struct Cyc_Tcpat_TcPatResult*_T14B;struct _tuple1*_T14C;struct Cyc_Tcpat_TcPatResult*_T14D;struct _tuple1*_T14E;struct _tuple1 _T14F;struct Cyc_List_List*_T150;struct Cyc_Tcpat_TcPatResult*_T151;struct Cyc_List_List*_T152;struct _tuple1 _T153;struct Cyc_List_List*_T154;struct Cyc_List_List*_T155;struct Cyc_List_List*_T156;struct Cyc_Tcpat_TcPatResult*_T157;struct _tuple1*_T158;struct Cyc_List_List*(*_T159)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*);struct Cyc_List_List*(*_T15A)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_T15B)(struct _tuple15*);void*(*_T15C)(struct _tuple0*);struct Cyc_Tcpat_TcPatResult*_T15D;struct _tuple1*_T15E;struct _tuple1 _T15F;struct Cyc_List_List*_T160;struct Cyc_Tcenv_Tenv*_T161;struct Cyc_List_List*_T162;struct Cyc_List_List*_T163;struct Cyc_List_List*_T164;struct Cyc_List_List*_T165;struct Cyc_List_List*_T166;void*_T167;struct Cyc_Absyn_Switch_clause*_T168;struct Cyc_List_List*_T169;void*_T16A;struct Cyc_Absyn_Switch_clause*_T16B;struct Cyc_List_List*_T16C;void*_T16D;struct Cyc_Absyn_Switch_clause*_T16E;unsigned _T16F;void*_T170;struct Cyc_Absyn_Pat*_T171;void*_T172;void*_T173;enum Cyc_Absyn_Coercion _T174;int _T175;struct Cyc_Warn_String_Warn_Warg_struct _T176;struct Cyc_Warn_Typ_Warn_Warg_struct _T177;struct Cyc_Warn_String_Warn_Warg_struct _T178;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T179;struct Cyc_Absyn_Pat*_T17A;void**_T17B;void**_T17C;void**_T17D;void**_T17E;struct Cyc_List_List*_T17F;void*_T180;struct Cyc_Absyn_Switch_clause*_T181;unsigned _T182;struct _fat_ptr _T183;struct Cyc_Tcenv_Tenv*_T184;struct Cyc_Absyn_Pat*_T185;struct Cyc_Tcpat_TcPatResult*_T186;struct Cyc_Tcpat_TcPatResult*_T187;struct Cyc_List_List*_T188;void*_T189;struct Cyc_Absyn_Switch_clause*_T18A;struct Cyc_Core_Opt*_T18B;struct Cyc_Tcpat_TcPatResult _T18C;struct Cyc_List_List*_T18D;void*_T18E;struct Cyc_Absyn_Switch_clause*_T18F;unsigned _T190;struct Cyc_Tcenv_Tenv*_T191;struct Cyc_Tcpat_TcPatResult _T192;struct Cyc_List_List*_T193;void*_T194;struct Cyc_Absyn_Switch_clause*_T195;struct Cyc_Absyn_Stmt*_T196;struct Cyc_List_List*_T197;void*_T198;struct Cyc_Absyn_Switch_clause*_T199;struct Cyc_Absyn_Exp*_T19A;struct Cyc_List_List*_T19B;struct Cyc_Absyn_Stmt*_T19C;unsigned _T19D;struct Cyc_Tcenv_Tenv*_T19E;struct Cyc_List_List**_T19F;struct Cyc_List_List*_T1A0;void**_T1A1;struct Cyc_Tcenv_Tenv*_T1A2;int _T1A3;struct Cyc_Absyn_Stmt*_T1A4;unsigned _T1A5;struct Cyc_Tcenv_Tenv*_T1A6;struct Cyc_Absyn_Decl*_T1A7;int*_T1A8;unsigned _T1A9;struct Cyc_Absyn_Vardecl*_T1AA;struct Cyc_Absyn_Vardecl*_T1AB;void**_T1AC;enum Cyc_Absyn_Scope _T1AD;int _T1AE;struct Cyc_Absyn_Vardecl*_T1AF;struct Cyc_Warn_String_Warn_Warg_struct _T1B0;void**_T1B1;struct Cyc_Absyn_Decl*_T1B2;unsigned _T1B3;struct _fat_ptr _T1B4;void**_T1B5;void*_T1B6;int _T1B7;int*_T1B8;unsigned _T1B9;int _T1BA;struct Cyc_Absyn_Vardecl*_T1BB;struct Cyc_Absyn_Vardecl*_T1BC;struct Cyc_Absyn_Exp*_T1BD;struct Cyc_Absyn_ArrayInfo _T1BE;struct Cyc_Absyn_Exp*_T1BF;struct Cyc_Absyn_Vardecl*_T1C0;struct Cyc_Absyn_Exp*_T1C1;int*_T1C2;unsigned _T1C3;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T1C4;union Cyc_Absyn_Cnst _T1C5;struct _union_Cnst_Wstring_c _T1C6;unsigned _T1C7;union Cyc_Absyn_Cnst _T1C8;struct _union_Cnst_String_c _T1C9;struct _fat_ptr _T1CA;unsigned _T1CB;union Cyc_Absyn_Cnst _T1CC;struct _union_Cnst_Wstring_c _T1CD;int _T1CE;unsigned _T1CF;struct Cyc_Absyn_Vardecl*_T1D0;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T1D1;struct Cyc_List_List*_T1D2;int _T1D3;struct Cyc_Absyn_Stmt*_T1D4;unsigned _T1D5;struct Cyc_Tcenv_Tenv*_T1D6;struct Cyc_List_List*_T1D7;struct Cyc_Absyn_Kind*_T1D8;struct Cyc_Absyn_Kind*_T1D9;int _T1DA;void*_T1DB;struct Cyc_Absyn_Vardecl*_T1DC;struct Cyc_Absyn_Stmt*_T1DD;unsigned _T1DE;struct Cyc_Absyn_Vardecl*_T1DF;struct Cyc_Absyn_Tqual _T1E0;int _T1E1;void*_T1E2;enum Cyc_Absyn_Scope _T1E3;int _T1E4;enum Cyc_Absyn_Scope _T1E5;int _T1E6;struct Cyc_Warn_String_Warn_Warg_struct _T1E7;void**_T1E8;struct Cyc_Absyn_Decl*_T1E9;unsigned _T1EA;struct _fat_ptr _T1EB;struct Cyc_Tcenv_Tenv*_T1EC;void**_T1ED;struct Cyc_Absyn_Exp*_T1EE;int _T1EF;int _T1F0;struct Cyc_Warn_String_Warn_Warg_struct _T1F1;void**_T1F2;struct Cyc_Absyn_Decl*_T1F3;unsigned _T1F4;struct _fat_ptr _T1F5;struct Cyc_List_List*_T1F6;struct Cyc_Absyn_Exp*_T1F7;void*_T1F8;int _T1F9;struct Cyc_Warn_Vardecl_Warn_Warg_struct _T1FA;struct Cyc_Warn_String_Warn_Warg_struct _T1FB;struct Cyc_Warn_Typ_Warn_Warg_struct _T1FC;struct Cyc_Warn_String_Warn_Warg_struct _T1FD;struct Cyc_Warn_TypOpt_Warn_Warg_struct _T1FE;struct Cyc_Absyn_Exp*_T1FF;void**_T200;void**_T201;void**_T202;void**_T203;void**_T204;struct Cyc_Absyn_Decl*_T205;unsigned _T206;struct _fat_ptr _T207;void*_T208;struct Cyc_Absyn_Exp*_T209;void*_T20A;void*_T20B;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T20C;struct Cyc_Core_Opt**_T20D;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T20E;struct Cyc_Absyn_Exp**_T20F;struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T210;void**_T211;struct Cyc_Absyn_Pat*_T212;int*_T213;unsigned _T214;int _T215;struct Cyc_Absyn_Exp**_T216;struct Cyc_Absyn_Exp*_T217;void*_T218;struct Cyc_Tcenv_Tenv*_T219;struct Cyc_Absyn_Pat*_T21A;void**_T21B;struct Cyc_Absyn_Exp*_T21C;struct Cyc_Core_Opt**_T21D;struct Cyc_Core_Opt*_T21E;struct Cyc_Tcpat_TcPatResult _T21F;struct Cyc_Absyn_Pat*_T220;void*_T221;struct Cyc_Absyn_Exp*_T222;void*_T223;int _T224;struct Cyc_List_List*_T225;struct Cyc_Absyn_Exp*_T226;void*_T227;int _T228;struct Cyc_Warn_String_Warn_Warg_struct _T229;struct Cyc_Warn_Typ_Warn_Warg_struct _T22A;struct Cyc_Warn_String_Warn_Warg_struct _T22B;struct Cyc_Warn_Typ_Warn_Warg_struct _T22C;void**_T22D;void**_T22E;void**_T22F;void**_T230;struct Cyc_Absyn_Decl*_T231;unsigned _T232;struct _fat_ptr _T233;struct Cyc_Tcenv_Tenv*_T234;struct Cyc_Absyn_Pat*_T235;struct Cyc_Tcpat_TcPatResult*_T236;struct Cyc_Tcpat_TcPatResult*_T237;struct Cyc_Absyn_Pat*_T238;unsigned _T239;struct Cyc_Tcenv_Tenv*_T23A;struct Cyc_Absyn_Pat*_T23B;void**_T23C;struct Cyc_Absyn_Stmt*_T23D;unsigned _T23E;struct Cyc_Tcenv_Tenv*_T23F;struct Cyc_Tcpat_TcPatResult _T240;struct Cyc_Absyn_Stmt*_T241;struct Cyc_List_List*_T242;void*_T243;struct Cyc_Absyn_Vardecl*_T244;struct Cyc_Absyn_Stmt*_T245;unsigned _T246;struct Cyc_Tcenv_Tenv*_T247;struct Cyc_List_List*_T248;struct Cyc_Absyn_Kind*_T249;struct Cyc_Absyn_Kind*_T24A;struct Cyc_List_List*_T24B;void*_T24C;struct Cyc_Absyn_Vardecl*_T24D;void*_T24E;struct Cyc_List_List*_T24F;struct Cyc_Absyn_Stmt*_T250;struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T251;struct Cyc_Absyn_Stmt*_T252;void*_T253;struct Cyc_Absyn_Stmt*_T254;unsigned _T255;struct Cyc_Absyn_Stmt*_T256;unsigned _T257;void*_T258;struct Cyc_Absyn_Stmt*_T259;unsigned _T25A;struct Cyc_Absyn_Stmt*_T25B;struct Cyc_Absyn_Vardecl*_T25C;struct Cyc_Absyn_Vardecl*_T25D;void**_T25E;struct Cyc_Absyn_Vardecl*_T25F;void**_T260;void**_T261;struct Cyc_Absyn_Exp*_T262;unsigned _T263;struct _tuple2*_T264;struct Cyc_List_List*_T265;struct _fat_ptr**_T266;struct _fat_ptr*_T267;struct _fat_ptr _T268;struct _tuple2*_T269;struct _fat_ptr*_T26A;struct Cyc_Core_Opt*_T26B;struct Cyc_Absyn_Kind*_T26C;struct Cyc_Core_Opt*_T26D;struct Cyc_Absyn_Kind*_T26E;struct Cyc_Core_Opt*_T26F;struct Cyc_Absyn_Kind*_T270;union Cyc_Absyn_AggrInfo _T271;struct Cyc_List_List*_T272;void**_T273;struct _fat_ptr _T274;void*_T275;void*_T276;void*_T277;struct Cyc_Absyn_Tqual _T278;void*_T279;void*_T27A;struct Cyc_Absyn_Stmt*_T27B;unsigned _T27C;struct Cyc_Tcenv_Tenv*_T27D;struct Cyc_List_List*_T27E;struct Cyc_Absyn_Kind*_T27F;struct Cyc_Absyn_Kind*_T280;void*_T281;struct Cyc_Tcenv_Tenv*_T282;void**_T283;struct Cyc_Absyn_Exp*_T284;int _T285;struct Cyc_List_List*_T286;struct Cyc_Absyn_Exp*_T287;void*_T288;int _T289;struct Cyc_Warn_String_Warn_Warg_struct _T28A;struct Cyc_Warn_Typ_Warn_Warg_struct _T28B;struct Cyc_Warn_String_Warn_Warg_struct _T28C;struct Cyc_Warn_Typ_Warn_Warg_struct _T28D;void**_T28E;void**_T28F;void**_T290;void**_T291;struct Cyc_Absyn_Stmt*_T292;unsigned _T293;struct _fat_ptr _T294;int _T295;struct Cyc_Warn_String_Warn_Warg_struct _T296;void**_T297;struct Cyc_Absyn_Stmt*_T298;unsigned _T299;struct _fat_ptr _T29A;void**_T29B;struct Cyc_Absyn_VarType_Absyn_Type_struct*_T29C;struct Cyc_Absyn_Stmt*_T29D;unsigned _T29E;struct Cyc_Tcenv_Tenv*_T29F;struct Cyc_List_List*_T2A0;struct Cyc_Absyn_Stmt*_T2A1;unsigned _T2A2;struct Cyc_Tcenv_Tenv*_T2A3;struct Cyc_List_List*_T2A4;struct Cyc_Absyn_Kind*_T2A5;struct Cyc_Absyn_Kind*_T2A6;void**_T2A7;void*_T2A8;void**_T2A9;void*_T2AA;void*_T2AB;int _T2AC;int(*_T2AD)(struct _fat_ptr,struct _fat_ptr);void*(*_T2AE)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T2AF;struct _fat_ptr _T2B0;struct Cyc_Absyn_Fndecl*_T2B1;enum Cyc_Absyn_Scope _T2B2;int _T2B3;struct Cyc_Warn_String_Warn_Warg_struct _T2B4;void**_T2B5;struct Cyc_Absyn_Decl*_T2B6;unsigned _T2B7;struct _fat_ptr _T2B8;struct Cyc_Absyn_Decl*_T2B9;unsigned _T2BA;struct Cyc_Absyn_Fndecl*_T2BB;struct Cyc_Absyn_FnInfo _T2BC;struct Cyc_List_List*_T2BD;struct Cyc_Absyn_Decl*_T2BE;unsigned _T2BF;struct Cyc_Tcenv_Tenv*_T2C0;struct Cyc_Absyn_Fndecl*_T2C1;struct Cyc_Absyn_Fndecl*_T2C2;void*_T2C3;struct Cyc_Absyn_Fndecl*_T2C4;struct Cyc_Absyn_FnInfo _T2C5;struct Cyc_List_List*_T2C6;struct Cyc_Absyn_Vardecl*_T2C7;struct Cyc_Absyn_Fndecl*_T2C8;struct Cyc_Absyn_Vardecl*_T2C9;struct Cyc_Absyn_Fndecl*_T2CA;struct Cyc_Absyn_Vardecl*_T2CB;struct Cyc_Absyn_Vardecl*_T2CC;struct Cyc_Absyn_Vardecl*_T2CD;void*_T2CE;void*_T2CF;void*_T2D0;struct Cyc_Absyn_Tqual _T2D1;void*_T2D2;void*_T2D3;struct Cyc_Absyn_Vardecl*_T2D4;struct Cyc_Absyn_Vardecl*_T2D5;struct Cyc_Absyn_Vardecl*_T2D6;struct Cyc_Absyn_Vardecl*_T2D7;struct Cyc_Absyn_Vardecl*_T2D8;struct Cyc_Absyn_Fndecl*_T2D9;struct Cyc_Absyn_Stmt*_T2DA;struct Cyc_Absyn_Vardecl*_T2DB;struct Cyc_Absyn_Fndecl*_T2DC;struct Cyc_Tcenv_Tenv*_T2DD;struct Cyc_Tcenv_Fenv*_T2DE;struct Cyc_Tcenv_Tenv*_T2DF;struct Cyc_Absyn_Decl*_T2E0;unsigned _T2E1;struct Cyc_Tcenv_Fenv*_T2E2;struct Cyc_Absyn_Fndecl*_T2E3;struct Cyc_Tcenv_Tenv*_T2E4;struct Cyc_Absyn_Fndecl*_T2E5;struct Cyc_Absyn_Stmt*_T2E6;struct Cyc_Tcenv_Tenv*_T2E7;struct Cyc_Warn_String_Warn_Warg_struct _T2E8;struct Cyc_Warn_String_Warn_Warg_struct _T2E9;struct Cyc_Warn_String_Warn_Warg_struct _T2EA;void**_T2EB;void**_T2EC;void**_T2ED;int(*_T2EE)(struct _fat_ptr);void*(*_T2EF)(struct _fat_ptr);struct _fat_ptr _T2F0;_T0=s0;{
# 131
void*_T2F1=_T0->r;struct Cyc_Absyn_Decl*_T2F2;struct Cyc_Absyn_Stmt*_T2F3;struct Cyc_Absyn_Stmt*_T2F4;struct Cyc_Absyn_Exp*_T2F5;void*_T2F6;void*_T2F7;void*_T2F8;_T1=(int*)_T2F1;_T2=*_T1;switch(_T2){case 0:
# 133
 return;case 1:{struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F8=_T2F9->f1;}{struct Cyc_Absyn_Exp*e=_T2F8;
# 136
Cyc_Tcexp_tcExp(te,0,e);_T3=
Cyc_Tcenv_in_stmt_exp(te);if(_T3)goto _TL2A;else{goto _TL2C;}
_TL2C: Cyc_Tcstmt_simplify_unused_result_exp(e);goto _TL2B;_TL2A: _TL2B:
 return;}case 2:{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F8=_T2F9->f1;_T2F7=_T2F9->f2;}{struct Cyc_Absyn_Stmt*s1=_T2F8;struct Cyc_Absyn_Stmt*s2=_T2F7;
# 142
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3:{struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F8=_T2F9->f1;}{struct Cyc_Absyn_Exp*eopt=_T2F8;
# 147
void*t=Cyc_Tcenv_return_typ(te);
if(eopt!=0)goto _TL2D;_T4=
Cyc_Tcutil_is_void_type(t);if(_T4)goto _TL2F;else{goto _TL31;}
_TL31: _T5=Cyc_Tcutil_is_any_float_type(t);if(_T5)goto _TL34;else{goto _TL35;}_TL35: _T6=Cyc_Tcutil_is_any_int_type(t);if(_T6)goto _TL34;else{goto _TL32;}
_TL34:{struct Cyc_Warn_String_Warn_Warg_struct _T2F9;_T2F9.tag=0;_T2F9.f1=_tag_fat("should return a value of type ",sizeof(char),31U);_T7=_T2F9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F9=_T7;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FA;_T2FA.tag=2;_T2FA.f1=t;_T8=_T2FA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FA=_T8;void*_T2FB[2];_T9=_T2FB + 0;*_T9=& _T2F9;_TA=_T2FB + 1;*_TA=& _T2FA;_TB=s0;_TC=_TB->loc;_TD=_tag_fat(_T2FB,sizeof(void*),2);Cyc_Warn_warn2(_TC,_TD);}}goto _TL33;
# 153
_TL32:{struct Cyc_Warn_String_Warn_Warg_struct _T2F9;_T2F9.tag=0;_T2F9.f1=_tag_fat("must return a value of type ",sizeof(char),29U);_TE=_T2F9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F9=_TE;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FA;_T2FA.tag=2;_T2FA.f1=t;_TF=_T2FA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FA=_TF;void*_T2FB[2];_T10=_T2FB + 0;*_T10=& _T2F9;_T11=_T2FB + 1;*_T11=& _T2FA;_T12=s0;_T13=_T12->loc;_T14=_tag_fat(_T2FB,sizeof(void*),2);Cyc_Warn_err2(_T13,_T14);}}_TL33: goto _TL30;_TL2F: _TL30:
# 155
 return;_TL2D: _T15=te;_T16=& t;_T17=eopt;
# 158
Cyc_Tcexp_tcExp(_T15,_T16,_T17);_T18=
Cyc_Tcenv_curr_aquals_bounds(te);_T19=eopt;_T1A=t;_T1B=Cyc_Tcutil_coerce_arg(_T18,_T19,_T1A,0);if(_T1B)goto _TL36;else{goto _TL38;}
_TL38:{struct Cyc_Warn_String_Warn_Warg_struct _T2F9;_T2F9.tag=0;_T2F9.f1=_tag_fat("returns value of type ",sizeof(char),23U);_T1C=_T2F9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F9=_T1C;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2FA;_T2FA.tag=3;_T1E=eopt;_T2FA.f1=_T1E->topt;_T1D=_T2FA;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2FA=_T1D;{struct Cyc_Warn_String_Warn_Warg_struct _T2FB;_T2FB.tag=0;_T2FB.f1=_tag_fat(" but requires ",sizeof(char),15U);_T1F=_T2FB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FB=_T1F;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FC;_T2FC.tag=2;_T2FC.f1=t;_T20=_T2FC;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FC=_T20;void*_T2FD[4];_T21=_T2FD + 0;*_T21=& _T2F9;_T22=_T2FD + 1;*_T22=& _T2FA;_T23=_T2FD + 2;*_T23=& _T2FB;_T24=_T2FD + 3;*_T24=& _T2FC;_T25=s0;_T26=_T25->loc;_T27=_tag_fat(_T2FD,sizeof(void*),4);Cyc_Warn_err2(_T26,_T27);}}}}
Cyc_Unify_explain_failure();goto _TL37;_TL36: _TL37: _T28=eopt;_T29=_T28->loc;_T2A=
# 163
Cyc_Tcenv_curr_aquals_bounds(te);_T2B=t;_T2C=eopt;Cyc_Tcexp_check_consume(_T29,_T2A,_T2B,_T2C);
return;}case 4:{struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F8=_T2F9->f1;_T2F7=_T2F9->f2;_T2F6=_T2F9->f3;}{struct Cyc_Absyn_Exp*e=_T2F8;struct Cyc_Absyn_Stmt*s1=_T2F7;struct Cyc_Absyn_Stmt*s2=_T2F6;_T2D=te;_T2E=e;_T2F=
# 167
_tag_fat("if statement",sizeof(char),13U);Cyc_Tcexp_tcTest(_T2D,_T2E,_T2F);
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5:{struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_T2F1;_T30=_T2F9->f1;_T2F8=_T30.f0;_T31=_T2F9->f1;_T2F7=_T31.f1;_T2F6=_T2F9->f2;}{struct Cyc_Absyn_Exp*e=_T2F8;struct Cyc_Absyn_Stmt*cont_s=_T2F7;struct Cyc_Absyn_Stmt*s=_T2F6;_T32=te;_T33=e;_T34=
# 173
_tag_fat("while loop",sizeof(char),11U);Cyc_Tcexp_tcTest(_T32,_T33,_T34);
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9:{struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F8=_T2F9->f1;_T35=_T2F9->f2;_T2F7=_T35.f0;_T36=_T2F9->f2;_T2F6=_T36.f1;_T37=_T2F9->f3;_T2F5=_T37.f0;_T38=_T2F9->f3;_T2F4=_T38.f1;_T2F3=_T2F9->f4;}{struct Cyc_Absyn_Exp*e1=_T2F8;struct Cyc_Absyn_Exp*e2=_T2F7;struct Cyc_Absyn_Stmt*guard_s=_T2F6;struct Cyc_Absyn_Exp*e3=_T2F5;struct Cyc_Absyn_Stmt*cont_s=_T2F4;struct Cyc_Absyn_Stmt*s=_T2F3;
# 178
Cyc_Tcexp_tcExp(te,0,e1);_T39=te;_T3A=e2;_T3B=
_tag_fat("for loop",sizeof(char),9U);Cyc_Tcexp_tcTest(_T39,_T3A,_T3B);
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14:{struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F8=_T2F9->f1;_T3C=_T2F9->f2;_T2F7=_T3C.f0;_T3D=_T2F9->f2;_T2F6=_T3D.f1;}{struct Cyc_Absyn_Stmt*s=_T2F8;struct Cyc_Absyn_Exp*e=_T2F7;struct Cyc_Absyn_Stmt*cont_s=_T2F6;
# 186
Cyc_Tcstmt_tcStmt(te,s,1);_T3E=te;_T3F=e;_T40=
_tag_fat("do loop",sizeof(char),8U);Cyc_Tcexp_tcTest(_T3E,_T3F,_T40);
return;}case 6: goto _LL14;case 7: _LL14: goto _LL16;case 8: _LL16:
# 192
 return;case 13:{struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F8=_T2F9->f1;_T2F7=_T2F9->f2;}{struct _fat_ptr*l=_T2F8;struct Cyc_Absyn_Stmt*s=_T2F7;
# 197
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar));_T41=tv;{struct _fat_ptr*_T2F9=_cycalloc(sizeof(struct _fat_ptr));{struct Cyc_String_pa_PrintArg_struct _T2FA;_T2FA.tag=0;_T45=l;_T2FA.f1=*_T45;_T44=_T2FA;}{struct Cyc_String_pa_PrintArg_struct _T2FA=_T44;void*_T2FB[1];_T46=_T2FB + 0;*_T46=& _T2FA;_T47=_tag_fat("`%s",sizeof(char),4U);_T48=_tag_fat(_T2FB,sizeof(void*),1);_T43=Cyc_aprintf(_T47,_T48);}*_T2F9=_T43;_T42=(struct _fat_ptr*)_T2F9;}_T41->name=_T42;_T49=tv;
_T49->identity=Cyc_Tcutil_new_tvar_id();_T4A=tv;_T4B=& Cyc_Kinds_ek;_T4C=(struct Cyc_Absyn_Kind*)_T4B;_T4A->kind=Cyc_Kinds_kind_to_bound(_T4C);_T4D=tv;_T4D->aquals_bound=0;_T4E=s0;_T4F=_T4E->loc;_T50=te;_T51=tv;_T52=
Cyc_Tcenv_new_named_block(_T4F,_T50,_T51);_T53=s;Cyc_Tcstmt_tcStmt(_T52,_T53,0);
return;}case 11:{struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F8=_T2F9->f1;_T54=(struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_T2F1;_T55=& _T54->f2;_T2F7=(struct Cyc_Absyn_Switch_clause***)_T55;}{struct Cyc_List_List*es=_T2F8;struct Cyc_Absyn_Switch_clause***clauseopt=_T2F7;
# 203
const struct _tuple14*trip_opt=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);
if(trip_opt!=0)goto _TL39;{struct Cyc_Warn_String_Warn_Warg_struct _T2F9;_T2F9.tag=0;
_T2F9.f1=_tag_fat("fallthru in a last case",sizeof(char),24U);_T56=_T2F9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F9=_T56;void*_T2FA[1];_T57=_T2FA + 0;*_T57=& _T2F9;_T58=s0;_T59=_T58->loc;_T5A=_tag_fat(_T2FA,sizeof(void*),1);Cyc_Warn_err2(_T59,_T5A);}
return;_TL39: _T5B=trip_opt;{
# 208
struct _tuple14 _T2F9=*_T5B;struct Cyc_List_List*_T2FA;struct Cyc_List_List*_T2FB;_T2FB=_T2F9.f1;_T2FA=_T2F9.f2;{struct Cyc_List_List*tvs=_T2FB;struct Cyc_List_List*ts_orig=_T2FA;_T5D=Cyc_List_map_c;{
struct Cyc_List_List*(*_T2FC)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*))_T5D;_T5C=_T2FC;}_T5E=Cyc_Tcutil_make_inst_var;_T5F=Cyc_Tcenv_lookup_type_vars(te);_T60=tvs;{struct Cyc_List_List*instantiation=_T5C(_T5E,_T5F,_T60);_T62=Cyc_List_map_c;{
struct Cyc_List_List*(*_T2FC)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*))_T62;_T61=_T2FC;}_T63=Cyc_Tcutil_substitute;_T64=instantiation;_T65=ts_orig;{struct Cyc_List_List*ts=_T61(_T63,_T64,_T65);
_TL3E: if(ts!=0)goto _TL3F;else{goto _TL3D;}_TL3F: if(es!=0)goto _TL3C;else{goto _TL3D;}
# 213
_TL3C: _T66=te;_T67=es;_T68=_T67->hd;_T69=(struct Cyc_Absyn_Exp*)_T68;Cyc_Tcexp_tcExp(_T66,0,_T69);_T6A=
Cyc_Tcenv_curr_aquals_bounds(te);_T6B=es;_T6C=_T6B->hd;_T6D=(struct Cyc_Absyn_Exp*)_T6C;_T6E=ts;_T6F=_T6E->hd;_T70=Cyc_Tcutil_coerce_arg(_T6A,_T6D,_T6F,0);if(_T70)goto _TL40;else{goto _TL42;}
_TL42:{struct Cyc_Warn_String_Warn_Warg_struct _T2FC;_T2FC.tag=0;_T2FC.f1=_tag_fat("fallthru argument has type ",sizeof(char),28U);_T71=_T2FC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FC=_T71;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2FD;_T2FD.tag=3;_T73=es;_T74=_T73->hd;_T75=(struct Cyc_Absyn_Exp*)_T74;_T2FD.f1=_T75->topt;_T72=_T2FD;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2FD=_T72;{struct Cyc_Warn_String_Warn_Warg_struct _T2FE;_T2FE.tag=0;
_T2FE.f1=_tag_fat("but pattern variable has type ",sizeof(char),31U);_T76=_T2FE;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FE=_T76;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FF;_T2FF.tag=2;_T78=ts;_T2FF.f1=_T78->hd;_T77=_T2FF;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FF=_T77;void*_T300[4];_T79=_T300 + 0;*_T79=& _T2FC;_T7A=_T300 + 1;*_T7A=& _T2FD;_T7B=_T300 + 2;*_T7B=& _T2FE;_T7C=_T300 + 3;*_T7C=& _T2FF;_T7D=s0;_T7E=_T7D->loc;_T7F=_tag_fat(_T300,sizeof(void*),4);Cyc_Warn_err2(_T7E,_T7F);}}}}
Cyc_Unify_explain_failure();goto _TL41;_TL40: _TL41: _T80=es;_T81=_T80->hd;_T82=(struct Cyc_Absyn_Exp*)_T81;_T83=_T82->loc;_T84=
# 219
Cyc_Tcenv_curr_aquals_bounds(te);_T85=ts;_T86=_T85->hd;_T87=es;_T88=_T87->hd;_T89=(struct Cyc_Absyn_Exp*)_T88;Cyc_Tcexp_check_consume(_T83,_T84,_T86,_T89);_T8A=ts;
# 211
ts=_T8A->tl;_T8B=es;es=_T8B->tl;goto _TL3E;_TL3D:
# 221
 if(es==0)goto _TL43;{struct Cyc_Warn_String_Warn_Warg_struct _T2FC;_T2FC.tag=0;
_T2FC.f1=_tag_fat("too many arguments in explicit fallthru",sizeof(char),40U);_T8C=_T2FC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FC=_T8C;void*_T2FD[1];_T8D=_T2FD + 0;*_T8D=& _T2FC;_T8E=s0;_T8F=_T8E->loc;_T90=_tag_fat(_T2FD,sizeof(void*),1);Cyc_Warn_err2(_T8F,_T90);}goto _TL44;_TL43: _TL44:
 if(ts==0)goto _TL45;{struct Cyc_Warn_String_Warn_Warg_struct _T2FC;_T2FC.tag=0;
_T2FC.f1=_tag_fat("too few arguments in explicit fallthru",sizeof(char),39U);_T91=_T2FC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FC=_T91;void*_T2FD[1];_T92=_T2FD + 0;*_T92=& _T2FC;_T93=s0;_T94=_T93->loc;_T95=_tag_fat(_T2FD,sizeof(void*),1);Cyc_Warn_err2(_T94,_T95);}goto _TL46;_TL45: _TL46:
 return;}}}}}case 10:{struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T2F1;_T96=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T2F1;_T97=& _T96->f1;_T2F8=(struct Cyc_Absyn_Exp**)_T97;_T2F7=_T2F9->f2;_T98=(struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_T2F1;_T99=& _T98->f3;_T2F6=(void**)_T99;}{struct Cyc_Absyn_Exp**exp=_T2F8;struct Cyc_List_List*scs0=_T2F7;void**dtp=(void**)_T2F6;_T9A=
# 229
Cyc_Tcstmt_make_var_exp(exp,s0);if(!_T9A)goto _TL47;
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;_TL47: _T9B=exp;{
# 234
struct Cyc_Absyn_Exp*e=*_T9B;
void*t=Cyc_Tcexp_tcExp(te,0,e);_T9C=e;_T9D=_T9C->loc;_T9E=
# 237
Cyc_Tcenv_curr_aquals_bounds(te);_T9F=t;_TA0=e;Cyc_Tcexp_check_consume(_T9D,_T9E,_T9F,_TA0);{
# 241
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;{
struct Cyc_List_List*scs=scs0;_TL4C: if(scs!=0)goto _TL4A;else{goto _TL4B;}
_TL4A: _TA1=first_case;if(!_TA1)goto _TL4D;
first_case=0;_TA2=te;_TA3=scs;_TA4=_TA3->hd;_TA5=(struct Cyc_Absyn_Switch_clause*)_TA4;_TA6=_TA5->pattern;_TA7=& t;_TA8=e;
this_pattern=Cyc_Tcpat_tcPat(_TA2,_TA6,_TA7,_TA8);goto _TL4E;
# 249
_TL4D: _TA9=_check_null(next_pattern);this_pattern=*_TA9;_TL4E: _TAA=scs;_TAB=_TAA->tl;
# 251
if(_TAB==0)goto _TL4F;{struct Cyc_Tcpat_TcPatResult*_T2F9=_cycalloc(sizeof(struct Cyc_Tcpat_TcPatResult));_TAD=te;_TAE=scs;_TAF=_TAE->tl;_TB0=_TAF->hd;_TB1=(struct Cyc_Absyn_Switch_clause*)_TB0;_TB2=_TB1->pattern;_TB3=& t;_TB4=e;
*_T2F9=Cyc_Tcpat_tcPat(_TAD,_TB2,_TB3,_TB4);_TAC=(struct Cyc_Tcpat_TcPatResult*)_T2F9;}next_pattern=_TAC;_TB5=next_pattern;_TB6=_TB5->tvars_and_effconstr_opt;
if(_TB6==0)goto _TL51;_TB7=next_pattern;_TB8=_TB7->tvars_and_effconstr_opt;_TB9=*_TB8;_TBA=_TB9.f1;if(_TBA==0)goto _TL51;
# 255
te=Cyc_Tcenv_clear_fallthru(te);goto _TL52;
# 257
_TL51: _TBB=next_pattern;_TBC=_TBB->patvars;_TBD=Cyc_List_split(_TBC);_TBE=_TBD.f0;_TBF=Cyc_Tcutil_filter_nulls(_TBE);{struct Cyc_List_List*vs=Cyc_List_imp_rev(_TBF);_TC1=next_pattern;_TC2=_TC1->tvars_and_effconstr_opt;
if(_TC2!=0)goto _TL53;_TC0=0;goto _TL54;_TL53: _TC4=Cyc_List_map;{
struct Cyc_List_List*(*_T2F9)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))_TC4;_TC3=_T2F9;}_TC6=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T2F9)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))_TC6;_TC5=_T2F9;}_TC7=next_pattern;_TC8=_TC7->tvars_and_effconstr_opt;_TC9=*_TC8;_TCA=_TC9.f0;_TC0=_TC3(_TC5,_TCA);_TL54: {
# 258
struct Cyc_List_List*new_tvs=_TC0;_TCB=te;_TCC=new_tvs;_TCD=vs;_TCE=scs;_TCF=_TCE->tl;_TD0=
# 260
_check_null(_TCF);_TD1=_TD0->hd;_TD2=(struct Cyc_Absyn_Switch_clause*)_TD1;te=Cyc_Tcenv_set_fallthru(_TCB,_TCC,_TCD,_TD2);}}_TL52: goto _TL50;
# 263
_TL4F: te=Cyc_Tcenv_clear_fallthru(te);_TL50: _TD3=scs;_TD4=_TD3->hd;_TD5=(struct Cyc_Absyn_Switch_clause*)_TD4;{
# 265
struct Cyc_Absyn_Pat*p=_TD5->pattern;_TD6=scs;_TD7=_TD6->hd;_TD8=(struct Cyc_Absyn_Switch_clause*)_TD7;_TD9=_TD8->loc;_TDA=t;_TDB=p;_TDC=_TDB->topt;_TDD=
_check_null(_TDC);{enum Cyc_Absyn_Coercion _c=Cyc_Subtype_coercible_exact(_TD9,_TDA,_TDD);_TDE=_c;_TDF=(int)_TDE;
if(_TDF!=0)goto _TL55;{struct Cyc_Warn_String_Warn_Warg_struct _T2F9;_T2F9.tag=0;
# 269
_T2F9.f1=_tag_fat("switch on type ",sizeof(char),16U);_TE0=_T2F9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F9=_TE0;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FA;_T2FA.tag=2;_T2FA.f1=t;_TE1=_T2FA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FA=_TE1;{struct Cyc_Warn_String_Warn_Warg_struct _T2FB;_T2FB.tag=0;_T2FB.f1=_tag_fat(" but case expects type ",sizeof(char),24U);_TE2=_T2FB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FB=_TE2;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2FC;_T2FC.tag=3;_TE4=p;_T2FC.f1=_TE4->topt;_TE3=_T2FC;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2FC=_TE3;void*_T2FD[4];_TE5=_T2FD + 0;*_TE5=& _T2F9;_TE6=_T2FD + 1;*_TE6=& _T2FA;_TE7=_T2FD + 2;*_TE7=& _T2FB;_TE8=_T2FD + 3;*_TE8=& _T2FC;_TE9=scs;_TEA=_TE9->hd;_TEB=(struct Cyc_Absyn_Switch_clause*)_TEA;_TEC=_TEB->loc;_TED=_tag_fat(_T2FD,sizeof(void*),4);Cyc_Warn_err2(_TEC,_TED);}}}}
Cyc_Unify_explain_failure();goto _TL56;
# 273
_TL55: _TEE=te;_TEF=p;_TF0=& this_pattern;_TF1=(struct Cyc_Tcpat_TcPatResult*)_TF0;Cyc_Tcpat_check_pat_regions(_TEE,_TEF,_TF1);_TL56: _TF2=scs;_TF3=_TF2->hd;_TF4=(struct Cyc_Absyn_Switch_clause*)_TF3;{struct Cyc_Core_Opt*_T2F9=_cycalloc(sizeof(struct Cyc_Core_Opt));_TF6=this_pattern;
# 275
_T2F9->v=_TF6.patvars;_TF5=(struct Cyc_Core_Opt*)_T2F9;}_TF4->pat_vars=_TF5;_TF7=scs;_TF8=_TF7->hd;_TF9=(struct Cyc_Absyn_Switch_clause*)_TF8;_TFA=_TF9->loc;_TFB=te;_TFC=this_pattern;_TFD=scs;_TFE=_TFD->hd;_TFF=(struct Cyc_Absyn_Switch_clause*)_TFE;_T100=_TFF->body;_T101=scs;_T102=_T101->hd;_T103=(struct Cyc_Absyn_Switch_clause*)_T102;_T104=_T103->where_clause;
Cyc_Tcstmt_pattern_synth(_TFA,_TFB,_TFC,_T100,_T104,1);}}_T105=scs;
# 244
scs=_T105->tl;goto _TL4C;_TL4B:;}_T106=s0;_T107=_T106->loc;_T108=te;_T109=scs0;_T10A=dtp;
# 279
Cyc_Tcpat_check_switch_exhaustive(_T107,_T108,_T109,_T10A);
return;}}}case 15:{struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F4=_T2F9->f1;_T10B=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T2F1;_T10C=& _T10B->f2;_T2F8=(struct Cyc_List_List**)_T10C;_T10D=(struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_T2F1;_T10E=& _T10D->f3;_T2F7=(void**)_T10E;}{struct Cyc_Absyn_Stmt*s=_T2F4;struct Cyc_List_List**scs0=(struct Cyc_List_List**)_T2F8;void**dtp=(void**)_T2F7;_T10F=s;{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T2F9=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_T2F9->tag=2;_T111=s;_T112=_T111->r;_T113=s;_T114=_T113->loc;
# 287
_T2F9->f1=Cyc_Absyn_new_stmt(_T112,_T114);_T115=s;_T116=_T115->loc;_T2F9->f2=Cyc_Absyn_skip_stmt(_T116);_T110=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T2F9;}_T117=(void*)_T110;_T118=s;_T119=_T118->loc;_T11A=Cyc_Absyn_new_stmt(_T117,_T119);_T10F->r=_T11A->r;
# 289
Cyc_Tcstmt_tcStmt(te,s,1);_T11C=Cyc_List_exists;{
# 291
int(*_T2F9)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))_T11C;_T11B=_T2F9;}_T11D=scs0;_T11E=*_T11D;_T11F=_T11B(Cyc_Tcstmt_is_case_default,_T11E);if(_T11F)goto _TL57;else{goto _TL59;}
# 293
_TL59:{struct _tuple2*def_v;def_v=_cycalloc(sizeof(struct _tuple2));_T120=def_v;_T120->f0=Cyc_Absyn_Loc_n();_T121=def_v;{struct _fat_ptr*_T2F9=_cycalloc(sizeof(struct _fat_ptr));*_T2F9=_tag_fat("exn",sizeof(char),4U);_T122=(struct _fat_ptr*)_T2F9;}_T121->f1=_T122;{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0,0);{struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_T2F9=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_T2F9->tag=1;
_T2F9->f1=vd;_T124=& Cyc_Absyn_Wild_p_val;_T125=(struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct*)_T124;_T126=(void*)_T125;_T2F9->f2=Cyc_Absyn_new_pat(_T126,0U);_T123=(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_T2F9;}_T127=(void*)_T123;{struct Cyc_Absyn_Pat*def_pat=Cyc_Absyn_new_pat(_T127,0U);{struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_T2F9=_cycalloc(sizeof(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct));_T2F9->tag=11;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T2FA=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_T2FA->tag=4;
# 297
_T2FA->f1=vd;_T129=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_T2FA;}_T12A=(void*)_T129;_T2F9->f1=Cyc_Absyn_varb_exp(_T12A,0U);_T2F9->f2=1;_T128=(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_T2F9;}_T12B=(void*)_T128;_T12C=Cyc_Absyn_new_exp(_T12B,0U);{
# 296
struct Cyc_Absyn_Stmt*def_stmt=
Cyc_Absyn_exp_stmt(_T12C,0U);
struct Cyc_Absyn_Switch_clause*def_clause;def_clause=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause));_T12D=def_clause;_T12D->pattern=def_pat;_T12E=def_clause;_T12E->pat_vars=0;_T12F=def_clause;_T12F->where_clause=0;_T130=def_clause;_T130->body=def_stmt;_T131=def_clause;_T131->loc=0U;_T132=scs0;_T133=scs0;_T134=*_T133;{struct Cyc_Absyn_Switch_clause*_T2F9[1];_T136=_T2F9 + 0;*_T136=def_clause;_T137=_tag_fat(_T2F9,sizeof(struct Cyc_Absyn_Switch_clause*),1);_T135=Cyc_List_list(_T137);}
*_T132=Cyc_List_append(_T134,_T135);}}}}goto _TL58;_TL57: _TL58: {
# 305
void*exception_type=Cyc_Absyn_exn_type();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;_T138=scs0;{
struct Cyc_List_List*scs=*_T138;_TL5D: if(scs!=0)goto _TL5B;else{goto _TL5C;}
_TL5B: _T139=first_case;if(!_T139)goto _TL5E;
first_case=0;_T13A=te;_T13B=scs;_T13C=_T13B->hd;_T13D=(struct Cyc_Absyn_Switch_clause*)_T13C;_T13E=_T13D->pattern;_T13F=& exception_type;
this_pattern=Cyc_Tcpat_tcPat(_T13A,_T13E,_T13F,0);goto _TL5F;
# 314
_TL5E: _T140=_check_null(next_pattern);this_pattern=*_T140;_TL5F: _T141=scs;_T142=_T141->tl;
# 316
if(_T142==0)goto _TL60;{struct Cyc_Tcpat_TcPatResult*_T2F9=_cycalloc(sizeof(struct Cyc_Tcpat_TcPatResult));_T144=te;_T145=scs;_T146=_T145->tl;_T147=_T146->hd;_T148=(struct Cyc_Absyn_Switch_clause*)_T147;_T149=_T148->pattern;_T14A=& exception_type;
*_T2F9=Cyc_Tcpat_tcPat(_T144,_T149,_T14A,0);_T143=(struct Cyc_Tcpat_TcPatResult*)_T2F9;}next_pattern=_T143;_T14B=next_pattern;_T14C=_T14B->tvars_and_effconstr_opt;
if(_T14C==0)goto _TL62;_T14D=next_pattern;_T14E=_T14D->tvars_and_effconstr_opt;_T14F=*_T14E;_T150=_T14F.f1;if(_T150==0)goto _TL62;
# 320
te=Cyc_Tcenv_clear_fallthru(te);goto _TL63;
# 322
_TL62: _T151=next_pattern;_T152=_T151->patvars;_T153=Cyc_List_split(_T152);_T154=_T153.f0;_T155=Cyc_Tcutil_filter_nulls(_T154);{struct Cyc_List_List*vs=Cyc_List_imp_rev(_T155);_T157=next_pattern;_T158=_T157->tvars_and_effconstr_opt;
if(_T158!=0)goto _TL64;_T156=0;goto _TL65;_TL64: _T15A=Cyc_List_map;{
struct Cyc_List_List*(*_T2F9)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))_T15A;_T159=_T2F9;}_T15C=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T2F9)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))_T15C;_T15B=_T2F9;}_T15D=next_pattern;_T15E=_T15D->tvars_and_effconstr_opt;_T15F=*_T15E;_T160=_T15F.f0;_T156=_T159(_T15B,_T160);_TL65: {
# 323
struct Cyc_List_List*new_tvs=_T156;_T161=te;_T162=new_tvs;_T163=vs;_T164=scs;_T165=_T164->tl;_T166=
# 325
_check_null(_T165);_T167=_T166->hd;_T168=(struct Cyc_Absyn_Switch_clause*)_T167;te=Cyc_Tcenv_set_fallthru(_T161,_T162,_T163,_T168);}}_TL63: goto _TL61;
# 328
_TL60: te=Cyc_Tcenv_clear_fallthru(te);_TL61: _T169=scs;_T16A=_T169->hd;_T16B=(struct Cyc_Absyn_Switch_clause*)_T16A;{
# 330
struct Cyc_Absyn_Pat*p=_T16B->pattern;_T16C=scs;_T16D=_T16C->hd;_T16E=(struct Cyc_Absyn_Switch_clause*)_T16D;_T16F=_T16E->loc;_T170=exception_type;_T171=p;_T172=_T171->topt;_T173=
_check_null(_T172);{enum Cyc_Absyn_Coercion _c=Cyc_Subtype_coercible_exact(_T16F,_T170,_T173);_T174=_c;_T175=(int)_T174;
if(_T175!=0)goto _TL66;{struct Cyc_Warn_String_Warn_Warg_struct _T2F9;_T2F9.tag=0;
_T2F9.f1=_tag_fat("switch on type ",sizeof(char),16U);_T176=_T2F9;}{struct Cyc_Warn_String_Warn_Warg_struct _T2F9=_T176;{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FA;_T2FA.tag=2;_T2FA.f1=exception_type;_T177=_T2FA;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T2FA=_T177;{struct Cyc_Warn_String_Warn_Warg_struct _T2FB;_T2FB.tag=0;
_T2FB.f1=_tag_fat(" but case expects type ",sizeof(char),24U);_T178=_T2FB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FB=_T178;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2FC;_T2FC.tag=3;_T17A=p;_T2FC.f1=_T17A->topt;_T179=_T2FC;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T2FC=_T179;void*_T2FD[4];_T17B=_T2FD + 0;*_T17B=& _T2F9;_T17C=_T2FD + 1;*_T17C=& _T2FA;_T17D=_T2FD + 2;*_T17D=& _T2FB;_T17E=_T2FD + 3;*_T17E=& _T2FC;_T17F=scs;_T180=_T17F->hd;_T181=(struct Cyc_Absyn_Switch_clause*)_T180;_T182=_T181->loc;_T183=_tag_fat(_T2FD,sizeof(void*),4);Cyc_Warn_err2(_T182,_T183);}}}}
Cyc_Unify_explain_failure();goto _TL67;
# 338
_TL66: _T184=te;_T185=p;_T186=& this_pattern;_T187=(struct Cyc_Tcpat_TcPatResult*)_T186;Cyc_Tcpat_check_pat_regions(_T184,_T185,_T187);_TL67: _T188=scs;_T189=_T188->hd;_T18A=(struct Cyc_Absyn_Switch_clause*)_T189;{struct Cyc_Core_Opt*_T2F9=_cycalloc(sizeof(struct Cyc_Core_Opt));_T18C=this_pattern;
# 340
_T2F9->v=_T18C.patvars;_T18B=(struct Cyc_Core_Opt*)_T2F9;}_T18A->pat_vars=_T18B;_T18D=scs;_T18E=_T18D->hd;_T18F=(struct Cyc_Absyn_Switch_clause*)_T18E;_T190=_T18F->loc;_T191=te;_T192=this_pattern;_T193=scs;_T194=_T193->hd;_T195=(struct Cyc_Absyn_Switch_clause*)_T194;_T196=_T195->body;_T197=scs;_T198=_T197->hd;_T199=(struct Cyc_Absyn_Switch_clause*)_T198;_T19A=_T199->where_clause;
Cyc_Tcstmt_pattern_synth(_T190,_T191,_T192,_T196,_T19A,1);}}_T19B=scs;
# 309
scs=_T19B->tl;goto _TL5D;_TL5C:;}_T19C=s0;_T19D=_T19C->loc;_T19E=te;_T19F=scs0;_T1A0=*_T19F;_T1A1=dtp;
# 344
Cyc_Tcpat_check_catch_overlap(_T19D,_T19E,_T1A0,_T1A1);
return;}}default:{struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*_T2F9=(struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_T2F1;_T2F2=_T2F9->f1;_T2F4=_T2F9->f2;}{struct Cyc_Absyn_Decl*d=_T2F2;struct Cyc_Absyn_Stmt*s=_T2F4;
# 348
struct _fat_ptr unimp_msg_part;_T1A3=new_block;
if(!_T1A3)goto _TL68;_T1A4=s0;_T1A5=_T1A4->loc;_T1A6=te;_T1A2=Cyc_Tcenv_new_block(_T1A5,_T1A6);goto _TL69;_TL68: _T1A2=te;_TL69: {struct Cyc_Tcenv_Tenv*_T2F9=_T1A2;struct Cyc_Tcenv_Tenv*te=_T2F9;_T1A7=d;{
void*_T2FA=_T1A7->r;struct _tuple2*_T2FB;struct _fat_ptr*_T2FC;struct Cyc_Absyn_Fndecl*_T2FD;struct Cyc_Absyn_Exp*_T2FE;struct Cyc_Absyn_Vardecl*_T2FF;struct Cyc_Absyn_Tvar*_T300;struct Cyc_List_List*_T301;void*_T302;void*_T303;struct Cyc_Absyn_Pat*_T304;void*_T305;_T1A8=(int*)_T2FA;_T1A9=*_T1A8;switch(_T1A9){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T306=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T2FA;_T305=_T306->f1;}{struct Cyc_Absyn_Vardecl*vd=_T305;void*_T306;struct Cyc_Absyn_Exp*_T307;void*_T308;struct Cyc_Absyn_Tqual _T309;enum Cyc_Absyn_Scope _T30A;_T1AA=vd;{struct Cyc_Absyn_Vardecl _T30B=*_T1AA;_T30A=_T30B.sc;_T309=_T30B.tq;_T308=_T30B.type;_T307=_T30B.initializer;_T1AB=vd;_T1AC=& _T1AB->rgn;_T306=(void**)_T1AC;}{enum Cyc_Absyn_Scope sc=_T30A;struct Cyc_Absyn_Tqual tq=_T309;void*t=_T308;struct Cyc_Absyn_Exp*initializer=_T307;void**rgn_ptr=(void**)_T306;
# 353
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
int is_local;_T1AD=sc;_T1AE=(int)_T1AD;switch(_T1AE){case Cyc_Absyn_Static: goto _LL4E;case Cyc_Absyn_Extern: _LL4E: goto _LL50;case Cyc_Absyn_ExternC: _LL50: _T1AF=vd;
# 358
_T1AF->escapes=1;is_local=0;goto _LL4A;case Cyc_Absyn_Abstract:{struct Cyc_Warn_String_Warn_Warg_struct _T30B;_T30B.tag=0;
# 360
_T30B.f1=_tag_fat("bad abstract scope for local variable",sizeof(char),38U);_T1B0=_T30B;}{struct Cyc_Warn_String_Warn_Warg_struct _T30B=_T1B0;void*_T30C[1];_T1B1=_T30C + 0;*_T1B1=& _T30B;_T1B2=d;_T1B3=_T1B2->loc;_T1B4=_tag_fat(_T30C,sizeof(void*),1);Cyc_Warn_err2(_T1B3,_T1B4);}goto _LL54;case Cyc_Absyn_Register: _LL54: goto _LL56;case Cyc_Absyn_Public: _LL56: goto _LL58;default: _LL58:
# 364
 is_local=1;goto _LL4A;}_LL4A: _T1B5=rgn_ptr;_T1B7=is_local;
# 367
if(!_T1B7)goto _TL6C;_T1B6=curr_bl;goto _TL6D;_TL6C: _T1B6=Cyc_Absyn_heap_rgn_type;_TL6D:*_T1B5=_T1B6;{
void*_T30B=Cyc_Absyn_compress(t);struct Cyc_Absyn_ArrayInfo _T30C;_T1B8=(int*)_T30B;_T1B9=*_T1B8;switch(_T1B9){case 6: _T1BA=is_local;if(!_T1BA)goto _TL6F;_T1BB=vd;
# 371
_T1BB->escapes=1;
sc=3U;
is_local=0;goto _LL59;_TL6F: goto _LL5E;case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T30D=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T30B;_T30C=_T30D->f1;}{struct Cyc_Absyn_ArrayInfo ai=_T30C;_T1BC=vd;_T1BD=_T1BC->initializer;
# 377
if(_T1BD==0)goto _TL73;else{goto _TL74;}_TL74: _T1BE=ai;_T1BF=_T1BE.num_elts;if(_T1BF!=0)goto _TL73;else{goto _TL71;}
_TL73: goto _LL59;_TL71: {
struct Cyc_Absyn_Exp*len;_T1C0=vd;_T1C1=_T1C0->initializer;{
void*_T30D=_T1C1->r;struct Cyc_List_List*_T30E;struct Cyc_Absyn_Exp*_T30F;struct _fat_ptr _T310;_T1C2=(int*)_T30D;_T1C3=*_T1C2;switch(_T1C3){case 0: _T1C4=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T30D;_T1C5=_T1C4->f1;_T1C6=_T1C5.Wstring_c;_T1C7=_T1C6.tag;switch(_T1C7){case 8:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T311=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T30D;_T1C8=_T311->f1;_T1C9=_T1C8.String_c;_T310=_T1C9.val;}{struct _fat_ptr s=_T310;_T1CA=s;_T1CB=
_get_fat_size(_T1CA,sizeof(char));len=Cyc_Absyn_uint_exp(_T1CB,0U);goto _LL60;}case 9:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T311=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T30D;_T1CC=_T311->f1;_T1CD=_T1CC.Wstring_c;_T310=_T1CD.val;}{struct _fat_ptr s=_T310;
len=Cyc_Absyn_uint_exp(1U,0U);goto _LL60;}default: goto _LL6D;};case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T311=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T30D;_T30F=_T311->f2;}{struct Cyc_Absyn_Exp*e=_T30F;_T30F=e;goto _LL68;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T311=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_T30D;_T30F=_T311->f1;}_LL68: {struct Cyc_Absyn_Exp*e=_T30F;
# 384
len=e;goto _LL60;}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_T311=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_T30D;_T30E=_T311->f2;}{struct Cyc_List_List*es=_T30E;_T30E=es;goto _LL6C;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T311=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_T30D;_T30E=_T311->f1;}_LL6C: {struct Cyc_List_List*es=_T30E;_T1CE=
# 386
Cyc_List_length(es);_T1CF=(unsigned)_T1CE;len=Cyc_Absyn_uint_exp(_T1CF,0U);goto _LL60;}default: _LL6D:
 len=0;goto _LL60;}_LL60:;}
# 389
ai.num_elts=len;_T1D0=vd;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T30D=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_T30D->tag=5;
_T30D->f1=ai;_T1D1=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T30D;}_T1D0->type=(void*)_T1D1;t=_T1D0->type;goto _LL59;}}default: _LL5E: goto _LL59;}_LL59:;}_T1D3=is_local;
# 397
if(_T1D3)goto _TL77;else{goto _TL79;}_TL79: _T1D2=0;goto _TL78;_TL77: _T1D2=Cyc_Tcenv_lookup_type_vars(te);_TL78: {struct Cyc_List_List*bound_vars=_T1D2;
int allow_evars=is_local;_T1D4=s0;_T1D5=_T1D4->loc;_T1D6=te;_T1D7=bound_vars;_T1D8=& Cyc_Kinds_mk;_T1D9=(struct Cyc_Absyn_Kind*)_T1D8;_T1DA=allow_evars;_T1DB=t;
Cyc_Tctyp_check_type(_T1D5,_T1D6,_T1D7,_T1D9,_T1DA,1,_T1DB);_T1DC=vd;_T1DD=s0;_T1DE=_T1DD->loc;_T1DF=vd;_T1E0=_T1DF->tq;_T1E1=_T1E0.print_const;_T1E2=t;
_T1DC->tq.real_const=Cyc_Tcutil_extract_const_from_typedef(_T1DE,_T1E1,_T1E2);_T1E3=sc;_T1E4=(int)_T1E3;
# 402
if(_T1E4==3)goto _TL7C;else{goto _TL7D;}_TL7D: _T1E5=sc;_T1E6=(int)_T1E5;if(_T1E6==4)goto _TL7C;else{goto _TL7A;}
_TL7C:{struct Cyc_Warn_String_Warn_Warg_struct _T30B;_T30B.tag=0;_T30B.f1=_tag_fat("extern declarations are not yet supported within functions",sizeof(char),59U);_T1E7=_T30B;}{struct Cyc_Warn_String_Warn_Warg_struct _T30B=_T1E7;void*_T30C[1];_T1E8=_T30C + 0;*_T1E8=& _T30B;_T1E9=d;_T1EA=_T1E9->loc;_T1EB=_tag_fat(_T30C,sizeof(void*),1);Cyc_Warn_err2(_T1EA,_T1EB);}goto _TL7B;_TL7A: _TL7B:
 if(initializer==0)goto _TL7E;_T1EC=te;_T1ED=& t;_T1EE=initializer;
Cyc_Tcexp_tcExpInitializer(_T1EC,_T1ED,_T1EE);_T1EF=is_local;
# 407
if(_T1EF)goto _TL80;else{goto _TL82;}_TL82: _T1F0=Cyc_Tcutil_is_const_exp(initializer);if(_T1F0)goto _TL80;else{goto _TL83;}
_TL83:{struct Cyc_Warn_String_Warn_Warg_struct _T30B;_T30B.tag=0;_T30B.f1=_tag_fat("initializer for static variable needs to be a constant expression",sizeof(char),66U);_T1F1=_T30B;}{struct Cyc_Warn_String_Warn_Warg_struct _T30B=_T1F1;void*_T30C[1];_T1F2=_T30C + 0;*_T1F2=& _T30B;_T1F3=d;_T1F4=_T1F3->loc;_T1F5=_tag_fat(_T30C,sizeof(void*),1);Cyc_Warn_err2(_T1F4,_T1F5);}goto _TL81;_TL80: _TL81: _T1F6=
Cyc_Tcenv_curr_aquals_bounds(te);_T1F7=initializer;_T1F8=t;_T1F9=Cyc_Tcutil_coerce_assign(_T1F6,_T1F7,_T1F8);if(_T1F9)goto _TL84;else{goto _TL86;}
_TL86:{struct Cyc_Warn_Vardecl_Warn_Warg_struct _T30B;_T30B.tag=11;_T30B.f1=vd;_T1FA=_T30B;}{struct Cyc_Warn_Vardecl_Warn_Warg_struct _T30B=_T1FA;{struct Cyc_Warn_String_Warn_Warg_struct _T30C;_T30C.tag=0;_T30C.f1=_tag_fat(" was declared with type ",sizeof(char),25U);_T1FB=_T30C;}{struct Cyc_Warn_String_Warn_Warg_struct _T30C=_T1FB;{struct Cyc_Warn_Typ_Warn_Warg_struct _T30D;_T30D.tag=2;_T30D.f1=t;_T1FC=_T30D;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T30D=_T1FC;{struct Cyc_Warn_String_Warn_Warg_struct _T30E;_T30E.tag=0;
_T30E.f1=_tag_fat(" but initialized with type ",sizeof(char),28U);_T1FD=_T30E;}{struct Cyc_Warn_String_Warn_Warg_struct _T30E=_T1FD;{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T30F;_T30F.tag=3;_T1FF=initializer;_T30F.f1=_T1FF->topt;_T1FE=_T30F;}{struct Cyc_Warn_TypOpt_Warn_Warg_struct _T30F=_T1FE;void*_T310[5];_T200=_T310 + 0;*_T200=& _T30B;_T201=_T310 + 1;*_T201=& _T30C;_T202=_T310 + 2;*_T202=& _T30D;_T203=_T310 + 3;*_T203=& _T30E;_T204=_T310 + 4;*_T204=& _T30F;_T205=d;_T206=_T205->loc;_T207=_tag_fat(_T310,sizeof(void*),5);Cyc_Warn_err2(_T206,_T207);}}}}}_T208=t;_T209=initializer;_T20A=_T209->topt;_T20B=
_check_null(_T20A);Cyc_Unify_unify(_T208,_T20B);
Cyc_Unify_explain_failure();goto _TL85;_TL84: _TL85: goto _TL7F;_TL7E: _TL7F:
# 416
 Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}case 2:{struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*_T306=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T2FA;_T304=_T306->f1;_T20C=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T2FA;_T20D=& _T20C->f2;_T305=(struct Cyc_Core_Opt**)_T20D;_T20E=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T2FA;_T20F=& _T20E->f3;_T303=(struct Cyc_Absyn_Exp**)_T20F;_T210=(struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_T2FA;_T211=& _T210->f4;_T302=(void**)_T211;}{struct Cyc_Absyn_Pat*p=_T304;struct Cyc_Core_Opt**vds=(struct Cyc_Core_Opt**)_T305;struct Cyc_Absyn_Exp**exp=(struct Cyc_Absyn_Exp**)_T303;void**dtp=(void**)_T302;_T212=p;{
# 421
void*_T306=_T212->r;_T213=(int*)_T306;_T214=*_T213;switch(_T214){case 1: goto _LL73;case 2: _LL73: goto _LL75;case 14: _LL75: goto _LL6F;default: _T215=
# 426
Cyc_Tcstmt_make_var_exp(exp,s0);if(!_T215)goto _TL88;
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;_TL88:;}_LL6F:;}_T216=exp;{
# 432
struct Cyc_Absyn_Exp*e=*_T216;
Cyc_Tcexp_tcExpInitializer(te,0,e);_T217=e;_T218=_T217->topt;{
# 435
void*pat_type=_check_null(_T218);_T219=te;_T21A=p;_T21B=& pat_type;_T21C=e;{
# 437
struct Cyc_Tcpat_TcPatResult pat_res=Cyc_Tcpat_tcPat(_T219,_T21A,_T21B,_T21C);_T21D=vds;{struct Cyc_Core_Opt*_T306=_cycalloc(sizeof(struct Cyc_Core_Opt));_T21F=pat_res;
_T306->v=_T21F.patvars;_T21E=(struct Cyc_Core_Opt*)_T306;}*_T21D=_T21E;_T220=p;_T221=_T220->topt;{
void*pt=_check_null(_T221);_T222=e;_T223=_T222->topt;{
void*et=_check_null(_T223);_T224=
Cyc_Unify_unify(pt,et);if(_T224)goto _TL8A;else{goto _TL8C;}_TL8C: _T225=Cyc_Tcenv_curr_aquals_bounds(te);_T226=e;_T227=pt;_T228=Cyc_Tcutil_coerce_assign(_T225,_T226,_T227);if(_T228)goto _TL8A;else{goto _TL8D;}
_TL8D:{struct Cyc_Warn_String_Warn_Warg_struct _T306;_T306.tag=0;
_T306.f1=_tag_fat("pattern type ",sizeof(char),14U);_T229=_T306;}{struct Cyc_Warn_String_Warn_Warg_struct _T306=_T229;{struct Cyc_Warn_Typ_Warn_Warg_struct _T307;_T307.tag=2;_T307.f1=pt;_T22A=_T307;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T307=_T22A;{struct Cyc_Warn_String_Warn_Warg_struct _T308;_T308.tag=0;_T308.f1=_tag_fat(" does not match definition type ",sizeof(char),33U);_T22B=_T308;}{struct Cyc_Warn_String_Warn_Warg_struct _T308=_T22B;{struct Cyc_Warn_Typ_Warn_Warg_struct _T309;_T309.tag=2;_T309.f1=et;_T22C=_T309;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T309=_T22C;void*_T30A[4];_T22D=_T30A + 0;*_T22D=& _T306;_T22E=_T30A + 1;*_T22E=& _T307;_T22F=_T30A + 2;*_T22F=& _T308;_T230=_T30A + 3;*_T230=& _T309;_T231=d;_T232=_T231->loc;_T233=_tag_fat(_T30A,sizeof(void*),4);Cyc_Warn_err2(_T232,_T233);}}}}
Cyc_Unify_unify(pt,et);
Cyc_Unify_explain_failure();goto _TL8B;
# 448
_TL8A: _T234=te;_T235=p;_T236=& pat_res;_T237=(struct Cyc_Tcpat_TcPatResult*)_T236;Cyc_Tcpat_check_pat_regions(_T234,_T235,_T237);_TL8B: _T238=p;_T239=_T238->loc;_T23A=te;_T23B=p;_T23C=dtp;
Cyc_Tcpat_check_let_pat_exhaustive(_T239,_T23A,_T23B,_T23C);_T23D=s0;_T23E=_T23D->loc;_T23F=te;_T240=pat_res;_T241=s;
Cyc_Tcstmt_pattern_synth(_T23E,_T23F,_T240,_T241,0,0);
return;}}}}}}case 3:{struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*_T306=(struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_T2FA;_T301=_T306->f1;}{struct Cyc_List_List*vds=_T301;
# 454
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
_TL91: if(vds!=0)goto _TL8F;else{goto _TL90;}
_TL8F: _T242=vds;_T243=_T242->hd;_T244=(struct Cyc_Absyn_Vardecl*)_T243;_T244->rgn=curr_bl;_T245=s0;_T246=_T245->loc;_T247=te;_T248=
Cyc_Tcenv_lookup_type_vars(te);_T249=& Cyc_Kinds_mk;_T24A=(struct Cyc_Absyn_Kind*)_T249;_T24B=vds;_T24C=_T24B->hd;_T24D=(struct Cyc_Absyn_Vardecl*)_T24C;_T24E=_T24D->type;Cyc_Tctyp_check_type(_T246,_T247,_T248,_T24A,1,1,_T24E);_T24F=vds;
# 455
vds=_T24F->tl;goto _TL91;_TL90:
# 460
 Cyc_Tcstmt_tcStmt(te,s,0);
return;}case 4:{struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*_T306=(struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_T2FA;_T300=_T306->f1;_T2FF=_T306->f2;_T2FE=_T306->f3;}{struct Cyc_Absyn_Tvar*tv=_T300;struct Cyc_Absyn_Vardecl*vd=_T2FF;struct Cyc_Absyn_Exp*open_exp_opt=_T2FE;_T250=s;{struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_T306=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_T306->tag=2;_T252=s;_T253=_T252->r;_T254=s;_T255=_T254->loc;
# 466
_T306->f1=Cyc_Absyn_new_stmt(_T253,_T255);_T256=s;_T257=_T256->loc;_T306->f2=Cyc_Absyn_skip_stmt(_T257);_T251=(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_T306;}_T258=(void*)_T251;_T259=s;_T25A=_T259->loc;_T25B=Cyc_Absyn_new_stmt(_T258,_T25A);_T250->r=_T25B->r;{
# 468
struct Cyc_Tcenv_Tenv*te2=te;void*_T306;void*_T307;_T25C=vd;{struct Cyc_Absyn_Vardecl _T308=*_T25C;_T25D=vd;_T25E=& _T25D->type;_T307=(void**)_T25E;_T25F=vd;_T260=& _T25F->rgn;_T306=(void**)_T260;}{void**t=(void**)_T307;void**rgn_ptr=(void**)_T306;
# 470
void*curr_bl=Cyc_Tcenv_curr_rgn(te);_T261=rgn_ptr;
*_T261=curr_bl;{
void*rgn_typ;_T262=open_exp_opt;_T263=(unsigned)_T262;
if(!_T263)goto _TL92;{
# 476
struct _tuple2*drname;drname=_cycalloc(sizeof(struct _tuple2));_T264=drname;{struct _fat_ptr*_T308[1];_T266=_T308 + 0;{struct _fat_ptr*_T309=_cycalloc(sizeof(struct _fat_ptr));*_T309=_tag_fat("Core",sizeof(char),5U);_T267=(struct _fat_ptr*)_T309;}*_T266=_T267;_T268=_tag_fat(_T308,sizeof(struct _fat_ptr*),1);_T265=Cyc_List_list(_T268);}_T264->f0=Cyc_Absyn_Abs_n(_T265,0);_T269=drname;{struct _fat_ptr*_T308=_cycalloc(sizeof(struct _fat_ptr));*_T308=_tag_fat("DynamicRegion",sizeof(char),14U);_T26A=(struct _fat_ptr*)_T308;}_T269->f1=_T26A;{struct Cyc_Core_Opt*_T308=_cycalloc(sizeof(struct Cyc_Core_Opt));_T26C=& Cyc_Kinds_ek;
_T308->v=(struct Cyc_Absyn_Kind*)_T26C;_T26B=(struct Cyc_Core_Opt*)_T308;}{void*outer_rgn=Cyc_Absyn_new_evar(_T26B,0);{struct Cyc_Core_Opt*_T308=_cycalloc(sizeof(struct Cyc_Core_Opt));_T26E=& Cyc_Kinds_ek;
_T308->v=(struct Cyc_Absyn_Kind*)_T26E;_T26D=(struct Cyc_Core_Opt*)_T308;}rgn_typ=Cyc_Absyn_new_evar(_T26D,0);{struct Cyc_Core_Opt*_T308=_cycalloc(sizeof(struct Cyc_Core_Opt));_T270=& Cyc_Kinds_aqk;
_T308->v=(struct Cyc_Absyn_Kind*)_T270;_T26F=(struct Cyc_Core_Opt*)_T308;}{void*aq=Cyc_Absyn_new_evar(_T26F,0);_T271=
Cyc_Absyn_UnknownAggr(0U,drname,0);{void*_T308[1];_T273=_T308 + 0;*_T273=rgn_typ;_T274=_tag_fat(_T308,sizeof(void*),1);_T272=Cyc_List_list(_T274);}{void*dr_type=Cyc_Absyn_aggr_type(_T271,_T272);_T275=dr_type;_T276=outer_rgn;_T277=aq;_T278=
Cyc_Absyn_empty_tqual(0U);_T279=Cyc_Absyn_false_type;_T27A=Cyc_Absyn_false_type;{void*exp_type=Cyc_Absyn_at_type(_T275,_T276,_T277,_T278,_T279,_T27A);_T27B=s0;_T27C=_T27B->loc;_T27D=te;_T27E=
Cyc_Tcenv_lookup_type_vars(te);_T27F=& Cyc_Kinds_mk;_T280=(struct Cyc_Absyn_Kind*)_T27F;_T281=exp_type;Cyc_Tctyp_check_type(_T27C,_T27D,_T27E,_T280,1,0,_T281);_T282=te;_T283=& exp_type;_T284=open_exp_opt;{
# 484
void*key_typ=Cyc_Tcexp_tcExp(_T282,_T283,_T284);_T285=
Cyc_Unify_unify(exp_type,key_typ);if(_T285)goto _TL94;else{goto _TL96;}_TL96: _T286=
Cyc_Tcenv_curr_aquals_bounds(te);_T287=open_exp_opt;_T288=exp_type;_T289=Cyc_Tcutil_coerce_assign(_T286,_T287,_T288);
# 485
if(_T289)goto _TL94;else{goto _TL97;}
# 487
_TL97:{struct Cyc_Warn_String_Warn_Warg_struct _T308;_T308.tag=0;_T308.f1=_tag_fat("expected ",sizeof(char),10U);_T28A=_T308;}{struct Cyc_Warn_String_Warn_Warg_struct _T308=_T28A;{struct Cyc_Warn_Typ_Warn_Warg_struct _T309;_T309.tag=2;_T309.f1=exp_type;_T28B=_T309;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T309=_T28B;{struct Cyc_Warn_String_Warn_Warg_struct _T30A;_T30A.tag=0;_T30A.f1=_tag_fat(" but found ",sizeof(char),12U);_T28C=_T30A;}{struct Cyc_Warn_String_Warn_Warg_struct _T30A=_T28C;{struct Cyc_Warn_Typ_Warn_Warg_struct _T30B;_T30B.tag=2;_T30B.f1=key_typ;_T28D=_T30B;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T30B=_T28D;void*_T30C[4];_T28E=_T30C + 0;*_T28E=& _T308;_T28F=_T30C + 1;*_T28F=& _T309;_T290=_T30C + 2;*_T290=& _T30A;_T291=_T30C + 3;*_T291=& _T30B;_T292=s0;_T293=_T292->loc;_T294=_tag_fat(_T30C,sizeof(void*),4);Cyc_Warn_err2(_T293,_T294);}}}}goto _TL95;_TL94: _TL95: _T295=
# 490
Cyc_Tcutil_is_aliasable_qual(aq);if(!_T295)goto _TL98;{struct Cyc_Warn_String_Warn_Warg_struct _T308;_T308.tag=0;
_T308.f1=_tag_fat("open is allowed only on unique or reference-counted keys",sizeof(char),57U);_T296=_T308;}{struct Cyc_Warn_String_Warn_Warg_struct _T308=_T296;void*_T309[1];_T297=_T309 + 0;*_T297=& _T308;_T298=s0;_T299=_T298->loc;_T29A=_tag_fat(_T309,sizeof(void*),1);Cyc_Warn_err2(_T299,_T29A);}goto _TL99;_TL98: _TL99: _T29B=t;
*_T29B=Cyc_Absyn_rgn_handle_type(rgn_typ);
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1,0);}}}}}}goto _TL93;
# 495
_TL92:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T308=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_T308->tag=2;_T308->f1=tv;_T29C=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T308;}rgn_typ=(void*)_T29C;_T29D=s0;_T29E=_T29D->loc;_T29F=te2;{struct Cyc_List_List*_T308=_cycalloc(sizeof(struct Cyc_List_List));
_T308->hd=tv;_T308->tl=0;_T2A0=(struct Cyc_List_List*)_T308;}te2=Cyc_Tcenv_add_type_vars(_T29E,_T29F,_T2A0);
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0,1);_TL93: _T2A1=s0;_T2A2=_T2A1->loc;_T2A3=te;_T2A4=
# 499
Cyc_Tcenv_lookup_type_vars(te2);_T2A5=& Cyc_Kinds_bk;_T2A6=(struct Cyc_Absyn_Kind*)_T2A5;_T2A7=t;_T2A8=*_T2A7;Cyc_Tctyp_check_type(_T2A2,_T2A3,_T2A4,_T2A6,1,0,_T2A8);_T2A9=t;_T2AA=*_T2A9;_T2AB=
Cyc_Absyn_rgn_handle_type(rgn_typ);_T2AC=Cyc_Unify_unify(_T2AA,_T2AB);if(_T2AC)goto _TL9A;else{goto _TL9C;}
_TL9C: _T2AE=Cyc_Warn_impos;{int(*_T308)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T2AE;_T2AD=_T308;}_T2AF=_tag_fat("region stmt: type of region handle is wrong!",sizeof(char),45U);_T2B0=_tag_fat(0U,sizeof(void*),0);_T2AD(_T2AF,_T2B0);goto _TL9B;_TL9A: _TL9B:
 Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T306=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T2FA;_T2FD=_T306->f1;}{struct Cyc_Absyn_Fndecl*fd=_T2FD;_T2B1=fd;_T2B2=_T2B1->sc;_T2B3=(int)_T2B2;
# 506
if(_T2B3==2)goto _TL9D;{struct Cyc_Warn_String_Warn_Warg_struct _T306;_T306.tag=0;
_T306.f1=_tag_fat("bad storage class for inner function",sizeof(char),37U);_T2B4=_T306;}{struct Cyc_Warn_String_Warn_Warg_struct _T306=_T2B4;void*_T307[1];_T2B5=_T307 + 0;*_T2B5=& _T306;_T2B6=d;_T2B7=_T2B6->loc;_T2B8=_tag_fat(_T307,sizeof(void*),1);Cyc_Warn_err2(_T2B7,_T2B8);}goto _TL9E;_TL9D: _TL9E: _T2B9=d;_T2BA=_T2B9->loc;_T2BB=fd;_T2BC=_T2BB->i;_T2BD=_T2BC.attributes;
Cyc_Atts_check_fndecl_atts(_T2BA,_T2BD,1);_T2BE=d;_T2BF=_T2BE->loc;_T2C0=te;_T2C1=fd;
Cyc_Tctyp_check_fndecl_valid_type(_T2BF,_T2C0,_T2C1);{
void*t=Cyc_Tcutil_fndecl2type(fd);_T2C2=fd;_T2C3=t;_T2C4=fd;_T2C5=_T2C4->i;_T2C6=_T2C5.attributes;
_T2C2->i.attributes=Cyc_Atts_transfer_fn_type_atts(_T2C3,_T2C6);{
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
struct Cyc_Absyn_Vardecl*vd;vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl));_T2C7=vd;_T2C8=fd;_T2C7->sc=_T2C8->sc;_T2C9=vd;_T2CA=fd;_T2C9->name=_T2CA->name;_T2CB=vd;_T2CB->varloc=0U;_T2CC=vd;_T2CC->tq=Cyc_Absyn_const_tqual(0U);_T2CD=vd;_T2CE=t;_T2CF=curr_bl;_T2D0=Cyc_Absyn_al_qual_type;_T2D1=
Cyc_Absyn_empty_tqual(0U);_T2D2=Cyc_Absyn_false_type;_T2D3=Cyc_Absyn_false_type;_T2CD->type=Cyc_Absyn_at_type(_T2CE,_T2CF,_T2D0,_T2D1,_T2D2,_T2D3);_T2D4=vd;
# 516
_T2D4->initializer=0;_T2D5=vd;_T2D5->rgn=curr_bl;_T2D6=vd;_T2D6->attributes=0;_T2D7=vd;_T2D7->escapes=0;_T2D8=vd;_T2D9=fd;_T2DA=_T2D9->body;
_T2D8->is_proto=_T2DA!=0;_T2DB=vd;
_T2DB->rename=0;_T2DC=fd;
_T2DC->fn_vardecl=vd;_T2DD=te;_T2DE=_T2DD->le;{
struct Cyc_Tcenv_Fenv*old_fenv=_check_null(_T2DE);_T2DF=te;_T2E0=d;_T2E1=_T2E0->loc;_T2E2=old_fenv;_T2E3=fd;
_T2DF->le=Cyc_Tcenv_nested_fenv(_T2E1,_T2E2,_T2E3);_T2E4=te;_T2E5=fd;_T2E6=_T2E5->body;
Cyc_Tcstmt_tcStmt(_T2E4,_T2E6,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);_T2E7=te;
_T2E7->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T306=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T2FA;_T2FC=_T306->f1;_T301=_T306->f2;}{struct _fat_ptr*n=_T2FC;struct Cyc_List_List*tds=_T301;
unimp_msg_part=_tag_fat("namespace",sizeof(char),10U);goto _LL24;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T306=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T2FA;_T2FB=_T306->f1;_T301=_T306->f2;}{struct _tuple2*q=_T2FB;struct Cyc_List_List*tds=_T301;
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
 unimp_msg_part=_tag_fat("__tempest_off__",sizeof(char),16U);goto _LL24;}_LL24:;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FA;_T2FA.tag=0;
# 541
_T2FA.f1=_tag_fat("tcStmt: nested ",sizeof(char),16U);_T2E8=_T2FA;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FA=_T2E8;{struct Cyc_Warn_String_Warn_Warg_struct _T2FB;_T2FB.tag=0;_T2FB.f1=unimp_msg_part;_T2E9=_T2FB;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FB=_T2E9;{struct Cyc_Warn_String_Warn_Warg_struct _T2FC;_T2FC.tag=0;_T2FC.f1=_tag_fat(" declarations not allowed",sizeof(char),26U);_T2EA=_T2FC;}{struct Cyc_Warn_String_Warn_Warg_struct _T2FC=_T2EA;void*_T2FD[3];_T2EB=_T2FD + 0;*_T2EB=& _T2FA;_T2EC=_T2FD + 1;*_T2EC=& _T2FB;_T2ED=_T2FD + 2;*_T2ED=& _T2FC;_T2EF=Cyc_Warn_impos2;{int(*_T2FE)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T2EF;_T2EE=_T2FE;}_T2F0=_tag_fat(_T2FD,sizeof(void*),3);_T2EE(_T2F0);}}}}}};}}
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
static int Cyc_Tcstmt_ensure_no_throw_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;int(*_T3)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*);int(*_T4)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T5;int _T6;int _T7;int _T8;int _T9;struct Cyc_Absyn_Exp*_TA;int*_TB;unsigned _TC;void*_TD;int _TE;struct Cyc_Absyn_Exp*_TF;int*_T10;int _T11;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T12;struct Cyc_Absyn_PtrInfo _T13;void*_T14;int*_T15;int _T16;struct Cyc_Absyn_PtrInfo _T17;void*_T18;int(*_T19)(void*,void*);struct Cyc_Absyn_FnInfo _T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_T1C;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_T1D;void*_T1E;struct Cyc_Warn_String_Warn_Warg_struct _T1F;void**_T20;int(*_T21)(struct _fat_ptr);void*(*_T22)(struct _fat_ptr);struct _fat_ptr _T23;int _T24;int _T25;int(*_T26)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*);int(*_T27)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*_T28;void*_T29;void*_T2A;int*_T2B;unsigned _T2C;int _T2D;int(*_T2E)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*);int(*_T2F)(int(*)(void*),struct Cyc_List_List*);struct Cyc_List_List*(*_T30)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*);struct Cyc_List_List*(*_T31)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Exp*(*_T32)(struct _tuple17*);void*(*_T33)(struct _tuple0*);struct Cyc_List_List*_T34;struct Cyc_List_List*_T35;int _T36;void*_T37;int _T38;int _T39;struct Cyc_Absyn_Exp*_T3A;int*_T3B;int _T3C;struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T3D;struct Cyc_Absyn_PtrInfo*_T3E;struct Cyc_Absyn_PtrInfo*_T3F;struct Cyc_Absyn_PtrAtts _T40;void*_T41;unsigned _T42;int _T43;_T0=e;{
void*_T44=_T0->r;enum Cyc_Absyn_Coercion _T45;int _T46;struct Cyc_Absyn_VarargCallInfo*_T47;struct Cyc_List_List*_T48;struct Cyc_Absyn_Exp*_T49;void*_T4A;_T1=(int*)_T44;_T2=*_T1;switch(_T2){case 0:
 return 1;case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T44;_T4A=_T4B->f2;}{struct Cyc_List_List*es=_T4A;_T4=Cyc_List_forall;{
# 573
int(*_T4B)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))_T4;_T3=_T4B;}_T5=es;_T6=_T3(Cyc_Tcstmt_ensure_no_throw_exp,_T5);return _T6;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T44;_T4A=_T4B->f1;_T49=_T4B->f2;}{struct Cyc_Absyn_Exp*e1=_T4A;struct Cyc_Absyn_Exp*e2=_T49;_T4A=e1;_T49=e2;goto _LL8;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T44;_T4A=_T4B->f1;_T49=_T4B->f2;}_LL8: {struct Cyc_Absyn_Exp*e1=_T4A;struct Cyc_Absyn_Exp*e2=_T49;_T4A=e1;_T49=e2;goto _LLA;}case 34:{struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_T44;_T4A=_T4B->f1;_T49=_T4B->f2;}_LLA: {struct Cyc_Absyn_Exp*e1=_T4A;struct Cyc_Absyn_Exp*e2=_T49;_T4A=e1;_T49=e2;goto _LLC;}case 9:{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T44;_T4A=_T4B->f1;_T49=_T4B->f2;}_LLC: {struct Cyc_Absyn_Exp*e1=_T4A;struct Cyc_Absyn_Exp*e2=_T49;_T8=
# 577
Cyc_Tcstmt_ensure_no_throw_exp(e1);if(!_T8)goto _TLAB;_T7=Cyc_Tcstmt_ensure_no_throw_exp(e2);goto _TLAC;_TLAB: _T7=0;_TLAC: return _T7;}case 10:{struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_T44;_T4A=_T4B->f1;_T48=_T4B->f2;_T47=_T4B->f3;_T46=_T4B->f4;}{struct Cyc_Absyn_Exp*fname=_T4A;struct Cyc_List_List*args=_T48;struct Cyc_Absyn_VarargCallInfo*vargs=_T47;int res=_T46;_T9=res;
# 579
if(_T9)goto _TLAD;else{goto _TLAF;}
_TLAF: return 0;_TLAD: {
int fname_ok=0;_TA=fname;{
void*_T4B=_TA->r;struct Cyc_List_List*_T4C;void*_T4D;_TB=(int*)_T4B;_TC=*_TB;switch(_TC){case 13:{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T4E=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_T4B;_T4D=_T4E->f1;_T4C=_T4E->f2;}{struct Cyc_Absyn_Exp*e1=_T4D;struct Cyc_List_List*ts=_T4C;
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(e1);goto _LL1D;}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T4E=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T4B;_TD=_T4E->f1;_T4D=(void*)_TD;}{void*b=_T4D;
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(fname);goto _LL1D;}default: goto _LL1D;}_LL1D:;}_TE=fname_ok;
# 587
if(!_TE)goto _TLB1;_TF=fname;{
void*_T4B=_TF->topt;struct Cyc_Absyn_FnInfo _T4C;if(_T4B==0)goto _TLB3;_T10=(int*)_T4B;_T11=*_T10;if(_T11!=4)goto _TLB5;_T12=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4B;_T13=_T12->f1;_T14=_T13.elt_type;_T15=(int*)_T14;_T16=*_T15;if(_T16!=6)goto _TLB7;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4B;_T17=_T4D->f1;_T18=_T17.elt_type;{struct Cyc_Absyn_FnType_Absyn_Type_struct*_T4E=(struct Cyc_Absyn_FnType_Absyn_Type_struct*)_T18;_T4C=_T4E->f1;}}{struct Cyc_Absyn_FnInfo fi=_T4C;_T19=Cyc_Atts_attribute_cmp;_T1A=fi;_T1B=_T1A.attributes;_T1C=& Cyc_Atts_No_throw_att_val;_T1D=(struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*)_T1C;_T1E=(void*)_T1D;
# 590
fname_ok=Cyc_List_mem(_T19,_T1B,_T1E);goto _LL24;}_TLB7: goto _LL27;_TLB5: goto _LL27;_TLB3: _LL27:{struct Cyc_Warn_String_Warn_Warg_struct _T4D;_T4D.tag=0;
# 593
_T4D.f1=_tag_fat("Expected function pointer type",sizeof(char),31U);_T1F=_T4D;}{struct Cyc_Warn_String_Warn_Warg_struct _T4D=_T1F;void*_T4E[1];_T20=_T4E + 0;*_T20=& _T4D;_T22=Cyc_Warn_impos2;{int(*_T4F)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T22;_T21=_T4F;}_T23=_tag_fat(_T4E,sizeof(void*),1);_T21(_T23);}_LL24:;}goto _TLB2;_TLB1: _TLB2: _T25=fname_ok;
# 595
if(!_T25)goto _TLB9;_T27=Cyc_List_forall;{int(*_T4B)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))_T27;_T26=_T4B;}_T28=args;_T24=_T26(Cyc_Tcstmt_ensure_no_throw_exp,_T28);goto _TLBA;_TLB9: _T24=0;_TLBA: return _T24;}}case 1:{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T44;_T29=_T4B->f1;_T4A=(void*)_T29;}{void*b=_T4A;_T2A=b;_T2B=(int*)_T2A;_T2C=*_T2B;switch(_T2C){case 3: goto _LL2D;case 4: _LL2D:
# 620 "tcstmt.cyc"
 return 1;default:
 return 0;};}case 15:{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_T44;_T4A=_T4B->f1;}{struct Cyc_Absyn_Exp*e1=_T4A;_T2D=
# 623
Cyc_Tcstmt_ensure_no_throw_exp(e1);return _T2D;}case 28:{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_T44;_T4A=_T4B->f3;}{struct Cyc_List_List*exps=_T4A;_T2F=Cyc_List_forall;{
# 625
int(*_T4B)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))_T2F;_T2E=_T4B;}_T31=Cyc_List_map;{struct Cyc_List_List*(*_T4B)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*))_T31;_T30=_T4B;}_T33=Cyc_Core_snd;{struct Cyc_Absyn_Exp*(*_T4B)(struct _tuple17*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))_T33;_T32=_T4B;}_T34=exps;_T35=_T30(_T32,_T34);_T36=_T2E(Cyc_Tcstmt_ensure_no_throw_exp,_T35);return _T36;}case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T44;_T37=_T4B->f1;_T4A=(void*)_T37;_T49=_T4B->f2;_T46=_T4B->f3;_T45=_T4B->f4;}{void*ct=_T4A;struct Cyc_Absyn_Exp*e1=_T49;int b=_T46;enum Cyc_Absyn_Coercion c=_T45;_T39=b;
# 627
if(_T39)goto _TLBC;else{goto _TLBE;}_TLBE: _T38=Cyc_Tcstmt_ensure_no_throw_exp(e1);goto _TLBD;_TLBC: _T38=0;_TLBD: return _T38;}case 20:{struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_T44;_T49=_T4B->f1;}{struct Cyc_Absyn_Exp*e1=_T49;_T49=e1;goto _LL1A;}case 22:{struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_T4B=(struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_T44;_T49=_T4B->f1;}_LL1A: {struct Cyc_Absyn_Exp*e1=_T49;_T3A=e1;{
# 630
void*_T4B=_T3A->topt;void*_T4C;if(_T4B==0)goto _TLBF;_T3B=(int*)_T4B;_T3C=*_T3B;if(_T3C!=4)goto _TLC1;{struct Cyc_Absyn_PointerType_Absyn_Type_struct*_T4D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4B;_T3D=(struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_T4B;_T3E=& _T3D->f1;_T4C=(struct Cyc_Absyn_PtrInfo*)_T3E;}{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_T4C;_T3F=pi;_T40=_T3F->ptr_atts;_T41=_T40.nullable;_T42=(unsigned)_T41;_T43=!_T42;
return _T43;}_TLC1: goto _LL33;_TLBF: _LL33:
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
