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
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple11{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple11*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 877 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 879
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 886
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 891
void*Cyc_Absyn_compress_kb(void*);
# 897
int Cyc_Absyn_type2bool(int def,void*);
# 902
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 907
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 909
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 914
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 916
extern void*Cyc_Absyn_empty_effect;
# 918
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 920
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 945
extern void*Cyc_Absyn_fat_bound_type;
# 947
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 949
void*Cyc_Absyn_bounds_one();
# 951
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 955
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 974
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 976
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 979
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 989
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 996
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 998
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1002
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1012
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1014
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1021
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
# 1033
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1042
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1081
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1125
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1127
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1129
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1135
int Cyc_Absyn_fntype_att(void*);
# 1137
int Cyc_Absyn_equal_att(void*,void*);
# 1139
int Cyc_Absyn_attribute_cmp(void*,void*);
# 1145
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
({void*_tmp64D=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp64D;});
return 1;}else{
return 0;}}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 223
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 229
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp64E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp64E,b);});struct Cyc_Absyn_Exp*eopt=_tmp37;
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
return({struct Cyc_Absyn_Tvar*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->name=tv->name,_tmp60->identity=- 1,({void*_tmp64F=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp60->kind=_tmp64F;});_tmp60;});}
# 367
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 369
struct _tuple9*_tmp61=arg;void*_tmp64;struct Cyc_Absyn_Tqual _tmp63;struct _fat_ptr*_tmp62;_LL1: _tmp62=_tmp61->f1;_tmp63=_tmp61->f2;_tmp64=_tmp61->f3;_LL2: {struct _fat_ptr*x=_tmp62;struct Cyc_Absyn_Tqual y=_tmp63;void*t=_tmp64;
return({struct _tuple9*_tmp65=_cycalloc(sizeof(*_tmp65));_tmp65->f1=x,_tmp65->f2=y,({void*_tmp650=Cyc_Tcutil_copy_type(t);_tmp65->f3=_tmp650;});_tmp65;});}}
# 372
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=(*arg).f1,({void*_tmp651=Cyc_Tcutil_copy_type((*arg).f2);_tmp66->f2=_tmp651;});_tmp66;});}
# 375
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}
# 379
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->name=f->name,_tmp67->tq=f->tq,({void*_tmp653=Cyc_Tcutil_copy_type(f->type);_tmp67->type=_tmp653;}),_tmp67->width=f->width,_tmp67->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp652=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp67->requires_clause=_tmp652;});_tmp67;});}struct _tuple15{void*f1;void*f2;};
# 383
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp68=x;void*_tmp6A;void*_tmp69;_LL1: _tmp69=_tmp68->f1;_tmp6A=_tmp68->f2;_LL2: {void*r1=_tmp69;void*r2=_tmp6A;
return({struct _tuple15*_tmp6B=_cycalloc(sizeof(*_tmp6B));({void*_tmp655=Cyc_Tcutil_copy_type(r1);_tmp6B->f1=_tmp655;}),({void*_tmp654=Cyc_Tcutil_copy_type(r2);_tmp6B->f2=_tmp654;});_tmp6B;});}}
# 387
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 391
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6C=Cyc_Tcutil_compress(t);void*_stmttmp19=_tmp6C;void*_tmp6D=_stmttmp19;struct Cyc_Absyn_Datatypedecl*_tmp6E;struct Cyc_Absyn_Enumdecl*_tmp6F;struct Cyc_Absyn_Aggrdecl*_tmp70;struct Cyc_Absyn_Typedefdecl*_tmp73;struct Cyc_List_List*_tmp72;struct _tuple1*_tmp71;struct Cyc_Absyn_Exp*_tmp74;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_List_List*_tmp77;enum Cyc_Absyn_AggrKind _tmp76;struct Cyc_List_List*_tmp78;struct Cyc_List_List*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_List_List*_tmp83;struct Cyc_Absyn_Exp*_tmp82;struct Cyc_List_List*_tmp81;struct Cyc_List_List*_tmp80;struct Cyc_Absyn_VarargInfo*_tmp7F;int _tmp7E;struct Cyc_List_List*_tmp7D;void*_tmp7C;struct Cyc_Absyn_Tqual _tmp7B;void*_tmp7A;struct Cyc_List_List*_tmp79;unsigned _tmp8A;void*_tmp89;struct Cyc_Absyn_Exp*_tmp88;struct Cyc_Absyn_Tqual _tmp87;void*_tmp86;struct Cyc_Absyn_PtrLoc*_tmp91;void*_tmp90;void*_tmp8F;void*_tmp8E;void*_tmp8D;struct Cyc_Absyn_Tqual _tmp8C;void*_tmp8B;struct Cyc_Absyn_Tvar*_tmp92;struct Cyc_List_List*_tmp94;void*_tmp93;void*_tmp95;switch(*((int*)_tmp6D)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2 == 0){_LL1: _tmp95=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_LL2: {void*c=_tmp95;
return t;}}else{_LL3: _tmp93=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f1;_tmp94=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6D)->f2;_LL4: {void*c=_tmp93;struct Cyc_List_List*ts=_tmp94;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp96=_cycalloc(sizeof(*_tmp96));_tmp96->tag=0U,_tmp96->f1=c,({struct Cyc_List_List*_tmp656=Cyc_Tcutil_copy_types(ts);_tmp96->f2=_tmp656;});_tmp96;});}}case 1U: _LL5: _LL6:
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
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->tag=4U,({void*_tmp659=Cyc_Tcutil_copy_type(et);(_tmp9E->f1).elt_type=_tmp659;}),(_tmp9E->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp658=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmp9E->f1).num_elts=_tmp658;}),({
void*_tmp657=Cyc_Tcutil_copy_type(zt);(_tmp9E->f1).zero_term=_tmp657;}),(_tmp9E->f1).zt_loc=ztl;_tmp9E;});}case 5U: _LLD: _tmp79=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).tvars;_tmp7A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).effect;_tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_tqual;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ret_type;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).args;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).c_varargs;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).cyc_varargs;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).rgn_po;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).attributes;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_clause;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).requires_relns;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_clause;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6D)->f1).ensures_relns;_LLE: {struct Cyc_List_List*tvs=_tmp79;void*effopt=_tmp7A;struct Cyc_Absyn_Tqual rt_tq=_tmp7B;void*rt=_tmp7C;struct Cyc_List_List*args=_tmp7D;int c_varargs=_tmp7E;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp7F;struct Cyc_List_List*rgn_po=_tmp80;struct Cyc_List_List*atts=_tmp81;struct Cyc_Absyn_Exp*req=_tmp82;struct Cyc_List_List*req_rlns=_tmp83;struct Cyc_Absyn_Exp*ens=_tmp84;struct Cyc_List_List*ens_rlns=_tmp85;
# 410
struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmp9F;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmpA0=Cyc_Tcutil_copy_type(rt);void*rt2=_tmpA0;
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmpA1;
int _tmpA2=c_varargs;int c_varargs2=_tmpA2;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo*cv=cyc_varargs;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA3=_cycalloc(sizeof(*_tmpA3));_tmpA3->name=cv->name,_tmpA3->tq=cv->tq,({void*_tmp65A=Cyc_Tcutil_copy_type(cv->type);_tmpA3->type=_tmp65A;}),_tmpA3->inject=cv->inject;_tmpA3;});}{
# 421
struct Cyc_List_List*_tmpA4=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmpA4;
struct Cyc_List_List*_tmpA5=atts;struct Cyc_List_List*atts2=_tmpA5;
struct Cyc_Absyn_Exp*_tmpA6=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmpA6;
struct Cyc_List_List*_tmpA7=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmpA7;
struct Cyc_Absyn_Exp*_tmpA8=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmpA8;
struct Cyc_List_List*_tmpA9=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmpA9;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=5U,(_tmpAA->f1).tvars=tvs2,(_tmpAA->f1).effect=effopt2,(_tmpAA->f1).ret_tqual=rt_tq,(_tmpAA->f1).ret_type=rt2,(_tmpAA->f1).args=args2,(_tmpAA->f1).c_varargs=c_varargs2,(_tmpAA->f1).cyc_varargs=cyc_varargs2,(_tmpAA->f1).rgn_po=rgn_po2,(_tmpAA->f1).attributes=atts2,(_tmpAA->f1).requires_clause=req2,(_tmpAA->f1).requires_relns=req_rlns2,(_tmpAA->f1).ensures_clause=ens2,(_tmpAA->f1).ensures_relns=ens_rlns2;_tmpAA;});}}case 6U: _LLF: _tmp78=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6D)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp78;
# 430
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAB=_cycalloc(sizeof(*_tmpAB));_tmpAB->tag=6U,({struct Cyc_List_List*_tmp65B=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmpAB->f1=_tmp65B;});_tmpAB;});}case 7U: _LL11: _tmp76=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f1;_tmp77=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6D)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp76;struct Cyc_List_List*fs=_tmp77;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=7U,_tmpAC->f1=k,({struct Cyc_List_List*_tmp65C=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmpAC->f2=_tmp65C;});_tmpAC;});}case 9U: _LL13: _tmp75=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6D)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp75;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpAD=_cycalloc(sizeof(*_tmpAD));_tmpAD->tag=9U,_tmpAD->f1=e;_tmpAD;});}case 11U: _LL15: _tmp74=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6D)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp74;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=11U,_tmpAE->f1=e;_tmpAE;});}case 8U: _LL17: _tmp71=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f1;_tmp72=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f2;_tmp73=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6D)->f3;_LL18: {struct _tuple1*tdn=_tmp71;struct Cyc_List_List*ts=_tmp72;struct Cyc_Absyn_Typedefdecl*td=_tmp73;
# 435
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=8U,_tmpAF->f1=tdn,({struct Cyc_List_List*_tmp65D=Cyc_Tcutil_copy_types(ts);_tmpAF->f2=_tmp65D;}),_tmpAF->f3=td,_tmpAF->f4=0;_tmpAF;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)){case 0U: _LL19: _tmp70=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp70;
# 438
struct Cyc_List_List*_tmpB0=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpB0;
return({union Cyc_Absyn_AggrInfo _tmp65E=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp65E,targs);});}case 1U: _LL1B: _tmp6F=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp6F;
# 441
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp6E=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6D)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp6E;
# 443
struct Cyc_List_List*_tmpB1=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpB1;
return({union Cyc_Absyn_DatatypeInfo _tmp65F=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp5FF;_tmp5FF.name=dd->name,_tmp5FF.is_extensible=0;_tmp5FF;}));Cyc_Absyn_datatype_type(_tmp65F,targs);});}}}_LL0:;}
# 450
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB2=d;struct _fat_ptr*_tmpB3;struct Cyc_Absyn_Exp*_tmpB4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->tag == 0U){_LL1: _tmpB4=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB2)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpB4;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB5=_cycalloc(sizeof(*_tmpB5));_tmpB5->tag=0U,({struct Cyc_Absyn_Exp*_tmp660=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpB5->f1=_tmp660;});_tmpB5;});}}else{_LL3: _tmpB3=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB2)->f1;_LL4: {struct _fat_ptr*v=_tmpB3;
return d;}}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 456
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 458
return({struct _tuple16*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct Cyc_List_List*_tmp662=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB6->f1=_tmp662;}),({struct Cyc_Absyn_Exp*_tmp661=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB6->f2=_tmp661;});_tmpB6;});}
# 461
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpB7=preserve_types;int pt=_tmpB7;
{void*_tmpB8=e->r;void*_stmttmp1A=_tmpB8;void*_tmpB9=_stmttmp1A;struct Cyc_Absyn_Exp*_tmpBA;struct Cyc_List_List*_tmpBF;struct Cyc_List_List*_tmpBE;struct Cyc_List_List*_tmpBD;struct _fat_ptr _tmpBC;int _tmpBB;void*_tmpC0;struct _fat_ptr*_tmpC2;struct Cyc_Absyn_Exp*_tmpC1;struct Cyc_List_List*_tmpC4;struct Cyc_Core_Opt*_tmpC3;struct Cyc_Absyn_Exp*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;int _tmpCC;int _tmpCB;struct Cyc_Absyn_Exp*_tmpCA;void**_tmpC9;struct Cyc_Absyn_Exp*_tmpC8;int _tmpC7;struct Cyc_Absyn_Enumfield*_tmpCE;void*_tmpCD;struct Cyc_Absyn_Enumfield*_tmpD0;struct Cyc_Absyn_Enumdecl*_tmpCF;struct Cyc_List_List*_tmpD4;void*_tmpD3;struct Cyc_Absyn_Tqual _tmpD2;struct _fat_ptr*_tmpD1;struct Cyc_List_List*_tmpD6;void*_tmpD5;struct Cyc_Absyn_Aggrdecl*_tmpDA;struct Cyc_List_List*_tmpD9;struct Cyc_List_List*_tmpD8;struct _tuple1*_tmpD7;int _tmpDD;void*_tmpDC;struct Cyc_Absyn_Exp*_tmpDB;int _tmpE1;struct Cyc_Absyn_Exp*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;struct Cyc_Absyn_Vardecl*_tmpDE;struct Cyc_Absyn_Datatypefield*_tmpE4;struct Cyc_Absyn_Datatypedecl*_tmpE3;struct Cyc_List_List*_tmpE2;struct Cyc_List_List*_tmpE5;struct Cyc_Absyn_Exp*_tmpE7;struct Cyc_Absyn_Exp*_tmpE6;int _tmpEB;int _tmpEA;struct _fat_ptr*_tmpE9;struct Cyc_Absyn_Exp*_tmpE8;int _tmpEF;int _tmpEE;struct _fat_ptr*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;struct Cyc_List_List*_tmpF0;struct Cyc_Absyn_Exp*_tmpF1;struct Cyc_Absyn_Exp*_tmpF2;struct Cyc_List_List*_tmpF4;void*_tmpF3;struct Cyc_Absyn_Exp*_tmpF5;void*_tmpF6;struct Cyc_Absyn_Exp*_tmpF7;struct Cyc_Absyn_Exp*_tmpF9;struct Cyc_Absyn_Exp*_tmpF8;enum Cyc_Absyn_Coercion _tmpFD;int _tmpFC;struct Cyc_Absyn_Exp*_tmpFB;void*_tmpFA;struct Cyc_List_List*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmp100;int _tmp102;struct Cyc_Absyn_Exp*_tmp101;int _tmp106;struct Cyc_Absyn_VarargCallInfo*_tmp105;struct Cyc_List_List*_tmp104;struct Cyc_Absyn_Exp*_tmp103;int _tmp10F;int _tmp10E;void*_tmp10D;struct Cyc_Absyn_Tqual _tmp10C;struct _fat_ptr*_tmp10B;struct Cyc_List_List*_tmp10A;int _tmp109;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_Exp*_tmp107;struct Cyc_Absyn_Exp*_tmp112;struct Cyc_Core_Opt*_tmp111;struct Cyc_Absyn_Exp*_tmp110;struct Cyc_Absyn_Exp*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;enum Cyc_Absyn_Incrementor _tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_List_List*_tmp11F;enum Cyc_Absyn_Primop _tmp11E;struct _fat_ptr _tmp120;void*_tmp121;union Cyc_Absyn_Cnst _tmp122;switch(*((int*)_tmpB9)){case 0U: _LL1: _tmp122=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp122;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp121=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL4: {void*b=_tmp121;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp120=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL6: {struct _fat_ptr p=_tmp120;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp11E=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11F=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp11E;struct Cyc_List_List*es=_tmp11F;
new_e=({enum Cyc_Absyn_Primop _tmp664=p;struct Cyc_List_List*_tmp663=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_primop_exp(_tmp664,_tmp663,e->loc);});goto _LL0;}case 5U: _LL9: _tmp11C=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11D=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp11C;enum Cyc_Absyn_Incrementor i=_tmp11D;
new_e=({struct Cyc_Absyn_Exp*_tmp666=Cyc_Tcutil_deep_copy_exp(pt,e1);enum Cyc_Absyn_Incrementor _tmp665=i;Cyc_Absyn_increment_exp(_tmp666,_tmp665,e->loc);});goto _LL0;}case 7U: _LLB: _tmp11A=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp11B=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
new_e=({struct Cyc_Absyn_Exp*_tmp668=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp667=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_and_exp(_tmp668,_tmp667,e->loc);});goto _LL0;}case 8U: _LLD: _tmp118=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp119=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp118;struct Cyc_Absyn_Exp*e2=_tmp119;
new_e=({struct Cyc_Absyn_Exp*_tmp66A=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp669=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_or_exp(_tmp66A,_tmp669,e->loc);});goto _LL0;}case 9U: _LLF: _tmp116=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp117=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp116;struct Cyc_Absyn_Exp*e2=_tmp117;
new_e=({struct Cyc_Absyn_Exp*_tmp66C=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp66B=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_seq_exp(_tmp66C,_tmp66B,e->loc);});goto _LL0;}case 6U: _LL11: _tmp113=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp114=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp115=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp113;struct Cyc_Absyn_Exp*e2=_tmp114;struct Cyc_Absyn_Exp*e3=_tmp115;
# 474
new_e=({struct Cyc_Absyn_Exp*_tmp66F=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp66E=Cyc_Tcutil_deep_copy_exp(pt,e2);struct Cyc_Absyn_Exp*_tmp66D=Cyc_Tcutil_deep_copy_exp(pt,e3);Cyc_Absyn_conditional_exp(_tmp66F,_tmp66E,_tmp66D,e->loc);});goto _LL0;}case 4U: _LL13: _tmp110=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp111=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp112=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp110;struct Cyc_Core_Opt*po=_tmp111;struct Cyc_Absyn_Exp*e2=_tmp112;
# 476
new_e=({struct Cyc_Absyn_Exp*_tmp672=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Core_Opt*_tmp671=(unsigned)po?({struct Cyc_Core_Opt*_tmp123=_cycalloc(sizeof(*_tmp123));_tmp123->v=(void*)po->v;_tmp123;}): 0;struct Cyc_Absyn_Exp*_tmp670=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_assignop_exp(_tmp672,_tmp671,_tmp670,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3 != 0){_LL15: _tmp107=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp108=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp109=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->num_varargs;_tmp10A=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->injectors;_tmp10B=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->name;_tmp10C=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->tq;_tmp10D=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->type;_tmp10E=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3)->vai)->inject;_tmp10F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp107;struct Cyc_List_List*es=_tmp108;int n=_tmp109;struct Cyc_List_List*is=_tmp10A;struct _fat_ptr*nm=_tmp10B;struct Cyc_Absyn_Tqual tq=_tmp10C;void*t=_tmp10D;int i=_tmp10E;int resolved=_tmp10F;
# 479
new_e=({void*_tmp678=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp126=_cycalloc(sizeof(*_tmp126));_tmp126->tag=10U,({
struct Cyc_Absyn_Exp*_tmp677=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp126->f1=_tmp677;}),({struct Cyc_List_List*_tmp676=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp126->f2=_tmp676;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp675=({struct Cyc_Absyn_VarargCallInfo*_tmp125=_cycalloc(sizeof(*_tmp125));_tmp125->num_varargs=n,_tmp125->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp674=({struct Cyc_Absyn_VarargInfo*_tmp124=_cycalloc(sizeof(*_tmp124));_tmp124->name=nm,_tmp124->tq=tq,({void*_tmp673=Cyc_Tcutil_copy_type(t);_tmp124->type=_tmp673;}),_tmp124->inject=i;_tmp124;});_tmp125->vai=_tmp674;});_tmp125;});
# 481
_tmp126->f3=_tmp675;}),_tmp126->f4=resolved;_tmp126;});
# 479
Cyc_Absyn_new_exp(_tmp678,e->loc);});
# 484
goto _LL0;}}else{_LL17: _tmp103=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp104=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmp105=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmp106=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp103;struct Cyc_List_List*es=_tmp104;struct Cyc_Absyn_VarargCallInfo*vci=_tmp105;int resolved=_tmp106;
# 486
new_e=({void*_tmp67B=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->tag=10U,({struct Cyc_Absyn_Exp*_tmp67A=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp127->f1=_tmp67A;}),({struct Cyc_List_List*_tmp679=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp127->f2=_tmp679;}),_tmp127->f3=vci,_tmp127->f4=resolved;_tmp127;});Cyc_Absyn_new_exp(_tmp67B,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmp101=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmp102=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp101;int b=_tmp102;
# 489
new_e=b?({struct Cyc_Absyn_Exp*_tmp67D=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_rethrow_exp(_tmp67D,e->loc);}):({struct Cyc_Absyn_Exp*_tmp67C=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_throw_exp(_tmp67C,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmp100=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp100;
# 492
new_e=({struct Cyc_Absyn_Exp*_tmp67E=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp67E,e->loc);});goto _LL0;}case 13U: _LL1D: _tmpFE=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmpFE;struct Cyc_List_List*ts=_tmpFF;
# 494
new_e=({struct Cyc_Absyn_Exp*_tmp680=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_List_List*_tmp67F=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp680,_tmp67F,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpFA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpFB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpFC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpFD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL20: {void*t=_tmpFA;struct Cyc_Absyn_Exp*e1=_tmpFB;int b=_tmpFC;enum Cyc_Absyn_Coercion c=_tmpFD;
# 496
new_e=({void*_tmp684=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp683=Cyc_Tcutil_deep_copy_exp(pt,e1);int _tmp682=b;enum Cyc_Absyn_Coercion _tmp681=c;Cyc_Absyn_cast_exp(_tmp684,_tmp683,_tmp682,_tmp681,e->loc);});goto _LL0;}case 16U: _LL21: _tmpF8=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF9=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpF8;struct Cyc_Absyn_Exp*e1=_tmpF9;
# 498
new_e=({struct Cyc_Absyn_Exp*_tmp686=(unsigned)eo?Cyc_Tcutil_deep_copy_exp(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp685=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_New_exp(_tmp686,_tmp685,e->loc);});goto _LL0;}case 15U: _LL23: _tmpF7=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpF7;
new_e=({struct Cyc_Absyn_Exp*_tmp687=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_address_exp(_tmp687,e->loc);});goto _LL0;}case 17U: _LL25: _tmpF6=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL26: {void*t=_tmpF6;
new_e=({void*_tmp688=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp688,e->loc);});goto _LL0;}case 18U: _LL27: _tmpF5=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF5;
new_e=({struct Cyc_Absyn_Exp*_tmp689=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp689,e->loc);});goto _LL0;}case 19U: _LL29: _tmpF3=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpF4=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL2A: {void*t=_tmpF3;struct Cyc_List_List*ofs=_tmpF4;
new_e=({void*_tmp68B=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp68A=ofs;Cyc_Absyn_offsetof_exp(_tmp68B,_tmp68A,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpF2=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpF2;
new_e=({struct Cyc_Absyn_Exp*_tmp68C=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_deref_exp(_tmp68C,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpF1=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpF1;
new_e=({struct Cyc_Absyn_Exp*_tmp68D=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_extension_exp(_tmp68D,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpF0=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL30: {struct Cyc_List_List*es=_tmpF0;
new_e=({struct Cyc_List_List*_tmp68E=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_tuple_exp(_tmp68E,e->loc);});goto _LL0;}case 21U: _LL31: _tmpEC=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpED=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpEF=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpEC;struct _fat_ptr*n=_tmpED;int f1=_tmpEE;int f2=_tmpEF;
# 507
new_e=({void*_tmp690=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->tag=21U,({struct Cyc_Absyn_Exp*_tmp68F=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp128->f1=_tmp68F;}),_tmp128->f2=n,_tmp128->f3=f1,_tmp128->f4=f2;_tmp128;});Cyc_Absyn_new_exp(_tmp690,e->loc);});goto _LL0;}case 22U: _LL33: _tmpE8=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE9=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpEA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpEB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpE8;struct _fat_ptr*n=_tmpE9;int f1=_tmpEA;int f2=_tmpEB;
# 509
new_e=({void*_tmp692=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->tag=22U,({struct Cyc_Absyn_Exp*_tmp691=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp129->f1=_tmp691;}),_tmp129->f2=n,_tmp129->f3=f1,_tmp129->f4=f2;_tmp129;});Cyc_Absyn_new_exp(_tmp692,e->loc);});goto _LL0;}case 23U: _LL35: _tmpE6=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE7=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpE6;struct Cyc_Absyn_Exp*e2=_tmpE7;
# 511
new_e=({struct Cyc_Absyn_Exp*_tmp694=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp693=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_subscript_exp(_tmp694,_tmp693,e->loc);});goto _LL0;}case 26U: _LL37: _tmpE5=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL38: {struct Cyc_List_List*eds=_tmpE5;
# 513
new_e=({void*_tmp696=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=26U,({struct Cyc_List_List*_tmp695=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12A->f1=_tmp695;});_tmp12A;});Cyc_Absyn_new_exp(_tmp696,e->loc);});goto _LL0;}case 31U: _LL39: _tmpE2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpE3=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE4=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3A: {struct Cyc_List_List*es=_tmpE2;struct Cyc_Absyn_Datatypedecl*dtd=_tmpE3;struct Cyc_Absyn_Datatypefield*dtf=_tmpE4;
# 515
new_e=({void*_tmp698=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=31U,({struct Cyc_List_List*_tmp697=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp12B->f1=_tmp697;}),_tmp12B->f2=dtd,_tmp12B->f3=dtf;_tmp12B;});Cyc_Absyn_new_exp(_tmp698,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpDE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDF=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpE0=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpE1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpDE;struct Cyc_Absyn_Exp*e1=_tmpDF;struct Cyc_Absyn_Exp*e2=_tmpE0;int b=_tmpE1;
# 517
new_e=({void*_tmp69B=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=27U,_tmp12C->f1=vd,({struct Cyc_Absyn_Exp*_tmp69A=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12C->f2=_tmp69A;}),({struct Cyc_Absyn_Exp*_tmp699=Cyc_Tcutil_deep_copy_exp(pt,e2);_tmp12C->f3=_tmp699;}),_tmp12C->f4=b;_tmp12C;});Cyc_Absyn_new_exp(_tmp69B,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpDB=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpDC=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpDD=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpDB;void*t=_tmpDC;int b=_tmpDD;
# 519
new_e=({void*_tmp69E=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=28U,({struct Cyc_Absyn_Exp*_tmp69D=Cyc_Tcutil_deep_copy_exp(pt,e);_tmp12D->f1=_tmp69D;}),({void*_tmp69C=Cyc_Tcutil_copy_type(t);_tmp12D->f2=_tmp69C;}),_tmp12D->f3=b;_tmp12D;});Cyc_Absyn_new_exp(_tmp69E,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpD7=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD8=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpD9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpDA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_LL40: {struct _tuple1*n=_tmpD7;struct Cyc_List_List*ts=_tmpD8;struct Cyc_List_List*eds=_tmpD9;struct Cyc_Absyn_Aggrdecl*agr=_tmpDA;
# 522
new_e=({void*_tmp6A1=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=29U,_tmp12E->f1=n,({struct Cyc_List_List*_tmp6A0=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp12E->f2=_tmp6A0;}),({struct Cyc_List_List*_tmp69F=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12E->f3=_tmp69F;}),_tmp12E->f4=agr;_tmp12E;});Cyc_Absyn_new_exp(_tmp6A1,e->loc);});
# 524
goto _LL0;}case 30U: _LL41: _tmpD5=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL42: {void*t=_tmpD5;struct Cyc_List_List*eds=_tmpD6;
# 526
new_e=({void*_tmp6A4=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->tag=30U,({void*_tmp6A3=Cyc_Tcutil_copy_type(t);_tmp12F->f1=_tmp6A3;}),({struct Cyc_List_List*_tmp6A2=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12F->f2=_tmp6A2;});_tmp12F;});Cyc_Absyn_new_exp(_tmp6A4,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpD1=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f1;_tmpD2=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f2;_tmpD3=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f1)->f3;_tmpD4=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL44: {struct _fat_ptr*vopt=_tmpD1;struct Cyc_Absyn_Tqual tq=_tmpD2;void*t=_tmpD3;struct Cyc_List_List*eds=_tmpD4;
# 529
new_e=({void*_tmp6A8=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=25U,({struct _tuple9*_tmp6A7=({struct _tuple9*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->f1=vopt,_tmp130->f2=tq,({void*_tmp6A6=Cyc_Tcutil_copy_type(t);_tmp130->f3=_tmp6A6;});_tmp130;});_tmp131->f1=_tmp6A7;}),({
struct Cyc_List_List*_tmp6A5=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp131->f2=_tmp6A5;});_tmp131;});
# 529
Cyc_Absyn_new_exp(_tmp6A8,e->loc);});
# 531
goto _LL0;}case 32U: _LL45: _tmpCF=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpD0=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpCF;struct Cyc_Absyn_Enumfield*ef=_tmpD0;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpCD=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpCE=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL48: {void*t=_tmpCD;struct Cyc_Absyn_Enumfield*ef=_tmpCE;
# 534
new_e=({void*_tmp6AA=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=33U,({void*_tmp6A9=Cyc_Tcutil_copy_type(t);_tmp132->f1=_tmp6A9;}),_tmp132->f2=ef;_tmp132;});Cyc_Absyn_new_exp(_tmp6AA,e->loc);});goto _LL0;}case 34U: _LL49: _tmpC7=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).is_calloc;_tmpC8=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).rgn;_tmpC9=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).elt_type;_tmpCA=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).num_elts;_tmpCB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).fat_result;_tmpCC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpB9)->f1).inline_call;_LL4A: {int ic=_tmpC7;struct Cyc_Absyn_Exp*r=_tmpC8;void**t=_tmpC9;struct Cyc_Absyn_Exp*n=_tmpCA;int res=_tmpCB;int inlc=_tmpCC;
# 536
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=Cyc_Tcutil_deep_copy_exp(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp133=_cycalloc(sizeof(*_tmp133));({void*_tmp6AB=Cyc_Tcutil_copy_type(*t);*_tmp133=_tmp6AB;});_tmp133;});
({void*_tmp6AC=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->tag=34U,(_tmp134->f1).is_calloc=ic,(_tmp134->f1).rgn=r1,(_tmp134->f1).elt_type=t1,(_tmp134->f1).num_elts=n,(_tmp134->f1).fat_result=res,(_tmp134->f1).inline_call=inlc;_tmp134;});e2->r=_tmp6AC;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpC5=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpC6=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpC5;struct Cyc_Absyn_Exp*e2=_tmpC6;
new_e=({struct Cyc_Absyn_Exp*_tmp6AE=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6AD=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_swap_exp(_tmp6AE,_tmp6AD,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpC3=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpC4=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpC3;struct Cyc_List_List*eds=_tmpC4;
# 544
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->v=(struct _tuple1*)nopt->v;_tmp135;});
new_e=({void*_tmp6B0=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->tag=36U,_tmp136->f1=nopt1,({struct Cyc_List_List*_tmp6AF=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp136->f2=_tmp6AF;});_tmp136;});Cyc_Absyn_new_exp(_tmp6B0,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 549
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6B1=({const char*_tmp137="deep_copy: statement expressions unsupported";_tag_fat(_tmp137,sizeof(char),45U);});_tmp138->f1=_tmp6B1;});_tmp138;}));case 38U: _LL51: _tmpC1=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpC2=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpC1;struct _fat_ptr*fn=_tmpC2;
new_e=({void*_tmp6B3=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->tag=38U,({struct Cyc_Absyn_Exp*_tmp6B2=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp139->f1=_tmp6B2;}),_tmp139->f2=fn;_tmp139;});Cyc_Absyn_new_exp(_tmp6B3,e->loc);});
goto _LL0;}case 39U: _LL53: _tmpC0=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL54: {void*t=_tmpC0;
new_e=({void*_tmp6B4=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp6B4,e->loc);});
goto _LL0;}case 40U: _LL55: _tmpBB=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_tmpBC=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f2;_tmpBD=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f3;_tmpBE=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f4;_tmpBF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpB9)->f5;_LL56: {int v=_tmpBB;struct _fat_ptr t=_tmpBC;struct Cyc_List_List*o=_tmpBD;struct Cyc_List_List*i=_tmpBE;struct Cyc_List_List*c=_tmpBF;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _LL57: _tmpBA=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpB9)->f1;_LL58: {struct Cyc_Absyn_Exp*e1=_tmpBA;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 558
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 562
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 573 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp13A=ka1;enum Cyc_Absyn_AliasQual _tmp13C;enum Cyc_Absyn_KindQual _tmp13B;_LL1: _tmp13B=_tmp13A->kind;_tmp13C=_tmp13A->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp13B;enum Cyc_Absyn_AliasQual a1=_tmp13C;
struct Cyc_Absyn_Kind*_tmp13D=ka2;enum Cyc_Absyn_AliasQual _tmp13F;enum Cyc_Absyn_KindQual _tmp13E;_LL4: _tmp13E=_tmp13D->kind;_tmp13F=_tmp13D->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp13E;enum Cyc_Absyn_AliasQual a2=_tmp13F;
# 577
if((int)k1 != (int)k2){
struct _tuple17 _tmp140=({struct _tuple17 _tmp600;_tmp600.f1=k1,_tmp600.f2=k2;_tmp600;});struct _tuple17 _stmttmp1B=_tmp140;struct _tuple17 _tmp141=_stmttmp1B;switch(_tmp141.f1){case Cyc_Absyn_BoxKind: switch(_tmp141.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp141.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 585
if((int)a1 != (int)a2){
struct _tuple18 _tmp142=({struct _tuple18 _tmp601;_tmp601.f1=a1,_tmp601.f2=a2;_tmp601;});struct _tuple18 _stmttmp1C=_tmp142;struct _tuple18 _tmp143=_stmttmp1C;switch(_tmp143.f1){case Cyc_Absyn_Aliasable: if(_tmp143.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp143.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 591
return 1;}}}
# 594
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp144=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1D=_tmp144;void*_tmp145=_stmttmp1D;struct Cyc_Absyn_Kind*_tmp146;struct Cyc_Absyn_Kind*_tmp147;switch(*((int*)_tmp145)){case 0U: _LL1: _tmp147=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp145)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp147;
return k;}case 2U: _LL3: _tmp146=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp145)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp146;
return k;}default: _LL5: _LL6:
({void*_tmp6B5=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp148=_cycalloc(sizeof(*_tmp148));_tmp148->tag=2U,_tmp148->f1=0,_tmp148->f2=def;_tmp148;});tv->kind=_tmp6B5;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 602
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp149=Cyc_Tcutil_compress(t);void*_stmttmp1E=_tmp149;void*_tmp14A=_stmttmp1E;struct Cyc_Absyn_Tvar*_tmp14B;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14A)->tag == 2U){_LL1: _tmp14B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14A)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp14B;
# 605
void*_tmp14C=tv->kind;void*oldkb=_tmp14C;
tv->kind=kb;
return({struct _tuple19 _tmp602;_tmp602.f1=tv,_tmp602.f2=oldkb;_tmp602;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp14F=({struct Cyc_String_pa_PrintArg_struct _tmp603;_tmp603.tag=0U,({struct _fat_ptr _tmp6B6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp603.f1=_tmp6B6;});_tmp603;});void*_tmp14D[1U];_tmp14D[0]=& _tmp14F;({struct _fat_ptr _tmp6B7=({const char*_tmp14E="swap_kind: cannot update the kind of %s";_tag_fat(_tmp14E,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6B7,_tag_fat(_tmp14D,sizeof(void*),1U));});});}_LL0:;}
# 614
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 616
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 620
struct _RegionHandle _tmp150=_new_region("temp");struct _RegionHandle*temp=& _tmp150;_push_region(temp);
{struct Cyc_List_List*_tmp151=0;struct Cyc_List_List*inst=_tmp151;
# 623
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp152=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp152;
void*_tmp153=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp153;
struct Cyc_Absyn_Kind*_tmp154=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp1F=_tmp154;struct Cyc_Absyn_Kind*_tmp155=_stmttmp1F;switch(((struct Cyc_Absyn_Kind*)_tmp155)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 629
 inst=({struct Cyc_List_List*_tmp157=_region_malloc(temp,sizeof(*_tmp157));({struct _tuple19*_tmp6B8=({struct _tuple19*_tmp156=_region_malloc(temp,sizeof(*_tmp156));_tmp156->f1=tv,_tmp156->f2=t;_tmp156;});_tmp157->hd=_tmp6B8;}),_tmp157->tl=inst;_tmp157;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 633
if(inst != 0){
field_type=({struct _RegionHandle*_tmp6BA=temp;struct Cyc_List_List*_tmp6B9=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp6BA,_tmp6B9,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 621
;_pop_region();}
# 638
return k;}
# 645
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 647
void*_tmp158=Cyc_Tcutil_compress(t);void*_stmttmp20=_tmp158;void*_tmp159=_stmttmp20;struct Cyc_Absyn_Typedefdecl*_tmp15A;struct Cyc_Absyn_Exp*_tmp15B;struct Cyc_Absyn_PtrInfo _tmp15C;struct Cyc_List_List*_tmp15E;void*_tmp15D;struct Cyc_Absyn_Tvar*_tmp15F;struct Cyc_Core_Opt*_tmp160;switch(*((int*)_tmp159)){case 1U: _LL1: _tmp160=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp159)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp160;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp15F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp159)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp15F;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp15D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp159)->f1;_tmp15E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp159)->f2;_LL6: {void*c=_tmp15D;struct Cyc_List_List*ts=_tmp15E;
# 651
void*_tmp161=c;int _tmp165;struct Cyc_Absyn_AggrdeclImpl*_tmp164;struct Cyc_List_List*_tmp163;enum Cyc_Absyn_AggrKind _tmp162;struct Cyc_Absyn_Kind*_tmp166;enum Cyc_Absyn_Size_of _tmp167;switch(*((int*)_tmp161)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp167=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp161)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp167;
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp166=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp161)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp166;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp161)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 672
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 674
({void*_tmp168=0U;({struct _fat_ptr _tmp6BB=({const char*_tmp169="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp169,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6BB,_tag_fat(_tmp168,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp161)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 678
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp162=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp161)->f1).KnownAggr).val)->kind;_tmp163=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp161)->f1).KnownAggr).val)->tvs;_tmp164=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp161)->f1).KnownAggr).val)->impl;_tmp165=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp161)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp162;struct Cyc_List_List*tvs=_tmp163;struct Cyc_Absyn_AggrdeclImpl*i=_tmp164;int expected_mem_kind=_tmp165;
# 680
if(i == 0){
if(expected_mem_kind)
return& Cyc_Tcutil_mk;else{
# 684
return& Cyc_Tcutil_ak;}}{
# 686
struct Cyc_List_List*_tmp16A=i->fields;struct Cyc_List_List*fields=_tmp16A;
if(fields == 0)return& Cyc_Tcutil_mk;
# 689
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){;}{
void*_tmp16B=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp16B;
struct Cyc_Absyn_Kind*_tmp16C=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp16C;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 697
for(0;fields != 0;fields=fields->tl){
void*_tmp16D=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp16D;
struct Cyc_Absyn_Kind*_tmp16E=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp16E;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 703
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 705
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp15C=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp159)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp15C;
# 708
void*_tmp16F=Cyc_Tcutil_compress((pinfo.ptr_atts).bounds);void*_stmttmp21=_tmp16F;void*_tmp170=_stmttmp21;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp170)->f1)){case 13U: _LL4D: _LL4E: {
# 710
enum Cyc_Absyn_AliasQual _tmp171=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp22=_tmp171;enum Cyc_Absyn_AliasQual _tmp172=_stmttmp22;switch(_tmp172){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 716
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 718
enum Cyc_Absyn_AliasQual _tmp173=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp23=_tmp173;enum Cyc_Absyn_AliasQual _tmp174=_stmttmp23;switch(_tmp174){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 724
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 728
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp15B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp159)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp15B;
# 730
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp15A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp159)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp15A;
# 734
if(td == 0 || td->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp177=({struct Cyc_String_pa_PrintArg_struct _tmp604;_tmp604.tag=0U,({struct _fat_ptr _tmp6BC=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp604.f1=_tmp6BC;});_tmp604;});void*_tmp175[1U];_tmp175[0]=& _tmp177;({struct _fat_ptr _tmp6BD=({const char*_tmp176="type_kind: typedef found: %s";_tag_fat(_tmp176,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6BD,_tag_fat(_tmp175,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp159)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 743
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 747
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 755
int Cyc_Tcutil_is_regparm0_att(void*a){
void*_tmp178=a;if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp178)->tag == 0U){if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp178)->f1 == 0){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 762
int Cyc_Tcutil_equiv_fn_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
return 1;}
# 771
static void*Cyc_Tcutil_rgns_of(void*t);
# 773
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 777
static struct _tuple19*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp179=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp24=_tmp179;struct Cyc_Absyn_Kind*_tmp17A=_stmttmp24;switch(((struct Cyc_Absyn_Kind*)_tmp17A)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp17A)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp17B=_cycalloc(sizeof(*_tmp17B));_tmp17B->tag=9U,({struct Cyc_Absyn_Exp*_tmp6BE=Cyc_Absyn_uint_exp(0U,0U);_tmp17B->f1=_tmp6BE;});_tmp17B;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 788
return({struct _tuple19*_tmp17C=_cycalloc(sizeof(*_tmp17C));_tmp17C->f1=tv,_tmp17C->f2=t;_tmp17C;});}
# 795
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp17D=Cyc_Tcutil_compress(t);void*_stmttmp25=_tmp17D;void*_tmp17E=_stmttmp25;struct Cyc_List_List*_tmp17F;struct Cyc_List_List*_tmp180;struct Cyc_List_List*_tmp187;struct Cyc_Absyn_VarargInfo*_tmp186;struct Cyc_List_List*_tmp185;void*_tmp184;struct Cyc_Absyn_Tqual _tmp183;void*_tmp182;struct Cyc_List_List*_tmp181;struct Cyc_List_List*_tmp188;void*_tmp189;void*_tmp18B;void*_tmp18A;struct Cyc_List_List*_tmp18C;switch(*((int*)_tmp17E)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17E)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17E)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp18C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp17E)->f2;_LL6: {struct Cyc_List_List*ts=_tmp18C;
# 800
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 804
struct Cyc_Absyn_Kind*_tmp18D=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp26=_tmp18D;struct Cyc_Absyn_Kind*_tmp18E=_stmttmp26;switch(((struct Cyc_Absyn_Kind*)_tmp18E)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp18A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17E)->f1).elt_type;_tmp18B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp17E)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp18A;void*r=_tmp18B;
# 812
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp18F[2U];({void*_tmp6C0=Cyc_Absyn_access_eff(r);_tmp18F[0]=_tmp6C0;}),({void*_tmp6BF=Cyc_Tcutil_rgns_of(et);_tmp18F[1]=_tmp6BF;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp18F,sizeof(void*),2U));})));}case 4U: _LLD: _tmp189=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp17E)->f1).elt_type;_LLE: {void*et=_tmp189;
# 814
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp188=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp17E)->f2;_LL10: {struct Cyc_List_List*fs=_tmp188;
# 816
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp181=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).tvars;_tmp182=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).effect;_tmp183=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).ret_tqual;_tmp184=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).ret_type;_tmp185=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).args;_tmp186=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).cyc_varargs;_tmp187=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp17E)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp181;void*eff=_tmp182;struct Cyc_Absyn_Tqual rt_tq=_tmp183;void*rt=_tmp184;struct Cyc_List_List*args=_tmp185;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp186;struct Cyc_List_List*rpo=_tmp187;
# 825
void*_tmp190=({struct Cyc_List_List*_tmp6C1=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp6C1,(void*)_check_null(eff));});void*e=_tmp190;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp180=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp17E)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp180;
# 828
struct Cyc_List_List*_tmp191=0;struct Cyc_List_List*ts=_tmp191;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp192=_cycalloc(sizeof(*_tmp192));_tmp192->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp192->tl=ts;_tmp192;});}
_tmp17F=ts;goto _LL16;}case 8U: _LL15: _tmp17F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17E)->f2;_LL16: {struct Cyc_List_List*ts=_tmp17F;
# 833
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({void*_tmp193=0U;({struct _fat_ptr _tmp6C2=({const char*_tmp194="typedecl in rgns_of";_tag_fat(_tmp194,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6C2,_tag_fat(_tmp193,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 843
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp195=e;void*_tmp196;struct Cyc_List_List**_tmp197;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f1)){case 9U: _LL1: _tmp197=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2;_LL2: {struct Cyc_List_List**es=_tmp197;
# 847
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp198=(void*)effs->hd;void*eff=_tmp198;
({void*_tmp6C3=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp6C3;});{
void*_tmp199=(void*)effs->hd;void*_stmttmp27=_tmp199;void*_tmp19A=_stmttmp27;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19A)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19A)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19A)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19A)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19A)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19A)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19A)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19A)->f2)->tl == 0){_LLE: _LLF:
# 856
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 860
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp19B=Cyc_Tcutil_compress((void*)effs->hd);void*_stmttmp28=_tmp19B;void*_tmp19C=_stmttmp28;void*_tmp19D;struct Cyc_List_List*_tmp19E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f1)){case 9U: _LL13: _tmp19E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp19E;
# 865
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);
goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19C)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp19D=_tmp19C;_LL1C: {void*e=_tmp19D;
effects=({struct Cyc_List_List*_tmp19F=_cycalloc(sizeof(*_tmp19F));_tmp19F->hd=e,_tmp19F->tl=effects;_tmp19F;});goto _LL12;}}_LL12:;}}
# 873
({struct Cyc_List_List*_tmp6C4=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp6C4;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2 != 0){_LL3: _tmp196=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp195)->f2)->hd;_LL4: {void*t=_tmp196;
# 876
void*_tmp1A0=Cyc_Tcutil_compress(t);void*_stmttmp29=_tmp1A0;void*_tmp1A1=_stmttmp29;switch(*((int*)_tmp1A1)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 881
 return e;}_LL0:;}}
# 886
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A2=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->tag=5U,(_tmp1A3->f1).tvars=0,(_tmp1A3->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp6C5=Cyc_Absyn_empty_tqual(0U);(_tmp1A3->f1).ret_tqual=_tmp6C5;}),(_tmp1A3->f1).ret_type=Cyc_Absyn_void_type,(_tmp1A3->f1).args=0,(_tmp1A3->f1).c_varargs=0,(_tmp1A3->f1).cyc_varargs=0,(_tmp1A3->f1).rgn_po=0,(_tmp1A3->f1).attributes=0,(_tmp1A3->f1).requires_clause=0,(_tmp1A3->f1).requires_relns=0,(_tmp1A3->f1).ensures_clause=0,(_tmp1A3->f1).ensures_relns=0;_tmp1A3;});
# 887
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp1A2;
# 896
return({void*_tmp6C9=(void*)fntype;void*_tmp6C8=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp6C7=Cyc_Absyn_empty_tqual(0U);void*_tmp6C6=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp6C9,_tmp6C8,_tmp6C7,_tmp6C6,Cyc_Absyn_false_type);});}
# 903
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp1A4=Cyc_Tcutil_compress(e);void*_stmttmp2A=_tmp1A4;void*_tmp1A5=_stmttmp2A;struct Cyc_Core_Opt*_tmp1A8;void**_tmp1A7;struct Cyc_Core_Opt*_tmp1A6;void*_tmp1A9;struct Cyc_List_List*_tmp1AA;void*_tmp1AB;switch(*((int*)_tmp1A5)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f2 != 0){_LL1: _tmp1AB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f2)->hd;_LL2: {void*r2=_tmp1AB;
# 910
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Tcutil_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp1AC=({struct _tuple15 _tmp605;_tmp605.f1=r,_tmp605.f2=r2;_tmp605;});struct _tuple15 _stmttmp2B=_tmp1AC;struct _tuple15 _tmp1AD=_stmttmp2B;struct Cyc_Absyn_Tvar*_tmp1AF;struct Cyc_Absyn_Tvar*_tmp1AE;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AD.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AD.f2)->tag == 2U){_LLC: _tmp1AE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AD.f1)->f1;_tmp1AF=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1AD.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1AE;struct Cyc_Absyn_Tvar*tv2=_tmp1AF;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1AA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f2;_LL4: {struct Cyc_List_List*es=_tmp1AA;
# 918
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f2 != 0){_LL5: _tmp1A9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A5)->f2)->hd;_LL6: {void*t=_tmp1A9;
# 922
void*_tmp1B0=Cyc_Tcutil_rgns_of(t);void*_stmttmp2C=_tmp1B0;void*_tmp1B1=_stmttmp2C;void*_tmp1B2;void*_tmp1B3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f2 != 0){_LL11: _tmp1B3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B1)->f2)->hd;_LL12: {void*t=_tmp1B3;
# 924
if(!constrain)return 0;{
void*_tmp1B4=Cyc_Tcutil_compress(t);void*_stmttmp2D=_tmp1B4;void*_tmp1B5=_stmttmp2D;struct Cyc_Core_Opt*_tmp1B8;void**_tmp1B7;struct Cyc_Core_Opt*_tmp1B6;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B5)->tag == 1U){_LL16: _tmp1B6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B5)->f1;_tmp1B7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B5)->f2;_tmp1B8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B5)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1B6;void**p=_tmp1B7;struct Cyc_Core_Opt*s=_tmp1B8;
# 929
void*_tmp1B9=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1B9;
# 932
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp6CB=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1BA[2U];_tmp1BA[0]=ev,({void*_tmp6CA=Cyc_Absyn_access_eff(r);_tmp1BA[1]=_tmp6CA;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BA,sizeof(void*),2U));})));*p=_tmp6CB;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1B2=_tmp1B1;_LL14: {void*e2=_tmp1B2;
# 937
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1A6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A5)->f1;_tmp1A7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A5)->f2;_tmp1A8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A5)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1A6;void**p=_tmp1A7;struct Cyc_Core_Opt*s=_tmp1A8;
# 940
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1BB=0U;({struct _fat_ptr _tmp6CC=({const char*_tmp1BC="effect evar has wrong kind";_tag_fat(_tmp1BC,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CC,_tag_fat(_tmp1BB,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 945
void*_tmp1BD=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1BD;
# 948
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);{
void*_tmp1BE=Cyc_Absyn_join_eff(({void*_tmp1BF[2U];_tmp1BF[0]=ev,({void*_tmp6CD=Cyc_Absyn_access_eff(r);_tmp1BF[1]=_tmp6CD;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BF,sizeof(void*),2U));}));void*new_typ=_tmp1BE;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 959
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1C0=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_stmttmp2E=_tmp1C0;void*_tmp1C1=_stmttmp2E;struct Cyc_Core_Opt*_tmp1C4;void**_tmp1C3;struct Cyc_Core_Opt*_tmp1C2;void*_tmp1C5;struct Cyc_List_List*_tmp1C6;switch(*((int*)_tmp1C1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C1)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1C6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C1)->f2;_LL4: {struct Cyc_List_List*es=_tmp1C6;
# 964
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C1)->f2 != 0){_LL5: _tmp1C5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C1)->f2)->hd;_LL6: {void*t2=_tmp1C5;
# 969
t2=Cyc_Tcutil_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1C7=Cyc_Tcutil_rgns_of(t);void*_stmttmp2F=_tmp1C7;void*_tmp1C8=_stmttmp2F;void*_tmp1C9;void*_tmp1CA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C8)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C8)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C8)->f2 != 0){_LLC: _tmp1CA=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C8)->f2)->hd;_LLD: {void*t3=_tmp1CA;
# 974
struct _tuple15 _tmp1CB=({struct _tuple15 _tmp606;({void*_tmp6CE=Cyc_Tcutil_compress(t3);_tmp606.f1=_tmp6CE;}),_tmp606.f2=t2;_tmp606;});struct _tuple15 _stmttmp30=_tmp1CB;struct _tuple15 _tmp1CC=_stmttmp30;struct Cyc_Absyn_Tvar*_tmp1CE;struct Cyc_Absyn_Tvar*_tmp1CD;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CC.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CC.f2)->tag == 2U){_LL11: _tmp1CD=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CC.f1)->f1;_tmp1CE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1CC.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1CD;struct Cyc_Absyn_Tvar*tv2=_tmp1CE;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1C9=_tmp1C8;_LLF: {void*e2=_tmp1C9;
# 978
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1C2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C1)->f1;_tmp1C3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C1)->f2;_tmp1C4=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C1)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1C2;void**p=_tmp1C3;struct Cyc_Core_Opt*s=_tmp1C4;
# 981
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1CF=0U;({struct _fat_ptr _tmp6CF=({const char*_tmp1D0="effect evar has wrong kind";_tag_fat(_tmp1D0,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6CF,_tag_fat(_tmp1CF,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 986
void*_tmp1D1=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1D1;
# 989
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1D2=Cyc_Absyn_join_eff(({void*_tmp1D3[2U];_tmp1D3[0]=ev,({void*_tmp6D0=Cyc_Absyn_regionsof_eff(t);_tmp1D3[1]=_tmp6D0;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D3,sizeof(void*),2U));}));void*new_typ=_tmp1D2;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1000
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1D4=e;struct Cyc_Core_Opt*_tmp1D7;void**_tmp1D6;struct Cyc_Core_Opt*_tmp1D5;void*_tmp1D8;struct Cyc_List_List*_tmp1D9;struct Cyc_Absyn_Tvar*_tmp1DA;switch(*((int*)_tmp1D4)){case 2U: _LL1: _tmp1DA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D4)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1DA;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D4)->f1)){case 9U: _LL3: _tmp1D9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D4)->f2;_LL4: {struct Cyc_List_List*es=_tmp1D9;
# 1005
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D4)->f2 != 0){_LL5: _tmp1D8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D4)->f2)->hd;_LL6: {void*t=_tmp1D8;
# 1010
void*_tmp1DB=Cyc_Tcutil_rgns_of(t);void*_stmttmp31=_tmp1DB;void*_tmp1DC=_stmttmp31;void*_tmp1DD;void*_tmp1DE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DC)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DC)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DC)->f2 != 0){_LLC: _tmp1DE=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DC)->f2)->hd;_LLD: {void*t2=_tmp1DE;
# 1012
if(!may_constrain_evars)return 0;{
void*_tmp1DF=Cyc_Tcutil_compress(t2);void*_stmttmp32=_tmp1DF;void*_tmp1E0=_stmttmp32;struct Cyc_Core_Opt*_tmp1E3;void**_tmp1E2;struct Cyc_Core_Opt*_tmp1E1;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E0)->tag == 1U){_LL11: _tmp1E1=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E0)->f1;_tmp1E2=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E0)->f2;_tmp1E3=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E0)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1E1;void**p=_tmp1E2;struct Cyc_Core_Opt*s=_tmp1E3;
# 1018
void*_tmp1E4=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E4;
# 1020
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp6D2=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1E5[2U];_tmp1E5[0]=ev,({void*_tmp6D1=Cyc_Absyn_var_type(v);_tmp1E5[1]=_tmp6D1;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E5,sizeof(void*),2U));})));*p=_tmp6D2;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1DD=_tmp1DC;_LLF: {void*e2=_tmp1DD;
# 1025
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1D5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D4)->f1;_tmp1D6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D4)->f2;_tmp1D7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D4)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1D5;void**p=_tmp1D6;struct Cyc_Core_Opt*s=_tmp1D7;
# 1028
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1E6=0U;({struct _fat_ptr _tmp6D3=({const char*_tmp1E7="effect evar has wrong kind";_tag_fat(_tmp1E7,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D3,_tag_fat(_tmp1E6,sizeof(void*),0U));});});{
# 1032
void*_tmp1E8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E8;
# 1034
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1E9=Cyc_Absyn_join_eff(({void*_tmp1EA[2U];_tmp1EA[0]=ev,({void*_tmp6D4=Cyc_Absyn_var_type(v);_tmp1EA[1]=_tmp6D4;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1EA,sizeof(void*),2U));}));void*new_typ=_tmp1E9;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1044
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1EB=e;void*_tmp1EC;struct Cyc_List_List*_tmp1ED;switch(*((int*)_tmp1EB)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f1)){case 9U: _LL1: _tmp1ED=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2;_LL2: {struct Cyc_List_List*es=_tmp1ED;
# 1048
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2 != 0){_LL3: _tmp1EC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2)->hd;_LL4: {void*t=_tmp1EC;
# 1053
void*_tmp1EE=Cyc_Tcutil_rgns_of(t);void*_stmttmp33=_tmp1EE;void*_tmp1EF=_stmttmp33;void*_tmp1F0;void*_tmp1F1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f2 != 0){_LLA: _tmp1F1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f2)->hd;_LLB: {void*t2=_tmp1F1;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1F0=_tmp1EF;_LLD: {void*e2=_tmp1F0;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1057
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 1071 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1076
void*_tmp1F2=Cyc_Tcutil_compress(e1);void*_stmttmp34=_tmp1F2;void*_tmp1F3=_stmttmp34;struct Cyc_Core_Opt*_tmp1F5;void**_tmp1F4;struct Cyc_Absyn_Tvar*_tmp1F6;void*_tmp1F7;void*_tmp1F8;struct Cyc_List_List*_tmp1F9;switch(*((int*)_tmp1F3)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f1)){case 9U: _LL1: _tmp1F9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2;_LL2: {struct Cyc_List_List*es=_tmp1F9;
# 1078
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2 != 0){_LL3: _tmp1F8=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2)->hd;_LL4: {void*r=_tmp1F8;
# 1089
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2 != 0){_LL7: _tmp1F7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2)->hd;_LL8: {void*t=_tmp1F7;
# 1093
void*_tmp1FA=Cyc_Tcutil_rgns_of(t);void*_stmttmp35=_tmp1FA;void*_tmp1FB=_stmttmp35;void*_tmp1FC;void*_tmp1FD;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FB)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FB)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FB)->f2 != 0){_LLE: _tmp1FD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FB)->f2)->hd;_LLF: {void*t2=_tmp1FD;
# 1098
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp1FC=_tmp1FB;_LL11: {void*e=_tmp1FC;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1F6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F3)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1F6;
# 1091
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1F4=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F3)->f2;_tmp1F5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F3)->f4;_LLA: {void**p=_tmp1F4;struct Cyc_Core_Opt*s=_tmp1F5;
# 1103
if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1107
*p=Cyc_Absyn_empty_effect;
# 1110
return 1;}else{
# 1112
return 0;}}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp200=({struct Cyc_String_pa_PrintArg_struct _tmp607;_tmp607.tag=0U,({struct _fat_ptr _tmp6D5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp607.f1=_tmp6D5;});_tmp607;});void*_tmp1FE[1U];_tmp1FE[0]=& _tmp200;({struct _fat_ptr _tmp6D6=({const char*_tmp1FF="subset_effect: bad effect: %s";_tag_fat(_tmp1FF,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D6,_tag_fat(_tmp1FE,sizeof(void*),1U));});});}_LL0:;}
# 1123
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1125
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp201=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp36=_tmp201;struct _tuple15*_tmp202=_stmttmp36;void*_tmp204;void*_tmp203;_LL1: _tmp203=_tmp202->f1;_tmp204=_tmp202->f2;_LL2: {void*t1a=_tmp203;void*t1b=_tmp204;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp205=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp37=_tmp205;struct _tuple15*_tmp206=_stmttmp37;void*_tmp208;void*_tmp207;_LL4: _tmp207=_tmp206->f1;_tmp208=_tmp206->f2;_LL5: {void*t2a=_tmp207;void*t2b=_tmp208;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1135
if(!found)return 0;}}}
# 1137
return 1;}
# 1143
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1145
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1148
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp209=t;switch(*((int*)_tmp209)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp209)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp209)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1154
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1157
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1160
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1163
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp209)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1152
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1155
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1158
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1161
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1164
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp209)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1153
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1156
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1159
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1162
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1165
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1187
({void*_tmp20A=0U;({struct _fat_ptr _tmp6D7=({const char*_tmp20B="bad con";_tag_fat(_tmp20B,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6D7,_tag_fat(_tmp20A,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp209)->f1){case 0U: _LL21: _LL22:
# 1166
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
# 1190
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp20C=t;void*_tmp20D;switch(*((int*)_tmp20C)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp20D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp20C)->f1;_LL18: {void*c=_tmp20D;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1206
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp20E=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp20E;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1211
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp20F=i;struct _tuple1*_tmp210;struct Cyc_Absyn_Datatypedecl*_tmp211;if((_tmp20F.KnownDatatype).tag == 2){_LL1: _tmp211=*(_tmp20F.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp211;
return dd->name;}}else{_LL3: _tmp210=((_tmp20F.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp210;
return n;}}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1217
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp212=i;struct _tuple1*_tmp214;struct _tuple1*_tmp213;struct Cyc_Absyn_Datatypefield*_tmp216;struct Cyc_Absyn_Datatypedecl*_tmp215;if((_tmp212.KnownDatatypefield).tag == 2){_LL1: _tmp215=((_tmp212.KnownDatatypefield).val).f1;_tmp216=((_tmp212.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp215;struct Cyc_Absyn_Datatypefield*df=_tmp216;
# 1220
return({struct _tuple20 _tmp608;_tmp608.f1=dd->name,_tmp608.f2=df->name;_tmp608;});}}else{_LL3: _tmp213=((_tmp212.UnknownDatatypefield).val).datatype_name;_tmp214=((_tmp212.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp213;struct _tuple1*f=_tmp214;
# 1222
return({struct _tuple20 _tmp609;_tmp609.f1=d,_tmp609.f2=f;_tmp609;});}}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1225
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp217=i;struct Cyc_Absyn_Aggrdecl*_tmp218;struct _tuple1*_tmp21A;enum Cyc_Absyn_AggrKind _tmp219;if((_tmp217.UnknownAggr).tag == 1){_LL1: _tmp219=((_tmp217.UnknownAggr).val).f1;_tmp21A=((_tmp217.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp219;struct _tuple1*n=_tmp21A;
return({struct _tuple21 _tmp60A;_tmp60A.f1=k,_tmp60A.f2=n;_tmp60A;});}}else{_LL3: _tmp218=*(_tmp217.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp218;
return({struct _tuple21 _tmp60B;_tmp60B.f1=ad->kind,_tmp60B.f2=ad->name;_tmp60B;});}}_LL0:;}
# 1231
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1237
struct _tuple15 _tmp21B=({struct _tuple15 _tmp60C;_tmp60C.f1=t1,_tmp60C.f2=t2;_tmp60C;});struct _tuple15 _stmttmp38=_tmp21B;struct _tuple15 _tmp21C=_stmttmp38;union Cyc_Absyn_AggrInfo _tmp21E;union Cyc_Absyn_AggrInfo _tmp21D;union Cyc_Absyn_DatatypeFieldInfo _tmp220;union Cyc_Absyn_DatatypeFieldInfo _tmp21F;union Cyc_Absyn_DatatypeInfo _tmp222;union Cyc_Absyn_DatatypeInfo _tmp221;struct Cyc_List_List*_tmp224;struct Cyc_List_List*_tmp223;struct _fat_ptr _tmp226;struct _fat_ptr _tmp225;struct _tuple1*_tmp228;struct _tuple1*_tmp227;switch(*((int*)_tmp21C.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21C.f2)->tag == 15U){_LL1: _tmp227=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21C.f1)->f1;_tmp228=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp21C.f2)->f1;_LL2: {struct _tuple1*n1=_tmp227;struct _tuple1*n2=_tmp228;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21C.f2)->tag == 17U){_LL3: _tmp225=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21C.f1)->f1;_tmp226=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp21C.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp225;struct _fat_ptr s2=_tmp226;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21C.f2)->tag == 16U){_LL5: _tmp223=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21C.f1)->f1;_tmp224=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp21C.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp223;struct Cyc_List_List*fs2=_tmp224;
# 1241
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21C.f2)->tag == 18U){_LL7: _tmp221=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21C.f1)->f1;_tmp222=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp21C.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp221;union Cyc_Absyn_DatatypeInfo info2=_tmp222;
# 1243
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(info1);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(info2);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21C.f2)->tag == 19U){_LL9: _tmp21F=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21C.f1)->f1;_tmp220=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp21C.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp21F;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp220;
# 1247
struct _tuple20 _tmp229=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple20 _stmttmp39=_tmp229;struct _tuple20 _tmp22A=_stmttmp39;struct _tuple1*_tmp22C;struct _tuple1*_tmp22B;_LL10: _tmp22B=_tmp22A.f1;_tmp22C=_tmp22A.f2;_LL11: {struct _tuple1*d1=_tmp22B;struct _tuple1*f1=_tmp22C;
struct _tuple20 _tmp22D=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple20 _stmttmp3A=_tmp22D;struct _tuple20 _tmp22E=_stmttmp3A;struct _tuple1*_tmp230;struct _tuple1*_tmp22F;_LL13: _tmp22F=_tmp22E.f1;_tmp230=_tmp22E.f2;_LL14: {struct _tuple1*d2=_tmp22F;struct _tuple1*f2=_tmp230;
int _tmp231=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp231;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21C.f2)->tag == 20U){_LLB: _tmp21D=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21C.f1)->f1;_tmp21E=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp21C.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp21D;union Cyc_Absyn_AggrInfo info2=_tmp21E;
# 1253
struct _tuple21 _tmp232=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple21 _stmttmp3B=_tmp232;struct _tuple21 _tmp233=_stmttmp3B;struct _tuple1*_tmp235;enum Cyc_Absyn_AggrKind _tmp234;_LL16: _tmp234=_tmp233.f1;_tmp235=_tmp233.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp234;struct _tuple1*q1=_tmp235;
struct _tuple21 _tmp236=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple21 _stmttmp3C=_tmp236;struct _tuple21 _tmp237=_stmttmp3C;struct _tuple1*_tmp239;enum Cyc_Absyn_AggrKind _tmp238;_LL19: _tmp238=_tmp237.f1;_tmp239=_tmp237.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp238;struct _tuple1*q2=_tmp239;
int _tmp23A=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp23A;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1262
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp6D9)(void*,void*)=cmp;void*_tmp6D8=(void*)_check_null(a1);_tmp6D9(_tmp6D8,(void*)_check_null(a2));});}
# 1268
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp23B=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp23B;
int _tmp23C=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp23C;
return Cyc_Core_intcmp(i1,i2);}
# 1273
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp23D=tqt1;void*_tmp23F;struct Cyc_Absyn_Tqual _tmp23E;_LL1: _tmp23E=_tmp23D->f1;_tmp23F=_tmp23D->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp23E;void*t1=_tmp23F;
struct _tuple12*_tmp240=tqt2;void*_tmp242;struct Cyc_Absyn_Tqual _tmp241;_LL4: _tmp241=_tmp240->f1;_tmp242=_tmp240->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp241;void*t2=_tmp242;
int _tmp243=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp243;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1281
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp244=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp244;
if(zsc != 0)return zsc;{
int _tmp245=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp245;
if(tqc != 0)return tqc;{
int _tmp246=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp246;
if(tc != 0)return tc;{
int _tmp247=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp247;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1298
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp248=({int _tmp6DA=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp6DA,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp248;
if(shallowcmp != 0)
return shallowcmp;{
# 1307
struct _tuple15 _tmp249=({struct _tuple15 _tmp60D;_tmp60D.f1=t2,_tmp60D.f2=t1;_tmp60D;});struct _tuple15 _stmttmp3D=_tmp249;struct _tuple15 _tmp24A=_stmttmp3D;struct Cyc_Absyn_Exp*_tmp24C;struct Cyc_Absyn_Exp*_tmp24B;struct Cyc_Absyn_Exp*_tmp24E;struct Cyc_Absyn_Exp*_tmp24D;struct Cyc_List_List*_tmp252;enum Cyc_Absyn_AggrKind _tmp251;struct Cyc_List_List*_tmp250;enum Cyc_Absyn_AggrKind _tmp24F;struct Cyc_List_List*_tmp254;struct Cyc_List_List*_tmp253;struct Cyc_Absyn_FnInfo _tmp256;struct Cyc_Absyn_FnInfo _tmp255;void*_tmp25E;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_Absyn_Tqual _tmp25C;void*_tmp25B;void*_tmp25A;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_Absyn_Tqual _tmp258;void*_tmp257;void*_tmp26A;void*_tmp269;void*_tmp268;void*_tmp267;struct Cyc_Absyn_Tqual _tmp266;void*_tmp265;void*_tmp264;void*_tmp263;void*_tmp262;void*_tmp261;struct Cyc_Absyn_Tqual _tmp260;void*_tmp25F;struct Cyc_Absyn_Tvar*_tmp26C;struct Cyc_Absyn_Tvar*_tmp26B;struct Cyc_List_List*_tmp270;void*_tmp26F;struct Cyc_List_List*_tmp26E;void*_tmp26D;switch(*((int*)_tmp24A.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A.f2)->tag == 0U){_LL1: _tmp26D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A.f1)->f1;_tmp26E=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A.f1)->f2;_tmp26F=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A.f2)->f1;_tmp270=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24A.f2)->f2;_LL2: {void*c1=_tmp26D;struct Cyc_List_List*ts1=_tmp26E;void*c2=_tmp26F;struct Cyc_List_List*ts2=_tmp270;
# 1309
int _tmp271=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp271;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24A.f2)->tag == 1U){_LL3: _LL4:
# 1313
({void*_tmp272=0U;({struct _fat_ptr _tmp6DB=({const char*_tmp273="typecmp: can only compare closed types";_tag_fat(_tmp273,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DB,_tag_fat(_tmp272,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24A.f2)->tag == 2U){_LL5: _tmp26B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24A.f1)->f1;_tmp26C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24A.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp26B;struct Cyc_Absyn_Tvar*tv1=_tmp26C;
# 1317
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f2)->tag == 3U){_LL7: _tmp25F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f1)->f1).elt_type;_tmp260=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f1)->f1).elt_tq;_tmp261=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f1)->f1).ptr_atts).rgn;_tmp262=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f1)->f1).ptr_atts).nullable;_tmp263=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f1)->f1).ptr_atts).bounds;_tmp264=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f1)->f1).ptr_atts).zero_term;_tmp265=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f2)->f1).elt_type;_tmp266=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f2)->f1).elt_tq;_tmp267=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f2)->f1).ptr_atts).rgn;_tmp268=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f2)->f1).ptr_atts).nullable;_tmp269=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f2)->f1).ptr_atts).bounds;_tmp26A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24A.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp25F;struct Cyc_Absyn_Tqual tqual2a=_tmp260;void*rgn2=_tmp261;void*null2a=_tmp262;void*b2=_tmp263;void*zt2=_tmp264;void*t1a=_tmp265;struct Cyc_Absyn_Tqual tqual1a=_tmp266;void*rgn1=_tmp267;void*null1a=_tmp268;void*b1=_tmp269;void*zt1=_tmp26A;
# 1321
int _tmp274=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp274;
if(etc != 0)return etc;{
int _tmp275=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp275;
if(rc != 0)return rc;{
int _tmp276=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp276;
if(tqc != 0)return tqc;{
int _tmp277=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp277;
if(cc != 0)return cc;{
int _tmp278=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp278;
if(zc != 0)return zc;{
int _tmp279=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp279;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f2)->tag == 4U){_LL9: _tmp257=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f1)->f1).elt_type;_tmp258=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f1)->f1).tq;_tmp259=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f1)->f1).num_elts;_tmp25A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f1)->f1).zero_term;_tmp25B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f2)->f1).elt_type;_tmp25C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f2)->f1).tq;_tmp25D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f2)->f1).num_elts;_tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24A.f2)->f1).zero_term;_LLA: {void*t2a=_tmp257;struct Cyc_Absyn_Tqual tq2a=_tmp258;struct Cyc_Absyn_Exp*e1=_tmp259;void*zt1=_tmp25A;void*t1a=_tmp25B;struct Cyc_Absyn_Tqual tq1a=_tmp25C;struct Cyc_Absyn_Exp*e2=_tmp25D;void*zt2=_tmp25E;
# 1337
int _tmp27A=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp27A;
if(tqc != 0)return tqc;{
int _tmp27B=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp27B;
if(tc != 0)return tc;{
int _tmp27C=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp27C;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({void*_tmp27D=0U;({struct _fat_ptr _tmp6DC=({const char*_tmp27E="missing expression in array index";_tag_fat(_tmp27E,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DC,_tag_fat(_tmp27D,sizeof(void*),0U));});});
# 1347
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24A.f2)->tag == 5U){_LLB: _tmp255=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24A.f1)->f1;_tmp256=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24A.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp255;struct Cyc_Absyn_FnInfo f2=_tmp256;
# 1350
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp27F=f1.args;struct Cyc_List_List*args1=_tmp27F;
struct Cyc_List_List*_tmp280=f2.args;struct Cyc_List_List*args2=_tmp280;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp281=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3E=_tmp281;struct _tuple9 _tmp282=_stmttmp3E;void*_tmp284;struct Cyc_Absyn_Tqual _tmp283;_LL18: _tmp283=_tmp282.f2;_tmp284=_tmp282.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp283;void*t1=_tmp284;
struct _tuple9 _tmp285=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp3F=_tmp285;struct _tuple9 _tmp286=_stmttmp3F;void*_tmp288;struct Cyc_Absyn_Tqual _tmp287;_LL1B: _tmp287=_tmp286.f2;_tmp288=_tmp286.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp287;void*t2=_tmp288;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1365
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp6DD=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp6DD,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1379
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp289=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp289;
struct Cyc_List_List*_tmp28A=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp28A;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp28B=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp40=_tmp28B;struct _tuple15 _tmp28C=_stmttmp40;void*_tmp28E;void*_tmp28D;_LL1E: _tmp28D=_tmp28C.f1;_tmp28E=_tmp28C.f2;_LL1F: {void*t1a=_tmp28D;void*t1b=_tmp28E;
struct _tuple15 _tmp28F=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp41=_tmp28F;struct _tuple15 _tmp290=_stmttmp41;void*_tmp292;void*_tmp291;_LL21: _tmp291=_tmp290.f1;_tmp292=_tmp290.f2;_LL22: {void*t2a=_tmp291;void*t2b=_tmp292;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1389
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1397
({void*_tmp293=0U;({struct _fat_ptr _tmp6DE=({const char*_tmp294="typecmp: function type comparison should never get here!";_tag_fat(_tmp294,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DE,_tag_fat(_tmp293,sizeof(void*),0U));});});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24A.f2)->tag == 6U){_LLD: _tmp253=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24A.f1)->f1;_tmp254=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24A.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp253;struct Cyc_List_List*ts1=_tmp254;
# 1400
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24A.f2)->tag == 7U){_LLF: _tmp24F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24A.f1)->f1;_tmp250=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24A.f1)->f2;_tmp251=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24A.f2)->f1;_tmp252=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24A.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp24F;struct Cyc_List_List*fs2=_tmp250;enum Cyc_Absyn_AggrKind k1=_tmp251;struct Cyc_List_List*fs1=_tmp252;
# 1403
if((int)k1 != (int)k2){
if((int)k1 == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24A.f2)->tag == 9U){_LL11: _tmp24D=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24A.f1)->f1;_tmp24E=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24A.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp24D;struct Cyc_Absyn_Exp*e2=_tmp24E;
# 1408
_tmp24B=e1;_tmp24C=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24A.f2)->tag == 11U){_LL13: _tmp24B=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24A.f1)->f1;_tmp24C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24A.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp24B;struct Cyc_Absyn_Exp*e2=_tmp24C;
# 1410
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp295=0U;({struct _fat_ptr _tmp6DF=({const char*_tmp296="Unmatched case in typecmp";_tag_fat(_tmp296,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6DF,_tag_fat(_tmp295,sizeof(void*),0U));});});}_LL0:;}}}
# 1417
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp297=({struct _tuple15 _tmp60F;({void*_tmp6E1=Cyc_Tcutil_compress(t1);_tmp60F.f1=_tmp6E1;}),({void*_tmp6E0=Cyc_Tcutil_compress(t2);_tmp60F.f2=_tmp6E0;});_tmp60F;});struct _tuple15 _stmttmp42=_tmp297;struct _tuple15 _tmp298=_stmttmp42;void*_tmp29A;void*_tmp299;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp298.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp298.f2)->tag == 0U){_LL1: _tmp299=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp298.f1)->f1;_tmp29A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp298.f2)->f1;_LL2: {void*c1=_tmp299;void*c2=_tmp29A;
# 1420
struct _tuple15 _tmp29B=({struct _tuple15 _tmp60E;_tmp60E.f1=c1,_tmp60E.f2=c2;_tmp60E;});struct _tuple15 _stmttmp43=_tmp29B;struct _tuple15 _tmp29C=_stmttmp43;int _tmp29E;int _tmp29D;switch(*((int*)_tmp29C.f1)){case 2U: switch(*((int*)_tmp29C.f2)){case 2U: _LL6: _tmp29D=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29C.f1)->f1;_tmp29E=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29C.f2)->f1;_LL7: {int i1=_tmp29D;int i2=_tmp29E;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp29C.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1428
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1433
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1436
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29C.f2)->f1 == 0){_LL14: _LL15:
# 1431
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp29C.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1429
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1434
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1437
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp29C.f2)->f1 == 0){_LL16: _LL17:
# 1432
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1438
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp29C.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1435
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1439
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1441
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1443
 return 0;}_LL0:;}
# 1447
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp29F=({struct _tuple15 _tmp611;({void*_tmp6E3=Cyc_Tcutil_compress(t1);_tmp611.f1=_tmp6E3;}),({void*_tmp6E2=Cyc_Tcutil_compress(t2);_tmp611.f2=_tmp6E2;});_tmp611;});struct _tuple15 _stmttmp44=_tmp29F;struct _tuple15 _tmp2A0=_stmttmp44;void*_tmp2A2;void*_tmp2A1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0.f2)->tag == 0U){_LL1: _tmp2A1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0.f1)->f1;_tmp2A2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A0.f2)->f1;_LL2: {void*c1=_tmp2A1;void*c2=_tmp2A2;
# 1450
{struct _tuple15 _tmp2A3=({struct _tuple15 _tmp610;_tmp610.f1=c1,_tmp610.f2=c2;_tmp610;});struct _tuple15 _stmttmp45=_tmp2A3;struct _tuple15 _tmp2A4=_stmttmp45;int _tmp2A6;int _tmp2A5;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A4.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 2U){_LL6: _tmp2A5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1;_tmp2A6=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1;_LL7: {int i1=_tmp2A5;int i2=_tmp2A6;
# 1452
if(i1 != 0 && i1 != 1)return t1;else{
if(i2 != 0 && i2 != 1)return t2;else{
if(i1 >= i2)return t1;else{
return t2;}}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1462
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1467
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1460
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1469
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1459
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1463
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A4.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1468
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1461
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A4.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A4.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1470
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A4.f1)->tag == 4U){_LL18: _LL19:
# 1465
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A4.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1471
 goto _LL5;}}}}}}_LL5:;}
# 1473
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1476
return Cyc_Absyn_sint_type;}
# 1481
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1484
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 ||
 Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2A7=_cycalloc(sizeof(*_tmp2A7));_tmp2A7->v=t1;_tmp2A7;});}}}
# 1493
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1497
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp2AA=({struct Cyc_String_pa_PrintArg_struct _tmp613;_tmp613.tag=0U,({
struct _fat_ptr _tmp6E4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp613.f1=_tmp6E4;});_tmp613;});struct Cyc_String_pa_PrintArg_struct _tmp2AB=({struct Cyc_String_pa_PrintArg_struct _tmp612;_tmp612.tag=0U,({struct _fat_ptr _tmp6E5=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp612.f1=_tmp6E5;});_tmp612;});void*_tmp2A8[2U];_tmp2A8[0]=& _tmp2AA,_tmp2A8[1]=& _tmp2AB;({unsigned _tmp6E7=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _fat_ptr _tmp6E6=({const char*_tmp2A9="type mismatch: expecting %s but found %s";_tag_fat(_tmp2A9,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp6E7,_tmp6E6,_tag_fat(_tmp2A8,sizeof(void*),2U));});});
return 0;}}}
# 1503
return 1;}
# 1508
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2AC=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp46=_tmp2AC;void*_tmp2AD=_stmttmp46;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AD)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1514
return 1;}
# 1518
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 1522
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp2AE=0U;({unsigned _tmp6E9=e->loc;struct _fat_ptr _tmp6E8=({const char*_tmp2AF="integral size mismatch; conversion supplied";_tag_fat(_tmp2AF,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6E9,_tmp6E8,_tag_fat(_tmp2AE,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1528
return 0;}
# 1532
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 1536
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp2B0=0U;({unsigned _tmp6EB=e->loc;struct _fat_ptr _tmp6EA=({const char*_tmp2B1="integral size mismatch; conversion supplied";_tag_fat(_tmp2B1,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp6EB,_tmp6EA,_tag_fat(_tmp2B0,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1542
return 0;}
# 1547
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1553
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp6EC=t;Cyc_Unify_unify(_tmp6EC,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1558
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2B2=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2B2;
void*_tmp2B3=t;struct Cyc_Absyn_Exp*_tmp2B4;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B3)->tag == 9U){_LL1: _tmp2B4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B3)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2B4;
return e;}}else{_LL3: _LL4: {
# 1563
struct Cyc_Absyn_Exp*_tmp2B5=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2B5;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1572
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2B6=Cyc_Tcutil_compress(b);void*_stmttmp47=_tmp2B6;void*_tmp2B7=_stmttmp47;struct Cyc_List_List*_tmp2B8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2B8=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B7)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2B8;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp2BB=({struct Cyc_String_pa_PrintArg_struct _tmp614;_tmp614.tag=0U,({struct _fat_ptr _tmp6ED=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp614.f1=_tmp6ED;});_tmp614;});void*_tmp2B9[1U];_tmp2B9[0]=& _tmp2BB;({struct _fat_ptr _tmp6EE=({const char*_tmp2BA="get_bounds_exp: %s";_tag_fat(_tmp2BA,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6EE,_tag_fat(_tmp2B9,sizeof(void*),1U));});});}_LL0:;}}
# 1581
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2BC=Cyc_Tcutil_compress(t);void*_stmttmp48=_tmp2BC;void*_tmp2BD=_stmttmp48;void*_tmp2BE;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BD)->tag == 3U){_LL1: _tmp2BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BD)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2BE;
# 1584
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2C1=({struct Cyc_String_pa_PrintArg_struct _tmp615;_tmp615.tag=0U,({struct _fat_ptr _tmp6EF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp615.f1=_tmp6EF;});_tmp615;});void*_tmp2BF[1U];_tmp2BF[0]=& _tmp2C1;({struct _fat_ptr _tmp6F0=({const char*_tmp2C0="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp2C0,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6F0,_tag_fat(_tmp2BF,sizeof(void*),1U));});});}_LL0:;}
# 1590
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2C2=_cycalloc(sizeof(*_tmp2C2));_tmp2C2->v=tvs;_tmp2C2;}));}
# 1594
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2C3=_cycalloc(sizeof(*_tmp2C3));_tmp2C3->v=tvs;_tmp2C3;}));}
# 1599
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1607
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp2C4=({struct _tuple15 _tmp619;_tmp619.f1=t1,_tmp619.f2=t2;_tmp619;});struct _tuple15 _stmttmp49=_tmp2C4;struct _tuple15 _tmp2C5=_stmttmp49;void*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;struct Cyc_Absyn_Tqual _tmp2CB;void*_tmp2CA;void*_tmp2C9;struct Cyc_Absyn_Exp*_tmp2C8;struct Cyc_Absyn_Tqual _tmp2C7;void*_tmp2C6;struct Cyc_Absyn_PtrInfo _tmp2CF;struct Cyc_Absyn_PtrInfo _tmp2CE;switch(*((int*)_tmp2C5.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5.f2)->tag == 3U){_LL1: _tmp2CE=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5.f1)->f1;_tmp2CF=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C5.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2CE;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2CF;
# 1612
int okay=1;
# 1614
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable))
# 1616
okay=!Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable);
# 1618
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple22 _tmp2D0=({struct _tuple22 _tmp616;({struct Cyc_Absyn_Exp*_tmp6F4=({void*_tmp6F3=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6F3,(pinfo_a.ptr_atts).bounds);});_tmp616.f1=_tmp6F4;}),({
struct Cyc_Absyn_Exp*_tmp6F2=({void*_tmp6F1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6F1,(pinfo_b.ptr_atts).bounds);});_tmp616.f2=_tmp6F2;});_tmp616;});
# 1619
struct _tuple22 _stmttmp4A=_tmp2D0;struct _tuple22 _tmp2D1=_stmttmp4A;struct Cyc_Absyn_Exp*_tmp2D3;struct Cyc_Absyn_Exp*_tmp2D2;if(_tmp2D1.f2 == 0){_LLA: _LLB:
# 1622
 okay=1;goto _LL9;}else{if(_tmp2D1.f1 == 0){_LLC: _LLD:
# 1625
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp6F5=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6F5,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp2D2=_tmp2D1.f1;_tmp2D3=_tmp2D1.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2D2;struct Cyc_Absyn_Exp*e2=_tmp2D3;
# 1631
okay=okay &&({struct Cyc_Absyn_Exp*_tmp6F6=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp6F6,(struct Cyc_Absyn_Exp*)_check_null(e1));});
# 1635
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({void*_tmp2D4=0U;({unsigned _tmp6F8=loc;struct _fat_ptr _tmp6F7=({const char*_tmp2D5="implicit cast to shorter array";_tag_fat(_tmp2D5,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp6F8,_tmp6F7,_tag_fat(_tmp2D4,sizeof(void*),0U));});});
goto _LL9;}}}_LL9:;}
# 1641
okay=okay &&(!(pinfo_a.elt_tq).real_const ||(pinfo_b.elt_tq).real_const);
# 1643
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
# 1646
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp2D8=({struct Cyc_String_pa_PrintArg_struct _tmp618;_tmp618.tag=0U,({
struct _fat_ptr _tmp6F9=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((pinfo_a.ptr_atts).rgn));_tmp618.f1=_tmp6F9;});_tmp618;});struct Cyc_String_pa_PrintArg_struct _tmp2D9=({struct Cyc_String_pa_PrintArg_struct _tmp617;_tmp617.tag=0U,({
struct _fat_ptr _tmp6FA=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((pinfo_b.ptr_atts).rgn));_tmp617.f1=_tmp6FA;});_tmp617;});void*_tmp2D6[2U];_tmp2D6[0]=& _tmp2D8,_tmp2D6[1]=& _tmp2D9;({unsigned _tmp6FC=loc;struct _fat_ptr _tmp6FB=({const char*_tmp2D7="implicit cast from region %s to region %s";_tag_fat(_tmp2D7,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp6FC,_tmp6FB,_tag_fat(_tmp2D6,sizeof(void*),2U));});});}else{
okay=0;}}
# 1653
okay=okay &&(Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)||
# 1655
 Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)&&(pinfo_b.elt_tq).real_const);{
# 1663
int _tmp2DA=
({void*_tmp6FD=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp6FD,(pinfo_b.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term);
# 1663
int deep_subtype=_tmp2DA;
# 1667
okay=okay &&(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type)||
(deep_subtype &&((pinfo_b.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type))))&& Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type));
return okay;}}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->tag == 4U){_LL3: _tmp2C6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f1)->f1).elt_type;_tmp2C7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f1)->f1).tq;_tmp2C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f1)->f1).num_elts;_tmp2C9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f1)->f1).zero_term;_tmp2CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->f1).elt_type;_tmp2CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->f1).tq;_tmp2CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->f1).num_elts;_tmp2CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C5.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2C6;struct Cyc_Absyn_Tqual tq1a=_tmp2C7;struct Cyc_Absyn_Exp*e1=_tmp2C8;void*zt1=_tmp2C9;void*t2a=_tmp2CA;struct Cyc_Absyn_Tqual tq2a=_tmp2CB;struct Cyc_Absyn_Exp*e2=_tmp2CC;void*zt2=_tmp2CD;
# 1673
int okay;
# 1676
okay=Cyc_Unify_unify(zt1,zt2)&&(
(e1 != 0 && e2 != 0)&& Cyc_Evexp_same_uint_const_exp(e1,e2));
# 1679
return(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C5.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C5.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C5.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1681
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1683
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1687
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2DB=Cyc_Tcutil_compress(t);void*_stmttmp4B=_tmp2DB;void*_tmp2DC=_stmttmp4B;void*_tmp2DD;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->tag == 3U){_LL1: _tmp2DD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2DC)->f1).elt_type;_LL2: {void*e=_tmp2DD;
return e;}}else{_LL3: _LL4:
({void*_tmp2DE=0U;({struct _fat_ptr _tmp6FE=({const char*_tmp2DF="pointer_elt_type";_tag_fat(_tmp2DF,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6FE,_tag_fat(_tmp2DE,sizeof(void*),0U));});});}_LL0:;}
# 1693
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2E0=Cyc_Tcutil_compress(t);void*_stmttmp4C=_tmp2E0;void*_tmp2E1=_stmttmp4C;struct Cyc_Absyn_PtrAtts*_tmp2E2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->tag == 3U){_LL1: _tmp2E2=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E1)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2E2;
return p->rgn;}}else{_LL3: _LL4:
({void*_tmp2E3=0U;({struct _fat_ptr _tmp6FF=({const char*_tmp2E4="pointer_elt_type";_tag_fat(_tmp2E4,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6FF,_tag_fat(_tmp2E3,sizeof(void*),0U));});});}_LL0:;}
# 1700
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2E5=Cyc_Tcutil_compress(t);void*_stmttmp4D=_tmp2E5;void*_tmp2E6=_stmttmp4D;void*_tmp2E7;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6)->tag == 3U){_LL1: _tmp2E7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E6)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2E7;
# 1703
*rgn=r;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1712
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2E8=Cyc_Tcutil_compress(t);void*_stmttmp4E=_tmp2E8;void*_tmp2E9=_stmttmp4E;void*_tmp2EB;void*_tmp2EA;switch(*((int*)_tmp2E9)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2E9)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2EA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9)->f1).ptr_atts).nullable;_tmp2EB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2EA;void*bounds=_tmp2EB;
# 1720
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1726
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2EC=e->r;void*_stmttmp4F=_tmp2EC;void*_tmp2ED=_stmttmp4F;struct Cyc_Absyn_Exp*_tmp2EF;void*_tmp2EE;struct _fat_ptr _tmp2F0;switch(*((int*)_tmp2ED)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2F0=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp2F0;
# 1735
unsigned long _tmp2F1=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2F1;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 1745
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1733
 return 1;case 14U: _LLD: _tmp2EE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2ED)->f1;_tmp2EF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2ED)->f2;_LLE: {void*t=_tmp2EE;struct Cyc_Absyn_Exp*e2=_tmp2EF;
# 1746
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1751
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1760
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1765
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1770
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1779
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1784
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1789
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2F2=ka;enum Cyc_Absyn_AliasQual _tmp2F4;enum Cyc_Absyn_KindQual _tmp2F3;_LL1: _tmp2F3=_tmp2F2->kind;_tmp2F4=_tmp2F2->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp2F3;enum Cyc_Absyn_AliasQual a=_tmp2F4;
{enum Cyc_Absyn_AliasQual _tmp2F5=a;switch(_tmp2F5){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1793
enum Cyc_Absyn_KindQual _tmp2F6=k;switch(_tmp2F6){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1804
{enum Cyc_Absyn_KindQual _tmp2F7=k;switch(_tmp2F7){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1811
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1813
{enum Cyc_Absyn_KindQual _tmp2F8=k;switch(_tmp2F8){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1820
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1823
({struct Cyc_String_pa_PrintArg_struct _tmp2FB=({struct Cyc_String_pa_PrintArg_struct _tmp61A;_tmp61A.tag=0U,({struct _fat_ptr _tmp700=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp61A.f1=_tmp700;});_tmp61A;});void*_tmp2F9[1U];_tmp2F9[0]=& _tmp2FB;({struct _fat_ptr _tmp701=({const char*_tmp2FA="kind_to_opt: bad kind %s\n";_tag_fat(_tmp2FA,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp701,_tag_fat(_tmp2F9,sizeof(void*),1U));});});}}
# 1826
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp2FC=_cycalloc(sizeof(*_tmp2FC));_tmp2FC->tag=0U,_tmp2FC->f1=k;_tmp2FC;});}
# 1829
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp2FD=_cycalloc(sizeof(*_tmp2FD));({void*_tmp702=Cyc_Tcutil_kind_to_bound(k);_tmp2FD->v=_tmp702;});_tmp2FD;});}
# 1835
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp2FE=Cyc_Tcutil_compress(t2);void*_stmttmp50=_tmp2FE;void*_tmp2FF=_stmttmp50;void*_tmp300;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->tag == 3U){_LL1: _tmp300=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp300;
# 1840
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp703=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp703;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1849
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1855
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1859
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp314=_cycalloc(sizeof(*_tmp314));_tmp314->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp706=({struct _fat_ptr*_tmp313=_cycalloc(sizeof(*_tmp313));({struct _fat_ptr _tmp705=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp312=({struct Cyc_Int_pa_PrintArg_struct _tmp61C;_tmp61C.tag=1U,_tmp61C.f1=(unsigned long)counter ++;_tmp61C;});void*_tmp310[1U];_tmp310[0]=& _tmp312;({struct _fat_ptr _tmp704=({const char*_tmp311="__aliasvar%d";_tag_fat(_tmp311,sizeof(char),13U);});Cyc_aprintf(_tmp704,_tag_fat(_tmp310,sizeof(void*),1U));});});*_tmp313=_tmp705;});_tmp313;});_tmp314->f2=_tmp706;});_tmp314;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp707=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp30F=_cycalloc(sizeof(*_tmp30F));_tmp30F->tag=4U,_tmp30F->f1=vd;_tmp30F;});Cyc_Absyn_varb_exp(_tmp707,e->loc);});
# 1868
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 1870
{void*_tmp301=Cyc_Tcutil_compress(e_type);void*_stmttmp51=_tmp301;void*_tmp302=_stmttmp51;struct Cyc_Absyn_PtrLoc*_tmp309;void*_tmp308;void*_tmp307;void*_tmp306;void*_tmp305;struct Cyc_Absyn_Tqual _tmp304;void*_tmp303;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->tag == 3U){_LL1: _tmp303=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).elt_type;_tmp304=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).elt_tq;_tmp305=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).ptr_atts).rgn;_tmp306=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).ptr_atts).nullable;_tmp307=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).ptr_atts).bounds;_tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).ptr_atts).zero_term;_tmp309=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp303;struct Cyc_Absyn_Tqual tq=_tmp304;void*old_r=_tmp305;void*nb=_tmp306;void*b=_tmp307;void*zt=_tmp308;struct Cyc_Absyn_PtrLoc*pl=_tmp309;
# 1872
{void*_tmp30A=Cyc_Tcutil_compress(old_r);void*_stmttmp52=_tmp30A;void*_tmp30B=_stmttmp52;struct Cyc_Core_Opt*_tmp30D;void**_tmp30C;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30B)->tag == 1U){_LL6: _tmp30C=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30B)->f2;_tmp30D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp30B)->f4;_LL7: {void**topt=_tmp30C;struct Cyc_Core_Opt*ts=_tmp30D;
# 1874
void*_tmp30E=Cyc_Absyn_var_type(tv);void*new_r=_tmp30E;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1879
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1883
e->topt=0;
vd->initializer=0;{
# 1887
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1889
return({struct _tuple23 _tmp61B;_tmp61B.f1=d,_tmp61B.f2=ve;_tmp61B;});}}
# 1894
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1897
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1902
void*_tmp315=Cyc_Tcutil_compress(wants_type);void*_stmttmp53=_tmp315;void*_tmp316=_stmttmp53;void*_tmp317;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp316)->tag == 3U){_LL1: _tmp317=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp316)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp317;
# 1904
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp318=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp318;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1910
return 0;}
# 1914
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1916
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1920
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1923
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1925
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp31A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp621;_tmp621.tag=0U,({struct _fat_ptr _tmp708=({const char*_tmp321="integral size mismatch; ";_tag_fat(_tmp321,sizeof(char),25U);});_tmp621.f1=_tmp708;});_tmp621;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp31B=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp620;_tmp620.tag=2U,_tmp620.f1=(void*)t1;_tmp620;});struct Cyc_Warn_String_Warn_Warg_struct _tmp31C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61F;_tmp61F.tag=0U,({
struct _fat_ptr _tmp709=({const char*_tmp320=" -> ";_tag_fat(_tmp320,sizeof(char),5U);});_tmp61F.f1=_tmp709;});_tmp61F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp31D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp61E;_tmp61E.tag=2U,_tmp61E.f1=(void*)t2;_tmp61E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp31E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp61D;_tmp61D.tag=0U,({struct _fat_ptr _tmp70A=({const char*_tmp31F=" conversion supplied";_tag_fat(_tmp31F,sizeof(char),21U);});_tmp61D.f1=_tmp70A;});_tmp61D;});void*_tmp319[5U];_tmp319[0]=& _tmp31A,_tmp319[1]=& _tmp31B,_tmp319[2]=& _tmp31C,_tmp319[3]=& _tmp31D,_tmp319[4]=& _tmp31E;({unsigned _tmp70B=e->loc;Cyc_Warn_warn2(_tmp70B,_tag_fat(_tmp319,sizeof(void*),5U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1933
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp324=({struct Cyc_String_pa_PrintArg_struct _tmp624;_tmp624.tag=0U,({
struct _fat_ptr _tmp70C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp624.f1=_tmp70C;});_tmp624;});struct Cyc_String_pa_PrintArg_struct _tmp325=({struct Cyc_String_pa_PrintArg_struct _tmp623;_tmp623.tag=0U,({struct _fat_ptr _tmp70D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp623.f1=_tmp70D;});_tmp623;});struct Cyc_String_pa_PrintArg_struct _tmp326=({struct Cyc_String_pa_PrintArg_struct _tmp622;_tmp622.tag=0U,({struct _fat_ptr _tmp70E=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp622.f1=_tmp70E;});_tmp622;});void*_tmp322[3U];_tmp322[0]=& _tmp324,_tmp322[1]=& _tmp325,_tmp322[2]=& _tmp326;({unsigned _tmp710=e->loc;struct _fat_ptr _tmp70F=({const char*_tmp323="implicit alias coercion for %s:%s to %s";_tag_fat(_tmp323,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp710,_tmp70F,_tag_fat(_tmp322,sizeof(void*),3U));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1941
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1945
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1949
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp329=({struct Cyc_String_pa_PrintArg_struct _tmp626;_tmp626.tag=0U,({struct _fat_ptr _tmp711=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp626.f1=_tmp711;});_tmp626;});struct Cyc_String_pa_PrintArg_struct _tmp32A=({struct Cyc_String_pa_PrintArg_struct _tmp625;_tmp625.tag=0U,({struct _fat_ptr _tmp712=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp625.f1=_tmp712;});_tmp625;});void*_tmp327[2U];_tmp327[0]=& _tmp329,_tmp327[1]=& _tmp32A;({unsigned _tmp714=e->loc;struct _fat_ptr _tmp713=({const char*_tmp328="implicit cast from %s to %s";_tag_fat(_tmp328,sizeof(char),28U);});Cyc_Tcutil_warn(_tmp714,_tmp713,_tag_fat(_tmp327,sizeof(void*),2U));});});
return 1;}
# 1955
return 0;}
# 1961
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1964
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1978 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1981
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1984
struct _tuple24 _tmp32B=*env;struct _tuple24 _stmttmp54=_tmp32B;struct _tuple24 _tmp32C=_stmttmp54;int _tmp32F;struct _RegionHandle*_tmp32E;struct Cyc_List_List*_tmp32D;_LL1: _tmp32D=_tmp32C.f1;_tmp32E=_tmp32C.f2;_tmp32F=_tmp32C.f3;_LL2: {struct Cyc_List_List*inst=_tmp32D;struct _RegionHandle*r=_tmp32E;int flatten=_tmp32F;
void*_tmp330=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp330;
struct Cyc_List_List*_tmp331=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp331;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp333=_region_malloc(r,sizeof(*_tmp333));({struct _tuple12*_tmp715=({struct _tuple12*_tmp332=_region_malloc(r,sizeof(*_tmp332));_tmp332->f1=x->tq,_tmp332->f2=t;_tmp332;});_tmp333->hd=_tmp715;}),_tmp333->tl=0;_tmp333;});
return ts;}}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1991
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 1993
struct _tuple25 _tmp334=*env;struct _tuple25 _stmttmp55=_tmp334;struct _tuple25 _tmp335=_stmttmp55;int _tmp337;struct _RegionHandle*_tmp336;_LL1: _tmp336=_tmp335.f1;_tmp337=_tmp335.f2;_LL2: {struct _RegionHandle*r=_tmp336;int flatten=_tmp337;
struct _tuple12 _tmp338=*x;struct _tuple12 _stmttmp56=_tmp338;struct _tuple12 _tmp339=_stmttmp56;void*_tmp33B;struct Cyc_Absyn_Tqual _tmp33A;_LL4: _tmp33A=_tmp339.f1;_tmp33B=_tmp339.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp33A;void*t=_tmp33B;
struct Cyc_List_List*_tmp33C=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp33C;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp33E=_region_malloc(r,sizeof(*_tmp33E));({struct _tuple12*_tmp716=({struct _tuple12*_tmp33D=_region_malloc(r,sizeof(*_tmp33D));_tmp33D->f1=tq,_tmp33D->f2=t;_tmp33D;});_tmp33E->hd=_tmp716;}),_tmp33E->tl=0;_tmp33E;});
return ts;}}}
# 2000
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 2003
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp33F=t1;struct Cyc_List_List*_tmp340;struct Cyc_List_List*_tmp341;struct Cyc_List_List*_tmp343;struct Cyc_Absyn_Aggrdecl*_tmp342;switch(*((int*)_tmp33F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp342=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f1)->f1).KnownAggr).val;_tmp343=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp33F)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp342;struct Cyc_List_List*ts=_tmp343;
# 2021
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 2023
return({struct Cyc_List_List*_tmp34E=_region_malloc(r,sizeof(*_tmp34E));({struct _tuple12*_tmp718=({struct _tuple12*_tmp34D=_region_malloc(r,sizeof(*_tmp34D));({struct Cyc_Absyn_Tqual _tmp717=Cyc_Absyn_empty_tqual(0U);_tmp34D->f1=_tmp717;}),_tmp34D->f2=t1;_tmp34D;});_tmp34E->hd=_tmp718;}),_tmp34E->tl=0;_tmp34E;});{
struct Cyc_List_List*_tmp34F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp34F;
struct _tuple24 env=({struct _tuple24 _tmp627;_tmp627.f1=inst,_tmp627.f2=r,_tmp627.f3=flatten;_tmp627;});
struct Cyc_List_List*_tmp350=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp57=_tmp350;struct Cyc_List_List*_tmp351=_stmttmp57;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_Aggrfield*_tmp352;if(_tmp351 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp352=(struct Cyc_Absyn_Aggrfield*)_tmp351->hd;_tmp353=_tmp351->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp352;struct Cyc_List_List*tl=_tmp353;
# 2029
struct Cyc_List_List*_tmp354=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp354;
env.f3=0;{
struct Cyc_List_List*_tmp355=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp355;
struct Cyc_List_List*_tmp356=({struct Cyc_List_List*_tmp357=_region_malloc(r,sizeof(*_tmp357));_tmp357->hd=hd2,_tmp357->tl=tl2;_tmp357;});struct Cyc_List_List*tts=_tmp356;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp341=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp33F)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp341;
# 2008
struct _tuple25 _tmp344=({struct _tuple25 _tmp628;_tmp628.f1=r,_tmp628.f2=flatten;_tmp628;});struct _tuple25 env=_tmp344;
# 2010
struct Cyc_List_List*_tmp345=tqs;struct Cyc_List_List*_tmp347;struct _tuple12*_tmp346;if(_tmp345 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp346=(struct _tuple12*)_tmp345->hd;_tmp347=_tmp345->tl;_LLF: {struct _tuple12*hd=_tmp346;struct Cyc_List_List*tl=_tmp347;
# 2013
struct Cyc_List_List*_tmp348=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp348;
env.f2=0;{
struct Cyc_List_List*_tmp349=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp349;
struct Cyc_List_List*_tmp34A=({struct Cyc_List_List*_tmp34C=_region_malloc(r,sizeof(*_tmp34C));_tmp34C->hd=hd2,_tmp34C->tl=tl2;_tmp34C;});struct Cyc_List_List*temp=_tmp34A;
return({struct _RegionHandle*_tmp719=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp719,({struct Cyc_List_List*_tmp34B=_region_malloc(r,sizeof(*_tmp34B));_tmp34B->hd=hd2,_tmp34B->tl=tl2;_tmp34B;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33F)->f1 == Cyc_Absyn_StructA){_LL7: _tmp340=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp33F)->f2;_LL8: {struct Cyc_List_List*fs=_tmp340;
# 2036
struct _tuple24 env=({struct _tuple24 _tmp629;_tmp629.f1=0,_tmp629.f2=r,_tmp629.f3=flatten;_tmp629;});
struct Cyc_List_List*_tmp358=fs;struct Cyc_List_List*_tmp35A;struct Cyc_Absyn_Aggrfield*_tmp359;if(_tmp358 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp359=(struct Cyc_Absyn_Aggrfield*)_tmp358->hd;_tmp35A=_tmp358->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp359;struct Cyc_List_List*tl=_tmp35A;
# 2040
struct Cyc_List_List*_tmp35B=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp35B;
env.f3=0;{
struct Cyc_List_List*_tmp35C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp35C;
struct Cyc_List_List*_tmp35D=({struct Cyc_List_List*_tmp35E=_region_malloc(r,sizeof(*_tmp35E));_tmp35E->hd=hd2,_tmp35E->tl=tl2;_tmp35E;});struct Cyc_List_List*tts=_tmp35D;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2046
 goto _LL0;}_LL0:;}}
# 2049
return({struct Cyc_List_List*_tmp360=_region_malloc(r,sizeof(*_tmp360));({struct _tuple12*_tmp71B=({struct _tuple12*_tmp35F=_region_malloc(r,sizeof(*_tmp35F));({struct Cyc_Absyn_Tqual _tmp71A=Cyc_Absyn_empty_tqual(0U);_tmp35F->f1=_tmp71A;}),_tmp35F->f2=t1;_tmp35F;});_tmp360->hd=_tmp71B;}),_tmp360->tl=0;_tmp360;});}
# 2053
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp361=(void*)t->hd;void*_stmttmp58=_tmp361;void*_tmp362=_stmttmp58;switch(*((int*)_tmp362)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
# 2060
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2063
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2066
return 1;}
# 2069
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp363=({struct _tuple15 _tmp62A;({void*_tmp71D=Cyc_Tcutil_compress(t1);_tmp62A.f1=_tmp71D;}),({void*_tmp71C=Cyc_Tcutil_compress(t2);_tmp62A.f2=_tmp71C;});_tmp62A;});struct _tuple15 _stmttmp59=_tmp363;struct _tuple15 _tmp364=_stmttmp59;enum Cyc_Absyn_Size_of _tmp366;enum Cyc_Absyn_Size_of _tmp365;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f2)->f1)->tag == 1U){_LL1: _tmp365=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f1)->f1)->f2;_tmp366=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp364.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp365;enum Cyc_Absyn_Size_of b2=_tmp366;
# 2072
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2080
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2082
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp367=({struct _tuple15 _tmp62B;_tmp62B.f1=t1,_tmp62B.f2=t2;_tmp62B;});struct _tuple15 _stmttmp5A=_tmp367;struct _tuple15 _tmp368=_stmttmp5A;struct Cyc_Absyn_FnInfo _tmp36A;struct Cyc_Absyn_FnInfo _tmp369;struct Cyc_List_List*_tmp36F;struct Cyc_Absyn_Datatypedecl*_tmp36E;struct Cyc_List_List*_tmp36D;struct Cyc_Absyn_Datatypefield*_tmp36C;struct Cyc_Absyn_Datatypedecl*_tmp36B;void*_tmp37B;void*_tmp37A;void*_tmp379;void*_tmp378;struct Cyc_Absyn_Tqual _tmp377;void*_tmp376;void*_tmp375;void*_tmp374;void*_tmp373;void*_tmp372;struct Cyc_Absyn_Tqual _tmp371;void*_tmp370;switch(*((int*)_tmp368.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->tag == 3U){_LL1: _tmp370=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).elt_type;_tmp371=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).elt_tq;_tmp372=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).ptr_atts).rgn;_tmp373=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).ptr_atts).nullable;_tmp374=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).ptr_atts).bounds;_tmp375=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f1)->f1).ptr_atts).zero_term;_tmp376=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).elt_type;_tmp377=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).elt_tq;_tmp378=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).ptr_atts).rgn;_tmp379=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).ptr_atts).nullable;_tmp37A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).ptr_atts).bounds;_tmp37B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp368.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp370;struct Cyc_Absyn_Tqual q_a=_tmp371;void*rt_a=_tmp372;void*null_a=_tmp373;void*b_a=_tmp374;void*zt_a=_tmp375;void*t_b=_tmp376;struct Cyc_Absyn_Tqual q_b=_tmp377;void*rt_b=_tmp378;void*null_b=_tmp379;void*b_b=_tmp37A;void*zt_b=_tmp37B;
# 2094
if(q_a.real_const && !q_b.real_const)
return 0;
# 2097
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 2101
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 2105
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 2109
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp37C=({void*_tmp71E=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp71E,b_a);});struct Cyc_Absyn_Exp*e1=_tmp37C;
struct Cyc_Absyn_Exp*_tmp37D=({void*_tmp71F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp71F,b_b);});struct Cyc_Absyn_Exp*e2=_tmp37D;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 2119
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 2125
int _tmp37E=
({void*_tmp720=b_b;Cyc_Unify_unify(_tmp720,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2125
int deep_subtype=_tmp37E;
# 2130
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp724=po;struct Cyc_List_List*_tmp723=({struct Cyc_List_List*_tmp380=_cycalloc(sizeof(*_tmp380));({struct _tuple15*_tmp721=({struct _tuple15*_tmp37F=_cycalloc(sizeof(*_tmp37F));_tmp37F->f1=t1,_tmp37F->f2=t2;_tmp37F;});_tmp380->hd=_tmp721;}),_tmp380->tl=assume;_tmp380;});void*_tmp722=t_a;Cyc_Tcutil_ptrsubtype(_tmp724,_tmp723,_tmp722,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp36B=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp36C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp36D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f1)->f2;_tmp36E=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->f1)->f1).KnownDatatype).val;_tmp36F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp368.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp36B;struct Cyc_Absyn_Datatypefield*df=_tmp36C;struct Cyc_List_List*ts1=_tmp36D;struct Cyc_Absyn_Datatypedecl*dd2=_tmp36E;struct Cyc_List_List*ts2=_tmp36F;
# 2137
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 2139
if(({int _tmp725=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp725 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp368.f2)->tag == 5U){_LL5: _tmp369=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp368.f1)->f1;_tmp36A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp368.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp369;struct Cyc_Absyn_FnInfo f2=_tmp36A;
# 2146
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp381=f1.tvars;struct Cyc_List_List*tvs1=_tmp381;
struct Cyc_List_List*_tmp382=f2.tvars;struct Cyc_List_List*tvs2=_tmp382;
if(({int _tmp726=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp726 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp384=_cycalloc(sizeof(*_tmp384));({struct _tuple19*_tmp728=({struct _tuple19*_tmp383=_cycalloc(sizeof(*_tmp383));_tmp383->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp727=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp383->f2=_tmp727;});_tmp383;});_tmp384->hd=_tmp728;}),_tmp384->tl=inst;_tmp384;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2157
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp72B=po;struct Cyc_List_List*_tmp72A=assume;void*_tmp729=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp385=_cycalloc(sizeof(*_tmp385));_tmp385->tag=5U,_tmp385->f1=f1;_tmp385;});Cyc_Tcutil_subtype(_tmp72B,_tmp72A,_tmp729,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp386=_cycalloc(sizeof(*_tmp386));_tmp386->tag=5U,_tmp386->f1=f2;_tmp386;}));});}}}
# 2164
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp387=f1.args;struct Cyc_List_List*args1=_tmp387;
struct Cyc_List_List*_tmp388=f2.args;struct Cyc_List_List*args2=_tmp388;
# 2169
if(({int _tmp72C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp72C != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2171
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp389=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp5B=_tmp389;struct _tuple9 _tmp38A=_stmttmp5B;void*_tmp38C;struct Cyc_Absyn_Tqual _tmp38B;_LLA: _tmp38B=_tmp38A.f2;_tmp38C=_tmp38A.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp38B;void*t1=_tmp38C;
struct _tuple9 _tmp38D=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp5C=_tmp38D;struct _tuple9 _tmp38E=_stmttmp5C;void*_tmp390;struct Cyc_Absyn_Tqual _tmp38F;_LLD: _tmp38F=_tmp38E.f2;_tmp390=_tmp38E.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp38F;void*t2=_tmp390;
# 2175
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2179
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp391=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp391;
struct Cyc_Absyn_VarargInfo _tmp392=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp392;
# 2184
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2189
if(!({void*_tmp72D=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp72D,(void*)_check_null(f2.effect));}))return 0;
# 2191
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2193
if(!Cyc_Tcutil_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2195
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2197
return 0;
# 2199
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2201
return 0;
# 2203
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2214
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2218
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp393=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp5D=_tmp393;struct _tuple12*_tmp394=_stmttmp5D;void*_tmp396;struct Cyc_Absyn_Tqual _tmp395;_LL1: _tmp395=_tmp394->f1;_tmp396=_tmp394->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp395;void*t1a=_tmp396;
struct _tuple12*_tmp397=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp5E=_tmp397;struct _tuple12*_tmp398=_stmttmp5E;void*_tmp39A;struct Cyc_Absyn_Tqual _tmp399;_LL4: _tmp399=_tmp398->f1;_tmp39A=_tmp398->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp399;void*t2a=_tmp39A;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a))
continue;
if(Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2234
return 1;}
# 2239
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2245
{void*_tmp39B=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39B)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2248
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2250
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2254
void*_tmp39C=t1;void*_tmp39D;struct Cyc_Absyn_Enumdecl*_tmp39E;void*_tmp3A2;struct Cyc_Absyn_Exp*_tmp3A1;struct Cyc_Absyn_Tqual _tmp3A0;void*_tmp39F;void*_tmp3A8;void*_tmp3A7;void*_tmp3A6;void*_tmp3A5;struct Cyc_Absyn_Tqual _tmp3A4;void*_tmp3A3;switch(*((int*)_tmp39C)){case 3U: _LLA: _tmp3A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).elt_type;_tmp3A4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).elt_tq;_tmp3A5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).ptr_atts).rgn;_tmp3A6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).ptr_atts).nullable;_tmp3A7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).ptr_atts).bounds;_tmp3A8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp39C)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp3A3;struct Cyc_Absyn_Tqual q_a=_tmp3A4;void*rt_a=_tmp3A5;void*null_a=_tmp3A6;void*b_a=_tmp3A7;void*zt_a=_tmp3A8;
# 2263
{void*_tmp3A9=t2;void*_tmp3AF;void*_tmp3AE;void*_tmp3AD;void*_tmp3AC;struct Cyc_Absyn_Tqual _tmp3AB;void*_tmp3AA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A9)->tag == 3U){_LL19: _tmp3AA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A9)->f1).elt_type;_tmp3AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A9)->f1).elt_tq;_tmp3AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A9)->f1).ptr_atts).rgn;_tmp3AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A9)->f1).ptr_atts).nullable;_tmp3AE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A9)->f1).ptr_atts).bounds;_tmp3AF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3A9)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3AA;struct Cyc_Absyn_Tqual q_b=_tmp3AB;void*rt_b=_tmp3AC;void*null_b=_tmp3AD;void*b_b=_tmp3AE;void*zt_b=_tmp3AF;
# 2265
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3B0=({struct Cyc_List_List*_tmp3B8=_cycalloc(sizeof(*_tmp3B8));({struct _tuple15*_tmp72E=({struct _tuple15*_tmp3B7=_cycalloc(sizeof(*_tmp3B7));_tmp3B7->f1=t1,_tmp3B7->f2=t2;_tmp3B7;});_tmp3B8->hd=_tmp72E;}),_tmp3B8->tl=0;_tmp3B8;});struct Cyc_List_List*assump=_tmp3B0;
int _tmp3B1=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3B1;
# 2278 "tcutil.cyc"
int _tmp3B2=
({void*_tmp72F=b_b;Cyc_Unify_unify(_tmp72F,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2278
int deep_castable=_tmp3B2;
# 2281
int _tmp3B3=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2281
int ptrsub=_tmp3B3;
# 2284
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2286
int _tmp3B4=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2286
int bitcase=_tmp3B4;
# 2289
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3B5=({void*_tmp730=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp730,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3B5;
struct Cyc_Absyn_Exp*_tmp3B6=({void*_tmp731=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp731,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3B6;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2298
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2303
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2309
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp39F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39C)->f1).elt_type;_tmp3A0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39C)->f1).tq;_tmp3A1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39C)->f1).num_elts;_tmp3A2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp39C)->f1).zero_term;_LLD: {void*t1a=_tmp39F;struct Cyc_Absyn_Tqual tq1a=_tmp3A0;struct Cyc_Absyn_Exp*e1=_tmp3A1;void*zt1=_tmp3A2;
# 2311
{void*_tmp3B9=t2;void*_tmp3BD;struct Cyc_Absyn_Exp*_tmp3BC;struct Cyc_Absyn_Tqual _tmp3BB;void*_tmp3BA;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B9)->tag == 4U){_LL1E: _tmp3BA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B9)->f1).elt_type;_tmp3BB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B9)->f1).tq;_tmp3BC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B9)->f1).num_elts;_tmp3BD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3B9)->f1).zero_term;_LL1F: {void*t2a=_tmp3BA;struct Cyc_Absyn_Tqual tq2a=_tmp3BB;struct Cyc_Absyn_Exp*e2=_tmp3BC;void*zt2=_tmp3BD;
# 2313
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2318
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2320
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2322
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39C)->f1)){case 15U: _LLE: _tmp39E=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39C)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp39E;
# 2326
{void*_tmp3BE=t2;struct Cyc_Absyn_Enumdecl*_tmp3BF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BE)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BE)->f1)->tag == 15U){_LL23: _tmp3BF=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BE)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3BF;
# 2328
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp732=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp732 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2334
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2337
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39C)->f2 != 0){_LL14: _tmp39D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp39C)->f2)->hd;_LL15: {void*r1=_tmp39D;
# 2340
{void*_tmp3C0=t2;void*_tmp3C1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->f2 != 0){_LL28: _tmp3C1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C0)->f2)->hd;_LL29: {void*r2=_tmp3C1;
# 2342
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2346
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2352
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3C2=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3C2;
({void*_tmp733=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3C3=_cycalloc(sizeof(*_tmp3C3));_tmp3C3->tag=14U,_tmp3C3->f1=t,_tmp3C3->f2=inner,_tmp3C3->f3=0,_tmp3C3->f4=c;_tmp3C3;});e->r=_tmp733;});
e->topt=t;}}
# 2361
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2366
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3C9=({struct Cyc_Int_pa_PrintArg_struct _tmp62C;_tmp62C.tag=1U,_tmp62C.f1=(unsigned long)i;_tmp62C;});void*_tmp3C7[1U];_tmp3C7[0]=& _tmp3C9;({struct _fat_ptr _tmp734=({const char*_tmp3C8="#%d";_tag_fat(_tmp3C8,sizeof(char),4U);});Cyc_aprintf(_tmp734,_tag_fat(_tmp3C7,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp3C6=_cycalloc(sizeof(*_tmp3C6));({struct _fat_ptr*_tmp735=({unsigned _tmp3C5=1;struct _fat_ptr*_tmp3C4=_cycalloc(_check_times(_tmp3C5,sizeof(struct _fat_ptr)));_tmp3C4[0]=s;_tmp3C4;});_tmp3C6->name=_tmp735;}),_tmp3C6->identity=- 1,_tmp3C6->kind=k;_tmp3C6;});}
# 2373
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp3CA=*t->name;struct _fat_ptr s=_tmp3CA;
return(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'#';}
# 2379
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2386
struct Cyc_List_List*_tmp3CB=0;struct Cyc_List_List*fn_type_atts=_tmp3CB;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3CC=_cycalloc(sizeof(*_tmp3CC));_tmp3CC->hd=(void*)atts->hd,_tmp3CC->tl=fn_type_atts;_tmp3CC;});}}{
struct Cyc_Absyn_FnInfo _tmp3CD=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3CD;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3CE=_cycalloc(sizeof(*_tmp3CE));_tmp3CE->tag=5U,_tmp3CE->f1=type_info;_tmp3CE;});}}
# 2394
return(void*)_check_null(fd->cached_type);}
# 2400
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2402
union Cyc_Relations_RelnOp _tmp3CF=*rop;union Cyc_Relations_RelnOp _stmttmp5F=_tmp3CF;union Cyc_Relations_RelnOp _tmp3D0=_stmttmp5F;struct Cyc_Absyn_Vardecl*_tmp3D1;struct Cyc_Absyn_Vardecl*_tmp3D2;switch((_tmp3D0.RNumelts).tag){case 2U: _LL1: _tmp3D2=(_tmp3D0.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3D2;
# 2404
struct _tuple1 _tmp3D3=*vd->name;struct _tuple1 _stmttmp60=_tmp3D3;struct _tuple1 _tmp3D4=_stmttmp60;struct _fat_ptr*_tmp3D6;union Cyc_Absyn_Nmspace _tmp3D5;_LL8: _tmp3D5=_tmp3D4.f1;_tmp3D6=_tmp3D4.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3D5;struct _fat_ptr*var=_tmp3D6;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp736=(struct _fat_ptr)*var;Cyc_strcmp(_tmp736,({const char*_tmp3D7="return_value";_tag_fat(_tmp3D7,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp737=Cyc_Relations_RReturn();*rop=_tmp737;});
goto _LL0;}{
# 2410
unsigned c=0U;
{struct Cyc_List_List*_tmp3D8=args;struct Cyc_List_List*a=_tmp3D8;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3D9=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp61=_tmp3D9;struct _tuple9*_tmp3DA=_stmttmp61;struct _fat_ptr*_tmp3DB;_LLB: _tmp3DB=_tmp3DA->f1;_LLC: {struct _fat_ptr*vopt=_tmp3DB;
if(vopt != 0){
if(Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp738=Cyc_Relations_RParam(c);*rop=_tmp738;});
break;}}}}}
# 2420
goto _LL0;}}}case 3U: _LL3: _tmp3D1=(_tmp3D0.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3D1;
# 2422
struct _tuple1 _tmp3DC=*vd->name;struct _tuple1 _stmttmp62=_tmp3DC;struct _tuple1 _tmp3DD=_stmttmp62;struct _fat_ptr*_tmp3DF;union Cyc_Absyn_Nmspace _tmp3DE;_LLE: _tmp3DE=_tmp3DD.f1;_tmp3DF=_tmp3DD.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3DE;struct _fat_ptr*var=_tmp3DF;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3E0=args;struct Cyc_List_List*a=_tmp3E0;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3E1=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp63=_tmp3E1;struct _tuple9*_tmp3E2=_stmttmp63;struct _fat_ptr*_tmp3E3;_LL11: _tmp3E3=_tmp3E2->f1;_LL12: {struct _fat_ptr*vopt=_tmp3E3;
if(vopt != 0){
if(Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp739=Cyc_Relations_RParamNumelts(c);*rop=_tmp739;});
break;}}}}}
# 2434
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2439
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2441
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2445
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2447
if(e == 0)return 0;{
struct Cyc_List_List*_tmp3E4=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3E4;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2454
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3E5=pr;void*_tmp3E7;struct Cyc_Absyn_Tqual _tmp3E6;_LL1: _tmp3E6=_tmp3E5->f1;_tmp3E7=_tmp3E5->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3E6;void*t2=_tmp3E7;
if(t2 == t)return pr;else{
return({struct _tuple12*_tmp3E8=_cycalloc(sizeof(*_tmp3E8));_tmp3E8->f1=tq,_tmp3E8->f2=t;_tmp3E8;});}}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2460
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2462
return({struct _tuple27*_tmp3EA=_region_malloc(rgn,sizeof(*_tmp3EA));({struct _tuple26*_tmp73A=({struct _tuple26*_tmp3E9=_region_malloc(rgn,sizeof(*_tmp3E9));_tmp3E9->f1=(*y).f1,_tmp3E9->f2=(*y).f2;_tmp3E9;});_tmp3EA->f1=_tmp73A;}),_tmp3EA->f2=(*y).f3;_tmp3EA;});}
# 2464
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2466
struct _tuple9 _tmp3EB=*orig_arg;struct _tuple9 _stmttmp64=_tmp3EB;struct _tuple9 _tmp3EC=_stmttmp64;void*_tmp3EF;struct Cyc_Absyn_Tqual _tmp3EE;struct _fat_ptr*_tmp3ED;_LL1: _tmp3ED=_tmp3EC.f1;_tmp3EE=_tmp3EC.f2;_tmp3EF=_tmp3EC.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp3ED;struct Cyc_Absyn_Tqual tq_orig=_tmp3EE;void*t_orig=_tmp3EF;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp3F0=_cycalloc(sizeof(*_tmp3F0));_tmp3F0->f1=vopt_orig,_tmp3F0->f2=tq_orig,_tmp3F0->f3=t;_tmp3F0;});}}
# 2470
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 2475
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2477
return({struct Cyc_Absyn_Exp*_tmp3F1=_cycalloc(sizeof(*_tmp3F1));_tmp3F1->topt=old->topt,_tmp3F1->r=r,_tmp3F1->loc=old->loc,_tmp3F1->annot=old->annot;_tmp3F1;});}
# 2482
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp3F2=e->r;void*_stmttmp65=_tmp3F2;void*_tmp3F3=_stmttmp65;void*_tmp3F4;struct Cyc_List_List*_tmp3F6;void*_tmp3F5;struct Cyc_Absyn_Exp*_tmp3F7;struct Cyc_Absyn_Exp*_tmp3F8;void*_tmp3F9;enum Cyc_Absyn_Coercion _tmp3FD;int _tmp3FC;struct Cyc_Absyn_Exp*_tmp3FB;void*_tmp3FA;struct Cyc_Absyn_Exp*_tmp3FF;struct Cyc_Absyn_Exp*_tmp3FE;struct Cyc_Absyn_Exp*_tmp401;struct Cyc_Absyn_Exp*_tmp400;struct Cyc_Absyn_Exp*_tmp403;struct Cyc_Absyn_Exp*_tmp402;struct Cyc_Absyn_Exp*_tmp406;struct Cyc_Absyn_Exp*_tmp405;struct Cyc_Absyn_Exp*_tmp404;struct Cyc_List_List*_tmp408;enum Cyc_Absyn_Primop _tmp407;switch(*((int*)_tmp3F3)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp407=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp408=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp407;struct Cyc_List_List*es=_tmp408;
# 2491
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp409=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp409;
struct Cyc_Absyn_Exp*_tmp40A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40A;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp73C=e;Cyc_Tcutil_copye(_tmp73C,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp40C=_cycalloc(sizeof(*_tmp40C));_tmp40C->tag=3U,_tmp40C->f1=p,({struct Cyc_List_List*_tmp73B=({struct Cyc_Absyn_Exp*_tmp40B[1U];_tmp40B[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp40B,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp40C->f2=_tmp73B;});_tmp40C;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp40D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp40D;
struct Cyc_Absyn_Exp*_tmp40E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp40E;
struct Cyc_Absyn_Exp*_tmp40F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp40F;
struct Cyc_Absyn_Exp*_tmp410=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp410;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp73E=e;Cyc_Tcutil_copye(_tmp73E,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp412=_cycalloc(sizeof(*_tmp412));_tmp412->tag=3U,_tmp412->f1=p,({struct Cyc_List_List*_tmp73D=({struct Cyc_Absyn_Exp*_tmp411[2U];_tmp411[0]=new_e1,_tmp411[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp411,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp412->f2=_tmp73D;});_tmp412;}));});}else{
return({void*_tmp413=0U;({struct _fat_ptr _tmp73F=({const char*_tmp414="primop does not have 1 or 2 args!";_tag_fat(_tmp414,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp73F,_tag_fat(_tmp413,sizeof(void*),0U));});});}}}case 6U: _LLD: _tmp404=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp405=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_tmp406=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp3F3)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp404;struct Cyc_Absyn_Exp*e2=_tmp405;struct Cyc_Absyn_Exp*e3=_tmp406;
# 2505
struct Cyc_Absyn_Exp*_tmp415=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp415;
struct Cyc_Absyn_Exp*_tmp416=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp416;
struct Cyc_Absyn_Exp*_tmp417=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp417;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp740=e;Cyc_Tcutil_copye(_tmp740,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp418=_cycalloc(sizeof(*_tmp418));_tmp418->tag=6U,_tmp418->f1=new_e1,_tmp418->f2=new_e2,_tmp418->f3=new_e3;_tmp418;}));});}case 7U: _LLF: _tmp402=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp403=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp402;struct Cyc_Absyn_Exp*e2=_tmp403;
# 2511
struct Cyc_Absyn_Exp*_tmp419=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp419;
struct Cyc_Absyn_Exp*_tmp41A=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp41A;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp741=e;Cyc_Tcutil_copye(_tmp741,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp41B=_cycalloc(sizeof(*_tmp41B));_tmp41B->tag=7U,_tmp41B->f1=new_e1,_tmp41B->f2=new_e2;_tmp41B;}));});}case 8U: _LL11: _tmp400=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp401=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp400;struct Cyc_Absyn_Exp*e2=_tmp401;
# 2516
struct Cyc_Absyn_Exp*_tmp41C=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41C;
struct Cyc_Absyn_Exp*_tmp41D=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp41D;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp742=e;Cyc_Tcutil_copye(_tmp742,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp41E=_cycalloc(sizeof(*_tmp41E));_tmp41E->tag=8U,_tmp41E->f1=new_e1,_tmp41E->f2=new_e2;_tmp41E;}));});}case 9U: _LL13: _tmp3FE=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp3FF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp3FE;struct Cyc_Absyn_Exp*e2=_tmp3FF;
# 2521
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41F;
struct Cyc_Absyn_Exp*_tmp420=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp420;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp743=e;Cyc_Tcutil_copye(_tmp743,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp421=_cycalloc(sizeof(*_tmp421));_tmp421->tag=9U,_tmp421->f1=new_e1,_tmp421->f2=new_e2;_tmp421;}));});}case 14U: _LL15: _tmp3FA=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp3FB=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_tmp3FC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F3)->f3;_tmp3FD=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3F3)->f4;_LL16: {void*t=_tmp3FA;struct Cyc_Absyn_Exp*e1=_tmp3FB;int b=_tmp3FC;enum Cyc_Absyn_Coercion c=_tmp3FD;
# 2526
struct Cyc_Absyn_Exp*_tmp422=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp422;
void*_tmp423=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp423;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp744=e;Cyc_Tcutil_copye(_tmp744,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp424=_cycalloc(sizeof(*_tmp424));_tmp424->tag=14U,_tmp424->f1=new_typ,_tmp424->f2=new_e1,_tmp424->f3=b,_tmp424->f4=c;_tmp424;}));});}case 17U: _LL17: _tmp3F9=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_LL18: {void*t=_tmp3F9;
# 2531
void*_tmp425=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp425;
if(new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp745=e;Cyc_Tcutil_copye(_tmp745,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp426=_cycalloc(sizeof(*_tmp426));_tmp426->tag=17U,_tmp426->f1=new_typ;_tmp426;}));});}case 18U: _LL19: _tmp3F8=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp3F8;
# 2535
struct Cyc_Absyn_Exp*_tmp427=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp427;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp746=e;Cyc_Tcutil_copye(_tmp746,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->tag=18U,_tmp428->f1=new_e1;_tmp428;}));});}case 41U: _LL1B: _tmp3F7=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp3F7;
# 2539
struct Cyc_Absyn_Exp*_tmp429=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp429;
if(new_e1 == e1)return e;
return({struct Cyc_Absyn_Exp*_tmp747=e;Cyc_Tcutil_copye(_tmp747,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp42A=_cycalloc(sizeof(*_tmp42A));_tmp42A->tag=41U,_tmp42A->f1=new_e1;_tmp42A;}));});}case 19U: _LL1D: _tmp3F5=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_tmp3F6=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp3F3)->f2;_LL1E: {void*t=_tmp3F5;struct Cyc_List_List*f=_tmp3F6;
# 2543
void*_tmp42B=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp42B;
if(new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp748=e;Cyc_Tcutil_copye(_tmp748,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->tag=19U,_tmp42C->f1=new_typ,_tmp42C->f2=f;_tmp42C;}));});}case 39U: _LL1F: _tmp3F4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp3F3)->f1;_LL20: {void*t=_tmp3F4;
# 2547
void*_tmp42D=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp42D;
if(new_typ == t)return e;{
# 2550
void*_tmp42E=Cyc_Tcutil_compress(new_typ);void*_stmttmp66=_tmp42E;void*_tmp42F=_stmttmp66;struct Cyc_Absyn_Exp*_tmp430;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp42F)->tag == 9U){_LL24: _tmp430=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp42F)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp430;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp749=e;Cyc_Tcutil_copye(_tmp749,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp431=_cycalloc(sizeof(*_tmp431));_tmp431->tag=39U,_tmp431->f1=new_typ;_tmp431;}));});}_LL23:;}}default: _LL21: _LL22:
# 2554
 return({void*_tmp432=0U;({struct _fat_ptr _tmp74A=({const char*_tmp433="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp433,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp74A,_tag_fat(_tmp432,sizeof(void*),0U));});});}_LL0:;}
# 2558
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2561
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 2565
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2568
void*_tmp434=f->type;void*t=_tmp434;
struct Cyc_Absyn_Exp*_tmp435=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp435;
void*_tmp436=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_t=_tmp436;
struct Cyc_Absyn_Exp*_tmp437=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp437;
if(t == new_t && req == new_req)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp438=_cycalloc(sizeof(*_tmp438));_tmp438->name=f->name,_tmp438->tq=f->tq,_tmp438->type=new_t,_tmp438->width=f->width,_tmp438->attributes=f->attributes,_tmp438->requires_clause=new_req;_tmp438;});}}
# 2578
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2581
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp439=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp439;
struct Cyc_List_List*_tmp43A=fs->tl;struct Cyc_List_List*t=_tmp43A;
struct Cyc_Absyn_Aggrfield*_tmp43B=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp43B;
struct Cyc_List_List*_tmp43C=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_t=_tmp43C;
if(new_f == f && new_t == t)return fs;
return({struct Cyc_List_List*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->hd=new_f,_tmp43D->tl=new_t;_tmp43D;});}}
# 2590
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2593
struct _tuple0 _tmp43E=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp67=_tmp43E;struct _tuple0 _tmp43F=_stmttmp67;struct Cyc_List_List*_tmp441;struct Cyc_List_List*_tmp440;_LL1: _tmp440=_tmp43F.f1;_tmp441=_tmp43F.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp440;struct Cyc_List_List*rpo1b=_tmp441;
struct Cyc_List_List*_tmp442=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp442;
struct Cyc_List_List*_tmp443=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp443;
if(rpo2a == rpo1a && rpo2b == rpo1b)
return rgn_po;else{
# 2599
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}}
# 2602
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2605
void*_tmp444=Cyc_Tcutil_compress(t);void*_stmttmp68=_tmp444;void*_tmp445=_stmttmp68;struct Cyc_Absyn_Exp*_tmp446;struct Cyc_Absyn_Exp*_tmp447;struct Cyc_List_List*_tmp449;void*_tmp448;void*_tmp44A;void*_tmp44B;struct Cyc_List_List*_tmp44D;enum Cyc_Absyn_AggrKind _tmp44C;struct Cyc_List_List*_tmp44E;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_Absyn_Exp*_tmp458;struct Cyc_List_List*_tmp457;struct Cyc_List_List*_tmp456;struct Cyc_Absyn_VarargInfo*_tmp455;int _tmp454;struct Cyc_List_List*_tmp453;void*_tmp452;struct Cyc_Absyn_Tqual _tmp451;void*_tmp450;struct Cyc_List_List*_tmp44F;void*_tmp45F;void*_tmp45E;void*_tmp45D;void*_tmp45C;struct Cyc_Absyn_Tqual _tmp45B;void*_tmp45A;unsigned _tmp464;void*_tmp463;struct Cyc_Absyn_Exp*_tmp462;struct Cyc_Absyn_Tqual _tmp461;void*_tmp460;void*_tmp468;struct Cyc_Absyn_Typedefdecl*_tmp467;struct Cyc_List_List*_tmp466;struct _tuple1*_tmp465;struct Cyc_Absyn_Tvar*_tmp469;switch(*((int*)_tmp445)){case 2U: _LL1: _tmp469=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp445)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp469;
# 2608
struct _handler_cons _tmp46A;_push_handler(& _tmp46A);{int _tmp46C=0;if(setjmp(_tmp46A.handler))_tmp46C=1;if(!_tmp46C){{void*_tmp46D=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp46D;};_pop_handler();}else{void*_tmp46B=(void*)Cyc_Core_get_exn_thrown();void*_tmp46E=_tmp46B;void*_tmp46F;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp46E)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp46F=_tmp46E;_LL1F: {void*exn=_tmp46F;(int)_rethrow(exn);}}_LL1B:;}}}case 8U: _LL3: _tmp465=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp445)->f1;_tmp466=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp445)->f2;_tmp467=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp445)->f3;_tmp468=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp445)->f4;_LL4: {struct _tuple1*n=_tmp465;struct Cyc_List_List*ts=_tmp466;struct Cyc_Absyn_Typedefdecl*td=_tmp467;void*topt=_tmp468;
# 2611
struct Cyc_List_List*_tmp470=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp470;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp471=_cycalloc(sizeof(*_tmp471));_tmp471->tag=8U,_tmp471->f1=n,_tmp471->f2=new_ts,_tmp471->f3=td,_tmp471->f4=topt;_tmp471;});}case 4U: _LL5: _tmp460=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).elt_type;_tmp461=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).tq;_tmp462=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).num_elts;_tmp463=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).zero_term;_tmp464=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp445)->f1).zt_loc;_LL6: {void*t1=_tmp460;struct Cyc_Absyn_Tqual tq=_tmp461;struct Cyc_Absyn_Exp*e=_tmp462;void*zt=_tmp463;unsigned ztl=_tmp464;
# 2614
void*_tmp472=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp472;
struct Cyc_Absyn_Exp*_tmp473=e == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp473;
void*_tmp474=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp474;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp475=_cycalloc(sizeof(*_tmp475));
_tmp475->tag=4U,(_tmp475->f1).elt_type=new_t1,(_tmp475->f1).tq=tq,(_tmp475->f1).num_elts=new_e,(_tmp475->f1).zero_term=new_zt,(_tmp475->f1).zt_loc=ztl;_tmp475;});}case 3U: _LL7: _tmp45A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).elt_type;_tmp45B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).elt_tq;_tmp45C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).rgn;_tmp45D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).nullable;_tmp45E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).bounds;_tmp45F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp445)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp45A;struct Cyc_Absyn_Tqual tq=_tmp45B;void*r=_tmp45C;void*n=_tmp45D;void*b=_tmp45E;void*zt=_tmp45F;
# 2620
void*_tmp476=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp476;
void*_tmp477=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp477;
void*_tmp478=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp478;
void*_tmp479=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp479;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp62D;_tmp62D.elt_type=new_t1,_tmp62D.elt_tq=tq,(_tmp62D.ptr_atts).rgn=new_r,(_tmp62D.ptr_atts).nullable=n,(_tmp62D.ptr_atts).bounds=new_b,(_tmp62D.ptr_atts).zero_term=new_zt,(_tmp62D.ptr_atts).ptrloc=0;_tmp62D;}));}case 5U: _LL9: _tmp44F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).tvars;_tmp450=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).effect;_tmp451=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).ret_tqual;_tmp452=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).ret_type;_tmp453=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).args;_tmp454=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).c_varargs;_tmp455=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).cyc_varargs;_tmp456=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).rgn_po;_tmp457=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).attributes;_tmp458=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).requires_clause;_tmp459=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp445)->f1).ensures_clause;_LLA: {struct Cyc_List_List*vs=_tmp44F;void*eff=_tmp450;struct Cyc_Absyn_Tqual rtq=_tmp451;void*rtyp=_tmp452;struct Cyc_List_List*args=_tmp453;int c_varargs=_tmp454;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp455;struct Cyc_List_List*rgn_po=_tmp456;struct Cyc_List_List*atts=_tmp457;struct Cyc_Absyn_Exp*req=_tmp458;struct Cyc_Absyn_Exp*ens=_tmp459;
# 2630
{struct Cyc_List_List*_tmp47A=vs;struct Cyc_List_List*p=_tmp47A;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp47C=_region_malloc(rgn,sizeof(*_tmp47C));({struct _tuple19*_tmp74C=({struct _tuple19*_tmp47B=_region_malloc(rgn,sizeof(*_tmp47B));_tmp47B->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp74B=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp47B->f2=_tmp74B;});_tmp47B;});_tmp47C->hd=_tmp74C;}),_tmp47C->tl=inst;_tmp47C;});}}{
struct _tuple0 _tmp47D=({struct _RegionHandle*_tmp74E=rgn;struct _RegionHandle*_tmp74D=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp74E,_tmp74D,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2632
struct _tuple0 _stmttmp69=_tmp47D;struct _tuple0 _tmp47E=_stmttmp69;struct Cyc_List_List*_tmp480;struct Cyc_List_List*_tmp47F;_LL21: _tmp47F=_tmp47E.f1;_tmp480=_tmp47E.f2;_LL22: {struct Cyc_List_List*qs=_tmp47F;struct Cyc_List_List*ts=_tmp480;
# 2634
struct Cyc_List_List*_tmp481=args;struct Cyc_List_List*args2=_tmp481;
struct Cyc_List_List*_tmp482=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp482;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2;
if(eff == 0)
eff2=0;else{
# 2642
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,eff);
if(new_eff == eff)
eff2=eff;else{
# 2646
eff2=new_eff;}}{
# 2648
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2652
struct Cyc_Absyn_VarargInfo _tmp483=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp6A=_tmp483;struct Cyc_Absyn_VarargInfo _tmp484=_stmttmp6A;int _tmp488;void*_tmp487;struct Cyc_Absyn_Tqual _tmp486;struct _fat_ptr*_tmp485;_LL24: _tmp485=_tmp484.name;_tmp486=_tmp484.tq;_tmp487=_tmp484.type;_tmp488=_tmp484.inject;_LL25: {struct _fat_ptr*n=_tmp485;struct Cyc_Absyn_Tqual tq=_tmp486;void*t=_tmp487;int i=_tmp488;
void*_tmp489=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp489;
if(t2 == t)cyc_varargs2=cyc_varargs;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp48A=_cycalloc(sizeof(*_tmp48A));_tmp48A->name=n,_tmp48A->tq=tq,_tmp48A->type=t2,_tmp48A->inject=i;_tmp48A;});}}}{
# 2657
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp48B=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp48B;
struct Cyc_List_List*_tmp48C=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp48C;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp48D=_cycalloc(sizeof(*_tmp48D));_tmp48D->tag=5U,(_tmp48D->f1).tvars=vs,(_tmp48D->f1).effect=eff2,(_tmp48D->f1).ret_tqual=rtq,({void*_tmp74F=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp48D->f1).ret_type=_tmp74F;}),(_tmp48D->f1).args=args2,(_tmp48D->f1).c_varargs=c_varargs,(_tmp48D->f1).cyc_varargs=cyc_varargs2,(_tmp48D->f1).rgn_po=rgn_po2,(_tmp48D->f1).attributes=atts,(_tmp48D->f1).requires_clause=req2,(_tmp48D->f1).requires_relns=req_relns2,(_tmp48D->f1).ensures_clause=ens2,(_tmp48D->f1).ensures_relns=ens_relns2;_tmp48D;});}}}}}}case 6U: _LLB: _tmp44E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp445)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp44E;
# 2666
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp48E=tqts;struct Cyc_List_List*ts1=_tmp48E;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp48F=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp48F;
void*_tmp490=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp490;
if(t1 != t2)
change=1;
# 2674
ts2=({struct Cyc_List_List*_tmp491=_region_malloc(rgn,sizeof(*_tmp491));_tmp491->hd=t2,_tmp491->tl=ts2;_tmp491;});}}
# 2676
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp492=_cycalloc(sizeof(*_tmp492));_tmp492->tag=6U,({struct Cyc_List_List*_tmp751=({struct Cyc_List_List*_tmp750=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp750,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp492->f1=_tmp751;});_tmp492;});}case 7U: _LLD: _tmp44C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp445)->f1;_tmp44D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp445)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp44C;struct Cyc_List_List*fs=_tmp44D;
# 2680
struct Cyc_List_List*_tmp493=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp493;
if(fs == new_fs)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->tag=7U,_tmp494->f1=k,_tmp494->f2=new_fs;_tmp494;});}case 1U: _LLF: _tmp44B=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp445)->f2;_LL10: {void*r=_tmp44B;
# 2684
if(r != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,r);else{
return t;}}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f2 == 0){_LL11: _tmp44A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f1;_LL12: {void*c=_tmp44A;
return t;}}else{_LL13: _tmp448=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f1;_tmp449=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp445)->f2;_LL14: {void*c=_tmp448;struct Cyc_List_List*ts=_tmp449;
# 2688
struct Cyc_List_List*_tmp495=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp495;
if(ts == new_ts)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp496=_cycalloc(sizeof(*_tmp496));_tmp496->tag=0U,_tmp496->f1=c,_tmp496->f2=new_ts;_tmp496;});}}}case 9U: _LL15: _tmp447=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp445)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp447;
# 2692
struct Cyc_Absyn_Exp*_tmp497=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp497;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp498=_cycalloc(sizeof(*_tmp498));_tmp498->tag=9U,_tmp498->f1=new_e;_tmp498;});}case 11U: _LL17: _tmp446=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp445)->f1;_LL18: {struct Cyc_Absyn_Exp*e=_tmp446;
# 2695
struct Cyc_Absyn_Exp*_tmp499=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp499;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp49A=_cycalloc(sizeof(*_tmp49A));_tmp49A->tag=11U,_tmp49A->f1=new_e;_tmp49A;});}default: _LL19: _LL1A:
({void*_tmp49B=0U;({struct _fat_ptr _tmp752=({const char*_tmp49C="found typedecltype in rsubs";_tag_fat(_tmp49C,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp752,_tag_fat(_tmp49B,sizeof(void*),0U));});});}_LL0:;}
# 2701
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2704
if(ts == 0)
return 0;{
void*_tmp49D=(void*)ts->hd;void*old_hd=_tmp49D;
struct Cyc_List_List*_tmp49E=ts->tl;struct Cyc_List_List*old_tl=_tmp49E;
void*_tmp49F=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp49F;
struct Cyc_List_List*_tmp4A0=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp4A0;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp4A1=_cycalloc(sizeof(*_tmp4A1));_tmp4A1->hd=new_hd,_tmp4A1->tl=new_tl;_tmp4A1;});}}
# 2715
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 2722
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4A2=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4A2;
return({struct _tuple19*_tmp4A4=_cycalloc(sizeof(*_tmp4A4));_tmp4A4->f1=tv,({void*_tmp754=({struct Cyc_Core_Opt*_tmp753=k;Cyc_Absyn_new_evar(_tmp753,({struct Cyc_Core_Opt*_tmp4A3=_cycalloc(sizeof(*_tmp4A3));_tmp4A3->v=s;_tmp4A3;}));});_tmp4A4->f2=_tmp754;});_tmp4A4;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2727
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2729
struct _tuple28*_tmp4A5=env;struct _RegionHandle*_tmp4A7;struct Cyc_List_List*_tmp4A6;_LL1: _tmp4A6=_tmp4A5->f1;_tmp4A7=_tmp4A5->f2;_LL2: {struct Cyc_List_List*s=_tmp4A6;struct _RegionHandle*rgn=_tmp4A7;
struct Cyc_Core_Opt*_tmp4A8=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4A8;
return({struct _tuple19*_tmp4AA=_region_malloc(rgn,sizeof(*_tmp4AA));_tmp4AA->f1=tv,({void*_tmp756=({struct Cyc_Core_Opt*_tmp755=k;Cyc_Absyn_new_evar(_tmp755,({struct Cyc_Core_Opt*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->v=s;_tmp4A9;}));});_tmp4AA->f2=_tmp756;});_tmp4AA;});}}
# 2734
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2736
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp4AD=({struct Cyc_String_pa_PrintArg_struct _tmp62E;_tmp62E.tag=0U,_tmp62E.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp62E;});void*_tmp4AB[1U];_tmp4AB[0]=& _tmp4AD;({unsigned _tmp758=loc;struct _fat_ptr _tmp757=({const char*_tmp4AC="bitfield %s does not have constant width";_tag_fat(_tmp4AC,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp758,_tmp757,_tag_fat(_tmp4AB,sizeof(void*),1U));});});else{
# 2741
struct _tuple13 _tmp4AE=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp6B=_tmp4AE;struct _tuple13 _tmp4AF=_stmttmp6B;int _tmp4B1;unsigned _tmp4B0;_LL1: _tmp4B0=_tmp4AF.f1;_tmp4B1=_tmp4AF.f2;_LL2: {unsigned i=_tmp4B0;int known=_tmp4B1;
if(!known)
({void*_tmp4B2=0U;({unsigned _tmp75A=loc;struct _fat_ptr _tmp759=({const char*_tmp4B3="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4B3,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp75A,_tmp759,_tag_fat(_tmp4B2,sizeof(void*),0U));});});
if((int)i < 0)
({void*_tmp4B4=0U;({unsigned _tmp75C=loc;struct _fat_ptr _tmp75B=({const char*_tmp4B5="bitfield has negative width";_tag_fat(_tmp4B5,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp75C,_tmp75B,_tag_fat(_tmp4B4,sizeof(void*),0U));});});
w=i;}}{
# 2748
void*_tmp4B6=Cyc_Tcutil_compress(field_type);void*_stmttmp6C=_tmp4B6;void*_tmp4B7=_stmttmp6C;enum Cyc_Absyn_Size_of _tmp4B8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B7)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B7)->f1)->tag == 1U){_LL4: _tmp4B8=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4B7)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4B8;
# 2751
{enum Cyc_Absyn_Size_of _tmp4B9=b;switch(_tmp4B9){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp4BA=0U;({unsigned _tmp75E=loc;struct _fat_ptr _tmp75D=({const char*_tmp4BB="bitfield larger than type";_tag_fat(_tmp4BB,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp75E,_tmp75D,_tag_fat(_tmp4BA,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp4BC=0U;({unsigned _tmp760=loc;struct _fat_ptr _tmp75F=({const char*_tmp4BD="bitfield larger than type";_tag_fat(_tmp4BD,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp760,_tmp75F,_tag_fat(_tmp4BC,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp4BE=0U;({unsigned _tmp762=loc;struct _fat_ptr _tmp761=({const char*_tmp4BF="bitfield larger than type";_tag_fat(_tmp4BF,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp762,_tmp761,_tag_fat(_tmp4BE,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp4C0=0U;({unsigned _tmp764=loc;struct _fat_ptr _tmp763=({const char*_tmp4C1="bitfield larger than type";_tag_fat(_tmp4C1,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp764,_tmp763,_tag_fat(_tmp4C0,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 2759
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2761
({struct Cyc_String_pa_PrintArg_struct _tmp4C4=({struct Cyc_String_pa_PrintArg_struct _tmp630;_tmp630.tag=0U,_tmp630.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp630;});struct Cyc_String_pa_PrintArg_struct _tmp4C5=({struct Cyc_String_pa_PrintArg_struct _tmp62F;_tmp62F.tag=0U,({
struct _fat_ptr _tmp765=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field_type));_tmp62F.f1=_tmp765;});_tmp62F;});void*_tmp4C2[2U];_tmp4C2[0]=& _tmp4C4,_tmp4C2[1]=& _tmp4C5;({unsigned _tmp767=loc;struct _fat_ptr _tmp766=({const char*_tmp4C3="bitfield %s must have integral type but has type %s";_tag_fat(_tmp4C3,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp767,_tmp766,_tag_fat(_tmp4C2,sizeof(void*),2U));});});
goto _LL3;}_LL3:;}}}
# 2770
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4C6=t;void*_tmp4C8;struct Cyc_Absyn_Typedefdecl*_tmp4C7;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C6)->tag == 8U){_LL1: _tmp4C7=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C6)->f3;_tmp4C8=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4C6)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4C7;void*tdopt=_tmp4C8;
# 2773
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({void*_tmp4C9=0U;({unsigned _tmp769=loc;struct _fat_ptr _tmp768=({const char*_tmp4CA="extra const";_tag_fat(_tmp4CA,sizeof(char),12U);});Cyc_Tcutil_warn(_tmp769,_tmp768,_tag_fat(_tmp4C9,sizeof(void*),0U));});});
return 1;}
# 2778
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);else{
return declared_const;}}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2785
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp76A=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp76A;});}
# 2789
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2795
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2798
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4CD=({struct Cyc_String_pa_PrintArg_struct _tmp632;_tmp632.tag=0U,_tmp632.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp632;});struct Cyc_String_pa_PrintArg_struct _tmp4CE=({struct Cyc_String_pa_PrintArg_struct _tmp631;_tmp631.tag=0U,({struct _fat_ptr _tmp76B=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp631.f1=_tmp76B;});_tmp631;});void*_tmp4CB[2U];_tmp4CB[0]=& _tmp4CD,_tmp4CB[1]=& _tmp4CE;({unsigned _tmp76D=loc;struct _fat_ptr _tmp76C=({const char*_tmp4CC="%s: %s";_tag_fat(_tmp4CC,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp76D,_tmp76C,_tag_fat(_tmp4CB,sizeof(void*),2U));});});}}}
# 2804
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2808
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2812
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp76F=tvs;unsigned _tmp76E=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp76F,_tmp76E,Cyc_Absynpp_tvar2string,({const char*_tmp4CF="duplicate type variable";_tag_fat(_tmp4CF,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2826 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2831
struct _RegionHandle _tmp4D0=_new_region("temp");struct _RegionHandle*temp=& _tmp4D0;_push_region(temp);
# 2835
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp770=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp770,({const char*_tmp4D1="";_tag_fat(_tmp4D1,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4D3=_region_malloc(temp,sizeof(*_tmp4D3));({struct _tuple29*_tmp771=({struct _tuple29*_tmp4D2=_region_malloc(temp,sizeof(*_tmp4D2));_tmp4D2->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4D2->f2=0;_tmp4D2;});_tmp4D3->hd=_tmp771;}),_tmp4D3->tl=fields;_tmp4D3;});}}
# 2840
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2842
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp4F8="struct";_tag_fat(_tmp4F8,sizeof(char),7U);}):({const char*_tmp4F9="union";_tag_fat(_tmp4F9,sizeof(char),6U);});
# 2845
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4D4=(struct _tuple30*)des->hd;struct _tuple30*_stmttmp6D=_tmp4D4;struct _tuple30*_tmp4D5=_stmttmp6D;void*_tmp4D7;struct Cyc_List_List*_tmp4D6;_LL1: _tmp4D6=_tmp4D5->f1;_tmp4D7=_tmp4D5->f2;_LL2: {struct Cyc_List_List*dl=_tmp4D6;void*a=_tmp4D7;
if(dl == 0){
# 2850
struct Cyc_List_List*_tmp4D8=fields;struct Cyc_List_List*fields2=_tmp4D8;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple29*)fields2->hd)).f2){
(*((struct _tuple29*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp773=({struct Cyc_List_List*_tmp4DA=_cycalloc(sizeof(*_tmp4DA));({void*_tmp772=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4D9=_cycalloc(sizeof(*_tmp4D9));_tmp4D9->tag=1U,_tmp4D9->f1=((*((struct _tuple29*)fields2->hd)).f1)->name;_tmp4D9;});_tmp4DA->hd=_tmp772;}),_tmp4DA->tl=0;_tmp4DA;});(*((struct _tuple30*)des->hd)).f1=_tmp773;});
ans=({struct Cyc_List_List*_tmp4DC=_region_malloc(rgn,sizeof(*_tmp4DC));({struct _tuple31*_tmp774=({struct _tuple31*_tmp4DB=_region_malloc(rgn,sizeof(*_tmp4DB));_tmp4DB->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4DB->f2=a;_tmp4DB;});_tmp4DC->hd=_tmp774;}),_tmp4DC->tl=ans;_tmp4DC;});
break;}}
# 2858
if(fields2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4DF=({struct Cyc_String_pa_PrintArg_struct _tmp633;_tmp633.tag=0U,_tmp633.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp633;});void*_tmp4DD[1U];_tmp4DD[0]=& _tmp4DF;({unsigned _tmp776=loc;struct _fat_ptr _tmp775=({const char*_tmp4DE="too many arguments to %s";_tag_fat(_tmp4DE,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp776,_tmp775,_tag_fat(_tmp4DD,sizeof(void*),1U));});});}else{
if(dl->tl != 0)
# 2862
({void*_tmp4E0=0U;({unsigned _tmp778=loc;struct _fat_ptr _tmp777=({const char*_tmp4E1="multiple designators are not yet supported";_tag_fat(_tmp4E1,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp778,_tmp777,_tag_fat(_tmp4E0,sizeof(void*),0U));});});else{
# 2865
void*_tmp4E2=(void*)dl->hd;void*_stmttmp6E=_tmp4E2;void*_tmp4E3=_stmttmp6E;struct _fat_ptr*_tmp4E4;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp4E3)->tag == 0U){_LL4: _LL5:
# 2867
({struct Cyc_String_pa_PrintArg_struct _tmp4E7=({struct Cyc_String_pa_PrintArg_struct _tmp634;_tmp634.tag=0U,_tmp634.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp634;});void*_tmp4E5[1U];_tmp4E5[0]=& _tmp4E7;({unsigned _tmp77A=loc;struct _fat_ptr _tmp779=({const char*_tmp4E6="array designator used in argument to %s";_tag_fat(_tmp4E6,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp77A,_tmp779,_tag_fat(_tmp4E5,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp4E4=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp4E3)->f1;_LL7: {struct _fat_ptr*v=_tmp4E4;
# 2870
struct Cyc_List_List*_tmp4E8=fields;struct Cyc_List_List*fields2=_tmp4E8;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple29*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple29*)fields2->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp4EB=({struct Cyc_String_pa_PrintArg_struct _tmp635;_tmp635.tag=0U,_tmp635.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp635;});void*_tmp4E9[1U];_tmp4E9[0]=& _tmp4EB;({unsigned _tmp77C=loc;struct _fat_ptr _tmp77B=({const char*_tmp4EA="member %s has already been used as an argument";_tag_fat(_tmp4EA,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp77C,_tmp77B,_tag_fat(_tmp4E9,sizeof(void*),1U));});});
(*((struct _tuple29*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp4ED=_region_malloc(rgn,sizeof(*_tmp4ED));({struct _tuple31*_tmp77D=({struct _tuple31*_tmp4EC=_region_malloc(rgn,sizeof(*_tmp4EC));_tmp4EC->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4EC->f2=a;_tmp4EC;});_tmp4ED->hd=_tmp77D;}),_tmp4ED->tl=ans;_tmp4ED;});
break;}}
# 2879
if(fields2 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4F0=({struct Cyc_String_pa_PrintArg_struct _tmp636;_tmp636.tag=0U,_tmp636.f1=(struct _fat_ptr)((struct _fat_ptr)*v);_tmp636;});void*_tmp4EE[1U];_tmp4EE[0]=& _tmp4F0;({unsigned _tmp77F=loc;struct _fat_ptr _tmp77E=({const char*_tmp4EF="bad field designator %s";_tag_fat(_tmp4EF,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp77F,_tmp77E,_tag_fat(_tmp4EE,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}}
# 2884
if((int)aggr_kind == (int)0U)
# 2886
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple29*)fields->hd)).f2){
({void*_tmp4F1=0U;({unsigned _tmp781=loc;struct _fat_ptr _tmp780=({const char*_tmp4F2="too few arguments to struct";_tag_fat(_tmp4F2,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp781,_tmp780,_tag_fat(_tmp4F1,sizeof(void*),0U));});});
break;}}else{
# 2893
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple29*)fields->hd)).f2){
if(found)({void*_tmp4F3=0U;({unsigned _tmp783=loc;struct _fat_ptr _tmp782=({const char*_tmp4F4="only one member of a union is allowed";_tag_fat(_tmp4F4,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp783,_tmp782,_tag_fat(_tmp4F3,sizeof(void*),0U));});});
found=1;}}
# 2899
if(!found)({void*_tmp4F5=0U;({unsigned _tmp785=loc;struct _fat_ptr _tmp784=({const char*_tmp4F6="missing member for union";_tag_fat(_tmp4F6,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp785,_tmp784,_tag_fat(_tmp4F5,sizeof(void*),0U));});});}{
# 2902
struct Cyc_List_List*_tmp4F7=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp4F7;}}}
# 2835
;_pop_region();}
# 2908
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2910
void*_tmp4FA=e1->r;void*_stmttmp6F=_tmp4FA;void*_tmp4FB=_stmttmp6F;struct Cyc_Absyn_Exp*_tmp4FC;struct Cyc_Absyn_Exp*_tmp4FD;struct Cyc_Absyn_Exp*_tmp4FE;struct Cyc_Absyn_Exp*_tmp4FF;struct Cyc_Absyn_Exp*_tmp500;struct Cyc_Absyn_Exp*_tmp501;switch(*((int*)_tmp4FB)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp504=({struct Cyc_String_pa_PrintArg_struct _tmp637;_tmp637.tag=0U,({struct _fat_ptr _tmp786=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp637.f1=_tmp786;});_tmp637;});void*_tmp502[1U];_tmp502[0]=& _tmp504;({struct _fat_ptr _tmp787=({const char*_tmp503="we have a cast in a lhs:  %s";_tag_fat(_tmp503,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp787,_tag_fat(_tmp502,sizeof(void*),1U));});});case 20U: _LL3: _tmp501=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp501;
_tmp500=e1a;goto _LL6;}case 23U: _LL5: _tmp500=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp500;
# 2914
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp4FF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp4FF;
_tmp4FE=e1a;goto _LLA;}case 21U: _LL9: _tmp4FE=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp4FE;
# 2918
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp507=({struct Cyc_String_pa_PrintArg_struct _tmp638;_tmp638.tag=0U,({
struct _fat_ptr _tmp788=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp638.f1=_tmp788;});_tmp638;});void*_tmp505[1U];_tmp505[0]=& _tmp507;({struct _fat_ptr _tmp789=({const char*_tmp506="found zero pointer aggregate member assignment: %s";_tag_fat(_tmp506,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp789,_tag_fat(_tmp505,sizeof(void*),1U));});});
return 0;}case 13U: _LLB: _tmp4FD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp4FD;
_tmp4FC=e1a;goto _LLE;}case 12U: _LLD: _tmp4FC=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp4FB)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp4FC;
# 2924
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp50A=({struct Cyc_String_pa_PrintArg_struct _tmp639;_tmp639.tag=0U,({
struct _fat_ptr _tmp78A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp639.f1=_tmp78A;});_tmp639;});void*_tmp508[1U];_tmp508[0]=& _tmp50A;({struct _fat_ptr _tmp78B=({const char*_tmp509="found zero pointer instantiate/noinstantiate: %s";_tag_fat(_tmp509,sizeof(char),49U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78B,_tag_fat(_tmp508,sizeof(void*),1U));});});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp50D=({struct Cyc_String_pa_PrintArg_struct _tmp63A;_tmp63A.tag=0U,({struct _fat_ptr _tmp78C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp63A.f1=_tmp78C;});_tmp63A;});void*_tmp50B[1U];_tmp50B[0]=& _tmp50D;({struct _fat_ptr _tmp78D=({const char*_tmp50C="found bad lhs in is_zero_ptr_deref: %s";_tag_fat(_tmp50C,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp78D,_tag_fat(_tmp50B,sizeof(void*),1U));});});}_LL0:;}
# 2939
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 2942
void*_tmp50E=Cyc_Tcutil_compress(r);void*_stmttmp70=_tmp50E;void*_tmp50F=_stmttmp70;struct Cyc_Absyn_Tvar*_tmp510;enum Cyc_Absyn_AliasQual _tmp512;enum Cyc_Absyn_KindQual _tmp511;switch(*((int*)_tmp50F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp50F)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f4 == 0){_LL5: _tmp511=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f3)->kind)->v)->kind;_tmp512=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp50F)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp511;enum Cyc_Absyn_AliasQual a=_tmp512;
# 2946
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp510=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp50F)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp510;
# 2950
struct Cyc_Absyn_Kind*_tmp513=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp71=_tmp513;struct Cyc_Absyn_Kind*_tmp514=_stmttmp71;enum Cyc_Absyn_AliasQual _tmp516;enum Cyc_Absyn_KindQual _tmp515;_LLC: _tmp515=_tmp514->kind;_tmp516=_tmp514->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp515;enum Cyc_Absyn_AliasQual a=_tmp516;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp517=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp72=_tmp517;void*_tmp518=_stmttmp72;struct Cyc_Core_Opt**_tmp519;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp518)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp518)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp518)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp519=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp518)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp519;
# 2954
({struct Cyc_Core_Opt*_tmp78F=({struct Cyc_Core_Opt*_tmp51B=_cycalloc(sizeof(*_tmp51B));({void*_tmp78E=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp51A=_cycalloc(sizeof(*_tmp51A));_tmp51A->tag=2U,_tmp51A->f1=0,_tmp51A->f2=& Cyc_Tcutil_rk;_tmp51A;});_tmp51B->v=_tmp78E;});_tmp51B;});*x=_tmp78F;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2959
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2966
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp51C=Cyc_Tcutil_compress(t);void*_stmttmp73=_tmp51C;void*_tmp51D=_stmttmp73;struct Cyc_Absyn_Tvar*_tmp51E;void*_tmp51F;switch(*((int*)_tmp51D)){case 3U: _LL1: _tmp51F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp51D)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp51F;
# 2969
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp51E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp51D)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp51E;
# 2971
struct Cyc_Absyn_Kind*_tmp520=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp74=_tmp520;struct Cyc_Absyn_Kind*_tmp521=_stmttmp74;enum Cyc_Absyn_AliasQual _tmp523;enum Cyc_Absyn_KindQual _tmp522;_LL8: _tmp522=_tmp521->kind;_tmp523=_tmp521->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp522;enum Cyc_Absyn_AliasQual a=_tmp523;
enum Cyc_Absyn_KindQual _tmp524=k;switch(_tmp524){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp525=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp75=_tmp525;void*_tmp526=_stmttmp75;enum Cyc_Absyn_KindQual _tmp528;struct Cyc_Core_Opt**_tmp527;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp526)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp526)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp527=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp526)->f1;_tmp528=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp526)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp527;enum Cyc_Absyn_KindQual k=_tmp528;
# 2977
({struct Cyc_Core_Opt*_tmp792=({struct Cyc_Core_Opt*_tmp52B=_cycalloc(sizeof(*_tmp52B));({void*_tmp791=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->tag=2U,_tmp52A->f1=0,({struct Cyc_Absyn_Kind*_tmp790=({struct Cyc_Absyn_Kind*_tmp529=_cycalloc(sizeof(*_tmp529));_tmp529->kind=k,_tmp529->aliasqual=Cyc_Absyn_Aliasable;_tmp529;});_tmp52A->f2=_tmp790;});_tmp52A;});_tmp52B->v=_tmp791;});_tmp52B;});*x=_tmp792;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
# 2981
 return 1;}_LL13:;}
# 2984
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2987
 return 0;}_LL0:;}
# 2990
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp52C=Cyc_Tcutil_compress(t);void*t=_tmp52C;
if(Cyc_Tcutil_is_noalias_pointer(t,0))return 1;{
void*_tmp52D=t;struct Cyc_List_List*_tmp52E;struct Cyc_List_List*_tmp530;union Cyc_Absyn_DatatypeFieldInfo _tmp52F;struct Cyc_List_List*_tmp532;union Cyc_Absyn_DatatypeInfo _tmp531;struct Cyc_List_List*_tmp534;struct Cyc_Absyn_Aggrdecl**_tmp533;struct Cyc_List_List*_tmp535;switch(*((int*)_tmp52D)){case 6U: _LL1: _tmp535=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp52D)->f1;_LL2: {struct Cyc_List_List*qts=_tmp535;
# 2995
while(qts != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))return 1;
qts=qts->tl;}
# 2999
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp533=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)->f1).KnownAggr).val;_tmp534=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp533;struct Cyc_List_List*ts=_tmp534;
# 3001
if((*adp)->impl == 0)return 0;else{
# 3003
struct Cyc_List_List*_tmp536=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp536;
struct Cyc_List_List*_tmp537=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp537;
void*t;
while(x != 0){
t=inst == 0?((struct Cyc_Absyn_Aggrfield*)x->hd)->type: Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
x=x->tl;}
# 3011
return 0;}}}else{_LL7: _LL8:
# 3021
 return 0;}case 18U: _LL9: _tmp531=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)->f1;_tmp532=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f2;_LLA: {union Cyc_Absyn_DatatypeInfo tinfo=_tmp531;struct Cyc_List_List*ts=_tmp532;
# 3023
union Cyc_Absyn_DatatypeInfo _tmp538=tinfo;struct Cyc_Core_Opt*_tmp53A;struct Cyc_List_List*_tmp539;int _tmp53C;struct _tuple1*_tmp53B;if((_tmp538.UnknownDatatype).tag == 1){_LL10: _tmp53B=((_tmp538.UnknownDatatype).val).name;_tmp53C=((_tmp538.UnknownDatatype).val).is_extensible;_LL11: {struct _tuple1*nm=_tmp53B;int isxt=_tmp53C;
# 3026
return 0;}}else{_LL12: _tmp539=(*(_tmp538.KnownDatatype).val)->tvs;_tmp53A=(*(_tmp538.KnownDatatype).val)->fields;_LL13: {struct Cyc_List_List*tvs=_tmp539;struct Cyc_Core_Opt*flds=_tmp53A;
# 3029
return 0;}}_LLF:;}case 19U: _LLB: _tmp52F=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f1)->f1;_tmp530=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp52D)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp52F;struct Cyc_List_List*ts=_tmp530;
# 3032
union Cyc_Absyn_DatatypeFieldInfo _tmp53D=tinfo;struct Cyc_Absyn_Datatypefield*_tmp53F;struct Cyc_Absyn_Datatypedecl*_tmp53E;if((_tmp53D.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 3035
 return 0;}else{_LL17: _tmp53E=((_tmp53D.KnownDatatypefield).val).f1;_tmp53F=((_tmp53D.KnownDatatypefield).val).f2;_LL18: {struct Cyc_Absyn_Datatypedecl*td=_tmp53E;struct Cyc_Absyn_Datatypefield*fld=_tmp53F;
# 3037
struct Cyc_List_List*_tmp540=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp540;
struct Cyc_List_List*_tmp541=fld->typs;struct Cyc_List_List*typs=_tmp541;
while(typs != 0){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
typs=typs->tl;}
# 3044
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp52E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp52D)->f2;_LL6: {struct Cyc_List_List*x=_tmp52E;
# 3014
while(x != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))return 1;
x=x->tl;}
# 3018
return 0;}default: _LLD: _LLE:
# 3046
 return 0;}_LL0:;}}
# 3053
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp542=e->r;void*_stmttmp76=_tmp542;void*_tmp543=_stmttmp76;struct Cyc_Absyn_Stmt*_tmp544;struct Cyc_Absyn_Exp*_tmp545;struct Cyc_Absyn_Exp*_tmp546;struct Cyc_Absyn_Exp*_tmp548;struct Cyc_Absyn_Exp*_tmp547;struct Cyc_Absyn_Exp*_tmp54A;struct Cyc_Absyn_Exp*_tmp549;struct _fat_ptr*_tmp54C;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54D;struct Cyc_Absyn_Exp*_tmp54E;switch(*((int*)_tmp543)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp543)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp54E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp54E;
_tmp54D=e1;goto _LL6;}case 20U: _LL5: _tmp54D=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp54D;
# 3058
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp54B=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp54C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp54B;struct _fat_ptr*f=_tmp54C;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp549=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_tmp54A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp549;struct Cyc_Absyn_Exp*e2=_tmp54A;
# 3061
void*_tmp54F=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp77=_tmp54F;void*_tmp550=_stmttmp77;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp550)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp547=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_tmp548=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp543)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp547;struct Cyc_Absyn_Exp*e2=_tmp548;
# 3066
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp546=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp546;
_tmp545=e2;goto _LL10;}case 14U: _LLF: _tmp545=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp543)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp545;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp544=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp543)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp544;
# 3070
while(1){
void*_tmp551=s->r;void*_stmttmp78=_tmp551;void*_tmp552=_stmttmp78;struct Cyc_Absyn_Exp*_tmp553;struct Cyc_Absyn_Stmt*_tmp555;struct Cyc_Absyn_Decl*_tmp554;struct Cyc_Absyn_Stmt*_tmp557;struct Cyc_Absyn_Stmt*_tmp556;switch(*((int*)_tmp552)){case 2U: _LL1B: _tmp556=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp557=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp556;struct Cyc_Absyn_Stmt*s2=_tmp557;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp554=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_tmp555=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp552)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp554;struct Cyc_Absyn_Stmt*s1=_tmp555;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp553=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp552)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp553;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({void*_tmp558=0U;({struct _fat_ptr _tmp793=({const char*_tmp559="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp559,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp793,_tag_fat(_tmp558,sizeof(void*),0U));});});}_LL1A:;}}default: _LL13: _LL14:
# 3078
 return 1;}_LL0:;}
# 3095 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 3097
struct _tuple14 bogus_ans=({struct _tuple14 _tmp647;_tmp647.f1=0,_tmp647.f2=Cyc_Absyn_heap_rgn_type;_tmp647;});
void*_tmp55A=e->r;void*_stmttmp79=_tmp55A;void*_tmp55B=_stmttmp79;struct Cyc_Absyn_Exp*_tmp55D;struct Cyc_Absyn_Exp*_tmp55C;struct Cyc_Absyn_Exp*_tmp55E;int _tmp561;struct _fat_ptr*_tmp560;struct Cyc_Absyn_Exp*_tmp55F;int _tmp564;struct _fat_ptr*_tmp563;struct Cyc_Absyn_Exp*_tmp562;void*_tmp565;switch(*((int*)_tmp55B)){case 1U: _LL1: _tmp565=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_LL2: {void*x=_tmp565;
# 3101
void*_tmp566=x;struct Cyc_Absyn_Vardecl*_tmp567;struct Cyc_Absyn_Vardecl*_tmp568;struct Cyc_Absyn_Vardecl*_tmp569;struct Cyc_Absyn_Vardecl*_tmp56A;switch(*((int*)_tmp566)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp56A=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp566)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp56A;
# 3105
void*_tmp56B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7A=_tmp56B;void*_tmp56C=_stmttmp7A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp56C)->tag == 4U){_LL1B: _LL1C:
# 3107
 return({struct _tuple14 _tmp63B;_tmp63B.f1=1,_tmp63B.f2=Cyc_Absyn_heap_rgn_type;_tmp63B;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp63C;_tmp63C.f1=(vd->tq).real_const,_tmp63C.f2=Cyc_Absyn_heap_rgn_type;_tmp63C;});}_LL1A:;}case 4U: _LL14: _tmp569=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp566)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp569;
# 3111
void*_tmp56D=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7B=_tmp56D;void*_tmp56E=_stmttmp7B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp56E)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp63D;_tmp63D.f1=1,_tmp63D.f2=(void*)_check_null(vd->rgn);_tmp63D;});}else{_LL22: _LL23:
# 3114
 vd->escapes=1;
return({struct _tuple14 _tmp63E;_tmp63E.f1=(vd->tq).real_const,_tmp63E.f2=(void*)_check_null(vd->rgn);_tmp63E;});}_LL1F:;}case 5U: _LL16: _tmp568=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp566)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp568;
# 3117
_tmp567=vd;goto _LL19;}default: _LL18: _tmp567=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp566)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp567;
# 3119
vd->escapes=1;
return({struct _tuple14 _tmp63F;_tmp63F.f1=(vd->tq).real_const,_tmp63F.f2=(void*)_check_null(vd->rgn);_tmp63F;});}}_LLD:;}case 21U: _LL3: _tmp562=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_tmp563=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55B)->f2;_tmp564=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp55B)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp562;struct _fat_ptr*f=_tmp563;int is_tagged=_tmp564;
# 3124
if(is_tagged)return bogus_ans;{
# 3127
void*_tmp56F=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7C=_tmp56F;void*_tmp570=_stmttmp7C;struct Cyc_Absyn_Aggrdecl*_tmp571;struct Cyc_List_List*_tmp572;switch(*((int*)_tmp570)){case 7U: _LL25: _tmp572=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp570)->f2;_LL26: {struct Cyc_List_List*fs=_tmp572;
# 3129
struct Cyc_Absyn_Aggrfield*_tmp573=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp573;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp574=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7D=_tmp574;struct _tuple14 _tmp575=_stmttmp7D;void*_tmp577;int _tmp576;_LL2C: _tmp576=_tmp575.f1;_tmp577=_tmp575.f2;_LL2D: {int c=_tmp576;void*t=_tmp577;
return({struct _tuple14 _tmp640;_tmp640.f1=(finfo->tq).real_const || c,_tmp640.f2=t;_tmp640;});}}
# 3134
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp571=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp570)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp571;
# 3136
struct Cyc_Absyn_Aggrfield*_tmp578=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp578;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp579=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7E=_tmp579;struct _tuple14 _tmp57A=_stmttmp7E;void*_tmp57C;int _tmp57B;_LL2F: _tmp57B=_tmp57A.f1;_tmp57C=_tmp57A.f2;_LL30: {int c=_tmp57B;void*t=_tmp57C;
return({struct _tuple14 _tmp641;_tmp641.f1=(finfo->tq).real_const || c,_tmp641.f2=t;_tmp641;});}}
# 3141
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp55F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_tmp560=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55B)->f2;_tmp561=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp55B)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp55F;struct _fat_ptr*f=_tmp560;int is_tagged=_tmp561;
# 3146
if(is_tagged)return bogus_ans;{
# 3149
void*_tmp57D=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7F=_tmp57D;void*_tmp57E=_stmttmp7F;void*_tmp580;void*_tmp57F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57E)->tag == 3U){_LL32: _tmp57F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57E)->f1).elt_type;_tmp580=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp57E)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp57F;void*r=_tmp580;
# 3151
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp581=Cyc_Tcutil_compress(t1);void*_stmttmp80=_tmp581;void*_tmp582=_stmttmp80;struct Cyc_Absyn_Aggrdecl*_tmp583;struct Cyc_List_List*_tmp584;switch(*((int*)_tmp582)){case 7U: _LL37: _tmp584=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp582)->f2;_LL38: {struct Cyc_List_List*fs=_tmp584;
# 3154
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp582)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp582)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp583=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp582)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp583;
# 3156
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3159
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp642;_tmp642.f1=(finfo->tq).real_const,_tmp642.f2=r;_tmp642;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp55E=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp55E;
# 3166
void*_tmp585=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp81=_tmp585;void*_tmp586=_stmttmp81;void*_tmp588;struct Cyc_Absyn_Tqual _tmp587;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp586)->tag == 3U){_LL3E: _tmp587=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp586)->f1).elt_tq;_tmp588=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp586)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp587;void*r=_tmp588;
# 3168
return({struct _tuple14 _tmp643;_tmp643.f1=tq.real_const,_tmp643.f2=r;_tmp643;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp55C=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55B)->f1;_tmp55D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp55B)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp55C;struct Cyc_Absyn_Exp*e2=_tmp55D;
# 3174
void*t=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*_tmp589=t;struct Cyc_Absyn_Tqual _tmp58A;void*_tmp58C;struct Cyc_Absyn_Tqual _tmp58B;struct Cyc_List_List*_tmp58D;switch(*((int*)_tmp589)){case 6U: _LL43: _tmp58D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp589)->f1;_LL44: {struct Cyc_List_List*ts=_tmp58D;
# 3178
struct _tuple13 _tmp58E=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp82=_tmp58E;struct _tuple13 _tmp58F=_stmttmp82;int _tmp591;unsigned _tmp590;_LL4C: _tmp590=_tmp58F.f1;_tmp591=_tmp58F.f2;_LL4D: {unsigned i=_tmp590;int known=_tmp591;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp592=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp592;
if(finfo != 0)
return({struct _tuple14 _tmp644;_tmp644.f1=((*finfo).f1).real_const,({void*_tmp794=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp644.f2=_tmp794;});_tmp644;});
return bogus_ans;}}}case 3U: _LL45: _tmp58B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).elt_tq;_tmp58C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp589)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp58B;void*r=_tmp58C;
# 3186
return({struct _tuple14 _tmp645;_tmp645.f1=tq.real_const,_tmp645.f2=r;_tmp645;});}case 4U: _LL47: _tmp58A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp589)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp58A;
# 3192
return({struct _tuple14 _tmp646;_tmp646.f1=tq.real_const,({void*_tmp795=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp646.f2=_tmp795;});_tmp646;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3196
({void*_tmp593=0U;({unsigned _tmp797=e->loc;struct _fat_ptr _tmp796=({const char*_tmp594="unary & applied to non-lvalue";_tag_fat(_tmp594,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp797,_tmp796,_tag_fat(_tmp593,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3203
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp595=({void*_tmp798=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp798,b);});struct Cyc_Absyn_Exp*eopt=_tmp595;
if(eopt == 0)return;{
struct Cyc_Absyn_Exp*_tmp596=eopt;struct Cyc_Absyn_Exp*e=_tmp596;
struct _tuple13 _tmp597=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple13 _stmttmp83=_tmp597;struct _tuple13 _tmp598=_stmttmp83;int _tmp59A;unsigned _tmp599;_LL1: _tmp599=_tmp598.f1;_tmp59A=_tmp598.f2;_LL2: {unsigned j=_tmp599;int known=_tmp59A;
if(known && j <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp59D=({struct Cyc_Int_pa_PrintArg_struct _tmp649;_tmp649.tag=1U,_tmp649.f1=(unsigned long)((int)j);_tmp649;});struct Cyc_Int_pa_PrintArg_struct _tmp59E=({struct Cyc_Int_pa_PrintArg_struct _tmp648;_tmp648.tag=1U,_tmp648.f1=(unsigned long)((int)i);_tmp648;});void*_tmp59B[2U];_tmp59B[0]=& _tmp59D,_tmp59B[1]=& _tmp59E;({unsigned _tmp79A=loc;struct _fat_ptr _tmp799=({const char*_tmp59C="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp59C,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp79A,_tmp799,_tag_fat(_tmp59B,sizeof(void*),2U));});});else{
# 3212
({struct Cyc_Int_pa_PrintArg_struct _tmp5A1=({struct Cyc_Int_pa_PrintArg_struct _tmp64B;_tmp64B.tag=1U,_tmp64B.f1=(unsigned long)((int)j);_tmp64B;});struct Cyc_Int_pa_PrintArg_struct _tmp5A2=({struct Cyc_Int_pa_PrintArg_struct _tmp64A;_tmp64A.tag=1U,_tmp64A.f1=(unsigned long)((int)i);_tmp64A;});void*_tmp59F[2U];_tmp59F[0]=& _tmp5A1,_tmp59F[1]=& _tmp5A2;({unsigned _tmp79C=loc;struct _fat_ptr _tmp79B=({const char*_tmp5A0="dereference is out of bounds: %d <= %d";_tag_fat(_tmp5A0,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp79C,_tmp79B,_tag_fat(_tmp59F,sizeof(void*),2U));});});}}
return;}}}
# 3216
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3224
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp5A3=e->r;void*_stmttmp84=_tmp5A3;void*_tmp5A4=_stmttmp84;struct Cyc_List_List*_tmp5A5;struct Cyc_List_List*_tmp5A6;struct Cyc_List_List*_tmp5A8;enum Cyc_Absyn_Primop _tmp5A7;struct Cyc_List_List*_tmp5A9;struct Cyc_List_List*_tmp5AA;struct Cyc_List_List*_tmp5AB;struct Cyc_Absyn_Exp*_tmp5AC;struct Cyc_Absyn_Exp*_tmp5AE;struct Cyc_Absyn_Exp*_tmp5AD;struct Cyc_Absyn_Exp*_tmp5AF;struct Cyc_Absyn_Exp*_tmp5B1;void*_tmp5B0;struct Cyc_Absyn_Exp*_tmp5B3;void*_tmp5B2;struct Cyc_Absyn_Exp*_tmp5B4;struct Cyc_Absyn_Exp*_tmp5B5;struct Cyc_Absyn_Exp*_tmp5B6;struct Cyc_Absyn_Exp*_tmp5B8;struct Cyc_Absyn_Exp*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5BB;struct Cyc_Absyn_Exp*_tmp5BA;struct Cyc_Absyn_Exp*_tmp5B9;void*_tmp5BC;switch(*((int*)_tmp5A4)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5BC=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL10: {void*b=_tmp5BC;
# 3236
void*_tmp5BD=b;struct Cyc_Absyn_Vardecl*_tmp5BE;struct Cyc_Absyn_Vardecl*_tmp5BF;switch(*((int*)_tmp5BD)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp5BF=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5BD)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5BF;
# 3239
void*_tmp5C0=Cyc_Tcutil_compress(vd->type);void*_stmttmp85=_tmp5C0;void*_tmp5C1=_stmttmp85;switch(*((int*)_tmp5C1)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp5BE=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5BD)->f1;_LL39: {struct Cyc_Absyn_Vardecl*vd=_tmp5BE;
# 3246
if((int)vd->sc == (int)Cyc_Absyn_Static){
void*_tmp5C2=Cyc_Tcutil_compress(vd->type);void*_stmttmp86=_tmp5C2;void*_tmp5C3=_stmttmp86;switch(*((int*)_tmp5C3)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3253
return var_okay;}}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp5B9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5BA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_tmp5BB=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5A4)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5B9;struct Cyc_Absyn_Exp*e2=_tmp5BA;struct Cyc_Absyn_Exp*e3=_tmp5BB;
# 3259
return(Cyc_Tcutil_cnst_exp(0,e1)&&
 Cyc_Tcutil_cnst_exp(0,e2))&&
 Cyc_Tcutil_cnst_exp(0,e3);}case 9U: _LL13: _tmp5B7=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5B8=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5B7;struct Cyc_Absyn_Exp*e2=_tmp5B8;
# 3263
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 41U: _LL15: _tmp5B6=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5B6;
_tmp5B5=e2;goto _LL18;}case 12U: _LL17: _tmp5B5=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5B5;
_tmp5B4=e2;goto _LL1A;}case 13U: _LL19: _tmp5B4=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5B4;
# 3267
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp5B2=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5B3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL1C: {void*t=_tmp5B2;struct Cyc_Absyn_Exp*e2=_tmp5B3;
# 3269
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}else{_LL1D: _tmp5B0=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5B1=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL1E: {void*t=_tmp5B0;struct Cyc_Absyn_Exp*e2=_tmp5B1;
# 3272
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}case 15U: _LL1F: _tmp5AF=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp5AF;
# 3274
return Cyc_Tcutil_cnst_exp(1,e2);}case 27U: _LL21: _tmp5AD=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_tmp5AE=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5A4)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp5AD;struct Cyc_Absyn_Exp*e2=_tmp5AE;
# 3276
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 28U: _LL23: _tmp5AC=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp5AC;
# 3278
return Cyc_Tcutil_cnst_exp(0,e);}case 26U: _LL25: _tmp5AB=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL26: {struct Cyc_List_List*des=_tmp5AB;
_tmp5AA=des;goto _LL28;}case 30U: _LL27: _tmp5AA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL28: {struct Cyc_List_List*des=_tmp5AA;
_tmp5A9=des;goto _LL2A;}case 29U: _LL29: _tmp5A9=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5A4)->f3;_LL2A: {struct Cyc_List_List*des=_tmp5A9;
# 3282
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL2B: _tmp5A7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_tmp5A8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5A4)->f2;_LL2C: {enum Cyc_Absyn_Primop p=_tmp5A7;struct Cyc_List_List*es=_tmp5A8;
# 3287
_tmp5A6=es;goto _LL2E;}case 24U: _LL2D: _tmp5A6=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL2E: {struct Cyc_List_List*es=_tmp5A6;
_tmp5A5=es;goto _LL30;}case 31U: _LL2F: _tmp5A5=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5A4)->f1;_LL30: {struct Cyc_List_List*es=_tmp5A5;
# 3290
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3297
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3301
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5C4=Cyc_Tcutil_compress(t);void*_stmttmp87=_tmp5C4;void*_tmp5C5=_stmttmp87;struct Cyc_List_List*_tmp5C6;struct Cyc_List_List*_tmp5C7;void*_tmp5C8;void*_tmp5C9;struct Cyc_List_List*_tmp5CB;void*_tmp5CA;switch(*((int*)_tmp5C5)){case 0U: _LL1: _tmp5CA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C5)->f1;_tmp5CB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5C5)->f2;_LL2: {void*c=_tmp5CA;struct Cyc_List_List*ts=_tmp5CB;
# 3305
void*_tmp5CC=c;union Cyc_Absyn_AggrInfo _tmp5CD;struct Cyc_List_List*_tmp5CE;struct Cyc_Absyn_Enumdecl*_tmp5CF;switch(*((int*)_tmp5CC)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5CF=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5CC)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5CF;
# 3310
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5CE=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5CE=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5CC)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5CE;
# 3314
{struct Cyc_List_List*_tmp5D0=fs;struct Cyc_List_List*fs2=_tmp5D0;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5D1=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp88=_tmp5D1;struct _tuple13 _tmp5D2=_stmttmp88;int _tmp5D4;unsigned _tmp5D3;_LL1D: _tmp5D3=_tmp5D2.f1;_tmp5D4=_tmp5D2.f2;_LL1E: {unsigned i=_tmp5D3;int known=_tmp5D4;
if(known && i == (unsigned)0)
return 1;}}}}
# 3321
return 0;}case 20U: _LL18: _tmp5CD=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5CC)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5CD;
# 3324
struct Cyc_Absyn_Aggrdecl*_tmp5D5=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5D5;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5C5)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5C9;
# 3332
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5C8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5C5)->f1).elt_type;_LL6: {void*t=_tmp5C8;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5C7=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5C5)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5C7;
# 3335
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))return 0;}
return 1;}case 7U: _LL9: _tmp5C6=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5C5)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5C6;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3342
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3344
struct _RegionHandle _tmp5D6=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5D6;_push_region(rgn);
{struct Cyc_List_List*_tmp5D7=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5D7;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5D8=0;_npop_handler(0U);return _tmp5D8;}}{
# 3352
int _tmp5D9=1;_npop_handler(0U);return _tmp5D9;}}
# 3345
;_pop_region();}
# 3356
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5DA=t;struct Cyc_Absyn_Typedefdecl*_tmp5DB;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5DA)->tag == 8U){_LL1: _tmp5DB=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5DA)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5DB;
# 3359
if(tdopt != 0){
struct Cyc_Absyn_Tqual _tmp5DC=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp5DC;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp5DF=({struct Cyc_String_pa_PrintArg_struct _tmp64C;_tmp64C.tag=0U,({struct _fat_ptr _tmp79D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp64C.f1=_tmp79D;});_tmp64C;});void*_tmp5DD[1U];_tmp5DD[0]=& _tmp5DF;({unsigned _tmp79F=loc;struct _fat_ptr _tmp79E=({const char*_tmp5DE="qualifier within typedef type %s is ignored";_tag_fat(_tmp5DE,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp79F,_tmp79E,_tag_fat(_tmp5DD,sizeof(void*),1U));});});}
# 3364
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3371
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp5E0=Cyc_Tcutil_compress(t);void*_stmttmp89=_tmp5E0;void*_tmp5E1=_stmttmp89;struct Cyc_List_List**_tmp5E2;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E1)->tag == 5U){_LL1: _tmp5E2=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp5E1)->f1).attributes;_LL2: {struct Cyc_List_List**fnatts=_tmp5E2;
# 3374
struct Cyc_List_List*_tmp5E3=0;struct Cyc_List_List*res_atts=_tmp5E3;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_tmp7A0=({struct Cyc_List_List*_tmp5E4=_cycalloc(sizeof(*_tmp5E4));_tmp5E4->hd=(void*)atts->hd,_tmp5E4->tl=*fnatts;_tmp5E4;});*fnatts=_tmp7A0;});}else{
# 3381
res_atts=({struct Cyc_List_List*_tmp5E5=_cycalloc(sizeof(*_tmp5E5));_tmp5E5->hd=(void*)atts->hd,_tmp5E5->tl=res_atts;_tmp5E5;});}}
return res_atts;}}else{_LL3: _LL4:
({void*_tmp5E6=0U;({struct _fat_ptr _tmp7A1=({const char*_tmp5E7="transfer_fn_type_atts";_tag_fat(_tmp5E7,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A1,_tag_fat(_tmp5E6,sizeof(void*),0U));});});}_LL0:;}
# 3388
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp5E8=Cyc_Tcutil_compress(t);void*_stmttmp8A=_tmp5E8;void*_tmp5E9=_stmttmp8A;struct Cyc_Absyn_Exp*_tmp5EA;struct Cyc_Absyn_PtrInfo _tmp5EB;switch(*((int*)_tmp5E9)){case 3U: _LL1: _tmp5EB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E9)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp5EB;
return({void*_tmp7A2=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7A2,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp5EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E9)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp5EA;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3398
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp5EC=b;struct Cyc_Absyn_Vardecl*_tmp5ED;struct Cyc_Absyn_Vardecl*_tmp5EE;struct Cyc_Absyn_Vardecl*_tmp5EF;struct Cyc_Absyn_Vardecl*_tmp5F0;switch(*((int*)_tmp5EC)){case 5U: _LL1: _tmp5F0=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp5EC)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp5F0;
_tmp5EF=x;goto _LL4;}case 4U: _LL3: _tmp5EF=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5EC)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp5EF;
_tmp5EE=x;goto _LL6;}case 3U: _LL5: _tmp5EE=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp5EC)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp5EE;
_tmp5ED=x;goto _LL8;}case 1U: _LL7: _tmp5ED=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5EC)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp5ED;
# 3404
if(!x->escapes)
return x;
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3412
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp5F1=0;struct Cyc_List_List*res=_tmp5F1;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)res=({struct Cyc_List_List*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->hd=*((void**)_check_null((void**)x->hd)),_tmp5F2->tl=res;_tmp5F2;});}}
return res;}
# 3419
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp5F3=Cyc_Tcutil_compress(t);void*_stmttmp8B=_tmp5F3;void*_tmp5F4=_stmttmp8B;unsigned _tmp5F9;void*_tmp5F8;struct Cyc_Absyn_Exp*_tmp5F7;struct Cyc_Absyn_Tqual _tmp5F6;void*_tmp5F5;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->tag == 4U){_LL1: _tmp5F5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).elt_type;_tmp5F6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).tq;_tmp5F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).num_elts;_tmp5F8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).zero_term;_tmp5F9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5F4)->f1).zt_loc;_LL2: {void*et=_tmp5F5;struct Cyc_Absyn_Tqual tq=_tmp5F6;struct Cyc_Absyn_Exp*eopt=_tmp5F7;void*zt=_tmp5F8;unsigned ztl=_tmp5F9;
# 3422
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 3426
if(convert_tag){
if(eopt->topt == 0)
({void*_tmp5FA=0U;({struct _fat_ptr _tmp7A3=({const char*_tmp5FB="cannot convert tag without type!";_tag_fat(_tmp5FB,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7A3,_tag_fat(_tmp5FA,sizeof(void*),0U));});});{
void*_tmp5FC=Cyc_Tcutil_compress((void*)_check_null(eopt->topt));void*_stmttmp8C=_tmp5FC;void*_tmp5FD=_stmttmp8C;void*_tmp5FE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5FD)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5FD)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5FD)->f2 != 0){_LL6: _tmp5FE=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5FD)->f2)->hd;_LL7: {void*t=_tmp5FE;
# 3432
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 3435
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3439
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
# 3441
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
