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
# 190
extern struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 322
extern int Cyc_List_mem(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 394
extern struct Cyc_List_List*Cyc_List_filter_c(int(*)(void*,void*),void*,struct Cyc_List_List*);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple1{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 161
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 181 "absyn.h"
enum Cyc_Absyn_AliasHint{Cyc_Absyn_UniqueHint =0U,Cyc_Absyn_RefcntHint =1U,Cyc_Absyn_RestrictedHint =2U,Cyc_Absyn_NoHint =3U};
# 187
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_EffKind =3U,Cyc_Absyn_IntKind =4U,Cyc_Absyn_BoolKind =5U,Cyc_Absyn_PtrBndKind =6U,Cyc_Absyn_AqualKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasHint aliashint;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;void*aquals_bound;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_AssnDef_AssnMap*requires_assn;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_AssnDef_AssnMap*ensures_assn;struct Cyc_Absyn_Exp*throws_clause;struct Cyc_AssnDef_AssnMap*throws_assn;struct Cyc_Absyn_Vardecl*return_value;struct Cyc_List_List*arg_vardecls;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct{int tag;void*f1;};struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct{int tag;void*f1;void*f2;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct _tuple9{struct _fat_ptr*f0;struct Cyc_Absyn_Tqual f1;void*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*qual_bnd;struct Cyc_List_List*fields;int tagged;struct Cyc_List_List*effconstr;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f0;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};
# 890 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 901
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*,int);
# 904
struct Cyc_Absyn_Tqual Cyc_Absyn_const_tqual(unsigned);
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 911
void*Cyc_Absyn_compress(void*);
# 924
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*,struct Cyc_Core_Opt*);
# 940
extern void*Cyc_Absyn_al_qual_type;
# 944
extern void*Cyc_Absyn_false_type;
# 974
void*Cyc_Absyn_string_type(void*,void*);
void*Cyc_Absyn_const_string_type(void*,void*);
# 995
void*Cyc_Absyn_fatptr_type(void*,void*,void*,struct Cyc_Absyn_Tqual,void*,void*);
# 1003
void*Cyc_Absyn_array_type(void*,struct Cyc_Absyn_Tqual,struct Cyc_Absyn_Exp*,void*,unsigned);
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1093
struct _tuple1*Cyc_Absyn_uniqueaqual_qvar (void);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple1*);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 49 "flags.h"
extern int Cyc_Flags_tc_aggressive_warn;
# 27 "unify.h"
void Cyc_Unify_explain_failure (void);
# 29
int Cyc_Unify_unify(void*,void*);
# 84 "relations-ap.h"
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*,struct Cyc_Absyn_Exp*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*);
# 39 "tcutil.h"
int Cyc_Tcutil_is_function_type(void*);
# 42
int Cyc_Tcutil_is_array_type(void*);
# 54
int Cyc_Tcutil_is_bits_only_type(void*);
# 84
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 105
int Cyc_Tcutil_coerce_assign(struct Cyc_List_List*,struct Cyc_Absyn_Exp*,void*);
# 156
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*);
# 166
void Cyc_Tcutil_check_bitfield(unsigned,void*,struct Cyc_Absyn_Exp*,struct _fat_ptr*);
# 169
void Cyc_Tcutil_check_unique_tvars(unsigned,struct Cyc_List_List*);
# 202
int Cyc_Tcutil_is_noalias_pointer_or_aggr(struct Cyc_List_List*,void*);
# 217
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*);
# 222
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 226
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int,void*);
# 243
int Cyc_Tcutil_zeroable_type(void*);
# 250
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*);struct _tuple12{struct Cyc_Absyn_Tvar*f0;int f1;};
# 268
struct _tuple12*Cyc_Tcutil_tvar_bool_pair(int,struct Cyc_Absyn_Tvar*);
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
# 83 "dict.h"
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 70 "tcenv.h"
struct Cyc_Tcenv_Fenv*Cyc_Tcenv_new_fenv(unsigned,struct Cyc_Absyn_Fndecl*);
# 79
struct Cyc_Absyn_Datatypedecl***Cyc_Tcenv_lookup_xdatatypedecl(struct _RegionHandle*,struct Cyc_Tcenv_Tenv*,unsigned,struct _tuple1*);
# 83
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_allow_valueof(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_extern_c_include(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_enter_tempest(struct Cyc_Tcenv_Tenv*);
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_clear_tempest(struct Cyc_Tcenv_Tenv*);
# 144
struct Cyc_List_List*Cyc_Tcenv_curr_aquals_bounds(struct Cyc_Tcenv_Tenv*);
# 155
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
# 66
void*Cyc_Tcdecl_merge_binding(void*,void*,unsigned,struct _fat_ptr*);
# 74
struct Cyc_List_List*Cyc_Tcdecl_sort_xdatatype_fields(struct Cyc_List_List*,int*,struct _fat_ptr*,unsigned,struct _fat_ptr*);
# 92 "graph.h"
extern struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict);
# 30 "callgraph.h"
struct Cyc_Dict_Dict Cyc_Callgraph_compute_callgraph(struct Cyc_List_List*);
# 36 "cifc.h"
void Cyc_Cifc_user_overrides(unsigned,struct Cyc_Tcenv_Tenv*,struct Cyc_List_List**,struct Cyc_List_List*);
# 47 "toc.h"
void Cyc_Toc_init (void);
void Cyc_Toc_finish (void);static char _TmpG0[1U]="";
# 46 "tc.cyc"
static struct _fat_ptr Cyc_Tc_tc_msg_c={_TmpG0,_TmpG0,_TmpG0 + 1U};
static struct _fat_ptr*Cyc_Tc_tc_msg=& Cyc_Tc_tc_msg_c;struct _tuple14{unsigned f0;struct _tuple1*f1;int f2;};
# 49
static int Cyc_Tc_export_member(struct _tuple1*x,struct Cyc_List_List*exports){
for(1;exports!=0;exports=exports->tl){
struct _tuple14*p=(struct _tuple14*)exports->hd;
if(!Cyc_Absyn_qvar_cmp(x,(*p).f1)==0)
continue;
# 56
(*p).f2=1;
return 1;}
# 59
return 0;}struct _tuple15{void*f0;int f1;};
# 62
static void Cyc_Tc_tcVardecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Vardecl*vd,int check_var_init,int in_cinclude,struct Cyc_List_List**exports){
# 64
void*_Tmp0;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_Scope _Tmp5;_Tmp5=vd->sc;_Tmp4=vd->name;_Tmp3=vd->type;_Tmp2=vd->initializer;_Tmp1=vd->attributes;_Tmp0=(int*)& vd->is_proto;{enum Cyc_Absyn_Scope sc=_Tmp5;struct _tuple1*q=_Tmp4;void*t=_Tmp3;struct Cyc_Absyn_Exp*initializer=_Tmp2;struct Cyc_List_List*atts=_Tmp1;int*is_proto=(int*)_Tmp0;
# 71
{void*_Tmp6=Cyc_Absyn_compress(t);unsigned _Tmp7;void*_Tmp8;struct Cyc_Absyn_Tqual _Tmp9;void*_TmpA;if(*((int*)_Tmp6)==4){if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.num_elts==0){_TmpA=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.elt_type;_Tmp9=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.tq;_Tmp8=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.zero_term;_Tmp7=((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_Tmp6)->f1.zt_loc;if(initializer!=0){void*telt=_TmpA;struct Cyc_Absyn_Tqual tq=_Tmp9;void*zt=_Tmp8;unsigned ztl=_Tmp7;
# 73
{void*_TmpB=initializer->r;void*_TmpC;struct _fat_ptr _TmpD;switch(*((int*)_TmpB)){case 0: switch(((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TmpB)->f1.Wstring_c.tag){case 8: _TmpD=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TmpB)->f1.String_c.val;{struct _fat_ptr s=_TmpD;
# 75
t=({void*_TmpE=({void*_TmpF=telt;struct Cyc_Absyn_Tqual _Tmp10=tq;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_uint_exp(_get_fat_size(s,sizeof(char)),0U);void*_Tmp12=zt;Cyc_Absyn_array_type(_TmpF,_Tmp10,_Tmp11,_Tmp12,ztl);});vd->type=_TmpE;});goto _LL8;}case 9: _TmpD=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_TmpB)->f1.Wstring_c.val;{struct _fat_ptr s=_TmpD;
# 77
t=({void*_TmpE=({void*_TmpF=telt;struct Cyc_Absyn_Tqual _Tmp10=tq;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_uint_exp(1U,0U);void*_Tmp12=zt;Cyc_Absyn_array_type(_TmpF,_Tmp10,_Tmp11,_Tmp12,ztl);});vd->type=_TmpE;});goto _LL8;}default: goto _LL15;}case 26: _TmpC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_TmpB)->f2;{struct Cyc_Absyn_Exp*e=_TmpC;
_TmpC=e;goto _LL10;}case 27: _TmpC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_TmpB)->f1;_LL10: {struct Cyc_Absyn_Exp*e=_TmpC;
# 80
t=({void*_TmpE=Cyc_Absyn_array_type(telt,tq,e,zt,ztl);vd->type=_TmpE;});goto _LL8;}case 35: _TmpC=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_TmpB)->f2;{struct Cyc_List_List*es=_TmpC;
_TmpC=es;goto _LL14;}case 25: _TmpC=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_TmpB)->f1;_LL14: {struct Cyc_List_List*es=_TmpC;
# 83
t=({void*_TmpE=({void*_TmpF=telt;struct Cyc_Absyn_Tqual _Tmp10=tq;struct Cyc_Absyn_Exp*_Tmp11=Cyc_Absyn_uint_exp((unsigned)Cyc_List_length(es),0U);void*_Tmp12=zt;Cyc_Absyn_array_type(_TmpF,_Tmp10,_Tmp11,_Tmp12,ztl);});vd->type=_TmpE;});
goto _LL8;}default: _LL15:
 goto _LL8;}_LL8:;}
# 87
goto _LL3;}else{goto _LL6;}}else{goto _LL6;}}else{_LL6:
 goto _LL3;}_LL3:;}
# 91
Cyc_Tctyp_check_valid_toplevel_type(loc,te,t);
# 93
({int _Tmp6=Cyc_Tcutil_extract_const_from_typedef(loc,vd->tq.print_const,t);vd->tq.real_const=_Tmp6;});
# 96
({int _Tmp6=!Cyc_Tcutil_is_array_type(t);vd->escapes=_Tmp6;});
# 98
if(Cyc_Tcutil_is_function_type(t)){
*is_proto=0;
atts=Cyc_Atts_transfer_fn_type_atts(t,atts);
Cyc_Atts_fnTypeAttsOK(loc,t);}
# 104
if((int)sc==3 ||(int)sc==4){
if(initializer!=0 && !in_cinclude)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("extern declaration should not have initializer",sizeof(char),47U);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),1));});}else{
if(!Cyc_Tcutil_is_function_type(t)){
# 111
Cyc_Atts_check_variable_atts(loc,vd,atts);
if(initializer==0 || in_cinclude){
if((check_var_init && !in_cinclude)&& !Cyc_Tcutil_zeroable_type(t))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat("initializer required for variable ",sizeof(char),35U);_Tmp7;});struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Vardecl_Warn_Warg_struct _Tmp8;_Tmp8.tag=11,_Tmp8.f1=vd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat(" of type ",sizeof(char),10U);_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)t;_TmpA;});void*_TmpA[4];_TmpA[0]=& _Tmp6,_TmpA[1]=& _Tmp7,_TmpA[2]=& _Tmp8,_TmpA[3]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),4));});}else{
# 119
struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){
{void*t2=Cyc_Tcexp_tcExpInitializer(te,& t,initializer);
if(!({struct Cyc_List_List*_Tmp8=Cyc_Tcenv_curr_aquals_bounds(te);struct Cyc_Absyn_Exp*_Tmp9=initializer;Cyc_Tcutil_coerce_assign(_Tmp8,_Tmp9,t);})){
struct _fat_ptr s0=Cyc_Absynpp_qvar2string(vd->name);
const char*s1=" declared with type ";
struct _fat_ptr s2=Cyc_Absynpp_typ2string(t);
const char*s3=" but initialized with type ";
struct _fat_ptr s4=Cyc_Absynpp_typ2string(t2);
if(({unsigned long _Tmp8=({unsigned long _Tmp9=({unsigned long _TmpA=({unsigned long _TmpB=Cyc_strlen((struct _fat_ptr)s0);_TmpB + Cyc_strlen(_tag_fat(s1,sizeof(char),21U));});_TmpA + Cyc_strlen((struct _fat_ptr)s2);});_Tmp9 + Cyc_strlen(_tag_fat(s3,sizeof(char),28U));});_Tmp8 + Cyc_strlen(s4);})> 70U)
({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=(struct _fat_ptr)s0;_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat(s1,sizeof(char),21U);_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=(struct _fat_ptr)s2;_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(s3,sizeof(char),28U);_TmpC;});struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=(struct _fat_ptr)s4;_TmpD;});void*_TmpD[5];_TmpD[0]=& _Tmp8,_TmpD[1]=& _Tmp9,_TmpD[2]=& _TmpA,_TmpD[3]=& _TmpB,_TmpD[4]=& _TmpC;Cyc_Warn_err(loc,_tag_fat("%s%s\n\t%s\n%s\n\t%s",sizeof(char),16U),_tag_fat(_TmpD,sizeof(void*),5));});else{
# 130
({struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=(struct _fat_ptr)s0;_Tmp9;});struct Cyc_String_pa_PrintArg_struct _Tmp9=({struct Cyc_String_pa_PrintArg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat(s1,sizeof(char),21U);_TmpA;});struct Cyc_String_pa_PrintArg_struct _TmpA=({struct Cyc_String_pa_PrintArg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=(struct _fat_ptr)s2;_TmpB;});struct Cyc_String_pa_PrintArg_struct _TmpB=({struct Cyc_String_pa_PrintArg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(s3,sizeof(char),28U);_TmpC;});struct Cyc_String_pa_PrintArg_struct _TmpC=({struct Cyc_String_pa_PrintArg_struct _TmpD;_TmpD.tag=0,_TmpD.f1=(struct _fat_ptr)s4;_TmpD;});void*_TmpD[5];_TmpD[0]=& _Tmp8,_TmpD[1]=& _Tmp9,_TmpD[2]=& _TmpA,_TmpD[3]=& _TmpB,_TmpD[4]=& _TmpC;Cyc_Warn_err(loc,_tag_fat("%s%s%s%s%s",sizeof(char),11U),_tag_fat(_TmpD,sizeof(void*),5));});}
Cyc_Unify_explain_failure();}
# 134
if(!Cyc_Tcutil_is_const_exp(initializer))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("initializer is not a constant expression",sizeof(char),41U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),1));});}
# 120
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Tcenv_Env_error_exn_struct*)_Tmp8)->tag==Cyc_Tcenv_Env_error){
# 137
({struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=_tag_fat("initializer is not a constant expression",sizeof(char),41U);_TmpB;});void*_TmpB[1];_TmpB[0]=& _TmpA;Cyc_Warn_err2(loc,_tag_fat(_TmpB,sizeof(void*),1));});goto _LL17;}else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}}_LL17:;}}}}else{
# 141
Cyc_Atts_check_fndecl_atts(loc,atts,0);}}
# 144
{struct _handler_cons _Tmp6;_push_handler(& _Tmp6);{int _Tmp7=0;if(setjmp(_Tmp6.handler))_Tmp7=1;if(!_Tmp7){
{struct _tuple15*ans=({struct _tuple15*(*_Tmp8)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup;_Tmp8;})(te->ae->ordinaries,q);
void*b0=(*ans).f0;
struct Cyc_Absyn_Global_b_Absyn_Binding_struct*b1;b1=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct)),b1->tag=1,b1->f1=vd;{
void*b=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);
if(b==0){_npop_handler(0);return;}
# 151
if(exports==0 || Cyc_Tc_export_member(vd->name,*exports)){
if(b!=b0 ||(*ans).f1)
# 154
({struct Cyc_Dict_Dict _Tmp8=({struct Cyc_Dict_Dict(*_Tmp9)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=({struct Cyc_Dict_Dict(*_TmpA)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*))Cyc_Dict_insert;_TmpA;});struct Cyc_Dict_Dict _TmpA=te->ae->ordinaries;struct _tuple1*_TmpB=q;_Tmp9(_TmpA,_TmpB,({struct _tuple15*_TmpC=_cycalloc(sizeof(struct _tuple15));
_TmpC->f0=b,_TmpC->f1=(*ans).f1;_TmpC;}));});
# 154
te->ae->ordinaries=_Tmp8;});}
# 156
_npop_handler(0);return;}}
# 145
;_pop_handler();}else{void*_Tmp8=(void*)Cyc_Core_get_exn_thrown();void*_Tmp9;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp8)->tag==Cyc_Dict_Absent)
# 157
goto _LL1C;else{_Tmp9=_Tmp8;{void*exn=_Tmp9;_rethrow(exn);}}_LL1C:;}}}
if(exports==0 || Cyc_Tc_export_member(vd->name,*exports))
({struct Cyc_Dict_Dict _Tmp6=({struct Cyc_Dict_Dict(*_Tmp7)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*))Cyc_Dict_insert;_Tmp8;});struct Cyc_Dict_Dict _Tmp8=te->ae->ordinaries;struct _tuple1*_Tmp9=q;_Tmp7(_Tmp8,_Tmp9,({struct _tuple15*_TmpA=_cycalloc(sizeof(struct _tuple15));
({void*_TmpB=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Global_b_Absyn_Binding_struct));_TmpC->tag=1,_TmpC->f1=vd;_TmpC;});_TmpA->f0=_TmpB;}),_TmpA->f1=0;_TmpA;}));});
# 159
te->ae->ordinaries=_Tmp6;});}}
# 163
static int Cyc_Tc_is_main(struct _tuple1*n){
void*_Tmp0;union Cyc_Absyn_Nmspace _Tmp1;_Tmp1=n->f0;_Tmp0=n->f1;{union Cyc_Absyn_Nmspace nms=_Tmp1;struct _fat_ptr*v=_Tmp0;
if(nms.Abs_n.tag==2){if(nms.Abs_n.val==0)
return Cyc_strcmp(*v,_tag_fat("main",sizeof(char),5U))==0;else{goto _LL6;}}else{_LL6:
 return 0;};}}
# 171
static void Cyc_Tc_tcFndecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Fndecl*fd,struct Cyc_List_List**exports){
# 173
struct _tuple1*q=fd->name;
# 177
if((int)fd->sc==4 && !te->in_extern_c_include)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("extern \"C\" functions cannot be implemented in Cyclone",sizeof(char),54U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
# 181
Cyc_Atts_check_fndecl_atts(loc,fd->i.attributes,1);
# 184
if(te->in_extern_c_inc_repeat)
fd->cached_type=0;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);{
void*t=Cyc_Tcutil_fndecl2type(fd);
# 189
int nothrow=Cyc_List_mem(Cyc_Atts_attribute_cmp,fd->i.attributes,(void*)& Cyc_Atts_No_throw_att_val);
# 192
({struct Cyc_List_List*_Tmp0=Cyc_Atts_transfer_fn_type_atts(t,fd->i.attributes);fd->i.attributes=_Tmp0;});
Cyc_Atts_fnTypeAttsOK(loc,t);
# 196
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _tuple15*ans=({struct _tuple15*(*_Tmp2)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup;_Tmp2;})(te->ae->ordinaries,q);
void*b0=(*ans).f0;
struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*b1;b1=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct)),b1->tag=2,b1->f1=fd;{
void*b=Cyc_Tcdecl_merge_binding(b0,(void*)b1,loc,Cyc_Tc_tc_msg);
if(b!=0){
# 203
if(exports==0 || Cyc_Tc_export_member(q,*exports)){
if(!(b==b0 &&(*ans).f1))
({struct Cyc_Dict_Dict _Tmp2=({struct Cyc_Dict_Dict(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=({struct Cyc_Dict_Dict(*_Tmp4)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*))Cyc_Dict_insert;_Tmp4;});struct Cyc_Dict_Dict _Tmp4=te->ae->ordinaries;struct _tuple1*_Tmp5=q;_Tmp3(_Tmp4,_Tmp5,({struct _tuple15*_Tmp6=_cycalloc(sizeof(struct _tuple15));
_Tmp6->f0=b,_Tmp6->f1=(*ans).f1;_Tmp6;}));});
# 205
te->ae->ordinaries=_Tmp2;});}}}}
# 197
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 208
if(exports==0 || Cyc_Tc_export_member(fd->name,*exports))
({struct Cyc_Dict_Dict _Tmp4=({struct Cyc_Dict_Dict(*_Tmp5)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=({struct Cyc_Dict_Dict(*_Tmp6)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct _tuple15*))Cyc_Dict_insert;_Tmp6;});struct Cyc_Dict_Dict _Tmp6=te->ae->ordinaries;struct _tuple1*_Tmp7=q;_Tmp5(_Tmp6,_Tmp7,({struct _tuple15*_Tmp8=_cycalloc(sizeof(struct _tuple15));
({void*_Tmp9=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_TmpA=_cycalloc(sizeof(struct Cyc_Absyn_Funname_b_Absyn_Binding_struct));_TmpA->tag=2,_TmpA->f1=fd;_TmpA;});_Tmp8->f0=_Tmp9;}),_Tmp8->f1=0;_Tmp8;}));});
# 209
te->ae->ordinaries=_Tmp4;});
# 211
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}
# 215
if(te->in_extern_c_include)return;{
# 220
struct Cyc_Tcenv_Fenv*fenv=Cyc_Tcenv_new_fenv(loc,fd);
struct Cyc_Tcenv_Tenv*_Tmp0;_Tmp0=_cycalloc(sizeof(struct Cyc_Tcenv_Tenv)),_Tmp0->ns=te->ns,_Tmp0->ae=te->ae,_Tmp0->le=fenv,_Tmp0->allow_valueof=0,_Tmp0->in_extern_c_include=0,_Tmp0->in_tempest=te->in_tempest,_Tmp0->tempest_generalize=te->tempest_generalize,_Tmp0->in_extern_c_inc_repeat=0;{struct Cyc_Tcenv_Tenv*te=_Tmp0;
# 223
Cyc_Tcstmt_tcStmt(te,fd->body,0);
# 226
Cyc_Tcenv_check_delayed_effects(te);
Cyc_Tcenv_check_delayed_constraints(te);
# 229
if(te->in_tempest){
te->tempest_generalize=1;
Cyc_Tctyp_check_fndecl_valid_type(loc,te,fd);
te->tempest_generalize=0;}
# 234
if(nothrow && !Cyc_Tcstmt_ensure_no_throw_stmt(fd->body))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("Function ",sizeof(char),10U);_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=fd->name;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" has attribute no_throw but may throw an exception",sizeof(char),51U);_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;Cyc_Warn_err2(loc,_tag_fat(_Tmp4,sizeof(void*),3));});
# 237
if(Cyc_Tc_is_main(q)){
# 239
{void*_Tmp1=Cyc_Absyn_compress(fd->i.ret_type);if(*((int*)_Tmp1)==0)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)){case 0:
# 241
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("main declared with return type void",sizeof(char),36U);_Tmp3;});void*_Tmp3[1];_Tmp3[0]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),1));});goto _LL5;case 1: switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp1)->f1)->f2){case Cyc_Absyn_Int_sz:
 goto _LLB;case Cyc_Absyn_Long_sz: _LLB:
 goto _LL5;default: goto _LLC;}default: goto _LLC;}else{_LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat("main declared with return type ",sizeof(char),32U);_Tmp3;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp4;_Tmp4.tag=2,_Tmp4.f1=(void*)fd->i.ret_type;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(" instead of int or void",sizeof(char),24U);_Tmp5;});void*_Tmp5[3];_Tmp5[0]=& _Tmp2,_Tmp5[1]=& _Tmp3,_Tmp5[2]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),3));});}_LL5:;}
# 247
if(fd->i.c_varargs || fd->i.cyc_varargs!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("main declared with varargs",sizeof(char),27U);_Tmp2;});void*_Tmp2[1];_Tmp2[0]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),1));});{
struct Cyc_List_List*args=fd->i.args;
if(args!=0){
struct _tuple9*_Tmp1=(struct _tuple9*)args->hd;void*_Tmp2;_Tmp2=_Tmp1->f2;{void*t1=_Tmp2;
{void*_Tmp3=Cyc_Absyn_compress(t1);if(*((int*)_Tmp3)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)==1)switch((int)((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_Tmp3)->f1)->f2){case Cyc_Absyn_Int_sz:
 goto _LL15;case Cyc_Absyn_Long_sz: _LL15:
 goto _LL11;default: goto _LL16;}else{goto _LL16;}}else{_LL16:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("main declared with first argument of type ",sizeof(char),43U);_Tmp5;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6;_Tmp6.tag=2,_Tmp6.f1=(void*)t1;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=
_tag_fat(" instead of int",sizeof(char),16U);_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_Warn_err2(loc,_tag_fat(_Tmp7,sizeof(void*),3));});}_LL11:;}
# 258
args=args->tl;
if(args!=0){
struct _tuple9*_Tmp3=(struct _tuple9*)args->hd;void*_Tmp4;_Tmp4=_Tmp3->f2;{void*t2=_Tmp4;
args=args->tl;
if(args!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("main declared with too many arguments",sizeof(char),38U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});{
struct Cyc_Core_Opt*tvs;tvs=_cycalloc(sizeof(struct Cyc_Core_Opt)),tvs->v=fd->i.tvars;
if(((!({void*_Tmp5=t2;Cyc_Unify_unify(_Tmp5,({void*_Tmp6=({void*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_ek;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});Cyc_Absyn_string_type(_Tmp7,Cyc_Absyn_al_qual_type);});void*_Tmp7=({
struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_ek;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});
# 265
void*_Tmp8=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp9=
# 267
Cyc_Absyn_empty_tqual(0U);
# 265
void*_TmpA=
# 267
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 265
Cyc_Absyn_fatptr_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Absyn_false_type);}));})&& !({
# 268
void*_Tmp5=t2;Cyc_Unify_unify(_Tmp5,({void*_Tmp6=({void*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_ek;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});Cyc_Absyn_const_string_type(_Tmp7,Cyc_Absyn_al_qual_type);});void*_Tmp7=({
struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_ek;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});
# 268
void*_Tmp8=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp9=
# 270
Cyc_Absyn_empty_tqual(0U);
# 268
void*_TmpA=
# 270
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 268
Cyc_Absyn_fatptr_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Absyn_false_type);}));}))&& !({
# 271
void*_Tmp5=t2;Cyc_Unify_unify(_Tmp5,({void*_Tmp6=({void*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_ek;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});Cyc_Absyn_string_type(_Tmp7,Cyc_Absyn_al_qual_type);});void*_Tmp7=({
struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_ek;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});
# 271
void*_Tmp8=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp9=
# 273
Cyc_Absyn_const_tqual(0U);
# 271
void*_TmpA=
# 273
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 271
Cyc_Absyn_fatptr_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Absyn_false_type);}));}))&& !({
# 274
void*_Tmp5=t2;Cyc_Unify_unify(_Tmp5,({void*_Tmp6=({void*_Tmp7=({struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_ek;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});Cyc_Absyn_const_string_type(_Tmp7,Cyc_Absyn_al_qual_type);});void*_Tmp7=({
struct Cyc_Core_Opt*_Tmp8=({struct Cyc_Core_Opt*_Tmp9=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp9->v=& Cyc_Kinds_ek;_Tmp9;});Cyc_Absyn_new_evar(_Tmp8,tvs);});
# 274
void*_Tmp8=Cyc_Absyn_al_qual_type;struct Cyc_Absyn_Tqual _Tmp9=
# 276
Cyc_Absyn_const_tqual(0U);
# 274
void*_TmpA=
# 276
Cyc_Tcutil_any_bool((struct Cyc_List_List*)tvs->v);
# 274
Cyc_Absyn_fatptr_type(_Tmp6,_Tmp7,_Tmp8,_Tmp9,_TmpA,Cyc_Absyn_false_type);}));}))
# 277
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("second argument of main has type ",sizeof(char),34U);_Tmp6;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp7;_Tmp7.tag=2,_Tmp7.f1=(void*)t2;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(" instead of char??",sizeof(char),19U);_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),3));});}}}}}}}}}}}
# 285
static void Cyc_Tc_tcTypedefdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Typedefdecl*td){
struct _tuple1*q=td->name;
# 291
if(({int(*_Tmp0)(struct Cyc_Dict_Dict,struct _tuple1*)=(int(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_member;_Tmp0;})(te->ae->typedefs,q)){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("redeclaration of typedef ",sizeof(char),26U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=q;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(loc,_tag_fat(_Tmp2,sizeof(void*),2));});
return;}
# 296
Cyc_Tcutil_check_unique_tvars(loc,td->tvs);
Cyc_Tcutil_add_tvar_identities(td->tvs);
if(td->defn!=0){
Cyc_Tctyp_check_type(loc,te,td->tvs,& Cyc_Kinds_ak,0,1,td->defn);
({int _Tmp0=
Cyc_Tcutil_extract_const_from_typedef(loc,td->tq.print_const,_check_null(td->defn));
# 300
td->tq.real_const=_Tmp0;});}
# 305
{struct Cyc_List_List*tvs=td->tvs;for(0;tvs!=0;tvs=tvs->tl){
void*_Tmp0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs->hd)->kind);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp2;
# 308
if(td->defn!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("type variable ",sizeof(char),15U);_Tmp4;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp5;_Tmp5.tag=7,_Tmp5.f1=(struct Cyc_Absyn_Tvar*)tvs->hd;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" is not used in typedef",sizeof(char),24U);_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_Warn_warn2(loc,_tag_fat(_Tmp6,sizeof(void*),3));});
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});goto _LL0;}case 2: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp2;struct Cyc_Absyn_Kind*k=_Tmp1;
# 317
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(k);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});
goto _LL0;}default:
 continue;}_LL0:;}}
# 322
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Typedefdecl*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Typedefdecl*))Cyc_Dict_insert;_Tmp1;})(te->ae->typedefs,q,td);te->ae->typedefs=_Tmp0;});}
# 325
static void Cyc_Tc_tcAggrImpl(struct Cyc_Tcenv_Tenv*te,unsigned loc,enum Cyc_Absyn_AggrKind str_or_union,struct Cyc_List_List*flagged_tvs,struct Cyc_List_List*effc,struct Cyc_List_List**qb,struct Cyc_List_List*fields){
# 333
struct _RegionHandle _Tmp0=_new_region(0U,"uprev_rgn");struct _RegionHandle*uprev_rgn=& _Tmp0;_push_region(uprev_rgn);
{struct Cyc_List_List*tvs=({struct Cyc_List_List*(*_Tmp1)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*),struct Cyc_List_List*))Cyc_List_map;_Tmp1;})(({struct Cyc_Absyn_Tvar*(*_Tmp1)(struct _tuple12*)=(struct Cyc_Absyn_Tvar*(*)(struct _tuple12*))Cyc_Core_fst;_Tmp1;}),flagged_tvs);
for(1;effc!=0;effc=effc->tl){
void*_Tmp1=(void*)effc->hd;void*_Tmp2;void*_Tmp3;switch(*((int*)_Tmp1)){case 1: _Tmp3=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp1)->f1;_Tmp2=(void*)((struct Cyc_Absyn_DisjointConstraint_Absyn_EffConstraint_struct*)_Tmp1)->f2;{void*e1=_Tmp3;void*e2=_Tmp2;
_Tmp3=e1;_Tmp2=e2;goto _LL4;}case 2: _Tmp3=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_Tmp1)->f1;_Tmp2=(void*)((struct Cyc_Absyn_SubsetConstraint_Absyn_EffConstraint_struct*)_Tmp1)->f2;_LL4: {void*e1=_Tmp3;void*e2=_Tmp2;
# 339
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Kinds_ek,0,0,e1);
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Kinds_ek,0,0,e2);
goto _LL0;}default: _Tmp3=(void*)((struct Cyc_Absyn_SingleConstraint_Absyn_EffConstraint_struct*)_Tmp1)->f1;{void*e1=_Tmp3;
# 343
Cyc_Tctyp_check_type(loc,te,tvs,& Cyc_Kinds_ek,0,0,e1);
goto _LL0;}}_LL0:;}{
# 348
struct Cyc_List_List*prev_fields=0;
struct Cyc_List_List*prev_relations=0;
# 351
struct Cyc_List_List*fs=fields;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_Tmp1=(struct Cyc_Absyn_Aggrfield*)fs->hd;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;struct Cyc_Absyn_Tqual _Tmp6;void*_Tmp7;_Tmp7=_Tmp1->name;_Tmp6=_Tmp1->tq;_Tmp5=_Tmp1->type;_Tmp4=_Tmp1->width;_Tmp3=_Tmp1->attributes;_Tmp2=_Tmp1->requires_clause;{struct _fat_ptr*fn=_Tmp7;struct Cyc_Absyn_Tqual tq=_Tmp6;void*t=_Tmp5;struct Cyc_Absyn_Exp*width=_Tmp4;struct Cyc_List_List*atts=_Tmp3;struct Cyc_Absyn_Exp*requires_clause=_Tmp2;
# 354
if(({int(*_Tmp8)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;_Tmp8;})(Cyc_strptrcmp,prev_fields,fn))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("duplicate member ",sizeof(char),18U);_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=*fn;_TmpA;});void*_TmpA[2];_TmpA[0]=& _Tmp8,_TmpA[1]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),2));});
# 358
if(Cyc_strcmp(*fn,_tag_fat("",sizeof(char),1U))!=0)
prev_fields=({struct Cyc_List_List*_Tmp8=_region_malloc(uprev_rgn,0U,sizeof(struct Cyc_List_List));_Tmp8->hd=fn,_Tmp8->tl=prev_fields;_Tmp8;});{
# 361
struct Cyc_Absyn_Kind*field_kind=& Cyc_Kinds_mk;
# 365
if((int)str_or_union==1 ||
 fs->tl==0 &&(int)str_or_union==0)
field_kind=& Cyc_Kinds_ak;
Cyc_Tctyp_check_type_with_bounds(loc,te,flagged_tvs,qb,field_kind,0,0,t);
# 370
({int _Tmp8=Cyc_Tcutil_extract_const_from_typedef(loc,((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq.print_const,t);((struct Cyc_Absyn_Aggrfield*)fs->hd)->tq.real_const=_Tmp8;});
# 373
Cyc_Tcutil_check_bitfield(loc,t,width,fn);
# 375
if((unsigned)requires_clause){
if((int)str_or_union!=1)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("@requires clauses are allowed only on union members",sizeof(char),52U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),1));});{
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_allow_valueof(te);
Cyc_Tcexp_tcExp(te2,0,requires_clause);
if(!Cyc_Tcutil_is_integral(requires_clause))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=
_tag_fat("@requires clause has type ",sizeof(char),27U);_Tmp9;});struct Cyc_Warn_Typ_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpA;_TmpA.tag=2,_TmpA.f1=(void*)_check_null(requires_clause->topt);_TmpA;});struct Cyc_Warn_String_Warn_Warg_struct _TmpA=({struct Cyc_Warn_String_Warn_Warg_struct _TmpB;_TmpB.tag=0,_TmpB.f1=
_tag_fat(" instead of integral type",sizeof(char),26U);_TmpB;});void*_TmpB[3];_TmpB[0]=& _Tmp8,_TmpB[1]=& _Tmp9,_TmpB[2]=& _TmpA;Cyc_Warn_err2(requires_clause->loc,_tag_fat(_TmpB,sizeof(void*),3));});else{
# 385
({unsigned _Tmp8=requires_clause->loc;struct Cyc_Tcenv_Tenv*_Tmp9=te;struct Cyc_List_List*_TmpA=flagged_tvs;struct Cyc_List_List**_TmpB=qb;Cyc_Tctyp_check_type_with_bounds(_Tmp8,_Tmp9,_TmpA,_TmpB,& Cyc_Kinds_ik,0,0,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_ValueofType_Absyn_Type_struct));
_TmpC->tag=8,_TmpC->f1=requires_clause;_TmpC;}));});{
# 388
struct Cyc_List_List*relns=Cyc_Relations_exp2relns(uprev_rgn,requires_clause);
# 397 "tc.cyc"
{struct Cyc_List_List*p=prev_relations;for(0;p!=0;p=p->tl){
if(Cyc_Relations_consistent_relations(Cyc_List_rappend(uprev_rgn,relns,(struct Cyc_List_List*)p->hd)))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=
_tag_fat("@requires clause may overlap with previous clauses",sizeof(char),51U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(requires_clause->loc,_tag_fat(_Tmp9,sizeof(void*),1));});}}
prev_relations=({struct Cyc_List_List*_Tmp8=_region_malloc(uprev_rgn,0U,sizeof(struct Cyc_List_List));_Tmp8->hd=relns,_Tmp8->tl=prev_relations;_Tmp8;});}}}}else{
# 404
if(prev_relations!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=_tag_fat("if one field has a @requires clause, they all must",sizeof(char),51U);_Tmp9;});void*_Tmp9[1];_Tmp9[0]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),1));});}}}}}}
# 334 "tc.cyc"
;_pop_region();}
# 409 "tc.cyc"
static void Cyc_Tc_rule_out_memkind(unsigned loc,struct _tuple1*n,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
void*_Tmp0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);void*_Tmp1;enum Cyc_Absyn_AliasHint _Tmp2;void*_Tmp3;switch(*((int*)_Tmp0)){case 1: _Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp3;
# 413
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});continue;}case 2: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->kind==Cyc_Absyn_MemKind){_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp2=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2->aliashint;{struct Cyc_Core_Opt**f=_Tmp3;enum Cyc_Absyn_AliasHint a=_Tmp2;
# 418
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});
continue;}}else{_Tmp3=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_Tmp1=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp3;struct Cyc_Absyn_Kind*k=_Tmp1;
# 424
({struct Cyc_Core_Opt*_Tmp4=({struct Cyc_Core_Opt*_Tmp5=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp6=Cyc_Kinds_kind_to_bound(k);_Tmp5->v=_Tmp6;});_Tmp5;});*f=_Tmp4;});continue;}}default: if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind==Cyc_Absyn_MemKind){_Tmp2=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1->aliashint;{enum Cyc_Absyn_AliasHint a=_Tmp2;
# 426
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=_tag_fat("type ",sizeof(char),6U);_Tmp5;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=n;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" attempts to abstract type variable ",sizeof(char),37U);_Tmp7;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp8;_Tmp8.tag=7,_Tmp8.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=
_tag_fat(" of kind ",sizeof(char),10U);_Tmp9;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_Kind_Warn_Warg_struct _TmpA;_TmpA.tag=9,({struct Cyc_Absyn_Kind*_TmpB=({struct Cyc_Absyn_Kind*_TmpC=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_TmpC->kind=1U,_TmpC->aliashint=a;_TmpC;});_TmpA.f1=_TmpB;});_TmpA;});void*_TmpA[6];_TmpA[0]=& _Tmp4,_TmpA[1]=& _Tmp5,_TmpA[2]=& _Tmp6,_TmpA[3]=& _Tmp7,_TmpA[4]=& _Tmp8,_TmpA[5]=& _Tmp9;Cyc_Warn_err2(loc,_tag_fat(_TmpA,sizeof(void*),6));});
continue;}}else{
continue;}};}}
# 433
static void Cyc_Tc_rule_out_mem_and_qual(unsigned loc,struct _tuple1*q,struct Cyc_List_List*tvs){
struct Cyc_List_List*tvs2=tvs;for(0;tvs2!=0;tvs2=tvs2->tl){
void*_Tmp0=Cyc_Kinds_compress_kb(((struct Cyc_Absyn_Tvar*)tvs2->hd)->kind);enum Cyc_Absyn_AliasHint _Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 1: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp2;
_Tmp2=f;goto _LL4;}case 2: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f2)->kind){case Cyc_Absyn_MemKind: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;_LL4: {struct Cyc_Core_Opt**f=_Tmp2;
# 439
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_bk);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});goto _LL0;}case Cyc_Absyn_AnyKind: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=_Tmp2;
# 441
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ak);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});goto _LL0;}case Cyc_Absyn_EffKind: _Tmp2=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_Tmp0)->f1;{struct Cyc_Core_Opt**f=(struct Cyc_Core_Opt**)_Tmp2;
# 443
({struct Cyc_Core_Opt*_Tmp3=({struct Cyc_Core_Opt*_Tmp4=_cycalloc(sizeof(struct Cyc_Core_Opt));({void*_Tmp5=Cyc_Kinds_kind_to_bound(& Cyc_Kinds_ek);_Tmp4->v=_Tmp5;});_Tmp4;});*f=_Tmp3;});goto _LL0;}default: goto _LLD;}default: switch((int)((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1)->kind){case Cyc_Absyn_AqualKind:
# 445
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("type ",sizeof(char),6U);_Tmp4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=q;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" attempts to abstract type variable ",sizeof(char),37U);_Tmp6;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=7,_Tmp7.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(" of kind Q",sizeof(char),11U);_Tmp8;});void*_Tmp8[5];_Tmp8[0]=& _Tmp3,_Tmp8[1]=& _Tmp4,_Tmp8[2]=& _Tmp5,_Tmp8[3]=& _Tmp6,_Tmp8[4]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),5));});goto _LL0;case Cyc_Absyn_MemKind: _Tmp1=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_Tmp0)->f1->aliashint;{enum Cyc_Absyn_AliasHint a=_Tmp1;
# 448
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat("type ",sizeof(char),6U);_Tmp4;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5;_Tmp5.tag=1,_Tmp5.f1=q;_Tmp5;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat(" attempts to abstract type variable ",sizeof(char),37U);_Tmp6;});struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_Tvar_Warn_Warg_struct _Tmp7;_Tmp7.tag=7,_Tmp7.f1=(struct Cyc_Absyn_Tvar*)tvs2->hd;_Tmp7;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=
_tag_fat(" of kind ",sizeof(char),10U);_Tmp8;});struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_Kind_Warn_Warg_struct _Tmp9;_Tmp9.tag=9,({struct Cyc_Absyn_Kind*_TmpA=({struct Cyc_Absyn_Kind*_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Kind));_TmpB->kind=1U,_TmpB->aliashint=a;_TmpB;});_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[6];_Tmp9[0]=& _Tmp3,_Tmp9[1]=& _Tmp4,_Tmp9[2]=& _Tmp5,_Tmp9[3]=& _Tmp6,_Tmp9[4]=& _Tmp7,_Tmp9[5]=& _Tmp8;Cyc_Warn_err2(loc,_tag_fat(_Tmp9,sizeof(void*),6));});goto _LL0;}default: _LLD:
 goto _LL0;}}_LL0:;}}struct _tuple16{struct Cyc_Absyn_AggrdeclImpl*f0;struct Cyc_Absyn_Aggrdecl***f1;};
# 456
void Cyc_Tc_tcAggrdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Aggrdecl*ad){
struct _tuple1*q=ad->name;
# 462
Cyc_Atts_check_field_atts(loc,(*q).f1,ad->attributes);{
# 464
struct Cyc_List_List*tvs=ad->tvs;
# 467
Cyc_Tcutil_check_unique_tvars(loc,ad->tvs);
Cyc_Tcutil_add_tvar_identities(ad->tvs);{
# 472
struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=ad->impl,({struct Cyc_Absyn_Aggrdecl***_Tmp2=({struct Cyc_Absyn_Aggrdecl***(*_Tmp3)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct Cyc_Absyn_Aggrdecl***(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;_Tmp3;})(te->ae->aggrdecls,q);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;if(_Tmp0.f0==0){if(_Tmp0.f1==0){
# 475
Cyc_Tc_rule_out_memkind(loc,q,tvs);
# 477
({struct Cyc_Dict_Dict _Tmp7=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**)=({struct Cyc_Dict_Dict(*_Tmp9)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**))Cyc_Dict_insert;_Tmp9;});struct Cyc_Dict_Dict _Tmp9=te->ae->aggrdecls;struct _tuple1*_TmpA=q;_Tmp8(_Tmp9,_TmpA,({struct Cyc_Absyn_Aggrdecl**_TmpB=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*));*_TmpB=ad;_TmpB;}));});te->ae->aggrdecls=_Tmp7;});
goto _LL0;}else{_Tmp6=*_Tmp0.f1;_LL8: {struct Cyc_Absyn_Aggrdecl**adp=_Tmp6;
# 543
struct Cyc_Absyn_Aggrdecl*ad2=Cyc_Tcdecl_merge_aggrdecl(*adp,ad,loc,Cyc_Tc_tc_msg);
if(ad2==0)
return;
Cyc_Tc_rule_out_memkind(loc,q,tvs);
# 551
*adp=ad2;}}}else{if(_Tmp0.f1==0){_Tmp6=_Tmp0.f0->exist_vars;_Tmp5=(struct Cyc_List_List**)& _Tmp0.f0->qual_bnd;_Tmp4=_Tmp0.f0->fields;_Tmp3=_Tmp0.f0->tagged;_Tmp2=_Tmp0.f0->effconstr;{struct Cyc_List_List*exist_vars=_Tmp6;struct Cyc_List_List**qb=_Tmp5;struct Cyc_List_List*fs=_Tmp4;int tagged=_Tmp3;struct Cyc_List_List*effconstr=_Tmp2;
# 484
struct Cyc_Absyn_Aggrdecl**adp;adp=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl*)),({struct Cyc_Absyn_Aggrdecl*_Tmp7=({struct Cyc_Absyn_Aggrdecl*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp8->kind=ad->kind,_Tmp8->sc=3U,_Tmp8->name=ad->name,_Tmp8->tvs=tvs,_Tmp8->impl=0,_Tmp8->attributes=ad->attributes,_Tmp8->expected_mem_kind=0;_Tmp8;});*adp=_Tmp7;});
# 486
({struct Cyc_Dict_Dict _Tmp7=({struct Cyc_Dict_Dict(*_Tmp8)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Aggrdecl**))Cyc_Dict_insert;_Tmp8;})(te->ae->aggrdecls,q,adp);te->ae->aggrdecls=_Tmp7;});
# 491
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 495
if(tagged &&(int)ad->kind==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("@tagged is allowed only on union declarations",sizeof(char),46U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),1));});{
struct Cyc_List_List*btvs=({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*(*_Tmp8)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Tcutil_tvar_bool_pair,0,tvs);Cyc_List_append(_Tmp7,
({struct Cyc_List_List*(*_Tmp8)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Tcutil_tvar_bool_pair,1,exist_vars));});
Cyc_Tc_tcAggrImpl(te,loc,ad->kind,btvs,effconstr,qb,fs);
# 501
Cyc_Tc_rule_out_memkind(loc,q,tvs);
# 504
Cyc_Tc_rule_out_memkind(loc,q,exist_vars);
# 507
if((int)ad->kind==1 && !tagged){
# 510
struct Cyc_List_List*f=fs;for(0;f!=0;f=f->tl){
if((Cyc_Flags_tc_aggressive_warn && !
Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)f->hd)->type))&&((struct Cyc_Absyn_Aggrfield*)f->hd)->requires_clause==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("member ",sizeof(char),8U);_Tmp8;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp8=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp9;_Tmp9.tag=0,_Tmp9.f1=*((struct Cyc_Absyn_Aggrfield*)f->hd)->name;_Tmp9;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp9=({struct Cyc_Warn_String_Warn_Warg_struct _TmpA;_TmpA.tag=0,_TmpA.f1=_tag_fat(" of union ",sizeof(char),11U);_TmpA;});struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpA=({struct Cyc_Warn_Qvar_Warn_Warg_struct _TmpB;_TmpB.tag=1,_TmpB.f1=q;_TmpB;});struct Cyc_Warn_String_Warn_Warg_struct _TmpB=({struct Cyc_Warn_String_Warn_Warg_struct _TmpC;_TmpC.tag=0,_TmpC.f1=_tag_fat(" has type ",sizeof(char),11U);_TmpC;});struct Cyc_Warn_Typ_Warn_Warg_struct _TmpC=({struct Cyc_Warn_Typ_Warn_Warg_struct _TmpD;_TmpD.tag=2,_TmpD.f1=(void*)((struct Cyc_Absyn_Aggrfield*)f->hd)->type;_TmpD;});struct Cyc_Warn_String_Warn_Warg_struct _TmpD=({struct Cyc_Warn_String_Warn_Warg_struct _TmpE;_TmpE.tag=0,_TmpE.f1=
_tag_fat(" so it can only be written and not read",sizeof(char),40U);_TmpE;});void*_TmpE[7];_TmpE[0]=& _Tmp7,_TmpE[1]=& _Tmp8,_TmpE[2]=& _Tmp9,_TmpE[3]=& _TmpA,_TmpE[4]=& _TmpB,_TmpE[5]=& _TmpC,_TmpE[6]=& _TmpD;Cyc_Warn_warn2(loc,_tag_fat(_TmpE,sizeof(void*),7));});}}
*adp=ad;
goto _LL0;}}}else{_Tmp6=_Tmp0.f0->exist_vars;_Tmp5=(struct Cyc_List_List**)& _Tmp0.f0->qual_bnd;_Tmp4=_Tmp0.f0->fields;_Tmp3=_Tmp0.f0->tagged;_Tmp2=_Tmp0.f0->effconstr;_Tmp1=*_Tmp0.f1;{struct Cyc_List_List*exist_vars=_Tmp6;struct Cyc_List_List**qb=(struct Cyc_List_List**)_Tmp5;struct Cyc_List_List*fs=_Tmp4;int tagged=_Tmp3;struct Cyc_List_List*effconstr=_Tmp2;struct Cyc_Absyn_Aggrdecl**adp=_Tmp1;
# 519
if((int)ad->kind!=(int)(*adp)->kind)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("cannot reuse struct names for unions and vice-versa",sizeof(char),52U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),1));});{
# 522
struct Cyc_Absyn_Aggrdecl*ad0=*adp;
# 524
({struct Cyc_Absyn_Aggrdecl*_Tmp7=({struct Cyc_Absyn_Aggrdecl*_Tmp8=_cycalloc(sizeof(struct Cyc_Absyn_Aggrdecl));_Tmp8->kind=ad->kind,_Tmp8->sc=3U,_Tmp8->name=ad->name,_Tmp8->tvs=tvs,_Tmp8->impl=0,_Tmp8->attributes=ad->attributes,_Tmp8->expected_mem_kind=0;_Tmp8;});*adp=_Tmp7;});
# 530
Cyc_Tcutil_check_unique_tvars(loc,exist_vars);
Cyc_Tcutil_add_tvar_identities(exist_vars);
# 533
if(tagged &&(int)ad->kind==0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=_tag_fat("@tagged is allowed only on union declarations",sizeof(char),46U);_Tmp8;});void*_Tmp8[1];_Tmp8[0]=& _Tmp7;Cyc_Warn_err2(loc,_tag_fat(_Tmp8,sizeof(void*),1));});{
struct Cyc_List_List*btvs=({struct Cyc_List_List*_Tmp7=({struct Cyc_List_List*(*_Tmp8)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Tcutil_tvar_bool_pair,0,tvs);Cyc_List_append(_Tmp7,
({struct Cyc_List_List*(*_Tmp8)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct _tuple12*(*)(int,struct Cyc_Absyn_Tvar*),int,struct Cyc_List_List*))Cyc_List_map_c;_Tmp8;})(Cyc_Tcutil_tvar_bool_pair,1,exist_vars));});
Cyc_Tc_tcAggrImpl(te,loc,ad->kind,btvs,effconstr,qb,fs);
# 539
*adp=ad0;
_Tmp6=adp;goto _LL8;}}}}}_LL0:;}}}struct _tuple17{struct Cyc_Absyn_Tqual f0;void*f1;};
# 555
static struct Cyc_List_List*Cyc_Tc_tcDatatypeFields(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct _fat_ptr obj,int is_extensible,struct _tuple1*name,struct Cyc_List_List*fields,struct Cyc_List_List*tvs,struct Cyc_Absyn_Datatypedecl*tudres){
# 563
{struct Cyc_List_List*fs=fields;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
struct Cyc_List_List*typs=f->typs;for(0;typs!=0;typs=typs->tl){
Cyc_Tctyp_check_type(f->loc,te,tvs,& Cyc_Kinds_mk,0,0,(*((struct _tuple17*)typs->hd)).f1);
# 568
if(({struct Cyc_List_List*_Tmp0=Cyc_Tcenv_curr_aquals_bounds(te);Cyc_Tcutil_is_noalias_pointer_or_aggr(_Tmp0,(*((struct _tuple17*)typs->hd)).f1);}))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("noalias pointers in datatypes are not allowed: ",sizeof(char),48U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=f->name;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp2,sizeof(void*),2));});
# 571
({int _Tmp0=
Cyc_Tcutil_extract_const_from_typedef(f->loc,(*((struct _tuple17*)typs->hd)).f0.print_const,(*((struct _tuple17*)typs->hd)).f1);
# 571
(*((struct _tuple17*)typs->hd)).f0.real_const=_Tmp0;});}}}
# 577
if(is_extensible){
# 579
int res=1;
struct Cyc_List_List*fs=Cyc_Tcdecl_sort_xdatatype_fields(fields,& res,(*name).f1,loc,Cyc_Tc_tc_msg);
if(res)return fs;else{return 0;}}{
# 583
struct _RegionHandle _Tmp0=_new_region(0U,"uprev_rgn");struct _RegionHandle*uprev_rgn=& _Tmp0;_push_region(uprev_rgn);
# 585
{struct Cyc_List_List*prev_fields=0;
{struct Cyc_List_List*fs=fields;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*f=(struct Cyc_Absyn_Datatypefield*)fs->hd;
if(({int(*_Tmp1)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;_Tmp1;})(Cyc_strptrcmp,prev_fields,(*f->name).f1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("duplicate field ",sizeof(char),17U);_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=f->name;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" in ",sizeof(char),5U);_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=obj;_Tmp5;});void*_Tmp5[4];_Tmp5[0]=& _Tmp1,_Tmp5[1]=& _Tmp2,_Tmp5[2]=& _Tmp3,_Tmp5[3]=& _Tmp4;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp5,sizeof(void*),4));});else{
# 591
prev_fields=({struct Cyc_List_List*_Tmp1=_region_malloc(uprev_rgn,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=(*f->name).f1,_Tmp1->tl=prev_fields;_Tmp1;});}
# 593
if((int)f->sc!=2){
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("ignoring scope of field ",sizeof(char),25U);_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=f->name;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_warn2(loc,_tag_fat(_Tmp3,sizeof(void*),2));});
f->sc=2U;}}}{
# 598
struct Cyc_List_List*_Tmp1=fields;_npop_handler(0);return _Tmp1;}}
# 585
;_pop_region();}}struct _tuple18{struct Cyc_Core_Opt*f0;struct Cyc_Absyn_Datatypedecl***f1;};
# 601
void Cyc_Tc_tcDatatypedecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Datatypedecl*tud){
struct _tuple1*q=tud->name;
struct _fat_ptr obj=tud->is_extensible?_tag_fat("@extensible datatype",sizeof(char),21U): _tag_fat("datatype",sizeof(char),9U);
# 607
struct Cyc_List_List*tvs=tud->tvs;
# 610
Cyc_Tcutil_check_unique_tvars(loc,tvs);
Cyc_Tcutil_add_tvar_identities(tvs);{
# 616
struct Cyc_Absyn_Datatypedecl***tud_opt;
# 626 "tc.cyc"
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
tud_opt=Cyc_Tcenv_lookup_xdatatypedecl(Cyc_Core_heap_region,te,loc,tud->name);
if(tud_opt==0 && !tud->is_extensible)_throw((void*)({struct Cyc_Dict_Absent_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Dict_Absent_exn_struct));_Tmp2->tag=Cyc_Dict_Absent;_Tmp2;}));
if(tud_opt!=0)
tud->name=(*(*tud_opt))->name;else{
# 632
({union Cyc_Absyn_Nmspace _Tmp2=Cyc_Absyn_Abs_n(te->ns,0);(*tud->name).f0=_Tmp2;});}
# 627
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 634
struct Cyc_Absyn_Datatypedecl***tdopt=({struct Cyc_Absyn_Datatypedecl***(*_Tmp4)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct Cyc_Absyn_Datatypedecl***(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;_Tmp4;})(te->ae->datatypedecls,q);
tud_opt=(unsigned)tdopt?({struct Cyc_Absyn_Datatypedecl***_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl**));*_Tmp4=*tdopt;_Tmp4;}): 0;
goto _LL0;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL0:;}}}{
# 641
struct _tuple18 _Tmp0=({struct _tuple18 _Tmp1;_Tmp1.f0=tud->fields,_Tmp1.f1=tud_opt;_Tmp1;});void*_Tmp1;void*_Tmp2;if(_Tmp0.f0==0){if(_Tmp0.f1==0){
# 645
Cyc_Tc_rule_out_mem_and_qual(loc,q,tvs);
({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict(*_Tmp4)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**)=({struct Cyc_Dict_Dict(*_Tmp5)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**))Cyc_Dict_insert;_Tmp5;});struct Cyc_Dict_Dict _Tmp5=te->ae->datatypedecls;struct _tuple1*_Tmp6=q;_Tmp4(_Tmp5,_Tmp6,({struct Cyc_Absyn_Datatypedecl**_Tmp7=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*));*_Tmp7=tud;_Tmp7;}));});te->ae->datatypedecls=_Tmp3;});
goto _LL5;}else{_Tmp2=*_Tmp0.f1;_LLD: {struct Cyc_Absyn_Datatypedecl**tudp=_Tmp2;
# 678
struct Cyc_Absyn_Datatypedecl*tud2=Cyc_Tcdecl_merge_datatypedecl(*tudp,tud,loc,Cyc_Tc_tc_msg);
Cyc_Tc_rule_out_mem_and_qual(loc,q,tvs);
if(tud2!=0)
*tudp=tud2;
goto _LL5;}}}else{if(_Tmp0.f1==0){_Tmp2=(struct Cyc_List_List**)& _Tmp0.f0->v;{struct Cyc_List_List**fs=_Tmp2;
# 650
struct Cyc_Absyn_Datatypedecl**tudp;tudp=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl*)),({struct Cyc_Absyn_Datatypedecl*_Tmp3=({struct Cyc_Absyn_Datatypedecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_Tmp4->sc=3U,_Tmp4->name=tud->name,_Tmp4->tvs=tvs,_Tmp4->fields=0,_Tmp4->is_extensible=tud->is_extensible;_Tmp4;});*tudp=_Tmp3;});
# 652
({struct Cyc_Dict_Dict _Tmp3=({struct Cyc_Dict_Dict(*_Tmp4)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Datatypedecl**))Cyc_Dict_insert;_Tmp4;})(te->ae->datatypedecls,q,tudp);te->ae->datatypedecls=_Tmp3;});
# 655
({struct Cyc_List_List*_Tmp3=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_Tmp3;});
Cyc_Tc_rule_out_mem_and_qual(loc,q,tvs);
*tudp=tud;
goto _LL5;}}else{_Tmp2=(struct Cyc_List_List**)& _Tmp0.f0->v;_Tmp1=*_Tmp0.f1;{struct Cyc_List_List**fs=(struct Cyc_List_List**)_Tmp2;struct Cyc_Absyn_Datatypedecl**tudp=_Tmp1;
# 660
struct Cyc_Absyn_Datatypedecl*tud0=*tudp;
# 663
if((!tud->is_extensible &&(unsigned)tud0)&& tud0->is_extensible)
tud->is_extensible=1;
# 666
({struct Cyc_Absyn_Datatypedecl*_Tmp3=({struct Cyc_Absyn_Datatypedecl*_Tmp4=_cycalloc(sizeof(struct Cyc_Absyn_Datatypedecl));_Tmp4->sc=3U,_Tmp4->name=tud->name,_Tmp4->tvs=tvs,_Tmp4->fields=0,_Tmp4->is_extensible=tud->is_extensible;_Tmp4;});*tudp=_Tmp3;});
# 670
({struct Cyc_List_List*_Tmp3=Cyc_Tc_tcDatatypeFields(te,loc,obj,tud->is_extensible,tud->name,*fs,tvs,tud);*fs=_Tmp3;});
# 673
*tudp=tud0;
_Tmp2=tudp;goto _LLD;}}}_LL5:;}}}
# 686
void Cyc_Tc_tcEnumdecl(struct Cyc_Tcenv_Tenv*te,unsigned loc,struct Cyc_Absyn_Enumdecl*ed){
struct _tuple1*q=ed->name;
# 692
if(ed->fields!=0){
struct _RegionHandle _Tmp0=_new_region(0U,"uprev_rgn");struct _RegionHandle*uprev_rgn=& _Tmp0;_push_region(uprev_rgn);
{struct Cyc_List_List*prev_fields=0;
unsigned tag_count=0U;
struct Cyc_List_List*fs=(struct Cyc_List_List*)ed->fields->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
# 699
if(({int(*_Tmp1)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*)=(int(*)(int(*)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*,struct _fat_ptr*))Cyc_List_mem;_Tmp1;})(Cyc_strptrcmp,prev_fields,(*f->name).f1))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("duplicate enum constructor ",sizeof(char),28U);_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=f->name;_Tmp3;});void*_Tmp3[2];_Tmp3[0]=& _Tmp1,_Tmp3[1]=& _Tmp2;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp3,sizeof(void*),2));});else{
# 702
prev_fields=({struct Cyc_List_List*_Tmp1=_region_malloc(uprev_rgn,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=(*f->name).f1,_Tmp1->tl=prev_fields;_Tmp1;});}
# 704
if(({struct _tuple15**(*_Tmp1)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple15**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup_opt;_Tmp1;})(te->ae->ordinaries,f->name)!=0)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=_tag_fat("enum field name ",sizeof(char),17U);_Tmp2;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=f->name;_Tmp3;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=_tag_fat(" shadows global name",sizeof(char),21U);_Tmp4;});void*_Tmp4[3];_Tmp4[0]=& _Tmp1,_Tmp4[1]=& _Tmp2,_Tmp4[2]=& _Tmp3;Cyc_Warn_err2(f->loc,_tag_fat(_Tmp4,sizeof(void*),3));});
if(f->tag==0)
({struct Cyc_Absyn_Exp*_Tmp1=Cyc_Absyn_uint_exp(tag_count ++,f->loc);f->tag=_Tmp1;});else{
# 709
if(Cyc_Tcutil_is_const_exp(f->tag)){
struct _tuple13 _Tmp1=Cyc_Evexp_eval_const_uint_exp(_check_null(f->tag));int _Tmp2;unsigned _Tmp3;_Tmp3=_Tmp1.f0;_Tmp2=_Tmp1.f1;{unsigned t1=_Tmp3;int known=_Tmp2;
if(known)
tag_count=t1 + 1U;}}}}}
# 694
;_pop_region();}
# 718
{struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_Absyn_Enumdecl**edp=({struct Cyc_Absyn_Enumdecl**(*_Tmp2)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct Cyc_Absyn_Enumdecl**(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup;_Tmp2;})(te->ae->enumdecls,q);
struct Cyc_Absyn_Enumdecl*ed2=Cyc_Tcdecl_merge_enumdecl(*edp,ed,loc,Cyc_Tc_tc_msg);
if(ed2==0){_npop_handler(0);return;}
*edp=ed2;}
# 719
;_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Dict_Absent_exn_struct*)_Tmp2)->tag==Cyc_Dict_Absent){
# 724
struct Cyc_Absyn_Enumdecl**edp;edp=_cycalloc(sizeof(struct Cyc_Absyn_Enumdecl*)),*edp=ed;
({struct Cyc_Dict_Dict _Tmp4=({struct Cyc_Dict_Dict(*_Tmp5)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Enumdecl**)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct _tuple1*,struct Cyc_Absyn_Enumdecl**))Cyc_Dict_insert;_Tmp5;})(te->ae->enumdecls,q,edp);te->ae->enumdecls=_Tmp4;});
goto _LL3;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}}_LL3:;}}}
# 730
if(ed->fields!=0){
struct Cyc_List_List*fs=(struct Cyc_List_List*)ed->fields->v;for(0;fs!=0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*f=(struct Cyc_Absyn_Enumfield*)fs->hd;
Cyc_Tcexp_tcExp(te,0,_check_null(f->tag));
if(!Cyc_Tcutil_is_const_exp(_check_null(f->tag)))
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("enum ",sizeof(char),6U);_Tmp1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=q;_Tmp2;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp2=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp3;_Tmp3.tag=0,_Tmp3.f1=_tag_fat(", field ",sizeof(char),9U);_Tmp3;});struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp3=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp4;_Tmp4.tag=1,_Tmp4.f1=f->name;_Tmp4;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp4=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=
_tag_fat(": expression is not constant",sizeof(char),29U);_Tmp5;});void*_Tmp5[5];_Tmp5[0]=& _Tmp0,_Tmp5[1]=& _Tmp1,_Tmp5[2]=& _Tmp2,_Tmp5[3]=& _Tmp3,_Tmp5[4]=& _Tmp4;Cyc_Warn_err2(loc,_tag_fat(_Tmp5,sizeof(void*),5));});}}}
# 740
static int Cyc_Tc_okay_externC(unsigned loc,enum Cyc_Absyn_Scope sc,int in_include,int in_inc_rep){
switch((int)sc){case Cyc_Absyn_Static:
# 743
 if(!in_include)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("static declaration within extern \"C\"",sizeof(char),37U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return 0;case Cyc_Absyn_Abstract:
# 747
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("abstract declaration within extern \"C\"",sizeof(char),39U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return 0;case Cyc_Absyn_Public:
 goto _LL8;case Cyc_Absyn_Register: _LL8:
 goto _LLA;case Cyc_Absyn_Extern: _LLA:
 return 1;case Cyc_Absyn_ExternC:
 goto _LLE;default: _LLE:
# 754
 if(!in_inc_rep)
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp0=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=_tag_fat("nested extern \"C\" declaration",sizeof(char),30U);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_warn2(loc,_tag_fat(_Tmp1,sizeof(void*),1));});
return 1;};}
# 760
static void Cyc_Tc_set_scopes(struct Cyc_List_List*ovrs,enum Cyc_Absyn_Scope sc){
for(1;ovrs!=0;ovrs=ovrs->tl){
void*_Tmp0=((struct Cyc_Absyn_Decl*)ovrs->hd)->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
vd->sc=sc;goto _LL0;}case 1: _Tmp1=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp1;
fd->sc=sc;goto _LL0;}case 5: _Tmp1=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp1;
ad->sc=sc;goto _LL0;}case 6: _Tmp1=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp1;
tud->sc=sc;goto _LL0;}case 7: _Tmp1=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp1;
ed->sc=sc;goto _LL0;}default:
 goto _LL0;}_LL0:;}}
# 777
static void Cyc_Tc_tc_decls(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds0,int in_externC,int check_var_init,struct Cyc_List_List**exports){
# 781
struct Cyc_List_List*ds=ds0;for(0;ds!=0;ds=ds->tl){
struct Cyc_Absyn_Decl*d=(struct Cyc_Absyn_Decl*)ds->hd;
unsigned loc=d->loc;
void*_Tmp0=d->r;void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;switch(*((int*)_Tmp0)){case 2:
 goto _LL4;case 3: _LL4:
# 787
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("top level let-declarations are not implemented",sizeof(char),47U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;case 4:
# 789
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("top level region declarations are not implemented",sizeof(char),50U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_err2(loc,_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;case 0: _Tmp4=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 791
if(in_externC && Cyc_Tc_okay_externC(d->loc,vd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
vd->sc=4U;
Cyc_Tc_tcVardecl(te,loc,vd,check_var_init,te->in_extern_c_include,exports);
goto _LL0;}case 1: _Tmp4=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Fndecl*fd=_Tmp4;
# 796
if(in_externC && Cyc_Tc_okay_externC(d->loc,fd->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
fd->sc=4U;
if(te->in_extern_c_include)
fd->orig_scope=4U;
Cyc_Tc_tcFndecl(te,loc,fd,exports);
goto _LL0;}case 8: _Tmp4=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Typedefdecl*td=_Tmp4;
# 803
td->extern_c=te->in_extern_c_include;
# 807
Cyc_Tc_tcTypedefdecl(te,loc,td);
goto _LL0;}case 5: _Tmp4=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Aggrdecl*ad=_Tmp4;
# 810
if(in_externC && Cyc_Tc_okay_externC(d->loc,ad->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ad->sc=4U;
Cyc_Tc_tcAggrdecl(te,loc,ad);
goto _LL0;}case 6: _Tmp4=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Datatypedecl*tud=_Tmp4;
# 815
if(in_externC && Cyc_Tc_okay_externC(d->loc,tud->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
tud->sc=4U;
Cyc_Tc_tcDatatypedecl(te,loc,tud);
goto _LL0;}case 7: _Tmp4=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Enumdecl*ed=_Tmp4;
# 820
if(in_externC && Cyc_Tc_okay_externC(d->loc,ed->sc,te->in_extern_c_include,te->in_extern_c_inc_repeat))
ed->sc=4U;
Cyc_Tc_tcEnumdecl(te,loc,ed);
goto _LL0;}case 13:
({struct Cyc_Warn_String_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=_tag_fat("spurious __cyclone_port_on__",sizeof(char),29U);_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_Warn_warn2(d->loc,_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;case 14:
 goto _LL0;case 15:
 te=Cyc_Tcenv_enter_tempest(te);goto _LL0;case 16:
 te=Cyc_Tcenv_clear_tempest(te);goto _LL0;case 9: _Tmp4=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct _fat_ptr*v=_Tmp4;struct Cyc_List_List*ds2=_Tmp3;
# 830
struct Cyc_List_List*ns=te->ns;
({struct Cyc_List_List*_Tmp5=({struct Cyc_List_List*_Tmp6=ns;Cyc_List_append(_Tmp6,({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=v,_Tmp7->tl=0;_Tmp7;}));});te->ns=_Tmp5;});
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);
te->ns=ns;
goto _LL0;}case 10: _Tmp4=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;{struct Cyc_List_List*ds2=_Tmp4;
# 837
Cyc_Tc_tc_decls(te,ds2,in_externC,check_var_init,exports);goto _LL0;}case 11: _Tmp4=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List*ds2=_Tmp4;
# 840
Cyc_Tc_tc_decls(te,ds2,1,check_var_init,exports);goto _LL0;}default: _Tmp4=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;_Tmp3=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_Tmp2=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f3;_Tmp1=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f4;{struct Cyc_List_List**ds2=(struct Cyc_List_List**)_Tmp4;struct Cyc_List_List*ovrs=_Tmp3;struct Cyc_List_List*exports2=_Tmp2;struct _tuple11*wc=_Tmp1;
# 845
if((unsigned)ovrs){
struct Cyc_Tcenv_Tenv*tecpy=Cyc_Tcenv_copy_tenv_dicts(te);
tecpy->in_extern_c_include=1;
Cyc_Tc_tc_decls(tecpy,*ds2,1,check_var_init,0);
Cyc_Toc_init();
Cyc_Tc_set_scopes(ovrs,4U);
Cyc_Cifc_user_overrides(d->loc,tecpy,ds2,ovrs);
Cyc_Toc_finish();}{
# 854
struct Cyc_List_List*newexs=
Cyc_List_append(exports2,(unsigned)exports?*exports: 0);
struct Cyc_Tcenv_Tenv*te2=Cyc_Tcenv_enter_extern_c_include(te);
te2->in_extern_c_inc_repeat=ovrs!=0;
Cyc_Tc_tc_decls(te2,*ds2,1,check_var_init,& newexs);
# 861
for(1;exports2!=0;exports2=exports2->tl){
struct _tuple14*exp=(struct _tuple14*)exports2->hd;
if(!(*exp).f2)
({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(*exp).f1;_Tmp6;});struct Cyc_Warn_String_Warn_Warg_struct _Tmp6=({struct Cyc_Warn_String_Warn_Warg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=_tag_fat(" is exported but not defined",sizeof(char),29U);_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_Warn_warn2((*exp).f0,_tag_fat(_Tmp7,sizeof(void*),2));});}
# 866
goto _LL0;}}}_LL0:;}}
# 871
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int check_var_init,struct Cyc_List_List*ds){
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);{
struct Cyc_Dict_Dict cg=Cyc_Callgraph_compute_callgraph(ds);
# 876
struct Cyc_Dict_Dict scc=Cyc_Graph_scc(cg);
# 879
Cyc_Tc_tc_decls(te,ds,0,check_var_init,0);}}struct Cyc_Tc_TreeshakeEnv{int in_cinclude;struct Cyc_Dict_Dict ordinaries;};
# 897 "tc.cyc"
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*);
# 899
static struct Cyc_List_List*Cyc_Tc_treeshake_f(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_List_List*ds){
return({struct Cyc_List_List*(*_Tmp0)(int(*)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(int(*)(struct Cyc_Tc_TreeshakeEnv*,struct Cyc_Absyn_Decl*),struct Cyc_Tc_TreeshakeEnv*,struct Cyc_List_List*))Cyc_List_filter_c;_Tmp0;})(Cyc_Tc_vardecl_needed,env,ds);}
# 903
static int Cyc_Tc_is_extern(struct Cyc_Absyn_Vardecl*vd){
if((int)vd->sc==3 ||(int)vd->sc==4)
return 1;{
void*_Tmp0=Cyc_Absyn_compress(vd->type);if(*((int*)_Tmp0)==5)
return 1;else{
return 0;};}}
# 912
static int Cyc_Tc_vardecl_needed(struct Cyc_Tc_TreeshakeEnv*env,struct Cyc_Absyn_Decl*d){
void*_Tmp0=d->r;void*_Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp1;
# 916
if((env->in_cinclude || !Cyc_Tc_is_extern(vd))|| !({
struct _tuple1*_Tmp2=vd->name;Cyc_Absyn_qvar_cmp(_Tmp2,Cyc_Absyn_uniqueaqual_qvar());}))
return 1;
return(*({struct _tuple15*(*_Tmp2)(struct Cyc_Dict_Dict,struct _tuple1*)=(struct _tuple15*(*)(struct Cyc_Dict_Dict,struct _tuple1*))Cyc_Dict_lookup;_Tmp2;})(env->ordinaries,vd->name)).f1;}case 11: _Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List**ds2p=_Tmp1;
_Tmp1=ds2p;goto _LL6;}case 10: _Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL6: {struct Cyc_List_List**ds2p=_Tmp1;
_Tmp1=ds2p;goto _LL8;}case 9: _Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_Tmp0)->f2;_LL8: {struct Cyc_List_List**ds2p=_Tmp1;
({struct Cyc_List_List*_Tmp2=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_Tmp2;});return 1;}case 12: _Tmp1=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_Tmp0)->f1;{struct Cyc_List_List**ds2p=(struct Cyc_List_List**)_Tmp1;
# 924
int in_cinclude=env->in_cinclude;
env->in_cinclude=1;
({struct Cyc_List_List*_Tmp2=Cyc_Tc_treeshake_f(env,*ds2p);*ds2p=_Tmp2;});
env->in_cinclude=in_cinclude;
return 1;}default:
 return 1;};}
# 933
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*ds){
struct Cyc_Tc_TreeshakeEnv env=({struct Cyc_Tc_TreeshakeEnv _Tmp0;_Tmp0.in_cinclude=0,_Tmp0.ordinaries=te->ae->ordinaries;_Tmp0;});
return Cyc_Tc_treeshake_f(& env,ds);}
