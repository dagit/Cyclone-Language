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
# 900
void*Cyc_Absyn_compress_kb(void*);
# 904
void*Cyc_Absyn_compress(void*);
# 909
int Cyc_Absyn_type2bool(int def,void*);
# 918
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 923
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 925
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 930
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 932
extern void*Cyc_Absyn_empty_effect;
# 934
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 936
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 961
extern void*Cyc_Absyn_fat_bound_type;
# 963
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 965
void*Cyc_Absyn_bounds_one (void);
# 967
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 971
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 990
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 992
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 995
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1005
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1018
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1028
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1030
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1037
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
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1058
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1097
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1141
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1143
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1145
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1155
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
void*Cyc_Warn_impos2(struct _fat_ptr);
# 47 "flags.h"
extern int Cyc_Flags_warn_alias_coerce;
# 51
extern int Cyc_Flags_warn_region_coerce;
# 73
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_EvalOrder =11U,Cyc_Flags_CCompiler =12U,Cyc_Flags_AllPasses =13U};struct _tuple13{unsigned f1;int f2;};
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
# 99
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*,struct Cyc_Absyn_Kind*);
# 104
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Kind*def);
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*);
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 108
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
# 111
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 115
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 117
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 119
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*assume,void*t1,void*t2);
# 122
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);
# 128
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 137
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 142
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 147
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_boolko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ptrbko;
# 156
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 161
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 166
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 175
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 178
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 181
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 187
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 190
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 194
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 214
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 227
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 230
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 235
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 240
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple14{int f1;void*f2;};
# 244
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 247
void*Cyc_Tcutil_normalize_effect(void*e);
# 250
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 252
int Cyc_Tcutil_new_tvar_id (void);
# 254
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 260
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 264
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 278
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 281
int Cyc_Tcutil_zeroable_type(void*);
# 285
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 293
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 297
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 302
int Cyc_Tcutil_tycon_cmp(void*,void*);
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*,void*);
# 38 "attributes.h"
int Cyc_Atts_sub_attributes(struct Cyc_List_List*,struct Cyc_List_List*);
# 41
int Cyc_Atts_fntype_att(void*);
# 52
int Cyc_Atts_attribute_cmp(void*,void*);
# 38 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Absyn_compress(t);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 44
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Absyn_compress(t);void*_stmttmp1=_tmp2;void*_tmp3=_stmttmp1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 50
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Absyn_compress(t);void*_stmttmp2=_tmp4;void*_tmp5=_stmttmp2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 56
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Absyn_compress(t);void*_stmttmp3=_tmp6;void*_tmp7=_stmttmp3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 63
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Absyn_compress(t);void*_stmttmp4=_tmp8;void*_tmp9=_stmttmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 70
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Absyn_compress(t);void*_stmttmp5=_tmpA;void*_tmpB=_stmttmp5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 77
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Absyn_compress(t);void*_stmttmp6=_tmpC;void*_tmpD=_stmttmp6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 84
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Absyn_compress(t);void*_stmttmp7=_tmpE;void*_tmpF=_stmttmp7;void*_tmp10;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {void*c=_tmp10;
# 87
void*_tmp11=c;switch(*((int*)_tmp11)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 94
 return 0;}_LL0:;}
# 97
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Absyn_compress(t);void*_stmttmp8=_tmp12;void*_tmp13=_stmttmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
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
void*_tmp14=Cyc_Absyn_compress(t);void*_stmttmp9=_tmp14;void*_tmp15=_stmttmp9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 116
int Cyc_Tcutil_is_typedef_type(void*t){
void*_tmp16=Cyc_Absyn_compress(t);void*_stmttmpA=_tmp16;void*_tmp17=_stmttmpA;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 122
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 130
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp18=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpB=_tmp18;void*_tmp19=_stmttmpB;void*_tmp1A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19)->tag == 1U){_LL1: _LL2:
 return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp1A=_tmp19;_LL4: {void*t=_tmp1A;
return Cyc_Tcutil_is_integral_type(t);}}_LL0:;}
# 138
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp1B=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmpC=_tmp1B;void*_tmp1C=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 148
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1D=Cyc_Absyn_compress(t);void*_stmttmpD=_tmp1D;void*_tmp1E=_stmttmpD;void*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->tag == 3U){_LL1: _tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->f1).ptr_atts).zero_term;_LL2: {void*ztl=_tmp1F;
# 151
return Cyc_Tcutil_force_type2bool(0,ztl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 157
int Cyc_Tcutil_is_nullable_pointer_type(void*t,int def){
void*_tmp20=Cyc_Absyn_compress(t);void*_stmttmpE=_tmp20;void*_tmp21=_stmttmpE;void*_tmp22;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->tag == 3U){_LL1: _tmp22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp22;
# 160
return Cyc_Tcutil_force_type2bool(def,nbl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 166
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp23=Cyc_Absyn_compress(t);void*_stmttmpF=_tmp23;void*_tmp24=_stmttmpF;void*_tmp25;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->tag == 3U){_LL1: _tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp25;
# 169
return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,b);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 176
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp26=Cyc_Absyn_compress(t);void*_stmttmp10=_tmp26;void*_tmp27=_stmttmp10;void*_tmp29;void*_tmp28;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->tag == 3U){_LL1: _tmp28=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).elt_type;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).ptr_atts).bounds;_LL2: {void*elt_type=_tmp28;void*b=_tmp29;
# 179
if(!Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type))
return 0;
*elt_type_dest=elt_type;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 189
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp2A=Cyc_Absyn_compress(t);void*_stmttmp11=_tmp2A;void*_tmp2B=_stmttmp11;void*_tmp2D;void*_tmp2C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->tag == 3U){_LL1: _tmp2C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_type;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;_LL2: {void*elt_type=_tmp2C;void*zt=_tmp2D;
# 192
*elt_type_dest=elt_type;
return Cyc_Absyn_type2bool(0,zt);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 201
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 203
void*_tmp2E=Cyc_Absyn_compress(t);void*_stmttmp12=_tmp2E;void*_tmp2F=_stmttmp12;void*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Tqual _tmp31;void*_tmp30;void*_tmp36;void*_tmp35;void*_tmp34;switch(*((int*)_tmp2F)){case 3U: _LL1: _tmp34=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp35=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).bounds;_tmp36=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).zero_term;_LL2: {void*elt=_tmp34;void*bnds=_tmp35;void*zt=_tmp36;
# 205
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*ptr_type=t;
*elt_type=elt;
{void*_tmp37=Cyc_Absyn_compress(bnds);void*_stmttmp13=_tmp37;void*_tmp38=_stmttmp13;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 213
return 1;}case 4U: _LL3: _tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).tq;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).zero_term;_LL4: {void*elt=_tmp30;struct Cyc_Absyn_Tqual tq=_tmp31;struct Cyc_Absyn_Exp*n=_tmp32;void*zt=_tmp33;
# 215
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*elt_type=elt;
*is_fat=0;
({void*_tmp699=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp699;});
return 1;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 228
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 234
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp39=({void*_tmp69A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp69A,b);});struct Cyc_Absyn_Exp*eopt=_tmp39;
if(eopt == 0)return 0;{
struct _tuple13 _tmp3A=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp14=_tmp3A;struct _tuple13 _tmp3B=_stmttmp14;int _tmp3D;unsigned _tmp3C;_LL1: _tmp3C=_tmp3B.f1;_tmp3D=_tmp3B.f2;_LL2: {unsigned i=_tmp3C;int known=_tmp3D;
return known && i == (unsigned)1;}}}
# 242
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3E=Cyc_Absyn_compress(t);void*_stmttmp15=_tmp3E;void*_tmp3F=_stmttmp15;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;void*_tmp43;void*_tmp42;struct Cyc_List_List*_tmp45;void*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LL1: _tmp44=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f2;_LL2: {void*c=_tmp44;struct Cyc_List_List*ts=_tmp45;
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
# 282
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 284
Cyc_Warn_verr(loc,fmt,ap);}
# 286
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 288
Cyc_Warn_vimpos(fmt,ap);}
# 290
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 292
Cyc_Warn_vwarn(sg,fmt,ap);}
# 296
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 303
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp4C=Cyc_Absyn_compress_kb(kb);void*_stmttmp16=_tmp4C;void*_tmp4D=_stmttmp16;struct Cyc_Absyn_Kind*_tmp4E;switch(*((int*)_tmp4D)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=1U,_tmp4F->f1=0;_tmp4F;});case 2U: _LL3: _tmp4E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4D)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp4E;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=2U,_tmp50->f1=0,_tmp50->f2=k;_tmp50;});}default: _LL5: _LL6:
 return kb;}_LL0:;}
# 310
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->name=tv->name,_tmp51->identity=- 1,({void*_tmp69B=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp51->kind=_tmp69B;});_tmp51;});}
# 313
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9*_tmp52=arg;void*_tmp55;struct Cyc_Absyn_Tqual _tmp54;struct _fat_ptr*_tmp53;_LL1: _tmp53=_tmp52->f1;_tmp54=_tmp52->f2;_tmp55=_tmp52->f3;_LL2: {struct _fat_ptr*x=_tmp53;struct Cyc_Absyn_Tqual y=_tmp54;void*t=_tmp55;
return({struct _tuple9*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->f1=x,_tmp56->f2=y,({void*_tmp69C=Cyc_Tcutil_copy_type(t);_tmp56->f3=_tmp69C;});_tmp56;});}}
# 317
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=(*arg).f1,({void*_tmp69D=Cyc_Tcutil_copy_type((*arg).f2);_tmp57->f2=_tmp69D;});_tmp57;});}
# 320
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
return e == 0?0: Cyc_Tcutil_deep_copy_exp(preserve_types,e);}
# 323
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->name=f->name,_tmp58->tq=f->tq,({void*_tmp69F=Cyc_Tcutil_copy_type(f->type);_tmp58->type=_tmp69F;}),_tmp58->width=f->width,_tmp58->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp69E=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp58->requires_clause=_tmp69E;});_tmp58;});}struct _tuple15{void*f1;void*f2;};
# 327
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp59=x;void*_tmp5B;void*_tmp5A;_LL1: _tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;_LL2: {void*r1=_tmp5A;void*r2=_tmp5B;
return({struct _tuple15*_tmp5C=_cycalloc(sizeof(*_tmp5C));({void*_tmp6A1=Cyc_Tcutil_copy_type(r1);_tmp5C->f1=_tmp6A1;}),({void*_tmp6A0=Cyc_Tcutil_copy_type(r2);_tmp5C->f2=_tmp6A0;});_tmp5C;});}}
# 331
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 335
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp5D=Cyc_Absyn_compress(t);void*_stmttmp17=_tmp5D;void*_tmp5E=_stmttmp17;struct Cyc_Absyn_Datatypedecl*_tmp5F;struct Cyc_Absyn_Enumdecl*_tmp60;struct Cyc_Absyn_Aggrdecl*_tmp61;struct Cyc_Absyn_Typedefdecl*_tmp64;struct Cyc_List_List*_tmp63;struct _tuple1*_tmp62;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_Absyn_Exp*_tmp66;struct Cyc_List_List*_tmp68;enum Cyc_Absyn_AggrKind _tmp67;struct Cyc_List_List*_tmp69;struct Cyc_Absyn_Vardecl*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_Exp*_tmp73;struct Cyc_List_List*_tmp72;struct Cyc_List_List*_tmp71;struct Cyc_Absyn_VarargInfo*_tmp70;int _tmp6F;struct Cyc_List_List*_tmp6E;void*_tmp6D;struct Cyc_Absyn_Tqual _tmp6C;void*_tmp6B;struct Cyc_List_List*_tmp6A;unsigned _tmp7C;void*_tmp7B;struct Cyc_Absyn_Exp*_tmp7A;struct Cyc_Absyn_Tqual _tmp79;void*_tmp78;struct Cyc_Absyn_PtrLoc*_tmp83;void*_tmp82;void*_tmp81;void*_tmp80;void*_tmp7F;struct Cyc_Absyn_Tqual _tmp7E;void*_tmp7D;struct Cyc_Absyn_Tvar*_tmp84;struct Cyc_List_List*_tmp86;void*_tmp85;void*_tmp87;switch(*((int*)_tmp5E)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E)->f2 == 0){_LL1: _tmp87=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E)->f1;_LL2: {void*c=_tmp87;
return t;}}else{_LL3: _tmp85=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E)->f1;_tmp86=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E)->f2;_LL4: {void*c=_tmp85;struct Cyc_List_List*ts=_tmp86;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=0U,_tmp88->f1=c,({struct Cyc_List_List*_tmp6A2=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp88->f2=_tmp6A2;});_tmp88;});}}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp84=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp84;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3U: _LL9: _tmp7D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_type;_tmp7E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_tq;_tmp7F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).rgn;_tmp80=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).nullable;_tmp81=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).bounds;_tmp82=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).zero_term;_tmp83=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).ptrloc;_LLA: {void*elt=_tmp7D;struct Cyc_Absyn_Tqual tq=_tmp7E;void*rgn=_tmp7F;void*nbl=_tmp80;void*bs=_tmp81;void*zt=_tmp82;struct Cyc_Absyn_PtrLoc*loc=_tmp83;
# 342
void*_tmp89=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp89;
void*_tmp8A=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp8A;
void*_tmp8B=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp8B;
struct Cyc_Absyn_Tqual _tmp8C=tq;struct Cyc_Absyn_Tqual tq2=_tmp8C;
# 347
void*_tmp8D=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp8D;
void*_tmp8E=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp8E;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=3U,(_tmp8F->f1).elt_type=elt2,(_tmp8F->f1).elt_tq=tq2,((_tmp8F->f1).ptr_atts).rgn=rgn2,((_tmp8F->f1).ptr_atts).nullable=nbl2,((_tmp8F->f1).ptr_atts).bounds=bs2,((_tmp8F->f1).ptr_atts).zero_term=zt2,((_tmp8F->f1).ptr_atts).ptrloc=loc;_tmp8F;});}case 4U: _LLB: _tmp78=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).elt_type;_tmp79=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).tq;_tmp7A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).num_elts;_tmp7B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zero_term;_tmp7C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zt_loc;_LLC: {void*et=_tmp78;struct Cyc_Absyn_Tqual tq=_tmp79;struct Cyc_Absyn_Exp*eo=_tmp7A;void*zt=_tmp7B;unsigned ztl=_tmp7C;
# 351
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=4U,({void*_tmp6A5=Cyc_Tcutil_copy_type(et);(_tmp90->f1).elt_type=_tmp6A5;}),(_tmp90->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp6A4=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp90->f1).num_elts=_tmp6A4;}),({
void*_tmp6A3=Cyc_Tcutil_copy_type(zt);(_tmp90->f1).zero_term=_tmp6A3;}),(_tmp90->f1).zt_loc=ztl;_tmp90;});}case 5U: _LLD: _tmp6A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).tvars;_tmp6B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).effect;_tmp6C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_tqual;_tmp6D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_type;_tmp6E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).args;_tmp6F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).c_varargs;_tmp70=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).cyc_varargs;_tmp71=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).rgn_po;_tmp72=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).attributes;_tmp73=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_clause;_tmp74=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_relns;_tmp75=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_clause;_tmp76=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_relns;_tmp77=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).return_value;_LLE: {struct Cyc_List_List*tvs=_tmp6A;void*effopt=_tmp6B;struct Cyc_Absyn_Tqual rt_tq=_tmp6C;void*rt=_tmp6D;struct Cyc_List_List*args=_tmp6E;int c_varargs=_tmp6F;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp70;struct Cyc_List_List*rgn_po=_tmp71;struct Cyc_List_List*atts=_tmp72;struct Cyc_Absyn_Exp*req=_tmp73;struct Cyc_List_List*req_rlns=_tmp74;struct Cyc_Absyn_Exp*ens=_tmp75;struct Cyc_List_List*ens_rlns=_tmp76;struct Cyc_Absyn_Vardecl*ret_val=_tmp77;
# 354
struct Cyc_List_List*_tmp91=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp91;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmp92=Cyc_Tcutil_copy_type(rt);void*rt2=_tmp92;
struct Cyc_List_List*_tmp93=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmp93;
int _tmp94=c_varargs;int c_varargs2=_tmp94;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=cyc_varargs == 0?0:({struct Cyc_Absyn_VarargInfo*_tmp9D=_cycalloc(sizeof(*_tmp9D));*_tmp9D=*cyc_varargs;_tmp9D;});
if(cyc_varargs != 0)
({void*_tmp6A6=Cyc_Tcutil_copy_type(cyc_varargs->type);((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_varargs2))->type=_tmp6A6;});{
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmp95;
struct Cyc_List_List*_tmp96=atts;struct Cyc_List_List*atts2=_tmp96;
struct Cyc_Absyn_Exp*_tmp97=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmp97;
struct Cyc_List_List*_tmp98=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmp98;
struct Cyc_Absyn_Exp*_tmp99=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmp99;
struct Cyc_List_List*_tmp9A=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmp9A;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmp9B=_cycalloc(sizeof(*_tmp9B));*_tmp9B=*ret_val;_tmp9B;});
({void*_tmp6A7=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp6A7;});
if((unsigned)ret_val2->rgn)({void*_tmp6A8=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp6A8;});}
# 374
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=5U,(_tmp9C->f1).tvars=tvs2,(_tmp9C->f1).effect=effopt2,(_tmp9C->f1).ret_tqual=rt_tq,(_tmp9C->f1).ret_type=rt2,(_tmp9C->f1).args=args2,(_tmp9C->f1).c_varargs=c_varargs2,(_tmp9C->f1).cyc_varargs=cyc_varargs2,(_tmp9C->f1).rgn_po=rgn_po2,(_tmp9C->f1).attributes=atts2,(_tmp9C->f1).requires_clause=req2,(_tmp9C->f1).requires_relns=req_rlns2,(_tmp9C->f1).ensures_clause=ens2,(_tmp9C->f1).ensures_relns=ens_rlns2,(_tmp9C->f1).return_value=ret_val2;_tmp9C;});}}case 6U: _LLF: _tmp69=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp69;
# 377
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=6U,({struct Cyc_List_List*_tmp6A9=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmp9E->f1=_tmp6A9;});_tmp9E;});}case 7U: _LL11: _tmp67=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f1;_tmp68=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp67;struct Cyc_List_List*fs=_tmp68;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=7U,_tmp9F->f1=k,({struct Cyc_List_List*_tmp6AA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmp9F->f2=_tmp6AA;});_tmp9F;});}case 9U: _LL13: _tmp66=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp66;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=9U,_tmpA0->f1=e;_tmpA0;});}case 11U: _LL15: _tmp65=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5E)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp65;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=11U,_tmpA1->f1=e;_tmpA1;});}case 8U: _LL17: _tmp62=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f1;_tmp63=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f2;_tmp64=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f3;_LL18: {struct _tuple1*tdn=_tmp62;struct Cyc_List_List*ts=_tmp63;struct Cyc_Absyn_Typedefdecl*td=_tmp64;
# 382
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=8U,_tmpA2->f1=tdn,({struct Cyc_List_List*_tmp6AB=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmpA2->f2=_tmp6AB;}),_tmpA2->f3=td,_tmpA2->f4=0;_tmpA2;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)){case 0U: _LL19: _tmp61=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp61;
# 385
struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpA3;
return({union Cyc_Absyn_AggrInfo _tmp6AC=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp6AC,targs);});}case 1U: _LL1B: _tmp60=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp60;
# 388
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp5F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp5F;
# 390
struct Cyc_List_List*_tmpA4=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpA4;
return({union Cyc_Absyn_DatatypeInfo _tmp6AD=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp611;_tmp611.name=dd->name,_tmp611.is_extensible=0;_tmp611;}));Cyc_Absyn_datatype_type(_tmp6AD,targs);});}}}_LL0:;}
# 396
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpA5=d;struct Cyc_Absyn_Exp*_tmpA6;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpA5)->tag == 0U){_LL1: _tmpA6=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpA5)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpA6;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->tag=0U,({struct Cyc_Absyn_Exp*_tmp6AE=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpA7->f1=_tmp6AE;});_tmpA7;});}}else{_LL3: _LL4:
 return d;}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 402
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 404
return({struct _tuple16*_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct Cyc_List_List*_tmp6B0=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpA8->f1=_tmp6B0;}),({
struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpA8->f2=_tmp6AF;});_tmpA8;});}
# 408
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*(*_tmpA9)(int preserve_types,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_deep_copy_exp;struct Cyc_Absyn_Exp*(*DC)(int preserve_types,struct Cyc_Absyn_Exp*e)=_tmpA9;
struct Cyc_Absyn_Exp*new_e;
int _tmpAA=preserve_types;int pt=_tmpAA;
{void*_tmpAB=e->r;void*_stmttmp18=_tmpAB;void*_tmpAC=_stmttmp18;struct Cyc_Absyn_Exp*_tmpAD;struct Cyc_List_List*_tmpB2;struct Cyc_List_List*_tmpB1;struct Cyc_List_List*_tmpB0;struct _fat_ptr _tmpAF;int _tmpAE;void*_tmpB3;struct _fat_ptr*_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;struct Cyc_List_List*_tmpB7;struct Cyc_Core_Opt*_tmpB6;struct Cyc_Absyn_Exp*_tmpB9;struct Cyc_Absyn_Exp*_tmpB8;int _tmpBF;int _tmpBE;struct Cyc_Absyn_Exp*_tmpBD;void**_tmpBC;struct Cyc_Absyn_Exp*_tmpBB;int _tmpBA;struct Cyc_Absyn_Enumfield*_tmpC1;void*_tmpC0;struct Cyc_Absyn_Enumfield*_tmpC3;struct Cyc_Absyn_Enumdecl*_tmpC2;struct Cyc_List_List*_tmpC7;void*_tmpC6;struct Cyc_Absyn_Tqual _tmpC5;struct _fat_ptr*_tmpC4;struct Cyc_List_List*_tmpC9;void*_tmpC8;struct Cyc_Absyn_Aggrdecl*_tmpCD;struct Cyc_List_List*_tmpCC;struct Cyc_List_List*_tmpCB;struct _tuple1*_tmpCA;int _tmpD0;void*_tmpCF;struct Cyc_Absyn_Exp*_tmpCE;int _tmpD4;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Exp*_tmpD2;struct Cyc_Absyn_Vardecl*_tmpD1;struct Cyc_Absyn_Datatypefield*_tmpD7;struct Cyc_Absyn_Datatypedecl*_tmpD6;struct Cyc_List_List*_tmpD5;struct Cyc_List_List*_tmpD8;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;int _tmpDE;int _tmpDD;struct _fat_ptr*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;int _tmpE2;int _tmpE1;struct _fat_ptr*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_List_List*_tmpE3;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_List_List*_tmpE7;void*_tmpE6;struct Cyc_Absyn_Exp*_tmpE8;void*_tmpE9;struct Cyc_Absyn_Exp*_tmpEA;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;enum Cyc_Absyn_Coercion _tmpF0;int _tmpEF;struct Cyc_Absyn_Exp*_tmpEE;void*_tmpED;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF3;int _tmpF5;struct Cyc_Absyn_Exp*_tmpF4;int _tmpF9;struct Cyc_Absyn_VarargCallInfo*_tmpF8;struct Cyc_List_List*_tmpF7;struct Cyc_Absyn_Exp*_tmpF6;int _tmp102;int _tmp101;void*_tmp100;struct Cyc_Absyn_Tqual _tmpFF;struct _fat_ptr*_tmpFE;struct Cyc_List_List*_tmpFD;int _tmpFC;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmp105;struct Cyc_Core_Opt*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;enum Cyc_Absyn_Incrementor _tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_List_List*_tmp112;enum Cyc_Absyn_Primop _tmp111;struct _fat_ptr _tmp113;void*_tmp114;union Cyc_Absyn_Cnst _tmp115;switch(*((int*)_tmpAC)){case 0U: _LL1: _tmp115=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp115;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp114=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL4: {void*b=_tmp114;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp113=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL6: {struct _fat_ptr p=_tmp113;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp111=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp112=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp111;struct Cyc_List_List*es=_tmp112;
new_e=({enum Cyc_Absyn_Primop _tmp6B2=p;struct Cyc_List_List*_tmp6B1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_primop_exp(_tmp6B2,_tmp6B1,e->loc);});goto _LL0;}case 5U: _LL9: _tmp10F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp110=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp10F;enum Cyc_Absyn_Incrementor i=_tmp110;
new_e=({struct Cyc_Absyn_Exp*_tmp6B4=DC(pt,e1);enum Cyc_Absyn_Incrementor _tmp6B3=i;Cyc_Absyn_increment_exp(_tmp6B4,_tmp6B3,e->loc);});goto _LL0;}case 7U: _LLB: _tmp10D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp10E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp10D;struct Cyc_Absyn_Exp*e2=_tmp10E;
new_e=({struct Cyc_Absyn_Exp*_tmp6B6=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6B5=DC(pt,e2);Cyc_Absyn_and_exp(_tmp6B6,_tmp6B5,e->loc);});goto _LL0;}case 8U: _LLD: _tmp10B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp10C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp10B;struct Cyc_Absyn_Exp*e2=_tmp10C;
new_e=({struct Cyc_Absyn_Exp*_tmp6B8=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6B7=DC(pt,e2);Cyc_Absyn_or_exp(_tmp6B8,_tmp6B7,e->loc);});goto _LL0;}case 9U: _LLF: _tmp109=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp10A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp109;struct Cyc_Absyn_Exp*e2=_tmp10A;
new_e=({struct Cyc_Absyn_Exp*_tmp6BA=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6B9=DC(pt,e2);Cyc_Absyn_seq_exp(_tmp6BA,_tmp6B9,e->loc);});goto _LL0;}case 6U: _LL11: _tmp106=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp107=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmp108=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp106;struct Cyc_Absyn_Exp*e2=_tmp107;struct Cyc_Absyn_Exp*e3=_tmp108;
# 422
new_e=({struct Cyc_Absyn_Exp*_tmp6BD=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6BC=DC(pt,e2);struct Cyc_Absyn_Exp*_tmp6BB=DC(pt,e3);Cyc_Absyn_conditional_exp(_tmp6BD,_tmp6BC,_tmp6BB,e->loc);});goto _LL0;}case 4U: _LL13: _tmp103=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmp104=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmp105=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp103;struct Cyc_Core_Opt*po=_tmp104;struct Cyc_Absyn_Exp*e2=_tmp105;
# 424
new_e=({struct Cyc_Absyn_Exp*_tmp6C0=DC(pt,e1);struct Cyc_Core_Opt*_tmp6BF=(unsigned)po?({struct Cyc_Core_Opt*_tmp116=_cycalloc(sizeof(*_tmp116));_tmp116->v=(void*)po->v;_tmp116;}): 0;struct Cyc_Absyn_Exp*_tmp6BE=DC(pt,e2);Cyc_Absyn_assignop_exp(_tmp6C0,_tmp6BF,_tmp6BE,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f3 != 0){_LL15: _tmpFA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpFB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpFC=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f3)->num_varargs;_tmpFD=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f3)->injectors;_tmpFE=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f3)->vai)->name;_tmpFF=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f3)->vai)->tq;_tmp100=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f3)->vai)->type;_tmp101=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f3)->vai)->inject;_tmp102=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_List_List*es=_tmpFB;int n=_tmpFC;struct Cyc_List_List*is=_tmpFD;struct _fat_ptr*nm=_tmpFE;struct Cyc_Absyn_Tqual tq=_tmpFF;void*t=_tmp100;int i=_tmp101;int resolved=_tmp102;
# 427
new_e=({void*_tmp6C6=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->tag=10U,({
struct Cyc_Absyn_Exp*_tmp6C5=DC(pt,e1);_tmp119->f1=_tmp6C5;}),({struct Cyc_List_List*_tmp6C4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp119->f2=_tmp6C4;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp6C3=({struct Cyc_Absyn_VarargCallInfo*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->num_varargs=n,_tmp118->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp6C2=({struct Cyc_Absyn_VarargInfo*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->name=nm,_tmp117->tq=tq,({void*_tmp6C1=Cyc_Tcutil_copy_type(t);_tmp117->type=_tmp6C1;}),_tmp117->inject=i;_tmp117;});_tmp118->vai=_tmp6C2;});_tmp118;});
# 429
_tmp119->f3=_tmp6C3;}),_tmp119->f4=resolved;_tmp119;});
# 427
Cyc_Absyn_new_exp(_tmp6C6,e->loc);});
# 432
goto _LL0;}}else{_LL17: _tmpF6=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpF7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpF8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpF9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpF6;struct Cyc_List_List*es=_tmpF7;struct Cyc_Absyn_VarargCallInfo*vci=_tmpF8;int resolved=_tmpF9;
# 434
new_e=({void*_tmp6C9=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=10U,({struct Cyc_Absyn_Exp*_tmp6C8=DC(pt,e1);_tmp11A->f1=_tmp6C8;}),({struct Cyc_List_List*_tmp6C7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp11A->f2=_tmp6C7;}),_tmp11A->f3=vci,_tmp11A->f4=resolved;_tmp11A;});Cyc_Absyn_new_exp(_tmp6C9,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmpF4=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpF5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpF4;int b=_tmpF5;
# 437
new_e=b?({struct Cyc_Absyn_Exp*_tmp6CB=DC(pt,e1);Cyc_Absyn_rethrow_exp(_tmp6CB,e->loc);}):({struct Cyc_Absyn_Exp*_tmp6CA=DC(pt,e1);Cyc_Absyn_throw_exp(_tmp6CA,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmpF3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpF3;
# 440
new_e=({struct Cyc_Absyn_Exp*_tmp6CC=DC(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp6CC,e->loc);});goto _LL0;}case 13U: _LL1D: _tmpF1=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpF2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpF1;struct Cyc_List_List*ts=_tmpF2;
# 442
new_e=({struct Cyc_Absyn_Exp*_tmp6CE=DC(pt,e1);struct Cyc_List_List*_tmp6CD=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp6CE,_tmp6CD,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpED=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpEE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpEF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpF0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL20: {void*t=_tmpED;struct Cyc_Absyn_Exp*e1=_tmpEE;int b=_tmpEF;enum Cyc_Absyn_Coercion c=_tmpF0;
# 444
new_e=({void*_tmp6D2=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp6D1=DC(pt,e1);int _tmp6D0=b;enum Cyc_Absyn_Coercion _tmp6CF=c;Cyc_Absyn_cast_exp(_tmp6D2,_tmp6D1,_tmp6D0,_tmp6CF,e->loc);});goto _LL0;}case 16U: _LL21: _tmpEB=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpEC=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpEB;struct Cyc_Absyn_Exp*e1=_tmpEC;
# 446
new_e=({struct Cyc_Absyn_Exp*_tmp6D4=(unsigned)eo?DC(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp6D3=DC(pt,e1);Cyc_Absyn_New_exp(_tmp6D4,_tmp6D3,e->loc);});goto _LL0;}case 15U: _LL23: _tmpEA=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpEA;
new_e=({struct Cyc_Absyn_Exp*_tmp6D5=DC(pt,e1);Cyc_Absyn_address_exp(_tmp6D5,e->loc);});goto _LL0;}case 17U: _LL25: _tmpE9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL26: {void*t=_tmpE9;
new_e=({void*_tmp6D6=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp6D6,e->loc);});goto _LL0;}case 18U: _LL27: _tmpE8=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpE8;
new_e=({struct Cyc_Absyn_Exp*_tmp6D7=DC(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp6D7,e->loc);});goto _LL0;}case 19U: _LL29: _tmpE6=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpE7=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL2A: {void*t=_tmpE6;struct Cyc_List_List*ofs=_tmpE7;
new_e=({void*_tmp6D9=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp6D8=ofs;Cyc_Absyn_offsetof_exp(_tmp6D9,_tmp6D8,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpE5=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpE5;
new_e=({struct Cyc_Absyn_Exp*_tmp6DA=DC(pt,e1);Cyc_Absyn_deref_exp(_tmp6DA,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpE4=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpE4;
new_e=({struct Cyc_Absyn_Exp*_tmp6DB=DC(pt,e1);Cyc_Absyn_extension_exp(_tmp6DB,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpE3=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL30: {struct Cyc_List_List*es=_tmpE3;
new_e=({struct Cyc_List_List*_tmp6DC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_tuple_exp(_tmp6DC,e->loc);});goto _LL0;}case 21U: _LL31: _tmpDF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpE0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpE1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpE2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpDF;struct _fat_ptr*n=_tmpE0;int f1=_tmpE1;int f2=_tmpE2;
# 455
new_e=({void*_tmp6DE=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=21U,({struct Cyc_Absyn_Exp*_tmp6DD=DC(pt,e1);_tmp11B->f1=_tmp6DD;}),_tmp11B->f2=n,_tmp11B->f3=f1,_tmp11B->f4=f2;_tmp11B;});Cyc_Absyn_new_exp(_tmp6DE,e->loc);});goto _LL0;}case 22U: _LL33: _tmpDB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpDC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpDD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpDE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpDB;struct _fat_ptr*n=_tmpDC;int f1=_tmpDD;int f2=_tmpDE;
# 457
new_e=({void*_tmp6E0=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=22U,({struct Cyc_Absyn_Exp*_tmp6DF=DC(pt,e1);_tmp11C->f1=_tmp6DF;}),_tmp11C->f2=n,_tmp11C->f3=f1,_tmp11C->f4=f2;_tmp11C;});Cyc_Absyn_new_exp(_tmp6E0,e->loc);});goto _LL0;}case 23U: _LL35: _tmpD9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpDA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpD9;struct Cyc_Absyn_Exp*e2=_tmpDA;
# 459
new_e=({struct Cyc_Absyn_Exp*_tmp6E2=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6E1=DC(pt,e2);Cyc_Absyn_subscript_exp(_tmp6E2,_tmp6E1,e->loc);});goto _LL0;}case 26U: _LL37: _tmpD8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL38: {struct Cyc_List_List*eds=_tmpD8;
# 461
new_e=({void*_tmp6E4=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=26U,({struct Cyc_List_List*_tmp6E3=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp11D->f1=_tmp6E3;});_tmp11D;});Cyc_Absyn_new_exp(_tmp6E4,e->loc);});goto _LL0;}case 31U: _LL39: _tmpD5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpD7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL3A: {struct Cyc_List_List*es=_tmpD5;struct Cyc_Absyn_Datatypedecl*dtd=_tmpD6;struct Cyc_Absyn_Datatypefield*dtf=_tmpD7;
# 463
new_e=({void*_tmp6E6=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->tag=31U,({struct Cyc_List_List*_tmp6E5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp11E->f1=_tmp6E5;}),_tmp11E->f2=dtd,_tmp11E->f3=dtf;_tmp11E;});Cyc_Absyn_new_exp(_tmp6E6,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpD1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpD2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpD3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpD4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpD1;struct Cyc_Absyn_Exp*e1=_tmpD2;struct Cyc_Absyn_Exp*e2=_tmpD3;int b=_tmpD4;
# 465
new_e=({void*_tmp6E9=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=27U,_tmp11F->f1=vd,({struct Cyc_Absyn_Exp*_tmp6E8=DC(pt,e1);_tmp11F->f2=_tmp6E8;}),({struct Cyc_Absyn_Exp*_tmp6E7=DC(pt,e2);_tmp11F->f3=_tmp6E7;}),_tmp11F->f4=b;_tmp11F;});Cyc_Absyn_new_exp(_tmp6E9,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpCE=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCF=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpD0=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpCE;void*t=_tmpCF;int b=_tmpD0;
# 467
new_e=({void*_tmp6EC=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->tag=28U,({struct Cyc_Absyn_Exp*_tmp6EB=DC(pt,e);_tmp120->f1=_tmp6EB;}),({void*_tmp6EA=Cyc_Tcutil_copy_type(t);_tmp120->f2=_tmp6EA;}),_tmp120->f3=b;_tmp120;});Cyc_Absyn_new_exp(_tmp6EC,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpCA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpCB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpCC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpCD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_LL40: {struct _tuple1*n=_tmpCA;struct Cyc_List_List*ts=_tmpCB;struct Cyc_List_List*eds=_tmpCC;struct Cyc_Absyn_Aggrdecl*agr=_tmpCD;
# 470
new_e=({void*_tmp6EF=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->tag=29U,_tmp121->f1=n,({struct Cyc_List_List*_tmp6EE=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp121->f2=_tmp6EE;}),({struct Cyc_List_List*_tmp6ED=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp121->f3=_tmp6ED;}),_tmp121->f4=agr;_tmp121;});Cyc_Absyn_new_exp(_tmp6EF,e->loc);});
# 472
goto _LL0;}case 30U: _LL41: _tmpC8=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL42: {void*t=_tmpC8;struct Cyc_List_List*eds=_tmpC9;
# 474
new_e=({void*_tmp6F2=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=30U,({void*_tmp6F1=Cyc_Tcutil_copy_type(t);_tmp122->f1=_tmp6F1;}),({struct Cyc_List_List*_tmp6F0=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp122->f2=_tmp6F0;});_tmp122;});Cyc_Absyn_new_exp(_tmp6F2,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpC4=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1)->f1;_tmpC5=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1)->f2;_tmpC6=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f1)->f3;_tmpC7=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL44: {struct _fat_ptr*vopt=_tmpC4;struct Cyc_Absyn_Tqual tq=_tmpC5;void*t=_tmpC6;struct Cyc_List_List*eds=_tmpC7;
# 477
new_e=({void*_tmp6F6=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=25U,({struct _tuple9*_tmp6F5=({struct _tuple9*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->f1=vopt,_tmp123->f2=tq,({void*_tmp6F4=Cyc_Tcutil_copy_type(t);_tmp123->f3=_tmp6F4;});_tmp123;});_tmp124->f1=_tmp6F5;}),({
struct Cyc_List_List*_tmp6F3=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp124->f2=_tmp6F3;});_tmp124;});
# 477
Cyc_Absyn_new_exp(_tmp6F6,e->loc);});
# 479
goto _LL0;}case 32U: _LL45: _tmpC2=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC3=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpC2;struct Cyc_Absyn_Enumfield*ef=_tmpC3;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpC0=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpC1=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL48: {void*t=_tmpC0;struct Cyc_Absyn_Enumfield*ef=_tmpC1;
# 482
new_e=({void*_tmp6F8=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=33U,({void*_tmp6F7=Cyc_Tcutil_copy_type(t);_tmp125->f1=_tmp6F7;}),_tmp125->f2=ef;_tmp125;});Cyc_Absyn_new_exp(_tmp6F8,e->loc);});goto _LL0;}case 34U: _LL49: _tmpBA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).is_calloc;_tmpBB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).rgn;_tmpBC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).elt_type;_tmpBD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).num_elts;_tmpBE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).fat_result;_tmpBF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAC)->f1).inline_call;_LL4A: {int ic=_tmpBA;struct Cyc_Absyn_Exp*r=_tmpBB;void**t=_tmpBC;struct Cyc_Absyn_Exp*n=_tmpBD;int res=_tmpBE;int inlc=_tmpBF;
# 484
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=DC(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp126=_cycalloc(sizeof(*_tmp126));({void*_tmp6F9=Cyc_Tcutil_copy_type(*t);*_tmp126=_tmp6F9;});_tmp126;});
({void*_tmp6FA=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=34U,(_tmp127->f1).is_calloc=ic,(_tmp127->f1).rgn=r1,(_tmp127->f1).elt_type=t1,(_tmp127->f1).num_elts=n,(_tmp127->f1).fat_result=res,(_tmp127->f1).inline_call=inlc;_tmp127;});e2->r=_tmp6FA;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpB8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpB8;struct Cyc_Absyn_Exp*e2=_tmpB9;
new_e=({struct Cyc_Absyn_Exp*_tmp6FC=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6FB=DC(pt,e2);Cyc_Absyn_swap_exp(_tmp6FC,_tmp6FB,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpB6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpB6;struct Cyc_List_List*eds=_tmpB7;
# 492
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->v=(struct _tuple1*)nopt->v;_tmp128;});
new_e=({void*_tmp6FE=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=36U,_tmp129->f1=nopt1,({struct Cyc_List_List*_tmp6FD=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp129->f2=_tmp6FD;});_tmp129;});Cyc_Absyn_new_exp(_tmp6FE,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 497
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6FF=({const char*_tmp12A="deep_copy: statement expressions unsupported";_tag_fat(_tmp12A,sizeof(char),45U);});_tmp12B->f1=_tmp6FF;});_tmp12B;}));case 38U: _LL51: _tmpB4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpB5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpB4;struct _fat_ptr*fn=_tmpB5;
# 499
new_e=({void*_tmp701=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=38U,({struct Cyc_Absyn_Exp*_tmp700=DC(pt,e1);_tmp12C->f1=_tmp700;}),_tmp12C->f2=fn;_tmp12C;});Cyc_Absyn_new_exp(_tmp701,e->loc);});goto _LL0;}case 39U: _LL53: _tmpB3=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL54: {void*t=_tmpB3;
new_e=({void*_tmp702=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp702,e->loc);});goto _LL0;}case 40U: _LL55: _tmpAE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_tmpAF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f2;_tmpB0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f3;_tmpB1=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f4;_tmpB2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAC)->f5;_LL56: {int v=_tmpAE;struct _fat_ptr t=_tmpAF;struct Cyc_List_List*o=_tmpB0;struct Cyc_List_List*i=_tmpB1;struct Cyc_List_List*c=_tmpB2;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _LL57: _tmpAD=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpAC)->f1;_LL58: {struct Cyc_Absyn_Exp*e1=_tmpAD;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 505
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 509
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 520 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp12D=ka1;enum Cyc_Absyn_AliasQual _tmp12F;enum Cyc_Absyn_KindQual _tmp12E;_LL1: _tmp12E=_tmp12D->kind;_tmp12F=_tmp12D->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp12E;enum Cyc_Absyn_AliasQual a1=_tmp12F;
struct Cyc_Absyn_Kind*_tmp130=ka2;enum Cyc_Absyn_AliasQual _tmp132;enum Cyc_Absyn_KindQual _tmp131;_LL4: _tmp131=_tmp130->kind;_tmp132=_tmp130->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp131;enum Cyc_Absyn_AliasQual a2=_tmp132;
# 524
if((int)k1 != (int)k2){
struct _tuple17 _tmp133=({struct _tuple17 _tmp612;_tmp612.f1=k1,_tmp612.f2=k2;_tmp612;});struct _tuple17 _stmttmp19=_tmp133;struct _tuple17 _tmp134=_stmttmp19;switch(_tmp134.f1){case Cyc_Absyn_BoxKind: switch(_tmp134.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp134.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 532
if((int)a1 != (int)a2){
struct _tuple18 _tmp135=({struct _tuple18 _tmp613;_tmp613.f1=a1,_tmp613.f2=a2;_tmp613;});struct _tuple18 _stmttmp1A=_tmp135;struct _tuple18 _tmp136=_stmttmp1A;switch(_tmp136.f1){case Cyc_Absyn_Aliasable: if(_tmp136.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp136.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 538
return 1;}}}
# 541
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp137=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1B=_tmp137;void*_tmp138=_stmttmp1B;struct Cyc_Absyn_Kind*_tmp139;struct Cyc_Absyn_Kind*_tmp13A;switch(*((int*)_tmp138)){case 0U: _LL1: _tmp13A=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp138)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp13A;
return k;}case 2U: _LL3: _tmp139=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp138)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp139;
return k;}default: _LL5: _LL6:
({void*_tmp703=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->tag=2U,_tmp13B->f1=0,_tmp13B->f2=def;_tmp13B;});tv->kind=_tmp703;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 549
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp13C=Cyc_Absyn_compress(t);void*_stmttmp1C=_tmp13C;void*_tmp13D=_stmttmp1C;struct Cyc_Absyn_Tvar*_tmp13E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13D)->tag == 2U){_LL1: _tmp13E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13D)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp13E;
# 552
void*_tmp13F=tv->kind;void*oldkb=_tmp13F;
tv->kind=kb;
return({struct _tuple19 _tmp614;_tmp614.f1=tv,_tmp614.f2=oldkb;_tmp614;});}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp141=({struct Cyc_Warn_String_Warn_Warg_struct _tmp616;_tmp616.tag=0U,({struct _fat_ptr _tmp704=({const char*_tmp143="swap_kind: cannot update the kind of ";_tag_fat(_tmp143,sizeof(char),38U);});_tmp616.f1=_tmp704;});_tmp616;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp142=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp615;_tmp615.tag=2U,_tmp615.f1=(void*)t;_tmp615;});void*_tmp140[2U];_tmp140[0]=& _tmp141,_tmp140[1]=& _tmp142;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp140,sizeof(void*),2U));});}_LL0:;}
# 561
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 563
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 567
struct _RegionHandle _tmp144=_new_region("r");struct _RegionHandle*r=& _tmp144;_push_region(r);
{struct Cyc_List_List*_tmp145=0;struct Cyc_List_List*inst=_tmp145;
# 570
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp146=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp146;
void*_tmp147=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp147;
enum Cyc_Absyn_KindQual _tmp148=(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk))->kind;enum Cyc_Absyn_KindQual _stmttmp1D=_tmp148;enum Cyc_Absyn_KindQual _tmp149=_stmttmp1D;switch(_tmp149){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
 inst=({struct Cyc_List_List*_tmp14B=_region_malloc(r,sizeof(*_tmp14B));({struct _tuple19*_tmp705=({struct _tuple19*_tmp14A=_region_malloc(r,sizeof(*_tmp14A));_tmp14A->f1=tv,_tmp14A->f2=t;_tmp14A;});_tmp14B->hd=_tmp705;}),_tmp14B->tl=inst;_tmp14B;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 579
if(inst != 0){
field_type=({struct _RegionHandle*_tmp707=r;struct Cyc_List_List*_tmp706=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp707,_tmp706,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 568
;_pop_region();}
# 584
return k;}
# 591
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 593
void*_tmp14C=Cyc_Absyn_compress(t);void*_stmttmp1E=_tmp14C;void*_tmp14D=_stmttmp1E;struct Cyc_Absyn_Typedefdecl*_tmp14E;struct Cyc_Absyn_Exp*_tmp14F;struct Cyc_Absyn_PtrInfo _tmp150;struct Cyc_List_List*_tmp152;void*_tmp151;struct Cyc_Absyn_Tvar*_tmp153;struct Cyc_Core_Opt*_tmp154;switch(*((int*)_tmp14D)){case 1U: _LL1: _tmp154=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14D)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp154;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp153=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14D)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp153;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp151=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14D)->f1;_tmp152=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14D)->f2;_LL6: {void*c=_tmp151;struct Cyc_List_List*ts=_tmp152;
# 597
void*_tmp155=c;int _tmp159;struct Cyc_Absyn_AggrdeclImpl*_tmp158;struct Cyc_List_List*_tmp157;enum Cyc_Absyn_AggrKind _tmp156;struct Cyc_Absyn_Kind*_tmp15A;enum Cyc_Absyn_Size_of _tmp15B;switch(*((int*)_tmp155)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp15B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp155)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp15B;
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp15A=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp155)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp15A;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp155)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 619
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp617;_tmp617.tag=0U,({struct _fat_ptr _tmp708=({const char*_tmp15E="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp15E,sizeof(char),40U);});_tmp617.f1=_tmp708;});_tmp617;});void*_tmp15C[1U];_tmp15C[0]=& _tmp15D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp15C,sizeof(void*),1U));});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp155)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 622
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp156=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp155)->f1).KnownAggr).val)->kind;_tmp157=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp155)->f1).KnownAggr).val)->tvs;_tmp158=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp155)->f1).KnownAggr).val)->impl;_tmp159=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp155)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp156;struct Cyc_List_List*tvs=_tmp157;struct Cyc_Absyn_AggrdeclImpl*i=_tmp158;int expected_mem_kind=_tmp159;
# 624
if(i == 0)
return expected_mem_kind?& Cyc_Tcutil_mk:& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp15F=i->fields;struct Cyc_List_List*fields=_tmp15F;
if(fields == 0)return& Cyc_Tcutil_mk;
# 629
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){
;}{
void*_tmp160=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp160;
struct Cyc_Absyn_Kind*_tmp161=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp161;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 638
for(0;fields != 0;fields=fields->tl){
void*_tmp162=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp162;
struct Cyc_Absyn_Kind*_tmp163=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp163;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 643
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 645
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp150=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14D)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp150;
# 648
void*_tmp164=Cyc_Absyn_compress((pinfo.ptr_atts).bounds);void*_stmttmp1F=_tmp164;void*_tmp165=_stmttmp1F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp165)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp165)->f1)){case 13U: _LL4D: _LL4E: {
# 650
enum Cyc_Absyn_AliasQual _tmp166=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp20=_tmp166;enum Cyc_Absyn_AliasQual _tmp167=_stmttmp20;switch(_tmp167){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 656
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 658
enum Cyc_Absyn_AliasQual _tmp168=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp21=_tmp168;enum Cyc_Absyn_AliasQual _tmp169=_stmttmp21;switch(_tmp169){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 664
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 668
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp14F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14D)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp14F;
# 670
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp14E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14D)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp14E;
# 674
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp619;_tmp619.tag=0U,({struct _fat_ptr _tmp709=({const char*_tmp16D="type_kind: typedef found: ";_tag_fat(_tmp16D,sizeof(char),27U);});_tmp619.f1=_tmp709;});_tmp619;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16C=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp618;_tmp618.tag=2U,_tmp618.f1=(void*)t;_tmp618;});void*_tmp16A[2U];_tmp16A[0]=& _tmp16B,_tmp16A[1]=& _tmp16C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp16A,sizeof(void*),2U));});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14D)->f1)->r)){case 1U: _LL17: _LL18:
 return& Cyc_Tcutil_bk;case 0U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 683
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 688
static void*Cyc_Tcutil_rgns_of(void*t);
# 690
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 694
static struct _tuple19*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp16E=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp22=_tmp16E;struct Cyc_Absyn_Kind*_tmp16F=_stmttmp22;switch(((struct Cyc_Absyn_Kind*)_tmp16F)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp16F)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp170=_cycalloc(sizeof(*_tmp170));_tmp170->tag=9U,({struct Cyc_Absyn_Exp*_tmp70A=Cyc_Absyn_uint_exp(0U,0U);_tmp170->f1=_tmp70A;});_tmp170;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 705
return({struct _tuple19*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->f1=tv,_tmp171->f2=t;_tmp171;});}
# 712
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp172=Cyc_Absyn_compress(t);void*_stmttmp23=_tmp172;void*_tmp173=_stmttmp23;struct Cyc_List_List*_tmp174;struct Cyc_List_List*_tmp175;struct Cyc_List_List*_tmp17C;struct Cyc_Absyn_VarargInfo*_tmp17B;struct Cyc_List_List*_tmp17A;void*_tmp179;struct Cyc_Absyn_Tqual _tmp178;void*_tmp177;struct Cyc_List_List*_tmp176;struct Cyc_List_List*_tmp17D;void*_tmp17E;void*_tmp180;void*_tmp17F;struct Cyc_List_List*_tmp181;switch(*((int*)_tmp173)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp173)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp173)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp181=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp173)->f2;_LL6: {struct Cyc_List_List*ts=_tmp181;
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 719
struct Cyc_Absyn_Kind*_tmp182=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp24=_tmp182;struct Cyc_Absyn_Kind*_tmp183=_stmttmp24;switch(((struct Cyc_Absyn_Kind*)_tmp183)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp17F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp173)->f1).elt_type;_tmp180=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp173)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp17F;void*r=_tmp180;
# 727
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp184[2U];({void*_tmp70C=Cyc_Absyn_access_eff(r);_tmp184[0]=_tmp70C;}),({void*_tmp70B=Cyc_Tcutil_rgns_of(et);_tmp184[1]=_tmp70B;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp184,sizeof(void*),2U));})));}case 4U: _LLD: _tmp17E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp173)->f1).elt_type;_LLE: {void*et=_tmp17E;
# 729
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp17D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp173)->f2;_LL10: {struct Cyc_List_List*fs=_tmp17D;
# 731
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp176=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).tvars;_tmp177=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).effect;_tmp178=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).ret_tqual;_tmp179=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).ret_type;_tmp17A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).args;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).cyc_varargs;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp173)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp176;void*eff=_tmp177;struct Cyc_Absyn_Tqual rt_tq=_tmp178;void*rt=_tmp179;struct Cyc_List_List*args=_tmp17A;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp17B;struct Cyc_List_List*rpo=_tmp17C;
# 740
void*_tmp185=({struct Cyc_List_List*_tmp70D=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp70D,(void*)_check_null(eff));});void*e=_tmp185;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp175=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp173)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp175;
# 743
struct Cyc_List_List*_tmp186=0;struct Cyc_List_List*ts=_tmp186;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp187=_cycalloc(sizeof(*_tmp187));_tmp187->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp187->tl=ts;_tmp187;});}
_tmp174=ts;goto _LL16;}case 8U: _LL15: _tmp174=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp173)->f2;_LL16: {struct Cyc_List_List*ts=_tmp174;
# 748
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp189=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp70E=({const char*_tmp18A="typedecl in rgns_of";_tag_fat(_tmp18A,sizeof(char),20U);});_tmp61A.f1=_tmp70E;});_tmp61A;});void*_tmp188[1U];_tmp188[0]=& _tmp189;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp188,sizeof(void*),1U));});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 758
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp18B=e;void*_tmp18C;struct Cyc_List_List**_tmp18D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f1)){case 9U: _LL1: _tmp18D=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f2;_LL2: {struct Cyc_List_List**es=_tmp18D;
# 762
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp18E=(void*)effs->hd;void*eff=_tmp18E;
({void*_tmp70F=(void*)Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp70F;});{
void*_tmp18F=(void*)effs->hd;void*_stmttmp25=_tmp18F;void*_tmp190=_stmttmp25;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2)->tl == 0){_LLE: _LLF:
# 771
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 775
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp191=Cyc_Absyn_compress((void*)effs->hd);void*_stmttmp26=_tmp191;void*_tmp192=_stmttmp26;void*_tmp193;struct Cyc_List_List*_tmp194;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f1)){case 9U: _LL13: _tmp194=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp194;
# 780
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp192)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp193=_tmp192;_LL1C: {void*e=_tmp193;
effects=({struct Cyc_List_List*_tmp195=_cycalloc(sizeof(*_tmp195));_tmp195->hd=e,_tmp195->tl=effects;_tmp195;});goto _LL12;}}_LL12:;}}
# 786
({struct Cyc_List_List*_tmp710=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp710;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f2 != 0){_LL3: _tmp18C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18B)->f2)->hd;_LL4: {void*t=_tmp18C;
# 789
void*_tmp196=Cyc_Absyn_compress(t);void*_stmttmp27=_tmp196;void*_tmp197=_stmttmp27;switch(*((int*)_tmp197)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 794
 return e;}_LL0:;}}
# 799
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp198=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->tag=5U,(_tmp199->f1).tvars=0,(_tmp199->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp711=Cyc_Absyn_empty_tqual(0U);(_tmp199->f1).ret_tqual=_tmp711;}),(_tmp199->f1).ret_type=Cyc_Absyn_void_type,(_tmp199->f1).args=0,(_tmp199->f1).c_varargs=0,(_tmp199->f1).cyc_varargs=0,(_tmp199->f1).rgn_po=0,(_tmp199->f1).attributes=0,(_tmp199->f1).requires_clause=0,(_tmp199->f1).requires_relns=0,(_tmp199->f1).ensures_clause=0,(_tmp199->f1).ensures_relns=0,(_tmp199->f1).return_value=0;_tmp199;});
# 800
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp198;
# 810
return({void*_tmp715=(void*)fntype;void*_tmp714=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp713=Cyc_Absyn_empty_tqual(0U);void*_tmp712=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp715,_tmp714,_tmp713,_tmp712,Cyc_Absyn_false_type);});}
# 817
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Absyn_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp19A=Cyc_Absyn_compress(e);void*_stmttmp28=_tmp19A;void*_tmp19B=_stmttmp28;struct Cyc_Core_Opt*_tmp19E;void**_tmp19D;struct Cyc_Core_Opt*_tmp19C;void*_tmp19F;struct Cyc_List_List*_tmp1A0;void*_tmp1A1;switch(*((int*)_tmp19B)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2 != 0){_LL1: _tmp1A1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->hd;_LL2: {void*r2=_tmp1A1;
# 824
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Absyn_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp1A2=({struct _tuple15 _tmp61B;_tmp61B.f1=r,_tmp61B.f2=r2;_tmp61B;});struct _tuple15 _stmttmp29=_tmp1A2;struct _tuple15 _tmp1A3=_stmttmp29;struct Cyc_Absyn_Tvar*_tmp1A5;struct Cyc_Absyn_Tvar*_tmp1A4;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A3.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A3.f2)->tag == 2U){_LLC: _tmp1A4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A3.f1)->f1;_tmp1A5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A3.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1A4;struct Cyc_Absyn_Tvar*tv2=_tmp1A5;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1A0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2;_LL4: {struct Cyc_List_List*es=_tmp1A0;
# 832
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2 != 0){_LL5: _tmp19F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->hd;_LL6: {void*t=_tmp19F;
# 836
void*_tmp1A6=Cyc_Tcutil_rgns_of(t);void*_stmttmp2A=_tmp1A6;void*_tmp1A7=_stmttmp2A;void*_tmp1A8;void*_tmp1A9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f2 != 0){_LL11: _tmp1A9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f2)->hd;_LL12: {void*t=_tmp1A9;
# 838
if(!constrain)return 0;{
void*_tmp1AA=Cyc_Absyn_compress(t);void*_stmttmp2B=_tmp1AA;void*_tmp1AB=_stmttmp2B;struct Cyc_Core_Opt*_tmp1AE;void**_tmp1AD;struct Cyc_Core_Opt*_tmp1AC;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AB)->tag == 1U){_LL16: _tmp1AC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AB)->f1;_tmp1AD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AB)->f2;_tmp1AE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AB)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1AC;void**p=_tmp1AD;struct Cyc_Core_Opt*s=_tmp1AE;
# 843
void*_tmp1AF=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1AF;
# 846
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp717=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1B0[2U];_tmp1B0[0]=ev,({void*_tmp716=Cyc_Absyn_access_eff(r);_tmp1B0[1]=_tmp716;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B0,sizeof(void*),2U));})));*p=_tmp717;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1A8=_tmp1A7;_LL14: {void*e2=_tmp1A8;
# 851
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp19C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19B)->f1;_tmp19D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19B)->f2;_tmp19E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19B)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp19C;void**p=_tmp19D;struct Cyc_Core_Opt*s=_tmp19E;
# 854
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61C;_tmp61C.tag=0U,({struct _fat_ptr _tmp718=({const char*_tmp1B3="effect evar has wrong kind";_tag_fat(_tmp1B3,sizeof(char),27U);});_tmp61C.f1=_tmp718;});_tmp61C;});void*_tmp1B1[1U];_tmp1B1[0]=& _tmp1B2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1B1,sizeof(void*),1U));});
if(!constrain)return 0;{
# 859
void*_tmp1B4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1B4;
# 862
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp71A=Cyc_Absyn_join_eff(({void*_tmp1B5[2U];_tmp1B5[0]=ev,({void*_tmp719=Cyc_Absyn_access_eff(r);_tmp1B5[1]=_tmp719;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B5,sizeof(void*),2U));}));*p=_tmp71A;});
return 1;}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 872
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Absyn_compress(t);{
void*_tmp1B6=Cyc_Tcutil_normalize_effect(Cyc_Absyn_compress(e));void*_stmttmp2C=_tmp1B6;void*_tmp1B7=_stmttmp2C;struct Cyc_Core_Opt*_tmp1BA;void**_tmp1B9;struct Cyc_Core_Opt*_tmp1B8;void*_tmp1BB;struct Cyc_List_List*_tmp1BC;switch(*((int*)_tmp1B7)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B7)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1BC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B7)->f2;_LL4: {struct Cyc_List_List*es=_tmp1BC;
# 877
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B7)->f2 != 0){_LL5: _tmp1BB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B7)->f2)->hd;_LL6: {void*t2=_tmp1BB;
# 882
t2=Cyc_Absyn_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1BD=Cyc_Tcutil_rgns_of(t);void*_stmttmp2D=_tmp1BD;void*_tmp1BE=_stmttmp2D;void*_tmp1BF;void*_tmp1C0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f2 != 0){_LLC: _tmp1C0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BE)->f2)->hd;_LLD: {void*t3=_tmp1C0;
# 887
struct _tuple15 _tmp1C1=({struct _tuple15 _tmp61D;({void*_tmp71B=Cyc_Absyn_compress(t3);_tmp61D.f1=_tmp71B;}),_tmp61D.f2=t2;_tmp61D;});struct _tuple15 _stmttmp2E=_tmp1C1;struct _tuple15 _tmp1C2=_stmttmp2E;struct Cyc_Absyn_Tvar*_tmp1C4;struct Cyc_Absyn_Tvar*_tmp1C3;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C2.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C2.f2)->tag == 2U){_LL11: _tmp1C3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C2.f1)->f1;_tmp1C4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C2.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1C3;struct Cyc_Absyn_Tvar*tv2=_tmp1C4;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1BF=_tmp1BE;_LLF: {void*e2=_tmp1BF;
# 891
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1B8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->f1;_tmp1B9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->f2;_tmp1BA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1B8;void**p=_tmp1B9;struct Cyc_Core_Opt*s=_tmp1BA;
# 894
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61E;_tmp61E.tag=0U,({struct _fat_ptr _tmp71C=({const char*_tmp1C7="effect evar has wrong kind";_tag_fat(_tmp1C7,sizeof(char),27U);});_tmp61E.f1=_tmp71C;});_tmp61E;});void*_tmp1C5[1U];_tmp1C5[0]=& _tmp1C6;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1C5,sizeof(void*),1U));});
if(!may_constrain_evars)return 0;{
# 899
void*_tmp1C8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1C8;
# 902
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1C9=Cyc_Absyn_join_eff(({void*_tmp1CA[2U];_tmp1CA[0]=ev,({void*_tmp71D=Cyc_Absyn_regionsof_eff(t);_tmp1CA[1]=_tmp71D;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CA,sizeof(void*),2U));}));void*new_typ=_tmp1C9;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 912
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp1CB=e;struct Cyc_Core_Opt*_tmp1CE;void**_tmp1CD;struct Cyc_Core_Opt*_tmp1CC;void*_tmp1CF;struct Cyc_List_List*_tmp1D0;struct Cyc_Absyn_Tvar*_tmp1D1;switch(*((int*)_tmp1CB)){case 2U: _LL1: _tmp1D1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CB)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1D1;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->f1)){case 9U: _LL3: _tmp1D0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->f2;_LL4: {struct Cyc_List_List*es=_tmp1D0;
# 917
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->f2 != 0){_LL5: _tmp1CF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CB)->f2)->hd;_LL6: {void*t=_tmp1CF;
# 922
void*_tmp1D2=Cyc_Tcutil_rgns_of(t);void*_stmttmp2F=_tmp1D2;void*_tmp1D3=_stmttmp2F;void*_tmp1D4;void*_tmp1D5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f2 != 0){_LLC: _tmp1D5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f2)->hd;_LLD: {void*t2=_tmp1D5;
# 924
if(!may_constrain_evars)return 0;{
void*_tmp1D6=Cyc_Absyn_compress(t2);void*_stmttmp30=_tmp1D6;void*_tmp1D7=_stmttmp30;struct Cyc_Core_Opt*_tmp1DA;void**_tmp1D9;struct Cyc_Core_Opt*_tmp1D8;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D7)->tag == 1U){_LL11: _tmp1D8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D7)->f1;_tmp1D9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D7)->f2;_tmp1DA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D7)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1D8;void**p=_tmp1D9;struct Cyc_Core_Opt*s=_tmp1DA;
# 930
void*_tmp1DB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1DB;
# 932
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp71F=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1DC[2U];_tmp1DC[0]=ev,({void*_tmp71E=Cyc_Absyn_var_type(v);_tmp1DC[1]=_tmp71E;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1DC,sizeof(void*),2U));})));*p=_tmp71F;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1D4=_tmp1D3;_LLF: {void*e2=_tmp1D4;
# 937
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1CC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CB)->f1;_tmp1CD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CB)->f2;_tmp1CE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CB)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1CC;void**p=_tmp1CD;struct Cyc_Core_Opt*s=_tmp1CE;
# 940
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61F;_tmp61F.tag=0U,({struct _fat_ptr _tmp720=({const char*_tmp1DF="effect evar has wrong kind";_tag_fat(_tmp1DF,sizeof(char),27U);});_tmp61F.f1=_tmp720;});_tmp61F;});void*_tmp1DD[1U];_tmp1DD[0]=& _tmp1DE;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1DD,sizeof(void*),1U));});{
# 944
void*_tmp1E0=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E0;
# 946
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1E1=Cyc_Absyn_join_eff(({void*_tmp1E2[2U];_tmp1E2[0]=ev,({void*_tmp721=Cyc_Absyn_var_type(v);_tmp1E2[1]=_tmp721;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E2,sizeof(void*),2U));}));void*new_typ=_tmp1E1;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 956
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp1E3=e;void*_tmp1E4;struct Cyc_List_List*_tmp1E5;switch(*((int*)_tmp1E3)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3)->f1)){case 9U: _LL1: _tmp1E5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3)->f2;_LL2: {struct Cyc_List_List*es=_tmp1E5;
# 960
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3)->f2 != 0){_LL3: _tmp1E4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E3)->f2)->hd;_LL4: {void*t=_tmp1E4;
# 965
void*_tmp1E6=Cyc_Tcutil_rgns_of(t);void*_stmttmp31=_tmp1E6;void*_tmp1E7=_stmttmp31;void*_tmp1E8;void*_tmp1E9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E7)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E7)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E7)->f2 != 0){_LLA: _tmp1E9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E7)->f2)->hd;_LLB: {void*t2=_tmp1E9;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1E8=_tmp1E7;_LLD: {void*e2=_tmp1E8;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 969
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 983 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 986
void*_tmp1EA=Cyc_Absyn_compress(e1);void*_stmttmp32=_tmp1EA;void*_tmp1EB=_stmttmp32;struct Cyc_Core_Opt*_tmp1ED;void**_tmp1EC;struct Cyc_Absyn_Tvar*_tmp1EE;void*_tmp1EF;void*_tmp1F0;struct Cyc_List_List*_tmp1F1;switch(*((int*)_tmp1EB)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f1)){case 9U: _LL1: _tmp1F1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2;_LL2: {struct Cyc_List_List*es=_tmp1F1;
# 988
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2 != 0){_LL3: _tmp1F0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2)->hd;_LL4: {void*r=_tmp1F0;
# 999
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2 != 0){_LL7: _tmp1EF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2)->hd;_LL8: {void*t=_tmp1EF;
# 1003
void*_tmp1F2=Cyc_Tcutil_rgns_of(t);void*_stmttmp33=_tmp1F2;void*_tmp1F3=_stmttmp33;void*_tmp1F4;void*_tmp1F5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2 != 0){_LLE: _tmp1F5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2)->hd;_LLF: {void*t2=_tmp1F5;
# 1008
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1F4=_tmp1F3;_LL11: {void*e=_tmp1F4;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1EE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EB)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1EE;
# 1001
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1EC=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EB)->f2;_tmp1ED=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EB)->f4;_LLA: {void**p=_tmp1EC;struct Cyc_Core_Opt*s=_tmp1ED;
# 1013
if(Cyc_Tcutil_evar_in_effect(e1,e2))
return 0;
# 1018
*p=Cyc_Absyn_empty_effect;
# 1021
return 1;}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp621;_tmp621.tag=0U,({struct _fat_ptr _tmp722=({const char*_tmp1F9="subset_effect: bad effect: ";_tag_fat(_tmp1F9,sizeof(char),28U);});_tmp621.f1=_tmp722;});_tmp621;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp620;_tmp620.tag=2U,_tmp620.f1=(void*)e1;_tmp620;});void*_tmp1F6[2U];_tmp1F6[0]=& _tmp1F7,_tmp1F6[1]=& _tmp1F8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1F6,sizeof(void*),2U));});}_LL0:;}
# 1032
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1034
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp1FA=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp34=_tmp1FA;struct _tuple15*_tmp1FB=_stmttmp34;void*_tmp1FD;void*_tmp1FC;_LL1: _tmp1FC=_tmp1FB->f1;_tmp1FD=_tmp1FB->f2;_LL2: {void*t1a=_tmp1FC;void*t1b=_tmp1FD;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp1FE=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp35=_tmp1FE;struct _tuple15*_tmp1FF=_stmttmp35;void*_tmp201;void*_tmp200;_LL4: _tmp200=_tmp1FF->f1;_tmp201=_tmp1FF->f2;_LL5: {void*t2a=_tmp200;void*t2b=_tmp201;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1044
if(!found)return 0;}}}
# 1046
return 1;}
# 1050
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1052
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1055
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp202=t;switch(*((int*)_tmp202)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp202)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp202)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1061
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1064
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1067
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1070
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp202)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1059
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1062
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1065
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1068
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1071
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp202)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1060
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1063
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1066
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1069
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1072
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1094
({struct Cyc_Warn_String_Warn_Warg_struct _tmp204=({struct Cyc_Warn_String_Warn_Warg_struct _tmp622;_tmp622.tag=0U,({struct _fat_ptr _tmp723=({const char*_tmp205="bad con";_tag_fat(_tmp205,sizeof(char),8U);});_tmp622.f1=_tmp723;});_tmp622;});void*_tmp203[1U];_tmp203[0]=& _tmp204;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp203,sizeof(void*),1U));});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp202)->f1){case 0U: _LL21: _LL22:
# 1073
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
# 1097
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp206=t;void*_tmp207;switch(*((int*)_tmp206)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp207=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp206)->f1;_LL18: {void*c=_tmp207;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1113
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp208=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp208;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1118
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp209=i;struct _tuple1*_tmp20A;struct Cyc_Absyn_Datatypedecl*_tmp20B;if((_tmp209.KnownDatatype).tag == 2){_LL1: _tmp20B=*(_tmp209.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp20B;
return dd->name;}}else{_LL3: _tmp20A=((_tmp209.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp20A;
return n;}}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1124
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp20C=i;struct _tuple1*_tmp20E;struct _tuple1*_tmp20D;struct Cyc_Absyn_Datatypefield*_tmp210;struct Cyc_Absyn_Datatypedecl*_tmp20F;if((_tmp20C.KnownDatatypefield).tag == 2){_LL1: _tmp20F=((_tmp20C.KnownDatatypefield).val).f1;_tmp210=((_tmp20C.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp20F;struct Cyc_Absyn_Datatypefield*df=_tmp210;
# 1127
return({struct _tuple20 _tmp623;_tmp623.f1=dd->name,_tmp623.f2=df->name;_tmp623;});}}else{_LL3: _tmp20D=((_tmp20C.UnknownDatatypefield).val).datatype_name;_tmp20E=((_tmp20C.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp20D;struct _tuple1*f=_tmp20E;
# 1129
return({struct _tuple20 _tmp624;_tmp624.f1=d,_tmp624.f2=f;_tmp624;});}}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1132
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp211=i;struct Cyc_Absyn_Aggrdecl*_tmp212;struct _tuple1*_tmp214;enum Cyc_Absyn_AggrKind _tmp213;if((_tmp211.UnknownAggr).tag == 1){_LL1: _tmp213=((_tmp211.UnknownAggr).val).f1;_tmp214=((_tmp211.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp213;struct _tuple1*n=_tmp214;
return({struct _tuple21 _tmp625;_tmp625.f1=k,_tmp625.f2=n;_tmp625;});}}else{_LL3: _tmp212=*(_tmp211.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp212;
return({struct _tuple21 _tmp626;_tmp626.f1=ad->kind,_tmp626.f2=ad->name;_tmp626;});}}_LL0:;}
# 1138
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1144
struct _tuple15 _tmp215=({struct _tuple15 _tmp627;_tmp627.f1=t1,_tmp627.f2=t2;_tmp627;});struct _tuple15 _stmttmp36=_tmp215;struct _tuple15 _tmp216=_stmttmp36;union Cyc_Absyn_AggrInfo _tmp218;union Cyc_Absyn_AggrInfo _tmp217;union Cyc_Absyn_DatatypeFieldInfo _tmp21A;union Cyc_Absyn_DatatypeFieldInfo _tmp219;union Cyc_Absyn_DatatypeInfo _tmp21C;union Cyc_Absyn_DatatypeInfo _tmp21B;struct Cyc_List_List*_tmp21E;struct Cyc_List_List*_tmp21D;struct _fat_ptr _tmp220;struct _fat_ptr _tmp21F;struct _tuple1*_tmp222;struct _tuple1*_tmp221;switch(*((int*)_tmp216.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp216.f2)->tag == 15U){_LL1: _tmp221=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp216.f1)->f1;_tmp222=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp216.f2)->f1;_LL2: {struct _tuple1*n1=_tmp221;struct _tuple1*n2=_tmp222;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp216.f2)->tag == 17U){_LL3: _tmp21F=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp216.f1)->f1;_tmp220=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp216.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp21F;struct _fat_ptr s2=_tmp220;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp216.f2)->tag == 16U){_LL5: _tmp21D=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp216.f1)->f1;_tmp21E=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp216.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp21D;struct Cyc_List_List*fs2=_tmp21E;
# 1148
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp216.f2)->tag == 18U){_LL7: _tmp21B=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp216.f1)->f1;_tmp21C=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp216.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp21B;union Cyc_Absyn_DatatypeInfo info2=_tmp21C;
# 1150
return({struct _tuple1*_tmp724=Cyc_Tcutil_get_datatype_qvar(info1);Cyc_Absyn_qvar_cmp(_tmp724,Cyc_Tcutil_get_datatype_qvar(info2));});}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp216.f2)->tag == 19U){_LL9: _tmp219=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp216.f1)->f1;_tmp21A=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp216.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp219;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp21A;
# 1152
struct _tuple20 _tmp223=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple20 _stmttmp37=_tmp223;struct _tuple20 _tmp224=_stmttmp37;struct _tuple1*_tmp226;struct _tuple1*_tmp225;_LL10: _tmp225=_tmp224.f1;_tmp226=_tmp224.f2;_LL11: {struct _tuple1*d1=_tmp225;struct _tuple1*f1=_tmp226;
struct _tuple20 _tmp227=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple20 _stmttmp38=_tmp227;struct _tuple20 _tmp228=_stmttmp38;struct _tuple1*_tmp22A;struct _tuple1*_tmp229;_LL13: _tmp229=_tmp228.f1;_tmp22A=_tmp228.f2;_LL14: {struct _tuple1*d2=_tmp229;struct _tuple1*f2=_tmp22A;
int _tmp22B=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp22B;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp216.f2)->tag == 20U){_LLB: _tmp217=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp216.f1)->f1;_tmp218=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp216.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp217;union Cyc_Absyn_AggrInfo info2=_tmp218;
# 1158
struct _tuple21 _tmp22C=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple21 _stmttmp39=_tmp22C;struct _tuple21 _tmp22D=_stmttmp39;struct _tuple1*_tmp22F;enum Cyc_Absyn_AggrKind _tmp22E;_LL16: _tmp22E=_tmp22D.f1;_tmp22F=_tmp22D.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp22E;struct _tuple1*q1=_tmp22F;
struct _tuple21 _tmp230=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple21 _stmttmp3A=_tmp230;struct _tuple21 _tmp231=_stmttmp3A;struct _tuple1*_tmp233;enum Cyc_Absyn_AggrKind _tmp232;_LL19: _tmp232=_tmp231.f1;_tmp233=_tmp231.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp232;struct _tuple1*q2=_tmp233;
int _tmp234=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp234;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1167
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp726)(void*,void*)=cmp;void*_tmp725=(void*)_check_null(a1);_tmp726(_tmp725,(void*)_check_null(a2));});}
# 1173
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp235=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp235;
int _tmp236=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp236;
return Cyc_Core_intcmp(i1,i2);}
# 1178
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp237=tqt1;void*_tmp239;struct Cyc_Absyn_Tqual _tmp238;_LL1: _tmp238=_tmp237->f1;_tmp239=_tmp237->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp238;void*t1=_tmp239;
struct _tuple12*_tmp23A=tqt2;void*_tmp23C;struct Cyc_Absyn_Tqual _tmp23B;_LL4: _tmp23B=_tmp23A->f1;_tmp23C=_tmp23A->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp23B;void*t2=_tmp23C;
int _tmp23D=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp23D;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1186
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp23E=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp23E;
if(zsc != 0)return zsc;{
int _tmp23F=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp23F;
if(tqc != 0)return tqc;{
int _tmp240=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp240;
if(tc != 0)return tc;{
int _tmp241=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Atts_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp241;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1203
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1 == t2)return 0;{
int _tmp242=({int _tmp727=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp727,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp242;
if(shallowcmp != 0)
return shallowcmp;{
# 1212
struct _tuple15 _tmp243=({struct _tuple15 _tmp62C;_tmp62C.f1=t2,_tmp62C.f2=t1;_tmp62C;});struct _tuple15 _stmttmp3B=_tmp243;struct _tuple15 _tmp244=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Exp*_tmp245;struct Cyc_Absyn_Exp*_tmp248;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_List_List*_tmp24C;enum Cyc_Absyn_AggrKind _tmp24B;struct Cyc_List_List*_tmp24A;enum Cyc_Absyn_AggrKind _tmp249;struct Cyc_List_List*_tmp24E;struct Cyc_List_List*_tmp24D;struct Cyc_Absyn_FnInfo _tmp250;struct Cyc_Absyn_FnInfo _tmp24F;void*_tmp258;struct Cyc_Absyn_Exp*_tmp257;struct Cyc_Absyn_Tqual _tmp256;void*_tmp255;void*_tmp254;struct Cyc_Absyn_Exp*_tmp253;struct Cyc_Absyn_Tqual _tmp252;void*_tmp251;void*_tmp264;void*_tmp263;void*_tmp262;void*_tmp261;struct Cyc_Absyn_Tqual _tmp260;void*_tmp25F;void*_tmp25E;void*_tmp25D;void*_tmp25C;void*_tmp25B;struct Cyc_Absyn_Tqual _tmp25A;void*_tmp259;struct Cyc_Absyn_Tvar*_tmp266;struct Cyc_Absyn_Tvar*_tmp265;struct Cyc_List_List*_tmp26A;void*_tmp269;struct Cyc_List_List*_tmp268;void*_tmp267;switch(*((int*)_tmp244.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244.f2)->tag == 0U){_LL1: _tmp267=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244.f1)->f1;_tmp268=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244.f1)->f2;_tmp269=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244.f2)->f1;_tmp26A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp244.f2)->f2;_LL2: {void*c1=_tmp267;struct Cyc_List_List*ts1=_tmp268;void*c2=_tmp269;struct Cyc_List_List*ts2=_tmp26A;
# 1214
int _tmp26B=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp26B;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp244.f2)->tag == 1U){_LL3: _LL4:
# 1218
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp628;_tmp628.tag=0U,({struct _fat_ptr _tmp728=({const char*_tmp26E="typecmp: can only compare closed types";_tag_fat(_tmp26E,sizeof(char),39U);});_tmp628.f1=_tmp728;});_tmp628;});void*_tmp26C[1U];_tmp26C[0]=& _tmp26D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp26C,sizeof(void*),1U));});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp244.f2)->tag == 2U){_LL5: _tmp265=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp244.f1)->f1;_tmp266=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp244.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp265;struct Cyc_Absyn_Tvar*tv1=_tmp266;
# 1222
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f2)->tag == 3U){_LL7: _tmp259=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f1)->f1).elt_type;_tmp25A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f1)->f1).elt_tq;_tmp25B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f1)->f1).ptr_atts).rgn;_tmp25C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f1)->f1).ptr_atts).nullable;_tmp25D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f1)->f1).ptr_atts).bounds;_tmp25E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f1)->f1).ptr_atts).zero_term;_tmp25F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f2)->f1).elt_type;_tmp260=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f2)->f1).elt_tq;_tmp261=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f2)->f1).ptr_atts).rgn;_tmp262=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f2)->f1).ptr_atts).nullable;_tmp263=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f2)->f1).ptr_atts).bounds;_tmp264=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp244.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp259;struct Cyc_Absyn_Tqual tqual2a=_tmp25A;void*rgn2=_tmp25B;void*null2a=_tmp25C;void*b2=_tmp25D;void*zt2=_tmp25E;void*t1a=_tmp25F;struct Cyc_Absyn_Tqual tqual1a=_tmp260;void*rgn1=_tmp261;void*null1a=_tmp262;void*b1=_tmp263;void*zt1=_tmp264;
# 1226
int _tmp26F=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp26F;
if(etc != 0)return etc;{
int _tmp270=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp270;
if(rc != 0)return rc;{
int _tmp271=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp271;
if(tqc != 0)return tqc;{
int _tmp272=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp272;
if(cc != 0)return cc;{
int _tmp273=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp273;
if(zc != 0)return zc;{
int _tmp274=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp274;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f2)->tag == 4U){_LL9: _tmp251=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f1)->f1).elt_type;_tmp252=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f1)->f1).tq;_tmp253=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f1)->f1).num_elts;_tmp254=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f1)->f1).zero_term;_tmp255=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f2)->f1).elt_type;_tmp256=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f2)->f1).tq;_tmp257=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f2)->f1).num_elts;_tmp258=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp244.f2)->f1).zero_term;_LLA: {void*t2a=_tmp251;struct Cyc_Absyn_Tqual tq2a=_tmp252;struct Cyc_Absyn_Exp*e1=_tmp253;void*zt1=_tmp254;void*t1a=_tmp255;struct Cyc_Absyn_Tqual tq1a=_tmp256;struct Cyc_Absyn_Exp*e2=_tmp257;void*zt2=_tmp258;
# 1242
int _tmp275=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp275;
if(tqc != 0)return tqc;{
int _tmp276=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp276;
if(tc != 0)return tc;{
int _tmp277=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp277;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp279=({struct Cyc_Warn_String_Warn_Warg_struct _tmp629;_tmp629.tag=0U,({struct _fat_ptr _tmp729=({const char*_tmp27A="missing expression in array index";_tag_fat(_tmp27A,sizeof(char),34U);});_tmp629.f1=_tmp729;});_tmp629;});void*_tmp278[1U];_tmp278[0]=& _tmp279;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp278,sizeof(void*),1U));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp244.f2)->tag == 5U){_LLB: _tmp24F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp244.f1)->f1;_tmp250=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp244.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp24F;struct Cyc_Absyn_FnInfo f2=_tmp250;
# 1254
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp27B=f1.args;struct Cyc_List_List*args1=_tmp27B;
struct Cyc_List_List*_tmp27C=f2.args;struct Cyc_List_List*args2=_tmp27C;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp27D=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3C=_tmp27D;struct _tuple9 _tmp27E=_stmttmp3C;void*_tmp280;struct Cyc_Absyn_Tqual _tmp27F;_LL18: _tmp27F=_tmp27E.f2;_tmp280=_tmp27E.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp27F;void*t1=_tmp280;
struct _tuple9 _tmp281=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp3D=_tmp281;struct _tuple9 _tmp282=_stmttmp3D;void*_tmp284;struct Cyc_Absyn_Tqual _tmp283;_LL1B: _tmp283=_tmp282.f2;_tmp284=_tmp282.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp283;void*t2=_tmp284;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1269
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp72A=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp72A,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1283
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp285=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp285;
struct Cyc_List_List*_tmp286=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp286;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp287=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp3E=_tmp287;struct _tuple15 _tmp288=_stmttmp3E;void*_tmp28A;void*_tmp289;_LL1E: _tmp289=_tmp288.f1;_tmp28A=_tmp288.f2;_LL1F: {void*t1a=_tmp289;void*t1b=_tmp28A;
struct _tuple15 _tmp28B=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp3F=_tmp28B;struct _tuple15 _tmp28C=_stmttmp3F;void*_tmp28E;void*_tmp28D;_LL21: _tmp28D=_tmp28C.f1;_tmp28E=_tmp28C.f2;_LL22: {void*t2a=_tmp28D;void*t2b=_tmp28E;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1293
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1301
({struct Cyc_Warn_String_Warn_Warg_struct _tmp290=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62A;_tmp62A.tag=0U,({struct _fat_ptr _tmp72B=({const char*_tmp291="typecmp: function type comparison should never get here!";_tag_fat(_tmp291,sizeof(char),57U);});_tmp62A.f1=_tmp72B;});_tmp62A;});void*_tmp28F[1U];_tmp28F[0]=& _tmp290;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp28F,sizeof(void*),1U));});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp244.f2)->tag == 6U){_LLD: _tmp24D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp244.f1)->f1;_tmp24E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp244.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp24D;struct Cyc_List_List*ts1=_tmp24E;
# 1304
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp244.f2)->tag == 7U){_LLF: _tmp249=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp244.f1)->f1;_tmp24A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp244.f1)->f2;_tmp24B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp244.f2)->f1;_tmp24C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp244.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp249;struct Cyc_List_List*fs2=_tmp24A;enum Cyc_Absyn_AggrKind k1=_tmp24B;struct Cyc_List_List*fs1=_tmp24C;
# 1307
if((int)k1 != (int)k2)
return(int)k1 == (int)0U?- 1: 1;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp244.f2)->tag == 9U){_LL11: _tmp247=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp244.f1)->f1;_tmp248=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp244.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp247;struct Cyc_Absyn_Exp*e2=_tmp248;
# 1311
_tmp245=e1;_tmp246=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp244.f2)->tag == 11U){_LL13: _tmp245=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp244.f1)->f1;_tmp246=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp244.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp245;struct Cyc_Absyn_Exp*e2=_tmp246;
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp293=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62B;_tmp62B.tag=0U,({struct _fat_ptr _tmp72C=({const char*_tmp294="Unmatched case in typecmp";_tag_fat(_tmp294,sizeof(char),26U);});_tmp62B.f1=_tmp72C;});_tmp62B;});void*_tmp292[1U];_tmp292[0]=& _tmp293;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp292,sizeof(void*),1U));});}_LL0:;}}}
# 1319
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp295=({struct _tuple15 _tmp62E;({void*_tmp72E=Cyc_Absyn_compress(t1);_tmp62E.f1=_tmp72E;}),({void*_tmp72D=Cyc_Absyn_compress(t2);_tmp62E.f2=_tmp72D;});_tmp62E;});struct _tuple15 _stmttmp40=_tmp295;struct _tuple15 _tmp296=_stmttmp40;void*_tmp298;void*_tmp297;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296.f2)->tag == 0U){_LL1: _tmp297=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296.f1)->f1;_tmp298=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp296.f2)->f1;_LL2: {void*c1=_tmp297;void*c2=_tmp298;
# 1322
struct _tuple15 _tmp299=({struct _tuple15 _tmp62D;_tmp62D.f1=c1,_tmp62D.f2=c2;_tmp62D;});struct _tuple15 _stmttmp41=_tmp299;struct _tuple15 _tmp29A=_stmttmp41;int _tmp29C;int _tmp29B;switch(*((int*)_tmp29A.f1)){case 2U: switch(*((int*)_tmp29A.f2)){case 2U: _LL6: _tmp29B=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29A.f1)->f1;_tmp29C=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29A.f2)->f1;_LL7: {int i1=_tmp29B;int i2=_tmp29C;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp29A.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1330
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1335
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1338
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29A.f2)->f1 == 0){_LL14: _LL15:
# 1333
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp29A.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1331
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1336
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1339
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29A.f2)->f1 == 0){_LL16: _LL17:
# 1334
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1340
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29A.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1337
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1341
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1343
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1345
 return 0;}_LL0:;}
# 1349
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp29D=({struct _tuple15 _tmp630;({void*_tmp730=Cyc_Absyn_compress(t1);_tmp630.f1=_tmp730;}),({void*_tmp72F=Cyc_Absyn_compress(t2);_tmp630.f2=_tmp72F;});_tmp630;});struct _tuple15 _stmttmp42=_tmp29D;struct _tuple15 _tmp29E=_stmttmp42;void*_tmp2A0;void*_tmp29F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E.f2)->tag == 0U){_LL1: _tmp29F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E.f1)->f1;_tmp2A0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29E.f2)->f1;_LL2: {void*c1=_tmp29F;void*c2=_tmp2A0;
# 1352
{struct _tuple15 _tmp2A1=({struct _tuple15 _tmp62F;_tmp62F.f1=c1,_tmp62F.f2=c2;_tmp62F;});struct _tuple15 _stmttmp43=_tmp2A1;struct _tuple15 _tmp2A2=_stmttmp43;int _tmp2A4;int _tmp2A3;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A2.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A2.f2)->tag == 2U){_LL6: _tmp2A3=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f1;_tmp2A4=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f1;_LL7: {int i1=_tmp2A3;int i2=_tmp2A4;
# 1354
if(i1 != 0 && i1 != 1)return t1;
if(i2 != 0 && i2 != 1)return t2;
if(i1 >= i2)return t1;
return t2;}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A2.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1364
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A2.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1369
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1362
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A2.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1371
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1361
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1365
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A2.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1370
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1363
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A2.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A2.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1372
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A2.f1)->tag == 4U){_LL18: _LL19:
# 1367
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A2.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1373
 goto _LL5;}}}}}}_LL5:;}
# 1375
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1378
return Cyc_Absyn_sint_type;}
# 1383
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1386
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Absyn_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2A5=_cycalloc(sizeof(*_tmp2A5));_tmp2A5->v=t1;_tmp2A5;});}}}
# 1394
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1398
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp634;_tmp634.tag=0U,({struct _fat_ptr _tmp731=({const char*_tmp2AC="type mismatch: expecting ";_tag_fat(_tmp2AC,sizeof(char),26U);});_tmp634.f1=_tmp731;});_tmp634;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp633;_tmp633.tag=2U,_tmp633.f1=(void*)t;_tmp633;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2A9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp632;_tmp632.tag=0U,({struct _fat_ptr _tmp732=({const char*_tmp2AB=" but found ";_tag_fat(_tmp2AB,sizeof(char),12U);});_tmp632.f1=_tmp732;});_tmp632;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2AA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp631;_tmp631.tag=2U,_tmp631.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt);_tmp631;});void*_tmp2A6[4U];_tmp2A6[0]=& _tmp2A7,_tmp2A6[1]=& _tmp2A8,_tmp2A6[2]=& _tmp2A9,_tmp2A6[3]=& _tmp2AA;({unsigned _tmp733=((struct Cyc_Absyn_Exp*)el->hd)->loc;Cyc_Warn_err2(_tmp733,_tag_fat(_tmp2A6,sizeof(void*),4U));});});
# 1402
return 0;}}}
# 1404
return 1;}
# 1409
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2AD=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp44=_tmp2AD;void*_tmp2AE=_stmttmp44;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AE)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1415
return 1;}
# 1418
static int Cyc_Tcutil_coerce_Xint_type(void*Xint_type,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Xint_type))
return 1;
# 1422
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Xint_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp734=({const char*_tmp2B1="integral size mismatch; conversion supplied";_tag_fat(_tmp2B1,sizeof(char),44U);});_tmp635.f1=_tmp734;});_tmp635;});void*_tmp2AF[1U];_tmp2AF[0]=& _tmp2B0;({unsigned _tmp735=e->loc;Cyc_Warn_warn2(_tmp735,_tag_fat(_tmp2AF,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e,Xint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1428
return 0;}
# 1431
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_uint_type,e);}
# 1435
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_sint_type,e);}
# 1441
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1447
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp736=t;Cyc_Unify_unify(_tmp736,Cyc_Absyn_bounds_one());});
return Cyc_Absyn_compress(t);}
# 1452
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2B2=Cyc_Absyn_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2B2;
void*_tmp2B3=t;struct Cyc_Absyn_Exp*_tmp2B4;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B3)->tag == 9U){_LL1: _tmp2B4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B3)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2B4;
return e;}}else{_LL3: _LL4: {
# 1457
struct Cyc_Absyn_Exp*_tmp2B5=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2B5;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1466
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B6=Cyc_Absyn_compress(b);void*_stmttmp45=_tmp2B6;void*_tmp2B7=_stmttmp45;struct Cyc_List_List*_tmp2B8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2B8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2B8;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp637;_tmp637.tag=0U,({struct _fat_ptr _tmp737=({const char*_tmp2BC="get_bounds_exp: ";_tag_fat(_tmp2BC,sizeof(char),17U);});_tmp637.f1=_tmp737;});_tmp637;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp636;_tmp636.tag=2U,_tmp636.f1=(void*)b;_tmp636;});void*_tmp2B9[2U];_tmp2B9[0]=& _tmp2BA,_tmp2B9[1]=& _tmp2BB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2B9,sizeof(void*),2U));});}_LL0:;}}
# 1475
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2BD=Cyc_Absyn_compress(t);void*_stmttmp46=_tmp2BD;void*_tmp2BE=_stmttmp46;void*_tmp2BF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BE)->tag == 3U){_LL1: _tmp2BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BE)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2BF;
# 1478
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp639;_tmp639.tag=0U,({struct _fat_ptr _tmp738=({const char*_tmp2C3="get_ptr_bounds_exp not pointer: ";_tag_fat(_tmp2C3,sizeof(char),33U);});_tmp639.f1=_tmp738;});_tmp639;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp638;_tmp638.tag=2U,_tmp638.f1=(void*)t;_tmp638;});void*_tmp2C0[2U];_tmp2C0[0]=& _tmp2C1,_tmp2C0[1]=& _tmp2C2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C0,sizeof(void*),2U));});}_LL0:;}
# 1484
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4->v=tvs;_tmp2C4;}));}
# 1488
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5->v=tvs;_tmp2C5;}));}
# 1493
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1500
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp2C6=({struct _tuple15 _tmp640;_tmp640.f1=t1,_tmp640.f2=t2;_tmp640;});struct _tuple15 _stmttmp47=_tmp2C6;struct _tuple15 _tmp2C7=_stmttmp47;void*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Absyn_Tqual _tmp2CD;void*_tmp2CC;void*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Tqual _tmp2C9;void*_tmp2C8;struct Cyc_Absyn_PtrInfo _tmp2D1;struct Cyc_Absyn_PtrInfo _tmp2D0;switch(*((int*)_tmp2C7.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7.f2)->tag == 3U){_LL1: _tmp2D0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7.f1)->f1;_tmp2D1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2D0;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2D1;
# 1506
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable)&&
 Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable))
return 0;
# 1510
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple22 _tmp2D2=({struct _tuple22 _tmp63B;({struct Cyc_Absyn_Exp*_tmp73C=({void*_tmp73B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp73B,(pinfo_a.ptr_atts).bounds);});_tmp63B.f1=_tmp73C;}),({
struct Cyc_Absyn_Exp*_tmp73A=({void*_tmp739=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp739,(pinfo_b.ptr_atts).bounds);});_tmp63B.f2=_tmp73A;});_tmp63B;});
# 1511
struct _tuple22 _stmttmp48=_tmp2D2;struct _tuple22 _tmp2D3=_stmttmp48;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D4;if(_tmp2D3.f2 == 0){_LLA: _LLB:
# 1514
 goto _LL9;}else{if(_tmp2D3.f1 == 0){_LLC: _LLD:
# 1517
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp73D=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp73D,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
return 0;}else{_LLE: _tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2D4;struct Cyc_Absyn_Exp*e2=_tmp2D5;
# 1525
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A;_tmp63A.tag=0U,({struct _fat_ptr _tmp73E=({const char*_tmp2D8="implicit cast to shorter array";_tag_fat(_tmp2D8,sizeof(char),31U);});_tmp63A.f1=_tmp73E;});_tmp63A;});void*_tmp2D6[1U];_tmp2D6[0]=& _tmp2D7;({unsigned _tmp73F=loc;Cyc_Warn_warn2(_tmp73F,_tag_fat(_tmp2D6,sizeof(void*),1U));});});
if(!({struct Cyc_Absyn_Exp*_tmp740=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp740,(struct Cyc_Absyn_Exp*)_check_null(e1));}))
return 0;
goto _LL9;}}}_LL9:;}
# 1533
if((pinfo_a.elt_tq).real_const && !(pinfo_b.elt_tq).real_const)
return 0;
# 1536
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(!Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn))
# 1539
return 0;
if(Cyc_Flags_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63F;_tmp63F.tag=0U,({struct _fat_ptr _tmp741=({const char*_tmp2DF="implicit cast from region ";_tag_fat(_tmp2DF,sizeof(char),27U);});_tmp63F.f1=_tmp741;});_tmp63F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63E;_tmp63E.tag=2U,_tmp63E.f1=(void*)(pinfo_a.ptr_atts).rgn;_tmp63E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D;_tmp63D.tag=0U,({
struct _fat_ptr _tmp742=({const char*_tmp2DE=" to region ";_tag_fat(_tmp2DE,sizeof(char),12U);});_tmp63D.f1=_tmp742;});_tmp63D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63C;_tmp63C.tag=2U,_tmp63C.f1=(void*)(pinfo_b.ptr_atts).rgn;_tmp63C;});void*_tmp2D9[4U];_tmp2D9[0]=& _tmp2DA,_tmp2D9[1]=& _tmp2DB,_tmp2D9[2]=& _tmp2DC,_tmp2D9[3]=& _tmp2DD;({unsigned _tmp743=loc;Cyc_Warn_warn2(_tmp743,_tag_fat(_tmp2D9,sizeof(void*),4U));});});}
# 1545
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)&&(
!Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)|| !(pinfo_b.elt_tq).real_const))
# 1548
return 0;
# 1552
if(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type))
return 1;
# 1556
if(!({void*_tmp744=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp744,(pinfo_b.ptr_atts).bounds);})||
 Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
return 0;
if(!(pinfo_b.elt_tq).real_const && !Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type)))
return 0;
return Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type);}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->tag == 4U){_LL3: _tmp2C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f1)->f1).elt_type;_tmp2C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f1)->f1).tq;_tmp2CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f1)->f1).num_elts;_tmp2CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f1)->f1).zero_term;_tmp2CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->f1).elt_type;_tmp2CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->f1).tq;_tmp2CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->f1).num_elts;_tmp2CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2C8;struct Cyc_Absyn_Tqual tq1a=_tmp2C9;struct Cyc_Absyn_Exp*e1=_tmp2CA;void*zt1=_tmp2CB;void*t2a=_tmp2CC;struct Cyc_Absyn_Tqual tq2a=_tmp2CD;struct Cyc_Absyn_Exp*e2=_tmp2CE;void*zt2=_tmp2CF;
# 1565
if(!Cyc_Unify_unify(zt1,zt2))return 0;
if(e1 == 0 || e2 == 0)return 0;
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))return 0;
if(!Cyc_Unify_unify(t1a,t2a))return 0;
# 1570
return !tq1a.real_const || tq2a.real_const;}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1572
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1574
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1578
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2E0=Cyc_Absyn_compress(t);void*_stmttmp49=_tmp2E0;void*_tmp2E1=_stmttmp49;void*_tmp2E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->tag == 3U){_LL1: _tmp2E2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).elt_type;_LL2: {void*e=_tmp2E2;
return e;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp641;_tmp641.tag=0U,({struct _fat_ptr _tmp745=({const char*_tmp2E5="pointer_elt_type";_tag_fat(_tmp2E5,sizeof(char),17U);});_tmp641.f1=_tmp745;});_tmp641;});void*_tmp2E3[1U];_tmp2E3[0]=& _tmp2E4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2E3,sizeof(void*),1U));});}_LL0:;}
# 1584
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2E6=Cyc_Absyn_compress(t);void*_stmttmp4A=_tmp2E6;void*_tmp2E7=_stmttmp4A;struct Cyc_Absyn_PtrAtts*_tmp2E8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E7)->tag == 3U){_LL1: _tmp2E8=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E7)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2E8;
return p->rgn;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp642;_tmp642.tag=0U,({struct _fat_ptr _tmp746=({const char*_tmp2EB="pointer_elt_type";_tag_fat(_tmp2EB,sizeof(char),17U);});_tmp642.f1=_tmp746;});_tmp642;});void*_tmp2E9[1U];_tmp2E9[0]=& _tmp2EA;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2E9,sizeof(void*),1U));});}_LL0:;}
# 1591
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2EC=Cyc_Absyn_compress(t);void*_stmttmp4B=_tmp2EC;void*_tmp2ED=_stmttmp4B;void*_tmp2EE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2ED)->tag == 3U){_LL1: _tmp2EE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2ED)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2EE;
*rgn=r;return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1601
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2EF=Cyc_Absyn_compress(t);void*_stmttmp4C=_tmp2EF;void*_tmp2F0=_stmttmp4C;void*_tmp2F2;void*_tmp2F1;switch(*((int*)_tmp2F0)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F0)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2F1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).ptr_atts).nullable;_tmp2F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F0)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2F1;void*bounds=_tmp2F2;
# 1609
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1615
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2F3=e->r;void*_stmttmp4D=_tmp2F3;void*_tmp2F4=_stmttmp4D;struct Cyc_Absyn_Exp*_tmp2F6;void*_tmp2F5;struct _fat_ptr _tmp2F7;switch(*((int*)_tmp2F4)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F4)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F4)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F4)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F4)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F4)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2F7=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F4)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp2F7;
# 1623
unsigned long _tmp2F8=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2F8;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}
# 1633
return 0;}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1621
 return 1;case 14U: _LLD: _tmp2F5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F4)->f1;_tmp2F6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F4)->f2;_LLE: {void*t=_tmp2F5;struct Cyc_Absyn_Exp*e2=_tmp2F6;
# 1634
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1639
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1648
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1653
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1658
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1667
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1672
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1677
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2F9=ka;enum Cyc_Absyn_AliasQual _tmp2FB;enum Cyc_Absyn_KindQual _tmp2FA;_LL1: _tmp2FA=_tmp2F9->kind;_tmp2FB=_tmp2F9->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp2FA;enum Cyc_Absyn_AliasQual a=_tmp2FB;
{enum Cyc_Absyn_AliasQual _tmp2FC=a;switch(_tmp2FC){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1681
enum Cyc_Absyn_KindQual _tmp2FD=k;switch(_tmp2FD){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1692
{enum Cyc_Absyn_KindQual _tmp2FE=k;switch(_tmp2FE){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1699
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1701
{enum Cyc_Absyn_KindQual _tmp2FF=k;switch(_tmp2FF){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1708
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1711
({struct Cyc_Warn_String_Warn_Warg_struct _tmp301=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0U,({struct _fat_ptr _tmp747=({const char*_tmp303="kind_to_opt: bad kind ";_tag_fat(_tmp303,sizeof(char),23U);});_tmp644.f1=_tmp747;});_tmp644;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp302=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp643;_tmp643.tag=9U,_tmp643.f1=ka;_tmp643;});void*_tmp300[2U];_tmp300[0]=& _tmp301,_tmp300[1]=& _tmp302;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp300,sizeof(void*),2U));});}}
# 1714
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->tag=0U,_tmp304->f1=k;_tmp304;});}
# 1717
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp305=_cycalloc(sizeof(*_tmp305));({void*_tmp748=Cyc_Tcutil_kind_to_bound(k);_tmp305->v=_tmp748;});_tmp305;});}
# 1723
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp306=Cyc_Absyn_compress(t2);void*_stmttmp4E=_tmp306;void*_tmp307=_stmttmp4E;void*_tmp308;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp307)->tag == 3U){_LL1: _tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp307)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp308;
# 1728
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp749=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp749;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1741
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1745
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp74C=({struct _fat_ptr*_tmp31B=_cycalloc(sizeof(*_tmp31B));({struct _fat_ptr _tmp74B=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp31A=({struct Cyc_Int_pa_PrintArg_struct _tmp646;_tmp646.tag=1U,_tmp646.f1=(unsigned long)counter ++;_tmp646;});void*_tmp318[1U];_tmp318[0]=& _tmp31A;({struct _fat_ptr _tmp74A=({const char*_tmp319="__aliasvar%d";_tag_fat(_tmp319,sizeof(char),13U);});Cyc_aprintf(_tmp74A,_tag_fat(_tmp318,sizeof(void*),1U));});});*_tmp31B=_tmp74B;});_tmp31B;});_tmp31C->f2=_tmp74C;});_tmp31C;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp74D=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->tag=4U,_tmp317->f1=vd;_tmp317;});Cyc_Absyn_varb_exp(_tmp74D,e->loc);});
# 1754
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp309=Cyc_Absyn_compress(e_type);void*_stmttmp4F=_tmp309;void*_tmp30A=_stmttmp4F;struct Cyc_Absyn_PtrLoc*_tmp311;void*_tmp310;void*_tmp30F;void*_tmp30E;void*_tmp30D;struct Cyc_Absyn_Tqual _tmp30C;void*_tmp30B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->tag == 3U){_LL1: _tmp30B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).elt_type;_tmp30C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).elt_tq;_tmp30D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).rgn;_tmp30E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).nullable;_tmp30F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).bounds;_tmp310=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).zero_term;_tmp311=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp30B;struct Cyc_Absyn_Tqual tq=_tmp30C;void*old_r=_tmp30D;void*nb=_tmp30E;void*b=_tmp30F;void*zt=_tmp310;struct Cyc_Absyn_PtrLoc*pl=_tmp311;
# 1757
{void*_tmp312=Cyc_Absyn_compress(old_r);void*_stmttmp50=_tmp312;void*_tmp313=_stmttmp50;struct Cyc_Core_Opt*_tmp315;void**_tmp314;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->tag == 1U){_LL6: _tmp314=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->f2;_tmp315=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->f4;_LL7: {void**topt=_tmp314;struct Cyc_Core_Opt*ts=_tmp315;
# 1759
void*_tmp316=Cyc_Absyn_var_type(tv);void*new_r=_tmp316;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1764
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1767
e->topt=0;
vd->initializer=0;{
# 1771
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1773
return({struct _tuple23 _tmp645;_tmp645.f1=d,_tmp645.f2=ve;_tmp645;});}}
# 1778
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1781
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1786
void*_tmp31D=Cyc_Absyn_compress(wants_type);void*_stmttmp51=_tmp31D;void*_tmp31E=_stmttmp51;void*_tmp31F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31E)->tag == 3U){_LL1: _tmp31F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31E)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp31F;
# 1788
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp320=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp320;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1794
return 0;}
# 1798
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1800
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1804
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1807
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1809
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp322=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64B;_tmp64B.tag=0U,({struct _fat_ptr _tmp74E=({const char*_tmp329="integral size mismatch; ";_tag_fat(_tmp329,sizeof(char),25U);});_tmp64B.f1=_tmp74E;});_tmp64B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp323=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64A;_tmp64A.tag=2U,_tmp64A.f1=(void*)t1;_tmp64A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp324=({struct Cyc_Warn_String_Warn_Warg_struct _tmp649;_tmp649.tag=0U,({
struct _fat_ptr _tmp74F=({const char*_tmp328=" -> ";_tag_fat(_tmp328,sizeof(char),5U);});_tmp649.f1=_tmp74F;});_tmp649;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp325=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp648;_tmp648.tag=2U,_tmp648.f1=(void*)t2;_tmp648;});struct Cyc_Warn_String_Warn_Warg_struct _tmp326=({struct Cyc_Warn_String_Warn_Warg_struct _tmp647;_tmp647.tag=0U,({struct _fat_ptr _tmp750=({const char*_tmp327=" conversion supplied";_tag_fat(_tmp327,sizeof(char),21U);});_tmp647.f1=_tmp750;});_tmp647;});void*_tmp321[5U];_tmp321[0]=& _tmp322,_tmp321[1]=& _tmp323,_tmp321[2]=& _tmp324,_tmp321[3]=& _tmp325,_tmp321[4]=& _tmp326;({unsigned _tmp751=e->loc;Cyc_Warn_warn2(_tmp751,_tag_fat(_tmp321,sizeof(void*),5U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1817
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Flags_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp32B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp651;_tmp651.tag=0U,({struct _fat_ptr _tmp752=({const char*_tmp333="implicit alias coercion for ";_tag_fat(_tmp333,sizeof(char),29U);});_tmp651.f1=_tmp752;});_tmp651;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp32C=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp650;_tmp650.tag=4U,_tmp650.f1=e;_tmp650;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64F;_tmp64F.tag=0U,({struct _fat_ptr _tmp753=({const char*_tmp332=":";_tag_fat(_tmp332,sizeof(char),2U);});_tmp64F.f1=_tmp753;});_tmp64F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp32E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64E;_tmp64E.tag=2U,_tmp64E.f1=(void*)t1;_tmp64E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64D;_tmp64D.tag=0U,({struct _fat_ptr _tmp754=({const char*_tmp331=" to ";_tag_fat(_tmp331,sizeof(char),5U);});_tmp64D.f1=_tmp754;});_tmp64D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp330=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64C;_tmp64C.tag=2U,_tmp64C.f1=(void*)t2;_tmp64C;});void*_tmp32A[6U];_tmp32A[0]=& _tmp32B,_tmp32A[1]=& _tmp32C,_tmp32A[2]=& _tmp32D,_tmp32A[3]=& _tmp32E,_tmp32A[4]=& _tmp32F,_tmp32A[5]=& _tmp330;({unsigned _tmp755=e->loc;Cyc_Warn_warn2(_tmp755,_tag_fat(_tmp32A,sizeof(void*),6U));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1824
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1828
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1832
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp335=({struct Cyc_Warn_String_Warn_Warg_struct _tmp655;_tmp655.tag=0U,({struct _fat_ptr _tmp756=({const char*_tmp33A="implicit cast from ";_tag_fat(_tmp33A,sizeof(char),20U);});_tmp655.f1=_tmp756;});_tmp655;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp336=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp654;_tmp654.tag=2U,_tmp654.f1=(void*)t1;_tmp654;});struct Cyc_Warn_String_Warn_Warg_struct _tmp337=({struct Cyc_Warn_String_Warn_Warg_struct _tmp653;_tmp653.tag=0U,({struct _fat_ptr _tmp757=({const char*_tmp339=" to ";_tag_fat(_tmp339,sizeof(char),5U);});_tmp653.f1=_tmp757;});_tmp653;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp338=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp652;_tmp652.tag=2U,_tmp652.f1=(void*)t2;_tmp652;});void*_tmp334[4U];_tmp334[0]=& _tmp335,_tmp334[1]=& _tmp336,_tmp334[2]=& _tmp337,_tmp334[3]=& _tmp338;({unsigned _tmp758=e->loc;Cyc_Warn_warn2(_tmp758,_tag_fat(_tmp334,sizeof(void*),4U));});});
return 1;}
# 1838
return 0;}
# 1844
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1847
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1857
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1859
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1862
struct _tuple24 _tmp33B=*env;struct _tuple24 _stmttmp52=_tmp33B;struct _tuple24 _tmp33C=_stmttmp52;int _tmp33F;struct _RegionHandle*_tmp33E;struct Cyc_List_List*_tmp33D;_LL1: _tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;_tmp33F=_tmp33C.f3;_LL2: {struct Cyc_List_List*inst=_tmp33D;struct _RegionHandle*r=_tmp33E;int flatten=_tmp33F;
void*_tmp340=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp340;
struct Cyc_List_List*_tmp341=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp341;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp343=_region_malloc(r,sizeof(*_tmp343));({struct _tuple12*_tmp759=({struct _tuple12*_tmp342=_region_malloc(r,sizeof(*_tmp342));_tmp342->f1=x->tq,_tmp342->f2=t;_tmp342;});_tmp343->hd=_tmp759;}),_tmp343->tl=0;_tmp343;});
return ts;}}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1869
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 1871
struct _tuple25 _tmp344=*env;struct _tuple25 _stmttmp53=_tmp344;struct _tuple25 _tmp345=_stmttmp53;int _tmp347;struct _RegionHandle*_tmp346;_LL1: _tmp346=_tmp345.f1;_tmp347=_tmp345.f2;_LL2: {struct _RegionHandle*r=_tmp346;int flatten=_tmp347;
struct _tuple12 _tmp348=*x;struct _tuple12 _stmttmp54=_tmp348;struct _tuple12 _tmp349=_stmttmp54;void*_tmp34B;struct Cyc_Absyn_Tqual _tmp34A;_LL4: _tmp34A=_tmp349.f1;_tmp34B=_tmp349.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp34A;void*t=_tmp34B;
struct Cyc_List_List*_tmp34C=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp34C;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp34E=_region_malloc(r,sizeof(*_tmp34E));({struct _tuple12*_tmp75A=({struct _tuple12*_tmp34D=_region_malloc(r,sizeof(*_tmp34D));_tmp34D->f1=tq,_tmp34D->f2=t;_tmp34D;});_tmp34E->hd=_tmp75A;}),_tmp34E->tl=0;_tmp34E;});
return ts;}}}
# 1878
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1880
if(flatten){
t1=Cyc_Absyn_compress(t1);{
void*_tmp34F=t1;struct Cyc_List_List*_tmp350;struct Cyc_List_List*_tmp351;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_Aggrdecl*_tmp352;switch(*((int*)_tmp34F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp352=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f1)->f1).KnownAggr).val;_tmp353=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp352;struct Cyc_List_List*ts=_tmp353;
# 1898
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 1900
return({struct Cyc_List_List*_tmp35E=_region_malloc(r,sizeof(*_tmp35E));({struct _tuple12*_tmp75C=({struct _tuple12*_tmp35D=_region_malloc(r,sizeof(*_tmp35D));({struct Cyc_Absyn_Tqual _tmp75B=Cyc_Absyn_empty_tqual(0U);_tmp35D->f1=_tmp75B;}),_tmp35D->f2=t1;_tmp35D;});_tmp35E->hd=_tmp75C;}),_tmp35E->tl=0;_tmp35E;});{
struct Cyc_List_List*_tmp35F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp35F;
struct _tuple24 env=({struct _tuple24 _tmp656;_tmp656.f1=inst,_tmp656.f2=r,_tmp656.f3=flatten;_tmp656;});
struct Cyc_List_List*_tmp360=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp55=_tmp360;struct Cyc_List_List*_tmp361=_stmttmp55;struct Cyc_List_List*_tmp363;struct Cyc_Absyn_Aggrfield*_tmp362;if(_tmp361 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp362=(struct Cyc_Absyn_Aggrfield*)_tmp361->hd;_tmp363=_tmp361->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp362;struct Cyc_List_List*tl=_tmp363;
# 1906
struct Cyc_List_List*_tmp364=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp364;
env.f3=0;{
struct Cyc_List_List*_tmp365=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp365;
struct Cyc_List_List*_tmp366=({struct Cyc_List_List*_tmp367=_region_malloc(r,sizeof(*_tmp367));_tmp367->hd=hd2,_tmp367->tl=tl2;_tmp367;});struct Cyc_List_List*tts=_tmp366;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp351=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp34F)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp351;
# 1885
struct _tuple25 _tmp354=({struct _tuple25 _tmp657;_tmp657.f1=r,_tmp657.f2=flatten;_tmp657;});struct _tuple25 env=_tmp354;
# 1887
struct Cyc_List_List*_tmp355=tqs;struct Cyc_List_List*_tmp357;struct _tuple12*_tmp356;if(_tmp355 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp356=(struct _tuple12*)_tmp355->hd;_tmp357=_tmp355->tl;_LLF: {struct _tuple12*hd=_tmp356;struct Cyc_List_List*tl=_tmp357;
# 1890
struct Cyc_List_List*_tmp358=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp358;
env.f2=0;{
struct Cyc_List_List*_tmp359=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp359;
struct Cyc_List_List*_tmp35A=({struct Cyc_List_List*_tmp35C=_region_malloc(r,sizeof(*_tmp35C));_tmp35C->hd=hd2,_tmp35C->tl=tl2;_tmp35C;});struct Cyc_List_List*temp=_tmp35A;
return({struct _RegionHandle*_tmp75D=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp75D,({struct Cyc_List_List*_tmp35B=_region_malloc(r,sizeof(*_tmp35B));_tmp35B->hd=hd2,_tmp35B->tl=tl2;_tmp35B;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp34F)->f1 == Cyc_Absyn_StructA){_LL7: _tmp350=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp34F)->f2;_LL8: {struct Cyc_List_List*fs=_tmp350;
# 1913
struct _tuple24 env=({struct _tuple24 _tmp658;_tmp658.f1=0,_tmp658.f2=r,_tmp658.f3=flatten;_tmp658;});
struct Cyc_List_List*_tmp368=fs;struct Cyc_List_List*_tmp36A;struct Cyc_Absyn_Aggrfield*_tmp369;if(_tmp368 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp369=(struct Cyc_Absyn_Aggrfield*)_tmp368->hd;_tmp36A=_tmp368->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp369;struct Cyc_List_List*tl=_tmp36A;
# 1917
struct Cyc_List_List*_tmp36B=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp36B;
env.f3=0;{
struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp36C;
struct Cyc_List_List*_tmp36D=({struct Cyc_List_List*_tmp36E=_region_malloc(r,sizeof(*_tmp36E));_tmp36E->hd=hd2,_tmp36E->tl=tl2;_tmp36E;});struct Cyc_List_List*tts=_tmp36D;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 1923
 goto _LL0;}_LL0:;}}
# 1926
return({struct Cyc_List_List*_tmp370=_region_malloc(r,sizeof(*_tmp370));({struct _tuple12*_tmp75F=({struct _tuple12*_tmp36F=_region_malloc(r,sizeof(*_tmp36F));({struct Cyc_Absyn_Tqual _tmp75E=Cyc_Absyn_empty_tqual(0U);_tmp36F->f1=_tmp75E;}),_tmp36F->f2=t1;_tmp36F;});_tmp370->hd=_tmp75F;}),_tmp370->tl=0;_tmp370;});}
# 1929
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp371=({struct _tuple15 _tmp659;({void*_tmp761=Cyc_Absyn_compress(t1);_tmp659.f1=_tmp761;}),({void*_tmp760=Cyc_Absyn_compress(t2);_tmp659.f2=_tmp760;});_tmp659;});struct _tuple15 _stmttmp56=_tmp371;struct _tuple15 _tmp372=_stmttmp56;enum Cyc_Absyn_Size_of _tmp374;enum Cyc_Absyn_Size_of _tmp373;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372.f2)->f1)->tag == 1U){_LL1: _tmp373=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372.f1)->f1)->f2;_tmp374=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp372.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp373;enum Cyc_Absyn_Size_of b2=_tmp374;
# 1932
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1940
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1942
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp375=({struct _tuple15 _tmp65A;_tmp65A.f1=t1,_tmp65A.f2=t2;_tmp65A;});struct _tuple15 _stmttmp57=_tmp375;struct _tuple15 _tmp376=_stmttmp57;struct Cyc_Absyn_FnInfo _tmp378;struct Cyc_Absyn_FnInfo _tmp377;struct Cyc_List_List*_tmp37D;struct Cyc_Absyn_Datatypedecl*_tmp37C;struct Cyc_List_List*_tmp37B;struct Cyc_Absyn_Datatypefield*_tmp37A;struct Cyc_Absyn_Datatypedecl*_tmp379;void*_tmp389;void*_tmp388;void*_tmp387;void*_tmp386;struct Cyc_Absyn_Tqual _tmp385;void*_tmp384;void*_tmp383;void*_tmp382;void*_tmp381;void*_tmp380;struct Cyc_Absyn_Tqual _tmp37F;void*_tmp37E;switch(*((int*)_tmp376.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->tag == 3U){_LL1: _tmp37E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).elt_type;_tmp37F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).elt_tq;_tmp380=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).ptr_atts).rgn;_tmp381=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).ptr_atts).nullable;_tmp382=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).ptr_atts).bounds;_tmp383=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f1)->f1).ptr_atts).zero_term;_tmp384=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).elt_type;_tmp385=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).elt_tq;_tmp386=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).ptr_atts).rgn;_tmp387=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).ptr_atts).nullable;_tmp388=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).ptr_atts).bounds;_tmp389=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp376.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp37E;struct Cyc_Absyn_Tqual q_a=_tmp37F;void*rt_a=_tmp380;void*null_a=_tmp381;void*b_a=_tmp382;void*zt_a=_tmp383;void*t_b=_tmp384;struct Cyc_Absyn_Tqual q_b=_tmp385;void*rt_b=_tmp386;void*null_b=_tmp387;void*b_b=_tmp388;void*zt_b=_tmp389;
# 1954
if(q_a.real_const && !q_b.real_const)
return 0;
# 1957
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 1961
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 1965
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 1969
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp38A=({void*_tmp762=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp762,b_a);});struct Cyc_Absyn_Exp*e1=_tmp38A;
struct Cyc_Absyn_Exp*_tmp38B=({void*_tmp763=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp763,b_b);});struct Cyc_Absyn_Exp*e2=_tmp38B;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 1978
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 1983
int _tmp38C=({void*_tmp764=b_b;Cyc_Unify_unify(_tmp764,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);int deep_subtype=_tmp38C;
# 1986
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp768=po;struct Cyc_List_List*_tmp767=({struct Cyc_List_List*_tmp38E=_cycalloc(sizeof(*_tmp38E));({struct _tuple15*_tmp765=({struct _tuple15*_tmp38D=_cycalloc(sizeof(*_tmp38D));_tmp38D->f1=t1,_tmp38D->f2=t2;_tmp38D;});_tmp38E->hd=_tmp765;}),_tmp38E->tl=assume;_tmp38E;});void*_tmp766=t_a;Cyc_Tcutil_ptrsubtype(_tmp768,_tmp767,_tmp766,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp379=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp37A=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp37B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f1)->f2;_tmp37C=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f2)->f1)->f1).KnownDatatype).val;_tmp37D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp376.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp379;struct Cyc_Absyn_Datatypefield*df=_tmp37A;struct Cyc_List_List*ts1=_tmp37B;struct Cyc_Absyn_Datatypedecl*dd2=_tmp37C;struct Cyc_List_List*ts2=_tmp37D;
# 1993
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 1995
if(({int _tmp769=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp769 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp376.f2)->tag == 5U){_LL5: _tmp377=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp376.f1)->f1;_tmp378=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp376.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp377;struct Cyc_Absyn_FnInfo f2=_tmp378;
# 2001
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp38F=f1.tvars;struct Cyc_List_List*tvs1=_tmp38F;
struct Cyc_List_List*_tmp390=f2.tvars;struct Cyc_List_List*tvs2=_tmp390;
if(({int _tmp76A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp76A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp392=_cycalloc(sizeof(*_tmp392));({struct _tuple19*_tmp76C=({struct _tuple19*_tmp391=_cycalloc(sizeof(*_tmp391));_tmp391->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp76B=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp391->f2=_tmp76B;});_tmp391;});_tmp392->hd=_tmp76C;}),_tmp392->tl=inst;_tmp392;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2012
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp76F=po;struct Cyc_List_List*_tmp76E=assume;void*_tmp76D=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->tag=5U,_tmp393->f1=f1;_tmp393;});Cyc_Tcutil_subtype(_tmp76F,_tmp76E,_tmp76D,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->tag=5U,_tmp394->f1=f2;_tmp394;}));});}}}
# 2019
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp395=f1.args;struct Cyc_List_List*args1=_tmp395;
struct Cyc_List_List*_tmp396=f2.args;struct Cyc_List_List*args2=_tmp396;
# 2024
if(({int _tmp770=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp770 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2026
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp397=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp58=_tmp397;struct _tuple9 _tmp398=_stmttmp58;void*_tmp39A;struct Cyc_Absyn_Tqual _tmp399;_LLA: _tmp399=_tmp398.f2;_tmp39A=_tmp398.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp399;void*t1=_tmp39A;
struct _tuple9 _tmp39B=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp59=_tmp39B;struct _tuple9 _tmp39C=_stmttmp59;void*_tmp39E;struct Cyc_Absyn_Tqual _tmp39D;_LLD: _tmp39D=_tmp39C.f2;_tmp39E=_tmp39C.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp39D;void*t2=_tmp39E;
# 2030
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2034
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp39F=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp39F;
struct Cyc_Absyn_VarargInfo _tmp3A0=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp3A0;
# 2039
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2044
if(!({void*_tmp771=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp771,(void*)_check_null(f2.effect));}))return 0;
# 2046
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2048
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2050
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2052
return 0;
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2055
return 0;
# 2057
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2068
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2071
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp3A1=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp5A=_tmp3A1;struct _tuple12*_tmp3A2=_stmttmp5A;void*_tmp3A4;struct Cyc_Absyn_Tqual _tmp3A3;_LL1: _tmp3A3=_tmp3A2->f1;_tmp3A4=_tmp3A2->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp3A3;void*t1a=_tmp3A4;
struct _tuple12*_tmp3A5=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp5B=_tmp3A5;struct _tuple12*_tmp3A6=_stmttmp5B;void*_tmp3A8;struct Cyc_Absyn_Tqual _tmp3A7;_LL4: _tmp3A7=_tmp3A6->f1;_tmp3A8=_tmp3A6->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp3A7;void*t2a=_tmp3A8;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2085
return 1;}
# 2090
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
{void*_tmp3A9=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A9)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A9)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2098
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2100
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2104
void*_tmp3AA=t1;void*_tmp3AB;struct Cyc_Absyn_Enumdecl*_tmp3AC;void*_tmp3B0;struct Cyc_Absyn_Exp*_tmp3AF;struct Cyc_Absyn_Tqual _tmp3AE;void*_tmp3AD;void*_tmp3B6;void*_tmp3B5;void*_tmp3B4;void*_tmp3B3;struct Cyc_Absyn_Tqual _tmp3B2;void*_tmp3B1;switch(*((int*)_tmp3AA)){case 3U: _LLA: _tmp3B1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->f1).elt_type;_tmp3B2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->f1).elt_tq;_tmp3B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->f1).ptr_atts).rgn;_tmp3B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->f1).ptr_atts).nullable;_tmp3B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->f1).ptr_atts).bounds;_tmp3B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AA)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp3B1;struct Cyc_Absyn_Tqual q_a=_tmp3B2;void*rt_a=_tmp3B3;void*null_a=_tmp3B4;void*b_a=_tmp3B5;void*zt_a=_tmp3B6;
# 2113
{void*_tmp3B7=t2;void*_tmp3BD;void*_tmp3BC;void*_tmp3BB;void*_tmp3BA;struct Cyc_Absyn_Tqual _tmp3B9;void*_tmp3B8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B7)->tag == 3U){_LL19: _tmp3B8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B7)->f1).elt_type;_tmp3B9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B7)->f1).elt_tq;_tmp3BA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B7)->f1).ptr_atts).rgn;_tmp3BB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B7)->f1).ptr_atts).nullable;_tmp3BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B7)->f1).ptr_atts).bounds;_tmp3BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B7)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3B8;struct Cyc_Absyn_Tqual q_b=_tmp3B9;void*rt_b=_tmp3BA;void*null_b=_tmp3BB;void*b_b=_tmp3BC;void*zt_b=_tmp3BD;
# 2115
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3BE=({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({struct _tuple15*_tmp772=({struct _tuple15*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->f1=t1,_tmp3C5->f2=t2;_tmp3C5;});_tmp3C6->hd=_tmp772;}),_tmp3C6->tl=0;_tmp3C6;});struct Cyc_List_List*assump=_tmp3BE;
int _tmp3BF=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3BF;
# 2128 "tcutil.cyc"
int _tmp3C0=
({void*_tmp773=b_b;Cyc_Unify_unify(_tmp773,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2128
int deep_castable=_tmp3C0;
# 2131
int _tmp3C1=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2131
int ptrsub=_tmp3C1;
# 2134
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2136
int _tmp3C2=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2136
int bitcase=_tmp3C2;
# 2139
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3C3=({void*_tmp774=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp774,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3C3;
struct Cyc_Absyn_Exp*_tmp3C4=({void*_tmp775=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp775,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3C4;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2148
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2153
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2159
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp3AD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AA)->f1).elt_type;_tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AA)->f1).tq;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AA)->f1).num_elts;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AA)->f1).zero_term;_LLD: {void*t1a=_tmp3AD;struct Cyc_Absyn_Tqual tq1a=_tmp3AE;struct Cyc_Absyn_Exp*e1=_tmp3AF;void*zt1=_tmp3B0;
# 2161
{void*_tmp3C7=t2;void*_tmp3CB;struct Cyc_Absyn_Exp*_tmp3CA;struct Cyc_Absyn_Tqual _tmp3C9;void*_tmp3C8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C7)->tag == 4U){_LL1E: _tmp3C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C7)->f1).elt_type;_tmp3C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C7)->f1).tq;_tmp3CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C7)->f1).num_elts;_tmp3CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C7)->f1).zero_term;_LL1F: {void*t2a=_tmp3C8;struct Cyc_Absyn_Tqual tq2a=_tmp3C9;struct Cyc_Absyn_Exp*e2=_tmp3CA;void*zt2=_tmp3CB;
# 2163
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2168
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2170
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2172
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f1)){case 15U: _LLE: _tmp3AC=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp3AC;
# 2176
{void*_tmp3CC=t2;struct Cyc_Absyn_Enumdecl*_tmp3CD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CC)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CC)->f1)->tag == 15U){_LL23: _tmp3CD=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CC)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3CD;
# 2178
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp776=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp776 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2184
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2187
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f2 != 0){_LL14: _tmp3AB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f2)->hd;_LL15: {void*r1=_tmp3AB;
# 2190
{void*_tmp3CE=t2;void*_tmp3CF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f2 != 0){_LL28: _tmp3CF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f2)->hd;_LL29: {void*r2=_tmp3CF;
# 2192
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2196
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2202
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3D0=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3D0;
({void*_tmp777=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));_tmp3D1->tag=14U,_tmp3D1->f1=t,_tmp3D1->f2=inner,_tmp3D1->f3=0,_tmp3D1->f4=c;_tmp3D1;});e->r=_tmp777;});
e->topt=t;}}
# 2211
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2216
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
return({struct Cyc_Absyn_Tvar*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _fat_ptr*_tmp77A=({struct _fat_ptr*_tmp3D5=_cycalloc(sizeof(*_tmp3D5));({struct _fat_ptr _tmp779=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D4=({struct Cyc_Int_pa_PrintArg_struct _tmp65B;_tmp65B.tag=1U,_tmp65B.f1=(unsigned long)Cyc_Tcutil_tvar_counter ++;_tmp65B;});void*_tmp3D2[1U];_tmp3D2[0]=& _tmp3D4;({struct _fat_ptr _tmp778=({const char*_tmp3D3="#%d";_tag_fat(_tmp3D3,sizeof(char),4U);});Cyc_aprintf(_tmp778,_tag_fat(_tmp3D2,sizeof(void*),1U));});});*_tmp3D5=_tmp779;});_tmp3D5;});_tmp3D6->name=_tmp77A;}),_tmp3D6->identity=- 1,_tmp3D6->kind=k;_tmp3D6;});}
# 2220
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
return(int)*((const char*)_check_fat_subscript((t->name)[0],sizeof(char),0U))== (int)'#';}
# 2225
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2232
struct Cyc_List_List*_tmp3D7=0;struct Cyc_List_List*fn_type_atts=_tmp3D7;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Atts_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3D8=_cycalloc(sizeof(*_tmp3D8));_tmp3D8->hd=(void*)atts->hd,_tmp3D8->tl=fn_type_atts;_tmp3D8;});}}{
struct Cyc_Absyn_FnInfo _tmp3D9=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3D9;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));_tmp3DA->tag=5U,_tmp3DA->f1=type_info;_tmp3DA;});}}
# 2240
return(void*)_check_null(fd->cached_type);}
# 2246
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2248
union Cyc_Relations_RelnOp _tmp3DB=*rop;union Cyc_Relations_RelnOp _stmttmp5C=_tmp3DB;union Cyc_Relations_RelnOp _tmp3DC=_stmttmp5C;struct Cyc_Absyn_Vardecl*_tmp3DD;struct Cyc_Absyn_Vardecl*_tmp3DE;switch((_tmp3DC.RNumelts).tag){case 2U: _LL1: _tmp3DE=(_tmp3DC.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3DE;
# 2250
struct _tuple1 _tmp3DF=*vd->name;struct _tuple1 _stmttmp5D=_tmp3DF;struct _tuple1 _tmp3E0=_stmttmp5D;struct _fat_ptr*_tmp3E2;union Cyc_Absyn_Nmspace _tmp3E1;_LL8: _tmp3E1=_tmp3E0.f1;_tmp3E2=_tmp3E0.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3E1;struct _fat_ptr*var=_tmp3E2;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp77B=(struct _fat_ptr)*var;Cyc_strcmp(_tmp77B,({const char*_tmp3E3="return_value";_tag_fat(_tmp3E3,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp77C=Cyc_Relations_RReturn();*rop=_tmp77C;});
goto _LL0;}{
# 2256
unsigned c=0U;
{struct Cyc_List_List*_tmp3E4=args;struct Cyc_List_List*a=_tmp3E4;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3E5=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5E=_tmp3E5;struct _tuple9*_tmp3E6=_stmttmp5E;struct _fat_ptr*_tmp3E7;_LLB: _tmp3E7=_tmp3E6->f1;_LLC: {struct _fat_ptr*vopt=_tmp3E7;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp77D=Cyc_Relations_RParam(c);*rop=_tmp77D;});
break;}}}}
# 2264
goto _LL0;}}}case 3U: _LL3: _tmp3DD=(_tmp3DC.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3DD;
# 2266
struct _tuple1 _tmp3E8=*vd->name;struct _tuple1 _stmttmp5F=_tmp3E8;struct _tuple1 _tmp3E9=_stmttmp5F;struct _fat_ptr*_tmp3EB;union Cyc_Absyn_Nmspace _tmp3EA;_LLE: _tmp3EA=_tmp3E9.f1;_tmp3EB=_tmp3E9.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3EA;struct _fat_ptr*var=_tmp3EB;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3EC=args;struct Cyc_List_List*a=_tmp3EC;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3ED=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp60=_tmp3ED;struct _tuple9*_tmp3EE=_stmttmp60;struct _fat_ptr*_tmp3EF;_LL11: _tmp3EF=_tmp3EE->f1;_LL12: {struct _fat_ptr*vopt=_tmp3EF;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp77E=Cyc_Relations_RParamNumelts(c);*rop=_tmp77E;});
break;}}}}
# 2276
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2281
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2283
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2287
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2289
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3F0=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3F0;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2296
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3F1=pr;void*_tmp3F3;struct Cyc_Absyn_Tqual _tmp3F2;_LL1: _tmp3F2=_tmp3F1->f1;_tmp3F3=_tmp3F1->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3F2;void*t2=_tmp3F3;
return t2 == t?pr:({struct _tuple12*_tmp3F4=_cycalloc(sizeof(*_tmp3F4));_tmp3F4->f1=tq,_tmp3F4->f2=t;_tmp3F4;});}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2300
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2302
return({struct _tuple27*_tmp3F6=_region_malloc(rgn,sizeof(*_tmp3F6));({struct _tuple26*_tmp77F=({struct _tuple26*_tmp3F5=_region_malloc(rgn,sizeof(*_tmp3F5));_tmp3F5->f1=(*y).f1,_tmp3F5->f2=(*y).f2;_tmp3F5;});_tmp3F6->f1=_tmp77F;}),_tmp3F6->f2=(*y).f3;_tmp3F6;});}
# 2304
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2306
struct _tuple9 _tmp3F7=*orig_arg;struct _tuple9 _stmttmp61=_tmp3F7;struct _tuple9 _tmp3F8=_stmttmp61;void*_tmp3FB;struct Cyc_Absyn_Tqual _tmp3FA;struct _fat_ptr*_tmp3F9;_LL1: _tmp3F9=_tmp3F8.f1;_tmp3FA=_tmp3F8.f2;_tmp3FB=_tmp3F8.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp3F9;struct Cyc_Absyn_Tqual tq_orig=_tmp3FA;void*t_orig=_tmp3FB;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp3FC=_cycalloc(sizeof(*_tmp3FC));_tmp3FC->f1=vopt_orig,_tmp3FC->f2=tq_orig,_tmp3FC->f3=t;_tmp3FC;});}}
# 2310
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2314
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2316
return({struct Cyc_Absyn_Exp*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->topt=old->topt,_tmp3FD->r=r,_tmp3FD->loc=old->loc,_tmp3FD->annot=old->annot;_tmp3FD;});}
# 2321
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3FE=e->r;void*_stmttmp62=_tmp3FE;void*_tmp3FF=_stmttmp62;void*_tmp400;struct Cyc_List_List*_tmp402;void*_tmp401;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp404;void*_tmp405;enum Cyc_Absyn_Coercion _tmp409;int _tmp408;struct Cyc_Absyn_Exp*_tmp407;void*_tmp406;struct Cyc_Absyn_Exp*_tmp40B;struct Cyc_Absyn_Exp*_tmp40A;struct Cyc_Absyn_Exp*_tmp40D;struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_Absyn_Exp*_tmp411;struct Cyc_Absyn_Exp*_tmp410;struct Cyc_List_List*_tmp414;enum Cyc_Absyn_Primop _tmp413;switch(*((int*)_tmp3FF)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp413=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_tmp414=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3FF)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp413;struct Cyc_List_List*es=_tmp414;
# 2330
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp415=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp415;
struct Cyc_Absyn_Exp*_tmp416=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp416;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp781=e;Cyc_Tcutil_copye(_tmp781,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->tag=3U,_tmp418->f1=p,({struct Cyc_List_List*_tmp780=({struct Cyc_Absyn_Exp*_tmp417[1U];_tmp417[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp417,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp418->f2=_tmp780;});_tmp418;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp419=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp419;
struct Cyc_Absyn_Exp*_tmp41A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp41A;
struct Cyc_Absyn_Exp*_tmp41B=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41B;
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp41C;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp783=e;Cyc_Tcutil_copye(_tmp783,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->tag=3U,_tmp41E->f1=p,({struct Cyc_List_List*_tmp782=({struct Cyc_Absyn_Exp*_tmp41D[2U];_tmp41D[0]=new_e1,_tmp41D[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp41D,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp41E->f2=_tmp782;});_tmp41E;}));});}}
# 2342
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp420=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65C;_tmp65C.tag=0U,({struct _fat_ptr _tmp784=({const char*_tmp421="primop does not have 1 or 2 args!";_tag_fat(_tmp421,sizeof(char),34U);});_tmp65C.f1=_tmp784;});_tmp65C;});void*_tmp41F[1U];_tmp41F[0]=& _tmp420;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp41F,sizeof(void*),1U));});}case 6U: _LLD: _tmp410=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_tmp411=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3FF)->f2;_tmp412=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3FF)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp410;struct Cyc_Absyn_Exp*e2=_tmp411;struct Cyc_Absyn_Exp*e3=_tmp412;
# 2344
struct Cyc_Absyn_Exp*_tmp422=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp422;
struct Cyc_Absyn_Exp*_tmp423=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp423;
struct Cyc_Absyn_Exp*_tmp424=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp424;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp785=e;Cyc_Tcutil_copye(_tmp785,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp425=_cycalloc(sizeof(*_tmp425));_tmp425->tag=6U,_tmp425->f1=new_e1,_tmp425->f2=new_e2,_tmp425->f3=new_e3;_tmp425;}));});}case 7U: _LLF: _tmp40E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_tmp40F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3FF)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp40E;struct Cyc_Absyn_Exp*e2=_tmp40F;
# 2350
struct Cyc_Absyn_Exp*_tmp426=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp426;
struct Cyc_Absyn_Exp*_tmp427=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp427;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp786=e;Cyc_Tcutil_copye(_tmp786,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->tag=7U,_tmp428->f1=new_e1,_tmp428->f2=new_e2;_tmp428;}));});}case 8U: _LL11: _tmp40C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_tmp40D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3FF)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp40C;struct Cyc_Absyn_Exp*e2=_tmp40D;
# 2355
struct Cyc_Absyn_Exp*_tmp429=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp429;
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp42A;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp787=e;Cyc_Tcutil_copye(_tmp787,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->tag=8U,_tmp42B->f1=new_e1,_tmp42B->f2=new_e2;_tmp42B;}));});}case 9U: _LL13: _tmp40A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_tmp40B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3FF)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp40A;struct Cyc_Absyn_Exp*e2=_tmp40B;
# 2360
struct Cyc_Absyn_Exp*_tmp42C=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42C;
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp42D;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp788=e;Cyc_Tcutil_copye(_tmp788,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->tag=9U,_tmp42E->f1=new_e1,_tmp42E->f2=new_e2;_tmp42E;}));});}case 14U: _LL15: _tmp406=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_tmp407=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FF)->f2;_tmp408=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FF)->f3;_tmp409=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FF)->f4;_LL16: {void*t=_tmp406;struct Cyc_Absyn_Exp*e1=_tmp407;int b=_tmp408;enum Cyc_Absyn_Coercion c=_tmp409;
# 2365
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42F;
void*_tmp430=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp430;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp789=e;Cyc_Tcutil_copye(_tmp789,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->tag=14U,_tmp431->f1=new_typ,_tmp431->f2=new_e1,_tmp431->f3=b,_tmp431->f4=c;_tmp431;}));});}case 17U: _LL17: _tmp405=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_LL18: {void*t=_tmp405;
# 2370
void*_tmp432=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp432;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp78A=e;Cyc_Tcutil_copye(_tmp78A,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->tag=17U,_tmp433->f1=new_typ;_tmp433;}));});}case 18U: _LL19: _tmp404=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp404;
# 2373
struct Cyc_Absyn_Exp*_tmp434=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp434;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp78B=e;Cyc_Tcutil_copye(_tmp78B,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->tag=18U,_tmp435->f1=new_e1;_tmp435;}));});}case 41U: _LL1B: _tmp403=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp403;
# 2376
struct Cyc_Absyn_Exp*_tmp436=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp436;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp78C=e;Cyc_Tcutil_copye(_tmp78C,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=41U,_tmp437->f1=new_e1;_tmp437;}));});}case 19U: _LL1D: _tmp401=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_tmp402=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3FF)->f2;_LL1E: {void*t=_tmp401;struct Cyc_List_List*f=_tmp402;
# 2379
void*_tmp438=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp438;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp78D=e;Cyc_Tcutil_copye(_tmp78D,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439->tag=19U,_tmp439->f1=new_typ,_tmp439->f2=f;_tmp439;}));});}case 39U: _LL1F: _tmp400=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3FF)->f1;_LL20: {void*t=_tmp400;
# 2382
void*_tmp43A=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp43A;
if(new_typ == t)return e;{
# 2385
void*_tmp43B=Cyc_Absyn_compress(new_typ);void*_stmttmp63=_tmp43B;void*_tmp43C=_stmttmp63;struct Cyc_Absyn_Exp*_tmp43D;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43C)->tag == 9U){_LL24: _tmp43D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43C)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp43D;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp78E=e;Cyc_Tcutil_copye(_tmp78E,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp43E=_cycalloc(sizeof(*_tmp43E));_tmp43E->tag=39U,_tmp43E->f1=new_typ;_tmp43E;}));});}_LL23:;}}default: _LL21: _LL22:
# 2389
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp440=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65D;_tmp65D.tag=0U,({struct _fat_ptr _tmp78F=({const char*_tmp441="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp441,sizeof(char),46U);});_tmp65D.f1=_tmp78F;});_tmp65D;});void*_tmp43F[1U];_tmp43F[0]=& _tmp440;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp43F,sizeof(void*),1U));});}_LL0:;}
# 2393
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2396
return e == 0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2399
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2402
void*_tmp442=f->type;void*t=_tmp442;
struct Cyc_Absyn_Exp*_tmp443=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp443;
void*_tmp444=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp444;
struct Cyc_Absyn_Exp*_tmp445=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp445;
if(t == new_typ && req == new_req)
return f;{
struct Cyc_Absyn_Aggrfield*_tmp446=({struct Cyc_Absyn_Aggrfield*_tmp447=_cycalloc(sizeof(*_tmp447));*_tmp447=*f;_tmp447;});struct Cyc_Absyn_Aggrfield*ans=_tmp446;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2414
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2417
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp448=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp448;
struct Cyc_List_List*_tmp449=fs->tl;struct Cyc_List_List*t=_tmp449;
struct Cyc_Absyn_Aggrfield*_tmp44A=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp44A;
struct Cyc_List_List*_tmp44B=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_typ=_tmp44B;
return new_f == f && new_typ == t?fs:({struct Cyc_List_List*_tmp44C=_cycalloc(sizeof(*_tmp44C));_tmp44C->hd=new_f,_tmp44C->tl=new_typ;_tmp44C;});}}
# 2425
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2428
struct _tuple0 _tmp44D=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp64=_tmp44D;struct _tuple0 _tmp44E=_stmttmp64;struct Cyc_List_List*_tmp450;struct Cyc_List_List*_tmp44F;_LL1: _tmp44F=_tmp44E.f1;_tmp450=_tmp44E.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp44F;struct Cyc_List_List*rpo1b=_tmp450;
struct Cyc_List_List*_tmp451=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp451;
struct Cyc_List_List*_tmp452=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp452;
return rpo2a == rpo1a && rpo2b == rpo1b?rgn_po:((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}
# 2436
static int Cyc_Tcutil_skip_fntypes=0;
# 2438
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2441
void*_tmp453=Cyc_Absyn_compress(t);void*_stmttmp65=_tmp453;void*_tmp454=_stmttmp65;struct Cyc_Absyn_Exp*_tmp455;struct Cyc_Absyn_Exp*_tmp456;struct Cyc_List_List*_tmp458;void*_tmp457;void*_tmp459;struct Cyc_List_List*_tmp45B;enum Cyc_Absyn_AggrKind _tmp45A;struct Cyc_List_List*_tmp45C;struct Cyc_Absyn_Vardecl*_tmp468;struct Cyc_Absyn_Exp*_tmp467;struct Cyc_Absyn_Exp*_tmp466;struct Cyc_List_List*_tmp465;struct Cyc_List_List*_tmp464;struct Cyc_Absyn_VarargInfo*_tmp463;int _tmp462;struct Cyc_List_List*_tmp461;void*_tmp460;struct Cyc_Absyn_Tqual _tmp45F;void*_tmp45E;struct Cyc_List_List*_tmp45D;void*_tmp46E;void*_tmp46D;void*_tmp46C;void*_tmp46B;struct Cyc_Absyn_Tqual _tmp46A;void*_tmp469;unsigned _tmp473;void*_tmp472;struct Cyc_Absyn_Exp*_tmp471;struct Cyc_Absyn_Tqual _tmp470;void*_tmp46F;void*_tmp477;struct Cyc_Absyn_Typedefdecl*_tmp476;struct Cyc_List_List*_tmp475;struct _tuple1*_tmp474;struct Cyc_Absyn_Tvar*_tmp478;switch(*((int*)_tmp454)){case 2U: _LL1: _tmp478=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp454)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp478;
# 2444
struct _handler_cons _tmp479;_push_handler(& _tmp479);{int _tmp47B=0;if(setjmp(_tmp479.handler))_tmp47B=1;if(!_tmp47B){{void*_tmp47C=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp47C;};_pop_handler();}else{void*_tmp47A=(void*)Cyc_Core_get_exn_thrown();void*_tmp47D=_tmp47A;void*_tmp47E;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp47D)->tag == Cyc_Core_Not_found){_LL1A: _LL1B:
 return t;}else{_LL1C: _tmp47E=_tmp47D;_LL1D: {void*exn=_tmp47E;(int)_rethrow(exn);}}_LL19:;}}}case 8U: _LL3: _tmp474=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp454)->f1;_tmp475=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp454)->f2;_tmp476=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp454)->f3;_tmp477=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp454)->f4;_LL4: {struct _tuple1*n=_tmp474;struct Cyc_List_List*ts=_tmp475;struct Cyc_Absyn_Typedefdecl*td=_tmp476;void*topt=_tmp477;
# 2447
struct Cyc_List_List*_tmp47F=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp47F;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->tag=8U,_tmp480->f1=n,_tmp480->f2=new_ts,_tmp480->f3=td,_tmp480->f4=topt;_tmp480;});}case 4U: _LL5: _tmp46F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp454)->f1).elt_type;_tmp470=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp454)->f1).tq;_tmp471=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp454)->f1).num_elts;_tmp472=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp454)->f1).zero_term;_tmp473=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp454)->f1).zt_loc;_LL6: {void*t1=_tmp46F;struct Cyc_Absyn_Tqual tq=_tmp470;struct Cyc_Absyn_Exp*e=_tmp471;void*zt=_tmp472;unsigned ztl=_tmp473;
# 2450
void*_tmp481=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp481;
struct Cyc_Absyn_Exp*_tmp482=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp482;
void*_tmp483=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp483;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp484=_cycalloc(sizeof(*_tmp484));
_tmp484->tag=4U,(_tmp484->f1).elt_type=new_t1,(_tmp484->f1).tq=tq,(_tmp484->f1).num_elts=new_e,(_tmp484->f1).zero_term=new_zt,(_tmp484->f1).zt_loc=ztl;_tmp484;});}case 3U: _LL7: _tmp469=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp454)->f1).elt_type;_tmp46A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp454)->f1).elt_tq;_tmp46B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp454)->f1).ptr_atts).rgn;_tmp46C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp454)->f1).ptr_atts).nullable;_tmp46D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp454)->f1).ptr_atts).bounds;_tmp46E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp454)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp469;struct Cyc_Absyn_Tqual tq=_tmp46A;void*r=_tmp46B;void*n=_tmp46C;void*b=_tmp46D;void*zt=_tmp46E;
# 2456
void*_tmp485=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp485;
void*_tmp486=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp486;
void*_tmp487=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp487;
void*_tmp488=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp488;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp65E;_tmp65E.elt_type=new_t1,_tmp65E.elt_tq=tq,(_tmp65E.ptr_atts).rgn=new_r,(_tmp65E.ptr_atts).nullable=n,(_tmp65E.ptr_atts).bounds=new_b,(_tmp65E.ptr_atts).zero_term=new_zt,(_tmp65E.ptr_atts).ptrloc=0;_tmp65E;}));}case 5U: _LL9: _tmp45D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).tvars;_tmp45E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).effect;_tmp45F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).ret_tqual;_tmp460=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).ret_type;_tmp461=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).args;_tmp462=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).c_varargs;_tmp463=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).cyc_varargs;_tmp464=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).rgn_po;_tmp465=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).attributes;_tmp466=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).requires_clause;_tmp467=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).ensures_clause;_tmp468=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp454)->f1).return_value;_LLA: {struct Cyc_List_List*vs=_tmp45D;void*eff=_tmp45E;struct Cyc_Absyn_Tqual rtq=_tmp45F;void*rtyp=_tmp460;struct Cyc_List_List*args=_tmp461;int c_varargs=_tmp462;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp463;struct Cyc_List_List*rgn_po=_tmp464;struct Cyc_List_List*atts=_tmp465;struct Cyc_Absyn_Exp*req=_tmp466;struct Cyc_Absyn_Exp*ens=_tmp467;struct Cyc_Absyn_Vardecl*ret_value=_tmp468;
# 2465
if(Cyc_Tcutil_skip_fntypes)return t;
# 2467
{struct Cyc_List_List*_tmp489=vs;struct Cyc_List_List*p=_tmp489;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp48B=_region_malloc(rgn,sizeof(*_tmp48B));({struct _tuple19*_tmp791=({struct _tuple19*_tmp48A=_region_malloc(rgn,sizeof(*_tmp48A));_tmp48A->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp790=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp48A->f2=_tmp790;});_tmp48A;});_tmp48B->hd=_tmp791;}),_tmp48B->tl=inst;_tmp48B;});}}{
struct _tuple0 _tmp48C=({struct _RegionHandle*_tmp793=rgn;struct _RegionHandle*_tmp792=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp793,_tmp792,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2469
struct _tuple0 _stmttmp66=_tmp48C;struct _tuple0 _tmp48D=_stmttmp66;struct Cyc_List_List*_tmp48F;struct Cyc_List_List*_tmp48E;_LL1F: _tmp48E=_tmp48D.f1;_tmp48F=_tmp48D.f2;_LL20: {struct Cyc_List_List*qs=_tmp48E;struct Cyc_List_List*ts=_tmp48F;
# 2471
struct Cyc_List_List*_tmp490=args;struct Cyc_List_List*args2=_tmp490;
struct Cyc_List_List*_tmp491=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp491;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff == 0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2477
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2481
struct Cyc_Absyn_VarargInfo _tmp492=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp67=_tmp492;struct Cyc_Absyn_VarargInfo _tmp493=_stmttmp67;int _tmp497;void*_tmp496;struct Cyc_Absyn_Tqual _tmp495;struct _fat_ptr*_tmp494;_LL22: _tmp494=_tmp493.name;_tmp495=_tmp493.tq;_tmp496=_tmp493.type;_tmp497=_tmp493.inject;_LL23: {struct _fat_ptr*n=_tmp494;struct Cyc_Absyn_Tqual tq=_tmp495;void*t=_tmp496;int i=_tmp497;
void*_tmp498=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp498;
cyc_varargs2=t2 == t?cyc_varargs:({struct Cyc_Absyn_VarargInfo*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->name=n,_tmp499->tq=tq,_tmp499->type=t2,_tmp499->inject=i;_tmp499;});}}{
# 2485
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp49A=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp49A;
struct Cyc_List_List*_tmp49B=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp49B;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49C=_cycalloc(sizeof(*_tmp49C));_tmp49C->tag=5U,(_tmp49C->f1).tvars=vs,(_tmp49C->f1).effect=eff2,(_tmp49C->f1).ret_tqual=rtq,({void*_tmp794=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp49C->f1).ret_type=_tmp794;}),(_tmp49C->f1).args=args2,(_tmp49C->f1).c_varargs=c_varargs,(_tmp49C->f1).cyc_varargs=cyc_varargs2,(_tmp49C->f1).rgn_po=rgn_po2,(_tmp49C->f1).attributes=atts,(_tmp49C->f1).requires_clause=req2,(_tmp49C->f1).requires_relns=req_relns2,(_tmp49C->f1).ensures_clause=ens2,(_tmp49C->f1).ensures_relns=ens_relns2,(_tmp49C->f1).return_value=ret_value;_tmp49C;});}}}}}case 6U: _LLB: _tmp45C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp454)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp45C;
# 2494
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp49D=tqts;struct Cyc_List_List*ts1=_tmp49D;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp49E=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp49E;
void*_tmp49F=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp49F;
if(t1 != t2)
change=1;
ts2=({struct Cyc_List_List*_tmp4A0=_region_malloc(rgn,sizeof(*_tmp4A0));_tmp4A0->hd=t2,_tmp4A0->tl=ts2;_tmp4A0;});}}
# 2503
return !change?t:(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->tag=6U,({struct Cyc_List_List*_tmp796=({struct Cyc_List_List*_tmp795=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp795,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp4A1->f1=_tmp796;});_tmp4A1;});}case 7U: _LLD: _tmp45A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp454)->f1;_tmp45B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp454)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp45A;struct Cyc_List_List*fs=_tmp45B;
# 2505
struct Cyc_List_List*_tmp4A2=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp4A2;
return fs == new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->tag=7U,_tmp4A3->f1=k,_tmp4A3->f2=new_fs;_tmp4A3;});}case 1U: _LLF: _tmp459=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp454)->f2;_LL10: {void*r=_tmp459;
# 2508
return r == 0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0U: _LL11: _tmp457=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp454)->f1;_tmp458=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp454)->f2;_LL12: {void*c=_tmp457;struct Cyc_List_List*ts=_tmp458;
# 2510
struct Cyc_List_List*_tmp4A4=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp4A4;
return ts == new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5->tag=0U,_tmp4A5->f1=c,_tmp4A5->f2=new_ts;_tmp4A5;});}case 9U: _LL13: _tmp456=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp454)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp456;
# 2513
struct Cyc_Absyn_Exp*_tmp4A6=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4A6;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->tag=9U,_tmp4A7->f1=new_e;_tmp4A7;});}case 11U: _LL15: _tmp455=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp454)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp455;
# 2516
struct Cyc_Absyn_Exp*_tmp4A8=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4A8;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->tag=11U,_tmp4A9->f1=new_e;_tmp4A9;});}default: _LL17: _LL18:
# 2519
 if(Cyc_Tcutil_skip_fntypes)return t;
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65F;_tmp65F.tag=0U,({struct _fat_ptr _tmp797=({const char*_tmp4AC="found typedecltype in rsubs";_tag_fat(_tmp4AC,sizeof(char),28U);});_tmp65F.f1=_tmp797;});_tmp65F;});void*_tmp4AA[1U];_tmp4AA[0]=& _tmp4AB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4AA,sizeof(void*),1U));});}_LL0:;}
# 2524
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2527
if(ts == 0)
return 0;{
void*_tmp4AD=(void*)ts->hd;void*old_hd=_tmp4AD;
struct Cyc_List_List*_tmp4AE=ts->tl;struct Cyc_List_List*old_tl=_tmp4AE;
void*_tmp4AF=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp4AF;
struct Cyc_List_List*_tmp4B0=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp4B0;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp4B1=_cycalloc(sizeof(*_tmp4B1));_tmp4B1->hd=new_hd,_tmp4B1->tl=new_tl;_tmp4B1;});}}
# 2538
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return inst == 0?t: Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}
# 2542
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t){
Cyc_Tcutil_skip_fntypes=1;{
void*_tmp4B2=Cyc_Tcutil_substitute(inst,t);void*new_typ=_tmp4B2;
Cyc_Tcutil_skip_fntypes=0;
return new_typ;}}
# 2550
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4B3=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4B3;
return({struct _tuple19*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->f1=tv,({void*_tmp799=({struct Cyc_Core_Opt*_tmp798=k;Cyc_Absyn_new_evar(_tmp798,({struct Cyc_Core_Opt*_tmp4B4=_cycalloc(sizeof(*_tmp4B4));_tmp4B4->v=s;_tmp4B4;}));});_tmp4B5->f2=_tmp799;});_tmp4B5;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2555
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2557
struct _tuple28*_tmp4B6=env;struct _RegionHandle*_tmp4B8;struct Cyc_List_List*_tmp4B7;_LL1: _tmp4B7=_tmp4B6->f1;_tmp4B8=_tmp4B6->f2;_LL2: {struct Cyc_List_List*s=_tmp4B7;struct _RegionHandle*rgn=_tmp4B8;
struct Cyc_Core_Opt*_tmp4B9=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4B9;
return({struct _tuple19*_tmp4BB=_region_malloc(rgn,sizeof(*_tmp4BB));_tmp4BB->f1=tv,({void*_tmp79B=({struct Cyc_Core_Opt*_tmp79A=k;Cyc_Absyn_new_evar(_tmp79A,({struct Cyc_Core_Opt*_tmp4BA=_cycalloc(sizeof(*_tmp4BA));_tmp4BA->v=s;_tmp4BA;}));});_tmp4BB->f2=_tmp79B;});_tmp4BB;});}}
# 2562
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2564
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp662;_tmp662.tag=0U,({struct _fat_ptr _tmp79C=({const char*_tmp4C1="bitfield ";_tag_fat(_tmp4C1,sizeof(char),10U);});_tmp662.f1=_tmp79C;});_tmp662;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp661;_tmp661.tag=0U,_tmp661.f1=*fn;_tmp661;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp660;_tmp660.tag=0U,({struct _fat_ptr _tmp79D=({const char*_tmp4C0=" does not have constant width";_tag_fat(_tmp4C0,sizeof(char),30U);});_tmp660.f1=_tmp79D;});_tmp660;});void*_tmp4BC[3U];_tmp4BC[0]=& _tmp4BD,_tmp4BC[1]=& _tmp4BE,_tmp4BC[2]=& _tmp4BF;({unsigned _tmp79E=loc;Cyc_Warn_err2(_tmp79E,_tag_fat(_tmp4BC,sizeof(void*),3U));});});else{
# 2569
struct _tuple13 _tmp4C2=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp68=_tmp4C2;struct _tuple13 _tmp4C3=_stmttmp68;int _tmp4C5;unsigned _tmp4C4;_LL1: _tmp4C4=_tmp4C3.f1;_tmp4C5=_tmp4C3.f2;_LL2: {unsigned i=_tmp4C4;int known=_tmp4C5;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp663;_tmp663.tag=0U,({struct _fat_ptr _tmp79F=({const char*_tmp4C8="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4C8,sizeof(char),47U);});_tmp663.f1=_tmp79F;});_tmp663;});void*_tmp4C6[1U];_tmp4C6[0]=& _tmp4C7;({unsigned _tmp7A0=loc;Cyc_Warn_warn2(_tmp7A0,_tag_fat(_tmp4C6,sizeof(void*),1U));});});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp664;_tmp664.tag=0U,({struct _fat_ptr _tmp7A1=({const char*_tmp4CB="bitfield has negative width";_tag_fat(_tmp4CB,sizeof(char),28U);});_tmp664.f1=_tmp7A1;});_tmp664;});void*_tmp4C9[1U];_tmp4C9[0]=& _tmp4CA;({unsigned _tmp7A2=loc;Cyc_Warn_err2(_tmp7A2,_tag_fat(_tmp4C9,sizeof(void*),1U));});});
w=i;}}{
# 2576
void*_tmp4CC=Cyc_Absyn_compress(field_type);void*_stmttmp69=_tmp4CC;void*_tmp4CD=_stmttmp69;enum Cyc_Absyn_Size_of _tmp4CE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4CD)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4CD)->f1)->tag == 1U){_LL4: _tmp4CE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4CD)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4CE;
# 2579
int bad;
{enum Cyc_Absyn_Size_of _tmp4CF=b;switch(_tmp4CF){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 bad=w > (unsigned)8;goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 bad=w > (unsigned)16;goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 bad=w > (unsigned)32;goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 bad=w > (unsigned)64;goto _LL8;}_LL8:;}
# 2588
if(bad)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp665;_tmp665.tag=0U,({struct _fat_ptr _tmp7A3=({const char*_tmp4D2="bitfield larger than type";_tag_fat(_tmp4D2,sizeof(char),26U);});_tmp665.f1=_tmp7A3;});_tmp665;});void*_tmp4D0[1U];_tmp4D0[0]=& _tmp4D1;({unsigned _tmp7A4=loc;Cyc_Warn_warn2(_tmp7A4,_tag_fat(_tmp4D0,sizeof(void*),1U));});});
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2592
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp669;_tmp669.tag=0U,({struct _fat_ptr _tmp7A5=({const char*_tmp4D9="bitfield ";_tag_fat(_tmp4D9,sizeof(char),10U);});_tmp669.f1=_tmp7A5;});_tmp669;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp668;_tmp668.tag=0U,_tmp668.f1=*fn;_tmp668;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp667;_tmp667.tag=0U,({struct _fat_ptr _tmp7A6=({const char*_tmp4D8=" must have integral type but has type ";_tag_fat(_tmp4D8,sizeof(char),39U);});_tmp667.f1=_tmp7A6;});_tmp667;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4D7=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp666;_tmp666.tag=2U,_tmp666.f1=(void*)field_type;_tmp666;});void*_tmp4D3[4U];_tmp4D3[0]=& _tmp4D4,_tmp4D3[1]=& _tmp4D5,_tmp4D3[2]=& _tmp4D6,_tmp4D3[3]=& _tmp4D7;({unsigned _tmp7A7=loc;Cyc_Warn_err2(_tmp7A7,_tag_fat(_tmp4D3,sizeof(void*),4U));});});
# 2594
goto _LL3;}_LL3:;}}}
# 2601
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4DA=t;void*_tmp4DC;struct Cyc_Absyn_Typedefdecl*_tmp4DB;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DA)->tag == 8U){_LL1: _tmp4DB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DA)->f3;_tmp4DC=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DA)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4DB;void*tdopt=_tmp4DC;
# 2604
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66A;_tmp66A.tag=0U,({struct _fat_ptr _tmp7A8=({const char*_tmp4DF="extra const";_tag_fat(_tmp4DF,sizeof(char),12U);});_tmp66A.f1=_tmp7A8;});_tmp66A;});void*_tmp4DD[1U];_tmp4DD[0]=& _tmp4DE;({unsigned _tmp7A9=loc;Cyc_Warn_warn2(_tmp7A9,_tag_fat(_tmp4DD,sizeof(void*),1U));});});
return 1;}
# 2609
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2616
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp7AA=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp7AA;});}
# 2620
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2626
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2629
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66D;_tmp66D.tag=0U,_tmp66D.f1=msg;_tmp66D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66C;_tmp66C.tag=0U,({struct _fat_ptr _tmp7AB=({const char*_tmp4E4=": ";_tag_fat(_tmp4E4,sizeof(char),3U);});_tmp66C.f1=_tmp7AB;});_tmp66C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66B;_tmp66B.tag=0U,({struct _fat_ptr _tmp7AC=a2string(vs->hd);_tmp66B.f1=_tmp7AC;});_tmp66B;});void*_tmp4E0[3U];_tmp4E0[0]=& _tmp4E1,_tmp4E0[1]=& _tmp4E2,_tmp4E0[2]=& _tmp4E3;({unsigned _tmp7AD=loc;Cyc_Warn_err2(_tmp7AD,_tag_fat(_tmp4E0,sizeof(void*),3U));});});}}}
# 2634
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2637
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2640
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp7AF=tvs;unsigned _tmp7AE=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp7AF,_tmp7AE,Cyc_Absynpp_tvar2string,({const char*_tmp4E5="duplicate type variable";_tag_fat(_tmp4E5,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2655 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2660
struct _RegionHandle _tmp4E6=_new_region("temp");struct _RegionHandle*temp=& _tmp4E6;_push_region(temp);
# 2664
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp7B0=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp7B0,({const char*_tmp4E7="";_tag_fat(_tmp4E7,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4E9=_region_malloc(temp,sizeof(*_tmp4E9));({struct _tuple29*_tmp7B1=({struct _tuple29*_tmp4E8=_region_malloc(temp,sizeof(*_tmp4E8));_tmp4E8->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4E8->f2=0;_tmp4E8;});_tmp4E9->hd=_tmp7B1;}),_tmp4E9->tl=fields;_tmp4E9;});}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2670
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp515="struct";_tag_fat(_tmp515,sizeof(char),7U);}):({const char*_tmp516="union";_tag_fat(_tmp516,sizeof(char),6U);});
# 2673
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4EA=(struct _tuple30*)des->hd;struct _tuple30*_stmttmp6A=_tmp4EA;struct _tuple30*_tmp4EB=_stmttmp6A;void*_tmp4ED;struct Cyc_List_List*_tmp4EC;_LL1: _tmp4EC=_tmp4EB->f1;_tmp4ED=_tmp4EB->f2;_LL2: {struct Cyc_List_List*dl=_tmp4EC;void*a=_tmp4ED;
if(dl == 0){
# 2678
struct Cyc_List_List*_tmp4EE=fields;struct Cyc_List_List*fields2=_tmp4EE;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple29*)fields2->hd)).f2){
(*((struct _tuple29*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp7B3=({struct Cyc_List_List*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));({void*_tmp7B2=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4EF=_cycalloc(sizeof(*_tmp4EF));_tmp4EF->tag=1U,_tmp4EF->f1=((*((struct _tuple29*)fields2->hd)).f1)->name;_tmp4EF;});_tmp4F0->hd=_tmp7B2;}),_tmp4F0->tl=0;_tmp4F0;});(*((struct _tuple30*)des->hd)).f1=_tmp7B3;});
ans=({struct Cyc_List_List*_tmp4F2=_region_malloc(rgn,sizeof(*_tmp4F2));({struct _tuple31*_tmp7B4=({struct _tuple31*_tmp4F1=_region_malloc(rgn,sizeof(*_tmp4F1));_tmp4F1->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4F1->f2=a;_tmp4F1;});_tmp4F2->hd=_tmp7B4;}),_tmp4F2->tl=ans;_tmp4F2;});
break;}}
# 2686
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66F;_tmp66F.tag=0U,({struct _fat_ptr _tmp7B5=({const char*_tmp4F6="too many arguments to ";_tag_fat(_tmp4F6,sizeof(char),23U);});_tmp66F.f1=_tmp7B5;});_tmp66F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66E;_tmp66E.tag=0U,_tmp66E.f1=aggr_str;_tmp66E;});void*_tmp4F3[2U];_tmp4F3[0]=& _tmp4F4,_tmp4F3[1]=& _tmp4F5;({unsigned _tmp7B6=loc;Cyc_Warn_err2(_tmp7B6,_tag_fat(_tmp4F3,sizeof(void*),2U));});});}else{
if(dl->tl != 0)
# 2690
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp670;_tmp670.tag=0U,({struct _fat_ptr _tmp7B7=({const char*_tmp4F9="multiple designators are not yet supported";_tag_fat(_tmp4F9,sizeof(char),43U);});_tmp670.f1=_tmp7B7;});_tmp670;});void*_tmp4F7[1U];_tmp4F7[0]=& _tmp4F8;({unsigned _tmp7B8=loc;Cyc_Warn_err2(_tmp7B8,_tag_fat(_tmp4F7,sizeof(void*),1U));});});else{
# 2693
void*_tmp4FA=(void*)dl->hd;void*_stmttmp6B=_tmp4FA;void*_tmp4FB=_stmttmp6B;struct _fat_ptr*_tmp4FC;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4FB)->tag == 0U){_LL4: _LL5:
# 2695
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp672;_tmp672.tag=0U,({struct _fat_ptr _tmp7B9=({const char*_tmp500="array designator used in argument to ";_tag_fat(_tmp500,sizeof(char),38U);});_tmp672.f1=_tmp7B9;});_tmp672;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp671;_tmp671.tag=0U,_tmp671.f1=aggr_str;_tmp671;});void*_tmp4FD[2U];_tmp4FD[0]=& _tmp4FE,_tmp4FD[1]=& _tmp4FF;({unsigned _tmp7BA=loc;Cyc_Warn_err2(_tmp7BA,_tag_fat(_tmp4FD,sizeof(void*),2U));});});
goto _LL3;}else{_LL6: _tmp4FC=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4FB)->f1;_LL7: {struct _fat_ptr*v=_tmp4FC;
# 2698
struct Cyc_List_List*_tmp501=fields;struct Cyc_List_List*fields2=_tmp501;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple29*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple29*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp503=({struct Cyc_Warn_String_Warn_Warg_struct _tmp675;_tmp675.tag=0U,({struct _fat_ptr _tmp7BB=({const char*_tmp507="member ";_tag_fat(_tmp507,sizeof(char),8U);});_tmp675.f1=_tmp7BB;});_tmp675;});struct Cyc_Warn_String_Warn_Warg_struct _tmp504=({struct Cyc_Warn_String_Warn_Warg_struct _tmp674;_tmp674.tag=0U,_tmp674.f1=*v;_tmp674;});struct Cyc_Warn_String_Warn_Warg_struct _tmp505=({struct Cyc_Warn_String_Warn_Warg_struct _tmp673;_tmp673.tag=0U,({struct _fat_ptr _tmp7BC=({const char*_tmp506=" has already been used as an argument";_tag_fat(_tmp506,sizeof(char),38U);});_tmp673.f1=_tmp7BC;});_tmp673;});void*_tmp502[3U];_tmp502[0]=& _tmp503,_tmp502[1]=& _tmp504,_tmp502[2]=& _tmp505;({unsigned _tmp7BD=loc;Cyc_Warn_err2(_tmp7BD,_tag_fat(_tmp502,sizeof(void*),3U));});});
(*((struct _tuple29*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp509=_region_malloc(rgn,sizeof(*_tmp509));({struct _tuple31*_tmp7BE=({struct _tuple31*_tmp508=_region_malloc(rgn,sizeof(*_tmp508));_tmp508->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp508->f2=a;_tmp508;});_tmp509->hd=_tmp7BE;}),_tmp509->tl=ans;_tmp509;});
break;}}
# 2707
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp677;_tmp677.tag=0U,({struct _fat_ptr _tmp7BF=({const char*_tmp50D="bad field designator ";_tag_fat(_tmp50D,sizeof(char),22U);});_tmp677.f1=_tmp7BF;});_tmp677;});struct Cyc_Warn_String_Warn_Warg_struct _tmp50C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp676;_tmp676.tag=0U,_tmp676.f1=*v;_tmp676;});void*_tmp50A[2U];_tmp50A[0]=& _tmp50B,_tmp50A[1]=& _tmp50C;({unsigned _tmp7C0=loc;Cyc_Warn_err2(_tmp7C0,_tag_fat(_tmp50A,sizeof(void*),2U));});});
goto _LL3;}}_LL3:;}}}}
# 2714
if((int)aggr_kind == (int)0U){
if(({int _tmp7C1=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans);_tmp7C1 < ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp678;_tmp678.tag=0U,({struct _fat_ptr _tmp7C2=({const char*_tmp510="too few arguments to struct";_tag_fat(_tmp510,sizeof(char),28U);});_tmp678.f1=_tmp7C2;});_tmp678;});void*_tmp50E[1U];_tmp50E[0]=& _tmp50F;({unsigned _tmp7C3=loc;Cyc_Warn_err2(_tmp7C3,_tag_fat(_tmp50E,sizeof(void*),1U));});});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp512=({struct Cyc_Warn_String_Warn_Warg_struct _tmp679;_tmp679.tag=0U,({struct _fat_ptr _tmp7C4=({const char*_tmp513="union expression should have one member";_tag_fat(_tmp513,sizeof(char),40U);});_tmp679.f1=_tmp7C4;});_tmp679;});void*_tmp511[1U];_tmp511[0]=& _tmp512;({unsigned _tmp7C5=loc;Cyc_Warn_err2(_tmp7C5,_tag_fat(_tmp511,sizeof(void*),1U));});});}{
struct Cyc_List_List*_tmp514=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp514;}}}
# 2664
;_pop_region();}
# 2725
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2727
void*_tmp517=e1->r;void*_stmttmp6C=_tmp517;void*_tmp518=_stmttmp6C;struct Cyc_Absyn_Exp*_tmp519;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51E;switch(*((int*)_tmp518)){case 14U: _LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp520=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67B;_tmp67B.tag=0U,({struct _fat_ptr _tmp7C6=({const char*_tmp522="we have a cast in a lhs: ";_tag_fat(_tmp522,sizeof(char),26U);});_tmp67B.f1=_tmp7C6;});_tmp67B;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp521=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp67A;_tmp67A.tag=4U,_tmp67A.f1=e1;_tmp67A;});void*_tmp51F[2U];_tmp51F[0]=& _tmp520,_tmp51F[1]=& _tmp521;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp51F,sizeof(void*),2U));});case 20U: _LL3: _tmp51E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp518)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp51E;
_tmp51D=e1a;goto _LL6;}case 23U: _LL5: _tmp51D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp518)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp51D;
# 2731
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp51C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp518)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp51C;
_tmp51B=e1a;goto _LLA;}case 21U: _LL9: _tmp51B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp518)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp51B;
# 2735
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp524=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67D;_tmp67D.tag=0U,({struct _fat_ptr _tmp7C7=({const char*_tmp526="found zero pointer aggregate member assignment: ";_tag_fat(_tmp526,sizeof(char),49U);});_tmp67D.f1=_tmp7C7;});_tmp67D;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp525=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp67C;_tmp67C.tag=4U,_tmp67C.f1=e1;_tmp67C;});void*_tmp523[2U];_tmp523[0]=& _tmp524,_tmp523[1]=& _tmp525;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp523,sizeof(void*),2U));});
return 0;}case 13U: _LLB: _tmp51A=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp518)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp51A;
_tmp519=e1a;goto _LLE;}case 12U: _LLD: _tmp519=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp518)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp519;
# 2740
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp528=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67F;_tmp67F.tag=0U,({struct _fat_ptr _tmp7C8=({const char*_tmp52A="found zero pointer instantiate/noinstantiate: ";_tag_fat(_tmp52A,sizeof(char),47U);});_tmp67F.f1=_tmp7C8;});_tmp67F;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp529=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp67E;_tmp67E.tag=4U,_tmp67E.f1=e1;_tmp67E;});void*_tmp527[2U];_tmp527[0]=& _tmp528,_tmp527[1]=& _tmp529;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp527,sizeof(void*),2U));});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp681;_tmp681.tag=0U,({struct _fat_ptr _tmp7C9=({const char*_tmp52E="found bad lhs in is_zero_ptr_deref: ";_tag_fat(_tmp52E,sizeof(char),37U);});_tmp681.f1=_tmp7C9;});_tmp681;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp52D=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp680;_tmp680.tag=4U,_tmp680.f1=e1;_tmp680;});void*_tmp52B[2U];_tmp52B[0]=& _tmp52C,_tmp52B[1]=& _tmp52D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp52B,sizeof(void*),2U));});}_LL0:;}
# 2754
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmp52F=Cyc_Absyn_compress(r);void*_stmttmp6D=_tmp52F;void*_tmp530=_stmttmp6D;struct Cyc_Absyn_Tvar*_tmp531;enum Cyc_Absyn_AliasQual _tmp533;enum Cyc_Absyn_KindQual _tmp532;switch(*((int*)_tmp530)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp530)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp530)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp530)->f4 == 0){_LL5: _tmp532=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp530)->f3)->kind)->v)->kind;_tmp533=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp530)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp532;enum Cyc_Absyn_AliasQual a=_tmp533;
# 2759
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp531=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp530)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp531;
# 2763
struct Cyc_Absyn_Kind*_tmp534=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp6E=_tmp534;struct Cyc_Absyn_Kind*_tmp535=_stmttmp6E;enum Cyc_Absyn_AliasQual _tmp537;enum Cyc_Absyn_KindQual _tmp536;_LLC: _tmp536=_tmp535->kind;_tmp537=_tmp535->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp536;enum Cyc_Absyn_AliasQual a=_tmp537;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp538=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp6F=_tmp538;void*_tmp539=_stmttmp6F;struct Cyc_Core_Opt**_tmp53A;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp539)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp539)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp539)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp53A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp539)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp53A;
# 2767
({struct Cyc_Core_Opt*_tmp7CB=({struct Cyc_Core_Opt*_tmp53C=_cycalloc(sizeof(*_tmp53C));({void*_tmp7CA=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp53B=_cycalloc(sizeof(*_tmp53B));_tmp53B->tag=2U,_tmp53B->f1=0,_tmp53B->f2=& Cyc_Tcutil_rk;_tmp53B;});_tmp53C->v=_tmp7CA;});_tmp53C;});*x=_tmp7CB;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2771
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2778
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp53D=Cyc_Absyn_compress(t);void*_stmttmp70=_tmp53D;void*_tmp53E=_stmttmp70;struct Cyc_Absyn_Tvar*_tmp53F;void*_tmp540;switch(*((int*)_tmp53E)){case 3U: _LL1: _tmp540=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53E)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp540;
# 2781
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp53F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53E)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp53F;
# 2783
struct Cyc_Absyn_Kind*_tmp541=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp71=_tmp541;struct Cyc_Absyn_Kind*_tmp542=_stmttmp71;enum Cyc_Absyn_AliasQual _tmp544;enum Cyc_Absyn_KindQual _tmp543;_LL8: _tmp543=_tmp542->kind;_tmp544=_tmp542->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp543;enum Cyc_Absyn_AliasQual a=_tmp544;
enum Cyc_Absyn_KindQual _tmp545=k;switch(_tmp545){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp546=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp72=_tmp546;void*_tmp547=_stmttmp72;enum Cyc_Absyn_KindQual _tmp549;struct Cyc_Core_Opt**_tmp548;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp547)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp547)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp548=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp547)->f1;_tmp549=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp547)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp548;enum Cyc_Absyn_KindQual k=_tmp549;
# 2789
({struct Cyc_Core_Opt*_tmp7CE=({struct Cyc_Core_Opt*_tmp54C=_cycalloc(sizeof(*_tmp54C));({void*_tmp7CD=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->tag=2U,_tmp54B->f1=0,({struct Cyc_Absyn_Kind*_tmp7CC=({struct Cyc_Absyn_Kind*_tmp54A=_cycalloc(sizeof(*_tmp54A));_tmp54A->kind=k,_tmp54A->aliasqual=Cyc_Absyn_Aliasable;_tmp54A;});_tmp54B->f2=_tmp7CC;});_tmp54B;});_tmp54C->v=_tmp7CD;});_tmp54C;});*x=_tmp7CE;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
 return 1;}_LL13:;}
# 2793
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2796
 return 0;}_LL0:;}
# 2799
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
t=Cyc_Absyn_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(t,0))
return 1;{
void*_tmp54D=t;struct Cyc_List_List*_tmp54E;struct Cyc_List_List*_tmp550;union Cyc_Absyn_DatatypeFieldInfo _tmp54F;struct Cyc_List_List*_tmp552;struct Cyc_Absyn_Aggrdecl**_tmp551;struct Cyc_List_List*_tmp553;switch(*((int*)_tmp54D)){case 6U: _LL1: _tmp553=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54D)->f1;_LL2: {struct Cyc_List_List*qts=_tmp553;
# 2805
for(0;qts != 0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54D)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54D)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp551=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54D)->f1)->f1).KnownAggr).val;_tmp552=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54D)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp551;struct Cyc_List_List*ts=_tmp552;
# 2810
if((*adp)->impl == 0)
return 0;{
struct Cyc_List_List*_tmp554=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp554;
{struct Cyc_List_List*_tmp555=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp555;for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type)))
return 1;}}
return 0;}}}else{_LL7: _LL8:
# 2822
 goto _LLA;}case 18U: _LL9: _LLA:
 return 0;case 19U: _LLB: _tmp54F=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54D)->f1)->f1;_tmp550=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54D)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp54F;struct Cyc_List_List*ts=_tmp550;
# 2826
union Cyc_Absyn_DatatypeFieldInfo _tmp556=tinfo;struct Cyc_Absyn_Datatypefield*_tmp558;struct Cyc_Absyn_Datatypedecl*_tmp557;if((_tmp556.UnknownDatatypefield).tag == 1){_LL10: _LL11:
 return 0;}else{_LL12: _tmp557=((_tmp556.KnownDatatypefield).val).f1;_tmp558=((_tmp556.KnownDatatypefield).val).f2;_LL13: {struct Cyc_Absyn_Datatypedecl*td=_tmp557;struct Cyc_Absyn_Datatypefield*fld=_tmp558;
# 2829
struct Cyc_List_List*_tmp559=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp559;
{struct Cyc_List_List*_tmp55A=fld->typs;struct Cyc_List_List*typs=_tmp55A;for(0;typs != 0;typs=typs->tl){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 2835
return 0;}}_LLF:;}default: goto _LLD;}case 7U: _LL5: _tmp54E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54D)->f2;_LL6: {struct Cyc_List_List*x=_tmp54E;
# 2818
for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD: _LLE:
# 2837
 return 0;}_LL0:;}}
# 2844
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp55B=e->r;void*_stmttmp73=_tmp55B;void*_tmp55C=_stmttmp73;struct Cyc_Absyn_Stmt*_tmp55D;struct Cyc_Absyn_Exp*_tmp55E;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp563;struct Cyc_Absyn_Exp*_tmp562;struct _fat_ptr*_tmp565;struct Cyc_Absyn_Exp*_tmp564;struct Cyc_Absyn_Exp*_tmp566;struct Cyc_Absyn_Exp*_tmp567;switch(*((int*)_tmp55C)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp55C)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp567=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55C)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp567;
_tmp566=e1;goto _LL6;}case 20U: _LL5: _tmp566=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp55C)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp566;
# 2849
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp564=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55C)->f1;_tmp565=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55C)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp564;struct _fat_ptr*f=_tmp565;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp562=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55C)->f1;_tmp563=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55C)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp562;struct Cyc_Absyn_Exp*e2=_tmp563;
# 2852
void*_tmp568=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp74=_tmp568;void*_tmp569=_stmttmp74;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp569)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp560=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp55C)->f2;_tmp561=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp55C)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp560;struct Cyc_Absyn_Exp*e2=_tmp561;
# 2857
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp55F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp55C)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp55F;
_tmp55E=e2;goto _LL10;}case 14U: _LLF: _tmp55E=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp55C)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp55E;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp55D=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp55C)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp55D;
# 2861
while(1){
void*_tmp56A=s->r;void*_stmttmp75=_tmp56A;void*_tmp56B=_stmttmp75;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_Absyn_Stmt*_tmp56E;struct Cyc_Absyn_Decl*_tmp56D;struct Cyc_Absyn_Stmt*_tmp570;struct Cyc_Absyn_Stmt*_tmp56F;switch(*((int*)_tmp56B)){case 2U: _LL1B: _tmp56F=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56B)->f1;_tmp570=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56B)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp56F;struct Cyc_Absyn_Stmt*s2=_tmp570;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp56D=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56B)->f1;_tmp56E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56B)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp56D;struct Cyc_Absyn_Stmt*s1=_tmp56E;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp56C=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp56B)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp56C;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp572=({struct Cyc_Warn_String_Warn_Warg_struct _tmp682;_tmp682.tag=0U,({struct _fat_ptr _tmp7CF=({const char*_tmp573="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp573,sizeof(char),40U);});_tmp682.f1=_tmp7CF;});_tmp682;});void*_tmp571[1U];_tmp571[0]=& _tmp572;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp571,sizeof(void*),1U));});}_LL1A:;}}default: _LL13: _LL14:
# 2869
 return 1;}_LL0:;}
# 2886 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 2888
struct _tuple14 bogus_ans=({struct _tuple14 _tmp690;_tmp690.f1=0,_tmp690.f2=Cyc_Absyn_heap_rgn_type;_tmp690;});
void*_tmp574=e->r;void*_stmttmp76=_tmp574;void*_tmp575=_stmttmp76;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp578;int _tmp57B;struct _fat_ptr*_tmp57A;struct Cyc_Absyn_Exp*_tmp579;int _tmp57E;struct _fat_ptr*_tmp57D;struct Cyc_Absyn_Exp*_tmp57C;void*_tmp57F;switch(*((int*)_tmp575)){case 1U: _LL1: _tmp57F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_LL2: {void*x=_tmp57F;
# 2892
void*_tmp580=x;struct Cyc_Absyn_Vardecl*_tmp581;struct Cyc_Absyn_Vardecl*_tmp582;struct Cyc_Absyn_Vardecl*_tmp583;struct Cyc_Absyn_Vardecl*_tmp584;switch(*((int*)_tmp580)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp584=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp580)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp584;
# 2896
void*_tmp585=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp77=_tmp585;void*_tmp586=_stmttmp77;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp586)->tag == 4U){_LL1B: _LL1C:
# 2898
 return({struct _tuple14 _tmp683;_tmp683.f1=1,_tmp683.f2=Cyc_Absyn_heap_rgn_type;_tmp683;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp684;_tmp684.f1=(vd->tq).real_const,_tmp684.f2=Cyc_Absyn_heap_rgn_type;_tmp684;});}_LL1A:;}case 4U: _LL14: _tmp583=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp580)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp583;
# 2902
void*_tmp587=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp78=_tmp587;void*_tmp588=_stmttmp78;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp588)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp685;_tmp685.f1=1,_tmp685.f2=(void*)_check_null(vd->rgn);_tmp685;});}else{_LL22: _LL23:
# 2905
 vd->escapes=1;
return({struct _tuple14 _tmp686;_tmp686.f1=(vd->tq).real_const,_tmp686.f2=(void*)_check_null(vd->rgn);_tmp686;});}_LL1F:;}case 5U: _LL16: _tmp582=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp580)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp582;
# 2908
_tmp581=vd;goto _LL19;}default: _LL18: _tmp581=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp580)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp581;
# 2910
vd->escapes=1;
return({struct _tuple14 _tmp687;_tmp687.f1=(vd->tq).real_const,_tmp687.f2=(void*)_check_null(vd->rgn);_tmp687;});}}_LLD:;}case 21U: _LL3: _tmp57C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp57D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp57E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp57C;struct _fat_ptr*f=_tmp57D;int is_tagged=_tmp57E;
# 2915
if(is_tagged)return bogus_ans;{
# 2918
void*_tmp589=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp79=_tmp589;void*_tmp58A=_stmttmp79;struct Cyc_Absyn_Aggrdecl*_tmp58B;struct Cyc_List_List*_tmp58C;switch(*((int*)_tmp58A)){case 7U: _LL25: _tmp58C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp58A)->f2;_LL26: {struct Cyc_List_List*fs=_tmp58C;
# 2920
struct Cyc_Absyn_Aggrfield*_tmp58D=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp58D;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp58E=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7A=_tmp58E;struct _tuple14 _tmp58F=_stmttmp7A;void*_tmp591;int _tmp590;_LL2C: _tmp590=_tmp58F.f1;_tmp591=_tmp58F.f2;_LL2D: {int c=_tmp590;void*t=_tmp591;
return({struct _tuple14 _tmp688;_tmp688.f1=(finfo->tq).real_const || c,_tmp688.f2=t;_tmp688;});}}
# 2925
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58A)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp58B=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58A)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp58B;
# 2927
struct Cyc_Absyn_Aggrfield*_tmp592=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp592;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp593=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7B=_tmp593;struct _tuple14 _tmp594=_stmttmp7B;void*_tmp596;int _tmp595;_LL2F: _tmp595=_tmp594.f1;_tmp596=_tmp594.f2;_LL30: {int c=_tmp595;void*t=_tmp596;
return({struct _tuple14 _tmp689;_tmp689.f1=(finfo->tq).real_const || c,_tmp689.f2=t;_tmp689;});}}
# 2932
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp579=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp57A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_tmp57B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp575)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp579;struct _fat_ptr*f=_tmp57A;int is_tagged=_tmp57B;
# 2937
if(is_tagged)return bogus_ans;{
# 2940
void*_tmp597=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp7C=_tmp597;void*_tmp598=_stmttmp7C;void*_tmp59A;void*_tmp599;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598)->tag == 3U){_LL32: _tmp599=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598)->f1).elt_type;_tmp59A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp598)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp599;void*r=_tmp59A;
# 2942
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp59B=Cyc_Absyn_compress(t1);void*_stmttmp7D=_tmp59B;void*_tmp59C=_stmttmp7D;struct Cyc_Absyn_Aggrdecl*_tmp59D;struct Cyc_List_List*_tmp59E;switch(*((int*)_tmp59C)){case 7U: _LL37: _tmp59E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp59C)->f2;_LL38: {struct Cyc_List_List*fs=_tmp59E;
# 2945
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59C)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59C)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp59D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59C)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp59D;
# 2947
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 2950
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp68A;_tmp68A.f1=(finfo->tq).real_const,_tmp68A.f2=r;_tmp68A;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp578=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp578;
# 2957
void*_tmp59F=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp7E=_tmp59F;void*_tmp5A0=_stmttmp7E;void*_tmp5A2;struct Cyc_Absyn_Tqual _tmp5A1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A0)->tag == 3U){_LL3E: _tmp5A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A0)->f1).elt_tq;_tmp5A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A0)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp5A1;void*r=_tmp5A2;
# 2959
return({struct _tuple14 _tmp68B;_tmp68B.f1=tq.real_const,_tmp68B.f2=r;_tmp68B;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp576=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp575)->f1;_tmp577=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp575)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp576;struct Cyc_Absyn_Exp*e2=_tmp577;
# 2965
void*t=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*_tmp5A3=t;struct Cyc_Absyn_Tqual _tmp5A4;void*_tmp5A6;struct Cyc_Absyn_Tqual _tmp5A5;struct Cyc_List_List*_tmp5A7;switch(*((int*)_tmp5A3)){case 6U: _LL43: _tmp5A7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5A3)->f1;_LL44: {struct Cyc_List_List*ts=_tmp5A7;
# 2969
struct _tuple13 _tmp5A8=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp7F=_tmp5A8;struct _tuple13 _tmp5A9=_stmttmp7F;int _tmp5AB;unsigned _tmp5AA;_LL4C: _tmp5AA=_tmp5A9.f1;_tmp5AB=_tmp5A9.f2;_LL4D: {unsigned i=_tmp5AA;int known=_tmp5AB;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp5AC=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp5AC;
if(finfo != 0)
return({struct _tuple14 _tmp68C;_tmp68C.f1=((*finfo).f1).real_const,({void*_tmp7D0=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp68C.f2=_tmp7D0;});_tmp68C;});
return bogus_ans;}}}case 3U: _LL45: _tmp5A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).elt_tq;_tmp5A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A3)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp5A5;void*r=_tmp5A6;
# 2977
return({struct _tuple14 _tmp68D;_tmp68D.f1=tq.real_const,_tmp68D.f2=r;_tmp68D;});}case 4U: _LL47: _tmp5A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A3)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp5A4;
# 2983
return({struct _tuple14 _tmp68E;_tmp68E.f1=tq.real_const,({void*_tmp7D1=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp68E.f2=_tmp7D1;});_tmp68E;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 2987
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68F;_tmp68F.tag=0U,({struct _fat_ptr _tmp7D2=({const char*_tmp5AF="unary & applied to non-lvalue";_tag_fat(_tmp5AF,sizeof(char),30U);});_tmp68F.f1=_tmp7D2;});_tmp68F;});void*_tmp5AD[1U];_tmp5AD[0]=& _tmp5AE;({unsigned _tmp7D3=e->loc;Cyc_Warn_err2(_tmp7D3,_tag_fat(_tmp5AD,sizeof(void*),1U));});});
return bogus_ans;}_LL0:;}
# 2994
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp5B0=({void*_tmp7D4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7D4,b);});struct Cyc_Absyn_Exp*eopt=_tmp5B0;
if(eopt == 0)return;{
struct _tuple13 _tmp5B1=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp80=_tmp5B1;struct _tuple13 _tmp5B2=_stmttmp80;int _tmp5B4;unsigned _tmp5B3;_LL1: _tmp5B3=_tmp5B2.f1;_tmp5B4=_tmp5B2.f2;_LL2: {unsigned j=_tmp5B3;int known=_tmp5B4;
if(known && j <= i)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp694;_tmp694.tag=0U,({struct _fat_ptr _tmp7D5=({const char*_tmp5BB="a dereference is out of bounds: ";_tag_fat(_tmp5BB,sizeof(char),33U);});_tmp694.f1=_tmp7D5;});_tmp694;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B7=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp693;_tmp693.tag=12U,_tmp693.f1=(int)j;_tmp693;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5B8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp692;_tmp692.tag=0U,({
struct _fat_ptr _tmp7D6=({const char*_tmp5BA=" <= ";_tag_fat(_tmp5BA,sizeof(char),5U);});_tmp692.f1=_tmp7D6;});_tmp692;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B9=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp691;_tmp691.tag=12U,_tmp691.f1=(int)i;_tmp691;});void*_tmp5B5[4U];_tmp5B5[0]=& _tmp5B6,_tmp5B5[1]=& _tmp5B7,_tmp5B5[2]=& _tmp5B8,_tmp5B5[3]=& _tmp5B9;({void(*_tmp7D8)(unsigned,struct _fat_ptr)=
# 2999
do_warn?Cyc_Warn_warn2: Cyc_Warn_err2;unsigned _tmp7D7=loc;_tmp7D8(_tmp7D7,_tag_fat(_tmp5B5,sizeof(void*),4U));});});
# 3001
return;}}}
# 3004
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3012
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
int(*_tmp5BC)(int var_okay,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_cnst_exp;int(*rec)(int var_okay,struct Cyc_Absyn_Exp*e)=_tmp5BC;
void*_tmp5BD=e->r;void*_stmttmp81=_tmp5BD;void*_tmp5BE=_stmttmp81;struct Cyc_List_List*_tmp5BF;struct Cyc_List_List*_tmp5C0;struct Cyc_List_List*_tmp5C1;struct Cyc_List_List*_tmp5C2;struct Cyc_List_List*_tmp5C3;struct Cyc_List_List*_tmp5C4;struct Cyc_Absyn_Exp*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C8;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5CA;struct Cyc_Absyn_Exp*_tmp5CB;struct Cyc_Absyn_Exp*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CE;struct Cyc_Absyn_Exp*_tmp5CD;struct Cyc_Absyn_Exp*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5CF;void*_tmp5D2;switch(*((int*)_tmp5BE)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5D2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL10: {void*b=_tmp5D2;
# 3025
void*_tmp5D3=b;struct Cyc_Absyn_Vardecl*_tmp5D4;struct Cyc_Absyn_Vardecl*_tmp5D5;switch(*((int*)_tmp5D3)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmp5D5=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5D3)->f1;_LL35: {struct Cyc_Absyn_Vardecl*vd=_tmp5D5;
# 3028
void*_tmp5D6=Cyc_Absyn_compress(vd->type);void*_stmttmp82=_tmp5D6;void*_tmp5D7=_stmttmp82;switch(*((int*)_tmp5D7)){case 4U: _LL3D: _LL3E:
 goto _LL40;case 5U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmp5D4=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5D3)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5D4;
# 3034
if((int)vd->sc != (int)Cyc_Absyn_Static)
return var_okay;{
void*_tmp5D8=Cyc_Absyn_compress(vd->type);void*_stmttmp83=_tmp5D8;void*_tmp5D9=_stmttmp83;switch(*((int*)_tmp5D9)){case 4U: _LL44: _LL45:
 goto _LL47;case 5U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}}case 0U: _LL38: _LL39:
# 3041
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmp5CF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_tmp5D0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5BE)->f2;_tmp5D1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5BE)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5CF;struct Cyc_Absyn_Exp*e2=_tmp5D0;struct Cyc_Absyn_Exp*e3=_tmp5D1;
# 3045
return(rec(0,e1)&& rec(0,e2))&& rec(0,e3);}case 9U: _LL13: _tmp5CD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_tmp5CE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5BE)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5CD;struct Cyc_Absyn_Exp*e2=_tmp5CE;
return rec(0,e1)&& rec(0,e2);}case 41U: _LL15: _tmp5CC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5CC;
_tmp5CB=e2;goto _LL18;}case 12U: _LL17: _tmp5CB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5CB;
_tmp5CA=e2;goto _LL1A;}case 13U: _LL19: _tmp5CA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5CA;
_tmp5C9=e2;goto _LL1C;}case 14U: _LL1B: _tmp5C9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5BE)->f2;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp5C9;
# 3052
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 15U: _LL1D: _tmp5C8=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL1E: {struct Cyc_Absyn_Exp*e2=_tmp5C8;
return rec(1,e2);}case 27U: _LL1F: _tmp5C6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BE)->f2;_tmp5C7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BE)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp5C6;struct Cyc_Absyn_Exp*e2=_tmp5C7;
return rec(0,e1)&& rec(0,e2);}case 28U: _LL21: _tmp5C5=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp5C5;
return rec(0,e);}case 26U: _LL23: _tmp5C4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL24: {struct Cyc_List_List*des=_tmp5C4;
_tmp5C3=des;goto _LL26;}case 30U: _LL25: _tmp5C3=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5BE)->f2;_LL26: {struct Cyc_List_List*des=_tmp5C3;
_tmp5C2=des;goto _LL28;}case 29U: _LL27: _tmp5C2=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BE)->f3;_LL28: {struct Cyc_List_List*des=_tmp5C2;
# 3059
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL29: _tmp5C1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5BE)->f2;_LL2A: {struct Cyc_List_List*es=_tmp5C1;
_tmp5C0=es;goto _LL2C;}case 24U: _LL2B: _tmp5C0=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL2C: {struct Cyc_List_List*es=_tmp5C0;
_tmp5BF=es;goto _LL2E;}case 31U: _LL2D: _tmp5BF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5BE)->f1;_LL2E: {struct Cyc_List_List*es=_tmp5BF;
# 3066
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 3073
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3077
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5DA=Cyc_Absyn_compress(t);void*_stmttmp84=_tmp5DA;void*_tmp5DB=_stmttmp84;struct Cyc_List_List*_tmp5DC;struct Cyc_List_List*_tmp5DD;void*_tmp5DE;void*_tmp5DF;struct Cyc_List_List*_tmp5E1;void*_tmp5E0;switch(*((int*)_tmp5DB)){case 0U: _LL1: _tmp5E0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5DB)->f1;_tmp5E1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5DB)->f2;_LL2: {void*c=_tmp5E0;struct Cyc_List_List*ts=_tmp5E1;
# 3081
void*_tmp5E2=c;union Cyc_Absyn_AggrInfo _tmp5E3;struct Cyc_List_List*_tmp5E4;struct Cyc_Absyn_Enumdecl*_tmp5E5;switch(*((int*)_tmp5E2)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5E5=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5E2)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5E5;
# 3086
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5E4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5E4=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5E2)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5E4;
# 3090
{struct Cyc_List_List*_tmp5E6=fs;struct Cyc_List_List*fs2=_tmp5E6;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5E7=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp85=_tmp5E7;struct _tuple13 _tmp5E8=_stmttmp85;int _tmp5EA;unsigned _tmp5E9;_LL1D: _tmp5E9=_tmp5E8.f1;_tmp5EA=_tmp5E8.f2;_LL1E: {unsigned i=_tmp5E9;int known=_tmp5EA;
if(known && i == (unsigned)0)
return 1;}}}}
# 3097
return 0;}case 20U: _LL18: _tmp5E3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5E2)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5E3;
# 3100
struct Cyc_Absyn_Aggrdecl*_tmp5EB=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5EB;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5DF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DB)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5DF;
# 3108
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DB)->f1).elt_type;_LL6: {void*t=_tmp5DE;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5DD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5DB)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5DD;
# 3111
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7U: _LL9: _tmp5DC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5DB)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5DC;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3119
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3121
struct _RegionHandle _tmp5EC=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5EC;_push_region(rgn);
{struct Cyc_List_List*_tmp5ED=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5ED;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5EE=0;_npop_handler(0U);return _tmp5EE;}}{
# 3129
int _tmp5EF=1;_npop_handler(0U);return _tmp5EF;}}
# 3122
;_pop_region();}
# 3133
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5F0=t;struct Cyc_Absyn_Typedefdecl*_tmp5F1;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5F0)->tag == 8U){_LL1: _tmp5F1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5F0)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5F1;
# 3136
if(tdopt == 0)
return;{
struct Cyc_Absyn_Tqual _tmp5F2=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5F2;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp697;_tmp697.tag=0U,({struct _fat_ptr _tmp7D9=({const char*_tmp5F8="qualifier within typedef type ";_tag_fat(_tmp5F8,sizeof(char),31U);});_tmp697.f1=_tmp7D9;});_tmp697;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5F5=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp696;_tmp696.tag=2U,_tmp696.f1=(void*)t;_tmp696;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp695;_tmp695.tag=0U,({struct _fat_ptr _tmp7DA=({const char*_tmp5F7=" is ignored";_tag_fat(_tmp5F7,sizeof(char),12U);});_tmp695.f1=_tmp7DA;});_tmp695;});void*_tmp5F3[3U];_tmp5F3[0]=& _tmp5F4,_tmp5F3[1]=& _tmp5F5,_tmp5F3[2]=& _tmp5F6;({unsigned _tmp7DB=loc;Cyc_Warn_warn2(_tmp7DB,_tag_fat(_tmp5F3,sizeof(void*),3U));});});
return;}}}else{_LL3: _LL4:
 return;}_LL0:;}
# 3147
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5F9=Cyc_Absyn_compress(t);void*_stmttmp86=_tmp5F9;void*_tmp5FA=_stmttmp86;struct Cyc_Absyn_Exp*_tmp5FB;struct Cyc_Absyn_PtrInfo _tmp5FC;switch(*((int*)_tmp5FA)){case 3U: _LL1: _tmp5FC=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FA)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp5FC;
return({void*_tmp7DC=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7DC,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp5FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5FA)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5FB;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3157
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5FD=b;struct Cyc_Absyn_Vardecl*_tmp5FE;struct Cyc_Absyn_Vardecl*_tmp5FF;struct Cyc_Absyn_Vardecl*_tmp600;struct Cyc_Absyn_Vardecl*_tmp601;switch(*((int*)_tmp5FD)){case 5U: _LL1: _tmp601=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5FD)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp601;
_tmp600=x;goto _LL4;}case 4U: _LL3: _tmp600=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5FD)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp600;
_tmp5FF=x;goto _LL6;}case 3U: _LL5: _tmp5FF=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5FD)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp5FF;
_tmp5FE=x;goto _LL8;}case 1U: _LL7: _tmp5FE=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5FD)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp5FE;
return x->escapes?0: x;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3168
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp602=0;struct Cyc_List_List*res=_tmp602;
for(0;l != 0;l=l->tl){
if((void**)l->hd != 0)res=({struct Cyc_List_List*_tmp603=_cycalloc(sizeof(*_tmp603));_tmp603->hd=*((void**)_check_null((void**)l->hd)),_tmp603->tl=res;_tmp603;});}
return res;}
# 3175
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp604=Cyc_Absyn_compress(t);void*_stmttmp87=_tmp604;void*_tmp605=_stmttmp87;unsigned _tmp60A;void*_tmp609;struct Cyc_Absyn_Exp*_tmp608;struct Cyc_Absyn_Tqual _tmp607;void*_tmp606;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp605)->tag == 4U){_LL1: _tmp606=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp605)->f1).elt_type;_tmp607=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp605)->f1).tq;_tmp608=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp605)->f1).num_elts;_tmp609=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp605)->f1).zero_term;_tmp60A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp605)->f1).zt_loc;_LL2: {void*et=_tmp606;struct Cyc_Absyn_Tqual tq=_tmp607;struct Cyc_Absyn_Exp*eopt=_tmp608;void*zt=_tmp609;unsigned ztl=_tmp60A;
# 3178
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
if(convert_tag){
if(eopt->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp60C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp698;_tmp698.tag=0U,({struct _fat_ptr _tmp7DD=({const char*_tmp60D="cannot convert tag without type!";_tag_fat(_tmp60D,sizeof(char),33U);});_tmp698.f1=_tmp7DD;});_tmp698;});void*_tmp60B[1U];_tmp60B[0]=& _tmp60C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp60B,sizeof(void*),1U));});{
void*_tmp60E=Cyc_Absyn_compress((void*)_check_null(eopt->topt));void*_stmttmp88=_tmp60E;void*_tmp60F=_stmttmp88;void*_tmp610;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp60F)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp60F)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp60F)->f2 != 0){_LL6: _tmp610=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp60F)->f2)->hd;_LL7: {void*t=_tmp610;
# 3187
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3192
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
