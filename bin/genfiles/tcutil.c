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
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Position_Error;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
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
# 27 "warn.h"
void Cyc_Warn_vwarn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 33
void Cyc_Warn_verr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 38
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct _tuple12{unsigned f1;int f2;};
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
# 269
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 276
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 294
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 297
int Cyc_Tcutil_zeroable_type(void*);
# 301
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 309
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 313
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 318
int Cyc_Tcutil_tycon_cmp(void*,void*);
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*,void*);
# 38 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 44
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_stmttmp1=_tmp2;void*_tmp3=_stmttmp1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 50
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_stmttmp2=_tmp4;void*_tmp5=_stmttmp2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 56
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_stmttmp3=_tmp6;void*_tmp7=_stmttmp3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 63
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmp8;void*_tmp9=_stmttmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 70
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_stmttmp5=_tmpA;void*_tmpB=_stmttmp5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 77
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmpC;void*_tmpD=_stmttmp6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 84
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_stmttmp7=_tmpE;void*_tmpF=_stmttmp7;void*_tmp10;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {void*c=_tmp10;
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
void*_tmp12=Cyc_Tcutil_compress(t);void*_stmttmp8=_tmp12;void*_tmp13=_stmttmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
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
void*_tmp14=Cyc_Tcutil_compress(t);void*_stmttmp9=_tmp14;void*_tmp15=_stmttmp9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 116
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 124
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp16=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmpA=_tmp16;void*_tmp17=_stmttmpA;void*_tmp18;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17)->tag == 1U){_LL1: _LL2:
 return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp18=_tmp17;_LL4: {void*t=_tmp18;
return Cyc_Tcutil_is_integral_type(t);}}_LL0:;}
# 132
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp19=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmpB=_tmp19;void*_tmp1A=_stmttmpB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 142
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1B=Cyc_Tcutil_compress(t);void*_stmttmpC=_tmp1B;void*_tmp1C=_stmttmpC;void*_tmp1D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->tag == 3U){_LL1: _tmp1D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->f1).ptr_atts).zero_term;_LL2: {void*ztl=_tmp1D;
# 145
return Cyc_Tcutil_force_type2bool(0,ztl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 151
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1E=Cyc_Tcutil_compress(t);void*_stmttmpD=_tmp1E;void*_tmp1F=_stmttmpD;void*_tmp20;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->tag == 3U){_LL1: _tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp20;
# 154
return Cyc_Tcutil_force_type2bool(0,nbl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 160
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp21=Cyc_Tcutil_compress(t);void*_stmttmpE=_tmp21;void*_tmp22=_stmttmpE;void*_tmp23;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->tag == 3U){_LL1: _tmp23=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp23;
# 163
return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,b);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 170
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp24=Cyc_Tcutil_compress(t);void*_stmttmpF=_tmp24;void*_tmp25=_stmttmpF;void*_tmp27;void*_tmp26;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->tag == 3U){_LL1: _tmp26=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).elt_type;_tmp27=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).ptr_atts).bounds;_LL2: {void*elt_type=_tmp26;void*b=_tmp27;
# 173
if(Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=elt_type;
return 1;}else{
# 177
return 0;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 184
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp28=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp28;void*_tmp29=_stmttmp10;void*_tmp2B;void*_tmp2A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->tag == 3U){_LL1: _tmp2A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).elt_type;_tmp2B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).ptr_atts).zero_term;_LL2: {void*elt_type=_tmp2A;void*zt=_tmp2B;
# 187
*elt_type_dest=elt_type;
return Cyc_Absyn_type2bool(0,zt);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 196
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 198
void*_tmp2C=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp2C;void*_tmp2D=_stmttmp11;void*_tmp31;struct Cyc_Absyn_Exp*_tmp30;struct Cyc_Absyn_Tqual _tmp2F;void*_tmp2E;void*_tmp34;void*_tmp33;void*_tmp32;switch(*((int*)_tmp2D)){case 3U: _LL1: _tmp32=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp33=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).bounds;_tmp34=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).zero_term;_LL2: {void*elt=_tmp32;void*bnds=_tmp33;void*zt=_tmp34;
# 200
if(Cyc_Absyn_type2bool(0,zt)){
*ptr_type=t;
*elt_type=elt;
{void*_tmp35=Cyc_Tcutil_compress(bnds);void*_stmttmp12=_tmp35;void*_tmp36=_stmttmp12;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 207
return 1;}else{
return 0;}}case 4U: _LL3: _tmp2E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp2F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).tq;_tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).num_elts;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).zero_term;_LL4: {void*elt=_tmp2E;struct Cyc_Absyn_Tqual tq=_tmp2F;struct Cyc_Absyn_Exp*n=_tmp30;void*zt=_tmp31;
# 210
if(Cyc_Absyn_type2bool(0,zt)){
*elt_type=elt;
*is_fat=0;
({void*_tmp640=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp640;});
return 1;}else{
return 0;}}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 223
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 229
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp641=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp641,b);});struct Cyc_Absyn_Exp*eopt=_tmp37;
if(eopt == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp38=eopt;struct Cyc_Absyn_Exp*e=_tmp38;
struct _tuple12 _tmp39=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp13=_tmp39;struct _tuple12 _tmp3A=_stmttmp13;int _tmp3C;unsigned _tmp3B;_LL1: _tmp3B=_tmp3A.f1;_tmp3C=_tmp3A.f2;_LL2: {unsigned i=_tmp3B;int known=_tmp3C;
return known && i == (unsigned)1;}}}
# 238
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3D=Cyc_Tcutil_compress(t);void*_stmttmp14=_tmp3D;void*_tmp3E=_stmttmp14;struct Cyc_List_List*_tmp3F;struct Cyc_List_List*_tmp40;void*_tmp42;void*_tmp41;struct Cyc_List_List*_tmp44;void*_tmp43;switch(*((int*)_tmp3E)){case 0U: _LL1: _tmp43=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f1;_tmp44=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f2;_LL2: {void*c=_tmp43;struct Cyc_List_List*ts=_tmp44;
# 241
void*_tmp45=c;struct Cyc_Absyn_Aggrdecl*_tmp46;switch(*((int*)_tmp45)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return 1;case 15U: _LL12: _LL13:
 goto _LL15;case 16U: _LL14: _LL15:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp45)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
 return 0;}else{_LL18: _tmp46=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp45)->f1).KnownAggr).val;_LL19: {struct Cyc_Absyn_Aggrdecl*ad=_tmp46;
# 249
if(ad->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp47=_new_region("rgn");struct _RegionHandle*rgn=& _tmp47;_push_region(rgn);
{struct Cyc_List_List*_tmp48=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,ad->tvs,ts);struct Cyc_List_List*inst=_tmp48;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp49=0;_npop_handler(0U);return _tmp49;}}}{
int _tmp4A=1;_npop_handler(0U);return _tmp4A;}}
# 256
;_pop_region(rgn);}}}}default: _LL1A: _LL1B:
# 260
 return 0;}_LLB:;}case 4U: _LL3: _tmp41=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).elt_type;_tmp42=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zero_term;_LL4: {void*t=_tmp41;void*zero_term=_tmp42;
# 265
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6U: _LL5: _tmp40=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E)->f1;_LL6: {struct Cyc_List_List*tqs=_tmp40;
# 267
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple11*)tqs->hd)).f2))return 0;}
return 1;}case 7U: _LL7: _tmp3F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E)->f2;_LL8: {struct Cyc_List_List*fs=_tmp3F;
# 271
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 280
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp4B;void*_tmp4C=_stmttmp15;struct Cyc_List_List*_tmp4D;void*_tmp4E;switch(*((int*)_tmp4C)){case 3U: _LL1: _tmp4E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_LL2: {void*elt=_tmp4E;
return Cyc_Tcutil_is_noreturn_fn_type(elt);}case 5U: _LL3: _tmp4D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_LL4: {struct Cyc_List_List*atts=_tmp4D;
# 284
for(0;atts != 0;atts=atts->tl){
void*_tmp4F=(void*)atts->hd;void*_stmttmp16=_tmp4F;void*_tmp50=_stmttmp16;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp50)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 289
return 0;}default: _LL5: _LL6:
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
void*_tmp51=t;void*_tmp52;struct Cyc_Absyn_Exp*_tmp53;struct Cyc_Absyn_Exp*_tmp54;void**_tmp55;void**_tmp56;switch(*((int*)_tmp51)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp56=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp51)->f2;_LL8: {void**t2opt_ref=_tmp56;
# 328
void*ta=(void*)_check_null(*t2opt_ref);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*t2opt_ref=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51)->f4 == 0){_LL3: _LL4:
# 320
 return t;}else{_LL5: _tmp55=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp51)->f4;_LL6: {void**topt_ref=_tmp55;
# 322
void*ta=(void*)_check_null(*topt_ref);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*topt_ref=t2;
return t2;}}case 9U: _LL9: _tmp54=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp51)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp54;
# 334
Cyc_Evexp_eval_const_uint_exp(e);{
void*_tmp57=e->r;void*_stmttmp17=_tmp57;void*_tmp58=_stmttmp17;void*_tmp59;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp58)->tag == 39U){_LL12: _tmp59=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp58)->f1;_LL13: {void*t2=_tmp59;
return Cyc_Tcutil_compress(t2);}}else{_LL14: _LL15:
 return t;}_LL11:;}}case 11U: _LLB: _tmp53=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp51)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp53;
# 340
void*_tmp5A=e->topt;void*t2=_tmp5A;
if(t2 != 0)return t2;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp51)->f2 != 0){_LLD: _tmp52=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp51)->f2;_LLE: {void*t=_tmp52;
return Cyc_Tcutil_compress(t);}}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 352
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 356
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp5B=Cyc_Absyn_compress_kb(kb);void*_stmttmp18=_tmp5B;void*_tmp5C=_stmttmp18;struct Cyc_Absyn_Kind*_tmp5D;switch(*((int*)_tmp5C)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp5E=_cycalloc(sizeof(*_tmp5E));_tmp5E->tag=1U,_tmp5E->f1=0;_tmp5E;});case 2U: _LL3: _tmp5D=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5C)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp5D;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=2U,_tmp5F->f1=0,_tmp5F->f2=k;_tmp5F;});}default: _LL5: _LL6:
 return kb;}_LL0:;}
# 363
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 365
return({struct Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=tv->name,_tmp60->identity=- 1,({void*_tmp642=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp60->kind=_tmp642;});_tmp60;});}
# 367
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 369
struct _tuple9*_tmp61=arg;void*_tmp64;struct Cyc_Absyn_Tqual _tmp63;struct _fat_ptr*_tmp62;_LL1: _tmp62=_tmp61->f1;_tmp63=_tmp61->f2;_tmp64=_tmp61->f3;_LL2: {struct _fat_ptr*x=_tmp62;struct Cyc_Absyn_Tqual y=_tmp63;void*t=_tmp64;
return({struct _tuple9*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=x,_tmp65->f2=y,({void*_tmp643=Cyc_Tcutil_copy_type(t);_tmp65->f3=_tmp643;});_tmp65;});}}
# 372
static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
return({struct _tuple11*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=(*arg).f1,({void*_tmp644=Cyc_Tcutil_copy_type((*arg).f2);_tmp66->f2=_tmp644;});_tmp66;});}
# 375
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}
# 379
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->name=f->name,_tmp67->tq=f->tq,({void*_tmp646=Cyc_Tcutil_copy_type(f->type);_tmp67->type=_tmp646;}),_tmp67->width=f->width,_tmp67->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp645=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp67->requires_clause=_tmp645;});_tmp67;});}struct _tuple14{void*f1;void*f2;};
# 383
static struct _tuple14*Cyc_Tcutil_copy_rgncmp(struct _tuple14*x){
struct _tuple14*_tmp68=x;void*_tmp6A;void*_tmp69;_LL1: _tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;_LL2: {void*r1=_tmp69;void*r2=_tmp6A;
return({struct _tuple14*_tmp6B=_cycalloc(sizeof(*_tmp6B));({void*_tmp648=Cyc_Tcutil_copy_type(r1);_tmp6B->f1=_tmp648;}),({void*_tmp647=Cyc_Tcutil_copy_type(r2);_tmp6B->f2=_tmp647;});_tmp6B;});}}
# 387
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 391
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6C=Cyc_Tcutil_compress(t);void*_stmttmp19=_tmp6C;void*_tmp6D=_stmttmp19;struct Cyc_Absyn_Datatypedecl*_tmp6E;struct Cyc_Absyn_Enumdecl*_tmp6F;struct Cyc_Absyn_Aggrdecl*_tmp70;struct Cyc_Absyn_Typedefdecl*_tmp73;struct Cyc_List_List*_tmp72;struct _tuple1*_tmp71;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp77;enum Cyc_Absyn_AggrKind _tmp76;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_List_List*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_VarargInfo*_tmp7F;int _tmp7E;struct Cyc_List_List*_tmp7D;void*_tmp7C;struct Cyc_Absyn_Tqual _tmp7B;void*_tmp7A;struct Cyc_List_List*_tmp79;unsigned _tmp8A;void*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Tqual _tmp87;void*_tmp86;struct Cyc_Absyn_PtrLoc*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8C;void*_tmp8B;struct Cyc_Absyn_Tvar*_tmp92;struct Cyc_List_List*_tmp94;void*_tmp93;void*_tmp95;switch(*((int*)_tmp6D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2 == 0){_LL1: _tmp95=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_LL2: {void*c=_tmp95;
return t;}}else{_LL3: _tmp93=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_tmp94=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2;_LL4: {void*c=_tmp93;struct Cyc_List_List*ts=_tmp94;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=0U,_tmp96->f1=c,({struct Cyc_List_List*_tmp649=Cyc_Tcutil_copy_types(ts);_tmp96->f2=_tmp649;});_tmp96;});}}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp92=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp92;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3U: _LL9: _tmp8B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_tq;_tmp8D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).rgn;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).nullable;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).bounds;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).zero_term;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).ptrloc;_LLA: {void*elt=_tmp8B;struct Cyc_Absyn_Tqual tq=_tmp8C;void*rgn=_tmp8D;void*nbl=_tmp8E;void*bs=_tmp8F;void*zt=_tmp90;struct Cyc_Absyn_PtrLoc*loc=_tmp91;
# 398
void*_tmp97=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp97;
void*_tmp98=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp98;
void*_tmp99=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp99;
struct Cyc_Absyn_Tqual _tmp9A=tq;struct Cyc_Absyn_Tqual tq2=_tmp9A;
# 403
void*_tmp9B=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp9B;
void*_tmp9C=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp9C;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9D=_cycalloc(sizeof(*_tmp9D));_tmp9D->tag=3U,(_tmp9D->f1).elt_type=elt2,(_tmp9D->f1).elt_tq=tq2,((_tmp9D->f1).ptr_atts).rgn=rgn2,((_tmp9D->f1).ptr_atts).nullable=nbl2,((_tmp9D->f1).ptr_atts).bounds=bs2,((_tmp9D->f1).ptr_atts).zero_term=zt2,((_tmp9D->f1).ptr_atts).ptrloc=loc;_tmp9D;});}case 4U: _LLB: _tmp86=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp87=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).tq;_tmp88=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).num_elts;_tmp89=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).zero_term;_tmp8A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).zt_loc;_LLC: {void*et=_tmp86;struct Cyc_Absyn_Tqual tq=_tmp87;struct Cyc_Absyn_Exp*eo=_tmp88;void*zt=_tmp89;unsigned ztl=_tmp8A;
# 407
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=4U,({void*_tmp64C=Cyc_Tcutil_copy_type(et);(_tmp9E->f1).elt_type=_tmp64C;}),(_tmp9E->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp64B=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp9E->f1).num_elts=_tmp64B;}),({
void*_tmp64A=Cyc_Tcutil_copy_type(zt);(_tmp9E->f1).zero_term=_tmp64A;}),(_tmp9E->f1).zt_loc=ztl;_tmp9E;});}case 5U: _LLD: _tmp79=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).tvars;_tmp7A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).effect;_tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_tqual;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_type;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).args;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).c_varargs;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).cyc_varargs;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).rgn_po;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).attributes;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_clause;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_relns;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_clause;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_relns;_LLE: {struct Cyc_List_List*tvs=_tmp79;void*effopt=_tmp7A;struct Cyc_Absyn_Tqual rt_tq=_tmp7B;void*rt=_tmp7C;struct Cyc_List_List*args=_tmp7D;int c_varargs=_tmp7E;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp7F;struct Cyc_List_List*rgn_po=_tmp80;struct Cyc_List_List*atts=_tmp81;struct Cyc_Absyn_Exp*req=_tmp82;struct Cyc_List_List*req_rlns=_tmp83;struct Cyc_Absyn_Exp*ens=_tmp84;struct Cyc_List_List*ens_rlns=_tmp85;
# 410
struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp9F;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmpA0=Cyc_Tcutil_copy_type(rt);void*rt2=_tmpA0;
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmpA1;
int _tmpA2=c_varargs;int c_varargs2=_tmpA2;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo*cv=cyc_varargs;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->name=cv->name,_tmpA3->tq=cv->tq,({void*_tmp64D=Cyc_Tcutil_copy_type(cv->type);_tmpA3->type=_tmp64D;}),_tmpA3->inject=cv->inject;_tmpA3;});}{
# 421
struct Cyc_List_List*_tmpA4=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmpA4;
struct Cyc_List_List*_tmpA5=atts;struct Cyc_List_List*atts2=_tmpA5;
struct Cyc_Absyn_Exp*_tmpA6=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmpA6;
struct Cyc_List_List*_tmpA7=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmpA7;
struct Cyc_Absyn_Exp*_tmpA8=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmpA8;
struct Cyc_List_List*_tmpA9=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmpA9;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=5U,(_tmpAA->f1).tvars=tvs2,(_tmpAA->f1).effect=effopt2,(_tmpAA->f1).ret_tqual=rt_tq,(_tmpAA->f1).ret_type=rt2,(_tmpAA->f1).args=args2,(_tmpAA->f1).c_varargs=c_varargs2,(_tmpAA->f1).cyc_varargs=cyc_varargs2,(_tmpAA->f1).rgn_po=rgn_po2,(_tmpAA->f1).attributes=atts2,(_tmpAA->f1).requires_clause=req2,(_tmpAA->f1).requires_relns=req_rlns2,(_tmpAA->f1).ensures_clause=ens2,(_tmpAA->f1).ensures_relns=ens_rlns2;_tmpAA;});}}case 6U: _LLF: _tmp78=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp78;
# 430
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=6U,({struct Cyc_List_List*_tmp64E=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmpAB->f1=_tmp64E;});_tmpAB;});}case 7U: _LL11: _tmp76=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f1;_tmp77=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp76;struct Cyc_List_List*fs=_tmp77;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=7U,_tmpAC->f1=k,({struct Cyc_List_List*_tmp64F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmpAC->f2=_tmp64F;});_tmpAC;});}case 9U: _LL13: _tmp75=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp75;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=9U,_tmpAD->f1=e;_tmpAD;});}case 11U: _LL15: _tmp74=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6D)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp74;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=11U,_tmpAE->f1=e;_tmpAE;});}case 8U: _LL17: _tmp71=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f1;_tmp72=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f2;_tmp73=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f3;_LL18: {struct _tuple1*tdn=_tmp71;struct Cyc_List_List*ts=_tmp72;struct Cyc_Absyn_Typedefdecl*td=_tmp73;
# 435
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=8U,_tmpAF->f1=tdn,({struct Cyc_List_List*_tmp650=Cyc_Tcutil_copy_types(ts);_tmpAF->f2=_tmp650;}),_tmpAF->f3=td,_tmpAF->f4=0;_tmpAF;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)){case 0U: _LL19: _tmp70=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp70;
# 438
struct Cyc_List_List*_tmpB0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpB0;
return({union Cyc_Absyn_AggrInfo _tmp651=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp651,targs);});}case 1U: _LL1B: _tmp6F=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp6F;
# 441
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp6E=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp6E;
# 443
struct Cyc_List_List*_tmpB1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpB1;
return({union Cyc_Absyn_DatatypeInfo _tmp652=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5F5;_tmp5F5.name=dd->name,_tmp5F5.is_extensible=0;_tmp5F5;}));Cyc_Absyn_datatype_type(_tmp652,targs);});}}}_LL0:;}
# 450
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB2=d;struct _fat_ptr*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->tag == 0U){_LL1: _tmpB4=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpB4;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->tag=0U,({struct Cyc_Absyn_Exp*_tmp653=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpB5->f1=_tmp653;});_tmpB5;});}}else{_LL3: _tmpB3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB2)->f1;_LL4: {struct _fat_ptr*v=_tmpB3;
return d;}}_LL0:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 456
static struct _tuple15*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple15*e){
# 458
return({struct _tuple15*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_List_List*_tmp655=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB6->f1=_tmp655;}),({struct Cyc_Absyn_Exp*_tmp654=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB6->f2=_tmp654;});_tmpB6;});}
# 461
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB7=preserve_types;int pt=_tmpB7;
{void*_tmpB8=e->r;void*_stmttmp1A=_tmpB8;void*_tmpB9=_stmttmp1A;struct _fat_ptr _tmpBB;int _tmpBA;void*_tmpBC;struct _fat_ptr*_tmpBE;struct Cyc_Absyn_Exp*_tmpBD;struct Cyc_List_List*_tmpC0;struct Cyc_Core_Opt*_tmpBF;struct Cyc_Absyn_Exp*_tmpC2;struct Cyc_Absyn_Exp*_tmpC1;int _tmpC8;int _tmpC7;struct Cyc_Absyn_Exp*_tmpC6;void**_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;int _tmpC3;struct Cyc_Absyn_Enumfield*_tmpCA;void*_tmpC9;struct Cyc_Absyn_Enumfield*_tmpCC;struct Cyc_Absyn_Enumdecl*_tmpCB;struct Cyc_List_List*_tmpD0;void*_tmpCF;struct Cyc_Absyn_Tqual _tmpCE;struct _fat_ptr*_tmpCD;struct Cyc_List_List*_tmpD2;void*_tmpD1;struct Cyc_Absyn_Aggrdecl*_tmpD6;struct Cyc_List_List*_tmpD5;struct Cyc_List_List*_tmpD4;struct _tuple1*_tmpD3;int _tmpD9;void*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;int _tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Vardecl*_tmpDA;struct Cyc_Absyn_Datatypefield*_tmpE0;struct Cyc_Absyn_Datatypedecl*_tmpDF;struct Cyc_List_List*_tmpDE;struct Cyc_List_List*_tmpE1;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Exp*_tmpE2;int _tmpE7;int _tmpE6;struct _fat_ptr*_tmpE5;struct Cyc_Absyn_Exp*_tmpE4;int _tmpEB;int _tmpEA;struct _fat_ptr*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;struct Cyc_List_List*_tmpEC;struct Cyc_Absyn_Exp*_tmpED;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_List_List*_tmpF0;void*_tmpEF;struct Cyc_Absyn_Exp*_tmpF1;void*_tmpF2;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF4;enum Cyc_Absyn_Coercion _tmpF9;int _tmpF8;struct Cyc_Absyn_Exp*_tmpF7;void*_tmpF6;struct Cyc_List_List*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;struct Cyc_Absyn_Exp*_tmpFC;int _tmpFE;struct Cyc_Absyn_Exp*_tmpFD;int _tmp102;struct Cyc_Absyn_VarargCallInfo*_tmp101;struct Cyc_List_List*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;int _tmp10B;int _tmp10A;void*_tmp109;struct Cyc_Absyn_Tqual _tmp108;struct _fat_ptr*_tmp107;struct Cyc_List_List*_tmp106;int _tmp105;struct Cyc_List_List*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp10E;struct Cyc_Core_Opt*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;enum Cyc_Absyn_Incrementor _tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_List_List*_tmp11B;enum Cyc_Absyn_Primop _tmp11A;struct _fat_ptr _tmp11C;void*_tmp11D;union Cyc_Absyn_Cnst _tmp11E;switch(*((int*)_tmpB9)){case 0U: _LL1: _tmp11E=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp11E;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp11D=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL4: {void*b=_tmp11D;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp11C=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL6: {struct _fat_ptr p=_tmp11C;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp11A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp11A;struct Cyc_List_List*es=_tmp11B;
new_e=({enum Cyc_Absyn_Primop _tmp657=p;struct Cyc_List_List*_tmp656=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_primop_exp(_tmp657,_tmp656,e->loc);});goto _LL0;}case 5U: _LL9: _tmp118=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp119=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp118;enum Cyc_Absyn_Incrementor i=_tmp119;
new_e=({struct Cyc_Absyn_Exp*_tmp659=Cyc_Tcutil_deep_copy_exp(pt,e1);enum Cyc_Absyn_Incrementor _tmp658=i;Cyc_Absyn_increment_exp(_tmp659,_tmp658,e->loc);});goto _LL0;}case 7U: _LLB: _tmp116=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp117=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp116;struct Cyc_Absyn_Exp*e2=_tmp117;
new_e=({struct Cyc_Absyn_Exp*_tmp65B=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp65A=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_and_exp(_tmp65B,_tmp65A,e->loc);});goto _LL0;}case 8U: _LLD: _tmp114=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp115=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp114;struct Cyc_Absyn_Exp*e2=_tmp115;
new_e=({struct Cyc_Absyn_Exp*_tmp65D=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp65C=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_or_exp(_tmp65D,_tmp65C,e->loc);});goto _LL0;}case 9U: _LLF: _tmp112=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp113=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp112;struct Cyc_Absyn_Exp*e2=_tmp113;
new_e=({struct Cyc_Absyn_Exp*_tmp65F=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp65E=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_seq_exp(_tmp65F,_tmp65E,e->loc);});goto _LL0;}case 6U: _LL11: _tmp10F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp110=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp111=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp10F;struct Cyc_Absyn_Exp*e2=_tmp110;struct Cyc_Absyn_Exp*e3=_tmp111;
# 474
new_e=({struct Cyc_Absyn_Exp*_tmp662=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp661=Cyc_Tcutil_deep_copy_exp(pt,e2);struct Cyc_Absyn_Exp*_tmp660=Cyc_Tcutil_deep_copy_exp(pt,e3);Cyc_Absyn_conditional_exp(_tmp662,_tmp661,_tmp660,e->loc);});goto _LL0;}case 4U: _LL13: _tmp10C=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp10D=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp10E=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp10C;struct Cyc_Core_Opt*po=_tmp10D;struct Cyc_Absyn_Exp*e2=_tmp10E;
# 476
new_e=({struct Cyc_Absyn_Exp*_tmp665=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Core_Opt*_tmp664=(unsigned)po?({struct Cyc_Core_Opt*_tmp11F=_cycalloc(sizeof(*_tmp11F));_tmp11F->v=(void*)po->v;_tmp11F;}): 0;struct Cyc_Absyn_Exp*_tmp663=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_assignop_exp(_tmp665,_tmp664,_tmp663,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3 != 0){_LL15: _tmp103=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp104=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp105=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->num_varargs;_tmp106=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->injectors;_tmp107=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->name;_tmp108=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->tq;_tmp109=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->type;_tmp10A=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->inject;_tmp10B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp103;struct Cyc_List_List*es=_tmp104;int n=_tmp105;struct Cyc_List_List*is=_tmp106;struct _fat_ptr*nm=_tmp107;struct Cyc_Absyn_Tqual tq=_tmp108;void*t=_tmp109;int i=_tmp10A;int resolved=_tmp10B;
# 479
new_e=({void*_tmp66B=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->tag=10U,({
struct Cyc_Absyn_Exp*_tmp66A=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp122->f1=_tmp66A;}),({struct Cyc_List_List*_tmp669=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp122->f2=_tmp669;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp668=({struct Cyc_Absyn_VarargCallInfo*_tmp121=_cycalloc(sizeof(*_tmp121));_tmp121->num_varargs=n,_tmp121->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp667=({struct Cyc_Absyn_VarargInfo*_tmp120=_cycalloc(sizeof(*_tmp120));_tmp120->name=nm,_tmp120->tq=tq,({void*_tmp666=Cyc_Tcutil_copy_type(t);_tmp120->type=_tmp666;}),_tmp120->inject=i;_tmp120;});_tmp121->vai=_tmp667;});_tmp121;});
# 481
_tmp122->f3=_tmp668;}),_tmp122->f4=resolved;_tmp122;});
# 479
Cyc_Absyn_new_exp(_tmp66B,e->loc);});
# 484
goto _LL0;}}else{_LL17: _tmpFF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp100=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp101=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp102=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmpFF;struct Cyc_List_List*es=_tmp100;struct Cyc_Absyn_VarargCallInfo*vci=_tmp101;int resolved=_tmp102;
# 486
new_e=({void*_tmp66E=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->tag=10U,({struct Cyc_Absyn_Exp*_tmp66D=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp123->f1=_tmp66D;}),({struct Cyc_List_List*_tmp66C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp123->f2=_tmp66C;}),_tmp123->f3=vci,_tmp123->f4=resolved;_tmp123;});Cyc_Absyn_new_exp(_tmp66E,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmpFD=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFE=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmpFD;int b=_tmpFE;
# 489
new_e=b?({struct Cyc_Absyn_Exp*_tmp670=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_rethrow_exp(_tmp670,e->loc);}):({struct Cyc_Absyn_Exp*_tmp66F=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_throw_exp(_tmp66F,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmpFC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpFC;
# 492
new_e=({struct Cyc_Absyn_Exp*_tmp671=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp671,e->loc);});goto _LL0;}case 13U: _LL1D: _tmpFA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFB=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpFA;struct Cyc_List_List*ts=_tmpFB;
# 494
new_e=({struct Cyc_Absyn_Exp*_tmp673=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_List_List*_tmp672=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp673,_tmp672,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpF6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpF8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpF9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL20: {void*t=_tmpF6;struct Cyc_Absyn_Exp*e1=_tmpF7;int b=_tmpF8;enum Cyc_Absyn_Coercion c=_tmpF9;
# 496
new_e=({void*_tmp677=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp676=Cyc_Tcutil_deep_copy_exp(pt,e1);int _tmp675=b;enum Cyc_Absyn_Coercion _tmp674=c;Cyc_Absyn_cast_exp(_tmp677,_tmp676,_tmp675,_tmp674,e->loc);});goto _LL0;}case 16U: _LL21: _tmpF4=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF5=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpF4;struct Cyc_Absyn_Exp*e1=_tmpF5;
# 498
new_e=({struct Cyc_Absyn_Exp*_tmp679=(unsigned)eo?Cyc_Tcutil_deep_copy_exp(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp678=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_New_exp(_tmp679,_tmp678,e->loc);});goto _LL0;}case 15U: _LL23: _tmpF3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpF3;
new_e=({struct Cyc_Absyn_Exp*_tmp67A=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_address_exp(_tmp67A,e->loc);});goto _LL0;}case 17U: _LL25: _tmpF2=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL26: {void*t=_tmpF2;
new_e=({void*_tmp67B=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp67B,e->loc);});goto _LL0;}case 18U: _LL27: _tmpF1=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF1;
new_e=({struct Cyc_Absyn_Exp*_tmp67C=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp67C,e->loc);});goto _LL0;}case 19U: _LL29: _tmpEF=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF0=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL2A: {void*t=_tmpEF;struct Cyc_List_List*ofs=_tmpF0;
new_e=({void*_tmp67E=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp67D=ofs;Cyc_Absyn_offsetof_exp(_tmp67E,_tmp67D,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpEE=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpEE;
new_e=({struct Cyc_Absyn_Exp*_tmp67F=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_deref_exp(_tmp67F,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpED=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpED;
new_e=({struct Cyc_Absyn_Exp*_tmp680=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_extension_exp(_tmp680,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpEC=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL30: {struct Cyc_List_List*es=_tmpEC;
new_e=({struct Cyc_List_List*_tmp681=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_tuple_exp(_tmp681,e->loc);});goto _LL0;}case 21U: _LL31: _tmpE8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEA=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpEB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpE8;struct _fat_ptr*n=_tmpE9;int f1=_tmpEA;int f2=_tmpEB;
# 507
new_e=({void*_tmp683=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->tag=21U,({struct Cyc_Absyn_Exp*_tmp682=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp124->f1=_tmp682;}),_tmp124->f2=n,_tmp124->f3=f1,_tmp124->f4=f2;_tmp124;});Cyc_Absyn_new_exp(_tmp683,e->loc);});goto _LL0;}case 22U: _LL33: _tmpE4=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE6=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpE7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpE4;struct _fat_ptr*n=_tmpE5;int f1=_tmpE6;int f2=_tmpE7;
# 509
new_e=({void*_tmp685=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=22U,({struct Cyc_Absyn_Exp*_tmp684=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp125->f1=_tmp684;}),_tmp125->f2=n,_tmp125->f3=f1,_tmp125->f4=f2;_tmp125;});Cyc_Absyn_new_exp(_tmp685,e->loc);});goto _LL0;}case 23U: _LL35: _tmpE2=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE3=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpE2;struct Cyc_Absyn_Exp*e2=_tmpE3;
# 511
new_e=({struct Cyc_Absyn_Exp*_tmp687=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp686=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_subscript_exp(_tmp687,_tmp686,e->loc);});goto _LL0;}case 26U: _LL37: _tmpE1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL38: {struct Cyc_List_List*eds=_tmpE1;
# 513
new_e=({void*_tmp689=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=26U,({struct Cyc_List_List*_tmp688=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp126->f1=_tmp688;});_tmp126;});Cyc_Absyn_new_exp(_tmp689,e->loc);});goto _LL0;}case 31U: _LL39: _tmpDE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDF=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE0=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3A: {struct Cyc_List_List*es=_tmpDE;struct Cyc_Absyn_Datatypedecl*dtd=_tmpDF;struct Cyc_Absyn_Datatypefield*dtf=_tmpE0;
# 515
new_e=({void*_tmp68B=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=31U,({struct Cyc_List_List*_tmp68A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp127->f1=_tmp68A;}),_tmp127->f2=dtd,_tmp127->f3=dtf;_tmp127;});Cyc_Absyn_new_exp(_tmp68B,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpDA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDB=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpDC=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpDD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpDA;struct Cyc_Absyn_Exp*e1=_tmpDB;struct Cyc_Absyn_Exp*e2=_tmpDC;int b=_tmpDD;
# 517
new_e=({void*_tmp68E=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=27U,_tmp128->f1=vd,({struct Cyc_Absyn_Exp*_tmp68D=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp128->f2=_tmp68D;}),({struct Cyc_Absyn_Exp*_tmp68C=Cyc_Tcutil_deep_copy_exp(pt,e2);_tmp128->f3=_tmp68C;}),_tmp128->f4=b;_tmp128;});Cyc_Absyn_new_exp(_tmp68E,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpD7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD8=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpD9=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpD7;void*t=_tmpD8;int b=_tmpD9;
# 519
new_e=({void*_tmp691=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=28U,({struct Cyc_Absyn_Exp*_tmp690=Cyc_Tcutil_deep_copy_exp(pt,e);_tmp129->f1=_tmp690;}),({void*_tmp68F=Cyc_Tcutil_copy_type(t);_tmp129->f2=_tmp68F;}),_tmp129->f3=b;_tmp129;});Cyc_Absyn_new_exp(_tmp691,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpD3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD4=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpD5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpD6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL40: {struct _tuple1*n=_tmpD3;struct Cyc_List_List*ts=_tmpD4;struct Cyc_List_List*eds=_tmpD5;struct Cyc_Absyn_Aggrdecl*agr=_tmpD6;
# 522
new_e=({void*_tmp694=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=29U,_tmp12A->f1=n,({struct Cyc_List_List*_tmp693=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp12A->f2=_tmp693;}),({struct Cyc_List_List*_tmp692=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12A->f3=_tmp692;}),_tmp12A->f4=agr;_tmp12A;});Cyc_Absyn_new_exp(_tmp694,e->loc);});
# 524
goto _LL0;}case 30U: _LL41: _tmpD1=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD2=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL42: {void*t=_tmpD1;struct Cyc_List_List*eds=_tmpD2;
# 526
new_e=({void*_tmp697=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=30U,({void*_tmp696=Cyc_Tcutil_copy_type(t);_tmp12B->f1=_tmp696;}),({struct Cyc_List_List*_tmp695=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12B->f2=_tmp695;});_tmp12B;});Cyc_Absyn_new_exp(_tmp697,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpCD=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f1;_tmpCE=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f2;_tmpCF=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f3;_tmpD0=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL44: {struct _fat_ptr*vopt=_tmpCD;struct Cyc_Absyn_Tqual tq=_tmpCE;void*t=_tmpCF;struct Cyc_List_List*eds=_tmpD0;
# 529
new_e=({void*_tmp69B=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=25U,({struct _tuple9*_tmp69A=({struct _tuple9*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->f1=vopt,_tmp12C->f2=tq,({void*_tmp699=Cyc_Tcutil_copy_type(t);_tmp12C->f3=_tmp699;});_tmp12C;});_tmp12D->f1=_tmp69A;}),({
struct Cyc_List_List*_tmp698=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12D->f2=_tmp698;});_tmp12D;});
# 529
Cyc_Absyn_new_exp(_tmp69B,e->loc);});
# 531
goto _LL0;}case 32U: _LL45: _tmpCB=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpCC=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpCB;struct Cyc_Absyn_Enumfield*ef=_tmpCC;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpC9=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpCA=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL48: {void*t=_tmpC9;struct Cyc_Absyn_Enumfield*ef=_tmpCA;
# 534
new_e=({void*_tmp69D=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=33U,({void*_tmp69C=Cyc_Tcutil_copy_type(t);_tmp12E->f1=_tmp69C;}),_tmp12E->f2=ef;_tmp12E;});Cyc_Absyn_new_exp(_tmp69D,e->loc);});goto _LL0;}case 34U: _LL49: _tmpC3=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).is_calloc;_tmpC4=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).rgn;_tmpC5=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).elt_type;_tmpC6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).num_elts;_tmpC7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).fat_result;_tmpC8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).inline_call;_LL4A: {int ic=_tmpC3;struct Cyc_Absyn_Exp*r=_tmpC4;void**t=_tmpC5;struct Cyc_Absyn_Exp*n=_tmpC6;int res=_tmpC7;int inlc=_tmpC8;
# 536
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=Cyc_Tcutil_deep_copy_exp(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp12F=_cycalloc(sizeof(*_tmp12F));({void*_tmp69E=Cyc_Tcutil_copy_type(*t);*_tmp12F=_tmp69E;});_tmp12F;});
({void*_tmp69F=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=34U,(_tmp130->f1).is_calloc=ic,(_tmp130->f1).rgn=r1,(_tmp130->f1).elt_type=t1,(_tmp130->f1).num_elts=n,(_tmp130->f1).fat_result=res,(_tmp130->f1).inline_call=inlc;_tmp130;});e2->r=_tmp69F;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpC1=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpC2=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpC1;struct Cyc_Absyn_Exp*e2=_tmpC2;
new_e=({struct Cyc_Absyn_Exp*_tmp6A1=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6A0=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_swap_exp(_tmp6A1,_tmp6A0,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpBF=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpC0=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpBF;struct Cyc_List_List*eds=_tmpC0;
# 544
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->v=(struct _tuple1*)nopt->v;_tmp131;});
new_e=({void*_tmp6A3=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=36U,_tmp132->f1=nopt1,({struct Cyc_List_List*_tmp6A2=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp132->f2=_tmp6A2;});_tmp132;});Cyc_Absyn_new_exp(_tmp6A3,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 549
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6A4=({const char*_tmp133="deep_copy: statement expressions unsupported";_tag_fat(_tmp133,sizeof(char),45U);});_tmp134->f1=_tmp6A4;});_tmp134;}));case 38U: _LL51: _tmpBD=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpBE=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpBD;struct _fat_ptr*fn=_tmpBE;
# 551
new_e=({void*_tmp6A6=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->tag=38U,({struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp135->f1=_tmp6A5;}),_tmp135->f2=fn;_tmp135;});Cyc_Absyn_new_exp(_tmp6A6,e->loc);});goto _LL0;}case 39U: _LL53: _tmpBC=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL54: {void*t=_tmpBC;
new_e=({void*_tmp6A7=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp6A7,e->loc);});goto _LL0;}default: _LL55: _tmpBA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpBB=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL56: {int v=_tmpBA;struct _fat_ptr s=_tmpBB;
new_e=Cyc_Absyn_asm_exp(v,s,e->loc);goto _LL0;}}_LL0:;}
# 556
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 560
return new_e;}struct _tuple16{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple17{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 571 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp136=ka1;enum Cyc_Absyn_AliasQual _tmp138;enum Cyc_Absyn_KindQual _tmp137;_LL1: _tmp137=_tmp136->kind;_tmp138=_tmp136->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp137;enum Cyc_Absyn_AliasQual a1=_tmp138;
struct Cyc_Absyn_Kind*_tmp139=ka2;enum Cyc_Absyn_AliasQual _tmp13B;enum Cyc_Absyn_KindQual _tmp13A;_LL4: _tmp13A=_tmp139->kind;_tmp13B=_tmp139->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp13A;enum Cyc_Absyn_AliasQual a2=_tmp13B;
# 575
if((int)k1 != (int)k2){
struct _tuple16 _tmp13C=({struct _tuple16 _tmp5F6;_tmp5F6.f1=k1,_tmp5F6.f2=k2;_tmp5F6;});struct _tuple16 _stmttmp1B=_tmp13C;struct _tuple16 _tmp13D=_stmttmp1B;switch(_tmp13D.f1){case Cyc_Absyn_BoxKind: switch(_tmp13D.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp13D.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 583
if((int)a1 != (int)a2){
struct _tuple17 _tmp13E=({struct _tuple17 _tmp5F7;_tmp5F7.f1=a1,_tmp5F7.f2=a2;_tmp5F7;});struct _tuple17 _stmttmp1C=_tmp13E;struct _tuple17 _tmp13F=_stmttmp1C;switch(_tmp13F.f1){case Cyc_Absyn_Aliasable: if(_tmp13F.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp13F.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 589
return 1;}}}
# 592
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp140=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1D=_tmp140;void*_tmp141=_stmttmp1D;struct Cyc_Absyn_Kind*_tmp142;struct Cyc_Absyn_Kind*_tmp143;switch(*((int*)_tmp141)){case 0U: _LL1: _tmp143=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp141)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp143;
return k;}case 2U: _LL3: _tmp142=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp141)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp142;
return k;}default: _LL5: _LL6:
({void*_tmp6A8=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp144=_cycalloc(sizeof(*_tmp144));_tmp144->tag=2U,_tmp144->f1=0,_tmp144->f2=def;_tmp144;});tv->kind=_tmp6A8;});return def;}_LL0:;}struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 600
struct _tuple18 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp145=Cyc_Tcutil_compress(t);void*_stmttmp1E=_tmp145;void*_tmp146=_stmttmp1E;struct Cyc_Absyn_Tvar*_tmp147;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->tag == 2U){_LL1: _tmp147=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp146)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp147;
# 603
void*_tmp148=tv->kind;void*oldkb=_tmp148;
tv->kind=kb;
return({struct _tuple18 _tmp5F8;_tmp5F8.f1=tv,_tmp5F8.f2=oldkb;_tmp5F8;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp14B=({struct Cyc_String_pa_PrintArg_struct _tmp5F9;_tmp5F9.tag=0U,({struct _fat_ptr _tmp6A9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5F9.f1=_tmp6A9;});_tmp5F9;});void*_tmp149[1U];_tmp149[0]=& _tmp14B;({struct _fat_ptr _tmp6AA=({const char*_tmp14A="swap_kind: cannot update the kind of %s";_tag_fat(_tmp14A,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6AA,_tag_fat(_tmp149,sizeof(void*),1U));});});}_LL0:;}
# 612
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 614
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 618
struct _RegionHandle _tmp14C=_new_region("temp");struct _RegionHandle*temp=& _tmp14C;_push_region(temp);
{struct Cyc_List_List*_tmp14D=0;struct Cyc_List_List*inst=_tmp14D;
# 621
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp14E=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp14E;
void*_tmp14F=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp14F;
struct Cyc_Absyn_Kind*_tmp150=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp1F=_tmp150;struct Cyc_Absyn_Kind*_tmp151=_stmttmp1F;switch(((struct Cyc_Absyn_Kind*)_tmp151)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 627
 inst=({struct Cyc_List_List*_tmp153=_region_malloc(temp,sizeof(*_tmp153));({struct _tuple18*_tmp6AB=({struct _tuple18*_tmp152=_region_malloc(temp,sizeof(*_tmp152));_tmp152->f1=tv,_tmp152->f2=t;_tmp152;});_tmp153->hd=_tmp6AB;}),_tmp153->tl=inst;_tmp153;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 631
if(inst != 0){
field_type=({struct _RegionHandle*_tmp6AD=temp;struct Cyc_List_List*_tmp6AC=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp6AD,_tmp6AC,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 619
;_pop_region(temp);}
# 636
return k;}
# 643
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 645
void*_tmp154=Cyc_Tcutil_compress(t);void*_stmttmp20=_tmp154;void*_tmp155=_stmttmp20;struct Cyc_Absyn_Typedefdecl*_tmp156;struct Cyc_Absyn_Exp*_tmp157;struct Cyc_Absyn_PtrInfo _tmp158;struct Cyc_List_List*_tmp15A;void*_tmp159;struct Cyc_Absyn_Tvar*_tmp15B;struct Cyc_Core_Opt*_tmp15C;switch(*((int*)_tmp155)){case 1U: _LL1: _tmp15C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp155)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp15C;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp15B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp155)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp15B;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp159=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f1;_tmp15A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp155)->f2;_LL6: {void*c=_tmp159;struct Cyc_List_List*ts=_tmp15A;
# 649
void*_tmp15D=c;int _tmp161;struct Cyc_Absyn_AggrdeclImpl*_tmp160;struct Cyc_List_List*_tmp15F;enum Cyc_Absyn_AggrKind _tmp15E;struct Cyc_Absyn_Kind*_tmp162;enum Cyc_Absyn_Size_of _tmp163;switch(*((int*)_tmp15D)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp163=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp15D)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp163;
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp162=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp15D)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp162;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp15D)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 670
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 672
({void*_tmp164=0U;({struct _fat_ptr _tmp6AE=({const char*_tmp165="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp165,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6AE,_tag_fat(_tmp164,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp15D)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 676
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp15E=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp15D)->f1).KnownAggr).val)->kind;_tmp15F=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp15D)->f1).KnownAggr).val)->tvs;_tmp160=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp15D)->f1).KnownAggr).val)->impl;_tmp161=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp15D)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp15E;struct Cyc_List_List*tvs=_tmp15F;struct Cyc_Absyn_AggrdeclImpl*i=_tmp160;int expected_mem_kind=_tmp161;
# 678
if(i == 0){
if(expected_mem_kind)
return& Cyc_Tcutil_mk;else{
# 682
return& Cyc_Tcutil_ak;}}{
# 684
struct Cyc_List_List*_tmp166=i->fields;struct Cyc_List_List*fields=_tmp166;
if(fields == 0)return& Cyc_Tcutil_mk;
# 687
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){;}{
void*_tmp167=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp167;
struct Cyc_Absyn_Kind*_tmp168=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp168;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 695
for(0;fields != 0;fields=fields->tl){
void*_tmp169=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp169;
struct Cyc_Absyn_Kind*_tmp16A=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp16A;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 701
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 703
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp158=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp155)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp158;
# 706
void*_tmp16B=Cyc_Tcutil_compress((pinfo.ptr_atts).bounds);void*_stmttmp21=_tmp16B;void*_tmp16C=_stmttmp21;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16C)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16C)->f1)){case 13U: _LL4D: _LL4E: {
# 708
enum Cyc_Absyn_AliasQual _tmp16D=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp22=_tmp16D;enum Cyc_Absyn_AliasQual _tmp16E=_stmttmp22;switch(_tmp16E){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 714
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 716
enum Cyc_Absyn_AliasQual _tmp16F=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp23=_tmp16F;enum Cyc_Absyn_AliasQual _tmp170=_stmttmp23;switch(_tmp170){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 722
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 726
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp157=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp155)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp157;
# 728
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp156=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp155)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp156;
# 732
if(td == 0 || td->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp173=({struct Cyc_String_pa_PrintArg_struct _tmp5FA;_tmp5FA.tag=0U,({struct _fat_ptr _tmp6AF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5FA.f1=_tmp6AF;});_tmp5FA;});void*_tmp171[1U];_tmp171[0]=& _tmp173;({struct _fat_ptr _tmp6B0=({const char*_tmp172="type_kind: typedef found: %s";_tag_fat(_tmp172,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B0,_tag_fat(_tmp171,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp155)->f1)->r)){case 0U: _LL17: _LL18:
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
{struct Cyc_Absyn_Kind*_tmp174=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp24=_tmp174;struct Cyc_Absyn_Kind*_tmp175=_stmttmp24;switch(((struct Cyc_Absyn_Kind*)_tmp175)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp175)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp176=_cycalloc(sizeof(*_tmp176));_tmp176->tag=9U,({struct Cyc_Absyn_Exp*_tmp6B1=Cyc_Absyn_uint_exp(0U,0U);_tmp176->f1=_tmp6B1;});_tmp176;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 771
return({struct _tuple18*_tmp177=_cycalloc(sizeof(*_tmp177));_tmp177->f1=tv,_tmp177->f2=t;_tmp177;});}
# 778
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp178=Cyc_Tcutil_compress(t);void*_stmttmp25=_tmp178;void*_tmp179=_stmttmp25;struct Cyc_List_List*_tmp17A;struct Cyc_List_List*_tmp17B;struct Cyc_List_List*_tmp182;struct Cyc_Absyn_VarargInfo*_tmp181;struct Cyc_List_List*_tmp180;void*_tmp17F;struct Cyc_Absyn_Tqual _tmp17E;void*_tmp17D;struct Cyc_List_List*_tmp17C;struct Cyc_List_List*_tmp183;void*_tmp184;void*_tmp186;void*_tmp185;struct Cyc_List_List*_tmp187;switch(*((int*)_tmp179)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp187=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp179)->f2;_LL6: {struct Cyc_List_List*ts=_tmp187;
# 783
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 787
struct Cyc_Absyn_Kind*_tmp188=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp26=_tmp188;struct Cyc_Absyn_Kind*_tmp189=_stmttmp26;switch(((struct Cyc_Absyn_Kind*)_tmp189)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp185=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp179)->f1).elt_type;_tmp186=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp179)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp185;void*r=_tmp186;
# 795
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp18A[2U];({void*_tmp6B3=Cyc_Absyn_access_eff(r);_tmp18A[0]=_tmp6B3;}),({void*_tmp6B2=Cyc_Tcutil_rgns_of(et);_tmp18A[1]=_tmp6B2;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp18A,sizeof(void*),2U));})));}case 4U: _LLD: _tmp184=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp179)->f1).elt_type;_LLE: {void*et=_tmp184;
# 797
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp183=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp179)->f2;_LL10: {struct Cyc_List_List*fs=_tmp183;
# 799
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp17C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).tvars;_tmp17D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).effect;_tmp17E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ret_tqual;_tmp17F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).ret_type;_tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).args;_tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).cyc_varargs;_tmp182=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp179)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp17C;void*eff=_tmp17D;struct Cyc_Absyn_Tqual rt_tq=_tmp17E;void*rt=_tmp17F;struct Cyc_List_List*args=_tmp180;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp181;struct Cyc_List_List*rpo=_tmp182;
# 808
void*_tmp18B=({struct Cyc_List_List*_tmp6B4=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp6B4,(void*)_check_null(eff));});void*e=_tmp18B;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp17B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp179)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp17B;
# 811
struct Cyc_List_List*_tmp18C=0;struct Cyc_List_List*ts=_tmp18C;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp18D=_cycalloc(sizeof(*_tmp18D));_tmp18D->hd=(*((struct _tuple11*)tqts->hd)).f2,_tmp18D->tl=ts;_tmp18D;});}
_tmp17A=ts;goto _LL16;}case 8U: _LL15: _tmp17A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp179)->f2;_LL16: {struct Cyc_List_List*ts=_tmp17A;
# 816
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({void*_tmp18E=0U;({struct _fat_ptr _tmp6B5=({const char*_tmp18F="typedecl in rgns_of";_tag_fat(_tmp18F,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B5,_tag_fat(_tmp18E,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 826
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp190=e;void*_tmp191;struct Cyc_List_List**_tmp192;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f1)){case 9U: _LL1: _tmp192=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2;_LL2: {struct Cyc_List_List**es=_tmp192;
# 830
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp193=(void*)effs->hd;void*eff=_tmp193;
({void*_tmp6B6=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp6B6;});{
void*_tmp194=(void*)effs->hd;void*_stmttmp27=_tmp194;void*_tmp195=_stmttmp27;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->tl == 0){_LLE: _LLF:
# 839
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 843
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp196=Cyc_Tcutil_compress((void*)effs->hd);void*_stmttmp28=_tmp196;void*_tmp197=_stmttmp28;void*_tmp198;struct Cyc_List_List*_tmp199;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f1)){case 9U: _LL13: _tmp199=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp199;
# 848
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);
goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp198=_tmp197;_LL1C: {void*e=_tmp198;
effects=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->hd=e,_tmp19A->tl=effects;_tmp19A;});goto _LL12;}}_LL12:;}}
# 856
({struct Cyc_List_List*_tmp6B7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp6B7;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2 != 0){_LL3: _tmp191=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp190)->f2)->hd;_LL4: {void*t=_tmp191;
# 859
void*_tmp19B=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp19B;void*_tmp19C=_stmttmp29;switch(*((int*)_tmp19C)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 864
 return e;}_LL0:;}}
# 869
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19D=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->tag=5U,(_tmp19E->f1).tvars=0,(_tmp19E->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6B8=Cyc_Absyn_empty_tqual(0U);(_tmp19E->f1).ret_tqual=_tmp6B8;}),(_tmp19E->f1).ret_type=Cyc_Absyn_void_type,(_tmp19E->f1).args=0,(_tmp19E->f1).c_varargs=0,(_tmp19E->f1).cyc_varargs=0,(_tmp19E->f1).rgn_po=0,(_tmp19E->f1).attributes=0,(_tmp19E->f1).requires_clause=0,(_tmp19E->f1).requires_relns=0,(_tmp19E->f1).ensures_clause=0,(_tmp19E->f1).ensures_relns=0;_tmp19E;});
# 870
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp19D;
# 879
return({void*_tmp6BC=(void*)fntype;void*_tmp6BB=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6BA=Cyc_Absyn_empty_tqual(0U);void*_tmp6B9=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp6BC,_tmp6BB,_tmp6BA,_tmp6B9,Cyc_Absyn_false_type);});}
# 886
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp19F=Cyc_Tcutil_compress(e);void*_stmttmp2A=_tmp19F;void*_tmp1A0=_stmttmp2A;struct Cyc_Core_Opt*_tmp1A3;void**_tmp1A2;struct Cyc_Core_Opt*_tmp1A1;void*_tmp1A4;struct Cyc_List_List*_tmp1A5;void*_tmp1A6;switch(*((int*)_tmp1A0)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2 != 0){_LL1: _tmp1A6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->hd;_LL2: {void*r2=_tmp1A6;
# 893
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Tcutil_compress(r2);
if(r == r2)return 1;{
struct _tuple14 _tmp1A7=({struct _tuple14 _tmp5FB;_tmp5FB.f1=r,_tmp5FB.f2=r2;_tmp5FB;});struct _tuple14 _stmttmp2B=_tmp1A7;struct _tuple14 _tmp1A8=_stmttmp2B;struct Cyc_Absyn_Tvar*_tmp1AA;struct Cyc_Absyn_Tvar*_tmp1A9;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8.f2)->tag == 2U){_LLC: _tmp1A9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8.f1)->f1;_tmp1AA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A8.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1A9;struct Cyc_Absyn_Tvar*tv2=_tmp1AA;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1A5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2;_LL4: {struct Cyc_List_List*es=_tmp1A5;
# 901
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2 != 0){_LL5: _tmp1A4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->hd;_LL6: {void*t=_tmp1A4;
# 905
void*_tmp1AB=Cyc_Tcutil_rgns_of(t);void*_stmttmp2C=_tmp1AB;void*_tmp1AC=_stmttmp2C;void*_tmp1AD;void*_tmp1AE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f2 != 0){_LL11: _tmp1AE=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AC)->f2)->hd;_LL12: {void*t=_tmp1AE;
# 907
if(!constrain)return 0;{
void*_tmp1AF=Cyc_Tcutil_compress(t);void*_stmttmp2D=_tmp1AF;void*_tmp1B0=_stmttmp2D;struct Cyc_Core_Opt*_tmp1B3;void**_tmp1B2;struct Cyc_Core_Opt*_tmp1B1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B0)->tag == 1U){_LL16: _tmp1B1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B0)->f1;_tmp1B2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B0)->f2;_tmp1B3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B0)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1B1;void**p=_tmp1B2;struct Cyc_Core_Opt*s=_tmp1B3;
# 912
void*_tmp1B4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1B4;
# 915
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp6BE=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1B5[2U];_tmp1B5[0]=ev,({void*_tmp6BD=Cyc_Absyn_access_eff(r);_tmp1B5[1]=_tmp6BD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B5,sizeof(void*),2U));})));*p=_tmp6BE;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1AD=_tmp1AC;_LL14: {void*e2=_tmp1AD;
# 920
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1A1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f1;_tmp1A2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f2;_tmp1A3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A0)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1A1;void**p=_tmp1A2;struct Cyc_Core_Opt*s=_tmp1A3;
# 923
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1B6=0U;({struct _fat_ptr _tmp6BF=({const char*_tmp1B7="effect evar has wrong kind";_tag_fat(_tmp1B7,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6BF,_tag_fat(_tmp1B6,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 928
void*_tmp1B8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1B8;
# 931
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);{
void*_tmp1B9=Cyc_Absyn_join_eff(({void*_tmp1BA[2U];_tmp1BA[0]=ev,({void*_tmp6C0=Cyc_Absyn_access_eff(r);_tmp1BA[1]=_tmp6C0;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BA,sizeof(void*),2U));}));void*new_typ=_tmp1B9;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 942
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1BB=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_stmttmp2E=_tmp1BB;void*_tmp1BC=_stmttmp2E;struct Cyc_Core_Opt*_tmp1BF;void**_tmp1BE;struct Cyc_Core_Opt*_tmp1BD;void*_tmp1C0;struct Cyc_List_List*_tmp1C1;switch(*((int*)_tmp1BC)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1C1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2;_LL4: {struct Cyc_List_List*es=_tmp1C1;
# 947
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2 != 0){_LL5: _tmp1C0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1BC)->f2)->hd;_LL6: {void*t2=_tmp1C0;
# 952
t2=Cyc_Tcutil_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1C2=Cyc_Tcutil_rgns_of(t);void*_stmttmp2F=_tmp1C2;void*_tmp1C3=_stmttmp2F;void*_tmp1C4;void*_tmp1C5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f2 != 0){_LLC: _tmp1C5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f2)->hd;_LLD: {void*t3=_tmp1C5;
# 957
struct _tuple14 _tmp1C6=({struct _tuple14 _tmp5FC;({void*_tmp6C1=Cyc_Tcutil_compress(t3);_tmp5FC.f1=_tmp6C1;}),_tmp5FC.f2=t2;_tmp5FC;});struct _tuple14 _stmttmp30=_tmp1C6;struct _tuple14 _tmp1C7=_stmttmp30;struct Cyc_Absyn_Tvar*_tmp1C9;struct Cyc_Absyn_Tvar*_tmp1C8;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C7.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C7.f2)->tag == 2U){_LL11: _tmp1C8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C7.f1)->f1;_tmp1C9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C7.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1C8;struct Cyc_Absyn_Tvar*tv2=_tmp1C9;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1C4=_tmp1C3;_LLF: {void*e2=_tmp1C4;
# 961
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1BD=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f1;_tmp1BE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f2;_tmp1BF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BC)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1BD;void**p=_tmp1BE;struct Cyc_Core_Opt*s=_tmp1BF;
# 964
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1CA=0U;({struct _fat_ptr _tmp6C2=({const char*_tmp1CB="effect evar has wrong kind";_tag_fat(_tmp1CB,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C2,_tag_fat(_tmp1CA,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 969
void*_tmp1CC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1CC;
# 972
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1CD=Cyc_Absyn_join_eff(({void*_tmp1CE[2U];_tmp1CE[0]=ev,({void*_tmp6C3=Cyc_Absyn_regionsof_eff(t);_tmp1CE[1]=_tmp6C3;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CE,sizeof(void*),2U));}));void*new_typ=_tmp1CD;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 983
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1CF=e;struct Cyc_Core_Opt*_tmp1D2;void**_tmp1D1;struct Cyc_Core_Opt*_tmp1D0;void*_tmp1D3;struct Cyc_List_List*_tmp1D4;struct Cyc_Absyn_Tvar*_tmp1D5;switch(*((int*)_tmp1CF)){case 2U: _LL1: _tmp1D5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CF)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1D5;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f1)){case 9U: _LL3: _tmp1D4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2;_LL4: {struct Cyc_List_List*es=_tmp1D4;
# 988
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2 != 0){_LL5: _tmp1D3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CF)->f2)->hd;_LL6: {void*t=_tmp1D3;
# 993
void*_tmp1D6=Cyc_Tcutil_rgns_of(t);void*_stmttmp31=_tmp1D6;void*_tmp1D7=_stmttmp31;void*_tmp1D8;void*_tmp1D9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D7)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D7)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D7)->f2 != 0){_LLC: _tmp1D9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D7)->f2)->hd;_LLD: {void*t2=_tmp1D9;
# 995
if(!may_constrain_evars)return 0;{
void*_tmp1DA=Cyc_Tcutil_compress(t2);void*_stmttmp32=_tmp1DA;void*_tmp1DB=_stmttmp32;struct Cyc_Core_Opt*_tmp1DE;void**_tmp1DD;struct Cyc_Core_Opt*_tmp1DC;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DB)->tag == 1U){_LL11: _tmp1DC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DB)->f1;_tmp1DD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DB)->f2;_tmp1DE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DB)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1DC;void**p=_tmp1DD;struct Cyc_Core_Opt*s=_tmp1DE;
# 1001
void*_tmp1DF=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1DF;
# 1003
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp6C5=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1E0[2U];_tmp1E0[0]=ev,({void*_tmp6C4=Cyc_Absyn_var_type(v);_tmp1E0[1]=_tmp6C4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E0,sizeof(void*),2U));})));*p=_tmp6C5;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1D8=_tmp1D7;_LLF: {void*e2=_tmp1D8;
# 1008
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1D0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f1;_tmp1D1=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f2;_tmp1D2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1CF)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1D0;void**p=_tmp1D1;struct Cyc_Core_Opt*s=_tmp1D2;
# 1011
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1E1=0U;({struct _fat_ptr _tmp6C6=({const char*_tmp1E2="effect evar has wrong kind";_tag_fat(_tmp1E2,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C6,_tag_fat(_tmp1E1,sizeof(void*),0U));});});{
# 1015
void*_tmp1E3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E3;
# 1017
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1E4=Cyc_Absyn_join_eff(({void*_tmp1E5[2U];_tmp1E5[0]=ev,({void*_tmp6C7=Cyc_Absyn_var_type(v);_tmp1E5[1]=_tmp6C7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E5,sizeof(void*),2U));}));void*new_typ=_tmp1E4;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1027
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1E6=e;void*_tmp1E7;struct Cyc_List_List*_tmp1E8;switch(*((int*)_tmp1E6)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f1)){case 9U: _LL1: _tmp1E8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2;_LL2: {struct Cyc_List_List*es=_tmp1E8;
# 1031
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2 != 0){_LL3: _tmp1E7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E6)->f2)->hd;_LL4: {void*t=_tmp1E7;
# 1036
void*_tmp1E9=Cyc_Tcutil_rgns_of(t);void*_stmttmp33=_tmp1E9;void*_tmp1EA=_stmttmp33;void*_tmp1EB;void*_tmp1EC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f2 != 0){_LLA: _tmp1EC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f2)->hd;_LLB: {void*t2=_tmp1EC;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1EB=_tmp1EA;_LLD: {void*e2=_tmp1EB;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1040
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 1054 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1059
void*_tmp1ED=Cyc_Tcutil_compress(e1);void*_stmttmp34=_tmp1ED;void*_tmp1EE=_stmttmp34;struct Cyc_Core_Opt*_tmp1F0;void**_tmp1EF;struct Cyc_Absyn_Tvar*_tmp1F1;void*_tmp1F2;void*_tmp1F3;struct Cyc_List_List*_tmp1F4;switch(*((int*)_tmp1EE)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f1)){case 9U: _LL1: _tmp1F4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2;_LL2: {struct Cyc_List_List*es=_tmp1F4;
# 1061
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2 != 0){_LL3: _tmp1F3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2)->hd;_LL4: {void*r=_tmp1F3;
# 1072
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2 != 0){_LL7: _tmp1F2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2)->hd;_LL8: {void*t=_tmp1F2;
# 1076
void*_tmp1F5=Cyc_Tcutil_rgns_of(t);void*_stmttmp35=_tmp1F5;void*_tmp1F6=_stmttmp35;void*_tmp1F7;void*_tmp1F8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f2 != 0){_LLE: _tmp1F8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F6)->f2)->hd;_LLF: {void*t2=_tmp1F8;
# 1081
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1F7=_tmp1F6;_LL11: {void*e=_tmp1F7;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1F1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1EE)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1F1;
# 1074
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1EF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EE)->f2;_tmp1F0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1EE)->f4;_LLA: {void**p=_tmp1EF;struct Cyc_Core_Opt*s=_tmp1F0;
# 1086
if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1090
*p=Cyc_Absyn_empty_effect;
# 1093
return 1;}else{
# 1095
return 0;}}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp1FB=({struct Cyc_String_pa_PrintArg_struct _tmp5FD;_tmp5FD.tag=0U,({struct _fat_ptr _tmp6C8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp5FD.f1=_tmp6C8;});_tmp5FD;});void*_tmp1F9[1U];_tmp1F9[0]=& _tmp1FB;({struct _fat_ptr _tmp6C9=({const char*_tmp1FA="subset_effect: bad effect: %s";_tag_fat(_tmp1FA,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C9,_tag_fat(_tmp1F9,sizeof(void*),1U));});});}_LL0:;}
# 1106
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1108
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple14*_tmp1FC=(struct _tuple14*)r1->hd;struct _tuple14*_stmttmp36=_tmp1FC;struct _tuple14*_tmp1FD=_stmttmp36;void*_tmp1FF;void*_tmp1FE;_LL1: _tmp1FE=_tmp1FD->f1;_tmp1FF=_tmp1FD->f2;_LL2: {void*t1a=_tmp1FE;void*t1b=_tmp1FF;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple14*_tmp200=(struct _tuple14*)r2->hd;struct _tuple14*_stmttmp37=_tmp200;struct _tuple14*_tmp201=_stmttmp37;void*_tmp203;void*_tmp202;_LL4: _tmp202=_tmp201->f1;_tmp203=_tmp201->f2;_LL5: {void*t2a=_tmp202;void*t2b=_tmp203;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1118
if(!found)return 0;}}}
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
({void*_tmp205=0U;({struct _fat_ptr _tmp6CA=({const char*_tmp206="bad con";_tag_fat(_tmp206,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CA,_tag_fat(_tmp205,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp204)->f1){case 0U: _LL21: _LL22:
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
 return 11;default: _LL17: _tmp208=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f1;_LL18: {void*c=_tmp208;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1189
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp209=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp209;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1194
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp20A=i;struct _tuple1*_tmp20B;struct Cyc_Absyn_Datatypedecl*_tmp20C;if((_tmp20A.KnownDatatype).tag == 2){_LL1: _tmp20C=*(_tmp20A.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp20C;
return dd->name;}}else{_LL3: _tmp20B=((_tmp20A.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp20B;
return n;}}_LL0:;}struct _tuple19{struct _tuple1*f1;struct _tuple1*f2;};
# 1200
static struct _tuple19 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp20D=i;struct _tuple1*_tmp20F;struct _tuple1*_tmp20E;struct Cyc_Absyn_Datatypefield*_tmp211;struct Cyc_Absyn_Datatypedecl*_tmp210;if((_tmp20D.KnownDatatypefield).tag == 2){_LL1: _tmp210=((_tmp20D.KnownDatatypefield).val).f1;_tmp211=((_tmp20D.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp210;struct Cyc_Absyn_Datatypefield*df=_tmp211;
# 1203
return({struct _tuple19 _tmp5FE;_tmp5FE.f1=dd->name,_tmp5FE.f2=df->name;_tmp5FE;});}}else{_LL3: _tmp20E=((_tmp20D.UnknownDatatypefield).val).datatype_name;_tmp20F=((_tmp20D.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp20E;struct _tuple1*f=_tmp20F;
# 1205
return({struct _tuple19 _tmp5FF;_tmp5FF.f1=d,_tmp5FF.f2=f;_tmp5FF;});}}_LL0:;}struct _tuple20{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1208
static struct _tuple20 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp212=i;struct Cyc_Absyn_Aggrdecl*_tmp213;struct _tuple1*_tmp215;enum Cyc_Absyn_AggrKind _tmp214;if((_tmp212.UnknownAggr).tag == 1){_LL1: _tmp214=((_tmp212.UnknownAggr).val).f1;_tmp215=((_tmp212.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp214;struct _tuple1*n=_tmp215;
return({struct _tuple20 _tmp600;_tmp600.f1=k,_tmp600.f2=n;_tmp600;});}}else{_LL3: _tmp213=*(_tmp212.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp213;
return({struct _tuple20 _tmp601;_tmp601.f1=ad->kind,_tmp601.f2=ad->name;_tmp601;});}}_LL0:;}
# 1214
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1220
struct _tuple14 _tmp216=({struct _tuple14 _tmp602;_tmp602.f1=t1,_tmp602.f2=t2;_tmp602;});struct _tuple14 _stmttmp38=_tmp216;struct _tuple14 _tmp217=_stmttmp38;union Cyc_Absyn_AggrInfo _tmp219;union Cyc_Absyn_AggrInfo _tmp218;union Cyc_Absyn_DatatypeFieldInfo _tmp21B;union Cyc_Absyn_DatatypeFieldInfo _tmp21A;union Cyc_Absyn_DatatypeInfo _tmp21D;union Cyc_Absyn_DatatypeInfo _tmp21C;struct Cyc_List_List*_tmp21F;struct Cyc_List_List*_tmp21E;struct _fat_ptr _tmp221;struct _fat_ptr _tmp220;struct _tuple1*_tmp223;struct _tuple1*_tmp222;switch(*((int*)_tmp217.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 15U){_LL1: _tmp222=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp223=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL2: {struct _tuple1*n1=_tmp222;struct _tuple1*n2=_tmp223;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 17U){_LL3: _tmp220=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp221=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp220;struct _fat_ptr s2=_tmp221;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 16U){_LL5: _tmp21E=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp21F=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp21E;struct Cyc_List_List*fs2=_tmp21F;
# 1224
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 18U){_LL7: _tmp21C=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp21D=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp21C;union Cyc_Absyn_DatatypeInfo info2=_tmp21D;
# 1226
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(info1);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(info2);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 19U){_LL9: _tmp21A=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp21B=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp21A;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp21B;
# 1230
struct _tuple19 _tmp224=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple19 _stmttmp39=_tmp224;struct _tuple19 _tmp225=_stmttmp39;struct _tuple1*_tmp227;struct _tuple1*_tmp226;_LL10: _tmp226=_tmp225.f1;_tmp227=_tmp225.f2;_LL11: {struct _tuple1*d1=_tmp226;struct _tuple1*f1=_tmp227;
struct _tuple19 _tmp228=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple19 _stmttmp3A=_tmp228;struct _tuple19 _tmp229=_stmttmp3A;struct _tuple1*_tmp22B;struct _tuple1*_tmp22A;_LL13: _tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;_LL14: {struct _tuple1*d2=_tmp22A;struct _tuple1*f2=_tmp22B;
int _tmp22C=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp22C;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f2)->tag == 20U){_LLB: _tmp218=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f1)->f1;_tmp219=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp217.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp218;union Cyc_Absyn_AggrInfo info2=_tmp219;
# 1236
struct _tuple20 _tmp22D=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple20 _stmttmp3B=_tmp22D;struct _tuple20 _tmp22E=_stmttmp3B;struct _tuple1*_tmp230;enum Cyc_Absyn_AggrKind _tmp22F;_LL16: _tmp22F=_tmp22E.f1;_tmp230=_tmp22E.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp22F;struct _tuple1*q1=_tmp230;
struct _tuple20 _tmp231=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple20 _stmttmp3C=_tmp231;struct _tuple20 _tmp232=_stmttmp3C;struct _tuple1*_tmp234;enum Cyc_Absyn_AggrKind _tmp233;_LL19: _tmp233=_tmp232.f1;_tmp234=_tmp232.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp233;struct _tuple1*q2=_tmp234;
int _tmp235=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp235;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1245
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6CC)(void*,void*)=cmp;void*_tmp6CB=(void*)_check_null(a1);_tmp6CC(_tmp6CB,(void*)_check_null(a2));});}
# 1251
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp236=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp236;
int _tmp237=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp237;
return Cyc_Core_intcmp(i1,i2);}
# 1256
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct _tuple11*tqt2){
struct _tuple11*_tmp238=tqt1;void*_tmp23A;struct Cyc_Absyn_Tqual _tmp239;_LL1: _tmp239=_tmp238->f1;_tmp23A=_tmp238->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp239;void*t1=_tmp23A;
struct _tuple11*_tmp23B=tqt2;void*_tmp23D;struct Cyc_Absyn_Tqual _tmp23C;_LL4: _tmp23C=_tmp23B->f1;_tmp23D=_tmp23B->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp23C;void*t2=_tmp23D;
int _tmp23E=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp23E;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1264
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp23F=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp23F;
if(zsc != 0)return zsc;{
int _tmp240=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp240;
if(tqc != 0)return tqc;{
int _tmp241=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp241;
if(tc != 0)return tc;{
int _tmp242=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp242;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1281
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp243=({int _tmp6CD=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6CD,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp243;
if(shallowcmp != 0)
return shallowcmp;{
# 1290
struct _tuple14 _tmp244=({struct _tuple14 _tmp603;_tmp603.f1=t2,_tmp603.f2=t1;_tmp603;});struct _tuple14 _stmttmp3D=_tmp244;struct _tuple14 _tmp245=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp247;struct Cyc_Absyn_Exp*_tmp246;struct Cyc_Absyn_Exp*_tmp249;struct Cyc_Absyn_Exp*_tmp248;struct Cyc_List_List*_tmp24D;enum Cyc_Absyn_AggrKind _tmp24C;struct Cyc_List_List*_tmp24B;enum Cyc_Absyn_AggrKind _tmp24A;struct Cyc_List_List*_tmp24F;struct Cyc_List_List*_tmp24E;struct Cyc_Absyn_FnInfo _tmp251;struct Cyc_Absyn_FnInfo _tmp250;void*_tmp259;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Tqual _tmp257;void*_tmp256;void*_tmp255;struct Cyc_Absyn_Exp*_tmp254;struct Cyc_Absyn_Tqual _tmp253;void*_tmp252;void*_tmp265;void*_tmp264;void*_tmp263;void*_tmp262;struct Cyc_Absyn_Tqual _tmp261;void*_tmp260;void*_tmp25F;void*_tmp25E;void*_tmp25D;void*_tmp25C;struct Cyc_Absyn_Tqual _tmp25B;void*_tmp25A;struct Cyc_Absyn_Tvar*_tmp267;struct Cyc_Absyn_Tvar*_tmp266;struct Cyc_List_List*_tmp26B;void*_tmp26A;struct Cyc_List_List*_tmp269;void*_tmp268;switch(*((int*)_tmp245.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->tag == 0U){_LL1: _tmp268=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp269=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f1)->f2;_tmp26A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->f1;_tmp26B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp245.f2)->f2;_LL2: {void*c1=_tmp268;struct Cyc_List_List*ts1=_tmp269;void*c2=_tmp26A;struct Cyc_List_List*ts2=_tmp26B;
# 1292
int _tmp26C=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp26C;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp245.f2)->tag == 1U){_LL3: _LL4:
# 1296
({void*_tmp26D=0U;({struct _fat_ptr _tmp6CE=({const char*_tmp26E="typecmp: can only compare closed types";_tag_fat(_tmp26E,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CE,_tag_fat(_tmp26D,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f2)->tag == 2U){_LL5: _tmp266=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp267=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp266;struct Cyc_Absyn_Tvar*tv1=_tmp267;
# 1300
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->tag == 3U){_LL7: _tmp25A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_type;_tmp25B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_tq;_tmp25C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).rgn;_tmp25D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).nullable;_tmp25E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).bounds;_tmp25F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f1)->f1).ptr_atts).zero_term;_tmp260=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_type;_tmp261=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_tq;_tmp262=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).rgn;_tmp263=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).nullable;_tmp264=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).bounds;_tmp265=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp245.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp25A;struct Cyc_Absyn_Tqual tqual2a=_tmp25B;void*rgn2=_tmp25C;void*null2a=_tmp25D;void*b2=_tmp25E;void*zt2=_tmp25F;void*t1a=_tmp260;struct Cyc_Absyn_Tqual tqual1a=_tmp261;void*rgn1=_tmp262;void*null1a=_tmp263;void*b1=_tmp264;void*zt1=_tmp265;
# 1304
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
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->tag == 4U){_LL9: _tmp252=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).elt_type;_tmp253=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).tq;_tmp254=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).num_elts;_tmp255=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f1)->f1).zero_term;_tmp256=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).elt_type;_tmp257=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).tq;_tmp258=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).num_elts;_tmp259=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp245.f2)->f1).zero_term;_LLA: {void*t2a=_tmp252;struct Cyc_Absyn_Tqual tq2a=_tmp253;struct Cyc_Absyn_Exp*e1=_tmp254;void*zt1=_tmp255;void*t1a=_tmp256;struct Cyc_Absyn_Tqual tq1a=_tmp257;struct Cyc_Absyn_Exp*e2=_tmp258;void*zt2=_tmp259;
# 1320
int _tmp275=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp275;
if(tqc != 0)return tqc;{
int _tmp276=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp276;
if(tc != 0)return tc;{
int _tmp277=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp277;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({void*_tmp278=0U;({struct _fat_ptr _tmp6CF=({const char*_tmp279="missing expression in array index";_tag_fat(_tmp279,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CF,_tag_fat(_tmp278,sizeof(void*),0U));});});
# 1330
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f2)->tag == 5U){_LLB: _tmp250=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp251=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp245.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp250;struct Cyc_Absyn_FnInfo f2=_tmp251;
# 1333
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp27A=f1.args;struct Cyc_List_List*args1=_tmp27A;
struct Cyc_List_List*_tmp27B=f2.args;struct Cyc_List_List*args2=_tmp27B;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp27C=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3E=_tmp27C;struct _tuple9 _tmp27D=_stmttmp3E;void*_tmp27F;struct Cyc_Absyn_Tqual _tmp27E;_LL18: _tmp27E=_tmp27D.f2;_tmp27F=_tmp27D.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp27E;void*t1=_tmp27F;
struct _tuple9 _tmp280=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp3F=_tmp280;struct _tuple9 _tmp281=_stmttmp3F;void*_tmp283;struct Cyc_Absyn_Tqual _tmp282;_LL1B: _tmp282=_tmp281.f2;_tmp283=_tmp281.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp282;void*t2=_tmp283;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1348
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp6D0=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp6D0,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1362
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp284=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp284;
struct Cyc_List_List*_tmp285=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp285;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple14 _tmp286=*((struct _tuple14*)rpo1->hd);struct _tuple14 _stmttmp40=_tmp286;struct _tuple14 _tmp287=_stmttmp40;void*_tmp289;void*_tmp288;_LL1E: _tmp288=_tmp287.f1;_tmp289=_tmp287.f2;_LL1F: {void*t1a=_tmp288;void*t1b=_tmp289;
struct _tuple14 _tmp28A=*((struct _tuple14*)rpo2->hd);struct _tuple14 _stmttmp41=_tmp28A;struct _tuple14 _tmp28B=_stmttmp41;void*_tmp28D;void*_tmp28C;_LL21: _tmp28C=_tmp28B.f1;_tmp28D=_tmp28B.f2;_LL22: {void*t2a=_tmp28C;void*t2b=_tmp28D;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1372
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1380
({void*_tmp28E=0U;({struct _fat_ptr _tmp6D1=({const char*_tmp28F="typecmp: function type comparison should never get here!";_tag_fat(_tmp28F,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D1,_tag_fat(_tmp28E,sizeof(void*),0U));});});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f2)->tag == 6U){_LLD: _tmp24E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp24F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp245.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp24E;struct Cyc_List_List*ts1=_tmp24F;
# 1383
return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->tag == 7U){_LLF: _tmp24A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp24B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f1)->f2;_tmp24C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->f1;_tmp24D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp245.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp24A;struct Cyc_List_List*fs2=_tmp24B;enum Cyc_Absyn_AggrKind k1=_tmp24C;struct Cyc_List_List*fs1=_tmp24D;
# 1386
if((int)k1 != (int)k2){
if((int)k1 == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f2)->tag == 9U){_LL11: _tmp248=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp249=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp248;struct Cyc_Absyn_Exp*e2=_tmp249;
# 1391
_tmp246=e1;_tmp247=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f2)->tag == 11U){_LL13: _tmp246=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f1)->f1;_tmp247=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp245.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp246;struct Cyc_Absyn_Exp*e2=_tmp247;
# 1393
return Cyc_Evexp_const_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp290=0U;({struct _fat_ptr _tmp6D2=({const char*_tmp291="Unmatched case in typecmp";_tag_fat(_tmp291,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D2,_tag_fat(_tmp290,sizeof(void*),0U));});});}_LL0:;}}}
# 1400
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple14 _tmp292=({struct _tuple14 _tmp605;({void*_tmp6D4=Cyc_Tcutil_compress(t1);_tmp605.f1=_tmp6D4;}),({void*_tmp6D3=Cyc_Tcutil_compress(t2);_tmp605.f2=_tmp6D3;});_tmp605;});struct _tuple14 _stmttmp42=_tmp292;struct _tuple14 _tmp293=_stmttmp42;void*_tmp295;void*_tmp294;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp293.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp293.f2)->tag == 0U){_LL1: _tmp294=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp293.f1)->f1;_tmp295=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp293.f2)->f1;_LL2: {void*c1=_tmp294;void*c2=_tmp295;
# 1403
struct _tuple14 _tmp296=({struct _tuple14 _tmp604;_tmp604.f1=c1,_tmp604.f2=c2;_tmp604;});struct _tuple14 _stmttmp43=_tmp296;struct _tuple14 _tmp297=_stmttmp43;int _tmp299;int _tmp298;switch(*((int*)_tmp297.f1)){case 2U: switch(*((int*)_tmp297.f2)){case 2U: _LL6: _tmp298=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp297.f1)->f1;_tmp299=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp297.f2)->f1;_LL7: {int i1=_tmp298;int i2=_tmp299;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp297.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1411
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1416
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1419
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp297.f2)->f1 == 0){_LL14: _LL15:
# 1414
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp297.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1412
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1417
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1420
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp297.f2)->f1 == 0){_LL16: _LL17:
# 1415
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1421
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp297.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
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
{struct _tuple14 _tmp29A=({struct _tuple14 _tmp607;({void*_tmp6D6=Cyc_Tcutil_compress(t1);_tmp607.f1=_tmp6D6;}),({void*_tmp6D5=Cyc_Tcutil_compress(t2);_tmp607.f2=_tmp6D5;});_tmp607;});struct _tuple14 _stmttmp44=_tmp29A;struct _tuple14 _tmp29B=_stmttmp44;void*_tmp29D;void*_tmp29C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B.f2)->tag == 0U){_LL1: _tmp29C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B.f1)->f1;_tmp29D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29B.f2)->f1;_LL2: {void*c1=_tmp29C;void*c2=_tmp29D;
# 1433
{struct _tuple14 _tmp29E=({struct _tuple14 _tmp606;_tmp606.f1=c1,_tmp606.f2=c2;_tmp606;});struct _tuple14 _stmttmp45=_tmp29E;struct _tuple14 _tmp29F=_stmttmp45;int _tmp2A1;int _tmp2A0;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29F.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29F.f2)->tag == 2U){_LL6: _tmp2A0=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29F.f1)->f1;_tmp2A1=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29F.f2)->f1;_LL7: {int i1=_tmp2A0;int i2=_tmp2A1;
# 1435
if(i1 != 0 && i1 != 1)return t1;else{
if(i2 != 0 && i2 != 1)return t2;else{
if(i1 >= i2)return t1;else{
return t2;}}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29F.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1445
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29F.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1450
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1443
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29F.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1452
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1442
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1446
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29F.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1451
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1444
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29F.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29F.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1453
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29F.f1)->tag == 4U){_LL18: _LL19:
# 1448
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp29F.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1454
 goto _LL5;}}}}}}_LL5:;}
# 1456
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
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
({struct Cyc_String_pa_PrintArg_struct _tmp2A5=({struct Cyc_String_pa_PrintArg_struct _tmp609;_tmp609.tag=0U,({
struct _fat_ptr _tmp6D7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp609.f1=_tmp6D7;});_tmp609;});struct Cyc_String_pa_PrintArg_struct _tmp2A6=({struct Cyc_String_pa_PrintArg_struct _tmp608;_tmp608.tag=0U,({struct _fat_ptr _tmp6D8=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp608.f1=_tmp6D8;});_tmp608;});void*_tmp2A3[2U];_tmp2A3[0]=& _tmp2A5,_tmp2A3[1]=& _tmp2A6;({unsigned _tmp6DA=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _fat_ptr _tmp6D9=({const char*_tmp2A4="type mismatch: expecting %s but found %s";_tag_fat(_tmp2A4,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp6DA,_tmp6D9,_tag_fat(_tmp2A3,sizeof(void*),2U));});});
return 0;}}}
# 1486
return 1;}
# 1491
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2A7=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp46=_tmp2A7;void*_tmp2A8=_stmttmp46;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2A8)->tag == 3U){_LL1: _LL2:
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
({void*_tmp2A9=0U;({unsigned _tmp6DC=e->loc;struct _fat_ptr _tmp6DB=({const char*_tmp2AA="integral size mismatch; conversion supplied";_tag_fat(_tmp2AA,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6DC,_tmp6DB,_tag_fat(_tmp2A9,sizeof(void*),0U));});});
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
({void*_tmp2AB=0U;({unsigned _tmp6DE=e->loc;struct _fat_ptr _tmp6DD=({const char*_tmp2AC="integral size mismatch; conversion supplied";_tag_fat(_tmp2AC,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6DE,_tmp6DD,_tag_fat(_tmp2AB,sizeof(void*),0U));});});
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
({void*_tmp6DF=t;Cyc_Unify_unify(_tmp6DF,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1541
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2AD=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2AD;
void*_tmp2AE=t;struct Cyc_Absyn_Exp*_tmp2AF;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2AE)->tag == 9U){_LL1: _tmp2AF=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2AE)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2AF;
return e;}}else{_LL3: _LL4: {
# 1546
struct Cyc_Absyn_Exp*_tmp2B0=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2B0;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1555
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B1=Cyc_Tcutil_compress(b);void*_stmttmp47=_tmp2B1;void*_tmp2B2=_stmttmp47;struct Cyc_List_List*_tmp2B3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B2)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2B3=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B2)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2B3;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp2B6=({struct Cyc_String_pa_PrintArg_struct _tmp60A;_tmp60A.tag=0U,({struct _fat_ptr _tmp6E0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp60A.f1=_tmp6E0;});_tmp60A;});void*_tmp2B4[1U];_tmp2B4[0]=& _tmp2B6;({struct _fat_ptr _tmp6E1=({const char*_tmp2B5="get_bounds_exp: %s";_tag_fat(_tmp2B5,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E1,_tag_fat(_tmp2B4,sizeof(void*),1U));});});}_LL0:;}}
# 1564
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2B7=Cyc_Tcutil_compress(t);void*_stmttmp48=_tmp2B7;void*_tmp2B8=_stmttmp48;void*_tmp2B9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B8)->tag == 3U){_LL1: _tmp2B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B8)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2B9;
# 1567
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2BC=({struct Cyc_String_pa_PrintArg_struct _tmp60B;_tmp60B.tag=0U,({struct _fat_ptr _tmp6E2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp60B.f1=_tmp6E2;});_tmp60B;});void*_tmp2BA[1U];_tmp2BA[0]=& _tmp2BC;({struct _fat_ptr _tmp6E3=({const char*_tmp2BB="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp2BB,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E3,_tag_fat(_tmp2BA,sizeof(void*),1U));});});}_LL0:;}
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
struct _tuple14 _tmp2BF=({struct _tuple14 _tmp60F;_tmp60F.f1=t1,_tmp60F.f2=t2;_tmp60F;});struct _tuple14 _stmttmp49=_tmp2BF;struct _tuple14 _tmp2C0=_stmttmp49;void*_tmp2C8;struct Cyc_Absyn_Exp*_tmp2C7;struct Cyc_Absyn_Tqual _tmp2C6;void*_tmp2C5;void*_tmp2C4;struct Cyc_Absyn_Exp*_tmp2C3;struct Cyc_Absyn_Tqual _tmp2C2;void*_tmp2C1;struct Cyc_Absyn_PtrInfo _tmp2CA;struct Cyc_Absyn_PtrInfo _tmp2C9;switch(*((int*)_tmp2C0.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0.f2)->tag == 3U){_LL1: _tmp2C9=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0.f1)->f1;_tmp2CA=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2C9;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2CA;
# 1595
int okay=1;
# 1597
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable))
# 1599
okay=!Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable);
# 1601
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple21 _tmp2CB=({struct _tuple21 _tmp60C;({struct Cyc_Absyn_Exp*_tmp6E7=({void*_tmp6E6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6E6,(pinfo_a.ptr_atts).bounds);});_tmp60C.f1=_tmp6E7;}),({
struct Cyc_Absyn_Exp*_tmp6E5=({void*_tmp6E4=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6E4,(pinfo_b.ptr_atts).bounds);});_tmp60C.f2=_tmp6E5;});_tmp60C;});
# 1602
struct _tuple21 _stmttmp4A=_tmp2CB;struct _tuple21 _tmp2CC=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Absyn_Exp*_tmp2CD;if(_tmp2CC.f2 == 0){_LLA: _LLB:
# 1605
 okay=1;goto _LL9;}else{if(_tmp2CC.f1 == 0){_LLC: _LLD:
# 1608
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp6E8=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6E8,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp2CD=_tmp2CC.f1;_tmp2CE=_tmp2CC.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2CD;struct Cyc_Absyn_Exp*e2=_tmp2CE;
# 1614
okay=okay &&({struct Cyc_Absyn_Exp*_tmp6E9=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp6E9,(struct Cyc_Absyn_Exp*)_check_null(e1));});
# 1618
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({void*_tmp2CF=0U;({unsigned _tmp6EB=loc;struct _fat_ptr _tmp6EA=({const char*_tmp2D0="implicit cast to shorter array";_tag_fat(_tmp2D0,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp6EB,_tmp6EA,_tag_fat(_tmp2CF,sizeof(void*),0U));});});
goto _LL9;}}}_LL9:;}
# 1625
okay=okay &&(!(pinfo_a.elt_tq).real_const ||(pinfo_b.elt_tq).real_const);
# 1628
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
# 1631
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp2D3=({struct Cyc_String_pa_PrintArg_struct _tmp60E;_tmp60E.tag=0U,({
struct _fat_ptr _tmp6EC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((pinfo_a.ptr_atts).rgn));_tmp60E.f1=_tmp6EC;});_tmp60E;});struct Cyc_String_pa_PrintArg_struct _tmp2D4=({struct Cyc_String_pa_PrintArg_struct _tmp60D;_tmp60D.tag=0U,({
struct _fat_ptr _tmp6ED=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((pinfo_b.ptr_atts).rgn));_tmp60D.f1=_tmp6ED;});_tmp60D;});void*_tmp2D1[2U];_tmp2D1[0]=& _tmp2D3,_tmp2D1[1]=& _tmp2D4;({unsigned _tmp6EF=loc;struct _fat_ptr _tmp6EE=({const char*_tmp2D2="implicit cast from region %s to region %s";_tag_fat(_tmp2D2,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6EF,_tmp6EE,_tag_fat(_tmp2D1,sizeof(void*),2U));});});}else{
okay=0;}}
# 1639
okay=okay &&(Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)||
# 1641
 Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)&&(pinfo_b.elt_tq).real_const);{
# 1649
int _tmp2D5=
({void*_tmp6F0=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6F0,(pinfo_b.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term);
# 1649
int deep_subtype=_tmp2D5;
# 1655
okay=okay &&(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type)||
(deep_subtype &&((pinfo_b.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type))))&& Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type));
# 1658
return okay;}}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->tag == 4U){_LL3: _tmp2C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f1)->f1).elt_type;_tmp2C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f1)->f1).tq;_tmp2C3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f1)->f1).num_elts;_tmp2C4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f1)->f1).zero_term;_tmp2C5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->f1).elt_type;_tmp2C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->f1).tq;_tmp2C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->f1).num_elts;_tmp2C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C0.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2C1;struct Cyc_Absyn_Tqual tq1a=_tmp2C2;struct Cyc_Absyn_Exp*e1=_tmp2C3;void*zt1=_tmp2C4;void*t2a=_tmp2C5;struct Cyc_Absyn_Tqual tq2a=_tmp2C6;struct Cyc_Absyn_Exp*e2=_tmp2C7;void*zt2=_tmp2C8;
# 1662
int okay;
# 1665
okay=Cyc_Unify_unify(zt1,zt2)&&(
(e1 != 0 && e2 != 0)&& Cyc_Evexp_same_const_exp(e1,e2));
# 1668
return(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C0.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1670
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1672
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1676
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2D6=Cyc_Tcutil_compress(t);void*_stmttmp4B=_tmp2D6;void*_tmp2D7=_stmttmp4B;void*_tmp2D8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->tag == 3U){_LL1: _tmp2D8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D7)->f1).elt_type;_LL2: {void*e=_tmp2D8;
return e;}}else{_LL3: _LL4:
({void*_tmp2D9=0U;({struct _fat_ptr _tmp6F1=({const char*_tmp2DA="pointer_elt_type";_tag_fat(_tmp2DA,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F1,_tag_fat(_tmp2D9,sizeof(void*),0U));});});}_LL0:;}
# 1682
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2DB=Cyc_Tcutil_compress(t);void*_stmttmp4C=_tmp2DB;void*_tmp2DC=_stmttmp4C;struct Cyc_Absyn_PtrAtts*_tmp2DD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->tag == 3U){_LL1: _tmp2DD=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2DD;
return p->rgn;}}else{_LL3: _LL4:
({void*_tmp2DE=0U;({struct _fat_ptr _tmp6F2=({const char*_tmp2DF="pointer_elt_type";_tag_fat(_tmp2DF,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F2,_tag_fat(_tmp2DE,sizeof(void*),0U));});});}_LL0:;}
# 1689
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2E0=Cyc_Tcutil_compress(t);void*_stmttmp4D=_tmp2E0;void*_tmp2E1=_stmttmp4D;void*_tmp2E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->tag == 3U){_LL1: _tmp2E2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2E2;
# 1692
*rgn=r;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1701
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2E3=Cyc_Tcutil_compress(t);void*_stmttmp4E=_tmp2E3;void*_tmp2E4=_stmttmp4E;void*_tmp2E6;void*_tmp2E5;switch(*((int*)_tmp2E4)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E4)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2E5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E4)->f1).ptr_atts).nullable;_tmp2E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E4)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2E5;void*bounds=_tmp2E6;
# 1709
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1715
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2E7=e->r;void*_stmttmp4F=_tmp2E7;void*_tmp2E8=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp2EA;void*_tmp2E9;struct _fat_ptr _tmp2EB;switch(*((int*)_tmp2E8)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2EB=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp2EB;
# 1724
unsigned long _tmp2EC=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2EC;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 1734
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1722
 return 1;case 14U: _LLD: _tmp2E9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E8)->f1;_tmp2EA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2E8)->f2;_LLE: {void*t=_tmp2E9;struct Cyc_Absyn_Exp*e2=_tmp2EA;
# 1735
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
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
struct Cyc_Absyn_Kind*_tmp2ED=ka;enum Cyc_Absyn_AliasQual _tmp2EF;enum Cyc_Absyn_KindQual _tmp2EE;_LL1: _tmp2EE=_tmp2ED->kind;_tmp2EF=_tmp2ED->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp2EE;enum Cyc_Absyn_AliasQual a=_tmp2EF;
{enum Cyc_Absyn_AliasQual _tmp2F0=a;switch(_tmp2F0){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1782
enum Cyc_Absyn_KindQual _tmp2F1=k;switch(_tmp2F1){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1793
{enum Cyc_Absyn_KindQual _tmp2F2=k;switch(_tmp2F2){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1800
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1802
{enum Cyc_Absyn_KindQual _tmp2F3=k;switch(_tmp2F3){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1809
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1812
({struct Cyc_String_pa_PrintArg_struct _tmp2F6=({struct Cyc_String_pa_PrintArg_struct _tmp610;_tmp610.tag=0U,({struct _fat_ptr _tmp6F3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp610.f1=_tmp6F3;});_tmp610;});void*_tmp2F4[1U];_tmp2F4[0]=& _tmp2F6;({struct _fat_ptr _tmp6F4=({const char*_tmp2F5="kind_to_opt: bad kind %s\n";_tag_fat(_tmp2F5,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F4,_tag_fat(_tmp2F4,sizeof(void*),1U));});});}}
# 1815
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2F7=_cycalloc(sizeof(*_tmp2F7));_tmp2F7->tag=0U,_tmp2F7->f1=k;_tmp2F7;});}
# 1818
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp2F8=_cycalloc(sizeof(*_tmp2F8));({void*_tmp6F5=Cyc_Tcutil_kind_to_bound(k);_tmp2F8->v=_tmp6F5;});_tmp2F8;});}
# 1824
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp2F9=Cyc_Tcutil_compress(t2);void*_stmttmp50=_tmp2F9;void*_tmp2FA=_stmttmp50;void*_tmp2FB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->tag == 3U){_LL1: _tmp2FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FA)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp2FB;
# 1829
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp6F6=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp6F6;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1838
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple22{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1844
struct _tuple22 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1848
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp6F9=({struct _fat_ptr*_tmp30E=_cycalloc(sizeof(*_tmp30E));({struct _fat_ptr _tmp6F8=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp30D=({struct Cyc_Int_pa_PrintArg_struct _tmp612;_tmp612.tag=1U,_tmp612.f1=(unsigned long)counter ++;_tmp612;});void*_tmp30B[1U];_tmp30B[0]=& _tmp30D;({struct _fat_ptr _tmp6F7=({const char*_tmp30C="__aliasvar%d";_tag_fat(_tmp30C,sizeof(char),13U);});Cyc_aprintf(_tmp6F7,_tag_fat(_tmp30B,sizeof(void*),1U));});});*_tmp30E=_tmp6F8;});_tmp30E;});_tmp30F->f2=_tmp6F9;});_tmp30F;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp6FA=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp30A=_cycalloc(sizeof(*_tmp30A));_tmp30A->tag=4U,_tmp30A->f1=vd;_tmp30A;});Cyc_Absyn_varb_exp(_tmp6FA,e->loc);});
# 1857
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 1859
{void*_tmp2FC=Cyc_Tcutil_compress(e_type);void*_stmttmp51=_tmp2FC;void*_tmp2FD=_stmttmp51;struct Cyc_Absyn_PtrLoc*_tmp304;void*_tmp303;void*_tmp302;void*_tmp301;void*_tmp300;struct Cyc_Absyn_Tqual _tmp2FF;void*_tmp2FE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->tag == 3U){_LL1: _tmp2FE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_type;_tmp2FF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).elt_tq;_tmp300=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).rgn;_tmp301=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).nullable;_tmp302=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).bounds;_tmp303=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).zero_term;_tmp304=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FD)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp2FE;struct Cyc_Absyn_Tqual tq=_tmp2FF;void*old_r=_tmp300;void*nb=_tmp301;void*b=_tmp302;void*zt=_tmp303;struct Cyc_Absyn_PtrLoc*pl=_tmp304;
# 1861
{void*_tmp305=Cyc_Tcutil_compress(old_r);void*_stmttmp52=_tmp305;void*_tmp306=_stmttmp52;struct Cyc_Core_Opt*_tmp308;void**_tmp307;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp306)->tag == 1U){_LL6: _tmp307=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp306)->f2;_tmp308=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp306)->f4;_LL7: {void**topt=_tmp307;struct Cyc_Core_Opt*ts=_tmp308;
# 1863
void*_tmp309=Cyc_Absyn_var_type(tv);void*new_r=_tmp309;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1868
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1872
e->topt=0;
vd->initializer=0;{
# 1876
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1878
return({struct _tuple22 _tmp611;_tmp611.f1=d,_tmp611.f2=ve;_tmp611;});}}
# 1883
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1886
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1891
void*_tmp310=Cyc_Tcutil_compress(wants_type);void*_stmttmp53=_tmp310;void*_tmp311=_stmttmp53;void*_tmp312;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp311)->tag == 3U){_LL1: _tmp312=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp311)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp312;
# 1893
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp313=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp313;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
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
({struct Cyc_String_pa_PrintArg_struct _tmp316=({struct Cyc_String_pa_PrintArg_struct _tmp614;_tmp614.tag=0U,({
struct _fat_ptr _tmp6FB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp614.f1=_tmp6FB;});_tmp614;});struct Cyc_String_pa_PrintArg_struct _tmp317=({struct Cyc_String_pa_PrintArg_struct _tmp613;_tmp613.tag=0U,({struct _fat_ptr _tmp6FC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp613.f1=_tmp6FC;});_tmp613;});void*_tmp314[2U];_tmp314[0]=& _tmp316,_tmp314[1]=& _tmp317;({unsigned _tmp6FE=e->loc;struct _fat_ptr _tmp6FD=({const char*_tmp315="integral size mismatch; %s -> %s conversion supplied";_tag_fat(_tmp315,sizeof(char),53U);});Cyc_Tcutil_warn(_tmp6FE,_tmp6FD,_tag_fat(_tmp314,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1922
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp31A=({struct Cyc_String_pa_PrintArg_struct _tmp617;_tmp617.tag=0U,({
struct _fat_ptr _tmp6FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp617.f1=_tmp6FF;});_tmp617;});struct Cyc_String_pa_PrintArg_struct _tmp31B=({struct Cyc_String_pa_PrintArg_struct _tmp616;_tmp616.tag=0U,({struct _fat_ptr _tmp700=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp616.f1=_tmp700;});_tmp616;});struct Cyc_String_pa_PrintArg_struct _tmp31C=({struct Cyc_String_pa_PrintArg_struct _tmp615;_tmp615.tag=0U,({struct _fat_ptr _tmp701=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp615.f1=_tmp701;});_tmp615;});void*_tmp318[3U];_tmp318[0]=& _tmp31A,_tmp318[1]=& _tmp31B,_tmp318[2]=& _tmp31C;({unsigned _tmp703=e->loc;struct _fat_ptr _tmp702=({const char*_tmp319="implicit alias coercion for %s:%s to %s";_tag_fat(_tmp319,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp703,_tmp702,_tag_fat(_tmp318,sizeof(void*),3U));});});
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
({struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp619;_tmp619.tag=0U,({struct _fat_ptr _tmp704=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp619.f1=_tmp704;});_tmp619;});struct Cyc_String_pa_PrintArg_struct _tmp320=({struct Cyc_String_pa_PrintArg_struct _tmp618;_tmp618.tag=0U,({struct _fat_ptr _tmp705=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp618.f1=_tmp705;});_tmp618;});void*_tmp31D[2U];_tmp31D[0]=& _tmp31F,_tmp31D[1]=& _tmp320;({unsigned _tmp707=e->loc;struct _fat_ptr _tmp706=({const char*_tmp31E="implicit cast from %s to %s";_tag_fat(_tmp31E,sizeof(char),28U);});Cyc_Tcutil_warn(_tmp707,_tmp706,_tag_fat(_tmp31D,sizeof(void*),2U));});});
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
struct _tuple23 _tmp321=*env;struct _tuple23 _stmttmp54=_tmp321;struct _tuple23 _tmp322=_stmttmp54;int _tmp325;struct _RegionHandle*_tmp324;struct Cyc_List_List*_tmp323;_LL1: _tmp323=_tmp322.f1;_tmp324=_tmp322.f2;_tmp325=_tmp322.f3;_LL2: {struct Cyc_List_List*inst=_tmp323;struct _RegionHandle*r=_tmp324;int flatten=_tmp325;
void*_tmp326=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp326;
struct Cyc_List_List*_tmp327=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp327;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp329=_region_malloc(r,sizeof(*_tmp329));({struct _tuple11*_tmp708=({struct _tuple11*_tmp328=_region_malloc(r,sizeof(*_tmp328));_tmp328->f1=x->tq,_tmp328->f2=t;_tmp328;});_tmp329->hd=_tmp708;}),_tmp329->tl=0;_tmp329;});
return ts;}}struct _tuple24{struct _RegionHandle*f1;int f2;};
# 1979
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple24*env,struct _tuple11*x){
# 1981
struct _tuple24 _tmp32A=*env;struct _tuple24 _stmttmp55=_tmp32A;struct _tuple24 _tmp32B=_stmttmp55;int _tmp32D;struct _RegionHandle*_tmp32C;_LL1: _tmp32C=_tmp32B.f1;_tmp32D=_tmp32B.f2;_LL2: {struct _RegionHandle*r=_tmp32C;int flatten=_tmp32D;
struct _tuple11 _tmp32E=*x;struct _tuple11 _stmttmp56=_tmp32E;struct _tuple11 _tmp32F=_stmttmp56;void*_tmp331;struct Cyc_Absyn_Tqual _tmp330;_LL4: _tmp330=_tmp32F.f1;_tmp331=_tmp32F.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp330;void*t=_tmp331;
struct Cyc_List_List*_tmp332=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp332;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp334=_region_malloc(r,sizeof(*_tmp334));({struct _tuple11*_tmp709=({struct _tuple11*_tmp333=_region_malloc(r,sizeof(*_tmp333));_tmp333->f1=tq,_tmp333->f2=t;_tmp333;});_tmp334->hd=_tmp709;}),_tmp334->tl=0;_tmp334;});
return ts;}}}
# 1988
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1991
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp335=t1;struct Cyc_List_List*_tmp336;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp339;struct Cyc_Absyn_Aggrdecl*_tmp338;switch(*((int*)_tmp335)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp335)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp335)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp338=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp335)->f1)->f1).KnownAggr).val;_tmp339=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp335)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp338;struct Cyc_List_List*ts=_tmp339;
# 2009
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 2011
return({struct Cyc_List_List*_tmp344=_region_malloc(r,sizeof(*_tmp344));({struct _tuple11*_tmp70B=({struct _tuple11*_tmp343=_region_malloc(r,sizeof(*_tmp343));({struct Cyc_Absyn_Tqual _tmp70A=Cyc_Absyn_empty_tqual(0U);_tmp343->f1=_tmp70A;}),_tmp343->f2=t1;_tmp343;});_tmp344->hd=_tmp70B;}),_tmp344->tl=0;_tmp344;});{
struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp345;
struct _tuple23 env=({struct _tuple23 _tmp61A;_tmp61A.f1=inst,_tmp61A.f2=r,_tmp61A.f3=flatten;_tmp61A;});
struct Cyc_List_List*_tmp346=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp57=_tmp346;struct Cyc_List_List*_tmp347=_stmttmp57;struct Cyc_List_List*_tmp349;struct Cyc_Absyn_Aggrfield*_tmp348;if(_tmp347 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp348=(struct Cyc_Absyn_Aggrfield*)_tmp347->hd;_tmp349=_tmp347->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp348;struct Cyc_List_List*tl=_tmp349;
# 2017
struct Cyc_List_List*_tmp34A=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp34A;
env.f3=0;{
struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp34B;
struct Cyc_List_List*_tmp34C=({struct Cyc_List_List*_tmp34D=_region_malloc(r,sizeof(*_tmp34D));_tmp34D->hd=hd2,_tmp34D->tl=tl2;_tmp34D;});struct Cyc_List_List*tts=_tmp34C;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp337=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp335)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp337;
# 1996
struct _tuple24 _tmp33A=({struct _tuple24 _tmp61B;_tmp61B.f1=r,_tmp61B.f2=flatten;_tmp61B;});struct _tuple24 env=_tmp33A;
# 1998
struct Cyc_List_List*_tmp33B=tqs;struct Cyc_List_List*_tmp33D;struct _tuple11*_tmp33C;if(_tmp33B == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp33C=(struct _tuple11*)_tmp33B->hd;_tmp33D=_tmp33B->tl;_LLF: {struct _tuple11*hd=_tmp33C;struct Cyc_List_List*tl=_tmp33D;
# 2001
struct Cyc_List_List*_tmp33E=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp33E;
env.f2=0;{
struct Cyc_List_List*_tmp33F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct _tuple11*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp33F;
struct Cyc_List_List*_tmp340=({struct Cyc_List_List*_tmp342=_region_malloc(r,sizeof(*_tmp342));_tmp342->hd=hd2,_tmp342->tl=tl2;_tmp342;});struct Cyc_List_List*temp=_tmp340;
return({struct _RegionHandle*_tmp70C=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp70C,({struct Cyc_List_List*_tmp341=_region_malloc(r,sizeof(*_tmp341));_tmp341->hd=hd2,_tmp341->tl=tl2;_tmp341;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp335)->f1 == Cyc_Absyn_StructA){_LL7: _tmp336=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp335)->f2;_LL8: {struct Cyc_List_List*fs=_tmp336;
# 2024
struct _tuple23 env=({struct _tuple23 _tmp61C;_tmp61C.f1=0,_tmp61C.f2=r,_tmp61C.f3=flatten;_tmp61C;});
struct Cyc_List_List*_tmp34E=fs;struct Cyc_List_List*_tmp350;struct Cyc_Absyn_Aggrfield*_tmp34F;if(_tmp34E == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp34F=(struct Cyc_Absyn_Aggrfield*)_tmp34E->hd;_tmp350=_tmp34E->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp34F;struct Cyc_List_List*tl=_tmp350;
# 2028
struct Cyc_List_List*_tmp351=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp351;
env.f3=0;{
struct Cyc_List_List*_tmp352=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple23*,struct Cyc_Absyn_Aggrfield*),struct _tuple23*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp352;
struct Cyc_List_List*_tmp353=({struct Cyc_List_List*_tmp354=_region_malloc(r,sizeof(*_tmp354));_tmp354->hd=hd2,_tmp354->tl=tl2;_tmp354;});struct Cyc_List_List*tts=_tmp353;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2034
 goto _LL0;}_LL0:;}}
# 2037
return({struct Cyc_List_List*_tmp356=_region_malloc(r,sizeof(*_tmp356));({struct _tuple11*_tmp70E=({struct _tuple11*_tmp355=_region_malloc(r,sizeof(*_tmp355));({struct Cyc_Absyn_Tqual _tmp70D=Cyc_Absyn_empty_tqual(0U);_tmp355->f1=_tmp70D;}),_tmp355->f2=t1;_tmp355;});_tmp356->hd=_tmp70E;}),_tmp356->tl=0;_tmp356;});}
# 2041
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp357=(void*)t->hd;void*_stmttmp58=_tmp357;void*_tmp358=_stmttmp58;switch(*((int*)_tmp358)){case 23U: _LL1: _LL2:
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
struct _tuple14 _tmp359=({struct _tuple14 _tmp61D;({void*_tmp710=Cyc_Tcutil_compress(t1);_tmp61D.f1=_tmp710;}),({void*_tmp70F=Cyc_Tcutil_compress(t2);_tmp61D.f2=_tmp70F;});_tmp61D;});struct _tuple14 _stmttmp59=_tmp359;struct _tuple14 _tmp35A=_stmttmp59;enum Cyc_Absyn_Size_of _tmp35C;enum Cyc_Absyn_Size_of _tmp35B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->f1)->tag == 1U){_LL1: _tmp35B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f1)->f1)->f2;_tmp35C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35A.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp35B;enum Cyc_Absyn_Size_of b2=_tmp35C;
# 2060
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
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
struct _tuple14 _tmp35D=({struct _tuple14 _tmp61E;_tmp61E.f1=t1,_tmp61E.f2=t2;_tmp61E;});struct _tuple14 _stmttmp5A=_tmp35D;struct _tuple14 _tmp35E=_stmttmp5A;struct Cyc_Absyn_FnInfo _tmp360;struct Cyc_Absyn_FnInfo _tmp35F;struct Cyc_List_List*_tmp365;struct Cyc_Absyn_Datatypedecl*_tmp364;struct Cyc_List_List*_tmp363;struct Cyc_Absyn_Datatypefield*_tmp362;struct Cyc_Absyn_Datatypedecl*_tmp361;void*_tmp371;void*_tmp370;void*_tmp36F;void*_tmp36E;struct Cyc_Absyn_Tqual _tmp36D;void*_tmp36C;void*_tmp36B;void*_tmp36A;void*_tmp369;void*_tmp368;struct Cyc_Absyn_Tqual _tmp367;void*_tmp366;switch(*((int*)_tmp35E.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->tag == 3U){_LL1: _tmp366=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).elt_type;_tmp367=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).elt_tq;_tmp368=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).ptr_atts).rgn;_tmp369=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).ptr_atts).nullable;_tmp36A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).ptr_atts).bounds;_tmp36B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f1)->f1).ptr_atts).zero_term;_tmp36C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).elt_type;_tmp36D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).elt_tq;_tmp36E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).ptr_atts).rgn;_tmp36F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).ptr_atts).nullable;_tmp370=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).ptr_atts).bounds;_tmp371=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp35E.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp366;struct Cyc_Absyn_Tqual q_a=_tmp367;void*rt_a=_tmp368;void*null_a=_tmp369;void*b_a=_tmp36A;void*zt_a=_tmp36B;void*t_b=_tmp36C;struct Cyc_Absyn_Tqual q_b=_tmp36D;void*rt_b=_tmp36E;void*null_b=_tmp36F;void*b_b=_tmp370;void*zt_b=_tmp371;
# 2082
if(q_a.real_const && !q_b.real_const)
return 0;
# 2085
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 2089
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 2093
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 2097
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp372=({void*_tmp711=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp711,b_a);});struct Cyc_Absyn_Exp*e1=_tmp372;
struct Cyc_Absyn_Exp*_tmp373=({void*_tmp712=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp712,b_b);});struct Cyc_Absyn_Exp*e2=_tmp373;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 2107
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 2113
int _tmp374=
({void*_tmp713=b_b;Cyc_Unify_unify(_tmp713,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2113
int deep_subtype=_tmp374;
# 2118
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp717=po;struct Cyc_List_List*_tmp716=({struct Cyc_List_List*_tmp376=_cycalloc(sizeof(*_tmp376));({struct _tuple14*_tmp714=({struct _tuple14*_tmp375=_cycalloc(sizeof(*_tmp375));_tmp375->f1=t1,_tmp375->f2=t2;_tmp375;});_tmp376->hd=_tmp714;}),_tmp376->tl=assume;_tmp376;});void*_tmp715=t_a;Cyc_Tcutil_ptrsubtype(_tmp717,_tmp716,_tmp715,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp361=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp362=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp363=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f2;_tmp364=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->f1).KnownDatatype).val;_tmp365=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp361;struct Cyc_Absyn_Datatypefield*df=_tmp362;struct Cyc_List_List*ts1=_tmp363;struct Cyc_Absyn_Datatypedecl*dd2=_tmp364;struct Cyc_List_List*ts2=_tmp365;
# 2125
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 2127
if(({int _tmp718=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp718 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35E.f2)->tag == 5U){_LL5: _tmp35F=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35E.f1)->f1;_tmp360=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp35E.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp35F;struct Cyc_Absyn_FnInfo f2=_tmp360;
# 2134
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp377=f1.tvars;struct Cyc_List_List*tvs1=_tmp377;
struct Cyc_List_List*_tmp378=f2.tvars;struct Cyc_List_List*tvs2=_tmp378;
if(({int _tmp719=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp719 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp37A=_cycalloc(sizeof(*_tmp37A));({struct _tuple18*_tmp71B=({struct _tuple18*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp71A=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp379->f2=_tmp71A;});_tmp379;});_tmp37A->hd=_tmp71B;}),_tmp37A->tl=inst;_tmp37A;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2145
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp71E=po;struct Cyc_List_List*_tmp71D=assume;void*_tmp71C=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp37B=_cycalloc(sizeof(*_tmp37B));_tmp37B->tag=5U,_tmp37B->f1=f1;_tmp37B;});Cyc_Tcutil_subtype(_tmp71E,_tmp71D,_tmp71C,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp37C=_cycalloc(sizeof(*_tmp37C));_tmp37C->tag=5U,_tmp37C->f1=f2;_tmp37C;}));});}}}
# 2152
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp37D=f1.args;struct Cyc_List_List*args1=_tmp37D;
struct Cyc_List_List*_tmp37E=f2.args;struct Cyc_List_List*args2=_tmp37E;
# 2157
if(({int _tmp71F=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp71F != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2159
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp37F=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp5B=_tmp37F;struct _tuple9 _tmp380=_stmttmp5B;void*_tmp382;struct Cyc_Absyn_Tqual _tmp381;_LLA: _tmp381=_tmp380.f2;_tmp382=_tmp380.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp381;void*t1=_tmp382;
struct _tuple9 _tmp383=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp5C=_tmp383;struct _tuple9 _tmp384=_stmttmp5C;void*_tmp386;struct Cyc_Absyn_Tqual _tmp385;_LLD: _tmp385=_tmp384.f2;_tmp386=_tmp384.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp385;void*t2=_tmp386;
# 2163
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2167
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp387=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp387;
struct Cyc_Absyn_VarargInfo _tmp388=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp388;
# 2172
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2177
if(!({void*_tmp720=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp720,(void*)_check_null(f2.effect));}))return 0;
# 2179
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2181
if(!Cyc_Tcutil_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2183
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2185
return 0;
# 2187
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2189
return 0;
# 2191
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2202
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2206
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple11*_tmp389=(struct _tuple11*)tqs1->hd;struct _tuple11*_stmttmp5D=_tmp389;struct _tuple11*_tmp38A=_stmttmp5D;void*_tmp38C;struct Cyc_Absyn_Tqual _tmp38B;_LL1: _tmp38B=_tmp38A->f1;_tmp38C=_tmp38A->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp38B;void*t1a=_tmp38C;
struct _tuple11*_tmp38D=(struct _tuple11*)tqs2->hd;struct _tuple11*_stmttmp5E=_tmp38D;struct _tuple11*_tmp38E=_stmttmp5E;void*_tmp390;struct Cyc_Absyn_Tqual _tmp38F;_LL4: _tmp38F=_tmp38E->f1;_tmp390=_tmp38E->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp38F;void*t2a=_tmp390;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a))
continue;
if(Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
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
void*_tmp392=t1;void*_tmp393;struct Cyc_Absyn_Enumdecl*_tmp394;void*_tmp398;struct Cyc_Absyn_Exp*_tmp397;struct Cyc_Absyn_Tqual _tmp396;void*_tmp395;void*_tmp39E;void*_tmp39D;void*_tmp39C;void*_tmp39B;struct Cyc_Absyn_Tqual _tmp39A;void*_tmp399;switch(*((int*)_tmp392)){case 3U: _LLA: _tmp399=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).elt_type;_tmp39A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).elt_tq;_tmp39B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).rgn;_tmp39C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).nullable;_tmp39D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).bounds;_tmp39E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp392)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp399;struct Cyc_Absyn_Tqual q_a=_tmp39A;void*rt_a=_tmp39B;void*null_a=_tmp39C;void*b_a=_tmp39D;void*zt_a=_tmp39E;
# 2251
{void*_tmp39F=t2;void*_tmp3A5;void*_tmp3A4;void*_tmp3A3;void*_tmp3A2;struct Cyc_Absyn_Tqual _tmp3A1;void*_tmp3A0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->tag == 3U){_LL19: _tmp3A0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->f1).elt_type;_tmp3A1=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->f1).elt_tq;_tmp3A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->f1).ptr_atts).rgn;_tmp3A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->f1).ptr_atts).nullable;_tmp3A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->f1).ptr_atts).bounds;_tmp3A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39F)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3A0;struct Cyc_Absyn_Tqual q_b=_tmp3A1;void*rt_b=_tmp3A2;void*null_b=_tmp3A3;void*b_b=_tmp3A4;void*zt_b=_tmp3A5;
# 2253
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3A6=({struct Cyc_List_List*_tmp3AE=_cycalloc(sizeof(*_tmp3AE));({struct _tuple14*_tmp721=({struct _tuple14*_tmp3AD=_cycalloc(sizeof(*_tmp3AD));_tmp3AD->f1=t1,_tmp3AD->f2=t2;_tmp3AD;});_tmp3AE->hd=_tmp721;}),_tmp3AE->tl=0;_tmp3AE;});struct Cyc_List_List*assump=_tmp3A6;
int _tmp3A7=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3A7;
# 2266 "tcutil.cyc"
int _tmp3A8=
({void*_tmp722=b_b;Cyc_Unify_unify(_tmp722,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2266
int deep_castable=_tmp3A8;
# 2269
int _tmp3A9=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2269
int ptrsub=_tmp3A9;
# 2272
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2274
int _tmp3AA=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2274
int bitcase=_tmp3AA;
# 2277
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3AB=({void*_tmp723=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp723,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3AB;
struct Cyc_Absyn_Exp*_tmp3AC=({void*_tmp724=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp724,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3AC;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2286
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2291
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2297
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp395=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp392)->f1).elt_type;_tmp396=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp392)->f1).tq;_tmp397=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp392)->f1).num_elts;_tmp398=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp392)->f1).zero_term;_LLD: {void*t1a=_tmp395;struct Cyc_Absyn_Tqual tq1a=_tmp396;struct Cyc_Absyn_Exp*e1=_tmp397;void*zt1=_tmp398;
# 2299
{void*_tmp3AF=t2;void*_tmp3B3;struct Cyc_Absyn_Exp*_tmp3B2;struct Cyc_Absyn_Tqual _tmp3B1;void*_tmp3B0;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AF)->tag == 4U){_LL1E: _tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AF)->f1).elt_type;_tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AF)->f1).tq;_tmp3B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AF)->f1).num_elts;_tmp3B3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AF)->f1).zero_term;_LL1F: {void*t2a=_tmp3B0;struct Cyc_Absyn_Tqual tq2a=_tmp3B1;struct Cyc_Absyn_Exp*e2=_tmp3B2;void*zt2=_tmp3B3;
# 2301
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2306
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2308
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2310
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f1)){case 15U: _LLE: _tmp394=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp394;
# 2314
{void*_tmp3B4=t2;struct Cyc_Absyn_Enumdecl*_tmp3B5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->f1)->tag == 15U){_LL23: _tmp3B5=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B4)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3B5;
# 2316
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp725=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp725 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2322
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2325
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f2 != 0){_LL14: _tmp393=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp392)->f2)->hd;_LL15: {void*r1=_tmp393;
# 2328
{void*_tmp3B6=t2;void*_tmp3B7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->f2 != 0){_LL28: _tmp3B7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B6)->f2)->hd;_LL29: {void*r2=_tmp3B7;
# 2330
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2334
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2340
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3B8=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3B8;
({void*_tmp726=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->tag=14U,_tmp3B9->f1=t,_tmp3B9->f2=inner,_tmp3B9->f3=0,_tmp3B9->f4=c;_tmp3B9;});e->r=_tmp726;});
e->topt=t;}}
# 2349
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2354
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3BF=({struct Cyc_Int_pa_PrintArg_struct _tmp61F;_tmp61F.tag=1U,_tmp61F.f1=(unsigned long)i;_tmp61F;});void*_tmp3BD[1U];_tmp3BD[0]=& _tmp3BF;({struct _fat_ptr _tmp727=({const char*_tmp3BE="#%d";_tag_fat(_tmp3BE,sizeof(char),4U);});Cyc_aprintf(_tmp727,_tag_fat(_tmp3BD,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp3BC=_cycalloc(sizeof(*_tmp3BC));({struct _fat_ptr*_tmp728=({unsigned _tmp3BB=1;struct _fat_ptr*_tmp3BA=_cycalloc(_check_times(_tmp3BB,sizeof(struct _fat_ptr)));_tmp3BA[0]=s;_tmp3BA;});_tmp3BC->name=_tmp728;}),_tmp3BC->identity=- 1,_tmp3BC->kind=k;_tmp3BC;});}
# 2361
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp3C0=*t->name;struct _fat_ptr s=_tmp3C0;
return(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'#';}
# 2367
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2374
struct Cyc_List_List*_tmp3C1=0;struct Cyc_List_List*fn_type_atts=_tmp3C1;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3C2=_cycalloc(sizeof(*_tmp3C2));_tmp3C2->hd=(void*)atts->hd,_tmp3C2->tl=fn_type_atts;_tmp3C2;});}}{
struct Cyc_Absyn_FnInfo _tmp3C3=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3C3;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3C4=_cycalloc(sizeof(*_tmp3C4));_tmp3C4->tag=5U,_tmp3C4->f1=type_info;_tmp3C4;});}}
# 2382
return(void*)_check_null(fd->cached_type);}
# 2388
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2390
union Cyc_Relations_RelnOp _tmp3C5=*rop;union Cyc_Relations_RelnOp _stmttmp5F=_tmp3C5;union Cyc_Relations_RelnOp _tmp3C6=_stmttmp5F;struct Cyc_Absyn_Vardecl*_tmp3C7;struct Cyc_Absyn_Vardecl*_tmp3C8;switch((_tmp3C6.RNumelts).tag){case 2U: _LL1: _tmp3C8=(_tmp3C6.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3C8;
# 2392
struct _tuple1 _tmp3C9=*vd->name;struct _tuple1 _stmttmp60=_tmp3C9;struct _tuple1 _tmp3CA=_stmttmp60;struct _fat_ptr*_tmp3CC;union Cyc_Absyn_Nmspace _tmp3CB;_LL8: _tmp3CB=_tmp3CA.f1;_tmp3CC=_tmp3CA.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3CB;struct _fat_ptr*var=_tmp3CC;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp729=(struct _fat_ptr)*var;Cyc_strcmp(_tmp729,({const char*_tmp3CD="return_value";_tag_fat(_tmp3CD,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp72A=Cyc_Relations_RReturn();*rop=_tmp72A;});
goto _LL0;}{
# 2398
unsigned c=0U;
{struct Cyc_List_List*_tmp3CE=args;struct Cyc_List_List*a=_tmp3CE;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3CF=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp61=_tmp3CF;struct _tuple9*_tmp3D0=_stmttmp61;struct _fat_ptr*_tmp3D1;_LLB: _tmp3D1=_tmp3D0->f1;_LLC: {struct _fat_ptr*vopt=_tmp3D1;
if(vopt != 0){
if(Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp72B=Cyc_Relations_RParam(c);*rop=_tmp72B;});
break;}}}}}
# 2408
goto _LL0;}}}case 3U: _LL3: _tmp3C7=(_tmp3C6.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3C7;
# 2410
struct _tuple1 _tmp3D2=*vd->name;struct _tuple1 _stmttmp62=_tmp3D2;struct _tuple1 _tmp3D3=_stmttmp62;struct _fat_ptr*_tmp3D5;union Cyc_Absyn_Nmspace _tmp3D4;_LLE: _tmp3D4=_tmp3D3.f1;_tmp3D5=_tmp3D3.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3D4;struct _fat_ptr*var=_tmp3D5;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3D6=args;struct Cyc_List_List*a=_tmp3D6;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3D7=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp63=_tmp3D7;struct _tuple9*_tmp3D8=_stmttmp63;struct _fat_ptr*_tmp3D9;_LL11: _tmp3D9=_tmp3D8->f1;_LL12: {struct _fat_ptr*vopt=_tmp3D9;
if(vopt != 0){
if(Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp72C=Cyc_Relations_RParamNumelts(c);*rop=_tmp72C;});
break;}}}}}
# 2422
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2427
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2429
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2433
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2435
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3DA=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3DA;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2442
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){return(*t).f2;}
static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){
struct _tuple11*_tmp3DB=pr;void*_tmp3DD;struct Cyc_Absyn_Tqual _tmp3DC;_LL1: _tmp3DC=_tmp3DB->f1;_tmp3DD=_tmp3DB->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3DC;void*t2=_tmp3DD;
if(t2 == t)return pr;else{
return({struct _tuple11*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->f1=tq,_tmp3DE->f2=t;_tmp3DE;});}}}struct _tuple25{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple26{struct _tuple25*f1;void*f2;};
# 2448
static struct _tuple26*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2450
return({struct _tuple26*_tmp3E0=_region_malloc(rgn,sizeof(*_tmp3E0));({struct _tuple25*_tmp72D=({struct _tuple25*_tmp3DF=_region_malloc(rgn,sizeof(*_tmp3DF));_tmp3DF->f1=(*y).f1,_tmp3DF->f2=(*y).f2;_tmp3DF;});_tmp3E0->f1=_tmp72D;}),_tmp3E0->f2=(*y).f3;_tmp3E0;});}
# 2452
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2454
struct _tuple9 _tmp3E1=*orig_arg;struct _tuple9 _stmttmp64=_tmp3E1;struct _tuple9 _tmp3E2=_stmttmp64;void*_tmp3E5;struct Cyc_Absyn_Tqual _tmp3E4;struct _fat_ptr*_tmp3E3;_LL1: _tmp3E3=_tmp3E2.f1;_tmp3E4=_tmp3E2.f2;_tmp3E5=_tmp3E2.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp3E3;struct Cyc_Absyn_Tqual tq_orig=_tmp3E4;void*t_orig=_tmp3E5;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp3E6=_cycalloc(sizeof(*_tmp3E6));_tmp3E6->f1=vopt_orig,_tmp3E6->f2=tq_orig,_tmp3E6->f3=t;_tmp3E6;});}}
# 2458
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2463
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2465
return({struct Cyc_Absyn_Exp*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->topt=old->topt,_tmp3E7->r=r,_tmp3E7->loc=old->loc,_tmp3E7->annot=old->annot;_tmp3E7;});}
# 2470
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3E8=e->r;void*_stmttmp65=_tmp3E8;void*_tmp3E9=_stmttmp65;void*_tmp3EA;struct Cyc_List_List*_tmp3EC;void*_tmp3EB;struct Cyc_Absyn_Exp*_tmp3ED;struct Cyc_Absyn_Exp*_tmp3EE;void*_tmp3EF;enum Cyc_Absyn_Coercion _tmp3F3;int _tmp3F2;struct Cyc_Absyn_Exp*_tmp3F1;void*_tmp3F0;struct Cyc_Absyn_Exp*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F4;struct Cyc_Absyn_Exp*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F6;struct Cyc_Absyn_Exp*_tmp3F9;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_Absyn_Exp*_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FA;struct Cyc_List_List*_tmp3FE;enum Cyc_Absyn_Primop _tmp3FD;switch(*((int*)_tmp3E9)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp3FD=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3FE=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp3FD;struct Cyc_List_List*es=_tmp3FE;
# 2479
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp3FF=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp3FF;
struct Cyc_Absyn_Exp*_tmp400=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp400;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp72F=e;Cyc_Tcutil_copye(_tmp72F,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp402=_cycalloc(sizeof(*_tmp402));_tmp402->tag=3U,_tmp402->f1=p,({struct Cyc_List_List*_tmp72E=({struct Cyc_Absyn_Exp*_tmp401[1U];_tmp401[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp401,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp402->f2=_tmp72E;});_tmp402;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp403=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp403;
struct Cyc_Absyn_Exp*_tmp404=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp404;
struct Cyc_Absyn_Exp*_tmp405=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp405;
struct Cyc_Absyn_Exp*_tmp406=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp406;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp731=e;Cyc_Tcutil_copye(_tmp731,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->tag=3U,_tmp408->f1=p,({struct Cyc_List_List*_tmp730=({struct Cyc_Absyn_Exp*_tmp407[2U];_tmp407[0]=new_e1,_tmp407[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp407,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp408->f2=_tmp730;});_tmp408;}));});}else{
return({void*_tmp409=0U;({struct _fat_ptr _tmp732=({const char*_tmp40A="primop does not have 1 or 2 args!";_tag_fat(_tmp40A,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp732,_tag_fat(_tmp409,sizeof(void*),0U));});});}}}case 6U: _LLD: _tmp3FA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3FB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_tmp3FC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3E9)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp3FA;struct Cyc_Absyn_Exp*e2=_tmp3FB;struct Cyc_Absyn_Exp*e3=_tmp3FC;
# 2493
struct Cyc_Absyn_Exp*_tmp40B=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40B;
struct Cyc_Absyn_Exp*_tmp40C=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp40C;
struct Cyc_Absyn_Exp*_tmp40D=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp40D;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp733=e;Cyc_Tcutil_copye(_tmp733,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->tag=6U,_tmp40E->f1=new_e1,_tmp40E->f2=new_e2,_tmp40E->f3=new_e3;_tmp40E;}));});}case 7U: _LLF: _tmp3F8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3F9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp3F8;struct Cyc_Absyn_Exp*e2=_tmp3F9;
# 2499
struct Cyc_Absyn_Exp*_tmp40F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40F;
struct Cyc_Absyn_Exp*_tmp410=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp410;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp734=e;Cyc_Tcutil_copye(_tmp734,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->tag=7U,_tmp411->f1=new_e1,_tmp411->f2=new_e2;_tmp411;}));});}case 8U: _LL11: _tmp3F6=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3F7=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp3F6;struct Cyc_Absyn_Exp*e2=_tmp3F7;
# 2504
struct Cyc_Absyn_Exp*_tmp412=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp412;
struct Cyc_Absyn_Exp*_tmp413=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp413;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp735=e;Cyc_Tcutil_copye(_tmp735,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->tag=8U,_tmp414->f1=new_e1,_tmp414->f2=new_e2;_tmp414;}));});}case 9U: _LL13: _tmp3F4=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3F5=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp3F4;struct Cyc_Absyn_Exp*e2=_tmp3F5;
# 2509
struct Cyc_Absyn_Exp*_tmp415=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp415;
struct Cyc_Absyn_Exp*_tmp416=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp416;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp736=e;Cyc_Tcutil_copye(_tmp736,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp417=_cycalloc(sizeof(*_tmp417));_tmp417->tag=9U,_tmp417->f1=new_e1,_tmp417->f2=new_e2;_tmp417;}));});}case 14U: _LL15: _tmp3F0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3F1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_tmp3F2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E9)->f3;_tmp3F3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3E9)->f4;_LL16: {void*t=_tmp3F0;struct Cyc_Absyn_Exp*e1=_tmp3F1;int b=_tmp3F2;enum Cyc_Absyn_Coercion c=_tmp3F3;
# 2514
struct Cyc_Absyn_Exp*_tmp418=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp418;
void*_tmp419=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp419;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp737=e;Cyc_Tcutil_copye(_tmp737,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->tag=14U,_tmp41A->f1=new_typ,_tmp41A->f2=new_e1,_tmp41A->f3=b,_tmp41A->f4=c;_tmp41A;}));});}case 17U: _LL17: _tmp3EF=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_LL18: {void*t=_tmp3EF;
# 2519
void*_tmp41B=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp41B;
if(new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp738=e;Cyc_Tcutil_copye(_tmp738,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->tag=17U,_tmp41C->f1=new_typ;_tmp41C;}));});}case 18U: _LL19: _tmp3EE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp3EE;
# 2523
struct Cyc_Absyn_Exp*_tmp41D=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41D;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp739=e;Cyc_Tcutil_copye(_tmp739,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->tag=18U,_tmp41E->f1=new_e1;_tmp41E;}));});}case 41U: _LL1B: _tmp3ED=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp3ED;
# 2527
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41F;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp73A=e;Cyc_Tcutil_copye(_tmp73A,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->tag=41U,_tmp420->f1=new_e1;_tmp420;}));});}case 19U: _LL1D: _tmp3EB=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_tmp3EC=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3E9)->f2;_LL1E: {void*t=_tmp3EB;struct Cyc_List_List*f=_tmp3EC;
# 2531
void*_tmp421=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp421;
if(new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp73B=e;Cyc_Tcutil_copye(_tmp73B,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->tag=19U,_tmp422->f1=new_typ,_tmp422->f2=f;_tmp422;}));});}case 39U: _LL1F: _tmp3EA=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3E9)->f1;_LL20: {void*t=_tmp3EA;
# 2535
void*_tmp423=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp423;
if(new_typ == t)return e;{
# 2538
void*_tmp424=Cyc_Tcutil_compress(new_typ);void*_stmttmp66=_tmp424;void*_tmp425=_stmttmp66;struct Cyc_Absyn_Exp*_tmp426;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp425)->tag == 9U){_LL24: _tmp426=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp425)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp426;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp73C=e;Cyc_Tcutil_copye(_tmp73C,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp427=_cycalloc(sizeof(*_tmp427));_tmp427->tag=39U,_tmp427->f1=new_typ;_tmp427;}));});}_LL23:;}}default: _LL21: _LL22:
# 2542
 return({void*_tmp428=0U;({struct _fat_ptr _tmp73D=({const char*_tmp429="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp429,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp73D,_tag_fat(_tmp428,sizeof(void*),0U));});});}_LL0:;}
# 2546
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2549
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 2553
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2556
void*_tmp42A=f->type;void*t=_tmp42A;
struct Cyc_Absyn_Exp*_tmp42B=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp42B;
void*_tmp42C=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_t=_tmp42C;
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp42D;
if(t == new_t && req == new_req)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->name=f->name,_tmp42E->tq=f->tq,_tmp42E->type=new_t,_tmp42E->width=f->width,_tmp42E->attributes=f->attributes,_tmp42E->requires_clause=new_req;_tmp42E;});}}
# 2566
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2569
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp42F=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp42F;
struct Cyc_List_List*_tmp430=fs->tl;struct Cyc_List_List*t=_tmp430;
struct Cyc_Absyn_Aggrfield*_tmp431=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp431;
struct Cyc_List_List*_tmp432=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_t=_tmp432;
if(new_f == f && new_t == t)return fs;
return({struct Cyc_List_List*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->hd=new_f,_tmp433->tl=new_t;_tmp433;});}}
# 2578
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2581
struct _tuple0 _tmp434=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp67=_tmp434;struct _tuple0 _tmp435=_stmttmp67;struct Cyc_List_List*_tmp437;struct Cyc_List_List*_tmp436;_LL1: _tmp436=_tmp435.f1;_tmp437=_tmp435.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp436;struct Cyc_List_List*rpo1b=_tmp437;
struct Cyc_List_List*_tmp438=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp438;
struct Cyc_List_List*_tmp439=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp439;
if(rpo2a == rpo1a && rpo2b == rpo1b)
return rgn_po;else{
# 2587
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}}
# 2590
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2593
void*_tmp43A=Cyc_Tcutil_compress(t);void*_stmttmp68=_tmp43A;void*_tmp43B=_stmttmp68;struct Cyc_Absyn_Exp*_tmp43C;struct Cyc_Absyn_Exp*_tmp43D;struct Cyc_List_List*_tmp43F;void*_tmp43E;void*_tmp440;void*_tmp441;struct Cyc_List_List*_tmp443;enum Cyc_Absyn_AggrKind _tmp442;struct Cyc_List_List*_tmp444;struct Cyc_Absyn_Exp*_tmp44F;struct Cyc_Absyn_Exp*_tmp44E;struct Cyc_List_List*_tmp44D;struct Cyc_List_List*_tmp44C;struct Cyc_Absyn_VarargInfo*_tmp44B;int _tmp44A;struct Cyc_List_List*_tmp449;void*_tmp448;struct Cyc_Absyn_Tqual _tmp447;void*_tmp446;struct Cyc_List_List*_tmp445;void*_tmp455;void*_tmp454;void*_tmp453;void*_tmp452;struct Cyc_Absyn_Tqual _tmp451;void*_tmp450;unsigned _tmp45A;void*_tmp459;struct Cyc_Absyn_Exp*_tmp458;struct Cyc_Absyn_Tqual _tmp457;void*_tmp456;void*_tmp45E;struct Cyc_Absyn_Typedefdecl*_tmp45D;struct Cyc_List_List*_tmp45C;struct _tuple1*_tmp45B;struct Cyc_Absyn_Tvar*_tmp45F;switch(*((int*)_tmp43B)){case 2U: _LL1: _tmp45F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp43B)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp45F;
# 2596
struct _handler_cons _tmp460;_push_handler(& _tmp460);{int _tmp462=0;if(setjmp(_tmp460.handler))_tmp462=1;if(!_tmp462){{void*_tmp463=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp463;};_pop_handler();}else{void*_tmp461=(void*)Cyc_Core_get_exn_thrown();void*_tmp464=_tmp461;void*_tmp465;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp464)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp465=_tmp464;_LL1F: {void*exn=_tmp465;(int)_rethrow(exn);}}_LL1B:;}}}case 8U: _LL3: _tmp45B=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43B)->f1;_tmp45C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43B)->f2;_tmp45D=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43B)->f3;_tmp45E=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43B)->f4;_LL4: {struct _tuple1*n=_tmp45B;struct Cyc_List_List*ts=_tmp45C;struct Cyc_Absyn_Typedefdecl*td=_tmp45D;void*topt=_tmp45E;
# 2599
struct Cyc_List_List*_tmp466=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp466;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp467=_cycalloc(sizeof(*_tmp467));_tmp467->tag=8U,_tmp467->f1=n,_tmp467->f2=new_ts,_tmp467->f3=td,_tmp467->f4=topt;_tmp467;});}case 4U: _LL5: _tmp456=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43B)->f1).elt_type;_tmp457=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43B)->f1).tq;_tmp458=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43B)->f1).num_elts;_tmp459=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43B)->f1).zero_term;_tmp45A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43B)->f1).zt_loc;_LL6: {void*t1=_tmp456;struct Cyc_Absyn_Tqual tq=_tmp457;struct Cyc_Absyn_Exp*e=_tmp458;void*zt=_tmp459;unsigned ztl=_tmp45A;
# 2602
void*_tmp468=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp468;
struct Cyc_Absyn_Exp*_tmp469=e == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp469;
void*_tmp46A=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp46A;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46B=_cycalloc(sizeof(*_tmp46B));
_tmp46B->tag=4U,(_tmp46B->f1).elt_type=new_t1,(_tmp46B->f1).tq=tq,(_tmp46B->f1).num_elts=new_e,(_tmp46B->f1).zero_term=new_zt,(_tmp46B->f1).zt_loc=ztl;_tmp46B;});}case 3U: _LL7: _tmp450=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43B)->f1).elt_type;_tmp451=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43B)->f1).elt_tq;_tmp452=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43B)->f1).ptr_atts).rgn;_tmp453=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43B)->f1).ptr_atts).nullable;_tmp454=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43B)->f1).ptr_atts).bounds;_tmp455=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43B)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp450;struct Cyc_Absyn_Tqual tq=_tmp451;void*r=_tmp452;void*n=_tmp453;void*b=_tmp454;void*zt=_tmp455;
# 2608
void*_tmp46C=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp46C;
void*_tmp46D=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp46D;
void*_tmp46E=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp46E;
void*_tmp46F=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp46F;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp620;_tmp620.elt_type=new_t1,_tmp620.elt_tq=tq,(_tmp620.ptr_atts).rgn=new_r,(_tmp620.ptr_atts).nullable=n,(_tmp620.ptr_atts).bounds=new_b,(_tmp620.ptr_atts).zero_term=new_zt,(_tmp620.ptr_atts).ptrloc=0;_tmp620;}));}case 5U: _LL9: _tmp445=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).tvars;_tmp446=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).effect;_tmp447=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).ret_tqual;_tmp448=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).ret_type;_tmp449=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).args;_tmp44A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).c_varargs;_tmp44B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).cyc_varargs;_tmp44C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).rgn_po;_tmp44D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).attributes;_tmp44E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).requires_clause;_tmp44F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43B)->f1).ensures_clause;_LLA: {struct Cyc_List_List*vs=_tmp445;void*eff=_tmp446;struct Cyc_Absyn_Tqual rtq=_tmp447;void*rtyp=_tmp448;struct Cyc_List_List*args=_tmp449;int c_varargs=_tmp44A;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp44B;struct Cyc_List_List*rgn_po=_tmp44C;struct Cyc_List_List*atts=_tmp44D;struct Cyc_Absyn_Exp*req=_tmp44E;struct Cyc_Absyn_Exp*ens=_tmp44F;
# 2618
{struct Cyc_List_List*_tmp470=vs;struct Cyc_List_List*p=_tmp470;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp472=_region_malloc(rgn,sizeof(*_tmp472));({struct _tuple18*_tmp73F=({struct _tuple18*_tmp471=_region_malloc(rgn,sizeof(*_tmp471));_tmp471->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp73E=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp471->f2=_tmp73E;});_tmp471;});_tmp472->hd=_tmp73F;}),_tmp472->tl=inst;_tmp472;});}}{
struct _tuple0 _tmp473=({struct _RegionHandle*_tmp741=rgn;struct _RegionHandle*_tmp740=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp741,_tmp740,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple26*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2620
struct _tuple0 _stmttmp69=_tmp473;struct _tuple0 _tmp474=_stmttmp69;struct Cyc_List_List*_tmp476;struct Cyc_List_List*_tmp475;_LL21: _tmp475=_tmp474.f1;_tmp476=_tmp474.f2;_LL22: {struct Cyc_List_List*qs=_tmp475;struct Cyc_List_List*ts=_tmp476;
# 2622
struct Cyc_List_List*_tmp477=args;struct Cyc_List_List*args2=_tmp477;
struct Cyc_List_List*_tmp478=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp478;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2;
if(eff == 0)
eff2=0;else{
# 2630
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,eff);
if(new_eff == eff)
eff2=eff;else{
# 2634
eff2=new_eff;}}{
# 2636
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2640
struct Cyc_Absyn_VarargInfo _tmp479=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp6A=_tmp479;struct Cyc_Absyn_VarargInfo _tmp47A=_stmttmp6A;int _tmp47E;void*_tmp47D;struct Cyc_Absyn_Tqual _tmp47C;struct _fat_ptr*_tmp47B;_LL24: _tmp47B=_tmp47A.name;_tmp47C=_tmp47A.tq;_tmp47D=_tmp47A.type;_tmp47E=_tmp47A.inject;_LL25: {struct _fat_ptr*n=_tmp47B;struct Cyc_Absyn_Tqual tq=_tmp47C;void*t=_tmp47D;int i=_tmp47E;
void*_tmp47F=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp47F;
if(t2 == t)cyc_varargs2=cyc_varargs;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp480=_cycalloc(sizeof(*_tmp480));_tmp480->name=n,_tmp480->tq=tq,_tmp480->type=t2,_tmp480->inject=i;_tmp480;});}}}{
# 2645
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp481=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp481;
struct Cyc_List_List*_tmp482=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp482;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->tag=5U,(_tmp483->f1).tvars=vs,(_tmp483->f1).effect=eff2,(_tmp483->f1).ret_tqual=rtq,({void*_tmp742=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp483->f1).ret_type=_tmp742;}),(_tmp483->f1).args=args2,(_tmp483->f1).c_varargs=c_varargs,(_tmp483->f1).cyc_varargs=cyc_varargs2,(_tmp483->f1).rgn_po=rgn_po2,(_tmp483->f1).attributes=atts,(_tmp483->f1).requires_clause=req2,(_tmp483->f1).requires_relns=req_relns2,(_tmp483->f1).ensures_clause=ens2,(_tmp483->f1).ensures_relns=ens_relns2;_tmp483;});}}}}}}case 6U: _LLB: _tmp444=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp43B)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp444;
# 2654
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp484=tqts;struct Cyc_List_List*ts1=_tmp484;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp485=(*((struct _tuple11*)ts1->hd)).f2;void*t1=_tmp485;
void*_tmp486=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp486;
if(t1 != t2)
change=1;
# 2662
ts2=({struct Cyc_List_List*_tmp487=_region_malloc(rgn,sizeof(*_tmp487));_tmp487->hd=t2,_tmp487->tl=ts2;_tmp487;});}}
# 2664
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp488=_cycalloc(sizeof(*_tmp488));_tmp488->tag=6U,({struct Cyc_List_List*_tmp744=({struct Cyc_List_List*_tmp743=tqts;((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp743,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp488->f1=_tmp744;});_tmp488;});}case 7U: _LLD: _tmp442=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp43B)->f1;_tmp443=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp43B)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp442;struct Cyc_List_List*fs=_tmp443;
# 2668
struct Cyc_List_List*_tmp489=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp489;
if(fs == new_fs)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->tag=7U,_tmp48A->f1=k,_tmp48A->f2=new_fs;_tmp48A;});}case 1U: _LLF: _tmp441=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp43B)->f2;_LL10: {void*r=_tmp441;
# 2672
if(r != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,r);else{
return t;}}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43B)->f2 == 0){_LL11: _tmp440=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43B)->f1;_LL12: {void*c=_tmp440;
return t;}}else{_LL13: _tmp43E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43B)->f1;_tmp43F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43B)->f2;_LL14: {void*c=_tmp43E;struct Cyc_List_List*ts=_tmp43F;
# 2676
struct Cyc_List_List*_tmp48B=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp48B;
if(ts == new_ts)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->tag=0U,_tmp48C->f1=c,_tmp48C->f2=new_ts;_tmp48C;});}}}case 9U: _LL15: _tmp43D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43B)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp43D;
# 2680
struct Cyc_Absyn_Exp*_tmp48D=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp48D;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E->tag=9U,_tmp48E->f1=new_e;_tmp48E;});}case 11U: _LL17: _tmp43C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp43B)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp43C;
# 2683
struct Cyc_Absyn_Exp*_tmp48F=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp48F;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->tag=11U,_tmp490->f1=new_e;_tmp490;});}default: _LL19: _LL1A:
({void*_tmp491=0U;({struct _fat_ptr _tmp745=({const char*_tmp492="found typedecltype in rsubs";_tag_fat(_tmp492,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp745,_tag_fat(_tmp491,sizeof(void*),0U));});});}_LL0:;}
# 2689
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2692
if(ts == 0)
return 0;{
void*_tmp493=(void*)ts->hd;void*old_hd=_tmp493;
struct Cyc_List_List*_tmp494=ts->tl;struct Cyc_List_List*old_tl=_tmp494;
void*_tmp495=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp495;
struct Cyc_List_List*_tmp496=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp496;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->hd=new_hd,_tmp497->tl=new_tl;_tmp497;});}}
# 2703
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 2710
struct _tuple18*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp498=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp498;
return({struct _tuple18*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A->f1=tv,({void*_tmp747=({struct Cyc_Core_Opt*_tmp746=k;Cyc_Absyn_new_evar(_tmp746,({struct Cyc_Core_Opt*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->v=s;_tmp499;}));});_tmp49A->f2=_tmp747;});_tmp49A;});}struct _tuple27{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2715
struct _tuple18*Cyc_Tcutil_r_make_inst_var(struct _tuple27*env,struct Cyc_Absyn_Tvar*tv){
# 2717
struct _tuple27*_tmp49B=env;struct _RegionHandle*_tmp49D;struct Cyc_List_List*_tmp49C;_LL1: _tmp49C=_tmp49B->f1;_tmp49D=_tmp49B->f2;_LL2: {struct Cyc_List_List*s=_tmp49C;struct _RegionHandle*rgn=_tmp49D;
struct Cyc_Core_Opt*_tmp49E=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp49E;
return({struct _tuple18*_tmp4A0=_region_malloc(rgn,sizeof(*_tmp4A0));_tmp4A0->f1=tv,({void*_tmp749=({struct Cyc_Core_Opt*_tmp748=k;Cyc_Absyn_new_evar(_tmp748,({struct Cyc_Core_Opt*_tmp49F=_cycalloc(sizeof(*_tmp49F));_tmp49F->v=s;_tmp49F;}));});_tmp4A0->f2=_tmp749;});_tmp4A0;});}}
# 2722
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2724
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp4A3=({struct Cyc_String_pa_PrintArg_struct _tmp621;_tmp621.tag=0U,_tmp621.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp621;});void*_tmp4A1[1U];_tmp4A1[0]=& _tmp4A3;({unsigned _tmp74B=loc;struct _fat_ptr _tmp74A=({const char*_tmp4A2="bitfield %s does not have constant width";_tag_fat(_tmp4A2,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp74B,_tmp74A,_tag_fat(_tmp4A1,sizeof(void*),1U));});});else{
# 2729
struct _tuple12 _tmp4A4=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple12 _stmttmp6B=_tmp4A4;struct _tuple12 _tmp4A5=_stmttmp6B;int _tmp4A7;unsigned _tmp4A6;_LL1: _tmp4A6=_tmp4A5.f1;_tmp4A7=_tmp4A5.f2;_LL2: {unsigned i=_tmp4A6;int known=_tmp4A7;
if(!known)
({void*_tmp4A8=0U;({unsigned _tmp74D=loc;struct _fat_ptr _tmp74C=({const char*_tmp4A9="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4A9,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp74D,_tmp74C,_tag_fat(_tmp4A8,sizeof(void*),0U));});});
if((int)i < 0)
({void*_tmp4AA=0U;({unsigned _tmp74F=loc;struct _fat_ptr _tmp74E=({const char*_tmp4AB="bitfield has negative width";_tag_fat(_tmp4AB,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp74F,_tmp74E,_tag_fat(_tmp4AA,sizeof(void*),0U));});});
w=i;}}{
# 2736
void*_tmp4AC=Cyc_Tcutil_compress(field_type);void*_stmttmp6C=_tmp4AC;void*_tmp4AD=_stmttmp6C;enum Cyc_Absyn_Size_of _tmp4AE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f1)->tag == 1U){_LL4: _tmp4AE=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4AE;
# 2739
{enum Cyc_Absyn_Size_of _tmp4AF=b;switch(_tmp4AF){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp4B0=0U;({unsigned _tmp751=loc;struct _fat_ptr _tmp750=({const char*_tmp4B1="bitfield larger than type";_tag_fat(_tmp4B1,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp751,_tmp750,_tag_fat(_tmp4B0,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp4B2=0U;({unsigned _tmp753=loc;struct _fat_ptr _tmp752=({const char*_tmp4B3="bitfield larger than type";_tag_fat(_tmp4B3,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp753,_tmp752,_tag_fat(_tmp4B2,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp4B4=0U;({unsigned _tmp755=loc;struct _fat_ptr _tmp754=({const char*_tmp4B5="bitfield larger than type";_tag_fat(_tmp4B5,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp755,_tmp754,_tag_fat(_tmp4B4,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp4B6=0U;({unsigned _tmp757=loc;struct _fat_ptr _tmp756=({const char*_tmp4B7="bitfield larger than type";_tag_fat(_tmp4B7,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp757,_tmp756,_tag_fat(_tmp4B6,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 2747
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2749
({struct Cyc_String_pa_PrintArg_struct _tmp4BA=({struct Cyc_String_pa_PrintArg_struct _tmp623;_tmp623.tag=0U,_tmp623.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp623;});struct Cyc_String_pa_PrintArg_struct _tmp4BB=({struct Cyc_String_pa_PrintArg_struct _tmp622;_tmp622.tag=0U,({
struct _fat_ptr _tmp758=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field_type));_tmp622.f1=_tmp758;});_tmp622;});void*_tmp4B8[2U];_tmp4B8[0]=& _tmp4BA,_tmp4B8[1]=& _tmp4BB;({unsigned _tmp75A=loc;struct _fat_ptr _tmp759=({const char*_tmp4B9="bitfield %s must have integral type but has type %s";_tag_fat(_tmp4B9,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp75A,_tmp759,_tag_fat(_tmp4B8,sizeof(void*),2U));});});
goto _LL3;}_LL3:;}}}
# 2758
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4BC=t;void*_tmp4BE;struct Cyc_Absyn_Typedefdecl*_tmp4BD;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BC)->tag == 8U){_LL1: _tmp4BD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BC)->f3;_tmp4BE=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4BC)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4BD;void*tdopt=_tmp4BE;
# 2761
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({void*_tmp4BF=0U;({unsigned _tmp75C=loc;struct _fat_ptr _tmp75B=({const char*_tmp4C0="extra const";_tag_fat(_tmp4C0,sizeof(char),12U);});Cyc_Tcutil_warn(_tmp75C,_tmp75B,_tag_fat(_tmp4BF,sizeof(void*),0U));});});
return 1;}
# 2766
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);else{
return declared_const;}}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2773
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp75D=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp75D;});}
# 2777
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2783
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2786
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4C3=({struct Cyc_String_pa_PrintArg_struct _tmp625;_tmp625.tag=0U,_tmp625.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp625;});struct Cyc_String_pa_PrintArg_struct _tmp4C4=({struct Cyc_String_pa_PrintArg_struct _tmp624;_tmp624.tag=0U,({struct _fat_ptr _tmp75E=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp624.f1=_tmp75E;});_tmp624;});void*_tmp4C1[2U];_tmp4C1[0]=& _tmp4C3,_tmp4C1[1]=& _tmp4C4;({unsigned _tmp760=loc;struct _fat_ptr _tmp75F=({const char*_tmp4C2="%s: %s";_tag_fat(_tmp4C2,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp760,_tmp75F,_tag_fat(_tmp4C1,sizeof(void*),2U));});});}}}
# 2792
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2796
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2800
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp762=tvs;unsigned _tmp761=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp762,_tmp761,Cyc_Absynpp_tvar2string,({const char*_tmp4C5="duplicate type variable";_tag_fat(_tmp4C5,sizeof(char),24U);}));});}struct _tuple28{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple29{struct Cyc_List_List*f1;void*f2;};struct _tuple30{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2814 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2819
struct _RegionHandle _tmp4C6=_new_region("temp");struct _RegionHandle*temp=& _tmp4C6;_push_region(temp);
# 2823
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp763=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp763,({const char*_tmp4C7="";_tag_fat(_tmp4C7,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4C9=_region_malloc(temp,sizeof(*_tmp4C9));({struct _tuple28*_tmp764=({struct _tuple28*_tmp4C8=_region_malloc(temp,sizeof(*_tmp4C8));_tmp4C8->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4C8->f2=0;_tmp4C8;});_tmp4C9->hd=_tmp764;}),_tmp4C9->tl=fields;_tmp4C9;});}}
# 2828
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2830
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp4EE="struct";_tag_fat(_tmp4EE,sizeof(char),7U);}):({const char*_tmp4EF="union";_tag_fat(_tmp4EF,sizeof(char),6U);});
# 2833
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple29*_tmp4CA=(struct _tuple29*)des->hd;struct _tuple29*_stmttmp6D=_tmp4CA;struct _tuple29*_tmp4CB=_stmttmp6D;void*_tmp4CD;struct Cyc_List_List*_tmp4CC;_LL1: _tmp4CC=_tmp4CB->f1;_tmp4CD=_tmp4CB->f2;_LL2: {struct Cyc_List_List*dl=_tmp4CC;void*a=_tmp4CD;
if(dl == 0){
# 2838
struct Cyc_List_List*_tmp4CE=fields;struct Cyc_List_List*fields2=_tmp4CE;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple28*)fields2->hd)).f2){
(*((struct _tuple28*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp766=({struct Cyc_List_List*_tmp4D0=_cycalloc(sizeof(*_tmp4D0));({void*_tmp765=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4CF=_cycalloc(sizeof(*_tmp4CF));_tmp4CF->tag=1U,_tmp4CF->f1=((*((struct _tuple28*)fields2->hd)).f1)->name;_tmp4CF;});_tmp4D0->hd=_tmp765;}),_tmp4D0->tl=0;_tmp4D0;});(*((struct _tuple29*)des->hd)).f1=_tmp766;});
ans=({struct Cyc_List_List*_tmp4D2=_region_malloc(rgn,sizeof(*_tmp4D2));({struct _tuple30*_tmp767=({struct _tuple30*_tmp4D1=_region_malloc(rgn,sizeof(*_tmp4D1));_tmp4D1->f1=(*((struct _tuple28*)fields2->hd)).f1,_tmp4D1->f2=a;_tmp4D1;});_tmp4D2->hd=_tmp767;}),_tmp4D2->tl=ans;_tmp4D2;});
break;}}
# 2846
if(fields2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4D5=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,_tmp626.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp626;});void*_tmp4D3[1U];_tmp4D3[0]=& _tmp4D5;({unsigned _tmp769=loc;struct _fat_ptr _tmp768=({const char*_tmp4D4="too many arguments to %s";_tag_fat(_tmp4D4,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp769,_tmp768,_tag_fat(_tmp4D3,sizeof(void*),1U));});});}else{
if(dl->tl != 0)
# 2850
({void*_tmp4D6=0U;({unsigned _tmp76B=loc;struct _fat_ptr _tmp76A=({const char*_tmp4D7="multiple designators are not yet supported";_tag_fat(_tmp4D7,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp76B,_tmp76A,_tag_fat(_tmp4D6,sizeof(void*),0U));});});else{
# 2853
void*_tmp4D8=(void*)dl->hd;void*_stmttmp6E=_tmp4D8;void*_tmp4D9=_stmttmp6E;struct _fat_ptr*_tmp4DA;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4D9)->tag == 0U){_LL4: _LL5:
# 2855
({struct Cyc_String_pa_PrintArg_struct _tmp4DD=({struct Cyc_String_pa_PrintArg_struct _tmp627;_tmp627.tag=0U,_tmp627.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp627;});void*_tmp4DB[1U];_tmp4DB[0]=& _tmp4DD;({unsigned _tmp76D=loc;struct _fat_ptr _tmp76C=({const char*_tmp4DC="array designator used in argument to %s";_tag_fat(_tmp4DC,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp76D,_tmp76C,_tag_fat(_tmp4DB,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp4DA=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4D9)->f1;_LL7: {struct _fat_ptr*v=_tmp4DA;
# 2858
struct Cyc_List_List*_tmp4DE=fields;struct Cyc_List_List*fields2=_tmp4DE;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple28*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple28*)fields2->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp4E1=({struct Cyc_String_pa_PrintArg_struct _tmp628;_tmp628.tag=0U,_tmp628.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp628;});void*_tmp4DF[1U];_tmp4DF[0]=& _tmp4E1;({unsigned _tmp76F=loc;struct _fat_ptr _tmp76E=({const char*_tmp4E0="member %s has already been used as an argument";_tag_fat(_tmp4E0,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp76F,_tmp76E,_tag_fat(_tmp4DF,sizeof(void*),1U));});});
(*((struct _tuple28*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4E3=_region_malloc(rgn,sizeof(*_tmp4E3));({struct _tuple30*_tmp770=({struct _tuple30*_tmp4E2=_region_malloc(rgn,sizeof(*_tmp4E2));_tmp4E2->f1=(*((struct _tuple28*)fields2->hd)).f1,_tmp4E2->f2=a;_tmp4E2;});_tmp4E3->hd=_tmp770;}),_tmp4E3->tl=ans;_tmp4E3;});
break;}}
# 2867
if(fields2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4E6=({struct Cyc_String_pa_PrintArg_struct _tmp629;_tmp629.tag=0U,_tmp629.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp629;});void*_tmp4E4[1U];_tmp4E4[0]=& _tmp4E6;({unsigned _tmp772=loc;struct _fat_ptr _tmp771=({const char*_tmp4E5="bad field designator %s";_tag_fat(_tmp4E5,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp772,_tmp771,_tag_fat(_tmp4E4,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}}
# 2872
if((int)aggr_kind == (int)0U)
# 2874
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple28*)fields->hd)).f2){
({void*_tmp4E7=0U;({unsigned _tmp774=loc;struct _fat_ptr _tmp773=({const char*_tmp4E8="too few arguments to struct";_tag_fat(_tmp4E8,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp774,_tmp773,_tag_fat(_tmp4E7,sizeof(void*),0U));});});
break;}}else{
# 2881
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple28*)fields->hd)).f2){
if(found)({void*_tmp4E9=0U;({unsigned _tmp776=loc;struct _fat_ptr _tmp775=({const char*_tmp4EA="only one member of a union is allowed";_tag_fat(_tmp4EA,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp776,_tmp775,_tag_fat(_tmp4E9,sizeof(void*),0U));});});
found=1;}}
# 2887
if(!found)({void*_tmp4EB=0U;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp4EC="missing member for union";_tag_fat(_tmp4EC,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp778,_tmp777,_tag_fat(_tmp4EB,sizeof(void*),0U));});});}{
# 2890
struct Cyc_List_List*_tmp4ED=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp4ED;}}}
# 2823
;_pop_region(temp);}
# 2896
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2898
void*_tmp4F0=e1->r;void*_stmttmp6F=_tmp4F0;void*_tmp4F1=_stmttmp6F;struct Cyc_Absyn_Exp*_tmp4F2;struct Cyc_Absyn_Exp*_tmp4F3;struct Cyc_Absyn_Exp*_tmp4F4;struct Cyc_Absyn_Exp*_tmp4F5;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;switch(*((int*)_tmp4F1)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp4FA=({struct Cyc_String_pa_PrintArg_struct _tmp62A;_tmp62A.tag=0U,({struct _fat_ptr _tmp779=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp62A.f1=_tmp779;});_tmp62A;});void*_tmp4F8[1U];_tmp4F8[0]=& _tmp4FA;({struct _fat_ptr _tmp77A=({const char*_tmp4F9="we have a cast in a lhs:  %s";_tag_fat(_tmp4F9,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp77A,_tag_fat(_tmp4F8,sizeof(void*),1U));});});case 20U: _LL3: _tmp4F7=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4F1)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp4F7;
_tmp4F6=e1a;goto _LL6;}case 23U: _LL5: _tmp4F6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4F1)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp4F6;
# 2902
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp4F5=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4F1)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp4F5;
_tmp4F4=e1a;goto _LLA;}case 21U: _LL9: _tmp4F4=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4F1)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp4F4;
# 2906
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp4FD=({struct Cyc_String_pa_PrintArg_struct _tmp62B;_tmp62B.tag=0U,({
struct _fat_ptr _tmp77B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp62B.f1=_tmp77B;});_tmp62B;});void*_tmp4FB[1U];_tmp4FB[0]=& _tmp4FD;({struct _fat_ptr _tmp77C=({const char*_tmp4FC="found zero pointer aggregate member assignment: %s";_tag_fat(_tmp4FC,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp77C,_tag_fat(_tmp4FB,sizeof(void*),1U));});});
return 0;}case 13U: _LLB: _tmp4F3=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4F1)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp4F3;
_tmp4F2=e1a;goto _LLE;}case 12U: _LLD: _tmp4F2=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4F1)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp4F2;
# 2912
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp500=({struct Cyc_String_pa_PrintArg_struct _tmp62C;_tmp62C.tag=0U,({
struct _fat_ptr _tmp77D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp62C.f1=_tmp77D;});_tmp62C;});void*_tmp4FE[1U];_tmp4FE[0]=& _tmp500;({struct _fat_ptr _tmp77E=({const char*_tmp4FF="found zero pointer instantiate/noinstantiate: %s";_tag_fat(_tmp4FF,sizeof(char),49U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp77E,_tag_fat(_tmp4FE,sizeof(void*),1U));});});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp503=({struct Cyc_String_pa_PrintArg_struct _tmp62D;_tmp62D.tag=0U,({struct _fat_ptr _tmp77F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp62D.f1=_tmp77F;});_tmp62D;});void*_tmp501[1U];_tmp501[0]=& _tmp503;({struct _fat_ptr _tmp780=({const char*_tmp502="found bad lhs in is_zero_ptr_deref: %s";_tag_fat(_tmp502,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp780,_tag_fat(_tmp501,sizeof(void*),1U));});});}_LL0:;}
# 2927
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 2930
void*_tmp504=Cyc_Tcutil_compress(r);void*_stmttmp70=_tmp504;void*_tmp505=_stmttmp70;struct Cyc_Absyn_Tvar*_tmp506;enum Cyc_Absyn_AliasQual _tmp508;enum Cyc_Absyn_KindQual _tmp507;switch(*((int*)_tmp505)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp505)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp505)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp505)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp505)->f4 == 0){_LL5: _tmp507=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp505)->f3)->kind)->v)->kind;_tmp508=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp505)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp507;enum Cyc_Absyn_AliasQual a=_tmp508;
# 2934
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp506=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp505)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp506;
# 2938
struct Cyc_Absyn_Kind*_tmp509=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp71=_tmp509;struct Cyc_Absyn_Kind*_tmp50A=_stmttmp71;enum Cyc_Absyn_AliasQual _tmp50C;enum Cyc_Absyn_KindQual _tmp50B;_LLC: _tmp50B=_tmp50A->kind;_tmp50C=_tmp50A->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp50B;enum Cyc_Absyn_AliasQual a=_tmp50C;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp50D=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp72=_tmp50D;void*_tmp50E=_stmttmp72;struct Cyc_Core_Opt**_tmp50F;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp50E)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp50E)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp50E)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp50F=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp50E)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp50F;
# 2942
({struct Cyc_Core_Opt*_tmp782=({struct Cyc_Core_Opt*_tmp511=_cycalloc(sizeof(*_tmp511));({void*_tmp781=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp510=_cycalloc(sizeof(*_tmp510));_tmp510->tag=2U,_tmp510->f1=0,_tmp510->f2=& Cyc_Tcutil_rk;_tmp510;});_tmp511->v=_tmp781;});_tmp511;});*x=_tmp782;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2947
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2954
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp512=Cyc_Tcutil_compress(t);void*_stmttmp73=_tmp512;void*_tmp513=_stmttmp73;struct Cyc_Absyn_Tvar*_tmp514;void*_tmp515;switch(*((int*)_tmp513)){case 3U: _LL1: _tmp515=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp513)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp515;
# 2957
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp514=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp513)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp514;
# 2959
struct Cyc_Absyn_Kind*_tmp516=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp74=_tmp516;struct Cyc_Absyn_Kind*_tmp517=_stmttmp74;enum Cyc_Absyn_AliasQual _tmp519;enum Cyc_Absyn_KindQual _tmp518;_LL8: _tmp518=_tmp517->kind;_tmp519=_tmp517->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp518;enum Cyc_Absyn_AliasQual a=_tmp519;
enum Cyc_Absyn_KindQual _tmp51A=k;switch(_tmp51A){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp51B=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp75=_tmp51B;void*_tmp51C=_stmttmp75;enum Cyc_Absyn_KindQual _tmp51E;struct Cyc_Core_Opt**_tmp51D;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51C)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51C)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp51D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51C)->f1;_tmp51E=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51C)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp51D;enum Cyc_Absyn_KindQual k=_tmp51E;
# 2965
({struct Cyc_Core_Opt*_tmp785=({struct Cyc_Core_Opt*_tmp521=_cycalloc(sizeof(*_tmp521));({void*_tmp784=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp520=_cycalloc(sizeof(*_tmp520));_tmp520->tag=2U,_tmp520->f1=0,({struct Cyc_Absyn_Kind*_tmp783=({struct Cyc_Absyn_Kind*_tmp51F=_cycalloc(sizeof(*_tmp51F));_tmp51F->kind=k,_tmp51F->aliasqual=Cyc_Absyn_Aliasable;_tmp51F;});_tmp520->f2=_tmp783;});_tmp520;});_tmp521->v=_tmp784;});_tmp521;});*x=_tmp785;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
# 2969
 return 1;}_LL13:;}
# 2972
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2975
 return 0;}_LL0:;}
# 2978
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp522=Cyc_Tcutil_compress(t);void*t=_tmp522;
if(Cyc_Tcutil_is_noalias_pointer(t,0))return 1;{
void*_tmp523=t;struct Cyc_List_List*_tmp524;struct Cyc_List_List*_tmp526;union Cyc_Absyn_DatatypeFieldInfo _tmp525;struct Cyc_List_List*_tmp528;union Cyc_Absyn_DatatypeInfo _tmp527;struct Cyc_List_List*_tmp52A;struct Cyc_Absyn_Aggrdecl**_tmp529;struct Cyc_List_List*_tmp52B;switch(*((int*)_tmp523)){case 6U: _LL1: _tmp52B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp523)->f1;_LL2: {struct Cyc_List_List*qts=_tmp52B;
# 2983
while(qts != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple11*)qts->hd)).f2))return 1;
qts=qts->tl;}
# 2987
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp523)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp523)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp529=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp523)->f1)->f1).KnownAggr).val;_tmp52A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp523)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp529;struct Cyc_List_List*ts=_tmp52A;
# 2989
if((*adp)->impl == 0)return 0;else{
# 2991
struct Cyc_List_List*_tmp52C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp52C;
struct Cyc_List_List*_tmp52D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp52D;
void*t;
while(x != 0){
t=inst == 0?((struct Cyc_Absyn_Aggrfield*)x->hd)->type: Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
x=x->tl;}
# 2999
return 0;}}}else{_LL7: _LL8:
# 3009
 return 0;}case 18U: _LL9: _tmp527=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp523)->f1)->f1;_tmp528=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp523)->f2;_LLA: {union Cyc_Absyn_DatatypeInfo tinfo=_tmp527;struct Cyc_List_List*ts=_tmp528;
# 3011
union Cyc_Absyn_DatatypeInfo _tmp52E=tinfo;struct Cyc_Core_Opt*_tmp530;struct Cyc_List_List*_tmp52F;int _tmp532;struct _tuple1*_tmp531;if((_tmp52E.UnknownDatatype).tag == 1){_LL10: _tmp531=((_tmp52E.UnknownDatatype).val).name;_tmp532=((_tmp52E.UnknownDatatype).val).is_extensible;_LL11: {struct _tuple1*nm=_tmp531;int isxt=_tmp532;
# 3014
return 0;}}else{_LL12: _tmp52F=(*(_tmp52E.KnownDatatype).val)->tvs;_tmp530=(*(_tmp52E.KnownDatatype).val)->fields;_LL13: {struct Cyc_List_List*tvs=_tmp52F;struct Cyc_Core_Opt*flds=_tmp530;
# 3017
return 0;}}_LLF:;}case 19U: _LLB: _tmp525=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp523)->f1)->f1;_tmp526=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp523)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp525;struct Cyc_List_List*ts=_tmp526;
# 3020
union Cyc_Absyn_DatatypeFieldInfo _tmp533=tinfo;struct Cyc_Absyn_Datatypefield*_tmp535;struct Cyc_Absyn_Datatypedecl*_tmp534;if((_tmp533.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 3023
 return 0;}else{_LL17: _tmp534=((_tmp533.KnownDatatypefield).val).f1;_tmp535=((_tmp533.KnownDatatypefield).val).f2;_LL18: {struct Cyc_Absyn_Datatypedecl*td=_tmp534;struct Cyc_Absyn_Datatypefield*fld=_tmp535;
# 3025
struct Cyc_List_List*_tmp536=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp536;
struct Cyc_List_List*_tmp537=fld->typs;struct Cyc_List_List*typs=_tmp537;
while(typs != 0){
t=inst == 0?(*((struct _tuple11*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple11*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
typs=typs->tl;}
# 3032
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp524=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp523)->f2;_LL6: {struct Cyc_List_List*x=_tmp524;
# 3002
while(x != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))return 1;
x=x->tl;}
# 3006
return 0;}default: _LLD: _LLE:
# 3034
 return 0;}_LL0:;}}
# 3041
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp538=e->r;void*_stmttmp76=_tmp538;void*_tmp539=_stmttmp76;struct Cyc_Absyn_Stmt*_tmp53A;struct Cyc_Absyn_Exp*_tmp53B;struct Cyc_Absyn_Exp*_tmp53C;struct Cyc_Absyn_Exp*_tmp53E;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp540;struct Cyc_Absyn_Exp*_tmp53F;struct _fat_ptr*_tmp542;struct Cyc_Absyn_Exp*_tmp541;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Exp*_tmp544;switch(*((int*)_tmp539)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp539)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp544=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp539)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp544;
_tmp543=e1;goto _LL6;}case 20U: _LL5: _tmp543=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp539)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp543;
# 3046
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp541=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp539)->f1;_tmp542=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp539)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp541;struct _fat_ptr*f=_tmp542;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp53F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp539)->f1;_tmp540=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp539)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp53F;struct Cyc_Absyn_Exp*e2=_tmp540;
# 3049
void*_tmp545=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp77=_tmp545;void*_tmp546=_stmttmp77;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp546)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp53D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp539)->f2;_tmp53E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp539)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp53D;struct Cyc_Absyn_Exp*e2=_tmp53E;
# 3054
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp53C=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp539)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp53C;
_tmp53B=e2;goto _LL10;}case 14U: _LLF: _tmp53B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp539)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp53B;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp53A=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp539)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp53A;
# 3058
while(1){
void*_tmp547=s->r;void*_stmttmp78=_tmp547;void*_tmp548=_stmttmp78;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Stmt*_tmp54B;struct Cyc_Absyn_Decl*_tmp54A;struct Cyc_Absyn_Stmt*_tmp54D;struct Cyc_Absyn_Stmt*_tmp54C;switch(*((int*)_tmp548)){case 2U: _LL1B: _tmp54C=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp548)->f1;_tmp54D=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp548)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp54C;struct Cyc_Absyn_Stmt*s2=_tmp54D;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp54A=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp548)->f1;_tmp54B=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp548)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp54A;struct Cyc_Absyn_Stmt*s1=_tmp54B;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp549=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp548)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp549;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({void*_tmp54E=0U;({struct _fat_ptr _tmp786=({const char*_tmp54F="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp54F,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp786,_tag_fat(_tmp54E,sizeof(void*),0U));});});}_LL1A:;}}default: _LL13: _LL14:
# 3066
 return 1;}_LL0:;}
# 3083 "tcutil.cyc"
struct _tuple13 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 3085
struct _tuple13 bogus_ans=({struct _tuple13 _tmp63A;_tmp63A.f1=0,_tmp63A.f2=Cyc_Absyn_heap_rgn_type;_tmp63A;});
void*_tmp550=e->r;void*_stmttmp79=_tmp550;void*_tmp551=_stmttmp79;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp554;int _tmp557;struct _fat_ptr*_tmp556;struct Cyc_Absyn_Exp*_tmp555;int _tmp55A;struct _fat_ptr*_tmp559;struct Cyc_Absyn_Exp*_tmp558;void*_tmp55B;switch(*((int*)_tmp551)){case 1U: _LL1: _tmp55B=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp551)->f1;_LL2: {void*x=_tmp55B;
# 3089
void*_tmp55C=x;struct Cyc_Absyn_Vardecl*_tmp55D;struct Cyc_Absyn_Vardecl*_tmp55E;struct Cyc_Absyn_Vardecl*_tmp55F;struct Cyc_Absyn_Vardecl*_tmp560;switch(*((int*)_tmp55C)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp560=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp55C)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp560;
# 3093
void*_tmp561=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7A=_tmp561;void*_tmp562=_stmttmp7A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp562)->tag == 4U){_LL1B: _LL1C:
# 3095
 return({struct _tuple13 _tmp62E;_tmp62E.f1=1,_tmp62E.f2=Cyc_Absyn_heap_rgn_type;_tmp62E;});}else{_LL1D: _LL1E:
 return({struct _tuple13 _tmp62F;_tmp62F.f1=(vd->tq).real_const,_tmp62F.f2=Cyc_Absyn_heap_rgn_type;_tmp62F;});}_LL1A:;}case 4U: _LL14: _tmp55F=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp55C)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp55F;
# 3099
void*_tmp563=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7B=_tmp563;void*_tmp564=_stmttmp7B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp564)->tag == 4U){_LL20: _LL21:
 return({struct _tuple13 _tmp630;_tmp630.f1=1,_tmp630.f2=(void*)_check_null(vd->rgn);_tmp630;});}else{_LL22: _LL23:
# 3102
 vd->escapes=1;
return({struct _tuple13 _tmp631;_tmp631.f1=(vd->tq).real_const,_tmp631.f2=(void*)_check_null(vd->rgn);_tmp631;});}_LL1F:;}case 5U: _LL16: _tmp55E=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp55C)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp55E;
# 3105
_tmp55D=vd;goto _LL19;}default: _LL18: _tmp55D=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp55C)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp55D;
# 3107
vd->escapes=1;
return({struct _tuple13 _tmp632;_tmp632.f1=(vd->tq).real_const,_tmp632.f2=(void*)_check_null(vd->rgn);_tmp632;});}}_LLD:;}case 21U: _LL3: _tmp558=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp551)->f1;_tmp559=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp551)->f2;_tmp55A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp551)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp558;struct _fat_ptr*f=_tmp559;int is_tagged=_tmp55A;
# 3112
if(is_tagged)return bogus_ans;{
# 3115
void*_tmp565=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7C=_tmp565;void*_tmp566=_stmttmp7C;struct Cyc_Absyn_Aggrdecl*_tmp567;struct Cyc_List_List*_tmp568;switch(*((int*)_tmp566)){case 7U: _LL25: _tmp568=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp566)->f2;_LL26: {struct Cyc_List_List*fs=_tmp568;
# 3117
struct Cyc_Absyn_Aggrfield*_tmp569=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp569;
if(finfo != 0 && finfo->width == 0){
struct _tuple13 _tmp56A=Cyc_Tcutil_addressof_props(e1);struct _tuple13 _stmttmp7D=_tmp56A;struct _tuple13 _tmp56B=_stmttmp7D;void*_tmp56D;int _tmp56C;_LL2C: _tmp56C=_tmp56B.f1;_tmp56D=_tmp56B.f2;_LL2D: {int c=_tmp56C;void*t=_tmp56D;
return({struct _tuple13 _tmp633;_tmp633.f1=(finfo->tq).real_const || c,_tmp633.f2=t;_tmp633;});}}
# 3122
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp566)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp566)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp567=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp566)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp567;
# 3124
struct Cyc_Absyn_Aggrfield*_tmp56E=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp56E;
if(finfo != 0 && finfo->width == 0){
struct _tuple13 _tmp56F=Cyc_Tcutil_addressof_props(e1);struct _tuple13 _stmttmp7E=_tmp56F;struct _tuple13 _tmp570=_stmttmp7E;void*_tmp572;int _tmp571;_LL2F: _tmp571=_tmp570.f1;_tmp572=_tmp570.f2;_LL30: {int c=_tmp571;void*t=_tmp572;
return({struct _tuple13 _tmp634;_tmp634.f1=(finfo->tq).real_const || c,_tmp634.f2=t;_tmp634;});}}
# 3129
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp555=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp551)->f1;_tmp556=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp551)->f2;_tmp557=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp551)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp555;struct _fat_ptr*f=_tmp556;int is_tagged=_tmp557;
# 3134
if(is_tagged)return bogus_ans;{
# 3137
void*_tmp573=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7F=_tmp573;void*_tmp574=_stmttmp7F;void*_tmp576;void*_tmp575;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp574)->tag == 3U){_LL32: _tmp575=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp574)->f1).elt_type;_tmp576=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp574)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp575;void*r=_tmp576;
# 3139
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp577=Cyc_Tcutil_compress(t1);void*_stmttmp80=_tmp577;void*_tmp578=_stmttmp80;struct Cyc_Absyn_Aggrdecl*_tmp579;struct Cyc_List_List*_tmp57A;switch(*((int*)_tmp578)){case 7U: _LL37: _tmp57A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp578)->f2;_LL38: {struct Cyc_List_List*fs=_tmp57A;
# 3142
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp578)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp578)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp579=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp578)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp579;
# 3144
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3147
if(finfo != 0 && finfo->width == 0)
return({struct _tuple13 _tmp635;_tmp635.f1=(finfo->tq).real_const,_tmp635.f2=r;_tmp635;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp554=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp551)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp554;
# 3154
void*_tmp57B=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp81=_tmp57B;void*_tmp57C=_stmttmp81;void*_tmp57E;struct Cyc_Absyn_Tqual _tmp57D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->tag == 3U){_LL3E: _tmp57D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).elt_tq;_tmp57E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57C)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp57D;void*r=_tmp57E;
# 3156
return({struct _tuple13 _tmp636;_tmp636.f1=tq.real_const,_tmp636.f2=r;_tmp636;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp552=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp551)->f1;_tmp553=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp551)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp552;struct Cyc_Absyn_Exp*e2=_tmp553;
# 3162
void*t=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*_tmp57F=t;struct Cyc_Absyn_Tqual _tmp580;void*_tmp582;struct Cyc_Absyn_Tqual _tmp581;struct Cyc_List_List*_tmp583;switch(*((int*)_tmp57F)){case 6U: _LL43: _tmp583=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57F)->f1;_LL44: {struct Cyc_List_List*ts=_tmp583;
# 3166
struct _tuple12 _tmp584=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple12 _stmttmp82=_tmp584;struct _tuple12 _tmp585=_stmttmp82;int _tmp587;unsigned _tmp586;_LL4C: _tmp586=_tmp585.f1;_tmp587=_tmp585.f2;_LL4D: {unsigned i=_tmp586;int known=_tmp587;
if(!known)
return bogus_ans;{
struct _tuple11*_tmp588=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple11*finfo=_tmp588;
if(finfo != 0)
return({struct _tuple13 _tmp637;_tmp637.f1=((*finfo).f1).real_const,({void*_tmp787=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp637.f2=_tmp787;});_tmp637;});
return bogus_ans;}}}case 3U: _LL45: _tmp581=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57F)->f1).elt_tq;_tmp582=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57F)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp581;void*r=_tmp582;
# 3174
return({struct _tuple13 _tmp638;_tmp638.f1=tq.real_const,_tmp638.f2=r;_tmp638;});}case 4U: _LL47: _tmp580=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp57F)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp580;
# 3180
return({struct _tuple13 _tmp639;_tmp639.f1=tq.real_const,({void*_tmp788=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp639.f2=_tmp788;});_tmp639;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3184
({void*_tmp589=0U;({unsigned _tmp78A=e->loc;struct _fat_ptr _tmp789=({const char*_tmp58A="unary & applied to non-lvalue";_tag_fat(_tmp58A,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp78A,_tmp789,_tag_fat(_tmp589,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3191
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp58B=({void*_tmp78B=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp78B,b);});struct Cyc_Absyn_Exp*eopt=_tmp58B;
if(eopt == 0)return;{
struct Cyc_Absyn_Exp*_tmp58C=eopt;struct Cyc_Absyn_Exp*e=_tmp58C;
struct _tuple12 _tmp58D=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple12 _stmttmp83=_tmp58D;struct _tuple12 _tmp58E=_stmttmp83;int _tmp590;unsigned _tmp58F;_LL1: _tmp58F=_tmp58E.f1;_tmp590=_tmp58E.f2;_LL2: {unsigned j=_tmp58F;int known=_tmp590;
if(known && j <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp593=({struct Cyc_Int_pa_PrintArg_struct _tmp63C;_tmp63C.tag=1U,_tmp63C.f1=(unsigned long)((int)j);_tmp63C;});struct Cyc_Int_pa_PrintArg_struct _tmp594=({struct Cyc_Int_pa_PrintArg_struct _tmp63B;_tmp63B.tag=1U,_tmp63B.f1=(unsigned long)((int)i);_tmp63B;});void*_tmp591[2U];_tmp591[0]=& _tmp593,_tmp591[1]=& _tmp594;({unsigned _tmp78D=loc;struct _fat_ptr _tmp78C=({const char*_tmp592="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp592,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp78D,_tmp78C,_tag_fat(_tmp591,sizeof(void*),2U));});});else{
# 3200
({struct Cyc_Int_pa_PrintArg_struct _tmp597=({struct Cyc_Int_pa_PrintArg_struct _tmp63E;_tmp63E.tag=1U,_tmp63E.f1=(unsigned long)((int)j);_tmp63E;});struct Cyc_Int_pa_PrintArg_struct _tmp598=({struct Cyc_Int_pa_PrintArg_struct _tmp63D;_tmp63D.tag=1U,_tmp63D.f1=(unsigned long)((int)i);_tmp63D;});void*_tmp595[2U];_tmp595[0]=& _tmp597,_tmp595[1]=& _tmp598;({unsigned _tmp78F=loc;struct _fat_ptr _tmp78E=({const char*_tmp596="dereference is out of bounds: %d <= %d";_tag_fat(_tmp596,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp78F,_tmp78E,_tag_fat(_tmp595,sizeof(void*),2U));});});}}
return;}}}
# 3204
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3213
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp599=e->r;void*_stmttmp84=_tmp599;void*_tmp59A=_stmttmp84;struct Cyc_List_List*_tmp59B;struct Cyc_List_List*_tmp59C;struct Cyc_List_List*_tmp59E;enum Cyc_Absyn_Primop _tmp59D;struct Cyc_List_List*_tmp59F;struct Cyc_List_List*_tmp5A0;struct Cyc_List_List*_tmp5A1;struct Cyc_Absyn_Exp*_tmp5A2;struct Cyc_Absyn_Exp*_tmp5A4;struct Cyc_Absyn_Exp*_tmp5A3;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Exp*_tmp5A7;void*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A9;void*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5AF;void*_tmp5B2;switch(*((int*)_tmp59A)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5B2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL10: {void*b=_tmp5B2;
# 3225
void*_tmp5B3=b;struct Cyc_Absyn_Vardecl*_tmp5B4;struct Cyc_Absyn_Vardecl*_tmp5B5;switch(*((int*)_tmp5B3)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp5B5=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5B3)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5B5;
# 3228
void*_tmp5B6=Cyc_Tcutil_compress(vd->type);void*_stmttmp85=_tmp5B6;void*_tmp5B7=_stmttmp85;switch(*((int*)_tmp5B7)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp5B4=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5B3)->f1;_LL39: {struct Cyc_Absyn_Vardecl*vd=_tmp5B4;
# 3235
if((int)vd->sc == (int)Cyc_Absyn_Static){
void*_tmp5B8=Cyc_Tcutil_compress(vd->type);void*_stmttmp86=_tmp5B8;void*_tmp5B9=_stmttmp86;switch(*((int*)_tmp5B9)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3242
return var_okay;}}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp5AF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_tmp5B0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59A)->f2;_tmp5B1=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59A)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5AF;struct Cyc_Absyn_Exp*e2=_tmp5B0;struct Cyc_Absyn_Exp*e3=_tmp5B1;
# 3248
return(Cyc_Tcutil_cnst_exp(0,e1)&&
 Cyc_Tcutil_cnst_exp(0,e2))&&
 Cyc_Tcutil_cnst_exp(0,e3);}case 9U: _LL13: _tmp5AD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_tmp5AE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59A)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5AD;struct Cyc_Absyn_Exp*e2=_tmp5AE;
# 3252
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 41U: _LL15: _tmp5AC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5AC;
_tmp5AB=e2;goto _LL18;}case 12U: _LL17: _tmp5AB=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5AB;
_tmp5AA=e2;goto _LL1A;}case 13U: _LL19: _tmp5AA=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5AA;
# 3256
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59A)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp5A8=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_tmp5A9=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59A)->f2;_LL1C: {void*t=_tmp5A8;struct Cyc_Absyn_Exp*e2=_tmp5A9;
# 3258
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}else{_LL1D: _tmp5A6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_tmp5A7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59A)->f2;_LL1E: {void*t=_tmp5A6;struct Cyc_Absyn_Exp*e2=_tmp5A7;
# 3261
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}case 15U: _LL1F: _tmp5A5=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp5A5;
# 3263
return Cyc_Tcutil_cnst_exp(1,e2);}case 27U: _LL21: _tmp5A3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59A)->f2;_tmp5A4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59A)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp5A3;struct Cyc_Absyn_Exp*e2=_tmp5A4;
# 3265
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 28U: _LL23: _tmp5A2=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp5A2;
# 3267
return Cyc_Tcutil_cnst_exp(0,e);}case 26U: _LL25: _tmp5A1=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL26: {struct Cyc_List_List*des=_tmp5A1;
_tmp5A0=des;goto _LL28;}case 30U: _LL27: _tmp5A0=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp59A)->f2;_LL28: {struct Cyc_List_List*des=_tmp5A0;
_tmp59F=des;goto _LL2A;}case 29U: _LL29: _tmp59F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp59A)->f3;_LL2A: {struct Cyc_List_List*des=_tmp59F;
# 3271
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple15*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL2B: _tmp59D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_tmp59E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59A)->f2;_LL2C: {enum Cyc_Absyn_Primop p=_tmp59D;struct Cyc_List_List*es=_tmp59E;
# 3276
_tmp59C=es;goto _LL2E;}case 24U: _LL2D: _tmp59C=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL2E: {struct Cyc_List_List*es=_tmp59C;
_tmp59B=es;goto _LL30;}case 31U: _LL2F: _tmp59B=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp59A)->f1;_LL30: {struct Cyc_List_List*es=_tmp59B;
# 3279
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3286
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3290
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5BA=Cyc_Tcutil_compress(t);void*_stmttmp87=_tmp5BA;void*_tmp5BB=_stmttmp87;struct Cyc_List_List*_tmp5BC;struct Cyc_List_List*_tmp5BD;void*_tmp5BE;void*_tmp5BF;struct Cyc_List_List*_tmp5C1;void*_tmp5C0;switch(*((int*)_tmp5BB)){case 0U: _LL1: _tmp5C0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5BB)->f1;_tmp5C1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5BB)->f2;_LL2: {void*c=_tmp5C0;struct Cyc_List_List*ts=_tmp5C1;
# 3294
void*_tmp5C2=c;union Cyc_Absyn_AggrInfo _tmp5C3;struct Cyc_List_List*_tmp5C4;struct Cyc_Absyn_Enumdecl*_tmp5C5;switch(*((int*)_tmp5C2)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5C5=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5C2)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5C5;
# 3299
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5C4=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5C4=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5C2)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5C4;
# 3303
{struct Cyc_List_List*_tmp5C6=fs;struct Cyc_List_List*fs2=_tmp5C6;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple12 _tmp5C7=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple12 _stmttmp88=_tmp5C7;struct _tuple12 _tmp5C8=_stmttmp88;int _tmp5CA;unsigned _tmp5C9;_LL1D: _tmp5C9=_tmp5C8.f1;_tmp5CA=_tmp5C8.f2;_LL1E: {unsigned i=_tmp5C9;int known=_tmp5CA;
if(known && i == (unsigned)0)
return 1;}}}}
# 3310
return 0;}case 20U: _LL18: _tmp5C3=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5C2)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5C3;
# 3313
struct Cyc_Absyn_Aggrdecl*_tmp5CB=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5CB;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BB)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5BF;
# 3321
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5BB)->f1).elt_type;_LL6: {void*t=_tmp5BE;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5BD=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5BB)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5BD;
# 3324
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple11*)tqs->hd)).f2))return 0;}
return 1;}case 7U: _LL9: _tmp5BC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5BB)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5BC;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3331
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3333
struct _RegionHandle _tmp5CC=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5CC;_push_region(rgn);
{struct Cyc_List_List*_tmp5CD=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5CD;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5CE=0;_npop_handler(0U);return _tmp5CE;}}{
# 3341
int _tmp5CF=1;_npop_handler(0U);return _tmp5CF;}}
# 3334
;_pop_region(rgn);}
# 3345
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5D0=t;struct Cyc_Absyn_Typedefdecl*_tmp5D1;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5D0)->tag == 8U){_LL1: _tmp5D1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5D0)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5D1;
# 3348
if(tdopt != 0){
struct Cyc_Absyn_Tqual _tmp5D2=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5D2;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp5D5=({struct Cyc_String_pa_PrintArg_struct _tmp63F;_tmp63F.tag=0U,({struct _fat_ptr _tmp790=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp63F.f1=_tmp790;});_tmp63F;});void*_tmp5D3[1U];_tmp5D3[0]=& _tmp5D5;({unsigned _tmp792=loc;struct _fat_ptr _tmp791=({const char*_tmp5D4="qualifier within typedef type %s is ignored";_tag_fat(_tmp5D4,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp792,_tmp791,_tag_fat(_tmp5D3,sizeof(void*),1U));});});}
# 3353
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3360
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp5D6=Cyc_Tcutil_compress(t);void*_stmttmp89=_tmp5D6;void*_tmp5D7=_stmttmp89;struct Cyc_List_List**_tmp5D8;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D7)->tag == 5U){_LL1: _tmp5D8=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5D7)->f1).attributes;_LL2: {struct Cyc_List_List**fnatts=_tmp5D8;
# 3363
struct Cyc_List_List*_tmp5D9=0;struct Cyc_List_List*res_atts=_tmp5D9;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_tmp793=({struct Cyc_List_List*_tmp5DA=_cycalloc(sizeof(*_tmp5DA));_tmp5DA->hd=(void*)atts->hd,_tmp5DA->tl=*fnatts;_tmp5DA;});*fnatts=_tmp793;});}else{
# 3370
res_atts=({struct Cyc_List_List*_tmp5DB=_cycalloc(sizeof(*_tmp5DB));_tmp5DB->hd=(void*)atts->hd,_tmp5DB->tl=res_atts;_tmp5DB;});}}
return res_atts;}}else{_LL3: _LL4:
({void*_tmp5DC=0U;({struct _fat_ptr _tmp794=({const char*_tmp5DD="transfer_fn_type_atts";_tag_fat(_tmp5DD,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp794,_tag_fat(_tmp5DC,sizeof(void*),0U));});});}_LL0:;}
# 3377
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5DE=Cyc_Tcutil_compress(t);void*_stmttmp8A=_tmp5DE;void*_tmp5DF=_stmttmp8A;struct Cyc_Absyn_Exp*_tmp5E0;struct Cyc_Absyn_PtrInfo _tmp5E1;switch(*((int*)_tmp5DF)){case 3U: _LL1: _tmp5E1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5DF)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp5E1;
return({void*_tmp795=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp795,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp5E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5DF)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5E0;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3387
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5E2=b;struct Cyc_Absyn_Vardecl*_tmp5E3;struct Cyc_Absyn_Vardecl*_tmp5E4;struct Cyc_Absyn_Vardecl*_tmp5E5;struct Cyc_Absyn_Vardecl*_tmp5E6;switch(*((int*)_tmp5E2)){case 5U: _LL1: _tmp5E6=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5E2)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp5E6;
_tmp5E5=x;goto _LL4;}case 4U: _LL3: _tmp5E5=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5E2)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp5E5;
_tmp5E4=x;goto _LL6;}case 3U: _LL5: _tmp5E4=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5E2)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp5E4;
_tmp5E3=x;goto _LL8;}case 1U: _LL7: _tmp5E3=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5E2)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp5E3;
# 3393
if(!x->escapes)
return x;
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3401
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5E7=0;struct Cyc_List_List*res=_tmp5E7;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)res=({struct Cyc_List_List*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->hd=*((void**)_check_null((void**)x->hd)),_tmp5E8->tl=res;_tmp5E8;});}}
return res;}
# 3408
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5E9=Cyc_Tcutil_compress(t);void*_stmttmp8B=_tmp5E9;void*_tmp5EA=_stmttmp8B;unsigned _tmp5EF;void*_tmp5EE;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_Tqual _tmp5EC;void*_tmp5EB;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EA)->tag == 4U){_LL1: _tmp5EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EA)->f1).elt_type;_tmp5EC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EA)->f1).tq;_tmp5ED=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EA)->f1).num_elts;_tmp5EE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EA)->f1).zero_term;_tmp5EF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EA)->f1).zt_loc;_LL2: {void*et=_tmp5EB;struct Cyc_Absyn_Tqual tq=_tmp5EC;struct Cyc_Absyn_Exp*eopt=_tmp5ED;void*zt=_tmp5EE;unsigned ztl=_tmp5EF;
# 3411
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 3415
if(convert_tag){
if(eopt->topt == 0)
({void*_tmp5F0=0U;({struct _fat_ptr _tmp796=({const char*_tmp5F1="cannot convert tag without type!";_tag_fat(_tmp5F1,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp796,_tag_fat(_tmp5F0,sizeof(void*),0U));});});{
void*_tmp5F2=Cyc_Tcutil_compress((void*)_check_null(eopt->topt));void*_stmttmp8C=_tmp5F2;void*_tmp5F3=_stmttmp8C;void*_tmp5F4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F3)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F3)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F3)->f2 != 0){_LL6: _tmp5F4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F3)->f2)->hd;_LL7: {void*t=_tmp5F4;
# 3421
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 3424
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3428
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
# 3430
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
