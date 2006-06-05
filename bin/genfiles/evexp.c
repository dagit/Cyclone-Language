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
 struct Cyc_Core_Opt{void*v;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct _tuple2{enum Cyc_Absyn_AggrKind f0;struct _tuple0*f1;struct Cyc_Core_Opt*f2;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 524 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U,Cyc_Absyn_UDiv =20U,Cyc_Absyn_UMod =21U,Cyc_Absyn_UGt =22U,Cyc_Absyn_ULt =23U,Cyc_Absyn_UGte =24U,Cyc_Absyn_ULte =25U};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};
# 549
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;int f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
# 939
void*Cyc_Absyn_compress(void*);
# 957
extern void*Cyc_Absyn_uint_type;
# 1045
union Cyc_Absyn_Cnst Cyc_Absyn_Char_c(enum Cyc_Absyn_Sign,char);
# 1047
union Cyc_Absyn_Cnst Cyc_Absyn_Short_c(enum Cyc_Absyn_Sign,short);
union Cyc_Absyn_Cnst Cyc_Absyn_Int_c(enum Cyc_Absyn_Sign,int);
union Cyc_Absyn_Cnst Cyc_Absyn_LongLong_c(enum Cyc_Absyn_Sign,long long);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
# 1235
struct _tuple0*Cyc_Absyn_binding2qvar(void*);
# 29 "unify.h"
int Cyc_Unify_unify(void*,void*);
# 73 "tcutil.h"
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*);
# 128 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);
# 134
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
# 30 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
# 83 "kinds.h"
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*);
# 37 "warn.h"
void Cyc_Warn_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 48
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 79
void*Cyc_Warn_impos2(struct _fat_ptr);struct _tuple11{unsigned f0;int f1;};
# 46 "evexp.h"
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);struct _tuple12{union Cyc_Absyn_Cnst f0;int f1;};
# 33 "evexp.cyc"
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*);
# 35
static union Cyc_Absyn_Cnst Cyc_Evexp_promote_const(union Cyc_Absyn_Cnst cn){union Cyc_Absyn_Cnst _T0;struct _union_Cnst_Short_c _T1;unsigned _T2;union Cyc_Absyn_Cnst _T3;struct _union_Cnst_Char_c _T4;struct _tuple3 _T5;union Cyc_Absyn_Cnst _T6;struct _union_Cnst_Char_c _T7;struct _tuple3 _T8;enum Cyc_Absyn_Sign _T9;char _TA;int _TB;union Cyc_Absyn_Cnst _TC;union Cyc_Absyn_Cnst _TD;struct _union_Cnst_Short_c _TE;struct _tuple4 _TF;union Cyc_Absyn_Cnst _T10;struct _union_Cnst_Short_c _T11;struct _tuple4 _T12;enum Cyc_Absyn_Sign _T13;short _T14;int _T15;union Cyc_Absyn_Cnst _T16;union Cyc_Absyn_Cnst _T17;short _T18;char _T19;enum Cyc_Absyn_Sign _T1A;_T0=cn;_T1=_T0.Short_c;_T2=_T1.tag;switch(_T2){case 2: _T3=cn;_T4=_T3.Char_c;_T5=_T4.val;_T1A=_T5.f0;_T6=cn;_T7=_T6.Char_c;_T8=_T7.val;_T19=_T8.f1;{enum Cyc_Absyn_Sign sn=_T1A;char c=_T19;_T9=sn;_TA=c;_TB=(int)_TA;_TC=
# 37
Cyc_Absyn_Int_c(_T9,_TB);return _TC;}case 4: _TD=cn;_TE=_TD.Short_c;_TF=_TE.val;_T1A=_TF.f0;_T10=cn;_T11=_T10.Short_c;_T12=_T11.val;_T18=_T12.f1;{enum Cyc_Absyn_Sign sn=_T1A;short s=_T18;_T13=sn;_T14=s;_T15=(int)_T14;_T16=
# 39
Cyc_Absyn_Int_c(_T13,_T15);return _T16;}default: _T17=cn;
return _T17;};}
# 44
struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e){int _T0;struct _tuple11 _T1;struct _union_Cnst_Null_c _T2;unsigned _T3;struct _union_Cnst_Int_c _T4;struct _tuple5 _T5;struct _tuple11 _T6;int _T7;struct _union_Cnst_LongLong_c _T8;struct _tuple6 _T9;long long _TA;unsigned long long _TB;struct _tuple11 _TC;struct _tuple11 _TD;long long _TE;struct _tuple11 _TF;struct _tuple11 _T10;struct _tuple11 _T11;
struct _tuple12 _T12=Cyc_Evexp_eval_const_exp(e);int _T13;union Cyc_Absyn_Cnst _T14;_T14=_T12.f0;_T13=_T12.f1;{union Cyc_Absyn_Cnst cn=_T14;int known=_T13;_T0=known;
if(_T0)goto _TL1;else{goto _TL3;}
_TL3:{struct _tuple11 _T15;_T15.f0=0U;_T15.f1=0;_T1=_T15;}return _T1;_TL1: {
union Cyc_Absyn_Cnst _T15=Cyc_Evexp_promote_const(cn);long long _T16;int _T17;_T2=_T15.Null_c;_T3=_T2.tag;switch(_T3){case 5: _T4=_T15.Int_c;_T5=_T4.val;_T17=_T5.f1;{int i=_T17;{struct _tuple11 _T18;_T7=i;
_T18.f0=(unsigned)_T7;_T18.f1=1;_T6=_T18;}return _T6;}case 6: _T8=_T15.LongLong_c;_T9=_T8.val;_T16=_T9.f1;{long long x=_T16;_TA=x;_TB=(unsigned long long)_TA;{
# 52
unsigned long long y=_TB >> 32;
if(y==4294967295U)goto _TL5;if(y==0U)goto _TL5;{struct _tuple11 _T18;
_T18.f0=0U;_T18.f1=0;_TC=_T18;}return _TC;_TL5:{struct _tuple11 _T18;_TE=x;
_T18.f0=(unsigned)_TE;_T18.f1=1;_TD=_T18;}return _TD;}}case 7:{struct _tuple11 _T18;
_T18.f0=0U;_T18.f1=0;_TF=_T18;}return _TF;case 1:{struct _tuple11 _T18;
_T18.f0=0U;_T18.f1=1;_T10=_T18;}return _T10;default:{struct _tuple11 _T18;
_T18.f0=0U;_T18.f1=0;_T11=_T18;}return _T11;};}}}struct _tuple13{int f0;int f1;};
# 62
static struct _tuple13 Cyc_Evexp_eval_const_bool_exp(struct Cyc_Absyn_Exp*e){int _T0;struct _tuple13 _T1;struct _union_Cnst_Float_c _T2;unsigned _T3;struct _union_Cnst_Int_c _T4;struct _tuple5 _T5;struct _tuple13 _T6;struct _union_Cnst_LongLong_c _T7;struct _tuple6 _T8;struct _tuple13 _T9;struct _tuple13 _TA;struct _tuple13 _TB;struct _tuple13 _TC;
struct _tuple12 _TD=Cyc_Evexp_eval_const_exp(e);int _TE;union Cyc_Absyn_Cnst _TF;_TF=_TD.f0;_TE=_TD.f1;{union Cyc_Absyn_Cnst cn=_TF;int known=_TE;_T0=known;
if(_T0)goto _TL7;else{goto _TL9;}
_TL9:{struct _tuple13 _T10;_T10.f0=0;_T10.f1=0;_T1=_T10;}return _T1;_TL7: {
union Cyc_Absyn_Cnst _T10=Cyc_Evexp_promote_const(cn);long long _T11;int _T12;_T2=_T10.Float_c;_T3=_T2.tag;switch(_T3){case 5: _T4=_T10.Int_c;_T5=_T4.val;_T12=_T5.f1;{int b=_T12;{struct _tuple13 _T13;
_T13.f0=b!=0;_T13.f1=1;_T6=_T13;}return _T6;}case 6: _T7=_T10.LongLong_c;_T8=_T7.val;_T11=_T8.f1;{long long b=_T11;{struct _tuple13 _T13;
_T13.f0=b!=0;_T13.f1=1;_T9=_T13;}return _T9;}case 1:{struct _tuple13 _T13;
_T13.f0=0;_T13.f1=0;_TA=_T13;}return _TA;case 7:{struct _tuple13 _T13;
_T13.f0=0;_T13.f1=1;_TB=_T13;}return _TB;default:{struct _tuple13 _T13;
_T13.f0=0;_T13.f1=0;_TC=_T13;}return _TC;};}}}struct _tuple14{enum Cyc_Absyn_Primop f0;union Cyc_Absyn_Cnst f1;};
# 75
static struct _tuple12 Cyc_Evexp_eval_const_unprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e){int _T0;struct _tuple12 _T1;struct _tuple14 _T2;enum Cyc_Absyn_Primop _T3;int _T4;union Cyc_Absyn_Cnst _T5;struct _union_Cnst_LongLong_c _T6;unsigned _T7;union Cyc_Absyn_Cnst _T8;struct _union_Cnst_Int_c _T9;struct _tuple5 _TA;union Cyc_Absyn_Cnst _TB;struct _union_Cnst_Int_c _TC;struct _tuple5 _TD;int _TE;union Cyc_Absyn_Cnst _TF;struct _union_Cnst_Short_c _T10;struct _tuple4 _T11;union Cyc_Absyn_Cnst _T12;struct _union_Cnst_Short_c _T13;struct _tuple4 _T14;short _T15;union Cyc_Absyn_Cnst _T16;struct _union_Cnst_Char_c _T17;struct _tuple3 _T18;union Cyc_Absyn_Cnst _T19;struct _union_Cnst_Char_c _T1A;struct _tuple3 _T1B;char _T1C;union Cyc_Absyn_Cnst _T1D;struct _union_Cnst_LongLong_c _T1E;struct _tuple6 _T1F;union Cyc_Absyn_Cnst _T20;struct _union_Cnst_LongLong_c _T21;struct _tuple6 _T22;long long _T23;union Cyc_Absyn_Cnst _T24;struct _union_Cnst_LongLong_c _T25;unsigned _T26;union Cyc_Absyn_Cnst _T27;struct _union_Cnst_Int_c _T28;struct _tuple5 _T29;union Cyc_Absyn_Cnst _T2A;struct _union_Cnst_Int_c _T2B;struct _tuple5 _T2C;int _T2D;union Cyc_Absyn_Cnst _T2E;struct _union_Cnst_Short_c _T2F;struct _tuple4 _T30;union Cyc_Absyn_Cnst _T31;struct _union_Cnst_Short_c _T32;struct _tuple4 _T33;short _T34;union Cyc_Absyn_Cnst _T35;struct _union_Cnst_Char_c _T36;struct _tuple3 _T37;union Cyc_Absyn_Cnst _T38;struct _union_Cnst_Char_c _T39;struct _tuple3 _T3A;char _T3B;union Cyc_Absyn_Cnst _T3C;struct _union_Cnst_LongLong_c _T3D;struct _tuple6 _T3E;union Cyc_Absyn_Cnst _T3F;struct _union_Cnst_LongLong_c _T40;struct _tuple6 _T41;long long _T42;union Cyc_Absyn_Cnst _T43;struct _union_Cnst_LongLong_c _T44;unsigned _T45;union Cyc_Absyn_Cnst _T46;struct _union_Cnst_Int_c _T47;struct _tuple5 _T48;int _T49;union Cyc_Absyn_Cnst _T4A;struct _union_Cnst_Short_c _T4B;struct _tuple4 _T4C;short _T4D;short _T4E;int _T4F;union Cyc_Absyn_Cnst _T50;struct _union_Cnst_Char_c _T51;struct _tuple3 _T52;char _T53;char _T54;int _T55;union Cyc_Absyn_Cnst _T56;struct _union_Cnst_LongLong_c _T57;struct _tuple6 _T58;int _T59;long long _T5A;struct _tuple12 _T5B;struct _tuple12 _T5C;
struct _tuple12 _T5D=Cyc_Evexp_eval_const_exp(e);int _T5E;union Cyc_Absyn_Cnst _T5F;_T5F=_T5D.f0;_T5E=_T5D.f1;{union Cyc_Absyn_Cnst cn=_T5F;int known=_T5E;_T0=known;
if(_T0)goto _TLB;else{goto _TLD;}
_TLD:{struct _tuple12 _T60;_T60.f0=cn;_T60.f1=0;_T1=_T60;}return _T1;_TLB:{struct _tuple14 _T60;
_T60.f0=p;_T60.f1=cn;_T2=_T60;}{struct _tuple14 _T60=_T2;long long _T61;char _T62;short _T63;int _T64;enum Cyc_Absyn_Sign _T65;_T3=_T60.f0;_T4=(int)_T3;switch(_T4){case Cyc_Absyn_Plus: goto _LL3;case Cyc_Absyn_Minus: _T5=_T60.f1;_T6=_T5.LongLong_c;_T7=_T6.tag;switch(_T7){case 5: _T8=_T60.f1;_T9=_T8.Int_c;_TA=_T9.val;_T65=_TA.f0;_TB=_T60.f1;_TC=_TB.Int_c;_TD=_TC.val;_T64=_TD.f1;{enum Cyc_Absyn_Sign s=_T65;int i=_T64;_TE=- i;
# 81
cn=Cyc_Absyn_Int_c(0U,_TE);goto _LL3;}case 4: _TF=_T60.f1;_T10=_TF.Short_c;_T11=_T10.val;_T65=_T11.f0;_T12=_T60.f1;_T13=_T12.Short_c;_T14=_T13.val;_T63=_T14.f1;{enum Cyc_Absyn_Sign s=_T65;short i=_T63;_T15=- i;
cn=Cyc_Absyn_Short_c(0U,_T15);goto _LL3;}case 2: _T16=_T60.f1;_T17=_T16.Char_c;_T18=_T17.val;_T65=_T18.f0;_T19=_T60.f1;_T1A=_T19.Char_c;_T1B=_T1A.val;_T62=_T1B.f1;{enum Cyc_Absyn_Sign s=_T65;char i=_T62;_T1C=- i;
cn=Cyc_Absyn_Char_c(0U,_T1C);goto _LL3;}case 6: _T1D=_T60.f1;_T1E=_T1D.LongLong_c;_T1F=_T1E.val;_T65=_T1F.f0;_T20=_T60.f1;_T21=_T20.LongLong_c;_T22=_T21.val;_T61=_T22.f1;{enum Cyc_Absyn_Sign s=_T65;long long i=_T61;_T23=- i;
cn=Cyc_Absyn_LongLong_c(0U,_T23);goto _LL3;}default: goto _LL20;};case Cyc_Absyn_Bitnot: _T24=_T60.f1;_T25=_T24.LongLong_c;_T26=_T25.tag;switch(_T26){case 5: _T27=_T60.f1;_T28=_T27.Int_c;_T29=_T28.val;_T65=_T29.f0;_T2A=_T60.f1;_T2B=_T2A.Int_c;_T2C=_T2B.val;_T64=_T2C.f1;{enum Cyc_Absyn_Sign s=_T65;int i=_T64;_T2D=~ i;
cn=Cyc_Absyn_Int_c(1U,_T2D);goto _LL3;}case 4: _T2E=_T60.f1;_T2F=_T2E.Short_c;_T30=_T2F.val;_T65=_T30.f0;_T31=_T60.f1;_T32=_T31.Short_c;_T33=_T32.val;_T63=_T33.f1;{enum Cyc_Absyn_Sign s=_T65;short i=_T63;_T34=~ i;
cn=Cyc_Absyn_Short_c(1U,_T34);goto _LL3;}case 2: _T35=_T60.f1;_T36=_T35.Char_c;_T37=_T36.val;_T65=_T37.f0;_T38=_T60.f1;_T39=_T38.Char_c;_T3A=_T39.val;_T62=_T3A.f1;{enum Cyc_Absyn_Sign s=_T65;char i=_T62;_T3B=~ i;
cn=Cyc_Absyn_Char_c(1U,_T3B);goto _LL3;}case 6: _T3C=_T60.f1;_T3D=_T3C.LongLong_c;_T3E=_T3D.val;_T65=_T3E.f0;_T3F=_T60.f1;_T40=_T3F.LongLong_c;_T41=_T40.val;_T61=_T41.f1;{enum Cyc_Absyn_Sign s=_T65;long long i=_T61;_T42=~ i;
cn=Cyc_Absyn_LongLong_c(1U,_T42);goto _LL3;}default: goto _LL20;};case Cyc_Absyn_Not: _T43=_T60.f1;_T44=_T43.LongLong_c;_T45=_T44.tag;switch(_T45){case 5: _T46=_T60.f1;_T47=_T46.Int_c;_T48=_T47.val;_T64=_T48.f1;{int i=_T64;
if(i!=0)goto _TL12;_T49=1;goto _TL13;_TL12: _T49=0;_TL13: cn=Cyc_Absyn_Int_c(0U,_T49);goto _LL3;}case 4: _T4A=_T60.f1;_T4B=_T4A.Short_c;_T4C=_T4B.val;_T63=_T4C.f1;{short i=_T63;_T4E=i;_T4F=(int)_T4E;
if(_T4F!=0)goto _TL14;_T4D=1;goto _TL15;_TL14: _T4D=0;_TL15: cn=Cyc_Absyn_Short_c(0U,_T4D);goto _LL3;}case 2: _T50=_T60.f1;_T51=_T50.Char_c;_T52=_T51.val;_T62=_T52.f1;{char i=_T62;_T54=i;_T55=(int)_T54;
if(_T55!=0)goto _TL16;_T53='\001';goto _TL17;_TL16: _T53='\000';_TL17: cn=Cyc_Absyn_Char_c(0U,_T53);goto _LL3;}case 1:
 cn=Cyc_Absyn_Int_c(0U,1);goto _LL3;case 6: _T56=_T60.f1;_T57=_T56.LongLong_c;_T58=_T57.val;_T61=_T58.f1;{long long i=_T61;
# 94
if(i!=0)goto _TL18;_T59=1;goto _TL19;_TL18: _T59=0;_TL19: _T5A=(long long)_T59;cn=Cyc_Absyn_LongLong_c(0U,_T5A);goto _LL3;}default: goto _LL20;};default: _LL20:{struct _tuple12 _T66;
_T66.f0=cn;_T66.f1=0;_T5B=_T66;}return _T5B;}_LL3:;}{struct _tuple12 _T60;
# 97
_T60.f0=cn;_T60.f1=1;_T5C=_T60;}return _T5C;}}struct _tuple15{enum Cyc_Absyn_Primop f0;int f1;};
# 101
static struct _tuple12 Cyc_Evexp_eval_const_binprimop(enum Cyc_Absyn_Primop p,struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int _T0;int _T1;struct _tuple12 _T2;union Cyc_Absyn_Cnst _T3;struct _union_Cnst_Int_c _T4;unsigned _T5;union Cyc_Absyn_Cnst _T6;struct _union_Cnst_Int_c _T7;struct _tuple5 _T8;union Cyc_Absyn_Cnst _T9;struct _union_Cnst_Int_c _TA;struct _tuple5 _TB;struct _tuple12 _TC;union Cyc_Absyn_Cnst _TD;struct _union_Cnst_Int_c _TE;unsigned _TF;union Cyc_Absyn_Cnst _T10;struct _union_Cnst_Int_c _T11;struct _tuple5 _T12;union Cyc_Absyn_Cnst _T13;struct _union_Cnst_Int_c _T14;struct _tuple5 _T15;struct _tuple12 _T16;enum Cyc_Absyn_Primop _T17;int _T18;struct Cyc_Warn_String_Warn_Warg_struct _T19;struct Cyc_Absyn_Exp*_T1A;unsigned _T1B;struct _fat_ptr _T1C;struct _tuple12 _T1D;int _T1E;enum Cyc_Absyn_Sign _T1F;int _T20;enum Cyc_Absyn_Sign _T21;int _T22;int _T23;int _T24;struct _tuple15 _T25;enum Cyc_Absyn_Primop _T26;int _T27;int _T28;int _T29;int _T2A;int _T2B;int _T2C;int _T2D;int _T2E;int(*_T2F)(struct _fat_ptr,struct _fat_ptr);void*(*_T30)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T31;struct _fat_ptr _T32;int _T33;int(*_T34)(struct _fat_ptr,struct _fat_ptr);void*(*_T35)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T36;struct _fat_ptr _T37;int _T38;int(*_T39)(struct _fat_ptr,struct _fat_ptr);void*(*_T3A)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T3B;struct _fat_ptr _T3C;int _T3D;int(*_T3E)(struct _fat_ptr,struct _fat_ptr);void*(*_T3F)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T40;struct _fat_ptr _T41;int _T42;int _T43;int _T44;int _T45;int(*_T46)(struct _fat_ptr,struct _fat_ptr);void*(*_T47)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T48;struct _fat_ptr _T49;int _T4A;int _T4B;int _T4C;int _T4D;int(*_T4E)(struct _fat_ptr,struct _fat_ptr);void*(*_T4F)(struct _fat_ptr,struct _fat_ptr);struct _fat_ptr _T50;struct _fat_ptr _T51;enum Cyc_Absyn_Sign _T52;int _T53;struct Cyc_Absyn_Exp*_T54;unsigned _T55;struct _fat_ptr _T56;struct _fat_ptr _T57;enum Cyc_Absyn_Sign _T58;int _T59;struct Cyc_Absyn_Exp*_T5A;unsigned _T5B;struct _fat_ptr _T5C;struct _fat_ptr _T5D;enum Cyc_Absyn_Sign _T5E;int _T5F;struct Cyc_Warn_String_Warn_Warg_struct _T60;struct Cyc_Absyn_Exp*_T61;unsigned _T62;struct _fat_ptr _T63;struct _tuple12 _T64;int _T65;struct _tuple12 _T66;int _T67;struct _tuple12 _T68;unsigned _T69;int _T6A;int _T6B;struct _tuple12 _T6C;int _T6D;int _T6E;struct Cyc_Warn_String_Warn_Warg_struct _T6F;int(*_T70)(struct _fat_ptr);void*(*_T71)(struct _fat_ptr);struct _fat_ptr _T72;
struct _tuple12 _T73=Cyc_Evexp_eval_const_exp(e1);int _T74;union Cyc_Absyn_Cnst _T75;_T75=_T73.f0;_T74=_T73.f1;{union Cyc_Absyn_Cnst cn1=_T75;int known1=_T74;
struct _tuple12 _T76=Cyc_Evexp_eval_const_exp(e2);int _T77;union Cyc_Absyn_Cnst _T78;_T78=_T76.f0;_T77=_T76.f1;{union Cyc_Absyn_Cnst cn2=_T78;int known2=_T77;_T0=known1;
if(_T0)goto _TL1D;else{goto _TL1C;}_TL1D: _T1=known2;if(_T1)goto _TL1A;else{goto _TL1C;}
_TL1C:{struct _tuple12 _T79;_T79.f0=cn1;_T79.f1=0;_T2=_T79;}return _T2;_TL1A:
 cn1=Cyc_Evexp_promote_const(cn1);
cn2=Cyc_Evexp_promote_const(cn2);{
enum Cyc_Absyn_Sign s1;enum Cyc_Absyn_Sign s2;
int i1;int i2;{int _T79;enum Cyc_Absyn_Sign _T7A;_T3=cn1;_T4=_T3.Int_c;_T5=_T4.tag;if(_T5!=5)goto _TL1E;_T6=cn1;_T7=_T6.Int_c;_T8=_T7.val;_T7A=_T8.f0;_T9=cn1;_TA=_T9.Int_c;_TB=_TA.val;_T79=_TB.f1;{enum Cyc_Absyn_Sign x=_T7A;int y=_T79;
# 111
s1=x;i1=y;goto _LL6;}_TL1E:{struct _tuple12 _T7B;
_T7B.f0=cn1;_T7B.f1=0;_TC=_T7B;}return _TC;_LL6:;}{int _T79;enum Cyc_Absyn_Sign _T7A;_TD=cn2;_TE=_TD.Int_c;_TF=_TE.tag;if(_TF!=5)goto _TL20;_T10=cn2;_T11=_T10.Int_c;_T12=_T11.val;_T7A=_T12.f0;_T13=cn2;_T14=_T13.Int_c;_T15=_T14.val;_T79=_T15.f1;{enum Cyc_Absyn_Sign x=_T7A;int y=_T79;
# 115
s2=x;i2=y;goto _LLB;}_TL20:{struct _tuple12 _T7B;
_T7B.f0=cn1;_T7B.f1=0;_T16=_T7B;}return _T16;_LLB:;}_T17=p;_T18=(int)_T17;switch(_T18){case Cyc_Absyn_UDiv: goto _LL14;case Cyc_Absyn_Div: _LL14: goto _LL16;case Cyc_Absyn_UMod: _LL16: goto _LL18;case Cyc_Absyn_Mod: _LL18:
# 123
 if(i2==0)goto _TL23;goto _LL10;_TL23:{struct Cyc_Warn_String_Warn_Warg_struct _T79;_T79.tag=0;
# 125
_T79.f1=_tag_fat("division by zero in constant expression",sizeof(char),40U);_T19=_T79;}{struct Cyc_Warn_String_Warn_Warg_struct _T79=_T19;void*_T7A[1];_T7A[0]=& _T79;_T1A=e2;_T1B=_T1A->loc;_T1C=_tag_fat(_T7A,sizeof(void*),1);Cyc_Warn_err2(_T1B,_T1C);}{struct _tuple12 _T79;
_T79.f0=cn1;_T79.f1=1;_T1D=_T79;}return _T1D;default: goto _LL10;}_LL10: _T1F=s1;_T20=(int)_T1F;
# 129
if(_T20!=1)goto _TL25;_T1E=1;goto _TL26;_TL25: _T21=s2;_T22=(int)_T21;_T1E=_T22==1;_TL26: {int has_u_arg=_T1E;_T23=i1;{
unsigned u1=(unsigned)_T23;_T24=i2;{
unsigned u2=(unsigned)_T24;
int i3=0;
unsigned u3=0U;
int b3=1;
int use_i3=0;
int use_u3=0;
int use_b3=0;{struct _tuple15 _T79;
_T79.f0=p;_T79.f1=has_u_arg;_T25=_T79;}{struct _tuple15 _T79=_T25;_T26=_T79.f0;_T27=(int)_T26;switch(_T27){case Cyc_Absyn_Plus: _T28=_T79.f1;_T29=(int)_T28;switch(_T29){case 0:
 i3=i1 + i2;use_i3=1;goto _LL1B;case 1:
# 146
 u3=u1 + u2;use_u3=1;goto _LL1B;default: goto _LL66;};case Cyc_Absyn_Times: _T2A=_T79.f1;_T2B=(int)_T2A;switch(_T2B){case 0:
# 140
 i3=i1 * i2;use_i3=1;goto _LL1B;case 1:
# 147
 u3=u1 * u2;use_u3=1;goto _LL1B;default: goto _LL66;};case Cyc_Absyn_Minus: _T2C=_T79.f1;_T2D=(int)_T2C;switch(_T2D){case 0:
# 141
 i3=i1 - i2;use_i3=1;goto _LL1B;case 1:
# 148
 u3=u1 - u2;use_u3=1;goto _LL1B;default: goto _LL66;};case Cyc_Absyn_Div: _T2E=_T79.f1;if(_T2E!=0)goto _TL2B;
# 142
i3=i1 / i2;use_i3=1;goto _LL1B;_TL2B: _T30=Cyc_Warn_impos;{
# 144
int(*_T7A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T30;_T2F=_T7A;}_T31=_tag_fat("evexp: signed division on unsigned args!",sizeof(char),41U);_T32=_tag_fat(0U,sizeof(void*),0);_T2F(_T31,_T32);;case Cyc_Absyn_Mod: _T33=_T79.f1;if(_T33!=0)goto _TL2D;
# 143
i3=i1 % i2;use_i3=1;goto _LL1B;_TL2D: _T35=Cyc_Warn_impos;{
# 145
int(*_T7A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T35;_T34=_T7A;}_T36=_tag_fat("evexp: signed mod on unsigned args!",sizeof(char),36U);_T37=_tag_fat(0U,sizeof(void*),0);_T34(_T36,_T37);;case Cyc_Absyn_UDiv: _T38=_T79.f1;if(_T38!=1)goto _TL2F;
# 149
u3=u1 / u2;use_u3=1;goto _LL1B;_TL2F: _T3A=Cyc_Warn_impos;{
# 151
int(*_T7A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T3A;_T39=_T7A;}_T3B=_tag_fat("evexp: unsigned division on signed args!",sizeof(char),41U);_T3C=_tag_fat(0U,sizeof(void*),0);_T39(_T3B,_T3C);;case Cyc_Absyn_UMod: _T3D=_T79.f1;if(_T3D!=1)goto _TL31;
# 150
u3=u1 % u2;use_u3=1;goto _LL1B;_TL31: _T3F=Cyc_Warn_impos;{
# 152
int(*_T7A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T3F;_T3E=_T7A;}_T40=_tag_fat("evexp: unsigned mod on signed args!",sizeof(char),36U);_T41=_tag_fat(0U,sizeof(void*),0);_T3E(_T40,_T41);;case Cyc_Absyn_Eq:
 b3=i1==i2;use_b3=1;goto _LL1B;case Cyc_Absyn_Neq:
 b3=i1!=i2;use_b3=1;goto _LL1B;case Cyc_Absyn_Gt: _T42=_T79.f1;if(_T42!=0)goto _TL33;
b3=i1 > i2;use_b3=1;goto _LL1B;_TL33: goto _LL47;case Cyc_Absyn_Lt: _T43=_T79.f1;if(_T43!=0)goto _TL35;
b3=i1 < i2;use_b3=1;goto _LL1B;_TL35: _LL47: goto _LL49;case Cyc_Absyn_Gte: _T44=_T79.f1;if(_T44!=0)goto _TL37;
b3=i1 >= i2;use_b3=1;goto _LL1B;_TL37: _LL49: goto _LL4B;case Cyc_Absyn_Lte: _T45=_T79.f1;if(_T45!=0)goto _TL39;
b3=i1 <= i2;use_b3=1;goto _LL1B;_TL39: _LL4B: _T47=Cyc_Warn_impos;{
# 162
int(*_T7A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T47;_T46=_T7A;}_T48=_tag_fat("evexp: signed comparison on unsigned args!",sizeof(char),43U);_T49=_tag_fat(0U,sizeof(void*),0);_T46(_T48,_T49);;case Cyc_Absyn_UGt: _T4A=_T79.f1;if(_T4A!=1)goto _TL3B;
b3=u1 > u2;use_b3=1;goto _LL1B;_TL3B: goto _LL57;case Cyc_Absyn_ULt: _T4B=_T79.f1;if(_T4B!=1)goto _TL3D;
b3=u1 < u2;use_b3=1;goto _LL1B;_TL3D: _LL57: goto _LL59;case Cyc_Absyn_UGte: _T4C=_T79.f1;if(_T4C!=1)goto _TL3F;
b3=u1 >= u2;use_b3=1;goto _LL1B;_TL3F: _LL59: goto _LL5B;case Cyc_Absyn_ULte: _T4D=_T79.f1;if(_T4D!=1)goto _TL41;
b3=u1 <= u2;use_b3=1;goto _LL1B;_TL41: _LL5B: _T4F=Cyc_Warn_impos;{
# 170
int(*_T7A)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))_T4F;_T4E=_T7A;}_T50=_tag_fat("evexp: unsigned comparison on signed args!",sizeof(char),43U);_T51=_tag_fat(0U,sizeof(void*),0);_T4E(_T50,_T51);;case Cyc_Absyn_Bitand:
 u3=u1 & u2;use_u3=1;goto _LL1B;case Cyc_Absyn_Bitor:
 u3=u1 | u2;use_u3=1;goto _LL1B;case Cyc_Absyn_Bitxor:
 u3=u1 ^ u2;use_u3=1;goto _LL1B;case Cyc_Absyn_Bitlshift: _T52=s2;_T53=(int)_T52;
# 175
if(_T53!=0)goto _TL43;if(i2 >= 0)goto _TL43;_T54=e2;_T55=_T54->loc;_T56=
_tag_fat("left shift count is negative",sizeof(char),29U);_T57=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T55,_T56,_T57);goto _TL44;_TL43: _TL44:
# 179
 i3=i1 << i2;use_i3=1;goto _LL1B;case Cyc_Absyn_Bitlrshift: _T58=s2;_T59=(int)_T58;
# 181
if(_T59!=0)goto _TL45;if(i2 >= 0)goto _TL45;_T5A=e2;_T5B=_T5A->loc;_T5C=
_tag_fat("right shift count is negative",sizeof(char),30U);_T5D=_tag_fat(0U,sizeof(void*),0);Cyc_Warn_warn(_T5B,_T5C,_T5D);goto _TL46;_TL45: _TL46: _T5E=s1;_T5F=(int)_T5E;
if(_T5F!=0)goto _TL47;
i3=i1 >> i2;use_i3=1;goto _TL48;
# 187
_TL47: u3=u1 >> i2;use_u3=1;_TL48: goto _LL1B;default: _LL66:{struct Cyc_Warn_String_Warn_Warg_struct _T7A;_T7A.tag=0;
# 190
_T7A.f1=_tag_fat("bad constant expression",sizeof(char),24U);_T60=_T7A;}{struct Cyc_Warn_String_Warn_Warg_struct _T7A=_T60;void*_T7B[1];_T7B[0]=& _T7A;_T61=e1;_T62=_T61->loc;_T63=_tag_fat(_T7B,sizeof(void*),1);Cyc_Warn_err2(_T62,_T63);}{struct _tuple12 _T7A;_T7A.f0=cn1;_T7A.f1=1;_T64=_T7A;}return _T64;}_LL1B:;}_T65=use_i3;
# 192
if(!_T65)goto _TL49;{struct _tuple12 _T79;_T79.f0=Cyc_Absyn_Int_c(0U,i3);_T79.f1=1;_T66=_T79;}return _T66;_TL49: _T67=use_u3;
if(!_T67)goto _TL4B;{struct _tuple12 _T79;_T69=u3;_T6A=(int)_T69;_T79.f0=Cyc_Absyn_Int_c(1U,_T6A);_T79.f1=1;_T68=_T79;}return _T68;_TL4B: _T6B=use_b3;
if(!_T6B)goto _TL4D;{struct _tuple12 _T79;_T6E=b3;if(!_T6E)goto _TL4F;_T6D=1;goto _TL50;_TL4F: _T6D=0;_TL50: _T79.f0=Cyc_Absyn_Int_c(0U,_T6D);_T79.f1=1;_T6C=_T79;}return _T6C;_TL4D:{struct Cyc_Warn_String_Warn_Warg_struct _T79;_T79.tag=0;
_T79.f1=_tag_fat("Evexp::eval_const_binop",sizeof(char),24U);_T6F=_T79;}{struct Cyc_Warn_String_Warn_Warg_struct _T79=_T6F;void*_T7A[1];_T7A[0]=& _T79;_T71=Cyc_Warn_impos2;{int(*_T7B)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T71;_T70=_T7B;}_T72=_tag_fat(_T7A,sizeof(void*),1);_T70(_T72);}}}}}}}}struct _tuple16{void*f0;union Cyc_Absyn_Cnst f1;};
# 200
static struct _tuple12 Cyc_Evexp_eval_const_exp(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;struct _tuple12 _T3;struct _tuple12 _T4;struct _tuple12 _T5;int _T6;struct _tuple12 _T7;struct _tuple12 _T8;int _T9;int _TA;struct _tuple12 _TB;struct _tuple12 _TC;int _TD;struct _tuple12 _TE;int _TF;struct _tuple12 _T10;struct _tuple12 _T11;int _T12;struct _tuple12 _T13;struct Cyc_Warn_String_Warn_Warg_struct _T14;struct Cyc_Absyn_Exp*_T15;unsigned _T16;struct _fat_ptr _T17;struct _tuple12 _T18;struct Cyc_List_List*_T19;struct Cyc_List_List*_T1A;enum Cyc_Absyn_Primop _T1B;struct Cyc_List_List*_T1C;void*_T1D;struct Cyc_Absyn_Exp*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;struct Cyc_List_List*_T21;enum Cyc_Absyn_Primop _T22;struct Cyc_List_List*_T23;void*_T24;struct Cyc_Absyn_Exp*_T25;struct Cyc_List_List*_T26;struct Cyc_List_List*_T27;void*_T28;struct Cyc_Absyn_Exp*_T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;struct Cyc_Absyn_Exp*_T2B;unsigned _T2C;struct _fat_ptr _T2D;struct _tuple12 _T2E;void*_T2F;int*_T30;int _T31;struct Cyc_Absyn_Exp*_T32;struct Cyc_Absyn_Exp*_T33;struct _tuple12 _T34;struct _tuple12 _T35;void*_T36;struct _tuple12 _T37;int _T38;struct _tuple16 _T39;struct _tuple12 _T3A;void*_T3B;int*_T3C;int _T3D;void*_T3E;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T3F;void*_T40;int*_T41;unsigned _T42;union Cyc_Absyn_Cnst _T43;struct _union_Cnst_Int_c _T44;unsigned _T45;void*_T46;void*_T47;union Cyc_Absyn_Cnst _T48;struct _union_Cnst_Char_c _T49;struct _tuple3 _T4A;union Cyc_Absyn_Cnst _T4B;struct _union_Cnst_Char_c _T4C;struct _tuple3 _T4D;char _T4E;void*_T4F;void*_T50;union Cyc_Absyn_Cnst _T51;struct _union_Cnst_Short_c _T52;struct _tuple4 _T53;union Cyc_Absyn_Cnst _T54;struct _union_Cnst_Short_c _T55;struct _tuple4 _T56;short _T57;void*_T58;void*_T59;union Cyc_Absyn_Cnst _T5A;struct _union_Cnst_Int_c _T5B;struct _tuple5 _T5C;union Cyc_Absyn_Cnst _T5D;struct _union_Cnst_Int_c _T5E;struct _tuple5 _T5F;enum Cyc_Absyn_Sign _T60;int _T61;enum Cyc_Absyn_Sign _T62;int _T63;void*_T64;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T65;struct Cyc_List_List*_T66;union Cyc_Absyn_Cnst _T67;struct _union_Cnst_Int_c _T68;unsigned _T69;void*_T6A;struct Cyc_List_List*_T6B;void*_T6C;union Cyc_Absyn_Cnst _T6D;struct _union_Cnst_Char_c _T6E;struct _tuple3 _T6F;union Cyc_Absyn_Cnst _T70;struct _union_Cnst_Char_c _T71;struct _tuple3 _T72;char _T73;void*_T74;struct Cyc_List_List*_T75;void*_T76;union Cyc_Absyn_Cnst _T77;struct _union_Cnst_Short_c _T78;struct _tuple4 _T79;union Cyc_Absyn_Cnst _T7A;struct _union_Cnst_Short_c _T7B;struct _tuple4 _T7C;short _T7D;void*_T7E;struct Cyc_List_List*_T7F;void*_T80;union Cyc_Absyn_Cnst _T81;struct _union_Cnst_Int_c _T82;struct _tuple5 _T83;union Cyc_Absyn_Cnst _T84;struct _union_Cnst_Int_c _T85;struct _tuple5 _T86;struct _tuple12 _T87;void*_T88;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T89;struct _tuple12 _T8A;union Cyc_Absyn_Cnst _T8B;void*_T8C;struct _tuple12 _T8D;struct Cyc_Absyn_Enumfield*_T8E;struct Cyc_Absyn_Exp*_T8F;struct Cyc_Absyn_Exp*_T90;struct _tuple12 _T91;struct _tuple12 _T92;int _T93;struct _tuple12 _T94;struct Cyc_Absyn_Exp*_T95;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T96;struct _tuple12 _T97;
struct _tuple12 ans;_T0=e;{
void*_T98=_T0->r;struct Cyc_Absyn_Enumfield*_T99;enum Cyc_Absyn_Primop _T9A;struct Cyc_Absyn_Exp*_T9B;struct Cyc_Absyn_Exp*_T9C;void*_T9D;union Cyc_Absyn_Cnst _T9E;_T1=(int*)_T98;_T2=*_T1;switch(_T2){case 1:{struct _tuple12 _T9F;
# 204
_T9F.f0=Cyc_Absyn_Int_c(0U,0);_T9F.f1=0;_T3=_T9F;}return _T3;case 0:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T98;_T9E=_T9F->f1;}{union Cyc_Absyn_Cnst c=_T9E;{struct _tuple12 _T9F;
_T9F.f0=c;_T9F.f1=1;_T4=_T9F;}return _T4;}case 2:{struct _tuple12 _T9F;
_T9F.f0=Cyc_Absyn_Int_c(0U,0);_T9F.f1=1;_T5=_T9F;}return _T5;case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T98;_T9D=_T9F->f1;_T9C=_T9F->f2;_T9B=_T9F->f3;}{struct Cyc_Absyn_Exp*e1=_T9D;struct Cyc_Absyn_Exp*e2=_T9C;struct Cyc_Absyn_Exp*e3=_T9B;
# 208
struct _tuple13 _T9F=Cyc_Evexp_eval_const_bool_exp(e1);int _TA0;int _TA1;_TA1=_T9F.f0;_TA0=_T9F.f1;{int bool1=_TA1;int known1=_TA0;_T6=known1;
if(_T6)goto _TL52;else{goto _TL54;}
_TL54: Cyc_Evexp_eval_const_exp(e2);
Cyc_Evexp_eval_const_exp(e3);{struct _tuple12 _TA2;
_TA2.f0=Cyc_Absyn_Int_c(0U,0);_TA2.f1=0;_T7=_TA2;}return _T7;_TL52: _T9=bool1;
# 214
if(!_T9)goto _TL55;_T8=Cyc_Evexp_eval_const_exp(e2);goto _TL56;_TL55: _T8=Cyc_Evexp_eval_const_exp(e3);_TL56: ans=_T8;goto _LL0;}}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T98;_T9D=_T9F->f1;_T9C=_T9F->f2;}{struct Cyc_Absyn_Exp*e1=_T9D;struct Cyc_Absyn_Exp*e2=_T9C;
# 217
struct _tuple13 _T9F=Cyc_Evexp_eval_const_bool_exp(e1);int _TA0;int _TA1;_TA1=_T9F.f0;_TA0=_T9F.f1;{int bool1=_TA1;int known1=_TA0;_TA=known1;
if(_TA)goto _TL57;else{goto _TL59;}
_TL59: Cyc_Evexp_eval_const_exp(e2);{struct _tuple12 _TA2;
_TA2.f0=Cyc_Absyn_Int_c(0U,0);_TA2.f1=0;_TB=_TA2;}return _TB;_TL57: _TD=bool1;
# 222
if(!_TD)goto _TL5A;_TC=Cyc_Evexp_eval_const_exp(e2);goto _TL5B;_TL5A:{struct _tuple12 _TA2;_TA2.f0=Cyc_Absyn_Int_c(0U,0);_TA2.f1=1;_TE=_TA2;}_TC=_TE;_TL5B: ans=_TC;goto _LL0;}}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T98;_T9D=_T9F->f1;_T9C=_T9F->f2;}{struct Cyc_Absyn_Exp*e1=_T9D;struct Cyc_Absyn_Exp*e2=_T9C;
# 225
struct _tuple13 _T9F=Cyc_Evexp_eval_const_bool_exp(e1);int _TA0;int _TA1;_TA1=_T9F.f0;_TA0=_T9F.f1;{int bool1=_TA1;int known1=_TA0;_TF=known1;
if(_TF)goto _TL5C;else{goto _TL5E;}
_TL5E: Cyc_Evexp_eval_const_exp(e2);{struct _tuple12 _TA2;
_TA2.f0=Cyc_Absyn_Int_c(0U,0);_TA2.f1=0;_T10=_TA2;}return _T10;_TL5C: _T12=bool1;
# 230
if(!_T12)goto _TL5F;{struct _tuple12 _TA2;_TA2.f0=Cyc_Absyn_Int_c(0U,1);_TA2.f1=1;_T13=_TA2;}_T11=_T13;goto _TL60;_TL5F: _T11=Cyc_Evexp_eval_const_exp(e2);_TL60: ans=_T11;goto _LL0;}}case 3:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T98;_T9A=_T9F->f1;_T9D=_T9F->f2;}{enum Cyc_Absyn_Primop p=_T9A;struct Cyc_List_List*es=_T9D;
# 233
if(es!=0)goto _TL61;{struct Cyc_Warn_String_Warn_Warg_struct _T9F;_T9F.tag=0;
_T9F.f1=_tag_fat("bad static expression (no args to primop)",sizeof(char),42U);_T14=_T9F;}{struct Cyc_Warn_String_Warn_Warg_struct _T9F=_T14;void*_TA0[1];_TA0[0]=& _T9F;_T15=e;_T16=_T15->loc;_T17=_tag_fat(_TA0,sizeof(void*),1);Cyc_Warn_err2(_T16,_T17);}{struct _tuple12 _T9F;
_T9F.f0=Cyc_Absyn_Int_c(0U,0);_T9F.f1=1;_T18=_T9F;}return _T18;
_TL61: _T19=es;_T1A=_T19->tl;if(_T1A!=0)goto _TL63;_T1B=p;_T1C=es;_T1D=_T1C->hd;_T1E=(struct Cyc_Absyn_Exp*)_T1D;
ans=Cyc_Evexp_eval_const_unprimop(_T1B,_T1E);goto _TL64;
_TL63: _T1F=es;_T20=_T1F->tl;_T21=_T20->tl;if(_T21!=0)goto _TL65;_T22=p;_T23=es;_T24=_T23->hd;_T25=(struct Cyc_Absyn_Exp*)_T24;_T26=es;_T27=_T26->tl;_T28=_T27->hd;_T29=(struct Cyc_Absyn_Exp*)_T28;
ans=Cyc_Evexp_eval_const_binprimop(_T22,_T25,_T29);goto _TL66;
# 241
_TL65:{struct Cyc_Warn_String_Warn_Warg_struct _T9F;_T9F.tag=0;_T9F.f1=_tag_fat("bad static expression (too many args to primop)",sizeof(char),48U);_T2A=_T9F;}{struct Cyc_Warn_String_Warn_Warg_struct _T9F=_T2A;void*_TA0[1];_TA0[0]=& _T9F;_T2B=e;_T2C=_T2B->loc;_T2D=_tag_fat(_TA0,sizeof(void*),1);Cyc_Warn_err2(_T2C,_T2D);}{struct _tuple12 _T9F;
_T9F.f0=Cyc_Absyn_Int_c(0U,0);_T9F.f1=1;_T2E=_T9F;}return _T2E;_TL66: _TL64: goto _LL0;}case 38:{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T98;_T2F=_T9F->f1;_T9D=(void*)_T2F;}{void*t=_T9D;{
# 247
void*_T9F=Cyc_Absyn_compress(t);struct Cyc_Absyn_Exp*_TA0;_T30=(int*)_T9F;_T31=*_T30;if(_T31!=9)goto _TL67;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_TA1=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T9F;_TA0=_TA1->f1;}{struct Cyc_Absyn_Exp*e2=_TA0;_T32=e;_T33=e2;
# 249
_T32->r=_T33->r;_T34=
Cyc_Evexp_eval_const_exp(e2);return _T34;}_TL67: goto _LL28;_LL28:;}goto _LL12;}case 17: _LL12: goto _LL14;case 18: _LL14: goto _LL16;case 19: _LL16:{struct _tuple12 _T9F;
# 256
_T9F.f0=Cyc_Absyn_Int_c(0U,0);_T9F.f1=0;_T35=_T9F;}ans=_T35;goto _LL0;case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T98;_T36=_T9F->f1;_T9D=(void*)_T36;_T9C=_T9F->f2;}{void*t=_T9D;struct Cyc_Absyn_Exp*e2=_T9C;
# 259
ans=Cyc_Evexp_eval_const_exp(e2);_T37=ans;_T38=_T37.f1;
if(!_T38)goto _TL69;{struct _tuple16 _T9F;
_T9F.f0=Cyc_Absyn_compress(t);_T3A=ans;_T9F.f1=_T3A.f0;_T39=_T9F;}{struct _tuple16 _T9F=_T39;void*_TA0;int _TA1;short _TA2;char _TA3;enum Cyc_Absyn_Sign _TA4;enum Cyc_Absyn_Size_of _TA5;enum Cyc_Absyn_Sign _TA6;_T3B=_T9F.f0;_T3C=(int*)_T3B;_T3D=*_T3C;if(_T3D!=0)goto _TL6B;_T3E=_T9F.f0;_T3F=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T3E;_T40=_T3F->f1;_T41=(int*)_T40;_T42=*_T41;switch(_T42){case 1: _T43=_T9F.f1;_T44=_T43.Int_c;_T45=_T44.tag;switch(_T45){case 2: _T46=_T9F.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T46;_T47=_TA7->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TA8=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T47;_TA6=_TA8->f1;_TA5=_TA8->f2;}}_T48=_T9F.f1;_T49=_T48.Char_c;_T4A=_T49.val;_TA4=_T4A.f0;_T4B=_T9F.f1;_T4C=_T4B.Char_c;_T4D=_T4C.val;_TA3=_T4D.f1;{enum Cyc_Absyn_Sign sn=_TA6;enum Cyc_Absyn_Size_of sz=_TA5;enum Cyc_Absyn_Sign sn2=_TA4;char x=_TA3;_TA6=sn;_TA5=sz;_TA4=sn2;_T4E=x;_TA2=(short)_T4E;goto _LL31;}case 4: _T4F=_T9F.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4F;_T50=_TA7->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TA8=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T50;_TA6=_TA8->f1;_TA5=_TA8->f2;}}_T51=_T9F.f1;_T52=_T51.Short_c;_T53=_T52.val;_TA4=_T53.f0;_T54=_T9F.f1;_T55=_T54.Short_c;_T56=_T55.val;_TA2=_T56.f1;_LL31: {enum Cyc_Absyn_Sign sn=_TA6;enum Cyc_Absyn_Size_of sz=_TA5;enum Cyc_Absyn_Sign sn2=_TA4;short x=_TA2;_TA6=sn;_TA5=sz;_TA4=sn2;_T57=x;_TA1=(int)_T57;goto _LL33;}case 5: _T58=_T9F.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T58;_T59=_TA7->f1;{struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TA8=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T59;_TA6=_TA8->f1;_TA5=_TA8->f2;}}_T5A=_T9F.f1;_T5B=_T5A.Int_c;_T5C=_T5B.val;_TA4=_T5C.f0;_T5D=_T9F.f1;_T5E=_T5D.Int_c;_T5F=_T5E.val;_TA1=_T5F.f1;_LL33: {enum Cyc_Absyn_Sign sn=_TA6;enum Cyc_Absyn_Size_of sz=_TA5;enum Cyc_Absyn_Sign sn2=_TA4;int x=_TA1;_T60=sn;_T61=(int)_T60;_T62=sn2;_T63=(int)_T62;
# 268
if(_T61==_T63)goto _TL6F;
ans.f0=Cyc_Absyn_Int_c(sn,x);goto _TL70;_TL6F: _TL70: goto _LL2D;}default: goto _LL3A;};case 5: _T64=_T9F.f0;_T65=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T64;_T66=_T65->f2;if(_T66==0)goto _TL71;_T67=_T9F.f1;_T68=_T67.Int_c;_T69=_T68.tag;switch(_T69){case 2: _T6A=_T9F.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T6A;_T6B=_TA7->f2;{struct Cyc_List_List _TA8=*_T6B;_T6C=_TA8.hd;_TA0=(void*)_T6C;}}_T6D=_T9F.f1;_T6E=_T6D.Char_c;_T6F=_T6E.val;_TA6=_T6F.f0;_T70=_T9F.f1;_T71=_T70.Char_c;_T72=_T71.val;_TA3=_T72.f1;{void*it=_TA0;enum Cyc_Absyn_Sign sn2=_TA6;char x=_TA3;_TA0=it;_TA6=sn2;_T73=x;_TA2=(short)_T73;goto _LL37;}case 4: _T74=_T9F.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T74;_T75=_TA7->f2;{struct Cyc_List_List _TA8=*_T75;_T76=_TA8.hd;_TA0=(void*)_T76;}}_T77=_T9F.f1;_T78=_T77.Short_c;_T79=_T78.val;_TA6=_T79.f0;_T7A=_T9F.f1;_T7B=_T7A.Short_c;_T7C=_T7B.val;_TA2=_T7C.f1;_LL37: {void*it=_TA0;enum Cyc_Absyn_Sign sn2=_TA6;short x=_TA2;_TA0=it;_TA6=sn2;_T7D=x;_TA1=(int)_T7D;goto _LL39;}case 5: _T7E=_T9F.f0;{struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA7=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T7E;_T7F=_TA7->f2;{struct Cyc_List_List _TA8=*_T7F;_T80=_TA8.hd;_TA0=(void*)_T80;}}_T81=_T9F.f1;_T82=_T81.Int_c;_T83=_T82.val;_TA6=_T83.f0;_T84=_T9F.f1;_T85=_T84.Int_c;_T86=_T85.val;_TA1=_T86.f1;_LL39: {void*it=_TA0;enum Cyc_Absyn_Sign sn2=_TA6;int x=_TA1;
# 277
if(x >= 0)goto _TL74;{struct _tuple12 _TA7;
_TA7.f0=Cyc_Absyn_Int_c(0U,0);_TA7.f1=0;_T87=_TA7;}return _T87;_TL74: _T88=it;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_TA7=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_TA7->tag=9;_T8A=ans;_T8B=_T8A.f0;
_TA7->f1=Cyc_Absyn_const_exp(_T8B,0U);_T89=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_TA7;}_T8C=(void*)_T89;Cyc_Unify_unify(_T88,_T8C);
ans.f0=Cyc_Absyn_Int_c(1U,x);goto _LL2D;}default: goto _LL3A;}goto _TL72;_TL71: goto _LL3A;_TL72:;default: goto _LL3A;}goto _TL6C;_TL6B: _LL3A:{struct _tuple12 _TA7;
# 282
_TA7.f0=Cyc_Absyn_Int_c(0U,0);_TA7.f1=0;_T8D=_TA7;}return _T8D;_TL6C: _LL2D:;}goto _TL6A;_TL69: _TL6A: goto _LL0;}case 32:{struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_T98;_T99=_T9F->f2;}{struct Cyc_Absyn_Enumfield*ef=_T99;_T99=ef;goto _LL1C;}case 31:{struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_T9F=(struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_T98;_T99=_T9F->f2;}_LL1C: {struct Cyc_Absyn_Enumfield*ef=_T99;_T8E=ef;_T8F=_T8E->tag;_T90=
# 286
_check_null(_T8F);ans=Cyc_Evexp_eval_const_exp(_T90);goto _LL0;}default:{struct _tuple12 _T9F;
_T9F.f0=Cyc_Absyn_Int_c(0U,0);_T9F.f1=0;_T91=_T9F;}return _T91;}_LL0:;}_T92=ans;_T93=_T92.f1;
# 289
if(!_T93)goto _TL76;{
void*c;_T94=ans;{
union Cyc_Absyn_Cnst c=_T94.f0;_T95=e;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T98=_cycalloc(sizeof(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct));_T98->tag=0;
_T98->f1=c;_T96=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_T98;}_T95->r=(void*)_T96;}}goto _TL77;_TL76: _TL77: _T97=ans;
# 294
return _T97;}
# 299
int Cyc_Evexp_c_can_eval(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;unsigned _T2;int _T3;int _T4;int _T5;int _T6;int _T7;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T8;enum Cyc_Absyn_Primop _T9;struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;void*_TD;struct Cyc_Absyn_Exp*_TE;int _TF;struct Cyc_List_List*_T10;void*_T11;struct Cyc_Absyn_Exp*_T12;int _T13;struct Cyc_List_List*_T14;int _T15;_T0=e;{
void*_T16=_T0->r;struct Cyc_List_List*_T17;enum Cyc_Absyn_Primop _T18;struct Cyc_Absyn_Exp*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Exp*_T1B;_T1=(int*)_T16;_T2=*_T1;switch(_T2){case 32: goto _LL4;case 31: _LL4: goto _LL6;case 17: _LL6: goto _LL8;case 18: _LL8: goto _LLA;case 19: _LLA: goto _LLC;case 0: _LLC:
# 306
 return 1;case 6:{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T16;_T1B=_T1C->f1;_T1A=_T1C->f2;_T19=_T1C->f3;}{struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T1A;struct Cyc_Absyn_Exp*e3=_T19;_T4=
# 308
Cyc_Evexp_c_can_eval(e1);if(!_T4)goto _TL79;_T5=Cyc_Evexp_c_can_eval(e2);if(!_T5)goto _TL79;_T3=Cyc_Evexp_c_can_eval(e3);goto _TL7A;_TL79: _T3=0;_TL7A: return _T3;}case 7:{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T16;_T1B=_T1C->f1;_T1A=_T1C->f2;}{struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T1A;_T1B=e1;_T1A=e2;goto _LL12;}case 8:{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T16;_T1B=_T1C->f1;_T1A=_T1C->f2;}_LL12: {struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T1A;_T7=
# 310
Cyc_Evexp_c_can_eval(e1);if(!_T7)goto _TL7B;_T6=Cyc_Evexp_c_can_eval(e2);goto _TL7C;_TL7B: _T6=0;_TL7C: return _T6;}case 3: _T8=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T16;_T9=_T8->f1;if(_T9!=Cyc_Absyn_Numelts)goto _TL7D;_TA=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T16;_TB=_TA->f2;if(_TB==0)goto _TL7F;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T16;_TC=_T1C->f2;{struct Cyc_List_List _T1D=*_TC;_TD=_T1D.hd;_T1B=(struct Cyc_Absyn_Exp*)_TD;}}{struct Cyc_Absyn_Exp*e1=_T1B;_TE=e1;{
# 312
void*topt=_TE->topt;
if(topt==0)goto _TL81;{
struct Cyc_Absyn_Exp*eopt=Cyc_Tcutil_get_type_bound(topt);
if(eopt==0)goto _TL83;_TF=Cyc_Evexp_c_can_eval(eopt);return _TF;_TL83:;}goto _TL82;_TL81: _TL82:
# 317
 return 0;}}_TL7F: goto _LL15;_TL7D: _LL15:{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T16;_T18=_T1C->f1;_T17=_T1C->f2;}{enum Cyc_Absyn_Primop p=_T18;struct Cyc_List_List*es=_T17;
# 319
_TL88: if(es!=0)goto _TL86;else{goto _TL87;}
_TL86: _T10=es;_T11=_T10->hd;_T12=(struct Cyc_Absyn_Exp*)_T11;_T13=Cyc_Evexp_c_can_eval(_T12);if(_T13)goto _TL89;else{goto _TL8B;}
_TL8B: return 0;_TL89: _T14=es;
# 319
es=_T14->tl;goto _TL88;_TL87:
# 322
 return 1;}case 38:
 return 0;case 14:{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T1C=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T16;_T1B=_T1C->f2;}{struct Cyc_Absyn_Exp*e=_T1B;_T15=
Cyc_Evexp_c_can_eval(e);return _T15;}default:
 return 0;};}}
# 330
static struct Cyc_Absyn_Exp*Cyc_Evexp_strip_cast(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;int*_T1;int _T2;void*_T3;int _T4;struct Cyc_Absyn_Exp*_T5;int*_T6;int _T7;struct Cyc_Absyn_Exp*_T8;struct Cyc_Absyn_Exp*_T9;struct Cyc_Absyn_Exp*_TA;_T0=e;{
void*_TB=_T0->r;struct Cyc_Absyn_Exp*_TC;void*_TD;_T1=(int*)_TB;_T2=*_T1;if(_T2!=14)goto _TL8C;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_TE=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_TB;_T3=_TE->f1;_TD=(void*)_T3;_TC=_TE->f2;}{void*t=_TD;struct Cyc_Absyn_Exp*e2=_TC;_T4=
# 333
Cyc_Unify_unify(t,Cyc_Absyn_uint_type);if(!_T4)goto _TL8E;_T5=e2;{
void*_TE=_T5->r;_T6=(int*)_TE;_T7=*_T6;if(_T7!=38)goto _TL90;_T8=e2;
return _T8;_TL90: goto _LL5;_LL5:;}goto _TL8F;_TL8E: _TL8F: _T9=e;
# 338
return _T9;}_TL8C: _TA=e;
return _TA;;}}
# 343
static unsigned long long Cyc_Evexp_integral_const_to_long_long(union Cyc_Absyn_Cnst c){union Cyc_Absyn_Cnst _T0;struct _union_Cnst_LongLong_c _T1;unsigned _T2;union Cyc_Absyn_Cnst _T3;struct _union_Cnst_Char_c _T4;struct _tuple3 _T5;char _T6;union Cyc_Absyn_Cnst _T7;struct _union_Cnst_Short_c _T8;struct _tuple4 _T9;short _TA;union Cyc_Absyn_Cnst _TB;struct _union_Cnst_Int_c _TC;struct _tuple5 _TD;int _TE;union Cyc_Absyn_Cnst _TF;struct _union_Cnst_LongLong_c _T10;struct _tuple6 _T11;long long _T12;struct Cyc_Warn_String_Warn_Warg_struct _T13;int(*_T14)(struct _fat_ptr);void*(*_T15)(struct _fat_ptr);struct _fat_ptr _T16;unsigned long long _T17;
unsigned long long res;{long long _T18;int _T19;short _T1A;char _T1B;_T0=c;_T1=_T0.LongLong_c;_T2=_T1.tag;switch(_T2){case 2: _T3=c;_T4=_T3.Char_c;_T5=_T4.val;_T1B=_T5.f1;{char c=_T1B;_T6=c;
# 346
res=(unsigned long long)_T6;goto _LL0;}case 4: _T7=c;_T8=_T7.Short_c;_T9=_T8.val;_T1A=_T9.f1;{short s=_T1A;_TA=s;
res=(unsigned long long)_TA;goto _LL0;}case 5: _TB=c;_TC=_TB.Int_c;_TD=_TC.val;_T19=_TD.f1;{int i=_T19;_TE=i;
res=(unsigned long long)_TE;goto _LL0;}case 6: _TF=c;_T10=_TF.LongLong_c;_T11=_T10.val;_T18=_T11.f1;{long long x=_T18;_T12=x;
res=(unsigned long long)_T12;goto _LL0;}default:{struct Cyc_Warn_String_Warn_Warg_struct _T1C;_T1C.tag=0;
_T1C.f1=_tag_fat("Evexp::integral_const_to_long_long",sizeof(char),35U);_T13=_T1C;}{struct Cyc_Warn_String_Warn_Warg_struct _T1C=_T13;void*_T1D[1];_T1D[0]=& _T1C;_T15=Cyc_Warn_impos2;{int(*_T1E)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T15;_T14=_T1E;}_T16=_tag_fat(_T1D,sizeof(void*),1);_T14(_T16);}}_LL0:;}_T17=res;
# 352
return _T17;}struct _tuple17{union Cyc_Absyn_Cnst f0;union Cyc_Absyn_Cnst f1;};
# 355
static int Cyc_Evexp_cmp_cnst(union Cyc_Absyn_Cnst c1,union Cyc_Absyn_Cnst c2){struct _tuple17 _T0;union Cyc_Absyn_Cnst _T1;struct _union_Cnst_Null_c _T2;unsigned _T3;union Cyc_Absyn_Cnst _T4;struct _union_Cnst_Null_c _T5;unsigned _T6;int _T7;union Cyc_Absyn_Cnst _T8;struct _union_Cnst_Null_c _T9;unsigned _TA;union Cyc_Absyn_Cnst _TB;struct _union_Cnst_Wchar_c _TC;unsigned _TD;union Cyc_Absyn_Cnst _TE;struct _union_Cnst_Wchar_c _TF;unsigned _T10;union Cyc_Absyn_Cnst _T11;struct _union_Cnst_Wchar_c _T12;union Cyc_Absyn_Cnst _T13;struct _union_Cnst_Wchar_c _T14;int _T15;int _T16;union Cyc_Absyn_Cnst _T17;struct _union_Cnst_Wchar_c _T18;unsigned _T19;union Cyc_Absyn_Cnst _T1A;struct _union_Cnst_String_c _T1B;unsigned _T1C;union Cyc_Absyn_Cnst _T1D;struct _union_Cnst_String_c _T1E;unsigned _T1F;union Cyc_Absyn_Cnst _T20;struct _union_Cnst_String_c _T21;union Cyc_Absyn_Cnst _T22;struct _union_Cnst_String_c _T23;int _T24;int _T25;union Cyc_Absyn_Cnst _T26;struct _union_Cnst_String_c _T27;unsigned _T28;union Cyc_Absyn_Cnst _T29;struct _union_Cnst_Wstring_c _T2A;unsigned _T2B;union Cyc_Absyn_Cnst _T2C;struct _union_Cnst_Wstring_c _T2D;unsigned _T2E;union Cyc_Absyn_Cnst _T2F;struct _union_Cnst_Wstring_c _T30;union Cyc_Absyn_Cnst _T31;struct _union_Cnst_Wstring_c _T32;int _T33;union Cyc_Absyn_Cnst _T34;struct _union_Cnst_Wstring_c _T35;int _T36;union Cyc_Absyn_Cnst _T37;struct _union_Cnst_Wstring_c _T38;unsigned _T39;union Cyc_Absyn_Cnst _T3A;struct _union_Cnst_Wstring_c _T3B;union Cyc_Absyn_Cnst _T3C;struct _union_Cnst_Float_c _T3D;unsigned _T3E;union Cyc_Absyn_Cnst _T3F;struct _union_Cnst_Float_c _T40;unsigned _T41;union Cyc_Absyn_Cnst _T42;struct _union_Cnst_Float_c _T43;struct _tuple7 _T44;union Cyc_Absyn_Cnst _T45;struct _union_Cnst_Float_c _T46;struct _tuple7 _T47;union Cyc_Absyn_Cnst _T48;struct _union_Cnst_Float_c _T49;struct _tuple7 _T4A;union Cyc_Absyn_Cnst _T4B;struct _union_Cnst_Float_c _T4C;struct _tuple7 _T4D;int _T4E;int _T4F;int _T50;union Cyc_Absyn_Cnst _T51;struct _union_Cnst_Float_c _T52;unsigned _T53;unsigned long long _T54;int _T55;int _T56;{struct _tuple17 _T57;
_T57.f0=c1;_T57.f1=c2;_T0=_T57;}{struct _tuple17 _T57=_T0;int _T58;int _T59;struct _fat_ptr _T5A;struct _fat_ptr _T5B;_T1=_T57.f0;_T2=_T1.Null_c;_T3=_T2.tag;if(_T3!=1)goto _TL93;_T4=_T57.f1;_T5=_T4.Null_c;_T6=_T5.tag;if(_T6!=1)goto _TL95;
return 0;_TL95: _T7=- 1;
return _T7;_TL93: _T8=_T57.f1;_T9=_T8.Null_c;_TA=_T9.tag;if(_TA!=1)goto _TL97;
return 1;_TL97: _TB=_T57.f0;_TC=_TB.Wchar_c;_TD=_TC.tag;if(_TD!=3)goto _TL99;_TE=_T57.f1;_TF=_TE.Wchar_c;_T10=_TF.tag;if(_T10!=3)goto _TL9B;_T11=_T57.f0;_T12=_T11.Wchar_c;_T5B=_T12.val;_T13=_T57.f1;_T14=_T13.Wchar_c;_T5A=_T14.val;{struct _fat_ptr s1=_T5B;struct _fat_ptr s2=_T5A;_T15=
Cyc_strcmp(s1,s2);return _T15;}_TL9B: _T16=- 1;
return _T16;_TL99: _T17=_T57.f1;_T18=_T17.Wchar_c;_T19=_T18.tag;if(_T19!=3)goto _TL9D;
return 1;_TL9D: _T1A=_T57.f0;_T1B=_T1A.String_c;_T1C=_T1B.tag;if(_T1C!=8)goto _TL9F;_T1D=_T57.f1;_T1E=_T1D.String_c;_T1F=_T1E.tag;if(_T1F!=8)goto _TLA1;_T20=_T57.f0;_T21=_T20.String_c;_T5B=_T21.val;_T22=_T57.f1;_T23=_T22.String_c;_T5A=_T23.val;{struct _fat_ptr s1=_T5B;struct _fat_ptr s2=_T5A;_T24=
Cyc_strcmp(s1,s2);return _T24;}_TLA1: _T25=- 1;
return _T25;_TL9F: _T26=_T57.f1;_T27=_T26.String_c;_T28=_T27.tag;if(_T28!=8)goto _TLA3;
return 1;_TLA3: _T29=_T57.f0;_T2A=_T29.Wstring_c;_T2B=_T2A.tag;if(_T2B!=9)goto _TLA5;_T2C=_T57.f1;_T2D=_T2C.Wstring_c;_T2E=_T2D.tag;if(_T2E!=9)goto _TLA7;_T2F=_T57.f0;_T30=_T2F.Wstring_c;_T5B=_T30.val;_T31=_T57.f1;_T32=_T31.Wstring_c;_T5A=_T32.val;{struct _fat_ptr s1=_T5B;struct _fat_ptr s2=_T5A;_T33=
Cyc_strcmp(s1,s2);return _T33;}_TLA7: _T34=_T57.f0;_T35=_T34.Wstring_c;_T5B=_T35.val;{struct _fat_ptr s1=_T5B;_T36=- 1;
return _T36;}_TLA5: _T37=_T57.f1;_T38=_T37.Wstring_c;_T39=_T38.tag;if(_T39!=9)goto _TLA9;_T3A=_T57.f1;_T3B=_T3A.Wstring_c;_T5B=_T3B.val;{struct _fat_ptr s1=_T5B;
return 1;}_TLA9: _T3C=_T57.f0;_T3D=_T3C.Float_c;_T3E=_T3D.tag;if(_T3E!=7)goto _TLAB;_T3F=_T57.f1;_T40=_T3F.Float_c;_T41=_T40.tag;if(_T41!=7)goto _TLAD;_T42=_T57.f0;_T43=_T42.Float_c;_T44=_T43.val;_T5B=_T44.f0;_T45=_T57.f0;_T46=_T45.Float_c;_T47=_T46.val;_T59=_T47.f1;_T48=_T57.f1;_T49=_T48.Float_c;_T4A=_T49.val;_T5A=_T4A.f0;_T4B=_T57.f1;_T4C=_T4B.Float_c;_T4D=_T4C.val;_T58=_T4D.f1;{struct _fat_ptr s1=_T5B;int i1=_T59;struct _fat_ptr s2=_T5A;int i2=_T58;
# 370
if(i1==i2)goto _TLAF;_T4E=i1 - i2;return _T4E;_TLAF: _T4F=
Cyc_strcmp(s1,s2);return _T4F;}_TLAD: _T50=- 1;
return _T50;_TLAB: _T51=_T57.f1;_T52=_T51.Float_c;_T53=_T52.tag;if(_T53!=7)goto _TLB1;
return 1;_TLB1:{
# 375
unsigned long long i1=Cyc_Evexp_integral_const_to_long_long(c1);
unsigned long long i2=Cyc_Evexp_integral_const_to_long_long(c2);_T54=i1 - i2;{
long long diff=(long long)_T54;
if(diff!=0)goto _TLB3;_T55=0;goto _TLB4;_TLB3: if(diff >= 0)goto _TLB5;_T56=- 1;goto _TLB6;_TLB5: _T56=1;_TLB6: _T55=_T56;_TLB4: return _T55;}};}}struct _tuple18{void*f0;void*f1;};
# 388
static int Cyc_Evexp_designator_cmp(void*d1,void*d2){struct _tuple18 _T0;void*_T1;int*_T2;int _T3;void*_T4;int*_T5;int _T6;void*_T7;void*_T8;int _T9;void*_TA;int _TB;void*_TC;int*_TD;int _TE;void*_TF;void*_T10;void*_T11;struct _fat_ptr*_T12;struct _fat_ptr _T13;struct _fat_ptr*_T14;struct _fat_ptr _T15;int _T16;{struct _tuple18 _T17;
_T17.f0=d1;_T17.f1=d2;_T0=_T17;}{struct _tuple18 _T17=_T0;struct _fat_ptr*_T18;struct _fat_ptr*_T19;struct Cyc_Absyn_Exp*_T1A;struct Cyc_Absyn_Exp*_T1B;_T1=_T17.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TLB7;_T4=_T17.f1;_T5=(int*)_T4;_T6=*_T5;if(_T6!=0)goto _TLB9;_T7=_T17.f0;{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T1C=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_T7;_T1B=_T1C->f1;}_T8=_T17.f1;{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T1C=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_T8;_T1A=_T1C->f1;}{struct Cyc_Absyn_Exp*e1=_T1B;struct Cyc_Absyn_Exp*e2=_T1A;_T9=
Cyc_Evexp_exp_cmp(e1,e2);return _T9;}_TLB9: _TA=_T17.f0;{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T1C=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_TA;_T1B=_T1C->f1;}{struct Cyc_Absyn_Exp*e1=_T1B;_TB=- 1;
return _TB;}_TLB7: _TC=_T17.f1;_TD=(int*)_TC;_TE=*_TD;if(_TE!=0)goto _TLBB;_TF=_T17.f1;{struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_T1C=(struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_TF;_T1B=_T1C->f1;}{struct Cyc_Absyn_Exp*e1=_T1B;
return 1;}_TLBB: _T10=_T17.f0;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T10;_T19=_T1C->f1;}_T11=_T17.f1;{struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_T1C=(struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_T11;_T18=_T1C->f1;}{struct _fat_ptr*v1=_T19;struct _fat_ptr*v2=_T18;_T12=v1;_T13=*_T12;_T14=v2;_T15=*_T14;_T16=
Cyc_strcmp(_T13,_T15);return _T16;};}}struct _tuple19{struct Cyc_List_List*f0;struct Cyc_Absyn_Exp*f1;};
# 397
static int Cyc_Evexp_designator_exp_cmp(struct _tuple19*d1,struct _tuple19*d2){struct _tuple19*_T0;struct _tuple19*_T1;int _T2;struct Cyc_List_List*_T3;struct Cyc_List_List*_T4;int _T5;_T0=d1;{
# 399
struct _tuple19 _T6=*_T0;struct Cyc_Absyn_Exp*_T7;struct Cyc_List_List*_T8;_T8=_T6.f0;_T7=_T6.f1;{struct Cyc_List_List*des1=_T8;struct Cyc_Absyn_Exp*e1=_T7;_T1=d2;{
struct _tuple19 _T9=*_T1;struct Cyc_Absyn_Exp*_TA;struct Cyc_List_List*_TB;_TB=_T9.f0;_TA=_T9.f1;{struct Cyc_List_List*des2=_TB;struct Cyc_Absyn_Exp*e2=_TA;
int c1=Cyc_Evexp_exp_cmp(e1,e2);
if(c1==0)goto _TLBD;_T2=c1;return _T2;_TLBD: _T3=des1;_T4=des2;_T5=
Cyc_List_list_cmp(Cyc_Evexp_designator_cmp,_T3,_T4);return _T5;}}}}}
# 406
static struct Cyc_Absyn_Vardecl*Cyc_Evexp_binding2vardecl(void*b){void*_T0;int*_T1;unsigned _T2;void*_T3;struct Cyc_Absyn_Vardecl*_T4;void*_T5;struct Cyc_Absyn_Fndecl*_T6;struct Cyc_Absyn_Vardecl*_T7;void*_T8;struct Cyc_Absyn_Vardecl*_T9;void*_TA;struct Cyc_Absyn_Vardecl*_TB;void*_TC;struct Cyc_Absyn_Vardecl*_TD;struct Cyc_Absyn_Fndecl*_TE;struct Cyc_Absyn_Vardecl*_TF;_T0=b;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0:
# 408
 return 0;case 1: _T3=b;{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T10=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_T3;_TF=_T10->f1;}{struct Cyc_Absyn_Vardecl*vd=_TF;_T4=vd;
return _T4;}case 2: _T5=b;{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T10=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T5;_TE=_T10->f1;}{struct Cyc_Absyn_Fndecl*fd=_TE;_T6=fd;_T7=_T6->fn_vardecl;
return _T7;}case 3: _T8=b;{struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_T10=(struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_T8;_TF=_T10->f1;}{struct Cyc_Absyn_Vardecl*vd=_TF;_T9=vd;
return _T9;}case 4: _TA=b;{struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_T10=(struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_TA;_TF=_T10->f1;}{struct Cyc_Absyn_Vardecl*vd=_TF;_TB=vd;
return _TB;}default: _TC=b;{struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*_T10=(struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_TC;_TF=_T10->f1;}{struct Cyc_Absyn_Vardecl*vd=_TF;_TD=vd;
return _TD;}};}struct _tuple20{struct Cyc_Absyn_Vardecl*f0;struct Cyc_Absyn_Vardecl*f1;};
# 417
static int Cyc_Evexp_cmp_binding(void*b1,void*b2){struct _tuple20 _T0;struct Cyc_Absyn_Vardecl*_T1;struct Cyc_Absyn_Vardecl*_T2;struct _tuple0*_T3;struct _tuple0*_T4;int _T5;int _T6;struct Cyc_Absyn_Vardecl*_T7;struct Cyc_Absyn_Vardecl*_T8;int _T9;struct Cyc_Absyn_Vardecl*_TA;int _TB;int _TC;
struct Cyc_Absyn_Vardecl*vdopt1=Cyc_Evexp_binding2vardecl(b1);
struct Cyc_Absyn_Vardecl*vdopt2=Cyc_Evexp_binding2vardecl(b2);{struct _tuple20 _TD;
_TD.f0=vdopt1;_TD.f1=vdopt2;_T0=_TD;}{struct _tuple20 _TD=_T0;_T1=_TD.f0;if(_T1!=0)goto _TLC0;_T2=_TD.f1;if(_T2!=0)goto _TLC2;_T3=
Cyc_Absyn_binding2qvar(b1);_T4=Cyc_Absyn_binding2qvar(b2);_T5=Cyc_Absyn_qvar_cmp(_T3,_T4);return _T5;_TLC2: _T6=- 1;
return _T6;_TLC0: _T7=_TD.f1;if(_T7!=0)goto _TLC4;
return 1;_TLC4: _T8=vdopt1;_T9=(int)_T8;_TA=vdopt2;_TB=(int)_TA;_TC=_T9 - _TB;
return _TC;;}}
# 430
inline static unsigned Cyc_Evexp_exp_case_number(struct Cyc_Absyn_Exp*e){struct Cyc_Absyn_Exp*_T0;void*_T1;const unsigned*_T2;unsigned _T3;_T0=e;_T1=_T0->r;_T2=(const unsigned*)_T1;_T3=*_T2;
return _T3;}
# 434
static int Cyc_Evexp_unify_and_cmp(void*t1,void*t2){int _T0;int _T1;_T0=
Cyc_Unify_unify(t1,t2);if(!_T0)goto _TLC6;return 0;_TLC6: _T1=
Cyc_Tcutil_typecmp(t1,t2);return _T1;}
# 441
int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int _T0;int _T1;int _T2;unsigned _T3;int _T4;struct _tuple18 _T5;struct Cyc_Absyn_Exp*_T6;struct Cyc_Absyn_Exp*_T7;void*_T8;int*_T9;unsigned _TA;void*_TB;int*_TC;int _TD;void*_TE;void*_TF;int _T10;void*_T11;int*_T12;int _T13;void*_T14;void*_T15;void*_T16;void*_T17;int _T18;void*_T19;int*_T1A;int _T1B;void*_T1C;void*_T1D;int _T1E;void*_T1F;int*_T20;int _T21;void*_T22;void*_T23;int _T24;int _T25;int _T26;void*_T27;int*_T28;int _T29;void*_T2A;void*_T2B;struct Cyc_Absyn_Vardecl*_T2C;int _T2D;struct Cyc_Absyn_Vardecl*_T2E;int _T2F;int _T30;void*_T31;int*_T32;int _T33;void*_T34;void*_T35;void*_T36;int*_T37;int _T38;void*_T39;void*_T3A;void*_T3B;int*_T3C;int _T3D;void*_T3E;void*_T3F;int _T40;int _T41;void*_T42;int*_T43;int _T44;void*_T45;void*_T46;enum Cyc_Absyn_Primop _T47;int _T48;enum Cyc_Absyn_Primop _T49;int _T4A;int _T4B;struct Cyc_List_List*_T4C;void*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_List_List*_T4F;void*_T50;struct Cyc_Absyn_Exp*_T51;int _T52;struct Cyc_List_List*_T53;struct Cyc_List_List*_T54;void*_T55;int*_T56;unsigned _T57;void*_T58;void*_T59;void*_T5A;void*_T5B;int _T5C;void*_T5D;void*_T5E;void*_T5F;struct Cyc_Absyn_Exp*_T60;struct Cyc_Warn_String_Warn_Warg_struct _T61;struct Cyc_Absyn_Exp*_T62;unsigned _T63;struct _fat_ptr _T64;int _T65;void*_T66;int*_T67;unsigned _T68;void*_T69;void*_T6A;void*_T6B;struct Cyc_Absyn_Exp*_T6C;struct Cyc_Warn_String_Warn_Warg_struct _T6D;struct Cyc_Absyn_Exp*_T6E;unsigned _T6F;struct _fat_ptr _T70;int _T71;void*_T72;void*_T73;struct Cyc_Absyn_Exp*_T74;struct Cyc_Absyn_Exp*_T75;struct Cyc_Warn_String_Warn_Warg_struct _T76;struct Cyc_Absyn_Exp*_T77;unsigned _T78;struct _fat_ptr _T79;struct Cyc_Warn_String_Warn_Warg_struct _T7A;struct Cyc_Absyn_Exp*_T7B;unsigned _T7C;struct _fat_ptr _T7D;int _T7E;void*_T7F;int*_T80;int _T81;void*_T82;void*_T83;void*_T84;void*_T85;int _T86;int _T87;struct _tuple18 _T88;struct Cyc_List_List*_T89;struct Cyc_List_List*_T8A;void*_T8B;void*_T8C;int _T8D;struct Cyc_List_List*_T8E;struct Cyc_List_List*_T8F;void*_T90;int*_T91;int _T92;void*_T93;void*_T94;void*_T95;void*_T96;int _T97;int _T98;void*_T99;int*_T9A;int _T9B;void*_T9C;void*_T9D;void*_T9E;void*_T9F;int _TA0;void*_TA1;int*_TA2;int _TA3;void*_TA4;void*_TA5;void*_TA6;int*_TA7;int _TA8;void*_TA9;void*_TAA;int _TAB;void*_TAC;int*_TAD;int _TAE;void*_TAF;void*_TB0;int _TB1;struct Cyc_List_List*_TB2;struct Cyc_List_List*_TB3;int _TB4;void*_TB5;int*_TB6;int _TB7;void*_TB8;void*_TB9;int _TBA;void*_TBB;int*_TBC;int _TBD;void*_TBE;void*_TBF;void*_TC0;void*_TC1;int _TC2;int _TC3;int _TC4;void*_TC5;int*_TC6;int _TC7;void*_TC8;void*_TC9;void*_TCA;void*_TCB;int _TCC;void*_TCD;int*_TCE;int _TCF;void*_TD0;void*_TD1;int(*_TD2)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*);int(*_TD3)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*_TD4;struct Cyc_List_List*_TD5;int _TD6;void*_TD7;int*_TD8;int _TD9;void*_TDA;void*_TDB;struct Cyc_Absyn_Aggrdecl*_TDC;int _TDD;struct Cyc_Absyn_Aggrdecl*_TDE;int _TDF;int _TE0;struct Cyc_List_List*_TE1;struct Cyc_List_List*_TE2;int _TE3;int(*_TE4)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*);int(*_TE5)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*_TE6;struct Cyc_List_List*_TE7;int _TE8;void*_TE9;int*_TEA;int _TEB;void*_TEC;void*_TED;struct Cyc_Absyn_Datatypedecl*_TEE;int _TEF;struct Cyc_Absyn_Datatypedecl*_TF0;int _TF1;int _TF2;struct Cyc_Absyn_Datatypefield*_TF3;int _TF4;struct Cyc_Absyn_Datatypefield*_TF5;int _TF6;int _TF7;int(*_TF8)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*);int(*_TF9)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct Cyc_List_List*_TFA;struct Cyc_List_List*_TFB;int _TFC;void*_TFD;int*_TFE;int _TFF;void*_T100;int*_T101;int _T102;struct Cyc_Absyn_Exp*_T103;int _T104;struct Cyc_Absyn_Exp*_T105;int _T106;int _T107;struct Cyc_Warn_String_Warn_Warg_struct _T108;struct Cyc_Warn_Int_Warn_Warg_struct _T109;unsigned _T10A;struct Cyc_Warn_String_Warn_Warg_struct _T10B;struct Cyc_Warn_Exp_Warn_Warg_struct _T10C;struct Cyc_Warn_String_Warn_Warg_struct _T10D;struct Cyc_Warn_Int_Warn_Warg_struct _T10E;unsigned _T10F;struct Cyc_Warn_String_Warn_Warg_struct _T110;struct Cyc_Warn_Exp_Warn_Warg_struct _T111;struct Cyc_Warn_String_Warn_Warg_struct _T112;int(*_T113)(struct _fat_ptr);void*(*_T114)(struct _fat_ptr);struct _fat_ptr _T115;
struct _tuple12 _T116=Cyc_Evexp_eval_const_exp(e1);int _T117;union Cyc_Absyn_Cnst _T118;_T118=_T116.f0;_T117=_T116.f1;{union Cyc_Absyn_Cnst c1=_T118;int known1=_T117;
struct _tuple12 _T119=Cyc_Evexp_eval_const_exp(e2);int _T11A;union Cyc_Absyn_Cnst _T11B;_T11B=_T119.f0;_T11A=_T119.f1;{union Cyc_Absyn_Cnst c2=_T11B;int known2=_T11A;_T0=known1;
if(!_T0)goto _TLC8;_T1=known2;if(!_T1)goto _TLC8;_T2=Cyc_Evexp_cmp_cnst(c1,c2);return _T2;_TLC8: {
unsigned e1case=Cyc_Evexp_exp_case_number(e1);
unsigned e2case=Cyc_Evexp_exp_case_number(e2);
if(e1case==e2case)goto _TLCA;_T3=e1case - e2case;_T4=(int)_T3;
return _T4;_TLCA:{struct _tuple18 _T11C;_T6=e1;
_T11C.f0=_T6->r;_T7=e2;_T11C.f1=_T7->r;_T5=_T11C;}{struct _tuple18 _T11C=_T5;struct Cyc_Absyn_Datatypefield*_T11D;struct Cyc_Absyn_Datatypedecl*_T11E;struct Cyc_Absyn_Datatypefield*_T11F;struct Cyc_Absyn_Datatypedecl*_T120;struct Cyc_Absyn_Aggrdecl*_T121;struct Cyc_List_List*_T122;struct Cyc_List_List*_T123;struct Cyc_Absyn_Aggrdecl*_T124;struct Cyc_List_List*_T125;int _T126;int _T127;struct Cyc_List_List*_T128;enum Cyc_Absyn_Primop _T129;enum Cyc_Absyn_Primop _T12A;struct Cyc_Absyn_Vardecl*_T12B;struct Cyc_Absyn_Exp*_T12C;struct Cyc_Absyn_Exp*_T12D;struct Cyc_Absyn_Exp*_T12E;void*_T12F;struct _fat_ptr _T130;struct _fat_ptr _T131;void*_T132;void*_T133;union Cyc_Absyn_Cnst _T134;union Cyc_Absyn_Cnst _T135;_T8=_T11C.f0;_T9=(int*)_T8;_TA=*_T9;switch(_TA){case 0: _TB=_T11C.f1;_TC=(int*)_TB;_TD=*_TC;if(_TD!=0)goto _TLCD;_TE=_T11C.f0;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TE;_T135=_T136->f1;}_TF=_T11C.f1;{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TF;_T134=_T136->f1;}{union Cyc_Absyn_Cnst c1=_T135;union Cyc_Absyn_Cnst c2=_T134;_T10=
# 451
Cyc_Evexp_cmp_cnst(c1,c2);return _T10;}_TLCD: goto _LL3D;case 1: _T11=_T11C.f1;_T12=(int*)_T11;_T13=*_T12;if(_T13!=1)goto _TLCF;_T14=_T11C.f0;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T14;_T15=_T136->f1;_T133=(void*)_T15;}_T16=_T11C.f1;{struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_T16;_T17=_T136->f1;_T132=(void*)_T17;}{void*b1=_T133;void*b2=_T132;_T18=
Cyc_Evexp_cmp_binding(b1,b2);return _T18;}_TLCF: goto _LL3D;case 2: _T19=_T11C.f1;_T1A=(int*)_T19;_T1B=*_T1A;if(_T1B!=2)goto _TLD1;_T1C=_T11C.f0;{struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_T1C;_T131=_T136->f1;}_T1D=_T11C.f1;{struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_T1D;_T130=_T136->f1;}{struct _fat_ptr s1=_T131;struct _fat_ptr s2=_T130;_T1E=
Cyc_strcmp(s1,s2);return _T1E;}_TLD1: goto _LL3D;case 6: _T1F=_T11C.f1;_T20=(int*)_T1F;_T21=*_T20;if(_T21!=6)goto _TLD3;_T22=_T11C.f0;{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T22;_T133=_T136->f1;_T132=_T136->f2;_T12F=_T136->f3;}_T23=_T11C.f1;{struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_T23;_T12E=_T136->f1;_T12D=_T136->f2;_T12C=_T136->f3;}{struct Cyc_Absyn_Exp*e11=_T133;struct Cyc_Absyn_Exp*e12=_T132;struct Cyc_Absyn_Exp*e13=_T12F;struct Cyc_Absyn_Exp*e21=_T12E;struct Cyc_Absyn_Exp*e22=_T12D;struct Cyc_Absyn_Exp*e23=_T12C;
# 457
int c1=Cyc_Evexp_exp_cmp(e11,e21);
if(c1==0)goto _TLD5;_T24=c1;return _T24;_TLD5:
 c1=Cyc_Evexp_exp_cmp(e12,e22);
if(c1==0)goto _TLD7;_T25=c1;return _T25;_TLD7: _T26=
Cyc_Evexp_exp_cmp(e13,e23);return _T26;}_TLD3: goto _LL3D;case 26: _T27=_T11C.f1;_T28=(int*)_T27;_T29=*_T28;if(_T29!=26)goto _TLD9;_T2A=_T11C.f0;{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T2A;_T133=_T136->f1;_T132=_T136->f2;_T12F=_T136->f3;}_T2B=_T11C.f1;{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_T2B;_T12B=_T136->f1;_T12E=_T136->f2;_T12D=_T136->f3;}{struct Cyc_Absyn_Vardecl*vd1=_T133;struct Cyc_Absyn_Exp*e11=_T132;struct Cyc_Absyn_Exp*e12=_T12F;struct Cyc_Absyn_Vardecl*vd2=_T12B;struct Cyc_Absyn_Exp*e21=_T12E;struct Cyc_Absyn_Exp*e22=_T12D;
# 463
if(vd1==vd2)goto _TLDB;_T2C=vd1;_T2D=(int)_T2C;_T2E=vd2;_T2F=(int)_T2E;_T30=_T2D - _T2F;return _T30;_TLDB: _T133=e11;_T132=e12;_T12F=e21;_T12E=e22;goto _LL12;}_TLD9: goto _LL3D;case 7: _T31=_T11C.f1;_T32=(int*)_T31;_T33=*_T32;if(_T33!=7)goto _TLDD;_T34=_T11C.f0;{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T34;_T133=_T136->f1;_T132=_T136->f2;}_T35=_T11C.f1;{struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_T35;_T12F=_T136->f1;_T12E=_T136->f2;}_LL12:{struct Cyc_Absyn_Exp*e11=_T133;struct Cyc_Absyn_Exp*e12=_T132;struct Cyc_Absyn_Exp*e21=_T12F;struct Cyc_Absyn_Exp*e22=_T12E;_T133=e11;_T132=e12;_T12F=e21;_T12E=e22;goto _LL14;}_TLDD: goto _LL3D;case 8: _T36=_T11C.f1;_T37=(int*)_T36;_T38=*_T37;if(_T38!=8)goto _TLDF;_T39=_T11C.f0;{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T39;_T133=_T136->f1;_T132=_T136->f2;}_T3A=_T11C.f1;{struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_T3A;_T12F=_T136->f1;_T12E=_T136->f2;}_LL14:{struct Cyc_Absyn_Exp*e11=_T133;struct Cyc_Absyn_Exp*e12=_T132;struct Cyc_Absyn_Exp*e21=_T12F;struct Cyc_Absyn_Exp*e22=_T12E;_T133=e11;_T132=e12;_T12F=e21;_T12E=e22;goto _LL16;}_TLDF: goto _LL3D;case 9: _T3B=_T11C.f1;_T3C=(int*)_T3B;_T3D=*_T3C;if(_T3D!=9)goto _TLE1;_T3E=_T11C.f0;{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T3E;_T133=_T136->f1;_T132=_T136->f2;}_T3F=_T11C.f1;{struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_T3F;_T12F=_T136->f1;_T12E=_T136->f2;}_LL16:{struct Cyc_Absyn_Exp*e11=_T133;struct Cyc_Absyn_Exp*e12=_T132;struct Cyc_Absyn_Exp*e21=_T12F;struct Cyc_Absyn_Exp*e22=_T12E;
# 468
int c1=Cyc_Evexp_exp_cmp(e11,e21);
if(c1==0)goto _TLE3;_T40=c1;return _T40;_TLE3: _T41=
Cyc_Evexp_exp_cmp(e12,e22);return _T41;}_TLE1: goto _LL3D;case 3: _T42=_T11C.f1;_T43=(int*)_T42;_T44=*_T43;if(_T44!=3)goto _TLE5;_T45=_T11C.f0;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T45;_T12A=_T136->f1;_T133=_T136->f2;}_T46=_T11C.f1;{struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_T46;_T129=_T136->f1;_T132=_T136->f2;}{enum Cyc_Absyn_Primop p1=_T12A;struct Cyc_List_List*es1=_T133;enum Cyc_Absyn_Primop p2=_T129;struct Cyc_List_List*es2=_T132;_T47=p1;_T48=(int)_T47;_T49=p2;_T4A=(int)_T49;{
# 472
int c1=_T48 - _T4A;
if(c1==0)goto _TLE7;_T4B=c1;return _T4B;_TLE7:
 _TLEC: if(es1!=0)goto _TLED;else{goto _TLEB;}_TLED: if(es2!=0)goto _TLEA;else{goto _TLEB;}
_TLEA: _T4C=es1;_T4D=_T4C->hd;_T4E=(struct Cyc_Absyn_Exp*)_T4D;_T4F=es2;_T50=_T4F->hd;_T51=(struct Cyc_Absyn_Exp*)_T50;{int c2=Cyc_Evexp_exp_cmp(_T4E,_T51);
if(c2==0)goto _TLEE;_T52=c2;
return _T52;_TLEE:;}_T53=es1;
# 474
es1=_T53->tl;_T54=es2;es2=_T54->tl;goto _TLEC;_TLEB:
# 479
 return 0;}}_TLE5: goto _LL3D;case 17: _T55=_T11C.f1;_T56=(int*)_T55;_T57=*_T56;switch(_T57){case 17: _T58=_T11C.f0;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T58;_T59=_T136->f1;_T133=(void*)_T59;}_T5A=_T11C.f1;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T5A;_T5B=_T136->f1;_T132=(void*)_T5B;}{void*t1=_T133;void*t2=_T132;_T5C=
# 481
Cyc_Evexp_unify_and_cmp(t1,t2);return _T5C;}case 18: _T5D=_T11C.f0;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T5D;_T5E=_T136->f1;_T133=(void*)_T5E;}_T5F=_T11C.f1;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T5F;_T132=_T136->f1;}{void*t1=_T133;struct Cyc_Absyn_Exp*e2a=_T132;_T60=e2a;{
# 483
void*e2atopt=_T60->topt;
if(e2atopt!=0)goto _TLF1;{struct Cyc_Warn_String_Warn_Warg_struct _T136;_T136.tag=0;
_T136.f1=_tag_fat("cannot handle sizeof(exp) here -- use sizeof(type)",sizeof(char),51U);_T61=_T136;}{struct Cyc_Warn_String_Warn_Warg_struct _T136=_T61;void*_T137[1];_T137[0]=& _T136;_T62=e2;_T63=_T62->loc;_T64=_tag_fat(_T137,sizeof(void*),1);Cyc_Warn_err2(_T63,_T64);}
return 0;_TLF1: _T65=
# 488
Cyc_Evexp_unify_and_cmp(t1,e2atopt);return _T65;}}default: goto _LL3D;};case 18: _T66=_T11C.f1;_T67=(int*)_T66;_T68=*_T67;switch(_T68){case 17: _T69=_T11C.f0;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T69;_T133=_T136->f1;}_T6A=_T11C.f1;{struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_T6A;_T6B=_T136->f1;_T132=(void*)_T6B;}{struct Cyc_Absyn_Exp*e1a=_T133;void*t2=_T132;_T6C=e1a;{
# 490
void*e1atopt=_T6C->topt;
if(e1atopt!=0)goto _TLF4;{struct Cyc_Warn_String_Warn_Warg_struct _T136;_T136.tag=0;
_T136.f1=_tag_fat("cannot handle sizeof(exp) here -- use sizeof(type)",sizeof(char),51U);_T6D=_T136;}{struct Cyc_Warn_String_Warn_Warg_struct _T136=_T6D;void*_T137[1];_T137[0]=& _T136;_T6E=e1;_T6F=_T6E->loc;_T70=_tag_fat(_T137,sizeof(void*),1);Cyc_Warn_err2(_T6F,_T70);}
return 0;_TLF4: _T71=
# 495
Cyc_Evexp_unify_and_cmp(e1atopt,t2);return _T71;}}case 18: _T72=_T11C.f0;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T72;_T133=_T136->f1;}_T73=_T11C.f1;{struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_T73;_T132=_T136->f1;}{struct Cyc_Absyn_Exp*e1a=_T133;struct Cyc_Absyn_Exp*e2a=_T132;_T74=e1a;{
# 497
void*e1atopt=_T74->topt;_T75=e2a;{
void*e2atopt=_T75->topt;
if(e1atopt!=0)goto _TLF6;{struct Cyc_Warn_String_Warn_Warg_struct _T136;_T136.tag=0;
_T136.f1=_tag_fat("cannot handle sizeof(exp) here -- use sizeof(type)",sizeof(char),51U);_T76=_T136;}{struct Cyc_Warn_String_Warn_Warg_struct _T136=_T76;void*_T137[1];_T137[0]=& _T136;_T77=e1;_T78=_T77->loc;_T79=_tag_fat(_T137,sizeof(void*),1);Cyc_Warn_err2(_T78,_T79);}
return 0;
_TLF6: if(e2atopt!=0)goto _TLF8;{struct Cyc_Warn_String_Warn_Warg_struct _T136;_T136.tag=0;
_T136.f1=_tag_fat("cannot handle sizeof(exp) here -- use sizeof(type)",sizeof(char),51U);_T7A=_T136;}{struct Cyc_Warn_String_Warn_Warg_struct _T136=_T7A;void*_T137[1];_T137[0]=& _T136;_T7B=e2;_T7C=_T7B->loc;_T7D=_tag_fat(_T137,sizeof(void*),1);Cyc_Warn_err2(_T7C,_T7D);}
return 0;_TLF8: _T7E=
# 506
Cyc_Evexp_unify_and_cmp(e1atopt,e2atopt);return _T7E;}}}default: goto _LL3D;};case 19: _T7F=_T11C.f1;_T80=(int*)_T7F;_T81=*_T80;if(_T81!=19)goto _TLFA;_T82=_T11C.f0;{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T82;_T83=_T136->f1;_T133=(void*)_T83;_T132=_T136->f2;}_T84=_T11C.f1;{struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_T84;_T85=_T136->f1;_T12F=(void*)_T85;_T128=_T136->f2;}{void*t1=_T133;struct Cyc_List_List*f1=_T132;void*t2=_T12F;struct Cyc_List_List*f2=_T128;
# 508
int c1=Cyc_Evexp_unify_and_cmp(t1,t2);
if(c1==0)goto _TLFC;_T86=c1;return _T86;_TLFC: {
int l1=Cyc_List_length(f1);
int l2=Cyc_List_length(f2);
if(l1 >= l2)goto _TLFE;_T87=- 1;return _T87;_TLFE:
 if(l2 >= l1)goto _TL100;return 1;_TL100:
 _TL105: if(f1!=0)goto _TL106;else{goto _TL104;}_TL106: if(f2!=0)goto _TL103;else{goto _TL104;}
_TL103:{struct _tuple18 _T136;_T89=f1;_T136.f0=_T89->hd;_T8A=f2;_T136.f1=_T8A->hd;_T88=_T136;}{struct _tuple18 _T136=_T88;struct _fat_ptr*_T137;struct _fat_ptr*_T138;_T8B=_T136.f0;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T139=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T8B;_T138=_T139->f1;}_T8C=_T136.f1;{struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*_T139=(struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct*)_T8C;_T137=_T139->f1;}{struct _fat_ptr*fn1=_T138;struct _fat_ptr*fn2=_T137;
# 517
int c=Cyc_strptrcmp(fn1,fn2);
if(c==0)goto _TL107;_T8D=c;return _T8D;_TL107: goto _LL3F;}_LL3F:;}_T8E=f1;
# 514
f1=_T8E->tl;_T8F=f2;f2=_T8F->tl;goto _TL105;_TL104:
# 521
 return 0;}}_TLFA: goto _LL3D;case 14: _T90=_T11C.f1;_T91=(int*)_T90;_T92=*_T91;if(_T92!=14)goto _TL109;_T93=_T11C.f0;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T93;_T94=_T136->f1;_T133=(void*)_T94;_T132=_T136->f2;}_T95=_T11C.f1;{struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_T95;_T96=_T136->f1;_T12F=(void*)_T96;_T12E=_T136->f2;}{void*t1=_T133;struct Cyc_Absyn_Exp*e1a=_T132;void*t2=_T12F;struct Cyc_Absyn_Exp*e2a=_T12E;
# 523
int c1=Cyc_Evexp_unify_and_cmp(t1,t2);
if(c1==0)goto _TL10B;_T97=c1;return _T97;_TL10B: _T98=
Cyc_Evexp_exp_cmp(e1a,e2a);return _T98;}_TL109: goto _LL3D;case 38: _T99=_T11C.f1;_T9A=(int*)_T99;_T9B=*_T9A;if(_T9B!=38)goto _TL10D;_T9C=_T11C.f0;{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T9C;_T9D=_T136->f1;_T133=(void*)_T9D;}_T9E=_T11C.f1;{struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_T9E;_T9F=_T136->f1;_T132=(void*)_T9F;}{void*t1=_T133;void*t2=_T132;_TA0=
# 527
Cyc_Evexp_unify_and_cmp(t1,t2);return _TA0;}_TL10D: goto _LL3D;case 40: _TA1=_T11C.f1;_TA2=(int*)_TA1;_TA3=*_TA2;if(_TA3!=40)goto _TL10F;_TA4=_T11C.f0;{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_TA4;_T133=_T136->f1;}_TA5=_T11C.f1;{struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_TA5;_T132=_T136->f1;}{struct Cyc_Absyn_Exp*e1=_T133;struct Cyc_Absyn_Exp*e2=_T132;_T133=e1;_T132=e2;goto _LL2A;}_TL10F: goto _LL3D;case 12: _TA6=_T11C.f1;_TA7=(int*)_TA6;_TA8=*_TA7;if(_TA8!=12)goto _TL111;_TA9=_T11C.f0;{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_TA9;_T133=_T136->f1;}_TAA=_T11C.f1;{struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_TAA;_T132=_T136->f1;}_LL2A:{struct Cyc_Absyn_Exp*e1=_T133;struct Cyc_Absyn_Exp*e2=_T132;_TAB=
# 530
Cyc_Evexp_exp_cmp(e1,e2);return _TAB;}_TL111: goto _LL3D;case 13: _TAC=_T11C.f1;_TAD=(int*)_TAC;_TAE=*_TAD;if(_TAE!=13)goto _TL113;_TAF=_T11C.f0;{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_TAF;_T133=_T136->f1;_T132=_T136->f2;}_TB0=_T11C.f1;{struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_TB0;_T12E=_T136->f1;_T12F=_T136->f2;}{struct Cyc_Absyn_Exp*e1=_T133;struct Cyc_List_List*t1=_T132;struct Cyc_Absyn_Exp*e2=_T12E;struct Cyc_List_List*t2=_T12F;
# 532
int c1=Cyc_Evexp_exp_cmp(e1,e2);
if(c1==0)goto _TL115;_TB1=c1;return _TB1;_TL115: _TB2=t1;_TB3=t2;_TB4=
Cyc_List_list_cmp(Cyc_Evexp_unify_and_cmp,_TB2,_TB3);return _TB4;}_TL113: goto _LL3D;case 15: _TB5=_T11C.f1;_TB6=(int*)_TB5;_TB7=*_TB6;if(_TB7!=15)goto _TL117;_TB8=_T11C.f0;{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_TB8;_T133=_T136->f1;}_TB9=_T11C.f1;{struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_TB9;_T132=_T136->f1;}{struct Cyc_Absyn_Exp*e1=_T133;struct Cyc_Absyn_Exp*e2=_T132;_TBA=
Cyc_Evexp_exp_cmp(e1,e2);return _TBA;}_TL117: goto _LL3D;case 27: _TBB=_T11C.f1;_TBC=(int*)_TBB;_TBD=*_TBC;if(_TBD!=27)goto _TL119;_TBE=_T11C.f0;{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_TBE;_T133=_T136->f1;_TBF=_T136->f2;_T132=(void*)_TBF;_T127=_T136->f3;}_TC0=_T11C.f1;{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_TC0;_T12E=_T136->f1;_TC1=_T136->f2;_T12F=(void*)_TC1;_T126=_T136->f3;}{struct Cyc_Absyn_Exp*e1=_T133;void*t1=_T132;int b1=_T127;struct Cyc_Absyn_Exp*e2=_T12E;void*t2=_T12F;int b2=_T126;
# 537
int c1=b1 - b2;
if(c1==0)goto _TL11B;_TC2=c1;return _TC2;_TL11B:
 c1=Cyc_Evexp_unify_and_cmp(t1,t2);
if(c1==0)goto _TL11D;_TC3=c1;return _TC3;_TL11D: _TC4=
Cyc_Evexp_exp_cmp(e1,e2);return _TC4;}_TL119: goto _LL3D;case 29: _TC5=_T11C.f1;_TC6=(int*)_TC5;_TC7=*_TC6;if(_TC7!=29)goto _TL11F;_TC8=_T11C.f0;{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_TC8;_TC9=_T136->f1;_T133=(void*)_TC9;_T128=_T136->f3;}_TCA=_T11C.f1;{struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_TCA;_TCB=_T136->f1;_T132=(void*)_TCB;_T125=_T136->f3;}{void*t1=_T133;struct Cyc_List_List*des1=_T128;void*t2=_T132;struct Cyc_List_List*des2=_T125;
# 543
int c1=0;
if(t1==0)goto _TL121;if(t2==0)goto _TL121;
c1=Cyc_Evexp_unify_and_cmp(t1,t2);goto _TL122;
_TL121: if(t1!=0)goto _TL123;c1=- 1;goto _TL124;
_TL123: if(t2!=0)goto _TL125;c1=1;goto _TL126;_TL125: _TL126: _TL124: _TL122:
 if(c1==0)goto _TL127;_TCC=c1;return _TCC;_TL127: _T128=des1;_T125=des2;goto _LL34;}_TL11F: goto _LL3D;case 25: _TCD=_T11C.f1;_TCE=(int*)_TCD;_TCF=*_TCE;if(_TCF!=25)goto _TL129;_TD0=_T11C.f0;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_TD0;_T128=_T136->f1;}_TD1=_T11C.f1;{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_TD1;_T125=_T136->f1;}_LL34:{struct Cyc_List_List*des1=_T128;struct Cyc_List_List*des2=_T125;_TD3=Cyc_List_list_cmp;{
# 551
int(*_T136)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*)=(int(*)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*))_TD3;_TD2=_T136;}_TD4=des1;_TD5=des2;_TD6=_TD2(Cyc_Evexp_designator_exp_cmp,_TD4,_TD5);return _TD6;}_TL129: goto _LL3D;case 28: _TD7=_T11C.f1;_TD8=(int*)_TD7;_TD9=*_TD8;if(_TD9!=28)goto _TL12B;_TDA=_T11C.f0;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TDA;_T128=_T136->f2;_T125=_T136->f3;_T124=_T136->f4;}_TDB=_T11C.f1;{struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_TDB;_T123=_T136->f2;_T122=_T136->f3;_T121=_T136->f4;}{struct Cyc_List_List*ts1=_T128;struct Cyc_List_List*des1=_T125;struct Cyc_Absyn_Aggrdecl*ad1=_T124;struct Cyc_List_List*ts2=_T123;struct Cyc_List_List*des2=_T122;struct Cyc_Absyn_Aggrdecl*ad2=_T121;_TDC=ad1;_TDD=(int)_TDC;_TDE=ad2;_TDF=(int)_TDE;{
# 553
int c1=_TDD - _TDF;
if(c1==0)goto _TL12D;_TE0=c1;return _TE0;_TL12D: _TE1=ts1;_TE2=ts2;
c1=Cyc_List_list_cmp(Cyc_Evexp_unify_and_cmp,_TE1,_TE2);
if(c1==0)goto _TL12F;_TE3=c1;return _TE3;_TL12F: _TE5=Cyc_List_list_cmp;{
int(*_T136)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*)=(int(*)(int(*)(struct _tuple19*,struct _tuple19*),struct Cyc_List_List*,struct Cyc_List_List*))_TE5;_TE4=_T136;}_TE6=des1;_TE7=des1;_TE8=_TE4(Cyc_Evexp_designator_exp_cmp,_TE6,_TE7);return _TE8;}}_TL12B: goto _LL3D;case 30: _TE9=_T11C.f1;_TEA=(int*)_TE9;_TEB=*_TEA;if(_TEB!=30)goto _TL131;_TEC=_T11C.f0;{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_TEC;_T128=_T136->f1;_T120=_T136->f2;_T11F=_T136->f3;}_TED=_T11C.f1;{struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_T136=(struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_TED;_T125=_T136->f1;_T11E=_T136->f2;_T11D=_T136->f3;}{struct Cyc_List_List*es1=_T128;struct Cyc_Absyn_Datatypedecl*dd1=_T120;struct Cyc_Absyn_Datatypefield*df1=_T11F;struct Cyc_List_List*es2=_T125;struct Cyc_Absyn_Datatypedecl*dd2=_T11E;struct Cyc_Absyn_Datatypefield*df2=_T11D;_TEE=dd1;_TEF=(int)_TEE;_TF0=dd2;_TF1=(int)_TF0;{
# 559
int c1=_TEF - _TF1;
if(c1==0)goto _TL133;_TF2=c1;return _TF2;_TL133: _TF3=df1;_TF4=(int)_TF3;_TF5=df2;_TF6=(int)_TF5;
c1=_TF4 - _TF6;
if(c1==0)goto _TL135;_TF7=c1;return _TF7;_TL135: _TF9=Cyc_List_list_cmp;{
int(*_T136)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*)=(int(*)(int(*)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_List_List*,struct Cyc_List_List*))_TF9;_TF8=_T136;}_TFA=es1;_TFB=es2;_TFC=_TF8(Cyc_Evexp_exp_cmp,_TFA,_TFB);return _TFC;}}_TL131: goto _LL3D;case 31: _TFD=_T11C.f1;_TFE=(int*)_TFD;_TFF=*_TFE;if(_TFF!=31)goto _TL137;goto _LL3C;_TL137: goto _LL3D;case 32: _T100=_T11C.f1;_T101=(int*)_T100;_T102=*_T101;if(_T102!=32)goto _TL139;_LL3C: _T103=e1;_T104=(int)_T103;_T105=e2;_T106=(int)_T105;_T107=_T104 - _T106;
# 567
return _T107;_TL139: goto _LL3D;default: _LL3D:{struct Cyc_Warn_String_Warn_Warg_struct _T136;_T136.tag=0;
_T136.f1=_tag_fat("Evexp::exp_cmp, unexpected case $([",sizeof(char),36U);_T108=_T136;}{struct Cyc_Warn_String_Warn_Warg_struct _T136=_T108;{struct Cyc_Warn_Int_Warn_Warg_struct _T137;_T137.tag=12;_T10A=e1case;_T137.f1=(int)_T10A;_T109=_T137;}{struct Cyc_Warn_Int_Warn_Warg_struct _T137=_T109;{struct Cyc_Warn_String_Warn_Warg_struct _T138;_T138.tag=0;_T138.f1=_tag_fat("]",sizeof(char),2U);_T10B=_T138;}{struct Cyc_Warn_String_Warn_Warg_struct _T138=_T10B;{struct Cyc_Warn_Exp_Warn_Warg_struct _T139;_T139.tag=4;
_T139.f1=e1;_T10C=_T139;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T139=_T10C;{struct Cyc_Warn_String_Warn_Warg_struct _T13A;_T13A.tag=0;_T13A.f1=_tag_fat(",[",sizeof(char),3U);_T10D=_T13A;}{struct Cyc_Warn_String_Warn_Warg_struct _T13A=_T10D;{struct Cyc_Warn_Int_Warn_Warg_struct _T13B;_T13B.tag=12;_T10F=e2case;_T13B.f1=(int)_T10F;_T10E=_T13B;}{struct Cyc_Warn_Int_Warn_Warg_struct _T13B=_T10E;{struct Cyc_Warn_String_Warn_Warg_struct _T13C;_T13C.tag=0;_T13C.f1=_tag_fat("]",sizeof(char),2U);_T110=_T13C;}{struct Cyc_Warn_String_Warn_Warg_struct _T13C=_T110;{struct Cyc_Warn_Exp_Warn_Warg_struct _T13D;_T13D.tag=4;_T13D.f1=e2;_T111=_T13D;}{struct Cyc_Warn_Exp_Warn_Warg_struct _T13D=_T111;{struct Cyc_Warn_String_Warn_Warg_struct _T13E;_T13E.tag=0;_T13E.f1=_tag_fat(")",sizeof(char),2U);_T112=_T13E;}{struct Cyc_Warn_String_Warn_Warg_struct _T13E=_T112;void*_T13F[9];_T13F[0]=& _T136;_T13F[1]=& _T137;_T13F[2]=& _T138;_T13F[3]=& _T139;_T13F[4]=& _T13A;_T13F[5]=& _T13B;_T13F[6]=& _T13C;_T13F[7]=& _T13D;_T13F[8]=& _T13E;_T114=Cyc_Warn_impos2;{
# 568
int(*_T140)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T114;_T113=_T140;}_T115=_tag_fat(_T13F,sizeof(void*),9);_T113(_T115);}}}}}}}}}};}}}}}
# 573
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int _T0;int _T1;unsigned _T2;int _T3;int _T4;
# 575
e1=Cyc_Evexp_strip_cast(e1);
e2=Cyc_Evexp_strip_cast(e2);{
struct _tuple12 _T5=Cyc_Evexp_eval_const_exp(e1);int _T6;union Cyc_Absyn_Cnst _T7;_T7=_T5.f0;_T6=_T5.f1;{union Cyc_Absyn_Cnst c1=_T7;int known1=_T6;
struct _tuple12 _T8=Cyc_Evexp_eval_const_exp(e2);int _T9;union Cyc_Absyn_Cnst _TA;_TA=_T8.f0;_T9=_T8.f1;{union Cyc_Absyn_Cnst c2=_TA;int known2=_T9;_T0=known1;
if(!_T0)goto _TL13B;_T1=known2;if(!_T1)goto _TL13B;{
struct _tuple11 _TB=Cyc_Evexp_eval_const_uint_exp(e1);unsigned _TC;_TC=_TB.f0;{unsigned i1=_TC;
struct _tuple11 _TD=Cyc_Evexp_eval_const_uint_exp(e2);unsigned _TE;_TE=_TD.f0;{unsigned i2=_TE;_T2=i1 - i2;_T3=(int)_T2;
return _T3;}}}_TL13B: _T4=
# 584
Cyc_Evexp_exp_cmp(e1,e2);return _T4;}}}}
# 587
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int _T0;int _T1;_T0=
Cyc_Evexp_uint_exp_cmp(e1,e2);_T1=_T0==0;return _T1;}
# 590
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){int _T0;int _T1;int _T2;int _T3;
struct _tuple11 _T4=Cyc_Evexp_eval_const_uint_exp(e1);int _T5;unsigned _T6;_T6=_T4.f0;_T5=_T4.f1;{unsigned i1=_T6;int known1=_T5;
struct _tuple11 _T7=Cyc_Evexp_eval_const_uint_exp(e2);int _T8;unsigned _T9;_T9=_T7.f0;_T8=_T7.f1;{unsigned i2=_T9;int known2=_T8;_T0=known1;
if(!_T0)goto _TL13D;_T1=known2;if(!_T1)goto _TL13D;_T2=i1 <= i2;
return _T2;_TL13D: _T3=
Cyc_Evexp_same_uint_const_exp(e1,e2);return _T3;}}}
# 600
int Cyc_Evexp_okay_szofarg(void*t){int*_T0;unsigned _T1;void*_T2;void*_T3;int*_T4;unsigned _T5;void*_T6;struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T7;union Cyc_Absyn_AggrInfo _T8;struct _union_AggrInfo_UnknownAggr _T9;unsigned _TA;struct Cyc_Warn_String_Warn_Warg_struct _TB;int(*_TC)(struct _fat_ptr);void*(*_TD)(struct _fat_ptr);struct _fat_ptr _TE;void*_TF;union Cyc_Absyn_AggrInfo _T10;struct _union_AggrInfo_KnownAggr _T11;struct Cyc_Absyn_Aggrdecl**_T12;struct Cyc_Absyn_Aggrdecl*_T13;struct Cyc_Absyn_AggrdeclImpl*_T14;struct _RegionHandle*_T15;struct _RegionHandle*_T16;struct Cyc_Absyn_Aggrdecl*_T17;struct Cyc_List_List*_T18;struct Cyc_List_List*_T19;struct Cyc_Absyn_Aggrdecl*_T1A;struct Cyc_Absyn_AggrdeclImpl*_T1B;struct Cyc_Absyn_AggrdeclImpl*_T1C;struct _RegionHandle*_T1D;struct Cyc_List_List*_T1E;struct Cyc_List_List*_T1F;void*_T20;struct Cyc_Absyn_Aggrfield*_T21;void*_T22;void*_T23;int _T24;struct Cyc_List_List*_T25;struct Cyc_Absyn_Tvar*_T26;struct Cyc_Absyn_Kind*_T27;struct Cyc_Absyn_Kind*_T28;struct Cyc_Absyn_Kind*_T29;enum Cyc_Absyn_KindQual _T2A;int _T2B;int _T2C;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T2D;struct Cyc_Core_Opt*_T2E;struct Cyc_Absyn_Evar_Absyn_Type_struct*_T2F;struct Cyc_Core_Opt*_T30;struct Cyc_Core_Opt*_T31;struct Cyc_Absyn_Kind*_T32;struct Cyc_Absyn_Kind*_T33;enum Cyc_Absyn_KindQual _T34;struct Cyc_Absyn_ArrayInfo _T35;struct Cyc_Absyn_ArrayInfo _T36;int _T37;struct Cyc_List_List*_T38;void*_T39;struct Cyc_Absyn_Aggrfield*_T3A;void*_T3B;int _T3C;struct Cyc_List_List*_T3D;struct Cyc_Absyn_Typedefdecl*_T3E;struct Cyc_Core_Opt*_T3F;struct Cyc_Warn_String_Warn_Warg_struct _T40;struct Cyc_Warn_Typ_Warn_Warg_struct _T41;int(*_T42)(struct _fat_ptr);void*(*_T43)(struct _fat_ptr);struct _fat_ptr _T44;struct Cyc_Absyn_Typedefdecl*_T45;struct Cyc_Core_Opt*_T46;void*_T47;struct Cyc_Absyn_Kind*_T48;enum Cyc_Absyn_KindQual _T49;int _T4A;int _T4B;
void*_T4C=Cyc_Absyn_compress(t);struct Cyc_Absyn_Typedefdecl*_T4D;struct Cyc_Absyn_Exp*_T4E;struct Cyc_List_List*_T4F;void*_T50;_T0=(int*)_T4C;_T1=*_T0;switch(_T1){case 0:{struct Cyc_Absyn_AppType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T4C;_T2=_T51->f1;_T50=(void*)_T2;_T4F=_T51->f2;}{void*c=_T50;struct Cyc_List_List*ts=_T4F;struct Cyc_Absyn_Aggrdecl*_T51;_T3=c;_T4=(int*)_T3;_T5=*_T4;switch(_T5){case 23:
# 604
 return 0;case 5: goto _LL21;case 1: _LL21: goto _LL23;case 2: _LL23: goto _LL25;case 3: _LL25: goto _LL27;case 21: _LL27: goto _LL29;case 19: _LL29: goto _LL2B;case 4: _LL2B: goto _LL2D;case 18: _LL2D: goto _LL2F;case 22: _LL2F: goto _LL31;case 20: _LL31:
# 614
 return 1;case 0: goto _LL35;case 6: _LL35: goto _LL37;case 8: _LL37: goto _LL39;case 7: _LL39: goto _LL3B;case 9: _LL3B: goto _LL3D;case 11: _LL3D: goto _LL3F;case 12: _LL3F: goto _LL41;case 13: _LL41: goto _LL43;case 14: _LL43: goto _LL45;case 10: _LL45: goto _LL47;case 15: _LL47: goto _LL49;case 16: _LL49: goto _LL4B;case 17: _LL4B:
# 630
 return 0;default: _T6=c;_T7=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_T6;_T8=_T7->f1;_T9=_T8.UnknownAggr;_TA=_T9.tag;if(_TA!=1)goto _TL141;{struct Cyc_Warn_String_Warn_Warg_struct _T52;_T52.tag=0;
# 632
_T52.f1=_tag_fat("szof on unchecked StructType",sizeof(char),29U);_TB=_T52;}{struct Cyc_Warn_String_Warn_Warg_struct _T52=_TB;void*_T53[1];_T53[0]=& _T52;_TD=Cyc_Warn_impos2;{int(*_T54)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_TD;_TC=_T54;}_TE=_tag_fat(_T53,sizeof(void*),1);_TC(_TE);}goto _TL142;_TL141: _TF=c;{struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*_T52=(struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_TF;_T10=_T52->f1;_T11=_T10.KnownAggr;_T12=_T11.val;{struct Cyc_Absyn_Aggrdecl*_T53=*_T12;_T51=_T53;}}{struct Cyc_Absyn_Aggrdecl*ad=_T51;_T13=ad;_T14=_T13->impl;
# 634
if(_T14!=0)goto _TL143;return 0;_TL143: {struct _RegionHandle _T52=_new_region(0U,"temp");struct _RegionHandle*temp=& _T52;_push_region(temp);_T15=temp;_T16=temp;_T17=ad;_T18=_T17->tvs;_T19=ts;{
# 636
struct Cyc_List_List*inst=Cyc_List_rzip(_T15,_T16,_T18,_T19);_T1A=ad;_T1B=_T1A->impl;_T1C=
_check_null(_T1B);{struct Cyc_List_List*fs=_T1C->fields;_TL148: if(fs!=0)goto _TL146;else{goto _TL147;}
_TL146: _T1D=temp;_T1E=inst;_T1F=fs;_T20=_T1F->hd;_T21=(struct Cyc_Absyn_Aggrfield*)_T20;_T22=_T21->type;_T23=Cyc_Tcutil_rsubstitute(_T1D,_T1E,_T22);_T24=Cyc_Evexp_okay_szofarg(_T23);if(_T24)goto _TL149;else{goto _TL14B;}
_TL14B:{int _T53=0;_npop_handler(0);return _T53;}_TL149: _T25=fs;
# 637
fs=_T25->tl;goto _TL148;_TL147:;}{int _T53=1;_npop_handler(0);return _T53;}}_pop_region();}}_TL142:;};}case 2:{struct Cyc_Absyn_VarType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_VarType_Absyn_Type_struct*)_T4C;_T50=_T51->f1;}{struct Cyc_Absyn_Tvar*tv=_T50;_T26=tv;_T27=& Cyc_Kinds_bk;_T28=(struct Cyc_Absyn_Kind*)_T27;_T29=
# 642
Cyc_Kinds_tvar_kind(_T26,_T28);_T2A=_T29->kind;_T2B=(int)_T2A;_T2C=_T2B==2;return _T2C;}case 3:
 return 0;case 1: _T2D=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T4C;_T2E=_T2D->f1;if(_T2E==0)goto _TL14C;_T2F=(struct Cyc_Absyn_Evar_Absyn_Type_struct*)_T4C;_T30=_T2F->f1;_T31=(struct Cyc_Core_Opt*)_T30;_T32=_T31->v;_T33=(struct Cyc_Absyn_Kind*)_T32;_T34=_T33->kind;if(_T34!=Cyc_Absyn_BoxKind)goto _TL14E;
return 1;_TL14E: goto _LL9;_TL14C: _LL9:
 return 0;case 4:
# 648
 return 1;case 5:{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_T4C;_T35=_T51->f1;_T50=_T35.elt_type;_T36=_T51->f1;_T4E=_T36.num_elts;}{void*t2=_T50;struct Cyc_Absyn_Exp*e=_T4E;_T37=e!=0;
# 651
return _T37;}case 6:
 return 0;case 7:{struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_T4C;_T4F=_T51->f3;}{struct Cyc_List_List*fs=_T4F;
# 654
_TL153: if(fs!=0)goto _TL151;else{goto _TL152;}
_TL151: _T38=fs;_T39=_T38->hd;_T3A=(struct Cyc_Absyn_Aggrfield*)_T39;_T3B=_T3A->type;_T3C=Cyc_Evexp_okay_szofarg(_T3B);if(_T3C)goto _TL154;else{goto _TL156;}
_TL156: return 0;_TL154: _T3D=fs;
# 654
fs=_T3D->tl;goto _TL153;_TL152:
# 657
 return 1;}case 10: goto _LL16;case 9: _LL16: goto _LL18;case 11: _LL18:
# 660
 return 0;default:{struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_T51=(struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_T4C;_T4D=_T51->f3;}{struct Cyc_Absyn_Typedefdecl*td=_T4D;
# 663
if(td==0)goto _TL159;else{goto _TL15A;}_TL15A: _T3E=td;_T3F=_T3E->kind;if(_T3F==0)goto _TL159;else{goto _TL157;}
_TL159:{struct Cyc_Warn_String_Warn_Warg_struct _T51;_T51.tag=0;_T51.f1=_tag_fat("szof typedeftype ",sizeof(char),18U);_T40=_T51;}{struct Cyc_Warn_String_Warn_Warg_struct _T51=_T40;{struct Cyc_Warn_Typ_Warn_Warg_struct _T52;_T52.tag=2;_T52.f1=t;_T41=_T52;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T52=_T41;void*_T53[2];_T53[0]=& _T51;_T53[1]=& _T52;_T43=Cyc_Warn_impos2;{int(*_T54)(struct _fat_ptr)=(int(*)(struct _fat_ptr))_T43;_T42=_T54;}_T44=_tag_fat(_T53,sizeof(void*),2);_T42(_T44);}}goto _TL158;_TL157: _TL158: _T45=td;_T46=_T45->kind;_T47=_T46->v;_T48=(struct Cyc_Absyn_Kind*)_T47;_T49=_T48->kind;_T4A=(int)_T49;_T4B=_T4A==2;
return _T4B;}};}
