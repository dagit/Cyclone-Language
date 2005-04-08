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
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f0;struct _fat_ptr*f1;};
# 135 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 156
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};
# 481 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};
# 902 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
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
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_Set_Set;
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
extern void*Cyc_Set_fold(void*(*)(void*,void*),struct Cyc_Set_Set*,void*);struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_Addr_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_Select_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_Update_AssnDef_Term_struct{int tag;void*f1;void*f2;void*f3;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Unop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;};struct Cyc_AssnDef_Binop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;void*f2;void*f3;void*f4;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 93 "assndef.h"
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop,void*,void*,void*);
# 105
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Not_AssnDef_Assn_struct{int tag;void*f1;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};
# 120
struct _fat_ptr Cyc_AssnDef_assn2string(void*);
# 124
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 158 "assndef.h"
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
# 47 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
struct _fat_ptr res;
{enum Cyc_Absyn_Primop _Tmp0;int _Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("0x%x",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
res=Cyc_Absynpp_exp2string(e);goto _LL0;}case 2: _Tmp4=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 53
if(Cyc_Flags_debug_assn)
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned)vd;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s_%x",sizeof(char),6U),_tag_fat(_Tmp8,sizeof(void*),2));});else{
# 56
res=Cyc_Absynpp_qvar2string(vd->name);}
goto _LL0;}case 3: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
# 59
if(Cyc_Flags_debug_assn)
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned)vd;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("&%s_%x",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});else{
# 62
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("&%s",sizeof(char),4U),_tag_fat(_Tmp7,sizeof(void*),1));});}
goto _LL0;}case 4: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp3;
# 65
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("Sel(%s,%s)",sizeof(char),11U),_tag_fat(_Tmp8,sizeof(void*),2));});
goto _LL0;}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp3;void*t3=_Tmp2;
# 68
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({struct _fat_ptr _TmpA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t3));_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("Upd(%s,%s,%s)",sizeof(char),14U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 6: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1==0){_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{int i=_Tmp1;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[1];_Tmp7[0]=& _Tmp6;Cyc_aprintf(_tag_fat("_X%d",sizeof(char),5U),_tag_fat(_Tmp7,sizeof(void*),1));});goto _LL0;}}else{_Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp1;
# 72
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(_check_null(vd)->name));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_Int_pa_PrintArg_struct _Tmp7=({struct Cyc_Int_pa_PrintArg_struct _Tmp8;_Tmp8.tag=1,_Tmp8.f1=(unsigned long)i;_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("_%s%d",sizeof(char),6U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}}case 8: _Tmp0=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;void*t2=_Tmp3;
# 74
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp8.f1=_Tmp9;});_Tmp8;});struct Cyc_String_pa_PrintArg_struct _Tmp8=({struct Cyc_String_pa_PrintArg_struct _Tmp9;_Tmp9.tag=0,({
struct _fat_ptr _TmpA=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp9.f1=_TmpA;});_Tmp9;});void*_Tmp9[3];_Tmp9[0]=& _Tmp6,_Tmp9[1]=& _Tmp7,_Tmp9[2]=& _Tmp8;Cyc_aprintf(_tag_fat("(%s%s%s)",sizeof(char),9U),_tag_fat(_Tmp9,sizeof(void*),3));});
goto _LL0;}case 7: _Tmp0=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp0;void*t1=_Tmp4;
# 78
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("%s(%s)",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}default: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp4;void*tm=_Tmp3;
# 80
res=({struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({struct _fat_ptr _Tmp9=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(tm));_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[2];_Tmp8[0]=& _Tmp6,_Tmp8[1]=& _Tmp7;Cyc_aprintf(_tag_fat("(%s)%s",sizeof(char),7U),_tag_fat(_Tmp8,sizeof(void*),2));});goto _LL0;}}_LL0:;}
# 82
return res;}
# 85
static int Cyc_AssnDef_hash_exp(struct Cyc_Absyn_Exp*e){
void*_Tmp0=e->r;switch(*((int*)_Tmp0)){case 0:
 return 0;case 1:
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
# 110
static int Cyc_AssnDef_termhash(void*t){
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;int _Tmp3;void*_Tmp4;unsigned _Tmp5;switch(*((int*)t)){case 0: _Tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp5;
return(int)i;}case 1: _Tmp4=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp4;
return Cyc_AssnDef_hash_exp(e)+ 61;}case 2: _Tmp4=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(int)((unsigned)vd);}case 6: _Tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;int i=_Tmp3;
return(int)((unsigned)vd ^ (unsigned)(i + 1));}case 8: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp4;void*t2=_Tmp1;
# 117
return(int)(((unsigned)p ^ (unsigned)t1)^ (unsigned)t2);}case 7: _Tmp2=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;{enum Cyc_Absyn_Primop p=_Tmp2;void*t1=_Tmp4;
return(int)((unsigned)p ^ (unsigned)t1 + 1U);}case 9: _Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*t=_Tmp4;
return Cyc_AssnDef_termhash(t);}case 4: _Tmp4=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;{void*t1=_Tmp4;void*t2=_Tmp1;
# 121
return(int)((unsigned)t1 ^ (unsigned)t2 + 2U);}case 5: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp1;void*t3=_Tmp0;
# 123
return(int)(((unsigned)t1 ^ (unsigned)t2)^ (unsigned)t3 + 3U);}default: _Tmp4=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp4;
return(int)((unsigned)vd + 4U);}};}struct _tuple23{void*f0;void*f1;};
# 128
int Cyc_AssnDef_cmp_term(void*t1,void*t2){
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_Absyn_Primop _Tmp3;enum Cyc_Absyn_Primop _Tmp4;void*_Tmp5;void*_Tmp6;int _Tmp7;int _Tmp8;void*_Tmp9;void*_TmpA;unsigned _TmpB;unsigned _TmpC;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0){_TmpC=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpB=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;{unsigned i1=_TmpC;unsigned i2=_TmpB;
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
# 143
if(vd1==vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==6)
return 1;else{if(*((int*)_Tmp0.f0)==9){if(*((int*)_Tmp0.f1)==9){_TmpA=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*tp1=_TmpA;void*tm1=_Tmp9;void*tp2=_Tmp6;void*tm2=_Tmp5;
# 148
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(tm1,tm2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==9)
return 1;else{if(*((int*)_Tmp0.f0)==7){if(*((int*)_Tmp0.f1)==7){_Tmp4=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)_Tmp0.f1)->f2;{enum Cyc_Absyn_Primop p1=_Tmp4;void*t1=_TmpA;enum Cyc_Absyn_Primop p2=_Tmp3;void*t2=_Tmp9;
# 154
if((int)p1!=(int)p2)return(int)p1 - (int)p2;
return Cyc_AssnDef_cmp_term(t1,t2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==7)
return 1;else{if(*((int*)_Tmp0.f0)==4){if(*((int*)_Tmp0.f1)==4){_TmpA=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)_Tmp0.f1)->f2;{void*t11=_TmpA;void*t12=_Tmp9;void*t21=_Tmp6;void*t22=_Tmp5;
# 159
int c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==4)
return 1;else{if(*((int*)_Tmp0.f0)==5){if(*((int*)_Tmp0.f1)==5){_TmpA=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp9=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f1)->f3;{void*t11=_TmpA;void*t12=_Tmp9;void*t13=_Tmp6;void*t21=_Tmp5;void*t22=_Tmp2;void*t23=_Tmp1;
# 165
int c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;{
int c=Cyc_AssnDef_cmp_term(t12,t22);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t13,t23);}}}else{
return -1;}}else{if(*((int*)_Tmp0.f1)==5)
return 1;else{_Tmp4=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f1;_TmpA=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp9=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp6=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f0)->f4;_Tmp3=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)_Tmp0.f1)->f4;{enum Cyc_Absyn_Primop p1=_Tmp4;void*t11=_TmpA;void*t12=_Tmp9;void*tp1=_Tmp6;enum Cyc_Absyn_Primop p2=_Tmp3;void*t21=_Tmp5;void*t22=_Tmp2;void*tp2=_Tmp1;
# 173
if((int)p1!=(int)p2)return(int)p1 - (int)p2;{
int c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}}}}}}}}}}}}}}}}}}};}
# 180
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
return(void*)({struct Cyc_AssnDef_Select_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Select_AssnDef_Term_struct));_Tmp6->tag=4,_Tmp6->f1=t1,_Tmp6->f2=t2,_Tmp6->f3=tp;_Tmp6;});}default: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp4;void*t2=_Tmp2;void*t3=_Tmp0;
return(void*)({struct Cyc_AssnDef_Update_AssnDef_Term_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Update_AssnDef_Term_struct));_Tmp6->tag=5,_Tmp6->f1=t1,_Tmp6->f2=t2,_Tmp6->f3=t3;_Tmp6;});}};}
# 196
static struct Cyc_Hashtable_Table*Cyc_AssnDef_term_hash_cons_table=0;
# 198
static void*Cyc_AssnDef_hash_cons_term(void*t){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_term_hash_cons_table==0){
h=Cyc_Hashtable_create(123,Cyc_AssnDef_cmp_term,Cyc_AssnDef_termhash);
Cyc_AssnDef_term_hash_cons_table=h;}else{
# 204
h=_check_null(Cyc_AssnDef_term_hash_cons_table);}{
# 206
void**resopt=Cyc_Hashtable_lookup_other_opt(h,t,Cyc_AssnDef_cmp_term,Cyc_AssnDef_termhash);
if(resopt==0){
void*t2=Cyc_AssnDef_copy_term(t);
Cyc_Hashtable_insert(h,t2,t2);
return t2;}else{
return*resopt;}}}
# 216
void*Cyc_AssnDef_uint(unsigned i){
struct Cyc_AssnDef_Uint_AssnDef_Term_struct t=({struct Cyc_AssnDef_Uint_AssnDef_Term_struct _Tmp0;_Tmp0.tag=0,_Tmp0.f1=i;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 220
void*Cyc_AssnDef_zero (void){
return Cyc_AssnDef_uint(0U);}
# 223
void*Cyc_AssnDef_one (void){
return Cyc_AssnDef_uint(1U);}
# 226
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){
struct _tuple11 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{unsigned c=_Tmp2;int known=_Tmp1;
if(known)return Cyc_AssnDef_uint(c);{
struct Cyc_AssnDef_Const_AssnDef_Term_struct t=({struct Cyc_AssnDef_Const_AssnDef_Term_struct _Tmp3;_Tmp3.tag=1,_Tmp3.f1=e;_Tmp3;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}}
# 232
void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_AssnDef_Var_AssnDef_Term_struct t=({struct Cyc_AssnDef_Var_AssnDef_Term_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=vd;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 236
void*Cyc_AssnDef_addr(struct Cyc_Absyn_Vardecl*vd){
struct Cyc_AssnDef_Addr_AssnDef_Term_struct t=({struct Cyc_AssnDef_Addr_AssnDef_Term_struct _Tmp0;_Tmp0.tag=3,_Tmp0.f1=vd;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 240
void*Cyc_AssnDef_logicvar(struct Cyc_Absyn_Vardecl*vd,int i,void*tp){
struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct t=({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct _Tmp0;_Tmp0.tag=6,_Tmp0.f1=vd,_Tmp0.f2=i,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 244
void*Cyc_AssnDef_update(void*t1,void*t2,void*t3){
{struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;void*_Tmp5;if(*((int*)_Tmp0.f0)==5){if(*((int*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)==3){if(*((int*)_Tmp0.f1)==3){_Tmp5=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp3=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*m=_Tmp5;void*a=_Tmp4;struct Cyc_Absyn_Vardecl*x=_Tmp3;void*e1=_Tmp2;struct Cyc_Absyn_Vardecl*y=_Tmp1;
# 248
if(x==y)return Cyc_AssnDef_update(m,t2,t3);
# 255
goto _LL0;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3:
 goto _LL0;}_LL0:;}{
# 258
struct Cyc_AssnDef_Update_AssnDef_Term_struct t=({struct Cyc_AssnDef_Update_AssnDef_Term_struct _Tmp0;_Tmp0.tag=5,_Tmp0.f1=t1,_Tmp0.f2=t2,_Tmp0.f3=t3;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}}
# 261
void*Cyc_AssnDef_select(void*t1,void*t2,void*tp){
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=t1,_Tmp1.f1=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;void*_Tmp3;void*_Tmp4;if(*((int*)_Tmp0.f0)==5){if(*((int*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)==3){if(*((int*)_Tmp0.f1)==3){_Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)_Tmp0.f1)->f1;{void*m=_Tmp4;struct Cyc_Absyn_Vardecl*x=_Tmp3;void*v=_Tmp2;struct Cyc_Absyn_Vardecl*y=_Tmp1;
# 265
if(x==y)return v;else{
# 268
return Cyc_AssnDef_select(m,t2,tp);}}}else{goto _LL3;}}else{_LL3: _Tmp4=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)_Tmp0.f0)->f3;_Tmp1=_Tmp0.f1;{void*m=_Tmp4;void*a1=_Tmp3;void*v=_Tmp2;void*a2=_Tmp1;
# 271
if(a1==a2)return v;
goto _LL6;}}}else{_LL6: {
# 274
struct Cyc_AssnDef_Select_AssnDef_Term_struct t=({struct Cyc_AssnDef_Select_AssnDef_Term_struct _Tmp5;_Tmp5.tag=4,_Tmp5.f1=t1,_Tmp5.f2=t2,_Tmp5.f3=tp;_Tmp5;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}};}
# 279
inline static int Cyc_AssnDef_is_nonassociative_commutative(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Eq:
 goto _LL4;case Cyc_Absyn_Neq: _LL4:
 return 1;default:
 return 0;};}
# 287
inline static int Cyc_AssnDef_is_associative_commutative(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
 goto _LL6;case Cyc_Absyn_Bitand: _LL6:
 goto _LL8;case Cyc_Absyn_Bitor: _LL8:
 goto _LLA;case Cyc_Absyn_Bitxor: _LLA:
 return 1;default:
 return 0;};}
# 298
void*Cyc_AssnDef_get_term_type(void*t){
void*_Tmp0;switch(*((int*)t)){case 0:
 return Cyc_Absyn_int_type(2U,2U);case 1: _Tmp0=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
return e->topt;}case 2: _Tmp0=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd->type;}case 6: _Tmp0=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{void*topt=_Tmp0;
return topt;}case 8: _Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{void*tp=_Tmp0;
return tp;}case 7: _Tmp0=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp0;
return tp;}case 9: _Tmp0=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;{void*tp=_Tmp0;
return tp;}case 4: _Tmp0=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*tp=_Tmp0;
return tp;}case 3: _Tmp0=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
# 309
return Cyc_Absyn_int_type(2U,2U);}default:
 return 0;};}
# 314
void*Cyc_AssnDef_plus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(0U,t1,t2,tp);}
# 317
void*Cyc_AssnDef_minus(void*t1,void*t2,void*tp){
return Cyc_AssnDef_binop(2U,t1,t2,tp);}
# 321
void*Cyc_AssnDef_unop(enum Cyc_Absyn_Primop p,void*t,void*tp){
# 323
if((int)p==0)return t;{
struct Cyc_AssnDef_Unop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Unop_AssnDef_Term_struct _Tmp0;_Tmp0.tag=7,_Tmp0.f1=p,_Tmp0.f2=t,_Tmp0.f3=tp;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& ptr);}}
# 330
static void*Cyc_AssnDef_eval_binop(enum Cyc_Absyn_Primop p,unsigned i,unsigned j){
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_AssnDef_uint(i + j);case Cyc_Absyn_Times:
 return Cyc_AssnDef_uint(i * j);case Cyc_Absyn_Bitand:
 return Cyc_AssnDef_uint(i & j);case Cyc_Absyn_Bitor:
 return Cyc_AssnDef_uint(i | j);case Cyc_Absyn_Bitxor:
 return Cyc_AssnDef_uint(i ^ j);default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Error in evaluating binop",sizeof(char),26U),_tag_fat(0U,sizeof(void*),0));};}struct _tuple24{struct Cyc_List_List*f0;unsigned*f1;};
# 343
static struct _tuple24 Cyc_AssnDef_flatten(enum Cyc_Absyn_Primop p,void*t){
struct Cyc_List_List*termlist=0;
unsigned*res=0;
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_Absyn_Primop _Tmp2;unsigned _Tmp3;switch(*((int*)t)){case 0: _Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp3;
res=({unsigned*_Tmp4=_cycalloc_atomic(sizeof(unsigned));*_Tmp4=i;_Tmp4;});goto _LL0;}case 8: if(*((int*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)==0){_Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3)->f1;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;unsigned i=_Tmp3;
# 350
res=({unsigned*_Tmp4=_cycalloc_atomic(sizeof(unsigned));*_Tmp4=i;_Tmp4;});
t=t1;
goto LOOP;}else{goto _LL5;}}else{_LL5: _Tmp2=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;if((int)((enum Cyc_Absyn_Primop)_Tmp2)==(int)p){enum Cyc_Absyn_Primop p1=_Tmp2;void*t1=_Tmp1;void*t2=_Tmp0;
# 354
termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t2,_Tmp4->tl=termlist;_Tmp4;});
t=t1;
goto LOOP;}else{goto _LL7;}}default: _LL7:
# 358
 termlist=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=t,_Tmp4->tl=termlist;_Tmp4;});
goto _LL0;}_LL0:;}
# 361
return({struct _tuple24 _Tmp0;_Tmp0.f0=termlist,_Tmp0.f1=res;_Tmp0;});}
# 364
static int Cyc_AssnDef_inv_cmp_term(void*t1,void*t2){
return - Cyc_AssnDef_cmp_term(t1,t2);}
# 367
static void*Cyc_AssnDef_flatten_merge(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*topt){
struct _tuple24 _Tmp0=Cyc_AssnDef_flatten(p,t1);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*tlist1=_Tmp2;unsigned*ip1=_Tmp1;
struct _tuple24 _Tmp3=Cyc_AssnDef_flatten(p,t2);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f0;_Tmp4=_Tmp3.f1;{struct Cyc_List_List*tlist2=_Tmp5;unsigned*ip2=_Tmp4;
# 371
struct Cyc_List_List*termlist=Cyc_List_imp_merge(Cyc_AssnDef_inv_cmp_term,tlist1,tlist2);
if(termlist==0)
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat(" commutative associative operator has no non constant operators!",sizeof(char),65U),_tag_fat(0U,sizeof(void*),0));{
# 375
void*t=(void*)termlist->hd;
termlist=termlist->tl;
while(termlist!=0){
struct Cyc_AssnDef_Binop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=8,_Tmp6.f1=p,_Tmp6.f2=t,_Tmp6.f3=(void*)termlist->hd,_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& ptr);
termlist=termlist->tl;}
# 382
if(ip1==0){
if(ip2!=0){
if((int)p==0 &&*ip2==0U ||
(int)p==1 &&*ip2==1U)
return t;{
struct Cyc_AssnDef_Binop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=8,_Tmp6.f1=p,_Tmp6.f2=t,({void*_Tmp7=Cyc_AssnDef_uint(*ip2);_Tmp6.f3=_Tmp7;}),_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& ptr);}}}else{
# 391
if(ip2!=0){
void*binres=Cyc_AssnDef_eval_binop(p,*ip1,*ip2);
if((int)p==0 &&({void*_Tmp6=binres;Cyc_AssnDef_cmp_term(_Tmp6,Cyc_AssnDef_zero());})==0 ||
(int)p==1 &&({void*_Tmp6=binres;Cyc_AssnDef_cmp_term(_Tmp6,Cyc_AssnDef_one());})==0)
return t;{
struct Cyc_AssnDef_Binop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=8,_Tmp6.f1=p,_Tmp6.f2=t,_Tmp6.f3=binres,_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& ptr);}}else{
# 399
if((int)p==0 &&*ip1==0U ||
(int)p==1 &&*ip1==1U)
return t;{
struct Cyc_AssnDef_Binop_AssnDef_Term_struct ptr=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp6;_Tmp6.tag=8,_Tmp6.f1=p,_Tmp6.f2=t,({void*_Tmp7=Cyc_AssnDef_uint(*ip1);_Tmp6.f3=_Tmp7;}),_Tmp6.f4=topt;_Tmp6;});
t=Cyc_AssnDef_hash_cons_term((void*)& ptr);}}}
# 406
return t;}}}}struct _tuple25{enum Cyc_Absyn_Primop f0;void*f1;void*f2;};
# 409
void*Cyc_AssnDef_binop(enum Cyc_Absyn_Primop p,void*t1,void*t2,void*tp){
if(Cyc_AssnDef_is_nonassociative_commutative(p)&& Cyc_AssnDef_cmp_term(t1,t2)< 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 413
{struct _tuple25 _Tmp0=({struct _tuple25 _Tmp1;_Tmp1.f0=p,_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});void*_Tmp1;void*_Tmp2;unsigned _Tmp3;unsigned _Tmp4;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned i=_Tmp4;unsigned j=_Tmp3;
# 416
switch((int)p){case Cyc_Absyn_Plus:
 return Cyc_AssnDef_uint(i + j);case Cyc_Absyn_Minus:
 return Cyc_AssnDef_uint(i - j);case Cyc_Absyn_Bitand:
 return Cyc_AssnDef_uint(i & j);case Cyc_Absyn_Bitor:
 return Cyc_AssnDef_uint(i | j);case Cyc_Absyn_Bitxor:
 return Cyc_AssnDef_uint(i ^ j);case Cyc_Absyn_Bitlshift:
 return Cyc_AssnDef_uint(i << j);case Cyc_Absyn_Bitlrshift:
 return Cyc_AssnDef_uint(i >> j);default:
 goto _LL9;}_LL9:;
# 426
goto _LL0;}}else{if(_Tmp0.f0==Cyc_Absyn_Minus)goto _LL5;else{goto _LL7;}}}else{if(_Tmp0.f0==Cyc_Absyn_Minus){if(*((int*)_Tmp0.f2)==0){_Tmp2=_Tmp0.f1;_Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{void*t1=_Tmp2;unsigned i=_Tmp4;
# 428
return({void*_Tmp5=t1;void*_Tmp6=Cyc_AssnDef_uint(0U - i);Cyc_AssnDef_plus(_Tmp5,_Tmp6,tp);});}}else{_LL5: _Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*t1=_Tmp2;void*t2=_Tmp1;
# 431
if(Cyc_AssnDef_cmp_term(t1,t2)==0)return Cyc_AssnDef_uint(0U);
goto _LL0;}}}else{_LL7:
# 441
 if(Cyc_AssnDef_is_associative_commutative(p))return Cyc_AssnDef_flatten_merge(p,t1,t2,tp);
goto _LL0;}}_LL0:;}{
# 444
struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp0=({struct Cyc_AssnDef_Binop_AssnDef_Term_struct _Tmp1;_Tmp1.tag=8,_Tmp1.f1=p,_Tmp1.f2=t1,_Tmp1.f3=t2,_Tmp1.f4=tp;_Tmp1;});struct Cyc_AssnDef_Binop_AssnDef_Term_struct p=_Tmp0;
return Cyc_AssnDef_hash_cons_term((void*)& p);}}
# 448
void*Cyc_AssnDef_cast(void*tp,void*tm){
struct Cyc_AssnDef_Cast_AssnDef_Term_struct t=({struct Cyc_AssnDef_Cast_AssnDef_Term_struct _Tmp0;_Tmp0.tag=9,_Tmp0.f1=tp,_Tmp0.f2=tm;_Tmp0;});
return Cyc_AssnDef_hash_cons_term((void*)& t);}
# 452
void*Cyc_AssnDef_fresh_var(void*t){
static int counter=0;
return Cyc_AssnDef_logicvar(0,counter ++,t);}
# 457
static int Cyc_AssnDef_same_term(void*t1,void*t2){return Cyc_AssnDef_cmp_term(t1,t2)==0;}
# 472
static unsigned Cyc_AssnDef_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 474
LOOP:
 if(Cyc_List_memq(*prev,a))return c;
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=a,_Tmp1->tl=*prev;_Tmp1;});*prev=_Tmp0;});{
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return c;case 1:
 return c;case 2:
 return 1U + c;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
_Tmp2=a1;_Tmp1=a2;goto _LLA;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;_LLA: {void*a1=_Tmp2;void*a2=_Tmp1;
# 483
c=Cyc_AssnDef_assn_size_loop(a2,1U + c,prev);
a=a1;
goto LOOP;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp2;
# 487
++ c;
a=a1;
goto LOOP;}default: _Tmp2=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;void*t=_Tmp1;void*a1=_Tmp0;
# 491
++ c;
a=a1;
goto LOOP;}};}}
# 498
static unsigned Cyc_AssnDef_max_assn_size=0U;
# 501
unsigned Cyc_AssnDef_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned c=Cyc_AssnDef_assn_size_loop(a,0U,& prev);
if(c > Cyc_AssnDef_max_assn_size)
Cyc_AssnDef_max_assn_size=c;
# 507
return c;}struct _tuple26{void*f0;struct _fat_ptr f1;struct _fat_ptr f2;};
# 510
static struct _fat_ptr Cyc_AssnDef_d2string(void*a,int*c,struct Cyc_List_List**prev){
# 512
{struct Cyc_List_List*ps=*prev;for(0;ps!=0;ps=ps->tl){
struct _tuple26*_Tmp0=(struct _tuple26*)ps->hd;struct _fat_ptr _Tmp1;struct _fat_ptr _Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f0;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{void*a1=_Tmp3;struct _fat_ptr n=_Tmp2;struct _fat_ptr v=_Tmp1;
if((unsigned)a1==(unsigned)a)return n;}}}{
# 516
struct _fat_ptr n=({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)*c;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_aprintf(_tag_fat("A%d",sizeof(char),4U),_tag_fat(_Tmp1,sizeof(void*),1));});
*c=*c + 1;{
struct _fat_ptr s;
{void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 s=_tag_fat("true",sizeof(char),5U);goto _LL3;case 1:
 s=_tag_fat("false",sizeof(char),6U);goto _LL3;case 2:
 s=Cyc_AssnDef_assn2string(a);goto _LL3;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 524
struct _fat_ptr d1=Cyc_AssnDef_d2string(a1,c,prev);
struct _fat_ptr d2=Cyc_AssnDef_d2string(a2,c,prev);
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)d1);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)d2);_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL3;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 529
struct _fat_ptr d1=Cyc_AssnDef_d2string(a1,c,prev);
struct _fat_ptr d2=Cyc_AssnDef_d2string(a2,c,prev);
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)d1);_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,_Tmp5.f1=(struct _fat_ptr)((struct _fat_ptr)d2);_Tmp5;});void*_Tmp5[2];_Tmp5[0]=& _Tmp3,_Tmp5[1]=& _Tmp4;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_Tmp5,sizeof(void*),2));});
goto _LL3;}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp2;
# 534
struct _fat_ptr d=Cyc_AssnDef_d2string(a,c,prev);
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)d);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(_tag_fat("not(%s)",sizeof(char),8U),_tag_fat(_Tmp4,sizeof(void*),1));});
goto _LL3;}default: _Tmp2=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;void*t=_Tmp1;void*a=_Tmp0;
# 538
struct _fat_ptr d=Cyc_AssnDef_d2string(a,c,prev);
s=({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp4.f1=_Tmp5;});_Tmp4;});struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({
struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)d);_Tmp6;});void*_Tmp6[3];_Tmp6[0]=& _Tmp3,_Tmp6[1]=& _Tmp4,_Tmp6[2]=& _Tmp5;Cyc_aprintf(_tag_fat("Subst[%s=%s](%s)",sizeof(char),17U),_tag_fat(_Tmp6,sizeof(void*),3));});
goto _LL3;}}_LL3:;}
# 543
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple26*_Tmp2=({struct _tuple26*_Tmp3=_cycalloc(sizeof(struct _tuple26));_Tmp3->f0=a,_Tmp3->f1=(struct _fat_ptr)n,_Tmp3->f2=s;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=*prev;_Tmp1;});*prev=_Tmp0;});
return n;}}}
# 547
struct _fat_ptr Cyc_AssnDef_dag2string(void*a){
struct Cyc_List_List*decls=0;
struct _fat_ptr*newline;newline=_cycalloc(sizeof(struct _fat_ptr)),*newline=_tag_fat("\n",sizeof(char),2U);{
struct _fat_ptr*equals;equals=_cycalloc(sizeof(struct _fat_ptr)),*equals=_tag_fat(" = ",sizeof(char),4U);{
int count=0;
struct _fat_ptr res=Cyc_AssnDef_d2string(a,& count,& decls);
struct Cyc_List_List*strings=({struct _fat_ptr*_Tmp0[3];({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=_tag_fat(" in ",sizeof(char),5U);_Tmp2;});_Tmp0[0]=_Tmp1;}),({struct _fat_ptr*_Tmp1=({struct _fat_ptr*_Tmp2=_cycalloc(sizeof(struct _fat_ptr));*_Tmp2=res;_Tmp2;});_Tmp0[1]=_Tmp1;}),_Tmp0[2]=newline;Cyc_List_list(_tag_fat(_Tmp0,sizeof(struct _fat_ptr*),3));});
for(1;decls!=0;decls=_check_null(decls)->tl){
struct _tuple26*_Tmp0=(struct _tuple26*)decls->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(struct _fat_ptr*)& _Tmp0->f1;_Tmp1=(struct _fat_ptr*)& _Tmp0->f2;{struct _fat_ptr*n=(struct _fat_ptr*)_Tmp2;struct _fat_ptr*d=(struct _fat_ptr*)_Tmp1;
strings=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=n,({struct Cyc_List_List*_Tmp4=({struct Cyc_List_List*_Tmp5=_cycalloc(sizeof(struct Cyc_List_List));_Tmp5->hd=equals,({struct Cyc_List_List*_Tmp6=({struct Cyc_List_List*_Tmp7=_cycalloc(sizeof(struct Cyc_List_List));_Tmp7->hd=d,({struct Cyc_List_List*_Tmp8=({struct Cyc_List_List*_Tmp9=_cycalloc(sizeof(struct Cyc_List_List));_Tmp9->hd=newline,_Tmp9->tl=strings;_Tmp9;});_Tmp7->tl=_Tmp8;});_Tmp7;});_Tmp5->tl=_Tmp6;});_Tmp5;});_Tmp3->tl=_Tmp4;});_Tmp3;});}}
# 558
return Cyc_strconcat_l(strings);}}}
# 563
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec,int*size){
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 return _tag_fat("true",sizeof(char),5U);case 1:
 return _tag_fat("false",sizeof(char),6U);case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 568
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
# 579
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(_tag_fat("%s%s%s",sizeof(char),7U),_tag_fat(_Tmp7,sizeof(void*),3));});}}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 581
*size +=1;
if(prec <= 5)
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("%s || %s",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 585
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("(%s || %s)",sizeof(char),11U),_tag_fat(_Tmp6,sizeof(void*),2));});}}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 587
*size +=1;
if(prec <= 10)
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("%s && %s",sizeof(char),9U),_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 591
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(_tag_fat("(%s && %s)",sizeof(char),11U),_tag_fat(_Tmp6,sizeof(void*),2));});}}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
# 593
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,20,size));_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(_tag_fat("!%s",sizeof(char),4U),_tag_fat(_Tmp5,sizeof(void*),1));});}default: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a=_Tmp0;
# 596
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(_tag_fat("Subst[%s:=%s](%s)",sizeof(char),18U),_tag_fat(_Tmp7,sizeof(void*),3));});}};}
# 602
int Cyc_AssnDef_tree_size=0;
# 605
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
Cyc_AssnDef_tree_size=0;
return Cyc_AssnDef_a2string(a,0,& Cyc_AssnDef_tree_size);}
# 611
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 622 "assndef.cyc"
int Cyc_AssnDef_assnhash(void*a){
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 1:
 return 0;case 0:
 return 1;case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 627
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
return(int)((unsigned)a1 ^ (unsigned)a2 | 1U);}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
return(int)((unsigned)a1 + 61U);}default: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a=_Tmp0;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | 3U);}};}
# 638
int Cyc_AssnDef_assncmp(void*a1,void*a2){
LOOP:
 if(a1==a2)return 0;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_AssnDef_Primreln _Tmp3;enum Cyc_AssnDef_Primreln _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp0.f0)==0){if(*((int*)_Tmp0.f1)==0)
return 0;else{
return -1;}}else{if(*((int*)_Tmp0.f1)==0)
return 1;else{if(*((int*)_Tmp0.f0)==1){if(*((int*)_Tmp0.f1)==1)
return 0;else{
return -1;}}else{if(*((int*)_Tmp0.f1)==1)
return 1;else{switch(*((int*)_Tmp0.f0)){case 4: if(*((int*)_Tmp0.f1)==4){_Tmp8=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f2;{void*a11=_Tmp8;void*a12=_Tmp7;void*a21=_Tmp6;void*a22=_Tmp5;
_Tmp8=a11;_Tmp7=a12;_Tmp6=a21;_Tmp5=a22;goto _LL10;}}else{
# 655
return -1;}case 3: switch(*((int*)_Tmp0.f1)){case 3: _Tmp8=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_LL10: {void*a11=_Tmp8;void*a12=_Tmp7;void*a21=_Tmp6;void*a22=_Tmp5;
# 650
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c!=0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4: goto _LL13;default:
# 657
 return -1;}default: switch(*((int*)_Tmp0.f1)){case 4: _LL13:
# 656
 return 1;case 3:
# 658
 return 1;default: if(*((int*)_Tmp0.f0)==2){if(*((int*)_Tmp0.f1)==2){_Tmp8=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f1;_Tmp4=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f0)->f3;_Tmp6=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp3=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f3;{void*t11=_Tmp8;enum Cyc_AssnDef_Primreln p1=_Tmp4;void*t12=_Tmp7;void*t21=_Tmp6;enum Cyc_AssnDef_Primreln p2=_Tmp3;void*t22=_Tmp5;
# 660
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
# 671
int c=(int)vd1 - (int)vd2;
if(c!=0)return c;
c=Cyc_AssnDef_cmp_term(t1,t2);
if(c!=0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}};}}struct Cyc_AssnDef_AssnInfo{void*assn;void*reduced;struct Cyc_Set_Set**widened_set;};
# 691
static struct Cyc_Hashtable_Table*Cyc_AssnDef_hash_cons_table=0;
# 694
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_term_hash_cons_table=0;
Cyc_AssnDef_hash_cons_table=0;}
# 699
static struct Cyc_Hashtable_Table*Cyc_AssnDef_get_hash_cons_table (void){
# 701
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_hash_cons_table==0){
h=Cyc_Hashtable_create(123,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_hash_cons_table=h;}else{
# 706
h=_check_null(Cyc_AssnDef_hash_cons_table);}
# 708
return h;}
# 710
static struct Cyc_AssnDef_AssnInfo*Cyc_AssnDef_hash_cons_it(void*);
static void*Cyc_AssnDef_hash_cons(void*);
# 717
static struct Cyc_AssnDef_AssnInfo*Cyc_AssnDef_copy_assn(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:  {
# 720
struct Cyc_Set_Set*s=Cyc_Set_empty(Cyc_AssnDef_assncmp);
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=(void*)& Cyc_AssnDef_true_assn,_Tmp4->reduced=(void*)& Cyc_AssnDef_true_assn,({struct Cyc_Set_Set**_Tmp5=({struct Cyc_Set_Set**_Tmp6=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp6=s;_Tmp6;});_Tmp4->widened_set=_Tmp5;});_Tmp4;});}case 1:
# 723
 return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=(void*)& Cyc_AssnDef_false_assn,_Tmp4->reduced=(void*)& Cyc_AssnDef_false_assn,({struct Cyc_Set_Set**_Tmp5=({struct Cyc_Set_Set**_Tmp6=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp6=0;_Tmp6;});_Tmp4->widened_set=_Tmp5;});_Tmp4;});case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp2;
# 725
struct Cyc_AssnDef_AssnInfo*a1info=Cyc_AssnDef_hash_cons_it(a1);
struct Cyc_AssnDef_AssnInfo*a2info=Cyc_AssnDef_hash_cons_it(a2);
int reduced=1;
if(a1info->reduced==0){
reduced=0;
a1=a1info->assn;}else{
# 732
a1=_check_null(a1info->reduced);}
# 734
if(a2info->reduced==0){
reduced=0;
a2=a2info->assn;}else{
# 738
a2=_check_null(a2info->reduced);}{
# 740
void*newa=(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp4->tag=3,_Tmp4->f1=a1,_Tmp4->f2=a2;_Tmp4;});
void*reda=reduced?newa: 0;
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=newa,_Tmp4->reduced=reda,_Tmp4->widened_set=0;_Tmp4;});}}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp2;
# 744
struct Cyc_AssnDef_AssnInfo*a1info=Cyc_AssnDef_hash_cons_it(a1);
struct Cyc_AssnDef_AssnInfo*a2info=Cyc_AssnDef_hash_cons_it(a2);
int reduced=1;
if(a1info->reduced==0){
reduced=0;
a1=a1info->assn;}else{
# 751
a1=_check_null(a1info->reduced);}
# 753
if(a2info->reduced==0){
reduced=0;
a2=a2info->assn;}else{
# 757
a2=_check_null(a2info->reduced);}{
# 759
void*newa=(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp4->tag=4,_Tmp4->f1=a1,_Tmp4->f2=a2;_Tmp4;});
void*reda=reduced?newa: 0;
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=newa,_Tmp4->reduced=reda,_Tmp4->widened_set=0;_Tmp4;});}}case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp2;
# 763
void*newa=(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp4->tag=2,_Tmp4->f1=t1,_Tmp4->f2=p,_Tmp4->f3=t2;_Tmp4;});
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));_Tmp4->assn=newa,_Tmp4->reduced=newa,_Tmp4->widened_set=0;_Tmp4;});}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));({void*_Tmp5=(void*)({struct Cyc_AssnDef_Not_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Not_AssnDef_Assn_struct));_Tmp6->tag=5,({void*_Tmp7=Cyc_AssnDef_hash_cons(a);_Tmp6->f1=_Tmp7;});_Tmp6;});_Tmp4->assn=_Tmp5;}),_Tmp4->reduced=0,_Tmp4->widened_set=0;_Tmp4;});}default: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd1=_Tmp3;void*t1=_Tmp2;void*a1=_Tmp0;
# 767
return({struct Cyc_AssnDef_AssnInfo*_Tmp4=_cycalloc(sizeof(struct Cyc_AssnDef_AssnInfo));({void*_Tmp5=(void*)({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Subst_AssnDef_Assn_struct));_Tmp6->tag=6,_Tmp6->f1=vd1,_Tmp6->f2=t1,({void*_Tmp7=Cyc_AssnDef_hash_cons(a1);_Tmp6->f3=_Tmp7;});_Tmp6;});_Tmp4->assn=_Tmp5;}),_Tmp4->reduced=0,_Tmp4->widened_set=0;_Tmp4;});}};}
# 775
static struct Cyc_AssnDef_AssnInfo*Cyc_AssnDef_hash_cons_it(void*a){
struct Cyc_Hashtable_Table*h=Cyc_AssnDef_get_hash_cons_table();
struct Cyc_AssnDef_AssnInfo**resopt=({(struct Cyc_AssnDef_AssnInfo**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
if(resopt==0){
struct Cyc_AssnDef_AssnInfo*ainfo=Cyc_AssnDef_copy_assn(a);
({(void(*)(struct Cyc_Hashtable_Table*,void*,struct Cyc_AssnDef_AssnInfo*))Cyc_Hashtable_insert;})(h,ainfo->assn,ainfo);
return ainfo;}else{
# 783
return*resopt;}}
# 787
static void*Cyc_AssnDef_hash_cons(void*a){
struct Cyc_AssnDef_AssnInfo*ainfo=Cyc_AssnDef_hash_cons_it(a);
void*redopt=ainfo->reduced;
if(redopt==0)return ainfo->assn;else{
return redopt;}}static char _TmpG0[5U]="#Mem";
# 796
static struct _fat_ptr Cyc_AssnDef_memory_var={_TmpG0,_TmpG0,_TmpG0 + 5U};
static struct _tuple0 Cyc_AssnDef_memvar={.f0={.Loc_n={4,0}},.f1=& Cyc_AssnDef_memory_var};
static struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct Cyc_AssnDef_void_con={0};
static struct Cyc_Absyn_AppType_Absyn_Type_struct Cyc_AssnDef_vd_type={0,(void*)& Cyc_AssnDef_void_con,0};
# 802
struct Cyc_Absyn_Vardecl Cyc_AssnDef_memory_struct={Cyc_Absyn_Public,& Cyc_AssnDef_memvar,0U,{0,0,0,0,0U},(void*)& Cyc_AssnDef_vd_type,0,0,0,0,0};
# 814
struct Cyc_Absyn_Vardecl*Cyc_AssnDef_memory=& Cyc_AssnDef_memory_struct;
# 818
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a2;else{if(*((int*)_Tmp0.f1)==0)
return a1;else{if(*((int*)_Tmp0.f0)==1)
return a1;else{if(*((int*)_Tmp0.f1)==1)
return a2;else{
# 828
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=3,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons((void*)& a);}}}};}}
# 834
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a1;else{if(*((int*)_Tmp0.f1)==0)
return a2;else{if(*((int*)_Tmp0.f0)==1)
return a2;else{if(*((int*)_Tmp0.f1)==1)
return a1;else{
# 844
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons((void*)& a);}}}};}}
# 850
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 6: _Tmp2=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;if(x==(struct Cyc_Absyn_Vardecl*)_Tmp2){struct Cyc_Absyn_Vardecl*y=_Tmp2;void*newy=_Tmp1;void*a2=_Tmp0;
# 856
return({struct Cyc_Absyn_Vardecl*_Tmp3=x;void*_Tmp4=Cyc_AssnDef_subst_term(newx,x,newy);Cyc_AssnDef_subst(_Tmp3,_Tmp4,a2);});}else{goto _LL7;}default: _LL7: {
# 858
struct Cyc_AssnDef_Subst_AssnDef_Assn_struct p=({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _Tmp3;_Tmp3.tag=6,_Tmp3.f1=x,_Tmp3.f2=newx,_Tmp3.f3=a;_Tmp3;});
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 864
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=t1,_Tmp0.f2=p,_Tmp0.f3=t2;_Tmp0;});
return Cyc_AssnDef_hash_cons((void*)& ptr);}
# 870
void*Cyc_AssnDef_eq(void*t1opt,void*t2opt){
if(t1opt==0 || t2opt==0)return(void*)& Cyc_AssnDef_true_assn;{
void*t1=t1opt;
void*t2=t2opt;
if(Cyc_AssnDef_cmp_term(t1,t2)> 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 877
return Cyc_AssnDef_prim(t1,0U,t2);}}
# 881
void*Cyc_AssnDef_neq(void*t1opt,void*t2opt){
if(t1opt==0 || t2opt==0)return(void*)& Cyc_AssnDef_true_assn;{
void*t1=t1opt;
void*t2=t2opt;
if(Cyc_AssnDef_cmp_term(t1,t2)> 0)
({void*_Tmp0=t1;void*_Tmp1=t2;t1=_Tmp1;t2=_Tmp0;});
# 888
return Cyc_AssnDef_prim(t1,1U,t2);}}
# 897
void*Cyc_AssnDef_slt(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,2U,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,3U,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,4U,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,5U,t2);}
# 906
static void*Cyc_AssnDef_not_prim(void*t1,enum Cyc_Absyn_Primop p,void*t2){
switch((int)p){case Cyc_AssnDef_Eq:
 return Cyc_AssnDef_neq(t1,t2);case Cyc_AssnDef_Neq:
 return Cyc_AssnDef_eq(t1,t2);case Cyc_AssnDef_SLt:
 return Cyc_AssnDef_slte(t2,t1);case Cyc_AssnDef_SLte:
 return Cyc_AssnDef_slt(t2,t1);case Cyc_AssnDef_ULt:
 return Cyc_AssnDef_ulte(t2,t1);case Cyc_AssnDef_ULte:
 return Cyc_AssnDef_ult(t2,t1);default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(_tag_fat("Vcgen::not_prim: non-primop",sizeof(char),28U),_tag_fat(0U,sizeof(void*),0));};}
# 918
void*Cyc_AssnDef_not(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
return Cyc_AssnDef_not_prim(t1,(enum Cyc_Absyn_Primop)p,t2);}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp2;
return a;}default:  {
# 925
struct Cyc_AssnDef_Not_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Not_AssnDef_Assn_struct _Tmp3;_Tmp3.tag=5,_Tmp3.f1=a;_Tmp3;});
return Cyc_AssnDef_hash_cons((void*)& ptr);}};}struct _tuple27{struct Cyc_Absyn_Vardecl*f0;void*f1;};
# 943 "assndef.cyc"
static void*Cyc_AssnDef_reduce_term(void*t,struct Cyc_List_List*subst){
# 945
void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 6: _LL6:
 return t;case 9: _Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp3;void*tm=_Tmp2;
# 950
void*tm2=Cyc_AssnDef_reduce_term(tm,subst);
if(tm!=tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 7: _Tmp1=((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Unop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;void*tm=_Tmp3;void*tp=_Tmp2;
# 954
void*tm2=Cyc_AssnDef_reduce_term(tm,subst);
if(tm!=tm2)return Cyc_AssnDef_unop(p,tm2,tp);else{
return t;}}case 8: _Tmp1=((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f3;_Tmp0=(void*)((struct Cyc_AssnDef_Binop_AssnDef_Term_struct*)t)->f4;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp3;void*s1=_Tmp2;void*tp=_Tmp0;
# 958
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
void*s2=Cyc_AssnDef_reduce_term(s1,subst);
if(t1!=t2 || s1!=s2)return Cyc_AssnDef_binop(p,t2,s2,tp);else{
return t;}}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Select_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp3;void*s1=_Tmp2;void*tp=_Tmp0;
# 963
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
void*s2=Cyc_AssnDef_reduce_term(t2,subst);
if(t1!=t2 || s1!=s2)return Cyc_AssnDef_select(t2,s2,tp);else{
return t;}}case 5: _Tmp3=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Update_AssnDef_Term_struct*)t)->f3;{void*t1=_Tmp3;void*s1=_Tmp2;void*u1=_Tmp0;
# 968
void*t2=Cyc_AssnDef_reduce_term(t1,subst);
void*s2=Cyc_AssnDef_reduce_term(s1,subst);
void*u2=Cyc_AssnDef_reduce_term(u1,subst);
if((t1!=t2 || s1!=s2)|| u1!=u2)return Cyc_AssnDef_update(t2,s2,u2);else{
return t;}}case 3: _Tmp3=((struct Cyc_AssnDef_Addr_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
_Tmp3=vd;goto _LL14;}default: _Tmp3=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;_LL14: {struct Cyc_Absyn_Vardecl*vd=_Tmp3;
# 975
for(1;subst!=0;subst=subst->tl){
struct _tuple27 _Tmp4=*((struct _tuple27*)subst->hd);void*_Tmp5;void*_Tmp6;_Tmp6=_Tmp4.f0;_Tmp5=_Tmp4.f1;{struct Cyc_Absyn_Vardecl*vd2=_Tmp6;void*t2=_Tmp5;
if(vd==vd2)return t2;}}
# 979
return t;}};}
# 983
static void*Cyc_AssnDef_reduce_assn(void*a,struct Cyc_List_List*subst,int negate){
# 985
void*res;
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 if(negate)return(void*)& Cyc_AssnDef_false_assn;else{return(void*)& Cyc_AssnDef_true_assn;}case 1:
 if(negate)return(void*)& Cyc_AssnDef_true_assn;else{return(void*)& Cyc_AssnDef_false_assn;}case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 991
void*newt1=Cyc_AssnDef_reduce_term(t1,subst);
void*newt2=Cyc_AssnDef_reduce_term(t2,subst);
if(negate)return Cyc_AssnDef_not_prim(t1,(enum Cyc_Absyn_Primop)p,t2);
if(t1!=newt1 || t2!=newt2)
a=Cyc_AssnDef_prim(newt1,p,newt2);
return a;}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 998
void*newa1=Cyc_AssnDef_reduce_assn(a1,subst,negate);
void*newa2=Cyc_AssnDef_reduce_assn(a2,subst,negate);
if(negate)return Cyc_AssnDef_or(newa1,newa2);
if(a1!=newa1 || a2!=newa2)
a=Cyc_AssnDef_and(newa1,newa2);
return a;}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 1005
void*newa1=Cyc_AssnDef_reduce_assn(a1,subst,negate);
void*newa2=Cyc_AssnDef_reduce_assn(a2,subst,negate);
if(negate)return Cyc_AssnDef_and(newa1,newa2);
if(a1!=newa1 || a2!=newa2)
a=Cyc_AssnDef_or(newa1,newa2);
return a;}case 6: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a1=_Tmp0;
# 1012
t=Cyc_AssnDef_reduce_term(t,subst);
# 1014
subst=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple27*_Tmp5=({struct _tuple27*_Tmp6=_cycalloc(sizeof(struct _tuple27));_Tmp6->f0=vd,_Tmp6->f1=t;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=subst;_Tmp4;});
a=a1;
goto LOOP;}default: _Tmp3=(void*)((struct Cyc_AssnDef_Not_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp3;
# 1018
a=a1;
negate=!negate;
goto LOOP;}};}}
# 1025
void*Cyc_AssnDef_reduce(void*a){
# 1027
struct Cyc_AssnDef_AssnInfo*ainfo=Cyc_AssnDef_hash_cons_it(a);
void*redopt=ainfo->reduced;
if(redopt!=0)return redopt;
a=Cyc_AssnDef_reduce_assn(a,0,0);
ainfo->reduced=a;
return a;}
# 1036
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
struct _tuple27 s=({struct _tuple27 _Tmp0;_Tmp0.f0=x,_Tmp0.f1=newx;_Tmp0;});
struct Cyc_List_List subst=({struct Cyc_List_List _Tmp0;_Tmp0.hd=& s,_Tmp0.tl=0;_Tmp0;});
return Cyc_AssnDef_reduce_term(t,& subst);}struct _tuple28{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};
# 1051 "assndef.cyc"
static void Cyc_AssnDef_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 1055
while(1){
struct _tuple28 _Tmp0=({struct _tuple28 _Tmp1;_Tmp1.f0=a1s,_Tmp1.f1=a2s;_Tmp1;});if(_Tmp0.f0==0){if(_Tmp0.f1==0)
return;else{
# 1059
({struct Cyc_List_List*_Tmp1=Cyc_List_revappend(a2s,*res2);*res2=_Tmp1;});return;}}else{if(_Tmp0.f1==0){
# 1058
({struct Cyc_List_List*_Tmp1=Cyc_List_revappend(a1s,*res1);*res1=_Tmp1;});return;}else{
# 1061
int c=({void*_Tmp1=(void*)_check_null(a1s)->hd;Cyc_AssnDef_assncmp(_Tmp1,(void*)_check_null(a2s)->hd);});
if(c==0){
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a1s->hd,_Tmp2->tl=*common;_Tmp2;});*common=_Tmp1;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a1s->hd,_Tmp2->tl=*res1;_Tmp2;});*res1=_Tmp1;});
a1s=a1s->tl;}else{
# 1070
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a2s->hd,_Tmp2->tl=*res2;_Tmp2;});*res2=_Tmp1;});
a2s=a2s->tl;}}}};}}struct _tuple29{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 1077
struct _tuple29 Cyc_AssnDef_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 1079
struct Cyc_List_List*common=0;
struct Cyc_List_List*res1=0;
struct Cyc_List_List*res2=0;
Cyc_AssnDef_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple29 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(common);_Tmp0.f0=_Tmp1;}),({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res1);_Tmp0.f1=_Tmp1;}),({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res2);_Tmp0.f2=_Tmp1;});_Tmp0;});}struct _tuple30{struct Cyc_List_List*f0;void*f1;};
# 1088
struct _tuple30 Cyc_AssnDef_factor(void*a){
void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 return({struct _tuple30 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=(void*)& Cyc_AssnDef_true_assn;_Tmp2;});case 1:
 return({struct _tuple30 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=(void*)& Cyc_AssnDef_false_assn;_Tmp2;});case 2:
 return({struct _tuple30 _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=a,_Tmp4->tl=0;_Tmp4;});_Tmp2.f0=_Tmp3;}),_Tmp2.f1=(void*)& Cyc_AssnDef_true_assn;_Tmp2;});case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1094
struct _tuple30 _Tmp2=Cyc_AssnDef_factor(a1);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_List_List*c1s=_Tmp4;void*a1=_Tmp3;
struct _tuple30 _Tmp5=Cyc_AssnDef_factor(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*c2s=_Tmp7;void*a2=_Tmp6;
return({struct _tuple30 _Tmp8;({struct Cyc_List_List*_Tmp9=Cyc_List_merge(Cyc_AssnDef_assncmp,c1s,c2s);_Tmp8.f0=_Tmp9;}),({void*_Tmp9=Cyc_AssnDef_and(a1,a2);_Tmp8.f1=_Tmp9;});_Tmp8;});}}}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 1098
struct _tuple30 _Tmp2=Cyc_AssnDef_factor(a1);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f0;_Tmp3=_Tmp2.f1;{struct Cyc_List_List*c1s=_Tmp4;void*a1=_Tmp3;
struct _tuple30 _Tmp5=Cyc_AssnDef_factor(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f0;_Tmp6=_Tmp5.f1;{struct Cyc_List_List*c2s=_Tmp7;void*a2=_Tmp6;
struct _tuple29 _Tmp8=Cyc_AssnDef_intersect_assns(c1s,c2s);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f0;_TmpA=_Tmp8.f1;_Tmp9=_Tmp8.f2;{struct Cyc_List_List*cs=_TmpB;struct Cyc_List_List*c1s=_TmpA;struct Cyc_List_List*c2s=_Tmp9;
return({struct _tuple30 _TmpC;_TmpC.f0=cs,({void*_TmpD=({void*_TmpE=Cyc_List_fold_left(Cyc_AssnDef_and,a1,c1s);Cyc_AssnDef_or(_TmpE,Cyc_List_fold_left(Cyc_AssnDef_and,a2,c2s));});_TmpC.f1=_TmpD;});_TmpC;});}}}}default:
 return({struct _tuple30 _Tmp2;_Tmp2.f0=0,_Tmp2.f1=a;_Tmp2;});};}
# 1106
void*Cyc_AssnDef_factor_assn(void*a){
# 1108
struct _tuple30 _Tmp0=Cyc_AssnDef_factor(a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_List_List*cs=_Tmp2;void*a=_Tmp1;
a=Cyc_List_fold_left(Cyc_AssnDef_and,a,cs);
# 1113
return a;}}
# 1118
struct Cyc_Set_Set*Cyc_AssnDef_widen_it(void*a){
struct Cyc_AssnDef_AssnInfo*ainfo=Cyc_AssnDef_hash_cons_it(a);
if(ainfo->widened_set!=0)return*_check_null(ainfo->widened_set);{
struct Cyc_Set_Set*s;
{enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 s=Cyc_Set_empty(Cyc_AssnDef_assncmp);goto _LL0;case 1:
 s=0;goto _LL0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1126
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0 || s2==0)
s=0;else{
# 1131
s=Cyc_Set_union_two(s1,s2);}
goto _LL0;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 1134
struct Cyc_Set_Set*s1=Cyc_AssnDef_widen_it(a1);
struct Cyc_Set_Set*s2=Cyc_AssnDef_widen_it(a2);
if(s1==0)
s=s2;else{
if(s2==0)
s=s1;else{
# 1141
s=Cyc_Set_intersect(s1,s2);}}
goto _LL0;}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 1144
s=Cyc_Set_singleton(Cyc_AssnDef_assncmp,a);
switch((int)p){case Cyc_AssnDef_Eq:  {
# 1150
void*topt1=Cyc_AssnDef_get_term_type(t1);
void*topt2=Cyc_AssnDef_get_term_type(t2);
if(topt1!=0 && Cyc_Tcutil_is_integral_type(topt1)||
 topt2!=0 && Cyc_Tcutil_is_integral_type(topt2)){
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t2,t1));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t2,t1));});}
# 1159
goto _LLD;}case Cyc_AssnDef_SLt:
# 1161
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_slte(t1,t2));});
goto _LLD;case Cyc_AssnDef_ULt:
# 1164
 s=({struct Cyc_Set_Set*_Tmp3=s;Cyc_Set_insert(_Tmp3,Cyc_AssnDef_ulte(t1,t2));});
goto _LLD;default:
 goto _LLD;}_LLD:;
# 1168
goto _LL0;}default:
# 1170
 s=Cyc_AssnDef_widen_it(Cyc_AssnDef_reduce(a));
goto _LL0;}_LL0:;}
# 1173
({struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**_Tmp1=_cycalloc(sizeof(struct Cyc_Set_Set*));*_Tmp1=s;_Tmp1;});ainfo->widened_set=_Tmp0;});
return s;}}
# 1178
void*Cyc_AssnDef_widen(void*a){
# 1180
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(a);
if(sopt==0)return(void*)& Cyc_AssnDef_false_assn;{
void*res=Cyc_Set_fold(Cyc_AssnDef_and,sopt,(void*)& Cyc_AssnDef_true_assn);
return res;}}
# 1188
static int Cyc_AssnDef_simple_prv(struct Cyc_Set_Set*ctxt,void*a){
while(1){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;switch(*((int*)a)){case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp0;
# 1192
if(Cyc_Set_member(ctxt,a))return 1;
switch((int)p){case Cyc_AssnDef_Neq:
# 1195
 return((({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t2,t1));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t2,t1));}))||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));});case Cyc_AssnDef_SLte:
# 1200
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_slt(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});case Cyc_AssnDef_ULte:
# 1203
 return({struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_ult(t1,t2));})||({
struct Cyc_Set_Set*_Tmp3=ctxt;Cyc_Set_member(_Tmp3,Cyc_AssnDef_eq(t1,t2));});default:
 return 0;};}case 0:
# 1207
 return 1;case 1:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1210
if(!Cyc_AssnDef_simple_prv(ctxt,a1))return 0;
a=a2;
continue;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp0;
# 1214
if(Cyc_AssnDef_simple_prv(ctxt,a1))return 1;
a=a2;
continue;}default:
# 1218
 a=Cyc_AssnDef_reduce(a);
continue;};}}
# 1227
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
if(ctxt==a)return 1;{
struct Cyc_Set_Set*sopt=Cyc_AssnDef_widen_it(ctxt);
if(sopt==0)return 1;
return Cyc_AssnDef_simple_prv(sopt,a);}}
