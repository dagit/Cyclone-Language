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
struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 37 "position.h"
struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
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
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 110
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 146
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
# 31 "tcutil.h"
void*Cyc_Tcutil_impos(struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_terr(unsigned,struct _fat_ptr,struct _fat_ptr);
void Cyc_Tcutil_warn(unsigned,struct _fat_ptr,struct _fat_ptr);
# 38
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 60
int Cyc_Tcutil_is_noreturn_fn_type(void*);
# 73
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 78
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 82
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type);
# 85
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 88
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 90
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 95
void*Cyc_Tcutil_copy_type(void*);
# 98
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 101
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 106
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 115
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 118
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned,void*,void*);
# 120
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned,void*,void*);
# 122
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 128
extern int Cyc_Tcutil_warn_alias_coerce;
# 131
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
# 181
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_Absyn_Exp*);
# 186
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 189
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 191
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 197
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 200
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 204
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 228
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 241
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 244
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 249
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 254
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple13{int f1;void*f2;};
# 258
struct _tuple13 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*e);
# 261
void*Cyc_Tcutil_normalize_effect(void*e);
# 264
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 266
int Cyc_Tcutil_new_tvar_id();
# 268
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 271
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 279
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 286
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 304
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 307
int Cyc_Tcutil_zeroable_type(void*);
# 311
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 319
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 323
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 328
int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*,struct Cyc_List_List*);
# 330
int Cyc_Tcutil_tycon_cmp(void*,void*);
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*,void*);
# 39 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_tmp1=_tmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 45
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 51
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_tmp5=_tmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 57
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_tmp7=_tmp6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 64
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_tmp9=_tmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 71
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 78
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_tmpD=_tmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 85
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_tmpF=_tmpE;void*_tmp11;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp11=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {
# 88
void*_tmp10=_tmp11;switch(*((int*)_tmp10)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 95
 return 0;}_LL0:;}
# 98
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_tmp13=_tmp12;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
 return 1;}else{goto _LL5;}case 2U: _LL3: _LL4:
 return 1;default: goto _LL5;}else{_LL5: _LL6:
 return 0;}_LL0:;}
# 105
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 108
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 111
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp14=Cyc_Tcutil_compress(t);void*_tmp15=_tmp14;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 117
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 125
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp16=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp17=_tmp16;void*_tmp18;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17)->tag == 1U){_LL1: _LL2:
 return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp18=_tmp17;_LL4:
 return Cyc_Tcutil_is_integral_type(_tmp18);}_LL0:;}
# 133
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp19=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp1A=_tmp19;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 143
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1B=Cyc_Tcutil_compress(t);void*_tmp1C=_tmp1B;void*_tmp1D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->tag == 3U){_LL1: _tmp1D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->f1).ptr_atts).zero_term;_LL2:
# 146
 return Cyc_Tcutil_force_type2bool(0,_tmp1D);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 152
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1E=Cyc_Tcutil_compress(t);void*_tmp1F=_tmp1E;void*_tmp20;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->tag == 3U){_LL1: _tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1).ptr_atts).nullable;_LL2:
# 155
 return Cyc_Tcutil_force_type2bool(0,_tmp20);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 161
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp21=Cyc_Tcutil_compress(t);void*_tmp22=_tmp21;void*_tmp23;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->tag == 3U){_LL1: _tmp23=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->f1).ptr_atts).bounds;_LL2:
# 164
 return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,_tmp23);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 171
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp24=Cyc_Tcutil_compress(t);void*_tmp25=_tmp24;void*_tmp27;void*_tmp26;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->tag == 3U){_LL1: _tmp27=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).elt_type;_tmp26=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).ptr_atts).bounds;_LL2:
# 174
 if(Cyc_Unify_unify(_tmp26,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=_tmp27;
return 1;}else{
# 178
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 185
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp28=Cyc_Tcutil_compress(t);void*_tmp29=_tmp28;void*_tmp2B;void*_tmp2A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->tag == 3U){_LL1: _tmp2B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).elt_type;_tmp2A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).ptr_atts).zero_term;_LL2:
# 188
*elt_type_dest=_tmp2B;
return Cyc_Absyn_type2bool(0,_tmp2A);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 197
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 199
void*_tmp2C=Cyc_Tcutil_compress(t);void*_tmp2D=_tmp2C;void*_tmp36;struct Cyc_Absyn_Tqual _tmp35;struct Cyc_Absyn_Exp*_tmp34;void*_tmp33;void*_tmp32;void*_tmp31;void*_tmp30;switch(*((int*)_tmp2D)){case 3U: _LL1: _tmp32=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp31=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).bounds;_tmp30=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).zero_term;_LL2:
# 201
 if(Cyc_Absyn_type2bool(0,_tmp30)){
*ptr_type=t;
*elt_type=_tmp32;
{void*_tmp2E=Cyc_Tcutil_compress(_tmp31);void*_tmp2F=_tmp2E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 208
return 1;}else{
return 0;}case 4U: _LL3: _tmp36=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp35=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).tq;_tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).zero_term;_LL4:
# 211
 if(Cyc_Absyn_type2bool(0,_tmp33)){
*elt_type=_tmp36;
*is_fat=0;
({void*_tmp658=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp658;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 224
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 230
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp659=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp659,b);});
if(_tmp37 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp38=_tmp37;
struct _tuple12 _tmp39=Cyc_Evexp_eval_const_uint_exp(_tmp38);struct _tuple12 _tmp3A=_tmp39;unsigned _tmp3C;int _tmp3B;_LL1: _tmp3C=_tmp3A.f1;_tmp3B=_tmp3A.f2;_LL2:;
return _tmp3B && _tmp3C == (unsigned)1;};}
# 239
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3D=Cyc_Tcutil_compress(t);void*_tmp3E=_tmp3D;struct Cyc_List_List*_tmp4A;struct Cyc_List_List*_tmp49;void*_tmp48;void*_tmp47;void*_tmp46;struct Cyc_List_List*_tmp45;switch(*((int*)_tmp3E)){case 0U: _LL1: _tmp46=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f2;_LL2: {
# 242
void*_tmp3F=_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return 1;case 15U: _LL12: _LL13:
 goto _LL15;case 16U: _LL14: _LL15:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
 return 0;}else{_LL18: _tmp44=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).KnownAggr).val;_LL19:
# 250
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
# 257
;_pop_region(rgn);};};}default: _LL1A: _LL1B:
# 261
 return 0;}_LLB:;}case 4U: _LL3: _tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).elt_type;_tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zero_term;_LL4:
# 266
 return !Cyc_Absyn_type2bool(0,_tmp47)&& Cyc_Tcutil_is_bits_only_type(_tmp48);case 6U: _LL5: _tmp49=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E)->f1;_LL6:
# 268
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple11*)_tmp49->hd)).f2))return 0;}
return 1;case 7U: _LL7: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E)->f2;_LL8:
# 272
 for(0;_tmp4A != 0;_tmp4A=_tmp4A->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)_tmp4A->hd)->type))return 0;}
return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 281
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_List_List*_tmp50;void*_tmp4F;switch(*((int*)_tmp4C)){case 3U: _LL1: _tmp4F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_noreturn_fn_type(_tmp4F);case 5U: _LL3: _tmp50=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_LL4:
# 285
 for(0;_tmp50 != 0;_tmp50=_tmp50->tl){
void*_tmp4D=(void*)_tmp50->hd;void*_tmp4E=_tmp4D;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp4E)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 290
return 0;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 297
int Cyc_Tcutil_warn_region_coerce=0;
# 299
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 301
Cyc_Warn_verr(loc,fmt,ap);}
# 303
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 305
Cyc_Warn_vimpos(fmt,ap);}
# 307
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 309
Cyc_Warn_vwarn(sg,fmt,ap);}
# 313
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 318
void*Cyc_Tcutil_compress(void*t){
void*_tmp51=t;void*_tmp5A;struct Cyc_Absyn_Exp*_tmp59;struct Cyc_Absyn_Exp*_tmp58;void**_tmp57;void**_tmp56;switch(*((int*)_tmp51)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp56=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51)->f2;_LL8: {
# 329
void*ta=(void*)_check_null(*_tmp56);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp56=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51)->f4 == 0){_LL3: _LL4:
# 321
 return t;}else{_LL5: _tmp57=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51)->f4;_LL6: {
# 323
void*ta=(void*)_check_null(*_tmp57);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp57=t2;
return t2;}}case 9U: _LL9: _tmp58=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp51)->f1;_LLA:
# 335
 Cyc_Evexp_eval_const_uint_exp(_tmp58);{
void*_tmp52=_tmp58->r;void*_tmp53=_tmp52;void*_tmp54;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp53)->tag == 39U){_LL12: _tmp54=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp53)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp54);}else{_LL14: _LL15:
 return t;}_LL11:;};case 11U: _LLB: _tmp59=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp51)->f1;_LLC: {
# 341
void*_tmp55=_tmp59->topt;
if(_tmp55 != 0)return _tmp55;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp51)->f2 != 0){_LLD: _tmp5A=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp51)->f2;_LLE:
 return Cyc_Tcutil_compress(_tmp5A);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 353
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 357
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp5B=Cyc_Absyn_compress_kb(kb);void*_tmp5C=_tmp5B;struct Cyc_Absyn_Kind*_tmp5F;switch(*((int*)_tmp5C)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp5D=_cycalloc(sizeof(*_tmp5D));_tmp5D->tag=1U,_tmp5D->f1=0;_tmp5D;});case 2U: _LL3: _tmp5F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=2U,_tmp5E->f1=0,_tmp5E->f2=_tmp5F;_tmp5E;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 364
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 366
return({struct Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=tv->name,_tmp60->identity=- 1,({void*_tmp65A=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp60->kind=_tmp65A;});_tmp60;});}
# 368
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 370
struct _tuple9*_tmp61=arg;struct _fat_ptr*_tmp65;struct Cyc_Absyn_Tqual _tmp64;void*_tmp63;_LL1: _tmp65=_tmp61->f1;_tmp64=_tmp61->f2;_tmp63=_tmp61->f3;_LL2:;
return({struct _tuple9*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=_tmp65,_tmp62->f2=_tmp64,({void*_tmp65B=Cyc_Tcutil_copy_type(_tmp63);_tmp62->f3=_tmp65B;});_tmp62;});}
# 373
static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
return({struct _tuple11*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=(*arg).f1,({void*_tmp65C=Cyc_Tcutil_copy_type((*arg).f2);_tmp66->f2=_tmp65C;});_tmp66;});}
# 376
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}
# 380
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->name=f->name,_tmp67->tq=f->tq,({void*_tmp65E=Cyc_Tcutil_copy_type(f->type);_tmp67->type=_tmp65E;}),_tmp67->width=f->width,_tmp67->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp65D=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp67->requires_clause=_tmp65D;});_tmp67;});}struct _tuple14{void*f1;void*f2;};
# 384
static struct _tuple14*Cyc_Tcutil_copy_rgncmp(struct _tuple14*x){
struct _tuple14*_tmp68=x;void*_tmp6B;void*_tmp6A;_LL1: _tmp6B=_tmp68->f1;_tmp6A=_tmp68->f2;_LL2:;
return({struct _tuple14*_tmp69=_cycalloc(sizeof(*_tmp69));({void*_tmp660=Cyc_Tcutil_copy_type(_tmp6B);_tmp69->f1=_tmp660;}),({void*_tmp65F=Cyc_Tcutil_copy_type(_tmp6A);_tmp69->f2=_tmp65F;});_tmp69;});}
# 388
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 392
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6C=Cyc_Tcutil_compress(t);void*_tmp6D=_tmp6C;struct Cyc_Absyn_Datatypedecl*_tmpB1;struct Cyc_Absyn_Enumdecl*_tmpB0;struct Cyc_Absyn_Aggrdecl*_tmpAF;struct _tuple1*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Typedefdecl*_tmpAC;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAA;enum Cyc_Absyn_AggrKind _tmpA9;struct Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;struct Cyc_List_List*_tmpA2;int _tmpA1;struct Cyc_Absyn_VarargInfo*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_List_List*_tmp9A;void*_tmp99;struct Cyc_Absyn_Tqual _tmp98;struct Cyc_Absyn_Exp*_tmp97;void*_tmp96;unsigned _tmp95;void*_tmp94;struct Cyc_Absyn_Tqual _tmp93;void*_tmp92;void*_tmp91;void*_tmp90;void*_tmp8F;struct Cyc_Absyn_PtrLoc*_tmp8E;struct Cyc_Absyn_Tvar*_tmp8D;void*_tmp8C;struct Cyc_List_List*_tmp8B;void*_tmp8A;switch(*((int*)_tmp6D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2 == 0){_LL1: _tmp8A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_LL2:
 return t;}else{_LL3: _tmp8C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_tmp8B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=0U,_tmp6E->f1=_tmp8C,({struct Cyc_List_List*_tmp661=Cyc_Tcutil_copy_types(_tmp8B);_tmp6E->f2=_tmp661;});_tmp6E;});}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp8D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->f1;_LL8:
 return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(_tmp8D));case 3U: _LL9: _tmp94=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp93=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_tq;_tmp92=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).rgn;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).nullable;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).bounds;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).zero_term;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).ptrloc;_LLA: {
# 399
void*_tmp6F=Cyc_Tcutil_copy_type(_tmp94);
void*_tmp70=Cyc_Tcutil_copy_type(_tmp92);
void*_tmp71=Cyc_Tcutil_copy_type(_tmp91);
struct Cyc_Absyn_Tqual _tmp72=_tmp93;
# 404
void*_tmp73=Cyc_Tcutil_copy_type(_tmp90);
void*_tmp74=Cyc_Tcutil_copy_type(_tmp8F);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp75=_cycalloc(sizeof(*_tmp75));_tmp75->tag=3U,(_tmp75->f1).elt_type=_tmp6F,(_tmp75->f1).elt_tq=_tmp72,((_tmp75->f1).ptr_atts).rgn=_tmp70,((_tmp75->f1).ptr_atts).nullable=_tmp71,((_tmp75->f1).ptr_atts).bounds=_tmp73,((_tmp75->f1).ptr_atts).zero_term=_tmp74,((_tmp75->f1).ptr_atts).ptrloc=_tmp8E;_tmp75;});}case 4U: _LLB: _tmp99=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp98=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).tq;_tmp97=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).num_elts;_tmp96=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).zero_term;_tmp95=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).zt_loc;_LLC:
# 408
 return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=4U,({void*_tmp664=Cyc_Tcutil_copy_type(_tmp99);(_tmp76->f1).elt_type=_tmp664;}),(_tmp76->f1).tq=_tmp98,({struct Cyc_Absyn_Exp*_tmp663=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp97);(_tmp76->f1).num_elts=_tmp663;}),({
void*_tmp662=Cyc_Tcutil_copy_type(_tmp96);(_tmp76->f1).zero_term=_tmp662;}),(_tmp76->f1).zt_loc=_tmp95;_tmp76;});case 5U: _LLD: _tmpA6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).tvars;_tmpA5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).effect;_tmpA4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_tqual;_tmpA3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_type;_tmpA2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).args;_tmpA1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).c_varargs;_tmpA0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).cyc_varargs;_tmp9F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).rgn_po;_tmp9E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).attributes;_tmp9D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_clause;_tmp9C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_relns;_tmp9B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_clause;_tmp9A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_relns;_LLE: {
# 411
struct Cyc_List_List*_tmp77=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpA6);
void*effopt2=_tmpA5 == 0?0: Cyc_Tcutil_copy_type(_tmpA5);
void*_tmp78=Cyc_Tcutil_copy_type(_tmpA3);
struct Cyc_List_List*_tmp79=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpA2);
int _tmp7A=_tmpA1;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpA0 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpA0;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->name=cv->name,_tmp7B->tq=cv->tq,({void*_tmp665=Cyc_Tcutil_copy_type(cv->type);_tmp7B->type=_tmp665;}),_tmp7B->inject=cv->inject;_tmp7B;});}{
# 422
struct Cyc_List_List*_tmp7C=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9F);
struct Cyc_List_List*_tmp7D=_tmp9E;
struct Cyc_Absyn_Exp*_tmp7E=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp9D);
struct Cyc_List_List*_tmp7F=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp9C);
struct Cyc_Absyn_Exp*_tmp80=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp9B);
struct Cyc_List_List*_tmp81=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp9A);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=5U,(_tmp82->f1).tvars=_tmp77,(_tmp82->f1).effect=effopt2,(_tmp82->f1).ret_tqual=_tmpA4,(_tmp82->f1).ret_type=_tmp78,(_tmp82->f1).args=_tmp79,(_tmp82->f1).c_varargs=_tmp7A,(_tmp82->f1).cyc_varargs=cyc_varargs2,(_tmp82->f1).rgn_po=_tmp7C,(_tmp82->f1).attributes=_tmp7D,(_tmp82->f1).requires_clause=_tmp7E,(_tmp82->f1).requires_relns=_tmp7F,(_tmp82->f1).ensures_clause=_tmp80,(_tmp82->f1).ensures_relns=_tmp81;_tmp82;});};}case 6U: _LLF: _tmpA7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D)->f1;_LL10:
# 431
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=6U,({struct Cyc_List_List*_tmp666=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA7);_tmp83->f1=_tmp666;});_tmp83;});case 7U: _LL11: _tmpA9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f1;_tmpA8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=7U,_tmp84->f1=_tmpA9,({struct Cyc_List_List*_tmp667=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpA8);_tmp84->f2=_tmp667;});_tmp84;});case 9U: _LL13: _tmpAA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL14:
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=9U,_tmp85->f1=_tmpAA;_tmp85;});case 11U: _LL15: _tmpAB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6D)->f1;_LL16:
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=11U,_tmp86->f1=_tmpAB;_tmp86;});case 8U: _LL17: _tmpAE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f1;_tmpAD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f2;_tmpAC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f3;_LL18:
# 436
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=8U,_tmp87->f1=_tmpAE,({struct Cyc_List_List*_tmp668=Cyc_Tcutil_copy_types(_tmpAD);_tmp87->f2=_tmp668;}),_tmp87->f3=_tmpAC,_tmp87->f4=0;_tmp87;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)){case 0U: _LL19: _tmpAF=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1A: {
# 439
struct Cyc_List_List*_tmp88=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpAF->tvs);
return({union Cyc_Absyn_AggrInfo _tmp669=Cyc_Absyn_UnknownAggr(_tmpAF->kind,_tmpAF->name,0);Cyc_Absyn_aggr_type(_tmp669,_tmp88);});}case 1U: _LL1B: _tmpB0=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1C:
# 442
 return Cyc_Absyn_enum_type(_tmpB0->name,0);default: _LL1D: _tmpB1=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1E: {
# 444
struct Cyc_List_List*_tmp89=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB1->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmp66A=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp609;_tmp609.name=_tmpB1->name,_tmp609.is_extensible=0;_tmp609;}));Cyc_Absyn_datatype_type(_tmp66A,_tmp89);});}}}_LL0:;}
# 451
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB2=d;struct _fat_ptr*_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->tag == 0U){_LL1: _tmpB4=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->tag=0U,({struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpB4);_tmpB3->f1=_tmp66B;});_tmpB3;});}else{_LL3: _tmpB5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB2)->f1;_LL4:
 return d;}_LL0:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 457
static struct _tuple15*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple15*e){
# 459
return({struct _tuple15*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_List_List*_tmp66D=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB6->f1=_tmp66D;}),({struct Cyc_Absyn_Exp*_tmp66C=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB6->f2=_tmp66C;});_tmpB6;});}
# 462
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB7=preserve_types;
{void*_tmpB8=e->r;void*_tmpB9=_tmpB8;int _tmp135;struct _fat_ptr _tmp134;void*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct _fat_ptr*_tmp131;struct Cyc_Core_Opt*_tmp130;struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;int _tmp12C;struct Cyc_Absyn_Exp*_tmp12B;void**_tmp12A;struct Cyc_Absyn_Exp*_tmp129;int _tmp128;int _tmp127;void*_tmp126;struct Cyc_Absyn_Enumfield*_tmp125;struct Cyc_Absyn_Enumdecl*_tmp124;struct Cyc_Absyn_Enumfield*_tmp123;struct _fat_ptr*_tmp122;struct Cyc_Absyn_Tqual _tmp121;void*_tmp120;struct Cyc_List_List*_tmp11F;void*_tmp11E;struct Cyc_List_List*_tmp11D;struct _tuple1*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_Absyn_Aggrdecl*_tmp119;struct Cyc_Absyn_Exp*_tmp118;void*_tmp117;int _tmp116;struct Cyc_Absyn_Vardecl*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;int _tmp112;struct Cyc_List_List*_tmp111;struct Cyc_Absyn_Datatypedecl*_tmp110;struct Cyc_Absyn_Datatypefield*_tmp10F;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct _fat_ptr*_tmp10A;int _tmp109;int _tmp108;struct Cyc_Absyn_Exp*_tmp107;struct _fat_ptr*_tmp106;int _tmp105;int _tmp104;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;void*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;void*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;void*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;int _tmpF7;enum Cyc_Absyn_Coercion _tmpF6;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;int _tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_VarargCallInfo*_tmpEE;int _tmpED;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_List_List*_tmpEB;int _tmpEA;struct Cyc_List_List*_tmpE9;struct _fat_ptr*_tmpE8;struct Cyc_Absyn_Tqual _tmpE7;void*_tmpE6;int _tmpE5;int _tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Core_Opt*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;enum Cyc_Absyn_Incrementor _tmpD6;enum Cyc_Absyn_Primop _tmpD5;struct Cyc_List_List*_tmpD4;struct _fat_ptr _tmpD3;void*_tmpD2;union Cyc_Absyn_Cnst _tmpD1;switch(*((int*)_tmpB9)){case 0U: _LL1: _tmpD1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2:
 new_e=Cyc_Absyn_const_exp(_tmpD1,e->loc);goto _LL0;case 1U: _LL3: _tmpD2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL4:
 new_e=Cyc_Absyn_varb_exp(_tmpD2,e->loc);goto _LL0;case 2U: _LL5: _tmpD3=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL6:
 new_e=Cyc_Absyn_pragma_exp(_tmpD3,e->loc);goto _LL0;case 3U: _LL7: _tmpD5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL8:
 new_e=({enum Cyc_Absyn_Primop _tmp66F=_tmpD5;struct Cyc_List_List*_tmp66E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpD4);Cyc_Absyn_primop_exp(_tmp66F,_tmp66E,e->loc);});goto _LL0;case 5U: _LL9: _tmpD7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLA:
 new_e=({struct Cyc_Absyn_Exp*_tmp671=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD7);enum Cyc_Absyn_Incrementor _tmp670=_tmpD6;Cyc_Absyn_increment_exp(_tmp671,_tmp670,e->loc);});goto _LL0;case 7U: _LLB: _tmpD9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLC:
 new_e=({struct Cyc_Absyn_Exp*_tmp673=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD9);struct Cyc_Absyn_Exp*_tmp672=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD8);Cyc_Absyn_and_exp(_tmp673,_tmp672,e->loc);});goto _LL0;case 8U: _LLD: _tmpDB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLE:
 new_e=({struct Cyc_Absyn_Exp*_tmp675=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDB);struct Cyc_Absyn_Exp*_tmp674=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDA);Cyc_Absyn_or_exp(_tmp675,_tmp674,e->loc);});goto _LL0;case 9U: _LLF: _tmpDD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL10:
 new_e=({struct Cyc_Absyn_Exp*_tmp677=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDD);struct Cyc_Absyn_Exp*_tmp676=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDC);Cyc_Absyn_seq_exp(_tmp677,_tmp676,e->loc);});goto _LL0;case 6U: _LL11: _tmpE0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpDE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL12:
# 475
 new_e=({struct Cyc_Absyn_Exp*_tmp67A=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE0);struct Cyc_Absyn_Exp*_tmp679=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDF);struct Cyc_Absyn_Exp*_tmp678=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDE);Cyc_Absyn_conditional_exp(_tmp67A,_tmp679,_tmp678,e->loc);});goto _LL0;case 4U: _LL13: _tmpE3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL14:
# 477
 new_e=({struct Cyc_Absyn_Exp*_tmp67D=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE3);struct Cyc_Core_Opt*_tmp67C=(unsigned)_tmpE2?({struct Cyc_Core_Opt*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->v=(void*)_tmpE2->v;_tmpBA;}): 0;struct Cyc_Absyn_Exp*_tmp67B=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE1);Cyc_Absyn_assignop_exp(_tmp67D,_tmp67C,_tmp67B,e->loc);});
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3 != 0){_LL15: _tmpEC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->num_varargs;_tmpE9=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->injectors;_tmpE8=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->name;_tmpE7=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->tq;_tmpE6=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->type;_tmpE5=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->inject;_tmpE4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL16:
# 480
 new_e=({void*_tmp683=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->tag=10U,({
struct Cyc_Absyn_Exp*_tmp682=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpEC);_tmpBD->f1=_tmp682;}),({struct Cyc_List_List*_tmp681=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpEB);_tmpBD->f2=_tmp681;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp680=({struct Cyc_Absyn_VarargCallInfo*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->num_varargs=_tmpEA,_tmpBC->injectors=_tmpE9,({
struct Cyc_Absyn_VarargInfo*_tmp67F=({struct Cyc_Absyn_VarargInfo*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->name=_tmpE8,_tmpBB->tq=_tmpE7,({void*_tmp67E=Cyc_Tcutil_copy_type(_tmpE6);_tmpBB->type=_tmp67E;}),_tmpBB->inject=_tmpE5;_tmpBB;});_tmpBC->vai=_tmp67F;});_tmpBC;});
# 482
_tmpBD->f3=_tmp680;}),_tmpBD->f4=_tmpE4;_tmpBD;});
# 480
Cyc_Absyn_new_exp(_tmp683,e->loc);});
# 485
goto _LL0;}else{_LL17: _tmpF0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpEF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpED=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL18:
# 487
 new_e=({void*_tmp686=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=10U,({struct Cyc_Absyn_Exp*_tmp685=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF0);_tmpBE->f1=_tmp685;}),({struct Cyc_List_List*_tmp684=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpEF);_tmpBE->f2=_tmp684;}),_tmpBE->f3=_tmpEE,_tmpBE->f4=_tmpED;_tmpBE;});Cyc_Absyn_new_exp(_tmp686,e->loc);});
goto _LL0;}case 11U: _LL19: _tmpF2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF1=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1A:
# 490
 new_e=_tmpF1?({struct Cyc_Absyn_Exp*_tmp688=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF2);Cyc_Absyn_rethrow_exp(_tmp688,e->loc);}):({struct Cyc_Absyn_Exp*_tmp687=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF2);Cyc_Absyn_throw_exp(_tmp687,e->loc);});
goto _LL0;case 12U: _LL1B: _tmpF3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL1C:
# 493
 new_e=({struct Cyc_Absyn_Exp*_tmp689=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF3);Cyc_Absyn_noinstantiate_exp(_tmp689,e->loc);});goto _LL0;case 13U: _LL1D: _tmpF5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1E:
# 495
 new_e=({struct Cyc_Absyn_Exp*_tmp68B=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF5);struct Cyc_List_List*_tmp68A=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmpF4);Cyc_Absyn_instantiate_exp(_tmp68B,_tmp68A,e->loc);});goto _LL0;case 14U: _LL1F: _tmpF9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpF7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpF6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL20:
# 497
 new_e=({void*_tmp68F=Cyc_Tcutil_copy_type(_tmpF9);struct Cyc_Absyn_Exp*_tmp68E=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF8);int _tmp68D=_tmpF7;enum Cyc_Absyn_Coercion _tmp68C=_tmpF6;Cyc_Absyn_cast_exp(_tmp68F,_tmp68E,_tmp68D,_tmp68C,e->loc);});goto _LL0;case 16U: _LL21: _tmpFB=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL22:
# 499
 new_e=({struct Cyc_Absyn_Exp*_tmp691=(unsigned)_tmpFB?Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFB): 0;struct Cyc_Absyn_Exp*_tmp690=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFA);Cyc_Absyn_New_exp(_tmp691,_tmp690,e->loc);});goto _LL0;case 15U: _LL23: _tmpFC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL24:
 new_e=({struct Cyc_Absyn_Exp*_tmp692=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFC);Cyc_Absyn_address_exp(_tmp692,e->loc);});goto _LL0;case 17U: _LL25: _tmpFD=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL26:
 new_e=({void*_tmp693=Cyc_Tcutil_copy_type(_tmpFD);Cyc_Absyn_sizeoftype_exp(_tmp693,e->loc);});goto _LL0;case 18U: _LL27: _tmpFE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL28:
 new_e=({struct Cyc_Absyn_Exp*_tmp694=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFE);Cyc_Absyn_sizeofexp_exp(_tmp694,e->loc);});goto _LL0;case 19U: _LL29: _tmp100=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFF=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL2A:
 new_e=({void*_tmp696=Cyc_Tcutil_copy_type(_tmp100);struct Cyc_List_List*_tmp695=_tmpFF;Cyc_Absyn_offsetof_exp(_tmp696,_tmp695,e->loc);});goto _LL0;case 20U: _LL2B: _tmp101=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2C:
 new_e=({struct Cyc_Absyn_Exp*_tmp697=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp101);Cyc_Absyn_deref_exp(_tmp697,e->loc);});goto _LL0;case 41U: _LL2D: _tmp102=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2E:
 new_e=({struct Cyc_Absyn_Exp*_tmp698=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp102);Cyc_Absyn_extension_exp(_tmp698,e->loc);});goto _LL0;case 24U: _LL2F: _tmp103=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL30:
 new_e=({struct Cyc_List_List*_tmp699=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmp103);Cyc_Absyn_tuple_exp(_tmp699,e->loc);});goto _LL0;case 21U: _LL31: _tmp107=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp106=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp105=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp104=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL32:
# 508
 new_e=({void*_tmp69B=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->tag=21U,({struct Cyc_Absyn_Exp*_tmp69A=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp107);_tmpBF->f1=_tmp69A;}),_tmpBF->f2=_tmp106,_tmpBF->f3=_tmp105,_tmpBF->f4=_tmp104;_tmpBF;});Cyc_Absyn_new_exp(_tmp69B,e->loc);});goto _LL0;case 22U: _LL33: _tmp10B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp10A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp109=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp108=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL34:
# 510
 new_e=({void*_tmp69D=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->tag=22U,({struct Cyc_Absyn_Exp*_tmp69C=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10B);_tmpC0->f1=_tmp69C;}),_tmpC0->f2=_tmp10A,_tmpC0->f3=_tmp109,_tmpC0->f4=_tmp108;_tmpC0;});Cyc_Absyn_new_exp(_tmp69D,e->loc);});goto _LL0;case 23U: _LL35: _tmp10D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp10C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL36:
# 512
 new_e=({struct Cyc_Absyn_Exp*_tmp69F=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10D);struct Cyc_Absyn_Exp*_tmp69E=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10C);Cyc_Absyn_subscript_exp(_tmp69F,_tmp69E,e->loc);});goto _LL0;case 26U: _LL37: _tmp10E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL38:
# 514
 new_e=({void*_tmp6A1=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->tag=26U,({struct Cyc_List_List*_tmp6A0=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp10E);_tmpC1->f1=_tmp6A0;});_tmpC1;});Cyc_Absyn_new_exp(_tmp6A1,e->loc);});goto _LL0;case 31U: _LL39: _tmp111=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp110=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp10F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3A:
# 516
 new_e=({void*_tmp6A3=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->tag=31U,({struct Cyc_List_List*_tmp6A2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmp111);_tmpC2->f1=_tmp6A2;}),_tmpC2->f2=_tmp110,_tmpC2->f3=_tmp10F;_tmpC2;});Cyc_Absyn_new_exp(_tmp6A3,e->loc);});goto _LL0;case 27U: _LL3B: _tmp115=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp114=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp113=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp112=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL3C:
# 518
 new_e=({void*_tmp6A6=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=27U,_tmpC3->f1=_tmp115,({struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp114);_tmpC3->f2=_tmp6A5;}),({struct Cyc_Absyn_Exp*_tmp6A4=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp113);_tmpC3->f3=_tmp6A4;}),_tmpC3->f4=_tmp112;_tmpC3;});Cyc_Absyn_new_exp(_tmp6A6,e->loc);});goto _LL0;case 28U: _LL3D: _tmp118=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp117=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp116=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3E:
# 520
 new_e=({void*_tmp6A9=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->tag=28U,({struct Cyc_Absyn_Exp*_tmp6A8=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp118);_tmpC4->f1=_tmp6A8;}),({void*_tmp6A7=Cyc_Tcutil_copy_type(_tmp117);_tmpC4->f2=_tmp6A7;}),_tmpC4->f3=_tmp116;_tmpC4;});Cyc_Absyn_new_exp(_tmp6A9,_tmp118->loc);});
goto _LL0;case 29U: _LL3F: _tmp11C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp11A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp119=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL40:
# 523
 new_e=({void*_tmp6AC=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->tag=29U,_tmpC5->f1=_tmp11C,({struct Cyc_List_List*_tmp6AB=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp11B);_tmpC5->f2=_tmp6AB;}),({struct Cyc_List_List*_tmp6AA=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11A);_tmpC5->f3=_tmp6AA;}),_tmpC5->f4=_tmp119;_tmpC5;});Cyc_Absyn_new_exp(_tmp6AC,e->loc);});
# 525
goto _LL0;case 30U: _LL41: _tmp11E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL42:
# 527
 new_e=({void*_tmp6AF=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=30U,({void*_tmp6AE=Cyc_Tcutil_copy_type(_tmp11E);_tmpC6->f1=_tmp6AE;}),({struct Cyc_List_List*_tmp6AD=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11D);_tmpC6->f2=_tmp6AD;});_tmpC6;});Cyc_Absyn_new_exp(_tmp6AF,e->loc);});
goto _LL0;case 25U: _LL43: _tmp122=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f1;_tmp121=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f2;_tmp120=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f3;_tmp11F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL44:
# 530
 new_e=({void*_tmp6B3=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->tag=25U,({struct _tuple9*_tmp6B2=({struct _tuple9*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=_tmp122,_tmpC7->f2=_tmp121,({void*_tmp6B1=Cyc_Tcutil_copy_type(_tmp120);_tmpC7->f3=_tmp6B1;});_tmpC7;});_tmpC8->f1=_tmp6B2;}),({
struct Cyc_List_List*_tmp6B0=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11F);_tmpC8->f2=_tmp6B0;});_tmpC8;});
# 530
Cyc_Absyn_new_exp(_tmp6B3,e->loc);});
# 532
goto _LL0;case 32U: _LL45: _tmp124=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp123=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL46:
 new_e=e;goto _LL0;case 33U: _LL47: _tmp126=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp125=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL48:
# 535
 new_e=({void*_tmp6B5=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=33U,({void*_tmp6B4=Cyc_Tcutil_copy_type(_tmp126);_tmpC9->f1=_tmp6B4;}),_tmpC9->f2=_tmp125;_tmpC9;});Cyc_Absyn_new_exp(_tmp6B5,e->loc);});goto _LL0;case 34U: _LL49: _tmp12C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).is_calloc;_tmp12B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).rgn;_tmp12A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).elt_type;_tmp129=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).num_elts;_tmp128=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).fat_result;_tmp127=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).inline_call;_LL4A: {
# 537
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp12B;if(_tmp12B != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12B);{
void**t1=_tmp12A;if(_tmp12A != 0)t1=({void**_tmpCA=_cycalloc(sizeof(*_tmpCA));({void*_tmp6B6=Cyc_Tcutil_copy_type(*_tmp12A);*_tmpCA=_tmp6B6;});_tmpCA;});
({void*_tmp6B7=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=34U,(_tmpCB->f1).is_calloc=_tmp12C,(_tmpCB->f1).rgn=r1,(_tmpCB->f1).elt_type=t1,(_tmpCB->f1).num_elts=_tmp129,(_tmpCB->f1).fat_result=_tmp128,(_tmpCB->f1).inline_call=_tmp127;_tmpCB;});e2->r=_tmp6B7;});
new_e=e2;
goto _LL0;};}case 35U: _LL4B: _tmp12E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp12D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4C:
 new_e=({struct Cyc_Absyn_Exp*_tmp6B9=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12E);struct Cyc_Absyn_Exp*_tmp6B8=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12D);Cyc_Absyn_swap_exp(_tmp6B9,_tmp6B8,e->loc);});goto _LL0;case 36U: _LL4D: _tmp130=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp12F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4E: {
# 545
struct Cyc_Core_Opt*nopt1=_tmp130;
if(_tmp130 != 0)nopt1=({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->v=(struct _tuple1*)_tmp130->v;_tmpCC;});
new_e=({void*_tmp6BB=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->tag=36U,_tmpCD->f1=nopt1,({struct Cyc_List_List*_tmp6BA=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp12F);_tmpCD->f2=_tmp6BA;});_tmpCD;});Cyc_Absyn_new_exp(_tmp6BB,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 550
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6BC=({const char*_tmpCE="deep_copy: statement expressions unsupported";_tag_fat(_tmpCE,sizeof(char),45U);});_tmpCF->f1=_tmp6BC;});_tmpCF;}));case 38U: _LL51: _tmp132=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp131=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL52:
# 552
 new_e=({void*_tmp6BE=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->tag=38U,({struct Cyc_Absyn_Exp*_tmp6BD=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp132);_tmpD0->f1=_tmp6BD;}),_tmpD0->f2=_tmp131;_tmpD0;});Cyc_Absyn_new_exp(_tmp6BE,e->loc);});goto _LL0;case 39U: _LL53: _tmp133=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL54:
 new_e=({void*_tmp6BF=Cyc_Tcutil_copy_type(_tmp133);Cyc_Absyn_valueof_exp(_tmp6BF,e->loc);});goto _LL0;default: _LL55: _tmp135=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp134=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL56:
 new_e=Cyc_Absyn_asm_exp(_tmp135,_tmp134,e->loc);goto _LL0;}_LL0:;}
# 557
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 561
return new_e;}struct _tuple16{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple17{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 572 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp136=ka1;enum Cyc_Absyn_KindQual _tmp13F;enum Cyc_Absyn_AliasQual _tmp13E;_LL1: _tmp13F=_tmp136->kind;_tmp13E=_tmp136->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp137=ka2;enum Cyc_Absyn_KindQual _tmp13D;enum Cyc_Absyn_AliasQual _tmp13C;_LL4: _tmp13D=_tmp137->kind;_tmp13C=_tmp137->aliasqual;_LL5:;
# 576
if((int)_tmp13F != (int)_tmp13D){
struct _tuple16 _tmp138=({struct _tuple16 _tmp60A;_tmp60A.f1=_tmp13F,_tmp60A.f2=_tmp13D;_tmp60A;});struct _tuple16 _tmp139=_tmp138;switch(_tmp139.f1){case Cyc_Absyn_BoxKind: switch(_tmp139.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp139.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 584
if((int)_tmp13E != (int)_tmp13C){
struct _tuple17 _tmp13A=({struct _tuple17 _tmp60B;_tmp60B.f1=_tmp13E,_tmp60B.f2=_tmp13C;_tmp60B;});struct _tuple17 _tmp13B=_tmp13A;switch(_tmp13B.f1){case Cyc_Absyn_Aliasable: if(_tmp13B.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp13B.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 590
return 1;};}
# 593
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp140=Cyc_Absyn_compress_kb(tv->kind);void*_tmp141=_tmp140;struct Cyc_Absyn_Kind*_tmp144;struct Cyc_Absyn_Kind*_tmp143;switch(*((int*)_tmp141)){case 0U: _LL1: _tmp143=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp141)->f1;_LL2:
 return _tmp143;case 2U: _LL3: _tmp144=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141)->f2;_LL4:
 return _tmp144;default: _LL5: _LL6:
({void*_tmp6C0=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp142=_cycalloc(sizeof(*_tmp142));_tmp142->tag=2U,_tmp142->f1=0,_tmp142->f2=def;_tmp142;});tv->kind=_tmp6C0;});return def;}_LL0:;}struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 601
struct _tuple18 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp145=Cyc_Tcutil_compress(t);void*_tmp146=_tmp145;struct Cyc_Absyn_Tvar*_tmp14B;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->tag == 2U){_LL1: _tmp14B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->f1;_LL2: {
# 604
void*_tmp147=_tmp14B->kind;
_tmp14B->kind=kb;
return({struct _tuple18 _tmp60C;_tmp60C.f1=_tmp14B,_tmp60C.f2=_tmp147;_tmp60C;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp14A=({struct Cyc_String_pa_PrintArg_struct _tmp60D;_tmp60D.tag=0U,({struct _fat_ptr _tmp6C1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp60D.f1=_tmp6C1;});_tmp60D;});void*_tmp148[1U];_tmp148[0]=& _tmp14A;({struct _fat_ptr _tmp6C2=({const char*_tmp149="swap_kind: cannot update the kind of %s";_tag_fat(_tmp149,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C2,_tag_fat(_tmp148,sizeof(void*),1U));});});}_LL0:;}
# 613
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 615
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 619
struct _RegionHandle _tmp14C=_new_region("temp");struct _RegionHandle*temp=& _tmp14C;_push_region(temp);
{struct Cyc_List_List*_tmp14D=0;
# 622
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp14E=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp14F=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp150=Cyc_Tcutil_tvar_kind(_tmp14E,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp151=_tmp150;switch(((struct Cyc_Absyn_Kind*)_tmp151)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 628
 _tmp14D=({struct Cyc_List_List*_tmp153=_region_malloc(temp,sizeof(*_tmp153));({struct _tuple18*_tmp6C3=({struct _tuple18*_tmp152=_region_malloc(temp,sizeof(*_tmp152));_tmp152->f1=_tmp14E,_tmp152->f2=_tmp14F;_tmp152;});_tmp153->hd=_tmp6C3;}),_tmp153->tl=_tmp14D;_tmp153;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 632
if(_tmp14D != 0){
field_type=({struct _RegionHandle*_tmp6C5=temp;struct Cyc_List_List*_tmp6C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp14D);Cyc_Tcutil_rsubstitute(_tmp6C5,_tmp6C4,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 620
;_pop_region(temp);}
# 637
return k;}
# 644
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 646
void*_tmp154=Cyc_Tcutil_compress(t);void*_tmp155=_tmp154;struct Cyc_Absyn_Typedefdecl*_tmp173;struct Cyc_Absyn_Exp*_tmp172;struct Cyc_Absyn_PtrInfo _tmp171;void*_tmp170;struct Cyc_List_List*_tmp16F;struct Cyc_Absyn_Tvar*_tmp16E;struct Cyc_Core_Opt*_tmp16D;switch(*((int*)_tmp155)){case 1U: _LL1: _tmp16D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp155)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp16D))->v;case 2U: _LL3: _tmp16E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp155)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp16E,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp170=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f1;_tmp16F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f2;_LL6: {
# 650
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
# 671
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 673
({void*_tmp157=0U;({struct _fat_ptr _tmp6C6=({const char*_tmp158="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp158,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C6,_tag_fat(_tmp157,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 677
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp163=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->kind;_tmp162=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->tvs;_tmp161=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->impl;_tmp160=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 679
 if(_tmp161 == 0){
if(_tmp160)
return& Cyc_Tcutil_mk;else{
# 683
return& Cyc_Tcutil_ak;}}{
# 685
struct Cyc_List_List*_tmp159=_tmp161->fields;
if(_tmp159 == 0)return& Cyc_Tcutil_mk;
# 688
if((int)_tmp163 == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(_tmp159))->tl != 0;_tmp159=_tmp159->tl){;}{
void*_tmp15A=((struct Cyc_Absyn_Aggrfield*)_tmp159->hd)->type;
struct Cyc_Absyn_Kind*_tmp15B=Cyc_Tcutil_field_kind(_tmp15A,_tmp16F,_tmp162);
if(_tmp15B == & Cyc_Tcutil_ak || _tmp15B == & Cyc_Tcutil_tak)return _tmp15B;};}else{
# 696
for(0;_tmp159 != 0;_tmp159=_tmp159->tl){
void*_tmp15C=((struct Cyc_Absyn_Aggrfield*)_tmp159->hd)->type;
struct Cyc_Absyn_Kind*_tmp15D=Cyc_Tcutil_field_kind(_tmp15C,_tmp16F,_tmp162);
if(_tmp15D == & Cyc_Tcutil_ak || _tmp15D == & Cyc_Tcutil_tak)return _tmp15D;}}
# 702
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 704
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp171=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp155)->f1;_LLC: {
# 707
void*_tmp164=Cyc_Tcutil_compress((_tmp171.ptr_atts).bounds);void*_tmp165=_tmp164;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp165)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp165)->f1)){case 13U: _LL4D: _LL4E: {
# 709
enum Cyc_Absyn_AliasQual _tmp166=(Cyc_Tcutil_type_kind((_tmp171.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp167=_tmp166;switch(_tmp167){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 715
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 717
enum Cyc_Absyn_AliasQual _tmp168=(Cyc_Tcutil_type_kind((_tmp171.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp169=_tmp168;switch(_tmp169){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 723
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 727
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp172=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp155)->f1).num_elts;_LL12:
# 729
 if(_tmp172 == 0 || Cyc_Tcutil_is_const_exp(_tmp172))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp173=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp155)->f3;_LL16:
# 733
 if(_tmp173 == 0 || _tmp173->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp16C=({struct Cyc_String_pa_PrintArg_struct _tmp60E;_tmp60E.tag=0U,({struct _fat_ptr _tmp6C7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp60E.f1=_tmp6C7;});_tmp60E;});void*_tmp16A[1U];_tmp16A[0]=& _tmp16C;({struct _fat_ptr _tmp6C8=({const char*_tmp16B="type_kind: typedef found: %s";_tag_fat(_tmp16B,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C8,_tag_fat(_tmp16A,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp173->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp155)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 742
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 746
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 755
static void*Cyc_Tcutil_rgns_of(void*t);
# 757
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 761
static struct _tuple18*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp174=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp175=_tmp174;switch(((struct Cyc_Absyn_Kind*)_tmp175)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp175)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->tag=9U,({struct Cyc_Absyn_Exp*_tmp6C9=Cyc_Absyn_uint_exp(0U,0U);_tmp176->f1=_tmp6C9;});_tmp176;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 772
return({struct _tuple18*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->f1=tv,_tmp177->f2=t;_tmp177;});}
# 779
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp178=Cyc_Tcutil_compress(t);void*_tmp179=_tmp178;struct Cyc_List_List*_tmp18F;struct Cyc_List_List*_tmp18E;struct Cyc_List_List*_tmp18D;void*_tmp18C;struct Cyc_Absyn_Tqual _tmp18B;void*_tmp18A;struct Cyc_List_List*_tmp189;struct Cyc_Absyn_VarargInfo*_tmp188;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp186;void*_tmp185;void*_tmp184;void*_tmp183;struct Cyc_List_List*_tmp182;switch(*((int*)_tmp179)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp182=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f2;_LL6: {
# 784
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp182);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 788
struct Cyc_Absyn_Kind*_tmp17A=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp17B=_tmp17A;switch(((struct Cyc_Absyn_Kind*)_tmp17B)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp184=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp179)->f1).elt_type;_tmp183=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp179)->f1).ptr_atts).rgn;_LLC:
# 796
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp17C[2U];({void*_tmp6CB=Cyc_Absyn_access_eff(_tmp183);_tmp17C[0]=_tmp6CB;}),({void*_tmp6CA=Cyc_Tcutil_rgns_of(_tmp184);_tmp17C[1]=_tmp6CA;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp17C,sizeof(void*),2U));})));case 4U: _LLD: _tmp185=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp179)->f1).elt_type;_LLE:
# 798
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp185));case 7U: _LLF: _tmp186=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp179)->f2;_LL10:
# 800
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp186)));case 5U: _LL11: _tmp18D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).tvars;_tmp18C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).effect;_tmp18B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ret_tqual;_tmp18A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ret_type;_tmp189=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).args;_tmp188=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).cyc_varargs;_tmp187=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).rgn_po;_LL12: {
# 809
void*_tmp17D=({struct Cyc_List_List*_tmp6CC=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp18D);Cyc_Tcutil_substitute(_tmp6CC,(void*)_check_null(_tmp18C));});
return Cyc_Tcutil_normalize_effect(_tmp17D);}case 6U: _LL13: _tmp18E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp179)->f1;_LL14: {
# 812
struct Cyc_List_List*_tmp17E=0;
for(0;_tmp18E != 0;_tmp18E=_tmp18E->tl){
_tmp17E=({struct Cyc_List_List*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->hd=(*((struct _tuple11*)_tmp18E->hd)).f2,_tmp17F->tl=_tmp17E;_tmp17F;});}
_tmp18F=_tmp17E;goto _LL16;}case 8U: _LL15: _tmp18F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp179)->f2;_LL16:
# 817
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp18F)));case 10U: _LL17: _LL18:
({void*_tmp180=0U;({struct _fat_ptr _tmp6CD=({const char*_tmp181="typedecl in rgns_of";_tag_fat(_tmp181,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CD,_tag_fat(_tmp180,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 827
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp190=e;void*_tmp19C;struct Cyc_List_List**_tmp19B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f1)){case 9U: _LL1: _tmp19B=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2;_LL2: {
# 831
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp19B;for(0;effs != 0;effs=effs->tl){
void*_tmp191=(void*)effs->hd;
({void*_tmp6CE=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp191));effs->hd=_tmp6CE;});{
void*_tmp192=(void*)effs->hd;void*_tmp193=_tmp192;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LLE: _LLF:
# 840
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 844
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp19B;for(0;effs != 0;effs=effs->tl){
void*_tmp194=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp195=_tmp194;void*_tmp198;struct Cyc_List_List*_tmp197;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)){case 9U: _LL13: _tmp197=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2;_LL14:
# 849
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp197,effects);
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp198=_tmp195;_LL1C:
 effects=({struct Cyc_List_List*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->hd=_tmp198,_tmp196->tl=effects;_tmp196;});goto _LL12;}_LL12:;}}
# 857
({struct Cyc_List_List*_tmp6CF=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp19B=_tmp6CF;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2 != 0){_LL3: _tmp19C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2)->hd;_LL4: {
# 860
void*_tmp199=Cyc_Tcutil_compress(_tmp19C);void*_tmp19A=_tmp199;switch(*((int*)_tmp19A)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp19C);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 865
 return e;}_LL0:;};}
# 870
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19D=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=5U,(_tmp19E->f1).tvars=0,(_tmp19E->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6D0=Cyc_Absyn_empty_tqual(0U);(_tmp19E->f1).ret_tqual=_tmp6D0;}),(_tmp19E->f1).ret_type=Cyc_Absyn_void_type,(_tmp19E->f1).args=0,(_tmp19E->f1).c_varargs=0,(_tmp19E->f1).cyc_varargs=0,(_tmp19E->f1).rgn_po=0,(_tmp19E->f1).attributes=0,(_tmp19E->f1).requires_clause=0,(_tmp19E->f1).requires_relns=0,(_tmp19E->f1).ensures_clause=0,(_tmp19E->f1).ensures_relns=0;_tmp19E;});
# 880
return({void*_tmp6D4=(void*)_tmp19D;void*_tmp6D3=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6D2=Cyc_Absyn_empty_tqual(0U);void*_tmp6D1=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp6D4,_tmp6D3,_tmp6D2,_tmp6D1,Cyc_Absyn_false_type);});}
# 887
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp19F=Cyc_Tcutil_compress(e);void*_tmp1A0=_tmp19F;struct Cyc_Core_Opt*_tmp1BA;void**_tmp1B9;struct Cyc_Core_Opt*_tmp1B8;void*_tmp1B7;struct Cyc_List_List*_tmp1B6;void*_tmp1B5;switch(*((int*)_tmp1A0)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2 != 0){_LL1: _tmp1B5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->hd;_LL2:
# 894
 if(constrain)return Cyc_Unify_unify(r,_tmp1B5);
_tmp1B5=Cyc_Tcutil_compress(_tmp1B5);
if(r == _tmp1B5)return 1;{
struct _tuple14 _tmp1A1=({struct _tuple14 _tmp60F;_tmp60F.f1=r,_tmp60F.f2=_tmp1B5;_tmp60F;});struct _tuple14 _tmp1A2=_tmp1A1;struct Cyc_Absyn_Tvar*_tmp1A4;struct Cyc_Absyn_Tvar*_tmp1A3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2.f2)->tag == 2U){_LLC: _tmp1A4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2.f1)->f1;_tmp1A3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A2.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp1A4,_tmp1A3)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp1B6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2;_LL4:
# 902
 for(0;_tmp1B6 != 0;_tmp1B6=_tmp1B6->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp1B6->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2 != 0){_LL5: _tmp1B7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->hd;_LL6: {
# 906
void*_tmp1A5=Cyc_Tcutil_rgns_of(_tmp1B7);void*_tmp1A6=_tmp1A5;void*_tmp1AF;void*_tmp1AE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2 != 0){_LL11: _tmp1AE=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2)->hd;_LL12:
# 908
 if(!constrain)return 0;{
void*_tmp1A7=Cyc_Tcutil_compress(_tmp1AE);void*_tmp1A8=_tmp1A7;struct Cyc_Core_Opt*_tmp1AD;void**_tmp1AC;struct Cyc_Core_Opt*_tmp1AB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8)->tag == 1U){_LL16: _tmp1AD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8)->f1;_tmp1AC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8)->f2;_tmp1AB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A8)->f4;_LL17: {
# 913
void*_tmp1A9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1AB);
# 916
Cyc_Unify_occurs(_tmp1A9,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1AB))->v,r);
({void*_tmp6D6=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1AA[2U];_tmp1AA[0]=_tmp1A9,({void*_tmp6D5=Cyc_Absyn_access_eff(r);_tmp1AA[1]=_tmp6D5;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1AA,sizeof(void*),2U));})));*_tmp1AC=_tmp6D6;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1AF=_tmp1A6;_LL14:
# 921
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp1AF);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1BA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f1;_tmp1B9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f2;_tmp1B8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f4;_LL8:
# 924
 if(_tmp1BA == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1BA->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1B0=0U;({struct _fat_ptr _tmp6D7=({const char*_tmp1B1="effect evar has wrong kind";_tag_fat(_tmp1B1,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D7,_tag_fat(_tmp1B0,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 929
void*_tmp1B2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1B8);
# 932
Cyc_Unify_occurs(_tmp1B2,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1B8))->v,r);{
void*_tmp1B3=Cyc_Absyn_join_eff(({void*_tmp1B4[2U];_tmp1B4[0]=_tmp1B2,({void*_tmp6D8=Cyc_Absyn_access_eff(r);_tmp1B4[1]=_tmp6D8;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B4,sizeof(void*),2U));}));
*_tmp1B9=_tmp1B3;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 943
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1BB=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp1BC=_tmp1BB;struct Cyc_Core_Opt*_tmp1CE;void**_tmp1CD;struct Cyc_Core_Opt*_tmp1CC;void*_tmp1CB;struct Cyc_List_List*_tmp1CA;switch(*((int*)_tmp1BC)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1CA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2;_LL4:
# 948
 for(0;_tmp1CA != 0;_tmp1CA=_tmp1CA->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1CA->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2 != 0){_LL5: _tmp1CB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2)->hd;_LL6:
# 953
 _tmp1CB=Cyc_Tcutil_compress(_tmp1CB);
if(t == _tmp1CB)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,_tmp1CB);{
void*_tmp1BD=Cyc_Tcutil_rgns_of(t);void*_tmp1BE=_tmp1BD;void*_tmp1C4;void*_tmp1C3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f2 != 0){_LLC: _tmp1C3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f2)->hd;_LLD: {
# 958
struct _tuple14 _tmp1BF=({struct _tuple14 _tmp610;({void*_tmp6D9=Cyc_Tcutil_compress(_tmp1C3);_tmp610.f1=_tmp6D9;}),_tmp610.f2=_tmp1CB;_tmp610;});struct _tuple14 _tmp1C0=_tmp1BF;struct Cyc_Absyn_Tvar*_tmp1C2;struct Cyc_Absyn_Tvar*_tmp1C1;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C0.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C0.f2)->tag == 2U){_LL11: _tmp1C2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C0.f1)->f1;_tmp1C1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C0.f2)->f1;_LL12:
 return Cyc_Unify_unify(t,_tmp1CB);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp1C3 == _tmp1CB;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1C4=_tmp1BE;_LLF:
# 962
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp1C4);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1CE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f1;_tmp1CD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f2;_tmp1CC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f4;_LL8:
# 965
 if(_tmp1CE == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1CE->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1C5=0U;({struct _fat_ptr _tmp6DA=({const char*_tmp1C6="effect evar has wrong kind";_tag_fat(_tmp1C6,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DA,_tag_fat(_tmp1C5,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 970
void*_tmp1C7=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1CC);
# 973
Cyc_Unify_occurs(_tmp1C7,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1CC))->v,t);{
void*_tmp1C8=Cyc_Absyn_join_eff(({void*_tmp1C9[2U];_tmp1C9[0]=_tmp1C7,({void*_tmp6DB=Cyc_Absyn_regionsof_eff(t);_tmp1C9[1]=_tmp6DB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C9,sizeof(void*),2U));}));
*_tmp1CD=_tmp1C8;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 984
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1CF=e;struct Cyc_Core_Opt*_tmp1E5;void**_tmp1E4;struct Cyc_Core_Opt*_tmp1E3;void*_tmp1E2;struct Cyc_List_List*_tmp1E1;struct Cyc_Absyn_Tvar*_tmp1E0;switch(*((int*)_tmp1CF)){case 2U: _LL1: _tmp1E0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CF)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp1E0)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f1)){case 9U: _LL3: _tmp1E1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2;_LL4:
# 989
 for(0;_tmp1E1 != 0;_tmp1E1=_tmp1E1->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp1E1->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2 != 0){_LL5: _tmp1E2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2)->hd;_LL6: {
# 994
void*_tmp1D0=Cyc_Tcutil_rgns_of(_tmp1E2);void*_tmp1D1=_tmp1D0;void*_tmp1DA;void*_tmp1D9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2 != 0){_LLC: _tmp1D9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D1)->f2)->hd;_LLD:
# 996
 if(!may_constrain_evars)return 0;{
void*_tmp1D2=Cyc_Tcutil_compress(_tmp1D9);void*_tmp1D3=_tmp1D2;struct Cyc_Core_Opt*_tmp1D8;void**_tmp1D7;struct Cyc_Core_Opt*_tmp1D6;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->tag == 1U){_LL11: _tmp1D8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f1;_tmp1D7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f2;_tmp1D6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f4;_LL12: {
# 1002
void*_tmp1D4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1D6);
# 1004
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D6))->v,v))return 0;
({void*_tmp6DD=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1D5[2U];_tmp1D5[0]=_tmp1D4,({void*_tmp6DC=Cyc_Absyn_var_type(v);_tmp1D5[1]=_tmp6DC;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D5,sizeof(void*),2U));})));*_tmp1D7=_tmp6DD;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1DA=_tmp1D1;_LLF:
# 1009
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp1DA);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1E5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f1;_tmp1E4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f2;_tmp1E3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f4;_LL8:
# 1012
 if(_tmp1E5 == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1E5->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1DB=0U;({struct _fat_ptr _tmp6DE=({const char*_tmp1DC="effect evar has wrong kind";_tag_fat(_tmp1DC,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DE,_tag_fat(_tmp1DB,sizeof(void*),0U));});});{
# 1016
void*_tmp1DD=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1E3);
# 1018
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E3))->v,v))
return 0;{
void*_tmp1DE=Cyc_Absyn_join_eff(({void*_tmp1DF[2U];_tmp1DF[0]=_tmp1DD,({void*_tmp6DF=Cyc_Absyn_var_type(v);_tmp1DF[1]=_tmp6DF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1DF,sizeof(void*),2U));}));
*_tmp1E4=_tmp1DE;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1028
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1E6=e;void*_tmp1EC;struct Cyc_List_List*_tmp1EB;switch(*((int*)_tmp1E6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f1)){case 9U: _LL1: _tmp1EB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2;_LL2:
# 1032
 for(0;_tmp1EB != 0;_tmp1EB=_tmp1EB->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp1EB->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2 != 0){_LL3: _tmp1EC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2)->hd;_LL4: {
# 1037
void*_tmp1E7=Cyc_Tcutil_rgns_of(_tmp1EC);void*_tmp1E8=_tmp1E7;void*_tmp1EA;void*_tmp1E9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f2 != 0){_LLA: _tmp1E9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1EA=_tmp1E8;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp1EA);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1041
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1055 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1060
void*_tmp1ED=Cyc_Tcutil_compress(e1);void*_tmp1EE=_tmp1ED;void**_tmp1FB;struct Cyc_Core_Opt*_tmp1FA;struct Cyc_Absyn_Tvar*_tmp1F9;void*_tmp1F8;void*_tmp1F7;struct Cyc_List_List*_tmp1F6;switch(*((int*)_tmp1EE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f1)){case 9U: _LL1: _tmp1F6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2;_LL2:
# 1062
 for(0;_tmp1F6 != 0;_tmp1F6=_tmp1F6->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp1F6->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2 != 0){_LL3: _tmp1F7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2)->hd;_LL4:
# 1073
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp1F7,e2)||
 may_constrain_evars && Cyc_Unify_unify(_tmp1F7,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2 != 0){_LL7: _tmp1F8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2)->hd;_LL8: {
# 1077
void*_tmp1EF=Cyc_Tcutil_rgns_of(_tmp1F8);void*_tmp1F0=_tmp1EF;void*_tmp1F2;void*_tmp1F1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f2 != 0){_LLE: _tmp1F1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F0)->f2)->hd;_LLF:
# 1082
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp1F1,e2)||
 may_constrain_evars && Cyc_Unify_unify(_tmp1F1,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1F2=_tmp1F0;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp1F2,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1F9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EE)->f1;_LL6:
# 1075
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp1F9,e2);case 1U: _LL9: _tmp1FB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EE)->f2;_tmp1FA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EE)->f4;_LLA:
# 1087
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1091
*_tmp1FB=Cyc_Absyn_empty_effect;
# 1094
return 1;}else{
# 1096
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp1F5=({struct Cyc_String_pa_PrintArg_struct _tmp611;_tmp611.tag=0U,({struct _fat_ptr _tmp6E0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp611.f1=_tmp6E0;});_tmp611;});void*_tmp1F3[1U];_tmp1F3[0]=& _tmp1F5;({struct _fat_ptr _tmp6E1=({const char*_tmp1F4="subset_effect: bad effect: %s";_tag_fat(_tmp1F4,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E1,_tag_fat(_tmp1F3,sizeof(void*),1U));});});}_LL0:;}
# 1107
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1109
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple14*_tmp1FC=(struct _tuple14*)r1->hd;struct _tuple14*_tmp1FD=_tmp1FC;void*_tmp203;void*_tmp202;_LL1: _tmp203=_tmp1FD->f1;_tmp202=_tmp1FD->f2;_LL2:;{
int found=_tmp203 == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple14*_tmp1FE=(struct _tuple14*)r2->hd;struct _tuple14*_tmp1FF=_tmp1FE;void*_tmp201;void*_tmp200;_LL4: _tmp201=_tmp1FF->f1;_tmp200=_tmp1FF->f2;_LL5:;
if(Cyc_Unify_unify(_tmp203,_tmp201)&& Cyc_Unify_unify(_tmp202,_tmp200)){
found=1;
break;}}}
# 1119
if(!found)return 0;};}}
# 1121
return 1;}
# 1128
int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp204=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp205=({struct Cyc_List_List*_tmp206=_cycalloc(sizeof(*_tmp206));_tmp206->hd=_tmp204,_tmp206->tl=r1;_tmp206;});
if(Cyc_Relations_consistent_relations(_tmp205))return 0;}
# 1134
return 1;}
# 1138
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1140
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1143
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp207=t;switch(*((int*)_tmp207)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp207)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp207)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1149
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1152
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1155
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1158
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp207)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1147
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1150
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1153
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1156
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1159
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp207)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1148
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1151
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1154
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1157
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1160
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1182
({void*_tmp208=0U;({struct _fat_ptr _tmp6E2=({const char*_tmp209="bad con";_tag_fat(_tmp209,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E2,_tag_fat(_tmp208,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp207)->f1){case 0U: _LL21: _LL22:
# 1161
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
# 1185
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp20A=t;void*_tmp20B;switch(*((int*)_tmp20A)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp20B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20A)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp20B);}_LL0:;}
# 1201
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp20C=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp20C != 0)return _tmp20C;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1206
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp20D=i;struct _tuple1*_tmp20F;struct Cyc_Absyn_Datatypedecl*_tmp20E;if((_tmp20D.KnownDatatype).tag == 2){_LL1: _tmp20E=*(_tmp20D.KnownDatatype).val;_LL2:
 return _tmp20E->name;}else{_LL3: _tmp20F=((_tmp20D.UnknownDatatype).val).name;_LL4:
 return _tmp20F;}_LL0:;}struct _tuple19{struct _tuple1*f1;struct _tuple1*f2;};
# 1212
static struct _tuple19 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp210=i;struct _tuple1*_tmp214;struct _tuple1*_tmp213;struct Cyc_Absyn_Datatypedecl*_tmp212;struct Cyc_Absyn_Datatypefield*_tmp211;if((_tmp210.KnownDatatypefield).tag == 2){_LL1: _tmp212=((_tmp210.KnownDatatypefield).val).f1;_tmp211=((_tmp210.KnownDatatypefield).val).f2;_LL2:
# 1215
 return({struct _tuple19 _tmp612;_tmp612.f1=_tmp212->name,_tmp612.f2=_tmp211->name;_tmp612;});}else{_LL3: _tmp214=((_tmp210.UnknownDatatypefield).val).datatype_name;_tmp213=((_tmp210.UnknownDatatypefield).val).field_name;_LL4:
# 1217
 return({struct _tuple19 _tmp613;_tmp613.f1=_tmp214,_tmp613.f2=_tmp213;_tmp613;});}_LL0:;}struct _tuple20{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1220
static struct _tuple20 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp215=i;struct Cyc_Absyn_Aggrdecl*_tmp218;enum Cyc_Absyn_AggrKind _tmp217;struct _tuple1*_tmp216;if((_tmp215.UnknownAggr).tag == 1){_LL1: _tmp217=((_tmp215.UnknownAggr).val).f1;_tmp216=((_tmp215.UnknownAggr).val).f2;_LL2:
 return({struct _tuple20 _tmp614;_tmp614.f1=_tmp217,_tmp614.f2=_tmp216;_tmp614;});}else{_LL3: _tmp218=*(_tmp215.KnownAggr).val;_LL4:
 return({struct _tuple20 _tmp615;_tmp615.f1=_tmp218->kind,_tmp615.f2=_tmp218->name;_tmp615;});}_LL0:;}
# 1226
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1232
struct _tuple14 _tmp219=({struct _tuple14 _tmp616;_tmp616.f1=t1,_tmp616.f2=t2;_tmp616;});struct _tuple14 _tmp21A=_tmp219;union Cyc_Absyn_AggrInfo _tmp238;union Cyc_Absyn_AggrInfo _tmp237;union Cyc_Absyn_DatatypeFieldInfo _tmp236;union Cyc_Absyn_DatatypeFieldInfo _tmp235;union Cyc_Absyn_DatatypeInfo _tmp234;union Cyc_Absyn_DatatypeInfo _tmp233;struct Cyc_List_List*_tmp232;struct Cyc_List_List*_tmp231;struct _fat_ptr _tmp230;struct _fat_ptr _tmp22F;struct _tuple1*_tmp22E;struct _tuple1*_tmp22D;switch(*((int*)_tmp21A.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21A.f2)->tag == 15U){_LL1: _tmp22E=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21A.f1)->f1;_tmp22D=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21A.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp22E,_tmp22D);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21A.f2)->tag == 17U){_LL3: _tmp230=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21A.f1)->f1;_tmp22F=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21A.f2)->f1;_LL4:
 return Cyc_strcmp((struct _fat_ptr)_tmp230,(struct _fat_ptr)_tmp22F);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21A.f2)->tag == 16U){_LL5: _tmp232=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21A.f1)->f1;_tmp231=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21A.f2)->f1;_LL6:
# 1236
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp232,_tmp231);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21A.f2)->tag == 18U){_LL7: _tmp234=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21A.f1)->f1;_tmp233=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21A.f2)->f1;_LL8: {
# 1238
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(_tmp234);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(_tmp233);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21A.f2)->tag == 19U){_LL9: _tmp236=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21A.f1)->f1;_tmp235=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21A.f2)->f1;_LLA: {
# 1242
struct _tuple19 _tmp21B=Cyc_Tcutil_get_datatype_field_qvars(_tmp236);struct _tuple19 _tmp21C=_tmp21B;struct _tuple1*_tmp223;struct _tuple1*_tmp222;_LL10: _tmp223=_tmp21C.f1;_tmp222=_tmp21C.f2;_LL11:;{
struct _tuple19 _tmp21D=Cyc_Tcutil_get_datatype_field_qvars(_tmp235);struct _tuple19 _tmp21E=_tmp21D;struct _tuple1*_tmp221;struct _tuple1*_tmp220;_LL13: _tmp221=_tmp21E.f1;_tmp220=_tmp21E.f2;_LL14:;{
int _tmp21F=Cyc_Absyn_qvar_cmp(_tmp223,_tmp221);
if(_tmp21F != 0)return _tmp21F;
return Cyc_Absyn_qvar_cmp(_tmp222,_tmp220);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21A.f2)->tag == 20U){_LLB: _tmp238=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21A.f1)->f1;_tmp237=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21A.f2)->f1;_LLC: {
# 1248
struct _tuple20 _tmp224=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp238);struct _tuple20 _tmp225=_tmp224;enum Cyc_Absyn_AggrKind _tmp22C;struct _tuple1*_tmp22B;_LL16: _tmp22C=_tmp225.f1;_tmp22B=_tmp225.f2;_LL17:;{
struct _tuple20 _tmp226=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp237);struct _tuple20 _tmp227=_tmp226;enum Cyc_Absyn_AggrKind _tmp22A;struct _tuple1*_tmp229;_LL19: _tmp22A=_tmp227.f1;_tmp229=_tmp227.f2;_LL1A:;{
int _tmp228=Cyc_Absyn_qvar_cmp(_tmp22B,_tmp229);
if(_tmp228 != 0)return _tmp228;
return(int)_tmp22C - (int)_tmp22A;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}
# 1257
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6E4)(void*,void*)=cmp;void*_tmp6E3=(void*)_check_null(a1);_tmp6E4(_tmp6E3,(void*)_check_null(a2));});}
# 1263
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp239=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp23A=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp239,_tmp23A);}
# 1268
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct _tuple11*tqt2){
struct _tuple11*_tmp23B=tqt1;struct Cyc_Absyn_Tqual _tmp241;void*_tmp240;_LL1: _tmp241=_tmp23B->f1;_tmp240=_tmp23B->f2;_LL2:;{
struct _tuple11*_tmp23C=tqt2;struct Cyc_Absyn_Tqual _tmp23F;void*_tmp23E;_LL4: _tmp23F=_tmp23C->f1;_tmp23E=_tmp23C->f2;_LL5:;{
int _tmp23D=Cyc_Tcutil_tqual_cmp(_tmp241,_tmp23F);
if(_tmp23D != 0)return _tmp23D;
return Cyc_Tcutil_typecmp(_tmp240,_tmp23E);};};}
# 1276
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp242=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp242 != 0)return _tmp242;{
int _tmp243=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp243 != 0)return _tmp243;{
int _tmp244=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp244 != 0)return _tmp244;{
int _tmp245=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp245 != 0)return _tmp245;
_tmp245=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp245 != 0)return _tmp245;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1293
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp246=({int _tmp6E5=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6E5,Cyc_Tcutil_type_case_number(t2));});
if(_tmp246 != 0)
return _tmp246;{
# 1302
struct _tuple14 _tmp247=({struct _tuple14 _tmp617;_tmp617.f1=t2,_tmp617.f2=t1;_tmp617;});struct _tuple14 _tmp248=_tmp247;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp292;struct Cyc_Absyn_Exp*_tmp291;enum Cyc_Absyn_AggrKind _tmp290;struct Cyc_List_List*_tmp28F;enum Cyc_Absyn_AggrKind _tmp28E;struct Cyc_List_List*_tmp28D;struct Cyc_List_List*_tmp28C;struct Cyc_List_List*_tmp28B;struct Cyc_Absyn_FnInfo _tmp28A;struct Cyc_Absyn_FnInfo _tmp289;void*_tmp288;struct Cyc_Absyn_Tqual _tmp287;struct Cyc_Absyn_Exp*_tmp286;void*_tmp285;void*_tmp284;struct Cyc_Absyn_Tqual _tmp283;struct Cyc_Absyn_Exp*_tmp282;void*_tmp281;void*_tmp280;struct Cyc_Absyn_Tqual _tmp27F;void*_tmp27E;void*_tmp27D;void*_tmp27C;void*_tmp27B;void*_tmp27A;struct Cyc_Absyn_Tqual _tmp279;void*_tmp278;void*_tmp277;void*_tmp276;void*_tmp275;struct Cyc_Absyn_Tvar*_tmp274;struct Cyc_Absyn_Tvar*_tmp273;void*_tmp272;struct Cyc_List_List*_tmp271;void*_tmp270;struct Cyc_List_List*_tmp26F;switch(*((int*)_tmp248.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248.f2)->tag == 0U){_LL1: _tmp272=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248.f1)->f1;_tmp271=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248.f1)->f2;_tmp270=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248.f2)->f1;_tmp26F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp248.f2)->f2;_LL2: {
# 1304
int _tmp249=Cyc_Tcutil_tycon_cmp(_tmp272,_tmp270);
if(_tmp249 != 0)return _tmp249;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp271,_tmp26F);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp248.f2)->tag == 1U){_LL3: _LL4:
# 1308
({void*_tmp24A=0U;({struct _fat_ptr _tmp6E6=({const char*_tmp24B="typecmp: can only compare closed types";_tag_fat(_tmp24B,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E6,_tag_fat(_tmp24A,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp248.f2)->tag == 2U){_LL5: _tmp274=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp248.f1)->f1;_tmp273=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp248.f2)->f1;_LL6:
# 1312
 return Cyc_Core_intcmp(_tmp273->identity,_tmp274->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f2)->tag == 3U){_LL7: _tmp280=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f1)->f1).elt_type;_tmp27F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f1)->f1).elt_tq;_tmp27E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f1)->f1).ptr_atts).rgn;_tmp27D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f1)->f1).ptr_atts).nullable;_tmp27C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f1)->f1).ptr_atts).bounds;_tmp27B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f1)->f1).ptr_atts).zero_term;_tmp27A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f2)->f1).elt_type;_tmp279=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f2)->f1).elt_tq;_tmp278=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f2)->f1).ptr_atts).rgn;_tmp277=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f2)->f1).ptr_atts).nullable;_tmp276=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f2)->f1).ptr_atts).bounds;_tmp275=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp248.f2)->f1).ptr_atts).zero_term;_LL8: {
# 1316
int _tmp24C=Cyc_Tcutil_typecmp(_tmp27A,_tmp280);
if(_tmp24C != 0)return _tmp24C;{
int _tmp24D=Cyc_Tcutil_typecmp(_tmp278,_tmp27E);
if(_tmp24D != 0)return _tmp24D;{
int _tmp24E=Cyc_Tcutil_tqual_cmp(_tmp279,_tmp27F);
if(_tmp24E != 0)return _tmp24E;{
int _tmp24F=Cyc_Tcutil_typecmp(_tmp276,_tmp27C);
if(_tmp24F != 0)return _tmp24F;{
int _tmp250=Cyc_Tcutil_typecmp(_tmp275,_tmp27B);
if(_tmp250 != 0)return _tmp250;{
int _tmp251=Cyc_Tcutil_typecmp(_tmp276,_tmp27C);
if(_tmp251 != 0)return _tmp251;
return Cyc_Tcutil_typecmp(_tmp277,_tmp27D);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f2)->tag == 4U){_LL9: _tmp288=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f1)->f1).elt_type;_tmp287=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f1)->f1).tq;_tmp286=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f1)->f1).num_elts;_tmp285=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f1)->f1).zero_term;_tmp284=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f2)->f1).elt_type;_tmp283=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f2)->f1).tq;_tmp282=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f2)->f1).num_elts;_tmp281=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp248.f2)->f1).zero_term;_LLA: {
# 1332
int _tmp252=Cyc_Tcutil_tqual_cmp(_tmp283,_tmp287);
if(_tmp252 != 0)return _tmp252;{
int _tmp253=Cyc_Tcutil_typecmp(_tmp284,_tmp288);
if(_tmp253 != 0)return _tmp253;{
int _tmp254=Cyc_Tcutil_typecmp(_tmp285,_tmp281);
if(_tmp254 != 0)return _tmp254;
if(_tmp286 == _tmp282)return 0;
if(_tmp286 == 0 || _tmp282 == 0)
({void*_tmp255=0U;({struct _fat_ptr _tmp6E7=({const char*_tmp256="missing expression in array index";_tag_fat(_tmp256,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E7,_tag_fat(_tmp255,sizeof(void*),0U));});});
# 1342
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp286,_tmp282);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp248.f2)->tag == 5U){_LLB: _tmp28A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp248.f1)->f1;_tmp289=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp248.f2)->f1;_LLC:
# 1345
 if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp28A.ret_type,_tmp289.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(_tmp28A.ret_tqual,_tmp289.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp257=_tmp28A.args;
struct Cyc_List_List*_tmp258=_tmp289.args;
for(0;_tmp257 != 0 && _tmp258 != 0;(_tmp257=_tmp257->tl,_tmp258=_tmp258->tl)){
struct _tuple9 _tmp259=*((struct _tuple9*)_tmp257->hd);struct _tuple9 _tmp25A=_tmp259;struct Cyc_Absyn_Tqual _tmp260;void*_tmp25F;_LL18: _tmp260=_tmp25A.f2;_tmp25F=_tmp25A.f3;_LL19:;{
struct _tuple9 _tmp25B=*((struct _tuple9*)_tmp258->hd);struct _tuple9 _tmp25C=_tmp25B;struct Cyc_Absyn_Tqual _tmp25E;void*_tmp25D;_LL1B: _tmp25E=_tmp25C.f2;_tmp25D=_tmp25C.f3;_LL1C:;
r=Cyc_Tcutil_tqual_cmp(_tmp260,_tmp25E);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp25F,_tmp25D);
if(r != 0)return r;};}
# 1360
if(_tmp257 != 0)return 1;
if(_tmp258 != 0)return - 1;
if(_tmp28A.c_varargs && !_tmp289.c_varargs)return 1;
if(!_tmp28A.c_varargs && _tmp289.c_varargs)return - 1;
if(_tmp28A.cyc_varargs != 0 & _tmp289.cyc_varargs == 0)return 1;
if(_tmp28A.cyc_varargs == 0 & _tmp289.cyc_varargs != 0)return - 1;
if(_tmp28A.cyc_varargs != 0 & _tmp289.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp6E8=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp28A.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp6E8,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp289.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((_tmp28A.cyc_varargs)->type,(_tmp289.cyc_varargs)->type);
if(r != 0)return r;
if((_tmp28A.cyc_varargs)->inject && !(_tmp289.cyc_varargs)->inject)return 1;
if(!(_tmp28A.cyc_varargs)->inject &&(_tmp289.cyc_varargs)->inject)return - 1;}
# 1374
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp28A.effect,_tmp289.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp261=_tmp28A.rgn_po;
struct Cyc_List_List*_tmp262=_tmp289.rgn_po;
for(0;_tmp261 != 0 && _tmp262 != 0;(_tmp261=_tmp261->tl,_tmp262=_tmp262->tl)){
struct _tuple14 _tmp263=*((struct _tuple14*)_tmp261->hd);struct _tuple14 _tmp264=_tmp263;void*_tmp26A;void*_tmp269;_LL1E: _tmp26A=_tmp264.f1;_tmp269=_tmp264.f2;_LL1F:;{
struct _tuple14 _tmp265=*((struct _tuple14*)_tmp262->hd);struct _tuple14 _tmp266=_tmp265;void*_tmp268;void*_tmp267;_LL21: _tmp268=_tmp266.f1;_tmp267=_tmp266.f2;_LL22:;
r=Cyc_Tcutil_typecmp(_tmp26A,_tmp268);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp269,_tmp267);if(r != 0)return r;};}
# 1384
if(_tmp261 != 0)return 1;
if(_tmp262 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp28A.requires_clause,_tmp289.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp28A.ensures_clause,_tmp289.ensures_clause);
if(r != 0)return r;
# 1392
({void*_tmp26B=0U;({struct _fat_ptr _tmp6E9=({const char*_tmp26C="typecmp: function type comparison should never get here!";_tag_fat(_tmp26C,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E9,_tag_fat(_tmp26B,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp248.f2)->tag == 6U){_LLD: _tmp28C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp248.f1)->f1;_tmp28B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp248.f2)->f1;_LLE:
# 1395
 return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp28B,_tmp28C);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp248.f2)->tag == 7U){_LLF: _tmp290=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp248.f1)->f1;_tmp28F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp248.f1)->f2;_tmp28E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp248.f2)->f1;_tmp28D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp248.f2)->f2;_LL10:
# 1398
 if((int)_tmp28E != (int)_tmp290){
if((int)_tmp28E == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp28D,_tmp28F);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp248.f2)->tag == 9U){_LL11: _tmp292=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp248.f1)->f1;_tmp291=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp248.f2)->f1;_LL12:
# 1403
 _tmp294=_tmp292;_tmp293=_tmp291;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp248.f2)->tag == 11U){_LL13: _tmp294=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp248.f1)->f1;_tmp293=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp248.f2)->f1;_LL14:
# 1405
 return Cyc_Evexp_const_exp_cmp(_tmp294,_tmp293);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp26D=0U;({struct _fat_ptr _tmp6EA=({const char*_tmp26E="Unmatched case in typecmp";_tag_fat(_tmp26E,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6EA,_tag_fat(_tmp26D,sizeof(void*),0U));});});}_LL0:;};};}
# 1412
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple14 _tmp295=({struct _tuple14 _tmp619;({void*_tmp6EC=Cyc_Tcutil_compress(t1);_tmp619.f1=_tmp6EC;}),({void*_tmp6EB=Cyc_Tcutil_compress(t2);_tmp619.f2=_tmp6EB;});_tmp619;});struct _tuple14 _tmp296=_tmp295;void*_tmp29C;void*_tmp29B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296.f2)->tag == 0U){_LL1: _tmp29C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296.f1)->f1;_tmp29B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296.f2)->f1;_LL2: {
# 1415
struct _tuple14 _tmp297=({struct _tuple14 _tmp618;_tmp618.f1=_tmp29C,_tmp618.f2=_tmp29B;_tmp618;});struct _tuple14 _tmp298=_tmp297;int _tmp29A;int _tmp299;switch(*((int*)_tmp298.f1)){case 2U: switch(*((int*)_tmp298.f2)){case 2U: _LL6: _tmp29A=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp298.f1)->f1;_tmp299=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp298.f2)->f1;_LL7:
 return _tmp299 < _tmp29A;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp298.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1423
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1428
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1431
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp298.f2)->f1 == 0){_LL14: _LL15:
# 1426
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp298.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1424
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1429
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1432
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp298.f2)->f1 == 0){_LL16: _LL17:
# 1427
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1433
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp298.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1430
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1434
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1436
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1438
 return 0;}_LL0:;}
# 1442
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple14 _tmp29D=({struct _tuple14 _tmp61B;({void*_tmp6EE=Cyc_Tcutil_compress(t1);_tmp61B.f1=_tmp6EE;}),({void*_tmp6ED=Cyc_Tcutil_compress(t2);_tmp61B.f2=_tmp6ED;});_tmp61B;});struct _tuple14 _tmp29E=_tmp29D;void*_tmp2A4;void*_tmp2A3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E.f2)->tag == 0U){_LL1: _tmp2A4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E.f1)->f1;_tmp2A3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E.f2)->f1;_LL2:
# 1445
{struct _tuple14 _tmp29F=({struct _tuple14 _tmp61A;_tmp61A.f1=_tmp2A4,_tmp61A.f2=_tmp2A3;_tmp61A;});struct _tuple14 _tmp2A0=_tmp29F;int _tmp2A2;int _tmp2A1;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 2U){_LL6: _tmp2A2=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f1;_tmp2A1=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f1;_LL7:
# 1447
 if(_tmp2A2 != 0 && _tmp2A2 != 1)return t1;else{
if(_tmp2A1 != 0 && _tmp2A1 != 1)return t2;else{
if(_tmp2A2 >= _tmp2A1)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1457
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1462
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1455
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1464
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1454
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1458
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A0.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1463
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1456
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A0.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1465
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A0.f1)->tag == 4U){_LL18: _LL19:
# 1460
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1466
 goto _LL5;}}}}}}_LL5:;}
# 1468
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1471
return Cyc_Absyn_sint_type;}
# 1476
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 1479
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 ||
 Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5->v=t1;_tmp2A5;});}}}
# 1488
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1492
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp2A8=({struct Cyc_String_pa_PrintArg_struct _tmp61D;_tmp61D.tag=0U,({
struct _fat_ptr _tmp6EF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp61D.f1=_tmp6EF;});_tmp61D;});struct Cyc_String_pa_PrintArg_struct _tmp2A9=({struct Cyc_String_pa_PrintArg_struct _tmp61C;_tmp61C.tag=0U,({struct _fat_ptr _tmp6F0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp61C.f1=_tmp6F0;});_tmp61C;});void*_tmp2A6[2U];_tmp2A6[0]=& _tmp2A8,_tmp2A6[1]=& _tmp2A9;({unsigned _tmp6F2=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _fat_ptr _tmp6F1=({const char*_tmp2A7="type mismatch: expecting %s but found %s";_tag_fat(_tmp2A7,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp6F2,_tmp6F1,_tag_fat(_tmp2A6,sizeof(void*),2U));});});
return 0;}}}
# 1498
return 1;}
# 1503
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(te,e)){
void*_tmp2AA=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2AB=_tmp2AA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AB)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1509
return 1;}
# 1513
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 1517
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp2AC=0U;({unsigned _tmp6F4=e->loc;struct _fat_ptr _tmp6F3=({const char*_tmp2AD="integral size mismatch; conversion supplied";_tag_fat(_tmp2AD,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6F4,_tmp6F3,_tag_fat(_tmp2AC,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1523
return 0;}
# 1527
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 1531
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp2AE=0U;({unsigned _tmp6F6=e->loc;struct _fat_ptr _tmp6F5=({const char*_tmp2AF="integral size mismatch; conversion supplied";_tag_fat(_tmp2AF,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6F6,_tmp6F5,_tag_fat(_tmp2AE,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1537
return 0;}
# 1542
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1548
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp6F7=t;Cyc_Unify_unify(_tmp6F7,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1553
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2B0=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp2B1=_tmp2B0;struct Cyc_Absyn_Exp*_tmp2B3;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B1)->tag == 9U){_LL1: _tmp2B3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B1)->f1;_LL2:
 return _tmp2B3;}else{_LL3: _LL4: {
# 1558
struct Cyc_Absyn_Exp*_tmp2B2=Cyc_Absyn_valueof_exp(_tmp2B0,0U);
_tmp2B2->topt=Cyc_Absyn_uint_type;
return _tmp2B2;}}_LL0:;}
# 1567
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B4=Cyc_Tcutil_compress(b);void*_tmp2B5=_tmp2B4;struct Cyc_List_List*_tmp2B9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B5)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B5)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2B9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B5)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp2B9);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp2B8=({struct Cyc_String_pa_PrintArg_struct _tmp61E;_tmp61E.tag=0U,({struct _fat_ptr _tmp6F8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp61E.f1=_tmp6F8;});_tmp61E;});void*_tmp2B6[1U];_tmp2B6[0]=& _tmp2B8;({struct _fat_ptr _tmp6F9=({const char*_tmp2B7="get_bounds_exp: %s";_tag_fat(_tmp2B7,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F9,_tag_fat(_tmp2B6,sizeof(void*),1U));});});}_LL0:;};}
# 1576
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2BA=Cyc_Tcutil_compress(t);void*_tmp2BB=_tmp2BA;void*_tmp2BF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BB)->tag == 3U){_LL1: _tmp2BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BB)->f1).ptr_atts).bounds;_LL2:
# 1579
 return Cyc_Tcutil_get_bounds_exp(def,_tmp2BF);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2BE=({struct Cyc_String_pa_PrintArg_struct _tmp61F;_tmp61F.tag=0U,({struct _fat_ptr _tmp6FA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp61F.f1=_tmp6FA;});_tmp61F;});void*_tmp2BC[1U];_tmp2BC[0]=& _tmp2BE;({struct _fat_ptr _tmp6FB=({const char*_tmp2BD="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp2BD,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6FB,_tag_fat(_tmp2BC,sizeof(void*),1U));});});}_LL0:;}
# 1585
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2C0=_cycalloc(sizeof(*_tmp2C0));({struct Cyc_List_List*_tmp6FC=Cyc_Tcenv_lookup_type_vars(*tep);_tmp2C0->v=_tmp6FC;});_tmp2C0;}));else{
# 1589
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1->v=0;_tmp2C1;}));}}
# 1592
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));({struct Cyc_List_List*_tmp6FD=Cyc_Tcenv_lookup_type_vars(*tep);_tmp2C2->v=_tmp6FD;});_tmp2C2;}));else{
# 1596
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));_tmp2C3->v=0;_tmp2C3;}));}}
# 1600
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple21{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1608
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple14 _tmp2C4=({struct _tuple14 _tmp623;_tmp623.f1=t1,_tmp623.f2=t2;_tmp623;});struct _tuple14 _tmp2C5=_tmp2C4;void*_tmp2DA;struct Cyc_Absyn_Tqual _tmp2D9;struct Cyc_Absyn_Exp*_tmp2D8;void*_tmp2D7;void*_tmp2D6;struct Cyc_Absyn_Tqual _tmp2D5;struct Cyc_Absyn_Exp*_tmp2D4;void*_tmp2D3;struct Cyc_Absyn_PtrInfo _tmp2D2;struct Cyc_Absyn_PtrInfo _tmp2D1;switch(*((int*)_tmp2C5.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5.f2)->tag == 3U){_LL1: _tmp2D2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5.f1)->f1;_tmp2D1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5.f2)->f1;_LL2: {
# 1613
int okay=1;
# 1615
if(!Cyc_Unify_unify((_tmp2D2.ptr_atts).nullable,(_tmp2D1.ptr_atts).nullable))
# 1617
okay=!Cyc_Tcutil_force_type2bool(0,(_tmp2D2.ptr_atts).nullable);
# 1619
if(!Cyc_Unify_unify((_tmp2D2.ptr_atts).bounds,(_tmp2D1.ptr_atts).bounds)){
struct _tuple21 _tmp2C6=({struct _tuple21 _tmp620;({struct Cyc_Absyn_Exp*_tmp701=({void*_tmp700=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp700,(_tmp2D2.ptr_atts).bounds);});_tmp620.f1=_tmp701;}),({
struct Cyc_Absyn_Exp*_tmp6FF=({void*_tmp6FE=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6FE,(_tmp2D1.ptr_atts).bounds);});_tmp620.f2=_tmp6FF;});_tmp620;});
# 1620
struct _tuple21 _tmp2C7=_tmp2C6;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;if(_tmp2C7.f2 == 0){_LLA: _LLB:
# 1623
 okay=1;goto _LL9;}else{if(_tmp2C7.f1 == 0){_LLC: _LLD:
# 1626
 if(Cyc_Tcutil_force_type2bool(0,(_tmp2D2.ptr_atts).zero_term)&&({
void*_tmp702=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp702,(_tmp2D1.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp2CB=_tmp2C7.f1;_tmp2CA=_tmp2C7.f2;_LLF:
# 1632
 okay=okay &&({struct Cyc_Absyn_Exp*_tmp703=(struct Cyc_Absyn_Exp*)_check_null(_tmp2CA);Cyc_Evexp_lte_const_exp(_tmp703,(struct Cyc_Absyn_Exp*)_check_null(_tmp2CB));});
# 1636
if(!Cyc_Tcutil_force_type2bool(0,(_tmp2D1.ptr_atts).zero_term))
({void*_tmp2C8=0U;({unsigned _tmp705=loc;struct _fat_ptr _tmp704=({const char*_tmp2C9="implicit cast to shorter array";_tag_fat(_tmp2C9,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp705,_tmp704,_tag_fat(_tmp2C8,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 1643
okay=okay &&(!(_tmp2D2.elt_tq).real_const ||(_tmp2D1.elt_tq).real_const);
# 1646
if(!Cyc_Unify_unify((_tmp2D2.ptr_atts).rgn,(_tmp2D1.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp2D2.ptr_atts).rgn,(_tmp2D1.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp2CE=({struct Cyc_String_pa_PrintArg_struct _tmp622;_tmp622.tag=0U,({
struct _fat_ptr _tmp706=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((_tmp2D2.ptr_atts).rgn));_tmp622.f1=_tmp706;});_tmp622;});struct Cyc_String_pa_PrintArg_struct _tmp2CF=({struct Cyc_String_pa_PrintArg_struct _tmp621;_tmp621.tag=0U,({
struct _fat_ptr _tmp707=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((_tmp2D1.ptr_atts).rgn));_tmp621.f1=_tmp707;});_tmp621;});void*_tmp2CC[2U];_tmp2CC[0]=& _tmp2CE,_tmp2CC[1]=& _tmp2CF;({unsigned _tmp709=loc;struct _fat_ptr _tmp708=({const char*_tmp2CD="implicit cast from region %s to region %s";_tag_fat(_tmp2CD,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp709,_tmp708,_tag_fat(_tmp2CC,sizeof(void*),2U));});});}else{
okay=0;}}
# 1656
okay=okay &&(Cyc_Unify_unify((_tmp2D2.ptr_atts).zero_term,(_tmp2D1.ptr_atts).zero_term)||
# 1658
 Cyc_Tcutil_force_type2bool(1,(_tmp2D2.ptr_atts).zero_term)&&(_tmp2D1.elt_tq).real_const);{
# 1666
int _tmp2D0=
({void*_tmp70A=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp70A,(_tmp2D1.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(_tmp2D1.ptr_atts).zero_term);
# 1672
okay=okay &&(Cyc_Unify_unify(_tmp2D2.elt_type,_tmp2D1.elt_type)||
(_tmp2D0 &&((_tmp2D1.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp2D1.elt_type))))&& Cyc_Tcutil_ptrsubtype(te,0,_tmp2D2.elt_type,_tmp2D1.elt_type));
# 1675
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->tag == 4U){_LL3: _tmp2DA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f1)->f1).elt_type;_tmp2D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f1)->f1).tq;_tmp2D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f1)->f1).num_elts;_tmp2D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f1)->f1).zero_term;_tmp2D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->f1).elt_type;_tmp2D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->f1).tq;_tmp2D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->f1).num_elts;_tmp2D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->f1).zero_term;_LL4: {
# 1679
int okay;
# 1682
okay=Cyc_Unify_unify(_tmp2D7,_tmp2D3)&&(
(_tmp2D8 != 0 && _tmp2D4 != 0)&& Cyc_Evexp_same_const_exp(_tmp2D8,_tmp2D4));
# 1685
return(okay && Cyc_Unify_unify(_tmp2DA,_tmp2D6))&&(!_tmp2D9.real_const || _tmp2D5.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C5.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C5.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C5.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1687
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1689
 return Cyc_Unify_unify(t1,t2);}_LL0:;};}
# 1693
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2DB=Cyc_Tcutil_compress(t);void*_tmp2DC=_tmp2DB;void*_tmp2DF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->tag == 3U){_LL1: _tmp2DF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).elt_type;_LL2:
 return _tmp2DF;}else{_LL3: _LL4:
({void*_tmp2DD=0U;({struct _fat_ptr _tmp70B=({const char*_tmp2DE="pointer_elt_type";_tag_fat(_tmp2DE,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp70B,_tag_fat(_tmp2DD,sizeof(void*),0U));});});}_LL0:;}
# 1699
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2E0=Cyc_Tcutil_compress(t);void*_tmp2E1=_tmp2E0;struct Cyc_Absyn_PtrAtts*_tmp2E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->tag == 3U){_LL1: _tmp2E4=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts;_LL2:
 return _tmp2E4->rgn;}else{_LL3: _LL4:
({void*_tmp2E2=0U;({struct _fat_ptr _tmp70C=({const char*_tmp2E3="pointer_elt_type";_tag_fat(_tmp2E3,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp70C,_tag_fat(_tmp2E2,sizeof(void*),0U));});});}_LL0:;}
# 1706
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2E5=Cyc_Tcutil_compress(t);void*_tmp2E6=_tmp2E5;void*_tmp2E7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6)->tag == 3U){_LL1: _tmp2E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6)->f1).ptr_atts).rgn;_LL2:
# 1709
*rgn=_tmp2E7;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1718
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2E8=Cyc_Tcutil_compress(t);void*_tmp2E9=_tmp2E8;void*_tmp2EB;void*_tmp2EA;switch(*((int*)_tmp2E9)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E9)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9)->f1).ptr_atts).nullable;_tmp2EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9)->f1).ptr_atts).bounds;_LL6:
# 1726
 return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,_tmp2EA)&& Cyc_Tcutil_force_type2bool(0,_tmp2EB);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1732
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2EC=e->r;void*_tmp2ED=_tmp2EC;void*_tmp2F1;struct Cyc_Absyn_Exp*_tmp2F0;struct _fat_ptr _tmp2EF;switch(*((int*)_tmp2ED)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2EF=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Wchar_c).val;_LLC: {
# 1741
unsigned long _tmp2EE=Cyc_strlen((struct _fat_ptr)_tmp2EF);
int i=0;
if(_tmp2EE >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(_tmp2EF,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(_tmp2EF,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)_tmp2EF.curr)[1]== (int)'x' && _tmp2EE >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(_tmp2EF,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp2EE;++ i){
if((int)*((const char*)_check_fat_subscript(_tmp2EF,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 1751
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1739
 return 1;case 14U: _LLD: _tmp2F1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1;_tmp2F0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2ED)->f2;_LLE:
# 1752
 return Cyc_Tcutil_is_zero(_tmp2F0)&& Cyc_Tcutil_admits_zero(_tmp2F1);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1757
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1766
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1771
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1776
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1785
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1790
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1795
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2F2=ka;enum Cyc_Absyn_KindQual _tmp2FB;enum Cyc_Absyn_AliasQual _tmp2FA;_LL1: _tmp2FB=_tmp2F2->kind;_tmp2FA=_tmp2F2->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp2F3=_tmp2FA;switch(_tmp2F3){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1799
enum Cyc_Absyn_KindQual _tmp2F4=_tmp2FB;switch(_tmp2F4){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1810
{enum Cyc_Absyn_KindQual _tmp2F5=_tmp2FB;switch(_tmp2F5){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1817
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1819
{enum Cyc_Absyn_KindQual _tmp2F6=_tmp2FB;switch(_tmp2F6){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1826
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1829
({struct Cyc_String_pa_PrintArg_struct _tmp2F9=({struct Cyc_String_pa_PrintArg_struct _tmp624;_tmp624.tag=0U,({struct _fat_ptr _tmp70D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp624.f1=_tmp70D;});_tmp624;});void*_tmp2F7[1U];_tmp2F7[0]=& _tmp2F9;({struct _fat_ptr _tmp70E=({const char*_tmp2F8="kind_to_opt: bad kind %s\n";_tag_fat(_tmp2F8,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp70E,_tag_fat(_tmp2F7,sizeof(void*),1U));});});}
# 1832
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->tag=0U,_tmp2FC->f1=k;_tmp2FC;});}
# 1835
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));({void*_tmp70F=Cyc_Tcutil_kind_to_bound(k);_tmp2FD->v=_tmp70F;});_tmp2FD;});}
# 1841
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2)&& Cyc_Tcutil_is_zero(e1)){
({void*_tmp710=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp710;});{
struct Cyc_Core_Opt*_tmp2FE=Cyc_Tcenv_lookup_opt_type_vars(te);
void*_tmp2FF=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp627;({void*_tmp715=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp2FE);_tmp627.elt_type=_tmp715;}),({
struct Cyc_Absyn_Tqual _tmp714=Cyc_Absyn_empty_tqual(0U);_tmp627.elt_tq=_tmp714;}),
({void*_tmp713=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp2FE);(_tmp627.ptr_atts).rgn=_tmp713;}),(_tmp627.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 1849
void*_tmp712=Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,_tmp2FE);(_tmp627.ptr_atts).bounds=_tmp712;}),({
void*_tmp711=Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,_tmp2FE);(_tmp627.ptr_atts).zero_term=_tmp711;}),(_tmp627.ptr_atts).ptrloc=0;_tmp627;}));
e1->topt=_tmp2FF;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp302=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({
struct _fat_ptr _tmp716=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp626.f1=_tmp716;});_tmp626;});struct Cyc_String_pa_PrintArg_struct _tmp303=({struct Cyc_String_pa_PrintArg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp717=(struct _fat_ptr)((struct _fat_ptr)Cyc_Position_string_of_segment(e1->loc));_tmp625.f1=_tmp717;});_tmp625;});void*_tmp300[2U];_tmp300[0]=& _tmp302,_tmp300[1]=& _tmp303;({struct _fat_ptr _tmp718=({const char*_tmp301="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_fat(_tmp301,sizeof(char),56U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp718,_tag_fat(_tmp300,sizeof(void*),2U));});});
return retv;};};}
# 1859
return 0;}
# 1862
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple22{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1868
struct _tuple22 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1872
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp71B=({struct _fat_ptr*_tmp316=_cycalloc(sizeof(*_tmp316));({struct _fat_ptr _tmp71A=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp315=({struct Cyc_Int_pa_PrintArg_struct _tmp629;_tmp629.tag=1U,_tmp629.f1=(unsigned long)counter ++;_tmp629;});void*_tmp313[1U];_tmp313[0]=& _tmp315;({struct _fat_ptr _tmp719=({const char*_tmp314="__aliasvar%d";_tag_fat(_tmp314,sizeof(char),13U);});Cyc_aprintf(_tmp719,_tag_fat(_tmp313,sizeof(void*),1U));});});*_tmp316=_tmp71A;});_tmp316;});_tmp317->f2=_tmp71B;});_tmp317;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp71C=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp312=_cycalloc(sizeof(*_tmp312));_tmp312->tag=4U,_tmp312->f1=vd;_tmp312;});Cyc_Absyn_varb_exp(_tmp71C,e->loc);});
# 1881
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 1883
{void*_tmp304=Cyc_Tcutil_compress(e_type);void*_tmp305=_tmp304;void*_tmp311;struct Cyc_Absyn_Tqual _tmp310;void*_tmp30F;void*_tmp30E;void*_tmp30D;void*_tmp30C;struct Cyc_Absyn_PtrLoc*_tmp30B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->tag == 3U){_LL1: _tmp311=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).elt_type;_tmp310=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).elt_tq;_tmp30F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).ptr_atts).rgn;_tmp30E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).ptr_atts).nullable;_tmp30D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).ptr_atts).bounds;_tmp30C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).ptr_atts).zero_term;_tmp30B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp305)->f1).ptr_atts).ptrloc;_LL2:
# 1885
{void*_tmp306=Cyc_Tcutil_compress(_tmp30F);void*_tmp307=_tmp306;void**_tmp30A;struct Cyc_Core_Opt*_tmp309;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp307)->tag == 1U){_LL6: _tmp30A=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp307)->f2;_tmp309=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp307)->f4;_LL7: {
# 1887
void*_tmp308=Cyc_Absyn_var_type(tv);
*_tmp30A=_tmp308;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1892
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1896
e->topt=0;
vd->initializer=0;{
# 1900
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1902
return({struct _tuple22 _tmp628;_tmp628.f1=d,_tmp628.f2=ve;_tmp628;});};}
# 1907
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1910
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1915
void*_tmp318=Cyc_Tcutil_compress(wants_type);void*_tmp319=_tmp318;void*_tmp31B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp319)->tag == 3U){_LL1: _tmp31B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp319)->f1).ptr_atts).rgn;_LL2:
# 1917
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp31B))return 0;{
struct Cyc_Absyn_Kind*_tmp31A=Cyc_Tcutil_type_kind(_tmp31B);
return(int)_tmp31A->kind == (int)Cyc_Absyn_RgnKind &&(int)_tmp31A->aliasqual == (int)Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1923
return 0;}
# 1927
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1932
if(Cyc_Unify_unify(t1,t2))return 1;
# 1934
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1936
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp62B;_tmp62B.tag=0U,({
struct _fat_ptr _tmp71D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp62B.f1=_tmp71D;});_tmp62B;});struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp62A;_tmp62A.tag=0U,({struct _fat_ptr _tmp71E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp62A.f1=_tmp71E;});_tmp62A;});void*_tmp31C[2U];_tmp31C[0]=& _tmp31E,_tmp31C[1]=& _tmp31F;({unsigned _tmp720=e->loc;struct _fat_ptr _tmp71F=({const char*_tmp31D="integral size mismatch; %s -> %s conversion supplied";_tag_fat(_tmp31D,sizeof(char),53U);});Cyc_Tcutil_warn(_tmp720,_tmp71F,_tag_fat(_tmp31C,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 1944
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp322=({struct Cyc_String_pa_PrintArg_struct _tmp62E;_tmp62E.tag=0U,({
struct _fat_ptr _tmp721=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp62E.f1=_tmp721;});_tmp62E;});struct Cyc_String_pa_PrintArg_struct _tmp323=({struct Cyc_String_pa_PrintArg_struct _tmp62D;_tmp62D.tag=0U,({struct _fat_ptr _tmp722=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp62D.f1=_tmp722;});_tmp62D;});struct Cyc_String_pa_PrintArg_struct _tmp324=({struct Cyc_String_pa_PrintArg_struct _tmp62C;_tmp62C.tag=0U,({struct _fat_ptr _tmp723=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp62C.f1=_tmp723;});_tmp62C;});void*_tmp320[3U];_tmp320[0]=& _tmp322,_tmp320[1]=& _tmp323,_tmp320[2]=& _tmp324;({unsigned _tmp725=e->loc;struct _fat_ptr _tmp724=({const char*_tmp321="implicit alias coercion for %s:%s to %s";_tag_fat(_tmp321,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp725,_tmp724,_tag_fat(_tmp320,sizeof(void*),3U));});});
*alias_coercion=1;}
# 1951
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((int)(c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1958
if((int)c != (int)1U)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp327=({struct Cyc_String_pa_PrintArg_struct _tmp630;_tmp630.tag=0U,({
struct _fat_ptr _tmp726=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp630.f1=_tmp726;});_tmp630;});struct Cyc_String_pa_PrintArg_struct _tmp328=({struct Cyc_String_pa_PrintArg_struct _tmp62F;_tmp62F.tag=0U,({struct _fat_ptr _tmp727=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp62F.f1=_tmp727;});_tmp62F;});void*_tmp325[2U];_tmp325[0]=& _tmp327,_tmp325[1]=& _tmp328;({unsigned _tmp729=e->loc;struct _fat_ptr _tmp728=({const char*_tmp326="implicit cast from %s to %s";_tag_fat(_tmp326,sizeof(char),28U);});Cyc_Tcutil_warn(_tmp729,_tmp728,_tag_fat(_tmp325,sizeof(void*),2U));});});
return 1;}else{
# 1964
return 0;}}}}}
# 1971
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 1974
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 1988 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple23{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 1992
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple23*env,struct Cyc_Absyn_Aggrfield*x){
# 1995
struct _tuple23 _tmp329=*env;struct _tuple23 _tmp32A=_tmp329;struct Cyc_List_List*_tmp332;struct _RegionHandle*_tmp331;struct Cyc_Tcenv_Tenv*_tmp330;int _tmp32F;_LL1: _tmp332=_tmp32A.f1;_tmp331=_tmp32A.f2;_tmp330=_tmp32A.f3;_tmp32F=_tmp32A.f4;_LL2:;{
# 1997
void*_tmp32B=_tmp332 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp331,_tmp332,x->type);
struct Cyc_List_List*_tmp32C=Cyc_Tcutil_flatten_type(_tmp331,_tmp32F,_tmp330,_tmp32B);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp32C)== 1)
return({struct Cyc_List_List*_tmp32E=_region_malloc(_tmp331,sizeof(*_tmp32E));({struct _tuple11*_tmp72A=({struct _tuple11*_tmp32D=_region_malloc(_tmp331,sizeof(*_tmp32D));_tmp32D->f1=x->tq,_tmp32D->f2=_tmp32B;_tmp32D;});_tmp32E->hd=_tmp72A;}),_tmp32E->tl=0;_tmp32E;});else{
return _tmp32C;}};}struct _tuple24{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2003
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple11*x){
# 2005
struct _tuple24 _tmp333=*env;struct _tuple24 _tmp334=_tmp333;struct _RegionHandle*_tmp33E;struct Cyc_Tcenv_Tenv*_tmp33D;int _tmp33C;_LL1: _tmp33E=_tmp334.f1;_tmp33D=_tmp334.f2;_tmp33C=_tmp334.f3;_LL2:;{
struct _tuple11 _tmp335=*x;struct _tuple11 _tmp336=_tmp335;struct Cyc_Absyn_Tqual _tmp33B;void*_tmp33A;_LL4: _tmp33B=_tmp336.f1;_tmp33A=_tmp336.f2;_LL5:;{
struct Cyc_List_List*_tmp337=Cyc_Tcutil_flatten_type(_tmp33E,_tmp33C,_tmp33D,_tmp33A);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp337)== 1)
return({struct Cyc_List_List*_tmp339=_region_malloc(_tmp33E,sizeof(*_tmp339));({struct _tuple11*_tmp72B=({struct _tuple11*_tmp338=_region_malloc(_tmp33E,sizeof(*_tmp338));_tmp338->f1=_tmp33B,_tmp338->f2=_tmp33A;_tmp338;});_tmp339->hd=_tmp72B;}),_tmp339->tl=0;_tmp339;});else{
return _tmp337;}};};}
# 2012
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2016
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp33F=t1;struct Cyc_List_List*_tmp35E;struct Cyc_List_List*_tmp35D;struct Cyc_Absyn_Aggrdecl*_tmp35C;struct Cyc_List_List*_tmp35B;switch(*((int*)_tmp33F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp35C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f1)->f1).KnownAggr).val;_tmp35B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f2;_LL6:
# 2034
 if((((int)_tmp35C->kind == (int)Cyc_Absyn_UnionA || _tmp35C->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp35C->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp35C->impl))->rgn_po != 0)
# 2036
return({struct Cyc_List_List*_tmp34A=_region_malloc(r,sizeof(*_tmp34A));({struct _tuple11*_tmp72D=({struct _tuple11*_tmp349=_region_malloc(r,sizeof(*_tmp349));({struct Cyc_Absyn_Tqual _tmp72C=Cyc_Absyn_empty_tqual(0U);_tmp349->f1=_tmp72C;}),_tmp349->f2=t1;_tmp349;});_tmp34A->hd=_tmp72D;}),_tmp34A->tl=0;_tmp34A;});{
struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp35C->tvs,_tmp35B);
struct _tuple23 env=({struct _tuple23 _tmp631;_tmp631.f1=_tmp34B,_tmp631.f2=r,_tmp631.f3=te,_tmp631.f4=flatten;_tmp631;});
struct Cyc_List_List*_tmp34C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp35C->impl))->fields;struct Cyc_List_List*_tmp34D=_tmp34C;struct Cyc_Absyn_Aggrfield*_tmp353;struct Cyc_List_List*_tmp352;if(_tmp34D == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp353=(struct Cyc_Absyn_Aggrfield*)_tmp34D->hd;_tmp352=_tmp34D->tl;_LL14: {
# 2042
struct Cyc_List_List*_tmp34E=Cyc_Tcutil_flatten_type_f(& env,_tmp353);
env.f4=0;{
struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp352);
struct Cyc_List_List*_tmp350=({struct Cyc_List_List*_tmp351=_region_malloc(r,sizeof(*_tmp351));_tmp351->hd=_tmp34E,_tmp351->tl=_tmp34F;_tmp351;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp350);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp35D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp33F)->f1;_LL4: {
# 2021
struct _tuple24 _tmp340=({struct _tuple24 _tmp632;_tmp632.f1=r,_tmp632.f2=te,_tmp632.f3=flatten;_tmp632;});
# 2023
struct Cyc_List_List*_tmp341=_tmp35D;struct _tuple11*_tmp348;struct Cyc_List_List*_tmp347;if(_tmp341 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp348=(struct _tuple11*)_tmp341->hd;_tmp347=_tmp341->tl;_LLF: {
# 2026
struct Cyc_List_List*_tmp342=Cyc_Tcutil_rcopy_tqt(& _tmp340,_tmp348);
_tmp340.f3=0;{
struct Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple11*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp340,_tmp35D);
struct Cyc_List_List*_tmp344=({struct Cyc_List_List*_tmp346=_region_malloc(r,sizeof(*_tmp346));_tmp346->hd=_tmp342,_tmp346->tl=_tmp343;_tmp346;});
return({struct _RegionHandle*_tmp72E=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp72E,({struct Cyc_List_List*_tmp345=_region_malloc(r,sizeof(*_tmp345));_tmp345->hd=_tmp342,_tmp345->tl=_tmp343;_tmp345;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33F)->f1 == Cyc_Absyn_StructA){_LL7: _tmp35E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33F)->f2;_LL8: {
# 2049
struct _tuple23 env=({struct _tuple23 _tmp633;_tmp633.f1=0,_tmp633.f2=r,_tmp633.f3=te,_tmp633.f4=flatten;_tmp633;});
struct Cyc_List_List*_tmp354=_tmp35E;struct Cyc_Absyn_Aggrfield*_tmp35A;struct Cyc_List_List*_tmp359;if(_tmp354 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp35A=(struct Cyc_Absyn_Aggrfield*)_tmp354->hd;_tmp359=_tmp354->tl;_LL19: {
# 2053
struct Cyc_List_List*_tmp355=Cyc_Tcutil_flatten_type_f(& env,_tmp35A);
env.f4=0;{
struct Cyc_List_List*_tmp356=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp359);
struct Cyc_List_List*_tmp357=({struct Cyc_List_List*_tmp358=_region_malloc(r,sizeof(*_tmp358));_tmp358->hd=_tmp355,_tmp358->tl=_tmp356;_tmp358;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp357);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2059
 goto _LL0;}_LL0:;};}
# 2062
return({struct Cyc_List_List*_tmp360=_region_malloc(r,sizeof(*_tmp360));({struct _tuple11*_tmp730=({struct _tuple11*_tmp35F=_region_malloc(r,sizeof(*_tmp35F));({struct Cyc_Absyn_Tqual _tmp72F=Cyc_Absyn_empty_tqual(0U);_tmp35F->f1=_tmp72F;}),_tmp35F->f2=t1;_tmp35F;});_tmp360->hd=_tmp730;}),_tmp360->tl=0;_tmp360;});}
# 2066
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp361=(void*)t->hd;void*_tmp362=_tmp361;switch(*((int*)_tmp362)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
# 2073
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2076
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2079
return 1;}
# 2082
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple14 _tmp363=({struct _tuple14 _tmp634;({void*_tmp732=Cyc_Tcutil_compress(t1);_tmp634.f1=_tmp732;}),({void*_tmp731=Cyc_Tcutil_compress(t2);_tmp634.f2=_tmp731;});_tmp634;});struct _tuple14 _tmp364=_tmp363;enum Cyc_Absyn_Size_of _tmp366;enum Cyc_Absyn_Size_of _tmp365;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f2)->f1)->tag == 1U){_LL1: _tmp366=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f1)->f1)->f2;_tmp365=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f2)->f1)->f2;_LL2:
# 2085
 return((int)_tmp366 == (int)_tmp365 ||(int)_tmp366 == (int)2U &&(int)_tmp365 == (int)3U)||
(int)_tmp366 == (int)3U &&(int)_tmp365 == (int)Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2093
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2096
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple14*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple14*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple14 _tmp367=({struct _tuple14 _tmp635;_tmp635.f1=t1,_tmp635.f2=t2;_tmp635;});struct _tuple14 _tmp368=_tmp367;struct Cyc_Absyn_FnInfo _tmp392;struct Cyc_Absyn_FnInfo _tmp391;struct Cyc_Absyn_Datatypedecl*_tmp390;struct Cyc_Absyn_Datatypefield*_tmp38F;struct Cyc_List_List*_tmp38E;struct Cyc_Absyn_Datatypedecl*_tmp38D;struct Cyc_List_List*_tmp38C;void*_tmp38B;struct Cyc_Absyn_Tqual _tmp38A;void*_tmp389;void*_tmp388;void*_tmp387;void*_tmp386;void*_tmp385;struct Cyc_Absyn_Tqual _tmp384;void*_tmp383;void*_tmp382;void*_tmp381;void*_tmp380;switch(*((int*)_tmp368.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->tag == 3U){_LL1: _tmp38B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).elt_type;_tmp38A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).elt_tq;_tmp389=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).ptr_atts).rgn;_tmp388=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).ptr_atts).nullable;_tmp387=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).ptr_atts).bounds;_tmp386=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).ptr_atts).zero_term;_tmp385=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).elt_type;_tmp384=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).elt_tq;_tmp383=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).ptr_atts).rgn;_tmp382=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).ptr_atts).nullable;_tmp381=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).ptr_atts).bounds;_tmp380=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).ptr_atts).zero_term;_LL2:
# 2108
 if(_tmp38A.real_const && !_tmp384.real_const)
return 0;
# 2111
if((!Cyc_Unify_unify(_tmp388,_tmp382)&&
 Cyc_Absyn_type2bool(0,_tmp388))&& !Cyc_Absyn_type2bool(0,_tmp382))
return 0;
# 2115
if((Cyc_Unify_unify(_tmp386,_tmp380)&& !
Cyc_Absyn_type2bool(0,_tmp386))&& Cyc_Absyn_type2bool(0,_tmp380))
return 0;
# 2119
if((!Cyc_Unify_unify(_tmp389,_tmp383)&& !Cyc_Tcenv_region_outlives(te,_tmp389,_tmp383))&& !
Cyc_Tcutil_subtype(te,assume,_tmp389,_tmp383))
return 0;
# 2123
if(!Cyc_Unify_unify(_tmp387,_tmp381)){
struct Cyc_Absyn_Exp*_tmp369=({void*_tmp733=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp733,_tmp387);});
struct Cyc_Absyn_Exp*_tmp36A=({void*_tmp734=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp734,_tmp381);});
if(_tmp369 != _tmp36A){
if((_tmp369 == 0 || _tmp36A == 0)|| !Cyc_Evexp_lte_const_exp(_tmp36A,_tmp36A))
return 0;}}
# 2133
if(!_tmp384.real_const && _tmp38A.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp385)))
return 0;}{
# 2139
int _tmp36B=
({void*_tmp735=_tmp381;Cyc_Unify_unify(_tmp735,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,_tmp380);
# 2144
return(_tmp36B &&({struct Cyc_Tcenv_Tenv*_tmp739=te;struct Cyc_List_List*_tmp738=({struct Cyc_List_List*_tmp36D=_cycalloc(sizeof(*_tmp36D));({struct _tuple14*_tmp736=({struct _tuple14*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->f1=t1,_tmp36C->f2=t2;_tmp36C;});_tmp36D->hd=_tmp736;}),_tmp36D->tl=assume;_tmp36D;});void*_tmp737=_tmp38B;Cyc_Tcutil_ptrsubtype(_tmp739,_tmp738,_tmp737,_tmp385);})|| Cyc_Unify_unify(_tmp38B,_tmp385))|| Cyc_Tcutil_isomorphic(_tmp38B,_tmp385);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp390=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp38F=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp38E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f2;_tmp38D=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->f1)->f1).KnownDatatype).val;_tmp38C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->f2;_LL4:
# 2151
 if(_tmp390 != _tmp38D && Cyc_Absyn_qvar_cmp(_tmp390->name,_tmp38D->name)!= 0)return 0;
# 2153
if(({int _tmp73A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38E);_tmp73A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp38C);}))return 0;
for(0;_tmp38E != 0;(_tmp38E=_tmp38E->tl,_tmp38C=_tmp38C->tl)){
if(!Cyc_Unify_unify((void*)_tmp38E->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp38C))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp368.f2)->tag == 5U){_LL5: _tmp392=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp368.f1)->f1;_tmp391=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp368.f2)->f1;_LL6:
# 2160
 if(_tmp392.tvars != 0 || _tmp391.tvars != 0){
struct Cyc_List_List*_tmp36E=_tmp392.tvars;
struct Cyc_List_List*_tmp36F=_tmp391.tvars;
if(({int _tmp73B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp36E);_tmp73B != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp36F);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp36E != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp36E->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp36F))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp371=_cycalloc(sizeof(*_tmp371));({struct _tuple18*_tmp73D=({struct _tuple18*_tmp370=_cycalloc(sizeof(*_tmp370));_tmp370->f1=(struct Cyc_Absyn_Tvar*)_tmp36F->hd,({void*_tmp73C=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp36E->hd);_tmp370->f2=_tmp73C;});_tmp370;});_tmp371->hd=_tmp73D;}),_tmp371->tl=inst;_tmp371;});
_tmp36E=_tmp36E->tl;
_tmp36F=_tmp36F->tl;}
# 2171
if(inst != 0){
_tmp392.tvars=0;
_tmp391.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmp740=te;struct Cyc_List_List*_tmp73F=assume;void*_tmp73E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp372=_cycalloc(sizeof(*_tmp372));_tmp372->tag=5U,_tmp372->f1=_tmp392;_tmp372;});Cyc_Tcutil_subtype(_tmp740,_tmp73F,_tmp73E,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp373=_cycalloc(sizeof(*_tmp373));_tmp373->tag=5U,_tmp373->f1=_tmp391;_tmp373;}));});}};}
# 2178
if(!Cyc_Tcutil_subtype(te,assume,_tmp392.ret_type,_tmp391.ret_type))return 0;{
struct Cyc_List_List*_tmp374=_tmp392.args;
struct Cyc_List_List*_tmp375=_tmp391.args;
# 2183
if(({int _tmp741=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp374);_tmp741 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp375);}))return 0;
# 2185
for(0;_tmp374 != 0;(_tmp374=_tmp374->tl,_tmp375=_tmp375->tl)){
struct _tuple9 _tmp376=*((struct _tuple9*)_tmp374->hd);struct _tuple9 _tmp377=_tmp376;struct Cyc_Absyn_Tqual _tmp37D;void*_tmp37C;_LLA: _tmp37D=_tmp377.f2;_tmp37C=_tmp377.f3;_LLB:;{
struct _tuple9 _tmp378=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp375))->hd);struct _tuple9 _tmp379=_tmp378;struct Cyc_Absyn_Tqual _tmp37B;void*_tmp37A;_LLD: _tmp37B=_tmp379.f2;_tmp37A=_tmp379.f3;_LLE:;
# 2189
if(_tmp37B.real_const && !_tmp37D.real_const || !Cyc_Tcutil_subtype(te,assume,_tmp37A,_tmp37C))
return 0;};}
# 2193
if(_tmp392.c_varargs != _tmp391.c_varargs)return 0;
if(_tmp392.cyc_varargs != 0 && _tmp391.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp37E=*_tmp392.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp37F=*_tmp391.cyc_varargs;
# 2198
if((_tmp37F.tq).real_const && !(_tmp37E.tq).real_const || !
Cyc_Tcutil_subtype(te,assume,_tmp37F.type,_tmp37E.type))
return 0;}else{
if(_tmp392.cyc_varargs != 0 || _tmp391.cyc_varargs != 0)return 0;}
# 2203
if(!({void*_tmp742=(void*)_check_null(_tmp392.effect);Cyc_Tcutil_subset_effect(1,_tmp742,(void*)_check_null(_tmp391.effect));}))return 0;
# 2205
if(!Cyc_Tcutil_sub_rgnpo(_tmp392.rgn_po,_tmp391.rgn_po))return 0;
# 2207
if(!Cyc_Tcutil_sub_attributes(_tmp392.attributes,_tmp391.attributes))return 0;
# 2209
if(!Cyc_Tcutil_check_logical_implication(_tmp391.requires_relns,_tmp392.requires_relns))
return 0;
# 2212
if(!Cyc_Tcutil_check_logical_implication(_tmp392.ensures_relns,_tmp391.ensures_relns))
return 0;
# 2215
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2227 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2229
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple11*_tmp393=(struct _tuple11*)tqs1->hd;struct _tuple11*_tmp394=_tmp393;struct Cyc_Absyn_Tqual _tmp39A;void*_tmp399;_LL1: _tmp39A=_tmp394->f1;_tmp399=_tmp394->f2;_LL2:;{
struct _tuple11*_tmp395=(struct _tuple11*)tqs2->hd;struct _tuple11*_tmp396=_tmp395;struct Cyc_Absyn_Tqual _tmp398;void*_tmp397;_LL4: _tmp398=_tmp396->f1;_tmp397=_tmp396->f2;_LL5:;
# 2236
if(_tmp39A.real_const && !_tmp398.real_const)return 0;
# 2238
if((_tmp398.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp397)))&&
 Cyc_Tcutil_subtype(te,assume,_tmp399,_tmp397))
# 2241
continue;
# 2243
if(Cyc_Unify_unify(_tmp399,_tmp397))
# 2245
continue;
# 2247
if(Cyc_Tcutil_isomorphic(_tmp399,_tmp397))
# 2249
continue;
# 2252
return 0;};};}
# 2254
return 1;}
# 2259
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2265
{void*_tmp39B=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2268
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2270
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2274
void*_tmp39C=t1;void*_tmp3C1;struct Cyc_Absyn_Enumdecl*_tmp3C0;void*_tmp3BF;struct Cyc_Absyn_Tqual _tmp3BE;struct Cyc_Absyn_Exp*_tmp3BD;void*_tmp3BC;void*_tmp3BB;struct Cyc_Absyn_Tqual _tmp3BA;void*_tmp3B9;void*_tmp3B8;void*_tmp3B7;void*_tmp3B6;switch(*((int*)_tmp39C)){case 3U: _LLA: _tmp3BB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).elt_type;_tmp3BA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).elt_tq;_tmp3B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).ptr_atts).rgn;_tmp3B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).ptr_atts).nullable;_tmp3B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).ptr_atts).bounds;_tmp3B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).ptr_atts).zero_term;_LLB:
# 2283
{void*_tmp39D=t2;void*_tmp3AC;struct Cyc_Absyn_Tqual _tmp3AB;void*_tmp3AA;void*_tmp3A9;void*_tmp3A8;void*_tmp3A7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39D)->tag == 3U){_LL19: _tmp3AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39D)->f1).elt_type;_tmp3AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39D)->f1).elt_tq;_tmp3AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39D)->f1).ptr_atts).rgn;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39D)->f1).ptr_atts).nullable;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39D)->f1).ptr_atts).bounds;_tmp3A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39D)->f1).ptr_atts).zero_term;_LL1A: {
# 2287
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp39E=({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct _tuple14*_tmp743=({struct _tuple14*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->f1=t1,_tmp3A5->f2=t2;_tmp3A5;});_tmp3A6->hd=_tmp743;}),_tmp3A6->tl=0;_tmp3A6;});
int _tmp39F=_tmp3AB.real_const || !_tmp3BA.real_const;
# 2301 "tcutil.cyc"
int _tmp3A0=
({void*_tmp744=_tmp3A8;Cyc_Unify_unify(_tmp744,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,_tmp3A7);
# 2304
int _tmp3A1=_tmp39F &&(
((_tmp3A0 && Cyc_Tcutil_ptrsubtype(te,_tmp39E,_tmp3BB,_tmp3AC)||
 Cyc_Unify_unify(_tmp3BB,_tmp3AC))|| Cyc_Tcutil_isomorphic(_tmp3BB,_tmp3AC))|| Cyc_Unify_unify(_tmp3AC,Cyc_Absyn_void_type));
# 2308
int zeroterm_ok=Cyc_Unify_unify(_tmp3B6,_tmp3A7)|| !Cyc_Absyn_type2bool(0,_tmp3A7);
# 2310
int _tmp3A2=_tmp3A1?0:((Cyc_Tcutil_is_bits_only_type(_tmp3BB)&& Cyc_Tcutil_is_char_type(_tmp3AC))&& !
Cyc_Tcutil_force_type2bool(0,_tmp3A7))&&(
_tmp3AB.real_const || !_tmp3BA.real_const);
int bounds_ok=Cyc_Unify_unify(_tmp3B7,_tmp3A8);
if(!bounds_ok && !_tmp3A2){
struct Cyc_Absyn_Exp*_tmp3A3=({void*_tmp745=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp745,_tmp3B7);});
struct Cyc_Absyn_Exp*_tmp3A4=({void*_tmp746=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp746,_tmp3A8);});
if((_tmp3A3 != 0 && _tmp3A4 != 0)&& Cyc_Evexp_lte_const_exp(_tmp3A4,_tmp3A3))
bounds_ok=1;else{
if(_tmp3A3 == 0 || _tmp3A4 == 0)
bounds_ok=1;}}{
# 2322
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp3B8);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp3A9);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2329
if(((bounds_ok && zeroterm_ok)&&(_tmp3A1 || _tmp3A2))&&(
Cyc_Unify_unify(_tmp3B9,_tmp3AA)|| Cyc_Tcenv_region_outlives(te,_tmp3B9,_tmp3AA)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2335
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp3BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39C)->f1).elt_type;_tmp3BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39C)->f1).tq;_tmp3BD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39C)->f1).num_elts;_tmp3BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39C)->f1).zero_term;_LLD:
# 2337
{void*_tmp3AD=t2;void*_tmp3B1;struct Cyc_Absyn_Tqual _tmp3B0;struct Cyc_Absyn_Exp*_tmp3AF;void*_tmp3AE;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AD)->tag == 4U){_LL1E: _tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AD)->f1).elt_type;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AD)->f1).tq;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AD)->f1).num_elts;_tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AD)->f1).zero_term;_LL1F: {
# 2339
int okay;
okay=
(((_tmp3BD != 0 && _tmp3AF != 0)&& Cyc_Unify_unify(_tmp3BC,_tmp3AE))&&
 Cyc_Evexp_lte_const_exp(_tmp3AF,_tmp3BD))&&
 Cyc_Evexp_lte_const_exp(_tmp3BD,_tmp3AF);
return
# 2346
(okay && Cyc_Unify_unify(_tmp3BF,_tmp3B1))&&(!_tmp3BE.real_const || _tmp3B0.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2348
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2350
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39C)->f1)){case 15U: _LLE: _tmp3C0=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39C)->f1)->f2;_LLF:
# 2354
{void*_tmp3B2=t2;struct Cyc_Absyn_Enumdecl*_tmp3B3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2)->f1)->tag == 15U){_LL23: _tmp3B3=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B2)->f1)->f2;_LL24:
# 2356
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3C0))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3B3))->fields != 0)&&({
int _tmp747=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3C0->fields))->v);_tmp747 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3B3->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2362
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2365
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39C)->f2 != 0){_LL14: _tmp3C1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39C)->f2)->hd;_LL15:
# 2368
{void*_tmp3B4=t2;void*_tmp3B5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->f2 != 0){_LL28: _tmp3B5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->f2)->hd;_LL29:
# 2370
 if(Cyc_Tcenv_region_outlives(te,_tmp3C1,_tmp3B5))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2374
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 2380
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Unify_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp3C2=Cyc_Absyn_copy_exp(e);
({void*_tmp748=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->tag=14U,_tmp3C3->f1=t,_tmp3C3->f2=_tmp3C2,_tmp3C3->f3=0,_tmp3C3->f4=c;_tmp3C3;});e->r=_tmp748;});
e->topt=t;}}
# 2389
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2394
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3C9=({struct Cyc_Int_pa_PrintArg_struct _tmp636;_tmp636.tag=1U,_tmp636.f1=(unsigned long)i;_tmp636;});void*_tmp3C7[1U];_tmp3C7[0]=& _tmp3C9;({struct _fat_ptr _tmp749=({const char*_tmp3C8="#%d";_tag_fat(_tmp3C8,sizeof(char),4U);});Cyc_aprintf(_tmp749,_tag_fat(_tmp3C7,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({struct _fat_ptr*_tmp74A=({unsigned _tmp3C5=1;struct _fat_ptr*_tmp3C4=_cycalloc(_check_times(_tmp3C5,sizeof(struct _fat_ptr)));_tmp3C4[0]=s;_tmp3C4;});_tmp3C6->name=_tmp74A;}),_tmp3C6->identity=- 1,_tmp3C6->kind=k;_tmp3C6;});}
# 2401
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp3CA=*t->name;
return(int)*((const char*)_check_fat_subscript(_tmp3CA,sizeof(char),0))== (int)'#';}
# 2406
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp3CD=({struct Cyc_String_pa_PrintArg_struct _tmp637;_tmp637.tag=0U,_tmp637.f1=(struct _fat_ptr)((struct _fat_ptr)*t->name);_tmp637;});void*_tmp3CB[1U];_tmp3CB[0]=& _tmp3CD;({struct _fat_ptr _tmp74B=({const char*_tmp3CC="%s";_tag_fat(_tmp3CC,sizeof(char),3U);});Cyc_printf(_tmp74B,_tag_fat(_tmp3CB,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp3CE=({struct _fat_ptr _tmp74C=({const char*_tmp3D4="`";_tag_fat(_tmp3D4,sizeof(char),2U);});Cyc_strconcat(_tmp74C,(struct _fat_ptr)*t->name);});
({struct _fat_ptr _tmp3CF=_fat_ptr_plus(_tmp3CE,sizeof(char),1);char _tmp3D0=*((char*)_check_fat_subscript(_tmp3CF,sizeof(char),0U));char _tmp3D1='t';if(_get_fat_size(_tmp3CF,sizeof(char))== 1U &&(_tmp3D0 == 0 && _tmp3D1 != 0))_throw_arraybounds();*((char*)_tmp3CF.curr)=_tmp3D1;});
({struct _fat_ptr*_tmp74D=({unsigned _tmp3D3=1;struct _fat_ptr*_tmp3D2=_cycalloc(_check_times(_tmp3D3,sizeof(struct _fat_ptr)));_tmp3D2[0]=(struct _fat_ptr)_tmp3CE;_tmp3D2;});t->name=_tmp74D;});};}
# 2415
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2422
struct Cyc_List_List*_tmp3D5=0;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp3D5=({struct Cyc_List_List*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));_tmp3D6->hd=(void*)atts->hd,_tmp3D6->tl=_tmp3D5;_tmp3D6;});}}{
struct Cyc_Absyn_FnInfo _tmp3D7=fd->i;
_tmp3D7.attributes=_tmp3D5;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->tag=5U,_tmp3D8->f1=_tmp3D7;_tmp3D8;});};}
# 2430
return(void*)_check_null(fd->cached_type);}
# 2436
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2438
union Cyc_Relations_RelnOp _tmp3D9=*rop;union Cyc_Relations_RelnOp _tmp3DA=_tmp3D9;struct Cyc_Absyn_Vardecl*_tmp3ED;struct Cyc_Absyn_Vardecl*_tmp3EC;switch((_tmp3DA.RNumelts).tag){case 2U: _LL1: _tmp3EC=(_tmp3DA.RVar).val;_LL2: {
# 2440
struct _tuple1 _tmp3DB=*_tmp3EC->name;struct _tuple1 _tmp3DC=_tmp3DB;union Cyc_Absyn_Nmspace _tmp3E3;struct _fat_ptr*_tmp3E2;_LL8: _tmp3E3=_tmp3DC.f1;_tmp3E2=_tmp3DC.f2;_LL9:;
if(!((int)((_tmp3E3.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp74E=(struct _fat_ptr)*_tmp3E2;Cyc_strcmp(_tmp74E,({const char*_tmp3DD="return_value";_tag_fat(_tmp3DD,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp74F=Cyc_Relations_RReturn();*rop=_tmp74F;});
goto _LL0;}{
# 2446
unsigned c=0U;
{struct Cyc_List_List*_tmp3DE=args;for(0;_tmp3DE != 0;(_tmp3DE=_tmp3DE->tl,c ++)){
struct _tuple9*_tmp3DF=(struct _tuple9*)_tmp3DE->hd;struct _tuple9*_tmp3E0=_tmp3DF;struct _fat_ptr*_tmp3E1;_LLB: _tmp3E1=_tmp3E0->f1;_LLC:;
if(_tmp3E1 != 0){
if(Cyc_strcmp((struct _fat_ptr)*_tmp3E2,(struct _fat_ptr)*_tmp3E1)== 0){
({union Cyc_Relations_RelnOp _tmp750=Cyc_Relations_RParam(c);*rop=_tmp750;});
break;}}}}
# 2456
goto _LL0;};}case 3U: _LL3: _tmp3ED=(_tmp3DA.RNumelts).val;_LL4: {
# 2458
struct _tuple1 _tmp3E4=*_tmp3ED->name;struct _tuple1 _tmp3E5=_tmp3E4;union Cyc_Absyn_Nmspace _tmp3EB;struct _fat_ptr*_tmp3EA;_LLE: _tmp3EB=_tmp3E5.f1;_tmp3EA=_tmp3E5.f2;_LLF:;
if(!((int)((_tmp3EB.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3E6=args;for(0;_tmp3E6 != 0;(_tmp3E6=_tmp3E6->tl,c ++)){
struct _tuple9*_tmp3E7=(struct _tuple9*)_tmp3E6->hd;struct _tuple9*_tmp3E8=_tmp3E7;struct _fat_ptr*_tmp3E9;_LL11: _tmp3E9=_tmp3E8->f1;_LL12:;
if(_tmp3E9 != 0){
if(Cyc_strcmp((struct _fat_ptr)*_tmp3EA,(struct _fat_ptr)*_tmp3E9)== 0){
({union Cyc_Relations_RelnOp _tmp751=Cyc_Relations_RParamNumelts(c);*rop=_tmp751;});
break;}}}}
# 2470
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2475
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2477
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2481
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2483
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3EE=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp3EE);
return _tmp3EE;};}
# 2490
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){return(*t).f2;}
static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){
struct _tuple11*_tmp3EF=pr;struct Cyc_Absyn_Tqual _tmp3F2;void*_tmp3F1;_LL1: _tmp3F2=_tmp3EF->f1;_tmp3F1=_tmp3EF->f2;_LL2:;
if(_tmp3F1 == t)return pr;else{
return({struct _tuple11*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->f1=_tmp3F2,_tmp3F0->f2=t;_tmp3F0;});}}struct _tuple25{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple26{struct _tuple25*f1;void*f2;};
# 2496
static struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2498
return({struct _tuple26*_tmp3F4=_region_malloc(rgn,sizeof(*_tmp3F4));({struct _tuple25*_tmp752=({struct _tuple25*_tmp3F3=_region_malloc(rgn,sizeof(*_tmp3F3));_tmp3F3->f1=(*y).f1,_tmp3F3->f2=(*y).f2;_tmp3F3;});_tmp3F4->f1=_tmp752;}),_tmp3F4->f2=(*y).f3;_tmp3F4;});}
# 2500
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2502
struct _tuple9 _tmp3F5=*orig_arg;struct _tuple9 _tmp3F6=_tmp3F5;struct _fat_ptr*_tmp3FA;struct Cyc_Absyn_Tqual _tmp3F9;void*_tmp3F8;_LL1: _tmp3FA=_tmp3F6.f1;_tmp3F9=_tmp3F6.f2;_tmp3F8=_tmp3F6.f3;_LL2:;
if(t == _tmp3F8)return orig_arg;
return({struct _tuple9*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->f1=_tmp3FA,_tmp3F7->f2=_tmp3F9,_tmp3F7->f3=t;_tmp3F7;});}
# 2506
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2511
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2513
return({struct Cyc_Absyn_Exp*_tmp3FB=_cycalloc(sizeof(*_tmp3FB));_tmp3FB->topt=old->topt,_tmp3FB->r=r,_tmp3FB->loc=old->loc,_tmp3FB->annot=old->annot;_tmp3FB;});}
# 2518
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3FC=e->r;void*_tmp3FD=_tmp3FC;void*_tmp43D;void*_tmp43C;struct Cyc_List_List*_tmp43B;struct Cyc_Absyn_Exp*_tmp43A;struct Cyc_Absyn_Exp*_tmp439;void*_tmp438;void*_tmp437;struct Cyc_Absyn_Exp*_tmp436;int _tmp435;enum Cyc_Absyn_Coercion _tmp434;struct Cyc_Absyn_Exp*_tmp433;struct Cyc_Absyn_Exp*_tmp432;struct Cyc_Absyn_Exp*_tmp431;struct Cyc_Absyn_Exp*_tmp430;struct Cyc_Absyn_Exp*_tmp42F;struct Cyc_Absyn_Exp*_tmp42E;struct Cyc_Absyn_Exp*_tmp42D;struct Cyc_Absyn_Exp*_tmp42C;struct Cyc_Absyn_Exp*_tmp42B;enum Cyc_Absyn_Primop _tmp42A;struct Cyc_List_List*_tmp429;switch(*((int*)_tmp3FD)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp42A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_tmp429=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2;_LLC:
# 2527
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp429)== 1){
struct Cyc_Absyn_Exp*_tmp3FE=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp429))->hd;
struct Cyc_Absyn_Exp*_tmp3FF=Cyc_Tcutil_rsubsexp(r,inst,_tmp3FE);
if(_tmp3FF == _tmp3FE)return e;
return({struct Cyc_Absyn_Exp*_tmp754=e;Cyc_Tcutil_copye(_tmp754,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->tag=3U,_tmp401->f1=_tmp42A,({struct Cyc_List_List*_tmp753=({struct Cyc_Absyn_Exp*_tmp400[1U];_tmp400[0]=_tmp3FF;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp400,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp401->f2=_tmp753;});_tmp401;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp429)== 2){
struct Cyc_Absyn_Exp*_tmp402=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp429))->hd;
struct Cyc_Absyn_Exp*_tmp403=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp429->tl))->hd;
struct Cyc_Absyn_Exp*_tmp404=Cyc_Tcutil_rsubsexp(r,inst,_tmp402);
struct Cyc_Absyn_Exp*_tmp405=Cyc_Tcutil_rsubsexp(r,inst,_tmp403);
if(_tmp404 == _tmp402 && _tmp405 == _tmp403)return e;
return({struct Cyc_Absyn_Exp*_tmp756=e;Cyc_Tcutil_copye(_tmp756,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->tag=3U,_tmp407->f1=_tmp42A,({struct Cyc_List_List*_tmp755=({struct Cyc_Absyn_Exp*_tmp406[2U];_tmp406[0]=_tmp404,_tmp406[1]=_tmp405;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp406,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp407->f2=_tmp755;});_tmp407;}));});}else{
return({void*_tmp408=0U;({struct _fat_ptr _tmp757=({const char*_tmp409="primop does not have 1 or 2 args!";_tag_fat(_tmp409,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp757,_tag_fat(_tmp408,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp42D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_tmp42C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2;_tmp42B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3FD)->f3;_LLE: {
# 2541
struct Cyc_Absyn_Exp*_tmp40A=Cyc_Tcutil_rsubsexp(r,inst,_tmp42D);
struct Cyc_Absyn_Exp*_tmp40B=Cyc_Tcutil_rsubsexp(r,inst,_tmp42C);
struct Cyc_Absyn_Exp*_tmp40C=Cyc_Tcutil_rsubsexp(r,inst,_tmp42B);
if((_tmp40A == _tmp42D && _tmp40B == _tmp42C)&& _tmp40C == _tmp42B)return e;
return({struct Cyc_Absyn_Exp*_tmp758=e;Cyc_Tcutil_copye(_tmp758,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->tag=6U,_tmp40D->f1=_tmp40A,_tmp40D->f2=_tmp40B,_tmp40D->f3=_tmp40C;_tmp40D;}));});}case 7U: _LLF: _tmp42F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_tmp42E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2;_LL10: {
# 2547
struct Cyc_Absyn_Exp*_tmp40E=Cyc_Tcutil_rsubsexp(r,inst,_tmp42F);
struct Cyc_Absyn_Exp*_tmp40F=Cyc_Tcutil_rsubsexp(r,inst,_tmp42E);
if(_tmp40E == _tmp42F && _tmp40F == _tmp42E)return e;
return({struct Cyc_Absyn_Exp*_tmp759=e;Cyc_Tcutil_copye(_tmp759,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->tag=7U,_tmp410->f1=_tmp40E,_tmp410->f2=_tmp40F;_tmp410;}));});}case 8U: _LL11: _tmp431=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_tmp430=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2;_LL12: {
# 2552
struct Cyc_Absyn_Exp*_tmp411=Cyc_Tcutil_rsubsexp(r,inst,_tmp431);
struct Cyc_Absyn_Exp*_tmp412=Cyc_Tcutil_rsubsexp(r,inst,_tmp430);
if(_tmp411 == _tmp431 && _tmp412 == _tmp430)return e;
return({struct Cyc_Absyn_Exp*_tmp75A=e;Cyc_Tcutil_copye(_tmp75A,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp413=_cycalloc(sizeof(*_tmp413));_tmp413->tag=8U,_tmp413->f1=_tmp411,_tmp413->f2=_tmp412;_tmp413;}));});}case 9U: _LL13: _tmp433=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_tmp432=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2;_LL14: {
# 2557
struct Cyc_Absyn_Exp*_tmp414=Cyc_Tcutil_rsubsexp(r,inst,_tmp433);
struct Cyc_Absyn_Exp*_tmp415=Cyc_Tcutil_rsubsexp(r,inst,_tmp432);
if(_tmp414 == _tmp433 && _tmp415 == _tmp432)return e;
return({struct Cyc_Absyn_Exp*_tmp75B=e;Cyc_Tcutil_copye(_tmp75B,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp416=_cycalloc(sizeof(*_tmp416));_tmp416->tag=9U,_tmp416->f1=_tmp414,_tmp416->f2=_tmp415;_tmp416;}));});}case 14U: _LL15: _tmp437=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_tmp436=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2;_tmp435=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FD)->f3;_tmp434=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FD)->f4;_LL16: {
# 2562
struct Cyc_Absyn_Exp*_tmp417=Cyc_Tcutil_rsubsexp(r,inst,_tmp436);
void*_tmp418=Cyc_Tcutil_rsubstitute(r,inst,_tmp437);
if(_tmp417 == _tmp436 && _tmp418 == _tmp437)return e;
return({struct Cyc_Absyn_Exp*_tmp75C=e;Cyc_Tcutil_copye(_tmp75C,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->tag=14U,_tmp419->f1=_tmp418,_tmp419->f2=_tmp417,_tmp419->f3=_tmp435,_tmp419->f4=_tmp434;_tmp419;}));});}case 17U: _LL17: _tmp438=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_LL18: {
# 2567
void*_tmp41A=Cyc_Tcutil_rsubstitute(r,inst,_tmp438);
if(_tmp41A == _tmp438)return e;
return({struct Cyc_Absyn_Exp*_tmp75D=e;Cyc_Tcutil_copye(_tmp75D,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->tag=17U,_tmp41B->f1=_tmp41A;_tmp41B;}));});}case 18U: _LL19: _tmp439=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_LL1A: {
# 2571
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Tcutil_rsubsexp(r,inst,_tmp439);
if(_tmp41C == _tmp439)return e;
return({struct Cyc_Absyn_Exp*_tmp75E=e;Cyc_Tcutil_copye(_tmp75E,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->tag=18U,_tmp41D->f1=_tmp41C;_tmp41D;}));});}case 41U: _LL1B: _tmp43A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_LL1C: {
# 2575
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Tcutil_rsubsexp(r,inst,_tmp43A);
if(_tmp41E == _tmp43A)return e;
return({struct Cyc_Absyn_Exp*_tmp75F=e;Cyc_Tcutil_copye(_tmp75F,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->tag=41U,_tmp41F->f1=_tmp41E;_tmp41F;}));});}case 19U: _LL1D: _tmp43C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_tmp43B=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3FD)->f2;_LL1E: {
# 2579
void*_tmp420=Cyc_Tcutil_rsubstitute(r,inst,_tmp43C);
if(_tmp420 == _tmp43C)return e;
return({struct Cyc_Absyn_Exp*_tmp760=e;Cyc_Tcutil_copye(_tmp760,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->tag=19U,_tmp421->f1=_tmp420,_tmp421->f2=_tmp43B;_tmp421;}));});}case 39U: _LL1F: _tmp43D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3FD)->f1;_LL20: {
# 2583
void*_tmp422=Cyc_Tcutil_rsubstitute(r,inst,_tmp43D);
if(_tmp422 == _tmp43D)return e;{
# 2586
void*_tmp423=Cyc_Tcutil_compress(_tmp422);void*_tmp424=_tmp423;struct Cyc_Absyn_Exp*_tmp426;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp424)->tag == 9U){_LL24: _tmp426=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp424)->f1;_LL25:
 return _tmp426;}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp761=e;Cyc_Tcutil_copye(_tmp761,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->tag=39U,_tmp425->f1=_tmp422;_tmp425;}));});}_LL23:;};}default: _LL21: _LL22:
# 2590
 return({void*_tmp427=0U;({struct _fat_ptr _tmp762=({const char*_tmp428="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp428,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp762,_tag_fat(_tmp427,sizeof(void*),0U));});});}_LL0:;}
# 2594
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2597
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 2601
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2604
void*_tmp43E=f->type;
struct Cyc_Absyn_Exp*_tmp43F=f->requires_clause;
void*_tmp440=Cyc_Tcutil_rsubstitute(r,inst,_tmp43E);
struct Cyc_Absyn_Exp*_tmp441=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp43F);
if(_tmp43E == _tmp440 && _tmp43F == _tmp441)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->name=f->name,_tmp442->tq=f->tq,_tmp442->type=_tmp440,_tmp442->width=f->width,_tmp442->attributes=f->attributes,_tmp442->requires_clause=_tmp441;_tmp442;});}}
# 2614
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2617
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp443=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp444=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp445=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp443);
struct Cyc_List_List*_tmp446=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp444);
if(_tmp445 == _tmp443 && _tmp446 == _tmp444)return fs;
return({struct Cyc_List_List*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->hd=_tmp445,_tmp447->tl=_tmp446;_tmp447;});};}
# 2626
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2629
struct _tuple0 _tmp448=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _tmp449=_tmp448;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44C;_LL1: _tmp44D=_tmp449.f1;_tmp44C=_tmp449.f2;_LL2:;{
struct Cyc_List_List*_tmp44A=Cyc_Tcutil_substs(rgn,inst,_tmp44D);
struct Cyc_List_List*_tmp44B=Cyc_Tcutil_substs(rgn,inst,_tmp44C);
if(_tmp44A == _tmp44D && _tmp44B == _tmp44C)
return rgn_po;else{
# 2635
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp44A,_tmp44B);}};}
# 2638
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2641
void*_tmp44E=Cyc_Tcutil_compress(t);void*_tmp44F=_tmp44E;struct Cyc_Absyn_Exp*_tmp4A6;struct Cyc_Absyn_Exp*_tmp4A5;void*_tmp4A4;struct Cyc_List_List*_tmp4A3;void*_tmp4A2;void*_tmp4A1;enum Cyc_Absyn_AggrKind _tmp4A0;struct Cyc_List_List*_tmp49F;struct Cyc_List_List*_tmp49E;struct Cyc_List_List*_tmp49D;void*_tmp49C;struct Cyc_Absyn_Tqual _tmp49B;void*_tmp49A;struct Cyc_List_List*_tmp499;int _tmp498;struct Cyc_Absyn_VarargInfo*_tmp497;struct Cyc_List_List*_tmp496;struct Cyc_List_List*_tmp495;struct Cyc_Absyn_Exp*_tmp494;struct Cyc_Absyn_Exp*_tmp493;void*_tmp492;struct Cyc_Absyn_Tqual _tmp491;void*_tmp490;void*_tmp48F;void*_tmp48E;void*_tmp48D;void*_tmp48C;struct Cyc_Absyn_Tqual _tmp48B;struct Cyc_Absyn_Exp*_tmp48A;void*_tmp489;unsigned _tmp488;struct _tuple1*_tmp487;struct Cyc_List_List*_tmp486;struct Cyc_Absyn_Typedefdecl*_tmp485;void*_tmp484;struct Cyc_Absyn_Tvar*_tmp483;switch(*((int*)_tmp44F)){case 2U: _LL1: _tmp483=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp44F)->f1;_LL2: {
# 2644
struct _handler_cons _tmp450;_push_handler(& _tmp450);{int _tmp452=0;if(setjmp(_tmp450.handler))_tmp452=1;if(!_tmp452){{void*_tmp453=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp483);_npop_handler(0U);return _tmp453;};_pop_handler();}else{void*_tmp451=(void*)Cyc_Core_get_exn_thrown();void*_tmp454=_tmp451;void*_tmp455;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp454)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp455=_tmp454;_LL1F:(int)_rethrow(_tmp455);}_LL1B:;}};}case 8U: _LL3: _tmp487=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp44F)->f1;_tmp486=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp44F)->f2;_tmp485=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp44F)->f3;_tmp484=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp44F)->f4;_LL4: {
# 2647
struct Cyc_List_List*_tmp456=Cyc_Tcutil_substs(rgn,inst,_tmp486);
return _tmp456 == _tmp486?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->tag=8U,_tmp457->f1=_tmp487,_tmp457->f2=_tmp456,_tmp457->f3=_tmp485,_tmp457->f4=_tmp484;_tmp457;});}case 4U: _LL5: _tmp48C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44F)->f1).elt_type;_tmp48B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44F)->f1).tq;_tmp48A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44F)->f1).num_elts;_tmp489=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44F)->f1).zero_term;_tmp488=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp44F)->f1).zt_loc;_LL6: {
# 2650
void*_tmp458=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp48C);
struct Cyc_Absyn_Exp*_tmp459=_tmp48A == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp48A);
void*_tmp45A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp489);
return(_tmp458 == _tmp48C && _tmp459 == _tmp48A)&& _tmp45A == _tmp489?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));
_tmp45B->tag=4U,(_tmp45B->f1).elt_type=_tmp458,(_tmp45B->f1).tq=_tmp48B,(_tmp45B->f1).num_elts=_tmp459,(_tmp45B->f1).zero_term=_tmp45A,(_tmp45B->f1).zt_loc=_tmp488;_tmp45B;});}case 3U: _LL7: _tmp492=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).elt_type;_tmp491=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).elt_tq;_tmp490=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).ptr_atts).rgn;_tmp48F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).ptr_atts).nullable;_tmp48E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).ptr_atts).bounds;_tmp48D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp44F)->f1).ptr_atts).zero_term;_LL8: {
# 2656
void*_tmp45C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp492);
void*_tmp45D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp490);
void*_tmp45E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp48E);
void*_tmp45F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp48D);
if(((_tmp45C == _tmp492 && _tmp45D == _tmp490)&& _tmp45E == _tmp48E)&& _tmp45F == _tmp48D)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp638;_tmp638.elt_type=_tmp45C,_tmp638.elt_tq=_tmp491,(_tmp638.ptr_atts).rgn=_tmp45D,(_tmp638.ptr_atts).nullable=_tmp48F,(_tmp638.ptr_atts).bounds=_tmp45E,(_tmp638.ptr_atts).zero_term=_tmp45F,(_tmp638.ptr_atts).ptrloc=0;_tmp638;}));}case 5U: _LL9: _tmp49D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).tvars;_tmp49C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).effect;_tmp49B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).ret_tqual;_tmp49A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).ret_type;_tmp499=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).args;_tmp498=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).c_varargs;_tmp497=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).cyc_varargs;_tmp496=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).rgn_po;_tmp495=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).attributes;_tmp494=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).requires_clause;_tmp493=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp44F)->f1).ensures_clause;_LLA:
# 2666
{struct Cyc_List_List*_tmp460=_tmp49D;for(0;_tmp460 != 0;_tmp460=_tmp460->tl){
inst=({struct Cyc_List_List*_tmp462=_region_malloc(rgn,sizeof(*_tmp462));({struct _tuple18*_tmp764=({struct _tuple18*_tmp461=_region_malloc(rgn,sizeof(*_tmp461));_tmp461->f1=(struct Cyc_Absyn_Tvar*)_tmp460->hd,({void*_tmp763=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp460->hd);_tmp461->f2=_tmp763;});_tmp461;});_tmp462->hd=_tmp764;}),_tmp462->tl=inst;_tmp462;});}}{
struct _tuple0 _tmp463=({struct _RegionHandle*_tmp766=rgn;struct _RegionHandle*_tmp765=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp766,_tmp765,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp499));});
# 2668
struct _tuple0 _tmp464=_tmp463;struct Cyc_List_List*_tmp473;struct Cyc_List_List*_tmp472;_LL21: _tmp473=_tmp464.f1;_tmp472=_tmp464.f2;_LL22:;{
# 2670
struct Cyc_List_List*_tmp465=_tmp499;
struct Cyc_List_List*_tmp466=Cyc_Tcutil_substs(rgn,inst,_tmp472);
if(_tmp466 != _tmp472)
_tmp465=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp499,_tmp466);{
void*eff2;
if(_tmp49C == 0)
eff2=0;else{
# 2678
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp49C);
if(new_eff == _tmp49C)
eff2=_tmp49C;else{
# 2682
eff2=new_eff;}}{
# 2684
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp497 == 0)
cyc_varargs2=0;else{
# 2688
struct Cyc_Absyn_VarargInfo _tmp467=*_tmp497;struct Cyc_Absyn_VarargInfo _tmp468=_tmp467;struct _fat_ptr*_tmp46E;struct Cyc_Absyn_Tqual _tmp46D;void*_tmp46C;int _tmp46B;_LL24: _tmp46E=_tmp468.name;_tmp46D=_tmp468.tq;_tmp46C=_tmp468.type;_tmp46B=_tmp468.inject;_LL25:;{
void*_tmp469=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp46C);
if(_tmp469 == _tmp46C)cyc_varargs2=_tmp497;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->name=_tmp46E,_tmp46A->tq=_tmp46D,_tmp46A->type=_tmp469,_tmp46A->inject=_tmp46B;_tmp46A;});}};}{
# 2693
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp496);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp494);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp493);
struct Cyc_List_List*_tmp46F=Cyc_Tcutil_extract_relns(_tmp465,req2);
struct Cyc_List_List*_tmp470=Cyc_Tcutil_extract_relns(_tmp465,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->tag=5U,(_tmp471->f1).tvars=_tmp49D,(_tmp471->f1).effect=eff2,(_tmp471->f1).ret_tqual=_tmp49B,({void*_tmp767=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp49A);(_tmp471->f1).ret_type=_tmp767;}),(_tmp471->f1).args=_tmp465,(_tmp471->f1).c_varargs=_tmp498,(_tmp471->f1).cyc_varargs=cyc_varargs2,(_tmp471->f1).rgn_po=rgn_po2,(_tmp471->f1).attributes=_tmp495,(_tmp471->f1).requires_clause=req2,(_tmp471->f1).requires_relns=_tmp46F,(_tmp471->f1).ensures_clause=ens2,(_tmp471->f1).ensures_relns=_tmp470;_tmp471;});};};};};};case 6U: _LLB: _tmp49E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp44F)->f1;_LLC: {
# 2702
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp474=_tmp49E;for(0;_tmp474 != 0;_tmp474=_tmp474->tl){
void*_tmp475=(*((struct _tuple11*)_tmp474->hd)).f2;
void*_tmp476=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp475);
if(_tmp475 != _tmp476)
change=1;
# 2710
ts2=({struct Cyc_List_List*_tmp477=_region_malloc(rgn,sizeof(*_tmp477));_tmp477->hd=_tmp476,_tmp477->tl=ts2;_tmp477;});}}
# 2712
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->tag=6U,({struct Cyc_List_List*_tmp769=({struct Cyc_List_List*_tmp768=_tmp49E;((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp768,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp478->f1=_tmp769;});_tmp478;});}case 7U: _LLD: _tmp4A0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp44F)->f1;_tmp49F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp44F)->f2;_LLE: {
# 2716
struct Cyc_List_List*_tmp479=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp49F);
if(_tmp49F == _tmp479)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->tag=7U,_tmp47A->f1=_tmp4A0,_tmp47A->f2=_tmp479;_tmp47A;});}case 1U: _LLF: _tmp4A1=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp44F)->f2;_LL10:
# 2720
 if(_tmp4A1 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp4A1);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f2 == 0){_LL11: _tmp4A2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f1;_LL12:
 return t;}else{_LL13: _tmp4A4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f1;_tmp4A3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp44F)->f2;_LL14: {
# 2724
struct Cyc_List_List*_tmp47B=Cyc_Tcutil_substs(rgn,inst,_tmp4A3);
if(_tmp4A3 == _tmp47B)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->tag=0U,_tmp47C->f1=_tmp4A4,_tmp47C->f2=_tmp47B;_tmp47C;});}}}case 9U: _LL15: _tmp4A5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp44F)->f1;_LL16: {
# 2728
struct Cyc_Absyn_Exp*_tmp47D=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp4A5);
return _tmp47D == _tmp4A5?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->tag=9U,_tmp47E->f1=_tmp47D;_tmp47E;});}case 11U: _LL17: _tmp4A6=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp44F)->f1;_LL18: {
# 2731
struct Cyc_Absyn_Exp*_tmp47F=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp4A6);
return _tmp47F == _tmp4A6?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->tag=11U,_tmp480->f1=_tmp47F;_tmp480;});}default: _LL19: _LL1A:
({void*_tmp481=0U;({struct _fat_ptr _tmp76A=({const char*_tmp482="found typedecltype in rsubs";_tag_fat(_tmp482,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp76A,_tag_fat(_tmp481,sizeof(void*),0U));});});}_LL0:;}
# 2737
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2740
if(ts == 0)
return 0;{
void*_tmp4A7=(void*)ts->hd;
struct Cyc_List_List*_tmp4A8=ts->tl;
void*_tmp4A9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp4A7);
struct Cyc_List_List*_tmp4AA=Cyc_Tcutil_substs(rgn,inst,_tmp4A8);
if(_tmp4A7 == _tmp4A9 && _tmp4A8 == _tmp4AA)
return ts;
return({struct Cyc_List_List*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->hd=_tmp4A9,_tmp4AB->tl=_tmp4AA;_tmp4AB;});};}
# 2751
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 2758
struct _tuple18*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4AC=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple18*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE->f1=tv,({void*_tmp76C=({struct Cyc_Core_Opt*_tmp76B=_tmp4AC;Cyc_Absyn_new_evar(_tmp76B,({struct Cyc_Core_Opt*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->v=s;_tmp4AD;}));});_tmp4AE->f2=_tmp76C;});_tmp4AE;});}struct _tuple27{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2763
struct _tuple18*Cyc_Tcutil_r_make_inst_var(struct _tuple27*env,struct Cyc_Absyn_Tvar*tv){
# 2765
struct _tuple27*_tmp4AF=env;struct Cyc_List_List*_tmp4B4;struct _RegionHandle*_tmp4B3;_LL1: _tmp4B4=_tmp4AF->f1;_tmp4B3=_tmp4AF->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp4B0=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple18*_tmp4B2=_region_malloc(_tmp4B3,sizeof(*_tmp4B2));_tmp4B2->f1=tv,({void*_tmp76E=({struct Cyc_Core_Opt*_tmp76D=_tmp4B0;Cyc_Absyn_new_evar(_tmp76D,({struct Cyc_Core_Opt*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->v=_tmp4B4;_tmp4B1;}));});_tmp4B2->f2=_tmp76E;});_tmp4B2;});};}
# 2770
void Cyc_Tcutil_check_bitfield(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2772
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp4B7=({struct Cyc_String_pa_PrintArg_struct _tmp639;_tmp639.tag=0U,_tmp639.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp639;});void*_tmp4B5[1U];_tmp4B5[0]=& _tmp4B7;({unsigned _tmp770=loc;struct _fat_ptr _tmp76F=({const char*_tmp4B6="bitfield %s does not have constant width";_tag_fat(_tmp4B6,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp770,_tmp76F,_tag_fat(_tmp4B5,sizeof(void*),1U));});});else{
# 2777
struct _tuple12 _tmp4B8=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple12 _tmp4B9=_tmp4B8;unsigned _tmp4BF;int _tmp4BE;_LL1: _tmp4BF=_tmp4B9.f1;_tmp4BE=_tmp4B9.f2;_LL2:;
if(!_tmp4BE)
({void*_tmp4BA=0U;({unsigned _tmp772=loc;struct _fat_ptr _tmp771=({const char*_tmp4BB="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4BB,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp772,_tmp771,_tag_fat(_tmp4BA,sizeof(void*),0U));});});
if((int)_tmp4BF < 0)
({void*_tmp4BC=0U;({unsigned _tmp774=loc;struct _fat_ptr _tmp773=({const char*_tmp4BD="bitfield has negative width";_tag_fat(_tmp4BD,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp774,_tmp773,_tag_fat(_tmp4BC,sizeof(void*),0U));});});
w=_tmp4BF;}{
# 2784
void*_tmp4C0=Cyc_Tcutil_compress(field_type);void*_tmp4C1=_tmp4C0;enum Cyc_Absyn_Size_of _tmp4CF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f1)->tag == 1U){_LL4: _tmp4CF=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C1)->f1)->f2;_LL5:
# 2787
{enum Cyc_Absyn_Size_of _tmp4C2=_tmp4CF;switch(_tmp4C2){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp4C3=0U;({unsigned _tmp776=loc;struct _fat_ptr _tmp775=({const char*_tmp4C4="bitfield larger than type";_tag_fat(_tmp4C4,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp776,_tmp775,_tag_fat(_tmp4C3,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp4C5=0U;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp4C6="bitfield larger than type";_tag_fat(_tmp4C6,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp778,_tmp777,_tag_fat(_tmp4C5,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp4C7=0U;({unsigned _tmp77A=loc;struct _fat_ptr _tmp779=({const char*_tmp4C8="bitfield larger than type";_tag_fat(_tmp4C8,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp77A,_tmp779,_tag_fat(_tmp4C7,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp4C9=0U;({unsigned _tmp77C=loc;struct _fat_ptr _tmp77B=({const char*_tmp4CA="bitfield larger than type";_tag_fat(_tmp4CA,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp77C,_tmp77B,_tag_fat(_tmp4C9,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 2795
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 2797
({struct Cyc_String_pa_PrintArg_struct _tmp4CD=({struct Cyc_String_pa_PrintArg_struct _tmp63B;_tmp63B.tag=0U,_tmp63B.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp63B;});struct Cyc_String_pa_PrintArg_struct _tmp4CE=({struct Cyc_String_pa_PrintArg_struct _tmp63A;_tmp63A.tag=0U,({
struct _fat_ptr _tmp77D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field_type));_tmp63A.f1=_tmp77D;});_tmp63A;});void*_tmp4CB[2U];_tmp4CB[0]=& _tmp4CD,_tmp4CB[1]=& _tmp4CE;({unsigned _tmp77F=loc;struct _fat_ptr _tmp77E=({const char*_tmp4CC="bitfield %s must have integral type but has type %s";_tag_fat(_tmp4CC,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp77F,_tmp77E,_tag_fat(_tmp4CB,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 2806
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4D0=t;struct Cyc_Absyn_Typedefdecl*_tmp4D4;void*_tmp4D3;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D0)->tag == 8U){_LL1: _tmp4D4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D0)->f3;_tmp4D3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4D0)->f4;_LL2:
# 2809
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp4D4))->tq).real_const ||(_tmp4D4->tq).print_const){
if(declared_const)({void*_tmp4D1=0U;({unsigned _tmp781=loc;struct _fat_ptr _tmp780=({const char*_tmp4D2="extra const";_tag_fat(_tmp4D2,sizeof(char),12U);});Cyc_Tcutil_warn(_tmp781,_tmp780,_tag_fat(_tmp4D1,sizeof(void*),0U));});});
return 1;}
# 2814
if((unsigned)_tmp4D3)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp4D3);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2821
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp782=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp782;});}
# 2825
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2831
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2834
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4D7=({struct Cyc_String_pa_PrintArg_struct _tmp63D;_tmp63D.tag=0U,_tmp63D.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp63D;});struct Cyc_String_pa_PrintArg_struct _tmp4D8=({struct Cyc_String_pa_PrintArg_struct _tmp63C;_tmp63C.tag=0U,({struct _fat_ptr _tmp783=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp63C.f1=_tmp783;});_tmp63C;});void*_tmp4D5[2U];_tmp4D5[0]=& _tmp4D7,_tmp4D5[1]=& _tmp4D8;({unsigned _tmp785=loc;struct _fat_ptr _tmp784=({const char*_tmp4D6="%s: %s";_tag_fat(_tmp4D6,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp785,_tmp784,_tag_fat(_tmp4D5,sizeof(void*),2U));});});}}}
# 2840
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2844
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2848
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp787=tvs;unsigned _tmp786=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp787,_tmp786,Cyc_Absynpp_tvar2string,({const char*_tmp4D9="duplicate type variable";_tag_fat(_tmp4D9,sizeof(char),24U);}));});}struct _tuple28{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple29{struct Cyc_List_List*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2862 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2867
struct _RegionHandle _tmp4DA=_new_region("temp");struct _RegionHandle*temp=& _tmp4DA;_push_region(temp);
# 2871
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp788=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp788,({const char*_tmp4DB="";_tag_fat(_tmp4DB,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4DD=_region_malloc(temp,sizeof(*_tmp4DD));({struct _tuple28*_tmp789=({struct _tuple28*_tmp4DC=_region_malloc(temp,sizeof(*_tmp4DC));_tmp4DC->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4DC->f2=0;_tmp4DC;});_tmp4DD->hd=_tmp789;}),_tmp4DD->tl=fields;_tmp4DD;});}}
# 2876
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2878
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp502="struct";_tag_fat(_tmp502,sizeof(char),7U);}):({const char*_tmp503="union";_tag_fat(_tmp503,sizeof(char),6U);});
# 2881
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple29*_tmp4DE=(struct _tuple29*)des->hd;struct _tuple29*_tmp4DF=_tmp4DE;struct Cyc_List_List*_tmp4FA;void*_tmp4F9;_LL1: _tmp4FA=_tmp4DF->f1;_tmp4F9=_tmp4DF->f2;_LL2:;
if(_tmp4FA == 0){
# 2886
struct Cyc_List_List*_tmp4E0=fields;
for(0;_tmp4E0 != 0;_tmp4E0=_tmp4E0->tl){
if(!(*((struct _tuple28*)_tmp4E0->hd)).f2){
(*((struct _tuple28*)_tmp4E0->hd)).f2=1;
({struct Cyc_List_List*_tmp78B=({struct Cyc_List_List*_tmp4E2=_cycalloc(sizeof(*_tmp4E2));({void*_tmp78A=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4E1=_cycalloc(sizeof(*_tmp4E1));_tmp4E1->tag=1U,_tmp4E1->f1=((*((struct _tuple28*)_tmp4E0->hd)).f1)->name;_tmp4E1;});_tmp4E2->hd=_tmp78A;}),_tmp4E2->tl=0;_tmp4E2;});(*((struct _tuple29*)des->hd)).f1=_tmp78B;});
ans=({struct Cyc_List_List*_tmp4E4=_region_malloc(rgn,sizeof(*_tmp4E4));({struct _tuple30*_tmp78C=({struct _tuple30*_tmp4E3=_region_malloc(rgn,sizeof(*_tmp4E3));_tmp4E3->f1=(*((struct _tuple28*)_tmp4E0->hd)).f1,_tmp4E3->f2=_tmp4F9;_tmp4E3;});_tmp4E4->hd=_tmp78C;}),_tmp4E4->tl=ans;_tmp4E4;});
break;}}
# 2894
if(_tmp4E0 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4E7=({struct Cyc_String_pa_PrintArg_struct _tmp63E;_tmp63E.tag=0U,_tmp63E.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp63E;});void*_tmp4E5[1U];_tmp4E5[0]=& _tmp4E7;({unsigned _tmp78E=loc;struct _fat_ptr _tmp78D=({const char*_tmp4E6="too many arguments to %s";_tag_fat(_tmp4E6,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp78E,_tmp78D,_tag_fat(_tmp4E5,sizeof(void*),1U));});});}else{
if(_tmp4FA->tl != 0)
# 2898
({void*_tmp4E8=0U;({unsigned _tmp790=loc;struct _fat_ptr _tmp78F=({const char*_tmp4E9="multiple designators are not yet supported";_tag_fat(_tmp4E9,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp790,_tmp78F,_tag_fat(_tmp4E8,sizeof(void*),0U));});});else{
# 2901
void*_tmp4EA=(void*)_tmp4FA->hd;void*_tmp4EB=_tmp4EA;struct _fat_ptr*_tmp4F8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4EB)->tag == 0U){_LL4: _LL5:
# 2903
({struct Cyc_String_pa_PrintArg_struct _tmp4EE=({struct Cyc_String_pa_PrintArg_struct _tmp63F;_tmp63F.tag=0U,_tmp63F.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp63F;});void*_tmp4EC[1U];_tmp4EC[0]=& _tmp4EE;({unsigned _tmp792=loc;struct _fat_ptr _tmp791=({const char*_tmp4ED="array designator used in argument to %s";_tag_fat(_tmp4ED,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp792,_tmp791,_tag_fat(_tmp4EC,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp4F8=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4EB)->f1;_LL7: {
# 2906
struct Cyc_List_List*_tmp4EF=fields;
for(0;_tmp4EF != 0;_tmp4EF=_tmp4EF->tl){
if(Cyc_strptrcmp(_tmp4F8,((*((struct _tuple28*)_tmp4EF->hd)).f1)->name)== 0){
if((*((struct _tuple28*)_tmp4EF->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp4F2=({struct Cyc_String_pa_PrintArg_struct _tmp640;_tmp640.tag=0U,_tmp640.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp4F8);_tmp640;});void*_tmp4F0[1U];_tmp4F0[0]=& _tmp4F2;({unsigned _tmp794=loc;struct _fat_ptr _tmp793=({const char*_tmp4F1="member %s has already been used as an argument";_tag_fat(_tmp4F1,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp794,_tmp793,_tag_fat(_tmp4F0,sizeof(void*),1U));});});
(*((struct _tuple28*)_tmp4EF->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4F4=_region_malloc(rgn,sizeof(*_tmp4F4));({struct _tuple30*_tmp795=({struct _tuple30*_tmp4F3=_region_malloc(rgn,sizeof(*_tmp4F3));_tmp4F3->f1=(*((struct _tuple28*)_tmp4EF->hd)).f1,_tmp4F3->f2=_tmp4F9;_tmp4F3;});_tmp4F4->hd=_tmp795;}),_tmp4F4->tl=ans;_tmp4F4;});
break;}}
# 2915
if(_tmp4EF == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4F7=({struct Cyc_String_pa_PrintArg_struct _tmp641;_tmp641.tag=0U,_tmp641.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp4F8);_tmp641;});void*_tmp4F5[1U];_tmp4F5[0]=& _tmp4F7;({unsigned _tmp797=loc;struct _fat_ptr _tmp796=({const char*_tmp4F6="bad field designator %s";_tag_fat(_tmp4F6,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp797,_tmp796,_tag_fat(_tmp4F5,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 2920
if((int)aggr_kind == (int)0U)
# 2922
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple28*)fields->hd)).f2){
({void*_tmp4FB=0U;({unsigned _tmp799=loc;struct _fat_ptr _tmp798=({const char*_tmp4FC="too few arguments to struct";_tag_fat(_tmp4FC,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp799,_tmp798,_tag_fat(_tmp4FB,sizeof(void*),0U));});});
break;}}else{
# 2929
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple28*)fields->hd)).f2){
if(found)({void*_tmp4FD=0U;({unsigned _tmp79B=loc;struct _fat_ptr _tmp79A=({const char*_tmp4FE="only one member of a union is allowed";_tag_fat(_tmp4FE,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp79B,_tmp79A,_tag_fat(_tmp4FD,sizeof(void*),0U));});});
found=1;}}
# 2935
if(!found)({void*_tmp4FF=0U;({unsigned _tmp79D=loc;struct _fat_ptr _tmp79C=({const char*_tmp500="missing member for union";_tag_fat(_tmp500,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp79D,_tmp79C,_tag_fat(_tmp4FF,sizeof(void*),0U));});});}{
# 2938
struct Cyc_List_List*_tmp501=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp501;};};}
# 2871
;_pop_region(temp);}
# 2944
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2946
void*_tmp504=e1->r;void*_tmp505=_tmp504;struct Cyc_Absyn_Exp*_tmp517;struct Cyc_Absyn_Exp*_tmp516;struct Cyc_Absyn_Exp*_tmp515;struct Cyc_Absyn_Exp*_tmp514;struct Cyc_Absyn_Exp*_tmp513;struct Cyc_Absyn_Exp*_tmp512;switch(*((int*)_tmp505)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp508=({struct Cyc_String_pa_PrintArg_struct _tmp642;_tmp642.tag=0U,({struct _fat_ptr _tmp79E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp642.f1=_tmp79E;});_tmp642;});void*_tmp506[1U];_tmp506[0]=& _tmp508;({struct _fat_ptr _tmp79F=({const char*_tmp507="we have a cast in a lhs:  %s";_tag_fat(_tmp507,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp79F,_tag_fat(_tmp506,sizeof(void*),1U));});});case 20U: _LL3: _tmp512=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp505)->f1;_LL4:
 _tmp513=_tmp512;goto _LL6;case 23U: _LL5: _tmp513=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp505)->f1;_LL6:
# 2950
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp513->topt),ptr_type,is_fat,elt_type);case 22U: _LL7: _tmp514=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp505)->f1;_LL8:
 _tmp515=_tmp514;goto _LLA;case 21U: _LL9: _tmp515=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp505)->f1;_LLA:
# 2954
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp515->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp50B=({struct Cyc_String_pa_PrintArg_struct _tmp643;_tmp643.tag=0U,({
struct _fat_ptr _tmp7A0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp643.f1=_tmp7A0;});_tmp643;});void*_tmp509[1U];_tmp509[0]=& _tmp50B;({struct _fat_ptr _tmp7A1=({const char*_tmp50A="found zero pointer aggregate member assignment: %s";_tag_fat(_tmp50A,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A1,_tag_fat(_tmp509,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp516=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp505)->f1;_LLC:
 _tmp517=_tmp516;goto _LLE;case 12U: _LLD: _tmp517=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp505)->f1;_LLE:
# 2960
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp517->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp50E=({struct Cyc_String_pa_PrintArg_struct _tmp644;_tmp644.tag=0U,({
struct _fat_ptr _tmp7A2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp644.f1=_tmp7A2;});_tmp644;});void*_tmp50C[1U];_tmp50C[0]=& _tmp50E;({struct _fat_ptr _tmp7A3=({const char*_tmp50D="found zero pointer instantiate/noinstantiate: %s";_tag_fat(_tmp50D,sizeof(char),49U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A3,_tag_fat(_tmp50C,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp511=({struct Cyc_String_pa_PrintArg_struct _tmp645;_tmp645.tag=0U,({struct _fat_ptr _tmp7A4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp645.f1=_tmp7A4;});_tmp645;});void*_tmp50F[1U];_tmp50F[0]=& _tmp511;({struct _fat_ptr _tmp7A5=({const char*_tmp510="found bad lhs in is_zero_ptr_deref: %s";_tag_fat(_tmp510,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A5,_tag_fat(_tmp50F,sizeof(void*),1U));});});}_LL0:;}
# 2975
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 2978
void*_tmp518=Cyc_Tcutil_compress(r);void*_tmp519=_tmp518;struct Cyc_Absyn_Tvar*_tmp525;enum Cyc_Absyn_KindQual _tmp524;enum Cyc_Absyn_AliasQual _tmp523;switch(*((int*)_tmp519)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp519)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp519)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp519)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp519)->f4 == 0){_LL5: _tmp524=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp519)->f3)->kind)->v)->kind;_tmp523=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp519)->f3)->kind)->v)->aliasqual;_LL6:
# 2982
 return(int)_tmp524 == (int)3U &&((int)_tmp523 == (int)1U ||(int)_tmp523 == (int)2U && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp525=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp519)->f1;_LL8: {
# 2986
struct Cyc_Absyn_Kind*_tmp51A=Cyc_Tcutil_tvar_kind(_tmp525,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp51B=_tmp51A;enum Cyc_Absyn_KindQual _tmp522;enum Cyc_Absyn_AliasQual _tmp521;_LLC: _tmp522=_tmp51B->kind;_tmp521=_tmp51B->aliasqual;_LLD:;
if((int)_tmp522 == (int)3U &&((int)_tmp521 == (int)1U ||(int)_tmp521 == (int)2U && !must_be_unique)){
void*_tmp51C=Cyc_Absyn_compress_kb(_tmp525->kind);void*_tmp51D=_tmp51C;struct Cyc_Core_Opt**_tmp520;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51D)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51D)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51D)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp520=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51D)->f1;_LL10:
# 2990
({struct Cyc_Core_Opt*_tmp7A7=({struct Cyc_Core_Opt*_tmp51F=_cycalloc(sizeof(*_tmp51F));({void*_tmp7A6=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->tag=2U,_tmp51E->f1=0,_tmp51E->f2=& Cyc_Tcutil_rk;_tmp51E;});_tmp51F->v=_tmp7A6;});_tmp51F;});*_tmp520=_tmp7A7;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2995
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3002
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp526=Cyc_Tcutil_compress(t);void*_tmp527=_tmp526;struct Cyc_Absyn_Tvar*_tmp535;void*_tmp534;switch(*((int*)_tmp527)){case 3U: _LL1: _tmp534=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp527)->f1).ptr_atts).rgn;_LL2:
# 3005
 return Cyc_Tcutil_is_noalias_region(_tmp534,must_be_unique);case 2U: _LL3: _tmp535=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp527)->f1;_LL4: {
# 3007
struct Cyc_Absyn_Kind*_tmp528=Cyc_Tcutil_tvar_kind(_tmp535,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp529=_tmp528;enum Cyc_Absyn_KindQual _tmp533;enum Cyc_Absyn_AliasQual _tmp532;_LL8: _tmp533=_tmp529->kind;_tmp532=_tmp529->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp52A=_tmp533;switch(_tmp52A){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)_tmp532 == (int)1U ||(int)_tmp532 == (int)2U && !must_be_unique){
void*_tmp52B=Cyc_Absyn_compress_kb(_tmp535->kind);void*_tmp52C=_tmp52B;struct Cyc_Core_Opt**_tmp531;enum Cyc_Absyn_KindQual _tmp530;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52C)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52C)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp531=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52C)->f1;_tmp530=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp52C)->f2)->kind;_LL15:
# 3013
({struct Cyc_Core_Opt*_tmp7AA=({struct Cyc_Core_Opt*_tmp52F=_cycalloc(sizeof(*_tmp52F));({void*_tmp7A9=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->tag=2U,_tmp52E->f1=0,({struct Cyc_Absyn_Kind*_tmp7A8=({struct Cyc_Absyn_Kind*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->kind=_tmp530,_tmp52D->aliasqual=Cyc_Absyn_Aliasable;_tmp52D;});_tmp52E->f2=_tmp7A8;});_tmp52E;});_tmp52F->v=_tmp7A9;});_tmp52F;});*_tmp531=_tmp7AA;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 3017
 return 1;}_LL13:;}
# 3020
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 3023
 return 0;}_LL0:;}
# 3026
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp536=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp536,0))return 1;{
void*_tmp537=_tmp536;struct Cyc_List_List*_tmp54B;union Cyc_Absyn_DatatypeFieldInfo _tmp54A;struct Cyc_List_List*_tmp549;union Cyc_Absyn_DatatypeInfo _tmp548;struct Cyc_List_List*_tmp547;struct Cyc_Absyn_Aggrdecl**_tmp546;struct Cyc_List_List*_tmp545;struct Cyc_List_List*_tmp544;switch(*((int*)_tmp537)){case 6U: _LL1: _tmp544=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp537)->f1;_LL2:
# 3031
 while(_tmp544 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple11*)_tmp544->hd)).f2))return 1;
_tmp544=_tmp544->tl;}
# 3035
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp537)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp537)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp546=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp537)->f1)->f1).KnownAggr).val;_tmp545=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp537)->f2;_LL4:
# 3037
 if((*_tmp546)->impl == 0)return 0;else{
# 3039
struct Cyc_List_List*_tmp538=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp546)->tvs,_tmp545);
struct Cyc_List_List*_tmp539=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp546)->impl))->fields;
void*t;
while(_tmp539 != 0){
t=_tmp538 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp539->hd)->type: Cyc_Tcutil_substitute(_tmp538,((struct Cyc_Absyn_Aggrfield*)_tmp539->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp539=_tmp539->tl;}
# 3047
return 0;}}else{_LL7: _LL8:
# 3057
 return 0;}case 18U: _LL9: _tmp548=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp537)->f1)->f1;_tmp547=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp537)->f2;_LLA: {
# 3059
union Cyc_Absyn_DatatypeInfo _tmp53A=_tmp548;struct Cyc_List_List*_tmp53E;struct Cyc_Core_Opt*_tmp53D;struct _tuple1*_tmp53C;int _tmp53B;if((_tmp53A.UnknownDatatype).tag == 1){_LL10: _tmp53C=((_tmp53A.UnknownDatatype).val).name;_tmp53B=((_tmp53A.UnknownDatatype).val).is_extensible;_LL11:
# 3062
 return 0;}else{_LL12: _tmp53E=(*(_tmp53A.KnownDatatype).val)->tvs;_tmp53D=(*(_tmp53A.KnownDatatype).val)->fields;_LL13:
# 3065
 return 0;}_LLF:;}case 19U: _LLB: _tmp54A=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp537)->f1)->f1;_tmp549=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp537)->f2;_LLC: {
# 3068
union Cyc_Absyn_DatatypeFieldInfo _tmp53F=_tmp54A;struct Cyc_Absyn_Datatypedecl*_tmp543;struct Cyc_Absyn_Datatypefield*_tmp542;if((_tmp53F.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 3071
 return 0;}else{_LL17: _tmp543=((_tmp53F.KnownDatatypefield).val).f1;_tmp542=((_tmp53F.KnownDatatypefield).val).f2;_LL18: {
# 3073
struct Cyc_List_List*_tmp540=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp543->tvs,_tmp549);
struct Cyc_List_List*_tmp541=_tmp542->typs;
while(_tmp541 != 0){
_tmp536=_tmp540 == 0?(*((struct _tuple11*)_tmp541->hd)).f2: Cyc_Tcutil_substitute(_tmp540,(*((struct _tuple11*)_tmp541->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp536))return 1;
_tmp541=_tmp541->tl;}
# 3080
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp54B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp537)->f2;_LL6:
# 3050
 while(_tmp54B != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp54B->hd)->type))return 1;
_tmp54B=_tmp54B->tl;}
# 3054
return 0;default: _LLD: _LLE:
# 3082
 return 0;}_LL0:;};}
# 3089
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp54C=e->r;void*_tmp54D=_tmp54C;struct Cyc_Absyn_Stmt*_tmp563;struct Cyc_Absyn_Exp*_tmp562;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55C;struct _fat_ptr*_tmp55B;struct Cyc_Absyn_Exp*_tmp55A;struct Cyc_Absyn_Exp*_tmp559;switch(*((int*)_tmp54D)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54D)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp559=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp54D)->f1;_LL4:
 _tmp55A=_tmp559;goto _LL6;case 20U: _LL5: _tmp55A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp54D)->f1;_LL6:
# 3094
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp55A->topt),1)&& Cyc_Tcutil_is_noalias_path(_tmp55A);case 21U: _LL7: _tmp55C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp54D)->f1;_tmp55B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp54D)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp55C);case 23U: _LL9: _tmp55E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp54D)->f1;_tmp55D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp54D)->f2;_LLA: {
# 3097
void*_tmp54E=Cyc_Tcutil_compress((void*)_check_null(_tmp55E->topt));void*_tmp54F=_tmp54E;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54F)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp55E);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp560=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp54D)->f2;_tmp55F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp54D)->f3;_LLC:
# 3102
 return Cyc_Tcutil_is_noalias_path(_tmp560)&& Cyc_Tcutil_is_noalias_path(_tmp55F);case 9U: _LLD: _tmp561=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp54D)->f2;_LLE:
 _tmp562=_tmp561;goto _LL10;case 14U: _LLF: _tmp562=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp54D)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp562);case 37U: _LL11: _tmp563=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp54D)->f1;_LL12:
# 3106
 while(1){
void*_tmp550=_tmp563->r;void*_tmp551=_tmp550;struct Cyc_Absyn_Exp*_tmp558;struct Cyc_Absyn_Decl*_tmp557;struct Cyc_Absyn_Stmt*_tmp556;struct Cyc_Absyn_Stmt*_tmp555;struct Cyc_Absyn_Stmt*_tmp554;switch(*((int*)_tmp551)){case 2U: _LL1B: _tmp555=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp551)->f1;_tmp554=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp551)->f2;_LL1C:
 _tmp563=_tmp554;goto _LL1A;case 12U: _LL1D: _tmp557=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp551)->f1;_tmp556=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp551)->f2;_LL1E:
 _tmp563=_tmp556;goto _LL1A;case 1U: _LL1F: _tmp558=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp551)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp558);default: _LL21: _LL22:
({void*_tmp552=0U;({struct _fat_ptr _tmp7AB=({const char*_tmp553="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp553,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7AB,_tag_fat(_tmp552,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 3114
 return 1;}_LL0:;}
# 3131 "tcutil.cyc"
struct _tuple13 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 3133
struct _tuple13 bogus_ans=({struct _tuple13 _tmp652;_tmp652.f1=0,_tmp652.f2=Cyc_Absyn_heap_rgn_type;_tmp652;});
void*_tmp564=e->r;void*_tmp565=_tmp564;struct Cyc_Absyn_Exp*_tmp59E;struct Cyc_Absyn_Exp*_tmp59D;struct Cyc_Absyn_Exp*_tmp59C;struct Cyc_Absyn_Exp*_tmp59B;struct _fat_ptr*_tmp59A;int _tmp599;struct Cyc_Absyn_Exp*_tmp598;struct _fat_ptr*_tmp597;int _tmp596;void*_tmp595;switch(*((int*)_tmp565)){case 1U: _LL1: _tmp595=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_LL2: {
# 3137
void*_tmp566=_tmp595;struct Cyc_Absyn_Vardecl*_tmp56E;struct Cyc_Absyn_Vardecl*_tmp56D;struct Cyc_Absyn_Vardecl*_tmp56C;struct Cyc_Absyn_Vardecl*_tmp56B;switch(*((int*)_tmp566)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp56B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp566)->f1;_LL13: {
# 3141
void*_tmp567=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp568=_tmp567;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp568)->tag == 4U){_LL1B: _LL1C:
# 3143
 return({struct _tuple13 _tmp646;_tmp646.f1=1,_tmp646.f2=Cyc_Absyn_heap_rgn_type;_tmp646;});}else{_LL1D: _LL1E:
 return({struct _tuple13 _tmp647;_tmp647.f1=(_tmp56B->tq).real_const,_tmp647.f2=Cyc_Absyn_heap_rgn_type;_tmp647;});}_LL1A:;}case 4U: _LL14: _tmp56C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp566)->f1;_LL15: {
# 3147
void*_tmp569=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp56A=_tmp569;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp56A)->tag == 4U){_LL20: _LL21:
 return({struct _tuple13 _tmp648;_tmp648.f1=1,_tmp648.f2=(void*)_check_null(_tmp56C->rgn);_tmp648;});}else{_LL22: _LL23:
# 3150
 _tmp56C->escapes=1;
return({struct _tuple13 _tmp649;_tmp649.f1=(_tmp56C->tq).real_const,_tmp649.f2=(void*)_check_null(_tmp56C->rgn);_tmp649;});}_LL1F:;}case 5U: _LL16: _tmp56D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp566)->f1;_LL17:
# 3153
 _tmp56E=_tmp56D;goto _LL19;default: _LL18: _tmp56E=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp566)->f1;_LL19:
# 3155
 _tmp56E->escapes=1;
return({struct _tuple13 _tmp64A;_tmp64A.f1=(_tmp56E->tq).real_const,_tmp64A.f2=(void*)_check_null(_tmp56E->rgn);_tmp64A;});}_LLD:;}case 21U: _LL3: _tmp598=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_tmp597=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp565)->f2;_tmp596=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp565)->f3;_LL4:
# 3160
 if(_tmp596)return bogus_ans;{
# 3163
void*_tmp56F=Cyc_Tcutil_compress((void*)_check_null(_tmp598->topt));void*_tmp570=_tmp56F;struct Cyc_Absyn_Aggrdecl*_tmp57C;struct Cyc_List_List*_tmp57B;switch(*((int*)_tmp570)){case 7U: _LL25: _tmp57B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp570)->f2;_LL26: {
# 3165
struct Cyc_Absyn_Aggrfield*_tmp571=Cyc_Absyn_lookup_field(_tmp57B,_tmp597);
if(_tmp571 != 0 && _tmp571->width == 0){
struct _tuple13 _tmp572=Cyc_Tcutil_addressof_props(te,_tmp598);struct _tuple13 _tmp573=_tmp572;int _tmp575;void*_tmp574;_LL2C: _tmp575=_tmp573.f1;_tmp574=_tmp573.f2;_LL2D:;
return({struct _tuple13 _tmp64B;_tmp64B.f1=(_tmp571->tq).real_const || _tmp575,_tmp64B.f2=_tmp574;_tmp64B;});}
# 3170
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp57C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->f1).KnownAggr).val;_LL28: {
# 3172
struct Cyc_Absyn_Aggrfield*_tmp576=Cyc_Absyn_lookup_decl_field(_tmp57C,_tmp597);
if(_tmp576 != 0 && _tmp576->width == 0){
struct _tuple13 _tmp577=Cyc_Tcutil_addressof_props(te,_tmp598);struct _tuple13 _tmp578=_tmp577;int _tmp57A;void*_tmp579;_LL2F: _tmp57A=_tmp578.f1;_tmp579=_tmp578.f2;_LL30:;
return({struct _tuple13 _tmp64C;_tmp64C.f1=(_tmp576->tq).real_const || _tmp57A,_tmp64C.f2=_tmp579;_tmp64C;});}
# 3177
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp59B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_tmp59A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp565)->f2;_tmp599=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp565)->f3;_LL6:
# 3182
 if(_tmp599)return bogus_ans;{
# 3185
void*_tmp57D=Cyc_Tcutil_compress((void*)_check_null(_tmp59B->topt));void*_tmp57E=_tmp57D;void*_tmp584;void*_tmp583;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57E)->tag == 3U){_LL32: _tmp584=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57E)->f1).elt_type;_tmp583=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57E)->f1).ptr_atts).rgn;_LL33: {
# 3187
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp57F=Cyc_Tcutil_compress(_tmp584);void*_tmp580=_tmp57F;struct Cyc_Absyn_Aggrdecl*_tmp582;struct Cyc_List_List*_tmp581;switch(*((int*)_tmp580)){case 7U: _LL37: _tmp581=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp580)->f2;_LL38:
# 3190
 finfo=Cyc_Absyn_lookup_field(_tmp581,_tmp59A);goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp580)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp580)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp582=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp580)->f1)->f1).KnownAggr).val;_LL3A:
# 3192
 finfo=Cyc_Absyn_lookup_decl_field(_tmp582,_tmp59A);goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3195
if(finfo != 0 && finfo->width == 0)
return({struct _tuple13 _tmp64D;_tmp64D.f1=(finfo->tq).real_const,_tmp64D.f2=_tmp583;_tmp64D;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp59C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_LL8: {
# 3202
void*_tmp585=Cyc_Tcutil_compress((void*)_check_null(_tmp59C->topt));void*_tmp586=_tmp585;struct Cyc_Absyn_Tqual _tmp588;void*_tmp587;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp586)->tag == 3U){_LL3E: _tmp588=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp586)->f1).elt_tq;_tmp587=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp586)->f1).ptr_atts).rgn;_LL3F:
# 3204
 return({struct _tuple13 _tmp64E;_tmp64E.f1=_tmp588.real_const,_tmp64E.f2=_tmp587;_tmp64E;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp59E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp565)->f1;_tmp59D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp565)->f2;_LLA: {
# 3210
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp59E->topt));
void*_tmp589=t;struct Cyc_Absyn_Tqual _tmp592;struct Cyc_Absyn_Tqual _tmp591;void*_tmp590;struct Cyc_List_List*_tmp58F;switch(*((int*)_tmp589)){case 6U: _LL43: _tmp58F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp589)->f1;_LL44: {
# 3214
struct _tuple12 _tmp58A=Cyc_Evexp_eval_const_uint_exp(_tmp59D);struct _tuple12 _tmp58B=_tmp58A;unsigned _tmp58E;int _tmp58D;_LL4C: _tmp58E=_tmp58B.f1;_tmp58D=_tmp58B.f2;_LL4D:;
if(!_tmp58D)
return bogus_ans;{
struct _tuple11*_tmp58C=Cyc_Absyn_lookup_tuple_field(_tmp58F,(int)_tmp58E);
if(_tmp58C != 0)
return({struct _tuple13 _tmp64F;_tmp64F.f1=((*_tmp58C).f1).real_const,({void*_tmp7AC=(Cyc_Tcutil_addressof_props(te,_tmp59E)).f2;_tmp64F.f2=_tmp7AC;});_tmp64F;});
return bogus_ans;};}case 3U: _LL45: _tmp591=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).elt_tq;_tmp590=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).ptr_atts).rgn;_LL46:
# 3222
 return({struct _tuple13 _tmp650;_tmp650.f1=_tmp591.real_const,_tmp650.f2=_tmp590;_tmp650;});case 4U: _LL47: _tmp592=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp589)->f1).tq;_LL48:
# 3228
 return({struct _tuple13 _tmp651;_tmp651.f1=_tmp592.real_const,({void*_tmp7AD=(Cyc_Tcutil_addressof_props(te,_tmp59E)).f2;_tmp651.f2=_tmp7AD;});_tmp651;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3232
({void*_tmp593=0U;({unsigned _tmp7AF=e->loc;struct _fat_ptr _tmp7AE=({const char*_tmp594="unary & applied to non-lvalue";_tag_fat(_tmp594,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp7AF,_tmp7AE,_tag_fat(_tmp593,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3239
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp59F=({void*_tmp7B0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7B0,b);});
if(_tmp59F == 0)return;{
struct Cyc_Absyn_Exp*_tmp5A0=_tmp59F;
struct _tuple12 _tmp5A1=Cyc_Evexp_eval_const_uint_exp(_tmp5A0);struct _tuple12 _tmp5A2=_tmp5A1;unsigned _tmp5AC;int _tmp5AB;_LL1: _tmp5AC=_tmp5A2.f1;_tmp5AB=_tmp5A2.f2;_LL2:;
if(_tmp5AB && _tmp5AC <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp5A5=({struct Cyc_Int_pa_PrintArg_struct _tmp654;_tmp654.tag=1U,_tmp654.f1=(unsigned long)((int)_tmp5AC);_tmp654;});struct Cyc_Int_pa_PrintArg_struct _tmp5A6=({struct Cyc_Int_pa_PrintArg_struct _tmp653;_tmp653.tag=1U,_tmp653.f1=(unsigned long)((int)i);_tmp653;});void*_tmp5A3[2U];_tmp5A3[0]=& _tmp5A5,_tmp5A3[1]=& _tmp5A6;({unsigned _tmp7B2=loc;struct _fat_ptr _tmp7B1=({const char*_tmp5A4="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp5A4,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp7B2,_tmp7B1,_tag_fat(_tmp5A3,sizeof(void*),2U));});});else{
# 3248
({struct Cyc_Int_pa_PrintArg_struct _tmp5A9=({struct Cyc_Int_pa_PrintArg_struct _tmp656;_tmp656.tag=1U,_tmp656.f1=(unsigned long)((int)_tmp5AC);_tmp656;});struct Cyc_Int_pa_PrintArg_struct _tmp5AA=({struct Cyc_Int_pa_PrintArg_struct _tmp655;_tmp655.tag=1U,_tmp655.f1=(unsigned long)((int)i);_tmp655;});void*_tmp5A7[2U];_tmp5A7[0]=& _tmp5A9,_tmp5A7[1]=& _tmp5AA;({unsigned _tmp7B4=loc;struct _fat_ptr _tmp7B3=({const char*_tmp5A8="dereference is out of bounds: %d <= %d";_tag_fat(_tmp5A8,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp7B4,_tmp7B3,_tag_fat(_tmp5A7,sizeof(void*),2U));});});}}
return;};}
# 3252
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3261
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp5AD=e->r;void*_tmp5AE=_tmp5AD;struct Cyc_List_List*_tmp5CD;struct Cyc_List_List*_tmp5CC;enum Cyc_Absyn_Primop _tmp5CB;struct Cyc_List_List*_tmp5CA;struct Cyc_List_List*_tmp5C9;struct Cyc_List_List*_tmp5C8;struct Cyc_List_List*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C4;struct Cyc_Absyn_Exp*_tmp5C3;void*_tmp5C2;struct Cyc_Absyn_Exp*_tmp5C1;void*_tmp5C0;struct Cyc_Absyn_Exp*_tmp5BF;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BC;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5B9;struct Cyc_Absyn_Exp*_tmp5B8;struct Cyc_Absyn_Exp*_tmp5B7;void*_tmp5B6;switch(*((int*)_tmp5AE)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5B6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL10: {
# 3273
void*_tmp5AF=_tmp5B6;struct Cyc_Absyn_Vardecl*_tmp5B5;struct Cyc_Absyn_Vardecl*_tmp5B4;switch(*((int*)_tmp5AF)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp5B4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5AF)->f1;_LL37: {
# 3276
void*_tmp5B0=Cyc_Tcutil_compress(_tmp5B4->type);void*_tmp5B1=_tmp5B0;switch(*((int*)_tmp5B1)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp5B5=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5AF)->f1;_LL39:
# 3283
 if((int)_tmp5B5->sc == (int)Cyc_Absyn_Static){
void*_tmp5B2=Cyc_Tcutil_compress(_tmp5B5->type);void*_tmp5B3=_tmp5B2;switch(*((int*)_tmp5B3)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3290
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp5B9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_tmp5B8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5AE)->f2;_tmp5B7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5AE)->f3;_LL12:
# 3296
 return(Cyc_Tcutil_cnst_exp(0,_tmp5B9)&&
 Cyc_Tcutil_cnst_exp(0,_tmp5B8))&&
 Cyc_Tcutil_cnst_exp(0,_tmp5B7);case 9U: _LL13: _tmp5BB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_tmp5BA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5AE)->f2;_LL14:
# 3300
 return Cyc_Tcutil_cnst_exp(0,_tmp5BB)&& Cyc_Tcutil_cnst_exp(0,_tmp5BA);case 41U: _LL15: _tmp5BC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL16:
 _tmp5BD=_tmp5BC;goto _LL18;case 12U: _LL17: _tmp5BD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL18:
 _tmp5BE=_tmp5BD;goto _LL1A;case 13U: _LL19: _tmp5BE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL1A:
# 3304
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5BE);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5AE)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp5C0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_tmp5BF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5AE)->f2;_LL1C:
# 3306
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5BF);}else{_LL1D: _tmp5C2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_tmp5C1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5AE)->f2;_LL1E:
# 3309
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5C1);}case 15U: _LL1F: _tmp5C3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL20:
# 3311
 return Cyc_Tcutil_cnst_exp(1,_tmp5C3);case 27U: _LL21: _tmp5C5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5AE)->f2;_tmp5C4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5AE)->f3;_LL22:
# 3313
 return Cyc_Tcutil_cnst_exp(0,_tmp5C5)&& Cyc_Tcutil_cnst_exp(0,_tmp5C4);case 28U: _LL23: _tmp5C6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL24:
# 3315
 return Cyc_Tcutil_cnst_exp(0,_tmp5C6);case 26U: _LL25: _tmp5C7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL26:
 _tmp5C8=_tmp5C7;goto _LL28;case 30U: _LL27: _tmp5C8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5AE)->f2;_LL28:
 _tmp5C9=_tmp5C8;goto _LL2A;case 29U: _LL29: _tmp5C9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5AE)->f3;_LL2A:
# 3319
 for(0;_tmp5C9 != 0;_tmp5C9=_tmp5C9->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple15*)_tmp5C9->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp5CB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_tmp5CA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5AE)->f2;_LL2C:
# 3324
 _tmp5CC=_tmp5CA;goto _LL2E;case 24U: _LL2D: _tmp5CC=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL2E:
 _tmp5CD=_tmp5CC;goto _LL30;case 31U: _LL2F: _tmp5CD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5AE)->f1;_LL30:
# 3327
 for(0;_tmp5CD != 0;_tmp5CD=_tmp5CD->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp5CD->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3334
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3338
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5CE=Cyc_Tcutil_compress(t);void*_tmp5CF=_tmp5CE;struct Cyc_List_List*_tmp5DF;struct Cyc_List_List*_tmp5DE;void*_tmp5DD;void*_tmp5DC;void*_tmp5DB;struct Cyc_List_List*_tmp5DA;switch(*((int*)_tmp5CF)){case 0U: _LL1: _tmp5DB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5CF)->f1;_tmp5DA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5CF)->f2;_LL2: {
# 3342
void*_tmp5D0=_tmp5DB;union Cyc_Absyn_AggrInfo _tmp5D9;struct Cyc_List_List*_tmp5D8;struct Cyc_Absyn_Enumdecl*_tmp5D7;switch(*((int*)_tmp5D0)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5D7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5D0)->f2;_LL15:
# 3347
 if(_tmp5D7 == 0 || _tmp5D7->fields == 0)
return 0;
_tmp5D8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5D7->fields))->v;goto _LL17;case 16U: _LL16: _tmp5D8=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5D0)->f1;_LL17:
# 3351
{struct Cyc_List_List*_tmp5D1=_tmp5D8;for(0;_tmp5D1 != 0;_tmp5D1=_tmp5D1->tl){
if(((struct Cyc_Absyn_Enumfield*)_tmp5D1->hd)->tag == 0)
return _tmp5D1 == _tmp5D8;{
struct _tuple12 _tmp5D2=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_tmp5D1->hd)->tag));struct _tuple12 _tmp5D3=_tmp5D2;unsigned _tmp5D5;int _tmp5D4;_LL1D: _tmp5D5=_tmp5D3.f1;_tmp5D4=_tmp5D3.f2;_LL1E:;
if(_tmp5D4 && _tmp5D5 == (unsigned)0)
return 1;};}}
# 3358
return 0;case 20U: _LL18: _tmp5D9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5D0)->f1;_LL19: {
# 3361
struct Cyc_Absyn_Aggrdecl*_tmp5D6=Cyc_Absyn_get_known_aggrdecl(_tmp5D9);
if(_tmp5D6->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5D6->impl))->exist_vars != 0)return 0;
if((int)_tmp5D6->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5D6->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(_tmp5D6->tvs,_tmp5DA,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5D6->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5DC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5CF)->f1).ptr_atts).nullable;_LL4:
# 3369
 return Cyc_Tcutil_force_type2bool(1,_tmp5DC);case 4U: _LL5: _tmp5DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5CF)->f1).elt_type;_LL6:
 return Cyc_Tcutil_zeroable_type(_tmp5DD);case 6U: _LL7: _tmp5DE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5CF)->f1;_LL8:
# 3372
 for(0;_tmp5DE != 0;_tmp5DE=_tmp5DE->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple11*)_tmp5DE->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp5DF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5CF)->f2;_LLA:
 return Cyc_Tcutil_fields_zeroable(0,0,_tmp5DF);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3379
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3381
struct _RegionHandle _tmp5E0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5E0;_push_region(rgn);
{struct Cyc_List_List*_tmp5E1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,_tmp5E1,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5E2=0;_npop_handler(0U);return _tmp5E2;}}{
# 3389
int _tmp5E3=1;_npop_handler(0U);return _tmp5E3;};}
# 3382
;_pop_region(rgn);}
# 3393
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5E4=t;struct Cyc_Absyn_Typedefdecl*_tmp5E9;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E4)->tag == 8U){_LL1: _tmp5E9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E4)->f3;_LL2:
# 3396
 if(_tmp5E9 != 0){
struct Cyc_Absyn_Tqual _tmp5E5=_tmp5E9->tq;
if(((_tmp5E5.print_const || _tmp5E5.q_volatile)|| _tmp5E5.q_restrict)|| _tmp5E5.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp5E8=({struct Cyc_String_pa_PrintArg_struct _tmp657;_tmp657.tag=0U,({struct _fat_ptr _tmp7B5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp657.f1=_tmp7B5;});_tmp657;});void*_tmp5E6[1U];_tmp5E6[0]=& _tmp5E8;({unsigned _tmp7B7=loc;struct _fat_ptr _tmp7B6=({const char*_tmp5E7="qualifier within typedef type %s is ignored";_tag_fat(_tmp5E7,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp7B7,_tmp7B6,_tag_fat(_tmp5E6,sizeof(void*),1U));});});}
# 3401
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3408
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp5EA=Cyc_Tcutil_compress(t);void*_tmp5EB=_tmp5EA;struct Cyc_List_List**_tmp5F1;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5EB)->tag == 5U){_LL1: _tmp5F1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5EB)->f1).attributes;_LL2: {
# 3411
struct Cyc_List_List*_tmp5EC=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*_tmp5F1,(void*)atts->hd))
({struct Cyc_List_List*_tmp7B8=({struct Cyc_List_List*_tmp5ED=_cycalloc(sizeof(*_tmp5ED));_tmp5ED->hd=(void*)atts->hd,_tmp5ED->tl=*_tmp5F1;_tmp5ED;});*_tmp5F1=_tmp7B8;});}else{
# 3418
_tmp5EC=({struct Cyc_List_List*_tmp5EE=_cycalloc(sizeof(*_tmp5EE));_tmp5EE->hd=(void*)atts->hd,_tmp5EE->tl=_tmp5EC;_tmp5EE;});}}
return _tmp5EC;}}else{_LL3: _LL4:
({void*_tmp5EF=0U;({struct _fat_ptr _tmp7B9=({const char*_tmp5F0="transfer_fn_type_atts";_tag_fat(_tmp5F0,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7B9,_tag_fat(_tmp5EF,sizeof(void*),0U));});});}_LL0:;}
# 3425
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5F2=Cyc_Tcutil_compress(t);void*_tmp5F3=_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F5;struct Cyc_Absyn_PtrInfo _tmp5F4;switch(*((int*)_tmp5F3)){case 3U: _LL1: _tmp5F4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5F3)->f1;_LL2:
 return({void*_tmp7BA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7BA,(_tmp5F4.ptr_atts).bounds);});case 4U: _LL3: _tmp5F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F3)->f1).num_elts;_LL4:
 return _tmp5F5;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3435
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5F6=b;struct Cyc_Absyn_Vardecl*_tmp5FA;struct Cyc_Absyn_Vardecl*_tmp5F9;struct Cyc_Absyn_Vardecl*_tmp5F8;struct Cyc_Absyn_Vardecl*_tmp5F7;switch(*((int*)_tmp5F6)){case 5U: _LL1: _tmp5F7=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5F6)->f1;_LL2:
 _tmp5F8=_tmp5F7;goto _LL4;case 4U: _LL3: _tmp5F8=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5F6)->f1;_LL4:
 _tmp5F9=_tmp5F8;goto _LL6;case 3U: _LL5: _tmp5F9=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5F6)->f1;_LL6:
 _tmp5FA=_tmp5F9;goto _LL8;case 1U: _LL7: _tmp5FA=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5F6)->f1;_LL8:
# 3441
 if(!_tmp5FA->escapes)
return _tmp5FA;
return 0;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3449
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5FB=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmp5FB=({struct Cyc_List_List*_tmp5FC=_cycalloc(sizeof(*_tmp5FC));_tmp5FC->hd=*((void**)_check_null((void**)x->hd)),_tmp5FC->tl=_tmp5FB;_tmp5FC;});}}
return _tmp5FB;}
# 3456
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5FD=Cyc_Tcutil_compress(t);void*_tmp5FE=_tmp5FD;void*_tmp608;struct Cyc_Absyn_Tqual _tmp607;struct Cyc_Absyn_Exp*_tmp606;void*_tmp605;unsigned _tmp604;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5FE)->tag == 4U){_LL1: _tmp608=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5FE)->f1).elt_type;_tmp607=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5FE)->f1).tq;_tmp606=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5FE)->f1).num_elts;_tmp605=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5FE)->f1).zero_term;_tmp604=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5FE)->f1).zt_loc;_LL2: {
# 3459
void*b;
if(_tmp606 == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 3463
if(convert_tag){
if(_tmp606->topt == 0)
({void*_tmp5FF=0U;({struct _fat_ptr _tmp7BB=({const char*_tmp600="cannot convert tag without type!";_tag_fat(_tmp600,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7BB,_tag_fat(_tmp5FF,sizeof(void*),0U));});});{
void*_tmp601=Cyc_Tcutil_compress((void*)_check_null(_tmp606->topt));void*_tmp602=_tmp601;void*_tmp603;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp602)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp602)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp602)->f2 != 0){_LL6: _tmp603=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp602)->f2)->hd;_LL7:
# 3469
 b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(_tmp603,0U));
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 3472
 b=Cyc_Tcutil_is_const_exp(_tmp606)?Cyc_Absyn_thin_bounds_exp(_tmp606): Cyc_Absyn_fat_bound_type;}_LL5:;};}else{
# 3476
b=Cyc_Absyn_thin_bounds_exp(_tmp606);}}
# 3478
return Cyc_Absyn_atb_type(_tmp608,rgn,_tmp607,b,_tmp605);}}else{_LL3: _LL4:
 return t;}_LL0:;}
