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
# 900
void*Cyc_Absyn_compress(void*);
# 905
int Cyc_Absyn_type2bool(int def,void*);
# 914
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 919
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 921
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 926
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 928
extern void*Cyc_Absyn_empty_effect;
# 930
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 932
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 957
extern void*Cyc_Absyn_fat_bound_type;
# 959
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 961
void*Cyc_Absyn_bounds_one (void);
# 963
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 967
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 986
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 988
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1008
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1010
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1024
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1033
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
# 1045
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1050
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1054
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1093
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1137
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1139
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1141
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1151
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
if(Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=elt_type;
return 1;}
# 183
return 0;}}else{_LL3: _LL4:
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
({void*_tmp69A=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp69A;});
return 1;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 229
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 235
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp39=({void*_tmp69B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp69B,b);});struct Cyc_Absyn_Exp*eopt=_tmp39;
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
# 304
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp4C=Cyc_Absyn_compress_kb(kb);void*_stmttmp16=_tmp4C;void*_tmp4D=_stmttmp16;struct Cyc_Absyn_Kind*_tmp4E;switch(*((int*)_tmp4D)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp4F=_cycalloc(sizeof(*_tmp4F));_tmp4F->tag=1U,_tmp4F->f1=0;_tmp4F;});case 2U: _LL3: _tmp4E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp4D)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp4E;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=2U,_tmp50->f1=0,_tmp50->f2=k;_tmp50;});}default: _LL5: _LL6:
 return kb;}_LL0:;}
# 311
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->name=tv->name,_tmp51->identity=- 1,({void*_tmp69C=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp51->kind=_tmp69C;});_tmp51;});}
# 314
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9*_tmp52=arg;void*_tmp55;struct Cyc_Absyn_Tqual _tmp54;struct _fat_ptr*_tmp53;_LL1: _tmp53=_tmp52->f1;_tmp54=_tmp52->f2;_tmp55=_tmp52->f3;_LL2: {struct _fat_ptr*x=_tmp53;struct Cyc_Absyn_Tqual y=_tmp54;void*t=_tmp55;
return({struct _tuple9*_tmp56=_cycalloc(sizeof(*_tmp56));_tmp56->f1=x,_tmp56->f2=y,({void*_tmp69D=Cyc_Tcutil_copy_type(t);_tmp56->f3=_tmp69D;});_tmp56;});}}
# 318
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->f1=(*arg).f1,({void*_tmp69E=Cyc_Tcutil_copy_type((*arg).f2);_tmp57->f2=_tmp69E;});_tmp57;});}
# 321
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
return e == 0?0: Cyc_Tcutil_deep_copy_exp(preserve_types,e);}
# 324
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->name=f->name,_tmp58->tq=f->tq,({void*_tmp6A0=Cyc_Tcutil_copy_type(f->type);_tmp58->type=_tmp6A0;}),_tmp58->width=f->width,_tmp58->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp69F=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp58->requires_clause=_tmp69F;});_tmp58;});}struct _tuple15{void*f1;void*f2;};
# 328
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp59=x;void*_tmp5B;void*_tmp5A;_LL1: _tmp5A=_tmp59->f1;_tmp5B=_tmp59->f2;_LL2: {void*r1=_tmp5A;void*r2=_tmp5B;
return({struct _tuple15*_tmp5C=_cycalloc(sizeof(*_tmp5C));({void*_tmp6A2=Cyc_Tcutil_copy_type(r1);_tmp5C->f1=_tmp6A2;}),({void*_tmp6A1=Cyc_Tcutil_copy_type(r2);_tmp5C->f2=_tmp6A1;});_tmp5C;});}}
# 332
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 336
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp5D=Cyc_Absyn_compress(t);void*_stmttmp17=_tmp5D;void*_tmp5E=_stmttmp17;struct Cyc_Absyn_Datatypedecl*_tmp5F;struct Cyc_Absyn_Enumdecl*_tmp60;struct Cyc_Absyn_Aggrdecl*_tmp61;struct Cyc_Absyn_Typedefdecl*_tmp64;struct Cyc_List_List*_tmp63;struct _tuple1*_tmp62;struct Cyc_Absyn_Exp*_tmp65;struct Cyc_Absyn_Exp*_tmp66;struct Cyc_List_List*_tmp68;enum Cyc_Absyn_AggrKind _tmp67;struct Cyc_List_List*_tmp69;struct Cyc_Absyn_Vardecl*_tmp77;struct Cyc_List_List*_tmp76;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp74;struct Cyc_Absyn_Exp*_tmp73;struct Cyc_List_List*_tmp72;struct Cyc_List_List*_tmp71;struct Cyc_Absyn_VarargInfo*_tmp70;int _tmp6F;struct Cyc_List_List*_tmp6E;void*_tmp6D;struct Cyc_Absyn_Tqual _tmp6C;void*_tmp6B;struct Cyc_List_List*_tmp6A;unsigned _tmp7C;void*_tmp7B;struct Cyc_Absyn_Exp*_tmp7A;struct Cyc_Absyn_Tqual _tmp79;void*_tmp78;struct Cyc_Absyn_PtrLoc*_tmp83;void*_tmp82;void*_tmp81;void*_tmp80;void*_tmp7F;struct Cyc_Absyn_Tqual _tmp7E;void*_tmp7D;struct Cyc_Absyn_Tvar*_tmp84;struct Cyc_List_List*_tmp86;void*_tmp85;void*_tmp87;switch(*((int*)_tmp5E)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E)->f2 == 0){_LL1: _tmp87=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E)->f1;_LL2: {void*c=_tmp87;
return t;}}else{_LL3: _tmp85=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E)->f1;_tmp86=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E)->f2;_LL4: {void*c=_tmp85;struct Cyc_List_List*ts=_tmp86;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp88=_cycalloc(sizeof(*_tmp88));_tmp88->tag=0U,_tmp88->f1=c,({struct Cyc_List_List*_tmp6A3=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp88->f2=_tmp6A3;});_tmp88;});}}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp84=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp5E)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp84;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3U: _LL9: _tmp7D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_type;_tmp7E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).elt_tq;_tmp7F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).rgn;_tmp80=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).nullable;_tmp81=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).bounds;_tmp82=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).zero_term;_tmp83=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E)->f1).ptr_atts).ptrloc;_LLA: {void*elt=_tmp7D;struct Cyc_Absyn_Tqual tq=_tmp7E;void*rgn=_tmp7F;void*nbl=_tmp80;void*bs=_tmp81;void*zt=_tmp82;struct Cyc_Absyn_PtrLoc*loc=_tmp83;
# 343
void*_tmp89=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp89;
void*_tmp8A=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp8A;
void*_tmp8B=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp8B;
struct Cyc_Absyn_Tqual _tmp8C=tq;struct Cyc_Absyn_Tqual tq2=_tmp8C;
# 348
void*_tmp8D=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp8D;
void*_tmp8E=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp8E;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp8F=_cycalloc(sizeof(*_tmp8F));_tmp8F->tag=3U,(_tmp8F->f1).elt_type=elt2,(_tmp8F->f1).elt_tq=tq2,((_tmp8F->f1).ptr_atts).rgn=rgn2,((_tmp8F->f1).ptr_atts).nullable=nbl2,((_tmp8F->f1).ptr_atts).bounds=bs2,((_tmp8F->f1).ptr_atts).zero_term=zt2,((_tmp8F->f1).ptr_atts).ptrloc=loc;_tmp8F;});}case 4U: _LLB: _tmp78=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).elt_type;_tmp79=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).tq;_tmp7A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).num_elts;_tmp7B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zero_term;_tmp7C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E)->f1).zt_loc;_LLC: {void*et=_tmp78;struct Cyc_Absyn_Tqual tq=_tmp79;struct Cyc_Absyn_Exp*eo=_tmp7A;void*zt=_tmp7B;unsigned ztl=_tmp7C;
# 352
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=4U,({void*_tmp6A6=Cyc_Tcutil_copy_type(et);(_tmp90->f1).elt_type=_tmp6A6;}),(_tmp90->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp90->f1).num_elts=_tmp6A5;}),({
void*_tmp6A4=Cyc_Tcutil_copy_type(zt);(_tmp90->f1).zero_term=_tmp6A4;}),(_tmp90->f1).zt_loc=ztl;_tmp90;});}case 5U: _LLD: _tmp6A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).tvars;_tmp6B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).effect;_tmp6C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_tqual;_tmp6D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ret_type;_tmp6E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).args;_tmp6F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).c_varargs;_tmp70=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).cyc_varargs;_tmp71=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).rgn_po;_tmp72=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).attributes;_tmp73=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_clause;_tmp74=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).requires_relns;_tmp75=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_clause;_tmp76=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).ensures_relns;_tmp77=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E)->f1).return_value;_LLE: {struct Cyc_List_List*tvs=_tmp6A;void*effopt=_tmp6B;struct Cyc_Absyn_Tqual rt_tq=_tmp6C;void*rt=_tmp6D;struct Cyc_List_List*args=_tmp6E;int c_varargs=_tmp6F;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp70;struct Cyc_List_List*rgn_po=_tmp71;struct Cyc_List_List*atts=_tmp72;struct Cyc_Absyn_Exp*req=_tmp73;struct Cyc_List_List*req_rlns=_tmp74;struct Cyc_Absyn_Exp*ens=_tmp75;struct Cyc_List_List*ens_rlns=_tmp76;struct Cyc_Absyn_Vardecl*ret_val=_tmp77;
# 355
struct Cyc_List_List*_tmp91=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp91;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmp92=Cyc_Tcutil_copy_type(rt);void*rt2=_tmp92;
struct Cyc_List_List*_tmp93=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmp93;
int _tmp94=c_varargs;int c_varargs2=_tmp94;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=cyc_varargs == 0?0:({struct Cyc_Absyn_VarargInfo*_tmp9D=_cycalloc(sizeof(*_tmp9D));*_tmp9D=*cyc_varargs;_tmp9D;});
if(cyc_varargs != 0)
({void*_tmp6A7=Cyc_Tcutil_copy_type(cyc_varargs->type);((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_varargs2))->type=_tmp6A7;});{
struct Cyc_List_List*_tmp95=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmp95;
struct Cyc_List_List*_tmp96=atts;struct Cyc_List_List*atts2=_tmp96;
struct Cyc_Absyn_Exp*_tmp97=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmp97;
struct Cyc_List_List*_tmp98=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmp98;
struct Cyc_Absyn_Exp*_tmp99=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmp99;
struct Cyc_List_List*_tmp9A=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmp9A;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmp9B=_cycalloc(sizeof(*_tmp9B));*_tmp9B=*ret_val;_tmp9B;});
({void*_tmp6A8=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp6A8;});
if((unsigned)ret_val2->rgn)({void*_tmp6A9=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp6A9;});}
# 375
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp9C=_cycalloc(sizeof(*_tmp9C));_tmp9C->tag=5U,(_tmp9C->f1).tvars=tvs2,(_tmp9C->f1).effect=effopt2,(_tmp9C->f1).ret_tqual=rt_tq,(_tmp9C->f1).ret_type=rt2,(_tmp9C->f1).args=args2,(_tmp9C->f1).c_varargs=c_varargs2,(_tmp9C->f1).cyc_varargs=cyc_varargs2,(_tmp9C->f1).rgn_po=rgn_po2,(_tmp9C->f1).attributes=atts2,(_tmp9C->f1).requires_clause=req2,(_tmp9C->f1).requires_relns=req_rlns2,(_tmp9C->f1).ensures_clause=ens2,(_tmp9C->f1).ensures_relns=ens_rlns2,(_tmp9C->f1).return_value=ret_val2;_tmp9C;});}}case 6U: _LLF: _tmp69=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp69;
# 378
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=6U,({struct Cyc_List_List*_tmp6AA=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmp9E->f1=_tmp6AA;});_tmp9E;});}case 7U: _LL11: _tmp67=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f1;_tmp68=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp67;struct Cyc_List_List*fs=_tmp68;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=7U,_tmp9F->f1=k,({struct Cyc_List_List*_tmp6AB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmp9F->f2=_tmp6AB;});_tmp9F;});}case 9U: _LL13: _tmp66=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp5E)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp66;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=9U,_tmpA0->f1=e;_tmpA0;});}case 11U: _LL15: _tmp65=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp5E)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp65;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=11U,_tmpA1->f1=e;_tmpA1;});}case 8U: _LL17: _tmp62=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f1;_tmp63=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f2;_tmp64=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E)->f3;_LL18: {struct _tuple1*tdn=_tmp62;struct Cyc_List_List*ts=_tmp63;struct Cyc_Absyn_Typedefdecl*td=_tmp64;
# 383
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpA2=_cycalloc(sizeof(*_tmpA2));_tmpA2->tag=8U,_tmpA2->f1=tdn,({struct Cyc_List_List*_tmp6AC=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmpA2->f2=_tmp6AC;}),_tmpA2->f3=td,_tmpA2->f4=0;_tmpA2;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)){case 0U: _LL19: _tmp61=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp61;
# 386
struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpA3;
return({union Cyc_Absyn_AggrInfo _tmp6AD=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp6AD,targs);});}case 1U: _LL1B: _tmp60=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp60;
# 389
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp5F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp5E)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp5F;
# 391
struct Cyc_List_List*_tmpA4=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpA4;
return({union Cyc_Absyn_DatatypeInfo _tmp6AE=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp612;_tmp612.name=dd->name,_tmp612.is_extensible=0;_tmp612;}));Cyc_Absyn_datatype_type(_tmp6AE,targs);});}}}_LL0:;}
# 397
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpA5=d;struct _fat_ptr*_tmpA6;struct Cyc_Absyn_Exp*_tmpA7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpA5)->tag == 0U){_LL1: _tmpA7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpA5)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpA7;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=0U,({struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpA8->f1=_tmp6AF;});_tmpA8;});}}else{_LL3: _tmpA6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpA5)->f1;_LL4: {struct _fat_ptr*v=_tmpA6;
return d;}}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 403
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 405
return({struct _tuple16*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct Cyc_List_List*_tmp6B1=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpA9->f1=_tmp6B1;}),({
struct Cyc_Absyn_Exp*_tmp6B0=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpA9->f2=_tmp6B0;});_tmpA9;});}
# 409
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*(*_tmpAA)(int preserve_types,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_deep_copy_exp;struct Cyc_Absyn_Exp*(*DC)(int preserve_types,struct Cyc_Absyn_Exp*e)=_tmpAA;
struct Cyc_Absyn_Exp*new_e;
int _tmpAB=preserve_types;int pt=_tmpAB;
{void*_tmpAC=e->r;void*_stmttmp18=_tmpAC;void*_tmpAD=_stmttmp18;struct Cyc_Absyn_Exp*_tmpAE;struct Cyc_List_List*_tmpB3;struct Cyc_List_List*_tmpB2;struct Cyc_List_List*_tmpB1;struct _fat_ptr _tmpB0;int _tmpAF;void*_tmpB4;struct _fat_ptr*_tmpB6;struct Cyc_Absyn_Exp*_tmpB5;struct Cyc_List_List*_tmpB8;struct Cyc_Core_Opt*_tmpB7;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_Absyn_Exp*_tmpB9;int _tmpC0;int _tmpBF;struct Cyc_Absyn_Exp*_tmpBE;void**_tmpBD;struct Cyc_Absyn_Exp*_tmpBC;int _tmpBB;struct Cyc_Absyn_Enumfield*_tmpC2;void*_tmpC1;struct Cyc_Absyn_Enumfield*_tmpC4;struct Cyc_Absyn_Enumdecl*_tmpC3;struct Cyc_List_List*_tmpC8;void*_tmpC7;struct Cyc_Absyn_Tqual _tmpC6;struct _fat_ptr*_tmpC5;struct Cyc_List_List*_tmpCA;void*_tmpC9;struct Cyc_Absyn_Aggrdecl*_tmpCE;struct Cyc_List_List*_tmpCD;struct Cyc_List_List*_tmpCC;struct _tuple1*_tmpCB;int _tmpD1;void*_tmpD0;struct Cyc_Absyn_Exp*_tmpCF;int _tmpD5;struct Cyc_Absyn_Exp*_tmpD4;struct Cyc_Absyn_Exp*_tmpD3;struct Cyc_Absyn_Vardecl*_tmpD2;struct Cyc_Absyn_Datatypefield*_tmpD8;struct Cyc_Absyn_Datatypedecl*_tmpD7;struct Cyc_List_List*_tmpD6;struct Cyc_List_List*_tmpD9;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;int _tmpDF;int _tmpDE;struct _fat_ptr*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;int _tmpE3;int _tmpE2;struct _fat_ptr*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_List_List*_tmpE4;struct Cyc_Absyn_Exp*_tmpE5;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_List_List*_tmpE8;void*_tmpE7;struct Cyc_Absyn_Exp*_tmpE9;void*_tmpEA;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;enum Cyc_Absyn_Coercion _tmpF1;int _tmpF0;struct Cyc_Absyn_Exp*_tmpEF;void*_tmpEE;struct Cyc_List_List*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_Absyn_Exp*_tmpF4;int _tmpF6;struct Cyc_Absyn_Exp*_tmpF5;int _tmpFA;struct Cyc_Absyn_VarargCallInfo*_tmpF9;struct Cyc_List_List*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;int _tmp103;int _tmp102;void*_tmp101;struct Cyc_Absyn_Tqual _tmp100;struct _fat_ptr*_tmpFF;struct Cyc_List_List*_tmpFE;int _tmpFD;struct Cyc_List_List*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Core_Opt*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp10A;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp10E;enum Cyc_Absyn_Incrementor _tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_List_List*_tmp113;enum Cyc_Absyn_Primop _tmp112;struct _fat_ptr _tmp114;void*_tmp115;union Cyc_Absyn_Cnst _tmp116;switch(*((int*)_tmpAD)){case 0U: _LL1: _tmp116=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp116;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp115=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL4: {void*b=_tmp115;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp114=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL6: {struct _fat_ptr p=_tmp114;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp112=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmp113=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp112;struct Cyc_List_List*es=_tmp113;
new_e=({enum Cyc_Absyn_Primop _tmp6B3=p;struct Cyc_List_List*_tmp6B2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_primop_exp(_tmp6B3,_tmp6B2,e->loc);});goto _LL0;}case 5U: _LL9: _tmp110=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmp111=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp110;enum Cyc_Absyn_Incrementor i=_tmp111;
new_e=({struct Cyc_Absyn_Exp*_tmp6B5=DC(pt,e1);enum Cyc_Absyn_Incrementor _tmp6B4=i;Cyc_Absyn_increment_exp(_tmp6B5,_tmp6B4,e->loc);});goto _LL0;}case 7U: _LLB: _tmp10E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmp10F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp10E;struct Cyc_Absyn_Exp*e2=_tmp10F;
new_e=({struct Cyc_Absyn_Exp*_tmp6B7=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6B6=DC(pt,e2);Cyc_Absyn_and_exp(_tmp6B7,_tmp6B6,e->loc);});goto _LL0;}case 8U: _LLD: _tmp10C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmp10D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp10C;struct Cyc_Absyn_Exp*e2=_tmp10D;
new_e=({struct Cyc_Absyn_Exp*_tmp6B9=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6B8=DC(pt,e2);Cyc_Absyn_or_exp(_tmp6B9,_tmp6B8,e->loc);});goto _LL0;}case 9U: _LLF: _tmp10A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmp10B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp10A;struct Cyc_Absyn_Exp*e2=_tmp10B;
new_e=({struct Cyc_Absyn_Exp*_tmp6BB=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6BA=DC(pt,e2);Cyc_Absyn_seq_exp(_tmp6BB,_tmp6BA,e->loc);});goto _LL0;}case 6U: _LL11: _tmp107=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmp108=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmp109=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp107;struct Cyc_Absyn_Exp*e2=_tmp108;struct Cyc_Absyn_Exp*e3=_tmp109;
# 423
new_e=({struct Cyc_Absyn_Exp*_tmp6BE=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6BD=DC(pt,e2);struct Cyc_Absyn_Exp*_tmp6BC=DC(pt,e3);Cyc_Absyn_conditional_exp(_tmp6BE,_tmp6BD,_tmp6BC,e->loc);});goto _LL0;}case 4U: _LL13: _tmp104=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmp105=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmp106=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp104;struct Cyc_Core_Opt*po=_tmp105;struct Cyc_Absyn_Exp*e2=_tmp106;
# 425
new_e=({struct Cyc_Absyn_Exp*_tmp6C1=DC(pt,e1);struct Cyc_Core_Opt*_tmp6C0=(unsigned)po?({struct Cyc_Core_Opt*_tmp117=_cycalloc(sizeof(*_tmp117));_tmp117->v=(void*)po->v;_tmp117;}): 0;struct Cyc_Absyn_Exp*_tmp6BF=DC(pt,e2);Cyc_Absyn_assignop_exp(_tmp6C1,_tmp6C0,_tmp6BF,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f3 != 0){_LL15: _tmpFB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpFC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpFD=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f3)->num_varargs;_tmpFE=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f3)->injectors;_tmpFF=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f3)->vai)->name;_tmp100=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f3)->vai)->tq;_tmp101=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f3)->vai)->type;_tmp102=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f3)->vai)->inject;_tmp103=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmpFB;struct Cyc_List_List*es=_tmpFC;int n=_tmpFD;struct Cyc_List_List*is=_tmpFE;struct _fat_ptr*nm=_tmpFF;struct Cyc_Absyn_Tqual tq=_tmp100;void*t=_tmp101;int i=_tmp102;int resolved=_tmp103;
# 428
new_e=({void*_tmp6C7=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp11A=_cycalloc(sizeof(*_tmp11A));_tmp11A->tag=10U,({
struct Cyc_Absyn_Exp*_tmp6C6=DC(pt,e1);_tmp11A->f1=_tmp6C6;}),({struct Cyc_List_List*_tmp6C5=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp11A->f2=_tmp6C5;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp6C4=({struct Cyc_Absyn_VarargCallInfo*_tmp119=_cycalloc(sizeof(*_tmp119));_tmp119->num_varargs=n,_tmp119->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp6C3=({struct Cyc_Absyn_VarargInfo*_tmp118=_cycalloc(sizeof(*_tmp118));_tmp118->name=nm,_tmp118->tq=tq,({void*_tmp6C2=Cyc_Tcutil_copy_type(t);_tmp118->type=_tmp6C2;}),_tmp118->inject=i;_tmp118;});_tmp119->vai=_tmp6C3;});_tmp119;});
# 430
_tmp11A->f3=_tmp6C4;}),_tmp11A->f4=resolved;_tmp11A;});
# 428
Cyc_Absyn_new_exp(_tmp6C7,e->loc);});
# 433
goto _LL0;}}else{_LL17: _tmpF7=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpF8=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpF9=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_tmpFA=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpAD)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpF7;struct Cyc_List_List*es=_tmpF8;struct Cyc_Absyn_VarargCallInfo*vci=_tmpF9;int resolved=_tmpFA;
# 435
new_e=({void*_tmp6CA=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp11B=_cycalloc(sizeof(*_tmp11B));_tmp11B->tag=10U,({struct Cyc_Absyn_Exp*_tmp6C9=DC(pt,e1);_tmp11B->f1=_tmp6C9;}),({struct Cyc_List_List*_tmp6C8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp11B->f2=_tmp6C8;}),_tmp11B->f3=vci,_tmp11B->f4=resolved;_tmp11B;});Cyc_Absyn_new_exp(_tmp6CA,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmpF5=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpF6=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpF5;int b=_tmpF6;
# 438
new_e=b?({struct Cyc_Absyn_Exp*_tmp6CC=DC(pt,e1);Cyc_Absyn_rethrow_exp(_tmp6CC,e->loc);}):({struct Cyc_Absyn_Exp*_tmp6CB=DC(pt,e1);Cyc_Absyn_throw_exp(_tmp6CB,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmpF4=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpF4;
# 441
new_e=({struct Cyc_Absyn_Exp*_tmp6CD=DC(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp6CD,e->loc);});goto _LL0;}case 13U: _LL1D: _tmpF2=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpF3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpF2;struct Cyc_List_List*ts=_tmpF3;
# 443
new_e=({struct Cyc_Absyn_Exp*_tmp6CF=DC(pt,e1);struct Cyc_List_List*_tmp6CE=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp6CF,_tmp6CE,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpEE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpEF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpF0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_tmpF1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpAD)->f4;_LL20: {void*t=_tmpEE;struct Cyc_Absyn_Exp*e1=_tmpEF;int b=_tmpF0;enum Cyc_Absyn_Coercion c=_tmpF1;
# 445
new_e=({void*_tmp6D3=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp6D2=DC(pt,e1);int _tmp6D1=b;enum Cyc_Absyn_Coercion _tmp6D0=c;Cyc_Absyn_cast_exp(_tmp6D3,_tmp6D2,_tmp6D1,_tmp6D0,e->loc);});goto _LL0;}case 16U: _LL21: _tmpEC=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpED=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpEC;struct Cyc_Absyn_Exp*e1=_tmpED;
# 447
new_e=({struct Cyc_Absyn_Exp*_tmp6D5=(unsigned)eo?DC(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp6D4=DC(pt,e1);Cyc_Absyn_New_exp(_tmp6D5,_tmp6D4,e->loc);});goto _LL0;}case 15U: _LL23: _tmpEB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpEB;
new_e=({struct Cyc_Absyn_Exp*_tmp6D6=DC(pt,e1);Cyc_Absyn_address_exp(_tmp6D6,e->loc);});goto _LL0;}case 17U: _LL25: _tmpEA=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL26: {void*t=_tmpEA;
new_e=({void*_tmp6D7=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp6D7,e->loc);});goto _LL0;}case 18U: _LL27: _tmpE9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpE9;
new_e=({struct Cyc_Absyn_Exp*_tmp6D8=DC(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp6D8,e->loc);});goto _LL0;}case 19U: _LL29: _tmpE7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpE8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL2A: {void*t=_tmpE7;struct Cyc_List_List*ofs=_tmpE8;
new_e=({void*_tmp6DA=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp6D9=ofs;Cyc_Absyn_offsetof_exp(_tmp6DA,_tmp6D9,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpE6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpE6;
new_e=({struct Cyc_Absyn_Exp*_tmp6DB=DC(pt,e1);Cyc_Absyn_deref_exp(_tmp6DB,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpE5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpE5;
new_e=({struct Cyc_Absyn_Exp*_tmp6DC=DC(pt,e1);Cyc_Absyn_extension_exp(_tmp6DC,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpE4=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL30: {struct Cyc_List_List*es=_tmpE4;
new_e=({struct Cyc_List_List*_tmp6DD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_tuple_exp(_tmp6DD,e->loc);});goto _LL0;}case 21U: _LL31: _tmpE0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpE1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpE2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_tmpE3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpAD)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpE0;struct _fat_ptr*n=_tmpE1;int f1=_tmpE2;int f2=_tmpE3;
# 456
new_e=({void*_tmp6DF=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp11C=_cycalloc(sizeof(*_tmp11C));_tmp11C->tag=21U,({struct Cyc_Absyn_Exp*_tmp6DE=DC(pt,e1);_tmp11C->f1=_tmp6DE;}),_tmp11C->f2=n,_tmp11C->f3=f1,_tmp11C->f4=f2;_tmp11C;});Cyc_Absyn_new_exp(_tmp6DF,e->loc);});goto _LL0;}case 22U: _LL33: _tmpDC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpDD=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpDE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_tmpDF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpAD)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpDC;struct _fat_ptr*n=_tmpDD;int f1=_tmpDE;int f2=_tmpDF;
# 458
new_e=({void*_tmp6E1=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp11D=_cycalloc(sizeof(*_tmp11D));_tmp11D->tag=22U,({struct Cyc_Absyn_Exp*_tmp6E0=DC(pt,e1);_tmp11D->f1=_tmp6E0;}),_tmp11D->f2=n,_tmp11D->f3=f1,_tmp11D->f4=f2;_tmp11D;});Cyc_Absyn_new_exp(_tmp6E1,e->loc);});goto _LL0;}case 23U: _LL35: _tmpDA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpDB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpDA;struct Cyc_Absyn_Exp*e2=_tmpDB;
# 460
new_e=({struct Cyc_Absyn_Exp*_tmp6E3=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6E2=DC(pt,e2);Cyc_Absyn_subscript_exp(_tmp6E3,_tmp6E2,e->loc);});goto _LL0;}case 26U: _LL37: _tmpD9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL38: {struct Cyc_List_List*eds=_tmpD9;
# 462
new_e=({void*_tmp6E5=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp11E=_cycalloc(sizeof(*_tmp11E));_tmp11E->tag=26U,({struct Cyc_List_List*_tmp6E4=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp11E->f1=_tmp6E4;});_tmp11E;});Cyc_Absyn_new_exp(_tmp6E5,e->loc);});goto _LL0;}case 31U: _LL39: _tmpD6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpD7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpD8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_LL3A: {struct Cyc_List_List*es=_tmpD6;struct Cyc_Absyn_Datatypedecl*dtd=_tmpD7;struct Cyc_Absyn_Datatypefield*dtf=_tmpD8;
# 464
new_e=({void*_tmp6E7=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->tag=31U,({struct Cyc_List_List*_tmp6E6=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp11F->f1=_tmp6E6;}),_tmp11F->f2=dtd,_tmp11F->f3=dtf;_tmp11F;});Cyc_Absyn_new_exp(_tmp6E7,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpD2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpD3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpD4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_tmpD5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpAD)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpD2;struct Cyc_Absyn_Exp*e1=_tmpD3;struct Cyc_Absyn_Exp*e2=_tmpD4;int b=_tmpD5;
# 466
new_e=({void*_tmp6EA=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->tag=27U,_tmp120->f1=vd,({struct Cyc_Absyn_Exp*_tmp6E9=DC(pt,e1);_tmp120->f2=_tmp6E9;}),({struct Cyc_Absyn_Exp*_tmp6E8=DC(pt,e2);_tmp120->f3=_tmp6E8;}),_tmp120->f4=b;_tmp120;});Cyc_Absyn_new_exp(_tmp6EA,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpCF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpD0=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpD1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpCF;void*t=_tmpD0;int b=_tmpD1;
# 468
new_e=({void*_tmp6ED=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->tag=28U,({struct Cyc_Absyn_Exp*_tmp6EC=DC(pt,e);_tmp121->f1=_tmp6EC;}),({void*_tmp6EB=Cyc_Tcutil_copy_type(t);_tmp121->f2=_tmp6EB;}),_tmp121->f3=b;_tmp121;});Cyc_Absyn_new_exp(_tmp6ED,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpCB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpCC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpCD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_tmpCE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpAD)->f4;_LL40: {struct _tuple1*n=_tmpCB;struct Cyc_List_List*ts=_tmpCC;struct Cyc_List_List*eds=_tmpCD;struct Cyc_Absyn_Aggrdecl*agr=_tmpCE;
# 471
new_e=({void*_tmp6F0=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=29U,_tmp122->f1=n,({struct Cyc_List_List*_tmp6EF=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp122->f2=_tmp6EF;}),({struct Cyc_List_List*_tmp6EE=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp122->f3=_tmp6EE;}),_tmp122->f4=agr;_tmp122;});Cyc_Absyn_new_exp(_tmp6F0,e->loc);});
# 473
goto _LL0;}case 30U: _LL41: _tmpC9=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpCA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL42: {void*t=_tmpC9;struct Cyc_List_List*eds=_tmpCA;
# 475
new_e=({void*_tmp6F3=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=30U,({void*_tmp6F2=Cyc_Tcutil_copy_type(t);_tmp123->f1=_tmp6F2;}),({struct Cyc_List_List*_tmp6F1=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp123->f2=_tmp6F1;});_tmp123;});Cyc_Absyn_new_exp(_tmp6F3,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpC5=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAD)->f1)->f1;_tmpC6=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAD)->f1)->f2;_tmpC7=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAD)->f1)->f3;_tmpC8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL44: {struct _fat_ptr*vopt=_tmpC5;struct Cyc_Absyn_Tqual tq=_tmpC6;void*t=_tmpC7;struct Cyc_List_List*eds=_tmpC8;
# 478
new_e=({void*_tmp6F7=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=25U,({struct _tuple9*_tmp6F6=({struct _tuple9*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->f1=vopt,_tmp124->f2=tq,({void*_tmp6F5=Cyc_Tcutil_copy_type(t);_tmp124->f3=_tmp6F5;});_tmp124;});_tmp125->f1=_tmp6F6;}),({
struct Cyc_List_List*_tmp6F4=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp125->f2=_tmp6F4;});_tmp125;});
# 478
Cyc_Absyn_new_exp(_tmp6F7,e->loc);});
# 480
goto _LL0;}case 32U: _LL45: _tmpC3=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpC4=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpC3;struct Cyc_Absyn_Enumfield*ef=_tmpC4;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpC1=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpC2=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL48: {void*t=_tmpC1;struct Cyc_Absyn_Enumfield*ef=_tmpC2;
# 483
new_e=({void*_tmp6F9=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=33U,({void*_tmp6F8=Cyc_Tcutil_copy_type(t);_tmp126->f1=_tmp6F8;}),_tmp126->f2=ef;_tmp126;});Cyc_Absyn_new_exp(_tmp6F9,e->loc);});goto _LL0;}case 34U: _LL49: _tmpBB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAD)->f1).is_calloc;_tmpBC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAD)->f1).rgn;_tmpBD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAD)->f1).elt_type;_tmpBE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAD)->f1).num_elts;_tmpBF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAD)->f1).fat_result;_tmpC0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpAD)->f1).inline_call;_LL4A: {int ic=_tmpBB;struct Cyc_Absyn_Exp*r=_tmpBC;void**t=_tmpBD;struct Cyc_Absyn_Exp*n=_tmpBE;int res=_tmpBF;int inlc=_tmpC0;
# 485
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=DC(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp127=_cycalloc(sizeof(*_tmp127));({void*_tmp6FA=Cyc_Tcutil_copy_type(*t);*_tmp127=_tmp6FA;});_tmp127;});
({void*_tmp6FB=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=34U,(_tmp128->f1).is_calloc=ic,(_tmp128->f1).rgn=r1,(_tmp128->f1).elt_type=t1,(_tmp128->f1).num_elts=n,(_tmp128->f1).fat_result=res,(_tmp128->f1).inline_call=inlc;_tmp128;});e2->r=_tmp6FB;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpB9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpBA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpB9;struct Cyc_Absyn_Exp*e2=_tmpBA;
new_e=({struct Cyc_Absyn_Exp*_tmp6FD=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6FC=DC(pt,e2);Cyc_Absyn_swap_exp(_tmp6FD,_tmp6FC,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpB7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpB8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpB7;struct Cyc_List_List*eds=_tmpB8;
# 493
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->v=(struct _tuple1*)nopt->v;_tmp129;});
new_e=({void*_tmp6FF=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=36U,_tmp12A->f1=nopt1,({struct Cyc_List_List*_tmp6FE=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12A->f2=_tmp6FE;});_tmp12A;});Cyc_Absyn_new_exp(_tmp6FF,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 498
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp700=({const char*_tmp12B="deep_copy: statement expressions unsupported";_tag_fat(_tmp12B,sizeof(char),45U);});_tmp12C->f1=_tmp700;});_tmp12C;}));case 38U: _LL51: _tmpB5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpB6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpB5;struct _fat_ptr*fn=_tmpB6;
# 500
new_e=({void*_tmp702=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=38U,({struct Cyc_Absyn_Exp*_tmp701=DC(pt,e1);_tmp12D->f1=_tmp701;}),_tmp12D->f2=fn;_tmp12D;});Cyc_Absyn_new_exp(_tmp702,e->loc);});goto _LL0;}case 39U: _LL53: _tmpB4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL54: {void*t=_tmpB4;
new_e=({void*_tmp703=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp703,e->loc);});goto _LL0;}case 40U: _LL55: _tmpAF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_tmpB0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAD)->f2;_tmpB1=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAD)->f3;_tmpB2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAD)->f4;_tmpB3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpAD)->f5;_LL56: {int v=_tmpAF;struct _fat_ptr t=_tmpB0;struct Cyc_List_List*o=_tmpB1;struct Cyc_List_List*i=_tmpB2;struct Cyc_List_List*c=_tmpB3;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _LL57: _tmpAE=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpAD)->f1;_LL58: {struct Cyc_Absyn_Exp*e1=_tmpAE;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 506
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 510
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 521 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp12E=ka1;enum Cyc_Absyn_AliasQual _tmp130;enum Cyc_Absyn_KindQual _tmp12F;_LL1: _tmp12F=_tmp12E->kind;_tmp130=_tmp12E->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp12F;enum Cyc_Absyn_AliasQual a1=_tmp130;
struct Cyc_Absyn_Kind*_tmp131=ka2;enum Cyc_Absyn_AliasQual _tmp133;enum Cyc_Absyn_KindQual _tmp132;_LL4: _tmp132=_tmp131->kind;_tmp133=_tmp131->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp132;enum Cyc_Absyn_AliasQual a2=_tmp133;
# 525
if((int)k1 != (int)k2){
struct _tuple17 _tmp134=({struct _tuple17 _tmp613;_tmp613.f1=k1,_tmp613.f2=k2;_tmp613;});struct _tuple17 _stmttmp19=_tmp134;struct _tuple17 _tmp135=_stmttmp19;switch(_tmp135.f1){case Cyc_Absyn_BoxKind: switch(_tmp135.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp135.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 533
if((int)a1 != (int)a2){
struct _tuple18 _tmp136=({struct _tuple18 _tmp614;_tmp614.f1=a1,_tmp614.f2=a2;_tmp614;});struct _tuple18 _stmttmp1A=_tmp136;struct _tuple18 _tmp137=_stmttmp1A;switch(_tmp137.f1){case Cyc_Absyn_Aliasable: if(_tmp137.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp137.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 539
return 1;}}}
# 542
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp138=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1B=_tmp138;void*_tmp139=_stmttmp1B;struct Cyc_Absyn_Kind*_tmp13A;struct Cyc_Absyn_Kind*_tmp13B;switch(*((int*)_tmp139)){case 0U: _LL1: _tmp13B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp139)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp13B;
return k;}case 2U: _LL3: _tmp13A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp139)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp13A;
return k;}default: _LL5: _LL6:
({void*_tmp704=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=2U,_tmp13C->f1=0,_tmp13C->f2=def;_tmp13C;});tv->kind=_tmp704;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 550
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp13D=Cyc_Absyn_compress(t);void*_stmttmp1C=_tmp13D;void*_tmp13E=_stmttmp1C;struct Cyc_Absyn_Tvar*_tmp13F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13E)->tag == 2U){_LL1: _tmp13F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp13E)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp13F;
# 553
void*_tmp140=tv->kind;void*oldkb=_tmp140;
tv->kind=kb;
return({struct _tuple19 _tmp615;_tmp615.f1=tv,_tmp615.f2=oldkb;_tmp615;});}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp142=({struct Cyc_Warn_String_Warn_Warg_struct _tmp617;_tmp617.tag=0U,({struct _fat_ptr _tmp705=({const char*_tmp144="swap_kind: cannot update the kind of ";_tag_fat(_tmp144,sizeof(char),38U);});_tmp617.f1=_tmp705;});_tmp617;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp143=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp616;_tmp616.tag=2U,_tmp616.f1=(void*)t;_tmp616;});void*_tmp141[2U];_tmp141[0]=& _tmp142,_tmp141[1]=& _tmp143;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp141,sizeof(void*),2U));});}_LL0:;}
# 562
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 564
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 568
struct _RegionHandle _tmp145=_new_region("temp");struct _RegionHandle*temp=& _tmp145;_push_region(temp);
{struct Cyc_List_List*_tmp146=0;struct Cyc_List_List*inst=_tmp146;
# 571
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp147=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp147;
void*_tmp148=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp148;
struct Cyc_Absyn_Kind*_tmp149=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp1D=_tmp149;struct Cyc_Absyn_Kind*_tmp14A=_stmttmp1D;switch(((struct Cyc_Absyn_Kind*)_tmp14A)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 577
 inst=({struct Cyc_List_List*_tmp14C=_region_malloc(temp,sizeof(*_tmp14C));({struct _tuple19*_tmp706=({struct _tuple19*_tmp14B=_region_malloc(temp,sizeof(*_tmp14B));_tmp14B->f1=tv,_tmp14B->f2=t;_tmp14B;});_tmp14C->hd=_tmp706;}),_tmp14C->tl=inst;_tmp14C;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 581
if(inst != 0){
field_type=({struct _RegionHandle*_tmp708=temp;struct Cyc_List_List*_tmp707=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp708,_tmp707,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 569
;_pop_region();}
# 586
return k;}
# 593
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 595
void*_tmp14D=Cyc_Absyn_compress(t);void*_stmttmp1E=_tmp14D;void*_tmp14E=_stmttmp1E;struct Cyc_Absyn_Typedefdecl*_tmp14F;struct Cyc_Absyn_Exp*_tmp150;struct Cyc_Absyn_PtrInfo _tmp151;struct Cyc_List_List*_tmp153;void*_tmp152;struct Cyc_Absyn_Tvar*_tmp154;struct Cyc_Core_Opt*_tmp155;switch(*((int*)_tmp14E)){case 1U: _LL1: _tmp155=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp14E)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp155;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp154=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp154;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp152=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f1;_tmp153=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp14E)->f2;_LL6: {void*c=_tmp152;struct Cyc_List_List*ts=_tmp153;
# 599
void*_tmp156=c;int _tmp15A;struct Cyc_Absyn_AggrdeclImpl*_tmp159;struct Cyc_List_List*_tmp158;enum Cyc_Absyn_AggrKind _tmp157;struct Cyc_Absyn_Kind*_tmp15B;enum Cyc_Absyn_Size_of _tmp15C;switch(*((int*)_tmp156)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp15C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp156)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp15C;
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp15B=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp156)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp15B;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 621
({struct Cyc_Warn_String_Warn_Warg_struct _tmp15E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp618;_tmp618.tag=0U,({struct _fat_ptr _tmp709=({const char*_tmp15F="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp15F,sizeof(char),40U);});_tmp618.f1=_tmp709;});_tmp618;});void*_tmp15D[1U];_tmp15D[0]=& _tmp15E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp15D,sizeof(void*),1U));});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 624
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp157=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->kind;_tmp158=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->tvs;_tmp159=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->impl;_tmp15A=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp156)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp157;struct Cyc_List_List*tvs=_tmp158;struct Cyc_Absyn_AggrdeclImpl*i=_tmp159;int expected_mem_kind=_tmp15A;
# 626
if(i == 0)
return expected_mem_kind?& Cyc_Tcutil_mk:& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp160=i->fields;struct Cyc_List_List*fields=_tmp160;
if(fields == 0)return& Cyc_Tcutil_mk;
# 631
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){
;}{
void*_tmp161=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp161;
struct Cyc_Absyn_Kind*_tmp162=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp162;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 640
for(0;fields != 0;fields=fields->tl){
void*_tmp163=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp163;
struct Cyc_Absyn_Kind*_tmp164=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp164;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 645
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 647
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp151=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp14E)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp151;
# 650
void*_tmp165=Cyc_Absyn_compress((pinfo.ptr_atts).bounds);void*_stmttmp1F=_tmp165;void*_tmp166=_stmttmp1F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp166)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp166)->f1)){case 13U: _LL4D: _LL4E: {
# 652
enum Cyc_Absyn_AliasQual _tmp167=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp20=_tmp167;enum Cyc_Absyn_AliasQual _tmp168=_stmttmp20;switch(_tmp168){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 658
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 660
enum Cyc_Absyn_AliasQual _tmp169=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp21=_tmp169;enum Cyc_Absyn_AliasQual _tmp16A=_stmttmp21;switch(_tmp16A){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 666
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 670
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp150=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp14E)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp150;
# 672
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp14F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp14E)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp14F;
# 676
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp70A=({const char*_tmp16E="type_kind: typedef found: ";_tag_fat(_tmp16E,sizeof(char),27U);});_tmp61A.f1=_tmp70A;});_tmp61A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp16D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp619;_tmp619.tag=2U,_tmp619.f1=(void*)t;_tmp619;});void*_tmp16B[2U];_tmp16B[0]=& _tmp16C,_tmp16B[1]=& _tmp16D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp16B,sizeof(void*),2U));});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp14E)->f1)->r)){case 1U: _LL17: _LL18:
 return& Cyc_Tcutil_bk;case 0U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 685
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 690
static void*Cyc_Tcutil_rgns_of(void*t);
# 692
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 696
static struct _tuple19*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp16F=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp22=_tmp16F;struct Cyc_Absyn_Kind*_tmp170=_stmttmp22;switch(((struct Cyc_Absyn_Kind*)_tmp170)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp170)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp171=_cycalloc(sizeof(*_tmp171));_tmp171->tag=9U,({struct Cyc_Absyn_Exp*_tmp70B=Cyc_Absyn_uint_exp(0U,0U);_tmp171->f1=_tmp70B;});_tmp171;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 707
return({struct _tuple19*_tmp172=_cycalloc(sizeof(*_tmp172));_tmp172->f1=tv,_tmp172->f2=t;_tmp172;});}
# 714
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp173=Cyc_Absyn_compress(t);void*_stmttmp23=_tmp173;void*_tmp174=_stmttmp23;struct Cyc_List_List*_tmp175;struct Cyc_List_List*_tmp176;struct Cyc_List_List*_tmp17D;struct Cyc_Absyn_VarargInfo*_tmp17C;struct Cyc_List_List*_tmp17B;void*_tmp17A;struct Cyc_Absyn_Tqual _tmp179;void*_tmp178;struct Cyc_List_List*_tmp177;struct Cyc_List_List*_tmp17E;void*_tmp17F;void*_tmp181;void*_tmp180;struct Cyc_List_List*_tmp182;switch(*((int*)_tmp174)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp182=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f2;_LL6: {struct Cyc_List_List*ts=_tmp182;
# 719
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 723
struct Cyc_Absyn_Kind*_tmp183=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp24=_tmp183;struct Cyc_Absyn_Kind*_tmp184=_stmttmp24;switch(((struct Cyc_Absyn_Kind*)_tmp184)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp180=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->f1).elt_type;_tmp181=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp174)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp180;void*r=_tmp181;
# 731
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp185[2U];({void*_tmp70D=Cyc_Absyn_access_eff(r);_tmp185[0]=_tmp70D;}),({void*_tmp70C=Cyc_Tcutil_rgns_of(et);_tmp185[1]=_tmp70C;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp185,sizeof(void*),2U));})));}case 4U: _LLD: _tmp17F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp174)->f1).elt_type;_LLE: {void*et=_tmp17F;
# 733
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp17E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp174)->f2;_LL10: {struct Cyc_List_List*fs=_tmp17E;
# 735
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp177=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp174)->f1).tvars;_tmp178=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp174)->f1).effect;_tmp179=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp174)->f1).ret_tqual;_tmp17A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp174)->f1).ret_type;_tmp17B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp174)->f1).args;_tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp174)->f1).cyc_varargs;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp174)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp177;void*eff=_tmp178;struct Cyc_Absyn_Tqual rt_tq=_tmp179;void*rt=_tmp17A;struct Cyc_List_List*args=_tmp17B;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp17C;struct Cyc_List_List*rpo=_tmp17D;
# 744
void*_tmp186=({struct Cyc_List_List*_tmp70E=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp70E,(void*)_check_null(eff));});void*e=_tmp186;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp176=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp174)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp176;
# 747
struct Cyc_List_List*_tmp187=0;struct Cyc_List_List*ts=_tmp187;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp188=_cycalloc(sizeof(*_tmp188));_tmp188->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp188->tl=ts;_tmp188;});}
_tmp175=ts;goto _LL16;}case 8U: _LL15: _tmp175=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp174)->f2;_LL16: {struct Cyc_List_List*ts=_tmp175;
# 752
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp18A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61B;_tmp61B.tag=0U,({struct _fat_ptr _tmp70F=({const char*_tmp18B="typedecl in rgns_of";_tag_fat(_tmp18B,sizeof(char),20U);});_tmp61B.f1=_tmp70F;});_tmp61B;});void*_tmp189[1U];_tmp189[0]=& _tmp18A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp189,sizeof(void*),1U));});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 762
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp18C=e;void*_tmp18D;struct Cyc_List_List**_tmp18E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f1)){case 9U: _LL1: _tmp18E=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f2;_LL2: {struct Cyc_List_List**es=_tmp18E;
# 766
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp18F=(void*)effs->hd;void*eff=_tmp18F;
({void*_tmp710=(void*)Cyc_Absyn_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp710;});{
void*_tmp190=(void*)effs->hd;void*_stmttmp25=_tmp190;void*_tmp191=_stmttmp25;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp191)->f2)->tl == 0){_LLE: _LLF:
# 775
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 779
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp192=Cyc_Absyn_compress((void*)effs->hd);void*_stmttmp26=_tmp192;void*_tmp193=_stmttmp26;void*_tmp194;struct Cyc_List_List*_tmp195;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f1)){case 9U: _LL13: _tmp195=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp195;
# 784
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);
goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp193)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp194=_tmp193;_LL1C: {void*e=_tmp194;
effects=({struct Cyc_List_List*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->hd=e,_tmp196->tl=effects;_tmp196;});goto _LL12;}}_LL12:;}}
# 792
({struct Cyc_List_List*_tmp711=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp711;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f2 != 0){_LL3: _tmp18D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18C)->f2)->hd;_LL4: {void*t=_tmp18D;
# 795
void*_tmp197=Cyc_Absyn_compress(t);void*_stmttmp27=_tmp197;void*_tmp198=_stmttmp27;switch(*((int*)_tmp198)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 800
 return e;}_LL0:;}}
# 805
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp199=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->tag=5U,(_tmp19A->f1).tvars=0,(_tmp19A->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp712=Cyc_Absyn_empty_tqual(0U);(_tmp19A->f1).ret_tqual=_tmp712;}),(_tmp19A->f1).ret_type=Cyc_Absyn_void_type,(_tmp19A->f1).args=0,(_tmp19A->f1).c_varargs=0,(_tmp19A->f1).cyc_varargs=0,(_tmp19A->f1).rgn_po=0,(_tmp19A->f1).attributes=0,(_tmp19A->f1).requires_clause=0,(_tmp19A->f1).requires_relns=0,(_tmp19A->f1).ensures_clause=0,(_tmp19A->f1).ensures_relns=0,(_tmp19A->f1).return_value=0;_tmp19A;});
# 806
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp199;
# 816
return({void*_tmp716=(void*)fntype;void*_tmp715=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp714=Cyc_Absyn_empty_tqual(0U);void*_tmp713=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp716,_tmp715,_tmp714,_tmp713,Cyc_Absyn_false_type);});}
# 823
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Absyn_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp19B=Cyc_Absyn_compress(e);void*_stmttmp28=_tmp19B;void*_tmp19C=_stmttmp28;struct Cyc_Core_Opt*_tmp19F;void**_tmp19E;struct Cyc_Core_Opt*_tmp19D;void*_tmp1A0;struct Cyc_List_List*_tmp1A1;void*_tmp1A2;switch(*((int*)_tmp19C)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2 != 0){_LL1: _tmp1A2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->hd;_LL2: {void*r2=_tmp1A2;
# 830
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Absyn_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp1A3=({struct _tuple15 _tmp61C;_tmp61C.f1=r,_tmp61C.f2=r2;_tmp61C;});struct _tuple15 _stmttmp29=_tmp1A3;struct _tuple15 _tmp1A4=_stmttmp29;struct Cyc_Absyn_Tvar*_tmp1A6;struct Cyc_Absyn_Tvar*_tmp1A5;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A4.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A4.f2)->tag == 2U){_LLC: _tmp1A5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A4.f1)->f1;_tmp1A6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A4.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1A5;struct Cyc_Absyn_Tvar*tv2=_tmp1A6;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1A1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2;_LL4: {struct Cyc_List_List*es=_tmp1A1;
# 838
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2 != 0){_LL5: _tmp1A0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->hd;_LL6: {void*t=_tmp1A0;
# 842
void*_tmp1A7=Cyc_Tcutil_rgns_of(t);void*_stmttmp2A=_tmp1A7;void*_tmp1A8=_stmttmp2A;void*_tmp1A9;void*_tmp1AA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A8)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A8)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A8)->f2 != 0){_LL11: _tmp1AA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A8)->f2)->hd;_LL12: {void*t=_tmp1AA;
# 844
if(!constrain)return 0;{
void*_tmp1AB=Cyc_Absyn_compress(t);void*_stmttmp2B=_tmp1AB;void*_tmp1AC=_stmttmp2B;struct Cyc_Core_Opt*_tmp1AF;void**_tmp1AE;struct Cyc_Core_Opt*_tmp1AD;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AC)->tag == 1U){_LL16: _tmp1AD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AC)->f1;_tmp1AE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AC)->f2;_tmp1AF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AC)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1AD;void**p=_tmp1AE;struct Cyc_Core_Opt*s=_tmp1AF;
# 849
void*_tmp1B0=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1B0;
# 852
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp718=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1B1[2U];_tmp1B1[0]=ev,({void*_tmp717=Cyc_Absyn_access_eff(r);_tmp1B1[1]=_tmp717;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B1,sizeof(void*),2U));})));*p=_tmp718;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1A9=_tmp1A8;_LL14: {void*e2=_tmp1A9;
# 857
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp19D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19C)->f1;_tmp19E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19C)->f2;_tmp19F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19C)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp19D;void**p=_tmp19E;struct Cyc_Core_Opt*s=_tmp19F;
# 860
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1B3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp719=({const char*_tmp1B4="effect evar has wrong kind";_tag_fat(_tmp1B4,sizeof(char),27U);});_tmp61D.f1=_tmp719;});_tmp61D;});void*_tmp1B2[1U];_tmp1B2[0]=& _tmp1B3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1B2,sizeof(void*),1U));});
if(!constrain)return 0;{
# 865
void*_tmp1B5=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1B5;
# 868
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp71B=Cyc_Absyn_join_eff(({void*_tmp1B6[2U];_tmp1B6[0]=ev,({void*_tmp71A=Cyc_Absyn_access_eff(r);_tmp1B6[1]=_tmp71A;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B6,sizeof(void*),2U));}));*p=_tmp71B;});
return 1;}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 878
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Absyn_compress(t);{
void*_tmp1B7=Cyc_Tcutil_normalize_effect(Cyc_Absyn_compress(e));void*_stmttmp2C=_tmp1B7;void*_tmp1B8=_stmttmp2C;struct Cyc_Core_Opt*_tmp1BB;void**_tmp1BA;struct Cyc_Core_Opt*_tmp1B9;void*_tmp1BC;struct Cyc_List_List*_tmp1BD;switch(*((int*)_tmp1B8)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B8)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1BD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B8)->f2;_LL4: {struct Cyc_List_List*es=_tmp1BD;
# 883
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B8)->f2 != 0){_LL5: _tmp1BC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B8)->f2)->hd;_LL6: {void*t2=_tmp1BC;
# 888
t2=Cyc_Absyn_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1BE=Cyc_Tcutil_rgns_of(t);void*_stmttmp2D=_tmp1BE;void*_tmp1BF=_stmttmp2D;void*_tmp1C0;void*_tmp1C1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BF)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BF)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BF)->f2 != 0){_LLC: _tmp1C1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BF)->f2)->hd;_LLD: {void*t3=_tmp1C1;
# 893
struct _tuple15 _tmp1C2=({struct _tuple15 _tmp61E;({void*_tmp71C=Cyc_Absyn_compress(t3);_tmp61E.f1=_tmp71C;}),_tmp61E.f2=t2;_tmp61E;});struct _tuple15 _stmttmp2E=_tmp1C2;struct _tuple15 _tmp1C3=_stmttmp2E;struct Cyc_Absyn_Tvar*_tmp1C5;struct Cyc_Absyn_Tvar*_tmp1C4;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C3.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C3.f2)->tag == 2U){_LL11: _tmp1C4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C3.f1)->f1;_tmp1C5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C3.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1C4;struct Cyc_Absyn_Tvar*tv2=_tmp1C5;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1C0=_tmp1BF;_LLF: {void*e2=_tmp1C0;
# 897
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1B9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B8)->f1;_tmp1BA=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B8)->f2;_tmp1BB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B8)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1B9;void**p=_tmp1BA;struct Cyc_Core_Opt*s=_tmp1BB;
# 900
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61F;_tmp61F.tag=0U,({struct _fat_ptr _tmp71D=({const char*_tmp1C8="effect evar has wrong kind";_tag_fat(_tmp1C8,sizeof(char),27U);});_tmp61F.f1=_tmp71D;});_tmp61F;});void*_tmp1C6[1U];_tmp1C6[0]=& _tmp1C7;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1C6,sizeof(void*),1U));});
if(!may_constrain_evars)return 0;{
# 905
void*_tmp1C9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1C9;
# 908
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1CA=Cyc_Absyn_join_eff(({void*_tmp1CB[2U];_tmp1CB[0]=ev,({void*_tmp71E=Cyc_Absyn_regionsof_eff(t);_tmp1CB[1]=_tmp71E;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CB,sizeof(void*),2U));}));void*new_typ=_tmp1CA;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 918
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp1CC=e;struct Cyc_Core_Opt*_tmp1CF;void**_tmp1CE;struct Cyc_Core_Opt*_tmp1CD;void*_tmp1D0;struct Cyc_List_List*_tmp1D1;struct Cyc_Absyn_Tvar*_tmp1D2;switch(*((int*)_tmp1CC)){case 2U: _LL1: _tmp1D2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CC)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1D2;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC)->f1)){case 9U: _LL3: _tmp1D1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC)->f2;_LL4: {struct Cyc_List_List*es=_tmp1D1;
# 923
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC)->f2 != 0){_LL5: _tmp1D0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC)->f2)->hd;_LL6: {void*t=_tmp1D0;
# 928
void*_tmp1D3=Cyc_Tcutil_rgns_of(t);void*_stmttmp2F=_tmp1D3;void*_tmp1D4=_stmttmp2F;void*_tmp1D5;void*_tmp1D6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D4)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D4)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D4)->f2 != 0){_LLC: _tmp1D6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D4)->f2)->hd;_LLD: {void*t2=_tmp1D6;
# 930
if(!may_constrain_evars)return 0;{
void*_tmp1D7=Cyc_Absyn_compress(t2);void*_stmttmp30=_tmp1D7;void*_tmp1D8=_stmttmp30;struct Cyc_Core_Opt*_tmp1DB;void**_tmp1DA;struct Cyc_Core_Opt*_tmp1D9;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D8)->tag == 1U){_LL11: _tmp1D9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D8)->f1;_tmp1DA=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D8)->f2;_tmp1DB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D8)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1D9;void**p=_tmp1DA;struct Cyc_Core_Opt*s=_tmp1DB;
# 936
void*_tmp1DC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1DC;
# 938
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp720=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1DD[2U];_tmp1DD[0]=ev,({void*_tmp71F=Cyc_Absyn_var_type(v);_tmp1DD[1]=_tmp71F;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1DD,sizeof(void*),2U));})));*p=_tmp720;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1D5=_tmp1D4;_LLF: {void*e2=_tmp1D5;
# 943
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1CD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CC)->f1;_tmp1CE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CC)->f2;_tmp1CF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CC)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1CD;void**p=_tmp1CE;struct Cyc_Core_Opt*s=_tmp1CF;
# 946
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp620;_tmp620.tag=0U,({struct _fat_ptr _tmp721=({const char*_tmp1E0="effect evar has wrong kind";_tag_fat(_tmp1E0,sizeof(char),27U);});_tmp620.f1=_tmp721;});_tmp620;});void*_tmp1DE[1U];_tmp1DE[0]=& _tmp1DF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1DE,sizeof(void*),1U));});{
# 950
void*_tmp1E1=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E1;
# 952
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1E2=Cyc_Absyn_join_eff(({void*_tmp1E3[2U];_tmp1E3[0]=ev,({void*_tmp722=Cyc_Absyn_var_type(v);_tmp1E3[1]=_tmp722;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E3,sizeof(void*),2U));}));void*new_typ=_tmp1E2;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 962
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Absyn_compress(e);{
void*_tmp1E4=e;void*_tmp1E5;struct Cyc_List_List*_tmp1E6;switch(*((int*)_tmp1E4)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E4)->f1)){case 9U: _LL1: _tmp1E6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E4)->f2;_LL2: {struct Cyc_List_List*es=_tmp1E6;
# 966
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E4)->f2 != 0){_LL3: _tmp1E5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E4)->f2)->hd;_LL4: {void*t=_tmp1E5;
# 971
void*_tmp1E7=Cyc_Tcutil_rgns_of(t);void*_stmttmp31=_tmp1E7;void*_tmp1E8=_stmttmp31;void*_tmp1E9;void*_tmp1EA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f2 != 0){_LLA: _tmp1EA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E8)->f2)->hd;_LLB: {void*t2=_tmp1EA;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1E9=_tmp1E8;_LLD: {void*e2=_tmp1E9;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 975
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 989 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 992
void*_tmp1EB=Cyc_Absyn_compress(e1);void*_stmttmp32=_tmp1EB;void*_tmp1EC=_stmttmp32;struct Cyc_Core_Opt*_tmp1EE;void**_tmp1ED;struct Cyc_Absyn_Tvar*_tmp1EF;void*_tmp1F0;void*_tmp1F1;struct Cyc_List_List*_tmp1F2;switch(*((int*)_tmp1EC)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f1)){case 9U: _LL1: _tmp1F2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f2;_LL2: {struct Cyc_List_List*es=_tmp1F2;
# 994
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f2 != 0){_LL3: _tmp1F1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f2)->hd;_LL4: {void*r=_tmp1F1;
# 1005
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f2 != 0){_LL7: _tmp1F0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f2)->hd;_LL8: {void*t=_tmp1F0;
# 1009
void*_tmp1F3=Cyc_Tcutil_rgns_of(t);void*_stmttmp33=_tmp1F3;void*_tmp1F4=_stmttmp33;void*_tmp1F5;void*_tmp1F6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F4)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F4)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F4)->f2 != 0){_LLE: _tmp1F6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F4)->f2)->hd;_LLF: {void*t2=_tmp1F6;
# 1014
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1F5=_tmp1F4;_LL11: {void*e=_tmp1F5;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1EF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EC)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1EF;
# 1007
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1ED=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EC)->f2;_tmp1EE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EC)->f4;_LLA: {void**p=_tmp1ED;struct Cyc_Core_Opt*s=_tmp1EE;
# 1019
if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1023
*p=Cyc_Absyn_empty_effect;
# 1026
return 1;}else{
# 1028
return 0;}}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp622;_tmp622.tag=0U,({struct _fat_ptr _tmp723=({const char*_tmp1FA="subset_effect: bad effect: ";_tag_fat(_tmp1FA,sizeof(char),28U);});_tmp622.f1=_tmp723;});_tmp622;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp1F9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp621;_tmp621.tag=2U,_tmp621.f1=(void*)e1;_tmp621;});void*_tmp1F7[2U];_tmp1F7[0]=& _tmp1F8,_tmp1F7[1]=& _tmp1F9;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1F7,sizeof(void*),2U));});}_LL0:;}
# 1039
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1041
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp1FB=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp34=_tmp1FB;struct _tuple15*_tmp1FC=_stmttmp34;void*_tmp1FE;void*_tmp1FD;_LL1: _tmp1FD=_tmp1FC->f1;_tmp1FE=_tmp1FC->f2;_LL2: {void*t1a=_tmp1FD;void*t1b=_tmp1FE;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp1FF=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp35=_tmp1FF;struct _tuple15*_tmp200=_stmttmp35;void*_tmp202;void*_tmp201;_LL4: _tmp201=_tmp200->f1;_tmp202=_tmp200->f2;_LL5: {void*t2a=_tmp201;void*t2b=_tmp202;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1051
if(!found)return 0;}}}
# 1053
return 1;}
# 1057
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1059
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1062
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp203=t;switch(*((int*)_tmp203)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp203)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp203)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1068
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1071
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1074
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1077
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp203)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1066
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1069
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1072
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1075
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1078
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp203)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1067
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1070
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1073
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1076
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1079
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1101
({struct Cyc_Warn_String_Warn_Warg_struct _tmp205=({struct Cyc_Warn_String_Warn_Warg_struct _tmp623;_tmp623.tag=0U,({struct _fat_ptr _tmp724=({const char*_tmp206="bad con";_tag_fat(_tmp206,sizeof(char),8U);});_tmp623.f1=_tmp724;});_tmp623;});void*_tmp204[1U];_tmp204[0]=& _tmp205;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp204,sizeof(void*),1U));});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp203)->f1){case 0U: _LL21: _LL22:
# 1080
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
# 1104
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
 return 11;default: _LL17: _tmp208=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f1;_LL18: {void*c=_tmp208;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1120
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp209=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp209;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1125
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp20A=i;struct _tuple1*_tmp20B;struct Cyc_Absyn_Datatypedecl*_tmp20C;if((_tmp20A.KnownDatatype).tag == 2){_LL1: _tmp20C=*(_tmp20A.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp20C;
return dd->name;}}else{_LL3: _tmp20B=((_tmp20A.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp20B;
return n;}}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1131
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp20D=i;struct _tuple1*_tmp20F;struct _tuple1*_tmp20E;struct Cyc_Absyn_Datatypefield*_tmp211;struct Cyc_Absyn_Datatypedecl*_tmp210;if((_tmp20D.KnownDatatypefield).tag == 2){_LL1: _tmp210=((_tmp20D.KnownDatatypefield).val).f1;_tmp211=((_tmp20D.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp210;struct Cyc_Absyn_Datatypefield*df=_tmp211;
# 1134
return({struct _tuple20 _tmp624;_tmp624.f1=dd->name,_tmp624.f2=df->name;_tmp624;});}}else{_LL3: _tmp20E=((_tmp20D.UnknownDatatypefield).val).datatype_name;_tmp20F=((_tmp20D.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp20E;struct _tuple1*f=_tmp20F;
# 1136
return({struct _tuple20 _tmp625;_tmp625.f1=d,_tmp625.f2=f;_tmp625;});}}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1139
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp212=i;struct Cyc_Absyn_Aggrdecl*_tmp213;struct _tuple1*_tmp215;enum Cyc_Absyn_AggrKind _tmp214;if((_tmp212.UnknownAggr).tag == 1){_LL1: _tmp214=((_tmp212.UnknownAggr).val).f1;_tmp215=((_tmp212.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp214;struct _tuple1*n=_tmp215;
return({struct _tuple21 _tmp626;_tmp626.f1=k,_tmp626.f2=n;_tmp626;});}}else{_LL3: _tmp213=*(_tmp212.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp213;
return({struct _tuple21 _tmp627;_tmp627.f1=ad->kind,_tmp627.f2=ad->name;_tmp627;});}}_LL0:;}
# 1145
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1151
struct _tuple15 _tmp216=({struct _tuple15 _tmp628;_tmp628.f1=t1,_tmp628.f2=t2;_tmp628;});struct _tuple15 _stmttmp36=_tmp216;struct _tuple15 _tmp217=_stmttmp36;union Cyc_Absyn_AggrInfo _tmp219;union Cyc_Absyn_AggrInfo _tmp218;union Cyc_Absyn_DatatypeFieldInfo _tmp21B;union Cyc_Absyn_DatatypeFieldInfo _tmp21A;union Cyc_Absyn_DatatypeInfo _tmp21D;union Cyc_Absyn_DatatypeInfo _tmp21C;struct Cyc_List_List*_tmp21F;struct Cyc_List_List*_tmp21E;struct _fat_ptr _tmp221;struct _fat_ptr _tmp220;struct _tuple1*_tmp223;struct _tuple1*_tmp222;switch(*((int*)_tmp217.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 15U){_LL1: _tmp222=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp223=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL2: {struct _tuple1*n1=_tmp222;struct _tuple1*n2=_tmp223;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 17U){_LL3: _tmp220=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp221=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp220;struct _fat_ptr s2=_tmp221;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 16U){_LL5: _tmp21E=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp21F=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp21E;struct Cyc_List_List*fs2=_tmp21F;
# 1155
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 18U){_LL7: _tmp21C=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp21D=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp21C;union Cyc_Absyn_DatatypeInfo info2=_tmp21D;
# 1157
return({struct _tuple1*_tmp725=Cyc_Tcutil_get_datatype_qvar(info1);Cyc_Absyn_qvar_cmp(_tmp725,Cyc_Tcutil_get_datatype_qvar(info2));});}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 19U){_LL9: _tmp21A=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp21B=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp21A;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp21B;
# 1159
struct _tuple20 _tmp224=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple20 _stmttmp37=_tmp224;struct _tuple20 _tmp225=_stmttmp37;struct _tuple1*_tmp227;struct _tuple1*_tmp226;_LL10: _tmp226=_tmp225.f1;_tmp227=_tmp225.f2;_LL11: {struct _tuple1*d1=_tmp226;struct _tuple1*f1=_tmp227;
struct _tuple20 _tmp228=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple20 _stmttmp38=_tmp228;struct _tuple20 _tmp229=_stmttmp38;struct _tuple1*_tmp22B;struct _tuple1*_tmp22A;_LL13: _tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;_LL14: {struct _tuple1*d2=_tmp22A;struct _tuple1*f2=_tmp22B;
int _tmp22C=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp22C;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 20U){_LLB: _tmp218=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp219=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp218;union Cyc_Absyn_AggrInfo info2=_tmp219;
# 1165
struct _tuple21 _tmp22D=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple21 _stmttmp39=_tmp22D;struct _tuple21 _tmp22E=_stmttmp39;struct _tuple1*_tmp230;enum Cyc_Absyn_AggrKind _tmp22F;_LL16: _tmp22F=_tmp22E.f1;_tmp230=_tmp22E.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp22F;struct _tuple1*q1=_tmp230;
struct _tuple21 _tmp231=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple21 _stmttmp3A=_tmp231;struct _tuple21 _tmp232=_stmttmp3A;struct _tuple1*_tmp234;enum Cyc_Absyn_AggrKind _tmp233;_LL19: _tmp233=_tmp232.f1;_tmp234=_tmp232.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp233;struct _tuple1*q2=_tmp234;
int _tmp235=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp235;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1174
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp727)(void*,void*)=cmp;void*_tmp726=(void*)_check_null(a1);_tmp727(_tmp726,(void*)_check_null(a2));});}
# 1180
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp236=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp236;
int _tmp237=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp237;
return Cyc_Core_intcmp(i1,i2);}
# 1185
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp238=tqt1;void*_tmp23A;struct Cyc_Absyn_Tqual _tmp239;_LL1: _tmp239=_tmp238->f1;_tmp23A=_tmp238->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp239;void*t1=_tmp23A;
struct _tuple12*_tmp23B=tqt2;void*_tmp23D;struct Cyc_Absyn_Tqual _tmp23C;_LL4: _tmp23C=_tmp23B->f1;_tmp23D=_tmp23B->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp23C;void*t2=_tmp23D;
int _tmp23E=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp23E;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1193
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp23F=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp23F;
if(zsc != 0)return zsc;{
int _tmp240=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp240;
if(tqc != 0)return tqc;{
int _tmp241=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp241;
if(tc != 0)return tc;{
int _tmp242=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Atts_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp242;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1210
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
if(t1 == t2)return 0;{
int _tmp243=({int _tmp728=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp728,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp243;
if(shallowcmp != 0)
return shallowcmp;{
# 1219
struct _tuple15 _tmp244=({struct _tuple15 _tmp62D;_tmp62D.f1=t2,_tmp62D.f2=t1;_tmp62D;});struct _tuple15 _stmttmp3B=_tmp244;struct _tuple15 _tmp245=_stmttmp3B;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Exp*_tmp249;struct Cyc_Absyn_Exp*_tmp248;struct Cyc_List_List*_tmp24D;enum Cyc_Absyn_AggrKind _tmp24C;struct Cyc_List_List*_tmp24B;enum Cyc_Absyn_AggrKind _tmp24A;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp24E;struct Cyc_Absyn_FnInfo _tmp251;struct Cyc_Absyn_FnInfo _tmp250;void*_tmp259;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Tqual _tmp257;void*_tmp256;void*_tmp255;struct Cyc_Absyn_Exp*_tmp254;struct Cyc_Absyn_Tqual _tmp253;void*_tmp252;void*_tmp265;void*_tmp264;void*_tmp263;void*_tmp262;struct Cyc_Absyn_Tqual _tmp261;void*_tmp260;void*_tmp25F;void*_tmp25E;void*_tmp25D;void*_tmp25C;struct Cyc_Absyn_Tqual _tmp25B;void*_tmp25A;struct Cyc_Absyn_Tvar*_tmp267;struct Cyc_Absyn_Tvar*_tmp266;struct Cyc_List_List*_tmp26B;void*_tmp26A;struct Cyc_List_List*_tmp269;void*_tmp268;switch(*((int*)_tmp245.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->tag == 0U){_LL1: _tmp268=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp269=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f1)->f2;_tmp26A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->f1;_tmp26B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->f2;_LL2: {void*c1=_tmp268;struct Cyc_List_List*ts1=_tmp269;void*c2=_tmp26A;struct Cyc_List_List*ts2=_tmp26B;
# 1221
int _tmp26C=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp26C;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp245.f2)->tag == 1U){_LL3: _LL4:
# 1225
({struct Cyc_Warn_String_Warn_Warg_struct _tmp26E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp629;_tmp629.tag=0U,({struct _fat_ptr _tmp729=({const char*_tmp26F="typecmp: can only compare closed types";_tag_fat(_tmp26F,sizeof(char),39U);});_tmp629.f1=_tmp729;});_tmp629;});void*_tmp26D[1U];_tmp26D[0]=& _tmp26E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp26D,sizeof(void*),1U));});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f2)->tag == 2U){_LL5: _tmp266=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp267=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp266;struct Cyc_Absyn_Tvar*tv1=_tmp267;
# 1229
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->tag == 3U){_LL7: _tmp25A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_type;_tmp25B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_tq;_tmp25C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).rgn;_tmp25D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).nullable;_tmp25E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).bounds;_tmp25F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).zero_term;_tmp260=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_type;_tmp261=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_tq;_tmp262=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).rgn;_tmp263=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).nullable;_tmp264=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).bounds;_tmp265=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp25A;struct Cyc_Absyn_Tqual tqual2a=_tmp25B;void*rgn2=_tmp25C;void*null2a=_tmp25D;void*b2=_tmp25E;void*zt2=_tmp25F;void*t1a=_tmp260;struct Cyc_Absyn_Tqual tqual1a=_tmp261;void*rgn1=_tmp262;void*null1a=_tmp263;void*b1=_tmp264;void*zt1=_tmp265;
# 1233
int _tmp270=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp270;
if(etc != 0)return etc;{
int _tmp271=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp271;
if(rc != 0)return rc;{
int _tmp272=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp272;
if(tqc != 0)return tqc;{
int _tmp273=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp273;
if(cc != 0)return cc;{
int _tmp274=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp274;
if(zc != 0)return zc;{
int _tmp275=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp275;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->tag == 4U){_LL9: _tmp252=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_type;_tmp253=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).tq;_tmp254=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).num_elts;_tmp255=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).zero_term;_tmp256=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_type;_tmp257=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).tq;_tmp258=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).num_elts;_tmp259=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).zero_term;_LLA: {void*t2a=_tmp252;struct Cyc_Absyn_Tqual tq2a=_tmp253;struct Cyc_Absyn_Exp*e1=_tmp254;void*zt1=_tmp255;void*t1a=_tmp256;struct Cyc_Absyn_Tqual tq1a=_tmp257;struct Cyc_Absyn_Exp*e2=_tmp258;void*zt2=_tmp259;
# 1249
int _tmp276=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp276;
if(tqc != 0)return tqc;{
int _tmp277=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp277;
if(tc != 0)return tc;{
int _tmp278=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp278;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62A;_tmp62A.tag=0U,({struct _fat_ptr _tmp72A=({const char*_tmp27B="missing expression in array index";_tag_fat(_tmp27B,sizeof(char),34U);});_tmp62A.f1=_tmp72A;});_tmp62A;});void*_tmp279[1U];_tmp279[0]=& _tmp27A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp279,sizeof(void*),1U));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f2)->tag == 5U){_LLB: _tmp250=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp251=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp250;struct Cyc_Absyn_FnInfo f2=_tmp251;
# 1261
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp27C=f1.args;struct Cyc_List_List*args1=_tmp27C;
struct Cyc_List_List*_tmp27D=f2.args;struct Cyc_List_List*args2=_tmp27D;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp27E=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3C=_tmp27E;struct _tuple9 _tmp27F=_stmttmp3C;void*_tmp281;struct Cyc_Absyn_Tqual _tmp280;_LL18: _tmp280=_tmp27F.f2;_tmp281=_tmp27F.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp280;void*t1=_tmp281;
struct _tuple9 _tmp282=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp3D=_tmp282;struct _tuple9 _tmp283=_stmttmp3D;void*_tmp285;struct Cyc_Absyn_Tqual _tmp284;_LL1B: _tmp284=_tmp283.f2;_tmp285=_tmp283.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp284;void*t2=_tmp285;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1276
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp72B=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp72B,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1290
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp286=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp286;
struct Cyc_List_List*_tmp287=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp287;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp288=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp3E=_tmp288;struct _tuple15 _tmp289=_stmttmp3E;void*_tmp28B;void*_tmp28A;_LL1E: _tmp28A=_tmp289.f1;_tmp28B=_tmp289.f2;_LL1F: {void*t1a=_tmp28A;void*t1b=_tmp28B;
struct _tuple15 _tmp28C=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp3F=_tmp28C;struct _tuple15 _tmp28D=_stmttmp3F;void*_tmp28F;void*_tmp28E;_LL21: _tmp28E=_tmp28D.f1;_tmp28F=_tmp28D.f2;_LL22: {void*t2a=_tmp28E;void*t2b=_tmp28F;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1300
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1308
({struct Cyc_Warn_String_Warn_Warg_struct _tmp291=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62B;_tmp62B.tag=0U,({struct _fat_ptr _tmp72C=({const char*_tmp292="typecmp: function type comparison should never get here!";_tag_fat(_tmp292,sizeof(char),57U);});_tmp62B.f1=_tmp72C;});_tmp62B;});void*_tmp290[1U];_tmp290[0]=& _tmp291;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp290,sizeof(void*),1U));});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f2)->tag == 6U){_LLD: _tmp24E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp24F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp24E;struct Cyc_List_List*ts1=_tmp24F;
# 1311
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->tag == 7U){_LLF: _tmp24A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp24B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f1)->f2;_tmp24C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->f1;_tmp24D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp24A;struct Cyc_List_List*fs2=_tmp24B;enum Cyc_Absyn_AggrKind k1=_tmp24C;struct Cyc_List_List*fs1=_tmp24D;
# 1314
if((int)k1 != (int)k2)
return(int)k1 == (int)0U?- 1: 1;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f2)->tag == 9U){_LL11: _tmp248=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp249=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp248;struct Cyc_Absyn_Exp*e2=_tmp249;
# 1318
_tmp246=e1;_tmp247=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f2)->tag == 11U){_LL13: _tmp246=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp247=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp246;struct Cyc_Absyn_Exp*e2=_tmp247;
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp294=({struct Cyc_Warn_String_Warn_Warg_struct _tmp62C;_tmp62C.tag=0U,({struct _fat_ptr _tmp72D=({const char*_tmp295="Unmatched case in typecmp";_tag_fat(_tmp295,sizeof(char),26U);});_tmp62C.f1=_tmp72D;});_tmp62C;});void*_tmp293[1U];_tmp293[0]=& _tmp294;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp293,sizeof(void*),1U));});}_LL0:;}}}
# 1326
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp296=({struct _tuple15 _tmp62F;({void*_tmp72F=Cyc_Absyn_compress(t1);_tmp62F.f1=_tmp72F;}),({void*_tmp72E=Cyc_Absyn_compress(t2);_tmp62F.f2=_tmp72E;});_tmp62F;});struct _tuple15 _stmttmp40=_tmp296;struct _tuple15 _tmp297=_stmttmp40;void*_tmp299;void*_tmp298;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f2)->tag == 0U){_LL1: _tmp298=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f1)->f1;_tmp299=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f2)->f1;_LL2: {void*c1=_tmp298;void*c2=_tmp299;
# 1329
struct _tuple15 _tmp29A=({struct _tuple15 _tmp62E;_tmp62E.f1=c1,_tmp62E.f2=c2;_tmp62E;});struct _tuple15 _stmttmp41=_tmp29A;struct _tuple15 _tmp29B=_stmttmp41;int _tmp29D;int _tmp29C;switch(*((int*)_tmp29B.f1)){case 2U: switch(*((int*)_tmp29B.f2)){case 2U: _LL6: _tmp29C=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29B.f1)->f1;_tmp29D=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29B.f2)->f1;_LL7: {int i1=_tmp29C;int i2=_tmp29D;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp29B.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1337
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1342
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1345
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29B.f2)->f1 == 0){_LL14: _LL15:
# 1340
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp29B.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1338
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1343
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1346
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29B.f2)->f1 == 0){_LL16: _LL17:
# 1341
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1347
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1344
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1348
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1350
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1352
 return 0;}_LL0:;}
# 1356
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp29E=({struct _tuple15 _tmp631;({void*_tmp731=Cyc_Absyn_compress(t1);_tmp631.f1=_tmp731;}),({void*_tmp730=Cyc_Absyn_compress(t2);_tmp631.f2=_tmp730;});_tmp631;});struct _tuple15 _stmttmp42=_tmp29E;struct _tuple15 _tmp29F=_stmttmp42;void*_tmp2A1;void*_tmp2A0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f2)->tag == 0U){_LL1: _tmp2A0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f1)->f1;_tmp2A1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f2)->f1;_LL2: {void*c1=_tmp2A0;void*c2=_tmp2A1;
# 1359
{struct _tuple15 _tmp2A2=({struct _tuple15 _tmp630;_tmp630.f1=c1,_tmp630.f2=c2;_tmp630;});struct _tuple15 _stmttmp43=_tmp2A2;struct _tuple15 _tmp2A3=_stmttmp43;int _tmp2A5;int _tmp2A4;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 2U){_LL6: _tmp2A4=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f1;_tmp2A5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f1;_LL7: {int i1=_tmp2A4;int i2=_tmp2A5;
# 1361
if(i1 != 0 && i1 != 1)return t1;
if(i2 != 0 && i2 != 1)return t2;
if(i1 >= i2)return t1;
return t2;}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1371
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1376
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1369
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1378
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1368
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1372
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1377
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1370
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1379
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 4U){_LL18: _LL19:
# 1374
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1380
 goto _LL5;}}}}}}_LL5:;}
# 1382
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1385
return Cyc_Absyn_sint_type;}
# 1390
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1393
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Absyn_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6->v=t1;_tmp2A6;});}}}
# 1401
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1405
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp732=({const char*_tmp2AD="type mismatch: expecting ";_tag_fat(_tmp2AD,sizeof(char),26U);});_tmp635.f1=_tmp732;});_tmp635;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2A9=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp634;_tmp634.tag=2U,_tmp634.f1=(void*)t;_tmp634;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2AA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,({struct _fat_ptr _tmp733=({const char*_tmp2AC=" but found ";_tag_fat(_tmp2AC,sizeof(char),12U);});_tmp633.f1=_tmp733;});_tmp633;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2AB=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp632;_tmp632.tag=2U,_tmp632.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt);_tmp632;});void*_tmp2A7[4U];_tmp2A7[0]=& _tmp2A8,_tmp2A7[1]=& _tmp2A9,_tmp2A7[2]=& _tmp2AA,_tmp2A7[3]=& _tmp2AB;({unsigned _tmp734=((struct Cyc_Absyn_Exp*)el->hd)->loc;Cyc_Warn_err2(_tmp734,_tag_fat(_tmp2A7,sizeof(void*),4U));});});
# 1409
return 0;}}}
# 1411
return 1;}
# 1416
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2AE=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp44=_tmp2AE;void*_tmp2AF=_stmttmp44;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AF)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1422
return 1;}
# 1425
static int Cyc_Tcutil_coerce_Xint_type(void*Xint_type,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Xint_type))
return 1;
# 1429
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Xint_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp636;_tmp636.tag=0U,({struct _fat_ptr _tmp735=({const char*_tmp2B2="integral size mismatch; conversion supplied";_tag_fat(_tmp2B2,sizeof(char),44U);});_tmp636.f1=_tmp735;});_tmp636;});void*_tmp2B0[1U];_tmp2B0[0]=& _tmp2B1;({unsigned _tmp736=e->loc;Cyc_Warn_warn2(_tmp736,_tag_fat(_tmp2B0,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e,Xint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1435
return 0;}
# 1438
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_uint_type,e);}
# 1442
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_sint_type,e);}
# 1448
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1454
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp737=t;Cyc_Unify_unify(_tmp737,Cyc_Absyn_bounds_one());});
return Cyc_Absyn_compress(t);}
# 1459
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2B3=Cyc_Absyn_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2B3;
void*_tmp2B4=t;struct Cyc_Absyn_Exp*_tmp2B5;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B4)->tag == 9U){_LL1: _tmp2B5=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B4)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2B5;
return e;}}else{_LL3: _LL4: {
# 1464
struct Cyc_Absyn_Exp*_tmp2B6=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2B6;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1473
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B7=Cyc_Absyn_compress(b);void*_stmttmp45=_tmp2B7;void*_tmp2B8=_stmttmp45;struct Cyc_List_List*_tmp2B9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2B9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B8)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2B9;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp638;_tmp638.tag=0U,({struct _fat_ptr _tmp738=({const char*_tmp2BD="get_bounds_exp: ";_tag_fat(_tmp2BD,sizeof(char),17U);});_tmp638.f1=_tmp738;});_tmp638;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp637;_tmp637.tag=2U,_tmp637.f1=(void*)b;_tmp637;});void*_tmp2BA[2U];_tmp2BA[0]=& _tmp2BB,_tmp2BA[1]=& _tmp2BC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2BA,sizeof(void*),2U));});}_LL0:;}}
# 1482
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2BE=Cyc_Absyn_compress(t);void*_stmttmp46=_tmp2BE;void*_tmp2BF=_stmttmp46;void*_tmp2C0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->tag == 3U){_LL1: _tmp2C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2C0;
# 1485
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A;_tmp63A.tag=0U,({struct _fat_ptr _tmp739=({const char*_tmp2C4="get_ptr_bounds_exp not pointer: ";_tag_fat(_tmp2C4,sizeof(char),33U);});_tmp63A.f1=_tmp739;});_tmp63A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2C3=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp639;_tmp639.tag=2U,_tmp639.f1=(void*)t;_tmp639;});void*_tmp2C1[2U];_tmp2C1[0]=& _tmp2C2,_tmp2C1[1]=& _tmp2C3;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2C1,sizeof(void*),2U));});}_LL0:;}
# 1491
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5->v=tvs;_tmp2C5;}));}
# 1495
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6->v=tvs;_tmp2C6;}));}
# 1500
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1507
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp2C7=({struct _tuple15 _tmp641;_tmp641.f1=t1,_tmp641.f2=t2;_tmp641;});struct _tuple15 _stmttmp47=_tmp2C7;struct _tuple15 _tmp2C8=_stmttmp47;void*_tmp2D0;struct Cyc_Absyn_Exp*_tmp2CF;struct Cyc_Absyn_Tqual _tmp2CE;void*_tmp2CD;void*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Tqual _tmp2CA;void*_tmp2C9;struct Cyc_Absyn_PtrInfo _tmp2D2;struct Cyc_Absyn_PtrInfo _tmp2D1;switch(*((int*)_tmp2C8.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C8.f2)->tag == 3U){_LL1: _tmp2D1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C8.f1)->f1;_tmp2D2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C8.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2D1;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2D2;
# 1513
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable)&&
 Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable))
return 0;
# 1517
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple22 _tmp2D3=({struct _tuple22 _tmp63C;({struct Cyc_Absyn_Exp*_tmp73D=({void*_tmp73C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp73C,(pinfo_a.ptr_atts).bounds);});_tmp63C.f1=_tmp73D;}),({
struct Cyc_Absyn_Exp*_tmp73B=({void*_tmp73A=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp73A,(pinfo_b.ptr_atts).bounds);});_tmp63C.f2=_tmp73B;});_tmp63C;});
# 1518
struct _tuple22 _stmttmp48=_tmp2D3;struct _tuple22 _tmp2D4=_stmttmp48;struct Cyc_Absyn_Exp*_tmp2D6;struct Cyc_Absyn_Exp*_tmp2D5;if(_tmp2D4.f2 == 0){_LLA: _LLB:
# 1521
 goto _LL9;}else{if(_tmp2D4.f1 == 0){_LLC: _LLD:
# 1524
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp73E=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp73E,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
return 0;}else{_LLE: _tmp2D5=_tmp2D4.f1;_tmp2D6=_tmp2D4.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2D5;struct Cyc_Absyn_Exp*e2=_tmp2D6;
# 1532
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63B;_tmp63B.tag=0U,({struct _fat_ptr _tmp73F=({const char*_tmp2D9="implicit cast to shorter array";_tag_fat(_tmp2D9,sizeof(char),31U);});_tmp63B.f1=_tmp73F;});_tmp63B;});void*_tmp2D7[1U];_tmp2D7[0]=& _tmp2D8;({unsigned _tmp740=loc;Cyc_Warn_warn2(_tmp740,_tag_fat(_tmp2D7,sizeof(void*),1U));});});
if(!({struct Cyc_Absyn_Exp*_tmp741=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp741,(struct Cyc_Absyn_Exp*)_check_null(e1));}))
return 0;
goto _LL9;}}}_LL9:;}
# 1540
if((pinfo_a.elt_tq).real_const && !(pinfo_b.elt_tq).real_const)
return 0;
# 1543
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(!Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn))
# 1546
return 0;
if(Cyc_Flags_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp640;_tmp640.tag=0U,({struct _fat_ptr _tmp742=({const char*_tmp2E0="implicit cast from region ";_tag_fat(_tmp2E0,sizeof(char),27U);});_tmp640.f1=_tmp742;});_tmp640;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63F;_tmp63F.tag=2U,_tmp63F.f1=(void*)(pinfo_a.ptr_atts).rgn;_tmp63F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63E;_tmp63E.tag=0U,({
struct _fat_ptr _tmp743=({const char*_tmp2DF=" to region ";_tag_fat(_tmp2DF,sizeof(char),12U);});_tmp63E.f1=_tmp743;});_tmp63E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DE=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63D;_tmp63D.tag=2U,_tmp63D.f1=(void*)(pinfo_b.ptr_atts).rgn;_tmp63D;});void*_tmp2DA[4U];_tmp2DA[0]=& _tmp2DB,_tmp2DA[1]=& _tmp2DC,_tmp2DA[2]=& _tmp2DD,_tmp2DA[3]=& _tmp2DE;({unsigned _tmp744=loc;Cyc_Warn_warn2(_tmp744,_tag_fat(_tmp2DA,sizeof(void*),4U));});});}
# 1552
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)&&(
!Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)|| !(pinfo_b.elt_tq).real_const))
# 1555
return 0;
# 1559
if(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type))
return 1;
# 1563
if(!({void*_tmp745=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp745,(pinfo_b.ptr_atts).bounds);})||
 Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
return 0;
if(!(pinfo_b.elt_tq).real_const && !Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type)))
return 0;
return Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type);}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f2)->tag == 4U){_LL3: _tmp2C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f1)->f1).elt_type;_tmp2CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f1)->f1).tq;_tmp2CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f1)->f1).num_elts;_tmp2CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f1)->f1).zero_term;_tmp2CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f2)->f1).elt_type;_tmp2CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f2)->f1).tq;_tmp2CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f2)->f1).num_elts;_tmp2D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C8.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2C9;struct Cyc_Absyn_Tqual tq1a=_tmp2CA;struct Cyc_Absyn_Exp*e1=_tmp2CB;void*zt1=_tmp2CC;void*t2a=_tmp2CD;struct Cyc_Absyn_Tqual tq2a=_tmp2CE;struct Cyc_Absyn_Exp*e2=_tmp2CF;void*zt2=_tmp2D0;
# 1572
if(!Cyc_Unify_unify(zt1,zt2))return 0;
if(e1 == 0 || e2 == 0)return 0;
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))return 0;
if(!Cyc_Unify_unify(t1a,t2a))return 0;
# 1577
return !tq1a.real_const || tq2a.real_const;}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C8.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C8.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C8.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1579
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1581
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1585
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2E1=Cyc_Absyn_compress(t);void*_stmttmp49=_tmp2E1;void*_tmp2E2=_stmttmp49;void*_tmp2E3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E2)->tag == 3U){_LL1: _tmp2E3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E2)->f1).elt_type;_LL2: {void*e=_tmp2E3;
return e;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp642;_tmp642.tag=0U,({struct _fat_ptr _tmp746=({const char*_tmp2E6="pointer_elt_type";_tag_fat(_tmp2E6,sizeof(char),17U);});_tmp642.f1=_tmp746;});_tmp642;});void*_tmp2E4[1U];_tmp2E4[0]=& _tmp2E5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2E4,sizeof(void*),1U));});}_LL0:;}
# 1591
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2E7=Cyc_Absyn_compress(t);void*_stmttmp4A=_tmp2E7;void*_tmp2E8=_stmttmp4A;struct Cyc_Absyn_PtrAtts*_tmp2E9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->tag == 3U){_LL1: _tmp2E9=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2E9;
return p->rgn;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp643;_tmp643.tag=0U,({struct _fat_ptr _tmp747=({const char*_tmp2EC="pointer_elt_type";_tag_fat(_tmp2EC,sizeof(char),17U);});_tmp643.f1=_tmp747;});_tmp643;});void*_tmp2EA[1U];_tmp2EA[0]=& _tmp2EB;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2EA,sizeof(void*),1U));});}_LL0:;}
# 1598
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2ED=Cyc_Absyn_compress(t);void*_stmttmp4B=_tmp2ED;void*_tmp2EE=_stmttmp4B;void*_tmp2EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EE)->tag == 3U){_LL1: _tmp2EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EE)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2EF;
*rgn=r;return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1608
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2F0=Cyc_Absyn_compress(t);void*_stmttmp4C=_tmp2F0;void*_tmp2F1=_stmttmp4C;void*_tmp2F3;void*_tmp2F2;switch(*((int*)_tmp2F1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F1)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->f1).ptr_atts).nullable;_tmp2F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2F2;void*bounds=_tmp2F3;
# 1616
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1622
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2F4=e->r;void*_stmttmp4D=_tmp2F4;void*_tmp2F5=_stmttmp4D;struct Cyc_Absyn_Exp*_tmp2F7;void*_tmp2F6;struct _fat_ptr _tmp2F8;switch(*((int*)_tmp2F5)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2F8=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp2F8;
# 1630
unsigned long _tmp2F9=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2F9;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}
# 1640
return 0;}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1628
 return 1;case 14U: _LLD: _tmp2F6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1;_tmp2F7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F5)->f2;_LLE: {void*t=_tmp2F6;struct Cyc_Absyn_Exp*e2=_tmp2F7;
# 1641
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1646
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1655
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1660
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1665
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1674
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1679
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1684
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2FA=ka;enum Cyc_Absyn_AliasQual _tmp2FC;enum Cyc_Absyn_KindQual _tmp2FB;_LL1: _tmp2FB=_tmp2FA->kind;_tmp2FC=_tmp2FA->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp2FB;enum Cyc_Absyn_AliasQual a=_tmp2FC;
{enum Cyc_Absyn_AliasQual _tmp2FD=a;switch(_tmp2FD){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1688
enum Cyc_Absyn_KindQual _tmp2FE=k;switch(_tmp2FE){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1699
{enum Cyc_Absyn_KindQual _tmp2FF=k;switch(_tmp2FF){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1706
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1708
{enum Cyc_Absyn_KindQual _tmp300=k;switch(_tmp300){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1715
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1718
({struct Cyc_Warn_String_Warn_Warg_struct _tmp302=({struct Cyc_Warn_String_Warn_Warg_struct _tmp645;_tmp645.tag=0U,({struct _fat_ptr _tmp748=({const char*_tmp304="kind_to_opt: bad kind ";_tag_fat(_tmp304,sizeof(char),23U);});_tmp645.f1=_tmp748;});_tmp645;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp303=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp644;_tmp644.tag=9U,_tmp644.f1=ka;_tmp644;});void*_tmp301[2U];_tmp301[0]=& _tmp302,_tmp301[1]=& _tmp303;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp301,sizeof(void*),2U));});}}
# 1721
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp305=_cycalloc(sizeof(*_tmp305));_tmp305->tag=0U,_tmp305->f1=k;_tmp305;});}
# 1724
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp306=_cycalloc(sizeof(*_tmp306));({void*_tmp749=Cyc_Tcutil_kind_to_bound(k);_tmp306->v=_tmp749;});_tmp306;});}
# 1730
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp307=Cyc_Absyn_compress(t2);void*_stmttmp4E=_tmp307;void*_tmp308=_stmttmp4E;void*_tmp309;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp308)->tag == 3U){_LL1: _tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp308)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp309;
# 1735
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp74A=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp74A;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1748
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1752
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp31D=_cycalloc(sizeof(*_tmp31D));_tmp31D->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp74D=({struct _fat_ptr*_tmp31C=_cycalloc(sizeof(*_tmp31C));({struct _fat_ptr _tmp74C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp31B=({struct Cyc_Int_pa_PrintArg_struct _tmp647;_tmp647.tag=1U,_tmp647.f1=(unsigned long)counter ++;_tmp647;});void*_tmp319[1U];_tmp319[0]=& _tmp31B;({struct _fat_ptr _tmp74B=({const char*_tmp31A="__aliasvar%d";_tag_fat(_tmp31A,sizeof(char),13U);});Cyc_aprintf(_tmp74B,_tag_fat(_tmp319,sizeof(void*),1U));});});*_tmp31C=_tmp74C;});_tmp31C;});_tmp31D->f2=_tmp74D;});_tmp31D;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp74E=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp318=_cycalloc(sizeof(*_tmp318));_tmp318->tag=4U,_tmp318->f1=vd;_tmp318;});Cyc_Absyn_varb_exp(_tmp74E,e->loc);});
# 1761
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp30A=Cyc_Absyn_compress(e_type);void*_stmttmp4F=_tmp30A;void*_tmp30B=_stmttmp4F;struct Cyc_Absyn_PtrLoc*_tmp312;void*_tmp311;void*_tmp310;void*_tmp30F;void*_tmp30E;struct Cyc_Absyn_Tqual _tmp30D;void*_tmp30C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30B)->tag == 3U){_LL1: _tmp30C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30B)->f1).elt_type;_tmp30D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30B)->f1).elt_tq;_tmp30E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30B)->f1).ptr_atts).rgn;_tmp30F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30B)->f1).ptr_atts).nullable;_tmp310=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30B)->f1).ptr_atts).bounds;_tmp311=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30B)->f1).ptr_atts).zero_term;_tmp312=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30B)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp30C;struct Cyc_Absyn_Tqual tq=_tmp30D;void*old_r=_tmp30E;void*nb=_tmp30F;void*b=_tmp310;void*zt=_tmp311;struct Cyc_Absyn_PtrLoc*pl=_tmp312;
# 1764
{void*_tmp313=Cyc_Absyn_compress(old_r);void*_stmttmp50=_tmp313;void*_tmp314=_stmttmp50;struct Cyc_Core_Opt*_tmp316;void**_tmp315;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp314)->tag == 1U){_LL6: _tmp315=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp314)->f2;_tmp316=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp314)->f4;_LL7: {void**topt=_tmp315;struct Cyc_Core_Opt*ts=_tmp316;
# 1766
void*_tmp317=Cyc_Absyn_var_type(tv);void*new_r=_tmp317;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1771
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1774
e->topt=0;
vd->initializer=0;{
# 1778
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1780
return({struct _tuple23 _tmp646;_tmp646.f1=d,_tmp646.f2=ve;_tmp646;});}}
# 1785
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1788
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1793
void*_tmp31E=Cyc_Absyn_compress(wants_type);void*_stmttmp51=_tmp31E;void*_tmp31F=_stmttmp51;void*_tmp320;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31F)->tag == 3U){_LL1: _tmp320=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31F)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp320;
# 1795
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp321=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp321;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1801
return 0;}
# 1805
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1807
void*t1=Cyc_Absyn_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1811
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1814
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1816
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp323=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64C;_tmp64C.tag=0U,({struct _fat_ptr _tmp74F=({const char*_tmp32A="integral size mismatch; ";_tag_fat(_tmp32A,sizeof(char),25U);});_tmp64C.f1=_tmp74F;});_tmp64C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp324=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64B;_tmp64B.tag=2U,_tmp64B.f1=(void*)t1;_tmp64B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp325=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64A;_tmp64A.tag=0U,({
struct _fat_ptr _tmp750=({const char*_tmp329=" -> ";_tag_fat(_tmp329,sizeof(char),5U);});_tmp64A.f1=_tmp750;});_tmp64A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp326=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp649;_tmp649.tag=2U,_tmp649.f1=(void*)t2;_tmp649;});struct Cyc_Warn_String_Warn_Warg_struct _tmp327=({struct Cyc_Warn_String_Warn_Warg_struct _tmp648;_tmp648.tag=0U,({struct _fat_ptr _tmp751=({const char*_tmp328=" conversion supplied";_tag_fat(_tmp328,sizeof(char),21U);});_tmp648.f1=_tmp751;});_tmp648;});void*_tmp322[5U];_tmp322[0]=& _tmp323,_tmp322[1]=& _tmp324,_tmp322[2]=& _tmp325,_tmp322[3]=& _tmp326,_tmp322[4]=& _tmp327;({unsigned _tmp752=e->loc;Cyc_Warn_warn2(_tmp752,_tag_fat(_tmp322,sizeof(void*),5U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1824
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Flags_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp32C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp652;_tmp652.tag=0U,({struct _fat_ptr _tmp753=({const char*_tmp334="implicit alias coercion for ";_tag_fat(_tmp334,sizeof(char),29U);});_tmp652.f1=_tmp753;});_tmp652;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp32D=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp651;_tmp651.tag=4U,_tmp651.f1=e;_tmp651;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp650;_tmp650.tag=0U,({struct _fat_ptr _tmp754=({const char*_tmp333=":";_tag_fat(_tmp333,sizeof(char),2U);});_tmp650.f1=_tmp754;});_tmp650;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp32F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64F;_tmp64F.tag=2U,_tmp64F.f1=(void*)t1;_tmp64F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp330=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64E;_tmp64E.tag=0U,({struct _fat_ptr _tmp755=({const char*_tmp332=" to ";_tag_fat(_tmp332,sizeof(char),5U);});_tmp64E.f1=_tmp755;});_tmp64E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp331=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64D;_tmp64D.tag=2U,_tmp64D.f1=(void*)t2;_tmp64D;});void*_tmp32B[6U];_tmp32B[0]=& _tmp32C,_tmp32B[1]=& _tmp32D,_tmp32B[2]=& _tmp32E,_tmp32B[3]=& _tmp32F,_tmp32B[4]=& _tmp330,_tmp32B[5]=& _tmp331;({unsigned _tmp756=e->loc;Cyc_Warn_warn2(_tmp756,_tag_fat(_tmp32B,sizeof(void*),6U));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1831
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1835
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1839
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp336=({struct Cyc_Warn_String_Warn_Warg_struct _tmp656;_tmp656.tag=0U,({struct _fat_ptr _tmp757=({const char*_tmp33B="implicit cast from ";_tag_fat(_tmp33B,sizeof(char),20U);});_tmp656.f1=_tmp757;});_tmp656;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp337=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp655;_tmp655.tag=2U,_tmp655.f1=(void*)t1;_tmp655;});struct Cyc_Warn_String_Warn_Warg_struct _tmp338=({struct Cyc_Warn_String_Warn_Warg_struct _tmp654;_tmp654.tag=0U,({struct _fat_ptr _tmp758=({const char*_tmp33A=" to ";_tag_fat(_tmp33A,sizeof(char),5U);});_tmp654.f1=_tmp758;});_tmp654;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp339=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp653;_tmp653.tag=2U,_tmp653.f1=(void*)t2;_tmp653;});void*_tmp335[4U];_tmp335[0]=& _tmp336,_tmp335[1]=& _tmp337,_tmp335[2]=& _tmp338,_tmp335[3]=& _tmp339;({unsigned _tmp759=e->loc;Cyc_Warn_warn2(_tmp759,_tag_fat(_tmp335,sizeof(void*),4U));});});
return 1;}
# 1845
return 0;}
# 1851
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1854
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1864
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1866
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1869
struct _tuple24 _tmp33C=*env;struct _tuple24 _stmttmp52=_tmp33C;struct _tuple24 _tmp33D=_stmttmp52;int _tmp340;struct _RegionHandle*_tmp33F;struct Cyc_List_List*_tmp33E;_LL1: _tmp33E=_tmp33D.f1;_tmp33F=_tmp33D.f2;_tmp340=_tmp33D.f3;_LL2: {struct Cyc_List_List*inst=_tmp33E;struct _RegionHandle*r=_tmp33F;int flatten=_tmp340;
void*_tmp341=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp341;
struct Cyc_List_List*_tmp342=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp342;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp344=_region_malloc(r,sizeof(*_tmp344));({struct _tuple12*_tmp75A=({struct _tuple12*_tmp343=_region_malloc(r,sizeof(*_tmp343));_tmp343->f1=x->tq,_tmp343->f2=t;_tmp343;});_tmp344->hd=_tmp75A;}),_tmp344->tl=0;_tmp344;});
return ts;}}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1876
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 1878
struct _tuple25 _tmp345=*env;struct _tuple25 _stmttmp53=_tmp345;struct _tuple25 _tmp346=_stmttmp53;int _tmp348;struct _RegionHandle*_tmp347;_LL1: _tmp347=_tmp346.f1;_tmp348=_tmp346.f2;_LL2: {struct _RegionHandle*r=_tmp347;int flatten=_tmp348;
struct _tuple12 _tmp349=*x;struct _tuple12 _stmttmp54=_tmp349;struct _tuple12 _tmp34A=_stmttmp54;void*_tmp34C;struct Cyc_Absyn_Tqual _tmp34B;_LL4: _tmp34B=_tmp34A.f1;_tmp34C=_tmp34A.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp34B;void*t=_tmp34C;
struct Cyc_List_List*_tmp34D=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp34D;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp34F=_region_malloc(r,sizeof(*_tmp34F));({struct _tuple12*_tmp75B=({struct _tuple12*_tmp34E=_region_malloc(r,sizeof(*_tmp34E));_tmp34E->f1=tq,_tmp34E->f2=t;_tmp34E;});_tmp34F->hd=_tmp75B;}),_tmp34F->tl=0;_tmp34F;});
return ts;}}}
# 1885
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1887
if(flatten){
t1=Cyc_Absyn_compress(t1);{
void*_tmp350=t1;struct Cyc_List_List*_tmp351;struct Cyc_List_List*_tmp352;struct Cyc_List_List*_tmp354;struct Cyc_Absyn_Aggrdecl*_tmp353;switch(*((int*)_tmp350)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp350)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp350)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp353=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp350)->f1)->f1).KnownAggr).val;_tmp354=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp350)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp353;struct Cyc_List_List*ts=_tmp354;
# 1905
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 1907
return({struct Cyc_List_List*_tmp35F=_region_malloc(r,sizeof(*_tmp35F));({struct _tuple12*_tmp75D=({struct _tuple12*_tmp35E=_region_malloc(r,sizeof(*_tmp35E));({struct Cyc_Absyn_Tqual _tmp75C=Cyc_Absyn_empty_tqual(0U);_tmp35E->f1=_tmp75C;}),_tmp35E->f2=t1;_tmp35E;});_tmp35F->hd=_tmp75D;}),_tmp35F->tl=0;_tmp35F;});{
struct Cyc_List_List*_tmp360=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp360;
struct _tuple24 env=({struct _tuple24 _tmp657;_tmp657.f1=inst,_tmp657.f2=r,_tmp657.f3=flatten;_tmp657;});
struct Cyc_List_List*_tmp361=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp55=_tmp361;struct Cyc_List_List*_tmp362=_stmttmp55;struct Cyc_List_List*_tmp364;struct Cyc_Absyn_Aggrfield*_tmp363;if(_tmp362 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp363=(struct Cyc_Absyn_Aggrfield*)_tmp362->hd;_tmp364=_tmp362->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp363;struct Cyc_List_List*tl=_tmp364;
# 1913
struct Cyc_List_List*_tmp365=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp365;
env.f3=0;{
struct Cyc_List_List*_tmp366=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp366;
struct Cyc_List_List*_tmp367=({struct Cyc_List_List*_tmp368=_region_malloc(r,sizeof(*_tmp368));_tmp368->hd=hd2,_tmp368->tl=tl2;_tmp368;});struct Cyc_List_List*tts=_tmp367;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp352=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp350)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp352;
# 1892
struct _tuple25 _tmp355=({struct _tuple25 _tmp658;_tmp658.f1=r,_tmp658.f2=flatten;_tmp658;});struct _tuple25 env=_tmp355;
# 1894
struct Cyc_List_List*_tmp356=tqs;struct Cyc_List_List*_tmp358;struct _tuple12*_tmp357;if(_tmp356 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp357=(struct _tuple12*)_tmp356->hd;_tmp358=_tmp356->tl;_LLF: {struct _tuple12*hd=_tmp357;struct Cyc_List_List*tl=_tmp358;
# 1897
struct Cyc_List_List*_tmp359=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp359;
env.f2=0;{
struct Cyc_List_List*_tmp35A=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp35A;
struct Cyc_List_List*_tmp35B=({struct Cyc_List_List*_tmp35D=_region_malloc(r,sizeof(*_tmp35D));_tmp35D->hd=hd2,_tmp35D->tl=tl2;_tmp35D;});struct Cyc_List_List*temp=_tmp35B;
return({struct _RegionHandle*_tmp75E=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp75E,({struct Cyc_List_List*_tmp35C=_region_malloc(r,sizeof(*_tmp35C));_tmp35C->hd=hd2,_tmp35C->tl=tl2;_tmp35C;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp350)->f1 == Cyc_Absyn_StructA){_LL7: _tmp351=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp350)->f2;_LL8: {struct Cyc_List_List*fs=_tmp351;
# 1920
struct _tuple24 env=({struct _tuple24 _tmp659;_tmp659.f1=0,_tmp659.f2=r,_tmp659.f3=flatten;_tmp659;});
struct Cyc_List_List*_tmp369=fs;struct Cyc_List_List*_tmp36B;struct Cyc_Absyn_Aggrfield*_tmp36A;if(_tmp369 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp36A=(struct Cyc_Absyn_Aggrfield*)_tmp369->hd;_tmp36B=_tmp369->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp36A;struct Cyc_List_List*tl=_tmp36B;
# 1924
struct Cyc_List_List*_tmp36C=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp36C;
env.f3=0;{
struct Cyc_List_List*_tmp36D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp36D;
struct Cyc_List_List*_tmp36E=({struct Cyc_List_List*_tmp36F=_region_malloc(r,sizeof(*_tmp36F));_tmp36F->hd=hd2,_tmp36F->tl=tl2;_tmp36F;});struct Cyc_List_List*tts=_tmp36E;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 1930
 goto _LL0;}_LL0:;}}
# 1933
return({struct Cyc_List_List*_tmp371=_region_malloc(r,sizeof(*_tmp371));({struct _tuple12*_tmp760=({struct _tuple12*_tmp370=_region_malloc(r,sizeof(*_tmp370));({struct Cyc_Absyn_Tqual _tmp75F=Cyc_Absyn_empty_tqual(0U);_tmp370->f1=_tmp75F;}),_tmp370->f2=t1;_tmp370;});_tmp371->hd=_tmp760;}),_tmp371->tl=0;_tmp371;});}
# 1936
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp372=({struct _tuple15 _tmp65A;({void*_tmp762=Cyc_Absyn_compress(t1);_tmp65A.f1=_tmp762;}),({void*_tmp761=Cyc_Absyn_compress(t2);_tmp65A.f2=_tmp761;});_tmp65A;});struct _tuple15 _stmttmp56=_tmp372;struct _tuple15 _tmp373=_stmttmp56;enum Cyc_Absyn_Size_of _tmp375;enum Cyc_Absyn_Size_of _tmp374;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f2)->f1)->tag == 1U){_LL1: _tmp374=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f1)->f1)->f2;_tmp375=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp373.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp374;enum Cyc_Absyn_Size_of b2=_tmp375;
# 1939
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1947
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 1949
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);{
struct _tuple15 _tmp376=({struct _tuple15 _tmp65B;_tmp65B.f1=t1,_tmp65B.f2=t2;_tmp65B;});struct _tuple15 _stmttmp57=_tmp376;struct _tuple15 _tmp377=_stmttmp57;struct Cyc_Absyn_FnInfo _tmp379;struct Cyc_Absyn_FnInfo _tmp378;struct Cyc_List_List*_tmp37E;struct Cyc_Absyn_Datatypedecl*_tmp37D;struct Cyc_List_List*_tmp37C;struct Cyc_Absyn_Datatypefield*_tmp37B;struct Cyc_Absyn_Datatypedecl*_tmp37A;void*_tmp38A;void*_tmp389;void*_tmp388;void*_tmp387;struct Cyc_Absyn_Tqual _tmp386;void*_tmp385;void*_tmp384;void*_tmp383;void*_tmp382;void*_tmp381;struct Cyc_Absyn_Tqual _tmp380;void*_tmp37F;switch(*((int*)_tmp377.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f2)->tag == 3U){_LL1: _tmp37F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->f1).elt_type;_tmp380=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->f1).elt_tq;_tmp381=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->f1).ptr_atts).rgn;_tmp382=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->f1).ptr_atts).nullable;_tmp383=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->f1).ptr_atts).bounds;_tmp384=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f1)->f1).ptr_atts).zero_term;_tmp385=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f2)->f1).elt_type;_tmp386=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f2)->f1).elt_tq;_tmp387=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f2)->f1).ptr_atts).rgn;_tmp388=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f2)->f1).ptr_atts).nullable;_tmp389=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f2)->f1).ptr_atts).bounds;_tmp38A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp377.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp37F;struct Cyc_Absyn_Tqual q_a=_tmp380;void*rt_a=_tmp381;void*null_a=_tmp382;void*b_a=_tmp383;void*zt_a=_tmp384;void*t_b=_tmp385;struct Cyc_Absyn_Tqual q_b=_tmp386;void*rt_b=_tmp387;void*null_b=_tmp388;void*b_b=_tmp389;void*zt_b=_tmp38A;
# 1961
if(q_a.real_const && !q_b.real_const)
return 0;
# 1964
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 1968
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 1972
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 1976
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp38B=({void*_tmp763=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp763,b_a);});struct Cyc_Absyn_Exp*e1=_tmp38B;
struct Cyc_Absyn_Exp*_tmp38C=({void*_tmp764=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp764,b_b);});struct Cyc_Absyn_Exp*e2=_tmp38C;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 1985
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 1990
int _tmp38D=({void*_tmp765=b_b;Cyc_Unify_unify(_tmp765,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);int deep_subtype=_tmp38D;
# 1993
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp769=po;struct Cyc_List_List*_tmp768=({struct Cyc_List_List*_tmp38F=_cycalloc(sizeof(*_tmp38F));({struct _tuple15*_tmp766=({struct _tuple15*_tmp38E=_cycalloc(sizeof(*_tmp38E));_tmp38E->f1=t1,_tmp38E->f2=t2;_tmp38E;});_tmp38F->hd=_tmp766;}),_tmp38F->tl=assume;_tmp38F;});void*_tmp767=t_a;Cyc_Tcutil_ptrsubtype(_tmp769,_tmp768,_tmp767,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp37A=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp37B=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp37C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f1)->f2;_tmp37D=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f2)->f1)->f1).KnownDatatype).val;_tmp37E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp377.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp37A;struct Cyc_Absyn_Datatypefield*df=_tmp37B;struct Cyc_List_List*ts1=_tmp37C;struct Cyc_Absyn_Datatypedecl*dd2=_tmp37D;struct Cyc_List_List*ts2=_tmp37E;
# 2000
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 2002
if(({int _tmp76A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp76A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp377.f2)->tag == 5U){_LL5: _tmp378=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp377.f1)->f1;_tmp379=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp377.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp378;struct Cyc_Absyn_FnInfo f2=_tmp379;
# 2008
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp390=f1.tvars;struct Cyc_List_List*tvs1=_tmp390;
struct Cyc_List_List*_tmp391=f2.tvars;struct Cyc_List_List*tvs2=_tmp391;
if(({int _tmp76B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp76B != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp393=_cycalloc(sizeof(*_tmp393));({struct _tuple19*_tmp76D=({struct _tuple19*_tmp392=_cycalloc(sizeof(*_tmp392));_tmp392->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp76C=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp392->f2=_tmp76C;});_tmp392;});_tmp393->hd=_tmp76D;}),_tmp393->tl=inst;_tmp393;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2019
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp770=po;struct Cyc_List_List*_tmp76F=assume;void*_tmp76E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp394=_cycalloc(sizeof(*_tmp394));_tmp394->tag=5U,_tmp394->f1=f1;_tmp394;});Cyc_Tcutil_subtype(_tmp770,_tmp76F,_tmp76E,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->tag=5U,_tmp395->f1=f2;_tmp395;}));});}}}
# 2026
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp396=f1.args;struct Cyc_List_List*args1=_tmp396;
struct Cyc_List_List*_tmp397=f2.args;struct Cyc_List_List*args2=_tmp397;
# 2031
if(({int _tmp771=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp771 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2033
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp398=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp58=_tmp398;struct _tuple9 _tmp399=_stmttmp58;void*_tmp39B;struct Cyc_Absyn_Tqual _tmp39A;_LLA: _tmp39A=_tmp399.f2;_tmp39B=_tmp399.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp39A;void*t1=_tmp39B;
struct _tuple9 _tmp39C=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp59=_tmp39C;struct _tuple9 _tmp39D=_stmttmp59;void*_tmp39F;struct Cyc_Absyn_Tqual _tmp39E;_LLD: _tmp39E=_tmp39D.f2;_tmp39F=_tmp39D.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp39E;void*t2=_tmp39F;
# 2037
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2041
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp3A0=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp3A0;
struct Cyc_Absyn_VarargInfo _tmp3A1=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp3A1;
# 2046
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2051
if(!({void*_tmp772=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp772,(void*)_check_null(f2.effect));}))return 0;
# 2053
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2055
if(!Cyc_Atts_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2057
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2059
return 0;
# 2061
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2063
return 0;
# 2065
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2076
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2079
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp3A2=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp5A=_tmp3A2;struct _tuple12*_tmp3A3=_stmttmp5A;void*_tmp3A5;struct Cyc_Absyn_Tqual _tmp3A4;_LL1: _tmp3A4=_tmp3A3->f1;_tmp3A5=_tmp3A3->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp3A4;void*t1a=_tmp3A5;
struct _tuple12*_tmp3A6=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp5B=_tmp3A6;struct _tuple12*_tmp3A7=_stmttmp5B;void*_tmp3A9;struct Cyc_Absyn_Tqual _tmp3A8;_LL4: _tmp3A8=_tmp3A7->f1;_tmp3A9=_tmp3A7->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp3A8;void*t2a=_tmp3A9;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2093
return 1;}
# 2098
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Absyn_compress(t1);
t2=Cyc_Absyn_compress(t2);
{void*_tmp3AA=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AA)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2106
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2108
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2112
void*_tmp3AB=t1;void*_tmp3AC;struct Cyc_Absyn_Enumdecl*_tmp3AD;void*_tmp3B1;struct Cyc_Absyn_Exp*_tmp3B0;struct Cyc_Absyn_Tqual _tmp3AF;void*_tmp3AE;void*_tmp3B7;void*_tmp3B6;void*_tmp3B5;void*_tmp3B4;struct Cyc_Absyn_Tqual _tmp3B3;void*_tmp3B2;switch(*((int*)_tmp3AB)){case 3U: _LLA: _tmp3B2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).elt_type;_tmp3B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).elt_tq;_tmp3B4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).ptr_atts).rgn;_tmp3B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).ptr_atts).nullable;_tmp3B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).ptr_atts).bounds;_tmp3B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp3B2;struct Cyc_Absyn_Tqual q_a=_tmp3B3;void*rt_a=_tmp3B4;void*null_a=_tmp3B5;void*b_a=_tmp3B6;void*zt_a=_tmp3B7;
# 2121
{void*_tmp3B8=t2;void*_tmp3BE;void*_tmp3BD;void*_tmp3BC;void*_tmp3BB;struct Cyc_Absyn_Tqual _tmp3BA;void*_tmp3B9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8)->tag == 3U){_LL19: _tmp3B9=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8)->f1).elt_type;_tmp3BA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8)->f1).elt_tq;_tmp3BB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8)->f1).ptr_atts).rgn;_tmp3BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8)->f1).ptr_atts).nullable;_tmp3BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8)->f1).ptr_atts).bounds;_tmp3BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B8)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3B9;struct Cyc_Absyn_Tqual q_b=_tmp3BA;void*rt_b=_tmp3BB;void*null_b=_tmp3BC;void*b_b=_tmp3BD;void*zt_b=_tmp3BE;
# 2123
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3BF=({struct Cyc_List_List*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));({struct _tuple15*_tmp773=({struct _tuple15*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->f1=t1,_tmp3C6->f2=t2;_tmp3C6;});_tmp3C7->hd=_tmp773;}),_tmp3C7->tl=0;_tmp3C7;});struct Cyc_List_List*assump=_tmp3BF;
int _tmp3C0=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3C0;
# 2136 "tcutil.cyc"
int _tmp3C1=
({void*_tmp774=b_b;Cyc_Unify_unify(_tmp774,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2136
int deep_castable=_tmp3C1;
# 2139
int _tmp3C2=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2139
int ptrsub=_tmp3C2;
# 2142
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2144
int _tmp3C3=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2144
int bitcase=_tmp3C3;
# 2147
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3C4=({void*_tmp775=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp775,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3C4;
struct Cyc_Absyn_Exp*_tmp3C5=({void*_tmp776=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp776,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3C5;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2156
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2161
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2167
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp3AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AB)->f1).elt_type;_tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AB)->f1).tq;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AB)->f1).num_elts;_tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AB)->f1).zero_term;_LLD: {void*t1a=_tmp3AE;struct Cyc_Absyn_Tqual tq1a=_tmp3AF;struct Cyc_Absyn_Exp*e1=_tmp3B0;void*zt1=_tmp3B1;
# 2169
{void*_tmp3C8=t2;void*_tmp3CC;struct Cyc_Absyn_Exp*_tmp3CB;struct Cyc_Absyn_Tqual _tmp3CA;void*_tmp3C9;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C8)->tag == 4U){_LL1E: _tmp3C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C8)->f1).elt_type;_tmp3CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C8)->f1).tq;_tmp3CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C8)->f1).num_elts;_tmp3CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C8)->f1).zero_term;_LL1F: {void*t2a=_tmp3C9;struct Cyc_Absyn_Tqual tq2a=_tmp3CA;struct Cyc_Absyn_Exp*e2=_tmp3CB;void*zt2=_tmp3CC;
# 2171
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2176
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2178
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2180
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AB)->f1)){case 15U: _LLE: _tmp3AD=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AB)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp3AD;
# 2184
{void*_tmp3CD=t2;struct Cyc_Absyn_Enumdecl*_tmp3CE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CD)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CD)->f1)->tag == 15U){_LL23: _tmp3CE=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CD)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3CE;
# 2186
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp777=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp777 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2192
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2195
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AB)->f2 != 0){_LL14: _tmp3AC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AB)->f2)->hd;_LL15: {void*r1=_tmp3AC;
# 2198
{void*_tmp3CF=t2;void*_tmp3D0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CF)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CF)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CF)->f2 != 0){_LL28: _tmp3D0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CF)->f2)->hd;_LL29: {void*r2=_tmp3D0;
# 2200
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2204
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2210
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3D1=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3D1;
({void*_tmp778=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->tag=14U,_tmp3D2->f1=t,_tmp3D2->f2=inner,_tmp3D2->f3=0,_tmp3D2->f4=c;_tmp3D2;});e->r=_tmp778;});
e->topt=t;}}
# 2219
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2224
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
return({struct Cyc_Absyn_Tvar*_tmp3D7=_cycalloc(sizeof(*_tmp3D7));({struct _fat_ptr*_tmp77B=({struct _fat_ptr*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _fat_ptr _tmp77A=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D5=({struct Cyc_Int_pa_PrintArg_struct _tmp65C;_tmp65C.tag=1U,_tmp65C.f1=(unsigned long)Cyc_Tcutil_tvar_counter ++;_tmp65C;});void*_tmp3D3[1U];_tmp3D3[0]=& _tmp3D5;({struct _fat_ptr _tmp779=({const char*_tmp3D4="#%d";_tag_fat(_tmp3D4,sizeof(char),4U);});Cyc_aprintf(_tmp779,_tag_fat(_tmp3D3,sizeof(void*),1U));});});*_tmp3D6=_tmp77A;});_tmp3D6;});_tmp3D7->name=_tmp77B;}),_tmp3D7->identity=- 1,_tmp3D7->kind=k;_tmp3D7;});}
# 2228
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
return(int)*((const char*)_check_fat_subscript((t->name)[0],sizeof(char),0U))== (int)'#';}
# 2233
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2240
struct Cyc_List_List*_tmp3D8=0;struct Cyc_List_List*fn_type_atts=_tmp3D8;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Atts_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->hd=(void*)atts->hd,_tmp3D9->tl=fn_type_atts;_tmp3D9;});}}{
struct Cyc_Absyn_FnInfo _tmp3DA=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3DA;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3DB=_cycalloc(sizeof(*_tmp3DB));_tmp3DB->tag=5U,_tmp3DB->f1=type_info;_tmp3DB;});}}
# 2248
return(void*)_check_null(fd->cached_type);}
# 2254
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2256
union Cyc_Relations_RelnOp _tmp3DC=*rop;union Cyc_Relations_RelnOp _stmttmp5C=_tmp3DC;union Cyc_Relations_RelnOp _tmp3DD=_stmttmp5C;struct Cyc_Absyn_Vardecl*_tmp3DE;struct Cyc_Absyn_Vardecl*_tmp3DF;switch((_tmp3DD.RNumelts).tag){case 2U: _LL1: _tmp3DF=(_tmp3DD.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3DF;
# 2258
struct _tuple1 _tmp3E0=*vd->name;struct _tuple1 _stmttmp5D=_tmp3E0;struct _tuple1 _tmp3E1=_stmttmp5D;struct _fat_ptr*_tmp3E3;union Cyc_Absyn_Nmspace _tmp3E2;_LL8: _tmp3E2=_tmp3E1.f1;_tmp3E3=_tmp3E1.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3E2;struct _fat_ptr*var=_tmp3E3;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp77C=(struct _fat_ptr)*var;Cyc_strcmp(_tmp77C,({const char*_tmp3E4="return_value";_tag_fat(_tmp3E4,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp77D=Cyc_Relations_RReturn();*rop=_tmp77D;});
goto _LL0;}{
# 2264
unsigned c=0U;
{struct Cyc_List_List*_tmp3E5=args;struct Cyc_List_List*a=_tmp3E5;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3E6=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp5E=_tmp3E6;struct _tuple9*_tmp3E7=_stmttmp5E;struct _fat_ptr*_tmp3E8;_LLB: _tmp3E8=_tmp3E7->f1;_LLC: {struct _fat_ptr*vopt=_tmp3E8;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp77E=Cyc_Relations_RParam(c);*rop=_tmp77E;});
break;}}}}
# 2272
goto _LL0;}}}case 3U: _LL3: _tmp3DE=(_tmp3DD.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3DE;
# 2274
struct _tuple1 _tmp3E9=*vd->name;struct _tuple1 _stmttmp5F=_tmp3E9;struct _tuple1 _tmp3EA=_stmttmp5F;struct _fat_ptr*_tmp3EC;union Cyc_Absyn_Nmspace _tmp3EB;_LLE: _tmp3EB=_tmp3EA.f1;_tmp3EC=_tmp3EA.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3EB;struct _fat_ptr*var=_tmp3EC;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3ED=args;struct Cyc_List_List*a=_tmp3ED;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3EE=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp60=_tmp3EE;struct _tuple9*_tmp3EF=_stmttmp60;struct _fat_ptr*_tmp3F0;_LL11: _tmp3F0=_tmp3EF->f1;_LL12: {struct _fat_ptr*vopt=_tmp3F0;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp77F=Cyc_Relations_RParamNumelts(c);*rop=_tmp77F;});
break;}}}}
# 2284
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2289
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2291
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2295
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2297
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3F1=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3F1;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2304
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3F2=pr;void*_tmp3F4;struct Cyc_Absyn_Tqual _tmp3F3;_LL1: _tmp3F3=_tmp3F2->f1;_tmp3F4=_tmp3F2->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3F3;void*t2=_tmp3F4;
return t2 == t?pr:({struct _tuple12*_tmp3F5=_cycalloc(sizeof(*_tmp3F5));_tmp3F5->f1=tq,_tmp3F5->f2=t;_tmp3F5;});}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2308
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2310
return({struct _tuple27*_tmp3F7=_region_malloc(rgn,sizeof(*_tmp3F7));({struct _tuple26*_tmp780=({struct _tuple26*_tmp3F6=_region_malloc(rgn,sizeof(*_tmp3F6));_tmp3F6->f1=(*y).f1,_tmp3F6->f2=(*y).f2;_tmp3F6;});_tmp3F7->f1=_tmp780;}),_tmp3F7->f2=(*y).f3;_tmp3F7;});}
# 2312
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2314
struct _tuple9 _tmp3F8=*orig_arg;struct _tuple9 _stmttmp61=_tmp3F8;struct _tuple9 _tmp3F9=_stmttmp61;void*_tmp3FC;struct Cyc_Absyn_Tqual _tmp3FB;struct _fat_ptr*_tmp3FA;_LL1: _tmp3FA=_tmp3F9.f1;_tmp3FB=_tmp3F9.f2;_tmp3FC=_tmp3F9.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp3FA;struct Cyc_Absyn_Tqual tq_orig=_tmp3FB;void*t_orig=_tmp3FC;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->f1=vopt_orig,_tmp3FD->f2=tq_orig,_tmp3FD->f3=t;_tmp3FD;});}}
# 2318
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2322
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2324
return({struct Cyc_Absyn_Exp*_tmp3FE=_cycalloc(sizeof(*_tmp3FE));_tmp3FE->topt=old->topt,_tmp3FE->r=r,_tmp3FE->loc=old->loc,_tmp3FE->annot=old->annot;_tmp3FE;});}
# 2329
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3FF=e->r;void*_stmttmp62=_tmp3FF;void*_tmp400=_stmttmp62;void*_tmp401;struct Cyc_List_List*_tmp403;void*_tmp402;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*_tmp405;void*_tmp406;enum Cyc_Absyn_Coercion _tmp40A;int _tmp409;struct Cyc_Absyn_Exp*_tmp408;void*_tmp407;struct Cyc_Absyn_Exp*_tmp40C;struct Cyc_Absyn_Exp*_tmp40B;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Absyn_Exp*_tmp40D;struct Cyc_Absyn_Exp*_tmp410;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_Absyn_Exp*_tmp411;struct Cyc_List_List*_tmp415;enum Cyc_Absyn_Primop _tmp414;switch(*((int*)_tmp400)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp414=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_tmp415=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp400)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp414;struct Cyc_List_List*es=_tmp415;
# 2338
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp416=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp416;
struct Cyc_Absyn_Exp*_tmp417=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp417;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp782=e;Cyc_Tcutil_copye(_tmp782,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp419=_cycalloc(sizeof(*_tmp419));_tmp419->tag=3U,_tmp419->f1=p,({struct Cyc_List_List*_tmp781=({struct Cyc_Absyn_Exp*_tmp418[1U];_tmp418[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp418,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp419->f2=_tmp781;});_tmp419;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp41A=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp41A;
struct Cyc_Absyn_Exp*_tmp41B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp41B;
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41C;
struct Cyc_Absyn_Exp*_tmp41D=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp41D;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp784=e;Cyc_Tcutil_copye(_tmp784,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp41F=_cycalloc(sizeof(*_tmp41F));_tmp41F->tag=3U,_tmp41F->f1=p,({struct Cyc_List_List*_tmp783=({struct Cyc_Absyn_Exp*_tmp41E[2U];_tmp41E[0]=new_e1,_tmp41E[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp41E,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp41F->f2=_tmp783;});_tmp41F;}));});}}
# 2350
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp421=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65D;_tmp65D.tag=0U,({struct _fat_ptr _tmp785=({const char*_tmp422="primop does not have 1 or 2 args!";_tag_fat(_tmp422,sizeof(char),34U);});_tmp65D.f1=_tmp785;});_tmp65D;});void*_tmp420[1U];_tmp420[0]=& _tmp421;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp420,sizeof(void*),1U));});}case 6U: _LLD: _tmp411=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_tmp412=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp400)->f2;_tmp413=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp400)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp411;struct Cyc_Absyn_Exp*e2=_tmp412;struct Cyc_Absyn_Exp*e3=_tmp413;
# 2352
struct Cyc_Absyn_Exp*_tmp423=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp423;
struct Cyc_Absyn_Exp*_tmp424=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp424;
struct Cyc_Absyn_Exp*_tmp425=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp425;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp786=e;Cyc_Tcutil_copye(_tmp786,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->tag=6U,_tmp426->f1=new_e1,_tmp426->f2=new_e2,_tmp426->f3=new_e3;_tmp426;}));});}case 7U: _LLF: _tmp40F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_tmp410=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp400)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp40F;struct Cyc_Absyn_Exp*e2=_tmp410;
# 2358
struct Cyc_Absyn_Exp*_tmp427=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp427;
struct Cyc_Absyn_Exp*_tmp428=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp428;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp787=e;Cyc_Tcutil_copye(_tmp787,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->tag=7U,_tmp429->f1=new_e1,_tmp429->f2=new_e2;_tmp429;}));});}case 8U: _LL11: _tmp40D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_tmp40E=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp400)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp40D;struct Cyc_Absyn_Exp*e2=_tmp40E;
# 2363
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42A;
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp42B;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp788=e;Cyc_Tcutil_copye(_tmp788,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->tag=8U,_tmp42C->f1=new_e1,_tmp42C->f2=new_e2;_tmp42C;}));});}case 9U: _LL13: _tmp40B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_tmp40C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp400)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp40B;struct Cyc_Absyn_Exp*e2=_tmp40C;
# 2368
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42D;
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp42E;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp789=e;Cyc_Tcutil_copye(_tmp789,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->tag=9U,_tmp42F->f1=new_e1,_tmp42F->f2=new_e2;_tmp42F;}));});}case 14U: _LL15: _tmp407=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_tmp408=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp400)->f2;_tmp409=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp400)->f3;_tmp40A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp400)->f4;_LL16: {void*t=_tmp407;struct Cyc_Absyn_Exp*e1=_tmp408;int b=_tmp409;enum Cyc_Absyn_Coercion c=_tmp40A;
# 2373
struct Cyc_Absyn_Exp*_tmp430=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp430;
void*_tmp431=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp431;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp78A=e;Cyc_Tcutil_copye(_tmp78A,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->tag=14U,_tmp432->f1=new_typ,_tmp432->f2=new_e1,_tmp432->f3=b,_tmp432->f4=c;_tmp432;}));});}case 17U: _LL17: _tmp406=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_LL18: {void*t=_tmp406;
# 2378
void*_tmp433=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp433;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp78B=e;Cyc_Tcutil_copye(_tmp78B,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp434=_cycalloc(sizeof(*_tmp434));_tmp434->tag=17U,_tmp434->f1=new_typ;_tmp434;}));});}case 18U: _LL19: _tmp405=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp405;
# 2381
struct Cyc_Absyn_Exp*_tmp435=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp435;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp78C=e;Cyc_Tcutil_copye(_tmp78C,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp436=_cycalloc(sizeof(*_tmp436));_tmp436->tag=18U,_tmp436->f1=new_e1;_tmp436;}));});}case 41U: _LL1B: _tmp404=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp404;
# 2384
struct Cyc_Absyn_Exp*_tmp437=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp437;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp78D=e;Cyc_Tcutil_copye(_tmp78D,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->tag=41U,_tmp438->f1=new_e1;_tmp438;}));});}case 19U: _LL1D: _tmp402=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_tmp403=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp400)->f2;_LL1E: {void*t=_tmp402;struct Cyc_List_List*f=_tmp403;
# 2387
void*_tmp439=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp439;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp78E=e;Cyc_Tcutil_copye(_tmp78E,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->tag=19U,_tmp43A->f1=new_typ,_tmp43A->f2=f;_tmp43A;}));});}case 39U: _LL1F: _tmp401=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp400)->f1;_LL20: {void*t=_tmp401;
# 2390
void*_tmp43B=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp43B;
if(new_typ == t)return e;{
# 2393
void*_tmp43C=Cyc_Absyn_compress(new_typ);void*_stmttmp63=_tmp43C;void*_tmp43D=_stmttmp63;struct Cyc_Absyn_Exp*_tmp43E;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43D)->tag == 9U){_LL24: _tmp43E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43D)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp43E;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp78F=e;Cyc_Tcutil_copye(_tmp78F,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=39U,_tmp43F->f1=new_typ;_tmp43F;}));});}_LL23:;}}default: _LL21: _LL22:
# 2397
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp441=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65E;_tmp65E.tag=0U,({struct _fat_ptr _tmp790=({const char*_tmp442="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp442,sizeof(char),46U);});_tmp65E.f1=_tmp790;});_tmp65E;});void*_tmp440[1U];_tmp440[0]=& _tmp441;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp440,sizeof(void*),1U));});}_LL0:;}
# 2401
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2404
return e == 0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2407
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2410
void*_tmp443=f->type;void*t=_tmp443;
struct Cyc_Absyn_Exp*_tmp444=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp444;
void*_tmp445=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp445;
struct Cyc_Absyn_Exp*_tmp446=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp446;
if(t == new_typ && req == new_req)
return f;{
struct Cyc_Absyn_Aggrfield*_tmp447=({struct Cyc_Absyn_Aggrfield*_tmp448=_cycalloc(sizeof(*_tmp448));*_tmp448=*f;_tmp448;});struct Cyc_Absyn_Aggrfield*ans=_tmp447;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2422
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2425
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp449=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp449;
struct Cyc_List_List*_tmp44A=fs->tl;struct Cyc_List_List*t=_tmp44A;
struct Cyc_Absyn_Aggrfield*_tmp44B=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp44B;
struct Cyc_List_List*_tmp44C=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_typ=_tmp44C;
if(new_f == f && new_typ == t)
return fs;
return({struct Cyc_List_List*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->hd=new_f,_tmp44D->tl=new_typ;_tmp44D;});}}
# 2435
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2438
struct _tuple0 _tmp44E=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp64=_tmp44E;struct _tuple0 _tmp44F=_stmttmp64;struct Cyc_List_List*_tmp451;struct Cyc_List_List*_tmp450;_LL1: _tmp450=_tmp44F.f1;_tmp451=_tmp44F.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp450;struct Cyc_List_List*rpo1b=_tmp451;
struct Cyc_List_List*_tmp452=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp452;
struct Cyc_List_List*_tmp453=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp453;
if(rpo2a == rpo1a && rpo2b == rpo1b)
return rgn_po;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}
# 2448
static int Cyc_Tcutil_skip_fntypes=0;
# 2450
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2453
void*_tmp454=Cyc_Absyn_compress(t);void*_stmttmp65=_tmp454;void*_tmp455=_stmttmp65;struct Cyc_Absyn_Exp*_tmp456;struct Cyc_Absyn_Exp*_tmp457;struct Cyc_List_List*_tmp459;void*_tmp458;void*_tmp45A;struct Cyc_List_List*_tmp45C;enum Cyc_Absyn_AggrKind _tmp45B;struct Cyc_List_List*_tmp45D;struct Cyc_Absyn_Vardecl*_tmp469;struct Cyc_Absyn_Exp*_tmp468;struct Cyc_Absyn_Exp*_tmp467;struct Cyc_List_List*_tmp466;struct Cyc_List_List*_tmp465;struct Cyc_Absyn_VarargInfo*_tmp464;int _tmp463;struct Cyc_List_List*_tmp462;void*_tmp461;struct Cyc_Absyn_Tqual _tmp460;void*_tmp45F;struct Cyc_List_List*_tmp45E;void*_tmp46F;void*_tmp46E;void*_tmp46D;void*_tmp46C;struct Cyc_Absyn_Tqual _tmp46B;void*_tmp46A;unsigned _tmp474;void*_tmp473;struct Cyc_Absyn_Exp*_tmp472;struct Cyc_Absyn_Tqual _tmp471;void*_tmp470;void*_tmp478;struct Cyc_Absyn_Typedefdecl*_tmp477;struct Cyc_List_List*_tmp476;struct _tuple1*_tmp475;struct Cyc_Absyn_Tvar*_tmp479;switch(*((int*)_tmp455)){case 2U: _LL1: _tmp479=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp455)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp479;
# 2456
struct _handler_cons _tmp47A;_push_handler(& _tmp47A);{int _tmp47C=0;if(setjmp(_tmp47A.handler))_tmp47C=1;if(!_tmp47C){{void*_tmp47D=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp47D;};_pop_handler();}else{void*_tmp47B=(void*)Cyc_Core_get_exn_thrown();void*_tmp47E=_tmp47B;void*_tmp47F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp47E)->tag == Cyc_Core_Not_found){_LL1A: _LL1B:
 return t;}else{_LL1C: _tmp47F=_tmp47E;_LL1D: {void*exn=_tmp47F;(int)_rethrow(exn);}}_LL19:;}}}case 8U: _LL3: _tmp475=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp455)->f1;_tmp476=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp455)->f2;_tmp477=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp455)->f3;_tmp478=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp455)->f4;_LL4: {struct _tuple1*n=_tmp475;struct Cyc_List_List*ts=_tmp476;struct Cyc_Absyn_Typedefdecl*td=_tmp477;void*topt=_tmp478;
# 2459
struct Cyc_List_List*_tmp480=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp480;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->tag=8U,_tmp481->f1=n,_tmp481->f2=new_ts,_tmp481->f3=td,_tmp481->f4=topt;_tmp481;});}case 4U: _LL5: _tmp470=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp455)->f1).elt_type;_tmp471=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp455)->f1).tq;_tmp472=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp455)->f1).num_elts;_tmp473=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp455)->f1).zero_term;_tmp474=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp455)->f1).zt_loc;_LL6: {void*t1=_tmp470;struct Cyc_Absyn_Tqual tq=_tmp471;struct Cyc_Absyn_Exp*e=_tmp472;void*zt=_tmp473;unsigned ztl=_tmp474;
# 2462
void*_tmp482=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp482;
struct Cyc_Absyn_Exp*_tmp483=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp483;
void*_tmp484=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp484;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp485=_cycalloc(sizeof(*_tmp485));
_tmp485->tag=4U,(_tmp485->f1).elt_type=new_t1,(_tmp485->f1).tq=tq,(_tmp485->f1).num_elts=new_e,(_tmp485->f1).zero_term=new_zt,(_tmp485->f1).zt_loc=ztl;_tmp485;});}case 3U: _LL7: _tmp46A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp455)->f1).elt_type;_tmp46B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp455)->f1).elt_tq;_tmp46C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp455)->f1).ptr_atts).rgn;_tmp46D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp455)->f1).ptr_atts).nullable;_tmp46E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp455)->f1).ptr_atts).bounds;_tmp46F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp455)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp46A;struct Cyc_Absyn_Tqual tq=_tmp46B;void*r=_tmp46C;void*n=_tmp46D;void*b=_tmp46E;void*zt=_tmp46F;
# 2468
void*_tmp486=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp486;
void*_tmp487=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp487;
void*_tmp488=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp488;
void*_tmp489=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp489;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp65F;_tmp65F.elt_type=new_t1,_tmp65F.elt_tq=tq,(_tmp65F.ptr_atts).rgn=new_r,(_tmp65F.ptr_atts).nullable=n,(_tmp65F.ptr_atts).bounds=new_b,(_tmp65F.ptr_atts).zero_term=new_zt,(_tmp65F.ptr_atts).ptrloc=0;_tmp65F;}));}case 5U: _LL9: _tmp45E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).tvars;_tmp45F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).effect;_tmp460=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).ret_tqual;_tmp461=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).ret_type;_tmp462=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).args;_tmp463=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).c_varargs;_tmp464=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).cyc_varargs;_tmp465=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).rgn_po;_tmp466=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).attributes;_tmp467=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).requires_clause;_tmp468=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).ensures_clause;_tmp469=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp455)->f1).return_value;_LLA: {struct Cyc_List_List*vs=_tmp45E;void*eff=_tmp45F;struct Cyc_Absyn_Tqual rtq=_tmp460;void*rtyp=_tmp461;struct Cyc_List_List*args=_tmp462;int c_varargs=_tmp463;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp464;struct Cyc_List_List*rgn_po=_tmp465;struct Cyc_List_List*atts=_tmp466;struct Cyc_Absyn_Exp*req=_tmp467;struct Cyc_Absyn_Exp*ens=_tmp468;struct Cyc_Absyn_Vardecl*ret_value=_tmp469;
# 2477
if(Cyc_Tcutil_skip_fntypes)return t;
# 2479
{struct Cyc_List_List*_tmp48A=vs;struct Cyc_List_List*p=_tmp48A;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp48C=_region_malloc(rgn,sizeof(*_tmp48C));({struct _tuple19*_tmp792=({struct _tuple19*_tmp48B=_region_malloc(rgn,sizeof(*_tmp48B));_tmp48B->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp791=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp48B->f2=_tmp791;});_tmp48B;});_tmp48C->hd=_tmp792;}),_tmp48C->tl=inst;_tmp48C;});}}{
struct _tuple0 _tmp48D=({struct _RegionHandle*_tmp794=rgn;struct _RegionHandle*_tmp793=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp794,_tmp793,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2481
struct _tuple0 _stmttmp66=_tmp48D;struct _tuple0 _tmp48E=_stmttmp66;struct Cyc_List_List*_tmp490;struct Cyc_List_List*_tmp48F;_LL1F: _tmp48F=_tmp48E.f1;_tmp490=_tmp48E.f2;_LL20: {struct Cyc_List_List*qs=_tmp48F;struct Cyc_List_List*ts=_tmp490;
# 2483
struct Cyc_List_List*_tmp491=args;struct Cyc_List_List*args2=_tmp491;
struct Cyc_List_List*_tmp492=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp492;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff == 0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2489
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2493
struct Cyc_Absyn_VarargInfo _tmp493=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp67=_tmp493;struct Cyc_Absyn_VarargInfo _tmp494=_stmttmp67;int _tmp498;void*_tmp497;struct Cyc_Absyn_Tqual _tmp496;struct _fat_ptr*_tmp495;_LL22: _tmp495=_tmp494.name;_tmp496=_tmp494.tq;_tmp497=_tmp494.type;_tmp498=_tmp494.inject;_LL23: {struct _fat_ptr*n=_tmp495;struct Cyc_Absyn_Tqual tq=_tmp496;void*t=_tmp497;int i=_tmp498;
void*_tmp499=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp499;
if(t2 == t)cyc_varargs2=cyc_varargs;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A->name=n,_tmp49A->tq=tq,_tmp49A->type=t2,_tmp49A->inject=i;_tmp49A;});}}}{
# 2498
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp49B=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp49B;
struct Cyc_List_List*_tmp49C=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp49C;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->tag=5U,(_tmp49D->f1).tvars=vs,(_tmp49D->f1).effect=eff2,(_tmp49D->f1).ret_tqual=rtq,({void*_tmp795=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp49D->f1).ret_type=_tmp795;}),(_tmp49D->f1).args=args2,(_tmp49D->f1).c_varargs=c_varargs,(_tmp49D->f1).cyc_varargs=cyc_varargs2,(_tmp49D->f1).rgn_po=rgn_po2,(_tmp49D->f1).attributes=atts,(_tmp49D->f1).requires_clause=req2,(_tmp49D->f1).requires_relns=req_relns2,(_tmp49D->f1).ensures_clause=ens2,(_tmp49D->f1).ensures_relns=ens_relns2,(_tmp49D->f1).return_value=ret_value;_tmp49D;});}}}}}case 6U: _LLB: _tmp45D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp455)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp45D;
# 2507
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp49E=tqts;struct Cyc_List_List*ts1=_tmp49E;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp49F=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp49F;
void*_tmp4A0=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp4A0;
if(t1 != t2)
change=1;
ts2=({struct Cyc_List_List*_tmp4A1=_region_malloc(rgn,sizeof(*_tmp4A1));_tmp4A1->hd=t2,_tmp4A1->tl=ts2;_tmp4A1;});}}
# 2516
return !change?t:(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A2=_cycalloc(sizeof(*_tmp4A2));_tmp4A2->tag=6U,({struct Cyc_List_List*_tmp797=({struct Cyc_List_List*_tmp796=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp796,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp4A2->f1=_tmp797;});_tmp4A2;});}case 7U: _LLD: _tmp45B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp455)->f1;_tmp45C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp455)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp45B;struct Cyc_List_List*fs=_tmp45C;
# 2518
struct Cyc_List_List*_tmp4A3=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp4A3;
return fs == new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->tag=7U,_tmp4A4->f1=k,_tmp4A4->f2=new_fs;_tmp4A4;});}case 1U: _LLF: _tmp45A=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp455)->f2;_LL10: {void*r=_tmp45A;
# 2521
return r == 0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0U: _LL11: _tmp458=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp455)->f1;_tmp459=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp455)->f2;_LL12: {void*c=_tmp458;struct Cyc_List_List*ts=_tmp459;
# 2523
struct Cyc_List_List*_tmp4A5=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp4A5;
return ts == new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->tag=0U,_tmp4A6->f1=c,_tmp4A6->f2=new_ts;_tmp4A6;});}case 9U: _LL13: _tmp457=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp455)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp457;
# 2526
struct Cyc_Absyn_Exp*_tmp4A7=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4A7;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4A8=_cycalloc(sizeof(*_tmp4A8));_tmp4A8->tag=9U,_tmp4A8->f1=new_e;_tmp4A8;});}case 11U: _LL15: _tmp456=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp455)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp456;
# 2529
struct Cyc_Absyn_Exp*_tmp4A9=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4A9;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp4AA=_cycalloc(sizeof(*_tmp4AA));_tmp4AA->tag=11U,_tmp4AA->f1=new_e;_tmp4AA;});}default: _LL17: _LL18:
# 2532
 if(Cyc_Tcutil_skip_fntypes)return t;else{
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp660;_tmp660.tag=0U,({struct _fat_ptr _tmp798=({const char*_tmp4AD="found typedecltype in rsubs";_tag_fat(_tmp4AD,sizeof(char),28U);});_tmp660.f1=_tmp798;});_tmp660;});void*_tmp4AB[1U];_tmp4AB[0]=& _tmp4AC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4AB,sizeof(void*),1U));});}}_LL0:;}
# 2537
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2540
if(ts == 0)
return 0;{
void*_tmp4AE=(void*)ts->hd;void*old_hd=_tmp4AE;
struct Cyc_List_List*_tmp4AF=ts->tl;struct Cyc_List_List*old_tl=_tmp4AF;
void*_tmp4B0=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp4B0;
struct Cyc_List_List*_tmp4B1=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp4B1;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp4B2=_cycalloc(sizeof(*_tmp4B2));_tmp4B2->hd=new_hd,_tmp4B2->tl=new_tl;_tmp4B2;});}}
# 2551
void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return inst == 0?t: Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}
# 2555
void*Cyc_Tcutil_substitute_nofun(struct Cyc_List_List*inst,void*t){
Cyc_Tcutil_skip_fntypes=1;{
void*_tmp4B3=Cyc_Tcutil_substitute(inst,t);void*new_t=_tmp4B3;
Cyc_Tcutil_skip_fntypes=0;
return new_t;}}
# 2563
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4B4=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4B4;
return({struct _tuple19*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->f1=tv,({void*_tmp79A=({struct Cyc_Core_Opt*_tmp799=k;Cyc_Absyn_new_evar(_tmp799,({struct Cyc_Core_Opt*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->v=s;_tmp4B5;}));});_tmp4B6->f2=_tmp79A;});_tmp4B6;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2568
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2570
struct _tuple28*_tmp4B7=env;struct _RegionHandle*_tmp4B9;struct Cyc_List_List*_tmp4B8;_LL1: _tmp4B8=_tmp4B7->f1;_tmp4B9=_tmp4B7->f2;_LL2: {struct Cyc_List_List*s=_tmp4B8;struct _RegionHandle*rgn=_tmp4B9;
struct Cyc_Core_Opt*_tmp4BA=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4BA;
return({struct _tuple19*_tmp4BC=_region_malloc(rgn,sizeof(*_tmp4BC));_tmp4BC->f1=tv,({void*_tmp79C=({struct Cyc_Core_Opt*_tmp79B=k;Cyc_Absyn_new_evar(_tmp79B,({struct Cyc_Core_Opt*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->v=s;_tmp4BB;}));});_tmp4BC->f2=_tmp79C;});_tmp4BC;});}}
# 2575
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2577
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp663;_tmp663.tag=0U,({struct _fat_ptr _tmp79D=({const char*_tmp4C2="bitfield ";_tag_fat(_tmp4C2,sizeof(char),10U);});_tmp663.f1=_tmp79D;});_tmp663;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp662;_tmp662.tag=0U,_tmp662.f1=*fn;_tmp662;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp661;_tmp661.tag=0U,({struct _fat_ptr _tmp79E=({const char*_tmp4C1=" does not have constant width";_tag_fat(_tmp4C1,sizeof(char),30U);});_tmp661.f1=_tmp79E;});_tmp661;});void*_tmp4BD[3U];_tmp4BD[0]=& _tmp4BE,_tmp4BD[1]=& _tmp4BF,_tmp4BD[2]=& _tmp4C0;({unsigned _tmp79F=loc;Cyc_Warn_err2(_tmp79F,_tag_fat(_tmp4BD,sizeof(void*),3U));});});else{
# 2582
struct _tuple13 _tmp4C3=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp68=_tmp4C3;struct _tuple13 _tmp4C4=_stmttmp68;int _tmp4C6;unsigned _tmp4C5;_LL1: _tmp4C5=_tmp4C4.f1;_tmp4C6=_tmp4C4.f2;_LL2: {unsigned i=_tmp4C5;int known=_tmp4C6;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp664;_tmp664.tag=0U,({struct _fat_ptr _tmp7A0=({const char*_tmp4C9="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4C9,sizeof(char),47U);});_tmp664.f1=_tmp7A0;});_tmp664;});void*_tmp4C7[1U];_tmp4C7[0]=& _tmp4C8;({unsigned _tmp7A1=loc;Cyc_Warn_warn2(_tmp7A1,_tag_fat(_tmp4C7,sizeof(void*),1U));});});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp665;_tmp665.tag=0U,({struct _fat_ptr _tmp7A2=({const char*_tmp4CC="bitfield has negative width";_tag_fat(_tmp4CC,sizeof(char),28U);});_tmp665.f1=_tmp7A2;});_tmp665;});void*_tmp4CA[1U];_tmp4CA[0]=& _tmp4CB;({unsigned _tmp7A3=loc;Cyc_Warn_err2(_tmp7A3,_tag_fat(_tmp4CA,sizeof(void*),1U));});});
w=i;}}{
# 2589
void*_tmp4CD=Cyc_Absyn_compress(field_type);void*_stmttmp69=_tmp4CD;void*_tmp4CE=_stmttmp69;enum Cyc_Absyn_Size_of _tmp4CF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4CE)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4CE)->f1)->tag == 1U){_LL4: _tmp4CF=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4CE)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4CF;
# 2592
int bad;
{enum Cyc_Absyn_Size_of _tmp4D0=b;switch(_tmp4D0){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 bad=w > (unsigned)8;goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 bad=w > (unsigned)16;goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 bad=w > (unsigned)32;goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 bad=w > (unsigned)64;goto _LL8;}_LL8:;}
# 2601
if(bad)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp666;_tmp666.tag=0U,({struct _fat_ptr _tmp7A4=({const char*_tmp4D3="bitfield larger than type";_tag_fat(_tmp4D3,sizeof(char),26U);});_tmp666.f1=_tmp7A4;});_tmp666;});void*_tmp4D1[1U];_tmp4D1[0]=& _tmp4D2;({unsigned _tmp7A5=loc;Cyc_Warn_warn2(_tmp7A5,_tag_fat(_tmp4D1,sizeof(void*),1U));});});
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2605
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66A;_tmp66A.tag=0U,({struct _fat_ptr _tmp7A6=({const char*_tmp4DA="bitfield ";_tag_fat(_tmp4DA,sizeof(char),10U);});_tmp66A.f1=_tmp7A6;});_tmp66A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4D6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp669;_tmp669.tag=0U,_tmp669.f1=*fn;_tmp669;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4D7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp668;_tmp668.tag=0U,({struct _fat_ptr _tmp7A7=({const char*_tmp4D9=" must have integral type but has type ";_tag_fat(_tmp4D9,sizeof(char),39U);});_tmp668.f1=_tmp7A7;});_tmp668;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4D8=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp667;_tmp667.tag=2U,_tmp667.f1=(void*)field_type;_tmp667;});void*_tmp4D4[4U];_tmp4D4[0]=& _tmp4D5,_tmp4D4[1]=& _tmp4D6,_tmp4D4[2]=& _tmp4D7,_tmp4D4[3]=& _tmp4D8;({unsigned _tmp7A8=loc;Cyc_Warn_err2(_tmp7A8,_tag_fat(_tmp4D4,sizeof(void*),4U));});});
# 2607
goto _LL3;}_LL3:;}}}
# 2614
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4DB=t;void*_tmp4DD;struct Cyc_Absyn_Typedefdecl*_tmp4DC;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DB)->tag == 8U){_LL1: _tmp4DC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DB)->f3;_tmp4DD=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4DB)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4DC;void*tdopt=_tmp4DD;
# 2617
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66B;_tmp66B.tag=0U,({struct _fat_ptr _tmp7A9=({const char*_tmp4E0="extra const";_tag_fat(_tmp4E0,sizeof(char),12U);});_tmp66B.f1=_tmp7A9;});_tmp66B;});void*_tmp4DE[1U];_tmp4DE[0]=& _tmp4DF;({unsigned _tmp7AA=loc;Cyc_Warn_warn2(_tmp7AA,_tag_fat(_tmp4DE,sizeof(void*),1U));});});
return 1;}
# 2622
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2629
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp7AB=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp7AB;});}
# 2633
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2639
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2642
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66E;_tmp66E.tag=0U,_tmp66E.f1=msg;_tmp66E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66D;_tmp66D.tag=0U,({struct _fat_ptr _tmp7AC=({const char*_tmp4E5=": ";_tag_fat(_tmp4E5,sizeof(char),3U);});_tmp66D.f1=_tmp7AC;});_tmp66D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66C;_tmp66C.tag=0U,({struct _fat_ptr _tmp7AD=a2string(vs->hd);_tmp66C.f1=_tmp7AD;});_tmp66C;});void*_tmp4E1[3U];_tmp4E1[0]=& _tmp4E2,_tmp4E1[1]=& _tmp4E3,_tmp4E1[2]=& _tmp4E4;({unsigned _tmp7AE=loc;Cyc_Warn_err2(_tmp7AE,_tag_fat(_tmp4E1,sizeof(void*),3U));});});}}}
# 2647
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2650
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2653
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp7B0=tvs;unsigned _tmp7AF=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp7B0,_tmp7AF,Cyc_Absynpp_tvar2string,({const char*_tmp4E6="duplicate type variable";_tag_fat(_tmp4E6,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2668 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2673
struct _RegionHandle _tmp4E7=_new_region("temp");struct _RegionHandle*temp=& _tmp4E7;_push_region(temp);
# 2677
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp7B1=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp7B1,({const char*_tmp4E8="";_tag_fat(_tmp4E8,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4EA=_region_malloc(temp,sizeof(*_tmp4EA));({struct _tuple29*_tmp7B2=({struct _tuple29*_tmp4E9=_region_malloc(temp,sizeof(*_tmp4E9));_tmp4E9->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4E9->f2=0;_tmp4E9;});_tmp4EA->hd=_tmp7B2;}),_tmp4EA->tl=fields;_tmp4EA;});}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2683
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp516="struct";_tag_fat(_tmp516,sizeof(char),7U);}):({const char*_tmp517="union";_tag_fat(_tmp517,sizeof(char),6U);});
# 2686
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4EB=(struct _tuple30*)des->hd;struct _tuple30*_stmttmp6A=_tmp4EB;struct _tuple30*_tmp4EC=_stmttmp6A;void*_tmp4EE;struct Cyc_List_List*_tmp4ED;_LL1: _tmp4ED=_tmp4EC->f1;_tmp4EE=_tmp4EC->f2;_LL2: {struct Cyc_List_List*dl=_tmp4ED;void*a=_tmp4EE;
if(dl == 0){
# 2691
struct Cyc_List_List*_tmp4EF=fields;struct Cyc_List_List*fields2=_tmp4EF;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple29*)fields2->hd)).f2){
(*((struct _tuple29*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp7B4=({struct Cyc_List_List*_tmp4F1=_cycalloc(sizeof(*_tmp4F1));({void*_tmp7B3=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4F0=_cycalloc(sizeof(*_tmp4F0));_tmp4F0->tag=1U,_tmp4F0->f1=((*((struct _tuple29*)fields2->hd)).f1)->name;_tmp4F0;});_tmp4F1->hd=_tmp7B3;}),_tmp4F1->tl=0;_tmp4F1;});(*((struct _tuple30*)des->hd)).f1=_tmp7B4;});
ans=({struct Cyc_List_List*_tmp4F3=_region_malloc(rgn,sizeof(*_tmp4F3));({struct _tuple31*_tmp7B5=({struct _tuple31*_tmp4F2=_region_malloc(rgn,sizeof(*_tmp4F2));_tmp4F2->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4F2->f2=a;_tmp4F2;});_tmp4F3->hd=_tmp7B5;}),_tmp4F3->tl=ans;_tmp4F3;});
break;}}
# 2699
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp670;_tmp670.tag=0U,({struct _fat_ptr _tmp7B6=({const char*_tmp4F7="too many arguments to ";_tag_fat(_tmp4F7,sizeof(char),23U);});_tmp670.f1=_tmp7B6;});_tmp670;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66F;_tmp66F.tag=0U,_tmp66F.f1=aggr_str;_tmp66F;});void*_tmp4F4[2U];_tmp4F4[0]=& _tmp4F5,_tmp4F4[1]=& _tmp4F6;({unsigned _tmp7B7=loc;Cyc_Warn_err2(_tmp7B7,_tag_fat(_tmp4F4,sizeof(void*),2U));});});}else{
if(dl->tl != 0)
# 2703
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp671;_tmp671.tag=0U,({struct _fat_ptr _tmp7B8=({const char*_tmp4FA="multiple designators are not yet supported";_tag_fat(_tmp4FA,sizeof(char),43U);});_tmp671.f1=_tmp7B8;});_tmp671;});void*_tmp4F8[1U];_tmp4F8[0]=& _tmp4F9;({unsigned _tmp7B9=loc;Cyc_Warn_err2(_tmp7B9,_tag_fat(_tmp4F8,sizeof(void*),1U));});});else{
# 2706
void*_tmp4FB=(void*)dl->hd;void*_stmttmp6B=_tmp4FB;void*_tmp4FC=_stmttmp6B;struct _fat_ptr*_tmp4FD;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4FC)->tag == 0U){_LL4: _LL5:
# 2708
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp673;_tmp673.tag=0U,({struct _fat_ptr _tmp7BA=({const char*_tmp501="array designator used in argument to ";_tag_fat(_tmp501,sizeof(char),38U);});_tmp673.f1=_tmp7BA;});_tmp673;});struct Cyc_Warn_String_Warn_Warg_struct _tmp500=({struct Cyc_Warn_String_Warn_Warg_struct _tmp672;_tmp672.tag=0U,_tmp672.f1=aggr_str;_tmp672;});void*_tmp4FE[2U];_tmp4FE[0]=& _tmp4FF,_tmp4FE[1]=& _tmp500;({unsigned _tmp7BB=loc;Cyc_Warn_err2(_tmp7BB,_tag_fat(_tmp4FE,sizeof(void*),2U));});});
goto _LL3;}else{_LL6: _tmp4FD=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4FC)->f1;_LL7: {struct _fat_ptr*v=_tmp4FD;
# 2711
struct Cyc_List_List*_tmp502=fields;struct Cyc_List_List*fields2=_tmp502;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple29*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple29*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp504=({struct Cyc_Warn_String_Warn_Warg_struct _tmp676;_tmp676.tag=0U,({struct _fat_ptr _tmp7BC=({const char*_tmp508="member ";_tag_fat(_tmp508,sizeof(char),8U);});_tmp676.f1=_tmp7BC;});_tmp676;});struct Cyc_Warn_String_Warn_Warg_struct _tmp505=({struct Cyc_Warn_String_Warn_Warg_struct _tmp675;_tmp675.tag=0U,_tmp675.f1=*v;_tmp675;});struct Cyc_Warn_String_Warn_Warg_struct _tmp506=({struct Cyc_Warn_String_Warn_Warg_struct _tmp674;_tmp674.tag=0U,({struct _fat_ptr _tmp7BD=({const char*_tmp507=" has already been used as an argument";_tag_fat(_tmp507,sizeof(char),38U);});_tmp674.f1=_tmp7BD;});_tmp674;});void*_tmp503[3U];_tmp503[0]=& _tmp504,_tmp503[1]=& _tmp505,_tmp503[2]=& _tmp506;({unsigned _tmp7BE=loc;Cyc_Warn_err2(_tmp7BE,_tag_fat(_tmp503,sizeof(void*),3U));});});
(*((struct _tuple29*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp50A=_region_malloc(rgn,sizeof(*_tmp50A));({struct _tuple31*_tmp7BF=({struct _tuple31*_tmp509=_region_malloc(rgn,sizeof(*_tmp509));_tmp509->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp509->f2=a;_tmp509;});_tmp50A->hd=_tmp7BF;}),_tmp50A->tl=ans;_tmp50A;});
break;}}
# 2720
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp678;_tmp678.tag=0U,({struct _fat_ptr _tmp7C0=({const char*_tmp50E="bad field designator ";_tag_fat(_tmp50E,sizeof(char),22U);});_tmp678.f1=_tmp7C0;});_tmp678;});struct Cyc_Warn_String_Warn_Warg_struct _tmp50D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp677;_tmp677.tag=0U,_tmp677.f1=*v;_tmp677;});void*_tmp50B[2U];_tmp50B[0]=& _tmp50C,_tmp50B[1]=& _tmp50D;({unsigned _tmp7C1=loc;Cyc_Warn_err2(_tmp7C1,_tag_fat(_tmp50B,sizeof(void*),2U));});});
goto _LL3;}}_LL3:;}}}}
# 2727
if((int)aggr_kind == (int)0U){
if(({int _tmp7C2=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans);_tmp7C2 < ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp510=({struct Cyc_Warn_String_Warn_Warg_struct _tmp679;_tmp679.tag=0U,({struct _fat_ptr _tmp7C3=({const char*_tmp511="too few arguments to struct";_tag_fat(_tmp511,sizeof(char),28U);});_tmp679.f1=_tmp7C3;});_tmp679;});void*_tmp50F[1U];_tmp50F[0]=& _tmp510;({unsigned _tmp7C4=loc;Cyc_Warn_err2(_tmp7C4,_tag_fat(_tmp50F,sizeof(void*),1U));});});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp513=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67A;_tmp67A.tag=0U,({struct _fat_ptr _tmp7C5=({const char*_tmp514="union expression should have one member";_tag_fat(_tmp514,sizeof(char),40U);});_tmp67A.f1=_tmp7C5;});_tmp67A;});void*_tmp512[1U];_tmp512[0]=& _tmp513;({unsigned _tmp7C6=loc;Cyc_Warn_err2(_tmp7C6,_tag_fat(_tmp512,sizeof(void*),1U));});});}{
struct Cyc_List_List*_tmp515=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp515;}}}
# 2677
;_pop_region();}
# 2738
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2740
void*_tmp518=e1->r;void*_stmttmp6C=_tmp518;void*_tmp519=_stmttmp6C;struct Cyc_Absyn_Exp*_tmp51A;struct Cyc_Absyn_Exp*_tmp51B;struct Cyc_Absyn_Exp*_tmp51C;struct Cyc_Absyn_Exp*_tmp51D;struct Cyc_Absyn_Exp*_tmp51E;struct Cyc_Absyn_Exp*_tmp51F;switch(*((int*)_tmp519)){case 14U: _LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp521=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67C;_tmp67C.tag=0U,({struct _fat_ptr _tmp7C7=({const char*_tmp523="we have a cast in a lhs: ";_tag_fat(_tmp523,sizeof(char),26U);});_tmp67C.f1=_tmp7C7;});_tmp67C;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp522=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp67B;_tmp67B.tag=4U,_tmp67B.f1=e1;_tmp67B;});void*_tmp520[2U];_tmp520[0]=& _tmp521,_tmp520[1]=& _tmp522;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp520,sizeof(void*),2U));});case 20U: _LL3: _tmp51F=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp519)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp51F;
_tmp51E=e1a;goto _LL6;}case 23U: _LL5: _tmp51E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp519)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp51E;
# 2744
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp51D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp519)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp51D;
_tmp51C=e1a;goto _LLA;}case 21U: _LL9: _tmp51C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp519)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp51C;
# 2748
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp525=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67E;_tmp67E.tag=0U,({struct _fat_ptr _tmp7C8=({const char*_tmp527="found zero pointer aggregate member assignment: ";_tag_fat(_tmp527,sizeof(char),49U);});_tmp67E.f1=_tmp7C8;});_tmp67E;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp526=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp67D;_tmp67D.tag=4U,_tmp67D.f1=e1;_tmp67D;});void*_tmp524[2U];_tmp524[0]=& _tmp525,_tmp524[1]=& _tmp526;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp524,sizeof(void*),2U));});
return 0;}case 13U: _LLB: _tmp51B=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp519)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp51B;
_tmp51A=e1a;goto _LLE;}case 12U: _LLD: _tmp51A=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp519)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp51A;
# 2753
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp529=({struct Cyc_Warn_String_Warn_Warg_struct _tmp680;_tmp680.tag=0U,({struct _fat_ptr _tmp7C9=({const char*_tmp52B="found zero pointer instantiate/noinstantiate: ";_tag_fat(_tmp52B,sizeof(char),47U);});_tmp680.f1=_tmp7C9;});_tmp680;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp52A=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp67F;_tmp67F.tag=4U,_tmp67F.f1=e1;_tmp67F;});void*_tmp528[2U];_tmp528[0]=& _tmp529,_tmp528[1]=& _tmp52A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp528,sizeof(void*),2U));});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp682;_tmp682.tag=0U,({struct _fat_ptr _tmp7CA=({const char*_tmp52F="found bad lhs in is_zero_ptr_deref: ";_tag_fat(_tmp52F,sizeof(char),37U);});_tmp682.f1=_tmp7CA;});_tmp682;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp52E=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp681;_tmp681.tag=4U,_tmp681.f1=e1;_tmp681;});void*_tmp52C[2U];_tmp52C[0]=& _tmp52D,_tmp52C[1]=& _tmp52E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp52C,sizeof(void*),2U));});}_LL0:;}
# 2767
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmp530=Cyc_Absyn_compress(r);void*_stmttmp6D=_tmp530;void*_tmp531=_stmttmp6D;struct Cyc_Absyn_Tvar*_tmp532;enum Cyc_Absyn_AliasQual _tmp534;enum Cyc_Absyn_KindQual _tmp533;switch(*((int*)_tmp531)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp531)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp531)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp531)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp531)->f4 == 0){_LL5: _tmp533=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp531)->f3)->kind)->v)->kind;_tmp534=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp531)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp533;enum Cyc_Absyn_AliasQual a=_tmp534;
# 2772
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp532=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp531)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp532;
# 2776
struct Cyc_Absyn_Kind*_tmp535=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp6E=_tmp535;struct Cyc_Absyn_Kind*_tmp536=_stmttmp6E;enum Cyc_Absyn_AliasQual _tmp538;enum Cyc_Absyn_KindQual _tmp537;_LLC: _tmp537=_tmp536->kind;_tmp538=_tmp536->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp537;enum Cyc_Absyn_AliasQual a=_tmp538;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp539=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp6F=_tmp539;void*_tmp53A=_stmttmp6F;struct Cyc_Core_Opt**_tmp53B;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp53A)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp53A)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp53A)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp53B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp53A)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp53B;
# 2780
({struct Cyc_Core_Opt*_tmp7CC=({struct Cyc_Core_Opt*_tmp53D=_cycalloc(sizeof(*_tmp53D));({void*_tmp7CB=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp53C=_cycalloc(sizeof(*_tmp53C));_tmp53C->tag=2U,_tmp53C->f1=0,_tmp53C->f2=& Cyc_Tcutil_rk;_tmp53C;});_tmp53D->v=_tmp7CB;});_tmp53D;});*x=_tmp7CC;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2785
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2792
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp53E=Cyc_Absyn_compress(t);void*_stmttmp70=_tmp53E;void*_tmp53F=_stmttmp70;struct Cyc_Absyn_Tvar*_tmp540;void*_tmp541;switch(*((int*)_tmp53F)){case 3U: _LL1: _tmp541=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp53F)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp541;
# 2795
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp540=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53F)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp540;
# 2797
struct Cyc_Absyn_Kind*_tmp542=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp71=_tmp542;struct Cyc_Absyn_Kind*_tmp543=_stmttmp71;enum Cyc_Absyn_AliasQual _tmp545;enum Cyc_Absyn_KindQual _tmp544;_LL8: _tmp544=_tmp543->kind;_tmp545=_tmp543->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp544;enum Cyc_Absyn_AliasQual a=_tmp545;
enum Cyc_Absyn_KindQual _tmp546=k;switch(_tmp546){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp547=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp72=_tmp547;void*_tmp548=_stmttmp72;enum Cyc_Absyn_KindQual _tmp54A;struct Cyc_Core_Opt**_tmp549;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp548)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp548)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp549=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp548)->f1;_tmp54A=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp548)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp549;enum Cyc_Absyn_KindQual k=_tmp54A;
# 2803
({struct Cyc_Core_Opt*_tmp7CF=({struct Cyc_Core_Opt*_tmp54D=_cycalloc(sizeof(*_tmp54D));({void*_tmp7CE=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp54C=_cycalloc(sizeof(*_tmp54C));_tmp54C->tag=2U,_tmp54C->f1=0,({struct Cyc_Absyn_Kind*_tmp7CD=({struct Cyc_Absyn_Kind*_tmp54B=_cycalloc(sizeof(*_tmp54B));_tmp54B->kind=k,_tmp54B->aliasqual=Cyc_Absyn_Aliasable;_tmp54B;});_tmp54C->f2=_tmp7CD;});_tmp54C;});_tmp54D->v=_tmp7CE;});_tmp54D;});*x=_tmp7CF;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
 return 1;}_LL13:;}
# 2808
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2811
 return 0;}_LL0:;}
# 2814
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
t=Cyc_Absyn_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(t,0))
return 1;{
void*_tmp54E=t;struct Cyc_List_List*_tmp54F;struct Cyc_List_List*_tmp551;union Cyc_Absyn_DatatypeFieldInfo _tmp550;struct Cyc_List_List*_tmp553;struct Cyc_Absyn_Aggrdecl**_tmp552;struct Cyc_List_List*_tmp554;switch(*((int*)_tmp54E)){case 6U: _LL1: _tmp554=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54E)->f1;_LL2: {struct Cyc_List_List*qts=_tmp554;
# 2820
for(0;qts != 0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54E)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54E)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp552=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54E)->f1)->f1).KnownAggr).val;_tmp553=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54E)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp552;struct Cyc_List_List*ts=_tmp553;
# 2825
if((*adp)->impl == 0)
return 0;{
struct Cyc_List_List*_tmp555=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp555;
{struct Cyc_List_List*_tmp556=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp556;for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type)))
return 1;}}
return 0;}}}else{_LL7: _LL8:
# 2837
 goto _LLA;}case 18U: _LL9: _LLA:
 return 0;case 19U: _LLB: _tmp550=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54E)->f1)->f1;_tmp551=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp54E)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp550;struct Cyc_List_List*ts=_tmp551;
# 2841
union Cyc_Absyn_DatatypeFieldInfo _tmp557=tinfo;struct Cyc_Absyn_Datatypefield*_tmp559;struct Cyc_Absyn_Datatypedecl*_tmp558;if((_tmp557.UnknownDatatypefield).tag == 1){_LL10: _LL11:
 return 0;}else{_LL12: _tmp558=((_tmp557.KnownDatatypefield).val).f1;_tmp559=((_tmp557.KnownDatatypefield).val).f2;_LL13: {struct Cyc_Absyn_Datatypedecl*td=_tmp558;struct Cyc_Absyn_Datatypefield*fld=_tmp559;
# 2844
struct Cyc_List_List*_tmp55A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp55A;
{struct Cyc_List_List*_tmp55B=fld->typs;struct Cyc_List_List*typs=_tmp55B;for(0;typs != 0;typs=typs->tl){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 2850
return 0;}}_LLF:;}default: goto _LLD;}case 7U: _LL5: _tmp54F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp54E)->f2;_LL6: {struct Cyc_List_List*x=_tmp54F;
# 2833
for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD: _LLE:
# 2852
 return 0;}_LL0:;}}
# 2859
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp55C=e->r;void*_stmttmp73=_tmp55C;void*_tmp55D=_stmttmp73;struct Cyc_Absyn_Stmt*_tmp55E;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp562;struct Cyc_Absyn_Exp*_tmp561;struct Cyc_Absyn_Exp*_tmp564;struct Cyc_Absyn_Exp*_tmp563;struct _fat_ptr*_tmp566;struct Cyc_Absyn_Exp*_tmp565;struct Cyc_Absyn_Exp*_tmp567;struct Cyc_Absyn_Exp*_tmp568;switch(*((int*)_tmp55D)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp55D)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp568=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55D)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp568;
_tmp567=e1;goto _LL6;}case 20U: _LL5: _tmp567=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp55D)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp567;
# 2864
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp565=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55D)->f1;_tmp566=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55D)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp565;struct _fat_ptr*f=_tmp566;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp563=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55D)->f1;_tmp564=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55D)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp563;struct Cyc_Absyn_Exp*e2=_tmp564;
# 2867
void*_tmp569=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp74=_tmp569;void*_tmp56A=_stmttmp74;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp56A)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp561=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp55D)->f2;_tmp562=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp55D)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp561;struct Cyc_Absyn_Exp*e2=_tmp562;
# 2872
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp560=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp55D)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp560;
_tmp55F=e2;goto _LL10;}case 14U: _LLF: _tmp55F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp55D)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp55F;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp55E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp55D)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp55E;
# 2876
while(1){
void*_tmp56B=s->r;void*_stmttmp75=_tmp56B;void*_tmp56C=_stmttmp75;struct Cyc_Absyn_Exp*_tmp56D;struct Cyc_Absyn_Stmt*_tmp56F;struct Cyc_Absyn_Decl*_tmp56E;struct Cyc_Absyn_Stmt*_tmp571;struct Cyc_Absyn_Stmt*_tmp570;switch(*((int*)_tmp56C)){case 2U: _LL1B: _tmp570=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp571=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp570;struct Cyc_Absyn_Stmt*s2=_tmp571;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp56E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_tmp56F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp56C)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp56E;struct Cyc_Absyn_Stmt*s1=_tmp56F;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp56D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp56C)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp56D;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp573=({struct Cyc_Warn_String_Warn_Warg_struct _tmp683;_tmp683.tag=0U,({struct _fat_ptr _tmp7D0=({const char*_tmp574="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp574,sizeof(char),40U);});_tmp683.f1=_tmp7D0;});_tmp683;});void*_tmp572[1U];_tmp572[0]=& _tmp573;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp572,sizeof(void*),1U));});}_LL1A:;}}default: _LL13: _LL14:
# 2884
 return 1;}_LL0:;}
# 2901 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 2903
struct _tuple14 bogus_ans=({struct _tuple14 _tmp691;_tmp691.f1=0,_tmp691.f2=Cyc_Absyn_heap_rgn_type;_tmp691;});
void*_tmp575=e->r;void*_stmttmp76=_tmp575;void*_tmp576=_stmttmp76;struct Cyc_Absyn_Exp*_tmp578;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_Absyn_Exp*_tmp579;int _tmp57C;struct _fat_ptr*_tmp57B;struct Cyc_Absyn_Exp*_tmp57A;int _tmp57F;struct _fat_ptr*_tmp57E;struct Cyc_Absyn_Exp*_tmp57D;void*_tmp580;switch(*((int*)_tmp576)){case 1U: _LL1: _tmp580=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_LL2: {void*x=_tmp580;
# 2907
void*_tmp581=x;struct Cyc_Absyn_Vardecl*_tmp582;struct Cyc_Absyn_Vardecl*_tmp583;struct Cyc_Absyn_Vardecl*_tmp584;struct Cyc_Absyn_Vardecl*_tmp585;switch(*((int*)_tmp581)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp585=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp581)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp585;
# 2911
void*_tmp586=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp77=_tmp586;void*_tmp587=_stmttmp77;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp587)->tag == 4U){_LL1B: _LL1C:
# 2913
 return({struct _tuple14 _tmp684;_tmp684.f1=1,_tmp684.f2=Cyc_Absyn_heap_rgn_type;_tmp684;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp685;_tmp685.f1=(vd->tq).real_const,_tmp685.f2=Cyc_Absyn_heap_rgn_type;_tmp685;});}_LL1A:;}case 4U: _LL14: _tmp584=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp581)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp584;
# 2917
void*_tmp588=Cyc_Absyn_compress((void*)_check_null(e->topt));void*_stmttmp78=_tmp588;void*_tmp589=_stmttmp78;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp589)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp686;_tmp686.f1=1,_tmp686.f2=(void*)_check_null(vd->rgn);_tmp686;});}else{_LL22: _LL23:
# 2920
 vd->escapes=1;
return({struct _tuple14 _tmp687;_tmp687.f1=(vd->tq).real_const,_tmp687.f2=(void*)_check_null(vd->rgn);_tmp687;});}_LL1F:;}case 5U: _LL16: _tmp583=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp581)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp583;
# 2923
_tmp582=vd;goto _LL19;}default: _LL18: _tmp582=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp581)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp582;
# 2925
vd->escapes=1;
return({struct _tuple14 _tmp688;_tmp688.f1=(vd->tq).real_const,_tmp688.f2=(void*)_check_null(vd->rgn);_tmp688;});}}_LLD:;}case 21U: _LL3: _tmp57D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_tmp57E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp576)->f2;_tmp57F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp576)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp57D;struct _fat_ptr*f=_tmp57E;int is_tagged=_tmp57F;
# 2930
if(is_tagged)return bogus_ans;{
# 2933
void*_tmp58A=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp79=_tmp58A;void*_tmp58B=_stmttmp79;struct Cyc_Absyn_Aggrdecl*_tmp58C;struct Cyc_List_List*_tmp58D;switch(*((int*)_tmp58B)){case 7U: _LL25: _tmp58D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp58B)->f2;_LL26: {struct Cyc_List_List*fs=_tmp58D;
# 2935
struct Cyc_Absyn_Aggrfield*_tmp58E=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp58E;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp58F=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7A=_tmp58F;struct _tuple14 _tmp590=_stmttmp7A;void*_tmp592;int _tmp591;_LL2C: _tmp591=_tmp590.f1;_tmp592=_tmp590.f2;_LL2D: {int c=_tmp591;void*t=_tmp592;
return({struct _tuple14 _tmp689;_tmp689.f1=(finfo->tq).real_const || c,_tmp689.f2=t;_tmp689;});}}
# 2940
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58B)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58B)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp58C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp58B)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp58C;
# 2942
struct Cyc_Absyn_Aggrfield*_tmp593=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp593;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp594=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7B=_tmp594;struct _tuple14 _tmp595=_stmttmp7B;void*_tmp597;int _tmp596;_LL2F: _tmp596=_tmp595.f1;_tmp597=_tmp595.f2;_LL30: {int c=_tmp596;void*t=_tmp597;
return({struct _tuple14 _tmp68A;_tmp68A.f1=(finfo->tq).real_const || c,_tmp68A.f2=t;_tmp68A;});}}
# 2947
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp57A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_tmp57B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp576)->f2;_tmp57C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp576)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp57A;struct _fat_ptr*f=_tmp57B;int is_tagged=_tmp57C;
# 2952
if(is_tagged)return bogus_ans;{
# 2955
void*_tmp598=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp7C=_tmp598;void*_tmp599=_stmttmp7C;void*_tmp59B;void*_tmp59A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp599)->tag == 3U){_LL32: _tmp59A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp599)->f1).elt_type;_tmp59B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp599)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp59A;void*r=_tmp59B;
# 2957
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp59C=Cyc_Absyn_compress(t1);void*_stmttmp7D=_tmp59C;void*_tmp59D=_stmttmp7D;struct Cyc_Absyn_Aggrdecl*_tmp59E;struct Cyc_List_List*_tmp59F;switch(*((int*)_tmp59D)){case 7U: _LL37: _tmp59F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp59D)->f2;_LL38: {struct Cyc_List_List*fs=_tmp59F;
# 2960
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59D)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp59E=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59D)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp59E;
# 2962
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 2965
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp68B;_tmp68B.f1=(finfo->tq).real_const,_tmp68B.f2=r;_tmp68B;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp579=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp579;
# 2972
void*_tmp5A0=Cyc_Absyn_compress((void*)_check_null(e1->topt));void*_stmttmp7E=_tmp5A0;void*_tmp5A1=_stmttmp7E;void*_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A1)->tag == 3U){_LL3E: _tmp5A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A1)->f1).elt_tq;_tmp5A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A1)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp5A2;void*r=_tmp5A3;
# 2974
return({struct _tuple14 _tmp68C;_tmp68C.f1=tq.real_const,_tmp68C.f2=r;_tmp68C;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp577=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp576)->f1;_tmp578=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp576)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp577;struct Cyc_Absyn_Exp*e2=_tmp578;
# 2980
void*t=Cyc_Absyn_compress((void*)_check_null(e1->topt));
void*_tmp5A4=t;struct Cyc_Absyn_Tqual _tmp5A5;void*_tmp5A7;struct Cyc_Absyn_Tqual _tmp5A6;struct Cyc_List_List*_tmp5A8;switch(*((int*)_tmp5A4)){case 6U: _LL43: _tmp5A8=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5A4)->f1;_LL44: {struct Cyc_List_List*ts=_tmp5A8;
# 2984
struct _tuple13 _tmp5A9=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp7F=_tmp5A9;struct _tuple13 _tmp5AA=_stmttmp7F;int _tmp5AC;unsigned _tmp5AB;_LL4C: _tmp5AB=_tmp5AA.f1;_tmp5AC=_tmp5AA.f2;_LL4D: {unsigned i=_tmp5AB;int known=_tmp5AC;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp5AD=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp5AD;
if(finfo != 0)
return({struct _tuple14 _tmp68D;_tmp68D.f1=((*finfo).f1).real_const,({void*_tmp7D1=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp68D.f2=_tmp7D1;});_tmp68D;});
return bogus_ans;}}}case 3U: _LL45: _tmp5A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).elt_tq;_tmp5A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A4)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp5A6;void*r=_tmp5A7;
# 2992
return({struct _tuple14 _tmp68E;_tmp68E.f1=tq.real_const,_tmp68E.f2=r;_tmp68E;});}case 4U: _LL47: _tmp5A5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5A4)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp5A5;
# 2998
return({struct _tuple14 _tmp68F;_tmp68F.f1=tq.real_const,({void*_tmp7D2=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp68F.f2=_tmp7D2;});_tmp68F;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3002
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp690;_tmp690.tag=0U,({struct _fat_ptr _tmp7D3=({const char*_tmp5B0="unary & applied to non-lvalue";_tag_fat(_tmp5B0,sizeof(char),30U);});_tmp690.f1=_tmp7D3;});_tmp690;});void*_tmp5AE[1U];_tmp5AE[0]=& _tmp5AF;({unsigned _tmp7D4=e->loc;Cyc_Warn_err2(_tmp7D4,_tag_fat(_tmp5AE,sizeof(void*),1U));});});
return bogus_ans;}_LL0:;}
# 3009
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp5B1=({void*_tmp7D5=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7D5,b);});struct Cyc_Absyn_Exp*eopt=_tmp5B1;
if(eopt == 0)return;{
struct _tuple13 _tmp5B2=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp80=_tmp5B2;struct _tuple13 _tmp5B3=_stmttmp80;int _tmp5B5;unsigned _tmp5B4;_LL1: _tmp5B4=_tmp5B3.f1;_tmp5B5=_tmp5B3.f2;_LL2: {unsigned j=_tmp5B4;int known=_tmp5B5;
if(known && j <= i)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5B7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp695;_tmp695.tag=0U,({struct _fat_ptr _tmp7D6=({const char*_tmp5BC="a dereference is out of bounds: ";_tag_fat(_tmp5BC,sizeof(char),33U);});_tmp695.f1=_tmp7D6;});_tmp695;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp5B8=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp694;_tmp694.tag=12U,_tmp694.f1=(int)j;_tmp694;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp693;_tmp693.tag=0U,({
struct _fat_ptr _tmp7D7=({const char*_tmp5BB=" <= ";_tag_fat(_tmp5BB,sizeof(char),5U);});_tmp693.f1=_tmp7D7;});_tmp693;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp5BA=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp692;_tmp692.tag=12U,_tmp692.f1=(int)i;_tmp692;});void*_tmp5B6[4U];_tmp5B6[0]=& _tmp5B7,_tmp5B6[1]=& _tmp5B8,_tmp5B6[2]=& _tmp5B9,_tmp5B6[3]=& _tmp5BA;({void(*_tmp7D9)(unsigned,struct _fat_ptr)=
# 3014
do_warn?Cyc_Warn_warn2: Cyc_Warn_err2;unsigned _tmp7D8=loc;_tmp7D9(_tmp7D8,_tag_fat(_tmp5B6,sizeof(void*),4U));});});
# 3016
return;}}}
# 3019
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3027
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
int(*_tmp5BD)(int var_okay,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_cnst_exp;int(*rec)(int var_okay,struct Cyc_Absyn_Exp*e)=_tmp5BD;
void*_tmp5BE=e->r;void*_stmttmp81=_tmp5BE;void*_tmp5BF=_stmttmp81;struct Cyc_List_List*_tmp5C0;struct Cyc_List_List*_tmp5C1;struct Cyc_List_List*_tmp5C2;struct Cyc_List_List*_tmp5C3;struct Cyc_List_List*_tmp5C4;struct Cyc_List_List*_tmp5C5;struct Cyc_Absyn_Exp*_tmp5C6;struct Cyc_Absyn_Exp*_tmp5C8;struct Cyc_Absyn_Exp*_tmp5C7;struct Cyc_Absyn_Exp*_tmp5C9;struct Cyc_Absyn_Exp*_tmp5CA;struct Cyc_Absyn_Exp*_tmp5CB;struct Cyc_Absyn_Exp*_tmp5CC;struct Cyc_Absyn_Exp*_tmp5CD;struct Cyc_Absyn_Exp*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5CE;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D0;void*_tmp5D3;switch(*((int*)_tmp5BF)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5D3=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL10: {void*b=_tmp5D3;
# 3040
void*_tmp5D4=b;struct Cyc_Absyn_Vardecl*_tmp5D5;struct Cyc_Absyn_Vardecl*_tmp5D6;switch(*((int*)_tmp5D4)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmp5D6=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5D4)->f1;_LL35: {struct Cyc_Absyn_Vardecl*vd=_tmp5D6;
# 3043
void*_tmp5D7=Cyc_Absyn_compress(vd->type);void*_stmttmp82=_tmp5D7;void*_tmp5D8=_stmttmp82;switch(*((int*)_tmp5D8)){case 4U: _LL3D: _LL3E:
 goto _LL40;case 5U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmp5D5=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5D4)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5D5;
# 3049
if((int)vd->sc != (int)Cyc_Absyn_Static)
return var_okay;{
void*_tmp5D9=Cyc_Absyn_compress(vd->type);void*_stmttmp83=_tmp5D9;void*_tmp5DA=_stmttmp83;switch(*((int*)_tmp5DA)){case 4U: _LL44: _LL45:
 goto _LL47;case 5U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}}case 0U: _LL38: _LL39:
# 3056
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmp5D0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_tmp5D1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5BF)->f2;_tmp5D2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5BF)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5D0;struct Cyc_Absyn_Exp*e2=_tmp5D1;struct Cyc_Absyn_Exp*e3=_tmp5D2;
# 3060
return(rec(0,e1)&& rec(0,e2))&& rec(0,e3);}case 9U: _LL13: _tmp5CE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_tmp5CF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5BF)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5CE;struct Cyc_Absyn_Exp*e2=_tmp5CF;
return rec(0,e1)&& rec(0,e2);}case 41U: _LL15: _tmp5CD=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5CD;
_tmp5CC=e2;goto _LL18;}case 12U: _LL17: _tmp5CC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5CC;
_tmp5CB=e2;goto _LL1A;}case 13U: _LL19: _tmp5CB=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5CB;
_tmp5CA=e2;goto _LL1C;}case 14U: _LL1B: _tmp5CA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5BF)->f2;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp5CA;
# 3067
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 15U: _LL1D: _tmp5C9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL1E: {struct Cyc_Absyn_Exp*e2=_tmp5C9;
return rec(1,e2);}case 27U: _LL1F: _tmp5C7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BF)->f2;_tmp5C8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5BF)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp5C7;struct Cyc_Absyn_Exp*e2=_tmp5C8;
return rec(0,e1)&& rec(0,e2);}case 28U: _LL21: _tmp5C6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp5C6;
return rec(0,e);}case 26U: _LL23: _tmp5C5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL24: {struct Cyc_List_List*des=_tmp5C5;
_tmp5C4=des;goto _LL26;}case 30U: _LL25: _tmp5C4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5BF)->f2;_LL26: {struct Cyc_List_List*des=_tmp5C4;
_tmp5C3=des;goto _LL28;}case 29U: _LL27: _tmp5C3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5BF)->f3;_LL28: {struct Cyc_List_List*des=_tmp5C3;
# 3074
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL29: _tmp5C2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5BF)->f2;_LL2A: {struct Cyc_List_List*es=_tmp5C2;
_tmp5C1=es;goto _LL2C;}case 24U: _LL2B: _tmp5C1=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL2C: {struct Cyc_List_List*es=_tmp5C1;
_tmp5C0=es;goto _LL2E;}case 31U: _LL2D: _tmp5C0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5BF)->f1;_LL2E: {struct Cyc_List_List*es=_tmp5C0;
# 3081
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 3088
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3092
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5DB=Cyc_Absyn_compress(t);void*_stmttmp84=_tmp5DB;void*_tmp5DC=_stmttmp84;struct Cyc_List_List*_tmp5DD;struct Cyc_List_List*_tmp5DE;void*_tmp5DF;void*_tmp5E0;struct Cyc_List_List*_tmp5E2;void*_tmp5E1;switch(*((int*)_tmp5DC)){case 0U: _LL1: _tmp5E1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5DC)->f1;_tmp5E2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5DC)->f2;_LL2: {void*c=_tmp5E1;struct Cyc_List_List*ts=_tmp5E2;
# 3096
void*_tmp5E3=c;union Cyc_Absyn_AggrInfo _tmp5E4;struct Cyc_List_List*_tmp5E5;struct Cyc_Absyn_Enumdecl*_tmp5E6;switch(*((int*)_tmp5E3)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5E6=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5E3)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5E6;
# 3101
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5E5=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5E5=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5E3)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5E5;
# 3105
{struct Cyc_List_List*_tmp5E7=fs;struct Cyc_List_List*fs2=_tmp5E7;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5E8=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp85=_tmp5E8;struct _tuple13 _tmp5E9=_stmttmp85;int _tmp5EB;unsigned _tmp5EA;_LL1D: _tmp5EA=_tmp5E9.f1;_tmp5EB=_tmp5E9.f2;_LL1E: {unsigned i=_tmp5EA;int known=_tmp5EB;
if(known && i == (unsigned)0)
return 1;}}}}
# 3112
return 0;}case 20U: _LL18: _tmp5E4=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5E3)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5E4;
# 3115
struct Cyc_Absyn_Aggrdecl*_tmp5EC=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5EC;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5E0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DC)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5E0;
# 3123
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DC)->f1).elt_type;_LL6: {void*t=_tmp5DF;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5DE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5DC)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5DE;
# 3126
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7U: _LL9: _tmp5DD=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5DC)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5DD;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3134
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3136
struct _RegionHandle _tmp5ED=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5ED;_push_region(rgn);
{struct Cyc_List_List*_tmp5EE=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5EE;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5EF=0;_npop_handler(0U);return _tmp5EF;}}{
# 3144
int _tmp5F0=1;_npop_handler(0U);return _tmp5F0;}}
# 3137
;_pop_region();}
# 3148
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5F1=t;struct Cyc_Absyn_Typedefdecl*_tmp5F2;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5F1)->tag == 8U){_LL1: _tmp5F2=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5F1)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5F2;
# 3151
if(tdopt == 0)
return;{
struct Cyc_Absyn_Tqual _tmp5F3=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5F3;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp698;_tmp698.tag=0U,({struct _fat_ptr _tmp7DA=({const char*_tmp5F9="qualifier within typedef type ";_tag_fat(_tmp5F9,sizeof(char),31U);});_tmp698.f1=_tmp7DA;});_tmp698;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp5F6=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp697;_tmp697.tag=2U,_tmp697.f1=(void*)t;_tmp697;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5F7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp696;_tmp696.tag=0U,({struct _fat_ptr _tmp7DB=({const char*_tmp5F8=" is ignored";_tag_fat(_tmp5F8,sizeof(char),12U);});_tmp696.f1=_tmp7DB;});_tmp696;});void*_tmp5F4[3U];_tmp5F4[0]=& _tmp5F5,_tmp5F4[1]=& _tmp5F6,_tmp5F4[2]=& _tmp5F7;({unsigned _tmp7DC=loc;Cyc_Warn_warn2(_tmp7DC,_tag_fat(_tmp5F4,sizeof(void*),3U));});});
return;}}}else{_LL3: _LL4:
 return;}_LL0:;}
# 3162
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5FA=Cyc_Absyn_compress(t);void*_stmttmp86=_tmp5FA;void*_tmp5FB=_stmttmp86;struct Cyc_Absyn_Exp*_tmp5FC;struct Cyc_Absyn_PtrInfo _tmp5FD;switch(*((int*)_tmp5FB)){case 3U: _LL1: _tmp5FD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5FB)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp5FD;
return({void*_tmp7DD=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7DD,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp5FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5FB)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5FC;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3172
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5FE=b;struct Cyc_Absyn_Vardecl*_tmp5FF;struct Cyc_Absyn_Vardecl*_tmp600;struct Cyc_Absyn_Vardecl*_tmp601;struct Cyc_Absyn_Vardecl*_tmp602;switch(*((int*)_tmp5FE)){case 5U: _LL1: _tmp602=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5FE)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp602;
_tmp601=x;goto _LL4;}case 4U: _LL3: _tmp601=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5FE)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp601;
_tmp600=x;goto _LL6;}case 3U: _LL5: _tmp600=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5FE)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp600;
_tmp5FF=x;goto _LL8;}case 1U: _LL7: _tmp5FF=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5FE)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp5FF;
return x->escapes?0: x;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3183
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp603=0;struct Cyc_List_List*res=_tmp603;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)res=({struct Cyc_List_List*_tmp604=_cycalloc(sizeof(*_tmp604));_tmp604->hd=*((void**)_check_null((void**)x->hd)),_tmp604->tl=res;_tmp604;});}}
return res;}
# 3190
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp605=Cyc_Absyn_compress(t);void*_stmttmp87=_tmp605;void*_tmp606=_stmttmp87;unsigned _tmp60B;void*_tmp60A;struct Cyc_Absyn_Exp*_tmp609;struct Cyc_Absyn_Tqual _tmp608;void*_tmp607;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp606)->tag == 4U){_LL1: _tmp607=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp606)->f1).elt_type;_tmp608=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp606)->f1).tq;_tmp609=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp606)->f1).num_elts;_tmp60A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp606)->f1).zero_term;_tmp60B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp606)->f1).zt_loc;_LL2: {void*et=_tmp607;struct Cyc_Absyn_Tqual tq=_tmp608;struct Cyc_Absyn_Exp*eopt=_tmp609;void*zt=_tmp60A;unsigned ztl=_tmp60B;
# 3193
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
if(convert_tag){
if(eopt->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp60D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp699;_tmp699.tag=0U,({struct _fat_ptr _tmp7DE=({const char*_tmp60E="cannot convert tag without type!";_tag_fat(_tmp60E,sizeof(char),33U);});_tmp699.f1=_tmp7DE;});_tmp699;});void*_tmp60C[1U];_tmp60C[0]=& _tmp60D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp60C,sizeof(void*),1U));});{
void*_tmp60F=Cyc_Absyn_compress((void*)_check_null(eopt->topt));void*_stmttmp88=_tmp60F;void*_tmp610=_stmttmp88;void*_tmp611;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp610)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp610)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp610)->f2 != 0){_LL6: _tmp611=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp610)->f2)->hd;_LL7: {void*t=_tmp611;
# 3202
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3207
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
