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
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1079
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1120
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1122
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1124
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1132
int Cyc_Absyn_fntype_att(void*);
# 1134
int Cyc_Absyn_equal_att(void*,void*);
# 1136
int Cyc_Absyn_attribute_cmp(void*,void*);
# 1142
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
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple1*f1;};struct _tuple13{unsigned f1;int f2;};
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
({void*_tmp644=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp644;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 223
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 229
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp645=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp645,b);});
if(_tmp37 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp38=_tmp37;
struct _tuple13 _tmp39=Cyc_Evexp_eval_const_uint_exp(_tmp38);struct _tuple13 _tmp3A=_tmp39;unsigned _tmp3C;int _tmp3B;_LL1: _tmp3C=_tmp3A.f1;_tmp3B=_tmp3A.f2;_LL2:;
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
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)_tmp49->hd)).f2))return 0;}
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
return({struct Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=tv->name,_tmp60->identity=- 1,({void*_tmp646=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp60->kind=_tmp646;});_tmp60;});}
# 367
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 369
struct _tuple9*_tmp61=arg;struct _fat_ptr*_tmp65;struct Cyc_Absyn_Tqual _tmp64;void*_tmp63;_LL1: _tmp65=_tmp61->f1;_tmp64=_tmp61->f2;_tmp63=_tmp61->f3;_LL2:;
return({struct _tuple9*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->f1=_tmp65,_tmp62->f2=_tmp64,({void*_tmp647=Cyc_Tcutil_copy_type(_tmp63);_tmp62->f3=_tmp647;});_tmp62;});}
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
struct _tuple15*_tmp68=x;void*_tmp6B;void*_tmp6A;_LL1: _tmp6B=_tmp68->f1;_tmp6A=_tmp68->f2;_LL2:;
return({struct _tuple15*_tmp69=_cycalloc(sizeof(*_tmp69));({void*_tmp64C=Cyc_Tcutil_copy_type(_tmp6B);_tmp69->f1=_tmp64C;}),({void*_tmp64B=Cyc_Tcutil_copy_type(_tmp6A);_tmp69->f2=_tmp64B;});_tmp69;});}
# 387
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 391
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6C=Cyc_Tcutil_compress(t);void*_tmp6D=_tmp6C;struct Cyc_Absyn_Datatypedecl*_tmpB1;struct Cyc_Absyn_Enumdecl*_tmpB0;struct Cyc_Absyn_Aggrdecl*_tmpAF;struct _tuple1*_tmpAE;struct Cyc_List_List*_tmpAD;struct Cyc_Absyn_Typedefdecl*_tmpAC;struct Cyc_Absyn_Exp*_tmpAB;struct Cyc_Absyn_Exp*_tmpAA;enum Cyc_Absyn_AggrKind _tmpA9;struct Cyc_List_List*_tmpA8;struct Cyc_List_List*_tmpA7;struct Cyc_List_List*_tmpA6;void*_tmpA5;struct Cyc_Absyn_Tqual _tmpA4;void*_tmpA3;struct Cyc_List_List*_tmpA2;int _tmpA1;struct Cyc_Absyn_VarargInfo*_tmpA0;struct Cyc_List_List*_tmp9F;struct Cyc_List_List*_tmp9E;struct Cyc_Absyn_Exp*_tmp9D;struct Cyc_List_List*_tmp9C;struct Cyc_Absyn_Exp*_tmp9B;struct Cyc_List_List*_tmp9A;void*_tmp99;struct Cyc_Absyn_Tqual _tmp98;struct Cyc_Absyn_Exp*_tmp97;void*_tmp96;unsigned _tmp95;void*_tmp94;struct Cyc_Absyn_Tqual _tmp93;void*_tmp92;void*_tmp91;void*_tmp90;void*_tmp8F;struct Cyc_Absyn_PtrLoc*_tmp8E;struct Cyc_Absyn_Tvar*_tmp8D;void*_tmp8C;struct Cyc_List_List*_tmp8B;void*_tmp8A;switch(*((int*)_tmp6D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2 == 0){_LL1: _tmp8A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_LL2:
 return t;}else{_LL3: _tmp8C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_tmp8B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp6E=_cycalloc(sizeof(*_tmp6E));_tmp6E->tag=0U,_tmp6E->f1=_tmp8C,({struct Cyc_List_List*_tmp64D=Cyc_Tcutil_copy_types(_tmp8B);_tmp6E->f2=_tmp64D;});_tmp6E;});}case 1U: _LL5: _LL6:
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
 return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp76=_cycalloc(sizeof(*_tmp76));_tmp76->tag=4U,({void*_tmp650=Cyc_Tcutil_copy_type(_tmp99);(_tmp76->f1).elt_type=_tmp650;}),(_tmp76->f1).tq=_tmp98,({struct Cyc_Absyn_Exp*_tmp64F=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp97);(_tmp76->f1).num_elts=_tmp64F;}),({
void*_tmp64E=Cyc_Tcutil_copy_type(_tmp96);(_tmp76->f1).zero_term=_tmp64E;}),(_tmp76->f1).zt_loc=_tmp95;_tmp76;});case 5U: _LLD: _tmpA6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).tvars;_tmpA5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).effect;_tmpA4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_tqual;_tmpA3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_type;_tmpA2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).args;_tmpA1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).c_varargs;_tmpA0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).cyc_varargs;_tmp9F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).rgn_po;_tmp9E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).attributes;_tmp9D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_clause;_tmp9C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_relns;_tmp9B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_clause;_tmp9A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_relns;_LLE: {
# 410
struct Cyc_List_List*_tmp77=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpA6);
void*effopt2=_tmpA5 == 0?0: Cyc_Tcutil_copy_type(_tmpA5);
void*_tmp78=Cyc_Tcutil_copy_type(_tmpA3);
struct Cyc_List_List*_tmp79=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpA2);
int _tmp7A=_tmpA1;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpA0 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpA0;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp7B=_cycalloc(sizeof(*_tmp7B));_tmp7B->name=cv->name,_tmp7B->tq=cv->tq,({void*_tmp651=Cyc_Tcutil_copy_type(cv->type);_tmp7B->type=_tmp651;}),_tmp7B->inject=cv->inject;_tmp7B;});}{
# 421
struct Cyc_List_List*_tmp7C=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmp9F);
struct Cyc_List_List*_tmp7D=_tmp9E;
struct Cyc_Absyn_Exp*_tmp7E=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp9D);
struct Cyc_List_List*_tmp7F=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp9C);
struct Cyc_Absyn_Exp*_tmp80=Cyc_Tcutil_deep_copy_exp_opt(1,_tmp9B);
struct Cyc_List_List*_tmp81=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmp9A);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp82=_cycalloc(sizeof(*_tmp82));_tmp82->tag=5U,(_tmp82->f1).tvars=_tmp77,(_tmp82->f1).effect=effopt2,(_tmp82->f1).ret_tqual=_tmpA4,(_tmp82->f1).ret_type=_tmp78,(_tmp82->f1).args=_tmp79,(_tmp82->f1).c_varargs=_tmp7A,(_tmp82->f1).cyc_varargs=cyc_varargs2,(_tmp82->f1).rgn_po=_tmp7C,(_tmp82->f1).attributes=_tmp7D,(_tmp82->f1).requires_clause=_tmp7E,(_tmp82->f1).requires_relns=_tmp7F,(_tmp82->f1).ensures_clause=_tmp80,(_tmp82->f1).ensures_relns=_tmp81;_tmp82;});};}case 6U: _LLF: _tmpA7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D)->f1;_LL10:
# 430
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->tag=6U,({struct Cyc_List_List*_tmp652=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpA7);_tmp83->f1=_tmp652;});_tmp83;});case 7U: _LL11: _tmpA9=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f1;_tmpA8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp84=_cycalloc(sizeof(*_tmp84));_tmp84->tag=7U,_tmp84->f1=_tmpA9,({struct Cyc_List_List*_tmp653=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpA8);_tmp84->f2=_tmp653;});_tmp84;});case 9U: _LL13: _tmpAA=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL14:
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->tag=9U,_tmp85->f1=_tmpAA;_tmp85;});case 11U: _LL15: _tmpAB=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6D)->f1;_LL16:
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp86=_cycalloc(sizeof(*_tmp86));_tmp86->tag=11U,_tmp86->f1=_tmpAB;_tmp86;});case 8U: _LL17: _tmpAE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f1;_tmpAD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f2;_tmpAC=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f3;_LL18:
# 435
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp87=_cycalloc(sizeof(*_tmp87));_tmp87->tag=8U,_tmp87->f1=_tmpAE,({struct Cyc_List_List*_tmp654=Cyc_Tcutil_copy_types(_tmpAD);_tmp87->f2=_tmp654;}),_tmp87->f3=_tmpAC,_tmp87->f4=0;_tmp87;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)){case 0U: _LL19: _tmpAF=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1A: {
# 438
struct Cyc_List_List*_tmp88=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpAF->tvs);
return({union Cyc_Absyn_AggrInfo _tmp655=Cyc_Absyn_UnknownAggr(_tmpAF->kind,_tmpAF->name,0);Cyc_Absyn_aggr_type(_tmp655,_tmp88);});}case 1U: _LL1B: _tmpB0=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1C:
# 441
 return Cyc_Absyn_enum_type(_tmpB0->name,0);default: _LL1D: _tmpB1=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1E: {
# 443
struct Cyc_List_List*_tmp89=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpB1->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmp656=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5F9;_tmp5F9.name=_tmpB1->name,_tmp5F9.is_extensible=0;_tmp5F9;}));Cyc_Absyn_datatype_type(_tmp656,_tmp89);});}}}_LL0:;}
# 450
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB2=d;struct _fat_ptr*_tmpB5;struct Cyc_Absyn_Exp*_tmpB4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->tag == 0U){_LL1: _tmpB4=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->tag=0U,({struct Cyc_Absyn_Exp*_tmp657=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpB4);_tmpB3->f1=_tmp657;});_tmpB3;});}else{_LL3: _tmpB5=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB2)->f1;_LL4:
 return d;}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 456
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 458
return({struct _tuple16*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_List_List*_tmp659=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB6->f1=_tmp659;}),({struct Cyc_Absyn_Exp*_tmp658=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB6->f2=_tmp658;});_tmpB6;});}
# 461
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB7=preserve_types;
{void*_tmpB8=e->r;void*_tmpB9=_tmpB8;int _tmp138;struct _fat_ptr _tmp137;struct Cyc_List_List*_tmp136;struct Cyc_List_List*_tmp135;struct Cyc_List_List*_tmp134;void*_tmp133;struct Cyc_Absyn_Exp*_tmp132;struct _fat_ptr*_tmp131;struct Cyc_Core_Opt*_tmp130;struct Cyc_List_List*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct Cyc_Absyn_Exp*_tmp12D;int _tmp12C;struct Cyc_Absyn_Exp*_tmp12B;void**_tmp12A;struct Cyc_Absyn_Exp*_tmp129;int _tmp128;int _tmp127;void*_tmp126;struct Cyc_Absyn_Enumfield*_tmp125;struct Cyc_Absyn_Enumdecl*_tmp124;struct Cyc_Absyn_Enumfield*_tmp123;struct _fat_ptr*_tmp122;struct Cyc_Absyn_Tqual _tmp121;void*_tmp120;struct Cyc_List_List*_tmp11F;void*_tmp11E;struct Cyc_List_List*_tmp11D;struct _tuple1*_tmp11C;struct Cyc_List_List*_tmp11B;struct Cyc_List_List*_tmp11A;struct Cyc_Absyn_Aggrdecl*_tmp119;struct Cyc_Absyn_Exp*_tmp118;void*_tmp117;int _tmp116;struct Cyc_Absyn_Vardecl*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;int _tmp112;struct Cyc_List_List*_tmp111;struct Cyc_Absyn_Datatypedecl*_tmp110;struct Cyc_Absyn_Datatypefield*_tmp10F;struct Cyc_List_List*_tmp10E;struct Cyc_Absyn_Exp*_tmp10D;struct Cyc_Absyn_Exp*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct _fat_ptr*_tmp10A;int _tmp109;int _tmp108;struct Cyc_Absyn_Exp*_tmp107;struct _fat_ptr*_tmp106;int _tmp105;int _tmp104;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;void*_tmp100;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;void*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;void*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;int _tmpF7;enum Cyc_Absyn_Coercion _tmpF6;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF3;struct Cyc_Absyn_Exp*_tmpF2;int _tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpEF;struct Cyc_Absyn_VarargCallInfo*_tmpEE;int _tmpED;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_List_List*_tmpEB;int _tmpEA;struct Cyc_List_List*_tmpE9;struct _fat_ptr*_tmpE8;struct Cyc_Absyn_Tqual _tmpE7;void*_tmpE6;int _tmpE5;int _tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Core_Opt*_tmpE2;struct Cyc_Absyn_Exp*_tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Exp*_tmpDE;struct Cyc_Absyn_Exp*_tmpDD;struct Cyc_Absyn_Exp*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;struct Cyc_Absyn_Exp*_tmpDA;struct Cyc_Absyn_Exp*_tmpD9;struct Cyc_Absyn_Exp*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;enum Cyc_Absyn_Incrementor _tmpD6;enum Cyc_Absyn_Primop _tmpD5;struct Cyc_List_List*_tmpD4;struct _fat_ptr _tmpD3;void*_tmpD2;union Cyc_Absyn_Cnst _tmpD1;switch(*((int*)_tmpB9)){case 0U: _LL1: _tmpD1=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2:
 new_e=Cyc_Absyn_const_exp(_tmpD1,e->loc);goto _LL0;case 1U: _LL3: _tmpD2=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL4:
 new_e=Cyc_Absyn_varb_exp(_tmpD2,e->loc);goto _LL0;case 2U: _LL5: _tmpD3=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL6:
 new_e=Cyc_Absyn_pragma_exp(_tmpD3,e->loc);goto _LL0;case 3U: _LL7: _tmpD5=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL8:
 new_e=({enum Cyc_Absyn_Primop _tmp65B=_tmpD5;struct Cyc_List_List*_tmp65A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpD4);Cyc_Absyn_primop_exp(_tmp65B,_tmp65A,e->loc);});goto _LL0;case 5U: _LL9: _tmpD7=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD6=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLA:
 new_e=({struct Cyc_Absyn_Exp*_tmp65D=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD7);enum Cyc_Absyn_Incrementor _tmp65C=_tmpD6;Cyc_Absyn_increment_exp(_tmp65D,_tmp65C,e->loc);});goto _LL0;case 7U: _LLB: _tmpD9=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD8=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLC:
 new_e=({struct Cyc_Absyn_Exp*_tmp65F=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD9);struct Cyc_Absyn_Exp*_tmp65E=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpD8);Cyc_Absyn_and_exp(_tmp65F,_tmp65E,e->loc);});goto _LL0;case 8U: _LLD: _tmpDB=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDA=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLE:
 new_e=({struct Cyc_Absyn_Exp*_tmp661=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDB);struct Cyc_Absyn_Exp*_tmp660=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDA);Cyc_Absyn_or_exp(_tmp661,_tmp660,e->loc);});goto _LL0;case 9U: _LLF: _tmpDD=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL10:
 new_e=({struct Cyc_Absyn_Exp*_tmp663=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDD);struct Cyc_Absyn_Exp*_tmp662=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDC);Cyc_Absyn_seq_exp(_tmp663,_tmp662,e->loc);});goto _LL0;case 6U: _LL11: _tmpE0=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpDE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL12:
# 474
 new_e=({struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE0);struct Cyc_Absyn_Exp*_tmp665=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDF);struct Cyc_Absyn_Exp*_tmp664=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpDE);Cyc_Absyn_conditional_exp(_tmp666,_tmp665,_tmp664,e->loc);});goto _LL0;case 4U: _LL13: _tmpE3=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE2=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE1=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL14:
# 476
 new_e=({struct Cyc_Absyn_Exp*_tmp669=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE3);struct Cyc_Core_Opt*_tmp668=(unsigned)_tmpE2?({struct Cyc_Core_Opt*_tmpBA=_cycalloc(sizeof(*_tmpBA));_tmpBA->v=(void*)_tmpE2->v;_tmpBA;}): 0;struct Cyc_Absyn_Exp*_tmp667=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpE1);Cyc_Absyn_assignop_exp(_tmp669,_tmp668,_tmp667,e->loc);});
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3 != 0){_LL15: _tmpEC=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpEB=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEA=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->num_varargs;_tmpE9=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->injectors;_tmpE8=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->name;_tmpE7=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->tq;_tmpE6=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->type;_tmpE5=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->inject;_tmpE4=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL16:
# 479
 new_e=({void*_tmp66F=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBD=_cycalloc(sizeof(*_tmpBD));_tmpBD->tag=10U,({
struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpEC);_tmpBD->f1=_tmp66E;}),({struct Cyc_List_List*_tmp66D=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpEB);_tmpBD->f2=_tmp66D;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp66C=({struct Cyc_Absyn_VarargCallInfo*_tmpBC=_cycalloc(sizeof(*_tmpBC));_tmpBC->num_varargs=_tmpEA,_tmpBC->injectors=_tmpE9,({
struct Cyc_Absyn_VarargInfo*_tmp66B=({struct Cyc_Absyn_VarargInfo*_tmpBB=_cycalloc(sizeof(*_tmpBB));_tmpBB->name=_tmpE8,_tmpBB->tq=_tmpE7,({void*_tmp66A=Cyc_Tcutil_copy_type(_tmpE6);_tmpBB->type=_tmp66A;}),_tmpBB->inject=_tmpE5;_tmpBB;});_tmpBC->vai=_tmp66B;});_tmpBC;});
# 481
_tmpBD->f3=_tmp66C;}),_tmpBD->f4=_tmpE4;_tmpBD;});
# 479
Cyc_Absyn_new_exp(_tmp66F,e->loc);});
# 484
goto _LL0;}else{_LL17: _tmpF0=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpEF=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEE=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpED=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL18:
# 486
 new_e=({void*_tmp672=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpBE=_cycalloc(sizeof(*_tmpBE));_tmpBE->tag=10U,({struct Cyc_Absyn_Exp*_tmp671=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF0);_tmpBE->f1=_tmp671;}),({struct Cyc_List_List*_tmp670=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmpEF);_tmpBE->f2=_tmp670;}),_tmpBE->f3=_tmpEE,_tmpBE->f4=_tmpED;_tmpBE;});Cyc_Absyn_new_exp(_tmp672,e->loc);});
goto _LL0;}case 11U: _LL19: _tmpF2=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF1=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1A:
# 489
 new_e=_tmpF1?({struct Cyc_Absyn_Exp*_tmp674=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF2);Cyc_Absyn_rethrow_exp(_tmp674,e->loc);}):({struct Cyc_Absyn_Exp*_tmp673=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF2);Cyc_Absyn_throw_exp(_tmp673,e->loc);});
goto _LL0;case 12U: _LL1B: _tmpF3=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL1C:
# 492
 new_e=({struct Cyc_Absyn_Exp*_tmp675=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF3);Cyc_Absyn_noinstantiate_exp(_tmp675,e->loc);});goto _LL0;case 13U: _LL1D: _tmpF5=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1E:
# 494
 new_e=({struct Cyc_Absyn_Exp*_tmp677=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF5);struct Cyc_List_List*_tmp676=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmpF4);Cyc_Absyn_instantiate_exp(_tmp677,_tmp676,e->loc);});goto _LL0;case 14U: _LL1F: _tmpF9=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF8=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpF7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpF6=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL20:
# 496
 new_e=({void*_tmp67B=Cyc_Tcutil_copy_type(_tmpF9);struct Cyc_Absyn_Exp*_tmp67A=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpF8);int _tmp679=_tmpF7;enum Cyc_Absyn_Coercion _tmp678=_tmpF6;Cyc_Absyn_cast_exp(_tmp67B,_tmp67A,_tmp679,_tmp678,e->loc);});goto _LL0;case 16U: _LL21: _tmpFB=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFA=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL22:
# 498
 new_e=({struct Cyc_Absyn_Exp*_tmp67D=(unsigned)_tmpFB?Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFB): 0;struct Cyc_Absyn_Exp*_tmp67C=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFA);Cyc_Absyn_New_exp(_tmp67D,_tmp67C,e->loc);});goto _LL0;case 15U: _LL23: _tmpFC=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL24:
 new_e=({struct Cyc_Absyn_Exp*_tmp67E=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFC);Cyc_Absyn_address_exp(_tmp67E,e->loc);});goto _LL0;case 17U: _LL25: _tmpFD=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL26:
 new_e=({void*_tmp67F=Cyc_Tcutil_copy_type(_tmpFD);Cyc_Absyn_sizeoftype_exp(_tmp67F,e->loc);});goto _LL0;case 18U: _LL27: _tmpFE=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL28:
 new_e=({struct Cyc_Absyn_Exp*_tmp680=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmpFE);Cyc_Absyn_sizeofexp_exp(_tmp680,e->loc);});goto _LL0;case 19U: _LL29: _tmp100=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFF=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL2A:
 new_e=({void*_tmp682=Cyc_Tcutil_copy_type(_tmp100);struct Cyc_List_List*_tmp681=_tmpFF;Cyc_Absyn_offsetof_exp(_tmp682,_tmp681,e->loc);});goto _LL0;case 20U: _LL2B: _tmp101=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2C:
 new_e=({struct Cyc_Absyn_Exp*_tmp683=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp101);Cyc_Absyn_deref_exp(_tmp683,e->loc);});goto _LL0;case 41U: _LL2D: _tmp102=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2E:
 new_e=({struct Cyc_Absyn_Exp*_tmp684=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp102);Cyc_Absyn_extension_exp(_tmp684,e->loc);});goto _LL0;case 24U: _LL2F: _tmp103=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL30:
 new_e=({struct Cyc_List_List*_tmp685=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmp103);Cyc_Absyn_tuple_exp(_tmp685,e->loc);});goto _LL0;case 21U: _LL31: _tmp107=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp106=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp105=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp104=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL32:
# 507
 new_e=({void*_tmp687=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpBF=_cycalloc(sizeof(*_tmpBF));_tmpBF->tag=21U,({struct Cyc_Absyn_Exp*_tmp686=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp107);_tmpBF->f1=_tmp686;}),_tmpBF->f2=_tmp106,_tmpBF->f3=_tmp105,_tmpBF->f4=_tmp104;_tmpBF;});Cyc_Absyn_new_exp(_tmp687,e->loc);});goto _LL0;case 22U: _LL33: _tmp10B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp10A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp109=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp108=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL34:
# 509
 new_e=({void*_tmp689=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpC0=_cycalloc(sizeof(*_tmpC0));_tmpC0->tag=22U,({struct Cyc_Absyn_Exp*_tmp688=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10B);_tmpC0->f1=_tmp688;}),_tmpC0->f2=_tmp10A,_tmpC0->f3=_tmp109,_tmpC0->f4=_tmp108;_tmpC0;});Cyc_Absyn_new_exp(_tmp689,e->loc);});goto _LL0;case 23U: _LL35: _tmp10D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp10C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL36:
# 511
 new_e=({struct Cyc_Absyn_Exp*_tmp68B=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10D);struct Cyc_Absyn_Exp*_tmp68A=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp10C);Cyc_Absyn_subscript_exp(_tmp68B,_tmp68A,e->loc);});goto _LL0;case 26U: _LL37: _tmp10E=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL38:
# 513
 new_e=({void*_tmp68D=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpC1=_cycalloc(sizeof(*_tmpC1));_tmpC1->tag=26U,({struct Cyc_List_List*_tmp68C=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp10E);_tmpC1->f1=_tmp68C;});_tmpC1;});Cyc_Absyn_new_exp(_tmp68D,e->loc);});goto _LL0;case 31U: _LL39: _tmp111=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp110=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp10F=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3A:
# 515
 new_e=({void*_tmp68F=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpC2=_cycalloc(sizeof(*_tmpC2));_tmpC2->tag=31U,({struct Cyc_List_List*_tmp68E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpB7,_tmp111);_tmpC2->f1=_tmp68E;}),_tmpC2->f2=_tmp110,_tmpC2->f3=_tmp10F;_tmpC2;});Cyc_Absyn_new_exp(_tmp68F,e->loc);});goto _LL0;case 27U: _LL3B: _tmp115=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp114=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp113=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp112=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL3C:
# 517
 new_e=({void*_tmp692=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpC3=_cycalloc(sizeof(*_tmpC3));_tmpC3->tag=27U,_tmpC3->f1=_tmp115,({struct Cyc_Absyn_Exp*_tmp691=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp114);_tmpC3->f2=_tmp691;}),({struct Cyc_Absyn_Exp*_tmp690=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp113);_tmpC3->f3=_tmp690;}),_tmpC3->f4=_tmp112;_tmpC3;});Cyc_Absyn_new_exp(_tmp692,e->loc);});goto _LL0;case 28U: _LL3D: _tmp118=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp117=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp116=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3E:
# 519
 new_e=({void*_tmp695=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpC4=_cycalloc(sizeof(*_tmpC4));_tmpC4->tag=28U,({struct Cyc_Absyn_Exp*_tmp694=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp118);_tmpC4->f1=_tmp694;}),({void*_tmp693=Cyc_Tcutil_copy_type(_tmp117);_tmpC4->f2=_tmp693;}),_tmpC4->f3=_tmp116;_tmpC4;});Cyc_Absyn_new_exp(_tmp695,_tmp118->loc);});
goto _LL0;case 29U: _LL3F: _tmp11C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11B=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp11A=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp119=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL40:
# 522
 new_e=({void*_tmp698=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpC5=_cycalloc(sizeof(*_tmpC5));_tmpC5->tag=29U,_tmpC5->f1=_tmp11C,({struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp11B);_tmpC5->f2=_tmp697;}),({struct Cyc_List_List*_tmp696=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11A);_tmpC5->f3=_tmp696;}),_tmpC5->f4=_tmp119;_tmpC5;});Cyc_Absyn_new_exp(_tmp698,e->loc);});
# 524
goto _LL0;case 30U: _LL41: _tmp11E=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11D=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL42:
# 526
 new_e=({void*_tmp69B=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpC6=_cycalloc(sizeof(*_tmpC6));_tmpC6->tag=30U,({void*_tmp69A=Cyc_Tcutil_copy_type(_tmp11E);_tmpC6->f1=_tmp69A;}),({struct Cyc_List_List*_tmp699=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11D);_tmpC6->f2=_tmp699;});_tmpC6;});Cyc_Absyn_new_exp(_tmp69B,e->loc);});
goto _LL0;case 25U: _LL43: _tmp122=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f1;_tmp121=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f2;_tmp120=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f3;_tmp11F=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL44:
# 529
 new_e=({void*_tmp69F=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpC8=_cycalloc(sizeof(*_tmpC8));_tmpC8->tag=25U,({struct _tuple9*_tmp69E=({struct _tuple9*_tmpC7=_cycalloc(sizeof(*_tmpC7));_tmpC7->f1=_tmp122,_tmpC7->f2=_tmp121,({void*_tmp69D=Cyc_Tcutil_copy_type(_tmp120);_tmpC7->f3=_tmp69D;});_tmpC7;});_tmpC8->f1=_tmp69E;}),({
struct Cyc_List_List*_tmp69C=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp11F);_tmpC8->f2=_tmp69C;});_tmpC8;});
# 529
Cyc_Absyn_new_exp(_tmp69F,e->loc);});
# 531
goto _LL0;case 32U: _LL45: _tmp124=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp123=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL46:
 new_e=e;goto _LL0;case 33U: _LL47: _tmp126=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp125=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL48:
# 534
 new_e=({void*_tmp6A1=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpC9=_cycalloc(sizeof(*_tmpC9));_tmpC9->tag=33U,({void*_tmp6A0=Cyc_Tcutil_copy_type(_tmp126);_tmpC9->f1=_tmp6A0;}),_tmpC9->f2=_tmp125;_tmpC9;});Cyc_Absyn_new_exp(_tmp6A1,e->loc);});goto _LL0;case 34U: _LL49: _tmp12C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).is_calloc;_tmp12B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).rgn;_tmp12A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).elt_type;_tmp129=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).num_elts;_tmp128=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).fat_result;_tmp127=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).inline_call;_LL4A: {
# 536
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp12B;if(_tmp12B != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12B);{
void**t1=_tmp12A;if(_tmp12A != 0)t1=({void**_tmpCA=_cycalloc(sizeof(*_tmpCA));({void*_tmp6A2=Cyc_Tcutil_copy_type(*_tmp12A);*_tmpCA=_tmp6A2;});_tmpCA;});
({void*_tmp6A3=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpCB=_cycalloc(sizeof(*_tmpCB));_tmpCB->tag=34U,(_tmpCB->f1).is_calloc=_tmp12C,(_tmpCB->f1).rgn=r1,(_tmpCB->f1).elt_type=t1,(_tmpCB->f1).num_elts=_tmp129,(_tmpCB->f1).fat_result=_tmp128,(_tmpCB->f1).inline_call=_tmp127;_tmpCB;});e2->r=_tmp6A3;});
new_e=e2;
goto _LL0;};}case 35U: _LL4B: _tmp12E=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp12D=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4C:
 new_e=({struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12E);struct Cyc_Absyn_Exp*_tmp6A4=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp12D);Cyc_Absyn_swap_exp(_tmp6A5,_tmp6A4,e->loc);});goto _LL0;case 36U: _LL4D: _tmp130=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp12F=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4E: {
# 544
struct Cyc_Core_Opt*nopt1=_tmp130;
if(_tmp130 != 0)nopt1=({struct Cyc_Core_Opt*_tmpCC=_cycalloc(sizeof(*_tmpCC));_tmpCC->v=(struct _tuple1*)_tmp130->v;_tmpCC;});
new_e=({void*_tmp6A7=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpCD=_cycalloc(sizeof(*_tmpCD));_tmpCD->tag=36U,_tmpCD->f1=nopt1,({struct Cyc_List_List*_tmp6A6=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpB7,_tmp12F);_tmpCD->f2=_tmp6A6;});_tmpCD;});Cyc_Absyn_new_exp(_tmp6A7,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 549
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpCF=_cycalloc(sizeof(*_tmpCF));_tmpCF->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6A8=({const char*_tmpCE="deep_copy: statement expressions unsupported";_tag_fat(_tmpCE,sizeof(char),45U);});_tmpCF->f1=_tmp6A8;});_tmpCF;}));case 38U: _LL51: _tmp132=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp131=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL52:
 new_e=({void*_tmp6AA=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpD0=_cycalloc(sizeof(*_tmpD0));_tmpD0->tag=38U,({struct Cyc_Absyn_Exp*_tmp6A9=Cyc_Tcutil_deep_copy_exp(_tmpB7,_tmp132);_tmpD0->f1=_tmp6A9;}),_tmpD0->f2=_tmp131;_tmpD0;});Cyc_Absyn_new_exp(_tmp6AA,e->loc);});
goto _LL0;case 39U: _LL53: _tmp133=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL54:
 new_e=({void*_tmp6AB=Cyc_Tcutil_copy_type(_tmp133);Cyc_Absyn_valueof_exp(_tmp6AB,e->loc);});
goto _LL0;default: _LL55: _tmp138=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp137=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp136=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp135=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_tmp134=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f5;_LL56:
 new_e=Cyc_Absyn_asm_exp(_tmp138,_tmp137,_tmp136,_tmp135,_tmp134,e->loc);goto _LL0;}_LL0:;}
# 557
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 561
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 572 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp139=ka1;enum Cyc_Absyn_KindQual _tmp142;enum Cyc_Absyn_AliasQual _tmp141;_LL1: _tmp142=_tmp139->kind;_tmp141=_tmp139->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp13A=ka2;enum Cyc_Absyn_KindQual _tmp140;enum Cyc_Absyn_AliasQual _tmp13F;_LL4: _tmp140=_tmp13A->kind;_tmp13F=_tmp13A->aliasqual;_LL5:;
# 576
if((int)_tmp142 != (int)_tmp140){
struct _tuple17 _tmp13B=({struct _tuple17 _tmp5FA;_tmp5FA.f1=_tmp142,_tmp5FA.f2=_tmp140;_tmp5FA;});struct _tuple17 _tmp13C=_tmp13B;switch(_tmp13C.f1){case Cyc_Absyn_BoxKind: switch(_tmp13C.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp13C.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 584
if((int)_tmp141 != (int)_tmp13F){
struct _tuple18 _tmp13D=({struct _tuple18 _tmp5FB;_tmp5FB.f1=_tmp141,_tmp5FB.f2=_tmp13F;_tmp5FB;});struct _tuple18 _tmp13E=_tmp13D;switch(_tmp13E.f1){case Cyc_Absyn_Aliasable: if(_tmp13E.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp13E.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 590
return 1;};}
# 593
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp143=Cyc_Absyn_compress_kb(tv->kind);void*_tmp144=_tmp143;struct Cyc_Absyn_Kind*_tmp147;struct Cyc_Absyn_Kind*_tmp146;switch(*((int*)_tmp144)){case 0U: _LL1: _tmp146=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp144)->f1;_LL2:
 return _tmp146;case 2U: _LL3: _tmp147=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp144)->f2;_LL4:
 return _tmp147;default: _LL5: _LL6:
({void*_tmp6AC=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp145=_cycalloc(sizeof(*_tmp145));_tmp145->tag=2U,_tmp145->f1=0,_tmp145->f2=def;_tmp145;});tv->kind=_tmp6AC;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 601
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp148=Cyc_Tcutil_compress(t);void*_tmp149=_tmp148;struct Cyc_Absyn_Tvar*_tmp14E;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->tag == 2U){_LL1: _tmp14E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp149)->f1;_LL2: {
# 604
void*_tmp14A=_tmp14E->kind;
_tmp14E->kind=kb;
return({struct _tuple19 _tmp5FC;_tmp5FC.f1=_tmp14E,_tmp5FC.f2=_tmp14A;_tmp5FC;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp14D=({struct Cyc_String_pa_PrintArg_struct _tmp5FD;_tmp5FD.tag=0U,({struct _fat_ptr _tmp6AD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5FD.f1=_tmp6AD;});_tmp5FD;});void*_tmp14B[1U];_tmp14B[0]=& _tmp14D;({struct _fat_ptr _tmp6AE=({const char*_tmp14C="swap_kind: cannot update the kind of %s";_tag_fat(_tmp14C,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6AE,_tag_fat(_tmp14B,sizeof(void*),1U));});});}_LL0:;}
# 613
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 615
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 619
struct _RegionHandle _tmp14F=_new_region("temp");struct _RegionHandle*temp=& _tmp14F;_push_region(temp);
{struct Cyc_List_List*_tmp150=0;
# 622
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp151=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp152=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp153=Cyc_Tcutil_tvar_kind(_tmp151,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp154=_tmp153;switch(((struct Cyc_Absyn_Kind*)_tmp154)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 628
 _tmp150=({struct Cyc_List_List*_tmp156=_region_malloc(temp,sizeof(*_tmp156));({struct _tuple19*_tmp6AF=({struct _tuple19*_tmp155=_region_malloc(temp,sizeof(*_tmp155));_tmp155->f1=_tmp151,_tmp155->f2=_tmp152;_tmp155;});_tmp156->hd=_tmp6AF;}),_tmp156->tl=_tmp150;_tmp156;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 632
if(_tmp150 != 0){
field_type=({struct _RegionHandle*_tmp6B1=temp;struct Cyc_List_List*_tmp6B0=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp150);Cyc_Tcutil_rsubstitute(_tmp6B1,_tmp6B0,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 620
;_pop_region(temp);}
# 637
return k;}
# 644
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 646
void*_tmp157=Cyc_Tcutil_compress(t);void*_tmp158=_tmp157;struct Cyc_Absyn_Typedefdecl*_tmp176;struct Cyc_Absyn_Exp*_tmp175;struct Cyc_Absyn_PtrInfo _tmp174;void*_tmp173;struct Cyc_List_List*_tmp172;struct Cyc_Absyn_Tvar*_tmp171;struct Cyc_Core_Opt*_tmp170;switch(*((int*)_tmp158)){case 1U: _LL1: _tmp170=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp158)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp170))->v;case 2U: _LL3: _tmp171=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp158)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp171,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp173=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp158)->f1;_tmp172=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp158)->f2;_LL6: {
# 650
void*_tmp159=_tmp173;enum Cyc_Absyn_AggrKind _tmp166;struct Cyc_List_List*_tmp165;struct Cyc_Absyn_AggrdeclImpl*_tmp164;int _tmp163;struct Cyc_Absyn_Kind*_tmp162;enum Cyc_Absyn_Size_of _tmp161;switch(*((int*)_tmp159)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp161=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp159)->f2;_LL21:
 return((int)_tmp161 == (int)2U ||(int)_tmp161 == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp162=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp159)->f2;_LL31:
 return _tmp162;case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp159)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 671
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 673
({void*_tmp15A=0U;({struct _fat_ptr _tmp6B2=({const char*_tmp15B="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp15B,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B2,_tag_fat(_tmp15A,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp159)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 677
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp166=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp159)->f1).KnownAggr).val)->kind;_tmp165=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp159)->f1).KnownAggr).val)->tvs;_tmp164=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp159)->f1).KnownAggr).val)->impl;_tmp163=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp159)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 679
 if(_tmp164 == 0){
if(_tmp163)
return& Cyc_Tcutil_mk;else{
# 683
return& Cyc_Tcutil_ak;}}{
# 685
struct Cyc_List_List*_tmp15C=_tmp164->fields;
if(_tmp15C == 0)return& Cyc_Tcutil_mk;
# 688
if((int)_tmp166 == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(_tmp15C))->tl != 0;_tmp15C=_tmp15C->tl){;}{
void*_tmp15D=((struct Cyc_Absyn_Aggrfield*)_tmp15C->hd)->type;
struct Cyc_Absyn_Kind*_tmp15E=Cyc_Tcutil_field_kind(_tmp15D,_tmp172,_tmp165);
if(_tmp15E == & Cyc_Tcutil_ak || _tmp15E == & Cyc_Tcutil_tak)return _tmp15E;};}else{
# 696
for(0;_tmp15C != 0;_tmp15C=_tmp15C->tl){
void*_tmp15F=((struct Cyc_Absyn_Aggrfield*)_tmp15C->hd)->type;
struct Cyc_Absyn_Kind*_tmp160=Cyc_Tcutil_field_kind(_tmp15F,_tmp172,_tmp165);
if(_tmp160 == & Cyc_Tcutil_ak || _tmp160 == & Cyc_Tcutil_tak)return _tmp160;}}
# 702
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 704
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp174=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp158)->f1;_LLC: {
# 707
void*_tmp167=Cyc_Tcutil_compress((_tmp174.ptr_atts).bounds);void*_tmp168=_tmp167;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp168)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp168)->f1)){case 13U: _LL4D: _LL4E: {
# 709
enum Cyc_Absyn_AliasQual _tmp169=(Cyc_Tcutil_type_kind((_tmp174.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp16A=_tmp169;switch(_tmp16A){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 715
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 717
enum Cyc_Absyn_AliasQual _tmp16B=(Cyc_Tcutil_type_kind((_tmp174.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp16C=_tmp16B;switch(_tmp16C){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 723
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 727
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp175=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp158)->f1).num_elts;_LL12:
# 729
 if(_tmp175 == 0 || Cyc_Tcutil_is_const_exp(_tmp175))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp176=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp158)->f3;_LL16:
# 733
 if(_tmp176 == 0 || _tmp176->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp16F=({struct Cyc_String_pa_PrintArg_struct _tmp5FE;_tmp5FE.tag=0U,({struct _fat_ptr _tmp6B3=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp5FE.f1=_tmp6B3;});_tmp5FE;});void*_tmp16D[1U];_tmp16D[0]=& _tmp16F;({struct _fat_ptr _tmp6B4=({const char*_tmp16E="type_kind: typedef found: %s";_tag_fat(_tmp16E,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B4,_tag_fat(_tmp16D,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp176->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp158)->f1)->r)){case 0U: _LL17: _LL18:
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
{struct Cyc_Absyn_Kind*_tmp178=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp179=_tmp178;switch(((struct Cyc_Absyn_Kind*)_tmp179)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp179)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
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
void*_tmp17C=Cyc_Tcutil_compress(t);void*_tmp17D=_tmp17C;struct Cyc_List_List*_tmp193;struct Cyc_List_List*_tmp192;struct Cyc_List_List*_tmp191;void*_tmp190;struct Cyc_Absyn_Tqual _tmp18F;void*_tmp18E;struct Cyc_List_List*_tmp18D;struct Cyc_Absyn_VarargInfo*_tmp18C;struct Cyc_List_List*_tmp18B;struct Cyc_List_List*_tmp18A;void*_tmp189;void*_tmp188;void*_tmp187;struct Cyc_List_List*_tmp186;switch(*((int*)_tmp17D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17D)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17D)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp186=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17D)->f2;_LL6: {
# 799
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp186);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 803
struct Cyc_Absyn_Kind*_tmp17E=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp17F=_tmp17E;switch(((struct Cyc_Absyn_Kind*)_tmp17F)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp188=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17D)->f1).elt_type;_tmp187=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17D)->f1).ptr_atts).rgn;_LLC:
# 811
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp180[2U];({void*_tmp6B7=Cyc_Absyn_access_eff(_tmp187);_tmp180[0]=_tmp6B7;}),({void*_tmp6B6=Cyc_Tcutil_rgns_of(_tmp188);_tmp180[1]=_tmp6B6;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp180,sizeof(void*),2U));})));case 4U: _LLD: _tmp189=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17D)->f1).elt_type;_LLE:
# 813
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp189));case 7U: _LLF: _tmp18A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp17D)->f2;_LL10:
# 815
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp18A)));case 5U: _LL11: _tmp191=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).tvars;_tmp190=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).effect;_tmp18F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).ret_tqual;_tmp18E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).ret_type;_tmp18D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).args;_tmp18C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).cyc_varargs;_tmp18B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17D)->f1).rgn_po;_LL12: {
# 824
void*_tmp181=({struct Cyc_List_List*_tmp6B8=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp191);Cyc_Tcutil_substitute(_tmp6B8,(void*)_check_null(_tmp190));});
return Cyc_Tcutil_normalize_effect(_tmp181);}case 6U: _LL13: _tmp192=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp17D)->f1;_LL14: {
# 827
struct Cyc_List_List*_tmp182=0;
for(0;_tmp192 != 0;_tmp192=_tmp192->tl){
_tmp182=({struct Cyc_List_List*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->hd=(*((struct _tuple12*)_tmp192->hd)).f2,_tmp183->tl=_tmp182;_tmp183;});}
_tmp193=_tmp182;goto _LL16;}case 8U: _LL15: _tmp193=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17D)->f2;_LL16:
# 832
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp193)));case 10U: _LL17: _LL18:
({void*_tmp184=0U;({struct _fat_ptr _tmp6B9=({const char*_tmp185="typedecl in rgns_of";_tag_fat(_tmp185,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B9,_tag_fat(_tmp184,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 842
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp194=e;void*_tmp1A0;struct Cyc_List_List**_tmp19F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f1)){case 9U: _LL1: _tmp19F=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2;_LL2: {
# 846
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp19F;for(0;effs != 0;effs=effs->tl){
void*_tmp195=(void*)effs->hd;
({void*_tmp6BA=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp195));effs->hd=_tmp6BA;});{
void*_tmp196=(void*)effs->hd;void*_tmp197=_tmp196;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp197)->f2)->tl == 0){_LLE: _LLF:
# 855
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 859
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp19F;for(0;effs != 0;effs=effs->tl){
void*_tmp198=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp199=_tmp198;void*_tmp19C;struct Cyc_List_List*_tmp19B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f1)){case 9U: _LL13: _tmp19B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2;_LL14:
# 864
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp19B,effects);
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp19C=_tmp199;_LL1C:
 effects=({struct Cyc_List_List*_tmp19A=_cycalloc(sizeof(*_tmp19A));_tmp19A->hd=_tmp19C,_tmp19A->tl=effects;_tmp19A;});goto _LL12;}_LL12:;}}
# 872
({struct Cyc_List_List*_tmp6BB=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp19F=_tmp6BB;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2 != 0){_LL3: _tmp1A0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp194)->f2)->hd;_LL4: {
# 875
void*_tmp19D=Cyc_Tcutil_compress(_tmp1A0);void*_tmp19E=_tmp19D;switch(*((int*)_tmp19E)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp1A0);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 880
 return e;}_LL0:;};}
# 885
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A1=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A2=_cycalloc(sizeof(*_tmp1A2));_tmp1A2->tag=5U,(_tmp1A2->f1).tvars=0,(_tmp1A2->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6BC=Cyc_Absyn_empty_tqual(0U);(_tmp1A2->f1).ret_tqual=_tmp6BC;}),(_tmp1A2->f1).ret_type=Cyc_Absyn_void_type,(_tmp1A2->f1).args=0,(_tmp1A2->f1).c_varargs=0,(_tmp1A2->f1).cyc_varargs=0,(_tmp1A2->f1).rgn_po=0,(_tmp1A2->f1).attributes=0,(_tmp1A2->f1).requires_clause=0,(_tmp1A2->f1).requires_relns=0,(_tmp1A2->f1).ensures_clause=0,(_tmp1A2->f1).ensures_relns=0;_tmp1A2;});
# 895
return({void*_tmp6C0=(void*)_tmp1A1;void*_tmp6BF=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6BE=Cyc_Absyn_empty_tqual(0U);void*_tmp6BD=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp6C0,_tmp6BF,_tmp6BE,_tmp6BD,Cyc_Absyn_false_type);});}
# 902
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp1A3=Cyc_Tcutil_compress(e);void*_tmp1A4=_tmp1A3;struct Cyc_Core_Opt*_tmp1BE;void**_tmp1BD;struct Cyc_Core_Opt*_tmp1BC;void*_tmp1BB;struct Cyc_List_List*_tmp1BA;void*_tmp1B9;switch(*((int*)_tmp1A4)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2 != 0){_LL1: _tmp1B9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->hd;_LL2:
# 909
 if(constrain)return Cyc_Unify_unify(r,_tmp1B9);
_tmp1B9=Cyc_Tcutil_compress(_tmp1B9);
if(r == _tmp1B9)return 1;{
struct _tuple15 _tmp1A5=({struct _tuple15 _tmp5FF;_tmp5FF.f1=r,_tmp5FF.f2=_tmp1B9;_tmp5FF;});struct _tuple15 _tmp1A6=_tmp1A5;struct Cyc_Absyn_Tvar*_tmp1A8;struct Cyc_Absyn_Tvar*_tmp1A7;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6.f2)->tag == 2U){_LLC: _tmp1A8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6.f1)->f1;_tmp1A7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1A6.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp1A8,_tmp1A7)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp1BA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2;_LL4:
# 917
 for(0;_tmp1BA != 0;_tmp1BA=_tmp1BA->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp1BA->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2 != 0){_LL5: _tmp1BB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->hd;_LL6: {
# 921
void*_tmp1A9=Cyc_Tcutil_rgns_of(_tmp1BB);void*_tmp1AA=_tmp1A9;void*_tmp1B3;void*_tmp1B2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f2 != 0){_LL11: _tmp1B2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AA)->f2)->hd;_LL12:
# 923
 if(!constrain)return 0;{
void*_tmp1AB=Cyc_Tcutil_compress(_tmp1B2);void*_tmp1AC=_tmp1AB;struct Cyc_Core_Opt*_tmp1B1;void**_tmp1B0;struct Cyc_Core_Opt*_tmp1AF;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AC)->tag == 1U){_LL16: _tmp1B1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AC)->f1;_tmp1B0=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AC)->f2;_tmp1AF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AC)->f4;_LL17: {
# 928
void*_tmp1AD=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1AF);
# 931
Cyc_Unify_occurs(_tmp1AD,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1AF))->v,r);
({void*_tmp6C2=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1AE[2U];_tmp1AE[0]=_tmp1AD,({void*_tmp6C1=Cyc_Absyn_access_eff(r);_tmp1AE[1]=_tmp6C1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1AE,sizeof(void*),2U));})));*_tmp1B0=_tmp6C2;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1B3=_tmp1AA;_LL14:
# 936
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp1B3);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A4)->f1;_tmp1BD=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A4)->f2;_tmp1BC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A4)->f4;_LL8:
# 939
 if(_tmp1BE == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1BE->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1B4=0U;({struct _fat_ptr _tmp6C3=({const char*_tmp1B5="effect evar has wrong kind";_tag_fat(_tmp1B5,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C3,_tag_fat(_tmp1B4,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 944
void*_tmp1B6=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1BC);
# 947
Cyc_Unify_occurs(_tmp1B6,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1BC))->v,r);{
void*_tmp1B7=Cyc_Absyn_join_eff(({void*_tmp1B8[2U];_tmp1B8[0]=_tmp1B6,({void*_tmp6C4=Cyc_Absyn_access_eff(r);_tmp1B8[1]=_tmp6C4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1B8,sizeof(void*),2U));}));
*_tmp1BD=_tmp1B7;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 958
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1BF=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp1C0=_tmp1BF;struct Cyc_Core_Opt*_tmp1D2;void**_tmp1D1;struct Cyc_Core_Opt*_tmp1D0;void*_tmp1CF;struct Cyc_List_List*_tmp1CE;switch(*((int*)_tmp1C0)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C0)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1CE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C0)->f2;_LL4:
# 963
 for(0;_tmp1CE != 0;_tmp1CE=_tmp1CE->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp1CE->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C0)->f2 != 0){_LL5: _tmp1CF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C0)->f2)->hd;_LL6:
# 968
 _tmp1CF=Cyc_Tcutil_compress(_tmp1CF);
if(t == _tmp1CF)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,_tmp1CF);{
void*_tmp1C1=Cyc_Tcutil_rgns_of(t);void*_tmp1C2=_tmp1C1;void*_tmp1C8;void*_tmp1C7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C2)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C2)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C2)->f2 != 0){_LLC: _tmp1C7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C2)->f2)->hd;_LLD: {
# 973
struct _tuple15 _tmp1C3=({struct _tuple15 _tmp600;({void*_tmp6C5=Cyc_Tcutil_compress(_tmp1C7);_tmp600.f1=_tmp6C5;}),_tmp600.f2=_tmp1CF;_tmp600;});struct _tuple15 _tmp1C4=_tmp1C3;struct Cyc_Absyn_Tvar*_tmp1C6;struct Cyc_Absyn_Tvar*_tmp1C5;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C4.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C4.f2)->tag == 2U){_LL11: _tmp1C6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C4.f1)->f1;_tmp1C5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1C4.f2)->f1;_LL12:
 return Cyc_Unify_unify(t,_tmp1CF);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp1C7 == _tmp1CF;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1C8=_tmp1C2;_LLF:
# 977
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp1C8);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1D2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C0)->f1;_tmp1D1=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C0)->f2;_tmp1D0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C0)->f4;_LL8:
# 980
 if(_tmp1D2 == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1D2->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1C9=0U;({struct _fat_ptr _tmp6C6=({const char*_tmp1CA="effect evar has wrong kind";_tag_fat(_tmp1CA,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C6,_tag_fat(_tmp1C9,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 985
void*_tmp1CB=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1D0);
# 988
Cyc_Unify_occurs(_tmp1CB,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1D0))->v,t);{
void*_tmp1CC=Cyc_Absyn_join_eff(({void*_tmp1CD[2U];_tmp1CD[0]=_tmp1CB,({void*_tmp6C7=Cyc_Absyn_regionsof_eff(t);_tmp1CD[1]=_tmp6C7;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1CD,sizeof(void*),2U));}));
*_tmp1D1=_tmp1CC;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 999
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1D3=e;struct Cyc_Core_Opt*_tmp1E9;void**_tmp1E8;struct Cyc_Core_Opt*_tmp1E7;void*_tmp1E6;struct Cyc_List_List*_tmp1E5;struct Cyc_Absyn_Tvar*_tmp1E4;switch(*((int*)_tmp1D3)){case 2U: _LL1: _tmp1E4=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D3)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp1E4)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f1)){case 9U: _LL3: _tmp1E5=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f2;_LL4:
# 1004
 for(0;_tmp1E5 != 0;_tmp1E5=_tmp1E5->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp1E5->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f2 != 0){_LL5: _tmp1E6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D3)->f2)->hd;_LL6: {
# 1009
void*_tmp1D4=Cyc_Tcutil_rgns_of(_tmp1E6);void*_tmp1D5=_tmp1D4;void*_tmp1DE;void*_tmp1DD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D5)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D5)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D5)->f2 != 0){_LLC: _tmp1DD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D5)->f2)->hd;_LLD:
# 1011
 if(!may_constrain_evars)return 0;{
void*_tmp1D6=Cyc_Tcutil_compress(_tmp1DD);void*_tmp1D7=_tmp1D6;struct Cyc_Core_Opt*_tmp1DC;void**_tmp1DB;struct Cyc_Core_Opt*_tmp1DA;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D7)->tag == 1U){_LL11: _tmp1DC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D7)->f1;_tmp1DB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D7)->f2;_tmp1DA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D7)->f4;_LL12: {
# 1017
void*_tmp1D8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1DA);
# 1019
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1DA))->v,v))return 0;
({void*_tmp6C9=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1D9[2U];_tmp1D9[0]=_tmp1D8,({void*_tmp6C8=Cyc_Absyn_var_type(v);_tmp1D9[1]=_tmp6C8;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D9,sizeof(void*),2U));})));*_tmp1DB=_tmp6C9;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1DE=_tmp1D5;_LLF:
# 1024
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp1DE);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1E9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f1;_tmp1E8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f2;_tmp1E7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D3)->f4;_LL8:
# 1027
 if(_tmp1E9 == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp1E9->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1DF=0U;({struct _fat_ptr _tmp6CA=({const char*_tmp1E0="effect evar has wrong kind";_tag_fat(_tmp1E0,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CA,_tag_fat(_tmp1DF,sizeof(void*),0U));});});{
# 1031
void*_tmp1E1=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp1E7);
# 1033
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp1E7))->v,v))
return 0;{
void*_tmp1E2=Cyc_Absyn_join_eff(({void*_tmp1E3[2U];_tmp1E3[0]=_tmp1E1,({void*_tmp6CB=Cyc_Absyn_var_type(v);_tmp1E3[1]=_tmp6CB;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E3,sizeof(void*),2U));}));
*_tmp1E8=_tmp1E2;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1043
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1EA=e;void*_tmp1F0;struct Cyc_List_List*_tmp1EF;switch(*((int*)_tmp1EA)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f1)){case 9U: _LL1: _tmp1EF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f2;_LL2:
# 1047
 for(0;_tmp1EF != 0;_tmp1EF=_tmp1EF->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp1EF->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f2 != 0){_LL3: _tmp1F0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EA)->f2)->hd;_LL4: {
# 1052
void*_tmp1EB=Cyc_Tcutil_rgns_of(_tmp1F0);void*_tmp1EC=_tmp1EB;void*_tmp1EE;void*_tmp1ED;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f2 != 0){_LLA: _tmp1ED=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EC)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1EE=_tmp1EC;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp1EE);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1056
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1070 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1075
void*_tmp1F1=Cyc_Tcutil_compress(e1);void*_tmp1F2=_tmp1F1;void**_tmp1FF;struct Cyc_Core_Opt*_tmp1FE;struct Cyc_Absyn_Tvar*_tmp1FD;void*_tmp1FC;void*_tmp1FB;struct Cyc_List_List*_tmp1FA;switch(*((int*)_tmp1F2)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f1)){case 9U: _LL1: _tmp1FA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2;_LL2:
# 1077
 for(0;_tmp1FA != 0;_tmp1FA=_tmp1FA->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp1FA->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2 != 0){_LL3: _tmp1FB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2)->hd;_LL4:
# 1088
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp1FB,e2)||
 may_constrain_evars && Cyc_Unify_unify(_tmp1FB,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2 != 0){_LL7: _tmp1FC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F2)->f2)->hd;_LL8: {
# 1092
void*_tmp1F3=Cyc_Tcutil_rgns_of(_tmp1FC);void*_tmp1F4=_tmp1F3;void*_tmp1F6;void*_tmp1F5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F4)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F4)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F4)->f2 != 0){_LLE: _tmp1F5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F4)->f2)->hd;_LLF:
# 1097
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp1F5,e2)||
 may_constrain_evars && Cyc_Unify_unify(_tmp1F5,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1F6=_tmp1F4;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp1F6,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1FD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F2)->f1;_LL6:
# 1090
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp1FD,e2);case 1U: _LL9: _tmp1FF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F2)->f2;_tmp1FE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F2)->f4;_LLA:
# 1102
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1106
*_tmp1FF=Cyc_Absyn_empty_effect;
# 1109
return 1;}else{
# 1111
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp1F9=({struct Cyc_String_pa_PrintArg_struct _tmp601;_tmp601.tag=0U,({struct _fat_ptr _tmp6CC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp601.f1=_tmp6CC;});_tmp601;});void*_tmp1F7[1U];_tmp1F7[0]=& _tmp1F9;({struct _fat_ptr _tmp6CD=({const char*_tmp1F8="subset_effect: bad effect: %s";_tag_fat(_tmp1F8,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CD,_tag_fat(_tmp1F7,sizeof(void*),1U));});});}_LL0:;}
# 1122
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1124
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp200=(struct _tuple15*)r1->hd;struct _tuple15*_tmp201=_tmp200;void*_tmp207;void*_tmp206;_LL1: _tmp207=_tmp201->f1;_tmp206=_tmp201->f2;_LL2:;{
int found=_tmp207 == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp202=(struct _tuple15*)r2->hd;struct _tuple15*_tmp203=_tmp202;void*_tmp205;void*_tmp204;_LL4: _tmp205=_tmp203->f1;_tmp204=_tmp203->f2;_LL5:;
if(Cyc_Unify_unify(_tmp207,_tmp205)&& Cyc_Unify_unify(_tmp206,_tmp204)){
found=1;
break;}}}
# 1134
if(!found)return 0;};}}
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
 return 11;default: _LL17: _tmp20C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20B)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp20C);}_LL0:;}
# 1205
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp20D=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp20D != 0)return _tmp20D;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1210
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp20E=i;struct _tuple1*_tmp210;struct Cyc_Absyn_Datatypedecl*_tmp20F;if((_tmp20E.KnownDatatype).tag == 2){_LL1: _tmp20F=*(_tmp20E.KnownDatatype).val;_LL2:
 return _tmp20F->name;}else{_LL3: _tmp210=((_tmp20E.UnknownDatatype).val).name;_LL4:
 return _tmp210;}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1216
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp211=i;struct _tuple1*_tmp215;struct _tuple1*_tmp214;struct Cyc_Absyn_Datatypedecl*_tmp213;struct Cyc_Absyn_Datatypefield*_tmp212;if((_tmp211.KnownDatatypefield).tag == 2){_LL1: _tmp213=((_tmp211.KnownDatatypefield).val).f1;_tmp212=((_tmp211.KnownDatatypefield).val).f2;_LL2:
# 1219
 return({struct _tuple20 _tmp602;_tmp602.f1=_tmp213->name,_tmp602.f2=_tmp212->name;_tmp602;});}else{_LL3: _tmp215=((_tmp211.UnknownDatatypefield).val).datatype_name;_tmp214=((_tmp211.UnknownDatatypefield).val).field_name;_LL4:
# 1221
 return({struct _tuple20 _tmp603;_tmp603.f1=_tmp215,_tmp603.f2=_tmp214;_tmp603;});}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1224
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp216=i;struct Cyc_Absyn_Aggrdecl*_tmp219;enum Cyc_Absyn_AggrKind _tmp218;struct _tuple1*_tmp217;if((_tmp216.UnknownAggr).tag == 1){_LL1: _tmp218=((_tmp216.UnknownAggr).val).f1;_tmp217=((_tmp216.UnknownAggr).val).f2;_LL2:
 return({struct _tuple21 _tmp604;_tmp604.f1=_tmp218,_tmp604.f2=_tmp217;_tmp604;});}else{_LL3: _tmp219=*(_tmp216.KnownAggr).val;_LL4:
 return({struct _tuple21 _tmp605;_tmp605.f1=_tmp219->kind,_tmp605.f2=_tmp219->name;_tmp605;});}_LL0:;}
# 1230
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1236
struct _tuple15 _tmp21A=({struct _tuple15 _tmp606;_tmp606.f1=t1,_tmp606.f2=t2;_tmp606;});struct _tuple15 _tmp21B=_tmp21A;union Cyc_Absyn_AggrInfo _tmp239;union Cyc_Absyn_AggrInfo _tmp238;union Cyc_Absyn_DatatypeFieldInfo _tmp237;union Cyc_Absyn_DatatypeFieldInfo _tmp236;union Cyc_Absyn_DatatypeInfo _tmp235;union Cyc_Absyn_DatatypeInfo _tmp234;struct Cyc_List_List*_tmp233;struct Cyc_List_List*_tmp232;struct _fat_ptr _tmp231;struct _fat_ptr _tmp230;struct _tuple1*_tmp22F;struct _tuple1*_tmp22E;switch(*((int*)_tmp21B.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 15U){_LL1: _tmp22F=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp22E=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp22F,_tmp22E);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 17U){_LL3: _tmp231=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp230=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LL4:
 return Cyc_strcmp((struct _fat_ptr)_tmp231,(struct _fat_ptr)_tmp230);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 16U){_LL5: _tmp233=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp232=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LL6:
# 1240
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp233,_tmp232);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 18U){_LL7: _tmp235=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp234=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LL8: {
# 1242
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(_tmp235);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(_tmp234);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 19U){_LL9: _tmp237=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp236=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LLA: {
# 1246
struct _tuple20 _tmp21C=Cyc_Tcutil_get_datatype_field_qvars(_tmp237);struct _tuple20 _tmp21D=_tmp21C;struct _tuple1*_tmp224;struct _tuple1*_tmp223;_LL10: _tmp224=_tmp21D.f1;_tmp223=_tmp21D.f2;_LL11:;{
struct _tuple20 _tmp21E=Cyc_Tcutil_get_datatype_field_qvars(_tmp236);struct _tuple20 _tmp21F=_tmp21E;struct _tuple1*_tmp222;struct _tuple1*_tmp221;_LL13: _tmp222=_tmp21F.f1;_tmp221=_tmp21F.f2;_LL14:;{
int _tmp220=Cyc_Absyn_qvar_cmp(_tmp224,_tmp222);
if(_tmp220 != 0)return _tmp220;
return Cyc_Absyn_qvar_cmp(_tmp223,_tmp221);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21B.f2)->tag == 20U){_LLB: _tmp239=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21B.f1)->f1;_tmp238=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21B.f2)->f1;_LLC: {
# 1252
struct _tuple21 _tmp225=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp239);struct _tuple21 _tmp226=_tmp225;enum Cyc_Absyn_AggrKind _tmp22D;struct _tuple1*_tmp22C;_LL16: _tmp22D=_tmp226.f1;_tmp22C=_tmp226.f2;_LL17:;{
struct _tuple21 _tmp227=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp238);struct _tuple21 _tmp228=_tmp227;enum Cyc_Absyn_AggrKind _tmp22B;struct _tuple1*_tmp22A;_LL19: _tmp22B=_tmp228.f1;_tmp22A=_tmp228.f2;_LL1A:;{
int _tmp229=Cyc_Absyn_qvar_cmp(_tmp22C,_tmp22A);
if(_tmp229 != 0)return _tmp229;
return(int)_tmp22D - (int)_tmp22B;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}
# 1261
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6D0)(void*,void*)=cmp;void*_tmp6CF=(void*)_check_null(a1);_tmp6D0(_tmp6CF,(void*)_check_null(a2));});}
# 1267
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp23A=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp23B=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp23A,_tmp23B);}
# 1272
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp23C=tqt1;struct Cyc_Absyn_Tqual _tmp242;void*_tmp241;_LL1: _tmp242=_tmp23C->f1;_tmp241=_tmp23C->f2;_LL2:;{
struct _tuple12*_tmp23D=tqt2;struct Cyc_Absyn_Tqual _tmp240;void*_tmp23F;_LL4: _tmp240=_tmp23D->f1;_tmp23F=_tmp23D->f2;_LL5:;{
int _tmp23E=Cyc_Tcutil_tqual_cmp(_tmp242,_tmp240);
if(_tmp23E != 0)return _tmp23E;
return Cyc_Tcutil_typecmp(_tmp241,_tmp23F);};};}
# 1280
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp243=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp243 != 0)return _tmp243;{
int _tmp244=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp244 != 0)return _tmp244;{
int _tmp245=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp245 != 0)return _tmp245;{
int _tmp246=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp246 != 0)return _tmp246;
_tmp246=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp246 != 0)return _tmp246;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1297
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp247=({int _tmp6D1=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6D1,Cyc_Tcutil_type_case_number(t2));});
if(_tmp247 != 0)
return _tmp247;{
# 1306
struct _tuple15 _tmp248=({struct _tuple15 _tmp607;_tmp607.f1=t2,_tmp607.f2=t1;_tmp607;});struct _tuple15 _tmp249=_tmp248;struct Cyc_Absyn_Exp*_tmp295;struct Cyc_Absyn_Exp*_tmp294;struct Cyc_Absyn_Exp*_tmp293;struct Cyc_Absyn_Exp*_tmp292;enum Cyc_Absyn_AggrKind _tmp291;struct Cyc_List_List*_tmp290;enum Cyc_Absyn_AggrKind _tmp28F;struct Cyc_List_List*_tmp28E;struct Cyc_List_List*_tmp28D;struct Cyc_List_List*_tmp28C;struct Cyc_Absyn_FnInfo _tmp28B;struct Cyc_Absyn_FnInfo _tmp28A;void*_tmp289;struct Cyc_Absyn_Tqual _tmp288;struct Cyc_Absyn_Exp*_tmp287;void*_tmp286;void*_tmp285;struct Cyc_Absyn_Tqual _tmp284;struct Cyc_Absyn_Exp*_tmp283;void*_tmp282;void*_tmp281;struct Cyc_Absyn_Tqual _tmp280;void*_tmp27F;void*_tmp27E;void*_tmp27D;void*_tmp27C;void*_tmp27B;struct Cyc_Absyn_Tqual _tmp27A;void*_tmp279;void*_tmp278;void*_tmp277;void*_tmp276;struct Cyc_Absyn_Tvar*_tmp275;struct Cyc_Absyn_Tvar*_tmp274;void*_tmp273;struct Cyc_List_List*_tmp272;void*_tmp271;struct Cyc_List_List*_tmp270;switch(*((int*)_tmp249.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f2)->tag == 0U){_LL1: _tmp273=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp272=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f1)->f2;_tmp271=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f2)->f1;_tmp270=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp249.f2)->f2;_LL2: {
# 1308
int _tmp24A=Cyc_Tcutil_tycon_cmp(_tmp273,_tmp271);
if(_tmp24A != 0)return _tmp24A;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp272,_tmp270);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp249.f2)->tag == 1U){_LL3: _LL4:
# 1312
({void*_tmp24B=0U;({struct _fat_ptr _tmp6D2=({const char*_tmp24C="typecmp: can only compare closed types";_tag_fat(_tmp24C,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D2,_tag_fat(_tmp24B,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp249.f2)->tag == 2U){_LL5: _tmp275=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp274=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp249.f2)->f1;_LL6:
# 1316
 return Cyc_Core_intcmp(_tmp274->identity,_tmp275->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->tag == 3U){_LL7: _tmp281=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).elt_type;_tmp280=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).elt_tq;_tmp27F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).ptr_atts).rgn;_tmp27E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).ptr_atts).nullable;_tmp27D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).ptr_atts).bounds;_tmp27C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f1)->f1).ptr_atts).zero_term;_tmp27B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).elt_type;_tmp27A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).elt_tq;_tmp279=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).ptr_atts).rgn;_tmp278=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).ptr_atts).nullable;_tmp277=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).ptr_atts).bounds;_tmp276=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp249.f2)->f1).ptr_atts).zero_term;_LL8: {
# 1320
int _tmp24D=Cyc_Tcutil_typecmp(_tmp27B,_tmp281);
if(_tmp24D != 0)return _tmp24D;{
int _tmp24E=Cyc_Tcutil_typecmp(_tmp279,_tmp27F);
if(_tmp24E != 0)return _tmp24E;{
int _tmp24F=Cyc_Tcutil_tqual_cmp(_tmp27A,_tmp280);
if(_tmp24F != 0)return _tmp24F;{
int _tmp250=Cyc_Tcutil_typecmp(_tmp277,_tmp27D);
if(_tmp250 != 0)return _tmp250;{
int _tmp251=Cyc_Tcutil_typecmp(_tmp276,_tmp27C);
if(_tmp251 != 0)return _tmp251;{
int _tmp252=Cyc_Tcutil_typecmp(_tmp277,_tmp27D);
if(_tmp252 != 0)return _tmp252;
return Cyc_Tcutil_typecmp(_tmp278,_tmp27E);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->tag == 4U){_LL9: _tmp289=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f1)->f1).elt_type;_tmp288=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f1)->f1).tq;_tmp287=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f1)->f1).num_elts;_tmp286=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f1)->f1).zero_term;_tmp285=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->f1).elt_type;_tmp284=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->f1).tq;_tmp283=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->f1).num_elts;_tmp282=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp249.f2)->f1).zero_term;_LLA: {
# 1336
int _tmp253=Cyc_Tcutil_tqual_cmp(_tmp284,_tmp288);
if(_tmp253 != 0)return _tmp253;{
int _tmp254=Cyc_Tcutil_typecmp(_tmp285,_tmp289);
if(_tmp254 != 0)return _tmp254;{
int _tmp255=Cyc_Tcutil_typecmp(_tmp286,_tmp282);
if(_tmp255 != 0)return _tmp255;
if(_tmp287 == _tmp283)return 0;
if(_tmp287 == 0 || _tmp283 == 0)
({void*_tmp256=0U;({struct _fat_ptr _tmp6D3=({const char*_tmp257="missing expression in array index";_tag_fat(_tmp257,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D3,_tag_fat(_tmp256,sizeof(void*),0U));});});
# 1346
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp287,_tmp283);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp249.f2)->tag == 5U){_LLB: _tmp28B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp28A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp249.f2)->f1;_LLC:
# 1349
 if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp28B.ret_type,_tmp28A.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(_tmp28B.ret_tqual,_tmp28A.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp258=_tmp28B.args;
struct Cyc_List_List*_tmp259=_tmp28A.args;
for(0;_tmp258 != 0 && _tmp259 != 0;(_tmp258=_tmp258->tl,_tmp259=_tmp259->tl)){
struct _tuple9 _tmp25A=*((struct _tuple9*)_tmp258->hd);struct _tuple9 _tmp25B=_tmp25A;struct Cyc_Absyn_Tqual _tmp261;void*_tmp260;_LL18: _tmp261=_tmp25B.f2;_tmp260=_tmp25B.f3;_LL19:;{
struct _tuple9 _tmp25C=*((struct _tuple9*)_tmp259->hd);struct _tuple9 _tmp25D=_tmp25C;struct Cyc_Absyn_Tqual _tmp25F;void*_tmp25E;_LL1B: _tmp25F=_tmp25D.f2;_tmp25E=_tmp25D.f3;_LL1C:;
r=Cyc_Tcutil_tqual_cmp(_tmp261,_tmp25F);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp260,_tmp25E);
if(r != 0)return r;};}
# 1364
if(_tmp258 != 0)return 1;
if(_tmp259 != 0)return - 1;
if(_tmp28B.c_varargs && !_tmp28A.c_varargs)return 1;
if(!_tmp28B.c_varargs && _tmp28A.c_varargs)return - 1;
if(_tmp28B.cyc_varargs != 0 & _tmp28A.cyc_varargs == 0)return 1;
if(_tmp28B.cyc_varargs == 0 & _tmp28A.cyc_varargs != 0)return - 1;
if(_tmp28B.cyc_varargs != 0 & _tmp28A.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp6D4=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp28B.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp6D4,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp28A.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((_tmp28B.cyc_varargs)->type,(_tmp28A.cyc_varargs)->type);
if(r != 0)return r;
if((_tmp28B.cyc_varargs)->inject && !(_tmp28A.cyc_varargs)->inject)return 1;
if(!(_tmp28B.cyc_varargs)->inject &&(_tmp28A.cyc_varargs)->inject)return - 1;}
# 1378
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp28B.effect,_tmp28A.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp262=_tmp28B.rgn_po;
struct Cyc_List_List*_tmp263=_tmp28A.rgn_po;
for(0;_tmp262 != 0 && _tmp263 != 0;(_tmp262=_tmp262->tl,_tmp263=_tmp263->tl)){
struct _tuple15 _tmp264=*((struct _tuple15*)_tmp262->hd);struct _tuple15 _tmp265=_tmp264;void*_tmp26B;void*_tmp26A;_LL1E: _tmp26B=_tmp265.f1;_tmp26A=_tmp265.f2;_LL1F:;{
struct _tuple15 _tmp266=*((struct _tuple15*)_tmp263->hd);struct _tuple15 _tmp267=_tmp266;void*_tmp269;void*_tmp268;_LL21: _tmp269=_tmp267.f1;_tmp268=_tmp267.f2;_LL22:;
r=Cyc_Tcutil_typecmp(_tmp26B,_tmp269);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp26A,_tmp268);if(r != 0)return r;};}
# 1388
if(_tmp262 != 0)return 1;
if(_tmp263 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp28B.requires_clause,_tmp28A.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp28B.ensures_clause,_tmp28A.ensures_clause);
if(r != 0)return r;
# 1396
({void*_tmp26C=0U;({struct _fat_ptr _tmp6D5=({const char*_tmp26D="typecmp: function type comparison should never get here!";_tag_fat(_tmp26D,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D5,_tag_fat(_tmp26C,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp249.f2)->tag == 6U){_LLD: _tmp28D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp28C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp249.f2)->f1;_LLE:
# 1399
 return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp28C,_tmp28D);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f2)->tag == 7U){_LLF: _tmp291=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp290=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f1)->f2;_tmp28F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f2)->f1;_tmp28E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp249.f2)->f2;_LL10:
# 1402
 if((int)_tmp28F != (int)_tmp291){
if((int)_tmp28F == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp28E,_tmp290);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp249.f2)->tag == 9U){_LL11: _tmp293=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp292=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp249.f2)->f1;_LL12:
# 1407
 _tmp295=_tmp293;_tmp294=_tmp292;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp249.f2)->tag == 11U){_LL13: _tmp295=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp249.f1)->f1;_tmp294=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp249.f2)->f1;_LL14:
# 1409
 return Cyc_Evexp_const_exp_cmp(_tmp295,_tmp294);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp26E=0U;({struct _fat_ptr _tmp6D6=({const char*_tmp26F="Unmatched case in typecmp";_tag_fat(_tmp26F,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D6,_tag_fat(_tmp26E,sizeof(void*),0U));});});}_LL0:;};};}
# 1416
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp296=({struct _tuple15 _tmp609;({void*_tmp6D8=Cyc_Tcutil_compress(t1);_tmp609.f1=_tmp6D8;}),({void*_tmp6D7=Cyc_Tcutil_compress(t2);_tmp609.f2=_tmp6D7;});_tmp609;});struct _tuple15 _tmp297=_tmp296;void*_tmp29D;void*_tmp29C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f2)->tag == 0U){_LL1: _tmp29D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f1)->f1;_tmp29C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp297.f2)->f1;_LL2: {
# 1419
struct _tuple15 _tmp298=({struct _tuple15 _tmp608;_tmp608.f1=_tmp29D,_tmp608.f2=_tmp29C;_tmp608;});struct _tuple15 _tmp299=_tmp298;int _tmp29B;int _tmp29A;switch(*((int*)_tmp299.f1)){case 2U: switch(*((int*)_tmp299.f2)){case 2U: _LL6: _tmp29B=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp299.f1)->f1;_tmp29A=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp299.f2)->f1;_LL7:
 return _tmp29A < _tmp29B;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp299.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1427
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1432
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1435
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp299.f2)->f1 == 0){_LL14: _LL15:
# 1430
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp299.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1428
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1433
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1436
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp299.f2)->f1 == 0){_LL16: _LL17:
# 1431
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1437
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp299.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
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
{struct _tuple15 _tmp29E=({struct _tuple15 _tmp60B;({void*_tmp6DA=Cyc_Tcutil_compress(t1);_tmp60B.f1=_tmp6DA;}),({void*_tmp6D9=Cyc_Tcutil_compress(t2);_tmp60B.f2=_tmp6D9;});_tmp60B;});struct _tuple15 _tmp29F=_tmp29E;void*_tmp2A5;void*_tmp2A4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f2)->tag == 0U){_LL1: _tmp2A5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f1)->f1;_tmp2A4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29F.f2)->f1;_LL2:
# 1449
{struct _tuple15 _tmp2A0=({struct _tuple15 _tmp60A;_tmp60A.f1=_tmp2A5,_tmp60A.f2=_tmp2A4;_tmp60A;});struct _tuple15 _tmp2A1=_tmp2A0;int _tmp2A3;int _tmp2A2;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A1.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A1.f2)->tag == 2U){_LL6: _tmp2A3=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f1;_tmp2A2=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f1;_LL7:
# 1451
 if(_tmp2A3 != 0 && _tmp2A3 != 1)return t1;else{
if(_tmp2A2 != 0 && _tmp2A2 != 1)return t2;else{
if(_tmp2A3 >= _tmp2A2)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A1.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1461
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A1.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1466
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1459
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A1.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1468
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1458
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1462
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A1.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1467
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1460
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A1.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A1.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1469
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A1.f1)->tag == 4U){_LL18: _LL19:
# 1464
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A1.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1470
 goto _LL5;}}}}}}_LL5:;}
# 1472
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
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
void*_tmp2AB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp2AC=_tmp2AB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AC)->tag == 3U){_LL1: _LL2:
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
void*_tmp2B1=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp2B2=_tmp2B1;struct Cyc_Absyn_Exp*_tmp2B4;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B2)->tag == 9U){_LL1: _tmp2B4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B2)->f1;_LL2:
 return _tmp2B4;}else{_LL3: _LL4: {
# 1562
struct Cyc_Absyn_Exp*_tmp2B3=Cyc_Absyn_valueof_exp(_tmp2B1,0U);
_tmp2B3->topt=Cyc_Absyn_uint_type;
return _tmp2B3;}}_LL0:;}
# 1571
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B5=Cyc_Tcutil_compress(b);void*_tmp2B6=_tmp2B5;struct Cyc_List_List*_tmp2BA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B6)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B6)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2BA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B6)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp2BA);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp2B9=({struct Cyc_String_pa_PrintArg_struct _tmp60E;_tmp60E.tag=0U,({struct _fat_ptr _tmp6E4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp60E.f1=_tmp6E4;});_tmp60E;});void*_tmp2B7[1U];_tmp2B7[0]=& _tmp2B9;({struct _fat_ptr _tmp6E5=({const char*_tmp2B8="get_bounds_exp: %s";_tag_fat(_tmp2B8,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E5,_tag_fat(_tmp2B7,sizeof(void*),1U));});});}_LL0:;};}
# 1580
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2BB=Cyc_Tcutil_compress(t);void*_tmp2BC=_tmp2BB;void*_tmp2C0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BC)->tag == 3U){_LL1: _tmp2C0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BC)->f1).ptr_atts).bounds;_LL2:
# 1583
 return Cyc_Tcutil_get_bounds_exp(def,_tmp2C0);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp60F;_tmp60F.tag=0U,({struct _fat_ptr _tmp6E6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp60F.f1=_tmp6E6;});_tmp60F;});void*_tmp2BD[1U];_tmp2BD[0]=& _tmp2BF;({struct _fat_ptr _tmp6E7=({const char*_tmp2BE="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp2BE,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6E7,_tag_fat(_tmp2BD,sizeof(void*),1U));});});}_LL0:;}
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
struct _tuple15 _tmp2C3=({struct _tuple15 _tmp613;_tmp613.f1=t1,_tmp613.f2=t2;_tmp613;});struct _tuple15 _tmp2C4=_tmp2C3;void*_tmp2D9;struct Cyc_Absyn_Tqual _tmp2D8;struct Cyc_Absyn_Exp*_tmp2D7;void*_tmp2D6;void*_tmp2D5;struct Cyc_Absyn_Tqual _tmp2D4;struct Cyc_Absyn_Exp*_tmp2D3;void*_tmp2D2;struct Cyc_Absyn_PtrInfo _tmp2D1;struct Cyc_Absyn_PtrInfo _tmp2D0;switch(*((int*)_tmp2C4.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4.f2)->tag == 3U){_LL1: _tmp2D1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4.f1)->f1;_tmp2D0=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C4.f2)->f1;_LL2: {
# 1611
int okay=1;
# 1613
if(!Cyc_Unify_unify((_tmp2D1.ptr_atts).nullable,(_tmp2D0.ptr_atts).nullable))
# 1615
okay=!Cyc_Tcutil_force_type2bool(0,(_tmp2D1.ptr_atts).nullable);
# 1617
if(!Cyc_Unify_unify((_tmp2D1.ptr_atts).bounds,(_tmp2D0.ptr_atts).bounds)){
struct _tuple22 _tmp2C5=({struct _tuple22 _tmp610;({struct Cyc_Absyn_Exp*_tmp6EB=({void*_tmp6EA=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6EA,(_tmp2D1.ptr_atts).bounds);});_tmp610.f1=_tmp6EB;}),({
struct Cyc_Absyn_Exp*_tmp6E9=({void*_tmp6E8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6E8,(_tmp2D0.ptr_atts).bounds);});_tmp610.f2=_tmp6E9;});_tmp610;});
# 1618
struct _tuple22 _tmp2C6=_tmp2C5;struct Cyc_Absyn_Exp*_tmp2CA;struct Cyc_Absyn_Exp*_tmp2C9;if(_tmp2C6.f2 == 0){_LLA: _LLB:
# 1621
 okay=1;goto _LL9;}else{if(_tmp2C6.f1 == 0){_LLC: _LLD:
# 1624
 if(Cyc_Tcutil_force_type2bool(0,(_tmp2D1.ptr_atts).zero_term)&&({
void*_tmp6EC=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6EC,(_tmp2D0.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp2CA=_tmp2C6.f1;_tmp2C9=_tmp2C6.f2;_LLF:
# 1630
 okay=okay &&({struct Cyc_Absyn_Exp*_tmp6ED=(struct Cyc_Absyn_Exp*)_check_null(_tmp2C9);Cyc_Evexp_lte_const_exp(_tmp6ED,(struct Cyc_Absyn_Exp*)_check_null(_tmp2CA));});
# 1634
if(!Cyc_Tcutil_force_type2bool(0,(_tmp2D0.ptr_atts).zero_term))
({void*_tmp2C7=0U;({unsigned _tmp6EF=loc;struct _fat_ptr _tmp6EE=({const char*_tmp2C8="implicit cast to shorter array";_tag_fat(_tmp2C8,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp6EF,_tmp6EE,_tag_fat(_tmp2C7,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 1641
okay=okay &&(!(_tmp2D1.elt_tq).real_const ||(_tmp2D0.elt_tq).real_const);
# 1644
if(!Cyc_Unify_unify((_tmp2D1.ptr_atts).rgn,(_tmp2D0.ptr_atts).rgn)){
if(Cyc_RgnOrder_rgn_outlives_rgn(po,(_tmp2D1.ptr_atts).rgn,(_tmp2D0.ptr_atts).rgn)){
# 1647
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp2CD=({struct Cyc_String_pa_PrintArg_struct _tmp612;_tmp612.tag=0U,({
struct _fat_ptr _tmp6F0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((_tmp2D1.ptr_atts).rgn));_tmp612.f1=_tmp6F0;});_tmp612;});struct Cyc_String_pa_PrintArg_struct _tmp2CE=({struct Cyc_String_pa_PrintArg_struct _tmp611;_tmp611.tag=0U,({
struct _fat_ptr _tmp6F1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((_tmp2D0.ptr_atts).rgn));_tmp611.f1=_tmp6F1;});_tmp611;});void*_tmp2CB[2U];_tmp2CB[0]=& _tmp2CD,_tmp2CB[1]=& _tmp2CE;({unsigned _tmp6F3=loc;struct _fat_ptr _tmp6F2=({const char*_tmp2CC="implicit cast from region %s to region %s";_tag_fat(_tmp2CC,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6F3,_tmp6F2,_tag_fat(_tmp2CB,sizeof(void*),2U));});});}else{
okay=0;}}
# 1655
okay=okay &&(Cyc_Unify_unify((_tmp2D1.ptr_atts).zero_term,(_tmp2D0.ptr_atts).zero_term)||
# 1657
 Cyc_Tcutil_force_type2bool(1,(_tmp2D1.ptr_atts).zero_term)&&(_tmp2D0.elt_tq).real_const);{
# 1665
int _tmp2CF=
({void*_tmp6F4=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6F4,(_tmp2D0.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(_tmp2D0.ptr_atts).zero_term);
# 1671
okay=okay &&(Cyc_Unify_unify(_tmp2D1.elt_type,_tmp2D0.elt_type)||
(_tmp2CF &&((_tmp2D0.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp2D0.elt_type))))&& Cyc_Tcutil_ptrsubtype(po,0,_tmp2D1.elt_type,_tmp2D0.elt_type));
# 1674
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->tag == 4U){_LL3: _tmp2D9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f1)->f1).elt_type;_tmp2D8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f1)->f1).tq;_tmp2D7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f1)->f1).num_elts;_tmp2D6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f1)->f1).zero_term;_tmp2D5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->f1).elt_type;_tmp2D4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->f1).tq;_tmp2D3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->f1).num_elts;_tmp2D2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C4.f2)->f1).zero_term;_LL4: {
# 1678
int okay;
# 1681
okay=Cyc_Unify_unify(_tmp2D6,_tmp2D2)&&(
(_tmp2D7 != 0 && _tmp2D3 != 0)&& Cyc_Evexp_same_const_exp(_tmp2D7,_tmp2D3));
# 1684
return(okay && Cyc_Unify_unify(_tmp2D9,_tmp2D5))&&(!_tmp2D8.real_const || _tmp2D4.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C4.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C4.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C4.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1686
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1688
 return Cyc_Unify_unify(t1,t2);}_LL0:;};}
# 1692
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2DA=Cyc_Tcutil_compress(t);void*_tmp2DB=_tmp2DA;void*_tmp2DE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->tag == 3U){_LL1: _tmp2DE=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DB)->f1).elt_type;_LL2:
 return _tmp2DE;}else{_LL3: _LL4:
({void*_tmp2DC=0U;({struct _fat_ptr _tmp6F5=({const char*_tmp2DD="pointer_elt_type";_tag_fat(_tmp2DD,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F5,_tag_fat(_tmp2DC,sizeof(void*),0U));});});}_LL0:;}
# 1698
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2DF=Cyc_Tcutil_compress(t);void*_tmp2E0=_tmp2DF;struct Cyc_Absyn_PtrAtts*_tmp2E3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E0)->tag == 3U){_LL1: _tmp2E3=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E0)->f1).ptr_atts;_LL2:
 return _tmp2E3->rgn;}else{_LL3: _LL4:
({void*_tmp2E1=0U;({struct _fat_ptr _tmp6F6=({const char*_tmp2E2="pointer_elt_type";_tag_fat(_tmp2E2,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F6,_tag_fat(_tmp2E1,sizeof(void*),0U));});});}_LL0:;}
# 1705
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2E4=Cyc_Tcutil_compress(t);void*_tmp2E5=_tmp2E4;void*_tmp2E6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->tag == 3U){_LL1: _tmp2E6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E5)->f1).ptr_atts).rgn;_LL2:
# 1708
*rgn=_tmp2E6;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1717
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2E7=Cyc_Tcutil_compress(t);void*_tmp2E8=_tmp2E7;void*_tmp2EA;void*_tmp2E9;switch(*((int*)_tmp2E8)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E8)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->f1).ptr_atts).nullable;_tmp2E9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E8)->f1).ptr_atts).bounds;_LL6:
# 1725
 return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,_tmp2E9)&& Cyc_Tcutil_force_type2bool(0,_tmp2EA);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1731
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2EB=e->r;void*_tmp2EC=_tmp2EB;void*_tmp2F0;struct Cyc_Absyn_Exp*_tmp2EF;struct _fat_ptr _tmp2EE;switch(*((int*)_tmp2EC)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2EE=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1).Wchar_c).val;_LLC: {
# 1740
unsigned long _tmp2ED=Cyc_strlen((struct _fat_ptr)_tmp2EE);
int i=0;
if(_tmp2ED >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(_tmp2EE,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(_tmp2EE,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)_tmp2EE.curr)[1]== (int)'x' && _tmp2ED >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(_tmp2EE,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp2ED;++ i){
if((int)*((const char*)_check_fat_subscript(_tmp2EE,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 1750
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1738
 return 1;case 14U: _LLD: _tmp2F0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EC)->f1;_tmp2EF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2EC)->f2;_LLE:
# 1751
 return Cyc_Tcutil_is_zero(_tmp2EF)&& Cyc_Tcutil_admits_zero(_tmp2F0);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1756
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1765
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1770
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1775
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1784
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1789
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1794
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2F1=ka;enum Cyc_Absyn_KindQual _tmp2FA;enum Cyc_Absyn_AliasQual _tmp2F9;_LL1: _tmp2FA=_tmp2F1->kind;_tmp2F9=_tmp2F1->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp2F2=_tmp2F9;switch(_tmp2F2){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1798
enum Cyc_Absyn_KindQual _tmp2F3=_tmp2FA;switch(_tmp2F3){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1809
{enum Cyc_Absyn_KindQual _tmp2F4=_tmp2FA;switch(_tmp2F4){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1816
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1818
{enum Cyc_Absyn_KindQual _tmp2F5=_tmp2FA;switch(_tmp2F5){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1825
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1828
({struct Cyc_String_pa_PrintArg_struct _tmp2F8=({struct Cyc_String_pa_PrintArg_struct _tmp614;_tmp614.tag=0U,({struct _fat_ptr _tmp6F7=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp614.f1=_tmp6F7;});_tmp614;});void*_tmp2F6[1U];_tmp2F6[0]=& _tmp2F8;({struct _fat_ptr _tmp6F8=({const char*_tmp2F7="kind_to_opt: bad kind %s\n";_tag_fat(_tmp2F7,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F8,_tag_fat(_tmp2F6,sizeof(void*),1U));});});}
# 1831
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2FB=_cycalloc(sizeof(*_tmp2FB));_tmp2FB->tag=0U,_tmp2FB->f1=k;_tmp2FB;});}
# 1834
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));({void*_tmp6F9=Cyc_Tcutil_kind_to_bound(k);_tmp2FC->v=_tmp6F9;});_tmp2FC;});}
# 1840
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp2FD=Cyc_Tcutil_compress(t2);void*_tmp2FE=_tmp2FD;void*_tmp2FF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->tag == 3U){_LL1: _tmp2FF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FE)->f1).ptr_atts).nullable;_LL2:
# 1845
 if(!Cyc_Tcutil_force_type2bool(1,_tmp2FF))
return 0;
({void*_tmp6FA=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp6FA;});
e1->topt=t2;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 1854
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1860
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1864
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp313=_cycalloc(sizeof(*_tmp313));_tmp313->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp6FD=({struct _fat_ptr*_tmp312=_cycalloc(sizeof(*_tmp312));({struct _fat_ptr _tmp6FC=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp311=({struct Cyc_Int_pa_PrintArg_struct _tmp616;_tmp616.tag=1U,_tmp616.f1=(unsigned long)counter ++;_tmp616;});void*_tmp30F[1U];_tmp30F[0]=& _tmp311;({struct _fat_ptr _tmp6FB=({const char*_tmp310="__aliasvar%d";_tag_fat(_tmp310,sizeof(char),13U);});Cyc_aprintf(_tmp6FB,_tag_fat(_tmp30F,sizeof(void*),1U));});});*_tmp312=_tmp6FC;});_tmp312;});_tmp313->f2=_tmp6FD;});_tmp313;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp6FE=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp30E=_cycalloc(sizeof(*_tmp30E));_tmp30E->tag=4U,_tmp30E->f1=vd;_tmp30E;});Cyc_Absyn_varb_exp(_tmp6FE,e->loc);});
# 1873
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 1875
{void*_tmp300=Cyc_Tcutil_compress(e_type);void*_tmp301=_tmp300;void*_tmp30D;struct Cyc_Absyn_Tqual _tmp30C;void*_tmp30B;void*_tmp30A;void*_tmp309;void*_tmp308;struct Cyc_Absyn_PtrLoc*_tmp307;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->tag == 3U){_LL1: _tmp30D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).elt_type;_tmp30C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).elt_tq;_tmp30B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).rgn;_tmp30A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).nullable;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).bounds;_tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).zero_term;_tmp307=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp301)->f1).ptr_atts).ptrloc;_LL2:
# 1877
{void*_tmp302=Cyc_Tcutil_compress(_tmp30B);void*_tmp303=_tmp302;void**_tmp306;struct Cyc_Core_Opt*_tmp305;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp303)->tag == 1U){_LL6: _tmp306=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp303)->f2;_tmp305=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp303)->f4;_LL7: {
# 1879
void*_tmp304=Cyc_Absyn_var_type(tv);
*_tmp306=_tmp304;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1884
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1888
e->topt=0;
vd->initializer=0;{
# 1892
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1894
return({struct _tuple23 _tmp615;_tmp615.f1=d,_tmp615.f2=ve;_tmp615;});};}
# 1899
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1902
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1907
void*_tmp314=Cyc_Tcutil_compress(wants_type);void*_tmp315=_tmp314;void*_tmp317;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->tag == 3U){_LL1: _tmp317=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp315)->f1).ptr_atts).rgn;_LL2:
# 1909
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp317))return 0;{
struct Cyc_Absyn_Kind*_tmp316=Cyc_Tcutil_type_kind(_tmp317);
return(int)_tmp316->kind == (int)Cyc_Absyn_RgnKind &&(int)_tmp316->aliasqual == (int)Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1915
return 0;}
# 1919
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1921
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1925
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1928
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1930
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp31A=({struct Cyc_String_pa_PrintArg_struct _tmp618;_tmp618.tag=0U,({
struct _fat_ptr _tmp6FF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp618.f1=_tmp6FF;});_tmp618;});struct Cyc_String_pa_PrintArg_struct _tmp31B=({struct Cyc_String_pa_PrintArg_struct _tmp617;_tmp617.tag=0U,({struct _fat_ptr _tmp700=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp617.f1=_tmp700;});_tmp617;});void*_tmp318[2U];_tmp318[0]=& _tmp31A,_tmp318[1]=& _tmp31B;({unsigned _tmp702=e->loc;struct _fat_ptr _tmp701=({const char*_tmp319="integral size mismatch; %s -> %s conversion supplied";_tag_fat(_tmp319,sizeof(char),53U);});Cyc_Tcutil_warn(_tmp702,_tmp701,_tag_fat(_tmp318,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1938
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp31E=({struct Cyc_String_pa_PrintArg_struct _tmp61B;_tmp61B.tag=0U,({
struct _fat_ptr _tmp703=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp61B.f1=_tmp703;});_tmp61B;});struct Cyc_String_pa_PrintArg_struct _tmp31F=({struct Cyc_String_pa_PrintArg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp704=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp61A.f1=_tmp704;});_tmp61A;});struct Cyc_String_pa_PrintArg_struct _tmp320=({struct Cyc_String_pa_PrintArg_struct _tmp619;_tmp619.tag=0U,({struct _fat_ptr _tmp705=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp619.f1=_tmp705;});_tmp619;});void*_tmp31C[3U];_tmp31C[0]=& _tmp31E,_tmp31C[1]=& _tmp31F,_tmp31C[2]=& _tmp320;({unsigned _tmp707=e->loc;struct _fat_ptr _tmp706=({const char*_tmp31D="implicit alias coercion for %s:%s to %s";_tag_fat(_tmp31D,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp707,_tmp706,_tag_fat(_tmp31C,sizeof(void*),3U));});});
*alias_coercion=1;}
# 1945
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1949
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1953
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp323=({struct Cyc_String_pa_PrintArg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp708=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp61D.f1=_tmp708;});_tmp61D;});struct Cyc_String_pa_PrintArg_struct _tmp324=({struct Cyc_String_pa_PrintArg_struct _tmp61C;_tmp61C.tag=0U,({struct _fat_ptr _tmp709=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp61C.f1=_tmp709;});_tmp61C;});void*_tmp321[2U];_tmp321[0]=& _tmp323,_tmp321[1]=& _tmp324;({unsigned _tmp70B=e->loc;struct _fat_ptr _tmp70A=({const char*_tmp322="implicit cast from %s to %s";_tag_fat(_tmp322,sizeof(char),28U);});Cyc_Tcutil_warn(_tmp70B,_tmp70A,_tag_fat(_tmp321,sizeof(void*),2U));});});
return 1;}
# 1959
return 0;}
# 1965
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1968
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1982 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1985
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1988
struct _tuple24 _tmp325=*env;struct _tuple24 _tmp326=_tmp325;struct Cyc_List_List*_tmp32D;struct _RegionHandle*_tmp32C;int _tmp32B;_LL1: _tmp32D=_tmp326.f1;_tmp32C=_tmp326.f2;_tmp32B=_tmp326.f3;_LL2:;{
void*_tmp327=_tmp32D == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp32C,_tmp32D,x->type);
struct Cyc_List_List*_tmp328=Cyc_Tcutil_flatten_type(_tmp32C,_tmp32B,_tmp327);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp328)== 1)
return({struct Cyc_List_List*_tmp32A=_region_malloc(_tmp32C,sizeof(*_tmp32A));({struct _tuple12*_tmp70C=({struct _tuple12*_tmp329=_region_malloc(_tmp32C,sizeof(*_tmp329));_tmp329->f1=x->tq,_tmp329->f2=_tmp327;_tmp329;});_tmp32A->hd=_tmp70C;}),_tmp32A->tl=0;_tmp32A;});
return _tmp328;};}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1995
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 1997
struct _tuple25 _tmp32E=*env;struct _tuple25 _tmp32F=_tmp32E;struct _RegionHandle*_tmp338;int _tmp337;_LL1: _tmp338=_tmp32F.f1;_tmp337=_tmp32F.f2;_LL2:;{
struct _tuple12 _tmp330=*x;struct _tuple12 _tmp331=_tmp330;struct Cyc_Absyn_Tqual _tmp336;void*_tmp335;_LL4: _tmp336=_tmp331.f1;_tmp335=_tmp331.f2;_LL5:;{
struct Cyc_List_List*_tmp332=Cyc_Tcutil_flatten_type(_tmp338,_tmp337,_tmp335);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp332)== 1)
return({struct Cyc_List_List*_tmp334=_region_malloc(_tmp338,sizeof(*_tmp334));({struct _tuple12*_tmp70D=({struct _tuple12*_tmp333=_region_malloc(_tmp338,sizeof(*_tmp333));_tmp333->f1=_tmp336,_tmp333->f2=_tmp335;_tmp333;});_tmp334->hd=_tmp70D;}),_tmp334->tl=0;_tmp334;});
return _tmp332;};};}
# 2004
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 2007
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp339=t1;struct Cyc_List_List*_tmp358;struct Cyc_List_List*_tmp357;struct Cyc_Absyn_Aggrdecl*_tmp356;struct Cyc_List_List*_tmp355;switch(*((int*)_tmp339)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp356=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f1)->f1).KnownAggr).val;_tmp355=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp339)->f2;_LL6:
# 2025
 if((((int)_tmp356->kind == (int)Cyc_Absyn_UnionA || _tmp356->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp356->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp356->impl))->rgn_po != 0)
# 2027
return({struct Cyc_List_List*_tmp344=_region_malloc(r,sizeof(*_tmp344));({struct _tuple12*_tmp70F=({struct _tuple12*_tmp343=_region_malloc(r,sizeof(*_tmp343));({struct Cyc_Absyn_Tqual _tmp70E=Cyc_Absyn_empty_tqual(0U);_tmp343->f1=_tmp70E;}),_tmp343->f2=t1;_tmp343;});_tmp344->hd=_tmp70F;}),_tmp344->tl=0;_tmp344;});{
struct Cyc_List_List*_tmp345=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp356->tvs,_tmp355);
struct _tuple24 env=({struct _tuple24 _tmp61E;_tmp61E.f1=_tmp345,_tmp61E.f2=r,_tmp61E.f3=flatten;_tmp61E;});
struct Cyc_List_List*_tmp346=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp356->impl))->fields;struct Cyc_List_List*_tmp347=_tmp346;struct Cyc_Absyn_Aggrfield*_tmp34D;struct Cyc_List_List*_tmp34C;if(_tmp347 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp34D=(struct Cyc_Absyn_Aggrfield*)_tmp347->hd;_tmp34C=_tmp347->tl;_LL14: {
# 2033
struct Cyc_List_List*_tmp348=Cyc_Tcutil_flatten_type_f(& env,_tmp34D);
env.f3=0;{
struct Cyc_List_List*_tmp349=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp34C);
struct Cyc_List_List*_tmp34A=({struct Cyc_List_List*_tmp34B=_region_malloc(r,sizeof(*_tmp34B));_tmp34B->hd=_tmp348,_tmp34B->tl=_tmp349;_tmp34B;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp34A);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp357=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp339)->f1;_LL4: {
# 2012
struct _tuple25 _tmp33A=({struct _tuple25 _tmp61F;_tmp61F.f1=r,_tmp61F.f2=flatten;_tmp61F;});
# 2014
struct Cyc_List_List*_tmp33B=_tmp357;struct _tuple12*_tmp342;struct Cyc_List_List*_tmp341;if(_tmp33B == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp342=(struct _tuple12*)_tmp33B->hd;_tmp341=_tmp33B->tl;_LLF: {
# 2017
struct Cyc_List_List*_tmp33C=Cyc_Tcutil_rcopy_tqt(& _tmp33A,_tmp342);
_tmp33A.f2=0;{
struct Cyc_List_List*_tmp33D=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp33A,_tmp357);
struct Cyc_List_List*_tmp33E=({struct Cyc_List_List*_tmp340=_region_malloc(r,sizeof(*_tmp340));_tmp340->hd=_tmp33C,_tmp340->tl=_tmp33D;_tmp340;});
return({struct _RegionHandle*_tmp710=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp710,({struct Cyc_List_List*_tmp33F=_region_malloc(r,sizeof(*_tmp33F));_tmp33F->hd=_tmp33C,_tmp33F->tl=_tmp33D;_tmp33F;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp339)->f1 == Cyc_Absyn_StructA){_LL7: _tmp358=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp339)->f2;_LL8: {
# 2040
struct _tuple24 env=({struct _tuple24 _tmp620;_tmp620.f1=0,_tmp620.f2=r,_tmp620.f3=flatten;_tmp620;});
struct Cyc_List_List*_tmp34E=_tmp358;struct Cyc_Absyn_Aggrfield*_tmp354;struct Cyc_List_List*_tmp353;if(_tmp34E == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp354=(struct Cyc_Absyn_Aggrfield*)_tmp34E->hd;_tmp353=_tmp34E->tl;_LL19: {
# 2044
struct Cyc_List_List*_tmp34F=Cyc_Tcutil_flatten_type_f(& env,_tmp354);
env.f3=0;{
struct Cyc_List_List*_tmp350=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp353);
struct Cyc_List_List*_tmp351=({struct Cyc_List_List*_tmp352=_region_malloc(r,sizeof(*_tmp352));_tmp352->hd=_tmp34F,_tmp352->tl=_tmp350;_tmp352;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp351);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2050
 goto _LL0;}_LL0:;};}
# 2053
return({struct Cyc_List_List*_tmp35A=_region_malloc(r,sizeof(*_tmp35A));({struct _tuple12*_tmp712=({struct _tuple12*_tmp359=_region_malloc(r,sizeof(*_tmp359));({struct Cyc_Absyn_Tqual _tmp711=Cyc_Absyn_empty_tqual(0U);_tmp359->f1=_tmp711;}),_tmp359->f2=t1;_tmp359;});_tmp35A->hd=_tmp712;}),_tmp35A->tl=0;_tmp35A;});}
# 2057
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp35B=(void*)t->hd;void*_tmp35C=_tmp35B;switch(*((int*)_tmp35C)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
# 2064
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2067
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2070
return 1;}
# 2073
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp35D=({struct _tuple15 _tmp621;({void*_tmp714=Cyc_Tcutil_compress(t1);_tmp621.f1=_tmp714;}),({void*_tmp713=Cyc_Tcutil_compress(t2);_tmp621.f2=_tmp713;});_tmp621;});struct _tuple15 _tmp35E=_tmp35D;enum Cyc_Absyn_Size_of _tmp360;enum Cyc_Absyn_Size_of _tmp35F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->tag == 1U){_LL1: _tmp360=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f1)->f1)->f2;_tmp35F=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp35E.f2)->f1)->f2;_LL2:
# 2076
 return((int)_tmp360 == (int)_tmp35F ||(int)_tmp360 == (int)2U &&(int)_tmp35F == (int)3U)||
(int)_tmp360 == (int)3U &&(int)_tmp35F == (int)Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2084
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2086
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp361=({struct _tuple15 _tmp622;_tmp622.f1=t1,_tmp622.f2=t2;_tmp622;});struct _tuple15 _tmp362=_tmp361;struct Cyc_Absyn_FnInfo _tmp38C;struct Cyc_Absyn_FnInfo _tmp38B;struct Cyc_Absyn_Datatypedecl*_tmp38A;struct Cyc_Absyn_Datatypefield*_tmp389;struct Cyc_List_List*_tmp388;struct Cyc_Absyn_Datatypedecl*_tmp387;struct Cyc_List_List*_tmp386;void*_tmp385;struct Cyc_Absyn_Tqual _tmp384;void*_tmp383;void*_tmp382;void*_tmp381;void*_tmp380;void*_tmp37F;struct Cyc_Absyn_Tqual _tmp37E;void*_tmp37D;void*_tmp37C;void*_tmp37B;void*_tmp37A;switch(*((int*)_tmp362.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->tag == 3U){_LL1: _tmp385=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).elt_type;_tmp384=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).elt_tq;_tmp383=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).ptr_atts).rgn;_tmp382=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).ptr_atts).nullable;_tmp381=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).ptr_atts).bounds;_tmp380=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f1)->f1).ptr_atts).zero_term;_tmp37F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).elt_type;_tmp37E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).elt_tq;_tmp37D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).ptr_atts).rgn;_tmp37C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).ptr_atts).nullable;_tmp37B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).ptr_atts).bounds;_tmp37A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp362.f2)->f1).ptr_atts).zero_term;_LL2:
# 2098
 if(_tmp384.real_const && !_tmp37E.real_const)
return 0;
# 2101
if((!Cyc_Unify_unify(_tmp382,_tmp37C)&&
 Cyc_Absyn_type2bool(0,_tmp382))&& !Cyc_Absyn_type2bool(0,_tmp37C))
return 0;
# 2105
if((Cyc_Unify_unify(_tmp380,_tmp37A)&& !
Cyc_Absyn_type2bool(0,_tmp380))&& Cyc_Absyn_type2bool(0,_tmp37A))
return 0;
# 2109
if((!Cyc_Unify_unify(_tmp383,_tmp37D)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,_tmp383,_tmp37D))&& !
Cyc_Tcutil_subtype(po,assume,_tmp383,_tmp37D))
return 0;
# 2113
if(!Cyc_Unify_unify(_tmp381,_tmp37B)){
struct Cyc_Absyn_Exp*_tmp363=({void*_tmp715=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp715,_tmp381);});
struct Cyc_Absyn_Exp*_tmp364=({void*_tmp716=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp716,_tmp37B);});
if(_tmp363 != _tmp364){
if((_tmp363 == 0 || _tmp364 == 0)|| !Cyc_Evexp_lte_const_exp(_tmp364,_tmp364))
return 0;}}
# 2123
if(!_tmp37E.real_const && _tmp384.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp37F)))
return 0;}{
# 2129
int _tmp365=
({void*_tmp717=_tmp37B;Cyc_Unify_unify(_tmp717,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,_tmp37A);
# 2134
return(_tmp365 &&({struct Cyc_RgnOrder_RgnPO*_tmp71B=po;struct Cyc_List_List*_tmp71A=({struct Cyc_List_List*_tmp367=_cycalloc(sizeof(*_tmp367));({struct _tuple15*_tmp718=({struct _tuple15*_tmp366=_cycalloc(sizeof(*_tmp366));_tmp366->f1=t1,_tmp366->f2=t2;_tmp366;});_tmp367->hd=_tmp718;}),_tmp367->tl=assume;_tmp367;});void*_tmp719=_tmp385;Cyc_Tcutil_ptrsubtype(_tmp71B,_tmp71A,_tmp719,_tmp37F);})|| Cyc_Unify_unify(_tmp385,_tmp37F))|| Cyc_Tcutil_isomorphic(_tmp385,_tmp37F);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp38A=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp389=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp388=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f1)->f2;_tmp387=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->f1)->f1).KnownDatatype).val;_tmp386=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp362.f2)->f2;_LL4:
# 2141
 if(_tmp38A != _tmp387 && Cyc_Absyn_qvar_cmp(_tmp38A->name,_tmp387->name)!= 0)return 0;
# 2143
if(({int _tmp71C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp388);_tmp71C != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp386);}))return 0;
for(0;_tmp388 != 0;(_tmp388=_tmp388->tl,_tmp386=_tmp386->tl)){
if(!Cyc_Unify_unify((void*)_tmp388->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp386))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp362.f2)->tag == 5U){_LL5: _tmp38C=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp362.f1)->f1;_tmp38B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp362.f2)->f1;_LL6:
# 2150
 if(_tmp38C.tvars != 0 || _tmp38B.tvars != 0){
struct Cyc_List_List*_tmp368=_tmp38C.tvars;
struct Cyc_List_List*_tmp369=_tmp38B.tvars;
if(({int _tmp71D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp368);_tmp71D != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp369);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp368 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp368->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp369))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp36B=_cycalloc(sizeof(*_tmp36B));({struct _tuple19*_tmp71F=({struct _tuple19*_tmp36A=_cycalloc(sizeof(*_tmp36A));_tmp36A->f1=(struct Cyc_Absyn_Tvar*)_tmp369->hd,({void*_tmp71E=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp368->hd);_tmp36A->f2=_tmp71E;});_tmp36A;});_tmp36B->hd=_tmp71F;}),_tmp36B->tl=inst;_tmp36B;});
_tmp368=_tmp368->tl;
_tmp369=_tmp369->tl;}
# 2161
if(inst != 0){
_tmp38C.tvars=0;
_tmp38B.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp722=po;struct Cyc_List_List*_tmp721=assume;void*_tmp720=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp36C=_cycalloc(sizeof(*_tmp36C));_tmp36C->tag=5U,_tmp36C->f1=_tmp38C;_tmp36C;});Cyc_Tcutil_subtype(_tmp722,_tmp721,_tmp720,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp36D=_cycalloc(sizeof(*_tmp36D));_tmp36D->tag=5U,_tmp36D->f1=_tmp38B;_tmp36D;}));});}};}
# 2168
if(!Cyc_Tcutil_subtype(po,assume,_tmp38C.ret_type,_tmp38B.ret_type))return 0;{
struct Cyc_List_List*_tmp36E=_tmp38C.args;
struct Cyc_List_List*_tmp36F=_tmp38B.args;
# 2173
if(({int _tmp723=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp36E);_tmp723 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp36F);}))return 0;
# 2175
for(0;_tmp36E != 0;(_tmp36E=_tmp36E->tl,_tmp36F=_tmp36F->tl)){
struct _tuple9 _tmp370=*((struct _tuple9*)_tmp36E->hd);struct _tuple9 _tmp371=_tmp370;struct Cyc_Absyn_Tqual _tmp377;void*_tmp376;_LLA: _tmp377=_tmp371.f2;_tmp376=_tmp371.f3;_LLB:;{
struct _tuple9 _tmp372=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp36F))->hd);struct _tuple9 _tmp373=_tmp372;struct Cyc_Absyn_Tqual _tmp375;void*_tmp374;_LLD: _tmp375=_tmp373.f2;_tmp374=_tmp373.f3;_LLE:;
# 2179
if(_tmp375.real_const && !_tmp377.real_const || !Cyc_Tcutil_subtype(po,assume,_tmp374,_tmp376))
return 0;};}
# 2183
if(_tmp38C.c_varargs != _tmp38B.c_varargs)return 0;
if(_tmp38C.cyc_varargs != 0 && _tmp38B.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp378=*_tmp38C.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp379=*_tmp38B.cyc_varargs;
# 2188
if((_tmp379.tq).real_const && !(_tmp378.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,_tmp379.type,_tmp378.type))
return 0;}else{
if(_tmp38C.cyc_varargs != 0 || _tmp38B.cyc_varargs != 0)return 0;}
# 2193
if(!({void*_tmp724=(void*)_check_null(_tmp38C.effect);Cyc_Tcutil_subset_effect(1,_tmp724,(void*)_check_null(_tmp38B.effect));}))return 0;
# 2195
if(!Cyc_Tcutil_sub_rgnpo(_tmp38C.rgn_po,_tmp38B.rgn_po))return 0;
# 2197
if(!Cyc_Tcutil_sub_attributes(_tmp38C.attributes,_tmp38B.attributes))return 0;
# 2199
if(!Cyc_Relations_check_logical_implication(_tmp38B.requires_relns,_tmp38C.requires_relns))
# 2201
return 0;
# 2203
if(!Cyc_Relations_check_logical_implication(_tmp38C.ensures_relns,_tmp38B.ensures_relns))
# 2205
return 0;
# 2207
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2218
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2222
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp38D=(struct _tuple12*)tqs1->hd;struct _tuple12*_tmp38E=_tmp38D;struct Cyc_Absyn_Tqual _tmp394;void*_tmp393;_LL1: _tmp394=_tmp38E->f1;_tmp393=_tmp38E->f2;_LL2:;{
struct _tuple12*_tmp38F=(struct _tuple12*)tqs2->hd;struct _tuple12*_tmp390=_tmp38F;struct Cyc_Absyn_Tqual _tmp392;void*_tmp391;_LL4: _tmp392=_tmp390->f1;_tmp391=_tmp390->f2;_LL5:;
if(_tmp394.real_const && !_tmp392.real_const)return 0;
if((_tmp392.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp391)))&&
 Cyc_Tcutil_subtype(po,assume,_tmp393,_tmp391))
continue;
if(Cyc_Unify_unify(_tmp393,_tmp391))
continue;
if(Cyc_Tcutil_isomorphic(_tmp393,_tmp391))
continue;
return 0;};};}
# 2238
return 1;}
# 2243
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2249
{void*_tmp395=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp395)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp395)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp395)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2252
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2254
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2258
void*_tmp396=t1;void*_tmp3BB;struct Cyc_Absyn_Enumdecl*_tmp3BA;void*_tmp3B9;struct Cyc_Absyn_Tqual _tmp3B8;struct Cyc_Absyn_Exp*_tmp3B7;void*_tmp3B6;void*_tmp3B5;struct Cyc_Absyn_Tqual _tmp3B4;void*_tmp3B3;void*_tmp3B2;void*_tmp3B1;void*_tmp3B0;switch(*((int*)_tmp396)){case 3U: _LLA: _tmp3B5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).elt_type;_tmp3B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).elt_tq;_tmp3B3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).ptr_atts).rgn;_tmp3B2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).ptr_atts).nullable;_tmp3B1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).ptr_atts).bounds;_tmp3B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp396)->f1).ptr_atts).zero_term;_LLB:
# 2267
{void*_tmp397=t2;void*_tmp3A6;struct Cyc_Absyn_Tqual _tmp3A5;void*_tmp3A4;void*_tmp3A3;void*_tmp3A2;void*_tmp3A1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp397)->tag == 3U){_LL19: _tmp3A6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp397)->f1).elt_type;_tmp3A5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp397)->f1).elt_tq;_tmp3A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp397)->f1).ptr_atts).rgn;_tmp3A3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp397)->f1).ptr_atts).nullable;_tmp3A2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp397)->f1).ptr_atts).bounds;_tmp3A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp397)->f1).ptr_atts).zero_term;_LL1A: {
# 2269
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp398=({struct Cyc_List_List*_tmp3A0=_cycalloc(sizeof(*_tmp3A0));({struct _tuple15*_tmp725=({struct _tuple15*_tmp39F=_cycalloc(sizeof(*_tmp39F));_tmp39F->f1=t1,_tmp39F->f2=t2;_tmp39F;});_tmp3A0->hd=_tmp725;}),_tmp3A0->tl=0;_tmp3A0;});
int _tmp399=_tmp3A5.real_const || !_tmp3B4.real_const;
# 2282 "tcutil.cyc"
int _tmp39A=
({void*_tmp726=_tmp3A2;Cyc_Unify_unify(_tmp726,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,_tmp3A1);
# 2285
int _tmp39B=_tmp399 &&(
((_tmp39A && Cyc_Tcutil_ptrsubtype(po,_tmp398,_tmp3B5,_tmp3A6)||
 Cyc_Unify_unify(_tmp3B5,_tmp3A6))|| Cyc_Tcutil_isomorphic(_tmp3B5,_tmp3A6))|| Cyc_Unify_unify(_tmp3A6,Cyc_Absyn_void_type));
int zeroterm_ok=Cyc_Unify_unify(_tmp3B0,_tmp3A1)|| !Cyc_Absyn_type2bool(0,_tmp3A1);
# 2290
int _tmp39C=_tmp39B?0:((Cyc_Tcutil_is_bits_only_type(_tmp3B5)&& Cyc_Tcutil_is_char_type(_tmp3A6))&& !
Cyc_Tcutil_force_type2bool(0,_tmp3A1))&&(
_tmp3A5.real_const || !_tmp3B4.real_const);
int bounds_ok=Cyc_Unify_unify(_tmp3B1,_tmp3A2);
if(!bounds_ok && !_tmp39C){
struct Cyc_Absyn_Exp*_tmp39D=({void*_tmp727=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp727,_tmp3B1);});
struct Cyc_Absyn_Exp*_tmp39E=({void*_tmp728=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp728,_tmp3A2);});
if((_tmp39D != 0 && _tmp39E != 0)&& Cyc_Evexp_lte_const_exp(_tmp39E,_tmp39D))
bounds_ok=1;else{
if(_tmp39D == 0 || _tmp39E == 0)
bounds_ok=1;}}{
# 2302
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp3B2);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp3A3);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2307
if(((bounds_ok && zeroterm_ok)&&(_tmp39B || _tmp39C))&&(
Cyc_Unify_unify(_tmp3B3,_tmp3A4)|| Cyc_RgnOrder_rgn_outlives_rgn(po,_tmp3B3,_tmp3A4)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2313
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp3B9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).elt_type;_tmp3B8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).tq;_tmp3B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).num_elts;_tmp3B6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp396)->f1).zero_term;_LLD:
# 2315
{void*_tmp3A7=t2;void*_tmp3AB;struct Cyc_Absyn_Tqual _tmp3AA;struct Cyc_Absyn_Exp*_tmp3A9;void*_tmp3A8;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A7)->tag == 4U){_LL1E: _tmp3AB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A7)->f1).elt_type;_tmp3AA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A7)->f1).tq;_tmp3A9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A7)->f1).num_elts;_tmp3A8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3A7)->f1).zero_term;_LL1F: {
# 2317
int okay=
(((_tmp3B7 != 0 && _tmp3A9 != 0)&& Cyc_Unify_unify(_tmp3B6,_tmp3A8))&&
 Cyc_Evexp_lte_const_exp(_tmp3A9,_tmp3B7))&& Cyc_Evexp_lte_const_exp(_tmp3B7,_tmp3A9);
return
# 2322
(okay && Cyc_Unify_unify(_tmp3B9,_tmp3AB))&&(!_tmp3B8.real_const || _tmp3AA.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2324
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2326
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp396)->f1)){case 15U: _LLE: _tmp3BA=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp396)->f1)->f2;_LLF:
# 2330
{void*_tmp3AC=t2;struct Cyc_Absyn_Enumdecl*_tmp3AD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f1)->tag == 15U){_LL23: _tmp3AD=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f1)->f2;_LL24:
# 2332
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3BA))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp3AD))->fields != 0)&&({
int _tmp729=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3BA->fields))->v);_tmp729 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp3AD->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2338
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2341
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp396)->f2 != 0){_LL14: _tmp3BB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp396)->f2)->hd;_LL15:
# 2344
{void*_tmp3AE=t2;void*_tmp3AF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f2 != 0){_LL28: _tmp3AF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AE)->f2)->hd;_LL29:
# 2346
 if(Cyc_RgnOrder_rgn_outlives_rgn(po,_tmp3BB,_tmp3AF))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2350
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 2356
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3BC=Cyc_Absyn_copy_exp(e);
({void*_tmp72A=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3BD=_cycalloc(sizeof(*_tmp3BD));_tmp3BD->tag=14U,_tmp3BD->f1=t,_tmp3BD->f2=_tmp3BC,_tmp3BD->f3=0,_tmp3BD->f4=c;_tmp3BD;});e->r=_tmp72A;});
e->topt=t;};}
# 2365
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2370
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3C3=({struct Cyc_Int_pa_PrintArg_struct _tmp623;_tmp623.tag=1U,_tmp623.f1=(unsigned long)i;_tmp623;});void*_tmp3C1[1U];_tmp3C1[0]=& _tmp3C3;({struct _fat_ptr _tmp72B=({const char*_tmp3C2="#%d";_tag_fat(_tmp3C2,sizeof(char),4U);});Cyc_aprintf(_tmp72B,_tag_fat(_tmp3C1,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp3C0=_cycalloc(sizeof(*_tmp3C0));({struct _fat_ptr*_tmp72C=({unsigned _tmp3BF=1;struct _fat_ptr*_tmp3BE=_cycalloc(_check_times(_tmp3BF,sizeof(struct _fat_ptr)));_tmp3BE[0]=s;_tmp3BE;});_tmp3C0->name=_tmp72C;}),_tmp3C0->identity=- 1,_tmp3C0->kind=k;_tmp3C0;});}
# 2377
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp3C4=*t->name;
return(int)*((const char*)_check_fat_subscript(_tmp3C4,sizeof(char),0))== (int)'#';}
# 2383
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2390
struct Cyc_List_List*_tmp3C5=0;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp3C5=({struct Cyc_List_List*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));_tmp3C6->hd=(void*)atts->hd,_tmp3C6->tl=_tmp3C5;_tmp3C6;});}}{
struct Cyc_Absyn_FnInfo _tmp3C7=fd->i;
_tmp3C7.attributes=_tmp3C5;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));_tmp3C8->tag=5U,_tmp3C8->f1=_tmp3C7;_tmp3C8;});};}
# 2398
return(void*)_check_null(fd->cached_type);}
# 2404
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2406
union Cyc_Relations_RelnOp _tmp3C9=*rop;union Cyc_Relations_RelnOp _tmp3CA=_tmp3C9;struct Cyc_Absyn_Vardecl*_tmp3DD;struct Cyc_Absyn_Vardecl*_tmp3DC;switch((_tmp3CA.RNumelts).tag){case 2U: _LL1: _tmp3DC=(_tmp3CA.RVar).val;_LL2: {
# 2408
struct _tuple1 _tmp3CB=*_tmp3DC->name;struct _tuple1 _tmp3CC=_tmp3CB;union Cyc_Absyn_Nmspace _tmp3D3;struct _fat_ptr*_tmp3D2;_LL8: _tmp3D3=_tmp3CC.f1;_tmp3D2=_tmp3CC.f2;_LL9:;
if(!((int)((_tmp3D3.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp72D=(struct _fat_ptr)*_tmp3D2;Cyc_strcmp(_tmp72D,({const char*_tmp3CD="return_value";_tag_fat(_tmp3CD,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp72E=Cyc_Relations_RReturn();*rop=_tmp72E;});
goto _LL0;}{
# 2414
unsigned c=0U;
{struct Cyc_List_List*_tmp3CE=args;for(0;_tmp3CE != 0;(_tmp3CE=_tmp3CE->tl,c ++)){
struct _tuple9*_tmp3CF=(struct _tuple9*)_tmp3CE->hd;struct _tuple9*_tmp3D0=_tmp3CF;struct _fat_ptr*_tmp3D1;_LLB: _tmp3D1=_tmp3D0->f1;_LLC:;
if(_tmp3D1 != 0){
if(Cyc_strcmp((struct _fat_ptr)*_tmp3D2,(struct _fat_ptr)*_tmp3D1)== 0){
({union Cyc_Relations_RelnOp _tmp72F=Cyc_Relations_RParam(c);*rop=_tmp72F;});
break;}}}}
# 2424
goto _LL0;};}case 3U: _LL3: _tmp3DD=(_tmp3CA.RNumelts).val;_LL4: {
# 2426
struct _tuple1 _tmp3D4=*_tmp3DD->name;struct _tuple1 _tmp3D5=_tmp3D4;union Cyc_Absyn_Nmspace _tmp3DB;struct _fat_ptr*_tmp3DA;_LLE: _tmp3DB=_tmp3D5.f1;_tmp3DA=_tmp3D5.f2;_LLF:;
if(!((int)((_tmp3DB.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3D6=args;for(0;_tmp3D6 != 0;(_tmp3D6=_tmp3D6->tl,c ++)){
struct _tuple9*_tmp3D7=(struct _tuple9*)_tmp3D6->hd;struct _tuple9*_tmp3D8=_tmp3D7;struct _fat_ptr*_tmp3D9;_LL11: _tmp3D9=_tmp3D8->f1;_LL12:;
if(_tmp3D9 != 0){
if(Cyc_strcmp((struct _fat_ptr)*_tmp3DA,(struct _fat_ptr)*_tmp3D9)== 0){
({union Cyc_Relations_RelnOp _tmp730=Cyc_Relations_RParamNumelts(c);*rop=_tmp730;});
break;}}}}
# 2438
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2443
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2445
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2449
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2451
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3DE=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp3DE);
return _tmp3DE;};}
# 2458
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3DF=pr;struct Cyc_Absyn_Tqual _tmp3E2;void*_tmp3E1;_LL1: _tmp3E2=_tmp3DF->f1;_tmp3E1=_tmp3DF->f2;_LL2:;
if(_tmp3E1 == t)return pr;else{
return({struct _tuple12*_tmp3E0=_cycalloc(sizeof(*_tmp3E0));_tmp3E0->f1=_tmp3E2,_tmp3E0->f2=t;_tmp3E0;});}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2464
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2466
return({struct _tuple27*_tmp3E4=_region_malloc(rgn,sizeof(*_tmp3E4));({struct _tuple26*_tmp731=({struct _tuple26*_tmp3E3=_region_malloc(rgn,sizeof(*_tmp3E3));_tmp3E3->f1=(*y).f1,_tmp3E3->f2=(*y).f2;_tmp3E3;});_tmp3E4->f1=_tmp731;}),_tmp3E4->f2=(*y).f3;_tmp3E4;});}
# 2468
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2470
struct _tuple9 _tmp3E5=*orig_arg;struct _tuple9 _tmp3E6=_tmp3E5;struct _fat_ptr*_tmp3EA;struct Cyc_Absyn_Tqual _tmp3E9;void*_tmp3E8;_LL1: _tmp3EA=_tmp3E6.f1;_tmp3E9=_tmp3E6.f2;_tmp3E8=_tmp3E6.f3;_LL2:;
if(t == _tmp3E8)return orig_arg;
return({struct _tuple9*_tmp3E7=_cycalloc(sizeof(*_tmp3E7));_tmp3E7->f1=_tmp3EA,_tmp3E7->f2=_tmp3E9,_tmp3E7->f3=t;_tmp3E7;});}
# 2474
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2479
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2481
return({struct Cyc_Absyn_Exp*_tmp3EB=_cycalloc(sizeof(*_tmp3EB));_tmp3EB->topt=old->topt,_tmp3EB->r=r,_tmp3EB->loc=old->loc,_tmp3EB->annot=old->annot;_tmp3EB;});}
# 2486
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3EC=e->r;void*_tmp3ED=_tmp3EC;void*_tmp42D;void*_tmp42C;struct Cyc_List_List*_tmp42B;struct Cyc_Absyn_Exp*_tmp42A;struct Cyc_Absyn_Exp*_tmp429;void*_tmp428;void*_tmp427;struct Cyc_Absyn_Exp*_tmp426;int _tmp425;enum Cyc_Absyn_Coercion _tmp424;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp422;struct Cyc_Absyn_Exp*_tmp421;struct Cyc_Absyn_Exp*_tmp420;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_Exp*_tmp41D;struct Cyc_Absyn_Exp*_tmp41C;struct Cyc_Absyn_Exp*_tmp41B;enum Cyc_Absyn_Primop _tmp41A;struct Cyc_List_List*_tmp419;switch(*((int*)_tmp3ED)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp41A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp419=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LLC:
# 2495
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp419)== 1){
struct Cyc_Absyn_Exp*_tmp3EE=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp419))->hd;
struct Cyc_Absyn_Exp*_tmp3EF=Cyc_Tcutil_rsubsexp(r,inst,_tmp3EE);
if(_tmp3EF == _tmp3EE)return e;
return({struct Cyc_Absyn_Exp*_tmp733=e;Cyc_Tcutil_copye(_tmp733,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->tag=3U,_tmp3F1->f1=_tmp41A,({struct Cyc_List_List*_tmp732=({struct Cyc_Absyn_Exp*_tmp3F0[1U];_tmp3F0[0]=_tmp3EF;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3F0,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp3F1->f2=_tmp732;});_tmp3F1;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp419)== 2){
struct Cyc_Absyn_Exp*_tmp3F2=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp419))->hd;
struct Cyc_Absyn_Exp*_tmp3F3=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp419->tl))->hd;
struct Cyc_Absyn_Exp*_tmp3F4=Cyc_Tcutil_rsubsexp(r,inst,_tmp3F2);
struct Cyc_Absyn_Exp*_tmp3F5=Cyc_Tcutil_rsubsexp(r,inst,_tmp3F3);
if(_tmp3F4 == _tmp3F2 && _tmp3F5 == _tmp3F3)return e;
return({struct Cyc_Absyn_Exp*_tmp735=e;Cyc_Tcutil_copye(_tmp735,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp3F7=_cycalloc(sizeof(*_tmp3F7));_tmp3F7->tag=3U,_tmp3F7->f1=_tmp41A,({struct Cyc_List_List*_tmp734=({struct Cyc_Absyn_Exp*_tmp3F6[2U];_tmp3F6[0]=_tmp3F4,_tmp3F6[1]=_tmp3F5;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp3F6,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp3F7->f2=_tmp734;});_tmp3F7;}));});}else{
return({void*_tmp3F8=0U;({struct _fat_ptr _tmp736=({const char*_tmp3F9="primop does not have 1 or 2 args!";_tag_fat(_tmp3F9,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp736,_tag_fat(_tmp3F8,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp41D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp41C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_tmp41B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3ED)->f3;_LLE: {
# 2509
struct Cyc_Absyn_Exp*_tmp3FA=Cyc_Tcutil_rsubsexp(r,inst,_tmp41D);
struct Cyc_Absyn_Exp*_tmp3FB=Cyc_Tcutil_rsubsexp(r,inst,_tmp41C);
struct Cyc_Absyn_Exp*_tmp3FC=Cyc_Tcutil_rsubsexp(r,inst,_tmp41B);
if((_tmp3FA == _tmp41D && _tmp3FB == _tmp41C)&& _tmp3FC == _tmp41B)return e;
return({struct Cyc_Absyn_Exp*_tmp737=e;Cyc_Tcutil_copye(_tmp737,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp3FD=_cycalloc(sizeof(*_tmp3FD));_tmp3FD->tag=6U,_tmp3FD->f1=_tmp3FA,_tmp3FD->f2=_tmp3FB,_tmp3FD->f3=_tmp3FC;_tmp3FD;}));});}case 7U: _LLF: _tmp41F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp41E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LL10: {
# 2515
struct Cyc_Absyn_Exp*_tmp3FE=Cyc_Tcutil_rsubsexp(r,inst,_tmp41F);
struct Cyc_Absyn_Exp*_tmp3FF=Cyc_Tcutil_rsubsexp(r,inst,_tmp41E);
if(_tmp3FE == _tmp41F && _tmp3FF == _tmp41E)return e;
return({struct Cyc_Absyn_Exp*_tmp738=e;Cyc_Tcutil_copye(_tmp738,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->tag=7U,_tmp400->f1=_tmp3FE,_tmp400->f2=_tmp3FF;_tmp400;}));});}case 8U: _LL11: _tmp421=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp420=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LL12: {
# 2520
struct Cyc_Absyn_Exp*_tmp401=Cyc_Tcutil_rsubsexp(r,inst,_tmp421);
struct Cyc_Absyn_Exp*_tmp402=Cyc_Tcutil_rsubsexp(r,inst,_tmp420);
if(_tmp401 == _tmp421 && _tmp402 == _tmp420)return e;
return({struct Cyc_Absyn_Exp*_tmp739=e;Cyc_Tcutil_copye(_tmp739,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp403=_cycalloc(sizeof(*_tmp403));_tmp403->tag=8U,_tmp403->f1=_tmp401,_tmp403->f2=_tmp402;_tmp403;}));});}case 9U: _LL13: _tmp423=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp422=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LL14: {
# 2525
struct Cyc_Absyn_Exp*_tmp404=Cyc_Tcutil_rsubsexp(r,inst,_tmp423);
struct Cyc_Absyn_Exp*_tmp405=Cyc_Tcutil_rsubsexp(r,inst,_tmp422);
if(_tmp404 == _tmp423 && _tmp405 == _tmp422)return e;
return({struct Cyc_Absyn_Exp*_tmp73A=e;Cyc_Tcutil_copye(_tmp73A,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp406=_cycalloc(sizeof(*_tmp406));_tmp406->tag=9U,_tmp406->f1=_tmp404,_tmp406->f2=_tmp405;_tmp406;}));});}case 14U: _LL15: _tmp427=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp426=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_tmp425=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3ED)->f3;_tmp424=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3ED)->f4;_LL16: {
# 2530
struct Cyc_Absyn_Exp*_tmp407=Cyc_Tcutil_rsubsexp(r,inst,_tmp426);
void*_tmp408=Cyc_Tcutil_rsubstitute(r,inst,_tmp427);
if(_tmp407 == _tmp426 && _tmp408 == _tmp427)return e;
return({struct Cyc_Absyn_Exp*_tmp73B=e;Cyc_Tcutil_copye(_tmp73B,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp409=_cycalloc(sizeof(*_tmp409));_tmp409->tag=14U,_tmp409->f1=_tmp408,_tmp409->f2=_tmp407,_tmp409->f3=_tmp425,_tmp409->f4=_tmp424;_tmp409;}));});}case 17U: _LL17: _tmp428=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_LL18: {
# 2535
void*_tmp40A=Cyc_Tcutil_rsubstitute(r,inst,_tmp428);
if(_tmp40A == _tmp428)return e;
return({struct Cyc_Absyn_Exp*_tmp73C=e;Cyc_Tcutil_copye(_tmp73C,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp40B=_cycalloc(sizeof(*_tmp40B));_tmp40B->tag=17U,_tmp40B->f1=_tmp40A;_tmp40B;}));});}case 18U: _LL19: _tmp429=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_LL1A: {
# 2539
struct Cyc_Absyn_Exp*_tmp40C=Cyc_Tcutil_rsubsexp(r,inst,_tmp429);
if(_tmp40C == _tmp429)return e;
return({struct Cyc_Absyn_Exp*_tmp73D=e;Cyc_Tcutil_copye(_tmp73D,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->tag=18U,_tmp40D->f1=_tmp40C;_tmp40D;}));});}case 41U: _LL1B: _tmp42A=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_LL1C: {
# 2543
struct Cyc_Absyn_Exp*_tmp40E=Cyc_Tcutil_rsubsexp(r,inst,_tmp42A);
if(_tmp40E == _tmp42A)return e;
return({struct Cyc_Absyn_Exp*_tmp73E=e;Cyc_Tcutil_copye(_tmp73E,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp40F=_cycalloc(sizeof(*_tmp40F));_tmp40F->tag=41U,_tmp40F->f1=_tmp40E;_tmp40F;}));});}case 19U: _LL1D: _tmp42C=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_tmp42B=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3ED)->f2;_LL1E: {
# 2547
void*_tmp410=Cyc_Tcutil_rsubstitute(r,inst,_tmp42C);
if(_tmp410 == _tmp42C)return e;
return({struct Cyc_Absyn_Exp*_tmp73F=e;Cyc_Tcutil_copye(_tmp73F,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->tag=19U,_tmp411->f1=_tmp410,_tmp411->f2=_tmp42B;_tmp411;}));});}case 39U: _LL1F: _tmp42D=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3ED)->f1;_LL20: {
# 2551
void*_tmp412=Cyc_Tcutil_rsubstitute(r,inst,_tmp42D);
if(_tmp412 == _tmp42D)return e;{
# 2554
void*_tmp413=Cyc_Tcutil_compress(_tmp412);void*_tmp414=_tmp413;struct Cyc_Absyn_Exp*_tmp416;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp414)->tag == 9U){_LL24: _tmp416=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp414)->f1;_LL25:
 return _tmp416;}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp740=e;Cyc_Tcutil_copye(_tmp740,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp415=_cycalloc(sizeof(*_tmp415));_tmp415->tag=39U,_tmp415->f1=_tmp412;_tmp415;}));});}_LL23:;};}default: _LL21: _LL22:
# 2558
 return({void*_tmp417=0U;({struct _fat_ptr _tmp741=({const char*_tmp418="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp418,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp741,_tag_fat(_tmp417,sizeof(void*),0U));});});}_LL0:;}
# 2562
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2565
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 2569
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2572
void*_tmp42E=f->type;
struct Cyc_Absyn_Exp*_tmp42F=f->requires_clause;
void*_tmp430=Cyc_Tcutil_rsubstitute(r,inst,_tmp42E);
struct Cyc_Absyn_Exp*_tmp431=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp42F);
if(_tmp42E == _tmp430 && _tmp42F == _tmp431)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->name=f->name,_tmp432->tq=f->tq,_tmp432->type=_tmp430,_tmp432->width=f->width,_tmp432->attributes=f->attributes,_tmp432->requires_clause=_tmp431;_tmp432;});}}
# 2582
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2585
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp433=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp434=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp435=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp433);
struct Cyc_List_List*_tmp436=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp434);
if(_tmp435 == _tmp433 && _tmp436 == _tmp434)return fs;
return({struct Cyc_List_List*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->hd=_tmp435,_tmp437->tl=_tmp436;_tmp437;});};}
# 2594
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2597
struct _tuple0 _tmp438=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _tmp439=_tmp438;struct Cyc_List_List*_tmp43D;struct Cyc_List_List*_tmp43C;_LL1: _tmp43D=_tmp439.f1;_tmp43C=_tmp439.f2;_LL2:;{
struct Cyc_List_List*_tmp43A=Cyc_Tcutil_substs(rgn,inst,_tmp43D);
struct Cyc_List_List*_tmp43B=Cyc_Tcutil_substs(rgn,inst,_tmp43C);
if(_tmp43A == _tmp43D && _tmp43B == _tmp43C)
return rgn_po;else{
# 2603
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp43A,_tmp43B);}};}
# 2606
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2609
void*_tmp43E=Cyc_Tcutil_compress(t);void*_tmp43F=_tmp43E;struct Cyc_Absyn_Exp*_tmp496;struct Cyc_Absyn_Exp*_tmp495;void*_tmp494;struct Cyc_List_List*_tmp493;void*_tmp492;void*_tmp491;enum Cyc_Absyn_AggrKind _tmp490;struct Cyc_List_List*_tmp48F;struct Cyc_List_List*_tmp48E;struct Cyc_List_List*_tmp48D;void*_tmp48C;struct Cyc_Absyn_Tqual _tmp48B;void*_tmp48A;struct Cyc_List_List*_tmp489;int _tmp488;struct Cyc_Absyn_VarargInfo*_tmp487;struct Cyc_List_List*_tmp486;struct Cyc_List_List*_tmp485;struct Cyc_Absyn_Exp*_tmp484;struct Cyc_Absyn_Exp*_tmp483;void*_tmp482;struct Cyc_Absyn_Tqual _tmp481;void*_tmp480;void*_tmp47F;void*_tmp47E;void*_tmp47D;void*_tmp47C;struct Cyc_Absyn_Tqual _tmp47B;struct Cyc_Absyn_Exp*_tmp47A;void*_tmp479;unsigned _tmp478;struct _tuple1*_tmp477;struct Cyc_List_List*_tmp476;struct Cyc_Absyn_Typedefdecl*_tmp475;void*_tmp474;struct Cyc_Absyn_Tvar*_tmp473;switch(*((int*)_tmp43F)){case 2U: _LL1: _tmp473=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp43F)->f1;_LL2: {
# 2612
struct _handler_cons _tmp440;_push_handler(& _tmp440);{int _tmp442=0;if(setjmp(_tmp440.handler))_tmp442=1;if(!_tmp442){{void*_tmp443=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp473);_npop_handler(0U);return _tmp443;};_pop_handler();}else{void*_tmp441=(void*)Cyc_Core_get_exn_thrown();void*_tmp444=_tmp441;void*_tmp445;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp444)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp445=_tmp444;_LL1F:(int)_rethrow(_tmp445);}_LL1B:;}};}case 8U: _LL3: _tmp477=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43F)->f1;_tmp476=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43F)->f2;_tmp475=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43F)->f3;_tmp474=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp43F)->f4;_LL4: {
# 2615
struct Cyc_List_List*_tmp446=Cyc_Tcutil_substs(rgn,inst,_tmp476);
return _tmp446 == _tmp476?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=8U,_tmp447->f1=_tmp477,_tmp447->f2=_tmp446,_tmp447->f3=_tmp475,_tmp447->f4=_tmp474;_tmp447;});}case 4U: _LL5: _tmp47C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).elt_type;_tmp47B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).tq;_tmp47A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).num_elts;_tmp479=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).zero_term;_tmp478=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp43F)->f1).zt_loc;_LL6: {
# 2618
void*_tmp448=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp47C);
struct Cyc_Absyn_Exp*_tmp449=_tmp47A == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp47A);
void*_tmp44A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp479);
return(_tmp448 == _tmp47C && _tmp449 == _tmp47A)&& _tmp44A == _tmp479?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));
_tmp44B->tag=4U,(_tmp44B->f1).elt_type=_tmp448,(_tmp44B->f1).tq=_tmp47B,(_tmp44B->f1).num_elts=_tmp449,(_tmp44B->f1).zero_term=_tmp44A,(_tmp44B->f1).zt_loc=_tmp478;_tmp44B;});}case 3U: _LL7: _tmp482=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).elt_type;_tmp481=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).elt_tq;_tmp480=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).rgn;_tmp47F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).nullable;_tmp47E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).bounds;_tmp47D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp43F)->f1).ptr_atts).zero_term;_LL8: {
# 2624
void*_tmp44C=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp482);
void*_tmp44D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp480);
void*_tmp44E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp47E);
void*_tmp44F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp47D);
if(((_tmp44C == _tmp482 && _tmp44D == _tmp480)&& _tmp44E == _tmp47E)&& _tmp44F == _tmp47D)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp624;_tmp624.elt_type=_tmp44C,_tmp624.elt_tq=_tmp481,(_tmp624.ptr_atts).rgn=_tmp44D,(_tmp624.ptr_atts).nullable=_tmp47F,(_tmp624.ptr_atts).bounds=_tmp44E,(_tmp624.ptr_atts).zero_term=_tmp44F,(_tmp624.ptr_atts).ptrloc=0;_tmp624;}));}case 5U: _LL9: _tmp48D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).tvars;_tmp48C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).effect;_tmp48B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ret_tqual;_tmp48A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ret_type;_tmp489=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).args;_tmp488=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).c_varargs;_tmp487=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).cyc_varargs;_tmp486=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).rgn_po;_tmp485=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).attributes;_tmp484=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).requires_clause;_tmp483=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp43F)->f1).ensures_clause;_LLA:
# 2634
{struct Cyc_List_List*_tmp450=_tmp48D;for(0;_tmp450 != 0;_tmp450=_tmp450->tl){
inst=({struct Cyc_List_List*_tmp452=_region_malloc(rgn,sizeof(*_tmp452));({struct _tuple19*_tmp743=({struct _tuple19*_tmp451=_region_malloc(rgn,sizeof(*_tmp451));_tmp451->f1=(struct Cyc_Absyn_Tvar*)_tmp450->hd,({void*_tmp742=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp450->hd);_tmp451->f2=_tmp742;});_tmp451;});_tmp452->hd=_tmp743;}),_tmp452->tl=inst;_tmp452;});}}{
struct _tuple0 _tmp453=({struct _RegionHandle*_tmp745=rgn;struct _RegionHandle*_tmp744=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp745,_tmp744,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp489));});
# 2636
struct _tuple0 _tmp454=_tmp453;struct Cyc_List_List*_tmp463;struct Cyc_List_List*_tmp462;_LL21: _tmp463=_tmp454.f1;_tmp462=_tmp454.f2;_LL22:;{
# 2638
struct Cyc_List_List*_tmp455=_tmp489;
struct Cyc_List_List*_tmp456=Cyc_Tcutil_substs(rgn,inst,_tmp462);
if(_tmp456 != _tmp462)
_tmp455=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp489,_tmp456);{
void*eff2;
if(_tmp48C == 0)
eff2=0;else{
# 2646
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp48C);
if(new_eff == _tmp48C)
eff2=_tmp48C;else{
# 2650
eff2=new_eff;}}{
# 2652
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp487 == 0)
cyc_varargs2=0;else{
# 2656
struct Cyc_Absyn_VarargInfo _tmp457=*_tmp487;struct Cyc_Absyn_VarargInfo _tmp458=_tmp457;struct _fat_ptr*_tmp45E;struct Cyc_Absyn_Tqual _tmp45D;void*_tmp45C;int _tmp45B;_LL24: _tmp45E=_tmp458.name;_tmp45D=_tmp458.tq;_tmp45C=_tmp458.type;_tmp45B=_tmp458.inject;_LL25:;{
void*_tmp459=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp45C);
if(_tmp459 == _tmp45C)cyc_varargs2=_tmp487;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp45A=_cycalloc(sizeof(*_tmp45A));_tmp45A->name=_tmp45E,_tmp45A->tq=_tmp45D,_tmp45A->type=_tmp459,_tmp45A->inject=_tmp45B;_tmp45A;});}};}{
# 2661
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp486);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp484);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp483);
struct Cyc_List_List*_tmp45F=Cyc_Tcutil_extract_relns(_tmp455,req2);
struct Cyc_List_List*_tmp460=Cyc_Tcutil_extract_relns(_tmp455,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp461=_cycalloc(sizeof(*_tmp461));_tmp461->tag=5U,(_tmp461->f1).tvars=_tmp48D,(_tmp461->f1).effect=eff2,(_tmp461->f1).ret_tqual=_tmp48B,({void*_tmp746=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp48A);(_tmp461->f1).ret_type=_tmp746;}),(_tmp461->f1).args=_tmp455,(_tmp461->f1).c_varargs=_tmp488,(_tmp461->f1).cyc_varargs=cyc_varargs2,(_tmp461->f1).rgn_po=rgn_po2,(_tmp461->f1).attributes=_tmp485,(_tmp461->f1).requires_clause=req2,(_tmp461->f1).requires_relns=_tmp45F,(_tmp461->f1).ensures_clause=ens2,(_tmp461->f1).ensures_relns=_tmp460;_tmp461;});};};};};};case 6U: _LLB: _tmp48E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp43F)->f1;_LLC: {
# 2670
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp464=_tmp48E;for(0;_tmp464 != 0;_tmp464=_tmp464->tl){
void*_tmp465=(*((struct _tuple12*)_tmp464->hd)).f2;
void*_tmp466=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp465);
if(_tmp465 != _tmp466)
change=1;
# 2678
ts2=({struct Cyc_List_List*_tmp467=_region_malloc(rgn,sizeof(*_tmp467));_tmp467->hd=_tmp466,_tmp467->tl=ts2;_tmp467;});}}
# 2680
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp468=_cycalloc(sizeof(*_tmp468));_tmp468->tag=6U,({struct Cyc_List_List*_tmp748=({struct Cyc_List_List*_tmp747=_tmp48E;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp747,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp468->f1=_tmp748;});_tmp468;});}case 7U: _LLD: _tmp490=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp43F)->f1;_tmp48F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp43F)->f2;_LLE: {
# 2684
struct Cyc_List_List*_tmp469=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp48F);
if(_tmp48F == _tmp469)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp46A=_cycalloc(sizeof(*_tmp46A));_tmp46A->tag=7U,_tmp46A->f1=_tmp490,_tmp46A->f2=_tmp469;_tmp46A;});}case 1U: _LLF: _tmp491=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp43F)->f2;_LL10:
# 2688
 if(_tmp491 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp491);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43F)->f2 == 0){_LL11: _tmp492=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43F)->f1;_LL12:
 return t;}else{_LL13: _tmp494=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43F)->f1;_tmp493=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp43F)->f2;_LL14: {
# 2692
struct Cyc_List_List*_tmp46B=Cyc_Tcutil_substs(rgn,inst,_tmp493);
if(_tmp493 == _tmp46B)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp46C=_cycalloc(sizeof(*_tmp46C));_tmp46C->tag=0U,_tmp46C->f1=_tmp494,_tmp46C->f2=_tmp46B;_tmp46C;});}}}case 9U: _LL15: _tmp495=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp43F)->f1;_LL16: {
# 2696
struct Cyc_Absyn_Exp*_tmp46D=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp495);
return _tmp46D == _tmp495?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp46E=_cycalloc(sizeof(*_tmp46E));_tmp46E->tag=9U,_tmp46E->f1=_tmp46D;_tmp46E;});}case 11U: _LL17: _tmp496=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp43F)->f1;_LL18: {
# 2699
struct Cyc_Absyn_Exp*_tmp46F=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp496);
return _tmp46F == _tmp496?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp470=_cycalloc(sizeof(*_tmp470));_tmp470->tag=11U,_tmp470->f1=_tmp46F;_tmp470;});}default: _LL19: _LL1A:
({void*_tmp471=0U;({struct _fat_ptr _tmp749=({const char*_tmp472="found typedecltype in rsubs";_tag_fat(_tmp472,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp749,_tag_fat(_tmp471,sizeof(void*),0U));});});}_LL0:;}
# 2705
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2708
if(ts == 0)
return 0;{
void*_tmp497=(void*)ts->hd;
struct Cyc_List_List*_tmp498=ts->tl;
void*_tmp499=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp497);
struct Cyc_List_List*_tmp49A=Cyc_Tcutil_substs(rgn,inst,_tmp498);
if(_tmp497 == _tmp499 && _tmp498 == _tmp49A)
return ts;
return({struct Cyc_List_List*_tmp49B=_cycalloc(sizeof(*_tmp49B));_tmp49B->hd=_tmp499,_tmp49B->tl=_tmp49A;_tmp49B;});};}
# 2719
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 2726
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp49C=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple19*_tmp49E=_cycalloc(sizeof(*_tmp49E));_tmp49E->f1=tv,({void*_tmp74B=({struct Cyc_Core_Opt*_tmp74A=_tmp49C;Cyc_Absyn_new_evar(_tmp74A,({struct Cyc_Core_Opt*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->v=s;_tmp49D;}));});_tmp49E->f2=_tmp74B;});_tmp49E;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2731
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2733
struct _tuple28*_tmp49F=env;struct Cyc_List_List*_tmp4A4;struct _RegionHandle*_tmp4A3;_LL1: _tmp4A4=_tmp49F->f1;_tmp4A3=_tmp49F->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp4A0=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple19*_tmp4A2=_region_malloc(_tmp4A3,sizeof(*_tmp4A2));_tmp4A2->f1=tv,({void*_tmp74D=({struct Cyc_Core_Opt*_tmp74C=_tmp4A0;Cyc_Absyn_new_evar(_tmp74C,({struct Cyc_Core_Opt*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->v=_tmp4A4;_tmp4A1;}));});_tmp4A2->f2=_tmp74D;});_tmp4A2;});};}
# 2738
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2740
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp4A7=({struct Cyc_String_pa_PrintArg_struct _tmp625;_tmp625.tag=0U,_tmp625.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp625;});void*_tmp4A5[1U];_tmp4A5[0]=& _tmp4A7;({unsigned _tmp74F=loc;struct _fat_ptr _tmp74E=({const char*_tmp4A6="bitfield %s does not have constant width";_tag_fat(_tmp4A6,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp74F,_tmp74E,_tag_fat(_tmp4A5,sizeof(void*),1U));});});else{
# 2745
struct _tuple13 _tmp4A8=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _tmp4A9=_tmp4A8;unsigned _tmp4AF;int _tmp4AE;_LL1: _tmp4AF=_tmp4A9.f1;_tmp4AE=_tmp4A9.f2;_LL2:;
if(!_tmp4AE)
({void*_tmp4AA=0U;({unsigned _tmp751=loc;struct _fat_ptr _tmp750=({const char*_tmp4AB="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4AB,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp751,_tmp750,_tag_fat(_tmp4AA,sizeof(void*),0U));});});
if((int)_tmp4AF < 0)
({void*_tmp4AC=0U;({unsigned _tmp753=loc;struct _fat_ptr _tmp752=({const char*_tmp4AD="bitfield has negative width";_tag_fat(_tmp4AD,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp753,_tmp752,_tag_fat(_tmp4AC,sizeof(void*),0U));});});
w=_tmp4AF;}{
# 2752
void*_tmp4B0=Cyc_Tcutil_compress(field_type);void*_tmp4B1=_tmp4B0;enum Cyc_Absyn_Size_of _tmp4BF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->f1)->tag == 1U){_LL4: _tmp4BF=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B1)->f1)->f2;_LL5:
# 2755
{enum Cyc_Absyn_Size_of _tmp4B2=_tmp4BF;switch(_tmp4B2){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp4B3=0U;({unsigned _tmp755=loc;struct _fat_ptr _tmp754=({const char*_tmp4B4="bitfield larger than type";_tag_fat(_tmp4B4,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp755,_tmp754,_tag_fat(_tmp4B3,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp4B5=0U;({unsigned _tmp757=loc;struct _fat_ptr _tmp756=({const char*_tmp4B6="bitfield larger than type";_tag_fat(_tmp4B6,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp757,_tmp756,_tag_fat(_tmp4B5,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp4B7=0U;({unsigned _tmp759=loc;struct _fat_ptr _tmp758=({const char*_tmp4B8="bitfield larger than type";_tag_fat(_tmp4B8,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp759,_tmp758,_tag_fat(_tmp4B7,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp4B9=0U;({unsigned _tmp75B=loc;struct _fat_ptr _tmp75A=({const char*_tmp4BA="bitfield larger than type";_tag_fat(_tmp4BA,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp75B,_tmp75A,_tag_fat(_tmp4B9,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 2763
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 2765
({struct Cyc_String_pa_PrintArg_struct _tmp4BD=({struct Cyc_String_pa_PrintArg_struct _tmp627;_tmp627.tag=0U,_tmp627.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp627;});struct Cyc_String_pa_PrintArg_struct _tmp4BE=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({
struct _fat_ptr _tmp75C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field_type));_tmp626.f1=_tmp75C;});_tmp626;});void*_tmp4BB[2U];_tmp4BB[0]=& _tmp4BD,_tmp4BB[1]=& _tmp4BE;({unsigned _tmp75E=loc;struct _fat_ptr _tmp75D=({const char*_tmp4BC="bitfield %s must have integral type but has type %s";_tag_fat(_tmp4BC,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp75E,_tmp75D,_tag_fat(_tmp4BB,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 2774
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4C0=t;struct Cyc_Absyn_Typedefdecl*_tmp4C4;void*_tmp4C3;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C0)->tag == 8U){_LL1: _tmp4C4=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C0)->f3;_tmp4C3=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C0)->f4;_LL2:
# 2777
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp4C4))->tq).real_const ||(_tmp4C4->tq).print_const){
if(declared_const)({void*_tmp4C1=0U;({unsigned _tmp760=loc;struct _fat_ptr _tmp75F=({const char*_tmp4C2="extra const";_tag_fat(_tmp4C2,sizeof(char),12U);});Cyc_Tcutil_warn(_tmp760,_tmp75F,_tag_fat(_tmp4C1,sizeof(void*),0U));});});
return 1;}
# 2782
if((unsigned)_tmp4C3)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp4C3);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2789
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp761=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp761;});}
# 2793
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2799
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2802
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4C7=({struct Cyc_String_pa_PrintArg_struct _tmp629;_tmp629.tag=0U,_tmp629.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp629;});struct Cyc_String_pa_PrintArg_struct _tmp4C8=({struct Cyc_String_pa_PrintArg_struct _tmp628;_tmp628.tag=0U,({struct _fat_ptr _tmp762=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp628.f1=_tmp762;});_tmp628;});void*_tmp4C5[2U];_tmp4C5[0]=& _tmp4C7,_tmp4C5[1]=& _tmp4C8;({unsigned _tmp764=loc;struct _fat_ptr _tmp763=({const char*_tmp4C6="%s: %s";_tag_fat(_tmp4C6,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp764,_tmp763,_tag_fat(_tmp4C5,sizeof(void*),2U));});});}}}
# 2808
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2812
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2816
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp766=tvs;unsigned _tmp765=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp766,_tmp765,Cyc_Absynpp_tvar2string,({const char*_tmp4C9="duplicate type variable";_tag_fat(_tmp4C9,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2830 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2835
struct _RegionHandle _tmp4CA=_new_region("temp");struct _RegionHandle*temp=& _tmp4CA;_push_region(temp);
# 2839
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp767=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp767,({const char*_tmp4CB="";_tag_fat(_tmp4CB,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4CD=_region_malloc(temp,sizeof(*_tmp4CD));({struct _tuple29*_tmp768=({struct _tuple29*_tmp4CC=_region_malloc(temp,sizeof(*_tmp4CC));_tmp4CC->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4CC->f2=0;_tmp4CC;});_tmp4CD->hd=_tmp768;}),_tmp4CD->tl=fields;_tmp4CD;});}}
# 2844
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2846
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp4F2="struct";_tag_fat(_tmp4F2,sizeof(char),7U);}):({const char*_tmp4F3="union";_tag_fat(_tmp4F3,sizeof(char),6U);});
# 2849
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4CE=(struct _tuple30*)des->hd;struct _tuple30*_tmp4CF=_tmp4CE;struct Cyc_List_List*_tmp4EA;void*_tmp4E9;_LL1: _tmp4EA=_tmp4CF->f1;_tmp4E9=_tmp4CF->f2;_LL2:;
if(_tmp4EA == 0){
# 2854
struct Cyc_List_List*_tmp4D0=fields;
for(0;_tmp4D0 != 0;_tmp4D0=_tmp4D0->tl){
if(!(*((struct _tuple29*)_tmp4D0->hd)).f2){
(*((struct _tuple29*)_tmp4D0->hd)).f2=1;
({struct Cyc_List_List*_tmp76A=({struct Cyc_List_List*_tmp4D2=_cycalloc(sizeof(*_tmp4D2));({void*_tmp769=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4D1=_cycalloc(sizeof(*_tmp4D1));_tmp4D1->tag=1U,_tmp4D1->f1=((*((struct _tuple29*)_tmp4D0->hd)).f1)->name;_tmp4D1;});_tmp4D2->hd=_tmp769;}),_tmp4D2->tl=0;_tmp4D2;});(*((struct _tuple30*)des->hd)).f1=_tmp76A;});
ans=({struct Cyc_List_List*_tmp4D4=_region_malloc(rgn,sizeof(*_tmp4D4));({struct _tuple31*_tmp76B=({struct _tuple31*_tmp4D3=_region_malloc(rgn,sizeof(*_tmp4D3));_tmp4D3->f1=(*((struct _tuple29*)_tmp4D0->hd)).f1,_tmp4D3->f2=_tmp4E9;_tmp4D3;});_tmp4D4->hd=_tmp76B;}),_tmp4D4->tl=ans;_tmp4D4;});
break;}}
# 2862
if(_tmp4D0 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4D7=({struct Cyc_String_pa_PrintArg_struct _tmp62A;_tmp62A.tag=0U,_tmp62A.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp62A;});void*_tmp4D5[1U];_tmp4D5[0]=& _tmp4D7;({unsigned _tmp76D=loc;struct _fat_ptr _tmp76C=({const char*_tmp4D6="too many arguments to %s";_tag_fat(_tmp4D6,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp76D,_tmp76C,_tag_fat(_tmp4D5,sizeof(void*),1U));});});}else{
if(_tmp4EA->tl != 0)
# 2866
({void*_tmp4D8=0U;({unsigned _tmp76F=loc;struct _fat_ptr _tmp76E=({const char*_tmp4D9="multiple designators are not yet supported";_tag_fat(_tmp4D9,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp76F,_tmp76E,_tag_fat(_tmp4D8,sizeof(void*),0U));});});else{
# 2869
void*_tmp4DA=(void*)_tmp4EA->hd;void*_tmp4DB=_tmp4DA;struct _fat_ptr*_tmp4E8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4DB)->tag == 0U){_LL4: _LL5:
# 2871
({struct Cyc_String_pa_PrintArg_struct _tmp4DE=({struct Cyc_String_pa_PrintArg_struct _tmp62B;_tmp62B.tag=0U,_tmp62B.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp62B;});void*_tmp4DC[1U];_tmp4DC[0]=& _tmp4DE;({unsigned _tmp771=loc;struct _fat_ptr _tmp770=({const char*_tmp4DD="array designator used in argument to %s";_tag_fat(_tmp4DD,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp771,_tmp770,_tag_fat(_tmp4DC,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp4E8=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4DB)->f1;_LL7: {
# 2874
struct Cyc_List_List*_tmp4DF=fields;
for(0;_tmp4DF != 0;_tmp4DF=_tmp4DF->tl){
if(Cyc_strptrcmp(_tmp4E8,((*((struct _tuple29*)_tmp4DF->hd)).f1)->name)== 0){
if((*((struct _tuple29*)_tmp4DF->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp4E2=({struct Cyc_String_pa_PrintArg_struct _tmp62C;_tmp62C.tag=0U,_tmp62C.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp4E8);_tmp62C;});void*_tmp4E0[1U];_tmp4E0[0]=& _tmp4E2;({unsigned _tmp773=loc;struct _fat_ptr _tmp772=({const char*_tmp4E1="member %s has already been used as an argument";_tag_fat(_tmp4E1,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp773,_tmp772,_tag_fat(_tmp4E0,sizeof(void*),1U));});});
(*((struct _tuple29*)_tmp4DF->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4E4=_region_malloc(rgn,sizeof(*_tmp4E4));({struct _tuple31*_tmp774=({struct _tuple31*_tmp4E3=_region_malloc(rgn,sizeof(*_tmp4E3));_tmp4E3->f1=(*((struct _tuple29*)_tmp4DF->hd)).f1,_tmp4E3->f2=_tmp4E9;_tmp4E3;});_tmp4E4->hd=_tmp774;}),_tmp4E4->tl=ans;_tmp4E4;});
break;}}
# 2883
if(_tmp4DF == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4E7=({struct Cyc_String_pa_PrintArg_struct _tmp62D;_tmp62D.tag=0U,_tmp62D.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp4E8);_tmp62D;});void*_tmp4E5[1U];_tmp4E5[0]=& _tmp4E7;({unsigned _tmp776=loc;struct _fat_ptr _tmp775=({const char*_tmp4E6="bad field designator %s";_tag_fat(_tmp4E6,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp776,_tmp775,_tag_fat(_tmp4E5,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 2888
if((int)aggr_kind == (int)0U)
# 2890
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple29*)fields->hd)).f2){
({void*_tmp4EB=0U;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp4EC="too few arguments to struct";_tag_fat(_tmp4EC,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp778,_tmp777,_tag_fat(_tmp4EB,sizeof(void*),0U));});});
break;}}else{
# 2897
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple29*)fields->hd)).f2){
if(found)({void*_tmp4ED=0U;({unsigned _tmp77A=loc;struct _fat_ptr _tmp779=({const char*_tmp4EE="only one member of a union is allowed";_tag_fat(_tmp4EE,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_fat(_tmp4ED,sizeof(void*),0U));});});
found=1;}}
# 2903
if(!found)({void*_tmp4EF=0U;({unsigned _tmp77C=loc;struct _fat_ptr _tmp77B=({const char*_tmp4F0="missing member for union";_tag_fat(_tmp4F0,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp77C,_tmp77B,_tag_fat(_tmp4EF,sizeof(void*),0U));});});}{
# 2906
struct Cyc_List_List*_tmp4F1=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp4F1;};};}
# 2839
;_pop_region(temp);}
# 2912
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2914
void*_tmp4F4=e1->r;void*_tmp4F5=_tmp4F4;struct Cyc_Absyn_Exp*_tmp507;struct Cyc_Absyn_Exp*_tmp506;struct Cyc_Absyn_Exp*_tmp505;struct Cyc_Absyn_Exp*_tmp504;struct Cyc_Absyn_Exp*_tmp503;struct Cyc_Absyn_Exp*_tmp502;switch(*((int*)_tmp4F5)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp4F8=({struct Cyc_String_pa_PrintArg_struct _tmp62E;_tmp62E.tag=0U,({struct _fat_ptr _tmp77D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp62E.f1=_tmp77D;});_tmp62E;});void*_tmp4F6[1U];_tmp4F6[0]=& _tmp4F8;({struct _fat_ptr _tmp77E=({const char*_tmp4F7="we have a cast in a lhs:  %s";_tag_fat(_tmp4F7,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp77E,_tag_fat(_tmp4F6,sizeof(void*),1U));});});case 20U: _LL3: _tmp502=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LL4:
 _tmp503=_tmp502;goto _LL6;case 23U: _LL5: _tmp503=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LL6:
# 2918
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp503->topt),ptr_type,is_fat,elt_type);case 22U: _LL7: _tmp504=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LL8:
 _tmp505=_tmp504;goto _LLA;case 21U: _LL9: _tmp505=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LLA:
# 2922
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp505->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp4FB=({struct Cyc_String_pa_PrintArg_struct _tmp62F;_tmp62F.tag=0U,({
struct _fat_ptr _tmp77F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp62F.f1=_tmp77F;});_tmp62F;});void*_tmp4F9[1U];_tmp4F9[0]=& _tmp4FB;({struct _fat_ptr _tmp780=({const char*_tmp4FA="found zero pointer aggregate member assignment: %s";_tag_fat(_tmp4FA,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp780,_tag_fat(_tmp4F9,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp506=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LLC:
 _tmp507=_tmp506;goto _LLE;case 12U: _LLD: _tmp507=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4F5)->f1;_LLE:
# 2928
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp507->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp4FE=({struct Cyc_String_pa_PrintArg_struct _tmp630;_tmp630.tag=0U,({
struct _fat_ptr _tmp781=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp630.f1=_tmp781;});_tmp630;});void*_tmp4FC[1U];_tmp4FC[0]=& _tmp4FE;({struct _fat_ptr _tmp782=({const char*_tmp4FD="found zero pointer instantiate/noinstantiate: %s";_tag_fat(_tmp4FD,sizeof(char),49U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp782,_tag_fat(_tmp4FC,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp501=({struct Cyc_String_pa_PrintArg_struct _tmp631;_tmp631.tag=0U,({struct _fat_ptr _tmp783=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp631.f1=_tmp783;});_tmp631;});void*_tmp4FF[1U];_tmp4FF[0]=& _tmp501;({struct _fat_ptr _tmp784=({const char*_tmp500="found bad lhs in is_zero_ptr_deref: %s";_tag_fat(_tmp500,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp784,_tag_fat(_tmp4FF,sizeof(void*),1U));});});}_LL0:;}
# 2943
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 2946
void*_tmp508=Cyc_Tcutil_compress(r);void*_tmp509=_tmp508;struct Cyc_Absyn_Tvar*_tmp515;enum Cyc_Absyn_KindQual _tmp514;enum Cyc_Absyn_AliasQual _tmp513;switch(*((int*)_tmp509)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp509)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f4 == 0){_LL5: _tmp514=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f3)->kind)->v)->kind;_tmp513=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp509)->f3)->kind)->v)->aliasqual;_LL6:
# 2950
 return(int)_tmp514 == (int)3U &&((int)_tmp513 == (int)1U ||(int)_tmp513 == (int)2U && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp515=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp509)->f1;_LL8: {
# 2954
struct Cyc_Absyn_Kind*_tmp50A=Cyc_Tcutil_tvar_kind(_tmp515,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp50B=_tmp50A;enum Cyc_Absyn_KindQual _tmp512;enum Cyc_Absyn_AliasQual _tmp511;_LLC: _tmp512=_tmp50B->kind;_tmp511=_tmp50B->aliasqual;_LLD:;
if((int)_tmp512 == (int)3U &&((int)_tmp511 == (int)1U ||(int)_tmp511 == (int)2U && !must_be_unique)){
void*_tmp50C=Cyc_Absyn_compress_kb(_tmp515->kind);void*_tmp50D=_tmp50C;struct Cyc_Core_Opt**_tmp510;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp50D)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp50D)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp50D)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp510=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp50D)->f1;_LL10:
# 2958
({struct Cyc_Core_Opt*_tmp786=({struct Cyc_Core_Opt*_tmp50F=_cycalloc(sizeof(*_tmp50F));({void*_tmp785=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp50E=_cycalloc(sizeof(*_tmp50E));_tmp50E->tag=2U,_tmp50E->f1=0,_tmp50E->f2=& Cyc_Tcutil_rk;_tmp50E;});_tmp50F->v=_tmp785;});_tmp50F;});*_tmp510=_tmp786;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2963
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2970
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp516=Cyc_Tcutil_compress(t);void*_tmp517=_tmp516;struct Cyc_Absyn_Tvar*_tmp525;void*_tmp524;switch(*((int*)_tmp517)){case 3U: _LL1: _tmp524=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp517)->f1).ptr_atts).rgn;_LL2:
# 2973
 return Cyc_Tcutil_is_noalias_region(_tmp524,must_be_unique);case 2U: _LL3: _tmp525=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp517)->f1;_LL4: {
# 2975
struct Cyc_Absyn_Kind*_tmp518=Cyc_Tcutil_tvar_kind(_tmp525,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp519=_tmp518;enum Cyc_Absyn_KindQual _tmp523;enum Cyc_Absyn_AliasQual _tmp522;_LL8: _tmp523=_tmp519->kind;_tmp522=_tmp519->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp51A=_tmp523;switch(_tmp51A){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)_tmp522 == (int)1U ||(int)_tmp522 == (int)2U && !must_be_unique){
void*_tmp51B=Cyc_Absyn_compress_kb(_tmp525->kind);void*_tmp51C=_tmp51B;struct Cyc_Core_Opt**_tmp521;enum Cyc_Absyn_KindQual _tmp520;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51C)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51C)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp521=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51C)->f1;_tmp520=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp51C)->f2)->kind;_LL15:
# 2981
({struct Cyc_Core_Opt*_tmp789=({struct Cyc_Core_Opt*_tmp51F=_cycalloc(sizeof(*_tmp51F));({void*_tmp788=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->tag=2U,_tmp51E->f1=0,({struct Cyc_Absyn_Kind*_tmp787=({struct Cyc_Absyn_Kind*_tmp51D=_cycalloc(sizeof(*_tmp51D));_tmp51D->kind=_tmp520,_tmp51D->aliasqual=Cyc_Absyn_Aliasable;_tmp51D;});_tmp51E->f2=_tmp787;});_tmp51E;});_tmp51F->v=_tmp788;});_tmp51F;});*_tmp521=_tmp789;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 2985
 return 1;}_LL13:;}
# 2988
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 2991
 return 0;}_LL0:;}
# 2994
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp526=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp526,0))return 1;{
void*_tmp527=_tmp526;struct Cyc_List_List*_tmp53B;union Cyc_Absyn_DatatypeFieldInfo _tmp53A;struct Cyc_List_List*_tmp539;union Cyc_Absyn_DatatypeInfo _tmp538;struct Cyc_List_List*_tmp537;struct Cyc_Absyn_Aggrdecl**_tmp536;struct Cyc_List_List*_tmp535;struct Cyc_List_List*_tmp534;switch(*((int*)_tmp527)){case 6U: _LL1: _tmp534=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp527)->f1;_LL2:
# 2999
 while(_tmp534 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)_tmp534->hd)).f2))return 1;
_tmp534=_tmp534->tl;}
# 3003
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp536=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)->f1).KnownAggr).val;_tmp535=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f2;_LL4:
# 3005
 if((*_tmp536)->impl == 0)return 0;else{
# 3007
struct Cyc_List_List*_tmp528=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp536)->tvs,_tmp535);
struct Cyc_List_List*_tmp529=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp536)->impl))->fields;
void*t;
while(_tmp529 != 0){
t=_tmp528 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp529->hd)->type: Cyc_Tcutil_substitute(_tmp528,((struct Cyc_Absyn_Aggrfield*)_tmp529->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp529=_tmp529->tl;}
# 3015
return 0;}}else{_LL7: _LL8:
# 3025
 return 0;}case 18U: _LL9: _tmp538=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)->f1;_tmp537=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f2;_LLA: {
# 3027
union Cyc_Absyn_DatatypeInfo _tmp52A=_tmp538;struct Cyc_List_List*_tmp52E;struct Cyc_Core_Opt*_tmp52D;struct _tuple1*_tmp52C;int _tmp52B;if((_tmp52A.UnknownDatatype).tag == 1){_LL10: _tmp52C=((_tmp52A.UnknownDatatype).val).name;_tmp52B=((_tmp52A.UnknownDatatype).val).is_extensible;_LL11:
# 3030
 return 0;}else{_LL12: _tmp52E=(*(_tmp52A.KnownDatatype).val)->tvs;_tmp52D=(*(_tmp52A.KnownDatatype).val)->fields;_LL13:
# 3033
 return 0;}_LLF:;}case 19U: _LLB: _tmp53A=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f1)->f1;_tmp539=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp527)->f2;_LLC: {
# 3036
union Cyc_Absyn_DatatypeFieldInfo _tmp52F=_tmp53A;struct Cyc_Absyn_Datatypedecl*_tmp533;struct Cyc_Absyn_Datatypefield*_tmp532;if((_tmp52F.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 3039
 return 0;}else{_LL17: _tmp533=((_tmp52F.KnownDatatypefield).val).f1;_tmp532=((_tmp52F.KnownDatatypefield).val).f2;_LL18: {
# 3041
struct Cyc_List_List*_tmp530=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp533->tvs,_tmp539);
struct Cyc_List_List*_tmp531=_tmp532->typs;
while(_tmp531 != 0){
_tmp526=_tmp530 == 0?(*((struct _tuple12*)_tmp531->hd)).f2: Cyc_Tcutil_substitute(_tmp530,(*((struct _tuple12*)_tmp531->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp526))return 1;
_tmp531=_tmp531->tl;}
# 3048
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp53B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp527)->f2;_LL6:
# 3018
 while(_tmp53B != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp53B->hd)->type))return 1;
_tmp53B=_tmp53B->tl;}
# 3022
return 0;default: _LLD: _LLE:
# 3050
 return 0;}_LL0:;};}
# 3057
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp53C=e->r;void*_tmp53D=_tmp53C;struct Cyc_Absyn_Stmt*_tmp553;struct Cyc_Absyn_Exp*_tmp552;struct Cyc_Absyn_Exp*_tmp551;struct Cyc_Absyn_Exp*_tmp550;struct Cyc_Absyn_Exp*_tmp54F;struct Cyc_Absyn_Exp*_tmp54E;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54C;struct _fat_ptr*_tmp54B;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp549;switch(*((int*)_tmp53D)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp53D)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp549=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_LL4:
 _tmp54A=_tmp549;goto _LL6;case 20U: _LL5: _tmp54A=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_LL6:
# 3062
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp54A->topt),1)&& Cyc_Tcutil_is_noalias_path(_tmp54A);case 21U: _LL7: _tmp54C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_tmp54B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp54C);case 23U: _LL9: _tmp54E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_tmp54D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_LLA: {
# 3065
void*_tmp53E=Cyc_Tcutil_compress((void*)_check_null(_tmp54E->topt));void*_tmp53F=_tmp53E;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp53F)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp54E);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp550=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_tmp54F=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp53D)->f3;_LLC:
# 3070
 return Cyc_Tcutil_is_noalias_path(_tmp550)&& Cyc_Tcutil_is_noalias_path(_tmp54F);case 9U: _LLD: _tmp551=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_LLE:
 _tmp552=_tmp551;goto _LL10;case 14U: _LLF: _tmp552=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp53D)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp552);case 37U: _LL11: _tmp553=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp53D)->f1;_LL12:
# 3074
 while(1){
void*_tmp540=_tmp553->r;void*_tmp541=_tmp540;struct Cyc_Absyn_Exp*_tmp548;struct Cyc_Absyn_Decl*_tmp547;struct Cyc_Absyn_Stmt*_tmp546;struct Cyc_Absyn_Stmt*_tmp545;struct Cyc_Absyn_Stmt*_tmp544;switch(*((int*)_tmp541)){case 2U: _LL1B: _tmp545=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp541)->f1;_tmp544=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp541)->f2;_LL1C:
 _tmp553=_tmp544;goto _LL1A;case 12U: _LL1D: _tmp547=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp541)->f1;_tmp546=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp541)->f2;_LL1E:
 _tmp553=_tmp546;goto _LL1A;case 1U: _LL1F: _tmp548=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp541)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp548);default: _LL21: _LL22:
({void*_tmp542=0U;({struct _fat_ptr _tmp78A=({const char*_tmp543="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp543,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78A,_tag_fat(_tmp542,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 3082
 return 1;}_LL0:;}
# 3099 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 3101
struct _tuple14 bogus_ans=({struct _tuple14 _tmp63E;_tmp63E.f1=0,_tmp63E.f2=Cyc_Absyn_heap_rgn_type;_tmp63E;});
void*_tmp554=e->r;void*_tmp555=_tmp554;struct Cyc_Absyn_Exp*_tmp58E;struct Cyc_Absyn_Exp*_tmp58D;struct Cyc_Absyn_Exp*_tmp58C;struct Cyc_Absyn_Exp*_tmp58B;struct _fat_ptr*_tmp58A;int _tmp589;struct Cyc_Absyn_Exp*_tmp588;struct _fat_ptr*_tmp587;int _tmp586;void*_tmp585;switch(*((int*)_tmp555)){case 1U: _LL1: _tmp585=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_LL2: {
# 3105
void*_tmp556=_tmp585;struct Cyc_Absyn_Vardecl*_tmp55E;struct Cyc_Absyn_Vardecl*_tmp55D;struct Cyc_Absyn_Vardecl*_tmp55C;struct Cyc_Absyn_Vardecl*_tmp55B;switch(*((int*)_tmp556)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp55B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp556)->f1;_LL13: {
# 3109
void*_tmp557=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp558=_tmp557;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp558)->tag == 4U){_LL1B: _LL1C:
# 3111
 return({struct _tuple14 _tmp632;_tmp632.f1=1,_tmp632.f2=Cyc_Absyn_heap_rgn_type;_tmp632;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp633;_tmp633.f1=(_tmp55B->tq).real_const,_tmp633.f2=Cyc_Absyn_heap_rgn_type;_tmp633;});}_LL1A:;}case 4U: _LL14: _tmp55C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp556)->f1;_LL15: {
# 3115
void*_tmp559=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp55A=_tmp559;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp55A)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp634;_tmp634.f1=1,_tmp634.f2=(void*)_check_null(_tmp55C->rgn);_tmp634;});}else{_LL22: _LL23:
# 3118
 _tmp55C->escapes=1;
return({struct _tuple14 _tmp635;_tmp635.f1=(_tmp55C->tq).real_const,_tmp635.f2=(void*)_check_null(_tmp55C->rgn);_tmp635;});}_LL1F:;}case 5U: _LL16: _tmp55D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp556)->f1;_LL17:
# 3121
 _tmp55E=_tmp55D;goto _LL19;default: _LL18: _tmp55E=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp556)->f1;_LL19:
# 3123
 _tmp55E->escapes=1;
return({struct _tuple14 _tmp636;_tmp636.f1=(_tmp55E->tq).real_const,_tmp636.f2=(void*)_check_null(_tmp55E->rgn);_tmp636;});}_LLD:;}case 21U: _LL3: _tmp588=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_tmp587=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp555)->f2;_tmp586=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp555)->f3;_LL4:
# 3128
 if(_tmp586)return bogus_ans;{
# 3131
void*_tmp55F=Cyc_Tcutil_compress((void*)_check_null(_tmp588->topt));void*_tmp560=_tmp55F;struct Cyc_Absyn_Aggrdecl*_tmp56C;struct Cyc_List_List*_tmp56B;switch(*((int*)_tmp560)){case 7U: _LL25: _tmp56B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp560)->f2;_LL26: {
# 3133
struct Cyc_Absyn_Aggrfield*_tmp561=Cyc_Absyn_lookup_field(_tmp56B,_tmp587);
if(_tmp561 != 0 && _tmp561->width == 0){
struct _tuple14 _tmp562=Cyc_Tcutil_addressof_props(_tmp588);struct _tuple14 _tmp563=_tmp562;int _tmp565;void*_tmp564;_LL2C: _tmp565=_tmp563.f1;_tmp564=_tmp563.f2;_LL2D:;
return({struct _tuple14 _tmp637;_tmp637.f1=(_tmp561->tq).real_const || _tmp565,_tmp637.f2=_tmp564;_tmp637;});}
# 3138
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp56C=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1)->f1).KnownAggr).val;_LL28: {
# 3140
struct Cyc_Absyn_Aggrfield*_tmp566=Cyc_Absyn_lookup_decl_field(_tmp56C,_tmp587);
if(_tmp566 != 0 && _tmp566->width == 0){
struct _tuple14 _tmp567=Cyc_Tcutil_addressof_props(_tmp588);struct _tuple14 _tmp568=_tmp567;int _tmp56A;void*_tmp569;_LL2F: _tmp56A=_tmp568.f1;_tmp569=_tmp568.f2;_LL30:;
return({struct _tuple14 _tmp638;_tmp638.f1=(_tmp566->tq).real_const || _tmp56A,_tmp638.f2=_tmp569;_tmp638;});}
# 3145
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp58B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_tmp58A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp555)->f2;_tmp589=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp555)->f3;_LL6:
# 3150
 if(_tmp589)return bogus_ans;{
# 3153
void*_tmp56D=Cyc_Tcutil_compress((void*)_check_null(_tmp58B->topt));void*_tmp56E=_tmp56D;void*_tmp574;void*_tmp573;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56E)->tag == 3U){_LL32: _tmp574=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56E)->f1).elt_type;_tmp573=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp56E)->f1).ptr_atts).rgn;_LL33: {
# 3155
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp56F=Cyc_Tcutil_compress(_tmp574);void*_tmp570=_tmp56F;struct Cyc_Absyn_Aggrdecl*_tmp572;struct Cyc_List_List*_tmp571;switch(*((int*)_tmp570)){case 7U: _LL37: _tmp571=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp570)->f2;_LL38:
# 3158
 finfo=Cyc_Absyn_lookup_field(_tmp571,_tmp58A);goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp572=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->f1).KnownAggr).val;_LL3A:
# 3160
 finfo=Cyc_Absyn_lookup_decl_field(_tmp572,_tmp58A);goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3163
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp639;_tmp639.f1=(finfo->tq).real_const,_tmp639.f2=_tmp573;_tmp639;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp58C=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_LL8: {
# 3170
void*_tmp575=Cyc_Tcutil_compress((void*)_check_null(_tmp58C->topt));void*_tmp576=_tmp575;struct Cyc_Absyn_Tqual _tmp578;void*_tmp577;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576)->tag == 3U){_LL3E: _tmp578=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576)->f1).elt_tq;_tmp577=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp576)->f1).ptr_atts).rgn;_LL3F:
# 3172
 return({struct _tuple14 _tmp63A;_tmp63A.f1=_tmp578.real_const,_tmp63A.f2=_tmp577;_tmp63A;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp58E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp555)->f1;_tmp58D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp555)->f2;_LLA: {
# 3178
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp58E->topt));
void*_tmp579=t;struct Cyc_Absyn_Tqual _tmp582;struct Cyc_Absyn_Tqual _tmp581;void*_tmp580;struct Cyc_List_List*_tmp57F;switch(*((int*)_tmp579)){case 6U: _LL43: _tmp57F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp579)->f1;_LL44: {
# 3182
struct _tuple13 _tmp57A=Cyc_Evexp_eval_const_uint_exp(_tmp58D);struct _tuple13 _tmp57B=_tmp57A;unsigned _tmp57E;int _tmp57D;_LL4C: _tmp57E=_tmp57B.f1;_tmp57D=_tmp57B.f2;_LL4D:;
if(!_tmp57D)
return bogus_ans;{
struct _tuple12*_tmp57C=Cyc_Absyn_lookup_tuple_field(_tmp57F,(int)_tmp57E);
if(_tmp57C != 0)
return({struct _tuple14 _tmp63B;_tmp63B.f1=((*_tmp57C).f1).real_const,({void*_tmp78B=(Cyc_Tcutil_addressof_props(_tmp58E)).f2;_tmp63B.f2=_tmp78B;});_tmp63B;});
return bogus_ans;};}case 3U: _LL45: _tmp581=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp579)->f1).elt_tq;_tmp580=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp579)->f1).ptr_atts).rgn;_LL46:
# 3190
 return({struct _tuple14 _tmp63C;_tmp63C.f1=_tmp581.real_const,_tmp63C.f2=_tmp580;_tmp63C;});case 4U: _LL47: _tmp582=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp579)->f1).tq;_LL48:
# 3196
 return({struct _tuple14 _tmp63D;_tmp63D.f1=_tmp582.real_const,({void*_tmp78C=(Cyc_Tcutil_addressof_props(_tmp58E)).f2;_tmp63D.f2=_tmp78C;});_tmp63D;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3200
({void*_tmp583=0U;({unsigned _tmp78E=e->loc;struct _fat_ptr _tmp78D=({const char*_tmp584="unary & applied to non-lvalue";_tag_fat(_tmp584,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp78E,_tmp78D,_tag_fat(_tmp583,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3207
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp58F=({void*_tmp78F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp78F,b);});
if(_tmp58F == 0)return;{
struct Cyc_Absyn_Exp*_tmp590=_tmp58F;
struct _tuple13 _tmp591=Cyc_Evexp_eval_const_uint_exp(_tmp590);struct _tuple13 _tmp592=_tmp591;unsigned _tmp59C;int _tmp59B;_LL1: _tmp59C=_tmp592.f1;_tmp59B=_tmp592.f2;_LL2:;
if(_tmp59B && _tmp59C <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp595=({struct Cyc_Int_pa_PrintArg_struct _tmp640;_tmp640.tag=1U,_tmp640.f1=(unsigned long)((int)_tmp59C);_tmp640;});struct Cyc_Int_pa_PrintArg_struct _tmp596=({struct Cyc_Int_pa_PrintArg_struct _tmp63F;_tmp63F.tag=1U,_tmp63F.f1=(unsigned long)((int)i);_tmp63F;});void*_tmp593[2U];_tmp593[0]=& _tmp595,_tmp593[1]=& _tmp596;({unsigned _tmp791=loc;struct _fat_ptr _tmp790=({const char*_tmp594="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp594,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp791,_tmp790,_tag_fat(_tmp593,sizeof(void*),2U));});});else{
# 3216
({struct Cyc_Int_pa_PrintArg_struct _tmp599=({struct Cyc_Int_pa_PrintArg_struct _tmp642;_tmp642.tag=1U,_tmp642.f1=(unsigned long)((int)_tmp59C);_tmp642;});struct Cyc_Int_pa_PrintArg_struct _tmp59A=({struct Cyc_Int_pa_PrintArg_struct _tmp641;_tmp641.tag=1U,_tmp641.f1=(unsigned long)((int)i);_tmp641;});void*_tmp597[2U];_tmp597[0]=& _tmp599,_tmp597[1]=& _tmp59A;({unsigned _tmp793=loc;struct _fat_ptr _tmp792=({const char*_tmp598="dereference is out of bounds: %d <= %d";_tag_fat(_tmp598,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp793,_tmp792,_tag_fat(_tmp597,sizeof(void*),2U));});});}}
return;};}
# 3220
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3229
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp59D=e->r;void*_tmp59E=_tmp59D;struct Cyc_List_List*_tmp5BD;struct Cyc_List_List*_tmp5BC;enum Cyc_Absyn_Primop _tmp5BB;struct Cyc_List_List*_tmp5BA;struct Cyc_List_List*_tmp5B9;struct Cyc_List_List*_tmp5B8;struct Cyc_List_List*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B3;void*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B1;void*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AA;struct Cyc_Absyn_Exp*_tmp5A9;struct Cyc_Absyn_Exp*_tmp5A8;struct Cyc_Absyn_Exp*_tmp5A7;void*_tmp5A6;switch(*((int*)_tmp59E)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5A6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL10: {
# 3241
void*_tmp59F=_tmp5A6;struct Cyc_Absyn_Vardecl*_tmp5A5;struct Cyc_Absyn_Vardecl*_tmp5A4;switch(*((int*)_tmp59F)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp5A4=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp59F)->f1;_LL37: {
# 3244
void*_tmp5A0=Cyc_Tcutil_compress(_tmp5A4->type);void*_tmp5A1=_tmp5A0;switch(*((int*)_tmp5A1)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp5A5=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp59F)->f1;_LL39:
# 3251
 if((int)_tmp5A5->sc == (int)Cyc_Absyn_Static){
void*_tmp5A2=Cyc_Tcutil_compress(_tmp5A5->type);void*_tmp5A3=_tmp5A2;switch(*((int*)_tmp5A3)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3258
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp5A9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5A8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_tmp5A7=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp59E)->f3;_LL12:
# 3264
 return(Cyc_Tcutil_cnst_exp(0,_tmp5A9)&&
 Cyc_Tcutil_cnst_exp(0,_tmp5A8))&&
 Cyc_Tcutil_cnst_exp(0,_tmp5A7);case 9U: _LL13: _tmp5AB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5AA=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL14:
# 3268
 return Cyc_Tcutil_cnst_exp(0,_tmp5AB)&& Cyc_Tcutil_cnst_exp(0,_tmp5AA);case 41U: _LL15: _tmp5AC=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL16:
 _tmp5AD=_tmp5AC;goto _LL18;case 12U: _LL17: _tmp5AD=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL18:
 _tmp5AE=_tmp5AD;goto _LL1A;case 13U: _LL19: _tmp5AE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL1A:
# 3272
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5AE);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp5B0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5AF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL1C:
# 3274
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5AF);}else{_LL1D: _tmp5B2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5B1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL1E:
# 3277
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp5B1);}case 15U: _LL1F: _tmp5B3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL20:
# 3279
 return Cyc_Tcutil_cnst_exp(1,_tmp5B3);case 27U: _LL21: _tmp5B5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_tmp5B4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp59E)->f3;_LL22:
# 3281
 return Cyc_Tcutil_cnst_exp(0,_tmp5B5)&& Cyc_Tcutil_cnst_exp(0,_tmp5B4);case 28U: _LL23: _tmp5B6=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL24:
# 3283
 return Cyc_Tcutil_cnst_exp(0,_tmp5B6);case 26U: _LL25: _tmp5B7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL26:
 _tmp5B8=_tmp5B7;goto _LL28;case 30U: _LL27: _tmp5B8=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL28:
 _tmp5B9=_tmp5B8;goto _LL2A;case 29U: _LL29: _tmp5B9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp59E)->f3;_LL2A:
# 3287
 for(0;_tmp5B9 != 0;_tmp5B9=_tmp5B9->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)_tmp5B9->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp5BB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_tmp5BA=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp59E)->f2;_LL2C:
# 3292
 _tmp5BC=_tmp5BA;goto _LL2E;case 24U: _LL2D: _tmp5BC=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL2E:
 _tmp5BD=_tmp5BC;goto _LL30;case 31U: _LL2F: _tmp5BD=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp59E)->f1;_LL30:
# 3295
 for(0;_tmp5BD != 0;_tmp5BD=_tmp5BD->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp5BD->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3302
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3306
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5BE=Cyc_Tcutil_compress(t);void*_tmp5BF=_tmp5BE;struct Cyc_List_List*_tmp5CF;struct Cyc_List_List*_tmp5CE;void*_tmp5CD;void*_tmp5CC;void*_tmp5CB;struct Cyc_List_List*_tmp5CA;switch(*((int*)_tmp5BF)){case 0U: _LL1: _tmp5CB=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5BF)->f1;_tmp5CA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5BF)->f2;_LL2: {
# 3310
void*_tmp5C0=_tmp5CB;union Cyc_Absyn_AggrInfo _tmp5C9;struct Cyc_List_List*_tmp5C8;struct Cyc_Absyn_Enumdecl*_tmp5C7;switch(*((int*)_tmp5C0)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5C7=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5C0)->f2;_LL15:
# 3315
 if(_tmp5C7 == 0 || _tmp5C7->fields == 0)
return 0;
_tmp5C8=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp5C7->fields))->v;goto _LL17;case 16U: _LL16: _tmp5C8=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5C0)->f1;_LL17:
# 3319
{struct Cyc_List_List*_tmp5C1=_tmp5C8;for(0;_tmp5C1 != 0;_tmp5C1=_tmp5C1->tl){
if(((struct Cyc_Absyn_Enumfield*)_tmp5C1->hd)->tag == 0)
return _tmp5C1 == _tmp5C8;{
struct _tuple13 _tmp5C2=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_tmp5C1->hd)->tag));struct _tuple13 _tmp5C3=_tmp5C2;unsigned _tmp5C5;int _tmp5C4;_LL1D: _tmp5C5=_tmp5C3.f1;_tmp5C4=_tmp5C3.f2;_LL1E:;
if(_tmp5C4 && _tmp5C5 == (unsigned)0)
return 1;};}}
# 3326
return 0;case 20U: _LL18: _tmp5C9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5C0)->f1;_LL19: {
# 3329
struct Cyc_Absyn_Aggrdecl*_tmp5C6=Cyc_Absyn_get_known_aggrdecl(_tmp5C9);
if(_tmp5C6->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5C6->impl))->exist_vars != 0)return 0;
if((int)_tmp5C6->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5C6->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(_tmp5C6->tvs,_tmp5CA,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp5C6->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5CC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5BF)->f1).ptr_atts).nullable;_LL4:
# 3337
 return Cyc_Tcutil_force_type2bool(1,_tmp5CC);case 4U: _LL5: _tmp5CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5BF)->f1).elt_type;_LL6:
 return Cyc_Tcutil_zeroable_type(_tmp5CD);case 6U: _LL7: _tmp5CE=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5BF)->f1;_LL8:
# 3340
 for(0;_tmp5CE != 0;_tmp5CE=_tmp5CE->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)_tmp5CE->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp5CF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5BF)->f2;_LLA:
 return Cyc_Tcutil_fields_zeroable(0,0,_tmp5CF);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3347
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3349
struct _RegionHandle _tmp5D0=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D0;_push_region(rgn);
{struct Cyc_List_List*_tmp5D1=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,_tmp5D1,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5D2=0;_npop_handler(0U);return _tmp5D2;}}{
# 3357
int _tmp5D3=1;_npop_handler(0U);return _tmp5D3;};}
# 3350
;_pop_region(rgn);}
# 3361
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5D4=t;struct Cyc_Absyn_Typedefdecl*_tmp5D9;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5D4)->tag == 8U){_LL1: _tmp5D9=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5D4)->f3;_LL2:
# 3364
 if(_tmp5D9 != 0){
struct Cyc_Absyn_Tqual _tmp5D5=_tmp5D9->tq;
if(((_tmp5D5.print_const || _tmp5D5.q_volatile)|| _tmp5D5.q_restrict)|| _tmp5D5.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp5D8=({struct Cyc_String_pa_PrintArg_struct _tmp643;_tmp643.tag=0U,({struct _fat_ptr _tmp794=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp643.f1=_tmp794;});_tmp643;});void*_tmp5D6[1U];_tmp5D6[0]=& _tmp5D8;({unsigned _tmp796=loc;struct _fat_ptr _tmp795=({const char*_tmp5D7="qualifier within typedef type %s is ignored";_tag_fat(_tmp5D7,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp796,_tmp795,_tag_fat(_tmp5D6,sizeof(void*),1U));});});}
# 3369
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3376
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp5DA=Cyc_Tcutil_compress(t);void*_tmp5DB=_tmp5DA;struct Cyc_List_List**_tmp5E1;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5DB)->tag == 5U){_LL1: _tmp5E1=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5DB)->f1).attributes;_LL2: {
# 3379
struct Cyc_List_List*_tmp5DC=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*_tmp5E1,(void*)atts->hd))
({struct Cyc_List_List*_tmp797=({struct Cyc_List_List*_tmp5DD=_cycalloc(sizeof(*_tmp5DD));_tmp5DD->hd=(void*)atts->hd,_tmp5DD->tl=*_tmp5E1;_tmp5DD;});*_tmp5E1=_tmp797;});}else{
# 3386
_tmp5DC=({struct Cyc_List_List*_tmp5DE=_cycalloc(sizeof(*_tmp5DE));_tmp5DE->hd=(void*)atts->hd,_tmp5DE->tl=_tmp5DC;_tmp5DE;});}}
return _tmp5DC;}}else{_LL3: _LL4:
({void*_tmp5DF=0U;({struct _fat_ptr _tmp798=({const char*_tmp5E0="transfer_fn_type_atts";_tag_fat(_tmp5E0,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp798,_tag_fat(_tmp5DF,sizeof(void*),0U));});});}_LL0:;}
# 3393
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5E2=Cyc_Tcutil_compress(t);void*_tmp5E3=_tmp5E2;struct Cyc_Absyn_Exp*_tmp5E5;struct Cyc_Absyn_PtrInfo _tmp5E4;switch(*((int*)_tmp5E3)){case 3U: _LL1: _tmp5E4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E3)->f1;_LL2:
 return({void*_tmp799=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp799,(_tmp5E4.ptr_atts).bounds);});case 4U: _LL3: _tmp5E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E3)->f1).num_elts;_LL4:
 return _tmp5E5;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3403
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5E6=b;struct Cyc_Absyn_Vardecl*_tmp5EA;struct Cyc_Absyn_Vardecl*_tmp5E9;struct Cyc_Absyn_Vardecl*_tmp5E8;struct Cyc_Absyn_Vardecl*_tmp5E7;switch(*((int*)_tmp5E6)){case 5U: _LL1: _tmp5E7=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL2:
 _tmp5E8=_tmp5E7;goto _LL4;case 4U: _LL3: _tmp5E8=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL4:
 _tmp5E9=_tmp5E8;goto _LL6;case 3U: _LL5: _tmp5E9=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL6:
 _tmp5EA=_tmp5E9;goto _LL8;case 1U: _LL7: _tmp5EA=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL8:
# 3409
 if(!_tmp5EA->escapes)
return _tmp5EA;
return 0;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3417
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5EB=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmp5EB=({struct Cyc_List_List*_tmp5EC=_cycalloc(sizeof(*_tmp5EC));_tmp5EC->hd=*((void**)_check_null((void**)x->hd)),_tmp5EC->tl=_tmp5EB;_tmp5EC;});}}
return _tmp5EB;}
# 3424
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5ED=Cyc_Tcutil_compress(t);void*_tmp5EE=_tmp5ED;void*_tmp5F8;struct Cyc_Absyn_Tqual _tmp5F7;struct Cyc_Absyn_Exp*_tmp5F6;void*_tmp5F5;unsigned _tmp5F4;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->tag == 4U){_LL1: _tmp5F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).elt_type;_tmp5F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).tq;_tmp5F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).num_elts;_tmp5F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).zero_term;_tmp5F4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).zt_loc;_LL2: {
# 3427
void*b;
if(_tmp5F6 == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 3431
if(convert_tag){
if(_tmp5F6->topt == 0)
({void*_tmp5EF=0U;({struct _fat_ptr _tmp79A=({const char*_tmp5F0="cannot convert tag without type!";_tag_fat(_tmp5F0,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp79A,_tag_fat(_tmp5EF,sizeof(void*),0U));});});{
void*_tmp5F1=Cyc_Tcutil_compress((void*)_check_null(_tmp5F6->topt));void*_tmp5F2=_tmp5F1;void*_tmp5F3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F2)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F2)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F2)->f2 != 0){_LL6: _tmp5F3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5F2)->f2)->hd;_LL7:
# 3437
 b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(_tmp5F3,0U));
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 3440
 b=Cyc_Tcutil_is_const_exp(_tmp5F6)?Cyc_Absyn_thin_bounds_exp(_tmp5F6): Cyc_Absyn_fat_bound_type;}_LL5:;};}else{
# 3444
b=Cyc_Absyn_thin_bounds_exp(_tmp5F6);}}
# 3446
return Cyc_Absyn_atb_type(_tmp5F8,rgn,_tmp5F7,b,_tmp5F5);}}else{_LL3: _LL4:
 return t;}_LL0:;}
