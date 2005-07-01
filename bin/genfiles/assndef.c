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

# 125 "core.h"
 int Cyc_Core_ptrcmp(void*,void*);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 145
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 234
extern struct Cyc_List_List*Cyc_List_imp_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);struct Cyc_AssnDef_AssnMap;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 140 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 162
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U,Cyc_Absyn_Tagof =19U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};
# 929 "absyn.h"
extern void*Cyc_Absyn_uint_type;
# 969
void*Cyc_Absyn_exn_type (void);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 68
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 70
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 72
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 76 "flags.h"
extern int Cyc_Flags_debug_assn;struct _tuple11{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 35 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
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
# 50 "string.h"
extern int Cyc_strptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 57
extern struct Cyc_Set_Set*Cyc_Set_singleton(int(*)(void*,void*),void*);
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 75
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 79
extern struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 114
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;void*f2;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Select_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Update_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Aggr_AssnDef_Term_struct{int tag;int f1;unsigned f2;struct Cyc_List_List*f3;void*f4;};struct Cyc_AssnDef_Proj_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_Alloc_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;void*f4;};struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct{int tag;struct _fat_ptr*f1;void*f2;void*f3;};struct Cyc_AssnDef_Offseti_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Tagof_AssnDef_Term_struct{int tag;void*f1;};
# 126 "assndef.h"
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
# 158 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};
# 173
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 178
void*Cyc_AssnDef_not(void*);struct Cyc_AssnDef_AssnMap{void*assn;struct Cyc_Dict_Dict map;};struct _tuple12{void*f0;void*f1;struct Cyc_Dict_Dict f2;};struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _fat_ptr tab;};static char _TmpG0[2U]=",";static char _TmpG1[2U]="}";static char _TmpG2[2U]="{";
# 68 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
struct _fat_ptr res;
{enum Cyc_Absyn_Primop _Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("0x%x",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
res=Cyc_Absynpp_exp2string(e);goto _LL0;}case 14: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("tagof_tm(%s)",sizeof(char),13U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 2: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp3;
# 75
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("Sel(%s,%s)",sizeof(char),11U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}case 3: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;
# 78
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_term2string(t3);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Upd(%s,%s,%s)",sizeof(char),14U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 4: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1==0){_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{int i=_Tmp1;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("_X%d",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}}else{_Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp1;
# 82
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("_%s%d",sizeof(char),6U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}}case 5: _Tmp0=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;
# 84
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_prim2string(p);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s(%s)",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 6: _Tmp0=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;void*t2=_Tmp3;
# 86
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_Absynpp_prim2string(p);_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("(%s%s%s)",sizeof(char),9U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 7: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp4;void*tm=_Tmp3;
# 90
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_typ2string(tp);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(tm);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("(%s)%s",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 8: _Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{int is_union=_Tmp1;unsigned tag=_Tmp5;struct Cyc_List_List*tms=_Tmp4;
# 92
static struct _fat_ptr lb={_TmpG2,_TmpG2,_TmpG2 + 2U};
static struct _fat_ptr rb={_TmpG1,_TmpG1,_TmpG1 + 2U};
static struct _fat_ptr comma={_TmpG0,_TmpG0,_TmpG0 + 2U};
struct Cyc_List_List*ss;ss=_cycalloc(sizeof(struct Cyc_List_List)),ss->hd=& lb,ss->tl=0;
for(1;tms!=0;tms=tms->tl){
struct _fat_ptr s=Cyc_AssnDef_term2string((void*)tms->hd);
ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));*_Tmp8=s;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=ss;_Tmp6;});
if(tms->tl!=0)ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=& comma,_Tmp6->tl=ss;_Tmp6;});}
# 101
ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=& rb,_Tmp6->tl=ss;_Tmp6;});
ss=Cyc_List_imp_rev(ss);
res=Cyc_strconcat_l(ss);
if(tag!=4294967295U)res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)tag);_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=(struct _fat_ptr)res;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("tag(%d)%s",sizeof(char),10U),_tag_fat(_Tmp8,sizeof(void*),2));});
if(is_union)res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)res;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("union%s",sizeof(char),8U),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL0;}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;unsigned i=_Tmp5;
# 108
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)i);_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("#%d(%s)",sizeof(char),8U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}case 10: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 111
if(Cyc_Flags_debug_assn)
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned)vd;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("&%s_%x",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});else{
# 114
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("&%s",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));});}
goto _LL0;}case 11: _Tmp4=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;int i=_Tmp1;
# 117
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_Absynpp_exp2string(e);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("Alloc(%s,%d)",sizeof(char),13U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}case 12: _Tmp4=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;{struct _fat_ptr*field=_Tmp4;void*t=_Tmp3;
# 120
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)*field;_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("Offsetf(%s)+%s",sizeof(char),15U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}default: _Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp3;
# 123
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s+%s",sizeof(char),6U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}}_LL0:;}
# 126
return res;}
# 129
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
# 140
return Cyc_Hashtable_hash_string(s);}};}
# 144
static int Cyc_AssnDef_hash_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;union Cyc_Absyn_Cnst _Tmp1;switch(*((int*)_Tmp0)){case 0: _Tmp1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_Tmp0)->f1;{union Cyc_Absyn_Cnst c=_Tmp1;
return Cyc_AssnDef_hash_const(c);}case 1:
 return 1;case 2:
 return 2;case 3:
 return 3;case 6:
 return 4;case 7:
 return 5;case 8:
 return 6;case 9:
 return 7;case 12:
 return 8;case 13:
 return 9;case 14:
 return 10;case 15:
 return 11;case 17:
 return 12;case 18:
 return 13;case 19:
 return 14;case 31:
 return 15;case 32:
 return 16;case 38:
 return 17;case 41:
 return 18;default:
 return 19;};}
# 169
int Cyc_AssnDef_termhash(void*t){
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;int _Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
return(int)i;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return Cyc_AssnDef_hash_exp(e)+ 61;}case 4: _Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp3;
return(int)(((unsigned)vd >> 3U)+ (unsigned)(i + 1));}case 5: _Tmp2=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp4;
return(int)(((unsigned)p + ((unsigned)t1 >> 4U))+ 1U);}case 6: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp4;void*t2=_Tmp1;
# 176
return(int)((unsigned)p + ((unsigned)t1 + (unsigned)t2 >> 4U));}case 7: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;
return Cyc_AssnDef_termhash(t);}case 2: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp1;
# 179
return(int)(((unsigned)t1 + (unsigned)t2 >> 4U)+ 2U);}case 3: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp1;void*t3=_Tmp0;
# 181
return(int)((((unsigned)t1 + (unsigned)t2)+ (unsigned)t3 >> 4U)+ 3U);}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;unsigned i=_Tmp5;
return(int)((((unsigned)t >> 4U)+ i)+ 23U);}case 8: _Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{int is_union=_Tmp3;unsigned tag=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
# 184
unsigned res=(37U + tag)+ (unsigned)is_union;
for(1;ts!=0;ts=ts->tl){
res=res + ((unsigned)((void*)ts->hd)>> 4U);}
return(int)res;}case 10: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(int)(((unsigned)vd >> 4U)+ 7U);}case 11: _Tmp4=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Exp*e=_Tmp4;int i=_Tmp3;
return({int _Tmp6=Cyc_AssnDef_hash_exp(e);_Tmp6 + i;});}case 12: _Tmp4=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;{struct _fat_ptr*f=_Tmp4;void*t=_Tmp1;
return(int)(({unsigned _Tmp6=(unsigned)f;_Tmp6 + (unsigned)Cyc_AssnDef_termhash(t);})+ 11U);}case 13: _Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;{void*index=_Tmp4;void*t=_Tmp1;
return({int _Tmp6=Cyc_AssnDef_termhash(index);_Tmp6 + Cyc_AssnDef_termhash(t);})+ 13;}default: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
return(int)(((unsigned)t >> 3U)+ 741U);}};}struct _tuple13{void*f0;void*f1;};
# 196
int Cyc_AssnDef_termcmp(void*_t1,void*_t2){
void*t1=_t1;
void*t2=_t2;
LOOP:
 if(t1==t2)return 0;{
int c=(int)(*((const unsigned*)t1)- *((const unsigned*)t2));
if(c!=0)return c;{
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;enum Cyc_Absyn_Primop _Tmp5;enum Cyc_Absyn_Primop _Tmp6;unsigned _Tmp7;unsigned _Tmp8;int _Tmp9;void*_TmpA;int _TmpB;void*_TmpC;switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_TmpC=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*vd1=_TmpC;int i=_TmpB;struct Cyc_Absyn_Vardecl*vd2=_TmpA;int j=_Tmp9;
# 205
int c=i - j;
if(c!=0)return c;
return(int)vd1 - (int)vd2;}}else{goto _LL1F;}case 0: if(*((int*)_Tmp0.f1)==0){_Tmp8=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp7=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i1=_Tmp8;unsigned i2=_Tmp7;
# 210
return(int)(i1 - i2);}}else{goto _LL1F;}case 1: if(*((int*)_Tmp0.f1)==1){_TmpC=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Exp*e1=_TmpC;struct Cyc_Absyn_Exp*e2=_TmpA;
# 213
return Cyc_Evexp_exp_cmp(e1,e2);}}else{goto _LL1F;}case 10: if(*((int*)_Tmp0.f1)==10){_TmpC=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl*vd1=_TmpC;struct Cyc_Absyn_Vardecl*vd2=_TmpA;
# 216
return(int)vd1 - (int)vd2;}}else{goto _LL1F;}case 5: if(*((int*)_Tmp0.f1)==5){_Tmp6=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpC=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp5=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;{enum Cyc_Absyn_Primop p1=_Tmp6;void*t1a=_TmpC;enum Cyc_Absyn_Primop p2=_Tmp5;void*t2a=_TmpA;
# 220
if((int)p1!=(int)p2)return(int)p1 - (int)p2;
t1=t1a;
t2=t2a;
goto LOOP;}}else{goto _LL1F;}case 6: if(*((int*)_Tmp0.f1)==6){_Tmp6=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpC=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp5=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f4;{enum Cyc_Absyn_Primop p1=_Tmp6;void*t11=_TmpC;void*t12=_TmpA;void*tp1=_Tmp4;enum Cyc_Absyn_Primop p2=_Tmp5;void*t21=_Tmp3;void*t22=_Tmp2;void*tp2=_Tmp1;
# 227
if((int)p1!=(int)p2)return(int)p1 - (int)p2;{
int c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;
t1=t12;
t2=t22;
goto LOOP;}}}else{goto _LL1F;}case 7: if(*((int*)_Tmp0.f1)==7){_TmpC=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tp1=_TmpC;void*tm1=_TmpA;void*tp2=_Tmp4;void*tm2=_Tmp3;
# 236
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c!=0)return c;
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL1F;}case 2: if(*((int*)_Tmp0.f1)==2){_TmpC=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*t11=_TmpC;void*t12=_TmpA;void*t21=_Tmp4;void*t22=_Tmp3;
# 244
int c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;
t1=t12;
t2=t22;
goto LOOP;}}else{goto _LL1F;}case 3: if(*((int*)_Tmp0.f1)==3){_TmpC=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*t11=_TmpC;void*t12=_TmpA;void*t13=_Tmp4;void*t21=_Tmp3;void*t22=_Tmp2;void*t23=_Tmp1;
# 252
int c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;{
int c=Cyc_AssnDef_termcmp(t12,t22);
if(c!=0)return c;
t1=t13;
t2=t23;
goto LOOP;}}}else{goto _LL1F;}case 14: if(*((int*)_Tmp0.f1)==14){_TmpC=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*tm1=_TmpC;void*tm2=_TmpA;
# 262
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL1F;}case 9: if(*((int*)_Tmp0.f1)==9){_TmpC=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp8=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tm1=_TmpC;unsigned i1=_Tmp8;void*tm2=_TmpA;unsigned i2=_Tmp7;
# 268
int c=(int)(i1 - i2);
if(c!=0)return c;
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL1F;}case 11: if(*((int*)_Tmp0.f1)==11){_TmpC=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f0)->f3;_TmpA=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)_Tmp0.f1)->f3;{struct Cyc_Absyn_Exp*e1=_TmpC;int i1=_TmpB;struct Cyc_Absyn_Exp*e2=_TmpA;int i2=_Tmp9;
# 276
if(e1!=e2)return(int)e1 - (int)e2;else{
return i1 - i2;}}}else{goto _LL1F;}case 12: if(*((int*)_Tmp0.f1)==12){_TmpC=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp4=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)_Tmp0.f1)->f2;{struct _fat_ptr*f1=_TmpC;void*tm1=_TmpA;struct _fat_ptr*f2=_Tmp4;void*tm2=_Tmp3;
# 281
int c=Cyc_strptrcmp(f1,f2);
if(c!=0)return c;
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL1F;}case 13: if(*((int*)_Tmp0.f1)==13){_TmpC=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*i1=_TmpC;void*tm1=_TmpA;void*i2=_Tmp4;void*tm2=_Tmp3;
# 289
int c=Cyc_AssnDef_termcmp(tm1,tm2);
if(c!=0)return c;
t1=tm1;
t2=tm2;
goto LOOP;}}else{goto _LL1F;}default: if(*((int*)_Tmp0.f1)==8){_TmpB=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp8=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpC=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp9=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f2;_TmpA=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f3;{int is_union1=_TmpB;unsigned tag1=_Tmp8;struct Cyc_List_List*ts1=_TmpC;int is_union2=_Tmp9;unsigned tag2=_Tmp7;struct Cyc_List_List*ts2=_TmpA;
# 297
int c=(int)(tag1 - tag2);
if(c!=0)return c;
c=is_union2 - is_union1;
if(c!=0)return c;
for(1;ts1!=0 && ts2!=0;(ts1=ts1->tl,ts2=ts2->tl)){
c=Cyc_AssnDef_termcmp((void*)ts1->hd,(void*)ts2->hd);
if(c!=0)return c;}
# 305
if(ts1==ts2)return 0;
if(ts1==0)return -1;
return 1;}}else{_LL1F:
({int(*_TmpD)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_TmpD;})(_tag_fat("different terms but same codes!",sizeof(char),32U),_tag_fat(0U,sizeof(void*),0));}};}}}
# 312
static void*Cyc_AssnDef_copy_term(void*t){
void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f2;{unsigned i=_Tmp5;void*tp=_Tmp4;
return(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Uint_AssnDef_Term_struct));_Tmp6->tag=0,_Tmp6->f1=i,_Tmp6->f2=tp;_Tmp6;});}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Const_AssnDef_Term_struct));_Tmp6->tag=1,_Tmp6->f1=e;_Tmp6;});}case 10: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(void*)({struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Addr_AssnDef_Term_struct));_Tmp6->tag=10,_Tmp6->f1=vd;_Tmp6;});}case 4: _Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp3;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct));_Tmp6->tag=4,_Tmp6->f1=vd,_Tmp6->f2=i,_Tmp6->f3=tp;_Tmp6;});}case 6: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp4;void*t2=_Tmp2;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Binop_AssnDef_Term_struct));_Tmp6->tag=6,_Tmp6->f1=p,_Tmp6->f2=t1,_Tmp6->f3=t2,_Tmp6->f4=tp;_Tmp6;});}case 5: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t=_Tmp4;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Unop_AssnDef_Term_struct));_Tmp6->tag=5,_Tmp6->f1=p,_Tmp6->f2=t,_Tmp6->f3=tp;_Tmp6;});}case 7: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp4;void*t=_Tmp2;
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Cast_AssnDef_Term_struct));_Tmp6->tag=7,_Tmp6->f1=tp,_Tmp6->f2=t;_Tmp6;});}case 2: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp2;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Select_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Select_AssnDef_Term_struct));_Tmp6->tag=2,_Tmp6->f1=t1,_Tmp6->f2=t2,_Tmp6->f3=tp;_Tmp6;});}case 3: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp2;void*t3=_Tmp0;
return(void*)({struct Cyc_AssnDef_Update_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Update_AssnDef_Term_struct));_Tmp6->tag=3,_Tmp6->f1=t1,_Tmp6->f2=t2,_Tmp6->f3=t3;_Tmp6;});}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp4;unsigned i=_Tmp5;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Proj_AssnDef_Term_struct));_Tmp6->tag=9,_Tmp6->f1=t,_Tmp6->f2=i,_Tmp6->f3=tp;_Tmp6;});}case 8: _Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int is_union=_Tmp3;unsigned tag=_Tmp5;struct Cyc_List_List*ts=_Tmp4;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct));_Tmp6->tag=8,_Tmp6->f1=is_union,_Tmp6->f2=tag,_Tmp6->f3=ts,_Tmp6->f4=tp;_Tmp6;});}case 11: _Tmp4=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;{struct Cyc_Absyn_Exp*e=_Tmp4;void*offset=_Tmp2;int i=_Tmp3;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Alloc_AssnDef_Term_struct));_Tmp6->tag=11,_Tmp6->f1=e,_Tmp6->f2=offset,_Tmp6->f3=i,_Tmp6->f4=tp;_Tmp6;});}case 12: _Tmp4=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{struct _fat_ptr*f=_Tmp4;void*t=_Tmp2;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct));_Tmp6->tag=12,_Tmp6->f1=f,_Tmp6->f2=t,_Tmp6->f3=tp;_Tmp6;});}case 13: _Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*i=_Tmp4;void*t=_Tmp2;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Offseti_AssnDef_Term_struct));_Tmp6->tag=13,_Tmp6->f1=i,_Tmp6->f2=t,_Tmp6->f3=tp;_Tmp6;});}default: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
return(void*)({struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct));_Tmp6->tag=14,_Tmp6->f1=t;_Tmp6;});}};}
# 332
struct Cyc_AssnDef_Uint_AssnDef_Term_struct Cyc_AssnDef_zero_value={0,0U,0};
struct Cyc_AssnDef_Uint_AssnDef_Term_struct Cyc_AssnDef_one_value={0,1U,0};
# 335
static struct Cyc_Set_Set*Cyc_AssnDef_empty_term_set (void){
static struct Cyc_Set_Set**term_set=0;
if(term_set==0)
term_set=({struct Cyc_Set_Set**_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set*));({struct Cyc_Set_Set*_Tmp1=Cyc_Set_empty(Cyc_AssnDef_termcmp);*_Tmp0=_Tmp1;});_Tmp0;});
# 340
return*_check_null(term_set);}struct Cyc_AssnDef_TermHashedInfo{void*term;struct Cyc_Set_Set**free_logicvar_set;};
# 349
static struct Cyc_Hashtable_Table*Cyc_AssnDef_term_hash_cons_table=0;
# 352
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_term_hash_cons_table (void){
# 354
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_term_hash_cons_table==0){
h=Cyc_Hashtable_create(221,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);
Cyc_AssnDef_term_hash_cons_table=h;{
struct Cyc_Set_Set*tst=Cyc_AssnDef_empty_term_set();
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_zero_value,({struct Cyc_AssnDef_TermHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_Tmp2->term=(void*)& Cyc_AssnDef_zero_value,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_one_value,({struct Cyc_AssnDef_TermHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_Tmp2->term=(void*)& Cyc_AssnDef_one_value,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});}}else{
# 362
h=_check_null(Cyc_AssnDef_term_hash_cons_table);}
# 364
return h;}
# 367
static void*Cyc_AssnDef_hash_cons_term(void*t){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_term_hash_cons_table();
struct Cyc_AssnDef_TermHashedInfo**resopt=({struct Cyc_AssnDef_TermHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*))=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;_Tmp0;})(h,t,Cyc_AssnDef_termcmp,Cyc_AssnDef_termhash);
if(resopt==0){
void*t2=Cyc_AssnDef_copy_term(t);
# 373
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_TermHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;void*_Tmp2=t2;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_TermHashedInfo*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_TermHashedInfo));_Tmp3->term=t2,_Tmp3->free_logicvar_set=0;_Tmp3;}));});
# 376
return t2;}else{
# 378
return(*resopt)->term;}}
# 384
static struct Cyc_Set_Set*Cyc_AssnDef_term_fr_logicvar(void*t){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_term_hash_cons_table();
struct Cyc_AssnDef_TermHashedInfo**thinfo=({struct Cyc_AssnDef_TermHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,t);
# 388
if(thinfo==0){
t=Cyc_AssnDef_hash_cons_term(t);
thinfo=({struct Cyc_AssnDef_TermHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_TermHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,t);}
# 392
if((*_check_null(thinfo))->free_logicvar_set!=0)
return*(*thinfo)->free_logicvar_set;{
struct Cyc_Set_Set*res;
{void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 10: _LL6:
 goto _LL8;case 11: _LL8:
# 400
 res=Cyc_AssnDef_empty_term_set();goto _LL0;case 4:
# 402
 res=Cyc_Set_singleton(Cyc_AssnDef_termcmp,t);
goto _LL0;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp2;void*t2=_Tmp1;
# 405
_Tmp2=t1;_Tmp1=t2;goto _LLE;}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_LLE: {void*t1=_Tmp2;void*t2=_Tmp1;
# 407
_Tmp2=t1;_Tmp1=t2;goto _LL10;}case 13: _Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_LL10: {void*t1=_Tmp2;void*t2=_Tmp1;
# 409
res=({struct Cyc_Set_Set*_Tmp3=Cyc_AssnDef_term_fr_logicvar(t1);Cyc_Set_union_two(_Tmp3,Cyc_AssnDef_term_fr_logicvar(t2));});
goto _LL0;}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp2;void*t2=_Tmp1;void*t3=_Tmp0;
# 412
res=({struct Cyc_Set_Set*_Tmp3=({struct Cyc_Set_Set*_Tmp4=Cyc_AssnDef_term_fr_logicvar(t1);Cyc_Set_union_two(_Tmp4,
Cyc_AssnDef_term_fr_logicvar(t2));});
# 412
Cyc_Set_union_two(_Tmp3,
# 414
Cyc_AssnDef_term_fr_logicvar(t3));});
goto _LL0;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp2;
# 417
_Tmp2=t1;goto _LL16;}case 9: _Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_LL16: {void*t1=_Tmp2;
# 419
_Tmp2=t1;goto _LL18;}case 12: _Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_LL18: {void*t1=_Tmp2;
# 421
_Tmp2=t1;goto _LL1A;}case 14: _Tmp2=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;_LL1A: {void*t1=_Tmp2;
# 423
_Tmp2=t1;goto _LL1C;}case 7: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;_LL1C: {void*t1=_Tmp2;
# 425
res=Cyc_AssnDef_term_fr_logicvar(t1);
goto _LL0;}default: _Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{struct Cyc_List_List*ts=_Tmp2;
# 428
struct Cyc_List_List*tsets=({struct Cyc_List_List*(*_Tmp3)(struct Cyc_Set_Set*(*)(void*),struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_Set_Set*(*)(void*),struct Cyc_List_List*))Cyc_List_map;_Tmp3;})(Cyc_AssnDef_term_fr_logicvar,ts);
res=({struct Cyc_Set_Set*(*_Tmp3)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*)=({struct Cyc_Set_Set*(*_Tmp4)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,struct Cyc_Set_Set*),struct Cyc_Set_Set*,struct Cyc_List_List*))Cyc_List_fold_left;_Tmp4;});struct Cyc_Set_Set*_Tmp4=Cyc_AssnDef_empty_term_set();_Tmp3(Cyc_Set_union_two,_Tmp4,tsets);});
goto _LL0;}}_LL0:;}
# 432
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=res;_Tmp1;});(*thinfo)->free_logicvar_set=_Tmp0;});
return res;}}
# 436
static int Cyc_AssnDef_logicvar_in_term(void*lvar,void*t){
struct Cyc_Set_Set*lvset=Cyc_AssnDef_term_fr_logicvar(t);
return Cyc_Set_member(lvset,lvar);}
# 441
void*Cyc_AssnDef_uint(unsigned i){
struct Cyc_AssnDef_Uint_AssnDef_Term_struct t=({struct Cyc_AssnDef_Uint_AssnDef_Term_struct _Tmp0;_Tmp0.tag=0,_Tmp0.f1=i,_Tmp0.f2=0;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 445
void*Cyc_AssnDef_zero (void){
return(void*)& Cyc_AssnDef_zero_value;}
# 448
void*Cyc_AssnDef_one (void){
return(void*)& Cyc_AssnDef_one_value;}
# 451
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){
struct _tuple11 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{unsigned c=_Tmp2;int known=_Tmp1;
if(known)return Cyc_AssnDef_uint(c);{
struct Cyc_AssnDef_Const_AssnDef_Term_struct t=({struct Cyc_AssnDef_Const_AssnDef_Term_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=e;_Tmp3;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}}
# 457
void*Cyc_AssnDef_addr(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_AssnDef_Addr_AssnDef_Term_struct t=({struct Cyc_AssnDef_Addr_AssnDef_Term_struct _Tmp0;_Tmp0.tag=10,_Tmp0.f1=vd;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 461
void*Cyc_AssnDef_logicvar(struct Cyc_Absyn_Vardecl*vd,int i,void*tp){
struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct t=({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct _Tmp0;_Tmp0.tag=4,_Tmp0.f1=vd,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 465
void*Cyc_AssnDef_update(void*t1,void*t2,void*t3){
{struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp0.f0)==3){if(*((int*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)==10){if(*((int*)_Tmp0.f1)==10){_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*m=_Tmp5;void*a=_Tmp4;struct Cyc_Absyn_Vardecl*x=_Tmp3;void*e1=_Tmp2;struct Cyc_Absyn_Vardecl*y=_Tmp1;
# 469
if(x==y)return Cyc_AssnDef_update(m,t2,t3);
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 473
struct Cyc_AssnDef_Update_AssnDef_Term_struct t=({struct Cyc_AssnDef_Update_AssnDef_Term_struct _Tmp0;_Tmp0.tag=3,_Tmp0.f1=t1,_Tmp0.f2=t2,_Tmp0.f3=t3;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 477
void*Cyc_AssnDef_select(void*t1,void*t2,void*tp){
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==3){if(*((int*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)==10){if(*((int*)_Tmp0.f1)==10){_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*m=_Tmp4;struct Cyc_Absyn_Vardecl*x=_Tmp3;void*v=_Tmp2;struct Cyc_Absyn_Vardecl*y=_Tmp1;
# 481
if(x==y)return v;else{
# 484
return Cyc_AssnDef_select(m,t2,tp);}}}else{goto _LL3;}}else{_LL3: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=_Tmp0.f1;{void*m=_Tmp4;void*a1=_Tmp3;void*v=_Tmp2;void*a2=_Tmp1;
# 487
if(a1==a2)return v;
goto _LL6;}}}else{_LL6: {
# 490
struct Cyc_AssnDef_Select_AssnDef_Term_struct t=({struct Cyc_AssnDef_Select_AssnDef_Term_struct _Tmp5;_Tmp5.tag=2,_Tmp5.f1=t1,_Tmp5.f2=t2,_Tmp5.f3=tp;_Tmp5;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}};}
# 502 "assndef.cyc"
void*Cyc_AssnDef_proj(void*t1,unsigned i,void*tp){
{void*_Tmp0;unsigned _Tmp1;int _Tmp2;if(*((int*)t1)==8){_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f1;_Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;_Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f3;{int is_union=_Tmp2;unsigned tag=_Tmp1;struct Cyc_List_List*ts=_Tmp0;
# 505
int n=Cyc_List_length(ts);
if(!is_union && i < (unsigned)n)
return Cyc_List_nth(ts,(int)i);else{
if(tag==i && n >= 1)
return(void*)_check_null(ts)->hd;else{
goto _LL0;}}}}else{
goto _LL0;}_LL0:;}{
# 513
struct Cyc_AssnDef_Proj_AssnDef_Term_struct t=({struct Cyc_AssnDef_Proj_AssnDef_Term_struct _Tmp0;_Tmp0.tag=9,_Tmp0.f1=t1,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 517
void*Cyc_AssnDef_aggr(int is_union,unsigned tag,struct Cyc_List_List*ts,void*tp){
# 520
struct Cyc_AssnDef_Aggr_AssnDef_Term_struct t=({struct Cyc_AssnDef_Aggr_AssnDef_Term_struct _Tmp0;_Tmp0.tag=8,_Tmp0.f1=is_union,_Tmp0.f2=tag,_Tmp0.f3=ts,_Tmp0.f4=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 523
void*Cyc_AssnDef_struct_aggr(struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(0,-1,ts,tp);}
# 526
void*Cyc_AssnDef_datatype_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(0,tag,ts,tp);}
# 529
void*Cyc_AssnDef_union_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(1,tag,ts,tp);}
# 532
void*Cyc_AssnDef_tagof_tm(void*t1){
unsigned _Tmp0;if(*((int*)t1)==8){_Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;{unsigned tag=_Tmp0;
return Cyc_AssnDef_uint(tag);}}else{
# 536
struct Cyc_AssnDef_Tagof_AssnDef_Term_struct t=({struct Cyc_AssnDef_Tagof_AssnDef_Term_struct _Tmp1;_Tmp1.tag=14,_Tmp1.f1=t1;_Tmp1;});
return Cyc_AssnDef_hash_cons_term((void*)& t);};}
# 541
void*Cyc_AssnDef_cast(void*tp,void*tm){
struct Cyc_AssnDef_Cast_AssnDef_Term_struct t=({struct Cyc_AssnDef_Cast_AssnDef_Term_struct _Tmp0;_Tmp0.tag=7,_Tmp0.f1=tp,_Tmp0.f2=tm;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 546
void*Cyc_AssnDef_alloc(struct Cyc_Absyn_Exp*e,void*offset,void*tp){
static int counter=0;
struct Cyc_AssnDef_Alloc_AssnDef_Term_struct t=({struct Cyc_AssnDef_Alloc_AssnDef_Term_struct _Tmp0;_Tmp0.tag=11,_Tmp0.f1=e,_Tmp0.f2=offset,_Tmp0.f3=counter,_Tmp0.f4=tp;_Tmp0;});
++ counter;
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 553
void*Cyc_AssnDef_offsetf(struct _fat_ptr*f,void*tm,void*tp){
struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct t=({struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct _Tmp0;_Tmp0.tag=12,_Tmp0.f1=f,_Tmp0.f2=tm,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 558
void*Cyc_AssnDef_offseti(void*i,void*tm,void*tp){
struct Cyc_AssnDef_Offseti_AssnDef_Term_struct t=({struct Cyc_AssnDef_Offseti_AssnDef_Term_struct _Tmp0;_Tmp0.tag=13,_Tmp0.f1=i,_Tmp0.f2=tm,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 562
void*Cyc_AssnDef_fresh_var(void*tp){
static int counter=0;
void*t=Cyc_AssnDef_logicvar(0,counter,tp);
++ counter;
return t;}
# 569
void*Cyc_AssnDef_plus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(0U,t1,t2,tp);}
# 572
void*Cyc_AssnDef_minus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(2U,t1,t2,tp);}
# 576
void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop p,void*t,void*tp){
# 578
if((int)p==0)return t;
# 580
if((int)p==2)return({void*_Tmp0=t;void*_Tmp1=Cyc_AssnDef_uint(-1);Cyc_AssnDef_binop(1U,_Tmp0,_Tmp1,tp);});{
struct Cyc_AssnDef_Unop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Unop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=5,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& ptr);}}
# 585
void*Cyc_AssnDef_numelts_term(void*t){
return Cyc_AssnDef_unop(18U,t,Cyc_Absyn_uint_type);}static char _TmpG3[5U]="#Mem";
# 591
static struct _fat_ptr Cyc_AssnDef_memory_var={_TmpG3,_TmpG3,_TmpG3 + 5U};
static struct _tuple0 Cyc_AssnDef_memvar={.f0={.Loc_n={4,0}},.f1=& Cyc_AssnDef_memory_var};
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_AssnDef_void_con={0};
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_AssnDef_vd_type={0,(void*)& Cyc_AssnDef_void_con,0};
# 596
struct Cyc_Absyn_Vardecl Cyc_AssnDef_memory_struct={Cyc_Absyn_Public,& Cyc_AssnDef_memvar,0U,{0,0,0,0,0U},(void*)& Cyc_AssnDef_vd_type,0,0,0,0,0};
# 608
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory=& Cyc_AssnDef_memory_struct;static char _TmpG4[5U]="#Exn";
# 610
static struct _fat_ptr Cyc_AssnDef_exception_var={_TmpG4,_TmpG4,_TmpG4 + 5U};
static struct _tuple0 Cyc_AssnDef_exnvar={.f0={.Loc_n={4,0}},.f1=& Cyc_AssnDef_exception_var};
# 613
struct Cyc_Absyn_Vardecl Cyc_AssnDef_exception_struct={Cyc_Absyn_Public,& Cyc_AssnDef_exnvar,0U,{0,0,0,0,0U},(void*)& Cyc_AssnDef_vd_type,0,0,0,0,0};
# 626
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_exception_vardecl (void){
struct Cyc_Absyn_Vardecl*vd=& Cyc_AssnDef_exception_struct;
({void*_Tmp0=Cyc_Absyn_exn_type();vd->type=_Tmp0;});
return vd;}struct _tuple14{struct Cyc_List_List*f0;int f1;};struct _tuple15{int f0;void*f1;};
# 643
struct _tuple14 Cyc_AssnDef_get_coefficient_term_list(void*t){
struct Cyc_List_List*coefficient_term_list=0;
int res=0;
LOOP: {
void*_Tmp0;void*_Tmp1;unsigned _Tmp2;switch(*((int*)t)){case 0: _Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp2;
res=(int)i;goto _LL0;}case 6: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned i=_Tmp2;
# 650
res=(int)i;
t=t1;
goto LOOP;}}else{_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 654
{unsigned _Tmp3;void*_Tmp4;if(*((int*)t2)==6)switch((int)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f1){case Cyc_Absyn_Times: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3)==0){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3)->f1;{void*t21=_Tmp4;unsigned c2=_Tmp3;
# 656
coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_cycalloc(sizeof(struct _tuple15));_Tmp7->f0=(int)c2,_Tmp7->f1=t21;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL9;}}else{goto _LLE;}case Cyc_Absyn_Plus:
# 659
({int(*_Tmp5)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp5;})(_tag_fat("Plus in the right branch, not a normal form",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
goto _LL9;default: goto _LLE;}else{_LLE:
# 662
 coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_cycalloc(sizeof(struct _tuple15));_Tmp7->f0=1,_Tmp7->f1=t2;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL9;}_LL9:;}
# 665
t=t1;
goto LOOP;}}}else{goto _LL7;}default: _LL7:
# 668
{unsigned _Tmp3;void*_Tmp4;if(*((int*)t)==6){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Times){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t21=_Tmp4;unsigned c2=_Tmp3;
# 670
coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_cycalloc(sizeof(struct _tuple15));_Tmp7->f0=(int)c2,_Tmp7->f1=t21;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL10;}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13:
# 673
 coefficient_term_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple15*_Tmp6=({struct _tuple15*_Tmp7=_cycalloc(sizeof(struct _tuple15));_Tmp7->f0=1,_Tmp7->f1=t;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=coefficient_term_list;_Tmp5;});
goto _LL10;}_LL10:;}
# 676
goto _LL0;}_LL0:;}
# 678
return({struct _tuple14 _Tmp0;_Tmp0.f0=coefficient_term_list,_Tmp0.f1=res;_Tmp0;});}
# 688 "assndef.cyc"
static struct Cyc_List_List*Cyc_AssnDef_imp_merge_coefficient_term_list(int(*less_eq)(void*,void*),struct Cyc_List_List*termlist1,struct Cyc_List_List*termlist2){
# 692
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
# 705
res_tail->tl=termlist1;
res_tail=termlist1;
termlist1=termlist1->tl;}}else{
# 709
if(cmp_result > 0){
if(res_tail==0){
res_tail=termlist2;
termlist2=termlist2->tl;
res=res_tail;}else{
# 715
res_tail->tl=termlist2;
res_tail=termlist2;
termlist2=termlist2->tl;}}else{
# 721
int i=((struct _tuple15*)termlist1->hd)->f0 + ((struct _tuple15*)termlist2->hd)->f0;
if(i!=0){
((struct _tuple15*)termlist1->hd)->f0=i;
if(res_tail==0){
res_tail=termlist1;
res=res_tail;}else{
# 728
res_tail->tl=termlist1;
res_tail=termlist1;}}
# 732
termlist1=termlist1->tl;
termlist2=termlist2->tl;}}}
# 698
1U;}
# 736
if(res_tail==0){
res_tail=termlist1==0?termlist2: termlist1;
res=res_tail;}else{
# 740
res_tail->tl=termlist1==0?termlist2: termlist1;}
# 742
return res;}}
# 748
inline static unsigned Cyc_AssnDef_id_of_primop(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 return 0U;case Cyc_Absyn_Times:
 return 1U;case Cyc_Absyn_Bitand:
 return 4294967295U;case Cyc_Absyn_Bitor:
 return 0U;case Cyc_Absyn_Bitxor:
 return 0U;default:
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_Absynpp_prim2string(p);_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({int(*_Tmp2)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp2;})(_tag_fat("no id for primop %s",sizeof(char),20U),_tag_fat(_Tmp1,sizeof(void*),1));});};}
# 762
static void*Cyc_AssnDef_termlist2term(struct Cyc_List_List*termlist,int newc,enum Cyc_Absyn_Primop p,void*tp){
unsigned id=Cyc_AssnDef_id_of_primop(p);
if(termlist==0)return Cyc_AssnDef_uint((unsigned)newc);{
void*t=(void*)termlist->hd;
termlist=termlist->tl;
while(termlist!=0){
{struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=6,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=(void*)termlist->hd,_Tmp0.f4=tp;_Tmp0;});
t=Cyc_AssnDef_hash_cons_term((void*)& newterm);
termlist=termlist->tl;}
# 768
1U;}
# 772
if(id==(unsigned)newc)return t;else{
# 786 "assndef.cyc"
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=6,_Tmp0.f1=p,_Tmp0.f2=t,({void*_Tmp1=Cyc_AssnDef_uint((unsigned)newc);_Tmp0.f3=_Tmp1;}),_Tmp0.f4=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& newterm);}}}
# 793
static void*Cyc_AssnDef_compute_factor(void*tp,struct _tuple15*coefficient_term){
void*_Tmp0;int _Tmp1;_Tmp1=coefficient_term->f0;_Tmp0=coefficient_term->f1;{int c=_Tmp1;void*t=_Tmp0;
if(c==1)return t;else{
# 797
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newfactor=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp2;_Tmp2.tag=6,_Tmp2.f1=1U,_Tmp2.f2=t,({void*_Tmp3=Cyc_AssnDef_uint((unsigned)c);_Tmp2.f3=_Tmp3;}),_Tmp2.f4=tp;_Tmp2;});
return Cyc_AssnDef_hash_cons_term((void*)& newfactor);}}}
# 802
static int Cyc_AssnDef_inv_termcmp(void*t1,void*t2){
return - Cyc_AssnDef_termcmp(t1,t2);}
# 813 "assndef.cyc"
static void*Cyc_AssnDef_normalize_plus(void*t1,void*t2,void*tp){
struct _tuple14 _Tmp0=Cyc_AssnDef_get_coefficient_term_list(t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*coefficient_term_list1=_Tmp2;int i1=_Tmp1;
struct _tuple14 _Tmp3=Cyc_AssnDef_get_coefficient_term_list(t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*coefficient_term_list2=_Tmp5;int i2=_Tmp4;
struct Cyc_List_List*coefficient_term_list=
Cyc_AssnDef_imp_merge_coefficient_term_list(Cyc_AssnDef_inv_termcmp,coefficient_term_list1,coefficient_term_list2);
# 821
struct Cyc_List_List*term_list=({struct Cyc_List_List*(*_Tmp6)(void*(*)(void*,struct _tuple15*),void*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(void*(*)(void*,struct _tuple15*),void*,struct Cyc_List_List*))Cyc_List_map_c;_Tmp6;})(Cyc_AssnDef_compute_factor,tp,coefficient_term_list);
return Cyc_AssnDef_termlist2term(term_list,i1 + i2,0U,tp);}}}
# 830
static struct _tuple14 Cyc_AssnDef_term2termlist(enum Cyc_Absyn_Primop p,void*t){
struct Cyc_List_List*termlist=0;
int res=(int)Cyc_AssnDef_id_of_primop(p);
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;unsigned _Tmp3;switch(*((int*)t)){case 0: _Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp3;
res=(int)i;goto _LL0;}case 6: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;unsigned i=_Tmp3;
# 837
res=(int)i;
t=t1;
goto LOOP;}else{goto _LL5;}}else{_LL5: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;void*t2=_Tmp0;
# 841
termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t2,_Tmp4->tl=termlist;_Tmp4;});
t=t1;
goto LOOP;}else{goto _LL7;}}default: _LL7:
# 845
 termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t,_Tmp4->tl=termlist;_Tmp4;});
goto _LL0;}_LL0:;}
# 848
return({struct _tuple14 _Tmp0;_Tmp0.f0=termlist,_Tmp0.f1=res;_Tmp0;});}
# 852
inline static unsigned Cyc_AssnDef_eval_binop(enum Cyc_Absyn_Primop p,unsigned i,unsigned j){
switch((int)p){case Cyc_Absyn_Plus:
 return i + j;case Cyc_Absyn_Times:
 return i * j;case Cyc_Absyn_Bitand:
 return i & j;case Cyc_Absyn_Bitor:
 return i | j;case Cyc_Absyn_Bitxor:
 return i ^ j;default:
({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp0;})(_tag_fat("Error in evaluating binop",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}
# 864
static void*Cyc_AssnDef_normalize_assoc_commu(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
struct _tuple14 _Tmp0=Cyc_AssnDef_term2termlist(p,t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*term_list1=_Tmp2;int c1=_Tmp1;
struct _tuple14 _Tmp3=Cyc_AssnDef_term2termlist(p,t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*term_list2=_Tmp5;int c2=_Tmp4;
struct Cyc_List_List*term_list=Cyc_List_imp_merge(Cyc_AssnDef_inv_termcmp,term_list1,term_list2);
unsigned c=Cyc_AssnDef_eval_binop(p,(unsigned)c1,(unsigned)c2);
return Cyc_AssnDef_termlist2term(term_list,(int)c,p,tp);}}}
# 872
inline static int Cyc_AssnDef_is_nonassociative_commutative(enum Cyc_Absyn_Primop p,void*tp){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
# 876
 if(tp!=0){if(*((int*)tp)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)tp)->f1)==2)
return 1;else{goto _LLE;}}else{goto _LLE;}}else{_LLE:
 return 0;};case Cyc_Absyn_Eq:
# 880
 goto _LL8;case Cyc_Absyn_Neq: _LL8:
 return 1;default:
 return 0;};}
# 886
inline static int Cyc_AssnDef_is_associative_commutative(enum Cyc_Absyn_Primop p,void*tp){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
# 890
 if(tp!=0){if(*((int*)tp)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)tp)->f1)==1)
# 892
return 1;else{goto _LL10;}}else{goto _LL10;}}else{_LL10:
 return 0;};case Cyc_Absyn_Bitand:
# 895
 goto _LL8;case Cyc_Absyn_Bitor: _LL8:
 goto _LLA;case Cyc_Absyn_Bitxor: _LLA:
 return 1;default:
 return 0;};}struct _tuple16{enum Cyc_Absyn_Primop f0;void*f1;void*f2;};
# 903
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
{struct _tuple16 _Tmp0=({struct _tuple16 _Tmp1;_Tmp1.f0=p,_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;unsigned _Tmp3;unsigned _Tmp4;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned i=_Tmp4;unsigned j=_Tmp3;
# 907
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_AssnDef_uint(i + j);case Cyc_Absyn_Minus:
 return Cyc_AssnDef_uint(i - j);case Cyc_Absyn_Bitand:
 return Cyc_AssnDef_uint(i & j);case Cyc_Absyn_Bitor:
 return Cyc_AssnDef_uint(i | j);case Cyc_Absyn_Bitxor:
 return Cyc_AssnDef_uint(i ^ j);case Cyc_Absyn_Bitlshift:
 return Cyc_AssnDef_uint(i << j);case Cyc_Absyn_Bitlrshift:
 return Cyc_AssnDef_uint(i >> j);default:
 goto _LL7;}_LL7:;
# 917
goto _LL0;}}else{if(_Tmp0.f0==Cyc_Absyn_Minus)goto _LL3;else{goto _LL5;}}}else{if(_Tmp0.f0==Cyc_Absyn_Minus){_LL3: _Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp2;void*t2=_Tmp1;
# 919
if(tp!=0){if(*((int*)tp)==0){if(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)tp)->f1)==1){
# 922
if(Cyc_AssnDef_termcmp(t1,t2)==0)return Cyc_AssnDef_uint(0U);
# 924
t2=({void*_Tmp5=t2;void*_Tmp6=Cyc_AssnDef_uint(-1);Cyc_AssnDef_binop(1U,_Tmp5,_Tmp6,tp);});
return Cyc_AssnDef_binop(0U,t1,t2,tp);}else{goto _LL1B;}}else{goto _LL1B;}}else{_LL1B:
 goto _LL18;}_LL18:;
# 928
goto _LL0;}}else{_LL5:
# 933
 if(Cyc_AssnDef_is_associative_commutative(p,tp)){
# 938
if((int)p==0)return Cyc_AssnDef_normalize_plus(t1,t2,tp);else{
return Cyc_AssnDef_normalize_assoc_commu(p,t1,t2,tp);}}
# 941
if(Cyc_AssnDef_is_nonassociative_commutative(p,tp)){
if(Cyc_AssnDef_termcmp(t1,t2)> 0){
void*tmpt=t1;
t1=t2;
t2=tmpt;}}
# 948
goto _LL0;}}_LL0:;}{
# 950
struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp1;_Tmp1.tag=6,_Tmp1.f1=p,_Tmp1.f2=t1,_Tmp1.f3=t2,_Tmp1.f4=tp;_Tmp1;});struct Cyc_AssnDef_Binop_AssnDef_Term_struct p=_Tmp0;
return Cyc_AssnDef_hash_cons_term((void*)& p);}}
# 954
void*Cyc_AssnDef_get_term_type(void*t){
void*_Tmp0;unsigned _Tmp1;void*_Tmp2;switch(*((int*)t)){case 0: _Tmp2=(void*)((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp2;
return tp;}case 1: _Tmp2=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
return e->topt;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{void*topt=_Tmp2;
return topt;}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp2;
return tp;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}case 7: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;{void*tp=_Tmp2;
return tp;}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}case 8: _Tmp2=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp2;
return tp;}case 9: _Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp2;unsigned i=_Tmp1;void*tp=_Tmp0;
return tp;}case 10: _Tmp2=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
return Cyc_Absyn_uint_type;}case 3:
 return 0;case 11: _Tmp2=(void*)((struct Cyc_AssnDef_Alloc_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp2;
return tp;}case 12: _Tmp2=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}case 13: _Tmp2=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}default:
 return Cyc_Absyn_uint_type;};}
# 976
static void*Cyc_AssnDef_subst_term(struct Cyc_Dict_Dict dict,void*t,struct Cyc_Hashtable_Table*term_h){
# 978
void**t1=Cyc_Hashtable_lookup_opt(term_h,t);
if(t1!=0)
# 984
return*t1;{
# 986
void*res;
{unsigned _Tmp0;int _Tmp1;enum Cyc_Absyn_Primop _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 10: _LL6:
 goto _LL8;case 11: _LL8:
 res=t;goto _LL0;case 2: _Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;void*t2=_Tmp4;void*tp=_Tmp3;
# 993
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_select(s1,s2,tp);
goto _LL0;}case 3: _Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;void*t2=_Tmp4;void*t3=_Tmp3;
# 998
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
void*s3=Cyc_AssnDef_subst_term(dict,t3,term_h);
res=Cyc_AssnDef_update(s1,s2,s3);
goto _LL0;}case 4:  {
# 1004
void**t1=Cyc_Dict_lookup_opt(dict,t);
if(t1!=0)res=*t1;else{
res=t;}
goto _LL0;}case 5: _Tmp2=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp5;void*tp=_Tmp4;
# 1009
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_unop(p,s1,tp);
goto _LL0;}case 6: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp5;void*t2=_Tmp4;void*tp=_Tmp3;
# 1013
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_binop(p,s1,s2,tp);
goto _LL0;}case 7: _Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp5;void*t1=_Tmp4;
# 1018
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_cast(tp,t1);
goto _LL0;}case 8: _Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int b=_Tmp1;unsigned tag=_Tmp0;struct Cyc_List_List*tlist=_Tmp5;void*tp=_Tmp4;
# 1022
struct Cyc_List_List*slist=0;
for(1;tlist!=0;tlist=tlist->tl){
slist=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({void*_Tmp7=Cyc_AssnDef_subst_term(dict,(void*)tlist->hd,term_h);_Tmp6->hd=_Tmp7;}),_Tmp6->tl=slist;_Tmp6;});}
# 1026
Cyc_List_imp_rev(slist);
res=Cyc_AssnDef_aggr(b,tag,slist,tp);
goto _LL0;}case 9: _Tmp5=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp0=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;unsigned i=_Tmp0;void*tp=_Tmp4;
# 1030
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_proj(s1,i,tp);
goto _LL0;}case 12: _Tmp5=((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Offsetf_AssnDef_Term_struct*)t)->f3;{struct _fat_ptr*f=_Tmp5;void*t1=_Tmp4;void*tp=_Tmp3;
# 1034
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
res=Cyc_AssnDef_offsetf(f,s1,tp);
goto _LL0;}case 13: _Tmp5=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Offseti_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;void*t2=_Tmp4;void*tp=_Tmp3;
# 1038
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_offseti(s1,s2,tp);
goto _LL0;}default: _Tmp5=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp5;
# 1043
res=Cyc_AssnDef_tagof_tm(Cyc_AssnDef_subst_term(dict,t1,term_h));
goto _LL0;}}_LL0:;}
# 1046
Cyc_Hashtable_insert(term_h,t,res);
return res;}}
# 1050
static int Cyc_AssnDef_hash_ptr(void*s){return(int)s;}
# 1053
static void*Cyc_AssnDef_subst_t(struct Cyc_Dict_Dict dict,void*t){
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
return Cyc_AssnDef_subst_term(dict,t,term_h);}
# 1061
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return _tag_fat("true",sizeof(char),5U);case 1:
 return _tag_fat("false",sizeof(char),6U);case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1066
struct _fat_ptr ps;
switch((int)p){case Cyc_AssnDef_Eq:
 ps=_tag_fat("==",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LLB;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LLB;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LLB;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LLB;default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("assn2string primop",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));}_LLB:;
# 1076
return({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_term2string(t1);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)ps;_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)Cyc_AssnDef_term2string(t2);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_aprintf(_tag_fat("%s%s%s",sizeof(char),7U),_tag_fat(_Tmp6,sizeof(void*),3));});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1078
if(prec <= 5)
return({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_a2string(a1,5);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_AssnDef_a2string(a2,5);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s OR %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});else{
# 1081
return({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_a2string(a1,5);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_AssnDef_a2string(a2,5);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("(%s OR %s)",sizeof(char),11U),_tag_fat(_Tmp5,sizeof(void*),2));});}}default: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1083
if(prec <= 10)
return({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_a2string(a1,10);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_AssnDef_a2string(a2,10);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s AND %s",sizeof(char),10U),_tag_fat(_Tmp5,sizeof(void*),2));});else{
# 1086
return({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)Cyc_AssnDef_a2string(a1,10);_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)Cyc_AssnDef_a2string(a2,10);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("(%s AND %s)",sizeof(char),12U),_tag_fat(_Tmp5,sizeof(void*),2));});}}};}
# 1091
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
return Cyc_AssnDef_a2string(a,0);}
# 1103 "assndef.cyc"
int Cyc_AssnDef_assnhash(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 1:
 return 0;case 0:
 return 1;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1108
return({int _Tmp3=({int _Tmp4=Cyc_AssnDef_termhash(t1);_Tmp4 + (int)p;});_Tmp3 + Cyc_AssnDef_termhash(t2);});}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1110
return(int)(((unsigned)a1 + (unsigned)a2 >> 4U)+ 3U);}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1112
return(int)(((unsigned)a2 + (unsigned)a1 >> 4U)+ 5U);}};}
# 1116
inline static int Cyc_AssnDef_assncode(void*a){
switch(*((int*)a)){case 0:
 return 0;case 1:
 return 1;case 2:
 return 2;case 3:
 return 3;default:
 return 4;};}
# 1128
int Cyc_AssnDef_assncmp(void*_a1,void*_a2){
void*a1=_a1;
void*a2=_a2;
LOOP:
 if(a1==a2)return 0;{
int c=(int)(*((const unsigned*)a2)- *((const unsigned*)a1));
if(c!=0)return c;{
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});enum Cyc_AssnDef_Primreln _Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;void*_Tmp6;switch(*((int*)_Tmp0.f0)){case 3: if(*((int*)_Tmp0.f1)==3){_Tmp6=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f2;{void*a11=_Tmp6;void*a12=_Tmp5;void*a21=_Tmp4;void*a22=_Tmp3;
_Tmp6=a11;_Tmp5=a12;_Tmp4=a21;_Tmp3=a22;goto _LL4;}}else{goto _LL7;}case 4: if(*((int*)_Tmp0.f1)==4){_Tmp6=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_LL4: {void*a11=_Tmp6;void*a12=_Tmp5;void*a21=_Tmp4;void*a22=_Tmp3;
# 1138
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c!=0)return c;
a1=a12;
a2=a22;
goto LOOP;}}else{goto _LL7;}case 2: if(*((int*)_Tmp0.f1)==2){_Tmp6=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f3;{void*t11=_Tmp6;enum Cyc_AssnDef_Primreln p1=_Tmp2;void*t12=_Tmp5;void*t21=_Tmp4;enum Cyc_AssnDef_Primreln p2=_Tmp1;void*t22=_Tmp3;
# 1144
int c=(int)p1 - (int)p2;
if(c!=0)return c;
c=Cyc_AssnDef_termcmp(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_termcmp(t12,t22);}}else{goto _LL7;}default: _LL7:
({int(*_Tmp7)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp7;})(_tag_fat("assncmp:  tagof failure",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));};}}}
# 1156
static void*Cyc_AssnDef_copy_assn(void*a){
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
# 1159
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("True assertion should be in the table",sizeof(char),38U),_tag_fat(0U,sizeof(void*),0));case 1:
# 1161
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("False assertion should be in the table",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp3->tag=3,_Tmp3->f1=a1,_Tmp3->f2=a2;_Tmp3;});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp3->tag=4,_Tmp3->f1=a1,_Tmp3->f2=a2;_Tmp3;});}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=p,_Tmp3->f3=t2;_Tmp3;});}};}
# 1170
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 1173
static void*Cyc_AssnDef_neg_assn(void*a){
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp3->tag=4,({void*_Tmp4=Cyc_AssnDef_not(a1);_Tmp3->f1=_Tmp4;}),({void*_Tmp4=Cyc_AssnDef_not(a2);_Tmp3->f2=_Tmp4;});_Tmp3;});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp3->tag=3,({void*_Tmp4=Cyc_AssnDef_not(a1);_Tmp3->f1=_Tmp4;}),({void*_Tmp4=Cyc_AssnDef_not(a2);_Tmp3->f2=_Tmp4;});_Tmp3;});}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1180
switch((int)p){case Cyc_AssnDef_Eq:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=1U,_Tmp3->f3=t2;_Tmp3;});case Cyc_AssnDef_Neq:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t1,_Tmp3->f2=0U,_Tmp3->f3=t2;_Tmp3;});case Cyc_AssnDef_SLt:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=3U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_SLte:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=2U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_ULt:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=5U,_Tmp3->f3=t1;_Tmp3;});case Cyc_AssnDef_ULte:
 return(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp3->tag=2,_Tmp3->f1=t2,_Tmp3->f2=4U,_Tmp3->f3=t1;_Tmp3;});default:
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("undefined prim relation",sizeof(char),24U),_tag_fat(0U,sizeof(void*),0));};}};}struct Cyc_AssnDef_AssnHashedInfo{void*assn;void*negation;struct Cyc_Set_Set**widened_set;struct Cyc_Set_Set**free_logicvar_set;};
# 1203
static struct Cyc_Hashtable_Table*Cyc_AssnDef_assn_hash_cons_table;
# 1205
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_assn_hash_cons_table (void){
# 1207
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_assn_hash_cons_table==0){
h=Cyc_Hashtable_create(221,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_assn_hash_cons_table=h;{
struct Cyc_Set_Set*ast=Cyc_Set_empty(Cyc_AssnDef_assncmp);
struct Cyc_Set_Set*tst=Cyc_AssnDef_empty_term_set();
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_true_assn,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp2->assn=(void*)& Cyc_AssnDef_true_assn,_Tmp2->negation=(void*)& Cyc_AssnDef_false_assn,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=ast;_Tmp4;});_Tmp2->widened_set=_Tmp3;}),({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;_Tmp0(_Tmp1,(void*)& Cyc_AssnDef_false_assn,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp2=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp2->assn=(void*)& Cyc_AssnDef_false_assn,_Tmp2->negation=(void*)& Cyc_AssnDef_true_assn,({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=0;_Tmp4;});_Tmp2->widened_set=_Tmp3;}),({struct Cyc_Set_Set**_Tmp3=({struct Cyc_Set_Set**_Tmp4=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp4=tst;_Tmp4;});_Tmp2->free_logicvar_set=_Tmp3;});_Tmp2;}));});}}else{
# 1216
h=_check_null(Cyc_AssnDef_assn_hash_cons_table);}
# 1218
return h;}
# 1220
static void*Cyc_AssnDef_hash_cons_assn(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**resopt=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*))=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;_Tmp0;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
if(resopt==0){
# 1226
void*a2=Cyc_AssnDef_copy_assn(a);
void*nega2=Cyc_AssnDef_neg_assn(a);
# 1229
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;void*_Tmp2=a2;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp3->assn=a2,_Tmp3->negation=nega2,_Tmp3->widened_set=0,_Tmp3->free_logicvar_set=0;_Tmp3;}));});
({void(*_Tmp0)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=({void(*_Tmp1)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*)=(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnHashedInfo*))Cyc_Hashtable_insert;_Tmp1;});struct Cyc_Hashtable_Table*_Tmp1=h;void*_Tmp2=nega2;_Tmp0(_Tmp1,_Tmp2,({struct Cyc_AssnDef_AssnHashedInfo*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_AssnHashedInfo));_Tmp3->assn=nega2,_Tmp3->negation=a2,_Tmp3->widened_set=0,_Tmp3->free_logicvar_set=0;_Tmp3;}));});
# 1233
return a2;}else{
# 1235
return(*resopt)->assn;}}
# 1238
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a2;else{if(*((int*)_Tmp0.f1)==0)
return a1;else{if(*((int*)_Tmp0.f0)==1)
return a1;else{if(*((int*)_Tmp0.f1)==1)
return a2;else{
# 1248
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=3,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}}};}}
# 1253
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a1;else{if(*((int*)_Tmp0.f1)==0)
return a2;else{if(*((int*)_Tmp0.f0)==1)
return a2;else{if(*((int*)_Tmp0.f1)==1)
return a1;else{
# 1263
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}}};}}
# 1277 "assndef.cyc"
static void*Cyc_AssnDef_reinsert(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1283
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a1)==0)
a1=Cyc_AssnDef_reinsert(a1);
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a2)==0)
a2=Cyc_AssnDef_reinsert(a2);{
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp2;_Tmp2.tag=3,_Tmp2.f1=a1,_Tmp2.f2=a2;_Tmp2;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1290
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a1)==0)
a1=Cyc_AssnDef_reinsert(a1);
if(({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp2)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp2;})(h,a2)==0)
a2=Cyc_AssnDef_reinsert(a2);{
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp2;_Tmp2.tag=4,_Tmp2.f1=a1,_Tmp2.f2=a2;_Tmp2;});
return Cyc_AssnDef_hash_cons_assn((void*)& a);}}default:
# 1297
 return Cyc_AssnDef_hash_cons_assn(a);};}
# 1301
static struct Cyc_Set_Set*Cyc_AssnDef_assn_fr_logicvar(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1308
if((*_check_null(ahinfo))->free_logicvar_set!=0)
return*(*ahinfo)->free_logicvar_set;{
struct Cyc_Set_Set*res;
{void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
# 1314
 res=Cyc_AssnDef_empty_term_set();
goto _LL0;case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1317
_Tmp1=a1;_Tmp0=a2;goto _LL8;}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;_LL8: {void*a1=_Tmp1;void*a2=_Tmp0;
# 1319
res=({struct Cyc_Set_Set*_Tmp2=Cyc_AssnDef_assn_fr_logicvar(a1);Cyc_Set_union_two(_Tmp2,Cyc_AssnDef_assn_fr_logicvar(a2));});
goto _LL0;}default: _Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 1322
res=({struct Cyc_Set_Set*_Tmp2=Cyc_AssnDef_term_fr_logicvar(t1);Cyc_Set_union_two(_Tmp2,Cyc_AssnDef_term_fr_logicvar(t2));});
goto _LL0;}}_LL0:;}
# 1325
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=res;_Tmp1;});(*ahinfo)->free_logicvar_set=_Tmp0;});
return res;}}
# 1329
static int Cyc_AssnDef_logicvar_in_assn(void*lvar,void*a){
struct Cyc_Set_Set*lvset=Cyc_AssnDef_assn_fr_logicvar(a);
return Cyc_Set_member(lvset,lvar);}
# 1336
void*Cyc_AssnDef_not(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1343
return(*_check_null(ahinfo))->negation;}
# 1346
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=t1,_Tmp0.f2=p,_Tmp0.f3=t2;_Tmp0;});
return Cyc_AssnDef_hash_cons_assn((void*)& ptr);}
# 1356
void*Cyc_AssnDef_slt(void*t1,void*t2){return Cyc_AssnDef_prim(t1,2U,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){return Cyc_AssnDef_prim(t1,3U,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){return Cyc_AssnDef_prim(t1,4U,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){return Cyc_AssnDef_prim(t1,5U,t2);}
# 1361
void*Cyc_AssnDef_eq(void*t1,void*t2){
if(Cyc_AssnDef_termcmp(t1,t2)> 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 1365
return Cyc_AssnDef_prim(t1,0U,t2);}
# 1368
void*Cyc_AssnDef_neq(void*t1,void*t2){
if(({void*_Tmp0=t1;Cyc_AssnDef_termcmp(_Tmp0,Cyc_AssnDef_zero());})==0)
return({void*_Tmp0=Cyc_AssnDef_zero();Cyc_AssnDef_prim(_Tmp0,4U,t2);});else{
if(({void*_Tmp0=t2;Cyc_AssnDef_termcmp(_Tmp0,Cyc_AssnDef_zero());})==0)
return({void*_Tmp0=Cyc_AssnDef_zero();Cyc_AssnDef_prim(_Tmp0,4U,t1);});}
if(Cyc_AssnDef_termcmp(t1,t2)> 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 1376
return Cyc_AssnDef_prim(t1,1U,t2);}
# 1379
static struct Cyc_Set_Set*Cyc_AssnDef_widen_it(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_assn_hash_cons_table();
struct Cyc_AssnDef_AssnHashedInfo**ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);
if(ahinfo==0){
a=Cyc_AssnDef_reinsert(a);
ahinfo=({struct Cyc_AssnDef_AssnHashedInfo**(*_Tmp0)(struct Cyc_Hashtable_Table*,void*)=(struct Cyc_AssnDef_AssnHashedInfo**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;_Tmp0;})(h,a);}
# 1386
if((*_check_null(ahinfo))->widened_set!=0)return*(*ahinfo)->widened_set;{
struct Cyc_Set_Set*s;
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
# 1391
({int(*_Tmp3)(struct _fat_ptr,struct _fat_ptr)=(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;_Tmp3;})(_tag_fat("true and false should have widened_set",sizeof(char),39U),_tag_fat(0U,sizeof(void*),0));case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1393
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0 || s2==0)
s=0;else{
# 1398
s=Cyc_Set_union_two(s1,s2);}
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1401
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0)
s=s2;else{
if(s2==0)
s=s1;else{
# 1408
s=Cyc_Set_intersect(s1,s2);}}
goto _LL0;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1411
s=Cyc_Set_singleton(Cyc_AssnDef_assncmp,a);
switch((int)p){case Cyc_AssnDef_Eq:  {
# 1417
void*topt1=Cyc_AssnDef_get_term_type(t1);
void*topt2=Cyc_AssnDef_get_term_type(t2);
if(topt1!=0 && Cyc_Tcutil_is_integral_type(topt1)||
 topt2!=0 && Cyc_Tcutil_is_integral_type(topt2)){
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t2,t1));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t2,t1));});}
# 1426
goto _LLB;}case Cyc_AssnDef_SLt:
# 1428
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
goto _LLB;case Cyc_AssnDef_ULt:
# 1431
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
goto _LLB;default:
 goto _LLB;}_LLB:;
# 1435
goto _LL0;}}_LL0:;}
# 1437
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=s;_Tmp1;});(*ahinfo)->widened_set=_Tmp0;});
return s;}}
# 1442
static void*Cyc_AssnDef_widen(void*a){
# 1444
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(a);
if(sopt==0)return(void*)& Cyc_AssnDef_false_assn;{
void*res=Cyc_Set_fold(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);
return res;}}
# 1450
static void*Cyc_AssnDef_subst_assn(struct Cyc_Dict_Dict dict,void*a,struct Cyc_Hashtable_Table*assn_h,struct Cyc_Hashtable_Table*term_h){
# 1453
void**a1=Cyc_Hashtable_lookup_opt(assn_h,a);
if(a1!=0)
# 1459
return*a1;{
# 1461
void*res;
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 res=a;goto _LL0;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1466
void*s1=Cyc_AssnDef_subst_term(dict,t1,term_h);
void*s2=Cyc_AssnDef_subst_term(dict,t2,term_h);
res=Cyc_AssnDef_prim(s1,p,s2);
goto _LL0;}case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1471
void*b1=Cyc_AssnDef_subst_assn(dict,a1,assn_h,term_h);
void*b2=Cyc_AssnDef_subst_assn(dict,a2,assn_h,term_h);
res=Cyc_AssnDef_and(b1,b2);
goto _LL0;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1476
void*b1=Cyc_AssnDef_subst_assn(dict,a1,assn_h,term_h);
void*b2=Cyc_AssnDef_subst_assn(dict,a2,assn_h,term_h);
res=Cyc_AssnDef_or(b1,b2);
goto _LL0;}}_LL0:;}
# 1481
Cyc_Hashtable_insert(assn_h,a,res);
return res;}}
# 1485
static void*Cyc_AssnDef_subst_a(struct Cyc_Dict_Dict dict,void*a){
# 1488
struct Cyc_Hashtable_Table*assn_h=Cyc_Hashtable_create(107,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
return Cyc_AssnDef_subst_assn(dict,a,assn_h,term_h);}
# 1495
static int Cyc_AssnDef_simple_prv(struct Cyc_Set_Set*ctxt,void*a){
while(1){
{void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1499
if(Cyc_Set_member(ctxt,a))return 1;
switch((int)p){case Cyc_AssnDef_Neq:
# 1502
 return((({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t2,t1));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t2,t1));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));});case Cyc_AssnDef_SLte:
# 1507
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});case Cyc_AssnDef_ULte:
# 1510
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});default:
 return 0;};}case 0:
# 1514
 return 1;case 1:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1517
if(!Cyc_AssnDef_simple_prv(ctxt,a1))return 0;
a=a2;
continue;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1521
if(Cyc_AssnDef_simple_prv(ctxt,a1))return 1;
a=a2;
continue;}};}
# 1497
1U;}}
# 1531
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
if(ctxt==a)return 1;{
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(ctxt);
if(sopt==0)return 1;
return Cyc_AssnDef_simple_prv(sopt,a);}}
# 1540
static struct Cyc_List_List*Cyc_AssnDef_m2string(struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_List_List*stringlist){
struct _fat_ptr res=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_AssnDef_term2string(value);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("%s == %s",sizeof(char),9U),_tag_fat(_Tmp2,sizeof(void*),2));});
return({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=res;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=stringlist;_Tmp0;});}
# 1545
struct _fat_ptr Cyc_AssnDef_map2string(struct Cyc_Dict_Dict m){
struct Cyc_List_List*stringlist=({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_List_List*),struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_m2string,m,0);
struct _fat_ptr s;
if(stringlist==0)return Cyc_aprintf(_tag_fat(" ",sizeof(char),2U),_tag_fat(0U,sizeof(void*),0));else{
s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)*((struct _fat_ptr*)stringlist->hd);_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp1,sizeof(void*),1));});}
while(stringlist->tl!=0){
stringlist=stringlist->tl;
s=({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=(struct _fat_ptr)s;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,_Tmp2.f1=(struct _fat_ptr)*((struct _fat_ptr*)stringlist->hd);_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp2,sizeof(void*),2));});
# 1551
1U;}
# 1554
return s;}
# 1558
struct _fat_ptr Cyc_AssnDef_assnmap2string(struct Cyc_AssnDef_AssnMap am){
return({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)Cyc_AssnDef_assn2string(am.assn);_Tmp1.f1=_Tmp2;});_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=(struct _fat_ptr)Cyc_AssnDef_map2string(am.map);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_aprintf(_tag_fat("%s\n%s",sizeof(char),6U),_tag_fat(_Tmp2,sizeof(void*),2));});}
# 1562
static struct Cyc_Dict_Dict Cyc_AssnDef_empty_map (void){
static struct Cyc_Dict_Dict*mt=0;
if(mt==0){
struct Cyc_Dict_Dict d=({struct Cyc_Dict_Dict(*_Tmp0)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))=(struct Cyc_Dict_Dict(*)(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)))Cyc_Dict_empty;_Tmp0;})(({int(*_Tmp0)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*)=(int(*)(struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Vardecl*))Cyc_Core_ptrcmp;_Tmp0;}));
mt=({struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));*_Tmp0=d;_Tmp0;});
return d;}else{
# 1569
return*mt;}}
# 1572
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_false_assnmap (void){
return({struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=(void*)& Cyc_AssnDef_false_assn,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.map=_Tmp1;});_Tmp0;});}
# 1576
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_true_assnmap (void){
return({struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=(void*)& Cyc_AssnDef_true_assn,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.map=_Tmp1;});_Tmp0;});}struct _tuple17{void*f0;struct Cyc_AssnDef_AssnMap f1;};
# 1583
struct _tuple17 Cyc_AssnDef_lookup_var_map(struct Cyc_Absyn_Vardecl*vd,struct Cyc_AssnDef_AssnMap am){
void**t=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,vd);
if(t!=0)return({struct _tuple17 _Tmp0;_Tmp0.f0=*t,_Tmp0.f1=am;_Tmp0;});{
void*newt=Cyc_AssnDef_fresh_var(vd->type);
struct Cyc_Dict_Dict newm=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(am.map,vd,newt);
return({struct _tuple17 _Tmp0;_Tmp0.f0=newt,_Tmp0.f1.assn=am.assn,_Tmp0.f1.map=newm;_Tmp0;});}}
# 1593
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_update_var_map(struct Cyc_Absyn_Vardecl*vd,void*newt,struct Cyc_AssnDef_AssnMap am){
struct Cyc_Dict_Dict newm=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(am.map,vd,newt);
return({struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=am.assn,_Tmp0.map=newm;_Tmp0;});}
# 1599
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_widen_assnmap(struct Cyc_AssnDef_AssnMap am){
return({struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_widen(am.assn);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;_Tmp0;});}
# 1604
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assn(struct Cyc_AssnDef_AssnMap am,void*a){
return({struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_or(am.assn,a);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;_Tmp0;});}
# 1609
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_and_assnmap_assn(struct Cyc_AssnDef_AssnMap am,void*a){
return({struct Cyc_AssnDef_AssnMap _Tmp0;({void*_Tmp1=Cyc_AssnDef_and(am.assn,a);_Tmp0.assn=_Tmp1;}),_Tmp0.map=am.map;_Tmp0;});}
# 1613
void*Cyc_AssnDef_merge(struct Cyc_Absyn_Vardecl*vd,void*t1,void*t2){
if(t1==t2)return t1;else{
return Cyc_AssnDef_fresh_var(vd->type);}}struct _tuple18{struct Cyc_Hashtable_Table*f0;struct Cyc_Dict_Dict f1;};
# 1618
static void*Cyc_AssnDef_subst_term_internal(struct _tuple18*env,void*t){
return Cyc_AssnDef_subst_term(env->f1,t,env->f0);}
# 1622
static struct Cyc_AssnDef_AssnMap Cyc_AssnDef_subst_am(struct Cyc_Dict_Dict dict,struct Cyc_AssnDef_AssnMap am){
# 1625
struct Cyc_Hashtable_Table*assn_h=Cyc_Hashtable_create(107,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
struct Cyc_Hashtable_Table*term_h=Cyc_Hashtable_create(137,Cyc_Core_ptrcmp,Cyc_AssnDef_hash_ptr);
void*assn=Cyc_AssnDef_subst_assn(dict,am.assn,assn_h,term_h);
struct Cyc_Dict_Dict map=({struct Cyc_Dict_Dict(*_Tmp0)(void*(*)(struct _tuple18*,void*),struct _tuple18*,struct Cyc_Dict_Dict)=({struct Cyc_Dict_Dict(*_Tmp1)(void*(*)(struct _tuple18*,void*),struct _tuple18*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(struct _tuple18*,void*),struct _tuple18*,struct Cyc_Dict_Dict))Cyc_Dict_map_c;_Tmp1;});struct _tuple18*_Tmp1=({struct _tuple18*_Tmp2=_cycalloc(sizeof(struct _tuple18));_Tmp2->f0=term_h,_Tmp2->f1=dict;_Tmp2;});_Tmp0(Cyc_AssnDef_subst_term_internal,_Tmp1,am.map);});
return({struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=assn,_Tmp0.map=map;_Tmp0;});}struct Cyc_AssnDef_Accum{void*a1;void*a2;struct Cyc_Dict_Dict m1;struct Cyc_Dict_Dict m2;struct Cyc_Dict_Dict m;};
# 1639
static int Cyc_AssnDef_logicvar_in_map_internal(void*lvar,struct Cyc_Absyn_Vardecl*dummy,void*value,int in_map){
return in_map || Cyc_AssnDef_logicvar_in_term(lvar,value);}
# 1643
static int Cyc_AssnDef_logicvar_in_map(void*lv,struct Cyc_Dict_Dict m){
return({int(*_Tmp0)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int)=(int(*)(int(*)(void*,struct Cyc_Absyn_Vardecl*,void*,int),void*,struct Cyc_Dict_Dict,int))Cyc_Dict_fold_c;_Tmp0;})(Cyc_AssnDef_logicvar_in_map_internal,lv,m,0);}
# 1650
static struct Cyc_AssnDef_Accum*Cyc_AssnDef_foldm1(struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_AssnDef_Accum*accum){
void**t2=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(accum->m2,vd);
# 1654
if(t2==0){
if(*((int*)t)==4){
# 1657
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,t);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,t);accum->m=_Tmp0;});
return accum;}
# 1662
goto _LL0;}else{
# 1664
goto _LL0;}_LL0:;{
# 1666
void*v=Cyc_AssnDef_fresh_var(vd->type);
({void*_Tmp0=({void*_Tmp1=accum->a1;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,t));});accum->a1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m1,vd,v);accum->m1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,v);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,v);accum->m=_Tmp0;});
return accum;}}
# 1674
if(t==*t2){
# 1677
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,t);accum->m=_Tmp0;});
return accum;}
# 1682
{struct _tuple13 _Tmp0=({struct _tuple13 _Tmp1;_Tmp1.f0=t,_Tmp1.f1=*t2;_Tmp1;});if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){
# 1684
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
# 1686
({void*_Tmp1=({void*_Tmp2=Cyc_AssnDef_eq(t,*t2);Cyc_AssnDef_and(_Tmp2,accum->a2);});accum->a2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m2,vd,t);accum->m2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,t);accum->m=_Tmp1;});
return accum;}else{
if(!Cyc_AssnDef_logicvar_in_assn(*t2,accum->a1)&& !Cyc_AssnDef_logicvar_in_map(*t2,accum->m1)){
# 1692
({void*_Tmp1=({void*_Tmp2=Cyc_AssnDef_eq(t,*t2);Cyc_AssnDef_and(_Tmp2,accum->a1);});accum->a1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m1,vd,*t2);accum->m1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,*t2);accum->m=_Tmp1;});}}
# 1696
goto _LL5;}else{
# 1698
if(!Cyc_AssnDef_logicvar_in_assn(t,accum->a2)&& !Cyc_AssnDef_logicvar_in_map(t,accum->m2)){
# 1700
({void*_Tmp1=({void*_Tmp2=Cyc_AssnDef_eq(t,*t2);Cyc_AssnDef_and(_Tmp2,accum->a2);});accum->a2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m2,vd,t);accum->m2=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,t);accum->m=_Tmp1;});
return accum;}
# 1705
goto _LL5;}}else{if(*((int*)_Tmp0.f1)==4){
# 1707
if(!Cyc_AssnDef_logicvar_in_assn(*t2,accum->a1)&& !Cyc_AssnDef_logicvar_in_map(*t2,accum->m1)){
# 1709
({void*_Tmp1=({void*_Tmp2=Cyc_AssnDef_eq(t,*t2);Cyc_AssnDef_and(_Tmp2,accum->a1);});accum->a1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m1,vd,*t2);accum->m1=_Tmp1;});
({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp2;})(accum->m,vd,*t2);accum->m=_Tmp1;});}
# 1713
goto _LL5;}else{
# 1715
goto _LL5;}}_LL5:;}{
# 1717
void*v=Cyc_AssnDef_fresh_var(vd->type);
({void*_Tmp0=({void*_Tmp1=Cyc_AssnDef_eq(v,t);Cyc_AssnDef_and(_Tmp1,accum->a1);});accum->a1=_Tmp0;});
({void*_Tmp0=({void*_Tmp1=Cyc_AssnDef_eq(v,*t2);Cyc_AssnDef_and(_Tmp1,accum->a2);});accum->a2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m1,vd,v);accum->m1=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m2,vd,v);accum->m2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,v);accum->m=_Tmp0;});
return accum;}}
# 1730
static struct Cyc_AssnDef_Accum*Cyc_AssnDef_foldm2(struct Cyc_Absyn_Vardecl*vd,void*t,struct Cyc_AssnDef_Accum*accum){
{
# 1736
void*v=Cyc_AssnDef_fresh_var(vd->type);
({void*_Tmp0=({void*_Tmp1=Cyc_AssnDef_eq(v,t);Cyc_AssnDef_and(_Tmp1,accum->a2);});accum->a2=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->m,vd,v);accum->m=_Tmp0;});
return accum;};}
# 1744
struct Cyc_Dict_Dict Cyc_AssnDef_empty_term_dict (void){
static struct Cyc_Dict_Dict*term_dict=0;
if(term_dict==0)
term_dict=({struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=Cyc_Dict_empty(Cyc_Core_ptrcmp);*_Tmp0=_Tmp1;});_Tmp0;});
# 1749
return*_check_null(term_dict);}
# 1757
struct _tuple12 Cyc_AssnDef_unify_var_maps(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
if(am1.assn==(void*)& Cyc_AssnDef_false_assn)return({struct _tuple12 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am2.map;_Tmp0;});
if(am2.assn==(void*)& Cyc_AssnDef_false_assn)return({struct _tuple12 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am1.map;_Tmp0;});{
struct Cyc_AssnDef_Accum accum=({struct Cyc_AssnDef_Accum _Tmp0;_Tmp0.a1=am1.assn,_Tmp0.a2=am2.assn,_Tmp0.m1=am1.map,_Tmp0.m2=am2.map,({struct Cyc_Dict_Dict _Tmp1=Cyc_AssnDef_empty_map();_Tmp0.m=_Tmp1;});_Tmp0;});
({struct Cyc_AssnDef_Accum*(*_Tmp0)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=(struct Cyc_AssnDef_Accum*(*)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_foldm1,am1.map,& accum);
({struct Cyc_AssnDef_Accum*(*_Tmp0)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=({struct Cyc_AssnDef_Accum*(*_Tmp1)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*)=(struct Cyc_AssnDef_Accum*(*)(struct Cyc_AssnDef_Accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Accum*))Cyc_Dict_fold;_Tmp1;});_Tmp0(Cyc_AssnDef_foldm2,Cyc_Dict_difference(am2.map,am1.map),& accum);});
return({struct _tuple12 _Tmp0;_Tmp0.f0=accum.a1,_Tmp0.f1=accum.a2,_Tmp0.f2=accum.m;_Tmp0;});}}
# 1767
static struct Cyc_Dict_Dict*Cyc_AssnDef_collect_subst(struct Cyc_Dict_Dict*map2,struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_Dict_Dict*dict){
if(*((int*)value)==4){
# 1773
void**v=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(*map2,vd);
# 1776
if(Cyc_Dict_lookup_opt(*dict,value)==0){
if(v!=0 &&*v==value)
return({struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=Cyc_Dict_insert(*dict,value,*v);*_Tmp0=_Tmp1;});_Tmp0;});
return({struct Cyc_Dict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict _Tmp2=*dict;void*_Tmp3=value;Cyc_Dict_insert(_Tmp2,_Tmp3,Cyc_AssnDef_fresh_var(vd->type));});*_Tmp0=_Tmp1;});_Tmp0;});}
# 1781
goto _LL0;}else{
# 1783
goto _LL0;}_LL0:;
# 1785
return dict;}
# 1795 "assndef.cyc"
struct _tuple12 Cyc_AssnDef_unify_var_maps_subst(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
if(am1.assn==(void*)& Cyc_AssnDef_false_assn)return({struct _tuple12 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am2.map;_Tmp0;});
if(am2.assn==(void*)& Cyc_AssnDef_false_assn)return({struct _tuple12 _Tmp0;_Tmp0.f0=am1.assn,_Tmp0.f1=am2.assn,_Tmp0.f2=am1.map;_Tmp0;});{
# 1801
struct Cyc_Dict_Dict subst_dict=*({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=({struct Cyc_Dict_Dict*(*_Tmp1)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*,struct Cyc_Absyn_Vardecl*,void*,struct Cyc_Dict_Dict*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_Tmp1;});struct Cyc_Dict_Dict*_Tmp1=& am2.map;struct Cyc_Dict_Dict _Tmp2=am1.map;_Tmp0(Cyc_AssnDef_collect_subst,_Tmp1,_Tmp2,({struct Cyc_Dict_Dict*_Tmp3=_cycalloc(sizeof(struct Cyc_Dict_Dict));({struct Cyc_Dict_Dict _Tmp4=Cyc_AssnDef_empty_term_dict();*_Tmp3=_Tmp4;});_Tmp3;}));});
struct Cyc_AssnDef_AssnMap _Tmp0=Cyc_AssnDef_subst_am(subst_dict,am2);struct Cyc_AssnDef_AssnMap am2=_Tmp0;
return Cyc_AssnDef_unify_var_maps(am1,am2);}}
# 1806
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_or_assnmap_assnmap(struct Cyc_AssnDef_AssnMap am1,struct Cyc_AssnDef_AssnMap am2){
struct _tuple12 _Tmp0=Cyc_AssnDef_unify_var_maps(am1,am2);struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*a1=_Tmp3;void*a2=_Tmp2;struct Cyc_Dict_Dict map=_Tmp1;
return({struct Cyc_AssnDef_AssnMap _Tmp4;({void*_Tmp5=Cyc_AssnDef_or(a1,a2);_Tmp4.assn=_Tmp5;}),_Tmp4.map=map;_Tmp4;});}}struct Cyc_AssnDef_Canonical_am_accum{void*assn;struct Cyc_Dict_Dict map;struct Cyc_Set_Set*used_lvars;};
# 1818
static struct Cyc_AssnDef_Canonical_am_accum*Cyc_AssnDef_fold_canonical(struct Cyc_Absyn_Vardecl*vd,void*value,struct Cyc_AssnDef_Canonical_am_accum*accum){
if(*((int*)value)==4){
# 1821
if(!Cyc_Set_member(accum->used_lvars,value)){
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(accum->used_lvars,value);accum->used_lvars=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->map,vd,value);accum->map=_Tmp0;});
return accum;}
# 1826
goto _LL0;}else{
# 1828
goto _LL0;}_LL0:;{
# 1830
void*v=Cyc_AssnDef_fresh_var(vd->type);
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(accum->used_lvars,v);accum->used_lvars=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp1;})(accum->map,vd,v);accum->map=_Tmp0;});
({void*_Tmp0=({void*_Tmp1=accum->assn;Cyc_AssnDef_and(_Tmp1,Cyc_AssnDef_eq(v,value));});accum->assn=_Tmp0;});
return accum;}}
# 1841
struct Cyc_AssnDef_AssnMap Cyc_AssnDef_canonical_assnmap(struct Cyc_AssnDef_AssnMap am){
struct Cyc_AssnDef_Canonical_am_accum*accum;accum=_cycalloc(sizeof(struct Cyc_AssnDef_Canonical_am_accum)),accum->assn=am.assn,({struct Cyc_Dict_Dict _Tmp0=Cyc_AssnDef_empty_map();accum->map=_Tmp0;}),({struct Cyc_Set_Set*_Tmp0=Cyc_Set_empty(Cyc_Core_ptrcmp);accum->used_lvars=_Tmp0;});
({struct Cyc_AssnDef_Canonical_am_accum*(*_Tmp0)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*)=(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_AssnDef_Canonical_am_accum*(*)(struct Cyc_Absyn_Vardecl*,void*,struct Cyc_AssnDef_Canonical_am_accum*),struct Cyc_Dict_Dict,struct Cyc_AssnDef_Canonical_am_accum*))Cyc_Dict_fold;_Tmp0;})(Cyc_AssnDef_fold_canonical,am.map,accum);
return({struct Cyc_AssnDef_AssnMap _Tmp0;_Tmp0.assn=accum->assn,_Tmp0.map=accum->map;_Tmp0;});}
# 1854 "assndef.cyc"
void*Cyc_AssnDef_merge_assnmaps(struct Cyc_List_List*vds1,struct Cyc_AssnDef_AssnMap a1,struct Cyc_List_List*vds2,struct Cyc_AssnDef_AssnMap a2){
# 1856
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
for(1;vds1!=0 && vds2!=0;(vds1=vds1->tl,vds2=vds2->tl)){
if((struct Cyc_Absyn_Vardecl*)vds1->hd!=0 &&(struct Cyc_Absyn_Vardecl*)vds2->hd!=0){
void**v1=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a1.map,_check_null((struct Cyc_Absyn_Vardecl*)vds1->hd));
void**v2=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a2.map,_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd));
if(v1!=0 && v2!=0)
dict=Cyc_Dict_insert(dict,*v2,*v1);}}
# 1872
if(Cyc_Dict_is_empty(dict))return a2.assn;else{
return Cyc_AssnDef_subst_a(dict,a2.assn);}}
# 1884 "assndef.cyc"
extern struct Cyc_AssnDef_AssnMap*Cyc_AssnDef_subst_vardecls(struct Cyc_List_List*vds1,struct Cyc_List_List*vds2,struct Cyc_AssnDef_AssnMap*a){
# 1887
if(a==0)return 0;{
struct Cyc_Dict_Dict newmap=Cyc_AssnDef_empty_map();
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
for(1;vds1!=0 && vds2!=0;(vds1=vds1->tl,vds2=vds2->tl)){
if((struct Cyc_Absyn_Vardecl*)vds1->hd!=0 &&(struct Cyc_Absyn_Vardecl*)vds2->hd!=0){
void**v=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(a->map,_check_null((struct Cyc_Absyn_Vardecl*)vds1->hd));
if(v!=0){
void*u=Cyc_AssnDef_fresh_var(_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd)->type);
# 1896
newmap=({struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*,void*))Cyc_Dict_insert;_Tmp0;})(newmap,_check_null((struct Cyc_Absyn_Vardecl*)vds2->hd),u);
# 1898
dict=Cyc_Dict_insert(dict,*v,u);}}}{
# 1903
void*newa=Cyc_AssnDef_subst_a(dict,a->assn);
return({struct Cyc_AssnDef_AssnMap*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_AssnMap));_Tmp0->assn=newa,_Tmp0->map=newmap;_Tmp0;});}}}
# 1912
void*Cyc_AssnDef_subst_args(struct Cyc_List_List*args,struct Cyc_Absyn_Vardecl*res,struct Cyc_List_List*actual_args,void*actual_res,struct Cyc_AssnDef_AssnMap am){
# 1915
struct Cyc_Dict_Dict dict=Cyc_AssnDef_empty_term_dict();
if(res!=0 && actual_res!=0){
void**res_val=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,res);
if(res_val!=0)
dict=Cyc_Dict_insert(dict,*res_val,actual_res);}
# 1921
for(1;args!=0 && actual_args!=0;(args=args->tl,actual_args=actual_args->tl)){
if((struct Cyc_Absyn_Vardecl*)args->hd!=0 &&(void*)actual_args->hd!=0){
void**arg_val=({void**(*_Tmp0)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*)=(void**(*)(struct Cyc_Dict_Dict,struct Cyc_Absyn_Vardecl*))Cyc_Dict_lookup_opt;_Tmp0;})(am.map,_check_null((struct Cyc_Absyn_Vardecl*)args->hd));
if(arg_val!=0)
dict=Cyc_Dict_insert(dict,*arg_val,(void*)actual_args->hd);}}
# 1928
return Cyc_AssnDef_subst_a(dict,am.assn);}
# 1931
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_term_hash_cons_table=0;
Cyc_AssnDef_assn_hash_cons_table=0;}
