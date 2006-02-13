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
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple2{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 145 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 167
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 186 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 192
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*eff;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*autoreleased;void*aqual;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple4{enum Cyc_Absyn_AggrKind f0;struct _tuple2*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple4 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 337
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple2*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple5{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple5 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple6{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple7 val;};struct _tuple8{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple8 val;};struct _tuple9{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple9 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 529 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};
# 536
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};
# 554
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple11{struct Cyc_Absyn_Exp*f0;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple11 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple11 f2;struct _tuple11 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple11 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};
# 736 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple2*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple2*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple2*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple2*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 931
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
# 934
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 937
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 944
void*Cyc_Absyn_compress(void*);
# 957
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 971
extern void*Cyc_Absyn_heap_rgn_type;
# 973
extern void*Cyc_Absyn_al_qual_type;
# 977
extern void*Cyc_Absyn_false_type;
# 979
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_rgn_handle_type(void*);
# 1002
void*Cyc_Absyn_exn_type (void);
# 1028
void*Cyc_Absyn_at_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1045
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*);
# 1060
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
# 1070
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1077
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1139
struct Cyc_Absyn_Stmt*Cyc_Absyn_new_stmt(void*,unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_skip_stmt(unsigned);
struct Cyc_Absyn_Stmt*Cyc_Absyn_exp_stmt(struct Cyc_Absyn_Exp*,unsigned);
# 1152
struct Cyc_Absyn_Stmt*Cyc_Absyn_decl_stmt(struct Cyc_Absyn_Decl*,struct Cyc_Absyn_Stmt*,unsigned);
# 1161
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1166
struct Cyc_Absyn_Decl*Cyc_Absyn_let_decl(struct Cyc_Absyn_Pat*,struct Cyc_Absyn_Exp*,unsigned);
# 1170
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned,struct _tuple2*,void*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 1214
int Cyc_Absyn_no_side_effects_exp(struct Cyc_Absyn_Exp*);
# 1243
void Cyc_Absyn_visit_stmt(int(*)(void*,struct Cyc_Absyn_Exp*),int(*)(void*,struct Cyc_Absyn_Stmt*),void*,struct Cyc_Absyn_Stmt*);
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
int Cyc_Unify_unify(void*,void*);
# 31 "tcutil.h"
int Cyc_Tcutil_is_void_type(void*);
# 33
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
# 106
int Cyc_Tcutil_coerce_arg(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*,int*);
int Cyc_Tcutil_coerce_assign(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
# 132 "tcutil.h"
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 160
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);struct _tuple13{struct Cyc_Absyn_Tvar*f0;void*f1;};
# 164
struct _tuple13*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*);
# 218
int Cyc_Tcutil_new_tvar_id (void);
# 227
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 233
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 243
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*);
# 275
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
static void Cyc_Tcstmt_simplify_unused_result_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;void*_Tmp1;if(*((int*)_Tmp0)==5)switch((int)((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f2){case Cyc_Absyn_PostInc: _Tmp1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
({void*_Tmp2=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_Tmp3->tag=5,_Tmp3->f1=e1,_Tmp3->f2=0U;_Tmp3;});e->r=_Tmp2;});goto _LL0;}case Cyc_Absyn_PostDec: _Tmp1=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp1;
({void*_Tmp2=(void*)({struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct));_Tmp3->tag=5,_Tmp3->f1=e1,_Tmp3->f2=2U;_Tmp3;});e->r=_Tmp2;});goto _LL0;}default: goto _LL5;}else{_LL5:
 goto _LL0;}_LL0:;}
# 50
static struct _tuple2*Cyc_Tcstmt_stmt_temp_var (void){
static int counter=0;
struct _tuple2*_Tmp0=_cycalloc(sizeof(struct _tuple2));_Tmp0->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp3=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _Tmp4=({struct Cyc_Int_pa_PrintArg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=(unsigned)counter ++;_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("_tmpS%X",sizeof(char),8U),_tag_fat(_Tmp5,sizeof(void*),1));});*_Tmp2=_Tmp3;});_Tmp2;});_Tmp0->f1=_Tmp1;});return _Tmp0;}
# 57
static int Cyc_Tcstmt_make_var_exp(struct Cyc_Absyn_Exp**eptr,struct Cyc_Absyn_Stmt*s0){
struct Cyc_Absyn_Exp*e2=*eptr;
int done=0;
while(!done){
{void*_Tmp0=e2->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 1:
 return 0;case 12: _Tmp1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e3=_Tmp1;
_Tmp1=e3;goto _LL6;}case 13: _Tmp1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL6: {struct Cyc_Absyn_Exp*e3=_Tmp1;
e2=e3;continue;}default:
 done=1;};}
# 61
1U;}{
# 67
struct Cyc_Absyn_Exp*e=*eptr;
struct Cyc_Absyn_Vardecl*vd=({struct _tuple2*_Tmp0=Cyc_Tcstmt_stmt_temp_var();Cyc_Absyn_new_vardecl(0U,_Tmp0,Cyc_Absyn_new_evar(0,0),0,0);});
struct Cyc_Absyn_Decl*d=({struct Cyc_Absyn_Pat*_Tmp0=({void*_Tmp1=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp2->tag=1,_Tmp2->f1=vd,({struct Cyc_Absyn_Pat*_Tmp3=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,e->loc);_Tmp2->f2=_Tmp3;});_Tmp2;});Cyc_Absyn_new_pat(_Tmp1,e->loc);});struct Cyc_Absyn_Exp*_Tmp1=e;Cyc_Absyn_let_decl(_Tmp0,_Tmp1,s0->loc);});
# 71
({void*_Tmp0=({struct Cyc_Absyn_Decl*_Tmp1=d;struct Cyc_Absyn_Stmt*_Tmp2=Cyc_Absyn_new_stmt(s0->r,s0->loc);Cyc_Absyn_decl_stmt(_Tmp1,_Tmp2,s0->loc);})->r;s0->r=_Tmp0;});
({struct Cyc_Absyn_Exp*_Tmp0=({void*_Tmp1=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp2->tag=4,_Tmp2->f1=vd;_Tmp2;});Cyc_Absyn_varb_exp(_Tmp1,e->loc);});*eptr=_Tmp0;});
return 1;}}struct _tuple15{struct Cyc_Absyn_Tvar*f0;int f1;};struct _tuple16{struct Cyc_Absyn_Vardecl**f0;struct Cyc_Absyn_Exp*f1;};
# 77
static void Cyc_Tcstmt_pattern_synth(unsigned loc,struct Cyc_Tcenv_Tenv*te,struct Cyc_Tcpat_TcPatResult pat_res,struct Cyc_Absyn_Stmt*s,struct Cyc_Absyn_Exp*where_opt,int new_block){
# 79
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=pat_res.tvars_and_effconstr_opt;_Tmp1=pat_res.patvars;_Tmp0=pat_res.aquals_bounds;{struct _tuple1*p=_Tmp2;struct Cyc_List_List*vse=_Tmp1;struct Cyc_List_List*aqb=_Tmp0;
struct Cyc_List_List*vs=({struct Cyc_List_List*(*_Tmp3)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(({struct Cyc_Absyn_Vardecl**(*_Tmp3)(struct _tuple16*)=(struct Cyc_Absyn_Vardecl**(*)(struct _tuple16*))Cyc_Core_fst;_Tmp3;}),vse);
struct Cyc_List_List*tvs=p==0?0:({struct Cyc_List_List*(*_Tmp3)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(({struct Cyc_Absyn_Tvar*(*_Tmp3)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst;_Tmp3;}),(*p).f0);
struct Cyc_List_List*ecs=p==0?0:(*p).f1;
struct Cyc_List_List*rgns=p==0?0:({struct Cyc_List_List*(*_Tmp3)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=({struct Cyc_List_List*(*_Tmp4)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;_Tmp4;});struct Cyc_Absyn_Tvar*(*_Tmp4)(struct _tuple15*)=({struct Cyc_Absyn_Tvar*(*_Tmp5)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst;_Tmp5;});_Tmp3(_Tmp4,({struct Cyc_List_List*(*_Tmp5)(int(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_filter;_Tmp5;})(({int(*_Tmp5)(struct _tuple15*)=(int(*)(struct _tuple15*))Cyc_Core_snd;_Tmp5;}),(*p).f0));});
te=Cyc_Tcenv_add_type_vars(loc,te,tvs);
for(1;rgns!=0;rgns=rgns->tl){
te=({struct Cyc_Tcenv_Tenv*_Tmp3=te;Cyc_Tcenv_add_region(_Tmp3,(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_Tmp4->tag=2,_Tmp4->f1=(struct Cyc_Absyn_Tvar*)rgns->hd;_Tmp4;}),0,0);});}
te=Cyc_Tcenv_new_effect_constraints(te,ecs,loc);
# 89
if(new_block)
te=Cyc_Tcenv_new_block(loc,te);{
void*ropt=Cyc_Tcenv_curr_rgn(te);
for(1;vs!=0;vs=vs->tl){
if((struct Cyc_Absyn_Vardecl**)vs->hd!=0)
(*((struct Cyc_Absyn_Vardecl**)vs->hd))->rgn=ropt;}
for(1;aqb!=0;aqb=aqb->tl){
struct _tuple0*_Tmp3=(struct _tuple0*)aqb->hd;void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3->f0;_Tmp4=_Tmp3->f1;{void*tv=_Tmp5;void*b=_Tmp4;
void*_Tmp6=Cyc_Absyn_compress(tv);if(*((int*)_Tmp6)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp6)->f1)==15){
# 99
te=Cyc_Tcenv_add_aquals_bound(te,tv,b);
goto _LL6;}else{goto _LL9;}}else{_LL9:
# 102
 goto _LL6;}_LL6:;}}
# 105
if(where_opt!=0){
Cyc_Tcexp_tcTest(te,where_opt,_tag_fat("switch clause guard",sizeof(char),20U));
if(!Cyc_Absyn_no_side_effects_exp(where_opt))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("cannot show &&-clause in pattern has no effects",sizeof(char),48U);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_Warn_err2(where_opt->loc,_tag_fat(_Tmp4,sizeof(void*),1));});}
# 110
Cyc_Tcstmt_tcStmt(te,s,0);}}}
# 113
static int Cyc_Tcstmt_is_case_default(struct Cyc_Absyn_Switch_clause*c){
void*_Tmp0=c->pattern->r;if(*((int*)_Tmp0)==0)
return 1;else{
return 0;};}
# 128 "tcstmt.cyc"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Stmt*s0,int new_block){
# 130
void*_Tmp0=s0->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0)){case 0:
# 132
 return;case 1: _Tmp6=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e=_Tmp6;
# 135
Cyc_Tcexp_tcExp(te,0,e);
if(!Cyc_Tcenv_in_stmt_exp(te))
Cyc_Tcstmt_simplify_unused_result_exp(e);
return;}case 2: _Tmp6=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Stmt*s1=_Tmp6;struct Cyc_Absyn_Stmt*s2=_Tmp5;
# 141
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 3: _Tmp6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*eopt=_Tmp6;
# 146
void*t=Cyc_Tcenv_return_typ(te);
if(eopt==0){
if(!Cyc_Tcutil_is_void_type(t)){
if(Cyc_Tcutil_is_any_float_type(t)|| Cyc_Tcutil_is_any_int_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("should return a value of type ",sizeof(char),31U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_warn2(s0->loc,_tag_fat(_Tmp9,sizeof(void*),2));});else{
# 152
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("must return a value of type ",sizeof(char),29U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});void*_Tmp9[2];_Tmp9[0]=& _Tmp7,_Tmp9[1]=& _Tmp8;Cyc_Warn_err2(s0->loc,_tag_fat(_Tmp9,sizeof(void*),2));});}}
# 154
return;}
# 157
Cyc_Tcexp_tcExp(te,& t,eopt);
if(!({struct Cyc_List_List*_Tmp7=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp8=eopt;Cyc_Tcutil_coerce_arg(_Tmp7,_Tmp8,t,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("returns value of type ",sizeof(char),23U);_Tmp8;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp9;_Tmp9.tag=3,_Tmp9.f1=(void*)eopt->topt;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat(" but requires ",sizeof(char),15U);_TmpA;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpB;_TmpB.tag=2,_TmpB.f1=(void*)t;_TmpB;});void*_TmpB[4];_TmpB[0]=& _Tmp7,_TmpB[1]=& _Tmp8,_TmpB[2]=& _Tmp9,_TmpB[3]=& _TmpA;Cyc_Warn_err2(s0->loc,_tag_fat(_TmpB,sizeof(void*),4));});
Cyc_Unify_explain_failure();}
# 162
({unsigned _Tmp7=eopt->loc;struct Cyc_List_List*_Tmp8=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp9=t;Cyc_Tcexp_check_consume(_Tmp7,_Tmp8,_Tmp9,eopt);});
return;}case 4: _Tmp6=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*s1=_Tmp5;struct Cyc_Absyn_Stmt*s2=_Tmp4;
# 166
Cyc_Tcexp_tcTest(te,e,_tag_fat("if statement",sizeof(char),13U));
Cyc_Tcstmt_tcStmt(te,s1,1);
Cyc_Tcstmt_tcStmt(te,s2,1);
return;}case 5: _Tmp6=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f0;_Tmp5=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1.f1;_Tmp4=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e=_Tmp6;struct Cyc_Absyn_Stmt*cont_s=_Tmp5;struct Cyc_Absyn_Stmt*s=_Tmp4;
# 172
Cyc_Tcexp_tcTest(te,e,_tag_fat("while loop",sizeof(char),11U));
Cyc_Tcstmt_tcStmt(te,s,1);
return;}case 9: _Tmp6=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;_Tmp3=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f0;_Tmp2=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3.f1;_Tmp1=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*e1=_Tmp6;struct Cyc_Absyn_Exp*e2=_Tmp5;struct Cyc_Absyn_Stmt*guard_s=_Tmp4;struct Cyc_Absyn_Exp*e3=_Tmp3;struct Cyc_Absyn_Stmt*cont_s=_Tmp2;struct Cyc_Absyn_Stmt*s=_Tmp1;
# 177
Cyc_Tcexp_tcExp(te,0,e1);
Cyc_Tcexp_tcTest(te,e2,_tag_fat("for loop",sizeof(char),9U));
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcExp(te,0,e3);
Cyc_Tcstmt_simplify_unused_result_exp(e3);
return;}case 14: _Tmp6=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f0;_Tmp4=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2.f1;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_Absyn_Exp*e=_Tmp5;struct Cyc_Absyn_Stmt*cont_s=_Tmp4;
# 185
Cyc_Tcstmt_tcStmt(te,s,1);
Cyc_Tcexp_tcTest(te,e,_tag_fat("do loop",sizeof(char),8U));
return;}case 6:
# 189
 goto _LL14;case 7: _LL14:
 goto _LL16;case 8: _LL16:
 return;case 13: _Tmp6=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct _fat_ptr*l=_Tmp6;struct Cyc_Absyn_Stmt*s=_Tmp5;
# 196
struct Cyc_Absyn_Tvar*tv;tv=_cycalloc(sizeof(struct Cyc_Absyn_Tvar)),({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));({struct _fat_ptr _Tmp9=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=*l;_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_aprintf(_tag_fat("`%s",sizeof(char),4U),_tag_fat(_TmpB,sizeof(void*),1));});*_Tmp8=_Tmp9;});_Tmp8;});tv->name=_Tmp7;}),({
int _Tmp7=Cyc_Tcutil_new_tvar_id();tv->identity=_Tmp7;}),({void*_Tmp7=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ek);tv->kind=_Tmp7;}),tv->aquals_bound=0;
({struct Cyc_Tcenv_Tenv*_Tmp7=Cyc_Tcenv_new_named_block(s0->loc,te,tv);Cyc_Tcstmt_tcStmt(_Tmp7,s,0);});
return;}case 11: _Tmp6=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=(struct Cyc_Absyn_Switch_clause***)&((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp6;struct Cyc_Absyn_Switch_clause***clauseopt=_Tmp5;
# 202
const struct _tuple14*trip_opt=Cyc_Tcenv_process_fallthru(te,s0,clauseopt);
if(trip_opt==(const struct _tuple14*)0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("fallthru in a last case",sizeof(char),24U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(s0->loc,_tag_fat(_Tmp8,sizeof(void*),1));});
return;}{
# 207
struct _tuple14 _Tmp7=*trip_opt;void*_Tmp8;void*_Tmp9;_Tmp9=_Tmp7.f1;_Tmp8=_Tmp7.f2;{struct Cyc_List_List*tvs=_Tmp9;struct Cyc_List_List*ts_orig=_Tmp8;
struct Cyc_List_List*instantiation=({struct Cyc_List_List*(*_TmpA)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*)=({struct Cyc_List_List*(*_TmpB)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple13*(*)(struct Cyc_List_List*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c;_TmpB;});struct Cyc_List_List*_TmpB=Cyc_Tcenv_lookup_type_vars(te);_TmpA(Cyc_Tcutil_make_inst_var,_TmpB,tvs);});
struct Cyc_List_List*ts=({struct Cyc_List_List*(*_TmpA)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(struct Cyc_List_List*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_map_c;_TmpA;})(Cyc_Tcutil_substitute,instantiation,ts_orig);
for(1;ts!=0 && es!=0;(ts=ts->tl,es=es->tl)){
# 212
Cyc_Tcexp_tcExp(te,0,(struct Cyc_Absyn_Exp*)es->hd);
if(!({struct Cyc_List_List*_TmpA=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_TmpB=(struct Cyc_Absyn_Exp*)es->hd;Cyc_Tcutil_coerce_arg(_TmpA,_TmpB,(void*)ts->hd,0);})){
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("fallthru argument has type ",sizeof(char),28U);_TmpB;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpB=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpC;_TmpC.tag=3,_TmpC.f1=(void*)((struct Cyc_Absyn_Exp*)es->hd)->topt;_TmpC;});struct Cyc_Warn_String_Warn_Warg_struct _TmpC=({struct Cyc_Warn_String_Warn_Warg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=
_tag_fat("but pattern variable has type ",sizeof(char),31U);_TmpD;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpD=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpE;_TmpE.tag=2,_TmpE.f1=(void*)ts->hd;_TmpE;});void*_TmpE[4];_TmpE[0]=& _TmpA,_TmpE[1]=& _TmpB,_TmpE[2]=& _TmpC,_TmpE[3]=& _TmpD;Cyc_Warn_err2(s0->loc,_tag_fat(_TmpE,sizeof(void*),4));});
Cyc_Unify_explain_failure();}
# 218
({unsigned _TmpA=((struct Cyc_Absyn_Exp*)es->hd)->loc;struct Cyc_List_List*_TmpB=Cyc_Tcenv_curr_aquals_bounds(te);void*_TmpC=(void*)ts->hd;Cyc_Tcexp_check_consume(_TmpA,_TmpB,_TmpC,(struct Cyc_Absyn_Exp*)es->hd);});}
# 220
if(es!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("too many arguments in explicit fallthru",sizeof(char),40U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(s0->loc,_tag_fat(_TmpB,sizeof(void*),1));});
if(ts!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("too few arguments in explicit fallthru",sizeof(char),39U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(s0->loc,_tag_fat(_TmpB,sizeof(void*),1));});
return;}}}case 10: _Tmp6=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=(void**)&((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Exp**exp=_Tmp6;struct Cyc_List_List*scs0=_Tmp5;void**dtp=_Tmp4;
# 228
if(Cyc_Tcstmt_make_var_exp(exp,s0)){
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}{
# 233
struct Cyc_Absyn_Exp*e=*exp;
void*t=Cyc_Tcexp_tcExp(te,0,e);
# 236
({unsigned _Tmp7=e->loc;struct Cyc_List_List*_Tmp8=Cyc_Tcenv_curr_aquals_bounds(te);void*_Tmp9=t;Cyc_Tcexp_check_consume(_Tmp7,_Tmp8,_Tmp9,e);});{
# 240
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*scs=scs0;for(0;scs!=0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& t,e);}else{
# 248
this_pattern=*_check_null(next_pattern);}
# 250
if(scs->tl!=0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_Tmp7=_cycalloc(sizeof(struct Cyc_Tcpat_TcPatResult));({struct Cyc_Tcpat_TcPatResult _Tmp8=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->tl->hd)->pattern,& t,e);*_Tmp7=_Tmp8;});_Tmp7;});
if(next_pattern->tvars_and_effconstr_opt!=0 &&(*next_pattern->tvars_and_effconstr_opt).f1!=0)
# 254
te=Cyc_Tcenv_clear_fallthru(te);else{
# 256
struct Cyc_List_List*vs=Cyc_List_imp_rev(Cyc_Tcutil_filter_nulls(Cyc_List_split(next_pattern->patvars).f0));
struct Cyc_List_List*new_tvs=next_pattern->tvars_and_effconstr_opt==0?0:
({struct Cyc_List_List*(*_Tmp7)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;_Tmp7;})(({struct Cyc_Absyn_Tvar*(*_Tmp7)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst;_Tmp7;}),(*next_pattern->tvars_and_effconstr_opt).f0);
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)_check_null(scs->tl)->hd);}}else{
# 262
te=Cyc_Tcenv_clear_fallthru(te);}{
# 264
struct Cyc_Absyn_Pat*p=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
enum Cyc_Absyn_Coercion _c=Cyc_Subtype_coercible_exact(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,t,_check_null(p->topt));
if((int)_c==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat("switch on type ",sizeof(char),16U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)t;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat(" but case expects type ",sizeof(char),24U);_TmpA;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpA=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpB;_TmpB.tag=3,_TmpB.f1=(void*)p->topt;_TmpB;});void*_TmpB[4];_TmpB[0]=& _Tmp7,_TmpB[1]=& _Tmp8,_TmpB[2]=& _Tmp9,_TmpB[3]=& _TmpA;Cyc_Warn_err2(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tag_fat(_TmpB,sizeof(void*),4));});
Cyc_Unify_explain_failure();}else{
# 272
Cyc_Tcpat_check_pat_regions(te,p,& this_pattern);}
# 274
({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp8->v=this_pattern.patvars;_Tmp8;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_Tmp7;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 278
Cyc_Tcpat_check_switch_exhaustive(s0->loc,te,scs0,dtp);
return;}}}case 15: _Tmp6=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=(struct Cyc_List_List**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;_Tmp4=(void**)&((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f3;{struct Cyc_Absyn_Stmt*s=_Tmp6;struct Cyc_List_List**scs0=_Tmp5;void**dtp=(void**)_Tmp4;
# 286
({void*_Tmp7=({void*_Tmp8=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_Tmp9->tag=2,({struct Cyc_Absyn_Stmt*_TmpA=Cyc_Absyn_new_stmt(s->r,s->loc);_Tmp9->f1=_TmpA;}),({struct Cyc_Absyn_Stmt*_TmpA=Cyc_Absyn_skip_stmt(s->loc);_Tmp9->f2=_TmpA;});_Tmp9;});Cyc_Absyn_new_stmt(_Tmp8,s->loc);})->r;s->r=_Tmp7;});
# 288
Cyc_Tcstmt_tcStmt(te,s,1);
# 290
if(!({int(*_Tmp7)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Switch_clause*),struct Cyc_List_List*))Cyc_List_exists;_Tmp7;})(Cyc_Tcstmt_is_case_default,*scs0)){
# 292
struct _tuple2*def_v;def_v=_cycalloc(sizeof(struct _tuple2)),def_v->f0=Cyc_Absyn_Loc_n,({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));*_Tmp8=_tag_fat("exn",sizeof(char),4U);_Tmp8;});def_v->f1=_Tmp7;});{
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,def_v,Cyc_Absyn_void_type,0,0);
struct Cyc_Absyn_Pat*def_pat=Cyc_Absyn_new_pat((void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct));_Tmp7->tag=1,_Tmp7->f1=vd,({struct Cyc_Absyn_Pat*_Tmp8=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_Tmp7->f2=_Tmp8;});_Tmp7;}),0U);
struct Cyc_Absyn_Stmt*def_stmt=
Cyc_Absyn_exp_stmt(Cyc_Absyn_new_exp((void*)({struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct));_Tmp7->tag=11,({struct Cyc_Absyn_Exp*_Tmp8=Cyc_Absyn_varb_exp((void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_Tmp9=_cycalloc(sizeof(struct Cyc_Absyn_Local_b_Absyn_Binding_struct));_Tmp9->tag=4,_Tmp9->f1=vd;_Tmp9;}),0U);_Tmp7->f1=_Tmp8;}),_Tmp7->f2=1;_Tmp7;}),0U),0U);
struct Cyc_Absyn_Switch_clause*def_clause;def_clause=_cycalloc(sizeof(struct Cyc_Absyn_Switch_clause)),def_clause->pattern=def_pat,def_clause->pat_vars=0,def_clause->where_clause=0,def_clause->body=def_stmt,def_clause->loc=0U;
({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*_Tmp8=*scs0;Cyc_List_append(_Tmp8,({struct Cyc_Absyn_Switch_clause*_Tmp9[1];_Tmp9[0]=def_clause;Cyc_List_list(_tag_fat(_Tmp9,sizeof(struct Cyc_Absyn_Switch_clause*),1));}));});*scs0=_Tmp7;});}}{
# 304
void*exception_type=Cyc_Absyn_exn_type();
struct Cyc_Tcpat_TcPatResult this_pattern;
struct Cyc_Tcpat_TcPatResult*next_pattern=0;
int first_case=1;
{struct Cyc_List_List*scs=*scs0;for(0;scs!=0;scs=scs->tl){
if(first_case){
first_case=0;
this_pattern=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern,& exception_type,0);}else{
# 313
this_pattern=*_check_null(next_pattern);}
# 315
if(scs->tl!=0){
next_pattern=({struct Cyc_Tcpat_TcPatResult*_Tmp7=_cycalloc(sizeof(struct Cyc_Tcpat_TcPatResult));({struct Cyc_Tcpat_TcPatResult _Tmp8=Cyc_Tcpat_tcPat(te,((struct Cyc_Absyn_Switch_clause*)scs->tl->hd)->pattern,& exception_type,0);*_Tmp7=_Tmp8;});_Tmp7;});
if(next_pattern->tvars_and_effconstr_opt!=0 &&(*next_pattern->tvars_and_effconstr_opt).f1!=0)
# 319
te=Cyc_Tcenv_clear_fallthru(te);else{
# 321
struct Cyc_List_List*vs=Cyc_List_imp_rev(Cyc_Tcutil_filter_nulls(Cyc_List_split(next_pattern->patvars).f0));
struct Cyc_List_List*new_tvs=next_pattern->tvars_and_effconstr_opt==0?0:
({struct Cyc_List_List*(*_Tmp7)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*),struct Cyc_List_List*))Cyc_List_map;_Tmp7;})(({struct Cyc_Absyn_Tvar*(*_Tmp7)(struct _tuple15*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple15*))Cyc_Core_fst;_Tmp7;}),(*next_pattern->tvars_and_effconstr_opt).f0);
te=Cyc_Tcenv_set_fallthru(te,new_tvs,vs,(struct Cyc_Absyn_Switch_clause*)_check_null(scs->tl)->hd);}}else{
# 327
te=Cyc_Tcenv_clear_fallthru(te);}{
# 329
struct Cyc_Absyn_Pat*p=((struct Cyc_Absyn_Switch_clause*)scs->hd)->pattern;
enum Cyc_Absyn_Coercion _c=Cyc_Subtype_coercible_exact(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,exception_type,_check_null(p->topt));
if((int)_c==0){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("switch on type ",sizeof(char),16U);_Tmp8;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9;_Tmp9.tag=2,_Tmp9.f1=(void*)exception_type;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=
_tag_fat(" but case expects type ",sizeof(char),24U);_TmpA;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpA=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _TmpB;_TmpB.tag=3,_TmpB.f1=(void*)p->topt;_TmpB;});void*_TmpB[4];_TmpB[0]=& _Tmp7,_TmpB[1]=& _Tmp8,_TmpB[2]=& _Tmp9,_TmpB[3]=& _TmpA;Cyc_Warn_err2(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,_tag_fat(_TmpB,sizeof(void*),4));});
Cyc_Unify_explain_failure();}else{
# 337
Cyc_Tcpat_check_pat_regions(te,p,& this_pattern);}
# 339
({struct Cyc_Core_Opt*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp8->v=this_pattern.patvars;_Tmp8;});((struct Cyc_Absyn_Switch_clause*)scs->hd)->pat_vars=_Tmp7;});
Cyc_Tcstmt_pattern_synth(((struct Cyc_Absyn_Switch_clause*)scs->hd)->loc,te,this_pattern,((struct Cyc_Absyn_Switch_clause*)scs->hd)->body,((struct Cyc_Absyn_Switch_clause*)scs->hd)->where_clause,1);}}}
# 343
Cyc_Tcpat_check_catch_overlap(s0->loc,te,*scs0,dtp);
return;}}default: _Tmp6=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f1;_Tmp5=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Decl*d=_Tmp6;struct Cyc_Absyn_Stmt*s=_Tmp5;
# 347
struct _fat_ptr unimp_msg_part;
struct Cyc_Tcenv_Tenv*_Tmp7=new_block?Cyc_Tcenv_new_block(s0->loc,te): te;struct Cyc_Tcenv_Tenv*te=_Tmp7;
{void*_Tmp8=d->r;void*_Tmp9;void*_TmpA;void*_TmpB;void*_TmpC;switch(*((int*)_Tmp8)){case 0: _TmpC=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;{struct Cyc_Absyn_Vardecl*vd=_TmpC;
# 351
void*_TmpD;void*_TmpE;void*_TmpF;struct Cyc_Absyn_Tqual _Tmp10;enum Cyc_Absyn_Scope _Tmp11;_Tmp11=vd->sc;_Tmp10=vd->tq;_TmpF=vd->type;_TmpE=vd->initializer;_TmpD=(void**)& vd->rgn;{enum Cyc_Absyn_Scope sc=_Tmp11;struct Cyc_Absyn_Tqual tq=_Tmp10;void*t=_TmpF;struct Cyc_Absyn_Exp*initializer=_TmpE;void**rgn_ptr=(void**)_TmpD;
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
int is_local;
switch((int)sc){case Cyc_Absyn_Static:
 goto _LL4E;case Cyc_Absyn_Extern: _LL4E:
 goto _LL50;case Cyc_Absyn_ExternC: _LL50:
 vd->escapes=1;is_local=0;goto _LL4A;case Cyc_Absyn_Abstract:
# 359
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("bad abstract scope for local variable",sizeof(char),38U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp13,sizeof(void*),1));});
goto _LL54;case Cyc_Absyn_Register: _LL54:
 goto _LL56;case Cyc_Absyn_Public: _LL56:
 goto _LL58;default: _LL58:
 is_local=1;goto _LL4A;}_LL4A:;
# 366
*rgn_ptr=is_local?curr_bl: Cyc_Absyn_heap_rgn_type;
{void*_Tmp12=Cyc_Absyn_compress(t);struct Cyc_Absyn_ArrayInfo _Tmp13;switch(*((int*)_Tmp12)){case 6: if(is_local){
# 370
vd->escapes=1;
sc=3U;
is_local=0;
goto _LL59;}else{goto _LL5E;}case 5: _Tmp13=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp12)->f1;{struct Cyc_Absyn_ArrayInfo ai=_Tmp13;
# 376
if(vd->initializer==0 || ai.num_elts!=0)
goto _LL59;{
struct Cyc_Absyn_Exp*len;
{void*_Tmp14=vd->initializer->r;void*_Tmp15;struct _fat_ptr _Tmp16;switch(*((int*)_Tmp14)){case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp14)->f1.Wstring_c.tag){case 8: _Tmp16=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp14)->f1.String_c.val;{struct _fat_ptr s=_Tmp16;
len=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);goto _LL60;}case 9: _Tmp16=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp14)->f1.Wstring_c.val;{struct _fat_ptr s=_Tmp16;
len=Cyc_Absyn_uint_exp(1U,0U);goto _LL60;}default: goto _LL6D;}case 26: _Tmp15=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_Tmp14)->f2;{struct Cyc_Absyn_Exp*e=_Tmp15;
_Tmp15=e;goto _LL68;}case 27: _Tmp15=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_Tmp14)->f1;_LL68: {struct Cyc_Absyn_Exp*e=_Tmp15;
len=e;goto _LL60;}case 35: _Tmp15=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_Tmp14)->f2;{struct Cyc_List_List*es=_Tmp15;
_Tmp15=es;goto _LL6C;}case 25: _Tmp15=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_Tmp14)->f1;_LL6C: {struct Cyc_List_List*es=_Tmp15;
len=Cyc_Absyn_uint_exp((unsigned)Cyc_List_length(es),0U);goto _LL60;}default: _LL6D:
 len=0;goto _LL60;}_LL60:;}
# 388
ai.num_elts=len;
t=({void*_Tmp14=(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_Tmp15=_cycalloc(sizeof(struct Cyc_Absyn_ArrayType_Absyn_Type_struct));_Tmp15->tag=5,_Tmp15->f1=ai;_Tmp15;});vd->type=_Tmp14;});
goto _LL59;}}default: _LL5E:
 goto _LL59;}_LL59:;}{
# 396
struct Cyc_List_List*bound_vars=!is_local?0: Cyc_Tcenv_lookup_type_vars(te);
int allow_evars=is_local;
Cyc_Tctyp_check_type(s0->loc,te,bound_vars,& Cyc_Kinds_mk,allow_evars,1,t);
({int _Tmp12=Cyc_Tcutil_extract_const_from_typedef(s0->loc,vd->tq.print_const,t);vd->tq.real_const=_Tmp12;});
# 401
if((int)sc==3 ||(int)sc==4)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("extern declarations are not yet supported within functions",sizeof(char),59U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp13,sizeof(void*),1));});
if(initializer!=0){
Cyc_Tcexp_tcExpInitializer(te,& t,initializer);
# 406
if(!is_local && !Cyc_Tcutil_is_const_exp(initializer))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp13;_Tmp13.tag=0,_Tmp13.f1=_tag_fat("initializer for static variable needs to be a constant expression",sizeof(char),66U);_Tmp13;});void*_Tmp13[1];_Tmp13[0]=& _Tmp12;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp13,sizeof(void*),1));});
if(!({struct Cyc_List_List*_Tmp12=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp13=initializer;Cyc_Tcutil_coerce_assign(_Tmp12,_Tmp13,t);})){
({struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp13;_Tmp13.tag=11,_Tmp13.f1=vd;_Tmp13;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp14;_Tmp14.tag=0,_Tmp14.f1=_tag_fat(" was declared with type ",sizeof(char),25U);_Tmp14;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp14=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp15;_Tmp15.tag=2,_Tmp15.f1=(void*)t;_Tmp15;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp15=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp16;_Tmp16.tag=0,_Tmp16.f1=
_tag_fat(" but initialized with type ",sizeof(char),28U);_Tmp16;});struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp16=({struct Cyc_Warn_TypOpt_Warn_Warg_struct _Tmp17;_Tmp17.tag=3,_Tmp17.f1=(void*)initializer->topt;_Tmp17;});void*_Tmp17[5];_Tmp17[0]=& _Tmp12,_Tmp17[1]=& _Tmp13,_Tmp17[2]=& _Tmp14,_Tmp17[3]=& _Tmp15,_Tmp17[4]=& _Tmp16;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp17,sizeof(void*),5));});
Cyc_Unify_unify(t,_check_null(initializer->topt));
Cyc_Unify_explain_failure();}}
# 415
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}case 2: _TmpC=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;_TmpB=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp8)->f2;_TmpA=(struct Cyc_Absyn_Exp**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp8)->f3;_Tmp9=(void**)&((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_Tmp8)->f4;{struct Cyc_Absyn_Pat*p=_TmpC;struct Cyc_Core_Opt**vds=_TmpB;struct Cyc_Absyn_Exp**exp=_TmpA;void**dtp=(void**)_Tmp9;
# 420
{void*_TmpD=p->r;switch(*((int*)_TmpD)){case 1:
 goto _LL73;case 2: _LL73:
 goto _LL75;case 14: _LL75:
 goto _LL6F;default:
# 425
 if(Cyc_Tcstmt_make_var_exp(exp,s0)){
Cyc_Tcstmt_tcStmt(te,s0,new_block);
return;}}_LL6F:;}{
# 431
struct Cyc_Absyn_Exp*e=*exp;
Cyc_Tcexp_tcExpInitializer(te,0,e);{
# 434
void*pat_type=_check_null(e->topt);
# 436
struct Cyc_Tcpat_TcPatResult pat_res=Cyc_Tcpat_tcPat(te,p,& pat_type,e);
({struct Cyc_Core_Opt*_TmpD=({struct Cyc_Core_Opt*_TmpE=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpE->v=pat_res.patvars;_TmpE;});*vds=_TmpD;});{
void*pt=_check_null(p->topt);
void*et=_check_null(e->topt);
if(!Cyc_Unify_unify(pt,et)&& !({struct Cyc_List_List*_TmpD=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_TmpE=e;Cyc_Tcutil_coerce_assign(_TmpD,_TmpE,pt);})){
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=
_tag_fat("pattern type ",sizeof(char),14U);_TmpE;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpE=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpF;_TmpF.tag=2,_TmpF.f1=(void*)pt;_TmpF;});struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat(" does not match definition type ",sizeof(char),33U);_Tmp10;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp11;_Tmp11.tag=2,_Tmp11.f1=(void*)et;_Tmp11;});void*_Tmp11[4];_Tmp11[0]=& _TmpD,_Tmp11[1]=& _TmpE,_Tmp11[2]=& _TmpF,_Tmp11[3]=& _Tmp10;Cyc_Warn_err2(d->loc,_tag_fat(_Tmp11,sizeof(void*),4));});
Cyc_Unify_unify(pt,et);
Cyc_Unify_explain_failure();}else{
# 447
Cyc_Tcpat_check_pat_regions(te,p,& pat_res);}
Cyc_Tcpat_check_let_pat_exhaustive(p->loc,te,p,dtp);
Cyc_Tcstmt_pattern_synth(s0->loc,te,pat_res,s,0,0);
return;}}}}case 3: _TmpC=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;{struct Cyc_List_List*vds=_TmpC;
# 453
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
for(1;vds!=0;vds=vds->tl){
((struct Cyc_Absyn_Vardecl*)vds->hd)->rgn=curr_bl;
({unsigned _TmpD=s0->loc;struct Cyc_Tcenv_Tenv*_TmpE=te;struct Cyc_List_List*_TmpF=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_TmpD,_TmpE,_TmpF,& Cyc_Kinds_mk,1,1,((struct Cyc_Absyn_Vardecl*)vds->hd)->type);});}
# 459
Cyc_Tcstmt_tcStmt(te,s,0);
return;}case 4: _TmpC=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;_TmpB=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp8)->f2;_TmpA=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_Tmp8)->f3;{struct Cyc_Absyn_Tvar*tv=_TmpC;struct Cyc_Absyn_Vardecl*vd=_TmpB;struct Cyc_Absyn_Exp*open_exp_opt=_TmpA;
# 465
({void*_TmpD=({void*_TmpE=(void*)({struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct));_TmpF->tag=2,({struct Cyc_Absyn_Stmt*_Tmp10=Cyc_Absyn_new_stmt(s->r,s->loc);_TmpF->f1=_Tmp10;}),({struct Cyc_Absyn_Stmt*_Tmp10=Cyc_Absyn_skip_stmt(s->loc);_TmpF->f2=_Tmp10;});_TmpF;});Cyc_Absyn_new_stmt(_TmpE,s->loc);})->r;s->r=_TmpD;});{
# 467
struct Cyc_Tcenv_Tenv*te2=te;
void*_TmpD;void*_TmpE;_TmpE=(void**)& vd->type;_TmpD=(void**)& vd->rgn;{void**t=(void**)_TmpE;void**rgn_ptr=(void**)_TmpD;
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
*rgn_ptr=curr_bl;{
void*rgn_typ;
if((unsigned)open_exp_opt){
# 475
struct _tuple2*drname;drname=_cycalloc(sizeof(struct _tuple2)),({union Cyc_Absyn_Nmspace _TmpF=Cyc_Absyn_Abs_n(({struct _fat_ptr*_Tmp10[1];({struct _fat_ptr*_Tmp11=({struct _fat_ptr*_Tmp12=_cycalloc(sizeof(struct _fat_ptr));*_Tmp12=_tag_fat("Core",sizeof(char),5U);_Tmp12;});_Tmp10[0]=_Tmp11;});Cyc_List_list(_tag_fat(_Tmp10,sizeof(struct _fat_ptr*),1));}),0);drname->f0=_TmpF;}),({struct _fat_ptr*_TmpF=({struct _fat_ptr*_Tmp10=_cycalloc(sizeof(struct _fat_ptr));*_Tmp10=_tag_fat("DynamicRegion",sizeof(char),14U);_Tmp10;});drname->f1=_TmpF;});{
void*outer_rgn=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_TmpF=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpF->v=& Cyc_Kinds_ek;_TmpF;}),0);
rgn_typ=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_TmpF=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpF->v=& Cyc_Kinds_ek;_TmpF;}),0);{
void*aq=Cyc_Absyn_new_evar(({struct Cyc_Core_Opt*_TmpF=_cycalloc(sizeof(struct Cyc_Core_Opt));_TmpF->v=& Cyc_Kinds_aqk;_TmpF;}),0);
void*dr_type=({union Cyc_Absyn_AggrInfo _TmpF=Cyc_Absyn_UnknownAggr(0U,drname,0);Cyc_Absyn_aggr_type(_TmpF,({void*_Tmp10[1];_Tmp10[0]=rgn_typ;Cyc_List_list(_tag_fat(_Tmp10,sizeof(void*),1));}));});
void*exp_type=({void*_TmpF=dr_type;void*_Tmp10=outer_rgn;void*_Tmp11=aq;struct Cyc_Absyn_Tqual _Tmp12=Cyc_Absyn_empty_tqual(0U);void*_Tmp13=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_TmpF,_Tmp10,_Tmp11,_Tmp12,_Tmp13,Cyc_Absyn_false_type);});
({unsigned _TmpF=s0->loc;struct Cyc_Tcenv_Tenv*_Tmp10=te;struct Cyc_List_List*_Tmp11=Cyc_Tcenv_lookup_type_vars(te);Cyc_Tctyp_check_type(_TmpF,_Tmp10,_Tmp11,& Cyc_Kinds_mk,1,0,exp_type);});{
# 483
void*key_typ=Cyc_Tcexp_tcExp(te,& exp_type,open_exp_opt);
if(!Cyc_Unify_unify(exp_type,key_typ)&& !({
struct Cyc_List_List*_TmpF=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp10=open_exp_opt;Cyc_Tcutil_coerce_assign(_TmpF,_Tmp10,exp_type);}))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat("expected ",sizeof(char),10U);_Tmp10;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp10=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp11;_Tmp11.tag=2,_Tmp11.f1=(void*)exp_type;_Tmp11;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp12;_Tmp12.tag=0,_Tmp12.f1=_tag_fat(" but found ",sizeof(char),12U);_Tmp12;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp12=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp13;_Tmp13.tag=2,_Tmp13.f1=(void*)key_typ;_Tmp13;});void*_Tmp13[4];_Tmp13[0]=& _TmpF,_Tmp13[1]=& _Tmp10,_Tmp13[2]=& _Tmp11,_Tmp13[3]=& _Tmp12;Cyc_Warn_err2(s0->loc,_tag_fat(_Tmp13,sizeof(void*),4));});
# 489
if(Cyc_Tcutil_is_aliasable_qual(aq))
({struct Cyc_Warn_String_Warn_Warg_struct _TmpF=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp10;_Tmp10.tag=0,_Tmp10.f1=_tag_fat("open is allowed only on unique or reference-counted keys",sizeof(char),57U);_Tmp10;});void*_Tmp10[1];_Tmp10[0]=& _TmpF;Cyc_Warn_err2(s0->loc,_tag_fat(_Tmp10,sizeof(void*),1));});
({void*_TmpF=Cyc_Absyn_rgn_handle_type(rgn_typ);*t=_TmpF;});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,1,0);}}}}else{
# 494
rgn_typ=(void*)({struct Cyc_Absyn_VarType_Absyn_Type_struct*_TmpF=_cycalloc(sizeof(struct Cyc_Absyn_VarType_Absyn_Type_struct));_TmpF->tag=2,_TmpF->f1=tv;_TmpF;});
te2=({unsigned _TmpF=s0->loc;struct Cyc_Tcenv_Tenv*_Tmp10=te2;Cyc_Tcenv_add_type_vars(_TmpF,_Tmp10,({struct Cyc_List_List*_Tmp11=_cycalloc(sizeof(struct Cyc_List_List));_Tmp11->hd=tv,_Tmp11->tl=0;_Tmp11;}));});
te2=Cyc_Tcenv_add_region(te2,rgn_typ,0,1);}
# 498
({unsigned _TmpF=s0->loc;struct Cyc_Tcenv_Tenv*_Tmp10=te;struct Cyc_List_List*_Tmp11=Cyc_Tcenv_lookup_type_vars(te2);Cyc_Tctyp_check_type(_TmpF,_Tmp10,_Tmp11,& Cyc_Kinds_bk,1,0,*t);});
if(!({void*_TmpF=*t;Cyc_Unify_unify(_TmpF,Cyc_Absyn_rgn_handle_type(rgn_typ));}))
({int(*_TmpF)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpF;})(_tag_fat("region stmt: type of region handle is wrong!",sizeof(char),45U),_tag_fat(0U,sizeof(void*),0));
Cyc_Tcstmt_tcStmt(te2,s,0);
return;}}}}case 1: _TmpC=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;{struct Cyc_Absyn_Fndecl*fd=_TmpC;
# 505
if((int)fd->sc!=2)
({struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=_tag_fat("bad storage class for inner function",sizeof(char),37U);_TmpE;});void*_TmpE[1];_TmpE[0]=& _TmpD;Cyc_Warn_err2(d->loc,_tag_fat(_TmpE,sizeof(void*),1));});
Cyc_Atts_check_fndecl_atts(d->loc,fd->i.attributes,1);
Cyc_Tctyp_check_fndecl_valid_type(d->loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
({struct Cyc_List_List*_TmpD=Cyc_Atts_transfer_fn_type_atts(t,fd->i.attributes);fd->i.attributes=_TmpD;});{
void*curr_bl=Cyc_Tcenv_curr_rgn(te);
struct Cyc_Absyn_Vardecl*vd;vd=_cycalloc(sizeof(struct Cyc_Absyn_Vardecl)),vd->sc=fd->sc,vd->name=fd->name,vd->varloc=0U,({struct Cyc_Absyn_Tqual _TmpD=Cyc_Absyn_const_tqual(0U);vd->tq=_TmpD;}),({
void*_TmpD=({void*_TmpE=t;void*_TmpF=curr_bl;void*_Tmp10=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp11=Cyc_Absyn_empty_tqual(0U);void*_Tmp12=Cyc_Absyn_false_type;Cyc_Absyn_at_type(_TmpE,_TmpF,_Tmp10,_Tmp11,_Tmp12,Cyc_Absyn_false_type);});vd->type=_TmpD;}),vd->initializer=0,vd->rgn=curr_bl,vd->attributes=0,vd->escapes=0,vd->is_proto=fd->body!=0,vd->rename=0;
# 518
fd->fn_vardecl=vd;{
struct Cyc_Tcenv_Fenv*old_fenv=_check_null(te->le);
({struct Cyc_Tcenv_Fenv*_TmpD=Cyc_Tcenv_nested_fenv(d->loc,old_fenv,fd);te->le=_TmpD;});
Cyc_Tcstmt_tcStmt(te,fd->body,0);
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
te->le=old_fenv;
Cyc_Tcstmt_tcStmt(te,s,0);
return;}}}}case 9: _TmpC=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;_TmpB=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp8)->f2;{struct _fat_ptr*n=_TmpC;struct Cyc_List_List*tds=_TmpB;
unimp_msg_part=_tag_fat("namespace",sizeof(char),10U);goto _LL24;}case 10: _TmpC=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp8)->f1;_TmpB=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp8)->f2;{struct _tuple2*q=_TmpC;struct Cyc_List_List*tds=_TmpB;
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
 unimp_msg_part=_tag_fat("__tempest_off__",sizeof(char),16U);goto _LL24;}_LL24:;}
# 540
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("tcStmt: nested ",sizeof(char),16U);_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=unimp_msg_part;_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat(" declarations not allowed",sizeof(char),26U);_TmpB;});void*_TmpB[3];_TmpB[0]=& _Tmp8,_TmpB[1]=& _Tmp9,_TmpB[2]=& _TmpA;({int(*_TmpC)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpC;})(_tag_fat(_TmpB,sizeof(void*),3));});}};}
# 551 "tcstmt.cyc"
static int Cyc_Tcstmt_check_catchall_handler(struct Cyc_Absyn_Stmt*b){
void*_Tmp0=b->r;void*_Tmp1;if(*((int*)_Tmp0)==15){_Tmp1=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_Tmp0)->f2;{struct Cyc_List_List*case_list=_Tmp1;
# 554
int found_wild=0;
for(1;(unsigned)case_list;case_list=case_list->tl){
struct Cyc_Absyn_Switch_clause*clause=(struct Cyc_Absyn_Switch_clause*)case_list->hd;
{void*_Tmp2=clause->pattern->r;if(*((int*)_Tmp2)==0){
found_wild=1;goto _LL5;}else{
goto _LL5;}_LL5:;}
# 561
if(!Cyc_Tcstmt_ensure_no_throw_stmt(clause->body))
return 0;}
# 564
return found_wild;}}else{
return 0;};}struct _tuple17{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 568
static int Cyc_Tcstmt_ensure_no_throw_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;enum Cyc_Absyn_Coercion _Tmp1;int _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)_Tmp0)){case 0:
 return 1;case 3: _Tmp5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_List_List*es=_Tmp5;
# 572
return({int(*_Tmp6)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;_Tmp6;})(Cyc_Tcstmt_ensure_no_throw_exp,es);}case 7: _Tmp5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LL8;}case 8: _Tmp5=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LLA;}case 34: _Tmp5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
_Tmp5=e1;_Tmp4=e2;goto _LLC;}case 9: _Tmp5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_Tmp5;struct Cyc_Absyn_Exp*e2=_Tmp4;
return Cyc_Tcstmt_ensure_no_throw_exp(e1)&& Cyc_Tcstmt_ensure_no_throw_exp(e2);}case 10: _Tmp5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{struct Cyc_Absyn_Exp*fname=_Tmp5;struct Cyc_List_List*args=_Tmp4;struct Cyc_Absyn_VarargCallInfo*vargs=_Tmp3;int res=_Tmp2;
# 578
if(!res)
return 0;{
int fname_ok=0;
{void*_Tmp6=fname->r;void*_Tmp7;void*_Tmp8;switch(*((int*)_Tmp6)){case 13: _Tmp8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp6)->f1;_Tmp7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_Tmp6)->f2;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_List_List*ts=_Tmp7;
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(e1);goto _LL1D;}case 1: _Tmp8=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp6)->f1;{void*b=_Tmp8;
fname_ok=Cyc_Tcstmt_ensure_no_throw_exp(fname);goto _LL1D;}default:
 goto _LL1D;}_LL1D:;}
# 586
if(fname_ok){
void*_Tmp6=fname->topt;struct Cyc_Absyn_FnInfo _Tmp7;if(_Tmp6!=0){if(*((int*)_Tmp6)==4){if(*((int*)((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1.elt_type)==6){_Tmp7=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1.elt_type)->f1;{struct Cyc_Absyn_FnInfo fi=_Tmp7;
# 589
fname_ok=Cyc_List_mem(Cyc_Atts_attribute_cmp,fi.attributes,(void*)& Cyc_Atts_No_throw_att_val);
goto _LL24;}}else{goto _LL27;}}else{goto _LL27;}}else{_LL27:
# 592
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("Expected function pointer type",sizeof(char),31U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;({int(*_TmpA)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_Warn_impos2;_TmpA;})(_tag_fat(_Tmp9,sizeof(void*),1));});}_LL24:;}
# 594
return fname_ok &&({int(*_Tmp6)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;_Tmp6;})(Cyc_Tcstmt_ensure_no_throw_exp,args);}}case 1: _Tmp5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{void*b=_Tmp5;
# 596
switch(*((int*)b)){case 3:
# 618 "tcstmt.cyc"
 goto _LL2D;case 4: _LL2D:
 return 1;default:
 return 0;};}case 15: _Tmp5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
# 622
return Cyc_Tcstmt_ensure_no_throw_exp(e1);}case 28: _Tmp5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;{struct Cyc_List_List*exps=_Tmp5;
# 624
int(*_Tmp6)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=({int(*_Tmp7)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*),struct Cyc_List_List*))Cyc_List_forall;_Tmp7;});return _Tmp6(Cyc_Tcstmt_ensure_no_throw_exp,({struct Cyc_List_List*(*_Tmp7)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*)(struct _tuple17*),struct Cyc_List_List*))Cyc_List_map;_Tmp7;})(({struct Cyc_Absyn_Exp*(*_Tmp7)(struct _tuple17*)=(struct Cyc_Absyn_Exp*(*)(struct _tuple17*))Cyc_Core_snd;_Tmp7;}),exps));}case 14: _Tmp5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_Tmp4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_Tmp0)->f4;{void*ct=_Tmp5;struct Cyc_Absyn_Exp*e1=_Tmp4;int b=_Tmp2;enum Cyc_Absyn_Coercion c=_Tmp1;
# 626
return !b && Cyc_Tcstmt_ensure_no_throw_exp(e1);}case 20: _Tmp5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp5;
_Tmp5=e1;goto _LL1A;}case 22: _Tmp5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_Tmp5;
# 629
void*_Tmp6=e1->topt;void*_Tmp7;if(_Tmp6!=0){if(*((int*)_Tmp6)==4){_Tmp7=(struct Cyc_Absyn_PtrInfo*)&((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_Tmp6)->f1;{struct Cyc_Absyn_PtrInfo*pi=(struct Cyc_Absyn_PtrInfo*)_Tmp7;
return !((unsigned)pi->ptr_atts.nullable);}}else{goto _LL33;}}else{_LL33:
 return 0;};}default:
# 633
 return 0;};}
# 645 "tcstmt.cyc"
static int Cyc_Tcstmt_nothrow_visit_f1(int*env,struct Cyc_Absyn_Exp*e){
({int _Tmp0=Cyc_Tcstmt_ensure_no_throw_exp(e);*env=_Tmp0;});
return 0;}
# 649
static int Cyc_Tcstmt_nothrow_visit_f2(int*env,struct Cyc_Absyn_Stmt*s){
# 651
void*_Tmp0=s->r;if(*((int*)_Tmp0)==15){
# 653
({int _Tmp1=*env && Cyc_Tcstmt_check_catchall_handler(s);*env=_Tmp1;});return 0;}else{
return 1;};}
# 657
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*s){
int env=1;
({void(*_Tmp0)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Stmt*)=(void(*)(int(*)(int*,struct Cyc_Absyn_Exp*),int(*)(int*,struct Cyc_Absyn_Stmt*),int*,struct Cyc_Absyn_Stmt*))Cyc_Absyn_visit_stmt;_Tmp0;})(Cyc_Tcstmt_nothrow_visit_f1,Cyc_Tcstmt_nothrow_visit_f2,& env,s);
return env;}
