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

# 76 "flags.h"
 extern int Cyc_Flags_debug_assn;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 145
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 234
extern struct Cyc_List_List*Cyc_List_imp_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 242
extern void*Cyc_List_nth(struct Cyc_List_List*,int);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 135 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 157
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f0;char f1;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f0;short f1;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f0;int f1;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f0;long long f1;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f0;int f1;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 481 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};
# 904 "absyn.h"
extern void*Cyc_Absyn_uint_type;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
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
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct _tuple11{unsigned f0;int f1;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 35 "tcutil.h"
int Cyc_Tcutil_is_integral_type(void*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
# 64 "string.h"
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));
# 82
extern int Cyc_Hashtable_hash_string(struct _fat_ptr);struct Cyc_Set_Set;
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
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_Select_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Update_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Aggr_AssnDef_Term_struct{int tag;int f1;unsigned f2;struct Cyc_List_List*f3;void*f4;};struct Cyc_AssnDef_Proj_AssnDef_Term_struct{int tag;void*f1;unsigned f2;void*f3;};struct Cyc_AssnDef_Okderef_AssnDef_Term_struct{int tag;void*f1;};struct Cyc_AssnDef_Tagof_AssnDef_Term_struct{int tag;void*f1;};
# 107 "assndef.h"
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);struct _tuple12{struct Cyc_List_List*f0;int f1;};
# 131
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Not_AssnDef_Assn_struct{int tag;void*f1;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};
# 146
struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 150
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 185 "assndef.h"
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);static char _TmpG0[2U]=",";static char _TmpG1[2U]="}";static char _TmpG2[2U]="{";
# 35 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
struct _fat_ptr res;
{enum Cyc_Absyn_Primop _Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("0x%x",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
res=Cyc_Absynpp_exp2string(e);goto _LL0;}case 13: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("tagof(%s)",sizeof(char),10U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 2: _Tmp4=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 42
if(Cyc_Flags_debug_assn)
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned)vd;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s_%x",sizeof(char),6U),_tag_fat(_Tmp8,sizeof(void*),2));});else{
# 45
res=Cyc_Absynpp_qvar2string(vd->name);}
goto _LL0;}case 3: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 48
if(Cyc_Flags_debug_assn)
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned)vd;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("&%s_%x",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});else{
# 51
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("&%s",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));});}
goto _LL0;}case 4: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp3;
# 54
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("Sel(%s,%s)",sizeof(char),11U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;
# 57
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t3));_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Upd(%s,%s,%s)",sizeof(char),14U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 6: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1==0){_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{int i=_Tmp1;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("_X%d",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}}else{_Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp1;
# 61
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_check_null(vd)->name));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("_%s%d",sizeof(char),6U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}}case 8: _Tmp0=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;void*t2=_Tmp3;
# 63
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("(%s%s%s)",sizeof(char),9U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 7: _Tmp0=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;
# 67
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s(%s)",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp4;void*tm=_Tmp3;
# 69
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(tm));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("(%s)%s",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}case 10: _Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{int is_union=_Tmp1;unsigned tag=_Tmp5;struct Cyc_List_List*tms=_Tmp4;
# 71
static struct _fat_ptr lb={_TmpG2,_TmpG2,_TmpG2 + 2U};
static struct _fat_ptr rb={_TmpG1,_TmpG1,_TmpG1 + 2U};
static struct _fat_ptr comma={_TmpG0,_TmpG0,_TmpG0 + 2U};
struct Cyc_List_List*ss;ss=_cycalloc(sizeof(struct Cyc_List_List)),ss->hd=& lb,ss->tl=0;
for(1;tms!=0;tms=tms->tl){
struct _fat_ptr s=Cyc_AssnDef_term2string((void*)tms->hd);
ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));({struct _fat_ptr*_Tmp7=({struct _fat_ptr*_Tmp8=_cycalloc(sizeof(struct _fat_ptr));*_Tmp8=s;_Tmp8;});_Tmp6->hd=_Tmp7;}),_Tmp6->tl=ss;_Tmp6;});
if(tms->tl!=0)ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=& comma,_Tmp6->tl=ss;_Tmp6;});}
# 80
ss=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=& rb,_Tmp6->tl=ss;_Tmp6;});
ss=Cyc_List_imp_rev(ss);
res=Cyc_strconcat_l(ss);
if(tag!=4294967295U)res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)tag);_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,_Tmp8.f1=(struct _fat_ptr)((struct _fat_ptr)res);_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("tag(%d)%s",sizeof(char),10U),_tag_fat(_Tmp8,sizeof(void*),2));});
if(is_union)res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,_Tmp7.f1=(struct _fat_ptr)((struct _fat_ptr)res);_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("union%s",sizeof(char),8U),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL0;}case 11: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;unsigned i=_Tmp5;
# 87
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)((int)i);_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("#%d(%s)",sizeof(char),8U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}default: _Tmp4=(void*)((struct Cyc_AssnDef_Okderef_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
# 90
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("Okderef(%s)",sizeof(char),12U),_tag_fat(_Tmp7,sizeof(void*),1));});
goto _LL0;}}_LL0:;}
# 93
return res;}
# 96
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
# 107
return Cyc_Hashtable_hash_string(s);}};}
# 111
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
# 136
int Cyc_AssnDef_termhash(void*t){
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;int _Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
return(int)i;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return Cyc_AssnDef_hash_exp(e)+ 61;}case 2: _Tmp4=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(int)((unsigned)vd);}case 6: _Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp3;
return(int)((unsigned)vd ^ (unsigned)(i + 1));}case 8: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp4;void*t2=_Tmp1;
# 143
return(int)(((unsigned)p ^ (unsigned)t1)^ (unsigned)t2);}case 7: _Tmp2=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp4;
return(int)((unsigned)p ^ (unsigned)t1 + 1U);}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;
return Cyc_AssnDef_termhash(t);}case 4: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp1;
# 147
return(int)((unsigned)t1 ^ (unsigned)t2 + 2U);}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp1;void*t3=_Tmp0;
# 149
return(int)(((unsigned)t1 ^ (unsigned)t2)^ (unsigned)t3 + 3U);}case 3: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(int)((unsigned)vd + 4U);}case 11: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;unsigned i=_Tmp5;
return(int)((unsigned)t ^ i);}case 10: _Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;{int is_union=_Tmp3;unsigned tag=_Tmp5;struct Cyc_List_List*ts=_Tmp4;
# 153
unsigned res=(37U + tag)+ (unsigned)is_union;
for(1;ts!=0;ts=ts->tl){
res=res ^ (unsigned)((void*)ts->hd);}
return(int)res;}case 12: _Tmp4=(void*)((struct Cyc_AssnDef_Okderef_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
return(int)((unsigned)t ^ 97U);}default: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
return(int)((unsigned)t ^ 741U);}};}struct _tuple24{void*f0;void*f1;};
# 162
int Cyc_AssnDef_cmp_term(void*t1,void*t2){
struct _tuple24 _Tmp0=({struct _tuple24 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;enum Cyc_Absyn_Primop _Tmp4;void*_Tmp5;void*_Tmp6;int _Tmp7;int _Tmp8;void*_Tmp9;void*_TmpA;unsigned _TmpB;unsigned _TmpC;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_TmpC=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i1=_TmpC;unsigned i2=_TmpB;
return(int)(i1 - i2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==0)
return 1;else{if(*((int*)_Tmp0.f0)==1){if(*((int*)_Tmp0.f1)==1){_TmpA=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Exp*e1=_TmpA;struct Cyc_Absyn_Exp*e2=_Tmp9;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==1)
return 1;else{if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2){_TmpA=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl*vd1=_TmpA;struct Cyc_Absyn_Vardecl*vd2=_Tmp9;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==2)
return 1;else{if(*((int*)_Tmp0.f0)==3){if(*((int*)_Tmp0.f1)==3){_TmpA=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{struct Cyc_Absyn_Vardecl*vd1=_TmpA;struct Cyc_Absyn_Vardecl*vd2=_Tmp9;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==3)
return 1;else{if(*((int*)_Tmp0.f0)==6){if(*((int*)_Tmp0.f1)==6){_TmpA=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp8=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f2;{struct Cyc_Absyn_Vardecl*vd1=_TmpA;int i=_Tmp8;struct Cyc_Absyn_Vardecl*vd2=_Tmp9;int j=_Tmp7;
# 177
if(vd1==vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==6)
return 1;else{if(*((int*)_Tmp0.f0)==9){if(*((int*)_Tmp0.f1)==9){_TmpA=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tp1=_TmpA;void*tm1=_Tmp9;void*tp2=_Tmp6;void*tm2=_Tmp5;
# 182
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(tm1,tm2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==9)
return 1;else{if(*((int*)_Tmp0.f0)==7){if(*((int*)_Tmp0.f1)==7){_Tmp4=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;{enum Cyc_Absyn_Primop p1=_Tmp4;void*t1=_TmpA;enum Cyc_Absyn_Primop p2=_Tmp3;void*t2=_Tmp9;
# 188
if((int)p1!=(int)p2)return(int)p1 - (int)p2;
return Cyc_AssnDef_cmp_term(t1,t2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==7)
return 1;else{if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_TmpA=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*t11=_TmpA;void*t12=_Tmp9;void*t21=_Tmp6;void*t22=_Tmp5;
# 193
int c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==4)
return 1;else{if(*((int*)_Tmp0.f0)==5){if(*((int*)_Tmp0.f1)==5){_TmpA=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*t11=_TmpA;void*t12=_Tmp9;void*t13=_Tmp6;void*t21=_Tmp5;void*t22=_Tmp2;void*t23=_Tmp1;
# 199
int c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;{
int c=Cyc_AssnDef_cmp_term(t12,t22);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t13,t23);}}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==5)
return 1;else{if(*((int*)_Tmp0.f0)==11){if(*((int*)_Tmp0.f1)==11){_TmpA=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpC=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f1;_TmpB=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*t1=_TmpA;unsigned i1=_TmpC;void*t2=_Tmp9;unsigned i2=_TmpB;
# 207
int c=(int)(i1 - i2);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t1,t2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==11)
return 1;else{if(*((int*)_Tmp0.f0)==12){if(*((int*)_Tmp0.f1)==12){_TmpA=(void*)((struct Cyc_AssnDef_Okderef_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Okderef_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*t1=_TmpA;void*t2=_Tmp9;
# 213
return Cyc_AssnDef_cmp_term(t1,t2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==12)
return 1;else{if(*((int*)_Tmp0.f0)==13){if(*((int*)_Tmp0.f1)==13){_TmpA=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*t1=_TmpA;void*t2=_Tmp9;
return Cyc_AssnDef_cmp_term(t1,t2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==13)
return 1;else{if(*((int*)_Tmp0.f0)==10){if(*((int*)_Tmp0.f1)==10){_Tmp8=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpC=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f2;_TmpA=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp7=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f1;_TmpB=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp9=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)_Tmp0.f1)->f3;{int is_union1=_Tmp8;unsigned tag1=_TmpC;struct Cyc_List_List*ts1=_TmpA;int is_union2=_Tmp7;unsigned tag2=_TmpB;struct Cyc_List_List*ts2=_Tmp9;
# 220
int c=(int)(tag1 - tag2);
if(c!=0)return c;
c=is_union2 - is_union1;
if(c!=0)return c;
for(1;ts1!=0 && ts2!=0;(ts1=ts1->tl,ts2=ts2->tl)){
c=Cyc_AssnDef_cmp_term((void*)ts1->hd,(void*)ts2->hd);
if(c!=0)return c;}
# 228
if(ts1==ts2)return 0;
if(ts1==0)return -1;
return 1;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==10)
return 1;else{_Tmp4=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f4;{enum Cyc_Absyn_Primop p1=_Tmp4;void*t11=_TmpA;void*t12=_Tmp9;void*tp1=_Tmp6;enum Cyc_Absyn_Primop p2=_Tmp3;void*t21=_Tmp5;void*t22=_Tmp2;void*tp2=_Tmp1;
# 234
if((int)p1!=(int)p2)return(int)p1 - (int)p2;{
int c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}}}}}}}}}}}}}}}}}}}}}}}}}}};}
# 241
static void*Cyc_AssnDef_copy_term(void*t){
void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;int _Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
return(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Uint_AssnDef_Term_struct));_Tmp6->tag=0,_Tmp6->f1=i;_Tmp6;});}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Const_AssnDef_Term_struct));_Tmp6->tag=1,_Tmp6->f1=e;_Tmp6;});}case 2: _Tmp4=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct));_Tmp6->tag=2,_Tmp6->f1=vd;_Tmp6;});}case 3: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(void*)({struct Cyc_AssnDef_Addr_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Addr_AssnDef_Term_struct));_Tmp6->tag=3,_Tmp6->f1=vd;_Tmp6;});}case 6: _Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp3;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct));_Tmp6->tag=6,_Tmp6->f1=vd,_Tmp6->f2=i,_Tmp6->f3=tp;_Tmp6;});}case 8: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp4;void*t2=_Tmp2;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Binop_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Binop_AssnDef_Term_struct));_Tmp6->tag=8,_Tmp6->f1=p,_Tmp6->f2=t1,_Tmp6->f3=t2,_Tmp6->f4=tp;_Tmp6;});}case 7: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*t=_Tmp4;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_Unop_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Unop_AssnDef_Term_struct));_Tmp6->tag=7,_Tmp6->f1=p,_Tmp6->f2=t,_Tmp6->f3=tp;_Tmp6;});}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp4;void*t=_Tmp2;
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Cast_AssnDef_Term_struct));_Tmp6->tag=9,_Tmp6->f1=tp,_Tmp6->f2=t;_Tmp6;});}case 4: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp2;void*tp=_Tmp0;
return(void*)({struct Cyc_AssnDef_Select_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Select_AssnDef_Term_struct));_Tmp6->tag=4,_Tmp6->f1=t1,_Tmp6->f2=t2,_Tmp6->f3=tp;_Tmp6;});}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp2;void*t3=_Tmp0;
return(void*)({struct Cyc_AssnDef_Update_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Update_AssnDef_Term_struct));_Tmp6->tag=5,_Tmp6->f1=t1,_Tmp6->f2=t2,_Tmp6->f3=t3;_Tmp6;});}case 11: _Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp4;unsigned i=_Tmp5;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_Proj_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Proj_AssnDef_Term_struct));_Tmp6->tag=11,_Tmp6->f1=t,_Tmp6->f2=i,_Tmp6->f3=tp;_Tmp6;});}case 10: _Tmp3=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp4=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int is_union=_Tmp3;unsigned tag=_Tmp5;struct Cyc_List_List*ts=_Tmp4;void*tp=_Tmp2;
return(void*)({struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Aggr_AssnDef_Term_struct));_Tmp6->tag=10,_Tmp6->f1=is_union,_Tmp6->f2=tag,_Tmp6->f3=ts,_Tmp6->f4=tp;_Tmp6;});}case 12: _Tmp4=(void*)((struct Cyc_AssnDef_Okderef_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
return(void*)({struct Cyc_AssnDef_Okderef_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Okderef_AssnDef_Term_struct));_Tmp6->tag=12,_Tmp6->f1=t;_Tmp6;});}default: _Tmp4=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t=_Tmp4;
return(void*)({struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Tagof_AssnDef_Term_struct));_Tmp6->tag=13,_Tmp6->f1=t;_Tmp6;});}};}
# 261
static struct Cyc_Hashtable_Table*Cyc_AssnDef_term_hash_cons_table=0;
# 263
struct Cyc_AssnDef_Uint_AssnDef_Term_struct Cyc_AssnDef_zero_value={0,0U};
struct Cyc_AssnDef_Uint_AssnDef_Term_struct Cyc_AssnDef_one_value={0,1U};
# 266
static void*Cyc_AssnDef_hash_cons_term(void*t){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_term_hash_cons_table==0){
h=Cyc_Hashtable_create(123,Cyc_AssnDef_cmp_term,Cyc_AssnDef_termhash);
Cyc_AssnDef_term_hash_cons_table=h;
Cyc_Hashtable_insert(h,(void*)& Cyc_AssnDef_zero_value,(void*)& Cyc_AssnDef_zero_value);
Cyc_Hashtable_insert(h,(void*)& Cyc_AssnDef_one_value,(void*)& Cyc_AssnDef_one_value);}else{
# 274
h=_check_null(Cyc_AssnDef_term_hash_cons_table);}{
# 276
void**resopt=Cyc_Hashtable_lookup_other_opt(h,t,Cyc_AssnDef_cmp_term,Cyc_AssnDef_termhash);
if(resopt==0){
void*t2=Cyc_AssnDef_copy_term(t);
Cyc_Hashtable_insert(h,t2,t2);
return t2;}else{
return*resopt;}}}
# 286
void*Cyc_AssnDef_uint(unsigned i){
struct Cyc_AssnDef_Uint_AssnDef_Term_struct t=({struct Cyc_AssnDef_Uint_AssnDef_Term_struct _Tmp0;_Tmp0.tag=0,_Tmp0.f1=i;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 290
void*Cyc_AssnDef_zero (void){
return(void*)& Cyc_AssnDef_zero_value;}
# 293
void*Cyc_AssnDef_one (void){
return(void*)& Cyc_AssnDef_one_value;}
# 296
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){
struct _tuple11 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{unsigned c=_Tmp2;int known=_Tmp1;
if(known)return Cyc_AssnDef_uint(c);{
struct Cyc_AssnDef_Const_AssnDef_Term_struct t=({struct Cyc_AssnDef_Const_AssnDef_Term_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=e;_Tmp3;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}}
# 302
void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_AssnDef_Var_AssnDef_Term_struct t=({struct Cyc_AssnDef_Var_AssnDef_Term_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=vd;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 306
void*Cyc_AssnDef_addr(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_AssnDef_Addr_AssnDef_Term_struct t=({struct Cyc_AssnDef_Addr_AssnDef_Term_struct _Tmp0;_Tmp0.tag=3,_Tmp0.f1=vd;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 310
void*Cyc_AssnDef_logicvar(struct Cyc_Absyn_Vardecl*vd,int i,void*tp){
struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct t=({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct _Tmp0;_Tmp0.tag=6,_Tmp0.f1=vd,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 314
void*Cyc_AssnDef_update(void*t1,void*t2,void*t3){
{struct _tuple24 _Tmp0=({struct _tuple24 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp0.f0)==5){if(*((int*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)==3){if(*((int*)_Tmp0.f1)==3){_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*m=_Tmp5;void*a=_Tmp4;struct Cyc_Absyn_Vardecl*x=_Tmp3;void*e1=_Tmp2;struct Cyc_Absyn_Vardecl*y=_Tmp1;
# 318
if(x==y)return Cyc_AssnDef_update(m,t2,t3);
# 325
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 328
struct Cyc_AssnDef_Update_AssnDef_Term_struct t=({struct Cyc_AssnDef_Update_AssnDef_Term_struct _Tmp0;_Tmp0.tag=5,_Tmp0.f1=t1,_Tmp0.f2=t2,_Tmp0.f3=t3;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 331
void*Cyc_AssnDef_select(void*t1,void*t2,void*tp){
struct _tuple24 _Tmp0=({struct _tuple24 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==5){if(*((int*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)==3){if(*((int*)_Tmp0.f1)==3){_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*m=_Tmp4;struct Cyc_Absyn_Vardecl*x=_Tmp3;void*v=_Tmp2;struct Cyc_Absyn_Vardecl*y=_Tmp1;
# 335
if(x==y)return v;else{
# 338
return Cyc_AssnDef_select(m,t2,tp);}}}else{goto _LL3;}}else{_LL3: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=_Tmp0.f1;{void*m=_Tmp4;void*a1=_Tmp3;void*v=_Tmp2;void*a2=_Tmp1;
# 341
if(a1==a2)return v;
goto _LL6;}}}else{_LL6: {
# 344
struct Cyc_AssnDef_Select_AssnDef_Term_struct t=({struct Cyc_AssnDef_Select_AssnDef_Term_struct _Tmp5;_Tmp5.tag=4,_Tmp5.f1=t1,_Tmp5.f2=t2,_Tmp5.f3=tp;_Tmp5;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}};}
# 356 "assndef.cyc"
void*Cyc_AssnDef_proj(void*t1,unsigned i,void*tp){
{void*_Tmp0;unsigned _Tmp1;int _Tmp2;if(*((int*)t1)==10){_Tmp2=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f1;_Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;_Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f3;{int is_union=_Tmp2;unsigned tag=_Tmp1;struct Cyc_List_List*ts=_Tmp0;
# 359
int n=Cyc_List_length(ts);
if(!is_union && i < (unsigned)n)
return Cyc_List_nth(ts,(int)i);else{
if(tag==i && n >= 1)
return(void*)_check_null(ts)->hd;else{
goto _LL0;}}}}else{
goto _LL0;}_LL0:;}{
# 367
struct Cyc_AssnDef_Proj_AssnDef_Term_struct t=({struct Cyc_AssnDef_Proj_AssnDef_Term_struct _Tmp0;_Tmp0.tag=11,_Tmp0.f1=t1,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 370
void*Cyc_AssnDef_tagof(void*t1){
unsigned _Tmp0;if(*((int*)t1)==10){_Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t1)->f2;{unsigned tag=_Tmp0;
return Cyc_AssnDef_uint(tag);}}else{
# 374
struct Cyc_AssnDef_Tagof_AssnDef_Term_struct t=({struct Cyc_AssnDef_Tagof_AssnDef_Term_struct _Tmp1;_Tmp1.tag=13,_Tmp1.f1=t1;_Tmp1;});
return Cyc_AssnDef_hash_cons_term((void*)& t);};}
# 378
void*Cyc_AssnDef_aggr(int is_union,unsigned tag,struct Cyc_List_List*ts,void*tp){
# 381
struct Cyc_AssnDef_Aggr_AssnDef_Term_struct t=({struct Cyc_AssnDef_Aggr_AssnDef_Term_struct _Tmp0;_Tmp0.tag=10,_Tmp0.f1=is_union,_Tmp0.f2=tag,_Tmp0.f3=ts,_Tmp0.f4=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 384
void*Cyc_AssnDef_struct_aggr(struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(0,-1,ts,tp);}
# 387
void*Cyc_AssnDef_datatype_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(0,tag,ts,tp);}
# 390
void*Cyc_AssnDef_union_aggr(unsigned tag,struct Cyc_List_List*ts,void*tp){
return Cyc_AssnDef_aggr(1,tag,ts,tp);}
# 393
void*Cyc_AssnDef_okderef(void*t1){
struct Cyc_AssnDef_Okderef_AssnDef_Term_struct t=({struct Cyc_AssnDef_Okderef_AssnDef_Term_struct _Tmp0;_Tmp0.tag=12,_Tmp0.f1=t1;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 397
inline static int Cyc_AssnDef_is_nonassociative_commutative(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Eq:
 goto _LL4;case Cyc_Absyn_Neq: _LL4:
 return 1;default:
 return 0;};}
# 405
inline static int Cyc_AssnDef_is_associative_commutative(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
 goto _LL6;case Cyc_Absyn_Bitand: _LL6:
 goto _LL8;case Cyc_Absyn_Bitor: _LL8:
 goto _LLA;case Cyc_Absyn_Bitxor: _LLA:
 return 1;default:
 return 0;};}
# 419
inline static unsigned Cyc_AssnDef_id_of_primop(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 return 0U;case Cyc_Absyn_Times:
 return 1U;case Cyc_Absyn_Bitand:
 return 4294967295U;case Cyc_Absyn_Bitor:
 return 0U;case Cyc_Absyn_Bitxor:
 return 0U;default:
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,({struct _fat_ptr _Tmp2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp1.f1=_Tmp2;});_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("no id for primop %s",sizeof(char),20U),_tag_fat(_Tmp1,sizeof(void*),1));});};}
# 431
inline static unsigned Cyc_AssnDef_eval_binop(enum Cyc_Absyn_Primop p,unsigned i,unsigned j){
switch((int)p){case Cyc_Absyn_Plus:
 return i + j;case Cyc_Absyn_Times:
 return i * j;case Cyc_Absyn_Bitand:
 return i & j;case Cyc_Absyn_Bitor:
 return i | j;case Cyc_Absyn_Bitxor:
 return i ^ j;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Error in evaluating binop",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}
# 442
void*Cyc_AssnDef_get_term_type(void*t){
void*_Tmp0;unsigned _Tmp1;void*_Tmp2;switch(*((int*)t)){case 0:
 return Cyc_Absyn_uint_type;case 1: _Tmp2=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp2;
return e->topt;}case 2: _Tmp2=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
return vd->type;}case 6: _Tmp2=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{void*topt=_Tmp2;
return topt;}case 8: _Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp2;
return tp;}case 7: _Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}case 9: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;{void*tp=_Tmp2;
return tp;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp2;
return tp;}case 10: _Tmp2=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp2;
return tp;}case 11: _Tmp2=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp2;unsigned i=_Tmp1;void*tp=_Tmp0;
return tp;}case 3: _Tmp2=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 455
return Cyc_Absyn_uint_type;}case 5:
 return 0;case 12:
 return 0;default:
 return Cyc_Absyn_uint_type;};}
# 462
void*Cyc_AssnDef_plus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(0U,t1,t2,tp);}
# 465
void*Cyc_AssnDef_minus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(2U,t1,t2,tp);}
# 469
void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop p,void*t,void*tp){
# 471
if((int)p==0)return t;
# 473
if((int)p==2)return({void*_Tmp0=t;void*_Tmp1=Cyc_AssnDef_uint(-1);Cyc_AssnDef_binop(1U,_Tmp0,_Tmp1,tp);});{
struct Cyc_AssnDef_Unop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Unop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=7,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& ptr);}}
# 478
void*Cyc_AssnDef_numelts_term(void*t){
return Cyc_AssnDef_unop(18U,t,Cyc_Absyn_uint_type);}
# 482
static int Cyc_AssnDef_inv_cmp_term(void*t1,void*t2){
return - Cyc_AssnDef_cmp_term(t1,t2);}
# 487
inline static void*Cyc_AssnDef_compute_factor(int c,void*t,void*topt){
if(c==1)return t;else{
# 490
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newfactor=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=8,_Tmp0.f1=1U,_Tmp0.f2=t,({void*_Tmp1=Cyc_AssnDef_uint((unsigned)c);_Tmp0.f3=_Tmp1;}),_Tmp0.f4=topt;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& newfactor);}}struct _tuple25{int f0;void*f1;};
# 494
static void*Cyc_AssnDef_termcoeflist2term(struct Cyc_List_List*q_list,int newc,void*topt){
if(q_list==0)return Cyc_AssnDef_uint((unsigned)newc);{
# 497
void*res=Cyc_AssnDef_compute_factor(((struct _tuple25*)q_list->hd)->f0,((struct _tuple25*)q_list->hd)->f1,topt);
q_list=q_list->tl;
while(q_list!=0){
void*newf=Cyc_AssnDef_compute_factor(((struct _tuple25*)q_list->hd)->f0,((struct _tuple25*)q_list->hd)->f1,topt);
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=8,_Tmp0.f1=0U,_Tmp0.f2=res,_Tmp0.f3=newf,_Tmp0.f4=topt;_Tmp0;});
res=Cyc_AssnDef_hash_cons_term((void*)& newterm);
q_list=q_list->tl;}
# 505
if(newc==0)return res;else{
# 507
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=8,_Tmp0.f1=0U,_Tmp0.f2=res,({void*_Tmp1=Cyc_AssnDef_uint((unsigned)newc);_Tmp0.f3=_Tmp1;}),_Tmp0.f4=topt;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& newterm);}}}
# 512
struct _tuple12 Cyc_AssnDef_flatten_plus(void*t){
struct Cyc_List_List*term_coef_list=0;
int res=0;
LOOP: {
void*_Tmp0;void*_Tmp1;unsigned _Tmp2;switch(*((int*)t)){case 0: _Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp2;
res=(int)i;goto _LL0;}case 8: if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t1=_Tmp1;unsigned i=_Tmp2;
# 519
res=(int)i;
t=t1;
goto LOOP;}}else{_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp1;void*t2=_Tmp0;
# 523
{unsigned _Tmp3;void*_Tmp4;if(*((int*)t2)==8)switch((int)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f1){case Cyc_Absyn_Times: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3)==0){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t2)->f3)->f1;{void*t21=_Tmp4;unsigned c2=_Tmp3;
# 525
term_coef_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp6=({struct _tuple25*_Tmp7=_cycalloc(sizeof(struct _tuple25));_Tmp7->f0=(int)c2,_Tmp7->f1=t21;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=term_coef_list;_Tmp5;});
goto _LL9;}}else{goto _LLE;}case Cyc_Absyn_Plus:
# 528
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Plus in the right branch, not a normal form",sizeof(char),44U),_tag_fat(0U,sizeof(void*),0));
goto _LL9;default: goto _LLE;}else{_LLE:
# 531
 term_coef_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp6=({struct _tuple25*_Tmp7=_cycalloc(sizeof(struct _tuple25));_Tmp7->f0=1,_Tmp7->f1=t2;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=term_coef_list;_Tmp5;});
goto _LL9;}_LL9:;}
# 534
t=t1;
goto LOOP;}}}else{goto _LL7;}default: _LL7:
# 537
{unsigned _Tmp3;void*_Tmp4;if(*((int*)t)==8){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Times){if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;{void*t21=_Tmp4;unsigned c2=_Tmp3;
# 539
term_coef_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp6=({struct _tuple25*_Tmp7=_cycalloc(sizeof(struct _tuple25));_Tmp7->f0=(int)c2,_Tmp7->f1=t21;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=term_coef_list;_Tmp5;});
goto _LL10;}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13:
# 542
 term_coef_list=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp6=({struct _tuple25*_Tmp7=_cycalloc(sizeof(struct _tuple25));_Tmp7->f0=1,_Tmp7->f1=t;_Tmp7;});_Tmp5->hd=_Tmp6;}),_Tmp5->tl=term_coef_list;_Tmp5;});
goto _LL10;}_LL10:;}
# 545
goto _LL0;}_LL0:;}
# 547
return({struct _tuple12 _Tmp0;_Tmp0.f0=term_coef_list,_Tmp0.f1=res;_Tmp0;});}
# 557 "assndef.cyc"
static struct Cyc_List_List*Cyc_AssnDef_imp_merge_with_coef(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
struct Cyc_List_List*c=0;struct Cyc_List_List*d=0;
if(a==0)return b;
if(b==0)return a;{
int cmp_result=less_eq(((struct _tuple25*)a->hd)->f1,((struct _tuple25*)b->hd)->f1);
while(a!=0 && b!=0){
int cmp_result=less_eq(((struct _tuple25*)a->hd)->f1,((struct _tuple25*)b->hd)->f1);
if(cmp_result < 0){
if(c==0){
c=a;
a=a->tl;
d=c;}else{
# 570
c->tl=a;
c=a;
a=a->tl;}}else{
# 574
if(cmp_result > 0){
if(c==0){
c=b;
b=b->tl;
d=c;}else{
# 580
c->tl=b;
c=b;
b=b->tl;}}else{
# 586
int i=((struct _tuple25*)a->hd)->f0 + ((struct _tuple25*)b->hd)->f0;
if(i!=0){
((struct _tuple25*)a->hd)->f0=i;
if(c==0){
c=a;
d=c;}else{
# 593
c->tl=a;
c=a;}}
# 597
a=a->tl;
b=b->tl;}}}
# 601
if(c==0){
c=a==0?b: a;
d=c;}else{
# 605
c->tl=a==0?b: a;}
# 607
return d;}}
# 613
static void*Cyc_AssnDef_flatten_merge_plus(void*t,void*s,void*topt){
struct _tuple12 _Tmp0=Cyc_AssnDef_flatten_plus(t);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*t_list=_Tmp2;int c=_Tmp1;
struct _tuple12 _Tmp3=Cyc_AssnDef_flatten_plus(s);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*s_list=_Tmp5;int d=_Tmp4;
struct Cyc_List_List*q_list=Cyc_AssnDef_imp_merge_with_coef(Cyc_AssnDef_inv_cmp_term,t_list,s_list);
int newc=c + d;
return Cyc_AssnDef_termcoeflist2term(q_list,newc,topt);}}}
# 627
static struct _tuple12 Cyc_AssnDef_flatten(enum Cyc_Absyn_Primop p,void*t){
struct Cyc_List_List*termlist=0;
int res=(int)Cyc_AssnDef_id_of_primop(p);
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;unsigned _Tmp3;switch(*((int*)t)){case 0: _Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp3;
res=(int)i;goto _LL0;}case 8: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;unsigned i=_Tmp3;
# 634
res=(int)i;
t=t1;
goto LOOP;}else{goto _LL5;}}else{_LL5: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;void*t2=_Tmp0;
# 638
termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t2,_Tmp4->tl=termlist;_Tmp4;});
t=t1;
goto LOOP;}else{goto _LL7;}}default: _LL7:
# 642
 termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t,_Tmp4->tl=termlist;_Tmp4;});
goto _LL0;}_LL0:;}
# 645
return({struct _tuple12 _Tmp0;_Tmp0.f0=termlist,_Tmp0.f1=res;_Tmp0;});}
# 648
static struct _tuple25*Cyc_AssnDef_times_newc(int c,struct _tuple25*td){
return({struct _tuple25*_Tmp0=_cycalloc(sizeof(struct _tuple25));_Tmp0->f0=td->f0 * c,_Tmp0->f1=td->f1;_Tmp0;});}
# 655
static void*Cyc_AssnDef_flatten_merge(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*topt){
struct _tuple12 _Tmp0=Cyc_AssnDef_flatten(p,t1);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*tlist1=_Tmp2;int c1=_Tmp1;
struct _tuple12 _Tmp3=Cyc_AssnDef_flatten(p,t2);int _Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*tlist2=_Tmp5;int c2=_Tmp4;
# 659
struct Cyc_List_List*termlist=Cyc_List_imp_merge(Cyc_AssnDef_inv_cmp_term,tlist1,tlist2);
unsigned id=Cyc_AssnDef_id_of_primop(p);
unsigned newc=Cyc_AssnDef_eval_binop(p,(unsigned)c1,(unsigned)c2);
if(termlist==0)return Cyc_AssnDef_uint(newc);{
void*t=(void*)termlist->hd;
termlist=termlist->tl;
while(termlist!=0){
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=8,_Tmp6.f1=p,_Tmp6.f2=t,_Tmp6.f3=(void*)termlist->hd,_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& newterm);
termlist=termlist->tl;}
# 670
if(id==newc)return t;else{
# 673
if((int)p==1){
if(*((int*)t)==8){if(((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1==Cyc_Absyn_Plus){
# 677
struct _tuple12 _Tmp6=Cyc_AssnDef_flatten_plus(t);int _Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{struct Cyc_List_List*t_list=_Tmp8;int c=_Tmp7;
struct Cyc_List_List*newt_list=({(struct Cyc_List_List*(*)(struct _tuple25*(*)(int,struct _tuple25*),int,struct Cyc_List_List*))Cyc_List_map_c;})(Cyc_AssnDef_times_newc,(int)newc,t_list);
return Cyc_AssnDef_termcoeflist2term(newt_list,(int)((unsigned)c * newc),topt);
goto _LL6;}}else{goto _LL9;}}else{_LL9:
 goto _LL6;}_LL6:;}{
# 684
struct Cyc_AssnDef_Binop_AssnDef_Term_struct newterm=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=8,_Tmp6.f1=p,_Tmp6.f2=t,({void*_Tmp7=Cyc_AssnDef_uint(newc);_Tmp6.f3=_Tmp7;}),_Tmp6.f4=topt;_Tmp6;});
return Cyc_AssnDef_hash_cons_term((void*)& newterm);}}}}}}struct _tuple26{enum Cyc_Absyn_Primop f0;void*f1;void*f2;};
# 689
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
if(Cyc_AssnDef_is_nonassociative_commutative(p)&& Cyc_AssnDef_cmp_term(t1,t2)< 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 693
{struct _tuple26 _Tmp0=({struct _tuple26 _Tmp1;_Tmp1.f0=p,_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;unsigned _Tmp3;unsigned _Tmp4;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned i=_Tmp4;unsigned j=_Tmp3;
# 696
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_AssnDef_uint(i + j);case Cyc_Absyn_Minus:
 return Cyc_AssnDef_uint(i - j);case Cyc_Absyn_Bitand:
 return Cyc_AssnDef_uint(i & j);case Cyc_Absyn_Bitor:
 return Cyc_AssnDef_uint(i | j);case Cyc_Absyn_Bitxor:
 return Cyc_AssnDef_uint(i ^ j);case Cyc_Absyn_Bitlshift:
 return Cyc_AssnDef_uint(i << j);case Cyc_Absyn_Bitlrshift:
 return Cyc_AssnDef_uint(i >> j);default:
 goto _LL9;}_LL9:;
# 706
goto _LL0;}}else{if(_Tmp0.f0==Cyc_Absyn_Minus)goto _LL5;else{goto _LL7;}}}else{if(_Tmp0.f0==Cyc_Absyn_Minus){if(*((int*)_Tmp0.f2)==0){_Tmp2=_Tmp0.f1;_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{void*t1=_Tmp2;unsigned i=_Tmp4;
# 708
return({void*_Tmp5=t1;void*_Tmp6=Cyc_AssnDef_uint(0U - i);Cyc_AssnDef_plus(_Tmp5,_Tmp6,tp);});}}else{_LL5: _Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp2;void*t2=_Tmp1;
# 711
if(Cyc_AssnDef_cmp_term(t1,t2)==0)return Cyc_AssnDef_uint(0U);
(int)p==0;
t2=({void*_Tmp5=t2;void*_Tmp6=Cyc_AssnDef_uint(-1);Cyc_AssnDef_binop(1U,_Tmp5,_Tmp6,tp);});
return Cyc_AssnDef_binop(0U,t1,t2,tp);}}}else{_LL7:
# 723
 if((int)p==0)return Cyc_AssnDef_flatten_merge_plus(t1,t2,tp);
if(Cyc_AssnDef_is_associative_commutative(p))return Cyc_AssnDef_flatten_merge(p,t1,t2,tp);
goto _LL0;}}_LL0:;}{
# 727
struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp1;_Tmp1.tag=8,_Tmp1.f1=p,_Tmp1.f2=t1,_Tmp1.f3=t2,_Tmp1.f4=tp;_Tmp1;});struct Cyc_AssnDef_Binop_AssnDef_Term_struct p=_Tmp0;
return Cyc_AssnDef_hash_cons_term((void*)& p);}}
# 731
void*Cyc_AssnDef_cast(void*tp,void*tm){
struct Cyc_AssnDef_Cast_AssnDef_Term_struct t=({struct Cyc_AssnDef_Cast_AssnDef_Term_struct _Tmp0;_Tmp0.tag=9,_Tmp0.f1=tp,_Tmp0.f2=tm;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 735
void*Cyc_AssnDef_fresh_var(void*t){
static int counter=0;
return Cyc_AssnDef_logicvar(0,counter ++,t);}
# 740
static int Cyc_AssnDef_same_term(void*t1,void*t2){return Cyc_AssnDef_cmp_term(t1,t2)==0;}
# 745
static unsigned Cyc_AssnDef_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 747
LOOP:
 if(Cyc_List_memq(*prev,a))return c;
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=a,_Tmp1->tl=*prev;_Tmp1;});*prev=_Tmp0;});{
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return c;case 1:
 return c;case 2:
 return 1U + c;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
_Tmp2=a1;_Tmp1=a2;goto _LLA;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;_LLA: {void*a1=_Tmp2;void*a2=_Tmp1;
# 756
c=Cyc_AssnDef_assn_size_loop(a2,1U + c,prev);
a=a1;
goto LOOP;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp2;
# 760
++ c;
a=a1;
goto LOOP;}default: _Tmp2=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;void*t=_Tmp1;void*a1=_Tmp0;
# 764
++ c;
a=a1;
goto LOOP;}};}}
# 771
static unsigned Cyc_AssnDef_max_assn_size=0U;
# 774
unsigned Cyc_AssnDef_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned c=Cyc_AssnDef_assn_size_loop(a,0U,& prev);
if(c > Cyc_AssnDef_max_assn_size)
Cyc_AssnDef_max_assn_size=c;
# 780
return c;}struct _tuple27{void*f0;struct _fat_ptr f1;struct _fat_ptr f2;};
# 783
static struct _fat_ptr Cyc_AssnDef_d2string(void*a,int*c,struct Cyc_List_List**prev){
# 785
{struct Cyc_List_List*ps=*prev;for(0;ps!=0;ps=ps->tl){
struct _tuple27*_Tmp0=(struct _tuple27*)ps->hd;struct _fat_ptr _Tmp1;struct _fat_ptr _Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f0;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{void*a1=_Tmp3;struct _fat_ptr n=_Tmp2;struct _fat_ptr v=_Tmp1;
if((unsigned)a1==(unsigned)a)return n;}}}{
# 789
struct _fat_ptr n=({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)*c;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("A%d",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});
*c=*c + 1;{
struct _fat_ptr s;
{void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 s=_tag_fat("true",sizeof(char),5U);goto _LL3;case 1:
 s=_tag_fat("false",sizeof(char),6U);goto _LL3;case 2:
 s=Cyc_AssnDef_assn2string(a);goto _LL3;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 797
struct _fat_ptr d1=Cyc_AssnDef_d2string(a1,c,prev);
struct _fat_ptr d2=Cyc_AssnDef_d2string(a2,c,prev);
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)d1);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)d2);_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL3;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 802
struct _fat_ptr d1=Cyc_AssnDef_d2string(a1,c,prev);
struct _fat_ptr d2=Cyc_AssnDef_d2string(a2,c,prev);
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)d1);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)d2);_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL3;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp2;
# 807
struct _fat_ptr d=Cyc_AssnDef_d2string(a,c,prev);
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)d);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(_tag_fat("not(%s)",sizeof(char),8U),_tag_fat(_Tmp4,sizeof(void*),1));});
goto _LL3;}default: _Tmp2=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;void*t=_Tmp1;void*a=_Tmp0;
# 811
struct _fat_ptr d=Cyc_AssnDef_d2string(a,c,prev);
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)d);_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_aprintf(_tag_fat("Subst[%s=%s](%s)",sizeof(char),17U),_tag_fat(_Tmp6,sizeof(void*),3));});
goto _LL3;}}_LL3:;}
# 816
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple27*_Tmp2=({struct _tuple27*_Tmp3=_cycalloc(sizeof(struct _tuple27));_Tmp3->f0=a,_Tmp3->f1=(struct _fat_ptr)n,_Tmp3->f2=s;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*prev;_Tmp1;});*prev=_Tmp0;});
return n;}}}
# 820
struct _fat_ptr Cyc_AssnDef_dag2string(void*a){
struct Cyc_List_List*decls=0;
struct _fat_ptr*newline;newline=_cycalloc(sizeof(struct _fat_ptr)),*newline=_tag_fat("\n",sizeof(char),2U);{
struct _fat_ptr*equals;equals=_cycalloc(sizeof(struct _fat_ptr)),*equals=_tag_fat(" = ",sizeof(char),4U);{
int count=0;
struct _fat_ptr res=Cyc_AssnDef_d2string(a,& count,& decls);
struct Cyc_List_List*strings=({struct _fat_ptr*_Tmp0[3];({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat(" in ",sizeof(char),5U);_Tmp2;});_Tmp0[0]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=res;_Tmp2;});_Tmp0[1]=_Tmp1;}),_Tmp0[2]=newline;Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _fat_ptr*),3));});
for(1;decls!=0;decls=decls->tl){
struct _tuple27*_Tmp0=(struct _tuple27*)decls->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(struct _fat_ptr*)& _Tmp0->f1;_Tmp1=(struct _fat_ptr*)& _Tmp0->f2;{struct _fat_ptr*n=(struct _fat_ptr*)_Tmp2;struct _fat_ptr*d=(struct _fat_ptr*)_Tmp1;
strings=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=n,({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=equals,({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=d,({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=newline,_Tmp9->tl=strings;_Tmp9;});_Tmp7->tl=_Tmp8;});_Tmp7;});_Tmp5->tl=_Tmp6;});_Tmp5;});_Tmp3->tl=_Tmp4;});_Tmp3;});}}
# 831
return Cyc_strconcat_l(strings);}}}
# 836
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec,int*size){
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 return _tag_fat("true",sizeof(char),5U);case 1:
 return _tag_fat("false",sizeof(char),6U);case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 841
*size +=1;{
struct _fat_ptr ps;
switch((int)p){case Cyc_AssnDef_Eq:
 ps=_tag_fat("==",sizeof(char),3U);goto _LLF;case Cyc_AssnDef_Neq:
 ps=_tag_fat("!=",sizeof(char),3U);goto _LLF;case Cyc_AssnDef_SLt:
 ps=_tag_fat(" S< ",sizeof(char),5U);goto _LLF;case Cyc_AssnDef_SLte:
 ps=_tag_fat(" S<= ",sizeof(char),6U);goto _LLF;case Cyc_AssnDef_ULt:
 ps=_tag_fat(" U< ",sizeof(char),5U);goto _LLF;case Cyc_AssnDef_ULte:
 ps=_tag_fat(" U<= ",sizeof(char),6U);goto _LLF;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("assn2string primop",sizeof(char),19U),_tag_fat(0U,sizeof(void*),0));}_LLF:;
# 852
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(_tag_fat("%s%s%s",sizeof(char),7U),_tag_fat(_Tmp7,sizeof(void*),3));});}}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 854
*size +=1;
if(prec <= 5)
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 858
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("(%s || %s)",sizeof(char),11U),_tag_fat(_Tmp6,sizeof(void*),2));});}}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 860
*size +=1;
if(prec <= 10)
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 864
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("(%s && %s)",sizeof(char),11U),_tag_fat(_Tmp6,sizeof(void*),2));});}}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
# 866
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,20,size));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("!%s",sizeof(char),4U),_tag_fat(_Tmp5,sizeof(void*),1));});}default: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a=_Tmp0;
# 869
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(_tag_fat("Subst[%s:=%s](%s)",sizeof(char),18U),_tag_fat(_Tmp7,sizeof(void*),3));});}};}
# 875
int Cyc_AssnDef_tree_size=0;
# 878
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
Cyc_AssnDef_tree_size=0;
return Cyc_AssnDef_a2string(a,0,& Cyc_AssnDef_tree_size);}
# 884
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 895 "assndef.cyc"
int Cyc_AssnDef_assnhash(void*a){
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 1:
 return 0;case 0:
 return 1;case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 900
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
return(int)((unsigned)a1 ^ (unsigned)a2 | 1U);}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
return(int)((unsigned)a1 + 61U);}default: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a=_Tmp0;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | 3U);}};}
# 911
int Cyc_AssnDef_assncmp(void*a1,void*a2){
LOOP:
 if(a1==a2)return 0;{
struct _tuple24 _Tmp0=({struct _tuple24 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_AssnDef_Primreln _Tmp3;enum Cyc_AssnDef_Primreln _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0)
return 0;else{
return -1;}}else{if(*((int*)_Tmp0.f1)==0)
return 1;else{if(*((int*)_Tmp0.f0)==1){if(*((int*)_Tmp0.f1)==1)
return 0;else{
return -1;}}else{if(*((int*)_Tmp0.f1)==1)
return 1;else{switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_Tmp8=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f2;{void*a11=_Tmp8;void*a12=_Tmp7;void*a21=_Tmp6;void*a22=_Tmp5;
_Tmp8=a11;_Tmp7=a12;_Tmp6=a21;_Tmp5=a22;goto _LL10;}}else{
# 928
return -1;}case 3: switch(*((int*)_Tmp0.f1)){case 3: _Tmp8=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_LL10: {void*a11=_Tmp8;void*a12=_Tmp7;void*a21=_Tmp6;void*a22=_Tmp5;
# 923
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c!=0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4: goto _LL13;default:
# 930
 return -1;}default: switch(*((int*)_Tmp0.f1)){case 4: _LL13:
# 929
 return 1;case 3:
# 931
 return 1;default: if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2){_Tmp8=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp4=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f3;_Tmp6=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f3;{void*t11=_Tmp8;enum Cyc_AssnDef_Primreln p1=_Tmp4;void*t12=_Tmp7;void*t21=_Tmp6;enum Cyc_AssnDef_Primreln p2=_Tmp3;void*t22=_Tmp5;
# 933
int c=(int)p1 - (int)p2;
if(c!=0)return c;
c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==2)
return 1;else{if(*((int*)_Tmp0.f0)==5){if(*((int*)_Tmp0.f1)==5){_Tmp8=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)_Tmp0.f1)->f1;{void*na1=_Tmp8;void*na2=_Tmp7;
a1=na1;a2=na2;goto LOOP;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==5)
return 1;else{_Tmp8=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f0)->f3;_Tmp5=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f3;{struct Cyc_Absyn_Vardecl*vd1=_Tmp8;void*t1=_Tmp7;void*a11=_Tmp6;struct Cyc_Absyn_Vardecl*vd2=_Tmp5;void*t2=_Tmp2;void*a22=_Tmp1;
# 944
int c=(int)vd1 - (int)vd2;
if(c!=0)return c;
c=Cyc_AssnDef_cmp_term(t1,t2);
if(c!=0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}};}}struct Cyc_AssnDef_AssnInfo{void*assn;void*reduced;struct Cyc_Set_Set**widened_set;};
# 964
static struct Cyc_Hashtable_Table*Cyc_AssnDef_hash_cons_table=0;
# 966
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_term_hash_cons_table=0;
Cyc_AssnDef_hash_cons_table=0;}
# 971
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_hash_cons_table (void){
# 973
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_hash_cons_table==0){
h=Cyc_Hashtable_create(123,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_hash_cons_table=h;}else{
# 978
h=_check_null(Cyc_AssnDef_hash_cons_table);}
# 980
return h;}
# 982
static struct Cyc_AssnDef_AssnInfo*Cyc_AssnDef_hash_cons_it(void*);
static void*Cyc_AssnDef_hash_cons(void*);
# 989
static struct Cyc_AssnDef_AssnInfo*Cyc_AssnDef_copy_assn(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:  {
# 992
struct Cyc_Set_Set*s=Cyc_Set_empty(Cyc_AssnDef_assncmp);
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=(void*)& Cyc_AssnDef_true_assn,_Tmp4->reduced=(void*)& Cyc_AssnDef_true_assn,({struct Cyc_Set_Set**_Tmp5=({struct Cyc_Set_Set**_Tmp6=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp6=s;_Tmp6;});_Tmp4->widened_set=_Tmp5;});_Tmp4;});}case 1:
# 995
 return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=(void*)& Cyc_AssnDef_false_assn,_Tmp4->reduced=(void*)& Cyc_AssnDef_false_assn,({struct Cyc_Set_Set**_Tmp5=({struct Cyc_Set_Set**_Tmp6=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp6=0;_Tmp6;});_Tmp4->widened_set=_Tmp5;});_Tmp4;});case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp2;
# 997
struct Cyc_AssnDef_AssnInfo*a1info=Cyc_AssnDef_hash_cons_it(a1);
struct Cyc_AssnDef_AssnInfo*a2info=Cyc_AssnDef_hash_cons_it(a2);
int reduced=1;
if(a1info->reduced==0){
reduced=0;
a1=a1info->assn;}else{
# 1004
a1=_check_null(a1info->reduced);}
# 1006
if(a2info->reduced==0){
reduced=0;
a2=a2info->assn;}else{
# 1010
a2=_check_null(a2info->reduced);}{
# 1012
void*newa=(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp4->tag=3,_Tmp4->f1=a1,_Tmp4->f2=a2;_Tmp4;});
void*reda=reduced?newa: 0;
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=newa,_Tmp4->reduced=reda,_Tmp4->widened_set=0;_Tmp4;});}}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp2;
# 1016
struct Cyc_AssnDef_AssnInfo*a1info=Cyc_AssnDef_hash_cons_it(a1);
struct Cyc_AssnDef_AssnInfo*a2info=Cyc_AssnDef_hash_cons_it(a2);
int reduced=1;
if(a1info->reduced==0){
reduced=0;
a1=a1info->assn;}else{
# 1023
a1=_check_null(a1info->reduced);}
# 1025
if(a2info->reduced==0){
reduced=0;
a2=a2info->assn;}else{
# 1029
a2=_check_null(a2info->reduced);}{
# 1031
void*newa=(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp4->tag=4,_Tmp4->f1=a1,_Tmp4->f2=a2;_Tmp4;});
void*reda=reduced?newa: 0;
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=newa,_Tmp4->reduced=reda,_Tmp4->widened_set=0;_Tmp4;});}}case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp2;
# 1035
void*newa=(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp4->tag=2,_Tmp4->f1=t1,_Tmp4->f2=p,_Tmp4->f3=t2;_Tmp4;});
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=newa,_Tmp4->reduced=newa,_Tmp4->widened_set=0;_Tmp4;});}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));({void*_Tmp5=(void*)({struct Cyc_AssnDef_Not_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Not_AssnDef_Assn_struct));_Tmp6->tag=5,({void*_Tmp7=Cyc_AssnDef_hash_cons(a);_Tmp6->f1=_Tmp7;});_Tmp6;});_Tmp4->assn=_Tmp5;}),_Tmp4->reduced=0,_Tmp4->widened_set=0;_Tmp4;});}default: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd1=_Tmp3;void*t1=_Tmp2;void*a1=_Tmp0;
# 1039
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));({void*_Tmp5=(void*)({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Subst_AssnDef_Assn_struct));_Tmp6->tag=6,_Tmp6->f1=vd1,_Tmp6->f2=t1,({void*_Tmp7=Cyc_AssnDef_hash_cons(a1);_Tmp6->f3=_Tmp7;});_Tmp6;});_Tmp4->assn=_Tmp5;}),_Tmp4->reduced=0,_Tmp4->widened_set=0;_Tmp4;});}};}
# 1061 "assndef.cyc"
static struct Cyc_AssnDef_AssnInfo*Cyc_AssnDef_hash_cons_it(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_hash_cons_table();
struct Cyc_AssnDef_AssnInfo**resopt=({(struct Cyc_AssnDef_AssnInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
if(resopt==0){
struct Cyc_AssnDef_AssnInfo*ainfo=Cyc_AssnDef_copy_assn(a);
({(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnInfo*))Cyc_Hashtable_insert;})(h,ainfo->assn,ainfo);
return ainfo;}else{
# 1069
return*resopt;}}
# 1073
static void*Cyc_AssnDef_hash_cons(void*a){
struct Cyc_AssnDef_AssnInfo*ainfo=Cyc_AssnDef_hash_cons_it(a);
void*redopt=ainfo->reduced;
if(redopt==0)return ainfo->assn;else{
return redopt;}}static char _TmpG3[5U]="#Mem";
# 1082
static struct _fat_ptr Cyc_AssnDef_memory_var={_TmpG3,_TmpG3,_TmpG3 + 5U};
static struct _tuple0 Cyc_AssnDef_memvar={.f0={.Loc_n={4,0}},.f1=& Cyc_AssnDef_memory_var};
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_AssnDef_void_con={0};
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_AssnDef_vd_type={0,(void*)& Cyc_AssnDef_void_con,0};
# 1088
struct Cyc_Absyn_Vardecl Cyc_AssnDef_memory_struct={Cyc_Absyn_Public,& Cyc_AssnDef_memvar,0U,{0,0,0,0,0U},(void*)& Cyc_AssnDef_vd_type,0,0,0,0,0};
# 1100
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory=& Cyc_AssnDef_memory_struct;
# 1104
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple24 _Tmp0=({struct _tuple24 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a2;else{if(*((int*)_Tmp0.f1)==0)
return a1;else{if(*((int*)_Tmp0.f0)==1)
return a1;else{if(*((int*)_Tmp0.f1)==1)
return a2;else{
# 1114
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=3,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons((void*)& a);}}}};}}
# 1120
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple24 _Tmp0=({struct _tuple24 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a1;else{if(*((int*)_Tmp0.f1)==0)
return a2;else{if(*((int*)_Tmp0.f0)==1)
return a2;else{if(*((int*)_Tmp0.f1)==1)
return a1;else{
# 1130
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons((void*)& a);}}}};}}
# 1136
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 6: _Tmp2=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;if(x==(struct Cyc_Absyn_Vardecl*)_Tmp2){struct Cyc_Absyn_Vardecl*y=_Tmp2;void*newy=_Tmp1;void*a2=_Tmp0;
# 1142
return({struct Cyc_Absyn_Vardecl*_Tmp3=x;void*_Tmp4=Cyc_AssnDef_subst_term(newx,x,newy);Cyc_AssnDef_subst(_Tmp3,_Tmp4,a2);});}else{goto _LL7;}default: _LL7: {
# 1144
struct Cyc_AssnDef_Subst_AssnDef_Assn_struct p=({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _Tmp3;_Tmp3.tag=6,_Tmp3.f1=x,_Tmp3.f2=newx,_Tmp3.f3=a;_Tmp3;});
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 1150
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=t1,_Tmp0.f2=p,_Tmp0.f3=t2;_Tmp0;});
return Cyc_AssnDef_hash_cons((void*)& ptr);}
# 1156
void*Cyc_AssnDef_eq(void*t1opt,void*t2opt){
if(t1opt==0 || t2opt==0)return(void*)& Cyc_AssnDef_true_assn;{
void*t1=t1opt;
void*t2=t2opt;
if(Cyc_AssnDef_cmp_term(t1,t2)> 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 1163
return Cyc_AssnDef_prim(t1,0U,t2);}}
# 1167
void*Cyc_AssnDef_neq(void*t1opt,void*t2opt){
if(t1opt==0 || t2opt==0)return(void*)& Cyc_AssnDef_true_assn;{
void*t1=t1opt;
void*t2=t2opt;
if(Cyc_AssnDef_cmp_term(t1,t2)> 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 1174
return Cyc_AssnDef_prim(t1,1U,t2);}}
# 1183
void*Cyc_AssnDef_slt(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,2U,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,3U,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,4U,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,5U,t2);}
# 1192
static void*Cyc_AssnDef_not_prim(void*t1,enum Cyc_Absyn_Primop p,void*t2){
switch((int)p){case Cyc_AssnDef_Eq:
 return Cyc_AssnDef_neq(t1,t2);case Cyc_AssnDef_Neq:
 return Cyc_AssnDef_eq(t1,t2);case Cyc_AssnDef_SLt:
 return Cyc_AssnDef_slte(t2,t1);case Cyc_AssnDef_SLte:
 return Cyc_AssnDef_slt(t2,t1);case Cyc_AssnDef_ULt:
 return Cyc_AssnDef_ulte(t2,t1);case Cyc_AssnDef_ULte:
 return Cyc_AssnDef_ult(t2,t1);default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Vcgen::not_prim: non-primop",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));};}
# 1204
void*Cyc_AssnDef_not(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
return Cyc_AssnDef_not_prim(t1,(enum Cyc_Absyn_Primop)p,t2);}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp2;
return a;}default:  {
# 1211
struct Cyc_AssnDef_Not_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Not_AssnDef_Assn_struct _Tmp3;_Tmp3.tag=5,_Tmp3.f1=a;_Tmp3;});
return Cyc_AssnDef_hash_cons((void*)& ptr);}};}struct _tuple28{struct Cyc_Absyn_Vardecl*f0;void*f1;};
# 1230 "assndef.cyc"
static void*Cyc_AssnDef_reduce_term(void*t,struct Cyc_List_List*subst){
# 1232
int _Tmp0;unsigned _Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;void*_Tmp4;void*_Tmp5;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 6: _LL6:
 return t;case 9: _Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp5;void*tm=_Tmp4;
# 1237
void*tm2=Cyc_AssnDef_reduce_term(tm,subst);
if(tm!=tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 7: _Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp3;void*tm=_Tmp5;void*tp=_Tmp4;
# 1241
void*tm2=Cyc_AssnDef_reduce_term(tm,subst);
if(tm!=tm2)return Cyc_AssnDef_unop(p,tm2,tp);else{
return t;}}case 8: _Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp3;void*t1=_Tmp5;void*s1=_Tmp4;void*tp=_Tmp2;
# 1245
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
void*s2=Cyc_AssnDef_reduce_term(s1,subst);
if(t1!=t2 || s1!=s2)return Cyc_AssnDef_binop(p,t2,s2,tp);else{
return t;}}case 4: _Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;void*s1=_Tmp4;void*tp=_Tmp2;
# 1250
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
void*s2=Cyc_AssnDef_reduce_term(s1,subst);
if(t1!=t2 || s1!=s2)return Cyc_AssnDef_select(t2,s2,tp);else{
return t;}}case 5: _Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;void*s1=_Tmp4;void*u1=_Tmp2;
# 1255
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
void*s2=Cyc_AssnDef_reduce_term(s1,subst);
void*u2=Cyc_AssnDef_reduce_term(u1,subst);
if((t1!=t2 || s1!=s2)|| u1!=u2)return Cyc_AssnDef_update(t2,s2,u2);else{
return t;}}case 11: _Tmp5=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Proj_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp5;unsigned i=_Tmp1;void*tp=_Tmp4;
# 1261
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
if(t1!=t2)return Cyc_AssnDef_proj(t1,i,tp);else{
return t;}}case 10: _Tmp0=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f2;_Tmp5=((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f3;_Tmp4=(void*)((struct Cyc_AssnDef_Aggr_AssnDef_Term_struct*)t)->f4;{int is_union=_Tmp0;unsigned tag=_Tmp1;struct Cyc_List_List*ts1=_Tmp5;void*tp=_Tmp4;
# 1265
struct Cyc_List_List*ts2=0;
int change=0;
for(1;ts1!=0;ts1=ts1->tl){
void*t1=(void*)ts1->hd;
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
ts2=({struct Cyc_List_List*_Tmp6=_cycalloc(sizeof(struct Cyc_List_List));_Tmp6->hd=t2,_Tmp6->tl=ts2;_Tmp6;});
if(t1!=t2)change=1;}
# 1273
if(change)
return({int _Tmp6=is_union;unsigned _Tmp7=tag;struct Cyc_List_List*_Tmp8=Cyc_List_imp_rev(ts2);Cyc_AssnDef_aggr(_Tmp6,_Tmp7,_Tmp8,tp);});else{
return t;}}case 3: _Tmp5=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp5;
_Tmp5=vd;goto _LL18;}case 2: _Tmp5=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;_LL18: {struct Cyc_Absyn_Vardecl*vd=_Tmp5;
# 1278
for(1;subst!=0;subst=subst->tl){
struct _tuple28 _Tmp6=*((struct _tuple28*)subst->hd);void*_Tmp7;void*_Tmp8;_Tmp8=_Tmp6.f0;_Tmp7=_Tmp6.f1;{struct Cyc_Absyn_Vardecl*vd2=_Tmp8;void*t2=_Tmp7;
if(vd==vd2)return t2;}}
# 1282
return t;}case 12: _Tmp5=(void*)((struct Cyc_AssnDef_Okderef_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp5;
# 1284
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
if(t1!=t2)return Cyc_AssnDef_okderef(t2);else{
return t;}}default: _Tmp5=(void*)((struct Cyc_AssnDef_Tagof_AssnDef_Term_struct*)t)->f1;{void*t1=_Tmp5;
# 1288
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
if(t1!=t2)return Cyc_AssnDef_tagof(t2);else{
return t;}}};}
# 1297
static int Cyc_AssnDef_same_subst(struct Cyc_List_List*s1,struct Cyc_List_List*s2){
if(s1==s2)return 1;
for(1;s1!=0 && s2!=0;(s1=s1->tl,s2=s2->tl)){
struct _tuple28 _Tmp0=*((struct _tuple28*)s1->hd);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_Absyn_Vardecl*vd1=_Tmp2;void*t1=_Tmp1;
struct _tuple28 _Tmp3=*((struct _tuple28*)s2->hd);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_Absyn_Vardecl*vd2=_Tmp5;void*t2=_Tmp4;
if(vd1!=vd2 || t1!=t2)return 0;
if(s1->tl==s2->tl)return 1;}}}
# 1305
return s1!=s2;}struct _tuple29{struct Cyc_List_List*f0;int f1;void*f2;};
# 1312
static void*Cyc_AssnDef_reduce_assn(void*a,struct Cyc_List_List*subst,int negate,struct Cyc_Hashtable_Table*h){
# 1314
struct Cyc_List_List**info_opt=({(struct Cyc_List_List**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;})(h,a);
struct Cyc_List_List*info=0;
if(info_opt!=0){
info=*info_opt;{
struct Cyc_List_List*p=info;for(0;p!=0;p=p->tl){
struct _tuple29 _Tmp0=*((struct _tuple29*)p->hd);void*_Tmp1;int _Tmp2;void*_Tmp3;_Tmp3=_Tmp0.f0;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*subst2=_Tmp3;int negate2=_Tmp2;void*a2=_Tmp1;
if(negate==negate2 && Cyc_AssnDef_same_subst(subst,subst2))
return a2;}}}}{
# 1325
void*res;
{void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 if(negate)return(void*)& Cyc_AssnDef_false_assn;else{return(void*)& Cyc_AssnDef_true_assn;}case 1:
 if(negate)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 1330
void*newt1=Cyc_AssnDef_reduce_term(t1,subst);
void*newt2=Cyc_AssnDef_reduce_term(t2,subst);
if(negate){res=Cyc_AssnDef_not_prim(newt1,(enum Cyc_Absyn_Primop)p,newt2);goto _LL3;}
if(t1!=newt1 || t2!=newt2)
a=Cyc_AssnDef_prim(newt1,p,newt2);
res=a;
goto _LL3;}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 1338
void*newa1=Cyc_AssnDef_reduce_assn(a1,subst,negate,h);
void*newa2=Cyc_AssnDef_reduce_assn(a2,subst,negate,h);
if(negate){res=Cyc_AssnDef_or(newa1,newa2);goto _LL3;}
if(a1!=newa1 || a2!=newa2)
a=Cyc_AssnDef_and(newa1,newa2);
res=a;
goto _LL3;}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 1346
void*newa1=Cyc_AssnDef_reduce_assn(a1,subst,negate,h);
void*newa2=Cyc_AssnDef_reduce_assn(a2,subst,negate,h);
if(negate){res=Cyc_AssnDef_and(newa1,newa2);goto _LL3;}
if(a1!=newa1 || a2!=newa2)
a=Cyc_AssnDef_or(newa1,newa2);
res=a;
goto _LL3;}case 6: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a1=_Tmp0;
# 1354
t=Cyc_AssnDef_reduce_term(t,subst);{
# 1356
struct Cyc_List_List*new_subst;new_subst=_cycalloc(sizeof(struct Cyc_List_List)),({struct _tuple28*_Tmp4=({struct _tuple28*_Tmp5=_cycalloc(sizeof(struct _tuple28));_Tmp5->f0=vd,_Tmp5->f1=t;_Tmp5;});new_subst->hd=_Tmp4;}),new_subst->tl=subst;
res=Cyc_AssnDef_reduce_assn(a1,new_subst,negate,h);
goto _LL3;}}default: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
# 1360
res=Cyc_AssnDef_reduce_assn(a1,subst,!negate,h);
goto _LL3;}}_LL3:;}
# 1363
({struct Cyc_Hashtable_Table*_Tmp0=h;void*_Tmp1=a;({(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_List_List*))Cyc_Hashtable_insert;})(_Tmp0,_Tmp1,({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple29*_Tmp3=({struct _tuple29*_Tmp4=_cycalloc(sizeof(struct _tuple29));_Tmp4->f0=subst,_Tmp4->f1=negate,_Tmp4->f2=res;_Tmp4;});_Tmp2->hd=_Tmp3;}),_Tmp2->tl=info;_Tmp2;}));});
return res;}}
# 1368
void*Cyc_AssnDef_reduce(void*a){
# 1370
struct Cyc_AssnDef_AssnInfo*ainfo=Cyc_AssnDef_hash_cons_it(a);
void*redopt=ainfo->reduced;
if(redopt!=0)return redopt;
a=({void*_Tmp0=a;Cyc_AssnDef_reduce_assn(_Tmp0,0,0,Cyc_Hashtable_create(33,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash));});
ainfo->reduced=a;
return a;}
# 1379
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
struct _tuple28 s=({struct _tuple28 _Tmp0;_Tmp0.f0=x,_Tmp0.f1=newx;_Tmp0;});
struct Cyc_List_List subst=({struct Cyc_List_List _Tmp0;_Tmp0.hd=& s,_Tmp0.tl=0;_Tmp0;});
return Cyc_AssnDef_reduce_term(t,& subst);}struct _tuple30{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 1394 "assndef.cyc"
static void Cyc_AssnDef_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 1398
while(1){
struct _tuple30 _Tmp0=({struct _tuple30 _Tmp1;_Tmp1.f0=a1s,_Tmp1.f1=a2s;_Tmp1;});if(_Tmp0.f0==0){if(_Tmp0.f1==0)
return;else{
# 1402
({struct Cyc_List_List*_Tmp1=Cyc_List_revappend(a2s,*res2);*res2=_Tmp1;});return;}}else{if(_Tmp0.f1==0){
# 1401
({struct Cyc_List_List*_Tmp1=Cyc_List_revappend(a1s,*res1);*res1=_Tmp1;});return;}else{
# 1404
int c=({void*_Tmp1=(void*)_check_null(a1s)->hd;Cyc_AssnDef_assncmp(_Tmp1,(void*)_check_null(a2s)->hd);});
if(c==0){
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a1s->hd,_Tmp2->tl=*common;_Tmp2;});*common=_Tmp1;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a1s->hd,_Tmp2->tl=*res1;_Tmp2;});*res1=_Tmp1;});
a1s=a1s->tl;}else{
# 1413
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a2s->hd,_Tmp2->tl=*res2;_Tmp2;});*res2=_Tmp1;});
a2s=a2s->tl;}}}};}}struct _tuple31{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1420
struct _tuple31 Cyc_AssnDef_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 1422
struct Cyc_List_List*common=0;
struct Cyc_List_List*res1=0;
struct Cyc_List_List*res2=0;
Cyc_AssnDef_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple31 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(common);_Tmp0.f0=_Tmp1;}),({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res1);_Tmp0.f1=_Tmp1;}),({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res2);_Tmp0.f2=_Tmp1;});_Tmp0;});}struct _tuple32{struct Cyc_List_List*f0;void*f1;};
# 1431
struct _tuple32 Cyc_AssnDef_factor(void*a){
void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 return({struct _tuple32 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=(void*)& Cyc_AssnDef_true_assn;_Tmp2;});case 1:
 return({struct _tuple32 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=(void*)& Cyc_AssnDef_false_assn;_Tmp2;});case 2:
 return({struct _tuple32 _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=a,_Tmp4->tl=0;_Tmp4;});_Tmp2.f0=_Tmp3;}),_Tmp2.f1=(void*)& Cyc_AssnDef_true_assn;_Tmp2;});case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1437
struct _tuple32 _Tmp2=Cyc_AssnDef_factor(a1);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_List_List*c1s=_Tmp4;void*a1=_Tmp3;
struct _tuple32 _Tmp5=Cyc_AssnDef_factor(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*c2s=_Tmp7;void*a2=_Tmp6;
return({struct _tuple32 _Tmp8;({struct Cyc_List_List*_Tmp9=Cyc_List_merge(Cyc_AssnDef_assncmp,c1s,c2s);_Tmp8.f0=_Tmp9;}),({void*_Tmp9=Cyc_AssnDef_and(a1,a2);_Tmp8.f1=_Tmp9;});_Tmp8;});}}}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1441
struct _tuple32 _Tmp2=Cyc_AssnDef_factor(a1);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_List_List*c1s=_Tmp4;void*a1=_Tmp3;
struct _tuple32 _Tmp5=Cyc_AssnDef_factor(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*c2s=_Tmp7;void*a2=_Tmp6;
struct _tuple31 _Tmp8=Cyc_AssnDef_intersect_assns(c1s,c2s);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_List_List*cs=_TmpB;struct Cyc_List_List*c1s=_TmpA;struct Cyc_List_List*c2s=_Tmp9;
return({struct _tuple32 _TmpC;_TmpC.f0=cs,({void*_TmpD=({void*_TmpE=Cyc_List_fold_left(Cyc_AssnDef_and,a1,c1s);Cyc_AssnDef_or(_TmpE,Cyc_List_fold_left(Cyc_AssnDef_and,a2,c2s));});_TmpC.f1=_TmpD;});_TmpC;});}}}}default:
 return({struct _tuple32 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=a;_Tmp2;});};}
# 1449
void*Cyc_AssnDef_factor_assn(void*a){
# 1451
struct _tuple32 _Tmp0=Cyc_AssnDef_factor(a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*cs=_Tmp2;void*a=_Tmp1;
a=Cyc_List_fold_left(Cyc_AssnDef_and,a,cs);
# 1456
return a;}}
# 1461
struct Cyc_Set_Set*Cyc_AssnDef_widen_it(void*a){
struct Cyc_AssnDef_AssnInfo*ainfo=Cyc_AssnDef_hash_cons_it(a);
if(ainfo->widened_set!=0)return*ainfo->widened_set;{
struct Cyc_Set_Set*s;
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 s=Cyc_Set_empty(Cyc_AssnDef_assncmp);goto _LL0;case 1:
 s=0;goto _LL0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1469
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0 || s2==0)
s=0;else{
# 1474
s=Cyc_Set_union_two(s1,s2);}
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1477
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0)
s=s2;else{
if(s2==0)
s=s1;else{
# 1484
s=Cyc_Set_intersect(s1,s2);}}
goto _LL0;}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1487
s=Cyc_Set_singleton(Cyc_AssnDef_assncmp,a);
switch((int)p){case Cyc_AssnDef_Eq:  {
# 1493
void*topt1=Cyc_AssnDef_get_term_type(t1);
void*topt2=Cyc_AssnDef_get_term_type(t2);
if(topt1!=0 && Cyc_Tcutil_is_integral_type(topt1)||
 topt2!=0 && Cyc_Tcutil_is_integral_type(topt2)){
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t2,t1));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t2,t1));});}
# 1502
goto _LLD;}case Cyc_AssnDef_SLt:
# 1504
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
goto _LLD;case Cyc_AssnDef_ULt:
# 1507
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
goto _LLD;default:
 goto _LLD;}_LLD:;
# 1511
goto _LL0;}default:
# 1513
 s=Cyc_AssnDef_widen_it(Cyc_AssnDef_reduce(a));
goto _LL0;}_LL0:;}
# 1516
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=s;_Tmp1;});ainfo->widened_set=_Tmp0;});
return s;}}
# 1521
void*Cyc_AssnDef_widen(void*a){
# 1523
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(a);
if(sopt==0)return(void*)& Cyc_AssnDef_false_assn;{
void*res=Cyc_Set_fold(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);
return res;}}
# 1531
static int Cyc_AssnDef_simple_prv(struct Cyc_Set_Set*ctxt,void*a){
while(1){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1535
if(Cyc_Set_member(ctxt,a))return 1;
switch((int)p){case Cyc_AssnDef_Neq:
# 1538
 return((({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t2,t1));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t2,t1));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));});case Cyc_AssnDef_SLte:
# 1543
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});case Cyc_AssnDef_ULte:
# 1546
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});default:
 return 0;};}case 0:
# 1550
 return 1;case 1:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1553
if(!Cyc_AssnDef_simple_prv(ctxt,a1))return 0;
a=a2;
continue;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1557
if(Cyc_AssnDef_simple_prv(ctxt,a1))return 1;
a=a2;
continue;}default:
# 1561
 a=Cyc_AssnDef_reduce(a);
continue;};}}
# 1570
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
if(ctxt==a)return 1;{
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(ctxt);
if(sopt==0)return 1;
return Cyc_AssnDef_simple_prv(sopt,a);}}
