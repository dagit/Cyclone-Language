#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file lib/runtime_*.c */
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
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
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

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons *);
void _push_region(struct _RegionHandle *);
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
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
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
      _curr != (unsigned char *)0) \
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

/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);
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
 extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];
# 145 "list.h"
extern void*Cyc_List_fold_left(void*(*)(void*,void*),void*,struct Cyc_List_List*);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*,struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 227
extern struct Cyc_List_List*Cyc_List_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];
# 319
extern int Cyc_List_memq(struct Cyc_List_List*,void*);
# 383
extern int Cyc_List_list_cmp(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 464 "absyn.h"
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};extern char Cyc_Absyn_EmptyAnnot[11U];
# 879 "absyn.h"
void*Cyc_Absyn_int_type(enum Cyc_Absyn_Sign,enum Cyc_Absyn_Size_of);
# 971
struct Cyc_Absyn_Exp*Cyc_Absyn_int_exp(enum Cyc_Absyn_Sign,int,unsigned);struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 71
struct _fat_ptr Cyc_Absynpp_qvar2string(struct _tuple0*);
# 73
struct _fat_ptr Cyc_Absynpp_prim2string(enum Cyc_Absyn_Primop);
# 40 "warn.h"
void*Cyc_Warn_impos(struct _fat_ptr,struct _fat_ptr);
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
extern void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*));struct Cyc_AssnDef_Uint_AssnDef_Term_struct{int tag;unsigned f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 62 "assndef.h"
struct _fat_ptr Cyc_AssnDef_term2string(void*);
# 64
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*);
# 71
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop,struct Cyc_List_List*);
void*Cyc_AssnDef_cast(void*,void*);
void*Cyc_AssnDef_fresh_var(void*);
# 75
int Cyc_AssnDef_cmp_term(void*,void*);
# 80
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};
# 92
int Cyc_AssnDef_assncmp(void*,void*);
# 97
extern struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn;
extern struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn;
# 117 "assndef.h"
void*Cyc_AssnDef_and(void*,void*);
void*Cyc_AssnDef_or(void*,void*);
# 121
void*Cyc_AssnDef_prim(void*,enum Cyc_AssnDef_Primreln,void*);
void*Cyc_AssnDef_eq(void*,void*);
void*Cyc_AssnDef_neq(void*,void*);
void*Cyc_AssnDef_slt(void*,void*);
void*Cyc_AssnDef_slte(void*,void*);
void*Cyc_AssnDef_ult(void*,void*);
void*Cyc_AssnDef_ulte(void*,void*);
# 130
void*Cyc_AssnDef_reduce(void*);
void*Cyc_AssnDef_subst_term(void*,struct Cyc_Absyn_Vardecl*,void*);
void*Cyc_AssnDef_not(void*);
int Cyc_AssnDef_simple_prove(void*,void*);
# 42 "assndef.cyc"
struct _fat_ptr Cyc_AssnDef_term2string(void*t){
void*_tmp0=t;void*_tmp2;enum Cyc_Absyn_Primop _tmp3;int _tmp4;void*_tmp1;unsigned _tmp5;switch(*((int*)_tmp0)){case 0: _tmp5=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_tmp0)->f1;{unsigned i=_tmp5;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp8=({struct Cyc_Int_pa_PrintArg_struct _tmp100;_tmp100.tag=1,_tmp100.f1=(unsigned long)((int)i);_tmp100;});void*_tmp6[1];_tmp6[0]=& _tmp8;({struct _fat_ptr _tmp131=({const char*_tmp7="%d";_tag_fat(_tmp7,sizeof(char),3U);});Cyc_aprintf(_tmp131,_tag_fat(_tmp6,sizeof(void*),1));});});}case 1: _tmp1=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp0)->f1;{struct Cyc_Absyn_Exp*e=_tmp1;
return Cyc_Absynpp_exp2string(e);}case 2: _tmp1=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp0)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp1;
return Cyc_Absynpp_qvar2string(vd->name);}case 3: if(((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f1 == 0){_tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f2;{int i=_tmp4;
return(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmpB=({struct Cyc_Int_pa_PrintArg_struct _tmp101;_tmp101.tag=1,_tmp101.f1=(unsigned long)i;_tmp101;});void*_tmp9[1];_tmp9[0]=& _tmpB;({struct _fat_ptr _tmp132=({const char*_tmpA="_X%d";_tag_fat(_tmpA,sizeof(char),5U);});Cyc_aprintf(_tmp132,_tag_fat(_tmp9,sizeof(void*),1));});});}}else{_tmp1=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f1;_tmp4=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp0)->f2;{struct Cyc_Absyn_Vardecl*vd=_tmp1;int i=_tmp4;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpE=({struct Cyc_String_pa_PrintArg_struct _tmp103;_tmp103.tag=0,({struct _fat_ptr _tmp133=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string((_check_null(vd))->name));_tmp103.f1=_tmp133;});_tmp103;});struct Cyc_Int_pa_PrintArg_struct _tmpF=({struct Cyc_Int_pa_PrintArg_struct _tmp102;_tmp102.tag=1,_tmp102.f1=(unsigned long)i;_tmp102;});void*_tmpC[2];_tmpC[0]=& _tmpE,_tmpC[1]=& _tmpF;({struct _fat_ptr _tmp134=({const char*_tmpD="_%s%d";_tag_fat(_tmpD,sizeof(char),6U);});Cyc_aprintf(_tmp134,_tag_fat(_tmpC,sizeof(void*),2));});});}}case 4: if(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2 != 0){if(((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl != 0){if(((struct Cyc_List_List*)((struct Cyc_List_List*)((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl)->tl == 0){_tmp3=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f1;_tmp1=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->hd;_tmp2=(void*)((((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->tl)->hd;{enum Cyc_Absyn_Primop p=_tmp3;void*t1=_tmp1;void*t2=_tmp2;
# 50
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp106;_tmp106.tag=0,({struct _fat_ptr _tmp135=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp106.f1=_tmp135;});_tmp106;});struct Cyc_String_pa_PrintArg_struct _tmp13=({struct Cyc_String_pa_PrintArg_struct _tmp105;_tmp105.tag=0,({
struct _fat_ptr _tmp136=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp105.f1=_tmp136;});_tmp105;});struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp104;_tmp104.tag=0,({struct _fat_ptr _tmp137=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp104.f1=_tmp137;});_tmp104;});void*_tmp10[3];_tmp10[0]=& _tmp12,_tmp10[1]=& _tmp13,_tmp10[2]=& _tmp14;({struct _fat_ptr _tmp138=({const char*_tmp11="(%s%s%s)";_tag_fat(_tmp11,sizeof(char),9U);});Cyc_aprintf(_tmp138,_tag_fat(_tmp10,sizeof(void*),3));});});}}else{goto _LLD;}}else{_LLD: _tmp3=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f1;_tmp1=(void*)(((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp0)->f2)->hd;{enum Cyc_Absyn_Primop p=_tmp3;void*t=_tmp1;
# 53
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp17=({struct Cyc_String_pa_PrintArg_struct _tmp108;_tmp108.tag=0,({struct _fat_ptr _tmp139=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_prim2string(p));_tmp108.f1=_tmp139;});_tmp108;});struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp107;_tmp107.tag=0,({struct _fat_ptr _tmp13A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp107.f1=_tmp13A;});_tmp107;});void*_tmp15[2];_tmp15[0]=& _tmp17,_tmp15[1]=& _tmp18;({struct _fat_ptr _tmp13B=({const char*_tmp16="%s(%s)";_tag_fat(_tmp16,sizeof(char),7U);});Cyc_aprintf(_tmp13B,_tag_fat(_tmp15,sizeof(void*),2));});});}}}else{
# 56
({void*_tmp1D=0U;({int(*_tmp13D)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp13C=({const char*_tmp1E="term2string primop args";_tag_fat(_tmp1E,sizeof(char),24U);});_tmp13D(_tmp13C,_tag_fat(_tmp1D,sizeof(void*),0));});});}default: _tmp1=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp0)->f1;_tmp2=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp0)->f2;{void*tp=_tmp1;void*tm=_tmp2;
# 55
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1B=({struct Cyc_String_pa_PrintArg_struct _tmp10A;_tmp10A.tag=0,({struct _fat_ptr _tmp13E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(tp));_tmp10A.f1=_tmp13E;});_tmp10A;});struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp109;_tmp109.tag=0,({struct _fat_ptr _tmp13F=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(tm));_tmp109.f1=_tmp13F;});_tmp109;});void*_tmp19[2];_tmp19[0]=& _tmp1B,_tmp19[1]=& _tmp1C;({struct _fat_ptr _tmp140=({const char*_tmp1A="(%s)%s";_tag_fat(_tmp1A,sizeof(char),7U);});Cyc_aprintf(_tmp140,_tag_fat(_tmp19,sizeof(void*),2));});});}};}
# 60
void*Cyc_AssnDef_cnst(struct Cyc_Absyn_Exp*e){return(void*)({struct Cyc_AssnDef_Const_AssnDef_Term_struct*_tmp1F=_cycalloc(sizeof(*_tmp1F));_tmp1F->tag=1,_tmp1F->f1=e;_tmp1F;});}
void*Cyc_AssnDef_zero (void){
static void*z=0;
if(z == 0){
struct Cyc_Absyn_Exp*_tmp20=Cyc_Absyn_int_exp(2U,0,0U);struct Cyc_Absyn_Exp*e=_tmp20;
({void*_tmp141=Cyc_Absyn_int_type(2U,2U);e->topt=_tmp141;});{
void*_tmp21=Cyc_AssnDef_cnst(e);void*r=_tmp21;
z=r;
return r;}}else{
return _check_null(z);}}
# 71
void*Cyc_AssnDef_one (void){
static void*n=0;
if(n == 0){
struct Cyc_Absyn_Exp*_tmp22=Cyc_Absyn_int_exp(2U,1,0U);struct Cyc_Absyn_Exp*e=_tmp22;
({void*_tmp142=Cyc_Absyn_int_type(2U,2U);e->topt=_tmp142;});{
void*_tmp23=Cyc_AssnDef_cnst(e);void*r=_tmp23;
n=r;
return r;}}else{
return _check_null(n);}}
# 82
void*Cyc_AssnDef_var(struct Cyc_Absyn_Vardecl*vd){return(void*)({struct Cyc_AssnDef_Var_AssnDef_Term_struct*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->tag=2,_tmp24->f1=vd;_tmp24;});}
# 84
inline static int Cyc_AssnDef_is_commutative(enum Cyc_Absyn_Primop p){
enum Cyc_Absyn_Primop _tmp25=p;switch((int)_tmp25){case Cyc_Absyn_Plus:
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
void*_tmp26=t;void*_tmp27;switch(*((int*)_tmp26)){case 0:
 return Cyc_Absyn_int_type(2U,2U);case 1: _tmp27=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp26)->f1;{struct Cyc_Absyn_Exp*e=_tmp27;
return e->topt;}case 2: _tmp27=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp26)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmp27;
return vd->type;}case 3: _tmp27=(void*)((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp26)->f3;{void*topt=_tmp27;
return topt;}case 4:
# 104
 return 0;default: _tmp27=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp26)->f1;{void*type=_tmp27;
return type;}};}
# 109
void*Cyc_AssnDef_primop(enum Cyc_Absyn_Primop p,struct Cyc_List_List*ts){
return(void*)({struct Cyc_AssnDef_Primop_AssnDef_Term_struct*_tmp28=_cycalloc(sizeof(*_tmp28));_tmp28->tag=4,_tmp28->f1=p,_tmp28->f2=ts;_tmp28;});}
# 112
void*Cyc_AssnDef_cast(void*tp,void*tm){
return(void*)({struct Cyc_AssnDef_Cast_AssnDef_Term_struct*_tmp29=_cycalloc(sizeof(*_tmp29));_tmp29->tag=5,_tmp29->f1=tp,_tmp29->f2=tm;_tmp29;});}
# 115
void*Cyc_AssnDef_fresh_var(void*t){
static int counter=0;
return(void*)({struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*_tmp2A=_cycalloc(sizeof(*_tmp2A));_tmp2A->tag=3,_tmp2A->f1=0,_tmp2A->f2=counter ++,_tmp2A->f3=t;_tmp2A;});}struct _tuple11{void*f1;void*f2;};
# 120
int Cyc_AssnDef_cmp_term(void*t1,void*t2){
struct _tuple11 _tmp2B=({struct _tuple11 _tmp10B;_tmp10B.f1=t1,_tmp10B.f2=t2;_tmp10B;});struct _tuple11 _stmttmp0=_tmp2B;struct _tuple11 _tmp2C=_stmttmp0;enum Cyc_Absyn_Primop _tmp2F;enum Cyc_Absyn_Primop _tmp2D;void*_tmp32;void*_tmp31;int _tmp34;int _tmp33;void*_tmp30;void*_tmp2E;unsigned _tmp36;unsigned _tmp35;if(*((int*)_tmp2C.f1)== 0){if(*((int*)_tmp2C.f2)== 0){_tmp35=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_tmp2C.f1)->f1;_tmp36=((struct Cyc_AssnDef_Uint_AssnDef_Term_struct*)_tmp2C.f2)->f1;{unsigned i1=_tmp35;unsigned i2=_tmp36;
return(int)(i1 - i2);}}else{
return -1;}}else{if(*((int*)_tmp2C.f2)== 0)
return 1;else{if(*((int*)_tmp2C.f1)== 1){if(*((int*)_tmp2C.f2)== 1){_tmp2E=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2C.f1)->f1;_tmp30=((struct Cyc_AssnDef_Const_AssnDef_Term_struct*)_tmp2C.f2)->f1;{struct Cyc_Absyn_Exp*e1=_tmp2E;struct Cyc_Absyn_Exp*e2=_tmp30;
return Cyc_Evexp_exp_cmp(e1,e2);}}else{
return -1;}}else{if(*((int*)_tmp2C.f2)== 1)
return 1;else{if(*((int*)_tmp2C.f1)== 2){if(*((int*)_tmp2C.f2)== 2){_tmp2E=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2C.f1)->f1;_tmp30=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmp2C.f2)->f1;{struct Cyc_Absyn_Vardecl*vd1=_tmp2E;struct Cyc_Absyn_Vardecl*vd2=_tmp30;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_tmp2C.f2)== 2)
return 1;else{if(*((int*)_tmp2C.f1)== 3){if(*((int*)_tmp2C.f2)== 3){_tmp2E=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2C.f1)->f1;_tmp33=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2C.f1)->f2;_tmp30=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2C.f2)->f1;_tmp34=((struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct*)_tmp2C.f2)->f2;{struct Cyc_Absyn_Vardecl*vd1=_tmp2E;int i=_tmp33;struct Cyc_Absyn_Vardecl*vd2=_tmp30;int j=_tmp34;
# 132
if(vd1 == vd2)return i - j;
return(int)vd1 - (int)vd2;}}else{
return -1;}}else{if(*((int*)_tmp2C.f2)== 3)
return 1;else{if(*((int*)_tmp2C.f1)== 5){if(*((int*)_tmp2C.f2)== 5){_tmp2E=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2C.f1)->f1;_tmp30=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2C.f1)->f2;_tmp31=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2C.f2)->f1;_tmp32=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmp2C.f2)->f2;{void*tp1=_tmp2E;void*tm1=_tmp30;void*tp2=_tmp31;void*tm2=_tmp32;
# 137
int c=Cyc_Tcutil_typecmp(tp1,tp2);
if(c != 0)return c;
return Cyc_AssnDef_cmp_term(tm1,tm2);}}else{
return -1;}}else{if(*((int*)_tmp2C.f2)== 5)
return 1;else{_tmp2D=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2C.f1)->f1;_tmp2E=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2C.f1)->f2;_tmp2F=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2C.f2)->f1;_tmp30=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmp2C.f2)->f2;{enum Cyc_Absyn_Primop p1=_tmp2D;struct Cyc_List_List*ts1=_tmp2E;enum Cyc_Absyn_Primop p2=_tmp2F;struct Cyc_List_List*ts2=_tmp30;
# 143
if((int)p1 != (int)p2)return(int)p1 - (int)p2;
return({(int(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_list_cmp;})(Cyc_AssnDef_cmp_term,ts1,ts2);}}}}}}}}}}};}
# 148
static int Cyc_AssnDef_same_term(void*t1,void*t2){return Cyc_AssnDef_cmp_term(t1,t2)== 0;}
# 167
static unsigned Cyc_AssnDef_assn_size_loop(void*a,unsigned c,struct Cyc_List_List**prev){
# 169
LOOP:
 if(({(int(*)(struct Cyc_List_List*,void*))Cyc_List_memq;})(*prev,a))return c;
({struct Cyc_List_List*_tmp143=({struct Cyc_List_List*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->hd=a,_tmp37->tl=*prev;_tmp37;});*prev=_tmp143;});{
void*_tmp38=a;void*_tmp3B;void*_tmp3A;void*_tmp39;switch(*((int*)_tmp38)){case 0:
 return c;case 1:
 return c;case 2:
 return 1U + c;case 3: _tmp39=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp38)->f1;_tmp3A=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp38)->f2;{void*a1=_tmp39;void*a2=_tmp3A;
_tmp39=a1;_tmp3A=a2;goto _LLA;}case 4: _tmp39=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp38)->f1;_tmp3A=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp38)->f2;_LLA: {void*a1=_tmp39;void*a2=_tmp3A;
# 178
c=Cyc_AssnDef_assn_size_loop(a2,1U + c,prev);
a=a1;
goto LOOP;}case 5: _tmp39=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp38)->f1;_tmp3A=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp38)->f2;_tmp3B=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp38)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp39;void*t=_tmp3A;void*a1=_tmp3B;
# 182
++ c;
a=a1;
goto LOOP;}default: _tmp39=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp38)->f1;{void*a1=_tmp39;
# 186
++ c;
a=a1;
goto LOOP;}};}}
# 195
static unsigned Cyc_AssnDef_max_assn_size=0U;
# 198
static unsigned Cyc_AssnDef_assn_size(void*a){
struct Cyc_List_List*prev=0;
unsigned _tmp3C=Cyc_AssnDef_assn_size_loop(a,0U,& prev);unsigned c=_tmp3C;
if(c > Cyc_AssnDef_max_assn_size)
Cyc_AssnDef_max_assn_size=c;
# 204
return c;}
# 209
static struct _fat_ptr Cyc_AssnDef_a2string(void*a,int prec,int*size){
void*_tmp3D=a;void*_tmp40;void*_tmp3F;enum Cyc_AssnDef_Primreln _tmp41;void*_tmp3E;switch(*((int*)_tmp3D)){case 0:
 return({const char*_tmp42="true";_tag_fat(_tmp42,sizeof(char),5U);});case 1:
 return({const char*_tmp43="false";_tag_fat(_tmp43,sizeof(char),6U);});case 2: _tmp3E=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp3D)->f1;_tmp41=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp3D)->f2;_tmp3F=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp3D)->f3;{void*t1=_tmp3E;enum Cyc_AssnDef_Primreln p=_tmp41;void*t2=_tmp3F;
# 214
*size +=1;{
struct _fat_ptr ps;
{enum Cyc_AssnDef_Primreln _tmp44=p;switch((int)_tmp44){case Cyc_AssnDef_Eq:
 ps=({const char*_tmp45="==";_tag_fat(_tmp45,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_Neq:
 ps=({const char*_tmp46="!=";_tag_fat(_tmp46,sizeof(char),3U);});goto _LLF;case Cyc_AssnDef_SLt:
 ps=({const char*_tmp47=" S< ";_tag_fat(_tmp47,sizeof(char),5U);});goto _LLF;case Cyc_AssnDef_SLte:
 ps=({const char*_tmp48=" S<= ";_tag_fat(_tmp48,sizeof(char),6U);});goto _LLF;case Cyc_AssnDef_ULt:
 ps=({const char*_tmp49=" U< ";_tag_fat(_tmp49,sizeof(char),5U);});goto _LLF;case Cyc_AssnDef_ULte:
 ps=({const char*_tmp4A=" U<= ";_tag_fat(_tmp4A,sizeof(char),6U);});goto _LLF;default:
({void*_tmp4B=0U;({int(*_tmp145)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp144=({const char*_tmp4C="assn2string primop";_tag_fat(_tmp4C,sizeof(char),19U);});_tmp145(_tmp144,_tag_fat(_tmp4B,sizeof(void*),0));});});}_LLF:;}
# 225
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp4F=({struct Cyc_String_pa_PrintArg_struct _tmp10E;_tmp10E.tag=0,({struct _fat_ptr _tmp146=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t1));_tmp10E.f1=_tmp146;});_tmp10E;});struct Cyc_String_pa_PrintArg_struct _tmp50=({struct Cyc_String_pa_PrintArg_struct _tmp10D;_tmp10D.tag=0,_tmp10D.f1=(struct _fat_ptr)((struct _fat_ptr)ps);_tmp10D;});struct Cyc_String_pa_PrintArg_struct _tmp51=({struct Cyc_String_pa_PrintArg_struct _tmp10C;_tmp10C.tag=0,({struct _fat_ptr _tmp147=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t2));_tmp10C.f1=_tmp147;});_tmp10C;});void*_tmp4D[3];_tmp4D[0]=& _tmp4F,_tmp4D[1]=& _tmp50,_tmp4D[2]=& _tmp51;({struct _fat_ptr _tmp148=({const char*_tmp4E="%s%s%s";_tag_fat(_tmp4E,sizeof(char),7U);});Cyc_aprintf(_tmp148,_tag_fat(_tmp4D,sizeof(void*),3));});});}}case 4: _tmp3E=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp3D)->f1;_tmp3F=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp3D)->f2;{void*a1=_tmp3E;void*a2=_tmp3F;
# 227
*size +=1;
if(prec <= 5)
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp110;_tmp110.tag=0,({struct _fat_ptr _tmp149=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_tmp110.f1=_tmp149;});_tmp110;});struct Cyc_String_pa_PrintArg_struct _tmp55=({struct Cyc_String_pa_PrintArg_struct _tmp10F;_tmp10F.tag=0,({struct _fat_ptr _tmp14A=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_tmp10F.f1=_tmp14A;});_tmp10F;});void*_tmp52[2];_tmp52[0]=& _tmp54,_tmp52[1]=& _tmp55;({struct _fat_ptr _tmp14B=({const char*_tmp53="%s || %s";_tag_fat(_tmp53,sizeof(char),9U);});Cyc_aprintf(_tmp14B,_tag_fat(_tmp52,sizeof(void*),2));});});else{
# 231
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp58=({struct Cyc_String_pa_PrintArg_struct _tmp112;_tmp112.tag=0,({struct _fat_ptr _tmp14C=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,5,size));_tmp112.f1=_tmp14C;});_tmp112;});struct Cyc_String_pa_PrintArg_struct _tmp59=({struct Cyc_String_pa_PrintArg_struct _tmp111;_tmp111.tag=0,({struct _fat_ptr _tmp14D=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,5,size));_tmp111.f1=_tmp14D;});_tmp111;});void*_tmp56[2];_tmp56[0]=& _tmp58,_tmp56[1]=& _tmp59;({struct _fat_ptr _tmp14E=({const char*_tmp57="(%s || %s)";_tag_fat(_tmp57,sizeof(char),11U);});Cyc_aprintf(_tmp14E,_tag_fat(_tmp56,sizeof(void*),2));});});}}case 3: _tmp3E=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp3D)->f1;_tmp3F=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp3D)->f2;{void*a1=_tmp3E;void*a2=_tmp3F;
# 233
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp114;_tmp114.tag=0,({struct _fat_ptr _tmp14F=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a1,10,size));_tmp114.f1=_tmp14F;});_tmp114;});struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp113;_tmp113.tag=0,({struct _fat_ptr _tmp150=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a2,10,size));_tmp113.f1=_tmp150;});_tmp113;});void*_tmp5A[2];_tmp5A[0]=& _tmp5C,_tmp5A[1]=& _tmp5D;({struct _fat_ptr _tmp151=({const char*_tmp5B="%s && %s";_tag_fat(_tmp5B,sizeof(char),9U);});Cyc_aprintf(_tmp151,_tag_fat(_tmp5A,sizeof(void*),2));});});}case 5: _tmp3E=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp3D)->f1;_tmp3F=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp3D)->f2;_tmp40=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp3D)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp3E;void*t=_tmp3F;void*a=_tmp40;
# 236
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp60=({struct Cyc_String_pa_PrintArg_struct _tmp117;_tmp117.tag=0,({struct _fat_ptr _tmp152=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_qvar2string(vd->name));_tmp117.f1=_tmp152;});_tmp117;});struct Cyc_String_pa_PrintArg_struct _tmp61=({struct Cyc_String_pa_PrintArg_struct _tmp116;_tmp116.tag=0,({
struct _fat_ptr _tmp153=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_term2string(t));_tmp116.f1=_tmp153;});_tmp116;});struct Cyc_String_pa_PrintArg_struct _tmp62=({struct Cyc_String_pa_PrintArg_struct _tmp115;_tmp115.tag=0,({struct _fat_ptr _tmp154=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_tmp115.f1=_tmp154;});_tmp115;});void*_tmp5E[3];_tmp5E[0]=& _tmp60,_tmp5E[1]=& _tmp61,_tmp5E[2]=& _tmp62;({struct _fat_ptr _tmp155=({const char*_tmp5F="Subst[%s:=%s](%s)";_tag_fat(_tmp5F,sizeof(char),18U);});Cyc_aprintf(_tmp155,_tag_fat(_tmp5E,sizeof(void*),3));});});}default: _tmp3E=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp3D)->f1;{void*a=_tmp3E;
# 240
*size +=1;
return(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp118;_tmp118.tag=0,({struct _fat_ptr _tmp156=(struct _fat_ptr)((struct _fat_ptr)Cyc_AssnDef_a2string(a,0,size));_tmp118.f1=_tmp156;});_tmp118;});void*_tmp63[1];_tmp63[0]=& _tmp65;({struct _fat_ptr _tmp157=({const char*_tmp64="Kill(%s)";_tag_fat(_tmp64,sizeof(char),9U);});Cyc_aprintf(_tmp157,_tag_fat(_tmp63,sizeof(void*),1));});});}};}
# 248
int Cyc_AssnDef_tree_size=0;
# 251
struct _fat_ptr Cyc_AssnDef_assn2string(void*a){
Cyc_AssnDef_tree_size=0;
return Cyc_AssnDef_a2string(a,0,& Cyc_AssnDef_tree_size);}
# 257
struct Cyc_AssnDef_True_AssnDef_Assn_struct Cyc_AssnDef_true_assn={0};
struct Cyc_AssnDef_False_AssnDef_Assn_struct Cyc_AssnDef_false_assn={1};
# 268 "assndef.cyc"
static int Cyc_AssnDef_assnhash(void*a){
void*_tmp66=a;void*_tmp69;void*_tmp68;enum Cyc_AssnDef_Primreln _tmp6A;void*_tmp67;switch(*((int*)_tmp66)){case 1:
 return 0;case 0:
 return 1;case 2: _tmp67=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp66)->f1;_tmp6A=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp66)->f2;_tmp68=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp66)->f3;{void*t1=_tmp67;enum Cyc_AssnDef_Primreln p=_tmp6A;void*t2=_tmp68;
# 273
return(int)(((unsigned)t1 ^ (unsigned)p)^ (unsigned)t2);}case 3: _tmp67=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp66)->f1;_tmp68=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp66)->f2;{void*a1=_tmp67;void*a2=_tmp68;
return(int)((unsigned)a1 ^ (unsigned)a2);}case 4: _tmp67=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp66)->f1;_tmp68=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp66)->f2;{void*a1=_tmp67;void*a2=_tmp68;
return(int)((unsigned)a1 ^ (unsigned)a2 | 1U);}case 5: _tmp67=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp66)->f1;_tmp68=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp66)->f2;_tmp69=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp66)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmp67;void*t=_tmp68;void*a=_tmp69;
return(int)(((unsigned)a ^ (unsigned)vd)^ (unsigned)t | 3U);}default: _tmp67=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp66)->f1;{void*a=_tmp67;
# 278
return(int)((unsigned)a + 37U);}};}
# 285
int Cyc_AssnDef_assncmp(void*a1,void*a2){
LOOP:
 if(a1 == a2)return 0;{
struct _tuple11 _tmp6B=({struct _tuple11 _tmp119;_tmp119.f1=a1,_tmp119.f2=a2;_tmp119;});struct _tuple11 _stmttmp1=_tmp6B;struct _tuple11 _tmp6C=_stmttmp1;void*_tmp72;void*_tmp71;enum Cyc_AssnDef_Primreln _tmp74;enum Cyc_AssnDef_Primreln _tmp73;void*_tmp70;void*_tmp6F;void*_tmp6E;void*_tmp6D;if(*((int*)_tmp6C.f1)== 0){if(*((int*)_tmp6C.f2)== 0)
return 0;else{
return -1;}}else{if(*((int*)_tmp6C.f2)== 0)
return 1;else{if(*((int*)_tmp6C.f1)== 1){if(*((int*)_tmp6C.f2)== 1)
return 0;else{
return -1;}}else{if(*((int*)_tmp6C.f2)== 1)
return 1;else{switch(*((int*)_tmp6C.f1)){case 4: if(*((int*)_tmp6C.f2)== 4){_tmp6D=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp6C.f1)->f1;_tmp6E=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp6C.f1)->f2;_tmp6F=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp6C.f2)->f1;_tmp70=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp6C.f2)->f2;{void*a11=_tmp6D;void*a12=_tmp6E;void*a21=_tmp6F;void*a22=_tmp70;
_tmp6D=a11;_tmp6E=a12;_tmp6F=a21;_tmp70=a22;goto _LL10;}}else{
# 302
return -1;}case 3: switch(*((int*)_tmp6C.f2)){case 3: _tmp6D=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp6C.f1)->f1;_tmp6E=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp6C.f1)->f2;_tmp6F=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp6C.f2)->f1;_tmp70=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp6C.f2)->f2;_LL10: {void*a11=_tmp6D;void*a12=_tmp6E;void*a21=_tmp6F;void*a22=_tmp70;
# 297
int c=Cyc_AssnDef_assncmp(a11,a21);
if(c != 0)return c;
a1=a12;
a2=a22;
goto LOOP;}case 4: goto _LL13;default:
# 304
 return -1;}default: switch(*((int*)_tmp6C.f2)){case 4: _LL13:
# 303
 return 1;case 3:
# 305
 return 1;default: if(*((int*)_tmp6C.f1)== 2){if(*((int*)_tmp6C.f2)== 2){_tmp6D=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6C.f1)->f1;_tmp73=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6C.f1)->f2;_tmp6E=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6C.f1)->f3;_tmp6F=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6C.f2)->f1;_tmp74=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6C.f2)->f2;_tmp70=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp6C.f2)->f3;{void*t11=_tmp6D;enum Cyc_AssnDef_Primreln p1=_tmp73;void*t12=_tmp6E;void*t21=_tmp6F;enum Cyc_AssnDef_Primreln p2=_tmp74;void*t22=_tmp70;
# 307
int c=(int)p1 - (int)p2;
if(c != 0)return c;
c=Cyc_AssnDef_cmp_term(t11,t21);
if(c != 0)return c;
return Cyc_AssnDef_cmp_term(t12,t22);}}else{
return -1;}}else{if(*((int*)_tmp6C.f2)== 2)
return 1;else{if(*((int*)_tmp6C.f1)== 6){if(*((int*)_tmp6C.f2)== 6){_tmp6D=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp6C.f1)->f1;_tmp6E=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp6C.f2)->f1;{void*a1=_tmp6D;void*a2=_tmp6E;
return Cyc_AssnDef_assncmp(a1,a2);}}else{
return -1;}}else{if(*((int*)_tmp6C.f2)== 6)
return 1;else{_tmp6D=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6C.f1)->f1;_tmp6E=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6C.f1)->f2;_tmp6F=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6C.f1)->f3;_tmp70=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6C.f2)->f1;_tmp71=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6C.f2)->f2;_tmp72=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp6C.f2)->f3;{struct Cyc_Absyn_Vardecl*vd1=_tmp6D;void*t1=_tmp6E;void*a11=_tmp6F;struct Cyc_Absyn_Vardecl*vd2=_tmp70;void*t2=_tmp71;void*a22=_tmp72;
# 318
int c=(int)vd1 - (int)vd2;
if(c != 0)return c;
c=Cyc_AssnDef_cmp_term(t1,t2);
if(c != 0)return c;
a1=a11;
a2=a22;
goto LOOP;}}}}}}}}}}};}}
# 334
static struct Cyc_Hashtable_Table*Cyc_AssnDef_hash_cons_table=0;
# 337
void Cyc_AssnDef_reset_hash_cons_table (void){
Cyc_AssnDef_hash_cons_table=0;}struct _tuple12{void*f1;int f2;};
# 342
int Cyc_AssnDef_is_reduced(void*a){
if(Cyc_AssnDef_hash_cons_table == 0)return 0;{
struct _tuple12**_tmp75=({(struct _tuple12**(*)(struct Cyc_Hashtable_Table*,void*))Cyc_Hashtable_lookup_opt;})(_check_null(Cyc_AssnDef_hash_cons_table),a);struct _tuple12**resopt=_tmp75;
if(resopt == 0)return 0;{
struct _tuple12*_tmp76=*resopt;struct _tuple12*_stmttmp2=_tmp76;struct _tuple12*_tmp77=_stmttmp2;int _tmp78;_tmp78=_tmp77->f2;{int b=_tmp78;
return b;}}}}
# 353
static struct _tuple12 Cyc_AssnDef_copy_assn(void*a){
void*_tmp79=a;void*_tmp7C;enum Cyc_AssnDef_Primreln _tmp7D;void*_tmp7B;void*_tmp7A;switch(*((int*)_tmp79)){case 0:
 return({struct _tuple12 _tmp11A;_tmp11A.f1=(void*)& Cyc_AssnDef_true_assn,_tmp11A.f2=1;_tmp11A;});case 1:
 return({struct _tuple12 _tmp11B;_tmp11B.f1=(void*)& Cyc_AssnDef_false_assn,_tmp11B.f2=0;_tmp11B;});case 3: _tmp7A=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp79)->f1;_tmp7B=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp79)->f2;{void*a1=_tmp7A;void*a2=_tmp7B;
# 358
return({struct _tuple12 _tmp11C;({void*_tmp159=(void*)({struct Cyc_AssnDef_And_AssnDef_Assn_struct*_tmp7E=_cycalloc(sizeof(*_tmp7E));_tmp7E->tag=3,_tmp7E->f1=a1,_tmp7E->f2=a2;_tmp7E;});_tmp11C.f1=_tmp159;}),({int _tmp158=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_tmp11C.f2=_tmp158;});_tmp11C;});}case 4: _tmp7A=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp79)->f1;_tmp7B=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp79)->f2;{void*a1=_tmp7A;void*a2=_tmp7B;
# 360
return({struct _tuple12 _tmp11D;({void*_tmp15B=(void*)({struct Cyc_AssnDef_Or_AssnDef_Assn_struct*_tmp7F=_cycalloc(sizeof(*_tmp7F));_tmp7F->tag=4,_tmp7F->f1=a1,_tmp7F->f2=a2;_tmp7F;});_tmp11D.f1=_tmp15B;}),({int _tmp15A=Cyc_AssnDef_is_reduced(a1)&& Cyc_AssnDef_is_reduced(a2);_tmp11D.f2=_tmp15A;});_tmp11D;});}case 2: _tmp7A=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp79)->f1;_tmp7D=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp79)->f2;_tmp7B=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp79)->f3;{void*t1=_tmp7A;enum Cyc_AssnDef_Primreln p=_tmp7D;void*t2=_tmp7B;
return({struct _tuple12 _tmp11E;({void*_tmp15C=(void*)({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=2,_tmp80->f1=t1,_tmp80->f2=p,_tmp80->f3=t2;_tmp80;});_tmp11E.f1=_tmp15C;}),_tmp11E.f2=1;_tmp11E;});}case 5: _tmp7A=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp79)->f1;_tmp7B=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp79)->f2;_tmp7C=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmp79)->f3;{struct Cyc_Absyn_Vardecl*vd1=_tmp7A;void*t1=_tmp7B;void*a1=_tmp7C;
return({struct _tuple12 _tmp11F;({void*_tmp15D=(void*)({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=5,_tmp81->f1=vd1,_tmp81->f2=t1,_tmp81->f3=a1;_tmp81;});_tmp11F.f1=_tmp15D;}),_tmp11F.f2=0;_tmp11F;});}default: _tmp7A=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmp79)->f1;{void*a=_tmp7A;
return({struct _tuple12 _tmp120;({void*_tmp15E=(void*)({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=6,_tmp82->f1=a;_tmp82;});_tmp120.f1=_tmp15E;}),_tmp120.f2=0;_tmp120;});}};}
# 371
static void*Cyc_AssnDef_hash_cons(void*a){
struct Cyc_Hashtable_Table*h;
if(Cyc_AssnDef_hash_cons_table == 0){
h=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_create;})(123,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);
Cyc_AssnDef_hash_cons_table=h;
# 378
({void(*_tmp160)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*)=({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp15F=h;_tmp160(_tmp15F,(void*)& Cyc_AssnDef_true_assn,({struct _tuple12*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->f1=(void*)& Cyc_AssnDef_true_assn,_tmp83->f2=1;_tmp83;}));});
({void(*_tmp162)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*)=({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp161=h;_tmp162(_tmp161,(void*)& Cyc_AssnDef_false_assn,({struct _tuple12*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->f1=(void*)& Cyc_AssnDef_false_assn,_tmp84->f2=1;_tmp84;}));});}else{
# 381
h=_check_null(Cyc_AssnDef_hash_cons_table);}{
struct _tuple12**_tmp85=({(struct _tuple12**(*)(struct Cyc_Hashtable_Table*,void*,int(*)(void*,void*),int(*)(void*)))Cyc_Hashtable_lookup_other_opt;})(h,a,Cyc_AssnDef_assncmp,Cyc_AssnDef_assnhash);struct _tuple12**resopt=_tmp85;
if(resopt == 0){
struct _tuple12 _tmp86=Cyc_AssnDef_copy_assn(a);struct _tuple12 _stmttmp3=_tmp86;struct _tuple12 _tmp87=_stmttmp3;int _tmp89;void*_tmp88;_tmp88=_tmp87.f1;_tmp89=_tmp87.f2;{void*res=_tmp88;int reduced=_tmp89;
({void(*_tmp165)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*)=({(void(*)(struct Cyc_Hashtable_Table*,void*,struct _tuple12*))Cyc_Hashtable_insert;});struct Cyc_Hashtable_Table*_tmp164=h;void*_tmp163=res;_tmp165(_tmp164,_tmp163,({struct _tuple12*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->f1=res,_tmp8A->f2=reduced;_tmp8A;}));});
return res;}}else{
# 388
struct _tuple12*_tmp8B=*resopt;struct _tuple12*_stmttmp4=_tmp8B;struct _tuple12*_tmp8C=_stmttmp4;void*_tmp8D;_tmp8D=_tmp8C->f1;{void*res=_tmp8D;
return res;}}}}
# 394
void*Cyc_AssnDef_kill(void*a){
void*_tmp8E=a;switch(*((int*)_tmp8E)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 6: _LL6:
 return a;default:  {
# 400
struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _tmp8F=({struct Cyc_AssnDef_Kill_AssnDef_Assn_struct _tmp121;_tmp121.tag=6,_tmp121.f1=a;_tmp121;});struct Cyc_AssnDef_Kill_AssnDef_Assn_struct p=_tmp8F;
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 406
void*Cyc_AssnDef_and(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmp90=({struct _tuple11 _tmp123;_tmp123.f1=a1,_tmp123.f2=a2;_tmp123;});struct _tuple11 _stmttmp5=_tmp90;struct _tuple11 _tmp91=_stmttmp5;if(*((int*)_tmp91.f1)== 0)
return a2;else{if(*((int*)_tmp91.f2)== 0)
return a1;else{if(*((int*)_tmp91.f1)== 1)
return a1;else{if(*((int*)_tmp91.f2)== 1)
return a2;else{
# 417
if((int)a1 > (int)a2)
({void*_tmp92=a1;void*_tmp93=a2;a1=_tmp93;a2=_tmp92;});{
struct Cyc_AssnDef_And_AssnDef_Assn_struct _tmp94=({struct Cyc_AssnDef_And_AssnDef_Assn_struct _tmp122;_tmp122.tag=3,_tmp122.f1=a1,_tmp122.f2=a2;_tmp122;});struct Cyc_AssnDef_And_AssnDef_Assn_struct a=_tmp94;
return Cyc_AssnDef_hash_cons((void*)& a);}}}}};}}
# 425
void*Cyc_AssnDef_or(void*a1,void*a2){
if(a1 == a2)return a1;{
struct _tuple11 _tmp95=({struct _tuple11 _tmp125;_tmp125.f1=a1,_tmp125.f2=a2;_tmp125;});struct _tuple11 _stmttmp6=_tmp95;struct _tuple11 _tmp96=_stmttmp6;if(*((int*)_tmp96.f1)== 0)
return a1;else{if(*((int*)_tmp96.f2)== 0)
return a2;else{if(*((int*)_tmp96.f1)== 1)
return a2;else{if(*((int*)_tmp96.f2)== 1)
return a1;else{
# 434
if((int)a1 > (int)a2)
({void*_tmp97=a1;void*_tmp98=a2;a1=_tmp98;a2=_tmp97;});{
struct Cyc_AssnDef_Or_AssnDef_Assn_struct _tmp99=({struct Cyc_AssnDef_Or_AssnDef_Assn_struct _tmp124;_tmp124.tag=4,_tmp124.f1=a1,_tmp124.f2=a2;_tmp124;});struct Cyc_AssnDef_Or_AssnDef_Assn_struct a=_tmp99;
void*_tmp9A=Cyc_AssnDef_hash_cons((void*)& a);void*res=_tmp9A;
return res;}}}}};}}
# 443
void*Cyc_AssnDef_subst(struct Cyc_Absyn_Vardecl*x,void*newx,void*a){
void*_tmp9B=a;switch(*((int*)_tmp9B)){case 0:
 goto _LL4;case 1: _LL4:
 return a;default:  {
# 448
struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _tmp9C=({struct Cyc_AssnDef_Subst_AssnDef_Assn_struct _tmp126;_tmp126.tag=5,_tmp126.f1=x,_tmp126.f2=newx,_tmp126.f3=a;_tmp126;});struct Cyc_AssnDef_Subst_AssnDef_Assn_struct p=_tmp9C;
return Cyc_AssnDef_hash_cons((void*)& p);}};}
# 454
void*Cyc_AssnDef_prim(void*t1,enum Cyc_AssnDef_Primreln p,void*t2){
struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _tmp9D=({struct Cyc_AssnDef_Prim_AssnDef_Assn_struct _tmp127;_tmp127.tag=2,_tmp127.f1=t1,_tmp127.f2=p,_tmp127.f3=t2;_tmp127;});struct Cyc_AssnDef_Prim_AssnDef_Assn_struct ptr=_tmp9D;
return Cyc_AssnDef_hash_cons((void*)& ptr);}
# 464
void*Cyc_AssnDef_eq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,0U,t2);}
void*Cyc_AssnDef_neq(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,1U,t2);}
void*Cyc_AssnDef_slt(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,2U,t2);}
void*Cyc_AssnDef_slte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,3U,t2);}
void*Cyc_AssnDef_ult(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,4U,t2);}
void*Cyc_AssnDef_ulte(void*t1,void*t2){if(t1 == 0 || t2 == 0)return(void*)& Cyc_AssnDef_true_assn;return Cyc_AssnDef_prim(t1,5U,t2);}
# 481 "assndef.cyc"
void*Cyc_AssnDef_not(void*a){
void*_tmp9E=a;enum Cyc_AssnDef_Primreln _tmpA0;void*_tmpA1;void*_tmp9F;switch(*((int*)_tmp9E)){case 0:
 return(void*)& Cyc_AssnDef_false_assn;case 1:
 return(void*)& Cyc_AssnDef_true_assn;case 3: _tmp9F=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp9E)->f1;_tmpA1=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmp9E)->f2;{void*a1=_tmp9F;void*a2=_tmpA1;
return({void*_tmp166=Cyc_AssnDef_not(a1);Cyc_AssnDef_or(_tmp166,Cyc_AssnDef_not(a2));});}case 4: _tmp9F=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp9E)->f1;_tmpA1=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmp9E)->f2;{void*a1=_tmp9F;void*a2=_tmpA1;
return({void*_tmp167=Cyc_AssnDef_not(a1);Cyc_AssnDef_and(_tmp167,Cyc_AssnDef_not(a2));});}case 2: _tmp9F=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp9E)->f1;_tmpA0=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp9E)->f2;_tmpA1=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmp9E)->f3;{void*t1=_tmp9F;enum Cyc_AssnDef_Primreln p=_tmpA0;void*t2=_tmpA1;
# 488
enum Cyc_AssnDef_Primreln _tmpA2=p;switch((int)_tmpA2){case Cyc_AssnDef_Eq:
 return Cyc_AssnDef_neq(t1,t2);case Cyc_AssnDef_Neq:
 return Cyc_AssnDef_eq(t1,t2);case Cyc_AssnDef_SLt:
 return Cyc_AssnDef_slte(t2,t1);case Cyc_AssnDef_SLte:
 return Cyc_AssnDef_slt(t2,t1);case Cyc_AssnDef_ULt:
 return Cyc_AssnDef_ulte(t2,t1);case Cyc_AssnDef_ULte:
 return Cyc_AssnDef_ult(t2,t1);default:
({void*_tmpA3=0U;({int(*_tmp169)(struct _fat_ptr,struct _fat_ptr)=({(int(*)(struct _fat_ptr,struct _fat_ptr))Cyc_Warn_impos;});struct _fat_ptr _tmp168=({const char*_tmpA4="Vcgen::invert: non-primop";_tag_fat(_tmpA4,sizeof(char),26U);});_tmp169(_tmp168,_tag_fat(_tmpA3,sizeof(void*),0));});});};}case 5:
# 497
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));default:
 return Cyc_AssnDef_not(Cyc_AssnDef_reduce(a));};}struct _tuple13{struct Cyc_Absyn_Vardecl*f1;void*f2;};
# 520 "assndef.cyc"
static void*Cyc_AssnDef_reduce_term(void*t,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 523
void*_tmpA5=t;enum Cyc_Absyn_Primop _tmpA7;void*_tmpA8;void*_tmpA6;switch(*((int*)_tmpA5)){case 0:
 goto _LL4;case 1: _LL4:
 goto _LL6;case 3: _LL6:
 return t;case 5: _tmpA6=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpA5)->f1;_tmpA8=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpA5)->f2;{void*tp=_tmpA6;void*tm=_tmpA8;
# 528
void*_tmpA9=Cyc_AssnDef_reduce_term(tm,non_esc_subst,esc_subst);void*tm2=_tmpA9;
if(tm != tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 4: _tmpA7=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpA5)->f1;_tmpA6=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpA5)->f2;{enum Cyc_Absyn_Primop p=_tmpA7;struct Cyc_List_List*ts=_tmpA6;
# 532
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpAA=ts;struct Cyc_List_List*ptr=_tmpAA;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpAB=(void*)ptr->hd;void*old_term=_tmpAB;
void*_tmpAC=Cyc_AssnDef_reduce_term(old_term,non_esc_subst,esc_subst);void*new_term=_tmpAC;
res=({struct Cyc_List_List*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->hd=new_term,_tmpAD->tl=res;_tmpAD;});
if(new_term != old_term)change=1;}}
# 540
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp16A=p;Cyc_AssnDef_primop(_tmp16A,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res));});}default: _tmpA6=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmpA5)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpA6;
# 543
if(vd->escapes && esc_subst != 0){
# 546
struct Cyc_List_List*_tmpAE=*esc_subst;struct Cyc_List_List*s=_tmpAE;
for(1;s != 0;s=s->tl){
struct _tuple13 _tmpAF=*((struct _tuple13*)s->hd);struct _tuple13 _stmttmp7=_tmpAF;struct _tuple13 _tmpB0=_stmttmp7;void*_tmpB2;void*_tmpB1;_tmpB1=_tmpB0.f1;_tmpB2=_tmpB0.f2;{struct Cyc_Absyn_Vardecl*vd2=_tmpB1;void*t2=_tmpB2;
# 550
if(vd2 == vd)return t2;}}{
# 554
void*_tmpB3=Cyc_AssnDef_fresh_var(vd->type);void*t=_tmpB3;
({struct Cyc_List_List*_tmp16C=({struct Cyc_List_List*_tmpB5=_cycalloc(sizeof(*_tmpB5));({struct _tuple13*_tmp16B=({struct _tuple13*_tmpB4=_cycalloc(sizeof(*_tmpB4));_tmpB4->f1=vd,_tmpB4->f2=t;_tmpB4;});_tmpB5->hd=_tmp16B;}),_tmpB5->tl=*esc_subst;_tmpB5;});*esc_subst=_tmp16C;});
# 559
return t;}}else{
if(!vd->escapes)
# 562
for(1;non_esc_subst != 0;non_esc_subst=non_esc_subst->tl){
struct _tuple13 _tmpB6=*((struct _tuple13*)non_esc_subst->hd);struct _tuple13 _stmttmp8=_tmpB6;struct _tuple13 _tmpB7=_stmttmp8;void*_tmpB9;void*_tmpB8;_tmpB8=_tmpB7.f1;_tmpB9=_tmpB7.f2;{struct Cyc_Absyn_Vardecl*vd2=_tmpB8;void*t2=_tmpB9;
if(vd == vd2)return t2;}}}
# 567
return t;}};}
# 571
static void*Cyc_AssnDef_reduce_assn(void*a,struct Cyc_List_List*non_esc_subst,struct Cyc_List_List**esc_subst){
# 574
LOOP: {
void*_tmpBA=a;void*_tmpBD;void*_tmpBC;enum Cyc_AssnDef_Primreln _tmpBE;void*_tmpBB;switch(*((int*)_tmpBA)){case 0:
 goto _LL4;case 1: _LL4:
 return a;case 2: _tmpBB=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpBA)->f1;_tmpBE=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpBA)->f2;_tmpBC=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpBA)->f3;{void*t1=_tmpBB;enum Cyc_AssnDef_Primreln p=_tmpBE;void*t2=_tmpBC;
# 579
void*_tmpBF=Cyc_AssnDef_reduce_term(t1,non_esc_subst,esc_subst);void*newt1=_tmpBF;
void*_tmpC0=Cyc_AssnDef_reduce_term(t2,non_esc_subst,esc_subst);void*newt2=_tmpC0;
if(t1 == newt1 && t2 == newt2)return a;
return Cyc_AssnDef_prim(newt1,p,newt2);}case 3: _tmpBB=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpBA)->f1;_tmpBC=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpBA)->f2;{void*a1=_tmpBB;void*a2=_tmpBC;
# 584
void*_tmpC1=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpC1;
void*_tmpC2=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpC2;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_AssnDef_and(newa1,newa2);}case 4: _tmpBB=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpBA)->f1;_tmpBC=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpBA)->f2;{void*a1=_tmpBB;void*a2=_tmpBC;
# 589
void*_tmpC3=Cyc_AssnDef_reduce_assn(a1,non_esc_subst,esc_subst);void*newa1=_tmpC3;
void*_tmpC4=Cyc_AssnDef_reduce_assn(a2,non_esc_subst,esc_subst);void*newa2=_tmpC4;
if(a1 == newa1 && a2 == newa2)return a;
return Cyc_AssnDef_or(newa1,newa2);}case 6: _tmpBB=(void*)((struct Cyc_AssnDef_Kill_AssnDef_Assn_struct*)_tmpBA)->f1;{void*a=_tmpBB;
# 596
struct Cyc_List_List*new_esc_subst=0;
return Cyc_AssnDef_reduce_assn(a,non_esc_subst,& new_esc_subst);}default: _tmpBB=((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpBA)->f1;_tmpBC=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpBA)->f2;_tmpBD=(void*)((struct Cyc_AssnDef_Subst_AssnDef_Assn_struct*)_tmpBA)->f3;{struct Cyc_Absyn_Vardecl*vd=_tmpBB;void*t=_tmpBC;void*a1=_tmpBD;
# 600
t=Cyc_AssnDef_reduce_term(t,non_esc_subst,esc_subst);
non_esc_subst=({struct Cyc_List_List*_tmpC6=_cycalloc(sizeof(*_tmpC6));({struct _tuple13*_tmp16D=({struct _tuple13*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->f1=vd,_tmpC5->f2=t;_tmpC5;});_tmpC6->hd=_tmp16D;}),_tmpC6->tl=non_esc_subst;_tmpC6;});
a=a1;
goto LOOP;}};}}
# 608
void*Cyc_AssnDef_reduce(void*a){
# 610
if(Cyc_AssnDef_is_reduced(a))return a;
return Cyc_AssnDef_reduce_assn(a,0,0);}
# 615
void*Cyc_AssnDef_subst_term(void*t,struct Cyc_Absyn_Vardecl*x,void*newx){
void*_tmpC7=t;void*_tmpC9;enum Cyc_Absyn_Primop _tmpCA;void*_tmpC8;switch(*((int*)_tmpC7)){case 2: _tmpC8=((struct Cyc_AssnDef_Var_AssnDef_Term_struct*)_tmpC7)->f1;{struct Cyc_Absyn_Vardecl*vd=_tmpC8;
# 618
if(vd == x)return newx;
return t;}case 4: _tmpCA=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpC7)->f1;_tmpC8=((struct Cyc_AssnDef_Primop_AssnDef_Term_struct*)_tmpC7)->f2;{enum Cyc_Absyn_Primop p=_tmpCA;struct Cyc_List_List*ts=_tmpC8;
# 621
struct Cyc_List_List*res=0;
int change=0;
{struct Cyc_List_List*_tmpCB=ts;struct Cyc_List_List*ptr=_tmpCB;for(0;ptr != 0;ptr=ptr->tl){
void*_tmpCC=(void*)ptr->hd;void*old_term=_tmpCC;
void*_tmpCD=Cyc_AssnDef_subst_term(old_term,x,newx);void*new_term=_tmpCD;
res=({struct Cyc_List_List*_tmpCE=_cycalloc(sizeof(*_tmpCE));_tmpCE->hd=new_term,_tmpCE->tl=res;_tmpCE;});
if(new_term != old_term)change=1;}}
# 629
if(!change)return t;
return({enum Cyc_Absyn_Primop _tmp16E=p;Cyc_AssnDef_primop(_tmp16E,({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res));});}case 5: _tmpC8=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpC7)->f1;_tmpC9=(void*)((struct Cyc_AssnDef_Cast_AssnDef_Term_struct*)_tmpC7)->f2;{void*tp=_tmpC8;void*tm=_tmpC9;
# 632
void*_tmpCF=Cyc_AssnDef_subst_term(tm,x,newx);void*tm2=_tmpCF;
if(tm != tm2)return Cyc_AssnDef_cast(tp,tm2);else{
return t;}}case 0:
 goto _LLA;case 1: _LLA:
 goto _LLC;default: _LLC:
 return t;};}
# 642
void*Cyc_AssnDef_kill_mem_term(void*t){
struct Cyc_List_List*_tmpD0=0;struct Cyc_List_List*esc_subst=_tmpD0;
return Cyc_AssnDef_reduce_term(t,0,& esc_subst);}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 657 "assndef.cyc"
static void Cyc_AssnDef_intersect(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s,struct Cyc_List_List**common,struct Cyc_List_List**res1,struct Cyc_List_List**res2){
# 661
while(1){
struct _tuple14 _tmpD1=({struct _tuple14 _tmp128;_tmp128.f1=a1s,_tmp128.f2=a2s;_tmp128;});struct _tuple14 _stmttmp9=_tmpD1;struct _tuple14 _tmpD2=_stmttmp9;if(_tmpD2.f1 == 0){if(_tmpD2.f2 == 0)
return;else{
# 665
({struct Cyc_List_List*_tmp16F=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_revappend;})(a2s,*res2);*res2=_tmp16F;});return;}}else{if(_tmpD2.f2 == 0){
# 664
({struct Cyc_List_List*_tmp170=({(struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_revappend;})(a1s,*res1);*res1=_tmp170;});return;}else{
# 667
int c=({void*_tmp171=(void*)(_check_null(a1s))->hd;Cyc_AssnDef_assncmp(_tmp171,(void*)(_check_null(a2s))->hd);});
if(c == 0){
({struct Cyc_List_List*_tmp172=({struct Cyc_List_List*_tmpD3=_cycalloc(sizeof(*_tmpD3));_tmpD3->hd=(void*)a1s->hd,_tmpD3->tl=*common;_tmpD3;});*common=_tmp172;});
a1s=a1s->tl;
a2s=a2s->tl;}else{
if(c < 0){
({struct Cyc_List_List*_tmp173=({struct Cyc_List_List*_tmpD4=_cycalloc(sizeof(*_tmpD4));_tmpD4->hd=(void*)a1s->hd,_tmpD4->tl=*res1;_tmpD4;});*res1=_tmp173;});
a1s=a1s->tl;}else{
# 676
({struct Cyc_List_List*_tmp174=({struct Cyc_List_List*_tmpD5=_cycalloc(sizeof(*_tmpD5));_tmpD5->hd=(void*)a2s->hd,_tmpD5->tl=*res2;_tmpD5;});*res2=_tmp174;});
a2s=a2s->tl;}}}};}}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 683
struct _tuple15 Cyc_AssnDef_intersect_assns(struct Cyc_List_List*a1s,struct Cyc_List_List*a2s){
# 685
struct Cyc_List_List*_tmpD6=0;struct Cyc_List_List*common=_tmpD6;
struct Cyc_List_List*_tmpD7=0;struct Cyc_List_List*res1=_tmpD7;
struct Cyc_List_List*_tmpD8=0;struct Cyc_List_List*res2=_tmpD8;
Cyc_AssnDef_intersect(a1s,a2s,& common,& res1,& res2);
return({struct _tuple15 _tmp129;({struct Cyc_List_List*_tmp177=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(common);_tmp129.f1=_tmp177;}),({struct Cyc_List_List*_tmp176=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res1);_tmp129.f2=_tmp176;}),({struct Cyc_List_List*_tmp175=({(struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev;})(res2);_tmp129.f3=_tmp175;});_tmp129;});}struct _tuple16{struct Cyc_List_List*f1;void*f2;};
# 694
struct _tuple16 Cyc_AssnDef_factor(void*a){
void*_tmpD9=a;void*_tmpDB;void*_tmpDA;switch(*((int*)_tmpD9)){case 0:
 return({struct _tuple16 _tmp12A;_tmp12A.f1=0,_tmp12A.f2=(void*)& Cyc_AssnDef_true_assn;_tmp12A;});case 1:
 return({struct _tuple16 _tmp12B;_tmp12B.f1=0,_tmp12B.f2=(void*)& Cyc_AssnDef_false_assn;_tmp12B;});case 2:
 return({struct _tuple16 _tmp12C;({struct Cyc_List_List*_tmp178=({struct Cyc_List_List*_tmpDC=_cycalloc(sizeof(*_tmpDC));_tmpDC->hd=a,_tmpDC->tl=0;_tmpDC;});_tmp12C.f1=_tmp178;}),_tmp12C.f2=(void*)& Cyc_AssnDef_true_assn;_tmp12C;});case 3: _tmpDA=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpD9)->f1;_tmpDB=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpD9)->f2;{void*a1=_tmpDA;void*a2=_tmpDB;
# 700
struct _tuple16 _tmpDD=Cyc_AssnDef_factor(a1);struct _tuple16 _stmttmpA=_tmpDD;struct _tuple16 _tmpDE=_stmttmpA;void*_tmpE0;void*_tmpDF;_tmpDF=_tmpDE.f1;_tmpE0=_tmpDE.f2;{struct Cyc_List_List*c1s=_tmpDF;void*a1=_tmpE0;
struct _tuple16 _tmpE1=Cyc_AssnDef_factor(a2);struct _tuple16 _stmttmpB=_tmpE1;struct _tuple16 _tmpE2=_stmttmpB;void*_tmpE4;void*_tmpE3;_tmpE3=_tmpE2.f1;_tmpE4=_tmpE2.f2;{struct Cyc_List_List*c2s=_tmpE3;void*a2=_tmpE4;
return({struct _tuple16 _tmp12D;({struct Cyc_List_List*_tmp17A=({(struct Cyc_List_List*(*)(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_merge;})(Cyc_AssnDef_assncmp,c1s,c2s);_tmp12D.f1=_tmp17A;}),({void*_tmp179=Cyc_AssnDef_and(a1,a2);_tmp12D.f2=_tmp179;});_tmp12D;});}}}case 4: _tmpDA=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpD9)->f1;_tmpDB=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpD9)->f2;{void*a1=_tmpDA;void*a2=_tmpDB;
# 704
struct _tuple16 _tmpE5=Cyc_AssnDef_factor(a1);struct _tuple16 _stmttmpC=_tmpE5;struct _tuple16 _tmpE6=_stmttmpC;void*_tmpE8;void*_tmpE7;_tmpE7=_tmpE6.f1;_tmpE8=_tmpE6.f2;{struct Cyc_List_List*c1s=_tmpE7;void*a1=_tmpE8;
struct _tuple16 _tmpE9=Cyc_AssnDef_factor(a2);struct _tuple16 _stmttmpD=_tmpE9;struct _tuple16 _tmpEA=_stmttmpD;void*_tmpEC;void*_tmpEB;_tmpEB=_tmpEA.f1;_tmpEC=_tmpEA.f2;{struct Cyc_List_List*c2s=_tmpEB;void*a2=_tmpEC;
struct _tuple15 _tmpED=Cyc_AssnDef_intersect_assns(c1s,c2s);struct _tuple15 _stmttmpE=_tmpED;struct _tuple15 _tmpEE=_stmttmpE;void*_tmpF1;void*_tmpF0;void*_tmpEF;_tmpEF=_tmpEE.f1;_tmpF0=_tmpEE.f2;_tmpF1=_tmpEE.f3;{struct Cyc_List_List*cs=_tmpEF;struct Cyc_List_List*c1s=_tmpF0;struct Cyc_List_List*c2s=_tmpF1;
return({struct _tuple16 _tmp12E;_tmp12E.f1=cs,({void*_tmp17C=({void*_tmp17B=({(void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left;})(Cyc_AssnDef_and,a1,c1s);Cyc_AssnDef_or(_tmp17B,({(void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left;})(Cyc_AssnDef_and,a2,c2s));});_tmp12E.f2=_tmp17C;});_tmp12E;});}}}}default:
 return({struct _tuple16 _tmp12F;_tmp12F.f1=0,_tmp12F.f2=a;_tmp12F;});};}
# 712
void*Cyc_AssnDef_factor_assn(void*a){
# 714
struct _tuple16 _tmpF2=Cyc_AssnDef_factor(a);struct _tuple16 _stmttmpF=_tmpF2;struct _tuple16 _tmpF3=_stmttmpF;void*_tmpF5;void*_tmpF4;_tmpF4=_tmpF3.f1;_tmpF5=_tmpF3.f2;{struct Cyc_List_List*cs=_tmpF4;void*a=_tmpF5;
a=({(void*(*)(void*(*)(void*,void*),void*,struct Cyc_List_List*))Cyc_List_fold_left;})(Cyc_AssnDef_and,a,cs);
# 719
return a;}}struct _tuple17{enum Cyc_AssnDef_Primreln f1;enum Cyc_AssnDef_Primreln f2;};
# 726
int Cyc_AssnDef_find_ctxt(void*ctxt,void*t1,enum Cyc_AssnDef_Primreln p1,void*u1){
LOOP: {
void*_tmpF6=ctxt;enum Cyc_AssnDef_Primreln _tmpF8;void*_tmpF9;void*_tmpF7;switch(*((int*)_tmpF6)){case 1:
 return 1;case 0:
 return 0;case 3: _tmpF7=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpF6)->f1;_tmpF9=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpF6)->f2;{void*a1=_tmpF7;void*a2=_tmpF9;
# 732
if(Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 1;
ctxt=a2;
goto LOOP;}case 4: _tmpF7=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpF6)->f1;_tmpF9=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpF6)->f2;{void*a1=_tmpF7;void*a2=_tmpF9;
# 736
if(!Cyc_AssnDef_find_ctxt(a1,t1,p1,u1))return 0;
ctxt=a2;
goto LOOP;}case 2: _tmpF7=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF6)->f1;_tmpF8=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF6)->f2;_tmpF9=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpF6)->f3;{void*t2=_tmpF7;enum Cyc_AssnDef_Primreln p2=_tmpF8;void*u2=_tmpF9;
# 740
struct _tuple17 _tmpFA=({struct _tuple17 _tmp130;_tmp130.f1=p2,_tmp130.f2=p1;_tmp130;});struct _tuple17 _stmttmp10=_tmpFA;struct _tuple17 _tmpFB=_stmttmp10;switch((int)_tmpFB.f1){case Cyc_AssnDef_Neq: if(_tmpFB.f2 == Cyc_AssnDef_Neq)
goto _LL11;else{goto _LL26;}case Cyc_AssnDef_Eq: switch((int)_tmpFB.f2){case Cyc_AssnDef_Eq: _LL11:
 goto _LL13;case Cyc_AssnDef_SLte: _LL13:
 goto _LL15;case Cyc_AssnDef_ULte: _LL15:
# 745
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);default: goto _LL26;}case Cyc_AssnDef_SLt: switch((int)_tmpFB.f2){case Cyc_AssnDef_Neq:
 goto _LL19;case Cyc_AssnDef_SLt:
# 751
 goto _LL1D;case Cyc_AssnDef_SLte: _LL1F:
# 753
 goto _LL21;default: goto _LL26;}case Cyc_AssnDef_ULt: switch((int)_tmpFB.f2){case Cyc_AssnDef_Neq: _LL19:
# 749
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2)||
 Cyc_AssnDef_same_term(t1,u2)&& Cyc_AssnDef_same_term(u1,t2);case Cyc_AssnDef_ULt: _LL1D:
# 752
 goto _LL1F;case Cyc_AssnDef_ULte: _LL21:
# 754
 goto _LL23;default: goto _LL26;}case Cyc_AssnDef_SLte: if(_tmpFB.f2 == Cyc_AssnDef_SLte){_LL23:
 goto _LL25;}else{goto _LL26;}case Cyc_AssnDef_ULte: if(_tmpFB.f2 == Cyc_AssnDef_ULte){_LL25:
# 757
 return Cyc_AssnDef_same_term(t1,t2)&& Cyc_AssnDef_same_term(u1,u2);}else{goto _LL26;}default: _LL26:
 return 0;};}default:
# 761
 ctxt=Cyc_AssnDef_reduce(ctxt);
goto LOOP;};}}
# 768
int Cyc_AssnDef_simple_prove(void*ctxt,void*a){
LOOP:
 if(ctxt == a)return 1;{
void*_tmpFC=a;enum Cyc_AssnDef_Primreln _tmpFE;void*_tmpFF;void*_tmpFD;switch(*((int*)_tmpFC)){case 0:
 return 1;case 1:
 return 0;case 3: _tmpFD=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpFC)->f1;_tmpFF=(void*)((struct Cyc_AssnDef_And_AssnDef_Assn_struct*)_tmpFC)->f2;{void*a1=_tmpFD;void*a2=_tmpFF;
# 775
if(!Cyc_AssnDef_simple_prove(ctxt,a1))return 0;
a=a2;
goto LOOP;}case 4: _tmpFD=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpFC)->f1;_tmpFF=(void*)((struct Cyc_AssnDef_Or_AssnDef_Assn_struct*)_tmpFC)->f2;{void*a1=_tmpFD;void*a2=_tmpFF;
# 779
if(Cyc_AssnDef_simple_prove(ctxt,a1))return 1;
a=a2;
goto LOOP;}case 2: _tmpFD=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpFC)->f1;_tmpFE=((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpFC)->f2;_tmpFF=(void*)((struct Cyc_AssnDef_Prim_AssnDef_Assn_struct*)_tmpFC)->f3;{void*t1=_tmpFD;enum Cyc_AssnDef_Primreln p=_tmpFE;void*t2=_tmpFF;
return Cyc_AssnDef_find_ctxt(ctxt,t1,p,t2);}default:
# 784
 a=Cyc_AssnDef_reduce(a);
goto LOOP;};}}
