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
 struct Cyc_Core_Opt{void*v;};
# 119 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
int Cyc_List_length(struct Cyc_List_List*x);
# 76
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 161
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 261
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
# 270
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 62
struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 289
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 309
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 412 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 501
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 508
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 526
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 874 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 876
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 883
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 888
void*Cyc_Absyn_compress_kb(void*);
# 894
int Cyc_Absyn_type2bool(int def,void*);
# 899
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 904
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 906
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 911
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 913
extern void*Cyc_Absyn_empty_effect;
# 915
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 917
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 942
extern void*Cyc_Absyn_fat_bound_type;
# 944
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 946
void*Cyc_Absyn_bounds_one();
# 948
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 953
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 973
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 975
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 978
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1020
struct Cyc_Absyn_Exp*Cyc_Absyn_throw_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_rethrow_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_noinstantiate_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_instantiate_exp(struct Cyc_Absyn_Exp*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_cast_exp(void*,struct Cyc_Absyn_Exp*,int user_cast,enum Cyc_Absyn_Coercion,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_address_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeoftype_exp(void*t,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_sizeofexp_exp(struct Cyc_Absyn_Exp*e,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_offsetof_exp(void*,struct Cyc_List_List*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_deref_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1032
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1037
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr body,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1076
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1117
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1119
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1121
struct _tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1129
int Cyc_Absyn_fntype_att(void*);
# 1131
int Cyc_Absyn_equal_att(void*,void*);
# 1133
int Cyc_Absyn_attribute_cmp(void*,void*);
# 1139
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 62 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 74
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 32
void Cyc_Warn_verr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 32
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 110
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 131
int Cyc_Relations_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2);struct Cyc_RgnOrder_RgnPO;
# 44 "rgnorder.h"
int Cyc_RgnOrder_rgn_outlives_rgn(struct Cyc_RgnOrder_RgnPO*,void*rgn1,void*rgn2);
# 30 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 37
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 44
int Cyc_Tcutil_is_pointer_type(void*);
# 57
int Cyc_Tcutil_is_bits_only_type(void*);
# 59
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 72
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 77
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 81
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type);
# 84
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 87
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 89
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 94
void*Cyc_Tcutil_copy_type(void*);
# 97
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 105
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 110
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
# 113
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 117
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 119
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 121
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*assume,void*t1,void*t2);
# 124
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);
# 129
extern int Cyc_Tcutil_warn_alias_coerce;
# 132
extern int Cyc_Tcutil_warn_region_coerce;
# 135
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 144
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 149
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 154
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_boolko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ptrbko;
# 163
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 168
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 173
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 182
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 185
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 187
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 193
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 196
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 200
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 220
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 233
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 236
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 241
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 246
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple13{int f1;void*f2;};
# 250
struct _tuple13 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 253
void*Cyc_Tcutil_normalize_effect(void*e);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 258
int Cyc_Tcutil_new_tvar_id();
# 260
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 263
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 271
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 278
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 296
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 299
int Cyc_Tcutil_zeroable_type(void*);
# 303
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 311
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 315
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 320
int Cyc_Tcutil_tycon_cmp(void*,void*);
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*,void*);
# 38 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_tmp1=_tmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 44
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 50
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_tmp5=_tmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 56
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_tmp7=_tmp6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 63
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_tmp9=_tmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 70
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 77
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_tmpD=_tmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 84
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_tmpF=_tmpE;void*_tmp11;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp11=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {
# 87
void*_tmp10=_tmp11;switch(*((int*)_tmp10)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 94
 return 0;}_LL0:;}
# 97
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_tmp13=_tmp12;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
 return 1;}else{goto _LL5;}case 2U: _LL3: _LL4:
 return 1;default: goto _LL5;}else{_LL5: _LL6:
 return 0;}_LL0:;}
# 104
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 107
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 110
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp14=Cyc_Tcutil_compress(t);void*_tmp15=_tmp14;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 116
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 124
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp16=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp17=_tmp16;void*_tmp18;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17)->tag == 1U){_LL1: _LL2:
 return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp18=_tmp17;_LL4:
 return Cyc_Tcutil_is_integral_type(_tmp18);}_LL0:;}
# 132
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp19=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp1A=_tmp19;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 142
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1B=Cyc_Tcutil_compress(t);void*_tmp1C=_tmp1B;void*_tmp1D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->tag == 3U){_LL1: _tmp1D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->f1).ptr_atts).zero_term;_LL2:
# 145
 return Cyc_Tcutil_force_type2bool(0,_tmp1D);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 151
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1E=Cyc_Tcutil_compress(t);void*_tmp1F=_tmp1E;void*_tmp20;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->tag == 3U){_LL1: _tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1).ptr_atts).nullable;_LL2:
# 154
 return Cyc_Tcutil_force_type2bool(0,_tmp20);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 160
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp21=Cyc_Tcutil_compress(t);void*_tmp22=_tmp21;void*_tmp23;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->tag == 3U){_LL1: _tmp23=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->f1).ptr_atts).bounds;_LL2:
# 163
 return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,_tmp23);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 170
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp24=Cyc_Tcutil_compress(t);void*_tmp25=_tmp24;void*_tmp27;void*_tmp26;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->tag == 3U){_LL1: _tmp27=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).elt_type;_tmp26=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).ptr_atts).bounds;_LL2:
# 173
 if(Cyc_Unify_unify(_tmp26,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=_tmp27;
return 1;}else{
# 177
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 184
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp28=Cyc_Tcutil_compress(t);void*_tmp29=_tmp28;void*_tmp2B;void*_tmp2A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->tag == 3U){_LL1: _tmp2B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).elt_type;_tmp2A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).ptr_atts).zero_term;_LL2:
# 187
*elt_type_dest=_tmp2B;
return Cyc_Absyn_type2bool(0,_tmp2A);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 196
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 198
void*_tmp2C=Cyc_Tcutil_compress(t);void*_tmp2D=_tmp2C;void*_tmp36;struct Cyc_Absyn_Tqual _tmp35;struct Cyc_Absyn_Exp*_tmp34;void*_tmp33;void*_tmp32;void*_tmp31;void*_tmp30;switch(*((int*)_tmp2D)){case 3U: _LL1: _tmp32=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp31=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).bounds;_tmp30=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).zero_term;_LL2:
# 200
 if(Cyc_Absyn_type2bool(0,_tmp30)){
*ptr_type=t;
*elt_type=_tmp32;
{void*_tmp2E=Cyc_Tcutil_compress(_tmp31);void*_tmp2F=_tmp2E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 207
return 1;}else{
return 0;}case 4U: _LL3: _tmp36=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp35=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).tq;_tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).zero_term;_LL4:
# 210
 if(Cyc_Absyn_type2bool(0,_tmp33)){
*elt_type=_tmp36;
*is_fat=0;
({void*_tmp64B=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp64B;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 223
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 229
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp64C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp64C,b);});
if(_tmp37 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp38=_tmp37;
struct _tuple12 _tmp39=Cyc_Evexp_eval_const_uint_exp(_tmp38);struct _tuple12 _tmp3A=_tmp39;unsigned _tmp3C;int _tmp3B;_LL1: _tmp3C=_tmp3A.f1;_tmp3B=_tmp3A.f2;_LL2:;
return _tmp3B && _tmp3C == (unsigned)1;};}
# 238
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3D=Cyc_Tcutil_compress(t);void*_tmp3E=_tmp3D;struct Cyc_List_List*_tmp4A;struct Cyc_List_List*_tmp49;void*_tmp48;void*_tmp47;void*_tmp46;struct Cyc_List_List*_tmp45;switch(*((int*)_tmp3E)){case 0U: _LL1: _tmp46=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f2;_LL2: {
# 241
void*_tmp3F=_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return 1;case 15U: _LL12: _LL13:
 goto _LL15;case 16U: _LL14: _LL15:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
 return 0;}else{_LL18: _tmp44=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).KnownAggr).val;_LL19:
# 249
 if(_tmp44->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp40=_new_region("rgn");struct _RegionHandle*rgn=& _tmp40;_push_region(rgn);
{struct Cyc_List_List*_tmp41=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,_tmp44->tvs,_tmp45);
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp44->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,_tmp41,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp42=0;_npop_handler(0U);return _tmp42;}}}{
int _tmp43=1;_npop_handler(0U);return _tmp43;};}
# 256
;_pop_region(rgn);};};}default: _LL1A: _LL1B:
# 260
 return 0;}_LLB:;}case 4U: _LL3: _tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).elt_type;_tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zero_term;_LL4:
# 265
 return !Cyc_Absyn_type2bool(0,_tmp47)&& Cyc_Tcutil_is_bits_only_type(_tmp48);case 6U: _LL5: _tmp49=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E)->f1;_LL6:
# 267
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple11*)_tmp49->hd)).f2))return 0;}
return 1;case 7U: _LL7: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E)->f2;_LL8:
# 271
 for(0;_tmp4A != 0;_tmp4A=_tmp4A->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)_tmp4A->hd)->type))return 0;}
return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 280
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_List_List*_tmp50;void*_tmp4F;switch(*((int*)_tmp4C)){case 3U: _LL1: _tmp4F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_noreturn_fn_type(_tmp4F);case 5U: _LL3: _tmp50=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_LL4:
# 284
 for(0;_tmp50 != 0;_tmp50=_tmp50->tl){
void*_tmp4D=(void*)_tmp50->hd;void*_tmp4E=_tmp4D;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp4E)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 289
return 0;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 296
int Cyc_Tcutil_warn_region_coerce=0;
# 298
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 300
Cyc_Warn_verr(loc,fmt,ap);}
# 302
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 304
Cyc_Warn_vimpos(fmt,ap);}
# 306
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 308
Cyc_Warn_vwarn(sg,fmt,ap);}
# 312
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 317
void*Cyc_Tcutil_compress(void*t){
void*_tmp51=t;void*_tmp5A;struct Cyc_Absyn_Exp*_tmp59;struct Cyc_Absyn_Exp*_tmp58;void**_tmp57;void**_tmp56;switch(*((int*)_tmp51)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp56=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51)->f2;_LL8: {
# 328
void*ta=(void*)_check_null(*_tmp56);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp56=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51)->f4 == 0){_LL3: _LL4:
# 320
 return t;}else{_LL5: _tmp57=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51)->f4;_LL6: {
# 322
void*ta=(void*)_check_null(*_tmp57);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp57=t2;
return t2;}}case 9U: _LL9: _tmp58=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp51)->f1;_LLA:
# 334
 Cyc_Evexp_eval_const_uint_exp(_tmp58);{
void*_tmp52=_tmp58->r;void*_tmp53=_tmp52;void*_tmp54;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp53)->tag == 39U){_LL12: _tmp54=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp53)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp54);}else{_LL14: _LL15:
 return t;}_LL11:;};case 11U: _LLB: _tmp59=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp51)->f1;_LLC: {
# 340
void*_tmp55=_tmp59->topt;
if(_tmp55 != 0)return _tmp55;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp51)->f2 != 0){_LLD: _tmp5A=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp51)->f2;_LLE:
 return Cyc_Tcutil_compress(_tmp5A);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 352
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 356
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp5B=Cyc_Absyn_compress_kb(kb);void*_tmp5C=_tmp5B;struct Cyc_Absyn_Kind*_tmp5F;switch(*((int*)_tmp5C)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=1U,_tmp5D->f1=0;_tmp5D;});case 2U: _LL3: _tmp5F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=2U,_tmp5E->f1=0,_tmp5E->f2=_tmp5F;_tmp5E;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 363
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 365
return({struct Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=tv->name,_tmp60->identity=- 1,({void*_tmp64D=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp60->kind=_tmp64D;});_tmp60;});}
# 367
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 369
struct _tuple9*_tmp61=arg;struct _fat_ptr*_tmp65;struct Cyc_Absyn_Tqual _tmp64;void*_tmp63;_LL1: _tmp65=_tmp61->f1;_tmp64=_tmp61->f2;_tmp63=_tmp61->f3;_LL2:;
return({struct _tuple9*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=_tmp65,_tmp62->f2=_tmp64,({void*_tmp64E=Cyc_Tcutil_copy_type(_tmp63);_tmp62->f3=_tmp64E;});_tmp62;});}
# 372
static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
return({struct _tuple11*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=(*arg).f1,({void*_tmp64F=Cyc_Tcutil_copy_type((*arg).f2);_tmp66->f2=_tmp64F;});_tmp66;});}
# 375
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}
# 379
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->name=f->name,_tmp67->tq=f->tq,({void*_tmp651=Cyc_Tcutil_copy_type(f->type);_tmp67->type=_tmp651;}),_tmp67->width=f->width,_tmp67->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp650=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp67->requires_clause=_tmp650;});_tmp67;});}struct _tuple14{void*f1;void*f2;};
# 383
static struct _tuple14*Cyc_Tcutil_copy_rgncmp(struct _tuple14*x){
struct _tuple14*_tmp68=x;void*_tmp6B;void*_tmp6A;_LL1: _tmp6B=_tmp68->f1;_tmp6A=_tmp68->f2;_LL2:;
return({struct _tuple14*_tmp69=_cycalloc(sizeof(*_tmp69));({void*_tmp653=Cyc_Tcutil_copy_type(_tmp6B);_tmp69->f1=_tmp653;}),({void*_tmp652=Cyc_Tcutil_copy_type(_tmp6A);_tmp69->f2=_tmp652;});_tmp69;});}
# 387
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 391
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6C=Cyc_Tcutil_compress(t);void*_tmp6D=_tmp6C;struct Cyc_Absyn_Datatypedecl*_tmpB1;struct Cyc_Absyn_Enumdecl*_tmpB0;struct Cyc_Absyn_Aggrdecl*_tmpAF;struct _tuple1*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Typedefdecl*_tmpAC;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAA;enum Cyc_Absyn_AggrKind _tmpA9;struct Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;struct Cyc_List_List*_tmpA2;int _tmpA1;struct Cyc_Absyn_VarargInfo*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_List_List*_tmp9A;void*_tmp99;struct Cyc_Absyn_Tqual _tmp98;struct Cyc_Absyn_Exp*_tmp97;void*_tmp96;unsigned _tmp95;void*_tmp94;struct Cyc_Absyn_Tqual _tmp93;void*_tmp92;void*_tmp91;void*_tmp90;void*_tmp8F;struct Cyc_Absyn_PtrLoc*_tmp8E;struct Cyc_Absyn_Tvar*_tmp8D;void*_tmp8C;struct Cyc_List_List*_tmp8B;void*_tmp8A;switch(*((int*)_tmp6D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2 == 0){_LL1: _tmp8A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_LL2:
 return t;}else{_LL3: _tmp8C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_tmp8B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=0U,_tmp6E->f1=_tmp8C,({struct Cyc_List_List*_tmp654=Cyc_Tcutil_copy_types(_tmp8B);_tmp6E->f2=_tmp654;});_tmp6E;});}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp8D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->f1;_LL8:
 return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(_tmp8D));case 3U: _LL9: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp93=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_tq;_tmp92=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).rgn;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).nullable;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).bounds;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).zero_term;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).ptrloc;_LLA: {
# 398
void*_tmp6F=Cyc_Tcutil_copy_type(_tmp94);
void*_tmp70=Cyc_Tcutil_copy_type(_tmp92);
void*_tmp71=Cyc_Tcutil_copy_type(_tmp91);
struct Cyc_Absyn_Tqual _tmp72=_tmp93;
# 403
void*_tmp73=Cyc_Tcutil_copy_type(_tmp90);
void*_tmp74=Cyc_Tcutil_copy_type(_tmp8F);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=3U,(_tmp75->f1).elt_type=_tmp6F,(_tmp75->f1).elt_tq=_tmp72,((_tmp75->f1).ptr_atts).rgn=_tmp70,((_tmp75->f1).ptr_atts).nullable=_tmp71,((_tmp75->f1).ptr_atts).bounds=_tmp73,((_tmp75->f1).ptr_atts).zero_term=_tmp74,((_tmp75->f1).ptr_atts).ptrloc=_tmp8E;_tmp75;});}case 4U: _LLB: _tmp99=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp98=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).tq;_tmp97=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).num_elts;_tmp96=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).zero_term;_tmp95=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).zt_loc;_LLC:
# 407
 return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=4U,({void*_tmp657=Cyc_Tcutil_copy_type(_tmp99);(_tmp76->f1).elt_type=_tmp657;}),(_tmp76->f1).tq=_tmp98,({struct Cyc_Absyn_Exp*_tmp656=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp97);(_tmp76->f1).num_elts=_tmp656;}),({
void*_tmp655=Cyc_Tcutil_copy_type(_tmp96);(_tmp76->f1).zero_term=_tmp655;}),(_tmp76->f1).zt_loc=_tmp95;_tmp76;});case 5U: _LLD: _tmpA6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).tvars;_tmpA5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).effect;_tmpA4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_tqual;_tmpA3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_type;_tmpA2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).args;_tmpA1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).c_varargs;_tmpA0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).cyc_varargs;_tmp9F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).rgn_po;_tmp9E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).attributes;_tmp9D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_clause;_tmp9C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_relns;_tmp9B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_clause;_tmp9A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_relns;_LLE: {
# 410
struct Cyc_List_List*_tmp77=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpA6);
void*effopt2=_tmpA5 == 0?0: Cyc_Tcutil_copy_type(_tmpA5);
void*_tmp78=Cyc_Tcutil_copy_type(_tmpA3);
struct Cyc_List_List*_tmp79=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpA2);
int _tmp7A=_tmpA1;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpA0 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpA0;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->name=cv->name,_tmp7B->tq=cv->tq,({void*_tmp658=Cyc_Tcutil_copy_type(cv->type);_tmp7B->type=_tmp658;}),_tmp7B->inject=cv->inject;_tmp7B;});}{
# 421
struct Cyc_List_List*_tmp7C=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9F);
struct Cyc_List_List*_tmp7D=_tmp9E;
struct Cyc_Absyn_Exp*_tmp7E=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp9D);
struct Cyc_List_List*_tmp7F=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp9C);
struct Cyc_Absyn_Exp*_tmp80=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp9B);
struct Cyc_List_List*_tmp81=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp9A);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=5U,(_tmp82->f1).tvars=_tmp77,(_tmp82->f1).effect=effopt2,(_tmp82->f1).ret_tqual=_tmpA4,(_tmp82->f1).ret_type=_tmp78,(_tmp82->f1).args=_tmp79,(_tmp82->f1).c_varargs=_tmp7A,(_tmp82->f1).cyc_varargs=cyc_varargs2,(_tmp82->f1).rgn_po=_tmp7C,(_tmp82->f1).attributes=_tmp7D,(_tmp82->f1).requires_clause=_tmp7E,(_tmp82->f1).requires_relns=_tmp7F,(_tmp82->f1).ensures_clause=_tmp80,(_tmp82->f1).ensures_relns=_tmp81;_tmp82;});};}case 6U: _LLF: _tmpA7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D)->f1;_LL10:
# 430
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=6U,({struct Cyc_List_List*_tmp659=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA7);_tmp83->f1=_tmp659;});_tmp83;});case 7U: _LL11: _tmpA9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f1;_tmpA8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=7U,_tmp84->f1=_tmpA9,({struct Cyc_List_List*_tmp65A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpA8);_tmp84->f2=_tmp65A;});_tmp84;});case 9U: _LL13: _tmpAA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL14:
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=9U,_tmp85->f1=_tmpAA;_tmp85;});case 11U: _LL15: _tmpAB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6D)->f1;_LL16:
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=11U,_tmp86->f1=_tmpAB;_tmp86;});case 8U: _LL17: _tmpAE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f1;_tmpAD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f2;_tmpAC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f3;_LL18:
# 435
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=8U,_tmp87->f1=_tmpAE,({struct Cyc_List_List*_tmp65B=Cyc_Tcutil_copy_types(_tmpAD);_tmp87->f2=_tmp65B;}),_tmp87->f3=_tmpAC,_tmp87->f4=0;_tmp87;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)){case 0U: _LL19: _tmpAF=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1A: {
# 438
struct Cyc_List_List*_tmp88=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpAF->tvs);
return({union Cyc_Absyn_AggrInfo _tmp65C=Cyc_Absyn_UnknownAggr(_tmpAF->kind,_tmpAF->name,0);Cyc_Absyn_aggr_type(_tmp65C,_tmp88);});}case 1U: _LL1B: _tmpB0=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1C:
# 441
 return Cyc_Absyn_enum_type(_tmpB0->name,0);default: _LL1D: _tmpB1=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1E: {
# 443
struct Cyc_List_List*_tmp89=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB1->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmp65D=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5FF;_tmp5FF.name=_tmpB1->name,_tmp5FF.is_extensible=0;_tmp5FF;}));Cyc_Absyn_datatype_type(_tmp65D,_tmp89);});}}}_LL0:;}
# 450
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB2=d;struct _fat_ptr*_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->tag == 0U){_LL1: _tmpB4=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->tag=0U,({struct Cyc_Absyn_Exp*_tmp65E=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpB4);_tmpB3->f1=_tmp65E;});_tmpB3;});}else{_LL3: _tmpB5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB2)->f1;_LL4:
 return d;}_LL0:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 456
static struct _tuple15*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple15*e){
# 458
return({struct _tuple15*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_List_List*_tmp660=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB6->f1=_tmp660;}),({struct Cyc_Absyn_Exp*_tmp65F=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB6->f2=_tmp65F;});_tmpB6;});}
# 461
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB7=preserve_types;
{void*_tmpB8=e->r;void*_tmpB9=_tmpB8;int _tmp135;struct _fat_ptr _tmp134;void*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct _fat_ptr*_tmp131;struct Cyc_Core_Opt*_tmp130;struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;int _tmp12C;struct Cyc_Absyn_Exp*_tmp12B;void**_tmp12A;struct Cyc_Absyn_Exp*_tmp129;int _tmp128;int _tmp127;void*_tmp126;struct Cyc_Absyn_Enumfield*_tmp125;struct Cyc_Absyn_Enumdecl*_tmp124;struct Cyc_Absyn_Enumfield*_tmp123;struct _fat_ptr*_tmp122;struct Cyc_Absyn_Tqual _tmp121;void*_tmp120;struct Cyc_List_List*_tmp11F;void*_tmp11E;struct Cyc_List_List*_tmp11D;struct _tuple1*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_Absyn_Aggrdecl*_tmp119;struct Cyc_Absyn_Exp*_tmp118;void*_tmp117;int _tmp116;struct Cyc_Absyn_Vardecl*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;int _tmp112;struct Cyc_List_List*_tmp111;struct Cyc_Absyn_Datatypedecl*_tmp110;struct Cyc_Absyn_Datatypefield*_tmp10F;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct _fat_ptr*_tmp10A;int _tmp109;int _tmp108;struct Cyc_Absyn_Exp*_tmp107;struct _fat_ptr*_tmp106;int _tmp105;int _tmp104;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;void*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;void*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;void*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;int _tmpF7;enum Cyc_Absyn_Coercion _tmpF6;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;int _tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_VarargCallInfo*_tmpEE;int _tmpED;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_List_List*_tmpEB;int _tmpEA;struct Cyc_List_List*_tmpE9;struct _fat_ptr*_tmpE8;struct Cyc_Absyn_Tqual _tmpE7;void*_tmpE6;int _tmpE5;int _tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Core_Opt*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;enum Cyc_Absyn_Incrementor _tmpD6;enum Cyc_Absyn_Primop _tmpD5;struct Cyc_List_List*_tmpD4;struct _fat_ptr _tmpD3;void*_tmpD2;union Cyc_Absyn_Cnst _tmpD1;switch(*((int*)_tmpB9)){case 0U: _LL1: _tmpD1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2:
 new_e=Cyc_Absyn_const_exp(_tmpD1,e->loc);goto _LL0;case 1U: _LL3: _tmpD2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL4:
 new_e=Cyc_Absyn_varb_exp(_tmpD2,e->loc);goto _LL0;case 2U: _LL5: _tmpD3=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL6:
 new_e=Cyc_Absyn_pragma_exp(_tmpD3,e->loc);goto _LL0;case 3U: _LL7: _tmpD5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL8:
 new_e=({enum Cyc_Absyn_Primop _tmp662=_tmpD5;struct Cyc_List_List*_tmp661=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpD4);Cyc_Absyn_primop_exp(_tmp662,_tmp661,e->loc);});goto _LL0;case 5U: _LL9: _tmpD7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLA:
 new_e=({struct Cyc_Absyn_Exp*_tmp664=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD7);enum Cyc_Absyn_Incrementor _tmp663=_tmpD6;Cyc_Absyn_increment_exp(_tmp664,_tmp663,e->loc);});goto _LL0;case 7U: _LLB: _tmpD9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLC:
 new_e=({struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD9);struct Cyc_Absyn_Exp*_tmp665=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD8);Cyc_Absyn_and_exp(_tmp666,_tmp665,e->loc);});goto _LL0;case 8U: _LLD: _tmpDB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLE:
 new_e=({struct Cyc_Absyn_Exp*_tmp668=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDB);struct Cyc_Absyn_Exp*_tmp667=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDA);Cyc_Absyn_or_exp(_tmp668,_tmp667,e->loc);});goto _LL0;case 9U: _LLF: _tmpDD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL10:
 new_e=({struct Cyc_Absyn_Exp*_tmp66A=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDD);struct Cyc_Absyn_Exp*_tmp669=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDC);Cyc_Absyn_seq_exp(_tmp66A,_tmp669,e->loc);});goto _LL0;case 6U: _LL11: _tmpE0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpDE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL12:
# 474
 new_e=({struct Cyc_Absyn_Exp*_tmp66D=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE0);struct Cyc_Absyn_Exp*_tmp66C=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDF);struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDE);Cyc_Absyn_conditional_exp(_tmp66D,_tmp66C,_tmp66B,e->loc);});goto _LL0;case 4U: _LL13: _tmpE3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL14:
# 476
 new_e=({struct Cyc_Absyn_Exp*_tmp670=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE3);struct Cyc_Core_Opt*_tmp66F=(unsigned)_tmpE2?({struct Cyc_Core_Opt*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->v=(void*)_tmpE2->v;_tmpBA;}): 0;struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE1);Cyc_Absyn_assignop_exp(_tmp670,_tmp66F,_tmp66E,e->loc);});
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3 != 0){_LL15: _tmpEC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->num_varargs;_tmpE9=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->injectors;_tmpE8=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->name;_tmpE7=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->tq;_tmpE6=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->type;_tmpE5=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->inject;_tmpE4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL16:
# 479
 new_e=({void*_tmp676=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->tag=10U,({
struct Cyc_Absyn_Exp*_tmp675=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpEC);_tmpBD->f1=_tmp675;}),({struct Cyc_List_List*_tmp674=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpEB);_tmpBD->f2=_tmp674;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp673=({struct Cyc_Absyn_VarargCallInfo*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->num_varargs=_tmpEA,_tmpBC->injectors=_tmpE9,({
struct Cyc_Absyn_VarargInfo*_tmp672=({struct Cyc_Absyn_VarargInfo*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->name=_tmpE8,_tmpBB->tq=_tmpE7,({void*_tmp671=Cyc_Tcutil_copy_type(_tmpE6);_tmpBB->type=_tmp671;}),_tmpBB->inject=_tmpE5;_tmpBB;});_tmpBC->vai=_tmp672;});_tmpBC;});
# 481
_tmpBD->f3=_tmp673;}),_tmpBD->f4=_tmpE4;_tmpBD;});
# 479
Cyc_Absyn_new_exp(_tmp676,e->loc);});
# 484
goto _LL0;}else{_LL17: _tmpF0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpEF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpED=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL18:
# 486
 new_e=({void*_tmp679=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=10U,({struct Cyc_Absyn_Exp*_tmp678=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF0);_tmpBE->f1=_tmp678;}),({struct Cyc_List_List*_tmp677=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpEF);_tmpBE->f2=_tmp677;}),_tmpBE->f3=_tmpEE,_tmpBE->f4=_tmpED;_tmpBE;});Cyc_Absyn_new_exp(_tmp679,e->loc);});
goto _LL0;}case 11U: _LL19: _tmpF2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF1=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1A:
# 489
 new_e=_tmpF1?({struct Cyc_Absyn_Exp*_tmp67B=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF2);Cyc_Absyn_rethrow_exp(_tmp67B,e->loc);}):({struct Cyc_Absyn_Exp*_tmp67A=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF2);Cyc_Absyn_throw_exp(_tmp67A,e->loc);});
goto _LL0;case 12U: _LL1B: _tmpF3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL1C:
# 492
 new_e=({struct Cyc_Absyn_Exp*_tmp67C=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF3);Cyc_Absyn_noinstantiate_exp(_tmp67C,e->loc);});goto _LL0;case 13U: _LL1D: _tmpF5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1E:
# 494
 new_e=({struct Cyc_Absyn_Exp*_tmp67E=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF5);struct Cyc_List_List*_tmp67D=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmpF4);Cyc_Absyn_instantiate_exp(_tmp67E,_tmp67D,e->loc);});goto _LL0;case 14U: _LL1F: _tmpF9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpF7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpF6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL20:
# 496
 new_e=({void*_tmp682=Cyc_Tcutil_copy_type(_tmpF9);struct Cyc_Absyn_Exp*_tmp681=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF8);int _tmp680=_tmpF7;enum Cyc_Absyn_Coercion _tmp67F=_tmpF6;Cyc_Absyn_cast_exp(_tmp682,_tmp681,_tmp680,_tmp67F,e->loc);});goto _LL0;case 16U: _LL21: _tmpFB=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL22:
# 498
 new_e=({struct Cyc_Absyn_Exp*_tmp684=(unsigned)_tmpFB?Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFB): 0;struct Cyc_Absyn_Exp*_tmp683=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFA);Cyc_Absyn_New_exp(_tmp684,_tmp683,e->loc);});goto _LL0;case 15U: _LL23: _tmpFC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL24:
 new_e=({struct Cyc_Absyn_Exp*_tmp685=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFC);Cyc_Absyn_address_exp(_tmp685,e->loc);});goto _LL0;case 17U: _LL25: _tmpFD=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL26:
 new_e=({void*_tmp686=Cyc_Tcutil_copy_type(_tmpFD);Cyc_Absyn_sizeoftype_exp(_tmp686,e->loc);});goto _LL0;case 18U: _LL27: _tmpFE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL28:
 new_e=({struct Cyc_Absyn_Exp*_tmp687=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFE);Cyc_Absyn_sizeofexp_exp(_tmp687,e->loc);});goto _LL0;case 19U: _LL29: _tmp100=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFF=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL2A:
 new_e=({void*_tmp689=Cyc_Tcutil_copy_type(_tmp100);struct Cyc_List_List*_tmp688=_tmpFF;Cyc_Absyn_offsetof_exp(_tmp689,_tmp688,e->loc);});goto _LL0;case 20U: _LL2B: _tmp101=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2C:
 new_e=({struct Cyc_Absyn_Exp*_tmp68A=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp101);Cyc_Absyn_deref_exp(_tmp68A,e->loc);});goto _LL0;case 41U: _LL2D: _tmp102=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2E:
 new_e=({struct Cyc_Absyn_Exp*_tmp68B=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp102);Cyc_Absyn_extension_exp(_tmp68B,e->loc);});goto _LL0;case 24U: _LL2F: _tmp103=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL30:
 new_e=({struct Cyc_List_List*_tmp68C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmp103);Cyc_Absyn_tuple_exp(_tmp68C,e->loc);});goto _LL0;case 21U: _LL31: _tmp107=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp106=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp105=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp104=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL32:
# 507
 new_e=({void*_tmp68E=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->tag=21U,({struct Cyc_Absyn_Exp*_tmp68D=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp107);_tmpBF->f1=_tmp68D;}),_tmpBF->f2=_tmp106,_tmpBF->f3=_tmp105,_tmpBF->f4=_tmp104;_tmpBF;});Cyc_Absyn_new_exp(_tmp68E,e->loc);});goto _LL0;case 22U: _LL33: _tmp10B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp10A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp109=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp108=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL34:
# 509
 new_e=({void*_tmp690=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->tag=22U,({struct Cyc_Absyn_Exp*_tmp68F=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10B);_tmpC0->f1=_tmp68F;}),_tmpC0->f2=_tmp10A,_tmpC0->f3=_tmp109,_tmpC0->f4=_tmp108;_tmpC0;});Cyc_Absyn_new_exp(_tmp690,e->loc);});goto _LL0;case 23U: _LL35: _tmp10D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp10C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL36:
# 511
 new_e=({struct Cyc_Absyn_Exp*_tmp692=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10D);struct Cyc_Absyn_Exp*_tmp691=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10C);Cyc_Absyn_subscript_exp(_tmp692,_tmp691,e->loc);});goto _LL0;case 26U: _LL37: _tmp10E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL38:
# 513
 new_e=({void*_tmp694=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->tag=26U,({struct Cyc_List_List*_tmp693=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp10E);_tmpC1->f1=_tmp693;});_tmpC1;});Cyc_Absyn_new_exp(_tmp694,e->loc);});goto _LL0;case 31U: _LL39: _tmp111=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp110=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp10F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3A:
# 515
 new_e=({void*_tmp696=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->tag=31U,({struct Cyc_List_List*_tmp695=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmp111);_tmpC2->f1=_tmp695;}),_tmpC2->f2=_tmp110,_tmpC2->f3=_tmp10F;_tmpC2;});Cyc_Absyn_new_exp(_tmp696,e->loc);});goto _LL0;case 27U: _LL3B: _tmp115=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp114=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp113=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp112=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL3C:
# 517
 new_e=({void*_tmp699=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=27U,_tmpC3->f1=_tmp115,({struct Cyc_Absyn_Exp*_tmp698=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp114);_tmpC3->f2=_tmp698;}),({struct Cyc_Absyn_Exp*_tmp697=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp113);_tmpC3->f3=_tmp697;}),_tmpC3->f4=_tmp112;_tmpC3;});Cyc_Absyn_new_exp(_tmp699,e->loc);});goto _LL0;case 28U: _LL3D: _tmp118=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp117=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp116=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3E:
# 519
 new_e=({void*_tmp69C=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->tag=28U,({struct Cyc_Absyn_Exp*_tmp69B=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp118);_tmpC4->f1=_tmp69B;}),({void*_tmp69A=Cyc_Tcutil_copy_type(_tmp117);_tmpC4->f2=_tmp69A;}),_tmpC4->f3=_tmp116;_tmpC4;});Cyc_Absyn_new_exp(_tmp69C,_tmp118->loc);});
goto _LL0;case 29U: _LL3F: _tmp11C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp11A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp119=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL40:
# 522
 new_e=({void*_tmp69F=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->tag=29U,_tmpC5->f1=_tmp11C,({struct Cyc_List_List*_tmp69E=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp11B);_tmpC5->f2=_tmp69E;}),({struct Cyc_List_List*_tmp69D=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11A);_tmpC5->f3=_tmp69D;}),_tmpC5->f4=_tmp119;_tmpC5;});Cyc_Absyn_new_exp(_tmp69F,e->loc);});
# 524
goto _LL0;case 30U: _LL41: _tmp11E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL42:
# 526
 new_e=({void*_tmp6A2=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=30U,({void*_tmp6A1=Cyc_Tcutil_copy_type(_tmp11E);_tmpC6->f1=_tmp6A1;}),({struct Cyc_List_List*_tmp6A0=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11D);_tmpC6->f2=_tmp6A0;});_tmpC6;});Cyc_Absyn_new_exp(_tmp6A2,e->loc);});
goto _LL0;case 25U: _LL43: _tmp122=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f1;_tmp121=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f2;_tmp120=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f3;_tmp11F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL44:
# 529
 new_e=({void*_tmp6A6=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->tag=25U,({struct _tuple9*_tmp6A5=({struct _tuple9*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=_tmp122,_tmpC7->f2=_tmp121,({void*_tmp6A4=Cyc_Tcutil_copy_type(_tmp120);_tmpC7->f3=_tmp6A4;});_tmpC7;});_tmpC8->f1=_tmp6A5;}),({
struct Cyc_List_List*_tmp6A3=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11F);_tmpC8->f2=_tmp6A3;});_tmpC8;});
# 529
Cyc_Absyn_new_exp(_tmp6A6,e->loc);});
# 531
goto _LL0;case 32U: _LL45: _tmp124=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp123=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL46:
 new_e=e;goto _LL0;case 33U: _LL47: _tmp126=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp125=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL48:
# 534
 new_e=({void*_tmp6A8=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=33U,({void*_tmp6A7=Cyc_Tcutil_copy_type(_tmp126);_tmpC9->f1=_tmp6A7;}),_tmpC9->f2=_tmp125;_tmpC9;});Cyc_Absyn_new_exp(_tmp6A8,e->loc);});goto _LL0;case 34U: _LL49: _tmp12C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).is_calloc;_tmp12B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).rgn;_tmp12A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).elt_type;_tmp129=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).num_elts;_tmp128=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).fat_result;_tmp127=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).inline_call;_LL4A: {
# 536
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp12B;if(_tmp12B != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12B);{
void**t1=_tmp12A;if(_tmp12A != 0)t1=({void**_tmpCA=_cycalloc(sizeof(*_tmpCA));({void*_tmp6A9=Cyc_Tcutil_copy_type(*_tmp12A);*_tmpCA=_tmp6A9;});_tmpCA;});
({void*_tmp6AA=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=34U,(_tmpCB->f1).is_calloc=_tmp12C,(_tmpCB->f1).rgn=r1,(_tmpCB->f1).elt_type=t1,(_tmpCB->f1).num_elts=_tmp129,(_tmpCB->f1).fat_result=_tmp128,(_tmpCB->f1).inline_call=_tmp127;_tmpCB;});e2->r=_tmp6AA;});
new_e=e2;
goto _LL0;};}case 35U: _LL4B: _tmp12E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp12D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4C:
 new_e=({struct Cyc_Absyn_Exp*_tmp6AC=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12E);struct Cyc_Absyn_Exp*_tmp6AB=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12D);Cyc_Absyn_swap_exp(_tmp6AC,_tmp6AB,e->loc);});goto _LL0;case 36U: _LL4D: _tmp130=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp12F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4E: {
# 544
struct Cyc_Core_Opt*nopt1=_tmp130;
if(_tmp130 != 0)nopt1=({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->v=(struct _tuple1*)_tmp130->v;_tmpCC;});
new_e=({void*_tmp6AE=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->tag=36U,_tmpCD->f1=nopt1,({struct Cyc_List_List*_tmp6AD=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp12F);_tmpCD->f2=_tmp6AD;});_tmpCD;});Cyc_Absyn_new_exp(_tmp6AE,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 549
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6AF=({const char*_tmpCE="deep_copy: statement expressions unsupported";_tag_fat(_tmpCE,sizeof(char),45U);});_tmpCF->f1=_tmp6AF;});_tmpCF;}));case 38U: _LL51: _tmp132=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp131=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL52:
# 551
 new_e=({void*_tmp6B1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->tag=38U,({struct Cyc_Absyn_Exp*_tmp6B0=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp132);_tmpD0->f1=_tmp6B0;}),_tmpD0->f2=_tmp131;_tmpD0;});Cyc_Absyn_new_exp(_tmp6B1,e->loc);});goto _LL0;case 39U: _LL53: _tmp133=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL54:
 new_e=({void*_tmp6B2=Cyc_Tcutil_copy_type(_tmp133);Cyc_Absyn_valueof_exp(_tmp6B2,e->loc);});goto _LL0;default: _LL55: _tmp135=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp134=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL56:
 new_e=Cyc_Absyn_asm_exp(_tmp135,_tmp134,e->loc);goto _LL0;}_LL0:;}
# 556
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 560
return new_e;}struct _tuple16{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple17{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 571 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp136=ka1;enum Cyc_Absyn_KindQual _tmp13F;enum Cyc_Absyn_AliasQual _tmp13E;_LL1: _tmp13F=_tmp136->kind;_tmp13E=_tmp136->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp137=ka2;enum Cyc_Absyn_KindQual _tmp13D;enum Cyc_Absyn_AliasQual _tmp13C;_LL4: _tmp13D=_tmp137->kind;_tmp13C=_tmp137->aliasqual;_LL5:;
# 575
if((int)_tmp13F != (int)_tmp13D){
struct _tuple16 _tmp138=({struct _tuple16 _tmp600;_tmp600.f1=_tmp13F,_tmp600.f2=_tmp13D;_tmp600;});struct _tuple16 _tmp139=_tmp138;switch(_tmp139.f1){case Cyc_Absyn_BoxKind: switch(_tmp139.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp139.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 583
if((int)_tmp13E != (int)_tmp13C){
struct _tuple17 _tmp13A=({struct _tuple17 _tmp601;_tmp601.f1=_tmp13E,_tmp601.f2=_tmp13C;_tmp601;});struct _tuple17 _tmp13B=_tmp13A;switch(_tmp13B.f1){case Cyc_Absyn_Aliasable: if(_tmp13B.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp13B.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 589
return 1;};}
# 592
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp140=Cyc_Absyn_compress_kb(tv->kind);void*_tmp141=_tmp140;struct Cyc_Absyn_Kind*_tmp144;struct Cyc_Absyn_Kind*_tmp143;switch(*((int*)_tmp141)){case 0U: _LL1: _tmp143=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp141)->f1;_LL2:
 return _tmp143;case 2U: _LL3: _tmp144=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141)->f2;_LL4:
 return _tmp144;default: _LL5: _LL6:
({void*_tmp6B3=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->tag=2U,_tmp142->f1=0,_tmp142->f2=def;_tmp142;});tv->kind=_tmp6B3;});return def;}_LL0:;}struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 600
struct _tuple18 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp145=Cyc_Tcutil_compress(t);void*_tmp146=_tmp145;struct Cyc_Absyn_Tvar*_tmp14B;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->tag == 2U){_LL1: _tmp14B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->f1;_LL2: {
# 603
void*_tmp147=_tmp14B->kind;
_tmp14B->kind=kb;
return({struct _tuple18 _tmp602;_tmp602.f1=_tmp14B,_tmp602.f2=_tmp147;_tmp602;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp14A=({struct Cyc_String_pa_PrintArg_struct _tmp603;_tmp603.tag=0U,({struct _fat_ptr _tmp6B4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp603.f1=_tmp6B4;});_tmp603;});void*_tmp148[1U];_tmp148[0]=& _tmp14A;({struct _fat_ptr _tmp6B5=({const char*_tmp149="swap_kind: cannot update the kind of %s";_tag_fat(_tmp149,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B5,_tag_fat(_tmp148,sizeof(void*),1U));});});}_LL0:;}
# 612
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 614
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 618
struct _RegionHandle _tmp14C=_new_region("temp");struct _RegionHandle*temp=& _tmp14C;_push_region(temp);
{struct Cyc_List_List*_tmp14D=0;
# 621
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp14E=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp14F=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp150=Cyc_Tcutil_tvar_kind(_tmp14E,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp151=_tmp150;switch(((struct Cyc_Absyn_Kind*)_tmp151)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 627
 _tmp14D=({struct Cyc_List_List*_tmp153=_region_malloc(temp,sizeof(*_tmp153));({struct _tuple18*_tmp6B6=({struct _tuple18*_tmp152=_region_malloc(temp,sizeof(*_tmp152));_tmp152->f1=_tmp14E,_tmp152->f2=_tmp14F;_tmp152;});_tmp153->hd=_tmp6B6;}),_tmp153->tl=_tmp14D;_tmp153;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 631
if(_tmp14D != 0){
field_type=({struct _RegionHandle*_tmp6B8=temp;struct Cyc_List_List*_tmp6B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14D);Cyc_Tcutil_rsubstitute(_tmp6B8,_tmp6B7,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 619
;_pop_region(temp);}
# 636
return k;}
# 643
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 645
void*_tmp154=Cyc_Tcutil_compress(t);void*_tmp155=_tmp154;struct Cyc_Absyn_Typedefdecl*_tmp173;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_PtrInfo _tmp171;void*_tmp170;struct Cyc_List_List*_tmp16F;struct Cyc_Absyn_Tvar*_tmp16E;struct Cyc_Core_Opt*_tmp16D;switch(*((int*)_tmp155)){case 1U: _LL1: _tmp16D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp155)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp16D))->v;case 2U: _LL3: _tmp16E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp155)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp16E,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp170=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f1;_tmp16F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f2;_LL6: {
# 649
void*_tmp156=_tmp170;enum Cyc_Absyn_AggrKind _tmp163;struct Cyc_List_List*_tmp162;struct Cyc_Absyn_AggrdeclImpl*_tmp161;int _tmp160;struct Cyc_Absyn_Kind*_tmp15F;enum Cyc_Absyn_Size_of _tmp15E;switch(*((int*)_tmp156)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp15E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp156)->f2;_LL21:
 return((int)_tmp15E == (int)2U ||(int)_tmp15E == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp15F=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp156)->f2;_LL31:
 return _tmp15F;case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 670
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 672
({void*_tmp157=0U;({struct _fat_ptr _tmp6B9=({const char*_tmp158="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp158,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B9,_tag_fat(_tmp157,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 676
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp163=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->kind;_tmp162=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->tvs;_tmp161=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->impl;_tmp160=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 678
 if(_tmp161 == 0){
if(_tmp160)
return& Cyc_Tcutil_mk;else{
# 682
return& Cyc_Tcutil_ak;}}{
# 684
struct Cyc_List_List*_tmp159=_tmp161->fields;
if(_tmp159 == 0)return& Cyc_Tcutil_mk;
# 687
if((int)_tmp163 == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(_tmp159))->tl != 0;_tmp159=_tmp159->tl){;}{
void*_tmp15A=((struct Cyc_Absyn_Aggrfield*)_tmp159->hd)->type;
struct Cyc_Absyn_Kind*_tmp15B=Cyc_Tcutil_field_kind(_tmp15A,_tmp16F,_tmp162);
if(_tmp15B == & Cyc_Tcutil_ak || _tmp15B == & Cyc_Tcutil_tak)return _tmp15B;};}else{
# 695
for(0;_tmp159 != 0;_tmp159=_tmp159->tl){
void*_tmp15C=((struct Cyc_Absyn_Aggrfield*)_tmp159->hd)->type;
struct Cyc_Absyn_Kind*_tmp15D=Cyc_Tcutil_field_kind(_tmp15C,_tmp16F,_tmp162);
if(_tmp15D == & Cyc_Tcutil_ak || _tmp15D == & Cyc_Tcutil_tak)return _tmp15D;}}
# 701
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 703
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp171=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp155)->f1;_LLC: {
# 706
void*_tmp164=Cyc_Tcutil_compress((_tmp171.ptr_atts).bounds);void*_tmp165=_tmp164;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp165)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp165)->f1)){case 13U: _LL4D: _LL4E: {
# 708
enum Cyc_Absyn_AliasQual _tmp166=(Cyc_Tcutil_type_kind((_tmp171.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp167=_tmp166;switch(_tmp167){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 714
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 716
enum Cyc_Absyn_AliasQual _tmp168=(Cyc_Tcutil_type_kind((_tmp171.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp169=_tmp168;switch(_tmp169){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 722
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 726
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp172=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp155)->f1).num_elts;_LL12:
# 728
 if(_tmp172 == 0 || Cyc_Tcutil_is_const_exp(_tmp172))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp173=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp155)->f3;_LL16:
# 732
 if(_tmp173 == 0 || _tmp173->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp16C=({struct Cyc_String_pa_PrintArg_struct _tmp604;_tmp604.tag=0U,({struct _fat_ptr _tmp6BA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp604.f1=_tmp6BA;});_tmp604;});void*_tmp16A[1U];_tmp16A[0]=& _tmp16C;({struct _fat_ptr _tmp6BB=({const char*_tmp16B="type_kind: typedef found: %s";_tag_fat(_tmp16B,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6BB,_tag_fat(_tmp16A,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp173->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp155)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 741
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 745
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 754
static void*Cyc_Tcutil_rgns_of(void*t);
# 756
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 760
static struct _tuple18*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp174=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp175=_tmp174;switch(((struct Cyc_Absyn_Kind*)_tmp175)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp175)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->tag=9U,({struct Cyc_Absyn_Exp*_tmp6BC=Cyc_Absyn_uint_exp(0U,0U);_tmp176->f1=_tmp6BC;});_tmp176;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 771
return({struct _tuple18*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->f1=tv,_tmp177->f2=t;_tmp177;});}
# 778
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp178=Cyc_Tcutil_compress(t);void*_tmp179=_tmp178;struct Cyc_List_List*_tmp18F;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18D;void*_tmp18C;struct Cyc_Absyn_Tqual _tmp18B;void*_tmp18A;struct Cyc_List_List*_tmp189;struct Cyc_Absyn_VarargInfo*_tmp188;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp186;void*_tmp185;void*_tmp184;void*_tmp183;struct Cyc_List_List*_tmp182;switch(*((int*)_tmp179)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp182=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f2;_LL6: {
# 783
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp182);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 787
struct Cyc_Absyn_Kind*_tmp17A=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp17B=_tmp17A;switch(((struct Cyc_Absyn_Kind*)_tmp17B)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp184=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp179)->f1).elt_type;_tmp183=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp179)->f1).ptr_atts).rgn;_LLC:
# 795
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp17C[2U];({void*_tmp6BE=Cyc_Absyn_access_eff(_tmp183);_tmp17C[0]=_tmp6BE;}),({void*_tmp6BD=Cyc_Tcutil_rgns_of(_tmp184);_tmp17C[1]=_tmp6BD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17C,sizeof(void*),2U));})));case 4U: _LLD: _tmp185=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp179)->f1).elt_type;_LLE:
# 797
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp185));case 7U: _LLF: _tmp186=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp179)->f2;_LL10:
# 799
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp186)));case 5U: _LL11: _tmp18D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).tvars;_tmp18C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).effect;_tmp18B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ret_tqual;_tmp18A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ret_type;_tmp189=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).args;_tmp188=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).cyc_varargs;_tmp187=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).rgn_po;_LL12: {
# 808
void*_tmp17D=({struct Cyc_List_List*_tmp6BF=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp18D);Cyc_Tcutil_substitute(_tmp6BF,(void*)_check_null(_tmp18C));});
return Cyc_Tcutil_normalize_effect(_tmp17D);}case 6U: _LL13: _tmp18E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp179)->f1;_LL14: {
# 811
struct Cyc_List_List*_tmp17E=0;
for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){
_tmp17E=({struct Cyc_List_List*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->hd=(*((struct _tuple11*)_tmp18E->hd)).f2,_tmp17F->tl=_tmp17E;_tmp17F;});}
_tmp18F=_tmp17E;goto _LL16;}case 8U: _LL15: _tmp18F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp179)->f2;_LL16:
# 816
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp18F)));case 10U: _LL17: _LL18:
({void*_tmp180=0U;({struct _fat_ptr _tmp6C0=({const char*_tmp181="typedecl in rgns_of";_tag_fat(_tmp181,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C0,_tag_fat(_tmp180,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 826
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp190=e;void*_tmp19C;struct Cyc_List_List**_tmp19B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f1)){case 9U: _LL1: _tmp19B=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2;_LL2: {
# 830
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp19B;for(0;effs != 0;effs=effs->tl){
void*_tmp191=(void*)effs->hd;
({void*_tmp6C1=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp191));effs->hd=_tmp6C1;});{
void*_tmp192=(void*)effs->hd;void*_tmp193=_tmp192;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LLE: _LLF:
# 839
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 843
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp19B;for(0;effs != 0;effs=effs->tl){
void*_tmp194=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp195=_tmp194;void*_tmp198;struct Cyc_List_List*_tmp197;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)){case 9U: _LL13: _tmp197=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2;_LL14:
# 848
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp197,effects);
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp198=_tmp195;_LL1C:
 effects=({struct Cyc_List_List*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->hd=_tmp198,_tmp196->tl=effects;_tmp196;});goto _LL12;}_LL12:;}}
# 856
({struct Cyc_List_List*_tmp6C2=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp19B=_tmp6C2;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2 != 0){_LL3: _tmp19C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2)->hd;_LL4: {
# 859
void*_tmp199=Cyc_Tcutil_compress(_tmp19C);void*_tmp19A=_tmp199;switch(*((int*)_tmp19A)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp19C);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 864
 return e;}_LL0:;};}
# 869
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19D=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=5U,(_tmp19E->f1).tvars=0,(_tmp19E->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6C3=Cyc_Absyn_empty_tqual(0U);(_tmp19E->f1).ret_tqual=_tmp6C3;}),(_tmp19E->f1).ret_type=Cyc_Absyn_void_type,(_tmp19E->f1).args=0,(_tmp19E->f1).c_varargs=0,(_tmp19E->f1).cyc_varargs=0,(_tmp19E->f1).rgn_po=0,(_tmp19E->f1).attributes=0,(_tmp19E->f1).requires_clause=0,(_tmp19E->f1).requires_relns=0,(_tmp19E->f1).ensures_clause=0,(_tmp19E->f1).ensures_relns=0;_tmp19E;});
# 879
return({void*_tmp6C7=(void*)_tmp19D;void*_tmp6C6=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6C5=Cyc_Absyn_empty_tqual(0U);void*_tmp6C4=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp6C7,_tmp6C6,_tmp6C5,_tmp6C4,Cyc_Absyn_false_type);});}
# 886
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp19F=Cyc_Tcutil_compress(e);void*_tmp1A0=_tmp19F;struct Cyc_Core_Opt*_tmp1BA;void**_tmp1B9;struct Cyc_Core_Opt*_tmp1B8;void*_tmp1B7;struct Cyc_List_List*_tmp1B6;void*_tmp1B5;switch(*((int*)_tmp1A0)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2 != 0){_LL1: _tmp1B5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->hd;_LL2:
# 893
 if(constrain)return Cyc_Unify_unify(r,_tmp1B5);
_tmp1B5=Cyc_Tcutil_compress(_tmp1B5);
if(r == _tmp1B5)return 1;{
struct _tuple14 _tmp1A1=({struct _tuple14 _tmp605;_tmp605.f1=r,_tmp605.f2=_tmp1B5;_tmp605;});struct _tuple14 _tmp1A2=_tmp1A1;struct Cyc_Absyn_Tvar*_tmp1A4;struct Cyc_Absyn_Tvar*_tmp1A3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2.f2)->tag == 2U){_LLC: _tmp1A4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2.f1)->f1;_tmp1A3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp1A4,_tmp1A3)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp1B6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2;_LL4:
# 901
 for(0;_tmp1B6 != 0;_tmp1B6=_tmp1B6->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp1B6->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2 != 0){_LL5: _tmp1B7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->hd;_LL6: {
# 905
void*_tmp1A5=Cyc_Tcutil_rgns_of(_tmp1B7);void*_tmp1A6=_tmp1A5;void*_tmp1AF;void*_tmp1AE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2 != 0){_LL11: _tmp1AE=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2)->hd;_LL12:
# 907
 if(!constrain)return 0;{
void*_tmp1A7=Cyc_Tcutil_compress(_tmp1AE);void*_tmp1A8=_tmp1A7;struct Cyc_Core_Opt*_tmp1AD;void**_tmp1AC;struct Cyc_Core_Opt*_tmp1AB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8)->tag == 1U){_LL16: _tmp1AD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8)->f1;_tmp1AC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8)->f2;_tmp1AB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8)->f4;_LL17: {
# 912
void*_tmp1A9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1AB);
# 915
Cyc_Unify_occurs(_tmp1A9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1AB))->v,r);
({void*_tmp6C9=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1AA[2U];_tmp1AA[0]=_tmp1A9,({void*_tmp6C8=Cyc_Absyn_access_eff(r);_tmp1AA[1]=_tmp6C8;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1AA,sizeof(void*),2U));})));*_tmp1AC=_tmp6C9;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1AF=_tmp1A6;_LL14:
# 920
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp1AF);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1BA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f1;_tmp1B9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f2;_tmp1B8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f4;_LL8:
# 923
 if(_tmp1BA == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1BA->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1B0=0U;({struct _fat_ptr _tmp6CA=({const char*_tmp1B1="effect evar has wrong kind";_tag_fat(_tmp1B1,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CA,_tag_fat(_tmp1B0,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 928
void*_tmp1B2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1B8);
# 931
Cyc_Unify_occurs(_tmp1B2,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1B8))->v,r);{
void*_tmp1B3=Cyc_Absyn_join_eff(({void*_tmp1B4[2U];_tmp1B4[0]=_tmp1B2,({void*_tmp6CB=Cyc_Absyn_access_eff(r);_tmp1B4[1]=_tmp6CB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B4,sizeof(void*),2U));}));
*_tmp1B9=_tmp1B3;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 942
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1BB=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp1BC=_tmp1BB;struct Cyc_Core_Opt*_tmp1CE;void**_tmp1CD;struct Cyc_Core_Opt*_tmp1CC;void*_tmp1CB;struct Cyc_List_List*_tmp1CA;switch(*((int*)_tmp1BC)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1CA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2;_LL4:
# 947
 for(0;_tmp1CA != 0;_tmp1CA=_tmp1CA->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1CA->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2 != 0){_LL5: _tmp1CB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2)->hd;_LL6:
# 952
 _tmp1CB=Cyc_Tcutil_compress(_tmp1CB);
if(t == _tmp1CB)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,_tmp1CB);{
void*_tmp1BD=Cyc_Tcutil_rgns_of(t);void*_tmp1BE=_tmp1BD;void*_tmp1C4;void*_tmp1C3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f2 != 0){_LLC: _tmp1C3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f2)->hd;_LLD: {
# 957
struct _tuple14 _tmp1BF=({struct _tuple14 _tmp606;({void*_tmp6CC=Cyc_Tcutil_compress(_tmp1C3);_tmp606.f1=_tmp6CC;}),_tmp606.f2=_tmp1CB;_tmp606;});struct _tuple14 _tmp1C0=_tmp1BF;struct Cyc_Absyn_Tvar*_tmp1C2;struct Cyc_Absyn_Tvar*_tmp1C1;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C0.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C0.f2)->tag == 2U){_LL11: _tmp1C2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C0.f1)->f1;_tmp1C1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C0.f2)->f1;_LL12:
 return Cyc_Unify_unify(t,_tmp1CB);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp1C3 == _tmp1CB;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1C4=_tmp1BE;_LLF:
# 961
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp1C4);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1CE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f1;_tmp1CD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f2;_tmp1CC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f4;_LL8:
# 964
 if(_tmp1CE == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1CE->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1C5=0U;({struct _fat_ptr _tmp6CD=({const char*_tmp1C6="effect evar has wrong kind";_tag_fat(_tmp1C6,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CD,_tag_fat(_tmp1C5,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 969
void*_tmp1C7=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1CC);
# 972
Cyc_Unify_occurs(_tmp1C7,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CC))->v,t);{
void*_tmp1C8=Cyc_Absyn_join_eff(({void*_tmp1C9[2U];_tmp1C9[0]=_tmp1C7,({void*_tmp6CE=Cyc_Absyn_regionsof_eff(t);_tmp1C9[1]=_tmp6CE;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C9,sizeof(void*),2U));}));
*_tmp1CD=_tmp1C8;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 983
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1CF=e;struct Cyc_Core_Opt*_tmp1E5;void**_tmp1E4;struct Cyc_Core_Opt*_tmp1E3;void*_tmp1E2;struct Cyc_List_List*_tmp1E1;struct Cyc_Absyn_Tvar*_tmp1E0;switch(*((int*)_tmp1CF)){case 2U: _LL1: _tmp1E0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CF)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp1E0)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f1)){case 9U: _LL3: _tmp1E1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2;_LL4:
# 988
 for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp1E1->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2 != 0){_LL5: _tmp1E2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2)->hd;_LL6: {
# 993
void*_tmp1D0=Cyc_Tcutil_rgns_of(_tmp1E2);void*_tmp1D1=_tmp1D0;void*_tmp1DA;void*_tmp1D9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2 != 0){_LLC: _tmp1D9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2)->hd;_LLD:
# 995
 if(!may_constrain_evars)return 0;{
void*_tmp1D2=Cyc_Tcutil_compress(_tmp1D9);void*_tmp1D3=_tmp1D2;struct Cyc_Core_Opt*_tmp1D8;void**_tmp1D7;struct Cyc_Core_Opt*_tmp1D6;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->tag == 1U){_LL11: _tmp1D8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f1;_tmp1D7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f2;_tmp1D6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f4;_LL12: {
# 1001
void*_tmp1D4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1D6);
# 1003
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D6))->v,v))return 0;
({void*_tmp6D0=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1D5[2U];_tmp1D5[0]=_tmp1D4,({void*_tmp6CF=Cyc_Absyn_var_type(v);_tmp1D5[1]=_tmp6CF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D5,sizeof(void*),2U));})));*_tmp1D7=_tmp6D0;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1DA=_tmp1D1;_LLF:
# 1008
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp1DA);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1E5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f1;_tmp1E4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f2;_tmp1E3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f4;_LL8:
# 1011
 if(_tmp1E5 == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1E5->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1DB=0U;({struct _fat_ptr _tmp6D1=({const char*_tmp1DC="effect evar has wrong kind";_tag_fat(_tmp1DC,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D1,_tag_fat(_tmp1DB,sizeof(void*),0U));});});{
# 1015
void*_tmp1DD=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1E3);
# 1017
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E3))->v,v))
return 0;{
void*_tmp1DE=Cyc_Absyn_join_eff(({void*_tmp1DF[2U];_tmp1DF[0]=_tmp1DD,({void*_tmp6D2=Cyc_Absyn_var_type(v);_tmp1DF[1]=_tmp6D2;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1DF,sizeof(void*),2U));}));
*_tmp1E4=_tmp1DE;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1027
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1E6=e;void*_tmp1EC;struct Cyc_List_List*_tmp1EB;switch(*((int*)_tmp1E6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f1)){case 9U: _LL1: _tmp1EB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2;_LL2:
# 1031
 for(0;_tmp1EB != 0;_tmp1EB=_tmp1EB->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp1EB->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2 != 0){_LL3: _tmp1EC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2)->hd;_LL4: {
# 1036
void*_tmp1E7=Cyc_Tcutil_rgns_of(_tmp1EC);void*_tmp1E8=_tmp1E7;void*_tmp1EA;void*_tmp1E9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f2 != 0){_LLA: _tmp1E9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1EA=_tmp1E8;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp1EA);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1040
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1054 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1059
void*_tmp1ED=Cyc_Tcutil_compress(e1);void*_tmp1EE=_tmp1ED;void**_tmp1FB;struct Cyc_Core_Opt*_tmp1FA;struct Cyc_Absyn_Tvar*_tmp1F9;void*_tmp1F8;void*_tmp1F7;struct Cyc_List_List*_tmp1F6;switch(*((int*)_tmp1EE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f1)){case 9U: _LL1: _tmp1F6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2;_LL2:
# 1061
 for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp1F6->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2 != 0){_LL3: _tmp1F7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2)->hd;_LL4:
# 1072
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp1F7,e2)||
 may_constrain_evars && Cyc_Unify_unify(_tmp1F7,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2 != 0){_LL7: _tmp1F8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2)->hd;_LL8: {
# 1076
void*_tmp1EF=Cyc_Tcutil_rgns_of(_tmp1F8);void*_tmp1F0=_tmp1EF;void*_tmp1F2;void*_tmp1F1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f2 != 0){_LLE: _tmp1F1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f2)->hd;_LLF:
# 1081
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp1F1,e2)||
 may_constrain_evars && Cyc_Unify_unify(_tmp1F1,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1F2=_tmp1F0;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp1F2,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1F9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EE)->f1;_LL6:
# 1074
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp1F9,e2);case 1U: _LL9: _tmp1FB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EE)->f2;_tmp1FA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EE)->f4;_LLA:
# 1086
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1090
*_tmp1FB=Cyc_Absyn_empty_effect;
# 1093
return 1;}else{
# 1095
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp1F5=({struct Cyc_String_pa_PrintArg_struct _tmp607;_tmp607.tag=0U,({struct _fat_ptr _tmp6D3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp607.f1=_tmp6D3;});_tmp607;});void*_tmp1F3[1U];_tmp1F3[0]=& _tmp1F5;({struct _fat_ptr _tmp6D4=({const char*_tmp1F4="subset_effect: bad effect: %s";_tag_fat(_tmp1F4,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D4,_tag_fat(_tmp1F3,sizeof(void*),1U));});});}_LL0:;}
# 1106
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1108
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple14*_tmp1FC=(struct _tuple14*)r1->hd;struct _tuple14*_tmp1FD=_tmp1FC;void*_tmp203;void*_tmp202;_LL1: _tmp203=_tmp1FD->f1;_tmp202=_tmp1FD->f2;_LL2:;{
int found=_tmp203 == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple14*_tmp1FE=(struct _tuple14*)r2->hd;struct _tuple14*_tmp1FF=_tmp1FE;void*_tmp201;void*_tmp200;_LL4: _tmp201=_tmp1FF->f1;_tmp200=_tmp1FF->f2;_LL5:;
if(Cyc_Unify_unify(_tmp203,_tmp201)&& Cyc_Unify_unify(_tmp202,_tmp200)){
found=1;
break;}}}
# 1118
if(!found)return 0;};}}
# 1120
return 1;}
# 1126
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1128
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1131
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp204=t;switch(*((int*)_tmp204)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp204)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp204)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1137
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1140
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1143
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1146
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp204)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1135
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1138
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1141
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1144
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1147
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp204)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1136
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1139
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1142
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1145
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1148
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1170
({void*_tmp205=0U;({struct _fat_ptr _tmp6D5=({const char*_tmp206="bad con";_tag_fat(_tmp206,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D5,_tag_fat(_tmp205,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp204)->f1){case 0U: _LL21: _LL22:
# 1149
 return 16;case 1U: _LL23: _LL24:
 return 17;default: _LL25: _LL26:
 return 18;}case 3U: _LL27: _LL28:
 return 19;case 4U: _LL29: _LL2A:
 return 20;case 5U: _LL2B: _LL2C:
 return 21;case 6U: _LL2D: _LL2E:
 return 22;case 7U: _LL2F: _LL30:
 return 23;case 8U: _LL31: _LL32:
 return 24;case 9U: _LL33: _LL34:
 return 25;case 10U: _LL35: _LL36:
 return 26;case 11U: _LL37: _LL38:
 return 27;case 12U: _LL39: _LL3A:
 return 28;case 14U: _LL3B: _LL3C:
 return 29;case 13U: _LL3D: _LL3E:
 return 30;case 15U: _LL3F: _LL40:
 return 31;case 16U: _LL41: _LL42:
 return 32;case 17U: _LL43: _LL44:
 return 33;case 18U: _LL45: _LL46:
 return 34;case 19U: _LL47: _LL48:
 return 35;default: _LL49: _LL4A:
 return 36;}_LL0:;}
# 1173
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp207=t;void*_tmp208;switch(*((int*)_tmp207)){case 1U: _LL1: _LL2:
 return 1;case 2U: _LL3: _LL4:
 return 2;case 3U: _LL5: _LL6:
 return 3;case 4U: _LL7: _LL8:
 return 4;case 5U: _LL9: _LLA:
 return 5;case 6U: _LLB: _LLC:
 return 6;case 7U: _LLD: _LLE:
 return 7;case 8U: _LLF: _LL10:
 return 8;case 9U: _LL11: _LL12:
 return 9;case 10U: _LL13: _LL14:
 return 10;case 11U: _LL15: _LL16:
 return 11;default: _LL17: _tmp208=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp208);}_LL0:;}
# 1189
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp209=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp209 != 0)return _tmp209;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1194
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp20A=i;struct _tuple1*_tmp20C;struct Cyc_Absyn_Datatypedecl*_tmp20B;if((_tmp20A.KnownDatatype).tag == 2){_LL1: _tmp20B=*(_tmp20A.KnownDatatype).val;_LL2:
 return _tmp20B->name;}else{_LL3: _tmp20C=((_tmp20A.UnknownDatatype).val).name;_LL4:
 return _tmp20C;}_LL0:;}struct _tuple19{struct _tuple1*f1;struct _tuple1*f2;};
# 1200
static struct _tuple19 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp20D=i;struct _tuple1*_tmp211;struct _tuple1*_tmp210;struct Cyc_Absyn_Datatypedecl*_tmp20F;struct Cyc_Absyn_Datatypefield*_tmp20E;if((_tmp20D.KnownDatatypefield).tag == 2){_LL1: _tmp20F=((_tmp20D.KnownDatatypefield).val).f1;_tmp20E=((_tmp20D.KnownDatatypefield).val).f2;_LL2:
# 1203
 return({struct _tuple19 _tmp608;_tmp608.f1=_tmp20F->name,_tmp608.f2=_tmp20E->name;_tmp608;});}else{_LL3: _tmp211=((_tmp20D.UnknownDatatypefield).val).datatype_name;_tmp210=((_tmp20D.UnknownDatatypefield).val).field_name;_LL4:
# 1205
 return({struct _tuple19 _tmp609;_tmp609.f1=_tmp211,_tmp609.f2=_tmp210;_tmp609;});}_LL0:;}struct _tuple20{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1208
static struct _tuple20 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp212=i;struct Cyc_Absyn_Aggrdecl*_tmp215;enum Cyc_Absyn_AggrKind _tmp214;struct _tuple1*_tmp213;if((_tmp212.UnknownAggr).tag == 1){_LL1: _tmp214=((_tmp212.UnknownAggr).val).f1;_tmp213=((_tmp212.UnknownAggr).val).f2;_LL2:
 return({struct _tuple20 _tmp60A;_tmp60A.f1=_tmp214,_tmp60A.f2=_tmp213;_tmp60A;});}else{_LL3: _tmp215=*(_tmp212.KnownAggr).val;_LL4:
 return({struct _tuple20 _tmp60B;_tmp60B.f1=_tmp215->kind,_tmp60B.f2=_tmp215->name;_tmp60B;});}_LL0:;}
# 1214
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1220
struct _tuple14 _tmp216=({struct _tuple14 _tmp60C;_tmp60C.f1=t1,_tmp60C.f2=t2;_tmp60C;});struct _tuple14 _tmp217=_tmp216;union Cyc_Absyn_AggrInfo _tmp235;union Cyc_Absyn_AggrInfo _tmp234;union Cyc_Absyn_DatatypeFieldInfo _tmp233;union Cyc_Absyn_DatatypeFieldInfo _tmp232;union Cyc_Absyn_DatatypeInfo _tmp231;union Cyc_Absyn_DatatypeInfo _tmp230;struct Cyc_List_List*_tmp22F;struct Cyc_List_List*_tmp22E;struct _fat_ptr _tmp22D;struct _fat_ptr _tmp22C;struct _tuple1*_tmp22B;struct _tuple1*_tmp22A;switch(*((int*)_tmp217.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 15U){_LL1: _tmp22B=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp22A=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp22B,_tmp22A);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 17U){_LL3: _tmp22D=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp22C=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL4:
 return Cyc_strcmp((struct _fat_ptr)_tmp22D,(struct _fat_ptr)_tmp22C);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 16U){_LL5: _tmp22F=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp22E=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL6:
# 1224
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp22F,_tmp22E);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 18U){_LL7: _tmp231=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp230=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL8: {
# 1226
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(_tmp231);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(_tmp230);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 19U){_LL9: _tmp233=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp232=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LLA: {
# 1230
struct _tuple19 _tmp218=Cyc_Tcutil_get_datatype_field_qvars(_tmp233);struct _tuple19 _tmp219=_tmp218;struct _tuple1*_tmp220;struct _tuple1*_tmp21F;_LL10: _tmp220=_tmp219.f1;_tmp21F=_tmp219.f2;_LL11:;{
struct _tuple19 _tmp21A=Cyc_Tcutil_get_datatype_field_qvars(_tmp232);struct _tuple19 _tmp21B=_tmp21A;struct _tuple1*_tmp21E;struct _tuple1*_tmp21D;_LL13: _tmp21E=_tmp21B.f1;_tmp21D=_tmp21B.f2;_LL14:;{
int _tmp21C=Cyc_Absyn_qvar_cmp(_tmp220,_tmp21E);
if(_tmp21C != 0)return _tmp21C;
return Cyc_Absyn_qvar_cmp(_tmp21F,_tmp21D);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 20U){_LLB: _tmp235=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp234=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LLC: {
# 1236
struct _tuple20 _tmp221=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp235);struct _tuple20 _tmp222=_tmp221;enum Cyc_Absyn_AggrKind _tmp229;struct _tuple1*_tmp228;_LL16: _tmp229=_tmp222.f1;_tmp228=_tmp222.f2;_LL17:;{
struct _tuple20 _tmp223=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp234);struct _tuple20 _tmp224=_tmp223;enum Cyc_Absyn_AggrKind _tmp227;struct _tuple1*_tmp226;_LL19: _tmp227=_tmp224.f1;_tmp226=_tmp224.f2;_LL1A:;{
int _tmp225=Cyc_Absyn_qvar_cmp(_tmp228,_tmp226);
if(_tmp225 != 0)return _tmp225;
return(int)_tmp229 - (int)_tmp227;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}
# 1245
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6D7)(void*,void*)=cmp;void*_tmp6D6=(void*)_check_null(a1);_tmp6D7(_tmp6D6,(void*)_check_null(a2));});}
# 1251
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp236=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp237=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp236,_tmp237);}
# 1256
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct _tuple11*tqt2){
struct _tuple11*_tmp238=tqt1;struct Cyc_Absyn_Tqual _tmp23E;void*_tmp23D;_LL1: _tmp23E=_tmp238->f1;_tmp23D=_tmp238->f2;_LL2:;{
struct _tuple11*_tmp239=tqt2;struct Cyc_Absyn_Tqual _tmp23C;void*_tmp23B;_LL4: _tmp23C=_tmp239->f1;_tmp23B=_tmp239->f2;_LL5:;{
int _tmp23A=Cyc_Tcutil_tqual_cmp(_tmp23E,_tmp23C);
if(_tmp23A != 0)return _tmp23A;
return Cyc_Tcutil_typecmp(_tmp23D,_tmp23B);};};}
# 1264
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp23F=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp23F != 0)return _tmp23F;{
int _tmp240=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp240 != 0)return _tmp240;{
int _tmp241=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp241 != 0)return _tmp241;{
int _tmp242=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp242 != 0)return _tmp242;
_tmp242=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp242 != 0)return _tmp242;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1281
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp243=({int _tmp6D8=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6D8,Cyc_Tcutil_type_case_number(t2));});
if(_tmp243 != 0)
return _tmp243;{
# 1290
struct _tuple14 _tmp244=({struct _tuple14 _tmp60D;_tmp60D.f1=t2,_tmp60D.f2=t1;_tmp60D;});struct _tuple14 _tmp245=_tmp244;struct Cyc_Absyn_Exp*_tmp291;struct Cyc_Absyn_Exp*_tmp290;struct Cyc_Absyn_Exp*_tmp28F;struct Cyc_Absyn_Exp*_tmp28E;enum Cyc_Absyn_AggrKind _tmp28D;struct Cyc_List_List*_tmp28C;enum Cyc_Absyn_AggrKind _tmp28B;struct Cyc_List_List*_tmp28A;struct Cyc_List_List*_tmp289;struct Cyc_List_List*_tmp288;struct Cyc_Absyn_FnInfo _tmp287;struct Cyc_Absyn_FnInfo _tmp286;void*_tmp285;struct Cyc_Absyn_Tqual _tmp284;struct Cyc_Absyn_Exp*_tmp283;void*_tmp282;void*_tmp281;struct Cyc_Absyn_Tqual _tmp280;struct Cyc_Absyn_Exp*_tmp27F;void*_tmp27E;void*_tmp27D;struct Cyc_Absyn_Tqual _tmp27C;void*_tmp27B;void*_tmp27A;void*_tmp279;void*_tmp278;void*_tmp277;struct Cyc_Absyn_Tqual _tmp276;void*_tmp275;void*_tmp274;void*_tmp273;void*_tmp272;struct Cyc_Absyn_Tvar*_tmp271;struct Cyc_Absyn_Tvar*_tmp270;void*_tmp26F;struct Cyc_List_List*_tmp26E;void*_tmp26D;struct Cyc_List_List*_tmp26C;switch(*((int*)_tmp245.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->tag == 0U){_LL1: _tmp26F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp26E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f1)->f2;_tmp26D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->f1;_tmp26C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->f2;_LL2: {
# 1292
int _tmp246=Cyc_Tcutil_tycon_cmp(_tmp26F,_tmp26D);
if(_tmp246 != 0)return _tmp246;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp26E,_tmp26C);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp245.f2)->tag == 1U){_LL3: _LL4:
# 1296
({void*_tmp247=0U;({struct _fat_ptr _tmp6D9=({const char*_tmp248="typecmp: can only compare closed types";_tag_fat(_tmp248,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D9,_tag_fat(_tmp247,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f2)->tag == 2U){_LL5: _tmp271=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp270=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL6:
# 1300
 return Cyc_Core_intcmp(_tmp270->identity,_tmp271->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->tag == 3U){_LL7: _tmp27D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_type;_tmp27C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_tq;_tmp27B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).rgn;_tmp27A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).nullable;_tmp279=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).bounds;_tmp278=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).zero_term;_tmp277=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_type;_tmp276=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_tq;_tmp275=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).rgn;_tmp274=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).nullable;_tmp273=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).bounds;_tmp272=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).zero_term;_LL8: {
# 1304
int _tmp249=Cyc_Tcutil_typecmp(_tmp277,_tmp27D);
if(_tmp249 != 0)return _tmp249;{
int _tmp24A=Cyc_Tcutil_typecmp(_tmp275,_tmp27B);
if(_tmp24A != 0)return _tmp24A;{
int _tmp24B=Cyc_Tcutil_tqual_cmp(_tmp276,_tmp27C);
if(_tmp24B != 0)return _tmp24B;{
int _tmp24C=Cyc_Tcutil_typecmp(_tmp273,_tmp279);
if(_tmp24C != 0)return _tmp24C;{
int _tmp24D=Cyc_Tcutil_typecmp(_tmp272,_tmp278);
if(_tmp24D != 0)return _tmp24D;{
int _tmp24E=Cyc_Tcutil_typecmp(_tmp273,_tmp279);
if(_tmp24E != 0)return _tmp24E;
return Cyc_Tcutil_typecmp(_tmp274,_tmp27A);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->tag == 4U){_LL9: _tmp285=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_type;_tmp284=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).tq;_tmp283=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).num_elts;_tmp282=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).zero_term;_tmp281=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_type;_tmp280=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).tq;_tmp27F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).num_elts;_tmp27E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).zero_term;_LLA: {
# 1320
int _tmp24F=Cyc_Tcutil_tqual_cmp(_tmp280,_tmp284);
if(_tmp24F != 0)return _tmp24F;{
int _tmp250=Cyc_Tcutil_typecmp(_tmp281,_tmp285);
if(_tmp250 != 0)return _tmp250;{
int _tmp251=Cyc_Tcutil_typecmp(_tmp282,_tmp27E);
if(_tmp251 != 0)return _tmp251;
if(_tmp283 == _tmp27F)return 0;
if(_tmp283 == 0 || _tmp27F == 0)
({void*_tmp252=0U;({struct _fat_ptr _tmp6DA=({const char*_tmp253="missing expression in array index";_tag_fat(_tmp253,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DA,_tag_fat(_tmp252,sizeof(void*),0U));});});
# 1330
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp283,_tmp27F);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f2)->tag == 5U){_LLB: _tmp287=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp286=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f2)->f1;_LLC:
# 1333
 if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp287.ret_type,_tmp286.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(_tmp287.ret_tqual,_tmp286.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp254=_tmp287.args;
struct Cyc_List_List*_tmp255=_tmp286.args;
for(0;_tmp254 != 0 && _tmp255 != 0;(_tmp254=_tmp254->tl,_tmp255=_tmp255->tl)){
struct _tuple9 _tmp256=*((struct _tuple9*)_tmp254->hd);struct _tuple9 _tmp257=_tmp256;struct Cyc_Absyn_Tqual _tmp25D;void*_tmp25C;_LL18: _tmp25D=_tmp257.f2;_tmp25C=_tmp257.f3;_LL19:;{
struct _tuple9 _tmp258=*((struct _tuple9*)_tmp255->hd);struct _tuple9 _tmp259=_tmp258;struct Cyc_Absyn_Tqual _tmp25B;void*_tmp25A;_LL1B: _tmp25B=_tmp259.f2;_tmp25A=_tmp259.f3;_LL1C:;
r=Cyc_Tcutil_tqual_cmp(_tmp25D,_tmp25B);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp25C,_tmp25A);
if(r != 0)return r;};}
# 1348
if(_tmp254 != 0)return 1;
if(_tmp255 != 0)return - 1;
if(_tmp287.c_varargs && !_tmp286.c_varargs)return 1;
if(!_tmp287.c_varargs && _tmp286.c_varargs)return - 1;
if(_tmp287.cyc_varargs != 0 & _tmp286.cyc_varargs == 0)return 1;
if(_tmp287.cyc_varargs == 0 & _tmp286.cyc_varargs != 0)return - 1;
if(_tmp287.cyc_varargs != 0 & _tmp286.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp6DB=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp287.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp6DB,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp286.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((_tmp287.cyc_varargs)->type,(_tmp286.cyc_varargs)->type);
if(r != 0)return r;
if((_tmp287.cyc_varargs)->inject && !(_tmp286.cyc_varargs)->inject)return 1;
if(!(_tmp287.cyc_varargs)->inject &&(_tmp286.cyc_varargs)->inject)return - 1;}
# 1362
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp287.effect,_tmp286.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp25E=_tmp287.rgn_po;
struct Cyc_List_List*_tmp25F=_tmp286.rgn_po;
for(0;_tmp25E != 0 && _tmp25F != 0;(_tmp25E=_tmp25E->tl,_tmp25F=_tmp25F->tl)){
struct _tuple14 _tmp260=*((struct _tuple14*)_tmp25E->hd);struct _tuple14 _tmp261=_tmp260;void*_tmp267;void*_tmp266;_LL1E: _tmp267=_tmp261.f1;_tmp266=_tmp261.f2;_LL1F:;{
struct _tuple14 _tmp262=*((struct _tuple14*)_tmp25F->hd);struct _tuple14 _tmp263=_tmp262;void*_tmp265;void*_tmp264;_LL21: _tmp265=_tmp263.f1;_tmp264=_tmp263.f2;_LL22:;
r=Cyc_Tcutil_typecmp(_tmp267,_tmp265);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp266,_tmp264);if(r != 0)return r;};}
# 1372
if(_tmp25E != 0)return 1;
if(_tmp25F != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp287.requires_clause,_tmp286.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp287.ensures_clause,_tmp286.ensures_clause);
if(r != 0)return r;
# 1380
({void*_tmp268=0U;({struct _fat_ptr _tmp6DC=({const char*_tmp269="typecmp: function type comparison should never get here!";_tag_fat(_tmp269,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DC,_tag_fat(_tmp268,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f2)->tag == 6U){_LLD: _tmp289=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp288=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f2)->f1;_LLE:
# 1383
 return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp288,_tmp289);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->tag == 7U){_LLF: _tmp28D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp28C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f1)->f2;_tmp28B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->f1;_tmp28A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->f2;_LL10:
# 1386
 if((int)_tmp28B != (int)_tmp28D){
if((int)_tmp28B == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp28A,_tmp28C);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f2)->tag == 9U){_LL11: _tmp28F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp28E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL12:
# 1391
 _tmp291=_tmp28F;_tmp290=_tmp28E;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f2)->tag == 11U){_LL13: _tmp291=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp290=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL14:
# 1393
 return Cyc_Evexp_const_exp_cmp(_tmp291,_tmp290);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp26A=0U;({struct _fat_ptr _tmp6DD=({const char*_tmp26B="Unmatched case in typecmp";_tag_fat(_tmp26B,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DD,_tag_fat(_tmp26A,sizeof(void*),0U));});});}_LL0:;};};}
# 1400
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple14 _tmp292=({struct _tuple14 _tmp60F;({void*_tmp6DF=Cyc_Tcutil_compress(t1);_tmp60F.f1=_tmp6DF;}),({void*_tmp6DE=Cyc_Tcutil_compress(t2);_tmp60F.f2=_tmp6DE;});_tmp60F;});struct _tuple14 _tmp293=_tmp292;void*_tmp299;void*_tmp298;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp293.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp293.f2)->tag == 0U){_LL1: _tmp299=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp293.f1)->f1;_tmp298=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp293.f2)->f1;_LL2: {
# 1403
struct _tuple14 _tmp294=({struct _tuple14 _tmp60E;_tmp60E.f1=_tmp299,_tmp60E.f2=_tmp298;_tmp60E;});struct _tuple14 _tmp295=_tmp294;int _tmp297;int _tmp296;switch(*((int*)_tmp295.f1)){case 2U: switch(*((int*)_tmp295.f2)){case 2U: _LL6: _tmp297=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp295.f1)->f1;_tmp296=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp295.f2)->f1;_LL7:
 return _tmp296 < _tmp297;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp295.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1411
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1416
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1419
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp295.f2)->f1 == 0){_LL14: _LL15:
# 1414
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp295.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1412
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1417
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1420
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp295.f2)->f1 == 0){_LL16: _LL17:
# 1415
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1421
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp295.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1418
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1422
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1424
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1426
 return 0;}_LL0:;}
# 1430
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple14 _tmp29A=({struct _tuple14 _tmp611;({void*_tmp6E1=Cyc_Tcutil_compress(t1);_tmp611.f1=_tmp6E1;}),({void*_tmp6E0=Cyc_Tcutil_compress(t2);_tmp611.f2=_tmp6E0;});_tmp611;});struct _tuple14 _tmp29B=_tmp29A;void*_tmp2A1;void*_tmp2A0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B.f2)->tag == 0U){_LL1: _tmp2A1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B.f1)->f1;_tmp2A0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B.f2)->f1;_LL2:
# 1433
{struct _tuple14 _tmp29C=({struct _tuple14 _tmp610;_tmp610.f1=_tmp2A1,_tmp610.f2=_tmp2A0;_tmp610;});struct _tuple14 _tmp29D=_tmp29C;int _tmp29F;int _tmp29E;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29D.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29D.f2)->tag == 2U){_LL6: _tmp29F=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29D.f1)->f1;_tmp29E=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29D.f2)->f1;_LL7:
# 1435
 if(_tmp29F != 0 && _tmp29F != 1)return t1;else{
if(_tmp29E != 0 && _tmp29E != 1)return t2;else{
if(_tmp29F >= _tmp29E)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29D.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1445
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29D.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1450
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1443
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29D.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1452
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1442
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1446
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29D.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1451
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1444
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29D.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29D.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1453
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29D.f1)->tag == 4U){_LL18: _LL19:
# 1448
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29D.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1454
 goto _LL5;}}}}}}_LL5:;}
# 1456
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1459
return Cyc_Absyn_sint_type;}
# 1464
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1467
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 ||
 Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2A2=_cycalloc(sizeof(*_tmp2A2));_tmp2A2->v=t1;_tmp2A2;});}}}
# 1476
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1480
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp2A5=({struct Cyc_String_pa_PrintArg_struct _tmp613;_tmp613.tag=0U,({
struct _fat_ptr _tmp6E2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp613.f1=_tmp6E2;});_tmp613;});struct Cyc_String_pa_PrintArg_struct _tmp2A6=({struct Cyc_String_pa_PrintArg_struct _tmp612;_tmp612.tag=0U,({struct _fat_ptr _tmp6E3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp612.f1=_tmp6E3;});_tmp612;});void*_tmp2A3[2U];_tmp2A3[0]=& _tmp2A5,_tmp2A3[1]=& _tmp2A6;({unsigned _tmp6E5=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _fat_ptr _tmp6E4=({const char*_tmp2A4="type mismatch: expecting %s but found %s";_tag_fat(_tmp2A4,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp6E5,_tmp6E4,_tag_fat(_tmp2A3,sizeof(void*),2U));});});
return 0;}}}
# 1486
return 1;}
# 1491
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2A7=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2A8=_tmp2A7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A8)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1497
return 1;}
# 1501
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 1505
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp2A9=0U;({unsigned _tmp6E7=e->loc;struct _fat_ptr _tmp6E6=({const char*_tmp2AA="integral size mismatch; conversion supplied";_tag_fat(_tmp2AA,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6E7,_tmp6E6,_tag_fat(_tmp2A9,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1511
return 0;}
# 1515
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 1519
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp2AB=0U;({unsigned _tmp6E9=e->loc;struct _fat_ptr _tmp6E8=({const char*_tmp2AC="integral size mismatch; conversion supplied";_tag_fat(_tmp2AC,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6E9,_tmp6E8,_tag_fat(_tmp2AB,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1525
return 0;}
# 1530
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1536
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp6EA=t;Cyc_Unify_unify(_tmp6EA,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1541
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2AD=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp2AE=_tmp2AD;struct Cyc_Absyn_Exp*_tmp2B0;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2AE)->tag == 9U){_LL1: _tmp2B0=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2AE)->f1;_LL2:
 return _tmp2B0;}else{_LL3: _LL4: {
# 1546
struct Cyc_Absyn_Exp*_tmp2AF=Cyc_Absyn_valueof_exp(_tmp2AD,0U);
_tmp2AF->topt=Cyc_Absyn_uint_type;
return _tmp2AF;}}_LL0:;}
# 1555
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B1=Cyc_Tcutil_compress(b);void*_tmp2B2=_tmp2B1;struct Cyc_List_List*_tmp2B6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B2)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2B6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B2)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp2B6);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp2B5=({struct Cyc_String_pa_PrintArg_struct _tmp614;_tmp614.tag=0U,({struct _fat_ptr _tmp6EB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp614.f1=_tmp6EB;});_tmp614;});void*_tmp2B3[1U];_tmp2B3[0]=& _tmp2B5;({struct _fat_ptr _tmp6EC=({const char*_tmp2B4="get_bounds_exp: %s";_tag_fat(_tmp2B4,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6EC,_tag_fat(_tmp2B3,sizeof(void*),1U));});});}_LL0:;};}
# 1564
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2B7=Cyc_Tcutil_compress(t);void*_tmp2B8=_tmp2B7;void*_tmp2BC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B8)->tag == 3U){_LL1: _tmp2BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B8)->f1).ptr_atts).bounds;_LL2:
# 1567
 return Cyc_Tcutil_get_bounds_exp(def,_tmp2BC);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2BB=({struct Cyc_String_pa_PrintArg_struct _tmp615;_tmp615.tag=0U,({struct _fat_ptr _tmp6ED=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp615.f1=_tmp6ED;});_tmp615;});void*_tmp2B9[1U];_tmp2B9[0]=& _tmp2BB;({struct _fat_ptr _tmp6EE=({const char*_tmp2BA="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp2BA,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6EE,_tag_fat(_tmp2B9,sizeof(void*),1U));});});}_LL0:;}
# 1573
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2BD=_cycalloc(sizeof(*_tmp2BD));_tmp2BD->v=tvs;_tmp2BD;}));}
# 1577
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2BE=_cycalloc(sizeof(*_tmp2BE));_tmp2BE->v=tvs;_tmp2BE;}));}
# 1582
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple21{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1590
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple14 _tmp2BF=({struct _tuple14 _tmp619;_tmp619.f1=t1,_tmp619.f2=t2;_tmp619;});struct _tuple14 _tmp2C0=_tmp2BF;void*_tmp2D5;struct Cyc_Absyn_Tqual _tmp2D4;struct Cyc_Absyn_Exp*_tmp2D3;void*_tmp2D2;void*_tmp2D1;struct Cyc_Absyn_Tqual _tmp2D0;struct Cyc_Absyn_Exp*_tmp2CF;void*_tmp2CE;struct Cyc_Absyn_PtrInfo _tmp2CD;struct Cyc_Absyn_PtrInfo _tmp2CC;switch(*((int*)_tmp2C0.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0.f2)->tag == 3U){_LL1: _tmp2CD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0.f1)->f1;_tmp2CC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0.f2)->f1;_LL2: {
# 1595
int okay=1;
# 1597
if(!Cyc_Unify_unify((_tmp2CD.ptr_atts).nullable,(_tmp2CC.ptr_atts).nullable))
# 1599
okay=!Cyc_Tcutil_force_type2bool(0,(_tmp2CD.ptr_atts).nullable);
# 1601
if(!Cyc_Unify_unify((_tmp2CD.ptr_atts).bounds,(_tmp2CC.ptr_atts).bounds)){
struct _tuple21 _tmp2C1=({struct _tuple21 _tmp616;({struct Cyc_Absyn_Exp*_tmp6F2=({void*_tmp6F1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6F1,(_tmp2CD.ptr_atts).bounds);});_tmp616.f1=_tmp6F2;}),({
struct Cyc_Absyn_Exp*_tmp6F0=({void*_tmp6EF=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6EF,(_tmp2CC.ptr_atts).bounds);});_tmp616.f2=_tmp6F0;});_tmp616;});
# 1602
struct _tuple21 _tmp2C2=_tmp2C1;struct Cyc_Absyn_Exp*_tmp2C6;struct Cyc_Absyn_Exp*_tmp2C5;if(_tmp2C2.f2 == 0){_LLA: _LLB:
# 1605
 okay=1;goto _LL9;}else{if(_tmp2C2.f1 == 0){_LLC: _LLD:
# 1608
 if(Cyc_Tcutil_force_type2bool(0,(_tmp2CD.ptr_atts).zero_term)&&({
void*_tmp6F3=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6F3,(_tmp2CC.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp2C6=_tmp2C2.f1;_tmp2C5=_tmp2C2.f2;_LLF:
# 1614
 okay=okay &&({struct Cyc_Absyn_Exp*_tmp6F4=(struct Cyc_Absyn_Exp*)_check_null(_tmp2C5);Cyc_Evexp_lte_const_exp(_tmp6F4,(struct Cyc_Absyn_Exp*)_check_null(_tmp2C6));});
# 1618
if(!Cyc_Tcutil_force_type2bool(0,(_tmp2CC.ptr_atts).zero_term))
({void*_tmp2C3=0U;({unsigned _tmp6F6=loc;struct _fat_ptr _tmp6F5=({const char*_tmp2C4="implicit cast to shorter array";_tag_fat(_tmp2C4,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp6F6,_tmp6F5,_tag_fat(_tmp2C3,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 1625
okay=okay &&(!(_tmp2CD.elt_tq).real_const ||(_tmp2CC.elt_tq).real_const);
# 1628
if(!Cyc_Unify_unify((_tmp2CD.ptr_atts).rgn,(_tmp2CC.ptr_atts).rgn)){
if(Cyc_RgnOrder_rgn_outlives_rgn(po,(_tmp2CD.ptr_atts).rgn,(_tmp2CC.ptr_atts).rgn)){
# 1631
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp2C9=({struct Cyc_String_pa_PrintArg_struct _tmp618;_tmp618.tag=0U,({
struct _fat_ptr _tmp6F7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((_tmp2CD.ptr_atts).rgn));_tmp618.f1=_tmp6F7;});_tmp618;});struct Cyc_String_pa_PrintArg_struct _tmp2CA=({struct Cyc_String_pa_PrintArg_struct _tmp617;_tmp617.tag=0U,({
struct _fat_ptr _tmp6F8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((_tmp2CC.ptr_atts).rgn));_tmp617.f1=_tmp6F8;});_tmp617;});void*_tmp2C7[2U];_tmp2C7[0]=& _tmp2C9,_tmp2C7[1]=& _tmp2CA;({unsigned _tmp6FA=loc;struct _fat_ptr _tmp6F9=({const char*_tmp2C8="implicit cast from region %s to region %s";_tag_fat(_tmp2C8,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6FA,_tmp6F9,_tag_fat(_tmp2C7,sizeof(void*),2U));});});}else{
okay=0;}}
# 1639
okay=okay &&(Cyc_Unify_unify((_tmp2CD.ptr_atts).zero_term,(_tmp2CC.ptr_atts).zero_term)||
# 1641
 Cyc_Tcutil_force_type2bool(1,(_tmp2CD.ptr_atts).zero_term)&&(_tmp2CC.elt_tq).real_const);{
# 1649
int _tmp2CB=
({void*_tmp6FB=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6FB,(_tmp2CC.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(_tmp2CC.ptr_atts).zero_term);
# 1655
okay=okay &&(Cyc_Unify_unify(_tmp2CD.elt_type,_tmp2CC.elt_type)||
(_tmp2CB &&((_tmp2CC.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp2CC.elt_type))))&& Cyc_Tcutil_ptrsubtype(po,0,_tmp2CD.elt_type,_tmp2CC.elt_type));
# 1658
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->tag == 4U){_LL3: _tmp2D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f1)->f1).elt_type;_tmp2D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f1)->f1).tq;_tmp2D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f1)->f1).num_elts;_tmp2D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f1)->f1).zero_term;_tmp2D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->f1).elt_type;_tmp2D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->f1).tq;_tmp2CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->f1).num_elts;_tmp2CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->f1).zero_term;_LL4: {
# 1662
int okay;
# 1665
okay=Cyc_Unify_unify(_tmp2D2,_tmp2CE)&&(
(_tmp2D3 != 0 && _tmp2CF != 0)&& Cyc_Evexp_same_const_exp(_tmp2D3,_tmp2CF));
# 1668
return(okay && Cyc_Unify_unify(_tmp2D5,_tmp2D1))&&(!_tmp2D4.real_const || _tmp2D0.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1670
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1672
 return Cyc_Unify_unify(t1,t2);}_LL0:;};}
# 1676
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2D6=Cyc_Tcutil_compress(t);void*_tmp2D7=_tmp2D6;void*_tmp2DA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->tag == 3U){_LL1: _tmp2DA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).elt_type;_LL2:
 return _tmp2DA;}else{_LL3: _LL4:
({void*_tmp2D8=0U;({struct _fat_ptr _tmp6FC=({const char*_tmp2D9="pointer_elt_type";_tag_fat(_tmp2D9,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6FC,_tag_fat(_tmp2D8,sizeof(void*),0U));});});}_LL0:;}
# 1682
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2DB=Cyc_Tcutil_compress(t);void*_tmp2DC=_tmp2DB;struct Cyc_Absyn_PtrAtts*_tmp2DF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->tag == 3U){_LL1: _tmp2DF=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).ptr_atts;_LL2:
 return _tmp2DF->rgn;}else{_LL3: _LL4:
({void*_tmp2DD=0U;({struct _fat_ptr _tmp6FD=({const char*_tmp2DE="pointer_elt_type";_tag_fat(_tmp2DE,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6FD,_tag_fat(_tmp2DD,sizeof(void*),0U));});});}_LL0:;}
# 1689
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2E0=Cyc_Tcutil_compress(t);void*_tmp2E1=_tmp2E0;void*_tmp2E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->tag == 3U){_LL1: _tmp2E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts).rgn;_LL2:
# 1692
*rgn=_tmp2E2;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1701
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2E3=Cyc_Tcutil_compress(t);void*_tmp2E4=_tmp2E3;void*_tmp2E6;void*_tmp2E5;switch(*((int*)_tmp2E4)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E4)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E4)->f1).ptr_atts).nullable;_tmp2E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E4)->f1).ptr_atts).bounds;_LL6:
# 1709
 return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,_tmp2E5)&& Cyc_Tcutil_force_type2bool(0,_tmp2E6);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1715
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2E7=e->r;void*_tmp2E8=_tmp2E7;void*_tmp2EC;struct Cyc_Absyn_Exp*_tmp2EB;struct _fat_ptr _tmp2EA;switch(*((int*)_tmp2E8)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2EA=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Wchar_c).val;_LLC: {
# 1724
unsigned long _tmp2E9=Cyc_strlen((struct _fat_ptr)_tmp2EA);
int i=0;
if(_tmp2E9 >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(_tmp2EA,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(_tmp2EA,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)_tmp2EA.curr)[1]== (int)'x' && _tmp2E9 >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(_tmp2EA,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp2E9;++ i){
if((int)*((const char*)_check_fat_subscript(_tmp2EA,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 1734
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1722
 return 1;case 14U: _LLD: _tmp2EC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1;_tmp2EB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E8)->f2;_LLE:
# 1735
 return Cyc_Tcutil_is_zero(_tmp2EB)&& Cyc_Tcutil_admits_zero(_tmp2EC);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1740
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1749
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1754
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1759
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1768
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1773
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1778
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2ED=ka;enum Cyc_Absyn_KindQual _tmp2F6;enum Cyc_Absyn_AliasQual _tmp2F5;_LL1: _tmp2F6=_tmp2ED->kind;_tmp2F5=_tmp2ED->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp2EE=_tmp2F5;switch(_tmp2EE){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1782
enum Cyc_Absyn_KindQual _tmp2EF=_tmp2F6;switch(_tmp2EF){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1793
{enum Cyc_Absyn_KindQual _tmp2F0=_tmp2F6;switch(_tmp2F0){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1800
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1802
{enum Cyc_Absyn_KindQual _tmp2F1=_tmp2F6;switch(_tmp2F1){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1809
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1812
({struct Cyc_String_pa_PrintArg_struct _tmp2F4=({struct Cyc_String_pa_PrintArg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp6FE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp61A.f1=_tmp6FE;});_tmp61A;});void*_tmp2F2[1U];_tmp2F2[0]=& _tmp2F4;({struct _fat_ptr _tmp6FF=({const char*_tmp2F3="kind_to_opt: bad kind %s\n";_tag_fat(_tmp2F3,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6FF,_tag_fat(_tmp2F2,sizeof(void*),1U));});});}
# 1815
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7->tag=0U,_tmp2F7->f1=k;_tmp2F7;});}
# 1818
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));({void*_tmp700=Cyc_Tcutil_kind_to_bound(k);_tmp2F8->v=_tmp700;});_tmp2F8;});}
# 1824
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp2F9=Cyc_Tcutil_compress(t2);void*_tmp2FA=_tmp2F9;void*_tmp2FB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->tag == 3U){_LL1: _tmp2FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).nullable;_LL2:
# 1829
 if(!Cyc_Tcutil_force_type2bool(1,_tmp2FB))
return 0;
({void*_tmp701=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp701;});
e1->topt=t2;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 1838
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple22{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1844
struct _tuple22 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1848
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp704=({struct _fat_ptr*_tmp30E=_cycalloc(sizeof(*_tmp30E));({struct _fat_ptr _tmp703=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30D=({struct Cyc_Int_pa_PrintArg_struct _tmp61C;_tmp61C.tag=1U,_tmp61C.f1=(unsigned long)counter ++;_tmp61C;});void*_tmp30B[1U];_tmp30B[0]=& _tmp30D;({struct _fat_ptr _tmp702=({const char*_tmp30C="__aliasvar%d";_tag_fat(_tmp30C,sizeof(char),13U);});Cyc_aprintf(_tmp702,_tag_fat(_tmp30B,sizeof(void*),1U));});});*_tmp30E=_tmp703;});_tmp30E;});_tmp30F->f2=_tmp704;});_tmp30F;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp705=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A->tag=4U,_tmp30A->f1=vd;_tmp30A;});Cyc_Absyn_varb_exp(_tmp705,e->loc);});
# 1857
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 1859
{void*_tmp2FC=Cyc_Tcutil_compress(e_type);void*_tmp2FD=_tmp2FC;void*_tmp309;struct Cyc_Absyn_Tqual _tmp308;void*_tmp307;void*_tmp306;void*_tmp305;void*_tmp304;struct Cyc_Absyn_PtrLoc*_tmp303;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->tag == 3U){_LL1: _tmp309=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_type;_tmp308=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_tq;_tmp307=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).rgn;_tmp306=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).nullable;_tmp305=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).bounds;_tmp304=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).zero_term;_tmp303=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).ptrloc;_LL2:
# 1861
{void*_tmp2FE=Cyc_Tcutil_compress(_tmp307);void*_tmp2FF=_tmp2FE;void**_tmp302;struct Cyc_Core_Opt*_tmp301;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FF)->tag == 1U){_LL6: _tmp302=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FF)->f2;_tmp301=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2FF)->f4;_LL7: {
# 1863
void*_tmp300=Cyc_Absyn_var_type(tv);
*_tmp302=_tmp300;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1868
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1872
e->topt=0;
vd->initializer=0;{
# 1876
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1878
return({struct _tuple22 _tmp61B;_tmp61B.f1=d,_tmp61B.f2=ve;_tmp61B;});};}
# 1883
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1886
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1891
void*_tmp310=Cyc_Tcutil_compress(wants_type);void*_tmp311=_tmp310;void*_tmp313;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp311)->tag == 3U){_LL1: _tmp313=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp311)->f1).ptr_atts).rgn;_LL2:
# 1893
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp313))return 0;{
struct Cyc_Absyn_Kind*_tmp312=Cyc_Tcutil_type_kind(_tmp313);
return(int)_tmp312->kind == (int)Cyc_Absyn_RgnKind &&(int)_tmp312->aliasqual == (int)Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1899
return 0;}
# 1903
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1905
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1909
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1912
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1914
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp316=({struct Cyc_String_pa_PrintArg_struct _tmp61E;_tmp61E.tag=0U,({
struct _fat_ptr _tmp706=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp61E.f1=_tmp706;});_tmp61E;});struct Cyc_String_pa_PrintArg_struct _tmp317=({struct Cyc_String_pa_PrintArg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp707=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp61D.f1=_tmp707;});_tmp61D;});void*_tmp314[2U];_tmp314[0]=& _tmp316,_tmp314[1]=& _tmp317;({unsigned _tmp709=e->loc;struct _fat_ptr _tmp708=({const char*_tmp315="integral size mismatch; %s -> %s conversion supplied";_tag_fat(_tmp315,sizeof(char),53U);});Cyc_Tcutil_warn(_tmp709,_tmp708,_tag_fat(_tmp314,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1922
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp31A=({struct Cyc_String_pa_PrintArg_struct _tmp621;_tmp621.tag=0U,({
struct _fat_ptr _tmp70A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp621.f1=_tmp70A;});_tmp621;});struct Cyc_String_pa_PrintArg_struct _tmp31B=({struct Cyc_String_pa_PrintArg_struct _tmp620;_tmp620.tag=0U,({struct _fat_ptr _tmp70B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp620.f1=_tmp70B;});_tmp620;});struct Cyc_String_pa_PrintArg_struct _tmp31C=({struct Cyc_String_pa_PrintArg_struct _tmp61F;_tmp61F.tag=0U,({struct _fat_ptr _tmp70C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp61F.f1=_tmp70C;});_tmp61F;});void*_tmp318[3U];_tmp318[0]=& _tmp31A,_tmp318[1]=& _tmp31B,_tmp318[2]=& _tmp31C;({unsigned _tmp70E=e->loc;struct _fat_ptr _tmp70D=({const char*_tmp319="implicit alias coercion for %s:%s to %s";_tag_fat(_tmp319,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp70E,_tmp70D,_tag_fat(_tmp318,sizeof(void*),3U));});});
*alias_coercion=1;}
# 1929
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1933
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1937
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp623;_tmp623.tag=0U,({struct _fat_ptr _tmp70F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp623.f1=_tmp70F;});_tmp623;});struct Cyc_String_pa_PrintArg_struct _tmp320=({struct Cyc_String_pa_PrintArg_struct _tmp622;_tmp622.tag=0U,({struct _fat_ptr _tmp710=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp622.f1=_tmp710;});_tmp622;});void*_tmp31D[2U];_tmp31D[0]=& _tmp31F,_tmp31D[1]=& _tmp320;({unsigned _tmp712=e->loc;struct _fat_ptr _tmp711=({const char*_tmp31E="implicit cast from %s to %s";_tag_fat(_tmp31E,sizeof(char),28U);});Cyc_Tcutil_warn(_tmp712,_tmp711,_tag_fat(_tmp31D,sizeof(void*),2U));});});
return 1;}
# 1943
return 0;}
# 1949
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1952
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1966 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1969
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 1972
struct _tuple23 _tmp321=*env;struct _tuple23 _tmp322=_tmp321;struct Cyc_List_List*_tmp329;struct _RegionHandle*_tmp328;int _tmp327;_LL1: _tmp329=_tmp322.f1;_tmp328=_tmp322.f2;_tmp327=_tmp322.f3;_LL2:;{
void*_tmp323=_tmp329 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp328,_tmp329,x->type);
struct Cyc_List_List*_tmp324=Cyc_Tcutil_flatten_type(_tmp328,_tmp327,_tmp323);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp324)== 1)
return({struct Cyc_List_List*_tmp326=_region_malloc(_tmp328,sizeof(*_tmp326));({struct _tuple11*_tmp713=({struct _tuple11*_tmp325=_region_malloc(_tmp328,sizeof(*_tmp325));_tmp325->f1=x->tq,_tmp325->f2=_tmp323;_tmp325;});_tmp326->hd=_tmp713;}),_tmp326->tl=0;_tmp326;});
return _tmp324;};}struct _tuple24{struct _RegionHandle*f1;int f2;};
# 1979
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple11*x){
# 1981
struct _tuple24 _tmp32A=*env;struct _tuple24 _tmp32B=_tmp32A;struct _RegionHandle*_tmp334;int _tmp333;_LL1: _tmp334=_tmp32B.f1;_tmp333=_tmp32B.f2;_LL2:;{
struct _tuple11 _tmp32C=*x;struct _tuple11 _tmp32D=_tmp32C;struct Cyc_Absyn_Tqual _tmp332;void*_tmp331;_LL4: _tmp332=_tmp32D.f1;_tmp331=_tmp32D.f2;_LL5:;{
struct Cyc_List_List*_tmp32E=Cyc_Tcutil_flatten_type(_tmp334,_tmp333,_tmp331);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32E)== 1)
return({struct Cyc_List_List*_tmp330=_region_malloc(_tmp334,sizeof(*_tmp330));({struct _tuple11*_tmp714=({struct _tuple11*_tmp32F=_region_malloc(_tmp334,sizeof(*_tmp32F));_tmp32F->f1=_tmp332,_tmp32F->f2=_tmp331;_tmp32F;});_tmp330->hd=_tmp714;}),_tmp330->tl=0;_tmp330;});
return _tmp32E;};};}
# 1988
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1991
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp335=t1;struct Cyc_List_List*_tmp354;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_Aggrdecl*_tmp352;struct Cyc_List_List*_tmp351;switch(*((int*)_tmp335)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp335)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp335)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp352=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp335)->f1)->f1).KnownAggr).val;_tmp351=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp335)->f2;_LL6:
# 2009
 if((((int)_tmp352->kind == (int)Cyc_Absyn_UnionA || _tmp352->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp352->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp352->impl))->rgn_po != 0)
# 2011
return({struct Cyc_List_List*_tmp340=_region_malloc(r,sizeof(*_tmp340));({struct _tuple11*_tmp716=({struct _tuple11*_tmp33F=_region_malloc(r,sizeof(*_tmp33F));({struct Cyc_Absyn_Tqual _tmp715=Cyc_Absyn_empty_tqual(0U);_tmp33F->f1=_tmp715;}),_tmp33F->f2=t1;_tmp33F;});_tmp340->hd=_tmp716;}),_tmp340->tl=0;_tmp340;});{
struct Cyc_List_List*_tmp341=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp352->tvs,_tmp351);
struct _tuple23 env=({struct _tuple23 _tmp624;_tmp624.f1=_tmp341,_tmp624.f2=r,_tmp624.f3=flatten;_tmp624;});
struct Cyc_List_List*_tmp342=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp352->impl))->fields;struct Cyc_List_List*_tmp343=_tmp342;struct Cyc_Absyn_Aggrfield*_tmp349;struct Cyc_List_List*_tmp348;if(_tmp343 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp349=(struct Cyc_Absyn_Aggrfield*)_tmp343->hd;_tmp348=_tmp343->tl;_LL14: {
# 2017
struct Cyc_List_List*_tmp344=Cyc_Tcutil_flatten_type_f(& env,_tmp349);
env.f3=0;{
struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp348);
struct Cyc_List_List*_tmp346=({struct Cyc_List_List*_tmp347=_region_malloc(r,sizeof(*_tmp347));_tmp347->hd=_tmp344,_tmp347->tl=_tmp345;_tmp347;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp346);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp353=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp335)->f1;_LL4: {
# 1996
struct _tuple24 _tmp336=({struct _tuple24 _tmp625;_tmp625.f1=r,_tmp625.f2=flatten;_tmp625;});
# 1998
struct Cyc_List_List*_tmp337=_tmp353;struct _tuple11*_tmp33E;struct Cyc_List_List*_tmp33D;if(_tmp337 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp33E=(struct _tuple11*)_tmp337->hd;_tmp33D=_tmp337->tl;_LLF: {
# 2001
struct Cyc_List_List*_tmp338=Cyc_Tcutil_rcopy_tqt(& _tmp336,_tmp33E);
_tmp336.f2=0;{
struct Cyc_List_List*_tmp339=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple11*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp336,_tmp353);
struct Cyc_List_List*_tmp33A=({struct Cyc_List_List*_tmp33C=_region_malloc(r,sizeof(*_tmp33C));_tmp33C->hd=_tmp338,_tmp33C->tl=_tmp339;_tmp33C;});
return({struct _RegionHandle*_tmp717=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp717,({struct Cyc_List_List*_tmp33B=_region_malloc(r,sizeof(*_tmp33B));_tmp33B->hd=_tmp338,_tmp33B->tl=_tmp339;_tmp33B;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp335)->f1 == Cyc_Absyn_StructA){_LL7: _tmp354=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp335)->f2;_LL8: {
# 2024
struct _tuple23 env=({struct _tuple23 _tmp626;_tmp626.f1=0,_tmp626.f2=r,_tmp626.f3=flatten;_tmp626;});
struct Cyc_List_List*_tmp34A=_tmp354;struct Cyc_Absyn_Aggrfield*_tmp350;struct Cyc_List_List*_tmp34F;if(_tmp34A == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp350=(struct Cyc_Absyn_Aggrfield*)_tmp34A->hd;_tmp34F=_tmp34A->tl;_LL19: {
# 2028
struct Cyc_List_List*_tmp34B=Cyc_Tcutil_flatten_type_f(& env,_tmp350);
env.f3=0;{
struct Cyc_List_List*_tmp34C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp34F);
struct Cyc_List_List*_tmp34D=({struct Cyc_List_List*_tmp34E=_region_malloc(r,sizeof(*_tmp34E));_tmp34E->hd=_tmp34B,_tmp34E->tl=_tmp34C;_tmp34E;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp34D);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2034
 goto _LL0;}_LL0:;};}
# 2037
return({struct Cyc_List_List*_tmp356=_region_malloc(r,sizeof(*_tmp356));({struct _tuple11*_tmp719=({struct _tuple11*_tmp355=_region_malloc(r,sizeof(*_tmp355));({struct Cyc_Absyn_Tqual _tmp718=Cyc_Absyn_empty_tqual(0U);_tmp355->f1=_tmp718;}),_tmp355->f2=t1;_tmp355;});_tmp356->hd=_tmp719;}),_tmp356->tl=0;_tmp356;});}
# 2041
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp357=(void*)t->hd;void*_tmp358=_tmp357;switch(*((int*)_tmp358)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
# 2048
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2051
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2054
return 1;}
# 2057
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple14 _tmp359=({struct _tuple14 _tmp627;({void*_tmp71B=Cyc_Tcutil_compress(t1);_tmp627.f1=_tmp71B;}),({void*_tmp71A=Cyc_Tcutil_compress(t2);_tmp627.f2=_tmp71A;});_tmp627;});struct _tuple14 _tmp35A=_tmp359;enum Cyc_Absyn_Size_of _tmp35C;enum Cyc_Absyn_Size_of _tmp35B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->f1)->tag == 1U){_LL1: _tmp35C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->f1)->f2;_tmp35B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->f1)->f2;_LL2:
# 2060
 return((int)_tmp35C == (int)_tmp35B ||(int)_tmp35C == (int)2U &&(int)_tmp35B == (int)3U)||
(int)_tmp35C == (int)3U &&(int)_tmp35B == (int)Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2068
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2070
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple14*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple14*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple14 _tmp35D=({struct _tuple14 _tmp628;_tmp628.f1=t1,_tmp628.f2=t2;_tmp628;});struct _tuple14 _tmp35E=_tmp35D;struct Cyc_Absyn_FnInfo _tmp388;struct Cyc_Absyn_FnInfo _tmp387;struct Cyc_Absyn_Datatypedecl*_tmp386;struct Cyc_Absyn_Datatypefield*_tmp385;struct Cyc_List_List*_tmp384;struct Cyc_Absyn_Datatypedecl*_tmp383;struct Cyc_List_List*_tmp382;void*_tmp381;struct Cyc_Absyn_Tqual _tmp380;void*_tmp37F;void*_tmp37E;void*_tmp37D;void*_tmp37C;void*_tmp37B;struct Cyc_Absyn_Tqual _tmp37A;void*_tmp379;void*_tmp378;void*_tmp377;void*_tmp376;switch(*((int*)_tmp35E.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->tag == 3U){_LL1: _tmp381=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).elt_type;_tmp380=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).elt_tq;_tmp37F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).ptr_atts).rgn;_tmp37E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).ptr_atts).nullable;_tmp37D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).ptr_atts).bounds;_tmp37C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).ptr_atts).zero_term;_tmp37B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).elt_type;_tmp37A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).elt_tq;_tmp379=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).ptr_atts).rgn;_tmp378=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).ptr_atts).nullable;_tmp377=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).ptr_atts).bounds;_tmp376=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).ptr_atts).zero_term;_LL2:
# 2082
 if(_tmp380.real_const && !_tmp37A.real_const)
return 0;
# 2085
if((!Cyc_Unify_unify(_tmp37E,_tmp378)&&
 Cyc_Absyn_type2bool(0,_tmp37E))&& !Cyc_Absyn_type2bool(0,_tmp378))
return 0;
# 2089
if((Cyc_Unify_unify(_tmp37C,_tmp376)&& !
Cyc_Absyn_type2bool(0,_tmp37C))&& Cyc_Absyn_type2bool(0,_tmp376))
return 0;
# 2093
if((!Cyc_Unify_unify(_tmp37F,_tmp379)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,_tmp37F,_tmp379))&& !
Cyc_Tcutil_subtype(po,assume,_tmp37F,_tmp379))
return 0;
# 2097
if(!Cyc_Unify_unify(_tmp37D,_tmp377)){
struct Cyc_Absyn_Exp*_tmp35F=({void*_tmp71C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp71C,_tmp37D);});
struct Cyc_Absyn_Exp*_tmp360=({void*_tmp71D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp71D,_tmp377);});
if(_tmp35F != _tmp360){
if((_tmp35F == 0 || _tmp360 == 0)|| !Cyc_Evexp_lte_const_exp(_tmp360,_tmp360))
return 0;}}
# 2107
if(!_tmp37A.real_const && _tmp380.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp37B)))
return 0;}{
# 2113
int _tmp361=
({void*_tmp71E=_tmp377;Cyc_Unify_unify(_tmp71E,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,_tmp376);
# 2118
return(_tmp361 &&({struct Cyc_RgnOrder_RgnPO*_tmp722=po;struct Cyc_List_List*_tmp721=({struct Cyc_List_List*_tmp363=_cycalloc(sizeof(*_tmp363));({struct _tuple14*_tmp71F=({struct _tuple14*_tmp362=_cycalloc(sizeof(*_tmp362));_tmp362->f1=t1,_tmp362->f2=t2;_tmp362;});_tmp363->hd=_tmp71F;}),_tmp363->tl=assume;_tmp363;});void*_tmp720=_tmp381;Cyc_Tcutil_ptrsubtype(_tmp722,_tmp721,_tmp720,_tmp37B);})|| Cyc_Unify_unify(_tmp381,_tmp37B))|| Cyc_Tcutil_isomorphic(_tmp381,_tmp37B);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp386=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp385=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp384=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f2;_tmp383=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->f1).KnownDatatype).val;_tmp382=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f2;_LL4:
# 2125
 if(_tmp386 != _tmp383 && Cyc_Absyn_qvar_cmp(_tmp386->name,_tmp383->name)!= 0)return 0;
# 2127
if(({int _tmp723=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp384);_tmp723 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp382);}))return 0;
for(0;_tmp384 != 0;(_tmp384=_tmp384->tl,_tmp382=_tmp382->tl)){
if(!Cyc_Unify_unify((void*)_tmp384->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp382))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35E.f2)->tag == 5U){_LL5: _tmp388=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35E.f1)->f1;_tmp387=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35E.f2)->f1;_LL6:
# 2134
 if(_tmp388.tvars != 0 || _tmp387.tvars != 0){
struct Cyc_List_List*_tmp364=_tmp388.tvars;
struct Cyc_List_List*_tmp365=_tmp387.tvars;
if(({int _tmp724=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp364);_tmp724 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp365);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp364 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp364->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp365))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp367=_cycalloc(sizeof(*_tmp367));({struct _tuple18*_tmp726=({struct _tuple18*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->f1=(struct Cyc_Absyn_Tvar*)_tmp365->hd,({void*_tmp725=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp364->hd);_tmp366->f2=_tmp725;});_tmp366;});_tmp367->hd=_tmp726;}),_tmp367->tl=inst;_tmp367;});
_tmp364=_tmp364->tl;
_tmp365=_tmp365->tl;}
# 2145
if(inst != 0){
_tmp388.tvars=0;
_tmp387.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp729=po;struct Cyc_List_List*_tmp728=assume;void*_tmp727=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->tag=5U,_tmp368->f1=_tmp388;_tmp368;});Cyc_Tcutil_subtype(_tmp729,_tmp728,_tmp727,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp369=_cycalloc(sizeof(*_tmp369));_tmp369->tag=5U,_tmp369->f1=_tmp387;_tmp369;}));});}};}
# 2152
if(!Cyc_Tcutil_subtype(po,assume,_tmp388.ret_type,_tmp387.ret_type))return 0;{
struct Cyc_List_List*_tmp36A=_tmp388.args;
struct Cyc_List_List*_tmp36B=_tmp387.args;
# 2157
if(({int _tmp72A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp36A);_tmp72A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp36B);}))return 0;
# 2159
for(0;_tmp36A != 0;(_tmp36A=_tmp36A->tl,_tmp36B=_tmp36B->tl)){
struct _tuple9 _tmp36C=*((struct _tuple9*)_tmp36A->hd);struct _tuple9 _tmp36D=_tmp36C;struct Cyc_Absyn_Tqual _tmp373;void*_tmp372;_LLA: _tmp373=_tmp36D.f2;_tmp372=_tmp36D.f3;_LLB:;{
struct _tuple9 _tmp36E=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp36B))->hd);struct _tuple9 _tmp36F=_tmp36E;struct Cyc_Absyn_Tqual _tmp371;void*_tmp370;_LLD: _tmp371=_tmp36F.f2;_tmp370=_tmp36F.f3;_LLE:;
# 2163
if(_tmp371.real_const && !_tmp373.real_const || !Cyc_Tcutil_subtype(po,assume,_tmp370,_tmp372))
return 0;};}
# 2167
if(_tmp388.c_varargs != _tmp387.c_varargs)return 0;
if(_tmp388.cyc_varargs != 0 && _tmp387.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp374=*_tmp388.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp375=*_tmp387.cyc_varargs;
# 2172
if((_tmp375.tq).real_const && !(_tmp374.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,_tmp375.type,_tmp374.type))
return 0;}else{
if(_tmp388.cyc_varargs != 0 || _tmp387.cyc_varargs != 0)return 0;}
# 2177
if(!({void*_tmp72B=(void*)_check_null(_tmp388.effect);Cyc_Tcutil_subset_effect(1,_tmp72B,(void*)_check_null(_tmp387.effect));}))return 0;
# 2179
if(!Cyc_Tcutil_sub_rgnpo(_tmp388.rgn_po,_tmp387.rgn_po))return 0;
# 2181
if(!Cyc_Tcutil_sub_attributes(_tmp388.attributes,_tmp387.attributes))return 0;
# 2183
if(!Cyc_Relations_check_logical_implication(_tmp387.requires_relns,_tmp388.requires_relns))
# 2185
return 0;
# 2187
if(!Cyc_Relations_check_logical_implication(_tmp388.ensures_relns,_tmp387.ensures_relns))
# 2189
return 0;
# 2191
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2202
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2206
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple11*_tmp389=(struct _tuple11*)tqs1->hd;struct _tuple11*_tmp38A=_tmp389;struct Cyc_Absyn_Tqual _tmp390;void*_tmp38F;_LL1: _tmp390=_tmp38A->f1;_tmp38F=_tmp38A->f2;_LL2:;{
struct _tuple11*_tmp38B=(struct _tuple11*)tqs2->hd;struct _tuple11*_tmp38C=_tmp38B;struct Cyc_Absyn_Tqual _tmp38E;void*_tmp38D;_LL4: _tmp38E=_tmp38C->f1;_tmp38D=_tmp38C->f2;_LL5:;
if(_tmp390.real_const && !_tmp38E.real_const)return 0;
if((_tmp38E.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp38D)))&&
 Cyc_Tcutil_subtype(po,assume,_tmp38F,_tmp38D))
continue;
if(Cyc_Unify_unify(_tmp38F,_tmp38D))
continue;
if(Cyc_Tcutil_isomorphic(_tmp38F,_tmp38D))
continue;
return 0;};};}
# 2222
return 1;}
# 2227
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2233
{void*_tmp391=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp391)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp391)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp391)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2236
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2238
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2242
void*_tmp392=t1;void*_tmp3B7;struct Cyc_Absyn_Enumdecl*_tmp3B6;void*_tmp3B5;struct Cyc_Absyn_Tqual _tmp3B4;struct Cyc_Absyn_Exp*_tmp3B3;void*_tmp3B2;void*_tmp3B1;struct Cyc_Absyn_Tqual _tmp3B0;void*_tmp3AF;void*_tmp3AE;void*_tmp3AD;void*_tmp3AC;switch(*((int*)_tmp392)){case 3U: _LLA: _tmp3B1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).elt_type;_tmp3B0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).elt_tq;_tmp3AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).rgn;_tmp3AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).nullable;_tmp3AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).bounds;_tmp3AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).zero_term;_LLB:
# 2251
{void*_tmp393=t2;void*_tmp3A2;struct Cyc_Absyn_Tqual _tmp3A1;void*_tmp3A0;void*_tmp39F;void*_tmp39E;void*_tmp39D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp393)->tag == 3U){_LL19: _tmp3A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp393)->f1).elt_type;_tmp3A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp393)->f1).elt_tq;_tmp3A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp393)->f1).ptr_atts).rgn;_tmp39F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp393)->f1).ptr_atts).nullable;_tmp39E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp393)->f1).ptr_atts).bounds;_tmp39D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp393)->f1).ptr_atts).zero_term;_LL1A: {
# 2253
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp394=({struct Cyc_List_List*_tmp39C=_cycalloc(sizeof(*_tmp39C));({struct _tuple14*_tmp72C=({struct _tuple14*_tmp39B=_cycalloc(sizeof(*_tmp39B));_tmp39B->f1=t1,_tmp39B->f2=t2;_tmp39B;});_tmp39C->hd=_tmp72C;}),_tmp39C->tl=0;_tmp39C;});
int _tmp395=_tmp3A1.real_const || !_tmp3B0.real_const;
# 2266 "tcutil.cyc"
int _tmp396=
({void*_tmp72D=_tmp39E;Cyc_Unify_unify(_tmp72D,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,_tmp39D);
# 2269
int _tmp397=_tmp395 &&(
((_tmp396 && Cyc_Tcutil_ptrsubtype(po,_tmp394,_tmp3B1,_tmp3A2)||
 Cyc_Unify_unify(_tmp3B1,_tmp3A2))|| Cyc_Tcutil_isomorphic(_tmp3B1,_tmp3A2))|| Cyc_Unify_unify(_tmp3A2,Cyc_Absyn_void_type));
int zeroterm_ok=Cyc_Unify_unify(_tmp3AC,_tmp39D)|| !Cyc_Absyn_type2bool(0,_tmp39D);
# 2274
int _tmp398=_tmp397?0:((Cyc_Tcutil_is_bits_only_type(_tmp3B1)&& Cyc_Tcutil_is_char_type(_tmp3A2))&& !
Cyc_Tcutil_force_type2bool(0,_tmp39D))&&(
_tmp3A1.real_const || !_tmp3B0.real_const);
int bounds_ok=Cyc_Unify_unify(_tmp3AD,_tmp39E);
if(!bounds_ok && !_tmp398){
struct Cyc_Absyn_Exp*_tmp399=({void*_tmp72E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp72E,_tmp3AD);});
struct Cyc_Absyn_Exp*_tmp39A=({void*_tmp72F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp72F,_tmp39E);});
if((_tmp399 != 0 && _tmp39A != 0)&& Cyc_Evexp_lte_const_exp(_tmp39A,_tmp399))
bounds_ok=1;else{
if(_tmp399 == 0 || _tmp39A == 0)
bounds_ok=1;}}{
# 2286
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp3AE);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp39F);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2291
if(((bounds_ok && zeroterm_ok)&&(_tmp397 || _tmp398))&&(
Cyc_Unify_unify(_tmp3AF,_tmp3A0)|| Cyc_RgnOrder_rgn_outlives_rgn(po,_tmp3AF,_tmp3A0)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2297
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp3B5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp392)->f1).elt_type;_tmp3B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp392)->f1).tq;_tmp3B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp392)->f1).num_elts;_tmp3B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp392)->f1).zero_term;_LLD:
# 2299
{void*_tmp3A3=t2;void*_tmp3A7;struct Cyc_Absyn_Tqual _tmp3A6;struct Cyc_Absyn_Exp*_tmp3A5;void*_tmp3A4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->tag == 4U){_LL1E: _tmp3A7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).elt_type;_tmp3A6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).tq;_tmp3A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).num_elts;_tmp3A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A3)->f1).zero_term;_LL1F: {
# 2301
int okay=
(((_tmp3B3 != 0 && _tmp3A5 != 0)&& Cyc_Unify_unify(_tmp3B2,_tmp3A4))&&
 Cyc_Evexp_lte_const_exp(_tmp3A5,_tmp3B3))&& Cyc_Evexp_lte_const_exp(_tmp3B3,_tmp3A5);
return
# 2306
(okay && Cyc_Unify_unify(_tmp3B5,_tmp3A7))&&(!_tmp3B4.real_const || _tmp3A6.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2308
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2310
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f1)){case 15U: _LLE: _tmp3B6=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f1)->f2;_LLF:
# 2314
{void*_tmp3A8=t2;struct Cyc_Absyn_Enumdecl*_tmp3A9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->f1)->tag == 15U){_LL23: _tmp3A9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A8)->f1)->f2;_LL24:
# 2316
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3B6))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3A9))->fields != 0)&&({
int _tmp730=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3B6->fields))->v);_tmp730 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3A9->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2322
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2325
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f2 != 0){_LL14: _tmp3B7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f2)->hd;_LL15:
# 2328
{void*_tmp3AA=t2;void*_tmp3AB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f2 != 0){_LL28: _tmp3AB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f2)->hd;_LL29:
# 2330
 if(Cyc_RgnOrder_rgn_outlives_rgn(po,_tmp3B7,_tmp3AB))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2334
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 2340
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3B8=Cyc_Absyn_copy_exp(e);
({void*_tmp731=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->tag=14U,_tmp3B9->f1=t,_tmp3B9->f2=_tmp3B8,_tmp3B9->f3=0,_tmp3B9->f4=c;_tmp3B9;});e->r=_tmp731;});
e->topt=t;};}
# 2349
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2354
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3BF=({struct Cyc_Int_pa_PrintArg_struct _tmp629;_tmp629.tag=1U,_tmp629.f1=(unsigned long)i;_tmp629;});void*_tmp3BD[1U];_tmp3BD[0]=& _tmp3BF;({struct _fat_ptr _tmp732=({const char*_tmp3BE="#%d";_tag_fat(_tmp3BE,sizeof(char),4U);});Cyc_aprintf(_tmp732,_tag_fat(_tmp3BD,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({struct _fat_ptr*_tmp733=({unsigned _tmp3BB=1;struct _fat_ptr*_tmp3BA=_cycalloc(_check_times(_tmp3BB,sizeof(struct _fat_ptr)));_tmp3BA[0]=s;_tmp3BA;});_tmp3BC->name=_tmp733;}),_tmp3BC->identity=- 1,_tmp3BC->kind=k;_tmp3BC;});}
# 2361
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp3C0=*t->name;
return(int)*((const char*)_check_fat_subscript(_tmp3C0,sizeof(char),0))== (int)'#';}
# 2366
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp3C3=({struct Cyc_String_pa_PrintArg_struct _tmp62A;_tmp62A.tag=0U,_tmp62A.f1=(struct _fat_ptr)((struct _fat_ptr)*t->name);_tmp62A;});void*_tmp3C1[1U];_tmp3C1[0]=& _tmp3C3;({struct _fat_ptr _tmp734=({const char*_tmp3C2="%s";_tag_fat(_tmp3C2,sizeof(char),3U);});Cyc_printf(_tmp734,_tag_fat(_tmp3C1,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp3C4=({struct _fat_ptr _tmp735=({const char*_tmp3CA="`";_tag_fat(_tmp3CA,sizeof(char),2U);});Cyc_strconcat(_tmp735,(struct _fat_ptr)*t->name);});
({struct _fat_ptr _tmp3C5=_fat_ptr_plus(_tmp3C4,sizeof(char),1);char _tmp3C6=*((char*)_check_fat_subscript(_tmp3C5,sizeof(char),0U));char _tmp3C7='t';if(_get_fat_size(_tmp3C5,sizeof(char))== 1U &&(_tmp3C6 == 0 && _tmp3C7 != 0))_throw_arraybounds();*((char*)_tmp3C5.curr)=_tmp3C7;});
({struct _fat_ptr*_tmp736=({unsigned _tmp3C9=1;struct _fat_ptr*_tmp3C8=_cycalloc(_check_times(_tmp3C9,sizeof(struct _fat_ptr)));_tmp3C8[0]=(struct _fat_ptr)_tmp3C4;_tmp3C8;});t->name=_tmp736;});};}
# 2375
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2382
struct Cyc_List_List*_tmp3CB=0;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp3CB=({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->hd=(void*)atts->hd,_tmp3CC->tl=_tmp3CB;_tmp3CC;});}}{
struct Cyc_Absyn_FnInfo _tmp3CD=fd->i;
_tmp3CD.attributes=_tmp3CB;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->tag=5U,_tmp3CE->f1=_tmp3CD;_tmp3CE;});};}
# 2390
return(void*)_check_null(fd->cached_type);}
# 2396
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2398
union Cyc_Relations_RelnOp _tmp3CF=*rop;union Cyc_Relations_RelnOp _tmp3D0=_tmp3CF;struct Cyc_Absyn_Vardecl*_tmp3E3;struct Cyc_Absyn_Vardecl*_tmp3E2;switch((_tmp3D0.RNumelts).tag){case 2U: _LL1: _tmp3E2=(_tmp3D0.RVar).val;_LL2: {
# 2400
struct _tuple1 _tmp3D1=*_tmp3E2->name;struct _tuple1 _tmp3D2=_tmp3D1;union Cyc_Absyn_Nmspace _tmp3D9;struct _fat_ptr*_tmp3D8;_LL8: _tmp3D9=_tmp3D2.f1;_tmp3D8=_tmp3D2.f2;_LL9:;
if(!((int)((_tmp3D9.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp737=(struct _fat_ptr)*_tmp3D8;Cyc_strcmp(_tmp737,({const char*_tmp3D3="return_value";_tag_fat(_tmp3D3,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp738=Cyc_Relations_RReturn();*rop=_tmp738;});
goto _LL0;}{
# 2406
unsigned c=0U;
{struct Cyc_List_List*_tmp3D4=args;for(0;_tmp3D4 != 0;(_tmp3D4=_tmp3D4->tl,c ++)){
struct _tuple9*_tmp3D5=(struct _tuple9*)_tmp3D4->hd;struct _tuple9*_tmp3D6=_tmp3D5;struct _fat_ptr*_tmp3D7;_LLB: _tmp3D7=_tmp3D6->f1;_LLC:;
if(_tmp3D7 != 0){
if(Cyc_strcmp((struct _fat_ptr)*_tmp3D8,(struct _fat_ptr)*_tmp3D7)== 0){
({union Cyc_Relations_RelnOp _tmp739=Cyc_Relations_RParam(c);*rop=_tmp739;});
break;}}}}
# 2416
goto _LL0;};}case 3U: _LL3: _tmp3E3=(_tmp3D0.RNumelts).val;_LL4: {
# 2418
struct _tuple1 _tmp3DA=*_tmp3E3->name;struct _tuple1 _tmp3DB=_tmp3DA;union Cyc_Absyn_Nmspace _tmp3E1;struct _fat_ptr*_tmp3E0;_LLE: _tmp3E1=_tmp3DB.f1;_tmp3E0=_tmp3DB.f2;_LLF:;
if(!((int)((_tmp3E1.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3DC=args;for(0;_tmp3DC != 0;(_tmp3DC=_tmp3DC->tl,c ++)){
struct _tuple9*_tmp3DD=(struct _tuple9*)_tmp3DC->hd;struct _tuple9*_tmp3DE=_tmp3DD;struct _fat_ptr*_tmp3DF;_LL11: _tmp3DF=_tmp3DE->f1;_LL12:;
if(_tmp3DF != 0){
if(Cyc_strcmp((struct _fat_ptr)*_tmp3E0,(struct _fat_ptr)*_tmp3DF)== 0){
({union Cyc_Relations_RelnOp _tmp73A=Cyc_Relations_RParamNumelts(c);*rop=_tmp73A;});
break;}}}}
# 2430
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2435
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2437
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2441
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2443
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3E4=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp3E4);
return _tmp3E4;};}
# 2450
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){return(*t).f2;}
static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){
struct _tuple11*_tmp3E5=pr;struct Cyc_Absyn_Tqual _tmp3E8;void*_tmp3E7;_LL1: _tmp3E8=_tmp3E5->f1;_tmp3E7=_tmp3E5->f2;_LL2:;
if(_tmp3E7 == t)return pr;else{
return({struct _tuple11*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->f1=_tmp3E8,_tmp3E6->f2=t;_tmp3E6;});}}struct _tuple25{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple26{struct _tuple25*f1;void*f2;};
# 2456
static struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2458
return({struct _tuple26*_tmp3EA=_region_malloc(rgn,sizeof(*_tmp3EA));({struct _tuple25*_tmp73B=({struct _tuple25*_tmp3E9=_region_malloc(rgn,sizeof(*_tmp3E9));_tmp3E9->f1=(*y).f1,_tmp3E9->f2=(*y).f2;_tmp3E9;});_tmp3EA->f1=_tmp73B;}),_tmp3EA->f2=(*y).f3;_tmp3EA;});}
# 2460
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2462
struct _tuple9 _tmp3EB=*orig_arg;struct _tuple9 _tmp3EC=_tmp3EB;struct _fat_ptr*_tmp3F0;struct Cyc_Absyn_Tqual _tmp3EF;void*_tmp3EE;_LL1: _tmp3F0=_tmp3EC.f1;_tmp3EF=_tmp3EC.f2;_tmp3EE=_tmp3EC.f3;_LL2:;
if(t == _tmp3EE)return orig_arg;
return({struct _tuple9*_tmp3ED=_cycalloc(sizeof(*_tmp3ED));_tmp3ED->f1=_tmp3F0,_tmp3ED->f2=_tmp3EF,_tmp3ED->f3=t;_tmp3ED;});}
# 2466
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2471
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2473
return({struct Cyc_Absyn_Exp*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->topt=old->topt,_tmp3F1->r=r,_tmp3F1->loc=old->loc,_tmp3F1->annot=old->annot;_tmp3F1;});}
# 2478
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3F2=e->r;void*_tmp3F3=_tmp3F2;void*_tmp433;void*_tmp432;struct Cyc_List_List*_tmp431;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Exp*_tmp42F;void*_tmp42E;void*_tmp42D;struct Cyc_Absyn_Exp*_tmp42C;int _tmp42B;enum Cyc_Absyn_Coercion _tmp42A;struct Cyc_Absyn_Exp*_tmp429;struct Cyc_Absyn_Exp*_tmp428;struct Cyc_Absyn_Exp*_tmp427;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_Absyn_Exp*_tmp424;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp422;struct Cyc_Absyn_Exp*_tmp421;enum Cyc_Absyn_Primop _tmp420;struct Cyc_List_List*_tmp41F;switch(*((int*)_tmp3F3)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp420=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp41F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LLC:
# 2487
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp41F)== 1){
struct Cyc_Absyn_Exp*_tmp3F4=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp41F))->hd;
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Tcutil_rsubsexp(r,inst,_tmp3F4);
if(_tmp3F5 == _tmp3F4)return e;
return({struct Cyc_Absyn_Exp*_tmp73D=e;Cyc_Tcutil_copye(_tmp73D,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->tag=3U,_tmp3F7->f1=_tmp420,({struct Cyc_List_List*_tmp73C=({struct Cyc_Absyn_Exp*_tmp3F6[1U];_tmp3F6[0]=_tmp3F5;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3F6,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp3F7->f2=_tmp73C;});_tmp3F7;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp41F)== 2){
struct Cyc_Absyn_Exp*_tmp3F8=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp41F))->hd;
struct Cyc_Absyn_Exp*_tmp3F9=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp41F->tl))->hd;
struct Cyc_Absyn_Exp*_tmp3FA=Cyc_Tcutil_rsubsexp(r,inst,_tmp3F8);
struct Cyc_Absyn_Exp*_tmp3FB=Cyc_Tcutil_rsubsexp(r,inst,_tmp3F9);
if(_tmp3FA == _tmp3F8 && _tmp3FB == _tmp3F9)return e;
return({struct Cyc_Absyn_Exp*_tmp73F=e;Cyc_Tcutil_copye(_tmp73F,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->tag=3U,_tmp3FD->f1=_tmp420,({struct Cyc_List_List*_tmp73E=({struct Cyc_Absyn_Exp*_tmp3FC[2U];_tmp3FC[0]=_tmp3FA,_tmp3FC[1]=_tmp3FB;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3FC,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp3FD->f2=_tmp73E;});_tmp3FD;}));});}else{
return({void*_tmp3FE=0U;({struct _fat_ptr _tmp740=({const char*_tmp3FF="primop does not have 1 or 2 args!";_tag_fat(_tmp3FF,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp740,_tag_fat(_tmp3FE,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp423=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp422=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_tmp421=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F3)->f3;_LLE: {
# 2501
struct Cyc_Absyn_Exp*_tmp400=Cyc_Tcutil_rsubsexp(r,inst,_tmp423);
struct Cyc_Absyn_Exp*_tmp401=Cyc_Tcutil_rsubsexp(r,inst,_tmp422);
struct Cyc_Absyn_Exp*_tmp402=Cyc_Tcutil_rsubsexp(r,inst,_tmp421);
if((_tmp400 == _tmp423 && _tmp401 == _tmp422)&& _tmp402 == _tmp421)return e;
return({struct Cyc_Absyn_Exp*_tmp741=e;Cyc_Tcutil_copye(_tmp741,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->tag=6U,_tmp403->f1=_tmp400,_tmp403->f2=_tmp401,_tmp403->f3=_tmp402;_tmp403;}));});}case 7U: _LLF: _tmp425=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp424=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LL10: {
# 2507
struct Cyc_Absyn_Exp*_tmp404=Cyc_Tcutil_rsubsexp(r,inst,_tmp425);
struct Cyc_Absyn_Exp*_tmp405=Cyc_Tcutil_rsubsexp(r,inst,_tmp424);
if(_tmp404 == _tmp425 && _tmp405 == _tmp424)return e;
return({struct Cyc_Absyn_Exp*_tmp742=e;Cyc_Tcutil_copye(_tmp742,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->tag=7U,_tmp406->f1=_tmp404,_tmp406->f2=_tmp405;_tmp406;}));});}case 8U: _LL11: _tmp427=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp426=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LL12: {
# 2512
struct Cyc_Absyn_Exp*_tmp407=Cyc_Tcutil_rsubsexp(r,inst,_tmp427);
struct Cyc_Absyn_Exp*_tmp408=Cyc_Tcutil_rsubsexp(r,inst,_tmp426);
if(_tmp407 == _tmp427 && _tmp408 == _tmp426)return e;
return({struct Cyc_Absyn_Exp*_tmp743=e;Cyc_Tcutil_copye(_tmp743,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->tag=8U,_tmp409->f1=_tmp407,_tmp409->f2=_tmp408;_tmp409;}));});}case 9U: _LL13: _tmp429=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp428=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LL14: {
# 2517
struct Cyc_Absyn_Exp*_tmp40A=Cyc_Tcutil_rsubsexp(r,inst,_tmp429);
struct Cyc_Absyn_Exp*_tmp40B=Cyc_Tcutil_rsubsexp(r,inst,_tmp428);
if(_tmp40A == _tmp429 && _tmp40B == _tmp428)return e;
return({struct Cyc_Absyn_Exp*_tmp744=e;Cyc_Tcutil_copye(_tmp744,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->tag=9U,_tmp40C->f1=_tmp40A,_tmp40C->f2=_tmp40B;_tmp40C;}));});}case 14U: _LL15: _tmp42D=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp42C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_tmp42B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F3)->f3;_tmp42A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F3)->f4;_LL16: {
# 2522
struct Cyc_Absyn_Exp*_tmp40D=Cyc_Tcutil_rsubsexp(r,inst,_tmp42C);
void*_tmp40E=Cyc_Tcutil_rsubstitute(r,inst,_tmp42D);
if(_tmp40D == _tmp42C && _tmp40E == _tmp42D)return e;
return({struct Cyc_Absyn_Exp*_tmp745=e;Cyc_Tcutil_copye(_tmp745,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->tag=14U,_tmp40F->f1=_tmp40E,_tmp40F->f2=_tmp40D,_tmp40F->f3=_tmp42B,_tmp40F->f4=_tmp42A;_tmp40F;}));});}case 17U: _LL17: _tmp42E=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_LL18: {
# 2527
void*_tmp410=Cyc_Tcutil_rsubstitute(r,inst,_tmp42E);
if(_tmp410 == _tmp42E)return e;
return({struct Cyc_Absyn_Exp*_tmp746=e;Cyc_Tcutil_copye(_tmp746,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->tag=17U,_tmp411->f1=_tmp410;_tmp411;}));});}case 18U: _LL19: _tmp42F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_LL1A: {
# 2531
struct Cyc_Absyn_Exp*_tmp412=Cyc_Tcutil_rsubsexp(r,inst,_tmp42F);
if(_tmp412 == _tmp42F)return e;
return({struct Cyc_Absyn_Exp*_tmp747=e;Cyc_Tcutil_copye(_tmp747,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->tag=18U,_tmp413->f1=_tmp412;_tmp413;}));});}case 41U: _LL1B: _tmp430=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_LL1C: {
# 2535
struct Cyc_Absyn_Exp*_tmp414=Cyc_Tcutil_rsubsexp(r,inst,_tmp430);
if(_tmp414 == _tmp430)return e;
return({struct Cyc_Absyn_Exp*_tmp748=e;Cyc_Tcutil_copye(_tmp748,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->tag=41U,_tmp415->f1=_tmp414;_tmp415;}));});}case 19U: _LL1D: _tmp432=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp431=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LL1E: {
# 2539
void*_tmp416=Cyc_Tcutil_rsubstitute(r,inst,_tmp432);
if(_tmp416 == _tmp432)return e;
return({struct Cyc_Absyn_Exp*_tmp749=e;Cyc_Tcutil_copye(_tmp749,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->tag=19U,_tmp417->f1=_tmp416,_tmp417->f2=_tmp431;_tmp417;}));});}case 39U: _LL1F: _tmp433=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_LL20: {
# 2543
void*_tmp418=Cyc_Tcutil_rsubstitute(r,inst,_tmp433);
if(_tmp418 == _tmp433)return e;{
# 2546
void*_tmp419=Cyc_Tcutil_compress(_tmp418);void*_tmp41A=_tmp419;struct Cyc_Absyn_Exp*_tmp41C;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp41A)->tag == 9U){_LL24: _tmp41C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp41A)->f1;_LL25:
 return _tmp41C;}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp74A=e;Cyc_Tcutil_copye(_tmp74A,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->tag=39U,_tmp41B->f1=_tmp418;_tmp41B;}));});}_LL23:;};}default: _LL21: _LL22:
# 2550
 return({void*_tmp41D=0U;({struct _fat_ptr _tmp74B=({const char*_tmp41E="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp41E,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp74B,_tag_fat(_tmp41D,sizeof(void*),0U));});});}_LL0:;}
# 2554
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2557
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 2561
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2564
void*_tmp434=f->type;
struct Cyc_Absyn_Exp*_tmp435=f->requires_clause;
void*_tmp436=Cyc_Tcutil_rsubstitute(r,inst,_tmp434);
struct Cyc_Absyn_Exp*_tmp437=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp435);
if(_tmp434 == _tmp436 && _tmp435 == _tmp437)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->name=f->name,_tmp438->tq=f->tq,_tmp438->type=_tmp436,_tmp438->width=f->width,_tmp438->attributes=f->attributes,_tmp438->requires_clause=_tmp437;_tmp438;});}}
# 2574
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2577
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp439=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp43A=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp43B=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp439);
struct Cyc_List_List*_tmp43C=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp43A);
if(_tmp43B == _tmp439 && _tmp43C == _tmp43A)return fs;
return({struct Cyc_List_List*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->hd=_tmp43B,_tmp43D->tl=_tmp43C;_tmp43D;});};}
# 2586
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2589
struct _tuple0 _tmp43E=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _tmp43F=_tmp43E;struct Cyc_List_List*_tmp443;struct Cyc_List_List*_tmp442;_LL1: _tmp443=_tmp43F.f1;_tmp442=_tmp43F.f2;_LL2:;{
struct Cyc_List_List*_tmp440=Cyc_Tcutil_substs(rgn,inst,_tmp443);
struct Cyc_List_List*_tmp441=Cyc_Tcutil_substs(rgn,inst,_tmp442);
if(_tmp440 == _tmp443 && _tmp441 == _tmp442)
return rgn_po;else{
# 2595
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp440,_tmp441);}};}
# 2598
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2601
void*_tmp444=Cyc_Tcutil_compress(t);void*_tmp445=_tmp444;struct Cyc_Absyn_Exp*_tmp49C;struct Cyc_Absyn_Exp*_tmp49B;void*_tmp49A;struct Cyc_List_List*_tmp499;void*_tmp498;void*_tmp497;enum Cyc_Absyn_AggrKind _tmp496;struct Cyc_List_List*_tmp495;struct Cyc_List_List*_tmp494;struct Cyc_List_List*_tmp493;void*_tmp492;struct Cyc_Absyn_Tqual _tmp491;void*_tmp490;struct Cyc_List_List*_tmp48F;int _tmp48E;struct Cyc_Absyn_VarargInfo*_tmp48D;struct Cyc_List_List*_tmp48C;struct Cyc_List_List*_tmp48B;struct Cyc_Absyn_Exp*_tmp48A;struct Cyc_Absyn_Exp*_tmp489;void*_tmp488;struct Cyc_Absyn_Tqual _tmp487;void*_tmp486;void*_tmp485;void*_tmp484;void*_tmp483;void*_tmp482;struct Cyc_Absyn_Tqual _tmp481;struct Cyc_Absyn_Exp*_tmp480;void*_tmp47F;unsigned _tmp47E;struct _tuple1*_tmp47D;struct Cyc_List_List*_tmp47C;struct Cyc_Absyn_Typedefdecl*_tmp47B;void*_tmp47A;struct Cyc_Absyn_Tvar*_tmp479;switch(*((int*)_tmp445)){case 2U: _LL1: _tmp479=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp445)->f1;_LL2: {
# 2604
struct _handler_cons _tmp446;_push_handler(& _tmp446);{int _tmp448=0;if(setjmp(_tmp446.handler))_tmp448=1;if(!_tmp448){{void*_tmp449=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp479);_npop_handler(0U);return _tmp449;};_pop_handler();}else{void*_tmp447=(void*)Cyc_Core_get_exn_thrown();void*_tmp44A=_tmp447;void*_tmp44B;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp44A)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp44B=_tmp44A;_LL1F:(int)_rethrow(_tmp44B);}_LL1B:;}};}case 8U: _LL3: _tmp47D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp445)->f1;_tmp47C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp445)->f2;_tmp47B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp445)->f3;_tmp47A=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp445)->f4;_LL4: {
# 2607
struct Cyc_List_List*_tmp44C=Cyc_Tcutil_substs(rgn,inst,_tmp47C);
return _tmp44C == _tmp47C?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->tag=8U,_tmp44D->f1=_tmp47D,_tmp44D->f2=_tmp44C,_tmp44D->f3=_tmp47B,_tmp44D->f4=_tmp47A;_tmp44D;});}case 4U: _LL5: _tmp482=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).elt_type;_tmp481=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).tq;_tmp480=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).num_elts;_tmp47F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).zero_term;_tmp47E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).zt_loc;_LL6: {
# 2610
void*_tmp44E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp482);
struct Cyc_Absyn_Exp*_tmp44F=_tmp480 == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp480);
void*_tmp450=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp47F);
return(_tmp44E == _tmp482 && _tmp44F == _tmp480)&& _tmp450 == _tmp47F?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp451=_cycalloc(sizeof(*_tmp451));
_tmp451->tag=4U,(_tmp451->f1).elt_type=_tmp44E,(_tmp451->f1).tq=_tmp481,(_tmp451->f1).num_elts=_tmp44F,(_tmp451->f1).zero_term=_tmp450,(_tmp451->f1).zt_loc=_tmp47E;_tmp451;});}case 3U: _LL7: _tmp488=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).elt_type;_tmp487=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).elt_tq;_tmp486=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).rgn;_tmp485=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).nullable;_tmp484=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).bounds;_tmp483=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).zero_term;_LL8: {
# 2616
void*_tmp452=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp488);
void*_tmp453=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp486);
void*_tmp454=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp484);
void*_tmp455=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp483);
if(((_tmp452 == _tmp488 && _tmp453 == _tmp486)&& _tmp454 == _tmp484)&& _tmp455 == _tmp483)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp62B;_tmp62B.elt_type=_tmp452,_tmp62B.elt_tq=_tmp487,(_tmp62B.ptr_atts).rgn=_tmp453,(_tmp62B.ptr_atts).nullable=_tmp485,(_tmp62B.ptr_atts).bounds=_tmp454,(_tmp62B.ptr_atts).zero_term=_tmp455,(_tmp62B.ptr_atts).ptrloc=0;_tmp62B;}));}case 5U: _LL9: _tmp493=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).tvars;_tmp492=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).effect;_tmp491=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).ret_tqual;_tmp490=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).ret_type;_tmp48F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).args;_tmp48E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).c_varargs;_tmp48D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).cyc_varargs;_tmp48C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).rgn_po;_tmp48B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).attributes;_tmp48A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).requires_clause;_tmp489=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).ensures_clause;_LLA:
# 2626
{struct Cyc_List_List*_tmp456=_tmp493;for(0;_tmp456 != 0;_tmp456=_tmp456->tl){
inst=({struct Cyc_List_List*_tmp458=_region_malloc(rgn,sizeof(*_tmp458));({struct _tuple18*_tmp74D=({struct _tuple18*_tmp457=_region_malloc(rgn,sizeof(*_tmp457));_tmp457->f1=(struct Cyc_Absyn_Tvar*)_tmp456->hd,({void*_tmp74C=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp456->hd);_tmp457->f2=_tmp74C;});_tmp457;});_tmp458->hd=_tmp74D;}),_tmp458->tl=inst;_tmp458;});}}{
struct _tuple0 _tmp459=({struct _RegionHandle*_tmp74F=rgn;struct _RegionHandle*_tmp74E=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp74F,_tmp74E,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp48F));});
# 2628
struct _tuple0 _tmp45A=_tmp459;struct Cyc_List_List*_tmp469;struct Cyc_List_List*_tmp468;_LL21: _tmp469=_tmp45A.f1;_tmp468=_tmp45A.f2;_LL22:;{
# 2630
struct Cyc_List_List*_tmp45B=_tmp48F;
struct Cyc_List_List*_tmp45C=Cyc_Tcutil_substs(rgn,inst,_tmp468);
if(_tmp45C != _tmp468)
_tmp45B=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp48F,_tmp45C);{
void*eff2;
if(_tmp492 == 0)
eff2=0;else{
# 2638
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp492);
if(new_eff == _tmp492)
eff2=_tmp492;else{
# 2642
eff2=new_eff;}}{
# 2644
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp48D == 0)
cyc_varargs2=0;else{
# 2648
struct Cyc_Absyn_VarargInfo _tmp45D=*_tmp48D;struct Cyc_Absyn_VarargInfo _tmp45E=_tmp45D;struct _fat_ptr*_tmp464;struct Cyc_Absyn_Tqual _tmp463;void*_tmp462;int _tmp461;_LL24: _tmp464=_tmp45E.name;_tmp463=_tmp45E.tq;_tmp462=_tmp45E.type;_tmp461=_tmp45E.inject;_LL25:;{
void*_tmp45F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp462);
if(_tmp45F == _tmp462)cyc_varargs2=_tmp48D;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->name=_tmp464,_tmp460->tq=_tmp463,_tmp460->type=_tmp45F,_tmp460->inject=_tmp461;_tmp460;});}};}{
# 2653
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp48C);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp48A);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp489);
struct Cyc_List_List*_tmp465=Cyc_Tcutil_extract_relns(_tmp45B,req2);
struct Cyc_List_List*_tmp466=Cyc_Tcutil_extract_relns(_tmp45B,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->tag=5U,(_tmp467->f1).tvars=_tmp493,(_tmp467->f1).effect=eff2,(_tmp467->f1).ret_tqual=_tmp491,({void*_tmp750=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp490);(_tmp467->f1).ret_type=_tmp750;}),(_tmp467->f1).args=_tmp45B,(_tmp467->f1).c_varargs=_tmp48E,(_tmp467->f1).cyc_varargs=cyc_varargs2,(_tmp467->f1).rgn_po=rgn_po2,(_tmp467->f1).attributes=_tmp48B,(_tmp467->f1).requires_clause=req2,(_tmp467->f1).requires_relns=_tmp465,(_tmp467->f1).ensures_clause=ens2,(_tmp467->f1).ensures_relns=_tmp466;_tmp467;});};};};};};case 6U: _LLB: _tmp494=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp445)->f1;_LLC: {
# 2662
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp46A=_tmp494;for(0;_tmp46A != 0;_tmp46A=_tmp46A->tl){
void*_tmp46B=(*((struct _tuple11*)_tmp46A->hd)).f2;
void*_tmp46C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp46B);
if(_tmp46B != _tmp46C)
change=1;
# 2670
ts2=({struct Cyc_List_List*_tmp46D=_region_malloc(rgn,sizeof(*_tmp46D));_tmp46D->hd=_tmp46C,_tmp46D->tl=ts2;_tmp46D;});}}
# 2672
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->tag=6U,({struct Cyc_List_List*_tmp752=({struct Cyc_List_List*_tmp751=_tmp494;((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp751,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp46E->f1=_tmp752;});_tmp46E;});}case 7U: _LLD: _tmp496=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp445)->f1;_tmp495=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp445)->f2;_LLE: {
# 2676
struct Cyc_List_List*_tmp46F=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp495);
if(_tmp495 == _tmp46F)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=7U,_tmp470->f1=_tmp496,_tmp470->f2=_tmp46F;_tmp470;});}case 1U: _LLF: _tmp497=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp445)->f2;_LL10:
# 2680
 if(_tmp497 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp497);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f2 == 0){_LL11: _tmp498=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f1;_LL12:
 return t;}else{_LL13: _tmp49A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f1;_tmp499=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f2;_LL14: {
# 2684
struct Cyc_List_List*_tmp471=Cyc_Tcutil_substs(rgn,inst,_tmp499);
if(_tmp499 == _tmp471)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp472=_cycalloc(sizeof(*_tmp472));_tmp472->tag=0U,_tmp472->f1=_tmp49A,_tmp472->f2=_tmp471;_tmp472;});}}}case 9U: _LL15: _tmp49B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp445)->f1;_LL16: {
# 2688
struct Cyc_Absyn_Exp*_tmp473=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp49B);
return _tmp473 == _tmp49B?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->tag=9U,_tmp474->f1=_tmp473;_tmp474;});}case 11U: _LL17: _tmp49C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp445)->f1;_LL18: {
# 2691
struct Cyc_Absyn_Exp*_tmp475=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp49C);
return _tmp475 == _tmp49C?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp476=_cycalloc(sizeof(*_tmp476));_tmp476->tag=11U,_tmp476->f1=_tmp475;_tmp476;});}default: _LL19: _LL1A:
({void*_tmp477=0U;({struct _fat_ptr _tmp753=({const char*_tmp478="found typedecltype in rsubs";_tag_fat(_tmp478,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp753,_tag_fat(_tmp477,sizeof(void*),0U));});});}_LL0:;}
# 2697
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2700
if(ts == 0)
return 0;{
void*_tmp49D=(void*)ts->hd;
struct Cyc_List_List*_tmp49E=ts->tl;
void*_tmp49F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp49D);
struct Cyc_List_List*_tmp4A0=Cyc_Tcutil_substs(rgn,inst,_tmp49E);
if(_tmp49D == _tmp49F && _tmp49E == _tmp4A0)
return ts;
return({struct Cyc_List_List*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->hd=_tmp49F,_tmp4A1->tl=_tmp4A0;_tmp4A1;});};}
# 2711
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 2718
struct _tuple18*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4A2=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple18*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->f1=tv,({void*_tmp755=({struct Cyc_Core_Opt*_tmp754=_tmp4A2;Cyc_Absyn_new_evar(_tmp754,({struct Cyc_Core_Opt*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->v=s;_tmp4A3;}));});_tmp4A4->f2=_tmp755;});_tmp4A4;});}struct _tuple27{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2723
struct _tuple18*Cyc_Tcutil_r_make_inst_var(struct _tuple27*env,struct Cyc_Absyn_Tvar*tv){
# 2725
struct _tuple27*_tmp4A5=env;struct Cyc_List_List*_tmp4AA;struct _RegionHandle*_tmp4A9;_LL1: _tmp4AA=_tmp4A5->f1;_tmp4A9=_tmp4A5->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp4A6=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple18*_tmp4A8=_region_malloc(_tmp4A9,sizeof(*_tmp4A8));_tmp4A8->f1=tv,({void*_tmp757=({struct Cyc_Core_Opt*_tmp756=_tmp4A6;Cyc_Absyn_new_evar(_tmp756,({struct Cyc_Core_Opt*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->v=_tmp4AA;_tmp4A7;}));});_tmp4A8->f2=_tmp757;});_tmp4A8;});};}
# 2730
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2732
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp4AD=({struct Cyc_String_pa_PrintArg_struct _tmp62C;_tmp62C.tag=0U,_tmp62C.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp62C;});void*_tmp4AB[1U];_tmp4AB[0]=& _tmp4AD;({unsigned _tmp759=loc;struct _fat_ptr _tmp758=({const char*_tmp4AC="bitfield %s does not have constant width";_tag_fat(_tmp4AC,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp759,_tmp758,_tag_fat(_tmp4AB,sizeof(void*),1U));});});else{
# 2737
struct _tuple12 _tmp4AE=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple12 _tmp4AF=_tmp4AE;unsigned _tmp4B5;int _tmp4B4;_LL1: _tmp4B5=_tmp4AF.f1;_tmp4B4=_tmp4AF.f2;_LL2:;
if(!_tmp4B4)
({void*_tmp4B0=0U;({unsigned _tmp75B=loc;struct _fat_ptr _tmp75A=({const char*_tmp4B1="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4B1,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp75B,_tmp75A,_tag_fat(_tmp4B0,sizeof(void*),0U));});});
if((int)_tmp4B5 < 0)
({void*_tmp4B2=0U;({unsigned _tmp75D=loc;struct _fat_ptr _tmp75C=({const char*_tmp4B3="bitfield has negative width";_tag_fat(_tmp4B3,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp75D,_tmp75C,_tag_fat(_tmp4B2,sizeof(void*),0U));});});
w=_tmp4B5;}{
# 2744
void*_tmp4B6=Cyc_Tcutil_compress(field_type);void*_tmp4B7=_tmp4B6;enum Cyc_Absyn_Size_of _tmp4C5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B7)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B7)->f1)->tag == 1U){_LL4: _tmp4C5=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B7)->f1)->f2;_LL5:
# 2747
{enum Cyc_Absyn_Size_of _tmp4B8=_tmp4C5;switch(_tmp4B8){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp4B9=0U;({unsigned _tmp75F=loc;struct _fat_ptr _tmp75E=({const char*_tmp4BA="bitfield larger than type";_tag_fat(_tmp4BA,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp75F,_tmp75E,_tag_fat(_tmp4B9,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp4BB=0U;({unsigned _tmp761=loc;struct _fat_ptr _tmp760=({const char*_tmp4BC="bitfield larger than type";_tag_fat(_tmp4BC,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp761,_tmp760,_tag_fat(_tmp4BB,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp4BD=0U;({unsigned _tmp763=loc;struct _fat_ptr _tmp762=({const char*_tmp4BE="bitfield larger than type";_tag_fat(_tmp4BE,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp763,_tmp762,_tag_fat(_tmp4BD,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp4BF=0U;({unsigned _tmp765=loc;struct _fat_ptr _tmp764=({const char*_tmp4C0="bitfield larger than type";_tag_fat(_tmp4C0,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp765,_tmp764,_tag_fat(_tmp4BF,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 2755
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 2757
({struct Cyc_String_pa_PrintArg_struct _tmp4C3=({struct Cyc_String_pa_PrintArg_struct _tmp62E;_tmp62E.tag=0U,_tmp62E.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp62E;});struct Cyc_String_pa_PrintArg_struct _tmp4C4=({struct Cyc_String_pa_PrintArg_struct _tmp62D;_tmp62D.tag=0U,({
struct _fat_ptr _tmp766=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field_type));_tmp62D.f1=_tmp766;});_tmp62D;});void*_tmp4C1[2U];_tmp4C1[0]=& _tmp4C3,_tmp4C1[1]=& _tmp4C4;({unsigned _tmp768=loc;struct _fat_ptr _tmp767=({const char*_tmp4C2="bitfield %s must have integral type but has type %s";_tag_fat(_tmp4C2,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp768,_tmp767,_tag_fat(_tmp4C1,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 2766
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4C6=t;struct Cyc_Absyn_Typedefdecl*_tmp4CA;void*_tmp4C9;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C6)->tag == 8U){_LL1: _tmp4CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C6)->f3;_tmp4C9=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C6)->f4;_LL2:
# 2769
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp4CA))->tq).real_const ||(_tmp4CA->tq).print_const){
if(declared_const)({void*_tmp4C7=0U;({unsigned _tmp76A=loc;struct _fat_ptr _tmp769=({const char*_tmp4C8="extra const";_tag_fat(_tmp4C8,sizeof(char),12U);});Cyc_Tcutil_warn(_tmp76A,_tmp769,_tag_fat(_tmp4C7,sizeof(void*),0U));});});
return 1;}
# 2774
if((unsigned)_tmp4C9)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp4C9);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2781
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp76B=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp76B;});}
# 2785
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2791
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2794
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4CD=({struct Cyc_String_pa_PrintArg_struct _tmp630;_tmp630.tag=0U,_tmp630.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp630;});struct Cyc_String_pa_PrintArg_struct _tmp4CE=({struct Cyc_String_pa_PrintArg_struct _tmp62F;_tmp62F.tag=0U,({struct _fat_ptr _tmp76C=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp62F.f1=_tmp76C;});_tmp62F;});void*_tmp4CB[2U];_tmp4CB[0]=& _tmp4CD,_tmp4CB[1]=& _tmp4CE;({unsigned _tmp76E=loc;struct _fat_ptr _tmp76D=({const char*_tmp4CC="%s: %s";_tag_fat(_tmp4CC,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp76E,_tmp76D,_tag_fat(_tmp4CB,sizeof(void*),2U));});});}}}
# 2800
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2804
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2808
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp770=tvs;unsigned _tmp76F=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp770,_tmp76F,Cyc_Absynpp_tvar2string,({const char*_tmp4CF="duplicate type variable";_tag_fat(_tmp4CF,sizeof(char),24U);}));});}struct _tuple28{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple29{struct Cyc_List_List*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2822 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2827
struct _RegionHandle _tmp4D0=_new_region("temp");struct _RegionHandle*temp=& _tmp4D0;_push_region(temp);
# 2831
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp771=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp771,({const char*_tmp4D1="";_tag_fat(_tmp4D1,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4D3=_region_malloc(temp,sizeof(*_tmp4D3));({struct _tuple28*_tmp772=({struct _tuple28*_tmp4D2=_region_malloc(temp,sizeof(*_tmp4D2));_tmp4D2->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4D2->f2=0;_tmp4D2;});_tmp4D3->hd=_tmp772;}),_tmp4D3->tl=fields;_tmp4D3;});}}
# 2836
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2838
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp4F8="struct";_tag_fat(_tmp4F8,sizeof(char),7U);}):({const char*_tmp4F9="union";_tag_fat(_tmp4F9,sizeof(char),6U);});
# 2841
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple29*_tmp4D4=(struct _tuple29*)des->hd;struct _tuple29*_tmp4D5=_tmp4D4;struct Cyc_List_List*_tmp4F0;void*_tmp4EF;_LL1: _tmp4F0=_tmp4D5->f1;_tmp4EF=_tmp4D5->f2;_LL2:;
if(_tmp4F0 == 0){
# 2846
struct Cyc_List_List*_tmp4D6=fields;
for(0;_tmp4D6 != 0;_tmp4D6=_tmp4D6->tl){
if(!(*((struct _tuple28*)_tmp4D6->hd)).f2){
(*((struct _tuple28*)_tmp4D6->hd)).f2=1;
({struct Cyc_List_List*_tmp774=({struct Cyc_List_List*_tmp4D8=_cycalloc(sizeof(*_tmp4D8));({void*_tmp773=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));_tmp4D7->tag=1U,_tmp4D7->f1=((*((struct _tuple28*)_tmp4D6->hd)).f1)->name;_tmp4D7;});_tmp4D8->hd=_tmp773;}),_tmp4D8->tl=0;_tmp4D8;});(*((struct _tuple29*)des->hd)).f1=_tmp774;});
ans=({struct Cyc_List_List*_tmp4DA=_region_malloc(rgn,sizeof(*_tmp4DA));({struct _tuple30*_tmp775=({struct _tuple30*_tmp4D9=_region_malloc(rgn,sizeof(*_tmp4D9));_tmp4D9->f1=(*((struct _tuple28*)_tmp4D6->hd)).f1,_tmp4D9->f2=_tmp4EF;_tmp4D9;});_tmp4DA->hd=_tmp775;}),_tmp4DA->tl=ans;_tmp4DA;});
break;}}
# 2854
if(_tmp4D6 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4DD=({struct Cyc_String_pa_PrintArg_struct _tmp631;_tmp631.tag=0U,_tmp631.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp631;});void*_tmp4DB[1U];_tmp4DB[0]=& _tmp4DD;({unsigned _tmp777=loc;struct _fat_ptr _tmp776=({const char*_tmp4DC="too many arguments to %s";_tag_fat(_tmp4DC,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp777,_tmp776,_tag_fat(_tmp4DB,sizeof(void*),1U));});});}else{
if(_tmp4F0->tl != 0)
# 2858
({void*_tmp4DE=0U;({unsigned _tmp779=loc;struct _fat_ptr _tmp778=({const char*_tmp4DF="multiple designators are not yet supported";_tag_fat(_tmp4DF,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp779,_tmp778,_tag_fat(_tmp4DE,sizeof(void*),0U));});});else{
# 2861
void*_tmp4E0=(void*)_tmp4F0->hd;void*_tmp4E1=_tmp4E0;struct _fat_ptr*_tmp4EE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4E1)->tag == 0U){_LL4: _LL5:
# 2863
({struct Cyc_String_pa_PrintArg_struct _tmp4E4=({struct Cyc_String_pa_PrintArg_struct _tmp632;_tmp632.tag=0U,_tmp632.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp632;});void*_tmp4E2[1U];_tmp4E2[0]=& _tmp4E4;({unsigned _tmp77B=loc;struct _fat_ptr _tmp77A=({const char*_tmp4E3="array designator used in argument to %s";_tag_fat(_tmp4E3,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp77B,_tmp77A,_tag_fat(_tmp4E2,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp4EE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4E1)->f1;_LL7: {
# 2866
struct Cyc_List_List*_tmp4E5=fields;
for(0;_tmp4E5 != 0;_tmp4E5=_tmp4E5->tl){
if(Cyc_strptrcmp(_tmp4EE,((*((struct _tuple28*)_tmp4E5->hd)).f1)->name)== 0){
if((*((struct _tuple28*)_tmp4E5->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp4E8=({struct Cyc_String_pa_PrintArg_struct _tmp633;_tmp633.tag=0U,_tmp633.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp4EE);_tmp633;});void*_tmp4E6[1U];_tmp4E6[0]=& _tmp4E8;({unsigned _tmp77D=loc;struct _fat_ptr _tmp77C=({const char*_tmp4E7="member %s has already been used as an argument";_tag_fat(_tmp4E7,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp77D,_tmp77C,_tag_fat(_tmp4E6,sizeof(void*),1U));});});
(*((struct _tuple28*)_tmp4E5->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4EA=_region_malloc(rgn,sizeof(*_tmp4EA));({struct _tuple30*_tmp77E=({struct _tuple30*_tmp4E9=_region_malloc(rgn,sizeof(*_tmp4E9));_tmp4E9->f1=(*((struct _tuple28*)_tmp4E5->hd)).f1,_tmp4E9->f2=_tmp4EF;_tmp4E9;});_tmp4EA->hd=_tmp77E;}),_tmp4EA->tl=ans;_tmp4EA;});
break;}}
# 2875
if(_tmp4E5 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4ED=({struct Cyc_String_pa_PrintArg_struct _tmp634;_tmp634.tag=0U,_tmp634.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp4EE);_tmp634;});void*_tmp4EB[1U];_tmp4EB[0]=& _tmp4ED;({unsigned _tmp780=loc;struct _fat_ptr _tmp77F=({const char*_tmp4EC="bad field designator %s";_tag_fat(_tmp4EC,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp780,_tmp77F,_tag_fat(_tmp4EB,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 2880
if((int)aggr_kind == (int)0U)
# 2882
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple28*)fields->hd)).f2){
({void*_tmp4F1=0U;({unsigned _tmp782=loc;struct _fat_ptr _tmp781=({const char*_tmp4F2="too few arguments to struct";_tag_fat(_tmp4F2,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp782,_tmp781,_tag_fat(_tmp4F1,sizeof(void*),0U));});});
break;}}else{
# 2889
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple28*)fields->hd)).f2){
if(found)({void*_tmp4F3=0U;({unsigned _tmp784=loc;struct _fat_ptr _tmp783=({const char*_tmp4F4="only one member of a union is allowed";_tag_fat(_tmp4F4,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp784,_tmp783,_tag_fat(_tmp4F3,sizeof(void*),0U));});});
found=1;}}
# 2895
if(!found)({void*_tmp4F5=0U;({unsigned _tmp786=loc;struct _fat_ptr _tmp785=({const char*_tmp4F6="missing member for union";_tag_fat(_tmp4F6,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp786,_tmp785,_tag_fat(_tmp4F5,sizeof(void*),0U));});});}{
# 2898
struct Cyc_List_List*_tmp4F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp4F7;};};}
# 2831
;_pop_region(temp);}
# 2904
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2906
void*_tmp4FA=e1->r;void*_tmp4FB=_tmp4FA;struct Cyc_Absyn_Exp*_tmp50D;struct Cyc_Absyn_Exp*_tmp50C;struct Cyc_Absyn_Exp*_tmp50B;struct Cyc_Absyn_Exp*_tmp50A;struct Cyc_Absyn_Exp*_tmp509;struct Cyc_Absyn_Exp*_tmp508;switch(*((int*)_tmp4FB)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp4FE=({struct Cyc_String_pa_PrintArg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp787=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp635.f1=_tmp787;});_tmp635;});void*_tmp4FC[1U];_tmp4FC[0]=& _tmp4FE;({struct _fat_ptr _tmp788=({const char*_tmp4FD="we have a cast in a lhs:  %s";_tag_fat(_tmp4FD,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp788,_tag_fat(_tmp4FC,sizeof(void*),1U));});});case 20U: _LL3: _tmp508=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LL4:
 _tmp509=_tmp508;goto _LL6;case 23U: _LL5: _tmp509=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LL6:
# 2910
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp509->topt),ptr_type,is_fat,elt_type);case 22U: _LL7: _tmp50A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LL8:
 _tmp50B=_tmp50A;goto _LLA;case 21U: _LL9: _tmp50B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LLA:
# 2914
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp50B->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp501=({struct Cyc_String_pa_PrintArg_struct _tmp636;_tmp636.tag=0U,({
struct _fat_ptr _tmp789=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp636.f1=_tmp789;});_tmp636;});void*_tmp4FF[1U];_tmp4FF[0]=& _tmp501;({struct _fat_ptr _tmp78A=({const char*_tmp500="found zero pointer aggregate member assignment: %s";_tag_fat(_tmp500,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78A,_tag_fat(_tmp4FF,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp50C=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LLC:
 _tmp50D=_tmp50C;goto _LLE;case 12U: _LLD: _tmp50D=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LLE:
# 2920
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp50D->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp504=({struct Cyc_String_pa_PrintArg_struct _tmp637;_tmp637.tag=0U,({
struct _fat_ptr _tmp78B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp637.f1=_tmp78B;});_tmp637;});void*_tmp502[1U];_tmp502[0]=& _tmp504;({struct _fat_ptr _tmp78C=({const char*_tmp503="found zero pointer instantiate/noinstantiate: %s";_tag_fat(_tmp503,sizeof(char),49U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78C,_tag_fat(_tmp502,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp507=({struct Cyc_String_pa_PrintArg_struct _tmp638;_tmp638.tag=0U,({struct _fat_ptr _tmp78D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp638.f1=_tmp78D;});_tmp638;});void*_tmp505[1U];_tmp505[0]=& _tmp507;({struct _fat_ptr _tmp78E=({const char*_tmp506="found bad lhs in is_zero_ptr_deref: %s";_tag_fat(_tmp506,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78E,_tag_fat(_tmp505,sizeof(void*),1U));});});}_LL0:;}
# 2935
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 2938
void*_tmp50E=Cyc_Tcutil_compress(r);void*_tmp50F=_tmp50E;struct Cyc_Absyn_Tvar*_tmp51B;enum Cyc_Absyn_KindQual _tmp51A;enum Cyc_Absyn_AliasQual _tmp519;switch(*((int*)_tmp50F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50F)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f4 == 0){_LL5: _tmp51A=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f3)->kind)->v)->kind;_tmp519=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f3)->kind)->v)->aliasqual;_LL6:
# 2942
 return(int)_tmp51A == (int)3U &&((int)_tmp519 == (int)1U ||(int)_tmp519 == (int)2U && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp51B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp50F)->f1;_LL8: {
# 2946
struct Cyc_Absyn_Kind*_tmp510=Cyc_Tcutil_tvar_kind(_tmp51B,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp511=_tmp510;enum Cyc_Absyn_KindQual _tmp518;enum Cyc_Absyn_AliasQual _tmp517;_LLC: _tmp518=_tmp511->kind;_tmp517=_tmp511->aliasqual;_LLD:;
if((int)_tmp518 == (int)3U &&((int)_tmp517 == (int)1U ||(int)_tmp517 == (int)2U && !must_be_unique)){
void*_tmp512=Cyc_Absyn_compress_kb(_tmp51B->kind);void*_tmp513=_tmp512;struct Cyc_Core_Opt**_tmp516;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp513)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp513)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp513)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp516=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp513)->f1;_LL10:
# 2950
({struct Cyc_Core_Opt*_tmp790=({struct Cyc_Core_Opt*_tmp515=_cycalloc(sizeof(*_tmp515));({void*_tmp78F=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->tag=2U,_tmp514->f1=0,_tmp514->f2=& Cyc_Tcutil_rk;_tmp514;});_tmp515->v=_tmp78F;});_tmp515;});*_tmp516=_tmp790;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2955
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2962
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp51C=Cyc_Tcutil_compress(t);void*_tmp51D=_tmp51C;struct Cyc_Absyn_Tvar*_tmp52B;void*_tmp52A;switch(*((int*)_tmp51D)){case 3U: _LL1: _tmp52A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51D)->f1).ptr_atts).rgn;_LL2:
# 2965
 return Cyc_Tcutil_is_noalias_region(_tmp52A,must_be_unique);case 2U: _LL3: _tmp52B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp51D)->f1;_LL4: {
# 2967
struct Cyc_Absyn_Kind*_tmp51E=Cyc_Tcutil_tvar_kind(_tmp52B,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp51F=_tmp51E;enum Cyc_Absyn_KindQual _tmp529;enum Cyc_Absyn_AliasQual _tmp528;_LL8: _tmp529=_tmp51F->kind;_tmp528=_tmp51F->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp520=_tmp529;switch(_tmp520){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)_tmp528 == (int)1U ||(int)_tmp528 == (int)2U && !must_be_unique){
void*_tmp521=Cyc_Absyn_compress_kb(_tmp52B->kind);void*_tmp522=_tmp521;struct Cyc_Core_Opt**_tmp527;enum Cyc_Absyn_KindQual _tmp526;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp522)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp522)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp527=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp522)->f1;_tmp526=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp522)->f2)->kind;_LL15:
# 2973
({struct Cyc_Core_Opt*_tmp793=({struct Cyc_Core_Opt*_tmp525=_cycalloc(sizeof(*_tmp525));({void*_tmp792=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->tag=2U,_tmp524->f1=0,({struct Cyc_Absyn_Kind*_tmp791=({struct Cyc_Absyn_Kind*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->kind=_tmp526,_tmp523->aliasqual=Cyc_Absyn_Aliasable;_tmp523;});_tmp524->f2=_tmp791;});_tmp524;});_tmp525->v=_tmp792;});_tmp525;});*_tmp527=_tmp793;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 2977
 return 1;}_LL13:;}
# 2980
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 2983
 return 0;}_LL0:;}
# 2986
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp52C=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp52C,0))return 1;{
void*_tmp52D=_tmp52C;struct Cyc_List_List*_tmp541;union Cyc_Absyn_DatatypeFieldInfo _tmp540;struct Cyc_List_List*_tmp53F;union Cyc_Absyn_DatatypeInfo _tmp53E;struct Cyc_List_List*_tmp53D;struct Cyc_Absyn_Aggrdecl**_tmp53C;struct Cyc_List_List*_tmp53B;struct Cyc_List_List*_tmp53A;switch(*((int*)_tmp52D)){case 6U: _LL1: _tmp53A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52D)->f1;_LL2:
# 2991
 while(_tmp53A != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple11*)_tmp53A->hd)).f2))return 1;
_tmp53A=_tmp53A->tl;}
# 2995
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp53C=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)->f1).KnownAggr).val;_tmp53B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f2;_LL4:
# 2997
 if((*_tmp53C)->impl == 0)return 0;else{
# 2999
struct Cyc_List_List*_tmp52E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp53C)->tvs,_tmp53B);
struct Cyc_List_List*_tmp52F=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp53C)->impl))->fields;
void*t;
while(_tmp52F != 0){
t=_tmp52E == 0?((struct Cyc_Absyn_Aggrfield*)_tmp52F->hd)->type: Cyc_Tcutil_substitute(_tmp52E,((struct Cyc_Absyn_Aggrfield*)_tmp52F->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp52F=_tmp52F->tl;}
# 3007
return 0;}}else{_LL7: _LL8:
# 3017
 return 0;}case 18U: _LL9: _tmp53E=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)->f1;_tmp53D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f2;_LLA: {
# 3019
union Cyc_Absyn_DatatypeInfo _tmp530=_tmp53E;struct Cyc_List_List*_tmp534;struct Cyc_Core_Opt*_tmp533;struct _tuple1*_tmp532;int _tmp531;if((_tmp530.UnknownDatatype).tag == 1){_LL10: _tmp532=((_tmp530.UnknownDatatype).val).name;_tmp531=((_tmp530.UnknownDatatype).val).is_extensible;_LL11:
# 3022
 return 0;}else{_LL12: _tmp534=(*(_tmp530.KnownDatatype).val)->tvs;_tmp533=(*(_tmp530.KnownDatatype).val)->fields;_LL13:
# 3025
 return 0;}_LLF:;}case 19U: _LLB: _tmp540=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)->f1;_tmp53F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f2;_LLC: {
# 3028
union Cyc_Absyn_DatatypeFieldInfo _tmp535=_tmp540;struct Cyc_Absyn_Datatypedecl*_tmp539;struct Cyc_Absyn_Datatypefield*_tmp538;if((_tmp535.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 3031
 return 0;}else{_LL17: _tmp539=((_tmp535.KnownDatatypefield).val).f1;_tmp538=((_tmp535.KnownDatatypefield).val).f2;_LL18: {
# 3033
struct Cyc_List_List*_tmp536=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp539->tvs,_tmp53F);
struct Cyc_List_List*_tmp537=_tmp538->typs;
while(_tmp537 != 0){
_tmp52C=_tmp536 == 0?(*((struct _tuple11*)_tmp537->hd)).f2: Cyc_Tcutil_substitute(_tmp536,(*((struct _tuple11*)_tmp537->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp52C))return 1;
_tmp537=_tmp537->tl;}
# 3040
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp541=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52D)->f2;_LL6:
# 3010
 while(_tmp541 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp541->hd)->type))return 1;
_tmp541=_tmp541->tl;}
# 3014
return 0;default: _LLD: _LLE:
# 3042
 return 0;}_LL0:;};}
# 3049
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp542=e->r;void*_tmp543=_tmp542;struct Cyc_Absyn_Stmt*_tmp559;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp555;struct Cyc_Absyn_Exp*_tmp554;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp552;struct _fat_ptr*_tmp551;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp54F;switch(*((int*)_tmp543)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp54F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL4:
 _tmp550=_tmp54F;goto _LL6;case 20U: _LL5: _tmp550=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL6:
# 3054
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp550->topt),1)&& Cyc_Tcutil_is_noalias_path(_tmp550);case 21U: _LL7: _tmp552=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp551=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp552);case 23U: _LL9: _tmp554=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp553=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LLA: {
# 3057
void*_tmp544=Cyc_Tcutil_compress((void*)_check_null(_tmp554->topt));void*_tmp545=_tmp544;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp545)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp554);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp556=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp555=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_LLC:
# 3062
 return Cyc_Tcutil_is_noalias_path(_tmp556)&& Cyc_Tcutil_is_noalias_path(_tmp555);case 9U: _LLD: _tmp557=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LLE:
 _tmp558=_tmp557;goto _LL10;case 14U: _LLF: _tmp558=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp558);case 37U: _LL11: _tmp559=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL12:
# 3066
 while(1){
void*_tmp546=_tmp559->r;void*_tmp547=_tmp546;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Decl*_tmp54D;struct Cyc_Absyn_Stmt*_tmp54C;struct Cyc_Absyn_Stmt*_tmp54B;struct Cyc_Absyn_Stmt*_tmp54A;switch(*((int*)_tmp547)){case 2U: _LL1B: _tmp54B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp547)->f1;_tmp54A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp547)->f2;_LL1C:
 _tmp559=_tmp54A;goto _LL1A;case 12U: _LL1D: _tmp54D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp547)->f1;_tmp54C=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp547)->f2;_LL1E:
 _tmp559=_tmp54C;goto _LL1A;case 1U: _LL1F: _tmp54E=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp547)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp54E);default: _LL21: _LL22:
({void*_tmp548=0U;({struct _fat_ptr _tmp794=({const char*_tmp549="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp549,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp794,_tag_fat(_tmp548,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 3074
 return 1;}_LL0:;}
# 3091 "tcutil.cyc"
struct _tuple13 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 3093
struct _tuple13 bogus_ans=({struct _tuple13 _tmp645;_tmp645.f1=0,_tmp645.f2=Cyc_Absyn_heap_rgn_type;_tmp645;});
void*_tmp55A=e->r;void*_tmp55B=_tmp55A;struct Cyc_Absyn_Exp*_tmp594;struct Cyc_Absyn_Exp*_tmp593;struct Cyc_Absyn_Exp*_tmp592;struct Cyc_Absyn_Exp*_tmp591;struct _fat_ptr*_tmp590;int _tmp58F;struct Cyc_Absyn_Exp*_tmp58E;struct _fat_ptr*_tmp58D;int _tmp58C;void*_tmp58B;switch(*((int*)_tmp55B)){case 1U: _LL1: _tmp58B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_LL2: {
# 3097
void*_tmp55C=_tmp58B;struct Cyc_Absyn_Vardecl*_tmp564;struct Cyc_Absyn_Vardecl*_tmp563;struct Cyc_Absyn_Vardecl*_tmp562;struct Cyc_Absyn_Vardecl*_tmp561;switch(*((int*)_tmp55C)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp561=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp55C)->f1;_LL13: {
# 3101
void*_tmp55D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp55E=_tmp55D;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp55E)->tag == 4U){_LL1B: _LL1C:
# 3103
 return({struct _tuple13 _tmp639;_tmp639.f1=1,_tmp639.f2=Cyc_Absyn_heap_rgn_type;_tmp639;});}else{_LL1D: _LL1E:
 return({struct _tuple13 _tmp63A;_tmp63A.f1=(_tmp561->tq).real_const,_tmp63A.f2=Cyc_Absyn_heap_rgn_type;_tmp63A;});}_LL1A:;}case 4U: _LL14: _tmp562=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp55C)->f1;_LL15: {
# 3107
void*_tmp55F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp560=_tmp55F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp560)->tag == 4U){_LL20: _LL21:
 return({struct _tuple13 _tmp63B;_tmp63B.f1=1,_tmp63B.f2=(void*)_check_null(_tmp562->rgn);_tmp63B;});}else{_LL22: _LL23:
# 3110
 _tmp562->escapes=1;
return({struct _tuple13 _tmp63C;_tmp63C.f1=(_tmp562->tq).real_const,_tmp63C.f2=(void*)_check_null(_tmp562->rgn);_tmp63C;});}_LL1F:;}case 5U: _LL16: _tmp563=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp55C)->f1;_LL17:
# 3113
 _tmp564=_tmp563;goto _LL19;default: _LL18: _tmp564=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp55C)->f1;_LL19:
# 3115
 _tmp564->escapes=1;
return({struct _tuple13 _tmp63D;_tmp63D.f1=(_tmp564->tq).real_const,_tmp63D.f2=(void*)_check_null(_tmp564->rgn);_tmp63D;});}_LLD:;}case 21U: _LL3: _tmp58E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_tmp58D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55B)->f2;_tmp58C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55B)->f3;_LL4:
# 3120
 if(_tmp58C)return bogus_ans;{
# 3123
void*_tmp565=Cyc_Tcutil_compress((void*)_check_null(_tmp58E->topt));void*_tmp566=_tmp565;struct Cyc_Absyn_Aggrdecl*_tmp572;struct Cyc_List_List*_tmp571;switch(*((int*)_tmp566)){case 7U: _LL25: _tmp571=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp566)->f2;_LL26: {
# 3125
struct Cyc_Absyn_Aggrfield*_tmp567=Cyc_Absyn_lookup_field(_tmp571,_tmp58D);
if(_tmp567 != 0 && _tmp567->width == 0){
struct _tuple13 _tmp568=Cyc_Tcutil_addressof_props(_tmp58E);struct _tuple13 _tmp569=_tmp568;int _tmp56B;void*_tmp56A;_LL2C: _tmp56B=_tmp569.f1;_tmp56A=_tmp569.f2;_LL2D:;
return({struct _tuple13 _tmp63E;_tmp63E.f1=(_tmp567->tq).real_const || _tmp56B,_tmp63E.f2=_tmp56A;_tmp63E;});}
# 3130
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp566)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp566)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp572=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp566)->f1)->f1).KnownAggr).val;_LL28: {
# 3132
struct Cyc_Absyn_Aggrfield*_tmp56C=Cyc_Absyn_lookup_decl_field(_tmp572,_tmp58D);
if(_tmp56C != 0 && _tmp56C->width == 0){
struct _tuple13 _tmp56D=Cyc_Tcutil_addressof_props(_tmp58E);struct _tuple13 _tmp56E=_tmp56D;int _tmp570;void*_tmp56F;_LL2F: _tmp570=_tmp56E.f1;_tmp56F=_tmp56E.f2;_LL30:;
return({struct _tuple13 _tmp63F;_tmp63F.f1=(_tmp56C->tq).real_const || _tmp570,_tmp63F.f2=_tmp56F;_tmp63F;});}
# 3137
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp591=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_tmp590=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55B)->f2;_tmp58F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55B)->f3;_LL6:
# 3142
 if(_tmp58F)return bogus_ans;{
# 3145
void*_tmp573=Cyc_Tcutil_compress((void*)_check_null(_tmp591->topt));void*_tmp574=_tmp573;void*_tmp57A;void*_tmp579;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp574)->tag == 3U){_LL32: _tmp57A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp574)->f1).elt_type;_tmp579=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp574)->f1).ptr_atts).rgn;_LL33: {
# 3147
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp575=Cyc_Tcutil_compress(_tmp57A);void*_tmp576=_tmp575;struct Cyc_Absyn_Aggrdecl*_tmp578;struct Cyc_List_List*_tmp577;switch(*((int*)_tmp576)){case 7U: _LL37: _tmp577=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp576)->f2;_LL38:
# 3150
 finfo=Cyc_Absyn_lookup_field(_tmp577,_tmp590);goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp576)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp576)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp578=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp576)->f1)->f1).KnownAggr).val;_LL3A:
# 3152
 finfo=Cyc_Absyn_lookup_decl_field(_tmp578,_tmp590);goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3155
if(finfo != 0 && finfo->width == 0)
return({struct _tuple13 _tmp640;_tmp640.f1=(finfo->tq).real_const,_tmp640.f2=_tmp579;_tmp640;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp592=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_LL8: {
# 3162
void*_tmp57B=Cyc_Tcutil_compress((void*)_check_null(_tmp592->topt));void*_tmp57C=_tmp57B;struct Cyc_Absyn_Tqual _tmp57E;void*_tmp57D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->tag == 3U){_LL3E: _tmp57E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).elt_tq;_tmp57D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).rgn;_LL3F:
# 3164
 return({struct _tuple13 _tmp641;_tmp641.f1=_tmp57E.real_const,_tmp641.f2=_tmp57D;_tmp641;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp594=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_tmp593=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55B)->f2;_LLA: {
# 3170
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp594->topt));
void*_tmp57F=t;struct Cyc_Absyn_Tqual _tmp588;struct Cyc_Absyn_Tqual _tmp587;void*_tmp586;struct Cyc_List_List*_tmp585;switch(*((int*)_tmp57F)){case 6U: _LL43: _tmp585=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57F)->f1;_LL44: {
# 3174
struct _tuple12 _tmp580=Cyc_Evexp_eval_const_uint_exp(_tmp593);struct _tuple12 _tmp581=_tmp580;unsigned _tmp584;int _tmp583;_LL4C: _tmp584=_tmp581.f1;_tmp583=_tmp581.f2;_LL4D:;
if(!_tmp583)
return bogus_ans;{
struct _tuple11*_tmp582=Cyc_Absyn_lookup_tuple_field(_tmp585,(int)_tmp584);
if(_tmp582 != 0)
return({struct _tuple13 _tmp642;_tmp642.f1=((*_tmp582).f1).real_const,({void*_tmp795=(Cyc_Tcutil_addressof_props(_tmp594)).f2;_tmp642.f2=_tmp795;});_tmp642;});
return bogus_ans;};}case 3U: _LL45: _tmp587=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57F)->f1).elt_tq;_tmp586=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57F)->f1).ptr_atts).rgn;_LL46:
# 3182
 return({struct _tuple13 _tmp643;_tmp643.f1=_tmp587.real_const,_tmp643.f2=_tmp586;_tmp643;});case 4U: _LL47: _tmp588=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57F)->f1).tq;_LL48:
# 3188
 return({struct _tuple13 _tmp644;_tmp644.f1=_tmp588.real_const,({void*_tmp796=(Cyc_Tcutil_addressof_props(_tmp594)).f2;_tmp644.f2=_tmp796;});_tmp644;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3192
({void*_tmp589=0U;({unsigned _tmp798=e->loc;struct _fat_ptr _tmp797=({const char*_tmp58A="unary & applied to non-lvalue";_tag_fat(_tmp58A,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp798,_tmp797,_tag_fat(_tmp589,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3199
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp595=({void*_tmp799=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp799,b);});
if(_tmp595 == 0)return;{
struct Cyc_Absyn_Exp*_tmp596=_tmp595;
struct _tuple12 _tmp597=Cyc_Evexp_eval_const_uint_exp(_tmp596);struct _tuple12 _tmp598=_tmp597;unsigned _tmp5A2;int _tmp5A1;_LL1: _tmp5A2=_tmp598.f1;_tmp5A1=_tmp598.f2;_LL2:;
if(_tmp5A1 && _tmp5A2 <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp59B=({struct Cyc_Int_pa_PrintArg_struct _tmp647;_tmp647.tag=1U,_tmp647.f1=(unsigned long)((int)_tmp5A2);_tmp647;});struct Cyc_Int_pa_PrintArg_struct _tmp59C=({struct Cyc_Int_pa_PrintArg_struct _tmp646;_tmp646.tag=1U,_tmp646.f1=(unsigned long)((int)i);_tmp646;});void*_tmp599[2U];_tmp599[0]=& _tmp59B,_tmp599[1]=& _tmp59C;({unsigned _tmp79B=loc;struct _fat_ptr _tmp79A=({const char*_tmp59A="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp59A,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp79B,_tmp79A,_tag_fat(_tmp599,sizeof(void*),2U));});});else{
# 3208
({struct Cyc_Int_pa_PrintArg_struct _tmp59F=({struct Cyc_Int_pa_PrintArg_struct _tmp649;_tmp649.tag=1U,_tmp649.f1=(unsigned long)((int)_tmp5A2);_tmp649;});struct Cyc_Int_pa_PrintArg_struct _tmp5A0=({struct Cyc_Int_pa_PrintArg_struct _tmp648;_tmp648.tag=1U,_tmp648.f1=(unsigned long)((int)i);_tmp648;});void*_tmp59D[2U];_tmp59D[0]=& _tmp59F,_tmp59D[1]=& _tmp5A0;({unsigned _tmp79D=loc;struct _fat_ptr _tmp79C=({const char*_tmp59E="dereference is out of bounds: %d <= %d";_tag_fat(_tmp59E,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp79D,_tmp79C,_tag_fat(_tmp59D,sizeof(void*),2U));});});}}
return;};}
# 3212
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3221
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp5A3=e->r;void*_tmp5A4=_tmp5A3;struct Cyc_List_List*_tmp5C3;struct Cyc_List_List*_tmp5C2;enum Cyc_Absyn_Primop _tmp5C1;struct Cyc_List_List*_tmp5C0;struct Cyc_List_List*_tmp5BF;struct Cyc_List_List*_tmp5BE;struct Cyc_List_List*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5B9;void*_tmp5B8;struct Cyc_Absyn_Exp*_tmp5B7;void*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;void*_tmp5AC;switch(*((int*)_tmp5A4)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5AC=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL10: {
# 3233
void*_tmp5A5=_tmp5AC;struct Cyc_Absyn_Vardecl*_tmp5AB;struct Cyc_Absyn_Vardecl*_tmp5AA;switch(*((int*)_tmp5A5)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp5AA=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5A5)->f1;_LL37: {
# 3236
void*_tmp5A6=Cyc_Tcutil_compress(_tmp5AA->type);void*_tmp5A7=_tmp5A6;switch(*((int*)_tmp5A7)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp5AB=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5A5)->f1;_LL39:
# 3243
 if((int)_tmp5AB->sc == (int)Cyc_Absyn_Static){
void*_tmp5A8=Cyc_Tcutil_compress(_tmp5AB->type);void*_tmp5A9=_tmp5A8;switch(*((int*)_tmp5A9)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3250
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp5AF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5AE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_tmp5AD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A4)->f3;_LL12:
# 3256
 return(Cyc_Tcutil_cnst_exp(0,_tmp5AF)&&
 Cyc_Tcutil_cnst_exp(0,_tmp5AE))&&
 Cyc_Tcutil_cnst_exp(0,_tmp5AD);case 9U: _LL13: _tmp5B1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5B0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL14:
# 3260
 return Cyc_Tcutil_cnst_exp(0,_tmp5B1)&& Cyc_Tcutil_cnst_exp(0,_tmp5B0);case 41U: _LL15: _tmp5B2=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL16:
 _tmp5B3=_tmp5B2;goto _LL18;case 12U: _LL17: _tmp5B3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL18:
 _tmp5B4=_tmp5B3;goto _LL1A;case 13U: _LL19: _tmp5B4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL1A:
# 3264
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5B4);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp5B6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5B5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL1C:
# 3266
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5B5);}else{_LL1D: _tmp5B8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5B7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL1E:
# 3269
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5B7);}case 15U: _LL1F: _tmp5B9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL20:
# 3271
 return Cyc_Tcutil_cnst_exp(1,_tmp5B9);case 27U: _LL21: _tmp5BB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_tmp5BA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A4)->f3;_LL22:
# 3273
 return Cyc_Tcutil_cnst_exp(0,_tmp5BB)&& Cyc_Tcutil_cnst_exp(0,_tmp5BA);case 28U: _LL23: _tmp5BC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL24:
# 3275
 return Cyc_Tcutil_cnst_exp(0,_tmp5BC);case 26U: _LL25: _tmp5BD=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL26:
 _tmp5BE=_tmp5BD;goto _LL28;case 30U: _LL27: _tmp5BE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL28:
 _tmp5BF=_tmp5BE;goto _LL2A;case 29U: _LL29: _tmp5BF=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5A4)->f3;_LL2A:
# 3279
 for(0;_tmp5BF != 0;_tmp5BF=_tmp5BF->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple15*)_tmp5BF->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp5C1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5C0=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL2C:
# 3284
 _tmp5C2=_tmp5C0;goto _LL2E;case 24U: _LL2D: _tmp5C2=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL2E:
 _tmp5C3=_tmp5C2;goto _LL30;case 31U: _LL2F: _tmp5C3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL30:
# 3287
 for(0;_tmp5C3 != 0;_tmp5C3=_tmp5C3->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp5C3->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3294
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3298
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5C4=Cyc_Tcutil_compress(t);void*_tmp5C5=_tmp5C4;struct Cyc_List_List*_tmp5D5;struct Cyc_List_List*_tmp5D4;void*_tmp5D3;void*_tmp5D2;void*_tmp5D1;struct Cyc_List_List*_tmp5D0;switch(*((int*)_tmp5C5)){case 0U: _LL1: _tmp5D1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C5)->f1;_tmp5D0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C5)->f2;_LL2: {
# 3302
void*_tmp5C6=_tmp5D1;union Cyc_Absyn_AggrInfo _tmp5CF;struct Cyc_List_List*_tmp5CE;struct Cyc_Absyn_Enumdecl*_tmp5CD;switch(*((int*)_tmp5C6)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5CD=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5C6)->f2;_LL15:
# 3307
 if(_tmp5CD == 0 || _tmp5CD->fields == 0)
return 0;
_tmp5CE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5CD->fields))->v;goto _LL17;case 16U: _LL16: _tmp5CE=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5C6)->f1;_LL17:
# 3311
{struct Cyc_List_List*_tmp5C7=_tmp5CE;for(0;_tmp5C7 != 0;_tmp5C7=_tmp5C7->tl){
if(((struct Cyc_Absyn_Enumfield*)_tmp5C7->hd)->tag == 0)
return _tmp5C7 == _tmp5CE;{
struct _tuple12 _tmp5C8=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_tmp5C7->hd)->tag));struct _tuple12 _tmp5C9=_tmp5C8;unsigned _tmp5CB;int _tmp5CA;_LL1D: _tmp5CB=_tmp5C9.f1;_tmp5CA=_tmp5C9.f2;_LL1E:;
if(_tmp5CA && _tmp5CB == (unsigned)0)
return 1;};}}
# 3318
return 0;case 20U: _LL18: _tmp5CF=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5C6)->f1;_LL19: {
# 3321
struct Cyc_Absyn_Aggrdecl*_tmp5CC=Cyc_Absyn_get_known_aggrdecl(_tmp5CF);
if(_tmp5CC->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5CC->impl))->exist_vars != 0)return 0;
if((int)_tmp5CC->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5CC->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(_tmp5CC->tvs,_tmp5D0,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5CC->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5)->f1).ptr_atts).nullable;_LL4:
# 3329
 return Cyc_Tcutil_force_type2bool(1,_tmp5D2);case 4U: _LL5: _tmp5D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).elt_type;_LL6:
 return Cyc_Tcutil_zeroable_type(_tmp5D3);case 6U: _LL7: _tmp5D4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5C5)->f1;_LL8:
# 3332
 for(0;_tmp5D4 != 0;_tmp5D4=_tmp5D4->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple11*)_tmp5D4->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp5D5=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5C5)->f2;_LLA:
 return Cyc_Tcutil_fields_zeroable(0,0,_tmp5D5);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3339
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3341
struct _RegionHandle _tmp5D6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D6;_push_region(rgn);
{struct Cyc_List_List*_tmp5D7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,_tmp5D7,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5D8=0;_npop_handler(0U);return _tmp5D8;}}{
# 3349
int _tmp5D9=1;_npop_handler(0U);return _tmp5D9;};}
# 3342
;_pop_region(rgn);}
# 3353
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5DA=t;struct Cyc_Absyn_Typedefdecl*_tmp5DF;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5DA)->tag == 8U){_LL1: _tmp5DF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5DA)->f3;_LL2:
# 3356
 if(_tmp5DF != 0){
struct Cyc_Absyn_Tqual _tmp5DB=_tmp5DF->tq;
if(((_tmp5DB.print_const || _tmp5DB.q_volatile)|| _tmp5DB.q_restrict)|| _tmp5DB.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp5DE=({struct Cyc_String_pa_PrintArg_struct _tmp64A;_tmp64A.tag=0U,({struct _fat_ptr _tmp79E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp64A.f1=_tmp79E;});_tmp64A;});void*_tmp5DC[1U];_tmp5DC[0]=& _tmp5DE;({unsigned _tmp7A0=loc;struct _fat_ptr _tmp79F=({const char*_tmp5DD="qualifier within typedef type %s is ignored";_tag_fat(_tmp5DD,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp7A0,_tmp79F,_tag_fat(_tmp5DC,sizeof(void*),1U));});});}
# 3361
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3368
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp5E0=Cyc_Tcutil_compress(t);void*_tmp5E1=_tmp5E0;struct Cyc_List_List**_tmp5E7;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E1)->tag == 5U){_LL1: _tmp5E7=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E1)->f1).attributes;_LL2: {
# 3371
struct Cyc_List_List*_tmp5E2=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*_tmp5E7,(void*)atts->hd))
({struct Cyc_List_List*_tmp7A1=({struct Cyc_List_List*_tmp5E3=_cycalloc(sizeof(*_tmp5E3));_tmp5E3->hd=(void*)atts->hd,_tmp5E3->tl=*_tmp5E7;_tmp5E3;});*_tmp5E7=_tmp7A1;});}else{
# 3378
_tmp5E2=({struct Cyc_List_List*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->hd=(void*)atts->hd,_tmp5E4->tl=_tmp5E2;_tmp5E4;});}}
return _tmp5E2;}}else{_LL3: _LL4:
({void*_tmp5E5=0U;({struct _fat_ptr _tmp7A2=({const char*_tmp5E6="transfer_fn_type_atts";_tag_fat(_tmp5E6,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A2,_tag_fat(_tmp5E5,sizeof(void*),0U));});});}_LL0:;}
# 3385
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5E8=Cyc_Tcutil_compress(t);void*_tmp5E9=_tmp5E8;struct Cyc_Absyn_Exp*_tmp5EB;struct Cyc_Absyn_PtrInfo _tmp5EA;switch(*((int*)_tmp5E9)){case 3U: _LL1: _tmp5EA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E9)->f1;_LL2:
 return({void*_tmp7A3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7A3,(_tmp5EA.ptr_atts).bounds);});case 4U: _LL3: _tmp5EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E9)->f1).num_elts;_LL4:
 return _tmp5EB;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3395
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5EC=b;struct Cyc_Absyn_Vardecl*_tmp5F0;struct Cyc_Absyn_Vardecl*_tmp5EF;struct Cyc_Absyn_Vardecl*_tmp5EE;struct Cyc_Absyn_Vardecl*_tmp5ED;switch(*((int*)_tmp5EC)){case 5U: _LL1: _tmp5ED=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5EC)->f1;_LL2:
 _tmp5EE=_tmp5ED;goto _LL4;case 4U: _LL3: _tmp5EE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5EC)->f1;_LL4:
 _tmp5EF=_tmp5EE;goto _LL6;case 3U: _LL5: _tmp5EF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5EC)->f1;_LL6:
 _tmp5F0=_tmp5EF;goto _LL8;case 1U: _LL7: _tmp5F0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5EC)->f1;_LL8:
# 3401
 if(!_tmp5F0->escapes)
return _tmp5F0;
return 0;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3409
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5F1=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmp5F1=({struct Cyc_List_List*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->hd=*((void**)_check_null((void**)x->hd)),_tmp5F2->tl=_tmp5F1;_tmp5F2;});}}
return _tmp5F1;}
# 3416
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5F3=Cyc_Tcutil_compress(t);void*_tmp5F4=_tmp5F3;void*_tmp5FE;struct Cyc_Absyn_Tqual _tmp5FD;struct Cyc_Absyn_Exp*_tmp5FC;void*_tmp5FB;unsigned _tmp5FA;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->tag == 4U){_LL1: _tmp5FE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).elt_type;_tmp5FD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).tq;_tmp5FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).num_elts;_tmp5FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).zero_term;_tmp5FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).zt_loc;_LL2: {
# 3419
void*b;
if(_tmp5FC == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 3423
if(convert_tag){
if(_tmp5FC->topt == 0)
({void*_tmp5F5=0U;({struct _fat_ptr _tmp7A4=({const char*_tmp5F6="cannot convert tag without type!";_tag_fat(_tmp5F6,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A4,_tag_fat(_tmp5F5,sizeof(void*),0U));});});{
void*_tmp5F7=Cyc_Tcutil_compress((void*)_check_null(_tmp5FC->topt));void*_tmp5F8=_tmp5F7;void*_tmp5F9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F8)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F8)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F8)->f2 != 0){_LL6: _tmp5F9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F8)->f2)->hd;_LL7:
# 3429
 b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(_tmp5F9,0U));
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 3432
 b=Cyc_Tcutil_is_const_exp(_tmp5FC)?Cyc_Absyn_thin_bounds_exp(_tmp5FC): Cyc_Absyn_fat_bound_type;}_LL5:;};}else{
# 3436
b=Cyc_Absyn_thin_bounds_exp(_tmp5FC);}}
# 3438
return Cyc_Absyn_atb_type(_tmp5FE,rgn,_tmp5FD,b,_tmp5FB);}}else{_LL3: _LL4:
 return t;}_LL0:;}
