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

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

/* Regions */
struct _RegionPage
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  /* MWH: wish we didn't have to include the stuff below ... */
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
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
void   _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void   _pop_dynregion();

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

struct _xtunion_struct* Cyc_Core_get_exn_thrown();
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
#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#else
#define _INLINE inline
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_cks_null = (void*)(ptr); \
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

static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
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

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172 "list.h"
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 50 "string.h"
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 66
struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*cmp)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*k);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*k,void*v);
# 101
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*cmp)(void*,void*),void*k,void*v);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*k);
# 149 "dict.h"
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d);
# 257
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;
union Cyc_Absyn_Nmspace Cyc_Absyn_Rel_n(struct Cyc_List_List*);
# 98
union Cyc_Absyn_Nmspace Cyc_Absyn_Abs_n(struct Cyc_List_List*ns,int C_scope);struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 289
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};
# 302
union Cyc_Absyn_DatatypeFieldInfo Cyc_Absyn_UnknownDatatypefield(struct Cyc_Absyn_UnknownDatatypeFieldInfo);struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple0*,struct Cyc_Core_Opt*);
union Cyc_Absyn_AggrInfo Cyc_Absyn_KnownAggr(struct Cyc_Absyn_Aggrdecl**);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 693 "absyn.h"
extern struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct Cyc_Absyn_Wild_p_val;struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 876
int Cyc_Absyn_qvar_cmp(struct _tuple0*,struct _tuple0*);
int Cyc_Absyn_varlist_cmp(struct Cyc_List_List*,struct Cyc_List_List*);
# 913
extern void*Cyc_Absyn_heap_rgn_type;
# 917
extern void*Cyc_Absyn_false_type;
# 919
extern void*Cyc_Absyn_void_type;
# 935
struct Cyc_Absyn_Datatypedecl*Cyc_Absyn_exn_tud();
# 964
void*Cyc_Absyn_fatptr_type(void*t,void*rgn,struct Cyc_Absyn_Tqual,void*zeroterm);
# 1071
struct Cyc_Absyn_Pat*Cyc_Absyn_new_pat(void*,unsigned);
# 1080
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple0*,void*,struct Cyc_Absyn_Exp*init);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 40
void*Cyc_Warn_impos(struct _fat_ptr fmt,struct _fat_ptr);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 66
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 68
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 29 "binding.h"
extern int Cyc_Binding_warn_override;struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};
# 46
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*,void*(*mkdata)(void*));
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*,struct _fat_ptr*,void*,void*(*mkdata)(void*));
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename);
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt);
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt);
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned,struct Cyc_Binding_NSCtxt*,struct Cyc_List_List*);
# 34 "cifc.h"
void Cyc_Cifc_user_overrides(unsigned,struct Cyc_List_List*ds,struct Cyc_List_List*ovrs);char Cyc_Binding_BindingError[13U]="BindingError";struct Cyc_Binding_BindingError_exn_struct{char*tag;};
# 44 "binding.cyc"
int Cyc_Binding_warn_override=0;
# 58
struct Cyc_Binding_NSCtxt*Cyc_Binding_mt_nsctxt(void*env,void*(*mkdata)(void*)){
void*_tmp0=mkdata(env);void*data=_tmp0;
return({struct Cyc_Binding_NSCtxt*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->curr_ns=0,({
struct Cyc_List_List*_tmp30F=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));({void*_tmp30E=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0U,_tmp1->f1=0;_tmp1;});_tmp2->hd=_tmp30E;}),_tmp2->tl=0;_tmp2;});_tmp3->availables=_tmp30F;}),({
struct Cyc_Dict_Dict _tmp30D=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct Cyc_List_List*,struct Cyc_List_List*),struct Cyc_List_List*k,void*v))Cyc_Dict_singleton)(Cyc_Absyn_varlist_cmp,0,data);_tmp3->ns_data=_tmp30D;});_tmp3;});}
# 64
void*Cyc_Binding_get_ns_data(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns){
union Cyc_Absyn_Nmspace _tmp4=abs_ns;struct Cyc_List_List*_tmp5;struct Cyc_List_List*_tmp6;switch((_tmp4.Abs_n).tag){case 3U: _LL1: _tmp6=(_tmp4.C_n).val;_LL2: {struct Cyc_List_List*vs=_tmp6;
_tmp5=vs;goto _LL4;}case 2U: _LL3: _tmp5=(_tmp4.Abs_n).val;_LL4: {struct Cyc_List_List*vs=_tmp5;
return((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,vs);}default: _LL5: _LL6:
({void*_tmp7=0U;({struct _fat_ptr _tmp310=({const char*_tmp8="Binding:get_ns_data: relative ns";_tag_fat(_tmp8,sizeof(char),33U);});Cyc_Warn_impos(_tmp310,_tag_fat(_tmp7,sizeof(void*),0U));});});}_LL0:;}
# 82 "binding.cyc"
struct Cyc_List_List*Cyc_Binding_resolve_rel_ns(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct Cyc_List_List*rel_ns){
# 85
struct Cyc_List_List*_tmp9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ctxt->curr_ns,rel_ns);struct Cyc_List_List*fullname=_tmp9;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,fullname))
return fullname;
{struct Cyc_List_List*_tmpA=ctxt->availables;struct Cyc_List_List*as=_tmpA;for(0;as != 0;as=as->tl){
void*_tmpB=(void*)as->hd;void*_stmttmp0=_tmpB;void*_tmpC=_stmttmp0;struct Cyc_List_List*_tmpD;struct Cyc_List_List*_tmpE;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->tag == 1U){_LL1: _tmpE=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmpC)->f1;_LL2: {struct Cyc_List_List*ns=_tmpE;
_tmpD=ns;goto _LL4;}}else{_LL3: _tmpD=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmpC)->f1;_LL4: {struct Cyc_List_List*ns=_tmpD;
# 92
struct Cyc_List_List*_tmpF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(ns,rel_ns);struct Cyc_List_List*fullname=_tmpF;
if(((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,fullname))
return fullname;
goto _LL0;}}_LL0:;}}
# 97
({struct Cyc_Warn_String_Warn_Warg_struct _tmp11=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E0;_tmp2E0.tag=0U,({struct _fat_ptr _tmp311=({const char*_tmp16="namespace ";_tag_fat(_tmp16,sizeof(char),11U);});_tmp2E0.f1=_tmp311;});_tmp2E0;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DF;_tmp2DF.tag=0U,({struct _fat_ptr _tmp313=(struct _fat_ptr)({struct Cyc_List_List*_tmp312=rel_ns;Cyc_str_sepstr(_tmp312,({const char*_tmp15="::";_tag_fat(_tmp15,sizeof(char),3U);}));});_tmp2DF.f1=_tmp313;});_tmp2DF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp13=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DE;_tmp2DE.tag=0U,({struct _fat_ptr _tmp314=({const char*_tmp14=" not found";_tag_fat(_tmp14,sizeof(char),11U);});_tmp2DE.f1=_tmp314;});_tmp2DE;});void*_tmp10[3U];_tmp10[0]=& _tmp11,_tmp10[1]=& _tmp12,_tmp10[2]=& _tmp13;({unsigned _tmp315=loc;Cyc_Warn_err2(_tmp315,_tag_fat(_tmp10,sizeof(void*),3U));});});
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=Cyc_Binding_BindingError;_tmp17;}));}
# 102
void*Cyc_Binding_resolve_lookup(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(void*,struct _fat_ptr*)){
struct _tuple0*_tmp18=qv;struct _fat_ptr*_tmp1A;union Cyc_Absyn_Nmspace _tmp19;_LL1: _tmp19=_tmp18->f1;_tmp1A=_tmp18->f2;_LL2: {union Cyc_Absyn_Nmspace ns=_tmp19;struct _fat_ptr*v=_tmp1A;
union Cyc_Absyn_Nmspace _tmp1B=ns;struct Cyc_List_List*_tmp1C;struct Cyc_List_List*_tmp1D;struct Cyc_List_List*_tmp1E;switch((_tmp1B.Abs_n).tag){case 1U: if((_tmp1B.Rel_n).val == 0){_LL4: _LL5:
# 106
{struct _handler_cons _tmp1F;_push_handler(& _tmp1F);{int _tmp21=0;if(setjmp(_tmp1F.handler))_tmp21=1;if(!_tmp21){{void*_tmp22=({void*(*_tmp317)(void*,struct _fat_ptr*)=lookup;void*_tmp316=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ctxt->curr_ns);_tmp317(_tmp316,v);});_npop_handler(0U);return _tmp22;};_pop_handler();}else{void*_tmp20=(void*)Cyc_Core_get_exn_thrown();void*_tmp23=_tmp20;void*_tmp24;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp23)->tag == Cyc_Dict_Absent){_LLF: _LL10:
 goto _LLE;}else{_LL11: _tmp24=_tmp23;_LL12: {void*exn=_tmp24;(int)_rethrow(exn);}}_LLE:;}}}
{struct Cyc_List_List*_tmp25=ctxt->availables;struct Cyc_List_List*as=_tmp25;for(0;as != 0;as=as->tl){
void*_tmp26=(void*)as->hd;void*_stmttmp1=_tmp26;void*_tmp27=_stmttmp1;struct Cyc_List_List*_tmp28;struct Cyc_List_List*_tmp29;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp27)->tag == 1U){_LL14: _tmp29=((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp27)->f1;_LL15: {struct Cyc_List_List*ns=_tmp29;
_tmp28=ns;goto _LL17;}}else{_LL16: _tmp28=((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp27)->f1;_LL17: {struct Cyc_List_List*ns=_tmp28;
# 112
{struct _handler_cons _tmp2A;_push_handler(& _tmp2A);{int _tmp2C=0;if(setjmp(_tmp2A.handler))_tmp2C=1;if(!_tmp2C){{void*_tmp2D=({void*(*_tmp319)(void*,struct _fat_ptr*)=lookup;void*_tmp318=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ns);_tmp319(_tmp318,v);});_npop_handler(0U);return _tmp2D;};_pop_handler();}else{void*_tmp2B=(void*)Cyc_Core_get_exn_thrown();void*_tmp2E=_tmp2B;void*_tmp2F;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp2E)->tag == Cyc_Dict_Absent){_LL19: _LL1A:
 goto _LL18;}else{_LL1B: _tmp2F=_tmp2E;_LL1C: {void*exn=_tmp2F;(int)_rethrow(exn);}}_LL18:;}}}
goto _LL13;}}_LL13:;}}
# 116
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=Cyc_Binding_BindingError;_tmp30;}));}else{_LL6: _tmp1E=(_tmp1B.Rel_n).val;_LL7: {struct Cyc_List_List*ns=_tmp1E;
# 118
struct _handler_cons _tmp31;_push_handler(& _tmp31);{int _tmp33=0;if(setjmp(_tmp31.handler))_tmp33=1;if(!_tmp33){
{struct Cyc_List_List*_tmp34=Cyc_Binding_resolve_rel_ns(loc,ctxt,ns);struct Cyc_List_List*abs_ns=_tmp34;
void*_tmp35=({void*(*_tmp31B)(void*,struct _fat_ptr*)=lookup;void*_tmp31A=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,abs_ns);_tmp31B(_tmp31A,v);});_npop_handler(0U);return _tmp35;}
# 119
;_pop_handler();}else{void*_tmp32=(void*)Cyc_Core_get_exn_thrown();void*_tmp36=_tmp32;void*_tmp37;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp36)->tag == Cyc_Dict_Absent){_LL1E: _LL1F:
# 121
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp38=_cycalloc(sizeof(*_tmp38));_tmp38->tag=Cyc_Binding_BindingError;_tmp38;}));}else{_LL20: _tmp37=_tmp36;_LL21: {void*exn=_tmp37;(int)_rethrow(exn);}}_LL1D:;}}}}case 3U: _LL8: _tmp1D=(_tmp1B.C_n).val;_LL9: {struct Cyc_List_List*ns=_tmp1D;
_tmp1C=ns;goto _LLB;}case 2U: _LLA: _tmp1C=(_tmp1B.Abs_n).val;_LLB: {struct Cyc_List_List*ns=_tmp1C;
# 124
struct _handler_cons _tmp39;_push_handler(& _tmp39);{int _tmp3B=0;if(setjmp(_tmp39.handler))_tmp3B=1;if(!_tmp3B){{void*_tmp3C=({void*(*_tmp31D)(void*,struct _fat_ptr*)=lookup;void*_tmp31C=((void*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)(ctxt->ns_data,ns);_tmp31D(_tmp31C,v);});_npop_handler(0U);return _tmp3C;};_pop_handler();}else{void*_tmp3A=(void*)Cyc_Core_get_exn_thrown();void*_tmp3D=_tmp3A;void*_tmp3E;if(((struct Cyc_Dict_Absent_exn_struct*)_tmp3D)->tag == Cyc_Dict_Absent){_LL23: _LL24:
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp3F=_cycalloc(sizeof(*_tmp3F));_tmp3F->tag=Cyc_Binding_BindingError;_tmp3F;}));}else{_LL25: _tmp3E=_tmp3D;_LL26: {void*exn=_tmp3E;(int)_rethrow(exn);}}_LL22:;}}}default: _LLC: _LLD:
({void*_tmp40=0U;({struct _fat_ptr _tmp31E=({const char*_tmp41="lookup local in global";_tag_fat(_tmp41,sizeof(char),23U);});Cyc_Warn_impos(_tmp31E,_tag_fat(_tmp40,sizeof(void*),0U));});});}_LL3:;}}
# 130
void Cyc_Binding_enter_ns(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,void*env,void*(*mkdata)(void*)){
struct Cyc_List_List*_tmp42=ctxt->curr_ns;struct Cyc_List_List*ns=_tmp42;
struct Cyc_List_List*_tmp43=({struct Cyc_List_List*_tmp31F=ns;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp31F,({struct Cyc_List_List*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->hd=subname,_tmp46->tl=0;_tmp46;}));});struct Cyc_List_List*ns2=_tmp43;
if(!((int(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_member)(ctxt->ns_data,ns2))
({struct Cyc_Dict_Dict _tmp322=({struct Cyc_Dict_Dict _tmp321=ctxt->ns_data;struct Cyc_List_List*_tmp320=ns2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k,void*v))Cyc_Dict_insert)(_tmp321,_tmp320,mkdata(env));});ctxt->ns_data=_tmp322;});
ctxt->curr_ns=ns2;
({struct Cyc_List_List*_tmp324=({struct Cyc_List_List*_tmp45=_cycalloc(sizeof(*_tmp45));({void*_tmp323=(void*)({struct Cyc_Binding_Namespace_Binding_NSDirective_struct*_tmp44=_cycalloc(sizeof(*_tmp44));_tmp44->tag=0U,_tmp44->f1=ns2;_tmp44;});_tmp45->hd=_tmp323;}),_tmp45->tl=ctxt->availables;_tmp45;});ctxt->availables=_tmp324;});}
# 138
void Cyc_Binding_leave_ns(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp47=0U;({struct _fat_ptr _tmp325=({const char*_tmp48="leaving topmost namespace";_tag_fat(_tmp48,sizeof(char),26U);});Cyc_Warn_impos(_tmp325,_tag_fat(_tmp47,sizeof(void*),0U));});});{
void*_tmp49=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_stmttmp2=_tmp49;void*_tmp4A=_stmttmp2;if(((struct Cyc_Binding_Namespace_Binding_NSDirective_struct*)_tmp4A)->tag == 0U){_LL1: _LL2:
# 143
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;
({struct Cyc_List_List*_tmp326=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ctxt->curr_ns)))->tl);ctxt->curr_ns=_tmp326;});
goto _LL0;}else{_LL3: _LL4:
# 147
({void*_tmp4B=0U;({struct _fat_ptr _tmp327=({const char*_tmp4C="leaving using as namespace";_tag_fat(_tmp4C,sizeof(char),27U);});Cyc_Warn_impos(_tmp327,_tag_fat(_tmp4B,sizeof(void*),0U));});});}_LL0:;}}
# 150
struct Cyc_List_List*Cyc_Binding_enter_using(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename){
struct _tuple0*_tmp4D=usename;struct _fat_ptr*_tmp4F;union Cyc_Absyn_Nmspace _tmp4E;_LL1: _tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;_LL2: {union Cyc_Absyn_Nmspace nsl=_tmp4E;struct _fat_ptr*v=_tmp4F;
struct Cyc_List_List*ns;
{union Cyc_Absyn_Nmspace _tmp50=nsl;struct Cyc_List_List*_tmp51;struct Cyc_List_List*_tmp52;switch((_tmp50.Loc_n).tag){case 1U: _LL4: _tmp52=(_tmp50.Rel_n).val;_LL5: {struct Cyc_List_List*vs=_tmp52;
# 155
ns=({unsigned _tmp32A=loc;struct Cyc_Binding_NSCtxt*_tmp329=ctxt;Cyc_Binding_resolve_rel_ns(_tmp32A,_tmp329,({struct Cyc_List_List*_tmp328=vs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp328,({struct Cyc_List_List*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->hd=v,_tmp53->tl=0;_tmp53;}));}));});{
struct Cyc_List_List*_tmp54=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(((struct Cyc_List_List*)_check_null(((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(ns)))->tl);struct Cyc_List_List*abs_vs=_tmp54;
({union Cyc_Absyn_Nmspace _tmp32B=Cyc_Absyn_Abs_n(abs_vs,0);(*usename).f1=_tmp32B;});
goto _LL3;}}case 2U: _LL6: _tmp51=(_tmp50.Abs_n).val;_LL7: {struct Cyc_List_List*vs=_tmp51;
# 160
ns=({struct Cyc_List_List*_tmp32C=vs;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp32C,({struct Cyc_List_List*_tmp55=_cycalloc(sizeof(*_tmp55));_tmp55->hd=v,_tmp55->tl=0;_tmp55;}));});
goto _LL3;}case 4U: _LL8: _LL9:
({void*_tmp56=0U;({struct _fat_ptr _tmp32D=({const char*_tmp57="enter_using, Loc_n";_tag_fat(_tmp57,sizeof(char),19U);});Cyc_Warn_impos(_tmp32D,_tag_fat(_tmp56,sizeof(void*),0U));});});default: _LLA: _LLB:
({void*_tmp58=0U;({struct _fat_ptr _tmp32E=({const char*_tmp59="enter_using, C_n";_tag_fat(_tmp59,sizeof(char),17U);});Cyc_Warn_impos(_tmp32E,_tag_fat(_tmp58,sizeof(void*),0U));});});}_LL3:;}
# 165
({struct Cyc_List_List*_tmp330=({struct Cyc_List_List*_tmp5B=_cycalloc(sizeof(*_tmp5B));({void*_tmp32F=(void*)({struct Cyc_Binding_Using_Binding_NSDirective_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=1U,_tmp5A->f1=ns;_tmp5A;});_tmp5B->hd=_tmp32F;}),_tmp5B->tl=ctxt->availables;_tmp5B;});ctxt->availables=_tmp330;});
return ns;}}
# 168
void Cyc_Binding_leave_using(struct Cyc_Binding_NSCtxt*ctxt){
if(ctxt->availables == 0)
({void*_tmp5C=0U;({struct _fat_ptr _tmp331=({const char*_tmp5D="leaving topmost namespace as a using";_tag_fat(_tmp5D,sizeof(char),37U);});Cyc_Warn_impos(_tmp331,_tag_fat(_tmp5C,sizeof(void*),0U));});});{
void*_tmp5E=(void*)((struct Cyc_List_List*)_check_null(ctxt->availables))->hd;void*_stmttmp3=_tmp5E;void*_tmp5F=_stmttmp3;if(((struct Cyc_Binding_Using_Binding_NSDirective_struct*)_tmp5F)->tag == 1U){_LL1: _LL2:
 ctxt->availables=((struct Cyc_List_List*)_check_null(ctxt->availables))->tl;goto _LL0;}else{_LL3: _LL4:
({void*_tmp60=0U;({struct _fat_ptr _tmp332=({const char*_tmp61="leaving namespace as using";_tag_fat(_tmp61,sizeof(char),27U);});Cyc_Warn_impos(_tmp332,_tag_fat(_tmp60,sizeof(void*),0U));});});}_LL0:;}}struct Cyc_Binding_VarRes_Binding_Resolved_struct{int tag;void*f1;};struct Cyc_Binding_AggrRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct Cyc_Binding_EnumRes_Binding_Resolved_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Binding_ResolveNSEnv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Binding_Env{int in_cinclude;struct Cyc_Binding_NSCtxt*ns;struct Cyc_Dict_Dict*local_vars;};
# 212 "binding.cyc"
inline static int Cyc_Binding_in_cinclude(struct Cyc_Binding_Env*env){
return env->in_cinclude;}
# 215
inline static int Cyc_Binding_at_toplevel(struct Cyc_Binding_Env*env){
return env->local_vars == 0;}
# 218
static struct Cyc_Binding_ResolveNSEnv*Cyc_Binding_mt_renv(int ignore){
return({struct Cyc_Binding_ResolveNSEnv*_tmp62=_cycalloc(sizeof(*_tmp62));({struct Cyc_Dict_Dict _tmp337=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->aggrdecls=_tmp337;}),({
struct Cyc_Dict_Dict _tmp336=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->datatypedecls=_tmp336;}),({
struct Cyc_Dict_Dict _tmp335=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->enumdecls=_tmp335;}),({
struct Cyc_Dict_Dict _tmp334=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->typedefs=_tmp334;}),({
struct Cyc_Dict_Dict _tmp333=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);_tmp62->ordinaries=_tmp333;});_tmp62;});}
# 226
static struct Cyc_Absyn_Aggrdecl*Cyc_Binding_lookup_aggrdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Aggrdecl*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->aggrdecls,v);}
# 229
static struct Cyc_List_List*Cyc_Binding_lookup_datatypedecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_List_List*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->datatypedecls,v);}
# 232
static struct Cyc_Absyn_Enumdecl*Cyc_Binding_lookup_enumdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Enumdecl*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->enumdecls,v);}
# 235
static struct Cyc_Absyn_Typedefdecl*Cyc_Binding_lookup_typedefdecl(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((struct Cyc_Absyn_Typedefdecl*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->typedefs,v);}
# 238
static void*Cyc_Binding_lookup_ordinary_global(struct Cyc_Binding_ResolveNSEnv*renv,struct _fat_ptr*v){
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(renv->ordinaries,v);}
# 241
static void*Cyc_Binding_lookup_ordinary(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv){
struct _tuple0*_tmp63=qv;struct _fat_ptr*_tmp65;union Cyc_Absyn_Nmspace _tmp64;_LL1: _tmp64=_tmp63->f1;_tmp65=_tmp63->f2;_LL2: {union Cyc_Absyn_Nmspace nsl=_tmp64;struct _fat_ptr*v=_tmp65;
union Cyc_Absyn_Nmspace _tmp66=nsl;switch((_tmp66.Rel_n).tag){case 4U: _LL4: _LL5:
# 245
 if(Cyc_Binding_at_toplevel(env)|| !((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v))
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->tag=Cyc_Binding_BindingError;_tmp67;}));
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v);case 1U: if((_tmp66.Rel_n).val == 0){_LL6: _LL7:
# 249
 if(!Cyc_Binding_at_toplevel(env)&&((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v))
return((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(*((struct Cyc_Dict_Dict*)_check_null(env->local_vars)),v);
goto _LL9;}else{goto _LL8;}default: _LL8: _LL9:
# 253
 return((void*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,void*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_ordinary_global);}_LL3:;}}
# 257
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*,struct Cyc_Absyn_Decl*);
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*,struct Cyc_Absyn_Stmt*);
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*,struct Cyc_Absyn_Pat*);
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*,struct Cyc_Absyn_Exp*);
void Cyc_Binding_resolve_type(unsigned,struct Cyc_Binding_Env*,void*);
void Cyc_Binding_resolve_rgnpo(unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_aggrfields(unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*);
void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo i);
void Cyc_Binding_resolve_asm_iolist(struct Cyc_Binding_Env*,struct Cyc_List_List*l);
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int,unsigned,struct Cyc_Binding_Env*,struct Cyc_List_List*,struct Cyc_Absyn_VarargInfo*);
# 273
void Cyc_Binding_absolutize_decl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
union Cyc_Absyn_Nmspace _tmp68=(*qv).f1;union Cyc_Absyn_Nmspace _stmttmp4=_tmp68;union Cyc_Absyn_Nmspace _tmp69=_stmttmp4;switch((_tmp69.Abs_n).tag){case 1U: if((_tmp69.Rel_n).val == 0){_LL1: _LL2:
# 276
 if(Cyc_Binding_at_toplevel(env))
({union Cyc_Absyn_Nmspace _tmp339=({struct Cyc_List_List*_tmp338=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp338,Cyc_Binding_in_cinclude(env)||(int)sc == (int)Cyc_Absyn_ExternC);});(*qv).f1=_tmp339;});else{
# 279
(*qv).f1=Cyc_Absyn_Loc_n;}
goto _LL0;}else{goto _LL7;}case 4U: _LL3: _LL4:
 goto _LL0;case 2U: _LL5: _LL6:
# 283
 if(!Cyc_Binding_at_toplevel(env))
goto _LL8;
goto _LL0;default: _LL7: _LL8:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E3;_tmp2E3.tag=0U,({struct _fat_ptr _tmp33A=({const char*_tmp6F="qualified names in declarations unimplemented (";_tag_fat(_tmp6F,sizeof(char),48U);});_tmp2E3.f1=_tmp33A;});_tmp2E3;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp6C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E2;_tmp2E2.tag=1U,_tmp2E2.f1=qv;_tmp2E2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp6D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E1;_tmp2E1.tag=0U,({struct _fat_ptr _tmp33B=({const char*_tmp6E=")";_tag_fat(_tmp6E,sizeof(char),2U);});_tmp2E1.f1=_tmp33B;});_tmp2E1;});void*_tmp6A[3U];_tmp6A[0]=& _tmp6B,_tmp6A[1]=& _tmp6C,_tmp6A[2]=& _tmp6D;({unsigned _tmp33C=loc;Cyc_Warn_err2(_tmp33C,_tag_fat(_tmp6A,sizeof(void*),3U));});});}_LL0:;}
# 292
void Cyc_Binding_absolutize_topdecl(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*qv,enum Cyc_Absyn_Scope sc){
struct Cyc_Dict_Dict*_tmp70=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp70;
env->local_vars=0;
Cyc_Binding_absolutize_decl(loc,env,qv,sc);
env->local_vars=old_locals;}
# 299
void Cyc_Binding_check_warn_override(unsigned loc,struct Cyc_Binding_Env*env,struct _tuple0*q){
struct _fat_ptr hides_what;
struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){
{void*_tmp74=Cyc_Binding_lookup_ordinary(loc,env,q);void*_stmttmp5=_tmp74;void*_tmp75=_stmttmp5;void*_tmp76;switch(*((int*)_tmp75)){case 0U: _LL1: _tmp76=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp75)->f1;_LL2: {void*b=_tmp76;
# 304
if(Cyc_Binding_at_toplevel(env)){
_npop_handler(0U);return;}
{void*_tmp77=b;switch(*((int*)_tmp77)){case 1U: _LLC: _LLD:
 hides_what=({const char*_tmp78="global variable";_tag_fat(_tmp78,sizeof(char),16U);});goto _LLB;case 4U: _LLE: _LLF:
 hides_what=({const char*_tmp79="local variable";_tag_fat(_tmp79,sizeof(char),15U);});goto _LLB;case 3U: _LL10: _LL11:
 hides_what=({const char*_tmp7A="parameter";_tag_fat(_tmp7A,sizeof(char),10U);});goto _LLB;case 5U: _LL12: _LL13:
 hides_what=({const char*_tmp7B="pattern variable";_tag_fat(_tmp7B,sizeof(char),17U);});goto _LLB;case 2U: _LL14: _LL15:
 hides_what=({const char*_tmp7C="function";_tag_fat(_tmp7C,sizeof(char),9U);});goto _LLB;default: _LL16: _LL17:
({void*_tmp7D=0U;({struct _fat_ptr _tmp33D=({const char*_tmp7E="shadowing free variable!";_tag_fat(_tmp7E,sizeof(char),25U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp33D,_tag_fat(_tmp7D,sizeof(void*),0U));});});}_LLB:;}
# 314
goto _LL0;}case 1U: _LL3: _LL4:
 hides_what=({const char*_tmp7F="struct constructor";_tag_fat(_tmp7F,sizeof(char),19U);});goto _LL0;case 2U: _LL5: _LL6:
 hides_what=({const char*_tmp80="datatype constructor";_tag_fat(_tmp80,sizeof(char),21U);});goto _LL0;case 3U: _LL7: _LL8:
 goto _LLA;default: _LL9: _LLA:
 hides_what=({const char*_tmp81="enum tag";_tag_fat(_tmp81,sizeof(char),9U);});goto _LL0;}_LL0:;}
# 320
({struct Cyc_Warn_String_Warn_Warg_struct _tmp83=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5;_tmp2E5.tag=0U,({struct _fat_ptr _tmp33E=({const char*_tmp85="declaration hides ";_tag_fat(_tmp85,sizeof(char),19U);});_tmp2E5.f1=_tmp33E;});_tmp2E5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp84=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4;_tmp2E4.tag=0U,_tmp2E4.f1=hides_what;_tmp2E4;});void*_tmp82[2U];_tmp82[0]=& _tmp83,_tmp82[1]=& _tmp84;({unsigned _tmp33F=loc;Cyc_Warn_warn2(_tmp33F,_tag_fat(_tmp82,sizeof(void*),2U));});});
_npop_handler(0U);return;
# 302
;_pop_handler();}else{void*_tmp72=(void*)Cyc_Core_get_exn_thrown();void*_tmp86=_tmp72;void*_tmp87;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp86)->tag == Cyc_Binding_BindingError){_LL19: _LL1A:
# 322
 return;}else{_LL1B: _tmp87=_tmp86;_LL1C: {void*exn=_tmp87;(int)_rethrow(exn);}}_LL18:;}}}
# 325
void Cyc_Binding_resolve_and_add_vardecl(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_Vardecl*vd){
Cyc_Binding_absolutize_decl(loc,env,vd->name,vd->sc);{
struct _tuple0*_tmp88=vd->name;struct _tuple0*_stmttmp6=_tmp88;struct _tuple0*_tmp89=_stmttmp6;struct _fat_ptr*_tmp8B;union Cyc_Absyn_Nmspace _tmp8A;_LL1: _tmp8A=_tmp89->f1;_tmp8B=_tmp89->f2;_LL2: {union Cyc_Absyn_Nmspace decl_ns=_tmp8A;struct _fat_ptr*decl_name=_tmp8B;
Cyc_Binding_resolve_type(loc,env,vd->type);
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,vd->name);
if(!Cyc_Binding_at_toplevel(env))
({struct Cyc_Dict_Dict _tmp343=({struct Cyc_Dict_Dict _tmp342=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp341=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp342,_tmp341,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp8D=_cycalloc(sizeof(*_tmp8D));
_tmp8D->tag=0U,({void*_tmp340=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=4U,_tmp8C->f1=vd;_tmp8C;});_tmp8D->f1=_tmp340;});_tmp8D;}));});
# 332
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp343;});else{
# 335
struct Cyc_Binding_ResolveNSEnv*_tmp8E=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp8E;
({struct Cyc_Dict_Dict _tmp347=({struct Cyc_Dict_Dict _tmp346=decl_ns_data->ordinaries;struct _fat_ptr*_tmp345=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp346,_tmp345,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp90=_cycalloc(sizeof(*_tmp90));
# 338
_tmp90->tag=0U,({void*_tmp344=(void*)({struct Cyc_Absyn_Global_b_Absyn_Binding_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=1U,_tmp8F->f1=vd;_tmp8F;});_tmp90->f1=_tmp344;});_tmp90;}));});
# 336
decl_ns_data->ordinaries=_tmp347;});}}}}
# 342
void Cyc_Binding_resolve_stmt(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Stmt*s){
void*_tmp91=s->r;void*_stmttmp7=_tmp91;void*_tmp92=_stmttmp7;struct Cyc_List_List*_tmp94;struct Cyc_Absyn_Stmt*_tmp93;struct Cyc_List_List*_tmp96;struct Cyc_Absyn_Exp*_tmp95;struct Cyc_Absyn_Stmt*_tmp98;struct Cyc_Absyn_Decl*_tmp97;struct Cyc_Absyn_Stmt*_tmp99;struct Cyc_List_List*_tmp9A;struct Cyc_Absyn_Stmt*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_Absyn_Exp*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_Absyn_Stmt*_tmpA0;struct Cyc_Absyn_Exp*_tmp9F;struct Cyc_Absyn_Exp*_tmpA2;struct Cyc_Absyn_Stmt*_tmpA1;struct Cyc_Absyn_Stmt*_tmpA5;struct Cyc_Absyn_Stmt*_tmpA4;struct Cyc_Absyn_Exp*_tmpA3;struct Cyc_Absyn_Exp*_tmpA6;struct Cyc_Absyn_Stmt*_tmpA8;struct Cyc_Absyn_Stmt*_tmpA7;struct Cyc_Absyn_Exp*_tmpA9;switch(*((int*)_tmp92)){case 0U: _LL1: _LL2:
 return;case 1U: _LL3: _tmpA9=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL4: {struct Cyc_Absyn_Exp*e=_tmpA9;
Cyc_Binding_resolve_exp(env,e);return;}case 2U: _LL5: _tmpA7=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA8=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL6: {struct Cyc_Absyn_Stmt*s1=_tmpA7;struct Cyc_Absyn_Stmt*s2=_tmpA8;
Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_stmt(env,s2);return;}case 3U: _LL7: _tmpA6=((struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL8: {struct Cyc_Absyn_Exp*eopt=_tmpA6;
if((unsigned)eopt)Cyc_Binding_resolve_exp(env,eopt);return;}case 4U: _LL9: _tmpA3=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA4=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_tmpA5=((struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct*)_tmp92)->f3;_LLA: {struct Cyc_Absyn_Exp*e=_tmpA3;struct Cyc_Absyn_Stmt*s1=_tmpA4;struct Cyc_Absyn_Stmt*s2=_tmpA5;
# 349
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_stmt(env,s2);return;}case 14U: _LLB: _tmpA1=((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmpA2=(((struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct*)_tmp92)->f2).f1;_LLC: {struct Cyc_Absyn_Stmt*s1=_tmpA1;struct Cyc_Absyn_Exp*e=_tmpA2;
_tmp9F=e;_tmpA0=s1;goto _LLE;}case 5U: _LLD: _tmp9F=(((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp92)->f1).f1;_tmpA0=((struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LLE: {struct Cyc_Absyn_Exp*e=_tmp9F;struct Cyc_Absyn_Stmt*s1=_tmpA0;
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_stmt(env,s1);return;}case 9U: _LLF: _tmp9B=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp9C=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f2).f1;_tmp9D=(((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f3).f1;_tmp9E=((struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct*)_tmp92)->f4;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp9B;struct Cyc_Absyn_Exp*e2=_tmp9C;struct Cyc_Absyn_Exp*e3=_tmp9D;struct Cyc_Absyn_Stmt*s1=_tmp9E;
# 353
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);Cyc_Binding_resolve_exp(env,e3);
Cyc_Binding_resolve_stmt(env,s1);
return;}case 6U: _LL11: _LL12:
 goto _LL14;case 7U: _LL13: _LL14:
 goto _LL16;case 8U: _LL15: _LL16:
 return;case 11U: _LL17: _tmp9A=((struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_LL18: {struct Cyc_List_List*es=_tmp9A;
# 360
for(0;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 13U: _LL19: _tmp99=((struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1A: {struct Cyc_Absyn_Stmt*s1=_tmp99;
Cyc_Binding_resolve_stmt(env,s1);return;}case 12U: _LL1B: _tmp97=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp98=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1C: {struct Cyc_Absyn_Decl*d=_tmp97;struct Cyc_Absyn_Stmt*s1=_tmp98;
# 365
struct Cyc_Dict_Dict _tmpAA=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmpAA;
Cyc_Binding_resolve_decl(env,d);
Cyc_Binding_resolve_stmt(env,s1);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;
return;}case 10U: _LL1D: _tmp95=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp96=((struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL1E: {struct Cyc_Absyn_Exp*e=_tmp95;struct Cyc_List_List*scs=_tmp96;
Cyc_Binding_resolve_exp(env,e);Cyc_Binding_resolve_scs(env,scs);return;}default: _LL1F: _tmp93=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp92)->f1;_tmp94=((struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct*)_tmp92)->f2;_LL20: {struct Cyc_Absyn_Stmt*s1=_tmp93;struct Cyc_List_List*scs=_tmp94;
Cyc_Binding_resolve_stmt(env,s1);Cyc_Binding_resolve_scs(env,scs);return;}}_LL0:;}struct _tuple11{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 374
void Cyc_Binding_resolve_exp(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Exp*e){
void*_tmpAB=e->r;void*_stmttmp8=_tmpAB;void*_tmpAC=_stmttmp8;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct Cyc_List_List*_tmpAF;struct _fat_ptr _tmpAE;int _tmpAD;struct Cyc_List_List*_tmpB3;void*_tmpB2;struct Cyc_Absyn_Datatypefield*_tmpB6;struct Cyc_Absyn_Datatypedecl*_tmpB5;struct Cyc_List_List*_tmpB4;struct Cyc_Absyn_Enumfield*_tmpB8;struct Cyc_Absyn_Enumdecl*_tmpB7;struct Cyc_Absyn_Enumfield*_tmpBA;void*_tmpB9;struct Cyc_List_List*_tmpBB;struct Cyc_List_List*_tmpBD;void*_tmpBC;void*_tmpBE;void*_tmpBF;void*_tmpC0;struct Cyc_Absyn_Stmt*_tmpC1;struct Cyc_Absyn_Exp*_tmpC3;void*_tmpC2;struct Cyc_List_List*_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;struct Cyc_Absyn_Exp*_tmpCD;void**_tmpCC;struct Cyc_Absyn_Exp*_tmpCB;struct Cyc_Absyn_Exp*_tmpCF;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpD1;struct Cyc_Absyn_Exp*_tmpD0;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE5;void*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_List_List*_tmpE8;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Vardecl*_tmpE9;struct Cyc_Absyn_Aggrdecl**_tmpEF;struct Cyc_List_List*_tmpEE;struct Cyc_List_List*_tmpED;struct _tuple0**_tmpEC;int*_tmpF2;struct Cyc_List_List*_tmpF1;struct Cyc_Absyn_Exp*_tmpF0;void**_tmpF3;switch(*((int*)_tmpAC)){case 1U: _LL1: _tmpF3=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL2: {void**b=_tmpF3;
# 377
void*_tmpF4=*b;void*_stmttmp9=_tmpF4;void*_tmpF5=_stmttmp9;struct _tuple0*_tmpF6;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF5)->tag == 0U){_LL56: _tmpF6=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmpF5)->f1;_LL57: {struct _tuple0*q=_tmpF6;
# 379
struct _handler_cons _tmpF7;_push_handler(& _tmpF7);{int _tmpF9=0;if(setjmp(_tmpF7.handler))_tmpF9=1;if(!_tmpF9){{void*_tmpFA=Cyc_Binding_lookup_ordinary(e->loc,env,q);void*_stmttmpA=_tmpFA;void*_tmpFB=_stmttmpA;struct Cyc_Absyn_Aggrdecl*_tmpFC;struct Cyc_Absyn_Enumfield*_tmpFE;void*_tmpFD;struct Cyc_Absyn_Enumfield*_tmp100;struct Cyc_Absyn_Enumdecl*_tmpFF;struct Cyc_Absyn_Datatypefield*_tmp102;struct Cyc_Absyn_Datatypedecl*_tmp101;void*_tmp103;switch(*((int*)_tmpFB)){case 0U: _LL5B: _tmp103=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmpFB)->f1;_LL5C: {void*bnd=_tmp103;
*b=bnd;_npop_handler(0U);return;}case 2U: _LL5D: _tmp101=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpFB)->f1;_tmp102=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmpFB)->f2;_LL5E: {struct Cyc_Absyn_Datatypedecl*tud=_tmp101;struct Cyc_Absyn_Datatypefield*tuf=_tmp102;
({void*_tmp348=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->tag=31U,_tmp104->f1=0,_tmp104->f2=tud,_tmp104->f3=tuf;_tmp104;});e->r=_tmp348;});_npop_handler(0U);return;}case 3U: _LL5F: _tmpFF=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpFB)->f1;_tmp100=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmpFB)->f2;_LL60: {struct Cyc_Absyn_Enumdecl*ed=_tmpFF;struct Cyc_Absyn_Enumfield*ef=_tmp100;
({void*_tmp349=(void*)({struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*_tmp105=_cycalloc(sizeof(*_tmp105));_tmp105->tag=32U,_tmp105->f1=ed,_tmp105->f2=ef;_tmp105;});e->r=_tmp349;});_npop_handler(0U);return;}case 4U: _LL61: _tmpFD=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpFB)->f1;_tmpFE=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmpFB)->f2;_LL62: {void*t=_tmpFD;struct Cyc_Absyn_Enumfield*ef=_tmpFE;
({void*_tmp34A=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp106=_cycalloc(sizeof(*_tmp106));_tmp106->tag=33U,_tmp106->f1=t,_tmp106->f2=ef;_tmp106;});e->r=_tmp34A;});_npop_handler(0U);return;}default: _LL63: _tmpFC=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmpFB)->f1;_LL64: {struct Cyc_Absyn_Aggrdecl*ad=_tmpFC;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp108=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7;_tmp2E7.tag=0U,({struct _fat_ptr _tmp34B=({const char*_tmp10A="bad occurrence of type name ";_tag_fat(_tmp10A,sizeof(char),29U);});_tmp2E7.f1=_tmp34B;});_tmp2E7;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp109=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E6;_tmp2E6.tag=1U,_tmp2E6.f1=q;_tmp2E6;});void*_tmp107[2U];_tmp107[0]=& _tmp108,_tmp107[1]=& _tmp109;({unsigned _tmp34C=e->loc;Cyc_Warn_err2(_tmp34C,_tag_fat(_tmp107,sizeof(void*),2U));});});_npop_handler(0U);return;}}_LL5A:;}
# 379
;_pop_handler();}else{void*_tmpF8=(void*)Cyc_Core_get_exn_thrown();void*_tmp10B=_tmpF8;void*_tmp10C;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp10B)->tag == Cyc_Binding_BindingError){_LL66: _LL67:
# 387
({struct Cyc_Warn_String_Warn_Warg_struct _tmp10E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9;_tmp2E9.tag=0U,({struct _fat_ptr _tmp34D=({const char*_tmp110="undeclared identifier ";_tag_fat(_tmp110,sizeof(char),23U);});_tmp2E9.f1=_tmp34D;});_tmp2E9;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp10F=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2E8;_tmp2E8.tag=1U,_tmp2E8.f1=q;_tmp2E8;});void*_tmp10D[2U];_tmp10D[0]=& _tmp10E,_tmp10D[1]=& _tmp10F;({unsigned _tmp34E=e->loc;Cyc_Warn_err2(_tmp34E,_tag_fat(_tmp10D,sizeof(void*),2U));});});return;}else{_LL68: _tmp10C=_tmp10B;_LL69: {void*exn=_tmp10C;(int)_rethrow(exn);}}_LL65:;}}}}else{_LL58: _LL59:
# 389
 return;}_LL55:;}case 10U: _LL3: _tmpF0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpF1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpF2=(int*)&((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL4: {struct Cyc_Absyn_Exp*e1=_tmpF0;struct Cyc_List_List*es=_tmpF1;int*b=_tmpF2;
# 392
*b=1;
# 394
{struct Cyc_List_List*_tmp111=es;struct Cyc_List_List*es2=_tmp111;for(0;es2 != 0;es2=es2->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es2->hd);}}{
void*_tmp112=e1->r;void*_stmttmpB=_tmp112;void*_tmp113=_stmttmpB;void**_tmp114;if(((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp113)->tag == 1U){_LL6B: _tmp114=(void**)&((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp113)->f1;_LL6C: {void**b=_tmp114;
# 398
void*_tmp115=*b;void*_stmttmpC=_tmp115;void*_tmp116=_stmttmpC;struct _tuple0*_tmp117;if(((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp116)->tag == 0U){_LL70: _tmp117=((struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct*)_tmp116)->f1;_LL71: {struct _tuple0*q=_tmp117;
# 400
struct _handler_cons _tmp118;_push_handler(& _tmp118);{int _tmp11A=0;if(setjmp(_tmp118.handler))_tmp11A=1;if(!_tmp11A){{void*_tmp11B=Cyc_Binding_lookup_ordinary(e1->loc,env,q);void*_stmttmpD=_tmp11B;void*_tmp11C=_stmttmpD;struct Cyc_Absyn_Datatypefield*_tmp11E;struct Cyc_Absyn_Datatypedecl*_tmp11D;struct Cyc_Absyn_Aggrdecl*_tmp11F;void*_tmp120;switch(*((int*)_tmp11C)){case 0U: _LL75: _tmp120=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp11C)->f1;_LL76: {void*bnd=_tmp120;
# 402
*b=bnd;_npop_handler(0U);return;}case 1U: _LL77: _tmp11F=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp11C)->f1;_LL78: {struct Cyc_Absyn_Aggrdecl*ad=_tmp11F;
# 404
struct Cyc_List_List*_tmp121=0;struct Cyc_List_List*dles=_tmp121;
for(0;es != 0;es=es->tl){
dles=({struct Cyc_List_List*_tmp123=_cycalloc(sizeof(*_tmp123));({struct _tuple11*_tmp34F=({struct _tuple11*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->f1=0,_tmp122->f2=(struct Cyc_Absyn_Exp*)es->hd;_tmp122;});_tmp123->hd=_tmp34F;}),_tmp123->tl=dles;_tmp123;});}
({void*_tmp351=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=29U,_tmp124->f1=ad->name,_tmp124->f2=0,({struct Cyc_List_List*_tmp350=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(dles);_tmp124->f3=_tmp350;}),_tmp124->f4=ad;_tmp124;});e->r=_tmp351;});
_npop_handler(0U);return;}case 2U: _LL79: _tmp11D=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp11C)->f1;_tmp11E=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp11C)->f2;_LL7A: {struct Cyc_Absyn_Datatypedecl*tud=_tmp11D;struct Cyc_Absyn_Datatypefield*tuf=_tmp11E;
# 410
if(tuf->typs == 0)
# 412
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp126=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2EB;_tmp2EB.tag=1U,_tmp2EB.f1=tuf->name;_tmp2EB;});struct Cyc_Warn_String_Warn_Warg_struct _tmp127=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EA;_tmp2EA.tag=0U,({struct _fat_ptr _tmp352=({const char*_tmp128=" is a constant, not a function";_tag_fat(_tmp128,sizeof(char),31U);});_tmp2EA.f1=_tmp352;});_tmp2EA;});void*_tmp125[2U];_tmp125[0]=& _tmp126,_tmp125[1]=& _tmp127;({unsigned _tmp353=e->loc;Cyc_Warn_err2(_tmp353,_tag_fat(_tmp125,sizeof(void*),2U));});});
({void*_tmp354=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=31U,_tmp129->f1=es,_tmp129->f2=tud,_tmp129->f3=tuf;_tmp129;});e->r=_tmp354;});
_npop_handler(0U);return;}case 4U: _LL7B: _LL7C:
 goto _LL7E;default: _LL7D: _LL7E:
# 417
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp12B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2ED;_tmp2ED.tag=1U,_tmp2ED.f1=q;_tmp2ED;});struct Cyc_Warn_String_Warn_Warg_struct _tmp12C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC;_tmp2EC.tag=0U,({struct _fat_ptr _tmp355=({const char*_tmp12D=" is an enum constructor, not a function";_tag_fat(_tmp12D,sizeof(char),40U);});_tmp2EC.f1=_tmp355;});_tmp2EC;});void*_tmp12A[2U];_tmp12A[0]=& _tmp12B,_tmp12A[1]=& _tmp12C;({unsigned _tmp356=e->loc;Cyc_Warn_err2(_tmp356,_tag_fat(_tmp12A,sizeof(void*),2U));});});_npop_handler(0U);return;}_LL74:;}
# 400
;_pop_handler();}else{void*_tmp119=(void*)Cyc_Core_get_exn_thrown();void*_tmp12E=_tmp119;void*_tmp12F;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp12E)->tag == Cyc_Binding_BindingError){_LL80: _LL81:
# 419
({struct Cyc_Warn_String_Warn_Warg_struct _tmp131=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF;_tmp2EF.tag=0U,({struct _fat_ptr _tmp357=({const char*_tmp133="undeclared identifier ";_tag_fat(_tmp133,sizeof(char),23U);});_tmp2EF.f1=_tmp357;});_tmp2EF;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp132=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2EE;_tmp2EE.tag=1U,_tmp2EE.f1=q;_tmp2EE;});void*_tmp130[2U];_tmp130[0]=& _tmp131,_tmp130[1]=& _tmp132;({unsigned _tmp358=e->loc;Cyc_Warn_err2(_tmp358,_tag_fat(_tmp130,sizeof(void*),2U));});});return;}else{_LL82: _tmp12F=_tmp12E;_LL83: {void*exn=_tmp12F;(int)_rethrow(exn);}}_LL7F:;}}}}else{_LL72: _LL73:
# 421
 return;}_LL6F:;}}else{_LL6D: _LL6E:
# 423
 Cyc_Binding_resolve_exp(env,e1);return;}_LL6A:;}}case 29U: _LL5: _tmpEC=(struct _tuple0**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpED=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpEE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpEF=(struct Cyc_Absyn_Aggrdecl**)&((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL6: {struct _tuple0**tn=_tmpEC;struct Cyc_List_List*ts=_tmpED;struct Cyc_List_List*dles=_tmpEE;struct Cyc_Absyn_Aggrdecl**adopt=_tmpEF;
# 427
for(0;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}{
struct _handler_cons _tmp134;_push_handler(& _tmp134);{int _tmp136=0;if(setjmp(_tmp134.handler))_tmp136=1;if(!_tmp136){
({struct Cyc_Absyn_Aggrdecl*_tmp359=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(e->loc,env->ns,*tn,Cyc_Binding_lookup_aggrdecl);*adopt=_tmp359;});
*tn=((struct Cyc_Absyn_Aggrdecl*)_check_null(*adopt))->name;
_npop_handler(0U);return;
# 430
;_pop_handler();}else{void*_tmp135=(void*)Cyc_Core_get_exn_thrown();void*_tmp137=_tmp135;void*_tmp138;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp137)->tag == Cyc_Binding_BindingError){_LL85: _LL86:
# 434
({struct Cyc_Warn_String_Warn_Warg_struct _tmp13A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F1;_tmp2F1.tag=0U,({struct _fat_ptr _tmp35A=({const char*_tmp13C="unbound struct/union name ";_tag_fat(_tmp13C,sizeof(char),27U);});_tmp2F1.f1=_tmp35A;});_tmp2F1;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp13B=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F0;_tmp2F0.tag=1U,_tmp2F0.f1=*tn;_tmp2F0;});void*_tmp139[2U];_tmp139[0]=& _tmp13A,_tmp139[1]=& _tmp13B;({unsigned _tmp35B=e->loc;Cyc_Warn_err2(_tmp35B,_tag_fat(_tmp139,sizeof(void*),2U));});});
return;}else{_LL87: _tmp138=_tmp137;_LL88: {void*exn=_tmp138;(int)_rethrow(exn);}}_LL84:;}}}}case 27U: _LL7: _tmpE9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpEA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpEB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL8: {struct Cyc_Absyn_Vardecl*vd=_tmpE9;struct Cyc_Absyn_Exp*e1=_tmpEA;struct Cyc_Absyn_Exp*e2=_tmpEB;
# 439
Cyc_Binding_resolve_exp(env,e1);
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Dict_Dict*_tmp35D=({struct Cyc_Dict_Dict*_tmp13D=_cycalloc(sizeof(*_tmp13D));({struct Cyc_Dict_Dict _tmp35C=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp13D=_tmp35C;});_tmp13D;});env->local_vars=_tmp35D;});
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
env->local_vars=0;
return;}{
# 447
struct Cyc_Dict_Dict _tmp13E=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmp13E;
Cyc_Binding_resolve_and_add_vardecl(e->loc,env,vd);
Cyc_Binding_resolve_exp(env,e2);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;
return;}}case 36U: _LL9: _tmpE8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLA: {struct Cyc_List_List*dles=_tmpE8;
# 455
for(0;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}case 28U: _LLB: _tmpE6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpE7=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmpE6;void*t=_tmpE7;
# 461
Cyc_Binding_resolve_exp(env,e1);
Cyc_Binding_resolve_type(e->loc,env,t);
return;}case 2U: _LLD: _LLE:
# 465
 goto _LL10;case 0U: _LLF: _LL10:
 return;case 24U: _LL11: _tmpE5=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL12: {struct Cyc_List_List*es=_tmpE5;
# 468
_tmpE4=es;goto _LL14;}case 3U: _LL13: _tmpE4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL14: {struct Cyc_List_List*es=_tmpE4;
# 470
for(0;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 41U: _LL15: _tmpE3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpE3;
# 474
_tmpE2=e1;goto _LL18;}case 38U: _LL17: _tmpE2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpE2;
_tmpE1=e1;goto _LL1A;}case 12U: _LL19: _tmpE1=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpE1;
_tmpE0=e1;goto _LL1C;}case 18U: _LL1B: _tmpE0=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpE0;
_tmpDF=e1;goto _LL1E;}case 11U: _LL1D: _tmpDF=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpDF;
_tmpDE=e1;goto _LL20;}case 5U: _LL1F: _tmpDE=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL20: {struct Cyc_Absyn_Exp*e1=_tmpDE;
_tmpDD=e1;goto _LL22;}case 22U: _LL21: _tmpDD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL22: {struct Cyc_Absyn_Exp*e1=_tmpDD;
_tmpDC=e1;goto _LL24;}case 21U: _LL23: _tmpDC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpDC;
_tmpDB=e1;goto _LL26;}case 15U: _LL25: _tmpDB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL26: {struct Cyc_Absyn_Exp*e1=_tmpDB;
_tmpDA=e1;goto _LL28;}case 20U: _LL27: _tmpDA=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpDA;
Cyc_Binding_resolve_exp(env,e1);return;}case 35U: _LL29: _tmpD8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2A: {struct Cyc_Absyn_Exp*e1=_tmpD8;struct Cyc_Absyn_Exp*e2=_tmpD9;
# 485
_tmpD6=e1;_tmpD7=e2;goto _LL2C;}case 9U: _LL2B: _tmpD6=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpD6;struct Cyc_Absyn_Exp*e2=_tmpD7;
_tmpD4=e1;_tmpD5=e2;goto _LL2E;}case 4U: _LL2D: _tmpD4=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD5=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpD4;struct Cyc_Absyn_Exp*e2=_tmpD5;
_tmpD2=e1;_tmpD3=e2;goto _LL30;}case 23U: _LL2F: _tmpD2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL30: {struct Cyc_Absyn_Exp*e1=_tmpD2;struct Cyc_Absyn_Exp*e2=_tmpD3;
_tmpD0=e1;_tmpD1=e2;goto _LL32;}case 7U: _LL31: _tmpD0=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD1=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpD0;struct Cyc_Absyn_Exp*e2=_tmpD1;
_tmpCE=e1;_tmpCF=e2;goto _LL34;}case 8U: _LL33: _tmpCE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCF=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpCE;struct Cyc_Absyn_Exp*e2=_tmpCF;
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);return;}case 34U: _LL35: _tmpCB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).rgn;_tmpCC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).elt_type;_tmpCD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).num_elts;_LL36: {struct Cyc_Absyn_Exp*eo=_tmpCB;void**to=_tmpCC;struct Cyc_Absyn_Exp*e1=_tmpCD;
# 493
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
if(to != 0)Cyc_Binding_resolve_type(e->loc,env,*to);
Cyc_Binding_resolve_exp(env,e1);
return;}case 16U: _LL37: _tmpC9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL38: {struct Cyc_Absyn_Exp*eo=_tmpC9;struct Cyc_Absyn_Exp*e2=_tmpCA;
# 499
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
Cyc_Binding_resolve_exp(env,e2);
return;}case 6U: _LL39: _tmpC6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpC8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL3A: {struct Cyc_Absyn_Exp*e1=_tmpC6;struct Cyc_Absyn_Exp*e2=_tmpC7;struct Cyc_Absyn_Exp*e3=_tmpC8;
# 504
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_exp(env,e2);Cyc_Binding_resolve_exp(env,e3);return;}case 13U: _LL3B: _tmpC4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL3C: {struct Cyc_Absyn_Exp*e1=_tmpC4;struct Cyc_List_List*ts=_tmpC5;
# 507
Cyc_Binding_resolve_exp(env,e1);
for(0;ts != 0;ts=ts->tl){
Cyc_Binding_resolve_type(e->loc,env,(void*)ts->hd);}
return;}case 14U: _LL3D: _tmpC2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL3E: {void*t=_tmpC2;struct Cyc_Absyn_Exp*e1=_tmpC3;
# 513
Cyc_Binding_resolve_exp(env,e1);Cyc_Binding_resolve_type(e->loc,env,t);return;}case 37U: _LL3F: _tmpC1=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL40: {struct Cyc_Absyn_Stmt*s=_tmpC1;
# 515
Cyc_Binding_resolve_stmt(env,s);return;}case 39U: _LL41: _tmpC0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL42: {void*t=_tmpC0;
# 517
_tmpBF=t;goto _LL44;}case 19U: _LL43: _tmpBF=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL44: {void*t=_tmpBF;
_tmpBE=t;goto _LL46;}case 17U: _LL45: _tmpBE=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL46: {void*t=_tmpBE;
Cyc_Binding_resolve_type(e->loc,env,t);return;}case 25U: _LL47: _tmpBC=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1)->f3;_tmpBD=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL48: {void*t=_tmpBC;struct Cyc_List_List*dles=_tmpBD;
# 522
Cyc_Binding_resolve_type(e->loc,env,t);
_tmpBB=dles;goto _LL4A;}case 26U: _LL49: _tmpBB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL4A: {struct Cyc_List_List*dles=_tmpBB;
# 526
for(0;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}case 33U: _LL4B: _tmpB9=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpBA=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4C: {void*t=_tmpB9;struct Cyc_Absyn_Enumfield*ef=_tmpBA;
# 531
Cyc_Binding_resolve_type(e->loc,env,t);return;}case 32U: _LL4D: _tmpB7=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB8=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4E: {struct Cyc_Absyn_Enumdecl*ed=_tmpB7;struct Cyc_Absyn_Enumfield*ef=_tmpB8;
return;}case 31U: _LL4F: _tmpB4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpB6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL50: {struct Cyc_List_List*es=_tmpB4;struct Cyc_Absyn_Datatypedecl*tud=_tmpB5;struct Cyc_Absyn_Datatypefield*tuf=_tmpB6;
# 534
for(0;es != 0;es=es->tl){
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)es->hd);}
return;}case 30U: _LL51: _tmpB2=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL52: {void*t=_tmpB2;struct Cyc_List_List*dles=_tmpB3;
# 538
Cyc_Binding_resolve_type(e->loc,env,t);
for(0;dles != 0;dles=dles->tl){
Cyc_Binding_resolve_exp(env,(*((struct _tuple11*)dles->hd)).f2);}
return;}default: _LL53: _tmpAD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpAE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpAF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpB0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_tmpB1=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f5;_LL54: {int v=_tmpAD;struct _fat_ptr t=_tmpAE;struct Cyc_List_List*o=_tmpAF;struct Cyc_List_List*i=_tmpB0;struct Cyc_List_List*c=_tmpB1;
# 544
Cyc_Binding_resolve_asm_iolist(env,o);
Cyc_Binding_resolve_asm_iolist(env,i);
goto _LL0;}}_LL0:;}struct _tuple12{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};
# 550
void Cyc_Binding_resolve_asm_iolist(struct Cyc_Binding_Env*env,struct Cyc_List_List*l){
while((unsigned)l){
struct _tuple12*_tmp13F=(struct _tuple12*)l->hd;struct _tuple12*_stmttmpE=_tmp13F;struct _tuple12*_tmp140=_stmttmpE;struct Cyc_Absyn_Exp*_tmp141;_LL1: _tmp141=_tmp140->f2;_LL2: {struct Cyc_Absyn_Exp*e=_tmp141;
Cyc_Binding_resolve_exp(env,e);
l=l->tl;}}}
# 558
void Cyc_Binding_resolve_scs(struct Cyc_Binding_Env*env,struct Cyc_List_List*scs){
struct Cyc_Dict_Dict _tmp142=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict old_locals=_tmp142;
for(0;scs != 0;scs=scs->tl){
struct Cyc_Absyn_Switch_clause*_tmp143=(struct Cyc_Absyn_Switch_clause*)scs->hd;struct Cyc_Absyn_Switch_clause*_stmttmpF=_tmp143;struct Cyc_Absyn_Switch_clause*_tmp144=_stmttmpF;struct Cyc_Absyn_Stmt*_tmp147;struct Cyc_Absyn_Exp*_tmp146;struct Cyc_Absyn_Pat*_tmp145;_LL1: _tmp145=_tmp144->pattern;_tmp146=_tmp144->where_clause;_tmp147=_tmp144->body;_LL2: {struct Cyc_Absyn_Pat*pattern=_tmp145;struct Cyc_Absyn_Exp*where_clause=_tmp146;struct Cyc_Absyn_Stmt*body=_tmp147;
Cyc_Binding_resolve_pat(env,pattern);
if(where_clause != 0)
Cyc_Binding_resolve_exp(env,where_clause);
Cyc_Binding_resolve_stmt(env,body);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=old_locals;}}
# 568
return;}
# 570
void Cyc_Binding_resolve_aggrfields(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*fs){
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Aggrfield*_tmp148=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*_stmttmp10=_tmp148;struct Cyc_Absyn_Aggrfield*_tmp149=_stmttmp10;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_List_List*_tmp14E;struct Cyc_Absyn_Exp*_tmp14D;void*_tmp14C;struct Cyc_Absyn_Tqual _tmp14B;struct _fat_ptr*_tmp14A;_LL1: _tmp14A=_tmp149->name;_tmp14B=_tmp149->tq;_tmp14C=_tmp149->type;_tmp14D=_tmp149->width;_tmp14E=_tmp149->attributes;_tmp14F=_tmp149->requires_clause;_LL2: {struct _fat_ptr*fn=_tmp14A;struct Cyc_Absyn_Tqual tq=_tmp14B;void*t=_tmp14C;struct Cyc_Absyn_Exp*width=_tmp14D;struct Cyc_List_List*atts=_tmp14E;struct Cyc_Absyn_Exp*requires_clause=_tmp14F;
Cyc_Binding_resolve_type(loc,env,t);
if(width != 0)
Cyc_Binding_resolve_exp(env,width);
if(requires_clause != 0)
Cyc_Binding_resolve_exp(env,requires_clause);}}
# 579
return;}
# 582
struct Cyc_List_List*Cyc_Binding_get_fun_vardecls(int need_va_name,unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*args,struct Cyc_Absyn_VarargInfo*vai){
# 588
struct Cyc_List_List*_tmp150=0;struct Cyc_List_List*vds=_tmp150;
for(0;args != 0;args=args->tl){
struct _tuple8*_tmp151=(struct _tuple8*)args->hd;struct _tuple8*_stmttmp11=_tmp151;struct _tuple8*_tmp152=_stmttmp11;void*_tmp155;struct Cyc_Absyn_Tqual _tmp154;struct _fat_ptr*_tmp153;_LL1: _tmp153=_tmp152->f1;_tmp154=_tmp152->f2;_tmp155=_tmp152->f3;_LL2: {struct _fat_ptr*a=_tmp153;struct Cyc_Absyn_Tqual tq=_tmp154;void*t=_tmp155;
if(a == 0)
continue;{
struct Cyc_Absyn_Vardecl*_tmp156=({struct _tuple0*_tmp35E=({struct _tuple0*_tmp158=_cycalloc(sizeof(*_tmp158));_tmp158->f1=Cyc_Absyn_Loc_n,_tmp158->f2=a;_tmp158;});Cyc_Absyn_new_vardecl(0U,_tmp35E,t,0);});struct Cyc_Absyn_Vardecl*vd=_tmp156;
vd->tq=tq;
vds=({struct Cyc_List_List*_tmp157=_cycalloc(sizeof(*_tmp157));_tmp157->hd=vd,_tmp157->tl=vds;_tmp157;});}}}
# 597
if(vai != 0){
struct Cyc_Absyn_VarargInfo _tmp159=*vai;struct Cyc_Absyn_VarargInfo _stmttmp12=_tmp159;struct Cyc_Absyn_VarargInfo _tmp15A=_stmttmp12;int _tmp15E;void*_tmp15D;struct Cyc_Absyn_Tqual _tmp15C;struct _fat_ptr*_tmp15B;_LL4: _tmp15B=_tmp15A.name;_tmp15C=_tmp15A.tq;_tmp15D=_tmp15A.type;_tmp15E=_tmp15A.inject;_LL5: {struct _fat_ptr*v=_tmp15B;struct Cyc_Absyn_Tqual tq=_tmp15C;void*t=_tmp15D;int i=_tmp15E;
if(v == 0){
if(need_va_name)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp160=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F2;_tmp2F2.tag=0U,({struct _fat_ptr _tmp35F=({const char*_tmp161="missing name for vararg";_tag_fat(_tmp161,sizeof(char),24U);});_tmp2F2.f1=_tmp35F;});_tmp2F2;});void*_tmp15F[1U];_tmp15F[0]=& _tmp160;({unsigned _tmp360=loc;Cyc_Warn_err2(_tmp360,_tag_fat(_tmp15F,sizeof(void*),1U));});});}else{
# 604
void*_tmp162=Cyc_Absyn_fatptr_type(t,Cyc_Absyn_heap_rgn_type,tq,Cyc_Absyn_false_type);void*typ=_tmp162;
struct Cyc_Absyn_Vardecl*_tmp163=({struct _tuple0*_tmp361=({struct _tuple0*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->f1=Cyc_Absyn_Loc_n,_tmp165->f2=v;_tmp165;});Cyc_Absyn_new_vardecl(0U,_tmp361,typ,0);});struct Cyc_Absyn_Vardecl*vd=_tmp163;
vds=({struct Cyc_List_List*_tmp164=_cycalloc(sizeof(*_tmp164));_tmp164->hd=vd,_tmp164->tl=vds;_tmp164;});}}}
# 609
vds=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(vds);
return vds;}
# 613
void Cyc_Binding_resolve_function_stuff(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_Absyn_FnInfo i){
if(i.effect != 0)
Cyc_Binding_resolve_type(loc,env,i.effect);
Cyc_Binding_resolve_type(loc,env,i.ret_type);
{struct Cyc_List_List*_tmp166=i.args;struct Cyc_List_List*args=_tmp166;for(0;args != 0;args=args->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple8*)args->hd)).f3);}}
if(i.cyc_varargs != 0)
Cyc_Binding_resolve_type(loc,env,(i.cyc_varargs)->type);
Cyc_Binding_resolve_rgnpo(loc,env,i.rgn_po);
if(i.requires_clause != 0)
Cyc_Binding_resolve_exp(env,i.requires_clause);
if(i.ensures_clause != 0){
# 626
struct Cyc_Dict_Dict _tmp167=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct Cyc_Dict_Dict locs=_tmp167;
struct _fat_ptr*_tmp168=({struct _fat_ptr*_tmp16E=_cycalloc(sizeof(*_tmp16E));({struct _fat_ptr _tmp362=({const char*_tmp16D="return_value";_tag_fat(_tmp16D,sizeof(char),13U);});*_tmp16E=_tmp362;});_tmp16E;});struct _fat_ptr*v=_tmp168;
struct Cyc_Absyn_Vardecl*_tmp169=({struct _tuple0*_tmp363=({struct _tuple0*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->f1=Cyc_Absyn_Loc_n,_tmp16C->f2=v;_tmp16C;});Cyc_Absyn_new_vardecl(0U,_tmp363,i.ret_type,0);});struct Cyc_Absyn_Vardecl*vd=_tmp169;
({struct Cyc_Dict_Dict _tmp367=({struct Cyc_Dict_Dict _tmp366=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp365=v;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp366,_tmp365,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp16B=_cycalloc(sizeof(*_tmp16B));
_tmp16B->tag=0U,({void*_tmp364=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->tag=4U,_tmp16A->f1=vd;_tmp16A;});_tmp16B->f1=_tmp364;});_tmp16B;}));});
# 629
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp367;});
# 631
Cyc_Binding_resolve_exp(env,i.ensures_clause);
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=locs;}}struct _tuple13{struct Cyc_Absyn_Tqual f1;void*f2;};
# 641
void Cyc_Binding_resolve_type(unsigned loc,struct Cyc_Binding_Env*env,void*t){
void*_tmp16F=t;struct Cyc_Absyn_Exp*_tmp170;struct Cyc_Absyn_Exp*_tmp171;struct Cyc_Absyn_FnInfo _tmp172;struct Cyc_Absyn_Exp*_tmp174;void*_tmp173;struct Cyc_List_List*_tmp175;void*_tmp178;void*_tmp177;void*_tmp176;void*_tmp179;struct Cyc_List_List*_tmp17A;void***_tmp17C;struct Cyc_Absyn_TypeDecl*_tmp17B;struct Cyc_Absyn_Typedefdecl**_tmp17F;struct Cyc_List_List*_tmp17E;struct _tuple0**_tmp17D;struct Cyc_List_List*_tmp181;void*_tmp180;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_Enumdecl*_tmp184;struct _tuple0**_tmp183;struct Cyc_List_List*_tmp186;union Cyc_Absyn_AggrInfo*_tmp185;struct Cyc_List_List*_tmp188;union Cyc_Absyn_DatatypeFieldInfo*_tmp187;struct Cyc_List_List*_tmp18A;union Cyc_Absyn_DatatypeInfo*_tmp189;switch(*((int*)_tmp16F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)){case 18U: _LL1: _tmp189=(union Cyc_Absyn_DatatypeInfo*)&((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)->f1;_tmp18A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2;_LL2: {union Cyc_Absyn_DatatypeInfo*info=_tmp189;struct Cyc_List_List*targs=_tmp18A;
# 644
for(0;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeInfo _tmp18B=*info;union Cyc_Absyn_DatatypeInfo _stmttmp13=_tmp18B;union Cyc_Absyn_DatatypeInfo _tmp18C=_stmttmp13;int _tmp18E;struct _tuple0*_tmp18D;if((_tmp18C.UnknownDatatype).tag == 1){_LL26: _tmp18D=((_tmp18C.UnknownDatatype).val).name;_tmp18E=((_tmp18C.UnknownDatatype).val).is_extensible;_LL27: {struct _tuple0*qv=_tmp18D;int b=_tmp18E;
# 648
struct _handler_cons _tmp18F;_push_handler(& _tmp18F);{int _tmp191=0;if(setjmp(_tmp18F.handler))_tmp191=1;if(!_tmp191){
{struct Cyc_Absyn_Datatypedecl*_tmp192=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_datatypedecl))->hd;struct Cyc_Absyn_Datatypedecl*tud=_tmp192;
({union Cyc_Absyn_DatatypeInfo _tmp368=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp2F3;_tmp2F3.name=tud->name,_tmp2F3.is_extensible=b;_tmp2F3;}));*info=_tmp368;});
_npop_handler(0U);return;}
# 649
;_pop_handler();}else{void*_tmp190=(void*)Cyc_Core_get_exn_thrown();void*_tmp193=_tmp190;void*_tmp194;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp193)->tag == Cyc_Binding_BindingError){_LL2B: _LL2C:
# 653
 Cyc_Binding_absolutize_topdecl(loc,env,qv,Cyc_Absyn_Public);return;}else{_LL2D: _tmp194=_tmp193;_LL2E: {void*exn=_tmp194;(int)_rethrow(exn);}}_LL2A:;}}}}else{_LL28: _LL29:
 return;}_LL25:;}}case 19U: _LL3: _tmp187=(union Cyc_Absyn_DatatypeFieldInfo*)&((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)->f1;_tmp188=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2;_LL4: {union Cyc_Absyn_DatatypeFieldInfo*info=_tmp187;struct Cyc_List_List*targs=_tmp188;
# 657
for(0;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_DatatypeFieldInfo _tmp195=*info;union Cyc_Absyn_DatatypeFieldInfo _stmttmp14=_tmp195;union Cyc_Absyn_DatatypeFieldInfo _tmp196=_stmttmp14;int _tmp199;struct _tuple0*_tmp198;struct _tuple0*_tmp197;if((_tmp196.UnknownDatatypefield).tag == 1){_LL30: _tmp197=((_tmp196.UnknownDatatypefield).val).datatype_name;_tmp198=((_tmp196.UnknownDatatypefield).val).field_name;_tmp199=((_tmp196.UnknownDatatypefield).val).is_extensible;_LL31: {struct _tuple0*qvd=_tmp197;struct _tuple0*qvf=_tmp198;int b=_tmp199;
# 663
{struct _handler_cons _tmp19A;_push_handler(& _tmp19A);{int _tmp19C=0;if(setjmp(_tmp19A.handler))_tmp19C=1;if(!_tmp19C){
{void*_tmp19D=Cyc_Binding_lookup_ordinary(loc,env,qvf);void*_stmttmp15=_tmp19D;void*_tmp19E=_stmttmp15;struct Cyc_Absyn_Datatypefield*_tmp1A0;struct Cyc_Absyn_Datatypedecl*_tmp19F;if(((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp19E)->tag == 2U){_LL35: _tmp19F=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp19E)->f1;_tmp1A0=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp19E)->f2;_LL36: {struct Cyc_Absyn_Datatypedecl*tud=_tmp19F;struct Cyc_Absyn_Datatypefield*tuf=_tmp1A0;
# 666
struct Cyc_Absyn_Datatypedecl*_tmp1A1=(struct Cyc_Absyn_Datatypedecl*)(((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qvd,Cyc_Binding_lookup_datatypedecl))->hd;struct Cyc_Absyn_Datatypedecl*tud2=_tmp1A1;
if(Cyc_Absyn_qvar_cmp(tud->name,tud2->name)!= 0){
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A3=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F9;_tmp2F9.tag=1U,_tmp2F9.f1=tuf->name;_tmp2F9;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F8;_tmp2F8.tag=0U,({struct _fat_ptr _tmp369=({const char*_tmp1AB=" is a variant of ";_tag_fat(_tmp1AB,sizeof(char),18U);});_tmp2F8.f1=_tmp369;});_tmp2F8;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A5=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F7;_tmp2F7.tag=1U,_tmp2F7.f1=tud2->name;_tmp2F7;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F6;_tmp2F6.tag=0U,({struct _fat_ptr _tmp36A=({const char*_tmp1AA=" not ";_tag_fat(_tmp1AA,sizeof(char),6U);});_tmp2F6.f1=_tmp36A;});_tmp2F6;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1A7=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2F5;_tmp2F5.tag=1U,_tmp2F5.f1=tud->name;_tmp2F5;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F4;_tmp2F4.tag=0U,({
struct _fat_ptr _tmp36B=({const char*_tmp1A9=" (shadowing not yet implemented properly)";_tag_fat(_tmp1A9,sizeof(char),42U);});_tmp2F4.f1=_tmp36B;});_tmp2F4;});void*_tmp1A2[6U];_tmp1A2[0]=& _tmp1A3,_tmp1A2[1]=& _tmp1A4,_tmp1A2[2]=& _tmp1A5,_tmp1A2[3]=& _tmp1A6,_tmp1A2[4]=& _tmp1A7,_tmp1A2[5]=& _tmp1A8;({unsigned _tmp36C=loc;Cyc_Warn_err2(_tmp36C,_tag_fat(_tmp1A2,sizeof(void*),6U));});});
_npop_handler(0U);return;}
# 672
({union Cyc_Absyn_DatatypeFieldInfo _tmp36D=Cyc_Absyn_UnknownDatatypefield(({struct Cyc_Absyn_UnknownDatatypeFieldInfo _tmp2FA;_tmp2FA.datatype_name=tud->name,_tmp2FA.field_name=tuf->name,_tmp2FA.is_extensible=b;_tmp2FA;}));*info=_tmp36D;});
# 674
_npop_handler(0U);return;}}else{_LL37: _LL38:
 goto _LL34;}_LL34:;}
# 664
;_pop_handler();}else{void*_tmp19B=(void*)Cyc_Core_get_exn_thrown();void*_tmp1AC=_tmp19B;void*_tmp1AD;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1AC)->tag == Cyc_Binding_BindingError){_LL3A: _LL3B:
# 677
 goto _LL39;}else{_LL3C: _tmp1AD=_tmp1AC;_LL3D: {void*exn=_tmp1AD;(int)_rethrow(exn);}}_LL39:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1AF=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FC;_tmp2FC.tag=1U,_tmp2FC.f1=qvf;_tmp2FC;});struct Cyc_Warn_String_Warn_Warg_struct _tmp1B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FB;_tmp2FB.tag=0U,({struct _fat_ptr _tmp36E=({const char*_tmp1B1=" is not a datatype field";_tag_fat(_tmp1B1,sizeof(char),25U);});_tmp2FB.f1=_tmp36E;});_tmp2FB;});void*_tmp1AE[2U];_tmp1AE[0]=& _tmp1AF,_tmp1AE[1]=& _tmp1B0;({unsigned _tmp36F=loc;Cyc_Warn_err2(_tmp36F,_tag_fat(_tmp1AE,sizeof(void*),2U));});});return;}}else{_LL32: _LL33:
 return;}_LL2F:;}}case 20U: _LL5: _tmp185=(union Cyc_Absyn_AggrInfo*)&((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)->f1;_tmp186=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2;_LL6: {union Cyc_Absyn_AggrInfo*info=_tmp185;struct Cyc_List_List*targs=_tmp186;
# 683
for(0;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}{
union Cyc_Absyn_AggrInfo _tmp1B2=*info;union Cyc_Absyn_AggrInfo _stmttmp16=_tmp1B2;union Cyc_Absyn_AggrInfo _tmp1B3=_stmttmp16;struct Cyc_Core_Opt*_tmp1B6;struct _tuple0*_tmp1B5;enum Cyc_Absyn_AggrKind _tmp1B4;if((_tmp1B3.UnknownAggr).tag == 1){_LL3F: _tmp1B4=((_tmp1B3.UnknownAggr).val).f1;_tmp1B5=((_tmp1B3.UnknownAggr).val).f2;_tmp1B6=((_tmp1B3.UnknownAggr).val).f3;_LL40: {enum Cyc_Absyn_AggrKind ak=_tmp1B4;struct _tuple0*qv=_tmp1B5;struct Cyc_Core_Opt*bo=_tmp1B6;
# 687
struct _handler_cons _tmp1B7;_push_handler(& _tmp1B7);{int _tmp1B9=0;if(setjmp(_tmp1B7.handler))_tmp1B9=1;if(!_tmp1B9){
{struct Cyc_Absyn_Aggrdecl*_tmp1BA=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,qv,Cyc_Binding_lookup_aggrdecl);struct Cyc_Absyn_Aggrdecl*ad=_tmp1BA;
if((int)ad->kind != (int)ak)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FD;_tmp2FD.tag=0U,({struct _fat_ptr _tmp370=({const char*_tmp1BD="struct vs. union mismatch with earlier declaration";_tag_fat(_tmp1BD,sizeof(char),51U);});_tmp2FD.f1=_tmp370;});_tmp2FD;});void*_tmp1BB[1U];_tmp1BB[0]=& _tmp1BC;({unsigned _tmp371=loc;Cyc_Warn_err2(_tmp371,_tag_fat(_tmp1BB,sizeof(void*),1U));});});
if((((int)ak == (int)1U && bo != 0)&& ad->impl != 0)&&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged != (int)bo->v)
# 693
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FE;_tmp2FE.tag=0U,({struct _fat_ptr _tmp372=({const char*_tmp1C0="@tagged mismatch with earlier declaration";_tag_fat(_tmp1C0,sizeof(char),42U);});_tmp2FE.f1=_tmp372;});_tmp2FE;});void*_tmp1BE[1U];_tmp1BE[0]=& _tmp1BF;({unsigned _tmp373=loc;Cyc_Warn_err2(_tmp373,_tag_fat(_tmp1BE,sizeof(void*),1U));});});
({union Cyc_Absyn_AggrInfo _tmp374=Cyc_Absyn_UnknownAggr(ak,ad->name,bo);*info=_tmp374;});
_npop_handler(0U);return;}
# 688
;_pop_handler();}else{void*_tmp1B8=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C1=_tmp1B8;void*_tmp1C2;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1C1)->tag == Cyc_Binding_BindingError){_LL44: _LL45:
# 697
 Cyc_Binding_absolutize_topdecl(loc,env,qv,Cyc_Absyn_Public);return;}else{_LL46: _tmp1C2=_tmp1C1;_LL47: {void*exn=_tmp1C2;(int)_rethrow(exn);}}_LL43:;}}}}else{_LL41: _LL42:
 return;}_LL3E:;}}case 15U: _LL7: _tmp183=(struct _tuple0**)&((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)->f1;_tmp184=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)->f2;_LL8: {struct _tuple0**qv=_tmp183;struct Cyc_Absyn_Enumdecl*edo=_tmp184;
# 701
if(edo != 0)
return;{
struct _handler_cons _tmp1C3;_push_handler(& _tmp1C3);{int _tmp1C5=0;if(setjmp(_tmp1C3.handler))_tmp1C5=1;if(!_tmp1C5){
{struct Cyc_Absyn_Enumdecl*_tmp1C6=((struct Cyc_Absyn_Enumdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Enumdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*qv,Cyc_Binding_lookup_enumdecl);struct Cyc_Absyn_Enumdecl*ed=_tmp1C6;
*qv=ed->name;
_npop_handler(0U);return;}
# 704
;_pop_handler();}else{void*_tmp1C4=(void*)Cyc_Core_get_exn_thrown();void*_tmp1C7=_tmp1C4;void*_tmp1C8;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1C7)->tag == Cyc_Binding_BindingError){_LL49: _LL4A:
# 708
 Cyc_Binding_absolutize_topdecl(loc,env,*qv,Cyc_Absyn_Public);return;}else{_LL4B: _tmp1C8=_tmp1C7;_LL4C: {void*exn=_tmp1C8;(int)_rethrow(exn);}}_LL48:;}}}}case 16U: _LLF: _tmp182=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)->f1;_LL10: {struct Cyc_List_List*fs=_tmp182;
# 746
struct Cyc_Binding_ResolveNSEnv*_tmp1DF=({struct Cyc_Binding_NSCtxt*_tmp376=env->ns;((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(_tmp376,({struct Cyc_List_List*_tmp375=(env->ns)->curr_ns;Cyc_Absyn_Abs_n(_tmp375,
Cyc_Binding_in_cinclude(env));}));});
# 746
struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp1DF;
# 748
for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp1E0=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp1E0;
if(f->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
({struct Cyc_Dict_Dict _tmp379=({struct Cyc_Dict_Dict _tmp378=decl_ns_data->ordinaries;struct _fat_ptr*_tmp377=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp378,_tmp377,(void*)({struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->tag=4U,_tmp1E1->f1=t,_tmp1E1->f2=f;_tmp1E1;}));});decl_ns_data->ordinaries=_tmp379;});}
# 754
return;}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2 == 0){_LL15: _LL16:
# 758
 return;}else{_LL17: _tmp180=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1;_tmp181=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2;_LL18: {void*c=_tmp180;struct Cyc_List_List*ts=_tmp181;
# 760
for(0;ts != 0;ts=ts->tl){
Cyc_Binding_resolve_type(loc,env,(void*)ts->hd);}
return;}}}case 8U: _LL9: _tmp17D=(struct _tuple0**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16F)->f1;_tmp17E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16F)->f2;_tmp17F=(struct Cyc_Absyn_Typedefdecl**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp16F)->f3;_LLA: {struct _tuple0**tdn=_tmp17D;struct Cyc_List_List*targs=_tmp17E;struct Cyc_Absyn_Typedefdecl**tdo=_tmp17F;
# 711
for(0;targs != 0;targs=targs->tl){
Cyc_Binding_resolve_type(loc,env,(void*)targs->hd);}
{struct _handler_cons _tmp1C9;_push_handler(& _tmp1C9);{int _tmp1CB=0;if(setjmp(_tmp1C9.handler))_tmp1CB=1;if(!_tmp1CB){
{struct Cyc_Absyn_Typedefdecl*_tmp1CC=((struct Cyc_Absyn_Typedefdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Typedefdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,*tdn,Cyc_Binding_lookup_typedefdecl);struct Cyc_Absyn_Typedefdecl*td=_tmp1CC;
# 716
*tdn=td->name;
_npop_handler(0U);return;}
# 714
;_pop_handler();}else{void*_tmp1CA=(void*)Cyc_Core_get_exn_thrown();void*_tmp1CD=_tmp1CA;void*_tmp1CE;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp1CD)->tag == Cyc_Binding_BindingError){_LL4E: _LL4F:
# 718
 goto _LL4D;}else{_LL50: _tmp1CE=_tmp1CD;_LL51: {void*exn=_tmp1CE;(int)_rethrow(exn);}}_LL4D:;}}}
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp300;_tmp300.tag=0U,({struct _fat_ptr _tmp37A=({const char*_tmp1D2="unbound typdef name ";_tag_fat(_tmp1D2,sizeof(char),21U);});_tmp300.f1=_tmp37A;});_tmp300;});struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp1D1=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp2FF;_tmp2FF.tag=1U,_tmp2FF.f1=*tdn;_tmp2FF;});void*_tmp1CF[2U];_tmp1CF[0]=& _tmp1D0,_tmp1CF[1]=& _tmp1D1;({unsigned _tmp37B=loc;Cyc_Warn_err2(_tmp37B,_tag_fat(_tmp1CF,sizeof(void*),2U));});});
return;}case 10U: _LLB: _tmp17B=((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16F)->f1;_tmp17C=(void***)&((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp16F)->f2;_LLC: {struct Cyc_Absyn_TypeDecl*td=_tmp17B;void***to=_tmp17C;
# 724
struct Cyc_Dict_Dict*_tmp1D3=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp1D3;
env->local_vars=0;
{void*_tmp1D4=td->r;void*_stmttmp17=_tmp1D4;void*_tmp1D5=_stmttmp17;struct Cyc_Absyn_Datatypedecl*_tmp1D6;struct Cyc_Absyn_Enumdecl*_tmp1D7;struct Cyc_Absyn_Aggrdecl*_tmp1D8;switch(*((int*)_tmp1D5)){case 0U: _LL53: _tmp1D8=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)_tmp1D5)->f1;_LL54: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1D8;
# 728
({struct Cyc_Binding_Env*_tmp37D=env;Cyc_Binding_resolve_decl(_tmp37D,({struct Cyc_Absyn_Decl*_tmp1DA=_cycalloc(sizeof(*_tmp1DA));({void*_tmp37C=(void*)({struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*_tmp1D9=_cycalloc(sizeof(*_tmp1D9));_tmp1D9->tag=5U,_tmp1D9->f1=ad;_tmp1D9;});_tmp1DA->r=_tmp37C;}),_tmp1DA->loc=td->loc;_tmp1DA;}));});goto _LL52;}case 1U: _LL55: _tmp1D7=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)_tmp1D5)->f1;_LL56: {struct Cyc_Absyn_Enumdecl*ed=_tmp1D7;
# 730
({struct Cyc_Binding_Env*_tmp37F=env;Cyc_Binding_resolve_decl(_tmp37F,({struct Cyc_Absyn_Decl*_tmp1DC=_cycalloc(sizeof(*_tmp1DC));({void*_tmp37E=(void*)({struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*_tmp1DB=_cycalloc(sizeof(*_tmp1DB));_tmp1DB->tag=7U,_tmp1DB->f1=ed;_tmp1DB;});_tmp1DC->r=_tmp37E;}),_tmp1DC->loc=td->loc;_tmp1DC;}));});goto _LL52;}default: _LL57: _tmp1D6=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)_tmp1D5)->f1;_LL58: {struct Cyc_Absyn_Datatypedecl*tud=_tmp1D6;
# 732
({struct Cyc_Binding_Env*_tmp381=env;Cyc_Binding_resolve_decl(_tmp381,({struct Cyc_Absyn_Decl*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));({void*_tmp380=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp1DD=_cycalloc(sizeof(*_tmp1DD));_tmp1DD->tag=6U,_tmp1DD->f1=tud;_tmp1DD;});_tmp1DE->r=_tmp380;}),_tmp1DE->loc=td->loc;_tmp1DE;}));});goto _LL52;}}_LL52:;}
# 734
env->local_vars=old_locals;
return;}case 7U: _LLD: _tmp17A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp16F)->f2;_LLE: {struct Cyc_List_List*fs=_tmp17A;
# 741
Cyc_Binding_resolve_aggrfields(loc,env,fs);
return;}case 1U: _LL11: _tmp179=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp16F)->f2;_LL12: {void*to=_tmp179;
# 755
if(to != 0)Cyc_Binding_resolve_type(loc,env,to);return;}case 2U: _LL13: _LL14:
# 757
 goto _LL16;case 3U: _LL19: _tmp176=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16F)->f1).elt_type;_tmp177=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16F)->f1).ptr_atts).rgn;_tmp178=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp16F)->f1).ptr_atts).bounds;_LL1A: {void*t1=_tmp176;void*t2=_tmp177;void*bds=_tmp178;
# 765
Cyc_Binding_resolve_type(loc,env,t1);
Cyc_Binding_resolve_type(loc,env,t2);
Cyc_Binding_resolve_type(loc,env,bds);
return;}case 6U: _LL1B: _tmp175=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp16F)->f1;_LL1C: {struct Cyc_List_List*tqts=_tmp175;
# 771
for(0;tqts != 0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)tqts->hd)).f2);}
return;}case 4U: _LL1D: _tmp173=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16F)->f1).elt_type;_tmp174=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp16F)->f1).num_elts;_LL1E: {void*t1=_tmp173;struct Cyc_Absyn_Exp*eo=_tmp174;
# 775
Cyc_Binding_resolve_type(loc,env,t1);
if(eo != 0)Cyc_Binding_resolve_exp(env,eo);
return;}case 5U: _LL1F: _tmp172=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp16F)->f1;_LL20: {struct Cyc_Absyn_FnInfo i=_tmp172;
# 782
struct Cyc_List_List*_tmp1E2=Cyc_Binding_get_fun_vardecls(0,loc,env,i.args,i.cyc_varargs);struct Cyc_List_List*vds=_tmp1E2;
# 784
struct Cyc_Dict_Dict*_tmp1E3=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp1E3;
if(old_locals != 0)
({struct Cyc_Dict_Dict*_tmp382=({struct Cyc_Dict_Dict*_tmp1E4=_cycalloc(sizeof(*_tmp1E4));*_tmp1E4=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp1E4;});env->local_vars=_tmp382;});else{
# 788
({struct Cyc_Dict_Dict*_tmp384=({struct Cyc_Dict_Dict*_tmp1E5=_cycalloc(sizeof(*_tmp1E5));({struct Cyc_Dict_Dict _tmp383=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp1E5=_tmp383;});_tmp1E5;});env->local_vars=_tmp384;});}
{struct Cyc_List_List*_tmp1E6=vds;struct Cyc_List_List*vds1=_tmp1E6;for(0;vds1 != 0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _tmp388=({struct Cyc_Dict_Dict _tmp387=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp386=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp387,_tmp386,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));
_tmp1E8->tag=0U,({void*_tmp385=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp1E7=_cycalloc(sizeof(*_tmp1E7));_tmp1E7->tag=3U,_tmp1E7->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_tmp1E7;});_tmp1E8->f1=_tmp385;});_tmp1E8;}));});
# 790
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp388;});}}
# 793
Cyc_Binding_resolve_function_stuff(loc,env,i);
env->local_vars=old_locals;
return;}case 9U: _LL21: _tmp171=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp16F)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp171;
# 797
_tmp170=e;goto _LL24;}default: _LL23: _tmp170=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp16F)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp170;
Cyc_Binding_resolve_exp(env,e);return;}}_LL0:;}struct _tuple14{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};
# 802
void Cyc_Binding_resolve_pat(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Pat*p){
struct Cyc_Absyn_Pat*_tmp1E9=p;unsigned _tmp1EB;void*_tmp1EA;_LL1: _tmp1EA=_tmp1E9->r;_tmp1EB=_tmp1E9->loc;_LL2: {void*r=_tmp1EA;unsigned loc=_tmp1EB;
void*_tmp1EC=r;struct Cyc_List_List*_tmp1ED;struct Cyc_Absyn_Exp*_tmp1EE;struct Cyc_List_List*_tmp1EF;struct Cyc_Absyn_Pat*_tmp1F0;struct Cyc_Absyn_Vardecl*_tmp1F1;struct Cyc_Absyn_Vardecl*_tmp1F2;struct Cyc_Absyn_Pat*_tmp1F4;struct Cyc_Absyn_Vardecl*_tmp1F3;struct Cyc_Absyn_Pat*_tmp1F6;struct Cyc_Absyn_Vardecl*_tmp1F5;struct Cyc_List_List*_tmp1F7;struct Cyc_List_List*_tmp1F8;int _tmp1FC;struct Cyc_List_List*_tmp1FB;struct Cyc_List_List*_tmp1FA;struct _tuple0*_tmp1F9;int _tmp1FF;struct Cyc_List_List*_tmp1FE;struct _tuple0*_tmp1FD;struct _tuple0*_tmp200;switch(*((int*)_tmp1EC)){case 15U: _LL4: _tmp200=((struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_LL5: {struct _tuple0*qv=_tmp200;
# 806
{struct _handler_cons _tmp201;_push_handler(& _tmp201);{int _tmp203=0;if(setjmp(_tmp201.handler))_tmp203=1;if(!_tmp203){{void*_tmp204=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_stmttmp18=_tmp204;void*_tmp205=_stmttmp18;struct Cyc_Absyn_Enumfield*_tmp207;void*_tmp206;struct Cyc_Absyn_Enumfield*_tmp209;struct Cyc_Absyn_Enumdecl*_tmp208;struct Cyc_Absyn_Datatypefield*_tmp20B;struct Cyc_Absyn_Datatypedecl*_tmp20A;switch(*((int*)_tmp205)){case 0U: _LL2D: _LL2E:
 goto _LL2C;case 1U: _LL2F: _LL30:
# 809
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp301;_tmp301.tag=0U,({struct _fat_ptr _tmp389=({const char*_tmp20E="struct tag used without arguments in pattern";_tag_fat(_tmp20E,sizeof(char),45U);});_tmp301.f1=_tmp389;});_tmp301;});void*_tmp20C[1U];_tmp20C[0]=& _tmp20D;({unsigned _tmp38A=loc;Cyc_Warn_err2(_tmp38A,_tag_fat(_tmp20C,sizeof(void*),1U));});});_npop_handler(0U);return;case 2U: _LL31: _tmp20A=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp205)->f1;_tmp20B=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp205)->f2;_LL32: {struct Cyc_Absyn_Datatypedecl*tud=_tmp20A;struct Cyc_Absyn_Datatypefield*tuf=_tmp20B;
# 811
({void*_tmp38B=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp20F=_cycalloc(sizeof(*_tmp20F));_tmp20F->tag=8U,_tmp20F->f1=tud,_tmp20F->f2=tuf,_tmp20F->f3=0,_tmp20F->f4=0;_tmp20F;});p->r=_tmp38B;});_npop_handler(0U);return;}case 3U: _LL33: _tmp208=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp205)->f1;_tmp209=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp205)->f2;_LL34: {struct Cyc_Absyn_Enumdecl*ed=_tmp208;struct Cyc_Absyn_Enumfield*ef=_tmp209;
# 813
({void*_tmp38C=(void*)({struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->tag=13U,_tmp210->f1=ed,_tmp210->f2=ef;_tmp210;});p->r=_tmp38C;});_npop_handler(0U);return;}default: _LL35: _tmp206=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp205)->f1;_tmp207=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp205)->f2;_LL36: {void*t=_tmp206;struct Cyc_Absyn_Enumfield*ef=_tmp207;
# 815
({void*_tmp38D=(void*)({struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct*_tmp211=_cycalloc(sizeof(*_tmp211));_tmp211->tag=14U,_tmp211->f1=t,_tmp211->f2=ef;_tmp211;});p->r=_tmp38D;});_npop_handler(0U);return;}}_LL2C:;}
# 806
;_pop_handler();}else{void*_tmp202=(void*)Cyc_Core_get_exn_thrown();void*_tmp212=_tmp202;void*_tmp213;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp212)->tag == Cyc_Binding_BindingError){_LL38: _LL39:
# 816
 goto _LL37;}else{_LL3A: _tmp213=_tmp212;_LL3B: {void*exn=_tmp213;(int)_rethrow(exn);}}_LL37:;}}}{
struct Cyc_Absyn_Vardecl*_tmp214=Cyc_Absyn_new_vardecl(0U,qv,Cyc_Absyn_void_type,0);struct Cyc_Absyn_Vardecl*vd=_tmp214;
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
({void*_tmp38F=(void*)({struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*_tmp215=_cycalloc(sizeof(*_tmp215));_tmp215->tag=1U,_tmp215->f1=vd,({struct Cyc_Absyn_Pat*_tmp38E=Cyc_Absyn_new_pat((void*)& Cyc_Absyn_Wild_p_val,0U);_tmp215->f2=_tmp38E;});_tmp215;});p->r=_tmp38F;});
return;}}case 16U: _LL6: _tmp1FD=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_tmp1FE=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_tmp1FF=((struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_LL7: {struct _tuple0*qv=_tmp1FD;struct Cyc_List_List*ps=_tmp1FE;int dots=_tmp1FF;
# 823
{struct Cyc_List_List*_tmp216=ps;struct Cyc_List_List*ps2=_tmp216;for(0;ps2 != 0;ps2=ps2->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps2->hd);}}
{struct _handler_cons _tmp217;_push_handler(& _tmp217);{int _tmp219=0;if(setjmp(_tmp217.handler))_tmp219=1;if(!_tmp219){{void*_tmp21A=Cyc_Binding_lookup_ordinary(loc,env,qv);void*_stmttmp19=_tmp21A;void*_tmp21B=_stmttmp19;struct Cyc_Absyn_Datatypefield*_tmp21D;struct Cyc_Absyn_Datatypedecl*_tmp21C;struct Cyc_Absyn_Aggrdecl*_tmp21E;switch(*((int*)_tmp21B)){case 0U: _LL3D: _LL3E:
 goto _LL3C;case 1U: _LL3F: _tmp21E=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp21B)->f1;_LL40: {struct Cyc_Absyn_Aggrdecl*ad=_tmp21E;
# 828
struct Cyc_List_List*_tmp21F=0;struct Cyc_List_List*new_ps=_tmp21F;
for(0;ps != 0;ps=ps->tl){
new_ps=({struct Cyc_List_List*_tmp221=_cycalloc(sizeof(*_tmp221));({struct _tuple14*_tmp390=({struct _tuple14*_tmp220=_cycalloc(sizeof(*_tmp220));_tmp220->f1=0,_tmp220->f2=(struct Cyc_Absyn_Pat*)ps->hd;_tmp220;});_tmp221->hd=_tmp390;}),_tmp221->tl=new_ps;_tmp221;});}
({void*_tmp394=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp224=_cycalloc(sizeof(*_tmp224));_tmp224->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp393=({union Cyc_Absyn_AggrInfo*_tmp223=_cycalloc(sizeof(*_tmp223));({union Cyc_Absyn_AggrInfo _tmp392=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp222=_cycalloc(sizeof(*_tmp222));*_tmp222=ad;_tmp222;}));*_tmp223=_tmp392;});_tmp223;});_tmp224->f1=_tmp393;}),_tmp224->f2=0,({struct Cyc_List_List*_tmp391=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(new_ps);_tmp224->f3=_tmp391;}),_tmp224->f4=dots;_tmp224;});p->r=_tmp394;});
_npop_handler(0U);return;}case 2U: _LL41: _tmp21C=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp21B)->f1;_tmp21D=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp21B)->f2;_LL42: {struct Cyc_Absyn_Datatypedecl*tud=_tmp21C;struct Cyc_Absyn_Datatypefield*tuf=_tmp21D;
# 834
({void*_tmp395=(void*)({struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*_tmp225=_cycalloc(sizeof(*_tmp225));_tmp225->tag=8U,_tmp225->f1=tud,_tmp225->f2=tuf,_tmp225->f3=ps,_tmp225->f4=dots;_tmp225;});p->r=_tmp395;});_npop_handler(0U);return;}case 3U: _LL43: _LL44:
 goto _LL46;default: _LL45: _LL46:
# 837
({struct Cyc_Warn_String_Warn_Warg_struct _tmp227=({struct Cyc_Warn_String_Warn_Warg_struct _tmp302;_tmp302.tag=0U,({struct _fat_ptr _tmp396=({const char*_tmp228="enum tag used with arguments in pattern";_tag_fat(_tmp228,sizeof(char),40U);});_tmp302.f1=_tmp396;});_tmp302;});void*_tmp226[1U];_tmp226[0]=& _tmp227;({unsigned _tmp397=loc;Cyc_Warn_err2(_tmp397,_tag_fat(_tmp226,sizeof(void*),1U));});});_npop_handler(0U);return;}_LL3C:;}
# 825
;_pop_handler();}else{void*_tmp218=(void*)Cyc_Core_get_exn_thrown();void*_tmp229=_tmp218;void*_tmp22A;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp229)->tag == Cyc_Binding_BindingError){_LL48: _LL49:
# 838
 goto _LL47;}else{_LL4A: _tmp22A=_tmp229;_LL4B: {void*exn=_tmp22A;(int)_rethrow(exn);}}_LL47:;}}}
({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp22C=({struct Cyc_Warn_Qvar_Warn_Warg_struct _tmp304;_tmp304.tag=1U,_tmp304.f1=qv;_tmp304;});struct Cyc_Warn_String_Warn_Warg_struct _tmp22D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp303;_tmp303.tag=0U,({struct _fat_ptr _tmp398=({const char*_tmp22E=" is not a constructor in pattern";_tag_fat(_tmp22E,sizeof(char),33U);});_tmp303.f1=_tmp398;});_tmp303;});void*_tmp22B[2U];_tmp22B[0]=& _tmp22C,_tmp22B[1]=& _tmp22D;({unsigned _tmp399=loc;Cyc_Warn_err2(_tmp399,_tag_fat(_tmp22B,sizeof(void*),2U));});});return;}case 7U: if(((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1 != 0){if((((union Cyc_Absyn_AggrInfo*)((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1)->UnknownAggr).tag == 1){_LL8: _tmp1F9=(((((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1)->UnknownAggr).val).f2;_tmp1FA=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_tmp1FB=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_tmp1FC=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f4;_LL9: {struct _tuple0*n=_tmp1F9;struct Cyc_List_List*exist_ts=_tmp1FA;struct Cyc_List_List*dps=_tmp1FB;int dots=_tmp1FC;
# 842
{struct Cyc_List_List*_tmp22F=dps;struct Cyc_List_List*dps2=_tmp22F;for(0;dps2 != 0;dps2=dps2->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple14*)dps2->hd)).f2);}}
{struct _handler_cons _tmp230;_push_handler(& _tmp230);{int _tmp232=0;if(setjmp(_tmp230.handler))_tmp232=1;if(!_tmp232){
{struct Cyc_Absyn_Aggrdecl*_tmp233=((struct Cyc_Absyn_Aggrdecl*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_Absyn_Aggrdecl*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,n,Cyc_Binding_lookup_aggrdecl);struct Cyc_Absyn_Aggrdecl*ad=_tmp233;
({void*_tmp39C=(void*)({struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*_tmp236=_cycalloc(sizeof(*_tmp236));_tmp236->tag=7U,({union Cyc_Absyn_AggrInfo*_tmp39B=({union Cyc_Absyn_AggrInfo*_tmp235=_cycalloc(sizeof(*_tmp235));({union Cyc_Absyn_AggrInfo _tmp39A=Cyc_Absyn_KnownAggr(({struct Cyc_Absyn_Aggrdecl**_tmp234=_cycalloc(sizeof(*_tmp234));*_tmp234=ad;_tmp234;}));*_tmp235=_tmp39A;});_tmp235;});_tmp236->f1=_tmp39B;}),_tmp236->f2=exist_ts,_tmp236->f3=dps,_tmp236->f4=dots;_tmp236;});p->r=_tmp39C;});}
# 845
;_pop_handler();}else{void*_tmp231=(void*)Cyc_Core_get_exn_thrown();void*_tmp237=_tmp231;void*_tmp238;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp237)->tag == Cyc_Binding_BindingError){_LL4D: _LL4E:
# 848
({struct Cyc_Warn_String_Warn_Warg_struct _tmp23A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp305;_tmp305.tag=0U,({struct _fat_ptr _tmp39D=({const char*_tmp23B="non-aggregate name has designator patterns";_tag_fat(_tmp23B,sizeof(char),43U);});_tmp305.f1=_tmp39D;});_tmp305;});void*_tmp239[1U];_tmp239[0]=& _tmp23A;({unsigned _tmp39E=loc;Cyc_Warn_err2(_tmp39E,_tag_fat(_tmp239,sizeof(void*),1U));});});return;}else{_LL4F: _tmp238=_tmp237;_LL50: {void*exn=_tmp238;(int)_rethrow(exn);}}_LL4C:;}}}
# 850
return;}}else{_LL24: _tmp1F8=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_LL25: {struct Cyc_List_List*dps=_tmp1F8;
# 879
for(0;dps != 0;dps=dps->tl){
Cyc_Binding_resolve_pat(env,(*((struct _tuple14*)dps->hd)).f2);}
return;}}}else{_LL22: _tmp1F7=((struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_LL23: {struct Cyc_List_List*dps=_tmp1F7;
# 877
_tmp1F8=dps;goto _LL25;}}case 0U: _LLA: _LLB:
# 852
 return;case 3U: _LLC: _tmp1F5=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_tmp1F6=((struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_LLD: {struct Cyc_Absyn_Vardecl*vd=_tmp1F5;struct Cyc_Absyn_Pat*p2=_tmp1F6;
# 854
_tmp1F3=vd;_tmp1F4=p2;goto _LLF;}case 1U: _LLE: _tmp1F3=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_tmp1F4=((struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_LLF: {struct Cyc_Absyn_Vardecl*vd=_tmp1F3;struct Cyc_Absyn_Pat*p2=_tmp1F4;
# 856
Cyc_Binding_resolve_pat(env,p2);
_tmp1F2=vd;goto _LL11;}case 4U: _LL10: _tmp1F2=((struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_LL11: {struct Cyc_Absyn_Vardecl*vd=_tmp1F2;
_tmp1F1=vd;goto _LL13;}case 2U: _LL12: _tmp1F1=((struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct*)_tmp1EC)->f2;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp1F1;
# 860
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);goto _LL3;}case 6U: _LL14: _tmp1F0=((struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_LL15: {struct Cyc_Absyn_Pat*p2=_tmp1F0;
# 862
Cyc_Binding_resolve_pat(env,p2);return;}case 5U: _LL16: _tmp1EF=((struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_LL17: {struct Cyc_List_List*ps=_tmp1EF;
# 864
for(0;ps != 0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
goto _LL3;}case 17U: _LL18: _tmp1EE=((struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct*)_tmp1EC)->f1;_LL19: {struct Cyc_Absyn_Exp*e=_tmp1EE;
# 868
Cyc_Binding_resolve_exp(env,e);return;}case 9U: _LL1A: _LL1B:
# 870
 goto _LL1D;case 10U: _LL1C: _LL1D:
 goto _LL1F;case 11U: _LL1E: _LL1F:
 goto _LL21;case 12U: _LL20: _LL21:
 return;case 8U: _LL26: _tmp1ED=((struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct*)_tmp1EC)->f3;_LL27: {struct Cyc_List_List*ps=_tmp1ED;
# 883
for(0;ps != 0;ps=ps->tl){
Cyc_Binding_resolve_pat(env,(struct Cyc_Absyn_Pat*)ps->hd);}
return;}case 13U: _LL28: _LL29:
 goto _LL2B;default: _LL2A: _LL2B:
 return;}_LL3:;}}struct _tuple15{void*f1;void*f2;};
# 891
void Cyc_Binding_resolve_rgnpo(unsigned loc,struct Cyc_Binding_Env*env,struct Cyc_List_List*po){
for(0;po != 0;po=po->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple15*)po->hd)).f1);
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple15*)po->hd)).f2);}}struct _tuple16{struct Cyc_List_List**f1;struct Cyc_Dict_Dict*f2;struct Cyc_Binding_Env*f3;struct _tuple10*f4;};struct _tuple17{unsigned f1;struct _tuple0*f2;int f3;};
# 900
void Cyc_Binding_export_all_symbols(struct _tuple16*cenv,struct _fat_ptr*name,void*res){
# 902
struct _tuple16*_tmp23C=cenv;struct Cyc_List_List*_tmp241;unsigned _tmp240;struct Cyc_Binding_Env*_tmp23F;struct Cyc_Dict_Dict*_tmp23E;struct Cyc_List_List**_tmp23D;_LL1: _tmp23D=_tmp23C->f1;_tmp23E=_tmp23C->f2;_tmp23F=_tmp23C->f3;_tmp240=(_tmp23C->f4)->f1;_tmp241=(_tmp23C->f4)->f2;_LL2: {struct Cyc_List_List**exlist_ptr=_tmp23D;struct Cyc_Dict_Dict*out_dict=_tmp23E;struct Cyc_Binding_Env*env=_tmp23F;unsigned wcloc=_tmp240;struct Cyc_List_List*hidelist=_tmp241;
void*_tmp242=res;struct Cyc_Absyn_Datatypefield*_tmp244;struct Cyc_Absyn_Datatypedecl*_tmp243;struct Cyc_Absyn_Enumfield*_tmp246;void*_tmp245;struct Cyc_Absyn_Aggrdecl*_tmp247;struct Cyc_Absyn_Enumfield*_tmp249;struct Cyc_Absyn_Enumdecl*_tmp248;void*_tmp24A;switch(*((int*)_tmp242)){case 0U: _LL4: _tmp24A=(void*)((struct Cyc_Binding_VarRes_Binding_Resolved_struct*)_tmp242)->f1;_LL5: {void*bnd=_tmp24A;
# 905
struct _tuple0*qv=({struct _tuple0*_tmp24E=_cycalloc(sizeof(*_tmp24E));({union Cyc_Absyn_Nmspace _tmp39F=Cyc_Absyn_Rel_n(0);_tmp24E->f1=_tmp39F;}),_tmp24E->f2=name;_tmp24E;});
if(((int(*)(int(*compare)(struct _tuple0*,struct _tuple0*),struct Cyc_List_List*l,struct _tuple0*x))Cyc_List_mem)(Cyc_Absyn_qvar_cmp,hidelist,qv))
# 908
return;
# 910
Cyc_Binding_absolutize_decl(wcloc,env,qv,Cyc_Absyn_ExternC);{
struct _tuple17*_tmp24B=({struct _tuple17*_tmp24D=_cycalloc(sizeof(*_tmp24D));_tmp24D->f1=wcloc,_tmp24D->f2=qv,_tmp24D->f3=0;_tmp24D;});struct _tuple17*ex_sym=_tmp24B;
({struct Cyc_List_List*_tmp3A0=({struct Cyc_List_List*_tmp24C=_cycalloc(sizeof(*_tmp24C));_tmp24C->hd=ex_sym,_tmp24C->tl=*exlist_ptr;_tmp24C;});*exlist_ptr=_tmp3A0;});
({struct Cyc_Dict_Dict _tmp3A1=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(*out_dict,name,res);*out_dict=_tmp3A1;});
goto _LL3;}}case 3U: _LL6: _tmp248=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp242)->f1;_tmp249=((struct Cyc_Binding_EnumRes_Binding_Resolved_struct*)_tmp242)->f2;_LL7: {struct Cyc_Absyn_Enumdecl*ed=_tmp248;struct Cyc_Absyn_Enumfield*ef=_tmp249;
# 916
({struct Cyc_Dict_Dict _tmp3A2=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(*out_dict,name,res);*out_dict=_tmp3A2;});goto _LL3;}case 1U: _LL8: _tmp247=((struct Cyc_Binding_AggrRes_Binding_Resolved_struct*)_tmp242)->f1;_LL9: {struct Cyc_Absyn_Aggrdecl*ad=_tmp247;
# 918
goto _LL3;}case 4U: _LLA: _tmp245=(void*)((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp242)->f1;_tmp246=((struct Cyc_Binding_AnonEnumRes_Binding_Resolved_struct*)_tmp242)->f2;_LLB: {void*t=_tmp245;struct Cyc_Absyn_Enumfield*ef=_tmp246;
goto _LL3;}default: _LLC: _tmp243=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp242)->f1;_tmp244=((struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*)_tmp242)->f2;_LLD: {struct Cyc_Absyn_Datatypedecl*dd=_tmp243;struct Cyc_Absyn_Datatypefield*df=_tmp244;
# 921
({void*_tmp24F=0U;({unsigned _tmp3A4=wcloc;struct _fat_ptr _tmp3A3=({const char*_tmp250="Unexpected binding from extern C Include";_tag_fat(_tmp250,sizeof(char),41U);});Cyc_Warn_err(_tmp3A4,_tmp3A3,_tag_fat(_tmp24F,sizeof(void*),0U));});});
goto _LL3;}}_LL3:;}}
# 934 "binding.cyc"
void Cyc_Binding_resolve_decl(struct Cyc_Binding_Env*env,struct Cyc_Absyn_Decl*d){
# 937
unsigned loc=d->loc;
void*_tmp251=d->r;void*_stmttmp1A=_tmp251;void*_tmp252=_stmttmp1A;struct _tuple10*_tmp256;struct Cyc_List_List**_tmp255;struct Cyc_List_List*_tmp254;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp257;struct Cyc_List_List*_tmp259;struct _tuple0*_tmp258;struct Cyc_List_List*_tmp25B;struct _fat_ptr*_tmp25A;struct Cyc_Absyn_Datatypedecl*_tmp25C;struct Cyc_Absyn_Enumdecl*_tmp25D;struct Cyc_Absyn_Aggrdecl*_tmp25E;struct Cyc_Absyn_Typedefdecl*_tmp25F;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Vardecl*_tmp261;struct Cyc_Absyn_Tvar*_tmp260;struct Cyc_List_List*_tmp263;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Pat*_tmp264;struct Cyc_Absyn_Fndecl*_tmp266;struct Cyc_Absyn_Vardecl*_tmp267;switch(*((int*)_tmp252)){case 0U: _LL1: _tmp267=((struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp267;
# 941
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
# 946
if(vd->initializer != 0 && !Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(vd->initializer));
goto _LL0;}case 1U: _LL3: _tmp266=((struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LL4: {struct Cyc_Absyn_Fndecl*fd=_tmp266;
# 951
Cyc_Binding_absolutize_decl(loc,env,fd->name,fd->sc);{
struct _tuple0*_tmp268=fd->name;struct _tuple0*_stmttmp1B=_tmp268;struct _tuple0*_tmp269=_stmttmp1B;struct _fat_ptr*_tmp26B;union Cyc_Absyn_Nmspace _tmp26A;_LL24: _tmp26A=_tmp269->f1;_tmp26B=_tmp269->f2;_LL25: {union Cyc_Absyn_Nmspace decl_ns=_tmp26A;struct _fat_ptr*decl_name=_tmp26B;
# 954
struct Cyc_List_List*_tmp26C=Cyc_Binding_get_fun_vardecls(1,loc,env,(fd->i).args,(fd->i).cyc_varargs);struct Cyc_List_List*vds=_tmp26C;
({struct Cyc_Core_Opt*_tmp3A5=({struct Cyc_Core_Opt*_tmp26D=_cycalloc(sizeof(*_tmp26D));_tmp26D->v=vds;_tmp26D;});fd->param_vardecls=_tmp3A5;});
# 957
if(Cyc_Binding_warn_override)
Cyc_Binding_check_warn_override(loc,env,fd->name);{
# 960
struct Cyc_Dict_Dict*_tmp26E=env->local_vars;struct Cyc_Dict_Dict*old_locals=_tmp26E;
if(old_locals != 0)
({struct Cyc_Dict_Dict*_tmp3A6=({struct Cyc_Dict_Dict*_tmp26F=_cycalloc(sizeof(*_tmp26F));*_tmp26F=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));_tmp26F;});env->local_vars=_tmp3A6;});else{
# 964
({struct Cyc_Dict_Dict*_tmp3A8=({struct Cyc_Dict_Dict*_tmp270=_cycalloc(sizeof(*_tmp270));({struct Cyc_Dict_Dict _tmp3A7=((struct Cyc_Dict_Dict(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*)))Cyc_Dict_empty)(Cyc_strptrcmp);*_tmp270=_tmp3A7;});_tmp270;});env->local_vars=_tmp3A8;});}
{struct Cyc_List_List*_tmp271=vds;struct Cyc_List_List*vds1=_tmp271;for(0;vds1 != 0;vds1=vds1->tl){
({struct Cyc_Dict_Dict _tmp3AC=({struct Cyc_Dict_Dict _tmp3AB=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp3AA=(*((struct Cyc_Absyn_Vardecl*)vds1->hd)->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3AB,_tmp3AA,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp273=_cycalloc(sizeof(*_tmp273));
_tmp273->tag=0U,({void*_tmp3A9=(void*)({struct Cyc_Absyn_Param_b_Absyn_Binding_struct*_tmp272=_cycalloc(sizeof(*_tmp272));_tmp272->tag=3U,_tmp272->f1=(struct Cyc_Absyn_Vardecl*)vds1->hd;_tmp272;});_tmp273->f1=_tmp3A9;});_tmp273;}));});
# 966
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp3AC;});}}
# 969
Cyc_Binding_resolve_function_stuff(loc,env,fd->i);
# 971
if(old_locals != 0){
# 973
({struct Cyc_Dict_Dict _tmp3B0=({struct Cyc_Dict_Dict _tmp3AF=*old_locals;struct _fat_ptr*_tmp3AE=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3AF,_tmp3AE,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp275=_cycalloc(sizeof(*_tmp275));
_tmp275->tag=0U,({void*_tmp3AD=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp274=_cycalloc(sizeof(*_tmp274));_tmp274->tag=2U,_tmp274->f1=fd;_tmp274;});_tmp275->f1=_tmp3AD;});_tmp275;}));});
# 973
*old_locals=_tmp3B0;});
# 975
({struct Cyc_Dict_Dict _tmp3B4=({struct Cyc_Dict_Dict _tmp3B3=*((struct Cyc_Dict_Dict*)_check_null(env->local_vars));struct _fat_ptr*_tmp3B2=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3B3,_tmp3B2,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp277=_cycalloc(sizeof(*_tmp277));
_tmp277->tag=0U,({void*_tmp3B1=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp276=_cycalloc(sizeof(*_tmp276));_tmp276->tag=2U,_tmp276->f1=fd;_tmp276;});_tmp277->f1=_tmp3B1;});_tmp277;}));});
# 975
*((struct Cyc_Dict_Dict*)_check_null(env->local_vars))=_tmp3B4;});}else{
# 978
struct Cyc_Binding_ResolveNSEnv*_tmp278=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp278;
({struct Cyc_Dict_Dict _tmp3B8=({struct Cyc_Dict_Dict _tmp3B7=decl_ns_data->ordinaries;struct _fat_ptr*_tmp3B6=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3B7,_tmp3B6,(void*)({struct Cyc_Binding_VarRes_Binding_Resolved_struct*_tmp27A=_cycalloc(sizeof(*_tmp27A));
# 981
_tmp27A->tag=0U,({void*_tmp3B5=(void*)({struct Cyc_Absyn_Funname_b_Absyn_Binding_struct*_tmp279=_cycalloc(sizeof(*_tmp279));_tmp279->tag=2U,_tmp279->f1=fd;_tmp279;});_tmp27A->f1=_tmp3B5;});_tmp27A;}));});
# 979
decl_ns_data->ordinaries=_tmp3B8;});}
# 984
if(!Cyc_Binding_in_cinclude(env))
Cyc_Binding_resolve_stmt(env,fd->body);
# 987
env->local_vars=old_locals;
goto _LL0;}}}}case 2U: _LL5: _tmp264=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_tmp265=((struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct*)_tmp252)->f3;_LL6: {struct Cyc_Absyn_Pat*p=_tmp264;struct Cyc_Absyn_Exp*exp=_tmp265;
# 991
if(Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp306;_tmp306.tag=0U,({struct _fat_ptr _tmp3B9=({const char*_tmp27D="let not allowed at top-level";_tag_fat(_tmp27D,sizeof(char),29U);});_tmp306.f1=_tmp3B9;});_tmp306;});void*_tmp27B[1U];_tmp27B[0]=& _tmp27C;({unsigned _tmp3BA=loc;Cyc_Warn_err2(_tmp3BA,_tag_fat(_tmp27B,sizeof(void*),1U));});});
goto _LL0;}
# 995
Cyc_Binding_resolve_exp(env,exp);
Cyc_Binding_resolve_pat(env,p);
goto _LL0;}case 3U: _LL7: _tmp263=((struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LL8: {struct Cyc_List_List*vds=_tmp263;
# 1000
for(0;vds != 0;vds=vds->tl){
Cyc_Binding_resolve_and_add_vardecl(loc,env,(struct Cyc_Absyn_Vardecl*)vds->hd);}
goto _LL0;}case 4U: _LL9: _tmp260=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_tmp261=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp252)->f2;_tmp262=((struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct*)_tmp252)->f3;_LLA: {struct Cyc_Absyn_Tvar*tv=_tmp260;struct Cyc_Absyn_Vardecl*vd=_tmp261;struct Cyc_Absyn_Exp*open_exp_opt=_tmp262;
# 1005
if(open_exp_opt != 0)
Cyc_Binding_resolve_exp(env,open_exp_opt);
Cyc_Binding_resolve_and_add_vardecl(loc,env,vd);
goto _LL0;}case 8U: _LLB: _tmp25F=((struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LLC: {struct Cyc_Absyn_Typedefdecl*td=_tmp25F;
# 1011
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp307;_tmp307.tag=0U,({struct _fat_ptr _tmp3BB=({const char*_tmp280="nested type definitions are not yet supported";_tag_fat(_tmp280,sizeof(char),46U);});_tmp307.f1=_tmp3BB;});_tmp307;});void*_tmp27E[1U];_tmp27E[0]=& _tmp27F;({unsigned _tmp3BC=loc;Cyc_Warn_err2(_tmp3BC,_tag_fat(_tmp27E,sizeof(void*),1U));});});
goto _LL0;}
# 1015
Cyc_Binding_absolutize_decl(loc,env,td->name,td->extern_c?Cyc_Absyn_ExternC: Cyc_Absyn_Public);{
struct _tuple0*_tmp281=td->name;struct _tuple0*_stmttmp1C=_tmp281;struct _tuple0*_tmp282=_stmttmp1C;struct _fat_ptr*_tmp284;union Cyc_Absyn_Nmspace _tmp283;_LL27: _tmp283=_tmp282->f1;_tmp284=_tmp282->f2;_LL28: {union Cyc_Absyn_Nmspace decl_ns=_tmp283;struct _fat_ptr*decl_name=_tmp284;
# 1018
struct Cyc_Binding_ResolveNSEnv*_tmp285=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp285;
if(td->defn != 0)
Cyc_Binding_resolve_type(loc,env,(void*)_check_null(td->defn));
# 1022
({struct Cyc_Dict_Dict _tmp3BD=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Typedefdecl*v))Cyc_Dict_insert)(decl_ns_data->typedefs,decl_name,td);decl_ns_data->typedefs=_tmp3BD;});
# 1024
goto _LL0;}}}case 5U: _LLD: _tmp25E=((struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LLE: {struct Cyc_Absyn_Aggrdecl*ad=_tmp25E;
# 1027
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp287=({struct Cyc_Warn_String_Warn_Warg_struct _tmp308;_tmp308.tag=0U,({struct _fat_ptr _tmp3BE=({const char*_tmp288="nested type definitions are not yet supported";_tag_fat(_tmp288,sizeof(char),46U);});_tmp308.f1=_tmp3BE;});_tmp308;});void*_tmp286[1U];_tmp286[0]=& _tmp287;({unsigned _tmp3BF=loc;Cyc_Warn_err2(_tmp3BF,_tag_fat(_tmp286,sizeof(void*),1U));});});
goto _LL0;}
# 1031
Cyc_Binding_absolutize_decl(loc,env,ad->name,ad->sc);{
struct _tuple0*_tmp289=ad->name;struct _tuple0*_stmttmp1D=_tmp289;struct _tuple0*_tmp28A=_stmttmp1D;struct _fat_ptr*_tmp28C;union Cyc_Absyn_Nmspace _tmp28B;_LL2A: _tmp28B=_tmp28A->f1;_tmp28C=_tmp28A->f2;_LL2B: {union Cyc_Absyn_Nmspace decl_ns=_tmp28B;struct _fat_ptr*decl_name=_tmp28C;
# 1034
struct Cyc_Binding_ResolveNSEnv*_tmp28D=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp28D;
# 1036
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(decl_ns_data->aggrdecls,decl_name)&& ad->impl == 0)
goto _LL0;
# 1039
({struct Cyc_Dict_Dict _tmp3C0=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Aggrdecl*v))Cyc_Dict_insert)(decl_ns_data->aggrdecls,decl_name,ad);decl_ns_data->aggrdecls=_tmp3C0;});
# 1041
({struct Cyc_Dict_Dict _tmp3C3=({struct Cyc_Dict_Dict _tmp3C2=decl_ns_data->ordinaries;struct _fat_ptr*_tmp3C1=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3C2,_tmp3C1,(void*)({struct Cyc_Binding_AggrRes_Binding_Resolved_struct*_tmp28E=_cycalloc(sizeof(*_tmp28E));
_tmp28E->tag=1U,_tmp28E->f1=ad;_tmp28E;}));});
# 1041
decl_ns_data->ordinaries=_tmp3C3;});
# 1043
if(ad->impl != 0){
struct Cyc_Absyn_AggrdeclImpl*_tmp28F=(struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl);struct Cyc_Absyn_AggrdeclImpl*_stmttmp1E=_tmp28F;struct Cyc_Absyn_AggrdeclImpl*_tmp290=_stmttmp1E;struct Cyc_List_List*_tmp292;struct Cyc_List_List*_tmp291;_LL2D: _tmp291=_tmp290->rgn_po;_tmp292=_tmp290->fields;_LL2E: {struct Cyc_List_List*rpo=_tmp291;struct Cyc_List_List*fs=_tmp292;
Cyc_Binding_resolve_rgnpo(loc,env,rpo);
Cyc_Binding_resolve_aggrfields(loc,env,fs);}}
# 1048
goto _LL0;}}}case 7U: _LLF: _tmp25D=((struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LL10: {struct Cyc_Absyn_Enumdecl*ed=_tmp25D;
# 1051
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp294=({struct Cyc_Warn_String_Warn_Warg_struct _tmp309;_tmp309.tag=0U,({struct _fat_ptr _tmp3C4=({const char*_tmp295="nested type definitions are not yet supported";_tag_fat(_tmp295,sizeof(char),46U);});_tmp309.f1=_tmp3C4;});_tmp309;});void*_tmp293[1U];_tmp293[0]=& _tmp294;({unsigned _tmp3C5=loc;Cyc_Warn_err2(_tmp3C5,_tag_fat(_tmp293,sizeof(void*),1U));});});
goto _LL0;}
# 1055
Cyc_Binding_absolutize_decl(loc,env,ed->name,ed->sc);{
struct _tuple0*_tmp296=ed->name;struct _tuple0*_stmttmp1F=_tmp296;struct _tuple0*_tmp297=_stmttmp1F;struct _fat_ptr*_tmp299;union Cyc_Absyn_Nmspace _tmp298;_LL30: _tmp298=_tmp297->f1;_tmp299=_tmp297->f2;_LL31: {union Cyc_Absyn_Nmspace decl_ns=_tmp298;struct _fat_ptr*decl_name=_tmp299;
# 1058
struct Cyc_Binding_ResolveNSEnv*_tmp29A=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp29A;
# 1060
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(decl_ns_data->enumdecls,decl_name)&& ed->fields == 0)
goto _LL0;
# 1064
({struct Cyc_Dict_Dict _tmp3C6=((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_Absyn_Enumdecl*v))Cyc_Dict_insert)(decl_ns_data->enumdecls,decl_name,ed);decl_ns_data->enumdecls=_tmp3C6;});
# 1066
if(ed->fields != 0){
# 1068
struct Cyc_List_List*fs=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed->fields))->v;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Enumfield*_tmp29B=(struct Cyc_Absyn_Enumfield*)fs->hd;struct Cyc_Absyn_Enumfield*f=_tmp29B;
Cyc_Binding_absolutize_decl(f->loc,env,f->name,ed->sc);
if(f->tag != 0)
Cyc_Binding_resolve_exp(env,(struct Cyc_Absyn_Exp*)_check_null(f->tag));
({struct Cyc_Dict_Dict _tmp3C9=({struct Cyc_Dict_Dict _tmp3C8=decl_ns_data->ordinaries;struct _fat_ptr*_tmp3C7=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3C8,_tmp3C7,(void*)({struct Cyc_Binding_EnumRes_Binding_Resolved_struct*_tmp29C=_cycalloc(sizeof(*_tmp29C));
# 1075
_tmp29C->tag=3U,_tmp29C->f1=ed,_tmp29C->f2=f;_tmp29C;}));});
# 1073
decl_ns_data->ordinaries=_tmp3C9;});}}
# 1077
goto _LL0;}}}case 6U: _LL11: _tmp25C=((struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LL12: {struct Cyc_Absyn_Datatypedecl*tud=_tmp25C;
# 1081
{struct _handler_cons _tmp29D;_push_handler(& _tmp29D);{int _tmp29F=0;if(setjmp(_tmp29D.handler))_tmp29F=1;if(!_tmp29F){
{struct Cyc_List_List*_tmp2A0=((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*qv,struct Cyc_List_List*(*lookup)(struct Cyc_Binding_ResolveNSEnv*,struct _fat_ptr*)))Cyc_Binding_resolve_lookup)(loc,env->ns,tud->name,Cyc_Binding_lookup_datatypedecl);struct Cyc_List_List*decls=_tmp2A0;
struct Cyc_Absyn_Datatypedecl*_tmp2A1=(struct Cyc_Absyn_Datatypedecl*)decls->hd;struct Cyc_Absyn_Datatypedecl*last_decl=_tmp2A1;
if(!last_decl->is_extensible)
(int)_throw((void*)({struct Cyc_Binding_BindingError_exn_struct*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->tag=Cyc_Binding_BindingError;_tmp2A2;}));
tud->name=last_decl->name;
tud->is_extensible=1;{
struct _tuple0*_tmp2A3=tud->name;struct _tuple0*_stmttmp20=_tmp2A3;struct _tuple0*_tmp2A4=_stmttmp20;struct _fat_ptr*_tmp2A6;union Cyc_Absyn_Nmspace _tmp2A5;_LL33: _tmp2A5=_tmp2A4->f1;_tmp2A6=_tmp2A4->f2;_LL34: {union Cyc_Absyn_Nmspace decl_ns=_tmp2A5;struct _fat_ptr*decl_name=_tmp2A6;
struct Cyc_Binding_ResolveNSEnv*_tmp2A7=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp2A7;
({struct Cyc_Dict_Dict _tmp3CC=({struct Cyc_Dict_Dict _tmp3CB=decl_ns_data->datatypedecls;struct _fat_ptr*_tmp3CA=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp3CB,_tmp3CA,({struct Cyc_List_List*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));
_tmp2A8->hd=tud,_tmp2A8->tl=decls;_tmp2A8;}));});
# 1090
decl_ns_data->datatypedecls=_tmp3CC;});}}}
# 1082
;_pop_handler();}else{void*_tmp29E=(void*)Cyc_Core_get_exn_thrown();void*_tmp2A9=_tmp29E;void*_tmp2AA;if(((struct Cyc_Binding_BindingError_exn_struct*)_tmp2A9)->tag == Cyc_Binding_BindingError){_LL36: _LL37:
# 1094
 Cyc_Binding_absolutize_topdecl(loc,env,tud->name,tud->sc);{
struct _tuple0*_tmp2AB=tud->name;struct _tuple0*_stmttmp21=_tmp2AB;struct _tuple0*_tmp2AC=_stmttmp21;struct _fat_ptr*_tmp2AE;union Cyc_Absyn_Nmspace _tmp2AD;_LL3B: _tmp2AD=_tmp2AC->f1;_tmp2AE=_tmp2AC->f2;_LL3C: {union Cyc_Absyn_Nmspace decl_ns=_tmp2AD;struct _fat_ptr*decl_name=_tmp2AE;
struct Cyc_Binding_ResolveNSEnv*_tmp2AF=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,decl_ns);struct Cyc_Binding_ResolveNSEnv*decl_ns_data=_tmp2AF;
# 1098
if(((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(decl_ns_data->datatypedecls,decl_name)&& tud->fields == 0)
# 1100
goto _LL35;
({struct Cyc_Dict_Dict _tmp3CF=({struct Cyc_Dict_Dict _tmp3CE=decl_ns_data->datatypedecls;struct _fat_ptr*_tmp3CD=decl_name;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,struct Cyc_List_List*v))Cyc_Dict_insert)(_tmp3CE,_tmp3CD,({struct Cyc_List_List*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));
_tmp2B0->hd=tud,_tmp2B0->tl=0;_tmp2B0;}));});
# 1101
decl_ns_data->datatypedecls=_tmp3CF;});
# 1103
goto _LL35;}}}else{_LL38: _tmp2AA=_tmp2A9;_LL39: {void*exn=_tmp2AA;(int)_rethrow(exn);}}_LL35:;}}}{
# 1106
struct _tuple0*_tmp2B1=tud->name;struct _tuple0*_stmttmp22=_tmp2B1;struct _tuple0*_tmp2B2=_stmttmp22;struct _fat_ptr*_tmp2B4;union Cyc_Absyn_Nmspace _tmp2B3;_LL3E: _tmp2B3=_tmp2B2->f1;_tmp2B4=_tmp2B2->f2;_LL3F: {union Cyc_Absyn_Nmspace decl_ns=_tmp2B3;struct _fat_ptr*decl_name=_tmp2B4;
if(tud->fields != 0){
struct Cyc_List_List*_tmp2B5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(tud->fields))->v;struct Cyc_List_List*fs=_tmp2B5;for(0;fs != 0;fs=fs->tl){
struct Cyc_Absyn_Datatypefield*_tmp2B6=(struct Cyc_Absyn_Datatypefield*)fs->hd;struct Cyc_Absyn_Datatypefield*f=_tmp2B6;
{struct Cyc_List_List*_tmp2B7=f->typs;struct Cyc_List_List*tqts=_tmp2B7;for(0;tqts != 0;tqts=tqts->tl){
Cyc_Binding_resolve_type(loc,env,(*((struct _tuple13*)tqts->hd)).f2);}}
{union Cyc_Absyn_Nmspace _tmp2B8=(*f->name).f1;union Cyc_Absyn_Nmspace _stmttmp23=_tmp2B8;union Cyc_Absyn_Nmspace _tmp2B9=_stmttmp23;switch((_tmp2B9.Abs_n).tag){case 1U: if((_tmp2B9.Rel_n).val == 0){_LL41: _LL42:
# 1114
 if(tud->is_extensible)
({union Cyc_Absyn_Nmspace _tmp3D0=Cyc_Absyn_Abs_n((env->ns)->curr_ns,0);(*f->name).f1=_tmp3D0;});else{
# 1117
(*f->name).f1=(*tud->name).f1;}
goto _LL40;}else{_LL43: _LL44:
# 1120
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30A;_tmp30A.tag=0U,({struct _fat_ptr _tmp3D1=({const char*_tmp2BC="qualified datatype field declaratations not allowed";_tag_fat(_tmp2BC,sizeof(char),52U);});_tmp30A.f1=_tmp3D1;});_tmp30A;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BB;({unsigned _tmp3D2=f->loc;Cyc_Warn_err2(_tmp3D2,_tag_fat(_tmp2BA,sizeof(void*),1U));});});
return;}case 2U: _LL45: _LL46:
 goto _LL40;default: _LL47: _LL48:
({void*_tmp2BD=0U;({struct _fat_ptr _tmp3D3=({const char*_tmp2BE="datatype field Loc_n or C_n";_tag_fat(_tmp2BE,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr))Cyc_Warn_impos)(_tmp3D3,_tag_fat(_tmp2BD,sizeof(void*),0U));});});}_LL40:;}{
# 1125
struct Cyc_Binding_ResolveNSEnv*_tmp2BF=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Binding_NSCtxt*ctxt,union Cyc_Absyn_Nmspace abs_ns))Cyc_Binding_get_ns_data)(env->ns,(*f->name).f1);struct Cyc_Binding_ResolveNSEnv*ns_data=_tmp2BF;
({struct Cyc_Dict_Dict _tmp3D6=({struct Cyc_Dict_Dict _tmp3D5=ns_data->ordinaries;struct _fat_ptr*_tmp3D4=(*f->name).f2;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3D5,_tmp3D4,(void*)({struct Cyc_Binding_DatatypeRes_Binding_Resolved_struct*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));
# 1128
_tmp2C0->tag=2U,_tmp2C0->f1=tud,_tmp2C0->f2=f;_tmp2C0;}));});
# 1126
ns_data->ordinaries=_tmp3D6;});}}}
# 1130
goto _LL0;}}}case 9U: _LL13: _tmp25A=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_tmp25B=((struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct*)_tmp252)->f2;_LL14: {struct _fat_ptr*v=_tmp25A;struct Cyc_List_List*ds2=_tmp25B;
# 1133
((void(*)(struct Cyc_Binding_NSCtxt*ctxt,struct _fat_ptr*subname,int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_enter_ns)(env->ns,v,1,Cyc_Binding_mt_renv);
Cyc_Binding_resolve_decls(env,ds2);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_ns)(env->ns);
goto _LL0;}case 10U: _LL15: _tmp258=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_tmp259=((struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct*)_tmp252)->f2;_LL16: {struct _tuple0*qv=_tmp258;struct Cyc_List_List*ds2=_tmp259;
# 1138
((struct Cyc_List_List*(*)(unsigned loc,struct Cyc_Binding_NSCtxt*ctxt,struct _tuple0*usename))Cyc_Binding_enter_using)(d->loc,env->ns,qv);
Cyc_Binding_resolve_decls(env,ds2);
((void(*)(struct Cyc_Binding_NSCtxt*ctxt))Cyc_Binding_leave_using)(env->ns);
goto _LL0;}case 11U: _LL17: _tmp257=((struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_LL18: {struct Cyc_List_List*ds2=_tmp257;
# 1144
int _tmp2C1=env->in_cinclude;int old=_tmp2C1;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
env->in_cinclude=old;
goto _LL0;}case 12U: _LL19: _tmp253=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp252)->f1;_tmp254=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp252)->f2;_tmp255=(struct Cyc_List_List**)&((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp252)->f3;_tmp256=((struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct*)_tmp252)->f4;_LL1A: {struct Cyc_List_List*ds2=_tmp253;struct Cyc_List_List*ovrs=_tmp254;struct Cyc_List_List**exports=_tmp255;struct _tuple10*hides=_tmp256;
# 1155
{struct Cyc_List_List*_tmp2C2=*exports;struct Cyc_List_List*exs=_tmp2C2;for(0;exs != 0;exs=exs->tl){
struct _tuple17*_tmp2C3=(struct _tuple17*)exs->hd;struct _tuple17*_stmttmp24=_tmp2C3;struct _tuple17*_tmp2C4=_stmttmp24;struct _tuple0*_tmp2C6;unsigned _tmp2C5;_LL4A: _tmp2C5=_tmp2C4->f1;_tmp2C6=_tmp2C4->f2;_LL4B: {unsigned loc=_tmp2C5;struct _tuple0*qv=_tmp2C6;
Cyc_Binding_absolutize_decl(loc,env,qv,Cyc_Absyn_ExternC);}}}
# 1163
if(!Cyc_Binding_at_toplevel(env)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp30B;_tmp30B.tag=0U,({struct _fat_ptr _tmp3D7=({const char*_tmp2C9="extern \"C include\" not at toplevel";_tag_fat(_tmp2C9,sizeof(char),35U);});_tmp30B.f1=_tmp3D7;});_tmp30B;});void*_tmp2C7[1U];_tmp2C7[0]=& _tmp2C8;({unsigned _tmp3D8=loc;Cyc_Warn_err2(_tmp3D8,_tag_fat(_tmp2C7,sizeof(void*),1U));});});
goto _LL0;}{
# 1167
struct Cyc_Binding_ResolveNSEnv*_tmp2CA=((struct Cyc_Binding_ResolveNSEnv*(*)(struct Cyc_Dict_Dict d,struct Cyc_List_List*k))Cyc_Dict_lookup)((env->ns)->ns_data,(env->ns)->curr_ns);struct Cyc_Binding_ResolveNSEnv*data=_tmp2CA;
struct Cyc_Dict_Dict _tmp2CB=data->ordinaries;struct Cyc_Dict_Dict old_dict=_tmp2CB;
int _tmp2CC=env->in_cinclude;int old=_tmp2CC;
env->in_cinclude=1;
Cyc_Binding_resolve_decls(env,ds2);
Cyc_Binding_resolve_decls(env,ovrs);{
struct Cyc_Dict_Dict _tmp2CD=data->ordinaries;struct Cyc_Dict_Dict new_dict=_tmp2CD;
struct Cyc_Dict_Dict _tmp2CE=old_dict;struct Cyc_Dict_Dict out_dict=_tmp2CE;
if((*hides).f1 > (unsigned)0){
if((unsigned)*exports)
({void*_tmp2CF=0U;({unsigned _tmp3DA=(*hides).f1;struct _fat_ptr _tmp3D9=({const char*_tmp2D0="export wildcard expects empty export list";_tag_fat(_tmp2D0,sizeof(char),42U);});Cyc_Warn_err(_tmp3DA,_tmp3D9,_tag_fat(_tmp2CF,sizeof(void*),0U));});});
env->in_cinclude=old;
({struct _tuple16*_tmp3DB=({struct _tuple16*_tmp2D1=_cycalloc(sizeof(*_tmp2D1));_tmp2D1->f1=exports,_tmp2D1->f2=& out_dict,_tmp2D1->f3=env,_tmp2D1->f4=hides;_tmp2D1;});((void(*)(void(*f)(struct _tuple16*,struct _fat_ptr*,void*),struct _tuple16*env,struct Cyc_Dict_Dict d))Cyc_Dict_iter_c)(Cyc_Binding_export_all_symbols,_tmp3DB,
((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2))Cyc_Dict_difference)(new_dict,old_dict));});}else{
# 1183
struct Cyc_List_List*_tmp2D2=*exports;struct Cyc_List_List*exs=_tmp2D2;for(0;exs != 0;exs=exs->tl){
struct _tuple17*_tmp2D3=(struct _tuple17*)exs->hd;struct _tuple17*_stmttmp25=_tmp2D3;struct _tuple17*_tmp2D4=_stmttmp25;struct _fat_ptr*_tmp2D6;unsigned _tmp2D5;_LL4D: _tmp2D5=_tmp2D4->f1;_tmp2D6=(_tmp2D4->f2)->f2;_LL4E: {unsigned loc=_tmp2D5;struct _fat_ptr*v=_tmp2D6;
if(!((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(new_dict,v)||
((int(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_member)(old_dict,v)&&({
void*_tmp3DC=((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(old_dict,v);_tmp3DC == ((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(new_dict,v);}))
({struct Cyc_String_pa_PrintArg_struct _tmp2D9=({struct Cyc_String_pa_PrintArg_struct _tmp30C;_tmp30C.tag=0U,_tmp30C.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp30C;});void*_tmp2D7[1U];_tmp2D7[0]=& _tmp2D9;({unsigned _tmp3DE=loc;struct _fat_ptr _tmp3DD=({const char*_tmp2D8="%s is exported but not defined";_tag_fat(_tmp2D8,sizeof(char),31U);});Cyc_Warn_err(_tmp3DE,_tmp3DD,_tag_fat(_tmp2D7,sizeof(void*),1U));});});
out_dict=({struct Cyc_Dict_Dict _tmp3E0=out_dict;struct _fat_ptr*_tmp3DF=v;((struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k,void*v))Cyc_Dict_insert)(_tmp3E0,_tmp3DF,((void*(*)(struct Cyc_Dict_Dict d,struct _fat_ptr*k))Cyc_Dict_lookup)(new_dict,v));});}}}
# 1192
data->ordinaries=out_dict;
env->in_cinclude=old;
# 1196
Cyc_Cifc_user_overrides(loc,ds2,ovrs);
goto _LL0;}}}case 13U: _LL1B: _LL1C:
# 1199
 goto _LL1E;case 14U: _LL1D: _LL1E:
 goto _LL20;case 15U: _LL1F: _LL20:
 goto _LL22;default: _LL21: _LL22:
 goto _LL0;}_LL0:;}
# 1206
void Cyc_Binding_resolve_decls(struct Cyc_Binding_Env*env,struct Cyc_List_List*tds){
for(0;tds != 0;tds=tds->tl){
Cyc_Binding_resolve_decl(env,(struct Cyc_Absyn_Decl*)tds->hd);}}
# 1211
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds){
struct Cyc_Binding_Env*_tmp2DA=({struct Cyc_Binding_Env*_tmp2DD=_cycalloc(sizeof(*_tmp2DD));_tmp2DD->in_cinclude=0,({struct Cyc_Binding_NSCtxt*_tmp3E1=((struct Cyc_Binding_NSCtxt*(*)(int env,struct Cyc_Binding_ResolveNSEnv*(*mkdata)(int)))Cyc_Binding_mt_nsctxt)(1,Cyc_Binding_mt_renv);_tmp2DD->ns=_tmp3E1;}),_tmp2DD->local_vars=0;_tmp2DD;});struct Cyc_Binding_Env*env=_tmp2DA;
({struct Cyc_Binding_Env*_tmp3E4=env;Cyc_Binding_resolve_decl(_tmp3E4,({struct Cyc_Absyn_Decl*_tmp2DC=_cycalloc(sizeof(*_tmp2DC));({void*_tmp3E3=(void*)({struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct*_tmp2DB=_cycalloc(sizeof(*_tmp2DB));_tmp2DB->tag=6U,({struct Cyc_Absyn_Datatypedecl*_tmp3E2=Cyc_Absyn_exn_tud();_tmp2DB->f1=_tmp3E2;});_tmp2DB;});_tmp2DC->r=_tmp3E3;}),_tmp2DC->loc=0U;_tmp2DC;}));});
Cyc_Binding_resolve_decls(env,tds);}
