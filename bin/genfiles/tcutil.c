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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 879 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 881
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 888
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 893
void*Cyc_Absyn_compress_kb(void*);
# 899
int Cyc_Absyn_type2bool(int def,void*);
# 904
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 909
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 911
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 916
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 918
extern void*Cyc_Absyn_empty_effect;
# 920
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 922
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 947
extern void*Cyc_Absyn_fat_bound_type;
# 949
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 951
void*Cyc_Absyn_bounds_one();
# 953
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 957
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 976
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 978
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 981
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 991
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1004
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1016
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1023
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
# 1035
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1040
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1083
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1127
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1129
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1131
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1137
int Cyc_Absyn_fntype_att(void*);
# 1139
int Cyc_Absyn_equal_att(void*,void*);
# 1141
int Cyc_Absyn_attribute_cmp(void*,void*);
# 1147
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 69
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 76
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 27 "warn.h"
void Cyc_Warn_vwarn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 33
void Cyc_Warn_verr(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 38
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 69
void Cyc_Warn_warn2(unsigned,struct _fat_ptr);struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_uint_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 50
int Cyc_Evexp_uint_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
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
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple14{int f1;void*f2;};
# 250
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 253
void*Cyc_Tcutil_normalize_effect(void*e);
# 256
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 258
int Cyc_Tcutil_new_tvar_id();
# 260
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 273
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 280
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 298
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 301
int Cyc_Tcutil_zeroable_type(void*);
# 305
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 313
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 317
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 322
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
int Cyc_Tcutil_is_nullable_pointer_type(void*t,int def){
void*_tmp1E=Cyc_Tcutil_compress(t);void*_stmttmpD=_tmp1E;void*_tmp1F=_stmttmpD;void*_tmp20;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->tag == 3U){_LL1: _tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp20;
# 154
return Cyc_Tcutil_force_type2bool(def,nbl);}}else{_LL3: _LL4:
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
({void*_tmp650=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp650;});
return 1;}else{
return 0;}}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 223
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 229
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp651=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp651,b);});struct Cyc_Absyn_Exp*eopt=_tmp37;
if(eopt == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp38=eopt;struct Cyc_Absyn_Exp*e=_tmp38;
struct _tuple13 _tmp39=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple13 _stmttmp13=_tmp39;struct _tuple13 _tmp3A=_stmttmp13;int _tmp3C;unsigned _tmp3B;_LL1: _tmp3B=_tmp3A.f1;_tmp3C=_tmp3A.f2;_LL2: {unsigned i=_tmp3B;int known=_tmp3C;
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
;_pop_region();}}}}default: _LL1A: _LL1B:
# 260
 return 0;}_LLB:;}case 4U: _LL3: _tmp41=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).elt_type;_tmp42=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zero_term;_LL4: {void*t=_tmp41;void*zero_term=_tmp42;
# 265
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6U: _LL5: _tmp40=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E)->f1;_LL6: {struct Cyc_List_List*tqs=_tmp40;
# 267
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)tqs->hd)).f2))return 0;}
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
return({struct Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=tv->name,_tmp60->identity=- 1,({void*_tmp652=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp60->kind=_tmp652;});_tmp60;});}
# 367
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 369
struct _tuple9*_tmp61=arg;void*_tmp64;struct Cyc_Absyn_Tqual _tmp63;struct _fat_ptr*_tmp62;_LL1: _tmp62=_tmp61->f1;_tmp63=_tmp61->f2;_tmp64=_tmp61->f3;_LL2: {struct _fat_ptr*x=_tmp62;struct Cyc_Absyn_Tqual y=_tmp63;void*t=_tmp64;
return({struct _tuple9*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=x,_tmp65->f2=y,({void*_tmp653=Cyc_Tcutil_copy_type(t);_tmp65->f3=_tmp653;});_tmp65;});}}
# 372
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=(*arg).f1,({void*_tmp654=Cyc_Tcutil_copy_type((*arg).f2);_tmp66->f2=_tmp654;});_tmp66;});}
# 375
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}
# 379
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->name=f->name,_tmp67->tq=f->tq,({void*_tmp656=Cyc_Tcutil_copy_type(f->type);_tmp67->type=_tmp656;}),_tmp67->width=f->width,_tmp67->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp655=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp67->requires_clause=_tmp655;});_tmp67;});}struct _tuple15{void*f1;void*f2;};
# 383
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp68=x;void*_tmp6A;void*_tmp69;_LL1: _tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;_LL2: {void*r1=_tmp69;void*r2=_tmp6A;
return({struct _tuple15*_tmp6B=_cycalloc(sizeof(*_tmp6B));({void*_tmp658=Cyc_Tcutil_copy_type(r1);_tmp6B->f1=_tmp658;}),({void*_tmp657=Cyc_Tcutil_copy_type(r2);_tmp6B->f2=_tmp657;});_tmp6B;});}}
# 387
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 391
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6C=Cyc_Tcutil_compress(t);void*_stmttmp19=_tmp6C;void*_tmp6D=_stmttmp19;struct Cyc_Absyn_Datatypedecl*_tmp6E;struct Cyc_Absyn_Enumdecl*_tmp6F;struct Cyc_Absyn_Aggrdecl*_tmp70;struct Cyc_Absyn_Typedefdecl*_tmp73;struct Cyc_List_List*_tmp72;struct _tuple1*_tmp71;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp77;enum Cyc_Absyn_AggrKind _tmp76;struct Cyc_List_List*_tmp78;struct Cyc_Absyn_Vardecl*_tmp86;struct Cyc_List_List*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_List_List*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_VarargInfo*_tmp7F;int _tmp7E;struct Cyc_List_List*_tmp7D;void*_tmp7C;struct Cyc_Absyn_Tqual _tmp7B;void*_tmp7A;struct Cyc_List_List*_tmp79;unsigned _tmp8B;void*_tmp8A;struct Cyc_Absyn_Exp*_tmp89;struct Cyc_Absyn_Tqual _tmp88;void*_tmp87;struct Cyc_Absyn_PtrLoc*_tmp92;void*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;struct Cyc_Absyn_Tqual _tmp8D;void*_tmp8C;struct Cyc_Absyn_Tvar*_tmp93;struct Cyc_List_List*_tmp95;void*_tmp94;void*_tmp96;switch(*((int*)_tmp6D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2 == 0){_LL1: _tmp96=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_LL2: {void*c=_tmp96;
return t;}}else{_LL3: _tmp94=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_tmp95=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2;_LL4: {void*c=_tmp94;struct Cyc_List_List*ts=_tmp95;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp97=_cycalloc(sizeof(*_tmp97));_tmp97->tag=0U,_tmp97->f1=c,({struct Cyc_List_List*_tmp659=Cyc_Tcutil_copy_types(ts);_tmp97->f2=_tmp659;});_tmp97;});}}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp93=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6D)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp93;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3U: _LL9: _tmp8C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp8D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).elt_tq;_tmp8E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).rgn;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).nullable;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).bounds;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).zero_term;_tmp92=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6D)->f1).ptr_atts).ptrloc;_LLA: {void*elt=_tmp8C;struct Cyc_Absyn_Tqual tq=_tmp8D;void*rgn=_tmp8E;void*nbl=_tmp8F;void*bs=_tmp90;void*zt=_tmp91;struct Cyc_Absyn_PtrLoc*loc=_tmp92;
# 398
void*_tmp98=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp98;
void*_tmp99=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp99;
void*_tmp9A=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp9A;
struct Cyc_Absyn_Tqual _tmp9B=tq;struct Cyc_Absyn_Tqual tq2=_tmp9B;
# 403
void*_tmp9C=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp9C;
void*_tmp9D=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp9D;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=3U,(_tmp9E->f1).elt_type=elt2,(_tmp9E->f1).elt_tq=tq2,((_tmp9E->f1).ptr_atts).rgn=rgn2,((_tmp9E->f1).ptr_atts).nullable=nbl2,((_tmp9E->f1).ptr_atts).bounds=bs2,((_tmp9E->f1).ptr_atts).zero_term=zt2,((_tmp9E->f1).ptr_atts).ptrloc=loc;_tmp9E;});}case 4U: _LLB: _tmp87=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).elt_type;_tmp88=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).tq;_tmp89=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).num_elts;_tmp8A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).zero_term;_tmp8B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6D)->f1).zt_loc;_LLC: {void*et=_tmp87;struct Cyc_Absyn_Tqual tq=_tmp88;struct Cyc_Absyn_Exp*eo=_tmp89;void*zt=_tmp8A;unsigned ztl=_tmp8B;
# 407
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=4U,({void*_tmp65C=Cyc_Tcutil_copy_type(et);(_tmp9F->f1).elt_type=_tmp65C;}),(_tmp9F->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp65B=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp9F->f1).num_elts=_tmp65B;}),({
void*_tmp65A=Cyc_Tcutil_copy_type(zt);(_tmp9F->f1).zero_term=_tmp65A;}),(_tmp9F->f1).zt_loc=ztl;_tmp9F;});}case 5U: _LLD: _tmp79=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).tvars;_tmp7A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).effect;_tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_tqual;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_type;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).args;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).c_varargs;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).cyc_varargs;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).rgn_po;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).attributes;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_clause;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_relns;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_clause;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_relns;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).return_value;_LLE: {struct Cyc_List_List*tvs=_tmp79;void*effopt=_tmp7A;struct Cyc_Absyn_Tqual rt_tq=_tmp7B;void*rt=_tmp7C;struct Cyc_List_List*args=_tmp7D;int c_varargs=_tmp7E;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp7F;struct Cyc_List_List*rgn_po=_tmp80;struct Cyc_List_List*atts=_tmp81;struct Cyc_Absyn_Exp*req=_tmp82;struct Cyc_List_List*req_rlns=_tmp83;struct Cyc_Absyn_Exp*ens=_tmp84;struct Cyc_List_List*ens_rlns=_tmp85;struct Cyc_Absyn_Vardecl*ret_val=_tmp86;
# 410
struct Cyc_List_List*_tmpA0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmpA0;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmpA1=Cyc_Tcutil_copy_type(rt);void*rt2=_tmpA1;
struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmpA2;
int _tmpA3=c_varargs;int c_varargs2=_tmpA3;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo*cv=cyc_varargs;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA4=_cycalloc(sizeof(*_tmpA4));_tmpA4->name=cv->name,_tmpA4->tq=cv->tq,({void*_tmp65D=Cyc_Tcutil_copy_type(cv->type);_tmpA4->type=_tmp65D;}),_tmpA4->inject=cv->inject;_tmpA4;});}{
# 421
struct Cyc_List_List*_tmpA5=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmpA5;
struct Cyc_List_List*_tmpA6=atts;struct Cyc_List_List*atts2=_tmpA6;
struct Cyc_Absyn_Exp*_tmpA7=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmpA7;
struct Cyc_List_List*_tmpA8=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmpA8;
struct Cyc_Absyn_Exp*_tmpA9=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmpA9;
struct Cyc_List_List*_tmpAA=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmpAA;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmpAB=_cycalloc(sizeof(*_tmpAB));*_tmpAB=*ret_val;_tmpAB;});
({void*_tmp65E=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp65E;});
if((unsigned)ret_val2->rgn)({void*_tmp65F=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp65F;});}
# 433
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=5U,(_tmpAC->f1).tvars=tvs2,(_tmpAC->f1).effect=effopt2,(_tmpAC->f1).ret_tqual=rt_tq,(_tmpAC->f1).ret_type=rt2,(_tmpAC->f1).args=args2,(_tmpAC->f1).c_varargs=c_varargs2,(_tmpAC->f1).cyc_varargs=cyc_varargs2,(_tmpAC->f1).rgn_po=rgn_po2,(_tmpAC->f1).attributes=atts2,(_tmpAC->f1).requires_clause=req2,(_tmpAC->f1).requires_relns=req_rlns2,(_tmpAC->f1).ensures_clause=ens2,(_tmpAC->f1).ensures_relns=ens_rlns2,(_tmpAC->f1).return_value=ret_val2;_tmpAC;});}}case 6U: _LLF: _tmp78=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp78;
# 436
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=6U,({struct Cyc_List_List*_tmp660=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmpAD->f1=_tmp660;});_tmpAD;});}case 7U: _LL11: _tmp76=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f1;_tmp77=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp76;struct Cyc_List_List*fs=_tmp77;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=7U,_tmpAE->f1=k,({struct Cyc_List_List*_tmp661=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmpAE->f2=_tmp661;});_tmpAE;});}case 9U: _LL13: _tmp75=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp75;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=9U,_tmpAF->f1=e;_tmpAF;});}case 11U: _LL15: _tmp74=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6D)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp74;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=11U,_tmpB0->f1=e;_tmpB0;});}case 8U: _LL17: _tmp71=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f1;_tmp72=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f2;_tmp73=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f3;_LL18: {struct _tuple1*tdn=_tmp71;struct Cyc_List_List*ts=_tmp72;struct Cyc_Absyn_Typedefdecl*td=_tmp73;
# 441
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=8U,_tmpB1->f1=tdn,({struct Cyc_List_List*_tmp662=Cyc_Tcutil_copy_types(ts);_tmpB1->f2=_tmp662;}),_tmpB1->f3=td,_tmpB1->f4=0;_tmpB1;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)){case 0U: _LL19: _tmp70=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp70;
# 444
struct Cyc_List_List*_tmpB2=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpB2;
return({union Cyc_Absyn_AggrInfo _tmp663=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp663,targs);});}case 1U: _LL1B: _tmp6F=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp6F;
# 447
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp6E=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp6E;
# 449
struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpB3;
return({union Cyc_Absyn_DatatypeInfo _tmp664=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp602;_tmp602.name=dd->name,_tmp602.is_extensible=0;_tmp602;}));Cyc_Absyn_datatype_type(_tmp664,targs);});}}}_LL0:;}
# 456
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB4=d;struct _fat_ptr*_tmpB5;struct Cyc_Absyn_Exp*_tmpB6;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB4)->tag == 0U){_LL1: _tmpB6=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB4)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpB6;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->tag=0U,({struct Cyc_Absyn_Exp*_tmp665=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpB7->f1=_tmp665;});_tmpB7;});}}else{_LL3: _tmpB5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB4)->f1;_LL4: {struct _fat_ptr*v=_tmpB5;
return d;}}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 462
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 464
return({struct _tuple16*_tmpB8=_cycalloc(sizeof(*_tmpB8));({struct Cyc_List_List*_tmp667=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB8->f1=_tmp667;}),({struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB8->f2=_tmp666;});_tmpB8;});}
# 467
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB9=preserve_types;int pt=_tmpB9;
{void*_tmpBA=e->r;void*_stmttmp1A=_tmpBA;void*_tmpBB=_stmttmp1A;struct Cyc_Absyn_Exp*_tmpBC;struct Cyc_List_List*_tmpC1;struct Cyc_List_List*_tmpC0;struct Cyc_List_List*_tmpBF;struct _fat_ptr _tmpBE;int _tmpBD;void*_tmpC2;struct _fat_ptr*_tmpC4;struct Cyc_Absyn_Exp*_tmpC3;struct Cyc_List_List*_tmpC6;struct Cyc_Core_Opt*_tmpC5;struct Cyc_Absyn_Exp*_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;int _tmpCE;int _tmpCD;struct Cyc_Absyn_Exp*_tmpCC;void**_tmpCB;struct Cyc_Absyn_Exp*_tmpCA;int _tmpC9;struct Cyc_Absyn_Enumfield*_tmpD0;void*_tmpCF;struct Cyc_Absyn_Enumfield*_tmpD2;struct Cyc_Absyn_Enumdecl*_tmpD1;struct Cyc_List_List*_tmpD6;void*_tmpD5;struct Cyc_Absyn_Tqual _tmpD4;struct _fat_ptr*_tmpD3;struct Cyc_List_List*_tmpD8;void*_tmpD7;struct Cyc_Absyn_Aggrdecl*_tmpDC;struct Cyc_List_List*_tmpDB;struct Cyc_List_List*_tmpDA;struct _tuple1*_tmpD9;int _tmpDF;void*_tmpDE;struct Cyc_Absyn_Exp*_tmpDD;int _tmpE3;struct Cyc_Absyn_Exp*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Vardecl*_tmpE0;struct Cyc_Absyn_Datatypefield*_tmpE6;struct Cyc_Absyn_Datatypedecl*_tmpE5;struct Cyc_List_List*_tmpE4;struct Cyc_List_List*_tmpE7;struct Cyc_Absyn_Exp*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;int _tmpED;int _tmpEC;struct _fat_ptr*_tmpEB;struct Cyc_Absyn_Exp*_tmpEA;int _tmpF1;int _tmpF0;struct _fat_ptr*_tmpEF;struct Cyc_Absyn_Exp*_tmpEE;struct Cyc_List_List*_tmpF2;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF4;struct Cyc_List_List*_tmpF6;void*_tmpF5;struct Cyc_Absyn_Exp*_tmpF7;void*_tmpF8;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;enum Cyc_Absyn_Coercion _tmpFF;int _tmpFE;struct Cyc_Absyn_Exp*_tmpFD;void*_tmpFC;struct Cyc_List_List*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmp102;int _tmp104;struct Cyc_Absyn_Exp*_tmp103;int _tmp108;struct Cyc_Absyn_VarargCallInfo*_tmp107;struct Cyc_List_List*_tmp106;struct Cyc_Absyn_Exp*_tmp105;int _tmp111;int _tmp110;void*_tmp10F;struct Cyc_Absyn_Tqual _tmp10E;struct _fat_ptr*_tmp10D;struct Cyc_List_List*_tmp10C;int _tmp10B;struct Cyc_List_List*_tmp10A;struct Cyc_Absyn_Exp*_tmp109;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Core_Opt*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;enum Cyc_Absyn_Incrementor _tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_List_List*_tmp121;enum Cyc_Absyn_Primop _tmp120;struct _fat_ptr _tmp122;void*_tmp123;union Cyc_Absyn_Cnst _tmp124;switch(*((int*)_tmpBB)){case 0U: _LL1: _tmp124=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp124;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp123=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL4: {void*b=_tmp123;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp122=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL6: {struct _fat_ptr p=_tmp122;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp120=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp121=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp120;struct Cyc_List_List*es=_tmp121;
new_e=({enum Cyc_Absyn_Primop _tmp669=p;struct Cyc_List_List*_tmp668=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_primop_exp(_tmp669,_tmp668,e->loc);});goto _LL0;}case 5U: _LL9: _tmp11E=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp11F=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp11E;enum Cyc_Absyn_Incrementor i=_tmp11F;
new_e=({struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_deep_copy_exp(pt,e1);enum Cyc_Absyn_Incrementor _tmp66A=i;Cyc_Absyn_increment_exp(_tmp66B,_tmp66A,e->loc);});goto _LL0;}case 7U: _LLB: _tmp11C=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp11D=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp11C;struct Cyc_Absyn_Exp*e2=_tmp11D;
new_e=({struct Cyc_Absyn_Exp*_tmp66D=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp66C=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_and_exp(_tmp66D,_tmp66C,e->loc);});goto _LL0;}case 8U: _LLD: _tmp11A=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp11B=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
new_e=({struct Cyc_Absyn_Exp*_tmp66F=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_or_exp(_tmp66F,_tmp66E,e->loc);});goto _LL0;}case 9U: _LLF: _tmp118=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp119=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp118;struct Cyc_Absyn_Exp*e2=_tmp119;
new_e=({struct Cyc_Absyn_Exp*_tmp671=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp670=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_seq_exp(_tmp671,_tmp670,e->loc);});goto _LL0;}case 6U: _LL11: _tmp115=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp116=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmp117=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp115;struct Cyc_Absyn_Exp*e2=_tmp116;struct Cyc_Absyn_Exp*e3=_tmp117;
# 480
new_e=({struct Cyc_Absyn_Exp*_tmp674=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp673=Cyc_Tcutil_deep_copy_exp(pt,e2);struct Cyc_Absyn_Exp*_tmp672=Cyc_Tcutil_deep_copy_exp(pt,e3);Cyc_Absyn_conditional_exp(_tmp674,_tmp673,_tmp672,e->loc);});goto _LL0;}case 4U: _LL13: _tmp112=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp113=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmp114=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp112;struct Cyc_Core_Opt*po=_tmp113;struct Cyc_Absyn_Exp*e2=_tmp114;
# 482
new_e=({struct Cyc_Absyn_Exp*_tmp677=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Core_Opt*_tmp676=(unsigned)po?({struct Cyc_Core_Opt*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->v=(void*)po->v;_tmp125;}): 0;struct Cyc_Absyn_Exp*_tmp675=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_assignop_exp(_tmp677,_tmp676,_tmp675,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f3 != 0){_LL15: _tmp109=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp10A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmp10B=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f3)->num_varargs;_tmp10C=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f3)->injectors;_tmp10D=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f3)->vai)->name;_tmp10E=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f3)->vai)->tq;_tmp10F=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f3)->vai)->type;_tmp110=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f3)->vai)->inject;_tmp111=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp109;struct Cyc_List_List*es=_tmp10A;int n=_tmp10B;struct Cyc_List_List*is=_tmp10C;struct _fat_ptr*nm=_tmp10D;struct Cyc_Absyn_Tqual tq=_tmp10E;void*t=_tmp10F;int i=_tmp110;int resolved=_tmp111;
# 485
new_e=({void*_tmp67D=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=10U,({
struct Cyc_Absyn_Exp*_tmp67C=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp128->f1=_tmp67C;}),({struct Cyc_List_List*_tmp67B=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp128->f2=_tmp67B;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp67A=({struct Cyc_Absyn_VarargCallInfo*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->num_varargs=n,_tmp127->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp679=({struct Cyc_Absyn_VarargInfo*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->name=nm,_tmp126->tq=tq,({void*_tmp678=Cyc_Tcutil_copy_type(t);_tmp126->type=_tmp678;}),_tmp126->inject=i;_tmp126;});_tmp127->vai=_tmp679;});_tmp127;});
# 487
_tmp128->f3=_tmp67A;}),_tmp128->f4=resolved;_tmp128;});
# 485
Cyc_Absyn_new_exp(_tmp67D,e->loc);});
# 490
goto _LL0;}}else{_LL17: _tmp105=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp106=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmp107=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_tmp108=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBB)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp105;struct Cyc_List_List*es=_tmp106;struct Cyc_Absyn_VarargCallInfo*vci=_tmp107;int resolved=_tmp108;
# 492
new_e=({void*_tmp680=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=10U,({struct Cyc_Absyn_Exp*_tmp67F=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp129->f1=_tmp67F;}),({struct Cyc_List_List*_tmp67E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp129->f2=_tmp67E;}),_tmp129->f3=vci,_tmp129->f4=resolved;_tmp129;});Cyc_Absyn_new_exp(_tmp680,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmp103=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp104=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp103;int b=_tmp104;
# 495
new_e=b?({struct Cyc_Absyn_Exp*_tmp682=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_rethrow_exp(_tmp682,e->loc);}):({struct Cyc_Absyn_Exp*_tmp681=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_throw_exp(_tmp681,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmp102=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp102;
# 498
new_e=({struct Cyc_Absyn_Exp*_tmp683=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp683,e->loc);});goto _LL0;}case 13U: _LL1D: _tmp100=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmp101=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp100;struct Cyc_List_List*ts=_tmp101;
# 500
new_e=({struct Cyc_Absyn_Exp*_tmp685=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_List_List*_tmp684=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp685,_tmp684,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpFC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpFD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmpFE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_tmpFF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBB)->f4;_LL20: {void*t=_tmpFC;struct Cyc_Absyn_Exp*e1=_tmpFD;int b=_tmpFE;enum Cyc_Absyn_Coercion c=_tmpFF;
# 502
new_e=({void*_tmp689=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp688=Cyc_Tcutil_deep_copy_exp(pt,e1);int _tmp687=b;enum Cyc_Absyn_Coercion _tmp686=c;Cyc_Absyn_cast_exp(_tmp689,_tmp688,_tmp687,_tmp686,e->loc);});goto _LL0;}case 16U: _LL21: _tmpFA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpFB=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpFA;struct Cyc_Absyn_Exp*e1=_tmpFB;
# 504
new_e=({struct Cyc_Absyn_Exp*_tmp68B=(unsigned)eo?Cyc_Tcutil_deep_copy_exp(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp68A=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_New_exp(_tmp68B,_tmp68A,e->loc);});goto _LL0;}case 15U: _LL23: _tmpF9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpF9;
new_e=({struct Cyc_Absyn_Exp*_tmp68C=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_address_exp(_tmp68C,e->loc);});goto _LL0;}case 17U: _LL25: _tmpF8=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL26: {void*t=_tmpF8;
new_e=({void*_tmp68D=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp68D,e->loc);});goto _LL0;}case 18U: _LL27: _tmpF7=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF7;
new_e=({struct Cyc_Absyn_Exp*_tmp68E=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp68E,e->loc);});goto _LL0;}case 19U: _LL29: _tmpF5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpF6=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL2A: {void*t=_tmpF5;struct Cyc_List_List*ofs=_tmpF6;
new_e=({void*_tmp690=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp68F=ofs;Cyc_Absyn_offsetof_exp(_tmp690,_tmp68F,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpF4=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpF4;
new_e=({struct Cyc_Absyn_Exp*_tmp691=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_deref_exp(_tmp691,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpF3=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpF3;
new_e=({struct Cyc_Absyn_Exp*_tmp692=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_extension_exp(_tmp692,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpF2=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL30: {struct Cyc_List_List*es=_tmpF2;
new_e=({struct Cyc_List_List*_tmp693=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_tuple_exp(_tmp693,e->loc);});goto _LL0;}case 21U: _LL31: _tmpEE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpEF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmpF0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_tmpF1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBB)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpEE;struct _fat_ptr*n=_tmpEF;int f1=_tmpF0;int f2=_tmpF1;
# 513
new_e=({void*_tmp695=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=21U,({struct Cyc_Absyn_Exp*_tmp694=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12A->f1=_tmp694;}),_tmp12A->f2=n,_tmp12A->f3=f1,_tmp12A->f4=f2;_tmp12A;});Cyc_Absyn_new_exp(_tmp695,e->loc);});goto _LL0;}case 22U: _LL33: _tmpEA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpEB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmpEC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_tmpED=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBB)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpEA;struct _fat_ptr*n=_tmpEB;int f1=_tmpEC;int f2=_tmpED;
# 515
new_e=({void*_tmp697=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=22U,({struct Cyc_Absyn_Exp*_tmp696=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12B->f1=_tmp696;}),_tmp12B->f2=n,_tmp12B->f3=f1,_tmp12B->f4=f2;_tmp12B;});Cyc_Absyn_new_exp(_tmp697,e->loc);});goto _LL0;}case 23U: _LL35: _tmpE8=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpE9=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpE8;struct Cyc_Absyn_Exp*e2=_tmpE9;
# 517
new_e=({struct Cyc_Absyn_Exp*_tmp699=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp698=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_subscript_exp(_tmp699,_tmp698,e->loc);});goto _LL0;}case 26U: _LL37: _tmpE7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL38: {struct Cyc_List_List*eds=_tmpE7;
# 519
new_e=({void*_tmp69B=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=26U,({struct Cyc_List_List*_tmp69A=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12C->f1=_tmp69A;});_tmp12C;});Cyc_Absyn_new_exp(_tmp69B,e->loc);});goto _LL0;}case 31U: _LL39: _tmpE4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpE5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmpE6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_LL3A: {struct Cyc_List_List*es=_tmpE4;struct Cyc_Absyn_Datatypedecl*dtd=_tmpE5;struct Cyc_Absyn_Datatypefield*dtf=_tmpE6;
# 521
new_e=({void*_tmp69D=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=31U,({struct Cyc_List_List*_tmp69C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp12D->f1=_tmp69C;}),_tmp12D->f2=dtd,_tmp12D->f3=dtf;_tmp12D;});Cyc_Absyn_new_exp(_tmp69D,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpE0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpE1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmpE2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_tmpE3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBB)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpE0;struct Cyc_Absyn_Exp*e1=_tmpE1;struct Cyc_Absyn_Exp*e2=_tmpE2;int b=_tmpE3;
# 523
new_e=({void*_tmp6A0=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=27U,_tmp12E->f1=vd,({struct Cyc_Absyn_Exp*_tmp69F=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12E->f2=_tmp69F;}),({struct Cyc_Absyn_Exp*_tmp69E=Cyc_Tcutil_deep_copy_exp(pt,e2);_tmp12E->f3=_tmp69E;}),_tmp12E->f4=b;_tmp12E;});Cyc_Absyn_new_exp(_tmp6A0,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpDD=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpDE=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmpDF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpDD;void*t=_tmpDE;int b=_tmpDF;
# 525
new_e=({void*_tmp6A3=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->tag=28U,({struct Cyc_Absyn_Exp*_tmp6A2=Cyc_Tcutil_deep_copy_exp(pt,e);_tmp12F->f1=_tmp6A2;}),({void*_tmp6A1=Cyc_Tcutil_copy_type(t);_tmp12F->f2=_tmp6A1;}),_tmp12F->f3=b;_tmp12F;});Cyc_Absyn_new_exp(_tmp6A3,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpD9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpDA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmpDB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_tmpDC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBB)->f4;_LL40: {struct _tuple1*n=_tmpD9;struct Cyc_List_List*ts=_tmpDA;struct Cyc_List_List*eds=_tmpDB;struct Cyc_Absyn_Aggrdecl*agr=_tmpDC;
# 528
new_e=({void*_tmp6A6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=29U,_tmp130->f1=n,({struct Cyc_List_List*_tmp6A5=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp130->f2=_tmp6A5;}),({struct Cyc_List_List*_tmp6A4=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp130->f3=_tmp6A4;}),_tmp130->f4=agr;_tmp130;});Cyc_Absyn_new_exp(_tmp6A6,e->loc);});
# 530
goto _LL0;}case 30U: _LL41: _tmpD7=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpD8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL42: {void*t=_tmpD7;struct Cyc_List_List*eds=_tmpD8;
# 532
new_e=({void*_tmp6A9=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=30U,({void*_tmp6A8=Cyc_Tcutil_copy_type(t);_tmp131->f1=_tmp6A8;}),({struct Cyc_List_List*_tmp6A7=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp131->f2=_tmp6A7;});_tmp131;});Cyc_Absyn_new_exp(_tmp6A9,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpD3=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBB)->f1)->f1;_tmpD4=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBB)->f1)->f2;_tmpD5=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBB)->f1)->f3;_tmpD6=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL44: {struct _fat_ptr*vopt=_tmpD3;struct Cyc_Absyn_Tqual tq=_tmpD4;void*t=_tmpD5;struct Cyc_List_List*eds=_tmpD6;
# 535
new_e=({void*_tmp6AD=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->tag=25U,({struct _tuple9*_tmp6AC=({struct _tuple9*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->f1=vopt,_tmp132->f2=tq,({void*_tmp6AB=Cyc_Tcutil_copy_type(t);_tmp132->f3=_tmp6AB;});_tmp132;});_tmp133->f1=_tmp6AC;}),({
struct Cyc_List_List*_tmp6AA=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp133->f2=_tmp6AA;});_tmp133;});
# 535
Cyc_Absyn_new_exp(_tmp6AD,e->loc);});
# 537
goto _LL0;}case 32U: _LL45: _tmpD1=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpD2=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpD1;struct Cyc_Absyn_Enumfield*ef=_tmpD2;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpCF=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpD0=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL48: {void*t=_tmpCF;struct Cyc_Absyn_Enumfield*ef=_tmpD0;
# 540
new_e=({void*_tmp6AF=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->tag=33U,({void*_tmp6AE=Cyc_Tcutil_copy_type(t);_tmp134->f1=_tmp6AE;}),_tmp134->f2=ef;_tmp134;});Cyc_Absyn_new_exp(_tmp6AF,e->loc);});goto _LL0;}case 34U: _LL49: _tmpC9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBB)->f1).is_calloc;_tmpCA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBB)->f1).rgn;_tmpCB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBB)->f1).elt_type;_tmpCC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBB)->f1).num_elts;_tmpCD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBB)->f1).fat_result;_tmpCE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBB)->f1).inline_call;_LL4A: {int ic=_tmpC9;struct Cyc_Absyn_Exp*r=_tmpCA;void**t=_tmpCB;struct Cyc_Absyn_Exp*n=_tmpCC;int res=_tmpCD;int inlc=_tmpCE;
# 542
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=Cyc_Tcutil_deep_copy_exp(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp135=_cycalloc(sizeof(*_tmp135));({void*_tmp6B0=Cyc_Tcutil_copy_type(*t);*_tmp135=_tmp6B0;});_tmp135;});
({void*_tmp6B1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->tag=34U,(_tmp136->f1).is_calloc=ic,(_tmp136->f1).rgn=r1,(_tmp136->f1).elt_type=t1,(_tmp136->f1).num_elts=n,(_tmp136->f1).fat_result=res,(_tmp136->f1).inline_call=inlc;_tmp136;});e2->r=_tmp6B1;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpC7=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpC8=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpC7;struct Cyc_Absyn_Exp*e2=_tmpC8;
new_e=({struct Cyc_Absyn_Exp*_tmp6B3=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6B2=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_swap_exp(_tmp6B3,_tmp6B2,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpC5=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpC6=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpC5;struct Cyc_List_List*eds=_tmpC6;
# 550
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->v=(struct _tuple1*)nopt->v;_tmp137;});
new_e=({void*_tmp6B5=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->tag=36U,_tmp138->f1=nopt1,({struct Cyc_List_List*_tmp6B4=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp138->f2=_tmp6B4;});_tmp138;});Cyc_Absyn_new_exp(_tmp6B5,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 555
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6B6=({const char*_tmp139="deep_copy: statement expressions unsupported";_tag_fat(_tmp139,sizeof(char),45U);});_tmp13A->f1=_tmp6B6;});_tmp13A;}));case 38U: _LL51: _tmpC3=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpC4=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpC3;struct _fat_ptr*fn=_tmpC4;
new_e=({void*_tmp6B8=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp13B=_cycalloc(sizeof(*_tmp13B));_tmp13B->tag=38U,({struct Cyc_Absyn_Exp*_tmp6B7=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp13B->f1=_tmp6B7;}),_tmp13B->f2=fn;_tmp13B;});Cyc_Absyn_new_exp(_tmp6B8,e->loc);});
goto _LL0;}case 39U: _LL53: _tmpC2=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL54: {void*t=_tmpC2;
new_e=({void*_tmp6B9=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp6B9,e->loc);});
goto _LL0;}case 40U: _LL55: _tmpBD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_tmpBE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBB)->f2;_tmpBF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBB)->f3;_tmpC0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBB)->f4;_tmpC1=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBB)->f5;_LL56: {int v=_tmpBD;struct _fat_ptr t=_tmpBE;struct Cyc_List_List*o=_tmpBF;struct Cyc_List_List*i=_tmpC0;struct Cyc_List_List*c=_tmpC1;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _LL57: _tmpBC=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpBB)->f1;_LL58: {struct Cyc_Absyn_Exp*e1=_tmpBC;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 564
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 568
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 579 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp13C=ka1;enum Cyc_Absyn_AliasQual _tmp13E;enum Cyc_Absyn_KindQual _tmp13D;_LL1: _tmp13D=_tmp13C->kind;_tmp13E=_tmp13C->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp13D;enum Cyc_Absyn_AliasQual a1=_tmp13E;
struct Cyc_Absyn_Kind*_tmp13F=ka2;enum Cyc_Absyn_AliasQual _tmp141;enum Cyc_Absyn_KindQual _tmp140;_LL4: _tmp140=_tmp13F->kind;_tmp141=_tmp13F->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp140;enum Cyc_Absyn_AliasQual a2=_tmp141;
# 583
if((int)k1 != (int)k2){
struct _tuple17 _tmp142=({struct _tuple17 _tmp603;_tmp603.f1=k1,_tmp603.f2=k2;_tmp603;});struct _tuple17 _stmttmp1B=_tmp142;struct _tuple17 _tmp143=_stmttmp1B;switch(_tmp143.f1){case Cyc_Absyn_BoxKind: switch(_tmp143.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp143.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 591
if((int)a1 != (int)a2){
struct _tuple18 _tmp144=({struct _tuple18 _tmp604;_tmp604.f1=a1,_tmp604.f2=a2;_tmp604;});struct _tuple18 _stmttmp1C=_tmp144;struct _tuple18 _tmp145=_stmttmp1C;switch(_tmp145.f1){case Cyc_Absyn_Aliasable: if(_tmp145.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp145.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 597
return 1;}}}
# 600
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp146=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1D=_tmp146;void*_tmp147=_stmttmp1D;struct Cyc_Absyn_Kind*_tmp148;struct Cyc_Absyn_Kind*_tmp149;switch(*((int*)_tmp147)){case 0U: _LL1: _tmp149=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp147)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp149;
return k;}case 2U: _LL3: _tmp148=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp147)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp148;
return k;}default: _LL5: _LL6:
({void*_tmp6BA=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp14A=_cycalloc(sizeof(*_tmp14A));_tmp14A->tag=2U,_tmp14A->f1=0,_tmp14A->f2=def;_tmp14A;});tv->kind=_tmp6BA;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 608
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp14B=Cyc_Tcutil_compress(t);void*_stmttmp1E=_tmp14B;void*_tmp14C=_stmttmp1E;struct Cyc_Absyn_Tvar*_tmp14D;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14C)->tag == 2U){_LL1: _tmp14D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14C)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp14D;
# 611
void*_tmp14E=tv->kind;void*oldkb=_tmp14E;
tv->kind=kb;
return({struct _tuple19 _tmp605;_tmp605.f1=tv,_tmp605.f2=oldkb;_tmp605;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp151=({struct Cyc_String_pa_PrintArg_struct _tmp606;_tmp606.tag=0U,({struct _fat_ptr _tmp6BB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp606.f1=_tmp6BB;});_tmp606;});void*_tmp14F[1U];_tmp14F[0]=& _tmp151;({struct _fat_ptr _tmp6BC=({const char*_tmp150="swap_kind: cannot update the kind of %s";_tag_fat(_tmp150,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6BC,_tag_fat(_tmp14F,sizeof(void*),1U));});});}_LL0:;}
# 620
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 622
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 626
struct _RegionHandle _tmp152=_new_region("temp");struct _RegionHandle*temp=& _tmp152;_push_region(temp);
{struct Cyc_List_List*_tmp153=0;struct Cyc_List_List*inst=_tmp153;
# 629
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp154=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp154;
void*_tmp155=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp155;
struct Cyc_Absyn_Kind*_tmp156=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp1F=_tmp156;struct Cyc_Absyn_Kind*_tmp157=_stmttmp1F;switch(((struct Cyc_Absyn_Kind*)_tmp157)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 635
 inst=({struct Cyc_List_List*_tmp159=_region_malloc(temp,sizeof(*_tmp159));({struct _tuple19*_tmp6BD=({struct _tuple19*_tmp158=_region_malloc(temp,sizeof(*_tmp158));_tmp158->f1=tv,_tmp158->f2=t;_tmp158;});_tmp159->hd=_tmp6BD;}),_tmp159->tl=inst;_tmp159;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 639
if(inst != 0){
field_type=({struct _RegionHandle*_tmp6BF=temp;struct Cyc_List_List*_tmp6BE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp6BF,_tmp6BE,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 627
;_pop_region();}
# 644
return k;}
# 651
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 653
void*_tmp15A=Cyc_Tcutil_compress(t);void*_stmttmp20=_tmp15A;void*_tmp15B=_stmttmp20;struct Cyc_Absyn_Typedefdecl*_tmp15C;struct Cyc_Absyn_Exp*_tmp15D;struct Cyc_Absyn_PtrInfo _tmp15E;struct Cyc_List_List*_tmp160;void*_tmp15F;struct Cyc_Absyn_Tvar*_tmp161;struct Cyc_Core_Opt*_tmp162;switch(*((int*)_tmp15B)){case 1U: _LL1: _tmp162=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15B)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp162;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp161=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp15B)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp161;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp15F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f1;_tmp160=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15B)->f2;_LL6: {void*c=_tmp15F;struct Cyc_List_List*ts=_tmp160;
# 657
void*_tmp163=c;int _tmp167;struct Cyc_Absyn_AggrdeclImpl*_tmp166;struct Cyc_List_List*_tmp165;enum Cyc_Absyn_AggrKind _tmp164;struct Cyc_Absyn_Kind*_tmp168;enum Cyc_Absyn_Size_of _tmp169;switch(*((int*)_tmp163)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp169=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp163)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp169;
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp168=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp163)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp168;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp163)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 678
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 680
({void*_tmp16A=0U;({struct _fat_ptr _tmp6C0=({const char*_tmp16B="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp16B,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C0,_tag_fat(_tmp16A,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp163)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 684
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp164=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp163)->f1).KnownAggr).val)->kind;_tmp165=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp163)->f1).KnownAggr).val)->tvs;_tmp166=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp163)->f1).KnownAggr).val)->impl;_tmp167=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp163)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp164;struct Cyc_List_List*tvs=_tmp165;struct Cyc_Absyn_AggrdeclImpl*i=_tmp166;int expected_mem_kind=_tmp167;
# 686
if(i == 0){
if(expected_mem_kind)
return& Cyc_Tcutil_mk;else{
# 690
return& Cyc_Tcutil_ak;}}{
# 692
struct Cyc_List_List*_tmp16C=i->fields;struct Cyc_List_List*fields=_tmp16C;
if(fields == 0)return& Cyc_Tcutil_mk;
# 695
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){;}{
void*_tmp16D=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp16D;
struct Cyc_Absyn_Kind*_tmp16E=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp16E;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 703
for(0;fields != 0;fields=fields->tl){
void*_tmp16F=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp16F;
struct Cyc_Absyn_Kind*_tmp170=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp170;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 709
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 711
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp15E=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15B)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp15E;
# 714
void*_tmp171=Cyc_Tcutil_compress((pinfo.ptr_atts).bounds);void*_stmttmp21=_tmp171;void*_tmp172=_stmttmp21;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp172)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp172)->f1)){case 13U: _LL4D: _LL4E: {
# 716
enum Cyc_Absyn_AliasQual _tmp173=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp22=_tmp173;enum Cyc_Absyn_AliasQual _tmp174=_stmttmp22;switch(_tmp174){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 722
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 724
enum Cyc_Absyn_AliasQual _tmp175=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp23=_tmp175;enum Cyc_Absyn_AliasQual _tmp176=_stmttmp23;switch(_tmp176){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 730
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 734
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp15D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15B)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp15D;
# 736
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp15C=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15B)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp15C;
# 740
if(td == 0 || td->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp179=({struct Cyc_String_pa_PrintArg_struct _tmp607;_tmp607.tag=0U,({struct _fat_ptr _tmp6C1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp607.f1=_tmp6C1;});_tmp607;});void*_tmp177[1U];_tmp177[0]=& _tmp179;({struct _fat_ptr _tmp6C2=({const char*_tmp178="type_kind: typedef found: %s";_tag_fat(_tmp178,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C2,_tag_fat(_tmp177,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15B)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 749
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 753
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 761
int Cyc_Tcutil_is_regparm0_att(void*a){
void*_tmp17A=a;if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp17A)->tag == 0U){if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp17A)->f1 == 0){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 768
int Cyc_Tcutil_equiv_fn_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
return 1;}
# 777
static void*Cyc_Tcutil_rgns_of(void*t);
# 779
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 783
static struct _tuple19*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp17B=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp24=_tmp17B;struct Cyc_Absyn_Kind*_tmp17C=_stmttmp24;switch(((struct Cyc_Absyn_Kind*)_tmp17C)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp17C)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp17D=_cycalloc(sizeof(*_tmp17D));_tmp17D->tag=9U,({struct Cyc_Absyn_Exp*_tmp6C3=Cyc_Absyn_uint_exp(0U,0U);_tmp17D->f1=_tmp6C3;});_tmp17D;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 794
return({struct _tuple19*_tmp17E=_cycalloc(sizeof(*_tmp17E));_tmp17E->f1=tv,_tmp17E->f2=t;_tmp17E;});}
# 801
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp17F=Cyc_Tcutil_compress(t);void*_stmttmp25=_tmp17F;void*_tmp180=_stmttmp25;struct Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp182;struct Cyc_List_List*_tmp189;struct Cyc_Absyn_VarargInfo*_tmp188;struct Cyc_List_List*_tmp187;void*_tmp186;struct Cyc_Absyn_Tqual _tmp185;void*_tmp184;struct Cyc_List_List*_tmp183;struct Cyc_List_List*_tmp18A;void*_tmp18B;void*_tmp18D;void*_tmp18C;struct Cyc_List_List*_tmp18E;switch(*((int*)_tmp180)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp180)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp180)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp18E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp180)->f2;_LL6: {struct Cyc_List_List*ts=_tmp18E;
# 806
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 810
struct Cyc_Absyn_Kind*_tmp18F=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp26=_tmp18F;struct Cyc_Absyn_Kind*_tmp190=_stmttmp26;switch(((struct Cyc_Absyn_Kind*)_tmp190)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp18C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp180)->f1).elt_type;_tmp18D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp180)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp18C;void*r=_tmp18D;
# 818
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp191[2U];({void*_tmp6C5=Cyc_Absyn_access_eff(r);_tmp191[0]=_tmp6C5;}),({void*_tmp6C4=Cyc_Tcutil_rgns_of(et);_tmp191[1]=_tmp6C4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp191,sizeof(void*),2U));})));}case 4U: _LLD: _tmp18B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp180)->f1).elt_type;_LLE: {void*et=_tmp18B;
# 820
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp18A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp180)->f2;_LL10: {struct Cyc_List_List*fs=_tmp18A;
# 822
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp183=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp180)->f1).tvars;_tmp184=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp180)->f1).effect;_tmp185=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp180)->f1).ret_tqual;_tmp186=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp180)->f1).ret_type;_tmp187=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp180)->f1).args;_tmp188=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp180)->f1).cyc_varargs;_tmp189=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp180)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp183;void*eff=_tmp184;struct Cyc_Absyn_Tqual rt_tq=_tmp185;void*rt=_tmp186;struct Cyc_List_List*args=_tmp187;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp188;struct Cyc_List_List*rpo=_tmp189;
# 831
void*_tmp192=({struct Cyc_List_List*_tmp6C6=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp6C6,(void*)_check_null(eff));});void*e=_tmp192;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp182=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp180)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp182;
# 834
struct Cyc_List_List*_tmp193=0;struct Cyc_List_List*ts=_tmp193;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp194=_cycalloc(sizeof(*_tmp194));_tmp194->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp194->tl=ts;_tmp194;});}
_tmp181=ts;goto _LL16;}case 8U: _LL15: _tmp181=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp180)->f2;_LL16: {struct Cyc_List_List*ts=_tmp181;
# 839
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({void*_tmp195=0U;({struct _fat_ptr _tmp6C7=({const char*_tmp196="typedecl in rgns_of";_tag_fat(_tmp196,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C7,_tag_fat(_tmp195,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 849
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp197=e;void*_tmp198;struct Cyc_List_List**_tmp199;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f1)){case 9U: _LL1: _tmp199=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2;_LL2: {struct Cyc_List_List**es=_tmp199;
# 853
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp19A=(void*)effs->hd;void*eff=_tmp19A;
({void*_tmp6C8=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp6C8;});{
void*_tmp19B=(void*)effs->hd;void*_stmttmp27=_tmp19B;void*_tmp19C=_stmttmp27;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->tl == 0){_LLE: _LLF:
# 862
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 866
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp19D=Cyc_Tcutil_compress((void*)effs->hd);void*_stmttmp28=_tmp19D;void*_tmp19E=_stmttmp28;void*_tmp19F;struct Cyc_List_List*_tmp1A0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f1)){case 9U: _LL13: _tmp1A0=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp1A0;
# 871
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);
goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp19F=_tmp19E;_LL1C: {void*e=_tmp19F;
effects=({struct Cyc_List_List*_tmp1A1=_cycalloc(sizeof(*_tmp1A1));_tmp1A1->hd=e,_tmp1A1->tl=effects;_tmp1A1;});goto _LL12;}}_LL12:;}}
# 879
({struct Cyc_List_List*_tmp6C9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp6C9;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2 != 0){_LL3: _tmp198=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->hd;_LL4: {void*t=_tmp198;
# 882
void*_tmp1A2=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp1A2;void*_tmp1A3=_stmttmp29;switch(*((int*)_tmp1A3)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 887
 return e;}_LL0:;}}
# 892
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A4=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A5=_cycalloc(sizeof(*_tmp1A5));_tmp1A5->tag=5U,(_tmp1A5->f1).tvars=0,(_tmp1A5->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6CA=Cyc_Absyn_empty_tqual(0U);(_tmp1A5->f1).ret_tqual=_tmp6CA;}),(_tmp1A5->f1).ret_type=Cyc_Absyn_void_type,(_tmp1A5->f1).args=0,(_tmp1A5->f1).c_varargs=0,(_tmp1A5->f1).cyc_varargs=0,(_tmp1A5->f1).rgn_po=0,(_tmp1A5->f1).attributes=0,(_tmp1A5->f1).requires_clause=0,(_tmp1A5->f1).requires_relns=0,(_tmp1A5->f1).ensures_clause=0,(_tmp1A5->f1).ensures_relns=0,(_tmp1A5->f1).return_value=0;_tmp1A5;});
# 893
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp1A4;
# 903
return({void*_tmp6CE=(void*)fntype;void*_tmp6CD=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6CC=Cyc_Absyn_empty_tqual(0U);void*_tmp6CB=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp6CE,_tmp6CD,_tmp6CC,_tmp6CB,Cyc_Absyn_false_type);});}
# 910
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp1A6=Cyc_Tcutil_compress(e);void*_stmttmp2A=_tmp1A6;void*_tmp1A7=_stmttmp2A;struct Cyc_Core_Opt*_tmp1AA;void**_tmp1A9;struct Cyc_Core_Opt*_tmp1A8;void*_tmp1AB;struct Cyc_List_List*_tmp1AC;void*_tmp1AD;switch(*((int*)_tmp1A7)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f2 != 0){_LL1: _tmp1AD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f2)->hd;_LL2: {void*r2=_tmp1AD;
# 917
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Tcutil_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp1AE=({struct _tuple15 _tmp608;_tmp608.f1=r,_tmp608.f2=r2;_tmp608;});struct _tuple15 _stmttmp2B=_tmp1AE;struct _tuple15 _tmp1AF=_stmttmp2B;struct Cyc_Absyn_Tvar*_tmp1B1;struct Cyc_Absyn_Tvar*_tmp1B0;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AF.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AF.f2)->tag == 2U){_LLC: _tmp1B0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AF.f1)->f1;_tmp1B1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AF.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1B0;struct Cyc_Absyn_Tvar*tv2=_tmp1B1;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1AC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f2;_LL4: {struct Cyc_List_List*es=_tmp1AC;
# 925
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f2 != 0){_LL5: _tmp1AB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A7)->f2)->hd;_LL6: {void*t=_tmp1AB;
# 929
void*_tmp1B2=Cyc_Tcutil_rgns_of(t);void*_stmttmp2C=_tmp1B2;void*_tmp1B3=_stmttmp2C;void*_tmp1B4;void*_tmp1B5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B3)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B3)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B3)->f2 != 0){_LL11: _tmp1B5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B3)->f2)->hd;_LL12: {void*t=_tmp1B5;
# 931
if(!constrain)return 0;{
void*_tmp1B6=Cyc_Tcutil_compress(t);void*_stmttmp2D=_tmp1B6;void*_tmp1B7=_stmttmp2D;struct Cyc_Core_Opt*_tmp1BA;void**_tmp1B9;struct Cyc_Core_Opt*_tmp1B8;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->tag == 1U){_LL16: _tmp1B8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->f1;_tmp1B9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->f2;_tmp1BA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B7)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1B8;void**p=_tmp1B9;struct Cyc_Core_Opt*s=_tmp1BA;
# 936
void*_tmp1BB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1BB;
# 939
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp6D0=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1BC[2U];_tmp1BC[0]=ev,({void*_tmp6CF=Cyc_Absyn_access_eff(r);_tmp1BC[1]=_tmp6CF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BC,sizeof(void*),2U));})));*p=_tmp6D0;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1B4=_tmp1B3;_LL14: {void*e2=_tmp1B4;
# 944
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1A8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A7)->f1;_tmp1A9=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A7)->f2;_tmp1AA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A7)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1A8;void**p=_tmp1A9;struct Cyc_Core_Opt*s=_tmp1AA;
# 947
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1BD=0U;({struct _fat_ptr _tmp6D1=({const char*_tmp1BE="effect evar has wrong kind";_tag_fat(_tmp1BE,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D1,_tag_fat(_tmp1BD,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 952
void*_tmp1BF=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1BF;
# 955
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);{
void*_tmp1C0=Cyc_Absyn_join_eff(({void*_tmp1C1[2U];_tmp1C1[0]=ev,({void*_tmp6D2=Cyc_Absyn_access_eff(r);_tmp1C1[1]=_tmp6D2;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C1,sizeof(void*),2U));}));void*new_typ=_tmp1C0;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 966
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1C2=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_stmttmp2E=_tmp1C2;void*_tmp1C3=_stmttmp2E;struct Cyc_Core_Opt*_tmp1C6;void**_tmp1C5;struct Cyc_Core_Opt*_tmp1C4;void*_tmp1C7;struct Cyc_List_List*_tmp1C8;switch(*((int*)_tmp1C3)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1C8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f2;_LL4: {struct Cyc_List_List*es=_tmp1C8;
# 971
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f2 != 0){_LL5: _tmp1C7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C3)->f2)->hd;_LL6: {void*t2=_tmp1C7;
# 976
t2=Cyc_Tcutil_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1C9=Cyc_Tcutil_rgns_of(t);void*_stmttmp2F=_tmp1C9;void*_tmp1CA=_stmttmp2F;void*_tmp1CB;void*_tmp1CC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CA)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CA)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CA)->f2 != 0){_LLC: _tmp1CC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CA)->f2)->hd;_LLD: {void*t3=_tmp1CC;
# 981
struct _tuple15 _tmp1CD=({struct _tuple15 _tmp609;({void*_tmp6D3=Cyc_Tcutil_compress(t3);_tmp609.f1=_tmp6D3;}),_tmp609.f2=t2;_tmp609;});struct _tuple15 _stmttmp30=_tmp1CD;struct _tuple15 _tmp1CE=_stmttmp30;struct Cyc_Absyn_Tvar*_tmp1D0;struct Cyc_Absyn_Tvar*_tmp1CF;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CE.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CE.f2)->tag == 2U){_LL11: _tmp1CF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CE.f1)->f1;_tmp1D0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CE.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1CF;struct Cyc_Absyn_Tvar*tv2=_tmp1D0;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1CB=_tmp1CA;_LLF: {void*e2=_tmp1CB;
# 985
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1C4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C3)->f1;_tmp1C5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C3)->f2;_tmp1C6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C3)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1C4;void**p=_tmp1C5;struct Cyc_Core_Opt*s=_tmp1C6;
# 988
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1D1=0U;({struct _fat_ptr _tmp6D4=({const char*_tmp1D2="effect evar has wrong kind";_tag_fat(_tmp1D2,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D4,_tag_fat(_tmp1D1,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 993
void*_tmp1D3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1D3;
# 996
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1D4=Cyc_Absyn_join_eff(({void*_tmp1D5[2U];_tmp1D5[0]=ev,({void*_tmp6D5=Cyc_Absyn_regionsof_eff(t);_tmp1D5[1]=_tmp6D5;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D5,sizeof(void*),2U));}));void*new_typ=_tmp1D4;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1007
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1D6=e;struct Cyc_Core_Opt*_tmp1D9;void**_tmp1D8;struct Cyc_Core_Opt*_tmp1D7;void*_tmp1DA;struct Cyc_List_List*_tmp1DB;struct Cyc_Absyn_Tvar*_tmp1DC;switch(*((int*)_tmp1D6)){case 2U: _LL1: _tmp1DC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D6)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1DC;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D6)->f1)){case 9U: _LL3: _tmp1DB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D6)->f2;_LL4: {struct Cyc_List_List*es=_tmp1DB;
# 1012
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D6)->f2 != 0){_LL5: _tmp1DA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D6)->f2)->hd;_LL6: {void*t=_tmp1DA;
# 1017
void*_tmp1DD=Cyc_Tcutil_rgns_of(t);void*_stmttmp31=_tmp1DD;void*_tmp1DE=_stmttmp31;void*_tmp1DF;void*_tmp1E0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->f2 != 0){_LLC: _tmp1E0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DE)->f2)->hd;_LLD: {void*t2=_tmp1E0;
# 1019
if(!may_constrain_evars)return 0;{
void*_tmp1E1=Cyc_Tcutil_compress(t2);void*_stmttmp32=_tmp1E1;void*_tmp1E2=_stmttmp32;struct Cyc_Core_Opt*_tmp1E5;void**_tmp1E4;struct Cyc_Core_Opt*_tmp1E3;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->tag == 1U){_LL11: _tmp1E3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->f1;_tmp1E4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->f2;_tmp1E5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E2)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1E3;void**p=_tmp1E4;struct Cyc_Core_Opt*s=_tmp1E5;
# 1025
void*_tmp1E6=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E6;
# 1027
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp6D7=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1E7[2U];_tmp1E7[0]=ev,({void*_tmp6D6=Cyc_Absyn_var_type(v);_tmp1E7[1]=_tmp6D6;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E7,sizeof(void*),2U));})));*p=_tmp6D7;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1DF=_tmp1DE;_LLF: {void*e2=_tmp1DF;
# 1032
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1D7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D6)->f1;_tmp1D8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D6)->f2;_tmp1D9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D6)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1D7;void**p=_tmp1D8;struct Cyc_Core_Opt*s=_tmp1D9;
# 1035
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1E8=0U;({struct _fat_ptr _tmp6D8=({const char*_tmp1E9="effect evar has wrong kind";_tag_fat(_tmp1E9,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D8,_tag_fat(_tmp1E8,sizeof(void*),0U));});});{
# 1039
void*_tmp1EA=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1EA;
# 1041
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1EB=Cyc_Absyn_join_eff(({void*_tmp1EC[2U];_tmp1EC[0]=ev,({void*_tmp6D9=Cyc_Absyn_var_type(v);_tmp1EC[1]=_tmp6D9;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1EC,sizeof(void*),2U));}));void*new_typ=_tmp1EB;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1051
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1ED=e;void*_tmp1EE;struct Cyc_List_List*_tmp1EF;switch(*((int*)_tmp1ED)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1ED)->f1)){case 9U: _LL1: _tmp1EF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1ED)->f2;_LL2: {struct Cyc_List_List*es=_tmp1EF;
# 1055
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1ED)->f2 != 0){_LL3: _tmp1EE=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1ED)->f2)->hd;_LL4: {void*t=_tmp1EE;
# 1060
void*_tmp1F0=Cyc_Tcutil_rgns_of(t);void*_stmttmp33=_tmp1F0;void*_tmp1F1=_stmttmp33;void*_tmp1F2;void*_tmp1F3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F1)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F1)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F1)->f2 != 0){_LLA: _tmp1F3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F1)->f2)->hd;_LLB: {void*t2=_tmp1F3;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1F2=_tmp1F1;_LLD: {void*e2=_tmp1F2;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1064
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 1078 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1083
void*_tmp1F4=Cyc_Tcutil_compress(e1);void*_stmttmp34=_tmp1F4;void*_tmp1F5=_stmttmp34;struct Cyc_Core_Opt*_tmp1F7;void**_tmp1F6;struct Cyc_Absyn_Tvar*_tmp1F8;void*_tmp1F9;void*_tmp1FA;struct Cyc_List_List*_tmp1FB;switch(*((int*)_tmp1F5)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f1)){case 9U: _LL1: _tmp1FB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2;_LL2: {struct Cyc_List_List*es=_tmp1FB;
# 1085
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2 != 0){_LL3: _tmp1FA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2)->hd;_LL4: {void*r=_tmp1FA;
# 1096
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2 != 0){_LL7: _tmp1F9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2)->hd;_LL8: {void*t=_tmp1F9;
# 1100
void*_tmp1FC=Cyc_Tcutil_rgns_of(t);void*_stmttmp35=_tmp1FC;void*_tmp1FD=_stmttmp35;void*_tmp1FE;void*_tmp1FF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f2 != 0){_LLE: _tmp1FF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f2)->hd;_LLF: {void*t2=_tmp1FF;
# 1105
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1FE=_tmp1FD;_LL11: {void*e=_tmp1FE;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1F8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F5)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1F8;
# 1098
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1F6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F5)->f2;_tmp1F7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F5)->f4;_LLA: {void**p=_tmp1F6;struct Cyc_Core_Opt*s=_tmp1F7;
# 1110
if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1114
*p=Cyc_Absyn_empty_effect;
# 1117
return 1;}else{
# 1119
return 0;}}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp202=({struct Cyc_String_pa_PrintArg_struct _tmp60A;_tmp60A.tag=0U,({struct _fat_ptr _tmp6DA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp60A.f1=_tmp6DA;});_tmp60A;});void*_tmp200[1U];_tmp200[0]=& _tmp202;({struct _fat_ptr _tmp6DB=({const char*_tmp201="subset_effect: bad effect: %s";_tag_fat(_tmp201,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DB,_tag_fat(_tmp200,sizeof(void*),1U));});});}_LL0:;}
# 1130
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1132
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp203=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp36=_tmp203;struct _tuple15*_tmp204=_stmttmp36;void*_tmp206;void*_tmp205;_LL1: _tmp205=_tmp204->f1;_tmp206=_tmp204->f2;_LL2: {void*t1a=_tmp205;void*t1b=_tmp206;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp207=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp37=_tmp207;struct _tuple15*_tmp208=_stmttmp37;void*_tmp20A;void*_tmp209;_LL4: _tmp209=_tmp208->f1;_tmp20A=_tmp208->f2;_LL5: {void*t2a=_tmp209;void*t2b=_tmp20A;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1142
if(!found)return 0;}}}
# 1144
return 1;}
# 1150
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1152
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1155
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp20B=t;switch(*((int*)_tmp20B)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20B)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20B)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1161
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1164
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1167
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1170
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20B)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1159
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1162
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1165
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1168
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1171
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20B)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1160
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1163
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1166
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1169
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1172
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1194
({void*_tmp20C=0U;({struct _fat_ptr _tmp6DC=({const char*_tmp20D="bad con";_tag_fat(_tmp20D,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DC,_tag_fat(_tmp20C,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp20B)->f1){case 0U: _LL21: _LL22:
# 1173
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
# 1197
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp20E=t;void*_tmp20F;switch(*((int*)_tmp20E)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp20F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20E)->f1;_LL18: {void*c=_tmp20F;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1213
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp210=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp210;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1218
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp211=i;struct _tuple1*_tmp212;struct Cyc_Absyn_Datatypedecl*_tmp213;if((_tmp211.KnownDatatype).tag == 2){_LL1: _tmp213=*(_tmp211.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp213;
return dd->name;}}else{_LL3: _tmp212=((_tmp211.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp212;
return n;}}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1224
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp214=i;struct _tuple1*_tmp216;struct _tuple1*_tmp215;struct Cyc_Absyn_Datatypefield*_tmp218;struct Cyc_Absyn_Datatypedecl*_tmp217;if((_tmp214.KnownDatatypefield).tag == 2){_LL1: _tmp217=((_tmp214.KnownDatatypefield).val).f1;_tmp218=((_tmp214.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp217;struct Cyc_Absyn_Datatypefield*df=_tmp218;
# 1227
return({struct _tuple20 _tmp60B;_tmp60B.f1=dd->name,_tmp60B.f2=df->name;_tmp60B;});}}else{_LL3: _tmp215=((_tmp214.UnknownDatatypefield).val).datatype_name;_tmp216=((_tmp214.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp215;struct _tuple1*f=_tmp216;
# 1229
return({struct _tuple20 _tmp60C;_tmp60C.f1=d,_tmp60C.f2=f;_tmp60C;});}}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1232
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp219=i;struct Cyc_Absyn_Aggrdecl*_tmp21A;struct _tuple1*_tmp21C;enum Cyc_Absyn_AggrKind _tmp21B;if((_tmp219.UnknownAggr).tag == 1){_LL1: _tmp21B=((_tmp219.UnknownAggr).val).f1;_tmp21C=((_tmp219.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp21B;struct _tuple1*n=_tmp21C;
return({struct _tuple21 _tmp60D;_tmp60D.f1=k,_tmp60D.f2=n;_tmp60D;});}}else{_LL3: _tmp21A=*(_tmp219.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp21A;
return({struct _tuple21 _tmp60E;_tmp60E.f1=ad->kind,_tmp60E.f2=ad->name;_tmp60E;});}}_LL0:;}
# 1238
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1244
struct _tuple15 _tmp21D=({struct _tuple15 _tmp60F;_tmp60F.f1=t1,_tmp60F.f2=t2;_tmp60F;});struct _tuple15 _stmttmp38=_tmp21D;struct _tuple15 _tmp21E=_stmttmp38;union Cyc_Absyn_AggrInfo _tmp220;union Cyc_Absyn_AggrInfo _tmp21F;union Cyc_Absyn_DatatypeFieldInfo _tmp222;union Cyc_Absyn_DatatypeFieldInfo _tmp221;union Cyc_Absyn_DatatypeInfo _tmp224;union Cyc_Absyn_DatatypeInfo _tmp223;struct Cyc_List_List*_tmp226;struct Cyc_List_List*_tmp225;struct _fat_ptr _tmp228;struct _fat_ptr _tmp227;struct _tuple1*_tmp22A;struct _tuple1*_tmp229;switch(*((int*)_tmp21E.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21E.f2)->tag == 15U){_LL1: _tmp229=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21E.f1)->f1;_tmp22A=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21E.f2)->f1;_LL2: {struct _tuple1*n1=_tmp229;struct _tuple1*n2=_tmp22A;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21E.f2)->tag == 17U){_LL3: _tmp227=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21E.f1)->f1;_tmp228=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21E.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp227;struct _fat_ptr s2=_tmp228;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21E.f2)->tag == 16U){_LL5: _tmp225=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21E.f1)->f1;_tmp226=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21E.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp225;struct Cyc_List_List*fs2=_tmp226;
# 1248
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21E.f2)->tag == 18U){_LL7: _tmp223=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21E.f1)->f1;_tmp224=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21E.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp223;union Cyc_Absyn_DatatypeInfo info2=_tmp224;
# 1250
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(info1);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(info2);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21E.f2)->tag == 19U){_LL9: _tmp221=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21E.f1)->f1;_tmp222=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21E.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp221;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp222;
# 1254
struct _tuple20 _tmp22B=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple20 _stmttmp39=_tmp22B;struct _tuple20 _tmp22C=_stmttmp39;struct _tuple1*_tmp22E;struct _tuple1*_tmp22D;_LL10: _tmp22D=_tmp22C.f1;_tmp22E=_tmp22C.f2;_LL11: {struct _tuple1*d1=_tmp22D;struct _tuple1*f1=_tmp22E;
struct _tuple20 _tmp22F=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple20 _stmttmp3A=_tmp22F;struct _tuple20 _tmp230=_stmttmp3A;struct _tuple1*_tmp232;struct _tuple1*_tmp231;_LL13: _tmp231=_tmp230.f1;_tmp232=_tmp230.f2;_LL14: {struct _tuple1*d2=_tmp231;struct _tuple1*f2=_tmp232;
int _tmp233=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp233;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21E.f2)->tag == 20U){_LLB: _tmp21F=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21E.f1)->f1;_tmp220=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21E.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp21F;union Cyc_Absyn_AggrInfo info2=_tmp220;
# 1260
struct _tuple21 _tmp234=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple21 _stmttmp3B=_tmp234;struct _tuple21 _tmp235=_stmttmp3B;struct _tuple1*_tmp237;enum Cyc_Absyn_AggrKind _tmp236;_LL16: _tmp236=_tmp235.f1;_tmp237=_tmp235.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp236;struct _tuple1*q1=_tmp237;
struct _tuple21 _tmp238=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple21 _stmttmp3C=_tmp238;struct _tuple21 _tmp239=_stmttmp3C;struct _tuple1*_tmp23B;enum Cyc_Absyn_AggrKind _tmp23A;_LL19: _tmp23A=_tmp239.f1;_tmp23B=_tmp239.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp23A;struct _tuple1*q2=_tmp23B;
int _tmp23C=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp23C;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1269
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6DE)(void*,void*)=cmp;void*_tmp6DD=(void*)_check_null(a1);_tmp6DE(_tmp6DD,(void*)_check_null(a2));});}
# 1275
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp23D=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp23D;
int _tmp23E=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp23E;
return Cyc_Core_intcmp(i1,i2);}
# 1280
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp23F=tqt1;void*_tmp241;struct Cyc_Absyn_Tqual _tmp240;_LL1: _tmp240=_tmp23F->f1;_tmp241=_tmp23F->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp240;void*t1=_tmp241;
struct _tuple12*_tmp242=tqt2;void*_tmp244;struct Cyc_Absyn_Tqual _tmp243;_LL4: _tmp243=_tmp242->f1;_tmp244=_tmp242->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp243;void*t2=_tmp244;
int _tmp245=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp245;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1288
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp246=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp246;
if(zsc != 0)return zsc;{
int _tmp247=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp247;
if(tqc != 0)return tqc;{
int _tmp248=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp248;
if(tc != 0)return tc;{
int _tmp249=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp249;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1305
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp24A=({int _tmp6DF=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6DF,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp24A;
if(shallowcmp != 0)
return shallowcmp;{
# 1314
struct _tuple15 _tmp24B=({struct _tuple15 _tmp610;_tmp610.f1=t2,_tmp610.f2=t1;_tmp610;});struct _tuple15 _stmttmp3D=_tmp24B;struct _tuple15 _tmp24C=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_Absyn_Exp*_tmp250;struct Cyc_Absyn_Exp*_tmp24F;struct Cyc_List_List*_tmp254;enum Cyc_Absyn_AggrKind _tmp253;struct Cyc_List_List*_tmp252;enum Cyc_Absyn_AggrKind _tmp251;struct Cyc_List_List*_tmp256;struct Cyc_List_List*_tmp255;struct Cyc_Absyn_FnInfo _tmp258;struct Cyc_Absyn_FnInfo _tmp257;void*_tmp260;struct Cyc_Absyn_Exp*_tmp25F;struct Cyc_Absyn_Tqual _tmp25E;void*_tmp25D;void*_tmp25C;struct Cyc_Absyn_Exp*_tmp25B;struct Cyc_Absyn_Tqual _tmp25A;void*_tmp259;void*_tmp26C;void*_tmp26B;void*_tmp26A;void*_tmp269;struct Cyc_Absyn_Tqual _tmp268;void*_tmp267;void*_tmp266;void*_tmp265;void*_tmp264;void*_tmp263;struct Cyc_Absyn_Tqual _tmp262;void*_tmp261;struct Cyc_Absyn_Tvar*_tmp26E;struct Cyc_Absyn_Tvar*_tmp26D;struct Cyc_List_List*_tmp272;void*_tmp271;struct Cyc_List_List*_tmp270;void*_tmp26F;switch(*((int*)_tmp24C.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C.f2)->tag == 0U){_LL1: _tmp26F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C.f1)->f1;_tmp270=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C.f1)->f2;_tmp271=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C.f2)->f1;_tmp272=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24C.f2)->f2;_LL2: {void*c1=_tmp26F;struct Cyc_List_List*ts1=_tmp270;void*c2=_tmp271;struct Cyc_List_List*ts2=_tmp272;
# 1316
int _tmp273=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp273;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24C.f2)->tag == 1U){_LL3: _LL4:
# 1320
({void*_tmp274=0U;({struct _fat_ptr _tmp6E0=({const char*_tmp275="typecmp: can only compare closed types";_tag_fat(_tmp275,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E0,_tag_fat(_tmp274,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24C.f2)->tag == 2U){_LL5: _tmp26D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24C.f1)->f1;_tmp26E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24C.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp26D;struct Cyc_Absyn_Tvar*tv1=_tmp26E;
# 1324
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f2)->tag == 3U){_LL7: _tmp261=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f1)->f1).elt_type;_tmp262=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f1)->f1).elt_tq;_tmp263=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f1)->f1).ptr_atts).rgn;_tmp264=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f1)->f1).ptr_atts).nullable;_tmp265=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f1)->f1).ptr_atts).bounds;_tmp266=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f1)->f1).ptr_atts).zero_term;_tmp267=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f2)->f1).elt_type;_tmp268=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f2)->f1).elt_tq;_tmp269=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f2)->f1).ptr_atts).rgn;_tmp26A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f2)->f1).ptr_atts).nullable;_tmp26B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f2)->f1).ptr_atts).bounds;_tmp26C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24C.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp261;struct Cyc_Absyn_Tqual tqual2a=_tmp262;void*rgn2=_tmp263;void*null2a=_tmp264;void*b2=_tmp265;void*zt2=_tmp266;void*t1a=_tmp267;struct Cyc_Absyn_Tqual tqual1a=_tmp268;void*rgn1=_tmp269;void*null1a=_tmp26A;void*b1=_tmp26B;void*zt1=_tmp26C;
# 1328
int _tmp276=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp276;
if(etc != 0)return etc;{
int _tmp277=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp277;
if(rc != 0)return rc;{
int _tmp278=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp278;
if(tqc != 0)return tqc;{
int _tmp279=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp279;
if(cc != 0)return cc;{
int _tmp27A=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp27A;
if(zc != 0)return zc;{
int _tmp27B=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp27B;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f2)->tag == 4U){_LL9: _tmp259=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f1)->f1).elt_type;_tmp25A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f1)->f1).tq;_tmp25B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f1)->f1).num_elts;_tmp25C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f1)->f1).zero_term;_tmp25D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f2)->f1).elt_type;_tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f2)->f1).tq;_tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f2)->f1).num_elts;_tmp260=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24C.f2)->f1).zero_term;_LLA: {void*t2a=_tmp259;struct Cyc_Absyn_Tqual tq2a=_tmp25A;struct Cyc_Absyn_Exp*e1=_tmp25B;void*zt1=_tmp25C;void*t1a=_tmp25D;struct Cyc_Absyn_Tqual tq1a=_tmp25E;struct Cyc_Absyn_Exp*e2=_tmp25F;void*zt2=_tmp260;
# 1344
int _tmp27C=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp27C;
if(tqc != 0)return tqc;{
int _tmp27D=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp27D;
if(tc != 0)return tc;{
int _tmp27E=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp27E;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({void*_tmp27F=0U;({struct _fat_ptr _tmp6E1=({const char*_tmp280="missing expression in array index";_tag_fat(_tmp280,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E1,_tag_fat(_tmp27F,sizeof(void*),0U));});});
# 1354
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24C.f2)->tag == 5U){_LLB: _tmp257=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24C.f1)->f1;_tmp258=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24C.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp257;struct Cyc_Absyn_FnInfo f2=_tmp258;
# 1357
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp281=f1.args;struct Cyc_List_List*args1=_tmp281;
struct Cyc_List_List*_tmp282=f2.args;struct Cyc_List_List*args2=_tmp282;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp283=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3E=_tmp283;struct _tuple9 _tmp284=_stmttmp3E;void*_tmp286;struct Cyc_Absyn_Tqual _tmp285;_LL18: _tmp285=_tmp284.f2;_tmp286=_tmp284.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp285;void*t1=_tmp286;
struct _tuple9 _tmp287=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp3F=_tmp287;struct _tuple9 _tmp288=_stmttmp3F;void*_tmp28A;struct Cyc_Absyn_Tqual _tmp289;_LL1B: _tmp289=_tmp288.f2;_tmp28A=_tmp288.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp289;void*t2=_tmp28A;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1372
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp6E2=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp6E2,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1386
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp28B=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp28B;
struct Cyc_List_List*_tmp28C=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp28C;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp28D=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp40=_tmp28D;struct _tuple15 _tmp28E=_stmttmp40;void*_tmp290;void*_tmp28F;_LL1E: _tmp28F=_tmp28E.f1;_tmp290=_tmp28E.f2;_LL1F: {void*t1a=_tmp28F;void*t1b=_tmp290;
struct _tuple15 _tmp291=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp41=_tmp291;struct _tuple15 _tmp292=_stmttmp41;void*_tmp294;void*_tmp293;_LL21: _tmp293=_tmp292.f1;_tmp294=_tmp292.f2;_LL22: {void*t2a=_tmp293;void*t2b=_tmp294;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1396
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1404
({void*_tmp295=0U;({struct _fat_ptr _tmp6E3=({const char*_tmp296="typecmp: function type comparison should never get here!";_tag_fat(_tmp296,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E3,_tag_fat(_tmp295,sizeof(void*),0U));});});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24C.f2)->tag == 6U){_LLD: _tmp255=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24C.f1)->f1;_tmp256=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24C.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp255;struct Cyc_List_List*ts1=_tmp256;
# 1407
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24C.f2)->tag == 7U){_LLF: _tmp251=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24C.f1)->f1;_tmp252=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24C.f1)->f2;_tmp253=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24C.f2)->f1;_tmp254=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24C.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp251;struct Cyc_List_List*fs2=_tmp252;enum Cyc_Absyn_AggrKind k1=_tmp253;struct Cyc_List_List*fs1=_tmp254;
# 1410
if((int)k1 != (int)k2){
if((int)k1 == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24C.f2)->tag == 9U){_LL11: _tmp24F=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24C.f1)->f1;_tmp250=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24C.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp24F;struct Cyc_Absyn_Exp*e2=_tmp250;
# 1415
_tmp24D=e1;_tmp24E=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24C.f2)->tag == 11U){_LL13: _tmp24D=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24C.f1)->f1;_tmp24E=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24C.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp24D;struct Cyc_Absyn_Exp*e2=_tmp24E;
# 1417
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp297=0U;({struct _fat_ptr _tmp6E4=({const char*_tmp298="Unmatched case in typecmp";_tag_fat(_tmp298,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E4,_tag_fat(_tmp297,sizeof(void*),0U));});});}_LL0:;}}}
# 1424
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp299=({struct _tuple15 _tmp612;({void*_tmp6E6=Cyc_Tcutil_compress(t1);_tmp612.f1=_tmp6E6;}),({void*_tmp6E5=Cyc_Tcutil_compress(t2);_tmp612.f2=_tmp6E5;});_tmp612;});struct _tuple15 _stmttmp42=_tmp299;struct _tuple15 _tmp29A=_stmttmp42;void*_tmp29C;void*_tmp29B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29A.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29A.f2)->tag == 0U){_LL1: _tmp29B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29A.f1)->f1;_tmp29C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29A.f2)->f1;_LL2: {void*c1=_tmp29B;void*c2=_tmp29C;
# 1427
struct _tuple15 _tmp29D=({struct _tuple15 _tmp611;_tmp611.f1=c1,_tmp611.f2=c2;_tmp611;});struct _tuple15 _stmttmp43=_tmp29D;struct _tuple15 _tmp29E=_stmttmp43;int _tmp2A0;int _tmp29F;switch(*((int*)_tmp29E.f1)){case 2U: switch(*((int*)_tmp29E.f2)){case 2U: _LL6: _tmp29F=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29E.f1)->f1;_tmp2A0=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29E.f2)->f1;_LL7: {int i1=_tmp29F;int i2=_tmp2A0;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp29E.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1435
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1440
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1443
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29E.f2)->f1 == 0){_LL14: _LL15:
# 1438
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp29E.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1436
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1441
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1444
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29E.f2)->f1 == 0){_LL16: _LL17:
# 1439
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1445
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29E.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1442
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1446
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1448
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1450
 return 0;}_LL0:;}
# 1454
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp2A1=({struct _tuple15 _tmp614;({void*_tmp6E8=Cyc_Tcutil_compress(t1);_tmp614.f1=_tmp6E8;}),({void*_tmp6E7=Cyc_Tcutil_compress(t2);_tmp614.f2=_tmp6E7;});_tmp614;});struct _tuple15 _stmttmp44=_tmp2A1;struct _tuple15 _tmp2A2=_stmttmp44;void*_tmp2A4;void*_tmp2A3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A2.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A2.f2)->tag == 0U){_LL1: _tmp2A3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A2.f1)->f1;_tmp2A4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A2.f2)->f1;_LL2: {void*c1=_tmp2A3;void*c2=_tmp2A4;
# 1457
{struct _tuple15 _tmp2A5=({struct _tuple15 _tmp613;_tmp613.f1=c1,_tmp613.f2=c2;_tmp613;});struct _tuple15 _stmttmp45=_tmp2A5;struct _tuple15 _tmp2A6=_stmttmp45;int _tmp2A8;int _tmp2A7;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A6.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 2U){_LL6: _tmp2A7=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f1;_tmp2A8=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1;_LL7: {int i1=_tmp2A7;int i2=_tmp2A8;
# 1459
if(i1 != 0 && i1 != 1)return t1;else{
if(i2 != 0 && i2 != 1)return t2;else{
if(i1 >= i2)return t1;else{
return t2;}}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1469
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1474
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1467
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1476
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1466
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1470
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A6.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1475
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1468
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A6.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A6.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1477
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A6.f1)->tag == 4U){_LL18: _LL19:
# 1472
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A6.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1478
 goto _LL5;}}}}}}_LL5:;}
# 1480
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1483
return Cyc_Absyn_sint_type;}
# 1488
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1491
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 ||
 Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2A9=_cycalloc(sizeof(*_tmp2A9));_tmp2A9->v=t1;_tmp2A9;});}}}
# 1500
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1504
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp2AC=({struct Cyc_String_pa_PrintArg_struct _tmp616;_tmp616.tag=0U,({
struct _fat_ptr _tmp6E9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp616.f1=_tmp6E9;});_tmp616;});struct Cyc_String_pa_PrintArg_struct _tmp2AD=({struct Cyc_String_pa_PrintArg_struct _tmp615;_tmp615.tag=0U,({struct _fat_ptr _tmp6EA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp615.f1=_tmp6EA;});_tmp615;});void*_tmp2AA[2U];_tmp2AA[0]=& _tmp2AC,_tmp2AA[1]=& _tmp2AD;({unsigned _tmp6EC=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _fat_ptr _tmp6EB=({const char*_tmp2AB="type mismatch: expecting %s but found %s";_tag_fat(_tmp2AB,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp6EC,_tmp6EB,_tag_fat(_tmp2AA,sizeof(void*),2U));});});
return 0;}}}
# 1510
return 1;}
# 1515
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2AE=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp46=_tmp2AE;void*_tmp2AF=_stmttmp46;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AF)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1521
return 1;}
# 1525
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 1529
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp2B0=0U;({unsigned _tmp6EE=e->loc;struct _fat_ptr _tmp6ED=({const char*_tmp2B1="integral size mismatch; conversion supplied";_tag_fat(_tmp2B1,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6EE,_tmp6ED,_tag_fat(_tmp2B0,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1535
return 0;}
# 1539
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 1543
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp2B2=0U;({unsigned _tmp6F0=e->loc;struct _fat_ptr _tmp6EF=({const char*_tmp2B3="integral size mismatch; conversion supplied";_tag_fat(_tmp2B3,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6F0,_tmp6EF,_tag_fat(_tmp2B2,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1549
return 0;}
# 1554
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1560
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp6F1=t;Cyc_Unify_unify(_tmp6F1,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1565
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2B4=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2B4;
void*_tmp2B5=t;struct Cyc_Absyn_Exp*_tmp2B6;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B5)->tag == 9U){_LL1: _tmp2B6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B5)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2B6;
return e;}}else{_LL3: _LL4: {
# 1570
struct Cyc_Absyn_Exp*_tmp2B7=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2B7;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1579
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B8=Cyc_Tcutil_compress(b);void*_stmttmp47=_tmp2B8;void*_tmp2B9=_stmttmp47;struct Cyc_List_List*_tmp2BA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B9)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2BA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B9)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2BA;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp2BD=({struct Cyc_String_pa_PrintArg_struct _tmp617;_tmp617.tag=0U,({struct _fat_ptr _tmp6F2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp617.f1=_tmp6F2;});_tmp617;});void*_tmp2BB[1U];_tmp2BB[0]=& _tmp2BD;({struct _fat_ptr _tmp6F3=({const char*_tmp2BC="get_bounds_exp: %s";_tag_fat(_tmp2BC,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F3,_tag_fat(_tmp2BB,sizeof(void*),1U));});});}_LL0:;}}
# 1588
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2BE=Cyc_Tcutil_compress(t);void*_stmttmp48=_tmp2BE;void*_tmp2BF=_stmttmp48;void*_tmp2C0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->tag == 3U){_LL1: _tmp2C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BF)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2C0;
# 1591
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2C3=({struct Cyc_String_pa_PrintArg_struct _tmp618;_tmp618.tag=0U,({struct _fat_ptr _tmp6F4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp618.f1=_tmp6F4;});_tmp618;});void*_tmp2C1[1U];_tmp2C1[0]=& _tmp2C3;({struct _fat_ptr _tmp6F5=({const char*_tmp2C2="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp2C2,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F5,_tag_fat(_tmp2C1,sizeof(void*),1U));});});}_LL0:;}
# 1597
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2C4=_cycalloc(sizeof(*_tmp2C4));_tmp2C4->v=tvs;_tmp2C4;}));}
# 1601
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2C5=_cycalloc(sizeof(*_tmp2C5));_tmp2C5->v=tvs;_tmp2C5;}));}
# 1606
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1614
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp2C6=({struct _tuple15 _tmp61C;_tmp61C.f1=t1,_tmp61C.f2=t2;_tmp61C;});struct _tuple15 _stmttmp49=_tmp2C6;struct _tuple15 _tmp2C7=_stmttmp49;void*_tmp2CF;struct Cyc_Absyn_Exp*_tmp2CE;struct Cyc_Absyn_Tqual _tmp2CD;void*_tmp2CC;void*_tmp2CB;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Tqual _tmp2C9;void*_tmp2C8;struct Cyc_Absyn_PtrInfo _tmp2D1;struct Cyc_Absyn_PtrInfo _tmp2D0;switch(*((int*)_tmp2C7.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7.f2)->tag == 3U){_LL1: _tmp2D0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7.f1)->f1;_tmp2D1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C7.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2D0;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2D1;
# 1619
int okay=1;
# 1621
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable))
# 1623
okay=!Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable);
# 1625
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple22 _tmp2D2=({struct _tuple22 _tmp619;({struct Cyc_Absyn_Exp*_tmp6F9=({void*_tmp6F8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6F8,(pinfo_a.ptr_atts).bounds);});_tmp619.f1=_tmp6F9;}),({
struct Cyc_Absyn_Exp*_tmp6F7=({void*_tmp6F6=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6F6,(pinfo_b.ptr_atts).bounds);});_tmp619.f2=_tmp6F7;});_tmp619;});
# 1626
struct _tuple22 _stmttmp4A=_tmp2D2;struct _tuple22 _tmp2D3=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp2D5;struct Cyc_Absyn_Exp*_tmp2D4;if(_tmp2D3.f2 == 0){_LLA: _LLB:
# 1629
 okay=1;goto _LL9;}else{if(_tmp2D3.f1 == 0){_LLC: _LLD:
# 1632
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp6FA=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6FA,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp2D4=_tmp2D3.f1;_tmp2D5=_tmp2D3.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2D4;struct Cyc_Absyn_Exp*e2=_tmp2D5;
# 1638
okay=okay &&({struct Cyc_Absyn_Exp*_tmp6FB=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp6FB,(struct Cyc_Absyn_Exp*)_check_null(e1));});
# 1642
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({void*_tmp2D6=0U;({unsigned _tmp6FD=loc;struct _fat_ptr _tmp6FC=({const char*_tmp2D7="implicit cast to shorter array";_tag_fat(_tmp2D7,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp6FD,_tmp6FC,_tag_fat(_tmp2D6,sizeof(void*),0U));});});
goto _LL9;}}}_LL9:;}
# 1648
okay=okay &&(!(pinfo_a.elt_tq).real_const ||(pinfo_b.elt_tq).real_const);
# 1650
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
# 1653
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp2DA=({struct Cyc_String_pa_PrintArg_struct _tmp61B;_tmp61B.tag=0U,({
struct _fat_ptr _tmp6FE=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((pinfo_a.ptr_atts).rgn));_tmp61B.f1=_tmp6FE;});_tmp61B;});struct Cyc_String_pa_PrintArg_struct _tmp2DB=({struct Cyc_String_pa_PrintArg_struct _tmp61A;_tmp61A.tag=0U,({
struct _fat_ptr _tmp6FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((pinfo_b.ptr_atts).rgn));_tmp61A.f1=_tmp6FF;});_tmp61A;});void*_tmp2D8[2U];_tmp2D8[0]=& _tmp2DA,_tmp2D8[1]=& _tmp2DB;({unsigned _tmp701=loc;struct _fat_ptr _tmp700=({const char*_tmp2D9="implicit cast from region %s to region %s";_tag_fat(_tmp2D9,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp701,_tmp700,_tag_fat(_tmp2D8,sizeof(void*),2U));});});}else{
okay=0;}}
# 1660
okay=okay &&(Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)||
# 1662
 Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)&&(pinfo_b.elt_tq).real_const);{
# 1670
int _tmp2DC=
({void*_tmp702=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp702,(pinfo_b.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term);
# 1670
int deep_subtype=_tmp2DC;
# 1674
okay=okay &&(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type)||
(deep_subtype &&((pinfo_b.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type))))&& Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type));
return okay;}}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->tag == 4U){_LL3: _tmp2C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f1)->f1).elt_type;_tmp2C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f1)->f1).tq;_tmp2CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f1)->f1).num_elts;_tmp2CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f1)->f1).zero_term;_tmp2CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->f1).elt_type;_tmp2CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->f1).tq;_tmp2CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->f1).num_elts;_tmp2CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C7.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2C8;struct Cyc_Absyn_Tqual tq1a=_tmp2C9;struct Cyc_Absyn_Exp*e1=_tmp2CA;void*zt1=_tmp2CB;void*t2a=_tmp2CC;struct Cyc_Absyn_Tqual tq2a=_tmp2CD;struct Cyc_Absyn_Exp*e2=_tmp2CE;void*zt2=_tmp2CF;
# 1680
int okay;
# 1683
okay=Cyc_Unify_unify(zt1,zt2)&&(
(e1 != 0 && e2 != 0)&& Cyc_Evexp_same_uint_const_exp(e1,e2));
# 1686
return(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C7.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1688
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1690
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1694
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2DD=Cyc_Tcutil_compress(t);void*_stmttmp4B=_tmp2DD;void*_tmp2DE=_stmttmp4B;void*_tmp2DF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->tag == 3U){_LL1: _tmp2DF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DE)->f1).elt_type;_LL2: {void*e=_tmp2DF;
return e;}}else{_LL3: _LL4:
({void*_tmp2E0=0U;({struct _fat_ptr _tmp703=({const char*_tmp2E1="pointer_elt_type";_tag_fat(_tmp2E1,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp703,_tag_fat(_tmp2E0,sizeof(void*),0U));});});}_LL0:;}
# 1700
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2E2=Cyc_Tcutil_compress(t);void*_stmttmp4C=_tmp2E2;void*_tmp2E3=_stmttmp4C;struct Cyc_Absyn_PtrAtts*_tmp2E4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E3)->tag == 3U){_LL1: _tmp2E4=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E3)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2E4;
return p->rgn;}}else{_LL3: _LL4:
({void*_tmp2E5=0U;({struct _fat_ptr _tmp704=({const char*_tmp2E6="pointer_elt_type";_tag_fat(_tmp2E6,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp704,_tag_fat(_tmp2E5,sizeof(void*),0U));});});}_LL0:;}
# 1707
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2E7=Cyc_Tcutil_compress(t);void*_stmttmp4D=_tmp2E7;void*_tmp2E8=_stmttmp4D;void*_tmp2E9;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->tag == 3U){_LL1: _tmp2E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2E9;
# 1710
*rgn=r;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1719
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2EA=Cyc_Tcutil_compress(t);void*_stmttmp4E=_tmp2EA;void*_tmp2EB=_stmttmp4E;void*_tmp2ED;void*_tmp2EC;switch(*((int*)_tmp2EB)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2EB)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2EC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EB)->f1).ptr_atts).nullable;_tmp2ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EB)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2EC;void*bounds=_tmp2ED;
# 1727
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1733
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2EE=e->r;void*_stmttmp4F=_tmp2EE;void*_tmp2EF=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp2F1;void*_tmp2F0;struct _fat_ptr _tmp2F2;switch(*((int*)_tmp2EF)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2F2=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp2F2;
# 1742
unsigned long _tmp2F3=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2F3;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 1752
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1740
 return 1;case 14U: _LLD: _tmp2F0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EF)->f1;_tmp2F1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EF)->f2;_LLE: {void*t=_tmp2F0;struct Cyc_Absyn_Exp*e2=_tmp2F1;
# 1753
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1758
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1767
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1772
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1777
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1786
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1791
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1796
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2F4=ka;enum Cyc_Absyn_AliasQual _tmp2F6;enum Cyc_Absyn_KindQual _tmp2F5;_LL1: _tmp2F5=_tmp2F4->kind;_tmp2F6=_tmp2F4->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp2F5;enum Cyc_Absyn_AliasQual a=_tmp2F6;
{enum Cyc_Absyn_AliasQual _tmp2F7=a;switch(_tmp2F7){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1800
enum Cyc_Absyn_KindQual _tmp2F8=k;switch(_tmp2F8){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1811
{enum Cyc_Absyn_KindQual _tmp2F9=k;switch(_tmp2F9){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1818
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1820
{enum Cyc_Absyn_KindQual _tmp2FA=k;switch(_tmp2FA){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1827
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1830
({struct Cyc_String_pa_PrintArg_struct _tmp2FD=({struct Cyc_String_pa_PrintArg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp705=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp61D.f1=_tmp705;});_tmp61D;});void*_tmp2FB[1U];_tmp2FB[0]=& _tmp2FD;({struct _fat_ptr _tmp706=({const char*_tmp2FC="kind_to_opt: bad kind %s\n";_tag_fat(_tmp2FC,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp706,_tag_fat(_tmp2FB,sizeof(void*),1U));});});}}
# 1833
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2FE=_cycalloc(sizeof(*_tmp2FE));_tmp2FE->tag=0U,_tmp2FE->f1=k;_tmp2FE;});}
# 1836
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp2FF=_cycalloc(sizeof(*_tmp2FF));({void*_tmp707=Cyc_Tcutil_kind_to_bound(k);_tmp2FF->v=_tmp707;});_tmp2FF;});}
# 1842
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp300=Cyc_Tcutil_compress(t2);void*_stmttmp50=_tmp300;void*_tmp301=_stmttmp50;void*_tmp302;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->tag == 3U){_LL1: _tmp302=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp302;
# 1847
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp708=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp708;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1856
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1862
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1866
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp70B=({struct _fat_ptr*_tmp315=_cycalloc(sizeof(*_tmp315));({struct _fat_ptr _tmp70A=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp314=({struct Cyc_Int_pa_PrintArg_struct _tmp61F;_tmp61F.tag=1U,_tmp61F.f1=(unsigned long)counter ++;_tmp61F;});void*_tmp312[1U];_tmp312[0]=& _tmp314;({struct _fat_ptr _tmp709=({const char*_tmp313="__aliasvar%d";_tag_fat(_tmp313,sizeof(char),13U);});Cyc_aprintf(_tmp709,_tag_fat(_tmp312,sizeof(void*),1U));});});*_tmp315=_tmp70A;});_tmp315;});_tmp316->f2=_tmp70B;});_tmp316;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp70C=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp311=_cycalloc(sizeof(*_tmp311));_tmp311->tag=4U,_tmp311->f1=vd;_tmp311;});Cyc_Absyn_varb_exp(_tmp70C,e->loc);});
# 1875
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 1877
{void*_tmp303=Cyc_Tcutil_compress(e_type);void*_stmttmp51=_tmp303;void*_tmp304=_stmttmp51;struct Cyc_Absyn_PtrLoc*_tmp30B;void*_tmp30A;void*_tmp309;void*_tmp308;void*_tmp307;struct Cyc_Absyn_Tqual _tmp306;void*_tmp305;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->tag == 3U){_LL1: _tmp305=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).elt_type;_tmp306=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).elt_tq;_tmp307=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).ptr_atts).rgn;_tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).ptr_atts).nullable;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).ptr_atts).bounds;_tmp30A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).ptr_atts).zero_term;_tmp30B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp304)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp305;struct Cyc_Absyn_Tqual tq=_tmp306;void*old_r=_tmp307;void*nb=_tmp308;void*b=_tmp309;void*zt=_tmp30A;struct Cyc_Absyn_PtrLoc*pl=_tmp30B;
# 1879
{void*_tmp30C=Cyc_Tcutil_compress(old_r);void*_stmttmp52=_tmp30C;void*_tmp30D=_stmttmp52;struct Cyc_Core_Opt*_tmp30F;void**_tmp30E;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30D)->tag == 1U){_LL6: _tmp30E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30D)->f2;_tmp30F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30D)->f4;_LL7: {void**topt=_tmp30E;struct Cyc_Core_Opt*ts=_tmp30F;
# 1881
void*_tmp310=Cyc_Absyn_var_type(tv);void*new_r=_tmp310;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1886
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1890
e->topt=0;
vd->initializer=0;{
# 1894
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1896
return({struct _tuple23 _tmp61E;_tmp61E.f1=d,_tmp61E.f2=ve;_tmp61E;});}}
# 1901
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1904
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1909
void*_tmp317=Cyc_Tcutil_compress(wants_type);void*_stmttmp53=_tmp317;void*_tmp318=_stmttmp53;void*_tmp319;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318)->tag == 3U){_LL1: _tmp319=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp318)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp319;
# 1911
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp31A=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp31A;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1917
return 0;}
# 1921
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1923
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1927
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1930
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1932
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp31C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp624;_tmp624.tag=0U,({struct _fat_ptr _tmp70D=({const char*_tmp323="integral size mismatch; ";_tag_fat(_tmp323,sizeof(char),25U);});_tmp624.f1=_tmp70D;});_tmp624;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp31D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp623;_tmp623.tag=2U,_tmp623.f1=(void*)t1;_tmp623;});struct Cyc_Warn_String_Warn_Warg_struct _tmp31E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp622;_tmp622.tag=0U,({
struct _fat_ptr _tmp70E=({const char*_tmp322=" -> ";_tag_fat(_tmp322,sizeof(char),5U);});_tmp622.f1=_tmp70E;});_tmp622;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp31F=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp621;_tmp621.tag=2U,_tmp621.f1=(void*)t2;_tmp621;});struct Cyc_Warn_String_Warn_Warg_struct _tmp320=({struct Cyc_Warn_String_Warn_Warg_struct _tmp620;_tmp620.tag=0U,({struct _fat_ptr _tmp70F=({const char*_tmp321=" conversion supplied";_tag_fat(_tmp321,sizeof(char),21U);});_tmp620.f1=_tmp70F;});_tmp620;});void*_tmp31B[5U];_tmp31B[0]=& _tmp31C,_tmp31B[1]=& _tmp31D,_tmp31B[2]=& _tmp31E,_tmp31B[3]=& _tmp31F,_tmp31B[4]=& _tmp320;({unsigned _tmp710=e->loc;Cyc_Warn_warn2(_tmp710,_tag_fat(_tmp31B,sizeof(void*),5U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1940
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp326=({struct Cyc_String_pa_PrintArg_struct _tmp627;_tmp627.tag=0U,({
struct _fat_ptr _tmp711=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp627.f1=_tmp711;});_tmp627;});struct Cyc_String_pa_PrintArg_struct _tmp327=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({struct _fat_ptr _tmp712=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp626.f1=_tmp712;});_tmp626;});struct Cyc_String_pa_PrintArg_struct _tmp328=({struct Cyc_String_pa_PrintArg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp713=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp625.f1=_tmp713;});_tmp625;});void*_tmp324[3U];_tmp324[0]=& _tmp326,_tmp324[1]=& _tmp327,_tmp324[2]=& _tmp328;({unsigned _tmp715=e->loc;struct _fat_ptr _tmp714=({const char*_tmp325="implicit alias coercion for %s:%s to %s";_tag_fat(_tmp325,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp715,_tmp714,_tag_fat(_tmp324,sizeof(void*),3U));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1948
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1952
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1956
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp32B=({struct Cyc_String_pa_PrintArg_struct _tmp629;_tmp629.tag=0U,({struct _fat_ptr _tmp716=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp629.f1=_tmp716;});_tmp629;});struct Cyc_String_pa_PrintArg_struct _tmp32C=({struct Cyc_String_pa_PrintArg_struct _tmp628;_tmp628.tag=0U,({struct _fat_ptr _tmp717=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp628.f1=_tmp717;});_tmp628;});void*_tmp329[2U];_tmp329[0]=& _tmp32B,_tmp329[1]=& _tmp32C;({unsigned _tmp719=e->loc;struct _fat_ptr _tmp718=({const char*_tmp32A="implicit cast from %s to %s";_tag_fat(_tmp32A,sizeof(char),28U);});Cyc_Tcutil_warn(_tmp719,_tmp718,_tag_fat(_tmp329,sizeof(void*),2U));});});
return 1;}
# 1962
return 0;}
# 1968
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1971
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1985 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1988
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1991
struct _tuple24 _tmp32D=*env;struct _tuple24 _stmttmp54=_tmp32D;struct _tuple24 _tmp32E=_stmttmp54;int _tmp331;struct _RegionHandle*_tmp330;struct Cyc_List_List*_tmp32F;_LL1: _tmp32F=_tmp32E.f1;_tmp330=_tmp32E.f2;_tmp331=_tmp32E.f3;_LL2: {struct Cyc_List_List*inst=_tmp32F;struct _RegionHandle*r=_tmp330;int flatten=_tmp331;
void*_tmp332=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp332;
struct Cyc_List_List*_tmp333=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp333;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp335=_region_malloc(r,sizeof(*_tmp335));({struct _tuple12*_tmp71A=({struct _tuple12*_tmp334=_region_malloc(r,sizeof(*_tmp334));_tmp334->f1=x->tq,_tmp334->f2=t;_tmp334;});_tmp335->hd=_tmp71A;}),_tmp335->tl=0;_tmp335;});
return ts;}}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1998
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 2000
struct _tuple25 _tmp336=*env;struct _tuple25 _stmttmp55=_tmp336;struct _tuple25 _tmp337=_stmttmp55;int _tmp339;struct _RegionHandle*_tmp338;_LL1: _tmp338=_tmp337.f1;_tmp339=_tmp337.f2;_LL2: {struct _RegionHandle*r=_tmp338;int flatten=_tmp339;
struct _tuple12 _tmp33A=*x;struct _tuple12 _stmttmp56=_tmp33A;struct _tuple12 _tmp33B=_stmttmp56;void*_tmp33D;struct Cyc_Absyn_Tqual _tmp33C;_LL4: _tmp33C=_tmp33B.f1;_tmp33D=_tmp33B.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp33C;void*t=_tmp33D;
struct Cyc_List_List*_tmp33E=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp33E;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp340=_region_malloc(r,sizeof(*_tmp340));({struct _tuple12*_tmp71B=({struct _tuple12*_tmp33F=_region_malloc(r,sizeof(*_tmp33F));_tmp33F->f1=tq,_tmp33F->f2=t;_tmp33F;});_tmp340->hd=_tmp71B;}),_tmp340->tl=0;_tmp340;});
return ts;}}}
# 2007
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 2010
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp341=t1;struct Cyc_List_List*_tmp342;struct Cyc_List_List*_tmp343;struct Cyc_List_List*_tmp345;struct Cyc_Absyn_Aggrdecl*_tmp344;switch(*((int*)_tmp341)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp341)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp341)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp344=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp341)->f1)->f1).KnownAggr).val;_tmp345=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp341)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp344;struct Cyc_List_List*ts=_tmp345;
# 2028
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 2030
return({struct Cyc_List_List*_tmp350=_region_malloc(r,sizeof(*_tmp350));({struct _tuple12*_tmp71D=({struct _tuple12*_tmp34F=_region_malloc(r,sizeof(*_tmp34F));({struct Cyc_Absyn_Tqual _tmp71C=Cyc_Absyn_empty_tqual(0U);_tmp34F->f1=_tmp71C;}),_tmp34F->f2=t1;_tmp34F;});_tmp350->hd=_tmp71D;}),_tmp350->tl=0;_tmp350;});{
struct Cyc_List_List*_tmp351=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp351;
struct _tuple24 env=({struct _tuple24 _tmp62A;_tmp62A.f1=inst,_tmp62A.f2=r,_tmp62A.f3=flatten;_tmp62A;});
struct Cyc_List_List*_tmp352=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp57=_tmp352;struct Cyc_List_List*_tmp353=_stmttmp57;struct Cyc_List_List*_tmp355;struct Cyc_Absyn_Aggrfield*_tmp354;if(_tmp353 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp354=(struct Cyc_Absyn_Aggrfield*)_tmp353->hd;_tmp355=_tmp353->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp354;struct Cyc_List_List*tl=_tmp355;
# 2036
struct Cyc_List_List*_tmp356=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp356;
env.f3=0;{
struct Cyc_List_List*_tmp357=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp357;
struct Cyc_List_List*_tmp358=({struct Cyc_List_List*_tmp359=_region_malloc(r,sizeof(*_tmp359));_tmp359->hd=hd2,_tmp359->tl=tl2;_tmp359;});struct Cyc_List_List*tts=_tmp358;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp343=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp341)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp343;
# 2015
struct _tuple25 _tmp346=({struct _tuple25 _tmp62B;_tmp62B.f1=r,_tmp62B.f2=flatten;_tmp62B;});struct _tuple25 env=_tmp346;
# 2017
struct Cyc_List_List*_tmp347=tqs;struct Cyc_List_List*_tmp349;struct _tuple12*_tmp348;if(_tmp347 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp348=(struct _tuple12*)_tmp347->hd;_tmp349=_tmp347->tl;_LLF: {struct _tuple12*hd=_tmp348;struct Cyc_List_List*tl=_tmp349;
# 2020
struct Cyc_List_List*_tmp34A=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp34A;
env.f2=0;{
struct Cyc_List_List*_tmp34B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp34B;
struct Cyc_List_List*_tmp34C=({struct Cyc_List_List*_tmp34E=_region_malloc(r,sizeof(*_tmp34E));_tmp34E->hd=hd2,_tmp34E->tl=tl2;_tmp34E;});struct Cyc_List_List*temp=_tmp34C;
return({struct _RegionHandle*_tmp71E=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp71E,({struct Cyc_List_List*_tmp34D=_region_malloc(r,sizeof(*_tmp34D));_tmp34D->hd=hd2,_tmp34D->tl=tl2;_tmp34D;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp341)->f1 == Cyc_Absyn_StructA){_LL7: _tmp342=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp341)->f2;_LL8: {struct Cyc_List_List*fs=_tmp342;
# 2043
struct _tuple24 env=({struct _tuple24 _tmp62C;_tmp62C.f1=0,_tmp62C.f2=r,_tmp62C.f3=flatten;_tmp62C;});
struct Cyc_List_List*_tmp35A=fs;struct Cyc_List_List*_tmp35C;struct Cyc_Absyn_Aggrfield*_tmp35B;if(_tmp35A == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp35B=(struct Cyc_Absyn_Aggrfield*)_tmp35A->hd;_tmp35C=_tmp35A->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp35B;struct Cyc_List_List*tl=_tmp35C;
# 2047
struct Cyc_List_List*_tmp35D=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp35D;
env.f3=0;{
struct Cyc_List_List*_tmp35E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp35E;
struct Cyc_List_List*_tmp35F=({struct Cyc_List_List*_tmp360=_region_malloc(r,sizeof(*_tmp360));_tmp360->hd=hd2,_tmp360->tl=tl2;_tmp360;});struct Cyc_List_List*tts=_tmp35F;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2053
 goto _LL0;}_LL0:;}}
# 2056
return({struct Cyc_List_List*_tmp362=_region_malloc(r,sizeof(*_tmp362));({struct _tuple12*_tmp720=({struct _tuple12*_tmp361=_region_malloc(r,sizeof(*_tmp361));({struct Cyc_Absyn_Tqual _tmp71F=Cyc_Absyn_empty_tqual(0U);_tmp361->f1=_tmp71F;}),_tmp361->f2=t1;_tmp361;});_tmp362->hd=_tmp720;}),_tmp362->tl=0;_tmp362;});}
# 2060
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp363=(void*)t->hd;void*_stmttmp58=_tmp363;void*_tmp364=_stmttmp58;switch(*((int*)_tmp364)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
# 2067
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2070
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2073
return 1;}
# 2076
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp365=({struct _tuple15 _tmp62D;({void*_tmp722=Cyc_Tcutil_compress(t1);_tmp62D.f1=_tmp722;}),({void*_tmp721=Cyc_Tcutil_compress(t2);_tmp62D.f2=_tmp721;});_tmp62D;});struct _tuple15 _stmttmp59=_tmp365;struct _tuple15 _tmp366=_stmttmp59;enum Cyc_Absyn_Size_of _tmp368;enum Cyc_Absyn_Size_of _tmp367;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp366.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp366.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp366.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp366.f2)->f1)->tag == 1U){_LL1: _tmp367=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp366.f1)->f1)->f2;_tmp368=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp366.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp367;enum Cyc_Absyn_Size_of b2=_tmp368;
# 2079
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2087
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2089
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp369=({struct _tuple15 _tmp62E;_tmp62E.f1=t1,_tmp62E.f2=t2;_tmp62E;});struct _tuple15 _stmttmp5A=_tmp369;struct _tuple15 _tmp36A=_stmttmp5A;struct Cyc_Absyn_FnInfo _tmp36C;struct Cyc_Absyn_FnInfo _tmp36B;struct Cyc_List_List*_tmp371;struct Cyc_Absyn_Datatypedecl*_tmp370;struct Cyc_List_List*_tmp36F;struct Cyc_Absyn_Datatypefield*_tmp36E;struct Cyc_Absyn_Datatypedecl*_tmp36D;void*_tmp37D;void*_tmp37C;void*_tmp37B;void*_tmp37A;struct Cyc_Absyn_Tqual _tmp379;void*_tmp378;void*_tmp377;void*_tmp376;void*_tmp375;void*_tmp374;struct Cyc_Absyn_Tqual _tmp373;void*_tmp372;switch(*((int*)_tmp36A.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f2)->tag == 3U){_LL1: _tmp372=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f1)->f1).elt_type;_tmp373=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f1)->f1).elt_tq;_tmp374=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f1)->f1).ptr_atts).rgn;_tmp375=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f1)->f1).ptr_atts).nullable;_tmp376=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f1)->f1).ptr_atts).bounds;_tmp377=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f1)->f1).ptr_atts).zero_term;_tmp378=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f2)->f1).elt_type;_tmp379=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f2)->f1).elt_tq;_tmp37A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f2)->f1).ptr_atts).rgn;_tmp37B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f2)->f1).ptr_atts).nullable;_tmp37C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f2)->f1).ptr_atts).bounds;_tmp37D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp36A.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp372;struct Cyc_Absyn_Tqual q_a=_tmp373;void*rt_a=_tmp374;void*null_a=_tmp375;void*b_a=_tmp376;void*zt_a=_tmp377;void*t_b=_tmp378;struct Cyc_Absyn_Tqual q_b=_tmp379;void*rt_b=_tmp37A;void*null_b=_tmp37B;void*b_b=_tmp37C;void*zt_b=_tmp37D;
# 2101
if(q_a.real_const && !q_b.real_const)
return 0;
# 2104
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 2108
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 2112
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 2116
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp37E=({void*_tmp723=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp723,b_a);});struct Cyc_Absyn_Exp*e1=_tmp37E;
struct Cyc_Absyn_Exp*_tmp37F=({void*_tmp724=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp724,b_b);});struct Cyc_Absyn_Exp*e2=_tmp37F;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 2126
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 2132
int _tmp380=
({void*_tmp725=b_b;Cyc_Unify_unify(_tmp725,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2132
int deep_subtype=_tmp380;
# 2137
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp729=po;struct Cyc_List_List*_tmp728=({struct Cyc_List_List*_tmp382=_cycalloc(sizeof(*_tmp382));({struct _tuple15*_tmp726=({struct _tuple15*_tmp381=_cycalloc(sizeof(*_tmp381));_tmp381->f1=t1,_tmp381->f2=t2;_tmp381;});_tmp382->hd=_tmp726;}),_tmp382->tl=assume;_tmp382;});void*_tmp727=t_a;Cyc_Tcutil_ptrsubtype(_tmp729,_tmp728,_tmp727,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp36D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp36E=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp36F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f1)->f2;_tmp370=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f2)->f1)->f1).KnownDatatype).val;_tmp371=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp36A.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp36D;struct Cyc_Absyn_Datatypefield*df=_tmp36E;struct Cyc_List_List*ts1=_tmp36F;struct Cyc_Absyn_Datatypedecl*dd2=_tmp370;struct Cyc_List_List*ts2=_tmp371;
# 2144
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 2146
if(({int _tmp72A=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp72A != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp36A.f2)->tag == 5U){_LL5: _tmp36B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp36A.f1)->f1;_tmp36C=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp36A.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp36B;struct Cyc_Absyn_FnInfo f2=_tmp36C;
# 2153
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp383=f1.tvars;struct Cyc_List_List*tvs1=_tmp383;
struct Cyc_List_List*_tmp384=f2.tvars;struct Cyc_List_List*tvs2=_tmp384;
if(({int _tmp72B=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp72B != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp386=_cycalloc(sizeof(*_tmp386));({struct _tuple19*_tmp72D=({struct _tuple19*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp72C=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp385->f2=_tmp72C;});_tmp385;});_tmp386->hd=_tmp72D;}),_tmp386->tl=inst;_tmp386;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2164
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp730=po;struct Cyc_List_List*_tmp72F=assume;void*_tmp72E=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp387=_cycalloc(sizeof(*_tmp387));_tmp387->tag=5U,_tmp387->f1=f1;_tmp387;});Cyc_Tcutil_subtype(_tmp730,_tmp72F,_tmp72E,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp388=_cycalloc(sizeof(*_tmp388));_tmp388->tag=5U,_tmp388->f1=f2;_tmp388;}));});}}}
# 2171
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp389=f1.args;struct Cyc_List_List*args1=_tmp389;
struct Cyc_List_List*_tmp38A=f2.args;struct Cyc_List_List*args2=_tmp38A;
# 2176
if(({int _tmp731=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp731 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2178
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp38B=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp5B=_tmp38B;struct _tuple9 _tmp38C=_stmttmp5B;void*_tmp38E;struct Cyc_Absyn_Tqual _tmp38D;_LLA: _tmp38D=_tmp38C.f2;_tmp38E=_tmp38C.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp38D;void*t1=_tmp38E;
struct _tuple9 _tmp38F=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp5C=_tmp38F;struct _tuple9 _tmp390=_stmttmp5C;void*_tmp392;struct Cyc_Absyn_Tqual _tmp391;_LLD: _tmp391=_tmp390.f2;_tmp392=_tmp390.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp391;void*t2=_tmp392;
# 2182
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2186
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp393=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp393;
struct Cyc_Absyn_VarargInfo _tmp394=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp394;
# 2191
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2196
if(!({void*_tmp732=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp732,(void*)_check_null(f2.effect));}))return 0;
# 2198
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2200
if(!Cyc_Tcutil_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2202
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2204
return 0;
# 2206
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2208
return 0;
# 2210
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2221
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2225
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp395=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp5D=_tmp395;struct _tuple12*_tmp396=_stmttmp5D;void*_tmp398;struct Cyc_Absyn_Tqual _tmp397;_LL1: _tmp397=_tmp396->f1;_tmp398=_tmp396->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp397;void*t1a=_tmp398;
struct _tuple12*_tmp399=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp5E=_tmp399;struct _tuple12*_tmp39A=_stmttmp5E;void*_tmp39C;struct Cyc_Absyn_Tqual _tmp39B;_LL4: _tmp39B=_tmp39A->f1;_tmp39C=_tmp39A->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp39B;void*t2a=_tmp39C;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a))
continue;
if(Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2241
return 1;}
# 2246
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2252
{void*_tmp39D=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39D)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39D)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2255
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2257
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2261
void*_tmp39E=t1;void*_tmp39F;struct Cyc_Absyn_Enumdecl*_tmp3A0;void*_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Tqual _tmp3A2;void*_tmp3A1;void*_tmp3AA;void*_tmp3A9;void*_tmp3A8;void*_tmp3A7;struct Cyc_Absyn_Tqual _tmp3A6;void*_tmp3A5;switch(*((int*)_tmp39E)){case 3U: _LLA: _tmp3A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39E)->f1).elt_type;_tmp3A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39E)->f1).elt_tq;_tmp3A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39E)->f1).ptr_atts).rgn;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39E)->f1).ptr_atts).nullable;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39E)->f1).ptr_atts).bounds;_tmp3AA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39E)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp3A5;struct Cyc_Absyn_Tqual q_a=_tmp3A6;void*rt_a=_tmp3A7;void*null_a=_tmp3A8;void*b_a=_tmp3A9;void*zt_a=_tmp3AA;
# 2270
{void*_tmp3AB=t2;void*_tmp3B1;void*_tmp3B0;void*_tmp3AF;void*_tmp3AE;struct Cyc_Absyn_Tqual _tmp3AD;void*_tmp3AC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->tag == 3U){_LL19: _tmp3AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).elt_type;_tmp3AD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).elt_tq;_tmp3AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).ptr_atts).rgn;_tmp3AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).ptr_atts).nullable;_tmp3B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).ptr_atts).bounds;_tmp3B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AB)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3AC;struct Cyc_Absyn_Tqual q_b=_tmp3AD;void*rt_b=_tmp3AE;void*null_b=_tmp3AF;void*b_b=_tmp3B0;void*zt_b=_tmp3B1;
# 2272
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3B2=({struct Cyc_List_List*_tmp3BA=_cycalloc(sizeof(*_tmp3BA));({struct _tuple15*_tmp733=({struct _tuple15*_tmp3B9=_cycalloc(sizeof(*_tmp3B9));_tmp3B9->f1=t1,_tmp3B9->f2=t2;_tmp3B9;});_tmp3BA->hd=_tmp733;}),_tmp3BA->tl=0;_tmp3BA;});struct Cyc_List_List*assump=_tmp3B2;
int _tmp3B3=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3B3;
# 2285 "tcutil.cyc"
int _tmp3B4=
({void*_tmp734=b_b;Cyc_Unify_unify(_tmp734,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2285
int deep_castable=_tmp3B4;
# 2288
int _tmp3B5=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2288
int ptrsub=_tmp3B5;
# 2291
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2293
int _tmp3B6=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2293
int bitcase=_tmp3B6;
# 2296
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3B7=({void*_tmp735=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp735,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3B7;
struct Cyc_Absyn_Exp*_tmp3B8=({void*_tmp736=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp736,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3B8;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2305
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2310
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2316
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp3A1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).elt_type;_tmp3A2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).tq;_tmp3A3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).num_elts;_tmp3A4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39E)->f1).zero_term;_LLD: {void*t1a=_tmp3A1;struct Cyc_Absyn_Tqual tq1a=_tmp3A2;struct Cyc_Absyn_Exp*e1=_tmp3A3;void*zt1=_tmp3A4;
# 2318
{void*_tmp3BB=t2;void*_tmp3BF;struct Cyc_Absyn_Exp*_tmp3BE;struct Cyc_Absyn_Tqual _tmp3BD;void*_tmp3BC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BB)->tag == 4U){_LL1E: _tmp3BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BB)->f1).elt_type;_tmp3BD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BB)->f1).tq;_tmp3BE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BB)->f1).num_elts;_tmp3BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BB)->f1).zero_term;_LL1F: {void*t2a=_tmp3BC;struct Cyc_Absyn_Tqual tq2a=_tmp3BD;struct Cyc_Absyn_Exp*e2=_tmp3BE;void*zt2=_tmp3BF;
# 2320
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2325
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2327
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2329
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39E)->f1)){case 15U: _LLE: _tmp3A0=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39E)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp3A0;
# 2333
{void*_tmp3C0=t2;struct Cyc_Absyn_Enumdecl*_tmp3C1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->f1)->tag == 15U){_LL23: _tmp3C1=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3C1;
# 2335
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp737=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp737 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2341
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2344
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39E)->f2 != 0){_LL14: _tmp39F=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39E)->f2)->hd;_LL15: {void*r1=_tmp39F;
# 2347
{void*_tmp3C2=t2;void*_tmp3C3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2)->f2 != 0){_LL28: _tmp3C3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C2)->f2)->hd;_LL29: {void*r2=_tmp3C3;
# 2349
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2353
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2359
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3C4=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3C4;
({void*_tmp738=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3C5=_cycalloc(sizeof(*_tmp3C5));_tmp3C5->tag=14U,_tmp3C5->f1=t,_tmp3C5->f2=inner,_tmp3C5->f3=0,_tmp3C5->f4=c;_tmp3C5;});e->r=_tmp738;});
e->topt=t;}}
# 2368
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2373
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3CB=({struct Cyc_Int_pa_PrintArg_struct _tmp62F;_tmp62F.tag=1U,_tmp62F.f1=(unsigned long)i;_tmp62F;});void*_tmp3C9[1U];_tmp3C9[0]=& _tmp3CB;({struct _fat_ptr _tmp739=({const char*_tmp3CA="#%d";_tag_fat(_tmp3CA,sizeof(char),4U);});Cyc_aprintf(_tmp739,_tag_fat(_tmp3C9,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));({struct _fat_ptr*_tmp73A=({unsigned _tmp3C7=1;struct _fat_ptr*_tmp3C6=_cycalloc(_check_times(_tmp3C7,sizeof(struct _fat_ptr)));_tmp3C6[0]=s;_tmp3C6;});_tmp3C8->name=_tmp73A;}),_tmp3C8->identity=- 1,_tmp3C8->kind=k;_tmp3C8;});}
# 2380
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp3CC=*t->name;struct _fat_ptr s=_tmp3CC;
return(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'#';}
# 2386
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2393
struct Cyc_List_List*_tmp3CD=0;struct Cyc_List_List*fn_type_atts=_tmp3CD;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->hd=(void*)atts->hd,_tmp3CE->tl=fn_type_atts;_tmp3CE;});}}{
struct Cyc_Absyn_FnInfo _tmp3CF=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3CF;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3D0=_cycalloc(sizeof(*_tmp3D0));_tmp3D0->tag=5U,_tmp3D0->f1=type_info;_tmp3D0;});}}
# 2401
return(void*)_check_null(fd->cached_type);}
# 2407
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2409
union Cyc_Relations_RelnOp _tmp3D1=*rop;union Cyc_Relations_RelnOp _stmttmp5F=_tmp3D1;union Cyc_Relations_RelnOp _tmp3D2=_stmttmp5F;struct Cyc_Absyn_Vardecl*_tmp3D3;struct Cyc_Absyn_Vardecl*_tmp3D4;switch((_tmp3D2.RNumelts).tag){case 2U: _LL1: _tmp3D4=(_tmp3D2.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3D4;
# 2411
struct _tuple1 _tmp3D5=*vd->name;struct _tuple1 _stmttmp60=_tmp3D5;struct _tuple1 _tmp3D6=_stmttmp60;struct _fat_ptr*_tmp3D8;union Cyc_Absyn_Nmspace _tmp3D7;_LL8: _tmp3D7=_tmp3D6.f1;_tmp3D8=_tmp3D6.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3D7;struct _fat_ptr*var=_tmp3D8;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp73B=(struct _fat_ptr)*var;Cyc_strcmp(_tmp73B,({const char*_tmp3D9="return_value";_tag_fat(_tmp3D9,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp73C=Cyc_Relations_RReturn();*rop=_tmp73C;});
goto _LL0;}{
# 2417
unsigned c=0U;
{struct Cyc_List_List*_tmp3DA=args;struct Cyc_List_List*a=_tmp3DA;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3DB=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp61=_tmp3DB;struct _tuple9*_tmp3DC=_stmttmp61;struct _fat_ptr*_tmp3DD;_LLB: _tmp3DD=_tmp3DC->f1;_LLC: {struct _fat_ptr*vopt=_tmp3DD;
if(vopt != 0){
if(Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp73D=Cyc_Relations_RParam(c);*rop=_tmp73D;});
break;}}}}}
# 2427
goto _LL0;}}}case 3U: _LL3: _tmp3D3=(_tmp3D2.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3D3;
# 2429
struct _tuple1 _tmp3DE=*vd->name;struct _tuple1 _stmttmp62=_tmp3DE;struct _tuple1 _tmp3DF=_stmttmp62;struct _fat_ptr*_tmp3E1;union Cyc_Absyn_Nmspace _tmp3E0;_LLE: _tmp3E0=_tmp3DF.f1;_tmp3E1=_tmp3DF.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3E0;struct _fat_ptr*var=_tmp3E1;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3E2=args;struct Cyc_List_List*a=_tmp3E2;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3E3=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp63=_tmp3E3;struct _tuple9*_tmp3E4=_stmttmp63;struct _fat_ptr*_tmp3E5;_LL11: _tmp3E5=_tmp3E4->f1;_LL12: {struct _fat_ptr*vopt=_tmp3E5;
if(vopt != 0){
if(Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp73E=Cyc_Relations_RParamNumelts(c);*rop=_tmp73E;});
break;}}}}}
# 2441
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2446
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2448
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2452
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2454
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3E6=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3E6;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2461
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3E7=pr;void*_tmp3E9;struct Cyc_Absyn_Tqual _tmp3E8;_LL1: _tmp3E8=_tmp3E7->f1;_tmp3E9=_tmp3E7->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3E8;void*t2=_tmp3E9;
if(t2 == t)return pr;else{
return({struct _tuple12*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->f1=tq,_tmp3EA->f2=t;_tmp3EA;});}}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2467
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2469
return({struct _tuple27*_tmp3EC=_region_malloc(rgn,sizeof(*_tmp3EC));({struct _tuple26*_tmp73F=({struct _tuple26*_tmp3EB=_region_malloc(rgn,sizeof(*_tmp3EB));_tmp3EB->f1=(*y).f1,_tmp3EB->f2=(*y).f2;_tmp3EB;});_tmp3EC->f1=_tmp73F;}),_tmp3EC->f2=(*y).f3;_tmp3EC;});}
# 2471
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2473
struct _tuple9 _tmp3ED=*orig_arg;struct _tuple9 _stmttmp64=_tmp3ED;struct _tuple9 _tmp3EE=_stmttmp64;void*_tmp3F1;struct Cyc_Absyn_Tqual _tmp3F0;struct _fat_ptr*_tmp3EF;_LL1: _tmp3EF=_tmp3EE.f1;_tmp3F0=_tmp3EE.f2;_tmp3F1=_tmp3EE.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp3EF;struct Cyc_Absyn_Tqual tq_orig=_tmp3F0;void*t_orig=_tmp3F1;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp3F2=_cycalloc(sizeof(*_tmp3F2));_tmp3F2->f1=vopt_orig,_tmp3F2->f2=tq_orig,_tmp3F2->f3=t;_tmp3F2;});}}
# 2477
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2482
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2484
return({struct Cyc_Absyn_Exp*_tmp3F3=_cycalloc(sizeof(*_tmp3F3));_tmp3F3->topt=old->topt,_tmp3F3->r=r,_tmp3F3->loc=old->loc,_tmp3F3->annot=old->annot;_tmp3F3;});}
# 2489
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3F4=e->r;void*_stmttmp65=_tmp3F4;void*_tmp3F5=_stmttmp65;void*_tmp3F6;struct Cyc_List_List*_tmp3F8;void*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F9;struct Cyc_Absyn_Exp*_tmp3FA;void*_tmp3FB;enum Cyc_Absyn_Coercion _tmp3FF;int _tmp3FE;struct Cyc_Absyn_Exp*_tmp3FD;void*_tmp3FC;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp400;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp402;struct Cyc_Absyn_Exp*_tmp405;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_Absyn_Exp*_tmp408;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_List_List*_tmp40A;enum Cyc_Absyn_Primop _tmp409;switch(*((int*)_tmp3F5)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp409=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_tmp40A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F5)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp409;struct Cyc_List_List*es=_tmp40A;
# 2498
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp40B=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp40B;
struct Cyc_Absyn_Exp*_tmp40C=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40C;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp741=e;Cyc_Tcutil_copye(_tmp741,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp40E=_cycalloc(sizeof(*_tmp40E));_tmp40E->tag=3U,_tmp40E->f1=p,({struct Cyc_List_List*_tmp740=({struct Cyc_Absyn_Exp*_tmp40D[1U];_tmp40D[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp40D,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp40E->f2=_tmp740;});_tmp40E;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp40F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp40F;
struct Cyc_Absyn_Exp*_tmp410=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp410;
struct Cyc_Absyn_Exp*_tmp411=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp411;
struct Cyc_Absyn_Exp*_tmp412=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp412;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp743=e;Cyc_Tcutil_copye(_tmp743,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp414=_cycalloc(sizeof(*_tmp414));_tmp414->tag=3U,_tmp414->f1=p,({struct Cyc_List_List*_tmp742=({struct Cyc_Absyn_Exp*_tmp413[2U];_tmp413[0]=new_e1,_tmp413[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp413,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp414->f2=_tmp742;});_tmp414;}));});}else{
return({void*_tmp415=0U;({struct _fat_ptr _tmp744=({const char*_tmp416="primop does not have 1 or 2 args!";_tag_fat(_tmp416,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp744,_tag_fat(_tmp415,sizeof(void*),0U));});});}}}case 6U: _LLD: _tmp406=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_tmp407=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F5)->f2;_tmp408=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F5)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp406;struct Cyc_Absyn_Exp*e2=_tmp407;struct Cyc_Absyn_Exp*e3=_tmp408;
# 2512
struct Cyc_Absyn_Exp*_tmp417=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp417;
struct Cyc_Absyn_Exp*_tmp418=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp418;
struct Cyc_Absyn_Exp*_tmp419=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp419;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp745=e;Cyc_Tcutil_copye(_tmp745,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp41A=_cycalloc(sizeof(*_tmp41A));_tmp41A->tag=6U,_tmp41A->f1=new_e1,_tmp41A->f2=new_e2,_tmp41A->f3=new_e3;_tmp41A;}));});}case 7U: _LLF: _tmp404=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_tmp405=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3F5)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp404;struct Cyc_Absyn_Exp*e2=_tmp405;
# 2518
struct Cyc_Absyn_Exp*_tmp41B=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41B;
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp41C;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp746=e;Cyc_Tcutil_copye(_tmp746,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp41D=_cycalloc(sizeof(*_tmp41D));_tmp41D->tag=7U,_tmp41D->f1=new_e1,_tmp41D->f2=new_e2;_tmp41D;}));});}case 8U: _LL11: _tmp402=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_tmp403=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3F5)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp402;struct Cyc_Absyn_Exp*e2=_tmp403;
# 2523
struct Cyc_Absyn_Exp*_tmp41E=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41E;
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp41F;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp747=e;Cyc_Tcutil_copye(_tmp747,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->tag=8U,_tmp420->f1=new_e1,_tmp420->f2=new_e2;_tmp420;}));});}case 9U: _LL13: _tmp400=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_tmp401=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3F5)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp400;struct Cyc_Absyn_Exp*e2=_tmp401;
# 2528
struct Cyc_Absyn_Exp*_tmp421=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp421;
struct Cyc_Absyn_Exp*_tmp422=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp422;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp748=e;Cyc_Tcutil_copye(_tmp748,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->tag=9U,_tmp423->f1=new_e1,_tmp423->f2=new_e2;_tmp423;}));});}case 14U: _LL15: _tmp3FC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_tmp3FD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F5)->f2;_tmp3FE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F5)->f3;_tmp3FF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F5)->f4;_LL16: {void*t=_tmp3FC;struct Cyc_Absyn_Exp*e1=_tmp3FD;int b=_tmp3FE;enum Cyc_Absyn_Coercion c=_tmp3FF;
# 2533
struct Cyc_Absyn_Exp*_tmp424=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp424;
void*_tmp425=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp425;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp749=e;Cyc_Tcutil_copye(_tmp749,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->tag=14U,_tmp426->f1=new_typ,_tmp426->f2=new_e1,_tmp426->f3=b,_tmp426->f4=c;_tmp426;}));});}case 17U: _LL17: _tmp3FB=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_LL18: {void*t=_tmp3FB;
# 2538
void*_tmp427=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp427;
if(new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp74A=e;Cyc_Tcutil_copye(_tmp74A,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->tag=17U,_tmp428->f1=new_typ;_tmp428;}));});}case 18U: _LL19: _tmp3FA=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp3FA;
# 2542
struct Cyc_Absyn_Exp*_tmp429=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp429;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp74B=e;Cyc_Tcutil_copye(_tmp74B,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A->tag=18U,_tmp42A->f1=new_e1;_tmp42A;}));});}case 41U: _LL1B: _tmp3F9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp3F9;
# 2546
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42B;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp74C=e;Cyc_Tcutil_copye(_tmp74C,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->tag=41U,_tmp42C->f1=new_e1;_tmp42C;}));});}case 19U: _LL1D: _tmp3F7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_tmp3F8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3F5)->f2;_LL1E: {void*t=_tmp3F7;struct Cyc_List_List*f=_tmp3F8;
# 2550
void*_tmp42D=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp42D;
if(new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp74D=e;Cyc_Tcutil_copye(_tmp74D,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp42E=_cycalloc(sizeof(*_tmp42E));_tmp42E->tag=19U,_tmp42E->f1=new_typ,_tmp42E->f2=f;_tmp42E;}));});}case 39U: _LL1F: _tmp3F6=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3F5)->f1;_LL20: {void*t=_tmp3F6;
# 2554
void*_tmp42F=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp42F;
if(new_typ == t)return e;{
# 2557
void*_tmp430=Cyc_Tcutil_compress(new_typ);void*_stmttmp66=_tmp430;void*_tmp431=_stmttmp66;struct Cyc_Absyn_Exp*_tmp432;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp431)->tag == 9U){_LL24: _tmp432=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp431)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp432;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp74E=e;Cyc_Tcutil_copye(_tmp74E,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp433=_cycalloc(sizeof(*_tmp433));_tmp433->tag=39U,_tmp433->f1=new_typ;_tmp433;}));});}_LL23:;}}default: _LL21: _LL22:
# 2561
 return({void*_tmp434=0U;({struct _fat_ptr _tmp74F=({const char*_tmp435="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp435,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp74F,_tag_fat(_tmp434,sizeof(void*),0U));});});}_LL0:;}
# 2565
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2568
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 2572
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2575
void*_tmp436=f->type;void*t=_tmp436;
struct Cyc_Absyn_Exp*_tmp437=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp437;
void*_tmp438=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_t=_tmp438;
struct Cyc_Absyn_Exp*_tmp439=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp439;
if(t == new_t && req == new_req)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp43A=_cycalloc(sizeof(*_tmp43A));_tmp43A->name=f->name,_tmp43A->tq=f->tq,_tmp43A->type=new_t,_tmp43A->width=f->width,_tmp43A->attributes=f->attributes,_tmp43A->requires_clause=new_req;_tmp43A;});}}
# 2585
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2588
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp43B=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp43B;
struct Cyc_List_List*_tmp43C=fs->tl;struct Cyc_List_List*t=_tmp43C;
struct Cyc_Absyn_Aggrfield*_tmp43D=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp43D;
struct Cyc_List_List*_tmp43E=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_t=_tmp43E;
if(new_f == f && new_t == t)return fs;
return({struct Cyc_List_List*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->hd=new_f,_tmp43F->tl=new_t;_tmp43F;});}}
# 2597
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2600
struct _tuple0 _tmp440=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp67=_tmp440;struct _tuple0 _tmp441=_stmttmp67;struct Cyc_List_List*_tmp443;struct Cyc_List_List*_tmp442;_LL1: _tmp442=_tmp441.f1;_tmp443=_tmp441.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp442;struct Cyc_List_List*rpo1b=_tmp443;
struct Cyc_List_List*_tmp444=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp444;
struct Cyc_List_List*_tmp445=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp445;
if(rpo2a == rpo1a && rpo2b == rpo1b)
return rgn_po;else{
# 2606
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}}
# 2609
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2612
void*_tmp446=Cyc_Tcutil_compress(t);void*_stmttmp68=_tmp446;void*_tmp447=_stmttmp68;struct Cyc_Absyn_Exp*_tmp448;struct Cyc_Absyn_Exp*_tmp449;struct Cyc_List_List*_tmp44B;void*_tmp44A;void*_tmp44C;void*_tmp44D;struct Cyc_List_List*_tmp44F;enum Cyc_Absyn_AggrKind _tmp44E;struct Cyc_List_List*_tmp450;struct Cyc_Absyn_Vardecl*_tmp45C;struct Cyc_Absyn_Exp*_tmp45B;struct Cyc_Absyn_Exp*_tmp45A;struct Cyc_List_List*_tmp459;struct Cyc_List_List*_tmp458;struct Cyc_Absyn_VarargInfo*_tmp457;int _tmp456;struct Cyc_List_List*_tmp455;void*_tmp454;struct Cyc_Absyn_Tqual _tmp453;void*_tmp452;struct Cyc_List_List*_tmp451;void*_tmp462;void*_tmp461;void*_tmp460;void*_tmp45F;struct Cyc_Absyn_Tqual _tmp45E;void*_tmp45D;unsigned _tmp467;void*_tmp466;struct Cyc_Absyn_Exp*_tmp465;struct Cyc_Absyn_Tqual _tmp464;void*_tmp463;void*_tmp46B;struct Cyc_Absyn_Typedefdecl*_tmp46A;struct Cyc_List_List*_tmp469;struct _tuple1*_tmp468;struct Cyc_Absyn_Tvar*_tmp46C;switch(*((int*)_tmp447)){case 2U: _LL1: _tmp46C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp447)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp46C;
# 2615
struct _handler_cons _tmp46D;_push_handler(& _tmp46D);{int _tmp46F=0;if(setjmp(_tmp46D.handler))_tmp46F=1;if(!_tmp46F){{void*_tmp470=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp470;};_pop_handler();}else{void*_tmp46E=(void*)Cyc_Core_get_exn_thrown();void*_tmp471=_tmp46E;void*_tmp472;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp471)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp472=_tmp471;_LL1F: {void*exn=_tmp472;(int)_rethrow(exn);}}_LL1B:;}}}case 8U: _LL3: _tmp468=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp447)->f1;_tmp469=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp447)->f2;_tmp46A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp447)->f3;_tmp46B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp447)->f4;_LL4: {struct _tuple1*n=_tmp468;struct Cyc_List_List*ts=_tmp469;struct Cyc_Absyn_Typedefdecl*td=_tmp46A;void*topt=_tmp46B;
# 2618
struct Cyc_List_List*_tmp473=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp473;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp474=_cycalloc(sizeof(*_tmp474));_tmp474->tag=8U,_tmp474->f1=n,_tmp474->f2=new_ts,_tmp474->f3=td,_tmp474->f4=topt;_tmp474;});}case 4U: _LL5: _tmp463=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp447)->f1).elt_type;_tmp464=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp447)->f1).tq;_tmp465=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp447)->f1).num_elts;_tmp466=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp447)->f1).zero_term;_tmp467=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp447)->f1).zt_loc;_LL6: {void*t1=_tmp463;struct Cyc_Absyn_Tqual tq=_tmp464;struct Cyc_Absyn_Exp*e=_tmp465;void*zt=_tmp466;unsigned ztl=_tmp467;
# 2621
void*_tmp475=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp475;
struct Cyc_Absyn_Exp*_tmp476=e == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp476;
void*_tmp477=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp477;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp478=_cycalloc(sizeof(*_tmp478));
_tmp478->tag=4U,(_tmp478->f1).elt_type=new_t1,(_tmp478->f1).tq=tq,(_tmp478->f1).num_elts=new_e,(_tmp478->f1).zero_term=new_zt,(_tmp478->f1).zt_loc=ztl;_tmp478;});}case 3U: _LL7: _tmp45D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447)->f1).elt_type;_tmp45E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447)->f1).elt_tq;_tmp45F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447)->f1).ptr_atts).rgn;_tmp460=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447)->f1).ptr_atts).nullable;_tmp461=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447)->f1).ptr_atts).bounds;_tmp462=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp447)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp45D;struct Cyc_Absyn_Tqual tq=_tmp45E;void*r=_tmp45F;void*n=_tmp460;void*b=_tmp461;void*zt=_tmp462;
# 2627
void*_tmp479=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp479;
void*_tmp47A=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp47A;
void*_tmp47B=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp47B;
void*_tmp47C=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp47C;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp630;_tmp630.elt_type=new_t1,_tmp630.elt_tq=tq,(_tmp630.ptr_atts).rgn=new_r,(_tmp630.ptr_atts).nullable=n,(_tmp630.ptr_atts).bounds=new_b,(_tmp630.ptr_atts).zero_term=new_zt,(_tmp630.ptr_atts).ptrloc=0;_tmp630;}));}case 5U: _LL9: _tmp451=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).tvars;_tmp452=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).effect;_tmp453=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).ret_tqual;_tmp454=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).ret_type;_tmp455=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).args;_tmp456=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).c_varargs;_tmp457=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).cyc_varargs;_tmp458=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).rgn_po;_tmp459=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).attributes;_tmp45A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).requires_clause;_tmp45B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).ensures_clause;_tmp45C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp447)->f1).return_value;_LLA: {struct Cyc_List_List*vs=_tmp451;void*eff=_tmp452;struct Cyc_Absyn_Tqual rtq=_tmp453;void*rtyp=_tmp454;struct Cyc_List_List*args=_tmp455;int c_varargs=_tmp456;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp457;struct Cyc_List_List*rgn_po=_tmp458;struct Cyc_List_List*atts=_tmp459;struct Cyc_Absyn_Exp*req=_tmp45A;struct Cyc_Absyn_Exp*ens=_tmp45B;struct Cyc_Absyn_Vardecl*ret_value=_tmp45C;
# 2637
{struct Cyc_List_List*_tmp47D=vs;struct Cyc_List_List*p=_tmp47D;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp47F=_region_malloc(rgn,sizeof(*_tmp47F));({struct _tuple19*_tmp751=({struct _tuple19*_tmp47E=_region_malloc(rgn,sizeof(*_tmp47E));_tmp47E->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp750=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp47E->f2=_tmp750;});_tmp47E;});_tmp47F->hd=_tmp751;}),_tmp47F->tl=inst;_tmp47F;});}}{
struct _tuple0 _tmp480=({struct _RegionHandle*_tmp753=rgn;struct _RegionHandle*_tmp752=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp753,_tmp752,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2639
struct _tuple0 _stmttmp69=_tmp480;struct _tuple0 _tmp481=_stmttmp69;struct Cyc_List_List*_tmp483;struct Cyc_List_List*_tmp482;_LL21: _tmp482=_tmp481.f1;_tmp483=_tmp481.f2;_LL22: {struct Cyc_List_List*qs=_tmp482;struct Cyc_List_List*ts=_tmp483;
# 2641
struct Cyc_List_List*_tmp484=args;struct Cyc_List_List*args2=_tmp484;
struct Cyc_List_List*_tmp485=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp485;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2;
if(eff == 0)
eff2=0;else{
# 2649
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,eff);
if(new_eff == eff)
eff2=eff;else{
# 2653
eff2=new_eff;}}{
# 2655
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2659
struct Cyc_Absyn_VarargInfo _tmp486=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp6A=_tmp486;struct Cyc_Absyn_VarargInfo _tmp487=_stmttmp6A;int _tmp48B;void*_tmp48A;struct Cyc_Absyn_Tqual _tmp489;struct _fat_ptr*_tmp488;_LL24: _tmp488=_tmp487.name;_tmp489=_tmp487.tq;_tmp48A=_tmp487.type;_tmp48B=_tmp487.inject;_LL25: {struct _fat_ptr*n=_tmp488;struct Cyc_Absyn_Tqual tq=_tmp489;void*t=_tmp48A;int i=_tmp48B;
void*_tmp48C=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp48C;
if(t2 == t)cyc_varargs2=cyc_varargs;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->name=n,_tmp48D->tq=tq,_tmp48D->type=t2,_tmp48D->inject=i;_tmp48D;});}}}{
# 2664
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp48E=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp48E;
struct Cyc_List_List*_tmp48F=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp48F;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->tag=5U,(_tmp490->f1).tvars=vs,(_tmp490->f1).effect=eff2,(_tmp490->f1).ret_tqual=rtq,({void*_tmp754=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp490->f1).ret_type=_tmp754;}),(_tmp490->f1).args=args2,(_tmp490->f1).c_varargs=c_varargs,(_tmp490->f1).cyc_varargs=cyc_varargs2,(_tmp490->f1).rgn_po=rgn_po2,(_tmp490->f1).attributes=atts,(_tmp490->f1).requires_clause=req2,(_tmp490->f1).requires_relns=req_relns2,(_tmp490->f1).ensures_clause=ens2,(_tmp490->f1).ensures_relns=ens_relns2,(_tmp490->f1).return_value=ret_value;_tmp490;});}}}}}}case 6U: _LLB: _tmp450=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp447)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp450;
# 2673
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp491=tqts;struct Cyc_List_List*ts1=_tmp491;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp492=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp492;
void*_tmp493=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp493;
if(t1 != t2)
change=1;
# 2681
ts2=({struct Cyc_List_List*_tmp494=_region_malloc(rgn,sizeof(*_tmp494));_tmp494->hd=t2,_tmp494->tl=ts2;_tmp494;});}}
# 2683
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp495=_cycalloc(sizeof(*_tmp495));_tmp495->tag=6U,({struct Cyc_List_List*_tmp756=({struct Cyc_List_List*_tmp755=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp755,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp495->f1=_tmp756;});_tmp495;});}case 7U: _LLD: _tmp44E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp447)->f1;_tmp44F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp447)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp44E;struct Cyc_List_List*fs=_tmp44F;
# 2687
struct Cyc_List_List*_tmp496=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp496;
if(fs == new_fs)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp497=_cycalloc(sizeof(*_tmp497));_tmp497->tag=7U,_tmp497->f1=k,_tmp497->f2=new_fs;_tmp497;});}case 1U: _LLF: _tmp44D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp447)->f2;_LL10: {void*r=_tmp44D;
# 2691
if(r != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,r);else{
return t;}}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f2 == 0){_LL11: _tmp44C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f1;_LL12: {void*c=_tmp44C;
return t;}}else{_LL13: _tmp44A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f1;_tmp44B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp447)->f2;_LL14: {void*c=_tmp44A;struct Cyc_List_List*ts=_tmp44B;
# 2695
struct Cyc_List_List*_tmp498=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp498;
if(ts == new_ts)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp499=_cycalloc(sizeof(*_tmp499));_tmp499->tag=0U,_tmp499->f1=c,_tmp499->f2=new_ts;_tmp499;});}}}case 9U: _LL15: _tmp449=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp447)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp449;
# 2699
struct Cyc_Absyn_Exp*_tmp49A=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp49A;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->tag=9U,_tmp49B->f1=new_e;_tmp49B;});}case 11U: _LL17: _tmp448=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp447)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp448;
# 2702
struct Cyc_Absyn_Exp*_tmp49C=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp49C;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->tag=11U,_tmp49D->f1=new_e;_tmp49D;});}default: _LL19: _LL1A:
({void*_tmp49E=0U;({struct _fat_ptr _tmp757=({const char*_tmp49F="found typedecltype in rsubs";_tag_fat(_tmp49F,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp757,_tag_fat(_tmp49E,sizeof(void*),0U));});});}_LL0:;}
# 2708
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2711
if(ts == 0)
return 0;{
void*_tmp4A0=(void*)ts->hd;void*old_hd=_tmp4A0;
struct Cyc_List_List*_tmp4A1=ts->tl;struct Cyc_List_List*old_tl=_tmp4A1;
void*_tmp4A2=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp4A2;
struct Cyc_List_List*_tmp4A3=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp4A3;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->hd=new_hd,_tmp4A4->tl=new_tl;_tmp4A4;});}}
# 2722
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 2729
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4A5=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4A5;
return({struct _tuple19*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->f1=tv,({void*_tmp759=({struct Cyc_Core_Opt*_tmp758=k;Cyc_Absyn_new_evar(_tmp758,({struct Cyc_Core_Opt*_tmp4A6=_cycalloc(sizeof(*_tmp4A6));_tmp4A6->v=s;_tmp4A6;}));});_tmp4A7->f2=_tmp759;});_tmp4A7;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2734
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2736
struct _tuple28*_tmp4A8=env;struct _RegionHandle*_tmp4AA;struct Cyc_List_List*_tmp4A9;_LL1: _tmp4A9=_tmp4A8->f1;_tmp4AA=_tmp4A8->f2;_LL2: {struct Cyc_List_List*s=_tmp4A9;struct _RegionHandle*rgn=_tmp4AA;
struct Cyc_Core_Opt*_tmp4AB=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4AB;
return({struct _tuple19*_tmp4AD=_region_malloc(rgn,sizeof(*_tmp4AD));_tmp4AD->f1=tv,({void*_tmp75B=({struct Cyc_Core_Opt*_tmp75A=k;Cyc_Absyn_new_evar(_tmp75A,({struct Cyc_Core_Opt*_tmp4AC=_cycalloc(sizeof(*_tmp4AC));_tmp4AC->v=s;_tmp4AC;}));});_tmp4AD->f2=_tmp75B;});_tmp4AD;});}}
# 2741
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2743
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp4B0=({struct Cyc_String_pa_PrintArg_struct _tmp631;_tmp631.tag=0U,_tmp631.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp631;});void*_tmp4AE[1U];_tmp4AE[0]=& _tmp4B0;({unsigned _tmp75D=loc;struct _fat_ptr _tmp75C=({const char*_tmp4AF="bitfield %s does not have constant width";_tag_fat(_tmp4AF,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp75D,_tmp75C,_tag_fat(_tmp4AE,sizeof(void*),1U));});});else{
# 2748
struct _tuple13 _tmp4B1=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp6B=_tmp4B1;struct _tuple13 _tmp4B2=_stmttmp6B;int _tmp4B4;unsigned _tmp4B3;_LL1: _tmp4B3=_tmp4B2.f1;_tmp4B4=_tmp4B2.f2;_LL2: {unsigned i=_tmp4B3;int known=_tmp4B4;
if(!known)
({void*_tmp4B5=0U;({unsigned _tmp75F=loc;struct _fat_ptr _tmp75E=({const char*_tmp4B6="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4B6,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp75F,_tmp75E,_tag_fat(_tmp4B5,sizeof(void*),0U));});});
if((int)i < 0)
({void*_tmp4B7=0U;({unsigned _tmp761=loc;struct _fat_ptr _tmp760=({const char*_tmp4B8="bitfield has negative width";_tag_fat(_tmp4B8,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp761,_tmp760,_tag_fat(_tmp4B7,sizeof(void*),0U));});});
w=i;}}{
# 2755
void*_tmp4B9=Cyc_Tcutil_compress(field_type);void*_stmttmp6C=_tmp4B9;void*_tmp4BA=_stmttmp6C;enum Cyc_Absyn_Size_of _tmp4BB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4BA)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4BA)->f1)->tag == 1U){_LL4: _tmp4BB=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4BA)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4BB;
# 2758
{enum Cyc_Absyn_Size_of _tmp4BC=b;switch(_tmp4BC){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp4BD=0U;({unsigned _tmp763=loc;struct _fat_ptr _tmp762=({const char*_tmp4BE="bitfield larger than type";_tag_fat(_tmp4BE,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp763,_tmp762,_tag_fat(_tmp4BD,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp4BF=0U;({unsigned _tmp765=loc;struct _fat_ptr _tmp764=({const char*_tmp4C0="bitfield larger than type";_tag_fat(_tmp4C0,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp765,_tmp764,_tag_fat(_tmp4BF,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp4C1=0U;({unsigned _tmp767=loc;struct _fat_ptr _tmp766=({const char*_tmp4C2="bitfield larger than type";_tag_fat(_tmp4C2,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp767,_tmp766,_tag_fat(_tmp4C1,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp4C3=0U;({unsigned _tmp769=loc;struct _fat_ptr _tmp768=({const char*_tmp4C4="bitfield larger than type";_tag_fat(_tmp4C4,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp769,_tmp768,_tag_fat(_tmp4C3,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 2766
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2768
({struct Cyc_String_pa_PrintArg_struct _tmp4C7=({struct Cyc_String_pa_PrintArg_struct _tmp633;_tmp633.tag=0U,_tmp633.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp633;});struct Cyc_String_pa_PrintArg_struct _tmp4C8=({struct Cyc_String_pa_PrintArg_struct _tmp632;_tmp632.tag=0U,({
struct _fat_ptr _tmp76A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field_type));_tmp632.f1=_tmp76A;});_tmp632;});void*_tmp4C5[2U];_tmp4C5[0]=& _tmp4C7,_tmp4C5[1]=& _tmp4C8;({unsigned _tmp76C=loc;struct _fat_ptr _tmp76B=({const char*_tmp4C6="bitfield %s must have integral type but has type %s";_tag_fat(_tmp4C6,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp76C,_tmp76B,_tag_fat(_tmp4C5,sizeof(void*),2U));});});
goto _LL3;}_LL3:;}}}
# 2777
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4C9=t;void*_tmp4CB;struct Cyc_Absyn_Typedefdecl*_tmp4CA;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->tag == 8U){_LL1: _tmp4CA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f3;_tmp4CB=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C9)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4CA;void*tdopt=_tmp4CB;
# 2780
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({void*_tmp4CC=0U;({unsigned _tmp76E=loc;struct _fat_ptr _tmp76D=({const char*_tmp4CD="extra const";_tag_fat(_tmp4CD,sizeof(char),12U);});Cyc_Tcutil_warn(_tmp76E,_tmp76D,_tag_fat(_tmp4CC,sizeof(void*),0U));});});
return 1;}
# 2785
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);else{
return declared_const;}}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2792
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp76F=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp76F;});}
# 2796
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2802
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2805
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4D0=({struct Cyc_String_pa_PrintArg_struct _tmp635;_tmp635.tag=0U,_tmp635.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp635;});struct Cyc_String_pa_PrintArg_struct _tmp4D1=({struct Cyc_String_pa_PrintArg_struct _tmp634;_tmp634.tag=0U,({struct _fat_ptr _tmp770=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp634.f1=_tmp770;});_tmp634;});void*_tmp4CE[2U];_tmp4CE[0]=& _tmp4D0,_tmp4CE[1]=& _tmp4D1;({unsigned _tmp772=loc;struct _fat_ptr _tmp771=({const char*_tmp4CF="%s: %s";_tag_fat(_tmp4CF,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp772,_tmp771,_tag_fat(_tmp4CE,sizeof(void*),2U));});});}}}
# 2811
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2815
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2819
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp774=tvs;unsigned _tmp773=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp774,_tmp773,Cyc_Absynpp_tvar2string,({const char*_tmp4D2="duplicate type variable";_tag_fat(_tmp4D2,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2833 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2838
struct _RegionHandle _tmp4D3=_new_region("temp");struct _RegionHandle*temp=& _tmp4D3;_push_region(temp);
# 2842
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp775=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp775,({const char*_tmp4D4="";_tag_fat(_tmp4D4,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4D6=_region_malloc(temp,sizeof(*_tmp4D6));({struct _tuple29*_tmp776=({struct _tuple29*_tmp4D5=_region_malloc(temp,sizeof(*_tmp4D5));_tmp4D5->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4D5->f2=0;_tmp4D5;});_tmp4D6->hd=_tmp776;}),_tmp4D6->tl=fields;_tmp4D6;});}}
# 2847
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2849
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp4FB="struct";_tag_fat(_tmp4FB,sizeof(char),7U);}):({const char*_tmp4FC="union";_tag_fat(_tmp4FC,sizeof(char),6U);});
# 2852
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4D7=(struct _tuple30*)des->hd;struct _tuple30*_stmttmp6D=_tmp4D7;struct _tuple30*_tmp4D8=_stmttmp6D;void*_tmp4DA;struct Cyc_List_List*_tmp4D9;_LL1: _tmp4D9=_tmp4D8->f1;_tmp4DA=_tmp4D8->f2;_LL2: {struct Cyc_List_List*dl=_tmp4D9;void*a=_tmp4DA;
if(dl == 0){
# 2857
struct Cyc_List_List*_tmp4DB=fields;struct Cyc_List_List*fields2=_tmp4DB;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple29*)fields2->hd)).f2){
(*((struct _tuple29*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp778=({struct Cyc_List_List*_tmp4DD=_cycalloc(sizeof(*_tmp4DD));({void*_tmp777=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4DC=_cycalloc(sizeof(*_tmp4DC));_tmp4DC->tag=1U,_tmp4DC->f1=((*((struct _tuple29*)fields2->hd)).f1)->name;_tmp4DC;});_tmp4DD->hd=_tmp777;}),_tmp4DD->tl=0;_tmp4DD;});(*((struct _tuple30*)des->hd)).f1=_tmp778;});
ans=({struct Cyc_List_List*_tmp4DF=_region_malloc(rgn,sizeof(*_tmp4DF));({struct _tuple31*_tmp779=({struct _tuple31*_tmp4DE=_region_malloc(rgn,sizeof(*_tmp4DE));_tmp4DE->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4DE->f2=a;_tmp4DE;});_tmp4DF->hd=_tmp779;}),_tmp4DF->tl=ans;_tmp4DF;});
break;}}
# 2865
if(fields2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4E2=({struct Cyc_String_pa_PrintArg_struct _tmp636;_tmp636.tag=0U,_tmp636.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp636;});void*_tmp4E0[1U];_tmp4E0[0]=& _tmp4E2;({unsigned _tmp77B=loc;struct _fat_ptr _tmp77A=({const char*_tmp4E1="too many arguments to %s";_tag_fat(_tmp4E1,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp77B,_tmp77A,_tag_fat(_tmp4E0,sizeof(void*),1U));});});}else{
if(dl->tl != 0)
# 2869
({void*_tmp4E3=0U;({unsigned _tmp77D=loc;struct _fat_ptr _tmp77C=({const char*_tmp4E4="multiple designators are not yet supported";_tag_fat(_tmp4E4,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp77D,_tmp77C,_tag_fat(_tmp4E3,sizeof(void*),0U));});});else{
# 2872
void*_tmp4E5=(void*)dl->hd;void*_stmttmp6E=_tmp4E5;void*_tmp4E6=_stmttmp6E;struct _fat_ptr*_tmp4E7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4E6)->tag == 0U){_LL4: _LL5:
# 2874
({struct Cyc_String_pa_PrintArg_struct _tmp4EA=({struct Cyc_String_pa_PrintArg_struct _tmp637;_tmp637.tag=0U,_tmp637.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp637;});void*_tmp4E8[1U];_tmp4E8[0]=& _tmp4EA;({unsigned _tmp77F=loc;struct _fat_ptr _tmp77E=({const char*_tmp4E9="array designator used in argument to %s";_tag_fat(_tmp4E9,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp77F,_tmp77E,_tag_fat(_tmp4E8,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp4E7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4E6)->f1;_LL7: {struct _fat_ptr*v=_tmp4E7;
# 2877
struct Cyc_List_List*_tmp4EB=fields;struct Cyc_List_List*fields2=_tmp4EB;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple29*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple29*)fields2->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp4EE=({struct Cyc_String_pa_PrintArg_struct _tmp638;_tmp638.tag=0U,_tmp638.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp638;});void*_tmp4EC[1U];_tmp4EC[0]=& _tmp4EE;({unsigned _tmp781=loc;struct _fat_ptr _tmp780=({const char*_tmp4ED="member %s has already been used as an argument";_tag_fat(_tmp4ED,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp781,_tmp780,_tag_fat(_tmp4EC,sizeof(void*),1U));});});
(*((struct _tuple29*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4F0=_region_malloc(rgn,sizeof(*_tmp4F0));({struct _tuple31*_tmp782=({struct _tuple31*_tmp4EF=_region_malloc(rgn,sizeof(*_tmp4EF));_tmp4EF->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4EF->f2=a;_tmp4EF;});_tmp4F0->hd=_tmp782;}),_tmp4F0->tl=ans;_tmp4F0;});
break;}}
# 2886
if(fields2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4F3=({struct Cyc_String_pa_PrintArg_struct _tmp639;_tmp639.tag=0U,_tmp639.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp639;});void*_tmp4F1[1U];_tmp4F1[0]=& _tmp4F3;({unsigned _tmp784=loc;struct _fat_ptr _tmp783=({const char*_tmp4F2="bad field designator %s";_tag_fat(_tmp4F2,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp784,_tmp783,_tag_fat(_tmp4F1,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}}
# 2891
if((int)aggr_kind == (int)0U)
# 2893
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple29*)fields->hd)).f2){
({void*_tmp4F4=0U;({unsigned _tmp786=loc;struct _fat_ptr _tmp785=({const char*_tmp4F5="too few arguments to struct";_tag_fat(_tmp4F5,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp786,_tmp785,_tag_fat(_tmp4F4,sizeof(void*),0U));});});
break;}}else{
# 2900
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple29*)fields->hd)).f2){
if(found)({void*_tmp4F6=0U;({unsigned _tmp788=loc;struct _fat_ptr _tmp787=({const char*_tmp4F7="only one member of a union is allowed";_tag_fat(_tmp4F7,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp788,_tmp787,_tag_fat(_tmp4F6,sizeof(void*),0U));});});
found=1;}}
# 2906
if(!found)({void*_tmp4F8=0U;({unsigned _tmp78A=loc;struct _fat_ptr _tmp789=({const char*_tmp4F9="missing member for union";_tag_fat(_tmp4F9,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp78A,_tmp789,_tag_fat(_tmp4F8,sizeof(void*),0U));});});}{
# 2909
struct Cyc_List_List*_tmp4FA=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp4FA;}}}
# 2842
;_pop_region();}
# 2915
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2917
void*_tmp4FD=e1->r;void*_stmttmp6F=_tmp4FD;void*_tmp4FE=_stmttmp6F;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp501;struct Cyc_Absyn_Exp*_tmp502;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*_tmp504;switch(*((int*)_tmp4FE)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp507=({struct Cyc_String_pa_PrintArg_struct _tmp63A;_tmp63A.tag=0U,({struct _fat_ptr _tmp78B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp63A.f1=_tmp78B;});_tmp63A;});void*_tmp505[1U];_tmp505[0]=& _tmp507;({struct _fat_ptr _tmp78C=({const char*_tmp506="we have a cast in a lhs:  %s";_tag_fat(_tmp506,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78C,_tag_fat(_tmp505,sizeof(void*),1U));});});case 20U: _LL3: _tmp504=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4FE)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp504;
_tmp503=e1a;goto _LL6;}case 23U: _LL5: _tmp503=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4FE)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp503;
# 2921
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp502=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4FE)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp502;
_tmp501=e1a;goto _LLA;}case 21U: _LL9: _tmp501=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4FE)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp501;
# 2925
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp50A=({struct Cyc_String_pa_PrintArg_struct _tmp63B;_tmp63B.tag=0U,({
struct _fat_ptr _tmp78D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp63B.f1=_tmp78D;});_tmp63B;});void*_tmp508[1U];_tmp508[0]=& _tmp50A;({struct _fat_ptr _tmp78E=({const char*_tmp509="found zero pointer aggregate member assignment: %s";_tag_fat(_tmp509,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78E,_tag_fat(_tmp508,sizeof(void*),1U));});});
return 0;}case 13U: _LLB: _tmp500=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4FE)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp500;
_tmp4FF=e1a;goto _LLE;}case 12U: _LLD: _tmp4FF=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4FE)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp4FF;
# 2931
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp50D=({struct Cyc_String_pa_PrintArg_struct _tmp63C;_tmp63C.tag=0U,({
struct _fat_ptr _tmp78F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp63C.f1=_tmp78F;});_tmp63C;});void*_tmp50B[1U];_tmp50B[0]=& _tmp50D;({struct _fat_ptr _tmp790=({const char*_tmp50C="found zero pointer instantiate/noinstantiate: %s";_tag_fat(_tmp50C,sizeof(char),49U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp790,_tag_fat(_tmp50B,sizeof(void*),1U));});});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp510=({struct Cyc_String_pa_PrintArg_struct _tmp63D;_tmp63D.tag=0U,({struct _fat_ptr _tmp791=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp63D.f1=_tmp791;});_tmp63D;});void*_tmp50E[1U];_tmp50E[0]=& _tmp510;({struct _fat_ptr _tmp792=({const char*_tmp50F="found bad lhs in is_zero_ptr_deref: %s";_tag_fat(_tmp50F,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp792,_tag_fat(_tmp50E,sizeof(void*),1U));});});}_LL0:;}
# 2946
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 2949
void*_tmp511=Cyc_Tcutil_compress(r);void*_stmttmp70=_tmp511;void*_tmp512=_stmttmp70;struct Cyc_Absyn_Tvar*_tmp513;enum Cyc_Absyn_AliasQual _tmp515;enum Cyc_Absyn_KindQual _tmp514;switch(*((int*)_tmp512)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp512)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp512)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp512)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp512)->f4 == 0){_LL5: _tmp514=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp512)->f3)->kind)->v)->kind;_tmp515=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp512)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp514;enum Cyc_Absyn_AliasQual a=_tmp515;
# 2953
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp513=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp512)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp513;
# 2957
struct Cyc_Absyn_Kind*_tmp516=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp71=_tmp516;struct Cyc_Absyn_Kind*_tmp517=_stmttmp71;enum Cyc_Absyn_AliasQual _tmp519;enum Cyc_Absyn_KindQual _tmp518;_LLC: _tmp518=_tmp517->kind;_tmp519=_tmp517->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp518;enum Cyc_Absyn_AliasQual a=_tmp519;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp51A=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp72=_tmp51A;void*_tmp51B=_stmttmp72;struct Cyc_Core_Opt**_tmp51C;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51B)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51B)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51B)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp51C=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51B)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp51C;
# 2961
({struct Cyc_Core_Opt*_tmp794=({struct Cyc_Core_Opt*_tmp51E=_cycalloc(sizeof(*_tmp51E));({void*_tmp793=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->tag=2U,_tmp51D->f1=0,_tmp51D->f2=& Cyc_Tcutil_rk;_tmp51D;});_tmp51E->v=_tmp793;});_tmp51E;});*x=_tmp794;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2966
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2973
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp51F=Cyc_Tcutil_compress(t);void*_stmttmp73=_tmp51F;void*_tmp520=_stmttmp73;struct Cyc_Absyn_Tvar*_tmp521;void*_tmp522;switch(*((int*)_tmp520)){case 3U: _LL1: _tmp522=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp520)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp522;
# 2976
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp521=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp520)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp521;
# 2978
struct Cyc_Absyn_Kind*_tmp523=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp74=_tmp523;struct Cyc_Absyn_Kind*_tmp524=_stmttmp74;enum Cyc_Absyn_AliasQual _tmp526;enum Cyc_Absyn_KindQual _tmp525;_LL8: _tmp525=_tmp524->kind;_tmp526=_tmp524->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp525;enum Cyc_Absyn_AliasQual a=_tmp526;
enum Cyc_Absyn_KindQual _tmp527=k;switch(_tmp527){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp528=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp75=_tmp528;void*_tmp529=_stmttmp75;enum Cyc_Absyn_KindQual _tmp52B;struct Cyc_Core_Opt**_tmp52A;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp529)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp529)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp52A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp529)->f1;_tmp52B=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp529)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp52A;enum Cyc_Absyn_KindQual k=_tmp52B;
# 2984
({struct Cyc_Core_Opt*_tmp797=({struct Cyc_Core_Opt*_tmp52E=_cycalloc(sizeof(*_tmp52E));({void*_tmp796=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52D=_cycalloc(sizeof(*_tmp52D));_tmp52D->tag=2U,_tmp52D->f1=0,({struct Cyc_Absyn_Kind*_tmp795=({struct Cyc_Absyn_Kind*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->kind=k,_tmp52C->aliasqual=Cyc_Absyn_Aliasable;_tmp52C;});_tmp52D->f2=_tmp795;});_tmp52D;});_tmp52E->v=_tmp796;});_tmp52E;});*x=_tmp797;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
# 2988
 return 1;}_LL13:;}
# 2991
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2994
 return 0;}_LL0:;}
# 2997
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp52F=Cyc_Tcutil_compress(t);void*t=_tmp52F;
if(Cyc_Tcutil_is_noalias_pointer(t,0))return 1;{
void*_tmp530=t;struct Cyc_List_List*_tmp531;struct Cyc_List_List*_tmp533;union Cyc_Absyn_DatatypeFieldInfo _tmp532;struct Cyc_List_List*_tmp535;union Cyc_Absyn_DatatypeInfo _tmp534;struct Cyc_List_List*_tmp537;struct Cyc_Absyn_Aggrdecl**_tmp536;struct Cyc_List_List*_tmp538;switch(*((int*)_tmp530)){case 6U: _LL1: _tmp538=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp530)->f1;_LL2: {struct Cyc_List_List*qts=_tmp538;
# 3002
while(qts != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))return 1;
qts=qts->tl;}
# 3006
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp536=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f1)->f1).KnownAggr).val;_tmp537=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp536;struct Cyc_List_List*ts=_tmp537;
# 3008
if((*adp)->impl == 0)return 0;else{
# 3010
struct Cyc_List_List*_tmp539=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp539;
struct Cyc_List_List*_tmp53A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp53A;
void*t;
while(x != 0){
t=inst == 0?((struct Cyc_Absyn_Aggrfield*)x->hd)->type: Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
x=x->tl;}
# 3018
return 0;}}}else{_LL7: _LL8:
# 3028
 return 0;}case 18U: _LL9: _tmp534=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f1)->f1;_tmp535=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f2;_LLA: {union Cyc_Absyn_DatatypeInfo tinfo=_tmp534;struct Cyc_List_List*ts=_tmp535;
# 3030
union Cyc_Absyn_DatatypeInfo _tmp53B=tinfo;struct Cyc_Core_Opt*_tmp53D;struct Cyc_List_List*_tmp53C;int _tmp53F;struct _tuple1*_tmp53E;if((_tmp53B.UnknownDatatype).tag == 1){_LL10: _tmp53E=((_tmp53B.UnknownDatatype).val).name;_tmp53F=((_tmp53B.UnknownDatatype).val).is_extensible;_LL11: {struct _tuple1*nm=_tmp53E;int isxt=_tmp53F;
# 3033
return 0;}}else{_LL12: _tmp53C=(*(_tmp53B.KnownDatatype).val)->tvs;_tmp53D=(*(_tmp53B.KnownDatatype).val)->fields;_LL13: {struct Cyc_List_List*tvs=_tmp53C;struct Cyc_Core_Opt*flds=_tmp53D;
# 3036
return 0;}}_LLF:;}case 19U: _LLB: _tmp532=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f1)->f1;_tmp533=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp530)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp532;struct Cyc_List_List*ts=_tmp533;
# 3039
union Cyc_Absyn_DatatypeFieldInfo _tmp540=tinfo;struct Cyc_Absyn_Datatypefield*_tmp542;struct Cyc_Absyn_Datatypedecl*_tmp541;if((_tmp540.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 3042
 return 0;}else{_LL17: _tmp541=((_tmp540.KnownDatatypefield).val).f1;_tmp542=((_tmp540.KnownDatatypefield).val).f2;_LL18: {struct Cyc_Absyn_Datatypedecl*td=_tmp541;struct Cyc_Absyn_Datatypefield*fld=_tmp542;
# 3044
struct Cyc_List_List*_tmp543=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp543;
struct Cyc_List_List*_tmp544=fld->typs;struct Cyc_List_List*typs=_tmp544;
while(typs != 0){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
typs=typs->tl;}
# 3051
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp531=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp530)->f2;_LL6: {struct Cyc_List_List*x=_tmp531;
# 3021
while(x != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))return 1;
x=x->tl;}
# 3025
return 0;}default: _LLD: _LLE:
# 3053
 return 0;}_LL0:;}}
# 3060
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp545=e->r;void*_stmttmp76=_tmp545;void*_tmp546=_stmttmp76;struct Cyc_Absyn_Stmt*_tmp547;struct Cyc_Absyn_Exp*_tmp548;struct Cyc_Absyn_Exp*_tmp549;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54C;struct _fat_ptr*_tmp54F;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp551;switch(*((int*)_tmp546)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp546)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp551=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp551;
_tmp550=e1;goto _LL6;}case 20U: _LL5: _tmp550=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp550;
# 3065
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp54E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_tmp54F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp54E;struct _fat_ptr*f=_tmp54F;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp54C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_tmp54D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp54C;struct Cyc_Absyn_Exp*e2=_tmp54D;
# 3068
void*_tmp552=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp77=_tmp552;void*_tmp553=_stmttmp77;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp553)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp54A=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_tmp54B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp546)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp54A;struct Cyc_Absyn_Exp*e2=_tmp54B;
# 3073
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp549=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp549;
_tmp548=e2;goto _LL10;}case 14U: _LLF: _tmp548=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp546)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp548;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp547=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp546)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp547;
# 3077
while(1){
void*_tmp554=s->r;void*_stmttmp78=_tmp554;void*_tmp555=_stmttmp78;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Stmt*_tmp558;struct Cyc_Absyn_Decl*_tmp557;struct Cyc_Absyn_Stmt*_tmp55A;struct Cyc_Absyn_Stmt*_tmp559;switch(*((int*)_tmp555)){case 2U: _LL1B: _tmp559=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp555)->f1;_tmp55A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp555)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp559;struct Cyc_Absyn_Stmt*s2=_tmp55A;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp557=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp555)->f1;_tmp558=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp555)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp557;struct Cyc_Absyn_Stmt*s1=_tmp558;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp556=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp555)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp556;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({void*_tmp55B=0U;({struct _fat_ptr _tmp798=({const char*_tmp55C="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp55C,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp798,_tag_fat(_tmp55B,sizeof(void*),0U));});});}_LL1A:;}}default: _LL13: _LL14:
# 3085
 return 1;}_LL0:;}
# 3102 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 3104
struct _tuple14 bogus_ans=({struct _tuple14 _tmp64A;_tmp64A.f1=0,_tmp64A.f2=Cyc_Absyn_heap_rgn_type;_tmp64A;});
void*_tmp55D=e->r;void*_stmttmp79=_tmp55D;void*_tmp55E=_stmttmp79;struct Cyc_Absyn_Exp*_tmp560;struct Cyc_Absyn_Exp*_tmp55F;struct Cyc_Absyn_Exp*_tmp561;int _tmp564;struct _fat_ptr*_tmp563;struct Cyc_Absyn_Exp*_tmp562;int _tmp567;struct _fat_ptr*_tmp566;struct Cyc_Absyn_Exp*_tmp565;void*_tmp568;switch(*((int*)_tmp55E)){case 1U: _LL1: _tmp568=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp55E)->f1;_LL2: {void*x=_tmp568;
# 3108
void*_tmp569=x;struct Cyc_Absyn_Vardecl*_tmp56A;struct Cyc_Absyn_Vardecl*_tmp56B;struct Cyc_Absyn_Vardecl*_tmp56C;struct Cyc_Absyn_Vardecl*_tmp56D;switch(*((int*)_tmp569)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp56D=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp569)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp56D;
# 3112
void*_tmp56E=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7A=_tmp56E;void*_tmp56F=_stmttmp7A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp56F)->tag == 4U){_LL1B: _LL1C:
# 3114
 return({struct _tuple14 _tmp63E;_tmp63E.f1=1,_tmp63E.f2=Cyc_Absyn_heap_rgn_type;_tmp63E;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp63F;_tmp63F.f1=(vd->tq).real_const,_tmp63F.f2=Cyc_Absyn_heap_rgn_type;_tmp63F;});}_LL1A:;}case 4U: _LL14: _tmp56C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp569)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp56C;
# 3118
void*_tmp570=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7B=_tmp570;void*_tmp571=_stmttmp7B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp571)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp640;_tmp640.f1=1,_tmp640.f2=(void*)_check_null(vd->rgn);_tmp640;});}else{_LL22: _LL23:
# 3121
 vd->escapes=1;
return({struct _tuple14 _tmp641;_tmp641.f1=(vd->tq).real_const,_tmp641.f2=(void*)_check_null(vd->rgn);_tmp641;});}_LL1F:;}case 5U: _LL16: _tmp56B=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp569)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp56B;
# 3124
_tmp56A=vd;goto _LL19;}default: _LL18: _tmp56A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp569)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp56A;
# 3126
vd->escapes=1;
return({struct _tuple14 _tmp642;_tmp642.f1=(vd->tq).real_const,_tmp642.f2=(void*)_check_null(vd->rgn);_tmp642;});}}_LLD:;}case 21U: _LL3: _tmp565=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55E)->f1;_tmp566=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55E)->f2;_tmp567=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55E)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp565;struct _fat_ptr*f=_tmp566;int is_tagged=_tmp567;
# 3131
if(is_tagged)return bogus_ans;{
# 3134
void*_tmp572=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7C=_tmp572;void*_tmp573=_stmttmp7C;struct Cyc_Absyn_Aggrdecl*_tmp574;struct Cyc_List_List*_tmp575;switch(*((int*)_tmp573)){case 7U: _LL25: _tmp575=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp573)->f2;_LL26: {struct Cyc_List_List*fs=_tmp575;
# 3136
struct Cyc_Absyn_Aggrfield*_tmp576=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp576;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp577=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7D=_tmp577;struct _tuple14 _tmp578=_stmttmp7D;void*_tmp57A;int _tmp579;_LL2C: _tmp579=_tmp578.f1;_tmp57A=_tmp578.f2;_LL2D: {int c=_tmp579;void*t=_tmp57A;
return({struct _tuple14 _tmp643;_tmp643.f1=(finfo->tq).real_const || c,_tmp643.f2=t;_tmp643;});}}
# 3141
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp573)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp573)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp574=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp573)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp574;
# 3143
struct Cyc_Absyn_Aggrfield*_tmp57B=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp57B;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp57C=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7E=_tmp57C;struct _tuple14 _tmp57D=_stmttmp7E;void*_tmp57F;int _tmp57E;_LL2F: _tmp57E=_tmp57D.f1;_tmp57F=_tmp57D.f2;_LL30: {int c=_tmp57E;void*t=_tmp57F;
return({struct _tuple14 _tmp644;_tmp644.f1=(finfo->tq).real_const || c,_tmp644.f2=t;_tmp644;});}}
# 3148
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp562=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55E)->f1;_tmp563=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55E)->f2;_tmp564=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55E)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp562;struct _fat_ptr*f=_tmp563;int is_tagged=_tmp564;
# 3153
if(is_tagged)return bogus_ans;{
# 3156
void*_tmp580=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7F=_tmp580;void*_tmp581=_stmttmp7F;void*_tmp583;void*_tmp582;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp581)->tag == 3U){_LL32: _tmp582=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp581)->f1).elt_type;_tmp583=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp581)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp582;void*r=_tmp583;
# 3158
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp584=Cyc_Tcutil_compress(t1);void*_stmttmp80=_tmp584;void*_tmp585=_stmttmp80;struct Cyc_Absyn_Aggrdecl*_tmp586;struct Cyc_List_List*_tmp587;switch(*((int*)_tmp585)){case 7U: _LL37: _tmp587=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp585)->f2;_LL38: {struct Cyc_List_List*fs=_tmp587;
# 3161
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp585)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp585)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp586=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp585)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp586;
# 3163
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3166
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp645;_tmp645.f1=(finfo->tq).real_const,_tmp645.f2=r;_tmp645;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp561=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp55E)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp561;
# 3173
void*_tmp588=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp81=_tmp588;void*_tmp589=_stmttmp81;void*_tmp58B;struct Cyc_Absyn_Tqual _tmp58A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->tag == 3U){_LL3E: _tmp58A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).elt_tq;_tmp58B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp58A;void*r=_tmp58B;
# 3175
return({struct _tuple14 _tmp646;_tmp646.f1=tq.real_const,_tmp646.f2=r;_tmp646;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp55F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55E)->f1;_tmp560=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55E)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp55F;struct Cyc_Absyn_Exp*e2=_tmp560;
# 3181
void*t=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*_tmp58C=t;struct Cyc_Absyn_Tqual _tmp58D;void*_tmp58F;struct Cyc_Absyn_Tqual _tmp58E;struct Cyc_List_List*_tmp590;switch(*((int*)_tmp58C)){case 6U: _LL43: _tmp590=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp58C)->f1;_LL44: {struct Cyc_List_List*ts=_tmp590;
# 3185
struct _tuple13 _tmp591=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp82=_tmp591;struct _tuple13 _tmp592=_stmttmp82;int _tmp594;unsigned _tmp593;_LL4C: _tmp593=_tmp592.f1;_tmp594=_tmp592.f2;_LL4D: {unsigned i=_tmp593;int known=_tmp594;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp595=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp595;
if(finfo != 0)
return({struct _tuple14 _tmp647;_tmp647.f1=((*finfo).f1).real_const,({void*_tmp799=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp647.f2=_tmp799;});_tmp647;});
return bogus_ans;}}}case 3U: _LL45: _tmp58E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58C)->f1).elt_tq;_tmp58F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp58C)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp58E;void*r=_tmp58F;
# 3193
return({struct _tuple14 _tmp648;_tmp648.f1=tq.real_const,_tmp648.f2=r;_tmp648;});}case 4U: _LL47: _tmp58D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp58C)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp58D;
# 3199
return({struct _tuple14 _tmp649;_tmp649.f1=tq.real_const,({void*_tmp79A=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp649.f2=_tmp79A;});_tmp649;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3203
({void*_tmp596=0U;({unsigned _tmp79C=e->loc;struct _fat_ptr _tmp79B=({const char*_tmp597="unary & applied to non-lvalue";_tag_fat(_tmp597,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp79C,_tmp79B,_tag_fat(_tmp596,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3210
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp598=({void*_tmp79D=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp79D,b);});struct Cyc_Absyn_Exp*eopt=_tmp598;
if(eopt == 0)return;{
struct Cyc_Absyn_Exp*_tmp599=eopt;struct Cyc_Absyn_Exp*e=_tmp599;
struct _tuple13 _tmp59A=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple13 _stmttmp83=_tmp59A;struct _tuple13 _tmp59B=_stmttmp83;int _tmp59D;unsigned _tmp59C;_LL1: _tmp59C=_tmp59B.f1;_tmp59D=_tmp59B.f2;_LL2: {unsigned j=_tmp59C;int known=_tmp59D;
if(known && j <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp5A0=({struct Cyc_Int_pa_PrintArg_struct _tmp64C;_tmp64C.tag=1U,_tmp64C.f1=(unsigned long)((int)j);_tmp64C;});struct Cyc_Int_pa_PrintArg_struct _tmp5A1=({struct Cyc_Int_pa_PrintArg_struct _tmp64B;_tmp64B.tag=1U,_tmp64B.f1=(unsigned long)((int)i);_tmp64B;});void*_tmp59E[2U];_tmp59E[0]=& _tmp5A0,_tmp59E[1]=& _tmp5A1;({unsigned _tmp79F=loc;struct _fat_ptr _tmp79E=({const char*_tmp59F="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp59F,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp79F,_tmp79E,_tag_fat(_tmp59E,sizeof(void*),2U));});});else{
# 3219
({struct Cyc_Int_pa_PrintArg_struct _tmp5A4=({struct Cyc_Int_pa_PrintArg_struct _tmp64E;_tmp64E.tag=1U,_tmp64E.f1=(unsigned long)((int)j);_tmp64E;});struct Cyc_Int_pa_PrintArg_struct _tmp5A5=({struct Cyc_Int_pa_PrintArg_struct _tmp64D;_tmp64D.tag=1U,_tmp64D.f1=(unsigned long)((int)i);_tmp64D;});void*_tmp5A2[2U];_tmp5A2[0]=& _tmp5A4,_tmp5A2[1]=& _tmp5A5;({unsigned _tmp7A1=loc;struct _fat_ptr _tmp7A0=({const char*_tmp5A3="dereference is out of bounds: %d <= %d";_tag_fat(_tmp5A3,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp7A1,_tmp7A0,_tag_fat(_tmp5A2,sizeof(void*),2U));});});}}
return;}}}
# 3223
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3231
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp5A6=e->r;void*_stmttmp84=_tmp5A6;void*_tmp5A7=_stmttmp84;struct Cyc_List_List*_tmp5A8;struct Cyc_List_List*_tmp5A9;struct Cyc_List_List*_tmp5AB;enum Cyc_Absyn_Primop _tmp5AA;struct Cyc_List_List*_tmp5AC;struct Cyc_List_List*_tmp5AD;struct Cyc_List_List*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B4;void*_tmp5B3;struct Cyc_Absyn_Exp*_tmp5B6;void*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B8;struct Cyc_Absyn_Exp*_tmp5B9;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5BE;struct Cyc_Absyn_Exp*_tmp5BD;struct Cyc_Absyn_Exp*_tmp5BC;void*_tmp5BF;switch(*((int*)_tmp5A7)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5BF=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL10: {void*b=_tmp5BF;
# 3243
void*_tmp5C0=b;struct Cyc_Absyn_Vardecl*_tmp5C1;struct Cyc_Absyn_Vardecl*_tmp5C2;switch(*((int*)_tmp5C0)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp5C2=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5C0)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5C2;
# 3246
void*_tmp5C3=Cyc_Tcutil_compress(vd->type);void*_stmttmp85=_tmp5C3;void*_tmp5C4=_stmttmp85;switch(*((int*)_tmp5C4)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp5C1=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5C0)->f1;_LL39: {struct Cyc_Absyn_Vardecl*vd=_tmp5C1;
# 3253
if((int)vd->sc == (int)Cyc_Absyn_Static){
void*_tmp5C5=Cyc_Tcutil_compress(vd->type);void*_stmttmp86=_tmp5C5;void*_tmp5C6=_stmttmp86;switch(*((int*)_tmp5C6)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3260
return var_okay;}}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp5BC=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_tmp5BD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A7)->f2;_tmp5BE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A7)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5BC;struct Cyc_Absyn_Exp*e2=_tmp5BD;struct Cyc_Absyn_Exp*e3=_tmp5BE;
# 3266
return(Cyc_Tcutil_cnst_exp(0,e1)&&
 Cyc_Tcutil_cnst_exp(0,e2))&&
 Cyc_Tcutil_cnst_exp(0,e3);}case 9U: _LL13: _tmp5BA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_tmp5BB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A7)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5BA;struct Cyc_Absyn_Exp*e2=_tmp5BB;
# 3270
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 41U: _LL15: _tmp5B9=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5B9;
_tmp5B8=e2;goto _LL18;}case 12U: _LL17: _tmp5B8=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5B8;
_tmp5B7=e2;goto _LL1A;}case 13U: _LL19: _tmp5B7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5B7;
# 3274
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A7)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp5B5=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_tmp5B6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A7)->f2;_LL1C: {void*t=_tmp5B5;struct Cyc_Absyn_Exp*e2=_tmp5B6;
# 3276
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}else{_LL1D: _tmp5B3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_tmp5B4=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A7)->f2;_LL1E: {void*t=_tmp5B3;struct Cyc_Absyn_Exp*e2=_tmp5B4;
# 3279
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}case 15U: _LL1F: _tmp5B2=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp5B2;
# 3281
return Cyc_Tcutil_cnst_exp(1,e2);}case 27U: _LL21: _tmp5B0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A7)->f2;_tmp5B1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A7)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp5B0;struct Cyc_Absyn_Exp*e2=_tmp5B1;
# 3283
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 28U: _LL23: _tmp5AF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp5AF;
# 3285
return Cyc_Tcutil_cnst_exp(0,e);}case 26U: _LL25: _tmp5AE=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL26: {struct Cyc_List_List*des=_tmp5AE;
_tmp5AD=des;goto _LL28;}case 30U: _LL27: _tmp5AD=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5A7)->f2;_LL28: {struct Cyc_List_List*des=_tmp5AD;
_tmp5AC=des;goto _LL2A;}case 29U: _LL29: _tmp5AC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5A7)->f3;_LL2A: {struct Cyc_List_List*des=_tmp5AC;
# 3289
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL2B: _tmp5AA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_tmp5AB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A7)->f2;_LL2C: {enum Cyc_Absyn_Primop p=_tmp5AA;struct Cyc_List_List*es=_tmp5AB;
# 3294
_tmp5A9=es;goto _LL2E;}case 24U: _LL2D: _tmp5A9=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL2E: {struct Cyc_List_List*es=_tmp5A9;
_tmp5A8=es;goto _LL30;}case 31U: _LL2F: _tmp5A8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5A7)->f1;_LL30: {struct Cyc_List_List*es=_tmp5A8;
# 3297
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3304
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3308
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5C7=Cyc_Tcutil_compress(t);void*_stmttmp87=_tmp5C7;void*_tmp5C8=_stmttmp87;struct Cyc_List_List*_tmp5C9;struct Cyc_List_List*_tmp5CA;void*_tmp5CB;void*_tmp5CC;struct Cyc_List_List*_tmp5CE;void*_tmp5CD;switch(*((int*)_tmp5C8)){case 0U: _LL1: _tmp5CD=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C8)->f1;_tmp5CE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C8)->f2;_LL2: {void*c=_tmp5CD;struct Cyc_List_List*ts=_tmp5CE;
# 3312
void*_tmp5CF=c;union Cyc_Absyn_AggrInfo _tmp5D0;struct Cyc_List_List*_tmp5D1;struct Cyc_Absyn_Enumdecl*_tmp5D2;switch(*((int*)_tmp5CF)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5D2=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5CF)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5D2;
# 3317
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5D1=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5D1=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5CF)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5D1;
# 3321
{struct Cyc_List_List*_tmp5D3=fs;struct Cyc_List_List*fs2=_tmp5D3;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5D4=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp88=_tmp5D4;struct _tuple13 _tmp5D5=_stmttmp88;int _tmp5D7;unsigned _tmp5D6;_LL1D: _tmp5D6=_tmp5D5.f1;_tmp5D7=_tmp5D5.f2;_LL1E: {unsigned i=_tmp5D6;int known=_tmp5D7;
if(known && i == (unsigned)0)
return 1;}}}}
# 3328
return 0;}case 20U: _LL18: _tmp5D0=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5CF)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5D0;
# 3331
struct Cyc_Absyn_Aggrdecl*_tmp5D8=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5D8;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5CC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C8)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5CC;
# 3339
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C8)->f1).elt_type;_LL6: {void*t=_tmp5CB;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5CA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5C8)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5CA;
# 3342
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))return 0;}
return 1;}case 7U: _LL9: _tmp5C9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5C8)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5C9;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3349
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3351
struct _RegionHandle _tmp5D9=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D9;_push_region(rgn);
{struct Cyc_List_List*_tmp5DA=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5DA;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5DB=0;_npop_handler(0U);return _tmp5DB;}}{
# 3359
int _tmp5DC=1;_npop_handler(0U);return _tmp5DC;}}
# 3352
;_pop_region();}
# 3363
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5DD=t;struct Cyc_Absyn_Typedefdecl*_tmp5DE;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5DD)->tag == 8U){_LL1: _tmp5DE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5DD)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5DE;
# 3366
if(tdopt != 0){
struct Cyc_Absyn_Tqual _tmp5DF=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5DF;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp5E2=({struct Cyc_String_pa_PrintArg_struct _tmp64F;_tmp64F.tag=0U,({struct _fat_ptr _tmp7A2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp64F.f1=_tmp7A2;});_tmp64F;});void*_tmp5E0[1U];_tmp5E0[0]=& _tmp5E2;({unsigned _tmp7A4=loc;struct _fat_ptr _tmp7A3=({const char*_tmp5E1="qualifier within typedef type %s is ignored";_tag_fat(_tmp5E1,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp7A4,_tmp7A3,_tag_fat(_tmp5E0,sizeof(void*),1U));});});}
# 3371
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3378
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp5E3=Cyc_Tcutil_compress(t);void*_stmttmp89=_tmp5E3;void*_tmp5E4=_stmttmp89;struct Cyc_List_List**_tmp5E5;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->tag == 5U){_LL1: _tmp5E5=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E4)->f1).attributes;_LL2: {struct Cyc_List_List**fnatts=_tmp5E5;
# 3381
struct Cyc_List_List*_tmp5E6=0;struct Cyc_List_List*res_atts=_tmp5E6;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_tmp7A5=({struct Cyc_List_List*_tmp5E7=_cycalloc(sizeof(*_tmp5E7));_tmp5E7->hd=(void*)atts->hd,_tmp5E7->tl=*fnatts;_tmp5E7;});*fnatts=_tmp7A5;});}else{
# 3388
res_atts=({struct Cyc_List_List*_tmp5E8=_cycalloc(sizeof(*_tmp5E8));_tmp5E8->hd=(void*)atts->hd,_tmp5E8->tl=res_atts;_tmp5E8;});}}
return res_atts;}}else{_LL3: _LL4:
({void*_tmp5E9=0U;({struct _fat_ptr _tmp7A6=({const char*_tmp5EA="transfer_fn_type_atts";_tag_fat(_tmp5EA,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A6,_tag_fat(_tmp5E9,sizeof(void*),0U));});});}_LL0:;}
# 3395
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5EB=Cyc_Tcutil_compress(t);void*_stmttmp8A=_tmp5EB;void*_tmp5EC=_stmttmp8A;struct Cyc_Absyn_Exp*_tmp5ED;struct Cyc_Absyn_PtrInfo _tmp5EE;switch(*((int*)_tmp5EC)){case 3U: _LL1: _tmp5EE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5EC)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp5EE;
return({void*_tmp7A7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7A7,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp5ED=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EC)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5ED;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3405
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5EF=b;struct Cyc_Absyn_Vardecl*_tmp5F0;struct Cyc_Absyn_Vardecl*_tmp5F1;struct Cyc_Absyn_Vardecl*_tmp5F2;struct Cyc_Absyn_Vardecl*_tmp5F3;switch(*((int*)_tmp5EF)){case 5U: _LL1: _tmp5F3=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5EF)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp5F3;
_tmp5F2=x;goto _LL4;}case 4U: _LL3: _tmp5F2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5EF)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp5F2;
_tmp5F1=x;goto _LL6;}case 3U: _LL5: _tmp5F1=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5EF)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp5F1;
_tmp5F0=x;goto _LL8;}case 1U: _LL7: _tmp5F0=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5EF)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp5F0;
# 3411
if(!x->escapes)
return x;
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3419
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5F4=0;struct Cyc_List_List*res=_tmp5F4;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)res=({struct Cyc_List_List*_tmp5F5=_cycalloc(sizeof(*_tmp5F5));_tmp5F5->hd=*((void**)_check_null((void**)x->hd)),_tmp5F5->tl=res;_tmp5F5;});}}
return res;}
# 3426
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5F6=Cyc_Tcutil_compress(t);void*_stmttmp8B=_tmp5F6;void*_tmp5F7=_stmttmp8B;unsigned _tmp5FC;void*_tmp5FB;struct Cyc_Absyn_Exp*_tmp5FA;struct Cyc_Absyn_Tqual _tmp5F9;void*_tmp5F8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F7)->tag == 4U){_LL1: _tmp5F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F7)->f1).elt_type;_tmp5F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F7)->f1).tq;_tmp5FA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F7)->f1).num_elts;_tmp5FB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F7)->f1).zero_term;_tmp5FC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F7)->f1).zt_loc;_LL2: {void*et=_tmp5F8;struct Cyc_Absyn_Tqual tq=_tmp5F9;struct Cyc_Absyn_Exp*eopt=_tmp5FA;void*zt=_tmp5FB;unsigned ztl=_tmp5FC;
# 3429
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 3433
if(convert_tag){
if(eopt->topt == 0)
({void*_tmp5FD=0U;({struct _fat_ptr _tmp7A8=({const char*_tmp5FE="cannot convert tag without type!";_tag_fat(_tmp5FE,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A8,_tag_fat(_tmp5FD,sizeof(void*),0U));});});{
void*_tmp5FF=Cyc_Tcutil_compress((void*)_check_null(eopt->topt));void*_stmttmp8C=_tmp5FF;void*_tmp600=_stmttmp8C;void*_tmp601;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp600)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp600)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp600)->f2 != 0){_LL6: _tmp601=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp600)->f2)->hd;_LL7: {void*t=_tmp601;
# 3439
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 3442
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3446
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
# 3448
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
