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
extern int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 170
extern struct _RegionHandle*Cyc_Core_heap_region;
# 173
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
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 882 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 884
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 891
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 896
void*Cyc_Absyn_compress_kb(void*);
# 902
int Cyc_Absyn_type2bool(int def,void*);
# 911
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 916
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 918
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 923
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 925
extern void*Cyc_Absyn_empty_effect;
# 927
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 929
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 954
extern void*Cyc_Absyn_fat_bound_type;
# 956
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 958
void*Cyc_Absyn_bounds_one (void);
# 960
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 964
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 983
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 985
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 988
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1011
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1021
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1030
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
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1051
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1090
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1134
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1136
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1138
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1148
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 76 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
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
void*Cyc_Warn_impos2(struct _fat_ptr);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
extern struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
extern int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
extern int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 51
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
extern struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
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
# 99
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 104
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void*Cyc_Tcutil_compress(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 109
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
# 112
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 116
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 118
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 120
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*assume,void*t1,void*t2);
# 123
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);
# 128
extern int Cyc_Tcutil_warn_alias_coerce;
# 131
extern int Cyc_Tcutil_warn_region_coerce;
# 134
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 143
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 148
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 153
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_boolko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ptrbko;
# 162
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 167
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 172
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 181
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 184
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
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple14{int f1;void*f2;};
# 250
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 253
void*Cyc_Tcutil_normalize_effect(void*e);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 258
int Cyc_Tcutil_new_tvar_id (void);
# 260
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 266
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 270
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 284
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 287
int Cyc_Tcutil_zeroable_type(void*);
# 291
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 299
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 303
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 308
int Cyc_Tcutil_tycon_cmp(void*,void*);
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*,void*);
# 38 "attributes.h"
int Cyc_Atts_sub_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 41
int Cyc_Atts_fntype_att(void*);
# 52
int Cyc_Atts_attribute_cmp(void*,void*);
# 37 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 43
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_stmttmp1=_tmp2;void*_tmp3=_stmttmp1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 49
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_stmttmp2=_tmp4;void*_tmp5=_stmttmp2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 55
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_stmttmp3=_tmp6;void*_tmp7=_stmttmp3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 62
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmp8;void*_tmp9=_stmttmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 69
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_stmttmp5=_tmpA;void*_tmpB=_stmttmp5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 76
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmpC;void*_tmpD=_stmttmp6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 83
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_stmttmp7=_tmpE;void*_tmpF=_stmttmp7;void*_tmp10;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {void*c=_tmp10;
# 86
void*_tmp11=c;switch(*((int*)_tmp11)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 93
 return 0;}_LL0:;}
# 96
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_stmttmp8=_tmp12;void*_tmp13=_stmttmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
 return 1;}else{goto _LL5;}case 2U: _LL3: _LL4:
 return 1;default: goto _LL5;}else{_LL5: _LL6:
 return 0;}_LL0:;}
# 103
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 106
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 109
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp14=Cyc_Tcutil_compress(t);void*_stmttmp9=_tmp14;void*_tmp15=_stmttmp9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 115
int Cyc_Tcutil_is_typedef_type(void*t){
void*_tmp16=Cyc_Tcutil_compress(t);void*_stmttmpA=_tmp16;void*_tmp17=_stmttmpA;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 121
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 129
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp18=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmpB=_tmp18;void*_tmp19=_stmttmpB;void*_tmp1A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19)->tag == 1U){_LL1: _LL2:
 return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp1A=_tmp19;_LL4: {void*t=_tmp1A;
return Cyc_Tcutil_is_integral_type(t);}}_LL0:;}
# 137
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp1B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmpC=_tmp1B;void*_tmp1C=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 147
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1D=Cyc_Tcutil_compress(t);void*_stmttmpD=_tmp1D;void*_tmp1E=_stmttmpD;void*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->tag == 3U){_LL1: _tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->f1).ptr_atts).zero_term;_LL2: {void*ztl=_tmp1F;
# 150
return Cyc_Tcutil_force_type2bool(0,ztl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 156
int Cyc_Tcutil_is_nullable_pointer_type(void*t,int def){
void*_tmp20=Cyc_Tcutil_compress(t);void*_stmttmpE=_tmp20;void*_tmp21=_stmttmpE;void*_tmp22;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->tag == 3U){_LL1: _tmp22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp22;
# 159
return Cyc_Tcutil_force_type2bool(def,nbl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 165
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp23=Cyc_Tcutil_compress(t);void*_stmttmpF=_tmp23;void*_tmp24=_stmttmpF;void*_tmp25;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->tag == 3U){_LL1: _tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp25;
# 168
return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,b);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 175
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp26=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp26;void*_tmp27=_stmttmp10;void*_tmp29;void*_tmp28;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->tag == 3U){_LL1: _tmp28=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).elt_type;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).ptr_atts).bounds;_LL2: {void*elt_type=_tmp28;void*b=_tmp29;
# 178
if(Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=elt_type;
return 1;}
# 182
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 189
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp2A=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp2A;void*_tmp2B=_stmttmp11;void*_tmp2D;void*_tmp2C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->tag == 3U){_LL1: _tmp2C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_type;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;_LL2: {void*elt_type=_tmp2C;void*zt=_tmp2D;
# 192
*elt_type_dest=elt_type;
return Cyc_Absyn_type2bool(0,zt);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 201
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 203
void*_tmp2E=Cyc_Tcutil_compress(t);void*_stmttmp12=_tmp2E;void*_tmp2F=_stmttmp12;void*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Tqual _tmp31;void*_tmp30;void*_tmp36;void*_tmp35;void*_tmp34;switch(*((int*)_tmp2F)){case 3U: _LL1: _tmp34=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp35=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).bounds;_tmp36=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).zero_term;_LL2: {void*elt=_tmp34;void*bnds=_tmp35;void*zt=_tmp36;
# 205
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*ptr_type=t;
*elt_type=elt;
{void*_tmp37=Cyc_Tcutil_compress(bnds);void*_stmttmp13=_tmp37;void*_tmp38=_stmttmp13;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 213
return 1;}case 4U: _LL3: _tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).tq;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).zero_term;_LL4: {void*elt=_tmp30;struct Cyc_Absyn_Tqual tq=_tmp31;struct Cyc_Absyn_Exp*n=_tmp32;void*zt=_tmp33;
# 215
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*elt_type=elt;
*is_fat=0;
({void*_tmp6A4=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp6A4;});
return 1;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 228
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 234
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp39=({void*_tmp6A5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6A5,b);});struct Cyc_Absyn_Exp*eopt=_tmp39;
if(eopt == 0)return 0;{
struct _tuple13 _tmp3A=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp14=_tmp3A;struct _tuple13 _tmp3B=_stmttmp14;int _tmp3D;unsigned _tmp3C;_LL1: _tmp3C=_tmp3B.f1;_tmp3D=_tmp3B.f2;_LL2: {unsigned i=_tmp3C;int known=_tmp3D;
return known && i == (unsigned)1;}}}
# 242
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3E=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp3E;void*_tmp3F=_stmttmp15;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;void*_tmp43;void*_tmp42;struct Cyc_List_List*_tmp45;void*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LL1: _tmp44=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f2;_LL2: {void*c=_tmp44;struct Cyc_List_List*ts=_tmp45;
# 245
void*_tmp46=c;struct Cyc_Absyn_Aggrdecl*_tmp47;switch(*((int*)_tmp46)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 goto _LL13;case 16U: _LL12: _LL13:
 goto _LL15;case 15U: _LL14: _LL15:
 return 1;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp46)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
# 252
 return 0;}else{_LL18: _tmp47=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp46)->f1).KnownAggr).val;_LL19: {struct Cyc_Absyn_Aggrdecl*ad=_tmp47;
# 254
if(ad->impl == 0)
return 0;{
struct _RegionHandle _tmp48=_new_region("rgn");struct _RegionHandle*rgn=& _tmp48;_push_region(rgn);
{struct Cyc_List_List*_tmp49=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,ad->tvs,ts);struct Cyc_List_List*inst=_tmp49;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp4A=0;_npop_handler(0U);return _tmp4A;}}}{
int _tmp4B=1;_npop_handler(0U);return _tmp4B;}}
# 257
;_pop_region();}}}default: _LL1A: _LL1B:
# 262
 return 0;}_LLB:;}case 4U: _LL3: _tmp42=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp43=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;_LL4: {void*t=_tmp42;void*zero_term=_tmp43;
# 267
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6U: _LL5: _tmp41=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F)->f1;_LL6: {struct Cyc_List_List*tqs=_tmp41;
# 269
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7U: _LL7: _tmp40=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F)->f2;_LL8: {struct Cyc_List_List*fs=_tmp40;
# 274
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 284
int Cyc_Tcutil_warn_region_coerce=0;
# 286
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 288
Cyc_Warn_verr(loc,fmt,ap);}
# 290
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 292
Cyc_Warn_vimpos(fmt,ap);}
# 294
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 296
Cyc_Warn_vwarn(sg,fmt,ap);}
# 300
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 305
void*Cyc_Tcutil_compress(void*t){
void*_tmp4C=t;void*_tmp4D;struct Cyc_Absyn_Exp*_tmp4E;struct Cyc_Absyn_Exp*_tmp4F;void**_tmp50;void**_tmp51;switch(*((int*)_tmp4C)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp51=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp4C)->f2;_LL8: {void**t2opt_ref=_tmp51;
# 311
void*ta=(void*)_check_null(*t2opt_ref);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*t2opt_ref=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f4 == 0){_LL3: _LL4:
# 308
 return t;}else{_LL5: _tmp50=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C)->f4;_LL6: {void**topt_ref=_tmp50;
_tmp51=topt_ref;goto _LL8;}}case 9U: _LL9: _tmp4F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp4C)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp4F;
# 317
Cyc_Evexp_eval_const_uint_exp(e);{
void*_tmp52=e->r;void*_stmttmp16=_tmp52;void*_tmp53=_stmttmp16;void*_tmp54;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp53)->tag == 39U){_LL12: _tmp54=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp53)->f1;_LL13: {void*t2=_tmp54;
return Cyc_Tcutil_compress(t2);}}else{_LL14: _LL15:
 return t;}_LL11:;}}case 11U: _LLB: _tmp4E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp4C)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp4E;
# 323
void*_tmp55=e->topt;void*t2=_tmp55;
return t2 != 0?t2: t;}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f2 != 0){_LLD: _tmp4D=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp4C)->f2;_LLE: {void*t=_tmp4D;
return Cyc_Tcutil_compress(t);}}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 333
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp56=Cyc_Absyn_compress_kb(kb);void*_stmttmp17=_tmp56;void*_tmp57=_stmttmp17;struct Cyc_Absyn_Kind*_tmp58;switch(*((int*)_tmp57)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->tag=1U,_tmp59->f1=0;_tmp59;});case 2U: _LL3: _tmp58=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp57)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp58;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp5A=_cycalloc(sizeof(*_tmp5A));_tmp5A->tag=2U,_tmp5A->f1=0,_tmp5A->f2=k;_tmp5A;});}default: _LL5: _LL6:
 return kb;}_LL0:;}
# 340
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp5B=_cycalloc(sizeof(*_tmp5B));_tmp5B->name=tv->name,_tmp5B->identity=- 1,({void*_tmp6A6=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp5B->kind=_tmp6A6;});_tmp5B;});}
# 343
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9*_tmp5C=arg;void*_tmp5F;struct Cyc_Absyn_Tqual _tmp5E;struct _fat_ptr*_tmp5D;_LL1: _tmp5D=_tmp5C->f1;_tmp5E=_tmp5C->f2;_tmp5F=_tmp5C->f3;_LL2: {struct _fat_ptr*x=_tmp5D;struct Cyc_Absyn_Tqual y=_tmp5E;void*t=_tmp5F;
return({struct _tuple9*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->f1=x,_tmp60->f2=y,({void*_tmp6A7=Cyc_Tcutil_copy_type(t);_tmp60->f3=_tmp6A7;});_tmp60;});}}
# 347
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->f1=(*arg).f1,({void*_tmp6A8=Cyc_Tcutil_copy_type((*arg).f2);_tmp61->f2=_tmp6A8;});_tmp61;});}
# 350
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
return e == 0?0: Cyc_Tcutil_deep_copy_exp(preserve_types,e);}
# 353
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->name=f->name,_tmp62->tq=f->tq,({void*_tmp6AA=Cyc_Tcutil_copy_type(f->type);_tmp62->type=_tmp6AA;}),_tmp62->width=f->width,_tmp62->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp6A9=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp62->requires_clause=_tmp6A9;});_tmp62;});}struct _tuple15{void*f1;void*f2;};
# 357
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp63=x;void*_tmp65;void*_tmp64;_LL1: _tmp64=_tmp63->f1;_tmp65=_tmp63->f2;_LL2: {void*r1=_tmp64;void*r2=_tmp65;
return({struct _tuple15*_tmp66=_cycalloc(sizeof(*_tmp66));({void*_tmp6AC=Cyc_Tcutil_copy_type(r1);_tmp66->f1=_tmp6AC;}),({void*_tmp6AB=Cyc_Tcutil_copy_type(r2);_tmp66->f2=_tmp6AB;});_tmp66;});}}
# 361
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 365
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp67=Cyc_Tcutil_compress(t);void*_stmttmp18=_tmp67;void*_tmp68=_stmttmp18;struct Cyc_Absyn_Datatypedecl*_tmp69;struct Cyc_Absyn_Enumdecl*_tmp6A;struct Cyc_Absyn_Aggrdecl*_tmp6B;struct Cyc_Absyn_Typedefdecl*_tmp6E;struct Cyc_List_List*_tmp6D;struct _tuple1*_tmp6C;struct Cyc_Absyn_Exp*_tmp6F;struct Cyc_Absyn_Exp*_tmp70;struct Cyc_List_List*_tmp72;enum Cyc_Absyn_AggrKind _tmp71;struct Cyc_List_List*_tmp73;struct Cyc_Absyn_Vardecl*_tmp81;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_Exp*_tmp7F;struct Cyc_List_List*_tmp7E;struct Cyc_Absyn_Exp*_tmp7D;struct Cyc_List_List*_tmp7C;struct Cyc_List_List*_tmp7B;struct Cyc_Absyn_VarargInfo*_tmp7A;int _tmp79;struct Cyc_List_List*_tmp78;void*_tmp77;struct Cyc_Absyn_Tqual _tmp76;void*_tmp75;struct Cyc_List_List*_tmp74;unsigned _tmp86;void*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_Absyn_Tqual _tmp83;void*_tmp82;struct Cyc_Absyn_PtrLoc*_tmp8D;void*_tmp8C;void*_tmp8B;void*_tmp8A;void*_tmp89;struct Cyc_Absyn_Tqual _tmp88;void*_tmp87;struct Cyc_Absyn_Tvar*_tmp8E;struct Cyc_List_List*_tmp90;void*_tmp8F;void*_tmp91;switch(*((int*)_tmp68)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp68)->f2 == 0){_LL1: _tmp91=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp68)->f1;_LL2: {void*c=_tmp91;
return t;}}else{_LL3: _tmp8F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp68)->f1;_tmp90=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp68)->f2;_LL4: {void*c=_tmp8F;struct Cyc_List_List*ts=_tmp90;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp92=_cycalloc(sizeof(*_tmp92));_tmp92->tag=0U,_tmp92->f1=c,({struct Cyc_List_List*_tmp6AD=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp92->f2=_tmp6AD;});_tmp92;});}}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp8E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp68)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp8E;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3U: _LL9: _tmp87=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68)->f1).elt_type;_tmp88=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68)->f1).elt_tq;_tmp89=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68)->f1).ptr_atts).rgn;_tmp8A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68)->f1).ptr_atts).nullable;_tmp8B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68)->f1).ptr_atts).bounds;_tmp8C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68)->f1).ptr_atts).zero_term;_tmp8D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68)->f1).ptr_atts).ptrloc;_LLA: {void*elt=_tmp87;struct Cyc_Absyn_Tqual tq=_tmp88;void*rgn=_tmp89;void*nbl=_tmp8A;void*bs=_tmp8B;void*zt=_tmp8C;struct Cyc_Absyn_PtrLoc*loc=_tmp8D;
# 372
void*_tmp93=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp93;
void*_tmp94=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp94;
void*_tmp95=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp95;
struct Cyc_Absyn_Tqual _tmp96=tq;struct Cyc_Absyn_Tqual tq2=_tmp96;
# 377
void*_tmp97=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp97;
void*_tmp98=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp98;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=3U,(_tmp99->f1).elt_type=elt2,(_tmp99->f1).elt_tq=tq2,((_tmp99->f1).ptr_atts).rgn=rgn2,((_tmp99->f1).ptr_atts).nullable=nbl2,((_tmp99->f1).ptr_atts).bounds=bs2,((_tmp99->f1).ptr_atts).zero_term=zt2,((_tmp99->f1).ptr_atts).ptrloc=loc;_tmp99;});}case 4U: _LLB: _tmp82=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68)->f1).elt_type;_tmp83=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68)->f1).tq;_tmp84=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68)->f1).num_elts;_tmp85=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68)->f1).zero_term;_tmp86=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp68)->f1).zt_loc;_LLC: {void*et=_tmp82;struct Cyc_Absyn_Tqual tq=_tmp83;struct Cyc_Absyn_Exp*eo=_tmp84;void*zt=_tmp85;unsigned ztl=_tmp86;
# 381
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9A=_cycalloc(sizeof(*_tmp9A));_tmp9A->tag=4U,({void*_tmp6B0=Cyc_Tcutil_copy_type(et);(_tmp9A->f1).elt_type=_tmp6B0;}),(_tmp9A->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp9A->f1).num_elts=_tmp6AF;}),({
void*_tmp6AE=Cyc_Tcutil_copy_type(zt);(_tmp9A->f1).zero_term=_tmp6AE;}),(_tmp9A->f1).zt_loc=ztl;_tmp9A;});}case 5U: _LLD: _tmp74=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).tvars;_tmp75=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).effect;_tmp76=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).ret_tqual;_tmp77=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).ret_type;_tmp78=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).args;_tmp79=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).c_varargs;_tmp7A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).cyc_varargs;_tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).rgn_po;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).attributes;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).requires_clause;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).requires_relns;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).ensures_clause;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).ensures_relns;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp68)->f1).return_value;_LLE: {struct Cyc_List_List*tvs=_tmp74;void*effopt=_tmp75;struct Cyc_Absyn_Tqual rt_tq=_tmp76;void*rt=_tmp77;struct Cyc_List_List*args=_tmp78;int c_varargs=_tmp79;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp7A;struct Cyc_List_List*rgn_po=_tmp7B;struct Cyc_List_List*atts=_tmp7C;struct Cyc_Absyn_Exp*req=_tmp7D;struct Cyc_List_List*req_rlns=_tmp7E;struct Cyc_Absyn_Exp*ens=_tmp7F;struct Cyc_List_List*ens_rlns=_tmp80;struct Cyc_Absyn_Vardecl*ret_val=_tmp81;
# 384
struct Cyc_List_List*_tmp9B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp9B;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmp9C=Cyc_Tcutil_copy_type(rt);void*rt2=_tmp9C;
struct Cyc_List_List*_tmp9D=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmp9D;
int _tmp9E=c_varargs;int c_varargs2=_tmp9E;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=cyc_varargs == 0?0:({struct Cyc_Absyn_VarargInfo*_tmpA7=_cycalloc(sizeof(*_tmpA7));*_tmpA7=*cyc_varargs;_tmpA7;});
if(cyc_varargs != 0)
({void*_tmp6B1=Cyc_Tcutil_copy_type(cyc_varargs->type);((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_varargs2))->type=_tmp6B1;});{
struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmp9F;
struct Cyc_List_List*_tmpA0=atts;struct Cyc_List_List*atts2=_tmpA0;
struct Cyc_Absyn_Exp*_tmpA1=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmpA1;
struct Cyc_List_List*_tmpA2=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmpA2;
struct Cyc_Absyn_Exp*_tmpA3=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmpA3;
struct Cyc_List_List*_tmpA4=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmpA4;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmpA5=_cycalloc(sizeof(*_tmpA5));*_tmpA5=*ret_val;_tmpA5;});
({void*_tmp6B2=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp6B2;});
if((unsigned)ret_val2->rgn)({void*_tmp6B3=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp6B3;});}
# 404
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->tag=5U,(_tmpA6->f1).tvars=tvs2,(_tmpA6->f1).effect=effopt2,(_tmpA6->f1).ret_tqual=rt_tq,(_tmpA6->f1).ret_type=rt2,(_tmpA6->f1).args=args2,(_tmpA6->f1).c_varargs=c_varargs2,(_tmpA6->f1).cyc_varargs=cyc_varargs2,(_tmpA6->f1).rgn_po=rgn_po2,(_tmpA6->f1).attributes=atts2,(_tmpA6->f1).requires_clause=req2,(_tmpA6->f1).requires_relns=req_rlns2,(_tmpA6->f1).ensures_clause=ens2,(_tmpA6->f1).ensures_relns=ens_rlns2,(_tmpA6->f1).return_value=ret_val2;_tmpA6;});}}case 6U: _LLF: _tmp73=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp68)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp73;
# 407
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=6U,({struct Cyc_List_List*_tmp6B4=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmpA8->f1=_tmp6B4;});_tmpA8;});}case 7U: _LL11: _tmp71=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68)->f1;_tmp72=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp68)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp71;struct Cyc_List_List*fs=_tmp72;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=7U,_tmpA9->f1=k,({struct Cyc_List_List*_tmp6B5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmpA9->f2=_tmp6B5;});_tmpA9;});}case 9U: _LL13: _tmp70=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp68)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp70;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=9U,_tmpAA->f1=e;_tmpAA;});}case 11U: _LL15: _tmp6F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp68)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp6F;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=11U,_tmpAB->f1=e;_tmpAB;});}case 8U: _LL17: _tmp6C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68)->f1;_tmp6D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68)->f2;_tmp6E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp68)->f3;_LL18: {struct _tuple1*tdn=_tmp6C;struct Cyc_List_List*ts=_tmp6D;struct Cyc_Absyn_Typedefdecl*td=_tmp6E;
# 412
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=8U,_tmpAC->f1=tdn,({struct Cyc_List_List*_tmp6B6=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmpAC->f2=_tmp6B6;}),_tmpAC->f3=td,_tmpAC->f4=0;_tmpAC;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68)->f1)->r)){case 0U: _LL19: _tmp6B=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp6B;
# 415
struct Cyc_List_List*_tmpAD=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpAD;
return({union Cyc_Absyn_AggrInfo _tmp6B7=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp6B7,targs);});}case 1U: _LL1B: _tmp6A=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp6A;
# 418
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp69=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp68)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp69;
# 420
struct Cyc_List_List*_tmpAE=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpAE;
return({union Cyc_Absyn_DatatypeInfo _tmp6B8=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp61C;_tmp61C.name=dd->name,_tmp61C.is_extensible=0;_tmp61C;}));Cyc_Absyn_datatype_type(_tmp6B8,targs);});}}}_LL0:;}
# 426
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpAF=d;struct _fat_ptr*_tmpB0;struct Cyc_Absyn_Exp*_tmpB1;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAF)->tag == 0U){_LL1: _tmpB1=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpAF)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpB1;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=0U,({struct Cyc_Absyn_Exp*_tmp6B9=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpB2->f1=_tmp6B9;});_tmpB2;});}}else{_LL3: _tmpB0=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpAF)->f1;_LL4: {struct _fat_ptr*v=_tmpB0;
return d;}}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 432
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 434
return({struct _tuple16*_tmpB3=_cycalloc(sizeof(*_tmpB3));({struct Cyc_List_List*_tmp6BB=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB3->f1=_tmp6BB;}),({
struct Cyc_Absyn_Exp*_tmp6BA=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB3->f2=_tmp6BA;});_tmpB3;});}
# 438
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*(*_tmpB4)(int preserve_types,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_deep_copy_exp;struct Cyc_Absyn_Exp*(*DC)(int preserve_types,struct Cyc_Absyn_Exp*e)=_tmpB4;
struct Cyc_Absyn_Exp*new_e;
int _tmpB5=preserve_types;int pt=_tmpB5;
{void*_tmpB6=e->r;void*_stmttmp19=_tmpB6;void*_tmpB7=_stmttmp19;struct Cyc_Absyn_Exp*_tmpB8;struct Cyc_List_List*_tmpBD;struct Cyc_List_List*_tmpBC;struct Cyc_List_List*_tmpBB;struct _fat_ptr _tmpBA;int _tmpB9;void*_tmpBE;struct _fat_ptr*_tmpC0;struct Cyc_Absyn_Exp*_tmpBF;struct Cyc_List_List*_tmpC2;struct Cyc_Core_Opt*_tmpC1;struct Cyc_Absyn_Exp*_tmpC4;struct Cyc_Absyn_Exp*_tmpC3;int _tmpCA;int _tmpC9;struct Cyc_Absyn_Exp*_tmpC8;void**_tmpC7;struct Cyc_Absyn_Exp*_tmpC6;int _tmpC5;struct Cyc_Absyn_Enumfield*_tmpCC;void*_tmpCB;struct Cyc_Absyn_Enumfield*_tmpCE;struct Cyc_Absyn_Enumdecl*_tmpCD;struct Cyc_List_List*_tmpD2;void*_tmpD1;struct Cyc_Absyn_Tqual _tmpD0;struct _fat_ptr*_tmpCF;struct Cyc_List_List*_tmpD4;void*_tmpD3;struct Cyc_Absyn_Aggrdecl*_tmpD8;struct Cyc_List_List*_tmpD7;struct Cyc_List_List*_tmpD6;struct _tuple1*_tmpD5;int _tmpDB;void*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;int _tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Vardecl*_tmpDC;struct Cyc_Absyn_Datatypefield*_tmpE2;struct Cyc_Absyn_Datatypedecl*_tmpE1;struct Cyc_List_List*_tmpE0;struct Cyc_List_List*_tmpE3;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;int _tmpE9;int _tmpE8;struct _fat_ptr*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;int _tmpED;int _tmpEC;struct _fat_ptr*_tmpEB;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_List_List*_tmpEE;struct Cyc_Absyn_Exp*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpF2;void*_tmpF1;struct Cyc_Absyn_Exp*_tmpF3;void*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;enum Cyc_Absyn_Coercion _tmpFB;int _tmpFA;struct Cyc_Absyn_Exp*_tmpF9;void*_tmpF8;struct Cyc_List_List*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFE;int _tmp100;struct Cyc_Absyn_Exp*_tmpFF;int _tmp104;struct Cyc_Absyn_VarargCallInfo*_tmp103;struct Cyc_List_List*_tmp102;struct Cyc_Absyn_Exp*_tmp101;int _tmp10D;int _tmp10C;void*_tmp10B;struct Cyc_Absyn_Tqual _tmp10A;struct _fat_ptr*_tmp109;struct Cyc_List_List*_tmp108;int _tmp107;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Core_Opt*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;enum Cyc_Absyn_Incrementor _tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_List_List*_tmp11D;enum Cyc_Absyn_Primop _tmp11C;struct _fat_ptr _tmp11E;void*_tmp11F;union Cyc_Absyn_Cnst _tmp120;switch(*((int*)_tmpB7)){case 0U: _LL1: _tmp120=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp120;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp11F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL4: {void*b=_tmp11F;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp11E=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL6: {struct _fat_ptr p=_tmp11E;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp11C=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp11D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp11C;struct Cyc_List_List*es=_tmp11D;
new_e=({enum Cyc_Absyn_Primop _tmp6BD=p;struct Cyc_List_List*_tmp6BC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_primop_exp(_tmp6BD,_tmp6BC,e->loc);});goto _LL0;}case 5U: _LL9: _tmp11A=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp11B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp11A;enum Cyc_Absyn_Incrementor i=_tmp11B;
new_e=({struct Cyc_Absyn_Exp*_tmp6BF=DC(pt,e1);enum Cyc_Absyn_Incrementor _tmp6BE=i;Cyc_Absyn_increment_exp(_tmp6BF,_tmp6BE,e->loc);});goto _LL0;}case 7U: _LLB: _tmp118=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp119=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp118;struct Cyc_Absyn_Exp*e2=_tmp119;
new_e=({struct Cyc_Absyn_Exp*_tmp6C1=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6C0=DC(pt,e2);Cyc_Absyn_and_exp(_tmp6C1,_tmp6C0,e->loc);});goto _LL0;}case 8U: _LLD: _tmp116=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp117=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp116;struct Cyc_Absyn_Exp*e2=_tmp117;
new_e=({struct Cyc_Absyn_Exp*_tmp6C3=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6C2=DC(pt,e2);Cyc_Absyn_or_exp(_tmp6C3,_tmp6C2,e->loc);});goto _LL0;}case 9U: _LLF: _tmp114=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp115=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp114;struct Cyc_Absyn_Exp*e2=_tmp115;
new_e=({struct Cyc_Absyn_Exp*_tmp6C5=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6C4=DC(pt,e2);Cyc_Absyn_seq_exp(_tmp6C5,_tmp6C4,e->loc);});goto _LL0;}case 6U: _LL11: _tmp111=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp112=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmp113=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp112;struct Cyc_Absyn_Exp*e3=_tmp113;
# 452
new_e=({struct Cyc_Absyn_Exp*_tmp6C8=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6C7=DC(pt,e2);struct Cyc_Absyn_Exp*_tmp6C6=DC(pt,e3);Cyc_Absyn_conditional_exp(_tmp6C8,_tmp6C7,_tmp6C6,e->loc);});goto _LL0;}case 4U: _LL13: _tmp10E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp10F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmp110=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp10E;struct Cyc_Core_Opt*po=_tmp10F;struct Cyc_Absyn_Exp*e2=_tmp110;
# 454
new_e=({struct Cyc_Absyn_Exp*_tmp6CB=DC(pt,e1);struct Cyc_Core_Opt*_tmp6CA=(unsigned)po?({struct Cyc_Core_Opt*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->v=(void*)po->v;_tmp121;}): 0;struct Cyc_Absyn_Exp*_tmp6C9=DC(pt,e2);Cyc_Absyn_assignop_exp(_tmp6CB,_tmp6CA,_tmp6C9,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f3 != 0){_LL15: _tmp105=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp106=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmp107=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f3)->num_varargs;_tmp108=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f3)->injectors;_tmp109=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f3)->vai)->name;_tmp10A=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f3)->vai)->tq;_tmp10B=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f3)->vai)->type;_tmp10C=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f3)->vai)->inject;_tmp10D=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp105;struct Cyc_List_List*es=_tmp106;int n=_tmp107;struct Cyc_List_List*is=_tmp108;struct _fat_ptr*nm=_tmp109;struct Cyc_Absyn_Tqual tq=_tmp10A;void*t=_tmp10B;int i=_tmp10C;int resolved=_tmp10D;
# 457
new_e=({void*_tmp6D1=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=10U,({
struct Cyc_Absyn_Exp*_tmp6D0=DC(pt,e1);_tmp124->f1=_tmp6D0;}),({struct Cyc_List_List*_tmp6CF=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp124->f2=_tmp6CF;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp6CE=({struct Cyc_Absyn_VarargCallInfo*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->num_varargs=n,_tmp123->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp6CD=({struct Cyc_Absyn_VarargInfo*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->name=nm,_tmp122->tq=tq,({void*_tmp6CC=Cyc_Tcutil_copy_type(t);_tmp122->type=_tmp6CC;}),_tmp122->inject=i;_tmp122;});_tmp123->vai=_tmp6CD;});_tmp123;});
# 459
_tmp124->f3=_tmp6CE;}),_tmp124->f4=resolved;_tmp124;});
# 457
Cyc_Absyn_new_exp(_tmp6D1,e->loc);});
# 462
goto _LL0;}}else{_LL17: _tmp101=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp102=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmp103=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_tmp104=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB7)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp101;struct Cyc_List_List*es=_tmp102;struct Cyc_Absyn_VarargCallInfo*vci=_tmp103;int resolved=_tmp104;
# 464
new_e=({void*_tmp6D4=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=10U,({struct Cyc_Absyn_Exp*_tmp6D3=DC(pt,e1);_tmp125->f1=_tmp6D3;}),({struct Cyc_List_List*_tmp6D2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp125->f2=_tmp6D2;}),_tmp125->f3=vci,_tmp125->f4=resolved;_tmp125;});Cyc_Absyn_new_exp(_tmp6D4,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmpFF=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmp100=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpFF;int b=_tmp100;
# 467
new_e=b?({struct Cyc_Absyn_Exp*_tmp6D6=DC(pt,e1);Cyc_Absyn_rethrow_exp(_tmp6D6,e->loc);}):({struct Cyc_Absyn_Exp*_tmp6D5=DC(pt,e1);Cyc_Absyn_throw_exp(_tmp6D5,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmpFE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpFE;
# 470
new_e=({struct Cyc_Absyn_Exp*_tmp6D7=DC(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp6D7,e->loc);});goto _LL0;}case 13U: _LL1D: _tmpFC=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpFD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpFC;struct Cyc_List_List*ts=_tmpFD;
# 472
new_e=({struct Cyc_Absyn_Exp*_tmp6D9=DC(pt,e1);struct Cyc_List_List*_tmp6D8=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp6D9,_tmp6D8,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpF8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpF9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmpFA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_tmpFB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB7)->f4;_LL20: {void*t=_tmpF8;struct Cyc_Absyn_Exp*e1=_tmpF9;int b=_tmpFA;enum Cyc_Absyn_Coercion c=_tmpFB;
# 474
new_e=({void*_tmp6DD=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp6DC=DC(pt,e1);int _tmp6DB=b;enum Cyc_Absyn_Coercion _tmp6DA=c;Cyc_Absyn_cast_exp(_tmp6DD,_tmp6DC,_tmp6DB,_tmp6DA,e->loc);});goto _LL0;}case 16U: _LL21: _tmpF6=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpF7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpF6;struct Cyc_Absyn_Exp*e1=_tmpF7;
# 476
new_e=({struct Cyc_Absyn_Exp*_tmp6DF=(unsigned)eo?DC(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp6DE=DC(pt,e1);Cyc_Absyn_New_exp(_tmp6DF,_tmp6DE,e->loc);});goto _LL0;}case 15U: _LL23: _tmpF5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpF5;
new_e=({struct Cyc_Absyn_Exp*_tmp6E0=DC(pt,e1);Cyc_Absyn_address_exp(_tmp6E0,e->loc);});goto _LL0;}case 17U: _LL25: _tmpF4=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL26: {void*t=_tmpF4;
new_e=({void*_tmp6E1=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp6E1,e->loc);});goto _LL0;}case 18U: _LL27: _tmpF3=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF3;
new_e=({struct Cyc_Absyn_Exp*_tmp6E2=DC(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp6E2,e->loc);});goto _LL0;}case 19U: _LL29: _tmpF1=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpF2=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL2A: {void*t=_tmpF1;struct Cyc_List_List*ofs=_tmpF2;
new_e=({void*_tmp6E4=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp6E3=ofs;Cyc_Absyn_offsetof_exp(_tmp6E4,_tmp6E3,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpF0=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpF0;
new_e=({struct Cyc_Absyn_Exp*_tmp6E5=DC(pt,e1);Cyc_Absyn_deref_exp(_tmp6E5,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpEF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpEF;
new_e=({struct Cyc_Absyn_Exp*_tmp6E6=DC(pt,e1);Cyc_Absyn_extension_exp(_tmp6E6,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpEE=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL30: {struct Cyc_List_List*es=_tmpEE;
new_e=({struct Cyc_List_List*_tmp6E7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_tuple_exp(_tmp6E7,e->loc);});goto _LL0;}case 21U: _LL31: _tmpEA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpEB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmpEC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_tmpED=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB7)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpEA;struct _fat_ptr*n=_tmpEB;int f1=_tmpEC;int f2=_tmpED;
# 485
new_e=({void*_tmp6E9=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=21U,({struct Cyc_Absyn_Exp*_tmp6E8=DC(pt,e1);_tmp126->f1=_tmp6E8;}),_tmp126->f2=n,_tmp126->f3=f1,_tmp126->f4=f2;_tmp126;});Cyc_Absyn_new_exp(_tmp6E9,e->loc);});goto _LL0;}case 22U: _LL33: _tmpE6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpE7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmpE8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_tmpE9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB7)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpE6;struct _fat_ptr*n=_tmpE7;int f1=_tmpE8;int f2=_tmpE9;
# 487
new_e=({void*_tmp6EB=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=22U,({struct Cyc_Absyn_Exp*_tmp6EA=DC(pt,e1);_tmp127->f1=_tmp6EA;}),_tmp127->f2=n,_tmp127->f3=f1,_tmp127->f4=f2;_tmp127;});Cyc_Absyn_new_exp(_tmp6EB,e->loc);});goto _LL0;}case 23U: _LL35: _tmpE4=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpE5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpE4;struct Cyc_Absyn_Exp*e2=_tmpE5;
# 489
new_e=({struct Cyc_Absyn_Exp*_tmp6ED=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6EC=DC(pt,e2);Cyc_Absyn_subscript_exp(_tmp6ED,_tmp6EC,e->loc);});goto _LL0;}case 26U: _LL37: _tmpE3=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL38: {struct Cyc_List_List*eds=_tmpE3;
# 491
new_e=({void*_tmp6EF=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=26U,({struct Cyc_List_List*_tmp6EE=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp128->f1=_tmp6EE;});_tmp128;});Cyc_Absyn_new_exp(_tmp6EF,e->loc);});goto _LL0;}case 31U: _LL39: _tmpE0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpE1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmpE2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_LL3A: {struct Cyc_List_List*es=_tmpE0;struct Cyc_Absyn_Datatypedecl*dtd=_tmpE1;struct Cyc_Absyn_Datatypefield*dtf=_tmpE2;
# 493
new_e=({void*_tmp6F1=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=31U,({struct Cyc_List_List*_tmp6F0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp129->f1=_tmp6F0;}),_tmp129->f2=dtd,_tmp129->f3=dtf;_tmp129;});Cyc_Absyn_new_exp(_tmp6F1,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpDC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpDD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmpDE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_tmpDF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB7)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpDC;struct Cyc_Absyn_Exp*e1=_tmpDD;struct Cyc_Absyn_Exp*e2=_tmpDE;int b=_tmpDF;
# 495
new_e=({void*_tmp6F4=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=27U,_tmp12A->f1=vd,({struct Cyc_Absyn_Exp*_tmp6F3=DC(pt,e1);_tmp12A->f2=_tmp6F3;}),({struct Cyc_Absyn_Exp*_tmp6F2=DC(pt,e2);_tmp12A->f3=_tmp6F2;}),_tmp12A->f4=b;_tmp12A;});Cyc_Absyn_new_exp(_tmp6F4,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpD9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpDA=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmpDB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpD9;void*t=_tmpDA;int b=_tmpDB;
# 497
new_e=({void*_tmp6F7=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=28U,({struct Cyc_Absyn_Exp*_tmp6F6=DC(pt,e);_tmp12B->f1=_tmp6F6;}),({void*_tmp6F5=Cyc_Tcutil_copy_type(t);_tmp12B->f2=_tmp6F5;}),_tmp12B->f3=b;_tmp12B;});Cyc_Absyn_new_exp(_tmp6F7,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpD5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpD6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmpD7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_tmpD8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB7)->f4;_LL40: {struct _tuple1*n=_tmpD5;struct Cyc_List_List*ts=_tmpD6;struct Cyc_List_List*eds=_tmpD7;struct Cyc_Absyn_Aggrdecl*agr=_tmpD8;
# 500
new_e=({void*_tmp6FA=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=29U,_tmp12C->f1=n,({struct Cyc_List_List*_tmp6F9=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp12C->f2=_tmp6F9;}),({struct Cyc_List_List*_tmp6F8=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12C->f3=_tmp6F8;}),_tmp12C->f4=agr;_tmp12C;});Cyc_Absyn_new_exp(_tmp6FA,e->loc);});
# 502
goto _LL0;}case 30U: _LL41: _tmpD3=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpD4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL42: {void*t=_tmpD3;struct Cyc_List_List*eds=_tmpD4;
# 504
new_e=({void*_tmp6FD=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=30U,({void*_tmp6FC=Cyc_Tcutil_copy_type(t);_tmp12D->f1=_tmp6FC;}),({struct Cyc_List_List*_tmp6FB=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12D->f2=_tmp6FB;});_tmp12D;});Cyc_Absyn_new_exp(_tmp6FD,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpCF=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB7)->f1)->f1;_tmpD0=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB7)->f1)->f2;_tmpD1=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB7)->f1)->f3;_tmpD2=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL44: {struct _fat_ptr*vopt=_tmpCF;struct Cyc_Absyn_Tqual tq=_tmpD0;void*t=_tmpD1;struct Cyc_List_List*eds=_tmpD2;
# 507
new_e=({void*_tmp701=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->tag=25U,({struct _tuple9*_tmp700=({struct _tuple9*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->f1=vopt,_tmp12E->f2=tq,({void*_tmp6FF=Cyc_Tcutil_copy_type(t);_tmp12E->f3=_tmp6FF;});_tmp12E;});_tmp12F->f1=_tmp700;}),({
struct Cyc_List_List*_tmp6FE=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12F->f2=_tmp6FE;});_tmp12F;});
# 507
Cyc_Absyn_new_exp(_tmp701,e->loc);});
# 509
goto _LL0;}case 32U: _LL45: _tmpCD=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpCE=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpCD;struct Cyc_Absyn_Enumfield*ef=_tmpCE;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpCB=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpCC=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL48: {void*t=_tmpCB;struct Cyc_Absyn_Enumfield*ef=_tmpCC;
# 512
new_e=({void*_tmp703=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=33U,({void*_tmp702=Cyc_Tcutil_copy_type(t);_tmp130->f1=_tmp702;}),_tmp130->f2=ef;_tmp130;});Cyc_Absyn_new_exp(_tmp703,e->loc);});goto _LL0;}case 34U: _LL49: _tmpC5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB7)->f1).is_calloc;_tmpC6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB7)->f1).rgn;_tmpC7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB7)->f1).elt_type;_tmpC8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB7)->f1).num_elts;_tmpC9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB7)->f1).fat_result;_tmpCA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB7)->f1).inline_call;_LL4A: {int ic=_tmpC5;struct Cyc_Absyn_Exp*r=_tmpC6;void**t=_tmpC7;struct Cyc_Absyn_Exp*n=_tmpC8;int res=_tmpC9;int inlc=_tmpCA;
# 514
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=DC(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp131=_cycalloc(sizeof(*_tmp131));({void*_tmp704=Cyc_Tcutil_copy_type(*t);*_tmp131=_tmp704;});_tmp131;});
({void*_tmp705=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=34U,(_tmp132->f1).is_calloc=ic,(_tmp132->f1).rgn=r1,(_tmp132->f1).elt_type=t1,(_tmp132->f1).num_elts=n,(_tmp132->f1).fat_result=res,(_tmp132->f1).inline_call=inlc;_tmp132;});e2->r=_tmp705;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpC3=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpC4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpC3;struct Cyc_Absyn_Exp*e2=_tmpC4;
new_e=({struct Cyc_Absyn_Exp*_tmp707=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp706=DC(pt,e2);Cyc_Absyn_swap_exp(_tmp707,_tmp706,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpC1=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpC2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpC1;struct Cyc_List_List*eds=_tmpC2;
# 522
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->v=(struct _tuple1*)nopt->v;_tmp133;});
new_e=({void*_tmp709=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->tag=36U,_tmp134->f1=nopt1,({struct Cyc_List_List*_tmp708=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp134->f2=_tmp708;});_tmp134;});Cyc_Absyn_new_exp(_tmp709,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 527
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp70A=({const char*_tmp135="deep_copy: statement expressions unsupported";_tag_fat(_tmp135,sizeof(char),45U);});_tmp136->f1=_tmp70A;});_tmp136;}));case 38U: _LL51: _tmpBF=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpC0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpBF;struct _fat_ptr*fn=_tmpC0;
# 529
new_e=({void*_tmp70C=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->tag=38U,({struct Cyc_Absyn_Exp*_tmp70B=DC(pt,e1);_tmp137->f1=_tmp70B;}),_tmp137->f2=fn;_tmp137;});Cyc_Absyn_new_exp(_tmp70C,e->loc);});goto _LL0;}case 39U: _LL53: _tmpBE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL54: {void*t=_tmpBE;
new_e=({void*_tmp70D=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp70D,e->loc);});goto _LL0;}case 40U: _LL55: _tmpB9=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_tmpBA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB7)->f2;_tmpBB=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB7)->f3;_tmpBC=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB7)->f4;_tmpBD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB7)->f5;_LL56: {int v=_tmpB9;struct _fat_ptr t=_tmpBA;struct Cyc_List_List*o=_tmpBB;struct Cyc_List_List*i=_tmpBC;struct Cyc_List_List*c=_tmpBD;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _LL57: _tmpB8=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpB7)->f1;_LL58: {struct Cyc_Absyn_Exp*e1=_tmpB8;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 535
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 539
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 550 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp138=ka1;enum Cyc_Absyn_AliasQual _tmp13A;enum Cyc_Absyn_KindQual _tmp139;_LL1: _tmp139=_tmp138->kind;_tmp13A=_tmp138->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp139;enum Cyc_Absyn_AliasQual a1=_tmp13A;
struct Cyc_Absyn_Kind*_tmp13B=ka2;enum Cyc_Absyn_AliasQual _tmp13D;enum Cyc_Absyn_KindQual _tmp13C;_LL4: _tmp13C=_tmp13B->kind;_tmp13D=_tmp13B->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp13C;enum Cyc_Absyn_AliasQual a2=_tmp13D;
# 554
if((int)k1 != (int)k2){
struct _tuple17 _tmp13E=({struct _tuple17 _tmp61D;_tmp61D.f1=k1,_tmp61D.f2=k2;_tmp61D;});struct _tuple17 _stmttmp1A=_tmp13E;struct _tuple17 _tmp13F=_stmttmp1A;switch(_tmp13F.f1){case Cyc_Absyn_BoxKind: switch(_tmp13F.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp13F.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 562
if((int)a1 != (int)a2){
struct _tuple18 _tmp140=({struct _tuple18 _tmp61E;_tmp61E.f1=a1,_tmp61E.f2=a2;_tmp61E;});struct _tuple18 _stmttmp1B=_tmp140;struct _tuple18 _tmp141=_stmttmp1B;switch(_tmp141.f1){case Cyc_Absyn_Aliasable: if(_tmp141.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp141.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 568
return 1;}}}
# 571
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp142=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1C=_tmp142;void*_tmp143=_stmttmp1C;struct Cyc_Absyn_Kind*_tmp144;struct Cyc_Absyn_Kind*_tmp145;switch(*((int*)_tmp143)){case 0U: _LL1: _tmp145=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp143)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp145;
return k;}case 2U: _LL3: _tmp144=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp143)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp144;
return k;}default: _LL5: _LL6:
({void*_tmp70E=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp146=_cycalloc(sizeof(*_tmp146));_tmp146->tag=2U,_tmp146->f1=0,_tmp146->f2=def;_tmp146;});tv->kind=_tmp70E;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 579
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp147=Cyc_Tcutil_compress(t);void*_stmttmp1D=_tmp147;void*_tmp148=_stmttmp1D;struct Cyc_Absyn_Tvar*_tmp149;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp148)->tag == 2U){_LL1: _tmp149=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp148)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp149;
# 582
void*_tmp14A=tv->kind;void*oldkb=_tmp14A;
tv->kind=kb;
return({struct _tuple19 _tmp61F;_tmp61F.f1=tv,_tmp61F.f2=oldkb;_tmp61F;});}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp14C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp621;_tmp621.tag=0U,({struct _fat_ptr _tmp70F=({const char*_tmp14E="swap_kind: cannot update the kind of ";_tag_fat(_tmp14E,sizeof(char),38U);});_tmp621.f1=_tmp70F;});_tmp621;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp14D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp620;_tmp620.tag=2U,_tmp620.f1=(void*)t;_tmp620;});void*_tmp14B[2U];_tmp14B[0]=& _tmp14C,_tmp14B[1]=& _tmp14D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp14B,sizeof(void*),2U));});}_LL0:;}
# 591
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 593
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 597
struct _RegionHandle _tmp14F=_new_region("temp");struct _RegionHandle*temp=& _tmp14F;_push_region(temp);
{struct Cyc_List_List*_tmp150=0;struct Cyc_List_List*inst=_tmp150;
# 600
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp151=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp151;
void*_tmp152=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp152;
struct Cyc_Absyn_Kind*_tmp153=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp1E=_tmp153;struct Cyc_Absyn_Kind*_tmp154=_stmttmp1E;switch(((struct Cyc_Absyn_Kind*)_tmp154)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 606
 inst=({struct Cyc_List_List*_tmp156=_region_malloc(temp,sizeof(*_tmp156));({struct _tuple19*_tmp710=({struct _tuple19*_tmp155=_region_malloc(temp,sizeof(*_tmp155));_tmp155->f1=tv,_tmp155->f2=t;_tmp155;});_tmp156->hd=_tmp710;}),_tmp156->tl=inst;_tmp156;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 610
if(inst != 0){
field_type=({struct _RegionHandle*_tmp712=temp;struct Cyc_List_List*_tmp711=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp712,_tmp711,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 598
;_pop_region();}
# 615
return k;}
# 622
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 624
void*_tmp157=Cyc_Tcutil_compress(t);void*_stmttmp1F=_tmp157;void*_tmp158=_stmttmp1F;struct Cyc_Absyn_Typedefdecl*_tmp159;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_PtrInfo _tmp15B;struct Cyc_List_List*_tmp15D;void*_tmp15C;struct Cyc_Absyn_Tvar*_tmp15E;struct Cyc_Core_Opt*_tmp15F;switch(*((int*)_tmp158)){case 1U: _LL1: _tmp15F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp158)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp15F;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp15E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp158)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp15E;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp15C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp158)->f1;_tmp15D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp158)->f2;_LL6: {void*c=_tmp15C;struct Cyc_List_List*ts=_tmp15D;
# 628
void*_tmp160=c;int _tmp164;struct Cyc_Absyn_AggrdeclImpl*_tmp163;struct Cyc_List_List*_tmp162;enum Cyc_Absyn_AggrKind _tmp161;struct Cyc_Absyn_Kind*_tmp165;enum Cyc_Absyn_Size_of _tmp166;switch(*((int*)_tmp160)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp166=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp160)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp166;
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp165=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp160)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp165;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 650
({struct Cyc_Warn_String_Warn_Warg_struct _tmp168=({struct Cyc_Warn_String_Warn_Warg_struct _tmp622;_tmp622.tag=0U,({struct _fat_ptr _tmp713=({const char*_tmp169="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp169,sizeof(char),40U);});_tmp622.f1=_tmp713;});_tmp622;});void*_tmp167[1U];_tmp167[0]=& _tmp168;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp167,sizeof(void*),1U));});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 653
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp161=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownAggr).val)->kind;_tmp162=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownAggr).val)->tvs;_tmp163=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownAggr).val)->impl;_tmp164=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp161;struct Cyc_List_List*tvs=_tmp162;struct Cyc_Absyn_AggrdeclImpl*i=_tmp163;int expected_mem_kind=_tmp164;
# 655
if(i == 0)
return expected_mem_kind?& Cyc_Tcutil_mk:& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp16A=i->fields;struct Cyc_List_List*fields=_tmp16A;
if(fields == 0)return& Cyc_Tcutil_mk;
# 660
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){
;}{
void*_tmp16B=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp16B;
struct Cyc_Absyn_Kind*_tmp16C=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp16C;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 669
for(0;fields != 0;fields=fields->tl){
void*_tmp16D=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp16D;
struct Cyc_Absyn_Kind*_tmp16E=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp16E;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 674
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 676
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp15B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp158)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp15B;
# 679
void*_tmp16F=Cyc_Tcutil_compress((pinfo.ptr_atts).bounds);void*_stmttmp20=_tmp16F;void*_tmp170=_stmttmp20;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)){case 13U: _LL4D: _LL4E: {
# 681
enum Cyc_Absyn_AliasQual _tmp171=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp21=_tmp171;enum Cyc_Absyn_AliasQual _tmp172=_stmttmp21;switch(_tmp172){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 687
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 689
enum Cyc_Absyn_AliasQual _tmp173=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp22=_tmp173;enum Cyc_Absyn_AliasQual _tmp174=_stmttmp22;switch(_tmp174){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 695
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 699
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp15A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp15A;
# 701
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp159=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp158)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp159;
# 705
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp176=({struct Cyc_Warn_String_Warn_Warg_struct _tmp624;_tmp624.tag=0U,({struct _fat_ptr _tmp714=({const char*_tmp178="type_kind: typedef found: ";_tag_fat(_tmp178,sizeof(char),27U);});_tmp624.f1=_tmp714;});_tmp624;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp177=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp623;_tmp623.tag=2U,_tmp623.f1=(void*)t;_tmp623;});void*_tmp175[2U];_tmp175[0]=& _tmp176,_tmp175[1]=& _tmp177;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp175,sizeof(void*),2U));});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp158)->f1)->r)){case 1U: _LL17: _LL18:
 return& Cyc_Tcutil_bk;case 0U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 714
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 719
static void*Cyc_Tcutil_rgns_of(void*t);
# 721
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 725
static struct _tuple19*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp179=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp23=_tmp179;struct Cyc_Absyn_Kind*_tmp17A=_stmttmp23;switch(((struct Cyc_Absyn_Kind*)_tmp17A)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp17A)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->tag=9U,({struct Cyc_Absyn_Exp*_tmp715=Cyc_Absyn_uint_exp(0U,0U);_tmp17B->f1=_tmp715;});_tmp17B;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 736
return({struct _tuple19*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->f1=tv,_tmp17C->f2=t;_tmp17C;});}
# 743
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp17D=Cyc_Tcutil_compress(t);void*_stmttmp24=_tmp17D;void*_tmp17E=_stmttmp24;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_VarargInfo*_tmp186;struct Cyc_List_List*_tmp185;void*_tmp184;struct Cyc_Absyn_Tqual _tmp183;void*_tmp182;struct Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp188;void*_tmp189;void*_tmp18B;void*_tmp18A;struct Cyc_List_List*_tmp18C;switch(*((int*)_tmp17E)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17E)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17E)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp18C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17E)->f2;_LL6: {struct Cyc_List_List*ts=_tmp18C;
# 748
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 752
struct Cyc_Absyn_Kind*_tmp18D=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp25=_tmp18D;struct Cyc_Absyn_Kind*_tmp18E=_stmttmp25;switch(((struct Cyc_Absyn_Kind*)_tmp18E)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp18A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17E)->f1).elt_type;_tmp18B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17E)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp18A;void*r=_tmp18B;
# 760
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp18F[2U];({void*_tmp717=Cyc_Absyn_access_eff(r);_tmp18F[0]=_tmp717;}),({void*_tmp716=Cyc_Tcutil_rgns_of(et);_tmp18F[1]=_tmp716;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp18F,sizeof(void*),2U));})));}case 4U: _LLD: _tmp189=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17E)->f1).elt_type;_LLE: {void*et=_tmp189;
# 762
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp188=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp17E)->f2;_LL10: {struct Cyc_List_List*fs=_tmp188;
# 764
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).tvars;_tmp182=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).effect;_tmp183=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).ret_tqual;_tmp184=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).ret_type;_tmp185=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).args;_tmp186=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).cyc_varargs;_tmp187=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp181;void*eff=_tmp182;struct Cyc_Absyn_Tqual rt_tq=_tmp183;void*rt=_tmp184;struct Cyc_List_List*args=_tmp185;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp186;struct Cyc_List_List*rpo=_tmp187;
# 773
void*_tmp190=({struct Cyc_List_List*_tmp718=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp718,(void*)_check_null(eff));});void*e=_tmp190;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp180=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp17E)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp180;
# 776
struct Cyc_List_List*_tmp191=0;struct Cyc_List_List*ts=_tmp191;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp192->tl=ts;_tmp192;});}
_tmp17F=ts;goto _LL16;}case 8U: _LL15: _tmp17F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17E)->f2;_LL16: {struct Cyc_List_List*ts=_tmp17F;
# 781
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp194=({struct Cyc_Warn_String_Warn_Warg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp719=({const char*_tmp195="typedecl in rgns_of";_tag_fat(_tmp195,sizeof(char),20U);});_tmp625.f1=_tmp719;});_tmp625;});void*_tmp193[1U];_tmp193[0]=& _tmp194;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp193,sizeof(void*),1U));});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 791
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp196=e;void*_tmp197;struct Cyc_List_List**_tmp198;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->f1)){case 9U: _LL1: _tmp198=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->f2;_LL2: {struct Cyc_List_List**es=_tmp198;
# 795
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp199=(void*)effs->hd;void*eff=_tmp199;
({void*_tmp71A=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp71A;});{
void*_tmp19A=(void*)effs->hd;void*_stmttmp26=_tmp19A;void*_tmp19B=_stmttmp26;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->tl == 0){_LLE: _LLF:
# 804
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 808
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp19C=Cyc_Tcutil_compress((void*)effs->hd);void*_stmttmp27=_tmp19C;void*_tmp19D=_stmttmp27;void*_tmp19E;struct Cyc_List_List*_tmp19F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f1)){case 9U: _LL13: _tmp19F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp19F;
# 813
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);
goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp19E=_tmp19D;_LL1C: {void*e=_tmp19E;
effects=({struct Cyc_List_List*_tmp1A0=_cycalloc(sizeof(*_tmp1A0));_tmp1A0->hd=e,_tmp1A0->tl=effects;_tmp1A0;});goto _LL12;}}_LL12:;}}
# 821
({struct Cyc_List_List*_tmp71B=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp71B;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->f2 != 0){_LL3: _tmp197=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp196)->f2)->hd;_LL4: {void*t=_tmp197;
# 824
void*_tmp1A1=Cyc_Tcutil_compress(t);void*_stmttmp28=_tmp1A1;void*_tmp1A2=_stmttmp28;switch(*((int*)_tmp1A2)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 829
 return e;}_LL0:;}}
# 834
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A3=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A4=_cycalloc(sizeof(*_tmp1A4));_tmp1A4->tag=5U,(_tmp1A4->f1).tvars=0,(_tmp1A4->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp71C=Cyc_Absyn_empty_tqual(0U);(_tmp1A4->f1).ret_tqual=_tmp71C;}),(_tmp1A4->f1).ret_type=Cyc_Absyn_void_type,(_tmp1A4->f1).args=0,(_tmp1A4->f1).c_varargs=0,(_tmp1A4->f1).cyc_varargs=0,(_tmp1A4->f1).rgn_po=0,(_tmp1A4->f1).attributes=0,(_tmp1A4->f1).requires_clause=0,(_tmp1A4->f1).requires_relns=0,(_tmp1A4->f1).ensures_clause=0,(_tmp1A4->f1).ensures_relns=0,(_tmp1A4->f1).return_value=0;_tmp1A4;});
# 835
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp1A3;
# 845
return({void*_tmp720=(void*)fntype;void*_tmp71F=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp71E=Cyc_Absyn_empty_tqual(0U);void*_tmp71D=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp720,_tmp71F,_tmp71E,_tmp71D,Cyc_Absyn_false_type);});}
# 852
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp1A5=Cyc_Tcutil_compress(e);void*_stmttmp29=_tmp1A5;void*_tmp1A6=_stmttmp29;struct Cyc_Core_Opt*_tmp1A9;void**_tmp1A8;struct Cyc_Core_Opt*_tmp1A7;void*_tmp1AA;struct Cyc_List_List*_tmp1AB;void*_tmp1AC;switch(*((int*)_tmp1A6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2 != 0){_LL1: _tmp1AC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2)->hd;_LL2: {void*r2=_tmp1AC;
# 859
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Tcutil_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp1AD=({struct _tuple15 _tmp626;_tmp626.f1=r,_tmp626.f2=r2;_tmp626;});struct _tuple15 _stmttmp2A=_tmp1AD;struct _tuple15 _tmp1AE=_stmttmp2A;struct Cyc_Absyn_Tvar*_tmp1B0;struct Cyc_Absyn_Tvar*_tmp1AF;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AE.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AE.f2)->tag == 2U){_LLC: _tmp1AF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AE.f1)->f1;_tmp1B0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AE.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1AF;struct Cyc_Absyn_Tvar*tv2=_tmp1B0;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1AB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2;_LL4: {struct Cyc_List_List*es=_tmp1AB;
# 867
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2 != 0){_LL5: _tmp1AA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A6)->f2)->hd;_LL6: {void*t=_tmp1AA;
# 871
void*_tmp1B1=Cyc_Tcutil_rgns_of(t);void*_stmttmp2B=_tmp1B1;void*_tmp1B2=_stmttmp2B;void*_tmp1B3;void*_tmp1B4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B2)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B2)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B2)->f2 != 0){_LL11: _tmp1B4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B2)->f2)->hd;_LL12: {void*t=_tmp1B4;
# 873
if(!constrain)return 0;{
void*_tmp1B5=Cyc_Tcutil_compress(t);void*_stmttmp2C=_tmp1B5;void*_tmp1B6=_stmttmp2C;struct Cyc_Core_Opt*_tmp1B9;void**_tmp1B8;struct Cyc_Core_Opt*_tmp1B7;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B6)->tag == 1U){_LL16: _tmp1B7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B6)->f1;_tmp1B8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B6)->f2;_tmp1B9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B6)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1B7;void**p=_tmp1B8;struct Cyc_Core_Opt*s=_tmp1B9;
# 878
void*_tmp1BA=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1BA;
# 881
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp722=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1BB[2U];_tmp1BB[0]=ev,({void*_tmp721=Cyc_Absyn_access_eff(r);_tmp1BB[1]=_tmp721;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BB,sizeof(void*),2U));})));*p=_tmp722;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1B3=_tmp1B2;_LL14: {void*e2=_tmp1B3;
# 886
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1A7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A6)->f1;_tmp1A8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A6)->f2;_tmp1A9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A6)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1A7;void**p=_tmp1A8;struct Cyc_Core_Opt*s=_tmp1A9;
# 889
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp627;_tmp627.tag=0U,({struct _fat_ptr _tmp723=({const char*_tmp1BE="effect evar has wrong kind";_tag_fat(_tmp1BE,sizeof(char),27U);});_tmp627.f1=_tmp723;});_tmp627;});void*_tmp1BC[1U];_tmp1BC[0]=& _tmp1BD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1BC,sizeof(void*),1U));});
if(!constrain)return 0;{
# 894
void*_tmp1BF=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1BF;
# 897
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp725=Cyc_Absyn_join_eff(({void*_tmp1C0[2U];_tmp1C0[0]=ev,({void*_tmp724=Cyc_Absyn_access_eff(r);_tmp1C0[1]=_tmp724;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C0,sizeof(void*),2U));}));*p=_tmp725;});
return 1;}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 907
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1C1=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_stmttmp2D=_tmp1C1;void*_tmp1C2=_stmttmp2D;struct Cyc_Core_Opt*_tmp1C5;void**_tmp1C4;struct Cyc_Core_Opt*_tmp1C3;void*_tmp1C6;struct Cyc_List_List*_tmp1C7;switch(*((int*)_tmp1C2)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C2)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1C7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C2)->f2;_LL4: {struct Cyc_List_List*es=_tmp1C7;
# 912
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C2)->f2 != 0){_LL5: _tmp1C6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C2)->f2)->hd;_LL6: {void*t2=_tmp1C6;
# 917
t2=Cyc_Tcutil_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1C8=Cyc_Tcutil_rgns_of(t);void*_stmttmp2E=_tmp1C8;void*_tmp1C9=_stmttmp2E;void*_tmp1CA;void*_tmp1CB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f2 != 0){_LLC: _tmp1CB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f2)->hd;_LLD: {void*t3=_tmp1CB;
# 922
struct _tuple15 _tmp1CC=({struct _tuple15 _tmp628;({void*_tmp726=Cyc_Tcutil_compress(t3);_tmp628.f1=_tmp726;}),_tmp628.f2=t2;_tmp628;});struct _tuple15 _stmttmp2F=_tmp1CC;struct _tuple15 _tmp1CD=_stmttmp2F;struct Cyc_Absyn_Tvar*_tmp1CF;struct Cyc_Absyn_Tvar*_tmp1CE;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CD.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CD.f2)->tag == 2U){_LL11: _tmp1CE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CD.f1)->f1;_tmp1CF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CD.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1CE;struct Cyc_Absyn_Tvar*tv2=_tmp1CF;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1CA=_tmp1C9;_LLF: {void*e2=_tmp1CA;
# 926
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1C3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C2)->f1;_tmp1C4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C2)->f2;_tmp1C5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C2)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1C3;void**p=_tmp1C4;struct Cyc_Core_Opt*s=_tmp1C5;
# 929
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp629;_tmp629.tag=0U,({struct _fat_ptr _tmp727=({const char*_tmp1D2="effect evar has wrong kind";_tag_fat(_tmp1D2,sizeof(char),27U);});_tmp629.f1=_tmp727;});_tmp629;});void*_tmp1D0[1U];_tmp1D0[0]=& _tmp1D1;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1D0,sizeof(void*),1U));});
if(!may_constrain_evars)return 0;{
# 934
void*_tmp1D3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1D3;
# 937
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1D4=Cyc_Absyn_join_eff(({void*_tmp1D5[2U];_tmp1D5[0]=ev,({void*_tmp728=Cyc_Absyn_regionsof_eff(t);_tmp1D5[1]=_tmp728;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D5,sizeof(void*),2U));}));void*new_typ=_tmp1D4;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 947
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1D6=e;struct Cyc_Core_Opt*_tmp1D9;void**_tmp1D8;struct Cyc_Core_Opt*_tmp1D7;void*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Tvar*_tmp1DC;switch(*((int*)_tmp1D6)){case 2U: _LL1: _tmp1DC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D6)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1DC;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D6)->f1)){case 9U: _LL3: _tmp1DB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D6)->f2;_LL4: {struct Cyc_List_List*es=_tmp1DB;
# 952
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D6)->f2 != 0){_LL5: _tmp1DA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D6)->f2)->hd;_LL6: {void*t=_tmp1DA;
# 957
void*_tmp1DD=Cyc_Tcutil_rgns_of(t);void*_stmttmp30=_tmp1DD;void*_tmp1DE=_stmttmp30;void*_tmp1DF;void*_tmp1E0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->f2 != 0){_LLC: _tmp1E0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->f2)->hd;_LLD: {void*t2=_tmp1E0;
# 959
if(!may_constrain_evars)return 0;{
void*_tmp1E1=Cyc_Tcutil_compress(t2);void*_stmttmp31=_tmp1E1;void*_tmp1E2=_stmttmp31;struct Cyc_Core_Opt*_tmp1E5;void**_tmp1E4;struct Cyc_Core_Opt*_tmp1E3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->tag == 1U){_LL11: _tmp1E3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->f1;_tmp1E4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->f2;_tmp1E5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1E3;void**p=_tmp1E4;struct Cyc_Core_Opt*s=_tmp1E5;
# 965
void*_tmp1E6=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E6;
# 967
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp72A=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1E7[2U];_tmp1E7[0]=ev,({void*_tmp729=Cyc_Absyn_var_type(v);_tmp1E7[1]=_tmp729;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E7,sizeof(void*),2U));})));*p=_tmp72A;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1DF=_tmp1DE;_LLF: {void*e2=_tmp1DF;
# 972
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1D7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D6)->f1;_tmp1D8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D6)->f2;_tmp1D9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D6)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1D7;void**p=_tmp1D8;struct Cyc_Core_Opt*s=_tmp1D9;
# 975
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62A;_tmp62A.tag=0U,({struct _fat_ptr _tmp72B=({const char*_tmp1EA="effect evar has wrong kind";_tag_fat(_tmp1EA,sizeof(char),27U);});_tmp62A.f1=_tmp72B;});_tmp62A;});void*_tmp1E8[1U];_tmp1E8[0]=& _tmp1E9;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1E8,sizeof(void*),1U));});{
# 979
void*_tmp1EB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1EB;
# 981
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1EC=Cyc_Absyn_join_eff(({void*_tmp1ED[2U];_tmp1ED[0]=ev,({void*_tmp72C=Cyc_Absyn_var_type(v);_tmp1ED[1]=_tmp72C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1ED,sizeof(void*),2U));}));void*new_typ=_tmp1EC;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 991
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1EE=e;void*_tmp1EF;struct Cyc_List_List*_tmp1F0;switch(*((int*)_tmp1EE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f1)){case 9U: _LL1: _tmp1F0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2;_LL2: {struct Cyc_List_List*es=_tmp1F0;
# 995
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2 != 0){_LL3: _tmp1EF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2)->hd;_LL4: {void*t=_tmp1EF;
# 1000
void*_tmp1F1=Cyc_Tcutil_rgns_of(t);void*_stmttmp32=_tmp1F1;void*_tmp1F2=_stmttmp32;void*_tmp1F3;void*_tmp1F4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2 != 0){_LLA: _tmp1F4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2)->hd;_LLB: {void*t2=_tmp1F4;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1F3=_tmp1F2;_LLD: {void*e2=_tmp1F3;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1004
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 1018 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1021
void*_tmp1F5=Cyc_Tcutil_compress(e1);void*_stmttmp33=_tmp1F5;void*_tmp1F6=_stmttmp33;struct Cyc_Core_Opt*_tmp1F8;void**_tmp1F7;struct Cyc_Absyn_Tvar*_tmp1F9;void*_tmp1FA;void*_tmp1FB;struct Cyc_List_List*_tmp1FC;switch(*((int*)_tmp1F6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f1)){case 9U: _LL1: _tmp1FC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f2;_LL2: {struct Cyc_List_List*es=_tmp1FC;
# 1023
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f2 != 0){_LL3: _tmp1FB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f2)->hd;_LL4: {void*r=_tmp1FB;
# 1034
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f2 != 0){_LL7: _tmp1FA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f2)->hd;_LL8: {void*t=_tmp1FA;
# 1038
void*_tmp1FD=Cyc_Tcutil_rgns_of(t);void*_stmttmp34=_tmp1FD;void*_tmp1FE=_stmttmp34;void*_tmp1FF;void*_tmp200;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FE)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FE)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FE)->f2 != 0){_LLE: _tmp200=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FE)->f2)->hd;_LLF: {void*t2=_tmp200;
# 1043
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1FF=_tmp1FE;_LL11: {void*e=_tmp1FF;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1F9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F6)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1F9;
# 1036
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1F7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F6)->f2;_tmp1F8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F6)->f4;_LLA: {void**p=_tmp1F7;struct Cyc_Core_Opt*s=_tmp1F8;
# 1048
if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1052
*p=Cyc_Absyn_empty_effect;
# 1055
return 1;}else{
# 1057
return 0;}}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp202=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62C;_tmp62C.tag=0U,({struct _fat_ptr _tmp72D=({const char*_tmp204="subset_effect: bad effect: ";_tag_fat(_tmp204,sizeof(char),28U);});_tmp62C.f1=_tmp72D;});_tmp62C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp203=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp62B;_tmp62B.tag=2U,_tmp62B.f1=(void*)e1;_tmp62B;});void*_tmp201[2U];_tmp201[0]=& _tmp202,_tmp201[1]=& _tmp203;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp201,sizeof(void*),2U));});}_LL0:;}
# 1068
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1070
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp205=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp35=_tmp205;struct _tuple15*_tmp206=_stmttmp35;void*_tmp208;void*_tmp207;_LL1: _tmp207=_tmp206->f1;_tmp208=_tmp206->f2;_LL2: {void*t1a=_tmp207;void*t1b=_tmp208;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp209=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp36=_tmp209;struct _tuple15*_tmp20A=_stmttmp36;void*_tmp20C;void*_tmp20B;_LL4: _tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;_LL5: {void*t2a=_tmp20B;void*t2b=_tmp20C;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1080
if(!found)return 0;}}}
# 1082
return 1;}
# 1086
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1088
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1091
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp20D=t;switch(*((int*)_tmp20D)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20D)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20D)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1097
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1100
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1103
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1106
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20D)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1095
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1098
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1101
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1104
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1107
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20D)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1096
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1099
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1102
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1105
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1108
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1130
({struct Cyc_Warn_String_Warn_Warg_struct _tmp20F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62D;_tmp62D.tag=0U,({struct _fat_ptr _tmp72E=({const char*_tmp210="bad con";_tag_fat(_tmp210,sizeof(char),8U);});_tmp62D.f1=_tmp72E;});_tmp62D;});void*_tmp20E[1U];_tmp20E[0]=& _tmp20F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp20E,sizeof(void*),1U));});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp20D)->f1){case 0U: _LL21: _LL22:
# 1109
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
# 1133
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp211=t;void*_tmp212;switch(*((int*)_tmp211)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp212=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp211)->f1;_LL18: {void*c=_tmp212;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1149
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp213=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp213;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1154
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp214=i;struct _tuple1*_tmp215;struct Cyc_Absyn_Datatypedecl*_tmp216;if((_tmp214.KnownDatatype).tag == 2){_LL1: _tmp216=*(_tmp214.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp216;
return dd->name;}}else{_LL3: _tmp215=((_tmp214.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp215;
return n;}}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1160
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp217=i;struct _tuple1*_tmp219;struct _tuple1*_tmp218;struct Cyc_Absyn_Datatypefield*_tmp21B;struct Cyc_Absyn_Datatypedecl*_tmp21A;if((_tmp217.KnownDatatypefield).tag == 2){_LL1: _tmp21A=((_tmp217.KnownDatatypefield).val).f1;_tmp21B=((_tmp217.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp21A;struct Cyc_Absyn_Datatypefield*df=_tmp21B;
# 1163
return({struct _tuple20 _tmp62E;_tmp62E.f1=dd->name,_tmp62E.f2=df->name;_tmp62E;});}}else{_LL3: _tmp218=((_tmp217.UnknownDatatypefield).val).datatype_name;_tmp219=((_tmp217.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp218;struct _tuple1*f=_tmp219;
# 1165
return({struct _tuple20 _tmp62F;_tmp62F.f1=d,_tmp62F.f2=f;_tmp62F;});}}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1168
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp21C=i;struct Cyc_Absyn_Aggrdecl*_tmp21D;struct _tuple1*_tmp21F;enum Cyc_Absyn_AggrKind _tmp21E;if((_tmp21C.UnknownAggr).tag == 1){_LL1: _tmp21E=((_tmp21C.UnknownAggr).val).f1;_tmp21F=((_tmp21C.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp21E;struct _tuple1*n=_tmp21F;
return({struct _tuple21 _tmp630;_tmp630.f1=k,_tmp630.f2=n;_tmp630;});}}else{_LL3: _tmp21D=*(_tmp21C.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp21D;
return({struct _tuple21 _tmp631;_tmp631.f1=ad->kind,_tmp631.f2=ad->name;_tmp631;});}}_LL0:;}
# 1174
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1180
struct _tuple15 _tmp220=({struct _tuple15 _tmp632;_tmp632.f1=t1,_tmp632.f2=t2;_tmp632;});struct _tuple15 _stmttmp37=_tmp220;struct _tuple15 _tmp221=_stmttmp37;union Cyc_Absyn_AggrInfo _tmp223;union Cyc_Absyn_AggrInfo _tmp222;union Cyc_Absyn_DatatypeFieldInfo _tmp225;union Cyc_Absyn_DatatypeFieldInfo _tmp224;union Cyc_Absyn_DatatypeInfo _tmp227;union Cyc_Absyn_DatatypeInfo _tmp226;struct Cyc_List_List*_tmp229;struct Cyc_List_List*_tmp228;struct _fat_ptr _tmp22B;struct _fat_ptr _tmp22A;struct _tuple1*_tmp22D;struct _tuple1*_tmp22C;switch(*((int*)_tmp221.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp221.f2)->tag == 15U){_LL1: _tmp22C=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp221.f1)->f1;_tmp22D=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp221.f2)->f1;_LL2: {struct _tuple1*n1=_tmp22C;struct _tuple1*n2=_tmp22D;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp221.f2)->tag == 17U){_LL3: _tmp22A=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp221.f1)->f1;_tmp22B=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp221.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp22A;struct _fat_ptr s2=_tmp22B;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp221.f2)->tag == 16U){_LL5: _tmp228=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp221.f1)->f1;_tmp229=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp221.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp228;struct Cyc_List_List*fs2=_tmp229;
# 1184
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp221.f2)->tag == 18U){_LL7: _tmp226=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp221.f1)->f1;_tmp227=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp221.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp226;union Cyc_Absyn_DatatypeInfo info2=_tmp227;
# 1186
return({struct _tuple1*_tmp72F=Cyc_Tcutil_get_datatype_qvar(info1);Cyc_Absyn_qvar_cmp(_tmp72F,Cyc_Tcutil_get_datatype_qvar(info2));});}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp221.f2)->tag == 19U){_LL9: _tmp224=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp221.f1)->f1;_tmp225=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp221.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp224;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp225;
# 1188
struct _tuple20 _tmp22E=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple20 _stmttmp38=_tmp22E;struct _tuple20 _tmp22F=_stmttmp38;struct _tuple1*_tmp231;struct _tuple1*_tmp230;_LL10: _tmp230=_tmp22F.f1;_tmp231=_tmp22F.f2;_LL11: {struct _tuple1*d1=_tmp230;struct _tuple1*f1=_tmp231;
struct _tuple20 _tmp232=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple20 _stmttmp39=_tmp232;struct _tuple20 _tmp233=_stmttmp39;struct _tuple1*_tmp235;struct _tuple1*_tmp234;_LL13: _tmp234=_tmp233.f1;_tmp235=_tmp233.f2;_LL14: {struct _tuple1*d2=_tmp234;struct _tuple1*f2=_tmp235;
int _tmp236=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp236;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp221.f2)->tag == 20U){_LLB: _tmp222=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp221.f1)->f1;_tmp223=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp221.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp222;union Cyc_Absyn_AggrInfo info2=_tmp223;
# 1194
struct _tuple21 _tmp237=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple21 _stmttmp3A=_tmp237;struct _tuple21 _tmp238=_stmttmp3A;struct _tuple1*_tmp23A;enum Cyc_Absyn_AggrKind _tmp239;_LL16: _tmp239=_tmp238.f1;_tmp23A=_tmp238.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp239;struct _tuple1*q1=_tmp23A;
struct _tuple21 _tmp23B=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple21 _stmttmp3B=_tmp23B;struct _tuple21 _tmp23C=_stmttmp3B;struct _tuple1*_tmp23E;enum Cyc_Absyn_AggrKind _tmp23D;_LL19: _tmp23D=_tmp23C.f1;_tmp23E=_tmp23C.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp23D;struct _tuple1*q2=_tmp23E;
int _tmp23F=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp23F;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1203
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp731)(void*,void*)=cmp;void*_tmp730=(void*)_check_null(a1);_tmp731(_tmp730,(void*)_check_null(a2));});}
# 1209
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp240=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp240;
int _tmp241=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp241;
return Cyc_Core_intcmp(i1,i2);}
# 1214
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp242=tqt1;void*_tmp244;struct Cyc_Absyn_Tqual _tmp243;_LL1: _tmp243=_tmp242->f1;_tmp244=_tmp242->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp243;void*t1=_tmp244;
struct _tuple12*_tmp245=tqt2;void*_tmp247;struct Cyc_Absyn_Tqual _tmp246;_LL4: _tmp246=_tmp245->f1;_tmp247=_tmp245->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp246;void*t2=_tmp247;
int _tmp248=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp248;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1222
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp249=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp249;
if(zsc != 0)return zsc;{
int _tmp24A=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp24A;
if(tqc != 0)return tqc;{
int _tmp24B=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp24B;
if(tc != 0)return tc;{
int _tmp24C=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Atts_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp24C;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1239
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp24D=({int _tmp732=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp732,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp24D;
if(shallowcmp != 0)
return shallowcmp;{
# 1248
struct _tuple15 _tmp24E=({struct _tuple15 _tmp637;_tmp637.f1=t2,_tmp637.f2=t1;_tmp637;});struct _tuple15 _stmttmp3C=_tmp24E;struct _tuple15 _tmp24F=_stmttmp3C;struct Cyc_Absyn_Exp*_tmp251;struct Cyc_Absyn_Exp*_tmp250;struct Cyc_Absyn_Exp*_tmp253;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_List_List*_tmp257;enum Cyc_Absyn_AggrKind _tmp256;struct Cyc_List_List*_tmp255;enum Cyc_Absyn_AggrKind _tmp254;struct Cyc_List_List*_tmp259;struct Cyc_List_List*_tmp258;struct Cyc_Absyn_FnInfo _tmp25B;struct Cyc_Absyn_FnInfo _tmp25A;void*_tmp263;struct Cyc_Absyn_Exp*_tmp262;struct Cyc_Absyn_Tqual _tmp261;void*_tmp260;void*_tmp25F;struct Cyc_Absyn_Exp*_tmp25E;struct Cyc_Absyn_Tqual _tmp25D;void*_tmp25C;void*_tmp26F;void*_tmp26E;void*_tmp26D;void*_tmp26C;struct Cyc_Absyn_Tqual _tmp26B;void*_tmp26A;void*_tmp269;void*_tmp268;void*_tmp267;void*_tmp266;struct Cyc_Absyn_Tqual _tmp265;void*_tmp264;struct Cyc_Absyn_Tvar*_tmp271;struct Cyc_Absyn_Tvar*_tmp270;struct Cyc_List_List*_tmp275;void*_tmp274;struct Cyc_List_List*_tmp273;void*_tmp272;switch(*((int*)_tmp24F.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24F.f2)->tag == 0U){_LL1: _tmp272=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24F.f1)->f1;_tmp273=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24F.f1)->f2;_tmp274=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24F.f2)->f1;_tmp275=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24F.f2)->f2;_LL2: {void*c1=_tmp272;struct Cyc_List_List*ts1=_tmp273;void*c2=_tmp274;struct Cyc_List_List*ts2=_tmp275;
# 1250
int _tmp276=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp276;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24F.f2)->tag == 1U){_LL3: _LL4:
# 1254
({struct Cyc_Warn_String_Warn_Warg_struct _tmp278=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,({struct _fat_ptr _tmp733=({const char*_tmp279="typecmp: can only compare closed types";_tag_fat(_tmp279,sizeof(char),39U);});_tmp633.f1=_tmp733;});_tmp633;});void*_tmp277[1U];_tmp277[0]=& _tmp278;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp277,sizeof(void*),1U));});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24F.f2)->tag == 2U){_LL5: _tmp270=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24F.f1)->f1;_tmp271=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24F.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp270;struct Cyc_Absyn_Tvar*tv1=_tmp271;
# 1258
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f2)->tag == 3U){_LL7: _tmp264=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f1)->f1).elt_type;_tmp265=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f1)->f1).elt_tq;_tmp266=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f1)->f1).ptr_atts).rgn;_tmp267=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f1)->f1).ptr_atts).nullable;_tmp268=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f1)->f1).ptr_atts).bounds;_tmp269=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f1)->f1).ptr_atts).zero_term;_tmp26A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f2)->f1).elt_type;_tmp26B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f2)->f1).elt_tq;_tmp26C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f2)->f1).ptr_atts).rgn;_tmp26D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f2)->f1).ptr_atts).nullable;_tmp26E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f2)->f1).ptr_atts).bounds;_tmp26F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24F.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp264;struct Cyc_Absyn_Tqual tqual2a=_tmp265;void*rgn2=_tmp266;void*null2a=_tmp267;void*b2=_tmp268;void*zt2=_tmp269;void*t1a=_tmp26A;struct Cyc_Absyn_Tqual tqual1a=_tmp26B;void*rgn1=_tmp26C;void*null1a=_tmp26D;void*b1=_tmp26E;void*zt1=_tmp26F;
# 1262
int _tmp27A=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp27A;
if(etc != 0)return etc;{
int _tmp27B=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp27B;
if(rc != 0)return rc;{
int _tmp27C=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp27C;
if(tqc != 0)return tqc;{
int _tmp27D=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp27D;
if(cc != 0)return cc;{
int _tmp27E=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp27E;
if(zc != 0)return zc;{
int _tmp27F=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp27F;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f2)->tag == 4U){_LL9: _tmp25C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f1)->f1).elt_type;_tmp25D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f1)->f1).tq;_tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f1)->f1).num_elts;_tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f1)->f1).zero_term;_tmp260=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f2)->f1).elt_type;_tmp261=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f2)->f1).tq;_tmp262=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f2)->f1).num_elts;_tmp263=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24F.f2)->f1).zero_term;_LLA: {void*t2a=_tmp25C;struct Cyc_Absyn_Tqual tq2a=_tmp25D;struct Cyc_Absyn_Exp*e1=_tmp25E;void*zt1=_tmp25F;void*t1a=_tmp260;struct Cyc_Absyn_Tqual tq1a=_tmp261;struct Cyc_Absyn_Exp*e2=_tmp262;void*zt2=_tmp263;
# 1278
int _tmp280=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp280;
if(tqc != 0)return tqc;{
int _tmp281=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp281;
if(tc != 0)return tc;{
int _tmp282=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp282;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp284=({struct Cyc_Warn_String_Warn_Warg_struct _tmp634;_tmp634.tag=0U,({struct _fat_ptr _tmp734=({const char*_tmp285="missing expression in array index";_tag_fat(_tmp285,sizeof(char),34U);});_tmp634.f1=_tmp734;});_tmp634;});void*_tmp283[1U];_tmp283[0]=& _tmp284;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp283,sizeof(void*),1U));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24F.f2)->tag == 5U){_LLB: _tmp25A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24F.f1)->f1;_tmp25B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24F.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp25A;struct Cyc_Absyn_FnInfo f2=_tmp25B;
# 1290
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp286=f1.args;struct Cyc_List_List*args1=_tmp286;
struct Cyc_List_List*_tmp287=f2.args;struct Cyc_List_List*args2=_tmp287;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp288=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3D=_tmp288;struct _tuple9 _tmp289=_stmttmp3D;void*_tmp28B;struct Cyc_Absyn_Tqual _tmp28A;_LL18: _tmp28A=_tmp289.f2;_tmp28B=_tmp289.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp28A;void*t1=_tmp28B;
struct _tuple9 _tmp28C=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp3E=_tmp28C;struct _tuple9 _tmp28D=_stmttmp3E;void*_tmp28F;struct Cyc_Absyn_Tqual _tmp28E;_LL1B: _tmp28E=_tmp28D.f2;_tmp28F=_tmp28D.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp28E;void*t2=_tmp28F;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1305
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp735=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp735,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1319
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp290=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp290;
struct Cyc_List_List*_tmp291=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp291;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp292=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp3F=_tmp292;struct _tuple15 _tmp293=_stmttmp3F;void*_tmp295;void*_tmp294;_LL1E: _tmp294=_tmp293.f1;_tmp295=_tmp293.f2;_LL1F: {void*t1a=_tmp294;void*t1b=_tmp295;
struct _tuple15 _tmp296=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp40=_tmp296;struct _tuple15 _tmp297=_stmttmp40;void*_tmp299;void*_tmp298;_LL21: _tmp298=_tmp297.f1;_tmp299=_tmp297.f2;_LL22: {void*t2a=_tmp298;void*t2b=_tmp299;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1329
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1337
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp736=({const char*_tmp29C="typecmp: function type comparison should never get here!";_tag_fat(_tmp29C,sizeof(char),57U);});_tmp635.f1=_tmp736;});_tmp635;});void*_tmp29A[1U];_tmp29A[0]=& _tmp29B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp29A,sizeof(void*),1U));});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24F.f2)->tag == 6U){_LLD: _tmp258=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24F.f1)->f1;_tmp259=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24F.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp258;struct Cyc_List_List*ts1=_tmp259;
# 1340
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24F.f2)->tag == 7U){_LLF: _tmp254=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24F.f1)->f1;_tmp255=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24F.f1)->f2;_tmp256=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24F.f2)->f1;_tmp257=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24F.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp254;struct Cyc_List_List*fs2=_tmp255;enum Cyc_Absyn_AggrKind k1=_tmp256;struct Cyc_List_List*fs1=_tmp257;
# 1343
if((int)k1 != (int)k2)
return(int)k1 == (int)0U?- 1: 1;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24F.f2)->tag == 9U){_LL11: _tmp252=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24F.f1)->f1;_tmp253=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24F.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp252;struct Cyc_Absyn_Exp*e2=_tmp253;
# 1347
_tmp250=e1;_tmp251=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24F.f2)->tag == 11U){_LL13: _tmp250=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24F.f1)->f1;_tmp251=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24F.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp250;struct Cyc_Absyn_Exp*e2=_tmp251;
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp29E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp636;_tmp636.tag=0U,({struct _fat_ptr _tmp737=({const char*_tmp29F="Unmatched case in typecmp";_tag_fat(_tmp29F,sizeof(char),26U);});_tmp636.f1=_tmp737;});_tmp636;});void*_tmp29D[1U];_tmp29D[0]=& _tmp29E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp29D,sizeof(void*),1U));});}_LL0:;}}}
# 1355
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp2A0=({struct _tuple15 _tmp639;({void*_tmp739=Cyc_Tcutil_compress(t1);_tmp639.f1=_tmp739;}),({void*_tmp738=Cyc_Tcutil_compress(t2);_tmp639.f2=_tmp738;});_tmp639;});struct _tuple15 _stmttmp41=_tmp2A0;struct _tuple15 _tmp2A1=_stmttmp41;void*_tmp2A3;void*_tmp2A2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1.f2)->tag == 0U){_LL1: _tmp2A2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1.f1)->f1;_tmp2A3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A1.f2)->f1;_LL2: {void*c1=_tmp2A2;void*c2=_tmp2A3;
# 1358
struct _tuple15 _tmp2A4=({struct _tuple15 _tmp638;_tmp638.f1=c1,_tmp638.f2=c2;_tmp638;});struct _tuple15 _stmttmp42=_tmp2A4;struct _tuple15 _tmp2A5=_stmttmp42;int _tmp2A7;int _tmp2A6;switch(*((int*)_tmp2A5.f1)){case 2U: switch(*((int*)_tmp2A5.f2)){case 2U: _LL6: _tmp2A6=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A5.f1)->f1;_tmp2A7=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1;_LL7: {int i1=_tmp2A6;int i2=_tmp2A7;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp2A5.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1366
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1371
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1374
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1 == 0){_LL14: _LL15:
# 1369
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp2A5.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1367
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1372
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1375
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f1 == 0){_LL16: _LL17:
# 1370
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1376
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A5.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1373
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1377
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1379
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1381
 return 0;}_LL0:;}
# 1385
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp2A8=({struct _tuple15 _tmp63B;({void*_tmp73B=Cyc_Tcutil_compress(t1);_tmp63B.f1=_tmp73B;}),({void*_tmp73A=Cyc_Tcutil_compress(t2);_tmp63B.f2=_tmp73A;});_tmp63B;});struct _tuple15 _stmttmp43=_tmp2A8;struct _tuple15 _tmp2A9=_stmttmp43;void*_tmp2AB;void*_tmp2AA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9.f2)->tag == 0U){_LL1: _tmp2AA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9.f1)->f1;_tmp2AB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A9.f2)->f1;_LL2: {void*c1=_tmp2AA;void*c2=_tmp2AB;
# 1388
{struct _tuple15 _tmp2AC=({struct _tuple15 _tmp63A;_tmp63A.f1=c1,_tmp63A.f2=c2;_tmp63A;});struct _tuple15 _stmttmp44=_tmp2AC;struct _tuple15 _tmp2AD=_stmttmp44;int _tmp2AF;int _tmp2AE;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AD.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AD.f2)->tag == 2U){_LL6: _tmp2AE=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f1;_tmp2AF=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f1;_LL7: {int i1=_tmp2AE;int i2=_tmp2AF;
# 1390
if(i1 != 0 && i1 != 1)return t1;
if(i2 != 0 && i2 != 1)return t2;
if(i1 >= i2)return t1;
return t2;}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AD.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1400
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2AD.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1405
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1398
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2AD.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1407
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1397
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1401
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2AD.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1406
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1399
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2AD.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AD.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1408
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2AD.f1)->tag == 4U){_LL18: _LL19:
# 1403
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2AD.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1409
 goto _LL5;}}}}}}_LL5:;}
# 1411
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1414
return Cyc_Absyn_sint_type;}
# 1419
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1422
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2B0=_cycalloc(sizeof(*_tmp2B0));_tmp2B0->v=t1;_tmp2B0;});}}}
# 1430
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1434
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63F;_tmp63F.tag=0U,({struct _fat_ptr _tmp73C=({const char*_tmp2B7="type mismatch: expecting ";_tag_fat(_tmp2B7,sizeof(char),26U);});_tmp63F.f1=_tmp73C;});_tmp63F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2B3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63E;_tmp63E.tag=2U,_tmp63E.f1=(void*)t;_tmp63E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2B4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D;_tmp63D.tag=0U,({struct _fat_ptr _tmp73D=({const char*_tmp2B6=" but found ";_tag_fat(_tmp2B6,sizeof(char),12U);});_tmp63D.f1=_tmp73D;});_tmp63D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2B5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63C;_tmp63C.tag=2U,_tmp63C.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt);_tmp63C;});void*_tmp2B1[4U];_tmp2B1[0]=& _tmp2B2,_tmp2B1[1]=& _tmp2B3,_tmp2B1[2]=& _tmp2B4,_tmp2B1[3]=& _tmp2B5;({unsigned _tmp73E=((struct Cyc_Absyn_Exp*)el->hd)->loc;Cyc_Warn_err2(_tmp73E,_tag_fat(_tmp2B1,sizeof(void*),4U));});});
# 1438
return 0;}}}
# 1440
return 1;}
# 1445
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2B8=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp45=_tmp2B8;void*_tmp2B9=_stmttmp45;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B9)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1451
return 1;}
# 1454
static int Cyc_Tcutil_coerce_Xint_type(void*Xint_type,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Xint_type))
return 1;
# 1458
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Xint_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp640;_tmp640.tag=0U,({struct _fat_ptr _tmp73F=({const char*_tmp2BC="integral size mismatch; conversion supplied";_tag_fat(_tmp2BC,sizeof(char),44U);});_tmp640.f1=_tmp73F;});_tmp640;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BB;({unsigned _tmp740=e->loc;Cyc_Warn_warn2(_tmp740,_tag_fat(_tmp2BA,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e,Xint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1464
return 0;}
# 1467
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_uint_type,e);}
# 1471
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_sint_type,e);}
# 1477
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1483
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp741=t;Cyc_Unify_unify(_tmp741,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1488
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2BD=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2BD;
void*_tmp2BE=t;struct Cyc_Absyn_Exp*_tmp2BF;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2BE)->tag == 9U){_LL1: _tmp2BF=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2BE)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2BF;
return e;}}else{_LL3: _LL4: {
# 1493
struct Cyc_Absyn_Exp*_tmp2C0=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2C0;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1502
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2C1=Cyc_Tcutil_compress(b);void*_stmttmp46=_tmp2C1;void*_tmp2C2=_stmttmp46;struct Cyc_List_List*_tmp2C3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2C3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C2)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2C3;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp642;_tmp642.tag=0U,({struct _fat_ptr _tmp742=({const char*_tmp2C7="get_bounds_exp: ";_tag_fat(_tmp2C7,sizeof(char),17U);});_tmp642.f1=_tmp742;});_tmp642;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp641;_tmp641.tag=2U,_tmp641.f1=(void*)b;_tmp641;});void*_tmp2C4[2U];_tmp2C4[0]=& _tmp2C5,_tmp2C4[1]=& _tmp2C6;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C4,sizeof(void*),2U));});}_LL0:;}}
# 1511
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2C8=Cyc_Tcutil_compress(t);void*_stmttmp47=_tmp2C8;void*_tmp2C9=_stmttmp47;void*_tmp2CA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C9)->tag == 3U){_LL1: _tmp2CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C9)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2CA;
# 1514
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0U,({struct _fat_ptr _tmp743=({const char*_tmp2CE="get_ptr_bounds_exp not pointer: ";_tag_fat(_tmp2CE,sizeof(char),33U);});_tmp644.f1=_tmp743;});_tmp644;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2CD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp643;_tmp643.tag=2U,_tmp643.f1=(void*)t;_tmp643;});void*_tmp2CB[2U];_tmp2CB[0]=& _tmp2CC,_tmp2CB[1]=& _tmp2CD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2CB,sizeof(void*),2U));});}_LL0:;}
# 1520
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2CF=_cycalloc(sizeof(*_tmp2CF));_tmp2CF->v=tvs;_tmp2CF;}));}
# 1524
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2D0=_cycalloc(sizeof(*_tmp2D0));_tmp2D0->v=tvs;_tmp2D0;}));}
# 1529
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1536
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp2D1=({struct _tuple15 _tmp64B;_tmp64B.f1=t1,_tmp64B.f2=t2;_tmp64B;});struct _tuple15 _stmttmp48=_tmp2D1;struct _tuple15 _tmp2D2=_stmttmp48;void*_tmp2DA;struct Cyc_Absyn_Exp*_tmp2D9;struct Cyc_Absyn_Tqual _tmp2D8;void*_tmp2D7;void*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Tqual _tmp2D4;void*_tmp2D3;struct Cyc_Absyn_PtrInfo _tmp2DC;struct Cyc_Absyn_PtrInfo _tmp2DB;switch(*((int*)_tmp2D2.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D2.f2)->tag == 3U){_LL1: _tmp2DB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D2.f1)->f1;_tmp2DC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D2.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2DB;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2DC;
# 1542
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable)&&
 Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable))
return 0;
# 1546
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple22 _tmp2DD=({struct _tuple22 _tmp646;({struct Cyc_Absyn_Exp*_tmp747=({void*_tmp746=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp746,(pinfo_a.ptr_atts).bounds);});_tmp646.f1=_tmp747;}),({
struct Cyc_Absyn_Exp*_tmp745=({void*_tmp744=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp744,(pinfo_b.ptr_atts).bounds);});_tmp646.f2=_tmp745;});_tmp646;});
# 1547
struct _tuple22 _stmttmp49=_tmp2DD;struct _tuple22 _tmp2DE=_stmttmp49;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Exp*_tmp2DF;if(_tmp2DE.f2 == 0){_LLA: _LLB:
# 1550
 goto _LL9;}else{if(_tmp2DE.f1 == 0){_LLC: _LLD:
# 1553
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp748=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp748,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
return 0;}else{_LLE: _tmp2DF=_tmp2DE.f1;_tmp2E0=_tmp2DE.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2DF;struct Cyc_Absyn_Exp*e2=_tmp2E0;
# 1561
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp645;_tmp645.tag=0U,({struct _fat_ptr _tmp749=({const char*_tmp2E3="implicit cast to shorter array";_tag_fat(_tmp2E3,sizeof(char),31U);});_tmp645.f1=_tmp749;});_tmp645;});void*_tmp2E1[1U];_tmp2E1[0]=& _tmp2E2;({unsigned _tmp74A=loc;Cyc_Warn_warn2(_tmp74A,_tag_fat(_tmp2E1,sizeof(void*),1U));});});
if(!({struct Cyc_Absyn_Exp*_tmp74B=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp74B,(struct Cyc_Absyn_Exp*)_check_null(e1));}))
return 0;
goto _LL9;}}}_LL9:;}
# 1569
if((pinfo_a.elt_tq).real_const && !(pinfo_b.elt_tq).real_const)
return 0;
# 1572
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(!Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn))
# 1575
return 0;
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64A;_tmp64A.tag=0U,({struct _fat_ptr _tmp74C=({const char*_tmp2EA="implicit cast from region ";_tag_fat(_tmp2EA,sizeof(char),27U);});_tmp64A.f1=_tmp74C;});_tmp64A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2E6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp649;_tmp649.tag=2U,_tmp649.f1=(void*)(pinfo_a.ptr_atts).rgn;_tmp649;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp648;_tmp648.tag=0U,({
struct _fat_ptr _tmp74D=({const char*_tmp2E9=" to region ";_tag_fat(_tmp2E9,sizeof(char),12U);});_tmp648.f1=_tmp74D;});_tmp648;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2E8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp647;_tmp647.tag=2U,_tmp647.f1=(void*)(pinfo_b.ptr_atts).rgn;_tmp647;});void*_tmp2E4[4U];_tmp2E4[0]=& _tmp2E5,_tmp2E4[1]=& _tmp2E6,_tmp2E4[2]=& _tmp2E7,_tmp2E4[3]=& _tmp2E8;({unsigned _tmp74E=loc;Cyc_Warn_warn2(_tmp74E,_tag_fat(_tmp2E4,sizeof(void*),4U));});});}
# 1581
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)&&(
!Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)|| !(pinfo_b.elt_tq).real_const))
# 1584
return 0;
# 1588
if(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type))
return 1;
# 1592
if(!({void*_tmp74F=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp74F,(pinfo_b.ptr_atts).bounds);})||
 Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
return 0;
if(!(pinfo_b.elt_tq).real_const && !Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type)))
return 0;
return Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type);}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f2)->tag == 4U){_LL3: _tmp2D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f1)->f1).elt_type;_tmp2D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f1)->f1).tq;_tmp2D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f1)->f1).num_elts;_tmp2D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f1)->f1).zero_term;_tmp2D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f2)->f1).elt_type;_tmp2D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f2)->f1).tq;_tmp2D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f2)->f1).num_elts;_tmp2DA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D2.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2D3;struct Cyc_Absyn_Tqual tq1a=_tmp2D4;struct Cyc_Absyn_Exp*e1=_tmp2D5;void*zt1=_tmp2D6;void*t2a=_tmp2D7;struct Cyc_Absyn_Tqual tq2a=_tmp2D8;struct Cyc_Absyn_Exp*e2=_tmp2D9;void*zt2=_tmp2DA;
# 1601
if(!Cyc_Unify_unify(zt1,zt2))return 0;
if(e1 == 0 || e2 == 0)return 0;
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))return 0;
if(!Cyc_Unify_unify(t1a,t2a))return 0;
# 1606
return !tq1a.real_const || tq2a.real_const;}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D2.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D2.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D2.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1608
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1610
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1614
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2EB=Cyc_Tcutil_compress(t);void*_stmttmp4A=_tmp2EB;void*_tmp2EC=_stmttmp4A;void*_tmp2ED;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EC)->tag == 3U){_LL1: _tmp2ED=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EC)->f1).elt_type;_LL2: {void*e=_tmp2ED;
return e;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64C;_tmp64C.tag=0U,({struct _fat_ptr _tmp750=({const char*_tmp2F0="pointer_elt_type";_tag_fat(_tmp2F0,sizeof(char),17U);});_tmp64C.f1=_tmp750;});_tmp64C;});void*_tmp2EE[1U];_tmp2EE[0]=& _tmp2EF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2EE,sizeof(void*),1U));});}_LL0:;}
# 1620
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2F1=Cyc_Tcutil_compress(t);void*_stmttmp4B=_tmp2F1;void*_tmp2F2=_stmttmp4B;struct Cyc_Absyn_PtrAtts*_tmp2F3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F2)->tag == 3U){_LL1: _tmp2F3=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F2)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2F3;
return p->rgn;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64D;_tmp64D.tag=0U,({struct _fat_ptr _tmp751=({const char*_tmp2F6="pointer_elt_type";_tag_fat(_tmp2F6,sizeof(char),17U);});_tmp64D.f1=_tmp751;});_tmp64D;});void*_tmp2F4[1U];_tmp2F4[0]=& _tmp2F5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2F4,sizeof(void*),1U));});}_LL0:;}
# 1627
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2F7=Cyc_Tcutil_compress(t);void*_stmttmp4C=_tmp2F7;void*_tmp2F8=_stmttmp4C;void*_tmp2F9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F8)->tag == 3U){_LL1: _tmp2F9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F8)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2F9;
*rgn=r;return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1637
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2FA=Cyc_Tcutil_compress(t);void*_stmttmp4D=_tmp2FA;void*_tmp2FB=_stmttmp4D;void*_tmp2FD;void*_tmp2FC;switch(*((int*)_tmp2FB)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2FB)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FB)->f1).ptr_atts).nullable;_tmp2FD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FB)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2FC;void*bounds=_tmp2FD;
# 1645
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1651
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2FE=e->r;void*_stmttmp4E=_tmp2FE;void*_tmp2FF=_stmttmp4E;struct Cyc_Absyn_Exp*_tmp301;void*_tmp300;struct _fat_ptr _tmp302;switch(*((int*)_tmp2FF)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp302=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp302;
# 1660
unsigned long _tmp303=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp303;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}
# 1670
return 0;}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1658
 return 1;case 14U: _LLD: _tmp300=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FF)->f1;_tmp301=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2FF)->f2;_LLE: {void*t=_tmp300;struct Cyc_Absyn_Exp*e2=_tmp301;
# 1671
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1676
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1685
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1690
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1695
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1704
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1709
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1714
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp304=ka;enum Cyc_Absyn_AliasQual _tmp306;enum Cyc_Absyn_KindQual _tmp305;_LL1: _tmp305=_tmp304->kind;_tmp306=_tmp304->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp305;enum Cyc_Absyn_AliasQual a=_tmp306;
{enum Cyc_Absyn_AliasQual _tmp307=a;switch(_tmp307){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1718
enum Cyc_Absyn_KindQual _tmp308=k;switch(_tmp308){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1729
{enum Cyc_Absyn_KindQual _tmp309=k;switch(_tmp309){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1736
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1738
{enum Cyc_Absyn_KindQual _tmp30A=k;switch(_tmp30A){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1745
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1748
({struct Cyc_Warn_String_Warn_Warg_struct _tmp30C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64F;_tmp64F.tag=0U,({struct _fat_ptr _tmp752=({const char*_tmp30E="kind_to_opt: bad kind ";_tag_fat(_tmp30E,sizeof(char),23U);});_tmp64F.f1=_tmp752;});_tmp64F;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp30D=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp64E;_tmp64E.tag=9U,_tmp64E.f1=ka;_tmp64E;});void*_tmp30B[2U];_tmp30B[0]=& _tmp30C,_tmp30B[1]=& _tmp30D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp30B,sizeof(void*),2U));});}}
# 1751
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->tag=0U,_tmp30F->f1=k;_tmp30F;});}
# 1754
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp310=_cycalloc(sizeof(*_tmp310));({void*_tmp753=Cyc_Tcutil_kind_to_bound(k);_tmp310->v=_tmp753;});_tmp310;});}
# 1760
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp311=Cyc_Tcutil_compress(t2);void*_stmttmp4F=_tmp311;void*_tmp312=_stmttmp4F;void*_tmp313;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->tag == 3U){_LL1: _tmp313=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp312)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp313;
# 1765
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp754=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp754;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1774
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1780
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1784
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp327=_cycalloc(sizeof(*_tmp327));_tmp327->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp757=({struct _fat_ptr*_tmp326=_cycalloc(sizeof(*_tmp326));({struct _fat_ptr _tmp756=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp325=({struct Cyc_Int_pa_PrintArg_struct _tmp651;_tmp651.tag=1U,_tmp651.f1=(unsigned long)counter ++;_tmp651;});void*_tmp323[1U];_tmp323[0]=& _tmp325;({struct _fat_ptr _tmp755=({const char*_tmp324="__aliasvar%d";_tag_fat(_tmp324,sizeof(char),13U);});Cyc_aprintf(_tmp755,_tag_fat(_tmp323,sizeof(void*),1U));});});*_tmp326=_tmp756;});_tmp326;});_tmp327->f2=_tmp757;});_tmp327;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp758=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp322=_cycalloc(sizeof(*_tmp322));_tmp322->tag=4U,_tmp322->f1=vd;_tmp322;});Cyc_Absyn_varb_exp(_tmp758,e->loc);});
# 1793
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp314=Cyc_Tcutil_compress(e_type);void*_stmttmp50=_tmp314;void*_tmp315=_stmttmp50;struct Cyc_Absyn_PtrLoc*_tmp31C;void*_tmp31B;void*_tmp31A;void*_tmp319;void*_tmp318;struct Cyc_Absyn_Tqual _tmp317;void*_tmp316;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->tag == 3U){_LL1: _tmp316=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).elt_type;_tmp317=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).elt_tq;_tmp318=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).ptr_atts).rgn;_tmp319=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).ptr_atts).nullable;_tmp31A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).ptr_atts).bounds;_tmp31B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).ptr_atts).zero_term;_tmp31C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp316;struct Cyc_Absyn_Tqual tq=_tmp317;void*old_r=_tmp318;void*nb=_tmp319;void*b=_tmp31A;void*zt=_tmp31B;struct Cyc_Absyn_PtrLoc*pl=_tmp31C;
# 1796
{void*_tmp31D=Cyc_Tcutil_compress(old_r);void*_stmttmp51=_tmp31D;void*_tmp31E=_stmttmp51;struct Cyc_Core_Opt*_tmp320;void**_tmp31F;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31E)->tag == 1U){_LL6: _tmp31F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31E)->f2;_tmp320=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp31E)->f4;_LL7: {void**topt=_tmp31F;struct Cyc_Core_Opt*ts=_tmp320;
# 1798
void*_tmp321=Cyc_Absyn_var_type(tv);void*new_r=_tmp321;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1803
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1806
e->topt=0;
vd->initializer=0;{
# 1810
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1812
return({struct _tuple23 _tmp650;_tmp650.f1=d,_tmp650.f2=ve;_tmp650;});}}
# 1817
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1820
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1825
void*_tmp328=Cyc_Tcutil_compress(wants_type);void*_stmttmp52=_tmp328;void*_tmp329=_stmttmp52;void*_tmp32A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp329)->tag == 3U){_LL1: _tmp32A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp329)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp32A;
# 1827
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp32B=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp32B;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1833
return 0;}
# 1837
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1839
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1843
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1846
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1848
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp32D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp656;_tmp656.tag=0U,({struct _fat_ptr _tmp759=({const char*_tmp334="integral size mismatch; ";_tag_fat(_tmp334,sizeof(char),25U);});_tmp656.f1=_tmp759;});_tmp656;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp32E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp655;_tmp655.tag=2U,_tmp655.f1=(void*)t1;_tmp655;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp654;_tmp654.tag=0U,({
struct _fat_ptr _tmp75A=({const char*_tmp333=" -> ";_tag_fat(_tmp333,sizeof(char),5U);});_tmp654.f1=_tmp75A;});_tmp654;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp330=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp653;_tmp653.tag=2U,_tmp653.f1=(void*)t2;_tmp653;});struct Cyc_Warn_String_Warn_Warg_struct _tmp331=({struct Cyc_Warn_String_Warn_Warg_struct _tmp652;_tmp652.tag=0U,({struct _fat_ptr _tmp75B=({const char*_tmp332=" conversion supplied";_tag_fat(_tmp332,sizeof(char),21U);});_tmp652.f1=_tmp75B;});_tmp652;});void*_tmp32C[5U];_tmp32C[0]=& _tmp32D,_tmp32C[1]=& _tmp32E,_tmp32C[2]=& _tmp32F,_tmp32C[3]=& _tmp330,_tmp32C[4]=& _tmp331;({unsigned _tmp75C=e->loc;Cyc_Warn_warn2(_tmp75C,_tag_fat(_tmp32C,sizeof(void*),5U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1856
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp336=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65C;_tmp65C.tag=0U,({struct _fat_ptr _tmp75D=({const char*_tmp33E="implicit alias coercion for ";_tag_fat(_tmp33E,sizeof(char),29U);});_tmp65C.f1=_tmp75D;});_tmp65C;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp337=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp65B;_tmp65B.tag=4U,_tmp65B.f1=e;_tmp65B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp338=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65A;_tmp65A.tag=0U,({struct _fat_ptr _tmp75E=({const char*_tmp33D=":";_tag_fat(_tmp33D,sizeof(char),2U);});_tmp65A.f1=_tmp75E;});_tmp65A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp339=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp659;_tmp659.tag=2U,_tmp659.f1=(void*)t1;_tmp659;});struct Cyc_Warn_String_Warn_Warg_struct _tmp33A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp658;_tmp658.tag=0U,({struct _fat_ptr _tmp75F=({const char*_tmp33C=" to ";_tag_fat(_tmp33C,sizeof(char),5U);});_tmp658.f1=_tmp75F;});_tmp658;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp33B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp657;_tmp657.tag=2U,_tmp657.f1=(void*)t2;_tmp657;});void*_tmp335[6U];_tmp335[0]=& _tmp336,_tmp335[1]=& _tmp337,_tmp335[2]=& _tmp338,_tmp335[3]=& _tmp339,_tmp335[4]=& _tmp33A,_tmp335[5]=& _tmp33B;({unsigned _tmp760=e->loc;Cyc_Warn_warn2(_tmp760,_tag_fat(_tmp335,sizeof(void*),6U));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1863
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1867
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1871
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp340=({struct Cyc_Warn_String_Warn_Warg_struct _tmp660;_tmp660.tag=0U,({struct _fat_ptr _tmp761=({const char*_tmp345="implicit cast from ";_tag_fat(_tmp345,sizeof(char),20U);});_tmp660.f1=_tmp761;});_tmp660;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp341=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65F;_tmp65F.tag=2U,_tmp65F.f1=(void*)t1;_tmp65F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp342=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65E;_tmp65E.tag=0U,({struct _fat_ptr _tmp762=({const char*_tmp344=" to ";_tag_fat(_tmp344,sizeof(char),5U);});_tmp65E.f1=_tmp762;});_tmp65E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp343=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65D;_tmp65D.tag=2U,_tmp65D.f1=(void*)t2;_tmp65D;});void*_tmp33F[4U];_tmp33F[0]=& _tmp340,_tmp33F[1]=& _tmp341,_tmp33F[2]=& _tmp342,_tmp33F[3]=& _tmp343;({unsigned _tmp763=e->loc;Cyc_Warn_warn2(_tmp763,_tag_fat(_tmp33F,sizeof(void*),4U));});});
return 1;}
# 1877
return 0;}
# 1883
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1886
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1896
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1898
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1901
struct _tuple24 _tmp346=*env;struct _tuple24 _stmttmp53=_tmp346;struct _tuple24 _tmp347=_stmttmp53;int _tmp34A;struct _RegionHandle*_tmp349;struct Cyc_List_List*_tmp348;_LL1: _tmp348=_tmp347.f1;_tmp349=_tmp347.f2;_tmp34A=_tmp347.f3;_LL2: {struct Cyc_List_List*inst=_tmp348;struct _RegionHandle*r=_tmp349;int flatten=_tmp34A;
void*_tmp34B=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp34B;
struct Cyc_List_List*_tmp34C=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp34C;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp34E=_region_malloc(r,sizeof(*_tmp34E));({struct _tuple12*_tmp764=({struct _tuple12*_tmp34D=_region_malloc(r,sizeof(*_tmp34D));_tmp34D->f1=x->tq,_tmp34D->f2=t;_tmp34D;});_tmp34E->hd=_tmp764;}),_tmp34E->tl=0;_tmp34E;});
return ts;}}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1908
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 1910
struct _tuple25 _tmp34F=*env;struct _tuple25 _stmttmp54=_tmp34F;struct _tuple25 _tmp350=_stmttmp54;int _tmp352;struct _RegionHandle*_tmp351;_LL1: _tmp351=_tmp350.f1;_tmp352=_tmp350.f2;_LL2: {struct _RegionHandle*r=_tmp351;int flatten=_tmp352;
struct _tuple12 _tmp353=*x;struct _tuple12 _stmttmp55=_tmp353;struct _tuple12 _tmp354=_stmttmp55;void*_tmp356;struct Cyc_Absyn_Tqual _tmp355;_LL4: _tmp355=_tmp354.f1;_tmp356=_tmp354.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp355;void*t=_tmp356;
struct Cyc_List_List*_tmp357=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp357;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp359=_region_malloc(r,sizeof(*_tmp359));({struct _tuple12*_tmp765=({struct _tuple12*_tmp358=_region_malloc(r,sizeof(*_tmp358));_tmp358->f1=tq,_tmp358->f2=t;_tmp358;});_tmp359->hd=_tmp765;}),_tmp359->tl=0;_tmp359;});
return ts;}}}
# 1917
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1919
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp35A=t1;struct Cyc_List_List*_tmp35B;struct Cyc_List_List*_tmp35C;struct Cyc_List_List*_tmp35E;struct Cyc_Absyn_Aggrdecl*_tmp35D;switch(*((int*)_tmp35A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp35D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A)->f1)->f1).KnownAggr).val;_tmp35E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp35D;struct Cyc_List_List*ts=_tmp35E;
# 1937
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 1939
return({struct Cyc_List_List*_tmp369=_region_malloc(r,sizeof(*_tmp369));({struct _tuple12*_tmp767=({struct _tuple12*_tmp368=_region_malloc(r,sizeof(*_tmp368));({struct Cyc_Absyn_Tqual _tmp766=Cyc_Absyn_empty_tqual(0U);_tmp368->f1=_tmp766;}),_tmp368->f2=t1;_tmp368;});_tmp369->hd=_tmp767;}),_tmp369->tl=0;_tmp369;});{
struct Cyc_List_List*_tmp36A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp36A;
struct _tuple24 env=({struct _tuple24 _tmp661;_tmp661.f1=inst,_tmp661.f2=r,_tmp661.f3=flatten;_tmp661;});
struct Cyc_List_List*_tmp36B=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp56=_tmp36B;struct Cyc_List_List*_tmp36C=_stmttmp56;struct Cyc_List_List*_tmp36E;struct Cyc_Absyn_Aggrfield*_tmp36D;if(_tmp36C == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp36D=(struct Cyc_Absyn_Aggrfield*)_tmp36C->hd;_tmp36E=_tmp36C->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp36D;struct Cyc_List_List*tl=_tmp36E;
# 1945
struct Cyc_List_List*_tmp36F=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp36F;
env.f3=0;{
struct Cyc_List_List*_tmp370=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp370;
struct Cyc_List_List*_tmp371=({struct Cyc_List_List*_tmp372=_region_malloc(r,sizeof(*_tmp372));_tmp372->hd=hd2,_tmp372->tl=tl2;_tmp372;});struct Cyc_List_List*tts=_tmp371;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp35C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp35A)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp35C;
# 1924
struct _tuple25 _tmp35F=({struct _tuple25 _tmp662;_tmp662.f1=r,_tmp662.f2=flatten;_tmp662;});struct _tuple25 env=_tmp35F;
# 1926
struct Cyc_List_List*_tmp360=tqs;struct Cyc_List_List*_tmp362;struct _tuple12*_tmp361;if(_tmp360 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp361=(struct _tuple12*)_tmp360->hd;_tmp362=_tmp360->tl;_LLF: {struct _tuple12*hd=_tmp361;struct Cyc_List_List*tl=_tmp362;
# 1929
struct Cyc_List_List*_tmp363=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp363;
env.f2=0;{
struct Cyc_List_List*_tmp364=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp364;
struct Cyc_List_List*_tmp365=({struct Cyc_List_List*_tmp367=_region_malloc(r,sizeof(*_tmp367));_tmp367->hd=hd2,_tmp367->tl=tl2;_tmp367;});struct Cyc_List_List*temp=_tmp365;
return({struct _RegionHandle*_tmp768=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp768,({struct Cyc_List_List*_tmp366=_region_malloc(r,sizeof(*_tmp366));_tmp366->hd=hd2,_tmp366->tl=tl2;_tmp366;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp35A)->f1 == Cyc_Absyn_StructA){_LL7: _tmp35B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp35A)->f2;_LL8: {struct Cyc_List_List*fs=_tmp35B;
# 1952
struct _tuple24 env=({struct _tuple24 _tmp663;_tmp663.f1=0,_tmp663.f2=r,_tmp663.f3=flatten;_tmp663;});
struct Cyc_List_List*_tmp373=fs;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Aggrfield*_tmp374;if(_tmp373 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp374=(struct Cyc_Absyn_Aggrfield*)_tmp373->hd;_tmp375=_tmp373->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp374;struct Cyc_List_List*tl=_tmp375;
# 1956
struct Cyc_List_List*_tmp376=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp376;
env.f3=0;{
struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp377;
struct Cyc_List_List*_tmp378=({struct Cyc_List_List*_tmp379=_region_malloc(r,sizeof(*_tmp379));_tmp379->hd=hd2,_tmp379->tl=tl2;_tmp379;});struct Cyc_List_List*tts=_tmp378;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 1962
 goto _LL0;}_LL0:;}}
# 1965
return({struct Cyc_List_List*_tmp37B=_region_malloc(r,sizeof(*_tmp37B));({struct _tuple12*_tmp76A=({struct _tuple12*_tmp37A=_region_malloc(r,sizeof(*_tmp37A));({struct Cyc_Absyn_Tqual _tmp769=Cyc_Absyn_empty_tqual(0U);_tmp37A->f1=_tmp769;}),_tmp37A->f2=t1;_tmp37A;});_tmp37B->hd=_tmp76A;}),_tmp37B->tl=0;_tmp37B;});}
# 1968
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp37C=({struct _tuple15 _tmp664;({void*_tmp76C=Cyc_Tcutil_compress(t1);_tmp664.f1=_tmp76C;}),({void*_tmp76B=Cyc_Tcutil_compress(t2);_tmp664.f2=_tmp76B;});_tmp664;});struct _tuple15 _stmttmp57=_tmp37C;struct _tuple15 _tmp37D=_stmttmp57;enum Cyc_Absyn_Size_of _tmp37F;enum Cyc_Absyn_Size_of _tmp37E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D.f2)->f1)->tag == 1U){_LL1: _tmp37E=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D.f1)->f1)->f2;_tmp37F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp37D.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp37E;enum Cyc_Absyn_Size_of b2=_tmp37F;
# 1971
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1979
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1981
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp380=({struct _tuple15 _tmp665;_tmp665.f1=t1,_tmp665.f2=t2;_tmp665;});struct _tuple15 _stmttmp58=_tmp380;struct _tuple15 _tmp381=_stmttmp58;struct Cyc_Absyn_FnInfo _tmp383;struct Cyc_Absyn_FnInfo _tmp382;struct Cyc_List_List*_tmp388;struct Cyc_Absyn_Datatypedecl*_tmp387;struct Cyc_List_List*_tmp386;struct Cyc_Absyn_Datatypefield*_tmp385;struct Cyc_Absyn_Datatypedecl*_tmp384;void*_tmp394;void*_tmp393;void*_tmp392;void*_tmp391;struct Cyc_Absyn_Tqual _tmp390;void*_tmp38F;void*_tmp38E;void*_tmp38D;void*_tmp38C;void*_tmp38B;struct Cyc_Absyn_Tqual _tmp38A;void*_tmp389;switch(*((int*)_tmp381.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f2)->tag == 3U){_LL1: _tmp389=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f1)->f1).elt_type;_tmp38A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f1)->f1).elt_tq;_tmp38B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f1)->f1).ptr_atts).rgn;_tmp38C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f1)->f1).ptr_atts).nullable;_tmp38D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f1)->f1).ptr_atts).bounds;_tmp38E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f1)->f1).ptr_atts).zero_term;_tmp38F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f2)->f1).elt_type;_tmp390=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f2)->f1).elt_tq;_tmp391=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f2)->f1).ptr_atts).rgn;_tmp392=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f2)->f1).ptr_atts).nullable;_tmp393=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f2)->f1).ptr_atts).bounds;_tmp394=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp381.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp389;struct Cyc_Absyn_Tqual q_a=_tmp38A;void*rt_a=_tmp38B;void*null_a=_tmp38C;void*b_a=_tmp38D;void*zt_a=_tmp38E;void*t_b=_tmp38F;struct Cyc_Absyn_Tqual q_b=_tmp390;void*rt_b=_tmp391;void*null_b=_tmp392;void*b_b=_tmp393;void*zt_b=_tmp394;
# 1993
if(q_a.real_const && !q_b.real_const)
return 0;
# 1996
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 2000
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 2004
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 2008
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp395=({void*_tmp76D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp76D,b_a);});struct Cyc_Absyn_Exp*e1=_tmp395;
struct Cyc_Absyn_Exp*_tmp396=({void*_tmp76E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp76E,b_b);});struct Cyc_Absyn_Exp*e2=_tmp396;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 2017
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 2022
int _tmp397=({void*_tmp76F=b_b;Cyc_Unify_unify(_tmp76F,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);int deep_subtype=_tmp397;
# 2025
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp773=po;struct Cyc_List_List*_tmp772=({struct Cyc_List_List*_tmp399=_cycalloc(sizeof(*_tmp399));({struct _tuple15*_tmp770=({struct _tuple15*_tmp398=_cycalloc(sizeof(*_tmp398));_tmp398->f1=t1,_tmp398->f2=t2;_tmp398;});_tmp399->hd=_tmp770;}),_tmp399->tl=assume;_tmp399;});void*_tmp771=t_a;Cyc_Tcutil_ptrsubtype(_tmp773,_tmp772,_tmp771,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp384=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp385=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp386=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f1)->f2;_tmp387=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f2)->f1)->f1).KnownDatatype).val;_tmp388=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp381.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp384;struct Cyc_Absyn_Datatypefield*df=_tmp385;struct Cyc_List_List*ts1=_tmp386;struct Cyc_Absyn_Datatypedecl*dd2=_tmp387;struct Cyc_List_List*ts2=_tmp388;
# 2032
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 2034
if(({int _tmp774=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp774 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp381.f2)->tag == 5U){_LL5: _tmp382=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp381.f1)->f1;_tmp383=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp381.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp382;struct Cyc_Absyn_FnInfo f2=_tmp383;
# 2040
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp39A=f1.tvars;struct Cyc_List_List*tvs1=_tmp39A;
struct Cyc_List_List*_tmp39B=f2.tvars;struct Cyc_List_List*tvs2=_tmp39B;
if(({int _tmp775=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp775 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp39D=_cycalloc(sizeof(*_tmp39D));({struct _tuple19*_tmp777=({struct _tuple19*_tmp39C=_cycalloc(sizeof(*_tmp39C));_tmp39C->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp776=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp39C->f2=_tmp776;});_tmp39C;});_tmp39D->hd=_tmp777;}),_tmp39D->tl=inst;_tmp39D;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2051
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp77A=po;struct Cyc_List_List*_tmp779=assume;void*_tmp778=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp39E=_cycalloc(sizeof(*_tmp39E));_tmp39E->tag=5U,_tmp39E->f1=f1;_tmp39E;});Cyc_Tcutil_subtype(_tmp77A,_tmp779,_tmp778,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->tag=5U,_tmp39F->f1=f2;_tmp39F;}));});}}}
# 2058
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp3A0=f1.args;struct Cyc_List_List*args1=_tmp3A0;
struct Cyc_List_List*_tmp3A1=f2.args;struct Cyc_List_List*args2=_tmp3A1;
# 2063
if(({int _tmp77B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp77B != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2065
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp3A2=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp59=_tmp3A2;struct _tuple9 _tmp3A3=_stmttmp59;void*_tmp3A5;struct Cyc_Absyn_Tqual _tmp3A4;_LLA: _tmp3A4=_tmp3A3.f2;_tmp3A5=_tmp3A3.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp3A4;void*t1=_tmp3A5;
struct _tuple9 _tmp3A6=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp5A=_tmp3A6;struct _tuple9 _tmp3A7=_stmttmp5A;void*_tmp3A9;struct Cyc_Absyn_Tqual _tmp3A8;_LLD: _tmp3A8=_tmp3A7.f2;_tmp3A9=_tmp3A7.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp3A8;void*t2=_tmp3A9;
# 2069
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2073
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp3AA=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp3AA;
struct Cyc_Absyn_VarargInfo _tmp3AB=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp3AB;
# 2078
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2083
if(!({void*_tmp77C=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp77C,(void*)_check_null(f2.effect));}))return 0;
# 2085
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2087
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2089
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2091
return 0;
# 2093
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2095
return 0;
# 2097
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2108
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2111
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp3AC=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp5B=_tmp3AC;struct _tuple12*_tmp3AD=_stmttmp5B;void*_tmp3AF;struct Cyc_Absyn_Tqual _tmp3AE;_LL1: _tmp3AE=_tmp3AD->f1;_tmp3AF=_tmp3AD->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp3AE;void*t1a=_tmp3AF;
struct _tuple12*_tmp3B0=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp5C=_tmp3B0;struct _tuple12*_tmp3B1=_stmttmp5C;void*_tmp3B3;struct Cyc_Absyn_Tqual _tmp3B2;_LL4: _tmp3B2=_tmp3B1->f1;_tmp3B3=_tmp3B1->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp3B2;void*t2a=_tmp3B3;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2125
return 1;}
# 2130
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
{void*_tmp3B4=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2138
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2140
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2144
void*_tmp3B5=t1;void*_tmp3B6;struct Cyc_Absyn_Enumdecl*_tmp3B7;void*_tmp3BB;struct Cyc_Absyn_Exp*_tmp3BA;struct Cyc_Absyn_Tqual _tmp3B9;void*_tmp3B8;void*_tmp3C1;void*_tmp3C0;void*_tmp3BF;void*_tmp3BE;struct Cyc_Absyn_Tqual _tmp3BD;void*_tmp3BC;switch(*((int*)_tmp3B5)){case 3U: _LLA: _tmp3BC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5)->f1).elt_type;_tmp3BD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5)->f1).elt_tq;_tmp3BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5)->f1).ptr_atts).rgn;_tmp3BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5)->f1).ptr_atts).nullable;_tmp3C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5)->f1).ptr_atts).bounds;_tmp3C1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B5)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp3BC;struct Cyc_Absyn_Tqual q_a=_tmp3BD;void*rt_a=_tmp3BE;void*null_a=_tmp3BF;void*b_a=_tmp3C0;void*zt_a=_tmp3C1;
# 2153
{void*_tmp3C2=t2;void*_tmp3C8;void*_tmp3C7;void*_tmp3C6;void*_tmp3C5;struct Cyc_Absyn_Tqual _tmp3C4;void*_tmp3C3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->tag == 3U){_LL19: _tmp3C3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).elt_type;_tmp3C4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).elt_tq;_tmp3C5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).ptr_atts).rgn;_tmp3C6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).ptr_atts).nullable;_tmp3C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).ptr_atts).bounds;_tmp3C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3C2)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3C3;struct Cyc_Absyn_Tqual q_b=_tmp3C4;void*rt_b=_tmp3C5;void*null_b=_tmp3C6;void*b_b=_tmp3C7;void*zt_b=_tmp3C8;
# 2155
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3C9=({struct Cyc_List_List*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct _tuple15*_tmp77D=({struct _tuple15*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->f1=t1,_tmp3D0->f2=t2;_tmp3D0;});_tmp3D1->hd=_tmp77D;}),_tmp3D1->tl=0;_tmp3D1;});struct Cyc_List_List*assump=_tmp3C9;
int _tmp3CA=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3CA;
# 2168 "tcutil.cyc"
int _tmp3CB=
({void*_tmp77E=b_b;Cyc_Unify_unify(_tmp77E,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2168
int deep_castable=_tmp3CB;
# 2171
int _tmp3CC=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2171
int ptrsub=_tmp3CC;
# 2174
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2176
int _tmp3CD=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2176
int bitcase=_tmp3CD;
# 2179
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3CE=({void*_tmp77F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp77F,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3CE;
struct Cyc_Absyn_Exp*_tmp3CF=({void*_tmp780=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp780,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3CF;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2188
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2193
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2199
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp3B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B5)->f1).elt_type;_tmp3B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B5)->f1).tq;_tmp3BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B5)->f1).num_elts;_tmp3BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B5)->f1).zero_term;_LLD: {void*t1a=_tmp3B8;struct Cyc_Absyn_Tqual tq1a=_tmp3B9;struct Cyc_Absyn_Exp*e1=_tmp3BA;void*zt1=_tmp3BB;
# 2201
{void*_tmp3D2=t2;void*_tmp3D6;struct Cyc_Absyn_Exp*_tmp3D5;struct Cyc_Absyn_Tqual _tmp3D4;void*_tmp3D3;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D2)->tag == 4U){_LL1E: _tmp3D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D2)->f1).elt_type;_tmp3D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D2)->f1).tq;_tmp3D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D2)->f1).num_elts;_tmp3D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D2)->f1).zero_term;_LL1F: {void*t2a=_tmp3D3;struct Cyc_Absyn_Tqual tq2a=_tmp3D4;struct Cyc_Absyn_Exp*e2=_tmp3D5;void*zt2=_tmp3D6;
# 2203
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2208
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2210
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2212
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->f1)){case 15U: _LLE: _tmp3B7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp3B7;
# 2216
{void*_tmp3D7=t2;struct Cyc_Absyn_Enumdecl*_tmp3D8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->tag == 15U){_LL23: _tmp3D8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D7)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3D8;
# 2218
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp781=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp781 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2224
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2227
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->f2 != 0){_LL14: _tmp3B6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B5)->f2)->hd;_LL15: {void*r1=_tmp3B6;
# 2230
{void*_tmp3D9=t2;void*_tmp3DA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f2 != 0){_LL28: _tmp3DA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D9)->f2)->hd;_LL29: {void*r2=_tmp3DA;
# 2232
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2236
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2242
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3DB=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3DB;
({void*_tmp782=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->tag=14U,_tmp3DC->f1=t,_tmp3DC->f2=inner,_tmp3DC->f3=0,_tmp3DC->f4=c;_tmp3DC;});e->r=_tmp782;});
e->topt=t;}}
# 2251
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2256
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
return({struct Cyc_Absyn_Tvar*_tmp3E1=_cycalloc(sizeof(*_tmp3E1));({struct _fat_ptr*_tmp785=({struct _fat_ptr*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));({struct _fat_ptr _tmp784=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3DF=({struct Cyc_Int_pa_PrintArg_struct _tmp666;_tmp666.tag=1U,_tmp666.f1=(unsigned long)Cyc_Tcutil_tvar_counter ++;_tmp666;});void*_tmp3DD[1U];_tmp3DD[0]=& _tmp3DF;({struct _fat_ptr _tmp783=({const char*_tmp3DE="#%d";_tag_fat(_tmp3DE,sizeof(char),4U);});Cyc_aprintf(_tmp783,_tag_fat(_tmp3DD,sizeof(void*),1U));});});*_tmp3E0=_tmp784;});_tmp3E0;});_tmp3E1->name=_tmp785;}),_tmp3E1->identity=- 1,_tmp3E1->kind=k;_tmp3E1;});}
# 2260
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
return(int)*((const char*)_check_fat_subscript((t->name)[0],sizeof(char),0U))== (int)'#';}
# 2265
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2272
struct Cyc_List_List*_tmp3E2=0;struct Cyc_List_List*fn_type_atts=_tmp3E2;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Atts_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3E3=_cycalloc(sizeof(*_tmp3E3));_tmp3E3->hd=(void*)atts->hd,_tmp3E3->tl=fn_type_atts;_tmp3E3;});}}{
struct Cyc_Absyn_FnInfo _tmp3E4=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3E4;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->tag=5U,_tmp3E5->f1=type_info;_tmp3E5;});}}
# 2280
return(void*)_check_null(fd->cached_type);}
# 2286
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2288
union Cyc_Relations_RelnOp _tmp3E6=*rop;union Cyc_Relations_RelnOp _stmttmp5D=_tmp3E6;union Cyc_Relations_RelnOp _tmp3E7=_stmttmp5D;struct Cyc_Absyn_Vardecl*_tmp3E8;struct Cyc_Absyn_Vardecl*_tmp3E9;switch((_tmp3E7.RNumelts).tag){case 2U: _LL1: _tmp3E9=(_tmp3E7.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3E9;
# 2290
struct _tuple1 _tmp3EA=*vd->name;struct _tuple1 _stmttmp5E=_tmp3EA;struct _tuple1 _tmp3EB=_stmttmp5E;struct _fat_ptr*_tmp3ED;union Cyc_Absyn_Nmspace _tmp3EC;_LL8: _tmp3EC=_tmp3EB.f1;_tmp3ED=_tmp3EB.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3EC;struct _fat_ptr*var=_tmp3ED;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp786=(struct _fat_ptr)*var;Cyc_strcmp(_tmp786,({const char*_tmp3EE="return_value";_tag_fat(_tmp3EE,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp787=Cyc_Relations_RReturn();*rop=_tmp787;});
goto _LL0;}{
# 2296
unsigned c=0U;
{struct Cyc_List_List*_tmp3EF=args;struct Cyc_List_List*a=_tmp3EF;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3F0=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5F=_tmp3F0;struct _tuple9*_tmp3F1=_stmttmp5F;struct _fat_ptr*_tmp3F2;_LLB: _tmp3F2=_tmp3F1->f1;_LLC: {struct _fat_ptr*vopt=_tmp3F2;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp788=Cyc_Relations_RParam(c);*rop=_tmp788;});
break;}}}}
# 2304
goto _LL0;}}}case 3U: _LL3: _tmp3E8=(_tmp3E7.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3E8;
# 2306
struct _tuple1 _tmp3F3=*vd->name;struct _tuple1 _stmttmp60=_tmp3F3;struct _tuple1 _tmp3F4=_stmttmp60;struct _fat_ptr*_tmp3F6;union Cyc_Absyn_Nmspace _tmp3F5;_LLE: _tmp3F5=_tmp3F4.f1;_tmp3F6=_tmp3F4.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3F5;struct _fat_ptr*var=_tmp3F6;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3F7=args;struct Cyc_List_List*a=_tmp3F7;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3F8=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp61=_tmp3F8;struct _tuple9*_tmp3F9=_stmttmp61;struct _fat_ptr*_tmp3FA;_LL11: _tmp3FA=_tmp3F9->f1;_LL12: {struct _fat_ptr*vopt=_tmp3FA;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp789=Cyc_Relations_RParamNumelts(c);*rop=_tmp789;});
break;}}}}
# 2316
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2321
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2323
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2327
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2329
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3FB=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3FB;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2336
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3FC=pr;void*_tmp3FE;struct Cyc_Absyn_Tqual _tmp3FD;_LL1: _tmp3FD=_tmp3FC->f1;_tmp3FE=_tmp3FC->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3FD;void*t2=_tmp3FE;
return t2 == t?pr:({struct _tuple12*_tmp3FF=_cycalloc(sizeof(*_tmp3FF));_tmp3FF->f1=tq,_tmp3FF->f2=t;_tmp3FF;});}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2340
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2342
return({struct _tuple27*_tmp401=_region_malloc(rgn,sizeof(*_tmp401));({struct _tuple26*_tmp78A=({struct _tuple26*_tmp400=_region_malloc(rgn,sizeof(*_tmp400));_tmp400->f1=(*y).f1,_tmp400->f2=(*y).f2;_tmp400;});_tmp401->f1=_tmp78A;}),_tmp401->f2=(*y).f3;_tmp401;});}
# 2344
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2346
struct _tuple9 _tmp402=*orig_arg;struct _tuple9 _stmttmp62=_tmp402;struct _tuple9 _tmp403=_stmttmp62;void*_tmp406;struct Cyc_Absyn_Tqual _tmp405;struct _fat_ptr*_tmp404;_LL1: _tmp404=_tmp403.f1;_tmp405=_tmp403.f2;_tmp406=_tmp403.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp404;struct Cyc_Absyn_Tqual tq_orig=_tmp405;void*t_orig=_tmp406;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp407=_cycalloc(sizeof(*_tmp407));_tmp407->f1=vopt_orig,_tmp407->f2=tq_orig,_tmp407->f3=t;_tmp407;});}}
# 2350
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2354
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2356
return({struct Cyc_Absyn_Exp*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->topt=old->topt,_tmp408->r=r,_tmp408->loc=old->loc,_tmp408->annot=old->annot;_tmp408;});}
# 2361
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp409=e->r;void*_stmttmp63=_tmp409;void*_tmp40A=_stmttmp63;void*_tmp40B;struct Cyc_List_List*_tmp40D;void*_tmp40C;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Absyn_Exp*_tmp40F;void*_tmp410;enum Cyc_Absyn_Coercion _tmp414;int _tmp413;struct Cyc_Absyn_Exp*_tmp412;void*_tmp411;struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Exp*_tmp415;struct Cyc_Absyn_Exp*_tmp418;struct Cyc_Absyn_Exp*_tmp417;struct Cyc_Absyn_Exp*_tmp41A;struct Cyc_Absyn_Exp*_tmp419;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_Absyn_Exp*_tmp41B;struct Cyc_List_List*_tmp41F;enum Cyc_Absyn_Primop _tmp41E;switch(*((int*)_tmp40A)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp41E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_tmp41F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp40A)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp41E;struct Cyc_List_List*es=_tmp41F;
# 2370
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp420=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp420;
struct Cyc_Absyn_Exp*_tmp421=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp421;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp78C=e;Cyc_Tcutil_copye(_tmp78C,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->tag=3U,_tmp423->f1=p,({struct Cyc_List_List*_tmp78B=({struct Cyc_Absyn_Exp*_tmp422[1U];_tmp422[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp422,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp423->f2=_tmp78B;});_tmp423;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp424=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp424;
struct Cyc_Absyn_Exp*_tmp425=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp425;
struct Cyc_Absyn_Exp*_tmp426=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp426;
struct Cyc_Absyn_Exp*_tmp427=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp427;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp78E=e;Cyc_Tcutil_copye(_tmp78E,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->tag=3U,_tmp429->f1=p,({struct Cyc_List_List*_tmp78D=({struct Cyc_Absyn_Exp*_tmp428[2U];_tmp428[0]=new_e1,_tmp428[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp428,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp429->f2=_tmp78D;});_tmp429;}));});}}
# 2382
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp42B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp667;_tmp667.tag=0U,({struct _fat_ptr _tmp78F=({const char*_tmp42C="primop does not have 1 or 2 args!";_tag_fat(_tmp42C,sizeof(char),34U);});_tmp667.f1=_tmp78F;});_tmp667;});void*_tmp42A[1U];_tmp42A[0]=& _tmp42B;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp42A,sizeof(void*),1U));});}case 6U: _LLD: _tmp41B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_tmp41C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp40A)->f2;_tmp41D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp40A)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp41B;struct Cyc_Absyn_Exp*e2=_tmp41C;struct Cyc_Absyn_Exp*e3=_tmp41D;
# 2384
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42D;
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp42E;
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp42F;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp790=e;Cyc_Tcutil_copye(_tmp790,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp430=_cycalloc(sizeof(*_tmp430));_tmp430->tag=6U,_tmp430->f1=new_e1,_tmp430->f2=new_e2,_tmp430->f3=new_e3;_tmp430;}));});}case 7U: _LLF: _tmp419=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_tmp41A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp40A)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp419;struct Cyc_Absyn_Exp*e2=_tmp41A;
# 2390
struct Cyc_Absyn_Exp*_tmp431=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp431;
struct Cyc_Absyn_Exp*_tmp432=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp432;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp791=e;Cyc_Tcutil_copye(_tmp791,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->tag=7U,_tmp433->f1=new_e1,_tmp433->f2=new_e2;_tmp433;}));});}case 8U: _LL11: _tmp417=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_tmp418=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp40A)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp417;struct Cyc_Absyn_Exp*e2=_tmp418;
# 2395
struct Cyc_Absyn_Exp*_tmp434=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp434;
struct Cyc_Absyn_Exp*_tmp435=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp435;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp792=e;Cyc_Tcutil_copye(_tmp792,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->tag=8U,_tmp436->f1=new_e1,_tmp436->f2=new_e2;_tmp436;}));});}case 9U: _LL13: _tmp415=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_tmp416=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp40A)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp415;struct Cyc_Absyn_Exp*e2=_tmp416;
# 2400
struct Cyc_Absyn_Exp*_tmp437=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp437;
struct Cyc_Absyn_Exp*_tmp438=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp438;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp793=e;Cyc_Tcutil_copye(_tmp793,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439->tag=9U,_tmp439->f1=new_e1,_tmp439->f2=new_e2;_tmp439;}));});}case 14U: _LL15: _tmp411=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_tmp412=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40A)->f2;_tmp413=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40A)->f3;_tmp414=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp40A)->f4;_LL16: {void*t=_tmp411;struct Cyc_Absyn_Exp*e1=_tmp412;int b=_tmp413;enum Cyc_Absyn_Coercion c=_tmp414;
# 2405
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp43A;
void*_tmp43B=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp43B;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp794=e;Cyc_Tcutil_copye(_tmp794,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->tag=14U,_tmp43C->f1=new_typ,_tmp43C->f2=new_e1,_tmp43C->f3=b,_tmp43C->f4=c;_tmp43C;}));});}case 17U: _LL17: _tmp410=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_LL18: {void*t=_tmp410;
# 2410
void*_tmp43D=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp43D;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp795=e;Cyc_Tcutil_copye(_tmp795,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E->tag=17U,_tmp43E->f1=new_typ;_tmp43E;}));});}case 18U: _LL19: _tmp40F=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp40F;
# 2413
struct Cyc_Absyn_Exp*_tmp43F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp43F;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp796=e;Cyc_Tcutil_copye(_tmp796,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp440=_cycalloc(sizeof(*_tmp440));_tmp440->tag=18U,_tmp440->f1=new_e1;_tmp440;}));});}case 41U: _LL1B: _tmp40E=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp40E;
# 2416
struct Cyc_Absyn_Exp*_tmp441=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp441;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp797=e;Cyc_Tcutil_copye(_tmp797,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=41U,_tmp442->f1=new_e1;_tmp442;}));});}case 19U: _LL1D: _tmp40C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_tmp40D=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp40A)->f2;_LL1E: {void*t=_tmp40C;struct Cyc_List_List*f=_tmp40D;
# 2419
void*_tmp443=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp443;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp798=e;Cyc_Tcutil_copye(_tmp798,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp444=_cycalloc(sizeof(*_tmp444));_tmp444->tag=19U,_tmp444->f1=new_typ,_tmp444->f2=f;_tmp444;}));});}case 39U: _LL1F: _tmp40B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp40A)->f1;_LL20: {void*t=_tmp40B;
# 2422
void*_tmp445=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp445;
if(new_typ == t)return e;{
# 2425
void*_tmp446=Cyc_Tcutil_compress(new_typ);void*_stmttmp64=_tmp446;void*_tmp447=_stmttmp64;struct Cyc_Absyn_Exp*_tmp448;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp447)->tag == 9U){_LL24: _tmp448=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp447)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp448;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp799=e;Cyc_Tcutil_copye(_tmp799,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->tag=39U,_tmp449->f1=new_typ;_tmp449;}));});}_LL23:;}}default: _LL21: _LL22:
# 2429
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp44B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp668;_tmp668.tag=0U,({struct _fat_ptr _tmp79A=({const char*_tmp44C="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp44C,sizeof(char),46U);});_tmp668.f1=_tmp79A;});_tmp668;});void*_tmp44A[1U];_tmp44A[0]=& _tmp44B;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp44A,sizeof(void*),1U));});}_LL0:;}
# 2433
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2436
return e == 0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2439
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2442
void*_tmp44D=f->type;void*t=_tmp44D;
struct Cyc_Absyn_Exp*_tmp44E=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp44E;
void*_tmp44F=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp44F;
struct Cyc_Absyn_Exp*_tmp450=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp450;
if(t == new_typ && req == new_req)
return f;{
struct Cyc_Absyn_Aggrfield*_tmp451=({struct Cyc_Absyn_Aggrfield*_tmp452=_cycalloc(sizeof(*_tmp452));*_tmp452=*f;_tmp452;});struct Cyc_Absyn_Aggrfield*ans=_tmp451;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2454
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2457
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp453=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp453;
struct Cyc_List_List*_tmp454=fs->tl;struct Cyc_List_List*t=_tmp454;
struct Cyc_Absyn_Aggrfield*_tmp455=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp455;
struct Cyc_List_List*_tmp456=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_typ=_tmp456;
if(new_f == f && new_typ == t)
return fs;
return({struct Cyc_List_List*_tmp457=_cycalloc(sizeof(*_tmp457));_tmp457->hd=new_f,_tmp457->tl=new_typ;_tmp457;});}}
# 2467
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2470
struct _tuple0 _tmp458=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp65=_tmp458;struct _tuple0 _tmp459=_stmttmp65;struct Cyc_List_List*_tmp45B;struct Cyc_List_List*_tmp45A;_LL1: _tmp45A=_tmp459.f1;_tmp45B=_tmp459.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp45A;struct Cyc_List_List*rpo1b=_tmp45B;
struct Cyc_List_List*_tmp45C=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp45C;
struct Cyc_List_List*_tmp45D=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp45D;
if(rpo2a == rpo1a && rpo2b == rpo1b)
return rgn_po;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}
# 2480
static int Cyc_Tcutil_skip_fntypes=0;
# 2482
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2485
void*_tmp45E=Cyc_Tcutil_compress(t);void*_stmttmp66=_tmp45E;void*_tmp45F=_stmttmp66;struct Cyc_Absyn_Exp*_tmp460;struct Cyc_Absyn_Exp*_tmp461;struct Cyc_List_List*_tmp463;void*_tmp462;void*_tmp464;struct Cyc_List_List*_tmp466;enum Cyc_Absyn_AggrKind _tmp465;struct Cyc_List_List*_tmp467;struct Cyc_Absyn_Vardecl*_tmp473;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Exp*_tmp471;struct Cyc_List_List*_tmp470;struct Cyc_List_List*_tmp46F;struct Cyc_Absyn_VarargInfo*_tmp46E;int _tmp46D;struct Cyc_List_List*_tmp46C;void*_tmp46B;struct Cyc_Absyn_Tqual _tmp46A;void*_tmp469;struct Cyc_List_List*_tmp468;void*_tmp479;void*_tmp478;void*_tmp477;void*_tmp476;struct Cyc_Absyn_Tqual _tmp475;void*_tmp474;unsigned _tmp47E;void*_tmp47D;struct Cyc_Absyn_Exp*_tmp47C;struct Cyc_Absyn_Tqual _tmp47B;void*_tmp47A;void*_tmp482;struct Cyc_Absyn_Typedefdecl*_tmp481;struct Cyc_List_List*_tmp480;struct _tuple1*_tmp47F;struct Cyc_Absyn_Tvar*_tmp483;switch(*((int*)_tmp45F)){case 2U: _LL1: _tmp483=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp45F)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp483;
# 2488
struct _handler_cons _tmp484;_push_handler(& _tmp484);{int _tmp486=0;if(setjmp(_tmp484.handler))_tmp486=1;if(!_tmp486){{void*_tmp487=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp487;};_pop_handler();}else{void*_tmp485=(void*)Cyc_Core_get_exn_thrown();void*_tmp488=_tmp485;void*_tmp489;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp488)->tag == Cyc_Core_Not_found){_LL1A: _LL1B:
 return t;}else{_LL1C: _tmp489=_tmp488;_LL1D: {void*exn=_tmp489;(int)_rethrow(exn);}}_LL19:;}}}case 8U: _LL3: _tmp47F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp45F)->f1;_tmp480=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp45F)->f2;_tmp481=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp45F)->f3;_tmp482=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp45F)->f4;_LL4: {struct _tuple1*n=_tmp47F;struct Cyc_List_List*ts=_tmp480;struct Cyc_Absyn_Typedefdecl*td=_tmp481;void*topt=_tmp482;
# 2491
struct Cyc_List_List*_tmp48A=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp48A;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp48B=_cycalloc(sizeof(*_tmp48B));_tmp48B->tag=8U,_tmp48B->f1=n,_tmp48B->f2=new_ts,_tmp48B->f3=td,_tmp48B->f4=topt;_tmp48B;});}case 4U: _LL5: _tmp47A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp45F)->f1).elt_type;_tmp47B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp45F)->f1).tq;_tmp47C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp45F)->f1).num_elts;_tmp47D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp45F)->f1).zero_term;_tmp47E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp45F)->f1).zt_loc;_LL6: {void*t1=_tmp47A;struct Cyc_Absyn_Tqual tq=_tmp47B;struct Cyc_Absyn_Exp*e=_tmp47C;void*zt=_tmp47D;unsigned ztl=_tmp47E;
# 2494
void*_tmp48C=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp48C;
struct Cyc_Absyn_Exp*_tmp48D=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp48D;
void*_tmp48E=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp48E;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp48F=_cycalloc(sizeof(*_tmp48F));
_tmp48F->tag=4U,(_tmp48F->f1).elt_type=new_t1,(_tmp48F->f1).tq=tq,(_tmp48F->f1).num_elts=new_e,(_tmp48F->f1).zero_term=new_zt,(_tmp48F->f1).zt_loc=ztl;_tmp48F;});}case 3U: _LL7: _tmp474=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).elt_type;_tmp475=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).elt_tq;_tmp476=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).ptr_atts).rgn;_tmp477=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).ptr_atts).nullable;_tmp478=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).ptr_atts).bounds;_tmp479=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp45F)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp474;struct Cyc_Absyn_Tqual tq=_tmp475;void*r=_tmp476;void*n=_tmp477;void*b=_tmp478;void*zt=_tmp479;
# 2500
void*_tmp490=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp490;
void*_tmp491=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp491;
void*_tmp492=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp492;
void*_tmp493=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp493;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp669;_tmp669.elt_type=new_t1,_tmp669.elt_tq=tq,(_tmp669.ptr_atts).rgn=new_r,(_tmp669.ptr_atts).nullable=n,(_tmp669.ptr_atts).bounds=new_b,(_tmp669.ptr_atts).zero_term=new_zt,(_tmp669.ptr_atts).ptrloc=0;_tmp669;}));}case 5U: _LL9: _tmp468=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).tvars;_tmp469=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).effect;_tmp46A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).ret_tqual;_tmp46B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).ret_type;_tmp46C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).args;_tmp46D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).c_varargs;_tmp46E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).cyc_varargs;_tmp46F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).rgn_po;_tmp470=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).attributes;_tmp471=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).requires_clause;_tmp472=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).ensures_clause;_tmp473=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp45F)->f1).return_value;_LLA: {struct Cyc_List_List*vs=_tmp468;void*eff=_tmp469;struct Cyc_Absyn_Tqual rtq=_tmp46A;void*rtyp=_tmp46B;struct Cyc_List_List*args=_tmp46C;int c_varargs=_tmp46D;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp46E;struct Cyc_List_List*rgn_po=_tmp46F;struct Cyc_List_List*atts=_tmp470;struct Cyc_Absyn_Exp*req=_tmp471;struct Cyc_Absyn_Exp*ens=_tmp472;struct Cyc_Absyn_Vardecl*ret_value=_tmp473;
# 2509
if(Cyc_Tcutil_skip_fntypes)return t;
# 2511
{struct Cyc_List_List*_tmp494=vs;struct Cyc_List_List*p=_tmp494;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp496=_region_malloc(rgn,sizeof(*_tmp496));({struct _tuple19*_tmp79C=({struct _tuple19*_tmp495=_region_malloc(rgn,sizeof(*_tmp495));_tmp495->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp79B=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp495->f2=_tmp79B;});_tmp495;});_tmp496->hd=_tmp79C;}),_tmp496->tl=inst;_tmp496;});}}{
struct _tuple0 _tmp497=({struct _RegionHandle*_tmp79E=rgn;struct _RegionHandle*_tmp79D=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp79E,_tmp79D,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2513
struct _tuple0 _stmttmp67=_tmp497;struct _tuple0 _tmp498=_stmttmp67;struct Cyc_List_List*_tmp49A;struct Cyc_List_List*_tmp499;_LL1F: _tmp499=_tmp498.f1;_tmp49A=_tmp498.f2;_LL20: {struct Cyc_List_List*qs=_tmp499;struct Cyc_List_List*ts=_tmp49A;
# 2515
struct Cyc_List_List*_tmp49B=args;struct Cyc_List_List*args2=_tmp49B;
struct Cyc_List_List*_tmp49C=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp49C;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff == 0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2521
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2525
struct Cyc_Absyn_VarargInfo _tmp49D=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp68=_tmp49D;struct Cyc_Absyn_VarargInfo _tmp49E=_stmttmp68;int _tmp4A2;void*_tmp4A1;struct Cyc_Absyn_Tqual _tmp4A0;struct _fat_ptr*_tmp49F;_LL22: _tmp49F=_tmp49E.name;_tmp4A0=_tmp49E.tq;_tmp4A1=_tmp49E.type;_tmp4A2=_tmp49E.inject;_LL23: {struct _fat_ptr*n=_tmp49F;struct Cyc_Absyn_Tqual tq=_tmp4A0;void*t=_tmp4A1;int i=_tmp4A2;
void*_tmp4A3=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp4A3;
if(t2 == t)cyc_varargs2=cyc_varargs;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->name=n,_tmp4A4->tq=tq,_tmp4A4->type=t2,_tmp4A4->inject=i;_tmp4A4;});}}}{
# 2530
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp4A5=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp4A5;
struct Cyc_List_List*_tmp4A6=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp4A6;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->tag=5U,(_tmp4A7->f1).tvars=vs,(_tmp4A7->f1).effect=eff2,(_tmp4A7->f1).ret_tqual=rtq,({void*_tmp79F=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp4A7->f1).ret_type=_tmp79F;}),(_tmp4A7->f1).args=args2,(_tmp4A7->f1).c_varargs=c_varargs,(_tmp4A7->f1).cyc_varargs=cyc_varargs2,(_tmp4A7->f1).rgn_po=rgn_po2,(_tmp4A7->f1).attributes=atts,(_tmp4A7->f1).requires_clause=req2,(_tmp4A7->f1).requires_relns=req_relns2,(_tmp4A7->f1).ensures_clause=ens2,(_tmp4A7->f1).ensures_relns=ens_relns2,(_tmp4A7->f1).return_value=ret_value;_tmp4A7;});}}}}}case 6U: _LLB: _tmp467=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp45F)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp467;
# 2539
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp4A8=tqts;struct Cyc_List_List*ts1=_tmp4A8;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp4A9=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp4A9;
void*_tmp4AA=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp4AA;
if(t1 != t2)
change=1;
ts2=({struct Cyc_List_List*_tmp4AB=_region_malloc(rgn,sizeof(*_tmp4AB));_tmp4AB->hd=t2,_tmp4AB->tl=ts2;_tmp4AB;});}}
# 2548
return !change?t:(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->tag=6U,({struct Cyc_List_List*_tmp7A1=({struct Cyc_List_List*_tmp7A0=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp7A0,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp4AC->f1=_tmp7A1;});_tmp4AC;});}case 7U: _LLD: _tmp465=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp45F)->f1;_tmp466=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp45F)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp465;struct Cyc_List_List*fs=_tmp466;
# 2550
struct Cyc_List_List*_tmp4AD=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp4AD;
return fs == new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4AE=_cycalloc(sizeof(*_tmp4AE));_tmp4AE->tag=7U,_tmp4AE->f1=k,_tmp4AE->f2=new_fs;_tmp4AE;});}case 1U: _LLF: _tmp464=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp45F)->f2;_LL10: {void*r=_tmp464;
# 2553
return r == 0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0U: _LL11: _tmp462=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45F)->f1;_tmp463=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp45F)->f2;_LL12: {void*c=_tmp462;struct Cyc_List_List*ts=_tmp463;
# 2555
struct Cyc_List_List*_tmp4AF=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp4AF;
return ts == new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->tag=0U,_tmp4B0->f1=c,_tmp4B0->f2=new_ts;_tmp4B0;});}case 9U: _LL13: _tmp461=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp45F)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp461;
# 2558
struct Cyc_Absyn_Exp*_tmp4B1=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4B1;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2->tag=9U,_tmp4B2->f1=new_e;_tmp4B2;});}case 11U: _LL15: _tmp460=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp45F)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp460;
# 2561
struct Cyc_Absyn_Exp*_tmp4B3=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4B3;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->tag=11U,_tmp4B4->f1=new_e;_tmp4B4;});}default: _LL17: _LL18:
# 2564
 if(Cyc_Tcutil_skip_fntypes)return t;else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66A;_tmp66A.tag=0U,({struct _fat_ptr _tmp7A2=({const char*_tmp4B7="found typedecltype in rsubs";_tag_fat(_tmp4B7,sizeof(char),28U);});_tmp66A.f1=_tmp7A2;});_tmp66A;});void*_tmp4B5[1U];_tmp4B5[0]=& _tmp4B6;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4B5,sizeof(void*),1U));});}}_LL0:;}
# 2569
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2572
if(ts == 0)
return 0;{
void*_tmp4B8=(void*)ts->hd;void*old_hd=_tmp4B8;
struct Cyc_List_List*_tmp4B9=ts->tl;struct Cyc_List_List*old_tl=_tmp4B9;
void*_tmp4BA=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp4BA;
struct Cyc_List_List*_tmp4BB=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp4BB;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp4BC=_cycalloc(sizeof(*_tmp4BC));_tmp4BC->hd=new_hd,_tmp4BC->tl=new_tl;_tmp4BC;});}}
# 2583
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return inst == 0?t: Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}
# 2587
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t){
Cyc_Tcutil_skip_fntypes=1;{
void*_tmp4BD=Cyc_Tcutil_substitute(inst,t);void*new_t=_tmp4BD;
Cyc_Tcutil_skip_fntypes=0;
return new_t;}}
# 2595
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4BE=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4BE;
return({struct _tuple19*_tmp4C0=_cycalloc(sizeof(*_tmp4C0));_tmp4C0->f1=tv,({void*_tmp7A4=({struct Cyc_Core_Opt*_tmp7A3=k;Cyc_Absyn_new_evar(_tmp7A3,({struct Cyc_Core_Opt*_tmp4BF=_cycalloc(sizeof(*_tmp4BF));_tmp4BF->v=s;_tmp4BF;}));});_tmp4C0->f2=_tmp7A4;});_tmp4C0;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2600
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2602
struct _tuple28*_tmp4C1=env;struct _RegionHandle*_tmp4C3;struct Cyc_List_List*_tmp4C2;_LL1: _tmp4C2=_tmp4C1->f1;_tmp4C3=_tmp4C1->f2;_LL2: {struct Cyc_List_List*s=_tmp4C2;struct _RegionHandle*rgn=_tmp4C3;
struct Cyc_Core_Opt*_tmp4C4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4C4;
return({struct _tuple19*_tmp4C6=_region_malloc(rgn,sizeof(*_tmp4C6));_tmp4C6->f1=tv,({void*_tmp7A6=({struct Cyc_Core_Opt*_tmp7A5=k;Cyc_Absyn_new_evar(_tmp7A5,({struct Cyc_Core_Opt*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->v=s;_tmp4C5;}));});_tmp4C6->f2=_tmp7A6;});_tmp4C6;});}}
# 2607
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2609
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66D;_tmp66D.tag=0U,({struct _fat_ptr _tmp7A7=({const char*_tmp4CC="bitfield ";_tag_fat(_tmp4CC,sizeof(char),10U);});_tmp66D.f1=_tmp7A7;});_tmp66D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66C;_tmp66C.tag=0U,_tmp66C.f1=*fn;_tmp66C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66B;_tmp66B.tag=0U,({struct _fat_ptr _tmp7A8=({const char*_tmp4CB=" does not have constant width";_tag_fat(_tmp4CB,sizeof(char),30U);});_tmp66B.f1=_tmp7A8;});_tmp66B;});void*_tmp4C7[3U];_tmp4C7[0]=& _tmp4C8,_tmp4C7[1]=& _tmp4C9,_tmp4C7[2]=& _tmp4CA;({unsigned _tmp7A9=loc;Cyc_Warn_err2(_tmp7A9,_tag_fat(_tmp4C7,sizeof(void*),3U));});});else{
# 2614
struct _tuple13 _tmp4CD=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp69=_tmp4CD;struct _tuple13 _tmp4CE=_stmttmp69;int _tmp4D0;unsigned _tmp4CF;_LL1: _tmp4CF=_tmp4CE.f1;_tmp4D0=_tmp4CE.f2;_LL2: {unsigned i=_tmp4CF;int known=_tmp4D0;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66E;_tmp66E.tag=0U,({struct _fat_ptr _tmp7AA=({const char*_tmp4D3="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4D3,sizeof(char),47U);});_tmp66E.f1=_tmp7AA;});_tmp66E;});void*_tmp4D1[1U];_tmp4D1[0]=& _tmp4D2;({unsigned _tmp7AB=loc;Cyc_Warn_warn2(_tmp7AB,_tag_fat(_tmp4D1,sizeof(void*),1U));});});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66F;_tmp66F.tag=0U,({struct _fat_ptr _tmp7AC=({const char*_tmp4D6="bitfield has negative width";_tag_fat(_tmp4D6,sizeof(char),28U);});_tmp66F.f1=_tmp7AC;});_tmp66F;});void*_tmp4D4[1U];_tmp4D4[0]=& _tmp4D5;({unsigned _tmp7AD=loc;Cyc_Warn_err2(_tmp7AD,_tag_fat(_tmp4D4,sizeof(void*),1U));});});
w=i;}}{
# 2621
void*_tmp4D7=Cyc_Tcutil_compress(field_type);void*_stmttmp6A=_tmp4D7;void*_tmp4D8=_stmttmp6A;enum Cyc_Absyn_Size_of _tmp4D9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->f1)->tag == 1U){_LL4: _tmp4D9=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D8)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4D9;
# 2624
int bad;
{enum Cyc_Absyn_Size_of _tmp4DA=b;switch(_tmp4DA){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 bad=w > (unsigned)8;goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 bad=w > (unsigned)16;goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 bad=w > (unsigned)32;goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 bad=w > (unsigned)64;goto _LL8;}_LL8:;}
# 2633
if(bad)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp670;_tmp670.tag=0U,({struct _fat_ptr _tmp7AE=({const char*_tmp4DD="bitfield larger than type";_tag_fat(_tmp4DD,sizeof(char),26U);});_tmp670.f1=_tmp7AE;});_tmp670;});void*_tmp4DB[1U];_tmp4DB[0]=& _tmp4DC;({unsigned _tmp7AF=loc;Cyc_Warn_warn2(_tmp7AF,_tag_fat(_tmp4DB,sizeof(void*),1U));});});
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2637
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp674;_tmp674.tag=0U,({struct _fat_ptr _tmp7B0=({const char*_tmp4E4="bitfield ";_tag_fat(_tmp4E4,sizeof(char),10U);});_tmp674.f1=_tmp7B0;});_tmp674;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp673;_tmp673.tag=0U,_tmp673.f1=*fn;_tmp673;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp672;_tmp672.tag=0U,({struct _fat_ptr _tmp7B1=({const char*_tmp4E3=" must have integral type but has type ";_tag_fat(_tmp4E3,sizeof(char),39U);});_tmp672.f1=_tmp7B1;});_tmp672;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4E2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp671;_tmp671.tag=2U,_tmp671.f1=(void*)field_type;_tmp671;});void*_tmp4DE[4U];_tmp4DE[0]=& _tmp4DF,_tmp4DE[1]=& _tmp4E0,_tmp4DE[2]=& _tmp4E1,_tmp4DE[3]=& _tmp4E2;({unsigned _tmp7B2=loc;Cyc_Warn_err2(_tmp7B2,_tag_fat(_tmp4DE,sizeof(void*),4U));});});
# 2639
goto _LL3;}_LL3:;}}}
# 2646
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4E5=t;void*_tmp4E7;struct Cyc_Absyn_Typedefdecl*_tmp4E6;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E5)->tag == 8U){_LL1: _tmp4E6=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E5)->f3;_tmp4E7=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E5)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4E6;void*tdopt=_tmp4E7;
# 2649
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp675;_tmp675.tag=0U,({struct _fat_ptr _tmp7B3=({const char*_tmp4EA="extra const";_tag_fat(_tmp4EA,sizeof(char),12U);});_tmp675.f1=_tmp7B3;});_tmp675;});void*_tmp4E8[1U];_tmp4E8[0]=& _tmp4E9;({unsigned _tmp7B4=loc;Cyc_Warn_warn2(_tmp7B4,_tag_fat(_tmp4E8,sizeof(void*),1U));});});
return 1;}
# 2654
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2661
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp7B5=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp7B5;});}
# 2665
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2671
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2674
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp678;_tmp678.tag=0U,_tmp678.f1=msg;_tmp678;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4ED=({struct Cyc_Warn_String_Warn_Warg_struct _tmp677;_tmp677.tag=0U,({struct _fat_ptr _tmp7B6=({const char*_tmp4EF=": ";_tag_fat(_tmp4EF,sizeof(char),3U);});_tmp677.f1=_tmp7B6;});_tmp677;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp676;_tmp676.tag=0U,({struct _fat_ptr _tmp7B7=a2string(vs->hd);_tmp676.f1=_tmp7B7;});_tmp676;});void*_tmp4EB[3U];_tmp4EB[0]=& _tmp4EC,_tmp4EB[1]=& _tmp4ED,_tmp4EB[2]=& _tmp4EE;({unsigned _tmp7B8=loc;Cyc_Warn_err2(_tmp7B8,_tag_fat(_tmp4EB,sizeof(void*),3U));});});}}}
# 2679
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2682
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2685
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp7BA=tvs;unsigned _tmp7B9=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp7BA,_tmp7B9,Cyc_Absynpp_tvar2string,({const char*_tmp4F0="duplicate type variable";_tag_fat(_tmp4F0,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2700 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2705
struct _RegionHandle _tmp4F1=_new_region("temp");struct _RegionHandle*temp=& _tmp4F1;_push_region(temp);
# 2709
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp7BB=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp7BB,({const char*_tmp4F2="";_tag_fat(_tmp4F2,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4F4=_region_malloc(temp,sizeof(*_tmp4F4));({struct _tuple29*_tmp7BC=({struct _tuple29*_tmp4F3=_region_malloc(temp,sizeof(*_tmp4F3));_tmp4F3->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4F3->f2=0;_tmp4F3;});_tmp4F4->hd=_tmp7BC;}),_tmp4F4->tl=fields;_tmp4F4;});}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2715
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp520="struct";_tag_fat(_tmp520,sizeof(char),7U);}):({const char*_tmp521="union";_tag_fat(_tmp521,sizeof(char),6U);});
# 2718
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4F5=(struct _tuple30*)des->hd;struct _tuple30*_stmttmp6B=_tmp4F5;struct _tuple30*_tmp4F6=_stmttmp6B;void*_tmp4F8;struct Cyc_List_List*_tmp4F7;_LL1: _tmp4F7=_tmp4F6->f1;_tmp4F8=_tmp4F6->f2;_LL2: {struct Cyc_List_List*dl=_tmp4F7;void*a=_tmp4F8;
if(dl == 0){
# 2723
struct Cyc_List_List*_tmp4F9=fields;struct Cyc_List_List*fields2=_tmp4F9;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple29*)fields2->hd)).f2){
(*((struct _tuple29*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp7BE=({struct Cyc_List_List*_tmp4FB=_cycalloc(sizeof(*_tmp4FB));({void*_tmp7BD=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4FA=_cycalloc(sizeof(*_tmp4FA));_tmp4FA->tag=1U,_tmp4FA->f1=((*((struct _tuple29*)fields2->hd)).f1)->name;_tmp4FA;});_tmp4FB->hd=_tmp7BD;}),_tmp4FB->tl=0;_tmp4FB;});(*((struct _tuple30*)des->hd)).f1=_tmp7BE;});
ans=({struct Cyc_List_List*_tmp4FD=_region_malloc(rgn,sizeof(*_tmp4FD));({struct _tuple31*_tmp7BF=({struct _tuple31*_tmp4FC=_region_malloc(rgn,sizeof(*_tmp4FC));_tmp4FC->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4FC->f2=a;_tmp4FC;});_tmp4FD->hd=_tmp7BF;}),_tmp4FD->tl=ans;_tmp4FD;});
break;}}
# 2731
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67A;_tmp67A.tag=0U,({struct _fat_ptr _tmp7C0=({const char*_tmp501="too many arguments to ";_tag_fat(_tmp501,sizeof(char),23U);});_tmp67A.f1=_tmp7C0;});_tmp67A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp500=({struct Cyc_Warn_String_Warn_Warg_struct _tmp679;_tmp679.tag=0U,_tmp679.f1=aggr_str;_tmp679;});void*_tmp4FE[2U];_tmp4FE[0]=& _tmp4FF,_tmp4FE[1]=& _tmp500;({unsigned _tmp7C1=loc;Cyc_Warn_err2(_tmp7C1,_tag_fat(_tmp4FE,sizeof(void*),2U));});});}else{
if(dl->tl != 0)
# 2735
({struct Cyc_Warn_String_Warn_Warg_struct _tmp503=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67B;_tmp67B.tag=0U,({struct _fat_ptr _tmp7C2=({const char*_tmp504="multiple designators are not yet supported";_tag_fat(_tmp504,sizeof(char),43U);});_tmp67B.f1=_tmp7C2;});_tmp67B;});void*_tmp502[1U];_tmp502[0]=& _tmp503;({unsigned _tmp7C3=loc;Cyc_Warn_err2(_tmp7C3,_tag_fat(_tmp502,sizeof(void*),1U));});});else{
# 2738
void*_tmp505=(void*)dl->hd;void*_stmttmp6C=_tmp505;void*_tmp506=_stmttmp6C;struct _fat_ptr*_tmp507;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp506)->tag == 0U){_LL4: _LL5:
# 2740
({struct Cyc_Warn_String_Warn_Warg_struct _tmp509=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67D;_tmp67D.tag=0U,({struct _fat_ptr _tmp7C4=({const char*_tmp50B="array designator used in argument to ";_tag_fat(_tmp50B,sizeof(char),38U);});_tmp67D.f1=_tmp7C4;});_tmp67D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp50A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67C;_tmp67C.tag=0U,_tmp67C.f1=aggr_str;_tmp67C;});void*_tmp508[2U];_tmp508[0]=& _tmp509,_tmp508[1]=& _tmp50A;({unsigned _tmp7C5=loc;Cyc_Warn_err2(_tmp7C5,_tag_fat(_tmp508,sizeof(void*),2U));});});
goto _LL3;}else{_LL6: _tmp507=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp506)->f1;_LL7: {struct _fat_ptr*v=_tmp507;
# 2743
struct Cyc_List_List*_tmp50C=fields;struct Cyc_List_List*fields2=_tmp50C;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple29*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple29*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp680;_tmp680.tag=0U,({struct _fat_ptr _tmp7C6=({const char*_tmp512="member ";_tag_fat(_tmp512,sizeof(char),8U);});_tmp680.f1=_tmp7C6;});_tmp680;});struct Cyc_Warn_String_Warn_Warg_struct _tmp50F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67F;_tmp67F.tag=0U,_tmp67F.f1=*v;_tmp67F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp510=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67E;_tmp67E.tag=0U,({struct _fat_ptr _tmp7C7=({const char*_tmp511=" has already been used as an argument";_tag_fat(_tmp511,sizeof(char),38U);});_tmp67E.f1=_tmp7C7;});_tmp67E;});void*_tmp50D[3U];_tmp50D[0]=& _tmp50E,_tmp50D[1]=& _tmp50F,_tmp50D[2]=& _tmp510;({unsigned _tmp7C8=loc;Cyc_Warn_err2(_tmp7C8,_tag_fat(_tmp50D,sizeof(void*),3U));});});
(*((struct _tuple29*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp514=_region_malloc(rgn,sizeof(*_tmp514));({struct _tuple31*_tmp7C9=({struct _tuple31*_tmp513=_region_malloc(rgn,sizeof(*_tmp513));_tmp513->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp513->f2=a;_tmp513;});_tmp514->hd=_tmp7C9;}),_tmp514->tl=ans;_tmp514;});
break;}}
# 2752
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp516=({struct Cyc_Warn_String_Warn_Warg_struct _tmp682;_tmp682.tag=0U,({struct _fat_ptr _tmp7CA=({const char*_tmp518="bad field designator ";_tag_fat(_tmp518,sizeof(char),22U);});_tmp682.f1=_tmp7CA;});_tmp682;});struct Cyc_Warn_String_Warn_Warg_struct _tmp517=({struct Cyc_Warn_String_Warn_Warg_struct _tmp681;_tmp681.tag=0U,_tmp681.f1=*v;_tmp681;});void*_tmp515[2U];_tmp515[0]=& _tmp516,_tmp515[1]=& _tmp517;({unsigned _tmp7CB=loc;Cyc_Warn_err2(_tmp7CB,_tag_fat(_tmp515,sizeof(void*),2U));});});
goto _LL3;}}_LL3:;}}}}
# 2759
if((int)aggr_kind == (int)0U){
if(({int _tmp7CC=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans);_tmp7CC < ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp51A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp683;_tmp683.tag=0U,({struct _fat_ptr _tmp7CD=({const char*_tmp51B="too few arguments to struct";_tag_fat(_tmp51B,sizeof(char),28U);});_tmp683.f1=_tmp7CD;});_tmp683;});void*_tmp519[1U];_tmp519[0]=& _tmp51A;({unsigned _tmp7CE=loc;Cyc_Warn_err2(_tmp7CE,_tag_fat(_tmp519,sizeof(void*),1U));});});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp51D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp684;_tmp684.tag=0U,({struct _fat_ptr _tmp7CF=({const char*_tmp51E="union expression should have one member";_tag_fat(_tmp51E,sizeof(char),40U);});_tmp684.f1=_tmp7CF;});_tmp684;});void*_tmp51C[1U];_tmp51C[0]=& _tmp51D;({unsigned _tmp7D0=loc;Cyc_Warn_err2(_tmp7D0,_tag_fat(_tmp51C,sizeof(void*),1U));});});}{
struct Cyc_List_List*_tmp51F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp51F;}}}
# 2709
;_pop_region();}
# 2770
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2772
void*_tmp522=e1->r;void*_stmttmp6D=_tmp522;void*_tmp523=_stmttmp6D;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp528;struct Cyc_Absyn_Exp*_tmp529;switch(*((int*)_tmp523)){case 14U: _LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp686;_tmp686.tag=0U,({struct _fat_ptr _tmp7D1=({const char*_tmp52D="we have a cast in a lhs: ";_tag_fat(_tmp52D,sizeof(char),26U);});_tmp686.f1=_tmp7D1;});_tmp686;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp52C=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp685;_tmp685.tag=4U,_tmp685.f1=e1;_tmp685;});void*_tmp52A[2U];_tmp52A[0]=& _tmp52B,_tmp52A[1]=& _tmp52C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp52A,sizeof(void*),2U));});case 20U: _LL3: _tmp529=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp523)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp529;
_tmp528=e1a;goto _LL6;}case 23U: _LL5: _tmp528=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp523)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp528;
# 2776
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp527=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp523)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp527;
_tmp526=e1a;goto _LLA;}case 21U: _LL9: _tmp526=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp523)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp526;
# 2780
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp688;_tmp688.tag=0U,({struct _fat_ptr _tmp7D2=({const char*_tmp531="found zero pointer aggregate member assignment: ";_tag_fat(_tmp531,sizeof(char),49U);});_tmp688.f1=_tmp7D2;});_tmp688;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp530=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp687;_tmp687.tag=4U,_tmp687.f1=e1;_tmp687;});void*_tmp52E[2U];_tmp52E[0]=& _tmp52F,_tmp52E[1]=& _tmp530;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp52E,sizeof(void*),2U));});
return 0;}case 13U: _LLB: _tmp525=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp523)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp525;
_tmp524=e1a;goto _LLE;}case 12U: _LLD: _tmp524=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp523)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp524;
# 2785
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp533=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68A;_tmp68A.tag=0U,({struct _fat_ptr _tmp7D3=({const char*_tmp535="found zero pointer instantiate/noinstantiate: ";_tag_fat(_tmp535,sizeof(char),47U);});_tmp68A.f1=_tmp7D3;});_tmp68A;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp534=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp689;_tmp689.tag=4U,_tmp689.f1=e1;_tmp689;});void*_tmp532[2U];_tmp532[0]=& _tmp533,_tmp532[1]=& _tmp534;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp532,sizeof(void*),2U));});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp537=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68C;_tmp68C.tag=0U,({struct _fat_ptr _tmp7D4=({const char*_tmp539="found bad lhs in is_zero_ptr_deref: ";_tag_fat(_tmp539,sizeof(char),37U);});_tmp68C.f1=_tmp7D4;});_tmp68C;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp538=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp68B;_tmp68B.tag=4U,_tmp68B.f1=e1;_tmp68B;});void*_tmp536[2U];_tmp536[0]=& _tmp537,_tmp536[1]=& _tmp538;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp536,sizeof(void*),2U));});}_LL0:;}
# 2799
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmp53A=Cyc_Tcutil_compress(r);void*_stmttmp6E=_tmp53A;void*_tmp53B=_stmttmp6E;struct Cyc_Absyn_Tvar*_tmp53C;enum Cyc_Absyn_AliasQual _tmp53E;enum Cyc_Absyn_KindQual _tmp53D;switch(*((int*)_tmp53B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53B)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53B)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53B)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53B)->f4 == 0){_LL5: _tmp53D=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53B)->f3)->kind)->v)->kind;_tmp53E=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53B)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp53D;enum Cyc_Absyn_AliasQual a=_tmp53E;
# 2804
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp53C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53B)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp53C;
# 2808
struct Cyc_Absyn_Kind*_tmp53F=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp6F=_tmp53F;struct Cyc_Absyn_Kind*_tmp540=_stmttmp6F;enum Cyc_Absyn_AliasQual _tmp542;enum Cyc_Absyn_KindQual _tmp541;_LLC: _tmp541=_tmp540->kind;_tmp542=_tmp540->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp541;enum Cyc_Absyn_AliasQual a=_tmp542;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp543=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp70=_tmp543;void*_tmp544=_stmttmp70;struct Cyc_Core_Opt**_tmp545;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp545=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp544)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp545;
# 2812
({struct Cyc_Core_Opt*_tmp7D6=({struct Cyc_Core_Opt*_tmp547=_cycalloc(sizeof(*_tmp547));({void*_tmp7D5=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp546=_cycalloc(sizeof(*_tmp546));_tmp546->tag=2U,_tmp546->f1=0,_tmp546->f2=& Cyc_Tcutil_rk;_tmp546;});_tmp547->v=_tmp7D5;});_tmp547;});*x=_tmp7D6;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2817
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2824
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp548=Cyc_Tcutil_compress(t);void*_stmttmp71=_tmp548;void*_tmp549=_stmttmp71;struct Cyc_Absyn_Tvar*_tmp54A;void*_tmp54B;switch(*((int*)_tmp549)){case 3U: _LL1: _tmp54B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp549)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp54B;
# 2827
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp54A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp549)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp54A;
# 2829
struct Cyc_Absyn_Kind*_tmp54C=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp72=_tmp54C;struct Cyc_Absyn_Kind*_tmp54D=_stmttmp72;enum Cyc_Absyn_AliasQual _tmp54F;enum Cyc_Absyn_KindQual _tmp54E;_LL8: _tmp54E=_tmp54D->kind;_tmp54F=_tmp54D->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp54E;enum Cyc_Absyn_AliasQual a=_tmp54F;
enum Cyc_Absyn_KindQual _tmp550=k;switch(_tmp550){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp551=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp73=_tmp551;void*_tmp552=_stmttmp73;enum Cyc_Absyn_KindQual _tmp554;struct Cyc_Core_Opt**_tmp553;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp552)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp552)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp553=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp552)->f1;_tmp554=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp552)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp553;enum Cyc_Absyn_KindQual k=_tmp554;
# 2835
({struct Cyc_Core_Opt*_tmp7D9=({struct Cyc_Core_Opt*_tmp557=_cycalloc(sizeof(*_tmp557));({void*_tmp7D8=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp556=_cycalloc(sizeof(*_tmp556));_tmp556->tag=2U,_tmp556->f1=0,({struct Cyc_Absyn_Kind*_tmp7D7=({struct Cyc_Absyn_Kind*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555->kind=k,_tmp555->aliasqual=Cyc_Absyn_Aliasable;_tmp555;});_tmp556->f2=_tmp7D7;});_tmp556;});_tmp557->v=_tmp7D8;});_tmp557;});*x=_tmp7D9;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
 return 1;}_LL13:;}
# 2840
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2843
 return 0;}_LL0:;}
# 2846
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(t,0))
return 1;{
void*_tmp558=t;struct Cyc_List_List*_tmp559;struct Cyc_List_List*_tmp55B;union Cyc_Absyn_DatatypeFieldInfo _tmp55A;struct Cyc_List_List*_tmp55D;struct Cyc_Absyn_Aggrdecl**_tmp55C;struct Cyc_List_List*_tmp55E;switch(*((int*)_tmp558)){case 6U: _LL1: _tmp55E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp558)->f1;_LL2: {struct Cyc_List_List*qts=_tmp55E;
# 2852
for(0;qts != 0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp55C=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f1)->f1).KnownAggr).val;_tmp55D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp55C;struct Cyc_List_List*ts=_tmp55D;
# 2857
if((*adp)->impl == 0)
return 0;{
struct Cyc_List_List*_tmp55F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp55F;
{struct Cyc_List_List*_tmp560=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp560;for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type)))
return 1;}}
return 0;}}}else{_LL7: _LL8:
# 2869
 goto _LLA;}case 18U: _LL9: _LLA:
 return 0;case 19U: _LLB: _tmp55A=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f1)->f1;_tmp55B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp55A;struct Cyc_List_List*ts=_tmp55B;
# 2873
union Cyc_Absyn_DatatypeFieldInfo _tmp561=tinfo;struct Cyc_Absyn_Datatypefield*_tmp563;struct Cyc_Absyn_Datatypedecl*_tmp562;if((_tmp561.UnknownDatatypefield).tag == 1){_LL10: _LL11:
 return 0;}else{_LL12: _tmp562=((_tmp561.KnownDatatypefield).val).f1;_tmp563=((_tmp561.KnownDatatypefield).val).f2;_LL13: {struct Cyc_Absyn_Datatypedecl*td=_tmp562;struct Cyc_Absyn_Datatypefield*fld=_tmp563;
# 2876
struct Cyc_List_List*_tmp564=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp564;
{struct Cyc_List_List*_tmp565=fld->typs;struct Cyc_List_List*typs=_tmp565;for(0;typs != 0;typs=typs->tl){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 2882
return 0;}}_LLF:;}default: goto _LLD;}case 7U: _LL5: _tmp559=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp558)->f2;_LL6: {struct Cyc_List_List*x=_tmp559;
# 2865
for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD: _LLE:
# 2884
 return 0;}_LL0:;}}
# 2891
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp566=e->r;void*_stmttmp74=_tmp566;void*_tmp567=_stmttmp74;struct Cyc_Absyn_Stmt*_tmp568;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_Absyn_Exp*_tmp56B;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_Absyn_Exp*_tmp56D;struct _fat_ptr*_tmp570;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Exp*_tmp571;struct Cyc_Absyn_Exp*_tmp572;switch(*((int*)_tmp567)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp567)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp572=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp572;
_tmp571=e1;goto _LL6;}case 20U: _LL5: _tmp571=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp571;
# 2896
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp56F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_tmp570=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp56F;struct _fat_ptr*f=_tmp570;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp56D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_tmp56E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp56D;struct Cyc_Absyn_Exp*e2=_tmp56E;
# 2899
void*_tmp573=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp75=_tmp573;void*_tmp574=_stmttmp75;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp574)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp56B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_tmp56C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp567)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp56B;struct Cyc_Absyn_Exp*e2=_tmp56C;
# 2904
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp56A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp56A;
_tmp569=e2;goto _LL10;}case 14U: _LLF: _tmp569=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp569;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp568=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp568;
# 2908
while(1){
void*_tmp575=s->r;void*_stmttmp76=_tmp575;void*_tmp576=_stmttmp76;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_Absyn_Stmt*_tmp579;struct Cyc_Absyn_Decl*_tmp578;struct Cyc_Absyn_Stmt*_tmp57B;struct Cyc_Absyn_Stmt*_tmp57A;switch(*((int*)_tmp576)){case 2U: _LL1B: _tmp57A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp57B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp57A;struct Cyc_Absyn_Stmt*s2=_tmp57B;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp578=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp579=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp578;struct Cyc_Absyn_Stmt*s1=_tmp579;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp577=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp577;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp57D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68D;_tmp68D.tag=0U,({struct _fat_ptr _tmp7DA=({const char*_tmp57E="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp57E,sizeof(char),40U);});_tmp68D.f1=_tmp7DA;});_tmp68D;});void*_tmp57C[1U];_tmp57C[0]=& _tmp57D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp57C,sizeof(void*),1U));});}_LL1A:;}}default: _LL13: _LL14:
# 2916
 return 1;}_LL0:;}
# 2933 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 2935
struct _tuple14 bogus_ans=({struct _tuple14 _tmp69B;_tmp69B.f1=0,_tmp69B.f2=Cyc_Absyn_heap_rgn_type;_tmp69B;});
void*_tmp57F=e->r;void*_stmttmp77=_tmp57F;void*_tmp580=_stmttmp77;struct Cyc_Absyn_Exp*_tmp582;struct Cyc_Absyn_Exp*_tmp581;struct Cyc_Absyn_Exp*_tmp583;int _tmp586;struct _fat_ptr*_tmp585;struct Cyc_Absyn_Exp*_tmp584;int _tmp589;struct _fat_ptr*_tmp588;struct Cyc_Absyn_Exp*_tmp587;void*_tmp58A;switch(*((int*)_tmp580)){case 1U: _LL1: _tmp58A=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp580)->f1;_LL2: {void*x=_tmp58A;
# 2939
void*_tmp58B=x;struct Cyc_Absyn_Vardecl*_tmp58C;struct Cyc_Absyn_Vardecl*_tmp58D;struct Cyc_Absyn_Vardecl*_tmp58E;struct Cyc_Absyn_Vardecl*_tmp58F;switch(*((int*)_tmp58B)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp58F=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp58B)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp58F;
# 2943
void*_tmp590=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp78=_tmp590;void*_tmp591=_stmttmp78;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp591)->tag == 4U){_LL1B: _LL1C:
# 2945
 return({struct _tuple14 _tmp68E;_tmp68E.f1=1,_tmp68E.f2=Cyc_Absyn_heap_rgn_type;_tmp68E;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp68F;_tmp68F.f1=(vd->tq).real_const,_tmp68F.f2=Cyc_Absyn_heap_rgn_type;_tmp68F;});}_LL1A:;}case 4U: _LL14: _tmp58E=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp58B)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp58E;
# 2949
void*_tmp592=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp79=_tmp592;void*_tmp593=_stmttmp79;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp593)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp690;_tmp690.f1=1,_tmp690.f2=(void*)_check_null(vd->rgn);_tmp690;});}else{_LL22: _LL23:
# 2952
 vd->escapes=1;
return({struct _tuple14 _tmp691;_tmp691.f1=(vd->tq).real_const,_tmp691.f2=(void*)_check_null(vd->rgn);_tmp691;});}_LL1F:;}case 5U: _LL16: _tmp58D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp58B)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp58D;
# 2955
_tmp58C=vd;goto _LL19;}default: _LL18: _tmp58C=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp58B)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp58C;
# 2957
vd->escapes=1;
return({struct _tuple14 _tmp692;_tmp692.f1=(vd->tq).real_const,_tmp692.f2=(void*)_check_null(vd->rgn);_tmp692;});}}_LLD:;}case 21U: _LL3: _tmp587=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp580)->f1;_tmp588=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp580)->f2;_tmp589=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp580)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp587;struct _fat_ptr*f=_tmp588;int is_tagged=_tmp589;
# 2962
if(is_tagged)return bogus_ans;{
# 2965
void*_tmp594=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7A=_tmp594;void*_tmp595=_stmttmp7A;struct Cyc_Absyn_Aggrdecl*_tmp596;struct Cyc_List_List*_tmp597;switch(*((int*)_tmp595)){case 7U: _LL25: _tmp597=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp595)->f2;_LL26: {struct Cyc_List_List*fs=_tmp597;
# 2967
struct Cyc_Absyn_Aggrfield*_tmp598=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp598;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp599=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7B=_tmp599;struct _tuple14 _tmp59A=_stmttmp7B;void*_tmp59C;int _tmp59B;_LL2C: _tmp59B=_tmp59A.f1;_tmp59C=_tmp59A.f2;_LL2D: {int c=_tmp59B;void*t=_tmp59C;
return({struct _tuple14 _tmp693;_tmp693.f1=(finfo->tq).real_const || c,_tmp693.f2=t;_tmp693;});}}
# 2972
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp595)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp595)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp596=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp595)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp596;
# 2974
struct Cyc_Absyn_Aggrfield*_tmp59D=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp59D;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp59E=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7C=_tmp59E;struct _tuple14 _tmp59F=_stmttmp7C;void*_tmp5A1;int _tmp5A0;_LL2F: _tmp5A0=_tmp59F.f1;_tmp5A1=_tmp59F.f2;_LL30: {int c=_tmp5A0;void*t=_tmp5A1;
return({struct _tuple14 _tmp694;_tmp694.f1=(finfo->tq).real_const || c,_tmp694.f2=t;_tmp694;});}}
# 2979
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp584=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp580)->f1;_tmp585=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp580)->f2;_tmp586=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp580)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp584;struct _fat_ptr*f=_tmp585;int is_tagged=_tmp586;
# 2984
if(is_tagged)return bogus_ans;{
# 2987
void*_tmp5A2=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7D=_tmp5A2;void*_tmp5A3=_stmttmp7D;void*_tmp5A5;void*_tmp5A4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->tag == 3U){_LL32: _tmp5A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).elt_type;_tmp5A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp5A4;void*r=_tmp5A5;
# 2989
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp5A6=Cyc_Tcutil_compress(t1);void*_stmttmp7E=_tmp5A6;void*_tmp5A7=_stmttmp7E;struct Cyc_Absyn_Aggrdecl*_tmp5A8;struct Cyc_List_List*_tmp5A9;switch(*((int*)_tmp5A7)){case 7U: _LL37: _tmp5A9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5A7)->f2;_LL38: {struct Cyc_List_List*fs=_tmp5A9;
# 2992
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5A7)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5A7)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp5A8=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5A7)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp5A8;
# 2994
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 2997
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp695;_tmp695.f1=(finfo->tq).real_const,_tmp695.f2=r;_tmp695;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp583=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp580)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp583;
# 3004
void*_tmp5AA=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7F=_tmp5AA;void*_tmp5AB=_stmttmp7F;void*_tmp5AD;struct Cyc_Absyn_Tqual _tmp5AC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AB)->tag == 3U){_LL3E: _tmp5AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AB)->f1).elt_tq;_tmp5AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AB)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp5AC;void*r=_tmp5AD;
# 3006
return({struct _tuple14 _tmp696;_tmp696.f1=tq.real_const,_tmp696.f2=r;_tmp696;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp581=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp580)->f1;_tmp582=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp580)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp581;struct Cyc_Absyn_Exp*e2=_tmp582;
# 3012
void*t=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*_tmp5AE=t;struct Cyc_Absyn_Tqual _tmp5AF;void*_tmp5B1;struct Cyc_Absyn_Tqual _tmp5B0;struct Cyc_List_List*_tmp5B2;switch(*((int*)_tmp5AE)){case 6U: _LL43: _tmp5B2=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5AE)->f1;_LL44: {struct Cyc_List_List*ts=_tmp5B2;
# 3016
struct _tuple13 _tmp5B3=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp80=_tmp5B3;struct _tuple13 _tmp5B4=_stmttmp80;int _tmp5B6;unsigned _tmp5B5;_LL4C: _tmp5B5=_tmp5B4.f1;_tmp5B6=_tmp5B4.f2;_LL4D: {unsigned i=_tmp5B5;int known=_tmp5B6;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp5B7=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp5B7;
if(finfo != 0)
return({struct _tuple14 _tmp697;_tmp697.f1=((*finfo).f1).real_const,({void*_tmp7DB=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp697.f2=_tmp7DB;});_tmp697;});
return bogus_ans;}}}case 3U: _LL45: _tmp5B0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AE)->f1).elt_tq;_tmp5B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AE)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp5B0;void*r=_tmp5B1;
# 3024
return({struct _tuple14 _tmp698;_tmp698.f1=tq.real_const,_tmp698.f2=r;_tmp698;});}case 4U: _LL47: _tmp5AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5AE)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp5AF;
# 3030
return({struct _tuple14 _tmp699;_tmp699.f1=tq.real_const,({void*_tmp7DC=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp699.f2=_tmp7DC;});_tmp699;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3034
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69A;_tmp69A.tag=0U,({struct _fat_ptr _tmp7DD=({const char*_tmp5BA="unary & applied to non-lvalue";_tag_fat(_tmp5BA,sizeof(char),30U);});_tmp69A.f1=_tmp7DD;});_tmp69A;});void*_tmp5B8[1U];_tmp5B8[0]=& _tmp5B9;({unsigned _tmp7DE=e->loc;Cyc_Warn_err2(_tmp7DE,_tag_fat(_tmp5B8,sizeof(void*),1U));});});
return bogus_ans;}_LL0:;}
# 3041
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp5BB=({void*_tmp7DF=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7DF,b);});struct Cyc_Absyn_Exp*eopt=_tmp5BB;
if(eopt == 0)return;{
struct _tuple13 _tmp5BC=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp81=_tmp5BC;struct _tuple13 _tmp5BD=_stmttmp81;int _tmp5BF;unsigned _tmp5BE;_LL1: _tmp5BE=_tmp5BD.f1;_tmp5BF=_tmp5BD.f2;_LL2: {unsigned j=_tmp5BE;int known=_tmp5BF;
if(known && j <= i)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69F;_tmp69F.tag=0U,({struct _fat_ptr _tmp7E0=({const char*_tmp5C6="a dereference is out of bounds: ";_tag_fat(_tmp5C6,sizeof(char),33U);});_tmp69F.f1=_tmp7E0;});_tmp69F;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp5C2=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp69E;_tmp69E.tag=12U,_tmp69E.f1=(int)j;_tmp69E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5C3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69D;_tmp69D.tag=0U,({
struct _fat_ptr _tmp7E1=({const char*_tmp5C5=" <= ";_tag_fat(_tmp5C5,sizeof(char),5U);});_tmp69D.f1=_tmp7E1;});_tmp69D;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp5C4=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp69C;_tmp69C.tag=12U,_tmp69C.f1=(int)i;_tmp69C;});void*_tmp5C0[4U];_tmp5C0[0]=& _tmp5C1,_tmp5C0[1]=& _tmp5C2,_tmp5C0[2]=& _tmp5C3,_tmp5C0[3]=& _tmp5C4;({void(*_tmp7E3)(unsigned,struct _fat_ptr)=
# 3046
do_warn?Cyc_Warn_warn2: Cyc_Warn_err2;unsigned _tmp7E2=loc;_tmp7E3(_tmp7E2,_tag_fat(_tmp5C0,sizeof(void*),4U));});});
# 3048
return;}}}
# 3051
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3059
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
int(*_tmp5C7)(int var_okay,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_cnst_exp;int(*rec)(int var_okay,struct Cyc_Absyn_Exp*e)=_tmp5C7;
void*_tmp5C8=e->r;void*_stmttmp82=_tmp5C8;void*_tmp5C9=_stmttmp82;struct Cyc_List_List*_tmp5CA;struct Cyc_List_List*_tmp5CB;struct Cyc_List_List*_tmp5CC;struct Cyc_List_List*_tmp5CD;struct Cyc_List_List*_tmp5CE;struct Cyc_List_List*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D4;struct Cyc_Absyn_Exp*_tmp5D5;struct Cyc_Absyn_Exp*_tmp5D6;struct Cyc_Absyn_Exp*_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DA;void*_tmp5DD;switch(*((int*)_tmp5C9)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5DD=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL10: {void*b=_tmp5DD;
# 3072
void*_tmp5DE=b;struct Cyc_Absyn_Vardecl*_tmp5DF;struct Cyc_Absyn_Vardecl*_tmp5E0;switch(*((int*)_tmp5DE)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmp5E0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5DE)->f1;_LL35: {struct Cyc_Absyn_Vardecl*vd=_tmp5E0;
# 3075
void*_tmp5E1=Cyc_Tcutil_compress(vd->type);void*_stmttmp83=_tmp5E1;void*_tmp5E2=_stmttmp83;switch(*((int*)_tmp5E2)){case 4U: _LL3D: _LL3E:
 goto _LL40;case 5U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmp5DF=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5DE)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5DF;
# 3081
if((int)vd->sc != (int)Cyc_Absyn_Static)
return var_okay;{
void*_tmp5E3=Cyc_Tcutil_compress(vd->type);void*_stmttmp84=_tmp5E3;void*_tmp5E4=_stmttmp84;switch(*((int*)_tmp5E4)){case 4U: _LL44: _LL45:
 goto _LL47;case 5U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}}case 0U: _LL38: _LL39:
# 3088
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmp5DA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_tmp5DB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C9)->f2;_tmp5DC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C9)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5DA;struct Cyc_Absyn_Exp*e2=_tmp5DB;struct Cyc_Absyn_Exp*e3=_tmp5DC;
# 3092
return(rec(0,e1)&& rec(0,e2))&& rec(0,e3);}case 9U: _LL13: _tmp5D8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_tmp5D9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5C9)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5D8;struct Cyc_Absyn_Exp*e2=_tmp5D9;
return rec(0,e1)&& rec(0,e2);}case 41U: _LL15: _tmp5D7=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5D7;
_tmp5D6=e2;goto _LL18;}case 12U: _LL17: _tmp5D6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5D6;
_tmp5D5=e2;goto _LL1A;}case 13U: _LL19: _tmp5D5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5D5;
_tmp5D4=e2;goto _LL1C;}case 14U: _LL1B: _tmp5D4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C9)->f2;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp5D4;
# 3099
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 15U: _LL1D: _tmp5D3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL1E: {struct Cyc_Absyn_Exp*e2=_tmp5D3;
return rec(1,e2);}case 27U: _LL1F: _tmp5D1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C9)->f2;_tmp5D2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C9)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp5D1;struct Cyc_Absyn_Exp*e2=_tmp5D2;
return rec(0,e1)&& rec(0,e2);}case 28U: _LL21: _tmp5D0=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp5D0;
return rec(0,e);}case 26U: _LL23: _tmp5CF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL24: {struct Cyc_List_List*des=_tmp5CF;
_tmp5CE=des;goto _LL26;}case 30U: _LL25: _tmp5CE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5C9)->f2;_LL26: {struct Cyc_List_List*des=_tmp5CE;
_tmp5CD=des;goto _LL28;}case 29U: _LL27: _tmp5CD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5C9)->f3;_LL28: {struct Cyc_List_List*des=_tmp5CD;
# 3106
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL29: _tmp5CC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5C9)->f2;_LL2A: {struct Cyc_List_List*es=_tmp5CC;
_tmp5CB=es;goto _LL2C;}case 24U: _LL2B: _tmp5CB=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL2C: {struct Cyc_List_List*es=_tmp5CB;
_tmp5CA=es;goto _LL2E;}case 31U: _LL2D: _tmp5CA=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5C9)->f1;_LL2E: {struct Cyc_List_List*es=_tmp5CA;
# 3113
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 3120
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3124
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5E5=Cyc_Tcutil_compress(t);void*_stmttmp85=_tmp5E5;void*_tmp5E6=_stmttmp85;struct Cyc_List_List*_tmp5E7;struct Cyc_List_List*_tmp5E8;void*_tmp5E9;void*_tmp5EA;struct Cyc_List_List*_tmp5EC;void*_tmp5EB;switch(*((int*)_tmp5E6)){case 0U: _LL1: _tmp5EB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E6)->f1;_tmp5EC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E6)->f2;_LL2: {void*c=_tmp5EB;struct Cyc_List_List*ts=_tmp5EC;
# 3128
void*_tmp5ED=c;union Cyc_Absyn_AggrInfo _tmp5EE;struct Cyc_List_List*_tmp5EF;struct Cyc_Absyn_Enumdecl*_tmp5F0;switch(*((int*)_tmp5ED)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5F0=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5ED)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5F0;
# 3133
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5EF=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5EF=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5ED)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5EF;
# 3137
{struct Cyc_List_List*_tmp5F1=fs;struct Cyc_List_List*fs2=_tmp5F1;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5F2=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp86=_tmp5F2;struct _tuple13 _tmp5F3=_stmttmp86;int _tmp5F5;unsigned _tmp5F4;_LL1D: _tmp5F4=_tmp5F3.f1;_tmp5F5=_tmp5F3.f2;_LL1E: {unsigned i=_tmp5F4;int known=_tmp5F5;
if(known && i == (unsigned)0)
return 1;}}}}
# 3144
return 0;}case 20U: _LL18: _tmp5EE=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5ED)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5EE;
# 3147
struct Cyc_Absyn_Aggrdecl*_tmp5F6=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5F6;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E6)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5EA;
# 3155
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E6)->f1).elt_type;_LL6: {void*t=_tmp5E9;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5E8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E6)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5E8;
# 3158
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7U: _LL9: _tmp5E7=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E6)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5E7;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3166
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3168
struct _RegionHandle _tmp5F7=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5F7;_push_region(rgn);
{struct Cyc_List_List*_tmp5F8=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5F8;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5F9=0;_npop_handler(0U);return _tmp5F9;}}{
# 3176
int _tmp5FA=1;_npop_handler(0U);return _tmp5FA;}}
# 3169
;_pop_region();}
# 3180
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5FB=t;struct Cyc_Absyn_Typedefdecl*_tmp5FC;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5FB)->tag == 8U){_LL1: _tmp5FC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5FB)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5FC;
# 3183
if(tdopt == 0)
return;{
struct Cyc_Absyn_Tqual _tmp5FD=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5FD;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A2;_tmp6A2.tag=0U,({struct _fat_ptr _tmp7E4=({const char*_tmp603="qualifier within typedef type ";_tag_fat(_tmp603,sizeof(char),31U);});_tmp6A2.f1=_tmp7E4;});_tmp6A2;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp600=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6A1;_tmp6A1.tag=2U,_tmp6A1.f1=(void*)t;_tmp6A1;});struct Cyc_Warn_String_Warn_Warg_struct _tmp601=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A0;_tmp6A0.tag=0U,({struct _fat_ptr _tmp7E5=({const char*_tmp602=" is ignored";_tag_fat(_tmp602,sizeof(char),12U);});_tmp6A0.f1=_tmp7E5;});_tmp6A0;});void*_tmp5FE[3U];_tmp5FE[0]=& _tmp5FF,_tmp5FE[1]=& _tmp600,_tmp5FE[2]=& _tmp601;({unsigned _tmp7E6=loc;Cyc_Warn_warn2(_tmp7E6,_tag_fat(_tmp5FE,sizeof(void*),3U));});});
return;}}}else{_LL3: _LL4:
 return;}_LL0:;}
# 3194
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp604=Cyc_Tcutil_compress(t);void*_stmttmp87=_tmp604;void*_tmp605=_stmttmp87;struct Cyc_Absyn_Exp*_tmp606;struct Cyc_Absyn_PtrInfo _tmp607;switch(*((int*)_tmp605)){case 3U: _LL1: _tmp607=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp605)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp607;
return({void*_tmp7E7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7E7,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp606=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp605)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp606;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3204
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp608=b;struct Cyc_Absyn_Vardecl*_tmp609;struct Cyc_Absyn_Vardecl*_tmp60A;struct Cyc_Absyn_Vardecl*_tmp60B;struct Cyc_Absyn_Vardecl*_tmp60C;switch(*((int*)_tmp608)){case 5U: _LL1: _tmp60C=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp608)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp60C;
_tmp60B=x;goto _LL4;}case 4U: _LL3: _tmp60B=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp608)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp60B;
_tmp60A=x;goto _LL6;}case 3U: _LL5: _tmp60A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp608)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp60A;
_tmp609=x;goto _LL8;}case 1U: _LL7: _tmp609=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp608)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp609;
return x->escapes?0: x;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3215
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp60D=0;struct Cyc_List_List*res=_tmp60D;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)res=({struct Cyc_List_List*_tmp60E=_cycalloc(sizeof(*_tmp60E));_tmp60E->hd=*((void**)_check_null((void**)x->hd)),_tmp60E->tl=res;_tmp60E;});}}
return res;}
# 3222
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp60F=Cyc_Tcutil_compress(t);void*_stmttmp88=_tmp60F;void*_tmp610=_stmttmp88;unsigned _tmp615;void*_tmp614;struct Cyc_Absyn_Exp*_tmp613;struct Cyc_Absyn_Tqual _tmp612;void*_tmp611;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp610)->tag == 4U){_LL1: _tmp611=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp610)->f1).elt_type;_tmp612=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp610)->f1).tq;_tmp613=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp610)->f1).num_elts;_tmp614=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp610)->f1).zero_term;_tmp615=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp610)->f1).zt_loc;_LL2: {void*et=_tmp611;struct Cyc_Absyn_Tqual tq=_tmp612;struct Cyc_Absyn_Exp*eopt=_tmp613;void*zt=_tmp614;unsigned ztl=_tmp615;
# 3225
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
if(convert_tag){
if(eopt->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp617=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6A3;_tmp6A3.tag=0U,({struct _fat_ptr _tmp7E8=({const char*_tmp618="cannot convert tag without type!";_tag_fat(_tmp618,sizeof(char),33U);});_tmp6A3.f1=_tmp7E8;});_tmp6A3;});void*_tmp616[1U];_tmp616[0]=& _tmp617;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp616,sizeof(void*),1U));});{
void*_tmp619=Cyc_Tcutil_compress((void*)_check_null(eopt->topt));void*_stmttmp89=_tmp619;void*_tmp61A=_stmttmp89;void*_tmp61B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp61A)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp61A)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp61A)->f2 != 0){_LL6: _tmp61B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp61A)->f2)->hd;_LL7: {void*t=_tmp61B;
# 3234
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3239
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
