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
# 173
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_ExistAssnFn;struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*checks_clause;struct Cyc_AssnDef_ExistAssnFn*checks_assn;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_ExistAssnFn*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_ExistAssnFn*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_ExistAssnFn*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;struct Cyc_Absyn_Exp*rename;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;int escapes;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 918 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 929
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 932
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 939
void*Cyc_Absyn_compress(void*);
# 952
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 968
extern void*Cyc_Absyn_al_qual_type;
# 972
extern void*Cyc_Absyn_false_type;
# 1002
void*Cyc_Absyn_string_type(void*,void*);
void*Cyc_Absyn_const_string_type(void*,void*);
# 1027
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1035
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1065
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1130
struct _tuple1*Cyc_Absyn_uniqueaqual_qvar (void);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;int gen_clean_cyclone;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 63
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 43 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 75
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 77
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 55 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);
# 39 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 42
int Cyc_Tcutil_is_array_type(void*);
# 54
int Cyc_Tcutil_is_bits_only_type(void*);
# 85
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 106
int Cyc_Tcutil_coerce_assign(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
# 159 "tcutil.h"
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 169
void Cyc_Tcutil_check_bitfield(unsigned,void*,struct Cyc_Absyn_Exp*,struct _fat_ptr*);
# 172
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 205
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*,void*);
# 220
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 226
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 232
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 249
int Cyc_Tcutil_zeroable_type(void*);
# 256
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct _tuple12{struct Cyc_Absyn_Tvar*f0;int f1;};
# 273
struct _tuple12*Cyc_Tcutil_tvar_bool_pair(int,struct Cyc_Absyn_Tvar*);
# 283
int Cyc_Tcutil_is_main(struct _tuple1*);
# 29 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
# 77 "kinds.h"
void*Cyc_Kinds_kind_to_bound(struct Cyc_Absyn_Kind*);
# 89
void*Cyc_Kinds_compress_kb(void*);
# 43 "attributes.h"
extern struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct Cyc_Atts_No_throw_att_val;
# 62
struct Cyc_List_List*Cyc_Atts_transfer_fn_type_atts(void*,struct Cyc_List_List*);
# 70
void Cyc_Atts_check_fndecl_atts(unsigned,struct Cyc_List_List*,int);
void Cyc_Atts_check_variable_atts(unsigned,struct Cyc_Absyn_Vardecl*,struct Cyc_List_List*);
void Cyc_Atts_check_field_atts(unsigned,struct _fat_ptr*,struct Cyc_List_List*);
# 74
void Cyc_Atts_fnTypeAttsOK(unsigned,void*);
# 79
int Cyc_Atts_attribute_cmp(void*,void*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 87 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);
# 270
extern struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,void*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned,struct Cyc_Absyn_Fndecl*);
# 78
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 82
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*);
# 143
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
# 154
void Cyc_Tcenv_check_delayed_effects(struct Cyc_Tcenv_Tenv*);
void Cyc_Tcenv_check_delayed_constraints(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_copy_tenv_dicts(struct Cyc_Tcenv_Tenv*);
# 34 "tctyp.h"
void Cyc_Tctyp_check_valid_toplevel_type(unsigned,struct Cyc_Tcenv_Tenv*,void*);
void Cyc_Tctyp_check_fndecl_valid_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Fndecl*);
# 47 "tctyp.h"
void Cyc_Tctyp_check_type(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_Absyn_Kind*,int,int,void*);
# 50
void Cyc_Tctyp_check_type_with_bounds(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*,struct Cyc_List_List**,struct Cyc_Absyn_Kind*,int,int,void*);
# 26 "tcexp.h"
void*Cyc_Tcexp_tcExp(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
void*Cyc_Tcexp_tcExpInitializer(struct Cyc_Tcenv_Tenv*,void**,struct Cyc_Absyn_Exp*);
# 26 "tcstmt.h"
void Cyc_Tcstmt_tcStmt(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Stmt*,int);
int Cyc_Tcstmt_ensure_no_throw_stmt(struct Cyc_Absyn_Stmt*);struct _tuple13{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 54 "tcdecl.h"
struct Cyc_Absyn_Aggrdecl*Cyc_Tcdecl_merge_aggrdecl(struct Cyc_Absyn_Aggrdecl*,struct Cyc_Absyn_Aggrdecl*,unsigned,struct _fat_ptr*);
# 57
struct Cyc_Absyn_Datatypedecl*Cyc_Tcdecl_merge_datatypedecl(struct Cyc_Absyn_Datatypedecl*,struct Cyc_Absyn_Datatypedecl*,unsigned,struct _fat_ptr*);
# 59
struct Cyc_Absyn_Enumdecl*Cyc_Tcdecl_merge_enumdecl(struct Cyc_Absyn_Enumdecl*,struct Cyc_Absyn_Enumdecl*,unsigned,struct _fat_ptr*);
# 63
struct Cyc_Absyn_Typedefdecl*Cyc_Tcdecl_merge_typedefdecl(struct Cyc_Absyn_Typedefdecl*,struct Cyc_Absyn_Typedefdecl*,unsigned,struct _fat_ptr*);
# 66
void*Cyc_Tcdecl_merge_binding(void*,void*,unsigned,struct _fat_ptr*);
# 74
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*,int*,struct _fat_ptr*,unsigned,struct _fat_ptr*);struct Cyc_Set_Set;
# 36 "cifc.h"
void Cyc_Cifc_user_overrides(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List**,struct Cyc_List_List*);
# 47 "toc.h"
void Cyc_Toc_init (void);
void Cyc_Toc_finish (void);struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};
# 232 "assndef.h"
extern struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 234
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 238
extern void*Cyc_AssnDef_not(void*);struct Cyc_AssnDef_AssnFn{struct Cyc_List_List*actuals;void*assn;};struct Cyc_AssnDef_ExistAssnFn{struct Cyc_AssnDef_AssnFn*af;struct Cyc_Set_Set*existvars;};struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};
# 8 "pratt_prover.h"
int Cyc_PrattProver_constraint_prove(void*,void*);
# 45 "vcgen.h"
extern struct Cyc_AssnDef_AssnMap Cyc_Vcgen_clause2assn(struct Cyc_Absyn_Exp*);static char _TmpG0[1U]="";
# 49 "tc.cyc"
static struct _fat_ptr Cyc_Tc_tc_msg_c={_TmpG0,_TmpG0,_TmpG0 + 1U};
static struct _fat_ptr*Cyc_Tc_tc_msg=(struct _fat_ptr*)& Cyc_Tc_tc_msg_c;struct _tuple14{unsigned f0;struct _tuple1*f1;int f2;};
# 52
static int Cyc_Tc_export_member(struct _tuple1*x,struct Cyc_List_List*exports){struct Cyc_List_List*_T0;void*_T1;struct _tuple1*_T2;struct _tuple14*_T3;struct _tuple14 _T4;struct _tuple1*_T5;int _T6;int _T7;struct _tuple14*_T8;struct Cyc_List_List*_T9;
_TL3: if(exports!=0)goto _TL1;else{goto _TL2;}
_TL1: _T0=exports;_T1=_T0->hd;{struct _tuple14*p=(struct _tuple14*)_T1;_T2=x;_T3=p;_T4=*_T3;_T5=_T4.f1;_T6=
Cyc_Absyn_qvar_cmp(_T2,_T5);_T7=!_T6;if(_T7!=0)goto _TL4;goto _TL0;_TL4: _T8=p;
# 59
(*_T8).f2=1;
return 1;}_TL0: _T9=exports;
# 53
exports=_T9->tl;goto _TL3;_TL2:
# 62
 return 0;}struct _tuple15{void*f0;int f1;};
# 65
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){struct Cyc_Absyn_Vardecl*_T0;struct Cyc_Absyn_Vardecl*_T1;int*_T2;int*_T3;int _T4;struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_T5;struct Cyc_Absyn_ArrayInfo _T6;struct Cyc_Absyn_Exp*_T7;struct Cyc_Absyn_ArrayInfo _T8;struct Cyc_Absyn_ArrayInfo _T9;struct Cyc_Absyn_ArrayInfo _TA;struct Cyc_Absyn_ArrayInfo _TB;struct Cyc_Absyn_Exp*_TC;int*_TD;unsigned _TE;struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_TF;union Cyc_Absyn_Cnst _T10;struct _union_Cnst_Wstring_c _T11;unsigned _T12;union Cyc_Absyn_Cnst _T13;struct _union_Cnst_String_c _T14;struct Cyc_Absyn_Vardecl*_T15;void*_T16;struct Cyc_Absyn_Tqual _T17;struct _fat_ptr _T18;unsigned _T19;struct Cyc_Absyn_Exp*_T1A;void*_T1B;unsigned _T1C;union Cyc_Absyn_Cnst _T1D;struct _union_Cnst_Wstring_c _T1E;struct Cyc_Absyn_Vardecl*_T1F;void*_T20;struct Cyc_Absyn_Tqual _T21;struct Cyc_Absyn_Exp*_T22;void*_T23;unsigned _T24;struct Cyc_Absyn_Vardecl*_T25;struct Cyc_Absyn_Vardecl*_T26;void*_T27;struct Cyc_Absyn_Tqual _T28;int _T29;unsigned _T2A;struct Cyc_Absyn_Exp*_T2B;void*_T2C;unsigned _T2D;struct Cyc_Absyn_Vardecl*_T2E;unsigned _T2F;struct Cyc_Absyn_Vardecl*_T30;struct Cyc_Absyn_Tqual _T31;int _T32;void*_T33;struct Cyc_Absyn_Vardecl*_T34;int _T35;int _T36;int*_T37;enum Cyc_Absyn_Scope _T38;int _T39;enum Cyc_Absyn_Scope _T3A;int _T3B;int _T3C;struct Cyc_Warn_String_Warn_Warg_struct _T3D;unsigned _T3E;struct _fat_ptr _T3F;int _T40;int _T41;int _T42;int _T43;int _T44;struct Cyc_Warn_String_Warn_Warg_struct _T45;struct Cyc_Warn_Vardecl_Warn_Warg_struct _T46;struct Cyc_Warn_String_Warn_Warg_struct _T47;struct Cyc_Warn_Typ_Warn_Warg_struct _T48;unsigned _T49;struct _fat_ptr _T4A;struct _handler_cons*_T4B;int*_T4C;int _T4D;struct Cyc_Tcenv_Tenv*_T4E;void**_T4F;struct Cyc_Absyn_Exp*_T50;struct Cyc_List_List*_T51;struct Cyc_Absyn_Exp*_T52;void*_T53;int _T54;struct Cyc_Absyn_Vardecl*_T55;struct _tuple1*_T56;unsigned long _T57;const char*_T58;struct _fat_ptr _T59;unsigned long _T5A;unsigned long _T5B;unsigned long _T5C;unsigned long _T5D;const char*_T5E;struct _fat_ptr _T5F;unsigned long _T60;unsigned long _T61;unsigned long _T62;unsigned long _T63;struct Cyc_String_pa_PrintArg_struct _T64;struct Cyc_String_pa_PrintArg_struct _T65;const char*_T66;struct Cyc_String_pa_PrintArg_struct _T67;struct Cyc_String_pa_PrintArg_struct _T68;const char*_T69;struct Cyc_String_pa_PrintArg_struct _T6A;unsigned _T6B;struct _fat_ptr _T6C;struct _fat_ptr _T6D;struct Cyc_String_pa_PrintArg_struct _T6E;struct Cyc_String_pa_PrintArg_struct _T6F;const char*_T70;struct Cyc_String_pa_PrintArg_struct _T71;struct Cyc_String_pa_PrintArg_struct _T72;const char*_T73;struct Cyc_String_pa_PrintArg_struct _T74;unsigned _T75;struct _fat_ptr _T76;struct _fat_ptr _T77;int _T78;struct Cyc_Warn_String_Warn_Warg_struct _T79;unsigned _T7A;struct _fat_ptr _T7B;void*_T7C;struct Cyc_Tcenv_Env_error_exn_struct*_T7D;char*_T7E;char*_T7F;struct Cyc_Warn_String_Warn_Warg_struct _T80;unsigned _T81;struct _fat_ptr _T82;struct _handler_cons*_T83;int*_T84;int _T85;struct _tuple15*(*_T86)(struct Cyc_Dict_Dict,struct _tuple1*);void*(*_T87)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T88;struct Cyc_Tcenv_Genv*_T89;struct Cyc_Dict_Dict _T8A;struct _tuple1*_T8B;struct _tuple15*_T8C;struct _tuple15 _T8D;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T8E;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T8F;void*_T90;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_T91;void*_T92;unsigned _T93;struct _fat_ptr*_T94;struct Cyc_Absyn_Vardecl*_T95;struct _tuple1*_T96;struct Cyc_List_List**_T97;struct Cyc_List_List*_T98;int _T99;struct _tuple15*_T9A;struct _tuple15 _T9B;int _T9C;struct Cyc_Tcenv_Tenv*_T9D;struct Cyc_Tcenv_Genv*_T9E;struct Cyc_Dict_Dict(*_T9F)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*);struct Cyc_Dict_Dict(*_TA0)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_TA1;struct Cyc_Tcenv_Genv*_TA2;struct Cyc_Dict_Dict _TA3;struct _tuple1*_TA4;struct _tuple15*_TA5;struct _tuple15*_TA6;struct _tuple15 _TA7;void*_TA8;struct Cyc_Dict_Absent_exn_struct*_TA9;char*_TAA;char*_TAB;struct Cyc_Absyn_Vardecl*_TAC;struct _tuple1*_TAD;struct Cyc_List_List**_TAE;struct Cyc_List_List*_TAF;int _TB0;struct Cyc_Tcenv_Tenv*_TB1;struct Cyc_Tcenv_Genv*_TB2;struct Cyc_Dict_Dict(*_TB3)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*);struct Cyc_Dict_Dict(*_TB4)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_TB5;struct Cyc_Tcenv_Genv*_TB6;struct Cyc_Dict_Dict _TB7;struct _tuple1*_TB8;struct _tuple15*_TB9;struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_TBA;void*_TBB;struct Cyc_List_List*_TBC;struct Cyc_Absyn_Exp*_TBD;void*_TBE;struct _tuple1*_TBF;enum Cyc_Absyn_Scope _TC0;_T0=vd;{struct Cyc_Absyn_Vardecl _TC1=*_T0;_TC0=_TC1.sc;_TBF=_TC1.name;_TBE=_TC1.type;_TBD=_TC1.initializer;_TBC=_TC1.attributes;_T1=vd;_T2=& _T1->is_proto;_TBB=(int*)_T2;}{enum Cyc_Absyn_Scope sc=_TC0;struct _tuple1*q=_TBF;void*t=_TBE;struct Cyc_Absyn_Exp*initializer=_TBD;struct Cyc_List_List*atts=_TBC;int*is_proto=(int*)_TBB;{
# 74
void*_TC1=Cyc_Absyn_compress(t);unsigned _TC2;void*_TC3;struct Cyc_Absyn_Tqual _TC4;void*_TC5;_T3=(int*)_TC1;_T4=*_T3;if(_T4!=5)goto _TL6;_T5=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TC1;_T6=_T5->f1;_T7=_T6.num_elts;if(_T7!=0)goto _TL8;{struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_TC6=(struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_TC1;_T8=_TC6->f1;_TC5=_T8.elt_type;_T9=_TC6->f1;_TC4=_T9.tq;_TA=_TC6->f1;_TC3=_TA.zero_term;_TB=_TC6->f1;_TC2=_TB.zt_loc;}if(initializer==0)goto _TLA;{void*telt=_TC5;struct Cyc_Absyn_Tqual tq=_TC4;void*zt=_TC3;unsigned ztl=_TC2;_TC=initializer;{
# 76
void*_TC6=_TC->r;struct Cyc_List_List*_TC7;struct Cyc_Absyn_Exp*_TC8;struct _fat_ptr _TC9;_TD=(int*)_TC6;_TE=*_TD;switch(_TE){case 0: _TF=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TC6;_T10=_TF->f1;_T11=_T10.Wstring_c;_T12=_T11.tag;switch(_T12){case 8:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_TCA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TC6;_T13=_TCA->f1;_T14=_T13.String_c;_TC9=_T14.val;}{struct _fat_ptr s=_TC9;_T15=vd;_T16=telt;_T17=tq;_T18=s;_T19=
# 78
_get_fat_size(_T18,sizeof(char));_T1A=Cyc_Absyn_uint_exp(_T19,0U);_T1B=zt;_T1C=ztl;_T15->type=Cyc_Absyn_array_type(_T16,_T17,_T1A,_T1B,_T1C);t=_T15->type;goto _LL8;}case 9:{struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*_TCA=(struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TC6;_T1D=_TCA->f1;_T1E=_T1D.Wstring_c;_TC9=_T1E.val;}{struct _fat_ptr s=_TC9;_T1F=vd;_T20=telt;_T21=tq;_T22=
# 80
Cyc_Absyn_uint_exp(1U,0U);_T23=zt;_T24=ztl;_T1F->type=Cyc_Absyn_array_type(_T20,_T21,_T22,_T23,_T24);t=_T1F->type;goto _LL8;}default: goto _LL15;};case 26:{struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_TCA=(struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TC6;_TC8=_TCA->f2;}{struct Cyc_Absyn_Exp*e=_TC8;_TC8=e;goto _LL10;}case 27:{struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_TCA=(struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_TC6;_TC8=_TCA->f1;}_LL10: {struct Cyc_Absyn_Exp*e=_TC8;_T25=vd;
# 83
_T25->type=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);t=_T25->type;goto _LL8;}case 35:{struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_TCA=(struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_TC6;_TC7=_TCA->f2;}{struct Cyc_List_List*es=_TC7;_TC7=es;goto _LL14;}case 25:{struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_TCA=(struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_TC6;_TC7=_TCA->f1;}_LL14: {struct Cyc_List_List*es=_TC7;_T26=vd;_T27=telt;_T28=tq;_T29=
# 86
Cyc_List_length(es);_T2A=(unsigned)_T29;_T2B=Cyc_Absyn_uint_exp(_T2A,0U);_T2C=zt;_T2D=ztl;_T26->type=Cyc_Absyn_array_type(_T27,_T28,_T2B,_T2C,_T2D);t=_T26->type;goto _LL8;}default: _LL15: goto _LL8;}_LL8:;}goto _LL3;}_TLA: goto _LL6;_TL8: goto _LL6;_TL6: _LL6: goto _LL3;_LL3:;}
# 94
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);_T2E=vd;_T2F=loc;_T30=vd;_T31=_T30->tq;_T32=_T31.print_const;_T33=t;
# 96
_T2E->tq.real_const=Cyc_Tcutil_extract_const_from_typedef(_T2F,_T32,_T33);_T34=vd;_T35=
# 99
Cyc_Tcutil_is_array_type(t);_T34->escapes=!_T35;_T36=
# 101
Cyc_Tcutil_is_function_type(t);if(!_T36)goto _TLE;_T37=is_proto;
*_T37=0;
atts=Cyc_Atts_transfer_fn_type_atts(t,atts);
Cyc_Atts_fnTypeAttsOK(loc,t);goto _TLF;_TLE: _TLF: _T38=sc;_T39=(int)_T38;
# 107
if(_T39==3)goto _TL12;else{goto _TL13;}_TL13: _T3A=sc;_T3B=(int)_T3A;if(_T3B==4)goto _TL12;else{goto _TL10;}
_TL12: if(initializer==0)goto _TL14;_T3C=in_cinclude;if(_T3C)goto _TL14;else{goto _TL16;}
_TL16:{struct Cyc_Warn_String_Warn_Warg_struct _TC1;_TC1.tag=0;_TC1.f1=_tag_fat("extern declaration should not have initializer",sizeof(char),47U);_T3D=_TC1;}{struct Cyc_Warn_String_Warn_Warg_struct _TC1=_T3D;void*_TC2[1];_TC2[0]=& _TC1;_T3E=loc;_T3F=_tag_fat(_TC2,sizeof(void*),1);Cyc_Warn_err2(_T3E,_T3F);}goto _TL15;_TL14: _TL15: goto _TL11;
_TL10: _T40=Cyc_Tcutil_is_function_type(t);if(_T40)goto _TL17;else{goto _TL19;}
# 114
_TL19: Cyc_Atts_check_variable_atts(loc,vd,atts);
if(initializer==0)goto _TL1C;else{goto _TL1D;}_TL1D: _T41=in_cinclude;if(_T41)goto _TL1C;else{goto _TL1A;}
_TL1C: _T42=check_var_init;if(!_T42)goto _TL1E;_T43=in_cinclude;if(_T43)goto _TL1E;else{goto _TL20;}_TL20: _T44=Cyc_Tcutil_zeroable_type(t);if(_T44)goto _TL1E;else{goto _TL21;}
_TL21:{struct Cyc_Warn_String_Warn_Warg_struct _TC1;_TC1.tag=0;_TC1.f1=_tag_fat("initializer required for variable ",sizeof(char),35U);_T45=_TC1;}{struct Cyc_Warn_String_Warn_Warg_struct _TC1=_T45;{struct Cyc_Warn_Vardecl_Warn_Warg_struct _TC2;_TC2.tag=11;_TC2.f1=vd;_T46=_TC2;}{struct Cyc_Warn_Vardecl_Warn_Warg_struct _TC2=_T46;{struct Cyc_Warn_String_Warn_Warg_struct _TC3;_TC3.tag=0;_TC3.f1=_tag_fat(" of type ",sizeof(char),10U);_T47=_TC3;}{struct Cyc_Warn_String_Warn_Warg_struct _TC3=_T47;{struct Cyc_Warn_Typ_Warn_Warg_struct _TC4;_TC4.tag=2;_TC4.f1=t;_T48=_TC4;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TC4=_T48;void*_TC5[4];_TC5[0]=& _TC1;_TC5[1]=& _TC2;_TC5[2]=& _TC3;_TC5[3]=& _TC4;_T49=loc;_T4A=_tag_fat(_TC5,sizeof(void*),4);Cyc_Warn_err2(_T49,_T4A);}}}}goto _TL1F;_TL1E: _TL1F: goto _TL1B;
# 122
_TL1A:{struct _handler_cons _TC1;_T4B=& _TC1;_push_handler(_T4B);{int _TC2=0;_T4C=_TC1.handler;_T4D=setjmp(_T4C);if(!_T4D)goto _TL22;_TC2=1;goto _TL23;_TL22: _TL23: if(_TC2)goto _TL24;else{goto _TL26;}_TL26: _T4E=te;_T4F=& t;_T50=initializer;{
void*t2=Cyc_Tcexp_tcExpInitializer(_T4E,_T4F,_T50);_T51=
Cyc_Tcenv_curr_aquals_bounds(te);_T52=initializer;_T53=t;_T54=Cyc_Tcutil_coerce_assign(_T51,_T52,_T53);if(_T54)goto _TL27;else{goto _TL29;}
_TL29: _T55=vd;_T56=_T55->name;{struct _fat_ptr s0=Cyc_Absynpp_qvar2string(_T56);
const char*s1=" declared with type ";
struct _fat_ptr s2=Cyc_Absynpp_typ2string(t);
const char*s3=" but initialized with type ";
struct _fat_ptr s4=Cyc_Absynpp_typ2string(t2);_T57=
Cyc_strlen(s0);_T58=s1;_T59=_tag_fat(_T58,sizeof(char),21U);_T5A=Cyc_strlen(_T59);_T5B=_T57 + _T5A;_T5C=Cyc_strlen(s2);_T5D=_T5B + _T5C;_T5E=s3;_T5F=_tag_fat(_T5E,sizeof(char),28U);_T60=Cyc_strlen(_T5F);_T61=_T5D + _T60;_T62=Cyc_strlen(s4);_T63=_T61 + _T62;if(_T63 <= 70U)goto _TL2A;{struct Cyc_String_pa_PrintArg_struct _TC3;_TC3.tag=0;
_TC3.f1=s0;_T64=_TC3;}{struct Cyc_String_pa_PrintArg_struct _TC3=_T64;{struct Cyc_String_pa_PrintArg_struct _TC4;_TC4.tag=0;_T66=s1;_TC4.f1=_tag_fat(_T66,sizeof(char),21U);_T65=_TC4;}{struct Cyc_String_pa_PrintArg_struct _TC4=_T65;{struct Cyc_String_pa_PrintArg_struct _TC5;_TC5.tag=0;_TC5.f1=s2;_T67=_TC5;}{struct Cyc_String_pa_PrintArg_struct _TC5=_T67;{struct Cyc_String_pa_PrintArg_struct _TC6;_TC6.tag=0;_T69=s3;_TC6.f1=_tag_fat(_T69,sizeof(char),28U);_T68=_TC6;}{struct Cyc_String_pa_PrintArg_struct _TC6=_T68;{struct Cyc_String_pa_PrintArg_struct _TC7;_TC7.tag=0;_TC7.f1=s4;_T6A=_TC7;}{struct Cyc_String_pa_PrintArg_struct _TC7=_T6A;void*_TC8[5];_TC8[0]=& _TC3;_TC8[1]=& _TC4;_TC8[2]=& _TC5;_TC8[3]=& _TC6;_TC8[4]=& _TC7;_T6B=loc;_T6C=_tag_fat("%s%s\n\t%s\n%s\n\t%s",sizeof(char),16U);_T6D=_tag_fat(_TC8,sizeof(void*),5);Cyc_Warn_err(_T6B,_T6C,_T6D);}}}}}goto _TL2B;
# 133
_TL2A:{struct Cyc_String_pa_PrintArg_struct _TC3;_TC3.tag=0;_TC3.f1=s0;_T6E=_TC3;}{struct Cyc_String_pa_PrintArg_struct _TC3=_T6E;{struct Cyc_String_pa_PrintArg_struct _TC4;_TC4.tag=0;_T70=s1;_TC4.f1=_tag_fat(_T70,sizeof(char),21U);_T6F=_TC4;}{struct Cyc_String_pa_PrintArg_struct _TC4=_T6F;{struct Cyc_String_pa_PrintArg_struct _TC5;_TC5.tag=0;_TC5.f1=s2;_T71=_TC5;}{struct Cyc_String_pa_PrintArg_struct _TC5=_T71;{struct Cyc_String_pa_PrintArg_struct _TC6;_TC6.tag=0;_T73=s3;_TC6.f1=_tag_fat(_T73,sizeof(char),28U);_T72=_TC6;}{struct Cyc_String_pa_PrintArg_struct _TC6=_T72;{struct Cyc_String_pa_PrintArg_struct _TC7;_TC7.tag=0;_TC7.f1=s4;_T74=_TC7;}{struct Cyc_String_pa_PrintArg_struct _TC7=_T74;void*_TC8[5];_TC8[0]=& _TC3;_TC8[1]=& _TC4;_TC8[2]=& _TC5;_TC8[3]=& _TC6;_TC8[4]=& _TC7;_T75=loc;_T76=_tag_fat("%s%s%s%s%s",sizeof(char),11U);_T77=_tag_fat(_TC8,sizeof(void*),5);Cyc_Warn_err(_T75,_T76,_T77);}}}}}_TL2B:
 Cyc_Unify_explain_failure();}goto _TL28;_TL27: _TL28: _T78=
# 137
Cyc_Tcutil_is_const_exp(initializer);if(_T78)goto _TL2C;else{goto _TL2E;}
_TL2E:{struct Cyc_Warn_String_Warn_Warg_struct _TC3;_TC3.tag=0;_TC3.f1=_tag_fat("initializer is not a constant expression",sizeof(char),41U);_T79=_TC3;}{struct Cyc_Warn_String_Warn_Warg_struct _TC3=_T79;void*_TC4[1];_TC4[0]=& _TC3;_T7A=loc;_T7B=_tag_fat(_TC4,sizeof(void*),1);Cyc_Warn_err2(_T7A,_T7B);}goto _TL2D;_TL2C: _TL2D:;}_pop_handler();goto _TL25;_TL24: _T7C=Cyc_Core_get_exn_thrown();{void*_TC3=(void*)_T7C;void*_TC4;_T7D=(struct Cyc_Tcenv_Env_error_exn_struct*)_TC3;_T7E=_T7D->tag;_T7F=Cyc_Tcenv_Env_error;if(_T7E!=_T7F)goto _TL2F;{struct Cyc_Warn_String_Warn_Warg_struct _TC5;_TC5.tag=0;
# 140
_TC5.f1=_tag_fat("initializer is not a constant expression",sizeof(char),41U);_T80=_TC5;}{struct Cyc_Warn_String_Warn_Warg_struct _TC5=_T80;void*_TC6[1];_TC6[0]=& _TC5;_T81=loc;_T82=_tag_fat(_TC6,sizeof(void*),1);Cyc_Warn_err2(_T81,_T82);}goto _LL17;_TL2F: _TC4=_TC3;{void*exn=_TC4;_rethrow(exn);}_LL17:;}_TL25:;}}_TL1B: goto _TL18;
# 144
_TL17: Cyc_Atts_check_fndecl_atts(loc,atts,0);_TL18: _TL11:{struct _handler_cons _TC1;_T83=& _TC1;_push_handler(_T83);{int _TC2=0;_T84=_TC1.handler;_T85=setjmp(_T84);if(!_T85)goto _TL31;_TC2=1;goto _TL32;_TL31: _TL32: if(_TC2)goto _TL33;else{goto _TL35;}_TL35: _T87=Cyc_Dict_lookup;{
# 148
struct _tuple15*(*_TC3)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T87;_T86=_TC3;}_T88=te;_T89=_T88->ae;_T8A=_T89->ordinaries;_T8B=q;{struct _tuple15*ans=_T86(_T8A,_T8B);_T8C=ans;_T8D=*_T8C;{
void*b0=_T8D.f0;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*b1;b1=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_T8E=b1;_T8E->tag=1;_T8F=b1;_T8F->f1=vd;_T90=b0;_T91=b1;_T92=(void*)_T91;_T93=loc;_T94=Cyc_Tc_tc_msg;{
void*b=Cyc_Tcdecl_merge_binding(_T90,_T92,_T93,_T94);
if(b!=0)goto _TL36;_npop_handler(0);return;_TL36:
# 154
 if(exports==0)goto _TL3A;else{goto _TL3B;}_TL3B: _T95=vd;_T96=_T95->name;_T97=exports;_T98=*_T97;_T99=Cyc_Tc_export_member(_T96,_T98);if(_T99)goto _TL3A;else{goto _TL38;}
_TL3A: if(b!=b0)goto _TL3E;else{goto _TL3F;}_TL3F: _T9A=ans;_T9B=*_T9A;_T9C=_T9B.f1;if(_T9C)goto _TL3E;else{goto _TL3C;}
# 157
_TL3E: _T9D=te;_T9E=_T9D->ae;_TA0=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_TC3)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*))_TA0;_T9F=_TC3;}_TA1=te;_TA2=_TA1->ae;_TA3=_TA2->ordinaries;_TA4=q;{struct _tuple15*_TC3=_cycalloc(sizeof(struct _tuple15));
_TC3->f0=b;_TA6=ans;_TA7=*_TA6;_TC3->f1=_TA7.f1;_TA5=(struct _tuple15*)_TC3;}
# 157
_T9E->ordinaries=_T9F(_TA3,_TA4,_TA5);goto _TL3D;_TL3C: _TL3D: goto _TL39;_TL38: _TL39: _npop_handler(0);return;}}}_pop_handler();goto _TL34;_TL33: _TA8=Cyc_Core_get_exn_thrown();{void*_TC3=(void*)_TA8;void*_TC4;_TA9=(struct Cyc_Dict_Absent_exn_struct*)_TC3;_TAA=_TA9->tag;_TAB=Cyc_Dict_Absent;if(_TAA!=_TAB)goto _TL40;goto _LL1C;_TL40: _TC4=_TC3;{void*exn=_TC4;_rethrow(exn);}_LL1C:;}_TL34:;}}
# 161
if(exports==0)goto _TL44;else{goto _TL45;}_TL45: _TAC=vd;_TAD=_TAC->name;_TAE=exports;_TAF=*_TAE;_TB0=Cyc_Tc_export_member(_TAD,_TAF);if(_TB0)goto _TL44;else{goto _TL42;}
_TL44: _TB1=te;_TB2=_TB1->ae;_TB4=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_TC1)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*))_TB4;_TB3=_TC1;}_TB5=te;_TB6=_TB5->ae;_TB7=_TB6->ordinaries;_TB8=q;{struct _tuple15*_TC1=_cycalloc(sizeof(struct _tuple15));{struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_TC2=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_TC2->tag=1;
_TC2->f1=vd;_TBA=(struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_TC2;}_TC1->f0=(void*)_TBA;_TC1->f1=0;_TB9=(struct _tuple15*)_TC1;}
# 162
_TB2->ordinaries=_TB3(_TB7,_TB8,_TB9);goto _TL43;_TL42: _TL43:;}}
# 166
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){struct Cyc_Absyn_Fndecl*_T0;struct Cyc_Absyn_Fndecl*_T1;enum Cyc_Absyn_Scope _T2;int _T3;struct Cyc_Tcenv_Tenv*_T4;int _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;unsigned _T7;struct _fat_ptr _T8;unsigned _T9;struct Cyc_Absyn_Fndecl*_TA;struct Cyc_Absyn_FnInfo _TB;struct Cyc_List_List*_TC;struct Cyc_Tcenv_Tenv*_TD;int _TE;struct Cyc_Absyn_Fndecl*_TF;int(*_T10)(void*,void*);struct Cyc_Absyn_Fndecl*_T11;struct Cyc_Absyn_FnInfo _T12;struct Cyc_List_List*_T13;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_T14;struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*_T15;void*_T16;struct Cyc_Absyn_Fndecl*_T17;void*_T18;struct Cyc_Absyn_Fndecl*_T19;struct Cyc_Absyn_FnInfo _T1A;struct Cyc_List_List*_T1B;struct _handler_cons*_T1C;int*_T1D;int _T1E;struct _tuple15*(*_T1F)(struct Cyc_Dict_Dict,struct _tuple1*);void*(*_T20)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T21;struct Cyc_Tcenv_Genv*_T22;struct Cyc_Dict_Dict _T23;struct _tuple1*_T24;struct _tuple15*_T25;struct _tuple15 _T26;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T27;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T28;void*_T29;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T2A;void*_T2B;unsigned _T2C;struct _fat_ptr*_T2D;struct _tuple1*_T2E;struct Cyc_List_List**_T2F;struct Cyc_List_List*_T30;int _T31;struct _tuple15*_T32;struct _tuple15 _T33;int _T34;struct Cyc_Tcenv_Tenv*_T35;struct Cyc_Tcenv_Genv*_T36;struct Cyc_Dict_Dict(*_T37)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*);struct Cyc_Dict_Dict(*_T38)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_T39;struct Cyc_Tcenv_Genv*_T3A;struct Cyc_Dict_Dict _T3B;struct _tuple1*_T3C;struct _tuple15*_T3D;struct _tuple15*_T3E;struct _tuple15 _T3F;void*_T40;struct Cyc_Dict_Absent_exn_struct*_T41;char*_T42;char*_T43;struct Cyc_Absyn_Fndecl*_T44;struct _tuple1*_T45;struct Cyc_List_List**_T46;struct Cyc_List_List*_T47;int _T48;struct Cyc_Tcenv_Tenv*_T49;struct Cyc_Tcenv_Genv*_T4A;struct Cyc_Dict_Dict(*_T4B)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*);struct Cyc_Dict_Dict(*_T4C)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_T4D;struct Cyc_Tcenv_Genv*_T4E;struct Cyc_Dict_Dict _T4F;struct _tuple1*_T50;struct _tuple15*_T51;struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T52;struct Cyc_Tcenv_Tenv*_T53;int _T54;struct Cyc_Tcenv_Tenv*_T55;struct Cyc_Tcenv_Tenv*_T56;struct Cyc_Tcenv_Tenv*_T57;struct Cyc_Tcenv_Tenv*_T58;struct Cyc_Tcenv_Tenv*_T59;struct Cyc_Absyn_Fndecl*_T5A;struct Cyc_Absyn_Stmt*_T5B;struct Cyc_Tcenv_Tenv*_T5C;int _T5D;struct Cyc_Tcenv_Tenv*_T5E;struct Cyc_Tcenv_Tenv*_T5F;int _T60;struct Cyc_Absyn_Fndecl*_T61;struct Cyc_Absyn_Stmt*_T62;int _T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;struct Cyc_Warn_Qvar_Warn_Warg_struct _T65;struct Cyc_Absyn_Fndecl*_T66;struct Cyc_Warn_String_Warn_Warg_struct _T67;unsigned _T68;struct _fat_ptr _T69;int _T6A;struct Cyc_Absyn_Fndecl*_T6B;struct Cyc_Absyn_FnInfo _T6C;struct Cyc_Absyn_Exp*_T6D;struct Cyc_Warn_String_Warn_Warg_struct _T6E;unsigned _T6F;struct _fat_ptr _T70;struct Cyc_Absyn_Fndecl*_T71;struct Cyc_Absyn_FnInfo _T72;struct Cyc_Absyn_Exp*_T73;struct Cyc_Warn_String_Warn_Warg_struct _T74;unsigned _T75;struct _fat_ptr _T76;struct Cyc_Absyn_Fndecl*_T77;struct Cyc_Absyn_FnInfo _T78;void*_T79;int*_T7A;int _T7B;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T7C;void*_T7D;int*_T7E;unsigned _T7F;struct Cyc_Warn_String_Warn_Warg_struct _T80;unsigned _T81;struct _fat_ptr _T82;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T83;void*_T84;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_T85;enum Cyc_Absyn_Size_of _T86;int _T87;struct Cyc_Warn_String_Warn_Warg_struct _T88;struct Cyc_Warn_Typ_Warn_Warg_struct _T89;struct Cyc_Absyn_Fndecl*_T8A;struct Cyc_Absyn_FnInfo _T8B;struct Cyc_Warn_String_Warn_Warg_struct _T8C;unsigned _T8D;struct _fat_ptr _T8E;struct Cyc_Absyn_Fndecl*_T8F;struct Cyc_Absyn_FnInfo _T90;int _T91;struct Cyc_Absyn_Fndecl*_T92;struct Cyc_Absyn_FnInfo _T93;struct Cyc_Absyn_VarargInfo*_T94;struct Cyc_Warn_String_Warn_Warg_struct _T95;unsigned _T96;struct _fat_ptr _T97;struct Cyc_Absyn_Fndecl*_T98;struct Cyc_Absyn_FnInfo _T99;struct Cyc_List_List*_T9A;void*_T9B;int*_T9C;int _T9D;struct Cyc_Absyn_AppType_Absyn_Type_struct*_T9E;void*_T9F;int*_TA0;int _TA1;struct Cyc_Absyn_AppType_Absyn_Type_struct*_TA2;void*_TA3;struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*_TA4;enum Cyc_Absyn_Size_of _TA5;int _TA6;struct Cyc_Warn_String_Warn_Warg_struct _TA7;struct Cyc_Warn_Typ_Warn_Warg_struct _TA8;struct Cyc_Warn_String_Warn_Warg_struct _TA9;unsigned _TAA;struct _fat_ptr _TAB;struct Cyc_List_List*_TAC;struct Cyc_List_List*_TAD;void*_TAE;struct Cyc_List_List*_TAF;struct Cyc_Warn_String_Warn_Warg_struct _TB0;unsigned _TB1;struct _fat_ptr _TB2;struct Cyc_Core_Opt*_TB3;struct Cyc_Absyn_Fndecl*_TB4;struct Cyc_Absyn_FnInfo _TB5;void*_TB6;struct Cyc_Core_Opt*_TB7;struct Cyc_Absyn_Kind*_TB8;struct Cyc_Core_Opt*_TB9;void*_TBA;void*_TBB;void*_TBC;struct Cyc_Core_Opt*_TBD;struct Cyc_Absyn_Kind*_TBE;struct Cyc_Core_Opt*_TBF;void*_TC0;void*_TC1;struct Cyc_Absyn_Tqual _TC2;struct Cyc_Core_Opt*_TC3;void*_TC4;struct Cyc_List_List*_TC5;void*_TC6;void*_TC7;void*_TC8;int _TC9;void*_TCA;struct Cyc_Core_Opt*_TCB;struct Cyc_Absyn_Kind*_TCC;struct Cyc_Core_Opt*_TCD;void*_TCE;void*_TCF;void*_TD0;struct Cyc_Core_Opt*_TD1;struct Cyc_Absyn_Kind*_TD2;struct Cyc_Core_Opt*_TD3;void*_TD4;void*_TD5;struct Cyc_Absyn_Tqual _TD6;struct Cyc_Core_Opt*_TD7;void*_TD8;struct Cyc_List_List*_TD9;void*_TDA;void*_TDB;void*_TDC;int _TDD;void*_TDE;struct Cyc_Core_Opt*_TDF;struct Cyc_Absyn_Kind*_TE0;struct Cyc_Core_Opt*_TE1;void*_TE2;void*_TE3;void*_TE4;struct Cyc_Core_Opt*_TE5;struct Cyc_Absyn_Kind*_TE6;struct Cyc_Core_Opt*_TE7;void*_TE8;void*_TE9;struct Cyc_Absyn_Tqual _TEA;struct Cyc_Core_Opt*_TEB;void*_TEC;struct Cyc_List_List*_TED;void*_TEE;void*_TEF;void*_TF0;int _TF1;void*_TF2;struct Cyc_Core_Opt*_TF3;struct Cyc_Absyn_Kind*_TF4;struct Cyc_Core_Opt*_TF5;void*_TF6;void*_TF7;void*_TF8;struct Cyc_Core_Opt*_TF9;struct Cyc_Absyn_Kind*_TFA;struct Cyc_Core_Opt*_TFB;void*_TFC;void*_TFD;struct Cyc_Absyn_Tqual _TFE;struct Cyc_Core_Opt*_TFF;void*_T100;struct Cyc_List_List*_T101;void*_T102;void*_T103;void*_T104;int _T105;struct Cyc_Warn_String_Warn_Warg_struct _T106;struct Cyc_Warn_Typ_Warn_Warg_struct _T107;struct Cyc_Warn_String_Warn_Warg_struct _T108;unsigned _T109;struct _fat_ptr _T10A;_T0=fd;{
# 168
struct _tuple1*q=_T0->name;_T1=fd;_T2=_T1->sc;_T3=(int)_T2;
# 172
if(_T3!=4)goto _TL46;_T4=te;_T5=_T4->in_extern_c_include;if(_T5)goto _TL46;else{goto _TL48;}
_TL48:{struct Cyc_Warn_String_Warn_Warg_struct _T10B;_T10B.tag=0;_T10B.f1=_tag_fat("extern \"C\" functions cannot be implemented in Cyclone",sizeof(char),54U);_T6=_T10B;}{struct Cyc_Warn_String_Warn_Warg_struct _T10B=_T6;void*_T10C[1];_T10C[0]=& _T10B;_T7=loc;_T8=_tag_fat(_T10C,sizeof(void*),1);Cyc_Warn_err2(_T7,_T8);}goto _TL47;_TL46: _TL47: _T9=loc;_TA=fd;_TB=_TA->i;_TC=_TB.attributes;
# 176
Cyc_Atts_check_fndecl_atts(_T9,_TC,1);_TD=te;_TE=_TD->in_extern_c_inc_repeat;
# 179
if(!_TE)goto _TL49;_TF=fd;
_TF->cached_type=0;goto _TL4A;_TL49: _TL4A:
 Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);_T10=Cyc_Atts_attribute_cmp;_T11=fd;_T12=_T11->i;_T13=_T12.attributes;_T14=& Cyc_Atts_No_throw_att_val;_T15=(struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct*)_T14;_T16=(void*)_T15;{
# 184
int nothrow=Cyc_List_mem(_T10,_T13,_T16);_T17=fd;_T18=t;_T19=fd;_T1A=_T19->i;_T1B=_T1A.attributes;
# 187
_T17->i.attributes=Cyc_Atts_transfer_fn_type_atts(_T18,_T1B);
Cyc_Atts_fnTypeAttsOK(loc,t);{struct _handler_cons _T10B;_T1C=& _T10B;_push_handler(_T1C);{int _T10C=0;_T1D=_T10B.handler;_T1E=setjmp(_T1D);if(!_T1E)goto _TL4B;_T10C=1;goto _TL4C;_TL4B: _TL4C: if(_T10C)goto _TL4D;else{goto _TL4F;}_TL4F: _T20=Cyc_Dict_lookup;{
# 192
struct _tuple15*(*_T10D)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T20;_T1F=_T10D;}_T21=te;_T22=_T21->ae;_T23=_T22->ordinaries;_T24=q;{struct _tuple15*ans=_T1F(_T23,_T24);_T25=ans;_T26=*_T25;{
void*b0=_T26.f0;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b1;b1=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T27=b1;_T27->tag=2;_T28=b1;_T28->f1=fd;_T29=b0;_T2A=b1;_T2B=(void*)_T2A;_T2C=loc;_T2D=Cyc_Tc_tc_msg;{
void*b=Cyc_Tcdecl_merge_binding(_T29,_T2B,_T2C,_T2D);
if(b==0)goto _TL50;
# 198
if(exports==0)goto _TL54;else{goto _TL55;}_TL55: _T2E=q;_T2F=exports;_T30=*_T2F;_T31=Cyc_Tc_export_member(_T2E,_T30);if(_T31)goto _TL54;else{goto _TL52;}
_TL54: if(b==b0)goto _TL59;else{goto _TL58;}_TL59: _T32=ans;_T33=*_T32;_T34=_T33.f1;if(_T34)goto _TL56;else{goto _TL58;}
_TL58: _T35=te;_T36=_T35->ae;_T38=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T10D)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*))_T38;_T37=_T10D;}_T39=te;_T3A=_T39->ae;_T3B=_T3A->ordinaries;_T3C=q;{struct _tuple15*_T10D=_cycalloc(sizeof(struct _tuple15));
_T10D->f0=b;_T3E=ans;_T3F=*_T3E;_T10D->f1=_T3F.f1;_T3D=(struct _tuple15*)_T10D;}
# 200
_T36->ordinaries=_T37(_T3B,_T3C,_T3D);goto _TL57;_TL56: _TL57: goto _TL53;_TL52: _TL53: goto _TL51;_TL50: _TL51:;}}}_pop_handler();goto _TL4E;_TL4D: _T40=Cyc_Core_get_exn_thrown();{void*_T10D=(void*)_T40;void*_T10E;_T41=(struct Cyc_Dict_Absent_exn_struct*)_T10D;_T42=_T41->tag;_T43=Cyc_Dict_Absent;if(_T42!=_T43)goto _TL5A;
# 203
if(exports==0)goto _TL5E;else{goto _TL5F;}_TL5F: _T44=fd;_T45=_T44->name;_T46=exports;_T47=*_T46;_T48=Cyc_Tc_export_member(_T45,_T47);if(_T48)goto _TL5E;else{goto _TL5C;}
_TL5E: _T49=te;_T4A=_T49->ae;_T4C=Cyc_Dict_insert;{struct Cyc_Dict_Dict(*_T10F)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*))_T4C;_T4B=_T10F;}_T4D=te;_T4E=_T4D->ae;_T4F=_T4E->ordinaries;_T50=q;{struct _tuple15*_T10F=_cycalloc(sizeof(struct _tuple15));{struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_T110=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_T110->tag=2;
_T110->f1=fd;_T52=(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*)_T110;}_T10F->f0=(void*)_T52;_T10F->f1=0;_T51=(struct _tuple15*)_T10F;}
# 204
_T4A->ordinaries=_T4B(_T4F,_T50,_T51);goto _TL5D;_TL5C: _TL5D: goto _LL0;_TL5A: _T10E=_T10D;{void*exn=_T10E;_rethrow(exn);}_LL0:;}_TL4E:;}}_T53=te;_T54=_T53->in_extern_c_include;
# 210
if(!_T54)goto _TL60;return;_TL60: {
# 215
struct Cyc_Tcenv_Fenv*fenv=Cyc_Tcenv_new_fenv(loc,fd);
struct Cyc_Tcenv_Tenv*_T10B;_T10B=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv));_T55=te;_T10B->ns=_T55->ns;_T56=te;_T10B->ae=_T56->ae;_T10B->le=fenv;_T10B->allow_valueof=0;_T10B->in_extern_c_include=0;_T57=te;_T10B->in_tempest=_T57->in_tempest;_T58=te;_T10B->tempest_generalize=_T58->tempest_generalize;_T10B->in_extern_c_inc_repeat=0;{struct Cyc_Tcenv_Tenv*te=_T10B;_T59=te;_T5A=fd;_T5B=_T5A->body;
# 218
Cyc_Tcstmt_tcStmt(_T59,_T5B,0);
# 221
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);_T5C=te;_T5D=_T5C->in_tempest;
# 224
if(!_T5D)goto _TL62;_T5E=te;
_T5E->tempest_generalize=1;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);_T5F=te;
_T5F->tempest_generalize=0;goto _TL63;_TL62: _TL63: _T60=nothrow;
# 229
if(!_T60)goto _TL64;_T61=fd;_T62=_T61->body;_T63=Cyc_Tcstmt_ensure_no_throw_stmt(_T62);if(_T63)goto _TL64;else{goto _TL66;}
_TL66:{struct Cyc_Warn_String_Warn_Warg_struct _T10C;_T10C.tag=0;_T10C.f1=_tag_fat("Function ",sizeof(char),10U);_T64=_T10C;}{struct Cyc_Warn_String_Warn_Warg_struct _T10C=_T64;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T10D;_T10D.tag=1;_T66=fd;_T10D.f1=_T66->name;_T65=_T10D;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T10D=_T65;{struct Cyc_Warn_String_Warn_Warg_struct _T10E;_T10E.tag=0;_T10E.f1=_tag_fat(" has attribute no_throw but may throw an exception",sizeof(char),51U);_T67=_T10E;}{struct Cyc_Warn_String_Warn_Warg_struct _T10E=_T67;void*_T10F[3];_T10F[0]=& _T10C;_T10F[1]=& _T10D;_T10F[2]=& _T10E;_T68=loc;_T69=_tag_fat(_T10F,sizeof(void*),3);Cyc_Warn_err2(_T68,_T69);}}}goto _TL65;_TL64: _TL65: _T6A=
# 232
Cyc_Tcutil_is_main(q);if(!_T6A)goto _TL67;_T6B=fd;_T6C=_T6B->i;_T6D=_T6C.checks_clause;
# 234
if(_T6D==0)goto _TL69;{struct Cyc_Warn_String_Warn_Warg_struct _T10C;_T10C.tag=0;
_T10C.f1=_tag_fat("main declared with a @check clause",sizeof(char),35U);_T6E=_T10C;}{struct Cyc_Warn_String_Warn_Warg_struct _T10C=_T6E;void*_T10D[1];_T10D[0]=& _T10C;_T6F=loc;_T70=_tag_fat(_T10D,sizeof(void*),1);Cyc_Warn_err2(_T6F,_T70);}goto _TL6A;_TL69: _TL6A: _T71=fd;_T72=_T71->i;_T73=_T72.requires_clause;
if(_T73==0)goto _TL6B;{struct Cyc_Warn_String_Warn_Warg_struct _T10C;_T10C.tag=0;
_T10C.f1=_tag_fat("main declared with a @requires clause",sizeof(char),38U);_T74=_T10C;}{struct Cyc_Warn_String_Warn_Warg_struct _T10C=_T74;void*_T10D[1];_T10D[0]=& _T10C;_T75=loc;_T76=_tag_fat(_T10D,sizeof(void*),1);Cyc_Warn_err2(_T75,_T76);}goto _TL6C;_TL6B: _TL6C: _T77=fd;_T78=_T77->i;_T79=_T78.ret_type;{
# 239
void*_T10C=Cyc_Absyn_compress(_T79);_T7A=(int*)_T10C;_T7B=*_T7A;if(_T7B!=0)goto _TL6D;_T7C=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10C;_T7D=_T7C->f1;_T7E=(int*)_T7D;_T7F=*_T7E;switch(_T7F){case 0:{struct Cyc_Warn_String_Warn_Warg_struct _T10D;_T10D.tag=0;
# 241
_T10D.f1=_tag_fat("main declared with return type void",sizeof(char),36U);_T80=_T10D;}{struct Cyc_Warn_String_Warn_Warg_struct _T10D=_T80;void*_T10E[1];_T10E[0]=& _T10D;_T81=loc;_T82=_tag_fat(_T10E,sizeof(void*),1);Cyc_Warn_warn2(_T81,_T82);}goto _LL5;case 1: _T83=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10C;_T84=_T83->f1;_T85=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_T84;_T86=_T85->f2;_T87=(int)_T86;switch(_T87){case Cyc_Absyn_Int_sz: goto _LLB;case Cyc_Absyn_Long_sz: _LLB: goto _LL5;default: goto _LLC;};default: goto _LLC;}goto _TL6E;_TL6D: _LLC:{struct Cyc_Warn_String_Warn_Warg_struct _T10D;_T10D.tag=0;
# 244
_T10D.f1=_tag_fat("main declared with return type ",sizeof(char),32U);_T88=_T10D;}{struct Cyc_Warn_String_Warn_Warg_struct _T10D=_T88;{struct Cyc_Warn_Typ_Warn_Warg_struct _T10E;_T10E.tag=2;_T8A=fd;_T8B=_T8A->i;_T10E.f1=_T8B.ret_type;_T89=_T10E;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T10E=_T89;{struct Cyc_Warn_String_Warn_Warg_struct _T10F;_T10F.tag=0;
_T10F.f1=_tag_fat(" instead of int or void",sizeof(char),24U);_T8C=_T10F;}{struct Cyc_Warn_String_Warn_Warg_struct _T10F=_T8C;void*_T110[3];_T110[0]=& _T10D;_T110[1]=& _T10E;_T110[2]=& _T10F;_T8D=loc;_T8E=_tag_fat(_T110,sizeof(void*),3);Cyc_Warn_err2(_T8D,_T8E);}}}_TL6E: _LL5:;}_T8F=fd;_T90=_T8F->i;_T91=_T90.c_varargs;
# 247
if(_T91)goto _TL73;else{goto _TL74;}_TL74: _T92=fd;_T93=_T92->i;_T94=_T93.cyc_varargs;if(_T94!=0)goto _TL73;else{goto _TL71;}
_TL73:{struct Cyc_Warn_String_Warn_Warg_struct _T10C;_T10C.tag=0;_T10C.f1=_tag_fat("main declared with varargs",sizeof(char),27U);_T95=_T10C;}{struct Cyc_Warn_String_Warn_Warg_struct _T10C=_T95;void*_T10D[1];_T10D[0]=& _T10C;_T96=loc;_T97=_tag_fat(_T10D,sizeof(void*),1);Cyc_Warn_err2(_T96,_T97);}goto _TL72;_TL71: _TL72: _T98=fd;_T99=_T98->i;{
struct Cyc_List_List*args=_T99.args;
if(args==0)goto _TL75;_T9A=args;_T9B=_T9A->hd;{
struct _tuple9*_T10C=(struct _tuple9*)_T9B;void*_T10D;{struct _tuple9 _T10E=*_T10C;_T10D=_T10E.f2;}{void*t1=_T10D;{
void*_T10E=Cyc_Absyn_compress(t1);_T9C=(int*)_T10E;_T9D=*_T9C;if(_T9D!=0)goto _TL77;_T9E=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10E;_T9F=_T9E->f1;_TA0=(int*)_T9F;_TA1=*_TA0;if(_TA1!=1)goto _TL79;_TA2=(struct Cyc_Absyn_AppType_Absyn_Type_struct*)_T10E;_TA3=_TA2->f1;_TA4=(struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_TA3;_TA5=_TA4->f2;_TA6=(int)_TA5;switch(_TA6){case Cyc_Absyn_Int_sz: goto _LL15;case Cyc_Absyn_Long_sz: _LL15: goto _LL11;default: goto _LL16;}goto _TL7A;_TL79: goto _LL16;_TL7A: goto _TL78;_TL77: _LL16:{struct Cyc_Warn_String_Warn_Warg_struct _T10F;_T10F.tag=0;
# 255
_T10F.f1=_tag_fat("main declared with first argument of type ",sizeof(char),43U);_TA7=_T10F;}{struct Cyc_Warn_String_Warn_Warg_struct _T10F=_TA7;{struct Cyc_Warn_Typ_Warn_Warg_struct _T110;_T110.tag=2;_T110.f1=t1;_TA8=_T110;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T110=_TA8;{struct Cyc_Warn_String_Warn_Warg_struct _T111;_T111.tag=0;
_T111.f1=_tag_fat(" instead of int",sizeof(char),16U);_TA9=_T111;}{struct Cyc_Warn_String_Warn_Warg_struct _T111=_TA9;void*_T112[3];_T112[0]=& _T10F;_T112[1]=& _T110;_T112[2]=& _T111;_TAA=loc;_TAB=_tag_fat(_T112,sizeof(void*),3);Cyc_Warn_err2(_TAA,_TAB);}}}_TL78: _LL11:;}_TAC=args;
# 258
args=_TAC->tl;
if(args==0)goto _TL7C;_TAD=args;_TAE=_TAD->hd;{
struct _tuple9*_T10E=(struct _tuple9*)_TAE;void*_T10F;{struct _tuple9 _T110=*_T10E;_T10F=_T110.f2;}{void*t2=_T10F;_TAF=args;
args=_TAF->tl;
if(args==0)goto _TL7E;{struct Cyc_Warn_String_Warn_Warg_struct _T110;_T110.tag=0;
_T110.f1=_tag_fat("main declared with too many arguments",sizeof(char),38U);_TB0=_T110;}{struct Cyc_Warn_String_Warn_Warg_struct _T110=_TB0;void*_T111[1];_T111[0]=& _T110;_TB1=loc;_TB2=_tag_fat(_T111,sizeof(void*),1);Cyc_Warn_err2(_TB1,_TB2);}goto _TL7F;_TL7E: _TL7F: {
struct Cyc_Core_Opt*tvs;tvs=_cycalloc(sizeof(struct Cyc_Core_Opt));_TB3=tvs;_TB4=fd;_TB5=_TB4->i;_TB3->v=_TB5.tvars;_TB6=t2;{struct Cyc_Core_Opt*_T110=_cycalloc(sizeof(struct Cyc_Core_Opt));_TB8=& Cyc_Kinds_ek;
_T110->v=(struct Cyc_Absyn_Kind*)_TB8;_TB7=(struct Cyc_Core_Opt*)_T110;}_TB9=tvs;_TBA=Cyc_Absyn_new_evar(_TB7,_TB9);_TBB=Cyc_Absyn_al_qual_type;_TBC=Cyc_Absyn_string_type(_TBA,_TBB);{struct Cyc_Core_Opt*_T110=_cycalloc(sizeof(struct Cyc_Core_Opt));_TBE=& Cyc_Kinds_ek;
_T110->v=(struct Cyc_Absyn_Kind*)_TBE;_TBD=(struct Cyc_Core_Opt*)_T110;}_TBF=tvs;_TC0=Cyc_Absyn_new_evar(_TBD,_TBF);_TC1=Cyc_Absyn_al_qual_type;_TC2=
Cyc_Absyn_empty_tqual(0U);_TC3=tvs;_TC4=_TC3->v;_TC5=(struct Cyc_List_List*)_TC4;_TC6=Cyc_Tcutil_any_bool(_TC5);_TC7=Cyc_Absyn_false_type;_TC8=
# 265
Cyc_Absyn_fatptr_type(_TBC,_TC0,_TC1,_TC2,_TC6,_TC7);_TC9=Cyc_Unify_unify(_TB6,_TC8);if(_TC9)goto _TL80;else{goto _TL82;}_TL82: _TCA=t2;{struct Cyc_Core_Opt*_T110=_cycalloc(sizeof(struct Cyc_Core_Opt));_TCC=& Cyc_Kinds_ek;
# 268
_T110->v=(struct Cyc_Absyn_Kind*)_TCC;_TCB=(struct Cyc_Core_Opt*)_T110;}_TCD=tvs;_TCE=Cyc_Absyn_new_evar(_TCB,_TCD);_TCF=Cyc_Absyn_al_qual_type;_TD0=Cyc_Absyn_const_string_type(_TCE,_TCF);{struct Cyc_Core_Opt*_T110=_cycalloc(sizeof(struct Cyc_Core_Opt));_TD2=& Cyc_Kinds_ek;
_T110->v=(struct Cyc_Absyn_Kind*)_TD2;_TD1=(struct Cyc_Core_Opt*)_T110;}_TD3=tvs;_TD4=Cyc_Absyn_new_evar(_TD1,_TD3);_TD5=Cyc_Absyn_al_qual_type;_TD6=
Cyc_Absyn_empty_tqual(0U);_TD7=tvs;_TD8=_TD7->v;_TD9=(struct Cyc_List_List*)_TD8;_TDA=Cyc_Tcutil_any_bool(_TD9);_TDB=Cyc_Absyn_false_type;_TDC=
# 268
Cyc_Absyn_fatptr_type(_TD0,_TD4,_TD5,_TD6,_TDA,_TDB);_TDD=Cyc_Unify_unify(_TCA,_TDC);
# 265
if(_TDD)goto _TL80;else{goto _TL83;}_TL83: _TDE=t2;{struct Cyc_Core_Opt*_T110=_cycalloc(sizeof(struct Cyc_Core_Opt));_TE0=& Cyc_Kinds_ek;
# 271
_T110->v=(struct Cyc_Absyn_Kind*)_TE0;_TDF=(struct Cyc_Core_Opt*)_T110;}_TE1=tvs;_TE2=Cyc_Absyn_new_evar(_TDF,_TE1);_TE3=Cyc_Absyn_al_qual_type;_TE4=Cyc_Absyn_string_type(_TE2,_TE3);{struct Cyc_Core_Opt*_T110=_cycalloc(sizeof(struct Cyc_Core_Opt));_TE6=& Cyc_Kinds_ek;
_T110->v=(struct Cyc_Absyn_Kind*)_TE6;_TE5=(struct Cyc_Core_Opt*)_T110;}_TE7=tvs;_TE8=Cyc_Absyn_new_evar(_TE5,_TE7);_TE9=Cyc_Absyn_al_qual_type;_TEA=
Cyc_Absyn_const_tqual(0U);_TEB=tvs;_TEC=_TEB->v;_TED=(struct Cyc_List_List*)_TEC;_TEE=Cyc_Tcutil_any_bool(_TED);_TEF=Cyc_Absyn_false_type;_TF0=
# 271
Cyc_Absyn_fatptr_type(_TE4,_TE8,_TE9,_TEA,_TEE,_TEF);_TF1=Cyc_Unify_unify(_TDE,_TF0);
# 265
if(_TF1)goto _TL80;else{goto _TL84;}_TL84: _TF2=t2;{struct Cyc_Core_Opt*_T110=_cycalloc(sizeof(struct Cyc_Core_Opt));_TF4=& Cyc_Kinds_ek;
# 274
_T110->v=(struct Cyc_Absyn_Kind*)_TF4;_TF3=(struct Cyc_Core_Opt*)_T110;}_TF5=tvs;_TF6=Cyc_Absyn_new_evar(_TF3,_TF5);_TF7=Cyc_Absyn_al_qual_type;_TF8=Cyc_Absyn_const_string_type(_TF6,_TF7);{struct Cyc_Core_Opt*_T110=_cycalloc(sizeof(struct Cyc_Core_Opt));_TFA=& Cyc_Kinds_ek;
_T110->v=(struct Cyc_Absyn_Kind*)_TFA;_TF9=(struct Cyc_Core_Opt*)_T110;}_TFB=tvs;_TFC=Cyc_Absyn_new_evar(_TF9,_TFB);_TFD=Cyc_Absyn_al_qual_type;_TFE=
Cyc_Absyn_const_tqual(0U);_TFF=tvs;_T100=_TFF->v;_T101=(struct Cyc_List_List*)_T100;_T102=Cyc_Tcutil_any_bool(_T101);_T103=Cyc_Absyn_false_type;_T104=
# 274
Cyc_Absyn_fatptr_type(_TF8,_TFC,_TFD,_TFE,_T102,_T103);_T105=Cyc_Unify_unify(_TF2,_T104);
# 265
if(_T105)goto _TL80;else{goto _TL85;}
# 277
_TL85:{struct Cyc_Warn_String_Warn_Warg_struct _T110;_T110.tag=0;_T110.f1=_tag_fat("second argument of main has type ",sizeof(char),34U);_T106=_T110;}{struct Cyc_Warn_String_Warn_Warg_struct _T110=_T106;{struct Cyc_Warn_Typ_Warn_Warg_struct _T111;_T111.tag=2;_T111.f1=t2;_T107=_T111;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T111=_T107;{struct Cyc_Warn_String_Warn_Warg_struct _T112;_T112.tag=0;
_T112.f1=_tag_fat(" instead of char??",sizeof(char),19U);_T108=_T112;}{struct Cyc_Warn_String_Warn_Warg_struct _T112=_T108;void*_T113[3];_T113[0]=& _T110;_T113[1]=& _T111;_T113[2]=& _T112;_T109=loc;_T10A=_tag_fat(_T113,sizeof(void*),3);Cyc_Warn_err2(_T109,_T10A);}}}goto _TL81;_TL80: _TL81:;}}}goto _TL7D;_TL7C: _TL7D:;}}goto _TL76;_TL75: _TL76:;}goto _TL68;_TL67: _TL68:;}}}}}}
# 287
static void Cyc_Tc_tcOneTypedef(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){unsigned _T0;struct Cyc_Absyn_Typedefdecl*_T1;struct Cyc_List_List*_T2;struct Cyc_Absyn_Typedefdecl*_T3;struct Cyc_List_List*_T4;struct Cyc_Absyn_Typedefdecl*_T5;void*_T6;unsigned _T7;struct Cyc_Tcenv_Tenv*_T8;struct Cyc_Absyn_Typedefdecl*_T9;struct Cyc_List_List*_TA;struct Cyc_Absyn_Kind*_TB;struct Cyc_Absyn_Kind*_TC;struct Cyc_Absyn_Typedefdecl*_TD;void*_TE;struct Cyc_Absyn_Typedefdecl*_TF;unsigned _T10;struct Cyc_Absyn_Typedefdecl*_T11;struct Cyc_Absyn_Tqual _T12;int _T13;struct Cyc_Absyn_Typedefdecl*_T14;void*_T15;void*_T16;struct Cyc_Absyn_Typedefdecl*_T17;struct Cyc_List_List*_T18;void*_T19;struct Cyc_Absyn_Tvar*_T1A;void*_T1B;int*_T1C;unsigned _T1D;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T1E;struct Cyc_Core_Opt**_T1F;struct Cyc_Absyn_Typedefdecl*_T20;void*_T21;struct Cyc_Warn_String_Warn_Warg_struct _T22;struct Cyc_Warn_Tvar_Warn_Warg_struct _T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;unsigned _T27;struct _fat_ptr _T28;struct Cyc_Core_Opt**_T29;struct Cyc_Core_Opt*_T2A;struct Cyc_Absyn_Kind*_T2B;struct Cyc_Absyn_Kind*_T2C;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T2D;struct Cyc_Core_Opt**_T2E;struct Cyc_Core_Opt**_T2F;struct Cyc_Core_Opt*_T30;struct Cyc_List_List*_T31;_T0=loc;_T1=td;_T2=_T1->tvs;
Cyc_Tcutil_check_unique_tvars(_T0,_T2);_T3=td;_T4=_T3->tvs;
Cyc_Tcutil_add_tvar_identities(_T4);_T5=td;_T6=_T5->defn;
if(_T6==0)goto _TL86;_T7=loc;_T8=te;_T9=td;_TA=_T9->tvs;_TB=& Cyc_Kinds_ak;_TC=(struct Cyc_Absyn_Kind*)_TB;_TD=td;_TE=_TD->defn;
Cyc_Tctyp_check_type(_T7,_T8,_TA,_TC,0,1,_TE);_TF=td;_T10=loc;_T11=td;_T12=_T11->tq;_T13=_T12.print_const;_T14=td;_T15=_T14->defn;_T16=
# 293
_check_null(_T15);
# 292
_TF->tq.real_const=
Cyc_Tcutil_extract_const_from_typedef(_T10,_T13,_T16);goto _TL87;_TL86: _TL87: _T17=td;{
# 297
struct Cyc_List_List*tvs=_T17->tvs;_TL8B: if(tvs!=0)goto _TL89;else{goto _TL8A;}
_TL89: _T18=tvs;_T19=_T18->hd;_T1A=(struct Cyc_Absyn_Tvar*)_T19;_T1B=_T1A->kind;{void*_T32=Cyc_Kinds_compress_kb(_T1B);struct Cyc_Absyn_Kind*_T33;void*_T34;_T1C=(int*)_T32;_T1D=*_T1C;switch(_T1D){case 1:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T35=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T32;_T1E=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T32;_T1F=& _T1E->f1;_T34=(struct Cyc_Core_Opt**)_T1F;}{struct Cyc_Core_Opt**f=_T34;_T20=td;_T21=_T20->defn;
# 300
if(_T21==0)goto _TL8D;{struct Cyc_Warn_String_Warn_Warg_struct _T35;_T35.tag=0;
_T35.f1=_tag_fat("type variable ",sizeof(char),15U);_T22=_T35;}{struct Cyc_Warn_String_Warn_Warg_struct _T35=_T22;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T36;_T36.tag=7;_T24=tvs;_T25=_T24->hd;_T36.f1=(struct Cyc_Absyn_Tvar*)_T25;_T23=_T36;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T36=_T23;{struct Cyc_Warn_String_Warn_Warg_struct _T37;_T37.tag=0;_T37.f1=_tag_fat(" is not used in typedef",sizeof(char),24U);_T26=_T37;}{struct Cyc_Warn_String_Warn_Warg_struct _T37=_T26;void*_T38[3];_T38[0]=& _T35;_T38[1]=& _T36;_T38[2]=& _T37;_T27=loc;_T28=_tag_fat(_T38,sizeof(void*),3);Cyc_Warn_warn2(_T27,_T28);}}}goto _TL8E;_TL8D: _TL8E: _T29=f;{struct Cyc_Core_Opt*_T35=_cycalloc(sizeof(struct Cyc_Core_Opt));_T2B=& Cyc_Kinds_bk;_T2C=(struct Cyc_Absyn_Kind*)_T2B;
_T35->v=Cyc_Kinds_kind_to_bound(_T2C);_T2A=(struct Cyc_Core_Opt*)_T35;}*_T29=_T2A;goto _LL0;}case 2:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T35=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T32;_T2D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T32;_T2E=& _T2D->f1;_T34=(struct Cyc_Core_Opt**)_T2E;_T33=_T35->f2;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T34;struct Cyc_Absyn_Kind*k=_T33;_T2F=f;{struct Cyc_Core_Opt*_T35=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 304
_T35->v=Cyc_Kinds_kind_to_bound(k);_T30=(struct Cyc_Core_Opt*)_T35;}*_T2F=_T30;goto _LL0;}default: goto _TL88;}_LL0:;}_TL88: _T31=tvs;
# 297
tvs=_T31->tl;goto _TL8B;_TL8A:;}}
# 352 "tc.cyc"
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){struct Cyc_Absyn_Typedefdecl*_T0;struct Cyc_Absyn_Typedefdecl**(*_T1)(struct Cyc_Dict_Dict,struct _tuple1*);void**(*_T2)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T3;struct Cyc_Tcenv_Genv*_T4;struct Cyc_Dict_Dict _T5;struct _tuple1*_T6;struct Cyc_Tcenv_Tenv*_T7;struct Cyc_Tcenv_Genv*_T8;struct Cyc_Dict_Dict(*_T9)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Typedefdecl*);struct Cyc_Dict_Dict(*_TA)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_TB;struct Cyc_Tcenv_Genv*_TC;struct Cyc_Dict_Dict _TD;struct _tuple1*_TE;struct Cyc_Absyn_Typedefdecl*_TF;struct Cyc_Tcenv_Tenv*_T10;struct Cyc_Tcenv_Genv*_T11;struct Cyc_Dict_Dict(*_T12)(struct Cyc_Dict_Dict,struct _tuple1*);struct Cyc_Dict_Dict(*_T13)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T14;struct Cyc_Tcenv_Genv*_T15;struct Cyc_Dict_Dict _T16;struct _tuple1*_T17;struct Cyc_Absyn_Typedefdecl*_T18;struct Cyc_Absyn_Typedefdecl**_T19;struct Cyc_Absyn_Typedefdecl*_T1A;unsigned _T1B;struct _fat_ptr*_T1C;struct _fat_ptr*_T1D;struct Cyc_Absyn_Typedefdecl*_T1E;unsigned _T1F;struct Cyc_Tcenv_Tenv*_T20;struct Cyc_Tcenv_Genv*_T21;struct Cyc_Dict_Dict(*_T22)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Typedefdecl*);struct Cyc_Dict_Dict(*_T23)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_T24;struct Cyc_Tcenv_Genv*_T25;struct Cyc_Dict_Dict _T26;struct _tuple1*_T27;struct Cyc_Absyn_Typedefdecl*_T28;_T0=td;{
struct _tuple1*q=_T0->name;_T2=Cyc_Dict_lookup_opt;{
# 358
struct Cyc_Absyn_Typedefdecl**(*_T29)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct Cyc_Absyn_Typedefdecl**(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T2;_T1=_T29;}_T3=te;_T4=_T3->ae;_T5=_T4->typedefs;_T6=q;{struct Cyc_Absyn_Typedefdecl**_T29=_T1(_T5,_T6);struct Cyc_Absyn_Typedefdecl**_T2A;if(_T29!=0)goto _TL8F;
# 361
Cyc_Tc_tcOneTypedef(te,loc,td);_T7=te;_T8=_T7->ae;_TA=Cyc_Dict_insert;{
# 363
struct Cyc_Dict_Dict(*_T2B)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Typedefdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Typedefdecl*))_TA;_T9=_T2B;}_TB=te;_TC=_TB->ae;_TD=_TC->typedefs;_TE=q;_TF=td;_T8->typedefs=_T9(_TD,_TE,_TF);goto _LL0;_TL8F: _T2A=_T29;{struct Cyc_Absyn_Typedefdecl**tdold=_T2A;_T10=te;_T11=_T10->ae;_T13=Cyc_Dict_delete;{
# 366
struct Cyc_Dict_Dict(*_T2B)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T13;_T12=_T2B;}_T14=te;_T15=_T14->ae;_T16=_T15->typedefs;_T17=q;_T11->typedefs=_T12(_T16,_T17);
Cyc_Tc_tcOneTypedef(te,loc,td);{
struct _fat_ptr msg=_tag_fat("Typedef redeclaration",sizeof(char),22U);_T18=td;_T19=tdold;_T1A=*_T19;_T1B=loc;_T1C=& msg;_T1D=(struct _fat_ptr*)_T1C;{
struct Cyc_Absyn_Typedefdecl*td2=Cyc_Tcdecl_merge_typedefdecl(_T18,_T1A,_T1B,_T1D);_T1E=td2;_T1F=(unsigned)_T1E;
if(!_T1F)goto _TL91;_T20=te;_T21=_T20->ae;_T23=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T2B)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Typedefdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Typedefdecl*))_T23;_T22=_T2B;}_T24=te;_T25=_T24->ae;_T26=_T25->typedefs;_T27=q;_T28=td2;_T21->typedefs=_T22(_T26,_T27,_T28);goto _TL92;_TL91: _TL92: goto _LL0;}}}_LL0:;}}}
# 377
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*flagged_tvs,struct Cyc_List_List*effc,struct Cyc_List_List**qb,struct Cyc_List_List*fields){struct Cyc_List_List*(*_T0)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(void*(*)(void*),struct Cyc_List_List*);struct Cyc_Absyn_Tvar*(*_T2)(struct _tuple12*);void*(*_T3)(struct _tuple0*);struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;int*_T6;unsigned _T7;void*_T8;void*_T9;void*_TA;void*_TB;unsigned _TC;struct Cyc_Tcenv_Tenv*_TD;struct Cyc_List_List*_TE;struct Cyc_Absyn_Kind*_TF;struct Cyc_Absyn_Kind*_T10;void*_T11;unsigned _T12;struct Cyc_Tcenv_Tenv*_T13;struct Cyc_List_List*_T14;struct Cyc_Absyn_Kind*_T15;struct Cyc_Absyn_Kind*_T16;void*_T17;void*_T18;unsigned _T19;struct Cyc_Tcenv_Tenv*_T1A;struct Cyc_List_List*_T1B;struct Cyc_Absyn_Kind*_T1C;struct Cyc_Absyn_Kind*_T1D;void*_T1E;struct Cyc_List_List*_T1F;struct Cyc_List_List*_T20;void*_T21;int(*_T22)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T23)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T24)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T25;struct _fat_ptr*_T26;int _T27;struct Cyc_Warn_String_Warn_Warg_struct _T28;struct Cyc_Warn_String_Warn_Warg_struct _T29;struct _fat_ptr*_T2A;unsigned _T2B;struct _fat_ptr _T2C;struct _fat_ptr*_T2D;struct _fat_ptr _T2E;struct _fat_ptr _T2F;int _T30;struct Cyc_List_List*_T31;struct _RegionHandle*_T32;struct Cyc_Absyn_Kind*_T33;enum Cyc_Absyn_AggrKind _T34;int _T35;struct Cyc_List_List*_T36;struct Cyc_List_List*_T37;enum Cyc_Absyn_AggrKind _T38;int _T39;struct Cyc_Absyn_Kind*_T3A;struct Cyc_List_List*_T3B;void*_T3C;struct Cyc_Absyn_Aggrfield*_T3D;unsigned _T3E;struct Cyc_List_List*_T3F;void*_T40;struct Cyc_Absyn_Aggrfield*_T41;struct Cyc_Absyn_Tqual _T42;int _T43;void*_T44;struct Cyc_Absyn_Exp*_T45;unsigned _T46;enum Cyc_Absyn_AggrKind _T47;int _T48;struct Cyc_Warn_String_Warn_Warg_struct _T49;unsigned _T4A;struct _fat_ptr _T4B;int _T4C;struct Cyc_Warn_String_Warn_Warg_struct _T4D;struct Cyc_Warn_Typ_Warn_Warg_struct _T4E;struct Cyc_Absyn_Exp*_T4F;void*_T50;struct Cyc_Warn_String_Warn_Warg_struct _T51;struct Cyc_Absyn_Exp*_T52;unsigned _T53;struct _fat_ptr _T54;struct Cyc_Absyn_Exp*_T55;unsigned _T56;struct Cyc_Tcenv_Tenv*_T57;struct Cyc_List_List*_T58;struct Cyc_List_List**_T59;struct Cyc_Absyn_Kind*_T5A;struct Cyc_Absyn_Kind*_T5B;struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T5C;void*_T5D;struct Cyc_AssnDef_AssnMap _T5E;void*_T5F;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T60;struct Cyc_AssnDef_False_AssnDef_Assn_struct*_T61;void*_T62;int _T63;struct Cyc_Warn_String_Warn_Warg_struct _T64;struct Cyc_Absyn_Exp*_T65;unsigned _T66;struct _fat_ptr _T67;void*_T68;struct Cyc_List_List*_T69;void*_T6A;void*_T6B;int _T6C;struct Cyc_Warn_String_Warn_Warg_struct _T6D;struct Cyc_Warn_String_Warn_Warg_struct _T6E;struct Cyc_Warn_String_Warn_Warg_struct _T6F;struct Cyc_Warn_String_Warn_Warg_struct _T70;struct Cyc_List_List*_T71;void*_T72;struct Cyc_Absyn_Exp*_T73;unsigned _T74;struct _fat_ptr _T75;struct Cyc_List_List*_T76;struct Cyc_List_List*_T77;struct _RegionHandle*_T78;struct Cyc_Warn_String_Warn_Warg_struct _T79;unsigned _T7A;struct _fat_ptr _T7B;struct Cyc_List_List*_T7C;struct _RegionHandle _T7D=_new_region(0U,"uprev_rgn");struct _RegionHandle*uprev_rgn=& _T7D;_push_region(uprev_rgn);_T1=Cyc_List_map;{
# 386
struct Cyc_List_List*(*_T7E)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*))_T1;_T0=_T7E;}_T3=Cyc_Core_fst;{struct Cyc_Absyn_Tvar*(*_T7E)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))_T3;_T2=_T7E;}_T4=flagged_tvs;{struct Cyc_List_List*tvs=_T0(_T2,_T4);
_TL96: if(effc!=0)goto _TL94;else{goto _TL95;}
_TL94: _T5=effc;{void*_T7E=_T5->hd;void*_T7F;void*_T80;_T6=(int*)_T7E;_T7=*_T6;switch(_T7){case 1:{struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*_T81=(struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_T7E;_T8=_T81->f1;_T80=(void*)_T8;_T9=_T81->f2;_T7F=(void*)_T9;}{void*e1=_T80;void*e2=_T7F;_T80=e1;_T7F=e2;goto _LL4;}case 2:{struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*_T81=(struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_T7E;_TA=_T81->f1;_T80=(void*)_TA;_TB=_T81->f2;_T7F=(void*)_TB;}_LL4: {void*e1=_T80;void*e2=_T7F;_TC=loc;_TD=te;_TE=tvs;_TF=& Cyc_Kinds_ek;_T10=(struct Cyc_Absyn_Kind*)_TF;_T11=e1;
# 391
Cyc_Tctyp_check_type(_TC,_TD,_TE,_T10,0,0,_T11);_T12=loc;_T13=te;_T14=tvs;_T15=& Cyc_Kinds_ek;_T16=(struct Cyc_Absyn_Kind*)_T15;_T17=e2;
Cyc_Tctyp_check_type(_T12,_T13,_T14,_T16,0,0,_T17);goto _LL0;}default:{struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*_T81=(struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_T7E;_T18=_T81->f1;_T80=(void*)_T18;}{void*e1=_T80;_T19=loc;_T1A=te;_T1B=tvs;_T1C=& Cyc_Kinds_ek;_T1D=(struct Cyc_Absyn_Kind*)_T1C;_T1E=e1;
# 395
Cyc_Tctyp_check_type(_T19,_T1A,_T1B,_T1D,0,0,_T1E);goto _LL0;}}_LL0:;}_T1F=effc;
# 387
effc=_T1F->tl;goto _TL96;_TL95: {
# 400
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_requires=0;
struct Cyc_List_List*fs=fields;_TL9B: if(fs!=0)goto _TL99;else{goto _TL9A;}
_TL99: _T20=fs;_T21=_T20->hd;{struct Cyc_Absyn_Aggrfield*_T7E=(struct Cyc_Absyn_Aggrfield*)_T21;struct Cyc_Absyn_Exp*_T7F;struct Cyc_List_List*_T80;struct Cyc_Absyn_Exp*_T81;void*_T82;struct Cyc_Absyn_Tqual _T83;struct _fat_ptr*_T84;{struct Cyc_Absyn_Aggrfield _T85=*_T7E;_T84=_T85.name;_T83=_T85.tq;_T82=_T85.type;_T81=_T85.width;_T80=_T85.attributes;_T7F=_T85.requires_clause;}{struct _fat_ptr*fn=_T84;struct Cyc_Absyn_Tqual tq=_T83;void*t=_T82;struct Cyc_Absyn_Exp*width=_T81;struct Cyc_List_List*atts=_T80;struct Cyc_Absyn_Exp*requires_clause=_T7F;_T23=Cyc_List_mem;{
# 405
int(*_T85)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T23;_T22=_T85;}_T24=Cyc_strptrcmp;_T25=prev_fields;_T26=fn;_T27=_T22(_T24,_T25,_T26);if(!_T27)goto _TL9C;{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
_T85.f1=_tag_fat("duplicate member ",sizeof(char),18U);_T28=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T28;{struct Cyc_Warn_String_Warn_Warg_struct _T86;_T86.tag=0;_T2A=fn;_T86.f1=*_T2A;_T29=_T86;}{struct Cyc_Warn_String_Warn_Warg_struct _T86=_T29;void*_T87[2];_T87[0]=& _T85;_T87[1]=& _T86;_T2B=loc;_T2C=_tag_fat(_T87,sizeof(void*),2);Cyc_Warn_err2(_T2B,_T2C);}}goto _TL9D;_TL9C: _TL9D: _T2D=fn;_T2E=*_T2D;_T2F=
# 409
_tag_fat("",sizeof(char),1U);_T30=Cyc_strcmp(_T2E,_T2F);if(_T30==0)goto _TL9E;_T32=uprev_rgn;{struct Cyc_List_List*_T85=_region_malloc(_T32,0U,sizeof(struct Cyc_List_List));
_T85->hd=fn;_T85->tl=prev_fields;_T31=(struct Cyc_List_List*)_T85;}prev_fields=_T31;goto _TL9F;_TL9E: _TL9F: _T33=& Cyc_Kinds_mk;{
# 412
struct Cyc_Absyn_Kind*field_kind=(struct Cyc_Absyn_Kind*)_T33;_T34=str_or_union;_T35=(int)_T34;
# 416
if(_T35==1)goto _TLA2;else{goto _TLA3;}_TLA3: _T36=fs;_T37=_T36->tl;if(_T37==0)goto _TLA4;else{goto _TLA0;}_TLA4: _T38=str_or_union;_T39=(int)_T38;if(_T39==0)goto _TLA2;else{goto _TLA0;}
# 418
_TLA2: _T3A=& Cyc_Kinds_ak;field_kind=(struct Cyc_Absyn_Kind*)_T3A;goto _TLA1;_TLA0: _TLA1:
 Cyc_Tctyp_check_type_with_bounds(loc,te,flagged_tvs,qb,field_kind,0,0,t);_T3B=fs;_T3C=_T3B->hd;_T3D=(struct Cyc_Absyn_Aggrfield*)_T3C;_T3E=loc;_T3F=fs;_T40=_T3F->hd;_T41=(struct Cyc_Absyn_Aggrfield*)_T40;_T42=_T41->tq;_T43=_T42.print_const;_T44=t;
# 421
_T3D->tq.real_const=Cyc_Tcutil_extract_const_from_typedef(_T3E,_T43,_T44);
# 424
Cyc_Tcutil_check_bitfield(loc,t,width,fn);_T45=requires_clause;_T46=(unsigned)_T45;
# 426
if(!_T46)goto _TLA5;_T47=str_or_union;_T48=(int)_T47;
if(_T48==1)goto _TLA7;{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
_T85.f1=_tag_fat("@requires clauses are allowed only on union members",sizeof(char),52U);_T49=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T49;void*_T86[1];_T86[0]=& _T85;_T4A=loc;_T4B=_tag_fat(_T86,sizeof(void*),1);Cyc_Warn_err2(_T4A,_T4B);}goto _TLA8;_TLA7: _TLA8:{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_allow_valueof(te);
Cyc_Tcexp_tcExp(te2,0,requires_clause);_T4C=
Cyc_Tcutil_is_integral(requires_clause);if(_T4C)goto _TLA9;else{goto _TLAB;}
_TLAB:{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
_T85.f1=_tag_fat("@requires clause has type ",sizeof(char),27U);_T4D=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T4D;{struct Cyc_Warn_Typ_Warn_Warg_struct _T86;_T86.tag=2;_T4F=requires_clause;_T50=_T4F->topt;_T86.f1=_check_null(_T50);_T4E=_T86;}{struct Cyc_Warn_Typ_Warn_Warg_struct _T86=_T4E;{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;
_T87.f1=_tag_fat(" instead of integral type",sizeof(char),26U);_T51=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T51;void*_T88[3];_T88[0]=& _T85;_T88[1]=& _T86;_T88[2]=& _T87;_T52=requires_clause;_T53=_T52->loc;_T54=_tag_fat(_T88,sizeof(void*),3);Cyc_Warn_err2(_T53,_T54);}}}goto _TLAA;
# 436
_TLA9: _T55=requires_clause;_T56=_T55->loc;_T57=te;_T58=flagged_tvs;_T59=qb;_T5A=& Cyc_Kinds_ik;_T5B=(struct Cyc_Absyn_Kind*)_T5A;{struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_T85=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));_T85->tag=9;
_T85->f1=requires_clause;_T5C=(struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_T85;}_T5D=(void*)_T5C;
# 436
Cyc_Tctyp_check_type_with_bounds(_T56,_T57,_T58,_T59,_T5B,0,0,_T5D);{
# 439
struct Cyc_AssnDef_AssnMap assnmap=Cyc_Vcgen_clause2assn(requires_clause);_T5E=assnmap;{
void*req_assn=_T5E.assn;_T5F=req_assn;_T60=& Cyc_AssnDef_false_assn;_T61=(struct Cyc_AssnDef_False_AssnDef_Assn_struct*)_T60;_T62=(void*)_T61;_T63=
# 442
Cyc_PrattProver_constraint_prove(_T5F,_T62);if(!_T63)goto _TLAC;{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
_T85.f1=_tag_fat("@requires clause may be unsatisfiable",sizeof(char),38U);_T64=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T64;void*_T86[1];_T86[0]=& _T85;_T65=requires_clause;_T66=_T65->loc;_T67=_tag_fat(_T86,sizeof(void*),1);Cyc_Warn_err2(_T66,_T67);}goto _TLAD;_TLAC: _TLAD:{
# 448
struct Cyc_List_List*p=prev_requires;_TLB1: if(p!=0)goto _TLAF;else{goto _TLB0;}
_TLAF: _T68=req_assn;_T69=p;_T6A=_T69->hd;_T6B=Cyc_AssnDef_not(_T6A);_T6C=Cyc_PrattProver_constraint_prove(_T68,_T6B);if(_T6C)goto _TLB2;else{goto _TLB4;}
# 451
_TLB4:{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
_T85.f1=_tag_fat("@requires clause ",sizeof(char),18U);_T6D=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T6D;{struct Cyc_Warn_String_Warn_Warg_struct _T86;_T86.tag=0;_T86.f1=Cyc_AssnDef_assn2string(req_assn);_T6E=_T86;}{struct Cyc_Warn_String_Warn_Warg_struct _T86=_T6E;{struct Cyc_Warn_String_Warn_Warg_struct _T87;_T87.tag=0;_T87.f1=_tag_fat(" may overlap with previous clauses ",sizeof(char),36U);_T6F=_T87;}{struct Cyc_Warn_String_Warn_Warg_struct _T87=_T6F;{struct Cyc_Warn_String_Warn_Warg_struct _T88;_T88.tag=0;_T71=p;_T72=_T71->hd;_T88.f1=Cyc_AssnDef_assn2string(_T72);_T70=_T88;}{struct Cyc_Warn_String_Warn_Warg_struct _T88=_T70;void*_T89[4];_T89[0]=& _T85;_T89[1]=& _T86;_T89[2]=& _T87;_T89[3]=& _T88;_T73=requires_clause;_T74=_T73->loc;_T75=_tag_fat(_T89,sizeof(void*),4);Cyc_Warn_err2(_T74,_T75);}}}}goto _TLB3;_TLB2: _TLB3: _T76=p;
# 448
p=_T76->tl;goto _TLB1;_TLB0:;}_T78=uprev_rgn;{struct Cyc_List_List*_T85=_region_malloc(_T78,0U,sizeof(struct Cyc_List_List));
# 454
_T85->hd=req_assn;_T85->tl=prev_requires;_T77=(struct Cyc_List_List*)_T85;}prev_requires=_T77;}}_TLAA:;}goto _TLA6;
# 457
_TLA5: if(prev_requires==0)goto _TLB5;{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
_T85.f1=_tag_fat("if one field has a @requires clause, they all must",sizeof(char),51U);_T79=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T79;void*_T86[1];_T86[0]=& _T85;_T7A=loc;_T7B=_tag_fat(_T86,sizeof(void*),1);Cyc_Warn_err2(_T7A,_T7B);}goto _TLB6;_TLB5: _TLB6: _TLA6:;}}}_T7C=fs;
# 402
fs=_T7C->tl;goto _TL9B;_TL9A:;}}_pop_region();}
# 462
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple1*n,struct Cyc_List_List*tvs){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Tvar*_T2;void*_T3;int*_T4;unsigned _T5;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T6;struct Cyc_Core_Opt**_T7;struct Cyc_Core_Opt**_T8;struct Cyc_Core_Opt*_T9;struct Cyc_Absyn_Kind*_TA;struct Cyc_Absyn_Kind*_TB;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TC;struct Cyc_Absyn_Kind*_TD;struct Cyc_Absyn_Kind*_TE;enum Cyc_Absyn_KindQual _TF;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T10;struct Cyc_Core_Opt**_T11;struct Cyc_Absyn_Kind*_T12;struct Cyc_Core_Opt**_T13;struct Cyc_Core_Opt*_T14;struct Cyc_Absyn_Kind*_T15;struct Cyc_Absyn_Kind*_T16;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T17;struct Cyc_Core_Opt**_T18;struct Cyc_Core_Opt**_T19;struct Cyc_Core_Opt*_T1A;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1B;struct Cyc_Absyn_Kind*_T1C;struct Cyc_Absyn_Kind*_T1D;enum Cyc_Absyn_KindQual _T1E;struct Cyc_Absyn_Kind*_T1F;struct Cyc_Warn_String_Warn_Warg_struct _T20;struct Cyc_Warn_Qvar_Warn_Warg_struct _T21;struct Cyc_Warn_String_Warn_Warg_struct _T22;struct Cyc_Warn_Tvar_Warn_Warg_struct _T23;struct Cyc_List_List*_T24;void*_T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;struct Cyc_Warn_Kind_Warn_Warg_struct _T27;struct Cyc_Absyn_Kind*_T28;unsigned _T29;struct _fat_ptr _T2A;struct Cyc_List_List*_T2B;
struct Cyc_List_List*tvs2=tvs;_TLBA: if(tvs2!=0)goto _TLB8;else{goto _TLB9;}
_TLB8: _T0=tvs2;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Tvar*)_T1;_T3=_T2->kind;{void*_T2C=Cyc_Kinds_compress_kb(_T3);struct Cyc_Absyn_Kind*_T2D;enum Cyc_Absyn_AliasHint _T2E;void*_T2F;_T4=(int*)_T2C;_T5=*_T4;switch(_T5){case 1:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T2C;_T6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T2C;_T7=& _T6->f1;_T2F=(struct Cyc_Core_Opt**)_T7;}{struct Cyc_Core_Opt**f=_T2F;_T8=f;{struct Cyc_Core_Opt*_T30=_cycalloc(sizeof(struct Cyc_Core_Opt));_TA=& Cyc_Kinds_bk;_TB=(struct Cyc_Absyn_Kind*)_TA;
# 466
_T30->v=Cyc_Kinds_kind_to_bound(_TB);_T9=(struct Cyc_Core_Opt*)_T30;}*_T8=_T9;goto _TLB7;}case 2: _TC=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T2C;_TD=_TC->f2;_TE=(struct Cyc_Absyn_Kind*)_TD;_TF=_TE->kind;if(_TF!=Cyc_Absyn_MemKind)goto _TLBC;{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T2C;_T10=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T2C;_T11=& _T10->f1;_T2F=(struct Cyc_Core_Opt**)_T11;_T12=_T30->f2;{struct Cyc_Absyn_Kind _T31=*_T12;_T2E=_T31.aliashint;}}{struct Cyc_Core_Opt**f=_T2F;enum Cyc_Absyn_AliasHint a=_T2E;_T13=f;{struct Cyc_Core_Opt*_T30=_cycalloc(sizeof(struct Cyc_Core_Opt));_T15=& Cyc_Kinds_bk;_T16=(struct Cyc_Absyn_Kind*)_T15;
# 471
_T30->v=Cyc_Kinds_kind_to_bound(_T16);_T14=(struct Cyc_Core_Opt*)_T30;}*_T13=_T14;goto _TLB7;}_TLBC:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T2C;_T17=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T2C;_T18=& _T17->f1;_T2F=(struct Cyc_Core_Opt**)_T18;_T2D=_T30->f2;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T2F;struct Cyc_Absyn_Kind*k=_T2D;_T19=f;{struct Cyc_Core_Opt*_T30=_cycalloc(sizeof(struct Cyc_Core_Opt));
# 477
_T30->v=Cyc_Kinds_kind_to_bound(k);_T1A=(struct Cyc_Core_Opt*)_T30;}*_T19=_T1A;goto _TLB7;}default: _T1B=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T2C;_T1C=_T1B->f1;_T1D=(struct Cyc_Absyn_Kind*)_T1C;_T1E=_T1D->kind;if(_T1E!=Cyc_Absyn_MemKind)goto _TLBE;{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T30=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T2C;_T1F=_T30->f1;{struct Cyc_Absyn_Kind _T31=*_T1F;_T2E=_T31.aliashint;}}{enum Cyc_Absyn_AliasHint a=_T2E;{struct Cyc_Warn_String_Warn_Warg_struct _T30;_T30.tag=0;
# 479
_T30.f1=_tag_fat("type ",sizeof(char),6U);_T20=_T30;}{struct Cyc_Warn_String_Warn_Warg_struct _T30=_T20;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T31;_T31.tag=1;_T31.f1=n;_T21=_T31;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T31=_T21;{struct Cyc_Warn_String_Warn_Warg_struct _T32;_T32.tag=0;_T32.f1=_tag_fat(" attempts to abstract type variable ",sizeof(char),37U);_T22=_T32;}{struct Cyc_Warn_String_Warn_Warg_struct _T32=_T22;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T33;_T33.tag=7;_T24=tvs2;_T25=_T24->hd;
_T33.f1=(struct Cyc_Absyn_Tvar*)_T25;_T23=_T33;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T33=_T23;{struct Cyc_Warn_String_Warn_Warg_struct _T34;_T34.tag=0;_T34.f1=_tag_fat(" of kind ",sizeof(char),10U);_T26=_T34;}{struct Cyc_Warn_String_Warn_Warg_struct _T34=_T26;{struct Cyc_Warn_Kind_Warn_Warg_struct _T35;_T35.tag=9;{struct Cyc_Absyn_Kind*_T36=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_T36->kind=1U;_T36->aliashint=a;_T28=(struct Cyc_Absyn_Kind*)_T36;}_T35.f1=_T28;_T27=_T35;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T35=_T27;void*_T36[6];_T36[0]=& _T30;_T36[1]=& _T31;_T36[2]=& _T32;_T36[3]=& _T33;_T36[4]=& _T34;_T36[5]=& _T35;_T29=loc;_T2A=_tag_fat(_T36,sizeof(void*),6);Cyc_Warn_err2(_T29,_T2A);}}}}}}goto _TLB7;}_TLBE: goto _TLB7;};}_TLB7: _T2B=tvs2;
# 463
tvs2=_T2B->tl;goto _TLBA;_TLB9:;}
# 486
static void Cyc_Tc_rule_out_mem_and_qual(unsigned loc,struct _tuple1*q,struct Cyc_List_List*tvs){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Tvar*_T2;void*_T3;int*_T4;unsigned _T5;struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T6;struct Cyc_Core_Opt**_T7;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T8;struct Cyc_Absyn_Kind*_T9;struct Cyc_Absyn_Kind*_TA;enum Cyc_Absyn_KindQual _TB;int _TC;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_TD;struct Cyc_Core_Opt**_TE;struct Cyc_Core_Opt**_TF;struct Cyc_Core_Opt*_T10;struct Cyc_Absyn_Kind*_T11;struct Cyc_Absyn_Kind*_T12;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T13;struct Cyc_Core_Opt**_T14;struct Cyc_Core_Opt**_T15;struct Cyc_Core_Opt*_T16;struct Cyc_Absyn_Kind*_T17;struct Cyc_Absyn_Kind*_T18;struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T19;struct Cyc_Core_Opt**_T1A;struct Cyc_Core_Opt**_T1B;struct Cyc_Core_Opt*_T1C;struct Cyc_Absyn_Kind*_T1D;struct Cyc_Absyn_Kind*_T1E;struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T1F;struct Cyc_Absyn_Kind*_T20;struct Cyc_Absyn_Kind*_T21;enum Cyc_Absyn_KindQual _T22;int _T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;struct Cyc_Warn_Qvar_Warn_Warg_struct _T25;struct Cyc_Warn_String_Warn_Warg_struct _T26;struct Cyc_Warn_Tvar_Warn_Warg_struct _T27;struct Cyc_List_List*_T28;void*_T29;struct Cyc_Warn_String_Warn_Warg_struct _T2A;unsigned _T2B;struct _fat_ptr _T2C;struct Cyc_Absyn_Kind*_T2D;struct Cyc_Warn_String_Warn_Warg_struct _T2E;struct Cyc_Warn_Qvar_Warn_Warg_struct _T2F;struct Cyc_Warn_String_Warn_Warg_struct _T30;struct Cyc_Warn_Tvar_Warn_Warg_struct _T31;struct Cyc_List_List*_T32;void*_T33;struct Cyc_Warn_String_Warn_Warg_struct _T34;struct Cyc_Warn_Kind_Warn_Warg_struct _T35;struct Cyc_Absyn_Kind*_T36;unsigned _T37;struct _fat_ptr _T38;struct Cyc_List_List*_T39;
struct Cyc_List_List*tvs2=tvs;_TLC3: if(tvs2!=0)goto _TLC1;else{goto _TLC2;}
_TLC1: _T0=tvs2;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Tvar*)_T1;_T3=_T2->kind;{void*_T3A=Cyc_Kinds_compress_kb(_T3);enum Cyc_Absyn_AliasHint _T3B;void*_T3C;_T4=(int*)_T3A;_T5=*_T4;switch(_T5){case 1:{struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_T3D=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3A;_T6=(struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_T3A;_T7=& _T6->f1;_T3C=(struct Cyc_Core_Opt**)_T7;}{struct Cyc_Core_Opt**f=_T3C;_T3C=f;goto _LL4;}case 2: _T8=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3A;_T9=_T8->f2;_TA=(struct Cyc_Absyn_Kind*)_T9;_TB=_TA->kind;_TC=(int)_TB;switch(_TC){case Cyc_Absyn_MemKind:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3A;_TD=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3A;_TE=& _TD->f1;_T3C=(struct Cyc_Core_Opt**)_TE;}_LL4: {struct Cyc_Core_Opt**f=_T3C;_TF=f;{struct Cyc_Core_Opt*_T3D=_cycalloc(sizeof(struct Cyc_Core_Opt));_T11=& Cyc_Kinds_bk;_T12=(struct Cyc_Absyn_Kind*)_T11;
# 492
_T3D->v=Cyc_Kinds_kind_to_bound(_T12);_T10=(struct Cyc_Core_Opt*)_T3D;}*_TF=_T10;goto _LL0;}case Cyc_Absyn_AnyKind:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3A;_T13=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3A;_T14=& _T13->f1;_T3C=(struct Cyc_Core_Opt**)_T14;}{struct Cyc_Core_Opt**f=_T3C;_T15=f;{struct Cyc_Core_Opt*_T3D=_cycalloc(sizeof(struct Cyc_Core_Opt));_T17=& Cyc_Kinds_ak;_T18=(struct Cyc_Absyn_Kind*)_T17;
# 494
_T3D->v=Cyc_Kinds_kind_to_bound(_T18);_T16=(struct Cyc_Core_Opt*)_T3D;}*_T15=_T16;goto _LL0;}case Cyc_Absyn_EffKind:{struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_T3D=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3A;_T19=(struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_T3A;_T1A=& _T19->f1;_T3C=(struct Cyc_Core_Opt**)_T1A;}{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_T3C;_T1B=f;{struct Cyc_Core_Opt*_T3D=_cycalloc(sizeof(struct Cyc_Core_Opt));_T1D=& Cyc_Kinds_ek;_T1E=(struct Cyc_Absyn_Kind*)_T1D;
# 496
_T3D->v=Cyc_Kinds_kind_to_bound(_T1E);_T1C=(struct Cyc_Core_Opt*)_T3D;}*_T1B=_T1C;goto _LL0;}default: goto _LLD;};default: _T1F=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3A;_T20=_T1F->f1;_T21=(struct Cyc_Absyn_Kind*)_T20;_T22=_T21->kind;_T23=(int)_T22;switch(_T23){case Cyc_Absyn_AqualKind:{struct Cyc_Warn_String_Warn_Warg_struct _T3D;_T3D.tag=0;
# 498
_T3D.f1=_tag_fat("type ",sizeof(char),6U);_T24=_T3D;}{struct Cyc_Warn_String_Warn_Warg_struct _T3D=_T24;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3E;_T3E.tag=1;_T3E.f1=q;_T25=_T3E;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3E=_T25;{struct Cyc_Warn_String_Warn_Warg_struct _T3F;_T3F.tag=0;_T3F.f1=_tag_fat(" attempts to abstract type variable ",sizeof(char),37U);_T26=_T3F;}{struct Cyc_Warn_String_Warn_Warg_struct _T3F=_T26;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T40;_T40.tag=7;_T28=tvs2;_T29=_T28->hd;
_T40.f1=(struct Cyc_Absyn_Tvar*)_T29;_T27=_T40;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T40=_T27;{struct Cyc_Warn_String_Warn_Warg_struct _T41;_T41.tag=0;_T41.f1=_tag_fat(" of kind Q",sizeof(char),11U);_T2A=_T41;}{struct Cyc_Warn_String_Warn_Warg_struct _T41=_T2A;void*_T42[5];_T42[0]=& _T3D;_T42[1]=& _T3E;_T42[2]=& _T3F;_T42[3]=& _T40;_T42[4]=& _T41;_T2B=loc;_T2C=_tag_fat(_T42,sizeof(void*),5);Cyc_Warn_err2(_T2B,_T2C);}}}}}goto _LL0;case Cyc_Absyn_MemKind:{struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_T3D=(struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_T3A;_T2D=_T3D->f1;{struct Cyc_Absyn_Kind _T3E=*_T2D;_T3B=_T3E.aliashint;}}{enum Cyc_Absyn_AliasHint a=_T3B;{struct Cyc_Warn_String_Warn_Warg_struct _T3D;_T3D.tag=0;
# 501
_T3D.f1=_tag_fat("type ",sizeof(char),6U);_T2E=_T3D;}{struct Cyc_Warn_String_Warn_Warg_struct _T3D=_T2E;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3E;_T3E.tag=1;_T3E.f1=q;_T2F=_T3E;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T3E=_T2F;{struct Cyc_Warn_String_Warn_Warg_struct _T3F;_T3F.tag=0;_T3F.f1=_tag_fat(" attempts to abstract type variable ",sizeof(char),37U);_T30=_T3F;}{struct Cyc_Warn_String_Warn_Warg_struct _T3F=_T30;{struct Cyc_Warn_Tvar_Warn_Warg_struct _T40;_T40.tag=7;_T32=tvs2;_T33=_T32->hd;
_T40.f1=(struct Cyc_Absyn_Tvar*)_T33;_T31=_T40;}{struct Cyc_Warn_Tvar_Warn_Warg_struct _T40=_T31;{struct Cyc_Warn_String_Warn_Warg_struct _T41;_T41.tag=0;_T41.f1=_tag_fat(" of kind ",sizeof(char),10U);_T34=_T41;}{struct Cyc_Warn_String_Warn_Warg_struct _T41=_T34;{struct Cyc_Warn_Kind_Warn_Warg_struct _T42;_T42.tag=9;{struct Cyc_Absyn_Kind*_T43=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_T43->kind=1U;_T43->aliashint=a;_T36=(struct Cyc_Absyn_Kind*)_T43;}_T42.f1=_T36;_T35=_T42;}{struct Cyc_Warn_Kind_Warn_Warg_struct _T42=_T35;void*_T43[6];_T43[0]=& _T3D;_T43[1]=& _T3E;_T43[2]=& _T3F;_T43[3]=& _T40;_T43[4]=& _T41;_T43[5]=& _T42;_T37=loc;_T38=_tag_fat(_T43,sizeof(void*),6);Cyc_Warn_err2(_T37,_T38);}}}}}}goto _LL0;}default: _LLD: goto _LL0;};}_LL0:;}_T39=tvs2;
# 487
tvs2=_T39->tl;goto _TLC3;_TLC2:;}struct _tuple16{struct Cyc_Absyn_AggrdeclImpl*f0;struct Cyc_Absyn_Aggrdecl***f1;};
# 509
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){struct Cyc_Absyn_Aggrdecl*_T0;unsigned _T1;struct _tuple1*_T2;struct _tuple1 _T3;struct _fat_ptr*_T4;struct Cyc_Absyn_Aggrdecl*_T5;struct Cyc_List_List*_T6;struct Cyc_Absyn_Aggrdecl*_T7;unsigned _T8;struct Cyc_Absyn_Aggrdecl*_T9;struct Cyc_List_List*_TA;struct Cyc_Absyn_Aggrdecl*_TB;struct Cyc_List_List*_TC;struct _tuple16 _TD;struct Cyc_Absyn_Aggrdecl*_TE;struct Cyc_Absyn_Aggrdecl***(*_TF)(struct Cyc_Dict_Dict,struct _tuple1*);void**(*_T10)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T11;struct Cyc_Tcenv_Genv*_T12;struct Cyc_Dict_Dict _T13;struct _tuple1*_T14;struct Cyc_Absyn_AggrdeclImpl*_T15;struct Cyc_Absyn_Aggrdecl***_T16;struct Cyc_Tcenv_Tenv*_T17;struct Cyc_Tcenv_Genv*_T18;struct Cyc_Dict_Dict(*_T19)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**);struct Cyc_Dict_Dict(*_T1A)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_T1B;struct Cyc_Tcenv_Genv*_T1C;struct Cyc_Dict_Dict _T1D;struct _tuple1*_T1E;struct Cyc_Absyn_Aggrdecl**_T1F;struct Cyc_Absyn_Aggrdecl***_T20;struct Cyc_Absyn_Aggrdecl**_T21;struct Cyc_Absyn_Aggrdecl*_T22;struct Cyc_Absyn_Aggrdecl*_T23;unsigned _T24;struct _fat_ptr*_T25;struct Cyc_Absyn_Aggrdecl**_T26;struct Cyc_Absyn_Aggrdecl***_T27;struct Cyc_Absyn_AggrdeclImpl*_T28;struct Cyc_Absyn_AggrdeclImpl*_T29;struct Cyc_List_List**_T2A;struct Cyc_Absyn_Aggrdecl**_T2B;struct Cyc_Absyn_Aggrdecl*_T2C;struct Cyc_Absyn_Aggrdecl*_T2D;struct Cyc_Absyn_Aggrdecl*_T2E;struct Cyc_Absyn_Aggrdecl*_T2F;struct Cyc_Tcenv_Tenv*_T30;struct Cyc_Tcenv_Genv*_T31;struct Cyc_Dict_Dict(*_T32)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**);struct Cyc_Dict_Dict(*_T33)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_T34;struct Cyc_Tcenv_Genv*_T35;struct Cyc_Dict_Dict _T36;struct _tuple1*_T37;struct Cyc_Absyn_Aggrdecl**_T38;int _T39;struct Cyc_Absyn_Aggrdecl*_T3A;enum Cyc_Absyn_AggrKind _T3B;int _T3C;struct Cyc_Warn_String_Warn_Warg_struct _T3D;unsigned _T3E;struct _fat_ptr _T3F;struct Cyc_List_List*(*_T40)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*);struct Cyc_List_List*(*_T41)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple12*(*_T42)(int,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T43;struct Cyc_List_List*_T44;struct Cyc_List_List*(*_T45)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*);struct Cyc_List_List*(*_T46)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple12*(*_T47)(int,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T48;struct Cyc_List_List*_T49;struct Cyc_Tcenv_Tenv*_T4A;unsigned _T4B;struct Cyc_Absyn_Aggrdecl*_T4C;enum Cyc_Absyn_AggrKind _T4D;struct Cyc_List_List*_T4E;struct Cyc_List_List*_T4F;struct Cyc_List_List**_T50;struct Cyc_List_List*_T51;struct Cyc_Absyn_Aggrdecl*_T52;enum Cyc_Absyn_AggrKind _T53;int _T54;int _T55;int _T56;struct Cyc_List_List*_T57;void*_T58;struct Cyc_Absyn_Aggrfield*_T59;void*_T5A;int _T5B;struct Cyc_List_List*_T5C;void*_T5D;struct Cyc_Absyn_Aggrfield*_T5E;struct Cyc_Absyn_Exp*_T5F;struct Cyc_Warn_String_Warn_Warg_struct _T60;struct Cyc_Warn_String_Warn_Warg_struct _T61;struct Cyc_List_List*_T62;void*_T63;struct Cyc_Absyn_Aggrfield*_T64;struct _fat_ptr*_T65;struct Cyc_Warn_String_Warn_Warg_struct _T66;struct Cyc_Warn_Qvar_Warn_Warg_struct _T67;struct Cyc_Warn_String_Warn_Warg_struct _T68;struct Cyc_Warn_Typ_Warn_Warg_struct _T69;struct Cyc_List_List*_T6A;void*_T6B;struct Cyc_Absyn_Aggrfield*_T6C;struct Cyc_Warn_String_Warn_Warg_struct _T6D;unsigned _T6E;struct _fat_ptr _T6F;struct Cyc_List_List*_T70;struct Cyc_Absyn_Aggrdecl**_T71;struct Cyc_Absyn_AggrdeclImpl*_T72;struct Cyc_Absyn_AggrdeclImpl*_T73;struct Cyc_List_List**_T74;struct Cyc_Absyn_Aggrdecl***_T75;struct Cyc_Absyn_Aggrdecl*_T76;enum Cyc_Absyn_AggrKind _T77;int _T78;struct Cyc_Absyn_Aggrdecl**_T79;struct Cyc_Absyn_Aggrdecl*_T7A;enum Cyc_Absyn_AggrKind _T7B;int _T7C;struct Cyc_Warn_String_Warn_Warg_struct _T7D;unsigned _T7E;struct _fat_ptr _T7F;struct Cyc_Absyn_Aggrdecl**_T80;struct Cyc_Absyn_Aggrdecl**_T81;struct Cyc_Absyn_Aggrdecl*_T82;struct Cyc_Absyn_Aggrdecl*_T83;struct Cyc_Absyn_Aggrdecl*_T84;struct Cyc_Absyn_Aggrdecl*_T85;int _T86;struct Cyc_Absyn_Aggrdecl*_T87;enum Cyc_Absyn_AggrKind _T88;int _T89;struct Cyc_Warn_String_Warn_Warg_struct _T8A;unsigned _T8B;struct _fat_ptr _T8C;struct Cyc_List_List*(*_T8D)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*);struct Cyc_List_List*(*_T8E)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple12*(*_T8F)(int,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T90;struct Cyc_List_List*_T91;struct Cyc_List_List*(*_T92)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*);struct Cyc_List_List*(*_T93)(void*(*)(void*,void*),void*,struct Cyc_List_List*);struct _tuple12*(*_T94)(int,struct Cyc_Absyn_Tvar*);struct Cyc_List_List*_T95;struct Cyc_List_List*_T96;struct Cyc_Tcenv_Tenv*_T97;unsigned _T98;struct Cyc_Absyn_Aggrdecl*_T99;enum Cyc_Absyn_AggrKind _T9A;struct Cyc_List_List*_T9B;struct Cyc_List_List*_T9C;struct Cyc_List_List**_T9D;struct Cyc_List_List*_T9E;struct Cyc_Absyn_Aggrdecl**_T9F;_T0=ad;{
struct _tuple1*q=_T0->name;_T1=loc;_T2=q;_T3=*_T2;_T4=_T3.f1;_T5=ad;_T6=_T5->attributes;
# 515
Cyc_Atts_check_field_atts(_T1,_T4,_T6);_T7=ad;{
# 517
struct Cyc_List_List*tvs=_T7->tvs;_T8=loc;_T9=ad;_TA=_T9->tvs;
# 520
Cyc_Tcutil_check_unique_tvars(_T8,_TA);_TB=ad;_TC=_TB->tvs;
Cyc_Tcutil_add_tvar_identities(_TC);{struct _tuple16 _TA0;_TE=ad;
# 525
_TA0.f0=_TE->impl;_T10=Cyc_Dict_lookup_opt;{struct Cyc_Absyn_Aggrdecl***(*_TA1)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T10;_TF=_TA1;}_T11=te;_T12=_T11->ae;_T13=_T12->aggrdecls;_T14=q;_TA0.f1=_TF(_T13,_T14);_TD=_TA0;}{struct _tuple16 _TA0=_TD;struct Cyc_Absyn_Aggrdecl**_TA1;struct Cyc_List_List*_TA2;int _TA3;struct Cyc_List_List*_TA4;struct Cyc_List_List*_TA5;void*_TA6;_T15=_TA0.f0;if(_T15!=0)goto _TLC7;_T16=_TA0.f1;if(_T16!=0)goto _TLC9;
# 528
Cyc_Tc_rule_out_memkind(loc,q,tvs);_T17=te;_T18=_T17->ae;_T1A=Cyc_Dict_insert;{
# 530
struct Cyc_Dict_Dict(*_TA7)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**))_T1A;_T19=_TA7;}_T1B=te;_T1C=_T1B->ae;_T1D=_T1C->aggrdecls;_T1E=q;{struct Cyc_Absyn_Aggrdecl**_TA7=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_TA7=ad;_T1F=(struct Cyc_Absyn_Aggrdecl**)_TA7;}_T18->aggrdecls=_T19(_T1D,_T1E,_T1F);goto _LL0;_TLC9: _T20=_TA0.f1;{struct Cyc_Absyn_Aggrdecl**_TA7=*_T20;_TA6=_TA7;}_LL8:{struct Cyc_Absyn_Aggrdecl**adp=_TA6;_T21=adp;_T22=*_T21;_T23=ad;_T24=loc;_T25=Cyc_Tc_tc_msg;{
# 596
struct Cyc_Absyn_Aggrdecl*ad2=Cyc_Tcdecl_merge_aggrdecl(_T22,_T23,_T24,_T25);
if(ad2!=0)goto _TLCB;
return;_TLCB:
 Cyc_Tc_rule_out_memkind(loc,q,tvs);_T26=adp;
# 604
*_T26=ad2;}}goto _TLC8;_TLC7: _T27=_TA0.f1;if(_T27!=0)goto _TLCD;_T28=_TA0.f0;{struct Cyc_Absyn_AggrdeclImpl _TA7=*_T28;_TA5=_TA7.exist_vars;_T29=_TA0.f0;_T2A=& _T29->qual_bnd;_TA6=(struct Cyc_List_List**)_T2A;_TA4=_TA7.fields;_TA3=_TA7.tagged;_TA2=_TA7.effconstr;}{struct Cyc_List_List*exist_vars=_TA5;struct Cyc_List_List**qb=_TA6;struct Cyc_List_List*fs=_TA4;int tagged=_TA3;struct Cyc_List_List*effconstr=_TA2;
# 537
struct Cyc_Absyn_Aggrdecl**adp;adp=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));_T2B=adp;{struct Cyc_Absyn_Aggrdecl*_TA7=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T2D=ad;_TA7->kind=_T2D->kind;_TA7->sc=3U;_T2E=ad;_TA7->name=_T2E->name;_TA7->tvs=tvs;
_TA7->impl=0;_T2F=ad;_TA7->attributes=_T2F->attributes;_TA7->expected_mem_kind=0;_T2C=(struct Cyc_Absyn_Aggrdecl*)_TA7;}
# 537
*_T2B=_T2C;_T30=te;_T31=_T30->ae;_T33=Cyc_Dict_insert;{
# 539
struct Cyc_Dict_Dict(*_TA7)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**))_T33;_T32=_TA7;}_T34=te;_T35=_T34->ae;_T36=_T35->aggrdecls;_T37=q;_T38=adp;_T31->aggrdecls=_T32(_T36,_T37,_T38);
# 544
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);_T39=tagged;
# 548
if(!_T39)goto _TLCF;_T3A=ad;_T3B=_T3A->kind;_T3C=(int)_T3B;if(_T3C!=0)goto _TLCF;{struct Cyc_Warn_String_Warn_Warg_struct _TA7;_TA7.tag=0;
_TA7.f1=_tag_fat("@tagged is allowed only on union declarations",sizeof(char),46U);_T3D=_TA7;}{struct Cyc_Warn_String_Warn_Warg_struct _TA7=_T3D;void*_TA8[1];_TA8[0]=& _TA7;_T3E=loc;_T3F=_tag_fat(_TA8,sizeof(void*),1);Cyc_Warn_err2(_T3E,_T3F);}goto _TLD0;_TLCF: _TLD0: _T41=Cyc_List_map_c;{
struct Cyc_List_List*(*_TA7)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))_T41;_T40=_TA7;}_T42=Cyc_Tcutil_tvar_bool_pair;_T43=tvs;_T44=_T40(_T42,0,_T43);_T46=Cyc_List_map_c;{
struct Cyc_List_List*(*_TA7)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))_T46;_T45=_TA7;}_T47=Cyc_Tcutil_tvar_bool_pair;_T48=exist_vars;_T49=_T45(_T47,1,_T48);{
# 550
struct Cyc_List_List*btvs=Cyc_List_append(_T44,_T49);_T4A=te;_T4B=loc;_T4C=ad;_T4D=_T4C->kind;_T4E=btvs;_T4F=effconstr;_T50=qb;_T51=fs;
# 552
Cyc_Tc_tcAggrImpl(_T4A,_T4B,_T4D,_T4E,_T4F,_T50,_T51);
# 554
Cyc_Tc_rule_out_memkind(loc,q,tvs);
# 557
Cyc_Tc_rule_out_memkind(loc,q,exist_vars);_T52=ad;_T53=_T52->kind;_T54=(int)_T53;
# 560
if(_T54!=1)goto _TLD1;_T55=tagged;if(_T55)goto _TLD1;else{goto _TLD3;}
# 563
_TLD3:{struct Cyc_List_List*f=fs;_TLD7: if(f!=0)goto _TLD5;else{goto _TLD6;}
_TLD5: _T56=Cyc_Flags_tc_aggressive_warn;if(!_T56)goto _TLD8;_T57=f;_T58=_T57->hd;_T59=(struct Cyc_Absyn_Aggrfield*)_T58;_T5A=_T59->type;_T5B=
Cyc_Tcutil_is_bits_only_type(_T5A);
# 564
if(_T5B)goto _TLD8;else{goto _TLDA;}_TLDA: _T5C=f;_T5D=_T5C->hd;_T5E=(struct Cyc_Absyn_Aggrfield*)_T5D;_T5F=_T5E->requires_clause;if(_T5F!=0)goto _TLD8;{struct Cyc_Warn_String_Warn_Warg_struct _TA7;_TA7.tag=0;
# 566
_TA7.f1=_tag_fat("member ",sizeof(char),8U);_T60=_TA7;}{struct Cyc_Warn_String_Warn_Warg_struct _TA7=_T60;{struct Cyc_Warn_String_Warn_Warg_struct _TA8;_TA8.tag=0;_T62=f;_T63=_T62->hd;_T64=(struct Cyc_Absyn_Aggrfield*)_T63;_T65=_T64->name;_TA8.f1=*_T65;_T61=_TA8;}{struct Cyc_Warn_String_Warn_Warg_struct _TA8=_T61;{struct Cyc_Warn_String_Warn_Warg_struct _TA9;_TA9.tag=0;_TA9.f1=_tag_fat(" of union ",sizeof(char),11U);_T66=_TA9;}{struct Cyc_Warn_String_Warn_Warg_struct _TA9=_T66;{struct Cyc_Warn_Qvar_Warn_Warg_struct _TAA;_TAA.tag=1;_TAA.f1=q;_T67=_TAA;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _TAA=_T67;{struct Cyc_Warn_String_Warn_Warg_struct _TAB;_TAB.tag=0;_TAB.f1=_tag_fat(" has type ",sizeof(char),11U);_T68=_TAB;}{struct Cyc_Warn_String_Warn_Warg_struct _TAB=_T68;{struct Cyc_Warn_Typ_Warn_Warg_struct _TAC;_TAC.tag=2;_T6A=f;_T6B=_T6A->hd;_T6C=(struct Cyc_Absyn_Aggrfield*)_T6B;
_TAC.f1=_T6C->type;_T69=_TAC;}{struct Cyc_Warn_Typ_Warn_Warg_struct _TAC=_T69;{struct Cyc_Warn_String_Warn_Warg_struct _TAD;_TAD.tag=0;_TAD.f1=_tag_fat(" so it can only be written and not read",sizeof(char),40U);_T6D=_TAD;}{struct Cyc_Warn_String_Warn_Warg_struct _TAD=_T6D;void*_TAE[7];_TAE[0]=& _TA7;_TAE[1]=& _TA8;_TAE[2]=& _TA9;_TAE[3]=& _TAA;_TAE[4]=& _TAB;_TAE[5]=& _TAC;_TAE[6]=& _TAD;_T6E=loc;_T6F=_tag_fat(_TAE,sizeof(void*),7);Cyc_Warn_warn2(_T6E,_T6F);}}}}}}}goto _TLD9;_TLD8: _TLD9: _T70=f;
# 563
f=_T70->tl;goto _TLD7;_TLD6:;}goto _TLD2;_TLD1: _TLD2: _T71=adp;
# 568
*_T71=ad;goto _LL0;}}_TLCD: _T72=_TA0.f0;{struct Cyc_Absyn_AggrdeclImpl _TA7=*_T72;_TA5=_TA7.exist_vars;_T73=_TA0.f0;_T74=& _T73->qual_bnd;_TA6=(struct Cyc_List_List**)_T74;_TA4=_TA7.fields;_TA3=_TA7.tagged;_TA2=_TA7.effconstr;}_T75=_TA0.f1;{struct Cyc_Absyn_Aggrdecl**_TA7=*_T75;_TA1=_TA7;}{struct Cyc_List_List*exist_vars=_TA5;struct Cyc_List_List**qb=(struct Cyc_List_List**)_TA6;struct Cyc_List_List*fs=_TA4;int tagged=_TA3;struct Cyc_List_List*effconstr=_TA2;struct Cyc_Absyn_Aggrdecl**adp=_TA1;_T76=ad;_T77=_T76->kind;_T78=(int)_T77;_T79=adp;_T7A=*_T79;_T7B=_T7A->kind;_T7C=(int)_T7B;
# 572
if(_T78==_T7C)goto _TLDB;{struct Cyc_Warn_String_Warn_Warg_struct _TA7;_TA7.tag=0;
_TA7.f1=_tag_fat("cannot reuse struct names for unions and vice-versa",sizeof(char),52U);_T7D=_TA7;}{struct Cyc_Warn_String_Warn_Warg_struct _TA7=_T7D;void*_TA8[1];_TA8[0]=& _TA7;_T7E=loc;_T7F=_tag_fat(_TA8,sizeof(void*),1);Cyc_Warn_err2(_T7E,_T7F);}goto _TLDC;_TLDB: _TLDC: _T80=adp;{
# 575
struct Cyc_Absyn_Aggrdecl*ad0=*_T80;_T81=adp;{struct Cyc_Absyn_Aggrdecl*_TA7=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_T83=ad;
# 577
_TA7->kind=_T83->kind;_TA7->sc=3U;_T84=ad;_TA7->name=_T84->name;_TA7->tvs=tvs;
_TA7->impl=0;_T85=ad;_TA7->attributes=_T85->attributes;_TA7->expected_mem_kind=0;_T82=(struct Cyc_Absyn_Aggrdecl*)_TA7;}
# 577
*_T81=_T82;
# 583
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);_T86=tagged;
# 586
if(!_T86)goto _TLDD;_T87=ad;_T88=_T87->kind;_T89=(int)_T88;if(_T89!=0)goto _TLDD;{struct Cyc_Warn_String_Warn_Warg_struct _TA7;_TA7.tag=0;
_TA7.f1=_tag_fat("@tagged is allowed only on union declarations",sizeof(char),46U);_T8A=_TA7;}{struct Cyc_Warn_String_Warn_Warg_struct _TA7=_T8A;void*_TA8[1];_TA8[0]=& _TA7;_T8B=loc;_T8C=_tag_fat(_TA8,sizeof(void*),1);Cyc_Warn_err2(_T8B,_T8C);}goto _TLDE;_TLDD: _TLDE: _T8E=Cyc_List_map_c;{
struct Cyc_List_List*(*_TA7)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))_T8E;_T8D=_TA7;}_T8F=Cyc_Tcutil_tvar_bool_pair;_T90=tvs;_T91=_T8D(_T8F,0,_T90);_T93=Cyc_List_map_c;{
struct Cyc_List_List*(*_TA7)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))_T93;_T92=_TA7;}_T94=Cyc_Tcutil_tvar_bool_pair;_T95=exist_vars;_T96=_T92(_T94,1,_T95);{
# 588
struct Cyc_List_List*btvs=Cyc_List_append(_T91,_T96);_T97=te;_T98=loc;_T99=ad;_T9A=_T99->kind;_T9B=btvs;_T9C=effconstr;_T9D=qb;_T9E=fs;
# 590
Cyc_Tc_tcAggrImpl(_T97,_T98,_T9A,_T9B,_T9C,_T9D,_T9E);_T9F=adp;
# 592
*_T9F=ad0;_TA6=adp;goto _LL8;}}}_TLC8: _LL0:;}}}}struct _tuple17{struct Cyc_Absyn_Tqual f0;void*f1;};
# 608
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _fat_ptr obj,int is_extensible,struct _tuple1*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Datatypefield*_T2;struct Cyc_Absyn_Datatypefield*_T3;unsigned _T4;struct Cyc_Tcenv_Tenv*_T5;struct Cyc_List_List*_T6;struct Cyc_Absyn_Kind*_T7;struct Cyc_Absyn_Kind*_T8;struct Cyc_List_List*_T9;void*_TA;struct _tuple17*_TB;struct _tuple17 _TC;void*_TD;struct Cyc_List_List*_TE;struct Cyc_List_List*_TF;void*_T10;struct _tuple17*_T11;struct _tuple17 _T12;void*_T13;int _T14;struct Cyc_Warn_String_Warn_Warg_struct _T15;struct Cyc_Warn_Qvar_Warn_Warg_struct _T16;struct Cyc_Absyn_Datatypefield*_T17;struct Cyc_Absyn_Datatypefield*_T18;unsigned _T19;struct _fat_ptr _T1A;struct Cyc_List_List*_T1B;void*_T1C;struct _tuple17*_T1D;struct Cyc_Absyn_Datatypefield*_T1E;unsigned _T1F;struct Cyc_List_List*_T20;void*_T21;struct _tuple17*_T22;struct _tuple17 _T23;struct Cyc_Absyn_Tqual _T24;int _T25;struct Cyc_List_List*_T26;void*_T27;struct _tuple17*_T28;struct _tuple17 _T29;void*_T2A;struct Cyc_List_List*_T2B;struct Cyc_List_List*_T2C;int _T2D;struct Cyc_List_List*_T2E;int*_T2F;struct _tuple1*_T30;struct _tuple1 _T31;struct _fat_ptr*_T32;unsigned _T33;struct _fat_ptr*_T34;struct Cyc_List_List*_T35;int _T36;struct Cyc_List_List*_T37;void*_T38;int(*_T39)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T3A)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_T3B)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_T3C;struct Cyc_Absyn_Datatypefield*_T3D;struct _tuple1*_T3E;struct _tuple1 _T3F;struct _fat_ptr*_T40;int _T41;struct Cyc_Warn_String_Warn_Warg_struct _T42;struct Cyc_Warn_Qvar_Warn_Warg_struct _T43;struct Cyc_Absyn_Datatypefield*_T44;struct Cyc_Warn_String_Warn_Warg_struct _T45;struct Cyc_Warn_String_Warn_Warg_struct _T46;struct Cyc_Absyn_Datatypefield*_T47;unsigned _T48;struct _fat_ptr _T49;struct Cyc_List_List*_T4A;struct _RegionHandle*_T4B;struct Cyc_Absyn_Datatypefield*_T4C;struct _tuple1*_T4D;struct _tuple1 _T4E;struct Cyc_Absyn_Datatypefield*_T4F;enum Cyc_Absyn_Scope _T50;int _T51;struct Cyc_Warn_String_Warn_Warg_struct _T52;struct Cyc_Warn_Qvar_Warn_Warg_struct _T53;struct Cyc_Absyn_Datatypefield*_T54;unsigned _T55;struct _fat_ptr _T56;struct Cyc_Absyn_Datatypefield*_T57;struct Cyc_List_List*_T58;{
# 616
struct Cyc_List_List*fs=fields;_TLE2: if(fs!=0)goto _TLE0;else{goto _TLE1;}
_TLE0: _T0=fs;_T1=_T0->hd;{struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_T1;_T2=f;{
struct Cyc_List_List*typs=_T2->typs;_TLE6: if(typs!=0)goto _TLE4;else{goto _TLE5;}
_TLE4: _T3=f;_T4=_T3->loc;_T5=te;_T6=tvs;_T7=& Cyc_Kinds_mk;_T8=(struct Cyc_Absyn_Kind*)_T7;_T9=typs;_TA=_T9->hd;_TB=(struct _tuple17*)_TA;_TC=*_TB;_TD=_TC.f1;Cyc_Tctyp_check_type(_T4,_T5,_T6,_T8,0,0,_TD);_TE=
# 621
Cyc_Tcenv_curr_aquals_bounds(te);_TF=typs;_T10=_TF->hd;_T11=(struct _tuple17*)_T10;_T12=*_T11;_T13=_T12.f1;_T14=Cyc_Tcutil_is_noalias_pointer_or_aggr(_TE,_T13);if(!_T14)goto _TLE7;{struct Cyc_Warn_String_Warn_Warg_struct _T59;_T59.tag=0;
_T59.f1=_tag_fat("noalias pointers in datatypes are not allowed: ",sizeof(char),48U);_T15=_T59;}{struct Cyc_Warn_String_Warn_Warg_struct _T59=_T15;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T5A;_T5A.tag=1;_T17=f;
_T5A.f1=_T17->name;_T16=_T5A;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T5A=_T16;void*_T5B[2];_T5B[0]=& _T59;_T5B[1]=& _T5A;_T18=f;_T19=_T18->loc;_T1A=_tag_fat(_T5B,sizeof(void*),2);Cyc_Warn_err2(_T19,_T1A);}}goto _TLE8;_TLE7: _TLE8: _T1B=typs;_T1C=_T1B->hd;_T1D=(struct _tuple17*)_T1C;_T1E=f;_T1F=_T1E->loc;_T20=typs;_T21=_T20->hd;_T22=(struct _tuple17*)_T21;_T23=*_T22;_T24=_T23.f0;_T25=_T24.print_const;_T26=typs;_T27=_T26->hd;_T28=(struct _tuple17*)_T27;_T29=*_T28;_T2A=_T29.f1;
(*_T1D).f0.real_const=
Cyc_Tcutil_extract_const_from_typedef(_T1F,_T25,_T2A);_T2B=typs;
# 618
typs=_T2B->tl;goto _TLE6;_TLE5:;}}_T2C=fs;
# 616
fs=_T2C->tl;goto _TLE2;_TLE1:;}_T2D=is_extensible;
# 630
if(!_T2D)goto _TLE9;{
# 632
int res=1;_T2E=fields;_T2F=& res;_T30=name;_T31=*_T30;_T32=_T31.f1;_T33=loc;_T34=Cyc_Tc_tc_msg;{
struct Cyc_List_List*fs=Cyc_Tcdecl_sort_xdatatype_fields(_T2E,_T2F,_T32,_T33,_T34);_T36=res;
if(!_T36)goto _TLEB;_T35=fs;goto _TLEC;_TLEB: _T35=0;_TLEC: return _T35;}}_TLE9: {struct _RegionHandle _T59=_new_region(0U,"uprev_rgn");struct _RegionHandle*uprev_rgn=& _T59;_push_region(uprev_rgn);{
# 638
struct Cyc_List_List*prev_fields=0;{
struct Cyc_List_List*fs=fields;_TLF0: if(fs!=0)goto _TLEE;else{goto _TLEF;}
_TLEE: _T37=fs;_T38=_T37->hd;{struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)_T38;_T3A=Cyc_List_mem;{
int(*_T5A)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T3A;_T39=_T5A;}_T3B=Cyc_strptrcmp;_T3C=prev_fields;_T3D=f;_T3E=_T3D->name;_T3F=*_T3E;_T40=_T3F.f1;_T41=_T39(_T3B,_T3C,_T40);if(!_T41)goto _TLF1;{struct Cyc_Warn_String_Warn_Warg_struct _T5A;_T5A.tag=0;
_T5A.f1=_tag_fat("duplicate field ",sizeof(char),17U);_T42=_T5A;}{struct Cyc_Warn_String_Warn_Warg_struct _T5A=_T42;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T5B;_T5B.tag=1;_T44=f;_T5B.f1=_T44->name;_T43=_T5B;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T5B=_T43;{struct Cyc_Warn_String_Warn_Warg_struct _T5C;_T5C.tag=0;_T5C.f1=_tag_fat(" in ",sizeof(char),5U);_T45=_T5C;}{struct Cyc_Warn_String_Warn_Warg_struct _T5C=_T45;{struct Cyc_Warn_String_Warn_Warg_struct _T5D;_T5D.tag=0;_T5D.f1=obj;_T46=_T5D;}{struct Cyc_Warn_String_Warn_Warg_struct _T5D=_T46;void*_T5E[4];_T5E[0]=& _T5A;_T5E[1]=& _T5B;_T5E[2]=& _T5C;_T5E[3]=& _T5D;_T47=f;_T48=_T47->loc;_T49=_tag_fat(_T5E,sizeof(void*),4);Cyc_Warn_err2(_T48,_T49);}}}}goto _TLF2;
# 644
_TLF1: _T4B=uprev_rgn;{struct Cyc_List_List*_T5A=_region_malloc(_T4B,0U,sizeof(struct Cyc_List_List));_T4C=f;_T4D=_T4C->name;_T4E=*_T4D;_T5A->hd=_T4E.f1;_T5A->tl=prev_fields;_T4A=(struct Cyc_List_List*)_T5A;}prev_fields=_T4A;_TLF2: _T4F=f;_T50=_T4F->sc;_T51=(int)_T50;
# 646
if(_T51==2)goto _TLF3;{struct Cyc_Warn_String_Warn_Warg_struct _T5A;_T5A.tag=0;
_T5A.f1=_tag_fat("ignoring scope of field ",sizeof(char),25U);_T52=_T5A;}{struct Cyc_Warn_String_Warn_Warg_struct _T5A=_T52;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T5B;_T5B.tag=1;_T54=f;_T5B.f1=_T54->name;_T53=_T5B;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T5B=_T53;void*_T5C[2];_T5C[0]=& _T5A;_T5C[1]=& _T5B;_T55=loc;_T56=_tag_fat(_T5C,sizeof(void*),2);Cyc_Warn_warn2(_T55,_T56);}}_T57=f;
_T57->sc=2U;goto _TLF4;_TLF3: _TLF4:;}_T58=fs;
# 639
fs=_T58->tl;goto _TLF0;_TLEF:;}{struct Cyc_List_List*_T5A=fields;_npop_handler(0);return _T5A;}}_pop_region();}}struct _tuple18{struct Cyc_Core_Opt*f0;struct Cyc_Absyn_Datatypedecl***f1;};
# 654
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){struct Cyc_Absyn_Datatypedecl*_T0;struct _fat_ptr _T1;struct Cyc_Absyn_Datatypedecl*_T2;int _T3;struct Cyc_Absyn_Datatypedecl*_T4;struct _handler_cons*_T5;int*_T6;int _T7;struct _RegionHandle*_T8;struct Cyc_Tcenv_Tenv*_T9;unsigned _TA;struct Cyc_Absyn_Datatypedecl*_TB;struct _tuple1*_TC;struct Cyc_Absyn_Datatypedecl*_TD;int _TE;struct Cyc_Dict_Absent_exn_struct*_TF;void*_T10;struct Cyc_Absyn_Datatypedecl*_T11;struct Cyc_Absyn_Datatypedecl***_T12;struct Cyc_Absyn_Datatypedecl**_T13;struct Cyc_Absyn_Datatypedecl*_T14;struct Cyc_Absyn_Datatypedecl*_T15;struct _tuple1*_T16;struct Cyc_Tcenv_Tenv*_T17;struct Cyc_List_List*_T18;void*_T19;struct Cyc_Dict_Absent_exn_struct*_T1A;char*_T1B;char*_T1C;struct Cyc_Absyn_Datatypedecl***(*_T1D)(struct Cyc_Dict_Dict,struct _tuple1*);void**(*_T1E)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T1F;struct Cyc_Tcenv_Genv*_T20;struct Cyc_Dict_Dict _T21;struct _tuple1*_T22;struct Cyc_Absyn_Datatypedecl***_T23;struct Cyc_Absyn_Datatypedecl***_T24;unsigned _T25;struct Cyc_Absyn_Datatypedecl***_T26;struct Cyc_Absyn_Datatypedecl***_T27;struct _tuple18 _T28;struct Cyc_Absyn_Datatypedecl*_T29;struct Cyc_Core_Opt*_T2A;struct Cyc_Absyn_Datatypedecl***_T2B;struct Cyc_Tcenv_Tenv*_T2C;struct Cyc_Tcenv_Genv*_T2D;struct Cyc_Dict_Dict(*_T2E)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**);struct Cyc_Dict_Dict(*_T2F)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_T30;struct Cyc_Tcenv_Genv*_T31;struct Cyc_Dict_Dict _T32;struct _tuple1*_T33;struct Cyc_Absyn_Datatypedecl**_T34;struct Cyc_Absyn_Datatypedecl***_T35;struct Cyc_Absyn_Datatypedecl**_T36;struct Cyc_Absyn_Datatypedecl*_T37;struct Cyc_Absyn_Datatypedecl*_T38;unsigned _T39;struct _fat_ptr*_T3A;struct Cyc_Absyn_Datatypedecl**_T3B;struct Cyc_Absyn_Datatypedecl***_T3C;struct Cyc_Core_Opt*_T3D;struct Cyc_Core_Opt*_T3E;void**_T3F;struct Cyc_Absyn_Datatypedecl**_T40;struct Cyc_Absyn_Datatypedecl*_T41;struct Cyc_Absyn_Datatypedecl*_T42;struct Cyc_Absyn_Datatypedecl*_T43;struct Cyc_Tcenv_Tenv*_T44;struct Cyc_Tcenv_Genv*_T45;struct Cyc_Dict_Dict(*_T46)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**);struct Cyc_Dict_Dict(*_T47)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_T48;struct Cyc_Tcenv_Genv*_T49;struct Cyc_Dict_Dict _T4A;struct _tuple1*_T4B;struct Cyc_Absyn_Datatypedecl**_T4C;struct Cyc_List_List**_T4D;struct Cyc_Tcenv_Tenv*_T4E;unsigned _T4F;struct _fat_ptr _T50;struct Cyc_Absyn_Datatypedecl*_T51;int _T52;struct Cyc_Absyn_Datatypedecl*_T53;struct _tuple1*_T54;struct Cyc_List_List**_T55;struct Cyc_List_List*_T56;struct Cyc_List_List*_T57;struct Cyc_Absyn_Datatypedecl*_T58;struct Cyc_Absyn_Datatypedecl**_T59;struct Cyc_Core_Opt*_T5A;struct Cyc_Core_Opt*_T5B;void**_T5C;struct Cyc_Absyn_Datatypedecl***_T5D;struct Cyc_Absyn_Datatypedecl**_T5E;struct Cyc_Absyn_Datatypedecl*_T5F;int _T60;struct Cyc_Absyn_Datatypedecl*_T61;unsigned _T62;struct Cyc_Absyn_Datatypedecl*_T63;int _T64;struct Cyc_Absyn_Datatypedecl*_T65;struct Cyc_Absyn_Datatypedecl**_T66;struct Cyc_Absyn_Datatypedecl*_T67;struct Cyc_Absyn_Datatypedecl*_T68;struct Cyc_Absyn_Datatypedecl*_T69;struct Cyc_List_List**_T6A;struct Cyc_Tcenv_Tenv*_T6B;unsigned _T6C;struct _fat_ptr _T6D;struct Cyc_Absyn_Datatypedecl*_T6E;int _T6F;struct Cyc_Absyn_Datatypedecl*_T70;struct _tuple1*_T71;struct Cyc_List_List**_T72;struct Cyc_List_List*_T73;struct Cyc_List_List*_T74;struct Cyc_Absyn_Datatypedecl*_T75;struct Cyc_Absyn_Datatypedecl**_T76;_T0=tud;{
struct _tuple1*q=_T0->name;_T2=tud;_T3=_T2->is_extensible;
if(!_T3)goto _TLF5;_T1=_tag_fat("@extensible datatype",sizeof(char),21U);goto _TLF6;_TLF5: _T1=_tag_fat("datatype",sizeof(char),9U);_TLF6: {struct _fat_ptr obj=_T1;_T4=tud;{
# 660
struct Cyc_List_List*tvs=_T4->tvs;
# 663
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 669
struct Cyc_Absyn_Datatypedecl***tud_opt;{struct _handler_cons _T77;_T5=& _T77;_push_handler(_T5);{int _T78=0;_T6=_T77.handler;_T7=setjmp(_T6);if(!_T7)goto _TLF7;_T78=1;goto _TLF8;_TLF7: _TLF8: if(_T78)goto _TLF9;else{goto _TLFB;}_TLFB: _T8=Cyc_Core_heap_region;_T9=te;_TA=loc;_TB=tud;_TC=_TB->name;
# 680 "tc.cyc"
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(_T8,_T9,_TA,_TC);
if(tud_opt!=0)goto _TLFC;_TD=tud;_TE=_TD->is_extensible;if(_TE)goto _TLFC;else{goto _TLFE;}_TLFE:{struct Cyc_Dict_Absent_exn_struct*_T79=_cycalloc(sizeof(struct Cyc_Dict_Absent_exn_struct));_T79->tag=Cyc_Dict_Absent;_TF=(struct Cyc_Dict_Absent_exn_struct*)_T79;}_T10=(void*)_TF;_throw(_T10);goto _TLFD;_TLFC: _TLFD:
 if(tud_opt==0)goto _TLFF;_T11=tud;_T12=tud_opt;_T13=*_T12;_T14=*_T13;
_T11->name=_T14->name;goto _TL100;
# 685
_TLFF: _T15=tud;_T16=_T15->name;_T17=te;_T18=_T17->ns;(*_T16).f0=Cyc_Absyn_Abs_n(_T18,0);_TL100: _pop_handler();goto _TLFA;_TLF9: _T19=Cyc_Core_get_exn_thrown();{void*_T79=(void*)_T19;void*_T7A;_T1A=(struct Cyc_Dict_Absent_exn_struct*)_T79;_T1B=_T1A->tag;_T1C=Cyc_Dict_Absent;if(_T1B!=_T1C)goto _TL101;_T1E=Cyc_Dict_lookup_opt;{
# 687
struct Cyc_Absyn_Datatypedecl***(*_T7B)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T1E;_T1D=_T7B;}_T1F=te;_T20=_T1F->ae;_T21=_T20->datatypedecls;_T22=q;{struct Cyc_Absyn_Datatypedecl***tdopt=_T1D(_T21,_T22);_T24=tdopt;_T25=(unsigned)_T24;
if(!_T25)goto _TL103;{struct Cyc_Absyn_Datatypedecl***_T7B=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl**));_T27=tdopt;*_T7B=*_T27;_T26=(struct Cyc_Absyn_Datatypedecl***)_T7B;}_T23=_T26;goto _TL104;_TL103: _T23=0;_TL104: tud_opt=_T23;goto _LL0;}_TL101: _T7A=_T79;{void*exn=_T7A;_rethrow(exn);}_LL0:;}_TLFA:;}}{struct _tuple18 _T77;_T29=tud;
# 694
_T77.f0=_T29->fields;_T77.f1=tud_opt;_T28=_T77;}{struct _tuple18 _T77=_T28;struct Cyc_Absyn_Datatypedecl**_T78;void*_T79;_T2A=_T77.f0;if(_T2A!=0)goto _TL105;_T2B=_T77.f1;if(_T2B!=0)goto _TL107;
# 698
Cyc_Tc_rule_out_mem_and_qual(loc,q,tvs);_T2C=te;_T2D=_T2C->ae;_T2F=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T7A)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**))_T2F;_T2E=_T7A;}_T30=te;_T31=_T30->ae;_T32=_T31->datatypedecls;_T33=q;{struct Cyc_Absyn_Datatypedecl**_T7A=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));*_T7A=tud;_T34=(struct Cyc_Absyn_Datatypedecl**)_T7A;}_T2D->datatypedecls=_T2E(_T32,_T33,_T34);goto _LL5;_TL107: _T35=_T77.f1;{struct Cyc_Absyn_Datatypedecl**_T7A=*_T35;_T79=_T7A;}_LLD:{struct Cyc_Absyn_Datatypedecl**tudp=_T79;_T36=tudp;_T37=*_T36;_T38=tud;_T39=loc;_T3A=Cyc_Tc_tc_msg;{
# 731
struct Cyc_Absyn_Datatypedecl*tud2=Cyc_Tcdecl_merge_datatypedecl(_T37,_T38,_T39,_T3A);
Cyc_Tc_rule_out_mem_and_qual(loc,q,tvs);
if(tud2==0)goto _TL109;_T3B=tudp;
*_T3B=tud2;goto _TL10A;_TL109: _TL10A: goto _LL5;}}_TL105: _T3C=_T77.f1;if(_T3C!=0)goto _TL10B;_T3D=_T77.f0;{struct Cyc_Core_Opt _T7A=*_T3D;_T3E=_T77.f0;_T3F=& _T3E->v;_T79=(struct Cyc_List_List**)_T3F;}{struct Cyc_List_List**fs=_T79;
# 703
struct Cyc_Absyn_Datatypedecl**tudp;tudp=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));_T40=tudp;{struct Cyc_Absyn_Datatypedecl*_T7A=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_T7A->sc=3U;_T42=tud;_T7A->name=_T42->name;_T7A->tvs=tvs;_T7A->fields=0;_T43=tud;
_T7A->is_extensible=_T43->is_extensible;_T41=(struct Cyc_Absyn_Datatypedecl*)_T7A;}
# 703
*_T40=_T41;_T44=te;_T45=_T44->ae;_T47=Cyc_Dict_insert;{
# 705
struct Cyc_Dict_Dict(*_T7A)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**))_T47;_T46=_T7A;}_T48=te;_T49=_T48->ae;_T4A=_T49->datatypedecls;_T4B=q;_T4C=tudp;_T45->datatypedecls=_T46(_T4A,_T4B,_T4C);_T4D=fs;_T4E=te;_T4F=loc;_T50=obj;_T51=tud;_T52=_T51->is_extensible;_T53=tud;_T54=_T53->name;_T55=fs;_T56=*_T55;_T57=tvs;_T58=tud;
# 708
*_T4D=Cyc_Tc_tcDatatypeFields(_T4E,_T4F,_T50,_T52,_T54,_T56,_T57,_T58);
Cyc_Tc_rule_out_mem_and_qual(loc,q,tvs);_T59=tudp;
*_T59=tud;goto _LL5;}_TL10B: _T5A=_T77.f0;{struct Cyc_Core_Opt _T7A=*_T5A;_T5B=_T77.f0;_T5C=& _T5B->v;_T79=(struct Cyc_List_List**)_T5C;}_T5D=_T77.f1;{struct Cyc_Absyn_Datatypedecl**_T7A=*_T5D;_T78=_T7A;}{struct Cyc_List_List**fs=(struct Cyc_List_List**)_T79;struct Cyc_Absyn_Datatypedecl**tudp=_T78;_T5E=tudp;{
# 713
struct Cyc_Absyn_Datatypedecl*tud0=*_T5E;_T5F=tud;_T60=_T5F->is_extensible;
# 716
if(_T60)goto _TL10D;else{goto _TL10F;}_TL10F: _T61=tud0;_T62=(unsigned)_T61;if(!_T62)goto _TL10D;_T63=tud0;_T64=_T63->is_extensible;if(!_T64)goto _TL10D;_T65=tud;
_T65->is_extensible=1;goto _TL10E;_TL10D: _TL10E: _T66=tudp;{struct Cyc_Absyn_Datatypedecl*_T7A=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));
# 719
_T7A->sc=3U;_T68=tud;_T7A->name=_T68->name;_T7A->tvs=tvs;_T7A->fields=0;_T69=tud;
_T7A->is_extensible=_T69->is_extensible;_T67=(struct Cyc_Absyn_Datatypedecl*)_T7A;}
# 719
*_T66=_T67;_T6A=fs;_T6B=te;_T6C=loc;_T6D=obj;_T6E=tud;_T6F=_T6E->is_extensible;_T70=tud;_T71=_T70->name;_T72=fs;_T73=*_T72;_T74=tvs;_T75=tud;
# 723
*_T6A=Cyc_Tc_tcDatatypeFields(_T6B,_T6C,_T6D,_T6F,_T71,_T73,_T74,_T75);_T76=tudp;
# 726
*_T76=tud0;_T79=tudp;goto _LLD;}}_LL5:;}}}}}}
# 739
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){struct Cyc_Absyn_Enumdecl*_T0;struct Cyc_Absyn_Enumdecl*_T1;struct Cyc_Core_Opt*_T2;struct Cyc_Absyn_Enumdecl*_T3;struct Cyc_Core_Opt*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;int(*_T8)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*);int(*_T9)(int(*)(void*,void*),struct Cyc_List_List*,void*);int(*_TA)(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_List_List*_TB;struct Cyc_Absyn_Enumfield*_TC;struct _tuple1*_TD;struct _tuple1 _TE;struct _fat_ptr*_TF;int _T10;struct Cyc_Warn_String_Warn_Warg_struct _T11;struct Cyc_Warn_Qvar_Warn_Warg_struct _T12;struct Cyc_Absyn_Enumfield*_T13;struct Cyc_Absyn_Enumfield*_T14;unsigned _T15;struct _fat_ptr _T16;struct Cyc_List_List*_T17;struct _RegionHandle*_T18;struct Cyc_Absyn_Enumfield*_T19;struct _tuple1*_T1A;struct _tuple1 _T1B;struct _tuple15**(*_T1C)(struct Cyc_Dict_Dict,struct _tuple1*);void**(*_T1D)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T1E;struct Cyc_Tcenv_Genv*_T1F;struct Cyc_Dict_Dict _T20;struct Cyc_Absyn_Enumfield*_T21;struct _tuple1*_T22;struct _tuple15**_T23;struct Cyc_Warn_String_Warn_Warg_struct _T24;struct Cyc_Warn_Qvar_Warn_Warg_struct _T25;struct Cyc_Absyn_Enumfield*_T26;struct Cyc_Warn_String_Warn_Warg_struct _T27;struct Cyc_Absyn_Enumfield*_T28;unsigned _T29;struct _fat_ptr _T2A;struct Cyc_Absyn_Enumfield*_T2B;struct Cyc_Absyn_Exp*_T2C;struct Cyc_Absyn_Enumfield*_T2D;unsigned _T2E;unsigned _T2F;struct Cyc_Absyn_Enumfield*_T30;unsigned _T31;struct Cyc_Absyn_Enumfield*_T32;struct Cyc_Absyn_Exp*_T33;int _T34;struct Cyc_Absyn_Enumfield*_T35;struct Cyc_Absyn_Exp*_T36;struct Cyc_Absyn_Exp*_T37;int _T38;struct Cyc_List_List*_T39;struct _handler_cons*_T3A;int*_T3B;int _T3C;struct Cyc_Absyn_Enumdecl**(*_T3D)(struct Cyc_Dict_Dict,struct _tuple1*);void*(*_T3E)(struct Cyc_Dict_Dict,void*);struct Cyc_Tcenv_Tenv*_T3F;struct Cyc_Tcenv_Genv*_T40;struct Cyc_Dict_Dict _T41;struct _tuple1*_T42;struct Cyc_Absyn_Enumdecl**_T43;struct Cyc_Absyn_Enumdecl*_T44;struct Cyc_Absyn_Enumdecl*_T45;unsigned _T46;struct _fat_ptr*_T47;struct Cyc_Absyn_Enumdecl**_T48;void*_T49;struct Cyc_Dict_Absent_exn_struct*_T4A;char*_T4B;char*_T4C;struct Cyc_Absyn_Enumdecl**_T4D;struct Cyc_Tcenv_Tenv*_T4E;struct Cyc_Tcenv_Genv*_T4F;struct Cyc_Dict_Dict(*_T50)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Enumdecl**);struct Cyc_Dict_Dict(*_T51)(struct Cyc_Dict_Dict,void*,void*);struct Cyc_Tcenv_Tenv*_T52;struct Cyc_Tcenv_Genv*_T53;struct Cyc_Dict_Dict _T54;struct _tuple1*_T55;struct Cyc_Absyn_Enumdecl**_T56;struct Cyc_Absyn_Enumdecl*_T57;struct Cyc_Core_Opt*_T58;struct Cyc_Absyn_Enumdecl*_T59;struct Cyc_Core_Opt*_T5A;void*_T5B;struct Cyc_List_List*_T5C;void*_T5D;struct Cyc_Tcenv_Tenv*_T5E;struct Cyc_Absyn_Enumfield*_T5F;struct Cyc_Absyn_Exp*_T60;struct Cyc_Absyn_Exp*_T61;struct Cyc_Absyn_Enumfield*_T62;struct Cyc_Absyn_Exp*_T63;struct Cyc_Absyn_Exp*_T64;int _T65;struct Cyc_Warn_String_Warn_Warg_struct _T66;struct Cyc_Warn_Qvar_Warn_Warg_struct _T67;struct Cyc_Warn_String_Warn_Warg_struct _T68;struct Cyc_Warn_Qvar_Warn_Warg_struct _T69;struct Cyc_Absyn_Enumfield*_T6A;struct Cyc_Warn_String_Warn_Warg_struct _T6B;unsigned _T6C;struct _fat_ptr _T6D;struct Cyc_List_List*_T6E;_T0=ed;{
struct _tuple1*q=_T0->name;_T1=ed;_T2=_T1->fields;
# 745
if(_T2==0)goto _TL110;{struct _RegionHandle _T6F=_new_region(0U,"uprev_rgn");struct _RegionHandle*uprev_rgn=& _T6F;_push_region(uprev_rgn);{
# 747
struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;_T3=ed;_T4=_T3->fields;_T5=_T4->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T5;_TL115: if(fs!=0)goto _TL113;else{goto _TL114;}
_TL113: _T6=fs;_T7=_T6->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_T7;_T9=Cyc_List_mem;{
# 752
int(*_T70)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))_T9;_T8=_T70;}_TA=Cyc_strptrcmp;_TB=prev_fields;_TC=f;_TD=_TC->name;_TE=*_TD;_TF=_TE.f1;_T10=_T8(_TA,_TB,_TF);if(!_T10)goto _TL116;{struct Cyc_Warn_String_Warn_Warg_struct _T70;_T70.tag=0;
_T70.f1=_tag_fat("duplicate enum constructor ",sizeof(char),28U);_T11=_T70;}{struct Cyc_Warn_String_Warn_Warg_struct _T70=_T11;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T71;_T71.tag=1;_T13=f;_T71.f1=_T13->name;_T12=_T71;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T71=_T12;void*_T72[2];_T72[0]=& _T70;_T72[1]=& _T71;_T14=f;_T15=_T14->loc;_T16=_tag_fat(_T72,sizeof(void*),2);Cyc_Warn_err2(_T15,_T16);}}goto _TL117;
# 755
_TL116: _T18=uprev_rgn;{struct Cyc_List_List*_T70=_region_malloc(_T18,0U,sizeof(struct Cyc_List_List));_T19=f;_T1A=_T19->name;_T1B=*_T1A;_T70->hd=_T1B.f1;_T70->tl=prev_fields;_T17=(struct Cyc_List_List*)_T70;}prev_fields=_T17;_TL117: _T1D=Cyc_Dict_lookup_opt;{
# 757
struct _tuple15**(*_T70)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple15**(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T1D;_T1C=_T70;}_T1E=te;_T1F=_T1E->ae;_T20=_T1F->ordinaries;_T21=f;_T22=_T21->name;_T23=_T1C(_T20,_T22);if(_T23==0)goto _TL118;{struct Cyc_Warn_String_Warn_Warg_struct _T70;_T70.tag=0;
_T70.f1=_tag_fat("enum field name ",sizeof(char),17U);_T24=_T70;}{struct Cyc_Warn_String_Warn_Warg_struct _T70=_T24;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T71;_T71.tag=1;_T26=f;_T71.f1=_T26->name;_T25=_T71;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T71=_T25;{struct Cyc_Warn_String_Warn_Warg_struct _T72;_T72.tag=0;_T72.f1=_tag_fat(" shadows global name",sizeof(char),21U);_T27=_T72;}{struct Cyc_Warn_String_Warn_Warg_struct _T72=_T27;void*_T73[3];_T73[0]=& _T70;_T73[1]=& _T71;_T73[2]=& _T72;_T28=f;_T29=_T28->loc;_T2A=_tag_fat(_T73,sizeof(void*),3);Cyc_Warn_err2(_T29,_T2A);}}}goto _TL119;_TL118: _TL119: _T2B=f;_T2C=_T2B->tag;
if(_T2C!=0)goto _TL11A;_T2D=f;_T2E=tag_count;
tag_count=_T2E + 1;_T2F=_T2E;_T30=f;_T31=_T30->loc;_T2D->tag=Cyc_Absyn_uint_exp(_T2F,_T31);goto _TL11B;
# 762
_TL11A: _T32=f;_T33=_T32->tag;_T34=Cyc_Tcutil_is_const_exp(_T33);if(!_T34)goto _TL11C;_T35=f;_T36=_T35->tag;_T37=
_check_null(_T36);{struct _tuple13 _T70=Cyc_Evexp_eval_const_uint_exp(_T37);int _T71;unsigned _T72;_T72=_T70.f0;_T71=_T70.f1;{unsigned t1=_T72;int known=_T71;_T38=known;
if(!_T38)goto _TL11E;
tag_count=t1 + 1U;goto _TL11F;_TL11E: _TL11F:;}}goto _TL11D;_TL11C: _TL11D: _TL11B:;}_T39=fs;
# 749
fs=_T39->tl;goto _TL115;_TL114:;}}_pop_region();}goto _TL111;_TL110: _TL111:{struct _handler_cons _T6F;_T3A=& _T6F;_push_handler(_T3A);{int _T70=0;_T3B=_T6F.handler;_T3C=setjmp(_T3B);if(!_T3C)goto _TL120;_T70=1;goto _TL121;_TL120: _TL121: if(_T70)goto _TL122;else{goto _TL124;}_TL124: _T3E=Cyc_Dict_lookup;{
# 772
struct Cyc_Absyn_Enumdecl**(*_T71)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple1*))_T3E;_T3D=_T71;}_T3F=te;_T40=_T3F->ae;_T41=_T40->enumdecls;_T42=q;{struct Cyc_Absyn_Enumdecl**edp=_T3D(_T41,_T42);_T43=edp;_T44=*_T43;_T45=ed;_T46=loc;_T47=Cyc_Tc_tc_msg;{
struct Cyc_Absyn_Enumdecl*ed2=Cyc_Tcdecl_merge_enumdecl(_T44,_T45,_T46,_T47);
if(ed2!=0)goto _TL125;_npop_handler(0);return;_TL125: _T48=edp;
*_T48=ed2;}}_pop_handler();goto _TL123;_TL122: _T49=Cyc_Core_get_exn_thrown();{void*_T71=(void*)_T49;void*_T72;_T4A=(struct Cyc_Dict_Absent_exn_struct*)_T71;_T4B=_T4A->tag;_T4C=Cyc_Dict_Absent;if(_T4B!=_T4C)goto _TL127;{
# 777
struct Cyc_Absyn_Enumdecl**edp;edp=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl*));_T4D=edp;*_T4D=ed;_T4E=te;_T4F=_T4E->ae;_T51=Cyc_Dict_insert;{
struct Cyc_Dict_Dict(*_T73)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Enumdecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Enumdecl**))_T51;_T50=_T73;}_T52=te;_T53=_T52->ae;_T54=_T53->enumdecls;_T55=q;_T56=edp;_T4F->enumdecls=_T50(_T54,_T55,_T56);goto _LL3;}_TL127: _T72=_T71;{void*exn=_T72;_rethrow(exn);}_LL3:;}_TL123:;}}_T57=ed;_T58=_T57->fields;
# 783
if(_T58==0)goto _TL129;_T59=ed;_T5A=_T59->fields;_T5B=_T5A->v;{
struct Cyc_List_List*fs=(struct Cyc_List_List*)_T5B;_TL12E: if(fs!=0)goto _TL12C;else{goto _TL12D;}
_TL12C: _T5C=fs;_T5D=_T5C->hd;{struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)_T5D;_T5E=te;_T5F=f;_T60=_T5F->tag;_T61=
_check_null(_T60);Cyc_Tcexp_tcExp(_T5E,0,_T61);_T62=f;_T63=_T62->tag;_T64=
_check_null(_T63);_T65=Cyc_Tcutil_is_const_exp(_T64);if(_T65)goto _TL12F;else{goto _TL131;}
_TL131:{struct Cyc_Warn_String_Warn_Warg_struct _T6F;_T6F.tag=0;_T6F.f1=_tag_fat("enum ",sizeof(char),6U);_T66=_T6F;}{struct Cyc_Warn_String_Warn_Warg_struct _T6F=_T66;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T70;_T70.tag=1;_T70.f1=q;_T67=_T70;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T70=_T67;{struct Cyc_Warn_String_Warn_Warg_struct _T71;_T71.tag=0;_T71.f1=_tag_fat(", field ",sizeof(char),9U);_T68=_T71;}{struct Cyc_Warn_String_Warn_Warg_struct _T71=_T68;{struct Cyc_Warn_Qvar_Warn_Warg_struct _T72;_T72.tag=1;_T6A=f;_T72.f1=_T6A->name;_T69=_T72;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T72=_T69;{struct Cyc_Warn_String_Warn_Warg_struct _T73;_T73.tag=0;
_T73.f1=_tag_fat(": expression is not constant",sizeof(char),29U);_T6B=_T73;}{struct Cyc_Warn_String_Warn_Warg_struct _T73=_T6B;void*_T74[5];_T74[0]=& _T6F;_T74[1]=& _T70;_T74[2]=& _T71;_T74[3]=& _T72;_T74[4]=& _T73;_T6C=loc;_T6D=_tag_fat(_T74,sizeof(void*),5);Cyc_Warn_err2(_T6C,_T6D);}}}}}goto _TL130;_TL12F: _TL130:;}_T6E=fs;
# 784
fs=_T6E->tl;goto _TL12E;_TL12D:;}goto _TL12A;_TL129: _TL12A:;}}
# 793
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include,int in_inc_rep){enum Cyc_Absyn_Scope _T0;int _T1;int _T2;struct Cyc_Warn_String_Warn_Warg_struct _T3;unsigned _T4;struct _fat_ptr _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;unsigned _T7;struct _fat_ptr _T8;int _T9;struct Cyc_Warn_String_Warn_Warg_struct _TA;unsigned _TB;struct _fat_ptr _TC;_T0=sc;_T1=(int)_T0;switch(_T1){case Cyc_Absyn_Static: _T2=in_include;
# 796
if(_T2)goto _TL133;else{goto _TL135;}
_TL135:{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;_TD.f1=_tag_fat("static declaration within extern \"C\"",sizeof(char),37U);_T3=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T3;void*_TE[1];_TE[0]=& _TD;_T4=loc;_T5=_tag_fat(_TE,sizeof(void*),1);Cyc_Warn_warn2(_T4,_T5);}goto _TL134;_TL133: _TL134:
 return 0;case Cyc_Absyn_Abstract:{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;
# 800
_TD.f1=_tag_fat("abstract declaration within extern \"C\"",sizeof(char),39U);_T6=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_T6;void*_TE[1];_TE[0]=& _TD;_T7=loc;_T8=_tag_fat(_TE,sizeof(void*),1);Cyc_Warn_warn2(_T7,_T8);}
return 0;case Cyc_Absyn_Public: goto _LL8;case Cyc_Absyn_Register: _LL8: goto _LLA;case Cyc_Absyn_Extern: _LLA:
# 804
 return 1;case Cyc_Absyn_ExternC: goto _LLE;default: _LLE: _T9=in_inc_rep;
# 807
if(_T9)goto _TL136;else{goto _TL138;}
_TL138:{struct Cyc_Warn_String_Warn_Warg_struct _TD;_TD.tag=0;_TD.f1=_tag_fat("nested extern \"C\" declaration",sizeof(char),30U);_TA=_TD;}{struct Cyc_Warn_String_Warn_Warg_struct _TD=_TA;void*_TE[1];_TE[0]=& _TD;_TB=loc;_TC=_tag_fat(_TE,sizeof(void*),1);Cyc_Warn_warn2(_TB,_TC);}goto _TL137;_TL136: _TL137:
 return 1;};}
# 813
static void Cyc_Tc_set_scopes(struct Cyc_List_List*ovrs,enum Cyc_Absyn_Scope sc){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;int*_T3;unsigned _T4;struct Cyc_Absyn_Vardecl*_T5;struct Cyc_Absyn_Fndecl*_T6;struct Cyc_Absyn_Aggrdecl*_T7;struct Cyc_Absyn_Datatypedecl*_T8;struct Cyc_Absyn_Enumdecl*_T9;struct Cyc_List_List*_TA;
_TL13C: if(ovrs!=0)goto _TL13A;else{goto _TL13B;}
_TL13A: _T0=ovrs;_T1=_T0->hd;_T2=(struct Cyc_Absyn_Decl*)_T1;{void*_TB=_T2->r;struct Cyc_Absyn_Enumdecl*_TC;struct Cyc_Absyn_Datatypedecl*_TD;struct Cyc_Absyn_Aggrdecl*_TE;struct Cyc_Absyn_Fndecl*_TF;struct Cyc_Absyn_Vardecl*_T10;_T3=(int*)_TB;_T4=*_T3;switch(_T4){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_TB;_T10=_T11->f1;}{struct Cyc_Absyn_Vardecl*vd=_T10;_T5=vd;
_T5->sc=sc;goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_TB;_TF=_T11->f1;}{struct Cyc_Absyn_Fndecl*fd=_TF;_T6=fd;
_T6->sc=sc;goto _LL0;}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_TB;_TE=_T11->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_TE;_T7=ad;
_T7->sc=sc;goto _LL0;}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_TB;_TD=_T11->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_TD;_T8=tud;
_T8->sc=sc;goto _LL0;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T11=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_TB;_TC=_T11->f1;}{struct Cyc_Absyn_Enumdecl*ed=_TC;_T9=ed;
_T9->sc=sc;goto _LL0;}default: goto _LL0;}_LL0:;}_TA=ovrs;
# 814
ovrs=_TA->tl;goto _TL13C;_TL13B:;}
# 830
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){struct Cyc_List_List*_T0;void*_T1;struct Cyc_Absyn_Decl*_T2;struct Cyc_Absyn_Decl*_T3;int*_T4;unsigned _T5;struct Cyc_Warn_String_Warn_Warg_struct _T6;unsigned _T7;struct _fat_ptr _T8;struct Cyc_Warn_String_Warn_Warg_struct _T9;unsigned _TA;struct _fat_ptr _TB;int _TC;struct Cyc_Absyn_Decl*_TD;unsigned _TE;struct Cyc_Absyn_Vardecl*_TF;enum Cyc_Absyn_Scope _T10;struct Cyc_Tcenv_Tenv*_T11;int _T12;struct Cyc_Tcenv_Tenv*_T13;int _T14;int _T15;struct Cyc_Absyn_Vardecl*_T16;struct Cyc_Tcenv_Tenv*_T17;unsigned _T18;struct Cyc_Absyn_Vardecl*_T19;int _T1A;struct Cyc_Tcenv_Tenv*_T1B;int _T1C;struct Cyc_List_List**_T1D;int _T1E;struct Cyc_Absyn_Decl*_T1F;unsigned _T20;struct Cyc_Absyn_Fndecl*_T21;enum Cyc_Absyn_Scope _T22;struct Cyc_Tcenv_Tenv*_T23;int _T24;struct Cyc_Tcenv_Tenv*_T25;int _T26;int _T27;struct Cyc_Absyn_Fndecl*_T28;struct Cyc_Tcenv_Tenv*_T29;int _T2A;struct Cyc_Absyn_Fndecl*_T2B;struct Cyc_Absyn_Typedefdecl*_T2C;struct Cyc_Tcenv_Tenv*_T2D;int _T2E;struct Cyc_Absyn_Decl*_T2F;unsigned _T30;struct Cyc_Absyn_Aggrdecl*_T31;enum Cyc_Absyn_Scope _T32;struct Cyc_Tcenv_Tenv*_T33;int _T34;struct Cyc_Tcenv_Tenv*_T35;int _T36;int _T37;struct Cyc_Absyn_Aggrdecl*_T38;int _T39;struct Cyc_Absyn_Decl*_T3A;unsigned _T3B;struct Cyc_Absyn_Datatypedecl*_T3C;enum Cyc_Absyn_Scope _T3D;struct Cyc_Tcenv_Tenv*_T3E;int _T3F;struct Cyc_Tcenv_Tenv*_T40;int _T41;int _T42;struct Cyc_Absyn_Datatypedecl*_T43;int _T44;struct Cyc_Absyn_Decl*_T45;unsigned _T46;struct Cyc_Absyn_Enumdecl*_T47;enum Cyc_Absyn_Scope _T48;struct Cyc_Tcenv_Tenv*_T49;int _T4A;struct Cyc_Tcenv_Tenv*_T4B;int _T4C;int _T4D;struct Cyc_Absyn_Enumdecl*_T4E;struct Cyc_Warn_String_Warn_Warg_struct _T4F;struct Cyc_Absyn_Decl*_T50;unsigned _T51;struct _fat_ptr _T52;struct Cyc_Tcenv_Tenv*_T53;struct Cyc_Tcenv_Tenv*_T54;struct Cyc_List_List*_T55;struct Cyc_List_List*_T56;struct Cyc_Tcenv_Tenv*_T57;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T58;struct Cyc_List_List**_T59;struct Cyc_List_List*_T5A;unsigned _T5B;struct Cyc_Tcenv_Tenv*_T5C;struct Cyc_Tcenv_Tenv*_T5D;struct Cyc_List_List**_T5E;struct Cyc_List_List*_T5F;int _T60;struct Cyc_Absyn_Decl*_T61;unsigned _T62;struct Cyc_Tcenv_Tenv*_T63;struct Cyc_List_List**_T64;struct Cyc_List_List*_T65;struct Cyc_List_List*_T66;struct Cyc_List_List*_T67;struct Cyc_List_List**_T68;unsigned _T69;struct Cyc_List_List**_T6A;struct Cyc_Tcenv_Tenv*_T6B;struct Cyc_Tcenv_Tenv*_T6C;struct Cyc_List_List**_T6D;struct Cyc_List_List*_T6E;int _T6F;struct Cyc_List_List**_T70;struct Cyc_List_List*_T71;void*_T72;struct _tuple14*_T73;struct _tuple14 _T74;int _T75;struct Cyc_Warn_Qvar_Warn_Warg_struct _T76;struct _tuple14*_T77;struct _tuple14 _T78;struct Cyc_Warn_String_Warn_Warg_struct _T79;struct _tuple14*_T7A;struct _tuple14 _T7B;unsigned _T7C;struct _fat_ptr _T7D;struct Cyc_List_List*_T7E;struct Cyc_List_List*_T7F;
# 834
struct Cyc_List_List*ds=ds0;_TL141: if(ds!=0)goto _TL13F;else{goto _TL140;}
_TL13F: _T0=ds;_T1=_T0->hd;{struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)_T1;_T2=d;{
unsigned loc=_T2->loc;_T3=d;{
void*_T80=_T3->r;struct _tuple11*_T81;struct Cyc_List_List*_T82;struct Cyc_List_List*_T83;void*_T84;_T4=(int*)_T80;_T5=*_T4;switch(_T5){case 2: goto _LL4;case 3: _LL4:{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
# 840
_T85.f1=_tag_fat("top level let-declarations are not implemented",sizeof(char),47U);_T6=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T6;void*_T86[1];_T86[0]=& _T85;_T7=loc;_T8=_tag_fat(_T86,sizeof(void*),1);Cyc_Warn_err2(_T7,_T8);}goto _LL0;case 4:{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
# 842
_T85.f1=_tag_fat("top level region declarations are not implemented",sizeof(char),50U);_T9=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T9;void*_T86[1];_T86[0]=& _T85;_TA=loc;_TB=_tag_fat(_T86,sizeof(void*),1);Cyc_Warn_err2(_TA,_TB);}goto _LL0;case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f1;}{struct Cyc_Absyn_Vardecl*vd=_T84;_TC=in_externC;
# 844
if(!_TC)goto _TL143;_TD=d;_TE=_TD->loc;_TF=vd;_T10=_TF->sc;_T11=te;_T12=_T11->in_extern_c_include;_T13=te;_T14=_T13->in_extern_c_inc_repeat;_T15=Cyc_Tc_okay_externC(_TE,_T10,_T12,_T14);if(!_T15)goto _TL143;_T16=vd;
_T16->sc=4U;goto _TL144;_TL143: _TL144: _T17=te;_T18=loc;_T19=vd;_T1A=check_var_init;_T1B=te;_T1C=_T1B->in_extern_c_include;_T1D=exports;
Cyc_Tc_tcVardecl(_T17,_T18,_T19,_T1A,_T1C,_T1D);goto _LL0;}case 1:{struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f1;}{struct Cyc_Absyn_Fndecl*fd=_T84;_T1E=in_externC;
# 849
if(!_T1E)goto _TL145;_T1F=d;_T20=_T1F->loc;_T21=fd;_T22=_T21->sc;_T23=te;_T24=_T23->in_extern_c_include;_T25=te;_T26=_T25->in_extern_c_inc_repeat;_T27=Cyc_Tc_okay_externC(_T20,_T22,_T24,_T26);if(!_T27)goto _TL145;_T28=fd;
_T28->sc=4U;goto _TL146;_TL145: _TL146: _T29=te;_T2A=_T29->in_extern_c_include;
if(!_T2A)goto _TL147;_T2B=fd;
_T2B->orig_scope=4U;goto _TL148;_TL147: _TL148:
 Cyc_Tc_tcFndecl(te,loc,fd,exports);goto _LL0;}case 8:{struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f1;}{struct Cyc_Absyn_Typedefdecl*td=_T84;_T2C=td;_T2D=te;
# 856
_T2C->extern_c=_T2D->in_extern_c_include;
# 860
Cyc_Tc_tcTypedefdecl(te,loc,td);goto _LL0;}case 5:{struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f1;}{struct Cyc_Absyn_Aggrdecl*ad=_T84;_T2E=in_externC;
# 863
if(!_T2E)goto _TL149;_T2F=d;_T30=_T2F->loc;_T31=ad;_T32=_T31->sc;_T33=te;_T34=_T33->in_extern_c_include;_T35=te;_T36=_T35->in_extern_c_inc_repeat;_T37=Cyc_Tc_okay_externC(_T30,_T32,_T34,_T36);if(!_T37)goto _TL149;_T38=ad;
_T38->sc=4U;goto _TL14A;_TL149: _TL14A:
 Cyc_Tc_tcAggrdecl(te,loc,ad);goto _LL0;}case 6:{struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f1;}{struct Cyc_Absyn_Datatypedecl*tud=_T84;_T39=in_externC;
# 868
if(!_T39)goto _TL14B;_T3A=d;_T3B=_T3A->loc;_T3C=tud;_T3D=_T3C->sc;_T3E=te;_T3F=_T3E->in_extern_c_include;_T40=te;_T41=_T40->in_extern_c_inc_repeat;_T42=Cyc_Tc_okay_externC(_T3B,_T3D,_T3F,_T41);if(!_T42)goto _TL14B;_T43=tud;
_T43->sc=4U;goto _TL14C;_TL14B: _TL14C:
 Cyc_Tc_tcDatatypedecl(te,loc,tud);goto _LL0;}case 7:{struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f1;}{struct Cyc_Absyn_Enumdecl*ed=_T84;_T44=in_externC;
# 873
if(!_T44)goto _TL14D;_T45=d;_T46=_T45->loc;_T47=ed;_T48=_T47->sc;_T49=te;_T4A=_T49->in_extern_c_include;_T4B=te;_T4C=_T4B->in_extern_c_inc_repeat;_T4D=Cyc_Tc_okay_externC(_T46,_T48,_T4A,_T4C);if(!_T4D)goto _TL14D;_T4E=ed;
_T4E->sc=4U;goto _TL14E;_TL14D: _TL14E:
 Cyc_Tc_tcEnumdecl(te,loc,ed);goto _LL0;}case 13:{struct Cyc_Warn_String_Warn_Warg_struct _T85;_T85.tag=0;
# 877
_T85.f1=_tag_fat("spurious __cyclone_port_on__",sizeof(char),29U);_T4F=_T85;}{struct Cyc_Warn_String_Warn_Warg_struct _T85=_T4F;void*_T86[1];_T86[0]=& _T85;_T50=d;_T51=_T50->loc;_T52=_tag_fat(_T86,sizeof(void*),1);Cyc_Warn_warn2(_T51,_T52);}goto _LL0;case 14: goto _LL0;case 15:
# 879
 te=Cyc_Tcenv_enter_tempest(te);goto _LL0;case 16:
 te=Cyc_Tcenv_clear_tempest(te);goto _LL0;case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f1;_T83=_T85->f2;}{struct _fat_ptr*v=_T84;struct Cyc_List_List*ds2=_T83;_T53=te;{
# 883
struct Cyc_List_List*ns=_T53->ns;_T54=te;_T55=ns;{struct Cyc_List_List*_T85=_cycalloc(sizeof(struct Cyc_List_List));
_T85->hd=v;_T85->tl=0;_T56=(struct Cyc_List_List*)_T85;}_T54->ns=Cyc_List_append(_T55,_T56);
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);_T57=te;
_T57->ns=ns;goto _LL0;}}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f2;}{struct Cyc_List_List*ds2=_T84;
# 890
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);goto _LL0;}case 11:{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T80;_T84=_T85->f1;}{struct Cyc_List_List*ds2=_T84;
# 893
Cyc_Tc_tc_decls(te,ds2,1,check_var_init,exports);goto _LL0;}default:{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T85=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T80;_T58=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T80;_T59=& _T58->f1;_T84=(struct Cyc_List_List**)_T59;_T83=_T85->f2;_T82=_T85->f3;_T81=_T85->f4;}{struct Cyc_List_List**ds2=(struct Cyc_List_List**)_T84;struct Cyc_List_List*ovrs=_T83;struct Cyc_List_List*exports2=_T82;struct _tuple11*wc=_T81;_T5A=ovrs;_T5B=(unsigned)_T5A;
# 898
if(!_T5B)goto _TL14F;{
struct Cyc_Tcenv_Tenv*tecpy=Cyc_Tcenv_copy_tenv_dicts(te);_T5C=tecpy;
_T5C->in_extern_c_include=1;_T5D=tecpy;_T5E=ds2;_T5F=*_T5E;_T60=check_var_init;
Cyc_Tc_tc_decls(_T5D,_T5F,1,_T60,0);
Cyc_Toc_init();
Cyc_Tc_set_scopes(ovrs,4U);_T61=d;_T62=_T61->loc;_T63=tecpy;_T64=ds2;_T65=ovrs;
Cyc_Cifc_user_overrides(_T62,_T63,_T64,_T65);
Cyc_Toc_finish();}goto _TL150;_TL14F: _TL150: _T66=exports2;_T68=exports;_T69=(unsigned)_T68;
# 908
if(!_T69)goto _TL151;_T6A=exports;_T67=*_T6A;goto _TL152;_TL151: _T67=0;_TL152: {
# 907
struct Cyc_List_List*newexs=
Cyc_List_append(_T66,_T67);
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_enter_extern_c_include(te);_T6B=te2;
_T6B->in_extern_c_inc_repeat=ovrs!=0;_T6C=te2;_T6D=ds2;_T6E=*_T6D;_T6F=check_var_init;_T70=& newexs;
Cyc_Tc_tc_decls(_T6C,_T6E,1,_T6F,_T70);
# 914
_TL156: if(exports2!=0)goto _TL154;else{goto _TL155;}
_TL154: _T71=exports2;_T72=_T71->hd;{struct _tuple14*exp=(struct _tuple14*)_T72;_T73=exp;_T74=*_T73;_T75=_T74.f2;
if(_T75)goto _TL157;else{goto _TL159;}
_TL159:{struct Cyc_Warn_Qvar_Warn_Warg_struct _T85;_T85.tag=1;_T77=exp;_T78=*_T77;_T85.f1=_T78.f1;_T76=_T85;}{struct Cyc_Warn_Qvar_Warn_Warg_struct _T85=_T76;{struct Cyc_Warn_String_Warn_Warg_struct _T86;_T86.tag=0;_T86.f1=_tag_fat(" is exported but not defined",sizeof(char),29U);_T79=_T86;}{struct Cyc_Warn_String_Warn_Warg_struct _T86=_T79;void*_T87[2];_T87[0]=& _T85;_T87[1]=& _T86;_T7A=exp;_T7B=*_T7A;_T7C=_T7B.f0;_T7D=_tag_fat(_T87,sizeof(void*),2);Cyc_Warn_warn2(_T7C,_T7D);}}goto _TL158;_TL157: _TL158:;}_T7E=exports2;
# 914
exports2=_T7E->tl;goto _TL156;_TL155: goto _LL0;}}}_LL0:;}}}_T7F=ds;
# 834
ds=_T7F->tl;goto _TL141;_TL140:;}
# 924
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){struct Cyc_Absynpp_Params*_T0;struct Cyc_Absynpp_Params*_T1;_T0=& Cyc_Absynpp_tc_params_r;_T1=(struct Cyc_Absynpp_Params*)_T0;
Cyc_Absynpp_set_params(_T1);
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 946 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*);
# 948
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){struct Cyc_List_List*(*_T0)(int(*)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*,struct Cyc_List_List*);struct Cyc_List_List*(*_T1)(int(*)(void*,void*),void*,struct Cyc_List_List*);int(*_T2)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*);struct Cyc_Tc_TreeshakeEnv*_T3;struct Cyc_List_List*_T4;struct Cyc_List_List*_T5;_T1=Cyc_List_filter_c;{
struct Cyc_List_List*(*_T6)(int(*)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*,struct Cyc_List_List*))_T1;_T0=_T6;}_T2=Cyc_Tc_vardecl_needed;_T3=env;_T4=ds;_T5=_T0(_T2,_T3,_T4);return _T5;}
# 952
static int Cyc_Tc_is_extern(struct Cyc_Absyn_Vardecl*vd){struct Cyc_Absyn_Vardecl*_T0;enum Cyc_Absyn_Scope _T1;int _T2;struct Cyc_Absyn_Vardecl*_T3;enum Cyc_Absyn_Scope _T4;int _T5;struct Cyc_Absyn_Vardecl*_T6;void*_T7;int*_T8;int _T9;_T0=vd;_T1=_T0->sc;_T2=(int)_T1;
if(_T2==3)goto _TL15C;else{goto _TL15D;}_TL15D: _T3=vd;_T4=_T3->sc;_T5=(int)_T4;if(_T5==4)goto _TL15C;else{goto _TL15A;}
_TL15C: return 1;_TL15A: _T6=vd;_T7=_T6->type;{
void*_TA=Cyc_Absyn_compress(_T7);_T8=(int*)_TA;_T9=*_T8;if(_T9!=6)goto _TL15E;
return 1;_TL15E:
 return 0;;}}
# 961
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){struct Cyc_Absyn_Decl*_T0;int*_T1;unsigned _T2;struct Cyc_Tc_TreeshakeEnv*_T3;int _T4;int _T5;struct Cyc_Absyn_Vardecl*_T6;struct _tuple1*_T7;struct _tuple1*_T8;int _T9;struct _tuple15*(*_TA)(struct Cyc_Dict_Dict,struct _tuple1*);void*(*_TB)(struct Cyc_Dict_Dict,void*);struct Cyc_Tc_TreeshakeEnv*_TC;struct Cyc_Dict_Dict _TD;struct Cyc_Absyn_Vardecl*_TE;struct _tuple1*_TF;struct _tuple15*_T10;struct _tuple15 _T11;int _T12;struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T13;struct Cyc_List_List**_T14;struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T15;struct Cyc_List_List**_T16;struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T17;struct Cyc_List_List**_T18;struct Cyc_List_List**_T19;struct Cyc_Tc_TreeshakeEnv*_T1A;struct Cyc_List_List**_T1B;struct Cyc_List_List*_T1C;struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T1D;struct Cyc_List_List**_T1E;struct Cyc_Tc_TreeshakeEnv*_T1F;struct Cyc_Tc_TreeshakeEnv*_T20;struct Cyc_List_List**_T21;struct Cyc_Tc_TreeshakeEnv*_T22;struct Cyc_List_List**_T23;struct Cyc_List_List*_T24;struct Cyc_Tc_TreeshakeEnv*_T25;_T0=d;{
void*_T26=_T0->r;void*_T27;_T1=(int*)_T26;_T2=*_T1;switch(_T2){case 0:{struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*_T28=(struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_T26;_T27=_T28->f1;}{struct Cyc_Absyn_Vardecl*vd=_T27;_T3=env;_T4=_T3->in_cinclude;
# 965
if(_T4)goto _TL163;else{goto _TL165;}_TL165: _T5=Cyc_Tc_is_extern(vd);if(_T5)goto _TL164;else{goto _TL163;}_TL164: _T6=vd;_T7=_T6->name;_T8=
Cyc_Absyn_uniqueaqual_qvar();_T9=Cyc_Absyn_qvar_cmp(_T7,_T8);
# 965
if(_T9)goto _TL161;else{goto _TL163;}
# 967
_TL163: return 1;_TL161: _TB=Cyc_Dict_lookup;{
struct _tuple15*(*_T28)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple1*))_TB;_TA=_T28;}_TC=env;_TD=_TC->ordinaries;_TE=vd;_TF=_TE->name;_T10=_TA(_TD,_TF);_T11=*_T10;_T12=_T11.f1;return _T12;}case 11:{struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*_T28=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T26;_T13=(struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_T26;_T14=& _T13->f1;_T27=(struct Cyc_List_List**)_T14;}{struct Cyc_List_List**ds2p=_T27;_T27=ds2p;goto _LL6;}case 10:{struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*_T28=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T26;_T15=(struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_T26;_T16=& _T15->f2;_T27=(struct Cyc_List_List**)_T16;}_LL6: {struct Cyc_List_List**ds2p=_T27;_T27=ds2p;goto _LL8;}case 9:{struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*_T28=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T26;_T17=(struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_T26;_T18=& _T17->f2;_T27=(struct Cyc_List_List**)_T18;}_LL8: {struct Cyc_List_List**ds2p=_T27;_T19=ds2p;_T1A=env;_T1B=ds2p;_T1C=*_T1B;
# 971
*_T19=Cyc_Tc_treeshake_f(_T1A,_T1C);return 1;}case 12:{struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*_T28=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T26;_T1D=(struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_T26;_T1E=& _T1D->f1;_T27=(struct Cyc_List_List**)_T1E;}{struct Cyc_List_List**ds2p=(struct Cyc_List_List**)_T27;_T1F=env;{
# 973
int in_cinclude=_T1F->in_cinclude;_T20=env;
_T20->in_cinclude=1;_T21=ds2p;_T22=env;_T23=ds2p;_T24=*_T23;
*_T21=Cyc_Tc_treeshake_f(_T22,_T24);_T25=env;
_T25->in_cinclude=in_cinclude;
return 1;}}default:
 return 1;};}}
# 982
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){struct Cyc_Tc_TreeshakeEnv _T0;struct Cyc_Tcenv_Tenv*_T1;struct Cyc_Tcenv_Genv*_T2;struct Cyc_Tc_TreeshakeEnv*_T3;struct Cyc_Tc_TreeshakeEnv*_T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;{struct Cyc_Tc_TreeshakeEnv _T7;
_T7.in_cinclude=0;_T1=te;_T2=_T1->ae;_T7.ordinaries=_T2->ordinaries;_T0=_T7;}{struct Cyc_Tc_TreeshakeEnv env=_T0;_T3=& env;_T4=(struct Cyc_Tc_TreeshakeEnv*)_T3;_T5=ds;_T6=
Cyc_Tc_treeshake_f(_T4,_T5);return _T6;}}
