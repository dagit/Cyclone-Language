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
# 63 "absynpp.h"
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 65
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 76
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
# 281
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 299
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 302
int Cyc_Tcutil_zeroable_type(void*);
# 306
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 314
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 318
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 323
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
int Cyc_Tcutil_is_typedef_type(void*t){
void*_tmp16=Cyc_Tcutil_compress(t);void*_stmttmpA=_tmp16;void*_tmp17=_stmttmpA;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp17)->tag == 8U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 122
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 130
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp18=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmpB=_tmp18;void*_tmp19=_stmttmpB;void*_tmp1A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp19)->tag == 1U){_LL1: _LL2:
 return Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp1A=_tmp19;_LL4: {void*t=_tmp1A;
return Cyc_Tcutil_is_integral_type(t);}}_LL0:;}
# 138
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp1B=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmpC=_tmp1B;void*_tmp1C=_stmttmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 148
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1D=Cyc_Tcutil_compress(t);void*_stmttmpD=_tmp1D;void*_tmp1E=_stmttmpD;void*_tmp1F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->tag == 3U){_LL1: _tmp1F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1E)->f1).ptr_atts).zero_term;_LL2: {void*ztl=_tmp1F;
# 151
return Cyc_Tcutil_force_type2bool(0,ztl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 157
int Cyc_Tcutil_is_nullable_pointer_type(void*t,int def){
void*_tmp20=Cyc_Tcutil_compress(t);void*_stmttmpE=_tmp20;void*_tmp21=_stmttmpE;void*_tmp22;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->tag == 3U){_LL1: _tmp22=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp21)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp22;
# 160
return Cyc_Tcutil_force_type2bool(def,nbl);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 166
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp23=Cyc_Tcutil_compress(t);void*_stmttmpF=_tmp23;void*_tmp24=_stmttmpF;void*_tmp25;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->tag == 3U){_LL1: _tmp25=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp25;
# 169
return Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,b);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 176
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp26=Cyc_Tcutil_compress(t);void*_stmttmp10=_tmp26;void*_tmp27=_stmttmp10;void*_tmp29;void*_tmp28;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->tag == 3U){_LL1: _tmp28=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).elt_type;_tmp29=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp27)->f1).ptr_atts).bounds;_LL2: {void*elt_type=_tmp28;void*b=_tmp29;
# 179
if(Cyc_Unify_unify(b,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=elt_type;
return 1;}else{
# 183
return 0;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 190
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp2A=Cyc_Tcutil_compress(t);void*_stmttmp11=_tmp2A;void*_tmp2B=_stmttmp11;void*_tmp2D;void*_tmp2C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->tag == 3U){_LL1: _tmp2C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).elt_type;_tmp2D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B)->f1).ptr_atts).zero_term;_LL2: {void*elt_type=_tmp2C;void*zt=_tmp2D;
# 193
*elt_type_dest=elt_type;
return Cyc_Absyn_type2bool(0,zt);}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 202
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 204
void*_tmp2E=Cyc_Tcutil_compress(t);void*_stmttmp12=_tmp2E;void*_tmp2F=_stmttmp12;void*_tmp33;struct Cyc_Absyn_Exp*_tmp32;struct Cyc_Absyn_Tqual _tmp31;void*_tmp30;void*_tmp36;void*_tmp35;void*_tmp34;switch(*((int*)_tmp2F)){case 3U: _LL1: _tmp34=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp35=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).bounds;_tmp36=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F)->f1).ptr_atts).zero_term;_LL2: {void*elt=_tmp34;void*bnds=_tmp35;void*zt=_tmp36;
# 206
if(Cyc_Absyn_type2bool(0,zt)){
*ptr_type=t;
*elt_type=elt;
{void*_tmp37=Cyc_Tcutil_compress(bnds);void*_stmttmp13=_tmp37;void*_tmp38=_stmttmp13;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp38)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 213
return 1;}else{
return 0;}}case 4U: _LL3: _tmp30=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).elt_type;_tmp31=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).tq;_tmp32=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2F)->f1).zero_term;_LL4: {void*elt=_tmp30;struct Cyc_Absyn_Tqual tq=_tmp31;struct Cyc_Absyn_Exp*n=_tmp32;void*zt=_tmp33;
# 216
if(Cyc_Absyn_type2bool(0,zt)){
*elt_type=elt;
*is_fat=0;
({void*_tmp690=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp690;});
return 1;}else{
return 0;}}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 229
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 235
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp39=({void*_tmp691=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp691,b);});struct Cyc_Absyn_Exp*eopt=_tmp39;
if(eopt == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp3A=eopt;struct Cyc_Absyn_Exp*e=_tmp3A;
struct _tuple13 _tmp3B=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple13 _stmttmp14=_tmp3B;struct _tuple13 _tmp3C=_stmttmp14;int _tmp3E;unsigned _tmp3D;_LL1: _tmp3D=_tmp3C.f1;_tmp3E=_tmp3C.f2;_LL2: {unsigned i=_tmp3D;int known=_tmp3E;
return known && i == (unsigned)1;}}}
# 244
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3F=Cyc_Tcutil_compress(t);void*_stmttmp15=_tmp3F;void*_tmp40=_stmttmp15;struct Cyc_List_List*_tmp41;struct Cyc_List_List*_tmp42;void*_tmp44;void*_tmp43;struct Cyc_List_List*_tmp46;void*_tmp45;switch(*((int*)_tmp40)){case 0U: _LL1: _tmp45=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40)->f1;_tmp46=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp40)->f2;_LL2: {void*c=_tmp45;struct Cyc_List_List*ts=_tmp46;
# 247
void*_tmp47=c;struct Cyc_Absyn_Aggrdecl*_tmp48;switch(*((int*)_tmp47)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return 1;case 15U: _LL12: _LL13:
 goto _LL15;case 16U: _LL14: _LL15:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp47)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
 return 0;}else{_LL18: _tmp48=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp47)->f1).KnownAggr).val;_LL19: {struct Cyc_Absyn_Aggrdecl*ad=_tmp48;
# 255
if(ad->impl == 0)
return 0;{
int okay=1;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type)){okay=0;break;}}}
if(okay)return 1;{
struct _RegionHandle _tmp49=_new_region("rgn");struct _RegionHandle*rgn=& _tmp49;_push_region(rgn);
{struct Cyc_List_List*_tmp4A=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,ad->tvs,ts);struct Cyc_List_List*inst=_tmp4A;
{struct Cyc_List_List*fs=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))){int _tmp4B=0;_npop_handler(0U);return _tmp4B;}}}{
int _tmp4C=1;_npop_handler(0U);return _tmp4C;}}
# 262
;_pop_region();}}}}default: _LL1A: _LL1B:
# 266
 return 0;}_LLB:;}case 4U: _LL3: _tmp43=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40)->f1).elt_type;_tmp44=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp40)->f1).zero_term;_LL4: {void*t=_tmp43;void*zero_term=_tmp44;
# 271
return !Cyc_Absyn_type2bool(0,zero_term)&& Cyc_Tcutil_is_bits_only_type(t);}case 6U: _LL5: _tmp42=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp40)->f1;_LL6: {struct Cyc_List_List*tqs=_tmp42;
# 273
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple12*)tqs->hd)).f2))return 0;}
return 1;}case 7U: _LL7: _tmp41=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp40)->f2;_LL8: {struct Cyc_List_List*fs=_tmp41;
# 277
for(0;fs != 0;fs=fs->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)fs->hd)->type))return 0;}
return 1;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 286
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
void*_tmp4D=Cyc_Tcutil_compress(t);void*_stmttmp16=_tmp4D;void*_tmp4E=_stmttmp16;struct Cyc_List_List*_tmp4F;void*_tmp50;switch(*((int*)_tmp4E)){case 3U: _LL1: _tmp50=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4E)->f1).elt_type;_LL2: {void*elt=_tmp50;
return Cyc_Tcutil_is_noreturn_fn_type(elt);}case 5U: _LL3: _tmp4F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4E)->f1).attributes;_LL4: {struct Cyc_List_List*atts=_tmp4F;
# 290
for(0;atts != 0;atts=atts->tl){
void*_tmp51=(void*)atts->hd;void*_stmttmp17=_tmp51;void*_tmp52=_stmttmp17;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp52)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 295
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 302
int Cyc_Tcutil_warn_region_coerce=0;
# 304
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 306
Cyc_Warn_verr(loc,fmt,ap);}
# 308
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 310
Cyc_Warn_vimpos(fmt,ap);}
# 312
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 314
Cyc_Warn_vwarn(sg,fmt,ap);}
# 318
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 323
void*Cyc_Tcutil_compress(void*t){
void*_tmp53=t;void*_tmp54;struct Cyc_Absyn_Exp*_tmp55;struct Cyc_Absyn_Exp*_tmp56;void**_tmp57;void**_tmp58;switch(*((int*)_tmp53)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp53)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp58=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp53)->f2;_LL8: {void**t2opt_ref=_tmp58;
# 334
void*ta=(void*)_check_null(*t2opt_ref);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*t2opt_ref=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53)->f4 == 0){_LL3: _LL4:
# 326
 return t;}else{_LL5: _tmp57=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53)->f4;_LL6: {void**topt_ref=_tmp57;
# 328
void*ta=(void*)_check_null(*topt_ref);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*topt_ref=t2;
return t2;}}case 9U: _LL9: _tmp56=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp53)->f1;_LLA: {struct Cyc_Absyn_Exp*e=_tmp56;
# 340
Cyc_Evexp_eval_const_uint_exp(e);{
void*_tmp59=e->r;void*_stmttmp18=_tmp59;void*_tmp5A=_stmttmp18;void*_tmp5B;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5A)->tag == 39U){_LL12: _tmp5B=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp5A)->f1;_LL13: {void*t2=_tmp5B;
return Cyc_Tcutil_compress(t2);}}else{_LL14: _LL15:
 return t;}_LL11:;}}case 11U: _LLB: _tmp55=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp53)->f1;_LLC: {struct Cyc_Absyn_Exp*e=_tmp55;
# 346
void*_tmp5C=e->topt;void*t2=_tmp5C;
if(t2 != 0)return t2;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp53)->f2 != 0){_LLD: _tmp54=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp53)->f2;_LLE: {void*t=_tmp54;
return Cyc_Tcutil_compress(t);}}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 358
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 362
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp5D=Cyc_Absyn_compress_kb(kb);void*_stmttmp19=_tmp5D;void*_tmp5E=_stmttmp19;struct Cyc_Absyn_Kind*_tmp5F;switch(*((int*)_tmp5E)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp60=_cycalloc(sizeof(*_tmp60));_tmp60->tag=1U,_tmp60->f1=0;_tmp60;});case 2U: _LL3: _tmp5F=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp5E)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp5F;
return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp61=_cycalloc(sizeof(*_tmp61));_tmp61->tag=2U,_tmp61->f1=0,_tmp61->f2=k;_tmp61;});}default: _LL5: _LL6:
 return kb;}_LL0:;}
# 369
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 371
return({struct Cyc_Absyn_Tvar*_tmp62=_cycalloc(sizeof(*_tmp62));_tmp62->name=tv->name,_tmp62->identity=- 1,({void*_tmp692=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp62->kind=_tmp692;});_tmp62;});}
# 373
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 375
struct _tuple9*_tmp63=arg;void*_tmp66;struct Cyc_Absyn_Tqual _tmp65;struct _fat_ptr*_tmp64;_LL1: _tmp64=_tmp63->f1;_tmp65=_tmp63->f2;_tmp66=_tmp63->f3;_LL2: {struct _fat_ptr*x=_tmp64;struct Cyc_Absyn_Tqual y=_tmp65;void*t=_tmp66;
return({struct _tuple9*_tmp67=_cycalloc(sizeof(*_tmp67));_tmp67->f1=x,_tmp67->f2=y,({void*_tmp693=Cyc_Tcutil_copy_type(t);_tmp67->f3=_tmp693;});_tmp67;});}}
# 378
static struct _tuple12*Cyc_Tcutil_copy_tqt(struct _tuple12*arg){
return({struct _tuple12*_tmp68=_cycalloc(sizeof(*_tmp68));_tmp68->f1=(*arg).f1,({void*_tmp694=Cyc_Tcutil_copy_type((*arg).f2);_tmp68->f2=_tmp694;});_tmp68;});}
# 381
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}
# 385
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp69=_cycalloc(sizeof(*_tmp69));_tmp69->name=f->name,_tmp69->tq=f->tq,({void*_tmp696=Cyc_Tcutil_copy_type(f->type);_tmp69->type=_tmp696;}),_tmp69->width=f->width,_tmp69->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp695=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp69->requires_clause=_tmp695;});_tmp69;});}struct _tuple15{void*f1;void*f2;};
# 389
static struct _tuple15*Cyc_Tcutil_copy_rgncmp(struct _tuple15*x){
struct _tuple15*_tmp6A=x;void*_tmp6C;void*_tmp6B;_LL1: _tmp6B=_tmp6A->f1;_tmp6C=_tmp6A->f2;_LL2: {void*r1=_tmp6B;void*r2=_tmp6C;
return({struct _tuple15*_tmp6D=_cycalloc(sizeof(*_tmp6D));({void*_tmp698=Cyc_Tcutil_copy_type(r1);_tmp6D->f1=_tmp698;}),({void*_tmp697=Cyc_Tcutil_copy_type(r2);_tmp6D->f2=_tmp697;});_tmp6D;});}}
# 393
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 397
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp6E=Cyc_Tcutil_compress(t);void*_stmttmp1A=_tmp6E;void*_tmp6F=_stmttmp1A;struct Cyc_Absyn_Datatypedecl*_tmp70;struct Cyc_Absyn_Enumdecl*_tmp71;struct Cyc_Absyn_Aggrdecl*_tmp72;struct Cyc_Absyn_Typedefdecl*_tmp75;struct Cyc_List_List*_tmp74;struct _tuple1*_tmp73;struct Cyc_Absyn_Exp*_tmp76;struct Cyc_Absyn_Exp*_tmp77;struct Cyc_List_List*_tmp79;enum Cyc_Absyn_AggrKind _tmp78;struct Cyc_List_List*_tmp7A;struct Cyc_Absyn_Vardecl*_tmp88;struct Cyc_List_List*_tmp87;struct Cyc_Absyn_Exp*_tmp86;struct Cyc_List_List*_tmp85;struct Cyc_Absyn_Exp*_tmp84;struct Cyc_List_List*_tmp83;struct Cyc_List_List*_tmp82;struct Cyc_Absyn_VarargInfo*_tmp81;int _tmp80;struct Cyc_List_List*_tmp7F;void*_tmp7E;struct Cyc_Absyn_Tqual _tmp7D;void*_tmp7C;struct Cyc_List_List*_tmp7B;unsigned _tmp8D;void*_tmp8C;struct Cyc_Absyn_Exp*_tmp8B;struct Cyc_Absyn_Tqual _tmp8A;void*_tmp89;struct Cyc_Absyn_PtrLoc*_tmp94;void*_tmp93;void*_tmp92;void*_tmp91;void*_tmp90;struct Cyc_Absyn_Tqual _tmp8F;void*_tmp8E;struct Cyc_Absyn_Tvar*_tmp95;struct Cyc_List_List*_tmp97;void*_tmp96;void*_tmp98;switch(*((int*)_tmp6F)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6F)->f2 == 0){_LL1: _tmp98=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6F)->f1;_LL2: {void*c=_tmp98;
return t;}}else{_LL3: _tmp96=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6F)->f1;_tmp97=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6F)->f2;_LL4: {void*c=_tmp96;struct Cyc_List_List*ts=_tmp97;
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=0U,_tmp99->f1=c,({struct Cyc_List_List*_tmp699=Cyc_Tcutil_copy_types(ts);_tmp99->f2=_tmp699;});_tmp99;});}}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmp95=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp6F)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp95;
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(tv));}case 3U: _LL9: _tmp8E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).elt_type;_tmp8F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).elt_tq;_tmp90=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).ptr_atts).rgn;_tmp91=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).ptr_atts).nullable;_tmp92=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).ptr_atts).bounds;_tmp93=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).ptr_atts).zero_term;_tmp94=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6F)->f1).ptr_atts).ptrloc;_LLA: {void*elt=_tmp8E;struct Cyc_Absyn_Tqual tq=_tmp8F;void*rgn=_tmp90;void*nbl=_tmp91;void*bs=_tmp92;void*zt=_tmp93;struct Cyc_Absyn_PtrLoc*loc=_tmp94;
# 404
void*_tmp9A=Cyc_Tcutil_copy_type(elt);void*elt2=_tmp9A;
void*_tmp9B=Cyc_Tcutil_copy_type(rgn);void*rgn2=_tmp9B;
void*_tmp9C=Cyc_Tcutil_copy_type(nbl);void*nbl2=_tmp9C;
struct Cyc_Absyn_Tqual _tmp9D=tq;struct Cyc_Absyn_Tqual tq2=_tmp9D;
# 409
void*_tmp9E=Cyc_Tcutil_copy_type(bs);void*bs2=_tmp9E;
void*_tmp9F=Cyc_Tcutil_copy_type(zt);void*zt2=_tmp9F;
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmpA0=_cycalloc(sizeof(*_tmpA0));_tmpA0->tag=3U,(_tmpA0->f1).elt_type=elt2,(_tmpA0->f1).elt_tq=tq2,((_tmpA0->f1).ptr_atts).rgn=rgn2,((_tmpA0->f1).ptr_atts).nullable=nbl2,((_tmpA0->f1).ptr_atts).bounds=bs2,((_tmpA0->f1).ptr_atts).zero_term=zt2,((_tmpA0->f1).ptr_atts).ptrloc=loc;_tmpA0;});}case 4U: _LLB: _tmp89=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6F)->f1).elt_type;_tmp8A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6F)->f1).tq;_tmp8B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6F)->f1).num_elts;_tmp8C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6F)->f1).zero_term;_tmp8D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6F)->f1).zt_loc;_LLC: {void*et=_tmp89;struct Cyc_Absyn_Tqual tq=_tmp8A;struct Cyc_Absyn_Exp*eo=_tmp8B;void*zt=_tmp8C;unsigned ztl=_tmp8D;
# 413
return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmpA1=_cycalloc(sizeof(*_tmpA1));_tmpA1->tag=4U,({void*_tmp69C=Cyc_Tcutil_copy_type(et);(_tmpA1->f1).elt_type=_tmp69C;}),(_tmpA1->f1).tq=tq,({struct Cyc_Absyn_Exp*_tmp69B=Cyc_Tcutil_deep_copy_exp_opt(1,eo);(_tmpA1->f1).num_elts=_tmp69B;}),({
void*_tmp69A=Cyc_Tcutil_copy_type(zt);(_tmpA1->f1).zero_term=_tmp69A;}),(_tmpA1->f1).zt_loc=ztl;_tmpA1;});}case 5U: _LLD: _tmp7B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).tvars;_tmp7C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).effect;_tmp7D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).ret_tqual;_tmp7E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).ret_type;_tmp7F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).args;_tmp80=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).c_varargs;_tmp81=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).cyc_varargs;_tmp82=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).rgn_po;_tmp83=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).attributes;_tmp84=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).requires_clause;_tmp85=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).requires_relns;_tmp86=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).ensures_clause;_tmp87=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).ensures_relns;_tmp88=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6F)->f1).return_value;_LLE: {struct Cyc_List_List*tvs=_tmp7B;void*effopt=_tmp7C;struct Cyc_Absyn_Tqual rt_tq=_tmp7D;void*rt=_tmp7E;struct Cyc_List_List*args=_tmp7F;int c_varargs=_tmp80;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp81;struct Cyc_List_List*rgn_po=_tmp82;struct Cyc_List_List*atts=_tmp83;struct Cyc_Absyn_Exp*req=_tmp84;struct Cyc_List_List*req_rlns=_tmp85;struct Cyc_Absyn_Exp*ens=_tmp86;struct Cyc_List_List*ens_rlns=_tmp87;struct Cyc_Absyn_Vardecl*ret_val=_tmp88;
# 416
struct Cyc_List_List*_tmpA2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,tvs);struct Cyc_List_List*tvs2=_tmpA2;
void*effopt2=effopt == 0?0: Cyc_Tcutil_copy_type(effopt);
void*_tmpA3=Cyc_Tcutil_copy_type(rt);void*rt2=_tmpA3;
struct Cyc_List_List*_tmpA4=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,args);struct Cyc_List_List*args2=_tmpA4;
int _tmpA5=c_varargs;int c_varargs2=_tmpA5;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo*cv=cyc_varargs;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->name=cv->name,_tmpA6->tq=cv->tq,({void*_tmp69D=Cyc_Tcutil_copy_type(cv->type);_tmpA6->type=_tmp69D;}),_tmpA6->inject=cv->inject;_tmpA6;});}{
# 427
struct Cyc_List_List*_tmpA7=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(struct _tuple15*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,rgn_po);struct Cyc_List_List*rgn_po2=_tmpA7;
struct Cyc_List_List*_tmpA8=atts;struct Cyc_List_List*atts2=_tmpA8;
struct Cyc_Absyn_Exp*_tmpA9=Cyc_Tcutil_deep_copy_exp_opt(1,req);struct Cyc_Absyn_Exp*req2=_tmpA9;
struct Cyc_List_List*_tmpAA=Cyc_Relations_copy_relns(Cyc_Core_heap_region,req_rlns);struct Cyc_List_List*req_rlns2=_tmpAA;
struct Cyc_Absyn_Exp*_tmpAB=Cyc_Tcutil_deep_copy_exp_opt(1,ens);struct Cyc_Absyn_Exp*ens2=_tmpAB;
struct Cyc_List_List*_tmpAC=Cyc_Relations_copy_relns(Cyc_Core_heap_region,ens_rlns);struct Cyc_List_List*ens_rlns2=_tmpAC;
struct Cyc_Absyn_Vardecl*ret_val2=0;
if(ret_val != 0){
ret_val2=({struct Cyc_Absyn_Vardecl*_tmpAD=_cycalloc(sizeof(*_tmpAD));*_tmpAD=*ret_val;_tmpAD;});
({void*_tmp69E=Cyc_Tcutil_copy_type(ret_val2->type);ret_val2->type=_tmp69E;});
if((unsigned)ret_val2->rgn)({void*_tmp69F=Cyc_Tcutil_copy_type(ret_val2->rgn);ret_val2->rgn=_tmp69F;});}
# 439
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpAE=_cycalloc(sizeof(*_tmpAE));_tmpAE->tag=5U,(_tmpAE->f1).tvars=tvs2,(_tmpAE->f1).effect=effopt2,(_tmpAE->f1).ret_tqual=rt_tq,(_tmpAE->f1).ret_type=rt2,(_tmpAE->f1).args=args2,(_tmpAE->f1).c_varargs=c_varargs2,(_tmpAE->f1).cyc_varargs=cyc_varargs2,(_tmpAE->f1).rgn_po=rgn_po2,(_tmpAE->f1).attributes=atts2,(_tmpAE->f1).requires_clause=req2,(_tmpAE->f1).requires_relns=req_rlns2,(_tmpAE->f1).ensures_clause=ens2,(_tmpAE->f1).ensures_relns=ens_rlns2,(_tmpAE->f1).return_value=ret_val2;_tmpAE;});}}case 6U: _LLF: _tmp7A=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6F)->f1;_LL10: {struct Cyc_List_List*tqts=_tmp7A;
# 442
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpAF=_cycalloc(sizeof(*_tmpAF));_tmpAF->tag=6U,({struct Cyc_List_List*_tmp6A0=((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,tqts);_tmpAF->f1=_tmp6A0;});_tmpAF;});}case 7U: _LL11: _tmp78=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6F)->f1;_tmp79=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6F)->f2;_LL12: {enum Cyc_Absyn_AggrKind k=_tmp78;struct Cyc_List_List*fs=_tmp79;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpB0=_cycalloc(sizeof(*_tmpB0));_tmpB0->tag=7U,_tmpB0->f1=k,({struct Cyc_List_List*_tmp6A1=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,fs);_tmpB0->f2=_tmp6A1;});_tmpB0;});}case 9U: _LL13: _tmp77=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp6F)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp77;
return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpB1=_cycalloc(sizeof(*_tmpB1));_tmpB1->tag=9U,_tmpB1->f1=e;_tmpB1;});}case 11U: _LL15: _tmp76=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp6F)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp76;
return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpB2=_cycalloc(sizeof(*_tmpB2));_tmpB2->tag=11U,_tmpB2->f1=e;_tmpB2;});}case 8U: _LL17: _tmp73=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6F)->f1;_tmp74=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6F)->f2;_tmp75=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6F)->f3;_LL18: {struct _tuple1*tdn=_tmp73;struct Cyc_List_List*ts=_tmp74;struct Cyc_Absyn_Typedefdecl*td=_tmp75;
# 447
return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpB3=_cycalloc(sizeof(*_tmpB3));_tmpB3->tag=8U,_tmpB3->f1=tdn,({struct Cyc_List_List*_tmp6A2=Cyc_Tcutil_copy_types(ts);_tmpB3->f2=_tmp6A2;}),_tmpB3->f3=td,_tmpB3->f4=0;_tmpB3;});}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6F)->f1)->r)){case 0U: _LL19: _tmp72=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6F)->f1)->r)->f1;_LL1A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp72;
# 450
struct Cyc_List_List*_tmpB4=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,ad->tvs);struct Cyc_List_List*targs=_tmpB4;
return({union Cyc_Absyn_AggrInfo _tmp6A3=Cyc_Absyn_UnknownAggr(ad->kind,ad->name,0);Cyc_Absyn_aggr_type(_tmp6A3,targs);});}case 1U: _LL1B: _tmp71=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6F)->f1)->r)->f1;_LL1C: {struct Cyc_Absyn_Enumdecl*ed=_tmp71;
# 453
return Cyc_Absyn_enum_type(ed->name,0);}default: _LL1D: _tmp70=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp6F)->f1)->r)->f1;_LL1E: {struct Cyc_Absyn_Datatypedecl*dd=_tmp70;
# 455
struct Cyc_List_List*_tmpB5=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,dd->tvs);struct Cyc_List_List*targs=_tmpB5;
return({union Cyc_Absyn_DatatypeInfo _tmp6A4=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp623;_tmp623.name=dd->name,_tmp623.is_extensible=0;_tmp623;}));Cyc_Absyn_datatype_type(_tmp6A4,targs);});}}}_LL0:;}
# 462
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpB6=d;struct _fat_ptr*_tmpB7;struct Cyc_Absyn_Exp*_tmpB8;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB6)->tag == 0U){_LL1: _tmpB8=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpB6)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmpB8;
return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->tag=0U,({struct Cyc_Absyn_Exp*_tmp6A5=Cyc_Tcutil_deep_copy_exp(preserve_types,e);_tmpB9->f1=_tmp6A5;});_tmpB9;});}}else{_LL3: _tmpB7=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpB6)->f1;_LL4: {struct _fat_ptr*v=_tmpB7;
return d;}}_LL0:;}struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 468
static struct _tuple16*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple16*e){
# 470
return({struct _tuple16*_tmpBA=_cycalloc(sizeof(*_tmpBA));({struct Cyc_List_List*_tmp6A7=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpBA->f1=_tmp6A7;}),({struct Cyc_Absyn_Exp*_tmp6A6=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpBA->f2=_tmp6A6;});_tmpBA;});}
# 473
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpBB=preserve_types;int pt=_tmpBB;
{void*_tmpBC=e->r;void*_stmttmp1B=_tmpBC;void*_tmpBD=_stmttmp1B;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_List_List*_tmpC3;struct Cyc_List_List*_tmpC2;struct Cyc_List_List*_tmpC1;struct _fat_ptr _tmpC0;int _tmpBF;void*_tmpC4;struct _fat_ptr*_tmpC6;struct Cyc_Absyn_Exp*_tmpC5;struct Cyc_List_List*_tmpC8;struct Cyc_Core_Opt*_tmpC7;struct Cyc_Absyn_Exp*_tmpCA;struct Cyc_Absyn_Exp*_tmpC9;int _tmpD0;int _tmpCF;struct Cyc_Absyn_Exp*_tmpCE;void**_tmpCD;struct Cyc_Absyn_Exp*_tmpCC;int _tmpCB;struct Cyc_Absyn_Enumfield*_tmpD2;void*_tmpD1;struct Cyc_Absyn_Enumfield*_tmpD4;struct Cyc_Absyn_Enumdecl*_tmpD3;struct Cyc_List_List*_tmpD8;void*_tmpD7;struct Cyc_Absyn_Tqual _tmpD6;struct _fat_ptr*_tmpD5;struct Cyc_List_List*_tmpDA;void*_tmpD9;struct Cyc_Absyn_Aggrdecl*_tmpDE;struct Cyc_List_List*_tmpDD;struct Cyc_List_List*_tmpDC;struct _tuple1*_tmpDB;int _tmpE1;void*_tmpE0;struct Cyc_Absyn_Exp*_tmpDF;int _tmpE5;struct Cyc_Absyn_Exp*_tmpE4;struct Cyc_Absyn_Exp*_tmpE3;struct Cyc_Absyn_Vardecl*_tmpE2;struct Cyc_Absyn_Datatypefield*_tmpE8;struct Cyc_Absyn_Datatypedecl*_tmpE7;struct Cyc_List_List*_tmpE6;struct Cyc_List_List*_tmpE9;struct Cyc_Absyn_Exp*_tmpEB;struct Cyc_Absyn_Exp*_tmpEA;int _tmpEF;int _tmpEE;struct _fat_ptr*_tmpED;struct Cyc_Absyn_Exp*_tmpEC;int _tmpF3;int _tmpF2;struct _fat_ptr*_tmpF1;struct Cyc_Absyn_Exp*_tmpF0;struct Cyc_List_List*_tmpF4;struct Cyc_Absyn_Exp*_tmpF5;struct Cyc_Absyn_Exp*_tmpF6;struct Cyc_List_List*_tmpF8;void*_tmpF7;struct Cyc_Absyn_Exp*_tmpF9;void*_tmpFA;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;enum Cyc_Absyn_Coercion _tmp101;int _tmp100;struct Cyc_Absyn_Exp*_tmpFF;void*_tmpFE;struct Cyc_List_List*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp104;int _tmp106;struct Cyc_Absyn_Exp*_tmp105;int _tmp10A;struct Cyc_Absyn_VarargCallInfo*_tmp109;struct Cyc_List_List*_tmp108;struct Cyc_Absyn_Exp*_tmp107;int _tmp113;int _tmp112;void*_tmp111;struct Cyc_Absyn_Tqual _tmp110;struct _fat_ptr*_tmp10F;struct Cyc_List_List*_tmp10E;int _tmp10D;struct Cyc_List_List*_tmp10C;struct Cyc_Absyn_Exp*_tmp10B;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Core_Opt*_tmp115;struct Cyc_Absyn_Exp*_tmp114;struct Cyc_Absyn_Exp*_tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_Absyn_Exp*_tmp117;struct Cyc_Absyn_Exp*_tmp11B;struct Cyc_Absyn_Exp*_tmp11A;struct Cyc_Absyn_Exp*_tmp11D;struct Cyc_Absyn_Exp*_tmp11C;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;enum Cyc_Absyn_Incrementor _tmp121;struct Cyc_Absyn_Exp*_tmp120;struct Cyc_List_List*_tmp123;enum Cyc_Absyn_Primop _tmp122;struct _fat_ptr _tmp124;void*_tmp125;union Cyc_Absyn_Cnst _tmp126;switch(*((int*)_tmpBD)){case 0U: _LL1: _tmp126=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL2: {union Cyc_Absyn_Cnst c=_tmp126;
new_e=Cyc_Absyn_const_exp(c,e->loc);goto _LL0;}case 1U: _LL3: _tmp125=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL4: {void*b=_tmp125;
new_e=Cyc_Absyn_varb_exp(b,e->loc);goto _LL0;}case 2U: _LL5: _tmp124=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL6: {struct _fat_ptr p=_tmp124;
new_e=Cyc_Absyn_pragma_exp(p,e->loc);goto _LL0;}case 3U: _LL7: _tmp122=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp123=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL8: {enum Cyc_Absyn_Primop p=_tmp122;struct Cyc_List_List*es=_tmp123;
new_e=({enum Cyc_Absyn_Primop _tmp6A9=p;struct Cyc_List_List*_tmp6A8=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_primop_exp(_tmp6A9,_tmp6A8,e->loc);});goto _LL0;}case 5U: _LL9: _tmp120=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp121=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp120;enum Cyc_Absyn_Incrementor i=_tmp121;
new_e=({struct Cyc_Absyn_Exp*_tmp6AB=Cyc_Tcutil_deep_copy_exp(pt,e1);enum Cyc_Absyn_Incrementor _tmp6AA=i;Cyc_Absyn_increment_exp(_tmp6AB,_tmp6AA,e->loc);});goto _LL0;}case 7U: _LLB: _tmp11E=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp11F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp11E;struct Cyc_Absyn_Exp*e2=_tmp11F;
new_e=({struct Cyc_Absyn_Exp*_tmp6AD=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6AC=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_and_exp(_tmp6AD,_tmp6AC,e->loc);});goto _LL0;}case 8U: _LLD: _tmp11C=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp11D=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp11C;struct Cyc_Absyn_Exp*e2=_tmp11D;
new_e=({struct Cyc_Absyn_Exp*_tmp6AF=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6AE=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_or_exp(_tmp6AF,_tmp6AE,e->loc);});goto _LL0;}case 9U: _LLF: _tmp11A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp11B=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp11A;struct Cyc_Absyn_Exp*e2=_tmp11B;
new_e=({struct Cyc_Absyn_Exp*_tmp6B1=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6B0=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_seq_exp(_tmp6B1,_tmp6B0,e->loc);});goto _LL0;}case 6U: _LL11: _tmp117=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp118=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp119=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp117;struct Cyc_Absyn_Exp*e2=_tmp118;struct Cyc_Absyn_Exp*e3=_tmp119;
# 486
new_e=({struct Cyc_Absyn_Exp*_tmp6B4=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6B3=Cyc_Tcutil_deep_copy_exp(pt,e2);struct Cyc_Absyn_Exp*_tmp6B2=Cyc_Tcutil_deep_copy_exp(pt,e3);Cyc_Absyn_conditional_exp(_tmp6B4,_tmp6B3,_tmp6B2,e->loc);});goto _LL0;}case 4U: _LL13: _tmp114=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp115=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp116=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp114;struct Cyc_Core_Opt*po=_tmp115;struct Cyc_Absyn_Exp*e2=_tmp116;
# 488
new_e=({struct Cyc_Absyn_Exp*_tmp6B7=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Core_Opt*_tmp6B6=(unsigned)po?({struct Cyc_Core_Opt*_tmp127=_cycalloc(sizeof(*_tmp127));_tmp127->v=(void*)po->v;_tmp127;}): 0;struct Cyc_Absyn_Exp*_tmp6B5=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_assignop_exp(_tmp6B7,_tmp6B6,_tmp6B5,e->loc);});
goto _LL0;}case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3 != 0){_LL15: _tmp10B=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp10C=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp10D=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->num_varargs;_tmp10E=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->injectors;_tmp10F=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->vai)->name;_tmp110=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->vai)->tq;_tmp111=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->vai)->type;_tmp112=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3)->vai)->inject;_tmp113=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL16: {struct Cyc_Absyn_Exp*e1=_tmp10B;struct Cyc_List_List*es=_tmp10C;int n=_tmp10D;struct Cyc_List_List*is=_tmp10E;struct _fat_ptr*nm=_tmp10F;struct Cyc_Absyn_Tqual tq=_tmp110;void*t=_tmp111;int i=_tmp112;int resolved=_tmp113;
# 491
new_e=({void*_tmp6BD=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12A=_cycalloc(sizeof(*_tmp12A));_tmp12A->tag=10U,({
struct Cyc_Absyn_Exp*_tmp6BC=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12A->f1=_tmp6BC;}),({struct Cyc_List_List*_tmp6BB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp12A->f2=_tmp6BB;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp6BA=({struct Cyc_Absyn_VarargCallInfo*_tmp129=_cycalloc(sizeof(*_tmp129));_tmp129->num_varargs=n,_tmp129->injectors=is,({
struct Cyc_Absyn_VarargInfo*_tmp6B9=({struct Cyc_Absyn_VarargInfo*_tmp128=_cycalloc(sizeof(*_tmp128));_tmp128->name=nm,_tmp128->tq=tq,({void*_tmp6B8=Cyc_Tcutil_copy_type(t);_tmp128->type=_tmp6B8;}),_tmp128->inject=i;_tmp128;});_tmp129->vai=_tmp6B9;});_tmp129;});
# 493
_tmp12A->f3=_tmp6BA;}),_tmp12A->f4=resolved;_tmp12A;});
# 491
Cyc_Absyn_new_exp(_tmp6BD,e->loc);});
# 496
goto _LL0;}}else{_LL17: _tmp107=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp108=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp109=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmp10A=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL18: {struct Cyc_Absyn_Exp*e1=_tmp107;struct Cyc_List_List*es=_tmp108;struct Cyc_Absyn_VarargCallInfo*vci=_tmp109;int resolved=_tmp10A;
# 498
new_e=({void*_tmp6C0=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmp12B=_cycalloc(sizeof(*_tmp12B));_tmp12B->tag=10U,({struct Cyc_Absyn_Exp*_tmp6BF=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12B->f1=_tmp6BF;}),({struct Cyc_List_List*_tmp6BE=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp12B->f2=_tmp6BE;}),_tmp12B->f3=vci,_tmp12B->f4=resolved;_tmp12B;});Cyc_Absyn_new_exp(_tmp6C0,e->loc);});
goto _LL0;}}case 11U: _LL19: _tmp105=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp106=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp105;int b=_tmp106;
# 501
new_e=b?({struct Cyc_Absyn_Exp*_tmp6C2=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_rethrow_exp(_tmp6C2,e->loc);}):({struct Cyc_Absyn_Exp*_tmp6C1=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_throw_exp(_tmp6C1,e->loc);});
goto _LL0;}case 12U: _LL1B: _tmp104=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp104;
# 504
new_e=({struct Cyc_Absyn_Exp*_tmp6C3=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_noinstantiate_exp(_tmp6C3,e->loc);});goto _LL0;}case 13U: _LL1D: _tmp102=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmp103=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL1E: {struct Cyc_Absyn_Exp*e1=_tmp102;struct Cyc_List_List*ts=_tmp103;
# 506
new_e=({struct Cyc_Absyn_Exp*_tmp6C5=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_List_List*_tmp6C4=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);Cyc_Absyn_instantiate_exp(_tmp6C5,_tmp6C4,e->loc);});goto _LL0;}case 14U: _LL1F: _tmpFE=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpFF=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmp100=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmp101=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL20: {void*t=_tmpFE;struct Cyc_Absyn_Exp*e1=_tmpFF;int b=_tmp100;enum Cyc_Absyn_Coercion c=_tmp101;
# 508
new_e=({void*_tmp6C9=Cyc_Tcutil_copy_type(t);struct Cyc_Absyn_Exp*_tmp6C8=Cyc_Tcutil_deep_copy_exp(pt,e1);int _tmp6C7=b;enum Cyc_Absyn_Coercion _tmp6C6=c;Cyc_Absyn_cast_exp(_tmp6C9,_tmp6C8,_tmp6C7,_tmp6C6,e->loc);});goto _LL0;}case 16U: _LL21: _tmpFC=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpFD=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL22: {struct Cyc_Absyn_Exp*eo=_tmpFC;struct Cyc_Absyn_Exp*e1=_tmpFD;
# 510
new_e=({struct Cyc_Absyn_Exp*_tmp6CB=(unsigned)eo?Cyc_Tcutil_deep_copy_exp(pt,eo): 0;struct Cyc_Absyn_Exp*_tmp6CA=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_New_exp(_tmp6CB,_tmp6CA,e->loc);});goto _LL0;}case 15U: _LL23: _tmpFB=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL24: {struct Cyc_Absyn_Exp*e1=_tmpFB;
new_e=({struct Cyc_Absyn_Exp*_tmp6CC=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_address_exp(_tmp6CC,e->loc);});goto _LL0;}case 17U: _LL25: _tmpFA=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL26: {void*t=_tmpFA;
new_e=({void*_tmp6CD=Cyc_Tcutil_copy_type(t);Cyc_Absyn_sizeoftype_exp(_tmp6CD,e->loc);});goto _LL0;}case 18U: _LL27: _tmpF9=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL28: {struct Cyc_Absyn_Exp*e1=_tmpF9;
new_e=({struct Cyc_Absyn_Exp*_tmp6CE=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_sizeofexp_exp(_tmp6CE,e->loc);});goto _LL0;}case 19U: _LL29: _tmpF7=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpF8=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL2A: {void*t=_tmpF7;struct Cyc_List_List*ofs=_tmpF8;
new_e=({void*_tmp6D0=Cyc_Tcutil_copy_type(t);struct Cyc_List_List*_tmp6CF=ofs;Cyc_Absyn_offsetof_exp(_tmp6D0,_tmp6CF,e->loc);});goto _LL0;}case 20U: _LL2B: _tmpF6=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL2C: {struct Cyc_Absyn_Exp*e1=_tmpF6;
new_e=({struct Cyc_Absyn_Exp*_tmp6D1=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_deref_exp(_tmp6D1,e->loc);});goto _LL0;}case 41U: _LL2D: _tmpF5=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL2E: {struct Cyc_Absyn_Exp*e1=_tmpF5;
new_e=({struct Cyc_Absyn_Exp*_tmp6D2=Cyc_Tcutil_deep_copy_exp(pt,e1);Cyc_Absyn_extension_exp(_tmp6D2,e->loc);});goto _LL0;}case 24U: _LL2F: _tmpF4=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL30: {struct Cyc_List_List*es=_tmpF4;
new_e=({struct Cyc_List_List*_tmp6D3=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);Cyc_Absyn_tuple_exp(_tmp6D3,e->loc);});goto _LL0;}case 21U: _LL31: _tmpF0=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpF1=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpF2=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpF3=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL32: {struct Cyc_Absyn_Exp*e1=_tmpF0;struct _fat_ptr*n=_tmpF1;int f1=_tmpF2;int f2=_tmpF3;
# 519
new_e=({void*_tmp6D5=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmp12C=_cycalloc(sizeof(*_tmp12C));_tmp12C->tag=21U,({struct Cyc_Absyn_Exp*_tmp6D4=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12C->f1=_tmp6D4;}),_tmp12C->f2=n,_tmp12C->f3=f1,_tmp12C->f4=f2;_tmp12C;});Cyc_Absyn_new_exp(_tmp6D5,e->loc);});goto _LL0;}case 22U: _LL33: _tmpEC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpED=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpEE=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpEF=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL34: {struct Cyc_Absyn_Exp*e1=_tmpEC;struct _fat_ptr*n=_tmpED;int f1=_tmpEE;int f2=_tmpEF;
# 521
new_e=({void*_tmp6D7=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmp12D=_cycalloc(sizeof(*_tmp12D));_tmp12D->tag=22U,({struct Cyc_Absyn_Exp*_tmp6D6=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp12D->f1=_tmp6D6;}),_tmp12D->f2=n,_tmp12D->f3=f1,_tmp12D->f4=f2;_tmp12D;});Cyc_Absyn_new_exp(_tmp6D7,e->loc);});goto _LL0;}case 23U: _LL35: _tmpEA=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpEB=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL36: {struct Cyc_Absyn_Exp*e1=_tmpEA;struct Cyc_Absyn_Exp*e2=_tmpEB;
# 523
new_e=({struct Cyc_Absyn_Exp*_tmp6D9=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6D8=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_subscript_exp(_tmp6D9,_tmp6D8,e->loc);});goto _LL0;}case 26U: _LL37: _tmpE9=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL38: {struct Cyc_List_List*eds=_tmpE9;
# 525
new_e=({void*_tmp6DB=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmp12E=_cycalloc(sizeof(*_tmp12E));_tmp12E->tag=26U,({struct Cyc_List_List*_tmp6DA=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp12E->f1=_tmp6DA;});_tmp12E;});Cyc_Absyn_new_exp(_tmp6DB,e->loc);});goto _LL0;}case 31U: _LL39: _tmpE6=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpE7=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpE8=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_LL3A: {struct Cyc_List_List*es=_tmpE6;struct Cyc_Absyn_Datatypedecl*dtd=_tmpE7;struct Cyc_Absyn_Datatypefield*dtf=_tmpE8;
# 527
new_e=({void*_tmp6DD=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmp12F=_cycalloc(sizeof(*_tmp12F));_tmp12F->tag=31U,({struct Cyc_List_List*_tmp6DC=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,pt,es);_tmp12F->f1=_tmp6DC;}),_tmp12F->f2=dtd,_tmp12F->f3=dtf;_tmp12F;});Cyc_Absyn_new_exp(_tmp6DD,e->loc);});goto _LL0;}case 27U: _LL3B: _tmpE2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpE3=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpE4=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpE5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL3C: {struct Cyc_Absyn_Vardecl*vd=_tmpE2;struct Cyc_Absyn_Exp*e1=_tmpE3;struct Cyc_Absyn_Exp*e2=_tmpE4;int b=_tmpE5;
# 529
new_e=({void*_tmp6E0=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmp130=_cycalloc(sizeof(*_tmp130));_tmp130->tag=27U,_tmp130->f1=vd,({struct Cyc_Absyn_Exp*_tmp6DF=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp130->f2=_tmp6DF;}),({struct Cyc_Absyn_Exp*_tmp6DE=Cyc_Tcutil_deep_copy_exp(pt,e2);_tmp130->f3=_tmp6DE;}),_tmp130->f4=b;_tmp130;});Cyc_Absyn_new_exp(_tmp6E0,e->loc);});goto _LL0;}case 28U: _LL3D: _tmpDF=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpE0=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpE1=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_LL3E: {struct Cyc_Absyn_Exp*e=_tmpDF;void*t=_tmpE0;int b=_tmpE1;
# 531
new_e=({void*_tmp6E3=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmp131=_cycalloc(sizeof(*_tmp131));_tmp131->tag=28U,({struct Cyc_Absyn_Exp*_tmp6E2=Cyc_Tcutil_deep_copy_exp(pt,e);_tmp131->f1=_tmp6E2;}),({void*_tmp6E1=Cyc_Tcutil_copy_type(t);_tmp131->f2=_tmp6E1;}),_tmp131->f3=b;_tmp131;});Cyc_Absyn_new_exp(_tmp6E3,e->loc);});
goto _LL0;}case 29U: _LL3F: _tmpDB=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpDC=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpDD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpDE=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_LL40: {struct _tuple1*n=_tmpDB;struct Cyc_List_List*ts=_tmpDC;struct Cyc_List_List*eds=_tmpDD;struct Cyc_Absyn_Aggrdecl*agr=_tmpDE;
# 534
new_e=({void*_tmp6E6=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmp132=_cycalloc(sizeof(*_tmp132));_tmp132->tag=29U,_tmp132->f1=n,({struct Cyc_List_List*_tmp6E5=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);_tmp132->f2=_tmp6E5;}),({struct Cyc_List_List*_tmp6E4=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp132->f3=_tmp6E4;}),_tmp132->f4=agr;_tmp132;});Cyc_Absyn_new_exp(_tmp6E6,e->loc);});
# 536
goto _LL0;}case 30U: _LL41: _tmpD9=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpDA=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL42: {void*t=_tmpD9;struct Cyc_List_List*eds=_tmpDA;
# 538
new_e=({void*_tmp6E9=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmp133=_cycalloc(sizeof(*_tmp133));_tmp133->tag=30U,({void*_tmp6E8=Cyc_Tcutil_copy_type(t);_tmp133->f1=_tmp6E8;}),({struct Cyc_List_List*_tmp6E7=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp133->f2=_tmp6E7;});_tmp133;});Cyc_Absyn_new_exp(_tmp6E9,e->loc);});
goto _LL0;}case 25U: _LL43: _tmpD5=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBD)->f1)->f1;_tmpD6=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBD)->f1)->f2;_tmpD7=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBD)->f1)->f3;_tmpD8=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL44: {struct _fat_ptr*vopt=_tmpD5;struct Cyc_Absyn_Tqual tq=_tmpD6;void*t=_tmpD7;struct Cyc_List_List*eds=_tmpD8;
# 541
new_e=({void*_tmp6ED=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmp135=_cycalloc(sizeof(*_tmp135));_tmp135->tag=25U,({struct _tuple9*_tmp6EC=({struct _tuple9*_tmp134=_cycalloc(sizeof(*_tmp134));_tmp134->f1=vopt,_tmp134->f2=tq,({void*_tmp6EB=Cyc_Tcutil_copy_type(t);_tmp134->f3=_tmp6EB;});_tmp134;});_tmp135->f1=_tmp6EC;}),({
struct Cyc_List_List*_tmp6EA=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp135->f2=_tmp6EA;});_tmp135;});
# 541
Cyc_Absyn_new_exp(_tmp6ED,e->loc);});
# 543
goto _LL0;}case 32U: _LL45: _tmpD3=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpD4=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL46: {struct Cyc_Absyn_Enumdecl*ed=_tmpD3;struct Cyc_Absyn_Enumfield*ef=_tmpD4;
new_e=e;goto _LL0;}case 33U: _LL47: _tmpD1=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpD2=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL48: {void*t=_tmpD1;struct Cyc_Absyn_Enumfield*ef=_tmpD2;
# 546
new_e=({void*_tmp6EF=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmp136=_cycalloc(sizeof(*_tmp136));_tmp136->tag=33U,({void*_tmp6EE=Cyc_Tcutil_copy_type(t);_tmp136->f1=_tmp6EE;}),_tmp136->f2=ef;_tmp136;});Cyc_Absyn_new_exp(_tmp6EF,e->loc);});goto _LL0;}case 34U: _LL49: _tmpCB=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).is_calloc;_tmpCC=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).rgn;_tmpCD=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).elt_type;_tmpCE=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).num_elts;_tmpCF=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).fat_result;_tmpD0=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpBD)->f1).inline_call;_LL4A: {int ic=_tmpCB;struct Cyc_Absyn_Exp*r=_tmpCC;void**t=_tmpCD;struct Cyc_Absyn_Exp*n=_tmpCE;int res=_tmpCF;int inlc=_tmpD0;
# 548
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=r;if(r != 0)r1=Cyc_Tcutil_deep_copy_exp(pt,r);{
void**t1=t;if(t != 0)t1=({void**_tmp137=_cycalloc(sizeof(*_tmp137));({void*_tmp6F0=Cyc_Tcutil_copy_type(*t);*_tmp137=_tmp6F0;});_tmp137;});
({void*_tmp6F1=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmp138=_cycalloc(sizeof(*_tmp138));_tmp138->tag=34U,(_tmp138->f1).is_calloc=ic,(_tmp138->f1).rgn=r1,(_tmp138->f1).elt_type=t1,(_tmp138->f1).num_elts=n,(_tmp138->f1).fat_result=res,(_tmp138->f1).inline_call=inlc;_tmp138;});e2->r=_tmp6F1;});
new_e=e2;
goto _LL0;}}case 35U: _LL4B: _tmpC9=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpCA=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL4C: {struct Cyc_Absyn_Exp*e1=_tmpC9;struct Cyc_Absyn_Exp*e2=_tmpCA;
new_e=({struct Cyc_Absyn_Exp*_tmp6F3=Cyc_Tcutil_deep_copy_exp(pt,e1);struct Cyc_Absyn_Exp*_tmp6F2=Cyc_Tcutil_deep_copy_exp(pt,e2);Cyc_Absyn_swap_exp(_tmp6F3,_tmp6F2,e->loc);});goto _LL0;}case 36U: _LL4D: _tmpC7=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpC8=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL4E: {struct Cyc_Core_Opt*nopt=_tmpC7;struct Cyc_List_List*eds=_tmpC8;
# 556
struct Cyc_Core_Opt*nopt1=nopt;
if(nopt != 0)nopt1=({struct Cyc_Core_Opt*_tmp139=_cycalloc(sizeof(*_tmp139));_tmp139->v=(struct _tuple1*)nopt->v;_tmp139;});
new_e=({void*_tmp6F5=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmp13A=_cycalloc(sizeof(*_tmp13A));_tmp13A->tag=36U,_tmp13A->f1=nopt1,({struct Cyc_List_List*_tmp6F4=((struct Cyc_List_List*(*)(struct _tuple16*(*f)(int,struct _tuple16*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,pt,eds);_tmp13A->f2=_tmp6F4;});_tmp13A;});Cyc_Absyn_new_exp(_tmp6F5,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 561
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp13C=_cycalloc(sizeof(*_tmp13C));_tmp13C->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp6F6=({const char*_tmp13B="deep_copy: statement expressions unsupported";_tag_fat(_tmp13B,sizeof(char),45U);});_tmp13C->f1=_tmp6F6;});_tmp13C;}));case 38U: _LL51: _tmpC5=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpC6=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_LL52: {struct Cyc_Absyn_Exp*e1=_tmpC5;struct _fat_ptr*fn=_tmpC6;
new_e=({void*_tmp6F8=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmp13D=_cycalloc(sizeof(*_tmp13D));_tmp13D->tag=38U,({struct Cyc_Absyn_Exp*_tmp6F7=Cyc_Tcutil_deep_copy_exp(pt,e1);_tmp13D->f1=_tmp6F7;}),_tmp13D->f2=fn;_tmp13D;});Cyc_Absyn_new_exp(_tmp6F8,e->loc);});
goto _LL0;}case 39U: _LL53: _tmpC4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL54: {void*t=_tmpC4;
new_e=({void*_tmp6F9=Cyc_Tcutil_copy_type(t);Cyc_Absyn_valueof_exp(_tmp6F9,e->loc);});
goto _LL0;}case 40U: _LL55: _tmpBF=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_tmpC0=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f2;_tmpC1=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f3;_tmpC2=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f4;_tmpC3=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpBD)->f5;_LL56: {int v=_tmpBF;struct _fat_ptr t=_tmpC0;struct Cyc_List_List*o=_tmpC1;struct Cyc_List_List*i=_tmpC2;struct Cyc_List_List*c=_tmpC3;
new_e=Cyc_Absyn_asm_exp(v,t,o,i,c,e->loc);goto _LL0;}default: _LL57: _tmpBE=((struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct*)_tmpBD)->f1;_LL58: {struct Cyc_Absyn_Exp*e1=_tmpBE;
new_e=Cyc_Absyn_assert_exp(e1,e->loc);goto _LL0;}}_LL0:;}
# 570
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 574
return new_e;}struct _tuple17{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple18{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 585 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp13E=ka1;enum Cyc_Absyn_AliasQual _tmp140;enum Cyc_Absyn_KindQual _tmp13F;_LL1: _tmp13F=_tmp13E->kind;_tmp140=_tmp13E->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k1=_tmp13F;enum Cyc_Absyn_AliasQual a1=_tmp140;
struct Cyc_Absyn_Kind*_tmp141=ka2;enum Cyc_Absyn_AliasQual _tmp143;enum Cyc_Absyn_KindQual _tmp142;_LL4: _tmp142=_tmp141->kind;_tmp143=_tmp141->aliasqual;_LL5: {enum Cyc_Absyn_KindQual k2=_tmp142;enum Cyc_Absyn_AliasQual a2=_tmp143;
# 589
if((int)k1 != (int)k2){
struct _tuple17 _tmp144=({struct _tuple17 _tmp624;_tmp624.f1=k1,_tmp624.f2=k2;_tmp624;});struct _tuple17 _stmttmp1C=_tmp144;struct _tuple17 _tmp145=_stmttmp1C;switch(_tmp145.f1){case Cyc_Absyn_BoxKind: switch(_tmp145.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp145.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 597
if((int)a1 != (int)a2){
struct _tuple18 _tmp146=({struct _tuple18 _tmp625;_tmp625.f1=a1,_tmp625.f2=a2;_tmp625;});struct _tuple18 _stmttmp1D=_tmp146;struct _tuple18 _tmp147=_stmttmp1D;switch(_tmp147.f1){case Cyc_Absyn_Aliasable: if(_tmp147.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp147.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 603
return 1;}}}
# 606
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp148=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp1E=_tmp148;void*_tmp149=_stmttmp1E;struct Cyc_Absyn_Kind*_tmp14A;struct Cyc_Absyn_Kind*_tmp14B;switch(*((int*)_tmp149)){case 0U: _LL1: _tmp14B=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp149)->f1;_LL2: {struct Cyc_Absyn_Kind*k=_tmp14B;
return k;}case 2U: _LL3: _tmp14A=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp149)->f2;_LL4: {struct Cyc_Absyn_Kind*k=_tmp14A;
return k;}default: _LL5: _LL6:
({void*_tmp6FA=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp14C=_cycalloc(sizeof(*_tmp14C));_tmp14C->tag=2U,_tmp14C->f1=0,_tmp14C->f2=def;_tmp14C;});tv->kind=_tmp6FA;});return def;}_LL0:;}struct _tuple19{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 614
struct _tuple19 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp14D=Cyc_Tcutil_compress(t);void*_stmttmp1F=_tmp14D;void*_tmp14E=_stmttmp1F;struct Cyc_Absyn_Tvar*_tmp14F;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->tag == 2U){_LL1: _tmp14F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp14E)->f1;_LL2: {struct Cyc_Absyn_Tvar*tv=_tmp14F;
# 617
void*_tmp150=tv->kind;void*oldkb=_tmp150;
tv->kind=kb;
return({struct _tuple19 _tmp626;_tmp626.f1=tv,_tmp626.f2=oldkb;_tmp626;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp153=({struct Cyc_String_pa_PrintArg_struct _tmp627;_tmp627.tag=0U,({struct _fat_ptr _tmp6FB=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp627.f1=_tmp6FB;});_tmp627;});void*_tmp151[1U];_tmp151[0]=& _tmp153;({struct _fat_ptr _tmp6FC=({const char*_tmp152="swap_kind: cannot update the kind of %s";_tag_fat(_tmp152,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp6FC,_tag_fat(_tmp151,sizeof(void*),1U));});});}_LL0:;}
# 626
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 628
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 632
struct _RegionHandle _tmp154=_new_region("temp");struct _RegionHandle*temp=& _tmp154;_push_region(temp);
{struct Cyc_List_List*_tmp155=0;struct Cyc_List_List*inst=_tmp155;
# 635
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp156=(struct Cyc_Absyn_Tvar*)tvs->hd;struct Cyc_Absyn_Tvar*tv=_tmp156;
void*_tmp157=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;void*t=_tmp157;
struct Cyc_Absyn_Kind*_tmp158=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp20=_tmp158;struct Cyc_Absyn_Kind*_tmp159=_stmttmp20;switch(((struct Cyc_Absyn_Kind*)_tmp159)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 641
 inst=({struct Cyc_List_List*_tmp15B=_region_malloc(temp,sizeof(*_tmp15B));({struct _tuple19*_tmp6FD=({struct _tuple19*_tmp15A=_region_malloc(temp,sizeof(*_tmp15A));_tmp15A->f1=tv,_tmp15A->f2=t;_tmp15A;});_tmp15B->hd=_tmp6FD;}),_tmp15B->tl=inst;_tmp15B;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 645
if(inst != 0){
field_type=({struct _RegionHandle*_tmp6FF=temp;struct Cyc_List_List*_tmp6FE=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(inst);Cyc_Tcutil_rsubstitute(_tmp6FF,_tmp6FE,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 633
;_pop_region();}
# 650
return k;}
# 657
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 659
void*_tmp15C=Cyc_Tcutil_compress(t);void*_stmttmp21=_tmp15C;void*_tmp15D=_stmttmp21;struct Cyc_Absyn_Typedefdecl*_tmp15E;struct Cyc_Absyn_Exp*_tmp15F;struct Cyc_Absyn_PtrInfo _tmp160;struct Cyc_List_List*_tmp162;void*_tmp161;struct Cyc_Absyn_Tvar*_tmp163;struct Cyc_Core_Opt*_tmp164;switch(*((int*)_tmp15D)){case 1U: _LL1: _tmp164=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp15D)->f1;_LL2: {struct Cyc_Core_Opt*k=_tmp164;
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(k))->v;}case 2U: _LL3: _tmp163=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp15D)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp163;
return Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);}case 0U: _LL5: _tmp161=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15D)->f1;_tmp162=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp15D)->f2;_LL6: {void*c=_tmp161;struct Cyc_List_List*ts=_tmp162;
# 663
void*_tmp165=c;int _tmp169;struct Cyc_Absyn_AggrdeclImpl*_tmp168;struct Cyc_List_List*_tmp167;enum Cyc_Absyn_AggrKind _tmp166;struct Cyc_Absyn_Kind*_tmp16A;enum Cyc_Absyn_Size_of _tmp16B;switch(*((int*)_tmp165)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp16B=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp165)->f2;_LL21: {enum Cyc_Absyn_Size_of sz=_tmp16B;
return((int)sz == (int)2U ||(int)sz == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;}case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp16A=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp165)->f2;_LL31: {struct Cyc_Absyn_Kind*k=_tmp16A;
return k;}case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp165)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 684
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 686
({void*_tmp16C=0U;({struct _fat_ptr _tmp700=({const char*_tmp16D="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp16D,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp700,_tag_fat(_tmp16C,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp165)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 690
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp166=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp165)->f1).KnownAggr).val)->kind;_tmp167=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp165)->f1).KnownAggr).val)->tvs;_tmp168=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp165)->f1).KnownAggr).val)->impl;_tmp169=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp165)->f1).KnownAggr).val)->expected_mem_kind;_LL4B: {enum Cyc_Absyn_AggrKind strOrU=_tmp166;struct Cyc_List_List*tvs=_tmp167;struct Cyc_Absyn_AggrdeclImpl*i=_tmp168;int expected_mem_kind=_tmp169;
# 692
if(i == 0){
if(expected_mem_kind)
return& Cyc_Tcutil_mk;else{
# 696
return& Cyc_Tcutil_ak;}}{
# 698
struct Cyc_List_List*_tmp16E=i->fields;struct Cyc_List_List*fields=_tmp16E;
if(fields == 0)return& Cyc_Tcutil_mk;
# 701
if((int)strOrU == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(fields))->tl != 0;fields=fields->tl){;}{
void*_tmp16F=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*last_type=_tmp16F;
struct Cyc_Absyn_Kind*_tmp170=Cyc_Tcutil_field_kind(last_type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp170;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}else{
# 709
for(0;fields != 0;fields=fields->tl){
void*_tmp171=((struct Cyc_Absyn_Aggrfield*)fields->hd)->type;void*type=_tmp171;
struct Cyc_Absyn_Kind*_tmp172=Cyc_Tcutil_field_kind(type,ts,tvs);struct Cyc_Absyn_Kind*k=_tmp172;
if(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)return k;}}
# 715
return& Cyc_Tcutil_mk;}}}}_LL1D:;}case 5U: _LL7: _LL8:
# 717
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp160=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp15D)->f1;_LLC: {struct Cyc_Absyn_PtrInfo pinfo=_tmp160;
# 720
void*_tmp173=Cyc_Tcutil_compress((pinfo.ptr_atts).bounds);void*_stmttmp22=_tmp173;void*_tmp174=_stmttmp22;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp174)->f1)){case 13U: _LL4D: _LL4E: {
# 722
enum Cyc_Absyn_AliasQual _tmp175=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp23=_tmp175;enum Cyc_Absyn_AliasQual _tmp176=_stmttmp23;switch(_tmp176){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 728
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 730
enum Cyc_Absyn_AliasQual _tmp177=(Cyc_Tcutil_type_kind((pinfo.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _stmttmp24=_tmp177;enum Cyc_Absyn_AliasQual _tmp178=_stmttmp24;switch(_tmp178){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 736
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 740
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp15F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp15D)->f1).num_elts;_LL12: {struct Cyc_Absyn_Exp*num_elts=_tmp15F;
# 742
if(num_elts == 0 || Cyc_Tcutil_is_const_exp(num_elts))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;}case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp15E=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp15D)->f3;_LL16: {struct Cyc_Absyn_Typedefdecl*td=_tmp15E;
# 746
if(td == 0 || td->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp17B=({struct Cyc_String_pa_PrintArg_struct _tmp628;_tmp628.tag=0U,({struct _fat_ptr _tmp701=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp628.f1=_tmp701;});_tmp628;});void*_tmp179[1U];_tmp179[0]=& _tmp17B;({struct _fat_ptr _tmp702=({const char*_tmp17A="type_kind: typedef found: %s";_tag_fat(_tmp17A,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp702,_tag_fat(_tmp179,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(td->kind))->v;}default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp15D)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 755
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 759
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 767
int Cyc_Tcutil_is_regparm0_att(void*a){
void*_tmp17C=a;if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp17C)->tag == 0U){if(((struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct*)_tmp17C)->f1 == 0){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 774
int Cyc_Tcutil_equiv_fn_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1)&& !Cyc_Tcutil_is_regparm0_att((void*)a->hd))return 0;}}
return 1;}
# 783
static void*Cyc_Tcutil_rgns_of(void*t);
# 785
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 789
static struct _tuple19*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp17D=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp25=_tmp17D;struct Cyc_Absyn_Kind*_tmp17E=_stmttmp25;switch(((struct Cyc_Absyn_Kind*)_tmp17E)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp17E)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp17F=_cycalloc(sizeof(*_tmp17F));_tmp17F->tag=9U,({struct Cyc_Absyn_Exp*_tmp703=Cyc_Absyn_uint_exp(0U,0U);_tmp17F->f1=_tmp703;});_tmp17F;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 800
return({struct _tuple19*_tmp180=_cycalloc(sizeof(*_tmp180));_tmp180->f1=tv,_tmp180->f2=t;_tmp180;});}
# 807
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp181=Cyc_Tcutil_compress(t);void*_stmttmp26=_tmp181;void*_tmp182=_stmttmp26;struct Cyc_List_List*_tmp183;struct Cyc_List_List*_tmp184;struct Cyc_List_List*_tmp18B;struct Cyc_Absyn_VarargInfo*_tmp18A;struct Cyc_List_List*_tmp189;void*_tmp188;struct Cyc_Absyn_Tqual _tmp187;void*_tmp186;struct Cyc_List_List*_tmp185;struct Cyc_List_List*_tmp18C;void*_tmp18D;void*_tmp18F;void*_tmp18E;struct Cyc_List_List*_tmp190;switch(*((int*)_tmp182)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp182)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp182)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp190=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp182)->f2;_LL6: {struct Cyc_List_List*ts=_tmp190;
# 812
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 816
struct Cyc_Absyn_Kind*_tmp191=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_stmttmp27=_tmp191;struct Cyc_Absyn_Kind*_tmp192=_stmttmp27;switch(((struct Cyc_Absyn_Kind*)_tmp192)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp18E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp182)->f1).elt_type;_tmp18F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp182)->f1).ptr_atts).rgn;_LLC: {void*et=_tmp18E;void*r=_tmp18F;
# 824
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp193[2U];({void*_tmp705=Cyc_Absyn_access_eff(r);_tmp193[0]=_tmp705;}),({void*_tmp704=Cyc_Tcutil_rgns_of(et);_tmp193[1]=_tmp704;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp193,sizeof(void*),2U));})));}case 4U: _LLD: _tmp18D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp182)->f1).elt_type;_LLE: {void*et=_tmp18D;
# 826
return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(et));}case 7U: _LLF: _tmp18C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp182)->f2;_LL10: {struct Cyc_List_List*fs=_tmp18C;
# 828
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,fs)));}case 5U: _LL11: _tmp185=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp182)->f1).tvars;_tmp186=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp182)->f1).effect;_tmp187=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp182)->f1).ret_tqual;_tmp188=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp182)->f1).ret_type;_tmp189=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp182)->f1).args;_tmp18A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp182)->f1).cyc_varargs;_tmp18B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp182)->f1).rgn_po;_LL12: {struct Cyc_List_List*tvs=_tmp185;void*eff=_tmp186;struct Cyc_Absyn_Tqual rt_tq=_tmp187;void*rt=_tmp188;struct Cyc_List_List*args=_tmp189;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp18A;struct Cyc_List_List*rpo=_tmp18B;
# 837
void*_tmp194=({struct Cyc_List_List*_tmp706=((struct Cyc_List_List*(*)(struct _tuple19*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,tvs);Cyc_Tcutil_substitute(_tmp706,(void*)_check_null(eff));});void*e=_tmp194;
return Cyc_Tcutil_normalize_effect(e);}case 6U: _LL13: _tmp184=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp182)->f1;_LL14: {struct Cyc_List_List*tqts=_tmp184;
# 840
struct Cyc_List_List*_tmp195=0;struct Cyc_List_List*ts=_tmp195;
for(0;tqts != 0;tqts=tqts->tl){
ts=({struct Cyc_List_List*_tmp196=_cycalloc(sizeof(*_tmp196));_tmp196->hd=(*((struct _tuple12*)tqts->hd)).f2,_tmp196->tl=ts;_tmp196;});}
_tmp183=ts;goto _LL16;}case 8U: _LL15: _tmp183=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp182)->f2;_LL16: {struct Cyc_List_List*ts=_tmp183;
# 845
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,ts)));}case 10U: _LL17: _LL18:
({void*_tmp197=0U;({struct _fat_ptr _tmp707=({const char*_tmp198="typedecl in rgns_of";_tag_fat(_tmp198,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp707,_tag_fat(_tmp197,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 855
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp199=e;void*_tmp19A;struct Cyc_List_List**_tmp19B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f1)){case 9U: _LL1: _tmp19B=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2;_LL2: {struct Cyc_List_List**es=_tmp19B;
# 859
int redo_join=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp19C=(void*)effs->hd;void*eff=_tmp19C;
({void*_tmp708=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(eff));effs->hd=_tmp708;});{
void*_tmp19D=(void*)effs->hd;void*_stmttmp28=_tmp19D;void*_tmp19E=_stmttmp28;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19E)->f2)->tl == 0){_LLE: _LLF:
# 868
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;}}}
# 872
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*es;for(0;effs != 0;effs=effs->tl){
void*_tmp19F=Cyc_Tcutil_compress((void*)effs->hd);void*_stmttmp29=_tmp19F;void*_tmp1A0=_stmttmp29;void*_tmp1A1;struct Cyc_List_List*_tmp1A2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f1)){case 9U: _LL13: _tmp1A2=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2;_LL14: {struct Cyc_List_List*nested_effs=_tmp1A2;
# 877
effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(nested_effs,effects);
goto _LL12;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A0)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp1A1=_tmp1A0;_LL1C: {void*e=_tmp1A1;
effects=({struct Cyc_List_List*_tmp1A3=_cycalloc(sizeof(*_tmp1A3));_tmp1A3->hd=e,_tmp1A3->tl=effects;_tmp1A3;});goto _LL12;}}_LL12:;}}
# 885
({struct Cyc_List_List*_tmp709=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*es=_tmp709;});
return e;}}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2 != 0){_LL3: _tmp19A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp199)->f2)->hd;_LL4: {void*t=_tmp19A;
# 888
void*_tmp1A4=Cyc_Tcutil_compress(t);void*_stmttmp2A=_tmp1A4;void*_tmp1A5=_stmttmp2A;switch(*((int*)_tmp1A5)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(t);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 893
 return e;}_LL0:;}}
# 898
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A6=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp1A7=_cycalloc(sizeof(*_tmp1A7));_tmp1A7->tag=5U,(_tmp1A7->f1).tvars=0,(_tmp1A7->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp70A=Cyc_Absyn_empty_tqual(0U);(_tmp1A7->f1).ret_tqual=_tmp70A;}),(_tmp1A7->f1).ret_type=Cyc_Absyn_void_type,(_tmp1A7->f1).args=0,(_tmp1A7->f1).c_varargs=0,(_tmp1A7->f1).cyc_varargs=0,(_tmp1A7->f1).rgn_po=0,(_tmp1A7->f1).attributes=0,(_tmp1A7->f1).requires_clause=0,(_tmp1A7->f1).requires_relns=0,(_tmp1A7->f1).ensures_clause=0,(_tmp1A7->f1).ensures_relns=0,(_tmp1A7->f1).return_value=0;_tmp1A7;});
# 899
struct Cyc_Absyn_FnType_Absyn_Type_struct*fntype=_tmp1A6;
# 909
return({void*_tmp70E=(void*)fntype;void*_tmp70D=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp70C=Cyc_Absyn_empty_tqual(0U);void*_tmp70B=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp70E,_tmp70D,_tmp70C,_tmp70B,Cyc_Absyn_false_type);});}
# 916
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp1A8=Cyc_Tcutil_compress(e);void*_stmttmp2B=_tmp1A8;void*_tmp1A9=_stmttmp2B;struct Cyc_Core_Opt*_tmp1AC;void**_tmp1AB;struct Cyc_Core_Opt*_tmp1AA;void*_tmp1AD;struct Cyc_List_List*_tmp1AE;void*_tmp1AF;switch(*((int*)_tmp1A9)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f2 != 0){_LL1: _tmp1AF=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f2)->hd;_LL2: {void*r2=_tmp1AF;
# 923
if(constrain)return Cyc_Unify_unify(r,r2);
r2=Cyc_Tcutil_compress(r2);
if(r == r2)return 1;{
struct _tuple15 _tmp1B0=({struct _tuple15 _tmp629;_tmp629.f1=r,_tmp629.f2=r2;_tmp629;});struct _tuple15 _stmttmp2C=_tmp1B0;struct _tuple15 _tmp1B1=_stmttmp2C;struct Cyc_Absyn_Tvar*_tmp1B3;struct Cyc_Absyn_Tvar*_tmp1B2;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B1.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B1.f2)->tag == 2U){_LLC: _tmp1B2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B1.f1)->f1;_tmp1B3=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B1.f2)->f1;_LLD: {struct Cyc_Absyn_Tvar*tv1=_tmp1B2;struct Cyc_Absyn_Tvar*tv2=_tmp1B3;
return Cyc_Absyn_tvar_cmp(tv1,tv2)== 0;}}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;}}}else{goto _LL9;}case 9U: _LL3: _tmp1AE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f2;_LL4: {struct Cyc_List_List*es=_tmp1AE;
# 931
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)es->hd))return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f2 != 0){_LL5: _tmp1AD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A9)->f2)->hd;_LL6: {void*t=_tmp1AD;
# 935
void*_tmp1B4=Cyc_Tcutil_rgns_of(t);void*_stmttmp2D=_tmp1B4;void*_tmp1B5=_stmttmp2D;void*_tmp1B6;void*_tmp1B7;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B5)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B5)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B5)->f2 != 0){_LL11: _tmp1B7=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B5)->f2)->hd;_LL12: {void*t=_tmp1B7;
# 937
if(!constrain)return 0;{
void*_tmp1B8=Cyc_Tcutil_compress(t);void*_stmttmp2E=_tmp1B8;void*_tmp1B9=_stmttmp2E;struct Cyc_Core_Opt*_tmp1BC;void**_tmp1BB;struct Cyc_Core_Opt*_tmp1BA;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9)->tag == 1U){_LL16: _tmp1BA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9)->f1;_tmp1BB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9)->f2;_tmp1BC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B9)->f4;_LL17: {struct Cyc_Core_Opt*k=_tmp1BA;void**p=_tmp1BB;struct Cyc_Core_Opt*s=_tmp1BC;
# 942
void*_tmp1BD=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1BD;
# 945
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);
({void*_tmp710=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1BE[2U];_tmp1BE[0]=ev,({void*_tmp70F=Cyc_Absyn_access_eff(r);_tmp1BE[1]=_tmp70F;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1BE,sizeof(void*),2U));})));*p=_tmp710;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;}}}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp1B6=_tmp1B5;_LL14: {void*e2=_tmp1B6;
# 950
return Cyc_Tcutil_region_in_effect(constrain,r,e2);}}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1AA=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A9)->f1;_tmp1AB=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A9)->f2;_tmp1AC=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1A9)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1AA;void**p=_tmp1AB;struct Cyc_Core_Opt*s=_tmp1AC;
# 953
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1BF=0U;({struct _fat_ptr _tmp711=({const char*_tmp1C0="effect evar has wrong kind";_tag_fat(_tmp1C0,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp711,_tag_fat(_tmp1BF,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 958
void*_tmp1C1=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1C1;
# 961
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,r);{
void*_tmp1C2=Cyc_Absyn_join_eff(({void*_tmp1C3[2U];_tmp1C3[0]=ev,({void*_tmp712=Cyc_Absyn_access_eff(r);_tmp1C3[1]=_tmp712;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1C3,sizeof(void*),2U));}));void*new_typ=_tmp1C2;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 972
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp1C4=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_stmttmp2F=_tmp1C4;void*_tmp1C5=_stmttmp2F;struct Cyc_Core_Opt*_tmp1C8;void**_tmp1C7;struct Cyc_Core_Opt*_tmp1C6;void*_tmp1C9;struct Cyc_List_List*_tmp1CA;switch(*((int*)_tmp1C5)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C5)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp1CA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C5)->f2;_LL4: {struct Cyc_List_List*es=_tmp1CA;
# 977
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C5)->f2 != 0){_LL5: _tmp1C9=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1C5)->f2)->hd;_LL6: {void*t2=_tmp1C9;
# 982
t2=Cyc_Tcutil_compress(t2);
if(t == t2)return 1;
if(may_constrain_evars)return Cyc_Unify_unify(t,t2);{
void*_tmp1CB=Cyc_Tcutil_rgns_of(t);void*_stmttmp30=_tmp1CB;void*_tmp1CC=_stmttmp30;void*_tmp1CD;void*_tmp1CE;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC)->f2 != 0){_LLC: _tmp1CE=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1CC)->f2)->hd;_LLD: {void*t3=_tmp1CE;
# 987
struct _tuple15 _tmp1CF=({struct _tuple15 _tmp62A;({void*_tmp713=Cyc_Tcutil_compress(t3);_tmp62A.f1=_tmp713;}),_tmp62A.f2=t2;_tmp62A;});struct _tuple15 _stmttmp31=_tmp1CF;struct _tuple15 _tmp1D0=_stmttmp31;struct Cyc_Absyn_Tvar*_tmp1D2;struct Cyc_Absyn_Tvar*_tmp1D1;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D0.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D0.f2)->tag == 2U){_LL11: _tmp1D1=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D0.f1)->f1;_tmp1D2=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D0.f2)->f1;_LL12: {struct Cyc_Absyn_Tvar*tv1=_tmp1D1;struct Cyc_Absyn_Tvar*tv2=_tmp1D2;
return Cyc_Unify_unify(t,t2);}}else{goto _LL13;}}else{_LL13: _LL14:
 return t3 == t2;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1CD=_tmp1CC;_LLF: {void*e2=_tmp1CD;
# 991
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,e2);}}_LLB:;}}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1C6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C5)->f1;_tmp1C7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C5)->f2;_tmp1C8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1C5)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1C6;void**p=_tmp1C7;struct Cyc_Core_Opt*s=_tmp1C8;
# 994
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1D3=0U;({struct _fat_ptr _tmp714=({const char*_tmp1D4="effect evar has wrong kind";_tag_fat(_tmp1D4,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp714,_tag_fat(_tmp1D3,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 999
void*_tmp1D5=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1D5;
# 1002
Cyc_Unify_occurs(ev,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,t);{
void*_tmp1D6=Cyc_Absyn_join_eff(({void*_tmp1D7[2U];_tmp1D7[0]=ev,({void*_tmp715=Cyc_Absyn_regionsof_eff(t);_tmp1D7[1]=_tmp715;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1D7,sizeof(void*),2U));}));void*new_typ=_tmp1D6;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1013
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1D8=e;struct Cyc_Core_Opt*_tmp1DB;void**_tmp1DA;struct Cyc_Core_Opt*_tmp1D9;void*_tmp1DC;struct Cyc_List_List*_tmp1DD;struct Cyc_Absyn_Tvar*_tmp1DE;switch(*((int*)_tmp1D8)){case 2U: _LL1: _tmp1DE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1D8)->f1;_LL2: {struct Cyc_Absyn_Tvar*v2=_tmp1DE;
return Cyc_Absyn_tvar_cmp(v,v2)== 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D8)->f1)){case 9U: _LL3: _tmp1DD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D8)->f2;_LL4: {struct Cyc_List_List*es=_tmp1DD;
# 1018
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D8)->f2 != 0){_LL5: _tmp1DC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1D8)->f2)->hd;_LL6: {void*t=_tmp1DC;
# 1023
void*_tmp1DF=Cyc_Tcutil_rgns_of(t);void*_stmttmp32=_tmp1DF;void*_tmp1E0=_stmttmp32;void*_tmp1E1;void*_tmp1E2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E0)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E0)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E0)->f2 != 0){_LLC: _tmp1E2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1E0)->f2)->hd;_LLD: {void*t2=_tmp1E2;
# 1025
if(!may_constrain_evars)return 0;{
void*_tmp1E3=Cyc_Tcutil_compress(t2);void*_stmttmp33=_tmp1E3;void*_tmp1E4=_stmttmp33;struct Cyc_Core_Opt*_tmp1E7;void**_tmp1E6;struct Cyc_Core_Opt*_tmp1E5;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E4)->tag == 1U){_LL11: _tmp1E5=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E4)->f1;_tmp1E6=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E4)->f2;_tmp1E7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E4)->f4;_LL12: {struct Cyc_Core_Opt*k=_tmp1E5;void**p=_tmp1E6;struct Cyc_Core_Opt*s=_tmp1E7;
# 1031
void*_tmp1E8=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1E8;
# 1033
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))return 0;
({void*_tmp717=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp1E9[2U];_tmp1E9[0]=ev,({void*_tmp716=Cyc_Absyn_var_type(v);_tmp1E9[1]=_tmp716;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1E9,sizeof(void*),2U));})));*p=_tmp717;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;}}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp1E1=_tmp1E0;_LLF: {void*e2=_tmp1E1;
# 1038
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp1D9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D8)->f1;_tmp1DA=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D8)->f2;_tmp1DB=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1D8)->f4;_LL8: {struct Cyc_Core_Opt*k=_tmp1D9;void**p=_tmp1DA;struct Cyc_Core_Opt*s=_tmp1DB;
# 1041
if(k == 0 ||(int)((struct Cyc_Absyn_Kind*)k->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp1EA=0U;({struct _fat_ptr _tmp718=({const char*_tmp1EB="effect evar has wrong kind";_tag_fat(_tmp1EB,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp718,_tag_fat(_tmp1EA,sizeof(void*),0U));});});{
# 1045
void*_tmp1EC=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,s);void*ev=_tmp1EC;
# 1047
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(s))->v,v))
return 0;{
void*_tmp1ED=Cyc_Absyn_join_eff(({void*_tmp1EE[2U];_tmp1EE[0]=ev,({void*_tmp719=Cyc_Absyn_var_type(v);_tmp1EE[1]=_tmp719;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1EE,sizeof(void*),2U));}));void*new_typ=_tmp1ED;
*p=new_typ;
return 1;}}}default: _LL9: _LLA:
 return 0;}_LL0:;}}
# 1057
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp1EF=e;void*_tmp1F0;struct Cyc_List_List*_tmp1F1;switch(*((int*)_tmp1EF)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f1)){case 9U: _LL1: _tmp1F1=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f2;_LL2: {struct Cyc_List_List*es=_tmp1F1;
# 1061
for(0;es != 0;es=es->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)es->hd))
return 1;}
return 0;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f2 != 0){_LL3: _tmp1F0=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EF)->f2)->hd;_LL4: {void*t=_tmp1F0;
# 1066
void*_tmp1F2=Cyc_Tcutil_rgns_of(t);void*_stmttmp34=_tmp1F2;void*_tmp1F3=_stmttmp34;void*_tmp1F4;void*_tmp1F5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2 != 0){_LLA: _tmp1F5=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F3)->f2)->hd;_LLB: {void*t2=_tmp1F5;
return 0;}}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp1F4=_tmp1F3;_LLD: {void*e2=_tmp1F4;
return Cyc_Tcutil_evar_in_effect(evar,e2);}}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1070
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 1084 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1089
void*_tmp1F6=Cyc_Tcutil_compress(e1);void*_stmttmp35=_tmp1F6;void*_tmp1F7=_stmttmp35;struct Cyc_Core_Opt*_tmp1F9;void**_tmp1F8;struct Cyc_Absyn_Tvar*_tmp1FA;void*_tmp1FB;void*_tmp1FC;struct Cyc_List_List*_tmp1FD;switch(*((int*)_tmp1F7)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F7)->f1)){case 9U: _LL1: _tmp1FD=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F7)->f2;_LL2: {struct Cyc_List_List*es=_tmp1FD;
# 1091
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)es->hd,e2))
return 0;}
return 1;}case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F7)->f2 != 0){_LL3: _tmp1FC=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F7)->f2)->hd;_LL4: {void*r=_tmp1FC;
# 1102
return Cyc_Tcutil_region_in_effect(may_constrain_evars,r,e2)||
 may_constrain_evars && Cyc_Unify_unify(r,Cyc_Absyn_heap_rgn_type);}}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F7)->f2 != 0){_LL7: _tmp1FB=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1F7)->f2)->hd;_LL8: {void*t=_tmp1FB;
# 1106
void*_tmp1FE=Cyc_Tcutil_rgns_of(t);void*_stmttmp36=_tmp1FE;void*_tmp1FF=_stmttmp36;void*_tmp200;void*_tmp201;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FF)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FF)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FF)->f2 != 0){_LLE: _tmp201=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1FF)->f2)->hd;_LLF: {void*t2=_tmp201;
# 1111
return Cyc_Tcutil_type_in_effect(may_constrain_evars,t2,e2)||
 may_constrain_evars && Cyc_Unify_unify(t2,Cyc_Absyn_sint_type);}}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp200=_tmp1FF;_LL11: {void*e=_tmp200;
return Cyc_Tcutil_subset_effect(may_constrain_evars,e,e2);}}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp1FA=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1F7)->f1;_LL6: {struct Cyc_Absyn_Tvar*v=_tmp1FA;
# 1104
return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,e2);}case 1U: _LL9: _tmp1F8=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F7)->f2;_tmp1F9=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1F7)->f4;_LLA: {void**p=_tmp1F8;struct Cyc_Core_Opt*s=_tmp1F9;
# 1116
if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1120
*p=Cyc_Absyn_empty_effect;
# 1123
return 1;}else{
# 1125
return 0;}}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp204=({struct Cyc_String_pa_PrintArg_struct _tmp62B;_tmp62B.tag=0U,({struct _fat_ptr _tmp71A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp62B.f1=_tmp71A;});_tmp62B;});void*_tmp202[1U];_tmp202[0]=& _tmp204;({struct _fat_ptr _tmp71B=({const char*_tmp203="subset_effect: bad effect: %s";_tag_fat(_tmp203,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp71B,_tag_fat(_tmp202,sizeof(void*),1U));});});}_LL0:;}
# 1136
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1138
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple15*_tmp205=(struct _tuple15*)r1->hd;struct _tuple15*_stmttmp37=_tmp205;struct _tuple15*_tmp206=_stmttmp37;void*_tmp208;void*_tmp207;_LL1: _tmp207=_tmp206->f1;_tmp208=_tmp206->f2;_LL2: {void*t1a=_tmp207;void*t1b=_tmp208;
int found=t1a == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple15*_tmp209=(struct _tuple15*)r2->hd;struct _tuple15*_stmttmp38=_tmp209;struct _tuple15*_tmp20A=_stmttmp38;void*_tmp20C;void*_tmp20B;_LL4: _tmp20B=_tmp20A->f1;_tmp20C=_tmp20A->f2;_LL5: {void*t2a=_tmp20B;void*t2b=_tmp20C;
if(Cyc_Unify_unify(t1a,t2a)&& Cyc_Unify_unify(t1b,t2b)){
found=1;
break;}}}}
# 1148
if(!found)return 0;}}}
# 1150
return 1;}
# 1156
int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1158
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1161
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp20D=t;switch(*((int*)_tmp20D)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20D)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20D)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1167
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1170
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1173
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1176
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20D)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1165
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1168
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1171
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1174
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1177
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp20D)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1166
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1169
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1172
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1175
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1178
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1200
({void*_tmp20E=0U;({struct _fat_ptr _tmp71C=({const char*_tmp20F="bad con";_tag_fat(_tmp20F,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp71C,_tag_fat(_tmp20E,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp20D)->f1){case 0U: _LL21: _LL22:
# 1179
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
# 1203
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp210=t;void*_tmp211;switch(*((int*)_tmp210)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp211=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp210)->f1;_LL18: {void*c=_tmp211;
return 12 + Cyc_Tcutil_tycon2int(c);}}_LL0:;}
# 1219
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp212=Cyc_Absyn_qvar_cmp(e1->name,e2->name);int qc=_tmp212;
if(qc != 0)return qc;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1->tag,e2->tag);}
# 1224
static struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp213=i;struct _tuple1*_tmp214;struct Cyc_Absyn_Datatypedecl*_tmp215;if((_tmp213.KnownDatatype).tag == 2){_LL1: _tmp215=*(_tmp213.KnownDatatype).val;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp215;
return dd->name;}}else{_LL3: _tmp214=((_tmp213.UnknownDatatype).val).name;_LL4: {struct _tuple1*n=_tmp214;
return n;}}_LL0:;}struct _tuple20{struct _tuple1*f1;struct _tuple1*f2;};
# 1230
static struct _tuple20 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp216=i;struct _tuple1*_tmp218;struct _tuple1*_tmp217;struct Cyc_Absyn_Datatypefield*_tmp21A;struct Cyc_Absyn_Datatypedecl*_tmp219;if((_tmp216.KnownDatatypefield).tag == 2){_LL1: _tmp219=((_tmp216.KnownDatatypefield).val).f1;_tmp21A=((_tmp216.KnownDatatypefield).val).f2;_LL2: {struct Cyc_Absyn_Datatypedecl*dd=_tmp219;struct Cyc_Absyn_Datatypefield*df=_tmp21A;
# 1233
return({struct _tuple20 _tmp62C;_tmp62C.f1=dd->name,_tmp62C.f2=df->name;_tmp62C;});}}else{_LL3: _tmp217=((_tmp216.UnknownDatatypefield).val).datatype_name;_tmp218=((_tmp216.UnknownDatatypefield).val).field_name;_LL4: {struct _tuple1*d=_tmp217;struct _tuple1*f=_tmp218;
# 1235
return({struct _tuple20 _tmp62D;_tmp62D.f1=d,_tmp62D.f2=f;_tmp62D;});}}_LL0:;}struct _tuple21{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1238
static struct _tuple21 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp21B=i;struct Cyc_Absyn_Aggrdecl*_tmp21C;struct _tuple1*_tmp21E;enum Cyc_Absyn_AggrKind _tmp21D;if((_tmp21B.UnknownAggr).tag == 1){_LL1: _tmp21D=((_tmp21B.UnknownAggr).val).f1;_tmp21E=((_tmp21B.UnknownAggr).val).f2;_LL2: {enum Cyc_Absyn_AggrKind k=_tmp21D;struct _tuple1*n=_tmp21E;
return({struct _tuple21 _tmp62E;_tmp62E.f1=k,_tmp62E.f2=n;_tmp62E;});}}else{_LL3: _tmp21C=*(_tmp21B.KnownAggr).val;_LL4: {struct Cyc_Absyn_Aggrdecl*ad=_tmp21C;
return({struct _tuple21 _tmp62F;_tmp62F.f1=ad->kind,_tmp62F.f2=ad->name;_tmp62F;});}}_LL0:;}
# 1244
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1250
struct _tuple15 _tmp21F=({struct _tuple15 _tmp630;_tmp630.f1=t1,_tmp630.f2=t2;_tmp630;});struct _tuple15 _stmttmp39=_tmp21F;struct _tuple15 _tmp220=_stmttmp39;union Cyc_Absyn_AggrInfo _tmp222;union Cyc_Absyn_AggrInfo _tmp221;union Cyc_Absyn_DatatypeFieldInfo _tmp224;union Cyc_Absyn_DatatypeFieldInfo _tmp223;union Cyc_Absyn_DatatypeInfo _tmp226;union Cyc_Absyn_DatatypeInfo _tmp225;struct Cyc_List_List*_tmp228;struct Cyc_List_List*_tmp227;struct _fat_ptr _tmp22A;struct _fat_ptr _tmp229;struct _tuple1*_tmp22C;struct _tuple1*_tmp22B;switch(*((int*)_tmp220.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp220.f2)->tag == 15U){_LL1: _tmp22B=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp220.f1)->f1;_tmp22C=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp220.f2)->f1;_LL2: {struct _tuple1*n1=_tmp22B;struct _tuple1*n2=_tmp22C;
return Cyc_Absyn_qvar_cmp(n1,n2);}}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp220.f2)->tag == 17U){_LL3: _tmp229=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp220.f1)->f1;_tmp22A=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp220.f2)->f1;_LL4: {struct _fat_ptr s1=_tmp229;struct _fat_ptr s2=_tmp22A;
return Cyc_strcmp((struct _fat_ptr)s1,(struct _fat_ptr)s2);}}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp220.f2)->tag == 16U){_LL5: _tmp227=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp220.f1)->f1;_tmp228=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp220.f2)->f1;_LL6: {struct Cyc_List_List*fs1=_tmp227;struct Cyc_List_List*fs2=_tmp228;
# 1254
return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,fs1,fs2);}}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp220.f2)->tag == 18U){_LL7: _tmp225=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp220.f1)->f1;_tmp226=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp220.f2)->f1;_LL8: {union Cyc_Absyn_DatatypeInfo info1=_tmp225;union Cyc_Absyn_DatatypeInfo info2=_tmp226;
# 1256
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(info1);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(info2);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp220.f2)->tag == 19U){_LL9: _tmp223=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp220.f1)->f1;_tmp224=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp220.f2)->f1;_LLA: {union Cyc_Absyn_DatatypeFieldInfo info1=_tmp223;union Cyc_Absyn_DatatypeFieldInfo info2=_tmp224;
# 1260
struct _tuple20 _tmp22D=Cyc_Tcutil_get_datatype_field_qvars(info1);struct _tuple20 _stmttmp3A=_tmp22D;struct _tuple20 _tmp22E=_stmttmp3A;struct _tuple1*_tmp230;struct _tuple1*_tmp22F;_LL10: _tmp22F=_tmp22E.f1;_tmp230=_tmp22E.f2;_LL11: {struct _tuple1*d1=_tmp22F;struct _tuple1*f1=_tmp230;
struct _tuple20 _tmp231=Cyc_Tcutil_get_datatype_field_qvars(info2);struct _tuple20 _stmttmp3B=_tmp231;struct _tuple20 _tmp232=_stmttmp3B;struct _tuple1*_tmp234;struct _tuple1*_tmp233;_LL13: _tmp233=_tmp232.f1;_tmp234=_tmp232.f2;_LL14: {struct _tuple1*d2=_tmp233;struct _tuple1*f2=_tmp234;
int _tmp235=Cyc_Absyn_qvar_cmp(d1,d2);int c=_tmp235;
if(c != 0)return c;
return Cyc_Absyn_qvar_cmp(f1,f2);}}}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp220.f2)->tag == 20U){_LLB: _tmp221=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp220.f1)->f1;_tmp222=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp220.f2)->f1;_LLC: {union Cyc_Absyn_AggrInfo info1=_tmp221;union Cyc_Absyn_AggrInfo info2=_tmp222;
# 1266
struct _tuple21 _tmp236=Cyc_Tcutil_get_aggr_kind_and_qvar(info1);struct _tuple21 _stmttmp3C=_tmp236;struct _tuple21 _tmp237=_stmttmp3C;struct _tuple1*_tmp239;enum Cyc_Absyn_AggrKind _tmp238;_LL16: _tmp238=_tmp237.f1;_tmp239=_tmp237.f2;_LL17: {enum Cyc_Absyn_AggrKind k1=_tmp238;struct _tuple1*q1=_tmp239;
struct _tuple21 _tmp23A=Cyc_Tcutil_get_aggr_kind_and_qvar(info2);struct _tuple21 _stmttmp3D=_tmp23A;struct _tuple21 _tmp23B=_stmttmp3D;struct _tuple1*_tmp23D;enum Cyc_Absyn_AggrKind _tmp23C;_LL19: _tmp23C=_tmp23B.f1;_tmp23D=_tmp23B.f2;_LL1A: {enum Cyc_Absyn_AggrKind k2=_tmp23C;struct _tuple1*q2=_tmp23D;
int _tmp23E=Cyc_Absyn_qvar_cmp(q1,q2);int c=_tmp23E;
if(c != 0)return c;
return(int)k1 - (int)k2;}}}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;}}}
# 1275
int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp71E)(void*,void*)=cmp;void*_tmp71D=(void*)_check_null(a1);_tmp71E(_tmp71D,(void*)_check_null(a2));});}
# 1281
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp23F=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);int i1=_tmp23F;
int _tmp240=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);int i2=_tmp240;
return Cyc_Core_intcmp(i1,i2);}
# 1286
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple12*tqt1,struct _tuple12*tqt2){
struct _tuple12*_tmp241=tqt1;void*_tmp243;struct Cyc_Absyn_Tqual _tmp242;_LL1: _tmp242=_tmp241->f1;_tmp243=_tmp241->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp242;void*t1=_tmp243;
struct _tuple12*_tmp244=tqt2;void*_tmp246;struct Cyc_Absyn_Tqual _tmp245;_LL4: _tmp245=_tmp244->f1;_tmp246=_tmp244->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp245;void*t2=_tmp246;
int _tmp247=Cyc_Tcutil_tqual_cmp(tq1,tq2);int tqc=_tmp247;
if(tqc != 0)return tqc;
return Cyc_Tcutil_typecmp(t1,t2);}}}
# 1294
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp248=Cyc_strptrcmp(f1->name,f2->name);int zsc=_tmp248;
if(zsc != 0)return zsc;{
int _tmp249=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);int tqc=_tmp249;
if(tqc != 0)return tqc;{
int _tmp24A=Cyc_Tcutil_typecmp(f1->type,f2->type);int tc=_tmp24A;
if(tc != 0)return tc;{
int _tmp24B=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);int ac=_tmp24B;
if(ac != 0)return ac;
ac=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->width,f2->width);
if(ac != 0)return ac;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1->requires_clause,f2->requires_clause);}}}}
# 1311
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp24C=({int _tmp71F=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp71F,Cyc_Tcutil_type_case_number(t2));});int shallowcmp=_tmp24C;
if(shallowcmp != 0)
return shallowcmp;{
# 1320
struct _tuple15 _tmp24D=({struct _tuple15 _tmp631;_tmp631.f1=t2,_tmp631.f2=t1;_tmp631;});struct _tuple15 _stmttmp3E=_tmp24D;struct _tuple15 _tmp24E=_stmttmp3E;struct Cyc_Absyn_Exp*_tmp250;struct Cyc_Absyn_Exp*_tmp24F;struct Cyc_Absyn_Exp*_tmp252;struct Cyc_Absyn_Exp*_tmp251;struct Cyc_List_List*_tmp256;enum Cyc_Absyn_AggrKind _tmp255;struct Cyc_List_List*_tmp254;enum Cyc_Absyn_AggrKind _tmp253;struct Cyc_List_List*_tmp258;struct Cyc_List_List*_tmp257;struct Cyc_Absyn_FnInfo _tmp25A;struct Cyc_Absyn_FnInfo _tmp259;void*_tmp262;struct Cyc_Absyn_Exp*_tmp261;struct Cyc_Absyn_Tqual _tmp260;void*_tmp25F;void*_tmp25E;struct Cyc_Absyn_Exp*_tmp25D;struct Cyc_Absyn_Tqual _tmp25C;void*_tmp25B;void*_tmp26E;void*_tmp26D;void*_tmp26C;void*_tmp26B;struct Cyc_Absyn_Tqual _tmp26A;void*_tmp269;void*_tmp268;void*_tmp267;void*_tmp266;void*_tmp265;struct Cyc_Absyn_Tqual _tmp264;void*_tmp263;struct Cyc_Absyn_Tvar*_tmp270;struct Cyc_Absyn_Tvar*_tmp26F;struct Cyc_List_List*_tmp274;void*_tmp273;struct Cyc_List_List*_tmp272;void*_tmp271;switch(*((int*)_tmp24E.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E.f2)->tag == 0U){_LL1: _tmp271=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E.f1)->f1;_tmp272=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E.f1)->f2;_tmp273=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E.f2)->f1;_tmp274=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp24E.f2)->f2;_LL2: {void*c1=_tmp271;struct Cyc_List_List*ts1=_tmp272;void*c2=_tmp273;struct Cyc_List_List*ts2=_tmp274;
# 1322
int _tmp275=Cyc_Tcutil_tycon_cmp(c1,c2);int c=_tmp275;
if(c != 0)return c;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,ts1,ts2);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp24E.f2)->tag == 1U){_LL3: _LL4:
# 1326
({void*_tmp276=0U;({struct _fat_ptr _tmp720=({const char*_tmp277="typecmp: can only compare closed types";_tag_fat(_tmp277,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp720,_tag_fat(_tmp276,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24E.f2)->tag == 2U){_LL5: _tmp26F=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24E.f1)->f1;_tmp270=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp24E.f2)->f1;_LL6: {struct Cyc_Absyn_Tvar*tv2=_tmp26F;struct Cyc_Absyn_Tvar*tv1=_tmp270;
# 1330
return Cyc_Core_intcmp(tv1->identity,tv2->identity);}}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f2)->tag == 3U){_LL7: _tmp263=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f1)->f1).elt_type;_tmp264=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f1)->f1).elt_tq;_tmp265=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f1)->f1).ptr_atts).rgn;_tmp266=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f1)->f1).ptr_atts).nullable;_tmp267=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f1)->f1).ptr_atts).bounds;_tmp268=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f1)->f1).ptr_atts).zero_term;_tmp269=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f2)->f1).elt_type;_tmp26A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f2)->f1).elt_tq;_tmp26B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f2)->f1).ptr_atts).rgn;_tmp26C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f2)->f1).ptr_atts).nullable;_tmp26D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f2)->f1).ptr_atts).bounds;_tmp26E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp24E.f2)->f1).ptr_atts).zero_term;_LL8: {void*t2a=_tmp263;struct Cyc_Absyn_Tqual tqual2a=_tmp264;void*rgn2=_tmp265;void*null2a=_tmp266;void*b2=_tmp267;void*zt2=_tmp268;void*t1a=_tmp269;struct Cyc_Absyn_Tqual tqual1a=_tmp26A;void*rgn1=_tmp26B;void*null1a=_tmp26C;void*b1=_tmp26D;void*zt1=_tmp26E;
# 1334
int _tmp278=Cyc_Tcutil_typecmp(t1a,t2a);int etc=_tmp278;
if(etc != 0)return etc;{
int _tmp279=Cyc_Tcutil_typecmp(rgn1,rgn2);int rc=_tmp279;
if(rc != 0)return rc;{
int _tmp27A=Cyc_Tcutil_tqual_cmp(tqual1a,tqual2a);int tqc=_tmp27A;
if(tqc != 0)return tqc;{
int _tmp27B=Cyc_Tcutil_typecmp(b1,b2);int cc=_tmp27B;
if(cc != 0)return cc;{
int _tmp27C=Cyc_Tcutil_typecmp(zt1,zt2);int zc=_tmp27C;
if(zc != 0)return zc;{
int _tmp27D=Cyc_Tcutil_typecmp(b1,b2);int bc=_tmp27D;
if(bc != 0)return bc;
return Cyc_Tcutil_typecmp(null1a,null2a);}}}}}}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f2)->tag == 4U){_LL9: _tmp25B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f1)->f1).elt_type;_tmp25C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f1)->f1).tq;_tmp25D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f1)->f1).num_elts;_tmp25E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f1)->f1).zero_term;_tmp25F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f2)->f1).elt_type;_tmp260=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f2)->f1).tq;_tmp261=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f2)->f1).num_elts;_tmp262=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp24E.f2)->f1).zero_term;_LLA: {void*t2a=_tmp25B;struct Cyc_Absyn_Tqual tq2a=_tmp25C;struct Cyc_Absyn_Exp*e1=_tmp25D;void*zt1=_tmp25E;void*t1a=_tmp25F;struct Cyc_Absyn_Tqual tq1a=_tmp260;struct Cyc_Absyn_Exp*e2=_tmp261;void*zt2=_tmp262;
# 1350
int _tmp27E=Cyc_Tcutil_tqual_cmp(tq1a,tq2a);int tqc=_tmp27E;
if(tqc != 0)return tqc;{
int _tmp27F=Cyc_Tcutil_typecmp(t1a,t2a);int tc=_tmp27F;
if(tc != 0)return tc;{
int _tmp280=Cyc_Tcutil_typecmp(zt1,zt2);int ztc=_tmp280;
if(ztc != 0)return ztc;
if(e1 == e2)return 0;
if(e1 == 0 || e2 == 0)
({void*_tmp281=0U;({struct _fat_ptr _tmp721=({const char*_tmp282="missing expression in array index";_tag_fat(_tmp282,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp721,_tag_fat(_tmp281,sizeof(void*),0U));});});
# 1360
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,e1,e2);}}}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E.f2)->tag == 5U){_LLB: _tmp259=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E.f1)->f1;_tmp25A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp24E.f2)->f1;_LLC: {struct Cyc_Absyn_FnInfo f1=_tmp259;struct Cyc_Absyn_FnInfo f2=_tmp25A;
# 1363
if(Cyc_Unify_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(f1.ret_type,f2.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(f1.ret_tqual,f2.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp283=f1.args;struct Cyc_List_List*args1=_tmp283;
struct Cyc_List_List*_tmp284=f2.args;struct Cyc_List_List*args2=_tmp284;
for(0;args1 != 0 && args2 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp285=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp3F=_tmp285;struct _tuple9 _tmp286=_stmttmp3F;void*_tmp288;struct Cyc_Absyn_Tqual _tmp287;_LL18: _tmp287=_tmp286.f2;_tmp288=_tmp286.f3;_LL19: {struct Cyc_Absyn_Tqual tq1=_tmp287;void*t1=_tmp288;
struct _tuple9 _tmp289=*((struct _tuple9*)args2->hd);struct _tuple9 _stmttmp40=_tmp289;struct _tuple9 _tmp28A=_stmttmp40;void*_tmp28C;struct Cyc_Absyn_Tqual _tmp28B;_LL1B: _tmp28B=_tmp28A.f2;_tmp28C=_tmp28A.f3;_LL1C: {struct Cyc_Absyn_Tqual tq2=_tmp28B;void*t2=_tmp28C;
r=Cyc_Tcutil_tqual_cmp(tq1,tq2);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1,t2);
if(r != 0)return r;}}}
# 1378
if(args1 != 0)return 1;
if(args2 != 0)return - 1;
if(f1.c_varargs && !f2.c_varargs)return 1;
if(!f1.c_varargs && f2.c_varargs)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs == 0)return 1;
if(f1.cyc_varargs == 0 & f2.cyc_varargs != 0)return - 1;
if(f1.cyc_varargs != 0 & f2.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp722=((struct Cyc_Absyn_VarargInfo*)_check_null(f1.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp722,((struct Cyc_Absyn_VarargInfo*)_check_null(f2.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((f1.cyc_varargs)->type,(f2.cyc_varargs)->type);
if(r != 0)return r;
if((f1.cyc_varargs)->inject && !(f2.cyc_varargs)->inject)return 1;
if(!(f1.cyc_varargs)->inject &&(f2.cyc_varargs)->inject)return - 1;}
# 1392
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,f1.effect,f2.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp28D=f1.rgn_po;struct Cyc_List_List*rpo1=_tmp28D;
struct Cyc_List_List*_tmp28E=f2.rgn_po;struct Cyc_List_List*rpo2=_tmp28E;
for(0;rpo1 != 0 && rpo2 != 0;(rpo1=rpo1->tl,rpo2=rpo2->tl)){
struct _tuple15 _tmp28F=*((struct _tuple15*)rpo1->hd);struct _tuple15 _stmttmp41=_tmp28F;struct _tuple15 _tmp290=_stmttmp41;void*_tmp292;void*_tmp291;_LL1E: _tmp291=_tmp290.f1;_tmp292=_tmp290.f2;_LL1F: {void*t1a=_tmp291;void*t1b=_tmp292;
struct _tuple15 _tmp293=*((struct _tuple15*)rpo2->hd);struct _tuple15 _stmttmp42=_tmp293;struct _tuple15 _tmp294=_stmttmp42;void*_tmp296;void*_tmp295;_LL21: _tmp295=_tmp294.f1;_tmp296=_tmp294.f2;_LL22: {void*t2a=_tmp295;void*t2b=_tmp296;
r=Cyc_Tcutil_typecmp(t1a,t2a);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(t1b,t2b);if(r != 0)return r;}}}
# 1402
if(rpo1 != 0)return 1;
if(rpo2 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.requires_clause,f2.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_uint_exp_cmp,f1.ensures_clause,f2.ensures_clause);
if(r != 0)return r;
# 1410
({void*_tmp297=0U;({struct _fat_ptr _tmp723=({const char*_tmp298="typecmp: function type comparison should never get here!";_tag_fat(_tmp298,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp723,_tag_fat(_tmp297,sizeof(void*),0U));});});}}}}}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24E.f2)->tag == 6U){_LLD: _tmp257=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24E.f1)->f1;_tmp258=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp24E.f2)->f1;_LLE: {struct Cyc_List_List*ts2=_tmp257;struct Cyc_List_List*ts1=_tmp258;
# 1413
return((int(*)(int(*cmp)(struct _tuple12*,struct _tuple12*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,ts1,ts2);}}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24E.f2)->tag == 7U){_LLF: _tmp253=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24E.f1)->f1;_tmp254=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24E.f1)->f2;_tmp255=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24E.f2)->f1;_tmp256=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp24E.f2)->f2;_LL10: {enum Cyc_Absyn_AggrKind k2=_tmp253;struct Cyc_List_List*fs2=_tmp254;enum Cyc_Absyn_AggrKind k1=_tmp255;struct Cyc_List_List*fs1=_tmp256;
# 1416
if((int)k1 != (int)k2){
if((int)k1 == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,fs1,fs2);}}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24E.f2)->tag == 9U){_LL11: _tmp251=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24E.f1)->f1;_tmp252=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp24E.f2)->f1;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp251;struct Cyc_Absyn_Exp*e2=_tmp252;
# 1421
_tmp24F=e1;_tmp250=e2;goto _LL14;}}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24E.f2)->tag == 11U){_LL13: _tmp24F=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24E.f1)->f1;_tmp250=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp24E.f2)->f1;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp24F;struct Cyc_Absyn_Exp*e2=_tmp250;
# 1423
return Cyc_Evexp_uint_exp_cmp(e1,e2);}}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp299=0U;({struct _fat_ptr _tmp724=({const char*_tmp29A="Unmatched case in typecmp";_tag_fat(_tmp29A,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp724,_tag_fat(_tmp299,sizeof(void*),0U));});});}_LL0:;}}}
# 1430
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple15 _tmp29B=({struct _tuple15 _tmp633;({void*_tmp726=Cyc_Tcutil_compress(t1);_tmp633.f1=_tmp726;}),({void*_tmp725=Cyc_Tcutil_compress(t2);_tmp633.f2=_tmp725;});_tmp633;});struct _tuple15 _stmttmp43=_tmp29B;struct _tuple15 _tmp29C=_stmttmp43;void*_tmp29E;void*_tmp29D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29C.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29C.f2)->tag == 0U){_LL1: _tmp29D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29C.f1)->f1;_tmp29E=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp29C.f2)->f1;_LL2: {void*c1=_tmp29D;void*c2=_tmp29E;
# 1433
struct _tuple15 _tmp29F=({struct _tuple15 _tmp632;_tmp632.f1=c1,_tmp632.f2=c2;_tmp632;});struct _tuple15 _stmttmp44=_tmp29F;struct _tuple15 _tmp2A0=_stmttmp44;int _tmp2A2;int _tmp2A1;switch(*((int*)_tmp2A0.f1)){case 2U: switch(*((int*)_tmp2A0.f2)){case 2U: _LL6: _tmp2A1=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f1;_tmp2A2=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f1;_LL7: {int i1=_tmp2A1;int i2=_tmp2A2;
return i2 < i1;}case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp2A0.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 1441
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 1446
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 1449
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f1 == 0){_LL14: _LL15:
# 1444
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp2A0.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 1442
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 1447
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 1450
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f1 == 0){_LL16: _LL17:
# 1445
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 1451
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A0.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 1448
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 1452
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 1454
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 1456
 return 0;}_LL0:;}
# 1460
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple15 _tmp2A3=({struct _tuple15 _tmp635;({void*_tmp728=Cyc_Tcutil_compress(t1);_tmp635.f1=_tmp728;}),({void*_tmp727=Cyc_Tcutil_compress(t2);_tmp635.f2=_tmp727;});_tmp635;});struct _tuple15 _stmttmp45=_tmp2A3;struct _tuple15 _tmp2A4=_stmttmp45;void*_tmp2A6;void*_tmp2A5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4.f2)->tag == 0U){_LL1: _tmp2A5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4.f1)->f1;_tmp2A6=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2A4.f2)->f1;_LL2: {void*c1=_tmp2A5;void*c2=_tmp2A6;
# 1463
{struct _tuple15 _tmp2A7=({struct _tuple15 _tmp634;_tmp634.f1=c1,_tmp634.f2=c2;_tmp634;});struct _tuple15 _stmttmp46=_tmp2A7;struct _tuple15 _tmp2A8=_stmttmp46;int _tmp2AA;int _tmp2A9;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A8.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A8.f2)->tag == 2U){_LL6: _tmp2A9=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f1;_tmp2AA=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f1;_LL7: {int i1=_tmp2A9;int i2=_tmp2AA;
# 1465
if(i1 != 0 && i1 != 1)return t1;else{
if(i2 != 0 && i2 != 1)return t2;else{
if(i1 >= i2)return t1;else{
return t2;}}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp2A8.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 1475
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A8.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 1480
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 1473
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A8.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 1482
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 1472
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 1476
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A8.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 1481
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 1474
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A8.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp2A8.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 1483
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A8.f1)->tag == 4U){_LL18: _LL19:
# 1478
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp2A8.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 1484
 goto _LL5;}}}}}}_LL5:;}
# 1486
goto _LL0;}}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1489
return Cyc_Absyn_sint_type;}
# 1494
int Cyc_Tcutil_coerce_list(struct Cyc_RgnOrder_RgnPO*po,void*t,struct Cyc_List_List*es){
# 1497
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 ||
 Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp2AB=_cycalloc(sizeof(*_tmp2AB));_tmp2AB->v=t1;_tmp2AB;});}}}
# 1506
if(max_arith_type != 0){
if(!Cyc_Unify_unify(t,(void*)max_arith_type->v))
return 0;}
# 1510
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(po,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp2AE=({struct Cyc_String_pa_PrintArg_struct _tmp637;_tmp637.tag=0U,({
struct _fat_ptr _tmp729=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp637.f1=_tmp729;});_tmp637;});struct Cyc_String_pa_PrintArg_struct _tmp2AF=({struct Cyc_String_pa_PrintArg_struct _tmp636;_tmp636.tag=0U,({struct _fat_ptr _tmp72A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp636.f1=_tmp72A;});_tmp636;});void*_tmp2AC[2U];_tmp2AC[0]=& _tmp2AE,_tmp2AC[1]=& _tmp2AF;({unsigned _tmp72C=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _fat_ptr _tmp72B=({const char*_tmp2AD="type mismatch: expecting %s but found %s";_tag_fat(_tmp2AD,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp72C,_tmp72B,_tag_fat(_tmp2AC,sizeof(void*),2U));});});
return 0;}}}
# 1516
return 1;}
# 1521
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(e)){
void*_tmp2B0=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp47=_tmp2B0;void*_tmp2B1=_stmttmp47;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2B1)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1527
return 1;}
# 1531
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 1535
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp2B2=0U;({unsigned _tmp72E=e->loc;struct _fat_ptr _tmp72D=({const char*_tmp2B3="integral size mismatch; conversion supplied";_tag_fat(_tmp2B3,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp72E,_tmp72D,_tag_fat(_tmp2B2,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1541
return 0;}
# 1545
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Absyn_Exp*e){
if(Cyc_Unify_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 1549
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp2B4=0U;({unsigned _tmp730=e->loc;struct _fat_ptr _tmp72F=({const char*_tmp2B5="integral size mismatch; conversion supplied";_tag_fat(_tmp2B5,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp730,_tmp72F,_tag_fat(_tmp2B4,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 1555
return 0;}
# 1560
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Unify_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 1566
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp731=t;Cyc_Unify_unify(_tmp731,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 1571
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp2B6=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);void*t=_tmp2B6;
void*_tmp2B7=t;struct Cyc_Absyn_Exp*_tmp2B8;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B7)->tag == 9U){_LL1: _tmp2B8=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2B7)->f1;_LL2: {struct Cyc_Absyn_Exp*e=_tmp2B8;
return e;}}else{_LL3: _LL4: {
# 1576
struct Cyc_Absyn_Exp*_tmp2B9=Cyc_Absyn_valueof_exp(t,0U);struct Cyc_Absyn_Exp*v=_tmp2B9;
v->topt=Cyc_Absyn_uint_type;
return v;}}_LL0:;}
# 1585
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Unify_unify(def,b);{
void*_tmp2BA=Cyc_Tcutil_compress(b);void*_stmttmp48=_tmp2BA;void*_tmp2BB=_stmttmp48;struct Cyc_List_List*_tmp2BC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BB)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BB)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp2BC=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BB)->f2;_LL4: {struct Cyc_List_List*ts=_tmp2BC;
return Cyc_Tcutil_get_thin_bound(ts);}default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp2BF=({struct Cyc_String_pa_PrintArg_struct _tmp638;_tmp638.tag=0U,({struct _fat_ptr _tmp732=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp638.f1=_tmp732;});_tmp638;});void*_tmp2BD[1U];_tmp2BD[0]=& _tmp2BF;({struct _fat_ptr _tmp733=({const char*_tmp2BE="get_bounds_exp: %s";_tag_fat(_tmp2BE,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp733,_tag_fat(_tmp2BD,sizeof(void*),1U));});});}_LL0:;}}
# 1594
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp2C0=Cyc_Tcutil_compress(t);void*_stmttmp49=_tmp2C0;void*_tmp2C1=_stmttmp49;void*_tmp2C2;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1)->tag == 3U){_LL1: _tmp2C2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C1)->f1).ptr_atts).bounds;_LL2: {void*b=_tmp2C2;
# 1597
return Cyc_Tcutil_get_bounds_exp(def,b);}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp2C5=({struct Cyc_String_pa_PrintArg_struct _tmp639;_tmp639.tag=0U,({struct _fat_ptr _tmp734=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp639.f1=_tmp734;});_tmp639;});void*_tmp2C3[1U];_tmp2C3[0]=& _tmp2C5;({struct _fat_ptr _tmp735=({const char*_tmp2C4="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp2C4,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp735,_tag_fat(_tmp2C3,sizeof(void*),1U));});});}_LL0:;}
# 1603
void*Cyc_Tcutil_any_bool(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp2C6=_cycalloc(sizeof(*_tmp2C6));_tmp2C6->v=tvs;_tmp2C6;}));}
# 1607
void*Cyc_Tcutil_any_bounds(struct Cyc_List_List*tvs){
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp2C7=_cycalloc(sizeof(*_tmp2C7));_tmp2C7->v=tvs;_tmp2C7;}));}
# 1612
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 1620
int Cyc_Tcutil_silent_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp2C8=({struct _tuple15 _tmp640;_tmp640.f1=t1,_tmp640.f2=t2;_tmp640;});struct _tuple15 _stmttmp4A=_tmp2C8;struct _tuple15 _tmp2C9=_stmttmp4A;void*_tmp2D1;struct Cyc_Absyn_Exp*_tmp2D0;struct Cyc_Absyn_Tqual _tmp2CF;void*_tmp2CE;void*_tmp2CD;struct Cyc_Absyn_Exp*_tmp2CC;struct Cyc_Absyn_Tqual _tmp2CB;void*_tmp2CA;struct Cyc_Absyn_PtrInfo _tmp2D3;struct Cyc_Absyn_PtrInfo _tmp2D2;switch(*((int*)_tmp2C9.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C9.f2)->tag == 3U){_LL1: _tmp2D2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C9.f1)->f1;_tmp2D3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2C9.f2)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pinfo_a=_tmp2D2;struct Cyc_Absyn_PtrInfo pinfo_b=_tmp2D3;
# 1625
int okay=1;
# 1627
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).nullable,(pinfo_b.ptr_atts).nullable))
# 1629
okay=!Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).nullable);
# 1631
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).bounds,(pinfo_b.ptr_atts).bounds)){
struct _tuple22 _tmp2D4=({struct _tuple22 _tmp63B;({struct Cyc_Absyn_Exp*_tmp739=({void*_tmp738=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp738,(pinfo_a.ptr_atts).bounds);});_tmp63B.f1=_tmp739;}),({
struct Cyc_Absyn_Exp*_tmp737=({void*_tmp736=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp736,(pinfo_b.ptr_atts).bounds);});_tmp63B.f2=_tmp737;});_tmp63B;});
# 1632
struct _tuple22 _stmttmp4B=_tmp2D4;struct _tuple22 _tmp2D5=_stmttmp4B;struct Cyc_Absyn_Exp*_tmp2D7;struct Cyc_Absyn_Exp*_tmp2D6;if(_tmp2D5.f2 == 0){_LLA: _LLB:
# 1635
 okay=1;goto _LL9;}else{if(_tmp2D5.f1 == 0){_LLC: _LLD:
# 1638
 if(Cyc_Tcutil_force_type2bool(0,(pinfo_a.ptr_atts).zero_term)&&({
void*_tmp73A=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp73A,(pinfo_b.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp2D6=_tmp2D5.f1;_tmp2D7=_tmp2D5.f2;_LLF: {struct Cyc_Absyn_Exp*e1=_tmp2D6;struct Cyc_Absyn_Exp*e2=_tmp2D7;
# 1644
okay=okay &&({struct Cyc_Absyn_Exp*_tmp73B=(struct Cyc_Absyn_Exp*)_check_null(e2);Cyc_Evexp_lte_const_exp(_tmp73B,(struct Cyc_Absyn_Exp*)_check_null(e1));});
# 1648
if(!Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2D9=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63A;_tmp63A.tag=0U,({struct _fat_ptr _tmp73C=({const char*_tmp2DA="implicit cast to shorter array";_tag_fat(_tmp2DA,sizeof(char),31U);});_tmp63A.f1=_tmp73C;});_tmp63A;});void*_tmp2D8[1U];_tmp2D8[0]=& _tmp2D9;({unsigned _tmp73D=loc;Cyc_Warn_warn2(_tmp73D,_tag_fat(_tmp2D8,sizeof(void*),1U));});});
goto _LL9;}}}_LL9:;}
# 1654
okay=okay &&(!(pinfo_a.elt_tq).real_const ||(pinfo_b.elt_tq).real_const);
# 1656
if(!Cyc_Unify_unify((pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
if(Cyc_RgnOrder_rgn_outlives_rgn(po,(pinfo_a.ptr_atts).rgn,(pinfo_b.ptr_atts).rgn)){
# 1659
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp2DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63F;_tmp63F.tag=0U,({struct _fat_ptr _tmp73E=({const char*_tmp2E1="implicit cast from region ";_tag_fat(_tmp2E1,sizeof(char),27U);});_tmp63F.f1=_tmp73E;});_tmp63F;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DD=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63E;_tmp63E.tag=2U,_tmp63E.f1=(void*)(pinfo_a.ptr_atts).rgn;_tmp63E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp2DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp63D;_tmp63D.tag=0U,({
struct _fat_ptr _tmp73F=({const char*_tmp2E0=" to region ";_tag_fat(_tmp2E0,sizeof(char),12U);});_tmp63D.f1=_tmp73F;});_tmp63D;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp2DF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp63C;_tmp63C.tag=2U,_tmp63C.f1=(void*)(pinfo_b.ptr_atts).rgn;_tmp63C;});void*_tmp2DB[4U];_tmp2DB[0]=& _tmp2DC,_tmp2DB[1]=& _tmp2DD,_tmp2DB[2]=& _tmp2DE,_tmp2DB[3]=& _tmp2DF;({unsigned _tmp740=loc;Cyc_Warn_warn2(_tmp740,_tag_fat(_tmp2DB,sizeof(void*),4U));});});}else{
okay=0;}}
# 1665
okay=okay &&(Cyc_Unify_unify((pinfo_a.ptr_atts).zero_term,(pinfo_b.ptr_atts).zero_term)||
# 1667
 Cyc_Tcutil_force_type2bool(1,(pinfo_a.ptr_atts).zero_term)&&(pinfo_b.elt_tq).real_const);{
# 1675
int _tmp2E2=
({void*_tmp741=Cyc_Absyn_bounds_one();Cyc_Unify_unify(_tmp741,(pinfo_b.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(pinfo_b.ptr_atts).zero_term);
# 1675
int deep_subtype=_tmp2E2;
# 1679
okay=okay &&(Cyc_Unify_unify(pinfo_a.elt_type,pinfo_b.elt_type)||
(deep_subtype &&((pinfo_b.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(pinfo_b.elt_type))))&& Cyc_Tcutil_ptrsubtype(po,0,pinfo_a.elt_type,pinfo_b.elt_type));
return okay;}}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f2)->tag == 4U){_LL3: _tmp2CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f1)->f1).elt_type;_tmp2CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f1)->f1).tq;_tmp2CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f1)->f1).num_elts;_tmp2CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f1)->f1).zero_term;_tmp2CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f2)->f1).elt_type;_tmp2CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f2)->f1).tq;_tmp2D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f2)->f1).num_elts;_tmp2D1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2C9.f2)->f1).zero_term;_LL4: {void*t1a=_tmp2CA;struct Cyc_Absyn_Tqual tq1a=_tmp2CB;struct Cyc_Absyn_Exp*e1=_tmp2CC;void*zt1=_tmp2CD;void*t2a=_tmp2CE;struct Cyc_Absyn_Tqual tq2a=_tmp2CF;struct Cyc_Absyn_Exp*e2=_tmp2D0;void*zt2=_tmp2D1;
# 1685
int okay;
# 1688
okay=Cyc_Unify_unify(zt1,zt2)&&(
(e1 != 0 && e2 != 0)&& Cyc_Evexp_same_uint_const_exp(e1,e2));
# 1691
return(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C9.f2)->f1)->tag == 1U){_LL5: _LL6:
# 1693
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 1695
 return Cyc_Unify_unify(t1,t2);}_LL0:;}}
# 1699
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp2E3=Cyc_Tcutil_compress(t);void*_stmttmp4C=_tmp2E3;void*_tmp2E4=_stmttmp4C;void*_tmp2E5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E4)->tag == 3U){_LL1: _tmp2E5=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E4)->f1).elt_type;_LL2: {void*e=_tmp2E5;
return e;}}else{_LL3: _LL4:
({void*_tmp2E6=0U;({struct _fat_ptr _tmp742=({const char*_tmp2E7="pointer_elt_type";_tag_fat(_tmp2E7,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp742,_tag_fat(_tmp2E6,sizeof(void*),0U));});});}_LL0:;}
# 1705
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp2E8=Cyc_Tcutil_compress(t);void*_stmttmp4D=_tmp2E8;void*_tmp2E9=_stmttmp4D;struct Cyc_Absyn_PtrAtts*_tmp2EA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9)->tag == 3U){_LL1: _tmp2EA=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2E9)->f1).ptr_atts;_LL2: {struct Cyc_Absyn_PtrAtts*p=_tmp2EA;
return p->rgn;}}else{_LL3: _LL4:
({void*_tmp2EB=0U;({struct _fat_ptr _tmp743=({const char*_tmp2EC="pointer_elt_type";_tag_fat(_tmp2EC,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp743,_tag_fat(_tmp2EB,sizeof(void*),0U));});});}_LL0:;}
# 1712
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp2ED=Cyc_Tcutil_compress(t);void*_stmttmp4E=_tmp2ED;void*_tmp2EE=_stmttmp4E;void*_tmp2EF;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EE)->tag == 3U){_LL1: _tmp2EF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2EE)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp2EF;
# 1715
*rgn=r;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1724
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp2F0=Cyc_Tcutil_compress(t);void*_stmttmp4F=_tmp2F0;void*_tmp2F1=_stmttmp4F;void*_tmp2F3;void*_tmp2F2;switch(*((int*)_tmp2F1)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F1)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp2F2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->f1).ptr_atts).nullable;_tmp2F3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2F1)->f1).ptr_atts).bounds;_LL6: {void*nullable=_tmp2F2;void*bounds=_tmp2F3;
# 1732
return !Cyc_Unify_unify(Cyc_Absyn_fat_bound_type,bounds)&& Cyc_Tcutil_force_type2bool(0,nullable);}default: _LL7: _LL8:
 return 0;}_LL0:;}
# 1738
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp2F4=e->r;void*_stmttmp50=_tmp2F4;void*_tmp2F5=_stmttmp50;struct Cyc_Absyn_Exp*_tmp2F7;void*_tmp2F6;struct _fat_ptr _tmp2F8;switch(*((int*)_tmp2F5)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp2F8=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1).Wchar_c).val;_LLC: {struct _fat_ptr s=_tmp2F8;
# 1747
unsigned long _tmp2F9=Cyc_strlen((struct _fat_ptr)s);unsigned long l=_tmp2F9;
int i=0;
if(l >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)s.curr)[1]== (int)'x' && l >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < l;++ i){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 1757
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 1745
 return 1;case 14U: _LLD: _tmp2F6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F5)->f1;_tmp2F7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp2F5)->f2;_LLE: {void*t=_tmp2F6;struct Cyc_Absyn_Exp*e2=_tmp2F7;
# 1758
return Cyc_Tcutil_is_zero(e2)&& Cyc_Tcutil_admits_zero(t);}default: _LLF: _LL10:
 return 0;}_LL0:;}
# 1763
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 1772
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 1777
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 1782
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 1791
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 1796
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 1801
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp2FA=ka;enum Cyc_Absyn_AliasQual _tmp2FC;enum Cyc_Absyn_KindQual _tmp2FB;_LL1: _tmp2FB=_tmp2FA->kind;_tmp2FC=_tmp2FA->aliasqual;_LL2: {enum Cyc_Absyn_KindQual k=_tmp2FB;enum Cyc_Absyn_AliasQual a=_tmp2FC;
{enum Cyc_Absyn_AliasQual _tmp2FD=a;switch(_tmp2FD){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 1805
enum Cyc_Absyn_KindQual _tmp2FE=k;switch(_tmp2FE){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 1816
{enum Cyc_Absyn_KindQual _tmp2FF=k;switch(_tmp2FF){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 1823
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 1825
{enum Cyc_Absyn_KindQual _tmp300=k;switch(_tmp300){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 1832
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 1835
({struct Cyc_String_pa_PrintArg_struct _tmp303=({struct Cyc_String_pa_PrintArg_struct _tmp641;_tmp641.tag=0U,({struct _fat_ptr _tmp744=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp641.f1=_tmp744;});_tmp641;});void*_tmp301[1U];_tmp301[0]=& _tmp303;({struct _fat_ptr _tmp745=({const char*_tmp302="kind_to_opt: bad kind %s\n";_tag_fat(_tmp302,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp745,_tag_fat(_tmp301,sizeof(void*),1U));});});}}
# 1838
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp304=_cycalloc(sizeof(*_tmp304));_tmp304->tag=0U,_tmp304->f1=k;_tmp304;});}
# 1841
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp305=_cycalloc(sizeof(*_tmp305));({void*_tmp746=Cyc_Tcutil_kind_to_bound(k);_tmp305->v=_tmp746;});_tmp305;});}
# 1847
int Cyc_Tcutil_zero_to_null(void*t2,struct Cyc_Absyn_Exp*e1){
if(!Cyc_Tcutil_is_zero(e1))
return 0;{
void*_tmp306=Cyc_Tcutil_compress(t2);void*_stmttmp51=_tmp306;void*_tmp307=_stmttmp51;void*_tmp308;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp307)->tag == 3U){_LL1: _tmp308=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp307)->f1).ptr_atts).nullable;_LL2: {void*nbl=_tmp308;
# 1852
if(!Cyc_Tcutil_force_type2bool(1,nbl))
return 0;
({void*_tmp747=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp747;});
e1->topt=t2;
return 1;}}else{_LL3: _LL4:
 return 0;}_LL0:;}}
# 1861
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 1867
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 1871
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp31C=_cycalloc(sizeof(*_tmp31C));_tmp31C->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp74A=({struct _fat_ptr*_tmp31B=_cycalloc(sizeof(*_tmp31B));({struct _fat_ptr _tmp749=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp31A=({struct Cyc_Int_pa_PrintArg_struct _tmp643;_tmp643.tag=1U,_tmp643.f1=(unsigned long)counter ++;_tmp643;});void*_tmp318[1U];_tmp318[0]=& _tmp31A;({struct _fat_ptr _tmp748=({const char*_tmp319="__aliasvar%d";_tag_fat(_tmp319,sizeof(char),13U);});Cyc_aprintf(_tmp748,_tag_fat(_tmp318,sizeof(void*),1U));});});*_tmp31B=_tmp749;});_tmp31B;});_tmp31C->f2=_tmp74A;});_tmp31C;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp74B=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp317=_cycalloc(sizeof(*_tmp317));_tmp317->tag=4U,_tmp317->f1=vd;_tmp317;});Cyc_Absyn_varb_exp(_tmp74B,e->loc);});
# 1880
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
{void*_tmp309=Cyc_Tcutil_compress(e_type);void*_stmttmp52=_tmp309;void*_tmp30A=_stmttmp52;struct Cyc_Absyn_PtrLoc*_tmp311;void*_tmp310;void*_tmp30F;void*_tmp30E;void*_tmp30D;struct Cyc_Absyn_Tqual _tmp30C;void*_tmp30B;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->tag == 3U){_LL1: _tmp30B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).elt_type;_tmp30C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).elt_tq;_tmp30D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).rgn;_tmp30E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).nullable;_tmp30F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).bounds;_tmp310=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).zero_term;_tmp311=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp30A)->f1).ptr_atts).ptrloc;_LL2: {void*et=_tmp30B;struct Cyc_Absyn_Tqual tq=_tmp30C;void*old_r=_tmp30D;void*nb=_tmp30E;void*b=_tmp30F;void*zt=_tmp310;struct Cyc_Absyn_PtrLoc*pl=_tmp311;
# 1883
{void*_tmp312=Cyc_Tcutil_compress(old_r);void*_stmttmp53=_tmp312;void*_tmp313=_stmttmp53;struct Cyc_Core_Opt*_tmp315;void**_tmp314;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->tag == 1U){_LL6: _tmp314=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->f2;_tmp315=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp313)->f4;_LL7: {void**topt=_tmp314;struct Cyc_Core_Opt*ts=_tmp315;
# 1885
void*_tmp316=Cyc_Absyn_var_type(tv);void*new_r=_tmp316;
*topt=new_r;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 1890
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 1893
e->topt=0;
vd->initializer=0;{
# 1897
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 1899
return({struct _tuple23 _tmp642;_tmp642.f1=d,_tmp642.f2=ve;_tmp642;});}}
# 1904
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 1907
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 1912
void*_tmp31D=Cyc_Tcutil_compress(wants_type);void*_stmttmp54=_tmp31D;void*_tmp31E=_stmttmp54;void*_tmp31F;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31E)->tag == 3U){_LL1: _tmp31F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp31E)->f1).ptr_atts).rgn;_LL2: {void*r2=_tmp31F;
# 1914
if(Cyc_Tcutil_is_heap_rgn_type(r2))return 0;{
struct Cyc_Absyn_Kind*_tmp320=Cyc_Tcutil_type_kind(r2);struct Cyc_Absyn_Kind*k=_tmp320;
return(int)k->kind == (int)Cyc_Absyn_RgnKind &&(int)k->aliasqual == (int)Cyc_Absyn_Aliasable;}}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 1920
return 0;}
# 1924
int Cyc_Tcutil_coerce_arg(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
# 1926
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 1930
if(Cyc_Unify_unify(t1,t2))
return 1;
# 1933
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 1935
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp322=({struct Cyc_Warn_String_Warn_Warg_struct _tmp648;_tmp648.tag=0U,({struct _fat_ptr _tmp74C=({const char*_tmp329="integral size mismatch; ";_tag_fat(_tmp329,sizeof(char),25U);});_tmp648.f1=_tmp74C;});_tmp648;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp323=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp647;_tmp647.tag=2U,_tmp647.f1=(void*)t1;_tmp647;});struct Cyc_Warn_String_Warn_Warg_struct _tmp324=({struct Cyc_Warn_String_Warn_Warg_struct _tmp646;_tmp646.tag=0U,({
struct _fat_ptr _tmp74D=({const char*_tmp328=" -> ";_tag_fat(_tmp328,sizeof(char),5U);});_tmp646.f1=_tmp74D;});_tmp646;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp325=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp645;_tmp645.tag=2U,_tmp645.f1=(void*)t2;_tmp645;});struct Cyc_Warn_String_Warn_Warg_struct _tmp326=({struct Cyc_Warn_String_Warn_Warg_struct _tmp644;_tmp644.tag=0U,({struct _fat_ptr _tmp74E=({const char*_tmp327=" conversion supplied";_tag_fat(_tmp327,sizeof(char),21U);});_tmp644.f1=_tmp74E;});_tmp644;});void*_tmp321[5U];_tmp321[0]=& _tmp322,_tmp321[1]=& _tmp323,_tmp321[2]=& _tmp324,_tmp321[3]=& _tmp325,_tmp321[4]=& _tmp326;({unsigned _tmp74F=e->loc;Cyc_Warn_warn2(_tmp74F,_tag_fat(_tmp321,sizeof(void*),5U));});});
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_No_coercion);
return 1;}
# 1943
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp32B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64E;_tmp64E.tag=0U,({struct _fat_ptr _tmp750=({const char*_tmp333="implicit alias coercion for ";_tag_fat(_tmp333,sizeof(char),29U);});_tmp64E.f1=_tmp750;});_tmp64E;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp32C=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp64D;_tmp64D.tag=4U,_tmp64D.f1=e;_tmp64D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32D=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64C;_tmp64C.tag=0U,({struct _fat_ptr _tmp751=({const char*_tmp332=":";_tag_fat(_tmp332,sizeof(char),2U);});_tmp64C.f1=_tmp751;});_tmp64C;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp32E=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64B;_tmp64B.tag=2U,_tmp64B.f1=(void*)t1;_tmp64B;});struct Cyc_Warn_String_Warn_Warg_struct _tmp32F=({struct Cyc_Warn_String_Warn_Warg_struct _tmp64A;_tmp64A.tag=0U,({struct _fat_ptr _tmp752=({const char*_tmp331=" to ";_tag_fat(_tmp331,sizeof(char),5U);});_tmp64A.f1=_tmp752;});_tmp64A;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp330=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp649;_tmp649.tag=2U,_tmp649.f1=(void*)t2;_tmp649;});void*_tmp32A[6U];_tmp32A[0]=& _tmp32B,_tmp32A[1]=& _tmp32C,_tmp32A[2]=& _tmp32D,_tmp32A[3]=& _tmp32E,_tmp32A[4]=& _tmp32F,_tmp32A[5]=& _tmp330;({unsigned _tmp753=e->loc;Cyc_Warn_warn2(_tmp753,_tag_fat(_tmp32A,sizeof(void*),6U));});});
if(alias_coercion != 0)
*alias_coercion=1;}
# 1950
if(Cyc_Tcutil_silent_castable(po,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(e,t2,Cyc_Absyn_Other_coercion);
return 1;}
# 1954
if(Cyc_Tcutil_zero_to_null(t2,e))
return 1;
if((int)(c=Cyc_Tcutil_castable(po,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 1958
if((int)c != (int)1U)
Cyc_Tcutil_unchecked_cast(e,t2,c);
if((int)c != (int)2U)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp335=({struct Cyc_Warn_String_Warn_Warg_struct _tmp652;_tmp652.tag=0U,({struct _fat_ptr _tmp754=({const char*_tmp33A="implicit cast from ";_tag_fat(_tmp33A,sizeof(char),20U);});_tmp652.f1=_tmp754;});_tmp652;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp336=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp651;_tmp651.tag=2U,_tmp651.f1=(void*)t1;_tmp651;});struct Cyc_Warn_String_Warn_Warg_struct _tmp337=({struct Cyc_Warn_String_Warn_Warg_struct _tmp650;_tmp650.tag=0U,({struct _fat_ptr _tmp755=({const char*_tmp339=" to ";_tag_fat(_tmp339,sizeof(char),5U);});_tmp650.f1=_tmp755;});_tmp650;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp338=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp64F;_tmp64F.tag=2U,_tmp64F.f1=(void*)t2;_tmp64F;});void*_tmp334[4U];_tmp334[0]=& _tmp335,_tmp334[1]=& _tmp336,_tmp334[2]=& _tmp337,_tmp334[3]=& _tmp338;({unsigned _tmp756=e->loc;Cyc_Warn_warn2(_tmp756,_tag_fat(_tmp334,sizeof(void*),4U));});});
return 1;}
# 1964
return 0;}
# 1970
int Cyc_Tcutil_coerce_assign(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_Absyn_Exp*e,void*t){
# 1973
int bogus=0;
return Cyc_Tcutil_coerce_arg(po,e,t,& bogus);}
# 1987 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*,int,void*);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;int f3;};
# 1989
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 1992
struct _tuple24 _tmp33B=*env;struct _tuple24 _stmttmp55=_tmp33B;struct _tuple24 _tmp33C=_stmttmp55;int _tmp33F;struct _RegionHandle*_tmp33E;struct Cyc_List_List*_tmp33D;_LL1: _tmp33D=_tmp33C.f1;_tmp33E=_tmp33C.f2;_tmp33F=_tmp33C.f3;_LL2: {struct Cyc_List_List*inst=_tmp33D;struct _RegionHandle*r=_tmp33E;int flatten=_tmp33F;
void*_tmp340=inst == 0?x->type: Cyc_Tcutil_rsubstitute(r,inst,x->type);void*t=_tmp340;
struct Cyc_List_List*_tmp341=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp341;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp343=_region_malloc(r,sizeof(*_tmp343));({struct _tuple12*_tmp757=({struct _tuple12*_tmp342=_region_malloc(r,sizeof(*_tmp342));_tmp342->f1=x->tq,_tmp342->f2=t;_tmp342;});_tmp343->hd=_tmp757;}),_tmp343->tl=0;_tmp343;});
return ts;}}struct _tuple25{struct _RegionHandle*f1;int f2;};
# 1999
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple12*x){
# 2001
struct _tuple25 _tmp344=*env;struct _tuple25 _stmttmp56=_tmp344;struct _tuple25 _tmp345=_stmttmp56;int _tmp347;struct _RegionHandle*_tmp346;_LL1: _tmp346=_tmp345.f1;_tmp347=_tmp345.f2;_LL2: {struct _RegionHandle*r=_tmp346;int flatten=_tmp347;
struct _tuple12 _tmp348=*x;struct _tuple12 _stmttmp57=_tmp348;struct _tuple12 _tmp349=_stmttmp57;void*_tmp34B;struct Cyc_Absyn_Tqual _tmp34A;_LL4: _tmp34A=_tmp349.f1;_tmp34B=_tmp349.f2;_LL5: {struct Cyc_Absyn_Tqual tq=_tmp34A;void*t=_tmp34B;
struct Cyc_List_List*_tmp34C=Cyc_Tcutil_flatten_type(r,flatten,t);struct Cyc_List_List*ts=_tmp34C;
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts)== 1)
return({struct Cyc_List_List*_tmp34E=_region_malloc(r,sizeof(*_tmp34E));({struct _tuple12*_tmp758=({struct _tuple12*_tmp34D=_region_malloc(r,sizeof(*_tmp34D));_tmp34D->f1=tq,_tmp34D->f2=t;_tmp34D;});_tmp34E->hd=_tmp758;}),_tmp34E->tl=0;_tmp34E;});
return ts;}}}
# 2008
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,void*t1){
# 2011
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp34F=t1;struct Cyc_List_List*_tmp350;struct Cyc_List_List*_tmp351;struct Cyc_List_List*_tmp353;struct Cyc_Absyn_Aggrdecl*_tmp352;switch(*((int*)_tmp34F)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp352=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f1)->f1).KnownAggr).val;_tmp353=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp34F)->f2;_LL6: {struct Cyc_Absyn_Aggrdecl*ad=_tmp352;struct Cyc_List_List*ts=_tmp353;
# 2029
if((((int)ad->kind == (int)Cyc_Absyn_UnionA || ad->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->rgn_po != 0)
# 2031
return({struct Cyc_List_List*_tmp35E=_region_malloc(r,sizeof(*_tmp35E));({struct _tuple12*_tmp75A=({struct _tuple12*_tmp35D=_region_malloc(r,sizeof(*_tmp35D));({struct Cyc_Absyn_Tqual _tmp759=Cyc_Absyn_empty_tqual(0U);_tmp35D->f1=_tmp759;}),_tmp35D->f2=t1;_tmp35D;});_tmp35E->hd=_tmp75A;}),_tmp35E->tl=0;_tmp35E;});{
struct Cyc_List_List*_tmp35F=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,ad->tvs,ts);struct Cyc_List_List*inst=_tmp35F;
struct _tuple24 env=({struct _tuple24 _tmp653;_tmp653.f1=inst,_tmp653.f2=r,_tmp653.f3=flatten;_tmp653;});
struct Cyc_List_List*_tmp360=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields;struct Cyc_List_List*_stmttmp58=_tmp360;struct Cyc_List_List*_tmp361=_stmttmp58;struct Cyc_List_List*_tmp363;struct Cyc_Absyn_Aggrfield*_tmp362;if(_tmp361 == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp362=(struct Cyc_Absyn_Aggrfield*)_tmp361->hd;_tmp363=_tmp361->tl;_LL14: {struct Cyc_Absyn_Aggrfield*hd=_tmp362;struct Cyc_List_List*tl=_tmp363;
# 2037
struct Cyc_List_List*_tmp364=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp364;
env.f3=0;{
struct Cyc_List_List*_tmp365=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp365;
struct Cyc_List_List*_tmp366=({struct Cyc_List_List*_tmp367=_region_malloc(r,sizeof(*_tmp367));_tmp367->hd=hd2,_tmp367->tl=tl2;_tmp367;});struct Cyc_List_List*tts=_tmp366;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL10:;}}}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp351=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp34F)->f1;_LL4: {struct Cyc_List_List*tqs=_tmp351;
# 2016
struct _tuple25 _tmp354=({struct _tuple25 _tmp654;_tmp654.f1=r,_tmp654.f2=flatten;_tmp654;});struct _tuple25 env=_tmp354;
# 2018
struct Cyc_List_List*_tmp355=tqs;struct Cyc_List_List*_tmp357;struct _tuple12*_tmp356;if(_tmp355 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp356=(struct _tuple12*)_tmp355->hd;_tmp357=_tmp355->tl;_LLF: {struct _tuple12*hd=_tmp356;struct Cyc_List_List*tl=_tmp357;
# 2021
struct Cyc_List_List*_tmp358=Cyc_Tcutil_rcopy_tqt(& env,hd);struct Cyc_List_List*hd2=_tmp358;
env.f2=0;{
struct Cyc_List_List*_tmp359=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple12*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& env,tqs);struct Cyc_List_List*tl2=_tmp359;
struct Cyc_List_List*_tmp35A=({struct Cyc_List_List*_tmp35C=_region_malloc(r,sizeof(*_tmp35C));_tmp35C->hd=hd2,_tmp35C->tl=tl2;_tmp35C;});struct Cyc_List_List*temp=_tmp35A;
return({struct _RegionHandle*_tmp75B=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp75B,({struct Cyc_List_List*_tmp35B=_region_malloc(r,sizeof(*_tmp35B));_tmp35B->hd=hd2,_tmp35B->tl=tl2;_tmp35B;}));});}}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp34F)->f1 == Cyc_Absyn_StructA){_LL7: _tmp350=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp34F)->f2;_LL8: {struct Cyc_List_List*fs=_tmp350;
# 2044
struct _tuple24 env=({struct _tuple24 _tmp655;_tmp655.f1=0,_tmp655.f2=r,_tmp655.f3=flatten;_tmp655;});
struct Cyc_List_List*_tmp368=fs;struct Cyc_List_List*_tmp36A;struct Cyc_Absyn_Aggrfield*_tmp369;if(_tmp368 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp369=(struct Cyc_Absyn_Aggrfield*)_tmp368->hd;_tmp36A=_tmp368->tl;_LL19: {struct Cyc_Absyn_Aggrfield*hd=_tmp369;struct Cyc_List_List*tl=_tmp36A;
# 2048
struct Cyc_List_List*_tmp36B=Cyc_Tcutil_flatten_type_f(& env,hd);struct Cyc_List_List*hd2=_tmp36B;
env.f3=0;{
struct Cyc_List_List*_tmp36C=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,tl);struct Cyc_List_List*tl2=_tmp36C;
struct Cyc_List_List*_tmp36D=({struct Cyc_List_List*_tmp36E=_region_malloc(r,sizeof(*_tmp36E));_tmp36E->hd=hd2,_tmp36E->tl=tl2;_tmp36E;});struct Cyc_List_List*tts=_tmp36D;
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,tts);}}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2054
 goto _LL0;}_LL0:;}}
# 2057
return({struct Cyc_List_List*_tmp370=_region_malloc(r,sizeof(*_tmp370));({struct _tuple12*_tmp75D=({struct _tuple12*_tmp36F=_region_malloc(r,sizeof(*_tmp36F));({struct Cyc_Absyn_Tqual _tmp75C=Cyc_Absyn_empty_tqual(0U);_tmp36F->f1=_tmp75C;}),_tmp36F->f2=t1;_tmp36F;});_tmp370->hd=_tmp75D;}),_tmp370->tl=0;_tmp370;});}
# 2061
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp371=(void*)t->hd;void*_stmttmp59=_tmp371;void*_tmp372=_stmttmp59;switch(*((int*)_tmp372)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
# 2068
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2071
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2074
return 1;}
# 2077
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple15 _tmp373=({struct _tuple15 _tmp656;({void*_tmp75F=Cyc_Tcutil_compress(t1);_tmp656.f1=_tmp75F;}),({void*_tmp75E=Cyc_Tcutil_compress(t2);_tmp656.f2=_tmp75E;});_tmp656;});struct _tuple15 _stmttmp5A=_tmp373;struct _tuple15 _tmp374=_stmttmp5A;enum Cyc_Absyn_Size_of _tmp376;enum Cyc_Absyn_Size_of _tmp375;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->f1)->tag == 1U){_LL1: _tmp375=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f1)->f1)->f2;_tmp376=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp374.f2)->f1)->f2;_LL2: {enum Cyc_Absyn_Size_of b1=_tmp375;enum Cyc_Absyn_Size_of b2=_tmp376;
# 2080
return((int)b1 == (int)b2 ||(int)b1 == (int)2U &&(int)b2 == (int)3U)||
(int)b1 == (int)3U &&(int)b2 == (int)Cyc_Absyn_Int_sz;}}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2088
int Cyc_Tcutil_subtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2090
if(Cyc_Unify_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Unify_unify(t1,(*((struct _tuple15*)a->hd)).f1)&& Cyc_Unify_unify(t2,(*((struct _tuple15*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple15 _tmp377=({struct _tuple15 _tmp657;_tmp657.f1=t1,_tmp657.f2=t2;_tmp657;});struct _tuple15 _stmttmp5B=_tmp377;struct _tuple15 _tmp378=_stmttmp5B;struct Cyc_Absyn_FnInfo _tmp37A;struct Cyc_Absyn_FnInfo _tmp379;struct Cyc_List_List*_tmp37F;struct Cyc_Absyn_Datatypedecl*_tmp37E;struct Cyc_List_List*_tmp37D;struct Cyc_Absyn_Datatypefield*_tmp37C;struct Cyc_Absyn_Datatypedecl*_tmp37B;void*_tmp38B;void*_tmp38A;void*_tmp389;void*_tmp388;struct Cyc_Absyn_Tqual _tmp387;void*_tmp386;void*_tmp385;void*_tmp384;void*_tmp383;void*_tmp382;struct Cyc_Absyn_Tqual _tmp381;void*_tmp380;switch(*((int*)_tmp378.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f2)->tag == 3U){_LL1: _tmp380=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f1)->f1).elt_type;_tmp381=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f1)->f1).elt_tq;_tmp382=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f1)->f1).ptr_atts).rgn;_tmp383=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f1)->f1).ptr_atts).nullable;_tmp384=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f1)->f1).ptr_atts).bounds;_tmp385=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f1)->f1).ptr_atts).zero_term;_tmp386=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f2)->f1).elt_type;_tmp387=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f2)->f1).elt_tq;_tmp388=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f2)->f1).ptr_atts).rgn;_tmp389=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f2)->f1).ptr_atts).nullable;_tmp38A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f2)->f1).ptr_atts).bounds;_tmp38B=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp378.f2)->f1).ptr_atts).zero_term;_LL2: {void*t_a=_tmp380;struct Cyc_Absyn_Tqual q_a=_tmp381;void*rt_a=_tmp382;void*null_a=_tmp383;void*b_a=_tmp384;void*zt_a=_tmp385;void*t_b=_tmp386;struct Cyc_Absyn_Tqual q_b=_tmp387;void*rt_b=_tmp388;void*null_b=_tmp389;void*b_b=_tmp38A;void*zt_b=_tmp38B;
# 2102
if(q_a.real_const && !q_b.real_const)
return 0;
# 2105
if((!Cyc_Unify_unify(null_a,null_b)&&
 Cyc_Absyn_type2bool(0,null_a))&& !Cyc_Absyn_type2bool(0,null_b))
return 0;
# 2109
if((Cyc_Unify_unify(zt_a,zt_b)&& !
Cyc_Absyn_type2bool(0,zt_a))&& Cyc_Absyn_type2bool(0,zt_b))
return 0;
# 2113
if((!Cyc_Unify_unify(rt_a,rt_b)&& !Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b))&& !
Cyc_Tcutil_subtype(po,assume,rt_a,rt_b))
return 0;
# 2117
if(!Cyc_Unify_unify(b_a,b_b)){
struct Cyc_Absyn_Exp*_tmp38C=({void*_tmp760=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp760,b_a);});struct Cyc_Absyn_Exp*e1=_tmp38C;
struct Cyc_Absyn_Exp*_tmp38D=({void*_tmp761=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp761,b_b);});struct Cyc_Absyn_Exp*e2=_tmp38D;
if(e1 != e2){
if((e1 == 0 || e2 == 0)|| !Cyc_Evexp_lte_const_exp(e2,e2))
return 0;}}
# 2126
if(!q_b.real_const && q_a.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t_b)))
return 0;}{
# 2131
int _tmp38E=({void*_tmp762=b_b;Cyc_Unify_unify(_tmp762,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);int deep_subtype=_tmp38E;
# 2135
return(deep_subtype &&({struct Cyc_RgnOrder_RgnPO*_tmp766=po;struct Cyc_List_List*_tmp765=({struct Cyc_List_List*_tmp390=_cycalloc(sizeof(*_tmp390));({struct _tuple15*_tmp763=({struct _tuple15*_tmp38F=_cycalloc(sizeof(*_tmp38F));_tmp38F->f1=t1,_tmp38F->f2=t2;_tmp38F;});_tmp390->hd=_tmp763;}),_tmp390->tl=assume;_tmp390;});void*_tmp764=t_a;Cyc_Tcutil_ptrsubtype(_tmp766,_tmp765,_tmp764,t_b);})|| Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b);}}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp37B=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp37C=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp37D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f1)->f2;_tmp37E=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f2)->f1)->f1).KnownDatatype).val;_tmp37F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp378.f2)->f2;_LL4: {struct Cyc_Absyn_Datatypedecl*dd1=_tmp37B;struct Cyc_Absyn_Datatypefield*df=_tmp37C;struct Cyc_List_List*ts1=_tmp37D;struct Cyc_Absyn_Datatypedecl*dd2=_tmp37E;struct Cyc_List_List*ts2=_tmp37F;
# 2142
if(dd1 != dd2 && Cyc_Absyn_qvar_cmp(dd1->name,dd2->name)!= 0)return 0;
# 2144
if(({int _tmp767=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts1);_tmp767 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(ts2);}))return 0;
for(0;ts1 != 0;(ts1=ts1->tl,ts2=ts2->tl)){
if(!Cyc_Unify_unify((void*)ts1->hd,(void*)((struct Cyc_List_List*)_check_null(ts2))->hd))return 0;}
return 1;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp378.f2)->tag == 5U){_LL5: _tmp379=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp378.f1)->f1;_tmp37A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp378.f2)->f1;_LL6: {struct Cyc_Absyn_FnInfo f1=_tmp379;struct Cyc_Absyn_FnInfo f2=_tmp37A;
# 2151
if(f1.tvars != 0 || f2.tvars != 0){
struct Cyc_List_List*_tmp391=f1.tvars;struct Cyc_List_List*tvs1=_tmp391;
struct Cyc_List_List*_tmp392=f2.tvars;struct Cyc_List_List*tvs2=_tmp392;
if(({int _tmp768=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs1);_tmp768 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(tvs2);}))return 0;{
struct Cyc_List_List*inst=0;
while(tvs1 != 0){
if(!Cyc_Unify_unify_kindbound(((struct Cyc_Absyn_Tvar*)tvs1->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(tvs2))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp394=_cycalloc(sizeof(*_tmp394));({struct _tuple19*_tmp76A=({struct _tuple19*_tmp393=_cycalloc(sizeof(*_tmp393));_tmp393->f1=(struct Cyc_Absyn_Tvar*)tvs2->hd,({void*_tmp769=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)tvs1->hd);_tmp393->f2=_tmp769;});_tmp393;});_tmp394->hd=_tmp76A;}),_tmp394->tl=inst;_tmp394;});
tvs1=tvs1->tl;
tvs2=tvs2->tl;}
# 2162
if(inst != 0){
f1.tvars=0;
f2.tvars=0;
return({struct Cyc_RgnOrder_RgnPO*_tmp76D=po;struct Cyc_List_List*_tmp76C=assume;void*_tmp76B=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp395=_cycalloc(sizeof(*_tmp395));_tmp395->tag=5U,_tmp395->f1=f1;_tmp395;});Cyc_Tcutil_subtype(_tmp76D,_tmp76C,_tmp76B,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp396=_cycalloc(sizeof(*_tmp396));_tmp396->tag=5U,_tmp396->f1=f2;_tmp396;}));});}}}
# 2169
if(!Cyc_Tcutil_subtype(po,assume,f1.ret_type,f2.ret_type))return 0;{
struct Cyc_List_List*_tmp397=f1.args;struct Cyc_List_List*args1=_tmp397;
struct Cyc_List_List*_tmp398=f2.args;struct Cyc_List_List*args2=_tmp398;
# 2174
if(({int _tmp76E=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args1);_tmp76E != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(args2);}))return 0;
# 2176
for(0;args1 != 0;(args1=args1->tl,args2=args2->tl)){
struct _tuple9 _tmp399=*((struct _tuple9*)args1->hd);struct _tuple9 _stmttmp5C=_tmp399;struct _tuple9 _tmp39A=_stmttmp5C;void*_tmp39C;struct Cyc_Absyn_Tqual _tmp39B;_LLA: _tmp39B=_tmp39A.f2;_tmp39C=_tmp39A.f3;_LLB: {struct Cyc_Absyn_Tqual tq1=_tmp39B;void*t1=_tmp39C;
struct _tuple9 _tmp39D=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(args2))->hd);struct _tuple9 _stmttmp5D=_tmp39D;struct _tuple9 _tmp39E=_stmttmp5D;void*_tmp3A0;struct Cyc_Absyn_Tqual _tmp39F;_LLD: _tmp39F=_tmp39E.f2;_tmp3A0=_tmp39E.f3;_LLE: {struct Cyc_Absyn_Tqual tq2=_tmp39F;void*t2=_tmp3A0;
# 2180
if(tq2.real_const && !tq1.real_const || !Cyc_Tcutil_subtype(po,assume,t2,t1))
return 0;}}}
# 2184
if(f1.c_varargs != f2.c_varargs)return 0;
if(f1.cyc_varargs != 0 && f2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp3A1=*f1.cyc_varargs;struct Cyc_Absyn_VarargInfo v1=_tmp3A1;
struct Cyc_Absyn_VarargInfo _tmp3A2=*f2.cyc_varargs;struct Cyc_Absyn_VarargInfo v2=_tmp3A2;
# 2189
if((v2.tq).real_const && !(v1.tq).real_const || !
Cyc_Tcutil_subtype(po,assume,v2.type,v1.type))
return 0;}else{
if(f1.cyc_varargs != 0 || f2.cyc_varargs != 0)return 0;}
# 2194
if(!({void*_tmp76F=(void*)_check_null(f1.effect);Cyc_Tcutil_subset_effect(1,_tmp76F,(void*)_check_null(f2.effect));}))return 0;
# 2196
if(!Cyc_Tcutil_sub_rgnpo(f1.rgn_po,f2.rgn_po))return 0;
# 2198
if(!Cyc_Tcutil_sub_attributes(f1.attributes,f2.attributes))return 0;
# 2200
if(!Cyc_Relations_check_logical_implication(f2.requires_relns,f1.requires_relns))
# 2202
return 0;
# 2204
if(!Cyc_Relations_check_logical_implication(f1.ensures_relns,f2.ensures_relns))
# 2206
return 0;
# 2208
return 1;}}}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;}}
# 2219
static int Cyc_Tcutil_ptrsubtype(struct Cyc_RgnOrder_RgnPO*po,struct Cyc_List_List*assume,void*t1,void*t2){
# 2223
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple12*_tmp3A3=(struct _tuple12*)tqs1->hd;struct _tuple12*_stmttmp5E=_tmp3A3;struct _tuple12*_tmp3A4=_stmttmp5E;void*_tmp3A6;struct Cyc_Absyn_Tqual _tmp3A5;_LL1: _tmp3A5=_tmp3A4->f1;_tmp3A6=_tmp3A4->f2;_LL2: {struct Cyc_Absyn_Tqual tq1=_tmp3A5;void*t1a=_tmp3A6;
struct _tuple12*_tmp3A7=(struct _tuple12*)tqs2->hd;struct _tuple12*_stmttmp5F=_tmp3A7;struct _tuple12*_tmp3A8=_stmttmp5F;void*_tmp3AA;struct Cyc_Absyn_Tqual _tmp3A9;_LL4: _tmp3A9=_tmp3A8->f1;_tmp3AA=_tmp3A8->f2;_LL5: {struct Cyc_Absyn_Tqual tq2=_tmp3A9;void*t2a=_tmp3AA;
if(tq1.real_const && !tq2.real_const)return 0;
if((tq2.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(t2a)))&&
 Cyc_Tcutil_subtype(po,assume,t1a,t2a))
continue;
if(Cyc_Unify_unify(t1a,t2a))
continue;
if(Cyc_Tcutil_isomorphic(t1a,t2a))
continue;
return 0;}}}}
# 2239
return 1;}
# 2244
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_RgnOrder_RgnPO*po,unsigned loc,void*t1,void*t2){
if(Cyc_Unify_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
{void*_tmp3AB=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AB)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AB)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AB)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2252
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2254
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2258
void*_tmp3AC=t1;void*_tmp3AD;struct Cyc_Absyn_Enumdecl*_tmp3AE;void*_tmp3B2;struct Cyc_Absyn_Exp*_tmp3B1;struct Cyc_Absyn_Tqual _tmp3B0;void*_tmp3AF;void*_tmp3B8;void*_tmp3B7;void*_tmp3B6;void*_tmp3B5;struct Cyc_Absyn_Tqual _tmp3B4;void*_tmp3B3;switch(*((int*)_tmp3AC)){case 3U: _LLA: _tmp3B3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).elt_type;_tmp3B4=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).elt_tq;_tmp3B5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).ptr_atts).rgn;_tmp3B6=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).ptr_atts).nullable;_tmp3B7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).ptr_atts).bounds;_tmp3B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3AC)->f1).ptr_atts).zero_term;_LLB: {void*t_a=_tmp3B3;struct Cyc_Absyn_Tqual q_a=_tmp3B4;void*rt_a=_tmp3B5;void*null_a=_tmp3B6;void*b_a=_tmp3B7;void*zt_a=_tmp3B8;
# 2267
{void*_tmp3B9=t2;void*_tmp3BF;void*_tmp3BE;void*_tmp3BD;void*_tmp3BC;struct Cyc_Absyn_Tqual _tmp3BB;void*_tmp3BA;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->tag == 3U){_LL19: _tmp3BA=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->f1).elt_type;_tmp3BB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->f1).elt_tq;_tmp3BC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->f1).ptr_atts).rgn;_tmp3BD=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->f1).ptr_atts).nullable;_tmp3BE=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->f1).ptr_atts).bounds;_tmp3BF=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3B9)->f1).ptr_atts).zero_term;_LL1A: {void*t_b=_tmp3BA;struct Cyc_Absyn_Tqual q_b=_tmp3BB;void*rt_b=_tmp3BC;void*null_b=_tmp3BD;void*b_b=_tmp3BE;void*zt_b=_tmp3BF;
# 2269
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp3C0=({struct Cyc_List_List*_tmp3C8=_cycalloc(sizeof(*_tmp3C8));({struct _tuple15*_tmp770=({struct _tuple15*_tmp3C7=_cycalloc(sizeof(*_tmp3C7));_tmp3C7->f1=t1,_tmp3C7->f2=t2;_tmp3C7;});_tmp3C8->hd=_tmp770;}),_tmp3C8->tl=0;_tmp3C8;});struct Cyc_List_List*assump=_tmp3C0;
int _tmp3C1=q_b.real_const || !q_a.real_const;int quals_okay=_tmp3C1;
# 2282 "tcutil.cyc"
int _tmp3C2=
({void*_tmp771=b_b;Cyc_Unify_unify(_tmp771,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,zt_b);
# 2282
int deep_castable=_tmp3C2;
# 2285
int _tmp3C3=quals_okay &&(
((deep_castable && Cyc_Tcutil_ptrsubtype(po,assump,t_a,t_b)||
 Cyc_Unify_unify(t_a,t_b))|| Cyc_Tcutil_isomorphic(t_a,t_b))|| Cyc_Unify_unify(t_b,Cyc_Absyn_void_type));
# 2285
int ptrsub=_tmp3C3;
# 2288
int zeroterm_ok=Cyc_Unify_unify(zt_a,zt_b)|| !Cyc_Absyn_type2bool(0,zt_b);
# 2290
int _tmp3C4=ptrsub?0:((Cyc_Tcutil_is_bits_only_type(t_a)&& Cyc_Tcutil_is_char_type(t_b))&& !
Cyc_Tcutil_force_type2bool(0,zt_b))&&(
q_b.real_const || !q_a.real_const);
# 2290
int bitcase=_tmp3C4;
# 2293
int bounds_ok=Cyc_Unify_unify(b_a,b_b);
if(!bounds_ok && !bitcase){
struct Cyc_Absyn_Exp*_tmp3C5=({void*_tmp772=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp772,b_a);});struct Cyc_Absyn_Exp*e_a=_tmp3C5;
struct Cyc_Absyn_Exp*_tmp3C6=({void*_tmp773=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp773,b_b);});struct Cyc_Absyn_Exp*e_b=_tmp3C6;
if((e_a != 0 && e_b != 0)&& Cyc_Evexp_lte_const_exp(e_b,e_a))
bounds_ok=1;else{
if(e_a == 0 || e_b == 0)
bounds_ok=1;}}{
# 2302
int t1_nullable=Cyc_Tcutil_force_type2bool(0,null_a);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,null_b);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2307
if(((bounds_ok && zeroterm_ok)&&(ptrsub || bitcase))&&(
Cyc_Unify_unify(rt_a,rt_b)|| Cyc_RgnOrder_rgn_outlives_rgn(po,rt_a,rt_b)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}}}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2313
return Cyc_Absyn_Unknown_coercion;}case 4U: _LLC: _tmp3AF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AC)->f1).elt_type;_tmp3B0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AC)->f1).tq;_tmp3B1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AC)->f1).num_elts;_tmp3B2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3AC)->f1).zero_term;_LLD: {void*t1a=_tmp3AF;struct Cyc_Absyn_Tqual tq1a=_tmp3B0;struct Cyc_Absyn_Exp*e1=_tmp3B1;void*zt1=_tmp3B2;
# 2315
{void*_tmp3C9=t2;void*_tmp3CD;struct Cyc_Absyn_Exp*_tmp3CC;struct Cyc_Absyn_Tqual _tmp3CB;void*_tmp3CA;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C9)->tag == 4U){_LL1E: _tmp3CA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C9)->f1).elt_type;_tmp3CB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C9)->f1).tq;_tmp3CC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C9)->f1).num_elts;_tmp3CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3C9)->f1).zero_term;_LL1F: {void*t2a=_tmp3CA;struct Cyc_Absyn_Tqual tq2a=_tmp3CB;struct Cyc_Absyn_Exp*e2=_tmp3CC;void*zt2=_tmp3CD;
# 2317
int okay=
(((e1 != 0 && e2 != 0)&& Cyc_Unify_unify(zt1,zt2))&&
 Cyc_Evexp_lte_const_exp(e2,e1))&& Cyc_Evexp_lte_const_exp(e1,e2);
return
# 2322
(okay && Cyc_Unify_unify(t1a,t2a))&&(!tq1a.real_const || tq2a.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2324
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2326
return Cyc_Absyn_Unknown_coercion;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f1)){case 15U: _LLE: _tmp3AE=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f1)->f2;_LLF: {struct Cyc_Absyn_Enumdecl*ed1=_tmp3AE;
# 2330
{void*_tmp3CE=t2;struct Cyc_Absyn_Enumdecl*_tmp3CF;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f1)->tag == 15U){_LL23: _tmp3CF=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3CE)->f1)->f2;_LL24: {struct Cyc_Absyn_Enumdecl*ed2=_tmp3CF;
# 2332
if((((struct Cyc_Absyn_Enumdecl*)_check_null(ed1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(ed2))->fields != 0)&&({
int _tmp774=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed1->fields))->v);_tmp774 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(ed2->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2338
goto _LL11;}case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2341
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f2 != 0){_LL14: _tmp3AD=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AC)->f2)->hd;_LL15: {void*r1=_tmp3AD;
# 2344
{void*_tmp3D0=t2;void*_tmp3D1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D0)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D0)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D0)->f2 != 0){_LL28: _tmp3D1=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D0)->f2)->hd;_LL29: {void*r2=_tmp3D1;
# 2346
if(Cyc_RgnOrder_rgn_outlives_rgn(po,r1,r2))return Cyc_Absyn_No_coercion;
goto _LL27;}}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2350
return Cyc_Absyn_Unknown_coercion;}}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;}}
# 2356
void Cyc_Tcutil_unchecked_cast(struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(Cyc_Unify_unify((void*)_check_null(e->topt),t))
return;{
struct Cyc_Absyn_Exp*_tmp3D2=Cyc_Absyn_copy_exp(e);struct Cyc_Absyn_Exp*inner=_tmp3D2;
({void*_tmp775=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));_tmp3D3->tag=14U,_tmp3D3->f1=t,_tmp3D3->f2=inner,_tmp3D3->f3=0,_tmp3D3->f4=c;_tmp3D3;});e->r=_tmp775;});
e->topt=t;}}
# 2365
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id (void){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2370
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp3D9=({struct Cyc_Int_pa_PrintArg_struct _tmp658;_tmp658.tag=1U,_tmp658.f1=(unsigned long)i;_tmp658;});void*_tmp3D7[1U];_tmp3D7[0]=& _tmp3D9;({struct _fat_ptr _tmp776=({const char*_tmp3D8="#%d";_tag_fat(_tmp3D8,sizeof(char),4U);});Cyc_aprintf(_tmp776,_tag_fat(_tmp3D7,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp3D6=_cycalloc(sizeof(*_tmp3D6));({struct _fat_ptr*_tmp777=({unsigned _tmp3D5=1;struct _fat_ptr*_tmp3D4=_cycalloc(_check_times(_tmp3D5,sizeof(struct _fat_ptr)));_tmp3D4[0]=s;_tmp3D4;});_tmp3D6->name=_tmp777;}),_tmp3D6->identity=- 1,_tmp3D6->kind=k;_tmp3D6;});}
# 2377
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp3DA=*t->name;struct _fat_ptr s=_tmp3DA;
return(int)*((const char*)_check_fat_subscript(s,sizeof(char),0))== (int)'#';}
# 2383
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 2390
struct Cyc_List_List*_tmp3DB=0;struct Cyc_List_List*fn_type_atts=_tmp3DB;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
fn_type_atts=({struct Cyc_List_List*_tmp3DC=_cycalloc(sizeof(*_tmp3DC));_tmp3DC->hd=(void*)atts->hd,_tmp3DC->tl=fn_type_atts;_tmp3DC;});}}{
struct Cyc_Absyn_FnInfo _tmp3DD=fd->i;struct Cyc_Absyn_FnInfo type_info=_tmp3DD;
type_info.attributes=fn_type_atts;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp3DE=_cycalloc(sizeof(*_tmp3DE));_tmp3DE->tag=5U,_tmp3DE->f1=type_info;_tmp3DE;});}}
# 2398
return(void*)_check_null(fd->cached_type);}
# 2404
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 2406
union Cyc_Relations_RelnOp _tmp3DF=*rop;union Cyc_Relations_RelnOp _stmttmp60=_tmp3DF;union Cyc_Relations_RelnOp _tmp3E0=_stmttmp60;struct Cyc_Absyn_Vardecl*_tmp3E1;struct Cyc_Absyn_Vardecl*_tmp3E2;switch((_tmp3E0.RNumelts).tag){case 2U: _LL1: _tmp3E2=(_tmp3E0.RVar).val;_LL2: {struct Cyc_Absyn_Vardecl*vd=_tmp3E2;
# 2408
struct _tuple1 _tmp3E3=*vd->name;struct _tuple1 _stmttmp61=_tmp3E3;struct _tuple1 _tmp3E4=_stmttmp61;struct _fat_ptr*_tmp3E6;union Cyc_Absyn_Nmspace _tmp3E5;_LL8: _tmp3E5=_tmp3E4.f1;_tmp3E6=_tmp3E4.f2;_LL9: {union Cyc_Absyn_Nmspace nmspace=_tmp3E5;struct _fat_ptr*var=_tmp3E6;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp778=(struct _fat_ptr)*var;Cyc_strcmp(_tmp778,({const char*_tmp3E7="return_value";_tag_fat(_tmp3E7,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp779=Cyc_Relations_RReturn();*rop=_tmp779;});
goto _LL0;}{
# 2414
unsigned c=0U;
{struct Cyc_List_List*_tmp3E8=args;struct Cyc_List_List*a=_tmp3E8;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3E9=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp62=_tmp3E9;struct _tuple9*_tmp3EA=_stmttmp62;struct _fat_ptr*_tmp3EB;_LLB: _tmp3EB=_tmp3EA->f1;_LLC: {struct _fat_ptr*vopt=_tmp3EB;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp77A=Cyc_Relations_RParam(c);*rop=_tmp77A;});
break;}}}}
# 2422
goto _LL0;}}}case 3U: _LL3: _tmp3E1=(_tmp3E0.RNumelts).val;_LL4: {struct Cyc_Absyn_Vardecl*vd=_tmp3E1;
# 2424
struct _tuple1 _tmp3EC=*vd->name;struct _tuple1 _stmttmp63=_tmp3EC;struct _tuple1 _tmp3ED=_stmttmp63;struct _fat_ptr*_tmp3EF;union Cyc_Absyn_Nmspace _tmp3EE;_LLE: _tmp3EE=_tmp3ED.f1;_tmp3EF=_tmp3ED.f2;_LLF: {union Cyc_Absyn_Nmspace nmspace=_tmp3EE;struct _fat_ptr*var=_tmp3EF;
if(!((int)((nmspace.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp3F0=args;struct Cyc_List_List*a=_tmp3F0;for(0;a != 0;(a=a->tl,c ++)){
struct _tuple9*_tmp3F1=(struct _tuple9*)a->hd;struct _tuple9*_stmttmp64=_tmp3F1;struct _tuple9*_tmp3F2=_stmttmp64;struct _fat_ptr*_tmp3F3;_LL11: _tmp3F3=_tmp3F2->f1;_LL12: {struct _fat_ptr*vopt=_tmp3F3;
if(vopt != 0 && Cyc_strcmp((struct _fat_ptr)*var,(struct _fat_ptr)*vopt)== 0){
({union Cyc_Relations_RelnOp _tmp77B=Cyc_Relations_RParamNumelts(c);*rop=_tmp77B;});
break;}}}}
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
struct Cyc_List_List*_tmp3F4=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);struct Cyc_List_List*relns=_tmp3F4;
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,relns);
return relns;}}
# 2454
void*Cyc_Tcutil_snd_tqt(struct _tuple12*t){return(*t).f2;}
static struct _tuple12*Cyc_Tcutil_map2_tq(struct _tuple12*pr,void*t){
struct _tuple12*_tmp3F5=pr;void*_tmp3F7;struct Cyc_Absyn_Tqual _tmp3F6;_LL1: _tmp3F6=_tmp3F5->f1;_tmp3F7=_tmp3F5->f2;_LL2: {struct Cyc_Absyn_Tqual tq=_tmp3F6;void*t2=_tmp3F7;
if(t2 == t)return pr;
return({struct _tuple12*_tmp3F8=_cycalloc(sizeof(*_tmp3F8));_tmp3F8->f1=tq,_tmp3F8->f2=t;_tmp3F8;});}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 2460
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 2462
return({struct _tuple27*_tmp3FA=_region_malloc(rgn,sizeof(*_tmp3FA));({struct _tuple26*_tmp77C=({struct _tuple26*_tmp3F9=_region_malloc(rgn,sizeof(*_tmp3F9));_tmp3F9->f1=(*y).f1,_tmp3F9->f2=(*y).f2;_tmp3F9;});_tmp3FA->f1=_tmp77C;}),_tmp3FA->f2=(*y).f3;_tmp3FA;});}
# 2464
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 2466
struct _tuple9 _tmp3FB=*orig_arg;struct _tuple9 _stmttmp65=_tmp3FB;struct _tuple9 _tmp3FC=_stmttmp65;void*_tmp3FF;struct Cyc_Absyn_Tqual _tmp3FE;struct _fat_ptr*_tmp3FD;_LL1: _tmp3FD=_tmp3FC.f1;_tmp3FE=_tmp3FC.f2;_tmp3FF=_tmp3FC.f3;_LL2: {struct _fat_ptr*vopt_orig=_tmp3FD;struct Cyc_Absyn_Tqual tq_orig=_tmp3FE;void*t_orig=_tmp3FF;
if(t == t_orig)return orig_arg;
return({struct _tuple9*_tmp400=_cycalloc(sizeof(*_tmp400));_tmp400->f1=vopt_orig,_tmp400->f2=tq_orig,_tmp400->f3=t;_tmp400;});}}
# 2470
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 2474
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 2476
return({struct Cyc_Absyn_Exp*_tmp401=_cycalloc(sizeof(*_tmp401));_tmp401->topt=old->topt,_tmp401->r=r,_tmp401->loc=old->loc,_tmp401->annot=old->annot;_tmp401;});}
# 2481
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp402=e->r;void*_stmttmp66=_tmp402;void*_tmp403=_stmttmp66;void*_tmp404;struct Cyc_List_List*_tmp406;void*_tmp405;struct Cyc_Absyn_Exp*_tmp407;struct Cyc_Absyn_Exp*_tmp408;void*_tmp409;enum Cyc_Absyn_Coercion _tmp40D;int _tmp40C;struct Cyc_Absyn_Exp*_tmp40B;void*_tmp40A;struct Cyc_Absyn_Exp*_tmp40F;struct Cyc_Absyn_Exp*_tmp40E;struct Cyc_Absyn_Exp*_tmp411;struct Cyc_Absyn_Exp*_tmp410;struct Cyc_Absyn_Exp*_tmp413;struct Cyc_Absyn_Exp*_tmp412;struct Cyc_Absyn_Exp*_tmp416;struct Cyc_Absyn_Exp*_tmp415;struct Cyc_Absyn_Exp*_tmp414;struct Cyc_List_List*_tmp418;enum Cyc_Absyn_Primop _tmp417;switch(*((int*)_tmp403)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp417=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp418=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_LLC: {enum Cyc_Absyn_Primop p=_tmp417;struct Cyc_List_List*es=_tmp418;
# 2490
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 1){
struct Cyc_Absyn_Exp*_tmp419=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp419;
struct Cyc_Absyn_Exp*_tmp41A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41A;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp77E=e;Cyc_Tcutil_copye(_tmp77E,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp41C=_cycalloc(sizeof(*_tmp41C));_tmp41C->tag=3U,_tmp41C->f1=p,({struct Cyc_List_List*_tmp77D=({struct Cyc_Absyn_Exp*_tmp41B[1U];_tmp41B[0]=new_e1;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp41B,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp41C->f2=_tmp77D;});_tmp41C;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(es)== 2){
struct Cyc_Absyn_Exp*_tmp41D=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es))->hd;struct Cyc_Absyn_Exp*e1=_tmp41D;
struct Cyc_Absyn_Exp*_tmp41E=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(es->tl))->hd;struct Cyc_Absyn_Exp*e2=_tmp41E;
struct Cyc_Absyn_Exp*_tmp41F=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp41F;
struct Cyc_Absyn_Exp*_tmp420=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp420;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp780=e;Cyc_Tcutil_copye(_tmp780,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp422=_cycalloc(sizeof(*_tmp422));_tmp422->tag=3U,_tmp422->f1=p,({struct Cyc_List_List*_tmp77F=({struct Cyc_Absyn_Exp*_tmp421[2U];_tmp421[0]=new_e1,_tmp421[1]=new_e2;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp421,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp422->f2=_tmp77F;});_tmp422;}));});}}
# 2502
return({struct Cyc_Warn_String_Warn_Warg_struct _tmp424=({struct Cyc_Warn_String_Warn_Warg_struct _tmp659;_tmp659.tag=0U,({struct _fat_ptr _tmp781=({const char*_tmp425="primop does not have 1 or 2 args!";_tag_fat(_tmp425,sizeof(char),34U);});_tmp659.f1=_tmp781;});_tmp659;});void*_tmp423[1U];_tmp423[0]=& _tmp424;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp423,sizeof(void*),1U));});}case 6U: _LLD: _tmp414=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp415=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_tmp416=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp403)->f3;_LLE: {struct Cyc_Absyn_Exp*e1=_tmp414;struct Cyc_Absyn_Exp*e2=_tmp415;struct Cyc_Absyn_Exp*e3=_tmp416;
# 2504
struct Cyc_Absyn_Exp*_tmp426=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp426;
struct Cyc_Absyn_Exp*_tmp427=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp427;
struct Cyc_Absyn_Exp*_tmp428=Cyc_Tcutil_rsubsexp(r,inst,e3);struct Cyc_Absyn_Exp*new_e3=_tmp428;
if((new_e1 == e1 && new_e2 == e2)&& new_e3 == e3)return e;
return({struct Cyc_Absyn_Exp*_tmp782=e;Cyc_Tcutil_copye(_tmp782,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp429=_cycalloc(sizeof(*_tmp429));_tmp429->tag=6U,_tmp429->f1=new_e1,_tmp429->f2=new_e2,_tmp429->f3=new_e3;_tmp429;}));});}case 7U: _LLF: _tmp412=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp413=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_LL10: {struct Cyc_Absyn_Exp*e1=_tmp412;struct Cyc_Absyn_Exp*e2=_tmp413;
# 2510
struct Cyc_Absyn_Exp*_tmp42A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42A;
struct Cyc_Absyn_Exp*_tmp42B=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp42B;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp783=e;Cyc_Tcutil_copye(_tmp783,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp42C=_cycalloc(sizeof(*_tmp42C));_tmp42C->tag=7U,_tmp42C->f1=new_e1,_tmp42C->f2=new_e2;_tmp42C;}));});}case 8U: _LL11: _tmp410=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp411=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp410;struct Cyc_Absyn_Exp*e2=_tmp411;
# 2515
struct Cyc_Absyn_Exp*_tmp42D=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp42D;
struct Cyc_Absyn_Exp*_tmp42E=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp42E;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp784=e;Cyc_Tcutil_copye(_tmp784,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp42F=_cycalloc(sizeof(*_tmp42F));_tmp42F->tag=8U,_tmp42F->f1=new_e1,_tmp42F->f2=new_e2;_tmp42F;}));});}case 9U: _LL13: _tmp40E=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp40F=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp40E;struct Cyc_Absyn_Exp*e2=_tmp40F;
# 2520
struct Cyc_Absyn_Exp*_tmp430=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp430;
struct Cyc_Absyn_Exp*_tmp431=Cyc_Tcutil_rsubsexp(r,inst,e2);struct Cyc_Absyn_Exp*new_e2=_tmp431;
if(new_e1 == e1 && new_e2 == e2)return e;
return({struct Cyc_Absyn_Exp*_tmp785=e;Cyc_Tcutil_copye(_tmp785,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp432=_cycalloc(sizeof(*_tmp432));_tmp432->tag=9U,_tmp432->f1=new_e1,_tmp432->f2=new_e2;_tmp432;}));});}case 14U: _LL15: _tmp40A=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp40B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_tmp40C=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp403)->f3;_tmp40D=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp403)->f4;_LL16: {void*t=_tmp40A;struct Cyc_Absyn_Exp*e1=_tmp40B;int b=_tmp40C;enum Cyc_Absyn_Coercion c=_tmp40D;
# 2525
struct Cyc_Absyn_Exp*_tmp433=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp433;
void*_tmp434=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp434;
if(new_e1 == e1 && new_typ == t)return e;
return({struct Cyc_Absyn_Exp*_tmp786=e;Cyc_Tcutil_copye(_tmp786,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp435=_cycalloc(sizeof(*_tmp435));_tmp435->tag=14U,_tmp435->f1=new_typ,_tmp435->f2=new_e1,_tmp435->f3=b,_tmp435->f4=c;_tmp435;}));});}case 17U: _LL17: _tmp409=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_LL18: {void*t=_tmp409;
# 2530
void*_tmp436=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp436;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp787=e;Cyc_Tcutil_copye(_tmp787,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp437=_cycalloc(sizeof(*_tmp437));_tmp437->tag=17U,_tmp437->f1=new_typ;_tmp437;}));});}case 18U: _LL19: _tmp408=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_LL1A: {struct Cyc_Absyn_Exp*e1=_tmp408;
# 2533
struct Cyc_Absyn_Exp*_tmp438=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp438;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp788=e;Cyc_Tcutil_copye(_tmp788,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp439=_cycalloc(sizeof(*_tmp439));_tmp439->tag=18U,_tmp439->f1=new_e1;_tmp439;}));});}case 41U: _LL1B: _tmp407=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_LL1C: {struct Cyc_Absyn_Exp*e1=_tmp407;
# 2536
struct Cyc_Absyn_Exp*_tmp43A=Cyc_Tcutil_rsubsexp(r,inst,e1);struct Cyc_Absyn_Exp*new_e1=_tmp43A;
return new_e1 == e1?e:({struct Cyc_Absyn_Exp*_tmp789=e;Cyc_Tcutil_copye(_tmp789,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp43B=_cycalloc(sizeof(*_tmp43B));_tmp43B->tag=41U,_tmp43B->f1=new_e1;_tmp43B;}));});}case 19U: _LL1D: _tmp405=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_tmp406=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp403)->f2;_LL1E: {void*t=_tmp405;struct Cyc_List_List*f=_tmp406;
# 2539
void*_tmp43C=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp43C;
return new_typ == t?e:({struct Cyc_Absyn_Exp*_tmp78A=e;Cyc_Tcutil_copye(_tmp78A,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp43D=_cycalloc(sizeof(*_tmp43D));_tmp43D->tag=19U,_tmp43D->f1=new_typ,_tmp43D->f2=f;_tmp43D;}));});}case 39U: _LL1F: _tmp404=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp403)->f1;_LL20: {void*t=_tmp404;
# 2542
void*_tmp43E=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp43E;
if(new_typ == t)return e;{
# 2545
void*_tmp43F=Cyc_Tcutil_compress(new_typ);void*_stmttmp67=_tmp43F;void*_tmp440=_stmttmp67;struct Cyc_Absyn_Exp*_tmp441;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp440)->tag == 9U){_LL24: _tmp441=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp440)->f1;_LL25: {struct Cyc_Absyn_Exp*e=_tmp441;
return e;}}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp78B=e;Cyc_Tcutil_copye(_tmp78B,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp442=_cycalloc(sizeof(*_tmp442));_tmp442->tag=39U,_tmp442->f1=new_typ;_tmp442;}));});}_LL23:;}}default: _LL21: _LL22:
# 2549
 return({struct Cyc_Warn_String_Warn_Warg_struct _tmp444=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65A;_tmp65A.tag=0U,({struct _fat_ptr _tmp78C=({const char*_tmp445="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp445,sizeof(char),46U);});_tmp65A.f1=_tmp78C;});_tmp65A;});void*_tmp443[1U];_tmp443[0]=& _tmp444;((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp443,sizeof(void*),1U));});}_LL0:;}
# 2553
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 2556
return e == 0?0: Cyc_Tcutil_rsubsexp(r,inst,e);}
# 2559
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 2562
void*_tmp446=f->type;void*t=_tmp446;
struct Cyc_Absyn_Exp*_tmp447=f->requires_clause;struct Cyc_Absyn_Exp*req=_tmp447;
void*_tmp448=Cyc_Tcutil_rsubstitute(r,inst,t);void*new_typ=_tmp448;
struct Cyc_Absyn_Exp*_tmp449=Cyc_Tcutil_rsubs_exp_opt(r,inst,req);struct Cyc_Absyn_Exp*new_req=_tmp449;
if(t == new_typ && req == new_req)
return f;{
struct Cyc_Absyn_Aggrfield*_tmp44A=({struct Cyc_Absyn_Aggrfield*_tmp44B=_cycalloc(sizeof(*_tmp44B));*_tmp44B=*f;_tmp44B;});struct Cyc_Absyn_Aggrfield*ans=_tmp44A;
ans->type=new_typ;
ans->requires_clause=new_req;
return ans;}}
# 2574
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 2577
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp44C=(struct Cyc_Absyn_Aggrfield*)fs->hd;struct Cyc_Absyn_Aggrfield*f=_tmp44C;
struct Cyc_List_List*_tmp44D=fs->tl;struct Cyc_List_List*t=_tmp44D;
struct Cyc_Absyn_Aggrfield*_tmp44E=Cyc_Tcutil_subst_aggrfield(r,inst,f);struct Cyc_Absyn_Aggrfield*new_f=_tmp44E;
struct Cyc_List_List*_tmp44F=Cyc_Tcutil_subst_aggrfields(r,inst,t);struct Cyc_List_List*new_typ=_tmp44F;
if(new_f == f && new_typ == t)
return fs;
return({struct Cyc_List_List*_tmp450=_cycalloc(sizeof(*_tmp450));_tmp450->hd=new_f,_tmp450->tl=new_typ;_tmp450;});}}
# 2587
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 2590
struct _tuple0 _tmp451=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _stmttmp68=_tmp451;struct _tuple0 _tmp452=_stmttmp68;struct Cyc_List_List*_tmp454;struct Cyc_List_List*_tmp453;_LL1: _tmp453=_tmp452.f1;_tmp454=_tmp452.f2;_LL2: {struct Cyc_List_List*rpo1a=_tmp453;struct Cyc_List_List*rpo1b=_tmp454;
struct Cyc_List_List*_tmp455=Cyc_Tcutil_substs(rgn,inst,rpo1a);struct Cyc_List_List*rpo2a=_tmp455;
struct Cyc_List_List*_tmp456=Cyc_Tcutil_substs(rgn,inst,rpo1b);struct Cyc_List_List*rpo2b=_tmp456;
if(rpo2a == rpo1a && rpo2b == rpo1b)
return rgn_po;
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(rpo2a,rpo2b);}}
# 2598
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 2601
void*_tmp457=Cyc_Tcutil_compress(t);void*_stmttmp69=_tmp457;void*_tmp458=_stmttmp69;struct Cyc_Absyn_Exp*_tmp459;struct Cyc_Absyn_Exp*_tmp45A;struct Cyc_List_List*_tmp45C;void*_tmp45B;void*_tmp45D;struct Cyc_List_List*_tmp45F;enum Cyc_Absyn_AggrKind _tmp45E;struct Cyc_List_List*_tmp460;struct Cyc_Absyn_Vardecl*_tmp46C;struct Cyc_Absyn_Exp*_tmp46B;struct Cyc_Absyn_Exp*_tmp46A;struct Cyc_List_List*_tmp469;struct Cyc_List_List*_tmp468;struct Cyc_Absyn_VarargInfo*_tmp467;int _tmp466;struct Cyc_List_List*_tmp465;void*_tmp464;struct Cyc_Absyn_Tqual _tmp463;void*_tmp462;struct Cyc_List_List*_tmp461;void*_tmp472;void*_tmp471;void*_tmp470;void*_tmp46F;struct Cyc_Absyn_Tqual _tmp46E;void*_tmp46D;unsigned _tmp477;void*_tmp476;struct Cyc_Absyn_Exp*_tmp475;struct Cyc_Absyn_Tqual _tmp474;void*_tmp473;void*_tmp47B;struct Cyc_Absyn_Typedefdecl*_tmp47A;struct Cyc_List_List*_tmp479;struct _tuple1*_tmp478;struct Cyc_Absyn_Tvar*_tmp47C;switch(*((int*)_tmp458)){case 2U: _LL1: _tmp47C=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp458)->f1;_LL2: {struct Cyc_Absyn_Tvar*v=_tmp47C;
# 2604
struct _handler_cons _tmp47D;_push_handler(& _tmp47D);{int _tmp47F=0;if(setjmp(_tmp47D.handler))_tmp47F=1;if(!_tmp47F){{void*_tmp480=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,v);_npop_handler(0U);return _tmp480;};_pop_handler();}else{void*_tmp47E=(void*)Cyc_Core_get_exn_thrown();void*_tmp481=_tmp47E;void*_tmp482;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp481)->tag == Cyc_Core_Not_found){_LL1A: _LL1B:
 return t;}else{_LL1C: _tmp482=_tmp481;_LL1D: {void*exn=_tmp482;(int)_rethrow(exn);}}_LL19:;}}}case 8U: _LL3: _tmp478=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp458)->f1;_tmp479=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp458)->f2;_tmp47A=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp458)->f3;_tmp47B=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp458)->f4;_LL4: {struct _tuple1*n=_tmp478;struct Cyc_List_List*ts=_tmp479;struct Cyc_Absyn_Typedefdecl*td=_tmp47A;void*topt=_tmp47B;
# 2607
struct Cyc_List_List*_tmp483=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp483;
return new_ts == ts?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->tag=8U,_tmp484->f1=n,_tmp484->f2=new_ts,_tmp484->f3=td,_tmp484->f4=topt;_tmp484;});}case 4U: _LL5: _tmp473=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp458)->f1).elt_type;_tmp474=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp458)->f1).tq;_tmp475=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp458)->f1).num_elts;_tmp476=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp458)->f1).zero_term;_tmp477=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp458)->f1).zt_loc;_LL6: {void*t1=_tmp473;struct Cyc_Absyn_Tqual tq=_tmp474;struct Cyc_Absyn_Exp*e=_tmp475;void*zt=_tmp476;unsigned ztl=_tmp477;
# 2610
void*_tmp485=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp485;
struct Cyc_Absyn_Exp*_tmp486=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp486;
void*_tmp487=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp487;
return(new_t1 == t1 && new_e == e)&& new_zt == zt?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp488=_cycalloc(sizeof(*_tmp488));
_tmp488->tag=4U,(_tmp488->f1).elt_type=new_t1,(_tmp488->f1).tq=tq,(_tmp488->f1).num_elts=new_e,(_tmp488->f1).zero_term=new_zt,(_tmp488->f1).zt_loc=ztl;_tmp488;});}case 3U: _LL7: _tmp46D=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp458)->f1).elt_type;_tmp46E=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp458)->f1).elt_tq;_tmp46F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp458)->f1).ptr_atts).rgn;_tmp470=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp458)->f1).ptr_atts).nullable;_tmp471=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp458)->f1).ptr_atts).bounds;_tmp472=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp458)->f1).ptr_atts).zero_term;_LL8: {void*t1=_tmp46D;struct Cyc_Absyn_Tqual tq=_tmp46E;void*r=_tmp46F;void*n=_tmp470;void*b=_tmp471;void*zt=_tmp472;
# 2616
void*_tmp489=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*new_t1=_tmp489;
void*_tmp48A=Cyc_Tcutil_rsubstitute(rgn,inst,r);void*new_r=_tmp48A;
void*_tmp48B=Cyc_Tcutil_rsubstitute(rgn,inst,b);void*new_b=_tmp48B;
void*_tmp48C=Cyc_Tcutil_rsubstitute(rgn,inst,zt);void*new_zt=_tmp48C;
if(((new_t1 == t1 && new_r == r)&& new_b == b)&& new_zt == zt)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp65B;_tmp65B.elt_type=new_t1,_tmp65B.elt_tq=tq,(_tmp65B.ptr_atts).rgn=new_r,(_tmp65B.ptr_atts).nullable=n,(_tmp65B.ptr_atts).bounds=new_b,(_tmp65B.ptr_atts).zero_term=new_zt,(_tmp65B.ptr_atts).ptrloc=0;_tmp65B;}));}case 5U: _LL9: _tmp461=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).tvars;_tmp462=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).effect;_tmp463=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).ret_tqual;_tmp464=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).ret_type;_tmp465=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).args;_tmp466=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).c_varargs;_tmp467=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).cyc_varargs;_tmp468=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).rgn_po;_tmp469=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).attributes;_tmp46A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).requires_clause;_tmp46B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).ensures_clause;_tmp46C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp458)->f1).return_value;_LLA: {struct Cyc_List_List*vs=_tmp461;void*eff=_tmp462;struct Cyc_Absyn_Tqual rtq=_tmp463;void*rtyp=_tmp464;struct Cyc_List_List*args=_tmp465;int c_varargs=_tmp466;struct Cyc_Absyn_VarargInfo*cyc_varargs=_tmp467;struct Cyc_List_List*rgn_po=_tmp468;struct Cyc_List_List*atts=_tmp469;struct Cyc_Absyn_Exp*req=_tmp46A;struct Cyc_Absyn_Exp*ens=_tmp46B;struct Cyc_Absyn_Vardecl*ret_value=_tmp46C;
# 2626
{struct Cyc_List_List*_tmp48D=vs;struct Cyc_List_List*p=_tmp48D;for(0;p != 0;p=p->tl){
inst=({struct Cyc_List_List*_tmp48F=_region_malloc(rgn,sizeof(*_tmp48F));({struct _tuple19*_tmp78E=({struct _tuple19*_tmp48E=_region_malloc(rgn,sizeof(*_tmp48E));_tmp48E->f1=(struct Cyc_Absyn_Tvar*)p->hd,({void*_tmp78D=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)p->hd);_tmp48E->f2=_tmp78D;});_tmp48E;});_tmp48F->hd=_tmp78E;}),_tmp48F->tl=inst;_tmp48F;});}}{
struct _tuple0 _tmp490=({struct _RegionHandle*_tmp790=rgn;struct _RegionHandle*_tmp78F=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp790,_tmp78F,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,args));});
# 2628
struct _tuple0 _stmttmp6A=_tmp490;struct _tuple0 _tmp491=_stmttmp6A;struct Cyc_List_List*_tmp493;struct Cyc_List_List*_tmp492;_LL1F: _tmp492=_tmp491.f1;_tmp493=_tmp491.f2;_LL20: {struct Cyc_List_List*qs=_tmp492;struct Cyc_List_List*ts=_tmp493;
# 2630
struct Cyc_List_List*_tmp494=args;struct Cyc_List_List*args2=_tmp494;
struct Cyc_List_List*_tmp495=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*ts2=_tmp495;
if(ts2 != ts)
args2=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,args,ts2);{
void*eff2=eff == 0?0: Cyc_Tcutil_rsubstitute(rgn,inst,eff);
# 2636
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(cyc_varargs == 0)
cyc_varargs2=0;else{
# 2640
struct Cyc_Absyn_VarargInfo _tmp496=*cyc_varargs;struct Cyc_Absyn_VarargInfo _stmttmp6B=_tmp496;struct Cyc_Absyn_VarargInfo _tmp497=_stmttmp6B;int _tmp49B;void*_tmp49A;struct Cyc_Absyn_Tqual _tmp499;struct _fat_ptr*_tmp498;_LL22: _tmp498=_tmp497.name;_tmp499=_tmp497.tq;_tmp49A=_tmp497.type;_tmp49B=_tmp497.inject;_LL23: {struct _fat_ptr*n=_tmp498;struct Cyc_Absyn_Tqual tq=_tmp499;void*t=_tmp49A;int i=_tmp49B;
void*_tmp49C=Cyc_Tcutil_rsubstitute(rgn,inst,t);void*t2=_tmp49C;
if(t2 == t)cyc_varargs2=cyc_varargs;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp49D=_cycalloc(sizeof(*_tmp49D));_tmp49D->name=n,_tmp49D->tq=tq,_tmp49D->type=t2,_tmp49D->inject=i;_tmp49D;});}}}{
# 2645
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,rgn_po);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,req);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,ens);
struct Cyc_List_List*_tmp49E=Cyc_Tcutil_extract_relns(args2,req2);struct Cyc_List_List*req_relns2=_tmp49E;
struct Cyc_List_List*_tmp49F=Cyc_Tcutil_extract_relns(args2,ens2);struct Cyc_List_List*ens_relns2=_tmp49F;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4A0=_cycalloc(sizeof(*_tmp4A0));_tmp4A0->tag=5U,(_tmp4A0->f1).tvars=vs,(_tmp4A0->f1).effect=eff2,(_tmp4A0->f1).ret_tqual=rtq,({void*_tmp791=Cyc_Tcutil_rsubstitute(rgn,inst,rtyp);(_tmp4A0->f1).ret_type=_tmp791;}),(_tmp4A0->f1).args=args2,(_tmp4A0->f1).c_varargs=c_varargs,(_tmp4A0->f1).cyc_varargs=cyc_varargs2,(_tmp4A0->f1).rgn_po=rgn_po2,(_tmp4A0->f1).attributes=atts,(_tmp4A0->f1).requires_clause=req2,(_tmp4A0->f1).requires_relns=req_relns2,(_tmp4A0->f1).ensures_clause=ens2,(_tmp4A0->f1).ensures_relns=ens_relns2,(_tmp4A0->f1).return_value=ret_value;_tmp4A0;});}}}}}case 6U: _LLB: _tmp460=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp458)->f1;_LLC: {struct Cyc_List_List*tqts=_tmp460;
# 2654
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp4A1=tqts;struct Cyc_List_List*ts1=_tmp4A1;for(0;ts1 != 0;ts1=ts1->tl){
void*_tmp4A2=(*((struct _tuple12*)ts1->hd)).f2;void*t1=_tmp4A2;
void*_tmp4A3=Cyc_Tcutil_rsubstitute(rgn,inst,t1);void*t2=_tmp4A3;
if(t1 != t2)
change=1;
ts2=({struct Cyc_List_List*_tmp4A4=_region_malloc(rgn,sizeof(*_tmp4A4));_tmp4A4->hd=t2,_tmp4A4->tl=ts2;_tmp4A4;});}}
# 2663
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp4A5=_cycalloc(sizeof(*_tmp4A5));_tmp4A5->tag=6U,({struct Cyc_List_List*_tmp793=({struct Cyc_List_List*_tmp792=tqts;((struct Cyc_List_List*(*)(struct _tuple12*(*f)(struct _tuple12*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp792,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp4A5->f1=_tmp793;});_tmp4A5;});}case 7U: _LLD: _tmp45E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp458)->f1;_tmp45F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp458)->f2;_LLE: {enum Cyc_Absyn_AggrKind k=_tmp45E;struct Cyc_List_List*fs=_tmp45F;
# 2667
struct Cyc_List_List*_tmp4A6=Cyc_Tcutil_subst_aggrfields(rgn,inst,fs);struct Cyc_List_List*new_fs=_tmp4A6;
return fs == new_fs?t:(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp4A7=_cycalloc(sizeof(*_tmp4A7));_tmp4A7->tag=7U,_tmp4A7->f1=k,_tmp4A7->f2=new_fs;_tmp4A7;});}case 1U: _LLF: _tmp45D=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp458)->f2;_LL10: {void*r=_tmp45D;
# 2670
return r == 0?t: Cyc_Tcutil_rsubstitute(rgn,inst,r);}case 0U: _LL11: _tmp45B=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp458)->f1;_tmp45C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp458)->f2;_LL12: {void*c=_tmp45B;struct Cyc_List_List*ts=_tmp45C;
# 2672
struct Cyc_List_List*_tmp4A8=Cyc_Tcutil_substs(rgn,inst,ts);struct Cyc_List_List*new_ts=_tmp4A8;
return ts == new_ts?t:(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp4A9=_cycalloc(sizeof(*_tmp4A9));_tmp4A9->tag=0U,_tmp4A9->f1=c,_tmp4A9->f2=new_ts;_tmp4A9;});}case 9U: _LL13: _tmp45A=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp458)->f1;_LL14: {struct Cyc_Absyn_Exp*e=_tmp45A;
# 2675
struct Cyc_Absyn_Exp*_tmp4AA=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4AA;
return new_e == e?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp4AB=_cycalloc(sizeof(*_tmp4AB));_tmp4AB->tag=9U,_tmp4AB->f1=new_e;_tmp4AB;});}case 11U: _LL15: _tmp459=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp458)->f1;_LL16: {struct Cyc_Absyn_Exp*e=_tmp459;
# 2678
struct Cyc_Absyn_Exp*_tmp4AC=Cyc_Tcutil_rsubsexp(rgn,inst,e);struct Cyc_Absyn_Exp*new_e=_tmp4AC;
return new_e == e?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp4AD=_cycalloc(sizeof(*_tmp4AD));_tmp4AD->tag=11U,_tmp4AD->f1=new_e;_tmp4AD;});}default: _LL17: _LL18:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4AF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65C;_tmp65C.tag=0U,({struct _fat_ptr _tmp794=({const char*_tmp4B0="found typedecltype in rsubs";_tag_fat(_tmp4B0,sizeof(char),28U);});_tmp65C.f1=_tmp794;});_tmp65C;});void*_tmp4AE[1U];_tmp4AE[0]=& _tmp4AF;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp4AE,sizeof(void*),1U));});}_LL0:;}
# 2684
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 2687
if(ts == 0)
return 0;{
void*_tmp4B1=(void*)ts->hd;void*old_hd=_tmp4B1;
struct Cyc_List_List*_tmp4B2=ts->tl;struct Cyc_List_List*old_tl=_tmp4B2;
void*_tmp4B3=Cyc_Tcutil_rsubstitute(rgn,inst,old_hd);void*new_hd=_tmp4B3;
struct Cyc_List_List*_tmp4B4=Cyc_Tcutil_substs(rgn,inst,old_tl);struct Cyc_List_List*new_tl=_tmp4B4;
if(old_hd == new_hd && old_tl == new_tl)
return ts;
return({struct Cyc_List_List*_tmp4B5=_cycalloc(sizeof(*_tmp4B5));_tmp4B5->hd=new_hd,_tmp4B5->tl=new_tl;_tmp4B5;});}}
# 2698
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);
return t;}
# 2705
struct _tuple19*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp4B6=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4B6;
return({struct _tuple19*_tmp4B8=_cycalloc(sizeof(*_tmp4B8));_tmp4B8->f1=tv,({void*_tmp796=({struct Cyc_Core_Opt*_tmp795=k;Cyc_Absyn_new_evar(_tmp795,({struct Cyc_Core_Opt*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));_tmp4B7->v=s;_tmp4B7;}));});_tmp4B8->f2=_tmp796;});_tmp4B8;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 2710
struct _tuple19*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 2712
struct _tuple28*_tmp4B9=env;struct _RegionHandle*_tmp4BB;struct Cyc_List_List*_tmp4BA;_LL1: _tmp4BA=_tmp4B9->f1;_tmp4BB=_tmp4B9->f2;_LL2: {struct Cyc_List_List*s=_tmp4BA;struct _RegionHandle*rgn=_tmp4BB;
struct Cyc_Core_Opt*_tmp4BC=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));struct Cyc_Core_Opt*k=_tmp4BC;
return({struct _tuple19*_tmp4BE=_region_malloc(rgn,sizeof(*_tmp4BE));_tmp4BE->f1=tv,({void*_tmp798=({struct Cyc_Core_Opt*_tmp797=k;Cyc_Absyn_new_evar(_tmp797,({struct Cyc_Core_Opt*_tmp4BD=_cycalloc(sizeof(*_tmp4BD));_tmp4BD->v=s;_tmp4BD;}));});_tmp4BE->f2=_tmp798;});_tmp4BE;});}}
# 2717
void Cyc_Tcutil_check_bitfield(unsigned loc,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 2719
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4C0=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65F;_tmp65F.tag=0U,({struct _fat_ptr _tmp799=({const char*_tmp4C4="bitfield ";_tag_fat(_tmp4C4,sizeof(char),10U);});_tmp65F.f1=_tmp799;});_tmp65F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C1=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65E;_tmp65E.tag=0U,_tmp65E.f1=*fn;_tmp65E;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4C2=({struct Cyc_Warn_String_Warn_Warg_struct _tmp65D;_tmp65D.tag=0U,({struct _fat_ptr _tmp79A=({const char*_tmp4C3=" does not have constant width";_tag_fat(_tmp4C3,sizeof(char),30U);});_tmp65D.f1=_tmp79A;});_tmp65D;});void*_tmp4BF[3U];_tmp4BF[0]=& _tmp4C0,_tmp4BF[1]=& _tmp4C1,_tmp4BF[2]=& _tmp4C2;({unsigned _tmp79B=loc;Cyc_Warn_err2(_tmp79B,_tag_fat(_tmp4BF,sizeof(void*),3U));});});else{
# 2724
struct _tuple13 _tmp4C5=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple13 _stmttmp6C=_tmp4C5;struct _tuple13 _tmp4C6=_stmttmp6C;int _tmp4C8;unsigned _tmp4C7;_LL1: _tmp4C7=_tmp4C6.f1;_tmp4C8=_tmp4C6.f2;_LL2: {unsigned i=_tmp4C7;int known=_tmp4C8;
if(!known)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CA=({struct Cyc_Warn_String_Warn_Warg_struct _tmp660;_tmp660.tag=0U,({struct _fat_ptr _tmp79C=({const char*_tmp4CB="cannot evaluate bitfield width at compile time";_tag_fat(_tmp4CB,sizeof(char),47U);});_tmp660.f1=_tmp79C;});_tmp660;});void*_tmp4C9[1U];_tmp4C9[0]=& _tmp4CA;({unsigned _tmp79D=loc;Cyc_Warn_warn2(_tmp79D,_tag_fat(_tmp4C9,sizeof(void*),1U));});});
if((int)i < 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4CD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp661;_tmp661.tag=0U,({struct _fat_ptr _tmp79E=({const char*_tmp4CE="bitfield has negative width";_tag_fat(_tmp4CE,sizeof(char),28U);});_tmp661.f1=_tmp79E;});_tmp661;});void*_tmp4CC[1U];_tmp4CC[0]=& _tmp4CD;({unsigned _tmp79F=loc;Cyc_Warn_err2(_tmp79F,_tag_fat(_tmp4CC,sizeof(void*),1U));});});
w=i;}}{
# 2731
void*_tmp4CF=Cyc_Tcutil_compress(field_type);void*_stmttmp6D=_tmp4CF;void*_tmp4D0=_stmttmp6D;enum Cyc_Absyn_Size_of _tmp4D1;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D0)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D0)->f1)->tag == 1U){_LL4: _tmp4D1=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4D0)->f1)->f2;_LL5: {enum Cyc_Absyn_Size_of b=_tmp4D1;
# 2734
{enum Cyc_Absyn_Size_of _tmp4D2=b;switch(_tmp4D2){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp4D3=0U;({unsigned _tmp7A1=loc;struct _fat_ptr _tmp7A0=({const char*_tmp4D4="bitfield larger than type";_tag_fat(_tmp4D4,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp7A1,_tmp7A0,_tag_fat(_tmp4D3,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp4D5=0U;({unsigned _tmp7A3=loc;struct _fat_ptr _tmp7A2=({const char*_tmp4D6="bitfield larger than type";_tag_fat(_tmp4D6,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp7A3,_tmp7A2,_tag_fat(_tmp4D5,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp4D7=0U;({unsigned _tmp7A5=loc;struct _fat_ptr _tmp7A4=({const char*_tmp4D8="bitfield larger than type";_tag_fat(_tmp4D8,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp7A5,_tmp7A4,_tag_fat(_tmp4D7,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp4D9=0U;({unsigned _tmp7A7=loc;struct _fat_ptr _tmp7A6=({const char*_tmp4DA="bitfield larger than type";_tag_fat(_tmp4DA,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp7A7,_tmp7A6,_tag_fat(_tmp4D9,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 2742
goto _LL3;}}else{goto _LL6;}}else{_LL6: _LL7:
# 2744
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4DC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp665;_tmp665.tag=0U,({struct _fat_ptr _tmp7A8=({const char*_tmp4E1="bitfield ";_tag_fat(_tmp4E1,sizeof(char),10U);});_tmp665.f1=_tmp7A8;});_tmp665;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4DD=({struct Cyc_Warn_String_Warn_Warg_struct _tmp664;_tmp664.tag=0U,_tmp664.f1=*fn;_tmp664;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4DE=({struct Cyc_Warn_String_Warn_Warg_struct _tmp663;_tmp663.tag=0U,({struct _fat_ptr _tmp7A9=({const char*_tmp4E0=" must have integral type but has type ";_tag_fat(_tmp4E0,sizeof(char),39U);});_tmp663.f1=_tmp7A9;});_tmp663;});struct Cyc_Warn_Typ_Warn_Warg_struct _tmp4DF=({struct Cyc_Warn_Typ_Warn_Warg_struct _tmp662;_tmp662.tag=2U,_tmp662.f1=(void*)field_type;_tmp662;});void*_tmp4DB[4U];_tmp4DB[0]=& _tmp4DC,_tmp4DB[1]=& _tmp4DD,_tmp4DB[2]=& _tmp4DE,_tmp4DB[3]=& _tmp4DF;({unsigned _tmp7AA=loc;Cyc_Warn_err2(_tmp7AA,_tag_fat(_tmp4DB,sizeof(void*),4U));});});
# 2746
goto _LL3;}_LL3:;}}}
# 2753
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp4E2=t;void*_tmp4E4;struct Cyc_Absyn_Typedefdecl*_tmp4E3;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E2)->tag == 8U){_LL1: _tmp4E3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E2)->f3;_tmp4E4=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp4E2)->f4;_LL2: {struct Cyc_Absyn_Typedefdecl*td=_tmp4E3;void*tdopt=_tmp4E4;
# 2756
if((((struct Cyc_Absyn_Typedefdecl*)_check_null(td))->tq).real_const ||(td->tq).print_const){
if(declared_const)({struct Cyc_Warn_String_Warn_Warg_struct _tmp4E6=({struct Cyc_Warn_String_Warn_Warg_struct _tmp666;_tmp666.tag=0U,({struct _fat_ptr _tmp7AB=({const char*_tmp4E7="extra const";_tag_fat(_tmp4E7,sizeof(char),12U);});_tmp666.f1=_tmp7AB;});_tmp666;});void*_tmp4E5[1U];_tmp4E5[0]=& _tmp4E6;({unsigned _tmp7AC=loc;Cyc_Warn_warn2(_tmp7AC,_tag_fat(_tmp4E5,sizeof(void*),1U));});});
return 1;}
# 2761
if((unsigned)tdopt)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,tdopt);else{
return declared_const;}}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 2768
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp7AD=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp7AD;});}
# 2772
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 2778
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 2781
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp4EA=({struct Cyc_String_pa_PrintArg_struct _tmp668;_tmp668.tag=0U,_tmp668.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp668;});struct Cyc_String_pa_PrintArg_struct _tmp4EB=({struct Cyc_String_pa_PrintArg_struct _tmp667;_tmp667.tag=0U,({struct _fat_ptr _tmp7AE=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp667.f1=_tmp7AE;});_tmp667;});void*_tmp4E8[2U];_tmp4E8[0]=& _tmp4EA,_tmp4E8[1]=& _tmp4EB;({unsigned _tmp7B0=loc;struct _fat_ptr _tmp7AF=({const char*_tmp4E9="%s: %s";_tag_fat(_tmp4E9,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp7B0,_tmp7AF,_tag_fat(_tmp4E8,sizeof(void*),2U));});});}}}
# 2787
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 2791
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 2795
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp7B2=tvs;unsigned _tmp7B1=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp7B2,_tmp7B1,Cyc_Absynpp_tvar2string,({const char*_tmp4EC="duplicate type variable";_tag_fat(_tmp4EC,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 2809 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 2814
struct _RegionHandle _tmp4ED=_new_region("temp");struct _RegionHandle*temp=& _tmp4ED;_push_region(temp);
# 2818
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp7B3=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp7B3,({const char*_tmp4EE="";_tag_fat(_tmp4EE,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp4F0=_region_malloc(temp,sizeof(*_tmp4F0));({struct _tuple29*_tmp7B4=({struct _tuple29*_tmp4EF=_region_malloc(temp,sizeof(*_tmp4EF));_tmp4EF->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp4EF->f2=0;_tmp4EF;});_tmp4F0->hd=_tmp7B4;}),_tmp4F0->tl=fields;_tmp4F0;});}}
# 2823
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 2825
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp51F="struct";_tag_fat(_tmp51F,sizeof(char),7U);}):({const char*_tmp520="union";_tag_fat(_tmp520,sizeof(char),6U);});
# 2828
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp4F1=(struct _tuple30*)des->hd;struct _tuple30*_stmttmp6E=_tmp4F1;struct _tuple30*_tmp4F2=_stmttmp6E;void*_tmp4F4;struct Cyc_List_List*_tmp4F3;_LL1: _tmp4F3=_tmp4F2->f1;_tmp4F4=_tmp4F2->f2;_LL2: {struct Cyc_List_List*dl=_tmp4F3;void*a=_tmp4F4;
if(dl == 0){
# 2833
struct Cyc_List_List*_tmp4F5=fields;struct Cyc_List_List*fields2=_tmp4F5;
for(0;fields2 != 0;fields2=fields2->tl){
if(!(*((struct _tuple29*)fields2->hd)).f2){
(*((struct _tuple29*)fields2->hd)).f2=1;
({struct Cyc_List_List*_tmp7B6=({struct Cyc_List_List*_tmp4F7=_cycalloc(sizeof(*_tmp4F7));({void*_tmp7B5=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp4F6=_cycalloc(sizeof(*_tmp4F6));_tmp4F6->tag=1U,_tmp4F6->f1=((*((struct _tuple29*)fields2->hd)).f1)->name;_tmp4F6;});_tmp4F7->hd=_tmp7B5;}),_tmp4F7->tl=0;_tmp4F7;});(*((struct _tuple30*)des->hd)).f1=_tmp7B6;});
ans=({struct Cyc_List_List*_tmp4F9=_region_malloc(rgn,sizeof(*_tmp4F9));({struct _tuple31*_tmp7B7=({struct _tuple31*_tmp4F8=_region_malloc(rgn,sizeof(*_tmp4F8));_tmp4F8->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp4F8->f2=a;_tmp4F8;});_tmp4F9->hd=_tmp7B7;}),_tmp4F9->tl=ans;_tmp4F9;});
break;}}
# 2841
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FB=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66A;_tmp66A.tag=0U,({struct _fat_ptr _tmp7B8=({const char*_tmp4FD="too many arguments to ";_tag_fat(_tmp4FD,sizeof(char),23U);});_tmp66A.f1=_tmp7B8;});_tmp66A;});struct Cyc_Warn_String_Warn_Warg_struct _tmp4FC=({struct Cyc_Warn_String_Warn_Warg_struct _tmp669;_tmp669.tag=0U,_tmp669.f1=aggr_str;_tmp669;});void*_tmp4FA[2U];_tmp4FA[0]=& _tmp4FB,_tmp4FA[1]=& _tmp4FC;({unsigned _tmp7B9=loc;Cyc_Warn_err2(_tmp7B9,_tag_fat(_tmp4FA,sizeof(void*),2U));});});}else{
if(dl->tl != 0)
# 2845
({struct Cyc_Warn_String_Warn_Warg_struct _tmp4FF=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66B;_tmp66B.tag=0U,({struct _fat_ptr _tmp7BA=({const char*_tmp500="multiple designators are not yet supported";_tag_fat(_tmp500,sizeof(char),43U);});_tmp66B.f1=_tmp7BA;});_tmp66B;});void*_tmp4FE[1U];_tmp4FE[0]=& _tmp4FF;({unsigned _tmp7BB=loc;Cyc_Warn_err2(_tmp7BB,_tag_fat(_tmp4FE,sizeof(void*),1U));});});else{
# 2848
void*_tmp501=(void*)dl->hd;void*_stmttmp6F=_tmp501;void*_tmp502=_stmttmp6F;struct _fat_ptr*_tmp503;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp502)->tag == 0U){_LL4: _LL5:
# 2850
({struct Cyc_Warn_String_Warn_Warg_struct _tmp505=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66D;_tmp66D.tag=0U,({struct _fat_ptr _tmp7BC=({const char*_tmp507="array designator used in argument to ";_tag_fat(_tmp507,sizeof(char),38U);});_tmp66D.f1=_tmp7BC;});_tmp66D;});struct Cyc_Warn_String_Warn_Warg_struct _tmp506=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66C;_tmp66C.tag=0U,_tmp66C.f1=aggr_str;_tmp66C;});void*_tmp504[2U];_tmp504[0]=& _tmp505,_tmp504[1]=& _tmp506;({unsigned _tmp7BD=loc;Cyc_Warn_err2(_tmp7BD,_tag_fat(_tmp504,sizeof(void*),2U));});});
goto _LL3;}else{_LL6: _tmp503=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp502)->f1;_LL7: {struct _fat_ptr*v=_tmp503;
# 2853
struct Cyc_List_List*_tmp508=fields;struct Cyc_List_List*fields2=_tmp508;
for(0;fields2 != 0;fields2=fields2->tl){
if(Cyc_strptrcmp(v,((*((struct _tuple29*)fields2->hd)).f1)->name)== 0){
if((*((struct _tuple29*)fields2->hd)).f2)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp50A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp670;_tmp670.tag=0U,({struct _fat_ptr _tmp7BE=({const char*_tmp50E="member ";_tag_fat(_tmp50E,sizeof(char),8U);});_tmp670.f1=_tmp7BE;});_tmp670;});struct Cyc_Warn_String_Warn_Warg_struct _tmp50B=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66F;_tmp66F.tag=0U,_tmp66F.f1=*v;_tmp66F;});struct Cyc_Warn_String_Warn_Warg_struct _tmp50C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp66E;_tmp66E.tag=0U,({struct _fat_ptr _tmp7BF=({const char*_tmp50D=" has already been used as an argument";_tag_fat(_tmp50D,sizeof(char),38U);});_tmp66E.f1=_tmp7BF;});_tmp66E;});void*_tmp509[3U];_tmp509[0]=& _tmp50A,_tmp509[1]=& _tmp50B,_tmp509[2]=& _tmp50C;({unsigned _tmp7C0=loc;Cyc_Warn_err2(_tmp7C0,_tag_fat(_tmp509,sizeof(void*),3U));});});
(*((struct _tuple29*)fields2->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp510=_region_malloc(rgn,sizeof(*_tmp510));({struct _tuple31*_tmp7C1=({struct _tuple31*_tmp50F=_region_malloc(rgn,sizeof(*_tmp50F));_tmp50F->f1=(*((struct _tuple29*)fields2->hd)).f1,_tmp50F->f2=a;_tmp50F;});_tmp510->hd=_tmp7C1;}),_tmp510->tl=ans;_tmp510;});
break;}}
# 2862
if(fields2 == 0)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp512=({struct Cyc_Warn_String_Warn_Warg_struct _tmp672;_tmp672.tag=0U,({struct _fat_ptr _tmp7C2=({const char*_tmp514="bad field designator ";_tag_fat(_tmp514,sizeof(char),22U);});_tmp672.f1=_tmp7C2;});_tmp672;});struct Cyc_Warn_String_Warn_Warg_struct _tmp513=({struct Cyc_Warn_String_Warn_Warg_struct _tmp671;_tmp671.tag=0U,_tmp671.f1=*v;_tmp671;});void*_tmp511[2U];_tmp511[0]=& _tmp512,_tmp511[1]=& _tmp513;({unsigned _tmp7C3=loc;Cyc_Warn_err2(_tmp7C3,_tag_fat(_tmp511,sizeof(void*),2U));});});
goto _LL3;}}_LL3:;}}}}
# 2867
if((int)aggr_kind == (int)0U)
# 2869
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple29*)fields->hd)).f2){
({struct Cyc_Warn_String_Warn_Warg_struct _tmp516=({struct Cyc_Warn_String_Warn_Warg_struct _tmp673;_tmp673.tag=0U,({struct _fat_ptr _tmp7C4=({const char*_tmp517="too few arguments to struct";_tag_fat(_tmp517,sizeof(char),28U);});_tmp673.f1=_tmp7C4;});_tmp673;});void*_tmp515[1U];_tmp515[0]=& _tmp516;({unsigned _tmp7C5=loc;Cyc_Warn_err2(_tmp7C5,_tag_fat(_tmp515,sizeof(void*),1U));});});
break;}}else{
# 2876
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple29*)fields->hd)).f2){
if(found)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp519=({struct Cyc_Warn_String_Warn_Warg_struct _tmp674;_tmp674.tag=0U,({struct _fat_ptr _tmp7C6=({const char*_tmp51A="only one member of a union is allowed";_tag_fat(_tmp51A,sizeof(char),38U);});_tmp674.f1=_tmp7C6;});_tmp674;});void*_tmp518[1U];_tmp518[0]=& _tmp519;({unsigned _tmp7C7=loc;Cyc_Warn_err2(_tmp7C7,_tag_fat(_tmp518,sizeof(void*),1U));});});
found=1;}}
# 2883
if(!found)
({struct Cyc_Warn_String_Warn_Warg_struct _tmp51C=({struct Cyc_Warn_String_Warn_Warg_struct _tmp675;_tmp675.tag=0U,({struct _fat_ptr _tmp7C8=({const char*_tmp51D="missing member for union";_tag_fat(_tmp51D,sizeof(char),25U);});_tmp675.f1=_tmp7C8;});_tmp675;});void*_tmp51B[1U];_tmp51B[0]=& _tmp51C;({unsigned _tmp7C9=loc;Cyc_Warn_err2(_tmp7C9,_tag_fat(_tmp51B,sizeof(void*),1U));});});}{
# 2887
struct Cyc_List_List*_tmp51E=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp51E;}}}
# 2818
;_pop_region();}
# 2893
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 2895
void*_tmp521=e1->r;void*_stmttmp70=_tmp521;void*_tmp522=_stmttmp70;struct Cyc_Absyn_Exp*_tmp523;struct Cyc_Absyn_Exp*_tmp524;struct Cyc_Absyn_Exp*_tmp525;struct Cyc_Absyn_Exp*_tmp526;struct Cyc_Absyn_Exp*_tmp527;struct Cyc_Absyn_Exp*_tmp528;switch(*((int*)_tmp522)){case 14U: _LL1: _LL2:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52A=({struct Cyc_Warn_String_Warn_Warg_struct _tmp677;_tmp677.tag=0U,({struct _fat_ptr _tmp7CA=({const char*_tmp52C="we have a cast in a lhs: ";_tag_fat(_tmp52C,sizeof(char),26U);});_tmp677.f1=_tmp7CA;});_tmp677;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp52B=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp676;_tmp676.tag=4U,_tmp676.f1=e1;_tmp676;});void*_tmp529[2U];_tmp529[0]=& _tmp52A,_tmp529[1]=& _tmp52B;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp529,sizeof(void*),2U));});case 20U: _LL3: _tmp528=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp522)->f1;_LL4: {struct Cyc_Absyn_Exp*e1a=_tmp528;
_tmp527=e1a;goto _LL6;}case 23U: _LL5: _tmp527=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp522)->f1;_LL6: {struct Cyc_Absyn_Exp*e1a=_tmp527;
# 2899
return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type);}case 22U: _LL7: _tmp526=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp522)->f1;_LL8: {struct Cyc_Absyn_Exp*e1a=_tmp526;
_tmp525=e1a;goto _LLA;}case 21U: _LL9: _tmp525=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp522)->f1;_LLA: {struct Cyc_Absyn_Exp*e1a=_tmp525;
# 2903
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp52E=({struct Cyc_Warn_String_Warn_Warg_struct _tmp679;_tmp679.tag=0U,({struct _fat_ptr _tmp7CB=({const char*_tmp530="found zero pointer aggregate member assignment: ";_tag_fat(_tmp530,sizeof(char),49U);});_tmp679.f1=_tmp7CB;});_tmp679;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp52F=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp678;_tmp678.tag=4U,_tmp678.f1=e1;_tmp678;});void*_tmp52D[2U];_tmp52D[0]=& _tmp52E,_tmp52D[1]=& _tmp52F;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp52D,sizeof(void*),2U));});
return 0;}case 13U: _LLB: _tmp524=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp522)->f1;_LLC: {struct Cyc_Absyn_Exp*e1a=_tmp524;
_tmp523=e1a;goto _LLE;}case 12U: _LLD: _tmp523=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp522)->f1;_LLE: {struct Cyc_Absyn_Exp*e1a=_tmp523;
# 2908
if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(e1a->topt),ptr_type,is_fat,elt_type))
({struct Cyc_Warn_String_Warn_Warg_struct _tmp532=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67B;_tmp67B.tag=0U,({struct _fat_ptr _tmp7CC=({const char*_tmp534="found zero pointer instantiate/noinstantiate: ";_tag_fat(_tmp534,sizeof(char),47U);});_tmp67B.f1=_tmp7CC;});_tmp67B;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp533=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp67A;_tmp67A.tag=4U,_tmp67A.f1=e1;_tmp67A;});void*_tmp531[2U];_tmp531[0]=& _tmp532,_tmp531[1]=& _tmp533;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp531,sizeof(void*),2U));});
return 0;}case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_Warn_String_Warn_Warg_struct _tmp536=({struct Cyc_Warn_String_Warn_Warg_struct _tmp67D;_tmp67D.tag=0U,({struct _fat_ptr _tmp7CD=({const char*_tmp538="found bad lhs in is_zero_ptr_deref: ";_tag_fat(_tmp538,sizeof(char),37U);});_tmp67D.f1=_tmp7CD;});_tmp67D;});struct Cyc_Warn_Exp_Warn_Warg_struct _tmp537=({struct Cyc_Warn_Exp_Warn_Warg_struct _tmp67C;_tmp67C.tag=4U,_tmp67C.f1=e1;_tmp67C;});void*_tmp535[2U];_tmp535[0]=& _tmp536,_tmp535[1]=& _tmp537;((int(*)(struct _fat_ptr))Cyc_Warn_impos2)(_tag_fat(_tmp535,sizeof(void*),2U));});}_LL0:;}
# 2922
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
void*_tmp539=Cyc_Tcutil_compress(r);void*_stmttmp71=_tmp539;void*_tmp53A=_stmttmp71;struct Cyc_Absyn_Tvar*_tmp53B;enum Cyc_Absyn_AliasQual _tmp53D;enum Cyc_Absyn_KindQual _tmp53C;switch(*((int*)_tmp53A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp53A)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53A)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53A)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53A)->f4 == 0){_LL5: _tmp53C=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53A)->f3)->kind)->v)->kind;_tmp53D=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp53A)->f3)->kind)->v)->aliasqual;_LL6: {enum Cyc_Absyn_KindQual k=_tmp53C;enum Cyc_Absyn_AliasQual a=_tmp53D;
# 2927
return(int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique);}}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp53B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp53A)->f1;_LL8: {struct Cyc_Absyn_Tvar*tv=_tmp53B;
# 2931
struct Cyc_Absyn_Kind*_tmp53E=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_stmttmp72=_tmp53E;struct Cyc_Absyn_Kind*_tmp53F=_stmttmp72;enum Cyc_Absyn_AliasQual _tmp541;enum Cyc_Absyn_KindQual _tmp540;_LLC: _tmp540=_tmp53F->kind;_tmp541=_tmp53F->aliasqual;_LLD: {enum Cyc_Absyn_KindQual k=_tmp540;enum Cyc_Absyn_AliasQual a=_tmp541;
if((int)k == (int)3U &&((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique)){
void*_tmp542=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp73=_tmp542;void*_tmp543=_stmttmp73;struct Cyc_Core_Opt**_tmp544;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp544=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp543)->f1;_LL10: {struct Cyc_Core_Opt**x=_tmp544;
# 2935
({struct Cyc_Core_Opt*_tmp7CF=({struct Cyc_Core_Opt*_tmp546=_cycalloc(sizeof(*_tmp546));({void*_tmp7CE=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp545=_cycalloc(sizeof(*_tmp545));_tmp545->tag=2U,_tmp545->f1=0,_tmp545->f2=& Cyc_Tcutil_rk;_tmp545;});_tmp546->v=_tmp7CE;});_tmp546;});*x=_tmp7CF;});
return 0;}}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 2940
return 0;}}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 2947
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp547=Cyc_Tcutil_compress(t);void*_stmttmp74=_tmp547;void*_tmp548=_stmttmp74;struct Cyc_Absyn_Tvar*_tmp549;void*_tmp54A;switch(*((int*)_tmp548)){case 3U: _LL1: _tmp54A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp548)->f1).ptr_atts).rgn;_LL2: {void*r=_tmp54A;
# 2950
return Cyc_Tcutil_is_noalias_region(r,must_be_unique);}case 2U: _LL3: _tmp549=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp548)->f1;_LL4: {struct Cyc_Absyn_Tvar*tv=_tmp549;
# 2952
struct Cyc_Absyn_Kind*_tmp54B=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_stmttmp75=_tmp54B;struct Cyc_Absyn_Kind*_tmp54C=_stmttmp75;enum Cyc_Absyn_AliasQual _tmp54E;enum Cyc_Absyn_KindQual _tmp54D;_LL8: _tmp54D=_tmp54C->kind;_tmp54E=_tmp54C->aliasqual;_LL9: {enum Cyc_Absyn_KindQual k=_tmp54D;enum Cyc_Absyn_AliasQual a=_tmp54E;
enum Cyc_Absyn_KindQual _tmp54F=k;switch(_tmp54F){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)a == (int)1U ||(int)a == (int)2U && !must_be_unique){
void*_tmp550=Cyc_Absyn_compress_kb(tv->kind);void*_stmttmp76=_tmp550;void*_tmp551=_stmttmp76;enum Cyc_Absyn_KindQual _tmp553;struct Cyc_Core_Opt**_tmp552;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp551)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp551)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp552=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp551)->f1;_tmp553=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp551)->f2)->kind;_LL15: {struct Cyc_Core_Opt**x=_tmp552;enum Cyc_Absyn_KindQual k=_tmp553;
# 2958
({struct Cyc_Core_Opt*_tmp7D2=({struct Cyc_Core_Opt*_tmp556=_cycalloc(sizeof(*_tmp556));({void*_tmp7D1=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp555=_cycalloc(sizeof(*_tmp555));_tmp555->tag=2U,_tmp555->f1=0,({struct Cyc_Absyn_Kind*_tmp7D0=({struct Cyc_Absyn_Kind*_tmp554=_cycalloc(sizeof(*_tmp554));_tmp554->kind=k,_tmp554->aliasqual=Cyc_Absyn_Aliasable;_tmp554;});_tmp555->f2=_tmp7D0;});_tmp555;});_tmp556->v=_tmp7D1;});_tmp556;});*x=_tmp7D2;});
return 0;}}else{goto _LL16;}}else{_LL16: _LL17:
 return 1;}_LL13:;}
# 2963
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;}}default: _LL5: _LL6:
# 2966
 return 0;}_LL0:;}
# 2969
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp557=Cyc_Tcutil_compress(t);void*t=_tmp557;
if(Cyc_Tcutil_is_noalias_pointer(t,0))
return 1;{
void*_tmp558=t;struct Cyc_List_List*_tmp559;struct Cyc_List_List*_tmp55B;union Cyc_Absyn_DatatypeFieldInfo _tmp55A;struct Cyc_List_List*_tmp55D;struct Cyc_Absyn_Aggrdecl**_tmp55C;struct Cyc_List_List*_tmp55E;switch(*((int*)_tmp558)){case 6U: _LL1: _tmp55E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp558)->f1;_LL2: {struct Cyc_List_List*qts=_tmp55E;
# 2975
for(0;qts != 0;qts=qts->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple12*)qts->hd)).f2))
return 1;}
return 0;}case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp55C=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f1)->f1).KnownAggr).val;_tmp55D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f2;_LL4: {struct Cyc_Absyn_Aggrdecl**adp=_tmp55C;struct Cyc_List_List*ts=_tmp55D;
# 2980
if((*adp)->impl == 0)
return 0;{
struct Cyc_List_List*_tmp55F=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*adp)->tvs,ts);struct Cyc_List_List*inst=_tmp55F;
{struct Cyc_List_List*_tmp560=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*adp)->impl))->fields;struct Cyc_List_List*x=_tmp560;for(0;x != 0;x=x->tl){
void*t=inst == 0?((struct Cyc_Absyn_Aggrfield*)x->hd)->type: Cyc_Tcutil_substitute(inst,((struct Cyc_Absyn_Aggrfield*)x->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 2988
return 0;}}}else{_LL7: _LL8:
# 2994
 goto _LLA;}case 18U: _LL9: _LLA:
 return 0;case 19U: _LLB: _tmp55A=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f1)->f1;_tmp55B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp558)->f2;_LLC: {union Cyc_Absyn_DatatypeFieldInfo tinfo=_tmp55A;struct Cyc_List_List*ts=_tmp55B;
# 2998
union Cyc_Absyn_DatatypeFieldInfo _tmp561=tinfo;struct Cyc_Absyn_Datatypefield*_tmp563;struct Cyc_Absyn_Datatypedecl*_tmp562;if((_tmp561.UnknownDatatypefield).tag == 1){_LL10: _LL11:
 return 0;}else{_LL12: _tmp562=((_tmp561.KnownDatatypefield).val).f1;_tmp563=((_tmp561.KnownDatatypefield).val).f2;_LL13: {struct Cyc_Absyn_Datatypedecl*td=_tmp562;struct Cyc_Absyn_Datatypefield*fld=_tmp563;
# 3001
struct Cyc_List_List*_tmp564=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(td->tvs,ts);struct Cyc_List_List*inst=_tmp564;
{struct Cyc_List_List*_tmp565=fld->typs;struct Cyc_List_List*typs=_tmp565;for(0;typs != 0;typs=typs->tl){
t=inst == 0?(*((struct _tuple12*)typs->hd)).f2: Cyc_Tcutil_substitute(inst,(*((struct _tuple12*)typs->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))
return 1;}}
# 3007
return 0;}}_LLF:;}default: goto _LLD;}case 7U: _LL5: _tmp559=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp558)->f2;_LL6: {struct Cyc_List_List*x=_tmp559;
# 2990
for(0;x != 0;x=x->tl){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)x->hd)->type))
return 1;}
return 0;}default: _LLD: _LLE:
# 3009
 return 0;}_LL0:;}}
# 3016
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp566=e->r;void*_stmttmp77=_tmp566;void*_tmp567=_stmttmp77;struct Cyc_Absyn_Stmt*_tmp568;struct Cyc_Absyn_Exp*_tmp569;struct Cyc_Absyn_Exp*_tmp56A;struct Cyc_Absyn_Exp*_tmp56C;struct Cyc_Absyn_Exp*_tmp56B;struct Cyc_Absyn_Exp*_tmp56E;struct Cyc_Absyn_Exp*_tmp56D;struct _fat_ptr*_tmp570;struct Cyc_Absyn_Exp*_tmp56F;struct Cyc_Absyn_Exp*_tmp571;struct Cyc_Absyn_Exp*_tmp572;switch(*((int*)_tmp567)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp567)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp572=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp572;
_tmp571=e1;goto _LL6;}case 20U: _LL5: _tmp571=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp571;
# 3021
return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(e1->topt),1)&& Cyc_Tcutil_is_noalias_path(e1);}case 21U: _LL7: _tmp56F=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_tmp570=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp56F;struct _fat_ptr*f=_tmp570;
return Cyc_Tcutil_is_noalias_path(e1);}case 23U: _LL9: _tmp56D=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_tmp56E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp56D;struct Cyc_Absyn_Exp*e2=_tmp56E;
# 3024
void*_tmp573=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp78=_tmp573;void*_tmp574=_stmttmp78;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp574)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(e1);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp56B=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_tmp56C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp567)->f3;_LLC: {struct Cyc_Absyn_Exp*e1=_tmp56B;struct Cyc_Absyn_Exp*e2=_tmp56C;
# 3029
return Cyc_Tcutil_is_noalias_path(e1)&& Cyc_Tcutil_is_noalias_path(e2);}case 9U: _LLD: _tmp56A=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_LLE: {struct Cyc_Absyn_Exp*e2=_tmp56A;
_tmp569=e2;goto _LL10;}case 14U: _LLF: _tmp569=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp567)->f2;_LL10: {struct Cyc_Absyn_Exp*e2=_tmp569;
return Cyc_Tcutil_is_noalias_path(e2);}case 37U: _LL11: _tmp568=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp567)->f1;_LL12: {struct Cyc_Absyn_Stmt*s=_tmp568;
# 3033
while(1){
void*_tmp575=s->r;void*_stmttmp79=_tmp575;void*_tmp576=_stmttmp79;struct Cyc_Absyn_Exp*_tmp577;struct Cyc_Absyn_Stmt*_tmp579;struct Cyc_Absyn_Decl*_tmp578;struct Cyc_Absyn_Stmt*_tmp57B;struct Cyc_Absyn_Stmt*_tmp57A;switch(*((int*)_tmp576)){case 2U: _LL1B: _tmp57A=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp57B=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LL1C: {struct Cyc_Absyn_Stmt*s1=_tmp57A;struct Cyc_Absyn_Stmt*s2=_tmp57B;
s=s2;goto _LL1A;}case 12U: _LL1D: _tmp578=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_tmp579=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp576)->f2;_LL1E: {struct Cyc_Absyn_Decl*d=_tmp578;struct Cyc_Absyn_Stmt*s1=_tmp579;
s=s1;goto _LL1A;}case 1U: _LL1F: _tmp577=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp576)->f1;_LL20: {struct Cyc_Absyn_Exp*e=_tmp577;
return Cyc_Tcutil_is_noalias_path(e);}default: _LL21: _LL22:
({void*_tmp57C=0U;({struct _fat_ptr _tmp7D3=({const char*_tmp57D="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp57D,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7D3,_tag_fat(_tmp57C,sizeof(void*),0U));});});}_LL1A:;}}default: _LL13: _LL14:
# 3041
 return 1;}_LL0:;}
# 3058 "tcutil.cyc"
struct _tuple14 Cyc_Tcutil_addressof_props(struct Cyc_Absyn_Exp*e){
# 3060
struct _tuple14 bogus_ans=({struct _tuple14 _tmp68A;_tmp68A.f1=0,_tmp68A.f2=Cyc_Absyn_heap_rgn_type;_tmp68A;});
void*_tmp57E=e->r;void*_stmttmp7A=_tmp57E;void*_tmp57F=_stmttmp7A;struct Cyc_Absyn_Exp*_tmp581;struct Cyc_Absyn_Exp*_tmp580;struct Cyc_Absyn_Exp*_tmp582;int _tmp585;struct _fat_ptr*_tmp584;struct Cyc_Absyn_Exp*_tmp583;int _tmp588;struct _fat_ptr*_tmp587;struct Cyc_Absyn_Exp*_tmp586;void*_tmp589;switch(*((int*)_tmp57F)){case 1U: _LL1: _tmp589=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp57F)->f1;_LL2: {void*x=_tmp589;
# 3064
void*_tmp58A=x;struct Cyc_Absyn_Vardecl*_tmp58B;struct Cyc_Absyn_Vardecl*_tmp58C;struct Cyc_Absyn_Vardecl*_tmp58D;struct Cyc_Absyn_Vardecl*_tmp58E;switch(*((int*)_tmp58A)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp58E=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp58A)->f1;_LL13: {struct Cyc_Absyn_Vardecl*vd=_tmp58E;
# 3068
void*_tmp58F=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7B=_tmp58F;void*_tmp590=_stmttmp7B;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp590)->tag == 4U){_LL1B: _LL1C:
# 3070
 return({struct _tuple14 _tmp67E;_tmp67E.f1=1,_tmp67E.f2=Cyc_Absyn_heap_rgn_type;_tmp67E;});}else{_LL1D: _LL1E:
 return({struct _tuple14 _tmp67F;_tmp67F.f1=(vd->tq).real_const,_tmp67F.f2=Cyc_Absyn_heap_rgn_type;_tmp67F;});}_LL1A:;}case 4U: _LL14: _tmp58D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp58A)->f1;_LL15: {struct Cyc_Absyn_Vardecl*vd=_tmp58D;
# 3074
void*_tmp591=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_stmttmp7C=_tmp591;void*_tmp592=_stmttmp7C;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp592)->tag == 4U){_LL20: _LL21:
 return({struct _tuple14 _tmp680;_tmp680.f1=1,_tmp680.f2=(void*)_check_null(vd->rgn);_tmp680;});}else{_LL22: _LL23:
# 3077
 vd->escapes=1;
return({struct _tuple14 _tmp681;_tmp681.f1=(vd->tq).real_const,_tmp681.f2=(void*)_check_null(vd->rgn);_tmp681;});}_LL1F:;}case 5U: _LL16: _tmp58C=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp58A)->f1;_LL17: {struct Cyc_Absyn_Vardecl*vd=_tmp58C;
# 3080
_tmp58B=vd;goto _LL19;}default: _LL18: _tmp58B=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp58A)->f1;_LL19: {struct Cyc_Absyn_Vardecl*vd=_tmp58B;
# 3082
vd->escapes=1;
return({struct _tuple14 _tmp682;_tmp682.f1=(vd->tq).real_const,_tmp682.f2=(void*)_check_null(vd->rgn);_tmp682;});}}_LLD:;}case 21U: _LL3: _tmp586=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp57F)->f1;_tmp587=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp57F)->f2;_tmp588=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp57F)->f3;_LL4: {struct Cyc_Absyn_Exp*e1=_tmp586;struct _fat_ptr*f=_tmp587;int is_tagged=_tmp588;
# 3087
if(is_tagged)return bogus_ans;{
# 3090
void*_tmp593=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp7D=_tmp593;void*_tmp594=_stmttmp7D;struct Cyc_Absyn_Aggrdecl*_tmp595;struct Cyc_List_List*_tmp596;switch(*((int*)_tmp594)){case 7U: _LL25: _tmp596=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp594)->f2;_LL26: {struct Cyc_List_List*fs=_tmp596;
# 3092
struct Cyc_Absyn_Aggrfield*_tmp597=Cyc_Absyn_lookup_field(fs,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp597;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp598=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7E=_tmp598;struct _tuple14 _tmp599=_stmttmp7E;void*_tmp59B;int _tmp59A;_LL2C: _tmp59A=_tmp599.f1;_tmp59B=_tmp599.f2;_LL2D: {int c=_tmp59A;void*t=_tmp59B;
return({struct _tuple14 _tmp683;_tmp683.f1=(finfo->tq).real_const || c,_tmp683.f2=t;_tmp683;});}}
# 3097
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp594)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp594)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp595=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp594)->f1)->f1).KnownAggr).val;_LL28: {struct Cyc_Absyn_Aggrdecl*ad=_tmp595;
# 3099
struct Cyc_Absyn_Aggrfield*_tmp59C=Cyc_Absyn_lookup_decl_field(ad,f);struct Cyc_Absyn_Aggrfield*finfo=_tmp59C;
if(finfo != 0 && finfo->width == 0){
struct _tuple14 _tmp59D=Cyc_Tcutil_addressof_props(e1);struct _tuple14 _stmttmp7F=_tmp59D;struct _tuple14 _tmp59E=_stmttmp7F;void*_tmp5A0;int _tmp59F;_LL2F: _tmp59F=_tmp59E.f1;_tmp5A0=_tmp59E.f2;_LL30: {int c=_tmp59F;void*t=_tmp5A0;
return({struct _tuple14 _tmp684;_tmp684.f1=(finfo->tq).real_const || c,_tmp684.f2=t;_tmp684;});}}
# 3104
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;}}case 22U: _LL5: _tmp583=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp57F)->f1;_tmp584=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp57F)->f2;_tmp585=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp57F)->f3;_LL6: {struct Cyc_Absyn_Exp*e1=_tmp583;struct _fat_ptr*f=_tmp584;int is_tagged=_tmp585;
# 3109
if(is_tagged)return bogus_ans;{
# 3112
void*_tmp5A1=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp80=_tmp5A1;void*_tmp5A2=_stmttmp80;void*_tmp5A4;void*_tmp5A3;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->tag == 3U){_LL32: _tmp5A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->f1).elt_type;_tmp5A4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5A2)->f1).ptr_atts).rgn;_LL33: {void*t1=_tmp5A3;void*r=_tmp5A4;
# 3114
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp5A5=Cyc_Tcutil_compress(t1);void*_stmttmp81=_tmp5A5;void*_tmp5A6=_stmttmp81;struct Cyc_Absyn_Aggrdecl*_tmp5A7;struct Cyc_List_List*_tmp5A8;switch(*((int*)_tmp5A6)){case 7U: _LL37: _tmp5A8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5A6)->f2;_LL38: {struct Cyc_List_List*fs=_tmp5A8;
# 3117
finfo=Cyc_Absyn_lookup_field(fs,f);goto _LL36;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5A6)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5A6)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp5A7=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5A6)->f1)->f1).KnownAggr).val;_LL3A: {struct Cyc_Absyn_Aggrdecl*ad=_tmp5A7;
# 3119
finfo=Cyc_Absyn_lookup_decl_field(ad,f);goto _LL36;}}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3122
if(finfo != 0 && finfo->width == 0)
return({struct _tuple14 _tmp685;_tmp685.f1=(finfo->tq).real_const,_tmp685.f2=r;_tmp685;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;}}case 20U: _LL7: _tmp582=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp57F)->f1;_LL8: {struct Cyc_Absyn_Exp*e1=_tmp582;
# 3129
void*_tmp5A9=Cyc_Tcutil_compress((void*)_check_null(e1->topt));void*_stmttmp82=_tmp5A9;void*_tmp5AA=_stmttmp82;void*_tmp5AC;struct Cyc_Absyn_Tqual _tmp5AB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AA)->tag == 3U){_LL3E: _tmp5AB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AA)->f1).elt_tq;_tmp5AC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AA)->f1).ptr_atts).rgn;_LL3F: {struct Cyc_Absyn_Tqual tq=_tmp5AB;void*r=_tmp5AC;
# 3131
return({struct _tuple14 _tmp686;_tmp686.f1=tq.real_const,_tmp686.f2=r;_tmp686;});}}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp580=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp57F)->f1;_tmp581=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp57F)->f2;_LLA: {struct Cyc_Absyn_Exp*e1=_tmp580;struct Cyc_Absyn_Exp*e2=_tmp581;
# 3137
void*t=Cyc_Tcutil_compress((void*)_check_null(e1->topt));
void*_tmp5AD=t;struct Cyc_Absyn_Tqual _tmp5AE;void*_tmp5B0;struct Cyc_Absyn_Tqual _tmp5AF;struct Cyc_List_List*_tmp5B1;switch(*((int*)_tmp5AD)){case 6U: _LL43: _tmp5B1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5AD)->f1;_LL44: {struct Cyc_List_List*ts=_tmp5B1;
# 3141
struct _tuple13 _tmp5B2=Cyc_Evexp_eval_const_uint_exp(e2);struct _tuple13 _stmttmp83=_tmp5B2;struct _tuple13 _tmp5B3=_stmttmp83;int _tmp5B5;unsigned _tmp5B4;_LL4C: _tmp5B4=_tmp5B3.f1;_tmp5B5=_tmp5B3.f2;_LL4D: {unsigned i=_tmp5B4;int known=_tmp5B5;
if(!known)
return bogus_ans;{
struct _tuple12*_tmp5B6=Cyc_Absyn_lookup_tuple_field(ts,(int)i);struct _tuple12*finfo=_tmp5B6;
if(finfo != 0)
return({struct _tuple14 _tmp687;_tmp687.f1=((*finfo).f1).real_const,({void*_tmp7D4=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp687.f2=_tmp7D4;});_tmp687;});
return bogus_ans;}}}case 3U: _LL45: _tmp5AF=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AD)->f1).elt_tq;_tmp5B0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5AD)->f1).ptr_atts).rgn;_LL46: {struct Cyc_Absyn_Tqual tq=_tmp5AF;void*r=_tmp5B0;
# 3149
return({struct _tuple14 _tmp688;_tmp688.f1=tq.real_const,_tmp688.f2=r;_tmp688;});}case 4U: _LL47: _tmp5AE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5AD)->f1).tq;_LL48: {struct Cyc_Absyn_Tqual tq=_tmp5AE;
# 3155
return({struct _tuple14 _tmp689;_tmp689.f1=tq.real_const,({void*_tmp7D5=(Cyc_Tcutil_addressof_props(e1)).f2;_tmp689.f2=_tmp7D5;});_tmp689;});}default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3159
({void*_tmp5B7=0U;({unsigned _tmp7D7=e->loc;struct _fat_ptr _tmp7D6=({const char*_tmp5B8="unary & applied to non-lvalue";_tag_fat(_tmp5B8,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp7D7,_tmp7D6,_tag_fat(_tmp5B7,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3166
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp5B9=({void*_tmp7D8=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7D8,b);});struct Cyc_Absyn_Exp*eopt=_tmp5B9;
if(eopt == 0)return;{
struct Cyc_Absyn_Exp*_tmp5BA=eopt;struct Cyc_Absyn_Exp*e=_tmp5BA;
struct _tuple13 _tmp5BB=Cyc_Evexp_eval_const_uint_exp(e);struct _tuple13 _stmttmp84=_tmp5BB;struct _tuple13 _tmp5BC=_stmttmp84;int _tmp5BE;unsigned _tmp5BD;_LL1: _tmp5BD=_tmp5BC.f1;_tmp5BE=_tmp5BC.f2;_LL2: {unsigned j=_tmp5BD;int known=_tmp5BE;
if(known && j <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp5C1=({struct Cyc_Int_pa_PrintArg_struct _tmp68C;_tmp68C.tag=1U,_tmp68C.f1=(unsigned long)((int)j);_tmp68C;});struct Cyc_Int_pa_PrintArg_struct _tmp5C2=({struct Cyc_Int_pa_PrintArg_struct _tmp68B;_tmp68B.tag=1U,_tmp68B.f1=(unsigned long)((int)i);_tmp68B;});void*_tmp5BF[2U];_tmp5BF[0]=& _tmp5C1,_tmp5BF[1]=& _tmp5C2;({unsigned _tmp7DA=loc;struct _fat_ptr _tmp7D9=({const char*_tmp5C0="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp5C0,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp7DA,_tmp7D9,_tag_fat(_tmp5BF,sizeof(void*),2U));});});else{
# 3175
({struct Cyc_Int_pa_PrintArg_struct _tmp5C5=({struct Cyc_Int_pa_PrintArg_struct _tmp68E;_tmp68E.tag=1U,_tmp68E.f1=(unsigned long)((int)j);_tmp68E;});struct Cyc_Int_pa_PrintArg_struct _tmp5C6=({struct Cyc_Int_pa_PrintArg_struct _tmp68D;_tmp68D.tag=1U,_tmp68D.f1=(unsigned long)((int)i);_tmp68D;});void*_tmp5C3[2U];_tmp5C3[0]=& _tmp5C5,_tmp5C3[1]=& _tmp5C6;({unsigned _tmp7DC=loc;struct _fat_ptr _tmp7DB=({const char*_tmp5C4="dereference is out of bounds: %d <= %d";_tag_fat(_tmp5C4,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp7DC,_tmp7DB,_tag_fat(_tmp5C3,sizeof(void*),2U));});});}}
return;}}}
# 3179
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3187
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp5C7=e->r;void*_stmttmp85=_tmp5C7;void*_tmp5C8=_stmttmp85;struct Cyc_List_List*_tmp5C9;struct Cyc_List_List*_tmp5CA;struct Cyc_List_List*_tmp5CC;enum Cyc_Absyn_Primop _tmp5CB;struct Cyc_List_List*_tmp5CD;struct Cyc_List_List*_tmp5CE;struct Cyc_List_List*_tmp5CF;struct Cyc_Absyn_Exp*_tmp5D0;struct Cyc_Absyn_Exp*_tmp5D2;struct Cyc_Absyn_Exp*_tmp5D1;struct Cyc_Absyn_Exp*_tmp5D3;struct Cyc_Absyn_Exp*_tmp5D5;void*_tmp5D4;struct Cyc_Absyn_Exp*_tmp5D7;void*_tmp5D6;struct Cyc_Absyn_Exp*_tmp5D8;struct Cyc_Absyn_Exp*_tmp5D9;struct Cyc_Absyn_Exp*_tmp5DA;struct Cyc_Absyn_Exp*_tmp5DC;struct Cyc_Absyn_Exp*_tmp5DB;struct Cyc_Absyn_Exp*_tmp5DF;struct Cyc_Absyn_Exp*_tmp5DE;struct Cyc_Absyn_Exp*_tmp5DD;void*_tmp5E0;switch(*((int*)_tmp5C8)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp5E0=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL10: {void*b=_tmp5E0;
# 3199
void*_tmp5E1=b;struct Cyc_Absyn_Vardecl*_tmp5E2;struct Cyc_Absyn_Vardecl*_tmp5E3;switch(*((int*)_tmp5E1)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp5E3=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp5E1)->f1;_LL37: {struct Cyc_Absyn_Vardecl*vd=_tmp5E3;
# 3202
void*_tmp5E4=Cyc_Tcutil_compress(vd->type);void*_stmttmp86=_tmp5E4;void*_tmp5E5=_stmttmp86;switch(*((int*)_tmp5E5)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp5E2=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp5E1)->f1;_LL39: {struct Cyc_Absyn_Vardecl*vd=_tmp5E2;
# 3209
if((int)vd->sc == (int)Cyc_Absyn_Static){
void*_tmp5E6=Cyc_Tcutil_compress(vd->type);void*_stmttmp87=_tmp5E6;void*_tmp5E7=_stmttmp87;switch(*((int*)_tmp5E7)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3216
return var_okay;}}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp5DD=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_tmp5DE=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C8)->f2;_tmp5DF=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp5C8)->f3;_LL12: {struct Cyc_Absyn_Exp*e1=_tmp5DD;struct Cyc_Absyn_Exp*e2=_tmp5DE;struct Cyc_Absyn_Exp*e3=_tmp5DF;
# 3222
return(Cyc_Tcutil_cnst_exp(0,e1)&&
 Cyc_Tcutil_cnst_exp(0,e2))&&
 Cyc_Tcutil_cnst_exp(0,e3);}case 9U: _LL13: _tmp5DB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_tmp5DC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp5C8)->f2;_LL14: {struct Cyc_Absyn_Exp*e1=_tmp5DB;struct Cyc_Absyn_Exp*e2=_tmp5DC;
# 3226
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 41U: _LL15: _tmp5DA=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL16: {struct Cyc_Absyn_Exp*e2=_tmp5DA;
_tmp5D9=e2;goto _LL18;}case 12U: _LL17: _tmp5D9=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL18: {struct Cyc_Absyn_Exp*e2=_tmp5D9;
_tmp5D8=e2;goto _LL1A;}case 13U: _LL19: _tmp5D8=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL1A: {struct Cyc_Absyn_Exp*e2=_tmp5D8;
# 3230
return Cyc_Tcutil_cnst_exp(var_okay,e2);}case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C8)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp5D6=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_tmp5D7=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C8)->f2;_LL1C: {void*t=_tmp5D6;struct Cyc_Absyn_Exp*e2=_tmp5D7;
# 3232
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}else{_LL1D: _tmp5D4=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_tmp5D5=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp5C8)->f2;_LL1E: {void*t=_tmp5D4;struct Cyc_Absyn_Exp*e2=_tmp5D5;
# 3235
return Cyc_Tcutil_cnst_exp(var_okay,e2);}}case 15U: _LL1F: _tmp5D3=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL20: {struct Cyc_Absyn_Exp*e2=_tmp5D3;
# 3237
return Cyc_Tcutil_cnst_exp(1,e2);}case 27U: _LL21: _tmp5D1=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C8)->f2;_tmp5D2=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp5C8)->f3;_LL22: {struct Cyc_Absyn_Exp*e1=_tmp5D1;struct Cyc_Absyn_Exp*e2=_tmp5D2;
# 3239
return Cyc_Tcutil_cnst_exp(0,e1)&& Cyc_Tcutil_cnst_exp(0,e2);}case 28U: _LL23: _tmp5D0=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL24: {struct Cyc_Absyn_Exp*e=_tmp5D0;
# 3241
return Cyc_Tcutil_cnst_exp(0,e);}case 26U: _LL25: _tmp5CF=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL26: {struct Cyc_List_List*des=_tmp5CF;
_tmp5CE=des;goto _LL28;}case 30U: _LL27: _tmp5CE=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp5C8)->f2;_LL28: {struct Cyc_List_List*des=_tmp5CE;
_tmp5CD=des;goto _LL2A;}case 29U: _LL29: _tmp5CD=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp5C8)->f3;_LL2A: {struct Cyc_List_List*des=_tmp5CD;
# 3245
for(0;des != 0;des=des->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple16*)des->hd)).f2))
return 0;}
return 1;}case 3U: _LL2B: _tmp5CB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_tmp5CC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp5C8)->f2;_LL2C: {enum Cyc_Absyn_Primop p=_tmp5CB;struct Cyc_List_List*es=_tmp5CC;
# 3250
_tmp5CA=es;goto _LL2E;}case 24U: _LL2D: _tmp5CA=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL2E: {struct Cyc_List_List*es=_tmp5CA;
_tmp5C9=es;goto _LL30;}case 31U: _LL2F: _tmp5C9=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp5C8)->f1;_LL30: {struct Cyc_List_List*es=_tmp5C9;
# 3253
for(0;es != 0;es=es->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)es->hd))
return 0;}
return 1;}default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3260
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3264
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp5E8=Cyc_Tcutil_compress(t);void*_stmttmp88=_tmp5E8;void*_tmp5E9=_stmttmp88;struct Cyc_List_List*_tmp5EA;struct Cyc_List_List*_tmp5EB;void*_tmp5EC;void*_tmp5ED;struct Cyc_List_List*_tmp5EF;void*_tmp5EE;switch(*((int*)_tmp5E9)){case 0U: _LL1: _tmp5EE=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E9)->f1;_tmp5EF=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5E9)->f2;_LL2: {void*c=_tmp5EE;struct Cyc_List_List*ts=_tmp5EF;
# 3268
void*_tmp5F0=c;union Cyc_Absyn_AggrInfo _tmp5F1;struct Cyc_List_List*_tmp5F2;struct Cyc_Absyn_Enumdecl*_tmp5F3;switch(*((int*)_tmp5F0)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp5F3=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp5F0)->f2;_LL15: {struct Cyc_Absyn_Enumdecl*edo=_tmp5F3;
# 3273
if(edo == 0 || edo->fields == 0)
return 0;
_tmp5F2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(edo->fields))->v;goto _LL17;}case 16U: _LL16: _tmp5F2=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp5F0)->f1;_LL17: {struct Cyc_List_List*fs=_tmp5F2;
# 3277
{struct Cyc_List_List*_tmp5F4=fs;struct Cyc_List_List*fs2=_tmp5F4;for(0;fs2 != 0;fs2=fs2->tl){
if(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag == 0)
return fs2 == fs;{
struct _tuple13 _tmp5F5=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)fs2->hd)->tag));struct _tuple13 _stmttmp89=_tmp5F5;struct _tuple13 _tmp5F6=_stmttmp89;int _tmp5F8;unsigned _tmp5F7;_LL1D: _tmp5F7=_tmp5F6.f1;_tmp5F8=_tmp5F6.f2;_LL1E: {unsigned i=_tmp5F7;int known=_tmp5F8;
if(known && i == (unsigned)0)
return 1;}}}}
# 3284
return 0;}case 20U: _LL18: _tmp5F1=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp5F0)->f1;_LL19: {union Cyc_Absyn_AggrInfo info=_tmp5F1;
# 3287
struct Cyc_Absyn_Aggrdecl*_tmp5F9=Cyc_Absyn_get_known_aggrdecl(info);struct Cyc_Absyn_Aggrdecl*ad=_tmp5F9;
if(ad->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->exist_vars != 0)return 0;
if((int)ad->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(ad->tvs,ts,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(ad->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp5ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp5E9)->f1).ptr_atts).nullable;_LL4: {void*n=_tmp5ED;
# 3295
return Cyc_Tcutil_force_type2bool(1,n);}case 4U: _LL5: _tmp5EC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp5E9)->f1).elt_type;_LL6: {void*t=_tmp5EC;
return Cyc_Tcutil_zeroable_type(t);}case 6U: _LL7: _tmp5EB=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp5E9)->f1;_LL8: {struct Cyc_List_List*tqs=_tmp5EB;
# 3298
for(0;tqs != 0;tqs=tqs->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple12*)tqs->hd)).f2))return 0;}
return 1;}case 7U: _LL9: _tmp5EA=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp5E9)->f2;_LLA: {struct Cyc_List_List*fs=_tmp5EA;
return Cyc_Tcutil_fields_zeroable(0,0,fs);}default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3305
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3307
struct _RegionHandle _tmp5FA=_new_region("rgn");struct _RegionHandle*rgn=& _tmp5FA;_push_region(rgn);
{struct Cyc_List_List*_tmp5FB=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);struct Cyc_List_List*inst=_tmp5FB;
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,inst,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp5FC=0;_npop_handler(0U);return _tmp5FC;}}{
# 3315
int _tmp5FD=1;_npop_handler(0U);return _tmp5FD;}}
# 3308
;_pop_region();}
# 3319
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp5FE=t;struct Cyc_Absyn_Typedefdecl*_tmp5FF;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5FE)->tag == 8U){_LL1: _tmp5FF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5FE)->f3;_LL2: {struct Cyc_Absyn_Typedefdecl*tdopt=_tmp5FF;
# 3322
if(tdopt != 0){
struct Cyc_Absyn_Tqual _tmp600=tdopt->tq;struct Cyc_Absyn_Tqual tq=_tmp600;
if(((tq.print_const || tq.q_volatile)|| tq.q_restrict)|| tq.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp603=({struct Cyc_String_pa_PrintArg_struct _tmp68F;_tmp68F.tag=0U,({struct _fat_ptr _tmp7DD=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp68F.f1=_tmp7DD;});_tmp68F;});void*_tmp601[1U];_tmp601[0]=& _tmp603;({unsigned _tmp7DF=loc;struct _fat_ptr _tmp7DE=({const char*_tmp602="qualifier within typedef type %s is ignored";_tag_fat(_tmp602,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp7DF,_tmp7DE,_tag_fat(_tmp601,sizeof(void*),1U));});});}
# 3327
goto _LL0;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 3334
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp604=Cyc_Tcutil_compress(t);void*_stmttmp8A=_tmp604;void*_tmp605=_stmttmp8A;struct Cyc_List_List**_tmp606;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp605)->tag == 5U){_LL1: _tmp606=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp605)->f1).attributes;_LL2: {struct Cyc_List_List**fnatts=_tmp606;
# 3337
struct Cyc_List_List*_tmp607=0;struct Cyc_List_List*res_atts=_tmp607;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*fnatts,(void*)atts->hd))
({struct Cyc_List_List*_tmp7E0=({struct Cyc_List_List*_tmp608=_cycalloc(sizeof(*_tmp608));_tmp608->hd=(void*)atts->hd,_tmp608->tl=*fnatts;_tmp608;});*fnatts=_tmp7E0;});}else{
# 3344
res_atts=({struct Cyc_List_List*_tmp609=_cycalloc(sizeof(*_tmp609));_tmp609->hd=(void*)atts->hd,_tmp609->tl=res_atts;_tmp609;});}}
return res_atts;}}else{_LL3: _LL4:
({void*_tmp60A=0U;({struct _fat_ptr _tmp7E1=({const char*_tmp60B="transfer_fn_type_atts";_tag_fat(_tmp60B,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7E1,_tag_fat(_tmp60A,sizeof(void*),0U));});});}_LL0:;}
# 3351
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp60C=Cyc_Tcutil_compress(t);void*_stmttmp8B=_tmp60C;void*_tmp60D=_stmttmp8B;struct Cyc_Absyn_Exp*_tmp60E;struct Cyc_Absyn_PtrInfo _tmp60F;switch(*((int*)_tmp60D)){case 3U: _LL1: _tmp60F=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp60D)->f1;_LL2: {struct Cyc_Absyn_PtrInfo pi=_tmp60F;
return({void*_tmp7E2=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp7E2,(pi.ptr_atts).bounds);});}case 4U: _LL3: _tmp60E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp60D)->f1).num_elts;_LL4: {struct Cyc_Absyn_Exp*e=_tmp60E;
return e;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 3361
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp610=b;struct Cyc_Absyn_Vardecl*_tmp611;struct Cyc_Absyn_Vardecl*_tmp612;struct Cyc_Absyn_Vardecl*_tmp613;struct Cyc_Absyn_Vardecl*_tmp614;switch(*((int*)_tmp610)){case 5U: _LL1: _tmp614=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp610)->f1;_LL2: {struct Cyc_Absyn_Vardecl*x=_tmp614;
_tmp613=x;goto _LL4;}case 4U: _LL3: _tmp613=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp610)->f1;_LL4: {struct Cyc_Absyn_Vardecl*x=_tmp613;
_tmp612=x;goto _LL6;}case 3U: _LL5: _tmp612=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp610)->f1;_LL6: {struct Cyc_Absyn_Vardecl*x=_tmp612;
_tmp611=x;goto _LL8;}case 1U: _LL7: _tmp611=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp610)->f1;_LL8: {struct Cyc_Absyn_Vardecl*x=_tmp611;
# 3367
if(!x->escapes)
return x;
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3375
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp615=0;struct Cyc_List_List*res=_tmp615;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)res=({struct Cyc_List_List*_tmp616=_cycalloc(sizeof(*_tmp616));_tmp616->hd=*((void**)_check_null((void**)x->hd)),_tmp616->tl=res;_tmp616;});}}
return res;}
# 3382
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp617=Cyc_Tcutil_compress(t);void*_stmttmp8C=_tmp617;void*_tmp618=_stmttmp8C;unsigned _tmp61D;void*_tmp61C;struct Cyc_Absyn_Exp*_tmp61B;struct Cyc_Absyn_Tqual _tmp61A;void*_tmp619;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp618)->tag == 4U){_LL1: _tmp619=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp618)->f1).elt_type;_tmp61A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp618)->f1).tq;_tmp61B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp618)->f1).num_elts;_tmp61C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp618)->f1).zero_term;_tmp61D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp618)->f1).zt_loc;_LL2: {void*et=_tmp619;struct Cyc_Absyn_Tqual tq=_tmp61A;struct Cyc_Absyn_Exp*eopt=_tmp61B;void*zt=_tmp61C;unsigned ztl=_tmp61D;
# 3385
void*b;
if(eopt == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 3389
if(convert_tag){
if(eopt->topt == 0)
({void*_tmp61E=0U;({struct _fat_ptr _tmp7E3=({const char*_tmp61F="cannot convert tag without type!";_tag_fat(_tmp61F,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp7E3,_tag_fat(_tmp61E,sizeof(void*),0U));});});{
void*_tmp620=Cyc_Tcutil_compress((void*)_check_null(eopt->topt));void*_stmttmp8D=_tmp620;void*_tmp621=_stmttmp8D;void*_tmp622;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp621)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp621)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp621)->f2 != 0){_LL6: _tmp622=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp621)->f2)->hd;_LL7: {void*t=_tmp622;
# 3395
b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(t,0U));
goto _LL5;}}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 3398
 b=Cyc_Tcutil_is_const_exp(eopt)?Cyc_Absyn_thin_bounds_exp(eopt): Cyc_Absyn_fat_bound_type;}_LL5:;}}else{
# 3402
b=Cyc_Absyn_thin_bounds_exp(eopt);}}
# 3404
return Cyc_Absyn_atb_type(et,rgn,tq,b,zt);}}else{_LL3: _LL4:
 return t;}_LL0:;}
