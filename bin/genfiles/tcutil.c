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
# 165
extern struct _RegionHandle*Cyc_Core_heap_region;
# 168
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
# 261
extern int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x);
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
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
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
# 907
void*Cyc_Absyn_new_evar(struct Cyc_Core_Opt*k,struct Cyc_Core_Opt*tenv);
# 912
extern void*Cyc_Absyn_uint_type;extern void*Cyc_Absyn_ulong_type;extern void*Cyc_Absyn_ulonglong_type;
# 914
extern void*Cyc_Absyn_sint_type;extern void*Cyc_Absyn_slong_type;extern void*Cyc_Absyn_slonglong_type;
# 919
extern void*Cyc_Absyn_heap_rgn_type;extern void*Cyc_Absyn_unique_rgn_type;extern void*Cyc_Absyn_refcnt_rgn_type;
# 921
extern void*Cyc_Absyn_empty_effect;
# 923
extern void*Cyc_Absyn_true_type;extern void*Cyc_Absyn_false_type;
# 925
extern void*Cyc_Absyn_void_type;extern void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);extern void*Cyc_Absyn_access_eff(void*);extern void*Cyc_Absyn_join_eff(struct Cyc_List_List*);extern void*Cyc_Absyn_regionsof_eff(void*);extern void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 950
extern void*Cyc_Absyn_fat_bound_type;
# 952
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 954
void*Cyc_Absyn_bounds_one (void);
# 956
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 960
void*Cyc_Absyn_atb_type(void*,void*,struct Cyc_Absyn_Tqual,void*,void*zero_term);
# 979
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 981
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 984
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 994
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1001
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1003
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1017
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1019
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1026
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
# 1038
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1043
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int,struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*,unsigned);
# 1047
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_assert_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1086
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1130
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1132
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple12{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1134
struct _tuple12*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1140
int Cyc_Absyn_fntype_att(void*);
# 1142
int Cyc_Absyn_equal_att(void*,void*);
# 1144
int Cyc_Absyn_attribute_cmp(void*,void*);
# 1150
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
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
# 111
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*);
# 114
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*,struct Cyc_Absyn_Exp*,void*);
# 118
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 120
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*,unsigned,void*,void*);
# 122
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*assume,void*t1,void*t2);
# 125
int Cyc_Tcutil_zero_to_null(void*,struct Cyc_Absyn_Exp*);
# 130
extern int Cyc_Tcutil_warn_alias_coerce;
# 133
extern int Cyc_Tcutil_warn_region_coerce;
# 136
extern struct Cyc_Absyn_Kind Cyc_Tcutil_rk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_bk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_mk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ek;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ik;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_boolk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk;
# 145
extern struct Cyc_Absyn_Kind Cyc_Tcutil_trk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tbk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_tmk;
# 150
extern struct Cyc_Absyn_Kind Cyc_Tcutil_urk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_uak;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_ubk;
extern struct Cyc_Absyn_Kind Cyc_Tcutil_umk;
# 155
extern struct Cyc_Core_Opt Cyc_Tcutil_rko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ako;
extern struct Cyc_Core_Opt Cyc_Tcutil_bko;
extern struct Cyc_Core_Opt Cyc_Tcutil_mko;
extern struct Cyc_Core_Opt Cyc_Tcutil_iko;
extern struct Cyc_Core_Opt Cyc_Tcutil_eko;
extern struct Cyc_Core_Opt Cyc_Tcutil_boolko;
extern struct Cyc_Core_Opt Cyc_Tcutil_ptrbko;
# 164
extern struct Cyc_Core_Opt Cyc_Tcutil_trko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tako;
extern struct Cyc_Core_Opt Cyc_Tcutil_tbko;
extern struct Cyc_Core_Opt Cyc_Tcutil_tmko;
# 169
extern struct Cyc_Core_Opt Cyc_Tcutil_urko;
extern struct Cyc_Core_Opt Cyc_Tcutil_uako;
extern struct Cyc_Core_Opt Cyc_Tcutil_ubko;
extern struct Cyc_Core_Opt Cyc_Tcutil_umko;
# 174
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*k);
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k);
# 183
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 186
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 188
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 194
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 197
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 201
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 221
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 234
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 237
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 242
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*);
# 247
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*);struct _tuple14{int f1;void*f2;};
# 251
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*);
# 254
void*Cyc_Tcutil_normalize_effect(void*e);
# 257
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*);
# 259
int Cyc_Tcutil_new_tvar_id (void);
# 261
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 274
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*);
# 278
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 296
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 299
int Cyc_Tcutil_zeroable_type(void*);
# 303
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 311
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 315
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 320
int Cyc_Tcutil_tycon_cmp(void*,void*);
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*,void*);
# 36 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_stmttmp0=_tmp0;void*_tmp1=_stmttmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 42
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_stmttmp1=_tmp2;void*_tmp3=_stmttmp1;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 48
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_stmttmp2=_tmp4;void*_tmp5=_stmttmp2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 54
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_stmttmp3=_tmp6;void*_tmp7=_stmttmp3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 61
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_stmttmp4=_tmp8;void*_tmp9=_stmttmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 68
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_stmttmp5=_tmpA;void*_tmpB=_stmttmp5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 75
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_stmttmp6=_tmpC;void*_tmpD=_stmttmp6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 82
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_stmttmp7=_tmpE;void*_tmpF=_stmttmp7;void*_tmp10;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp10=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {void*c=_tmp10;
# 85
void*_tmp11=c;switch(*((int*)_tmp11)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 92
 return 0;}_LL0:;}
# 95
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_stmttmp8=_tmp12;void*_tmp13=_stmttmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
 return 1;}else{goto _LL5;}case 2U: _LL3: _LL4:
 return 1;default: goto _LL5;}else{_LL5: _LL6:
 return 0;}_LL0:;}
# 102
int Cyc_Tcutil_is_arithmetic_type(void*t){
return Cyc_Tcutil_is_integral_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 105
int Cyc_Tcutil_is_strict_arithmetic_type(void*t){
return Cyc_Tcutil_is_any_int_type(t)|| Cyc_Tcutil_is_any_float_type(t);}
# 108
int Cyc_Tcutil_is_function_type(void*t){
void*_tmp14=Cyc_Tcutil_compress(t);void*_stmttmp9=_tmp14;void*_tmp15=_stmttmp9;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 114
int Cyc_Tcutil_is_typedef_type(void*t){
void*_tmp16=Cyc_Tcutil_compress(t);void*_stmttmpA=_tmp16;void*_tmp17=_stmttmpA;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 120
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 128
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp18=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmpB=_tmp18;void*_tmp19=_stmttmpB;void*_tmp1A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19)->tag == 1U){_LL1: _LL2:
 return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp1A=_tmp19;_LL4: {void*t=_tmp1A;
return Cyc_Tcutil_is_integral_type(t);}}_LL0:;}
# 136
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp1B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmpC=_tmp1B;void*_tmp1C=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 146
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1D=Cyc_Tcutil_compress(t);void*_stmttmpD=_tmp1D;void*_tmp1E=_stmttmpD;void*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->tag == 3U){_LL1: _tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->f1).ptr_atts).zero_term;_LL2: {void*ztl=_tmp1F;
# 149
return Cyc_Tcutil_force_type2bool(0,ztl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 155
int Cyc_Tcutil_is_nullable_pointer_type(void*t,int def){
void*_tmp20=Cyc_Tcutil_compress(t);void*_stmttmpE=_tmp20;void*_tmp21=_stmttmpE;void*_tmp22;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->tag == 3U){_LL1: _tmp22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp22;
# 158
return Cyc_Tcutil_force_type2bool(def,nbl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 164
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp23=Cyc_Tcutil_compress(t);void*_stmttmpF=_tmp23;void*_tmp24=_stmttmpF;void*_tmp25;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->tag == 3U){_LL1: _tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp25;
# 167
return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,b);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 174
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp26=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp26;void*_tmp27=_stmttmp10;void*_tmp29;void*_tmp28;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->tag == 3U){_LL1: _tmp28=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).elt_type;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).ptr_atts).bounds;_LL2: {void*elt_type=_tmp28;void*b=_tmp29;
# 177
if(Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=elt_type;
return 1;}
# 181
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 188
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp2A=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp2A;void*_tmp2B=_stmttmp11;void*_tmp2D;void*_tmp2C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->tag == 3U){_LL1: _tmp2C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_type;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;_LL2: {void*elt_type=_tmp2C;void*zt=_tmp2D;
# 191
*elt_type_dest=elt_type;
return Cyc_Absyn_type2bool(0,zt);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 200
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 202
void*_tmp2E=Cyc_Tcutil_compress(t);void*_stmttmp12=_tmp2E;void*_tmp2F=_stmttmp12;void*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Tqual _tmp31;void*_tmp30;void*_tmp36;void*_tmp35;void*_tmp34;switch(*((int*)_tmp2F)){case 3U: _LL1: _tmp34=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp35=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).bounds;_tmp36=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).zero_term;_LL2: {void*elt=_tmp34;void*bnds=_tmp35;void*zt=_tmp36;
# 204
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*ptr_type=t;
*elt_type=elt;
{void*_tmp37=Cyc_Tcutil_compress(bnds);void*_stmttmp13=_tmp37;void*_tmp38=_stmttmp13;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 212
return 1;}case 4U: _LL3: _tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).tq;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).zero_term;_LL4: {void*elt=_tmp30;struct Cyc_Absyn_Tqual tq=_tmp31;struct Cyc_Absyn_Exp*n=_tmp32;void*zt=_tmp33;
# 214
if(!Cyc_Absyn_type2bool(0,zt))
return 0;
*elt_type=elt;
*is_fat=0;
({void*_tmp6B6=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp6B6;});
return 1;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 227
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 233
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp39=({void*_tmp6B7=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp6B7,b);});struct Cyc_Absyn_Exp*eopt=_tmp39;
if(eopt == 0)return 0;{
struct _tuple13 _tmp3A=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp14=_tmp3A;struct _tuple13 _tmp3B=_stmttmp14;int _tmp3D;unsigned _tmp3C;_LL1: _tmp3C=_tmp3B.f1;_tmp3D=_tmp3B.f2;_LL2: {unsigned i=_tmp3C;int known=_tmp3D;
return known && i == (unsigned)1;}}}
# 241
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3E=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp3E;void*_tmp3F=_stmttmp15;struct Cyc_List_List*_tmp40;struct Cyc_List_List*_tmp41;void*_tmp43;void*_tmp42;struct Cyc_List_List*_tmp45;void*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LL1: _tmp44=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3F)->f2;_LL2: {void*c=_tmp44;struct Cyc_List_List*ts=_tmp45;
# 244
void*_tmp46=c;struct Cyc_Absyn_Aggrdecl*_tmp47;switch(*((int*)_tmp46)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 goto _LL13;case 16U: _LL12: _LL13:
 goto _LL15;case 15U: _LL14: _LL15:
 return 1;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp46)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
# 251
 return 0;}else{_LL18: _tmp47=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp46)->f1).KnownAggr).val;_LL19: {struct Cyc_Absyn_Aggrdecl*ad=_tmp47;
# 253
if(ad->impl == 0)
return 0;{
struct _RegionHandle _tmp48=_new_region("rgn");struct _RegionHandle*rgn=& _tmp48;_push_region(rgn);
{struct Cyc_List_List*_tmp49=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,ad->tvs,ts);struct Cyc_List_List*inst=_tmp49;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){
int _tmp4A=0;_npop_handler(0U);return _tmp4A;}}}{
int _tmp4B=1;_npop_handler(0U);return _tmp4B;}}
# 256
;_pop_region();}}}default: _LL1A: _LL1B:
# 261
 return 0;}_LLB:;}case 4U: _LL3: _tmp42=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).elt_type;_tmp43=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3F)->f1).zero_term;_LL4: {void*t=_tmp42;void*zero_term=_tmp43;
# 266
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6U: _LL5: _tmp41=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3F)->f1;_LL6: {struct Cyc_List_List*tqs=_tmp41;
# 268
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7U: _LL7: _tmp40=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3F)->f2;_LL8: {struct Cyc_List_List*fs=_tmp40;
# 273
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))
return 0;}
return 1;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 283
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
void*_tmp4C=Cyc_Tcutil_compress(t);void*_stmttmp16=_tmp4C;void*_tmp4D=_stmttmp16;struct Cyc_List_List*_tmp4E;void*_tmp4F;switch(*((int*)_tmp4D)){case 3U: _LL1: _tmp4F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4D)->f1).elt_type;_LL2: {void*elt=_tmp4F;
return Cyc_Tcutil_is_noreturn_fn_type(elt);}case 5U: _LL3: _tmp4E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4D)->f1).attributes;_LL4: {struct Cyc_List_List*atts=_tmp4E;
# 287
for(0;atts != 0;atts=atts->tl){
void*_tmp50=(void*)atts->hd;void*_stmttmp17=_tmp50;void*_tmp51=_stmttmp17;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp51)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 292
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 299
int Cyc_Tcutil_warn_region_coerce=0;
# 301
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 303
Cyc_Warn_verr(loc,fmt,ap);}
# 305
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 307
Cyc_Warn_vimpos(fmt,ap);}
# 309
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 311
Cyc_Warn_vwarn(sg,fmt,ap);}
# 315
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 320
void*Cyc_Tcutil_compress(void*t){
void*_tmp52=t;void*_tmp53;struct Cyc_Absyn_Exp*_tmp54;struct Cyc_Absyn_Exp*_tmp55;void**_tmp56;void**_tmp57;switch(*((int*)_tmp52)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp52)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp57=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp52)->f2;_LL8: {void**t2opt_ref=_tmp57;
# 326
void*ta=(void*)_check_null(*t2opt_ref);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*t2opt_ref=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f4 == 0){_LL3: _LL4:
# 323
 return t;}else{_LL5: _tmp56=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp52)->f4;_LL6: {void**topt_ref=_tmp56;
_tmp57=topt_ref;goto _LL8;}}case 9U: _LL9: _tmp55=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp52)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp55;
# 332
Cyc_Evexp_eval_const_uint_exp(e);{
void*_tmp58=e->r;void*_stmttmp18=_tmp58;void*_tmp59=_stmttmp18;void*_tmp5A;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp59)->tag == 39U){_LL12: _tmp5A=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp59)->f1;_LL13: {void*t2=_tmp5A;
return Cyc_Tcutil_compress(t2);}}else{_LL14: _LL15:
 return t;}_LL11:;}}case 11U: _LLB: _tmp54=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp52)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp54;
# 338
void*_tmp5B=e->topt;void*t2=_tmp5B;
return t2 != 0?t2: t;}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f2 != 0){_LLD: _tmp53=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp52)->f2;_LLE: {void*t=_tmp53;
return Cyc_Tcutil_compress(t);}}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 348
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp5C=Cyc_Absyn_compress_kb(kb);void*_stmttmp19=_tmp5C;void*_tmp5D=_stmttmp19;struct Cyc_Absyn_Kind*_tmp5E;switch(*((int*)_tmp5D)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp5F=_cycalloc(sizeof(*_tmp5F));_tmp5F->tag=1U,_tmp5F->f1=0;_tmp5F;});case 2U: _LL3: _tmp5E=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5D)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp5E;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=2U,_tmp60->f1=0,_tmp60->f2=k;_tmp60;});}default: _LL5: _LL6:
 return kb;}_LL0:;}
# 355
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
return({struct Cyc_Absyn_Tvar*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->name=tv->name,_tmp61->identity=- 1,({void*_tmp6B8=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp61->kind=_tmp6B8;});_tmp61;});}
# 358
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
struct _tuple9*_tmp62=arg;void*_tmp65;struct Cyc_Absyn_Tqual _tmp64;struct _fat_ptr*_tmp63;_LL1: _tmp63=_tmp62->f1;_tmp64=_tmp62->f2;_tmp65=_tmp62->f3;_LL2: {struct _fat_ptr*x=_tmp63;struct Cyc_Absyn_Tqual y=_tmp64;void*t=_tmp65;
return({struct _tuple9*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=x,_tmp66->f2=y,({void*_tmp6B9=Cyc_Tcutil_copy_type(t);_tmp66->f3=_tmp6B9;});_tmp66;});}}
# 362
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->f1=(*arg).f1,({void*_tmp6BA=Cyc_Tcutil_copy_type((*arg).f2);_tmp67->f2=_tmp6BA;});_tmp67;});}
# 365
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
return e == 0?0: Cyc_Tcutil_deep_copy_exp(preserve_types,e);}
# 368
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->name=f->name,_tmp68->tq=f->tq,({void*_tmp6BC=Cyc_Tcutil_copy_type(f->type);_tmp68->type=_tmp6BC;}),_tmp68->width=f->width,_tmp68->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp6BB=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp68->requires_clause=_tmp6BB;});_tmp68;});}struct _tuple15{void*f1;void*f2;};
# 372
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp69=x;void*_tmp6B;void*_tmp6A;_LL1: _tmp6A=_tmp69->f1;_tmp6B=_tmp69->f2;_LL2: {void*r1=_tmp6A;void*r2=_tmp6B;
return({struct _tuple15*_tmp6C=_cycalloc(sizeof(*_tmp6C));({void*_tmp6BE=Cyc_Tcutil_copy_type(r1);_tmp6C->f1=_tmp6BE;}),({void*_tmp6BD=Cyc_Tcutil_copy_type(r2);_tmp6C->f2=_tmp6BD;});_tmp6C;});}}
# 376
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 380
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6D=Cyc_Tcutil_compress(t);void*_stmttmp1A=_tmp6D;void*_tmp6E=_stmttmp1A;struct Cyc_Absyn_Datatypedecl*_tmp6F;struct Cyc_Absyn_Enumdecl*_tmp70;struct Cyc_Absyn_Aggrdecl*_tmp71;struct Cyc_Absyn_Typedefdecl*_tmp74;struct Cyc_List_List*_tmp73;struct _tuple1*_tmp72;struct Cyc_Absyn_Exp*_tmp75;struct Cyc_Absyn_Exp*_tmp76;struct Cyc_List_List*_tmp78;enum Cyc_Absyn_AggrKind _tmp77;struct Cyc_List_List*_tmp79;struct Cyc_Absyn_Vardecl*_tmp87;struct Cyc_List_List*_tmp86;struct Cyc_Absyn_Exp*_tmp85;struct Cyc_List_List*_tmp84;struct Cyc_Absyn_Exp*_tmp83;struct Cyc_List_List*_tmp82;struct Cyc_List_List*_tmp81;struct Cyc_Absyn_VarargInfo*_tmp80;int _tmp7F;struct Cyc_List_List*_tmp7E;void*_tmp7D;struct Cyc_Absyn_Tqual _tmp7C;void*_tmp7B;struct Cyc_List_List*_tmp7A;unsigned _tmp8C;void*_tmp8B;struct Cyc_Absyn_Exp*_tmp8A;struct Cyc_Absyn_Tqual _tmp89;void*_tmp88;struct Cyc_Absyn_PtrLoc*_tmp93;void*_tmp92;void*_tmp91;void*_tmp90;void*_tmp8F;struct Cyc_Absyn_Tqual _tmp8E;void*_tmp8D;struct Cyc_Absyn_Tvar*_tmp94;struct Cyc_List_List*_tmp96;void*_tmp95;void*_tmp97;switch(*((int*)_tmp6E)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f2 == 0){_LL1: _tmp97=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1;_LL2: {void*c=_tmp97;
return t;}}else{_LL3: _tmp95=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f1;_tmp96=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E)->f2;_LL4: {void*c=_tmp95;struct Cyc_List_List*ts=_tmp96;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=0U,_tmp98->f1=c,({struct Cyc_List_List*_tmp6BF=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp98->f2=_tmp6BF;});_tmp98;});}}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp94=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6E)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp94;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3U: _LL9: _tmp8D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E)->f1).elt_type;_tmp8E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E)->f1).elt_tq;_tmp8F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E)->f1).ptr_atts).rgn;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E)->f1).ptr_atts).nullable;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E)->f1).ptr_atts).bounds;_tmp92=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E)->f1).ptr_atts).zero_term;_tmp93=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E)->f1).ptr_atts).ptrloc;_LLA: {void*elt=_tmp8D;struct Cyc_Absyn_Tqual tq=_tmp8E;void*rgn=_tmp8F;void*nbl=_tmp90;void*bs=_tmp91;void*zt=_tmp92;struct Cyc_Absyn_PtrLoc*loc=_tmp93;
# 387
void*_tmp99=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp99;
void*_tmp9A=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp9A;
void*_tmp9B=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp9B;
struct Cyc_Absyn_Tqual _tmp9C=tq;struct Cyc_Absyn_Tqual tq2=_tmp9C;
# 392
void*_tmp9D=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp9D;
void*_tmp9E=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp9E;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp9F=_cycalloc(sizeof(*_tmp9F));_tmp9F->tag=3U,(_tmp9F->f1).elt_type=elt2,(_tmp9F->f1).elt_tq=tq2,((_tmp9F->f1).ptr_atts).rgn=rgn2,((_tmp9F->f1).ptr_atts).nullable=nbl2,((_tmp9F->f1).ptr_atts).bounds=bs2,((_tmp9F->f1).ptr_atts).zero_term=zt2,((_tmp9F->f1).ptr_atts).ptrloc=loc;_tmp9F;});}case 4U: _LLB: _tmp88=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6E)->f1).elt_type;_tmp89=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6E)->f1).tq;_tmp8A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6E)->f1).num_elts;_tmp8B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6E)->f1).zero_term;_tmp8C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6E)->f1).zt_loc;_LLC: {void*et=_tmp88;struct Cyc_Absyn_Tqual tq=_tmp89;struct Cyc_Absyn_Exp*eo=_tmp8A;void*zt=_tmp8B;unsigned ztl=_tmp8C;
# 396
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=4U,({void*_tmp6C2=Cyc_Tcutil_copy_type(et);(_tmpA0->f1).elt_type=_tmp6C2;}),(_tmpA0->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp6C1=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmpA0->f1).num_elts=_tmp6C1;}),({
void*_tmp6C0=Cyc_Tcutil_copy_type(zt);(_tmpA0->f1).zero_term=_tmp6C0;}),(_tmpA0->f1).zt_loc=ztl;_tmpA0;});}case 5U: _LLD: _tmp7A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).tvars;_tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).effect;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).ret_tqual;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).ret_type;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).args;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).c_varargs;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).cyc_varargs;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).rgn_po;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).attributes;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).requires_clause;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).requires_relns;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).ensures_clause;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).ensures_relns;_tmp87=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6E)->f1).return_value;_LLE: {struct Cyc_List_List*tvs=_tmp7A;void*effopt=_tmp7B;struct Cyc_Absyn_Tqual rt_tq=_tmp7C;void*rt=_tmp7D;struct Cyc_List_List*args=_tmp7E;int c_varargs=_tmp7F;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp80;struct Cyc_List_List*rgn_po=_tmp81;struct Cyc_List_List*atts=_tmp82;struct Cyc_Absyn_Exp*req=_tmp83;struct Cyc_List_List*req_rlns=_tmp84;struct Cyc_Absyn_Exp*ens=_tmp85;struct Cyc_List_List*ens_rlns=_tmp86;struct Cyc_Absyn_Vardecl*ret_val=_tmp87;
# 399
struct Cyc_List_List*_tmpA1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmpA1;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmpA2=Cyc_Tcutil_copy_type(rt);void*rt2=_tmpA2;
struct Cyc_List_List*_tmpA3=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmpA3;
int _tmpA4=c_varargs;int c_varargs2=_tmpA4;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=cyc_varargs == 0?0:({struct Cyc_Absyn_VarargInfo*_tmpAD=_cycalloc(sizeof(*_tmpAD));*_tmpAD=*cyc_varargs;_tmpAD;});
if(cyc_varargs != 0)
({void*_tmp6C3=Cyc_Tcutil_copy_type(cyc_varargs->type);((struct Cyc_Absyn_VarargInfo*)_check_null(cyc_varargs2))->type=_tmp6C3;});{
struct Cyc_List_List*_tmpA5=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmpA5;
struct Cyc_List_List*_tmpA6=atts;struct Cyc_List_List*atts2=_tmpA6;
struct Cyc_Absyn_Exp*_tmpA7=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmpA7;
struct Cyc_List_List*_tmpA8=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmpA8;
struct Cyc_Absyn_Exp*_tmpA9=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmpA9;
struct Cyc_List_List*_tmpAA=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmpAA;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmpAB=_cycalloc(sizeof(*_tmpAB));*_tmpAB=*ret_val;_tmpAB;});
({void*_tmp6C4=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp6C4;});
if((unsigned)ret_val2->rgn)({void*_tmp6C5=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp6C5;});}
# 419
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAC=_cycalloc(sizeof(*_tmpAC));_tmpAC->tag=5U,(_tmpAC->f1).tvars=tvs2,(_tmpAC->f1).effect=effopt2,(_tmpAC->f1).ret_tqual=rt_tq,(_tmpAC->f1).ret_type=rt2,(_tmpAC->f1).args=args2,(_tmpAC->f1).c_varargs=c_varargs2,(_tmpAC->f1).cyc_varargs=cyc_varargs2,(_tmpAC->f1).rgn_po=rgn_po2,(_tmpAC->f1).attributes=atts2,(_tmpAC->f1).requires_clause=req2,(_tmpAC->f1).requires_relns=req_rlns2,(_tmpAC->f1).ensures_clause=ens2,(_tmpAC->f1).ensures_relns=ens_rlns2,(_tmpAC->f1).return_value=ret_val2;_tmpAC;});}}case 6U: _LLF: _tmp79=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6E)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp79;
# 422
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=6U,({struct Cyc_List_List*_tmp6C6=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmpAE->f1=_tmp6C6;});_tmpAE;});}case 7U: _LL11: _tmp77=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6E)->f1;_tmp78=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6E)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp77;struct Cyc_List_List*fs=_tmp78;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=7U,_tmpAF->f1=k,({struct Cyc_List_List*_tmp6C7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmpAF->f2=_tmp6C7;});_tmpAF;});}case 9U: _LL13: _tmp76=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6E)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp76;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=9U,_tmpB0->f1=e;_tmpB0;});}case 11U: _LL15: _tmp75=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6E)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp75;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=11U,_tmpB1->f1=e;_tmpB1;});}case 8U: _LL17: _tmp72=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6E)->f1;_tmp73=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6E)->f2;_tmp74=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6E)->f3;_LL18: {struct _tuple1*tdn=_tmp72;struct Cyc_List_List*ts=_tmp73;struct Cyc_Absyn_Typedefdecl*td=_tmp74;
# 427
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=8U,_tmpB2->f1=tdn,({struct Cyc_List_List*_tmp6C8=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmpB2->f2=_tmp6C8;}),_tmpB2->f3=td,_tmpB2->f4=0;_tmpB2;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6E)->f1)->r)){case 0U: _LL19: _tmp71=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6E)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp71;
# 430
struct Cyc_List_List*_tmpB3=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpB3;
return({union Cyc_Absyn_AggrInfo _tmp6C9=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp6C9,targs);});}case 1U: _LL1B: _tmp70=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6E)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp70;
# 433
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp6F=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6E)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp6F;
# 435
struct Cyc_List_List*_tmpB4=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpB4;
return({union Cyc_Absyn_DatatypeInfo _tmp6CA=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp62D;_tmp62D.name=dd->name,_tmp62D.is_extensible=0;_tmp62D;}));Cyc_Absyn_datatype_type(_tmp6CA,targs);});}}}_LL0:;}
# 441
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB5=d;struct _fat_ptr*_tmpB6;struct Cyc_Absyn_Exp*_tmpB7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB5)->tag == 0U){_LL1: _tmpB7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB5)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpB7;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB8=_cycalloc(sizeof(*_tmpB8));_tmpB8->tag=0U,({struct Cyc_Absyn_Exp*_tmp6CB=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpB8->f1=_tmp6CB;});_tmpB8;});}}else{_LL3: _tmpB6=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB5)->f1;_LL4: {struct _fat_ptr*v=_tmpB6;
return d;}}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 447
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 449
return({struct _tuple16*_tmpB9=_cycalloc(sizeof(*_tmpB9));({struct Cyc_List_List*_tmp6CD=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpB9->f1=_tmp6CD;}),({
struct Cyc_Absyn_Exp*_tmp6CC=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpB9->f2=_tmp6CC;});_tmpB9;});}
# 453
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*(*_tmpBA)(int preserve_types,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_deep_copy_exp;struct Cyc_Absyn_Exp*(*DC)(int preserve_types,struct Cyc_Absyn_Exp*e)=_tmpBA;
struct Cyc_Absyn_Exp*new_e;
int _tmpBB=preserve_types;int pt=_tmpBB;
{void*_tmpBC=e->r;void*_stmttmp1B=_tmpBC;void*_tmpBD=_stmttmp1B;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC2;struct Cyc_List_List*_tmpC1;struct _fat_ptr _tmpC0;int _tmpBF;void*_tmpC4;struct _fat_ptr*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_List_List*_tmpC8;struct Cyc_Core_Opt*_tmpC7;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;int _tmpD0;int _tmpCF;struct Cyc_Absyn_Exp*_tmpCE;void**_tmpCD;struct Cyc_Absyn_Exp*_tmpCC;int _tmpCB;struct Cyc_Absyn_Enumfield*_tmpD2;void*_tmpD1;struct Cyc_Absyn_Enumfield*_tmpD4;struct Cyc_Absyn_Enumdecl*_tmpD3;struct Cyc_List_List*_tmpD8;void*_tmpD7;struct Cyc_Absyn_Tqual _tmpD6;struct _fat_ptr*_tmpD5;struct Cyc_List_List*_tmpDA;void*_tmpD9;struct Cyc_Absyn_Aggrdecl*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_List_List*_tmpDC;struct _tuple1*_tmpDB;int _tmpE1;void*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;int _tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Vardecl*_tmpE2;struct Cyc_Absyn_Datatypefield*_tmpE8;struct Cyc_Absyn_Datatypedecl*_tmpE7;struct Cyc_List_List*_tmpE6;struct Cyc_List_List*_tmpE9;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEA;int _tmpEF;int _tmpEE;struct _fat_ptr*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;int _tmpF3;int _tmpF2;struct _fat_ptr*_tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_List_List*_tmpF8;void*_tmpF7;struct Cyc_Absyn_Exp*_tmpF9;void*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;enum Cyc_Absyn_Coercion _tmp101;int _tmp100;struct Cyc_Absyn_Exp*_tmpFF;void*_tmpFE;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp104;int _tmp106;struct Cyc_Absyn_Exp*_tmp105;int _tmp10A;struct Cyc_Absyn_VarargCallInfo*_tmp109;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_Exp*_tmp107;int _tmp113;int _tmp112;void*_tmp111;struct Cyc_Absyn_Tqual _tmp110;struct _fat_ptr*_tmp10F;struct Cyc_List_List*_tmp10E;int _tmp10D;struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Core_Opt*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;enum Cyc_Absyn_Incrementor _tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_List_List*_tmp123;enum Cyc_Absyn_Primop _tmp122;struct _fat_ptr _tmp124;void*_tmp125;union Cyc_Absyn_Cnst _tmp126;switch(*((int*)_tmpBD)){case 0U: _LL1: _tmp126=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp126;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp125=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL4: {void*b=_tmp125;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp124=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL6: {struct _fat_ptr p=_tmp124;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp122=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp123=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp122;struct Cyc_List_List*es=_tmp123;
new_e=({enum Cyc_Absyn_Primop _tmp6CF=p;struct Cyc_List_List*_tmp6CE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_primop_exp(_tmp6CF,_tmp6CE,e->loc);});goto _LL0;}case 5U: _LL9: _tmp120=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp121=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp120;enum Cyc_Absyn_Incrementor i=_tmp121;
new_e=({struct Cyc_Absyn_Exp*_tmp6D1=DC(pt,e1);enum Cyc_Absyn_Incrementor _tmp6D0=i;Cyc_Absyn_increment_exp(_tmp6D1,_tmp6D0,e->loc);});goto _LL0;}case 7U: _LLB: _tmp11E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp11F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp11E;struct Cyc_Absyn_Exp*e2=_tmp11F;
new_e=({struct Cyc_Absyn_Exp*_tmp6D3=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6D2=DC(pt,e2);Cyc_Absyn_and_exp(_tmp6D3,_tmp6D2,e->loc);});goto _LL0;}case 8U: _LLD: _tmp11C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp11D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp11C;struct Cyc_Absyn_Exp*e2=_tmp11D;
new_e=({struct Cyc_Absyn_Exp*_tmp6D5=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6D4=DC(pt,e2);Cyc_Absyn_or_exp(_tmp6D5,_tmp6D4,e->loc);});goto _LL0;}case 9U: _LLF: _tmp11A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp11B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
new_e=({struct Cyc_Absyn_Exp*_tmp6D7=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6D6=DC(pt,e2);Cyc_Absyn_seq_exp(_tmp6D7,_tmp6D6,e->loc);});goto _LL0;}case 6U: _LL11: _tmp117=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp118=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp117;struct Cyc_Absyn_Exp*e2=_tmp118;struct Cyc_Absyn_Exp*e3=_tmp119;
# 467
new_e=({struct Cyc_Absyn_Exp*_tmp6DA=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6D9=DC(pt,e2);struct Cyc_Absyn_Exp*_tmp6D8=DC(pt,e3);Cyc_Absyn_conditional_exp(_tmp6DA,_tmp6D9,_tmp6D8,e->loc);});goto _LL0;}case 4U: _LL13: _tmp114=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp115=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp116=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp114;struct Cyc_Core_Opt*po=_tmp115;struct Cyc_Absyn_Exp*e2=_tmp116;
# 469
new_e=({struct Cyc_Absyn_Exp*_tmp6DD=DC(pt,e1);struct Cyc_Core_Opt*_tmp6DC=(unsigned)po?({struct Cyc_Core_Opt*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->v=(void*)po->v;_tmp127;}): 0;struct Cyc_Absyn_Exp*_tmp6DB=DC(pt,e2);Cyc_Absyn_assignop_exp(_tmp6DD,_tmp6DC,_tmp6DB,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3 != 0){_LL15: _tmp10B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp10C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp10D=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->num_varargs;_tmp10E=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->injectors;_tmp10F=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->vai)->name;_tmp110=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->vai)->tq;_tmp111=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->vai)->type;_tmp112=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->vai)->inject;_tmp113=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp10B;struct Cyc_List_List*es=_tmp10C;int n=_tmp10D;struct Cyc_List_List*is=_tmp10E;struct _fat_ptr*nm=_tmp10F;struct Cyc_Absyn_Tqual tq=_tmp110;void*t=_tmp111;int i=_tmp112;int resolved=_tmp113;
# 472
new_e=({void*_tmp6E3=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=10U,({
struct Cyc_Absyn_Exp*_tmp6E2=DC(pt,e1);_tmp12A->f1=_tmp6E2;}),({struct Cyc_List_List*_tmp6E1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp12A->f2=_tmp6E1;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp6E0=({struct Cyc_Absyn_VarargCallInfo*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->num_varargs=n,_tmp129->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp6DF=({struct Cyc_Absyn_VarargInfo*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->name=nm,_tmp128->tq=tq,({void*_tmp6DE=Cyc_Tcutil_copy_type(t);_tmp128->type=_tmp6DE;}),_tmp128->inject=i;_tmp128;});_tmp129->vai=_tmp6DF;});_tmp129;});
# 474
_tmp12A->f3=_tmp6E0;}),_tmp12A->f4=resolved;_tmp12A;});
# 472
Cyc_Absyn_new_exp(_tmp6E3,e->loc);});
# 477
goto _LL0;}}else{_LL17: _tmp107=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp108=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp109=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmp10A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp107;struct Cyc_List_List*es=_tmp108;struct Cyc_Absyn_VarargCallInfo*vci=_tmp109;int resolved=_tmp10A;
# 479
new_e=({void*_tmp6E6=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=10U,({struct Cyc_Absyn_Exp*_tmp6E5=DC(pt,e1);_tmp12B->f1=_tmp6E5;}),({struct Cyc_List_List*_tmp6E4=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp12B->f2=_tmp6E4;}),_tmp12B->f3=vci,_tmp12B->f4=resolved;_tmp12B;});Cyc_Absyn_new_exp(_tmp6E6,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmp105=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp106=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp105;int b=_tmp106;
# 482
new_e=b?({struct Cyc_Absyn_Exp*_tmp6E8=DC(pt,e1);Cyc_Absyn_rethrow_exp(_tmp6E8,e->loc);}):({struct Cyc_Absyn_Exp*_tmp6E7=DC(pt,e1);Cyc_Absyn_throw_exp(_tmp6E7,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmp104=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp104;
# 485
new_e=({struct Cyc_Absyn_Exp*_tmp6E9=DC(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp6E9,e->loc);});goto _LL0;}case 13U: _LL1D: _tmp102=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp103=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp102;struct Cyc_List_List*ts=_tmp103;
# 487
new_e=({struct Cyc_Absyn_Exp*_tmp6EB=DC(pt,e1);struct Cyc_List_List*_tmp6EA=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp6EB,_tmp6EA,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpFE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpFF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp100=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmp101=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL20: {void*t=_tmpFE;struct Cyc_Absyn_Exp*e1=_tmpFF;int b=_tmp100;enum Cyc_Absyn_Coercion c=_tmp101;
# 489
new_e=({void*_tmp6EF=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp6EE=DC(pt,e1);int _tmp6ED=b;enum Cyc_Absyn_Coercion _tmp6EC=c;Cyc_Absyn_cast_exp(_tmp6EF,_tmp6EE,_tmp6ED,_tmp6EC,e->loc);});goto _LL0;}case 16U: _LL21: _tmpFC=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpFD=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpFC;struct Cyc_Absyn_Exp*e1=_tmpFD;
# 491
new_e=({struct Cyc_Absyn_Exp*_tmp6F1=(unsigned)eo?DC(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp6F0=DC(pt,e1);Cyc_Absyn_New_exp(_tmp6F1,_tmp6F0,e->loc);});goto _LL0;}case 15U: _LL23: _tmpFB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpFB;
new_e=({struct Cyc_Absyn_Exp*_tmp6F2=DC(pt,e1);Cyc_Absyn_address_exp(_tmp6F2,e->loc);});goto _LL0;}case 17U: _LL25: _tmpFA=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL26: {void*t=_tmpFA;
new_e=({void*_tmp6F3=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp6F3,e->loc);});goto _LL0;}case 18U: _LL27: _tmpF9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF9;
new_e=({struct Cyc_Absyn_Exp*_tmp6F4=DC(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp6F4,e->loc);});goto _LL0;}case 19U: _LL29: _tmpF7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpF8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL2A: {void*t=_tmpF7;struct Cyc_List_List*ofs=_tmpF8;
new_e=({void*_tmp6F6=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp6F5=ofs;Cyc_Absyn_offsetof_exp(_tmp6F6,_tmp6F5,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpF6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpF6;
new_e=({struct Cyc_Absyn_Exp*_tmp6F7=DC(pt,e1);Cyc_Absyn_deref_exp(_tmp6F7,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpF5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpF5;
new_e=({struct Cyc_Absyn_Exp*_tmp6F8=DC(pt,e1);Cyc_Absyn_extension_exp(_tmp6F8,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpF4=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL30: {struct Cyc_List_List*es=_tmpF4;
new_e=({struct Cyc_List_List*_tmp6F9=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);Cyc_Absyn_tuple_exp(_tmp6F9,e->loc);});goto _LL0;}case 21U: _LL31: _tmpF0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpF1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpF2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpF3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpF0;struct _fat_ptr*n=_tmpF1;int f1=_tmpF2;int f2=_tmpF3;
# 500
new_e=({void*_tmp6FB=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=21U,({struct Cyc_Absyn_Exp*_tmp6FA=DC(pt,e1);_tmp12C->f1=_tmp6FA;}),_tmp12C->f2=n,_tmp12C->f3=f1,_tmp12C->f4=f2;_tmp12C;});Cyc_Absyn_new_exp(_tmp6FB,e->loc);});goto _LL0;}case 22U: _LL33: _tmpEC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpED=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpEE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpEF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpEC;struct _fat_ptr*n=_tmpED;int f1=_tmpEE;int f2=_tmpEF;
# 502
new_e=({void*_tmp6FD=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=22U,({struct Cyc_Absyn_Exp*_tmp6FC=DC(pt,e1);_tmp12D->f1=_tmp6FC;}),_tmp12D->f2=n,_tmp12D->f3=f1,_tmp12D->f4=f2;_tmp12D;});Cyc_Absyn_new_exp(_tmp6FD,e->loc);});goto _LL0;}case 23U: _LL35: _tmpEA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpEB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpEA;struct Cyc_Absyn_Exp*e2=_tmpEB;
# 504
new_e=({struct Cyc_Absyn_Exp*_tmp6FF=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp6FE=DC(pt,e2);Cyc_Absyn_subscript_exp(_tmp6FF,_tmp6FE,e->loc);});goto _LL0;}case 26U: _LL37: _tmpE9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL38: {struct Cyc_List_List*eds=_tmpE9;
# 506
new_e=({void*_tmp701=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=26U,({struct Cyc_List_List*_tmp700=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12E->f1=_tmp700;});_tmp12E;});Cyc_Absyn_new_exp(_tmp701,e->loc);});goto _LL0;}case 31U: _LL39: _tmpE6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpE7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpE8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_LL3A: {struct Cyc_List_List*es=_tmpE6;struct Cyc_Absyn_Datatypedecl*dtd=_tmpE7;struct Cyc_Absyn_Datatypefield*dtf=_tmpE8;
# 508
new_e=({void*_tmp703=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->tag=31U,({struct Cyc_List_List*_tmp702=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(DC,pt,es);_tmp12F->f1=_tmp702;}),_tmp12F->f2=dtd,_tmp12F->f3=dtf;_tmp12F;});Cyc_Absyn_new_exp(_tmp703,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpE2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpE3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpE4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpE5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpE2;struct Cyc_Absyn_Exp*e1=_tmpE3;struct Cyc_Absyn_Exp*e2=_tmpE4;int b=_tmpE5;
# 510
new_e=({void*_tmp706=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=27U,_tmp130->f1=vd,({struct Cyc_Absyn_Exp*_tmp705=DC(pt,e1);_tmp130->f2=_tmp705;}),({struct Cyc_Absyn_Exp*_tmp704=DC(pt,e2);_tmp130->f3=_tmp704;}),_tmp130->f4=b;_tmp130;});Cyc_Absyn_new_exp(_tmp706,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpDF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpE0=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpE1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpDF;void*t=_tmpE0;int b=_tmpE1;
# 512
new_e=({void*_tmp709=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=28U,({struct Cyc_Absyn_Exp*_tmp708=DC(pt,e);_tmp131->f1=_tmp708;}),({void*_tmp707=Cyc_Tcutil_copy_type(t);_tmp131->f2=_tmp707;}),_tmp131->f3=b;_tmp131;});Cyc_Absyn_new_exp(_tmp709,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpDB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpDC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpDD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpDE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL40: {struct _tuple1*n=_tmpDB;struct Cyc_List_List*ts=_tmpDC;struct Cyc_List_List*eds=_tmpDD;struct Cyc_Absyn_Aggrdecl*agr=_tmpDE;
# 515
new_e=({void*_tmp70C=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=29U,_tmp132->f1=n,({struct Cyc_List_List*_tmp70B=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp132->f2=_tmp70B;}),({struct Cyc_List_List*_tmp70A=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp132->f3=_tmp70A;}),_tmp132->f4=agr;_tmp132;});Cyc_Absyn_new_exp(_tmp70C,e->loc);});
# 517
goto _LL0;}case 30U: _LL41: _tmpD9=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpDA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL42: {void*t=_tmpD9;struct Cyc_List_List*eds=_tmpDA;
# 519
new_e=({void*_tmp70F=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->tag=30U,({void*_tmp70E=Cyc_Tcutil_copy_type(t);_tmp133->f1=_tmp70E;}),({struct Cyc_List_List*_tmp70D=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp133->f2=_tmp70D;});_tmp133;});Cyc_Absyn_new_exp(_tmp70F,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpD5=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBD)->f1)->f1;_tmpD6=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBD)->f1)->f2;_tmpD7=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBD)->f1)->f3;_tmpD8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL44: {struct _fat_ptr*vopt=_tmpD5;struct Cyc_Absyn_Tqual tq=_tmpD6;void*t=_tmpD7;struct Cyc_List_List*eds=_tmpD8;
# 522
new_e=({void*_tmp713=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->tag=25U,({struct _tuple9*_tmp712=({struct _tuple9*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->f1=vopt,_tmp134->f2=tq,({void*_tmp711=Cyc_Tcutil_copy_type(t);_tmp134->f3=_tmp711;});_tmp134;});_tmp135->f1=_tmp712;}),({
struct Cyc_List_List*_tmp710=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp135->f2=_tmp710;});_tmp135;});
# 522
Cyc_Absyn_new_exp(_tmp713,e->loc);});
# 524
goto _LL0;}case 32U: _LL45: _tmpD3=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpD4=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpD3;struct Cyc_Absyn_Enumfield*ef=_tmpD4;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpD1=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpD2=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL48: {void*t=_tmpD1;struct Cyc_Absyn_Enumfield*ef=_tmpD2;
# 527
new_e=({void*_tmp715=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->tag=33U,({void*_tmp714=Cyc_Tcutil_copy_type(t);_tmp136->f1=_tmp714;}),_tmp136->f2=ef;_tmp136;});Cyc_Absyn_new_exp(_tmp715,e->loc);});goto _LL0;}case 34U: _LL49: _tmpCB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).is_calloc;_tmpCC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).rgn;_tmpCD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).elt_type;_tmpCE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).num_elts;_tmpCF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).fat_result;_tmpD0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).inline_call;_LL4A: {int ic=_tmpCB;struct Cyc_Absyn_Exp*r=_tmpCC;void**t=_tmpCD;struct Cyc_Absyn_Exp*n=_tmpCE;int res=_tmpCF;int inlc=_tmpD0;
# 529
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=DC(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp137=_cycalloc(sizeof(*_tmp137));({void*_tmp716=Cyc_Tcutil_copy_type(*t);*_tmp137=_tmp716;});_tmp137;});
({void*_tmp717=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->tag=34U,(_tmp138->f1).is_calloc=ic,(_tmp138->f1).rgn=r1,(_tmp138->f1).elt_type=t1,(_tmp138->f1).num_elts=n,(_tmp138->f1).fat_result=res,(_tmp138->f1).inline_call=inlc;_tmp138;});e2->r=_tmp717;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpC9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpCA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpC9;struct Cyc_Absyn_Exp*e2=_tmpCA;
new_e=({struct Cyc_Absyn_Exp*_tmp719=DC(pt,e1);struct Cyc_Absyn_Exp*_tmp718=DC(pt,e2);Cyc_Absyn_swap_exp(_tmp719,_tmp718,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpC7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpC8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpC7;struct Cyc_List_List*eds=_tmpC8;
# 537
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->v=(struct _tuple1*)nopt->v;_tmp139;});
new_e=({void*_tmp71B=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->tag=36U,_tmp13A->f1=nopt1,({struct Cyc_List_List*_tmp71A=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp13A->f2=_tmp71A;});_tmp13A;});Cyc_Absyn_new_exp(_tmp71B,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 542
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp71C=({const char*_tmp13B="deep_copy: statement expressions unsupported";_tag_fat(_tmp13B,sizeof(char),45U);});_tmp13C->f1=_tmp71C;});_tmp13C;}));case 38U: _LL51: _tmpC5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpC6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpC5;struct _fat_ptr*fn=_tmpC6;
# 544
new_e=({void*_tmp71E=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->tag=38U,({struct Cyc_Absyn_Exp*_tmp71D=DC(pt,e1);_tmp13D->f1=_tmp71D;}),_tmp13D->f2=fn;_tmp13D;});Cyc_Absyn_new_exp(_tmp71E,e->loc);});goto _LL0;}case 39U: _LL53: _tmpC4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL54: {void*t=_tmpC4;
new_e=({void*_tmp71F=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp71F,e->loc);});goto _LL0;}case 40U: _LL55: _tmpBF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpC0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpC1=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpC2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_tmpC3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f5;_LL56: {int v=_tmpBF;struct _fat_ptr t=_tmpC0;struct Cyc_List_List*o=_tmpC1;struct Cyc_List_List*i=_tmpC2;struct Cyc_List_List*c=_tmpC3;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _LL57: _tmpBE=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL58: {struct Cyc_Absyn_Exp*e1=_tmpBE;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 550
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 554
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 565 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp13E=ka1;enum Cyc_Absyn_AliasQual _tmp140;enum Cyc_Absyn_KindQual _tmp13F;_LL1: _tmp13F=_tmp13E->kind;_tmp140=_tmp13E->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp13F;enum Cyc_Absyn_AliasQual a1=_tmp140;
struct Cyc_Absyn_Kind*_tmp141=ka2;enum Cyc_Absyn_AliasQual _tmp143;enum Cyc_Absyn_KindQual _tmp142;_LL4: _tmp142=_tmp141->kind;_tmp143=_tmp141->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp142;enum Cyc_Absyn_AliasQual a2=_tmp143;
# 569
if((int)k1 != (int)k2){
struct _tuple17 _tmp144=({struct _tuple17 _tmp62E;_tmp62E.f1=k1,_tmp62E.f2=k2;_tmp62E;});struct _tuple17 _stmttmp1C=_tmp144;struct _tuple17 _tmp145=_stmttmp1C;switch(_tmp145.f1){case Cyc_Absyn_BoxKind: switch(_tmp145.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp145.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 577
if((int)a1 != (int)a2){
struct _tuple18 _tmp146=({struct _tuple18 _tmp62F;_tmp62F.f1=a1,_tmp62F.f2=a2;_tmp62F;});struct _tuple18 _stmttmp1D=_tmp146;struct _tuple18 _tmp147=_stmttmp1D;switch(_tmp147.f1){case Cyc_Absyn_Aliasable: if(_tmp147.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp147.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 583
return 1;}}}
# 586
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp148=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1E=_tmp148;void*_tmp149=_stmttmp1E;struct Cyc_Absyn_Kind*_tmp14A;struct Cyc_Absyn_Kind*_tmp14B;switch(*((int*)_tmp149)){case 0U: _LL1: _tmp14B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp149)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp14B;
return k;}case 2U: _LL3: _tmp14A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp149)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp14A;
return k;}default: _LL5: _LL6:
({void*_tmp720=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->tag=2U,_tmp14C->f1=0,_tmp14C->f2=def;_tmp14C;});tv->kind=_tmp720;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 594
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp14D=Cyc_Tcutil_compress(t);void*_stmttmp1F=_tmp14D;void*_tmp14E=_stmttmp1F;struct Cyc_Absyn_Tvar*_tmp14F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->tag == 2U){_LL1: _tmp14F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp14F;
# 597
void*_tmp150=tv->kind;void*oldkb=_tmp150;
tv->kind=kb;
return({struct _tuple19 _tmp630;_tmp630.f1=tv,_tmp630.f2=oldkb;_tmp630;});}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp152=({struct Cyc_Warn_String_Warn_Warg_struct _tmp632;_tmp632.tag=0U,({struct _fat_ptr _tmp721=({const char*_tmp154="swap_kind: cannot update the kind of ";_tag_fat(_tmp154,sizeof(char),38U);});_tmp632.f1=_tmp721;});_tmp632;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp153=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp631;_tmp631.tag=2U,_tmp631.f1=(void*)t;_tmp631;});void*_tmp151[2U];_tmp151[0]=& _tmp152,_tmp151[1]=& _tmp153;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp151,sizeof(void*),2U));});}_LL0:;}
# 606
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 608
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 612
struct _RegionHandle _tmp155=_new_region("temp");struct _RegionHandle*temp=& _tmp155;_push_region(temp);
{struct Cyc_List_List*_tmp156=0;struct Cyc_List_List*inst=_tmp156;
# 615
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp157=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp157;
void*_tmp158=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp158;
struct Cyc_Absyn_Kind*_tmp159=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp20=_tmp159;struct Cyc_Absyn_Kind*_tmp15A=_stmttmp20;switch(((struct Cyc_Absyn_Kind*)_tmp15A)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 621
 inst=({struct Cyc_List_List*_tmp15C=_region_malloc(temp,sizeof(*_tmp15C));({struct _tuple19*_tmp722=({struct _tuple19*_tmp15B=_region_malloc(temp,sizeof(*_tmp15B));_tmp15B->f1=tv,_tmp15B->f2=t;_tmp15B;});_tmp15C->hd=_tmp722;}),_tmp15C->tl=inst;_tmp15C;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 625
if(inst != 0){
field_type=({struct _RegionHandle*_tmp724=temp;struct Cyc_List_List*_tmp723=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp724,_tmp723,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 613
;_pop_region();}
# 630
return k;}
# 637
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 639
void*_tmp15D=Cyc_Tcutil_compress(t);void*_stmttmp21=_tmp15D;void*_tmp15E=_stmttmp21;struct Cyc_Absyn_Typedefdecl*_tmp15F;struct Cyc_Absyn_Exp*_tmp160;struct Cyc_Absyn_PtrInfo _tmp161;struct Cyc_List_List*_tmp163;void*_tmp162;struct Cyc_Absyn_Tvar*_tmp164;struct Cyc_Core_Opt*_tmp165;switch(*((int*)_tmp15E)){case 1U: _LL1: _tmp165=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15E)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp165;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp164=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp15E)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp164;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp162=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15E)->f1;_tmp163=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15E)->f2;_LL6: {void*c=_tmp162;struct Cyc_List_List*ts=_tmp163;
# 643
void*_tmp166=c;int _tmp16A;struct Cyc_Absyn_AggrdeclImpl*_tmp169;struct Cyc_List_List*_tmp168;enum Cyc_Absyn_AggrKind _tmp167;struct Cyc_Absyn_Kind*_tmp16B;enum Cyc_Absyn_Size_of _tmp16C;switch(*((int*)_tmp166)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp16C=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp166)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp16C;
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp16B=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp166)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp16B;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp166)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 665
({struct Cyc_Warn_String_Warn_Warg_struct _tmp16E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp633;_tmp633.tag=0U,({struct _fat_ptr _tmp725=({const char*_tmp16F="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp16F,sizeof(char),40U);});_tmp633.f1=_tmp725;});_tmp633;});void*_tmp16D[1U];_tmp16D[0]=& _tmp16E;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp16D,sizeof(void*),1U));});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp166)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 668
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp167=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp166)->f1).KnownAggr).val)->kind;_tmp168=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp166)->f1).KnownAggr).val)->tvs;_tmp169=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp166)->f1).KnownAggr).val)->impl;_tmp16A=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp166)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp167;struct Cyc_List_List*tvs=_tmp168;struct Cyc_Absyn_AggrdeclImpl*i=_tmp169;int expected_mem_kind=_tmp16A;
# 670
if(i == 0)
return expected_mem_kind?& Cyc_Tcutil_mk:& Cyc_Tcutil_ak;{
struct Cyc_List_List*_tmp170=i->fields;struct Cyc_List_List*fields=_tmp170;
if(fields == 0)return& Cyc_Tcutil_mk;
# 675
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){
;}{
void*_tmp171=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp171;
struct Cyc_Absyn_Kind*_tmp172=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp172;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 684
for(0;fields != 0;fields=fields->tl){
void*_tmp173=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp173;
struct Cyc_Absyn_Kind*_tmp174=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp174;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 689
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 691
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp161=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15E)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp161;
# 694
void*_tmp175=Cyc_Tcutil_compress((pinfo.ptr_atts).bounds);void*_stmttmp22=_tmp175;void*_tmp176=_stmttmp22;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp176)->f1)){case 13U: _LL4D: _LL4E: {
# 696
enum Cyc_Absyn_AliasQual _tmp177=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp23=_tmp177;enum Cyc_Absyn_AliasQual _tmp178=_stmttmp23;switch(_tmp178){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 702
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 704
enum Cyc_Absyn_AliasQual _tmp179=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp24=_tmp179;enum Cyc_Absyn_AliasQual _tmp17A=_stmttmp24;switch(_tmp17A){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 710
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 714
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp160=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15E)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp160;
# 716
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp15F=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15E)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp15F;
# 720
if(td == 0 || td->kind == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp17C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp635;_tmp635.tag=0U,({struct _fat_ptr _tmp726=({const char*_tmp17E="type_kind: typedef found: ";_tag_fat(_tmp17E,sizeof(char),27U);});_tmp635.f1=_tmp726;});_tmp635;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp17D=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp634;_tmp634.tag=2U,_tmp634.f1=(void*)t;_tmp634;});void*_tmp17B[2U];_tmp17B[0]=& _tmp17C,_tmp17B[1]=& _tmp17D;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp17B,sizeof(void*),2U));});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15E)->f1)->r)){case 1U: _LL17: _LL18:
 return& Cyc_Tcutil_bk;case 0U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 729
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 733
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 741
int Cyc_Tcutil_is_regparm0_att(void*a){
void*_tmp17F=a;if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp17F)->tag == 0U){if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp17F)->f1 == 0){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 748
int Cyc_Tcutil_equiv_fn_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
return 1;}
# 757
static void*Cyc_Tcutil_rgns_of(void*t);
# 759
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 763
static struct _tuple19*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp180=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp25=_tmp180;struct Cyc_Absyn_Kind*_tmp181=_stmttmp25;switch(((struct Cyc_Absyn_Kind*)_tmp181)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp181)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp182=_cycalloc(sizeof(*_tmp182));_tmp182->tag=9U,({struct Cyc_Absyn_Exp*_tmp727=Cyc_Absyn_uint_exp(0U,0U);_tmp182->f1=_tmp727;});_tmp182;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 774
return({struct _tuple19*_tmp183=_cycalloc(sizeof(*_tmp183));_tmp183->f1=tv,_tmp183->f2=t;_tmp183;});}
# 781
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp184=Cyc_Tcutil_compress(t);void*_stmttmp26=_tmp184;void*_tmp185=_stmttmp26;struct Cyc_List_List*_tmp186;struct Cyc_List_List*_tmp187;struct Cyc_List_List*_tmp18E;struct Cyc_Absyn_VarargInfo*_tmp18D;struct Cyc_List_List*_tmp18C;void*_tmp18B;struct Cyc_Absyn_Tqual _tmp18A;void*_tmp189;struct Cyc_List_List*_tmp188;struct Cyc_List_List*_tmp18F;void*_tmp190;void*_tmp192;void*_tmp191;struct Cyc_List_List*_tmp193;switch(*((int*)_tmp185)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp185)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp185)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp193=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp185)->f2;_LL6: {struct Cyc_List_List*ts=_tmp193;
# 786
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 790
struct Cyc_Absyn_Kind*_tmp194=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp27=_tmp194;struct Cyc_Absyn_Kind*_tmp195=_stmttmp27;switch(((struct Cyc_Absyn_Kind*)_tmp195)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp191=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp185)->f1).elt_type;_tmp192=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp185)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp191;void*r=_tmp192;
# 798
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp196[2U];({void*_tmp729=Cyc_Absyn_access_eff(r);_tmp196[0]=_tmp729;}),({void*_tmp728=Cyc_Tcutil_rgns_of(et);_tmp196[1]=_tmp728;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp196,sizeof(void*),2U));})));}case 4U: _LLD: _tmp190=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp185)->f1).elt_type;_LLE: {void*et=_tmp190;
# 800
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp18F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp185)->f2;_LL10: {struct Cyc_List_List*fs=_tmp18F;
# 802
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp188=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).tvars;_tmp189=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).effect;_tmp18A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).ret_tqual;_tmp18B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).ret_type;_tmp18C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).args;_tmp18D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).cyc_varargs;_tmp18E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp185)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp188;void*eff=_tmp189;struct Cyc_Absyn_Tqual rt_tq=_tmp18A;void*rt=_tmp18B;struct Cyc_List_List*args=_tmp18C;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp18D;struct Cyc_List_List*rpo=_tmp18E;
# 811
void*_tmp197=({struct Cyc_List_List*_tmp72A=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp72A,(void*)_check_null(eff));});void*e=_tmp197;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp187=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp185)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp187;
# 814
struct Cyc_List_List*_tmp198=0;struct Cyc_List_List*ts=_tmp198;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp199=_cycalloc(sizeof(*_tmp199));_tmp199->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp199->tl=ts;_tmp199;});}
_tmp186=ts;goto _LL16;}case 8U: _LL15: _tmp186=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp185)->f2;_LL16: {struct Cyc_List_List*ts=_tmp186;
# 819
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp19B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp636;_tmp636.tag=0U,({struct _fat_ptr _tmp72B=({const char*_tmp19C="typedecl in rgns_of";_tag_fat(_tmp19C,sizeof(char),20U);});_tmp636.f1=_tmp72B;});_tmp636;});void*_tmp19A[1U];_tmp19A[0]=& _tmp19B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp19A,sizeof(void*),1U));});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 829
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp19D=e;void*_tmp19E;struct Cyc_List_List**_tmp19F;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f1)){case 9U: _LL1: _tmp19F=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2;_LL2: {struct Cyc_List_List**es=_tmp19F;
# 833
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp1A0=(void*)effs->hd;void*eff=_tmp1A0;
({void*_tmp72C=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp72C;});{
void*_tmp1A1=(void*)effs->hd;void*_stmttmp28=_tmp1A1;void*_tmp1A2=_stmttmp28;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A2)->f2)->tl == 0){_LLE: _LLF:
# 842
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 846
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp1A3=Cyc_Tcutil_compress((void*)effs->hd);void*_stmttmp29=_tmp1A3;void*_tmp1A4=_stmttmp29;void*_tmp1A5;struct Cyc_List_List*_tmp1A6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f1)){case 9U: _LL13: _tmp1A6=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp1A6;
# 851
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);
goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A4)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp1A5=_tmp1A4;_LL1C: {void*e=_tmp1A5;
effects=({struct Cyc_List_List*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->hd=e,_tmp1A7->tl=effects;_tmp1A7;});goto _LL12;}}_LL12:;}}
# 859
({struct Cyc_List_List*_tmp72D=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp72D;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2 != 0){_LL3: _tmp19E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19D)->f2)->hd;_LL4: {void*t=_tmp19E;
# 862
void*_tmp1A8=Cyc_Tcutil_compress(t);void*_stmttmp2A=_tmp1A8;void*_tmp1A9=_stmttmp2A;switch(*((int*)_tmp1A9)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 867
 return e;}_LL0:;}}
# 872
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1AA=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1AB=_cycalloc(sizeof(*_tmp1AB));_tmp1AB->tag=5U,(_tmp1AB->f1).tvars=0,(_tmp1AB->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp72E=Cyc_Absyn_empty_tqual(0U);(_tmp1AB->f1).ret_tqual=_tmp72E;}),(_tmp1AB->f1).ret_type=Cyc_Absyn_void_type,(_tmp1AB->f1).args=0,(_tmp1AB->f1).c_varargs=0,(_tmp1AB->f1).cyc_varargs=0,(_tmp1AB->f1).rgn_po=0,(_tmp1AB->f1).attributes=0,(_tmp1AB->f1).requires_clause=0,(_tmp1AB->f1).requires_relns=0,(_tmp1AB->f1).ensures_clause=0,(_tmp1AB->f1).ensures_relns=0,(_tmp1AB->f1).return_value=0;_tmp1AB;});
# 873
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp1AA;
# 883
return({void*_tmp732=(void*)fntype;void*_tmp731=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp730=Cyc_Absyn_empty_tqual(0U);void*_tmp72F=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp732,_tmp731,_tmp730,_tmp72F,Cyc_Absyn_false_type);});}
# 890
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp1AC=Cyc_Tcutil_compress(e);void*_stmttmp2B=_tmp1AC;void*_tmp1AD=_stmttmp2B;struct Cyc_Core_Opt*_tmp1B0;void**_tmp1AF;struct Cyc_Core_Opt*_tmp1AE;void*_tmp1B1;struct Cyc_List_List*_tmp1B2;void*_tmp1B3;switch(*((int*)_tmp1AD)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AD)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AD)->f2 != 0){_LL1: _tmp1B3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AD)->f2)->hd;_LL2: {void*r2=_tmp1B3;
# 897
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Tcutil_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp1B4=({struct _tuple15 _tmp637;_tmp637.f1=r,_tmp637.f2=r2;_tmp637;});struct _tuple15 _stmttmp2C=_tmp1B4;struct _tuple15 _tmp1B5=_stmttmp2C;struct Cyc_Absyn_Tvar*_tmp1B7;struct Cyc_Absyn_Tvar*_tmp1B6;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B5.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B5.f2)->tag == 2U){_LLC: _tmp1B6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B5.f1)->f1;_tmp1B7=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B5.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1B6;struct Cyc_Absyn_Tvar*tv2=_tmp1B7;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1B2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AD)->f2;_LL4: {struct Cyc_List_List*es=_tmp1B2;
# 905
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AD)->f2 != 0){_LL5: _tmp1B1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1AD)->f2)->hd;_LL6: {void*t=_tmp1B1;
# 909
void*_tmp1B8=Cyc_Tcutil_rgns_of(t);void*_stmttmp2D=_tmp1B8;void*_tmp1B9=_stmttmp2D;void*_tmp1BA;void*_tmp1BB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->f2 != 0){_LL11: _tmp1BB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B9)->f2)->hd;_LL12: {void*t=_tmp1BB;
# 911
if(!constrain)return 0;{
void*_tmp1BC=Cyc_Tcutil_compress(t);void*_stmttmp2E=_tmp1BC;void*_tmp1BD=_stmttmp2E;struct Cyc_Core_Opt*_tmp1C0;void**_tmp1BF;struct Cyc_Core_Opt*_tmp1BE;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->tag == 1U){_LL16: _tmp1BE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->f1;_tmp1BF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->f2;_tmp1C0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1BD)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1BE;void**p=_tmp1BF;struct Cyc_Core_Opt*s=_tmp1C0;
# 916
void*_tmp1C1=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1C1;
# 919
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp734=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1C2[2U];_tmp1C2[0]=ev,({void*_tmp733=Cyc_Absyn_access_eff(r);_tmp1C2[1]=_tmp733;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C2,sizeof(void*),2U));})));*p=_tmp734;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1BA=_tmp1B9;_LL14: {void*e2=_tmp1BA;
# 924
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1AE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AD)->f1;_tmp1AF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AD)->f2;_tmp1B0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1AD)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1AE;void**p=_tmp1AF;struct Cyc_Core_Opt*s=_tmp1B0;
# 927
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1C4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp638;_tmp638.tag=0U,({struct _fat_ptr _tmp735=({const char*_tmp1C5="effect evar has wrong kind";_tag_fat(_tmp1C5,sizeof(char),27U);});_tmp638.f1=_tmp735;});_tmp638;});void*_tmp1C3[1U];_tmp1C3[0]=& _tmp1C4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1C3,sizeof(void*),1U));});
if(!constrain)return 0;{
# 932
void*_tmp1C6=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1C6;
# 935
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp737=Cyc_Absyn_join_eff(({void*_tmp1C7[2U];_tmp1C7[0]=ev,({void*_tmp736=Cyc_Absyn_access_eff(r);_tmp1C7[1]=_tmp736;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C7,sizeof(void*),2U));}));*p=_tmp737;});
return 1;}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 945
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1C8=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_stmttmp2F=_tmp1C8;void*_tmp1C9=_stmttmp2F;struct Cyc_Core_Opt*_tmp1CC;void**_tmp1CB;struct Cyc_Core_Opt*_tmp1CA;void*_tmp1CD;struct Cyc_List_List*_tmp1CE;switch(*((int*)_tmp1C9)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1CE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f2;_LL4: {struct Cyc_List_List*es=_tmp1CE;
# 950
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f2 != 0){_LL5: _tmp1CD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C9)->f2)->hd;_LL6: {void*t2=_tmp1CD;
# 955
t2=Cyc_Tcutil_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1CF=Cyc_Tcutil_rgns_of(t);void*_stmttmp30=_tmp1CF;void*_tmp1D0=_stmttmp30;void*_tmp1D1;void*_tmp1D2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0)->f2 != 0){_LLC: _tmp1D2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D0)->f2)->hd;_LLD: {void*t3=_tmp1D2;
# 960
struct _tuple15 _tmp1D3=({struct _tuple15 _tmp639;({void*_tmp738=Cyc_Tcutil_compress(t3);_tmp639.f1=_tmp738;}),_tmp639.f2=t2;_tmp639;});struct _tuple15 _stmttmp31=_tmp1D3;struct _tuple15 _tmp1D4=_stmttmp31;struct Cyc_Absyn_Tvar*_tmp1D6;struct Cyc_Absyn_Tvar*_tmp1D5;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D4.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D4.f2)->tag == 2U){_LL11: _tmp1D5=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D4.f1)->f1;_tmp1D6=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D4.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1D5;struct Cyc_Absyn_Tvar*tv2=_tmp1D6;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1D1=_tmp1D0;_LLF: {void*e2=_tmp1D1;
# 964
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1CA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C9)->f1;_tmp1CB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C9)->f2;_tmp1CC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C9)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1CA;void**p=_tmp1CB;struct Cyc_Core_Opt*s=_tmp1CC;
# 967
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1D8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A;_tmp63A.tag=0U,({struct _fat_ptr _tmp739=({const char*_tmp1D9="effect evar has wrong kind";_tag_fat(_tmp1D9,sizeof(char),27U);});_tmp63A.f1=_tmp739;});_tmp63A;});void*_tmp1D7[1U];_tmp1D7[0]=& _tmp1D8;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1D7,sizeof(void*),1U));});
if(!may_constrain_evars)return 0;{
# 972
void*_tmp1DA=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1DA;
# 975
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1DB=Cyc_Absyn_join_eff(({void*_tmp1DC[2U];_tmp1DC[0]=ev,({void*_tmp73A=Cyc_Absyn_regionsof_eff(t);_tmp1DC[1]=_tmp73A;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1DC,sizeof(void*),2U));}));void*new_typ=_tmp1DB;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 985
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1DD=e;struct Cyc_Core_Opt*_tmp1E0;void**_tmp1DF;struct Cyc_Core_Opt*_tmp1DE;void*_tmp1E1;struct Cyc_List_List*_tmp1E2;struct Cyc_Absyn_Tvar*_tmp1E3;switch(*((int*)_tmp1DD)){case 2U: _LL1: _tmp1E3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1DD)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1E3;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DD)->f1)){case 9U: _LL3: _tmp1E2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DD)->f2;_LL4: {struct Cyc_List_List*es=_tmp1E2;
# 990
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DD)->f2 != 0){_LL5: _tmp1E1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1DD)->f2)->hd;_LL6: {void*t=_tmp1E1;
# 995
void*_tmp1E4=Cyc_Tcutil_rgns_of(t);void*_stmttmp32=_tmp1E4;void*_tmp1E5=_stmttmp32;void*_tmp1E6;void*_tmp1E7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E5)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E5)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E5)->f2 != 0){_LLC: _tmp1E7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E5)->f2)->hd;_LLD: {void*t2=_tmp1E7;
# 997
if(!may_constrain_evars)return 0;{
void*_tmp1E8=Cyc_Tcutil_compress(t2);void*_stmttmp33=_tmp1E8;void*_tmp1E9=_stmttmp33;struct Cyc_Core_Opt*_tmp1EC;void**_tmp1EB;struct Cyc_Core_Opt*_tmp1EA;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E9)->tag == 1U){_LL11: _tmp1EA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E9)->f1;_tmp1EB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E9)->f2;_tmp1EC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E9)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1EA;void**p=_tmp1EB;struct Cyc_Core_Opt*s=_tmp1EC;
# 1003
void*_tmp1ED=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1ED;
# 1005
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp73C=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1EE[2U];_tmp1EE[0]=ev,({void*_tmp73B=Cyc_Absyn_var_type(v);_tmp1EE[1]=_tmp73B;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1EE,sizeof(void*),2U));})));*p=_tmp73C;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1E6=_tmp1E5;_LLF: {void*e2=_tmp1E6;
# 1010
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1DE=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DD)->f1;_tmp1DF=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DD)->f2;_tmp1E0=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DD)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1DE;void**p=_tmp1DF;struct Cyc_Core_Opt*s=_tmp1E0;
# 1013
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp1F0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63B;_tmp63B.tag=0U,({struct _fat_ptr _tmp73D=({const char*_tmp1F1="effect evar has wrong kind";_tag_fat(_tmp1F1,sizeof(char),27U);});_tmp63B.f1=_tmp73D;});_tmp63B;});void*_tmp1EF[1U];_tmp1EF[0]=& _tmp1F0;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp1EF,sizeof(void*),1U));});{
# 1017
void*_tmp1F2=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1F2;
# 1019
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1F3=Cyc_Absyn_join_eff(({void*_tmp1F4[2U];_tmp1F4[0]=ev,({void*_tmp73E=Cyc_Absyn_var_type(v);_tmp1F4[1]=_tmp73E;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1F4,sizeof(void*),2U));}));void*new_typ=_tmp1F3;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1029
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1F5=e;void*_tmp1F6;struct Cyc_List_List*_tmp1F7;switch(*((int*)_tmp1F5)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f1)){case 9U: _LL1: _tmp1F7=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2;_LL2: {struct Cyc_List_List*es=_tmp1F7;
# 1033
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2 != 0){_LL3: _tmp1F6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F5)->f2)->hd;_LL4: {void*t=_tmp1F6;
# 1038
void*_tmp1F8=Cyc_Tcutil_rgns_of(t);void*_stmttmp34=_tmp1F8;void*_tmp1F9=_stmttmp34;void*_tmp1FA;void*_tmp1FB;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F9)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F9)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F9)->f2 != 0){_LLA: _tmp1FB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F9)->f2)->hd;_LLB: {void*t2=_tmp1FB;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1FA=_tmp1F9;_LLD: {void*e2=_tmp1FA;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1042
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 1056 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1059
void*_tmp1FC=Cyc_Tcutil_compress(e1);void*_stmttmp35=_tmp1FC;void*_tmp1FD=_stmttmp35;struct Cyc_Core_Opt*_tmp1FF;void**_tmp1FE;struct Cyc_Absyn_Tvar*_tmp200;void*_tmp201;void*_tmp202;struct Cyc_List_List*_tmp203;switch(*((int*)_tmp1FD)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f1)){case 9U: _LL1: _tmp203=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f2;_LL2: {struct Cyc_List_List*es=_tmp203;
# 1061
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f2 != 0){_LL3: _tmp202=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f2)->hd;_LL4: {void*r=_tmp202;
# 1072
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f2 != 0){_LL7: _tmp201=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FD)->f2)->hd;_LL8: {void*t=_tmp201;
# 1076
void*_tmp204=Cyc_Tcutil_rgns_of(t);void*_stmttmp36=_tmp204;void*_tmp205=_stmttmp36;void*_tmp206;void*_tmp207;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f2 != 0){_LLE: _tmp207=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f2)->hd;_LLF: {void*t2=_tmp207;
# 1081
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp206=_tmp205;_LL11: {void*e=_tmp206;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp200=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1FD)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp200;
# 1074
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1FE=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1FD)->f2;_tmp1FF=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1FD)->f4;_LLA: {void**p=_tmp1FE;struct Cyc_Core_Opt*s=_tmp1FF;
# 1086
if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1090
*p=Cyc_Absyn_empty_effect;
# 1093
return 1;}else{
# 1095
return 0;}}default: _LLB: _LLC:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp209=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D;_tmp63D.tag=0U,({struct _fat_ptr _tmp73F=({const char*_tmp20B="subset_effect: bad effect: ";_tag_fat(_tmp20B,sizeof(char),28U);});_tmp63D.f1=_tmp73F;});_tmp63D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp20A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63C;_tmp63C.tag=2U,_tmp63C.f1=(void*)e1;_tmp63C;});void*_tmp208[2U];_tmp208[0]=& _tmp209,_tmp208[1]=& _tmp20A;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp208,sizeof(void*),2U));});}_LL0:;}
# 1106
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1108
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp20C=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp37=_tmp20C;struct _tuple15*_tmp20D=_stmttmp37;void*_tmp20F;void*_tmp20E;_LL1: _tmp20E=_tmp20D->f1;_tmp20F=_tmp20D->f2;_LL2: {void*t1a=_tmp20E;void*t1b=_tmp20F;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp210=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp38=_tmp210;struct _tuple15*_tmp211=_stmttmp38;void*_tmp213;void*_tmp212;_LL4: _tmp212=_tmp211->f1;_tmp213=_tmp211->f2;_LL5: {void*t2a=_tmp212;void*t2b=_tmp213;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1118
if(!found)return 0;}}}
# 1120
return 1;}
# 1124
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1126
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1129
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp214=t;switch(*((int*)_tmp214)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp214)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp214)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1135
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1138
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1141
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1144
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp214)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1133
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1136
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1139
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1142
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1145
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp214)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1134
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1137
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1140
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1143
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1146
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1168
({struct Cyc_Warn_String_Warn_Warg_struct _tmp216=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63E;_tmp63E.tag=0U,({struct _fat_ptr _tmp740=({const char*_tmp217="bad con";_tag_fat(_tmp217,sizeof(char),8U);});_tmp63E.f1=_tmp740;});_tmp63E;});void*_tmp215[1U];_tmp215[0]=& _tmp216;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp215,sizeof(void*),1U));});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp214)->f1){case 0U: _LL21: _LL22:
# 1147
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
# 1171
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp218=t;void*_tmp219;switch(*((int*)_tmp218)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp219=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->f1;_LL18: {void*c=_tmp219;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1187
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp21A=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp21A;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1192
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp21B=i;struct _tuple1*_tmp21C;struct Cyc_Absyn_Datatypedecl*_tmp21D;if((_tmp21B.KnownDatatype).tag == 2){_LL1: _tmp21D=*(_tmp21B.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp21D;
return dd->name;}}else{_LL3: _tmp21C=((_tmp21B.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp21C;
return n;}}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1198
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp21E=i;struct _tuple1*_tmp220;struct _tuple1*_tmp21F;struct Cyc_Absyn_Datatypefield*_tmp222;struct Cyc_Absyn_Datatypedecl*_tmp221;if((_tmp21E.KnownDatatypefield).tag == 2){_LL1: _tmp221=((_tmp21E.KnownDatatypefield).val).f1;_tmp222=((_tmp21E.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp221;struct Cyc_Absyn_Datatypefield*df=_tmp222;
# 1201
return({struct _tuple20 _tmp63F;_tmp63F.f1=dd->name,_tmp63F.f2=df->name;_tmp63F;});}}else{_LL3: _tmp21F=((_tmp21E.UnknownDatatypefield).val).datatype_name;_tmp220=((_tmp21E.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp21F;struct _tuple1*f=_tmp220;
# 1203
return({struct _tuple20 _tmp640;_tmp640.f1=d,_tmp640.f2=f;_tmp640;});}}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1206
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp223=i;struct Cyc_Absyn_Aggrdecl*_tmp224;struct _tuple1*_tmp226;enum Cyc_Absyn_AggrKind _tmp225;if((_tmp223.UnknownAggr).tag == 1){_LL1: _tmp225=((_tmp223.UnknownAggr).val).f1;_tmp226=((_tmp223.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp225;struct _tuple1*n=_tmp226;
return({struct _tuple21 _tmp641;_tmp641.f1=k,_tmp641.f2=n;_tmp641;});}}else{_LL3: _tmp224=*(_tmp223.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp224;
return({struct _tuple21 _tmp642;_tmp642.f1=ad->kind,_tmp642.f2=ad->name;_tmp642;});}}_LL0:;}
# 1212
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1218
struct _tuple15 _tmp227=({struct _tuple15 _tmp643;_tmp643.f1=t1,_tmp643.f2=t2;_tmp643;});struct _tuple15 _stmttmp39=_tmp227;struct _tuple15 _tmp228=_stmttmp39;union Cyc_Absyn_AggrInfo _tmp22A;union Cyc_Absyn_AggrInfo _tmp229;union Cyc_Absyn_DatatypeFieldInfo _tmp22C;union Cyc_Absyn_DatatypeFieldInfo _tmp22B;union Cyc_Absyn_DatatypeInfo _tmp22E;union Cyc_Absyn_DatatypeInfo _tmp22D;struct Cyc_List_List*_tmp230;struct Cyc_List_List*_tmp22F;struct _fat_ptr _tmp232;struct _fat_ptr _tmp231;struct _tuple1*_tmp234;struct _tuple1*_tmp233;switch(*((int*)_tmp228.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp228.f2)->tag == 15U){_LL1: _tmp233=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp228.f1)->f1;_tmp234=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp228.f2)->f1;_LL2: {struct _tuple1*n1=_tmp233;struct _tuple1*n2=_tmp234;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp228.f2)->tag == 17U){_LL3: _tmp231=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp228.f1)->f1;_tmp232=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp228.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp231;struct _fat_ptr s2=_tmp232;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp228.f2)->tag == 16U){_LL5: _tmp22F=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp228.f1)->f1;_tmp230=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp228.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp22F;struct Cyc_List_List*fs2=_tmp230;
# 1222
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp228.f2)->tag == 18U){_LL7: _tmp22D=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp228.f1)->f1;_tmp22E=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp228.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp22D;union Cyc_Absyn_DatatypeInfo info2=_tmp22E;
# 1224
return({struct _tuple1*_tmp741=Cyc_Tcutil_get_datatype_qvar(info1);Cyc_Absyn_qvar_cmp(_tmp741,Cyc_Tcutil_get_datatype_qvar(info2));});}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp228.f2)->tag == 19U){_LL9: _tmp22B=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp228.f1)->f1;_tmp22C=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp228.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp22B;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp22C;
# 1226
struct _tuple20 _tmp235=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple20 _stmttmp3A=_tmp235;struct _tuple20 _tmp236=_stmttmp3A;struct _tuple1*_tmp238;struct _tuple1*_tmp237;_LL10: _tmp237=_tmp236.f1;_tmp238=_tmp236.f2;_LL11: {struct _tuple1*d1=_tmp237;struct _tuple1*f1=_tmp238;
struct _tuple20 _tmp239=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple20 _stmttmp3B=_tmp239;struct _tuple20 _tmp23A=_stmttmp3B;struct _tuple1*_tmp23C;struct _tuple1*_tmp23B;_LL13: _tmp23B=_tmp23A.f1;_tmp23C=_tmp23A.f2;_LL14: {struct _tuple1*d2=_tmp23B;struct _tuple1*f2=_tmp23C;
int _tmp23D=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp23D;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp228.f2)->tag == 20U){_LLB: _tmp229=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp228.f1)->f1;_tmp22A=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp228.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp229;union Cyc_Absyn_AggrInfo info2=_tmp22A;
# 1232
struct _tuple21 _tmp23E=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple21 _stmttmp3C=_tmp23E;struct _tuple21 _tmp23F=_stmttmp3C;struct _tuple1*_tmp241;enum Cyc_Absyn_AggrKind _tmp240;_LL16: _tmp240=_tmp23F.f1;_tmp241=_tmp23F.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp240;struct _tuple1*q1=_tmp241;
struct _tuple21 _tmp242=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple21 _stmttmp3D=_tmp242;struct _tuple21 _tmp243=_stmttmp3D;struct _tuple1*_tmp245;enum Cyc_Absyn_AggrKind _tmp244;_LL19: _tmp244=_tmp243.f1;_tmp245=_tmp243.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp244;struct _tuple1*q2=_tmp245;
int _tmp246=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp246;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1241
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp743)(void*,void*)=cmp;void*_tmp742=(void*)_check_null(a1);_tmp743(_tmp742,(void*)_check_null(a2));});}
# 1247
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp247=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp247;
int _tmp248=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp248;
return Cyc_Core_intcmp(i1,i2);}
# 1252
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp249=tqt1;void*_tmp24B;struct Cyc_Absyn_Tqual _tmp24A;_LL1: _tmp24A=_tmp249->f1;_tmp24B=_tmp249->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp24A;void*t1=_tmp24B;
struct _tuple12*_tmp24C=tqt2;void*_tmp24E;struct Cyc_Absyn_Tqual _tmp24D;_LL4: _tmp24D=_tmp24C->f1;_tmp24E=_tmp24C->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp24D;void*t2=_tmp24E;
int _tmp24F=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp24F;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1260
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp250=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp250;
if(zsc != 0)return zsc;{
int _tmp251=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp251;
if(tqc != 0)return tqc;{
int _tmp252=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp252;
if(tc != 0)return tc;{
int _tmp253=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp253;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1277
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp254=({int _tmp744=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp744,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp254;
if(shallowcmp != 0)
return shallowcmp;{
# 1286
struct _tuple15 _tmp255=({struct _tuple15 _tmp648;_tmp648.f1=t2,_tmp648.f2=t1;_tmp648;});struct _tuple15 _stmttmp3E=_tmp255;struct _tuple15 _tmp256=_stmttmp3E;struct Cyc_Absyn_Exp*_tmp258;struct Cyc_Absyn_Exp*_tmp257;struct Cyc_Absyn_Exp*_tmp25A;struct Cyc_Absyn_Exp*_tmp259;struct Cyc_List_List*_tmp25E;enum Cyc_Absyn_AggrKind _tmp25D;struct Cyc_List_List*_tmp25C;enum Cyc_Absyn_AggrKind _tmp25B;struct Cyc_List_List*_tmp260;struct Cyc_List_List*_tmp25F;struct Cyc_Absyn_FnInfo _tmp262;struct Cyc_Absyn_FnInfo _tmp261;void*_tmp26A;struct Cyc_Absyn_Exp*_tmp269;struct Cyc_Absyn_Tqual _tmp268;void*_tmp267;void*_tmp266;struct Cyc_Absyn_Exp*_tmp265;struct Cyc_Absyn_Tqual _tmp264;void*_tmp263;void*_tmp276;void*_tmp275;void*_tmp274;void*_tmp273;struct Cyc_Absyn_Tqual _tmp272;void*_tmp271;void*_tmp270;void*_tmp26F;void*_tmp26E;void*_tmp26D;struct Cyc_Absyn_Tqual _tmp26C;void*_tmp26B;struct Cyc_Absyn_Tvar*_tmp278;struct Cyc_Absyn_Tvar*_tmp277;struct Cyc_List_List*_tmp27C;void*_tmp27B;struct Cyc_List_List*_tmp27A;void*_tmp279;switch(*((int*)_tmp256.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp256.f2)->tag == 0U){_LL1: _tmp279=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp256.f1)->f1;_tmp27A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp256.f1)->f2;_tmp27B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp256.f2)->f1;_tmp27C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp256.f2)->f2;_LL2: {void*c1=_tmp279;struct Cyc_List_List*ts1=_tmp27A;void*c2=_tmp27B;struct Cyc_List_List*ts2=_tmp27C;
# 1288
int _tmp27D=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp27D;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp256.f2)->tag == 1U){_LL3: _LL4:
# 1292
({struct Cyc_Warn_String_Warn_Warg_struct _tmp27F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0U,({struct _fat_ptr _tmp745=({const char*_tmp280="typecmp: can only compare closed types";_tag_fat(_tmp280,sizeof(char),39U);});_tmp644.f1=_tmp745;});_tmp644;});void*_tmp27E[1U];_tmp27E[0]=& _tmp27F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp27E,sizeof(void*),1U));});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp256.f2)->tag == 2U){_LL5: _tmp277=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp256.f1)->f1;_tmp278=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp256.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp277;struct Cyc_Absyn_Tvar*tv1=_tmp278;
# 1296
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f2)->tag == 3U){_LL7: _tmp26B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f1)->f1).elt_type;_tmp26C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f1)->f1).elt_tq;_tmp26D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f1)->f1).ptr_atts).rgn;_tmp26E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f1)->f1).ptr_atts).nullable;_tmp26F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f1)->f1).ptr_atts).bounds;_tmp270=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f1)->f1).ptr_atts).zero_term;_tmp271=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f2)->f1).elt_type;_tmp272=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f2)->f1).elt_tq;_tmp273=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f2)->f1).ptr_atts).rgn;_tmp274=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f2)->f1).ptr_atts).nullable;_tmp275=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f2)->f1).ptr_atts).bounds;_tmp276=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp256.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp26B;struct Cyc_Absyn_Tqual tqual2a=_tmp26C;void*rgn2=_tmp26D;void*null2a=_tmp26E;void*b2=_tmp26F;void*zt2=_tmp270;void*t1a=_tmp271;struct Cyc_Absyn_Tqual tqual1a=_tmp272;void*rgn1=_tmp273;void*null1a=_tmp274;void*b1=_tmp275;void*zt1=_tmp276;
# 1300
int _tmp281=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp281;
if(etc != 0)return etc;{
int _tmp282=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp282;
if(rc != 0)return rc;{
int _tmp283=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp283;
if(tqc != 0)return tqc;{
int _tmp284=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp284;
if(cc != 0)return cc;{
int _tmp285=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp285;
if(zc != 0)return zc;{
int _tmp286=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp286;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f2)->tag == 4U){_LL9: _tmp263=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f1)->f1).elt_type;_tmp264=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f1)->f1).tq;_tmp265=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f1)->f1).num_elts;_tmp266=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f1)->f1).zero_term;_tmp267=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f2)->f1).elt_type;_tmp268=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f2)->f1).tq;_tmp269=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f2)->f1).num_elts;_tmp26A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp256.f2)->f1).zero_term;_LLA: {void*t2a=_tmp263;struct Cyc_Absyn_Tqual tq2a=_tmp264;struct Cyc_Absyn_Exp*e1=_tmp265;void*zt1=_tmp266;void*t1a=_tmp267;struct Cyc_Absyn_Tqual tq1a=_tmp268;struct Cyc_Absyn_Exp*e2=_tmp269;void*zt2=_tmp26A;
# 1316
int _tmp287=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp287;
if(tqc != 0)return tqc;{
int _tmp288=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp288;
if(tc != 0)return tc;{
int _tmp289=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp289;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp28B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp645;_tmp645.tag=0U,({struct _fat_ptr _tmp746=({const char*_tmp28C="missing expression in array index";_tag_fat(_tmp28C,sizeof(char),34U);});_tmp645.f1=_tmp746;});_tmp645;});void*_tmp28A[1U];_tmp28A[0]=& _tmp28B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp28A,sizeof(void*),1U));});
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp256.f2)->tag == 5U){_LLB: _tmp261=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp256.f1)->f1;_tmp262=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp256.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp261;struct Cyc_Absyn_FnInfo f2=_tmp262;
# 1328
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp28D=f1.args;struct Cyc_List_List*args1=_tmp28D;
struct Cyc_List_List*_tmp28E=f2.args;struct Cyc_List_List*args2=_tmp28E;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp28F=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3F=_tmp28F;struct _tuple9 _tmp290=_stmttmp3F;void*_tmp292;struct Cyc_Absyn_Tqual _tmp291;_LL18: _tmp291=_tmp290.f2;_tmp292=_tmp290.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp291;void*t1=_tmp292;
struct _tuple9 _tmp293=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp40=_tmp293;struct _tuple9 _tmp294=_stmttmp40;void*_tmp296;struct Cyc_Absyn_Tqual _tmp295;_LL1B: _tmp295=_tmp294.f2;_tmp296=_tmp294.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp295;void*t2=_tmp296;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1343
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp747=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp747,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1357
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp297=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp297;
struct Cyc_List_List*_tmp298=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp298;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp299=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp41=_tmp299;struct _tuple15 _tmp29A=_stmttmp41;void*_tmp29C;void*_tmp29B;_LL1E: _tmp29B=_tmp29A.f1;_tmp29C=_tmp29A.f2;_LL1F: {void*t1a=_tmp29B;void*t1b=_tmp29C;
struct _tuple15 _tmp29D=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp42=_tmp29D;struct _tuple15 _tmp29E=_stmttmp42;void*_tmp2A0;void*_tmp29F;_LL21: _tmp29F=_tmp29E.f1;_tmp2A0=_tmp29E.f2;_LL22: {void*t2a=_tmp29F;void*t2b=_tmp2A0;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1367
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1375
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp646;_tmp646.tag=0U,({struct _fat_ptr _tmp748=({const char*_tmp2A3="typecmp: function type comparison should never get here!";_tag_fat(_tmp2A3,sizeof(char),57U);});_tmp646.f1=_tmp748;});_tmp646;});void*_tmp2A1[1U];_tmp2A1[0]=& _tmp2A2;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2A1,sizeof(void*),1U));});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp256.f2)->tag == 6U){_LLD: _tmp25F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp256.f1)->f1;_tmp260=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp256.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp25F;struct Cyc_List_List*ts1=_tmp260;
# 1378
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp256.f2)->tag == 7U){_LLF: _tmp25B=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp256.f1)->f1;_tmp25C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp256.f1)->f2;_tmp25D=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp256.f2)->f1;_tmp25E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp256.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp25B;struct Cyc_List_List*fs2=_tmp25C;enum Cyc_Absyn_AggrKind k1=_tmp25D;struct Cyc_List_List*fs1=_tmp25E;
# 1381
if((int)k1 != (int)k2)
return(int)k1 == (int)0U?- 1: 1;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp256.f2)->tag == 9U){_LL11: _tmp259=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp256.f1)->f1;_tmp25A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp256.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp259;struct Cyc_Absyn_Exp*e2=_tmp25A;
# 1385
_tmp257=e1;_tmp258=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp256.f2)->tag == 11U){_LL13: _tmp257=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp256.f1)->f1;_tmp258=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp256.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp257;struct Cyc_Absyn_Exp*e2=_tmp258;
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2A5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp647;_tmp647.tag=0U,({struct _fat_ptr _tmp749=({const char*_tmp2A6="Unmatched case in typecmp";_tag_fat(_tmp2A6,sizeof(char),26U);});_tmp647.f1=_tmp749;});_tmp647;});void*_tmp2A4[1U];_tmp2A4[0]=& _tmp2A5;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2A4,sizeof(void*),1U));});}_LL0:;}}}
# 1393
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp2A7=({struct _tuple15 _tmp64A;({void*_tmp74B=Cyc_Tcutil_compress(t1);_tmp64A.f1=_tmp74B;}),({void*_tmp74A=Cyc_Tcutil_compress(t2);_tmp64A.f2=_tmp74A;});_tmp64A;});struct _tuple15 _stmttmp43=_tmp2A7;struct _tuple15 _tmp2A8=_stmttmp43;void*_tmp2AA;void*_tmp2A9;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A8.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A8.f2)->tag == 0U){_LL1: _tmp2A9=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A8.f1)->f1;_tmp2AA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A8.f2)->f1;_LL2: {void*c1=_tmp2A9;void*c2=_tmp2AA;
# 1396
struct _tuple15 _tmp2AB=({struct _tuple15 _tmp649;_tmp649.f1=c1,_tmp649.f2=c2;_tmp649;});struct _tuple15 _stmttmp44=_tmp2AB;struct _tuple15 _tmp2AC=_stmttmp44;int _tmp2AE;int _tmp2AD;switch(*((int*)_tmp2AC.f1)){case 2U: switch(*((int*)_tmp2AC.f2)){case 2U: _LL6: _tmp2AD=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AC.f1)->f1;_tmp2AE=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AC.f2)->f1;_LL7: {int i1=_tmp2AD;int i2=_tmp2AE;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp2AC.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1404
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1409
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1412
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AC.f2)->f1 == 0){_LL14: _LL15:
# 1407
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp2AC.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1405
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1410
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1413
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2AC.f2)->f1 == 0){_LL16: _LL17:
# 1408
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1414
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2AC.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1411
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1415
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1417
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1419
 return 0;}_LL0:;}
# 1423
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp2AF=({struct _tuple15 _tmp64C;({void*_tmp74D=Cyc_Tcutil_compress(t1);_tmp64C.f1=_tmp74D;}),({void*_tmp74C=Cyc_Tcutil_compress(t2);_tmp64C.f2=_tmp74C;});_tmp64C;});struct _tuple15 _stmttmp45=_tmp2AF;struct _tuple15 _tmp2B0=_stmttmp45;void*_tmp2B2;void*_tmp2B1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0.f2)->tag == 0U){_LL1: _tmp2B1=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0.f1)->f1;_tmp2B2=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2B0.f2)->f1;_LL2: {void*c1=_tmp2B1;void*c2=_tmp2B2;
# 1426
{struct _tuple15 _tmp2B3=({struct _tuple15 _tmp64B;_tmp64B.f1=c1,_tmp64B.f2=c2;_tmp64B;});struct _tuple15 _stmttmp46=_tmp2B3;struct _tuple15 _tmp2B4=_stmttmp46;int _tmp2B6;int _tmp2B5;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2B4.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2B4.f2)->tag == 2U){_LL6: _tmp2B5=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f1;_tmp2B6=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f1;_LL7: {int i1=_tmp2B5;int i2=_tmp2B6;
# 1428
if(i1 != 0 && i1 != 1)return t1;
if(i2 != 0 && i2 != 1)return t2;
if(i1 >= i2)return t1;
return t2;}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2B4.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1438
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2B4.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1443
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1436
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2B4.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1445
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1435
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1439
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2B4.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1444
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1437
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2B4.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2B4.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1446
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2B4.f1)->tag == 4U){_LL18: _LL19:
# 1441
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2B4.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1447
 goto _LL5;}}}}}}_LL5:;}
# 1449
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1452
return Cyc_Absyn_sint_type;}
# 1457
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1460
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 || Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2B7=_cycalloc(sizeof(*_tmp2B7));_tmp2B7->v=t1;_tmp2B7;});}}}
# 1468
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1472
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2B9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp650;_tmp650.tag=0U,({struct _fat_ptr _tmp74E=({const char*_tmp2BE="type mismatch: expecting ";_tag_fat(_tmp2BE,sizeof(char),26U);});_tmp650.f1=_tmp74E;});_tmp650;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64F;_tmp64F.tag=2U,_tmp64F.f1=(void*)t;_tmp64F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2BB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64E;_tmp64E.tag=0U,({struct _fat_ptr _tmp74F=({const char*_tmp2BD=" but found ";_tag_fat(_tmp2BD,sizeof(char),12U);});_tmp64E.f1=_tmp74F;});_tmp64E;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2BC=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64D;_tmp64D.tag=2U,_tmp64D.f1=(void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt);_tmp64D;});void*_tmp2B8[4U];_tmp2B8[0]=& _tmp2B9,_tmp2B8[1]=& _tmp2BA,_tmp2B8[2]=& _tmp2BB,_tmp2B8[3]=& _tmp2BC;({unsigned _tmp750=((struct Cyc_Absyn_Exp*)el->hd)->loc;Cyc_Warn_err2(_tmp750,_tag_fat(_tmp2B8,sizeof(void*),4U));});});
# 1476
return 0;}}}
# 1478
return 1;}
# 1483
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2BF=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp47=_tmp2BF;void*_tmp2C0=_stmttmp47;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C0)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1489
return 1;}
# 1492
static int Cyc_Tcutil_coerce_Xint_type(void*Xint_type,struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Xint_type))
return 1;
# 1496
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Xint_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp651;_tmp651.tag=0U,({struct _fat_ptr _tmp751=({const char*_tmp2C3="integral size mismatch; conversion supplied";_tag_fat(_tmp2C3,sizeof(char),44U);});_tmp651.f1=_tmp751;});_tmp651;});void*_tmp2C1[1U];_tmp2C1[0]=& _tmp2C2;({unsigned _tmp752=e->loc;Cyc_Warn_warn2(_tmp752,_tag_fat(_tmp2C1,sizeof(void*),1U));});});
Cyc_Tcutil_unchecked_cast(e,Xint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1502
return 0;}
# 1505
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_uint_type,e);}
# 1509
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_coerce_Xint_type(Cyc_Absyn_sint_type,e);}
# 1515
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1521
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp753=t;Cyc_Unify_unify(_tmp753,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1526
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2C4=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2C4;
void*_tmp2C5=t;struct Cyc_Absyn_Exp*_tmp2C6;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2C5)->tag == 9U){_LL1: _tmp2C6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2C5)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2C6;
return e;}}else{_LL3: _LL4: {
# 1531
struct Cyc_Absyn_Exp*_tmp2C7=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2C7;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1540
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2C8=Cyc_Tcutil_compress(b);void*_stmttmp48=_tmp2C8;void*_tmp2C9=_stmttmp48;struct Cyc_List_List*_tmp2CA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2CA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2CA;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2CC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp653;_tmp653.tag=0U,({struct _fat_ptr _tmp754=({const char*_tmp2CE="get_bounds_exp: ";_tag_fat(_tmp2CE,sizeof(char),17U);});_tmp653.f1=_tmp754;});_tmp653;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2CD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp652;_tmp652.tag=2U,_tmp652.f1=(void*)b;_tmp652;});void*_tmp2CB[2U];_tmp2CB[0]=& _tmp2CC,_tmp2CB[1]=& _tmp2CD;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2CB,sizeof(void*),2U));});}_LL0:;}}
# 1549
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2CF=Cyc_Tcutil_compress(t);void*_stmttmp49=_tmp2CF;void*_tmp2D0=_stmttmp49;void*_tmp2D1;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D0)->tag == 3U){_LL1: _tmp2D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D0)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2D1;
# 1552
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D3=({struct Cyc_Warn_String_Warn_Warg_struct _tmp655;_tmp655.tag=0U,({struct _fat_ptr _tmp755=({const char*_tmp2D5="get_ptr_bounds_exp not pointer: ";_tag_fat(_tmp2D5,sizeof(char),33U);});_tmp655.f1=_tmp755;});_tmp655;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2D4=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp654;_tmp654.tag=2U,_tmp654.f1=(void*)t;_tmp654;});void*_tmp2D2[2U];_tmp2D2[0]=& _tmp2D3,_tmp2D2[1]=& _tmp2D4;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2D2,sizeof(void*),2U));});}_LL0:;}
# 1558
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2D6=_cycalloc(sizeof(*_tmp2D6));_tmp2D6->v=tvs;_tmp2D6;}));}
# 1562
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->v=tvs;_tmp2D7;}));}
# 1567
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*,struct Cyc_List_List*,void*,void*);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1574
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp2D8=({struct _tuple15 _tmp65C;_tmp65C.f1=t1,_tmp65C.f2=t2;_tmp65C;});struct _tuple15 _stmttmp4A=_tmp2D8;struct _tuple15 _tmp2D9=_stmttmp4A;void*_tmp2E1;struct Cyc_Absyn_Exp*_tmp2E0;struct Cyc_Absyn_Tqual _tmp2DF;void*_tmp2DE;void*_tmp2DD;struct Cyc_Absyn_Exp*_tmp2DC;struct Cyc_Absyn_Tqual _tmp2DB;void*_tmp2DA;struct Cyc_Absyn_PtrInfo _tmp2E3;struct Cyc_Absyn_PtrInfo _tmp2E2;switch(*((int*)_tmp2D9.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D9.f2)->tag == 3U){_LL1: _tmp2E2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D9.f1)->f1;_tmp2E3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D9.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2E2;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2E3;
# 1580
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable)&&
 Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable))
return 0;
# 1584
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple22 _tmp2E4=({struct _tuple22 _tmp657;({struct Cyc_Absyn_Exp*_tmp759=({void*_tmp758=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp758,(pinfo_a.ptr_atts).bounds);});_tmp657.f1=_tmp759;}),({
struct Cyc_Absyn_Exp*_tmp757=({void*_tmp756=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp756,(pinfo_b.ptr_atts).bounds);});_tmp657.f2=_tmp757;});_tmp657;});
# 1585
struct _tuple22 _stmttmp4B=_tmp2E4;struct _tuple22 _tmp2E5=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp2E7;struct Cyc_Absyn_Exp*_tmp2E6;if(_tmp2E5.f2 == 0){_LLA: _LLB:
# 1588
 goto _LL9;}else{if(_tmp2E5.f1 == 0){_LLC: _LLD:
# 1591
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp75A=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp75A,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
return 0;}else{_LLE: _tmp2E6=_tmp2E5.f1;_tmp2E7=_tmp2E5.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2E6;struct Cyc_Absyn_Exp*e2=_tmp2E7;
# 1599
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp656;_tmp656.tag=0U,({struct _fat_ptr _tmp75B=({const char*_tmp2EA="implicit cast to shorter array";_tag_fat(_tmp2EA,sizeof(char),31U);});_tmp656.f1=_tmp75B;});_tmp656;});void*_tmp2E8[1U];_tmp2E8[0]=& _tmp2E9;({unsigned _tmp75C=loc;Cyc_Warn_warn2(_tmp75C,_tag_fat(_tmp2E8,sizeof(void*),1U));});});
if(!({struct Cyc_Absyn_Exp*_tmp75D=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp75D,(struct Cyc_Absyn_Exp*)_check_null(e1));}))
return 0;
goto _LL9;}}}_LL9:;}
# 1607
if((pinfo_a.elt_tq).real_const && !(pinfo_b.elt_tq).real_const)
return 0;
# 1610
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(!Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn))
# 1613
return 0;
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2EC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65B;_tmp65B.tag=0U,({struct _fat_ptr _tmp75E=({const char*_tmp2F1="implicit cast from region ";_tag_fat(_tmp2F1,sizeof(char),27U);});_tmp65B.f1=_tmp75E;});_tmp65B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2ED=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp65A;_tmp65A.tag=2U,_tmp65A.f1=(void*)(pinfo_a.ptr_atts).rgn;_tmp65A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2EE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp659;_tmp659.tag=0U,({
struct _fat_ptr _tmp75F=({const char*_tmp2F0=" to region ";_tag_fat(_tmp2F0,sizeof(char),12U);});_tmp659.f1=_tmp75F;});_tmp659;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2EF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp658;_tmp658.tag=2U,_tmp658.f1=(void*)(pinfo_b.ptr_atts).rgn;_tmp658;});void*_tmp2EB[4U];_tmp2EB[0]=& _tmp2EC,_tmp2EB[1]=& _tmp2ED,_tmp2EB[2]=& _tmp2EE,_tmp2EB[3]=& _tmp2EF;({unsigned _tmp760=loc;Cyc_Warn_warn2(_tmp760,_tag_fat(_tmp2EB,sizeof(void*),4U));});});}
# 1619
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)&&(
!Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)|| !(pinfo_b.elt_tq).real_const))
# 1622
return 0;
# 1626
if(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type))
return 1;
# 1630
if(!({void*_tmp761=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp761,(pinfo_b.ptr_atts).bounds);})||
 Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
return 0;
if(!(pinfo_b.elt_tq).real_const && !Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type)))
return 0;
return Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type);}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f2)->tag == 4U){_LL3: _tmp2DA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f1)->f1).elt_type;_tmp2DB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f1)->f1).tq;_tmp2DC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f1)->f1).num_elts;_tmp2DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f1)->f1).zero_term;_tmp2DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f2)->f1).elt_type;_tmp2DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f2)->f1).tq;_tmp2E0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f2)->f1).num_elts;_tmp2E1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D9.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2DA;struct Cyc_Absyn_Tqual tq1a=_tmp2DB;struct Cyc_Absyn_Exp*e1=_tmp2DC;void*zt1=_tmp2DD;void*t2a=_tmp2DE;struct Cyc_Absyn_Tqual tq2a=_tmp2DF;struct Cyc_Absyn_Exp*e2=_tmp2E0;void*zt2=_tmp2E1;
# 1639
if(!Cyc_Unify_unify(zt1,zt2))return 0;
if(e1 == 0 || e2 == 0)return 0;
if(!Cyc_Evexp_same_uint_const_exp(e1,e2))return 0;
if(!Cyc_Unify_unify(t1a,t2a))return 0;
# 1644
return !tq1a.real_const || tq2a.real_const;}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2D9.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1646
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1648
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1652
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2F2=Cyc_Tcutil_compress(t);void*_stmttmp4C=_tmp2F2;void*_tmp2F3=_stmttmp4C;void*_tmp2F4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F3)->tag == 3U){_LL1: _tmp2F4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F3)->f1).elt_type;_LL2: {void*e=_tmp2F4;
return e;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65D;_tmp65D.tag=0U,({struct _fat_ptr _tmp762=({const char*_tmp2F7="pointer_elt_type";_tag_fat(_tmp2F7,sizeof(char),17U);});_tmp65D.f1=_tmp762;});_tmp65D;});void*_tmp2F5[1U];_tmp2F5[0]=& _tmp2F6;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2F5,sizeof(void*),1U));});}_LL0:;}
# 1658
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2F8=Cyc_Tcutil_compress(t);void*_stmttmp4D=_tmp2F8;void*_tmp2F9=_stmttmp4D;struct Cyc_Absyn_PtrAtts*_tmp2FA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F9)->tag == 3U){_LL1: _tmp2FA=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F9)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2FA;
return p->rgn;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65E;_tmp65E.tag=0U,({struct _fat_ptr _tmp763=({const char*_tmp2FD="pointer_elt_type";_tag_fat(_tmp2FD,sizeof(char),17U);});_tmp65E.f1=_tmp763;});_tmp65E;});void*_tmp2FB[1U];_tmp2FB[0]=& _tmp2FC;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp2FB,sizeof(void*),1U));});}_LL0:;}
# 1665
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2FE=Cyc_Tcutil_compress(t);void*_stmttmp4E=_tmp2FE;void*_tmp2FF=_stmttmp4E;void*_tmp300;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->tag == 3U){_LL1: _tmp300=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2FF)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp300;
*rgn=r;return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1675
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp301=Cyc_Tcutil_compress(t);void*_stmttmp4F=_tmp301;void*_tmp302=_stmttmp4F;void*_tmp304;void*_tmp303;switch(*((int*)_tmp302)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp302)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp303=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).ptr_atts).nullable;_tmp304=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp302)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp303;void*bounds=_tmp304;
# 1683
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1689
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp305=e->r;void*_stmttmp50=_tmp305;void*_tmp306=_stmttmp50;struct Cyc_Absyn_Exp*_tmp308;void*_tmp307;struct _fat_ptr _tmp309;switch(*((int*)_tmp306)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp309=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp306)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp309;
# 1698
unsigned long _tmp30A=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp30A;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}
# 1708
return 0;}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1696
 return 1;case 14U: _LLD: _tmp307=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp306)->f1;_tmp308=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp306)->f2;_LLE: {void*t=_tmp307;struct Cyc_Absyn_Exp*e2=_tmp308;
# 1709
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1714
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1723
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1728
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1733
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1742
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1747
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1752
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp30B=ka;enum Cyc_Absyn_AliasQual _tmp30D;enum Cyc_Absyn_KindQual _tmp30C;_LL1: _tmp30C=_tmp30B->kind;_tmp30D=_tmp30B->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp30C;enum Cyc_Absyn_AliasQual a=_tmp30D;
{enum Cyc_Absyn_AliasQual _tmp30E=a;switch(_tmp30E){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1756
enum Cyc_Absyn_KindQual _tmp30F=k;switch(_tmp30F){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1767
{enum Cyc_Absyn_KindQual _tmp310=k;switch(_tmp310){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1774
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1776
{enum Cyc_Absyn_KindQual _tmp311=k;switch(_tmp311){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1783
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1786
({struct Cyc_Warn_String_Warn_Warg_struct _tmp313=({struct Cyc_Warn_String_Warn_Warg_struct _tmp660;_tmp660.tag=0U,({struct _fat_ptr _tmp764=({const char*_tmp315="kind_to_opt: bad kind ";_tag_fat(_tmp315,sizeof(char),23U);});_tmp660.f1=_tmp764;});_tmp660;});struct Cyc_Warn_Kind_Warn_Warg_struct _tmp314=({struct Cyc_Warn_Kind_Warn_Warg_struct _tmp65F;_tmp65F.tag=9U,_tmp65F.f1=ka;_tmp65F;});void*_tmp312[2U];_tmp312[0]=& _tmp313,_tmp312[1]=& _tmp314;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp312,sizeof(void*),2U));});}}
# 1789
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp316=_cycalloc(sizeof(*_tmp316));_tmp316->tag=0U,_tmp316->f1=k;_tmp316;});}
# 1792
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp317=_cycalloc(sizeof(*_tmp317));({void*_tmp765=Cyc_Tcutil_kind_to_bound(k);_tmp317->v=_tmp765;});_tmp317;});}
# 1798
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp318=Cyc_Tcutil_compress(t2);void*_stmttmp51=_tmp318;void*_tmp319=_stmttmp51;void*_tmp31A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp319)->tag == 3U){_LL1: _tmp31A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp319)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp31A;
# 1803
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp766=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp766;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1812
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1818
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1822
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp32E=_cycalloc(sizeof(*_tmp32E));_tmp32E->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp769=({struct _fat_ptr*_tmp32D=_cycalloc(sizeof(*_tmp32D));({struct _fat_ptr _tmp768=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp32C=({struct Cyc_Int_pa_PrintArg_struct _tmp662;_tmp662.tag=1U,_tmp662.f1=(unsigned long)counter ++;_tmp662;});void*_tmp32A[1U];_tmp32A[0]=& _tmp32C;({struct _fat_ptr _tmp767=({const char*_tmp32B="__aliasvar%d";_tag_fat(_tmp32B,sizeof(char),13U);});Cyc_aprintf(_tmp767,_tag_fat(_tmp32A,sizeof(void*),1U));});});*_tmp32D=_tmp768;});_tmp32D;});_tmp32E->f2=_tmp769;});_tmp32E;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp76A=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp329=_cycalloc(sizeof(*_tmp329));_tmp329->tag=4U,_tmp329->f1=vd;_tmp329;});Cyc_Absyn_varb_exp(_tmp76A,e->loc);});
# 1831
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp31B=Cyc_Tcutil_compress(e_type);void*_stmttmp52=_tmp31B;void*_tmp31C=_stmttmp52;struct Cyc_Absyn_PtrLoc*_tmp323;void*_tmp322;void*_tmp321;void*_tmp320;void*_tmp31F;struct Cyc_Absyn_Tqual _tmp31E;void*_tmp31D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C)->tag == 3U){_LL1: _tmp31D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C)->f1).elt_type;_tmp31E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C)->f1).elt_tq;_tmp31F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C)->f1).ptr_atts).rgn;_tmp320=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C)->f1).ptr_atts).nullable;_tmp321=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C)->f1).ptr_atts).bounds;_tmp322=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C)->f1).ptr_atts).zero_term;_tmp323=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31C)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp31D;struct Cyc_Absyn_Tqual tq=_tmp31E;void*old_r=_tmp31F;void*nb=_tmp320;void*b=_tmp321;void*zt=_tmp322;struct Cyc_Absyn_PtrLoc*pl=_tmp323;
# 1834
{void*_tmp324=Cyc_Tcutil_compress(old_r);void*_stmttmp53=_tmp324;void*_tmp325=_stmttmp53;struct Cyc_Core_Opt*_tmp327;void**_tmp326;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp325)->tag == 1U){_LL6: _tmp326=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp325)->f2;_tmp327=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp325)->f4;_LL7: {void**topt=_tmp326;struct Cyc_Core_Opt*ts=_tmp327;
# 1836
void*_tmp328=Cyc_Absyn_var_type(tv);void*new_r=_tmp328;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1841
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1844
e->topt=0;
vd->initializer=0;{
# 1848
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1850
return({struct _tuple23 _tmp661;_tmp661.f1=d,_tmp661.f2=ve;_tmp661;});}}
# 1855
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1858
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1863
void*_tmp32F=Cyc_Tcutil_compress(wants_type);void*_stmttmp54=_tmp32F;void*_tmp330=_stmttmp54;void*_tmp331;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp330)->tag == 3U){_LL1: _tmp331=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp330)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp331;
# 1865
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp332=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp332;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1871
return 0;}
# 1875
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1877
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1881
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1884
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1886
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp334=({struct Cyc_Warn_String_Warn_Warg_struct _tmp667;_tmp667.tag=0U,({struct _fat_ptr _tmp76B=({const char*_tmp33B="integral size mismatch; ";_tag_fat(_tmp33B,sizeof(char),25U);});_tmp667.f1=_tmp76B;});_tmp667;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp335=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp666;_tmp666.tag=2U,_tmp666.f1=(void*)t1;_tmp666;});struct Cyc_Warn_String_Warn_Warg_struct _tmp336=({struct Cyc_Warn_String_Warn_Warg_struct _tmp665;_tmp665.tag=0U,({
struct _fat_ptr _tmp76C=({const char*_tmp33A=" -> ";_tag_fat(_tmp33A,sizeof(char),5U);});_tmp665.f1=_tmp76C;});_tmp665;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp337=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp664;_tmp664.tag=2U,_tmp664.f1=(void*)t2;_tmp664;});struct Cyc_Warn_String_Warn_Warg_struct _tmp338=({struct Cyc_Warn_String_Warn_Warg_struct _tmp663;_tmp663.tag=0U,({struct _fat_ptr _tmp76D=({const char*_tmp339=" conversion supplied";_tag_fat(_tmp339,sizeof(char),21U);});_tmp663.f1=_tmp76D;});_tmp663;});void*_tmp333[5U];_tmp333[0]=& _tmp334,_tmp333[1]=& _tmp335,_tmp333[2]=& _tmp336,_tmp333[3]=& _tmp337,_tmp333[4]=& _tmp338;({unsigned _tmp76E=e->loc;Cyc_Warn_warn2(_tmp76E,_tag_fat(_tmp333,sizeof(void*),5U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1894
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp33D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66D;_tmp66D.tag=0U,({struct _fat_ptr _tmp76F=({const char*_tmp345="implicit alias coercion for ";_tag_fat(_tmp345,sizeof(char),29U);});_tmp66D.f1=_tmp76F;});_tmp66D;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp33E=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp66C;_tmp66C.tag=4U,_tmp66C.f1=e;_tmp66C;});struct Cyc_Warn_String_Warn_Warg_struct _tmp33F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66B;_tmp66B.tag=0U,({struct _fat_ptr _tmp770=({const char*_tmp344=":";_tag_fat(_tmp344,sizeof(char),2U);});_tmp66B.f1=_tmp770;});_tmp66B;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp340=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp66A;_tmp66A.tag=2U,_tmp66A.f1=(void*)t1;_tmp66A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp341=({struct Cyc_Warn_String_Warn_Warg_struct _tmp669;_tmp669.tag=0U,({struct _fat_ptr _tmp771=({const char*_tmp343=" to ";_tag_fat(_tmp343,sizeof(char),5U);});_tmp669.f1=_tmp771;});_tmp669;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp342=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp668;_tmp668.tag=2U,_tmp668.f1=(void*)t2;_tmp668;});void*_tmp33C[6U];_tmp33C[0]=& _tmp33D,_tmp33C[1]=& _tmp33E,_tmp33C[2]=& _tmp33F,_tmp33C[3]=& _tmp340,_tmp33C[4]=& _tmp341,_tmp33C[5]=& _tmp342;({unsigned _tmp772=e->loc;Cyc_Warn_warn2(_tmp772,_tag_fat(_tmp33C,sizeof(void*),6U));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1901
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1905
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1909
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp347=({struct Cyc_Warn_String_Warn_Warg_struct _tmp671;_tmp671.tag=0U,({struct _fat_ptr _tmp773=({const char*_tmp34C="implicit cast from ";_tag_fat(_tmp34C,sizeof(char),20U);});_tmp671.f1=_tmp773;});_tmp671;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp348=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp670;_tmp670.tag=2U,_tmp670.f1=(void*)t1;_tmp670;});struct Cyc_Warn_String_Warn_Warg_struct _tmp349=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66F;_tmp66F.tag=0U,({struct _fat_ptr _tmp774=({const char*_tmp34B=" to ";_tag_fat(_tmp34B,sizeof(char),5U);});_tmp66F.f1=_tmp774;});_tmp66F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp34A=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp66E;_tmp66E.tag=2U,_tmp66E.f1=(void*)t2;_tmp66E;});void*_tmp346[4U];_tmp346[0]=& _tmp347,_tmp346[1]=& _tmp348,_tmp346[2]=& _tmp349,_tmp346[3]=& _tmp34A;({unsigned _tmp775=e->loc;Cyc_Warn_warn2(_tmp775,_tag_fat(_tmp346,sizeof(void*),4U));});});
return 1;}
# 1915
return 0;}
# 1921
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1924
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1934
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1936
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1939
struct _tuple24 _tmp34D=*env;struct _tuple24 _stmttmp55=_tmp34D;struct _tuple24 _tmp34E=_stmttmp55;int _tmp351;struct _RegionHandle*_tmp350;struct Cyc_List_List*_tmp34F;_LL1: _tmp34F=_tmp34E.f1;_tmp350=_tmp34E.f2;_tmp351=_tmp34E.f3;_LL2: {struct Cyc_List_List*inst=_tmp34F;struct _RegionHandle*r=_tmp350;int flatten=_tmp351;
void*_tmp352=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp352;
struct Cyc_List_List*_tmp353=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp353;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp355=_region_malloc(r,sizeof(*_tmp355));({struct _tuple12*_tmp776=({struct _tuple12*_tmp354=_region_malloc(r,sizeof(*_tmp354));_tmp354->f1=x->tq,_tmp354->f2=t;_tmp354;});_tmp355->hd=_tmp776;}),_tmp355->tl=0;_tmp355;});
return ts;}}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1946
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 1948
struct _tuple25 _tmp356=*env;struct _tuple25 _stmttmp56=_tmp356;struct _tuple25 _tmp357=_stmttmp56;int _tmp359;struct _RegionHandle*_tmp358;_LL1: _tmp358=_tmp357.f1;_tmp359=_tmp357.f2;_LL2: {struct _RegionHandle*r=_tmp358;int flatten=_tmp359;
struct _tuple12 _tmp35A=*x;struct _tuple12 _stmttmp57=_tmp35A;struct _tuple12 _tmp35B=_stmttmp57;void*_tmp35D;struct Cyc_Absyn_Tqual _tmp35C;_LL4: _tmp35C=_tmp35B.f1;_tmp35D=_tmp35B.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp35C;void*t=_tmp35D;
struct Cyc_List_List*_tmp35E=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp35E;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp360=_region_malloc(r,sizeof(*_tmp360));({struct _tuple12*_tmp777=({struct _tuple12*_tmp35F=_region_malloc(r,sizeof(*_tmp35F));_tmp35F->f1=tq,_tmp35F->f2=t;_tmp35F;});_tmp360->hd=_tmp777;}),_tmp360->tl=0;_tmp360;});
return ts;}}}
# 1955
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 1957
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp361=t1;struct Cyc_List_List*_tmp362;struct Cyc_List_List*_tmp363;struct Cyc_List_List*_tmp365;struct Cyc_Absyn_Aggrdecl*_tmp364;switch(*((int*)_tmp361)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp361)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp361)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp364=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp361)->f1)->f1).KnownAggr).val;_tmp365=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp361)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp364;struct Cyc_List_List*ts=_tmp365;
# 1975
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 1977
return({struct Cyc_List_List*_tmp370=_region_malloc(r,sizeof(*_tmp370));({struct _tuple12*_tmp779=({struct _tuple12*_tmp36F=_region_malloc(r,sizeof(*_tmp36F));({struct Cyc_Absyn_Tqual _tmp778=Cyc_Absyn_empty_tqual(0U);_tmp36F->f1=_tmp778;}),_tmp36F->f2=t1;_tmp36F;});_tmp370->hd=_tmp779;}),_tmp370->tl=0;_tmp370;});{
struct Cyc_List_List*_tmp371=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp371;
struct _tuple24 env=({struct _tuple24 _tmp672;_tmp672.f1=inst,_tmp672.f2=r,_tmp672.f3=flatten;_tmp672;});
struct Cyc_List_List*_tmp372=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp58=_tmp372;struct Cyc_List_List*_tmp373=_stmttmp58;struct Cyc_List_List*_tmp375;struct Cyc_Absyn_Aggrfield*_tmp374;if(_tmp373 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp374=(struct Cyc_Absyn_Aggrfield*)_tmp373->hd;_tmp375=_tmp373->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp374;struct Cyc_List_List*tl=_tmp375;
# 1983
struct Cyc_List_List*_tmp376=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp376;
env.f3=0;{
struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp377;
struct Cyc_List_List*_tmp378=({struct Cyc_List_List*_tmp379=_region_malloc(r,sizeof(*_tmp379));_tmp379->hd=hd2,_tmp379->tl=tl2;_tmp379;});struct Cyc_List_List*tts=_tmp378;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp363=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp361)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp363;
# 1962
struct _tuple25 _tmp366=({struct _tuple25 _tmp673;_tmp673.f1=r,_tmp673.f2=flatten;_tmp673;});struct _tuple25 env=_tmp366;
# 1964
struct Cyc_List_List*_tmp367=tqs;struct Cyc_List_List*_tmp369;struct _tuple12*_tmp368;if(_tmp367 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp368=(struct _tuple12*)_tmp367->hd;_tmp369=_tmp367->tl;_LLF: {struct _tuple12*hd=_tmp368;struct Cyc_List_List*tl=_tmp369;
# 1967
struct Cyc_List_List*_tmp36A=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp36A;
env.f2=0;{
struct Cyc_List_List*_tmp36B=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp36B;
struct Cyc_List_List*_tmp36C=({struct Cyc_List_List*_tmp36E=_region_malloc(r,sizeof(*_tmp36E));_tmp36E->hd=hd2,_tmp36E->tl=tl2;_tmp36E;});struct Cyc_List_List*temp=_tmp36C;
return({struct _RegionHandle*_tmp77A=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp77A,({struct Cyc_List_List*_tmp36D=_region_malloc(r,sizeof(*_tmp36D));_tmp36D->hd=hd2,_tmp36D->tl=tl2;_tmp36D;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp361)->f1 == Cyc_Absyn_StructA){_LL7: _tmp362=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp361)->f2;_LL8: {struct Cyc_List_List*fs=_tmp362;
# 1990
struct _tuple24 env=({struct _tuple24 _tmp674;_tmp674.f1=0,_tmp674.f2=r,_tmp674.f3=flatten;_tmp674;});
struct Cyc_List_List*_tmp37A=fs;struct Cyc_List_List*_tmp37C;struct Cyc_Absyn_Aggrfield*_tmp37B;if(_tmp37A == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp37B=(struct Cyc_Absyn_Aggrfield*)_tmp37A->hd;_tmp37C=_tmp37A->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp37B;struct Cyc_List_List*tl=_tmp37C;
# 1994
struct Cyc_List_List*_tmp37D=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp37D;
env.f3=0;{
struct Cyc_List_List*_tmp37E=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp37E;
struct Cyc_List_List*_tmp37F=({struct Cyc_List_List*_tmp380=_region_malloc(r,sizeof(*_tmp380));_tmp380->hd=hd2,_tmp380->tl=tl2;_tmp380;});struct Cyc_List_List*tts=_tmp37F;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2000
 goto _LL0;}_LL0:;}}
# 2003
return({struct Cyc_List_List*_tmp382=_region_malloc(r,sizeof(*_tmp382));({struct _tuple12*_tmp77C=({struct _tuple12*_tmp381=_region_malloc(r,sizeof(*_tmp381));({struct Cyc_Absyn_Tqual _tmp77B=Cyc_Absyn_empty_tqual(0U);_tmp381->f1=_tmp77B;}),_tmp381->f2=t1;_tmp381;});_tmp382->hd=_tmp77C;}),_tmp382->tl=0;_tmp382;});}
# 2007
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp383=(void*)t->hd;void*_stmttmp59=_tmp383;void*_tmp384=_stmttmp59;switch(*((int*)_tmp384)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2015
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))
return 0;}
return 1;}
# 2021
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp385=({struct _tuple15 _tmp675;({void*_tmp77E=Cyc_Tcutil_compress(t1);_tmp675.f1=_tmp77E;}),({void*_tmp77D=Cyc_Tcutil_compress(t2);_tmp675.f2=_tmp77D;});_tmp675;});struct _tuple15 _stmttmp5A=_tmp385;struct _tuple15 _tmp386=_stmttmp5A;enum Cyc_Absyn_Size_of _tmp388;enum Cyc_Absyn_Size_of _tmp387;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp386.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp386.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp386.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp386.f2)->f1)->tag == 1U){_LL1: _tmp387=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp386.f1)->f1)->f2;_tmp388=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp386.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp387;enum Cyc_Absyn_Size_of b2=_tmp388;
# 2024
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2032
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2034
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp389=({struct _tuple15 _tmp676;_tmp676.f1=t1,_tmp676.f2=t2;_tmp676;});struct _tuple15 _stmttmp5B=_tmp389;struct _tuple15 _tmp38A=_stmttmp5B;struct Cyc_Absyn_FnInfo _tmp38C;struct Cyc_Absyn_FnInfo _tmp38B;struct Cyc_List_List*_tmp391;struct Cyc_Absyn_Datatypedecl*_tmp390;struct Cyc_List_List*_tmp38F;struct Cyc_Absyn_Datatypefield*_tmp38E;struct Cyc_Absyn_Datatypedecl*_tmp38D;void*_tmp39D;void*_tmp39C;void*_tmp39B;void*_tmp39A;struct Cyc_Absyn_Tqual _tmp399;void*_tmp398;void*_tmp397;void*_tmp396;void*_tmp395;void*_tmp394;struct Cyc_Absyn_Tqual _tmp393;void*_tmp392;switch(*((int*)_tmp38A.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f2)->tag == 3U){_LL1: _tmp392=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f1)->f1).elt_type;_tmp393=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f1)->f1).elt_tq;_tmp394=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f1)->f1).ptr_atts).rgn;_tmp395=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f1)->f1).ptr_atts).nullable;_tmp396=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f1)->f1).ptr_atts).bounds;_tmp397=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f1)->f1).ptr_atts).zero_term;_tmp398=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f2)->f1).elt_type;_tmp399=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f2)->f1).elt_tq;_tmp39A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f2)->f1).ptr_atts).rgn;_tmp39B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f2)->f1).ptr_atts).nullable;_tmp39C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f2)->f1).ptr_atts).bounds;_tmp39D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp38A.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp392;struct Cyc_Absyn_Tqual q_a=_tmp393;void*rt_a=_tmp394;void*null_a=_tmp395;void*b_a=_tmp396;void*zt_a=_tmp397;void*t_b=_tmp398;struct Cyc_Absyn_Tqual q_b=_tmp399;void*rt_b=_tmp39A;void*null_b=_tmp39B;void*b_b=_tmp39C;void*zt_b=_tmp39D;
# 2046
if(q_a.real_const && !q_b.real_const)
return 0;
# 2049
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 2053
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 2057
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 2061
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp39E=({void*_tmp77F=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp77F,b_a);});struct Cyc_Absyn_Exp*e1=_tmp39E;
struct Cyc_Absyn_Exp*_tmp39F=({void*_tmp780=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp780,b_b);});struct Cyc_Absyn_Exp*e2=_tmp39F;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 2070
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 2075
int _tmp3A0=({void*_tmp781=b_b;Cyc_Unify_unify(_tmp781,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);int deep_subtype=_tmp3A0;
# 2078
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp785=po;struct Cyc_List_List*_tmp784=({struct Cyc_List_List*_tmp3A2=_cycalloc(sizeof(*_tmp3A2));({struct _tuple15*_tmp782=({struct _tuple15*_tmp3A1=_cycalloc(sizeof(*_tmp3A1));_tmp3A1->f1=t1,_tmp3A1->f2=t2;_tmp3A1;});_tmp3A2->hd=_tmp782;}),_tmp3A2->tl=assume;_tmp3A2;});void*_tmp783=t_a;Cyc_Tcutil_ptrsubtype(_tmp785,_tmp784,_tmp783,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp38D=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp38E=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp38F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f1)->f2;_tmp390=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f2)->f1)->f1).KnownDatatype).val;_tmp391=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38A.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp38D;struct Cyc_Absyn_Datatypefield*df=_tmp38E;struct Cyc_List_List*ts1=_tmp38F;struct Cyc_Absyn_Datatypedecl*dd2=_tmp390;struct Cyc_List_List*ts2=_tmp391;
# 2085
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 2087
if(({int _tmp786=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp786 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38A.f2)->tag == 5U){_LL5: _tmp38B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38A.f1)->f1;_tmp38C=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp38A.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp38B;struct Cyc_Absyn_FnInfo f2=_tmp38C;
# 2093
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp3A3=f1.tvars;struct Cyc_List_List*tvs1=_tmp3A3;
struct Cyc_List_List*_tmp3A4=f2.tvars;struct Cyc_List_List*tvs2=_tmp3A4;
if(({int _tmp787=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp787 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp3A6=_cycalloc(sizeof(*_tmp3A6));({struct _tuple19*_tmp789=({struct _tuple19*_tmp3A5=_cycalloc(sizeof(*_tmp3A5));_tmp3A5->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp788=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp3A5->f2=_tmp788;});_tmp3A5;});_tmp3A6->hd=_tmp789;}),_tmp3A6->tl=inst;_tmp3A6;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2104
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp78C=po;struct Cyc_List_List*_tmp78B=assume;void*_tmp78A=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3A7=_cycalloc(sizeof(*_tmp3A7));_tmp3A7->tag=5U,_tmp3A7->f1=f1;_tmp3A7;});Cyc_Tcutil_subtype(_tmp78C,_tmp78B,_tmp78A,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3A8=_cycalloc(sizeof(*_tmp3A8));_tmp3A8->tag=5U,_tmp3A8->f1=f2;_tmp3A8;}));});}}}
# 2111
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp3A9=f1.args;struct Cyc_List_List*args1=_tmp3A9;
struct Cyc_List_List*_tmp3AA=f2.args;struct Cyc_List_List*args2=_tmp3AA;
# 2116
if(({int _tmp78D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp78D != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2118
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp3AB=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp5C=_tmp3AB;struct _tuple9 _tmp3AC=_stmttmp5C;void*_tmp3AE;struct Cyc_Absyn_Tqual _tmp3AD;_LLA: _tmp3AD=_tmp3AC.f2;_tmp3AE=_tmp3AC.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp3AD;void*t1=_tmp3AE;
struct _tuple9 _tmp3AF=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp5D=_tmp3AF;struct _tuple9 _tmp3B0=_stmttmp5D;void*_tmp3B2;struct Cyc_Absyn_Tqual _tmp3B1;_LLD: _tmp3B1=_tmp3B0.f2;_tmp3B2=_tmp3B0.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp3B1;void*t2=_tmp3B2;
# 2122
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2126
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp3B3=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp3B3;
struct Cyc_Absyn_VarargInfo _tmp3B4=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp3B4;
# 2131
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2136
if(!({void*_tmp78E=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp78E,(void*)_check_null(f2.effect));}))return 0;
# 2138
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2140
if(!Cyc_Tcutil_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2142
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2144
return 0;
# 2146
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2148
return 0;
# 2150
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2161
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2164
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp3B5=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp5E=_tmp3B5;struct _tuple12*_tmp3B6=_stmttmp5E;void*_tmp3B8;struct Cyc_Absyn_Tqual _tmp3B7;_LL1: _tmp3B7=_tmp3B6->f1;_tmp3B8=_tmp3B6->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp3B7;void*t1a=_tmp3B8;
struct _tuple12*_tmp3B9=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp5F=_tmp3B9;struct _tuple12*_tmp3BA=_stmttmp5F;void*_tmp3BC;struct Cyc_Absyn_Tqual _tmp3BB;_LL4: _tmp3BB=_tmp3BA->f1;_tmp3BC=_tmp3BA->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp3BB;void*t2a=_tmp3BC;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a)|| Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2178
return 1;}
# 2183
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
{void*_tmp3BD=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BD)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BD)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BD)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2191
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2193
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2197
void*_tmp3BE=t1;void*_tmp3BF;struct Cyc_Absyn_Enumdecl*_tmp3C0;void*_tmp3C4;struct Cyc_Absyn_Exp*_tmp3C3;struct Cyc_Absyn_Tqual _tmp3C2;void*_tmp3C1;void*_tmp3CA;void*_tmp3C9;void*_tmp3C8;void*_tmp3C7;struct Cyc_Absyn_Tqual _tmp3C6;void*_tmp3C5;switch(*((int*)_tmp3BE)){case 3U: _LLA: _tmp3C5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BE)->f1).elt_type;_tmp3C6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BE)->f1).elt_tq;_tmp3C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BE)->f1).ptr_atts).rgn;_tmp3C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BE)->f1).ptr_atts).nullable;_tmp3C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BE)->f1).ptr_atts).bounds;_tmp3CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BE)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp3C5;struct Cyc_Absyn_Tqual q_a=_tmp3C6;void*rt_a=_tmp3C7;void*null_a=_tmp3C8;void*b_a=_tmp3C9;void*zt_a=_tmp3CA;
# 2206
{void*_tmp3CB=t2;void*_tmp3D1;void*_tmp3D0;void*_tmp3CF;void*_tmp3CE;struct Cyc_Absyn_Tqual _tmp3CD;void*_tmp3CC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->tag == 3U){_LL19: _tmp3CC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).elt_type;_tmp3CD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).elt_tq;_tmp3CE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).ptr_atts).rgn;_tmp3CF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).ptr_atts).nullable;_tmp3D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).ptr_atts).bounds;_tmp3D1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CB)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3CC;struct Cyc_Absyn_Tqual q_b=_tmp3CD;void*rt_b=_tmp3CE;void*null_b=_tmp3CF;void*b_b=_tmp3D0;void*zt_b=_tmp3D1;
# 2208
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3D2=({struct Cyc_List_List*_tmp3DA=_cycalloc(sizeof(*_tmp3DA));({struct _tuple15*_tmp78F=({struct _tuple15*_tmp3D9=_cycalloc(sizeof(*_tmp3D9));_tmp3D9->f1=t1,_tmp3D9->f2=t2;_tmp3D9;});_tmp3DA->hd=_tmp78F;}),_tmp3DA->tl=0;_tmp3DA;});struct Cyc_List_List*assump=_tmp3D2;
int _tmp3D3=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3D3;
# 2221 "tcutil.cyc"
int _tmp3D4=
({void*_tmp790=b_b;Cyc_Unify_unify(_tmp790,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2221
int deep_castable=_tmp3D4;
# 2224
int _tmp3D5=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2224
int ptrsub=_tmp3D5;
# 2227
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2229
int _tmp3D6=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2229
int bitcase=_tmp3D6;
# 2232
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3D7=({void*_tmp791=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp791,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3D7;
struct Cyc_Absyn_Exp*_tmp3D8=({void*_tmp792=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp792,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3D8;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2241
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2246
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2252
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp3C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE)->f1).elt_type;_tmp3C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE)->f1).tq;_tmp3C3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE)->f1).num_elts;_tmp3C4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3BE)->f1).zero_term;_LLD: {void*t1a=_tmp3C1;struct Cyc_Absyn_Tqual tq1a=_tmp3C2;struct Cyc_Absyn_Exp*e1=_tmp3C3;void*zt1=_tmp3C4;
# 2254
{void*_tmp3DB=t2;void*_tmp3DF;struct Cyc_Absyn_Exp*_tmp3DE;struct Cyc_Absyn_Tqual _tmp3DD;void*_tmp3DC;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3DB)->tag == 4U){_LL1E: _tmp3DC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3DB)->f1).elt_type;_tmp3DD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3DB)->f1).tq;_tmp3DE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3DB)->f1).num_elts;_tmp3DF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3DB)->f1).zero_term;_LL1F: {void*t2a=_tmp3DC;struct Cyc_Absyn_Tqual tq2a=_tmp3DD;struct Cyc_Absyn_Exp*e2=_tmp3DE;void*zt2=_tmp3DF;
# 2256
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2261
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2263
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2265
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BE)->f1)){case 15U: _LLE: _tmp3C0=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BE)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp3C0;
# 2269
{void*_tmp3E0=t2;struct Cyc_Absyn_Enumdecl*_tmp3E1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->f1)->tag == 15U){_LL23: _tmp3E1=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E0)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3E1;
# 2271
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp793=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp793 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2277
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2280
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BE)->f2 != 0){_LL14: _tmp3BF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3BE)->f2)->hd;_LL15: {void*r1=_tmp3BF;
# 2283
{void*_tmp3E2=t2;void*_tmp3E3;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E2)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E2)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E2)->f2 != 0){_LL28: _tmp3E3=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E2)->f2)->hd;_LL29: {void*r2=_tmp3E3;
# 2285
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2289
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2295
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3E4=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3E4;
({void*_tmp794=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3E5=_cycalloc(sizeof(*_tmp3E5));_tmp3E5->tag=14U,_tmp3E5->f1=t,_tmp3E5->f2=inner,_tmp3E5->f3=0,_tmp3E5->f4=c;_tmp3E5;});e->r=_tmp794;});
e->topt=t;}}
# 2304
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2309
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
return({struct Cyc_Absyn_Tvar*_tmp3EA=_cycalloc(sizeof(*_tmp3EA));({struct _fat_ptr*_tmp797=({struct _fat_ptr*_tmp3E9=_cycalloc(sizeof(*_tmp3E9));({struct _fat_ptr _tmp796=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3E8=({struct Cyc_Int_pa_PrintArg_struct _tmp677;_tmp677.tag=1U,_tmp677.f1=(unsigned long)Cyc_Tcutil_tvar_counter ++;_tmp677;});void*_tmp3E6[1U];_tmp3E6[0]=& _tmp3E8;({struct _fat_ptr _tmp795=({const char*_tmp3E7="#%d";_tag_fat(_tmp3E7,sizeof(char),4U);});Cyc_aprintf(_tmp795,_tag_fat(_tmp3E6,sizeof(void*),1U));});});*_tmp3E9=_tmp796;});_tmp3E9;});_tmp3EA->name=_tmp797;}),_tmp3EA->identity=- 1,_tmp3EA->kind=k;_tmp3EA;});}
# 2313
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
return(int)*((const char*)_check_fat_subscript((t->name)[0],sizeof(char),0U))== (int)'#';}
# 2318
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2325
struct Cyc_List_List*_tmp3EB=0;struct Cyc_List_List*fn_type_atts=_tmp3EB;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3EC=_cycalloc(sizeof(*_tmp3EC));_tmp3EC->hd=(void*)atts->hd,_tmp3EC->tl=fn_type_atts;_tmp3EC;});}}{
struct Cyc_Absyn_FnInfo _tmp3ED=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3ED;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3EE=_cycalloc(sizeof(*_tmp3EE));_tmp3EE->tag=5U,_tmp3EE->f1=type_info;_tmp3EE;});}}
# 2333
return(void*)_check_null(fd->cached_type);}
# 2339
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2341
union Cyc_Relations_RelnOp _tmp3EF=*rop;union Cyc_Relations_RelnOp _stmttmp60=_tmp3EF;union Cyc_Relations_RelnOp _tmp3F0=_stmttmp60;struct Cyc_Absyn_Vardecl*_tmp3F1;struct Cyc_Absyn_Vardecl*_tmp3F2;switch((_tmp3F0.RNumelts).tag){case 2U: _LL1: _tmp3F2=(_tmp3F0.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3F2;
# 2343
struct _tuple1 _tmp3F3=*vd->name;struct _tuple1 _stmttmp61=_tmp3F3;struct _tuple1 _tmp3F4=_stmttmp61;struct _fat_ptr*_tmp3F6;union Cyc_Absyn_Nmspace _tmp3F5;_LL8: _tmp3F5=_tmp3F4.f1;_tmp3F6=_tmp3F4.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3F5;struct _fat_ptr*var=_tmp3F6;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp798=(struct _fat_ptr)*var;Cyc_strcmp(_tmp798,({const char*_tmp3F7="return_value";_tag_fat(_tmp3F7,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp799=Cyc_Relations_RReturn();*rop=_tmp799;});
goto _LL0;}{
# 2349
unsigned c=0U;
{struct Cyc_List_List*_tmp3F8=args;struct Cyc_List_List*a=_tmp3F8;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3F9=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp62=_tmp3F9;struct _tuple9*_tmp3FA=_stmttmp62;struct _fat_ptr*_tmp3FB;_LLB: _tmp3FB=_tmp3FA->f1;_LLC: {struct _fat_ptr*vopt=_tmp3FB;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp79A=Cyc_Relations_RParam(c);*rop=_tmp79A;});
break;}}}}
# 2357
goto _LL0;}}}case 3U: _LL3: _tmp3F1=(_tmp3F0.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3F1;
# 2359
struct _tuple1 _tmp3FC=*vd->name;struct _tuple1 _stmttmp63=_tmp3FC;struct _tuple1 _tmp3FD=_stmttmp63;struct _fat_ptr*_tmp3FF;union Cyc_Absyn_Nmspace _tmp3FE;_LLE: _tmp3FE=_tmp3FD.f1;_tmp3FF=_tmp3FD.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3FE;struct _fat_ptr*var=_tmp3FF;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp400=args;struct Cyc_List_List*a=_tmp400;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp401=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp64=_tmp401;struct _tuple9*_tmp402=_stmttmp64;struct _fat_ptr*_tmp403;_LL11: _tmp403=_tmp402->f1;_LL12: {struct _fat_ptr*vopt=_tmp403;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp79B=Cyc_Relations_RParamNumelts(c);*rop=_tmp79B;});
break;}}}}
# 2369
goto _LL0;}}}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 2374
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 2376
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 2380
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 2382
if(e == 0)return 0;{
struct Cyc_List_List*_tmp404=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp404;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2389
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp405=pr;void*_tmp407;struct Cyc_Absyn_Tqual _tmp406;_LL1: _tmp406=_tmp405->f1;_tmp407=_tmp405->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp406;void*t2=_tmp407;
return t2 == t?pr:({struct _tuple12*_tmp408=_cycalloc(sizeof(*_tmp408));_tmp408->f1=tq,_tmp408->f2=t;_tmp408;});}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2393
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2395
return({struct _tuple27*_tmp40A=_region_malloc(rgn,sizeof(*_tmp40A));({struct _tuple26*_tmp79C=({struct _tuple26*_tmp409=_region_malloc(rgn,sizeof(*_tmp409));_tmp409->f1=(*y).f1,_tmp409->f2=(*y).f2;_tmp409;});_tmp40A->f1=_tmp79C;}),_tmp40A->f2=(*y).f3;_tmp40A;});}
# 2397
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2399
struct _tuple9 _tmp40B=*orig_arg;struct _tuple9 _stmttmp65=_tmp40B;struct _tuple9 _tmp40C=_stmttmp65;void*_tmp40F;struct Cyc_Absyn_Tqual _tmp40E;struct _fat_ptr*_tmp40D;_LL1: _tmp40D=_tmp40C.f1;_tmp40E=_tmp40C.f2;_tmp40F=_tmp40C.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp40D;struct Cyc_Absyn_Tqual tq_orig=_tmp40E;void*t_orig=_tmp40F;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp410=_cycalloc(sizeof(*_tmp410));_tmp410->f1=vopt_orig,_tmp410->f2=tq_orig,_tmp410->f3=t;_tmp410;});}}
# 2403
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2407
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2409
return({struct Cyc_Absyn_Exp*_tmp411=_cycalloc(sizeof(*_tmp411));_tmp411->topt=old->topt,_tmp411->r=r,_tmp411->loc=old->loc,_tmp411->annot=old->annot;_tmp411;});}
# 2414
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp412=e->r;void*_stmttmp66=_tmp412;void*_tmp413=_stmttmp66;void*_tmp414;struct Cyc_List_List*_tmp416;void*_tmp415;struct Cyc_Absyn_Exp*_tmp417;struct Cyc_Absyn_Exp*_tmp418;void*_tmp419;enum Cyc_Absyn_Coercion _tmp41D;int _tmp41C;struct Cyc_Absyn_Exp*_tmp41B;void*_tmp41A;struct Cyc_Absyn_Exp*_tmp41F;struct Cyc_Absyn_Exp*_tmp41E;struct Cyc_Absyn_Exp*_tmp421;struct Cyc_Absyn_Exp*_tmp420;struct Cyc_Absyn_Exp*_tmp423;struct Cyc_Absyn_Exp*_tmp422;struct Cyc_Absyn_Exp*_tmp426;struct Cyc_Absyn_Exp*_tmp425;struct Cyc_Absyn_Exp*_tmp424;struct Cyc_List_List*_tmp428;enum Cyc_Absyn_Primop _tmp427;switch(*((int*)_tmp413)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp427=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_tmp428=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp413)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp427;struct Cyc_List_List*es=_tmp428;
# 2423
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp429=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp429;
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42A;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp79E=e;Cyc_Tcutil_copye(_tmp79E,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->tag=3U,_tmp42C->f1=p,({struct Cyc_List_List*_tmp79D=({struct Cyc_Absyn_Exp*_tmp42B[1U];_tmp42B[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp42B,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp42C->f2=_tmp79D;});_tmp42C;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp42D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp42D;
struct Cyc_Absyn_Exp*_tmp42E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp42E;
struct Cyc_Absyn_Exp*_tmp42F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42F;
struct Cyc_Absyn_Exp*_tmp430=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp430;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp7A0=e;Cyc_Tcutil_copye(_tmp7A0,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->tag=3U,_tmp432->f1=p,({struct Cyc_List_List*_tmp79F=({struct Cyc_Absyn_Exp*_tmp431[2U];_tmp431[0]=new_e1,_tmp431[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp431,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp432->f2=_tmp79F;});_tmp432;}));});}}
# 2435
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp434=({struct Cyc_Warn_String_Warn_Warg_struct _tmp678;_tmp678.tag=0U,({struct _fat_ptr _tmp7A1=({const char*_tmp435="primop does not have 1 or 2 args!";_tag_fat(_tmp435,sizeof(char),34U);});_tmp678.f1=_tmp7A1;});_tmp678;});void*_tmp433[1U];_tmp433[0]=& _tmp434;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp433,sizeof(void*),1U));});}case 6U: _LLD: _tmp424=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_tmp425=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp413)->f2;_tmp426=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp413)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp424;struct Cyc_Absyn_Exp*e2=_tmp425;struct Cyc_Absyn_Exp*e3=_tmp426;
# 2437
struct Cyc_Absyn_Exp*_tmp436=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp436;
struct Cyc_Absyn_Exp*_tmp437=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp437;
struct Cyc_Absyn_Exp*_tmp438=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp438;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp7A2=e;Cyc_Tcutil_copye(_tmp7A2,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439->tag=6U,_tmp439->f1=new_e1,_tmp439->f2=new_e2,_tmp439->f3=new_e3;_tmp439;}));});}case 7U: _LLF: _tmp422=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_tmp423=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp413)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp422;struct Cyc_Absyn_Exp*e2=_tmp423;
# 2443
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp43A;
struct Cyc_Absyn_Exp*_tmp43B=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp43B;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp7A3=e;Cyc_Tcutil_copye(_tmp7A3,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp43C=_cycalloc(sizeof(*_tmp43C));_tmp43C->tag=7U,_tmp43C->f1=new_e1,_tmp43C->f2=new_e2;_tmp43C;}));});}case 8U: _LL11: _tmp420=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_tmp421=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp413)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp420;struct Cyc_Absyn_Exp*e2=_tmp421;
# 2448
struct Cyc_Absyn_Exp*_tmp43D=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp43D;
struct Cyc_Absyn_Exp*_tmp43E=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp43E;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp7A4=e;Cyc_Tcutil_copye(_tmp7A4,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp43F=_cycalloc(sizeof(*_tmp43F));_tmp43F->tag=8U,_tmp43F->f1=new_e1,_tmp43F->f2=new_e2;_tmp43F;}));});}case 9U: _LL13: _tmp41E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_tmp41F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp413)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp41E;struct Cyc_Absyn_Exp*e2=_tmp41F;
# 2453
struct Cyc_Absyn_Exp*_tmp440=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp440;
struct Cyc_Absyn_Exp*_tmp441=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp441;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp7A5=e;Cyc_Tcutil_copye(_tmp7A5,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=9U,_tmp442->f1=new_e1,_tmp442->f2=new_e2;_tmp442;}));});}case 14U: _LL15: _tmp41A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_tmp41B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp413)->f2;_tmp41C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp413)->f3;_tmp41D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp413)->f4;_LL16: {void*t=_tmp41A;struct Cyc_Absyn_Exp*e1=_tmp41B;int b=_tmp41C;enum Cyc_Absyn_Coercion c=_tmp41D;
# 2458
struct Cyc_Absyn_Exp*_tmp443=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp443;
void*_tmp444=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp444;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp7A6=e;Cyc_Tcutil_copye(_tmp7A6,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp445=_cycalloc(sizeof(*_tmp445));_tmp445->tag=14U,_tmp445->f1=new_typ,_tmp445->f2=new_e1,_tmp445->f3=b,_tmp445->f4=c;_tmp445;}));});}case 17U: _LL17: _tmp419=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_LL18: {void*t=_tmp419;
# 2463
void*_tmp446=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp446;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp7A7=e;Cyc_Tcutil_copye(_tmp7A7,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp447=_cycalloc(sizeof(*_tmp447));_tmp447->tag=17U,_tmp447->f1=new_typ;_tmp447;}));});}case 18U: _LL19: _tmp418=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp418;
# 2466
struct Cyc_Absyn_Exp*_tmp448=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp448;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp7A8=e;Cyc_Tcutil_copye(_tmp7A8,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp449=_cycalloc(sizeof(*_tmp449));_tmp449->tag=18U,_tmp449->f1=new_e1;_tmp449;}));});}case 41U: _LL1B: _tmp417=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp417;
# 2469
struct Cyc_Absyn_Exp*_tmp44A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp44A;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp7A9=e;Cyc_Tcutil_copye(_tmp7A9,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp44B=_cycalloc(sizeof(*_tmp44B));_tmp44B->tag=41U,_tmp44B->f1=new_e1;_tmp44B;}));});}case 19U: _LL1D: _tmp415=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_tmp416=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp413)->f2;_LL1E: {void*t=_tmp415;struct Cyc_List_List*f=_tmp416;
# 2472
void*_tmp44C=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp44C;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp7AA=e;Cyc_Tcutil_copye(_tmp7AA,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp44D=_cycalloc(sizeof(*_tmp44D));_tmp44D->tag=19U,_tmp44D->f1=new_typ,_tmp44D->f2=f;_tmp44D;}));});}case 39U: _LL1F: _tmp414=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp413)->f1;_LL20: {void*t=_tmp414;
# 2475
void*_tmp44E=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp44E;
if(new_typ == t)return e;{
# 2478
void*_tmp44F=Cyc_Tcutil_compress(new_typ);void*_stmttmp67=_tmp44F;void*_tmp450=_stmttmp67;struct Cyc_Absyn_Exp*_tmp451;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp450)->tag == 9U){_LL24: _tmp451=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp450)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp451;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp7AB=e;Cyc_Tcutil_copye(_tmp7AB,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp452=_cycalloc(sizeof(*_tmp452));_tmp452->tag=39U,_tmp452->f1=new_typ;_tmp452;}));});}_LL23:;}}default: _LL21: _LL22:
# 2482
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp454=({struct Cyc_Warn_String_Warn_Warg_struct _tmp679;_tmp679.tag=0U,({struct _fat_ptr _tmp7AC=({const char*_tmp455="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp455,sizeof(char),46U);});_tmp679.f1=_tmp7AC;});_tmp679;});void*_tmp453[1U];_tmp453[0]=& _tmp454;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp453,sizeof(void*),1U));});}_LL0:;}
# 2486
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2489
return e == 0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2492
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2495
void*_tmp456=f->type;void*t=_tmp456;
struct Cyc_Absyn_Exp*_tmp457=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp457;
void*_tmp458=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp458;
struct Cyc_Absyn_Exp*_tmp459=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp459;
if(t == new_typ && req == new_req)
return f;{
struct Cyc_Absyn_Aggrfield*_tmp45A=({struct Cyc_Absyn_Aggrfield*_tmp45B=_cycalloc(sizeof(*_tmp45B));*_tmp45B=*f;_tmp45B;});struct Cyc_Absyn_Aggrfield*ans=_tmp45A;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2507
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2510
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp45C=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp45C;
struct Cyc_List_List*_tmp45D=fs->tl;struct Cyc_List_List*t=_tmp45D;
struct Cyc_Absyn_Aggrfield*_tmp45E=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp45E;
struct Cyc_List_List*_tmp45F=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_typ=_tmp45F;
if(new_f == f && new_typ == t)
return fs;
return({struct Cyc_List_List*_tmp460=_cycalloc(sizeof(*_tmp460));_tmp460->hd=new_f,_tmp460->tl=new_typ;_tmp460;});}}
# 2520
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2523
struct _tuple0 _tmp461=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp68=_tmp461;struct _tuple0 _tmp462=_stmttmp68;struct Cyc_List_List*_tmp464;struct Cyc_List_List*_tmp463;_LL1: _tmp463=_tmp462.f1;_tmp464=_tmp462.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp463;struct Cyc_List_List*rpo1b=_tmp464;
struct Cyc_List_List*_tmp465=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp465;
struct Cyc_List_List*_tmp466=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp466;
if(rpo2a == rpo1a && rpo2b == rpo1b)
return rgn_po;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}
# 2531
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2534
void*_tmp467=Cyc_Tcutil_compress(t);void*_stmttmp69=_tmp467;void*_tmp468=_stmttmp69;struct Cyc_Absyn_Exp*_tmp469;struct Cyc_Absyn_Exp*_tmp46A;struct Cyc_List_List*_tmp46C;void*_tmp46B;void*_tmp46D;struct Cyc_List_List*_tmp46F;enum Cyc_Absyn_AggrKind _tmp46E;struct Cyc_List_List*_tmp470;struct Cyc_Absyn_Vardecl*_tmp47C;struct Cyc_Absyn_Exp*_tmp47B;struct Cyc_Absyn_Exp*_tmp47A;struct Cyc_List_List*_tmp479;struct Cyc_List_List*_tmp478;struct Cyc_Absyn_VarargInfo*_tmp477;int _tmp476;struct Cyc_List_List*_tmp475;void*_tmp474;struct Cyc_Absyn_Tqual _tmp473;void*_tmp472;struct Cyc_List_List*_tmp471;void*_tmp482;void*_tmp481;void*_tmp480;void*_tmp47F;struct Cyc_Absyn_Tqual _tmp47E;void*_tmp47D;unsigned _tmp487;void*_tmp486;struct Cyc_Absyn_Exp*_tmp485;struct Cyc_Absyn_Tqual _tmp484;void*_tmp483;void*_tmp48B;struct Cyc_Absyn_Typedefdecl*_tmp48A;struct Cyc_List_List*_tmp489;struct _tuple1*_tmp488;struct Cyc_Absyn_Tvar*_tmp48C;switch(*((int*)_tmp468)){case 2U: _LL1: _tmp48C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp468)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp48C;
# 2537
struct _handler_cons _tmp48D;_push_handler(& _tmp48D);{int _tmp48F=0;if(setjmp(_tmp48D.handler))_tmp48F=1;if(!_tmp48F){{void*_tmp490=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp490;};_pop_handler();}else{void*_tmp48E=(void*)Cyc_Core_get_exn_thrown();void*_tmp491=_tmp48E;void*_tmp492;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp491)->tag == Cyc_Core_Not_found){_LL1A: _LL1B:
 return t;}else{_LL1C: _tmp492=_tmp491;_LL1D: {void*exn=_tmp492;(int)_rethrow(exn);}}_LL19:;}}}case 8U: _LL3: _tmp488=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp468)->f1;_tmp489=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp468)->f2;_tmp48A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp468)->f3;_tmp48B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp468)->f4;_LL4: {struct _tuple1*n=_tmp488;struct Cyc_List_List*ts=_tmp489;struct Cyc_Absyn_Typedefdecl*td=_tmp48A;void*topt=_tmp48B;
# 2540
struct Cyc_List_List*_tmp493=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp493;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp494=_cycalloc(sizeof(*_tmp494));_tmp494->tag=8U,_tmp494->f1=n,_tmp494->f2=new_ts,_tmp494->f3=td,_tmp494->f4=topt;_tmp494;});}case 4U: _LL5: _tmp483=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468)->f1).elt_type;_tmp484=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468)->f1).tq;_tmp485=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468)->f1).num_elts;_tmp486=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468)->f1).zero_term;_tmp487=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp468)->f1).zt_loc;_LL6: {void*t1=_tmp483;struct Cyc_Absyn_Tqual tq=_tmp484;struct Cyc_Absyn_Exp*e=_tmp485;void*zt=_tmp486;unsigned ztl=_tmp487;
# 2543
void*_tmp495=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp495;
struct Cyc_Absyn_Exp*_tmp496=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp496;
void*_tmp497=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp497;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp498=_cycalloc(sizeof(*_tmp498));
_tmp498->tag=4U,(_tmp498->f1).elt_type=new_t1,(_tmp498->f1).tq=tq,(_tmp498->f1).num_elts=new_e,(_tmp498->f1).zero_term=new_zt,(_tmp498->f1).zt_loc=ztl;_tmp498;});}case 3U: _LL7: _tmp47D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468)->f1).elt_type;_tmp47E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468)->f1).elt_tq;_tmp47F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468)->f1).ptr_atts).rgn;_tmp480=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468)->f1).ptr_atts).nullable;_tmp481=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468)->f1).ptr_atts).bounds;_tmp482=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp468)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp47D;struct Cyc_Absyn_Tqual tq=_tmp47E;void*r=_tmp47F;void*n=_tmp480;void*b=_tmp481;void*zt=_tmp482;
# 2549
void*_tmp499=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp499;
void*_tmp49A=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp49A;
void*_tmp49B=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp49B;
void*_tmp49C=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp49C;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp67A;_tmp67A.elt_type=new_t1,_tmp67A.elt_tq=tq,(_tmp67A.ptr_atts).rgn=new_r,(_tmp67A.ptr_atts).nullable=n,(_tmp67A.ptr_atts).bounds=new_b,(_tmp67A.ptr_atts).zero_term=new_zt,(_tmp67A.ptr_atts).ptrloc=0;_tmp67A;}));}case 5U: _LL9: _tmp471=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).tvars;_tmp472=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).effect;_tmp473=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).ret_tqual;_tmp474=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).ret_type;_tmp475=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).args;_tmp476=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).c_varargs;_tmp477=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).cyc_varargs;_tmp478=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).rgn_po;_tmp479=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).attributes;_tmp47A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).requires_clause;_tmp47B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).ensures_clause;_tmp47C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp468)->f1).return_value;_LLA: {struct Cyc_List_List*vs=_tmp471;void*eff=_tmp472;struct Cyc_Absyn_Tqual rtq=_tmp473;void*rtyp=_tmp474;struct Cyc_List_List*args=_tmp475;int c_varargs=_tmp476;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp477;struct Cyc_List_List*rgn_po=_tmp478;struct Cyc_List_List*atts=_tmp479;struct Cyc_Absyn_Exp*req=_tmp47A;struct Cyc_Absyn_Exp*ens=_tmp47B;struct Cyc_Absyn_Vardecl*ret_value=_tmp47C;
# 2559
{struct Cyc_List_List*_tmp49D=vs;struct Cyc_List_List*p=_tmp49D;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp49F=_region_malloc(rgn,sizeof(*_tmp49F));({struct _tuple19*_tmp7AE=({struct _tuple19*_tmp49E=_region_malloc(rgn,sizeof(*_tmp49E));_tmp49E->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp7AD=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp49E->f2=_tmp7AD;});_tmp49E;});_tmp49F->hd=_tmp7AE;}),_tmp49F->tl=inst;_tmp49F;});}}{
struct _tuple0 _tmp4A0=({struct _RegionHandle*_tmp7B0=rgn;struct _RegionHandle*_tmp7AF=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp7B0,_tmp7AF,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2561
struct _tuple0 _stmttmp6A=_tmp4A0;struct _tuple0 _tmp4A1=_stmttmp6A;struct Cyc_List_List*_tmp4A3;struct Cyc_List_List*_tmp4A2;_LL1F: _tmp4A2=_tmp4A1.f1;_tmp4A3=_tmp4A1.f2;_LL20: {struct Cyc_List_List*qs=_tmp4A2;struct Cyc_List_List*ts=_tmp4A3;
# 2563
struct Cyc_List_List*_tmp4A4=args;struct Cyc_List_List*args2=_tmp4A4;
struct Cyc_List_List*_tmp4A5=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp4A5;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff == 0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2569
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2573
struct Cyc_Absyn_VarargInfo _tmp4A6=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp6B=_tmp4A6;struct Cyc_Absyn_VarargInfo _tmp4A7=_stmttmp6B;int _tmp4AB;void*_tmp4AA;struct Cyc_Absyn_Tqual _tmp4A9;struct _fat_ptr*_tmp4A8;_LL22: _tmp4A8=_tmp4A7.name;_tmp4A9=_tmp4A7.tq;_tmp4AA=_tmp4A7.type;_tmp4AB=_tmp4A7.inject;_LL23: {struct _fat_ptr*n=_tmp4A8;struct Cyc_Absyn_Tqual tq=_tmp4A9;void*t=_tmp4AA;int i=_tmp4AB;
void*_tmp4AC=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp4AC;
if(t2 == t)cyc_varargs2=cyc_varargs;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->name=n,_tmp4AD->tq=tq,_tmp4AD->type=t2,_tmp4AD->inject=i;_tmp4AD;});}}}{
# 2578
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp4AE=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp4AE;
struct Cyc_List_List*_tmp4AF=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp4AF;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4B0=_cycalloc(sizeof(*_tmp4B0));_tmp4B0->tag=5U,(_tmp4B0->f1).tvars=vs,(_tmp4B0->f1).effect=eff2,(_tmp4B0->f1).ret_tqual=rtq,({void*_tmp7B1=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp4B0->f1).ret_type=_tmp7B1;}),(_tmp4B0->f1).args=args2,(_tmp4B0->f1).c_varargs=c_varargs,(_tmp4B0->f1).cyc_varargs=cyc_varargs2,(_tmp4B0->f1).rgn_po=rgn_po2,(_tmp4B0->f1).attributes=atts,(_tmp4B0->f1).requires_clause=req2,(_tmp4B0->f1).requires_relns=req_relns2,(_tmp4B0->f1).ensures_clause=ens2,(_tmp4B0->f1).ensures_relns=ens_relns2,(_tmp4B0->f1).return_value=ret_value;_tmp4B0;});}}}}}case 6U: _LLB: _tmp470=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp468)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp470;
# 2587
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp4B1=tqts;struct Cyc_List_List*ts1=_tmp4B1;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp4B2=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp4B2;
void*_tmp4B3=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp4B3;
if(t1 != t2)
change=1;
ts2=({struct Cyc_List_List*_tmp4B4=_region_malloc(rgn,sizeof(*_tmp4B4));_tmp4B4->hd=t2,_tmp4B4->tl=ts2;_tmp4B4;});}}
# 2596
return !change?t:(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->tag=6U,({struct Cyc_List_List*_tmp7B3=({struct Cyc_List_List*_tmp7B2=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp7B2,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp4B5->f1=_tmp7B3;});_tmp4B5;});}case 7U: _LLD: _tmp46E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp468)->f1;_tmp46F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp468)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp46E;struct Cyc_List_List*fs=_tmp46F;
# 2598
struct Cyc_List_List*_tmp4B6=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp4B6;
return fs == new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->tag=7U,_tmp4B7->f1=k,_tmp4B7->f2=new_fs;_tmp4B7;});}case 1U: _LLF: _tmp46D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp468)->f2;_LL10: {void*r=_tmp46D;
# 2601
return r == 0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0U: _LL11: _tmp46B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp468)->f1;_tmp46C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp468)->f2;_LL12: {void*c=_tmp46B;struct Cyc_List_List*ts=_tmp46C;
# 2603
struct Cyc_List_List*_tmp4B8=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp4B8;
return ts == new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp4B9=_cycalloc(sizeof(*_tmp4B9));_tmp4B9->tag=0U,_tmp4B9->f1=c,_tmp4B9->f2=new_ts;_tmp4B9;});}case 9U: _LL13: _tmp46A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp468)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp46A;
# 2606
struct Cyc_Absyn_Exp*_tmp4BA=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4BA;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4BB=_cycalloc(sizeof(*_tmp4BB));_tmp4BB->tag=9U,_tmp4BB->f1=new_e;_tmp4BB;});}case 11U: _LL15: _tmp469=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp468)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp469;
# 2609
struct Cyc_Absyn_Exp*_tmp4BC=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4BC;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->tag=11U,_tmp4BD->f1=new_e;_tmp4BD;});}default: _LL17: _LL18:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4BF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67B;_tmp67B.tag=0U,({struct _fat_ptr _tmp7B4=({const char*_tmp4C0="found typedecltype in rsubs";_tag_fat(_tmp4C0,sizeof(char),28U);});_tmp67B.f1=_tmp7B4;});_tmp67B;});void*_tmp4BE[1U];_tmp4BE[0]=& _tmp4BF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4BE,sizeof(void*),1U));});}_LL0:;}
# 2615
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2618
if(ts == 0)
return 0;{
void*_tmp4C1=(void*)ts->hd;void*old_hd=_tmp4C1;
struct Cyc_List_List*_tmp4C2=ts->tl;struct Cyc_List_List*old_tl=_tmp4C2;
void*_tmp4C3=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp4C3;
struct Cyc_List_List*_tmp4C4=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp4C4;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp4C5=_cycalloc(sizeof(*_tmp4C5));_tmp4C5->hd=new_hd,_tmp4C5->tl=new_tl;_tmp4C5;});}}
# 2629
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
return inst == 0?t: Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);}
# 2634
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4C6=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4C6;
return({struct _tuple19*_tmp4C8=_cycalloc(sizeof(*_tmp4C8));_tmp4C8->f1=tv,({void*_tmp7B6=({struct Cyc_Core_Opt*_tmp7B5=k;Cyc_Absyn_new_evar(_tmp7B5,({struct Cyc_Core_Opt*_tmp4C7=_cycalloc(sizeof(*_tmp4C7));_tmp4C7->v=s;_tmp4C7;}));});_tmp4C8->f2=_tmp7B6;});_tmp4C8;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2639
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2641
struct _tuple28*_tmp4C9=env;struct _RegionHandle*_tmp4CB;struct Cyc_List_List*_tmp4CA;_LL1: _tmp4CA=_tmp4C9->f1;_tmp4CB=_tmp4C9->f2;_LL2: {struct Cyc_List_List*s=_tmp4CA;struct _RegionHandle*rgn=_tmp4CB;
struct Cyc_Core_Opt*_tmp4CC=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4CC;
return({struct _tuple19*_tmp4CE=_region_malloc(rgn,sizeof(*_tmp4CE));_tmp4CE->f1=tv,({void*_tmp7B8=({struct Cyc_Core_Opt*_tmp7B7=k;Cyc_Absyn_new_evar(_tmp7B7,({struct Cyc_Core_Opt*_tmp4CD=_cycalloc(sizeof(*_tmp4CD));_tmp4CD->v=s;_tmp4CD;}));});_tmp4CE->f2=_tmp7B8;});_tmp4CE;});}}
# 2646
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2648
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4D0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67E;_tmp67E.tag=0U,({struct _fat_ptr _tmp7B9=({const char*_tmp4D4="bitfield ";_tag_fat(_tmp4D4,sizeof(char),10U);});_tmp67E.f1=_tmp7B9;});_tmp67E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4D1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67D;_tmp67D.tag=0U,_tmp67D.f1=*fn;_tmp67D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4D2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67C;_tmp67C.tag=0U,({struct _fat_ptr _tmp7BA=({const char*_tmp4D3=" does not have constant width";_tag_fat(_tmp4D3,sizeof(char),30U);});_tmp67C.f1=_tmp7BA;});_tmp67C;});void*_tmp4CF[3U];_tmp4CF[0]=& _tmp4D0,_tmp4CF[1]=& _tmp4D1,_tmp4CF[2]=& _tmp4D2;({unsigned _tmp7BB=loc;Cyc_Warn_err2(_tmp7BB,_tag_fat(_tmp4CF,sizeof(void*),3U));});});else{
# 2653
struct _tuple13 _tmp4D5=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp6C=_tmp4D5;struct _tuple13 _tmp4D6=_stmttmp6C;int _tmp4D8;unsigned _tmp4D7;_LL1: _tmp4D7=_tmp4D6.f1;_tmp4D8=_tmp4D6.f2;_LL2: {unsigned i=_tmp4D7;int known=_tmp4D8;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67F;_tmp67F.tag=0U,({struct _fat_ptr _tmp7BC=({const char*_tmp4DB="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4DB,sizeof(char),47U);});_tmp67F.f1=_tmp7BC;});_tmp67F;});void*_tmp4D9[1U];_tmp4D9[0]=& _tmp4DA;({unsigned _tmp7BD=loc;Cyc_Warn_warn2(_tmp7BD,_tag_fat(_tmp4D9,sizeof(void*),1U));});});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp680;_tmp680.tag=0U,({struct _fat_ptr _tmp7BE=({const char*_tmp4DE="bitfield has negative width";_tag_fat(_tmp4DE,sizeof(char),28U);});_tmp680.f1=_tmp7BE;});_tmp680;});void*_tmp4DC[1U];_tmp4DC[0]=& _tmp4DD;({unsigned _tmp7BF=loc;Cyc_Warn_err2(_tmp7BF,_tag_fat(_tmp4DC,sizeof(void*),1U));});});
w=i;}}{
# 2660
void*_tmp4DF=Cyc_Tcutil_compress(field_type);void*_stmttmp6D=_tmp4DF;void*_tmp4E0=_stmttmp6D;enum Cyc_Absyn_Size_of _tmp4E1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E0)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E0)->f1)->tag == 1U){_LL4: _tmp4E1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4E0)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4E1;
# 2663
int bad;
{enum Cyc_Absyn_Size_of _tmp4E2=b;switch(_tmp4E2){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 bad=w > (unsigned)8;goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 bad=w > (unsigned)16;goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 bad=w > (unsigned)32;goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 bad=w > (unsigned)64;goto _LL8;}_LL8:;}
# 2672
if(bad)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp681;_tmp681.tag=0U,({struct _fat_ptr _tmp7C0=({const char*_tmp4E5="bitfield larger than type";_tag_fat(_tmp4E5,sizeof(char),26U);});_tmp681.f1=_tmp7C0;});_tmp681;});void*_tmp4E3[1U];_tmp4E3[0]=& _tmp4E4;({unsigned _tmp7C1=loc;Cyc_Warn_warn2(_tmp7C1,_tag_fat(_tmp4E3,sizeof(void*),1U));});});
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2676
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E7=({struct Cyc_Warn_String_Warn_Warg_struct _tmp685;_tmp685.tag=0U,({struct _fat_ptr _tmp7C2=({const char*_tmp4EC="bitfield ";_tag_fat(_tmp4EC,sizeof(char),10U);});_tmp685.f1=_tmp7C2;});_tmp685;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E8=({struct Cyc_Warn_String_Warn_Warg_struct _tmp684;_tmp684.tag=0U,_tmp684.f1=*fn;_tmp684;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4E9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp683;_tmp683.tag=0U,({struct _fat_ptr _tmp7C3=({const char*_tmp4EB=" must have integral type but has type ";_tag_fat(_tmp4EB,sizeof(char),39U);});_tmp683.f1=_tmp7C3;});_tmp683;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4EA=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp682;_tmp682.tag=2U,_tmp682.f1=(void*)field_type;_tmp682;});void*_tmp4E6[4U];_tmp4E6[0]=& _tmp4E7,_tmp4E6[1]=& _tmp4E8,_tmp4E6[2]=& _tmp4E9,_tmp4E6[3]=& _tmp4EA;({unsigned _tmp7C4=loc;Cyc_Warn_err2(_tmp7C4,_tag_fat(_tmp4E6,sizeof(void*),4U));});});
# 2678
goto _LL3;}_LL3:;}}}
# 2685
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4ED=t;void*_tmp4EF;struct Cyc_Absyn_Typedefdecl*_tmp4EE;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4ED)->tag == 8U){_LL1: _tmp4EE=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4ED)->f3;_tmp4EF=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4ED)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4EE;void*tdopt=_tmp4EF;
# 2688
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp686;_tmp686.tag=0U,({struct _fat_ptr _tmp7C5=({const char*_tmp4F2="extra const";_tag_fat(_tmp4F2,sizeof(char),12U);});_tmp686.f1=_tmp7C5;});_tmp686;});void*_tmp4F0[1U];_tmp4F0[0]=& _tmp4F1;({unsigned _tmp7C6=loc;Cyc_Warn_warn2(_tmp7C6,_tag_fat(_tmp4F0,sizeof(void*),1U));});});
return 1;}
# 2693
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2700
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp7C7=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp7C7;});}
# 2704
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2710
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2713
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4F4=({struct Cyc_Warn_String_Warn_Warg_struct _tmp689;_tmp689.tag=0U,_tmp689.f1=msg;_tmp689;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4F5=({struct Cyc_Warn_String_Warn_Warg_struct _tmp688;_tmp688.tag=0U,({struct _fat_ptr _tmp7C8=({const char*_tmp4F7=": ";_tag_fat(_tmp4F7,sizeof(char),3U);});_tmp688.f1=_tmp7C8;});_tmp688;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4F6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp687;_tmp687.tag=0U,({struct _fat_ptr _tmp7C9=a2string(vs->hd);_tmp687.f1=_tmp7C9;});_tmp687;});void*_tmp4F3[3U];_tmp4F3[0]=& _tmp4F4,_tmp4F3[1]=& _tmp4F5,_tmp4F3[2]=& _tmp4F6;({unsigned _tmp7CA=loc;Cyc_Warn_err2(_tmp7CA,_tag_fat(_tmp4F3,sizeof(void*),3U));});});}}}
# 2718
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2721
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2724
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp7CC=tvs;unsigned _tmp7CB=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp7CC,_tmp7CB,Cyc_Absynpp_tvar2string,({const char*_tmp4F8="duplicate type variable";_tag_fat(_tmp4F8,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2739 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2744
struct _RegionHandle _tmp4F9=_new_region("temp");struct _RegionHandle*temp=& _tmp4F9;_push_region(temp);
# 2748
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp7CD=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp7CD,({const char*_tmp4FA="";_tag_fat(_tmp4FA,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4FC=_region_malloc(temp,sizeof(*_tmp4FC));({struct _tuple29*_tmp7CE=({struct _tuple29*_tmp4FB=_region_malloc(temp,sizeof(*_tmp4FB));_tmp4FB->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4FB->f2=0;_tmp4FB;});_tmp4FC->hd=_tmp7CE;}),_tmp4FC->tl=fields;_tmp4FC;});}}
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2754
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp528="struct";_tag_fat(_tmp528,sizeof(char),7U);}):({const char*_tmp529="union";_tag_fat(_tmp529,sizeof(char),6U);});
# 2757
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4FD=(struct _tuple30*)des->hd;struct _tuple30*_stmttmp6E=_tmp4FD;struct _tuple30*_tmp4FE=_stmttmp6E;void*_tmp500;struct Cyc_List_List*_tmp4FF;_LL1: _tmp4FF=_tmp4FE->f1;_tmp500=_tmp4FE->f2;_LL2: {struct Cyc_List_List*dl=_tmp4FF;void*a=_tmp500;
if(dl == 0){
# 2762
struct Cyc_List_List*_tmp501=fields;struct Cyc_List_List*fields2=_tmp501;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple29*)fields2->hd)).f2){
(*((struct _tuple29*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp7D0=({struct Cyc_List_List*_tmp503=_cycalloc(sizeof(*_tmp503));({void*_tmp7CF=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp502=_cycalloc(sizeof(*_tmp502));_tmp502->tag=1U,_tmp502->f1=((*((struct _tuple29*)fields2->hd)).f1)->name;_tmp502;});_tmp503->hd=_tmp7CF;}),_tmp503->tl=0;_tmp503;});(*((struct _tuple30*)des->hd)).f1=_tmp7D0;});
ans=({struct Cyc_List_List*_tmp505=_region_malloc(rgn,sizeof(*_tmp505));({struct _tuple31*_tmp7D1=({struct _tuple31*_tmp504=_region_malloc(rgn,sizeof(*_tmp504));_tmp504->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp504->f2=a;_tmp504;});_tmp505->hd=_tmp7D1;}),_tmp505->tl=ans;_tmp505;});
break;}}
# 2770
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp507=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68B;_tmp68B.tag=0U,({struct _fat_ptr _tmp7D2=({const char*_tmp509="too many arguments to ";_tag_fat(_tmp509,sizeof(char),23U);});_tmp68B.f1=_tmp7D2;});_tmp68B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp508=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68A;_tmp68A.tag=0U,_tmp68A.f1=aggr_str;_tmp68A;});void*_tmp506[2U];_tmp506[0]=& _tmp507,_tmp506[1]=& _tmp508;({unsigned _tmp7D3=loc;Cyc_Warn_err2(_tmp7D3,_tag_fat(_tmp506,sizeof(void*),2U));});});}else{
if(dl->tl != 0)
# 2774
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68C;_tmp68C.tag=0U,({struct _fat_ptr _tmp7D4=({const char*_tmp50C="multiple designators are not yet supported";_tag_fat(_tmp50C,sizeof(char),43U);});_tmp68C.f1=_tmp7D4;});_tmp68C;});void*_tmp50A[1U];_tmp50A[0]=& _tmp50B;({unsigned _tmp7D5=loc;Cyc_Warn_err2(_tmp7D5,_tag_fat(_tmp50A,sizeof(void*),1U));});});else{
# 2777
void*_tmp50D=(void*)dl->hd;void*_stmttmp6F=_tmp50D;void*_tmp50E=_stmttmp6F;struct _fat_ptr*_tmp50F;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp50E)->tag == 0U){_LL4: _LL5:
# 2779
({struct Cyc_Warn_String_Warn_Warg_struct _tmp511=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68E;_tmp68E.tag=0U,({struct _fat_ptr _tmp7D6=({const char*_tmp513="array designator used in argument to ";_tag_fat(_tmp513,sizeof(char),38U);});_tmp68E.f1=_tmp7D6;});_tmp68E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp512=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68D;_tmp68D.tag=0U,_tmp68D.f1=aggr_str;_tmp68D;});void*_tmp510[2U];_tmp510[0]=& _tmp511,_tmp510[1]=& _tmp512;({unsigned _tmp7D7=loc;Cyc_Warn_err2(_tmp7D7,_tag_fat(_tmp510,sizeof(void*),2U));});});
goto _LL3;}else{_LL6: _tmp50F=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp50E)->f1;_LL7: {struct _fat_ptr*v=_tmp50F;
# 2782
struct Cyc_List_List*_tmp514=fields;struct Cyc_List_List*fields2=_tmp514;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple29*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple29*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp516=({struct Cyc_Warn_String_Warn_Warg_struct _tmp691;_tmp691.tag=0U,({struct _fat_ptr _tmp7D8=({const char*_tmp51A="member ";_tag_fat(_tmp51A,sizeof(char),8U);});_tmp691.f1=_tmp7D8;});_tmp691;});struct Cyc_Warn_String_Warn_Warg_struct _tmp517=({struct Cyc_Warn_String_Warn_Warg_struct _tmp690;_tmp690.tag=0U,_tmp690.f1=*v;_tmp690;});struct Cyc_Warn_String_Warn_Warg_struct _tmp518=({struct Cyc_Warn_String_Warn_Warg_struct _tmp68F;_tmp68F.tag=0U,({struct _fat_ptr _tmp7D9=({const char*_tmp519=" has already been used as an argument";_tag_fat(_tmp519,sizeof(char),38U);});_tmp68F.f1=_tmp7D9;});_tmp68F;});void*_tmp515[3U];_tmp515[0]=& _tmp516,_tmp515[1]=& _tmp517,_tmp515[2]=& _tmp518;({unsigned _tmp7DA=loc;Cyc_Warn_err2(_tmp7DA,_tag_fat(_tmp515,sizeof(void*),3U));});});
(*((struct _tuple29*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp51C=_region_malloc(rgn,sizeof(*_tmp51C));({struct _tuple31*_tmp7DB=({struct _tuple31*_tmp51B=_region_malloc(rgn,sizeof(*_tmp51B));_tmp51B->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp51B->f2=a;_tmp51B;});_tmp51C->hd=_tmp7DB;}),_tmp51C->tl=ans;_tmp51C;});
break;}}
# 2791
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp51E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp693;_tmp693.tag=0U,({struct _fat_ptr _tmp7DC=({const char*_tmp520="bad field designator ";_tag_fat(_tmp520,sizeof(char),22U);});_tmp693.f1=_tmp7DC;});_tmp693;});struct Cyc_Warn_String_Warn_Warg_struct _tmp51F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp692;_tmp692.tag=0U,_tmp692.f1=*v;_tmp692;});void*_tmp51D[2U];_tmp51D[0]=& _tmp51E,_tmp51D[1]=& _tmp51F;({unsigned _tmp7DD=loc;Cyc_Warn_err2(_tmp7DD,_tag_fat(_tmp51D,sizeof(void*),2U));});});
goto _LL3;}}_LL3:;}}}}
# 2798
if((int)aggr_kind == (int)0U){
if(({int _tmp7DE=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans);_tmp7DE < ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(fields);}))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp522=({struct Cyc_Warn_String_Warn_Warg_struct _tmp694;_tmp694.tag=0U,({struct _fat_ptr _tmp7DF=({const char*_tmp523="too few arguments to struct";_tag_fat(_tmp523,sizeof(char),28U);});_tmp694.f1=_tmp7DF;});_tmp694;});void*_tmp521[1U];_tmp521[0]=& _tmp522;({unsigned _tmp7E0=loc;Cyc_Warn_err2(_tmp7E0,_tag_fat(_tmp521,sizeof(void*),1U));});});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ans)!= 1)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp525=({struct Cyc_Warn_String_Warn_Warg_struct _tmp695;_tmp695.tag=0U,({struct _fat_ptr _tmp7E1=({const char*_tmp526="union expression should have one member";_tag_fat(_tmp526,sizeof(char),40U);});_tmp695.f1=_tmp7E1;});_tmp695;});void*_tmp524[1U];_tmp524[0]=& _tmp525;({unsigned _tmp7E2=loc;Cyc_Warn_err2(_tmp7E2,_tag_fat(_tmp524,sizeof(void*),1U));});});}{
struct Cyc_List_List*_tmp527=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp527;}}}
# 2748
;_pop_region();}
# 2809
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2811
void*_tmp52A=e1->r;void*_stmttmp70=_tmp52A;void*_tmp52B=_stmttmp70;struct Cyc_Absyn_Exp*_tmp52C;struct Cyc_Absyn_Exp*_tmp52D;struct Cyc_Absyn_Exp*_tmp52E;struct Cyc_Absyn_Exp*_tmp52F;struct Cyc_Absyn_Exp*_tmp530;struct Cyc_Absyn_Exp*_tmp531;switch(*((int*)_tmp52B)){case 14U: _LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp533=({struct Cyc_Warn_String_Warn_Warg_struct _tmp697;_tmp697.tag=0U,({struct _fat_ptr _tmp7E3=({const char*_tmp535="we have a cast in a lhs: ";_tag_fat(_tmp535,sizeof(char),26U);});_tmp697.f1=_tmp7E3;});_tmp697;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp534=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp696;_tmp696.tag=4U,_tmp696.f1=e1;_tmp696;});void*_tmp532[2U];_tmp532[0]=& _tmp533,_tmp532[1]=& _tmp534;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp532,sizeof(void*),2U));});case 20U: _LL3: _tmp531=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp531;
_tmp530=e1a;goto _LL6;}case 23U: _LL5: _tmp530=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp530;
# 2815
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp52F=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp52F;
_tmp52E=e1a;goto _LLA;}case 21U: _LL9: _tmp52E=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp52E;
# 2819
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp537=({struct Cyc_Warn_String_Warn_Warg_struct _tmp699;_tmp699.tag=0U,({struct _fat_ptr _tmp7E4=({const char*_tmp539="found zero pointer aggregate member assignment: ";_tag_fat(_tmp539,sizeof(char),49U);});_tmp699.f1=_tmp7E4;});_tmp699;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp538=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp698;_tmp698.tag=4U,_tmp698.f1=e1;_tmp698;});void*_tmp536[2U];_tmp536[0]=& _tmp537,_tmp536[1]=& _tmp538;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp536,sizeof(void*),2U));});
return 0;}case 13U: _LLB: _tmp52D=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp52D;
_tmp52C=e1a;goto _LLE;}case 12U: _LLD: _tmp52C=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp52B)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp52C;
# 2824
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp53B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69B;_tmp69B.tag=0U,({struct _fat_ptr _tmp7E5=({const char*_tmp53D="found zero pointer instantiate/noinstantiate: ";_tag_fat(_tmp53D,sizeof(char),47U);});_tmp69B.f1=_tmp7E5;});_tmp69B;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp53C=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp69A;_tmp69A.tag=4U,_tmp69A.f1=e1;_tmp69A;});void*_tmp53A[2U];_tmp53A[0]=& _tmp53B,_tmp53A[1]=& _tmp53C;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp53A,sizeof(void*),2U));});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp53F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69D;_tmp69D.tag=0U,({struct _fat_ptr _tmp7E6=({const char*_tmp541="found bad lhs in is_zero_ptr_deref: ";_tag_fat(_tmp541,sizeof(char),37U);});_tmp69D.f1=_tmp7E6;});_tmp69D;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp540=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp69C;_tmp69C.tag=4U,_tmp69C.f1=e1;_tmp69C;});void*_tmp53E[2U];_tmp53E[0]=& _tmp53F,_tmp53E[1]=& _tmp540;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp53E,sizeof(void*),2U));});}_LL0:;}
# 2838
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmp542=Cyc_Tcutil_compress(r);void*_stmttmp71=_tmp542;void*_tmp543=_stmttmp71;struct Cyc_Absyn_Tvar*_tmp544;enum Cyc_Absyn_AliasQual _tmp546;enum Cyc_Absyn_KindQual _tmp545;switch(*((int*)_tmp543)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp543)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp543)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp543)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp543)->f4 == 0){_LL5: _tmp545=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp543)->f3)->kind)->v)->kind;_tmp546=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp543)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp545;enum Cyc_Absyn_AliasQual a=_tmp546;
# 2843
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp544=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp543)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp544;
# 2847
struct Cyc_Absyn_Kind*_tmp547=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp72=_tmp547;struct Cyc_Absyn_Kind*_tmp548=_stmttmp72;enum Cyc_Absyn_AliasQual _tmp54A;enum Cyc_Absyn_KindQual _tmp549;_LLC: _tmp549=_tmp548->kind;_tmp54A=_tmp548->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp549;enum Cyc_Absyn_AliasQual a=_tmp54A;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp54B=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp73=_tmp54B;void*_tmp54C=_stmttmp73;struct Cyc_Core_Opt**_tmp54D;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp54C)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp54C)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp54C)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp54D=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp54C)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp54D;
# 2851
({struct Cyc_Core_Opt*_tmp7E8=({struct Cyc_Core_Opt*_tmp54F=_cycalloc(sizeof(*_tmp54F));({void*_tmp7E7=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp54E=_cycalloc(sizeof(*_tmp54E));_tmp54E->tag=2U,_tmp54E->f1=0,_tmp54E->f2=& Cyc_Tcutil_rk;_tmp54E;});_tmp54F->v=_tmp7E7;});_tmp54F;});*x=_tmp7E8;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2856
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2863
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp550=Cyc_Tcutil_compress(t);void*_stmttmp74=_tmp550;void*_tmp551=_stmttmp74;struct Cyc_Absyn_Tvar*_tmp552;void*_tmp553;switch(*((int*)_tmp551)){case 3U: _LL1: _tmp553=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp551)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp553;
# 2866
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp552=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp551)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp552;
# 2868
struct Cyc_Absyn_Kind*_tmp554=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp75=_tmp554;struct Cyc_Absyn_Kind*_tmp555=_stmttmp75;enum Cyc_Absyn_AliasQual _tmp557;enum Cyc_Absyn_KindQual _tmp556;_LL8: _tmp556=_tmp555->kind;_tmp557=_tmp555->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp556;enum Cyc_Absyn_AliasQual a=_tmp557;
enum Cyc_Absyn_KindQual _tmp558=k;switch(_tmp558){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp559=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp76=_tmp559;void*_tmp55A=_stmttmp76;enum Cyc_Absyn_KindQual _tmp55C;struct Cyc_Core_Opt**_tmp55B;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp55A)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp55A)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp55B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp55A)->f1;_tmp55C=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp55A)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp55B;enum Cyc_Absyn_KindQual k=_tmp55C;
# 2874
({struct Cyc_Core_Opt*_tmp7EB=({struct Cyc_Core_Opt*_tmp55F=_cycalloc(sizeof(*_tmp55F));({void*_tmp7EA=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp55E=_cycalloc(sizeof(*_tmp55E));_tmp55E->tag=2U,_tmp55E->f1=0,({struct Cyc_Absyn_Kind*_tmp7E9=({struct Cyc_Absyn_Kind*_tmp55D=_cycalloc(sizeof(*_tmp55D));_tmp55D->kind=k,_tmp55D->aliasqual=Cyc_Absyn_Aliasable;_tmp55D;});_tmp55E->f2=_tmp7E9;});_tmp55E;});_tmp55F->v=_tmp7EA;});_tmp55F;});*x=_tmp7EB;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
 return 1;}_LL13:;}
# 2879
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2882
 return 0;}_LL0:;}
# 2885
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
t=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(t,0))
return 1;{
void*_tmp560=t;struct Cyc_List_List*_tmp561;struct Cyc_List_List*_tmp563;union Cyc_Absyn_DatatypeFieldInfo _tmp562;struct Cyc_List_List*_tmp565;struct Cyc_Absyn_Aggrdecl**_tmp564;struct Cyc_List_List*_tmp566;switch(*((int*)_tmp560)){case 6U: _LL1: _tmp566=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp560)->f1;_LL2: {struct Cyc_List_List*qts=_tmp566;
# 2891
for(0;qts != 0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp564=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1)->f1).KnownAggr).val;_tmp565=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp564;struct Cyc_List_List*ts=_tmp565;
# 2896
if((*adp)->impl == 0)
return 0;{
struct Cyc_List_List*_tmp567=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp567;
{struct Cyc_List_List*_tmp568=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp568;for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type)))
return 1;}}
return 0;}}}else{_LL7: _LL8:
# 2908
 goto _LLA;}case 18U: _LL9: _LLA:
 return 0;case 19U: _LLB: _tmp562=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1)->f1;_tmp563=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp562;struct Cyc_List_List*ts=_tmp563;
# 2912
union Cyc_Absyn_DatatypeFieldInfo _tmp569=tinfo;struct Cyc_Absyn_Datatypefield*_tmp56B;struct Cyc_Absyn_Datatypedecl*_tmp56A;if((_tmp569.UnknownDatatypefield).tag == 1){_LL10: _LL11:
 return 0;}else{_LL12: _tmp56A=((_tmp569.KnownDatatypefield).val).f1;_tmp56B=((_tmp569.KnownDatatypefield).val).f2;_LL13: {struct Cyc_Absyn_Datatypedecl*td=_tmp56A;struct Cyc_Absyn_Datatypefield*fld=_tmp56B;
# 2915
struct Cyc_List_List*_tmp56C=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp56C;
{struct Cyc_List_List*_tmp56D=fld->typs;struct Cyc_List_List*typs=_tmp56D;for(0;typs != 0;typs=typs->tl){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 2921
return 0;}}_LLF:;}default: goto _LLD;}case 7U: _LL5: _tmp561=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp560)->f2;_LL6: {struct Cyc_List_List*x=_tmp561;
# 2904
for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD: _LLE:
# 2923
 return 0;}_LL0:;}}
# 2930
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp56E=e->r;void*_stmttmp77=_tmp56E;void*_tmp56F=_stmttmp77;struct Cyc_Absyn_Stmt*_tmp570;struct Cyc_Absyn_Exp*_tmp571;struct Cyc_Absyn_Exp*_tmp572;struct Cyc_Absyn_Exp*_tmp574;struct Cyc_Absyn_Exp*_tmp573;struct Cyc_Absyn_Exp*_tmp576;struct Cyc_Absyn_Exp*_tmp575;struct _fat_ptr*_tmp578;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_Absyn_Exp*_tmp579;struct Cyc_Absyn_Exp*_tmp57A;switch(*((int*)_tmp56F)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp56F)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp57A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp56F)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp57A;
_tmp579=e1;goto _LL6;}case 20U: _LL5: _tmp579=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp56F)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp579;
# 2935
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp577=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp56F)->f1;_tmp578=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp56F)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp577;struct _fat_ptr*f=_tmp578;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp575=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp56F)->f1;_tmp576=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp56F)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp575;struct Cyc_Absyn_Exp*e2=_tmp576;
# 2938
void*_tmp57B=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp78=_tmp57B;void*_tmp57C=_stmttmp78;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp57C)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp573=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp56F)->f2;_tmp574=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp56F)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp573;struct Cyc_Absyn_Exp*e2=_tmp574;
# 2943
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp572=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp56F)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp572;
_tmp571=e2;goto _LL10;}case 14U: _LLF: _tmp571=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp56F)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp571;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp570=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp56F)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp570;
# 2947
while(1){
void*_tmp57D=s->r;void*_stmttmp79=_tmp57D;void*_tmp57E=_stmttmp79;struct Cyc_Absyn_Exp*_tmp57F;struct Cyc_Absyn_Stmt*_tmp581;struct Cyc_Absyn_Decl*_tmp580;struct Cyc_Absyn_Stmt*_tmp583;struct Cyc_Absyn_Stmt*_tmp582;switch(*((int*)_tmp57E)){case 2U: _LL1B: _tmp582=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp57E)->f1;_tmp583=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp57E)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp582;struct Cyc_Absyn_Stmt*s2=_tmp583;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp580=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp57E)->f1;_tmp581=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp57E)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp580;struct Cyc_Absyn_Stmt*s1=_tmp581;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp57F=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp57E)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp57F;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp585=({struct Cyc_Warn_String_Warn_Warg_struct _tmp69E;_tmp69E.tag=0U,({struct _fat_ptr _tmp7EC=({const char*_tmp586="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp586,sizeof(char),40U);});_tmp69E.f1=_tmp7EC;});_tmp69E;});void*_tmp584[1U];_tmp584[0]=& _tmp585;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp584,sizeof(void*),1U));});}_LL1A:;}}default: _LL13: _LL14:
# 2955
 return 1;}_LL0:;}
# 2972 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 2974
struct _tuple14 bogus_ans=({struct _tuple14 _tmp6AC;_tmp6AC.f1=0,_tmp6AC.f2=Cyc_Absyn_heap_rgn_type;_tmp6AC;});
void*_tmp587=e->r;void*_stmttmp7A=_tmp587;void*_tmp588=_stmttmp7A;struct Cyc_Absyn_Exp*_tmp58A;struct Cyc_Absyn_Exp*_tmp589;struct Cyc_Absyn_Exp*_tmp58B;int _tmp58E;struct _fat_ptr*_tmp58D;struct Cyc_Absyn_Exp*_tmp58C;int _tmp591;struct _fat_ptr*_tmp590;struct Cyc_Absyn_Exp*_tmp58F;void*_tmp592;switch(*((int*)_tmp588)){case 1U: _LL1: _tmp592=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp588)->f1;_LL2: {void*x=_tmp592;
# 2978
void*_tmp593=x;struct Cyc_Absyn_Vardecl*_tmp594;struct Cyc_Absyn_Vardecl*_tmp595;struct Cyc_Absyn_Vardecl*_tmp596;struct Cyc_Absyn_Vardecl*_tmp597;switch(*((int*)_tmp593)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp597=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp593)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp597;
# 2982
void*_tmp598=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7B=_tmp598;void*_tmp599=_stmttmp7B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp599)->tag == 4U){_LL1B: _LL1C:
# 2984
 return({struct _tuple14 _tmp69F;_tmp69F.f1=1,_tmp69F.f2=Cyc_Absyn_heap_rgn_type;_tmp69F;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp6A0;_tmp6A0.f1=(vd->tq).real_const,_tmp6A0.f2=Cyc_Absyn_heap_rgn_type;_tmp6A0;});}_LL1A:;}case 4U: _LL14: _tmp596=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp593)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp596;
# 2988
void*_tmp59A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7C=_tmp59A;void*_tmp59B=_stmttmp7C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp59B)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp6A1;_tmp6A1.f1=1,_tmp6A1.f2=(void*)_check_null(vd->rgn);_tmp6A1;});}else{_LL22: _LL23:
# 2991
 vd->escapes=1;
return({struct _tuple14 _tmp6A2;_tmp6A2.f1=(vd->tq).real_const,_tmp6A2.f2=(void*)_check_null(vd->rgn);_tmp6A2;});}_LL1F:;}case 5U: _LL16: _tmp595=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp593)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp595;
# 2994
_tmp594=vd;goto _LL19;}default: _LL18: _tmp594=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp593)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp594;
# 2996
vd->escapes=1;
return({struct _tuple14 _tmp6A3;_tmp6A3.f1=(vd->tq).real_const,_tmp6A3.f2=(void*)_check_null(vd->rgn);_tmp6A3;});}}_LLD:;}case 21U: _LL3: _tmp58F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp588)->f1;_tmp590=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp588)->f2;_tmp591=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp588)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp58F;struct _fat_ptr*f=_tmp590;int is_tagged=_tmp591;
# 3001
if(is_tagged)return bogus_ans;{
# 3004
void*_tmp59C=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7D=_tmp59C;void*_tmp59D=_stmttmp7D;struct Cyc_Absyn_Aggrdecl*_tmp59E;struct Cyc_List_List*_tmp59F;switch(*((int*)_tmp59D)){case 7U: _LL25: _tmp59F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp59D)->f2;_LL26: {struct Cyc_List_List*fs=_tmp59F;
# 3006
struct Cyc_Absyn_Aggrfield*_tmp5A0=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp5A0;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp5A1=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7E=_tmp5A1;struct _tuple14 _tmp5A2=_stmttmp7E;void*_tmp5A4;int _tmp5A3;_LL2C: _tmp5A3=_tmp5A2.f1;_tmp5A4=_tmp5A2.f2;_LL2D: {int c=_tmp5A3;void*t=_tmp5A4;
return({struct _tuple14 _tmp6A4;_tmp6A4.f1=(finfo->tq).real_const || c,_tmp6A4.f2=t;_tmp6A4;});}}
# 3011
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59D)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59D)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp59E=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp59D)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp59E;
# 3013
struct Cyc_Absyn_Aggrfield*_tmp5A5=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp5A5;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp5A6=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7F=_tmp5A6;struct _tuple14 _tmp5A7=_stmttmp7F;void*_tmp5A9;int _tmp5A8;_LL2F: _tmp5A8=_tmp5A7.f1;_tmp5A9=_tmp5A7.f2;_LL30: {int c=_tmp5A8;void*t=_tmp5A9;
return({struct _tuple14 _tmp6A5;_tmp6A5.f1=(finfo->tq).real_const || c,_tmp6A5.f2=t;_tmp6A5;});}}
# 3018
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp58C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp588)->f1;_tmp58D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp588)->f2;_tmp58E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp588)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp58C;struct _fat_ptr*f=_tmp58D;int is_tagged=_tmp58E;
# 3023
if(is_tagged)return bogus_ans;{
# 3026
void*_tmp5AA=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp80=_tmp5AA;void*_tmp5AB=_stmttmp80;void*_tmp5AD;void*_tmp5AC;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AB)->tag == 3U){_LL32: _tmp5AC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AB)->f1).elt_type;_tmp5AD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AB)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp5AC;void*r=_tmp5AD;
# 3028
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp5AE=Cyc_Tcutil_compress(t1);void*_stmttmp81=_tmp5AE;void*_tmp5AF=_stmttmp81;struct Cyc_Absyn_Aggrdecl*_tmp5B0;struct Cyc_List_List*_tmp5B1;switch(*((int*)_tmp5AF)){case 7U: _LL37: _tmp5B1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5AF)->f2;_LL38: {struct Cyc_List_List*fs=_tmp5B1;
# 3031
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5AF)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5AF)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp5B0=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5AF)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp5B0;
# 3033
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3036
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp6A6;_tmp6A6.f1=(finfo->tq).real_const,_tmp6A6.f2=r;_tmp6A6;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp58B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp588)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp58B;
# 3043
void*_tmp5B2=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp82=_tmp5B2;void*_tmp5B3=_stmttmp82;void*_tmp5B5;struct Cyc_Absyn_Tqual _tmp5B4;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B3)->tag == 3U){_LL3E: _tmp5B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B3)->f1).elt_tq;_tmp5B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B3)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp5B4;void*r=_tmp5B5;
# 3045
return({struct _tuple14 _tmp6A7;_tmp6A7.f1=tq.real_const,_tmp6A7.f2=r;_tmp6A7;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp589=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp588)->f1;_tmp58A=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp588)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp589;struct Cyc_Absyn_Exp*e2=_tmp58A;
# 3051
void*t=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*_tmp5B6=t;struct Cyc_Absyn_Tqual _tmp5B7;void*_tmp5B9;struct Cyc_Absyn_Tqual _tmp5B8;struct Cyc_List_List*_tmp5BA;switch(*((int*)_tmp5B6)){case 6U: _LL43: _tmp5BA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5B6)->f1;_LL44: {struct Cyc_List_List*ts=_tmp5BA;
# 3055
struct _tuple13 _tmp5BB=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp83=_tmp5BB;struct _tuple13 _tmp5BC=_stmttmp83;int _tmp5BE;unsigned _tmp5BD;_LL4C: _tmp5BD=_tmp5BC.f1;_tmp5BE=_tmp5BC.f2;_LL4D: {unsigned i=_tmp5BD;int known=_tmp5BE;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp5BF=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp5BF;
if(finfo != 0)
return({struct _tuple14 _tmp6A8;_tmp6A8.f1=((*finfo).f1).real_const,({void*_tmp7ED=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp6A8.f2=_tmp7ED;});_tmp6A8;});
return bogus_ans;}}}case 3U: _LL45: _tmp5B8=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B6)->f1).elt_tq;_tmp5B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5B6)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp5B8;void*r=_tmp5B9;
# 3063
return({struct _tuple14 _tmp6A9;_tmp6A9.f1=tq.real_const,_tmp6A9.f2=r;_tmp6A9;});}case 4U: _LL47: _tmp5B7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5B6)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp5B7;
# 3069
return({struct _tuple14 _tmp6AA;_tmp6AA.f1=tq.real_const,({void*_tmp7EE=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp6AA.f2=_tmp7EE;});_tmp6AA;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3073
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6AB;_tmp6AB.tag=0U,({struct _fat_ptr _tmp7EF=({const char*_tmp5C2="unary & applied to non-lvalue";_tag_fat(_tmp5C2,sizeof(char),30U);});_tmp6AB.f1=_tmp7EF;});_tmp6AB;});void*_tmp5C0[1U];_tmp5C0[0]=& _tmp5C1;({unsigned _tmp7F0=e->loc;Cyc_Warn_err2(_tmp7F0,_tag_fat(_tmp5C0,sizeof(void*),1U));});});
return bogus_ans;}_LL0:;}
# 3080
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp5C3=({void*_tmp7F1=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7F1,b);});struct Cyc_Absyn_Exp*eopt=_tmp5C3;
if(eopt == 0)return;{
struct _tuple13 _tmp5C4=Cyc_Evexp_eval_const_uint_exp(eopt);struct _tuple13 _stmttmp84=_tmp5C4;struct _tuple13 _tmp5C5=_stmttmp84;int _tmp5C7;unsigned _tmp5C6;_LL1: _tmp5C6=_tmp5C5.f1;_tmp5C7=_tmp5C5.f2;_LL2: {unsigned j=_tmp5C6;int known=_tmp5C7;
if(known && j <= i)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp5C9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B0;_tmp6B0.tag=0U,({struct _fat_ptr _tmp7F2=({const char*_tmp5CE="a dereference is out of bounds: ";_tag_fat(_tmp5CE,sizeof(char),33U);});_tmp6B0.f1=_tmp7F2;});_tmp6B0;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp5CA=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp6AF;_tmp6AF.tag=12U,_tmp6AF.f1=(int)j;_tmp6AF;});struct Cyc_Warn_String_Warn_Warg_struct _tmp5CB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6AE;_tmp6AE.tag=0U,({
struct _fat_ptr _tmp7F3=({const char*_tmp5CD=" <= ";_tag_fat(_tmp5CD,sizeof(char),5U);});_tmp6AE.f1=_tmp7F3;});_tmp6AE;});struct Cyc_Warn_Int_Warn_Warg_struct _tmp5CC=({struct Cyc_Warn_Int_Warn_Warg_struct _tmp6AD;_tmp6AD.tag=12U,_tmp6AD.f1=(int)i;_tmp6AD;});void*_tmp5C8[4U];_tmp5C8[0]=& _tmp5C9,_tmp5C8[1]=& _tmp5CA,_tmp5C8[2]=& _tmp5CB,_tmp5C8[3]=& _tmp5CC;({void(*_tmp7F5)(unsigned,struct _fat_ptr)=
# 3085
do_warn?Cyc_Warn_warn2: Cyc_Warn_err2;unsigned _tmp7F4=loc;_tmp7F5(_tmp7F4,_tag_fat(_tmp5C8,sizeof(void*),4U));});});
# 3087
return;}}}
# 3090
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3098
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
int(*_tmp5CF)(int var_okay,struct Cyc_Absyn_Exp*e)=Cyc_Tcutil_cnst_exp;int(*rec)(int var_okay,struct Cyc_Absyn_Exp*e)=_tmp5CF;
void*_tmp5D0=e->r;void*_stmttmp85=_tmp5D0;void*_tmp5D1=_stmttmp85;struct Cyc_List_List*_tmp5D2;struct Cyc_List_List*_tmp5D3;struct Cyc_List_List*_tmp5D4;struct Cyc_List_List*_tmp5D5;struct Cyc_List_List*_tmp5D6;struct Cyc_List_List*_tmp5D7;struct Cyc_Absyn_Exp*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DD;struct Cyc_Absyn_Exp*_tmp5DE;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_Absyn_Exp*_tmp5E1;struct Cyc_Absyn_Exp*_tmp5E0;struct Cyc_Absyn_Exp*_tmp5E4;struct Cyc_Absyn_Exp*_tmp5E3;struct Cyc_Absyn_Exp*_tmp5E2;void*_tmp5E5;switch(*((int*)_tmp5D1)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5E5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL10: {void*b=_tmp5E5;
# 3111
void*_tmp5E6=b;struct Cyc_Absyn_Vardecl*_tmp5E7;struct Cyc_Absyn_Vardecl*_tmp5E8;switch(*((int*)_tmp5E6)){case 2U: _LL32: _LL33:
 return 1;case 1U: _LL34: _tmp5E8=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL35: {struct Cyc_Absyn_Vardecl*vd=_tmp5E8;
# 3114
void*_tmp5E9=Cyc_Tcutil_compress(vd->type);void*_stmttmp86=_tmp5E9;void*_tmp5EA=_stmttmp86;switch(*((int*)_tmp5EA)){case 4U: _LL3D: _LL3E:
 goto _LL40;case 5U: _LL3F: _LL40:
 return 1;default: _LL41: _LL42:
 return var_okay;}_LL3C:;}case 4U: _LL36: _tmp5E7=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5E6)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5E7;
# 3120
if((int)vd->sc != (int)Cyc_Absyn_Static)
return var_okay;{
void*_tmp5EB=Cyc_Tcutil_compress(vd->type);void*_stmttmp87=_tmp5EB;void*_tmp5EC=_stmttmp87;switch(*((int*)_tmp5EC)){case 4U: _LL44: _LL45:
 goto _LL47;case 5U: _LL46: _LL47:
 return 1;default: _LL48: _LL49:
 return var_okay;}_LL43:;}}case 0U: _LL38: _LL39:
# 3127
 return 0;default: _LL3A: _LL3B:
 return var_okay;}_LL31:;}case 6U: _LL11: _tmp5E2=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_tmp5E3=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5D1)->f2;_tmp5E4=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5D1)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5E2;struct Cyc_Absyn_Exp*e2=_tmp5E3;struct Cyc_Absyn_Exp*e3=_tmp5E4;
# 3131
return(rec(0,e1)&& rec(0,e2))&& rec(0,e3);}case 9U: _LL13: _tmp5E0=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_tmp5E1=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5D1)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5E0;struct Cyc_Absyn_Exp*e2=_tmp5E1;
return rec(0,e1)&& rec(0,e2);}case 41U: _LL15: _tmp5DF=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5DF;
_tmp5DE=e2;goto _LL18;}case 12U: _LL17: _tmp5DE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5DE;
_tmp5DD=e2;goto _LL1A;}case 13U: _LL19: _tmp5DD=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5DD;
_tmp5DC=e2;goto _LL1C;}case 14U: _LL1B: _tmp5DC=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5D1)->f2;_LL1C: {struct Cyc_Absyn_Exp*e2=_tmp5DC;
# 3138
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 15U: _LL1D: _tmp5DB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL1E: {struct Cyc_Absyn_Exp*e2=_tmp5DB;
return rec(1,e2);}case 27U: _LL1F: _tmp5D9=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5D1)->f2;_tmp5DA=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5D1)->f3;_LL20: {struct Cyc_Absyn_Exp*e1=_tmp5D9;struct Cyc_Absyn_Exp*e2=_tmp5DA;
return rec(0,e1)&& rec(0,e2);}case 28U: _LL21: _tmp5D8=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL22: {struct Cyc_Absyn_Exp*e=_tmp5D8;
return rec(0,e);}case 26U: _LL23: _tmp5D7=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL24: {struct Cyc_List_List*des=_tmp5D7;
_tmp5D6=des;goto _LL26;}case 30U: _LL25: _tmp5D6=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5D1)->f2;_LL26: {struct Cyc_List_List*des=_tmp5D6;
_tmp5D5=des;goto _LL28;}case 29U: _LL27: _tmp5D5=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5D1)->f3;_LL28: {struct Cyc_List_List*des=_tmp5D5;
# 3145
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL29: _tmp5D4=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5D1)->f2;_LL2A: {struct Cyc_List_List*es=_tmp5D4;
_tmp5D3=es;goto _LL2C;}case 24U: _LL2B: _tmp5D3=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL2C: {struct Cyc_List_List*es=_tmp5D3;
_tmp5D2=es;goto _LL2E;}case 31U: _LL2D: _tmp5D2=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5D1)->f1;_LL2E: {struct Cyc_List_List*es=_tmp5D2;
# 3152
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL2F: _LL30:
 return 0;}_LL0:;}
# 3159
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3163
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5ED=Cyc_Tcutil_compress(t);void*_stmttmp88=_tmp5ED;void*_tmp5EE=_stmttmp88;struct Cyc_List_List*_tmp5EF;struct Cyc_List_List*_tmp5F0;void*_tmp5F1;void*_tmp5F2;struct Cyc_List_List*_tmp5F4;void*_tmp5F3;switch(*((int*)_tmp5EE)){case 0U: _LL1: _tmp5F3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5EE)->f1;_tmp5F4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5EE)->f2;_LL2: {void*c=_tmp5F3;struct Cyc_List_List*ts=_tmp5F4;
# 3167
void*_tmp5F5=c;union Cyc_Absyn_AggrInfo _tmp5F6;struct Cyc_List_List*_tmp5F7;struct Cyc_Absyn_Enumdecl*_tmp5F8;switch(*((int*)_tmp5F5)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5F8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5F5)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5F8;
# 3172
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5F7=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5F7=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5F5)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5F7;
# 3176
{struct Cyc_List_List*_tmp5F9=fs;struct Cyc_List_List*fs2=_tmp5F9;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5FA=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp89=_tmp5FA;struct _tuple13 _tmp5FB=_stmttmp89;int _tmp5FD;unsigned _tmp5FC;_LL1D: _tmp5FC=_tmp5FB.f1;_tmp5FD=_tmp5FB.f2;_LL1E: {unsigned i=_tmp5FC;int known=_tmp5FD;
if(known && i == (unsigned)0)
return 1;}}}}
# 3183
return 0;}case 20U: _LL18: _tmp5F6=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5F5)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5F6;
# 3186
struct Cyc_Absyn_Aggrdecl*_tmp5FE=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5FE;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5EE)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5F2;
# 3194
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5F1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5EE)->f1).elt_type;_LL6: {void*t=_tmp5F1;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5F0=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5EE)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5F0;
# 3197
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))
return 0;}
return 1;}case 7U: _LL9: _tmp5EF=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5EE)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5EF;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3205
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3207
struct _RegionHandle _tmp5FF=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5FF;_push_region(rgn);
{struct Cyc_List_List*_tmp600=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp600;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp601=0;_npop_handler(0U);return _tmp601;}}{
# 3215
int _tmp602=1;_npop_handler(0U);return _tmp602;}}
# 3208
;_pop_region();}
# 3219
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp603=t;struct Cyc_Absyn_Typedefdecl*_tmp604;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp603)->tag == 8U){_LL1: _tmp604=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp603)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp604;
# 3222
if(tdopt == 0)
return;{
struct Cyc_Absyn_Tqual _tmp605=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp605;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp607=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B3;_tmp6B3.tag=0U,({struct _fat_ptr _tmp7F6=({const char*_tmp60B="qualifier within typedef type ";_tag_fat(_tmp60B,sizeof(char),31U);});_tmp6B3.f1=_tmp7F6;});_tmp6B3;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp608=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp6B2;_tmp6B2.tag=2U,_tmp6B2.f1=(void*)t;_tmp6B2;});struct Cyc_Warn_String_Warn_Warg_struct _tmp609=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B1;_tmp6B1.tag=0U,({struct _fat_ptr _tmp7F7=({const char*_tmp60A=" is ignored";_tag_fat(_tmp60A,sizeof(char),12U);});_tmp6B1.f1=_tmp7F7;});_tmp6B1;});void*_tmp606[3U];_tmp606[0]=& _tmp607,_tmp606[1]=& _tmp608,_tmp606[2]=& _tmp609;({unsigned _tmp7F8=loc;Cyc_Warn_warn2(_tmp7F8,_tag_fat(_tmp606,sizeof(void*),3U));});});
return;}}}else{_LL3: _LL4:
 return;}_LL0:;}
# 3234
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp60C=Cyc_Tcutil_compress(t);void*_stmttmp8A=_tmp60C;void*_tmp60D=_stmttmp8A;struct Cyc_List_List**_tmp60E;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp60D)->tag == 5U){_LL1: _tmp60E=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp60D)->f1).attributes;_LL2: {struct Cyc_List_List**fnatts=_tmp60E;
# 3237
struct Cyc_List_List*_tmp60F=0;struct Cyc_List_List*res_atts=_tmp60F;
for(0;atts != 0;atts=atts->tl){
if(!Cyc_Absyn_fntype_att((void*)atts->hd))
res_atts=({struct Cyc_List_List*_tmp610=_cycalloc(sizeof(*_tmp610));_tmp610->hd=(void*)atts->hd,_tmp610->tl=res_atts;_tmp610;});else{
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_tmp7F9=({struct Cyc_List_List*_tmp611=_cycalloc(sizeof(*_tmp611));_tmp611->hd=(void*)atts->hd,_tmp611->tl=*fnatts;_tmp611;});*fnatts=_tmp7F9;});}}
return res_atts;}}else{_LL3: _LL4:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp613=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B4;_tmp6B4.tag=0U,({struct _fat_ptr _tmp7FA=({const char*_tmp614="transfer_fn_type_atts";_tag_fat(_tmp614,sizeof(char),22U);});_tmp6B4.f1=_tmp7FA;});_tmp6B4;});void*_tmp612[1U];_tmp612[0]=& _tmp613;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp612,sizeof(void*),1U));});}_LL0:;}
# 3249
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp615=Cyc_Tcutil_compress(t);void*_stmttmp8B=_tmp615;void*_tmp616=_stmttmp8B;struct Cyc_Absyn_Exp*_tmp617;struct Cyc_Absyn_PtrInfo _tmp618;switch(*((int*)_tmp616)){case 3U: _LL1: _tmp618=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp616)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp618;
return({void*_tmp7FB=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7FB,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp617=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp616)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp617;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3259
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp619=b;struct Cyc_Absyn_Vardecl*_tmp61A;struct Cyc_Absyn_Vardecl*_tmp61B;struct Cyc_Absyn_Vardecl*_tmp61C;struct Cyc_Absyn_Vardecl*_tmp61D;switch(*((int*)_tmp619)){case 5U: _LL1: _tmp61D=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp619)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp61D;
_tmp61C=x;goto _LL4;}case 4U: _LL3: _tmp61C=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp619)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp61C;
_tmp61B=x;goto _LL6;}case 3U: _LL5: _tmp61B=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp619)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp61B;
_tmp61A=x;goto _LL8;}case 1U: _LL7: _tmp61A=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp619)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp61A;
return x->escapes?0: x;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3270
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp61E=0;struct Cyc_List_List*res=_tmp61E;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)res=({struct Cyc_List_List*_tmp61F=_cycalloc(sizeof(*_tmp61F));_tmp61F->hd=*((void**)_check_null((void**)x->hd)),_tmp61F->tl=res;_tmp61F;});}}
return res;}
# 3277
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp620=Cyc_Tcutil_compress(t);void*_stmttmp8C=_tmp620;void*_tmp621=_stmttmp8C;unsigned _tmp626;void*_tmp625;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_Absyn_Tqual _tmp623;void*_tmp622;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp621)->tag == 4U){_LL1: _tmp622=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp621)->f1).elt_type;_tmp623=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp621)->f1).tq;_tmp624=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp621)->f1).num_elts;_tmp625=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp621)->f1).zero_term;_tmp626=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp621)->f1).zt_loc;_LL2: {void*et=_tmp622;struct Cyc_Absyn_Tqual tq=_tmp623;struct Cyc_Absyn_Exp*eopt=_tmp624;void*zt=_tmp625;unsigned ztl=_tmp626;
# 3280
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
if(convert_tag){
if(eopt->topt == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp628=({struct Cyc_Warn_String_Warn_Warg_struct _tmp6B5;_tmp6B5.tag=0U,({struct _fat_ptr _tmp7FC=({const char*_tmp629="cannot convert tag without type!";_tag_fat(_tmp629,sizeof(char),33U);});_tmp6B5.f1=_tmp7FC;});_tmp6B5;});void*_tmp627[1U];_tmp627[0]=& _tmp628;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp627,sizeof(void*),1U));});{
void*_tmp62A=Cyc_Tcutil_compress((void*)_check_null(eopt->topt));void*_stmttmp8D=_tmp62A;void*_tmp62B=_stmttmp8D;void*_tmp62C;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp62B)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp62B)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp62B)->f2 != 0){_LL6: _tmp62C=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp62B)->f2)->hd;_LL7: {void*t=_tmp62C;
# 3289
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3294
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
