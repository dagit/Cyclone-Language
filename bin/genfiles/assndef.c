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
#define _zero_arr_plus_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_char_fn _zero_arr_plus_fn
#define _zero_arr_plus_short_fn _zero_arr_plus_fn
#define _zero_arr_plus_int_fn _zero_arr_plus_fn
#define _zero_arr_plus_float_fn _zero_arr_plus_fn
#define _zero_arr_plus_double_fn _zero_arr_plus_fn
#define _zero_arr_plus_longdouble_fn _zero_arr_plus_fn
#define _zero_arr_plus_voidstar_fn _zero_arr_plus_fn
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
short* _zero_arr_plus_short_fn(short*,unsigned,int,const char*,unsigned);
int* _zero_arr_plus_int_fn(int*,unsigned,int,const char*,unsigned);
float* _zero_arr_plus_float_fn(float*,unsigned,int,const char*,unsigned);
double* _zero_arr_plus_double_fn(double*,unsigned,int,const char*,unsigned);
long double* _zero_arr_plus_longdouble_fn(long double*,unsigned,int,const char*, unsigned);
void** _zero_arr_plus_voidstar_fn(void**,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
int _get_zero_arr_size_char(const char*,unsigned);
int _get_zero_arr_size_short(const short*,unsigned);
int _get_zero_arr_size_int(const int*,unsigned);
int _get_zero_arr_size_float(const float*,unsigned);
int _get_zero_arr_size_double(const double*,unsigned);
int _get_zero_arr_size_longdouble(const long double*,unsigned);
int _get_zero_arr_size_voidstar(const void**,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_short_fn(short**,int,const char*,unsigned);
int* _zero_arr_inplace_plus_int(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_longdouble_fn(long double**,int,const char*,unsigned);
void** _zero_arr_inplace_plus_voidstar_fn(void***,int,const char*,unsigned);
/* like the previous functions, but does post-addition (as in e++) */
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_post_short_fn(short**x,int,const char*,unsigned);
int* _zero_arr_inplace_plus_post_int_fn(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_post_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_post_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_post_longdouble_fn(long double**,int,const char *,unsigned);
void** _zero_arr_inplace_plus_post_voidstar_fn(void***,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
 struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 145 "list.h"
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 150 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 171
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 469 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};
# 890 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);struct _tuple11{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple11 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*);
# 46 "evexp.h"
extern int Cyc_Evexp_exp_cmp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*);
# 125 "tcutil.h"
int Cyc_Tcutil_typecmp(void*,void*);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 56
extern void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*,void*);
# 59
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 80 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 130 "assndef.h"
void*Cyc_AssnDef_reduce(void*);
# 42 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
struct _fat_ptr res;
{void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;int _Tmp2;void*_Tmp3;unsigned _Tmp4;switch(*((int*)t)){case 0: _Tmp4=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)t)->f1;{unsigned i=_Tmp4;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=i;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="0x%x";_tag_fat(_Tmp7,sizeof(char),5U);}),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}case 1: _Tmp3=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp3;
res=Cyc_Absynpp_exp2string(e);goto _LL0;}case 2: _Tmp3=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;
res=Cyc_Absynpp_qvar2string(vd->name);goto _LL0;}case 3: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1==0){_Tmp2=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{int i=_Tmp2;
res=({struct Cyc_Int_pa_PrintArg_struct _Tmp5=({struct Cyc_Int_pa_PrintArg_struct _Tmp6;_Tmp6.tag=1,_Tmp6.f1=(unsigned long)i;_Tmp6;});void*_Tmp6[1];_Tmp6[0]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="_X%d";_tag_fat(_Tmp7,sizeof(char),5U);}),_tag_fat(_Tmp6,sizeof(void*),1));});goto _LL0;}}else{_Tmp3=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f2;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;int i=_Tmp2;
# 50
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_qvar2string(_check_null(vd)->name);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_Int_pa_PrintArg_struct _Tmp6=({struct Cyc_Int_pa_PrintArg_struct _Tmp7;_Tmp7.tag=1,_Tmp7.f1=(unsigned long)i;_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="_%s%d";_tag_fat(_Tmp8,sizeof(char),6U);}),_tag_fat(_Tmp7,sizeof(void*),2));});goto _LL0;}}case 4: if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2!=0){if(((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2)->tl!=0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2)->tl)->tl==0){_Tmp1=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2->hd;_Tmp0=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2->tl->hd;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp3;void*t2=_Tmp0;
# 52
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(t1);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_Absynpp_prim2string(p);_Tmp7.f1=_Tmp8;});_Tmp7;});struct Cyc_String_pa_PrintArg_struct _Tmp7=({struct Cyc_String_pa_PrintArg_struct _Tmp8;_Tmp8.tag=0,({
struct _fat_ptr _Tmp9=Cyc_AssnDef_term2string(t2);_Tmp8.f1=_Tmp9;});_Tmp8;});void*_Tmp8[3];_Tmp8[0]=& _Tmp5,_Tmp8[1]=& _Tmp6,_Tmp8[2]=& _Tmp7;Cyc_aprintf(({const char*_Tmp9="(%s%s%s)";_tag_fat(_Tmp9,sizeof(char),9U);}),_tag_fat(_Tmp8,sizeof(void*),3));});
goto _LL0;}}else{goto _LLD;}}else{_LLD: _Tmp1=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2->hd;{enum Cyc_Absyn_Primop p=_Tmp1;void*t1=_Tmp3;
# 56
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_prim2string(p);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(t1);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="%s(%s)";_tag_fat(_Tmp8,sizeof(char),7U);}),_tag_fat(_Tmp7,sizeof(void*),2));});goto _LL0;}}}else{
# 59
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp5="term2string primop args";_tag_fat(_Tmp5,sizeof(char),24U);}),_tag_fat(0U,sizeof(void*),0));}default: _Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp3;void*tm=_Tmp0;
# 58
res=({struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_Absynpp_typ2string(tp);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(tm);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[2];_Tmp7[0]=& _Tmp5,_Tmp7[1]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="(%s)%s";_tag_fat(_Tmp8,sizeof(char),7U);}),_tag_fat(_Tmp7,sizeof(void*),2));});goto _LL0;}}_LL0:;}
# 61
return res;}
# 64
void*Cyc_AssnDef_zero (void){
static struct Cyc_AssnDef_Uint_AssnDef_Term_struct z={0,0U};
return(void*)& z;}
# 68
void*Cyc_AssnDef_one (void){
static struct Cyc_AssnDef_Uint_AssnDef_Term_struct x={0,1U};
return(void*)& x;}
# 72
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){
struct _tuple11 _Tmp0=Cyc_Evexp_eval_const_uint_exp(e);int _Tmp1;unsigned _Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{unsigned c=_Tmp2;int known=_Tmp1;
if(known){
if(c==0U)return Cyc_AssnDef_zero();
if(c==1U)return Cyc_AssnDef_one();
return(void*)({struct Cyc_AssnDef_Uint_AssnDef_Term_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Uint_AssnDef_Term_struct));_Tmp3->tag=0,_Tmp3->f1=c;_Tmp3;});}
# 79
return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_Tmp3=_cycalloc(sizeof(struct Cyc_AssnDef_Const_AssnDef_Term_struct));_Tmp3->tag=1,_Tmp3->f1=e;_Tmp3;});}}
# 82
void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*vd){return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_Var_AssnDef_Term_struct));_Tmp0->tag=2,_Tmp0->f1=vd;_Tmp0;});}
# 84
inline static int Cyc_AssnDef_is_commutative(enum Cyc_Absyn_Primop p){
switch((int)p){case Cyc_Absyn_Plus:
 goto _LL4;case Cyc_Absyn_Times: _LL4:
 goto _LL6;case Cyc_Absyn_Eq: _LL6:
 goto _LL8;case Cyc_Absyn_Neq: _LL8:
 goto _LLA;case Cyc_Absyn_Bitand: _LLA:
 goto _LLC;case Cyc_Absyn_Bitor: _LLC:
 goto _LLE;case Cyc_Absyn_Bitxor: _LLE:
 return 1;default:
 return 0;};}
# 97
void*Cyc_AssnDef_get_term_type(void*t){
void*_Tmp0;switch(*((int*)t)){case 0:
 return Cyc_Absyn_int_type(2U,2U);case 1: _Tmp0=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Exp*e=_Tmp0;
return e->topt;}case 2: _Tmp0=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp0;
return vd->type;}case 3: _Tmp0=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)t)->f3;{void*topt=_Tmp0;
return topt;}case 4: _Tmp0=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f3;{void*t=_Tmp0;
return t;}default: _Tmp0=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;{void*type=_Tmp0;
return type;}};}
# 108
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts,void*tp){
return(void*)({struct Cyc_AssnDef_Primop_AssnDef_Term_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_Primop_AssnDef_Term_struct));_Tmp0->tag=4,_Tmp0->f1=p,_Tmp0->f2=ts,_Tmp0->f3=tp;_Tmp0;});}
# 111
void*Cyc_AssnDef_cast(void*tp,void*tm){
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_Cast_AssnDef_Term_struct));_Tmp0->tag=5,_Tmp0->f1=tp,_Tmp0->f2=tm;_Tmp0;});}
# 114
void*Cyc_AssnDef_fresh_var(void*t){
static int counter=0;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct));_Tmp0->tag=3,_Tmp0->f1=0,_Tmp0->f2=counter ++,_Tmp0->f3=t;_Tmp0;});}struct _tuple23{void*f1;void*f2;};
# 119
int Cyc_AssnDef_cmp_term(void*t1,void*t2){
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f1=t1,_Tmp1.f2=t2;_Tmp1;});enum Cyc_Absyn_Primop _Tmp1;enum Cyc_Absyn_Primop _Tmp2;void*_Tmp3;void*_Tmp4;int _Tmp5;int _Tmp6;void*_Tmp7;void*_Tmp8;unsigned _Tmp9;unsigned _TmpA;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0){_TmpA=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp9=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_Tmp0.f2)->f1;{unsigned i1=_TmpA;unsigned i2=_Tmp9;
return(int)(i1 - i2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)==0)
return 1;else{if(*((int*)_Tmp0.f1)==1){if(*((int*)_Tmp0.f2)==1){_Tmp8=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Exp*e1=_Tmp8;struct Cyc_Absyn_Exp*e2=_Tmp7;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)==1)
return 1;else{if(*((int*)_Tmp0.f1)==2){if(*((int*)_Tmp0.f2)==2){_Tmp8=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_Tmp0.f2)->f1;{struct Cyc_Absyn_Vardecl*vd1=_Tmp8;struct Cyc_Absyn_Vardecl*vd2=_Tmp7;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)==2)
return 1;else{if(*((int*)_Tmp0.f1)==3){if(*((int*)_Tmp0.f2)==3){_Tmp8=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp6=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp7=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f2)->f1;_Tmp5=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_Tmp0.f2)->f2;{struct Cyc_Absyn_Vardecl*vd1=_Tmp8;int i=_Tmp6;struct Cyc_Absyn_Vardecl*vd2=_Tmp7;int j=_Tmp5;
# 131
if(vd1==vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)==3)
return 1;else{if(*((int*)_Tmp0.f1)==5){if(*((int*)_Tmp0.f2)==5){_Tmp8=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp4=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f2)->f1;_Tmp3=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_Tmp0.f2)->f2;{void*tp1=_Tmp8;void*tm1=_Tmp7;void*tp2=_Tmp4;void*tm2=_Tmp3;
# 136
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(tm1,tm2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)==5)
return 1;else{_Tmp2=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_Tmp0.f1)->f1;_Tmp8=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_Tmp0.f1)->f3;_Tmp1=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_Tmp0.f2)->f1;_Tmp4=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_Tmp0.f2)->f2;_Tmp3=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_Tmp0.f2)->f3;{enum Cyc_Absyn_Primop p1=_Tmp2;struct Cyc_List_List*ts1=_Tmp8;void*tp1=_Tmp7;enum Cyc_Absyn_Primop p2=_Tmp1;struct Cyc_List_List*ts2=_Tmp4;void*tp2=_Tmp3;
# 142
if((int)p1!=(int)p2)return(int)p1 - (int)p2;
return Cyc_List_list_cmp(Cyc_AssnDef_cmp_term,ts1,ts2);}}}}}}}}}}};}
# 147
static int Cyc_AssnDef_same_term(void*t1,void*t2){return Cyc_AssnDef_cmp_term(t1,t2)==0;}
# 166
static unsigned Cyc_AssnDef_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 168
LOOP:
 if(Cyc_List_memq(*prev,a))return c;
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=a,_Tmp1->tl=*prev;_Tmp1;});*prev=_Tmp0;});{
void*_Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return c;case 1:
 return c;case 2:
 return 1U + c;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
_Tmp2=a1;_Tmp1=a2;goto _LLA;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;_LLA: {void*a1=_Tmp2;void*a2=_Tmp1;
# 177
c=Cyc_AssnDef_assn_size_loop(a2,1U + c,prev);
a=a1;
goto LOOP;}case 5: _Tmp2=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;void*t=_Tmp1;void*a1=_Tmp0;
# 181
++ c;
a=a1;
goto LOOP;}default: _Tmp2=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a1=_Tmp2;
# 185
++ c;
a=a1;
goto LOOP;}};}}
# 194
static unsigned Cyc_AssnDef_max_assn_size=0U;
# 197
static unsigned Cyc_AssnDef_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned c=Cyc_AssnDef_assn_size_loop(a,0U,& prev);
if(c > Cyc_AssnDef_max_assn_size)
Cyc_AssnDef_max_assn_size=c;
# 203
return c;}
# 208
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec,int*size){
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 return({const char*_Tmp4="true";_tag_fat(_Tmp4,sizeof(char),5U);});case 1:
 return({const char*_Tmp4="false";_tag_fat(_Tmp4,sizeof(char),6U);});case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 213
*size +=1;{
struct _fat_ptr ps;
switch((int)p){case Cyc_AssnDef_Eq:
 ps=({const char*_Tmp4="==";_tag_fat(_Tmp4,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_Neq:
 ps=({const char*_Tmp4="!=";_tag_fat(_Tmp4,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_SLt:
 ps=({const char*_Tmp4=" S< ";_tag_fat(_Tmp4,sizeof(char),5U);});goto _LLF;case Cyc_AssnDef_SLte:
 ps=({const char*_Tmp4=" S<= ";_tag_fat(_Tmp4,sizeof(char),6U);});goto _LLF;case Cyc_AssnDef_ULt:
 ps=({const char*_Tmp4=" U< ";_tag_fat(_Tmp4,sizeof(char),5U);});goto _LLF;case Cyc_AssnDef_ULte:
 ps=({const char*_Tmp4=" U<= ";_tag_fat(_Tmp4,sizeof(char),6U);});goto _LLF;default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp4="assn2string primop";_tag_fat(_Tmp4,sizeof(char),19U);}),_tag_fat(0U,sizeof(void*),0));}_LLF:;
# 224
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_AssnDef_term2string(t1);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,_Tmp6.f1=ps;_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_term2string(t2);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="%s%s%s";_tag_fat(_Tmp8,sizeof(char),7U);}),_tag_fat(_Tmp7,sizeof(void*),3));});}}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 226
*size +=1;
if(prec <= 5)
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_AssnDef_a2string(a1,5,size);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_a2string(a2,5,size);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="%s || %s";_tag_fat(_Tmp7,sizeof(char),9U);}),_tag_fat(_Tmp6,sizeof(void*),2));});else{
# 230
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_AssnDef_a2string(a1,5,size);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_a2string(a2,5,size);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="(%s || %s)";_tag_fat(_Tmp7,sizeof(char),11U);}),_tag_fat(_Tmp6,sizeof(void*),2));});}}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 232
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_AssnDef_a2string(a1,10,size);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({struct _fat_ptr _Tmp7=Cyc_AssnDef_a2string(a2,10,size);_Tmp6.f1=_Tmp7;});_Tmp6;});void*_Tmp6[2];_Tmp6[0]=& _Tmp4,_Tmp6[1]=& _Tmp5;Cyc_aprintf(({const char*_Tmp7="%s && %s";_tag_fat(_Tmp7,sizeof(char),9U);}),_tag_fat(_Tmp6,sizeof(void*),2));});}case 5: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a=_Tmp0;
# 235
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_Absynpp_qvar2string(vd->name);_Tmp5.f1=_Tmp6;});_Tmp5;});struct Cyc_String_pa_PrintArg_struct _Tmp5=({struct Cyc_String_pa_PrintArg_struct _Tmp6;_Tmp6.tag=0,({
struct _fat_ptr _Tmp7=Cyc_AssnDef_term2string(t);_Tmp6.f1=_Tmp7;});_Tmp6;});struct Cyc_String_pa_PrintArg_struct _Tmp6=({struct Cyc_String_pa_PrintArg_struct _Tmp7;_Tmp7.tag=0,({struct _fat_ptr _Tmp8=Cyc_AssnDef_a2string(a,0,size);_Tmp7.f1=_Tmp8;});_Tmp7;});void*_Tmp7[3];_Tmp7[0]=& _Tmp4,_Tmp7[1]=& _Tmp5,_Tmp7[2]=& _Tmp6;Cyc_aprintf(({const char*_Tmp8="Subst[%s:=%s](%s)";_tag_fat(_Tmp8,sizeof(char),18U);}),_tag_fat(_Tmp7,sizeof(void*),3));});}default: _Tmp3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
# 239
*size +=1;
return({struct Cyc_String_pa_PrintArg_struct _Tmp4=({struct Cyc_String_pa_PrintArg_struct _Tmp5;_Tmp5.tag=0,({struct _fat_ptr _Tmp6=Cyc_AssnDef_a2string(a,0,size);_Tmp5.f1=_Tmp6;});_Tmp5;});void*_Tmp5[1];_Tmp5[0]=& _Tmp4;Cyc_aprintf(({const char*_Tmp6="Kill(%s)";_tag_fat(_Tmp6,sizeof(char),9U);}),_tag_fat(_Tmp5,sizeof(void*),1));});}};}
# 247
int Cyc_AssnDef_tree_size=0;
# 250
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
Cyc_AssnDef_tree_size=0;
return Cyc_AssnDef_a2string(a,0,& Cyc_AssnDef_tree_size);}
# 256
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 267 "assndef.cyc"
static int Cyc_AssnDef_assnhash(void*a){
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 1:
 return 0;case 0:
 return 1;case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 272
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
return(int)((unsigned)a1 ^ (unsigned)a2 | 1U);}case 5: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a=_Tmp0;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | 3U);}default: _Tmp3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
# 277
return(int)((unsigned)a + 37U);}};}
# 284
int Cyc_AssnDef_assncmp(void*a1,void*a2){
LOOP:
 if(a1==a2)return 0;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});void*_Tmp1;void*_Tmp2;enum Cyc_AssnDef_Primreln _Tmp3;enum Cyc_AssnDef_Primreln _Tmp4;void*_Tmp5;void*_Tmp6;void*_Tmp7;void*_Tmp8;if(*((int*)_Tmp0.f1)==0){if(*((int*)_Tmp0.f2)==0)
return 0;else{
return -1;}}else{if(*((int*)_Tmp0.f2)==0)
return 1;else{if(*((int*)_Tmp0.f1)==1){if(*((int*)_Tmp0.f2)==1)
return 0;else{
return -1;}}else{if(*((int*)_Tmp0.f2)==1)
return 1;else{switch(*((int*)_Tmp0.f1)){case 4: if(*((int*)_Tmp0.f2)==4){_Tmp8=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f2)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_Tmp0.f2)->f2;{void*a11=_Tmp8;void*a12=_Tmp7;void*a21=_Tmp6;void*a22=_Tmp5;
_Tmp8=a11;_Tmp7=a12;_Tmp6=a21;_Tmp5=a22;goto _LL10;}}else{
# 301
return -1;}case 3: switch(*((int*)_Tmp0.f2)){case 3: _Tmp8=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f2)->f1;_Tmp5=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_Tmp0.f2)->f2;_LL10: {void*a11=_Tmp8;void*a12=_Tmp7;void*a21=_Tmp6;void*a22=_Tmp5;
# 296
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c!=0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4: goto _LL13;default:
# 303
 return -1;}default: switch(*((int*)_Tmp0.f2)){case 4: _LL13:
# 302
 return 1;case 3:
# 304
 return 1;default: if(*((int*)_Tmp0.f1)==2){if(*((int*)_Tmp0.f2)==2){_Tmp8=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp4=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f1)->f3;_Tmp6=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f2)->f1;_Tmp3=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f2)->f2;_Tmp5=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_Tmp0.f2)->f3;{void*t11=_Tmp8;enum Cyc_AssnDef_Primreln p1=_Tmp4;void*t12=_Tmp7;void*t21=_Tmp6;enum Cyc_AssnDef_Primreln p2=_Tmp3;void*t22=_Tmp5;
# 306
int c=(int)p1 - (int)p2;
if(c!=0)return c;
c=Cyc_AssnDef_cmp_term(t11,t21);
if(c!=0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)==2)
return 1;else{if(*((int*)_Tmp0.f1)==6){if(*((int*)_Tmp0.f2)==6){_Tmp8=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_Tmp0.f2)->f1;{void*a1=_Tmp8;void*a2=_Tmp7;
return Cyc_AssnDef_assncmp(a1,a2);}}else{
return -1;}}else{if(*((int*)_Tmp0.f2)==6)
return 1;else{_Tmp8=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f1;_Tmp7=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f2;_Tmp6=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f1)->f3;_Tmp5=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f2)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f2)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_Tmp0.f2)->f3;{struct Cyc_Absyn_Vardecl*vd1=_Tmp8;void*t1=_Tmp7;void*a11=_Tmp6;struct Cyc_Absyn_Vardecl*vd2=_Tmp5;void*t2=_Tmp2;void*a22=_Tmp1;
# 317
int c=(int)vd1 - (int)vd2;
if(c!=0)return c;
c=Cyc_AssnDef_cmp_term(t1,t2);
if(c!=0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}};}}
# 333
static struct Cyc_Hashtable_Table*Cyc_AssnDef_hash_cons_table=0;
# 336
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_hash_cons_table=0;}struct _tuple24{void*f1;int f2;};
# 341
int Cyc_AssnDef_is_reduced(void*a){
if(Cyc_AssnDef_hash_cons_table==0)return 0;{
struct _tuple24**resopt=({(struct _tuple24**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;})(_check_null(Cyc_AssnDef_hash_cons_table),a);
if(resopt==0)return 0;{
struct _tuple24*_Tmp0=*resopt;int _Tmp1;_Tmp1=_Tmp0->f2;{int b=_Tmp1;
return b;}}}}
# 352
static struct _tuple24 Cyc_AssnDef_copy_assn(void*a){
void*_Tmp0;enum Cyc_AssnDef_Primreln _Tmp1;void*_Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 return({struct _tuple24 _Tmp4;_Tmp4.f1=(void*)& Cyc_AssnDef_true_assn,_Tmp4.f2=1;_Tmp4;});case 1:
 return({struct _tuple24 _Tmp4;_Tmp4.f1=(void*)& Cyc_AssnDef_false_assn,_Tmp4.f2=1;_Tmp4;});case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp2;
# 357
return({struct _tuple24 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_And_AssnDef_Assn_struct));_Tmp6->tag=3,_Tmp6->f1=a1,_Tmp6->f2=a2;_Tmp6;});_Tmp4.f1=_Tmp5;}),({int _Tmp5=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_Tmp4.f2=_Tmp5;});_Tmp4;});}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp2;
# 359
return({struct _tuple24 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Or_AssnDef_Assn_struct));_Tmp6->tag=4,_Tmp6->f1=a1,_Tmp6->f2=a2;_Tmp6;});_Tmp4.f1=_Tmp5;}),({int _Tmp5=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_Tmp4.f2=_Tmp5;});_Tmp4;});}case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp1=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp1;void*t2=_Tmp2;
return({struct _tuple24 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Prim_AssnDef_Assn_struct));_Tmp6->tag=2,_Tmp6->f1=t1,_Tmp6->f2=p,_Tmp6->f3=t2;_Tmp6;});_Tmp4.f1=_Tmp5;}),_Tmp4.f2=1;_Tmp4;});}case 5: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp2=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd1=_Tmp3;void*t1=_Tmp2;void*a1=_Tmp0;
return({struct _tuple24 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Subst_AssnDef_Assn_struct));_Tmp6->tag=5,_Tmp6->f1=vd1,_Tmp6->f2=t1,_Tmp6->f3=a1;_Tmp6;});_Tmp4.f1=_Tmp5;}),_Tmp4.f2=0;_Tmp4;});}default: _Tmp3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
return({struct _tuple24 _Tmp4;({void*_Tmp5=(void*)({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*_Tmp6=_cycalloc(sizeof(struct Cyc_AssnDef_Kill_AssnDef_Assn_struct));_Tmp6->tag=6,_Tmp6->f1=a;_Tmp6;});_Tmp4.f1=_Tmp5;}),_Tmp4.f2=0;_Tmp4;});}};}
# 370
static void*Cyc_AssnDef_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_hash_cons_table==0){
h=Cyc_Hashtable_create(123,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_hash_cons_table=h;
# 377
({struct Cyc_Hashtable_Table*_Tmp0=h;({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple24*))Cyc_Hashtable_insert;})(_Tmp0,(void*)& Cyc_AssnDef_true_assn,({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));_Tmp1->f1=(void*)& Cyc_AssnDef_true_assn,_Tmp1->f2=1;_Tmp1;}));});
({struct Cyc_Hashtable_Table*_Tmp0=h;({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple24*))Cyc_Hashtable_insert;})(_Tmp0,(void*)& Cyc_AssnDef_false_assn,({struct _tuple24*_Tmp1=_cycalloc(sizeof(struct _tuple24));_Tmp1->f1=(void*)& Cyc_AssnDef_false_assn,_Tmp1->f2=1;_Tmp1;}));});}else{
# 380
h=_check_null(Cyc_AssnDef_hash_cons_table);}{
struct _tuple24**resopt=({(struct _tuple24**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
if(resopt==0){
struct _tuple24 _Tmp0=Cyc_AssnDef_copy_assn(a);int _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*res=_Tmp2;int reduced=_Tmp1;
({struct Cyc_Hashtable_Table*_Tmp3=h;void*_Tmp4=res;({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple24*))Cyc_Hashtable_insert;})(_Tmp3,_Tmp4,({struct _tuple24*_Tmp5=_cycalloc(sizeof(struct _tuple24));_Tmp5->f1=res,_Tmp5->f2=reduced;_Tmp5;}));});
return res;}}else{
# 387
struct _tuple24*_Tmp0=*resopt;void*_Tmp1;_Tmp1=_Tmp0->f1;{void*res=_Tmp1;
return res;}}}}
# 393
void*Cyc_AssnDef_kill(void*a){
switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 6: _LL6:
 return a;default:  {
# 399
struct Cyc_AssnDef_Kill_AssnDef_Assn_struct p=({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=6,_Tmp0.f1=a;_Tmp0;});
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 405
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});if(*((int*)_Tmp0.f1)==0)
return a2;else{if(*((int*)_Tmp0.f2)==0)
return a1;else{if(*((int*)_Tmp0.f1)==1)
return a1;else{if(*((int*)_Tmp0.f2)==1)
return a2;else{
# 415
struct Cyc_AssnDef_And_AssnDef_Assn_struct a=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=3,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons((void*)& a);}}}};}}
# 421
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1==a2)return a1;{
struct _tuple23 _Tmp0=({struct _tuple23 _Tmp1;_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});if(*((int*)_Tmp0.f1)==0)
return a1;else{if(*((int*)_Tmp0.f2)==0)
return a2;else{if(*((int*)_Tmp0.f1)==1)
return a2;else{if(*((int*)_Tmp0.f2)==1)
return a1;else{
# 431
struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _Tmp1;_Tmp1.tag=4,_Tmp1.f1=a1,_Tmp1.f2=a2;_Tmp1;});
return Cyc_AssnDef_hash_cons((void*)& a);}}}};}}
# 437
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 return a;default:  {
# 442
struct Cyc_AssnDef_Subst_AssnDef_Assn_struct p=({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=5,_Tmp0.f1=x,_Tmp0.f2=newx,_Tmp0.f3=a;_Tmp0;});
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 448
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _Tmp0;_Tmp0.tag=2,_Tmp0.f1=t1,_Tmp0.f2=p,_Tmp0.f3=t2;_Tmp0;});
return Cyc_AssnDef_hash_cons((void*)& ptr);}
# 458
void*Cyc_AssnDef_eq(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,0U,t2);}
void*Cyc_AssnDef_neq(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,1U,t2);}
void*Cyc_AssnDef_slt(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,2U,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,3U,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,4U,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){if(t1==0 || t2==0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,5U,t2);}
# 475 "assndef.cyc"
void*Cyc_AssnDef_not(void*a){
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return({void*_Tmp3=Cyc_AssnDef_not(a1);Cyc_AssnDef_or(_Tmp3,Cyc_AssnDef_not(a2));});}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
return({void*_Tmp3=Cyc_AssnDef_not(a1);Cyc_AssnDef_and(_Tmp3,Cyc_AssnDef_not(a2));});}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
# 482
switch((int)p){case Cyc_AssnDef_Eq:
 return Cyc_AssnDef_neq(t1,t2);case Cyc_AssnDef_Neq:
 return Cyc_AssnDef_eq(t1,t2);case Cyc_AssnDef_SLt:
 return Cyc_AssnDef_slte(t2,t1);case Cyc_AssnDef_SLte:
 return Cyc_AssnDef_slt(t2,t1);case Cyc_AssnDef_ULt:
 return Cyc_AssnDef_ulte(t2,t1);case Cyc_AssnDef_ULte:
 return Cyc_AssnDef_ult(t2,t1);default:
({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;})(({const char*_Tmp3="Vcgen::invert: non-primop";_tag_fat(_Tmp3,sizeof(char),26U);}),_tag_fat(0U,sizeof(void*),0));};}case 5:
# 491
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));default:
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));};}struct _tuple25{struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 514 "assndef.cyc"
static void*Cyc_AssnDef_reduce_term(void*t,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 517
enum Cyc_Absyn_Primop _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)t)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 3: _LL6:
 return t;case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp2;void*tm=_Tmp1;
# 522
void*tm2=Cyc_AssnDef_reduce_term(tm,non_esc_subst,esc_subst);
if(tm!=tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 4: _Tmp0=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp0;struct Cyc_List_List*ts=_Tmp2;void*tp=_Tmp1;
# 526
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*ptr=ts;for(0;ptr!=0;ptr=ptr->tl){
void*old_term=(void*)ptr->hd;
void*new_term=Cyc_AssnDef_reduce_term(old_term,non_esc_subst,esc_subst);
res=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=new_term,_Tmp3->tl=res;_Tmp3;});
if(new_term!=old_term)change=1;}}
# 534
if(!change)return t;
return({enum Cyc_Absyn_Primop _Tmp3=p;struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(res);Cyc_AssnDef_primop(_Tmp3,_Tmp4,tp);});}default: _Tmp2=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 537
if(vd->escapes && esc_subst!=0){
# 540
struct Cyc_List_List*s=*esc_subst;
for(1;s!=0;s=s->tl){
struct _tuple25 _Tmp3=*((struct _tuple25*)s->hd);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{struct Cyc_Absyn_Vardecl*vd2=_Tmp5;void*t2=_Tmp4;
# 544
if(vd2==vd)return t2;}}{
# 548
void*t=Cyc_AssnDef_fresh_var(vd->type);
({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp5=({struct _tuple25*_Tmp6=_cycalloc(sizeof(struct _tuple25));_Tmp6->f1=vd,_Tmp6->f2=t;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=*esc_subst;_Tmp4;});*esc_subst=_Tmp3;});
return t;}}else{
if(!vd->escapes)
# 553
for(1;non_esc_subst!=0;non_esc_subst=non_esc_subst->tl){
struct _tuple25 _Tmp3=*((struct _tuple25*)non_esc_subst->hd);void*_Tmp4;void*_Tmp5;_Tmp5=_Tmp3.f1;_Tmp4=_Tmp3.f2;{struct Cyc_Absyn_Vardecl*vd2=_Tmp5;void*t2=_Tmp4;
if(vd==vd2)return t2;}}}
# 558
return t;}};}
# 562
static void*Cyc_AssnDef_reduce_assn(void*a,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 565
LOOP: {
void*_Tmp0;void*_Tmp1;enum Cyc_AssnDef_Primreln _Tmp2;void*_Tmp3;switch(*((int*)a)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 2: _Tmp3=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp2=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp3;enum Cyc_AssnDef_Primreln p=_Tmp2;void*t2=_Tmp1;
# 570
void*newt1=Cyc_AssnDef_reduce_term(t1,non_esc_subst,esc_subst);
void*newt2=Cyc_AssnDef_reduce_term(t2,non_esc_subst,esc_subst);
if(t1==newt1 && t2==newt2)return a;
return Cyc_AssnDef_prim(newt1,p,newt2);}case 3: _Tmp3=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 575
void*newa1=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);
void*newa2=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);
if(a1==newa1 && a2==newa2)return a;
return Cyc_AssnDef_and(newa1,newa2);}case 4: _Tmp3=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp3;void*a2=_Tmp1;
# 580
void*newa1=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);
void*newa2=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);
if(a1==newa1 && a2==newa2)return a;
return Cyc_AssnDef_or(newa1,newa2);}case 6: _Tmp3=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)a)->f1;{void*a=_Tmp3;
# 587
struct Cyc_List_List*new_esc_subst=0;
return Cyc_AssnDef_reduce_assn(a,non_esc_subst,& new_esc_subst);}default: _Tmp3=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)a)->f3;{struct Cyc_Absyn_Vardecl*vd=_Tmp3;void*t=_Tmp1;void*a1=_Tmp0;
# 591
t=Cyc_AssnDef_reduce_term(t,non_esc_subst,esc_subst);
non_esc_subst=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple25*_Tmp5=({struct _tuple25*_Tmp6=_cycalloc(sizeof(struct _tuple25));_Tmp6->f1=vd,_Tmp6->f2=t;_Tmp6;});_Tmp4->hd=_Tmp5;}),_Tmp4->tl=non_esc_subst;_Tmp4;});
a=a1;
goto LOOP;}};}}
# 599
void*Cyc_AssnDef_reduce(void*a){
# 601
if(Cyc_AssnDef_is_reduced(a))return a;
return Cyc_AssnDef_reduce_assn(a,0,0);}
# 606
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_Tmp0;enum Cyc_Absyn_Primop _Tmp1;void*_Tmp2;switch(*((int*)t)){case 2: _Tmp2=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)t)->f1;{struct Cyc_Absyn_Vardecl*vd=_Tmp2;
# 609
if(vd==x)return newx;
return t;}case 4: _Tmp1=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f1;_Tmp2=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f2;_Tmp0=(void*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)t)->f3;{enum Cyc_Absyn_Primop p=_Tmp1;struct Cyc_List_List*ts=_Tmp2;void*tp=_Tmp0;
# 612
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*ptr=ts;for(0;ptr!=0;ptr=ptr->tl){
void*old_term=(void*)ptr->hd;
void*new_term=Cyc_AssnDef_subst_term(old_term,x,newx);
res=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=new_term,_Tmp3->tl=res;_Tmp3;});
if(new_term!=old_term)change=1;}}
# 620
if(!change)return t;
return({enum Cyc_Absyn_Primop _Tmp3=p;struct Cyc_List_List*_Tmp4=Cyc_List_imp_rev(res);Cyc_AssnDef_primop(_Tmp3,_Tmp4,tp);});}case 5: _Tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)t)->f2;{void*tp=_Tmp2;void*tm=_Tmp0;
# 623
void*tm2=Cyc_AssnDef_subst_term(tm,x,newx);
if(tm!=tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 0:
 goto _LLA;case 1: _LLA:
 goto _LLC;default: _LLC:
 return t;};}
# 633
void*Cyc_AssnDef_kill_mem_term(void*t){
struct Cyc_List_List*esc_subst=0;
return Cyc_AssnDef_reduce_term(t,0,& esc_subst);}struct _tuple26{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 648 "assndef.cyc"
static void Cyc_AssnDef_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 652
while(1){
struct _tuple26 _Tmp0=({struct _tuple26 _Tmp1;_Tmp1.f1=a1s,_Tmp1.f2=a2s;_Tmp1;});if(_Tmp0.f1==0){if(_Tmp0.f2==0)
return;else{
# 656
({struct Cyc_List_List*_Tmp1=Cyc_List_revappend(a2s,*res2);*res2=_Tmp1;});return;}}else{if(_Tmp0.f2==0){
# 655
({struct Cyc_List_List*_Tmp1=Cyc_List_revappend(a1s,*res1);*res1=_Tmp1;});return;}else{
# 658
int c=({void*_Tmp1=(void*)_check_null(a1s)->hd;Cyc_AssnDef_assncmp(_Tmp1,(void*)_check_null(a2s)->hd);});
if(c==0){
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a1s->hd,_Tmp2->tl=*common;_Tmp2;});*common=_Tmp1;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a1s->hd,_Tmp2->tl=*res1;_Tmp2;});*res1=_Tmp1;});
a1s=a1s->tl;}else{
# 667
({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=(void*)a2s->hd,_Tmp2->tl=*res2;_Tmp2;});*res2=_Tmp1;});
a2s=a2s->tl;}}}};}}struct _tuple27{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 674
struct _tuple27 Cyc_AssnDef_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 676
struct Cyc_List_List*common=0;
struct Cyc_List_List*res1=0;
struct Cyc_List_List*res2=0;
Cyc_AssnDef_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple27 _Tmp0;({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(common);_Tmp0.f1=_Tmp1;}),({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res1);_Tmp0.f2=_Tmp1;}),({struct Cyc_List_List*_Tmp1=Cyc_List_imp_rev(res2);_Tmp0.f3=_Tmp1;});_Tmp0;});}struct _tuple28{struct Cyc_List_List*f1;void*f2;};
# 685
struct _tuple28 Cyc_AssnDef_factor(void*a){
void*_Tmp0;void*_Tmp1;switch(*((int*)a)){case 0:
 return({struct _tuple28 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=(void*)& Cyc_AssnDef_true_assn;_Tmp2;});case 1:
 return({struct _tuple28 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=(void*)& Cyc_AssnDef_false_assn;_Tmp2;});case 2:
 return({struct _tuple28 _Tmp2;({struct Cyc_List_List*_Tmp3=({struct Cyc_List_List*_Tmp4=_cycalloc(sizeof(struct Cyc_List_List));_Tmp4->hd=a,_Tmp4->tl=0;_Tmp4;});_Tmp2.f1=_Tmp3;}),_Tmp2.f2=(void*)& Cyc_AssnDef_true_assn;_Tmp2;});case 3: _Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 691
struct _tuple28 _Tmp2=Cyc_AssnDef_factor(a1);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_List_List*c1s=_Tmp4;void*a1=_Tmp3;
struct _tuple28 _Tmp5=Cyc_AssnDef_factor(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{struct Cyc_List_List*c2s=_Tmp7;void*a2=_Tmp6;
return({struct _tuple28 _Tmp8;({struct Cyc_List_List*_Tmp9=Cyc_List_merge(Cyc_AssnDef_assncmp,c1s,c2s);_Tmp8.f1=_Tmp9;}),({void*_Tmp9=Cyc_AssnDef_and(a1,a2);_Tmp8.f2=_Tmp9;});_Tmp8;});}}}case 4: _Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp0=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
# 695
struct _tuple28 _Tmp2=Cyc_AssnDef_factor(a1);void*_Tmp3;void*_Tmp4;_Tmp4=_Tmp2.f1;_Tmp3=_Tmp2.f2;{struct Cyc_List_List*c1s=_Tmp4;void*a1=_Tmp3;
struct _tuple28 _Tmp5=Cyc_AssnDef_factor(a2);void*_Tmp6;void*_Tmp7;_Tmp7=_Tmp5.f1;_Tmp6=_Tmp5.f2;{struct Cyc_List_List*c2s=_Tmp7;void*a2=_Tmp6;
struct _tuple27 _Tmp8=Cyc_AssnDef_intersect_assns(c1s,c2s);void*_Tmp9;void*_TmpA;void*_TmpB;_TmpB=_Tmp8.f1;_TmpA=_Tmp8.f2;_Tmp9=_Tmp8.f3;{struct Cyc_List_List*cs=_TmpB;struct Cyc_List_List*c1s=_TmpA;struct Cyc_List_List*c2s=_Tmp9;
return({struct _tuple28 _TmpC;_TmpC.f1=cs,({void*_TmpD=({void*_TmpE=Cyc_List_fold_left(Cyc_AssnDef_and,a1,c1s);Cyc_AssnDef_or(_TmpE,Cyc_List_fold_left(Cyc_AssnDef_and,a2,c2s));});_TmpC.f2=_TmpD;});_TmpC;});}}}}default:
 return({struct _tuple28 _Tmp2;_Tmp2.f1=0,_Tmp2.f2=a;_Tmp2;});};}
# 703
void*Cyc_AssnDef_factor_assn(void*a){
# 705
struct _tuple28 _Tmp0=Cyc_AssnDef_factor(a);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{struct Cyc_List_List*cs=_Tmp2;void*a=_Tmp1;
a=Cyc_List_fold_left(Cyc_AssnDef_and,a,cs);
# 710
return a;}}struct _tuple29{enum Cyc_AssnDef_Primreln f1;enum Cyc_AssnDef_Primreln f2;};
# 717
int Cyc_AssnDef_find_ctxt(void*ctxt,void*t1,enum Cyc_AssnDef_Primreln p1,void*u1){
LOOP: {
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)ctxt)){case 1:
 return 1;case 0:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)ctxt)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)ctxt)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 723
if(Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 1;
ctxt=a2;
goto LOOP;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)ctxt)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)ctxt)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 727
if(!Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 0;
ctxt=a2;
goto LOOP;}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)ctxt)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)ctxt)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)ctxt)->f3;{void*t2=_Tmp2;enum Cyc_AssnDef_Primreln p2=_Tmp0;void*u2=_Tmp1;
# 731
struct _tuple29 _Tmp3=({struct _tuple29 _Tmp4;_Tmp4.f1=p2,_Tmp4.f2=p1;_Tmp4;});switch((int)_Tmp3.f1){case Cyc_AssnDef_Neq: if(_Tmp3.f2==Cyc_AssnDef_Neq)
goto _LL11;else{goto _LL26;}case Cyc_AssnDef_Eq: switch((int)_Tmp3.f2){case Cyc_AssnDef_Eq: _LL11:
 goto _LL13;case Cyc_AssnDef_SLte: _LL13:
 goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 736
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);default: goto _LL26;}case Cyc_AssnDef_SLt: switch((int)_Tmp3.f2){case Cyc_AssnDef_Neq:
 goto _LL19;case Cyc_AssnDef_SLt:
# 742
 goto _LL1D;case Cyc_AssnDef_SLte: _LL1F:
# 744
 goto _LL21;default: goto _LL26;}case Cyc_AssnDef_ULt: switch((int)_Tmp3.f2){case Cyc_AssnDef_Neq: _LL19:
# 740
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);case Cyc_AssnDef_ULt: _LL1D:
# 743
 goto _LL1F;case Cyc_AssnDef_ULte: _LL21:
# 745
 goto _LL23;default: goto _LL26;}case Cyc_AssnDef_SLte: if(_Tmp3.f2==Cyc_AssnDef_SLte){_LL23:
 goto _LL25;}else{goto _LL26;}case Cyc_AssnDef_ULte: if(_Tmp3.f2==Cyc_AssnDef_ULte){_LL25:
# 748
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2);}else{goto _LL26;}default: _LL26:
 return 0;};}default:
# 752
 ctxt=Cyc_AssnDef_reduce(ctxt);
goto LOOP;};}}
# 759
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
LOOP:
 if(ctxt==a)return 1;{
enum Cyc_AssnDef_Primreln _Tmp0;void*_Tmp1;void*_Tmp2;switch(*((int*)a)){case 0:
 return 1;case 1:
 return 0;case 3: _Tmp2=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 766
if(!Cyc_AssnDef_simple_prove(ctxt,a1))return 0;
a=a2;
goto LOOP;}case 4: _Tmp2=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f1;_Tmp1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)a)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 770
if(Cyc_AssnDef_simple_prove(ctxt,a1))return 1;
a=a2;
goto LOOP;}case 2: _Tmp2=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f1;_Tmp0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f2;_Tmp1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)a)->f3;{void*t1=_Tmp2;enum Cyc_AssnDef_Primreln p=_Tmp0;void*t2=_Tmp1;
return Cyc_AssnDef_find_ctxt(ctxt,t1,p,t2);}default:
# 775
 a=Cyc_AssnDef_reduce(a);
goto LOOP;};}}
