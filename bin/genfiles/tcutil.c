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
# 116 "core.h"
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 61
extern int Cyc_List_length(struct Cyc_List_List*x);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);
# 83
extern struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 86
extern struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 94
extern struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y);
# 133
extern void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 161
extern struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 205
extern struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 270
extern struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y);
# 276
extern struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
extern struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x);
# 322
extern int Cyc_List_mem(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x);
# 336
extern void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x);
# 383
extern int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
extern int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 291
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 311
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 506
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 513
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 531
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 885 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 888
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 895
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 901
void*Cyc_Absyn_compress(void*);
# 906
int Cyc_Absyn_type2bool(int def,void*);
# 915
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 920
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 922
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 927
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 929
extern void*Cyc_Absyn_empty_effect;
# 931
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 933
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 958
extern void*Cyc_Absyn_fat_bound_type;
# 960
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 962
void*Cyc_Absyn_bounds_one (void);
# 964
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 968
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 987
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 989
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 992
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1027
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1034
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
# 1046
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1055
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1094
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1138
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1140
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1142
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1152
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 76 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 28 "kinds.h"
extern struct Cyc_Absyn_Kind Cyc_Kinds_rk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_bk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_mk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ek;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ik;
extern struct Cyc_Absyn_Kind Cyc_Kinds_boolk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_ptrbk;
# 37
extern struct Cyc_Absyn_Kind Cyc_Kinds_trk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tak;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tbk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_tmk;
# 42
extern struct Cyc_Absyn_Kind Cyc_Kinds_urk;
# 44
extern struct Cyc_Absyn_Kind Cyc_Kinds_ubk;
extern struct Cyc_Absyn_Kind Cyc_Kinds_umk;
# 52
extern struct Cyc_Core_Opt Cyc_Kinds_eko;
extern struct Cyc_Core_Opt Cyc_Kinds_boolko;
extern struct Cyc_Core_Opt Cyc_Kinds_ptrbko;
# 66
struct Cyc_Core_Opt*Cyc_Kinds_kind_to_opt(struct Cyc_Absyn_Kind*);
# 68
void*Cyc_Kinds_copy_kindbound(void*);
# 73
struct Cyc_Absyn_Kind*Cyc_Kinds_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
# 78
void*Cyc_Kinds_compress_kb(void*);
# 82
int Cyc_Kinds_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 27 "warn.h"
void Cyc_Warn_vwarn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 33
void Cyc_Warn_verr(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 38
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 67
void Cyc_Warn_err2(unsigned,struct _fat_ptr);
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);
# 71
void*Cyc_Warn_impos2(struct _fat_ptr);
# 47 "flags.h"
extern int Cyc_Flags_warn_alias_coerce;
# 51
extern int Cyc_Flags_warn_region_coerce;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
extern int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 50
extern int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 28 "unify.h"
int Cyc_Unify_unify_kindbound(void*,void*);
int Cyc_Unify_unify(void*,void*);
# 32
void Cyc_Unify_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
extern union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned);extern union Cyc_Relations_RelnOp Cyc_Relations_RReturn (void);
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
# 37 "tcutil.h"
int Cyc_Tcutil_is_char_type(void*);
int Cyc_Tcutil_is_any_int_type(void*);
int Cyc_Tcutil_is_any_float_type(void*);
int Cyc_Tcutil_is_integral_type(void*);
int Cyc_Tcutil_is_arithmetic_type(void*);
# 45
int Cyc_Tcutil_is_pointer_type(void*);
# 58
int Cyc_Tcutil_is_bits_only_type(void*);
# 71
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 76
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_dest);
# 80
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type);
# 83
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b);
# 86
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*);
# 88
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*);
# 93
void*Cyc_Tcutil_copy_type(void*);
# 96
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*);
# 100
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 103
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
# 106
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 110
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 112
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 114
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*assume,void*t1,void*t2);
# 117
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);
# 125
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 128
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 131
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 137
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 140
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 144
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 164
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 177
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 180
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 185
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 190
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple14{int f1;void*f2;};
# 194
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 197
void*Cyc_Tcutil_normalize_effect(void*e);
# 200
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 202
int Cyc_Tcutil_new_tvar_id (void);
# 204
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 210
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 214
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 228
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 231
int Cyc_Tcutil_zeroable_type(void*);
# 235
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 243
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 246
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 251
int Cyc_Tcutil_tycon_cmp(void*,void*);
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*,void*);
# 38 "attributes.h"
int Cyc_Atts_sub_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 41
int Cyc_Atts_fntype_att(void*);
# 52
int Cyc_Atts_attribute_cmp(void*,void*);
# 39 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Absyn_compress(t);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 45
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Absyn_compress(t);void*_stmttmp1=_tmp2;void*_tmp3=_stmttmp1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 51
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp4;void*_tmp5=_stmttmp2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 57
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Absyn_compress(t);void*_stmttmp3=_tmp6;void*_tmp7=_stmttmp3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 64
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Absyn_compress(t);void*_stmttmp4=_tmp8;void*_tmp9=_stmttmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 71
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Absyn_compress(t);void*_stmttmp5=_tmpA;void*_tmpB=_stmttmp5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 78
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Absyn_compress(t);void*_stmttmp6=_tmpC;void*_tmpD=_stmttmp6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 85
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Absyn_compress(t);void*_stmttmp7=_tmpE;void*_tmpF=_stmttmp7;void*_tmp10;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {void*c=_tmp10;
# 88
void*_tmp11=c;switch(*((int*)_tmp11)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 95
 return 0;}_LL0:;}
# 98
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Absyn_compress(t);void*_stmttmp8=_tmp12;void*_tmp13=_stmttmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
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
void*_tmp14=Cyc_Absyn_compress(t);void*_stmttmp9=_tmp14;void*_tmp15=_stmttmp9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 117
int Cyc_Tcutil_is_typedef_type(void*t){
void*_tmp16=Cyc_Absyn_compress(t);void*_stmttmpA=_tmp16;void*_tmp17=_stmttmpA;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 123
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 131
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp18=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpB=_tmp18;void*_tmp19=_stmttmpB;void*_tmp1A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19)->tag == 1U){_LL1: _LL2:
 return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp1A=_tmp19;_LL4: {void*t=_tmp1A;
return Cyc_Tcutil_is_integral_type(t);}}_LL0:;}
# 139
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp1B=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpC=_tmp1B;void*_tmp1C=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 149
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1D=Cyc_Absyn_compress(t);void*_stmttmpD=_tmp1D;void*_tmp1E=_stmttmpD;void*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->tag == 3U){_LL1: _tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->f1).ptr_atts).zero_term;_LL2: {void*ztl=_tmp1F;
# 152
return Cyc_Tcutil_force_type2bool(0,ztl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 158
int Cyc_Tcutil_is_nullable_pointer_type(void*t,int def){
void*_tmp20=Cyc_Absyn_compress(t);void*_stmttmpE=_tmp20;void*_tmp21=_stmttmpE;void*_tmp22;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->tag == 3U){_LL1: _tmp22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp22;
# 161
return Cyc_Tcutil_force_type2bool(def,nbl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 167
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp23=Cyc_Absyn_compress(t);void*_stmttmpF=_tmp23;void*_tmp24=_stmttmpF;void*_tmp25;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->tag == 3U){_LL1: _tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp25;
# 170
return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,b);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 177
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp26=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp26;void*_tmp27=_stmttmp10;void*_tmp29;void*_tmp28;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->tag == 3U){_LL1: _tmp28=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).elt_type;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).ptr_atts).bounds;_LL2: {void*elt_type=_tmp28;void*b=_tmp29;
# 180
if(!Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type))
return 0;
*elt_type_dest=elt_type;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 190
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp2A=Cyc_Absyn_compress(t);void*_stmttmp11=_tmp2A;void*_tmp2B=_stmttmp11;void*_tmp2D;void*_tmp2C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->tag == 3U){_LL1: _tmp2C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_type;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;_LL2: {void*elt_type=_tmp2C;void*zt=_tmp2D;
# 193
*elt_type_dest=elt_type;
return Cyc_Absyn_type2bool(0,zt);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 202
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 204
void*_tmp2E=Cyc_Absyn_compress(t);void*_stmttmp12=_tmp2E;void*_tmp2F=_stmttmp12;void*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Tqual _tmp31;void*_tmp30;void*_tmp36;void*_tmp35;void*_tmp34;switch(*((int*)_tmp2F)){case 3U: _LL1: _tmp34=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp35=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).bounds;_tmp36=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).zero_term;_LL2: {void*elt=_tmp34;void*bnds=_tmp35;void*zt=_tmp36;
# 206
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*ptr_type=t;
*elt_type=elt;
{void*_tmp37=Cyc_Absyn_compress(bnds);void*_stmttmp13=_tmp37;void*_tmp38=_stmttmp13;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 214
return 1;}case 4U: _LL3: _tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).tq;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).zero_term;_LL4: {void*elt=_tmp30;struct Cyc_Absyn_Tqual tq=_tmp31;struct Cyc_Absyn_Exp*n=_tmp32;void*zt=_tmp33;
# 216
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*elt_type=elt;
*is_fat=0;
({void*_tmp669=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp669;});
return 1;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 229
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 235
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp39=({void*_tmp66A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp66A,b);});struct Cyc_Absyn_Exp*eopt=_tmp39;
if(eopt == 0)return 0;{
struct _tuple13 _tmp3A=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp14=_tmp3A;struct _tuple13 _tmp3B=_stmttmp14;int _tmp3D;unsigned _tmp3C;_LL1: _tmp3C=_tmp3B.f1;_tmp3D=_tmp3B.f2;_LL2: {unsigned i=_tmp3C;int known=_tmp3D;
return known && i == (unsigned)1;}}}
# 243
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3E=Cyc_Absyn_compress(t);void*_stmttmp15=_tmp3E;void*_tmp3F=_stmttmp15;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;void*_tmp43;void*_tmp42;struct Cyc_List_List*_tmp45;void*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LL1: _tmp44=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f2;_LL2: {void*c=_tmp44;struct Cyc_List_List*ts=_tmp45;
# 246
void*_tmp46=c;struct Cyc_Absyn_Aggrdecl*_tmp47;switch(*((int*)_tmp46)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 goto _LL13;case 16U: _LL12: _LL13:
 goto _LL15;case 15U: _LL14: _LL15:
 return 1;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp46)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
# 253
 return 0;}else{_LL18: _tmp47=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp46)->f1).KnownAggr).val;_LL19: {struct Cyc_Absyn_Aggrdecl*ad=_tmp47;
# 255
if(ad->impl == 0)
return 0;{
struct _RegionHandle _tmp48=_new_region("rgn");struct _RegionHandle*rgn=& _tmp48;_push_region(rgn);
{struct Cyc_List_List*_tmp49=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,ad->tvs,ts);struct Cyc_List_List*inst=_tmp49;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp4A=0;_npop_handler(0U);return _tmp4A;}}}{
int _tmp4B=1;_npop_handler(0U);return _tmp4B;}}
# 258
;_pop_region();}}}default: _LL1A: _LL1B:
# 263
 return 0;}_LLB:;}case 4U: _LL3: _tmp42=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp43=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;_LL4: {void*t=_tmp42;void*zero_term=_tmp43;
# 268
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6U: _LL5: _tmp41=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F)->f1;_LL6: {struct Cyc_List_List*tqs=_tmp41;
# 270
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7U: _LL7: _tmp40=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F)->f2;_LL8: {struct Cyc_List_List*fs=_tmp40;
# 275
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 283
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 285
Cyc_Warn_verr(loc,fmt,ap);}
# 287
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 289
Cyc_Warn_vimpos(fmt,ap);}
# 291
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 293
Cyc_Warn_vwarn(sg,fmt,ap);}
# 297
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 301
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->name=tv->name,_tmp4C->identity=- 1,({void*_tmp66B=Cyc_Kinds_copy_kindbound(tv->kind);_tmp4C->kind=_tmp66B;});_tmp4C;});}
# 304
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9*_tmp4D=arg;void*_tmp50;struct Cyc_Absyn_Tqual _tmp4F;struct _fat_ptr*_tmp4E;_LL1: _tmp4E=_tmp4D->f1;_tmp4F=_tmp4D->f2;_tmp50=_tmp4D->f3;_LL2: {struct _fat_ptr*x=_tmp4E;struct Cyc_Absyn_Tqual y=_tmp4F;void*t=_tmp50;
return({struct _tuple9*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->f1=x,_tmp51->f2=y,({void*_tmp66C=Cyc_Tcutil_copy_type(t);_tmp51->f3=_tmp66C;});_tmp51;});}}
# 308
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp52=_cycalloc(sizeof(*_tmp52));_tmp52->f1=(*arg).f1,({void*_tmp66D=Cyc_Tcutil_copy_type((*arg).f2);_tmp52->f2=_tmp66D;});_tmp52;});}
# 311
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
return e == 0?0: Cyc_Tcutil_deep_copy_exp(preserve_types,e);}
# 314
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->name=f->name,_tmp53->tq=f->tq,({void*_tmp66F=Cyc_Tcutil_copy_type(f->type);_tmp53->type=_tmp66F;}),_tmp53->width=f->width,_tmp53->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp53->requires_clause=_tmp66E;});_tmp53;});}struct _tuple15{void*f1;void*f2;};
# 318
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp54=x;void*_tmp56;void*_tmp55;_LL1: _tmp55=_tmp54->f1;_tmp56=_tmp54->f2;_LL2: {void*r1=_tmp55;void*r2=_tmp56;
return({struct _tuple15*_tmp57=_cycalloc(sizeof(*_tmp57));({void*_tmp671=Cyc_Tcutil_copy_type(r1);_tmp57->f1=_tmp671;}),({void*_tmp670=Cyc_Tcutil_copy_type(r2);_tmp57->f2=_tmp670;});_tmp57;});}}
# 322
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 326
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp58=Cyc_Absyn_compress(t);void*_stmttmp16=_tmp58;void*_tmp59=_stmttmp16;struct Cyc_Absyn_Datatypedecl*_tmp5A;struct Cyc_Absyn_Enumdecl*_tmp5B;struct Cyc_Absyn_Aggrdecl*_tmp5C;struct Cyc_Absyn_Typedefdecl*_tmp5F;struct Cyc_List_List*_tmp5E;struct _tuple1*_tmp5D;struct Cyc_Absyn_Exp*_tmp60;struct Cyc_Absyn_Exp*_tmp61;struct Cyc_List_List*_tmp63;enum Cyc_Absyn_AggrKind _tmp62;struct Cyc_List_List*_tmp64;struct Cyc_Absyn_Vardecl*_tmp72;struct Cyc_List_List*_tmp71;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_List_List*_tmp6F;struct Cyc_Absyn_Exp*_tmp6E;struct Cyc_List_List*_tmp6D;struct Cyc_List_List*_tmp6C;struct Cyc_Absyn_VarargInfo*_tmp6B;int _tmp6A;struct Cyc_List_List*_tmp69;void*_tmp68;struct Cyc_Absyn_Tqual _tmp67;void*_tmp66;struct Cyc_List_List*_tmp65;unsigned _tmp77;void*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_Absyn_Tqual _tmp74;void*_tmp73;struct Cyc_Absyn_PtrLoc*_tmp7E;void*_tmp7D;void*_tmp7C;void*_tmp7B;void*_tmp7A;struct Cyc_Absyn_Tqual _tmp79;void*_tmp78;struct Cyc_Absyn_Tvar*_tmp7F;struct Cyc_List_List*_tmp81;void*_tmp80;void*_tmp82;switch(*((int*)_tmp59)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f2 == 0){_LL1: _tmp82=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f1;_LL2: {void*c=_tmp82;
return t;}}else{_LL3: _tmp80=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f1;_tmp81=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59)->f2;_LL4: {void*c=_tmp80;struct Cyc_List_List*ts=_tmp81;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=0U,_tmp83->f1=c,({struct Cyc_List_List*_tmp672=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp83->f2=_tmp672;});_tmp83;});}}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp7F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp59)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp7F;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3U: _LL9: _tmp78=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).elt_type;_tmp79=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).elt_tq;_tmp7A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).rgn;_tmp7B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).nullable;_tmp7C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).bounds;_tmp7D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).zero_term;_tmp7E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp59)->f1).ptr_atts).ptrloc;_LLA: {void*elt=_tmp78;struct Cyc_Absyn_Tqual tq=_tmp79;void*rgn=_tmp7A;void*nbl=_tmp7B;void*bs=_tmp7C;void*zt=_tmp7D;struct Cyc_Absyn_PtrLoc*loc=_tmp7E;
# 333
void*_tmp84=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp84;
void*_tmp85=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp85;
void*_tmp86=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp86;
struct Cyc_Absyn_Tqual _tmp87=tq;struct Cyc_Absyn_Tqual tq2=_tmp87;
# 338
void*_tmp88=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp88;
void*_tmp89=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp89;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->tag=3U,(_tmp8A->f1).elt_type=elt2,(_tmp8A->f1).elt_tq=tq2,((_tmp8A->f1).ptr_atts).rgn=rgn2,((_tmp8A->f1).ptr_atts).nullable=nbl2,((_tmp8A->f1).ptr_atts).bounds=bs2,((_tmp8A->f1).ptr_atts).zero_term=zt2,((_tmp8A->f1).ptr_atts).ptrloc=loc;_tmp8A;});}case 4U: _LLB: _tmp73=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).elt_type;_tmp74=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).tq;_tmp75=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).num_elts;_tmp76=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).zero_term;_tmp77=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59)->f1).zt_loc;_LLC: {void*et=_tmp73;struct Cyc_Absyn_Tqual tq=_tmp74;struct Cyc_Absyn_Exp*eo=_tmp75;void*zt=_tmp76;unsigned ztl=_tmp77;
# 342
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp8B=_cycalloc(sizeof(*_tmp8B));_tmp8B->tag=4U,({void*_tmp675=Cyc_Tcutil_copy_type(et);(_tmp8B->f1).elt_type=_tmp675;}),(_tmp8B->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp674=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp8B->f1).num_elts=_tmp674;}),({
void*_tmp673=Cyc_Tcutil_copy_type(zt);(_tmp8B->f1).zero_term=_tmp673;}),(_tmp8B->f1).zt_loc=ztl;_tmp8B;});}case 5U: _LLD: _tmp65=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).tvars;_tmp66=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).effect;_tmp67=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).ret_tqual;_tmp68=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).ret_type;_tmp69=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).args;_tmp6A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).c_varargs;_tmp6B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).cyc_varargs;_tmp6C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).rgn_po;_tmp6D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).attributes;_tmp6E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).requires_clause;_tmp6F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).requires_relns;_tmp70=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).ensures_clause;_tmp71=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).ensures_relns;_tmp72=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp59)->f1).return_value;_LLE: {struct Cyc_List_List*tvs=_tmp65;void*effopt=_tmp66;struct Cyc_Absyn_Tqual rt_tq=_tmp67;void*rt=_tmp68;struct Cyc_List_List*args=_tmp69;int c_varargs=_tmp6A;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp6B;struct Cyc_List_List*rgn_po=_tmp6C;struct Cyc_List_List*atts=_tmp6D;struct Cyc_Absyn_Exp*req=_tmp6E;struct Cyc_List_List*req_rlns=_tmp6F;struct Cyc_Absyn_Exp*ens=_tmp70;struct Cyc_List_List*ens_rlns=_tmp71;struct Cyc_Absyn_Vardecl*ret_val=_tmp72;
# 345
struct Cyc_List_List*_tmp8C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp8C;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmp8D=Cyc_Tcutil_copy_type(rt);void*rt2=_tmp8D;
struct Cyc_List_List*_tmp8E=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmp8E;
int _tmp8F=c_varargs;int c_varargs2=_tmp8F;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=cyc_varargs == 0?0:({struct Cyc_Absyn_VarargInfo*_tmp98=_cycalloc(sizeof(*_tmp98));*_tmp98=*cyc_varargs;_tmp98;});
if(cyc_varargs != 0)
({void*_tmp676=Cyc_Tcutil_copy_type(cyc_varargs->type);((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_varargs2))->type=_tmp676;});{
struct Cyc_List_List*_tmp90=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmp90;
struct Cyc_List_List*_tmp91=atts;struct Cyc_List_List*atts2=_tmp91;
struct Cyc_Absyn_Exp*_tmp92=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmp92;
struct Cyc_List_List*_tmp93=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmp93;
struct Cyc_Absyn_Exp*_tmp94=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmp94;
struct Cyc_List_List*_tmp95=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmp95;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmp96=_cycalloc(sizeof(*_tmp96));*_tmp96=*ret_val;_tmp96;});
({void*_tmp677=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp677;});
if((unsigned)ret_val2->rgn)({void*_tmp678=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp678;});}
# 365
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->tag=5U,(_tmp97->f1).tvars=tvs2,(_tmp97->f1).effect=effopt2,(_tmp97->f1).ret_tqual=rt_tq,(_tmp97->f1).ret_type=rt2,(_tmp97->f1).args=args2,(_tmp97->f1).c_varargs=c_varargs2,(_tmp97->f1).cyc_varargs=cyc_varargs2,(_tmp97->f1).rgn_po=rgn_po2,(_tmp97->f1).attributes=atts2,(_tmp97->f1).requires_clause=req2,(_tmp97->f1).requires_relns=req_rlns2,(_tmp97->f1).ensures_clause=ens2,(_tmp97->f1).ensures_relns=ens_rlns2,(_tmp97->f1).return_value=ret_val2;_tmp97;});}}case 6U: _LLF: _tmp64=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp59)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp64;
# 368
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=6U,({struct Cyc_List_List*_tmp679=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmp99->f1=_tmp679;});_tmp99;});}case 7U: _LL11: _tmp62=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp59)->f1;_tmp63=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp59)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp62;struct Cyc_List_List*fs=_tmp63;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=7U,_tmp9A->f1=k,({struct Cyc_List_List*_tmp67A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmp9A->f2=_tmp67A;});_tmp9A;});}case 9U: _LL13: _tmp61=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp59)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp61;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp9B=_cycalloc(sizeof(*_tmp9B));_tmp9B->tag=9U,_tmp9B->f1=e;_tmp9B;});}case 11U: _LL15: _tmp60=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp59)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp60;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=11U,_tmp9C->f1=e;_tmp9C;});}case 8U: _LL17: _tmp5D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp59)->f1;_tmp5E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp59)->f2;_tmp5F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp59)->f3;_LL18: {struct _tuple1*tdn=_tmp5D;struct Cyc_List_List*ts=_tmp5E;struct Cyc_Absyn_Typedefdecl*td=_tmp5F;
# 373
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=8U,_tmp9D->f1=tdn,({struct Cyc_List_List*_tmp67B=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp9D->f2=_tmp67B;}),_tmp9D->f3=td,_tmp9D->f4=0;_tmp9D;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp59)->f1)->r)){case 0U: _LL19: _tmp5C=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp59)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp5C;
# 376
struct Cyc_List_List*_tmp9E=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmp9E;
return({union Cyc_Absyn_AggrInfo _tmp67C=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp67C,targs);});}case 1U: _LL1B: _tmp5B=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp59)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp5B;
# 379
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp5A=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp59)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp5A;
# 381
struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmp9F;
return({union Cyc_Absyn_DatatypeInfo _tmp67D=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5E8;_tmp5E8.name=dd->name,_tmp5E8.is_extensible=0;_tmp5E8;}));Cyc_Absyn_datatype_type(_tmp67D,targs);});}}}_LL0:;}
# 387
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpA0=d;struct Cyc_Absyn_Exp*_tmpA1;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpA0)->tag == 0U){_LL1: _tmpA1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpA0)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpA1;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=0U,({struct Cyc_Absyn_Exp*_tmp67E=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpA2->f1=_tmp67E;});_tmpA2;});}}else{_LL3: _LL4:
 return d;}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 393
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 395
return({struct _tuple16*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct Cyc_List_List*_tmp680=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpA3->f1=_tmp680;}),({
struct Cyc_Absyn_Exp*_tmp67F=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpA3->f2=_tmp67F;});_tmpA3;});}
# 399
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*(*_tmpA4)(int preserve_types,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_deep_copy_exp;struct Cyc_Absyn_Exp*(*DC)(int preserve_types,struct Cyc_Absyn_Exp*e)=_tmpA4;
struct Cyc_Absyn_Exp*new_e;
int _tmpA5=preserve_types;int pt=_tmpA5;
{void*_tmpA6=e->r;void*_stmttmp17=_tmpA6;void*_tmpA7=_stmttmp17;struct Cyc_Absyn_Exp*_tmpA8;struct Cyc_List_List*_tmpAD;struct Cyc_List_List*_tmpAC;struct Cyc_List_List*_tmpAB;struct _fat_ptr _tmpAA;int _tmpA9;void*_tmpAE;struct _fat_ptr*_tmpB0;struct Cyc_Absyn_Exp*_tmpAF;struct Cyc_List_List*_tmpB2;struct Cyc_Core_Opt*_tmpB1;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_Absyn_Exp*_tmpB3;int _tmpBA;int _tmpB9;struct Cyc_Absyn_Exp*_tmpB8;void**_tmpB7;struct Cyc_Absyn_Exp*_tmpB6;int _tmpB5;struct Cyc_Absyn_Enumfield*_tmpBC;void*_tmpBB;struct Cyc_Absyn_Enumfield*_tmpBE;struct Cyc_Absyn_Enumdecl*_tmpBD;struct Cyc_List_List*_tmpC2;void*_tmpC1;struct Cyc_Absyn_Tqual _tmpC0;struct _fat_ptr*_tmpBF;struct Cyc_List_List*_tmpC4;void*_tmpC3;struct Cyc_Absyn_Aggrdecl*_tmpC8;struct Cyc_List_List*_tmpC7;struct Cyc_List_List*_tmpC6;struct _tuple1*_tmpC5;int _tmpCB;void*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;int _tmpCF;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCD;struct Cyc_Absyn_Vardecl*_tmpCC;struct Cyc_Absyn_Datatypefield*_tmpD2;struct Cyc_Absyn_Datatypedecl*_tmpD1;struct Cyc_List_List*_tmpD0;struct Cyc_List_List*_tmpD3;struct Cyc_Absyn_Exp*_tmpD5;struct Cyc_Absyn_Exp*_tmpD4;int _tmpD9;int _tmpD8;struct _fat_ptr*_tmpD7;struct Cyc_Absyn_Exp*_tmpD6;int _tmpDD;int _tmpDC;struct _fat_ptr*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_List_List*_tmpDE;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_List_List*_tmpE2;void*_tmpE1;struct Cyc_Absyn_Exp*_tmpE3;void*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;enum Cyc_Absyn_Coercion _tmpEB;int _tmpEA;struct Cyc_Absyn_Exp*_tmpE9;void*_tmpE8;struct Cyc_List_List*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpEE;int _tmpF0;struct Cyc_Absyn_Exp*_tmpEF;int _tmpF4;struct Cyc_Absyn_VarargCallInfo*_tmpF3;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;int _tmpFD;int _tmpFC;void*_tmpFB;struct Cyc_Absyn_Tqual _tmpFA;struct _fat_ptr*_tmpF9;struct Cyc_List_List*_tmpF8;int _tmpF7;struct Cyc_List_List*_tmpF6;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Core_Opt*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;enum Cyc_Absyn_Incrementor _tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_List_List*_tmp10D;enum Cyc_Absyn_Primop _tmp10C;struct _fat_ptr _tmp10E;void*_tmp10F;union Cyc_Absyn_Cnst _tmp110;switch(*((int*)_tmpA7)){case 0U: _LL1: _tmp110=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp110;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp10F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL4: {void*b=_tmp10F;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp10E=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL6: {struct _fat_ptr p=_tmp10E;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp10C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmp10D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp10C;struct Cyc_List_List*es=_tmp10D;
new_e=({enum Cyc_Absyn_Primop _tmp682=p;struct Cyc_List_List*_tmp681=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_primop_exp(_tmp682,_tmp681,e->loc);});goto _LL0;}case 5U: _LL9: _tmp10A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmp10B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp10A;enum Cyc_Absyn_Incrementor i=_tmp10B;
new_e=({struct Cyc_Absyn_Exp*_tmp684=DC(pt,e1);enum Cyc_Absyn_Incrementor _tmp683=i;Cyc_Absyn_increment_exp(_tmp684,_tmp683,e->loc);});goto _LL0;}case 7U: _LLB: _tmp108=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmp109=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp108;struct Cyc_Absyn_Exp*e2=_tmp109;
new_e=({struct Cyc_Absyn_Exp*_tmp686=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp685=DC(pt,e2);Cyc_Absyn_and_exp(_tmp686,_tmp685,e->loc);});goto _LL0;}case 8U: _LLD: _tmp106=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmp107=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp106;struct Cyc_Absyn_Exp*e2=_tmp107;
new_e=({struct Cyc_Absyn_Exp*_tmp688=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp687=DC(pt,e2);Cyc_Absyn_or_exp(_tmp688,_tmp687,e->loc);});goto _LL0;}case 9U: _LLF: _tmp104=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmp105=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp104;struct Cyc_Absyn_Exp*e2=_tmp105;
new_e=({struct Cyc_Absyn_Exp*_tmp68A=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp689=DC(pt,e2);Cyc_Absyn_seq_exp(_tmp68A,_tmp689,e->loc);});goto _LL0;}case 6U: _LL11: _tmp101=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmp102=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmp103=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp101;struct Cyc_Absyn_Exp*e2=_tmp102;struct Cyc_Absyn_Exp*e3=_tmp103;
# 413
new_e=({struct Cyc_Absyn_Exp*_tmp68D=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp68C=DC(pt,e2);struct Cyc_Absyn_Exp*_tmp68B=DC(pt,e3);Cyc_Absyn_conditional_exp(_tmp68D,_tmp68C,_tmp68B,e->loc);});goto _LL0;}case 4U: _LL13: _tmpFE=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpFF=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmp100=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmpFE;struct Cyc_Core_Opt*po=_tmpFF;struct Cyc_Absyn_Exp*e2=_tmp100;
# 415
new_e=({struct Cyc_Absyn_Exp*_tmp690=DC(pt,e1);struct Cyc_Core_Opt*_tmp68F=(unsigned)po?({struct Cyc_Core_Opt*_tmp111=_cycalloc(sizeof(*_tmp111));_tmp111->v=(void*)po->v;_tmp111;}): 0;struct Cyc_Absyn_Exp*_tmp68E=DC(pt,e2);Cyc_Absyn_assignop_exp(_tmp690,_tmp68F,_tmp68E,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f3 != 0){_LL15: _tmpF5=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpF6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpF7=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f3)->num_varargs;_tmpF8=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f3)->injectors;_tmpF9=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f3)->vai)->name;_tmpFA=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f3)->vai)->tq;_tmpFB=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f3)->vai)->type;_tmpFC=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f3)->vai)->inject;_tmpFD=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpF5;struct Cyc_List_List*es=_tmpF6;int n=_tmpF7;struct Cyc_List_List*is=_tmpF8;struct _fat_ptr*nm=_tmpF9;struct Cyc_Absyn_Tqual tq=_tmpFA;void*t=_tmpFB;int i=_tmpFC;int resolved=_tmpFD;
# 418
new_e=({void*_tmp696=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp114=_cycalloc(sizeof(*_tmp114));_tmp114->tag=10U,({
struct Cyc_Absyn_Exp*_tmp695=DC(pt,e1);_tmp114->f1=_tmp695;}),({struct Cyc_List_List*_tmp694=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp114->f2=_tmp694;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp693=({struct Cyc_Absyn_VarargCallInfo*_tmp113=_cycalloc(sizeof(*_tmp113));_tmp113->num_varargs=n,_tmp113->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp692=({struct Cyc_Absyn_VarargInfo*_tmp112=_cycalloc(sizeof(*_tmp112));_tmp112->name=nm,_tmp112->tq=tq,({void*_tmp691=Cyc_Tcutil_copy_type(t);_tmp112->type=_tmp691;}),_tmp112->inject=i;_tmp112;});_tmp113->vai=_tmp692;});_tmp113;});
# 420
_tmp114->f3=_tmp693;}),_tmp114->f4=resolved;_tmp114;});
# 418
Cyc_Absyn_new_exp(_tmp696,e->loc);});
# 423
goto _LL0;}}else{_LL17: _tmpF1=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpF2=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpF3=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_tmpF4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpA7)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpF1;struct Cyc_List_List*es=_tmpF2;struct Cyc_Absyn_VarargCallInfo*vci=_tmpF3;int resolved=_tmpF4;
# 425
new_e=({void*_tmp699=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp115=_cycalloc(sizeof(*_tmp115));_tmp115->tag=10U,({struct Cyc_Absyn_Exp*_tmp698=DC(pt,e1);_tmp115->f1=_tmp698;}),({struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp115->f2=_tmp697;}),_tmp115->f3=vci,_tmp115->f4=resolved;_tmp115;});Cyc_Absyn_new_exp(_tmp699,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmpEF=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpF0=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpEF;int b=_tmpF0;
# 428
new_e=b?({struct Cyc_Absyn_Exp*_tmp69B=DC(pt,e1);Cyc_Absyn_rethrow_exp(_tmp69B,e->loc);}):({struct Cyc_Absyn_Exp*_tmp69A=DC(pt,e1);Cyc_Absyn_throw_exp(_tmp69A,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmpEE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpEE;
# 431
new_e=({struct Cyc_Absyn_Exp*_tmp69C=DC(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp69C,e->loc);});goto _LL0;}case 13U: _LL1D: _tmpEC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpED=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpEC;struct Cyc_List_List*ts=_tmpED;
# 433
new_e=({struct Cyc_Absyn_Exp*_tmp69E=DC(pt,e1);struct Cyc_List_List*_tmp69D=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp69E,_tmp69D,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpE8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpE9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpEA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_tmpEB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpA7)->f4;_LL20: {void*t=_tmpE8;struct Cyc_Absyn_Exp*e1=_tmpE9;int b=_tmpEA;enum Cyc_Absyn_Coercion c=_tmpEB;
# 435
new_e=({void*_tmp6A2=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp6A1=DC(pt,e1);int _tmp6A0=b;enum Cyc_Absyn_Coercion _tmp69F=c;Cyc_Absyn_cast_exp(_tmp6A2,_tmp6A1,_tmp6A0,_tmp69F,e->loc);});goto _LL0;}case 16U: _LL21: _tmpE6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpE7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpE6;struct Cyc_Absyn_Exp*e1=_tmpE7;
# 437
new_e=({struct Cyc_Absyn_Exp*_tmp6A4=(unsigned)eo?DC(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp6A3=DC(pt,e1);Cyc_Absyn_New_exp(_tmp6A4,_tmp6A3,e->loc);});goto _LL0;}case 15U: _LL23: _tmpE5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpE5;
new_e=({struct Cyc_Absyn_Exp*_tmp6A5=DC(pt,e1);Cyc_Absyn_address_exp(_tmp6A5,e->loc);});goto _LL0;}case 17U: _LL25: _tmpE4=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL26: {void*t=_tmpE4;
new_e=({void*_tmp6A6=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp6A6,e->loc);});goto _LL0;}case 18U: _LL27: _tmpE3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpE3;
new_e=({struct Cyc_Absyn_Exp*_tmp6A7=DC(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp6A7,e->loc);});goto _LL0;}case 19U: _LL29: _tmpE1=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpE2=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL2A: {void*t=_tmpE1;struct Cyc_List_List*ofs=_tmpE2;
new_e=({void*_tmp6A9=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp6A8=ofs;Cyc_Absyn_offsetof_exp(_tmp6A9,_tmp6A8,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpE0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpE0;
new_e=({struct Cyc_Absyn_Exp*_tmp6AA=DC(pt,e1);Cyc_Absyn_deref_exp(_tmp6AA,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpDF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpDF;
new_e=({struct Cyc_Absyn_Exp*_tmp6AB=DC(pt,e1);Cyc_Absyn_extension_exp(_tmp6AB,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpDE=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL30: {struct Cyc_List_List*es=_tmpDE;
new_e=({struct Cyc_List_List*_tmp6AC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_tuple_exp(_tmp6AC,e->loc);});goto _LL0;}case 21U: _LL31: _tmpDA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpDB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpDC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_tmpDD=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpA7)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpDA;struct _fat_ptr*n=_tmpDB;int f1=_tmpDC;int f2=_tmpDD;
# 446
new_e=({void*_tmp6AE=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->tag=21U,({struct Cyc_Absyn_Exp*_tmp6AD=DC(pt,e1);_tmp116->f1=_tmp6AD;}),_tmp116->f2=n,_tmp116->f3=f1,_tmp116->f4=f2;_tmp116;});Cyc_Absyn_new_exp(_tmp6AE,e->loc);});goto _LL0;}case 22U: _LL33: _tmpD6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpD7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpD8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_tmpD9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpA7)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpD6;struct _fat_ptr*n=_tmpD7;int f1=_tmpD8;int f2=_tmpD9;
# 448
new_e=({void*_tmp6B0=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->tag=22U,({struct Cyc_Absyn_Exp*_tmp6AF=DC(pt,e1);_tmp117->f1=_tmp6AF;}),_tmp117->f2=n,_tmp117->f3=f1,_tmp117->f4=f2;_tmp117;});Cyc_Absyn_new_exp(_tmp6B0,e->loc);});goto _LL0;}case 23U: _LL35: _tmpD4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpD5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpD4;struct Cyc_Absyn_Exp*e2=_tmpD5;
# 450
new_e=({struct Cyc_Absyn_Exp*_tmp6B2=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6B1=DC(pt,e2);Cyc_Absyn_subscript_exp(_tmp6B2,_tmp6B1,e->loc);});goto _LL0;}case 26U: _LL37: _tmpD3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL38: {struct Cyc_List_List*eds=_tmpD3;
# 452
new_e=({void*_tmp6B4=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->tag=26U,({struct Cyc_List_List*_tmp6B3=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp118->f1=_tmp6B3;});_tmp118;});Cyc_Absyn_new_exp(_tmp6B4,e->loc);});goto _LL0;}case 31U: _LL39: _tmpD0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpD1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpD2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_LL3A: {struct Cyc_List_List*es=_tmpD0;struct Cyc_Absyn_Datatypedecl*dtd=_tmpD1;struct Cyc_Absyn_Datatypefield*dtf=_tmpD2;
# 454
new_e=({void*_tmp6B6=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=31U,({struct Cyc_List_List*_tmp6B5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp119->f1=_tmp6B5;}),_tmp119->f2=dtd,_tmp119->f3=dtf;_tmp119;});Cyc_Absyn_new_exp(_tmp6B6,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpCC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpCD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpCE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_tmpCF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpA7)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpCC;struct Cyc_Absyn_Exp*e1=_tmpCD;struct Cyc_Absyn_Exp*e2=_tmpCE;int b=_tmpCF;
# 456
new_e=({void*_tmp6B9=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=27U,_tmp11A->f1=vd,({struct Cyc_Absyn_Exp*_tmp6B8=DC(pt,e1);_tmp11A->f2=_tmp6B8;}),({struct Cyc_Absyn_Exp*_tmp6B7=DC(pt,e2);_tmp11A->f3=_tmp6B7;}),_tmp11A->f4=b;_tmp11A;});Cyc_Absyn_new_exp(_tmp6B9,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpC9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpCA=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpCB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpC9;void*t=_tmpCA;int b=_tmpCB;
# 458
new_e=({void*_tmp6BC=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=28U,({struct Cyc_Absyn_Exp*_tmp6BB=DC(pt,e);_tmp11B->f1=_tmp6BB;}),({void*_tmp6BA=Cyc_Tcutil_copy_type(t);_tmp11B->f2=_tmp6BA;}),_tmp11B->f3=b;_tmp11B;});Cyc_Absyn_new_exp(_tmp6BC,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpC5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpC6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpC7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_tmpC8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpA7)->f4;_LL40: {struct _tuple1*n=_tmpC5;struct Cyc_List_List*ts=_tmpC6;struct Cyc_List_List*eds=_tmpC7;struct Cyc_Absyn_Aggrdecl*agr=_tmpC8;
# 461
new_e=({void*_tmp6BF=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=29U,_tmp11C->f1=n,({struct Cyc_List_List*_tmp6BE=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp11C->f2=_tmp6BE;}),({struct Cyc_List_List*_tmp6BD=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp11C->f3=_tmp6BD;}),_tmp11C->f4=agr;_tmp11C;});Cyc_Absyn_new_exp(_tmp6BF,e->loc);});
# 463
goto _LL0;}case 30U: _LL41: _tmpC3=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpC4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL42: {void*t=_tmpC3;struct Cyc_List_List*eds=_tmpC4;
# 465
new_e=({void*_tmp6C2=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=30U,({void*_tmp6C1=Cyc_Tcutil_copy_type(t);_tmp11D->f1=_tmp6C1;}),({struct Cyc_List_List*_tmp6C0=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp11D->f2=_tmp6C0;});_tmp11D;});Cyc_Absyn_new_exp(_tmp6C2,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpBF=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpA7)->f1)->f1;_tmpC0=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpA7)->f1)->f2;_tmpC1=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpA7)->f1)->f3;_tmpC2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL44: {struct _fat_ptr*vopt=_tmpBF;struct Cyc_Absyn_Tqual tq=_tmpC0;void*t=_tmpC1;struct Cyc_List_List*eds=_tmpC2;
# 468
new_e=({void*_tmp6C6=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=25U,({struct _tuple9*_tmp6C5=({struct _tuple9*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->f1=vopt,_tmp11E->f2=tq,({void*_tmp6C4=Cyc_Tcutil_copy_type(t);_tmp11E->f3=_tmp6C4;});_tmp11E;});_tmp11F->f1=_tmp6C5;}),({
struct Cyc_List_List*_tmp6C3=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp11F->f2=_tmp6C3;});_tmp11F;});
# 468
Cyc_Absyn_new_exp(_tmp6C6,e->loc);});
# 470
goto _LL0;}case 32U: _LL45: _tmpBD=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpBE=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpBD;struct Cyc_Absyn_Enumfield*ef=_tmpBE;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpBB=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpBC=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL48: {void*t=_tmpBB;struct Cyc_Absyn_Enumfield*ef=_tmpBC;
# 473
new_e=({void*_tmp6C8=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->tag=33U,({void*_tmp6C7=Cyc_Tcutil_copy_type(t);_tmp120->f1=_tmp6C7;}),_tmp120->f2=ef;_tmp120;});Cyc_Absyn_new_exp(_tmp6C8,e->loc);});goto _LL0;}case 34U: _LL49: _tmpB5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA7)->f1).is_calloc;_tmpB6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA7)->f1).rgn;_tmpB7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA7)->f1).elt_type;_tmpB8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA7)->f1).num_elts;_tmpB9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA7)->f1).fat_result;_tmpBA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpA7)->f1).inline_call;_LL4A: {int ic=_tmpB5;struct Cyc_Absyn_Exp*r=_tmpB6;void**t=_tmpB7;struct Cyc_Absyn_Exp*n=_tmpB8;int res=_tmpB9;int inlc=_tmpBA;
# 475
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=DC(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp121=_cycalloc(sizeof(*_tmp121));({void*_tmp6C9=Cyc_Tcutil_copy_type(*t);*_tmp121=_tmp6C9;});_tmp121;});
({void*_tmp6CA=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=34U,(_tmp122->f1).is_calloc=ic,(_tmp122->f1).rgn=r1,(_tmp122->f1).elt_type=t1,(_tmp122->f1).num_elts=n,(_tmp122->f1).fat_result=res,(_tmp122->f1).inline_call=inlc;_tmp122;});e2->r=_tmp6CA;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpB3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpB4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpB3;struct Cyc_Absyn_Exp*e2=_tmpB4;
new_e=({struct Cyc_Absyn_Exp*_tmp6CC=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6CB=DC(pt,e2);Cyc_Absyn_swap_exp(_tmp6CC,_tmp6CB,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpB1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpB2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpB1;struct Cyc_List_List*eds=_tmpB2;
# 483
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->v=(struct _tuple1*)nopt->v;_tmp123;});
new_e=({void*_tmp6CE=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=36U,_tmp124->f1=nopt1,({struct Cyc_List_List*_tmp6CD=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp124->f2=_tmp6CD;});_tmp124;});Cyc_Absyn_new_exp(_tmp6CE,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 488
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6CF=({const char*_tmp125="deep_copy: statement expressions unsupported";_tag_fat(_tmp125,sizeof(char),45U);});_tmp126->f1=_tmp6CF;});_tmp126;}));case 38U: _LL51: _tmpAF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpB0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpAF;struct _fat_ptr*fn=_tmpB0;
# 490
new_e=({void*_tmp6D1=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=38U,({struct Cyc_Absyn_Exp*_tmp6D0=DC(pt,e1);_tmp127->f1=_tmp6D0;}),_tmp127->f2=fn;_tmp127;});Cyc_Absyn_new_exp(_tmp6D1,e->loc);});goto _LL0;}case 39U: _LL53: _tmpAE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL54: {void*t=_tmpAE;
new_e=({void*_tmp6D2=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp6D2,e->loc);});goto _LL0;}case 40U: _LL55: _tmpA9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_tmpAA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA7)->f2;_tmpAB=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA7)->f3;_tmpAC=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA7)->f4;_tmpAD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpA7)->f5;_LL56: {int v=_tmpA9;struct _fat_ptr t=_tmpAA;struct Cyc_List_List*o=_tmpAB;struct Cyc_List_List*i=_tmpAC;struct Cyc_List_List*c=_tmpAD;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _LL57: _tmpA8=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpA7)->f1;_LL58: {struct Cyc_Absyn_Exp*e1=_tmpA8;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 496
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 500
return new_e;}struct _tuple17{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 505
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 507
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Kinds_ak || k == & Cyc_Kinds_tak)){
# 511
struct _RegionHandle _tmp128=_new_region("r");struct _RegionHandle*r=& _tmp128;_push_region(r);
{struct Cyc_List_List*_tmp129=0;struct Cyc_List_List*inst=_tmp129;
# 514
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp12A=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp12A;
void*_tmp12B=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp12B;
enum Cyc_Absyn_KindQual _tmp12C=(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk))->kind;enum Cyc_Absyn_KindQual _stmttmp18=_tmp12C;enum Cyc_Absyn_KindQual _tmp12D=_stmttmp18;switch(_tmp12D){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
 inst=({struct Cyc_List_List*_tmp12F=_region_malloc(r,sizeof(*_tmp12F));({struct _tuple17*_tmp6D3=({struct _tuple17*_tmp12E=_region_malloc(r,sizeof(*_tmp12E));_tmp12E->f1=tv,_tmp12E->f2=t;_tmp12E;});_tmp12F->hd=_tmp6D3;}),_tmp12F->tl=inst;_tmp12F;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 523
if(inst != 0){
field_type=({struct _RegionHandle*_tmp6D5=r;struct Cyc_List_List*_tmp6D4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp6D5,_tmp6D4,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 512
;_pop_region();}
# 528
return k;}
# 535
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 537
void*_tmp130=Cyc_Absyn_compress(t);void*_stmttmp19=_tmp130;void*_tmp131=_stmttmp19;struct Cyc_Absyn_Typedefdecl*_tmp132;struct Cyc_Absyn_Exp*_tmp133;struct Cyc_Absyn_PtrInfo _tmp134;struct Cyc_List_List*_tmp136;void*_tmp135;struct Cyc_Absyn_Tvar*_tmp137;struct Cyc_Core_Opt*_tmp138;switch(*((int*)_tmp131)){case 1U: _LL1: _tmp138=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp131)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp138;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp137=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp131)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp137;
return Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);}case 0U: _LL5: _tmp135=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f1;_tmp136=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp131)->f2;_LL6: {void*c=_tmp135;struct Cyc_List_List*ts=_tmp136;
# 541
void*_tmp139=c;int _tmp13D;struct Cyc_Absyn_AggrdeclImpl*_tmp13C;struct Cyc_List_List*_tmp13B;enum Cyc_Absyn_AggrKind _tmp13A;struct Cyc_Absyn_Kind*_tmp13E;enum Cyc_Absyn_Size_of _tmp13F;switch(*((int*)_tmp139)){case 0U: _LL1E: _LL1F:
 return& Cyc_Kinds_mk;case 1U: _LL20: _tmp13F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp139)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp13F;
# 544
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Kinds_bk:& Cyc_Kinds_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Kinds_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Kinds_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Kinds_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Kinds_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Kinds_trk;case 17U: _LL30: _tmp13E=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp139)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp13E;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Kinds_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Kinds_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Kinds_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Kinds_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Kinds_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp139)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
 return& Cyc_Kinds_mk;}else{_LL46: _LL47:
# 564
({struct Cyc_Warn_String_Warn_Warg_struct _tmp141=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E9;_tmp5E9.tag=0U,({struct _fat_ptr _tmp6D6=({const char*_tmp142="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp142,sizeof(char),40U);});_tmp5E9.f1=_tmp6D6;});_tmp5E9;});void*_tmp140[1U];_tmp140[0]=& _tmp141;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp140,sizeof(void*),1U));});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp139)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 567
 return& Cyc_Kinds_ak;}else{_LL4A: _tmp13A=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp139)->f1).KnownAggr).val)->kind;_tmp13B=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp139)->f1).KnownAggr).val)->tvs;_tmp13C=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp139)->f1).KnownAggr).val)->impl;_tmp13D=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp139)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp13A;struct Cyc_List_List*tvs=_tmp13B;struct Cyc_Absyn_AggrdeclImpl*i=_tmp13C;int expected_mem_kind=_tmp13D;
# 569
if(i == 0)
return expected_mem_kind?& Cyc_Kinds_mk:& Cyc_Kinds_ak;{
struct Cyc_List_List*_tmp143=i->fields;struct Cyc_List_List*fields=_tmp143;
if(fields == 0)return& Cyc_Kinds_mk;
# 574
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){
;}{
void*_tmp144=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp144;
struct Cyc_Absyn_Kind*_tmp145=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp145;
if(k == & Cyc_Kinds_ak || k == & Cyc_Kinds_tak)return k;}}else{
# 583
for(0;fields != 0;fields=fields->tl){
void*_tmp146=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp146;
struct Cyc_Absyn_Kind*_tmp147=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp147;
if(k == & Cyc_Kinds_ak || k == & Cyc_Kinds_tak)return k;}}
# 588
return& Cyc_Kinds_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 590
 return& Cyc_Kinds_ak;case 7U: _LL9: _LLA:
 return& Cyc_Kinds_mk;case 3U: _LLB: _tmp134=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp131)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp134;
# 593
void*_tmp148=Cyc_Absyn_compress((pinfo.ptr_atts).bounds);void*_stmttmp1A=_tmp148;void*_tmp149=_stmttmp1A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp149)->f1)){case 13U: _LL4D: _LL4E: {
# 595
enum Cyc_Absyn_AliasQual _tmp14A=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp1B=_tmp14A;enum Cyc_Absyn_AliasQual _tmp14B=_stmttmp1B;switch(_tmp14B){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Kinds_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Kinds_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Kinds_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 601
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 603
enum Cyc_Absyn_AliasQual _tmp14C=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp1C=_tmp14C;enum Cyc_Absyn_AliasQual _tmp14D=_stmttmp1C;switch(_tmp14D){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Kinds_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Kinds_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Kinds_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 609
 return& Cyc_Kinds_ik;case 11U: _LLF: _LL10:
# 613
 return& Cyc_Kinds_ak;case 4U: _LL11: _tmp133=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp131)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp133;
# 615
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Kinds_mk;
return& Cyc_Kinds_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Kinds_mk;case 8U: _LL15: _tmp132=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp131)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp132;
# 619
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EB;_tmp5EB.tag=0U,({struct _fat_ptr _tmp6D7=({const char*_tmp151="type_kind: typedef found: ";_tag_fat(_tmp151,sizeof(char),27U);});_tmp5EB.f1=_tmp6D7;});_tmp5EB;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp150=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5EA;_tmp5EA.tag=2U,_tmp5EA.f1=(void*)t;_tmp5EA;});void*_tmp14E[2U];_tmp14E[0]=& _tmp14F,_tmp14E[1]=& _tmp150;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp14E,sizeof(void*),2U));});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp131)->f1)->r)){case 1U: _LL17: _LL18:
 return& Cyc_Kinds_bk;case 0U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return& Cyc_Kinds_ak;}}_LL0:;}
# 628
static void*Cyc_Tcutil_rgns_of(void*);
# 630
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 634
static struct _tuple17*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp152=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_stmttmp1D=_tmp152;struct Cyc_Absyn_Kind*_tmp153=_stmttmp1D;switch(((struct Cyc_Absyn_Kind*)_tmp153)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp153)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp154=_cycalloc(sizeof(*_tmp154));_tmp154->tag=9U,({struct Cyc_Absyn_Exp*_tmp6D8=Cyc_Absyn_uint_exp(0U,0U);_tmp154->f1=_tmp6D8;});_tmp154;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 645
return({struct _tuple17*_tmp155=_cycalloc(sizeof(*_tmp155));_tmp155->f1=tv,_tmp155->f2=t;_tmp155;});}
# 652
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp156=Cyc_Absyn_compress(t);void*_stmttmp1E=_tmp156;void*_tmp157=_stmttmp1E;struct Cyc_List_List*_tmp158;struct Cyc_List_List*_tmp159;struct Cyc_List_List*_tmp160;struct Cyc_Absyn_VarargInfo*_tmp15F;struct Cyc_List_List*_tmp15E;void*_tmp15D;struct Cyc_Absyn_Tqual _tmp15C;void*_tmp15B;struct Cyc_List_List*_tmp15A;struct Cyc_List_List*_tmp161;void*_tmp162;void*_tmp164;void*_tmp163;struct Cyc_List_List*_tmp165;switch(*((int*)_tmp157)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp157)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp157)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp165=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp157)->f2;_LL6: {struct Cyc_List_List*ts=_tmp165;
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 659
struct Cyc_Absyn_Kind*_tmp166=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp1F=_tmp166;struct Cyc_Absyn_Kind*_tmp167=_stmttmp1F;switch(((struct Cyc_Absyn_Kind*)_tmp167)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp163=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp157)->f1).elt_type;_tmp164=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp157)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp163;void*r=_tmp164;
# 667
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp168[2U];({void*_tmp6DA=Cyc_Absyn_access_eff(r);_tmp168[0]=_tmp6DA;}),({void*_tmp6D9=Cyc_Tcutil_rgns_of(et);_tmp168[1]=_tmp6D9;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp168,sizeof(void*),2U));})));}case 4U: _LLD: _tmp162=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp157)->f1).elt_type;_LLE: {void*et=_tmp162;
# 669
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp161=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp157)->f2;_LL10: {struct Cyc_List_List*fs=_tmp161;
# 671
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp15A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp157)->f1).tvars;_tmp15B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp157)->f1).effect;_tmp15C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp157)->f1).ret_tqual;_tmp15D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp157)->f1).ret_type;_tmp15E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp157)->f1).args;_tmp15F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp157)->f1).cyc_varargs;_tmp160=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp157)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp15A;void*eff=_tmp15B;struct Cyc_Absyn_Tqual rt_tq=_tmp15C;void*rt=_tmp15D;struct Cyc_List_List*args=_tmp15E;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp15F;struct Cyc_List_List*rpo=_tmp160;
# 680
void*_tmp169=({struct Cyc_List_List*_tmp6DB=((struct Cyc_List_List*(*)(struct _tuple17*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp6DB,(void*)_check_null(eff));});void*e=_tmp169;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp159=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp157)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp159;
# 683
struct Cyc_List_List*_tmp16A=0;struct Cyc_List_List*ts=_tmp16A;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp16B->tl=ts;_tmp16B;});}
_tmp158=ts;goto _LL16;}case 8U: _LL15: _tmp158=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp157)->f2;_LL16: {struct Cyc_List_List*ts=_tmp158;
# 688
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EC;_tmp5EC.tag=0U,({struct _fat_ptr _tmp6DC=({const char*_tmp16E="typedecl in rgns_of";_tag_fat(_tmp16E,sizeof(char),20U);});_tmp5EC.f1=_tmp6DC;});_tmp5EC;});void*_tmp16C[1U];_tmp16C[0]=& _tmp16D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp16C,sizeof(void*),1U));});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 698
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp16F=e;void*_tmp170;struct Cyc_List_List**_tmp171;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)){case 9U: _LL1: _tmp171=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2;_LL2: {struct Cyc_List_List**es=_tmp171;
# 702
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp172=(void*)effs->hd;void*eff=_tmp172;
({void*_tmp6DD=(void*)Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp6DD;});{
void*_tmp173=(void*)effs->hd;void*_stmttmp20=_tmp173;void*_tmp174=_stmttmp20;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f2)->tl == 0){_LLE: _LLF:
# 711
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 715
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp175=Cyc_Absyn_compress((void*)effs->hd);void*_stmttmp21=_tmp175;void*_tmp176=_stmttmp21;void*_tmp177;struct Cyc_List_List*_tmp178;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f1)){case 9U: _LL13: _tmp178=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp178;
# 720
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp177=_tmp176;_LL1C: {void*e=_tmp177;
effects=({struct Cyc_List_List*_tmp179=_cycalloc(sizeof(*_tmp179));_tmp179->hd=e,_tmp179->tl=effects;_tmp179;});goto _LL12;}}_LL12:;}}
# 726
({struct Cyc_List_List*_tmp6DE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp6DE;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2 != 0){_LL3: _tmp170=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f2)->hd;_LL4: {void*t=_tmp170;
# 729
void*_tmp17A=Cyc_Absyn_compress(t);void*_stmttmp22=_tmp17A;void*_tmp17B=_stmttmp22;switch(*((int*)_tmp17B)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 734
 return e;}_LL0:;}}
# 739
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17C=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->tag=5U,(_tmp17D->f1).tvars=0,(_tmp17D->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6DF=Cyc_Absyn_empty_tqual(0U);(_tmp17D->f1).ret_tqual=_tmp6DF;}),(_tmp17D->f1).ret_type=Cyc_Absyn_void_type,(_tmp17D->f1).args=0,(_tmp17D->f1).c_varargs=0,(_tmp17D->f1).cyc_varargs=0,(_tmp17D->f1).rgn_po=0,(_tmp17D->f1).attributes=0,(_tmp17D->f1).requires_clause=0,(_tmp17D->f1).requires_relns=0,(_tmp17D->f1).ensures_clause=0,(_tmp17D->f1).ensures_relns=0,(_tmp17D->f1).return_value=0;_tmp17D;});
# 740
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp17C;
# 750
return({void*_tmp6E3=(void*)fntype;void*_tmp6E2=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6E1=Cyc_Absyn_empty_tqual(0U);void*_tmp6E0=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp6E3,_tmp6E2,_tmp6E1,_tmp6E0,Cyc_Absyn_false_type);});}
# 757
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Absyn_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp17E=Cyc_Absyn_compress(e);void*_stmttmp23=_tmp17E;void*_tmp17F=_stmttmp23;struct Cyc_Core_Opt*_tmp182;void**_tmp181;struct Cyc_Core_Opt*_tmp180;void*_tmp183;struct Cyc_List_List*_tmp184;void*_tmp185;switch(*((int*)_tmp17F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17F)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17F)->f2 != 0){_LL1: _tmp185=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17F)->f2)->hd;_LL2: {void*r2=_tmp185;
# 764
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Absyn_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp186=({struct _tuple15 _tmp5ED;_tmp5ED.f1=r,_tmp5ED.f2=r2;_tmp5ED;});struct _tuple15 _stmttmp24=_tmp186;struct _tuple15 _tmp187=_stmttmp24;struct Cyc_Absyn_Tvar*_tmp189;struct Cyc_Absyn_Tvar*_tmp188;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp187.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp187.f2)->tag == 2U){_LLC: _tmp188=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp187.f1)->f1;_tmp189=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp187.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp188;struct Cyc_Absyn_Tvar*tv2=_tmp189;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp184=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17F)->f2;_LL4: {struct Cyc_List_List*es=_tmp184;
# 772
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17F)->f2 != 0){_LL5: _tmp183=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17F)->f2)->hd;_LL6: {void*t=_tmp183;
# 776
void*_tmp18A=Cyc_Tcutil_rgns_of(t);void*_stmttmp25=_tmp18A;void*_tmp18B=_stmttmp25;void*_tmp18C;void*_tmp18D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f2 != 0){_LL11: _tmp18D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f2)->hd;_LL12: {void*t=_tmp18D;
# 778
if(!constrain)return 0;{
void*_tmp18E=Cyc_Absyn_compress(t);void*_stmttmp26=_tmp18E;void*_tmp18F=_stmttmp26;struct Cyc_Core_Opt*_tmp192;void**_tmp191;struct Cyc_Core_Opt*_tmp190;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18F)->tag == 1U){_LL16: _tmp190=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18F)->f1;_tmp191=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18F)->f2;_tmp192=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18F)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp190;void**p=_tmp191;struct Cyc_Core_Opt*s=_tmp192;
# 783
void*_tmp193=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp193;
# 786
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp6E5=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp194[2U];_tmp194[0]=ev,({void*_tmp6E4=Cyc_Absyn_access_eff(r);_tmp194[1]=_tmp6E4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp194,sizeof(void*),2U));})));*p=_tmp6E5;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp18C=_tmp18B;_LL14: {void*e2=_tmp18C;
# 791
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp180=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17F)->f1;_tmp181=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17F)->f2;_tmp182=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17F)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp180;void**p=_tmp181;struct Cyc_Core_Opt*s=_tmp182;
# 794
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp196=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5EE;_tmp5EE.tag=0U,({struct _fat_ptr _tmp6E6=({const char*_tmp197="effect evar has wrong kind";_tag_fat(_tmp197,sizeof(char),27U);});_tmp5EE.f1=_tmp6E6;});_tmp5EE;});void*_tmp195[1U];_tmp195[0]=& _tmp196;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp195,sizeof(void*),1U));});
if(!constrain)return 0;{
# 799
void*_tmp198=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp198;
# 802
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp6E8=Cyc_Absyn_join_eff(({void*_tmp199[2U];_tmp199[0]=ev,({void*_tmp6E7=Cyc_Absyn_access_eff(r);_tmp199[1]=_tmp6E7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp199,sizeof(void*),2U));}));*p=_tmp6E8;});
return 1;}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 812
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Absyn_compress(t);{
void*_tmp19A=Cyc_Tcutil_normalize_effect(Cyc_Absyn_compress(e));void*_stmttmp27=_tmp19A;void*_tmp19B=_stmttmp27;struct Cyc_Core_Opt*_tmp19E;void**_tmp19D;struct Cyc_Core_Opt*_tmp19C;void*_tmp19F;struct Cyc_List_List*_tmp1A0;switch(*((int*)_tmp19B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1A0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2;_LL4: {struct Cyc_List_List*es=_tmp1A0;
# 817
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2 != 0){_LL5: _tmp19F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->hd;_LL6: {void*t2=_tmp19F;
# 822
t2=Cyc_Absyn_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1A1=Cyc_Tcutil_rgns_of(t);void*_stmttmp28=_tmp1A1;void*_tmp1A2=_stmttmp28;void*_tmp1A3;void*_tmp1A4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f2 != 0){_LLC: _tmp1A4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f2)->hd;_LLD: {void*t3=_tmp1A4;
# 827
struct _tuple15 _tmp1A5=({struct _tuple15 _tmp5EF;({void*_tmp6E9=Cyc_Absyn_compress(t3);_tmp5EF.f1=_tmp6E9;}),_tmp5EF.f2=t2;_tmp5EF;});struct _tuple15 _stmttmp29=_tmp1A5;struct _tuple15 _tmp1A6=_stmttmp29;struct Cyc_Absyn_Tvar*_tmp1A8;struct Cyc_Absyn_Tvar*_tmp1A7;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6.f2)->tag == 2U){_LL11: _tmp1A7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6.f1)->f1;_tmp1A8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1A7;struct Cyc_Absyn_Tvar*tv2=_tmp1A8;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1A3=_tmp1A2;_LLF: {void*e2=_tmp1A3;
# 831
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp19C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19B)->f1;_tmp19D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19B)->f2;_tmp19E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19B)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp19C;void**p=_tmp19D;struct Cyc_Core_Opt*s=_tmp19E;
# 834
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F0;_tmp5F0.tag=0U,({struct _fat_ptr _tmp6EA=({const char*_tmp1AB="effect evar has wrong kind";_tag_fat(_tmp1AB,sizeof(char),27U);});_tmp5F0.f1=_tmp6EA;});_tmp5F0;});void*_tmp1A9[1U];_tmp1A9[0]=& _tmp1AA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1A9,sizeof(void*),1U));});
if(!may_constrain_evars)return 0;{
# 839
void*_tmp1AC=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp1AC;
# 842
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1AD=Cyc_Absyn_join_eff(({void*_tmp1AE[2U];_tmp1AE[0]=ev,({void*_tmp6EB=Cyc_Absyn_regionsof_eff(t);_tmp1AE[1]=_tmp6EB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1AE,sizeof(void*),2U));}));void*new_typ=_tmp1AD;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 852
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp1AF=e;struct Cyc_Core_Opt*_tmp1B2;void**_tmp1B1;struct Cyc_Core_Opt*_tmp1B0;void*_tmp1B3;struct Cyc_List_List*_tmp1B4;struct Cyc_Absyn_Tvar*_tmp1B5;switch(*((int*)_tmp1AF)){case 2U: _LL1: _tmp1B5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AF)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1B5;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AF)->f1)){case 9U: _LL3: _tmp1B4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AF)->f2;_LL4: {struct Cyc_List_List*es=_tmp1B4;
# 857
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AF)->f2 != 0){_LL5: _tmp1B3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AF)->f2)->hd;_LL6: {void*t=_tmp1B3;
# 862
void*_tmp1B6=Cyc_Tcutil_rgns_of(t);void*_stmttmp2A=_tmp1B6;void*_tmp1B7=_stmttmp2A;void*_tmp1B8;void*_tmp1B9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B7)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B7)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B7)->f2 != 0){_LLC: _tmp1B9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B7)->f2)->hd;_LLD: {void*t2=_tmp1B9;
# 864
if(!may_constrain_evars)return 0;{
void*_tmp1BA=Cyc_Absyn_compress(t2);void*_stmttmp2B=_tmp1BA;void*_tmp1BB=_stmttmp2B;struct Cyc_Core_Opt*_tmp1BE;void**_tmp1BD;struct Cyc_Core_Opt*_tmp1BC;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BB)->tag == 1U){_LL11: _tmp1BC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BB)->f1;_tmp1BD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BB)->f2;_tmp1BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BB)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1BC;void**p=_tmp1BD;struct Cyc_Core_Opt*s=_tmp1BE;
# 870
void*_tmp1BF=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp1BF;
# 872
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp6ED=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1C0[2U];_tmp1C0[0]=ev,({void*_tmp6EC=Cyc_Absyn_var_type(v);_tmp1C0[1]=_tmp6EC;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C0,sizeof(void*),2U));})));*p=_tmp6ED;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1B8=_tmp1B7;_LLF: {void*e2=_tmp1B8;
# 877
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1B0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AF)->f1;_tmp1B1=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AF)->f2;_tmp1B2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AF)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1B0;void**p=_tmp1B1;struct Cyc_Core_Opt*s=_tmp1B2;
# 880
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F1;_tmp5F1.tag=0U,({struct _fat_ptr _tmp6EE=({const char*_tmp1C3="effect evar has wrong kind";_tag_fat(_tmp1C3,sizeof(char),27U);});_tmp5F1.f1=_tmp6EE;});_tmp5F1;});void*_tmp1C1[1U];_tmp1C1[0]=& _tmp1C2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1C1,sizeof(void*),1U));});{
# 884
void*_tmp1C4=Cyc_Absyn_new_evar(& Cyc_Kinds_eko,s);void*ev=_tmp1C4;
# 886
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1C5=Cyc_Absyn_join_eff(({void*_tmp1C6[2U];_tmp1C6[0]=ev,({void*_tmp6EF=Cyc_Absyn_var_type(v);_tmp1C6[1]=_tmp6EF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C6,sizeof(void*),2U));}));void*new_typ=_tmp1C5;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 896
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp1C7=e;void*_tmp1C8;struct Cyc_List_List*_tmp1C9;switch(*((int*)_tmp1C7)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f1)){case 9U: _LL1: _tmp1C9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f2;_LL2: {struct Cyc_List_List*es=_tmp1C9;
# 900
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f2 != 0){_LL3: _tmp1C8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f2)->hd;_LL4: {void*t=_tmp1C8;
# 905
void*_tmp1CA=Cyc_Tcutil_rgns_of(t);void*_stmttmp2C=_tmp1CA;void*_tmp1CB=_stmttmp2C;void*_tmp1CC;void*_tmp1CD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->f2 != 0){_LLA: _tmp1CD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->f2)->hd;_LLB: {void*t2=_tmp1CD;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1CC=_tmp1CB;_LLD: {void*e2=_tmp1CC;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 909
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 923 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 926
void*_tmp1CE=Cyc_Absyn_compress(e1);void*_stmttmp2D=_tmp1CE;void*_tmp1CF=_stmttmp2D;struct Cyc_Core_Opt*_tmp1D1;void**_tmp1D0;struct Cyc_Absyn_Tvar*_tmp1D2;void*_tmp1D3;void*_tmp1D4;struct Cyc_List_List*_tmp1D5;switch(*((int*)_tmp1CF)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f1)){case 9U: _LL1: _tmp1D5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2;_LL2: {struct Cyc_List_List*es=_tmp1D5;
# 928
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2 != 0){_LL3: _tmp1D4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2)->hd;_LL4: {void*r=_tmp1D4;
# 939
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2 != 0){_LL7: _tmp1D3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2)->hd;_LL8: {void*t=_tmp1D3;
# 943
void*_tmp1D6=Cyc_Tcutil_rgns_of(t);void*_stmttmp2E=_tmp1D6;void*_tmp1D7=_stmttmp2E;void*_tmp1D8;void*_tmp1D9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D7)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D7)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D7)->f2 != 0){_LLE: _tmp1D9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D7)->f2)->hd;_LLF: {void*t2=_tmp1D9;
# 948
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1D8=_tmp1D7;_LL11: {void*e=_tmp1D8;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1D2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CF)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1D2;
# 941
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1D0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f2;_tmp1D1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f4;_LLA: {void**p=_tmp1D0;struct Cyc_Core_Opt*s=_tmp1D1;
# 953
if(Cyc_Tcutil_evar_in_effect(e1,e2))
return 0;
# 958
*p=Cyc_Absyn_empty_effect;
# 961
return 1;}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F3;_tmp5F3.tag=0U,({struct _fat_ptr _tmp6F0=({const char*_tmp1DD="subset_effect: bad effect: ";_tag_fat(_tmp1DD,sizeof(char),28U);});_tmp5F3.f1=_tmp6F0;});_tmp5F3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1DC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5F2;_tmp5F2.tag=2U,_tmp5F2.f1=(void*)e1;_tmp5F2;});void*_tmp1DA[2U];_tmp1DA[0]=& _tmp1DB,_tmp1DA[1]=& _tmp1DC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1DA,sizeof(void*),2U));});}_LL0:;}
# 972
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 974
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp1DE=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp2F=_tmp1DE;struct _tuple15*_tmp1DF=_stmttmp2F;void*_tmp1E1;void*_tmp1E0;_LL1: _tmp1E0=_tmp1DF->f1;_tmp1E1=_tmp1DF->f2;_LL2: {void*t1a=_tmp1E0;void*t1b=_tmp1E1;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp1E2=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp30=_tmp1E2;struct _tuple15*_tmp1E3=_stmttmp30;void*_tmp1E5;void*_tmp1E4;_LL4: _tmp1E4=_tmp1E3->f1;_tmp1E5=_tmp1E3->f2;_LL5: {void*t2a=_tmp1E4;void*t2b=_tmp1E5;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 984
if(!found)return 0;}}}
# 986
return 1;}
# 990
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 992
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 995
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp1E6=t;switch(*((int*)_tmp1E6)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1E6)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1E6)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1001
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1004
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1007
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1010
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1E6)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 999
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1002
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1005
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1008
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1011
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp1E6)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1000
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1003
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1006
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1009
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1012
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1034
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F4;_tmp5F4.tag=0U,({struct _fat_ptr _tmp6F1=({const char*_tmp1E9="bad con";_tag_fat(_tmp1E9,sizeof(char),8U);});_tmp5F4.f1=_tmp6F1;});_tmp5F4;});void*_tmp1E7[1U];_tmp1E7[0]=& _tmp1E8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1E7,sizeof(void*),1U));});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp1E6)->f1){case 0U: _LL21: _LL22:
# 1013
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
# 1037
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp1EA=t;void*_tmp1EB;switch(*((int*)_tmp1EA)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp1EB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f1;_LL18: {void*c=_tmp1EB;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1053
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp1EC=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp1EC;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1058
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp1ED=i;struct _tuple1*_tmp1EE;struct Cyc_Absyn_Datatypedecl*_tmp1EF;if((_tmp1ED.KnownDatatype).tag == 2){_LL1: _tmp1EF=*(_tmp1ED.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp1EF;
return dd->name;}}else{_LL3: _tmp1EE=((_tmp1ED.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp1EE;
return n;}}_LL0:;}struct _tuple18{struct _tuple1*f1;struct _tuple1*f2;};
# 1064
static struct _tuple18 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp1F0=i;struct _tuple1*_tmp1F2;struct _tuple1*_tmp1F1;struct Cyc_Absyn_Datatypefield*_tmp1F4;struct Cyc_Absyn_Datatypedecl*_tmp1F3;if((_tmp1F0.KnownDatatypefield).tag == 2){_LL1: _tmp1F3=((_tmp1F0.KnownDatatypefield).val).f1;_tmp1F4=((_tmp1F0.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp1F3;struct Cyc_Absyn_Datatypefield*df=_tmp1F4;
# 1067
return({struct _tuple18 _tmp5F5;_tmp5F5.f1=dd->name,_tmp5F5.f2=df->name;_tmp5F5;});}}else{_LL3: _tmp1F1=((_tmp1F0.UnknownDatatypefield).val).datatype_name;_tmp1F2=((_tmp1F0.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp1F1;struct _tuple1*f=_tmp1F2;
# 1069
return({struct _tuple18 _tmp5F6;_tmp5F6.f1=d,_tmp5F6.f2=f;_tmp5F6;});}}_LL0:;}struct _tuple19{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1072
static struct _tuple19 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp1F5=i;struct Cyc_Absyn_Aggrdecl*_tmp1F6;struct _tuple1*_tmp1F8;enum Cyc_Absyn_AggrKind _tmp1F7;if((_tmp1F5.UnknownAggr).tag == 1){_LL1: _tmp1F7=((_tmp1F5.UnknownAggr).val).f1;_tmp1F8=((_tmp1F5.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp1F7;struct _tuple1*n=_tmp1F8;
return({struct _tuple19 _tmp5F7;_tmp5F7.f1=k,_tmp5F7.f2=n;_tmp5F7;});}}else{_LL3: _tmp1F6=*(_tmp1F5.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp1F6;
return({struct _tuple19 _tmp5F8;_tmp5F8.f1=ad->kind,_tmp5F8.f2=ad->name;_tmp5F8;});}}_LL0:;}
# 1078
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1084
struct _tuple15 _tmp1F9=({struct _tuple15 _tmp5F9;_tmp5F9.f1=t1,_tmp5F9.f2=t2;_tmp5F9;});struct _tuple15 _stmttmp31=_tmp1F9;struct _tuple15 _tmp1FA=_stmttmp31;union Cyc_Absyn_AggrInfo _tmp1FC;union Cyc_Absyn_AggrInfo _tmp1FB;union Cyc_Absyn_DatatypeFieldInfo _tmp1FE;union Cyc_Absyn_DatatypeFieldInfo _tmp1FD;union Cyc_Absyn_DatatypeInfo _tmp200;union Cyc_Absyn_DatatypeInfo _tmp1FF;struct Cyc_List_List*_tmp202;struct Cyc_List_List*_tmp201;struct _fat_ptr _tmp204;struct _fat_ptr _tmp203;struct _tuple1*_tmp206;struct _tuple1*_tmp205;switch(*((int*)_tmp1FA.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp1FA.f2)->tag == 15U){_LL1: _tmp205=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp1FA.f1)->f1;_tmp206=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp1FA.f2)->f1;_LL2: {struct _tuple1*n1=_tmp205;struct _tuple1*n2=_tmp206;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp1FA.f2)->tag == 17U){_LL3: _tmp203=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp1FA.f1)->f1;_tmp204=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp1FA.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp203;struct _fat_ptr s2=_tmp204;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp1FA.f2)->tag == 16U){_LL5: _tmp201=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp1FA.f1)->f1;_tmp202=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp1FA.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp201;struct Cyc_List_List*fs2=_tmp202;
# 1088
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp1FA.f2)->tag == 18U){_LL7: _tmp1FF=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp1FA.f1)->f1;_tmp200=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp1FA.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp1FF;union Cyc_Absyn_DatatypeInfo info2=_tmp200;
# 1090
return({struct _tuple1*_tmp6F2=Cyc_Tcutil_get_datatype_qvar(info1);Cyc_Absyn_qvar_cmp(_tmp6F2,Cyc_Tcutil_get_datatype_qvar(info2));});}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp1FA.f2)->tag == 19U){_LL9: _tmp1FD=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp1FA.f1)->f1;_tmp1FE=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp1FA.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp1FD;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp1FE;
# 1092
struct _tuple18 _tmp207=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple18 _stmttmp32=_tmp207;struct _tuple18 _tmp208=_stmttmp32;struct _tuple1*_tmp20A;struct _tuple1*_tmp209;_LL10: _tmp209=_tmp208.f1;_tmp20A=_tmp208.f2;_LL11: {struct _tuple1*d1=_tmp209;struct _tuple1*f1=_tmp20A;
struct _tuple18 _tmp20B=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple18 _stmttmp33=_tmp20B;struct _tuple18 _tmp20C=_stmttmp33;struct _tuple1*_tmp20E;struct _tuple1*_tmp20D;_LL13: _tmp20D=_tmp20C.f1;_tmp20E=_tmp20C.f2;_LL14: {struct _tuple1*d2=_tmp20D;struct _tuple1*f2=_tmp20E;
int _tmp20F=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp20F;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp1FA.f2)->tag == 20U){_LLB: _tmp1FB=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp1FA.f1)->f1;_tmp1FC=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp1FA.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp1FB;union Cyc_Absyn_AggrInfo info2=_tmp1FC;
# 1098
struct _tuple19 _tmp210=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple19 _stmttmp34=_tmp210;struct _tuple19 _tmp211=_stmttmp34;struct _tuple1*_tmp213;enum Cyc_Absyn_AggrKind _tmp212;_LL16: _tmp212=_tmp211.f1;_tmp213=_tmp211.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp212;struct _tuple1*q1=_tmp213;
struct _tuple19 _tmp214=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple19 _stmttmp35=_tmp214;struct _tuple19 _tmp215=_stmttmp35;struct _tuple1*_tmp217;enum Cyc_Absyn_AggrKind _tmp216;_LL19: _tmp216=_tmp215.f1;_tmp217=_tmp215.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp216;struct _tuple1*q2=_tmp217;
int _tmp218=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp218;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1107
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6F4)(void*,void*)=cmp;void*_tmp6F3=(void*)_check_null(a1);_tmp6F4(_tmp6F3,(void*)_check_null(a2));});}
# 1113
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp219=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp219;
int _tmp21A=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp21A;
return Cyc_Core_intcmp(i1,i2);}
# 1118
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp21B=tqt1;void*_tmp21D;struct Cyc_Absyn_Tqual _tmp21C;_LL1: _tmp21C=_tmp21B->f1;_tmp21D=_tmp21B->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp21C;void*t1=_tmp21D;
struct _tuple12*_tmp21E=tqt2;void*_tmp220;struct Cyc_Absyn_Tqual _tmp21F;_LL4: _tmp21F=_tmp21E->f1;_tmp220=_tmp21E->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp21F;void*t2=_tmp220;
int _tmp221=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp221;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1126
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp222=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp222;
if(zsc != 0)return zsc;{
int _tmp223=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp223;
if(tqc != 0)return tqc;{
int _tmp224=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp224;
if(tc != 0)return tc;{
int _tmp225=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Atts_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp225;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1143
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1 == t2)return 0;{
int _tmp226=({int _tmp6F5=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6F5,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp226;
if(shallowcmp != 0)
return shallowcmp;{
# 1152
struct _tuple15 _tmp227=({struct _tuple15 _tmp5FE;_tmp5FE.f1=t2,_tmp5FE.f2=t1;_tmp5FE;});struct _tuple15 _stmttmp36=_tmp227;struct _tuple15 _tmp228=_stmttmp36;struct Cyc_Absyn_Exp*_tmp22A;struct Cyc_Absyn_Exp*_tmp229;struct Cyc_Absyn_Exp*_tmp22C;struct Cyc_Absyn_Exp*_tmp22B;struct Cyc_List_List*_tmp230;enum Cyc_Absyn_AggrKind _tmp22F;struct Cyc_List_List*_tmp22E;enum Cyc_Absyn_AggrKind _tmp22D;struct Cyc_List_List*_tmp232;struct Cyc_List_List*_tmp231;struct Cyc_Absyn_FnInfo _tmp234;struct Cyc_Absyn_FnInfo _tmp233;void*_tmp23C;struct Cyc_Absyn_Exp*_tmp23B;struct Cyc_Absyn_Tqual _tmp23A;void*_tmp239;void*_tmp238;struct Cyc_Absyn_Exp*_tmp237;struct Cyc_Absyn_Tqual _tmp236;void*_tmp235;void*_tmp248;void*_tmp247;void*_tmp246;void*_tmp245;struct Cyc_Absyn_Tqual _tmp244;void*_tmp243;void*_tmp242;void*_tmp241;void*_tmp240;void*_tmp23F;struct Cyc_Absyn_Tqual _tmp23E;void*_tmp23D;struct Cyc_Absyn_Tvar*_tmp24A;struct Cyc_Absyn_Tvar*_tmp249;struct Cyc_List_List*_tmp24E;void*_tmp24D;struct Cyc_List_List*_tmp24C;void*_tmp24B;switch(*((int*)_tmp228.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp228.f2)->tag == 0U){_LL1: _tmp24B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp228.f1)->f1;_tmp24C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp228.f1)->f2;_tmp24D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp228.f2)->f1;_tmp24E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp228.f2)->f2;_LL2: {void*c1=_tmp24B;struct Cyc_List_List*ts1=_tmp24C;void*c2=_tmp24D;struct Cyc_List_List*ts2=_tmp24E;
# 1154
int _tmp24F=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp24F;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp228.f2)->tag == 1U){_LL3: _LL4:
# 1158
({struct Cyc_Warn_String_Warn_Warg_struct _tmp251=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FA;_tmp5FA.tag=0U,({struct _fat_ptr _tmp6F6=({const char*_tmp252="typecmp: can only compare closed types";_tag_fat(_tmp252,sizeof(char),39U);});_tmp5FA.f1=_tmp6F6;});_tmp5FA;});void*_tmp250[1U];_tmp250[0]=& _tmp251;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp250,sizeof(void*),1U));});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp228.f2)->tag == 2U){_LL5: _tmp249=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp228.f1)->f1;_tmp24A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp228.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp249;struct Cyc_Absyn_Tvar*tv1=_tmp24A;
# 1162
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->tag == 3U){_LL7: _tmp23D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f1)->f1).elt_type;_tmp23E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f1)->f1).elt_tq;_tmp23F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f1)->f1).ptr_atts).rgn;_tmp240=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f1)->f1).ptr_atts).nullable;_tmp241=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f1)->f1).ptr_atts).bounds;_tmp242=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f1)->f1).ptr_atts).zero_term;_tmp243=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).elt_type;_tmp244=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).elt_tq;_tmp245=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).ptr_atts).rgn;_tmp246=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).ptr_atts).nullable;_tmp247=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).ptr_atts).bounds;_tmp248=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp228.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp23D;struct Cyc_Absyn_Tqual tqual2a=_tmp23E;void*rgn2=_tmp23F;void*null2a=_tmp240;void*b2=_tmp241;void*zt2=_tmp242;void*t1a=_tmp243;struct Cyc_Absyn_Tqual tqual1a=_tmp244;void*rgn1=_tmp245;void*null1a=_tmp246;void*b1=_tmp247;void*zt1=_tmp248;
# 1166
int _tmp253=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp253;
if(etc != 0)return etc;{
int _tmp254=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp254;
if(rc != 0)return rc;{
int _tmp255=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp255;
if(tqc != 0)return tqc;{
int _tmp256=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp256;
if(cc != 0)return cc;{
int _tmp257=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp257;
if(zc != 0)return zc;{
int _tmp258=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp258;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f2)->tag == 4U){_LL9: _tmp235=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f1)->f1).elt_type;_tmp236=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f1)->f1).tq;_tmp237=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f1)->f1).num_elts;_tmp238=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f1)->f1).zero_term;_tmp239=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f2)->f1).elt_type;_tmp23A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f2)->f1).tq;_tmp23B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f2)->f1).num_elts;_tmp23C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp228.f2)->f1).zero_term;_LLA: {void*t2a=_tmp235;struct Cyc_Absyn_Tqual tq2a=_tmp236;struct Cyc_Absyn_Exp*e1=_tmp237;void*zt1=_tmp238;void*t1a=_tmp239;struct Cyc_Absyn_Tqual tq1a=_tmp23A;struct Cyc_Absyn_Exp*e2=_tmp23B;void*zt2=_tmp23C;
# 1182
int _tmp259=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp259;
if(tqc != 0)return tqc;{
int _tmp25A=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp25A;
if(tc != 0)return tc;{
int _tmp25B=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp25B;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp25D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FB;_tmp5FB.tag=0U,({struct _fat_ptr _tmp6F7=({const char*_tmp25E="missing expression in array index";_tag_fat(_tmp25E,sizeof(char),34U);});_tmp5FB.f1=_tmp6F7;});_tmp5FB;});void*_tmp25C[1U];_tmp25C[0]=& _tmp25D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp25C,sizeof(void*),1U));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp228.f2)->tag == 5U){_LLB: _tmp233=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp228.f1)->f1;_tmp234=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp228.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp233;struct Cyc_Absyn_FnInfo f2=_tmp234;
# 1194
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp25F=f1.args;struct Cyc_List_List*args1=_tmp25F;
struct Cyc_List_List*_tmp260=f2.args;struct Cyc_List_List*args2=_tmp260;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp261=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp37=_tmp261;struct _tuple9 _tmp262=_stmttmp37;void*_tmp264;struct Cyc_Absyn_Tqual _tmp263;_LL18: _tmp263=_tmp262.f2;_tmp264=_tmp262.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp263;void*t1=_tmp264;
struct _tuple9 _tmp265=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp38=_tmp265;struct _tuple9 _tmp266=_stmttmp38;void*_tmp268;struct Cyc_Absyn_Tqual _tmp267;_LL1B: _tmp267=_tmp266.f2;_tmp268=_tmp266.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp267;void*t2=_tmp268;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1209
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp6F8=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp6F8,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1223
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp269=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp269;
struct Cyc_List_List*_tmp26A=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp26A;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp26B=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp39=_tmp26B;struct _tuple15 _tmp26C=_stmttmp39;void*_tmp26E;void*_tmp26D;_LL1E: _tmp26D=_tmp26C.f1;_tmp26E=_tmp26C.f2;_LL1F: {void*t1a=_tmp26D;void*t1b=_tmp26E;
struct _tuple15 _tmp26F=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp3A=_tmp26F;struct _tuple15 _tmp270=_stmttmp3A;void*_tmp272;void*_tmp271;_LL21: _tmp271=_tmp270.f1;_tmp272=_tmp270.f2;_LL22: {void*t2a=_tmp271;void*t2b=_tmp272;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1233
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1241
({struct Cyc_Warn_String_Warn_Warg_struct _tmp274=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FC;_tmp5FC.tag=0U,({struct _fat_ptr _tmp6F9=({const char*_tmp275="typecmp: function type comparison should never get here!";_tag_fat(_tmp275,sizeof(char),57U);});_tmp5FC.f1=_tmp6F9;});_tmp5FC;});void*_tmp273[1U];_tmp273[0]=& _tmp274;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp273,sizeof(void*),1U));});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp228.f2)->tag == 6U){_LLD: _tmp231=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp228.f1)->f1;_tmp232=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp228.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp231;struct Cyc_List_List*ts1=_tmp232;
# 1244
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp228.f2)->tag == 7U){_LLF: _tmp22D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp228.f1)->f1;_tmp22E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp228.f1)->f2;_tmp22F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp228.f2)->f1;_tmp230=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp228.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp22D;struct Cyc_List_List*fs2=_tmp22E;enum Cyc_Absyn_AggrKind k1=_tmp22F;struct Cyc_List_List*fs1=_tmp230;
# 1247
if((int)k1 != (int)k2)
return(int)k1 == (int)0U?- 1: 1;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp228.f2)->tag == 9U){_LL11: _tmp22B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp228.f1)->f1;_tmp22C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp228.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp22B;struct Cyc_Absyn_Exp*e2=_tmp22C;
# 1251
_tmp229=e1;_tmp22A=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp228.f2)->tag == 11U){_LL13: _tmp229=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp228.f1)->f1;_tmp22A=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp228.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp229;struct Cyc_Absyn_Exp*e2=_tmp22A;
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp277=({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FD;_tmp5FD.tag=0U,({struct _fat_ptr _tmp6FA=({const char*_tmp278="Unmatched case in typecmp";_tag_fat(_tmp278,sizeof(char),26U);});_tmp5FD.f1=_tmp6FA;});_tmp5FD;});void*_tmp276[1U];_tmp276[0]=& _tmp277;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp276,sizeof(void*),1U));});}_LL0:;}}}
# 1259
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp279=({struct _tuple15 _tmp600;({void*_tmp6FC=Cyc_Absyn_compress(t1);_tmp600.f1=_tmp6FC;}),({void*_tmp6FB=Cyc_Absyn_compress(t2);_tmp600.f2=_tmp6FB;});_tmp600;});struct _tuple15 _stmttmp3B=_tmp279;struct _tuple15 _tmp27A=_stmttmp3B;void*_tmp27C;void*_tmp27B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A.f2)->tag == 0U){_LL1: _tmp27B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A.f1)->f1;_tmp27C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27A.f2)->f1;_LL2: {void*c1=_tmp27B;void*c2=_tmp27C;
# 1262
struct _tuple15 _tmp27D=({struct _tuple15 _tmp5FF;_tmp5FF.f1=c1,_tmp5FF.f2=c2;_tmp5FF;});struct _tuple15 _stmttmp3C=_tmp27D;struct _tuple15 _tmp27E=_stmttmp3C;int _tmp280;int _tmp27F;switch(*((int*)_tmp27E.f1)){case 2U: switch(*((int*)_tmp27E.f2)){case 2U: _LL6: _tmp27F=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27E.f1)->f1;_tmp280=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27E.f2)->f1;_LL7: {int i1=_tmp27F;int i2=_tmp280;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp27E.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1270
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1275
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1278
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27E.f2)->f1 == 0){_LL14: _LL15:
# 1273
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp27E.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1271
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1276
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1279
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp27E.f2)->f1 == 0){_LL16: _LL17:
# 1274
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1280
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp27E.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1277
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1281
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1283
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1285
 return 0;}_LL0:;}
# 1289
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp281=({struct _tuple15 _tmp602;({void*_tmp6FE=Cyc_Absyn_compress(t1);_tmp602.f1=_tmp6FE;}),({void*_tmp6FD=Cyc_Absyn_compress(t2);_tmp602.f2=_tmp6FD;});_tmp602;});struct _tuple15 _stmttmp3D=_tmp281;struct _tuple15 _tmp282=_stmttmp3D;void*_tmp284;void*_tmp283;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp282.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp282.f2)->tag == 0U){_LL1: _tmp283=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp282.f1)->f1;_tmp284=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp282.f2)->f1;_LL2: {void*c1=_tmp283;void*c2=_tmp284;
# 1292
{struct _tuple15 _tmp285=({struct _tuple15 _tmp601;_tmp601.f1=c1,_tmp601.f2=c2;_tmp601;});struct _tuple15 _stmttmp3E=_tmp285;struct _tuple15 _tmp286=_stmttmp3E;int _tmp288;int _tmp287;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp286.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp286.f2)->tag == 2U){_LL6: _tmp287=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp286.f1)->f1;_tmp288=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp286.f2)->f1;_LL7: {int i1=_tmp287;int i2=_tmp288;
# 1294
if(i1 != 0 && i1 != 1)return t1;
if(i2 != 0 && i2 != 1)return t2;
if(i1 >= i2)return t1;
return t2;}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp286.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1304
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp286.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1309
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1302
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp286.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1311
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1301
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1305
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp286.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1310
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1303
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp286.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp286.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1312
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp286.f1)->tag == 4U){_LL18: _LL19:
# 1307
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp286.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1313
 goto _LL5;}}}}}}_LL5:;}
# 1315
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1318
return Cyc_Absyn_sint_type;}
# 1323
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1326
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Absyn_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp289=_cycalloc(sizeof(*_tmp289));_tmp289->v=t1;_tmp289;});}}}
# 1334
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1338
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp606;_tmp606.tag=0U,({struct _fat_ptr _tmp6FF=({const char*_tmp290="type mismatch: expecting ";_tag_fat(_tmp290,sizeof(char),26U);});_tmp606.f1=_tmp6FF;});_tmp606;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp605;_tmp605.tag=2U,_tmp605.f1=(void*)t;_tmp605;});struct Cyc_Warn_String_Warn_Warg_struct _tmp28D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp604;_tmp604.tag=0U,({struct _fat_ptr _tmp700=({const char*_tmp28F=" but found ";_tag_fat(_tmp28F,sizeof(char),12U);});_tmp604.f1=_tmp700;});_tmp604;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp28E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp603;_tmp603.tag=2U,_tmp603.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt);_tmp603;});void*_tmp28A[4U];_tmp28A[0]=& _tmp28B,_tmp28A[1]=& _tmp28C,_tmp28A[2]=& _tmp28D,_tmp28A[3]=& _tmp28E;({unsigned _tmp701=((struct Cyc_Absyn_Exp*)el->hd)->loc;Cyc_Warn_err2(_tmp701,_tag_fat(_tmp28A,sizeof(void*),4U));});});
# 1342
return 0;}}}
# 1344
return 1;}
# 1349
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp291=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp3F=_tmp291;void*_tmp292=_stmttmp3F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp292)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1355
return 1;}
# 1358
static int Cyc_Tcutil_coerce_Xint_type(void*Xint_type,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Xint_type))
return 1;
# 1362
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Xint_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp294=({struct Cyc_Warn_String_Warn_Warg_struct _tmp607;_tmp607.tag=0U,({struct _fat_ptr _tmp702=({const char*_tmp295="integral size mismatch; conversion supplied";_tag_fat(_tmp295,sizeof(char),44U);});_tmp607.f1=_tmp702;});_tmp607;});void*_tmp293[1U];_tmp293[0]=& _tmp294;({unsigned _tmp703=e->loc;Cyc_Warn_warn2(_tmp703,_tag_fat(_tmp293,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e,Xint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1368
return 0;}
# 1371
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_uint_type,e);}
# 1375
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_sint_type,e);}
# 1381
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1387
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp704=t;Cyc_Unify_unify(_tmp704,Cyc_Absyn_bounds_one());});
return Cyc_Absyn_compress(t);}
# 1392
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp296=Cyc_Absyn_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp296;
void*_tmp297=t;struct Cyc_Absyn_Exp*_tmp298;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp297)->tag == 9U){_LL1: _tmp298=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp297)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp298;
return e;}}else{_LL3: _LL4: {
# 1397
struct Cyc_Absyn_Exp*_tmp299=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp299;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1406
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp29A=Cyc_Absyn_compress(b);void*_stmttmp40=_tmp29A;void*_tmp29B=_stmttmp40;struct Cyc_List_List*_tmp29C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp29C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B)->f2;_LL4: {struct Cyc_List_List*ts=_tmp29C;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp609;_tmp609.tag=0U,({struct _fat_ptr _tmp705=({const char*_tmp2A0="get_bounds_exp: ";_tag_fat(_tmp2A0,sizeof(char),17U);});_tmp609.f1=_tmp705;});_tmp609;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp29F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp608;_tmp608.tag=2U,_tmp608.f1=(void*)b;_tmp608;});void*_tmp29D[2U];_tmp29D[0]=& _tmp29E,_tmp29D[1]=& _tmp29F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp29D,sizeof(void*),2U));});}_LL0:;}}
# 1415
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2A1=Cyc_Absyn_compress(t);void*_stmttmp41=_tmp2A1;void*_tmp2A2=_stmttmp41;void*_tmp2A3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A2)->tag == 3U){_LL1: _tmp2A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A2)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2A3;
# 1418
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60B;_tmp60B.tag=0U,({struct _fat_ptr _tmp706=({const char*_tmp2A7="get_ptr_bounds_exp not pointer: ";_tag_fat(_tmp2A7,sizeof(char),33U);});_tmp60B.f1=_tmp706;});_tmp60B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp60A;_tmp60A.tag=2U,_tmp60A.f1=(void*)t;_tmp60A;});void*_tmp2A4[2U];_tmp2A4[0]=& _tmp2A5,_tmp2A4[1]=& _tmp2A6;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2A4,sizeof(void*),2U));});}_LL0:;}
# 1424
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Kinds_boolko,({struct Cyc_Core_Opt*_tmp2A8=_cycalloc(sizeof(*_tmp2A8));_tmp2A8->v=tvs;_tmp2A8;}));}
# 1428
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Kinds_ptrbko,({struct Cyc_Core_Opt*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->v=tvs;_tmp2A9;}));}
# 1433
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);struct _tuple20{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1440
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp2AA=({struct _tuple15 _tmp612;_tmp612.f1=t1,_tmp612.f2=t2;_tmp612;});struct _tuple15 _stmttmp42=_tmp2AA;struct _tuple15 _tmp2AB=_stmttmp42;void*_tmp2B3;struct Cyc_Absyn_Exp*_tmp2B2;struct Cyc_Absyn_Tqual _tmp2B1;void*_tmp2B0;void*_tmp2AF;struct Cyc_Absyn_Exp*_tmp2AE;struct Cyc_Absyn_Tqual _tmp2AD;void*_tmp2AC;struct Cyc_Absyn_PtrInfo _tmp2B5;struct Cyc_Absyn_PtrInfo _tmp2B4;switch(*((int*)_tmp2AB.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AB.f2)->tag == 3U){_LL1: _tmp2B4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AB.f1)->f1;_tmp2B5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AB.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2B4;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2B5;
# 1446
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable)&&
 Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable))
return 0;
# 1450
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple20 _tmp2B6=({struct _tuple20 _tmp60D;({struct Cyc_Absyn_Exp*_tmp70A=({void*_tmp709=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp709,(pinfo_a.ptr_atts).bounds);});_tmp60D.f1=_tmp70A;}),({
struct Cyc_Absyn_Exp*_tmp708=({void*_tmp707=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp707,(pinfo_b.ptr_atts).bounds);});_tmp60D.f2=_tmp708;});_tmp60D;});
# 1451
struct _tuple20 _stmttmp43=_tmp2B6;struct _tuple20 _tmp2B7=_stmttmp43;struct Cyc_Absyn_Exp*_tmp2B9;struct Cyc_Absyn_Exp*_tmp2B8;if(_tmp2B7.f2 == 0){_LLA: _LLB:
# 1454
 goto _LL9;}else{if(_tmp2B7.f1 == 0){_LLC: _LLD:
# 1457
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp70B=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp70B,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
return 0;}else{_LLE: _tmp2B8=_tmp2B7.f1;_tmp2B9=_tmp2B7.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2B8;struct Cyc_Absyn_Exp*e2=_tmp2B9;
# 1465
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60C;_tmp60C.tag=0U,({struct _fat_ptr _tmp70C=({const char*_tmp2BC="implicit cast to shorter array";_tag_fat(_tmp2BC,sizeof(char),31U);});_tmp60C.f1=_tmp70C;});_tmp60C;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BB;({unsigned _tmp70D=loc;Cyc_Warn_warn2(_tmp70D,_tag_fat(_tmp2BA,sizeof(void*),1U));});});
if(!({struct Cyc_Absyn_Exp*_tmp70E=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp70E,(struct Cyc_Absyn_Exp*)_check_null(e1));}))
return 0;
goto _LL9;}}}_LL9:;}
# 1473
if((pinfo_a.elt_tq).real_const && !(pinfo_b.elt_tq).real_const)
return 0;
# 1476
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(!Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn))
# 1479
return 0;
if(Cyc_Flags_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp611;_tmp611.tag=0U,({struct _fat_ptr _tmp70F=({const char*_tmp2C3="implicit cast from region ";_tag_fat(_tmp2C3,sizeof(char),27U);});_tmp611.f1=_tmp70F;});_tmp611;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp610;_tmp610.tag=2U,_tmp610.f1=(void*)(pinfo_a.ptr_atts).rgn;_tmp610;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp60F;_tmp60F.tag=0U,({
struct _fat_ptr _tmp710=({const char*_tmp2C2=" to region ";_tag_fat(_tmp2C2,sizeof(char),12U);});_tmp60F.f1=_tmp710;});_tmp60F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp60E;_tmp60E.tag=2U,_tmp60E.f1=(void*)(pinfo_b.ptr_atts).rgn;_tmp60E;});void*_tmp2BD[4U];_tmp2BD[0]=& _tmp2BE,_tmp2BD[1]=& _tmp2BF,_tmp2BD[2]=& _tmp2C0,_tmp2BD[3]=& _tmp2C1;({unsigned _tmp711=loc;Cyc_Warn_warn2(_tmp711,_tag_fat(_tmp2BD,sizeof(void*),4U));});});}
# 1485
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)&&(
!Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)|| !(pinfo_b.elt_tq).real_const))
# 1488
return 0;
# 1492
if(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type))
return 1;
# 1496
if(!({void*_tmp712=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp712,(pinfo_b.ptr_atts).bounds);})||
 Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
return 0;
if(!(pinfo_b.elt_tq).real_const && !
Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type)))
return 0;
return Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type);}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f2)->tag == 4U){_LL3: _tmp2AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f1)->f1).elt_type;_tmp2AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f1)->f1).tq;_tmp2AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f1)->f1).num_elts;_tmp2AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f1)->f1).zero_term;_tmp2B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f2)->f1).elt_type;_tmp2B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f2)->f1).tq;_tmp2B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f2)->f1).num_elts;_tmp2B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2AB.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2AC;struct Cyc_Absyn_Tqual tq1a=_tmp2AD;struct Cyc_Absyn_Exp*e1=_tmp2AE;void*zt1=_tmp2AF;void*t2a=_tmp2B0;struct Cyc_Absyn_Tqual tq2a=_tmp2B1;struct Cyc_Absyn_Exp*e2=_tmp2B2;void*zt2=_tmp2B3;
# 1506
if(!Cyc_Unify_unify(zt1,zt2))return 0;
if(e1 == 0 || e2 == 0)return 0;
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))return 0;
if(!Cyc_Unify_unify(t1a,t2a))return 0;
# 1511
return !tq1a.real_const || tq2a.real_const;}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AB.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AB.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2AB.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1513
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1515
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1519
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2C4=Cyc_Absyn_compress(t);void*_stmttmp44=_tmp2C4;void*_tmp2C5=_stmttmp44;void*_tmp2C6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5)->tag == 3U){_LL1: _tmp2C6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5)->f1).elt_type;_LL2: {void*e=_tmp2C6;
return e;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp613;_tmp613.tag=0U,({struct _fat_ptr _tmp713=({const char*_tmp2C9="pointer_elt_type";_tag_fat(_tmp2C9,sizeof(char),17U);});_tmp613.f1=_tmp713;});_tmp613;});void*_tmp2C7[1U];_tmp2C7[0]=& _tmp2C8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C7,sizeof(void*),1U));});}_LL0:;}
# 1525
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2CA=Cyc_Absyn_compress(t);void*_stmttmp45=_tmp2CA;void*_tmp2CB=_stmttmp45;struct Cyc_Absyn_PtrAtts*_tmp2CC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB)->tag == 3U){_LL1: _tmp2CC=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2CB)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2CC;
return p->rgn;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp614;_tmp614.tag=0U,({struct _fat_ptr _tmp714=({const char*_tmp2CF="pointer_elt_type";_tag_fat(_tmp2CF,sizeof(char),17U);});_tmp614.f1=_tmp714;});_tmp614;});void*_tmp2CD[1U];_tmp2CD[0]=& _tmp2CE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2CD,sizeof(void*),1U));});}_LL0:;}
# 1532
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2D0=Cyc_Absyn_compress(t);void*_stmttmp46=_tmp2D0;void*_tmp2D1=_stmttmp46;void*_tmp2D2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->tag == 3U){_LL1: _tmp2D2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D1)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2D2;
*rgn=r;return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1542
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2D3=Cyc_Absyn_compress(t);void*_stmttmp47=_tmp2D3;void*_tmp2D4=_stmttmp47;void*_tmp2D6;void*_tmp2D5;switch(*((int*)_tmp2D4)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D4)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2D5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4)->f1).ptr_atts).nullable;_tmp2D6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D4)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2D5;void*bounds=_tmp2D6;
# 1550
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1556
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2D7=e->r;void*_stmttmp48=_tmp2D7;void*_tmp2D8=_stmttmp48;struct Cyc_Absyn_Exp*_tmp2DA;void*_tmp2D9;struct _fat_ptr _tmp2DB;switch(*((int*)_tmp2D8)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2DB=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp2DB;
# 1564
unsigned long _tmp2DC=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2DC;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}
# 1574
return 0;}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1562
 return 1;case 14U: _LLD: _tmp2D9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f1;_tmp2DA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2D8)->f2;_LLE: {void*t=_tmp2D9;struct Cyc_Absyn_Exp*e2=_tmp2DA;
# 1575
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1582
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp2DD=Cyc_Absyn_compress(t2);void*_stmttmp49=_tmp2DD;void*_tmp2DE=_stmttmp49;void*_tmp2DF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->tag == 3U){_LL1: _tmp2DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp2DF;
# 1587
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp715=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp715;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}struct _tuple21{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1600
struct _tuple21 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Kinds_rk};
# 1604
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp2F3=_cycalloc(sizeof(*_tmp2F3));_tmp2F3->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp718=({struct _fat_ptr*_tmp2F2=_cycalloc(sizeof(*_tmp2F2));({struct _fat_ptr _tmp717=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp2F1=({struct Cyc_Int_pa_PrintArg_struct _tmp616;_tmp616.tag=1U,_tmp616.f1=(unsigned long)counter ++;_tmp616;});void*_tmp2EF[1U];_tmp2EF[0]=& _tmp2F1;({struct _fat_ptr _tmp716=({const char*_tmp2F0="__aliasvar%d";_tag_fat(_tmp2F0,sizeof(char),13U);});Cyc_aprintf(_tmp716,_tag_fat(_tmp2EF,sizeof(void*),1U));});});*_tmp2F2=_tmp717;});_tmp2F2;});_tmp2F3->f2=_tmp718;});_tmp2F3;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp719=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp2EE=_cycalloc(sizeof(*_tmp2EE));_tmp2EE->tag=4U,_tmp2EE->f1=vd;_tmp2EE;});Cyc_Absyn_varb_exp(_tmp719,e->loc);});
# 1613
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp2E0=Cyc_Absyn_compress(e_type);void*_stmttmp4A=_tmp2E0;void*_tmp2E1=_stmttmp4A;struct Cyc_Absyn_PtrLoc*_tmp2E8;void*_tmp2E7;void*_tmp2E6;void*_tmp2E5;void*_tmp2E4;struct Cyc_Absyn_Tqual _tmp2E3;void*_tmp2E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->tag == 3U){_LL1: _tmp2E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).elt_type;_tmp2E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).elt_tq;_tmp2E4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts).rgn;_tmp2E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts).nullable;_tmp2E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts).bounds;_tmp2E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts).zero_term;_tmp2E8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp2E2;struct Cyc_Absyn_Tqual tq=_tmp2E3;void*old_r=_tmp2E4;void*nb=_tmp2E5;void*b=_tmp2E6;void*zt=_tmp2E7;struct Cyc_Absyn_PtrLoc*pl=_tmp2E8;
# 1616
{void*_tmp2E9=Cyc_Absyn_compress(old_r);void*_stmttmp4B=_tmp2E9;void*_tmp2EA=_stmttmp4B;struct Cyc_Core_Opt*_tmp2EC;void**_tmp2EB;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EA)->tag == 1U){_LL6: _tmp2EB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EA)->f2;_tmp2EC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2EA)->f4;_LL7: {void**topt=_tmp2EB;struct Cyc_Core_Opt*ts=_tmp2EC;
# 1618
void*_tmp2ED=Cyc_Absyn_var_type(tv);void*new_r=_tmp2ED;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1623
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1626
e->topt=0;
vd->initializer=0;{
# 1630
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1632
return({struct _tuple21 _tmp615;_tmp615.f1=d,_tmp615.f2=ve;_tmp615;});}}
# 1637
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1640
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1645
void*_tmp2F4=Cyc_Absyn_compress(wants_type);void*_stmttmp4C=_tmp2F4;void*_tmp2F5=_stmttmp4C;void*_tmp2F6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F5)->tag == 3U){_LL1: _tmp2F6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F5)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp2F6;
# 1647
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp2F7=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp2F7;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1653
return 0;}
# 1657
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1659
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1663
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1666
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1668
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61B;_tmp61B.tag=0U,({struct _fat_ptr _tmp71A=({const char*_tmp300="integral size mismatch; ";_tag_fat(_tmp300,sizeof(char),25U);});_tmp61B.f1=_tmp71A;});_tmp61B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61A;_tmp61A.tag=2U,_tmp61A.f1=(void*)t1;_tmp61A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp619;_tmp619.tag=0U,({
struct _fat_ptr _tmp71B=({const char*_tmp2FF=" -> ";_tag_fat(_tmp2FF,sizeof(char),5U);});_tmp619.f1=_tmp71B;});_tmp619;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2FC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp618;_tmp618.tag=2U,_tmp618.f1=(void*)t2;_tmp618;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2FD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp617;_tmp617.tag=0U,({struct _fat_ptr _tmp71C=({const char*_tmp2FE=" conversion supplied";_tag_fat(_tmp2FE,sizeof(char),21U);});_tmp617.f1=_tmp71C;});_tmp617;});void*_tmp2F8[5U];_tmp2F8[0]=& _tmp2F9,_tmp2F8[1]=& _tmp2FA,_tmp2F8[2]=& _tmp2FB,_tmp2F8[3]=& _tmp2FC,_tmp2F8[4]=& _tmp2FD;({unsigned _tmp71D=e->loc;Cyc_Warn_warn2(_tmp71D,_tag_fat(_tmp2F8,sizeof(void*),5U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1676
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Flags_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp302=({struct Cyc_Warn_String_Warn_Warg_struct _tmp621;_tmp621.tag=0U,({struct _fat_ptr _tmp71E=({const char*_tmp30A="implicit alias coercion for ";_tag_fat(_tmp30A,sizeof(char),29U);});_tmp621.f1=_tmp71E;});_tmp621;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp303=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp620;_tmp620.tag=4U,_tmp620.f1=e;_tmp620;});struct Cyc_Warn_String_Warn_Warg_struct _tmp304=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61F;_tmp61F.tag=0U,({struct _fat_ptr _tmp71F=({const char*_tmp309=":";_tag_fat(_tmp309,sizeof(char),2U);});_tmp61F.f1=_tmp71F;});_tmp61F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp305=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61E;_tmp61E.tag=2U,_tmp61E.f1=(void*)t1;_tmp61E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp306=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp720=({const char*_tmp308=" to ";_tag_fat(_tmp308,sizeof(char),5U);});_tmp61D.f1=_tmp720;});_tmp61D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp307=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61C;_tmp61C.tag=2U,_tmp61C.f1=(void*)t2;_tmp61C;});void*_tmp301[6U];_tmp301[0]=& _tmp302,_tmp301[1]=& _tmp303,_tmp301[2]=& _tmp304,_tmp301[3]=& _tmp305,_tmp301[4]=& _tmp306,_tmp301[5]=& _tmp307;({unsigned _tmp721=e->loc;Cyc_Warn_warn2(_tmp721,_tag_fat(_tmp301,sizeof(void*),6U));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1683
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1687
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1691
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp30C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp722=({const char*_tmp311="implicit cast from ";_tag_fat(_tmp311,sizeof(char),20U);});_tmp625.f1=_tmp722;});_tmp625;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp30D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp624;_tmp624.tag=2U,_tmp624.f1=(void*)t1;_tmp624;});struct Cyc_Warn_String_Warn_Warg_struct _tmp30E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp623;_tmp623.tag=0U,({struct _fat_ptr _tmp723=({const char*_tmp310=" to ";_tag_fat(_tmp310,sizeof(char),5U);});_tmp623.f1=_tmp723;});_tmp623;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp30F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp622;_tmp622.tag=2U,_tmp622.f1=(void*)t2;_tmp622;});void*_tmp30B[4U];_tmp30B[0]=& _tmp30C,_tmp30B[1]=& _tmp30D,_tmp30B[2]=& _tmp30E,_tmp30B[3]=& _tmp30F;({unsigned _tmp724=e->loc;Cyc_Warn_warn2(_tmp724,_tag_fat(_tmp30B,sizeof(void*),4U));});});
return 1;}
# 1697
return 0;}
# 1703
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1706
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1716
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple22{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1718
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple22*env,struct Cyc_Absyn_Aggrfield*x){
# 1721
struct _tuple22 _tmp312=*env;struct _tuple22 _stmttmp4D=_tmp312;struct _tuple22 _tmp313=_stmttmp4D;int _tmp316;struct _RegionHandle*_tmp315;struct Cyc_List_List*_tmp314;_LL1: _tmp314=_tmp313.f1;_tmp315=_tmp313.f2;_tmp316=_tmp313.f3;_LL2: {struct Cyc_List_List*inst=_tmp314;struct _RegionHandle*r=_tmp315;int flatten=_tmp316;
void*_tmp317=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp317;
struct Cyc_List_List*_tmp318=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp318;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp31A=_region_malloc(r,sizeof(*_tmp31A));({struct _tuple12*_tmp725=({struct _tuple12*_tmp319=_region_malloc(r,sizeof(*_tmp319));_tmp319->f1=x->tq,_tmp319->f2=t;_tmp319;});_tmp31A->hd=_tmp725;}),_tmp31A->tl=0;_tmp31A;});
return ts;}}struct _tuple23{struct _RegionHandle*f1;int f2;};
# 1728
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple23*env,struct _tuple12*x){
# 1730
struct _tuple23 _tmp31B=*env;struct _tuple23 _stmttmp4E=_tmp31B;struct _tuple23 _tmp31C=_stmttmp4E;int _tmp31E;struct _RegionHandle*_tmp31D;_LL1: _tmp31D=_tmp31C.f1;_tmp31E=_tmp31C.f2;_LL2: {struct _RegionHandle*r=_tmp31D;int flatten=_tmp31E;
struct _tuple12 _tmp31F=*x;struct _tuple12 _stmttmp4F=_tmp31F;struct _tuple12 _tmp320=_stmttmp4F;void*_tmp322;struct Cyc_Absyn_Tqual _tmp321;_LL4: _tmp321=_tmp320.f1;_tmp322=_tmp320.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp321;void*t=_tmp322;
struct Cyc_List_List*_tmp323=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp323;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp325=_region_malloc(r,sizeof(*_tmp325));({struct _tuple12*_tmp726=({struct _tuple12*_tmp324=_region_malloc(r,sizeof(*_tmp324));_tmp324->f1=tq,_tmp324->f2=t;_tmp324;});_tmp325->hd=_tmp726;}),_tmp325->tl=0;_tmp325;});
return ts;}}}
# 1737
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1739
if(flatten){
t1=Cyc_Absyn_compress(t1);{
void*_tmp326=t1;struct Cyc_List_List*_tmp327;struct Cyc_List_List*_tmp328;struct Cyc_List_List*_tmp32A;struct Cyc_Absyn_Aggrdecl*_tmp329;switch(*((int*)_tmp326)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp329=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f1)->f1).KnownAggr).val;_tmp32A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp326)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp329;struct Cyc_List_List*ts=_tmp32A;
# 1757
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 1759
return({struct Cyc_List_List*_tmp335=_region_malloc(r,sizeof(*_tmp335));({struct _tuple12*_tmp728=({struct _tuple12*_tmp334=_region_malloc(r,sizeof(*_tmp334));({struct Cyc_Absyn_Tqual _tmp727=Cyc_Absyn_empty_tqual(0U);_tmp334->f1=_tmp727;}),_tmp334->f2=t1;_tmp334;});_tmp335->hd=_tmp728;}),_tmp335->tl=0;_tmp335;});{
struct Cyc_List_List*_tmp336=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp336;
struct _tuple22 env=({struct _tuple22 _tmp626;_tmp626.f1=inst,_tmp626.f2=r,_tmp626.f3=flatten;_tmp626;});
struct Cyc_List_List*_tmp337=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp50=_tmp337;struct Cyc_List_List*_tmp338=_stmttmp50;struct Cyc_List_List*_tmp33A;struct Cyc_Absyn_Aggrfield*_tmp339;if(_tmp338 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp339=(struct Cyc_Absyn_Aggrfield*)_tmp338->hd;_tmp33A=_tmp338->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp339;struct Cyc_List_List*tl=_tmp33A;
# 1765
struct Cyc_List_List*_tmp33B=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp33B;
env.f3=0;{
struct Cyc_List_List*_tmp33C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_Absyn_Aggrfield*),struct _tuple22*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp33C;
struct Cyc_List_List*_tmp33D=({struct Cyc_List_List*_tmp33E=_region_malloc(r,sizeof(*_tmp33E));_tmp33E->hd=hd2,_tmp33E->tl=tl2;_tmp33E;});struct Cyc_List_List*tts=_tmp33D;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp328=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp326)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp328;
# 1744
struct _tuple23 _tmp32B=({struct _tuple23 _tmp627;_tmp627.f1=r,_tmp627.f2=flatten;_tmp627;});struct _tuple23 env=_tmp32B;
# 1746
struct Cyc_List_List*_tmp32C=tqs;struct Cyc_List_List*_tmp32E;struct _tuple12*_tmp32D;if(_tmp32C == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp32D=(struct _tuple12*)_tmp32C->hd;_tmp32E=_tmp32C->tl;_LLF: {struct _tuple12*hd=_tmp32D;struct Cyc_List_List*tl=_tmp32E;
# 1749
struct Cyc_List_List*_tmp32F=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp32F;
env.f2=0;{
struct Cyc_List_List*_tmp330=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct _tuple12*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp330;
struct Cyc_List_List*_tmp331=({struct Cyc_List_List*_tmp333=_region_malloc(r,sizeof(*_tmp333));_tmp333->hd=hd2,_tmp333->tl=tl2;_tmp333;});struct Cyc_List_List*temp=_tmp331;
return({struct _RegionHandle*_tmp729=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp729,({struct Cyc_List_List*_tmp332=_region_malloc(r,sizeof(*_tmp332));_tmp332->hd=hd2,_tmp332->tl=tl2;_tmp332;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326)->f1 == Cyc_Absyn_StructA){_LL7: _tmp327=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp326)->f2;_LL8: {struct Cyc_List_List*fs=_tmp327;
# 1772
struct _tuple22 env=({struct _tuple22 _tmp628;_tmp628.f1=0,_tmp628.f2=r,_tmp628.f3=flatten;_tmp628;});
struct Cyc_List_List*_tmp33F=fs;struct Cyc_List_List*_tmp341;struct Cyc_Absyn_Aggrfield*_tmp340;if(_tmp33F == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp340=(struct Cyc_Absyn_Aggrfield*)_tmp33F->hd;_tmp341=_tmp33F->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp340;struct Cyc_List_List*tl=_tmp341;
# 1776
struct Cyc_List_List*_tmp342=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp342;
env.f3=0;{
struct Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple22*,struct Cyc_Absyn_Aggrfield*),struct _tuple22*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp343;
struct Cyc_List_List*_tmp344=({struct Cyc_List_List*_tmp345=_region_malloc(r,sizeof(*_tmp345));_tmp345->hd=hd2,_tmp345->tl=tl2;_tmp345;});struct Cyc_List_List*tts=_tmp344;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 1782
 goto _LL0;}_LL0:;}}
# 1785
return({struct Cyc_List_List*_tmp347=_region_malloc(r,sizeof(*_tmp347));({struct _tuple12*_tmp72B=({struct _tuple12*_tmp346=_region_malloc(r,sizeof(*_tmp346));({struct Cyc_Absyn_Tqual _tmp72A=Cyc_Absyn_empty_tqual(0U);_tmp346->f1=_tmp72A;}),_tmp346->f2=t1;_tmp346;});_tmp347->hd=_tmp72B;}),_tmp347->tl=0;_tmp347;});}
# 1788
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp348=({struct _tuple15 _tmp629;({void*_tmp72D=Cyc_Absyn_compress(t1);_tmp629.f1=_tmp72D;}),({void*_tmp72C=Cyc_Absyn_compress(t2);_tmp629.f2=_tmp72C;});_tmp629;});struct _tuple15 _stmttmp51=_tmp348;struct _tuple15 _tmp349=_stmttmp51;enum Cyc_Absyn_Size_of _tmp34B;enum Cyc_Absyn_Size_of _tmp34A;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp349.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp349.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp349.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp349.f2)->f1)->tag == 1U){_LL1: _tmp34A=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp349.f1)->f1)->f2;_tmp34B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp349.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp34A;enum Cyc_Absyn_Size_of b2=_tmp34B;
# 1791
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1799
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1801
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp34C=({struct _tuple15 _tmp62A;_tmp62A.f1=t1,_tmp62A.f2=t2;_tmp62A;});struct _tuple15 _stmttmp52=_tmp34C;struct _tuple15 _tmp34D=_stmttmp52;struct Cyc_Absyn_FnInfo _tmp34F;struct Cyc_Absyn_FnInfo _tmp34E;struct Cyc_List_List*_tmp354;struct Cyc_Absyn_Datatypedecl*_tmp353;struct Cyc_List_List*_tmp352;struct Cyc_Absyn_Datatypefield*_tmp351;struct Cyc_Absyn_Datatypedecl*_tmp350;void*_tmp360;void*_tmp35F;void*_tmp35E;void*_tmp35D;struct Cyc_Absyn_Tqual _tmp35C;void*_tmp35B;void*_tmp35A;void*_tmp359;void*_tmp358;void*_tmp357;struct Cyc_Absyn_Tqual _tmp356;void*_tmp355;switch(*((int*)_tmp34D.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f2)->tag == 3U){_LL1: _tmp355=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->f1).elt_type;_tmp356=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->f1).elt_tq;_tmp357=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->f1).ptr_atts).rgn;_tmp358=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->f1).ptr_atts).nullable;_tmp359=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->f1).ptr_atts).bounds;_tmp35A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f1)->f1).ptr_atts).zero_term;_tmp35B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f2)->f1).elt_type;_tmp35C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f2)->f1).elt_tq;_tmp35D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f2)->f1).ptr_atts).rgn;_tmp35E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f2)->f1).ptr_atts).nullable;_tmp35F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f2)->f1).ptr_atts).bounds;_tmp360=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp34D.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp355;struct Cyc_Absyn_Tqual q_a=_tmp356;void*rt_a=_tmp357;void*null_a=_tmp358;void*b_a=_tmp359;void*zt_a=_tmp35A;void*t_b=_tmp35B;struct Cyc_Absyn_Tqual q_b=_tmp35C;void*rt_b=_tmp35D;void*null_b=_tmp35E;void*b_b=_tmp35F;void*zt_b=_tmp360;
# 1813
if(q_a.real_const && !q_b.real_const)
return 0;
# 1816
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 1820
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 1824
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 1828
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp361=({void*_tmp72E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp72E,b_a);});struct Cyc_Absyn_Exp*e1=_tmp361;
struct Cyc_Absyn_Exp*_tmp362=({void*_tmp72F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp72F,b_b);});struct Cyc_Absyn_Exp*e2=_tmp362;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 1837
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 1842
int _tmp363=({void*_tmp730=b_b;Cyc_Unify_unify(_tmp730,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);int deep_subtype=_tmp363;
# 1845
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp734=po;struct Cyc_List_List*_tmp733=({struct Cyc_List_List*_tmp365=_cycalloc(sizeof(*_tmp365));({struct _tuple15*_tmp731=({struct _tuple15*_tmp364=_cycalloc(sizeof(*_tmp364));_tmp364->f1=t1,_tmp364->f2=t2;_tmp364;});_tmp365->hd=_tmp731;}),_tmp365->tl=assume;_tmp365;});void*_tmp732=t_a;Cyc_Tcutil_ptrsubtype(_tmp734,_tmp733,_tmp732,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp350=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp351=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp352=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f1)->f2;_tmp353=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f2)->f1)->f1).KnownDatatype).val;_tmp354=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34D.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp350;struct Cyc_Absyn_Datatypefield*df=_tmp351;struct Cyc_List_List*ts1=_tmp352;struct Cyc_Absyn_Datatypedecl*dd2=_tmp353;struct Cyc_List_List*ts2=_tmp354;
# 1852
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 1854
if(({int _tmp735=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp735 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp34D.f2)->tag == 5U){_LL5: _tmp34E=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp34D.f1)->f1;_tmp34F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp34D.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp34E;struct Cyc_Absyn_FnInfo f2=_tmp34F;
# 1860
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp366=f1.tvars;struct Cyc_List_List*tvs1=_tmp366;
struct Cyc_List_List*_tmp367=f2.tvars;struct Cyc_List_List*tvs2=_tmp367;
if(({int _tmp736=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp736 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp369=_cycalloc(sizeof(*_tmp369));({struct _tuple17*_tmp738=({struct _tuple17*_tmp368=_cycalloc(sizeof(*_tmp368));_tmp368->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp737=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp368->f2=_tmp737;});_tmp368;});_tmp369->hd=_tmp738;}),_tmp369->tl=inst;_tmp369;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 1871
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp73B=po;struct Cyc_List_List*_tmp73A=assume;void*_tmp739=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->tag=5U,_tmp36A->f1=f1;_tmp36A;});Cyc_Tcutil_subtype(_tmp73B,_tmp73A,_tmp739,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp36B=_cycalloc(sizeof(*_tmp36B));_tmp36B->tag=5U,_tmp36B->f1=f2;_tmp36B;}));});}}}
# 1878
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp36C=f1.args;struct Cyc_List_List*args1=_tmp36C;
struct Cyc_List_List*_tmp36D=f2.args;struct Cyc_List_List*args2=_tmp36D;
# 1883
if(({int _tmp73C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp73C != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 1885
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp36E=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp53=_tmp36E;struct _tuple9 _tmp36F=_stmttmp53;void*_tmp371;struct Cyc_Absyn_Tqual _tmp370;_LLA: _tmp370=_tmp36F.f2;_tmp371=_tmp36F.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp370;void*t1=_tmp371;
struct _tuple9 _tmp372=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp54=_tmp372;struct _tuple9 _tmp373=_stmttmp54;void*_tmp375;struct Cyc_Absyn_Tqual _tmp374;_LLD: _tmp374=_tmp373.f2;_tmp375=_tmp373.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp374;void*t2=_tmp375;
# 1889
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 1893
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp376=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp376;
struct Cyc_Absyn_VarargInfo _tmp377=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp377;
# 1898
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 1903
if(!({void*_tmp73D=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp73D,(void*)_check_null(f2.effect));}))return 0;
# 1905
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 1907
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes))return 0;
# 1909
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 1911
return 0;
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 1914
return 0;
# 1916
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 1927
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1930
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp378=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp55=_tmp378;struct _tuple12*_tmp379=_stmttmp55;void*_tmp37B;struct Cyc_Absyn_Tqual _tmp37A;_LL1: _tmp37A=_tmp379->f1;_tmp37B=_tmp379->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp37A;void*t1a=_tmp37B;
struct _tuple12*_tmp37C=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp56=_tmp37C;struct _tuple12*_tmp37D=_stmttmp56;void*_tmp37F;struct Cyc_Absyn_Tqual _tmp37E;_LL4: _tmp37E=_tmp37D->f1;_tmp37F=_tmp37D->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp37E;void*t2a=_tmp37F;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Kinds_kind_leq(& Cyc_Kinds_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 1944
return 1;}
# 1949
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
{void*_tmp380=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp380)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp380)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp380)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 1957
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 1959
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 1963
void*_tmp381=t1;void*_tmp382;struct Cyc_Absyn_Enumdecl*_tmp383;void*_tmp387;struct Cyc_Absyn_Exp*_tmp386;struct Cyc_Absyn_Tqual _tmp385;void*_tmp384;void*_tmp38D;void*_tmp38C;void*_tmp38B;void*_tmp38A;struct Cyc_Absyn_Tqual _tmp389;void*_tmp388;switch(*((int*)_tmp381)){case 3U: _LLA: _tmp388=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381)->f1).elt_type;_tmp389=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381)->f1).elt_tq;_tmp38A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381)->f1).ptr_atts).rgn;_tmp38B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381)->f1).ptr_atts).nullable;_tmp38C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381)->f1).ptr_atts).bounds;_tmp38D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp388;struct Cyc_Absyn_Tqual q_a=_tmp389;void*rt_a=_tmp38A;void*null_a=_tmp38B;void*b_a=_tmp38C;void*zt_a=_tmp38D;
# 1972
{void*_tmp38E=t2;void*_tmp394;void*_tmp393;void*_tmp392;void*_tmp391;struct Cyc_Absyn_Tqual _tmp390;void*_tmp38F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E)->tag == 3U){_LL19: _tmp38F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E)->f1).elt_type;_tmp390=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E)->f1).elt_tq;_tmp391=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E)->f1).ptr_atts).rgn;_tmp392=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E)->f1).ptr_atts).nullable;_tmp393=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E)->f1).ptr_atts).bounds;_tmp394=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38E)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp38F;struct Cyc_Absyn_Tqual q_b=_tmp390;void*rt_b=_tmp391;void*null_b=_tmp392;void*b_b=_tmp393;void*zt_b=_tmp394;
# 1974
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp395=({struct Cyc_List_List*_tmp39D=_cycalloc(sizeof(*_tmp39D));({struct _tuple15*_tmp73E=({struct _tuple15*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->f1=t1,_tmp39C->f2=t2;_tmp39C;});_tmp39D->hd=_tmp73E;}),_tmp39D->tl=0;_tmp39D;});struct Cyc_List_List*assump=_tmp395;
int _tmp396=q_b.real_const || !q_a.real_const;int quals_okay=_tmp396;
# 1987 "tcutil.cyc"
int _tmp397=
({void*_tmp73F=b_b;Cyc_Unify_unify(_tmp73F,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 1987
int deep_castable=_tmp397;
# 1990
int _tmp398=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 1990
int ptrsub=_tmp398;
# 1993
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 1995
int _tmp399=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 1995
int bitcase=_tmp399;
# 1998
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp39A=({void*_tmp740=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp740,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp39A;
struct Cyc_Absyn_Exp*_tmp39B=({void*_tmp741=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp741,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp39B;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2007
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2012
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2018
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp384=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp381)->f1).elt_type;_tmp385=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp381)->f1).tq;_tmp386=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp381)->f1).num_elts;_tmp387=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp381)->f1).zero_term;_LLD: {void*t1a=_tmp384;struct Cyc_Absyn_Tqual tq1a=_tmp385;struct Cyc_Absyn_Exp*e1=_tmp386;void*zt1=_tmp387;
# 2020
{void*_tmp39E=t2;void*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Tqual _tmp3A0;void*_tmp39F;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->tag == 4U){_LL1E: _tmp39F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).elt_type;_tmp3A0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).tq;_tmp3A1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).num_elts;_tmp3A2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).zero_term;_LL1F: {void*t2a=_tmp39F;struct Cyc_Absyn_Tqual tq2a=_tmp3A0;struct Cyc_Absyn_Exp*e2=_tmp3A1;void*zt2=_tmp3A2;
# 2022
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2027
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2029
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2031
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381)->f1)){case 15U: _LLE: _tmp383=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp383;
# 2035
{void*_tmp3A3=t2;struct Cyc_Absyn_Enumdecl*_tmp3A4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A3)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A3)->f1)->tag == 15U){_LL23: _tmp3A4=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A3)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3A4;
# 2037
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp742=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp742 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2043
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2046
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381)->f2 != 0){_LL14: _tmp382=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381)->f2)->hd;_LL15: {void*r1=_tmp382;
# 2049
{void*_tmp3A5=t2;void*_tmp3A6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A5)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A5)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A5)->f2 != 0){_LL28: _tmp3A6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A5)->f2)->hd;_LL29: {void*r2=_tmp3A6;
# 2051
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2055
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2061
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3A7=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3A7;
({void*_tmp743=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->tag=14U,_tmp3A8->f1=t,_tmp3A8->f2=inner,_tmp3A8->f3=0,_tmp3A8->f4=c;_tmp3A8;});e->r=_tmp743;});
e->topt=t;}}
# 2070
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2075
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
return({struct Cyc_Absyn_Tvar*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));({struct _fat_ptr*_tmp746=({struct _fat_ptr*_tmp3AC=_cycalloc(sizeof(*_tmp3AC));({struct _fat_ptr _tmp745=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3AB=({struct Cyc_Int_pa_PrintArg_struct _tmp62B;_tmp62B.tag=1U,_tmp62B.f1=(unsigned long)Cyc_Tcutil_tvar_counter ++;_tmp62B;});void*_tmp3A9[1U];_tmp3A9[0]=& _tmp3AB;({struct _fat_ptr _tmp744=({const char*_tmp3AA="#%d";_tag_fat(_tmp3AA,sizeof(char),4U);});Cyc_aprintf(_tmp744,_tag_fat(_tmp3A9,sizeof(void*),1U));});});*_tmp3AC=_tmp745;});_tmp3AC;});_tmp3AD->name=_tmp746;}),_tmp3AD->identity=- 1,_tmp3AD->kind=k;_tmp3AD;});}
# 2079
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
return(int)*((const char*)_check_fat_subscript((t->name)[0],sizeof(char),0U))== (int)'#';}
# 2084
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2091
struct Cyc_List_List*_tmp3AE=0;struct Cyc_List_List*fn_type_atts=_tmp3AE;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Atts_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3AF=_cycalloc(sizeof(*_tmp3AF));_tmp3AF->hd=(void*)atts->hd,_tmp3AF->tl=fn_type_atts;_tmp3AF;});}}{
struct Cyc_Absyn_FnInfo _tmp3B0=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3B0;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->tag=5U,_tmp3B1->f1=type_info;_tmp3B1;});}}
# 2099
return(void*)_check_null(fd->cached_type);}
# 2105
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2107
union Cyc_Relations_RelnOp _tmp3B2=*rop;union Cyc_Relations_RelnOp _stmttmp57=_tmp3B2;union Cyc_Relations_RelnOp _tmp3B3=_stmttmp57;struct Cyc_Absyn_Vardecl*_tmp3B4;struct Cyc_Absyn_Vardecl*_tmp3B5;switch((_tmp3B3.RNumelts).tag){case 2U: _LL1: _tmp3B5=(_tmp3B3.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3B5;
# 2109
struct _tuple1 _tmp3B6=*vd->name;struct _tuple1 _stmttmp58=_tmp3B6;struct _tuple1 _tmp3B7=_stmttmp58;struct _fat_ptr*_tmp3B9;union Cyc_Absyn_Nmspace _tmp3B8;_LL8: _tmp3B8=_tmp3B7.f1;_tmp3B9=_tmp3B7.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3B8;struct _fat_ptr*var=_tmp3B9;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp747=(struct _fat_ptr)*var;Cyc_strcmp(_tmp747,({const char*_tmp3BA="return_value";_tag_fat(_tmp3BA,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp748=Cyc_Relations_RReturn();*rop=_tmp748;});
goto _LL0;}{
# 2115
unsigned c=0U;
{struct Cyc_List_List*_tmp3BB=args;struct Cyc_List_List*a=_tmp3BB;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3BC=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp59=_tmp3BC;struct _tuple9*_tmp3BD=_stmttmp59;struct _fat_ptr*_tmp3BE;_LLB: _tmp3BE=_tmp3BD->f1;_LLC: {struct _fat_ptr*vopt=_tmp3BE;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp749=Cyc_Relations_RParam(c);*rop=_tmp749;});
break;}}}}
# 2123
goto _LL0;}}}case 3U: _LL3: _tmp3B4=(_tmp3B3.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3B4;
# 2125
struct _tuple1 _tmp3BF=*vd->name;struct _tuple1 _stmttmp5A=_tmp3BF;struct _tuple1 _tmp3C0=_stmttmp5A;struct _fat_ptr*_tmp3C2;union Cyc_Absyn_Nmspace _tmp3C1;_LLE: _tmp3C1=_tmp3C0.f1;_tmp3C2=_tmp3C0.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3C1;struct _fat_ptr*var=_tmp3C2;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3C3=args;struct Cyc_List_List*a=_tmp3C3;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3C4=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5B=_tmp3C4;struct _tuple9*_tmp3C5=_stmttmp5B;struct _fat_ptr*_tmp3C6;_LL11: _tmp3C6=_tmp3C5->f1;_LL12: {struct _fat_ptr*vopt=_tmp3C6;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp74A=Cyc_Relations_RParamNumelts(c);*rop=_tmp74A;});
break;}}}}
# 2135
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2140
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2142
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2146
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2148
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3C7=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3C7;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2155
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3C8=pr;void*_tmp3CA;struct Cyc_Absyn_Tqual _tmp3C9;_LL1: _tmp3C9=_tmp3C8->f1;_tmp3CA=_tmp3C8->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3C9;void*t2=_tmp3CA;
return t2 == t?pr:({struct _tuple12*_tmp3CB=_cycalloc(sizeof(*_tmp3CB));_tmp3CB->f1=tq,_tmp3CB->f2=t;_tmp3CB;});}}struct _tuple24{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple25{struct _tuple24*f1;void*f2;};
# 2159
static struct _tuple25*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2161
return({struct _tuple25*_tmp3CD=_region_malloc(rgn,sizeof(*_tmp3CD));({struct _tuple24*_tmp74B=({struct _tuple24*_tmp3CC=_region_malloc(rgn,sizeof(*_tmp3CC));_tmp3CC->f1=(*y).f1,_tmp3CC->f2=(*y).f2;_tmp3CC;});_tmp3CD->f1=_tmp74B;}),_tmp3CD->f2=(*y).f3;_tmp3CD;});}
# 2163
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2165
struct _tuple9 _tmp3CE=*orig_arg;struct _tuple9 _stmttmp5C=_tmp3CE;struct _tuple9 _tmp3CF=_stmttmp5C;void*_tmp3D2;struct Cyc_Absyn_Tqual _tmp3D1;struct _fat_ptr*_tmp3D0;_LL1: _tmp3D0=_tmp3CF.f1;_tmp3D1=_tmp3CF.f2;_tmp3D2=_tmp3CF.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp3D0;struct Cyc_Absyn_Tqual tq_orig=_tmp3D1;void*t_orig=_tmp3D2;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->f1=vopt_orig,_tmp3D3->f2=tq_orig,_tmp3D3->f3=t;_tmp3D3;});}}
# 2169
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2173
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2175
return({struct Cyc_Absyn_Exp*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->topt=old->topt,_tmp3D4->r=r,_tmp3D4->loc=old->loc,_tmp3D4->annot=old->annot;_tmp3D4;});}
# 2180
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3D5=e->r;void*_stmttmp5D=_tmp3D5;void*_tmp3D6=_stmttmp5D;void*_tmp3D7;struct Cyc_List_List*_tmp3D9;void*_tmp3D8;struct Cyc_Absyn_Exp*_tmp3DA;struct Cyc_Absyn_Exp*_tmp3DB;void*_tmp3DC;enum Cyc_Absyn_Coercion _tmp3E0;int _tmp3DF;struct Cyc_Absyn_Exp*_tmp3DE;void*_tmp3DD;struct Cyc_Absyn_Exp*_tmp3E2;struct Cyc_Absyn_Exp*_tmp3E1;struct Cyc_Absyn_Exp*_tmp3E4;struct Cyc_Absyn_Exp*_tmp3E3;struct Cyc_Absyn_Exp*_tmp3E6;struct Cyc_Absyn_Exp*_tmp3E5;struct Cyc_Absyn_Exp*_tmp3E9;struct Cyc_Absyn_Exp*_tmp3E8;struct Cyc_Absyn_Exp*_tmp3E7;struct Cyc_List_List*_tmp3EB;enum Cyc_Absyn_Primop _tmp3EA;switch(*((int*)_tmp3D6)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp3EA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3EB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp3EA;struct Cyc_List_List*es=_tmp3EB;
# 2189
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp3EC=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp3EC;
struct Cyc_Absyn_Exp*_tmp3ED=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp3ED;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp74D=e;Cyc_Tcutil_copye(_tmp74D,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3EF=_cycalloc(sizeof(*_tmp3EF));_tmp3EF->tag=3U,_tmp3EF->f1=p,({struct Cyc_List_List*_tmp74C=({struct Cyc_Absyn_Exp*_tmp3EE[1U];_tmp3EE[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3EE,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp3EF->f2=_tmp74C;});_tmp3EF;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp3F0=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp3F0;
struct Cyc_Absyn_Exp*_tmp3F1=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp3F1;
struct Cyc_Absyn_Exp*_tmp3F2=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp3F2;
struct Cyc_Absyn_Exp*_tmp3F3=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp3F3;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp74F=e;Cyc_Tcutil_copye(_tmp74F,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->tag=3U,_tmp3F5->f1=p,({struct Cyc_List_List*_tmp74E=({struct Cyc_Absyn_Exp*_tmp3F4[2U];_tmp3F4[0]=new_e1,_tmp3F4[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3F4,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp3F5->f2=_tmp74E;});_tmp3F5;}));});}}
# 2201
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp3F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62C;_tmp62C.tag=0U,({struct _fat_ptr _tmp750=({const char*_tmp3F8="primop does not have 1 or 2 args!";_tag_fat(_tmp3F8,sizeof(char),34U);});_tmp62C.f1=_tmp750;});_tmp62C;});void*_tmp3F6[1U];_tmp3F6[0]=& _tmp3F7;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp3F6,sizeof(void*),1U));});}case 6U: _LLD: _tmp3E7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3E8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_tmp3E9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3D6)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp3E7;struct Cyc_Absyn_Exp*e2=_tmp3E8;struct Cyc_Absyn_Exp*e3=_tmp3E9;
# 2203
struct Cyc_Absyn_Exp*_tmp3F9=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp3F9;
struct Cyc_Absyn_Exp*_tmp3FA=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp3FA;
struct Cyc_Absyn_Exp*_tmp3FB=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp3FB;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp751=e;Cyc_Tcutil_copye(_tmp751,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->tag=6U,_tmp3FC->f1=new_e1,_tmp3FC->f2=new_e2,_tmp3FC->f3=new_e3;_tmp3FC;}));});}case 7U: _LLF: _tmp3E5=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3E6=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp3E5;struct Cyc_Absyn_Exp*e2=_tmp3E6;
# 2209
struct Cyc_Absyn_Exp*_tmp3FD=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp3FD;
struct Cyc_Absyn_Exp*_tmp3FE=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp3FE;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp752=e;Cyc_Tcutil_copye(_tmp752,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->tag=7U,_tmp3FF->f1=new_e1,_tmp3FF->f2=new_e2;_tmp3FF;}));});}case 8U: _LL11: _tmp3E3=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3E4=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp3E3;struct Cyc_Absyn_Exp*e2=_tmp3E4;
# 2214
struct Cyc_Absyn_Exp*_tmp400=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp400;
struct Cyc_Absyn_Exp*_tmp401=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp401;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp753=e;Cyc_Tcutil_copye(_tmp753,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->tag=8U,_tmp402->f1=new_e1,_tmp402->f2=new_e2;_tmp402;}));});}case 9U: _LL13: _tmp3E1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3E2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp3E1;struct Cyc_Absyn_Exp*e2=_tmp3E2;
# 2219
struct Cyc_Absyn_Exp*_tmp403=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp403;
struct Cyc_Absyn_Exp*_tmp404=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp404;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp754=e;Cyc_Tcutil_copye(_tmp754,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp405=_cycalloc(sizeof(*_tmp405));_tmp405->tag=9U,_tmp405->f1=new_e1,_tmp405->f2=new_e2;_tmp405;}));});}case 14U: _LL15: _tmp3DD=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3DE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_tmp3DF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D6)->f3;_tmp3E0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3D6)->f4;_LL16: {void*t=_tmp3DD;struct Cyc_Absyn_Exp*e1=_tmp3DE;int b=_tmp3DF;enum Cyc_Absyn_Coercion c=_tmp3E0;
# 2224
struct Cyc_Absyn_Exp*_tmp406=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp406;
void*_tmp407=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp407;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp755=e;Cyc_Tcutil_copye(_tmp755,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->tag=14U,_tmp408->f1=new_typ,_tmp408->f2=new_e1,_tmp408->f3=b,_tmp408->f4=c;_tmp408;}));});}case 17U: _LL17: _tmp3DC=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_LL18: {void*t=_tmp3DC;
# 2229
void*_tmp409=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp409;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp756=e;Cyc_Tcutil_copye(_tmp756,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp40A=_cycalloc(sizeof(*_tmp40A));_tmp40A->tag=17U,_tmp40A->f1=new_typ;_tmp40A;}));});}case 18U: _LL19: _tmp3DB=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp3DB;
# 2232
struct Cyc_Absyn_Exp*_tmp40B=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40B;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp757=e;Cyc_Tcutil_copye(_tmp757,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->tag=18U,_tmp40C->f1=new_e1;_tmp40C;}));});}case 41U: _LL1B: _tmp3DA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp3DA;
# 2235
struct Cyc_Absyn_Exp*_tmp40D=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40D;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp758=e;Cyc_Tcutil_copye(_tmp758,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->tag=41U,_tmp40E->f1=new_e1;_tmp40E;}));});}case 19U: _LL1D: _tmp3D8=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_tmp3D9=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3D6)->f2;_LL1E: {void*t=_tmp3D8;struct Cyc_List_List*f=_tmp3D9;
# 2238
void*_tmp40F=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp40F;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp759=e;Cyc_Tcutil_copye(_tmp759,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->tag=19U,_tmp410->f1=new_typ,_tmp410->f2=f;_tmp410;}));});}case 39U: _LL1F: _tmp3D7=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3D6)->f1;_LL20: {void*t=_tmp3D7;
# 2241
void*_tmp411=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp411;
if(new_typ == t)return e;{
# 2244
void*_tmp412=Cyc_Absyn_compress(new_typ);void*_stmttmp5E=_tmp412;void*_tmp413=_stmttmp5E;struct Cyc_Absyn_Exp*_tmp414;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp413)->tag == 9U){_LL24: _tmp414=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp413)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp414;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp75A=e;Cyc_Tcutil_copye(_tmp75A,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->tag=39U,_tmp415->f1=new_typ;_tmp415;}));});}_LL23:;}}default: _LL21: _LL22:
# 2248
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp417=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62D;_tmp62D.tag=0U,({struct _fat_ptr _tmp75B=({const char*_tmp418="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp418,sizeof(char),46U);});_tmp62D.f1=_tmp75B;});_tmp62D;});void*_tmp416[1U];_tmp416[0]=& _tmp417;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp416,sizeof(void*),1U));});}_LL0:;}
# 2252
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2255
return e == 0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2258
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2261
void*_tmp419=f->type;void*t=_tmp419;
struct Cyc_Absyn_Exp*_tmp41A=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp41A;
void*_tmp41B=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp41B;
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp41C;
if(t == new_typ && req == new_req)
return f;{
struct Cyc_Absyn_Aggrfield*_tmp41D=({struct Cyc_Absyn_Aggrfield*_tmp41E=_cycalloc(sizeof(*_tmp41E));*_tmp41E=*f;_tmp41E;});struct Cyc_Absyn_Aggrfield*ans=_tmp41D;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2273
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2276
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp41F=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp41F;
struct Cyc_List_List*_tmp420=fs->tl;struct Cyc_List_List*t=_tmp420;
struct Cyc_Absyn_Aggrfield*_tmp421=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp421;
struct Cyc_List_List*_tmp422=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_typ=_tmp422;
return new_f == f && new_typ == t?fs:({struct Cyc_List_List*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->hd=new_f,_tmp423->tl=new_typ;_tmp423;});}}
# 2284
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2287
struct _tuple0 _tmp424=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp5F=_tmp424;struct _tuple0 _tmp425=_stmttmp5F;struct Cyc_List_List*_tmp427;struct Cyc_List_List*_tmp426;_LL1: _tmp426=_tmp425.f1;_tmp427=_tmp425.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp426;struct Cyc_List_List*rpo1b=_tmp427;
struct Cyc_List_List*_tmp428=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp428;
struct Cyc_List_List*_tmp429=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp429;
return rpo2a == rpo1a && rpo2b == rpo1b?rgn_po:((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}
# 2295
static int Cyc_Tcutil_skip_fntypes=0;
# 2297
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2300
void*_tmp42A=Cyc_Absyn_compress(t);void*_stmttmp60=_tmp42A;void*_tmp42B=_stmttmp60;struct Cyc_Absyn_Exp*_tmp42C;struct Cyc_Absyn_Exp*_tmp42D;struct Cyc_List_List*_tmp42F;void*_tmp42E;void*_tmp430;struct Cyc_List_List*_tmp432;enum Cyc_Absyn_AggrKind _tmp431;struct Cyc_List_List*_tmp433;struct Cyc_Absyn_Vardecl*_tmp43F;struct Cyc_Absyn_Exp*_tmp43E;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_List_List*_tmp43C;struct Cyc_List_List*_tmp43B;struct Cyc_Absyn_VarargInfo*_tmp43A;int _tmp439;struct Cyc_List_List*_tmp438;void*_tmp437;struct Cyc_Absyn_Tqual _tmp436;void*_tmp435;struct Cyc_List_List*_tmp434;void*_tmp445;void*_tmp444;void*_tmp443;void*_tmp442;struct Cyc_Absyn_Tqual _tmp441;void*_tmp440;unsigned _tmp44A;void*_tmp449;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Tqual _tmp447;void*_tmp446;void*_tmp44E;struct Cyc_Absyn_Typedefdecl*_tmp44D;struct Cyc_List_List*_tmp44C;struct _tuple1*_tmp44B;struct Cyc_Absyn_Tvar*_tmp44F;switch(*((int*)_tmp42B)){case 2U: _LL1: _tmp44F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp42B)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp44F;
# 2303
struct _handler_cons _tmp450;_push_handler(& _tmp450);{int _tmp452=0;if(setjmp(_tmp450.handler))_tmp452=1;if(!_tmp452){{void*_tmp453=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp453;};_pop_handler();}else{void*_tmp451=(void*)Cyc_Core_get_exn_thrown();void*_tmp454=_tmp451;void*_tmp455;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp454)->tag == Cyc_Core_Not_found){_LL1A: _LL1B:
 return t;}else{_LL1C: _tmp455=_tmp454;_LL1D: {void*exn=_tmp455;(int)_rethrow(exn);}}_LL19:;}}}case 8U: _LL3: _tmp44B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp42B)->f1;_tmp44C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp42B)->f2;_tmp44D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp42B)->f3;_tmp44E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp42B)->f4;_LL4: {struct _tuple1*n=_tmp44B;struct Cyc_List_List*ts=_tmp44C;struct Cyc_Absyn_Typedefdecl*td=_tmp44D;void*topt=_tmp44E;
# 2306
struct Cyc_List_List*_tmp456=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp456;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->tag=8U,_tmp457->f1=n,_tmp457->f2=new_ts,_tmp457->f3=td,_tmp457->f4=topt;_tmp457;});}case 4U: _LL5: _tmp446=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42B)->f1).elt_type;_tmp447=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42B)->f1).tq;_tmp448=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42B)->f1).num_elts;_tmp449=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42B)->f1).zero_term;_tmp44A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp42B)->f1).zt_loc;_LL6: {void*t1=_tmp446;struct Cyc_Absyn_Tqual tq=_tmp447;struct Cyc_Absyn_Exp*e=_tmp448;void*zt=_tmp449;unsigned ztl=_tmp44A;
# 2309
void*_tmp458=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp458;
struct Cyc_Absyn_Exp*_tmp459=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp459;
void*_tmp45A=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp45A;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp45B=_cycalloc(sizeof(*_tmp45B));
_tmp45B->tag=4U,(_tmp45B->f1).elt_type=new_t1,(_tmp45B->f1).tq=tq,(_tmp45B->f1).num_elts=new_e,(_tmp45B->f1).zero_term=new_zt,(_tmp45B->f1).zt_loc=ztl;_tmp45B;});}case 3U: _LL7: _tmp440=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).elt_type;_tmp441=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).elt_tq;_tmp442=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).rgn;_tmp443=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).nullable;_tmp444=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).bounds;_tmp445=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42B)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp440;struct Cyc_Absyn_Tqual tq=_tmp441;void*r=_tmp442;void*n=_tmp443;void*b=_tmp444;void*zt=_tmp445;
# 2315
void*_tmp45C=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp45C;
void*_tmp45D=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp45D;
void*_tmp45E=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp45E;
void*_tmp45F=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp45F;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp62E;_tmp62E.elt_type=new_t1,_tmp62E.elt_tq=tq,(_tmp62E.ptr_atts).rgn=new_r,(_tmp62E.ptr_atts).nullable=n,(_tmp62E.ptr_atts).bounds=new_b,(_tmp62E.ptr_atts).zero_term=new_zt,(_tmp62E.ptr_atts).ptrloc=0;_tmp62E;}));}case 5U: _LL9: _tmp434=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).tvars;_tmp435=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).effect;_tmp436=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).ret_tqual;_tmp437=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).ret_type;_tmp438=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).args;_tmp439=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).c_varargs;_tmp43A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).cyc_varargs;_tmp43B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).rgn_po;_tmp43C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).attributes;_tmp43D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).requires_clause;_tmp43E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).ensures_clause;_tmp43F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp42B)->f1).return_value;_LLA: {struct Cyc_List_List*vs=_tmp434;void*eff=_tmp435;struct Cyc_Absyn_Tqual rtq=_tmp436;void*rtyp=_tmp437;struct Cyc_List_List*args=_tmp438;int c_varargs=_tmp439;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp43A;struct Cyc_List_List*rgn_po=_tmp43B;struct Cyc_List_List*atts=_tmp43C;struct Cyc_Absyn_Exp*req=_tmp43D;struct Cyc_Absyn_Exp*ens=_tmp43E;struct Cyc_Absyn_Vardecl*ret_value=_tmp43F;
# 2324
if(Cyc_Tcutil_skip_fntypes)return t;
# 2326
{struct Cyc_List_List*_tmp460=vs;struct Cyc_List_List*p=_tmp460;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp462=_region_malloc(rgn,sizeof(*_tmp462));({struct _tuple17*_tmp75D=({struct _tuple17*_tmp461=_region_malloc(rgn,sizeof(*_tmp461));_tmp461->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp75C=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp461->f2=_tmp75C;});_tmp461;});_tmp462->hd=_tmp75D;}),_tmp462->tl=inst;_tmp462;});}}{
struct _tuple0 _tmp463=({struct _RegionHandle*_tmp75F=rgn;struct _RegionHandle*_tmp75E=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp75F,_tmp75E,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple25*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2328
struct _tuple0 _stmttmp61=_tmp463;struct _tuple0 _tmp464=_stmttmp61;struct Cyc_List_List*_tmp466;struct Cyc_List_List*_tmp465;_LL1F: _tmp465=_tmp464.f1;_tmp466=_tmp464.f2;_LL20: {struct Cyc_List_List*qs=_tmp465;struct Cyc_List_List*ts=_tmp466;
# 2330
struct Cyc_List_List*_tmp467=args;struct Cyc_List_List*args2=_tmp467;
struct Cyc_List_List*_tmp468=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp468;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff == 0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2336
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2340
struct Cyc_Absyn_VarargInfo _tmp469=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp62=_tmp469;struct Cyc_Absyn_VarargInfo _tmp46A=_stmttmp62;int _tmp46E;void*_tmp46D;struct Cyc_Absyn_Tqual _tmp46C;struct _fat_ptr*_tmp46B;_LL22: _tmp46B=_tmp46A.name;_tmp46C=_tmp46A.tq;_tmp46D=_tmp46A.type;_tmp46E=_tmp46A.inject;_LL23: {struct _fat_ptr*n=_tmp46B;struct Cyc_Absyn_Tqual tq=_tmp46C;void*t=_tmp46D;int i=_tmp46E;
void*_tmp46F=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp46F;
cyc_varargs2=t2 == t?cyc_varargs:({struct Cyc_Absyn_VarargInfo*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->name=n,_tmp470->tq=tq,_tmp470->type=t2,_tmp470->inject=i;_tmp470;});}}{
# 2344
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp471=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp471;
struct Cyc_List_List*_tmp472=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp472;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp473=_cycalloc(sizeof(*_tmp473));_tmp473->tag=5U,(_tmp473->f1).tvars=vs,(_tmp473->f1).effect=eff2,(_tmp473->f1).ret_tqual=rtq,({void*_tmp760=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp473->f1).ret_type=_tmp760;}),(_tmp473->f1).args=args2,(_tmp473->f1).c_varargs=c_varargs,(_tmp473->f1).cyc_varargs=cyc_varargs2,(_tmp473->f1).rgn_po=rgn_po2,(_tmp473->f1).attributes=atts,(_tmp473->f1).requires_clause=req2,(_tmp473->f1).requires_relns=req_relns2,(_tmp473->f1).ensures_clause=ens2,(_tmp473->f1).ensures_relns=ens_relns2,(_tmp473->f1).return_value=ret_value;_tmp473;});}}}}}case 6U: _LLB: _tmp433=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp42B)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp433;
# 2353
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp474=tqts;struct Cyc_List_List*ts1=_tmp474;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp475=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp475;
void*_tmp476=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp476;
if(t1 != t2)
change=1;
ts2=({struct Cyc_List_List*_tmp477=_region_malloc(rgn,sizeof(*_tmp477));_tmp477->hd=t2,_tmp477->tl=ts2;_tmp477;});}}
# 2362
return !change?t:(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp478=_cycalloc(sizeof(*_tmp478));_tmp478->tag=6U,({struct Cyc_List_List*_tmp762=({struct Cyc_List_List*_tmp761=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp761,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp478->f1=_tmp762;});_tmp478;});}case 7U: _LLD: _tmp431=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp42B)->f1;_tmp432=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp42B)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp431;struct Cyc_List_List*fs=_tmp432;
# 2364
struct Cyc_List_List*_tmp479=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp479;
return fs == new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp47A=_cycalloc(sizeof(*_tmp47A));_tmp47A->tag=7U,_tmp47A->f1=k,_tmp47A->f2=new_fs;_tmp47A;});}case 1U: _LLF: _tmp430=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp42B)->f2;_LL10: {void*r=_tmp430;
# 2367
return r == 0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0U: _LL11: _tmp42E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42B)->f1;_tmp42F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp42B)->f2;_LL12: {void*c=_tmp42E;struct Cyc_List_List*ts=_tmp42F;
# 2369
struct Cyc_List_List*_tmp47B=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp47B;
return ts == new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp47C=_cycalloc(sizeof(*_tmp47C));_tmp47C->tag=0U,_tmp47C->f1=c,_tmp47C->f2=new_ts;_tmp47C;});}case 9U: _LL13: _tmp42D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp42B)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp42D;
# 2372
struct Cyc_Absyn_Exp*_tmp47D=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp47D;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp47E=_cycalloc(sizeof(*_tmp47E));_tmp47E->tag=9U,_tmp47E->f1=new_e;_tmp47E;});}case 11U: _LL15: _tmp42C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp42B)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp42C;
# 2375
struct Cyc_Absyn_Exp*_tmp47F=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp47F;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->tag=11U,_tmp480->f1=new_e;_tmp480;});}default: _LL17: _LL18:
# 2378
 if(Cyc_Tcutil_skip_fntypes)return t;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp482=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62F;_tmp62F.tag=0U,({struct _fat_ptr _tmp763=({const char*_tmp483="found typedecltype in rsubs";_tag_fat(_tmp483,sizeof(char),28U);});_tmp62F.f1=_tmp763;});_tmp62F;});void*_tmp481[1U];_tmp481[0]=& _tmp482;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp481,sizeof(void*),1U));});}_LL0:;}
# 2383
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2386
if(ts == 0)
return 0;{
void*_tmp484=(void*)ts->hd;void*old_hd=_tmp484;
struct Cyc_List_List*_tmp485=ts->tl;struct Cyc_List_List*old_tl=_tmp485;
void*_tmp486=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp486;
struct Cyc_List_List*_tmp487=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp487;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488->hd=new_hd,_tmp488->tl=new_tl;_tmp488;});}}
# 2397
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return inst == 0?t: Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}
# 2401
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t){
Cyc_Tcutil_skip_fntypes=1;{
void*_tmp489=Cyc_Tcutil_substitute(inst,t);void*new_typ=_tmp489;
Cyc_Tcutil_skip_fntypes=0;
return new_typ;}}
# 2409
struct _tuple17*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp48A=Cyc_Kinds_kind_to_opt(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk));struct Cyc_Core_Opt*k=_tmp48A;
return({struct _tuple17*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->f1=tv,({void*_tmp765=({struct Cyc_Core_Opt*_tmp764=k;Cyc_Absyn_new_evar(_tmp764,({struct Cyc_Core_Opt*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->v=s;_tmp48B;}));});_tmp48C->f2=_tmp765;});_tmp48C;});}struct _tuple26{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2414
struct _tuple17*Cyc_Tcutil_r_make_inst_var(struct _tuple26*env,struct Cyc_Absyn_Tvar*tv){
# 2416
struct _tuple26*_tmp48D=env;struct _RegionHandle*_tmp48F;struct Cyc_List_List*_tmp48E;_LL1: _tmp48E=_tmp48D->f1;_tmp48F=_tmp48D->f2;_LL2: {struct Cyc_List_List*s=_tmp48E;struct _RegionHandle*rgn=_tmp48F;
struct Cyc_Core_Opt*_tmp490=Cyc_Kinds_kind_to_opt(Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk));struct Cyc_Core_Opt*k=_tmp490;
return({struct _tuple17*_tmp492=_region_malloc(rgn,sizeof(*_tmp492));_tmp492->f1=tv,({void*_tmp767=({struct Cyc_Core_Opt*_tmp766=k;Cyc_Absyn_new_evar(_tmp766,({struct Cyc_Core_Opt*_tmp491=_cycalloc(sizeof(*_tmp491));_tmp491->v=s;_tmp491;}));});_tmp492->f2=_tmp767;});_tmp492;});}}
# 2421
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2423
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp494=({struct Cyc_Warn_String_Warn_Warg_struct _tmp632;_tmp632.tag=0U,({struct _fat_ptr _tmp768=({const char*_tmp498="bitfield ";_tag_fat(_tmp498,sizeof(char),10U);});_tmp632.f1=_tmp768;});_tmp632;});struct Cyc_Warn_String_Warn_Warg_struct _tmp495=({struct Cyc_Warn_String_Warn_Warg_struct _tmp631;_tmp631.tag=0U,_tmp631.f1=*fn;_tmp631;});struct Cyc_Warn_String_Warn_Warg_struct _tmp496=({struct Cyc_Warn_String_Warn_Warg_struct _tmp630;_tmp630.tag=0U,({struct _fat_ptr _tmp769=({const char*_tmp497=" does not have constant width";_tag_fat(_tmp497,sizeof(char),30U);});_tmp630.f1=_tmp769;});_tmp630;});void*_tmp493[3U];_tmp493[0]=& _tmp494,_tmp493[1]=& _tmp495,_tmp493[2]=& _tmp496;({unsigned _tmp76A=loc;Cyc_Warn_err2(_tmp76A,_tag_fat(_tmp493,sizeof(void*),3U));});});else{
# 2428
struct _tuple13 _tmp499=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp63=_tmp499;struct _tuple13 _tmp49A=_stmttmp63;int _tmp49C;unsigned _tmp49B;_LL1: _tmp49B=_tmp49A.f1;_tmp49C=_tmp49A.f2;_LL2: {unsigned i=_tmp49B;int known=_tmp49C;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp49E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,({struct _fat_ptr _tmp76B=({const char*_tmp49F="cannot evaluate bitfield width at compile time";_tag_fat(_tmp49F,sizeof(char),47U);});_tmp633.f1=_tmp76B;});_tmp633;});void*_tmp49D[1U];_tmp49D[0]=& _tmp49E;({unsigned _tmp76C=loc;Cyc_Warn_warn2(_tmp76C,_tag_fat(_tmp49D,sizeof(void*),1U));});});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp634;_tmp634.tag=0U,({struct _fat_ptr _tmp76D=({const char*_tmp4A2="bitfield has negative width";_tag_fat(_tmp4A2,sizeof(char),28U);});_tmp634.f1=_tmp76D;});_tmp634;});void*_tmp4A0[1U];_tmp4A0[0]=& _tmp4A1;({unsigned _tmp76E=loc;Cyc_Warn_err2(_tmp76E,_tag_fat(_tmp4A0,sizeof(void*),1U));});});
w=i;}}{
# 2435
void*_tmp4A3=Cyc_Absyn_compress(field_type);void*_stmttmp64=_tmp4A3;void*_tmp4A4=_stmttmp64;enum Cyc_Absyn_Size_of _tmp4A5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A4)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A4)->f1)->tag == 1U){_LL4: _tmp4A5=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4A4)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4A5;
# 2438
int bad;
{enum Cyc_Absyn_Size_of _tmp4A6=b;switch(_tmp4A6){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 bad=w > (unsigned)8;goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 bad=w > (unsigned)16;goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 bad=w > (unsigned)32;goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 bad=w > (unsigned)64;goto _LL8;}_LL8:;}
# 2447
if(bad)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp76F=({const char*_tmp4A9="bitfield larger than type";_tag_fat(_tmp4A9,sizeof(char),26U);});_tmp635.f1=_tmp76F;});_tmp635;});void*_tmp4A7[1U];_tmp4A7[0]=& _tmp4A8;({unsigned _tmp770=loc;Cyc_Warn_warn2(_tmp770,_tag_fat(_tmp4A7,sizeof(void*),1U));});});
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2451
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp639;_tmp639.tag=0U,({struct _fat_ptr _tmp771=({const char*_tmp4B0="bitfield ";_tag_fat(_tmp4B0,sizeof(char),10U);});_tmp639.f1=_tmp771;});_tmp639;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp638;_tmp638.tag=0U,_tmp638.f1=*fn;_tmp638;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4AD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp637;_tmp637.tag=0U,({struct _fat_ptr _tmp772=({const char*_tmp4AF=" must have integral type but has type ";_tag_fat(_tmp4AF,sizeof(char),39U);});_tmp637.f1=_tmp772;});_tmp637;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4AE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp636;_tmp636.tag=2U,_tmp636.f1=(void*)field_type;_tmp636;});void*_tmp4AA[4U];_tmp4AA[0]=& _tmp4AB,_tmp4AA[1]=& _tmp4AC,_tmp4AA[2]=& _tmp4AD,_tmp4AA[3]=& _tmp4AE;({unsigned _tmp773=loc;Cyc_Warn_err2(_tmp773,_tag_fat(_tmp4AA,sizeof(void*),4U));});});
# 2453
goto _LL3;}_LL3:;}}}
# 2460
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4B1=t;void*_tmp4B3;struct Cyc_Absyn_Typedefdecl*_tmp4B2;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B1)->tag == 8U){_LL1: _tmp4B2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B1)->f3;_tmp4B3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4B1)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4B2;void*tdopt=_tmp4B3;
# 2463
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A;_tmp63A.tag=0U,({struct _fat_ptr _tmp774=({const char*_tmp4B6="extra const";_tag_fat(_tmp4B6,sizeof(char),12U);});_tmp63A.f1=_tmp774;});_tmp63A;});void*_tmp4B4[1U];_tmp4B4[0]=& _tmp4B5;({unsigned _tmp775=loc;Cyc_Warn_warn2(_tmp775,_tag_fat(_tmp4B4,sizeof(void*),1U));});});
return 1;}
# 2468
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2475
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp776=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp776;});}
# 2479
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2485
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2488
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D;_tmp63D.tag=0U,_tmp63D.f1=msg;_tmp63D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63C;_tmp63C.tag=0U,({struct _fat_ptr _tmp777=({const char*_tmp4BB=": ";_tag_fat(_tmp4BB,sizeof(char),3U);});_tmp63C.f1=_tmp777;});_tmp63C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63B;_tmp63B.tag=0U,({struct _fat_ptr _tmp778=a2string(vs->hd);_tmp63B.f1=_tmp778;});_tmp63B;});void*_tmp4B7[3U];_tmp4B7[0]=& _tmp4B8,_tmp4B7[1]=& _tmp4B9,_tmp4B7[2]=& _tmp4BA;({unsigned _tmp779=loc;Cyc_Warn_err2(_tmp779,_tag_fat(_tmp4B7,sizeof(void*),3U));});});}}}
# 2493
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2496
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2499
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp77B=tvs;unsigned _tmp77A=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp77B,_tmp77A,Cyc_Absynpp_tvar2string,({const char*_tmp4BC="duplicate type variable";_tag_fat(_tmp4BC,sizeof(char),24U);}));});}struct _tuple27{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple28{struct Cyc_List_List*f1;void*f2;};struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2514 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2519
struct _RegionHandle _tmp4BD=_new_region("temp");struct _RegionHandle*temp=& _tmp4BD;_push_region(temp);
# 2523
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp77C=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp77C,({const char*_tmp4BE="";_tag_fat(_tmp4BE,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4C0=_region_malloc(temp,sizeof(*_tmp4C0));({struct _tuple27*_tmp77D=({struct _tuple27*_tmp4BF=_region_malloc(temp,sizeof(*_tmp4BF));_tmp4BF->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4BF->f2=0;_tmp4BF;});_tmp4C0->hd=_tmp77D;}),_tmp4C0->tl=fields;_tmp4C0;});}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2529
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp4EC="struct";_tag_fat(_tmp4EC,sizeof(char),7U);}):({const char*_tmp4ED="union";_tag_fat(_tmp4ED,sizeof(char),6U);});
# 2532
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple28*_tmp4C1=(struct _tuple28*)des->hd;struct _tuple28*_stmttmp65=_tmp4C1;struct _tuple28*_tmp4C2=_stmttmp65;void*_tmp4C4;struct Cyc_List_List*_tmp4C3;_LL1: _tmp4C3=_tmp4C2->f1;_tmp4C4=_tmp4C2->f2;_LL2: {struct Cyc_List_List*dl=_tmp4C3;void*a=_tmp4C4;
if(dl == 0){
# 2537
struct Cyc_List_List*_tmp4C5=fields;struct Cyc_List_List*fields2=_tmp4C5;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple27*)fields2->hd)).f2){
(*((struct _tuple27*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp77F=({struct Cyc_List_List*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));({void*_tmp77E=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4C6=_cycalloc(sizeof(*_tmp4C6));_tmp4C6->tag=1U,_tmp4C6->f1=((*((struct _tuple27*)fields2->hd)).f1)->name;_tmp4C6;});_tmp4C7->hd=_tmp77E;}),_tmp4C7->tl=0;_tmp4C7;});(*((struct _tuple28*)des->hd)).f1=_tmp77F;});
ans=({struct Cyc_List_List*_tmp4C9=_region_malloc(rgn,sizeof(*_tmp4C9));({struct _tuple29*_tmp780=({struct _tuple29*_tmp4C8=_region_malloc(rgn,sizeof(*_tmp4C8));_tmp4C8->f1=(*((struct _tuple27*)fields2->hd)).f1,_tmp4C8->f2=a;_tmp4C8;});_tmp4C9->hd=_tmp780;}),_tmp4C9->tl=ans;_tmp4C9;});
break;}}
# 2545
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63F;_tmp63F.tag=0U,({struct _fat_ptr _tmp781=({const char*_tmp4CD="too many arguments to ";_tag_fat(_tmp4CD,sizeof(char),23U);});_tmp63F.f1=_tmp781;});_tmp63F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63E;_tmp63E.tag=0U,_tmp63E.f1=aggr_str;_tmp63E;});void*_tmp4CA[2U];_tmp4CA[0]=& _tmp4CB,_tmp4CA[1]=& _tmp4CC;({unsigned _tmp782=loc;Cyc_Warn_err2(_tmp782,_tag_fat(_tmp4CA,sizeof(void*),2U));});});}else{
if(dl->tl != 0)
# 2549
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp640;_tmp640.tag=0U,({struct _fat_ptr _tmp783=({const char*_tmp4D0="multiple designators are not yet supported";_tag_fat(_tmp4D0,sizeof(char),43U);});_tmp640.f1=_tmp783;});_tmp640;});void*_tmp4CE[1U];_tmp4CE[0]=& _tmp4CF;({unsigned _tmp784=loc;Cyc_Warn_err2(_tmp784,_tag_fat(_tmp4CE,sizeof(void*),1U));});});else{
# 2552
void*_tmp4D1=(void*)dl->hd;void*_stmttmp66=_tmp4D1;void*_tmp4D2=_stmttmp66;struct _fat_ptr*_tmp4D3;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4D2)->tag == 0U){_LL4: _LL5:
# 2554
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp642;_tmp642.tag=0U,({struct _fat_ptr _tmp785=({const char*_tmp4D7="array designator used in argument to ";_tag_fat(_tmp4D7,sizeof(char),38U);});_tmp642.f1=_tmp785;});_tmp642;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp641;_tmp641.tag=0U,_tmp641.f1=aggr_str;_tmp641;});void*_tmp4D4[2U];_tmp4D4[0]=& _tmp4D5,_tmp4D4[1]=& _tmp4D6;({unsigned _tmp786=loc;Cyc_Warn_err2(_tmp786,_tag_fat(_tmp4D4,sizeof(void*),2U));});});
goto _LL3;}else{_LL6: _tmp4D3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4D2)->f1;_LL7: {struct _fat_ptr*v=_tmp4D3;
# 2557
struct Cyc_List_List*_tmp4D8=fields;struct Cyc_List_List*fields2=_tmp4D8;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple27*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple27*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp645;_tmp645.tag=0U,({struct _fat_ptr _tmp787=({const char*_tmp4DE="member ";_tag_fat(_tmp4DE,sizeof(char),8U);});_tmp645.f1=_tmp787;});_tmp645;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0U,_tmp644.f1=*v;_tmp644;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp643;_tmp643.tag=0U,({struct _fat_ptr _tmp788=({const char*_tmp4DD=" has already been used as an argument";_tag_fat(_tmp4DD,sizeof(char),38U);});_tmp643.f1=_tmp788;});_tmp643;});void*_tmp4D9[3U];_tmp4D9[0]=& _tmp4DA,_tmp4D9[1]=& _tmp4DB,_tmp4D9[2]=& _tmp4DC;({unsigned _tmp789=loc;Cyc_Warn_err2(_tmp789,_tag_fat(_tmp4D9,sizeof(void*),3U));});});
(*((struct _tuple27*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4E0=_region_malloc(rgn,sizeof(*_tmp4E0));({struct _tuple29*_tmp78A=({struct _tuple29*_tmp4DF=_region_malloc(rgn,sizeof(*_tmp4DF));_tmp4DF->f1=(*((struct _tuple27*)fields2->hd)).f1,_tmp4DF->f2=a;_tmp4DF;});_tmp4E0->hd=_tmp78A;}),_tmp4E0->tl=ans;_tmp4E0;});
break;}}
# 2566
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp647;_tmp647.tag=0U,({struct _fat_ptr _tmp78B=({const char*_tmp4E4="bad field designator ";_tag_fat(_tmp4E4,sizeof(char),22U);});_tmp647.f1=_tmp78B;});_tmp647;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp646;_tmp646.tag=0U,_tmp646.f1=*v;_tmp646;});void*_tmp4E1[2U];_tmp4E1[0]=& _tmp4E2,_tmp4E1[1]=& _tmp4E3;({unsigned _tmp78C=loc;Cyc_Warn_err2(_tmp78C,_tag_fat(_tmp4E1,sizeof(void*),2U));});});
goto _LL3;}}_LL3:;}}}}
# 2573
if((int)aggr_kind == (int)0U){
if(({int _tmp78D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans);_tmp78D < ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp648;_tmp648.tag=0U,({struct _fat_ptr _tmp78E=({const char*_tmp4E7="too few arguments to struct";_tag_fat(_tmp4E7,sizeof(char),28U);});_tmp648.f1=_tmp78E;});_tmp648;});void*_tmp4E5[1U];_tmp4E5[0]=& _tmp4E6;({unsigned _tmp78F=loc;Cyc_Warn_err2(_tmp78F,_tag_fat(_tmp4E5,sizeof(void*),1U));});});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp649;_tmp649.tag=0U,({struct _fat_ptr _tmp790=({const char*_tmp4EA="union expression should have one member";_tag_fat(_tmp4EA,sizeof(char),40U);});_tmp649.f1=_tmp790;});_tmp649;});void*_tmp4E8[1U];_tmp4E8[0]=& _tmp4E9;({unsigned _tmp791=loc;Cyc_Warn_err2(_tmp791,_tag_fat(_tmp4E8,sizeof(void*),1U));});});}{
struct Cyc_List_List*_tmp4EB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp4EB;}}}
# 2523
;_pop_region();}
# 2584
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2586
void*_tmp4EE=e1->r;void*_stmttmp67=_tmp4EE;void*_tmp4EF=_stmttmp67;struct Cyc_Absyn_Exp*_tmp4F0;struct Cyc_Absyn_Exp*_tmp4F1;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;switch(*((int*)_tmp4EF)){case 14U: _LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64B;_tmp64B.tag=0U,({struct _fat_ptr _tmp792=({const char*_tmp4F9="we have a cast in a lhs: ";_tag_fat(_tmp4F9,sizeof(char),26U);});_tmp64B.f1=_tmp792;});_tmp64B;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4F8=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp64A;_tmp64A.tag=4U,_tmp64A.f1=e1;_tmp64A;});void*_tmp4F6[2U];_tmp4F6[0]=& _tmp4F7,_tmp4F6[1]=& _tmp4F8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4F6,sizeof(void*),2U));});case 20U: _LL3: _tmp4F5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4EF)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp4F5;
_tmp4F4=e1a;goto _LL6;}case 23U: _LL5: _tmp4F4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4EF)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp4F4;
# 2590
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp4F3=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4EF)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp4F3;
_tmp4F2=e1a;goto _LLA;}case 21U: _LL9: _tmp4F2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4EF)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp4F2;
# 2594
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64D;_tmp64D.tag=0U,({struct _fat_ptr _tmp793=({const char*_tmp4FD="found zero pointer aggregate member assignment: ";_tag_fat(_tmp4FD,sizeof(char),49U);});_tmp64D.f1=_tmp793;});_tmp64D;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp4FC=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp64C;_tmp64C.tag=4U,_tmp64C.f1=e1;_tmp64C;});void*_tmp4FA[2U];_tmp4FA[0]=& _tmp4FB,_tmp4FA[1]=& _tmp4FC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4FA,sizeof(void*),2U));});
return 0;}case 13U: _LLB: _tmp4F1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4EF)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp4F1;
_tmp4F0=e1a;goto _LLE;}case 12U: _LLD: _tmp4F0=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4EF)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp4F0;
# 2599
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64F;_tmp64F.tag=0U,({struct _fat_ptr _tmp794=({const char*_tmp501="found zero pointer instantiate/noinstantiate: ";_tag_fat(_tmp501,sizeof(char),47U);});_tmp64F.f1=_tmp794;});_tmp64F;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp500=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp64E;_tmp64E.tag=4U,_tmp64E.f1=e1;_tmp64E;});void*_tmp4FE[2U];_tmp4FE[0]=& _tmp4FF,_tmp4FE[1]=& _tmp500;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4FE,sizeof(void*),2U));});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp503=({struct Cyc_Warn_String_Warn_Warg_struct _tmp651;_tmp651.tag=0U,({struct _fat_ptr _tmp795=({const char*_tmp505="found bad lhs in is_zero_ptr_deref: ";_tag_fat(_tmp505,sizeof(char),37U);});_tmp651.f1=_tmp795;});_tmp651;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp504=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp650;_tmp650.tag=4U,_tmp650.f1=e1;_tmp650;});void*_tmp502[2U];_tmp502[0]=& _tmp503,_tmp502[1]=& _tmp504;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp502,sizeof(void*),2U));});}_LL0:;}
# 2613
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmp506=Cyc_Absyn_compress(r);void*_stmttmp68=_tmp506;void*_tmp507=_stmttmp68;struct Cyc_Absyn_Tvar*_tmp508;enum Cyc_Absyn_AliasQual _tmp50A;enum Cyc_Absyn_KindQual _tmp509;switch(*((int*)_tmp507)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp507)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp507)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp507)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp507)->f4 == 0){_LL5: _tmp509=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp507)->f3)->kind)->v)->kind;_tmp50A=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp507)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp509;enum Cyc_Absyn_AliasQual a=_tmp50A;
# 2618
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp508=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp507)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp508;
# 2622
struct Cyc_Absyn_Kind*_tmp50B=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_rk);struct Cyc_Absyn_Kind*_stmttmp69=_tmp50B;struct Cyc_Absyn_Kind*_tmp50C=_stmttmp69;enum Cyc_Absyn_AliasQual _tmp50E;enum Cyc_Absyn_KindQual _tmp50D;_LLC: _tmp50D=_tmp50C->kind;_tmp50E=_tmp50C->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp50D;enum Cyc_Absyn_AliasQual a=_tmp50E;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp50F=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp6A=_tmp50F;void*_tmp510=_stmttmp6A;struct Cyc_Core_Opt**_tmp511;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp510)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp510)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp510)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp511=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp510)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp511;
# 2626
({struct Cyc_Core_Opt*_tmp797=({struct Cyc_Core_Opt*_tmp513=_cycalloc(sizeof(*_tmp513));({void*_tmp796=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->tag=2U,_tmp512->f1=0,_tmp512->f2=& Cyc_Kinds_rk;_tmp512;});_tmp513->v=_tmp796;});_tmp513;});*x=_tmp797;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2630
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2637
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp514=Cyc_Absyn_compress(t);void*_stmttmp6B=_tmp514;void*_tmp515=_stmttmp6B;struct Cyc_Absyn_Tvar*_tmp516;void*_tmp517;switch(*((int*)_tmp515)){case 3U: _LL1: _tmp517=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp515)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp517;
# 2640
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp516=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp515)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp516;
# 2642
struct Cyc_Absyn_Kind*_tmp518=Cyc_Kinds_tvar_kind(tv,& Cyc_Kinds_bk);struct Cyc_Absyn_Kind*_stmttmp6C=_tmp518;struct Cyc_Absyn_Kind*_tmp519=_stmttmp6C;enum Cyc_Absyn_AliasQual _tmp51B;enum Cyc_Absyn_KindQual _tmp51A;_LL8: _tmp51A=_tmp519->kind;_tmp51B=_tmp519->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp51A;enum Cyc_Absyn_AliasQual a=_tmp51B;
enum Cyc_Absyn_KindQual _tmp51C=k;switch(_tmp51C){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp51D=Cyc_Kinds_compress_kb(tv->kind);void*_stmttmp6D=_tmp51D;void*_tmp51E=_stmttmp6D;enum Cyc_Absyn_KindQual _tmp520;struct Cyc_Core_Opt**_tmp51F;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51E)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51E)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp51F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51E)->f1;_tmp520=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51E)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp51F;enum Cyc_Absyn_KindQual k=_tmp520;
# 2648
({struct Cyc_Core_Opt*_tmp79A=({struct Cyc_Core_Opt*_tmp523=_cycalloc(sizeof(*_tmp523));({void*_tmp799=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp522=_cycalloc(sizeof(*_tmp522));_tmp522->tag=2U,_tmp522->f1=0,({struct Cyc_Absyn_Kind*_tmp798=({struct Cyc_Absyn_Kind*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->kind=k,_tmp521->aliasqual=Cyc_Absyn_Aliasable;_tmp521;});_tmp522->f2=_tmp798;});_tmp522;});_tmp523->v=_tmp799;});_tmp523;});*x=_tmp79A;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
 return 1;}_LL13:;}
# 2652
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2655
 return 0;}_LL0:;}
# 2658
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
t=Cyc_Absyn_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(t,0))
return 1;{
void*_tmp524=t;struct Cyc_List_List*_tmp525;struct Cyc_List_List*_tmp527;union Cyc_Absyn_DatatypeFieldInfo _tmp526;struct Cyc_List_List*_tmp529;struct Cyc_Absyn_Aggrdecl**_tmp528;struct Cyc_List_List*_tmp52A;switch(*((int*)_tmp524)){case 6U: _LL1: _tmp52A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp524)->f1;_LL2: {struct Cyc_List_List*qts=_tmp52A;
# 2664
for(0;qts != 0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp524)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp524)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp528=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp524)->f1)->f1).KnownAggr).val;_tmp529=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp524)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp528;struct Cyc_List_List*ts=_tmp529;
# 2669
if((*adp)->impl == 0)
return 0;{
struct Cyc_List_List*_tmp52B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp52B;
{struct Cyc_List_List*_tmp52C=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp52C;for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type)))
return 1;}}
return 0;}}}else{_LL7: _LL8:
# 2681
 goto _LLA;}case 18U: _LL9: _LLA:
 return 0;case 19U: _LLB: _tmp526=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp524)->f1)->f1;_tmp527=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp524)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp526;struct Cyc_List_List*ts=_tmp527;
# 2685
union Cyc_Absyn_DatatypeFieldInfo _tmp52D=tinfo;struct Cyc_Absyn_Datatypefield*_tmp52F;struct Cyc_Absyn_Datatypedecl*_tmp52E;if((_tmp52D.UnknownDatatypefield).tag == 1){_LL10: _LL11:
 return 0;}else{_LL12: _tmp52E=((_tmp52D.KnownDatatypefield).val).f1;_tmp52F=((_tmp52D.KnownDatatypefield).val).f2;_LL13: {struct Cyc_Absyn_Datatypedecl*td=_tmp52E;struct Cyc_Absyn_Datatypefield*fld=_tmp52F;
# 2688
struct Cyc_List_List*_tmp530=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp530;
{struct Cyc_List_List*_tmp531=fld->typs;struct Cyc_List_List*typs=_tmp531;for(0;typs != 0;typs=typs->tl){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 2694
return 0;}}_LLF:;}default: goto _LLD;}case 7U: _LL5: _tmp525=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp524)->f2;_LL6: {struct Cyc_List_List*x=_tmp525;
# 2677
for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD: _LLE:
# 2696
 return 0;}_LL0:;}}
# 2703
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp532=e->r;void*_stmttmp6E=_tmp532;void*_tmp533=_stmttmp6E;struct Cyc_Absyn_Stmt*_tmp534;struct Cyc_Absyn_Exp*_tmp535;struct Cyc_Absyn_Exp*_tmp536;struct Cyc_Absyn_Exp*_tmp538;struct Cyc_Absyn_Exp*_tmp537;struct Cyc_Absyn_Exp*_tmp53A;struct Cyc_Absyn_Exp*_tmp539;struct _fat_ptr*_tmp53C;struct Cyc_Absyn_Exp*_tmp53B;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp53E;switch(*((int*)_tmp533)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp533)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp53E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp533)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp53E;
_tmp53D=e1;goto _LL6;}case 20U: _LL5: _tmp53D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp533)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp53D;
# 2708
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp53B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp533)->f1;_tmp53C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp533)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp53B;struct _fat_ptr*f=_tmp53C;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp539=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp533)->f1;_tmp53A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp533)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp539;struct Cyc_Absyn_Exp*e2=_tmp53A;
# 2711
void*_tmp53F=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp6F=_tmp53F;void*_tmp540=_stmttmp6F;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp540)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp537=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp533)->f2;_tmp538=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp533)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp537;struct Cyc_Absyn_Exp*e2=_tmp538;
# 2716
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp536=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp533)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp536;
_tmp535=e2;goto _LL10;}case 14U: _LLF: _tmp535=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp533)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp535;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp534=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp533)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp534;
# 2720
while(1){
void*_tmp541=s->r;void*_stmttmp70=_tmp541;void*_tmp542=_stmttmp70;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Stmt*_tmp545;struct Cyc_Absyn_Decl*_tmp544;struct Cyc_Absyn_Stmt*_tmp547;struct Cyc_Absyn_Stmt*_tmp546;switch(*((int*)_tmp542)){case 2U: _LL1B: _tmp546=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp542)->f1;_tmp547=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp542)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp546;struct Cyc_Absyn_Stmt*s2=_tmp547;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp544=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp542)->f1;_tmp545=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp542)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp544;struct Cyc_Absyn_Stmt*s1=_tmp545;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp543=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp542)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp543;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp549=({struct Cyc_Warn_String_Warn_Warg_struct _tmp652;_tmp652.tag=0U,({struct _fat_ptr _tmp79B=({const char*_tmp54A="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp54A,sizeof(char),40U);});_tmp652.f1=_tmp79B;});_tmp652;});void*_tmp548[1U];_tmp548[0]=& _tmp549;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp548,sizeof(void*),1U));});}_LL1A:;}}default: _LL13: _LL14:
# 2728
 return 1;}_LL0:;}
# 2745 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 2747
struct _tuple14 bogus_ans=({struct _tuple14 _tmp660;_tmp660.f1=0,_tmp660.f2=Cyc_Absyn_heap_rgn_type;_tmp660;});
void*_tmp54B=e->r;void*_stmttmp71=_tmp54B;void*_tmp54C=_stmttmp71;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54F;int _tmp552;struct _fat_ptr*_tmp551;struct Cyc_Absyn_Exp*_tmp550;int _tmp555;struct _fat_ptr*_tmp554;struct Cyc_Absyn_Exp*_tmp553;void*_tmp556;switch(*((int*)_tmp54C)){case 1U: _LL1: _tmp556=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp54C)->f1;_LL2: {void*x=_tmp556;
# 2751
void*_tmp557=x;struct Cyc_Absyn_Vardecl*_tmp558;struct Cyc_Absyn_Vardecl*_tmp559;struct Cyc_Absyn_Vardecl*_tmp55A;struct Cyc_Absyn_Vardecl*_tmp55B;switch(*((int*)_tmp557)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp55B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp557)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp55B;
# 2755
void*_tmp55C=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp72=_tmp55C;void*_tmp55D=_stmttmp72;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp55D)->tag == 4U){_LL1B: _LL1C:
# 2757
 return({struct _tuple14 _tmp653;_tmp653.f1=1,_tmp653.f2=Cyc_Absyn_heap_rgn_type;_tmp653;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp654;_tmp654.f1=(vd->tq).real_const,_tmp654.f2=Cyc_Absyn_heap_rgn_type;_tmp654;});}_LL1A:;}case 4U: _LL14: _tmp55A=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp557)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp55A;
# 2761
void*_tmp55E=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp73=_tmp55E;void*_tmp55F=_stmttmp73;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp55F)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp655;_tmp655.f1=1,_tmp655.f2=(void*)_check_null(vd->rgn);_tmp655;});}else{_LL22: _LL23:
# 2764
 vd->escapes=1;
return({struct _tuple14 _tmp656;_tmp656.f1=(vd->tq).real_const,_tmp656.f2=(void*)_check_null(vd->rgn);_tmp656;});}_LL1F:;}case 5U: _LL16: _tmp559=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp557)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp559;
# 2767
_tmp558=vd;goto _LL19;}default: _LL18: _tmp558=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp557)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp558;
# 2769
vd->escapes=1;
return({struct _tuple14 _tmp657;_tmp657.f1=(vd->tq).real_const,_tmp657.f2=(void*)_check_null(vd->rgn);_tmp657;});}}_LLD:;}case 21U: _LL3: _tmp553=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp54C)->f1;_tmp554=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp54C)->f2;_tmp555=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp54C)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp553;struct _fat_ptr*f=_tmp554;int is_tagged=_tmp555;
# 2774
if(is_tagged)return bogus_ans;{
# 2777
void*_tmp560=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp74=_tmp560;void*_tmp561=_stmttmp74;struct Cyc_Absyn_Aggrdecl*_tmp562;struct Cyc_List_List*_tmp563;switch(*((int*)_tmp561)){case 7U: _LL25: _tmp563=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp561)->f2;_LL26: {struct Cyc_List_List*fs=_tmp563;
# 2779
struct Cyc_Absyn_Aggrfield*_tmp564=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp564;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp565=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp75=_tmp565;struct _tuple14 _tmp566=_stmttmp75;void*_tmp568;int _tmp567;_LL2C: _tmp567=_tmp566.f1;_tmp568=_tmp566.f2;_LL2D: {int c=_tmp567;void*t=_tmp568;
return({struct _tuple14 _tmp658;_tmp658.f1=(finfo->tq).real_const || c,_tmp658.f2=t;_tmp658;});}}
# 2784
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp561)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp561)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp562=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp561)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp562;
# 2786
struct Cyc_Absyn_Aggrfield*_tmp569=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp569;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp56A=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp76=_tmp56A;struct _tuple14 _tmp56B=_stmttmp76;void*_tmp56D;int _tmp56C;_LL2F: _tmp56C=_tmp56B.f1;_tmp56D=_tmp56B.f2;_LL30: {int c=_tmp56C;void*t=_tmp56D;
return({struct _tuple14 _tmp659;_tmp659.f1=(finfo->tq).real_const || c,_tmp659.f2=t;_tmp659;});}}
# 2791
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp550=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp54C)->f1;_tmp551=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp54C)->f2;_tmp552=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp54C)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp550;struct _fat_ptr*f=_tmp551;int is_tagged=_tmp552;
# 2796
if(is_tagged)return bogus_ans;{
# 2799
void*_tmp56E=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp77=_tmp56E;void*_tmp56F=_stmttmp77;void*_tmp571;void*_tmp570;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56F)->tag == 3U){_LL32: _tmp570=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56F)->f1).elt_type;_tmp571=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56F)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp570;void*r=_tmp571;
# 2801
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp572=Cyc_Absyn_compress(t1);void*_stmttmp78=_tmp572;void*_tmp573=_stmttmp78;struct Cyc_Absyn_Aggrdecl*_tmp574;struct Cyc_List_List*_tmp575;switch(*((int*)_tmp573)){case 7U: _LL37: _tmp575=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp573)->f2;_LL38: {struct Cyc_List_List*fs=_tmp575;
# 2804
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp573)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp573)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp574=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp573)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp574;
# 2806
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 2809
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp65A;_tmp65A.f1=(finfo->tq).real_const,_tmp65A.f2=r;_tmp65A;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp54F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp54C)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp54F;
# 2816
void*_tmp576=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp79=_tmp576;void*_tmp577=_stmttmp79;void*_tmp579;struct Cyc_Absyn_Tqual _tmp578;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp577)->tag == 3U){_LL3E: _tmp578=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp577)->f1).elt_tq;_tmp579=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp577)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp578;void*r=_tmp579;
# 2818
return({struct _tuple14 _tmp65B;_tmp65B.f1=tq.real_const,_tmp65B.f2=r;_tmp65B;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp54D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp54C)->f1;_tmp54E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp54C)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp54D;struct Cyc_Absyn_Exp*e2=_tmp54E;
# 2824
void*t=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*_tmp57A=t;struct Cyc_Absyn_Tqual _tmp57B;void*_tmp57D;struct Cyc_Absyn_Tqual _tmp57C;struct Cyc_List_List*_tmp57E;switch(*((int*)_tmp57A)){case 6U: _LL43: _tmp57E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57A)->f1;_LL44: {struct Cyc_List_List*ts=_tmp57E;
# 2828
struct _tuple13 _tmp57F=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp7A=_tmp57F;struct _tuple13 _tmp580=_stmttmp7A;int _tmp582;unsigned _tmp581;_LL4C: _tmp581=_tmp580.f1;_tmp582=_tmp580.f2;_LL4D: {unsigned i=_tmp581;int known=_tmp582;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp583=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp583;
if(finfo != 0)
return({struct _tuple14 _tmp65C;_tmp65C.f1=((*finfo).f1).real_const,({void*_tmp79C=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp65C.f2=_tmp79C;});_tmp65C;});
return bogus_ans;}}}case 3U: _LL45: _tmp57C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57A)->f1).elt_tq;_tmp57D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57A)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp57C;void*r=_tmp57D;
# 2836
return({struct _tuple14 _tmp65D;_tmp65D.f1=tq.real_const,_tmp65D.f2=r;_tmp65D;});}case 4U: _LL47: _tmp57B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57A)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp57B;
# 2842
return({struct _tuple14 _tmp65E;_tmp65E.f1=tq.real_const,({void*_tmp79D=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp65E.f2=_tmp79D;});_tmp65E;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 2846
({struct Cyc_Warn_String_Warn_Warg_struct _tmp585=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65F;_tmp65F.tag=0U,({struct _fat_ptr _tmp79E=({const char*_tmp586="unary & applied to non-lvalue";_tag_fat(_tmp586,sizeof(char),30U);});_tmp65F.f1=_tmp79E;});_tmp65F;});void*_tmp584[1U];_tmp584[0]=& _tmp585;({unsigned _tmp79F=e->loc;Cyc_Warn_err2(_tmp79F,_tag_fat(_tmp584,sizeof(void*),1U));});});
return bogus_ans;}_LL0:;}
# 2853
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp587=({void*_tmp7A0=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7A0,b);});struct Cyc_Absyn_Exp*eopt=_tmp587;
if(eopt == 0)return;{
struct _tuple13 _tmp588=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp7B=_tmp588;struct _tuple13 _tmp589=_stmttmp7B;int _tmp58B;unsigned _tmp58A;_LL1: _tmp58A=_tmp589.f1;_tmp58B=_tmp589.f2;_LL2: {unsigned j=_tmp58A;int known=_tmp58B;
if(known && j <= i)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp58D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp664;_tmp664.tag=0U,({struct _fat_ptr _tmp7A1=({const char*_tmp592="a dereference is out of bounds: ";_tag_fat(_tmp592,sizeof(char),33U);});_tmp664.f1=_tmp7A1;});_tmp664;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp58E=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp663;_tmp663.tag=12U,_tmp663.f1=(int)j;_tmp663;});struct Cyc_Warn_String_Warn_Warg_struct _tmp58F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp662;_tmp662.tag=0U,({
struct _fat_ptr _tmp7A2=({const char*_tmp591=" <= ";_tag_fat(_tmp591,sizeof(char),5U);});_tmp662.f1=_tmp7A2;});_tmp662;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp590=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp661;_tmp661.tag=12U,_tmp661.f1=(int)i;_tmp661;});void*_tmp58C[4U];_tmp58C[0]=& _tmp58D,_tmp58C[1]=& _tmp58E,_tmp58C[2]=& _tmp58F,_tmp58C[3]=& _tmp590;({void(*_tmp7A4)(unsigned,struct _fat_ptr)=
# 2858
do_warn?Cyc_Warn_warn2: Cyc_Warn_err2;unsigned _tmp7A3=loc;_tmp7A4(_tmp7A3,_tag_fat(_tmp58C,sizeof(void*),4U));});});
# 2860
return;}}}
# 2863
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 2871
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
int(*_tmp593)(int var_okay,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_cnst_exp;int(*rec)(int var_okay,struct Cyc_Absyn_Exp*e)=_tmp593;
void*_tmp594=e->r;void*_stmttmp7C=_tmp594;void*_tmp595=_stmttmp7C;struct Cyc_List_List*_tmp596;struct Cyc_List_List*_tmp597;struct Cyc_List_List*_tmp598;struct Cyc_List_List*_tmp599;struct Cyc_List_List*_tmp59A;struct Cyc_List_List*_tmp59B;struct Cyc_Absyn_Exp*_tmp59C;struct Cyc_Absyn_Exp*_tmp59E;struct Cyc_Absyn_Exp*_tmp59D;struct Cyc_Absyn_Exp*_tmp59F;struct Cyc_Absyn_Exp*_tmp5A0;struct Cyc_Absyn_Exp*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Exp*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A7;struct Cyc_Absyn_Exp*_tmp5A6;void*_tmp5A9;switch(*((int*)_tmp595)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5A9=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL10: {void*b=_tmp5A9;
# 2884
void*_tmp5AA=b;struct Cyc_Absyn_Vardecl*_tmp5AB;struct Cyc_Absyn_Vardecl*_tmp5AC;switch(*((int*)_tmp5AA)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmp5AC=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5AA)->f1;_LL35: {struct Cyc_Absyn_Vardecl*vd=_tmp5AC;
# 2887
void*_tmp5AD=Cyc_Absyn_compress(vd->type);void*_stmttmp7D=_tmp5AD;void*_tmp5AE=_stmttmp7D;switch(*((int*)_tmp5AE)){case 4U: _LL3D: _LL3E:
 goto _LL40;case 5U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmp5AB=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5AA)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5AB;
# 2893
if((int)vd->sc != (int)Cyc_Absyn_Static)
return var_okay;{
void*_tmp5AF=Cyc_Absyn_compress(vd->type);void*_stmttmp7E=_tmp5AF;void*_tmp5B0=_stmttmp7E;switch(*((int*)_tmp5B0)){case 4U: _LL44: _LL45:
 goto _LL47;case 5U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}}case 0U: _LL38: _LL39:
# 2900
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmp5A6=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_tmp5A7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp595)->f2;_tmp5A8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp595)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5A6;struct Cyc_Absyn_Exp*e2=_tmp5A7;struct Cyc_Absyn_Exp*e3=_tmp5A8;
# 2904
return(rec(0,e1)&& rec(0,e2))&& rec(0,e3);}case 9U: _LL13: _tmp5A4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_tmp5A5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp595)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5A4;struct Cyc_Absyn_Exp*e2=_tmp5A5;
return rec(0,e1)&& rec(0,e2);}case 41U: _LL15: _tmp5A3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5A3;
_tmp5A2=e2;goto _LL18;}case 12U: _LL17: _tmp5A2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5A2;
_tmp5A1=e2;goto _LL1A;}case 13U: _LL19: _tmp5A1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5A1;
_tmp5A0=e2;goto _LL1C;}case 14U: _LL1B: _tmp5A0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp595)->f2;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp5A0;
# 2911
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 15U: _LL1D: _tmp59F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL1E: {struct Cyc_Absyn_Exp*e2=_tmp59F;
return rec(1,e2);}case 27U: _LL1F: _tmp59D=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp595)->f2;_tmp59E=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp595)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp59D;struct Cyc_Absyn_Exp*e2=_tmp59E;
return rec(0,e1)&& rec(0,e2);}case 28U: _LL21: _tmp59C=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp59C;
return rec(0,e);}case 26U: _LL23: _tmp59B=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL24: {struct Cyc_List_List*des=_tmp59B;
_tmp59A=des;goto _LL26;}case 30U: _LL25: _tmp59A=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp595)->f2;_LL26: {struct Cyc_List_List*des=_tmp59A;
_tmp599=des;goto _LL28;}case 29U: _LL27: _tmp599=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp595)->f3;_LL28: {struct Cyc_List_List*des=_tmp599;
# 2918
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL29: _tmp598=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp595)->f2;_LL2A: {struct Cyc_List_List*es=_tmp598;
_tmp597=es;goto _LL2C;}case 24U: _LL2B: _tmp597=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL2C: {struct Cyc_List_List*es=_tmp597;
_tmp596=es;goto _LL2E;}case 31U: _LL2D: _tmp596=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp595)->f1;_LL2E: {struct Cyc_List_List*es=_tmp596;
# 2925
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 2932
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 2936
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5B1=Cyc_Absyn_compress(t);void*_stmttmp7F=_tmp5B1;void*_tmp5B2=_stmttmp7F;struct Cyc_List_List*_tmp5B3;struct Cyc_List_List*_tmp5B4;void*_tmp5B5;void*_tmp5B6;struct Cyc_List_List*_tmp5B8;void*_tmp5B7;switch(*((int*)_tmp5B2)){case 0U: _LL1: _tmp5B7=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B2)->f1;_tmp5B8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5B2)->f2;_LL2: {void*c=_tmp5B7;struct Cyc_List_List*ts=_tmp5B8;
# 2940
void*_tmp5B9=c;union Cyc_Absyn_AggrInfo _tmp5BA;struct Cyc_List_List*_tmp5BB;struct Cyc_Absyn_Enumdecl*_tmp5BC;switch(*((int*)_tmp5B9)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5BC=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5B9)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5BC;
# 2945
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5BB=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5BB=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5B9)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5BB;
# 2949
{struct Cyc_List_List*_tmp5BD=fs;struct Cyc_List_List*fs2=_tmp5BD;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5BE=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp80=_tmp5BE;struct _tuple13 _tmp5BF=_stmttmp80;int _tmp5C1;unsigned _tmp5C0;_LL1D: _tmp5C0=_tmp5BF.f1;_tmp5C1=_tmp5BF.f2;_LL1E: {unsigned i=_tmp5C0;int known=_tmp5C1;
if(known && i == (unsigned)0)
return 1;}}}}
# 2956
return 0;}case 20U: _LL18: _tmp5BA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5B9)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5BA;
# 2959
struct Cyc_Absyn_Aggrdecl*_tmp5C2=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5C2;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B2)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5B6;
# 2967
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5B5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B2)->f1).elt_type;_LL6: {void*t=_tmp5B5;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5B4=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B2)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5B4;
# 2970
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7U: _LL9: _tmp5B3=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5B2)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5B3;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 2978
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 2980
struct _RegionHandle _tmp5C3=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5C3;_push_region(rgn);
{struct Cyc_List_List*_tmp5C4=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5C4;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5C5=0;_npop_handler(0U);return _tmp5C5;}}{
# 2988
int _tmp5C6=1;_npop_handler(0U);return _tmp5C6;}}
# 2981
;_pop_region();}
# 2992
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5C7=t;struct Cyc_Absyn_Typedefdecl*_tmp5C8;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5C7)->tag == 8U){_LL1: _tmp5C8=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5C7)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5C8;
# 2995
if(tdopt == 0)
return;{
struct Cyc_Absyn_Tqual _tmp5C9=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5C9;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp667;_tmp667.tag=0U,({struct _fat_ptr _tmp7A5=({const char*_tmp5CF="qualifier within typedef type ";_tag_fat(_tmp5CF,sizeof(char),31U);});_tmp667.f1=_tmp7A5;});_tmp667;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5CC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp666;_tmp666.tag=2U,_tmp666.f1=(void*)t;_tmp666;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp665;_tmp665.tag=0U,({struct _fat_ptr _tmp7A6=({const char*_tmp5CE=" is ignored";_tag_fat(_tmp5CE,sizeof(char),12U);});_tmp665.f1=_tmp7A6;});_tmp665;});void*_tmp5CA[3U];_tmp5CA[0]=& _tmp5CB,_tmp5CA[1]=& _tmp5CC,_tmp5CA[2]=& _tmp5CD;({unsigned _tmp7A7=loc;Cyc_Warn_warn2(_tmp7A7,_tag_fat(_tmp5CA,sizeof(void*),3U));});});
return;}}}else{_LL3: _LL4:
 return;}_LL0:;}
# 3006
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5D0=Cyc_Absyn_compress(t);void*_stmttmp81=_tmp5D0;void*_tmp5D1=_stmttmp81;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_PtrInfo _tmp5D3;switch(*((int*)_tmp5D1)){case 3U: _LL1: _tmp5D3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5D1)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp5D3;
return({void*_tmp7A8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7A8,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp5D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5D1)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5D2;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3016
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5D4=b;struct Cyc_Absyn_Vardecl*_tmp5D5;struct Cyc_Absyn_Vardecl*_tmp5D6;struct Cyc_Absyn_Vardecl*_tmp5D7;struct Cyc_Absyn_Vardecl*_tmp5D8;switch(*((int*)_tmp5D4)){case 5U: _LL1: _tmp5D8=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5D4)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp5D8;
_tmp5D7=x;goto _LL4;}case 4U: _LL3: _tmp5D7=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5D4)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp5D7;
_tmp5D6=x;goto _LL6;}case 3U: _LL5: _tmp5D6=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5D4)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp5D6;
_tmp5D5=x;goto _LL8;}case 1U: _LL7: _tmp5D5=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5D4)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp5D5;
return x->escapes?0: x;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3027
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5D9=0;struct Cyc_List_List*res=_tmp5D9;
for(0;l != 0;l=l->tl){
if((void**)l->hd != 0)res=({struct Cyc_List_List*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->hd=*((void**)_check_null((void**)l->hd)),_tmp5DA->tl=res;_tmp5DA;});}
return res;}
# 3034
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5DB=Cyc_Absyn_compress(t);void*_stmttmp82=_tmp5DB;void*_tmp5DC=_stmttmp82;unsigned _tmp5E1;void*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_Absyn_Tqual _tmp5DE;void*_tmp5DD;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DC)->tag == 4U){_LL1: _tmp5DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DC)->f1).elt_type;_tmp5DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DC)->f1).tq;_tmp5DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DC)->f1).num_elts;_tmp5E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DC)->f1).zero_term;_tmp5E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DC)->f1).zt_loc;_LL2: {void*et=_tmp5DD;struct Cyc_Absyn_Tqual tq=_tmp5DE;struct Cyc_Absyn_Exp*eopt=_tmp5DF;void*zt=_tmp5E0;unsigned ztl=_tmp5E1;
# 3037
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
if(convert_tag){
if(eopt->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp668;_tmp668.tag=0U,({struct _fat_ptr _tmp7A9=({const char*_tmp5E4="cannot convert tag without type!";_tag_fat(_tmp5E4,sizeof(char),33U);});_tmp668.f1=_tmp7A9;});_tmp668;});void*_tmp5E2[1U];_tmp5E2[0]=& _tmp5E3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp5E2,sizeof(void*),1U));});{
void*_tmp5E5=Cyc_Absyn_compress((void*)_check_null(eopt->topt));void*_stmttmp83=_tmp5E5;void*_tmp5E6=_stmttmp83;void*_tmp5E7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E6)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E6)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E6)->f2 != 0){_LL6: _tmp5E7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E6)->f2)->hd;_LL7: {void*t=_tmp5E7;
# 3046
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3051
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
