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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 876 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 878
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 885
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 890
void*Cyc_Absyn_compress_kb(void*);
# 896
int Cyc_Absyn_type2bool(int def,void*);
# 901
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 906
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 908
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 913
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 915
extern void*Cyc_Absyn_empty_effect;
# 917
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 919
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 944
extern void*Cyc_Absyn_fat_bound_type;
# 946
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 948
void*Cyc_Absyn_bounds_one();
# 950
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 955
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 975
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 977
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 980
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 997
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 999
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1015
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1022
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
# 1034
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1039
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1080
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1122
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1124
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1126
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1134
int Cyc_Absyn_fntype_att(void*);
# 1136
int Cyc_Absyn_equal_att(void*,void*);
# 1138
int Cyc_Absyn_attribute_cmp(void*,void*);
# 1144
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 68
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 75
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 27 "warn.h"
void Cyc_Warn_vwarn(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 33
void Cyc_Warn_verr(unsigned,struct _fat_ptr fmt,struct _fat_ptr);
# 38
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct _tuple13{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple13 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
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
({void*_tmp644=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp644;});
return 1;}else{
return 0;}}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 223
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 229
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp645=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp645,b);});struct Cyc_Absyn_Exp*eopt=_tmp37;
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
;_pop_region(rgn);}}}}default: _LL1A: _LL1B:
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
return({struct Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=tv->name,_tmp60->identity=- 1,({void*_tmp646=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp60->kind=_tmp646;});_tmp60;});}
# 367
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 369
struct _tuple9*_tmp61=arg;void*_tmp64;struct Cyc_Absyn_Tqual _tmp63;struct _fat_ptr*_tmp62;_LL1: _tmp62=_tmp61->f1;_tmp63=_tmp61->f2;_tmp64=_tmp61->f3;_LL2: {struct _fat_ptr*x=_tmp62;struct Cyc_Absyn_Tqual y=_tmp63;void*t=_tmp64;
return({struct _tuple9*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=x,_tmp65->f2=y,({void*_tmp647=Cyc_Tcutil_copy_type(t);_tmp65->f3=_tmp647;});_tmp65;});}}
# 372
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=(*arg).f1,({void*_tmp648=Cyc_Tcutil_copy_type((*arg).f2);_tmp66->f2=_tmp648;});_tmp66;});}
# 375
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}
# 379
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->name=f->name,_tmp67->tq=f->tq,({void*_tmp64A=Cyc_Tcutil_copy_type(f->type);_tmp67->type=_tmp64A;}),_tmp67->width=f->width,_tmp67->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp649=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp67->requires_clause=_tmp649;});_tmp67;});}struct _tuple15{void*f1;void*f2;};
# 383
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp68=x;void*_tmp6A;void*_tmp69;_LL1: _tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;_LL2: {void*r1=_tmp69;void*r2=_tmp6A;
return({struct _tuple15*_tmp6B=_cycalloc(sizeof(*_tmp6B));({void*_tmp64C=Cyc_Tcutil_copy_type(r1);_tmp6B->f1=_tmp64C;}),({void*_tmp64B=Cyc_Tcutil_copy_type(r2);_tmp6B->f2=_tmp64B;});_tmp6B;});}}
# 387
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 391
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6C=Cyc_Tcutil_compress(t);void*_stmttmp19=_tmp6C;void*_tmp6D=_stmttmp19;struct Cyc_Absyn_Datatypedecl*_tmp6E;struct Cyc_Absyn_Enumdecl*_tmp6F;struct Cyc_Absyn_Aggrdecl*_tmp70;struct Cyc_Absyn_Typedefdecl*_tmp73;struct Cyc_List_List*_tmp72;struct _tuple1*_tmp71;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp77;enum Cyc_Absyn_AggrKind _tmp76;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_List_List*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_VarargInfo*_tmp7F;int _tmp7E;struct Cyc_List_List*_tmp7D;void*_tmp7C;struct Cyc_Absyn_Tqual _tmp7B;void*_tmp7A;struct Cyc_List_List*_tmp79;unsigned _tmp8A;void*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Tqual _tmp87;void*_tmp86;struct Cyc_Absyn_PtrLoc*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8C;void*_tmp8B;struct Cyc_Absyn_Tvar*_tmp92;struct Cyc_List_List*_tmp94;void*_tmp93;void*_tmp95;switch(*((int*)_tmp6D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2 == 0){_LL1: _tmp95=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_LL2: {void*c=_tmp95;
return t;}}else{_LL3: _tmp93=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_tmp94=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2;_LL4: {void*c=_tmp93;struct Cyc_List_List*ts=_tmp94;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=0U,_tmp96->f1=c,({struct Cyc_List_List*_tmp64D=Cyc_Tcutil_copy_types(ts);_tmp96->f2=_tmp64D;});_tmp96;});}}case 1U: _LL5: _LL6:
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
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=4U,({void*_tmp650=Cyc_Tcutil_copy_type(et);(_tmp9E->f1).elt_type=_tmp650;}),(_tmp9E->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp64F=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp9E->f1).num_elts=_tmp64F;}),({
void*_tmp64E=Cyc_Tcutil_copy_type(zt);(_tmp9E->f1).zero_term=_tmp64E;}),(_tmp9E->f1).zt_loc=ztl;_tmp9E;});}case 5U: _LLD: _tmp79=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).tvars;_tmp7A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).effect;_tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_tqual;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_type;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).args;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).c_varargs;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).cyc_varargs;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).rgn_po;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).attributes;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_clause;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_relns;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_clause;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_relns;_LLE: {struct Cyc_List_List*tvs=_tmp79;void*effopt=_tmp7A;struct Cyc_Absyn_Tqual rt_tq=_tmp7B;void*rt=_tmp7C;struct Cyc_List_List*args=_tmp7D;int c_varargs=_tmp7E;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp7F;struct Cyc_List_List*rgn_po=_tmp80;struct Cyc_List_List*atts=_tmp81;struct Cyc_Absyn_Exp*req=_tmp82;struct Cyc_List_List*req_rlns=_tmp83;struct Cyc_Absyn_Exp*ens=_tmp84;struct Cyc_List_List*ens_rlns=_tmp85;
# 410
struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp9F;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmpA0=Cyc_Tcutil_copy_type(rt);void*rt2=_tmpA0;
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmpA1;
int _tmpA2=c_varargs;int c_varargs2=_tmpA2;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo*cv=cyc_varargs;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->name=cv->name,_tmpA3->tq=cv->tq,({void*_tmp651=Cyc_Tcutil_copy_type(cv->type);_tmpA3->type=_tmp651;}),_tmpA3->inject=cv->inject;_tmpA3;});}{
# 421
struct Cyc_List_List*_tmpA4=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmpA4;
struct Cyc_List_List*_tmpA5=atts;struct Cyc_List_List*atts2=_tmpA5;
struct Cyc_Absyn_Exp*_tmpA6=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmpA6;
struct Cyc_List_List*_tmpA7=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmpA7;
struct Cyc_Absyn_Exp*_tmpA8=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmpA8;
struct Cyc_List_List*_tmpA9=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmpA9;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=5U,(_tmpAA->f1).tvars=tvs2,(_tmpAA->f1).effect=effopt2,(_tmpAA->f1).ret_tqual=rt_tq,(_tmpAA->f1).ret_type=rt2,(_tmpAA->f1).args=args2,(_tmpAA->f1).c_varargs=c_varargs2,(_tmpAA->f1).cyc_varargs=cyc_varargs2,(_tmpAA->f1).rgn_po=rgn_po2,(_tmpAA->f1).attributes=atts2,(_tmpAA->f1).requires_clause=req2,(_tmpAA->f1).requires_relns=req_rlns2,(_tmpAA->f1).ensures_clause=ens2,(_tmpAA->f1).ensures_relns=ens_rlns2;_tmpAA;});}}case 6U: _LLF: _tmp78=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp78;
# 430
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=6U,({struct Cyc_List_List*_tmp652=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmpAB->f1=_tmp652;});_tmpAB;});}case 7U: _LL11: _tmp76=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f1;_tmp77=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp76;struct Cyc_List_List*fs=_tmp77;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=7U,_tmpAC->f1=k,({struct Cyc_List_List*_tmp653=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmpAC->f2=_tmp653;});_tmpAC;});}case 9U: _LL13: _tmp75=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp75;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=9U,_tmpAD->f1=e;_tmpAD;});}case 11U: _LL15: _tmp74=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6D)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp74;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=11U,_tmpAE->f1=e;_tmpAE;});}case 8U: _LL17: _tmp71=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f1;_tmp72=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f2;_tmp73=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f3;_LL18: {struct _tuple1*tdn=_tmp71;struct Cyc_List_List*ts=_tmp72;struct Cyc_Absyn_Typedefdecl*td=_tmp73;
# 435
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=8U,_tmpAF->f1=tdn,({struct Cyc_List_List*_tmp654=Cyc_Tcutil_copy_types(ts);_tmpAF->f2=_tmp654;}),_tmpAF->f3=td,_tmpAF->f4=0;_tmpAF;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)){case 0U: _LL19: _tmp70=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp70;
# 438
struct Cyc_List_List*_tmpB0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpB0;
return({union Cyc_Absyn_AggrInfo _tmp655=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp655,targs);});}case 1U: _LL1B: _tmp6F=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp6F;
# 441
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp6E=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp6E;
# 443
struct Cyc_List_List*_tmpB1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpB1;
return({union Cyc_Absyn_DatatypeInfo _tmp656=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5F9;_tmp5F9.name=dd->name,_tmp5F9.is_extensible=0;_tmp5F9;}));Cyc_Absyn_datatype_type(_tmp656,targs);});}}}_LL0:;}
# 450
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB2=d;struct _fat_ptr*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->tag == 0U){_LL1: _tmpB4=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpB4;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->tag=0U,({struct Cyc_Absyn_Exp*_tmp657=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpB5->f1=_tmp657;});_tmpB5;});}}else{_LL3: _tmpB3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB2)->f1;_LL4: {struct _fat_ptr*v=_tmpB3;
return d;}}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 456
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 458
return({struct _tuple16*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_List_List*_tmp659=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB6->f1=_tmp659;}),({struct Cyc_Absyn_Exp*_tmp658=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB6->f2=_tmp658;});_tmpB6;});}
# 461
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB7=preserve_types;int pt=_tmpB7;
{void*_tmpB8=e->r;void*_stmttmp1A=_tmpB8;void*_tmpB9=_stmttmp1A;struct Cyc_List_List*_tmpBE;struct Cyc_List_List*_tmpBD;struct Cyc_List_List*_tmpBC;struct _fat_ptr _tmpBB;int _tmpBA;void*_tmpBF;struct _fat_ptr*_tmpC1;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_List_List*_tmpC3;struct Cyc_Core_Opt*_tmpC2;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_Absyn_Exp*_tmpC4;int _tmpCB;int _tmpCA;struct Cyc_Absyn_Exp*_tmpC9;void**_tmpC8;struct Cyc_Absyn_Exp*_tmpC7;int _tmpC6;struct Cyc_Absyn_Enumfield*_tmpCD;void*_tmpCC;struct Cyc_Absyn_Enumfield*_tmpCF;struct Cyc_Absyn_Enumdecl*_tmpCE;struct Cyc_List_List*_tmpD3;void*_tmpD2;struct Cyc_Absyn_Tqual _tmpD1;struct _fat_ptr*_tmpD0;struct Cyc_List_List*_tmpD5;void*_tmpD4;struct Cyc_Absyn_Aggrdecl*_tmpD9;struct Cyc_List_List*_tmpD8;struct Cyc_List_List*_tmpD7;struct _tuple1*_tmpD6;int _tmpDC;void*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;int _tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Vardecl*_tmpDD;struct Cyc_Absyn_Datatypefield*_tmpE3;struct Cyc_Absyn_Datatypedecl*_tmpE2;struct Cyc_List_List*_tmpE1;struct Cyc_List_List*_tmpE4;struct Cyc_Absyn_Exp*_tmpE6;struct Cyc_Absyn_Exp*_tmpE5;int _tmpEA;int _tmpE9;struct _fat_ptr*_tmpE8;struct Cyc_Absyn_Exp*_tmpE7;int _tmpEE;int _tmpED;struct _fat_ptr*_tmpEC;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_List_List*_tmpF3;void*_tmpF2;struct Cyc_Absyn_Exp*_tmpF4;void*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_Absyn_Exp*_tmpF8;struct Cyc_Absyn_Exp*_tmpF7;enum Cyc_Absyn_Coercion _tmpFC;int _tmpFB;struct Cyc_Absyn_Exp*_tmpFA;void*_tmpF9;struct Cyc_List_List*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFF;int _tmp101;struct Cyc_Absyn_Exp*_tmp100;int _tmp105;struct Cyc_Absyn_VarargCallInfo*_tmp104;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp102;int _tmp10E;int _tmp10D;void*_tmp10C;struct Cyc_Absyn_Tqual _tmp10B;struct _fat_ptr*_tmp10A;struct Cyc_List_List*_tmp109;int _tmp108;struct Cyc_List_List*_tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Absyn_Exp*_tmp111;struct Cyc_Core_Opt*_tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp119;enum Cyc_Absyn_Incrementor _tmp11C;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_List_List*_tmp11E;enum Cyc_Absyn_Primop _tmp11D;struct _fat_ptr _tmp11F;void*_tmp120;union Cyc_Absyn_Cnst _tmp121;switch(*((int*)_tmpB9)){case 0U: _LL1: _tmp121=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp121;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp120=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL4: {void*b=_tmp120;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp11F=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL6: {struct _fat_ptr p=_tmp11F;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp11D=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp11D;struct Cyc_List_List*es=_tmp11E;
new_e=({enum Cyc_Absyn_Primop _tmp65B=p;struct Cyc_List_List*_tmp65A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_primop_exp(_tmp65B,_tmp65A,e->loc);});goto _LL0;}case 5U: _LL9: _tmp11B=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp11B;enum Cyc_Absyn_Incrementor i=_tmp11C;
new_e=({struct Cyc_Absyn_Exp*_tmp65D=Cyc_Tcutil_deep_copy_exp(pt,e1);enum Cyc_Absyn_Incrementor _tmp65C=i;Cyc_Absyn_increment_exp(_tmp65D,_tmp65C,e->loc);});goto _LL0;}case 7U: _LLB: _tmp119=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp119;struct Cyc_Absyn_Exp*e2=_tmp11A;
new_e=({struct Cyc_Absyn_Exp*_tmp65F=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp65E=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_and_exp(_tmp65F,_tmp65E,e->loc);});goto _LL0;}case 8U: _LLD: _tmp117=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp118=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp117;struct Cyc_Absyn_Exp*e2=_tmp118;
new_e=({struct Cyc_Absyn_Exp*_tmp661=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp660=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_or_exp(_tmp661,_tmp660,e->loc);});goto _LL0;}case 9U: _LLF: _tmp115=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp116=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp115;struct Cyc_Absyn_Exp*e2=_tmp116;
new_e=({struct Cyc_Absyn_Exp*_tmp663=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp662=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_seq_exp(_tmp663,_tmp662,e->loc);});goto _LL0;}case 6U: _LL11: _tmp112=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp113=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp114=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp112;struct Cyc_Absyn_Exp*e2=_tmp113;struct Cyc_Absyn_Exp*e3=_tmp114;
# 474
new_e=({struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp665=Cyc_Tcutil_deep_copy_exp(pt,e2);struct Cyc_Absyn_Exp*_tmp664=Cyc_Tcutil_deep_copy_exp(pt,e3);Cyc_Absyn_conditional_exp(_tmp666,_tmp665,_tmp664,e->loc);});goto _LL0;}case 4U: _LL13: _tmp10F=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp110=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp111=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp10F;struct Cyc_Core_Opt*po=_tmp110;struct Cyc_Absyn_Exp*e2=_tmp111;
# 476
new_e=({struct Cyc_Absyn_Exp*_tmp669=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Core_Opt*_tmp668=(unsigned)po?({struct Cyc_Core_Opt*_tmp122=_cycalloc(sizeof(*_tmp122));_tmp122->v=(void*)po->v;_tmp122;}): 0;struct Cyc_Absyn_Exp*_tmp667=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_assignop_exp(_tmp669,_tmp668,_tmp667,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3 != 0){_LL15: _tmp106=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp107=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp108=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->num_varargs;_tmp109=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->injectors;_tmp10A=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->name;_tmp10B=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->tq;_tmp10C=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->type;_tmp10D=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->inject;_tmp10E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp106;struct Cyc_List_List*es=_tmp107;int n=_tmp108;struct Cyc_List_List*is=_tmp109;struct _fat_ptr*nm=_tmp10A;struct Cyc_Absyn_Tqual tq=_tmp10B;void*t=_tmp10C;int i=_tmp10D;int resolved=_tmp10E;
# 479
new_e=({void*_tmp66F=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->tag=10U,({
struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp125->f1=_tmp66E;}),({struct Cyc_List_List*_tmp66D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp125->f2=_tmp66D;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp66C=({struct Cyc_Absyn_VarargCallInfo*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->num_varargs=n,_tmp124->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp66B=({struct Cyc_Absyn_VarargInfo*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->name=nm,_tmp123->tq=tq,({void*_tmp66A=Cyc_Tcutil_copy_type(t);_tmp123->type=_tmp66A;}),_tmp123->inject=i;_tmp123;});_tmp124->vai=_tmp66B;});_tmp124;});
# 481
_tmp125->f3=_tmp66C;}),_tmp125->f4=resolved;_tmp125;});
# 479
Cyc_Absyn_new_exp(_tmp66F,e->loc);});
# 484
goto _LL0;}}else{_LL17: _tmp102=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp103=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp104=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp105=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp102;struct Cyc_List_List*es=_tmp103;struct Cyc_Absyn_VarargCallInfo*vci=_tmp104;int resolved=_tmp105;
# 486
new_e=({void*_tmp672=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=10U,({struct Cyc_Absyn_Exp*_tmp671=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp126->f1=_tmp671;}),({struct Cyc_List_List*_tmp670=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp126->f2=_tmp670;}),_tmp126->f3=vci,_tmp126->f4=resolved;_tmp126;});Cyc_Absyn_new_exp(_tmp672,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmp100=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp101=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp100;int b=_tmp101;
# 489
new_e=b?({struct Cyc_Absyn_Exp*_tmp674=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_rethrow_exp(_tmp674,e->loc);}):({struct Cyc_Absyn_Exp*_tmp673=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_throw_exp(_tmp673,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmpFF=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmpFF;
# 492
new_e=({struct Cyc_Absyn_Exp*_tmp675=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp675,e->loc);});goto _LL0;}case 13U: _LL1D: _tmpFD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpFD;struct Cyc_List_List*ts=_tmpFE;
# 494
new_e=({struct Cyc_Absyn_Exp*_tmp677=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_List_List*_tmp676=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp677,_tmp676,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpF9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFA=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpFB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpFC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL20: {void*t=_tmpF9;struct Cyc_Absyn_Exp*e1=_tmpFA;int b=_tmpFB;enum Cyc_Absyn_Coercion c=_tmpFC;
# 496
new_e=({void*_tmp67B=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp67A=Cyc_Tcutil_deep_copy_exp(pt,e1);int _tmp679=b;enum Cyc_Absyn_Coercion _tmp678=c;Cyc_Absyn_cast_exp(_tmp67B,_tmp67A,_tmp679,_tmp678,e->loc);});goto _LL0;}case 16U: _LL21: _tmpF7=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpF7;struct Cyc_Absyn_Exp*e1=_tmpF8;
# 498
new_e=({struct Cyc_Absyn_Exp*_tmp67D=(unsigned)eo?Cyc_Tcutil_deep_copy_exp(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp67C=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_New_exp(_tmp67D,_tmp67C,e->loc);});goto _LL0;}case 15U: _LL23: _tmpF6=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpF6;
new_e=({struct Cyc_Absyn_Exp*_tmp67E=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_address_exp(_tmp67E,e->loc);});goto _LL0;}case 17U: _LL25: _tmpF5=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL26: {void*t=_tmpF5;
new_e=({void*_tmp67F=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp67F,e->loc);});goto _LL0;}case 18U: _LL27: _tmpF4=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF4;
new_e=({struct Cyc_Absyn_Exp*_tmp680=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp680,e->loc);});goto _LL0;}case 19U: _LL29: _tmpF2=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF3=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL2A: {void*t=_tmpF2;struct Cyc_List_List*ofs=_tmpF3;
new_e=({void*_tmp682=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp681=ofs;Cyc_Absyn_offsetof_exp(_tmp682,_tmp681,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpF1=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpF1;
new_e=({struct Cyc_Absyn_Exp*_tmp683=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_deref_exp(_tmp683,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpF0=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpF0;
new_e=({struct Cyc_Absyn_Exp*_tmp684=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_extension_exp(_tmp684,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpEF=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL30: {struct Cyc_List_List*es=_tmpEF;
new_e=({struct Cyc_List_List*_tmp685=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_tuple_exp(_tmp685,e->loc);});goto _LL0;}case 21U: _LL31: _tmpEB=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpEC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpED=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpEE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpEB;struct _fat_ptr*n=_tmpEC;int f1=_tmpED;int f2=_tmpEE;
# 507
new_e=({void*_tmp687=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=21U,({struct Cyc_Absyn_Exp*_tmp686=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp127->f1=_tmp686;}),_tmp127->f2=n,_tmp127->f3=f1,_tmp127->f4=f2;_tmp127;});Cyc_Absyn_new_exp(_tmp687,e->loc);});goto _LL0;}case 22U: _LL33: _tmpE7=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpEA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpE7;struct _fat_ptr*n=_tmpE8;int f1=_tmpE9;int f2=_tmpEA;
# 509
new_e=({void*_tmp689=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=22U,({struct Cyc_Absyn_Exp*_tmp688=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp128->f1=_tmp688;}),_tmp128->f2=n,_tmp128->f3=f1,_tmp128->f4=f2;_tmp128;});Cyc_Absyn_new_exp(_tmp689,e->loc);});goto _LL0;}case 23U: _LL35: _tmpE5=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpE5;struct Cyc_Absyn_Exp*e2=_tmpE6;
# 511
new_e=({struct Cyc_Absyn_Exp*_tmp68B=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp68A=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_subscript_exp(_tmp68B,_tmp68A,e->loc);});goto _LL0;}case 26U: _LL37: _tmpE4=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL38: {struct Cyc_List_List*eds=_tmpE4;
# 513
new_e=({void*_tmp68D=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=26U,({struct Cyc_List_List*_tmp68C=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp129->f1=_tmp68C;});_tmp129;});Cyc_Absyn_new_exp(_tmp68D,e->loc);});goto _LL0;}case 31U: _LL39: _tmpE1=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3A: {struct Cyc_List_List*es=_tmpE1;struct Cyc_Absyn_Datatypedecl*dtd=_tmpE2;struct Cyc_Absyn_Datatypefield*dtf=_tmpE3;
# 515
new_e=({void*_tmp68F=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=31U,({struct Cyc_List_List*_tmp68E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp12A->f1=_tmp68E;}),_tmp12A->f2=dtd,_tmp12A->f3=dtf;_tmp12A;});Cyc_Absyn_new_exp(_tmp68F,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpDD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpDF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpE0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpDD;struct Cyc_Absyn_Exp*e1=_tmpDE;struct Cyc_Absyn_Exp*e2=_tmpDF;int b=_tmpE0;
# 517
new_e=({void*_tmp692=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=27U,_tmp12B->f1=vd,({struct Cyc_Absyn_Exp*_tmp691=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12B->f2=_tmp691;}),({struct Cyc_Absyn_Exp*_tmp690=Cyc_Tcutil_deep_copy_exp(pt,e2);_tmp12B->f3=_tmp690;}),_tmp12B->f4=b;_tmp12B;});Cyc_Absyn_new_exp(_tmp692,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpDA=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDB=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpDC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpDA;void*t=_tmpDB;int b=_tmpDC;
# 519
new_e=({void*_tmp695=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=28U,({struct Cyc_Absyn_Exp*_tmp694=Cyc_Tcutil_deep_copy_exp(pt,e);_tmp12C->f1=_tmp694;}),({void*_tmp693=Cyc_Tcutil_copy_type(t);_tmp12C->f2=_tmp693;}),_tmp12C->f3=b;_tmp12C;});Cyc_Absyn_new_exp(_tmp695,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpD6=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpD8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpD9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL40: {struct _tuple1*n=_tmpD6;struct Cyc_List_List*ts=_tmpD7;struct Cyc_List_List*eds=_tmpD8;struct Cyc_Absyn_Aggrdecl*agr=_tmpD9;
# 522
new_e=({void*_tmp698=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=29U,_tmp12D->f1=n,({struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp12D->f2=_tmp697;}),({struct Cyc_List_List*_tmp696=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12D->f3=_tmp696;}),_tmp12D->f4=agr;_tmp12D;});Cyc_Absyn_new_exp(_tmp698,e->loc);});
# 524
goto _LL0;}case 30U: _LL41: _tmpD4=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD5=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL42: {void*t=_tmpD4;struct Cyc_List_List*eds=_tmpD5;
# 526
new_e=({void*_tmp69B=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=30U,({void*_tmp69A=Cyc_Tcutil_copy_type(t);_tmp12E->f1=_tmp69A;}),({struct Cyc_List_List*_tmp699=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12E->f2=_tmp699;});_tmp12E;});Cyc_Absyn_new_exp(_tmp69B,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpD0=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f1;_tmpD1=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f2;_tmpD2=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f3;_tmpD3=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL44: {struct _fat_ptr*vopt=_tmpD0;struct Cyc_Absyn_Tqual tq=_tmpD1;void*t=_tmpD2;struct Cyc_List_List*eds=_tmpD3;
# 529
new_e=({void*_tmp69F=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=25U,({struct _tuple9*_tmp69E=({struct _tuple9*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->f1=vopt,_tmp12F->f2=tq,({void*_tmp69D=Cyc_Tcutil_copy_type(t);_tmp12F->f3=_tmp69D;});_tmp12F;});_tmp130->f1=_tmp69E;}),({
struct Cyc_List_List*_tmp69C=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp130->f2=_tmp69C;});_tmp130;});
# 529
Cyc_Absyn_new_exp(_tmp69F,e->loc);});
# 531
goto _LL0;}case 32U: _LL45: _tmpCE=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpCF=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpCE;struct Cyc_Absyn_Enumfield*ef=_tmpCF;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpCC=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpCD=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL48: {void*t=_tmpCC;struct Cyc_Absyn_Enumfield*ef=_tmpCD;
# 534
new_e=({void*_tmp6A1=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=33U,({void*_tmp6A0=Cyc_Tcutil_copy_type(t);_tmp131->f1=_tmp6A0;}),_tmp131->f2=ef;_tmp131;});Cyc_Absyn_new_exp(_tmp6A1,e->loc);});goto _LL0;}case 34U: _LL49: _tmpC6=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).is_calloc;_tmpC7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).rgn;_tmpC8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).elt_type;_tmpC9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).num_elts;_tmpCA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).fat_result;_tmpCB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).inline_call;_LL4A: {int ic=_tmpC6;struct Cyc_Absyn_Exp*r=_tmpC7;void**t=_tmpC8;struct Cyc_Absyn_Exp*n=_tmpC9;int res=_tmpCA;int inlc=_tmpCB;
# 536
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=Cyc_Tcutil_deep_copy_exp(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp132=_cycalloc(sizeof(*_tmp132));({void*_tmp6A2=Cyc_Tcutil_copy_type(*t);*_tmp132=_tmp6A2;});_tmp132;});
({void*_tmp6A3=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->tag=34U,(_tmp133->f1).is_calloc=ic,(_tmp133->f1).rgn=r1,(_tmp133->f1).elt_type=t1,(_tmp133->f1).num_elts=n,(_tmp133->f1).fat_result=res,(_tmp133->f1).inline_call=inlc;_tmp133;});e2->r=_tmp6A3;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpC4=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpC5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpC4;struct Cyc_Absyn_Exp*e2=_tmpC5;
new_e=({struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6A4=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_swap_exp(_tmp6A5,_tmp6A4,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpC2=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpC3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpC2;struct Cyc_List_List*eds=_tmpC3;
# 544
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->v=(struct _tuple1*)nopt->v;_tmp134;});
new_e=({void*_tmp6A7=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->tag=36U,_tmp135->f1=nopt1,({struct Cyc_List_List*_tmp6A6=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp135->f2=_tmp6A6;});_tmp135;});Cyc_Absyn_new_exp(_tmp6A7,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 549
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp137=_cycalloc(sizeof(*_tmp137));_tmp137->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6A8=({const char*_tmp136="deep_copy: statement expressions unsupported";_tag_fat(_tmp136,sizeof(char),45U);});_tmp137->f1=_tmp6A8;});_tmp137;}));case 38U: _LL51: _tmpC0=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpC1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpC0;struct _fat_ptr*fn=_tmpC1;
new_e=({void*_tmp6AA=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->tag=38U,({struct Cyc_Absyn_Exp*_tmp6A9=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp138->f1=_tmp6A9;}),_tmp138->f2=fn;_tmp138;});Cyc_Absyn_new_exp(_tmp6AA,e->loc);});
goto _LL0;}case 39U: _LL53: _tmpBF=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL54: {void*t=_tmpBF;
new_e=({void*_tmp6AB=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp6AB,e->loc);});
goto _LL0;}default: _LL55: _tmpBA=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpBB=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpBC=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpBD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_tmpBE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f5;_LL56: {int v=_tmpBA;struct _fat_ptr t=_tmpBB;struct Cyc_List_List*o=_tmpBC;struct Cyc_List_List*i=_tmpBD;struct Cyc_List_List*c=_tmpBE;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}}_LL0:;}
# 557
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 561
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 572 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp139=ka1;enum Cyc_Absyn_AliasQual _tmp13B;enum Cyc_Absyn_KindQual _tmp13A;_LL1: _tmp13A=_tmp139->kind;_tmp13B=_tmp139->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp13A;enum Cyc_Absyn_AliasQual a1=_tmp13B;
struct Cyc_Absyn_Kind*_tmp13C=ka2;enum Cyc_Absyn_AliasQual _tmp13E;enum Cyc_Absyn_KindQual _tmp13D;_LL4: _tmp13D=_tmp13C->kind;_tmp13E=_tmp13C->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp13D;enum Cyc_Absyn_AliasQual a2=_tmp13E;
# 576
if((int)k1 != (int)k2){
struct _tuple17 _tmp13F=({struct _tuple17 _tmp5FA;_tmp5FA.f1=k1,_tmp5FA.f2=k2;_tmp5FA;});struct _tuple17 _stmttmp1B=_tmp13F;struct _tuple17 _tmp140=_stmttmp1B;switch(_tmp140.f1){case Cyc_Absyn_BoxKind: switch(_tmp140.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp140.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 584
if((int)a1 != (int)a2){
struct _tuple18 _tmp141=({struct _tuple18 _tmp5FB;_tmp5FB.f1=a1,_tmp5FB.f2=a2;_tmp5FB;});struct _tuple18 _stmttmp1C=_tmp141;struct _tuple18 _tmp142=_stmttmp1C;switch(_tmp142.f1){case Cyc_Absyn_Aliasable: if(_tmp142.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp142.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 590
return 1;}}}
# 593
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp143=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1D=_tmp143;void*_tmp144=_stmttmp1D;struct Cyc_Absyn_Kind*_tmp145;struct Cyc_Absyn_Kind*_tmp146;switch(*((int*)_tmp144)){case 0U: _LL1: _tmp146=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp146;
return k;}case 2U: _LL3: _tmp145=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp145;
return k;}default: _LL5: _LL6:
({void*_tmp6AC=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp147=_cycalloc(sizeof(*_tmp147));_tmp147->tag=2U,_tmp147->f1=0,_tmp147->f2=def;_tmp147;});tv->kind=_tmp6AC;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 601
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp148=Cyc_Tcutil_compress(t);void*_stmttmp1E=_tmp148;void*_tmp149=_stmttmp1E;struct Cyc_Absyn_Tvar*_tmp14A;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->tag == 2U){_LL1: _tmp14A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp14A;
# 604
void*_tmp14B=tv->kind;void*oldkb=_tmp14B;
tv->kind=kb;
return({struct _tuple19 _tmp5FC;_tmp5FC.f1=tv,_tmp5FC.f2=oldkb;_tmp5FC;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp14E=({struct Cyc_String_pa_PrintArg_struct _tmp5FD;_tmp5FD.tag=0U,({struct _fat_ptr _tmp6AD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5FD.f1=_tmp6AD;});_tmp5FD;});void*_tmp14C[1U];_tmp14C[0]=& _tmp14E;({struct _fat_ptr _tmp6AE=({const char*_tmp14D="swap_kind: cannot update the kind of %s";_tag_fat(_tmp14D,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6AE,_tag_fat(_tmp14C,sizeof(void*),1U));});});}_LL0:;}
# 613
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 615
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 619
struct _RegionHandle _tmp14F=_new_region("temp");struct _RegionHandle*temp=& _tmp14F;_push_region(temp);
{struct Cyc_List_List*_tmp150=0;struct Cyc_List_List*inst=_tmp150;
# 622
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp151=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp151;
void*_tmp152=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp152;
struct Cyc_Absyn_Kind*_tmp153=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp1F=_tmp153;struct Cyc_Absyn_Kind*_tmp154=_stmttmp1F;switch(((struct Cyc_Absyn_Kind*)_tmp154)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 628
 inst=({struct Cyc_List_List*_tmp156=_region_malloc(temp,sizeof(*_tmp156));({struct _tuple19*_tmp6AF=({struct _tuple19*_tmp155=_region_malloc(temp,sizeof(*_tmp155));_tmp155->f1=tv,_tmp155->f2=t;_tmp155;});_tmp156->hd=_tmp6AF;}),_tmp156->tl=inst;_tmp156;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 632
if(inst != 0){
field_type=({struct _RegionHandle*_tmp6B1=temp;struct Cyc_List_List*_tmp6B0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp6B1,_tmp6B0,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 620
;_pop_region(temp);}
# 637
return k;}
# 644
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 646
void*_tmp157=Cyc_Tcutil_compress(t);void*_stmttmp20=_tmp157;void*_tmp158=_stmttmp20;struct Cyc_Absyn_Typedefdecl*_tmp159;struct Cyc_Absyn_Exp*_tmp15A;struct Cyc_Absyn_PtrInfo _tmp15B;struct Cyc_List_List*_tmp15D;void*_tmp15C;struct Cyc_Absyn_Tvar*_tmp15E;struct Cyc_Core_Opt*_tmp15F;switch(*((int*)_tmp158)){case 1U: _LL1: _tmp15F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp158)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp15F;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp15E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp158)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp15E;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp15C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp158)->f1;_tmp15D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp158)->f2;_LL6: {void*c=_tmp15C;struct Cyc_List_List*ts=_tmp15D;
# 650
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
# 671
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 673
({void*_tmp167=0U;({struct _fat_ptr _tmp6B2=({const char*_tmp168="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp168,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B2,_tag_fat(_tmp167,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 677
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp161=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownAggr).val)->kind;_tmp162=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownAggr).val)->tvs;_tmp163=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownAggr).val)->impl;_tmp164=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp160)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp161;struct Cyc_List_List*tvs=_tmp162;struct Cyc_Absyn_AggrdeclImpl*i=_tmp163;int expected_mem_kind=_tmp164;
# 679
if(i == 0){
if(expected_mem_kind)
return& Cyc_Tcutil_mk;else{
# 683
return& Cyc_Tcutil_ak;}}{
# 685
struct Cyc_List_List*_tmp169=i->fields;struct Cyc_List_List*fields=_tmp169;
if(fields == 0)return& Cyc_Tcutil_mk;
# 688
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){;}{
void*_tmp16A=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp16A;
struct Cyc_Absyn_Kind*_tmp16B=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp16B;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 696
for(0;fields != 0;fields=fields->tl){
void*_tmp16C=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp16C;
struct Cyc_Absyn_Kind*_tmp16D=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp16D;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 702
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 704
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp15B=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp158)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp15B;
# 707
void*_tmp16E=Cyc_Tcutil_compress((pinfo.ptr_atts).bounds);void*_stmttmp21=_tmp16E;void*_tmp16F=_stmttmp21;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp16F)->f1)){case 13U: _LL4D: _LL4E: {
# 709
enum Cyc_Absyn_AliasQual _tmp170=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp22=_tmp170;enum Cyc_Absyn_AliasQual _tmp171=_stmttmp22;switch(_tmp171){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 715
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 717
enum Cyc_Absyn_AliasQual _tmp172=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp23=_tmp172;enum Cyc_Absyn_AliasQual _tmp173=_stmttmp23;switch(_tmp173){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 723
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 727
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp15A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp15A;
# 729
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp159=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp158)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp159;
# 733
if(td == 0 || td->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp176=({struct Cyc_String_pa_PrintArg_struct _tmp5FE;_tmp5FE.tag=0U,({struct _fat_ptr _tmp6B3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5FE.f1=_tmp6B3;});_tmp5FE;});void*_tmp174[1U];_tmp174[0]=& _tmp176;({struct _fat_ptr _tmp6B4=({const char*_tmp175="type_kind: typedef found: %s";_tag_fat(_tmp175,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B4,_tag_fat(_tmp174,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp158)->f1)->r)){case 0U: _LL17: _LL18:
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
# 754
int Cyc_Tcutil_is_regparm0_att(void*a){
void*_tmp177=a;if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp177)->tag == 0U){if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp177)->f1 == 0){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 761
int Cyc_Tcutil_equiv_fn_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
return 1;}
# 770
static void*Cyc_Tcutil_rgns_of(void*t);
# 772
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 776
static struct _tuple19*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp178=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp24=_tmp178;struct Cyc_Absyn_Kind*_tmp179=_stmttmp24;switch(((struct Cyc_Absyn_Kind*)_tmp179)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp179)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp17A=_cycalloc(sizeof(*_tmp17A));_tmp17A->tag=9U,({struct Cyc_Absyn_Exp*_tmp6B5=Cyc_Absyn_uint_exp(0U,0U);_tmp17A->f1=_tmp6B5;});_tmp17A;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 787
return({struct _tuple19*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->f1=tv,_tmp17B->f2=t;_tmp17B;});}
# 794
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp17C=Cyc_Tcutil_compress(t);void*_stmttmp25=_tmp17C;void*_tmp17D=_stmttmp25;struct Cyc_List_List*_tmp17E;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp186;struct Cyc_Absyn_VarargInfo*_tmp185;struct Cyc_List_List*_tmp184;void*_tmp183;struct Cyc_Absyn_Tqual _tmp182;void*_tmp181;struct Cyc_List_List*_tmp180;struct Cyc_List_List*_tmp187;void*_tmp188;void*_tmp18A;void*_tmp189;struct Cyc_List_List*_tmp18B;switch(*((int*)_tmp17D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17D)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17D)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp18B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17D)->f2;_LL6: {struct Cyc_List_List*ts=_tmp18B;
# 799
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 803
struct Cyc_Absyn_Kind*_tmp18C=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp26=_tmp18C;struct Cyc_Absyn_Kind*_tmp18D=_stmttmp26;switch(((struct Cyc_Absyn_Kind*)_tmp18D)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp189=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17D)->f1).elt_type;_tmp18A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17D)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp189;void*r=_tmp18A;
# 811
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp18E[2U];({void*_tmp6B7=Cyc_Absyn_access_eff(r);_tmp18E[0]=_tmp6B7;}),({void*_tmp6B6=Cyc_Tcutil_rgns_of(et);_tmp18E[1]=_tmp6B6;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp18E,sizeof(void*),2U));})));}case 4U: _LLD: _tmp188=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17D)->f1).elt_type;_LLE: {void*et=_tmp188;
# 813
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp187=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp17D)->f2;_LL10: {struct Cyc_List_List*fs=_tmp187;
# 815
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp180=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).tvars;_tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).effect;_tmp182=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).ret_tqual;_tmp183=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).ret_type;_tmp184=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).args;_tmp185=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).cyc_varargs;_tmp186=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp180;void*eff=_tmp181;struct Cyc_Absyn_Tqual rt_tq=_tmp182;void*rt=_tmp183;struct Cyc_List_List*args=_tmp184;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp185;struct Cyc_List_List*rpo=_tmp186;
# 824
void*_tmp18F=({struct Cyc_List_List*_tmp6B8=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp6B8,(void*)_check_null(eff));});void*e=_tmp18F;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp17F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp17D)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp17F;
# 827
struct Cyc_List_List*_tmp190=0;struct Cyc_List_List*ts=_tmp190;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp191=_cycalloc(sizeof(*_tmp191));_tmp191->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp191->tl=ts;_tmp191;});}
_tmp17E=ts;goto _LL16;}case 8U: _LL15: _tmp17E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17D)->f2;_LL16: {struct Cyc_List_List*ts=_tmp17E;
# 832
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({void*_tmp192=0U;({struct _fat_ptr _tmp6B9=({const char*_tmp193="typedecl in rgns_of";_tag_fat(_tmp193,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B9,_tag_fat(_tmp192,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 842
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp194=e;void*_tmp195;struct Cyc_List_List**_tmp196;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)){case 9U: _LL1: _tmp196=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL2: {struct Cyc_List_List**es=_tmp196;
# 846
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp197=(void*)effs->hd;void*eff=_tmp197;
({void*_tmp6BA=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp6BA;});{
void*_tmp198=(void*)effs->hd;void*_stmttmp27=_tmp198;void*_tmp199=_stmttmp27;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->tl == 0){_LLE: _LLF:
# 855
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 859
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp19A=Cyc_Tcutil_compress((void*)effs->hd);void*_stmttmp28=_tmp19A;void*_tmp19B=_stmttmp28;void*_tmp19C;struct Cyc_List_List*_tmp19D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f1)){case 9U: _LL13: _tmp19D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp19D;
# 864
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);
goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19B)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp19C=_tmp19B;_LL1C: {void*e=_tmp19C;
effects=({struct Cyc_List_List*_tmp19E=_cycalloc(sizeof(*_tmp19E));_tmp19E->hd=e,_tmp19E->tl=effects;_tmp19E;});goto _LL12;}}_LL12:;}}
# 872
({struct Cyc_List_List*_tmp6BB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp6BB;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2 != 0){_LL3: _tmp195=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2)->hd;_LL4: {void*t=_tmp195;
# 875
void*_tmp19F=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp19F;void*_tmp1A0=_stmttmp29;switch(*((int*)_tmp1A0)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 880
 return e;}_LL0:;}}
# 885
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A1=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=5U,(_tmp1A2->f1).tvars=0,(_tmp1A2->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6BC=Cyc_Absyn_empty_tqual(0U);(_tmp1A2->f1).ret_tqual=_tmp6BC;}),(_tmp1A2->f1).ret_type=Cyc_Absyn_void_type,(_tmp1A2->f1).args=0,(_tmp1A2->f1).c_varargs=0,(_tmp1A2->f1).cyc_varargs=0,(_tmp1A2->f1).rgn_po=0,(_tmp1A2->f1).attributes=0,(_tmp1A2->f1).requires_clause=0,(_tmp1A2->f1).requires_relns=0,(_tmp1A2->f1).ensures_clause=0,(_tmp1A2->f1).ensures_relns=0;_tmp1A2;});
# 886
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp1A1;
# 895
return({void*_tmp6C0=(void*)fntype;void*_tmp6BF=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6BE=Cyc_Absyn_empty_tqual(0U);void*_tmp6BD=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp6C0,_tmp6BF,_tmp6BE,_tmp6BD,Cyc_Absyn_false_type);});}
# 902
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp1A3=Cyc_Tcutil_compress(e);void*_stmttmp2A=_tmp1A3;void*_tmp1A4=_stmttmp2A;struct Cyc_Core_Opt*_tmp1A7;void**_tmp1A6;struct Cyc_Core_Opt*_tmp1A5;void*_tmp1A8;struct Cyc_List_List*_tmp1A9;void*_tmp1AA;switch(*((int*)_tmp1A4)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2 != 0){_LL1: _tmp1AA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->hd;_LL2: {void*r2=_tmp1AA;
# 909
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Tcutil_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp1AB=({struct _tuple15 _tmp5FF;_tmp5FF.f1=r,_tmp5FF.f2=r2;_tmp5FF;});struct _tuple15 _stmttmp2B=_tmp1AB;struct _tuple15 _tmp1AC=_stmttmp2B;struct Cyc_Absyn_Tvar*_tmp1AE;struct Cyc_Absyn_Tvar*_tmp1AD;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AC.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AC.f2)->tag == 2U){_LLC: _tmp1AD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AC.f1)->f1;_tmp1AE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AC.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1AD;struct Cyc_Absyn_Tvar*tv2=_tmp1AE;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1A9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2;_LL4: {struct Cyc_List_List*es=_tmp1A9;
# 917
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2 != 0){_LL5: _tmp1A8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->hd;_LL6: {void*t=_tmp1A8;
# 921
void*_tmp1AF=Cyc_Tcutil_rgns_of(t);void*_stmttmp2C=_tmp1AF;void*_tmp1B0=_stmttmp2C;void*_tmp1B1;void*_tmp1B2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B0)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B0)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B0)->f2 != 0){_LL11: _tmp1B2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B0)->f2)->hd;_LL12: {void*t=_tmp1B2;
# 923
if(!constrain)return 0;{
void*_tmp1B3=Cyc_Tcutil_compress(t);void*_stmttmp2D=_tmp1B3;void*_tmp1B4=_stmttmp2D;struct Cyc_Core_Opt*_tmp1B7;void**_tmp1B6;struct Cyc_Core_Opt*_tmp1B5;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B4)->tag == 1U){_LL16: _tmp1B5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B4)->f1;_tmp1B6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B4)->f2;_tmp1B7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B4)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1B5;void**p=_tmp1B6;struct Cyc_Core_Opt*s=_tmp1B7;
# 928
void*_tmp1B8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1B8;
# 931
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp6C2=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1B9[2U];_tmp1B9[0]=ev,({void*_tmp6C1=Cyc_Absyn_access_eff(r);_tmp1B9[1]=_tmp6C1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B9,sizeof(void*),2U));})));*p=_tmp6C2;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1B1=_tmp1B0;_LL14: {void*e2=_tmp1B1;
# 936
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1A5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A4)->f1;_tmp1A6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A4)->f2;_tmp1A7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A4)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1A5;void**p=_tmp1A6;struct Cyc_Core_Opt*s=_tmp1A7;
# 939
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1BA=0U;({struct _fat_ptr _tmp6C3=({const char*_tmp1BB="effect evar has wrong kind";_tag_fat(_tmp1BB,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C3,_tag_fat(_tmp1BA,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 944
void*_tmp1BC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1BC;
# 947
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);{
void*_tmp1BD=Cyc_Absyn_join_eff(({void*_tmp1BE[2U];_tmp1BE[0]=ev,({void*_tmp6C4=Cyc_Absyn_access_eff(r);_tmp1BE[1]=_tmp6C4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BE,sizeof(void*),2U));}));void*new_typ=_tmp1BD;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 958
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1BF=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_stmttmp2E=_tmp1BF;void*_tmp1C0=_stmttmp2E;struct Cyc_Core_Opt*_tmp1C3;void**_tmp1C2;struct Cyc_Core_Opt*_tmp1C1;void*_tmp1C4;struct Cyc_List_List*_tmp1C5;switch(*((int*)_tmp1C0)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C0)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1C5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C0)->f2;_LL4: {struct Cyc_List_List*es=_tmp1C5;
# 963
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C0)->f2 != 0){_LL5: _tmp1C4=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C0)->f2)->hd;_LL6: {void*t2=_tmp1C4;
# 968
t2=Cyc_Tcutil_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1C6=Cyc_Tcutil_rgns_of(t);void*_stmttmp2F=_tmp1C6;void*_tmp1C7=_stmttmp2F;void*_tmp1C8;void*_tmp1C9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f2 != 0){_LLC: _tmp1C9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C7)->f2)->hd;_LLD: {void*t3=_tmp1C9;
# 973
struct _tuple15 _tmp1CA=({struct _tuple15 _tmp600;({void*_tmp6C5=Cyc_Tcutil_compress(t3);_tmp600.f1=_tmp6C5;}),_tmp600.f2=t2;_tmp600;});struct _tuple15 _stmttmp30=_tmp1CA;struct _tuple15 _tmp1CB=_stmttmp30;struct Cyc_Absyn_Tvar*_tmp1CD;struct Cyc_Absyn_Tvar*_tmp1CC;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CB.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CB.f2)->tag == 2U){_LL11: _tmp1CC=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CB.f1)->f1;_tmp1CD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CB.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1CC;struct Cyc_Absyn_Tvar*tv2=_tmp1CD;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1C8=_tmp1C7;_LLF: {void*e2=_tmp1C8;
# 977
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1C1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C0)->f1;_tmp1C2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C0)->f2;_tmp1C3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C0)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1C1;void**p=_tmp1C2;struct Cyc_Core_Opt*s=_tmp1C3;
# 980
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1CE=0U;({struct _fat_ptr _tmp6C6=({const char*_tmp1CF="effect evar has wrong kind";_tag_fat(_tmp1CF,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C6,_tag_fat(_tmp1CE,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 985
void*_tmp1D0=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1D0;
# 988
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1D1=Cyc_Absyn_join_eff(({void*_tmp1D2[2U];_tmp1D2[0]=ev,({void*_tmp6C7=Cyc_Absyn_regionsof_eff(t);_tmp1D2[1]=_tmp6C7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D2,sizeof(void*),2U));}));void*new_typ=_tmp1D1;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 999
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1D3=e;struct Cyc_Core_Opt*_tmp1D6;void**_tmp1D5;struct Cyc_Core_Opt*_tmp1D4;void*_tmp1D7;struct Cyc_List_List*_tmp1D8;struct Cyc_Absyn_Tvar*_tmp1D9;switch(*((int*)_tmp1D3)){case 2U: _LL1: _tmp1D9=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D3)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1D9;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f1)){case 9U: _LL3: _tmp1D8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f2;_LL4: {struct Cyc_List_List*es=_tmp1D8;
# 1004
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f2 != 0){_LL5: _tmp1D7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f2)->hd;_LL6: {void*t=_tmp1D7;
# 1009
void*_tmp1DA=Cyc_Tcutil_rgns_of(t);void*_stmttmp31=_tmp1DA;void*_tmp1DB=_stmttmp31;void*_tmp1DC;void*_tmp1DD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DB)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DB)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DB)->f2 != 0){_LLC: _tmp1DD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DB)->f2)->hd;_LLD: {void*t2=_tmp1DD;
# 1011
if(!may_constrain_evars)return 0;{
void*_tmp1DE=Cyc_Tcutil_compress(t2);void*_stmttmp32=_tmp1DE;void*_tmp1DF=_stmttmp32;struct Cyc_Core_Opt*_tmp1E2;void**_tmp1E1;struct Cyc_Core_Opt*_tmp1E0;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DF)->tag == 1U){_LL11: _tmp1E0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DF)->f1;_tmp1E1=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DF)->f2;_tmp1E2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DF)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1E0;void**p=_tmp1E1;struct Cyc_Core_Opt*s=_tmp1E2;
# 1017
void*_tmp1E3=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E3;
# 1019
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp6C9=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1E4[2U];_tmp1E4[0]=ev,({void*_tmp6C8=Cyc_Absyn_var_type(v);_tmp1E4[1]=_tmp6C8;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E4,sizeof(void*),2U));})));*p=_tmp6C9;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1DC=_tmp1DB;_LLF: {void*e2=_tmp1DC;
# 1024
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1D4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f1;_tmp1D5=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f2;_tmp1D6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1D4;void**p=_tmp1D5;struct Cyc_Core_Opt*s=_tmp1D6;
# 1027
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1E5=0U;({struct _fat_ptr _tmp6CA=({const char*_tmp1E6="effect evar has wrong kind";_tag_fat(_tmp1E6,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CA,_tag_fat(_tmp1E5,sizeof(void*),0U));});});{
# 1031
void*_tmp1E7=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E7;
# 1033
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1E8=Cyc_Absyn_join_eff(({void*_tmp1E9[2U];_tmp1E9[0]=ev,({void*_tmp6CB=Cyc_Absyn_var_type(v);_tmp1E9[1]=_tmp6CB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E9,sizeof(void*),2U));}));void*new_typ=_tmp1E8;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1043
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1EA=e;void*_tmp1EB;struct Cyc_List_List*_tmp1EC;switch(*((int*)_tmp1EA)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f1)){case 9U: _LL1: _tmp1EC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f2;_LL2: {struct Cyc_List_List*es=_tmp1EC;
# 1047
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f2 != 0){_LL3: _tmp1EB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f2)->hd;_LL4: {void*t=_tmp1EB;
# 1052
void*_tmp1ED=Cyc_Tcutil_rgns_of(t);void*_stmttmp33=_tmp1ED;void*_tmp1EE=_stmttmp33;void*_tmp1EF;void*_tmp1F0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2 != 0){_LLA: _tmp1F0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EE)->f2)->hd;_LLB: {void*t2=_tmp1F0;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1EF=_tmp1EE;_LLD: {void*e2=_tmp1EF;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1056
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 1070 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1075
void*_tmp1F1=Cyc_Tcutil_compress(e1);void*_stmttmp34=_tmp1F1;void*_tmp1F2=_stmttmp34;struct Cyc_Core_Opt*_tmp1F4;void**_tmp1F3;struct Cyc_Absyn_Tvar*_tmp1F5;void*_tmp1F6;void*_tmp1F7;struct Cyc_List_List*_tmp1F8;switch(*((int*)_tmp1F2)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f1)){case 9U: _LL1: _tmp1F8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2;_LL2: {struct Cyc_List_List*es=_tmp1F8;
# 1077
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2 != 0){_LL3: _tmp1F7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2)->hd;_LL4: {void*r=_tmp1F7;
# 1088
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2 != 0){_LL7: _tmp1F6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2)->hd;_LL8: {void*t=_tmp1F6;
# 1092
void*_tmp1F9=Cyc_Tcutil_rgns_of(t);void*_stmttmp35=_tmp1F9;void*_tmp1FA=_stmttmp35;void*_tmp1FB;void*_tmp1FC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FA)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FA)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FA)->f2 != 0){_LLE: _tmp1FC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FA)->f2)->hd;_LLF: {void*t2=_tmp1FC;
# 1097
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1FB=_tmp1FA;_LL11: {void*e=_tmp1FB;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1F5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F2)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1F5;
# 1090
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1F3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F2)->f2;_tmp1F4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F2)->f4;_LLA: {void**p=_tmp1F3;struct Cyc_Core_Opt*s=_tmp1F4;
# 1102
if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1106
*p=Cyc_Absyn_empty_effect;
# 1109
return 1;}else{
# 1111
return 0;}}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp1FF=({struct Cyc_String_pa_PrintArg_struct _tmp601;_tmp601.tag=0U,({struct _fat_ptr _tmp6CC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp601.f1=_tmp6CC;});_tmp601;});void*_tmp1FD[1U];_tmp1FD[0]=& _tmp1FF;({struct _fat_ptr _tmp6CD=({const char*_tmp1FE="subset_effect: bad effect: %s";_tag_fat(_tmp1FE,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CD,_tag_fat(_tmp1FD,sizeof(void*),1U));});});}_LL0:;}
# 1122
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1124
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp200=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp36=_tmp200;struct _tuple15*_tmp201=_stmttmp36;void*_tmp203;void*_tmp202;_LL1: _tmp202=_tmp201->f1;_tmp203=_tmp201->f2;_LL2: {void*t1a=_tmp202;void*t1b=_tmp203;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp204=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp37=_tmp204;struct _tuple15*_tmp205=_stmttmp37;void*_tmp207;void*_tmp206;_LL4: _tmp206=_tmp205->f1;_tmp207=_tmp205->f2;_LL5: {void*t2a=_tmp206;void*t2b=_tmp207;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1134
if(!found)return 0;}}}
# 1136
return 1;}
# 1142
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1144
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1147
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp208=t;switch(*((int*)_tmp208)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp208)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp208)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1153
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1156
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1159
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1162
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp208)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1151
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1154
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1157
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1160
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1163
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp208)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1152
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1155
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1158
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1161
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1164
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1186
({void*_tmp209=0U;({struct _fat_ptr _tmp6CE=({const char*_tmp20A="bad con";_tag_fat(_tmp20A,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CE,_tag_fat(_tmp209,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp208)->f1){case 0U: _LL21: _LL22:
# 1165
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
# 1189
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp20B=t;void*_tmp20C;switch(*((int*)_tmp20B)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp20C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20B)->f1;_LL18: {void*c=_tmp20C;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1205
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp20D=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp20D;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1210
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp20E=i;struct _tuple1*_tmp20F;struct Cyc_Absyn_Datatypedecl*_tmp210;if((_tmp20E.KnownDatatype).tag == 2){_LL1: _tmp210=*(_tmp20E.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp210;
return dd->name;}}else{_LL3: _tmp20F=((_tmp20E.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp20F;
return n;}}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1216
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp211=i;struct _tuple1*_tmp213;struct _tuple1*_tmp212;struct Cyc_Absyn_Datatypefield*_tmp215;struct Cyc_Absyn_Datatypedecl*_tmp214;if((_tmp211.KnownDatatypefield).tag == 2){_LL1: _tmp214=((_tmp211.KnownDatatypefield).val).f1;_tmp215=((_tmp211.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp214;struct Cyc_Absyn_Datatypefield*df=_tmp215;
# 1219
return({struct _tuple20 _tmp602;_tmp602.f1=dd->name,_tmp602.f2=df->name;_tmp602;});}}else{_LL3: _tmp212=((_tmp211.UnknownDatatypefield).val).datatype_name;_tmp213=((_tmp211.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp212;struct _tuple1*f=_tmp213;
# 1221
return({struct _tuple20 _tmp603;_tmp603.f1=d,_tmp603.f2=f;_tmp603;});}}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1224
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp216=i;struct Cyc_Absyn_Aggrdecl*_tmp217;struct _tuple1*_tmp219;enum Cyc_Absyn_AggrKind _tmp218;if((_tmp216.UnknownAggr).tag == 1){_LL1: _tmp218=((_tmp216.UnknownAggr).val).f1;_tmp219=((_tmp216.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp218;struct _tuple1*n=_tmp219;
return({struct _tuple21 _tmp604;_tmp604.f1=k,_tmp604.f2=n;_tmp604;});}}else{_LL3: _tmp217=*(_tmp216.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp217;
return({struct _tuple21 _tmp605;_tmp605.f1=ad->kind,_tmp605.f2=ad->name;_tmp605;});}}_LL0:;}
# 1230
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1236
struct _tuple15 _tmp21A=({struct _tuple15 _tmp606;_tmp606.f1=t1,_tmp606.f2=t2;_tmp606;});struct _tuple15 _stmttmp38=_tmp21A;struct _tuple15 _tmp21B=_stmttmp38;union Cyc_Absyn_AggrInfo _tmp21D;union Cyc_Absyn_AggrInfo _tmp21C;union Cyc_Absyn_DatatypeFieldInfo _tmp21F;union Cyc_Absyn_DatatypeFieldInfo _tmp21E;union Cyc_Absyn_DatatypeInfo _tmp221;union Cyc_Absyn_DatatypeInfo _tmp220;struct Cyc_List_List*_tmp223;struct Cyc_List_List*_tmp222;struct _fat_ptr _tmp225;struct _fat_ptr _tmp224;struct _tuple1*_tmp227;struct _tuple1*_tmp226;switch(*((int*)_tmp21B.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 15U){_LL1: _tmp226=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp227=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LL2: {struct _tuple1*n1=_tmp226;struct _tuple1*n2=_tmp227;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 17U){_LL3: _tmp224=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp225=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp224;struct _fat_ptr s2=_tmp225;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 16U){_LL5: _tmp222=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp223=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp222;struct Cyc_List_List*fs2=_tmp223;
# 1240
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 18U){_LL7: _tmp220=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp221=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp220;union Cyc_Absyn_DatatypeInfo info2=_tmp221;
# 1242
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(info1);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(info2);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 19U){_LL9: _tmp21E=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp21F=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp21E;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp21F;
# 1246
struct _tuple20 _tmp228=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple20 _stmttmp39=_tmp228;struct _tuple20 _tmp229=_stmttmp39;struct _tuple1*_tmp22B;struct _tuple1*_tmp22A;_LL10: _tmp22A=_tmp229.f1;_tmp22B=_tmp229.f2;_LL11: {struct _tuple1*d1=_tmp22A;struct _tuple1*f1=_tmp22B;
struct _tuple20 _tmp22C=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple20 _stmttmp3A=_tmp22C;struct _tuple20 _tmp22D=_stmttmp3A;struct _tuple1*_tmp22F;struct _tuple1*_tmp22E;_LL13: _tmp22E=_tmp22D.f1;_tmp22F=_tmp22D.f2;_LL14: {struct _tuple1*d2=_tmp22E;struct _tuple1*f2=_tmp22F;
int _tmp230=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp230;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 20U){_LLB: _tmp21C=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp21D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp21C;union Cyc_Absyn_AggrInfo info2=_tmp21D;
# 1252
struct _tuple21 _tmp231=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple21 _stmttmp3B=_tmp231;struct _tuple21 _tmp232=_stmttmp3B;struct _tuple1*_tmp234;enum Cyc_Absyn_AggrKind _tmp233;_LL16: _tmp233=_tmp232.f1;_tmp234=_tmp232.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp233;struct _tuple1*q1=_tmp234;
struct _tuple21 _tmp235=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple21 _stmttmp3C=_tmp235;struct _tuple21 _tmp236=_stmttmp3C;struct _tuple1*_tmp238;enum Cyc_Absyn_AggrKind _tmp237;_LL19: _tmp237=_tmp236.f1;_tmp238=_tmp236.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp237;struct _tuple1*q2=_tmp238;
int _tmp239=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp239;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1261
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6D0)(void*,void*)=cmp;void*_tmp6CF=(void*)_check_null(a1);_tmp6D0(_tmp6CF,(void*)_check_null(a2));});}
# 1267
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp23A=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp23A;
int _tmp23B=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp23B;
return Cyc_Core_intcmp(i1,i2);}
# 1272
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp23C=tqt1;void*_tmp23E;struct Cyc_Absyn_Tqual _tmp23D;_LL1: _tmp23D=_tmp23C->f1;_tmp23E=_tmp23C->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp23D;void*t1=_tmp23E;
struct _tuple12*_tmp23F=tqt2;void*_tmp241;struct Cyc_Absyn_Tqual _tmp240;_LL4: _tmp240=_tmp23F->f1;_tmp241=_tmp23F->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp240;void*t2=_tmp241;
int _tmp242=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp242;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1280
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp243=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp243;
if(zsc != 0)return zsc;{
int _tmp244=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp244;
if(tqc != 0)return tqc;{
int _tmp245=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp245;
if(tc != 0)return tc;{
int _tmp246=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp246;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1297
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp247=({int _tmp6D1=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6D1,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp247;
if(shallowcmp != 0)
return shallowcmp;{
# 1306
struct _tuple15 _tmp248=({struct _tuple15 _tmp607;_tmp607.f1=t2,_tmp607.f2=t1;_tmp607;});struct _tuple15 _stmttmp3D=_tmp248;struct _tuple15 _tmp249=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_Absyn_Exp*_tmp24A;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_List_List*_tmp251;enum Cyc_Absyn_AggrKind _tmp250;struct Cyc_List_List*_tmp24F;enum Cyc_Absyn_AggrKind _tmp24E;struct Cyc_List_List*_tmp253;struct Cyc_List_List*_tmp252;struct Cyc_Absyn_FnInfo _tmp255;struct Cyc_Absyn_FnInfo _tmp254;void*_tmp25D;struct Cyc_Absyn_Exp*_tmp25C;struct Cyc_Absyn_Tqual _tmp25B;void*_tmp25A;void*_tmp259;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Tqual _tmp257;void*_tmp256;void*_tmp269;void*_tmp268;void*_tmp267;void*_tmp266;struct Cyc_Absyn_Tqual _tmp265;void*_tmp264;void*_tmp263;void*_tmp262;void*_tmp261;void*_tmp260;struct Cyc_Absyn_Tqual _tmp25F;void*_tmp25E;struct Cyc_Absyn_Tvar*_tmp26B;struct Cyc_Absyn_Tvar*_tmp26A;struct Cyc_List_List*_tmp26F;void*_tmp26E;struct Cyc_List_List*_tmp26D;void*_tmp26C;switch(*((int*)_tmp249.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f2)->tag == 0U){_LL1: _tmp26C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp26D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f1)->f2;_tmp26E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f2)->f1;_tmp26F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f2)->f2;_LL2: {void*c1=_tmp26C;struct Cyc_List_List*ts1=_tmp26D;void*c2=_tmp26E;struct Cyc_List_List*ts2=_tmp26F;
# 1308
int _tmp270=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp270;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp249.f2)->tag == 1U){_LL3: _LL4:
# 1312
({void*_tmp271=0U;({struct _fat_ptr _tmp6D2=({const char*_tmp272="typecmp: can only compare closed types";_tag_fat(_tmp272,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D2,_tag_fat(_tmp271,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp249.f2)->tag == 2U){_LL5: _tmp26A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp26B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp249.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp26A;struct Cyc_Absyn_Tvar*tv1=_tmp26B;
# 1316
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->tag == 3U){_LL7: _tmp25E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).elt_type;_tmp25F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).elt_tq;_tmp260=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).ptr_atts).rgn;_tmp261=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).ptr_atts).nullable;_tmp262=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).ptr_atts).bounds;_tmp263=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).ptr_atts).zero_term;_tmp264=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).elt_type;_tmp265=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).elt_tq;_tmp266=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).ptr_atts).rgn;_tmp267=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).ptr_atts).nullable;_tmp268=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).ptr_atts).bounds;_tmp269=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp25E;struct Cyc_Absyn_Tqual tqual2a=_tmp25F;void*rgn2=_tmp260;void*null2a=_tmp261;void*b2=_tmp262;void*zt2=_tmp263;void*t1a=_tmp264;struct Cyc_Absyn_Tqual tqual1a=_tmp265;void*rgn1=_tmp266;void*null1a=_tmp267;void*b1=_tmp268;void*zt1=_tmp269;
# 1320
int _tmp273=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp273;
if(etc != 0)return etc;{
int _tmp274=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp274;
if(rc != 0)return rc;{
int _tmp275=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp275;
if(tqc != 0)return tqc;{
int _tmp276=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp276;
if(cc != 0)return cc;{
int _tmp277=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp277;
if(zc != 0)return zc;{
int _tmp278=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp278;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->tag == 4U){_LL9: _tmp256=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f1)->f1).elt_type;_tmp257=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f1)->f1).tq;_tmp258=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f1)->f1).num_elts;_tmp259=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f1)->f1).zero_term;_tmp25A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->f1).elt_type;_tmp25B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->f1).tq;_tmp25C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->f1).num_elts;_tmp25D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->f1).zero_term;_LLA: {void*t2a=_tmp256;struct Cyc_Absyn_Tqual tq2a=_tmp257;struct Cyc_Absyn_Exp*e1=_tmp258;void*zt1=_tmp259;void*t1a=_tmp25A;struct Cyc_Absyn_Tqual tq1a=_tmp25B;struct Cyc_Absyn_Exp*e2=_tmp25C;void*zt2=_tmp25D;
# 1336
int _tmp279=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp279;
if(tqc != 0)return tqc;{
int _tmp27A=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp27A;
if(tc != 0)return tc;{
int _tmp27B=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp27B;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({void*_tmp27C=0U;({struct _fat_ptr _tmp6D3=({const char*_tmp27D="missing expression in array index";_tag_fat(_tmp27D,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D3,_tag_fat(_tmp27C,sizeof(void*),0U));});});
# 1346
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp249.f2)->tag == 5U){_LLB: _tmp254=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp255=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp249.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp254;struct Cyc_Absyn_FnInfo f2=_tmp255;
# 1349
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp27E=f1.args;struct Cyc_List_List*args1=_tmp27E;
struct Cyc_List_List*_tmp27F=f2.args;struct Cyc_List_List*args2=_tmp27F;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp280=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3E=_tmp280;struct _tuple9 _tmp281=_stmttmp3E;void*_tmp283;struct Cyc_Absyn_Tqual _tmp282;_LL18: _tmp282=_tmp281.f2;_tmp283=_tmp281.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp282;void*t1=_tmp283;
struct _tuple9 _tmp284=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp3F=_tmp284;struct _tuple9 _tmp285=_stmttmp3F;void*_tmp287;struct Cyc_Absyn_Tqual _tmp286;_LL1B: _tmp286=_tmp285.f2;_tmp287=_tmp285.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp286;void*t2=_tmp287;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1364
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp6D4=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp6D4,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1378
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp288=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp288;
struct Cyc_List_List*_tmp289=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp289;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp28A=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp40=_tmp28A;struct _tuple15 _tmp28B=_stmttmp40;void*_tmp28D;void*_tmp28C;_LL1E: _tmp28C=_tmp28B.f1;_tmp28D=_tmp28B.f2;_LL1F: {void*t1a=_tmp28C;void*t1b=_tmp28D;
struct _tuple15 _tmp28E=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp41=_tmp28E;struct _tuple15 _tmp28F=_stmttmp41;void*_tmp291;void*_tmp290;_LL21: _tmp290=_tmp28F.f1;_tmp291=_tmp28F.f2;_LL22: {void*t2a=_tmp290;void*t2b=_tmp291;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1388
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1396
({void*_tmp292=0U;({struct _fat_ptr _tmp6D5=({const char*_tmp293="typecmp: function type comparison should never get here!";_tag_fat(_tmp293,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D5,_tag_fat(_tmp292,sizeof(void*),0U));});});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp249.f2)->tag == 6U){_LLD: _tmp252=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp253=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp249.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp252;struct Cyc_List_List*ts1=_tmp253;
# 1399
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f2)->tag == 7U){_LLF: _tmp24E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp24F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f1)->f2;_tmp250=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f2)->f1;_tmp251=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp24E;struct Cyc_List_List*fs2=_tmp24F;enum Cyc_Absyn_AggrKind k1=_tmp250;struct Cyc_List_List*fs1=_tmp251;
# 1402
if((int)k1 != (int)k2){
if((int)k1 == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp249.f2)->tag == 9U){_LL11: _tmp24C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp24D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp249.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp24C;struct Cyc_Absyn_Exp*e2=_tmp24D;
# 1407
_tmp24A=e1;_tmp24B=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp249.f2)->tag == 11U){_LL13: _tmp24A=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp24B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp249.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp24A;struct Cyc_Absyn_Exp*e2=_tmp24B;
# 1409
return Cyc_Evexp_const_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp294=0U;({struct _fat_ptr _tmp6D6=({const char*_tmp295="Unmatched case in typecmp";_tag_fat(_tmp295,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D6,_tag_fat(_tmp294,sizeof(void*),0U));});});}_LL0:;}}}
# 1416
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp296=({struct _tuple15 _tmp609;({void*_tmp6D8=Cyc_Tcutil_compress(t1);_tmp609.f1=_tmp6D8;}),({void*_tmp6D7=Cyc_Tcutil_compress(t2);_tmp609.f2=_tmp6D7;});_tmp609;});struct _tuple15 _stmttmp42=_tmp296;struct _tuple15 _tmp297=_stmttmp42;void*_tmp299;void*_tmp298;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f2)->tag == 0U){_LL1: _tmp298=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f1)->f1;_tmp299=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f2)->f1;_LL2: {void*c1=_tmp298;void*c2=_tmp299;
# 1419
struct _tuple15 _tmp29A=({struct _tuple15 _tmp608;_tmp608.f1=c1,_tmp608.f2=c2;_tmp608;});struct _tuple15 _stmttmp43=_tmp29A;struct _tuple15 _tmp29B=_stmttmp43;int _tmp29D;int _tmp29C;switch(*((int*)_tmp29B.f1)){case 2U: switch(*((int*)_tmp29B.f2)){case 2U: _LL6: _tmp29C=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29B.f1)->f1;_tmp29D=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29B.f2)->f1;_LL7: {int i1=_tmp29C;int i2=_tmp29D;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp29B.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1427
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1432
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1435
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29B.f2)->f1 == 0){_LL14: _LL15:
# 1430
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp29B.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1428
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1433
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1436
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29B.f2)->f1 == 0){_LL16: _LL17:
# 1431
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1437
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29B.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1434
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1438
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1440
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1442
 return 0;}_LL0:;}
# 1446
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp29E=({struct _tuple15 _tmp60B;({void*_tmp6DA=Cyc_Tcutil_compress(t1);_tmp60B.f1=_tmp6DA;}),({void*_tmp6D9=Cyc_Tcutil_compress(t2);_tmp60B.f2=_tmp6D9;});_tmp60B;});struct _tuple15 _stmttmp44=_tmp29E;struct _tuple15 _tmp29F=_stmttmp44;void*_tmp2A1;void*_tmp2A0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f2)->tag == 0U){_LL1: _tmp2A0=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f1)->f1;_tmp2A1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f2)->f1;_LL2: {void*c1=_tmp2A0;void*c2=_tmp2A1;
# 1449
{struct _tuple15 _tmp2A2=({struct _tuple15 _tmp60A;_tmp60A.f1=c1,_tmp60A.f2=c2;_tmp60A;});struct _tuple15 _stmttmp45=_tmp2A2;struct _tuple15 _tmp2A3=_stmttmp45;int _tmp2A5;int _tmp2A4;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 2U){_LL6: _tmp2A4=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f1;_tmp2A5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f1;_LL7: {int i1=_tmp2A4;int i2=_tmp2A5;
# 1451
if(i1 != 0 && i1 != 1)return t1;else{
if(i2 != 0 && i2 != 1)return t2;else{
if(i1 >= i2)return t1;else{
return t2;}}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1461
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1466
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1459
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1468
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1458
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1462
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1467
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1460
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A3.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1469
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f1)->tag == 4U){_LL18: _LL19:
# 1464
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A3.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1470
 goto _LL5;}}}}}}_LL5:;}
# 1472
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1475
return Cyc_Absyn_sint_type;}
# 1480
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1483
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 ||
 Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2A6=_cycalloc(sizeof(*_tmp2A6));_tmp2A6->v=t1;_tmp2A6;});}}}
# 1492
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1496
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp2A9=({struct Cyc_String_pa_PrintArg_struct _tmp60D;_tmp60D.tag=0U,({
struct _fat_ptr _tmp6DB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp60D.f1=_tmp6DB;});_tmp60D;});struct Cyc_String_pa_PrintArg_struct _tmp2AA=({struct Cyc_String_pa_PrintArg_struct _tmp60C;_tmp60C.tag=0U,({struct _fat_ptr _tmp6DC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp60C.f1=_tmp6DC;});_tmp60C;});void*_tmp2A7[2U];_tmp2A7[0]=& _tmp2A9,_tmp2A7[1]=& _tmp2AA;({unsigned _tmp6DE=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _fat_ptr _tmp6DD=({const char*_tmp2A8="type mismatch: expecting %s but found %s";_tag_fat(_tmp2A8,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp6DE,_tmp6DD,_tag_fat(_tmp2A7,sizeof(void*),2U));});});
return 0;}}}
# 1502
return 1;}
# 1507
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2AB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp46=_tmp2AB;void*_tmp2AC=_stmttmp46;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AC)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1513
return 1;}
# 1517
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 1521
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp2AD=0U;({unsigned _tmp6E0=e->loc;struct _fat_ptr _tmp6DF=({const char*_tmp2AE="integral size mismatch; conversion supplied";_tag_fat(_tmp2AE,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6E0,_tmp6DF,_tag_fat(_tmp2AD,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1527
return 0;}
# 1531
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 1535
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp2AF=0U;({unsigned _tmp6E2=e->loc;struct _fat_ptr _tmp6E1=({const char*_tmp2B0="integral size mismatch; conversion supplied";_tag_fat(_tmp2B0,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6E2,_tmp6E1,_tag_fat(_tmp2AF,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1541
return 0;}
# 1546
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1552
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp6E3=t;Cyc_Unify_unify(_tmp6E3,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1557
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2B1=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2B1;
void*_tmp2B2=t;struct Cyc_Absyn_Exp*_tmp2B3;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B2)->tag == 9U){_LL1: _tmp2B3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B2)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2B3;
return e;}}else{_LL3: _LL4: {
# 1562
struct Cyc_Absyn_Exp*_tmp2B4=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2B4;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1571
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B5=Cyc_Tcutil_compress(b);void*_stmttmp47=_tmp2B5;void*_tmp2B6=_stmttmp47;struct Cyc_List_List*_tmp2B7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B6)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B6)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2B7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B6)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2B7;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp2BA=({struct Cyc_String_pa_PrintArg_struct _tmp60E;_tmp60E.tag=0U,({struct _fat_ptr _tmp6E4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp60E.f1=_tmp6E4;});_tmp60E;});void*_tmp2B8[1U];_tmp2B8[0]=& _tmp2BA;({struct _fat_ptr _tmp6E5=({const char*_tmp2B9="get_bounds_exp: %s";_tag_fat(_tmp2B9,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E5,_tag_fat(_tmp2B8,sizeof(void*),1U));});});}_LL0:;}}
# 1580
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2BB=Cyc_Tcutil_compress(t);void*_stmttmp48=_tmp2BB;void*_tmp2BC=_stmttmp48;void*_tmp2BD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BC)->tag == 3U){_LL1: _tmp2BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BC)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2BD;
# 1583
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2C0=({struct Cyc_String_pa_PrintArg_struct _tmp60F;_tmp60F.tag=0U,({struct _fat_ptr _tmp6E6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp60F.f1=_tmp6E6;});_tmp60F;});void*_tmp2BE[1U];_tmp2BE[0]=& _tmp2C0;({struct _fat_ptr _tmp6E7=({const char*_tmp2BF="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp2BF,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E7,_tag_fat(_tmp2BE,sizeof(void*),1U));});});}_LL0:;}
# 1589
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2C1=_cycalloc(sizeof(*_tmp2C1));_tmp2C1->v=tvs;_tmp2C1;}));}
# 1593
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2->v=tvs;_tmp2C2;}));}
# 1598
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1606
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp2C3=({struct _tuple15 _tmp613;_tmp613.f1=t1,_tmp613.f2=t2;_tmp613;});struct _tuple15 _stmttmp49=_tmp2C3;struct _tuple15 _tmp2C4=_stmttmp49;void*_tmp2CC;struct Cyc_Absyn_Exp*_tmp2CB;struct Cyc_Absyn_Tqual _tmp2CA;void*_tmp2C9;void*_tmp2C8;struct Cyc_Absyn_Exp*_tmp2C7;struct Cyc_Absyn_Tqual _tmp2C6;void*_tmp2C5;struct Cyc_Absyn_PtrInfo _tmp2CE;struct Cyc_Absyn_PtrInfo _tmp2CD;switch(*((int*)_tmp2C4.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4.f2)->tag == 3U){_LL1: _tmp2CD=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4.f1)->f1;_tmp2CE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2CD;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2CE;
# 1611
int okay=1;
# 1613
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable))
# 1615
okay=!Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable);
# 1617
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple22 _tmp2CF=({struct _tuple22 _tmp610;({struct Cyc_Absyn_Exp*_tmp6EB=({void*_tmp6EA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6EA,(pinfo_a.ptr_atts).bounds);});_tmp610.f1=_tmp6EB;}),({
struct Cyc_Absyn_Exp*_tmp6E9=({void*_tmp6E8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6E8,(pinfo_b.ptr_atts).bounds);});_tmp610.f2=_tmp6E9;});_tmp610;});
# 1618
struct _tuple22 _stmttmp4A=_tmp2CF;struct _tuple22 _tmp2D0=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp2D2;struct Cyc_Absyn_Exp*_tmp2D1;if(_tmp2D0.f2 == 0){_LLA: _LLB:
# 1621
 okay=1;goto _LL9;}else{if(_tmp2D0.f1 == 0){_LLC: _LLD:
# 1624
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp6EC=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6EC,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp2D1=_tmp2D0.f1;_tmp2D2=_tmp2D0.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2D1;struct Cyc_Absyn_Exp*e2=_tmp2D2;
# 1630
okay=okay &&({struct Cyc_Absyn_Exp*_tmp6ED=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp6ED,(struct Cyc_Absyn_Exp*)_check_null(e1));});
# 1634
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({void*_tmp2D3=0U;({unsigned _tmp6EF=loc;struct _fat_ptr _tmp6EE=({const char*_tmp2D4="implicit cast to shorter array";_tag_fat(_tmp2D4,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp6EF,_tmp6EE,_tag_fat(_tmp2D3,sizeof(void*),0U));});});
goto _LL9;}}}_LL9:;}
# 1640
okay=okay &&(!(pinfo_a.elt_tq).real_const ||(pinfo_b.elt_tq).real_const);
# 1642
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
# 1645
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp2D7=({struct Cyc_String_pa_PrintArg_struct _tmp612;_tmp612.tag=0U,({
struct _fat_ptr _tmp6F0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((pinfo_a.ptr_atts).rgn));_tmp612.f1=_tmp6F0;});_tmp612;});struct Cyc_String_pa_PrintArg_struct _tmp2D8=({struct Cyc_String_pa_PrintArg_struct _tmp611;_tmp611.tag=0U,({
struct _fat_ptr _tmp6F1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((pinfo_b.ptr_atts).rgn));_tmp611.f1=_tmp6F1;});_tmp611;});void*_tmp2D5[2U];_tmp2D5[0]=& _tmp2D7,_tmp2D5[1]=& _tmp2D8;({unsigned _tmp6F3=loc;struct _fat_ptr _tmp6F2=({const char*_tmp2D6="implicit cast from region %s to region %s";_tag_fat(_tmp2D6,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6F3,_tmp6F2,_tag_fat(_tmp2D5,sizeof(void*),2U));});});}else{
okay=0;}}
# 1652
okay=okay &&(Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)||
# 1654
 Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)&&(pinfo_b.elt_tq).real_const);{
# 1662
int _tmp2D9=
({void*_tmp6F4=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6F4,(pinfo_b.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term);
# 1662
int deep_subtype=_tmp2D9;
# 1666
okay=okay &&(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type)||
(deep_subtype &&((pinfo_b.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type))))&& Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type));
return okay;}}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->tag == 4U){_LL3: _tmp2C5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f1)->f1).elt_type;_tmp2C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f1)->f1).tq;_tmp2C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f1)->f1).num_elts;_tmp2C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f1)->f1).zero_term;_tmp2C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->f1).elt_type;_tmp2CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->f1).tq;_tmp2CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->f1).num_elts;_tmp2CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2C5;struct Cyc_Absyn_Tqual tq1a=_tmp2C6;struct Cyc_Absyn_Exp*e1=_tmp2C7;void*zt1=_tmp2C8;void*t2a=_tmp2C9;struct Cyc_Absyn_Tqual tq2a=_tmp2CA;struct Cyc_Absyn_Exp*e2=_tmp2CB;void*zt2=_tmp2CC;
# 1672
int okay;
# 1675
okay=Cyc_Unify_unify(zt1,zt2)&&(
(e1 != 0 && e2 != 0)&& Cyc_Evexp_same_const_exp(e1,e2));
# 1678
return(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C4.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C4.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C4.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1680
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1682
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1686
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2DA=Cyc_Tcutil_compress(t);void*_stmttmp4B=_tmp2DA;void*_tmp2DB=_stmttmp4B;void*_tmp2DC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->tag == 3U){_LL1: _tmp2DC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->f1).elt_type;_LL2: {void*e=_tmp2DC;
return e;}}else{_LL3: _LL4:
({void*_tmp2DD=0U;({struct _fat_ptr _tmp6F5=({const char*_tmp2DE="pointer_elt_type";_tag_fat(_tmp2DE,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F5,_tag_fat(_tmp2DD,sizeof(void*),0U));});});}_LL0:;}
# 1692
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2DF=Cyc_Tcutil_compress(t);void*_stmttmp4C=_tmp2DF;void*_tmp2E0=_stmttmp4C;struct Cyc_Absyn_PtrAtts*_tmp2E1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E0)->tag == 3U){_LL1: _tmp2E1=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E0)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2E1;
return p->rgn;}}else{_LL3: _LL4:
({void*_tmp2E2=0U;({struct _fat_ptr _tmp6F6=({const char*_tmp2E3="pointer_elt_type";_tag_fat(_tmp2E3,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F6,_tag_fat(_tmp2E2,sizeof(void*),0U));});});}_LL0:;}
# 1699
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2E4=Cyc_Tcutil_compress(t);void*_stmttmp4D=_tmp2E4;void*_tmp2E5=_stmttmp4D;void*_tmp2E6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->tag == 3U){_LL1: _tmp2E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2E6;
# 1702
*rgn=r;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1711
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2E7=Cyc_Tcutil_compress(t);void*_stmttmp4E=_tmp2E7;void*_tmp2E8=_stmttmp4E;void*_tmp2EA;void*_tmp2E9;switch(*((int*)_tmp2E8)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E8)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->f1).ptr_atts).nullable;_tmp2EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2E9;void*bounds=_tmp2EA;
# 1719
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1725
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2EB=e->r;void*_stmttmp4F=_tmp2EB;void*_tmp2EC=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp2EE;void*_tmp2ED;struct _fat_ptr _tmp2EF;switch(*((int*)_tmp2EC)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2EF=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp2EF;
# 1734
unsigned long _tmp2F0=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2F0;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 1744
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1732
 return 1;case 14U: _LLD: _tmp2ED=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2EE=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_LLE: {void*t=_tmp2ED;struct Cyc_Absyn_Exp*e2=_tmp2EE;
# 1745
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1750
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1759
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1764
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1769
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1778
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1783
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1788
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2F1=ka;enum Cyc_Absyn_AliasQual _tmp2F3;enum Cyc_Absyn_KindQual _tmp2F2;_LL1: _tmp2F2=_tmp2F1->kind;_tmp2F3=_tmp2F1->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp2F2;enum Cyc_Absyn_AliasQual a=_tmp2F3;
{enum Cyc_Absyn_AliasQual _tmp2F4=a;switch(_tmp2F4){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1792
enum Cyc_Absyn_KindQual _tmp2F5=k;switch(_tmp2F5){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1803
{enum Cyc_Absyn_KindQual _tmp2F6=k;switch(_tmp2F6){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1810
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1812
{enum Cyc_Absyn_KindQual _tmp2F7=k;switch(_tmp2F7){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1819
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1822
({struct Cyc_String_pa_PrintArg_struct _tmp2FA=({struct Cyc_String_pa_PrintArg_struct _tmp614;_tmp614.tag=0U,({struct _fat_ptr _tmp6F7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp614.f1=_tmp6F7;});_tmp614;});void*_tmp2F8[1U];_tmp2F8[0]=& _tmp2FA;({struct _fat_ptr _tmp6F8=({const char*_tmp2F9="kind_to_opt: bad kind %s\n";_tag_fat(_tmp2F9,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F8,_tag_fat(_tmp2F8,sizeof(void*),1U));});});}}
# 1825
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->tag=0U,_tmp2FB->f1=k;_tmp2FB;});}
# 1828
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));({void*_tmp6F9=Cyc_Tcutil_kind_to_bound(k);_tmp2FC->v=_tmp6F9;});_tmp2FC;});}
# 1834
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp2FD=Cyc_Tcutil_compress(t2);void*_stmttmp50=_tmp2FD;void*_tmp2FE=_stmttmp50;void*_tmp2FF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->tag == 3U){_LL1: _tmp2FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp2FF;
# 1839
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp6FA=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp6FA;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1848
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1854
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1858
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp6FD=({struct _fat_ptr*_tmp312=_cycalloc(sizeof(*_tmp312));({struct _fat_ptr _tmp6FC=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp311=({struct Cyc_Int_pa_PrintArg_struct _tmp616;_tmp616.tag=1U,_tmp616.f1=(unsigned long)counter ++;_tmp616;});void*_tmp30F[1U];_tmp30F[0]=& _tmp311;({struct _fat_ptr _tmp6FB=({const char*_tmp310="__aliasvar%d";_tag_fat(_tmp310,sizeof(char),13U);});Cyc_aprintf(_tmp6FB,_tag_fat(_tmp30F,sizeof(void*),1U));});});*_tmp312=_tmp6FC;});_tmp312;});_tmp313->f2=_tmp6FD;});_tmp313;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp6FE=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E->tag=4U,_tmp30E->f1=vd;_tmp30E;});Cyc_Absyn_varb_exp(_tmp6FE,e->loc);});
# 1867
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 1869
{void*_tmp300=Cyc_Tcutil_compress(e_type);void*_stmttmp51=_tmp300;void*_tmp301=_stmttmp51;struct Cyc_Absyn_PtrLoc*_tmp308;void*_tmp307;void*_tmp306;void*_tmp305;void*_tmp304;struct Cyc_Absyn_Tqual _tmp303;void*_tmp302;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->tag == 3U){_LL1: _tmp302=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).elt_type;_tmp303=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).elt_tq;_tmp304=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).rgn;_tmp305=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).nullable;_tmp306=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).bounds;_tmp307=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).zero_term;_tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp302;struct Cyc_Absyn_Tqual tq=_tmp303;void*old_r=_tmp304;void*nb=_tmp305;void*b=_tmp306;void*zt=_tmp307;struct Cyc_Absyn_PtrLoc*pl=_tmp308;
# 1871
{void*_tmp309=Cyc_Tcutil_compress(old_r);void*_stmttmp52=_tmp309;void*_tmp30A=_stmttmp52;struct Cyc_Core_Opt*_tmp30C;void**_tmp30B;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30A)->tag == 1U){_LL6: _tmp30B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30A)->f2;_tmp30C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30A)->f4;_LL7: {void**topt=_tmp30B;struct Cyc_Core_Opt*ts=_tmp30C;
# 1873
void*_tmp30D=Cyc_Absyn_var_type(tv);void*new_r=_tmp30D;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1878
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1882
e->topt=0;
vd->initializer=0;{
# 1886
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1888
return({struct _tuple23 _tmp615;_tmp615.f1=d,_tmp615.f2=ve;_tmp615;});}}
# 1893
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1896
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1901
void*_tmp314=Cyc_Tcutil_compress(wants_type);void*_stmttmp53=_tmp314;void*_tmp315=_stmttmp53;void*_tmp316;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->tag == 3U){_LL1: _tmp316=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp316;
# 1903
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp317=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp317;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1909
return 0;}
# 1913
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1915
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1919
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1922
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1924
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp31A=({struct Cyc_String_pa_PrintArg_struct _tmp618;_tmp618.tag=0U,({
struct _fat_ptr _tmp6FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp618.f1=_tmp6FF;});_tmp618;});struct Cyc_String_pa_PrintArg_struct _tmp31B=({struct Cyc_String_pa_PrintArg_struct _tmp617;_tmp617.tag=0U,({struct _fat_ptr _tmp700=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp617.f1=_tmp700;});_tmp617;});void*_tmp318[2U];_tmp318[0]=& _tmp31A,_tmp318[1]=& _tmp31B;({unsigned _tmp702=e->loc;struct _fat_ptr _tmp701=({const char*_tmp319="integral size mismatch; %s -> %s conversion supplied";_tag_fat(_tmp319,sizeof(char),53U);});Cyc_Tcutil_warn(_tmp702,_tmp701,_tag_fat(_tmp318,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1932
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp61B;_tmp61B.tag=0U,({
struct _fat_ptr _tmp703=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp61B.f1=_tmp703;});_tmp61B;});struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp704=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp61A.f1=_tmp704;});_tmp61A;});struct Cyc_String_pa_PrintArg_struct _tmp320=({struct Cyc_String_pa_PrintArg_struct _tmp619;_tmp619.tag=0U,({struct _fat_ptr _tmp705=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp619.f1=_tmp705;});_tmp619;});void*_tmp31C[3U];_tmp31C[0]=& _tmp31E,_tmp31C[1]=& _tmp31F,_tmp31C[2]=& _tmp320;({unsigned _tmp707=e->loc;struct _fat_ptr _tmp706=({const char*_tmp31D="implicit alias coercion for %s:%s to %s";_tag_fat(_tmp31D,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp707,_tmp706,_tag_fat(_tmp31C,sizeof(void*),3U));});});
*alias_coercion=1;}
# 1939
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1943
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1947
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp323=({struct Cyc_String_pa_PrintArg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp708=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp61D.f1=_tmp708;});_tmp61D;});struct Cyc_String_pa_PrintArg_struct _tmp324=({struct Cyc_String_pa_PrintArg_struct _tmp61C;_tmp61C.tag=0U,({struct _fat_ptr _tmp709=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp61C.f1=_tmp709;});_tmp61C;});void*_tmp321[2U];_tmp321[0]=& _tmp323,_tmp321[1]=& _tmp324;({unsigned _tmp70B=e->loc;struct _fat_ptr _tmp70A=({const char*_tmp322="implicit cast from %s to %s";_tag_fat(_tmp322,sizeof(char),28U);});Cyc_Tcutil_warn(_tmp70B,_tmp70A,_tag_fat(_tmp321,sizeof(void*),2U));});});
return 1;}
# 1953
return 0;}
# 1959
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1962
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1976 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1979
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1982
struct _tuple24 _tmp325=*env;struct _tuple24 _stmttmp54=_tmp325;struct _tuple24 _tmp326=_stmttmp54;int _tmp329;struct _RegionHandle*_tmp328;struct Cyc_List_List*_tmp327;_LL1: _tmp327=_tmp326.f1;_tmp328=_tmp326.f2;_tmp329=_tmp326.f3;_LL2: {struct Cyc_List_List*inst=_tmp327;struct _RegionHandle*r=_tmp328;int flatten=_tmp329;
void*_tmp32A=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp32A;
struct Cyc_List_List*_tmp32B=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp32B;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp32D=_region_malloc(r,sizeof(*_tmp32D));({struct _tuple12*_tmp70C=({struct _tuple12*_tmp32C=_region_malloc(r,sizeof(*_tmp32C));_tmp32C->f1=x->tq,_tmp32C->f2=t;_tmp32C;});_tmp32D->hd=_tmp70C;}),_tmp32D->tl=0;_tmp32D;});
return ts;}}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1989
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 1991
struct _tuple25 _tmp32E=*env;struct _tuple25 _stmttmp55=_tmp32E;struct _tuple25 _tmp32F=_stmttmp55;int _tmp331;struct _RegionHandle*_tmp330;_LL1: _tmp330=_tmp32F.f1;_tmp331=_tmp32F.f2;_LL2: {struct _RegionHandle*r=_tmp330;int flatten=_tmp331;
struct _tuple12 _tmp332=*x;struct _tuple12 _stmttmp56=_tmp332;struct _tuple12 _tmp333=_stmttmp56;void*_tmp335;struct Cyc_Absyn_Tqual _tmp334;_LL4: _tmp334=_tmp333.f1;_tmp335=_tmp333.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp334;void*t=_tmp335;
struct Cyc_List_List*_tmp336=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp336;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp338=_region_malloc(r,sizeof(*_tmp338));({struct _tuple12*_tmp70D=({struct _tuple12*_tmp337=_region_malloc(r,sizeof(*_tmp337));_tmp337->f1=tq,_tmp337->f2=t;_tmp337;});_tmp338->hd=_tmp70D;}),_tmp338->tl=0;_tmp338;});
return ts;}}}
# 1998
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 2001
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp339=t1;struct Cyc_List_List*_tmp33A;struct Cyc_List_List*_tmp33B;struct Cyc_List_List*_tmp33D;struct Cyc_Absyn_Aggrdecl*_tmp33C;switch(*((int*)_tmp339)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp33C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f1)->f1).KnownAggr).val;_tmp33D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp33C;struct Cyc_List_List*ts=_tmp33D;
# 2019
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 2021
return({struct Cyc_List_List*_tmp348=_region_malloc(r,sizeof(*_tmp348));({struct _tuple12*_tmp70F=({struct _tuple12*_tmp347=_region_malloc(r,sizeof(*_tmp347));({struct Cyc_Absyn_Tqual _tmp70E=Cyc_Absyn_empty_tqual(0U);_tmp347->f1=_tmp70E;}),_tmp347->f2=t1;_tmp347;});_tmp348->hd=_tmp70F;}),_tmp348->tl=0;_tmp348;});{
struct Cyc_List_List*_tmp349=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp349;
struct _tuple24 env=({struct _tuple24 _tmp61E;_tmp61E.f1=inst,_tmp61E.f2=r,_tmp61E.f3=flatten;_tmp61E;});
struct Cyc_List_List*_tmp34A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp57=_tmp34A;struct Cyc_List_List*_tmp34B=_stmttmp57;struct Cyc_List_List*_tmp34D;struct Cyc_Absyn_Aggrfield*_tmp34C;if(_tmp34B == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp34C=(struct Cyc_Absyn_Aggrfield*)_tmp34B->hd;_tmp34D=_tmp34B->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp34C;struct Cyc_List_List*tl=_tmp34D;
# 2027
struct Cyc_List_List*_tmp34E=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp34E;
env.f3=0;{
struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp34F;
struct Cyc_List_List*_tmp350=({struct Cyc_List_List*_tmp351=_region_malloc(r,sizeof(*_tmp351));_tmp351->hd=hd2,_tmp351->tl=tl2;_tmp351;});struct Cyc_List_List*tts=_tmp350;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp33B=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp339)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp33B;
# 2006
struct _tuple25 _tmp33E=({struct _tuple25 _tmp61F;_tmp61F.f1=r,_tmp61F.f2=flatten;_tmp61F;});struct _tuple25 env=_tmp33E;
# 2008
struct Cyc_List_List*_tmp33F=tqs;struct Cyc_List_List*_tmp341;struct _tuple12*_tmp340;if(_tmp33F == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp340=(struct _tuple12*)_tmp33F->hd;_tmp341=_tmp33F->tl;_LLF: {struct _tuple12*hd=_tmp340;struct Cyc_List_List*tl=_tmp341;
# 2011
struct Cyc_List_List*_tmp342=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp342;
env.f2=0;{
struct Cyc_List_List*_tmp343=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp343;
struct Cyc_List_List*_tmp344=({struct Cyc_List_List*_tmp346=_region_malloc(r,sizeof(*_tmp346));_tmp346->hd=hd2,_tmp346->tl=tl2;_tmp346;});struct Cyc_List_List*temp=_tmp344;
return({struct _RegionHandle*_tmp710=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp710,({struct Cyc_List_List*_tmp345=_region_malloc(r,sizeof(*_tmp345));_tmp345->hd=hd2,_tmp345->tl=tl2;_tmp345;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp339)->f1 == Cyc_Absyn_StructA){_LL7: _tmp33A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp339)->f2;_LL8: {struct Cyc_List_List*fs=_tmp33A;
# 2034
struct _tuple24 env=({struct _tuple24 _tmp620;_tmp620.f1=0,_tmp620.f2=r,_tmp620.f3=flatten;_tmp620;});
struct Cyc_List_List*_tmp352=fs;struct Cyc_List_List*_tmp354;struct Cyc_Absyn_Aggrfield*_tmp353;if(_tmp352 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp353=(struct Cyc_Absyn_Aggrfield*)_tmp352->hd;_tmp354=_tmp352->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp353;struct Cyc_List_List*tl=_tmp354;
# 2038
struct Cyc_List_List*_tmp355=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp355;
env.f3=0;{
struct Cyc_List_List*_tmp356=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp356;
struct Cyc_List_List*_tmp357=({struct Cyc_List_List*_tmp358=_region_malloc(r,sizeof(*_tmp358));_tmp358->hd=hd2,_tmp358->tl=tl2;_tmp358;});struct Cyc_List_List*tts=_tmp357;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2044
 goto _LL0;}_LL0:;}}
# 2047
return({struct Cyc_List_List*_tmp35A=_region_malloc(r,sizeof(*_tmp35A));({struct _tuple12*_tmp712=({struct _tuple12*_tmp359=_region_malloc(r,sizeof(*_tmp359));({struct Cyc_Absyn_Tqual _tmp711=Cyc_Absyn_empty_tqual(0U);_tmp359->f1=_tmp711;}),_tmp359->f2=t1;_tmp359;});_tmp35A->hd=_tmp712;}),_tmp35A->tl=0;_tmp35A;});}
# 2051
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp35B=(void*)t->hd;void*_stmttmp58=_tmp35B;void*_tmp35C=_stmttmp58;switch(*((int*)_tmp35C)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
# 2058
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2061
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2064
return 1;}
# 2067
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp35D=({struct _tuple15 _tmp621;({void*_tmp714=Cyc_Tcutil_compress(t1);_tmp621.f1=_tmp714;}),({void*_tmp713=Cyc_Tcutil_compress(t2);_tmp621.f2=_tmp713;});_tmp621;});struct _tuple15 _stmttmp59=_tmp35D;struct _tuple15 _tmp35E=_stmttmp59;enum Cyc_Absyn_Size_of _tmp360;enum Cyc_Absyn_Size_of _tmp35F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->tag == 1U){_LL1: _tmp35F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->f2;_tmp360=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp35F;enum Cyc_Absyn_Size_of b2=_tmp360;
# 2070
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2078
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2080
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp361=({struct _tuple15 _tmp622;_tmp622.f1=t1,_tmp622.f2=t2;_tmp622;});struct _tuple15 _stmttmp5A=_tmp361;struct _tuple15 _tmp362=_stmttmp5A;struct Cyc_Absyn_FnInfo _tmp364;struct Cyc_Absyn_FnInfo _tmp363;struct Cyc_List_List*_tmp369;struct Cyc_Absyn_Datatypedecl*_tmp368;struct Cyc_List_List*_tmp367;struct Cyc_Absyn_Datatypefield*_tmp366;struct Cyc_Absyn_Datatypedecl*_tmp365;void*_tmp375;void*_tmp374;void*_tmp373;void*_tmp372;struct Cyc_Absyn_Tqual _tmp371;void*_tmp370;void*_tmp36F;void*_tmp36E;void*_tmp36D;void*_tmp36C;struct Cyc_Absyn_Tqual _tmp36B;void*_tmp36A;switch(*((int*)_tmp362.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->tag == 3U){_LL1: _tmp36A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).elt_type;_tmp36B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).elt_tq;_tmp36C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).ptr_atts).rgn;_tmp36D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).ptr_atts).nullable;_tmp36E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).ptr_atts).bounds;_tmp36F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).ptr_atts).zero_term;_tmp370=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).elt_type;_tmp371=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).elt_tq;_tmp372=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).ptr_atts).rgn;_tmp373=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).ptr_atts).nullable;_tmp374=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).ptr_atts).bounds;_tmp375=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp36A;struct Cyc_Absyn_Tqual q_a=_tmp36B;void*rt_a=_tmp36C;void*null_a=_tmp36D;void*b_a=_tmp36E;void*zt_a=_tmp36F;void*t_b=_tmp370;struct Cyc_Absyn_Tqual q_b=_tmp371;void*rt_b=_tmp372;void*null_b=_tmp373;void*b_b=_tmp374;void*zt_b=_tmp375;
# 2092
if(q_a.real_const && !q_b.real_const)
return 0;
# 2095
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 2099
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 2103
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 2107
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp376=({void*_tmp715=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp715,b_a);});struct Cyc_Absyn_Exp*e1=_tmp376;
struct Cyc_Absyn_Exp*_tmp377=({void*_tmp716=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp716,b_b);});struct Cyc_Absyn_Exp*e2=_tmp377;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 2117
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 2123
int _tmp378=
({void*_tmp717=b_b;Cyc_Unify_unify(_tmp717,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2123
int deep_subtype=_tmp378;
# 2128
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp71B=po;struct Cyc_List_List*_tmp71A=({struct Cyc_List_List*_tmp37A=_cycalloc(sizeof(*_tmp37A));({struct _tuple15*_tmp718=({struct _tuple15*_tmp379=_cycalloc(sizeof(*_tmp379));_tmp379->f1=t1,_tmp379->f2=t2;_tmp379;});_tmp37A->hd=_tmp718;}),_tmp37A->tl=assume;_tmp37A;});void*_tmp719=t_a;Cyc_Tcutil_ptrsubtype(_tmp71B,_tmp71A,_tmp719,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp365=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp366=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp367=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f2;_tmp368=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->f1)->f1).KnownDatatype).val;_tmp369=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp365;struct Cyc_Absyn_Datatypefield*df=_tmp366;struct Cyc_List_List*ts1=_tmp367;struct Cyc_Absyn_Datatypedecl*dd2=_tmp368;struct Cyc_List_List*ts2=_tmp369;
# 2135
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 2137
if(({int _tmp71C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp71C != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp362.f2)->tag == 5U){_LL5: _tmp363=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp362.f1)->f1;_tmp364=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp362.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp363;struct Cyc_Absyn_FnInfo f2=_tmp364;
# 2144
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp37B=f1.tvars;struct Cyc_List_List*tvs1=_tmp37B;
struct Cyc_List_List*_tmp37C=f2.tvars;struct Cyc_List_List*tvs2=_tmp37C;
if(({int _tmp71D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp71D != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp37E=_cycalloc(sizeof(*_tmp37E));({struct _tuple19*_tmp71F=({struct _tuple19*_tmp37D=_cycalloc(sizeof(*_tmp37D));_tmp37D->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp71E=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp37D->f2=_tmp71E;});_tmp37D;});_tmp37E->hd=_tmp71F;}),_tmp37E->tl=inst;_tmp37E;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2155
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp722=po;struct Cyc_List_List*_tmp721=assume;void*_tmp720=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F->tag=5U,_tmp37F->f1=f1;_tmp37F;});Cyc_Tcutil_subtype(_tmp722,_tmp721,_tmp720,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp380=_cycalloc(sizeof(*_tmp380));_tmp380->tag=5U,_tmp380->f1=f2;_tmp380;}));});}}}
# 2162
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp381=f1.args;struct Cyc_List_List*args1=_tmp381;
struct Cyc_List_List*_tmp382=f2.args;struct Cyc_List_List*args2=_tmp382;
# 2167
if(({int _tmp723=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp723 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2169
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp383=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp5B=_tmp383;struct _tuple9 _tmp384=_stmttmp5B;void*_tmp386;struct Cyc_Absyn_Tqual _tmp385;_LLA: _tmp385=_tmp384.f2;_tmp386=_tmp384.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp385;void*t1=_tmp386;
struct _tuple9 _tmp387=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp5C=_tmp387;struct _tuple9 _tmp388=_stmttmp5C;void*_tmp38A;struct Cyc_Absyn_Tqual _tmp389;_LLD: _tmp389=_tmp388.f2;_tmp38A=_tmp388.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp389;void*t2=_tmp38A;
# 2173
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2177
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp38B=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp38B;
struct Cyc_Absyn_VarargInfo _tmp38C=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp38C;
# 2182
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2187
if(!({void*_tmp724=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp724,(void*)_check_null(f2.effect));}))return 0;
# 2189
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2191
if(!Cyc_Tcutil_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2193
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2195
return 0;
# 2197
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2199
return 0;
# 2201
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2212
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2216
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp38D=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp5D=_tmp38D;struct _tuple12*_tmp38E=_stmttmp5D;void*_tmp390;struct Cyc_Absyn_Tqual _tmp38F;_LL1: _tmp38F=_tmp38E->f1;_tmp390=_tmp38E->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp38F;void*t1a=_tmp390;
struct _tuple12*_tmp391=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp5E=_tmp391;struct _tuple12*_tmp392=_stmttmp5E;void*_tmp394;struct Cyc_Absyn_Tqual _tmp393;_LL4: _tmp393=_tmp392->f1;_tmp394=_tmp392->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp393;void*t2a=_tmp394;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a))
continue;
if(Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2232
return 1;}
# 2237
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2243
{void*_tmp395=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp395)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp395)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp395)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2246
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2248
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2252
void*_tmp396=t1;void*_tmp397;struct Cyc_Absyn_Enumdecl*_tmp398;void*_tmp39C;struct Cyc_Absyn_Exp*_tmp39B;struct Cyc_Absyn_Tqual _tmp39A;void*_tmp399;void*_tmp3A2;void*_tmp3A1;void*_tmp3A0;void*_tmp39F;struct Cyc_Absyn_Tqual _tmp39E;void*_tmp39D;switch(*((int*)_tmp396)){case 3U: _LLA: _tmp39D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).elt_type;_tmp39E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).elt_tq;_tmp39F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).ptr_atts).rgn;_tmp3A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).ptr_atts).nullable;_tmp3A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).ptr_atts).bounds;_tmp3A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp39D;struct Cyc_Absyn_Tqual q_a=_tmp39E;void*rt_a=_tmp39F;void*null_a=_tmp3A0;void*b_a=_tmp3A1;void*zt_a=_tmp3A2;
# 2261
{void*_tmp3A3=t2;void*_tmp3A9;void*_tmp3A8;void*_tmp3A7;void*_tmp3A6;struct Cyc_Absyn_Tqual _tmp3A5;void*_tmp3A4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A3)->tag == 3U){_LL19: _tmp3A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A3)->f1).elt_type;_tmp3A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A3)->f1).elt_tq;_tmp3A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A3)->f1).ptr_atts).rgn;_tmp3A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A3)->f1).ptr_atts).nullable;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A3)->f1).ptr_atts).bounds;_tmp3A9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A3)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3A4;struct Cyc_Absyn_Tqual q_b=_tmp3A5;void*rt_b=_tmp3A6;void*null_b=_tmp3A7;void*b_b=_tmp3A8;void*zt_b=_tmp3A9;
# 2263
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3AA=({struct Cyc_List_List*_tmp3B2=_cycalloc(sizeof(*_tmp3B2));({struct _tuple15*_tmp725=({struct _tuple15*_tmp3B1=_cycalloc(sizeof(*_tmp3B1));_tmp3B1->f1=t1,_tmp3B1->f2=t2;_tmp3B1;});_tmp3B2->hd=_tmp725;}),_tmp3B2->tl=0;_tmp3B2;});struct Cyc_List_List*assump=_tmp3AA;
int _tmp3AB=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3AB;
# 2276 "tcutil.cyc"
int _tmp3AC=
({void*_tmp726=b_b;Cyc_Unify_unify(_tmp726,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2276
int deep_castable=_tmp3AC;
# 2279
int _tmp3AD=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2279
int ptrsub=_tmp3AD;
# 2282
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2284
int _tmp3AE=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2284
int bitcase=_tmp3AE;
# 2287
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3AF=({void*_tmp727=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp727,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3AF;
struct Cyc_Absyn_Exp*_tmp3B0=({void*_tmp728=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp728,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3B0;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2296
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2301
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2307
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp399=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).elt_type;_tmp39A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).tq;_tmp39B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).num_elts;_tmp39C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).zero_term;_LLD: {void*t1a=_tmp399;struct Cyc_Absyn_Tqual tq1a=_tmp39A;struct Cyc_Absyn_Exp*e1=_tmp39B;void*zt1=_tmp39C;
# 2309
{void*_tmp3B3=t2;void*_tmp3B7;struct Cyc_Absyn_Exp*_tmp3B6;struct Cyc_Absyn_Tqual _tmp3B5;void*_tmp3B4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->tag == 4U){_LL1E: _tmp3B4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->f1).elt_type;_tmp3B5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->f1).tq;_tmp3B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->f1).num_elts;_tmp3B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B3)->f1).zero_term;_LL1F: {void*t2a=_tmp3B4;struct Cyc_Absyn_Tqual tq2a=_tmp3B5;struct Cyc_Absyn_Exp*e2=_tmp3B6;void*zt2=_tmp3B7;
# 2311
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2316
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2318
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2320
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp396)->f1)){case 15U: _LLE: _tmp398=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp396)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp398;
# 2324
{void*_tmp3B8=t2;struct Cyc_Absyn_Enumdecl*_tmp3B9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B8)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B8)->f1)->tag == 15U){_LL23: _tmp3B9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3B8)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3B9;
# 2326
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp729=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp729 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2332
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2335
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp396)->f2 != 0){_LL14: _tmp397=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp396)->f2)->hd;_LL15: {void*r1=_tmp397;
# 2338
{void*_tmp3BA=t2;void*_tmp3BB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BA)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BA)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BA)->f2 != 0){_LL28: _tmp3BB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BA)->f2)->hd;_LL29: {void*r2=_tmp3BB;
# 2340
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2344
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2350
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3BC=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3BC;
({void*_tmp72A=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->tag=14U,_tmp3BD->f1=t,_tmp3BD->f2=inner,_tmp3BD->f3=0,_tmp3BD->f4=c;_tmp3BD;});e->r=_tmp72A;});
e->topt=t;}}
# 2359
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2364
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3C3=({struct Cyc_Int_pa_PrintArg_struct _tmp623;_tmp623.tag=1U,_tmp623.f1=(unsigned long)i;_tmp623;});void*_tmp3C1[1U];_tmp3C1[0]=& _tmp3C3;({struct _fat_ptr _tmp72B=({const char*_tmp3C2="#%d";_tag_fat(_tmp3C2,sizeof(char),4U);});Cyc_aprintf(_tmp72B,_tag_fat(_tmp3C1,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct _fat_ptr*_tmp72C=({unsigned _tmp3BF=1;struct _fat_ptr*_tmp3BE=_cycalloc(_check_times(_tmp3BF,sizeof(struct _fat_ptr)));_tmp3BE[0]=s;_tmp3BE;});_tmp3C0->name=_tmp72C;}),_tmp3C0->identity=- 1,_tmp3C0->kind=k;_tmp3C0;});}
# 2371
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp3C4=*t->name;struct _fat_ptr s=_tmp3C4;
return(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'#';}
# 2377
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2384
struct Cyc_List_List*_tmp3C5=0;struct Cyc_List_List*fn_type_atts=_tmp3C5;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->hd=(void*)atts->hd,_tmp3C6->tl=fn_type_atts;_tmp3C6;});}}{
struct Cyc_Absyn_FnInfo _tmp3C7=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3C7;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->tag=5U,_tmp3C8->f1=type_info;_tmp3C8;});}}
# 2392
return(void*)_check_null(fd->cached_type);}
# 2398
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2400
union Cyc_Relations_RelnOp _tmp3C9=*rop;union Cyc_Relations_RelnOp _stmttmp5F=_tmp3C9;union Cyc_Relations_RelnOp _tmp3CA=_stmttmp5F;struct Cyc_Absyn_Vardecl*_tmp3CB;struct Cyc_Absyn_Vardecl*_tmp3CC;switch((_tmp3CA.RNumelts).tag){case 2U: _LL1: _tmp3CC=(_tmp3CA.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3CC;
# 2402
struct _tuple1 _tmp3CD=*vd->name;struct _tuple1 _stmttmp60=_tmp3CD;struct _tuple1 _tmp3CE=_stmttmp60;struct _fat_ptr*_tmp3D0;union Cyc_Absyn_Nmspace _tmp3CF;_LL8: _tmp3CF=_tmp3CE.f1;_tmp3D0=_tmp3CE.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3CF;struct _fat_ptr*var=_tmp3D0;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp72D=(struct _fat_ptr)*var;Cyc_strcmp(_tmp72D,({const char*_tmp3D1="return_value";_tag_fat(_tmp3D1,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp72E=Cyc_Relations_RReturn();*rop=_tmp72E;});
goto _LL0;}{
# 2408
unsigned c=0U;
{struct Cyc_List_List*_tmp3D2=args;struct Cyc_List_List*a=_tmp3D2;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3D3=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp61=_tmp3D3;struct _tuple9*_tmp3D4=_stmttmp61;struct _fat_ptr*_tmp3D5;_LLB: _tmp3D5=_tmp3D4->f1;_LLC: {struct _fat_ptr*vopt=_tmp3D5;
if(vopt != 0){
if(Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp72F=Cyc_Relations_RParam(c);*rop=_tmp72F;});
break;}}}}}
# 2418
goto _LL0;}}}case 3U: _LL3: _tmp3CB=(_tmp3CA.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3CB;
# 2420
struct _tuple1 _tmp3D6=*vd->name;struct _tuple1 _stmttmp62=_tmp3D6;struct _tuple1 _tmp3D7=_stmttmp62;struct _fat_ptr*_tmp3D9;union Cyc_Absyn_Nmspace _tmp3D8;_LLE: _tmp3D8=_tmp3D7.f1;_tmp3D9=_tmp3D7.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3D8;struct _fat_ptr*var=_tmp3D9;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3DA=args;struct Cyc_List_List*a=_tmp3DA;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3DB=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp63=_tmp3DB;struct _tuple9*_tmp3DC=_stmttmp63;struct _fat_ptr*_tmp3DD;_LL11: _tmp3DD=_tmp3DC->f1;_LL12: {struct _fat_ptr*vopt=_tmp3DD;
if(vopt != 0){
if(Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp730=Cyc_Relations_RParamNumelts(c);*rop=_tmp730;});
break;}}}}}
# 2432
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2437
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2439
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2443
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2445
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3DE=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3DE;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2452
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3DF=pr;void*_tmp3E1;struct Cyc_Absyn_Tqual _tmp3E0;_LL1: _tmp3E0=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3E0;void*t2=_tmp3E1;
if(t2 == t)return pr;else{
return({struct _tuple12*_tmp3E2=_cycalloc(sizeof(*_tmp3E2));_tmp3E2->f1=tq,_tmp3E2->f2=t;_tmp3E2;});}}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2458
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2460
return({struct _tuple27*_tmp3E4=_region_malloc(rgn,sizeof(*_tmp3E4));({struct _tuple26*_tmp731=({struct _tuple26*_tmp3E3=_region_malloc(rgn,sizeof(*_tmp3E3));_tmp3E3->f1=(*y).f1,_tmp3E3->f2=(*y).f2;_tmp3E3;});_tmp3E4->f1=_tmp731;}),_tmp3E4->f2=(*y).f3;_tmp3E4;});}
# 2462
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2464
struct _tuple9 _tmp3E5=*orig_arg;struct _tuple9 _stmttmp64=_tmp3E5;struct _tuple9 _tmp3E6=_stmttmp64;void*_tmp3E9;struct Cyc_Absyn_Tqual _tmp3E8;struct _fat_ptr*_tmp3E7;_LL1: _tmp3E7=_tmp3E6.f1;_tmp3E8=_tmp3E6.f2;_tmp3E9=_tmp3E6.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp3E7;struct Cyc_Absyn_Tqual tq_orig=_tmp3E8;void*t_orig=_tmp3E9;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));_tmp3EA->f1=vopt_orig,_tmp3EA->f2=tq_orig,_tmp3EA->f3=t;_tmp3EA;});}}
# 2468
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2473
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2475
return({struct Cyc_Absyn_Exp*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->topt=old->topt,_tmp3EB->r=r,_tmp3EB->loc=old->loc,_tmp3EB->annot=old->annot;_tmp3EB;});}
# 2480
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3EC=e->r;void*_stmttmp65=_tmp3EC;void*_tmp3ED=_stmttmp65;void*_tmp3EE;struct Cyc_List_List*_tmp3F0;void*_tmp3EF;struct Cyc_Absyn_Exp*_tmp3F1;struct Cyc_Absyn_Exp*_tmp3F2;void*_tmp3F3;enum Cyc_Absyn_Coercion _tmp3F7;int _tmp3F6;struct Cyc_Absyn_Exp*_tmp3F5;void*_tmp3F4;struct Cyc_Absyn_Exp*_tmp3F9;struct Cyc_Absyn_Exp*_tmp3F8;struct Cyc_Absyn_Exp*_tmp3FB;struct Cyc_Absyn_Exp*_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FD;struct Cyc_Absyn_Exp*_tmp3FC;struct Cyc_Absyn_Exp*_tmp400;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_List_List*_tmp402;enum Cyc_Absyn_Primop _tmp401;switch(*((int*)_tmp3ED)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp401=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp402=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp401;struct Cyc_List_List*es=_tmp402;
# 2489
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp403=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp403;
struct Cyc_Absyn_Exp*_tmp404=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp404;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp733=e;Cyc_Tcutil_copye(_tmp733,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->tag=3U,_tmp406->f1=p,({struct Cyc_List_List*_tmp732=({struct Cyc_Absyn_Exp*_tmp405[1U];_tmp405[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp405,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp406->f2=_tmp732;});_tmp406;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp407=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp407;
struct Cyc_Absyn_Exp*_tmp408=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp408;
struct Cyc_Absyn_Exp*_tmp409=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp409;
struct Cyc_Absyn_Exp*_tmp40A=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp40A;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp735=e;Cyc_Tcutil_copye(_tmp735,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->tag=3U,_tmp40C->f1=p,({struct Cyc_List_List*_tmp734=({struct Cyc_Absyn_Exp*_tmp40B[2U];_tmp40B[0]=new_e1,_tmp40B[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp40B,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp40C->f2=_tmp734;});_tmp40C;}));});}else{
return({void*_tmp40D=0U;({struct _fat_ptr _tmp736=({const char*_tmp40E="primop does not have 1 or 2 args!";_tag_fat(_tmp40E,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp736,_tag_fat(_tmp40D,sizeof(void*),0U));});});}}}case 6U: _LLD: _tmp3FE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp3FF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_tmp400=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3ED)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp3FE;struct Cyc_Absyn_Exp*e2=_tmp3FF;struct Cyc_Absyn_Exp*e3=_tmp400;
# 2503
struct Cyc_Absyn_Exp*_tmp40F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40F;
struct Cyc_Absyn_Exp*_tmp410=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp410;
struct Cyc_Absyn_Exp*_tmp411=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp411;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp737=e;Cyc_Tcutil_copye(_tmp737,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->tag=6U,_tmp412->f1=new_e1,_tmp412->f2=new_e2,_tmp412->f3=new_e3;_tmp412;}));});}case 7U: _LLF: _tmp3FC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp3FD=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp3FC;struct Cyc_Absyn_Exp*e2=_tmp3FD;
# 2509
struct Cyc_Absyn_Exp*_tmp413=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp413;
struct Cyc_Absyn_Exp*_tmp414=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp414;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp738=e;Cyc_Tcutil_copye(_tmp738,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->tag=7U,_tmp415->f1=new_e1,_tmp415->f2=new_e2;_tmp415;}));});}case 8U: _LL11: _tmp3FA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp3FB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp3FA;struct Cyc_Absyn_Exp*e2=_tmp3FB;
# 2514
struct Cyc_Absyn_Exp*_tmp416=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp416;
struct Cyc_Absyn_Exp*_tmp417=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp417;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp739=e;Cyc_Tcutil_copye(_tmp739,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->tag=8U,_tmp418->f1=new_e1,_tmp418->f2=new_e2;_tmp418;}));});}case 9U: _LL13: _tmp3F8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp3F9=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp3F8;struct Cyc_Absyn_Exp*e2=_tmp3F9;
# 2519
struct Cyc_Absyn_Exp*_tmp419=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp419;
struct Cyc_Absyn_Exp*_tmp41A=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp41A;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp73A=e;Cyc_Tcutil_copye(_tmp73A,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->tag=9U,_tmp41B->f1=new_e1,_tmp41B->f2=new_e2;_tmp41B;}));});}case 14U: _LL15: _tmp3F4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp3F5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_tmp3F6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3ED)->f3;_tmp3F7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3ED)->f4;_LL16: {void*t=_tmp3F4;struct Cyc_Absyn_Exp*e1=_tmp3F5;int b=_tmp3F6;enum Cyc_Absyn_Coercion c=_tmp3F7;
# 2524
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41C;
void*_tmp41D=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp41D;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp73B=e;Cyc_Tcutil_copye(_tmp73B,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->tag=14U,_tmp41E->f1=new_typ,_tmp41E->f2=new_e1,_tmp41E->f3=b,_tmp41E->f4=c;_tmp41E;}));});}case 17U: _LL17: _tmp3F3=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_LL18: {void*t=_tmp3F3;
# 2529
void*_tmp41F=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp41F;
if(new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp73C=e;Cyc_Tcutil_copye(_tmp73C,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp420=_cycalloc(sizeof(*_tmp420));_tmp420->tag=17U,_tmp420->f1=new_typ;_tmp420;}));});}case 18U: _LL19: _tmp3F2=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp3F2;
# 2533
struct Cyc_Absyn_Exp*_tmp421=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp421;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp73D=e;Cyc_Tcutil_copye(_tmp73D,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->tag=18U,_tmp422->f1=new_e1;_tmp422;}));});}case 41U: _LL1B: _tmp3F1=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp3F1;
# 2537
struct Cyc_Absyn_Exp*_tmp423=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp423;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp73E=e;Cyc_Tcutil_copye(_tmp73E,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->tag=41U,_tmp424->f1=new_e1;_tmp424;}));});}case 19U: _LL1D: _tmp3EF=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp3F0=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LL1E: {void*t=_tmp3EF;struct Cyc_List_List*f=_tmp3F0;
# 2541
void*_tmp425=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp425;
if(new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp73F=e;Cyc_Tcutil_copye(_tmp73F,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->tag=19U,_tmp426->f1=new_typ,_tmp426->f2=f;_tmp426;}));});}case 39U: _LL1F: _tmp3EE=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_LL20: {void*t=_tmp3EE;
# 2545
void*_tmp427=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp427;
if(new_typ == t)return e;{
# 2548
void*_tmp428=Cyc_Tcutil_compress(new_typ);void*_stmttmp66=_tmp428;void*_tmp429=_stmttmp66;struct Cyc_Absyn_Exp*_tmp42A;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp429)->tag == 9U){_LL24: _tmp42A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp429)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp42A;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp740=e;Cyc_Tcutil_copye(_tmp740,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp42B=_cycalloc(sizeof(*_tmp42B));_tmp42B->tag=39U,_tmp42B->f1=new_typ;_tmp42B;}));});}_LL23:;}}default: _LL21: _LL22:
# 2552
 return({void*_tmp42C=0U;({struct _fat_ptr _tmp741=({const char*_tmp42D="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp42D,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp741,_tag_fat(_tmp42C,sizeof(void*),0U));});});}_LL0:;}
# 2556
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2559
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 2563
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2566
void*_tmp42E=f->type;void*t=_tmp42E;
struct Cyc_Absyn_Exp*_tmp42F=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp42F;
void*_tmp430=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_t=_tmp430;
struct Cyc_Absyn_Exp*_tmp431=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp431;
if(t == new_t && req == new_req)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->name=f->name,_tmp432->tq=f->tq,_tmp432->type=new_t,_tmp432->width=f->width,_tmp432->attributes=f->attributes,_tmp432->requires_clause=new_req;_tmp432;});}}
# 2576
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2579
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp433=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp433;
struct Cyc_List_List*_tmp434=fs->tl;struct Cyc_List_List*t=_tmp434;
struct Cyc_Absyn_Aggrfield*_tmp435=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp435;
struct Cyc_List_List*_tmp436=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_t=_tmp436;
if(new_f == f && new_t == t)return fs;
return({struct Cyc_List_List*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->hd=new_f,_tmp437->tl=new_t;_tmp437;});}}
# 2588
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2591
struct _tuple0 _tmp438=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp67=_tmp438;struct _tuple0 _tmp439=_stmttmp67;struct Cyc_List_List*_tmp43B;struct Cyc_List_List*_tmp43A;_LL1: _tmp43A=_tmp439.f1;_tmp43B=_tmp439.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp43A;struct Cyc_List_List*rpo1b=_tmp43B;
struct Cyc_List_List*_tmp43C=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp43C;
struct Cyc_List_List*_tmp43D=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp43D;
if(rpo2a == rpo1a && rpo2b == rpo1b)
return rgn_po;else{
# 2597
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}}
# 2600
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2603
void*_tmp43E=Cyc_Tcutil_compress(t);void*_stmttmp68=_tmp43E;void*_tmp43F=_stmttmp68;struct Cyc_Absyn_Exp*_tmp440;struct Cyc_Absyn_Exp*_tmp441;struct Cyc_List_List*_tmp443;void*_tmp442;void*_tmp444;void*_tmp445;struct Cyc_List_List*_tmp447;enum Cyc_Absyn_AggrKind _tmp446;struct Cyc_List_List*_tmp448;struct Cyc_Absyn_Exp*_tmp453;struct Cyc_Absyn_Exp*_tmp452;struct Cyc_List_List*_tmp451;struct Cyc_List_List*_tmp450;struct Cyc_Absyn_VarargInfo*_tmp44F;int _tmp44E;struct Cyc_List_List*_tmp44D;void*_tmp44C;struct Cyc_Absyn_Tqual _tmp44B;void*_tmp44A;struct Cyc_List_List*_tmp449;void*_tmp459;void*_tmp458;void*_tmp457;void*_tmp456;struct Cyc_Absyn_Tqual _tmp455;void*_tmp454;unsigned _tmp45E;void*_tmp45D;struct Cyc_Absyn_Exp*_tmp45C;struct Cyc_Absyn_Tqual _tmp45B;void*_tmp45A;void*_tmp462;struct Cyc_Absyn_Typedefdecl*_tmp461;struct Cyc_List_List*_tmp460;struct _tuple1*_tmp45F;struct Cyc_Absyn_Tvar*_tmp463;switch(*((int*)_tmp43F)){case 2U: _LL1: _tmp463=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp43F)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp463;
# 2606
struct _handler_cons _tmp464;_push_handler(& _tmp464);{int _tmp466=0;if(setjmp(_tmp464.handler))_tmp466=1;if(!_tmp466){{void*_tmp467=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp467;};_pop_handler();}else{void*_tmp465=(void*)Cyc_Core_get_exn_thrown();void*_tmp468=_tmp465;void*_tmp469;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp468)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp469=_tmp468;_LL1F: {void*exn=_tmp469;(int)_rethrow(exn);}}_LL1B:;}}}case 8U: _LL3: _tmp45F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43F)->f1;_tmp460=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43F)->f2;_tmp461=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43F)->f3;_tmp462=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43F)->f4;_LL4: {struct _tuple1*n=_tmp45F;struct Cyc_List_List*ts=_tmp460;struct Cyc_Absyn_Typedefdecl*td=_tmp461;void*topt=_tmp462;
# 2609
struct Cyc_List_List*_tmp46A=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp46A;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp46B=_cycalloc(sizeof(*_tmp46B));_tmp46B->tag=8U,_tmp46B->f1=n,_tmp46B->f2=new_ts,_tmp46B->f3=td,_tmp46B->f4=topt;_tmp46B;});}case 4U: _LL5: _tmp45A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).elt_type;_tmp45B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).tq;_tmp45C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).num_elts;_tmp45D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).zero_term;_tmp45E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).zt_loc;_LL6: {void*t1=_tmp45A;struct Cyc_Absyn_Tqual tq=_tmp45B;struct Cyc_Absyn_Exp*e=_tmp45C;void*zt=_tmp45D;unsigned ztl=_tmp45E;
# 2612
void*_tmp46C=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp46C;
struct Cyc_Absyn_Exp*_tmp46D=e == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp46D;
void*_tmp46E=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp46E;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp46F=_cycalloc(sizeof(*_tmp46F));
_tmp46F->tag=4U,(_tmp46F->f1).elt_type=new_t1,(_tmp46F->f1).tq=tq,(_tmp46F->f1).num_elts=new_e,(_tmp46F->f1).zero_term=new_zt,(_tmp46F->f1).zt_loc=ztl;_tmp46F;});}case 3U: _LL7: _tmp454=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).elt_type;_tmp455=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).elt_tq;_tmp456=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).rgn;_tmp457=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).nullable;_tmp458=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).bounds;_tmp459=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp454;struct Cyc_Absyn_Tqual tq=_tmp455;void*r=_tmp456;void*n=_tmp457;void*b=_tmp458;void*zt=_tmp459;
# 2618
void*_tmp470=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp470;
void*_tmp471=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp471;
void*_tmp472=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp472;
void*_tmp473=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp473;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp624;_tmp624.elt_type=new_t1,_tmp624.elt_tq=tq,(_tmp624.ptr_atts).rgn=new_r,(_tmp624.ptr_atts).nullable=n,(_tmp624.ptr_atts).bounds=new_b,(_tmp624.ptr_atts).zero_term=new_zt,(_tmp624.ptr_atts).ptrloc=0;_tmp624;}));}case 5U: _LL9: _tmp449=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).tvars;_tmp44A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).effect;_tmp44B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ret_tqual;_tmp44C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ret_type;_tmp44D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).args;_tmp44E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).c_varargs;_tmp44F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).cyc_varargs;_tmp450=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).rgn_po;_tmp451=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).attributes;_tmp452=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).requires_clause;_tmp453=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ensures_clause;_LLA: {struct Cyc_List_List*vs=_tmp449;void*eff=_tmp44A;struct Cyc_Absyn_Tqual rtq=_tmp44B;void*rtyp=_tmp44C;struct Cyc_List_List*args=_tmp44D;int c_varargs=_tmp44E;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp44F;struct Cyc_List_List*rgn_po=_tmp450;struct Cyc_List_List*atts=_tmp451;struct Cyc_Absyn_Exp*req=_tmp452;struct Cyc_Absyn_Exp*ens=_tmp453;
# 2628
{struct Cyc_List_List*_tmp474=vs;struct Cyc_List_List*p=_tmp474;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp476=_region_malloc(rgn,sizeof(*_tmp476));({struct _tuple19*_tmp743=({struct _tuple19*_tmp475=_region_malloc(rgn,sizeof(*_tmp475));_tmp475->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp742=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp475->f2=_tmp742;});_tmp475;});_tmp476->hd=_tmp743;}),_tmp476->tl=inst;_tmp476;});}}{
struct _tuple0 _tmp477=({struct _RegionHandle*_tmp745=rgn;struct _RegionHandle*_tmp744=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp745,_tmp744,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2630
struct _tuple0 _stmttmp69=_tmp477;struct _tuple0 _tmp478=_stmttmp69;struct Cyc_List_List*_tmp47A;struct Cyc_List_List*_tmp479;_LL21: _tmp479=_tmp478.f1;_tmp47A=_tmp478.f2;_LL22: {struct Cyc_List_List*qs=_tmp479;struct Cyc_List_List*ts=_tmp47A;
# 2632
struct Cyc_List_List*_tmp47B=args;struct Cyc_List_List*args2=_tmp47B;
struct Cyc_List_List*_tmp47C=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp47C;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2;
if(eff == 0)
eff2=0;else{
# 2640
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,eff);
if(new_eff == eff)
eff2=eff;else{
# 2644
eff2=new_eff;}}{
# 2646
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2650
struct Cyc_Absyn_VarargInfo _tmp47D=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp6A=_tmp47D;struct Cyc_Absyn_VarargInfo _tmp47E=_stmttmp6A;int _tmp482;void*_tmp481;struct Cyc_Absyn_Tqual _tmp480;struct _fat_ptr*_tmp47F;_LL24: _tmp47F=_tmp47E.name;_tmp480=_tmp47E.tq;_tmp481=_tmp47E.type;_tmp482=_tmp47E.inject;_LL25: {struct _fat_ptr*n=_tmp47F;struct Cyc_Absyn_Tqual tq=_tmp480;void*t=_tmp481;int i=_tmp482;
void*_tmp483=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp483;
if(t2 == t)cyc_varargs2=cyc_varargs;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->name=n,_tmp484->tq=tq,_tmp484->type=t2,_tmp484->inject=i;_tmp484;});}}}{
# 2655
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp485=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp485;
struct Cyc_List_List*_tmp486=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp486;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp487=_cycalloc(sizeof(*_tmp487));_tmp487->tag=5U,(_tmp487->f1).tvars=vs,(_tmp487->f1).effect=eff2,(_tmp487->f1).ret_tqual=rtq,({void*_tmp746=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp487->f1).ret_type=_tmp746;}),(_tmp487->f1).args=args2,(_tmp487->f1).c_varargs=c_varargs,(_tmp487->f1).cyc_varargs=cyc_varargs2,(_tmp487->f1).rgn_po=rgn_po2,(_tmp487->f1).attributes=atts,(_tmp487->f1).requires_clause=req2,(_tmp487->f1).requires_relns=req_relns2,(_tmp487->f1).ensures_clause=ens2,(_tmp487->f1).ensures_relns=ens_relns2;_tmp487;});}}}}}}case 6U: _LLB: _tmp448=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp43F)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp448;
# 2664
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp488=tqts;struct Cyc_List_List*ts1=_tmp488;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp489=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp489;
void*_tmp48A=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp48A;
if(t1 != t2)
change=1;
# 2672
ts2=({struct Cyc_List_List*_tmp48B=_region_malloc(rgn,sizeof(*_tmp48B));_tmp48B->hd=t2,_tmp48B->tl=ts2;_tmp48B;});}}
# 2674
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp48C=_cycalloc(sizeof(*_tmp48C));_tmp48C->tag=6U,({struct Cyc_List_List*_tmp748=({struct Cyc_List_List*_tmp747=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp747,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp48C->f1=_tmp748;});_tmp48C;});}case 7U: _LLD: _tmp446=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp43F)->f1;_tmp447=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp43F)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp446;struct Cyc_List_List*fs=_tmp447;
# 2678
struct Cyc_List_List*_tmp48D=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp48D;
if(fs == new_fs)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp48E=_cycalloc(sizeof(*_tmp48E));_tmp48E->tag=7U,_tmp48E->f1=k,_tmp48E->f2=new_fs;_tmp48E;});}case 1U: _LLF: _tmp445=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp43F)->f2;_LL10: {void*r=_tmp445;
# 2682
if(r != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,r);else{
return t;}}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43F)->f2 == 0){_LL11: _tmp444=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43F)->f1;_LL12: {void*c=_tmp444;
return t;}}else{_LL13: _tmp442=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43F)->f1;_tmp443=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43F)->f2;_LL14: {void*c=_tmp442;struct Cyc_List_List*ts=_tmp443;
# 2686
struct Cyc_List_List*_tmp48F=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp48F;
if(ts == new_ts)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp490=_cycalloc(sizeof(*_tmp490));_tmp490->tag=0U,_tmp490->f1=c,_tmp490->f2=new_ts;_tmp490;});}}}case 9U: _LL15: _tmp441=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43F)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp441;
# 2690
struct Cyc_Absyn_Exp*_tmp491=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp491;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->tag=9U,_tmp492->f1=new_e;_tmp492;});}case 11U: _LL17: _tmp440=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp43F)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp440;
# 2693
struct Cyc_Absyn_Exp*_tmp493=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp493;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->tag=11U,_tmp494->f1=new_e;_tmp494;});}default: _LL19: _LL1A:
({void*_tmp495=0U;({struct _fat_ptr _tmp749=({const char*_tmp496="found typedecltype in rsubs";_tag_fat(_tmp496,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp749,_tag_fat(_tmp495,sizeof(void*),0U));});});}_LL0:;}
# 2699
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2702
if(ts == 0)
return 0;{
void*_tmp497=(void*)ts->hd;void*old_hd=_tmp497;
struct Cyc_List_List*_tmp498=ts->tl;struct Cyc_List_List*old_tl=_tmp498;
void*_tmp499=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp499;
struct Cyc_List_List*_tmp49A=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp49A;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->hd=new_hd,_tmp49B->tl=new_tl;_tmp49B;});}}
# 2713
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 2720
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp49C=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp49C;
return({struct _tuple19*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E->f1=tv,({void*_tmp74B=({struct Cyc_Core_Opt*_tmp74A=k;Cyc_Absyn_new_evar(_tmp74A,({struct Cyc_Core_Opt*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->v=s;_tmp49D;}));});_tmp49E->f2=_tmp74B;});_tmp49E;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2725
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2727
struct _tuple28*_tmp49F=env;struct _RegionHandle*_tmp4A1;struct Cyc_List_List*_tmp4A0;_LL1: _tmp4A0=_tmp49F->f1;_tmp4A1=_tmp49F->f2;_LL2: {struct Cyc_List_List*s=_tmp4A0;struct _RegionHandle*rgn=_tmp4A1;
struct Cyc_Core_Opt*_tmp4A2=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4A2;
return({struct _tuple19*_tmp4A4=_region_malloc(rgn,sizeof(*_tmp4A4));_tmp4A4->f1=tv,({void*_tmp74D=({struct Cyc_Core_Opt*_tmp74C=k;Cyc_Absyn_new_evar(_tmp74C,({struct Cyc_Core_Opt*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->v=s;_tmp4A3;}));});_tmp4A4->f2=_tmp74D;});_tmp4A4;});}}
# 2732
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2734
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp4A7=({struct Cyc_String_pa_PrintArg_struct _tmp625;_tmp625.tag=0U,_tmp625.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp625;});void*_tmp4A5[1U];_tmp4A5[0]=& _tmp4A7;({unsigned _tmp74F=loc;struct _fat_ptr _tmp74E=({const char*_tmp4A6="bitfield %s does not have constant width";_tag_fat(_tmp4A6,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp74F,_tmp74E,_tag_fat(_tmp4A5,sizeof(void*),1U));});});else{
# 2739
struct _tuple13 _tmp4A8=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp6B=_tmp4A8;struct _tuple13 _tmp4A9=_stmttmp6B;int _tmp4AB;unsigned _tmp4AA;_LL1: _tmp4AA=_tmp4A9.f1;_tmp4AB=_tmp4A9.f2;_LL2: {unsigned i=_tmp4AA;int known=_tmp4AB;
if(!known)
({void*_tmp4AC=0U;({unsigned _tmp751=loc;struct _fat_ptr _tmp750=({const char*_tmp4AD="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4AD,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp751,_tmp750,_tag_fat(_tmp4AC,sizeof(void*),0U));});});
if((int)i < 0)
({void*_tmp4AE=0U;({unsigned _tmp753=loc;struct _fat_ptr _tmp752=({const char*_tmp4AF="bitfield has negative width";_tag_fat(_tmp4AF,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp753,_tmp752,_tag_fat(_tmp4AE,sizeof(void*),0U));});});
w=i;}}{
# 2746
void*_tmp4B0=Cyc_Tcutil_compress(field_type);void*_stmttmp6C=_tmp4B0;void*_tmp4B1=_stmttmp6C;enum Cyc_Absyn_Size_of _tmp4B2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->f1)->tag == 1U){_LL4: _tmp4B2=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4B2;
# 2749
{enum Cyc_Absyn_Size_of _tmp4B3=b;switch(_tmp4B3){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp4B4=0U;({unsigned _tmp755=loc;struct _fat_ptr _tmp754=({const char*_tmp4B5="bitfield larger than type";_tag_fat(_tmp4B5,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp755,_tmp754,_tag_fat(_tmp4B4,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp4B6=0U;({unsigned _tmp757=loc;struct _fat_ptr _tmp756=({const char*_tmp4B7="bitfield larger than type";_tag_fat(_tmp4B7,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp757,_tmp756,_tag_fat(_tmp4B6,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp4B8=0U;({unsigned _tmp759=loc;struct _fat_ptr _tmp758=({const char*_tmp4B9="bitfield larger than type";_tag_fat(_tmp4B9,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp759,_tmp758,_tag_fat(_tmp4B8,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp4BA=0U;({unsigned _tmp75B=loc;struct _fat_ptr _tmp75A=({const char*_tmp4BB="bitfield larger than type";_tag_fat(_tmp4BB,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp75B,_tmp75A,_tag_fat(_tmp4BA,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 2757
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2759
({struct Cyc_String_pa_PrintArg_struct _tmp4BE=({struct Cyc_String_pa_PrintArg_struct _tmp627;_tmp627.tag=0U,_tmp627.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp627;});struct Cyc_String_pa_PrintArg_struct _tmp4BF=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({
struct _fat_ptr _tmp75C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field_type));_tmp626.f1=_tmp75C;});_tmp626;});void*_tmp4BC[2U];_tmp4BC[0]=& _tmp4BE,_tmp4BC[1]=& _tmp4BF;({unsigned _tmp75E=loc;struct _fat_ptr _tmp75D=({const char*_tmp4BD="bitfield %s must have integral type but has type %s";_tag_fat(_tmp4BD,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp75E,_tmp75D,_tag_fat(_tmp4BC,sizeof(void*),2U));});});
goto _LL3;}_LL3:;}}}
# 2768
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4C0=t;void*_tmp4C2;struct Cyc_Absyn_Typedefdecl*_tmp4C1;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C0)->tag == 8U){_LL1: _tmp4C1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C0)->f3;_tmp4C2=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C0)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4C1;void*tdopt=_tmp4C2;
# 2771
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({void*_tmp4C3=0U;({unsigned _tmp760=loc;struct _fat_ptr _tmp75F=({const char*_tmp4C4="extra const";_tag_fat(_tmp4C4,sizeof(char),12U);});Cyc_Tcutil_warn(_tmp760,_tmp75F,_tag_fat(_tmp4C3,sizeof(void*),0U));});});
return 1;}
# 2776
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);else{
return declared_const;}}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2783
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp761=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp761;});}
# 2787
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2793
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2796
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4C7=({struct Cyc_String_pa_PrintArg_struct _tmp629;_tmp629.tag=0U,_tmp629.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp629;});struct Cyc_String_pa_PrintArg_struct _tmp4C8=({struct Cyc_String_pa_PrintArg_struct _tmp628;_tmp628.tag=0U,({struct _fat_ptr _tmp762=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp628.f1=_tmp762;});_tmp628;});void*_tmp4C5[2U];_tmp4C5[0]=& _tmp4C7,_tmp4C5[1]=& _tmp4C8;({unsigned _tmp764=loc;struct _fat_ptr _tmp763=({const char*_tmp4C6="%s: %s";_tag_fat(_tmp4C6,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp764,_tmp763,_tag_fat(_tmp4C5,sizeof(void*),2U));});});}}}
# 2802
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2806
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2810
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp766=tvs;unsigned _tmp765=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp766,_tmp765,Cyc_Absynpp_tvar2string,({const char*_tmp4C9="duplicate type variable";_tag_fat(_tmp4C9,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2824 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2829
struct _RegionHandle _tmp4CA=_new_region("temp");struct _RegionHandle*temp=& _tmp4CA;_push_region(temp);
# 2833
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp767=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp767,({const char*_tmp4CB="";_tag_fat(_tmp4CB,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4CD=_region_malloc(temp,sizeof(*_tmp4CD));({struct _tuple29*_tmp768=({struct _tuple29*_tmp4CC=_region_malloc(temp,sizeof(*_tmp4CC));_tmp4CC->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4CC->f2=0;_tmp4CC;});_tmp4CD->hd=_tmp768;}),_tmp4CD->tl=fields;_tmp4CD;});}}
# 2838
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2840
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp4F2="struct";_tag_fat(_tmp4F2,sizeof(char),7U);}):({const char*_tmp4F3="union";_tag_fat(_tmp4F3,sizeof(char),6U);});
# 2843
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4CE=(struct _tuple30*)des->hd;struct _tuple30*_stmttmp6D=_tmp4CE;struct _tuple30*_tmp4CF=_stmttmp6D;void*_tmp4D1;struct Cyc_List_List*_tmp4D0;_LL1: _tmp4D0=_tmp4CF->f1;_tmp4D1=_tmp4CF->f2;_LL2: {struct Cyc_List_List*dl=_tmp4D0;void*a=_tmp4D1;
if(dl == 0){
# 2848
struct Cyc_List_List*_tmp4D2=fields;struct Cyc_List_List*fields2=_tmp4D2;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple29*)fields2->hd)).f2){
(*((struct _tuple29*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp76A=({struct Cyc_List_List*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));({void*_tmp769=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4D3=_cycalloc(sizeof(*_tmp4D3));_tmp4D3->tag=1U,_tmp4D3->f1=((*((struct _tuple29*)fields2->hd)).f1)->name;_tmp4D3;});_tmp4D4->hd=_tmp769;}),_tmp4D4->tl=0;_tmp4D4;});(*((struct _tuple30*)des->hd)).f1=_tmp76A;});
ans=({struct Cyc_List_List*_tmp4D6=_region_malloc(rgn,sizeof(*_tmp4D6));({struct _tuple31*_tmp76B=({struct _tuple31*_tmp4D5=_region_malloc(rgn,sizeof(*_tmp4D5));_tmp4D5->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4D5->f2=a;_tmp4D5;});_tmp4D6->hd=_tmp76B;}),_tmp4D6->tl=ans;_tmp4D6;});
break;}}
# 2856
if(fields2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4D9=({struct Cyc_String_pa_PrintArg_struct _tmp62A;_tmp62A.tag=0U,_tmp62A.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp62A;});void*_tmp4D7[1U];_tmp4D7[0]=& _tmp4D9;({unsigned _tmp76D=loc;struct _fat_ptr _tmp76C=({const char*_tmp4D8="too many arguments to %s";_tag_fat(_tmp4D8,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp76D,_tmp76C,_tag_fat(_tmp4D7,sizeof(void*),1U));});});}else{
if(dl->tl != 0)
# 2860
({void*_tmp4DA=0U;({unsigned _tmp76F=loc;struct _fat_ptr _tmp76E=({const char*_tmp4DB="multiple designators are not yet supported";_tag_fat(_tmp4DB,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp76F,_tmp76E,_tag_fat(_tmp4DA,sizeof(void*),0U));});});else{
# 2863
void*_tmp4DC=(void*)dl->hd;void*_stmttmp6E=_tmp4DC;void*_tmp4DD=_stmttmp6E;struct _fat_ptr*_tmp4DE;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4DD)->tag == 0U){_LL4: _LL5:
# 2865
({struct Cyc_String_pa_PrintArg_struct _tmp4E1=({struct Cyc_String_pa_PrintArg_struct _tmp62B;_tmp62B.tag=0U,_tmp62B.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp62B;});void*_tmp4DF[1U];_tmp4DF[0]=& _tmp4E1;({unsigned _tmp771=loc;struct _fat_ptr _tmp770=({const char*_tmp4E0="array designator used in argument to %s";_tag_fat(_tmp4E0,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp771,_tmp770,_tag_fat(_tmp4DF,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp4DE=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4DD)->f1;_LL7: {struct _fat_ptr*v=_tmp4DE;
# 2868
struct Cyc_List_List*_tmp4E2=fields;struct Cyc_List_List*fields2=_tmp4E2;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple29*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple29*)fields2->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp4E5=({struct Cyc_String_pa_PrintArg_struct _tmp62C;_tmp62C.tag=0U,_tmp62C.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp62C;});void*_tmp4E3[1U];_tmp4E3[0]=& _tmp4E5;({unsigned _tmp773=loc;struct _fat_ptr _tmp772=({const char*_tmp4E4="member %s has already been used as an argument";_tag_fat(_tmp4E4,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp773,_tmp772,_tag_fat(_tmp4E3,sizeof(void*),1U));});});
(*((struct _tuple29*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4E7=_region_malloc(rgn,sizeof(*_tmp4E7));({struct _tuple31*_tmp774=({struct _tuple31*_tmp4E6=_region_malloc(rgn,sizeof(*_tmp4E6));_tmp4E6->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4E6->f2=a;_tmp4E6;});_tmp4E7->hd=_tmp774;}),_tmp4E7->tl=ans;_tmp4E7;});
break;}}
# 2877
if(fields2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4EA=({struct Cyc_String_pa_PrintArg_struct _tmp62D;_tmp62D.tag=0U,_tmp62D.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp62D;});void*_tmp4E8[1U];_tmp4E8[0]=& _tmp4EA;({unsigned _tmp776=loc;struct _fat_ptr _tmp775=({const char*_tmp4E9="bad field designator %s";_tag_fat(_tmp4E9,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp776,_tmp775,_tag_fat(_tmp4E8,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}}
# 2882
if((int)aggr_kind == (int)0U)
# 2884
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple29*)fields->hd)).f2){
({void*_tmp4EB=0U;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp4EC="too few arguments to struct";_tag_fat(_tmp4EC,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp778,_tmp777,_tag_fat(_tmp4EB,sizeof(void*),0U));});});
break;}}else{
# 2891
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple29*)fields->hd)).f2){
if(found)({void*_tmp4ED=0U;({unsigned _tmp77A=loc;struct _fat_ptr _tmp779=({const char*_tmp4EE="only one member of a union is allowed";_tag_fat(_tmp4EE,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_fat(_tmp4ED,sizeof(void*),0U));});});
found=1;}}
# 2897
if(!found)({void*_tmp4EF=0U;({unsigned _tmp77C=loc;struct _fat_ptr _tmp77B=({const char*_tmp4F0="missing member for union";_tag_fat(_tmp4F0,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp77C,_tmp77B,_tag_fat(_tmp4EF,sizeof(void*),0U));});});}{
# 2900
struct Cyc_List_List*_tmp4F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp4F1;}}}
# 2833
;_pop_region(temp);}
# 2906
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2908
void*_tmp4F4=e1->r;void*_stmttmp6F=_tmp4F4;void*_tmp4F5=_stmttmp6F;struct Cyc_Absyn_Exp*_tmp4F6;struct Cyc_Absyn_Exp*_tmp4F7;struct Cyc_Absyn_Exp*_tmp4F8;struct Cyc_Absyn_Exp*_tmp4F9;struct Cyc_Absyn_Exp*_tmp4FA;struct Cyc_Absyn_Exp*_tmp4FB;switch(*((int*)_tmp4F5)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp4FE=({struct Cyc_String_pa_PrintArg_struct _tmp62E;_tmp62E.tag=0U,({struct _fat_ptr _tmp77D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp62E.f1=_tmp77D;});_tmp62E;});void*_tmp4FC[1U];_tmp4FC[0]=& _tmp4FE;({struct _fat_ptr _tmp77E=({const char*_tmp4FD="we have a cast in a lhs:  %s";_tag_fat(_tmp4FD,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp77E,_tag_fat(_tmp4FC,sizeof(void*),1U));});});case 20U: _LL3: _tmp4FB=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp4FB;
_tmp4FA=e1a;goto _LL6;}case 23U: _LL5: _tmp4FA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp4FA;
# 2912
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp4F9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp4F9;
_tmp4F8=e1a;goto _LLA;}case 21U: _LL9: _tmp4F8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp4F8;
# 2916
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp501=({struct Cyc_String_pa_PrintArg_struct _tmp62F;_tmp62F.tag=0U,({
struct _fat_ptr _tmp77F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp62F.f1=_tmp77F;});_tmp62F;});void*_tmp4FF[1U];_tmp4FF[0]=& _tmp501;({struct _fat_ptr _tmp780=({const char*_tmp500="found zero pointer aggregate member assignment: %s";_tag_fat(_tmp500,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp780,_tag_fat(_tmp4FF,sizeof(void*),1U));});});
return 0;}case 13U: _LLB: _tmp4F7=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp4F7;
_tmp4F6=e1a;goto _LLE;}case 12U: _LLD: _tmp4F6=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp4F6;
# 2922
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp504=({struct Cyc_String_pa_PrintArg_struct _tmp630;_tmp630.tag=0U,({
struct _fat_ptr _tmp781=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp630.f1=_tmp781;});_tmp630;});void*_tmp502[1U];_tmp502[0]=& _tmp504;({struct _fat_ptr _tmp782=({const char*_tmp503="found zero pointer instantiate/noinstantiate: %s";_tag_fat(_tmp503,sizeof(char),49U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp782,_tag_fat(_tmp502,sizeof(void*),1U));});});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp507=({struct Cyc_String_pa_PrintArg_struct _tmp631;_tmp631.tag=0U,({struct _fat_ptr _tmp783=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp631.f1=_tmp783;});_tmp631;});void*_tmp505[1U];_tmp505[0]=& _tmp507;({struct _fat_ptr _tmp784=({const char*_tmp506="found bad lhs in is_zero_ptr_deref: %s";_tag_fat(_tmp506,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp784,_tag_fat(_tmp505,sizeof(void*),1U));});});}_LL0:;}
# 2937
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 2940
void*_tmp508=Cyc_Tcutil_compress(r);void*_stmttmp70=_tmp508;void*_tmp509=_stmttmp70;struct Cyc_Absyn_Tvar*_tmp50A;enum Cyc_Absyn_AliasQual _tmp50C;enum Cyc_Absyn_KindQual _tmp50B;switch(*((int*)_tmp509)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp509)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f4 == 0){_LL5: _tmp50B=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f3)->kind)->v)->kind;_tmp50C=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp50B;enum Cyc_Absyn_AliasQual a=_tmp50C;
# 2944
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp50A=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp509)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp50A;
# 2948
struct Cyc_Absyn_Kind*_tmp50D=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp71=_tmp50D;struct Cyc_Absyn_Kind*_tmp50E=_stmttmp71;enum Cyc_Absyn_AliasQual _tmp510;enum Cyc_Absyn_KindQual _tmp50F;_LLC: _tmp50F=_tmp50E->kind;_tmp510=_tmp50E->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp50F;enum Cyc_Absyn_AliasQual a=_tmp510;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp511=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp72=_tmp511;void*_tmp512=_stmttmp72;struct Cyc_Core_Opt**_tmp513;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp512)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp512)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp512)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp513=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp512)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp513;
# 2952
({struct Cyc_Core_Opt*_tmp786=({struct Cyc_Core_Opt*_tmp515=_cycalloc(sizeof(*_tmp515));({void*_tmp785=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp514=_cycalloc(sizeof(*_tmp514));_tmp514->tag=2U,_tmp514->f1=0,_tmp514->f2=& Cyc_Tcutil_rk;_tmp514;});_tmp515->v=_tmp785;});_tmp515;});*x=_tmp786;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2957
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2964
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp516=Cyc_Tcutil_compress(t);void*_stmttmp73=_tmp516;void*_tmp517=_stmttmp73;struct Cyc_Absyn_Tvar*_tmp518;void*_tmp519;switch(*((int*)_tmp517)){case 3U: _LL1: _tmp519=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp517)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp519;
# 2967
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp518=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp517)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp518;
# 2969
struct Cyc_Absyn_Kind*_tmp51A=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp74=_tmp51A;struct Cyc_Absyn_Kind*_tmp51B=_stmttmp74;enum Cyc_Absyn_AliasQual _tmp51D;enum Cyc_Absyn_KindQual _tmp51C;_LL8: _tmp51C=_tmp51B->kind;_tmp51D=_tmp51B->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp51C;enum Cyc_Absyn_AliasQual a=_tmp51D;
enum Cyc_Absyn_KindQual _tmp51E=k;switch(_tmp51E){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp51F=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp75=_tmp51F;void*_tmp520=_stmttmp75;enum Cyc_Absyn_KindQual _tmp522;struct Cyc_Core_Opt**_tmp521;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp520)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp520)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp521=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp520)->f1;_tmp522=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp520)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp521;enum Cyc_Absyn_KindQual k=_tmp522;
# 2975
({struct Cyc_Core_Opt*_tmp789=({struct Cyc_Core_Opt*_tmp525=_cycalloc(sizeof(*_tmp525));({void*_tmp788=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->tag=2U,_tmp524->f1=0,({struct Cyc_Absyn_Kind*_tmp787=({struct Cyc_Absyn_Kind*_tmp523=_cycalloc(sizeof(*_tmp523));_tmp523->kind=k,_tmp523->aliasqual=Cyc_Absyn_Aliasable;_tmp523;});_tmp524->f2=_tmp787;});_tmp524;});_tmp525->v=_tmp788;});_tmp525;});*x=_tmp789;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
# 2979
 return 1;}_LL13:;}
# 2982
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2985
 return 0;}_LL0:;}
# 2988
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp526=Cyc_Tcutil_compress(t);void*t=_tmp526;
if(Cyc_Tcutil_is_noalias_pointer(t,0))return 1;{
void*_tmp527=t;struct Cyc_List_List*_tmp528;struct Cyc_List_List*_tmp52A;union Cyc_Absyn_DatatypeFieldInfo _tmp529;struct Cyc_List_List*_tmp52C;union Cyc_Absyn_DatatypeInfo _tmp52B;struct Cyc_List_List*_tmp52E;struct Cyc_Absyn_Aggrdecl**_tmp52D;struct Cyc_List_List*_tmp52F;switch(*((int*)_tmp527)){case 6U: _LL1: _tmp52F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp527)->f1;_LL2: {struct Cyc_List_List*qts=_tmp52F;
# 2993
while(qts != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))return 1;
qts=qts->tl;}
# 2997
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp52D=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)->f1).KnownAggr).val;_tmp52E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp52D;struct Cyc_List_List*ts=_tmp52E;
# 2999
if((*adp)->impl == 0)return 0;else{
# 3001
struct Cyc_List_List*_tmp530=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp530;
struct Cyc_List_List*_tmp531=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp531;
void*t;
while(x != 0){
t=inst == 0?((struct Cyc_Absyn_Aggrfield*)x->hd)->type: Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
x=x->tl;}
# 3009
return 0;}}}else{_LL7: _LL8:
# 3019
 return 0;}case 18U: _LL9: _tmp52B=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)->f1;_tmp52C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f2;_LLA: {union Cyc_Absyn_DatatypeInfo tinfo=_tmp52B;struct Cyc_List_List*ts=_tmp52C;
# 3021
union Cyc_Absyn_DatatypeInfo _tmp532=tinfo;struct Cyc_Core_Opt*_tmp534;struct Cyc_List_List*_tmp533;int _tmp536;struct _tuple1*_tmp535;if((_tmp532.UnknownDatatype).tag == 1){_LL10: _tmp535=((_tmp532.UnknownDatatype).val).name;_tmp536=((_tmp532.UnknownDatatype).val).is_extensible;_LL11: {struct _tuple1*nm=_tmp535;int isxt=_tmp536;
# 3024
return 0;}}else{_LL12: _tmp533=(*(_tmp532.KnownDatatype).val)->tvs;_tmp534=(*(_tmp532.KnownDatatype).val)->fields;_LL13: {struct Cyc_List_List*tvs=_tmp533;struct Cyc_Core_Opt*flds=_tmp534;
# 3027
return 0;}}_LLF:;}case 19U: _LLB: _tmp529=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)->f1;_tmp52A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp529;struct Cyc_List_List*ts=_tmp52A;
# 3030
union Cyc_Absyn_DatatypeFieldInfo _tmp537=tinfo;struct Cyc_Absyn_Datatypefield*_tmp539;struct Cyc_Absyn_Datatypedecl*_tmp538;if((_tmp537.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 3033
 return 0;}else{_LL17: _tmp538=((_tmp537.KnownDatatypefield).val).f1;_tmp539=((_tmp537.KnownDatatypefield).val).f2;_LL18: {struct Cyc_Absyn_Datatypedecl*td=_tmp538;struct Cyc_Absyn_Datatypefield*fld=_tmp539;
# 3035
struct Cyc_List_List*_tmp53A=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp53A;
struct Cyc_List_List*_tmp53B=fld->typs;struct Cyc_List_List*typs=_tmp53B;
while(typs != 0){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
typs=typs->tl;}
# 3042
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp528=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp527)->f2;_LL6: {struct Cyc_List_List*x=_tmp528;
# 3012
while(x != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))return 1;
x=x->tl;}
# 3016
return 0;}default: _LLD: _LLE:
# 3044
 return 0;}_LL0:;}}
# 3051
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp53C=e->r;void*_stmttmp76=_tmp53C;void*_tmp53D=_stmttmp76;struct Cyc_Absyn_Stmt*_tmp53E;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Exp*_tmp540;struct Cyc_Absyn_Exp*_tmp542;struct Cyc_Absyn_Exp*_tmp541;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_Absyn_Exp*_tmp543;struct _fat_ptr*_tmp546;struct Cyc_Absyn_Exp*_tmp545;struct Cyc_Absyn_Exp*_tmp547;struct Cyc_Absyn_Exp*_tmp548;switch(*((int*)_tmp53D)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53D)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp548=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp548;
_tmp547=e1;goto _LL6;}case 20U: _LL5: _tmp547=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp547;
# 3056
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp545=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_tmp546=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp545;struct _fat_ptr*f=_tmp546;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp543=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_tmp544=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp543;struct Cyc_Absyn_Exp*e2=_tmp544;
# 3059
void*_tmp549=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp77=_tmp549;void*_tmp54A=_stmttmp77;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp54A)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp541=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_tmp542=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53D)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp541;struct Cyc_Absyn_Exp*e2=_tmp542;
# 3064
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp540=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp540;
_tmp53F=e2;goto _LL10;}case 14U: _LLF: _tmp53F=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp53F;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp53E=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp53E;
# 3068
while(1){
void*_tmp54B=s->r;void*_stmttmp78=_tmp54B;void*_tmp54C=_stmttmp78;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Stmt*_tmp54F;struct Cyc_Absyn_Decl*_tmp54E;struct Cyc_Absyn_Stmt*_tmp551;struct Cyc_Absyn_Stmt*_tmp550;switch(*((int*)_tmp54C)){case 2U: _LL1B: _tmp550=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp551=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp550;struct Cyc_Absyn_Stmt*s2=_tmp551;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp54E=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_tmp54F=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp54C)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp54E;struct Cyc_Absyn_Stmt*s1=_tmp54F;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp54D=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp54C)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp54D;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({void*_tmp552=0U;({struct _fat_ptr _tmp78A=({const char*_tmp553="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp553,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78A,_tag_fat(_tmp552,sizeof(void*),0U));});});}_LL1A:;}}default: _LL13: _LL14:
# 3076
 return 1;}_LL0:;}
# 3093 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 3095
struct _tuple14 bogus_ans=({struct _tuple14 _tmp63E;_tmp63E.f1=0,_tmp63E.f2=Cyc_Absyn_heap_rgn_type;_tmp63E;});
void*_tmp554=e->r;void*_stmttmp79=_tmp554;void*_tmp555=_stmttmp79;struct Cyc_Absyn_Exp*_tmp557;struct Cyc_Absyn_Exp*_tmp556;struct Cyc_Absyn_Exp*_tmp558;int _tmp55B;struct _fat_ptr*_tmp55A;struct Cyc_Absyn_Exp*_tmp559;int _tmp55E;struct _fat_ptr*_tmp55D;struct Cyc_Absyn_Exp*_tmp55C;void*_tmp55F;switch(*((int*)_tmp555)){case 1U: _LL1: _tmp55F=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_LL2: {void*x=_tmp55F;
# 3099
void*_tmp560=x;struct Cyc_Absyn_Vardecl*_tmp561;struct Cyc_Absyn_Vardecl*_tmp562;struct Cyc_Absyn_Vardecl*_tmp563;struct Cyc_Absyn_Vardecl*_tmp564;switch(*((int*)_tmp560)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp564=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp560)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp564;
# 3103
void*_tmp565=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7A=_tmp565;void*_tmp566=_stmttmp7A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp566)->tag == 4U){_LL1B: _LL1C:
# 3105
 return({struct _tuple14 _tmp632;_tmp632.f1=1,_tmp632.f2=Cyc_Absyn_heap_rgn_type;_tmp632;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp633;_tmp633.f1=(vd->tq).real_const,_tmp633.f2=Cyc_Absyn_heap_rgn_type;_tmp633;});}_LL1A:;}case 4U: _LL14: _tmp563=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp560)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp563;
# 3109
void*_tmp567=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7B=_tmp567;void*_tmp568=_stmttmp7B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp568)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp634;_tmp634.f1=1,_tmp634.f2=(void*)_check_null(vd->rgn);_tmp634;});}else{_LL22: _LL23:
# 3112
 vd->escapes=1;
return({struct _tuple14 _tmp635;_tmp635.f1=(vd->tq).real_const,_tmp635.f2=(void*)_check_null(vd->rgn);_tmp635;});}_LL1F:;}case 5U: _LL16: _tmp562=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp560)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp562;
# 3115
_tmp561=vd;goto _LL19;}default: _LL18: _tmp561=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp560)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp561;
# 3117
vd->escapes=1;
return({struct _tuple14 _tmp636;_tmp636.f1=(vd->tq).real_const,_tmp636.f2=(void*)_check_null(vd->rgn);_tmp636;});}}_LLD:;}case 21U: _LL3: _tmp55C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_tmp55D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp555)->f2;_tmp55E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp555)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp55C;struct _fat_ptr*f=_tmp55D;int is_tagged=_tmp55E;
# 3122
if(is_tagged)return bogus_ans;{
# 3125
void*_tmp569=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7C=_tmp569;void*_tmp56A=_stmttmp7C;struct Cyc_Absyn_Aggrdecl*_tmp56B;struct Cyc_List_List*_tmp56C;switch(*((int*)_tmp56A)){case 7U: _LL25: _tmp56C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp56A)->f2;_LL26: {struct Cyc_List_List*fs=_tmp56C;
# 3127
struct Cyc_Absyn_Aggrfield*_tmp56D=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp56D;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp56E=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7D=_tmp56E;struct _tuple14 _tmp56F=_stmttmp7D;void*_tmp571;int _tmp570;_LL2C: _tmp570=_tmp56F.f1;_tmp571=_tmp56F.f2;_LL2D: {int c=_tmp570;void*t=_tmp571;
return({struct _tuple14 _tmp637;_tmp637.f1=(finfo->tq).real_const || c,_tmp637.f2=t;_tmp637;});}}
# 3132
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56A)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56A)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp56B=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp56A)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp56B;
# 3134
struct Cyc_Absyn_Aggrfield*_tmp572=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp572;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp573=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7E=_tmp573;struct _tuple14 _tmp574=_stmttmp7E;void*_tmp576;int _tmp575;_LL2F: _tmp575=_tmp574.f1;_tmp576=_tmp574.f2;_LL30: {int c=_tmp575;void*t=_tmp576;
return({struct _tuple14 _tmp638;_tmp638.f1=(finfo->tq).real_const || c,_tmp638.f2=t;_tmp638;});}}
# 3139
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp559=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_tmp55A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp555)->f2;_tmp55B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp555)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp559;struct _fat_ptr*f=_tmp55A;int is_tagged=_tmp55B;
# 3144
if(is_tagged)return bogus_ans;{
# 3147
void*_tmp577=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7F=_tmp577;void*_tmp578=_stmttmp7F;void*_tmp57A;void*_tmp579;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp578)->tag == 3U){_LL32: _tmp579=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp578)->f1).elt_type;_tmp57A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp578)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp579;void*r=_tmp57A;
# 3149
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp57B=Cyc_Tcutil_compress(t1);void*_stmttmp80=_tmp57B;void*_tmp57C=_stmttmp80;struct Cyc_Absyn_Aggrdecl*_tmp57D;struct Cyc_List_List*_tmp57E;switch(*((int*)_tmp57C)){case 7U: _LL37: _tmp57E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp57C)->f2;_LL38: {struct Cyc_List_List*fs=_tmp57E;
# 3152
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57C)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57C)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp57D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp57C)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp57D;
# 3154
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3157
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp639;_tmp639.f1=(finfo->tq).real_const,_tmp639.f2=r;_tmp639;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp558=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp558;
# 3164
void*_tmp57F=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp81=_tmp57F;void*_tmp580=_stmttmp81;void*_tmp582;struct Cyc_Absyn_Tqual _tmp581;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp580)->tag == 3U){_LL3E: _tmp581=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp580)->f1).elt_tq;_tmp582=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp580)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp581;void*r=_tmp582;
# 3166
return({struct _tuple14 _tmp63A;_tmp63A.f1=tq.real_const,_tmp63A.f2=r;_tmp63A;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp556=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_tmp557=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp555)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp556;struct Cyc_Absyn_Exp*e2=_tmp557;
# 3172
void*t=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*_tmp583=t;struct Cyc_Absyn_Tqual _tmp584;void*_tmp586;struct Cyc_Absyn_Tqual _tmp585;struct Cyc_List_List*_tmp587;switch(*((int*)_tmp583)){case 6U: _LL43: _tmp587=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp583)->f1;_LL44: {struct Cyc_List_List*ts=_tmp587;
# 3176
struct _tuple13 _tmp588=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp82=_tmp588;struct _tuple13 _tmp589=_stmttmp82;int _tmp58B;unsigned _tmp58A;_LL4C: _tmp58A=_tmp589.f1;_tmp58B=_tmp589.f2;_LL4D: {unsigned i=_tmp58A;int known=_tmp58B;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp58C=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp58C;
if(finfo != 0)
return({struct _tuple14 _tmp63B;_tmp63B.f1=((*finfo).f1).real_const,({void*_tmp78B=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp63B.f2=_tmp78B;});_tmp63B;});
return bogus_ans;}}}case 3U: _LL45: _tmp585=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->f1).elt_tq;_tmp586=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp583)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp585;void*r=_tmp586;
# 3184
return({struct _tuple14 _tmp63C;_tmp63C.f1=tq.real_const,_tmp63C.f2=r;_tmp63C;});}case 4U: _LL47: _tmp584=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp583)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp584;
# 3190
return({struct _tuple14 _tmp63D;_tmp63D.f1=tq.real_const,({void*_tmp78C=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp63D.f2=_tmp78C;});_tmp63D;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3194
({void*_tmp58D=0U;({unsigned _tmp78E=e->loc;struct _fat_ptr _tmp78D=({const char*_tmp58E="unary & applied to non-lvalue";_tag_fat(_tmp58E,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp78E,_tmp78D,_tag_fat(_tmp58D,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3201
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp58F=({void*_tmp78F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp78F,b);});struct Cyc_Absyn_Exp*eopt=_tmp58F;
if(eopt == 0)return;{
struct Cyc_Absyn_Exp*_tmp590=eopt;struct Cyc_Absyn_Exp*e=_tmp590;
struct _tuple13 _tmp591=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple13 _stmttmp83=_tmp591;struct _tuple13 _tmp592=_stmttmp83;int _tmp594;unsigned _tmp593;_LL1: _tmp593=_tmp592.f1;_tmp594=_tmp592.f2;_LL2: {unsigned j=_tmp593;int known=_tmp594;
if(known && j <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp597=({struct Cyc_Int_pa_PrintArg_struct _tmp640;_tmp640.tag=1U,_tmp640.f1=(unsigned long)((int)j);_tmp640;});struct Cyc_Int_pa_PrintArg_struct _tmp598=({struct Cyc_Int_pa_PrintArg_struct _tmp63F;_tmp63F.tag=1U,_tmp63F.f1=(unsigned long)((int)i);_tmp63F;});void*_tmp595[2U];_tmp595[0]=& _tmp597,_tmp595[1]=& _tmp598;({unsigned _tmp791=loc;struct _fat_ptr _tmp790=({const char*_tmp596="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp596,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp791,_tmp790,_tag_fat(_tmp595,sizeof(void*),2U));});});else{
# 3210
({struct Cyc_Int_pa_PrintArg_struct _tmp59B=({struct Cyc_Int_pa_PrintArg_struct _tmp642;_tmp642.tag=1U,_tmp642.f1=(unsigned long)((int)j);_tmp642;});struct Cyc_Int_pa_PrintArg_struct _tmp59C=({struct Cyc_Int_pa_PrintArg_struct _tmp641;_tmp641.tag=1U,_tmp641.f1=(unsigned long)((int)i);_tmp641;});void*_tmp599[2U];_tmp599[0]=& _tmp59B,_tmp599[1]=& _tmp59C;({unsigned _tmp793=loc;struct _fat_ptr _tmp792=({const char*_tmp59A="dereference is out of bounds: %d <= %d";_tag_fat(_tmp59A,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp793,_tmp792,_tag_fat(_tmp599,sizeof(void*),2U));});});}}
return;}}}
# 3214
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3223
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp59D=e->r;void*_stmttmp84=_tmp59D;void*_tmp59E=_stmttmp84;struct Cyc_List_List*_tmp59F;struct Cyc_List_List*_tmp5A0;struct Cyc_List_List*_tmp5A2;enum Cyc_Absyn_Primop _tmp5A1;struct Cyc_List_List*_tmp5A3;struct Cyc_List_List*_tmp5A4;struct Cyc_List_List*_tmp5A5;struct Cyc_Absyn_Exp*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A7;struct Cyc_Absyn_Exp*_tmp5A9;struct Cyc_Absyn_Exp*_tmp5AB;void*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5AD;void*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B1;struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B3;void*_tmp5B6;switch(*((int*)_tmp59E)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5B6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL10: {void*b=_tmp5B6;
# 3235
void*_tmp5B7=b;struct Cyc_Absyn_Vardecl*_tmp5B8;struct Cyc_Absyn_Vardecl*_tmp5B9;switch(*((int*)_tmp5B7)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp5B9=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5B7)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5B9;
# 3238
void*_tmp5BA=Cyc_Tcutil_compress(vd->type);void*_stmttmp85=_tmp5BA;void*_tmp5BB=_stmttmp85;switch(*((int*)_tmp5BB)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp5B8=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5B7)->f1;_LL39: {struct Cyc_Absyn_Vardecl*vd=_tmp5B8;
# 3245
if((int)vd->sc == (int)Cyc_Absyn_Static){
void*_tmp5BC=Cyc_Tcutil_compress(vd->type);void*_stmttmp86=_tmp5BC;void*_tmp5BD=_stmttmp86;switch(*((int*)_tmp5BD)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3252
return var_okay;}}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp5B3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5B4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_tmp5B5=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59E)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5B3;struct Cyc_Absyn_Exp*e2=_tmp5B4;struct Cyc_Absyn_Exp*e3=_tmp5B5;
# 3258
return(Cyc_Tcutil_cnst_exp(0,e1)&&
 Cyc_Tcutil_cnst_exp(0,e2))&&
 Cyc_Tcutil_cnst_exp(0,e3);}case 9U: _LL13: _tmp5B1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5B2=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5B1;struct Cyc_Absyn_Exp*e2=_tmp5B2;
# 3262
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 41U: _LL15: _tmp5B0=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5B0;
_tmp5AF=e2;goto _LL18;}case 12U: _LL17: _tmp5AF=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5AF;
_tmp5AE=e2;goto _LL1A;}case 13U: _LL19: _tmp5AE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5AE;
# 3266
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp5AC=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5AD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL1C: {void*t=_tmp5AC;struct Cyc_Absyn_Exp*e2=_tmp5AD;
# 3268
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}else{_LL1D: _tmp5AA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5AB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL1E: {void*t=_tmp5AA;struct Cyc_Absyn_Exp*e2=_tmp5AB;
# 3271
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}case 15U: _LL1F: _tmp5A9=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp5A9;
# 3273
return Cyc_Tcutil_cnst_exp(1,e2);}case 27U: _LL21: _tmp5A7=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_tmp5A8=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59E)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp5A7;struct Cyc_Absyn_Exp*e2=_tmp5A8;
# 3275
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 28U: _LL23: _tmp5A6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp5A6;
# 3277
return Cyc_Tcutil_cnst_exp(0,e);}case 26U: _LL25: _tmp5A5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL26: {struct Cyc_List_List*des=_tmp5A5;
_tmp5A4=des;goto _LL28;}case 30U: _LL27: _tmp5A4=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL28: {struct Cyc_List_List*des=_tmp5A4;
_tmp5A3=des;goto _LL2A;}case 29U: _LL29: _tmp5A3=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp59E)->f3;_LL2A: {struct Cyc_List_List*des=_tmp5A3;
# 3281
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL2B: _tmp5A1=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5A2=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL2C: {enum Cyc_Absyn_Primop p=_tmp5A1;struct Cyc_List_List*es=_tmp5A2;
# 3286
_tmp5A0=es;goto _LL2E;}case 24U: _LL2D: _tmp5A0=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL2E: {struct Cyc_List_List*es=_tmp5A0;
_tmp59F=es;goto _LL30;}case 31U: _LL2F: _tmp59F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL30: {struct Cyc_List_List*es=_tmp59F;
# 3289
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3296
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3300
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5BE=Cyc_Tcutil_compress(t);void*_stmttmp87=_tmp5BE;void*_tmp5BF=_stmttmp87;struct Cyc_List_List*_tmp5C0;struct Cyc_List_List*_tmp5C1;void*_tmp5C2;void*_tmp5C3;struct Cyc_List_List*_tmp5C5;void*_tmp5C4;switch(*((int*)_tmp5BF)){case 0U: _LL1: _tmp5C4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5BF)->f1;_tmp5C5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5BF)->f2;_LL2: {void*c=_tmp5C4;struct Cyc_List_List*ts=_tmp5C5;
# 3304
void*_tmp5C6=c;union Cyc_Absyn_AggrInfo _tmp5C7;struct Cyc_List_List*_tmp5C8;struct Cyc_Absyn_Enumdecl*_tmp5C9;switch(*((int*)_tmp5C6)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5C9=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5C6)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5C9;
# 3309
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5C8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5C8=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5C6)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5C8;
# 3313
{struct Cyc_List_List*_tmp5CA=fs;struct Cyc_List_List*fs2=_tmp5CA;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5CB=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp88=_tmp5CB;struct _tuple13 _tmp5CC=_stmttmp88;int _tmp5CE;unsigned _tmp5CD;_LL1D: _tmp5CD=_tmp5CC.f1;_tmp5CE=_tmp5CC.f2;_LL1E: {unsigned i=_tmp5CD;int known=_tmp5CE;
if(known && i == (unsigned)0)
return 1;}}}}
# 3320
return 0;}case 20U: _LL18: _tmp5C7=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5C6)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5C7;
# 3323
struct Cyc_Absyn_Aggrdecl*_tmp5CF=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5CF;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5C3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5C3;
# 3331
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5BF)->f1).elt_type;_LL6: {void*t=_tmp5C2;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5C1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5BF)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5C1;
# 3334
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))return 0;}
return 1;}case 7U: _LL9: _tmp5C0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5BF)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5C0;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3341
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3343
struct _RegionHandle _tmp5D0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D0;_push_region(rgn);
{struct Cyc_List_List*_tmp5D1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5D1;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5D2=0;_npop_handler(0U);return _tmp5D2;}}{
# 3351
int _tmp5D3=1;_npop_handler(0U);return _tmp5D3;}}
# 3344
;_pop_region(rgn);}
# 3355
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5D4=t;struct Cyc_Absyn_Typedefdecl*_tmp5D5;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5D4)->tag == 8U){_LL1: _tmp5D5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5D4)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5D5;
# 3358
if(tdopt != 0){
struct Cyc_Absyn_Tqual _tmp5D6=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5D6;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp5D9=({struct Cyc_String_pa_PrintArg_struct _tmp643;_tmp643.tag=0U,({struct _fat_ptr _tmp794=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp643.f1=_tmp794;});_tmp643;});void*_tmp5D7[1U];_tmp5D7[0]=& _tmp5D9;({unsigned _tmp796=loc;struct _fat_ptr _tmp795=({const char*_tmp5D8="qualifier within typedef type %s is ignored";_tag_fat(_tmp5D8,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp796,_tmp795,_tag_fat(_tmp5D7,sizeof(void*),1U));});});}
# 3363
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3370
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp5DA=Cyc_Tcutil_compress(t);void*_stmttmp89=_tmp5DA;void*_tmp5DB=_stmttmp89;struct Cyc_List_List**_tmp5DC;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5DB)->tag == 5U){_LL1: _tmp5DC=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5DB)->f1).attributes;_LL2: {struct Cyc_List_List**fnatts=_tmp5DC;
# 3373
struct Cyc_List_List*_tmp5DD=0;struct Cyc_List_List*res_atts=_tmp5DD;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_tmp797=({struct Cyc_List_List*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->hd=(void*)atts->hd,_tmp5DE->tl=*fnatts;_tmp5DE;});*fnatts=_tmp797;});}else{
# 3380
res_atts=({struct Cyc_List_List*_tmp5DF=_cycalloc(sizeof(*_tmp5DF));_tmp5DF->hd=(void*)atts->hd,_tmp5DF->tl=res_atts;_tmp5DF;});}}
return res_atts;}}else{_LL3: _LL4:
({void*_tmp5E0=0U;({struct _fat_ptr _tmp798=({const char*_tmp5E1="transfer_fn_type_atts";_tag_fat(_tmp5E1,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp798,_tag_fat(_tmp5E0,sizeof(void*),0U));});});}_LL0:;}
# 3387
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5E2=Cyc_Tcutil_compress(t);void*_stmttmp8A=_tmp5E2;void*_tmp5E3=_stmttmp8A;struct Cyc_Absyn_Exp*_tmp5E4;struct Cyc_Absyn_PtrInfo _tmp5E5;switch(*((int*)_tmp5E3)){case 3U: _LL1: _tmp5E5=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E3)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp5E5;
return({void*_tmp799=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp799,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp5E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5E4;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3397
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5E6=b;struct Cyc_Absyn_Vardecl*_tmp5E7;struct Cyc_Absyn_Vardecl*_tmp5E8;struct Cyc_Absyn_Vardecl*_tmp5E9;struct Cyc_Absyn_Vardecl*_tmp5EA;switch(*((int*)_tmp5E6)){case 5U: _LL1: _tmp5EA=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp5EA;
_tmp5E9=x;goto _LL4;}case 4U: _LL3: _tmp5E9=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp5E9;
_tmp5E8=x;goto _LL6;}case 3U: _LL5: _tmp5E8=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp5E8;
_tmp5E7=x;goto _LL8;}case 1U: _LL7: _tmp5E7=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp5E7;
# 3403
if(!x->escapes)
return x;
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3411
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5EB=0;struct Cyc_List_List*res=_tmp5EB;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)res=({struct Cyc_List_List*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->hd=*((void**)_check_null((void**)x->hd)),_tmp5EC->tl=res;_tmp5EC;});}}
return res;}
# 3418
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5ED=Cyc_Tcutil_compress(t);void*_stmttmp8B=_tmp5ED;void*_tmp5EE=_stmttmp8B;unsigned _tmp5F3;void*_tmp5F2;struct Cyc_Absyn_Exp*_tmp5F1;struct Cyc_Absyn_Tqual _tmp5F0;void*_tmp5EF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->tag == 4U){_LL1: _tmp5EF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).elt_type;_tmp5F0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).tq;_tmp5F1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).num_elts;_tmp5F2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).zero_term;_tmp5F3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).zt_loc;_LL2: {void*et=_tmp5EF;struct Cyc_Absyn_Tqual tq=_tmp5F0;struct Cyc_Absyn_Exp*eopt=_tmp5F1;void*zt=_tmp5F2;unsigned ztl=_tmp5F3;
# 3421
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 3425
if(convert_tag){
if(eopt->topt == 0)
({void*_tmp5F4=0U;({struct _fat_ptr _tmp79A=({const char*_tmp5F5="cannot convert tag without type!";_tag_fat(_tmp5F5,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp79A,_tag_fat(_tmp5F4,sizeof(void*),0U));});});{
void*_tmp5F6=Cyc_Tcutil_compress((void*)_check_null(eopt->topt));void*_stmttmp8C=_tmp5F6;void*_tmp5F7=_stmttmp8C;void*_tmp5F8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F7)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F7)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F7)->f2 != 0){_LL6: _tmp5F8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F7)->f2)->hd;_LL7: {void*t=_tmp5F8;
# 3431
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 3434
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3438
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
# 3440
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
