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
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y);
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
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2);struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 88
int Cyc_fflush(struct Cyc___cycFILE*);
# 100
int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 157 "cycboot.h"
int Cyc_printf(struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 38 "string.h"
unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
int Cyc_strptrcmp(struct _fat_ptr*s1,struct _fat_ptr*s2);
# 62
struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;
# 37 "position.h"
struct _fat_ptr Cyc_Position_string_of_segment(unsigned);struct Cyc_Position_Error;
# 47
extern int Cyc_Position_num_errors;
extern int Cyc_Position_max_errors;struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};
# 95 "absyn.h"
extern union Cyc_Absyn_Nmspace Cyc_Absyn_Loc_n;struct _tuple1{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 158
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 190
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};
# 212
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
# 214
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple1*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};
# 301
union Cyc_Absyn_DatatypeInfo Cyc_Absyn_UnknownDatatype(struct Cyc_Absyn_UnknownDatatypeInfo);struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple1*datatype_name;struct _tuple1*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple2{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple2 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple3{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple3 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};
# 321
union Cyc_Absyn_AggrInfo Cyc_Absyn_UnknownAggr(enum Cyc_Absyn_AggrKind,struct _tuple1*,struct Cyc_Core_Opt*);struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple1*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 424 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple4{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple4 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple5{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple6 val;};struct _tuple7{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple7 val;};struct _tuple8{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple8 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 513
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Bitarshift =18U,Cyc_Absyn_Numelts =19U};
# 520
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 538
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple9{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple9*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple10{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple10 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple10 f2;struct _tuple10 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple10 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple1*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple1*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple1*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple1*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple1*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple1*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple1*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 886 "absyn.h"
int Cyc_Absyn_qvar_cmp(struct _tuple1*,struct _tuple1*);
# 888
int Cyc_Absyn_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);
# 895
struct Cyc_Absyn_Tqual Cyc_Absyn_empty_tqual(unsigned);
# 900
void*Cyc_Absyn_compress_kb(void*);
# 906
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
extern void*Cyc_Absyn_void_type;void*Cyc_Absyn_var_type(struct Cyc_Absyn_Tvar*);void*Cyc_Absyn_access_eff(void*);void*Cyc_Absyn_join_eff(struct Cyc_List_List*);void*Cyc_Absyn_regionsof_eff(void*);void*Cyc_Absyn_enum_type(struct _tuple1*n,struct Cyc_Absyn_Enumdecl*d);
# 954
extern void*Cyc_Absyn_fat_bound_type;
# 956
void*Cyc_Absyn_thin_bounds_exp(struct Cyc_Absyn_Exp*);
# 958
void*Cyc_Absyn_bounds_one();
# 960
void*Cyc_Absyn_pointer_type(struct Cyc_Absyn_PtrInfo);
# 965
void*Cyc_Absyn_atb_type(void*t,void*rgn,struct Cyc_Absyn_Tqual tq,void*b,void*zero_term);
# 985
void*Cyc_Absyn_datatype_type(union Cyc_Absyn_DatatypeInfo,struct Cyc_List_List*args);
# 987
void*Cyc_Absyn_aggr_type(union Cyc_Absyn_AggrInfo,struct Cyc_List_List*args);
# 990
struct Cyc_Absyn_Exp*Cyc_Absyn_new_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_New_exp(struct Cyc_Absyn_Exp*rgn_handle,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_copy_exp(struct Cyc_Absyn_Exp*);
struct Cyc_Absyn_Exp*Cyc_Absyn_const_exp(union Cyc_Absyn_Cnst,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_null_exp(unsigned);
# 1000
struct Cyc_Absyn_Exp*Cyc_Absyn_uint_exp(unsigned,unsigned);
# 1007
struct Cyc_Absyn_Exp*Cyc_Absyn_varb_exp(void*,unsigned);
# 1009
struct Cyc_Absyn_Exp*Cyc_Absyn_pragma_exp(struct _fat_ptr,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_primop_exp(enum Cyc_Absyn_Primop,struct Cyc_List_List*,unsigned);
# 1013
struct Cyc_Absyn_Exp*Cyc_Absyn_swap_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1023
struct Cyc_Absyn_Exp*Cyc_Absyn_assignop_exp(struct Cyc_Absyn_Exp*,struct Cyc_Core_Opt*,struct Cyc_Absyn_Exp*,unsigned);
# 1025
struct Cyc_Absyn_Exp*Cyc_Absyn_increment_exp(struct Cyc_Absyn_Exp*,enum Cyc_Absyn_Incrementor,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_conditional_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_and_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_or_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_seq_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
# 1032
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
# 1044
struct Cyc_Absyn_Exp*Cyc_Absyn_subscript_exp(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_tuple_exp(struct Cyc_List_List*,unsigned);
# 1049
struct Cyc_Absyn_Exp*Cyc_Absyn_valueof_exp(void*,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_asm_exp(int volatile_kw,struct _fat_ptr body,unsigned);
struct Cyc_Absyn_Exp*Cyc_Absyn_extension_exp(struct Cyc_Absyn_Exp*,unsigned);
# 1088
struct Cyc_Absyn_Decl*Cyc_Absyn_alias_decl(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Vardecl*,struct Cyc_Absyn_Exp*,unsigned);
struct Cyc_Absyn_Vardecl*Cyc_Absyn_new_vardecl(unsigned varloc,struct _tuple1*,void*,struct Cyc_Absyn_Exp*init);
# 1129
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_field(struct Cyc_List_List*,struct _fat_ptr*);
# 1131
struct Cyc_Absyn_Aggrfield*Cyc_Absyn_lookup_decl_field(struct Cyc_Absyn_Aggrdecl*,struct _fat_ptr*);struct _tuple11{struct Cyc_Absyn_Tqual f1;void*f2;};
# 1133
struct _tuple11*Cyc_Absyn_lookup_tuple_field(struct Cyc_List_List*,int);
# 1141
int Cyc_Absyn_fntype_att(void*);
# 1143
int Cyc_Absyn_equal_att(void*,void*);
# 1145
int Cyc_Absyn_attribute_cmp(void*,void*);
# 1151
struct Cyc_Absyn_Aggrdecl*Cyc_Absyn_get_known_aggrdecl(union Cyc_Absyn_AggrInfo);struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 53 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 55
extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 62
struct _fat_ptr Cyc_Absynpp_typ2string(void*);
# 64
struct _fat_ptr Cyc_Absynpp_kind2string(struct Cyc_Absyn_Kind*);
# 67
struct _fat_ptr Cyc_Absynpp_exp2string(struct Cyc_Absyn_Exp*);
# 74
struct _fat_ptr Cyc_Absynpp_tvar2string(struct Cyc_Absyn_Tvar*);
# 26 "warn.h"
void Cyc_Warn_vwarn(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 32
void Cyc_Warn_verr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap);
# 37
void*Cyc_Warn_vimpos(struct _fat_ptr fmt,struct _fat_ptr ap);struct _tuple12{unsigned f1;int f2;};
# 28 "evexp.h"
struct _tuple12 Cyc_Evexp_eval_const_uint_exp(struct Cyc_Absyn_Exp*e);
# 41 "evexp.h"
int Cyc_Evexp_same_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
int Cyc_Evexp_lte_const_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);
# 45
int Cyc_Evexp_const_exp_cmp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;};
# 89 "tcenv.h"
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 99
struct Cyc_List_List*Cyc_Tcenv_lookup_type_vars(struct Cyc_Tcenv_Tenv*);
struct Cyc_Core_Opt*Cyc_Tcenv_lookup_opt_type_vars(struct Cyc_Tcenv_Tenv*);
# 146
int Cyc_Tcenv_region_outlives(struct Cyc_Tcenv_Tenv*,void*r1,void*r2);
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
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,enum Cyc_Absyn_Coercion);
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*,int*alias_coercion);
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*,void*);
# 114
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*);
# 117
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*,unsigned,void*,void*);
# 119
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*,unsigned,void*,void*);
# 121
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);
# 127
extern int Cyc_Tcutil_warn_alias_coerce;
# 130
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
int Cyc_Tcutil_unify_kindbound(void*,void*);
# 181
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*,void*,struct Cyc_Absyn_Exp*);
# 189
int Cyc_Tcutil_unify(void*,void*);
# 191
int Cyc_Tcutil_typecmp(void*,void*);
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*);
# 194
void*Cyc_Tcutil_substitute(struct Cyc_List_List*,void*);
# 196
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*,struct Cyc_List_List*,void*);
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 202
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_Absyn_Exp*);
# 205
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2);
# 209
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e);
# 233
void Cyc_Tcutil_check_bound(unsigned,unsigned i,void*,int do_warn);
# 246
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique);
# 249
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique);
# 254
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e);
# 259
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t);struct _tuple13{int f1;void*f2;};
# 263
struct _tuple13 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*,struct Cyc_Absyn_Exp*e);
# 266
void*Cyc_Tcutil_normalize_effect(void*e);
# 269
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k);
# 271
int Cyc_Tcutil_new_tvar_id();
# 273
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*);
# 276
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*);
# 281
int Cyc_Tcutil_same_atts(struct Cyc_List_List*,struct Cyc_List_List*);
# 284
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e);
# 291
int Cyc_Tcutil_extract_const_from_typedef(unsigned,int declared_const,void*);
# 309
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag);
# 312
int Cyc_Tcutil_zeroable_type(void*);
# 316
int Cyc_Tcutil_force_type2bool(int desired,void*);
# 324
int Cyc_Tcutil_admits_zero(void*);
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*,struct Cyc_Relations_Reln*);
# 328
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 41 "relations-ap.h"
union Cyc_Relations_RelnOp Cyc_Relations_RParam(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RParamNumelts(unsigned);union Cyc_Relations_RelnOp Cyc_Relations_RReturn();
# 50
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 70
struct Cyc_Relations_Reln*Cyc_Relations_negate(struct _RegionHandle*,struct Cyc_Relations_Reln*);
# 84
struct Cyc_List_List*Cyc_Relations_exp2relns(struct _RegionHandle*r,struct Cyc_Absyn_Exp*e);
# 110
struct Cyc_List_List*Cyc_Relations_copy_relns(struct _RegionHandle*,struct Cyc_List_List*);
# 129
int Cyc_Relations_consistent_relations(struct Cyc_List_List*rlns);
# 37 "tcutil.cyc"
int Cyc_Tcutil_is_void_type(void*t){
void*_tmp0=Cyc_Tcutil_compress(t);void*_tmp1=_tmp0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->tag == 0U){if(((struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1)->f1)->tag == 0U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 43
int Cyc_Tcutil_is_array_type(void*t){
void*_tmp2=Cyc_Tcutil_compress(t);void*_tmp3=_tmp2;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3)->tag == 4U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 49
int Cyc_Tcutil_is_heap_rgn_type(void*t){
void*_tmp4=Cyc_Tcutil_compress(t);void*_tmp5=_tmp4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->tag == 0U){if(((struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5)->f1)->tag == 5U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 55
int Cyc_Tcutil_is_pointer_type(void*t){
void*_tmp6=Cyc_Tcutil_compress(t);void*_tmp7=_tmp6;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp7)->tag == 3U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 62
int Cyc_Tcutil_is_char_type(void*t){
void*_tmp8=Cyc_Tcutil_compress(t);void*_tmp9=_tmp8;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp9)->f1)->f2 == Cyc_Absyn_Char_sz){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 69
int Cyc_Tcutil_is_any_int_type(void*t){
void*_tmpA=Cyc_Tcutil_compress(t);void*_tmpB=_tmpA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpB)->f1)->tag == 1U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 76
int Cyc_Tcutil_is_any_float_type(void*t){
void*_tmpC=Cyc_Tcutil_compress(t);void*_tmpD=_tmpC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpD)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 83
int Cyc_Tcutil_is_integral_type(void*t){
void*_tmpE=Cyc_Tcutil_compress(t);void*_tmpF=_tmpE;void*_tmp11;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->tag == 0U){_LL1: _tmp11=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmpF)->f1;_LL2: {
# 86
void*_tmp10=_tmp11;switch(*((int*)_tmp10)){case 1U: _LL6: _LL7:
 goto _LL9;case 4U: _LL8: _LL9:
 goto _LLB;case 15U: _LLA: _LLB:
 goto _LLD;case 16U: _LLC: _LLD:
 return 1;default: _LLE: _LLF:
 return 0;}_LL5:;}}else{_LL3: _LL4:
# 93
 return 0;}_LL0:;}
# 96
int Cyc_Tcutil_is_signed_type(void*t){
void*_tmp12=Cyc_Tcutil_compress(t);void*_tmp13=_tmp12;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)){case 1U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp13)->f1)->f1 == Cyc_Absyn_Signed){_LL1: _LL2:
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
void*_tmp14=Cyc_Tcutil_compress(t);void*_tmp15=_tmp14;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp15)->tag == 5U){_LL1: _LL2:
 return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 115
int Cyc_Tcutil_is_boxed(void*t){
return(int)(Cyc_Tcutil_type_kind(t))->kind == (int)Cyc_Absyn_BoxKind;}
# 123
int Cyc_Tcutil_is_integral(struct Cyc_Absyn_Exp*e){
void*_tmp16=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp17=_tmp16;void*_tmp18;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp17)->tag == 1U){_LL1: _LL2:
 return Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type);}else{_LL3: _tmp18=_tmp17;_LL4:
 return Cyc_Tcutil_is_integral_type(_tmp18);}_LL0:;}
# 131
int Cyc_Tcutil_is_numeric(struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_is_integral(e))
return 1;{
void*_tmp19=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp1A=_tmp19;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->tag == 0U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1A)->f1)->tag == 2U){_LL1: _LL2:
 return 1;}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;};}
# 141
int Cyc_Tcutil_is_zeroterm_pointer_type(void*t){
void*_tmp1B=Cyc_Tcutil_compress(t);void*_tmp1C=_tmp1B;void*_tmp1D;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->tag == 3U){_LL1: _tmp1D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1C)->f1).ptr_atts).zero_term;_LL2:
# 144
 return Cyc_Tcutil_force_type2bool(0,_tmp1D);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 150
int Cyc_Tcutil_is_nullable_pointer_type(void*t){
void*_tmp1E=Cyc_Tcutil_compress(t);void*_tmp1F=_tmp1E;void*_tmp20;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->tag == 3U){_LL1: _tmp20=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1F)->f1).ptr_atts).nullable;_LL2:
# 153
 return Cyc_Tcutil_force_type2bool(0,_tmp20);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 159
int Cyc_Tcutil_is_fat_ptr(void*t){
void*_tmp21=Cyc_Tcutil_compress(t);void*_tmp22=_tmp21;void*_tmp23;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->tag == 3U){_LL1: _tmp23=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp22)->f1).ptr_atts).bounds;_LL2:
# 162
 return Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp23);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 169
int Cyc_Tcutil_is_fat_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp24=Cyc_Tcutil_compress(t);void*_tmp25=_tmp24;void*_tmp27;void*_tmp26;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->tag == 3U){_LL1: _tmp27=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).elt_type;_tmp26=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp25)->f1).ptr_atts).bounds;_LL2:
# 172
 if(Cyc_Tcutil_unify(_tmp26,Cyc_Absyn_fat_bound_type)){
*elt_type_dest=_tmp27;
return 1;}else{
# 176
return 0;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 183
int Cyc_Tcutil_is_zero_pointer_type_elt(void*t,void**elt_type_dest){
void*_tmp28=Cyc_Tcutil_compress(t);void*_tmp29=_tmp28;void*_tmp2B;void*_tmp2A;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->tag == 3U){_LL1: _tmp2B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).elt_type;_tmp2A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp29)->f1).ptr_atts).zero_term;_LL2:
# 186
*elt_type_dest=_tmp2B;
return Cyc_Absyn_type2bool(0,_tmp2A);}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 195
int Cyc_Tcutil_is_zero_ptr_type(void*t,void**ptr_type,int*is_fat,void**elt_type){
# 197
void*_tmp2C=Cyc_Tcutil_compress(t);void*_tmp2D=_tmp2C;void*_tmp36;struct Cyc_Absyn_Tqual _tmp35;struct Cyc_Absyn_Exp*_tmp34;void*_tmp33;void*_tmp32;void*_tmp31;void*_tmp30;switch(*((int*)_tmp2D)){case 3U: _LL1: _tmp32=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp31=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).bounds;_tmp30=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2D)->f1).ptr_atts).zero_term;_LL2:
# 199
 if(Cyc_Absyn_type2bool(0,_tmp30)){
*ptr_type=t;
*elt_type=_tmp32;
{void*_tmp2E=Cyc_Tcutil_compress(_tmp31);void*_tmp2F=_tmp2E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2F)->f1)->tag == 14U){_LL8: _LL9:
*is_fat=1;goto _LL7;}else{goto _LLA;}}else{_LLA: _LLB:
*is_fat=0;goto _LL7;}_LL7:;}
# 206
return 1;}else{
return 0;}case 4U: _LL3: _tmp36=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).elt_type;_tmp35=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).tq;_tmp34=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).num_elts;_tmp33=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2D)->f1).zero_term;_LL4:
# 209
 if(Cyc_Absyn_type2bool(0,_tmp33)){
*elt_type=_tmp36;
*is_fat=0;
({void*_tmp775=Cyc_Tcutil_promote_array(t,Cyc_Absyn_heap_rgn_type,0);*ptr_type=_tmp775;});
return 1;}else{
return 0;}default: _LL5: _LL6:
 return 0;}_LL0:;}
# 222
int Cyc_Tcutil_is_fat_pointer_type(void*t){
void*ignore=Cyc_Absyn_void_type;
return Cyc_Tcutil_is_fat_pointer_type_elt(t,& ignore);}
# 228
int Cyc_Tcutil_is_bound_one(void*b){
struct Cyc_Absyn_Exp*_tmp37=({void*_tmp776=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp776,b);});
if(_tmp37 == 0)return 0;{
struct Cyc_Absyn_Exp*_tmp38=_tmp37;
struct _tuple12 _tmp39=Cyc_Evexp_eval_const_uint_exp(_tmp38);struct _tuple12 _tmp3A=_tmp39;unsigned _tmp3C;int _tmp3B;_LL1: _tmp3C=_tmp3A.f1;_tmp3B=_tmp3A.f2;_LL2:;
return _tmp3B && _tmp3C == (unsigned)1;};}
# 237
int Cyc_Tcutil_is_bits_only_type(void*t){
void*_tmp3D=Cyc_Tcutil_compress(t);void*_tmp3E=_tmp3D;struct Cyc_List_List*_tmp4A;struct Cyc_List_List*_tmp49;void*_tmp48;void*_tmp47;void*_tmp46;struct Cyc_List_List*_tmp45;switch(*((int*)_tmp3E)){case 0U: _LL1: _tmp46=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f1;_tmp45=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3E)->f2;_LL2: {
# 240
void*_tmp3F=_tmp46;struct Cyc_Absyn_Aggrdecl*_tmp44;switch(*((int*)_tmp3F)){case 0U: _LLC: _LLD:
 goto _LLF;case 1U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return 1;case 15U: _LL12: _LL13:
 goto _LL15;case 16U: _LL14: _LL15:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).UnknownAggr).tag == 1){_LL16: _LL17:
 return 0;}else{_LL18: _tmp44=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp3F)->f1).KnownAggr).val;_LL19:
# 248
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
# 255
;_pop_region(rgn);};};}default: _LL1A: _LL1B:
# 259
 return 0;}_LLB:;}case 4U: _LL3: _tmp48=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).elt_type;_tmp47=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3E)->f1).zero_term;_LL4:
# 264
 return !Cyc_Absyn_type2bool(0,_tmp47)&& Cyc_Tcutil_is_bits_only_type(_tmp48);case 6U: _LL5: _tmp49=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp3E)->f1;_LL6:
# 266
 for(0;_tmp49 != 0;_tmp49=_tmp49->tl){
if(!Cyc_Tcutil_is_bits_only_type((*((struct _tuple11*)_tmp49->hd)).f2))return 0;}
return 1;case 7U: _LL7: _tmp4A=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp3E)->f2;_LL8:
# 270
 for(0;_tmp4A != 0;_tmp4A=_tmp4A->tl){
if(!Cyc_Tcutil_is_bits_only_type(((struct Cyc_Absyn_Aggrfield*)_tmp4A->hd)->type))return 0;}
return 1;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 279
int Cyc_Tcutil_is_noreturn_fn_type(void*t){
void*_tmp4B=Cyc_Tcutil_compress(t);void*_tmp4C=_tmp4B;struct Cyc_List_List*_tmp50;void*_tmp4F;switch(*((int*)_tmp4C)){case 3U: _LL1: _tmp4F=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4C)->f1).elt_type;_LL2:
 return Cyc_Tcutil_is_noreturn_fn_type(_tmp4F);case 5U: _LL3: _tmp50=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp4C)->f1).attributes;_LL4:
# 283
 for(0;_tmp50 != 0;_tmp50=_tmp50->tl){
void*_tmp4D=(void*)_tmp50->hd;void*_tmp4E=_tmp4D;if(((struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct*)_tmp4E)->tag == 4U){_LL8: _LL9:
 return 1;}else{_LLA: _LLB:
 continue;}_LL7:;}
# 288
return 0;default: _LL5: _LL6:
 return 0;}_LL0:;}char Cyc_Tcutil_Unify[6U]="Unify";struct Cyc_Tcutil_Unify_exn_struct{char*tag;};
# 294
struct Cyc_Tcutil_Unify_exn_struct Cyc_Tcutil_Unify_val={Cyc_Tcutil_Unify};
# 298
int Cyc_Tcutil_warn_region_coerce=0;
# 301
static void*Cyc_Tcutil_t1_failure=0;
static int Cyc_Tcutil_tq1_const=0;
static void*Cyc_Tcutil_t2_failure=0;
static int Cyc_Tcutil_tq2_const=0;
# 306
struct _fat_ptr Cyc_Tcutil_failure_reason={(void*)0,(void*)0,(void*)(0 + 0)};
# 310
void Cyc_Tcutil_explain_failure(){
if(Cyc_Position_num_errors >= Cyc_Position_max_errors)return;
Cyc_fflush(Cyc_stderr);
# 315
if(({struct _fat_ptr _tmp777=({const char*_tmp51="(qualifiers don't match)";_tag_fat(_tmp51,sizeof(char),25U);});Cyc_strcmp(_tmp777,(struct _fat_ptr)Cyc_Tcutil_failure_reason);})== 0){
({struct Cyc_String_pa_PrintArg_struct _tmp54=({struct Cyc_String_pa_PrintArg_struct _tmp71A;_tmp71A.tag=0U,_tmp71A.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcutil_failure_reason);_tmp71A;});void*_tmp52[1U];_tmp52[0]=& _tmp54;({struct Cyc___cycFILE*_tmp779=Cyc_stderr;struct _fat_ptr _tmp778=({const char*_tmp53="  %s\n";_tag_fat(_tmp53,sizeof(char),6U);});Cyc_fprintf(_tmp779,_tmp778,_tag_fat(_tmp52,sizeof(void*),1U));});});
return;}
# 320
if(({struct _fat_ptr _tmp77A=({const char*_tmp55="(function effects do not match)";_tag_fat(_tmp55,sizeof(char),32U);});Cyc_strcmp(_tmp77A,(struct _fat_ptr)Cyc_Tcutil_failure_reason);})== 0){
struct Cyc_Absynpp_Params _tmp56=Cyc_Absynpp_tc_params_r;
_tmp56.print_all_effects=1;
Cyc_Absynpp_set_params(& _tmp56);}{
# 325
void*_tmp57=Cyc_Tcutil_t1_failure;
void*_tmp58=Cyc_Tcutil_t2_failure;
struct _fat_ptr s1=(unsigned)_tmp57?Cyc_Absynpp_typ2string(_tmp57):({const char*_tmp73="<?>";_tag_fat(_tmp73,sizeof(char),4U);});
struct _fat_ptr s2=(unsigned)_tmp58?Cyc_Absynpp_typ2string(_tmp58):({const char*_tmp72="<?>";_tag_fat(_tmp72,sizeof(char),4U);});
int pos=2;
({struct Cyc_String_pa_PrintArg_struct _tmp5B=({struct Cyc_String_pa_PrintArg_struct _tmp71B;_tmp71B.tag=0U,_tmp71B.f1=(struct _fat_ptr)((struct _fat_ptr)s1);_tmp71B;});void*_tmp59[1U];_tmp59[0]=& _tmp5B;({struct Cyc___cycFILE*_tmp77C=Cyc_stderr;struct _fat_ptr _tmp77B=({const char*_tmp5A="  %s";_tag_fat(_tmp5A,sizeof(char),5U);});Cyc_fprintf(_tmp77C,_tmp77B,_tag_fat(_tmp59,sizeof(void*),1U));});});
pos +=_get_fat_size(s1,sizeof(char));
if(pos + 5 >= 80){
({void*_tmp5C=0U;({struct Cyc___cycFILE*_tmp77E=Cyc_stderr;struct _fat_ptr _tmp77D=({const char*_tmp5D="\n\t";_tag_fat(_tmp5D,sizeof(char),3U);});Cyc_fprintf(_tmp77E,_tmp77D,_tag_fat(_tmp5C,sizeof(void*),0U));});});
pos=8;}else{
# 336
({void*_tmp5E=0U;({struct Cyc___cycFILE*_tmp780=Cyc_stderr;struct _fat_ptr _tmp77F=({const char*_tmp5F=" ";_tag_fat(_tmp5F,sizeof(char),2U);});Cyc_fprintf(_tmp780,_tmp77F,_tag_fat(_tmp5E,sizeof(void*),0U));});});
++ pos;}
# 339
({void*_tmp60=0U;({struct Cyc___cycFILE*_tmp782=Cyc_stderr;struct _fat_ptr _tmp781=({const char*_tmp61="and ";_tag_fat(_tmp61,sizeof(char),5U);});Cyc_fprintf(_tmp782,_tmp781,_tag_fat(_tmp60,sizeof(void*),0U));});});
pos +=4;
if((unsigned)pos + _get_fat_size(s2,sizeof(char))>= (unsigned)80){
({void*_tmp62=0U;({struct Cyc___cycFILE*_tmp784=Cyc_stderr;struct _fat_ptr _tmp783=({const char*_tmp63="\n\t";_tag_fat(_tmp63,sizeof(char),3U);});Cyc_fprintf(_tmp784,_tmp783,_tag_fat(_tmp62,sizeof(void*),0U));});});
pos=8;}
# 345
({struct Cyc_String_pa_PrintArg_struct _tmp66=({struct Cyc_String_pa_PrintArg_struct _tmp71C;_tmp71C.tag=0U,_tmp71C.f1=(struct _fat_ptr)((struct _fat_ptr)s2);_tmp71C;});void*_tmp64[1U];_tmp64[0]=& _tmp66;({struct Cyc___cycFILE*_tmp786=Cyc_stderr;struct _fat_ptr _tmp785=({const char*_tmp65="%s ";_tag_fat(_tmp65,sizeof(char),4U);});Cyc_fprintf(_tmp786,_tmp785,_tag_fat(_tmp64,sizeof(void*),1U));});});
pos +=_get_fat_size(s2,sizeof(char))+ (unsigned)1;
if(pos + 17 >= 80){
({void*_tmp67=0U;({struct Cyc___cycFILE*_tmp788=Cyc_stderr;struct _fat_ptr _tmp787=({const char*_tmp68="\n\t";_tag_fat(_tmp68,sizeof(char),3U);});Cyc_fprintf(_tmp788,_tmp787,_tag_fat(_tmp67,sizeof(void*),0U));});});
pos=8;}
# 351
({void*_tmp69=0U;({struct Cyc___cycFILE*_tmp78A=Cyc_stderr;struct _fat_ptr _tmp789=({const char*_tmp6A="are not compatible. ";_tag_fat(_tmp6A,sizeof(char),21U);});Cyc_fprintf(_tmp78A,_tmp789,_tag_fat(_tmp69,sizeof(void*),0U));});});
pos +=17;
if(({char*_tmp78B=(char*)Cyc_Tcutil_failure_reason.curr;_tmp78B != (char*)(_tag_fat(0,0,0)).curr;})){
if(({unsigned long _tmp78C=(unsigned long)pos;_tmp78C + Cyc_strlen((struct _fat_ptr)Cyc_Tcutil_failure_reason);})>= (unsigned long)80)
({void*_tmp6B=0U;({struct Cyc___cycFILE*_tmp78E=Cyc_stderr;struct _fat_ptr _tmp78D=({const char*_tmp6C="\n\t";_tag_fat(_tmp6C,sizeof(char),3U);});Cyc_fprintf(_tmp78E,_tmp78D,_tag_fat(_tmp6B,sizeof(void*),0U));});});
# 357
({struct Cyc_String_pa_PrintArg_struct _tmp6F=({struct Cyc_String_pa_PrintArg_struct _tmp71D;_tmp71D.tag=0U,_tmp71D.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_Tcutil_failure_reason);_tmp71D;});void*_tmp6D[1U];_tmp6D[0]=& _tmp6F;({struct Cyc___cycFILE*_tmp790=Cyc_stderr;struct _fat_ptr _tmp78F=({const char*_tmp6E="%s";_tag_fat(_tmp6E,sizeof(char),3U);});Cyc_fprintf(_tmp790,_tmp78F,_tag_fat(_tmp6D,sizeof(void*),1U));});});}
# 359
({void*_tmp70=0U;({struct Cyc___cycFILE*_tmp792=Cyc_stderr;struct _fat_ptr _tmp791=({const char*_tmp71="\n";_tag_fat(_tmp71,sizeof(char),2U);});Cyc_fprintf(_tmp792,_tmp791,_tag_fat(_tmp70,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);};}
# 363
void Cyc_Tcutil_terr(unsigned loc,struct _fat_ptr fmt,struct _fat_ptr ap){
# 365
Cyc_Warn_verr(loc,fmt,ap);}
# 367
void*Cyc_Tcutil_impos(struct _fat_ptr fmt,struct _fat_ptr ap){
# 369
Cyc_Warn_vimpos(fmt,ap);}
# 371
void Cyc_Tcutil_warn(unsigned sg,struct _fat_ptr fmt,struct _fat_ptr ap){
# 373
Cyc_Warn_vwarn(sg,fmt,ap);}
# 377
int Cyc_Tcutil_fast_tvar_cmp(struct Cyc_Absyn_Tvar*tv1,struct Cyc_Absyn_Tvar*tv2){
return tv1->identity - tv2->identity;}
# 382
void*Cyc_Tcutil_compress(void*t){
void*_tmp74=t;void*_tmp7D;struct Cyc_Absyn_Exp*_tmp7C;struct Cyc_Absyn_Exp*_tmp7B;void**_tmp7A;void**_tmp79;switch(*((int*)_tmp74)){case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74)->f2 == 0){_LL1: _LL2:
 goto _LL4;}else{_LL7: _tmp79=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp74)->f2;_LL8: {
# 393
void*ta=(void*)_check_null(*_tmp79);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp79=t2;
return t2;}}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74)->f4 == 0){_LL3: _LL4:
# 385
 return t;}else{_LL5: _tmp7A=(void**)&((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp74)->f4;_LL6: {
# 387
void*ta=(void*)_check_null(*_tmp7A);
void*t2=Cyc_Tcutil_compress(ta);
if(t2 != ta)
*_tmp7A=t2;
return t2;}}case 9U: _LL9: _tmp7B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp74)->f1;_LLA:
# 399
 Cyc_Evexp_eval_const_uint_exp(_tmp7B);{
void*_tmp75=_tmp7B->r;void*_tmp76=_tmp75;void*_tmp77;if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp76)->tag == 39U){_LL12: _tmp77=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp76)->f1;_LL13:
 return Cyc_Tcutil_compress(_tmp77);}else{_LL14: _LL15:
 return t;}_LL11:;};case 11U: _LLB: _tmp7C=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp74)->f1;_LLC: {
# 405
void*_tmp78=_tmp7C->topt;
if(_tmp78 != 0)return _tmp78;else{
return t;}}case 10U: if(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74)->f2 != 0){_LLD: _tmp7D=*((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp74)->f2;_LLE:
 return Cyc_Tcutil_compress(_tmp7D);}else{goto _LLF;}default: _LLF: _LL10:
 return t;}_LL0:;}
# 417
void*Cyc_Tcutil_copy_type(void*t);
static struct Cyc_List_List*Cyc_Tcutil_copy_types(struct Cyc_List_List*ts){
return((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,ts);}
# 421
static void*Cyc_Tcutil_copy_kindbound(void*kb){
void*_tmp7E=Cyc_Absyn_compress_kb(kb);void*_tmp7F=_tmp7E;struct Cyc_Absyn_Kind*_tmp82;switch(*((int*)_tmp7F)){case 1U: _LL1: _LL2:
 return(void*)({struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*_tmp80=_cycalloc(sizeof(*_tmp80));_tmp80->tag=1U,_tmp80->f1=0;_tmp80;});case 2U: _LL3: _tmp82=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp7F)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp81=_cycalloc(sizeof(*_tmp81));_tmp81->tag=2U,_tmp81->f1=0,_tmp81->f2=_tmp82;_tmp81;});default: _LL5: _LL6:
 return kb;}_LL0:;}
# 428
static struct Cyc_Absyn_Tvar*Cyc_Tcutil_copy_tvar(struct Cyc_Absyn_Tvar*tv){
# 430
return({struct Cyc_Absyn_Tvar*_tmp83=_cycalloc(sizeof(*_tmp83));_tmp83->name=tv->name,_tmp83->identity=- 1,({void*_tmp793=Cyc_Tcutil_copy_kindbound(tv->kind);_tmp83->kind=_tmp793;});_tmp83;});}
# 432
static struct _tuple9*Cyc_Tcutil_copy_arg(struct _tuple9*arg){
# 434
struct _tuple9*_tmp84=arg;struct _fat_ptr*_tmp88;struct Cyc_Absyn_Tqual _tmp87;void*_tmp86;_LL1: _tmp88=_tmp84->f1;_tmp87=_tmp84->f2;_tmp86=_tmp84->f3;_LL2:;
return({struct _tuple9*_tmp85=_cycalloc(sizeof(*_tmp85));_tmp85->f1=_tmp88,_tmp85->f2=_tmp87,({void*_tmp794=Cyc_Tcutil_copy_type(_tmp86);_tmp85->f3=_tmp794;});_tmp85;});}
# 437
static struct _tuple11*Cyc_Tcutil_copy_tqt(struct _tuple11*arg){
return({struct _tuple11*_tmp89=_cycalloc(sizeof(*_tmp89));_tmp89->f1=(*arg).f1,({void*_tmp795=Cyc_Tcutil_copy_type((*arg).f2);_tmp89->f2=_tmp795;});_tmp89;});}
# 440
static struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp_opt(int preserve_types,struct Cyc_Absyn_Exp*e){
if(e == 0)return 0;else{
return Cyc_Tcutil_deep_copy_exp(preserve_types,e);}}
# 444
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_copy_field(struct Cyc_Absyn_Aggrfield*f){
return({struct Cyc_Absyn_Aggrfield*_tmp8A=_cycalloc(sizeof(*_tmp8A));_tmp8A->name=f->name,_tmp8A->tq=f->tq,({void*_tmp797=Cyc_Tcutil_copy_type(f->type);_tmp8A->type=_tmp797;}),_tmp8A->width=f->width,_tmp8A->attributes=f->attributes,({
struct Cyc_Absyn_Exp*_tmp796=Cyc_Tcutil_deep_copy_exp_opt(1,f->requires_clause);_tmp8A->requires_clause=_tmp796;});_tmp8A;});}struct _tuple14{void*f1;void*f2;};
# 448
static struct _tuple14*Cyc_Tcutil_copy_rgncmp(struct _tuple14*x){
struct _tuple14*_tmp8B=x;void*_tmp8E;void*_tmp8D;_LL1: _tmp8E=_tmp8B->f1;_tmp8D=_tmp8B->f2;_LL2:;
return({struct _tuple14*_tmp8C=_cycalloc(sizeof(*_tmp8C));({void*_tmp799=Cyc_Tcutil_copy_type(_tmp8E);_tmp8C->f1=_tmp799;}),({void*_tmp798=Cyc_Tcutil_copy_type(_tmp8D);_tmp8C->f2=_tmp798;});_tmp8C;});}
# 452
static void*Cyc_Tcutil_tvar2type(struct Cyc_Absyn_Tvar*t){
return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(t));}
# 456
void*Cyc_Tcutil_copy_type(void*t){
void*_tmp8F=Cyc_Tcutil_compress(t);void*_tmp90=_tmp8F;struct Cyc_Absyn_Datatypedecl*_tmpD4;struct Cyc_Absyn_Enumdecl*_tmpD3;struct Cyc_Absyn_Aggrdecl*_tmpD2;struct _tuple1*_tmpD1;struct Cyc_List_List*_tmpD0;struct Cyc_Absyn_Typedefdecl*_tmpCF;struct Cyc_Absyn_Exp*_tmpCE;struct Cyc_Absyn_Exp*_tmpCD;enum Cyc_Absyn_AggrKind _tmpCC;struct Cyc_List_List*_tmpCB;struct Cyc_List_List*_tmpCA;struct Cyc_List_List*_tmpC9;void*_tmpC8;struct Cyc_Absyn_Tqual _tmpC7;void*_tmpC6;struct Cyc_List_List*_tmpC5;int _tmpC4;struct Cyc_Absyn_VarargInfo*_tmpC3;struct Cyc_List_List*_tmpC2;struct Cyc_List_List*_tmpC1;struct Cyc_Absyn_Exp*_tmpC0;struct Cyc_List_List*_tmpBF;struct Cyc_Absyn_Exp*_tmpBE;struct Cyc_List_List*_tmpBD;void*_tmpBC;struct Cyc_Absyn_Tqual _tmpBB;struct Cyc_Absyn_Exp*_tmpBA;void*_tmpB9;unsigned _tmpB8;void*_tmpB7;struct Cyc_Absyn_Tqual _tmpB6;void*_tmpB5;void*_tmpB4;void*_tmpB3;void*_tmpB2;struct Cyc_Absyn_PtrLoc*_tmpB1;struct Cyc_Absyn_Tvar*_tmpB0;void*_tmpAF;struct Cyc_List_List*_tmpAE;void*_tmpAD;switch(*((int*)_tmp90)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp90)->f2 == 0){_LL1: _tmpAD=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp90)->f1;_LL2:
 return t;}else{_LL3: _tmpAF=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp90)->f1;_tmpAE=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp90)->f2;_LL4:
 return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp91=_cycalloc(sizeof(*_tmp91));_tmp91->tag=0U,_tmp91->f1=_tmpAF,({struct Cyc_List_List*_tmp79A=Cyc_Tcutil_copy_types(_tmpAE);_tmp91->f2=_tmp79A;});_tmp91;});}case 1U: _LL5: _LL6:
 return t;case 2U: _LL7: _tmpB0=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp90)->f1;_LL8:
 return Cyc_Absyn_var_type(Cyc_Tcutil_copy_tvar(_tmpB0));case 3U: _LL9: _tmpB7=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90)->f1).elt_type;_tmpB6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90)->f1).elt_tq;_tmpB5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90)->f1).ptr_atts).rgn;_tmpB4=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90)->f1).ptr_atts).nullable;_tmpB3=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90)->f1).ptr_atts).bounds;_tmpB2=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90)->f1).ptr_atts).zero_term;_tmpB1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp90)->f1).ptr_atts).ptrloc;_LLA: {
# 463
void*_tmp92=Cyc_Tcutil_copy_type(_tmpB7);
void*_tmp93=Cyc_Tcutil_copy_type(_tmpB5);
void*_tmp94=Cyc_Tcutil_copy_type(_tmpB4);
struct Cyc_Absyn_Tqual _tmp95=_tmpB6;
# 468
void*_tmp96=Cyc_Tcutil_copy_type(_tmpB3);
void*_tmp97=Cyc_Tcutil_copy_type(_tmpB2);
return(void*)({struct Cyc_Absyn_PointerType_Absyn_Type_struct*_tmp98=_cycalloc(sizeof(*_tmp98));_tmp98->tag=3U,(_tmp98->f1).elt_type=_tmp92,(_tmp98->f1).elt_tq=_tmp95,((_tmp98->f1).ptr_atts).rgn=_tmp93,((_tmp98->f1).ptr_atts).nullable=_tmp94,((_tmp98->f1).ptr_atts).bounds=_tmp96,((_tmp98->f1).ptr_atts).zero_term=_tmp97,((_tmp98->f1).ptr_atts).ptrloc=_tmpB1;_tmp98;});}case 4U: _LLB: _tmpBC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90)->f1).elt_type;_tmpBB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90)->f1).tq;_tmpBA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90)->f1).num_elts;_tmpB9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90)->f1).zero_term;_tmpB8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp90)->f1).zt_loc;_LLC:
# 472
 return(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp99=_cycalloc(sizeof(*_tmp99));_tmp99->tag=4U,({void*_tmp79D=Cyc_Tcutil_copy_type(_tmpBC);(_tmp99->f1).elt_type=_tmp79D;}),(_tmp99->f1).tq=_tmpBB,({struct Cyc_Absyn_Exp*_tmp79C=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBA);(_tmp99->f1).num_elts=_tmp79C;}),({
void*_tmp79B=Cyc_Tcutil_copy_type(_tmpB9);(_tmp99->f1).zero_term=_tmp79B;}),(_tmp99->f1).zt_loc=_tmpB8;_tmp99;});case 5U: _LLD: _tmpC9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).tvars;_tmpC8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).effect;_tmpC7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).ret_tqual;_tmpC6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).ret_type;_tmpC5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).args;_tmpC4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).c_varargs;_tmpC3=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).cyc_varargs;_tmpC2=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).rgn_po;_tmpC1=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).attributes;_tmpC0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).requires_clause;_tmpBF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).requires_relns;_tmpBE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).ensures_clause;_tmpBD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp90)->f1).ensures_relns;_LLE: {
# 475
struct Cyc_List_List*_tmp9A=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Tvar*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tvar,_tmpC9);
void*effopt2=_tmpC8 == 0?0: Cyc_Tcutil_copy_type(_tmpC8);
void*_tmp9B=Cyc_Tcutil_copy_type(_tmpC6);
struct Cyc_List_List*_tmp9C=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_arg,_tmpC5);
int _tmp9D=_tmpC4;
struct Cyc_Absyn_VarargInfo*cyc_varargs2=0;
if(_tmpC3 != 0){
struct Cyc_Absyn_VarargInfo*cv=_tmpC3;
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp9E=_cycalloc(sizeof(*_tmp9E));_tmp9E->name=cv->name,_tmp9E->tq=cv->tq,({void*_tmp79E=Cyc_Tcutil_copy_type(cv->type);_tmp9E->type=_tmp79E;}),_tmp9E->inject=cv->inject;_tmp9E;});}{
# 486
struct Cyc_List_List*_tmp9F=((struct Cyc_List_List*(*)(struct _tuple14*(*f)(struct _tuple14*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_rgncmp,_tmpC2);
struct Cyc_List_List*_tmpA0=_tmpC1;
struct Cyc_Absyn_Exp*_tmpA1=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpC0);
struct Cyc_List_List*_tmpA2=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBF);
struct Cyc_Absyn_Exp*_tmpA3=Cyc_Tcutil_deep_copy_exp_opt(1,_tmpBE);
struct Cyc_List_List*_tmpA4=Cyc_Relations_copy_relns(Cyc_Core_heap_region,_tmpBD);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmpA5=_cycalloc(sizeof(*_tmpA5));_tmpA5->tag=5U,(_tmpA5->f1).tvars=_tmp9A,(_tmpA5->f1).effect=effopt2,(_tmpA5->f1).ret_tqual=_tmpC7,(_tmpA5->f1).ret_type=_tmp9B,(_tmpA5->f1).args=_tmp9C,(_tmpA5->f1).c_varargs=_tmp9D,(_tmpA5->f1).cyc_varargs=cyc_varargs2,(_tmpA5->f1).rgn_po=_tmp9F,(_tmpA5->f1).attributes=_tmpA0,(_tmpA5->f1).requires_clause=_tmpA1,(_tmpA5->f1).requires_relns=_tmpA2,(_tmpA5->f1).ensures_clause=_tmpA3,(_tmpA5->f1).ensures_relns=_tmpA4;_tmpA5;});};}case 6U: _LLF: _tmpCA=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp90)->f1;_LL10:
# 495
 return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmpA6=_cycalloc(sizeof(*_tmpA6));_tmpA6->tag=6U,({struct Cyc_List_List*_tmp79F=((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_tqt,_tmpCA);_tmpA6->f1=_tmp79F;});_tmpA6;});case 7U: _LL11: _tmpCC=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp90)->f1;_tmpCB=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp90)->f2;_LL12:
 return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmpA7=_cycalloc(sizeof(*_tmpA7));_tmpA7->tag=7U,_tmpA7->f1=_tmpCC,({struct Cyc_List_List*_tmp7A0=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Aggrfield*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_field,_tmpCB);_tmpA7->f2=_tmp7A0;});_tmpA7;});case 9U: _LL13: _tmpCD=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp90)->f1;_LL14:
 return(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmpA8=_cycalloc(sizeof(*_tmpA8));_tmpA8->tag=9U,_tmpA8->f1=_tmpCD;_tmpA8;});case 11U: _LL15: _tmpCE=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp90)->f1;_LL16:
 return(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmpA9=_cycalloc(sizeof(*_tmpA9));_tmpA9->tag=11U,_tmpA9->f1=_tmpCE;_tmpA9;});case 8U: _LL17: _tmpD1=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp90)->f1;_tmpD0=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp90)->f2;_tmpCF=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp90)->f3;_LL18:
# 500
 return(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmpAA=_cycalloc(sizeof(*_tmpAA));_tmpAA->tag=8U,_tmpAA->f1=_tmpD1,({struct Cyc_List_List*_tmp7A1=Cyc_Tcutil_copy_types(_tmpD0);_tmpAA->f2=_tmp7A1;}),_tmpAA->f3=_tmpCF,_tmpAA->f4=0;_tmpAA;});default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp90)->f1)->r)){case 0U: _LL19: _tmpD2=((struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp90)->f1)->r)->f1;_LL1A: {
# 503
struct Cyc_List_List*_tmpAB=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD2->tvs);
return({union Cyc_Absyn_AggrInfo _tmp7A2=Cyc_Absyn_UnknownAggr(_tmpD2->kind,_tmpD2->name,0);Cyc_Absyn_aggr_type(_tmp7A2,_tmpAB);});}case 1U: _LL1B: _tmpD3=((struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp90)->f1)->r)->f1;_LL1C:
# 506
 return Cyc_Absyn_enum_type(_tmpD3->name,0);default: _LL1D: _tmpD4=((struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct*)(((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp90)->f1)->r)->f1;_LL1E: {
# 508
struct Cyc_List_List*_tmpAC=((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_tvar2type,_tmpD4->tvs);
return({union Cyc_Absyn_DatatypeInfo _tmp7A3=Cyc_Absyn_UnknownDatatype(({struct Cyc_Absyn_UnknownDatatypeInfo _tmp71E;_tmp71E.name=_tmpD4->name,_tmp71E.is_extensible=0;_tmp71E;}));Cyc_Absyn_datatype_type(_tmp7A3,_tmpAC);});}}}_LL0:;}
# 515
static void*Cyc_Tcutil_copy_designator(int preserve_types,void*d){
void*_tmpD5=d;struct _fat_ptr*_tmpD8;struct Cyc_Absyn_Exp*_tmpD7;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD5)->tag == 0U){_LL1: _tmpD7=((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmpD5)->f1;_LL2:
 return(void*)({struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*_tmpD6=_cycalloc(sizeof(*_tmpD6));_tmpD6->tag=0U,({struct Cyc_Absyn_Exp*_tmp7A4=Cyc_Tcutil_deep_copy_exp(preserve_types,_tmpD7);_tmpD6->f1=_tmp7A4;});_tmpD6;});}else{_LL3: _tmpD8=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmpD5)->f1;_LL4:
 return d;}_LL0:;}struct _tuple15{struct Cyc_List_List*f1;struct Cyc_Absyn_Exp*f2;};
# 521
static struct _tuple15*Cyc_Tcutil_copy_eds(int preserve_types,struct _tuple15*e){
# 523
return({struct _tuple15*_tmpD9=_cycalloc(sizeof(*_tmpD9));({struct Cyc_List_List*_tmp7A6=((struct Cyc_List_List*(*)(void*(*f)(int,void*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_designator,preserve_types,(e[0]).f1);_tmpD9->f1=_tmp7A6;}),({struct Cyc_Absyn_Exp*_tmp7A5=Cyc_Tcutil_deep_copy_exp(preserve_types,(e[0]).f2);_tmpD9->f2=_tmp7A5;});_tmpD9;});}
# 526
struct Cyc_Absyn_Exp*Cyc_Tcutil_deep_copy_exp(int preserve_types,struct Cyc_Absyn_Exp*e){
struct Cyc_Absyn_Exp*new_e;
int _tmpDA=preserve_types;
{void*_tmpDB=e->r;void*_tmpDC=_tmpDB;int _tmp158;struct _fat_ptr _tmp157;void*_tmp156;struct Cyc_Absyn_Exp*_tmp155;struct _fat_ptr*_tmp154;struct Cyc_Core_Opt*_tmp153;struct Cyc_List_List*_tmp152;struct Cyc_Absyn_Exp*_tmp151;struct Cyc_Absyn_Exp*_tmp150;int _tmp14F;struct Cyc_Absyn_Exp*_tmp14E;void**_tmp14D;struct Cyc_Absyn_Exp*_tmp14C;int _tmp14B;int _tmp14A;void*_tmp149;struct Cyc_Absyn_Enumfield*_tmp148;struct Cyc_Absyn_Enumdecl*_tmp147;struct Cyc_Absyn_Enumfield*_tmp146;struct _fat_ptr*_tmp145;struct Cyc_Absyn_Tqual _tmp144;void*_tmp143;struct Cyc_List_List*_tmp142;void*_tmp141;struct Cyc_List_List*_tmp140;struct _tuple1*_tmp13F;struct Cyc_List_List*_tmp13E;struct Cyc_List_List*_tmp13D;struct Cyc_Absyn_Aggrdecl*_tmp13C;struct Cyc_Absyn_Exp*_tmp13B;void*_tmp13A;int _tmp139;struct Cyc_Absyn_Vardecl*_tmp138;struct Cyc_Absyn_Exp*_tmp137;struct Cyc_Absyn_Exp*_tmp136;int _tmp135;struct Cyc_List_List*_tmp134;struct Cyc_Absyn_Datatypedecl*_tmp133;struct Cyc_Absyn_Datatypefield*_tmp132;struct Cyc_List_List*_tmp131;struct Cyc_Absyn_Exp*_tmp130;struct Cyc_Absyn_Exp*_tmp12F;struct Cyc_Absyn_Exp*_tmp12E;struct _fat_ptr*_tmp12D;int _tmp12C;int _tmp12B;struct Cyc_Absyn_Exp*_tmp12A;struct _fat_ptr*_tmp129;int _tmp128;int _tmp127;struct Cyc_List_List*_tmp126;struct Cyc_Absyn_Exp*_tmp125;struct Cyc_Absyn_Exp*_tmp124;void*_tmp123;struct Cyc_List_List*_tmp122;struct Cyc_Absyn_Exp*_tmp121;void*_tmp120;struct Cyc_Absyn_Exp*_tmp11F;struct Cyc_Absyn_Exp*_tmp11E;struct Cyc_Absyn_Exp*_tmp11D;void*_tmp11C;struct Cyc_Absyn_Exp*_tmp11B;int _tmp11A;enum Cyc_Absyn_Coercion _tmp119;struct Cyc_Absyn_Exp*_tmp118;struct Cyc_List_List*_tmp117;struct Cyc_Absyn_Exp*_tmp116;struct Cyc_Absyn_Exp*_tmp115;int _tmp114;struct Cyc_Absyn_Exp*_tmp113;struct Cyc_List_List*_tmp112;struct Cyc_Absyn_VarargCallInfo*_tmp111;int _tmp110;struct Cyc_Absyn_Exp*_tmp10F;struct Cyc_List_List*_tmp10E;int _tmp10D;struct Cyc_List_List*_tmp10C;struct _fat_ptr*_tmp10B;struct Cyc_Absyn_Tqual _tmp10A;void*_tmp109;int _tmp108;int _tmp107;struct Cyc_Absyn_Exp*_tmp106;struct Cyc_Core_Opt*_tmp105;struct Cyc_Absyn_Exp*_tmp104;struct Cyc_Absyn_Exp*_tmp103;struct Cyc_Absyn_Exp*_tmp102;struct Cyc_Absyn_Exp*_tmp101;struct Cyc_Absyn_Exp*_tmp100;struct Cyc_Absyn_Exp*_tmpFF;struct Cyc_Absyn_Exp*_tmpFE;struct Cyc_Absyn_Exp*_tmpFD;struct Cyc_Absyn_Exp*_tmpFC;struct Cyc_Absyn_Exp*_tmpFB;struct Cyc_Absyn_Exp*_tmpFA;enum Cyc_Absyn_Incrementor _tmpF9;enum Cyc_Absyn_Primop _tmpF8;struct Cyc_List_List*_tmpF7;struct _fat_ptr _tmpF6;void*_tmpF5;union Cyc_Absyn_Cnst _tmpF4;switch(*((int*)_tmpDC)){case 0U: _LL1: _tmpF4=((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL2:
 new_e=Cyc_Absyn_const_exp(_tmpF4,e->loc);goto _LL0;case 1U: _LL3: _tmpF5=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL4:
 new_e=Cyc_Absyn_varb_exp(_tmpF5,e->loc);goto _LL0;case 2U: _LL5: _tmpF6=((struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL6:
 new_e=Cyc_Absyn_pragma_exp(_tmpF6,e->loc);goto _LL0;case 3U: _LL7: _tmpF8=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpF7=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL8:
 new_e=({enum Cyc_Absyn_Primop _tmp7A8=_tmpF8;struct Cyc_List_List*_tmp7A7=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDA,_tmpF7);Cyc_Absyn_primop_exp(_tmp7A8,_tmp7A7,e->loc);});goto _LL0;case 5U: _LL9: _tmpFA=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpF9=((struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LLA:
 new_e=({struct Cyc_Absyn_Exp*_tmp7AA=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmpFA);enum Cyc_Absyn_Incrementor _tmp7A9=_tmpF9;Cyc_Absyn_increment_exp(_tmp7AA,_tmp7A9,e->loc);});goto _LL0;case 7U: _LLB: _tmpFC=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpFB=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LLC:
 new_e=({struct Cyc_Absyn_Exp*_tmp7AC=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmpFC);struct Cyc_Absyn_Exp*_tmp7AB=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmpFB);Cyc_Absyn_and_exp(_tmp7AC,_tmp7AB,e->loc);});goto _LL0;case 8U: _LLD: _tmpFE=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpFD=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LLE:
 new_e=({struct Cyc_Absyn_Exp*_tmp7AE=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmpFE);struct Cyc_Absyn_Exp*_tmp7AD=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmpFD);Cyc_Absyn_or_exp(_tmp7AE,_tmp7AD,e->loc);});goto _LL0;case 9U: _LLF: _tmp100=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmpFF=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL10:
 new_e=({struct Cyc_Absyn_Exp*_tmp7B0=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp100);struct Cyc_Absyn_Exp*_tmp7AF=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmpFF);Cyc_Absyn_seq_exp(_tmp7B0,_tmp7AF,e->loc);});goto _LL0;case 6U: _LL11: _tmp103=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp102=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp101=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL12:
# 539
 new_e=({struct Cyc_Absyn_Exp*_tmp7B3=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp103);struct Cyc_Absyn_Exp*_tmp7B2=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp102);struct Cyc_Absyn_Exp*_tmp7B1=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp101);Cyc_Absyn_conditional_exp(_tmp7B3,_tmp7B2,_tmp7B1,e->loc);});goto _LL0;case 4U: _LL13: _tmp106=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp105=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp104=((struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL14:
# 541
 new_e=({struct Cyc_Absyn_Exp*_tmp7B6=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp106);struct Cyc_Core_Opt*_tmp7B5=(unsigned)_tmp105?({struct Cyc_Core_Opt*_tmpDD=_cycalloc(sizeof(*_tmpDD));_tmpDD->v=(void*)_tmp105->v;_tmpDD;}): 0;struct Cyc_Absyn_Exp*_tmp7B4=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp104);Cyc_Absyn_assignop_exp(_tmp7B6,_tmp7B5,_tmp7B4,e->loc);});
goto _LL0;case 10U: if(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f3 != 0){_LL15: _tmp10F=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp10E=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp10D=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f3)->num_varargs;_tmp10C=(((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f3)->injectors;_tmp10B=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f3)->vai)->name;_tmp10A=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f3)->vai)->tq;_tmp109=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f3)->vai)->type;_tmp108=((((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f3)->vai)->inject;_tmp107=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f4;_LL16:
# 544
 new_e=({void*_tmp7BC=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE0=_cycalloc(sizeof(*_tmpE0));_tmpE0->tag=10U,({
struct Cyc_Absyn_Exp*_tmp7BB=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp10F);_tmpE0->f1=_tmp7BB;}),({struct Cyc_List_List*_tmp7BA=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDA,_tmp10E);_tmpE0->f2=_tmp7BA;}),({
struct Cyc_Absyn_VarargCallInfo*_tmp7B9=({struct Cyc_Absyn_VarargCallInfo*_tmpDF=_cycalloc(sizeof(*_tmpDF));_tmpDF->num_varargs=_tmp10D,_tmpDF->injectors=_tmp10C,({
struct Cyc_Absyn_VarargInfo*_tmp7B8=({struct Cyc_Absyn_VarargInfo*_tmpDE=_cycalloc(sizeof(*_tmpDE));_tmpDE->name=_tmp10B,_tmpDE->tq=_tmp10A,({void*_tmp7B7=Cyc_Tcutil_copy_type(_tmp109);_tmpDE->type=_tmp7B7;}),_tmpDE->inject=_tmp108;_tmpDE;});_tmpDF->vai=_tmp7B8;});_tmpDF;});
# 546
_tmpE0->f3=_tmp7B9;}),_tmpE0->f4=_tmp107;_tmpE0;});
# 544
Cyc_Absyn_new_exp(_tmp7BC,e->loc);});
# 549
goto _LL0;}else{_LL17: _tmp113=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp112=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp111=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_tmp110=((struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*)_tmpDC)->f4;_LL18:
# 551
 new_e=({void*_tmp7BF=(void*)({struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct*_tmpE1=_cycalloc(sizeof(*_tmpE1));_tmpE1->tag=10U,({struct Cyc_Absyn_Exp*_tmp7BE=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp113);_tmpE1->f1=_tmp7BE;}),({struct Cyc_List_List*_tmp7BD=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDA,_tmp112);_tmpE1->f2=_tmp7BD;}),_tmpE1->f3=_tmp111,_tmpE1->f4=_tmp110;_tmpE1;});Cyc_Absyn_new_exp(_tmp7BF,e->loc);});
goto _LL0;}case 11U: _LL19: _tmp115=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp114=((struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL1A:
# 554
 new_e=_tmp114?({struct Cyc_Absyn_Exp*_tmp7C1=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp115);Cyc_Absyn_rethrow_exp(_tmp7C1,e->loc);}):({struct Cyc_Absyn_Exp*_tmp7C0=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp115);Cyc_Absyn_throw_exp(_tmp7C0,e->loc);});
goto _LL0;case 12U: _LL1B: _tmp116=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL1C:
# 557
 new_e=({struct Cyc_Absyn_Exp*_tmp7C2=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp116);Cyc_Absyn_noinstantiate_exp(_tmp7C2,e->loc);});goto _LL0;case 13U: _LL1D: _tmp118=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp117=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL1E:
# 559
 new_e=({struct Cyc_Absyn_Exp*_tmp7C4=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp118);struct Cyc_List_List*_tmp7C3=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp117);Cyc_Absyn_instantiate_exp(_tmp7C4,_tmp7C3,e->loc);});goto _LL0;case 14U: _LL1F: _tmp11C=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp11B=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp11A=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_tmp119=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmpDC)->f4;_LL20:
# 561
 new_e=({void*_tmp7C8=Cyc_Tcutil_copy_type(_tmp11C);struct Cyc_Absyn_Exp*_tmp7C7=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp11B);int _tmp7C6=_tmp11A;enum Cyc_Absyn_Coercion _tmp7C5=_tmp119;Cyc_Absyn_cast_exp(_tmp7C8,_tmp7C7,_tmp7C6,_tmp7C5,e->loc);});goto _LL0;case 16U: _LL21: _tmp11E=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp11D=((struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL22:
# 563
 new_e=({struct Cyc_Absyn_Exp*_tmp7CA=(unsigned)_tmp11E?Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp11E): 0;struct Cyc_Absyn_Exp*_tmp7C9=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp11D);Cyc_Absyn_New_exp(_tmp7CA,_tmp7C9,e->loc);});goto _LL0;case 15U: _LL23: _tmp11F=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL24:
 new_e=({struct Cyc_Absyn_Exp*_tmp7CB=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp11F);Cyc_Absyn_address_exp(_tmp7CB,e->loc);});goto _LL0;case 17U: _LL25: _tmp120=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL26:
 new_e=({void*_tmp7CC=Cyc_Tcutil_copy_type(_tmp120);Cyc_Absyn_sizeoftype_exp(_tmp7CC,e->loc);});goto _LL0;case 18U: _LL27: _tmp121=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL28:
 new_e=({struct Cyc_Absyn_Exp*_tmp7CD=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp121);Cyc_Absyn_sizeofexp_exp(_tmp7CD,e->loc);});goto _LL0;case 19U: _LL29: _tmp123=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp122=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL2A:
 new_e=({void*_tmp7CF=Cyc_Tcutil_copy_type(_tmp123);struct Cyc_List_List*_tmp7CE=_tmp122;Cyc_Absyn_offsetof_exp(_tmp7CF,_tmp7CE,e->loc);});goto _LL0;case 20U: _LL2B: _tmp124=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL2C:
 new_e=({struct Cyc_Absyn_Exp*_tmp7D0=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp124);Cyc_Absyn_deref_exp(_tmp7D0,e->loc);});goto _LL0;case 41U: _LL2D: _tmp125=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL2E:
 new_e=({struct Cyc_Absyn_Exp*_tmp7D1=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp125);Cyc_Absyn_extension_exp(_tmp7D1,e->loc);});goto _LL0;case 24U: _LL2F: _tmp126=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL30:
 new_e=({struct Cyc_List_List*_tmp7D2=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDA,_tmp126);Cyc_Absyn_tuple_exp(_tmp7D2,e->loc);});goto _LL0;case 21U: _LL31: _tmp12A=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp129=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp128=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_tmp127=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmpDC)->f4;_LL32:
# 572
 new_e=({void*_tmp7D4=(void*)({struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*_tmpE2=_cycalloc(sizeof(*_tmpE2));_tmpE2->tag=21U,({struct Cyc_Absyn_Exp*_tmp7D3=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp12A);_tmpE2->f1=_tmp7D3;}),_tmpE2->f2=_tmp129,_tmpE2->f3=_tmp128,_tmpE2->f4=_tmp127;_tmpE2;});Cyc_Absyn_new_exp(_tmp7D4,e->loc);});goto _LL0;case 22U: _LL33: _tmp12E=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp12D=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp12C=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_tmp12B=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmpDC)->f4;_LL34:
# 574
 new_e=({void*_tmp7D6=(void*)({struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*_tmpE3=_cycalloc(sizeof(*_tmpE3));_tmpE3->tag=22U,({struct Cyc_Absyn_Exp*_tmp7D5=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp12E);_tmpE3->f1=_tmp7D5;}),_tmpE3->f2=_tmp12D,_tmpE3->f3=_tmp12C,_tmpE3->f4=_tmp12B;_tmpE3;});Cyc_Absyn_new_exp(_tmp7D6,e->loc);});goto _LL0;case 23U: _LL35: _tmp130=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp12F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL36:
# 576
 new_e=({struct Cyc_Absyn_Exp*_tmp7D8=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp130);struct Cyc_Absyn_Exp*_tmp7D7=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp12F);Cyc_Absyn_subscript_exp(_tmp7D8,_tmp7D7,e->loc);});goto _LL0;case 26U: _LL37: _tmp131=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL38:
# 578
 new_e=({void*_tmp7DA=(void*)({struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*_tmpE4=_cycalloc(sizeof(*_tmpE4));_tmpE4->tag=26U,({struct Cyc_List_List*_tmp7D9=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDA,_tmp131);_tmpE4->f1=_tmp7D9;});_tmpE4;});Cyc_Absyn_new_exp(_tmp7DA,e->loc);});goto _LL0;case 31U: _LL39: _tmp134=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp133=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp132=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL3A:
# 580
 new_e=({void*_tmp7DC=(void*)({struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*_tmpE5=_cycalloc(sizeof(*_tmpE5));_tmpE5->tag=31U,({struct Cyc_List_List*_tmp7DB=((struct Cyc_List_List*(*)(struct Cyc_Absyn_Exp*(*f)(int,struct Cyc_Absyn_Exp*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_deep_copy_exp,_tmpDA,_tmp134);_tmpE5->f1=_tmp7DB;}),_tmpE5->f2=_tmp133,_tmpE5->f3=_tmp132;_tmpE5;});Cyc_Absyn_new_exp(_tmp7DC,e->loc);});goto _LL0;case 27U: _LL3B: _tmp138=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp137=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp136=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_tmp135=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmpDC)->f4;_LL3C:
# 582
 new_e=({void*_tmp7DF=(void*)({struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*_tmpE6=_cycalloc(sizeof(*_tmpE6));_tmpE6->tag=27U,_tmpE6->f1=_tmp138,({struct Cyc_Absyn_Exp*_tmp7DE=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp137);_tmpE6->f2=_tmp7DE;}),({struct Cyc_Absyn_Exp*_tmp7DD=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp136);_tmpE6->f3=_tmp7DD;}),_tmpE6->f4=_tmp135;_tmpE6;});Cyc_Absyn_new_exp(_tmp7DF,e->loc);});goto _LL0;case 28U: _LL3D: _tmp13B=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp13A=(void*)((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp139=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_LL3E:
# 584
 new_e=({void*_tmp7E2=(void*)({struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*_tmpE7=_cycalloc(sizeof(*_tmpE7));_tmpE7->tag=28U,({struct Cyc_Absyn_Exp*_tmp7E1=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp13B);_tmpE7->f1=_tmp7E1;}),({void*_tmp7E0=Cyc_Tcutil_copy_type(_tmp13A);_tmpE7->f2=_tmp7E0;}),_tmpE7->f3=_tmp139;_tmpE7;});Cyc_Absyn_new_exp(_tmp7E2,_tmp13B->loc);});
goto _LL0;case 29U: _LL3F: _tmp13F=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp13E=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_tmp13D=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDC)->f3;_tmp13C=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmpDC)->f4;_LL40:
# 587
 new_e=({void*_tmp7E5=(void*)({struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*_tmpE8=_cycalloc(sizeof(*_tmpE8));_tmpE8->tag=29U,_tmpE8->f1=_tmp13F,({struct Cyc_List_List*_tmp7E4=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_copy_type,_tmp13E);_tmpE8->f2=_tmp7E4;}),({struct Cyc_List_List*_tmp7E3=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDA,_tmp13D);_tmpE8->f3=_tmp7E3;}),_tmpE8->f4=_tmp13C;_tmpE8;});Cyc_Absyn_new_exp(_tmp7E5,e->loc);});
# 589
goto _LL0;case 30U: _LL41: _tmp141=(void*)((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp140=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL42:
# 591
 new_e=({void*_tmp7E8=(void*)({struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*_tmpE9=_cycalloc(sizeof(*_tmpE9));_tmpE9->tag=30U,({void*_tmp7E7=Cyc_Tcutil_copy_type(_tmp141);_tmpE9->f1=_tmp7E7;}),({struct Cyc_List_List*_tmp7E6=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDA,_tmp140);_tmpE9->f2=_tmp7E6;});_tmpE9;});Cyc_Absyn_new_exp(_tmp7E8,e->loc);});
goto _LL0;case 25U: _LL43: _tmp145=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDC)->f1)->f1;_tmp144=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDC)->f1)->f2;_tmp143=(((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDC)->f1)->f3;_tmp142=((struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL44:
# 594
 new_e=({void*_tmp7EC=(void*)({struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct*_tmpEB=_cycalloc(sizeof(*_tmpEB));_tmpEB->tag=25U,({struct _tuple9*_tmp7EB=({struct _tuple9*_tmpEA=_cycalloc(sizeof(*_tmpEA));_tmpEA->f1=_tmp145,_tmpEA->f2=_tmp144,({void*_tmp7EA=Cyc_Tcutil_copy_type(_tmp143);_tmpEA->f3=_tmp7EA;});_tmpEA;});_tmpEB->f1=_tmp7EB;}),({
struct Cyc_List_List*_tmp7E9=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDA,_tmp142);_tmpEB->f2=_tmp7E9;});_tmpEB;});
# 594
Cyc_Absyn_new_exp(_tmp7EC,e->loc);});
# 596
goto _LL0;case 32U: _LL45: _tmp147=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp146=((struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL46:
 new_e=e;goto _LL0;case 33U: _LL47: _tmp149=(void*)((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp148=((struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL48:
# 599
 new_e=({void*_tmp7EE=(void*)({struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct*_tmpEC=_cycalloc(sizeof(*_tmpEC));_tmpEC->tag=33U,({void*_tmp7ED=Cyc_Tcutil_copy_type(_tmp149);_tmpEC->f1=_tmp7ED;}),_tmpEC->f2=_tmp148;_tmpEC;});Cyc_Absyn_new_exp(_tmp7EE,e->loc);});goto _LL0;case 34U: _LL49: _tmp14F=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).is_calloc;_tmp14E=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).rgn;_tmp14D=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).elt_type;_tmp14C=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).num_elts;_tmp14B=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).fat_result;_tmp14A=(((struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*)_tmpDC)->f1).inline_call;_LL4A: {
# 601
struct Cyc_Absyn_Exp*e2=Cyc_Absyn_copy_exp(e);
struct Cyc_Absyn_Exp*r1=_tmp14E;if(_tmp14E != 0)r1=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp14E);{
void**t1=_tmp14D;if(_tmp14D != 0)t1=({void**_tmpED=_cycalloc(sizeof(*_tmpED));({void*_tmp7EF=Cyc_Tcutil_copy_type(*_tmp14D);*_tmpED=_tmp7EF;});_tmpED;});
({void*_tmp7F0=(void*)({struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct*_tmpEE=_cycalloc(sizeof(*_tmpEE));_tmpEE->tag=34U,(_tmpEE->f1).is_calloc=_tmp14F,(_tmpEE->f1).rgn=r1,(_tmpEE->f1).elt_type=t1,(_tmpEE->f1).num_elts=_tmp14C,(_tmpEE->f1).fat_result=_tmp14B,(_tmpEE->f1).inline_call=_tmp14A;_tmpEE;});e2->r=_tmp7F0;});
new_e=e2;
goto _LL0;};}case 35U: _LL4B: _tmp151=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp150=((struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL4C:
 new_e=({struct Cyc_Absyn_Exp*_tmp7F2=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp151);struct Cyc_Absyn_Exp*_tmp7F1=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp150);Cyc_Absyn_swap_exp(_tmp7F2,_tmp7F1,e->loc);});goto _LL0;case 36U: _LL4D: _tmp153=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp152=((struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL4E: {
# 609
struct Cyc_Core_Opt*nopt1=_tmp153;
if(_tmp153 != 0)nopt1=({struct Cyc_Core_Opt*_tmpEF=_cycalloc(sizeof(*_tmpEF));_tmpEF->v=(struct _tuple1*)_tmp153->v;_tmpEF;});
new_e=({void*_tmp7F4=(void*)({struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct*_tmpF0=_cycalloc(sizeof(*_tmpF0));_tmpF0->tag=36U,_tmpF0->f1=nopt1,({struct Cyc_List_List*_tmp7F3=((struct Cyc_List_List*(*)(struct _tuple15*(*f)(int,struct _tuple15*),int env,struct Cyc_List_List*x))Cyc_List_map_c)(Cyc_Tcutil_copy_eds,_tmpDA,_tmp152);_tmpF0->f2=_tmp7F3;});_tmpF0;});Cyc_Absyn_new_exp(_tmp7F4,e->loc);});
goto _LL0;}case 37U: _LL4F: _LL50:
# 614
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpF2=_cycalloc(sizeof(*_tmpF2));_tmpF2->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp7F5=({const char*_tmpF1="deep_copy: statement expressions unsupported";_tag_fat(_tmpF1,sizeof(char),45U);});_tmpF2->f1=_tmp7F5;});_tmpF2;}));case 38U: _LL51: _tmp155=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp154=((struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL52:
# 616
 new_e=({void*_tmp7F7=(void*)({struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct*_tmpF3=_cycalloc(sizeof(*_tmpF3));_tmpF3->tag=38U,({struct Cyc_Absyn_Exp*_tmp7F6=Cyc_Tcutil_deep_copy_exp(_tmpDA,_tmp155);_tmpF3->f1=_tmp7F6;}),_tmpF3->f2=_tmp154;_tmpF3;});Cyc_Absyn_new_exp(_tmp7F7,e->loc);});goto _LL0;case 39U: _LL53: _tmp156=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_LL54:
 new_e=({void*_tmp7F8=Cyc_Tcutil_copy_type(_tmp156);Cyc_Absyn_valueof_exp(_tmp7F8,e->loc);});goto _LL0;default: _LL55: _tmp158=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDC)->f1;_tmp157=((struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct*)_tmpDC)->f2;_LL56:
 new_e=Cyc_Absyn_asm_exp(_tmp158,_tmp157,e->loc);goto _LL0;}_LL0:;}
# 621
if(preserve_types){
new_e->topt=e->topt;
new_e->annot=e->annot;}
# 625
return new_e;}struct _tuple16{enum Cyc_Absyn_KindQual f1;enum Cyc_Absyn_KindQual f2;};struct _tuple17{enum Cyc_Absyn_AliasQual f1;enum Cyc_Absyn_AliasQual f2;};
# 636 "tcutil.cyc"
int Cyc_Tcutil_kind_leq(struct Cyc_Absyn_Kind*ka1,struct Cyc_Absyn_Kind*ka2){
struct Cyc_Absyn_Kind*_tmp159=ka1;enum Cyc_Absyn_KindQual _tmp162;enum Cyc_Absyn_AliasQual _tmp161;_LL1: _tmp162=_tmp159->kind;_tmp161=_tmp159->aliasqual;_LL2:;{
struct Cyc_Absyn_Kind*_tmp15A=ka2;enum Cyc_Absyn_KindQual _tmp160;enum Cyc_Absyn_AliasQual _tmp15F;_LL4: _tmp160=_tmp15A->kind;_tmp15F=_tmp15A->aliasqual;_LL5:;
# 640
if((int)_tmp162 != (int)_tmp160){
struct _tuple16 _tmp15B=({struct _tuple16 _tmp71F;_tmp71F.f1=_tmp162,_tmp71F.f2=_tmp160;_tmp71F;});struct _tuple16 _tmp15C=_tmp15B;switch(_tmp15C.f1){case Cyc_Absyn_BoxKind: switch(_tmp15C.f2){case Cyc_Absyn_MemKind: _LL7: _LL8:
 goto _LLA;case Cyc_Absyn_AnyKind: _LL9: _LLA:
 goto _LLC;default: goto _LLD;}case Cyc_Absyn_MemKind: if(_tmp15C.f2 == Cyc_Absyn_AnyKind){_LLB: _LLC:
 goto _LL6;}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL6:;}
# 648
if((int)_tmp161 != (int)_tmp15F){
struct _tuple17 _tmp15D=({struct _tuple17 _tmp720;_tmp720.f1=_tmp161,_tmp720.f2=_tmp15F;_tmp720;});struct _tuple17 _tmp15E=_tmp15D;switch(_tmp15E.f1){case Cyc_Absyn_Aliasable: if(_tmp15E.f2 == Cyc_Absyn_Top){_LL10: _LL11:
 goto _LL13;}else{goto _LL14;}case Cyc_Absyn_Unique: if(_tmp15E.f2 == Cyc_Absyn_Top){_LL12: _LL13:
 return 1;}else{goto _LL14;}default: _LL14: _LL15:
 return 0;}_LLF:;}
# 654
return 1;};}
# 657
struct Cyc_Absyn_Kind*Cyc_Tcutil_tvar_kind(struct Cyc_Absyn_Tvar*tv,struct Cyc_Absyn_Kind*def){
void*_tmp163=Cyc_Absyn_compress_kb(tv->kind);void*_tmp164=_tmp163;struct Cyc_Absyn_Kind*_tmp167;struct Cyc_Absyn_Kind*_tmp166;switch(*((int*)_tmp164)){case 0U: _LL1: _tmp166=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp164)->f1;_LL2:
 return _tmp166;case 2U: _LL3: _tmp167=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp164)->f2;_LL4:
 return _tmp167;default: _LL5: _LL6:
({void*_tmp7F9=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp165=_cycalloc(sizeof(*_tmp165));_tmp165->tag=2U,_tmp165->f1=0,_tmp165->f2=def;_tmp165;});tv->kind=_tmp7F9;});return def;}_LL0:;}
# 665
int Cyc_Tcutil_unify_kindbound(void*kb1,void*kb2){
struct _tuple14 _tmp168=({struct _tuple14 _tmp721;({void*_tmp7FB=Cyc_Absyn_compress_kb(kb1);_tmp721.f1=_tmp7FB;}),({void*_tmp7FA=Cyc_Absyn_compress_kb(kb2);_tmp721.f2=_tmp7FA;});_tmp721;});struct _tuple14 _tmp169=_tmp168;struct Cyc_Core_Opt**_tmp17E;void*_tmp17D;void*_tmp17C;struct Cyc_Core_Opt**_tmp17B;struct Cyc_Core_Opt**_tmp17A;struct Cyc_Absyn_Kind*_tmp179;struct Cyc_Core_Opt**_tmp178;struct Cyc_Absyn_Kind*_tmp177;struct Cyc_Core_Opt**_tmp176;struct Cyc_Absyn_Kind*_tmp175;struct Cyc_Absyn_Kind*_tmp174;struct Cyc_Absyn_Kind*_tmp173;struct Cyc_Core_Opt**_tmp172;struct Cyc_Absyn_Kind*_tmp171;struct Cyc_Absyn_Kind*_tmp170;struct Cyc_Absyn_Kind*_tmp16F;switch(*((int*)_tmp169.f1)){case 0U: switch(*((int*)_tmp169.f2)){case 0U: _LL1: _tmp170=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp169.f1)->f1;_tmp16F=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp169.f2)->f1;_LL2:
 return _tmp170 == _tmp16F;case 2U: _LL5: _tmp173=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp169.f1)->f1;_tmp172=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169.f2)->f1;_tmp171=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169.f2)->f2;_LL6:
# 674
 if(Cyc_Tcutil_kind_leq(_tmp173,_tmp171)){
({struct Cyc_Core_Opt*_tmp7FC=({struct Cyc_Core_Opt*_tmp16B=_cycalloc(sizeof(*_tmp16B));_tmp16B->v=kb1;_tmp16B;});*_tmp172=_tmp7FC;});
return 1;}else{
return 0;}default: goto _LLB;}case 2U: switch(*((int*)_tmp169.f2)){case 0U: _LL3: _tmp176=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169.f1)->f1;_tmp175=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169.f1)->f2;_tmp174=((struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*)_tmp169.f2)->f1;_LL4:
# 669
 if(Cyc_Tcutil_kind_leq(_tmp174,_tmp175)){
({struct Cyc_Core_Opt*_tmp7FD=({struct Cyc_Core_Opt*_tmp16A=_cycalloc(sizeof(*_tmp16A));_tmp16A->v=kb2;_tmp16A;});*_tmp176=_tmp7FD;});
return 1;}else{
return 0;}case 2U: _LL7: _tmp17A=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169.f1)->f1;_tmp179=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169.f1)->f2;_tmp178=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169.f2)->f1;_tmp177=((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp169.f2)->f2;_LL8:
# 679
 if(Cyc_Tcutil_kind_leq(_tmp179,_tmp177)){
({struct Cyc_Core_Opt*_tmp7FE=({struct Cyc_Core_Opt*_tmp16C=_cycalloc(sizeof(*_tmp16C));_tmp16C->v=kb1;_tmp16C;});*_tmp178=_tmp7FE;});
return 1;}else{
if(Cyc_Tcutil_kind_leq(_tmp177,_tmp179)){
({struct Cyc_Core_Opt*_tmp7FF=({struct Cyc_Core_Opt*_tmp16D=_cycalloc(sizeof(*_tmp16D));_tmp16D->v=kb2;_tmp16D;});*_tmp17A=_tmp7FF;});
return 1;}else{
return 0;}}default: _LLB: _tmp17C=_tmp169.f1;_tmp17B=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp169.f2)->f1;_LLC:
# 688
({struct Cyc_Core_Opt*_tmp800=({struct Cyc_Core_Opt*_tmp16E=_cycalloc(sizeof(*_tmp16E));_tmp16E->v=_tmp17C;_tmp16E;});*_tmp17B=_tmp800;});
return 1;}default: _LL9: _tmp17E=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct*)_tmp169.f1)->f1;_tmp17D=_tmp169.f2;_LLA:
# 686
 _tmp17C=_tmp17D;_tmp17B=_tmp17E;goto _LLC;}_LL0:;}struct _tuple18{struct Cyc_Absyn_Tvar*f1;void*f2;};
# 693
struct _tuple18 Cyc_Tcutil_swap_kind(void*t,void*kb){
void*_tmp17F=Cyc_Tcutil_compress(t);void*_tmp180=_tmp17F;struct Cyc_Absyn_Tvar*_tmp185;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp180)->tag == 2U){_LL1: _tmp185=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp180)->f1;_LL2: {
# 696
void*_tmp181=_tmp185->kind;
_tmp185->kind=kb;
return({struct _tuple18 _tmp722;_tmp722.f1=_tmp185,_tmp722.f2=_tmp181;_tmp722;});}}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp184=({struct Cyc_String_pa_PrintArg_struct _tmp723;_tmp723.tag=0U,({struct _fat_ptr _tmp801=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp723.f1=_tmp801;});_tmp723;});void*_tmp182[1U];_tmp182[0]=& _tmp184;({struct _fat_ptr _tmp802=({const char*_tmp183="swap_kind: cannot update the kind of %s";_tag_fat(_tmp183,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp802,_tag_fat(_tmp182,sizeof(void*),1U));});});}_LL0:;}
# 705
static struct Cyc_Absyn_Kind*Cyc_Tcutil_field_kind(void*field_type,struct Cyc_List_List*ts,struct Cyc_List_List*tvs){
# 707
struct Cyc_Absyn_Kind*k=Cyc_Tcutil_type_kind(field_type);
if(ts != 0 &&(k == & Cyc_Tcutil_ak || k == & Cyc_Tcutil_tak)){
# 711
struct _RegionHandle _tmp186=_new_region("temp");struct _RegionHandle*temp=& _tmp186;_push_region(temp);
{struct Cyc_List_List*_tmp187=0;
# 714
for(0;tvs != 0;(tvs=tvs->tl,ts=ts->tl)){
struct Cyc_Absyn_Tvar*_tmp188=(struct Cyc_Absyn_Tvar*)tvs->hd;
void*_tmp189=(void*)((struct Cyc_List_List*)_check_null(ts))->hd;
struct Cyc_Absyn_Kind*_tmp18A=Cyc_Tcutil_tvar_kind(_tmp188,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp18B=_tmp18A;switch(((struct Cyc_Absyn_Kind*)_tmp18B)->kind){case Cyc_Absyn_IntKind: _LL1: _LL2:
 goto _LL4;case Cyc_Absyn_AnyKind: _LL3: _LL4:
# 720
 _tmp187=({struct Cyc_List_List*_tmp18D=_region_malloc(temp,sizeof(*_tmp18D));({struct _tuple18*_tmp803=({struct _tuple18*_tmp18C=_region_malloc(temp,sizeof(*_tmp18C));_tmp18C->f1=_tmp188,_tmp18C->f2=_tmp189;_tmp18C;});_tmp18D->hd=_tmp803;}),_tmp18D->tl=_tmp187;_tmp18D;});goto _LL0;default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 724
if(_tmp187 != 0){
field_type=({struct _RegionHandle*_tmp805=temp;struct Cyc_List_List*_tmp804=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(_tmp187);Cyc_Tcutil_rsubstitute(_tmp805,_tmp804,field_type);});
k=Cyc_Tcutil_type_kind(field_type);}}
# 712
;_pop_region(temp);}
# 729
return k;}
# 736
struct Cyc_Absyn_Kind*Cyc_Tcutil_type_kind(void*t){
# 738
void*_tmp18E=Cyc_Tcutil_compress(t);void*_tmp18F=_tmp18E;struct Cyc_Absyn_Typedefdecl*_tmp1AD;struct Cyc_Absyn_Exp*_tmp1AC;struct Cyc_Absyn_PtrInfo _tmp1AB;void*_tmp1AA;struct Cyc_List_List*_tmp1A9;struct Cyc_Absyn_Tvar*_tmp1A8;struct Cyc_Core_Opt*_tmp1A7;switch(*((int*)_tmp18F)){case 1U: _LL1: _tmp1A7=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp18F)->f1;_LL2:
 return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1A7))->v;case 2U: _LL3: _tmp1A8=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp18F)->f1;_LL4:
 return Cyc_Tcutil_tvar_kind(_tmp1A8,& Cyc_Tcutil_bk);case 0U: _LL5: _tmp1AA=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18F)->f1;_tmp1A9=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp18F)->f2;_LL6: {
# 742
void*_tmp190=_tmp1AA;enum Cyc_Absyn_AggrKind _tmp19D;struct Cyc_List_List*_tmp19C;struct Cyc_Absyn_AggrdeclImpl*_tmp19B;int _tmp19A;struct Cyc_Absyn_Kind*_tmp199;enum Cyc_Absyn_Size_of _tmp198;switch(*((int*)_tmp190)){case 0U: _LL1E: _LL1F:
 return& Cyc_Tcutil_mk;case 1U: _LL20: _tmp198=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp190)->f2;_LL21:
 return((int)_tmp198 == (int)2U ||(int)_tmp198 == (int)3U)?& Cyc_Tcutil_bk:& Cyc_Tcutil_mk;case 2U: _LL22: _LL23:
 return& Cyc_Tcutil_mk;case 15U: _LL24: _LL25:
 goto _LL27;case 16U: _LL26: _LL27:
 goto _LL29;case 3U: _LL28: _LL29:
 return& Cyc_Tcutil_bk;case 6U: _LL2A: _LL2B:
 return& Cyc_Tcutil_urk;case 5U: _LL2C: _LL2D:
 return& Cyc_Tcutil_rk;case 7U: _LL2E: _LL2F:
 return& Cyc_Tcutil_trk;case 17U: _LL30: _tmp199=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp190)->f2;_LL31:
 return _tmp199;case 4U: _LL32: _LL33:
 return& Cyc_Tcutil_bk;case 8U: _LL34: _LL35:
 goto _LL37;case 9U: _LL36: _LL37:
 goto _LL39;case 10U: _LL38: _LL39:
 return& Cyc_Tcutil_ek;case 12U: _LL3A: _LL3B:
 goto _LL3D;case 11U: _LL3C: _LL3D:
 return& Cyc_Tcutil_boolk;case 13U: _LL3E: _LL3F:
 goto _LL41;case 14U: _LL40: _LL41:
 return& Cyc_Tcutil_ptrbk;case 18U: _LL42: _LL43:
 return& Cyc_Tcutil_ak;case 19U: if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp190)->f1).KnownDatatypefield).tag == 2){_LL44: _LL45:
# 763
 return& Cyc_Tcutil_mk;}else{_LL46: _LL47:
# 765
({void*_tmp191=0U;({struct _fat_ptr _tmp806=({const char*_tmp192="type_kind: Unresolved DatatypeFieldType";_tag_fat(_tmp192,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp806,_tag_fat(_tmp191,sizeof(void*),0U));});});}default: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp190)->f1).UnknownAggr).tag == 1){_LL48: _LL49:
# 769
 return& Cyc_Tcutil_ak;}else{_LL4A: _tmp19D=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp190)->f1).KnownAggr).val)->kind;_tmp19C=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp190)->f1).KnownAggr).val)->tvs;_tmp19B=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp190)->f1).KnownAggr).val)->impl;_tmp19A=(*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp190)->f1).KnownAggr).val)->expected_mem_kind;_LL4B:
# 771
 if(_tmp19B == 0){
if(_tmp19A)
return& Cyc_Tcutil_mk;else{
# 775
return& Cyc_Tcutil_ak;}}{
# 777
struct Cyc_List_List*_tmp193=_tmp19B->fields;
if(_tmp193 == 0)return& Cyc_Tcutil_mk;
# 780
if((int)_tmp19D == (int)0U){
for(0;((struct Cyc_List_List*)_check_null(_tmp193))->tl != 0;_tmp193=_tmp193->tl){;}{
void*_tmp194=((struct Cyc_Absyn_Aggrfield*)_tmp193->hd)->type;
struct Cyc_Absyn_Kind*_tmp195=Cyc_Tcutil_field_kind(_tmp194,_tmp1A9,_tmp19C);
if(_tmp195 == & Cyc_Tcutil_ak || _tmp195 == & Cyc_Tcutil_tak)return _tmp195;};}else{
# 788
for(0;_tmp193 != 0;_tmp193=_tmp193->tl){
void*_tmp196=((struct Cyc_Absyn_Aggrfield*)_tmp193->hd)->type;
struct Cyc_Absyn_Kind*_tmp197=Cyc_Tcutil_field_kind(_tmp196,_tmp1A9,_tmp19C);
if(_tmp197 == & Cyc_Tcutil_ak || _tmp197 == & Cyc_Tcutil_tak)return _tmp197;}}
# 794
return& Cyc_Tcutil_mk;};}}_LL1D:;}case 5U: _LL7: _LL8:
# 796
 return& Cyc_Tcutil_ak;case 7U: _LL9: _LLA:
 return& Cyc_Tcutil_mk;case 3U: _LLB: _tmp1AB=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp18F)->f1;_LLC: {
# 799
void*_tmp19E=Cyc_Tcutil_compress((_tmp1AB.ptr_atts).bounds);void*_tmp19F=_tmp19E;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19F)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp19F)->f1)){case 13U: _LL4D: _LL4E: {
# 801
enum Cyc_Absyn_AliasQual _tmp1A0=(Cyc_Tcutil_type_kind((_tmp1AB.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A1=_tmp1A0;switch(_tmp1A1){case Cyc_Absyn_Aliasable: _LL54: _LL55:
 return& Cyc_Tcutil_bk;case Cyc_Absyn_Unique: _LL56: _LL57:
 return& Cyc_Tcutil_ubk;case Cyc_Absyn_Top: _LL58: _LL59:
 goto _LL5B;default: _LL5A: _LL5B: return& Cyc_Tcutil_tbk;}_LL53:;}case 14U: _LL4F: _LL50:
# 807
 goto _LL52;default: goto _LL51;}else{_LL51: _LL52: {
# 809
enum Cyc_Absyn_AliasQual _tmp1A2=(Cyc_Tcutil_type_kind((_tmp1AB.ptr_atts).rgn))->aliasqual;enum Cyc_Absyn_AliasQual _tmp1A3=_tmp1A2;switch(_tmp1A3){case Cyc_Absyn_Aliasable: _LL5D: _LL5E:
 return& Cyc_Tcutil_mk;case Cyc_Absyn_Unique: _LL5F: _LL60:
 return& Cyc_Tcutil_umk;case Cyc_Absyn_Top: _LL61: _LL62:
 goto _LL64;default: _LL63: _LL64: return& Cyc_Tcutil_tmk;}_LL5C:;}}_LL4C:;}case 9U: _LLD: _LLE:
# 815
 return& Cyc_Tcutil_ik;case 11U: _LLF: _LL10:
# 819
 return& Cyc_Tcutil_ak;case 4U: _LL11: _tmp1AC=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp18F)->f1).num_elts;_LL12:
# 821
 if(_tmp1AC == 0 || Cyc_Tcutil_is_const_exp(_tmp1AC))return& Cyc_Tcutil_mk;
return& Cyc_Tcutil_ak;case 6U: _LL13: _LL14:
 return& Cyc_Tcutil_mk;case 8U: _LL15: _tmp1AD=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp18F)->f3;_LL16:
# 825
 if(_tmp1AD == 0 || _tmp1AD->kind == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp1A6=({struct Cyc_String_pa_PrintArg_struct _tmp724;_tmp724.tag=0U,({struct _fat_ptr _tmp807=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp724.f1=_tmp807;});_tmp724;});void*_tmp1A4[1U];_tmp1A4[0]=& _tmp1A6;({struct _fat_ptr _tmp808=({const char*_tmp1A5="type_kind: typedef found: %s";_tag_fat(_tmp1A5,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp808,_tag_fat(_tmp1A4,sizeof(void*),1U));});});
return(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp1AD->kind))->v;default: switch(*((int*)((struct Cyc_Absyn_TypeDecl*)((struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct*)_tmp18F)->f1)->r)){case 0U: _LL17: _LL18:
 return& Cyc_Tcutil_ak;case 1U: _LL19: _LL1A:
 return& Cyc_Tcutil_bk;default: _LL1B: _LL1C:
 return& Cyc_Tcutil_ak;}}_LL0:;}
# 834
int Cyc_Tcutil_kind_eq(struct Cyc_Absyn_Kind*k1,struct Cyc_Absyn_Kind*k2){
return k1 == k2 ||(int)k1->kind == (int)k2->kind &&(int)k1->aliasqual == (int)k2->aliasqual;}
# 838
static void Cyc_Tcutil_unify_it(void*t1,void*t2);
# 841
int Cyc_Tcutil_unify(void*t1,void*t2){
struct _handler_cons _tmp1AE;_push_handler(& _tmp1AE);{int _tmp1B0=0;if(setjmp(_tmp1AE.handler))_tmp1B0=1;if(!_tmp1B0){
Cyc_Tcutil_unify_it(t1,t2);{
int _tmp1B1=1;_npop_handler(0U);return _tmp1B1;};
# 843
;_pop_handler();}else{void*_tmp1AF=(void*)Cyc_Core_get_exn_thrown();void*_tmp1B2=_tmp1AF;void*_tmp1B3;if(((struct Cyc_Tcutil_Unify_exn_struct*)_tmp1B2)->tag == Cyc_Tcutil_Unify){_LL1: _LL2:
# 845
 return 0;}else{_LL3: _tmp1B3=_tmp1B2;_LL4:(int)_rethrow(_tmp1B3);}_LL0:;}};}
# 850
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts);
static void Cyc_Tcutil_occurs(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,void*t){
t=Cyc_Tcutil_compress(t);{
void*_tmp1B4=t;struct Cyc_List_List*_tmp1D4;struct Cyc_List_List*_tmp1D3;struct Cyc_List_List*_tmp1D2;struct Cyc_List_List*_tmp1D1;struct Cyc_List_List*_tmp1D0;void*_tmp1CF;struct Cyc_Absyn_Tqual _tmp1CE;void*_tmp1CD;struct Cyc_List_List*_tmp1CC;int _tmp1CB;struct Cyc_Absyn_VarargInfo*_tmp1CA;struct Cyc_List_List*_tmp1C9;struct Cyc_List_List*_tmp1C8;struct Cyc_Absyn_Exp*_tmp1C7;struct Cyc_List_List*_tmp1C6;struct Cyc_Absyn_Exp*_tmp1C5;struct Cyc_List_List*_tmp1C4;void*_tmp1C3;void*_tmp1C2;struct Cyc_Absyn_PtrInfo _tmp1C1;void*_tmp1C0;struct Cyc_Core_Opt**_tmp1BF;struct Cyc_Absyn_Tvar*_tmp1BE;switch(*((int*)_tmp1B4)){case 2U: _LL1: _tmp1BE=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp1B4)->f1;_LL2:
# 855
 if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,_tmp1BE)){
Cyc_Tcutil_failure_reason=({const char*_tmp1B5="(type variable would escape scope)";_tag_fat(_tmp1B5,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 859
goto _LL0;case 1U: _LL3: _tmp1C0=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B4)->f2;_tmp1BF=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1B4)->f4;_LL4:
# 861
 if(t == evar){
Cyc_Tcutil_failure_reason=({const char*_tmp1B6="(occurs check)";_tag_fat(_tmp1B6,sizeof(char),15U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}else{
# 865
if(_tmp1C0 != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1C0);else{
# 868
int problem=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1BF))->v;for(0;s != 0;s=s->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd)){
problem=1;break;}}}
# 875
if(problem){
struct Cyc_List_List*_tmp1B7=0;
{struct Cyc_List_List*s=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(*_tmp1BF))->v;for(0;s != 0;s=s->tl){
if(((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,env,(struct Cyc_Absyn_Tvar*)s->hd))
_tmp1B7=({struct Cyc_List_List*_tmp1B8=_cycalloc(sizeof(*_tmp1B8));_tmp1B8->hd=(struct Cyc_Absyn_Tvar*)s->hd,_tmp1B8->tl=_tmp1B7;_tmp1B8;});}}
# 881
({struct Cyc_Core_Opt*_tmp809=({struct Cyc_Core_Opt*_tmp1B9=_cycalloc(sizeof(*_tmp1B9));_tmp1B9->v=_tmp1B7;_tmp1B9;});*_tmp1BF=_tmp809;});}}}
# 884
goto _LL0;case 3U: _LL5: _tmp1C1=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1B4)->f1;_LL6:
# 886
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C1.elt_type);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C1.ptr_atts).rgn);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C1.ptr_atts).nullable);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C1.ptr_atts).bounds);
Cyc_Tcutil_occurs(evar,r,env,(_tmp1C1.ptr_atts).zero_term);
goto _LL0;case 4U: _LL7: _tmp1C3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4)->f1).elt_type;_tmp1C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1B4)->f1).zero_term;_LL8:
# 894
 Cyc_Tcutil_occurs(evar,r,env,_tmp1C3);
Cyc_Tcutil_occurs(evar,r,env,_tmp1C2);
goto _LL0;case 5U: _LL9: _tmp1D0=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).tvars;_tmp1CF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).effect;_tmp1CE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).ret_tqual;_tmp1CD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).ret_type;_tmp1CC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).args;_tmp1CB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).c_varargs;_tmp1CA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).cyc_varargs;_tmp1C9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).rgn_po;_tmp1C8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).attributes;_tmp1C7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).requires_clause;_tmp1C6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).requires_relns;_tmp1C5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).ensures_clause;_tmp1C4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1B4)->f1).ensures_relns;_LLA:
# 899
 env=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rappend)(r,_tmp1D0,env);
if(_tmp1CF != 0)Cyc_Tcutil_occurs(evar,r,env,_tmp1CF);
Cyc_Tcutil_occurs(evar,r,env,_tmp1CD);
for(0;_tmp1CC != 0;_tmp1CC=_tmp1CC->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple9*)_tmp1CC->hd)).f3);}
if(_tmp1CA != 0)
Cyc_Tcutil_occurs(evar,r,env,_tmp1CA->type);
for(0;_tmp1C9 != 0;_tmp1C9=_tmp1C9->tl){
struct _tuple14*_tmp1BA=(struct _tuple14*)_tmp1C9->hd;struct _tuple14*_tmp1BB=_tmp1BA;void*_tmp1BD;void*_tmp1BC;_LL16: _tmp1BD=_tmp1BB->f1;_tmp1BC=_tmp1BB->f2;_LL17:;
Cyc_Tcutil_occurs(evar,r,env,_tmp1BD);
Cyc_Tcutil_occurs(evar,r,env,_tmp1BC);}
# 911
goto _LL0;case 6U: _LLB: _tmp1D1=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1B4)->f1;_LLC:
# 913
 for(0;_tmp1D1 != 0;_tmp1D1=_tmp1D1->tl){
Cyc_Tcutil_occurs(evar,r,env,(*((struct _tuple11*)_tmp1D1->hd)).f2);}
goto _LL0;case 7U: _LLD: _tmp1D2=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1B4)->f2;_LLE:
# 918
 for(0;_tmp1D2 != 0;_tmp1D2=_tmp1D2->tl){
Cyc_Tcutil_occurs(evar,r,env,((struct Cyc_Absyn_Aggrfield*)_tmp1D2->hd)->type);}
goto _LL0;case 8U: _LLF: _tmp1D3=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1B4)->f2;_LL10:
 _tmp1D4=_tmp1D3;goto _LL12;case 0U: _LL11: _tmp1D4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1B4)->f2;_LL12:
# 923
 Cyc_Tcutil_occurslist(evar,r,env,_tmp1D4);goto _LL0;default: _LL13: _LL14:
# 926
 goto _LL0;}_LL0:;};}
# 929
static void Cyc_Tcutil_occurslist(void*evar,struct _RegionHandle*r,struct Cyc_List_List*env,struct Cyc_List_List*ts){
# 931
for(0;ts != 0;ts=ts->tl){
Cyc_Tcutil_occurs(evar,r,env,(void*)ts->hd);}}
# 936
static void Cyc_Tcutil_unify_list(struct Cyc_List_List*t1,struct Cyc_List_List*t2){
for(0;t1 != 0 && t2 != 0;(t1=t1->tl,t2=t2->tl)){
Cyc_Tcutil_unify_it((void*)t1->hd,(void*)t2->hd);}
if(t1 != 0 || t2 != 0)
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 944
static void Cyc_Tcutil_unify_tqual(struct Cyc_Absyn_Tqual tq1,void*t1,struct Cyc_Absyn_Tqual tq2,void*t2){
if(tq1.print_const && !tq1.real_const)
({void*_tmp1D5=0U;({struct _fat_ptr _tmp80A=({const char*_tmp1D6="tq1 real_const not set.";_tag_fat(_tmp1D6,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp80A,_tag_fat(_tmp1D5,sizeof(void*),0U));});});
if(tq2.print_const && !tq2.real_const)
({void*_tmp1D7=0U;({struct _fat_ptr _tmp80B=({const char*_tmp1D8="tq2 real_const not set.";_tag_fat(_tmp1D8,sizeof(char),24U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp80B,_tag_fat(_tmp1D7,sizeof(void*),0U));});});
# 950
if((tq1.real_const != tq2.real_const || tq1.q_volatile != tq2.q_volatile)|| tq1.q_restrict != tq2.q_restrict){
# 953
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_tq1_const=tq1.real_const;
Cyc_Tcutil_tq2_const=tq2.real_const;
Cyc_Tcutil_failure_reason=({const char*_tmp1D9="(qualifiers don't match)";_tag_fat(_tmp1D9,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 961
Cyc_Tcutil_tq1_const=0;
Cyc_Tcutil_tq2_const=0;}
# 965
static void Cyc_Tcutil_unify_cmp_exp(struct Cyc_Absyn_Exp*e1,struct Cyc_Absyn_Exp*e2){
struct _tuple14 _tmp1DA=({struct _tuple14 _tmp725;_tmp725.f1=e1->r,_tmp725.f2=e2->r;_tmp725;});struct _tuple14 _tmp1DB=_tmp1DA;void*_tmp1E5;void*_tmp1E4;struct Cyc_Absyn_Exp*_tmp1E3;struct Cyc_Absyn_Exp*_tmp1E2;if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1DB.f1)->tag == 14U){_LL1: _tmp1E2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1DB.f1)->f2;_LL2:
# 970
 Cyc_Tcutil_unify_cmp_exp(_tmp1E2,e2);return;}else{if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1DB.f2)->tag == 14U){_LL3: _tmp1E3=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp1DB.f2)->f2;_LL4:
 Cyc_Tcutil_unify_cmp_exp(e1,_tmp1E3);return;}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1DB.f1)->tag == 39U){_LL5: _tmp1E4=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1DB.f1)->f1;_LL6: {
# 973
void*_tmp1DC=Cyc_Tcutil_compress(_tmp1E4);void*_tmp1DD=_tmp1DC;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1DD)->tag == 1U){_LLC: _LLD:
({void*_tmp80C=_tmp1E4;Cyc_Tcutil_unify_it(_tmp80C,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1DE=_cycalloc(sizeof(*_tmp1DE));_tmp1DE->tag=9U,_tmp1DE->f1=e2;_tmp1DE;}));});return;}else{_LLE: _LLF:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LLB:;}}else{if(((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1DB.f2)->tag == 39U){_LL7: _tmp1E5=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp1DB.f2)->f1;_LL8: {
# 978
void*_tmp1DF=Cyc_Tcutil_compress(_tmp1E5);void*_tmp1E0=_tmp1DF;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp1E0)->tag == 1U){_LL11: _LL12:
({void*_tmp80D=_tmp1E5;Cyc_Tcutil_unify_it(_tmp80D,(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E1=_cycalloc(sizeof(*_tmp1E1));_tmp1E1->tag=9U,_tmp1E1->f1=e1;_tmp1E1;}));});return;}else{_LL13: _LL14:
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}_LL10:;}}else{_LL9: _LLA:
# 982
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}}_LL0:;}
# 986
int Cyc_Tcutil_same_atts(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*a=a1;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a2))return 0;}}
{struct Cyc_List_List*a=a2;for(0;a != 0;a=a->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a->hd,a1))return 0;}}
return 1;}
# 995
static void*Cyc_Tcutil_rgns_of(void*t);
# 997
static void*Cyc_Tcutil_rgns_of_field(struct Cyc_Absyn_Aggrfield*af){
return Cyc_Tcutil_rgns_of(af->type);}
# 1001
static struct _tuple18*Cyc_Tcutil_region_free_subst(struct Cyc_Absyn_Tvar*tv){
void*t;
{struct Cyc_Absyn_Kind*_tmp1E6=Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp1E7=_tmp1E6;switch(((struct Cyc_Absyn_Kind*)_tmp1E7)->kind){case Cyc_Absyn_RgnKind: switch(((struct Cyc_Absyn_Kind*)_tmp1E7)->aliasqual){case Cyc_Absyn_Unique: _LL1: _LL2:
 t=Cyc_Absyn_unique_rgn_type;goto _LL0;case Cyc_Absyn_Aliasable: _LL3: _LL4:
 t=Cyc_Absyn_heap_rgn_type;goto _LL0;default: goto _LLD;}case Cyc_Absyn_EffKind: _LL5: _LL6:
 t=Cyc_Absyn_empty_effect;goto _LL0;case Cyc_Absyn_IntKind: _LL7: _LL8:
 t=(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp1E8=_cycalloc(sizeof(*_tmp1E8));_tmp1E8->tag=9U,({struct Cyc_Absyn_Exp*_tmp80E=Cyc_Absyn_uint_exp(0U,0U);_tmp1E8->f1=_tmp80E;});_tmp1E8;});goto _LL0;case Cyc_Absyn_BoolKind: _LL9: _LLA:
 t=Cyc_Absyn_true_type;goto _LL0;case Cyc_Absyn_PtrBndKind: _LLB: _LLC:
 t=Cyc_Absyn_fat_bound_type;goto _LL0;default: _LLD: _LLE:
 t=Cyc_Absyn_sint_type;goto _LL0;}_LL0:;}
# 1012
return({struct _tuple18*_tmp1E9=_cycalloc(sizeof(*_tmp1E9));_tmp1E9->f1=tv,_tmp1E9->f2=t;_tmp1E9;});}
# 1019
static void*Cyc_Tcutil_rgns_of(void*t){
void*_tmp1EA=Cyc_Tcutil_compress(t);void*_tmp1EB=_tmp1EA;struct Cyc_List_List*_tmp201;struct Cyc_List_List*_tmp200;struct Cyc_List_List*_tmp1FF;void*_tmp1FE;struct Cyc_Absyn_Tqual _tmp1FD;void*_tmp1FC;struct Cyc_List_List*_tmp1FB;struct Cyc_Absyn_VarargInfo*_tmp1FA;struct Cyc_List_List*_tmp1F9;struct Cyc_List_List*_tmp1F8;void*_tmp1F7;void*_tmp1F6;void*_tmp1F5;struct Cyc_List_List*_tmp1F4;switch(*((int*)_tmp1EB)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2 == 0){_LL1: _LL2:
 return Cyc_Absyn_empty_effect;}else{if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f1)->tag == 9U){_LL3: _LL4:
 return t;}else{_LL5: _tmp1F4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp1EB)->f2;_LL6: {
# 1024
struct Cyc_List_List*new_ts=((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp1F4);
return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(new_ts));}}}case 1U: _LL7: _LL8:
 goto _LLA;case 2U: _LL9: _LLA: {
# 1028
struct Cyc_Absyn_Kind*_tmp1EC=Cyc_Tcutil_type_kind(t);struct Cyc_Absyn_Kind*_tmp1ED=_tmp1EC;switch(((struct Cyc_Absyn_Kind*)_tmp1ED)->kind){case Cyc_Absyn_RgnKind: _LL1E: _LL1F:
 return Cyc_Absyn_access_eff(t);case Cyc_Absyn_EffKind: _LL20: _LL21:
 return t;case Cyc_Absyn_IntKind: _LL22: _LL23:
 return Cyc_Absyn_empty_effect;default: _LL24: _LL25:
 return Cyc_Absyn_regionsof_eff(t);}_LL1D:;}case 3U: _LLB: _tmp1F6=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EB)->f1).elt_type;_tmp1F5=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp1EB)->f1).ptr_atts).rgn;_LLC:
# 1036
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(({void*_tmp1EE[2U];({void*_tmp810=Cyc_Absyn_access_eff(_tmp1F5);_tmp1EE[0]=_tmp810;}),({void*_tmp80F=Cyc_Tcutil_rgns_of(_tmp1F6);_tmp1EE[1]=_tmp80F;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp1EE,sizeof(void*),2U));})));case 4U: _LLD: _tmp1F7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp1EB)->f1).elt_type;_LLE:
# 1038
 return Cyc_Tcutil_normalize_effect(Cyc_Tcutil_rgns_of(_tmp1F7));case 7U: _LLF: _tmp1F8=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp1EB)->f2;_LL10:
# 1040
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of_field,_tmp1F8)));case 5U: _LL11: _tmp1FF=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EB)->f1).tvars;_tmp1FE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EB)->f1).effect;_tmp1FD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EB)->f1).ret_tqual;_tmp1FC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EB)->f1).ret_type;_tmp1FB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EB)->f1).args;_tmp1FA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EB)->f1).cyc_varargs;_tmp1F9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp1EB)->f1).rgn_po;_LL12: {
# 1049
void*_tmp1EF=({struct Cyc_List_List*_tmp811=((struct Cyc_List_List*(*)(struct _tuple18*(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_region_free_subst,_tmp1FF);Cyc_Tcutil_substitute(_tmp811,(void*)_check_null(_tmp1FE));});
return Cyc_Tcutil_normalize_effect(_tmp1EF);}case 6U: _LL13: _tmp200=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp1EB)->f1;_LL14: {
# 1052
struct Cyc_List_List*_tmp1F0=0;
for(0;_tmp200 != 0;_tmp200=_tmp200->tl){
_tmp1F0=({struct Cyc_List_List*_tmp1F1=_cycalloc(sizeof(*_tmp1F1));_tmp1F1->hd=(*((struct _tuple11*)_tmp200->hd)).f2,_tmp1F1->tl=_tmp1F0;_tmp1F1;});}
_tmp201=_tmp1F0;goto _LL16;}case 8U: _LL15: _tmp201=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp1EB)->f2;_LL16:
# 1057
 return Cyc_Tcutil_normalize_effect(Cyc_Absyn_join_eff(((struct Cyc_List_List*(*)(void*(*f)(void*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_Tcutil_rgns_of,_tmp201)));case 10U: _LL17: _LL18:
({void*_tmp1F2=0U;({struct _fat_ptr _tmp812=({const char*_tmp1F3="typedecl in rgns_of";_tag_fat(_tmp1F3,sizeof(char),20U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp812,_tag_fat(_tmp1F2,sizeof(void*),0U));});});case 9U: _LL19: _LL1A:
 goto _LL1C;default: _LL1B: _LL1C:
 return Cyc_Absyn_empty_effect;}_LL0:;}
# 1067
void*Cyc_Tcutil_normalize_effect(void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp202=e;void*_tmp20E;struct Cyc_List_List**_tmp20D;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp202)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp202)->f1)){case 9U: _LL1: _tmp20D=(struct Cyc_List_List**)&((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp202)->f2;_LL2: {
# 1071
int redo_join=0;
{struct Cyc_List_List*effs=*_tmp20D;for(0;effs != 0;effs=effs->tl){
void*_tmp203=(void*)effs->hd;
({void*_tmp813=(void*)Cyc_Tcutil_compress(Cyc_Tcutil_normalize_effect(_tmp203));effs->hd=_tmp813;});{
void*_tmp204=(void*)effs->hd;void*_tmp205=_tmp204;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f1)){case 9U: _LL8: _LL9:
 goto _LLB;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f2)->tl == 0){_LLA: _LLB:
 goto _LLD;}else{goto _LL10;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f2)->tl == 0){_LLC: _LLD:
 goto _LLF;}else{goto _LL10;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp205)->f2)->tl == 0){_LLE: _LLF:
# 1080
 redo_join=1;goto _LL7;}else{goto _LL10;}default: goto _LL10;}else{goto _LL10;}}else{goto _LL10;}default: goto _LL10;}else{_LL10: _LL11:
 goto _LL7;}_LL7:;};}}
# 1084
if(!redo_join)return e;{
struct Cyc_List_List*effects=0;
{struct Cyc_List_List*effs=*_tmp20D;for(0;effs != 0;effs=effs->tl){
void*_tmp206=Cyc_Tcutil_compress((void*)effs->hd);void*_tmp207=_tmp206;void*_tmp20A;struct Cyc_List_List*_tmp209;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f1)){case 9U: _LL13: _tmp209=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2;_LL14:
# 1089
 effects=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_revappend)(_tmp209,effects);
goto _LL12;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2 != 0){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2)->hd)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2)->hd)->f1)){case 5U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2)->tl == 0){_LL15: _LL16:
 goto _LL18;}else{goto _LL1B;}case 7U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2)->tl == 0){_LL17: _LL18:
 goto _LL1A;}else{goto _LL1B;}case 6U: if(((struct Cyc_List_List*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp207)->f2)->tl == 0){_LL19: _LL1A:
 goto _LL12;}else{goto _LL1B;}default: goto _LL1B;}else{goto _LL1B;}}else{goto _LL1B;}default: goto _LL1B;}else{_LL1B: _tmp20A=_tmp207;_LL1C:
 effects=({struct Cyc_List_List*_tmp208=_cycalloc(sizeof(*_tmp208));_tmp208->hd=_tmp20A,_tmp208->tl=effects;_tmp208;});goto _LL12;}_LL12:;}}
# 1097
({struct Cyc_List_List*_tmp814=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(effects);*_tmp20D=_tmp814;});
return e;};}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp202)->f2 != 0){_LL3: _tmp20E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp202)->f2)->hd;_LL4: {
# 1100
void*_tmp20B=Cyc_Tcutil_compress(_tmp20E);void*_tmp20C=_tmp20B;switch(*((int*)_tmp20C)){case 1U: _LL1E: _LL1F:
 goto _LL21;case 2U: _LL20: _LL21:
 return e;default: _LL22: _LL23:
 return Cyc_Tcutil_rgns_of(_tmp20E);}_LL1D:;}}else{goto _LL5;}default: goto _LL5;}else{_LL5: _LL6:
# 1105
 return e;}_LL0:;};}
# 1110
static void*Cyc_Tcutil_dummy_fntype(void*eff){
struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp20F=({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp210=_cycalloc(sizeof(*_tmp210));_tmp210->tag=5U,(_tmp210->f1).tvars=0,(_tmp210->f1).effect=eff,({
struct Cyc_Absyn_Tqual _tmp815=Cyc_Absyn_empty_tqual(0U);(_tmp210->f1).ret_tqual=_tmp815;}),(_tmp210->f1).ret_type=Cyc_Absyn_void_type,(_tmp210->f1).args=0,(_tmp210->f1).c_varargs=0,(_tmp210->f1).cyc_varargs=0,(_tmp210->f1).rgn_po=0,(_tmp210->f1).attributes=0,(_tmp210->f1).requires_clause=0,(_tmp210->f1).requires_relns=0,(_tmp210->f1).ensures_clause=0,(_tmp210->f1).ensures_relns=0;_tmp210;});
# 1120
return({void*_tmp819=(void*)_tmp20F;void*_tmp818=Cyc_Absyn_heap_rgn_type;struct Cyc_Absyn_Tqual _tmp817=Cyc_Absyn_empty_tqual(0U);void*_tmp816=Cyc_Absyn_bounds_one();Cyc_Absyn_atb_type(_tmp819,_tmp818,_tmp817,_tmp816,Cyc_Absyn_false_type);});}
# 1127
int Cyc_Tcutil_region_in_effect(int constrain,void*r,void*e){
r=Cyc_Tcutil_compress(r);
if((r == Cyc_Absyn_heap_rgn_type || r == Cyc_Absyn_unique_rgn_type)|| r == Cyc_Absyn_refcnt_rgn_type)
return 1;{
void*_tmp211=Cyc_Tcutil_compress(e);void*_tmp212=_tmp211;struct Cyc_Core_Opt*_tmp22C;void**_tmp22B;struct Cyc_Core_Opt*_tmp22A;void*_tmp229;struct Cyc_List_List*_tmp228;void*_tmp227;switch(*((int*)_tmp212)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp212)->f1)){case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp212)->f2 != 0){_LL1: _tmp227=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp212)->f2)->hd;_LL2:
# 1134
 if(constrain)return Cyc_Tcutil_unify(r,_tmp227);
_tmp227=Cyc_Tcutil_compress(_tmp227);
if(r == _tmp227)return 1;{
struct _tuple14 _tmp213=({struct _tuple14 _tmp726;_tmp726.f1=r,_tmp726.f2=_tmp227;_tmp726;});struct _tuple14 _tmp214=_tmp213;struct Cyc_Absyn_Tvar*_tmp216;struct Cyc_Absyn_Tvar*_tmp215;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp214.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp214.f2)->tag == 2U){_LLC: _tmp216=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp214.f1)->f1;_tmp215=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp214.f2)->f1;_LLD:
 return Cyc_Absyn_tvar_cmp(_tmp216,_tmp215)== 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 0;}_LLB:;};}else{goto _LL9;}case 9U: _LL3: _tmp228=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp212)->f2;_LL4:
# 1142
 for(0;_tmp228 != 0;_tmp228=_tmp228->tl){
if(Cyc_Tcutil_region_in_effect(constrain,r,(void*)_tmp228->hd))return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp212)->f2 != 0){_LL5: _tmp229=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp212)->f2)->hd;_LL6: {
# 1146
void*_tmp217=Cyc_Tcutil_rgns_of(_tmp229);void*_tmp218=_tmp217;void*_tmp221;void*_tmp220;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->f2 != 0){_LL11: _tmp220=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp218)->f2)->hd;_LL12:
# 1148
 if(!constrain)return 0;{
void*_tmp219=Cyc_Tcutil_compress(_tmp220);void*_tmp21A=_tmp219;struct Cyc_Core_Opt*_tmp21F;void**_tmp21E;struct Cyc_Core_Opt*_tmp21D;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21A)->tag == 1U){_LL16: _tmp21F=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21A)->f1;_tmp21E=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21A)->f2;_tmp21D=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp21A)->f4;_LL17: {
# 1153
void*_tmp21B=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp21D);
# 1156
Cyc_Tcutil_occurs(_tmp21B,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp21D))->v,r);
({void*_tmp81B=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp21C[2U];_tmp21C[0]=_tmp21B,({void*_tmp81A=Cyc_Absyn_access_eff(r);_tmp21C[1]=_tmp81A;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp21C,sizeof(void*),2U));})));*_tmp21E=_tmp81B;});
return 1;}}else{_LL18: _LL19:
 return 0;}_LL15:;};}else{goto _LL13;}}else{goto _LL13;}}else{_LL13: _tmp221=_tmp218;_LL14:
# 1161
 return Cyc_Tcutil_region_in_effect(constrain,r,_tmp221);}_LL10:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp22C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp212)->f1;_tmp22B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp212)->f2;_tmp22A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp212)->f4;_LL8:
# 1164
 if(_tmp22C == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp22C->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp222=0U;({struct _fat_ptr _tmp81C=({const char*_tmp223="effect evar has wrong kind";_tag_fat(_tmp223,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp81C,_tag_fat(_tmp222,sizeof(void*),0U));});});
if(!constrain)return 0;{
# 1169
void*_tmp224=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp22A);
# 1172
Cyc_Tcutil_occurs(_tmp224,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp22A))->v,r);{
void*_tmp225=Cyc_Absyn_join_eff(({void*_tmp226[2U];_tmp226[0]=_tmp224,({void*_tmp81D=Cyc_Absyn_access_eff(r);_tmp226[1]=_tmp81D;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp226,sizeof(void*),2U));}));
*_tmp22B=_tmp225;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1183
static int Cyc_Tcutil_type_in_effect(int may_constrain_evars,void*t,void*e){
t=Cyc_Tcutil_compress(t);{
void*_tmp22D=Cyc_Tcutil_normalize_effect(Cyc_Tcutil_compress(e));void*_tmp22E=_tmp22D;struct Cyc_Core_Opt*_tmp240;void**_tmp23F;struct Cyc_Core_Opt*_tmp23E;void*_tmp23D;struct Cyc_List_List*_tmp23C;switch(*((int*)_tmp22E)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22E)->f1)){case 8U: _LL1: _LL2:
 return 0;case 9U: _LL3: _tmp23C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22E)->f2;_LL4:
# 1188
 for(0;_tmp23C != 0;_tmp23C=_tmp23C->tl){
if(Cyc_Tcutil_type_in_effect(may_constrain_evars,t,(void*)_tmp23C->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22E)->f2 != 0){_LL5: _tmp23D=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp22E)->f2)->hd;_LL6:
# 1193
 _tmp23D=Cyc_Tcutil_compress(_tmp23D);
if(t == _tmp23D)return 1;
if(may_constrain_evars)return Cyc_Tcutil_unify(t,_tmp23D);{
void*_tmp22F=Cyc_Tcutil_rgns_of(t);void*_tmp230=_tmp22F;void*_tmp236;void*_tmp235;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp230)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp230)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp230)->f2 != 0){_LLC: _tmp235=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp230)->f2)->hd;_LLD: {
# 1198
struct _tuple14 _tmp231=({struct _tuple14 _tmp727;({void*_tmp81E=Cyc_Tcutil_compress(_tmp235);_tmp727.f1=_tmp81E;}),_tmp727.f2=_tmp23D;_tmp727;});struct _tuple14 _tmp232=_tmp231;struct Cyc_Absyn_Tvar*_tmp234;struct Cyc_Absyn_Tvar*_tmp233;if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp232.f1)->tag == 2U){if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp232.f2)->tag == 2U){_LL11: _tmp234=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp232.f1)->f1;_tmp233=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp232.f2)->f1;_LL12:
 return Cyc_Tcutil_unify(t,_tmp23D);}else{goto _LL13;}}else{_LL13: _LL14:
 return _tmp235 == _tmp23D;}_LL10:;}}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp236=_tmp230;_LLF:
# 1202
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,t,_tmp236);}_LLB:;};}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp240=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f1;_tmp23F=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f2;_tmp23E=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp22E)->f4;_LL8:
# 1205
 if(_tmp240 == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp240->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp237=0U;({struct _fat_ptr _tmp81F=({const char*_tmp238="effect evar has wrong kind";_tag_fat(_tmp238,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp81F,_tag_fat(_tmp237,sizeof(void*),0U));});});
if(!may_constrain_evars)return 0;{
# 1210
void*_tmp239=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp23E);
# 1213
Cyc_Tcutil_occurs(_tmp239,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp23E))->v,t);{
void*_tmp23A=Cyc_Absyn_join_eff(({void*_tmp23B[2U];_tmp23B[0]=_tmp239,({void*_tmp820=Cyc_Absyn_regionsof_eff(t);_tmp23B[1]=_tmp820;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp23B,sizeof(void*),2U));}));
*_tmp23F=_tmp23A;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1224
static int Cyc_Tcutil_variable_in_effect(int may_constrain_evars,struct Cyc_Absyn_Tvar*v,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp241=e;struct Cyc_Core_Opt*_tmp257;void**_tmp256;struct Cyc_Core_Opt*_tmp255;void*_tmp254;struct Cyc_List_List*_tmp253;struct Cyc_Absyn_Tvar*_tmp252;switch(*((int*)_tmp241)){case 2U: _LL1: _tmp252=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp241)->f1;_LL2:
 return Cyc_Absyn_tvar_cmp(v,_tmp252)== 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f1)){case 9U: _LL3: _tmp253=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f2;_LL4:
# 1229
 for(0;_tmp253 != 0;_tmp253=_tmp253->tl){
if(Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,(void*)_tmp253->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f2 != 0){_LL5: _tmp254=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp241)->f2)->hd;_LL6: {
# 1234
void*_tmp242=Cyc_Tcutil_rgns_of(_tmp254);void*_tmp243=_tmp242;void*_tmp24C;void*_tmp24B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp243)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp243)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp243)->f2 != 0){_LLC: _tmp24B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp243)->f2)->hd;_LLD:
# 1236
 if(!may_constrain_evars)return 0;{
void*_tmp244=Cyc_Tcutil_compress(_tmp24B);void*_tmp245=_tmp244;struct Cyc_Core_Opt*_tmp24A;void**_tmp249;struct Cyc_Core_Opt*_tmp248;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp245)->tag == 1U){_LL11: _tmp24A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp245)->f1;_tmp249=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp245)->f2;_tmp248=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp245)->f4;_LL12: {
# 1242
void*_tmp246=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp248);
# 1244
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp248))->v,v))return 0;
({void*_tmp822=Cyc_Tcutil_dummy_fntype(Cyc_Absyn_join_eff(({void*_tmp247[2U];_tmp247[0]=_tmp246,({void*_tmp821=Cyc_Absyn_var_type(v);_tmp247[1]=_tmp821;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp247,sizeof(void*),2U));})));*_tmp249=_tmp822;});
return 1;}}else{_LL13: _LL14:
 return 0;}_LL10:;};}else{goto _LLE;}}else{goto _LLE;}}else{_LLE: _tmp24C=_tmp243;_LLF:
# 1249
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,v,_tmp24C);}_LLB:;}}else{goto _LL9;}default: goto _LL9;}case 1U: _LL7: _tmp257=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp241)->f1;_tmp256=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp241)->f2;_tmp255=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp241)->f4;_LL8:
# 1252
 if(_tmp257 == 0 ||(int)((struct Cyc_Absyn_Kind*)_tmp257->v)->kind != (int)Cyc_Absyn_EffKind)
({void*_tmp24D=0U;({struct _fat_ptr _tmp823=({const char*_tmp24E="effect evar has wrong kind";_tag_fat(_tmp24E,sizeof(char),27U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp823,_tag_fat(_tmp24D,sizeof(void*),0U));});});{
# 1256
void*_tmp24F=Cyc_Absyn_new_evar(& Cyc_Tcutil_eko,_tmp255);
# 1258
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp255))->v,v))
return 0;{
void*_tmp250=Cyc_Absyn_join_eff(({void*_tmp251[2U];_tmp251[0]=_tmp24F,({void*_tmp824=Cyc_Absyn_var_type(v);_tmp251[1]=_tmp824;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp251,sizeof(void*),2U));}));
*_tmp256=_tmp250;
return 1;};};default: _LL9: _LLA:
 return 0;}_LL0:;};}
# 1268
static int Cyc_Tcutil_evar_in_effect(void*evar,void*e){
e=Cyc_Tcutil_compress(e);{
void*_tmp258=e;void*_tmp25E;struct Cyc_List_List*_tmp25D;switch(*((int*)_tmp258)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp258)->f1)){case 9U: _LL1: _tmp25D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp258)->f2;_LL2:
# 1272
 for(0;_tmp25D != 0;_tmp25D=_tmp25D->tl){
if(Cyc_Tcutil_evar_in_effect(evar,(void*)_tmp25D->hd))
return 1;}
return 0;case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp258)->f2 != 0){_LL3: _tmp25E=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp258)->f2)->hd;_LL4: {
# 1277
void*_tmp259=Cyc_Tcutil_rgns_of(_tmp25E);void*_tmp25A=_tmp259;void*_tmp25C;void*_tmp25B;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25A)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25A)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25A)->f2 != 0){_LLA: _tmp25B=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp25A)->f2)->hd;_LLB:
 return 0;}else{goto _LLC;}}else{goto _LLC;}}else{_LLC: _tmp25C=_tmp25A;_LLD:
 return Cyc_Tcutil_evar_in_effect(evar,_tmp25C);}_LL9:;}}else{goto _LL7;}default: goto _LL7;}case 1U: _LL5: _LL6:
# 1281
 return evar == e;default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 1295 "tcutil.cyc"
int Cyc_Tcutil_subset_effect(int may_constrain_evars,void*e1,void*e2){
# 1300
void*_tmp25F=Cyc_Tcutil_compress(e1);void*_tmp260=_tmp25F;void**_tmp26D;struct Cyc_Core_Opt*_tmp26C;struct Cyc_Absyn_Tvar*_tmp26B;void*_tmp26A;void*_tmp269;struct Cyc_List_List*_tmp268;switch(*((int*)_tmp260)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp260)->f1)){case 9U: _LL1: _tmp268=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp260)->f2;_LL2:
# 1302
 for(0;_tmp268 != 0;_tmp268=_tmp268->tl){
if(!Cyc_Tcutil_subset_effect(may_constrain_evars,(void*)_tmp268->hd,e2))
return 0;}
return 1;case 8U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp260)->f2 != 0){_LL3: _tmp269=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp260)->f2)->hd;_LL4:
# 1313
 return Cyc_Tcutil_region_in_effect(may_constrain_evars,_tmp269,e2)||
 may_constrain_evars && Cyc_Tcutil_unify(_tmp269,Cyc_Absyn_heap_rgn_type);}else{goto _LLB;}case 10U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp260)->f2 != 0){_LL7: _tmp26A=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp260)->f2)->hd;_LL8: {
# 1317
void*_tmp261=Cyc_Tcutil_rgns_of(_tmp26A);void*_tmp262=_tmp261;void*_tmp264;void*_tmp263;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp262)->tag == 0U){if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp262)->f1)->tag == 10U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp262)->f2 != 0){_LLE: _tmp263=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp262)->f2)->hd;_LLF:
# 1322
 return Cyc_Tcutil_type_in_effect(may_constrain_evars,_tmp263,e2)||
 may_constrain_evars && Cyc_Tcutil_unify(_tmp263,Cyc_Absyn_sint_type);}else{goto _LL10;}}else{goto _LL10;}}else{_LL10: _tmp264=_tmp262;_LL11:
 return Cyc_Tcutil_subset_effect(may_constrain_evars,_tmp264,e2);}_LLD:;}}else{goto _LLB;}default: goto _LLB;}case 2U: _LL5: _tmp26B=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp260)->f1;_LL6:
# 1315
 return Cyc_Tcutil_variable_in_effect(may_constrain_evars,_tmp26B,e2);case 1U: _LL9: _tmp26D=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp260)->f2;_tmp26C=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp260)->f4;_LLA:
# 1327
 if(!Cyc_Tcutil_evar_in_effect(e1,e2)){
# 1331
*_tmp26D=Cyc_Absyn_empty_effect;
# 1334
return 1;}else{
# 1336
return 0;}default: _LLB: _LLC:
({struct Cyc_String_pa_PrintArg_struct _tmp267=({struct Cyc_String_pa_PrintArg_struct _tmp728;_tmp728.tag=0U,({struct _fat_ptr _tmp825=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(e1));_tmp728.f1=_tmp825;});_tmp728;});void*_tmp265[1U];_tmp265[0]=& _tmp267;({struct _fat_ptr _tmp826=({const char*_tmp266="subset_effect: bad effect: %s";_tag_fat(_tmp266,sizeof(char),30U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp826,_tag_fat(_tmp265,sizeof(void*),1U));});});}_LL0:;}
# 1352 "tcutil.cyc"
static int Cyc_Tcutil_unify_effect(void*e1,void*e2){
e1=Cyc_Tcutil_normalize_effect(e1);
e2=Cyc_Tcutil_normalize_effect(e2);
if(Cyc_Tcutil_subset_effect(0,e1,e2)&& Cyc_Tcutil_subset_effect(0,e2,e1))
return 1;
if(Cyc_Tcutil_subset_effect(1,e1,e2)&& Cyc_Tcutil_subset_effect(1,e2,e1))
return 1;
return 0;}
# 1368
static int Cyc_Tcutil_sub_rgnpo(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1370
{struct Cyc_List_List*r1=rpo1;for(0;r1 != 0;r1=r1->tl){
struct _tuple14*_tmp26E=(struct _tuple14*)r1->hd;struct _tuple14*_tmp26F=_tmp26E;void*_tmp275;void*_tmp274;_LL1: _tmp275=_tmp26F->f1;_tmp274=_tmp26F->f2;_LL2:;{
int found=_tmp275 == Cyc_Absyn_heap_rgn_type;
{struct Cyc_List_List*r2=rpo2;for(0;r2 != 0 && !found;r2=r2->tl){
struct _tuple14*_tmp270=(struct _tuple14*)r2->hd;struct _tuple14*_tmp271=_tmp270;void*_tmp273;void*_tmp272;_LL4: _tmp273=_tmp271->f1;_tmp272=_tmp271->f2;_LL5:;
if(Cyc_Tcutil_unify(_tmp275,_tmp273)&& Cyc_Tcutil_unify(_tmp274,_tmp272)){
found=1;
break;}}}
# 1380
if(!found)return 0;};}}
# 1382
return 1;}
# 1389
static int Cyc_Tcutil_check_logical_implication(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
for(0;r2 != 0;r2=r2->tl){
struct Cyc_Relations_Reln*_tmp276=Cyc_Relations_negate(Cyc_Core_heap_region,(struct Cyc_Relations_Reln*)r2->hd);
struct Cyc_List_List*_tmp277=({struct Cyc_List_List*_tmp278=_cycalloc(sizeof(*_tmp278));_tmp278->hd=_tmp276,_tmp278->tl=r1;_tmp278;});
if(Cyc_Relations_consistent_relations(_tmp277))return 0;}
# 1395
return 1;}
# 1400
static int Cyc_Tcutil_check_logical_equivalence(struct Cyc_List_List*r1,struct Cyc_List_List*r2){
if(r1 == r2)return 1;
return Cyc_Tcutil_check_logical_implication(r1,r2)&& Cyc_Tcutil_check_logical_implication(r2,r1);}
# 1406
static int Cyc_Tcutil_same_rgn_po(struct Cyc_List_List*rpo1,struct Cyc_List_List*rpo2){
# 1408
return Cyc_Tcutil_sub_rgnpo(rpo1,rpo2)&& Cyc_Tcutil_sub_rgnpo(rpo2,rpo1);}
# 1411
static int Cyc_Tcutil_tycon2int(void*t){
void*_tmp279=t;switch(*((int*)_tmp279)){case 0U: _LL1: _LL2:
 return 0;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp279)->f1){case Cyc_Absyn_Unsigned: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp279)->f2){case Cyc_Absyn_Char_sz: _LL3: _LL4:
 return 1;case Cyc_Absyn_Short_sz: _LL9: _LLA:
# 1417
 return 4;case Cyc_Absyn_Int_sz: _LLF: _LL10:
# 1420
 return 7;case Cyc_Absyn_Long_sz: _LL15: _LL16:
# 1423
 return 7;case Cyc_Absyn_LongLong_sz: _LL1B: _LL1C:
# 1426
 return 13;default: goto _LL4B;}case Cyc_Absyn_Signed: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp279)->f2){case Cyc_Absyn_Char_sz: _LL5: _LL6:
# 1415
 return 2;case Cyc_Absyn_Short_sz: _LLB: _LLC:
# 1418
 return 5;case Cyc_Absyn_Int_sz: _LL11: _LL12:
# 1421
 return 8;case Cyc_Absyn_Long_sz: _LL17: _LL18:
# 1424
 return 8;case Cyc_Absyn_LongLong_sz: _LL1D: _LL1E:
# 1427
 return 14;default: goto _LL4B;}case Cyc_Absyn_None: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp279)->f2){case Cyc_Absyn_Char_sz: _LL7: _LL8:
# 1416
 return 3;case Cyc_Absyn_Short_sz: _LLD: _LLE:
# 1419
 return 6;case Cyc_Absyn_Int_sz: _LL13: _LL14:
# 1422
 return 9;case Cyc_Absyn_Long_sz: _LL19: _LL1A:
# 1425
 return 9;case Cyc_Absyn_LongLong_sz: _LL1F: _LL20:
# 1428
 return 15;default: goto _LL4B;}default: _LL4B: _LL4C:
# 1450
({void*_tmp27A=0U;({struct _fat_ptr _tmp827=({const char*_tmp27B="bad con";_tag_fat(_tmp27B,sizeof(char),8U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp827,_tag_fat(_tmp27A,sizeof(void*),0U));});});}case 2U: switch(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp279)->f1){case 0U: _LL21: _LL22:
# 1429
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
# 1453
static int Cyc_Tcutil_type_case_number(void*t){
void*_tmp27C=t;void*_tmp27D;switch(*((int*)_tmp27C)){case 1U: _LL1: _LL2:
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
 return 11;default: _LL17: _tmp27D=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp27C)->f1;_LL18:
 return 12 + Cyc_Tcutil_tycon2int(_tmp27D);}_LL0:;}
# 1470
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2);
# 1472
struct _tuple1*Cyc_Tcutil_get_datatype_qvar(union Cyc_Absyn_DatatypeInfo i){
union Cyc_Absyn_DatatypeInfo _tmp27E=i;struct _tuple1*_tmp280;struct Cyc_Absyn_Datatypedecl*_tmp27F;if((_tmp27E.KnownDatatype).tag == 2){_LL1: _tmp27F=*(_tmp27E.KnownDatatype).val;_LL2:
 return _tmp27F->name;}else{_LL3: _tmp280=((_tmp27E.UnknownDatatype).val).name;_LL4:
 return _tmp280;}_LL0:;}struct _tuple19{struct _tuple1*f1;struct _tuple1*f2;};
# 1479
struct _tuple19 Cyc_Tcutil_get_datatype_field_qvars(union Cyc_Absyn_DatatypeFieldInfo i){
union Cyc_Absyn_DatatypeFieldInfo _tmp281=i;struct _tuple1*_tmp285;struct _tuple1*_tmp284;struct Cyc_Absyn_Datatypedecl*_tmp283;struct Cyc_Absyn_Datatypefield*_tmp282;if((_tmp281.KnownDatatypefield).tag == 2){_LL1: _tmp283=((_tmp281.KnownDatatypefield).val).f1;_tmp282=((_tmp281.KnownDatatypefield).val).f2;_LL2:
# 1482
 return({struct _tuple19 _tmp729;_tmp729.f1=_tmp283->name,_tmp729.f2=_tmp282->name;_tmp729;});}else{_LL3: _tmp285=((_tmp281.UnknownDatatypefield).val).datatype_name;_tmp284=((_tmp281.UnknownDatatypefield).val).field_name;_LL4:
# 1484
 return({struct _tuple19 _tmp72A;_tmp72A.f1=_tmp285,_tmp72A.f2=_tmp284;_tmp72A;});}_LL0:;}struct _tuple20{enum Cyc_Absyn_AggrKind f1;struct _tuple1*f2;};
# 1488
struct _tuple20 Cyc_Tcutil_get_aggr_kind_and_qvar(union Cyc_Absyn_AggrInfo i){
union Cyc_Absyn_AggrInfo _tmp286=i;struct Cyc_Absyn_Aggrdecl*_tmp289;enum Cyc_Absyn_AggrKind _tmp288;struct _tuple1*_tmp287;if((_tmp286.UnknownAggr).tag == 1){_LL1: _tmp288=((_tmp286.UnknownAggr).val).f1;_tmp287=((_tmp286.UnknownAggr).val).f2;_LL2:
 return({struct _tuple20 _tmp72B;_tmp72B.f1=_tmp288,_tmp72B.f2=_tmp287;_tmp72B;});}else{_LL3: _tmp289=*(_tmp286.KnownAggr).val;_LL4:
 return({struct _tuple20 _tmp72C;_tmp72C.f1=_tmp289->kind,_tmp72C.f2=_tmp289->name;_tmp72C;});}_LL0:;}
# 1495
int Cyc_Tcutil_tycon_cmp(void*t1,void*t2){
if(t1 == t2)return 0;{
int i1=Cyc_Tcutil_tycon2int(t1);
int i2=Cyc_Tcutil_tycon2int(t2);
if(i1 != i2)return i1 - i2;{
# 1501
struct _tuple14 _tmp28A=({struct _tuple14 _tmp72D;_tmp72D.f1=t1,_tmp72D.f2=t2;_tmp72D;});struct _tuple14 _tmp28B=_tmp28A;union Cyc_Absyn_AggrInfo _tmp2A9;union Cyc_Absyn_AggrInfo _tmp2A8;union Cyc_Absyn_DatatypeFieldInfo _tmp2A7;union Cyc_Absyn_DatatypeFieldInfo _tmp2A6;union Cyc_Absyn_DatatypeInfo _tmp2A5;union Cyc_Absyn_DatatypeInfo _tmp2A4;struct Cyc_List_List*_tmp2A3;struct Cyc_List_List*_tmp2A2;struct _fat_ptr _tmp2A1;struct _fat_ptr _tmp2A0;struct _tuple1*_tmp29F;struct _tuple1*_tmp29E;switch(*((int*)_tmp28B.f1)){case 15U: if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp28B.f2)->tag == 15U){_LL1: _tmp29F=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp28B.f1)->f1;_tmp29E=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1;_LL2:
 return Cyc_Absyn_qvar_cmp(_tmp29F,_tmp29E);}else{goto _LLD;}case 17U: if(((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp28B.f2)->tag == 17U){_LL3: _tmp2A1=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp28B.f1)->f1;_tmp2A0=((struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1;_LL4:
 return Cyc_strcmp((struct _fat_ptr)_tmp2A1,(struct _fat_ptr)_tmp2A0);}else{goto _LLD;}case 16U: if(((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp28B.f2)->tag == 16U){_LL5: _tmp2A3=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp28B.f1)->f1;_tmp2A2=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1;_LL6:
# 1505
 return((int(*)(int(*cmp)(struct Cyc_Absyn_Enumfield*,struct Cyc_Absyn_Enumfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_enumfield_cmp,_tmp2A3,_tmp2A2);}else{goto _LLD;}case 18U: if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp28B.f2)->tag == 18U){_LL7: _tmp2A5=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp28B.f1)->f1;_tmp2A4=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1;_LL8: {
# 1507
struct _tuple1*q1=Cyc_Tcutil_get_datatype_qvar(_tmp2A5);
struct _tuple1*q2=Cyc_Tcutil_get_datatype_qvar(_tmp2A4);
return Cyc_Absyn_qvar_cmp(q1,q2);}}else{goto _LLD;}case 19U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp28B.f2)->tag == 19U){_LL9: _tmp2A7=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp28B.f1)->f1;_tmp2A6=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1;_LLA: {
# 1511
struct _tuple19 _tmp28C=Cyc_Tcutil_get_datatype_field_qvars(_tmp2A7);struct _tuple19 _tmp28D=_tmp28C;struct _tuple1*_tmp294;struct _tuple1*_tmp293;_LL10: _tmp294=_tmp28D.f1;_tmp293=_tmp28D.f2;_LL11:;{
struct _tuple19 _tmp28E=Cyc_Tcutil_get_datatype_field_qvars(_tmp2A6);struct _tuple19 _tmp28F=_tmp28E;struct _tuple1*_tmp292;struct _tuple1*_tmp291;_LL13: _tmp292=_tmp28F.f1;_tmp291=_tmp28F.f2;_LL14:;{
int _tmp290=Cyc_Absyn_qvar_cmp(_tmp294,_tmp292);
if(_tmp290 != 0)return _tmp290;
return Cyc_Absyn_qvar_cmp(_tmp293,_tmp291);};};}}else{goto _LLD;}case 20U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp28B.f2)->tag == 20U){_LLB: _tmp2A9=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp28B.f1)->f1;_tmp2A8=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp28B.f2)->f1;_LLC: {
# 1517
struct _tuple20 _tmp295=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2A9);struct _tuple20 _tmp296=_tmp295;enum Cyc_Absyn_AggrKind _tmp29D;struct _tuple1*_tmp29C;_LL16: _tmp29D=_tmp296.f1;_tmp29C=_tmp296.f2;_LL17:;{
struct _tuple20 _tmp297=Cyc_Tcutil_get_aggr_kind_and_qvar(_tmp2A8);struct _tuple20 _tmp298=_tmp297;enum Cyc_Absyn_AggrKind _tmp29B;struct _tuple1*_tmp29A;_LL19: _tmp29B=_tmp298.f1;_tmp29A=_tmp298.f2;_LL1A:;{
int _tmp299=Cyc_Absyn_qvar_cmp(_tmp29C,_tmp29A);
if(_tmp299 != 0)return _tmp299;
return(int)_tmp29D - (int)_tmp29B;};};}}else{goto _LLD;}default: _LLD: _LLE:
 return 0;}_LL0:;};};}struct _tuple21{struct Cyc_Absyn_VarargInfo*f1;struct Cyc_Absyn_VarargInfo*f2;};
# 1527
static void Cyc_Tcutil_unify_it(void*t1,void*t2){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=_tag_fat(0,0,0);
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return;
{void*_tmp2AA=t1;struct Cyc_Core_Opt*_tmp2B8;void**_tmp2B7;struct Cyc_Core_Opt*_tmp2B6;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AA)->tag == 1U){_LL1: _tmp2B8=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AA)->f1;_tmp2B7=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AA)->f2;_tmp2B6=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AA)->f4;_LL2:
# 1538
 Cyc_Tcutil_occurs(t1,Cyc_Core_heap_region,(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B6))->v,t2);{
struct Cyc_Absyn_Kind*_tmp2AB=Cyc_Tcutil_type_kind(t2);
# 1543
if(Cyc_Tcutil_kind_leq(_tmp2AB,(struct Cyc_Absyn_Kind*)((struct Cyc_Core_Opt*)_check_null(_tmp2B8))->v)){
*_tmp2B7=t2;
return;}else{
# 1547
{void*_tmp2AC=t2;struct Cyc_Absyn_PtrInfo _tmp2B4;void**_tmp2B3;struct Cyc_Core_Opt*_tmp2B2;switch(*((int*)_tmp2AC)){case 1U: _LL6: _tmp2B3=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AC)->f2;_tmp2B2=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2AC)->f4;_LL7: {
# 1550
struct Cyc_List_List*_tmp2AD=(struct Cyc_List_List*)_tmp2B6->v;
{struct Cyc_List_List*s2=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp2B2))->v;for(0;s2 != 0;s2=s2->tl){
if(!((int(*)(int(*compare)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_mem)(Cyc_Tcutil_fast_tvar_cmp,_tmp2AD,(struct Cyc_Absyn_Tvar*)s2->hd)){
Cyc_Tcutil_failure_reason=({const char*_tmp2AE="(type variable would escape scope)";_tag_fat(_tmp2AE,sizeof(char),35U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}}
# 1557
if(Cyc_Tcutil_kind_leq((struct Cyc_Absyn_Kind*)_tmp2B8->v,_tmp2AB)){
*_tmp2B3=t1;return;}
# 1560
Cyc_Tcutil_failure_reason=({const char*_tmp2AF="(kinds are incompatible)";_tag_fat(_tmp2AF,sizeof(char),25U);});
goto _LL5;}case 3U: _LL8: _tmp2B4=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2AC)->f1;if((int)((struct Cyc_Absyn_Kind*)_tmp2B8->v)->kind == (int)Cyc_Absyn_BoxKind){_LL9: {
# 1566
void*_tmp2B0=Cyc_Tcutil_compress((_tmp2B4.ptr_atts).bounds);
{void*_tmp2B1=_tmp2B0;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp2B1)->tag == 1U){_LLD: _LLE:
# 1569
({void*_tmp828=_tmp2B0;Cyc_Tcutil_unify(_tmp828,Cyc_Absyn_bounds_one());});
*_tmp2B7=t2;
return;}else{_LLF: _LL10:
 goto _LLC;}_LLC:;}
# 1574
goto _LL5;}}else{goto _LLA;}default: _LLA: _LLB:
 goto _LL5;}_LL5:;}
# 1577
Cyc_Tcutil_failure_reason=({const char*_tmp2B5="(kinds are incompatible)";_tag_fat(_tmp2B5,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}};}else{_LL3: _LL4:
# 1580
 goto _LL0;}_LL0:;}
# 1585
{struct _tuple14 _tmp2B9=({struct _tuple14 _tmp733;_tmp733.f1=t2,_tmp733.f2=t1;_tmp733;});struct _tuple14 _tmp2BA=_tmp2B9;struct Cyc_List_List*_tmp348;struct Cyc_Absyn_Typedefdecl*_tmp347;struct Cyc_List_List*_tmp346;struct Cyc_Absyn_Typedefdecl*_tmp345;enum Cyc_Absyn_AggrKind _tmp344;struct Cyc_List_List*_tmp343;enum Cyc_Absyn_AggrKind _tmp342;struct Cyc_List_List*_tmp341;struct Cyc_List_List*_tmp340;struct Cyc_List_List*_tmp33F;struct Cyc_List_List*_tmp33E;void*_tmp33D;struct Cyc_Absyn_Tqual _tmp33C;void*_tmp33B;struct Cyc_List_List*_tmp33A;int _tmp339;struct Cyc_Absyn_VarargInfo*_tmp338;struct Cyc_List_List*_tmp337;struct Cyc_List_List*_tmp336;struct Cyc_Absyn_Exp*_tmp335;struct Cyc_List_List*_tmp334;struct Cyc_Absyn_Exp*_tmp333;struct Cyc_List_List*_tmp332;struct Cyc_List_List*_tmp331;void*_tmp330;struct Cyc_Absyn_Tqual _tmp32F;void*_tmp32E;struct Cyc_List_List*_tmp32D;int _tmp32C;struct Cyc_Absyn_VarargInfo*_tmp32B;struct Cyc_List_List*_tmp32A;struct Cyc_List_List*_tmp329;struct Cyc_Absyn_Exp*_tmp328;struct Cyc_List_List*_tmp327;struct Cyc_Absyn_Exp*_tmp326;struct Cyc_List_List*_tmp325;void*_tmp324;struct Cyc_Absyn_Tqual _tmp323;struct Cyc_Absyn_Exp*_tmp322;void*_tmp321;void*_tmp320;struct Cyc_Absyn_Tqual _tmp31F;struct Cyc_Absyn_Exp*_tmp31E;void*_tmp31D;struct Cyc_Absyn_Exp*_tmp31C;struct Cyc_Absyn_Exp*_tmp31B;void*_tmp31A;struct Cyc_Absyn_Tqual _tmp319;void*_tmp318;void*_tmp317;void*_tmp316;void*_tmp315;void*_tmp314;struct Cyc_Absyn_Tqual _tmp313;void*_tmp312;void*_tmp311;void*_tmp310;void*_tmp30F;struct Cyc_Absyn_Tvar*_tmp30E;struct Cyc_Absyn_Tvar*_tmp30D;void*_tmp30C;struct Cyc_List_List*_tmp30B;void*_tmp30A;struct Cyc_List_List*_tmp309;switch(*((int*)_tmp2BA.f1)){case 1U: _LL12: _LL13:
# 1588
 Cyc_Tcutil_unify_it(t2,t1);return;case 0U: if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f1)->f1)->tag == 9U){_LL14: _LL15:
# 1590
 goto _LL17;}else{if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 0U){if(((struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f2)->f1)->tag == 9U)goto _LL16;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f1)->f1)->tag == 8U)goto _LL18;else{if(((struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f2)->f1)->tag == 8U)goto _LL1A;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f1)->f1)->tag == 10U)goto _LL1C;else{if(((struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f2)->f1)->tag == 10U)goto _LL1E;else{_LL20: _tmp30C=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f1)->f1;_tmp30B=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f1)->f2;_tmp30A=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f2)->f1;_tmp309=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f2)->f2;_LL21:
# 1601
 if(Cyc_Tcutil_tycon_cmp(_tmp30C,_tmp30A)== 0){
Cyc_Tcutil_unify_list(_tmp30B,_tmp309);
return;}else{
# 1605
Cyc_Tcutil_failure_reason=({const char*_tmp2BC="(different type constructors)";_tag_fat(_tmp2BC,sizeof(char),30U);});}
goto _LL11;}}}}}}else{switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f1)->f1)){case 8U: _LL18: _LL19:
# 1592
 goto _LL1B;case 10U: _LL1C: _LL1D:
# 1594
 goto _LL1F;default: goto _LL32;}}}default: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2BA.f2)->f1)){case 9U: _LL16: _LL17:
# 1591
 goto _LL19;case 8U: _LL1A: _LL1B:
# 1593
 goto _LL1D;case 10U: _LL1E: _LL1F:
# 1596
 if(Cyc_Tcutil_unify_effect(t1,t2))return;
Cyc_Tcutil_failure_reason=({const char*_tmp2BB="(effects don't unify)";_tag_fat(_tmp2BB,sizeof(char),22U);});
goto _LL11;default: switch(*((int*)_tmp2BA.f1)){case 2U: goto _LL32;case 3U: goto _LL32;case 9U: goto _LL32;case 4U: goto _LL32;case 5U: goto _LL32;case 6U: goto _LL32;case 7U: goto _LL32;case 8U: goto _LL32;default: goto _LL32;}}else{switch(*((int*)_tmp2BA.f1)){case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 2U){_LL22: _tmp30E=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BA.f1)->f1;_tmp30D=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp2BA.f2)->f1;_LL23: {
# 1609
struct _fat_ptr*_tmp2BD=_tmp30E->name;
struct _fat_ptr*_tmp2BE=_tmp30D->name;
# 1612
int _tmp2BF=_tmp30E->identity;
int _tmp2C0=_tmp30D->identity;
if(_tmp2C0 == _tmp2BF)return;
Cyc_Tcutil_failure_reason=({const char*_tmp2C1="(variable types are not the same)";_tag_fat(_tmp2C1,sizeof(char),34U);});
goto _LL11;}}else{goto _LL32;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 3U){_LL24: _tmp31A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f1)->f1).elt_type;_tmp319=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f1)->f1).elt_tq;_tmp318=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f1)->f1).ptr_atts).rgn;_tmp317=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f1)->f1).ptr_atts).nullable;_tmp316=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f1)->f1).ptr_atts).bounds;_tmp315=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f1)->f1).ptr_atts).zero_term;_tmp314=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f2)->f1).elt_type;_tmp313=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f2)->f1).elt_tq;_tmp312=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f2)->f1).ptr_atts).rgn;_tmp311=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f2)->f1).ptr_atts).nullable;_tmp310=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f2)->f1).ptr_atts).bounds;_tmp30F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp2BA.f2)->f1).ptr_atts).zero_term;_LL25:
# 1620
 Cyc_Tcutil_unify_it(_tmp314,_tmp31A);
Cyc_Tcutil_unify_it(_tmp318,_tmp312);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
struct _fat_ptr _tmp2C2=Cyc_Tcutil_failure_reason;
if(!Cyc_Tcutil_unify(_tmp30F,_tmp315)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2C3="(not both zero terminated)";_tag_fat(_tmp2C3,sizeof(char),27U);});
goto _LL11;}
# 1630
Cyc_Tcutil_unify_tqual(_tmp313,_tmp314,_tmp319,_tmp31A);
if(!Cyc_Tcutil_unify(_tmp310,_tmp316)){
Cyc_Tcutil_t1_failure=t1;Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2C4="(different pointer bounds)";_tag_fat(_tmp2C4,sizeof(char),27U);});
goto _LL11;}{
# 1637
void*_tmp2C5=Cyc_Tcutil_compress(_tmp310);void*_tmp2C6=_tmp2C5;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C6)->tag == 0U){if(((struct Cyc_Absyn_FatCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp2C6)->f1)->tag == 14U){_LL35: _LL36:
# 1639
 Cyc_Tcutil_failure_reason=_tmp2C2;
return;}else{goto _LL37;}}else{_LL37: _LL38:
# 1642
 Cyc_Tcutil_failure_reason=({const char*_tmp2C7="(incompatible pointer types)";_tag_fat(_tmp2C7,sizeof(char),29U);});
Cyc_Tcutil_unify_it(_tmp311,_tmp317);
return;}_LL34:;};};}else{goto _LL32;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 9U){_LL26: _tmp31C=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2BA.f1)->f1;_tmp31B=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp2BA.f2)->f1;_LL27:
# 1648
 if(!Cyc_Evexp_same_const_exp(_tmp31C,_tmp31B)){
Cyc_Tcutil_failure_reason=({const char*_tmp2C8="(cannot prove expressions are the same)";_tag_fat(_tmp2C8,sizeof(char),40U);});
goto _LL11;}
# 1652
return;}else{goto _LL32;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 4U){_LL28: _tmp324=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f1)->f1).elt_type;_tmp323=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f1)->f1).tq;_tmp322=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f1)->f1).num_elts;_tmp321=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f1)->f1).zero_term;_tmp320=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f2)->f1).elt_type;_tmp31F=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f2)->f1).tq;_tmp31E=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f2)->f1).num_elts;_tmp31D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp2BA.f2)->f1).zero_term;_LL29:
# 1656
 Cyc_Tcutil_unify_it(_tmp320,_tmp324);
Cyc_Tcutil_unify_tqual(_tmp31F,_tmp320,_tmp323,_tmp324);
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2C9="(not both zero terminated)";_tag_fat(_tmp2C9,sizeof(char),27U);});
Cyc_Tcutil_unify_it(_tmp321,_tmp31D);
if(_tmp322 == _tmp31E)return;
if(_tmp322 == 0 || _tmp31E == 0)goto _LL11;
if(Cyc_Evexp_same_const_exp(_tmp322,_tmp31E))
return;
Cyc_Tcutil_failure_reason=({const char*_tmp2CA="(different array sizes)";_tag_fat(_tmp2CA,sizeof(char),24U);});
goto _LL11;}else{goto _LL32;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 5U){_LL2A: _tmp33E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).tvars;_tmp33D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).effect;_tmp33C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).ret_tqual;_tmp33B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).ret_type;_tmp33A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).args;_tmp339=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).c_varargs;_tmp338=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).cyc_varargs;_tmp337=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).rgn_po;_tmp336=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).attributes;_tmp335=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).requires_clause;_tmp334=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).requires_relns;_tmp333=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).ensures_clause;_tmp332=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f1)->f1).ensures_relns;_tmp331=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).tvars;_tmp330=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).effect;_tmp32F=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).ret_tqual;_tmp32E=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).ret_type;_tmp32D=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).args;_tmp32C=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).c_varargs;_tmp32B=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).cyc_varargs;_tmp32A=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).rgn_po;_tmp329=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).attributes;_tmp328=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).requires_clause;_tmp327=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).requires_relns;_tmp326=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).ensures_clause;_tmp325=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp2BA.f2)->f1).ensures_relns;_LL2B: {
# 1671
int done=0;
{struct _RegionHandle _tmp2CB=_new_region("rgn");struct _RegionHandle*rgn=& _tmp2CB;_push_region(rgn);
{struct Cyc_List_List*inst=0;
while(_tmp331 != 0){
if(_tmp33E == 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2CC="(second function type has too few type variables)";_tag_fat(_tmp2CC,sizeof(char),50U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}{
# 1679
void*_tmp2CD=((struct Cyc_Absyn_Tvar*)_tmp331->hd)->kind;
void*_tmp2CE=((struct Cyc_Absyn_Tvar*)_tmp33E->hd)->kind;
if(!Cyc_Tcutil_unify_kindbound(_tmp2CD,_tmp2CE)){
Cyc_Tcutil_failure_reason=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp2D1=({struct Cyc_String_pa_PrintArg_struct _tmp730;_tmp730.tag=0U,({
struct _fat_ptr _tmp829=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_tvar2string((struct Cyc_Absyn_Tvar*)_tmp331->hd));_tmp730.f1=_tmp829;});_tmp730;});struct Cyc_String_pa_PrintArg_struct _tmp2D2=({struct Cyc_String_pa_PrintArg_struct _tmp72F;_tmp72F.tag=0U,({
struct _fat_ptr _tmp82A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp331->hd,& Cyc_Tcutil_bk)));_tmp72F.f1=_tmp82A;});_tmp72F;});struct Cyc_String_pa_PrintArg_struct _tmp2D3=({struct Cyc_String_pa_PrintArg_struct _tmp72E;_tmp72E.tag=0U,({
struct _fat_ptr _tmp82B=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(Cyc_Tcutil_tvar_kind((struct Cyc_Absyn_Tvar*)_tmp33E->hd,& Cyc_Tcutil_bk)));_tmp72E.f1=_tmp82B;});_tmp72E;});void*_tmp2CF[3U];_tmp2CF[0]=& _tmp2D1,_tmp2CF[1]=& _tmp2D2,_tmp2CF[2]=& _tmp2D3;({struct _fat_ptr _tmp82C=({const char*_tmp2D0="(type var %s has different kinds %s and %s)";_tag_fat(_tmp2D0,sizeof(char),44U);});Cyc_aprintf(_tmp82C,_tag_fat(_tmp2CF,sizeof(void*),3U));});});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1688
inst=({struct Cyc_List_List*_tmp2D5=_region_malloc(rgn,sizeof(*_tmp2D5));({struct _tuple18*_tmp82E=({struct _tuple18*_tmp2D4=_region_malloc(rgn,sizeof(*_tmp2D4));_tmp2D4->f1=(struct Cyc_Absyn_Tvar*)_tmp33E->hd,({void*_tmp82D=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp331->hd);_tmp2D4->f2=_tmp82D;});_tmp2D4;});_tmp2D5->hd=_tmp82E;}),_tmp2D5->tl=inst;_tmp2D5;});
_tmp331=_tmp331->tl;
_tmp33E=_tmp33E->tl;};}
# 1692
if(_tmp33E != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2D6="(second function type has too many type variables)";_tag_fat(_tmp2D6,sizeof(char),51U);});
_npop_handler(0U);goto _LL11;}
# 1696
if(inst != 0){
({void*_tmp831=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2D7=_cycalloc(sizeof(*_tmp2D7));_tmp2D7->tag=5U,(_tmp2D7->f1).tvars=0,(_tmp2D7->f1).effect=_tmp330,(_tmp2D7->f1).ret_tqual=_tmp32F,(_tmp2D7->f1).ret_type=_tmp32E,(_tmp2D7->f1).args=_tmp32D,(_tmp2D7->f1).c_varargs=_tmp32C,(_tmp2D7->f1).cyc_varargs=_tmp32B,(_tmp2D7->f1).rgn_po=_tmp32A,(_tmp2D7->f1).attributes=_tmp329,(_tmp2D7->f1).requires_clause=_tmp335,(_tmp2D7->f1).requires_relns=_tmp334,(_tmp2D7->f1).ensures_clause=_tmp333,(_tmp2D7->f1).ensures_relns=_tmp332;_tmp2D7;});Cyc_Tcutil_unify_it(_tmp831,({
# 1700
struct _RegionHandle*_tmp830=rgn;struct Cyc_List_List*_tmp82F=inst;Cyc_Tcutil_rsubstitute(_tmp830,_tmp82F,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp2D8=_cycalloc(sizeof(*_tmp2D8));
_tmp2D8->tag=5U,(_tmp2D8->f1).tvars=0,(_tmp2D8->f1).effect=_tmp33D,(_tmp2D8->f1).ret_tqual=_tmp33C,(_tmp2D8->f1).ret_type=_tmp33B,(_tmp2D8->f1).args=_tmp33A,(_tmp2D8->f1).c_varargs=_tmp339,(_tmp2D8->f1).cyc_varargs=_tmp338,(_tmp2D8->f1).rgn_po=_tmp337,(_tmp2D8->f1).attributes=_tmp336,(_tmp2D8->f1).requires_clause=_tmp328,(_tmp2D8->f1).requires_relns=_tmp327,(_tmp2D8->f1).ensures_clause=_tmp326,(_tmp2D8->f1).ensures_relns=_tmp325;_tmp2D8;}));}));});
# 1705
done=1;}}
# 1673
;_pop_region(rgn);}
# 1708
if(done)
return;
Cyc_Tcutil_unify_it(_tmp32E,_tmp33B);
Cyc_Tcutil_unify_tqual(_tmp32F,_tmp32E,_tmp33C,_tmp33B);
for(0;_tmp32D != 0 && _tmp33A != 0;(_tmp32D=_tmp32D->tl,_tmp33A=_tmp33A->tl)){
struct _tuple9 _tmp2D9=*((struct _tuple9*)_tmp32D->hd);struct _tuple9 _tmp2DA=_tmp2D9;struct Cyc_Absyn_Tqual _tmp2E0;void*_tmp2DF;_LL3A: _tmp2E0=_tmp2DA.f2;_tmp2DF=_tmp2DA.f3;_LL3B:;{
struct _tuple9 _tmp2DB=*((struct _tuple9*)_tmp33A->hd);struct _tuple9 _tmp2DC=_tmp2DB;struct Cyc_Absyn_Tqual _tmp2DE;void*_tmp2DD;_LL3D: _tmp2DE=_tmp2DC.f2;_tmp2DD=_tmp2DC.f3;_LL3E:;
Cyc_Tcutil_unify_it(_tmp2DF,_tmp2DD);
Cyc_Tcutil_unify_tqual(_tmp2E0,_tmp2DF,_tmp2DE,_tmp2DD);};}
# 1718
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(_tmp32D != 0 || _tmp33A != 0){
Cyc_Tcutil_failure_reason=({const char*_tmp2E1="(function types have different number of arguments)";_tag_fat(_tmp2E1,sizeof(char),52U);});
goto _LL11;}
# 1724
if(_tmp32C != _tmp339){
Cyc_Tcutil_failure_reason=({const char*_tmp2E2="(only one function type takes C varargs)";_tag_fat(_tmp2E2,sizeof(char),41U);});
goto _LL11;}{
# 1729
int bad_cyc_vararg=0;
{struct _tuple21 _tmp2E3=({struct _tuple21 _tmp731;_tmp731.f1=_tmp32B,_tmp731.f2=_tmp338;_tmp731;});struct _tuple21 _tmp2E4=_tmp2E3;struct _fat_ptr*_tmp2EE;struct Cyc_Absyn_Tqual _tmp2ED;void*_tmp2EC;int _tmp2EB;struct _fat_ptr*_tmp2EA;struct Cyc_Absyn_Tqual _tmp2E9;void*_tmp2E8;int _tmp2E7;if(_tmp2E4.f1 == 0){if(_tmp2E4.f2 == 0){_LL40: _LL41:
 goto _LL3F;}else{_LL42: _LL43:
 goto _LL45;}}else{if(_tmp2E4.f2 == 0){_LL44: _LL45:
# 1734
 bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp2E5="(only one function type takes varargs)";_tag_fat(_tmp2E5,sizeof(char),39U);});
goto _LL3F;}else{_LL46: _tmp2EE=(_tmp2E4.f1)->name;_tmp2ED=(_tmp2E4.f1)->tq;_tmp2EC=(_tmp2E4.f1)->type;_tmp2EB=(_tmp2E4.f1)->inject;_tmp2EA=(_tmp2E4.f2)->name;_tmp2E9=(_tmp2E4.f2)->tq;_tmp2E8=(_tmp2E4.f2)->type;_tmp2E7=(_tmp2E4.f2)->inject;_LL47:
# 1738
 Cyc_Tcutil_unify_it(_tmp2EC,_tmp2E8);
Cyc_Tcutil_unify_tqual(_tmp2ED,_tmp2EC,_tmp2E9,_tmp2E8);
if(_tmp2EB != _tmp2E7){
bad_cyc_vararg=1;
Cyc_Tcutil_failure_reason=({const char*_tmp2E6="(only one function type injects varargs)";_tag_fat(_tmp2E6,sizeof(char),41U);});}
# 1744
goto _LL3F;}}_LL3F:;}
# 1746
if(bad_cyc_vararg)goto _LL11;{
# 1749
int bad_effect=0;
{struct _tuple14 _tmp2EF=({struct _tuple14 _tmp732;_tmp732.f1=_tmp330,_tmp732.f2=_tmp33D;_tmp732;});struct _tuple14 _tmp2F0=_tmp2EF;if(_tmp2F0.f1 == 0){if(_tmp2F0.f2 == 0){_LL49: _LL4A:
 goto _LL48;}else{_LL4B: _LL4C:
 goto _LL4E;}}else{if(_tmp2F0.f2 == 0){_LL4D: _LL4E:
 bad_effect=1;goto _LL48;}else{_LL4F: _LL50:
 bad_effect=!({void*_tmp832=(void*)_check_null(_tmp330);Cyc_Tcutil_unify_effect(_tmp832,(void*)_check_null(_tmp33D));});goto _LL48;}}_LL48:;}
# 1756
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
if(bad_effect){
Cyc_Tcutil_failure_reason=({const char*_tmp2F1="(function effects do not match)";_tag_fat(_tmp2F1,sizeof(char),32U);});
goto _LL11;}
# 1762
if(!Cyc_Tcutil_same_atts(_tmp336,_tmp329)){
Cyc_Tcutil_failure_reason=({const char*_tmp2F2="(function types have different attributes)";_tag_fat(_tmp2F2,sizeof(char),43U);});
goto _LL11;}
# 1766
if(!Cyc_Tcutil_same_rgn_po(_tmp337,_tmp32A)){
Cyc_Tcutil_failure_reason=({const char*_tmp2F3="(function types have different region lifetime orderings)";_tag_fat(_tmp2F3,sizeof(char),58U);});
goto _LL11;}
# 1770
if(!Cyc_Tcutil_check_logical_equivalence(_tmp334,_tmp327)){
Cyc_Tcutil_failure_reason=({const char*_tmp2F4="(@requires clauses not equivalent)";_tag_fat(_tmp2F4,sizeof(char),35U);});
goto _LL11;}
# 1774
if(!Cyc_Tcutil_check_logical_equivalence(_tmp332,_tmp325)){
Cyc_Tcutil_failure_reason=({const char*_tmp2F5="(@ensures clauses not equivalent)";_tag_fat(_tmp2F5,sizeof(char),34U);});
goto _LL11;}
# 1778
return;};};}}else{goto _LL32;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 6U){_LL2C: _tmp340=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2BA.f1)->f1;_tmp33F=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp2BA.f2)->f1;_LL2D:
# 1781
 for(0;_tmp33F != 0 && _tmp340 != 0;(_tmp33F=_tmp33F->tl,_tmp340=_tmp340->tl)){
struct _tuple11 _tmp2F6=*((struct _tuple11*)_tmp33F->hd);struct _tuple11 _tmp2F7=_tmp2F6;struct Cyc_Absyn_Tqual _tmp2FD;void*_tmp2FC;_LL52: _tmp2FD=_tmp2F7.f1;_tmp2FC=_tmp2F7.f2;_LL53:;{
struct _tuple11 _tmp2F8=*((struct _tuple11*)_tmp340->hd);struct _tuple11 _tmp2F9=_tmp2F8;struct Cyc_Absyn_Tqual _tmp2FB;void*_tmp2FA;_LL55: _tmp2FB=_tmp2F9.f1;_tmp2FA=_tmp2F9.f2;_LL56:;
Cyc_Tcutil_unify_it(_tmp2FC,_tmp2FA);
Cyc_Tcutil_unify_tqual(_tmp2FD,_tmp2FC,_tmp2FB,_tmp2FA);};}
# 1787
if(_tmp33F == 0 && _tmp340 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp2FE="(tuple types have different numbers of components)";_tag_fat(_tmp2FE,sizeof(char),51U);});
goto _LL11;}else{goto _LL32;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 7U){_LL2E: _tmp344=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2BA.f1)->f1;_tmp343=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2BA.f1)->f2;_tmp342=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2BA.f2)->f1;_tmp341=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp2BA.f2)->f2;_LL2F:
# 1794
 if((int)_tmp342 != (int)_tmp344){Cyc_Tcutil_failure_reason=({const char*_tmp2FF="(struct and union type)";_tag_fat(_tmp2FF,sizeof(char),24U);});goto _LL11;}
for(0;_tmp341 != 0 && _tmp343 != 0;(_tmp341=_tmp341->tl,_tmp343=_tmp343->tl)){
struct Cyc_Absyn_Aggrfield*_tmp300=(struct Cyc_Absyn_Aggrfield*)_tmp341->hd;
struct Cyc_Absyn_Aggrfield*_tmp301=(struct Cyc_Absyn_Aggrfield*)_tmp343->hd;
if(Cyc_strptrcmp(_tmp300->name,_tmp301->name)!= 0){
Cyc_Tcutil_failure_reason=({const char*_tmp302="(different member names)";_tag_fat(_tmp302,sizeof(char),25U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1802
Cyc_Tcutil_unify_it(_tmp300->type,_tmp301->type);
Cyc_Tcutil_unify_tqual(_tmp300->tq,_tmp300->type,_tmp301->tq,_tmp301->type);
if(!Cyc_Tcutil_same_atts(_tmp300->attributes,_tmp301->attributes)){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp303="(different attributes on member)";_tag_fat(_tmp303,sizeof(char),33U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1810
if((_tmp300->width != 0 && _tmp301->width == 0 ||
 _tmp301->width != 0 && _tmp300->width == 0)||
(_tmp300->width != 0 && _tmp301->width != 0)&& !({
struct Cyc_Absyn_Exp*_tmp833=(struct Cyc_Absyn_Exp*)_check_null(_tmp300->width);Cyc_Evexp_same_const_exp(_tmp833,(struct Cyc_Absyn_Exp*)_check_null(_tmp301->width));})){
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp304="(different bitfield widths on member)";_tag_fat(_tmp304,sizeof(char),38U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1819
if((_tmp300->requires_clause != 0 && _tmp301->requires_clause == 0 ||
 _tmp300->requires_clause == 0 && _tmp301->requires_clause != 0)||
(_tmp300->requires_clause == 0 && _tmp301->requires_clause != 0)&& !({
struct Cyc_Absyn_Exp*_tmp834=(struct Cyc_Absyn_Exp*)_check_null(_tmp300->requires_clause);Cyc_Evexp_same_const_exp(_tmp834,(struct Cyc_Absyn_Exp*)_check_null(_tmp301->requires_clause));})){
# 1824
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp305="(different @requires clauses on member)";_tag_fat(_tmp305,sizeof(char),40U);});
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}}
# 1830
if(_tmp341 == 0 && _tmp343 == 0)return;
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;
Cyc_Tcutil_failure_reason=({const char*_tmp306="(different number of members)";_tag_fat(_tmp306,sizeof(char),30U);});
goto _LL11;}else{goto _LL32;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2BA.f2)->tag == 8U){_LL30: _tmp348=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2BA.f1)->f2;_tmp347=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2BA.f1)->f3;_tmp346=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2BA.f2)->f2;_tmp345=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp2BA.f2)->f3;_LL31:
# 1836
 if(_tmp347 != _tmp345){
Cyc_Tcutil_failure_reason=({const char*_tmp307="(different abstract typedefs)";_tag_fat(_tmp307,sizeof(char),30U);});
goto _LL11;}
# 1840
Cyc_Tcutil_failure_reason=({const char*_tmp308="(type parameters to typedef differ)";_tag_fat(_tmp308,sizeof(char),36U);});
Cyc_Tcutil_unify_list(_tmp348,_tmp346);
return;}else{goto _LL32;}default: _LL32: _LL33:
 goto _LL11;}}}_LL11:;}
# 1845
(int)_throw((void*)& Cyc_Tcutil_Unify_val);}
# 1848
static int Cyc_Tcutil_star_cmp(int(*cmp)(void*,void*),void*a1,void*a2){
if(a1 == a2)return 0;
if(a1 == 0 && a2 != 0)return - 1;
if(a1 != 0 && a2 == 0)return 1;
return({int(*_tmp836)(void*,void*)=cmp;void*_tmp835=(void*)_check_null(a1);_tmp836(_tmp835,(void*)_check_null(a2));});}
# 1854
static int Cyc_Tcutil_tqual_cmp(struct Cyc_Absyn_Tqual tq1,struct Cyc_Absyn_Tqual tq2){
int _tmp349=(tq1.real_const + (tq1.q_volatile << 1))+ (tq1.q_restrict << 2);
int _tmp34A=(tq2.real_const + (tq2.q_volatile << 1))+ (tq2.q_restrict << 2);
return Cyc_Core_intcmp(_tmp349,_tmp34A);}
# 1859
static int Cyc_Tcutil_tqual_type_cmp(struct _tuple11*tqt1,struct _tuple11*tqt2){
struct _tuple11*_tmp34B=tqt1;struct Cyc_Absyn_Tqual _tmp351;void*_tmp350;_LL1: _tmp351=_tmp34B->f1;_tmp350=_tmp34B->f2;_LL2:;{
struct _tuple11*_tmp34C=tqt2;struct Cyc_Absyn_Tqual _tmp34F;void*_tmp34E;_LL4: _tmp34F=_tmp34C->f1;_tmp34E=_tmp34C->f2;_LL5:;{
int _tmp34D=Cyc_Tcutil_tqual_cmp(_tmp351,_tmp34F);
if(_tmp34D != 0)return _tmp34D;
return Cyc_Tcutil_typecmp(_tmp350,_tmp34E);};};}
# 1866
static int Cyc_Tcutil_enumfield_cmp(struct Cyc_Absyn_Enumfield*e1,struct Cyc_Absyn_Enumfield*e2){
int _tmp352=Cyc_Absyn_qvar_cmp(e1->name,e2->name);
if(_tmp352 != 0)return _tmp352;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,e1->tag,e2->tag);}
# 1872
int Cyc_Tcutil_aggrfield_cmp(struct Cyc_Absyn_Aggrfield*f1,struct Cyc_Absyn_Aggrfield*f2){
int _tmp353=Cyc_strptrcmp(f1->name,f2->name);
if(_tmp353 != 0)return _tmp353;{
int _tmp354=Cyc_Tcutil_tqual_cmp(f1->tq,f2->tq);
if(_tmp354 != 0)return _tmp354;{
int _tmp355=Cyc_Tcutil_typecmp(f1->type,f2->type);
if(_tmp355 != 0)return _tmp355;{
int _tmp356=((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Absyn_attribute_cmp,f1->attributes,f2->attributes);
if(_tmp356 != 0)return _tmp356;
_tmp356=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->width,f2->width);
if(_tmp356 != 0)return _tmp356;
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,f1->requires_clause,f2->requires_clause);};};};}
# 1889
int Cyc_Tcutil_typecmp(void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
if(t1 == t2)return 0;{
int _tmp357=({int _tmp837=Cyc_Tcutil_type_case_number(t1);Cyc_Core_intcmp(_tmp837,Cyc_Tcutil_type_case_number(t2));});
if(_tmp357 != 0)
return _tmp357;{
# 1898
struct _tuple14 _tmp358=({struct _tuple14 _tmp734;_tmp734.f1=t2,_tmp734.f2=t1;_tmp734;});struct _tuple14 _tmp359=_tmp358;struct Cyc_Absyn_Exp*_tmp3A5;struct Cyc_Absyn_Exp*_tmp3A4;struct Cyc_Absyn_Exp*_tmp3A3;struct Cyc_Absyn_Exp*_tmp3A2;enum Cyc_Absyn_AggrKind _tmp3A1;struct Cyc_List_List*_tmp3A0;enum Cyc_Absyn_AggrKind _tmp39F;struct Cyc_List_List*_tmp39E;struct Cyc_List_List*_tmp39D;struct Cyc_List_List*_tmp39C;struct Cyc_Absyn_FnInfo _tmp39B;struct Cyc_Absyn_FnInfo _tmp39A;void*_tmp399;struct Cyc_Absyn_Tqual _tmp398;struct Cyc_Absyn_Exp*_tmp397;void*_tmp396;void*_tmp395;struct Cyc_Absyn_Tqual _tmp394;struct Cyc_Absyn_Exp*_tmp393;void*_tmp392;void*_tmp391;struct Cyc_Absyn_Tqual _tmp390;void*_tmp38F;void*_tmp38E;void*_tmp38D;void*_tmp38C;void*_tmp38B;struct Cyc_Absyn_Tqual _tmp38A;void*_tmp389;void*_tmp388;void*_tmp387;void*_tmp386;struct Cyc_Absyn_Tvar*_tmp385;struct Cyc_Absyn_Tvar*_tmp384;void*_tmp383;struct Cyc_List_List*_tmp382;void*_tmp381;struct Cyc_List_List*_tmp380;switch(*((int*)_tmp359.f1)){case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp359.f2)->tag == 0U){_LL1: _tmp383=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp359.f1)->f1;_tmp382=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp359.f1)->f2;_tmp381=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp359.f2)->f1;_tmp380=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp359.f2)->f2;_LL2: {
# 1900
int _tmp35A=Cyc_Tcutil_tycon_cmp(_tmp383,_tmp381);
if(_tmp35A != 0)return _tmp35A;
return((int(*)(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_typecmp,_tmp382,_tmp380);}}else{goto _LL15;}case 1U: if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp359.f2)->tag == 1U){_LL3: _LL4:
# 1904
({void*_tmp35B=0U;({struct _fat_ptr _tmp838=({const char*_tmp35C="typecmp: can only compare closed types";_tag_fat(_tmp35C,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp838,_tag_fat(_tmp35B,sizeof(void*),0U));});});}else{goto _LL15;}case 2U: if(((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp359.f2)->tag == 2U){_LL5: _tmp385=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp359.f1)->f1;_tmp384=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp359.f2)->f1;_LL6:
# 1908
 return Cyc_Core_intcmp(_tmp384->identity,_tmp385->identity);}else{goto _LL15;}case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f2)->tag == 3U){_LL7: _tmp391=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f1)->f1).elt_type;_tmp390=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f1)->f1).elt_tq;_tmp38F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f1)->f1).ptr_atts).rgn;_tmp38E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f1)->f1).ptr_atts).nullable;_tmp38D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f1)->f1).ptr_atts).bounds;_tmp38C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f1)->f1).ptr_atts).zero_term;_tmp38B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f2)->f1).elt_type;_tmp38A=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f2)->f1).elt_tq;_tmp389=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f2)->f1).ptr_atts).rgn;_tmp388=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f2)->f1).ptr_atts).nullable;_tmp387=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f2)->f1).ptr_atts).bounds;_tmp386=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp359.f2)->f1).ptr_atts).zero_term;_LL8: {
# 1912
int _tmp35D=Cyc_Tcutil_typecmp(_tmp38B,_tmp391);
if(_tmp35D != 0)return _tmp35D;{
int _tmp35E=Cyc_Tcutil_typecmp(_tmp389,_tmp38F);
if(_tmp35E != 0)return _tmp35E;{
int _tmp35F=Cyc_Tcutil_tqual_cmp(_tmp38A,_tmp390);
if(_tmp35F != 0)return _tmp35F;{
int _tmp360=Cyc_Tcutil_typecmp(_tmp387,_tmp38D);
if(_tmp360 != 0)return _tmp360;{
int _tmp361=Cyc_Tcutil_typecmp(_tmp386,_tmp38C);
if(_tmp361 != 0)return _tmp361;{
int _tmp362=Cyc_Tcutil_typecmp(_tmp387,_tmp38D);
if(_tmp362 != 0)return _tmp362;
return Cyc_Tcutil_typecmp(_tmp388,_tmp38E);};};};};};}}else{goto _LL15;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f2)->tag == 4U){_LL9: _tmp399=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f1)->f1).elt_type;_tmp398=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f1)->f1).tq;_tmp397=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f1)->f1).num_elts;_tmp396=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f1)->f1).zero_term;_tmp395=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f2)->f1).elt_type;_tmp394=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f2)->f1).tq;_tmp393=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f2)->f1).num_elts;_tmp392=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp359.f2)->f1).zero_term;_LLA: {
# 1928
int _tmp363=Cyc_Tcutil_tqual_cmp(_tmp394,_tmp398);
if(_tmp363 != 0)return _tmp363;{
int _tmp364=Cyc_Tcutil_typecmp(_tmp395,_tmp399);
if(_tmp364 != 0)return _tmp364;{
int _tmp365=Cyc_Tcutil_typecmp(_tmp396,_tmp392);
if(_tmp365 != 0)return _tmp365;
if(_tmp397 == _tmp393)return 0;
if(_tmp397 == 0 || _tmp393 == 0)
({void*_tmp366=0U;({struct _fat_ptr _tmp839=({const char*_tmp367="missing expression in array index";_tag_fat(_tmp367,sizeof(char),34U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp839,_tag_fat(_tmp366,sizeof(void*),0U));});});
# 1938
return((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp397,_tmp393);};};}}else{goto _LL15;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp359.f2)->tag == 5U){_LLB: _tmp39B=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp359.f1)->f1;_tmp39A=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp359.f2)->f1;_LLC:
# 1941
 if(Cyc_Tcutil_unify(t1,t2))return 0;{
int r=Cyc_Tcutil_typecmp(_tmp39B.ret_type,_tmp39A.ret_type);
if(r != 0)return r;
r=Cyc_Tcutil_tqual_cmp(_tmp39B.ret_tqual,_tmp39A.ret_tqual);
if(r != 0)return r;{
struct Cyc_List_List*_tmp368=_tmp39B.args;
struct Cyc_List_List*_tmp369=_tmp39A.args;
for(0;_tmp368 != 0 && _tmp369 != 0;(_tmp368=_tmp368->tl,_tmp369=_tmp369->tl)){
struct _tuple9 _tmp36A=*((struct _tuple9*)_tmp368->hd);struct _tuple9 _tmp36B=_tmp36A;struct Cyc_Absyn_Tqual _tmp371;void*_tmp370;_LL18: _tmp371=_tmp36B.f2;_tmp370=_tmp36B.f3;_LL19:;{
struct _tuple9 _tmp36C=*((struct _tuple9*)_tmp369->hd);struct _tuple9 _tmp36D=_tmp36C;struct Cyc_Absyn_Tqual _tmp36F;void*_tmp36E;_LL1B: _tmp36F=_tmp36D.f2;_tmp36E=_tmp36D.f3;_LL1C:;
r=Cyc_Tcutil_tqual_cmp(_tmp371,_tmp36F);
if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp370,_tmp36E);
if(r != 0)return r;};}
# 1956
if(_tmp368 != 0)return 1;
if(_tmp369 != 0)return - 1;
if(_tmp39B.c_varargs && !_tmp39A.c_varargs)return 1;
if(!_tmp39B.c_varargs && _tmp39A.c_varargs)return - 1;
if(_tmp39B.cyc_varargs != 0 & _tmp39A.cyc_varargs == 0)return 1;
if(_tmp39B.cyc_varargs == 0 & _tmp39A.cyc_varargs != 0)return - 1;
if(_tmp39B.cyc_varargs != 0 & _tmp39A.cyc_varargs != 0){
r=({struct Cyc_Absyn_Tqual _tmp83A=((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp39B.cyc_varargs))->tq;Cyc_Tcutil_tqual_cmp(_tmp83A,((struct Cyc_Absyn_VarargInfo*)_check_null(_tmp39A.cyc_varargs))->tq);});
if(r != 0)return r;
r=Cyc_Tcutil_typecmp((_tmp39B.cyc_varargs)->type,(_tmp39A.cyc_varargs)->type);
if(r != 0)return r;
if((_tmp39B.cyc_varargs)->inject && !(_tmp39A.cyc_varargs)->inject)return 1;
if(!(_tmp39B.cyc_varargs)->inject &&(_tmp39A.cyc_varargs)->inject)return - 1;}
# 1970
r=Cyc_Tcutil_star_cmp(Cyc_Tcutil_typecmp,_tmp39B.effect,_tmp39A.effect);
if(r != 0)return r;{
struct Cyc_List_List*_tmp372=_tmp39B.rgn_po;
struct Cyc_List_List*_tmp373=_tmp39A.rgn_po;
for(0;_tmp372 != 0 && _tmp373 != 0;(_tmp372=_tmp372->tl,_tmp373=_tmp373->tl)){
struct _tuple14 _tmp374=*((struct _tuple14*)_tmp372->hd);struct _tuple14 _tmp375=_tmp374;void*_tmp37B;void*_tmp37A;_LL1E: _tmp37B=_tmp375.f1;_tmp37A=_tmp375.f2;_LL1F:;{
struct _tuple14 _tmp376=*((struct _tuple14*)_tmp373->hd);struct _tuple14 _tmp377=_tmp376;void*_tmp379;void*_tmp378;_LL21: _tmp379=_tmp377.f1;_tmp378=_tmp377.f2;_LL22:;
r=Cyc_Tcutil_typecmp(_tmp37B,_tmp379);if(r != 0)return r;
r=Cyc_Tcutil_typecmp(_tmp37A,_tmp378);if(r != 0)return r;};}
# 1980
if(_tmp372 != 0)return 1;
if(_tmp373 != 0)return - 1;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp39B.requires_clause,_tmp39A.requires_clause);
if(r != 0)return r;
r=((int(*)(int(*cmp)(struct Cyc_Absyn_Exp*,struct Cyc_Absyn_Exp*),struct Cyc_Absyn_Exp*a1,struct Cyc_Absyn_Exp*a2))Cyc_Tcutil_star_cmp)(Cyc_Evexp_const_exp_cmp,_tmp39B.ensures_clause,_tmp39A.ensures_clause);
if(r != 0)return r;
# 1988
({void*_tmp37C=0U;({struct _fat_ptr _tmp83B=({const char*_tmp37D="typecmp: function type comparison should never get here!";_tag_fat(_tmp37D,sizeof(char),57U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp83B,_tag_fat(_tmp37C,sizeof(void*),0U));});});};};};}else{goto _LL15;}case 6U: if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp359.f2)->tag == 6U){_LLD: _tmp39D=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp359.f1)->f1;_tmp39C=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp359.f2)->f1;_LLE:
# 1991
 return((int(*)(int(*cmp)(struct _tuple11*,struct _tuple11*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_tqual_type_cmp,_tmp39C,_tmp39D);}else{goto _LL15;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp359.f2)->tag == 7U){_LLF: _tmp3A1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp359.f1)->f1;_tmp3A0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp359.f1)->f2;_tmp39F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp359.f2)->f1;_tmp39E=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp359.f2)->f2;_LL10:
# 1994
 if((int)_tmp39F != (int)_tmp3A1){
if((int)_tmp39F == (int)0U)return - 1;else{
return 1;}}
return((int(*)(int(*cmp)(struct Cyc_Absyn_Aggrfield*,struct Cyc_Absyn_Aggrfield*),struct Cyc_List_List*l1,struct Cyc_List_List*l2))Cyc_List_list_cmp)(Cyc_Tcutil_aggrfield_cmp,_tmp39E,_tmp3A0);}else{goto _LL15;}case 9U: if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp359.f2)->tag == 9U){_LL11: _tmp3A3=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp359.f1)->f1;_tmp3A2=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp359.f2)->f1;_LL12:
# 1999
 _tmp3A5=_tmp3A3;_tmp3A4=_tmp3A2;goto _LL14;}else{goto _LL15;}case 11U: if(((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp359.f2)->tag == 11U){_LL13: _tmp3A5=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp359.f1)->f1;_tmp3A4=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp359.f2)->f1;_LL14:
# 2001
 return Cyc_Evexp_const_exp_cmp(_tmp3A5,_tmp3A4);}else{goto _LL15;}default: _LL15: _LL16:
({void*_tmp37E=0U;({struct _fat_ptr _tmp83C=({const char*_tmp37F="Unmatched case in typecmp";_tag_fat(_tmp37F,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp83C,_tag_fat(_tmp37E,sizeof(void*),0U));});});}_LL0:;};};}
# 2008
static int Cyc_Tcutil_will_lose_precision(void*t1,void*t2){
struct _tuple14 _tmp3A6=({struct _tuple14 _tmp736;({void*_tmp83E=Cyc_Tcutil_compress(t1);_tmp736.f1=_tmp83E;}),({void*_tmp83D=Cyc_Tcutil_compress(t2);_tmp736.f2=_tmp83D;});_tmp736;});struct _tuple14 _tmp3A7=_tmp3A6;void*_tmp3AD;void*_tmp3AC;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7.f2)->tag == 0U){_LL1: _tmp3AD=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7.f1)->f1;_tmp3AC=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3A7.f2)->f1;_LL2: {
# 2011
struct _tuple14 _tmp3A8=({struct _tuple14 _tmp735;_tmp735.f1=_tmp3AD,_tmp735.f2=_tmp3AC;_tmp735;});struct _tuple14 _tmp3A9=_tmp3A8;int _tmp3AB;int _tmp3AA;switch(*((int*)_tmp3A9.f1)){case 2U: switch(*((int*)_tmp3A9.f2)){case 2U: _LL6: _tmp3AB=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3A9.f1)->f1;_tmp3AA=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3A9.f2)->f1;_LL7:
 return _tmp3AA < _tmp3AB;case 1U: _LL8: _LL9:
 goto _LLB;case 4U: _LLA: _LLB:
 return 1;default: goto _LL26;}case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f1)->f2){case Cyc_Absyn_LongLong_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f2)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 return 0;}else{goto _LLE;}}else{_LLE: _LLF:
 return 1;}case Cyc_Absyn_Long_sz: switch(*((int*)_tmp3A9.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f2)->f2){case Cyc_Absyn_Int_sz: _LL10: _LL11:
# 2019
 goto _LL13;case Cyc_Absyn_Short_sz: _LL18: _LL19:
# 2024
 goto _LL1B;case Cyc_Absyn_Char_sz: _LL1E: _LL1F:
# 2027
 goto _LL21;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3A9.f2)->f1 == 0){_LL14: _LL15:
# 2022
 goto _LL17;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Int_sz: switch(*((int*)_tmp3A9.f2)){case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f2)->f2){case Cyc_Absyn_Long_sz: _LL12: _LL13:
# 2020
 return 0;case Cyc_Absyn_Short_sz: _LL1A: _LL1B:
# 2025
 goto _LL1D;case Cyc_Absyn_Char_sz: _LL20: _LL21:
# 2028
 goto _LL23;default: goto _LL26;}case 2U: if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3A9.f2)->f1 == 0){_LL16: _LL17:
# 2023
 goto _LL19;}else{goto _LL26;}default: goto _LL26;}case Cyc_Absyn_Short_sz: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f2)->f2 == Cyc_Absyn_Char_sz){_LL22: _LL23:
# 2029
 goto _LL25;}else{goto _LL26;}}else{goto _LL26;}default: goto _LL26;}case 4U: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f2)->tag == 1U)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3A9.f2)->f2){case Cyc_Absyn_Short_sz: _LL1C: _LL1D:
# 2026
 goto _LL1F;case Cyc_Absyn_Char_sz: _LL24: _LL25:
# 2030
 return 1;default: goto _LL26;}else{goto _LL26;}default: _LL26: _LL27:
# 2032
 return 0;}_LL5:;}}else{goto _LL3;}}else{_LL3: _LL4:
# 2034
 return 0;}_LL0:;}
# 2038
void*Cyc_Tcutil_max_arithmetic_type(void*t1,void*t2){
{struct _tuple14 _tmp3AE=({struct _tuple14 _tmp738;({void*_tmp840=Cyc_Tcutil_compress(t1);_tmp738.f1=_tmp840;}),({void*_tmp83F=Cyc_Tcutil_compress(t2);_tmp738.f2=_tmp83F;});_tmp738;});struct _tuple14 _tmp3AF=_tmp3AE;void*_tmp3B5;void*_tmp3B4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AF.f1)->tag == 0U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AF.f2)->tag == 0U){_LL1: _tmp3B5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AF.f1)->f1;_tmp3B4=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3AF.f2)->f1;_LL2:
# 2041
{struct _tuple14 _tmp3B0=({struct _tuple14 _tmp737;_tmp737.f1=_tmp3B5,_tmp737.f2=_tmp3B4;_tmp737;});struct _tuple14 _tmp3B1=_tmp3B0;int _tmp3B3;int _tmp3B2;if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B1.f1)->tag == 2U){if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B1.f2)->tag == 2U){_LL6: _tmp3B3=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f1;_tmp3B2=((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f1;_LL7:
# 2043
 if(_tmp3B3 != 0 && _tmp3B3 != 1)return t1;else{
if(_tmp3B2 != 0 && _tmp3B2 != 1)return t2;else{
if(_tmp3B3 >= _tmp3B2)return t1;else{
return t2;}}}}else{_LL8: _LL9:
 return t1;}}else{if(((struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct*)_tmp3B1.f2)->tag == 2U){_LLA: _LLB:
 return t2;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_LongLong_sz){_LLC: _LLD:
 goto _LLF;}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL14;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL16;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_Int_sz)goto _LL1C;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_Int_sz)goto _LL1E;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2){case Cyc_Absyn_Long_sz: goto _LL14;case Cyc_Absyn_Int_sz: goto _LL1C;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_Long_sz){_LL14: _LL15:
# 2053
 goto _LL17;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp3B1.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_Int_sz){_LL1C: _LL1D:
# 2058
 goto _LL1F;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f1 == Cyc_Absyn_Unsigned){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LLE;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2){case Cyc_Absyn_Long_sz: goto _LL16;case Cyc_Absyn_Int_sz: goto _LL1E;default: if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_LongLong_sz)goto _LL10;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_LongLong_sz)goto _LL12;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_Long_sz)goto _LL20;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_Long_sz)goto _LL22;else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_LongLong_sz){_LL10: _LL11:
# 2051
 goto _LL13;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp3B1.f2)->tag == 4U)goto _LL1A;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f1)->f2 == Cyc_Absyn_Long_sz){_LL20: _LL21:
# 2060
 goto _LL23;}else{goto _LL24;}}}}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->tag == 1U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f1 == Cyc_Absyn_Unsigned)switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2){case Cyc_Absyn_LongLong_sz: _LLE: _LLF:
# 2050
 return Cyc_Absyn_ulonglong_type;case Cyc_Absyn_Long_sz: _LL16: _LL17:
# 2054
 return Cyc_Absyn_ulong_type;default: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp3B1.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_Int_sz){_LL1E: _LL1F:
# 2059
 return Cyc_Absyn_uint_type;}else{goto _LL24;}}}else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_LongLong_sz){_LL12: _LL13:
# 2052
 return Cyc_Absyn_slonglong_type;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp3B1.f1)->tag == 4U)goto _LL18;else{if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)_tmp3B1.f2)->f2 == Cyc_Absyn_Long_sz){_LL22: _LL23:
# 2061
 return Cyc_Absyn_slong_type;}else{goto _LL24;}}}}}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp3B1.f1)->tag == 4U){_LL18: _LL19:
# 2056
 goto _LL1B;}else{if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)_tmp3B1.f2)->tag == 4U){_LL1A: _LL1B:
 goto _LL1D;}else{_LL24: _LL25:
# 2062
 goto _LL5;}}}}}}_LL5:;}
# 2064
goto _LL0;}else{goto _LL3;}}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2067
return Cyc_Absyn_sint_type;}
# 2072
int Cyc_Tcutil_coerce_list(struct Cyc_Tcenv_Tenv*te,void*t,struct Cyc_List_List*es){
# 2075
struct Cyc_Core_Opt*max_arith_type=0;
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
void*t1=Cyc_Tcutil_compress((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt));
if(Cyc_Tcutil_is_arithmetic_type(t1)){
if(max_arith_type == 0 ||
 Cyc_Tcutil_will_lose_precision(t1,(void*)max_arith_type->v))
max_arith_type=({struct Cyc_Core_Opt*_tmp3B6=_cycalloc(sizeof(*_tmp3B6));_tmp3B6->v=t1;_tmp3B6;});}}}
# 2084
if(max_arith_type != 0){
if(!Cyc_Tcutil_unify(t,(void*)max_arith_type->v))
return 0;}
# 2088
{struct Cyc_List_List*el=es;for(0;el != 0;el=el->tl){
if(!Cyc_Tcutil_coerce_assign(te,(struct Cyc_Absyn_Exp*)el->hd,t)){
({struct Cyc_String_pa_PrintArg_struct _tmp3B9=({struct Cyc_String_pa_PrintArg_struct _tmp73A;_tmp73A.tag=0U,({
struct _fat_ptr _tmp841=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp73A.f1=_tmp841;});_tmp73A;});struct Cyc_String_pa_PrintArg_struct _tmp3BA=({struct Cyc_String_pa_PrintArg_struct _tmp739;_tmp739.tag=0U,({struct _fat_ptr _tmp842=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((void*)_check_null(((struct Cyc_Absyn_Exp*)el->hd)->topt)));_tmp739.f1=_tmp842;});_tmp739;});void*_tmp3B7[2U];_tmp3B7[0]=& _tmp3B9,_tmp3B7[1]=& _tmp3BA;({unsigned _tmp844=((struct Cyc_Absyn_Exp*)el->hd)->loc;struct _fat_ptr _tmp843=({const char*_tmp3B8="type mismatch: expecting %s but found %s";_tag_fat(_tmp3B8,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp844,_tmp843,_tag_fat(_tmp3B7,sizeof(void*),2U));});});
return 0;}}}
# 2094
return 1;}
# 2099
int Cyc_Tcutil_coerce_to_bool(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(!Cyc_Tcutil_coerce_sint_type(te,e)){
void*_tmp3BB=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp3BC=_tmp3BB;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3BC)->tag == 3U){_LL1: _LL2:
 Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_Other_coercion);goto _LL0;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2105
return 1;}
# 2109
int Cyc_Tcutil_coerce_uint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
return 1;
# 2113
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_uint_type))
({void*_tmp3BD=0U;({unsigned _tmp846=e->loc;struct _fat_ptr _tmp845=({const char*_tmp3BE="integral size mismatch; conversion supplied";_tag_fat(_tmp3BE,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp846,_tmp845,_tag_fat(_tmp3BD,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_uint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2119
return 0;}
# 2123
int Cyc_Tcutil_coerce_sint_type(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
if(Cyc_Tcutil_unify((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
return 1;
# 2127
if(Cyc_Tcutil_is_integral_type((void*)_check_null(e->topt))){
if(Cyc_Tcutil_will_lose_precision((void*)_check_null(e->topt),Cyc_Absyn_sint_type))
({void*_tmp3BF=0U;({unsigned _tmp848=e->loc;struct _fat_ptr _tmp847=({const char*_tmp3C0="integral size mismatch; conversion supplied";_tag_fat(_tmp3C0,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp848,_tmp847,_tag_fat(_tmp3BF,sizeof(void*),0U));});});
Cyc_Tcutil_unchecked_cast(te,e,Cyc_Absyn_sint_type,Cyc_Absyn_No_coercion);
return 1;}
# 2133
return 0;}
# 2138
int Cyc_Tcutil_force_type2bool(int desired,void*t){
Cyc_Tcutil_unify(desired?Cyc_Absyn_true_type: Cyc_Absyn_false_type,t);
return Cyc_Absyn_type2bool(desired,t);}
# 2144
void*Cyc_Tcutil_force_bounds_one(void*t){
({void*_tmp849=t;Cyc_Tcutil_unify(_tmp849,Cyc_Absyn_bounds_one());});
return Cyc_Tcutil_compress(t);}
# 2149
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_thin_bound(struct Cyc_List_List*ts){
void*_tmp3C1=Cyc_Tcutil_compress((void*)((struct Cyc_List_List*)_check_null(ts))->hd);
void*_tmp3C2=_tmp3C1;struct Cyc_Absyn_Exp*_tmp3C4;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3C2)->tag == 9U){_LL1: _tmp3C4=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp3C2)->f1;_LL2:
 return _tmp3C4;}else{_LL3: _LL4: {
# 2154
struct Cyc_Absyn_Exp*_tmp3C3=Cyc_Absyn_valueof_exp(_tmp3C1,0U);
_tmp3C3->topt=Cyc_Absyn_uint_type;
return _tmp3C3;}}_LL0:;}
# 2163
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_bounds_exp(void*def,void*b){
Cyc_Tcutil_unify(def,b);{
void*_tmp3C5=Cyc_Tcutil_compress(b);void*_tmp3C6=_tmp3C5;struct Cyc_List_List*_tmp3CA;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C6)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C6)->f1)){case 14U: _LL1: _LL2:
 return 0;case 13U: _LL3: _tmp3CA=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3C6)->f2;_LL4:
 return Cyc_Tcutil_get_thin_bound(_tmp3CA);default: goto _LL5;}else{_LL5: _LL6:
({struct Cyc_String_pa_PrintArg_struct _tmp3C9=({struct Cyc_String_pa_PrintArg_struct _tmp73B;_tmp73B.tag=0U,({struct _fat_ptr _tmp84A=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(b));_tmp73B.f1=_tmp84A;});_tmp73B;});void*_tmp3C7[1U];_tmp3C7[0]=& _tmp3C9;({struct _fat_ptr _tmp84B=({const char*_tmp3C8="get_bounds_exp: %s";_tag_fat(_tmp3C8,sizeof(char),19U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp84B,_tag_fat(_tmp3C7,sizeof(void*),1U));});});}_LL0:;};}
# 2172
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_ptr_bounds_exp(void*def,void*t){
void*_tmp3CB=Cyc_Tcutil_compress(t);void*_tmp3CC=_tmp3CB;void*_tmp3D0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CC)->tag == 3U){_LL1: _tmp3D0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3CC)->f1).ptr_atts).bounds;_LL2:
# 2175
 return Cyc_Tcutil_get_bounds_exp(def,_tmp3D0);}else{_LL3: _LL4:
({struct Cyc_String_pa_PrintArg_struct _tmp3CF=({struct Cyc_String_pa_PrintArg_struct _tmp73C;_tmp73C.tag=0U,({struct _fat_ptr _tmp84C=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp73C.f1=_tmp84C;});_tmp73C;});void*_tmp3CD[1U];_tmp3CD[0]=& _tmp3CF;({struct _fat_ptr _tmp84D=({const char*_tmp3CE="get_ptr_bounds_exp not pointer: %s";_tag_fat(_tmp3CE,sizeof(char),35U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp84D,_tag_fat(_tmp3CD,sizeof(void*),1U));});});}_LL0:;}
# 2181
void*Cyc_Tcutil_any_bool(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3D1=_cycalloc(sizeof(*_tmp3D1));({struct Cyc_List_List*_tmp84E=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3D1->v=_tmp84E;});_tmp3D1;}));else{
# 2185
return Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,({struct Cyc_Core_Opt*_tmp3D2=_cycalloc(sizeof(*_tmp3D2));_tmp3D2->v=0;_tmp3D2;}));}}
# 2188
void*Cyc_Tcutil_any_bounds(struct Cyc_Tcenv_Tenv**tep){
if(tep != 0)
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3D3=_cycalloc(sizeof(*_tmp3D3));({struct Cyc_List_List*_tmp84F=Cyc_Tcenv_lookup_type_vars(*tep);_tmp3D3->v=_tmp84F;});_tmp3D3;}));else{
# 2192
return Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,({struct Cyc_Core_Opt*_tmp3D4=_cycalloc(sizeof(*_tmp3D4));_tmp3D4->v=0;_tmp3D4;}));}}
# 2196
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2);struct _tuple22{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};
# 2204
int Cyc_Tcutil_silent_castable(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*t1,void*t2){
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple14 _tmp3D5=({struct _tuple14 _tmp740;_tmp740.f1=t1,_tmp740.f2=t2;_tmp740;});struct _tuple14 _tmp3D6=_tmp3D5;void*_tmp3EB;struct Cyc_Absyn_Tqual _tmp3EA;struct Cyc_Absyn_Exp*_tmp3E9;void*_tmp3E8;void*_tmp3E7;struct Cyc_Absyn_Tqual _tmp3E6;struct Cyc_Absyn_Exp*_tmp3E5;void*_tmp3E4;struct Cyc_Absyn_PtrInfo _tmp3E3;struct Cyc_Absyn_PtrInfo _tmp3E2;switch(*((int*)_tmp3D6.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D6.f2)->tag == 3U){_LL1: _tmp3E3=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D6.f1)->f1;_tmp3E2=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3D6.f2)->f1;_LL2: {
# 2209
int okay=1;
# 2211
if(!Cyc_Tcutil_unify((_tmp3E3.ptr_atts).nullable,(_tmp3E2.ptr_atts).nullable))
# 2213
okay=!Cyc_Tcutil_force_type2bool(0,(_tmp3E3.ptr_atts).nullable);
# 2215
if(!Cyc_Tcutil_unify((_tmp3E3.ptr_atts).bounds,(_tmp3E2.ptr_atts).bounds)){
struct _tuple22 _tmp3D7=({struct _tuple22 _tmp73D;({struct Cyc_Absyn_Exp*_tmp853=({void*_tmp852=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp852,(_tmp3E3.ptr_atts).bounds);});_tmp73D.f1=_tmp853;}),({
struct Cyc_Absyn_Exp*_tmp851=({void*_tmp850=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp850,(_tmp3E2.ptr_atts).bounds);});_tmp73D.f2=_tmp851;});_tmp73D;});
# 2216
struct _tuple22 _tmp3D8=_tmp3D7;struct Cyc_Absyn_Exp*_tmp3DC;struct Cyc_Absyn_Exp*_tmp3DB;if(_tmp3D8.f2 == 0){_LLA: _LLB:
# 2219
 okay=1;goto _LL9;}else{if(_tmp3D8.f1 == 0){_LLC: _LLD:
# 2222
 if(Cyc_Tcutil_force_type2bool(0,(_tmp3E3.ptr_atts).zero_term)&&({
void*_tmp854=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmp854,(_tmp3E2.ptr_atts).bounds);}))
goto _LL9;
okay=0;
goto _LL9;}else{_LLE: _tmp3DC=_tmp3D8.f1;_tmp3DB=_tmp3D8.f2;_LLF:
# 2228
 okay=okay &&({struct Cyc_Absyn_Exp*_tmp855=(struct Cyc_Absyn_Exp*)_check_null(_tmp3DB);Cyc_Evexp_lte_const_exp(_tmp855,(struct Cyc_Absyn_Exp*)_check_null(_tmp3DC));});
# 2232
if(!Cyc_Tcutil_force_type2bool(0,(_tmp3E2.ptr_atts).zero_term))
({void*_tmp3D9=0U;({unsigned _tmp857=loc;struct _fat_ptr _tmp856=({const char*_tmp3DA="implicit cast to shorter array";_tag_fat(_tmp3DA,sizeof(char),31U);});Cyc_Tcutil_warn(_tmp857,_tmp856,_tag_fat(_tmp3D9,sizeof(void*),0U));});});
goto _LL9;}}_LL9:;}
# 2239
okay=okay &&(!(_tmp3E3.elt_tq).real_const ||(_tmp3E2.elt_tq).real_const);
# 2242
if(!Cyc_Tcutil_unify((_tmp3E3.ptr_atts).rgn,(_tmp3E2.ptr_atts).rgn)){
if(Cyc_Tcenv_region_outlives(te,(_tmp3E3.ptr_atts).rgn,(_tmp3E2.ptr_atts).rgn)){
if(Cyc_Tcutil_warn_region_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp3DF=({struct Cyc_String_pa_PrintArg_struct _tmp73F;_tmp73F.tag=0U,({
struct _fat_ptr _tmp858=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((_tmp3E3.ptr_atts).rgn));_tmp73F.f1=_tmp858;});_tmp73F;});struct Cyc_String_pa_PrintArg_struct _tmp3E0=({struct Cyc_String_pa_PrintArg_struct _tmp73E;_tmp73E.tag=0U,({
struct _fat_ptr _tmp859=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string((_tmp3E2.ptr_atts).rgn));_tmp73E.f1=_tmp859;});_tmp73E;});void*_tmp3DD[2U];_tmp3DD[0]=& _tmp3DF,_tmp3DD[1]=& _tmp3E0;({unsigned _tmp85B=loc;struct _fat_ptr _tmp85A=({const char*_tmp3DE="implicit cast from region %s to region %s";_tag_fat(_tmp3DE,sizeof(char),42U);});Cyc_Tcutil_warn(_tmp85B,_tmp85A,_tag_fat(_tmp3DD,sizeof(void*),2U));});});}else{
okay=0;}}
# 2252
okay=okay &&(Cyc_Tcutil_unify((_tmp3E3.ptr_atts).zero_term,(_tmp3E2.ptr_atts).zero_term)||
# 2254
 Cyc_Tcutil_force_type2bool(1,(_tmp3E3.ptr_atts).zero_term)&&(_tmp3E2.elt_tq).real_const);{
# 2262
int _tmp3E1=
({void*_tmp85C=Cyc_Absyn_bounds_one();Cyc_Tcutil_unify(_tmp85C,(_tmp3E2.ptr_atts).bounds);})&& !
Cyc_Tcutil_force_type2bool(0,(_tmp3E2.ptr_atts).zero_term);
# 2268
okay=okay &&(Cyc_Tcutil_unify(_tmp3E3.elt_type,_tmp3E2.elt_type)||
(_tmp3E1 &&((_tmp3E2.elt_tq).real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp3E2.elt_type))))&& Cyc_Tcutil_ptrsubtype(te,0,_tmp3E3.elt_type,_tmp3E2.elt_type));
# 2271
return okay;};}}else{goto _LL7;}case 4U: if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f2)->tag == 4U){_LL3: _tmp3EB=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f1)->f1).elt_type;_tmp3EA=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f1)->f1).tq;_tmp3E9=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f1)->f1).num_elts;_tmp3E8=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f1)->f1).zero_term;_tmp3E7=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f2)->f1).elt_type;_tmp3E6=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f2)->f1).tq;_tmp3E5=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f2)->f1).num_elts;_tmp3E4=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp3D6.f2)->f1).zero_term;_LL4: {
# 2275
int okay;
# 2278
okay=Cyc_Tcutil_unify(_tmp3E8,_tmp3E4)&&(
(_tmp3E9 != 0 && _tmp3E5 != 0)&& Cyc_Evexp_same_const_exp(_tmp3E9,_tmp3E5));
# 2281
return(okay && Cyc_Tcutil_unify(_tmp3EB,_tmp3E7))&&(!_tmp3EA.real_const || _tmp3E6.real_const);}}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D6.f1)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D6.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3D6.f2)->f1)->tag == 1U){_LL5: _LL6:
# 2283
 return 0;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}default: _LL7: _LL8:
# 2285
 return Cyc_Tcutil_unify(t1,t2);}_LL0:;};}
# 2289
void*Cyc_Tcutil_pointer_elt_type(void*t){
void*_tmp3EC=Cyc_Tcutil_compress(t);void*_tmp3ED=_tmp3EC;void*_tmp3F0;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3ED)->tag == 3U){_LL1: _tmp3F0=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3ED)->f1).elt_type;_LL2:
 return _tmp3F0;}else{_LL3: _LL4:
({void*_tmp3EE=0U;({struct _fat_ptr _tmp85D=({const char*_tmp3EF="pointer_elt_type";_tag_fat(_tmp3EF,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp85D,_tag_fat(_tmp3EE,sizeof(void*),0U));});});}_LL0:;}
# 2295
void*Cyc_Tcutil_pointer_region(void*t){
void*_tmp3F1=Cyc_Tcutil_compress(t);void*_tmp3F2=_tmp3F1;struct Cyc_Absyn_PtrAtts*_tmp3F5;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F2)->tag == 3U){_LL1: _tmp3F5=(struct Cyc_Absyn_PtrAtts*)&(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F2)->f1).ptr_atts;_LL2:
 return _tmp3F5->rgn;}else{_LL3: _LL4:
({void*_tmp3F3=0U;({struct _fat_ptr _tmp85E=({const char*_tmp3F4="pointer_elt_type";_tag_fat(_tmp3F4,sizeof(char),17U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp85E,_tag_fat(_tmp3F3,sizeof(void*),0U));});});}_LL0:;}
# 2302
int Cyc_Tcutil_rgn_of_pointer(void*t,void**rgn){
void*_tmp3F6=Cyc_Tcutil_compress(t);void*_tmp3F7=_tmp3F6;void*_tmp3F8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7)->tag == 3U){_LL1: _tmp3F8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3F7)->f1).ptr_atts).rgn;_LL2:
# 2305
*rgn=_tmp3F8;
return 1;}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2314
int Cyc_Tcutil_admits_zero(void*t){
void*_tmp3F9=Cyc_Tcutil_compress(t);void*_tmp3FA=_tmp3F9;void*_tmp3FC;void*_tmp3FB;switch(*((int*)_tmp3FA)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp3FA)->f1)){case 1U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 return 1;default: goto _LL7;}case 3U: _LL5: _tmp3FC=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FA)->f1).ptr_atts).nullable;_tmp3FB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp3FA)->f1).ptr_atts).bounds;_LL6:
# 2322
 return !Cyc_Tcutil_unify(Cyc_Absyn_fat_bound_type,_tmp3FB)&& Cyc_Tcutil_force_type2bool(0,_tmp3FC);default: _LL7: _LL8:
 return 0;}_LL0:;}
# 2328
int Cyc_Tcutil_is_zero(struct Cyc_Absyn_Exp*e){
void*_tmp3FD=e->r;void*_tmp3FE=_tmp3FD;void*_tmp402;struct Cyc_Absyn_Exp*_tmp401;struct _fat_ptr _tmp400;switch(*((int*)_tmp3FE)){case 0U: switch(((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FE)->f1).Wchar_c).tag){case 5U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FE)->f1).Int_c).val).f2 == 0){_LL1: _LL2:
 goto _LL4;}else{goto _LLF;}case 2U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FE)->f1).Char_c).val).f2 == 0){_LL3: _LL4:
 goto _LL6;}else{goto _LLF;}case 4U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FE)->f1).Short_c).val).f2 == 0){_LL5: _LL6:
 goto _LL8;}else{goto _LLF;}case 6U: if((((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FE)->f1).LongLong_c).val).f2 == 0){_LL7: _LL8:
 goto _LLA;}else{goto _LLF;}case 3U: _LLB: _tmp400=((((struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct*)_tmp3FE)->f1).Wchar_c).val;_LLC: {
# 2337
unsigned long _tmp3FF=Cyc_strlen((struct _fat_ptr)_tmp400);
int i=0;
if(_tmp3FF >= (unsigned long)2 &&(int)*((const char*)_check_fat_subscript(_tmp400,sizeof(char),0))== (int)'\\'){
if((int)*((const char*)_check_fat_subscript(_tmp400,sizeof(char),1))== (int)'0')i=2;else{
if(((int)((const char*)_tmp400.curr)[1]== (int)'x' && _tmp3FF >= (unsigned long)3)&&(int)*((const char*)_check_fat_subscript(_tmp400,sizeof(char),2))== (int)'0')i=3;else{
return 0;}}
for(0;(unsigned long)i < _tmp3FF;++ i){
if((int)*((const char*)_check_fat_subscript(_tmp400,sizeof(char),i))!= (int)'0')return 0;}
return 1;}else{
# 2347
return 0;}}default: goto _LLF;}case 2U: _LL9: _LLA:
# 2335
 return 1;case 14U: _LLD: _tmp402=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FE)->f1;_tmp401=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp3FE)->f2;_LLE:
# 2348
 return Cyc_Tcutil_is_zero(_tmp401)&& Cyc_Tcutil_admits_zero(_tmp402);default: _LLF: _LL10:
 return 0;}_LL0:;}
# 2353
struct Cyc_Absyn_Kind Cyc_Tcutil_rk={Cyc_Absyn_RgnKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ak={Cyc_Absyn_AnyKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_bk={Cyc_Absyn_BoxKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_mk={Cyc_Absyn_MemKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ik={Cyc_Absyn_IntKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ek={Cyc_Absyn_EffKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_boolk={Cyc_Absyn_BoolKind,Cyc_Absyn_Aliasable};
struct Cyc_Absyn_Kind Cyc_Tcutil_ptrbk={Cyc_Absyn_PtrBndKind,Cyc_Absyn_Aliasable};
# 2362
struct Cyc_Absyn_Kind Cyc_Tcutil_trk={Cyc_Absyn_RgnKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tak={Cyc_Absyn_AnyKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tbk={Cyc_Absyn_BoxKind,Cyc_Absyn_Top};
struct Cyc_Absyn_Kind Cyc_Tcutil_tmk={Cyc_Absyn_MemKind,Cyc_Absyn_Top};
# 2367
struct Cyc_Absyn_Kind Cyc_Tcutil_urk={Cyc_Absyn_RgnKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_uak={Cyc_Absyn_AnyKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_ubk={Cyc_Absyn_BoxKind,Cyc_Absyn_Unique};
struct Cyc_Absyn_Kind Cyc_Tcutil_umk={Cyc_Absyn_MemKind,Cyc_Absyn_Unique};
# 2372
struct Cyc_Core_Opt Cyc_Tcutil_rko={(void*)& Cyc_Tcutil_rk};
struct Cyc_Core_Opt Cyc_Tcutil_ako={(void*)& Cyc_Tcutil_ak};
struct Cyc_Core_Opt Cyc_Tcutil_bko={(void*)& Cyc_Tcutil_bk};
struct Cyc_Core_Opt Cyc_Tcutil_mko={(void*)& Cyc_Tcutil_mk};
struct Cyc_Core_Opt Cyc_Tcutil_iko={(void*)& Cyc_Tcutil_ik};
struct Cyc_Core_Opt Cyc_Tcutil_eko={(void*)& Cyc_Tcutil_ek};
struct Cyc_Core_Opt Cyc_Tcutil_boolko={(void*)& Cyc_Tcutil_boolk};
struct Cyc_Core_Opt Cyc_Tcutil_ptrbko={(void*)& Cyc_Tcutil_ptrbk};
# 2381
struct Cyc_Core_Opt Cyc_Tcutil_trko={(void*)& Cyc_Tcutil_trk};
struct Cyc_Core_Opt Cyc_Tcutil_tako={(void*)& Cyc_Tcutil_tak};
struct Cyc_Core_Opt Cyc_Tcutil_tbko={(void*)& Cyc_Tcutil_tbk};
struct Cyc_Core_Opt Cyc_Tcutil_tmko={(void*)& Cyc_Tcutil_tmk};
# 2386
struct Cyc_Core_Opt Cyc_Tcutil_urko={(void*)& Cyc_Tcutil_urk};
struct Cyc_Core_Opt Cyc_Tcutil_uako={(void*)& Cyc_Tcutil_uak};
struct Cyc_Core_Opt Cyc_Tcutil_ubko={(void*)& Cyc_Tcutil_ubk};
struct Cyc_Core_Opt Cyc_Tcutil_umko={(void*)& Cyc_Tcutil_umk};
# 2391
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_opt(struct Cyc_Absyn_Kind*ka){
struct Cyc_Absyn_Kind*_tmp403=ka;enum Cyc_Absyn_KindQual _tmp40C;enum Cyc_Absyn_AliasQual _tmp40B;_LL1: _tmp40C=_tmp403->kind;_tmp40B=_tmp403->aliasqual;_LL2:;
{enum Cyc_Absyn_AliasQual _tmp404=_tmp40B;switch(_tmp404){case Cyc_Absyn_Aliasable: _LL4: _LL5: {
# 2395
enum Cyc_Absyn_KindQual _tmp405=_tmp40C;switch(_tmp405){case Cyc_Absyn_AnyKind: _LLD: _LLE:
 return& Cyc_Tcutil_ako;case Cyc_Absyn_MemKind: _LLF: _LL10:
 return& Cyc_Tcutil_mko;case Cyc_Absyn_BoxKind: _LL11: _LL12:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_RgnKind: _LL13: _LL14:
 return& Cyc_Tcutil_rko;case Cyc_Absyn_EffKind: _LL15: _LL16:
 return& Cyc_Tcutil_eko;case Cyc_Absyn_IntKind: _LL17: _LL18:
 return& Cyc_Tcutil_iko;case Cyc_Absyn_BoolKind: _LL19: _LL1A:
 return& Cyc_Tcutil_bko;case Cyc_Absyn_PtrBndKind: _LL1B: _LL1C:
 goto _LL1E;default: _LL1D: _LL1E: return& Cyc_Tcutil_ptrbko;}_LLC:;}case Cyc_Absyn_Unique: _LL6: _LL7:
# 2406
{enum Cyc_Absyn_KindQual _tmp406=_tmp40C;switch(_tmp406){case Cyc_Absyn_AnyKind: _LL20: _LL21:
 return& Cyc_Tcutil_uako;case Cyc_Absyn_MemKind: _LL22: _LL23:
 return& Cyc_Tcutil_umko;case Cyc_Absyn_BoxKind: _LL24: _LL25:
 return& Cyc_Tcutil_ubko;case Cyc_Absyn_RgnKind: _LL26: _LL27:
 return& Cyc_Tcutil_urko;default: _LL28: _LL29:
 goto _LL1F;}_LL1F:;}
# 2413
goto _LL3;case Cyc_Absyn_Top: _LL8: _LL9:
# 2415
{enum Cyc_Absyn_KindQual _tmp407=_tmp40C;switch(_tmp407){case Cyc_Absyn_AnyKind: _LL2B: _LL2C:
 return& Cyc_Tcutil_tako;case Cyc_Absyn_MemKind: _LL2D: _LL2E:
 return& Cyc_Tcutil_tmko;case Cyc_Absyn_BoxKind: _LL2F: _LL30:
 return& Cyc_Tcutil_tbko;case Cyc_Absyn_RgnKind: _LL31: _LL32:
 return& Cyc_Tcutil_trko;default: _LL33: _LL34:
 goto _LL2A;}_LL2A:;}
# 2422
goto _LL3;default: _LLA: _LLB:
 goto _LL3;}_LL3:;}
# 2425
({struct Cyc_String_pa_PrintArg_struct _tmp40A=({struct Cyc_String_pa_PrintArg_struct _tmp741;_tmp741.tag=0U,({struct _fat_ptr _tmp85F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_kind2string(ka));_tmp741.f1=_tmp85F;});_tmp741;});void*_tmp408[1U];_tmp408[0]=& _tmp40A;({struct _fat_ptr _tmp860=({const char*_tmp409="kind_to_opt: bad kind %s\n";_tag_fat(_tmp409,sizeof(char),26U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp860,_tag_fat(_tmp408,sizeof(void*),1U));});});}
# 2428
void*Cyc_Tcutil_kind_to_bound(struct Cyc_Absyn_Kind*k){
return(void*)({struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct*_tmp40D=_cycalloc(sizeof(*_tmp40D));_tmp40D->tag=0U,_tmp40D->f1=k;_tmp40D;});}
# 2431
struct Cyc_Core_Opt*Cyc_Tcutil_kind_to_bound_opt(struct Cyc_Absyn_Kind*k){
return({struct Cyc_Core_Opt*_tmp40E=_cycalloc(sizeof(*_tmp40E));({void*_tmp861=Cyc_Tcutil_kind_to_bound(k);_tmp40E->v=_tmp861;});_tmp40E;});}
# 2437
int Cyc_Tcutil_zero_to_null(struct Cyc_Tcenv_Tenv*te,void*t2,struct Cyc_Absyn_Exp*e1){
if(Cyc_Tcutil_is_pointer_type(t2)&& Cyc_Tcutil_is_zero(e1)){
({void*_tmp862=(Cyc_Absyn_null_exp(0U))->r;e1->r=_tmp862;});{
struct Cyc_Core_Opt*_tmp40F=Cyc_Tcenv_lookup_opt_type_vars(te);
void*_tmp410=Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp744;({void*_tmp867=Cyc_Absyn_new_evar(& Cyc_Tcutil_ako,_tmp40F);_tmp744.elt_type=_tmp867;}),({
struct Cyc_Absyn_Tqual _tmp866=Cyc_Absyn_empty_tqual(0U);_tmp744.elt_tq=_tmp866;}),
({void*_tmp865=Cyc_Absyn_new_evar(& Cyc_Tcutil_trko,_tmp40F);(_tmp744.ptr_atts).rgn=_tmp865;}),(_tmp744.ptr_atts).nullable=Cyc_Absyn_true_type,({
# 2445
void*_tmp864=Cyc_Absyn_new_evar(& Cyc_Tcutil_ptrbko,_tmp40F);(_tmp744.ptr_atts).bounds=_tmp864;}),({
void*_tmp863=Cyc_Absyn_new_evar(& Cyc_Tcutil_boolko,_tmp40F);(_tmp744.ptr_atts).zero_term=_tmp863;}),(_tmp744.ptr_atts).ptrloc=0;_tmp744;}));
e1->topt=_tmp410;{
int bogus=0;
int retv=Cyc_Tcutil_coerce_arg(te,e1,t2,& bogus);
if(bogus != 0)
({struct Cyc_String_pa_PrintArg_struct _tmp413=({struct Cyc_String_pa_PrintArg_struct _tmp743;_tmp743.tag=0U,({
struct _fat_ptr _tmp868=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp743.f1=_tmp868;});_tmp743;});struct Cyc_String_pa_PrintArg_struct _tmp414=({struct Cyc_String_pa_PrintArg_struct _tmp742;_tmp742.tag=0U,({struct _fat_ptr _tmp869=(struct _fat_ptr)((struct _fat_ptr)Cyc_Position_string_of_segment(e1->loc));_tmp742.f1=_tmp869;});_tmp742;});void*_tmp411[2U];_tmp411[0]=& _tmp413,_tmp411[1]=& _tmp414;({struct _fat_ptr _tmp86A=({const char*_tmp412="zero_to_null resulted in an alias coercion on %s at %s\n";_tag_fat(_tmp412,sizeof(char),56U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp86A,_tag_fat(_tmp411,sizeof(void*),2U));});});
return retv;};};}
# 2455
return 0;}
# 2458
int Cyc_Tcutil_warn_alias_coerce=0;struct _tuple23{struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Exp*f2;};
# 2464
struct _tuple23 Cyc_Tcutil_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type){
static struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct rgn_kb={0U,& Cyc_Tcutil_rk};
# 2468
static int counter=0;
struct _tuple1*v=({struct _tuple1*_tmp428=_cycalloc(sizeof(*_tmp428));_tmp428->f1=Cyc_Absyn_Loc_n,({struct _fat_ptr*_tmp86D=({struct _fat_ptr*_tmp427=_cycalloc(sizeof(*_tmp427));({struct _fat_ptr _tmp86C=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp426=({struct Cyc_Int_pa_PrintArg_struct _tmp746;_tmp746.tag=1U,_tmp746.f1=(unsigned long)counter ++;_tmp746;});void*_tmp424[1U];_tmp424[0]=& _tmp426;({struct _fat_ptr _tmp86B=({const char*_tmp425="__aliasvar%d";_tag_fat(_tmp425,sizeof(char),13U);});Cyc_aprintf(_tmp86B,_tag_fat(_tmp424,sizeof(void*),1U));});});*_tmp427=_tmp86C;});_tmp427;});_tmp428->f2=_tmp86D;});_tmp428;});
struct Cyc_Absyn_Vardecl*vd=Cyc_Absyn_new_vardecl(0U,v,e_type,e);
struct Cyc_Absyn_Exp*ve=({void*_tmp86E=(void*)({struct Cyc_Absyn_Local_b_Absyn_Binding_struct*_tmp423=_cycalloc(sizeof(*_tmp423));_tmp423->tag=4U,_tmp423->f1=vd;_tmp423;});Cyc_Absyn_varb_exp(_tmp86E,e->loc);});
# 2477
struct Cyc_Absyn_Tvar*tv=Cyc_Tcutil_new_tvar((void*)& rgn_kb);
# 2479
{void*_tmp415=Cyc_Tcutil_compress(e_type);void*_tmp416=_tmp415;void*_tmp422;struct Cyc_Absyn_Tqual _tmp421;void*_tmp420;void*_tmp41F;void*_tmp41E;void*_tmp41D;struct Cyc_Absyn_PtrLoc*_tmp41C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->tag == 3U){_LL1: _tmp422=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).elt_type;_tmp421=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).elt_tq;_tmp420=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).ptr_atts).rgn;_tmp41F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).ptr_atts).nullable;_tmp41E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).ptr_atts).bounds;_tmp41D=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).ptr_atts).zero_term;_tmp41C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp416)->f1).ptr_atts).ptrloc;_LL2:
# 2481
{void*_tmp417=Cyc_Tcutil_compress(_tmp420);void*_tmp418=_tmp417;void**_tmp41B;struct Cyc_Core_Opt*_tmp41A;if(((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp418)->tag == 1U){_LL6: _tmp41B=(void**)&((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp418)->f2;_tmp41A=((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp418)->f4;_LL7: {
# 2483
void*_tmp419=Cyc_Absyn_var_type(tv);
*_tmp41B=_tmp419;
goto _LL5;}}else{_LL8: _LL9:
 goto _LL5;}_LL5:;}
# 2488
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 2492
e->topt=0;
vd->initializer=0;{
# 2496
struct Cyc_Absyn_Decl*d=Cyc_Absyn_alias_decl(tv,vd,e,e->loc);
# 2498
return({struct _tuple23 _tmp745;_tmp745.f1=d,_tmp745.f2=ve;_tmp745;});};}
# 2503
static int Cyc_Tcutil_can_insert_alias(struct Cyc_Absyn_Exp*e,void*e_type,void*wants_type,unsigned loc){
# 2506
if((Cyc_Tcutil_is_noalias_path(e)&&
 Cyc_Tcutil_is_noalias_pointer(e_type,0))&&
 Cyc_Tcutil_is_pointer_type(e_type)){
# 2511
void*_tmp429=Cyc_Tcutil_compress(wants_type);void*_tmp42A=_tmp429;void*_tmp42C;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->tag == 3U){_LL1: _tmp42C=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp42A)->f1).ptr_atts).rgn;_LL2:
# 2513
 if(Cyc_Tcutil_is_heap_rgn_type(_tmp42C))return 0;{
struct Cyc_Absyn_Kind*_tmp42B=Cyc_Tcutil_type_kind(_tmp42C);
return(int)_tmp42B->kind == (int)Cyc_Absyn_RgnKind &&(int)_tmp42B->aliasqual == (int)Cyc_Absyn_Aliasable;};}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2519
return 0;}
# 2523
int Cyc_Tcutil_coerce_arg(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t2,int*alias_coercion){
void*t1=Cyc_Tcutil_compress((void*)_check_null(e->topt));
enum Cyc_Absyn_Coercion c;
int do_alias_coercion=0;
# 2528
if(Cyc_Tcutil_unify(t1,t2))return 1;
# 2530
if(Cyc_Tcutil_is_arithmetic_type(t2)&& Cyc_Tcutil_is_arithmetic_type(t1)){
# 2532
if(Cyc_Tcutil_will_lose_precision(t1,t2))
({struct Cyc_String_pa_PrintArg_struct _tmp42F=({struct Cyc_String_pa_PrintArg_struct _tmp748;_tmp748.tag=0U,({
struct _fat_ptr _tmp86F=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp748.f1=_tmp86F;});_tmp748;});struct Cyc_String_pa_PrintArg_struct _tmp430=({struct Cyc_String_pa_PrintArg_struct _tmp747;_tmp747.tag=0U,({struct _fat_ptr _tmp870=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp747.f1=_tmp870;});_tmp747;});void*_tmp42D[2U];_tmp42D[0]=& _tmp42F,_tmp42D[1]=& _tmp430;({unsigned _tmp872=e->loc;struct _fat_ptr _tmp871=({const char*_tmp42E="integral size mismatch; %s -> %s conversion supplied";_tag_fat(_tmp42E,sizeof(char),53U);});Cyc_Tcutil_warn(_tmp872,_tmp871,_tag_fat(_tmp42D,sizeof(void*),2U));});});
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_No_coercion);
return 1;}else{
# 2540
if(Cyc_Tcutil_can_insert_alias(e,t1,t2,e->loc)){
if(Cyc_Tcutil_warn_alias_coerce)
({struct Cyc_String_pa_PrintArg_struct _tmp433=({struct Cyc_String_pa_PrintArg_struct _tmp74B;_tmp74B.tag=0U,({
struct _fat_ptr _tmp873=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e));_tmp74B.f1=_tmp873;});_tmp74B;});struct Cyc_String_pa_PrintArg_struct _tmp434=({struct Cyc_String_pa_PrintArg_struct _tmp74A;_tmp74A.tag=0U,({struct _fat_ptr _tmp874=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp74A.f1=_tmp874;});_tmp74A;});struct Cyc_String_pa_PrintArg_struct _tmp435=({struct Cyc_String_pa_PrintArg_struct _tmp749;_tmp749.tag=0U,({struct _fat_ptr _tmp875=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp749.f1=_tmp875;});_tmp749;});void*_tmp431[3U];_tmp431[0]=& _tmp433,_tmp431[1]=& _tmp434,_tmp431[2]=& _tmp435;({unsigned _tmp877=e->loc;struct _fat_ptr _tmp876=({const char*_tmp432="implicit alias coercion for %s:%s to %s";_tag_fat(_tmp432,sizeof(char),40U);});Cyc_Tcutil_warn(_tmp877,_tmp876,_tag_fat(_tmp431,sizeof(void*),3U));});});
*alias_coercion=1;}
# 2547
if(Cyc_Tcutil_silent_castable(te,e->loc,t1,t2)){
Cyc_Tcutil_unchecked_cast(te,e,t2,Cyc_Absyn_Other_coercion);
return 1;}else{
if(Cyc_Tcutil_zero_to_null(te,t2,e))
return 1;else{
if((int)(c=Cyc_Tcutil_castable(te,e->loc,t1,t2))!= (int)Cyc_Absyn_Unknown_coercion){
# 2554
if((int)c != (int)1U)Cyc_Tcutil_unchecked_cast(te,e,t2,c);
if((int)c != (int)2U)
({struct Cyc_String_pa_PrintArg_struct _tmp438=({struct Cyc_String_pa_PrintArg_struct _tmp74D;_tmp74D.tag=0U,({
struct _fat_ptr _tmp878=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t1));_tmp74D.f1=_tmp878;});_tmp74D;});struct Cyc_String_pa_PrintArg_struct _tmp439=({struct Cyc_String_pa_PrintArg_struct _tmp74C;_tmp74C.tag=0U,({struct _fat_ptr _tmp879=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t2));_tmp74C.f1=_tmp879;});_tmp74C;});void*_tmp436[2U];_tmp436[0]=& _tmp438,_tmp436[1]=& _tmp439;({unsigned _tmp87B=e->loc;struct _fat_ptr _tmp87A=({const char*_tmp437="implicit cast from %s to %s";_tag_fat(_tmp437,sizeof(char),28U);});Cyc_Tcutil_warn(_tmp87B,_tmp87A,_tag_fat(_tmp436,sizeof(void*),2U));});});
return 1;}else{
# 2560
return 0;}}}}}
# 2567
int Cyc_Tcutil_coerce_assign(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t){
# 2570
int bogus=0;
return Cyc_Tcutil_coerce_arg(te,e,t,& bogus);}
# 2584 "tcutil.cyc"
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1);struct _tuple24{struct Cyc_List_List*f1;struct _RegionHandle*f2;struct Cyc_Tcenv_Tenv*f3;int f4;};
# 2588
static struct Cyc_List_List*Cyc_Tcutil_flatten_type_f(struct _tuple24*env,struct Cyc_Absyn_Aggrfield*x){
# 2591
struct _tuple24 _tmp43A=*env;struct _tuple24 _tmp43B=_tmp43A;struct Cyc_List_List*_tmp443;struct _RegionHandle*_tmp442;struct Cyc_Tcenv_Tenv*_tmp441;int _tmp440;_LL1: _tmp443=_tmp43B.f1;_tmp442=_tmp43B.f2;_tmp441=_tmp43B.f3;_tmp440=_tmp43B.f4;_LL2:;{
# 2593
void*_tmp43C=_tmp443 == 0?x->type: Cyc_Tcutil_rsubstitute(_tmp442,_tmp443,x->type);
struct Cyc_List_List*_tmp43D=Cyc_Tcutil_flatten_type(_tmp442,_tmp440,_tmp441,_tmp43C);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp43D)== 1)
return({struct Cyc_List_List*_tmp43F=_region_malloc(_tmp442,sizeof(*_tmp43F));({struct _tuple11*_tmp87C=({struct _tuple11*_tmp43E=_region_malloc(_tmp442,sizeof(*_tmp43E));_tmp43E->f1=x->tq,_tmp43E->f2=_tmp43C;_tmp43E;});_tmp43F->hd=_tmp87C;}),_tmp43F->tl=0;_tmp43F;});else{
return _tmp43D;}};}struct _tuple25{struct _RegionHandle*f1;struct Cyc_Tcenv_Tenv*f2;int f3;};
# 2599
static struct Cyc_List_List*Cyc_Tcutil_rcopy_tqt(struct _tuple25*env,struct _tuple11*x){
# 2601
struct _tuple25 _tmp444=*env;struct _tuple25 _tmp445=_tmp444;struct _RegionHandle*_tmp44F;struct Cyc_Tcenv_Tenv*_tmp44E;int _tmp44D;_LL1: _tmp44F=_tmp445.f1;_tmp44E=_tmp445.f2;_tmp44D=_tmp445.f3;_LL2:;{
struct _tuple11 _tmp446=*x;struct _tuple11 _tmp447=_tmp446;struct Cyc_Absyn_Tqual _tmp44C;void*_tmp44B;_LL4: _tmp44C=_tmp447.f1;_tmp44B=_tmp447.f2;_LL5:;{
struct Cyc_List_List*_tmp448=Cyc_Tcutil_flatten_type(_tmp44F,_tmp44D,_tmp44E,_tmp44B);
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp448)== 1)
return({struct Cyc_List_List*_tmp44A=_region_malloc(_tmp44F,sizeof(*_tmp44A));({struct _tuple11*_tmp87D=({struct _tuple11*_tmp449=_region_malloc(_tmp44F,sizeof(*_tmp449));_tmp449->f1=_tmp44C,_tmp449->f2=_tmp44B;_tmp449;});_tmp44A->hd=_tmp87D;}),_tmp44A->tl=0;_tmp44A;});else{
return _tmp448;}};};}
# 2608
static struct Cyc_List_List*Cyc_Tcutil_flatten_type(struct _RegionHandle*r,int flatten,struct Cyc_Tcenv_Tenv*te,void*t1){
# 2612
if(flatten){
t1=Cyc_Tcutil_compress(t1);{
void*_tmp450=t1;struct Cyc_List_List*_tmp46F;struct Cyc_List_List*_tmp46E;struct Cyc_Absyn_Aggrdecl*_tmp46D;struct Cyc_List_List*_tmp46C;switch(*((int*)_tmp450)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp450)->f1)){case 0U: _LL1: _LL2:
 return 0;case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp450)->f1)->f1).KnownAggr).tag == 2){_LL5: _tmp46D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp450)->f1)->f1).KnownAggr).val;_tmp46C=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp450)->f2;_LL6:
# 2630
 if((((int)_tmp46D->kind == (int)Cyc_Absyn_UnionA || _tmp46D->impl == 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46D->impl))->exist_vars != 0)||((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46D->impl))->rgn_po != 0)
# 2632
return({struct Cyc_List_List*_tmp45B=_region_malloc(r,sizeof(*_tmp45B));({struct _tuple11*_tmp87F=({struct _tuple11*_tmp45A=_region_malloc(r,sizeof(*_tmp45A));({struct Cyc_Absyn_Tqual _tmp87E=Cyc_Absyn_empty_tqual(0U);_tmp45A->f1=_tmp87E;}),_tmp45A->f2=t1;_tmp45A;});_tmp45B->hd=_tmp87F;}),_tmp45B->tl=0;_tmp45B;});{
struct Cyc_List_List*_tmp45C=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(r,r,_tmp46D->tvs,_tmp46C);
struct _tuple24 env=({struct _tuple24 _tmp74E;_tmp74E.f1=_tmp45C,_tmp74E.f2=r,_tmp74E.f3=te,_tmp74E.f4=flatten;_tmp74E;});
struct Cyc_List_List*_tmp45D=((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp46D->impl))->fields;struct Cyc_List_List*_tmp45E=_tmp45D;struct Cyc_Absyn_Aggrfield*_tmp464;struct Cyc_List_List*_tmp463;if(_tmp45E == 0){_LL11: _LL12:
 return 0;}else{_LL13: _tmp464=(struct Cyc_Absyn_Aggrfield*)_tmp45E->hd;_tmp463=_tmp45E->tl;_LL14: {
# 2638
struct Cyc_List_List*_tmp45F=Cyc_Tcutil_flatten_type_f(& env,_tmp464);
env.f4=0;{
struct Cyc_List_List*_tmp460=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp463);
struct Cyc_List_List*_tmp461=({struct Cyc_List_List*_tmp462=_region_malloc(r,sizeof(*_tmp462));_tmp462->hd=_tmp45F,_tmp462->tl=_tmp460;_tmp462;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp461);};}}_LL10:;};}else{goto _LL9;}default: goto _LL9;}case 6U: _LL3: _tmp46E=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp450)->f1;_LL4: {
# 2617
struct _tuple25 _tmp451=({struct _tuple25 _tmp74F;_tmp74F.f1=r,_tmp74F.f2=te,_tmp74F.f3=flatten;_tmp74F;});
# 2619
struct Cyc_List_List*_tmp452=_tmp46E;struct _tuple11*_tmp459;struct Cyc_List_List*_tmp458;if(_tmp452 == 0){_LLC: _LLD:
 return 0;}else{_LLE: _tmp459=(struct _tuple11*)_tmp452->hd;_tmp458=_tmp452->tl;_LLF: {
# 2622
struct Cyc_List_List*_tmp453=Cyc_Tcutil_rcopy_tqt(& _tmp451,_tmp459);
_tmp451.f3=0;{
struct Cyc_List_List*_tmp454=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple25*,struct _tuple11*),struct _tuple25*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_rcopy_tqt,& _tmp451,_tmp46E);
struct Cyc_List_List*_tmp455=({struct Cyc_List_List*_tmp457=_region_malloc(r,sizeof(*_tmp457));_tmp457->hd=_tmp453,_tmp457->tl=_tmp454;_tmp457;});
return({struct _RegionHandle*_tmp880=r;((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(_tmp880,({struct Cyc_List_List*_tmp456=_region_malloc(r,sizeof(*_tmp456));_tmp456->hd=_tmp453,_tmp456->tl=_tmp454;_tmp456;}));});};}}_LLB:;}case 7U: if(((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp450)->f1 == Cyc_Absyn_StructA){_LL7: _tmp46F=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp450)->f2;_LL8: {
# 2645
struct _tuple24 env=({struct _tuple24 _tmp750;_tmp750.f1=0,_tmp750.f2=r,_tmp750.f3=te,_tmp750.f4=flatten;_tmp750;});
struct Cyc_List_List*_tmp465=_tmp46F;struct Cyc_Absyn_Aggrfield*_tmp46B;struct Cyc_List_List*_tmp46A;if(_tmp465 == 0){_LL16: _LL17:
 return 0;}else{_LL18: _tmp46B=(struct Cyc_Absyn_Aggrfield*)_tmp465->hd;_tmp46A=_tmp465->tl;_LL19: {
# 2649
struct Cyc_List_List*_tmp466=Cyc_Tcutil_flatten_type_f(& env,_tmp46B);
env.f4=0;{
struct Cyc_List_List*_tmp467=((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*(*f)(struct _tuple24*,struct Cyc_Absyn_Aggrfield*),struct _tuple24*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(r,Cyc_Tcutil_flatten_type_f,& env,_tmp46A);
struct Cyc_List_List*_tmp468=({struct Cyc_List_List*_tmp469=_region_malloc(r,sizeof(*_tmp469));_tmp469->hd=_tmp466,_tmp469->tl=_tmp467;_tmp469;});
return((struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*x))Cyc_List_rflatten)(r,_tmp468);};}}_LL15:;}}else{goto _LL9;}default: _LL9: _LLA:
# 2655
 goto _LL0;}_LL0:;};}
# 2658
return({struct Cyc_List_List*_tmp471=_region_malloc(r,sizeof(*_tmp471));({struct _tuple11*_tmp882=({struct _tuple11*_tmp470=_region_malloc(r,sizeof(*_tmp470));({struct Cyc_Absyn_Tqual _tmp881=Cyc_Absyn_empty_tqual(0U);_tmp470->f1=_tmp881;}),_tmp470->f2=t1;_tmp470;});_tmp471->hd=_tmp882;}),_tmp471->tl=0;_tmp471;});}
# 2662
static int Cyc_Tcutil_sub_attributes(struct Cyc_List_List*a1,struct Cyc_List_List*a2){
{struct Cyc_List_List*t=a1;for(0;t != 0;t=t->tl){
void*_tmp472=(void*)t->hd;void*_tmp473=_tmp472;switch(*((int*)_tmp473)){case 23U: _LL1: _LL2:
 goto _LL4;case 4U: _LL3: _LL4:
 goto _LL6;case 20U: _LL5: _LL6:
 continue;default: _LL7: _LL8:
# 2669
 if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)t->hd,a2))return 0;}_LL0:;}}
# 2672
for(0;a2 != 0;a2=a2->tl){
if(!((int(*)(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x))Cyc_List_exists_c)(Cyc_Absyn_equal_att,(void*)a2->hd,a1))return 0;}
# 2675
return 1;}
# 2678
static int Cyc_Tcutil_isomorphic(void*t1,void*t2){
struct _tuple14 _tmp474=({struct _tuple14 _tmp751;({void*_tmp884=Cyc_Tcutil_compress(t1);_tmp751.f1=_tmp884;}),({void*_tmp883=Cyc_Tcutil_compress(t2);_tmp751.f2=_tmp883;});_tmp751;});struct _tuple14 _tmp475=_tmp474;enum Cyc_Absyn_Size_of _tmp477;enum Cyc_Absyn_Size_of _tmp476;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp475.f1)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp475.f1)->f1)->tag == 1U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp475.f2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp475.f2)->f1)->tag == 1U){_LL1: _tmp477=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp475.f1)->f1)->f2;_tmp476=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp475.f2)->f1)->f2;_LL2:
# 2681
 return((int)_tmp477 == (int)_tmp476 ||(int)_tmp477 == (int)2U &&(int)_tmp476 == (int)3U)||
(int)_tmp477 == (int)3U &&(int)_tmp476 == (int)Cyc_Absyn_Int_sz;}else{goto _LL3;}}else{goto _LL3;}}else{goto _LL3;}}else{_LL3: _LL4:
 return 0;}_LL0:;}
# 2689
int Cyc_Tcutil_subtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2692
if(Cyc_Tcutil_unify(t1,t2))return 1;
{struct Cyc_List_List*a=assume;for(0;a != 0;a=a->tl){
if(Cyc_Tcutil_unify(t1,(*((struct _tuple14*)a->hd)).f1)&& Cyc_Tcutil_unify(t2,(*((struct _tuple14*)a->hd)).f2))
return 1;}}
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);{
struct _tuple14 _tmp478=({struct _tuple14 _tmp752;_tmp752.f1=t1,_tmp752.f2=t2;_tmp752;});struct _tuple14 _tmp479=_tmp478;struct Cyc_Absyn_FnInfo _tmp4A3;struct Cyc_Absyn_FnInfo _tmp4A2;struct Cyc_Absyn_Datatypedecl*_tmp4A1;struct Cyc_Absyn_Datatypefield*_tmp4A0;struct Cyc_List_List*_tmp49F;struct Cyc_Absyn_Datatypedecl*_tmp49E;struct Cyc_List_List*_tmp49D;void*_tmp49C;struct Cyc_Absyn_Tqual _tmp49B;void*_tmp49A;void*_tmp499;void*_tmp498;void*_tmp497;void*_tmp496;struct Cyc_Absyn_Tqual _tmp495;void*_tmp494;void*_tmp493;void*_tmp492;void*_tmp491;switch(*((int*)_tmp479.f1)){case 3U: if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f2)->tag == 3U){_LL1: _tmp49C=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f1)->f1).elt_type;_tmp49B=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f1)->f1).elt_tq;_tmp49A=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f1)->f1).ptr_atts).rgn;_tmp499=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f1)->f1).ptr_atts).nullable;_tmp498=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f1)->f1).ptr_atts).bounds;_tmp497=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f1)->f1).ptr_atts).zero_term;_tmp496=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f2)->f1).elt_type;_tmp495=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f2)->f1).elt_tq;_tmp494=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f2)->f1).ptr_atts).rgn;_tmp493=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f2)->f1).ptr_atts).nullable;_tmp492=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f2)->f1).ptr_atts).bounds;_tmp491=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp479.f2)->f1).ptr_atts).zero_term;_LL2:
# 2704
 if(_tmp49B.real_const && !_tmp495.real_const)
return 0;
# 2707
if((!Cyc_Tcutil_unify(_tmp499,_tmp493)&&
 Cyc_Absyn_type2bool(0,_tmp499))&& !Cyc_Absyn_type2bool(0,_tmp493))
return 0;
# 2711
if((Cyc_Tcutil_unify(_tmp497,_tmp491)&& !
Cyc_Absyn_type2bool(0,_tmp497))&& Cyc_Absyn_type2bool(0,_tmp491))
return 0;
# 2715
if((!Cyc_Tcutil_unify(_tmp49A,_tmp494)&& !Cyc_Tcenv_region_outlives(te,_tmp49A,_tmp494))&& !
Cyc_Tcutil_subtype(te,assume,_tmp49A,_tmp494))
return 0;
# 2719
if(!Cyc_Tcutil_unify(_tmp498,_tmp492)){
struct Cyc_Absyn_Exp*_tmp47A=({void*_tmp885=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp885,_tmp498);});
struct Cyc_Absyn_Exp*_tmp47B=({void*_tmp886=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp886,_tmp492);});
if(_tmp47A != _tmp47B){
if((_tmp47A == 0 || _tmp47B == 0)|| !Cyc_Evexp_lte_const_exp(_tmp47B,_tmp47B))
return 0;}}
# 2729
if(!_tmp495.real_const && _tmp49B.real_const){
if(!Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp496)))
return 0;}{
# 2735
int _tmp47C=
({void*_tmp887=_tmp492;Cyc_Tcutil_unify(_tmp887,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,_tmp491);
# 2740
return(_tmp47C &&({struct Cyc_Tcenv_Tenv*_tmp88B=te;struct Cyc_List_List*_tmp88A=({struct Cyc_List_List*_tmp47E=_cycalloc(sizeof(*_tmp47E));({struct _tuple14*_tmp888=({struct _tuple14*_tmp47D=_cycalloc(sizeof(*_tmp47D));_tmp47D->f1=t1,_tmp47D->f2=t2;_tmp47D;});_tmp47E->hd=_tmp888;}),_tmp47E->tl=assume;_tmp47E;});void*_tmp889=_tmp49C;Cyc_Tcutil_ptrsubtype(_tmp88B,_tmp88A,_tmp889,_tmp496);})|| Cyc_Tcutil_unify(_tmp49C,_tmp496))|| Cyc_Tcutil_isomorphic(_tmp49C,_tmp496);};}else{goto _LL7;}case 0U: if(((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f1)->f1)->tag == 19U){if(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f1)->f1)->f1).KnownDatatypefield).tag == 2){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f2)->tag == 0U){if(((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f2)->f1)->tag == 18U){if(((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f2)->f1)->f1).KnownDatatype).tag == 2){_LL3: _tmp4A1=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f1)->f1)->f1).KnownDatatypefield).val).f1;_tmp4A0=(((((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f1)->f1)->f1).KnownDatatypefield).val).f2;_tmp49F=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f1)->f2;_tmp49E=*((((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f2)->f1)->f1).KnownDatatype).val;_tmp49D=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp479.f2)->f2;_LL4:
# 2747
 if(_tmp4A1 != _tmp49E && Cyc_Absyn_qvar_cmp(_tmp4A1->name,_tmp49E->name)!= 0)return 0;
# 2749
if(({int _tmp88C=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49F);_tmp88C != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp49D);}))return 0;
for(0;_tmp49F != 0;(_tmp49F=_tmp49F->tl,_tmp49D=_tmp49D->tl)){
if(!Cyc_Tcutil_unify((void*)_tmp49F->hd,(void*)((struct Cyc_List_List*)_check_null(_tmp49D))->hd))return 0;}
return 1;}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}}else{goto _LL7;}case 5U: if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp479.f2)->tag == 5U){_LL5: _tmp4A3=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp479.f1)->f1;_tmp4A2=((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp479.f2)->f1;_LL6:
# 2756
 if(_tmp4A3.tvars != 0 || _tmp4A2.tvars != 0){
struct Cyc_List_List*_tmp47F=_tmp4A3.tvars;
struct Cyc_List_List*_tmp480=_tmp4A2.tvars;
if(({int _tmp88D=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp47F);_tmp88D != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp480);}))return 0;{
struct Cyc_List_List*inst=0;
while(_tmp47F != 0){
if(!Cyc_Tcutil_unify_kindbound(((struct Cyc_Absyn_Tvar*)_tmp47F->hd)->kind,((struct Cyc_Absyn_Tvar*)((struct Cyc_List_List*)_check_null(_tmp480))->hd)->kind))return 0;
inst=({struct Cyc_List_List*_tmp482=_cycalloc(sizeof(*_tmp482));({struct _tuple18*_tmp88F=({struct _tuple18*_tmp481=_cycalloc(sizeof(*_tmp481));_tmp481->f1=(struct Cyc_Absyn_Tvar*)_tmp480->hd,({void*_tmp88E=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp47F->hd);_tmp481->f2=_tmp88E;});_tmp481;});_tmp482->hd=_tmp88F;}),_tmp482->tl=inst;_tmp482;});
_tmp47F=_tmp47F->tl;
_tmp480=_tmp480->tl;}
# 2767
if(inst != 0){
_tmp4A3.tvars=0;
_tmp4A2.tvars=0;
return({struct Cyc_Tcenv_Tenv*_tmp892=te;struct Cyc_List_List*_tmp891=assume;void*_tmp890=(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp483=_cycalloc(sizeof(*_tmp483));_tmp483->tag=5U,_tmp483->f1=_tmp4A3;_tmp483;});Cyc_Tcutil_subtype(_tmp892,_tmp891,_tmp890,(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp484=_cycalloc(sizeof(*_tmp484));_tmp484->tag=5U,_tmp484->f1=_tmp4A2;_tmp484;}));});}};}
# 2774
if(!Cyc_Tcutil_subtype(te,assume,_tmp4A3.ret_type,_tmp4A2.ret_type))return 0;{
struct Cyc_List_List*_tmp485=_tmp4A3.args;
struct Cyc_List_List*_tmp486=_tmp4A2.args;
# 2779
if(({int _tmp893=((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp485);_tmp893 != ((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp486);}))return 0;
# 2781
for(0;_tmp485 != 0;(_tmp485=_tmp485->tl,_tmp486=_tmp486->tl)){
struct _tuple9 _tmp487=*((struct _tuple9*)_tmp485->hd);struct _tuple9 _tmp488=_tmp487;struct Cyc_Absyn_Tqual _tmp48E;void*_tmp48D;_LLA: _tmp48E=_tmp488.f2;_tmp48D=_tmp488.f3;_LLB:;{
struct _tuple9 _tmp489=*((struct _tuple9*)((struct Cyc_List_List*)_check_null(_tmp486))->hd);struct _tuple9 _tmp48A=_tmp489;struct Cyc_Absyn_Tqual _tmp48C;void*_tmp48B;_LLD: _tmp48C=_tmp48A.f2;_tmp48B=_tmp48A.f3;_LLE:;
# 2785
if(_tmp48C.real_const && !_tmp48E.real_const || !Cyc_Tcutil_subtype(te,assume,_tmp48B,_tmp48D))
return 0;};}
# 2789
if(_tmp4A3.c_varargs != _tmp4A2.c_varargs)return 0;
if(_tmp4A3.cyc_varargs != 0 && _tmp4A2.cyc_varargs != 0){
struct Cyc_Absyn_VarargInfo _tmp48F=*_tmp4A3.cyc_varargs;
struct Cyc_Absyn_VarargInfo _tmp490=*_tmp4A2.cyc_varargs;
# 2794
if((_tmp490.tq).real_const && !(_tmp48F.tq).real_const || !
Cyc_Tcutil_subtype(te,assume,_tmp490.type,_tmp48F.type))
return 0;}else{
if(_tmp4A3.cyc_varargs != 0 || _tmp4A2.cyc_varargs != 0)return 0;}
# 2799
if(!({void*_tmp894=(void*)_check_null(_tmp4A3.effect);Cyc_Tcutil_subset_effect(1,_tmp894,(void*)_check_null(_tmp4A2.effect));}))return 0;
# 2801
if(!Cyc_Tcutil_sub_rgnpo(_tmp4A3.rgn_po,_tmp4A2.rgn_po))return 0;
# 2803
if(!Cyc_Tcutil_sub_attributes(_tmp4A3.attributes,_tmp4A2.attributes))return 0;
# 2805
if(!Cyc_Tcutil_check_logical_implication(_tmp4A2.requires_relns,_tmp4A3.requires_relns))
return 0;
# 2808
if(!Cyc_Tcutil_check_logical_implication(_tmp4A3.ensures_relns,_tmp4A2.ensures_relns))
return 0;
# 2811
return 1;};}else{goto _LL7;}default: _LL7: _LL8:
 return 0;}_LL0:;};}
# 2823 "tcutil.cyc"
static int Cyc_Tcutil_ptrsubtype(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*assume,void*t1,void*t2){
# 2825
struct Cyc_List_List*tqs1=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t1);
struct Cyc_List_List*tqs2=Cyc_Tcutil_flatten_type(Cyc_Core_heap_region,1,te,t2);
for(0;tqs2 != 0;(tqs2=tqs2->tl,tqs1=tqs1->tl)){
if(tqs1 == 0)return 0;{
struct _tuple11*_tmp4A4=(struct _tuple11*)tqs1->hd;struct _tuple11*_tmp4A5=_tmp4A4;struct Cyc_Absyn_Tqual _tmp4AB;void*_tmp4AA;_LL1: _tmp4AB=_tmp4A5->f1;_tmp4AA=_tmp4A5->f2;_LL2:;{
struct _tuple11*_tmp4A6=(struct _tuple11*)tqs2->hd;struct _tuple11*_tmp4A7=_tmp4A6;struct Cyc_Absyn_Tqual _tmp4A9;void*_tmp4A8;_LL4: _tmp4A9=_tmp4A7->f1;_tmp4A8=_tmp4A7->f2;_LL5:;
# 2832
if(_tmp4AB.real_const && !_tmp4A9.real_const)return 0;
# 2834
if((_tmp4A9.real_const || Cyc_Tcutil_kind_leq(& Cyc_Tcutil_ak,Cyc_Tcutil_type_kind(_tmp4A8)))&&
 Cyc_Tcutil_subtype(te,assume,_tmp4AA,_tmp4A8))
# 2837
continue;
# 2839
if(Cyc_Tcutil_unify(_tmp4AA,_tmp4A8))
# 2841
continue;
# 2843
if(Cyc_Tcutil_isomorphic(_tmp4AA,_tmp4A8))
# 2845
continue;
# 2848
return 0;};};}
# 2850
return 1;}
# 2855
enum Cyc_Absyn_Coercion Cyc_Tcutil_castable(struct Cyc_Tcenv_Tenv*te,unsigned loc,void*t1,void*t2){
if(Cyc_Tcutil_unify(t1,t2))
return Cyc_Absyn_No_coercion;
t1=Cyc_Tcutil_compress(t1);
t2=Cyc_Tcutil_compress(t2);
# 2861
{void*_tmp4AC=t2;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AC)->tag == 0U)switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AC)->f1)){case 0U: _LL1: _LL2:
 return Cyc_Absyn_No_coercion;case 1U: switch(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AC)->f1)->f2){case Cyc_Absyn_Int_sz: _LL3: _LL4:
# 2864
 goto _LL6;case Cyc_Absyn_Long_sz: _LL5: _LL6:
# 2866
 if((int)(Cyc_Tcutil_type_kind(t1))->kind == (int)Cyc_Absyn_BoxKind)return Cyc_Absyn_Other_coercion;
goto _LL0;default: goto _LL7;}default: goto _LL7;}else{_LL7: _LL8:
 goto _LL0;}_LL0:;}{
# 2870
void*_tmp4AD=t1;void*_tmp4D2;struct Cyc_Absyn_Enumdecl*_tmp4D1;void*_tmp4D0;struct Cyc_Absyn_Tqual _tmp4CF;struct Cyc_Absyn_Exp*_tmp4CE;void*_tmp4CD;void*_tmp4CC;struct Cyc_Absyn_Tqual _tmp4CB;void*_tmp4CA;void*_tmp4C9;void*_tmp4C8;void*_tmp4C7;switch(*((int*)_tmp4AD)){case 3U: _LLA: _tmp4CC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).elt_type;_tmp4CB=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).elt_tq;_tmp4CA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).ptr_atts).rgn;_tmp4C9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).ptr_atts).nullable;_tmp4C8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).ptr_atts).bounds;_tmp4C7=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AD)->f1).ptr_atts).zero_term;_LLB:
# 2879
{void*_tmp4AE=t2;void*_tmp4BD;struct Cyc_Absyn_Tqual _tmp4BC;void*_tmp4BB;void*_tmp4BA;void*_tmp4B9;void*_tmp4B8;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AE)->tag == 3U){_LL19: _tmp4BD=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AE)->f1).elt_type;_tmp4BC=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AE)->f1).elt_tq;_tmp4BB=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AE)->f1).ptr_atts).rgn;_tmp4BA=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AE)->f1).ptr_atts).nullable;_tmp4B9=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AE)->f1).ptr_atts).bounds;_tmp4B8=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp4AE)->f1).ptr_atts).zero_term;_LL1A: {
# 2883
enum Cyc_Absyn_Coercion coercion=3U;
struct Cyc_List_List*_tmp4AF=({struct Cyc_List_List*_tmp4B7=_cycalloc(sizeof(*_tmp4B7));({struct _tuple14*_tmp895=({struct _tuple14*_tmp4B6=_cycalloc(sizeof(*_tmp4B6));_tmp4B6->f1=t1,_tmp4B6->f2=t2;_tmp4B6;});_tmp4B7->hd=_tmp895;}),_tmp4B7->tl=0;_tmp4B7;});
int _tmp4B0=_tmp4BC.real_const || !_tmp4CB.real_const;
# 2897 "tcutil.cyc"
int _tmp4B1=
({void*_tmp896=_tmp4B9;Cyc_Tcutil_unify(_tmp896,Cyc_Absyn_bounds_one());})&& !Cyc_Tcutil_force_type2bool(0,_tmp4B8);
# 2900
int _tmp4B2=_tmp4B0 &&(
((_tmp4B1 && Cyc_Tcutil_ptrsubtype(te,_tmp4AF,_tmp4CC,_tmp4BD)||
 Cyc_Tcutil_unify(_tmp4CC,_tmp4BD))|| Cyc_Tcutil_isomorphic(_tmp4CC,_tmp4BD))|| Cyc_Tcutil_unify(_tmp4BD,Cyc_Absyn_void_type));
# 2904
Cyc_Tcutil_t1_failure=t1;
Cyc_Tcutil_t2_failure=t2;{
int zeroterm_ok=Cyc_Tcutil_unify(_tmp4C7,_tmp4B8)|| !Cyc_Absyn_type2bool(0,_tmp4B8);
# 2908
int _tmp4B3=_tmp4B2?0:((Cyc_Tcutil_is_bits_only_type(_tmp4CC)&& Cyc_Tcutil_is_char_type(_tmp4BD))&& !
Cyc_Tcutil_force_type2bool(0,_tmp4B8))&&(
_tmp4BC.real_const || !_tmp4CB.real_const);
int bounds_ok=Cyc_Tcutil_unify(_tmp4C8,_tmp4B9);
if(!bounds_ok && !_tmp4B3){
struct Cyc_Absyn_Exp*_tmp4B4=({void*_tmp897=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp897,_tmp4C8);});
struct Cyc_Absyn_Exp*_tmp4B5=({void*_tmp898=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp898,_tmp4B9);});
if((_tmp4B4 != 0 && _tmp4B5 != 0)&& Cyc_Evexp_lte_const_exp(_tmp4B5,_tmp4B4))
bounds_ok=1;else{
if(_tmp4B4 == 0 || _tmp4B5 == 0)
bounds_ok=1;}}{
# 2920
int t1_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4C9);
int t2_nullable=Cyc_Tcutil_force_type2bool(0,_tmp4BA);
if(t1_nullable && !t2_nullable)
coercion=2U;
# 2927
if(((bounds_ok && zeroterm_ok)&&(_tmp4B2 || _tmp4B3))&&(
Cyc_Tcutil_unify(_tmp4CA,_tmp4BB)|| Cyc_Tcenv_region_outlives(te,_tmp4CA,_tmp4BB)))
return coercion;else{
return Cyc_Absyn_Unknown_coercion;}};};}}else{_LL1B: _LL1C:
 goto _LL18;}_LL18:;}
# 2933
return Cyc_Absyn_Unknown_coercion;case 4U: _LLC: _tmp4D0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AD)->f1).elt_type;_tmp4CF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AD)->f1).tq;_tmp4CE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AD)->f1).num_elts;_tmp4CD=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4AD)->f1).zero_term;_LLD:
# 2935
{void*_tmp4BE=t2;void*_tmp4C2;struct Cyc_Absyn_Tqual _tmp4C1;struct Cyc_Absyn_Exp*_tmp4C0;void*_tmp4BF;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BE)->tag == 4U){_LL1E: _tmp4C2=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BE)->f1).elt_type;_tmp4C1=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BE)->f1).tq;_tmp4C0=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BE)->f1).num_elts;_tmp4BF=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp4BE)->f1).zero_term;_LL1F: {
# 2937
int okay;
okay=
(((_tmp4CE != 0 && _tmp4C0 != 0)&& Cyc_Tcutil_unify(_tmp4CD,_tmp4BF))&&
 Cyc_Evexp_lte_const_exp(_tmp4C0,_tmp4CE))&&
 Cyc_Evexp_lte_const_exp(_tmp4CE,_tmp4C0);
return
# 2944
(okay && Cyc_Tcutil_unify(_tmp4D0,_tmp4C2))&&(!_tmp4CF.real_const || _tmp4C1.real_const)?Cyc_Absyn_No_coercion: Cyc_Absyn_Unknown_coercion;}}else{_LL20: _LL21:
# 2946
 return Cyc_Absyn_Unknown_coercion;}_LL1D:;}
# 2948
return Cyc_Absyn_Unknown_coercion;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f1)){case 15U: _LLE: _tmp4D1=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f1)->f2;_LLF:
# 2952
{void*_tmp4C3=t2;struct Cyc_Absyn_Enumdecl*_tmp4C4;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C3)->tag == 0U){if(((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C3)->f1)->tag == 15U){_LL23: _tmp4C4=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C3)->f1)->f2;_LL24:
# 2954
 if((((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4D1))->fields != 0 &&((struct Cyc_Absyn_Enumdecl*)_check_null(_tmp4C4))->fields != 0)&&({
int _tmp899=((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4D1->fields))->v);_tmp899 >= ((int(*)(struct Cyc_List_List*x))Cyc_List_length)((struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp4C4->fields))->v);}))
return Cyc_Absyn_Other_coercion;
goto _LL22;}else{goto _LL25;}}else{_LL25: _LL26:
 goto _LL22;}_LL22:;}
# 2960
goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
# 2963
 return Cyc_Tcutil_is_strict_arithmetic_type(t2)?Cyc_Absyn_Other_coercion: Cyc_Absyn_Unknown_coercion;case 3U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f2 != 0){_LL14: _tmp4D2=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4AD)->f2)->hd;_LL15:
# 2966
{void*_tmp4C5=t2;void*_tmp4C6;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C5)->tag == 0U){if(((struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C5)->f1)->tag == 3U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C5)->f2 != 0){_LL28: _tmp4C6=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp4C5)->f2)->hd;_LL29:
# 2968
 if(Cyc_Tcenv_region_outlives(te,_tmp4D2,_tmp4C6))return Cyc_Absyn_No_coercion;
goto _LL27;}else{goto _LL2A;}}else{goto _LL2A;}}else{_LL2A: _LL2B:
 goto _LL27;}_LL27:;}
# 2972
return Cyc_Absyn_Unknown_coercion;}else{goto _LL16;}default: goto _LL16;}default: _LL16: _LL17:
 return Cyc_Absyn_Unknown_coercion;}_LL9:;};}
# 2978
void Cyc_Tcutil_unchecked_cast(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e,void*t,enum Cyc_Absyn_Coercion c){
if(!Cyc_Tcutil_unify((void*)_check_null(e->topt),t)){
struct Cyc_Absyn_Exp*_tmp4D3=Cyc_Absyn_copy_exp(e);
({void*_tmp89A=(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp4D4=_cycalloc(sizeof(*_tmp4D4));_tmp4D4->tag=14U,_tmp4D4->f1=t,_tmp4D4->f2=_tmp4D3,_tmp4D4->f3=0,_tmp4D4->f4=c;_tmp4D4;});e->r=_tmp89A;});
e->topt=t;}}
# 2987
static int Cyc_Tcutil_tvar_id_counter=0;
int Cyc_Tcutil_new_tvar_id(){
return Cyc_Tcutil_tvar_id_counter ++;}
# 2992
static int Cyc_Tcutil_tvar_counter=0;
struct Cyc_Absyn_Tvar*Cyc_Tcutil_new_tvar(void*k){
int i=Cyc_Tcutil_tvar_counter ++;
struct _fat_ptr s=(struct _fat_ptr)({struct Cyc_Int_pa_PrintArg_struct _tmp4DA=({struct Cyc_Int_pa_PrintArg_struct _tmp753;_tmp753.tag=1U,_tmp753.f1=(unsigned long)i;_tmp753;});void*_tmp4D8[1U];_tmp4D8[0]=& _tmp4DA;({struct _fat_ptr _tmp89B=({const char*_tmp4D9="#%d";_tag_fat(_tmp4D9,sizeof(char),4U);});Cyc_aprintf(_tmp89B,_tag_fat(_tmp4D8,sizeof(void*),1U));});});
return({struct Cyc_Absyn_Tvar*_tmp4D7=_cycalloc(sizeof(*_tmp4D7));({struct _fat_ptr*_tmp89C=({unsigned _tmp4D6=1;struct _fat_ptr*_tmp4D5=_cycalloc(_check_times(_tmp4D6,sizeof(struct _fat_ptr)));_tmp4D5[0]=s;_tmp4D5;});_tmp4D7->name=_tmp89C;}),_tmp4D7->identity=- 1,_tmp4D7->kind=k;_tmp4D7;});}
# 2999
int Cyc_Tcutil_is_temp_tvar(struct Cyc_Absyn_Tvar*t){
struct _fat_ptr _tmp4DB=*t->name;
return(int)*((const char*)_check_fat_subscript(_tmp4DB,sizeof(char),0))== (int)'#';}
# 3004
void Cyc_Tcutil_rewrite_temp_tvar(struct Cyc_Absyn_Tvar*t){
({struct Cyc_String_pa_PrintArg_struct _tmp4DE=({struct Cyc_String_pa_PrintArg_struct _tmp754;_tmp754.tag=0U,_tmp754.f1=(struct _fat_ptr)((struct _fat_ptr)*t->name);_tmp754;});void*_tmp4DC[1U];_tmp4DC[0]=& _tmp4DE;({struct _fat_ptr _tmp89D=({const char*_tmp4DD="%s";_tag_fat(_tmp4DD,sizeof(char),3U);});Cyc_printf(_tmp89D,_tag_fat(_tmp4DC,sizeof(void*),1U));});});
if(!Cyc_Tcutil_is_temp_tvar(t))return;{
struct _fat_ptr _tmp4DF=({struct _fat_ptr _tmp89E=({const char*_tmp4E5="`";_tag_fat(_tmp4E5,sizeof(char),2U);});Cyc_strconcat(_tmp89E,(struct _fat_ptr)*t->name);});
({struct _fat_ptr _tmp4E0=_fat_ptr_plus(_tmp4DF,sizeof(char),1);char _tmp4E1=*((char*)_check_fat_subscript(_tmp4E0,sizeof(char),0U));char _tmp4E2='t';if(_get_fat_size(_tmp4E0,sizeof(char))== 1U &&(_tmp4E1 == 0 && _tmp4E2 != 0))_throw_arraybounds();*((char*)_tmp4E0.curr)=_tmp4E2;});
({struct _fat_ptr*_tmp89F=({unsigned _tmp4E4=1;struct _fat_ptr*_tmp4E3=_cycalloc(_check_times(_tmp4E4,sizeof(struct _fat_ptr)));_tmp4E3[0]=(struct _fat_ptr)_tmp4DF;_tmp4E3;});t->name=_tmp89F;});};}
# 3013
void*Cyc_Tcutil_fndecl2type(struct Cyc_Absyn_Fndecl*fd){
if(fd->cached_type == 0){
# 3020
struct Cyc_List_List*_tmp4E6=0;
{struct Cyc_List_List*atts=(fd->i).attributes;for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd))
_tmp4E6=({struct Cyc_List_List*_tmp4E7=_cycalloc(sizeof(*_tmp4E7));_tmp4E7->hd=(void*)atts->hd,_tmp4E7->tl=_tmp4E6;_tmp4E7;});}}{
struct Cyc_Absyn_FnInfo _tmp4E8=fd->i;
_tmp4E8.attributes=_tmp4E6;
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp4E9=_cycalloc(sizeof(*_tmp4E9));_tmp4E9->tag=5U,_tmp4E9->f1=_tmp4E8;_tmp4E9;});};}
# 3028
return(void*)_check_null(fd->cached_type);}
# 3034
static void Cyc_Tcutil_replace_rop(struct Cyc_List_List*args,union Cyc_Relations_RelnOp*rop){
# 3036
union Cyc_Relations_RelnOp _tmp4EA=*rop;union Cyc_Relations_RelnOp _tmp4EB=_tmp4EA;struct Cyc_Absyn_Vardecl*_tmp4FE;struct Cyc_Absyn_Vardecl*_tmp4FD;switch((_tmp4EB.RNumelts).tag){case 2U: _LL1: _tmp4FD=(_tmp4EB.RVar).val;_LL2: {
# 3038
struct _tuple1 _tmp4EC=*_tmp4FD->name;struct _tuple1 _tmp4ED=_tmp4EC;union Cyc_Absyn_Nmspace _tmp4F4;struct _fat_ptr*_tmp4F3;_LL8: _tmp4F4=_tmp4ED.f1;_tmp4F3=_tmp4ED.f2;_LL9:;
if(!((int)((_tmp4F4.Loc_n).tag == 4)))goto _LL0;
if(({struct _fat_ptr _tmp8A0=(struct _fat_ptr)*_tmp4F3;Cyc_strcmp(_tmp8A0,({const char*_tmp4EE="return_value";_tag_fat(_tmp4EE,sizeof(char),13U);}));})== 0){
({union Cyc_Relations_RelnOp _tmp8A1=Cyc_Relations_RReturn();*rop=_tmp8A1;});
goto _LL0;}{
# 3044
unsigned c=0U;
{struct Cyc_List_List*_tmp4EF=args;for(0;_tmp4EF != 0;(_tmp4EF=_tmp4EF->tl,c ++)){
struct _tuple9*_tmp4F0=(struct _tuple9*)_tmp4EF->hd;struct _tuple9*_tmp4F1=_tmp4F0;struct _fat_ptr*_tmp4F2;_LLB: _tmp4F2=_tmp4F1->f1;_LLC:;
if(_tmp4F2 != 0){
if(Cyc_strcmp((struct _fat_ptr)*_tmp4F3,(struct _fat_ptr)*_tmp4F2)== 0){
({union Cyc_Relations_RelnOp _tmp8A2=Cyc_Relations_RParam(c);*rop=_tmp8A2;});
break;}}}}
# 3054
goto _LL0;};}case 3U: _LL3: _tmp4FE=(_tmp4EB.RNumelts).val;_LL4: {
# 3056
struct _tuple1 _tmp4F5=*_tmp4FE->name;struct _tuple1 _tmp4F6=_tmp4F5;union Cyc_Absyn_Nmspace _tmp4FC;struct _fat_ptr*_tmp4FB;_LLE: _tmp4FC=_tmp4F6.f1;_tmp4FB=_tmp4F6.f2;_LLF:;
if(!((int)((_tmp4FC.Loc_n).tag == 4)))goto _LL0;{
unsigned c=0U;
{struct Cyc_List_List*_tmp4F7=args;for(0;_tmp4F7 != 0;(_tmp4F7=_tmp4F7->tl,c ++)){
struct _tuple9*_tmp4F8=(struct _tuple9*)_tmp4F7->hd;struct _tuple9*_tmp4F9=_tmp4F8;struct _fat_ptr*_tmp4FA;_LL11: _tmp4FA=_tmp4F9->f1;_LL12:;
if(_tmp4FA != 0){
if(Cyc_strcmp((struct _fat_ptr)*_tmp4FB,(struct _fat_ptr)*_tmp4FA)== 0){
({union Cyc_Relations_RelnOp _tmp8A3=Cyc_Relations_RParamNumelts(c);*rop=_tmp8A3;});
break;}}}}
# 3068
goto _LL0;};}default: _LL5: _LL6:
 goto _LL0;}_LL0:;}
# 3073
void Cyc_Tcutil_replace_rops(struct Cyc_List_List*args,struct Cyc_Relations_Reln*r){
# 3075
Cyc_Tcutil_replace_rop(args,& r->rop1);
Cyc_Tcutil_replace_rop(args,& r->rop2);}
# 3079
static struct Cyc_List_List*Cyc_Tcutil_extract_relns(struct Cyc_List_List*args,struct Cyc_Absyn_Exp*e){
# 3081
if(e == 0)return 0;{
struct Cyc_List_List*_tmp4FF=Cyc_Relations_exp2relns(Cyc_Core_heap_region,e);
((void(*)(void(*f)(struct Cyc_List_List*,struct Cyc_Relations_Reln*),struct Cyc_List_List*env,struct Cyc_List_List*x))Cyc_List_iter_c)(Cyc_Tcutil_replace_rops,args,_tmp4FF);
return _tmp4FF;};}
# 3088
void*Cyc_Tcutil_snd_tqt(struct _tuple11*t){return(*t).f2;}
static struct _tuple11*Cyc_Tcutil_map2_tq(struct _tuple11*pr,void*t){
struct _tuple11*_tmp500=pr;struct Cyc_Absyn_Tqual _tmp503;void*_tmp502;_LL1: _tmp503=_tmp500->f1;_tmp502=_tmp500->f2;_LL2:;
if(_tmp502 == t)return pr;else{
return({struct _tuple11*_tmp501=_cycalloc(sizeof(*_tmp501));_tmp501->f1=_tmp503,_tmp501->f2=t;_tmp501;});}}struct _tuple26{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;};struct _tuple27{struct _tuple26*f1;void*f2;};
# 3094
static struct _tuple27*Cyc_Tcutil_substitute_f1(struct _RegionHandle*rgn,struct _tuple9*y){
# 3096
return({struct _tuple27*_tmp505=_region_malloc(rgn,sizeof(*_tmp505));({struct _tuple26*_tmp8A4=({struct _tuple26*_tmp504=_region_malloc(rgn,sizeof(*_tmp504));_tmp504->f1=(*y).f1,_tmp504->f2=(*y).f2;_tmp504;});_tmp505->f1=_tmp8A4;}),_tmp505->f2=(*y).f3;_tmp505;});}
# 3098
static struct _tuple9*Cyc_Tcutil_substitute_f2(struct _tuple9*orig_arg,void*t){
# 3100
struct _tuple9 _tmp506=*orig_arg;struct _tuple9 _tmp507=_tmp506;struct _fat_ptr*_tmp50B;struct Cyc_Absyn_Tqual _tmp50A;void*_tmp509;_LL1: _tmp50B=_tmp507.f1;_tmp50A=_tmp507.f2;_tmp509=_tmp507.f3;_LL2:;
if(t == _tmp509)return orig_arg;
return({struct _tuple9*_tmp508=_cycalloc(sizeof(*_tmp508));_tmp508->f1=_tmp50B,_tmp508->f2=_tmp50A,_tmp508->f3=t;_tmp508;});}
# 3104
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts);
# 3109
static struct Cyc_Absyn_Exp*Cyc_Tcutil_copye(struct Cyc_Absyn_Exp*old,void*r){
# 3111
return({struct Cyc_Absyn_Exp*_tmp50C=_cycalloc(sizeof(*_tmp50C));_tmp50C->topt=old->topt,_tmp50C->r=r,_tmp50C->loc=old->loc,_tmp50C->annot=old->annot;_tmp50C;});}
# 3116
struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubsexp(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
void*_tmp50D=e->r;void*_tmp50E=_tmp50D;void*_tmp54E;void*_tmp54D;struct Cyc_List_List*_tmp54C;struct Cyc_Absyn_Exp*_tmp54B;struct Cyc_Absyn_Exp*_tmp54A;void*_tmp549;void*_tmp548;struct Cyc_Absyn_Exp*_tmp547;int _tmp546;enum Cyc_Absyn_Coercion _tmp545;struct Cyc_Absyn_Exp*_tmp544;struct Cyc_Absyn_Exp*_tmp543;struct Cyc_Absyn_Exp*_tmp542;struct Cyc_Absyn_Exp*_tmp541;struct Cyc_Absyn_Exp*_tmp540;struct Cyc_Absyn_Exp*_tmp53F;struct Cyc_Absyn_Exp*_tmp53E;struct Cyc_Absyn_Exp*_tmp53D;struct Cyc_Absyn_Exp*_tmp53C;enum Cyc_Absyn_Primop _tmp53B;struct Cyc_List_List*_tmp53A;switch(*((int*)_tmp50E)){case 0U: _LL1: _LL2:
 goto _LL4;case 32U: _LL3: _LL4:
 goto _LL6;case 33U: _LL5: _LL6:
 goto _LL8;case 2U: _LL7: _LL8:
 goto _LLA;case 1U: _LL9: _LLA:
 return e;case 3U: _LLB: _tmp53B=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_tmp53A=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp50E)->f2;_LLC:
# 3125
 if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp53A)== 1){
struct Cyc_Absyn_Exp*_tmp50F=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp53A))->hd;
struct Cyc_Absyn_Exp*_tmp510=Cyc_Tcutil_rsubsexp(r,inst,_tmp50F);
if(_tmp510 == _tmp50F)return e;
return({struct Cyc_Absyn_Exp*_tmp8A6=e;Cyc_Tcutil_copye(_tmp8A6,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp512=_cycalloc(sizeof(*_tmp512));_tmp512->tag=3U,_tmp512->f1=_tmp53B,({struct Cyc_List_List*_tmp8A5=({struct Cyc_Absyn_Exp*_tmp511[1U];_tmp511[0]=_tmp510;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp511,sizeof(struct Cyc_Absyn_Exp*),1U));});_tmp512->f2=_tmp8A5;});_tmp512;}));});}else{
if(((int(*)(struct Cyc_List_List*x))Cyc_List_length)(_tmp53A)== 2){
struct Cyc_Absyn_Exp*_tmp513=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp53A))->hd;
struct Cyc_Absyn_Exp*_tmp514=(struct Cyc_Absyn_Exp*)((struct Cyc_List_List*)_check_null(_tmp53A->tl))->hd;
struct Cyc_Absyn_Exp*_tmp515=Cyc_Tcutil_rsubsexp(r,inst,_tmp513);
struct Cyc_Absyn_Exp*_tmp516=Cyc_Tcutil_rsubsexp(r,inst,_tmp514);
if(_tmp515 == _tmp513 && _tmp516 == _tmp514)return e;
return({struct Cyc_Absyn_Exp*_tmp8A8=e;Cyc_Tcutil_copye(_tmp8A8,(void*)({struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*_tmp518=_cycalloc(sizeof(*_tmp518));_tmp518->tag=3U,_tmp518->f1=_tmp53B,({struct Cyc_List_List*_tmp8A7=({struct Cyc_Absyn_Exp*_tmp517[2U];_tmp517[0]=_tmp515,_tmp517[1]=_tmp516;((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmp517,sizeof(struct Cyc_Absyn_Exp*),2U));});_tmp518->f2=_tmp8A7;});_tmp518;}));});}else{
return({void*_tmp519=0U;({struct _fat_ptr _tmp8A9=({const char*_tmp51A="primop does not have 1 or 2 args!";_tag_fat(_tmp51A,sizeof(char),34U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp8A9,_tag_fat(_tmp519,sizeof(void*),0U));});});}}case 6U: _LLD: _tmp53E=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_tmp53D=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50E)->f2;_tmp53C=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp50E)->f3;_LLE: {
# 3139
struct Cyc_Absyn_Exp*_tmp51B=Cyc_Tcutil_rsubsexp(r,inst,_tmp53E);
struct Cyc_Absyn_Exp*_tmp51C=Cyc_Tcutil_rsubsexp(r,inst,_tmp53D);
struct Cyc_Absyn_Exp*_tmp51D=Cyc_Tcutil_rsubsexp(r,inst,_tmp53C);
if((_tmp51B == _tmp53E && _tmp51C == _tmp53D)&& _tmp51D == _tmp53C)return e;
return({struct Cyc_Absyn_Exp*_tmp8AA=e;Cyc_Tcutil_copye(_tmp8AA,(void*)({struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*_tmp51E=_cycalloc(sizeof(*_tmp51E));_tmp51E->tag=6U,_tmp51E->f1=_tmp51B,_tmp51E->f2=_tmp51C,_tmp51E->f3=_tmp51D;_tmp51E;}));});}case 7U: _LLF: _tmp540=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_tmp53F=((struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*)_tmp50E)->f2;_LL10: {
# 3145
struct Cyc_Absyn_Exp*_tmp51F=Cyc_Tcutil_rsubsexp(r,inst,_tmp540);
struct Cyc_Absyn_Exp*_tmp520=Cyc_Tcutil_rsubsexp(r,inst,_tmp53F);
if(_tmp51F == _tmp540 && _tmp520 == _tmp53F)return e;
return({struct Cyc_Absyn_Exp*_tmp8AB=e;Cyc_Tcutil_copye(_tmp8AB,(void*)({struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct*_tmp521=_cycalloc(sizeof(*_tmp521));_tmp521->tag=7U,_tmp521->f1=_tmp51F,_tmp521->f2=_tmp520;_tmp521;}));});}case 8U: _LL11: _tmp542=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_tmp541=((struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*)_tmp50E)->f2;_LL12: {
# 3150
struct Cyc_Absyn_Exp*_tmp522=Cyc_Tcutil_rsubsexp(r,inst,_tmp542);
struct Cyc_Absyn_Exp*_tmp523=Cyc_Tcutil_rsubsexp(r,inst,_tmp541);
if(_tmp522 == _tmp542 && _tmp523 == _tmp541)return e;
return({struct Cyc_Absyn_Exp*_tmp8AC=e;Cyc_Tcutil_copye(_tmp8AC,(void*)({struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct*_tmp524=_cycalloc(sizeof(*_tmp524));_tmp524->tag=8U,_tmp524->f1=_tmp522,_tmp524->f2=_tmp523;_tmp524;}));});}case 9U: _LL13: _tmp544=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_tmp543=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp50E)->f2;_LL14: {
# 3155
struct Cyc_Absyn_Exp*_tmp525=Cyc_Tcutil_rsubsexp(r,inst,_tmp544);
struct Cyc_Absyn_Exp*_tmp526=Cyc_Tcutil_rsubsexp(r,inst,_tmp543);
if(_tmp525 == _tmp544 && _tmp526 == _tmp543)return e;
return({struct Cyc_Absyn_Exp*_tmp8AD=e;Cyc_Tcutil_copye(_tmp8AD,(void*)({struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*_tmp527=_cycalloc(sizeof(*_tmp527));_tmp527->tag=9U,_tmp527->f1=_tmp525,_tmp527->f2=_tmp526;_tmp527;}));});}case 14U: _LL15: _tmp548=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_tmp547=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50E)->f2;_tmp546=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50E)->f3;_tmp545=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp50E)->f4;_LL16: {
# 3160
struct Cyc_Absyn_Exp*_tmp528=Cyc_Tcutil_rsubsexp(r,inst,_tmp547);
void*_tmp529=Cyc_Tcutil_rsubstitute(r,inst,_tmp548);
if(_tmp528 == _tmp547 && _tmp529 == _tmp548)return e;
return({struct Cyc_Absyn_Exp*_tmp8AE=e;Cyc_Tcutil_copye(_tmp8AE,(void*)({struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*_tmp52A=_cycalloc(sizeof(*_tmp52A));_tmp52A->tag=14U,_tmp52A->f1=_tmp529,_tmp52A->f2=_tmp528,_tmp52A->f3=_tmp546,_tmp52A->f4=_tmp545;_tmp52A;}));});}case 17U: _LL17: _tmp549=(void*)((struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_LL18: {
# 3165
void*_tmp52B=Cyc_Tcutil_rsubstitute(r,inst,_tmp549);
if(_tmp52B == _tmp549)return e;
return({struct Cyc_Absyn_Exp*_tmp8AF=e;Cyc_Tcutil_copye(_tmp8AF,(void*)({struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct*_tmp52C=_cycalloc(sizeof(*_tmp52C));_tmp52C->tag=17U,_tmp52C->f1=_tmp52B;_tmp52C;}));});}case 18U: _LL19: _tmp54A=((struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_LL1A: {
# 3169
struct Cyc_Absyn_Exp*_tmp52D=Cyc_Tcutil_rsubsexp(r,inst,_tmp54A);
if(_tmp52D == _tmp54A)return e;
return({struct Cyc_Absyn_Exp*_tmp8B0=e;Cyc_Tcutil_copye(_tmp8B0,(void*)({struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct*_tmp52E=_cycalloc(sizeof(*_tmp52E));_tmp52E->tag=18U,_tmp52E->f1=_tmp52D;_tmp52E;}));});}case 41U: _LL1B: _tmp54B=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_LL1C: {
# 3173
struct Cyc_Absyn_Exp*_tmp52F=Cyc_Tcutil_rsubsexp(r,inst,_tmp54B);
if(_tmp52F == _tmp54B)return e;
return({struct Cyc_Absyn_Exp*_tmp8B1=e;Cyc_Tcutil_copye(_tmp8B1,(void*)({struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*_tmp530=_cycalloc(sizeof(*_tmp530));_tmp530->tag=41U,_tmp530->f1=_tmp52F;_tmp530;}));});}case 19U: _LL1D: _tmp54D=(void*)((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_tmp54C=((struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*)_tmp50E)->f2;_LL1E: {
# 3177
void*_tmp531=Cyc_Tcutil_rsubstitute(r,inst,_tmp54D);
if(_tmp531 == _tmp54D)return e;
return({struct Cyc_Absyn_Exp*_tmp8B2=e;Cyc_Tcutil_copye(_tmp8B2,(void*)({struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct*_tmp532=_cycalloc(sizeof(*_tmp532));_tmp532->tag=19U,_tmp532->f1=_tmp531,_tmp532->f2=_tmp54C;_tmp532;}));});}case 39U: _LL1F: _tmp54E=(void*)((struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*)_tmp50E)->f1;_LL20: {
# 3181
void*_tmp533=Cyc_Tcutil_rsubstitute(r,inst,_tmp54E);
if(_tmp533 == _tmp54E)return e;{
# 3184
void*_tmp534=Cyc_Tcutil_compress(_tmp533);void*_tmp535=_tmp534;struct Cyc_Absyn_Exp*_tmp537;if(((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp535)->tag == 9U){_LL24: _tmp537=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp535)->f1;_LL25:
 return _tmp537;}else{_LL26: _LL27:
 return({struct Cyc_Absyn_Exp*_tmp8B3=e;Cyc_Tcutil_copye(_tmp8B3,(void*)({struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct*_tmp536=_cycalloc(sizeof(*_tmp536));_tmp536->tag=39U,_tmp536->f1=_tmp533;_tmp536;}));});}_LL23:;};}default: _LL21: _LL22:
# 3188
 return({void*_tmp538=0U;({struct _fat_ptr _tmp8B4=({const char*_tmp539="non-type-level-expression in Tcutil::rsubsexp";_tag_fat(_tmp539,sizeof(char),46U);});((struct Cyc_Absyn_Exp*(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp8B4,_tag_fat(_tmp538,sizeof(void*),0U));});});}_LL0:;}
# 3192
static struct Cyc_Absyn_Exp*Cyc_Tcutil_rsubs_exp_opt(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Exp*e){
# 3195
if(e == 0)return 0;else{
return Cyc_Tcutil_rsubsexp(r,inst,e);}}
# 3199
static struct Cyc_Absyn_Aggrfield*Cyc_Tcutil_subst_aggrfield(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_Absyn_Aggrfield*f){
# 3202
void*_tmp54F=f->type;
struct Cyc_Absyn_Exp*_tmp550=f->requires_clause;
void*_tmp551=Cyc_Tcutil_rsubstitute(r,inst,_tmp54F);
struct Cyc_Absyn_Exp*_tmp552=Cyc_Tcutil_rsubs_exp_opt(r,inst,_tmp550);
if(_tmp54F == _tmp551 && _tmp550 == _tmp552)return f;else{
return({struct Cyc_Absyn_Aggrfield*_tmp553=_cycalloc(sizeof(*_tmp553));_tmp553->name=f->name,_tmp553->tq=f->tq,_tmp553->type=_tmp551,_tmp553->width=f->width,_tmp553->attributes=f->attributes,_tmp553->requires_clause=_tmp552;_tmp553;});}}
# 3212
static struct Cyc_List_List*Cyc_Tcutil_subst_aggrfields(struct _RegionHandle*r,struct Cyc_List_List*inst,struct Cyc_List_List*fs){
# 3215
if(fs == 0)return 0;{
struct Cyc_Absyn_Aggrfield*_tmp554=(struct Cyc_Absyn_Aggrfield*)fs->hd;
struct Cyc_List_List*_tmp555=fs->tl;
struct Cyc_Absyn_Aggrfield*_tmp556=Cyc_Tcutil_subst_aggrfield(r,inst,_tmp554);
struct Cyc_List_List*_tmp557=Cyc_Tcutil_subst_aggrfields(r,inst,_tmp555);
if(_tmp556 == _tmp554 && _tmp557 == _tmp555)return fs;
return({struct Cyc_List_List*_tmp558=_cycalloc(sizeof(*_tmp558));_tmp558->hd=_tmp556,_tmp558->tl=_tmp557;_tmp558;});};}
# 3224
struct Cyc_List_List*Cyc_Tcutil_rsubst_rgnpo(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*rgn_po){
# 3227
struct _tuple0 _tmp559=((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(rgn,rgn,rgn_po);struct _tuple0 _tmp55A=_tmp559;struct Cyc_List_List*_tmp55E;struct Cyc_List_List*_tmp55D;_LL1: _tmp55E=_tmp55A.f1;_tmp55D=_tmp55A.f2;_LL2:;{
struct Cyc_List_List*_tmp55B=Cyc_Tcutil_substs(rgn,inst,_tmp55E);
struct Cyc_List_List*_tmp55C=Cyc_Tcutil_substs(rgn,inst,_tmp55D);
if(_tmp55B == _tmp55E && _tmp55C == _tmp55D)
return rgn_po;else{
# 3233
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp55B,_tmp55C);}};}
# 3236
void*Cyc_Tcutil_rsubstitute(struct _RegionHandle*rgn,struct Cyc_List_List*inst,void*t){
# 3239
void*_tmp55F=Cyc_Tcutil_compress(t);void*_tmp560=_tmp55F;struct Cyc_Absyn_Exp*_tmp5B7;struct Cyc_Absyn_Exp*_tmp5B6;void*_tmp5B5;struct Cyc_List_List*_tmp5B4;void*_tmp5B3;void*_tmp5B2;enum Cyc_Absyn_AggrKind _tmp5B1;struct Cyc_List_List*_tmp5B0;struct Cyc_List_List*_tmp5AF;struct Cyc_List_List*_tmp5AE;void*_tmp5AD;struct Cyc_Absyn_Tqual _tmp5AC;void*_tmp5AB;struct Cyc_List_List*_tmp5AA;int _tmp5A9;struct Cyc_Absyn_VarargInfo*_tmp5A8;struct Cyc_List_List*_tmp5A7;struct Cyc_List_List*_tmp5A6;struct Cyc_Absyn_Exp*_tmp5A5;struct Cyc_Absyn_Exp*_tmp5A4;void*_tmp5A3;struct Cyc_Absyn_Tqual _tmp5A2;void*_tmp5A1;void*_tmp5A0;void*_tmp59F;void*_tmp59E;void*_tmp59D;struct Cyc_Absyn_Tqual _tmp59C;struct Cyc_Absyn_Exp*_tmp59B;void*_tmp59A;unsigned _tmp599;struct _tuple1*_tmp598;struct Cyc_List_List*_tmp597;struct Cyc_Absyn_Typedefdecl*_tmp596;void*_tmp595;struct Cyc_Absyn_Tvar*_tmp594;switch(*((int*)_tmp560)){case 2U: _LL1: _tmp594=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp560)->f1;_LL2: {
# 3242
struct _handler_cons _tmp561;_push_handler(& _tmp561);{int _tmp563=0;if(setjmp(_tmp561.handler))_tmp563=1;if(!_tmp563){{void*_tmp564=((void*(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*l,struct Cyc_Absyn_Tvar*x))Cyc_List_assoc_cmp)(Cyc_Absyn_tvar_cmp,inst,_tmp594);_npop_handler(0U);return _tmp564;};_pop_handler();}else{void*_tmp562=(void*)Cyc_Core_get_exn_thrown();void*_tmp565=_tmp562;void*_tmp566;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp565)->tag == Cyc_Core_Not_found){_LL1C: _LL1D:
 return t;}else{_LL1E: _tmp566=_tmp565;_LL1F:(int)_rethrow(_tmp566);}_LL1B:;}};}case 8U: _LL3: _tmp598=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp560)->f1;_tmp597=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp560)->f2;_tmp596=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp560)->f3;_tmp595=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp560)->f4;_LL4: {
# 3245
struct Cyc_List_List*_tmp567=Cyc_Tcutil_substs(rgn,inst,_tmp597);
return _tmp567 == _tmp597?t:(void*)({struct Cyc_Absyn_TypedefType_Absyn_Type_struct*_tmp568=_cycalloc(sizeof(*_tmp568));_tmp568->tag=8U,_tmp568->f1=_tmp598,_tmp568->f2=_tmp567,_tmp568->f3=_tmp596,_tmp568->f4=_tmp595;_tmp568;});}case 4U: _LL5: _tmp59D=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp560)->f1).elt_type;_tmp59C=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp560)->f1).tq;_tmp59B=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp560)->f1).num_elts;_tmp59A=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp560)->f1).zero_term;_tmp599=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp560)->f1).zt_loc;_LL6: {
# 3248
void*_tmp569=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp59D);
struct Cyc_Absyn_Exp*_tmp56A=_tmp59B == 0?0: Cyc_Tcutil_rsubsexp(rgn,inst,_tmp59B);
void*_tmp56B=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp59A);
return(_tmp569 == _tmp59D && _tmp56A == _tmp59B)&& _tmp56B == _tmp59A?t:(void*)({struct Cyc_Absyn_ArrayType_Absyn_Type_struct*_tmp56C=_cycalloc(sizeof(*_tmp56C));
_tmp56C->tag=4U,(_tmp56C->f1).elt_type=_tmp569,(_tmp56C->f1).tq=_tmp59C,(_tmp56C->f1).num_elts=_tmp56A,(_tmp56C->f1).zero_term=_tmp56B,(_tmp56C->f1).zt_loc=_tmp599;_tmp56C;});}case 3U: _LL7: _tmp5A3=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp560)->f1).elt_type;_tmp5A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp560)->f1).elt_tq;_tmp5A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp560)->f1).ptr_atts).rgn;_tmp5A0=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp560)->f1).ptr_atts).nullable;_tmp59F=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp560)->f1).ptr_atts).bounds;_tmp59E=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp560)->f1).ptr_atts).zero_term;_LL8: {
# 3254
void*_tmp56D=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5A3);
void*_tmp56E=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5A1);
void*_tmp56F=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp59F);
void*_tmp570=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp59E);
if(((_tmp56D == _tmp5A3 && _tmp56E == _tmp5A1)&& _tmp56F == _tmp59F)&& _tmp570 == _tmp59E)
return t;
return Cyc_Absyn_pointer_type(({struct Cyc_Absyn_PtrInfo _tmp755;_tmp755.elt_type=_tmp56D,_tmp755.elt_tq=_tmp5A2,(_tmp755.ptr_atts).rgn=_tmp56E,(_tmp755.ptr_atts).nullable=_tmp5A0,(_tmp755.ptr_atts).bounds=_tmp56F,(_tmp755.ptr_atts).zero_term=_tmp570,(_tmp755.ptr_atts).ptrloc=0;_tmp755;}));}case 5U: _LL9: _tmp5AE=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).tvars;_tmp5AD=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).effect;_tmp5AC=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).ret_tqual;_tmp5AB=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).ret_type;_tmp5AA=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).args;_tmp5A9=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).c_varargs;_tmp5A8=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).cyc_varargs;_tmp5A7=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).rgn_po;_tmp5A6=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).attributes;_tmp5A5=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).requires_clause;_tmp5A4=(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp560)->f1).ensures_clause;_LLA:
# 3264
{struct Cyc_List_List*_tmp571=_tmp5AE;for(0;_tmp571 != 0;_tmp571=_tmp571->tl){
inst=({struct Cyc_List_List*_tmp573=_region_malloc(rgn,sizeof(*_tmp573));({struct _tuple18*_tmp8B6=({struct _tuple18*_tmp572=_region_malloc(rgn,sizeof(*_tmp572));_tmp572->f1=(struct Cyc_Absyn_Tvar*)_tmp571->hd,({void*_tmp8B5=Cyc_Absyn_var_type((struct Cyc_Absyn_Tvar*)_tmp571->hd);_tmp572->f2=_tmp8B5;});_tmp572;});_tmp573->hd=_tmp8B6;}),_tmp573->tl=inst;_tmp573;});}}{
struct _tuple0 _tmp574=({struct _RegionHandle*_tmp8B8=rgn;struct _RegionHandle*_tmp8B7=rgn;((struct _tuple0(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x))Cyc_List_rsplit)(_tmp8B8,_tmp8B7,
((struct Cyc_List_List*(*)(struct _RegionHandle*,struct _tuple27*(*f)(struct _RegionHandle*,struct _tuple9*),struct _RegionHandle*env,struct Cyc_List_List*x))Cyc_List_rmap_c)(rgn,Cyc_Tcutil_substitute_f1,rgn,_tmp5AA));});
# 3266
struct _tuple0 _tmp575=_tmp574;struct Cyc_List_List*_tmp584;struct Cyc_List_List*_tmp583;_LL21: _tmp584=_tmp575.f1;_tmp583=_tmp575.f2;_LL22:;{
# 3268
struct Cyc_List_List*_tmp576=_tmp5AA;
struct Cyc_List_List*_tmp577=Cyc_Tcutil_substs(rgn,inst,_tmp583);
if(_tmp577 != _tmp583)
_tmp576=((struct Cyc_List_List*(*)(struct _tuple9*(*f)(struct _tuple9*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_substitute_f2,_tmp5AA,_tmp577);{
void*eff2;
if(_tmp5AD == 0)
eff2=0;else{
# 3276
void*new_eff=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5AD);
if(new_eff == _tmp5AD)
eff2=_tmp5AD;else{
# 3280
eff2=new_eff;}}{
# 3282
struct Cyc_Absyn_VarargInfo*cyc_varargs2;
if(_tmp5A8 == 0)
cyc_varargs2=0;else{
# 3286
struct Cyc_Absyn_VarargInfo _tmp578=*_tmp5A8;struct Cyc_Absyn_VarargInfo _tmp579=_tmp578;struct _fat_ptr*_tmp57F;struct Cyc_Absyn_Tqual _tmp57E;void*_tmp57D;int _tmp57C;_LL24: _tmp57F=_tmp579.name;_tmp57E=_tmp579.tq;_tmp57D=_tmp579.type;_tmp57C=_tmp579.inject;_LL25:;{
void*_tmp57A=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp57D);
if(_tmp57A == _tmp57D)cyc_varargs2=_tmp5A8;else{
cyc_varargs2=({struct Cyc_Absyn_VarargInfo*_tmp57B=_cycalloc(sizeof(*_tmp57B));_tmp57B->name=_tmp57F,_tmp57B->tq=_tmp57E,_tmp57B->type=_tmp57A,_tmp57B->inject=_tmp57C;_tmp57B;});}};}{
# 3291
struct Cyc_List_List*rgn_po2=Cyc_Tcutil_rsubst_rgnpo(rgn,inst,_tmp5A7);
struct Cyc_Absyn_Exp*req2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5A5);
struct Cyc_Absyn_Exp*ens2=Cyc_Tcutil_rsubs_exp_opt(rgn,inst,_tmp5A4);
struct Cyc_List_List*_tmp580=Cyc_Tcutil_extract_relns(_tmp576,req2);
struct Cyc_List_List*_tmp581=Cyc_Tcutil_extract_relns(_tmp576,ens2);
return(void*)({struct Cyc_Absyn_FnType_Absyn_Type_struct*_tmp582=_cycalloc(sizeof(*_tmp582));_tmp582->tag=5U,(_tmp582->f1).tvars=_tmp5AE,(_tmp582->f1).effect=eff2,(_tmp582->f1).ret_tqual=_tmp5AC,({void*_tmp8B9=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5AB);(_tmp582->f1).ret_type=_tmp8B9;}),(_tmp582->f1).args=_tmp576,(_tmp582->f1).c_varargs=_tmp5A9,(_tmp582->f1).cyc_varargs=cyc_varargs2,(_tmp582->f1).rgn_po=rgn_po2,(_tmp582->f1).attributes=_tmp5A6,(_tmp582->f1).requires_clause=req2,(_tmp582->f1).requires_relns=_tmp580,(_tmp582->f1).ensures_clause=ens2,(_tmp582->f1).ensures_relns=_tmp581;_tmp582;});};};};};};case 6U: _LLB: _tmp5AF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp560)->f1;_LLC: {
# 3300
struct Cyc_List_List*ts2=0;
int change=0;
{struct Cyc_List_List*_tmp585=_tmp5AF;for(0;_tmp585 != 0;_tmp585=_tmp585->tl){
void*_tmp586=(*((struct _tuple11*)_tmp585->hd)).f2;
void*_tmp587=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp586);
if(_tmp586 != _tmp587)
change=1;
# 3308
ts2=({struct Cyc_List_List*_tmp588=_region_malloc(rgn,sizeof(*_tmp588));_tmp588->hd=_tmp587,_tmp588->tl=ts2;_tmp588;});}}
# 3310
if(!change)
return t;
return(void*)({struct Cyc_Absyn_TupleType_Absyn_Type_struct*_tmp589=_cycalloc(sizeof(*_tmp589));_tmp589->tag=6U,({struct Cyc_List_List*_tmp8BB=({struct Cyc_List_List*_tmp8BA=_tmp5AF;((struct Cyc_List_List*(*)(struct _tuple11*(*f)(struct _tuple11*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_map2)(Cyc_Tcutil_map2_tq,_tmp8BA,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ts2));});_tmp589->f1=_tmp8BB;});_tmp589;});}case 7U: _LLD: _tmp5B1=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp560)->f1;_tmp5B0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp560)->f2;_LLE: {
# 3314
struct Cyc_List_List*_tmp58A=Cyc_Tcutil_subst_aggrfields(rgn,inst,_tmp5B0);
if(_tmp5B0 == _tmp58A)return t;
return(void*)({struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*_tmp58B=_cycalloc(sizeof(*_tmp58B));_tmp58B->tag=7U,_tmp58B->f1=_tmp5B1,_tmp58B->f2=_tmp58A;_tmp58B;});}case 1U: _LLF: _tmp5B2=(void*)((struct Cyc_Absyn_Evar_Absyn_Type_struct*)_tmp560)->f2;_LL10:
# 3318
 if(_tmp5B2 != 0)return Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B2);else{
return t;}case 0U: if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f2 == 0){_LL11: _tmp5B3=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1;_LL12:
 return t;}else{_LL13: _tmp5B5=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f1;_tmp5B4=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp560)->f2;_LL14: {
# 3322
struct Cyc_List_List*_tmp58C=Cyc_Tcutil_substs(rgn,inst,_tmp5B4);
if(_tmp5B4 == _tmp58C)return t;else{
return(void*)({struct Cyc_Absyn_AppType_Absyn_Type_struct*_tmp58D=_cycalloc(sizeof(*_tmp58D));_tmp58D->tag=0U,_tmp58D->f1=_tmp5B5,_tmp58D->f2=_tmp58C;_tmp58D;});}}}case 9U: _LL15: _tmp5B6=((struct Cyc_Absyn_ValueofType_Absyn_Type_struct*)_tmp560)->f1;_LL16: {
# 3326
struct Cyc_Absyn_Exp*_tmp58E=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5B6);
return _tmp58E == _tmp5B6?t:(void*)({struct Cyc_Absyn_ValueofType_Absyn_Type_struct*_tmp58F=_cycalloc(sizeof(*_tmp58F));_tmp58F->tag=9U,_tmp58F->f1=_tmp58E;_tmp58F;});}case 11U: _LL17: _tmp5B7=((struct Cyc_Absyn_TypeofType_Absyn_Type_struct*)_tmp560)->f1;_LL18: {
# 3329
struct Cyc_Absyn_Exp*_tmp590=Cyc_Tcutil_rsubsexp(rgn,inst,_tmp5B7);
return _tmp590 == _tmp5B7?t:(void*)({struct Cyc_Absyn_TypeofType_Absyn_Type_struct*_tmp591=_cycalloc(sizeof(*_tmp591));_tmp591->tag=11U,_tmp591->f1=_tmp590;_tmp591;});}default: _LL19: _LL1A:
({void*_tmp592=0U;({struct _fat_ptr _tmp8BC=({const char*_tmp593="found typedecltype in rsubs";_tag_fat(_tmp593,sizeof(char),28U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp8BC,_tag_fat(_tmp592,sizeof(void*),0U));});});}_LL0:;}
# 3335
static struct Cyc_List_List*Cyc_Tcutil_substs(struct _RegionHandle*rgn,struct Cyc_List_List*inst,struct Cyc_List_List*ts){
# 3338
if(ts == 0)
return 0;{
void*_tmp5B8=(void*)ts->hd;
struct Cyc_List_List*_tmp5B9=ts->tl;
void*_tmp5BA=Cyc_Tcutil_rsubstitute(rgn,inst,_tmp5B8);
struct Cyc_List_List*_tmp5BB=Cyc_Tcutil_substs(rgn,inst,_tmp5B9);
if(_tmp5B8 == _tmp5BA && _tmp5B9 == _tmp5BB)
return ts;
return({struct Cyc_List_List*_tmp5BC=_cycalloc(sizeof(*_tmp5BC));_tmp5BC->hd=_tmp5BA,_tmp5BC->tl=_tmp5BB;_tmp5BC;});};}
# 3349
extern void*Cyc_Tcutil_substitute(struct Cyc_List_List*inst,void*t){
if(inst != 0)
return Cyc_Tcutil_rsubstitute(Cyc_Core_heap_region,inst,t);else{
return t;}}
# 3356
struct _tuple18*Cyc_Tcutil_make_inst_var(struct Cyc_List_List*s,struct Cyc_Absyn_Tvar*tv){
struct Cyc_Core_Opt*_tmp5BD=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple18*_tmp5BF=_cycalloc(sizeof(*_tmp5BF));_tmp5BF->f1=tv,({void*_tmp8BE=({struct Cyc_Core_Opt*_tmp8BD=_tmp5BD;Cyc_Absyn_new_evar(_tmp8BD,({struct Cyc_Core_Opt*_tmp5BE=_cycalloc(sizeof(*_tmp5BE));_tmp5BE->v=s;_tmp5BE;}));});_tmp5BF->f2=_tmp8BE;});_tmp5BF;});}struct _tuple28{struct Cyc_List_List*f1;struct _RegionHandle*f2;};
# 3361
struct _tuple18*Cyc_Tcutil_r_make_inst_var(struct _tuple28*env,struct Cyc_Absyn_Tvar*tv){
# 3363
struct _tuple28*_tmp5C0=env;struct Cyc_List_List*_tmp5C5;struct _RegionHandle*_tmp5C4;_LL1: _tmp5C5=_tmp5C0->f1;_tmp5C4=_tmp5C0->f2;_LL2:;{
struct Cyc_Core_Opt*_tmp5C1=Cyc_Tcutil_kind_to_opt(Cyc_Tcutil_tvar_kind(tv,& Cyc_Tcutil_bk));
return({struct _tuple18*_tmp5C3=_region_malloc(_tmp5C4,sizeof(*_tmp5C3));_tmp5C3->f1=tv,({void*_tmp8C0=({struct Cyc_Core_Opt*_tmp8BF=_tmp5C1;Cyc_Absyn_new_evar(_tmp8BF,({struct Cyc_Core_Opt*_tmp5C2=_cycalloc(sizeof(*_tmp5C2));_tmp5C2->v=_tmp5C5;_tmp5C2;}));});_tmp5C3->f2=_tmp8C0;});_tmp5C3;});};}
# 3368
void Cyc_Tcutil_check_bitfield(unsigned loc,struct Cyc_Tcenv_Tenv*te,void*field_type,struct Cyc_Absyn_Exp*width,struct _fat_ptr*fn){
# 3370
if(width != 0){
unsigned w=0U;
if(!Cyc_Tcutil_is_const_exp(width))
({struct Cyc_String_pa_PrintArg_struct _tmp5C8=({struct Cyc_String_pa_PrintArg_struct _tmp756;_tmp756.tag=0U,_tmp756.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp756;});void*_tmp5C6[1U];_tmp5C6[0]=& _tmp5C8;({unsigned _tmp8C2=loc;struct _fat_ptr _tmp8C1=({const char*_tmp5C7="bitfield %s does not have constant width";_tag_fat(_tmp5C7,sizeof(char),41U);});Cyc_Tcutil_terr(_tmp8C2,_tmp8C1,_tag_fat(_tmp5C6,sizeof(void*),1U));});});else{
# 3375
struct _tuple12 _tmp5C9=Cyc_Evexp_eval_const_uint_exp(width);struct _tuple12 _tmp5CA=_tmp5C9;unsigned _tmp5D0;int _tmp5CF;_LL1: _tmp5D0=_tmp5CA.f1;_tmp5CF=_tmp5CA.f2;_LL2:;
if(!_tmp5CF)
({void*_tmp5CB=0U;({unsigned _tmp8C4=loc;struct _fat_ptr _tmp8C3=({const char*_tmp5CC="cannot evaluate bitfield width at compile time";_tag_fat(_tmp5CC,sizeof(char),47U);});Cyc_Tcutil_warn(_tmp8C4,_tmp8C3,_tag_fat(_tmp5CB,sizeof(void*),0U));});});
if((int)_tmp5D0 < 0)
({void*_tmp5CD=0U;({unsigned _tmp8C6=loc;struct _fat_ptr _tmp8C5=({const char*_tmp5CE="bitfield has negative width";_tag_fat(_tmp5CE,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp8C6,_tmp8C5,_tag_fat(_tmp5CD,sizeof(void*),0U));});});
w=_tmp5D0;}{
# 3382
void*_tmp5D1=Cyc_Tcutil_compress(field_type);void*_tmp5D2=_tmp5D1;enum Cyc_Absyn_Size_of _tmp5E0;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5D2)->tag == 0U){if(((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5D2)->f1)->tag == 1U){_LL4: _tmp5E0=((struct Cyc_Absyn_IntCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp5D2)->f1)->f2;_LL5:
# 3385
{enum Cyc_Absyn_Size_of _tmp5D3=_tmp5E0;switch(_tmp5D3){case Cyc_Absyn_Char_sz: _LL9: _LLA:
 if(w > (unsigned)8)({void*_tmp5D4=0U;({unsigned _tmp8C8=loc;struct _fat_ptr _tmp8C7=({const char*_tmp5D5="bitfield larger than type";_tag_fat(_tmp5D5,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp8C8,_tmp8C7,_tag_fat(_tmp5D4,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Short_sz: _LLB: _LLC:
 if(w > (unsigned)16)({void*_tmp5D6=0U;({unsigned _tmp8CA=loc;struct _fat_ptr _tmp8C9=({const char*_tmp5D7="bitfield larger than type";_tag_fat(_tmp5D7,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp8CA,_tmp8C9,_tag_fat(_tmp5D6,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_Long_sz: _LLD: _LLE:
 goto _LL10;case Cyc_Absyn_Int_sz: _LLF: _LL10:
 if(w > (unsigned)32)({void*_tmp5D8=0U;({unsigned _tmp8CC=loc;struct _fat_ptr _tmp8CB=({const char*_tmp5D9="bitfield larger than type";_tag_fat(_tmp5D9,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp8CC,_tmp8CB,_tag_fat(_tmp5D8,sizeof(void*),0U));});});goto _LL8;case Cyc_Absyn_LongLong_sz: _LL11: _LL12:
 goto _LL14;default: _LL13: _LL14:
 if(w > (unsigned)64)({void*_tmp5DA=0U;({unsigned _tmp8CE=loc;struct _fat_ptr _tmp8CD=({const char*_tmp5DB="bitfield larger than type";_tag_fat(_tmp5DB,sizeof(char),26U);});Cyc_Tcutil_warn(_tmp8CE,_tmp8CD,_tag_fat(_tmp5DA,sizeof(void*),0U));});});goto _LL8;}_LL8:;}
# 3393
goto _LL3;}else{goto _LL6;}}else{_LL6: _LL7:
# 3395
({struct Cyc_String_pa_PrintArg_struct _tmp5DE=({struct Cyc_String_pa_PrintArg_struct _tmp758;_tmp758.tag=0U,_tmp758.f1=(struct _fat_ptr)((struct _fat_ptr)*fn);_tmp758;});struct Cyc_String_pa_PrintArg_struct _tmp5DF=({struct Cyc_String_pa_PrintArg_struct _tmp757;_tmp757.tag=0U,({
struct _fat_ptr _tmp8CF=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(field_type));_tmp757.f1=_tmp8CF;});_tmp757;});void*_tmp5DC[2U];_tmp5DC[0]=& _tmp5DE,_tmp5DC[1]=& _tmp5DF;({unsigned _tmp8D1=loc;struct _fat_ptr _tmp8D0=({const char*_tmp5DD="bitfield %s must have integral type but has type %s";_tag_fat(_tmp5DD,sizeof(char),52U);});Cyc_Tcutil_terr(_tmp8D1,_tmp8D0,_tag_fat(_tmp5DC,sizeof(void*),2U));});});
goto _LL3;}_LL3:;};}}
# 3404
int Cyc_Tcutil_extract_const_from_typedef(unsigned loc,int declared_const,void*t){
void*_tmp5E1=t;struct Cyc_Absyn_Typedefdecl*_tmp5E5;void*_tmp5E4;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E1)->tag == 8U){_LL1: _tmp5E5=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E1)->f3;_tmp5E4=(void*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp5E1)->f4;_LL2:
# 3407
 if((((struct Cyc_Absyn_Typedefdecl*)_check_null(_tmp5E5))->tq).real_const ||(_tmp5E5->tq).print_const){
if(declared_const)({void*_tmp5E2=0U;({unsigned _tmp8D3=loc;struct _fat_ptr _tmp8D2=({const char*_tmp5E3="extra const";_tag_fat(_tmp5E3,sizeof(char),12U);});Cyc_Tcutil_warn(_tmp8D3,_tmp8D2,_tag_fat(_tmp5E2,sizeof(void*),0U));});});
return 1;}
# 3412
if((unsigned)_tmp5E4)
return Cyc_Tcutil_extract_const_from_typedef(loc,declared_const,_tmp5E4);else{
return declared_const;}}else{_LL3: _LL4:
 return declared_const;}_LL0:;}
# 3419
void Cyc_Tcutil_add_tvar_identity(struct Cyc_Absyn_Tvar*tv){
if(tv->identity == - 1)
({int _tmp8D4=Cyc_Tcutil_new_tvar_id();tv->identity=_tmp8D4;});}
# 3423
void Cyc_Tcutil_add_tvar_identities(struct Cyc_List_List*tvs){
((void(*)(void(*f)(struct Cyc_Absyn_Tvar*),struct Cyc_List_List*x))Cyc_List_iter)(Cyc_Tcutil_add_tvar_identity,tvs);}
# 3429
static void Cyc_Tcutil_check_unique_unsorted(int(*cmp)(void*,void*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(void*),struct _fat_ptr msg){
# 3432
for(0;vs != 0;vs=vs->tl){
struct Cyc_List_List*vs2=vs->tl;for(0;vs2 != 0;vs2=vs2->tl){
if(cmp(vs->hd,vs2->hd)== 0)
({struct Cyc_String_pa_PrintArg_struct _tmp5E8=({struct Cyc_String_pa_PrintArg_struct _tmp75A;_tmp75A.tag=0U,_tmp75A.f1=(struct _fat_ptr)((struct _fat_ptr)msg);_tmp75A;});struct Cyc_String_pa_PrintArg_struct _tmp5E9=({struct Cyc_String_pa_PrintArg_struct _tmp759;_tmp759.tag=0U,({struct _fat_ptr _tmp8D5=(struct _fat_ptr)((struct _fat_ptr)a2string(vs->hd));_tmp759.f1=_tmp8D5;});_tmp759;});void*_tmp5E6[2U];_tmp5E6[0]=& _tmp5E8,_tmp5E6[1]=& _tmp5E9;({unsigned _tmp8D7=loc;struct _fat_ptr _tmp8D6=({const char*_tmp5E7="%s: %s";_tag_fat(_tmp5E7,sizeof(char),7U);});Cyc_Tcutil_terr(_tmp8D7,_tmp8D6,_tag_fat(_tmp5E6,sizeof(void*),2U));});});}}}
# 3438
static struct _fat_ptr Cyc_Tcutil_strptr2string(struct _fat_ptr*s){
return*s;}
# 3442
void Cyc_Tcutil_check_unique_vars(struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr msg){
((void(*)(int(*cmp)(struct _fat_ptr*,struct _fat_ptr*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct _fat_ptr*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_strptrcmp,vs,loc,Cyc_Tcutil_strptr2string,msg);}
# 3446
void Cyc_Tcutil_check_unique_tvars(unsigned loc,struct Cyc_List_List*tvs){
({struct Cyc_List_List*_tmp8D9=tvs;unsigned _tmp8D8=loc;((void(*)(int(*cmp)(struct Cyc_Absyn_Tvar*,struct Cyc_Absyn_Tvar*),struct Cyc_List_List*vs,unsigned loc,struct _fat_ptr(*a2string)(struct Cyc_Absyn_Tvar*),struct _fat_ptr msg))Cyc_Tcutil_check_unique_unsorted)(Cyc_Absyn_tvar_cmp,_tmp8D9,_tmp8D8,Cyc_Absynpp_tvar2string,({const char*_tmp5EA="duplicate type variable";_tag_fat(_tmp5EA,sizeof(char),24U);}));});}struct _tuple29{struct Cyc_Absyn_Aggrfield*f1;int f2;};struct _tuple30{struct Cyc_List_List*f1;void*f2;};struct _tuple31{struct Cyc_Absyn_Aggrfield*f1;void*f2;};
# 3460 "tcutil.cyc"
struct Cyc_List_List*Cyc_Tcutil_resolve_aggregate_designators(struct _RegionHandle*rgn,unsigned loc,struct Cyc_List_List*des,enum Cyc_Absyn_AggrKind aggr_kind,struct Cyc_List_List*sdfields){
# 3465
struct _RegionHandle _tmp5EB=_new_region("temp");struct _RegionHandle*temp=& _tmp5EB;_push_region(temp);
# 3469
{struct Cyc_List_List*fields=0;
{struct Cyc_List_List*sd_fields=sdfields;for(0;sd_fields != 0;sd_fields=sd_fields->tl){
if(({struct _fat_ptr _tmp8DA=(struct _fat_ptr)*((struct Cyc_Absyn_Aggrfield*)sd_fields->hd)->name;Cyc_strcmp(_tmp8DA,({const char*_tmp5EC="";_tag_fat(_tmp5EC,sizeof(char),1U);}));})!= 0)
fields=({struct Cyc_List_List*_tmp5EE=_region_malloc(temp,sizeof(*_tmp5EE));({struct _tuple29*_tmp8DB=({struct _tuple29*_tmp5ED=_region_malloc(temp,sizeof(*_tmp5ED));_tmp5ED->f1=(struct Cyc_Absyn_Aggrfield*)sd_fields->hd,_tmp5ED->f2=0;_tmp5ED;});_tmp5EE->hd=_tmp8DB;}),_tmp5EE->tl=fields;_tmp5EE;});}}
# 3474
fields=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(fields);{
# 3476
struct _fat_ptr aggr_str=(int)aggr_kind == (int)0U?({const char*_tmp613="struct";_tag_fat(_tmp613,sizeof(char),7U);}):({const char*_tmp614="union";_tag_fat(_tmp614,sizeof(char),6U);});
# 3479
struct Cyc_List_List*ans=0;
for(0;des != 0;des=des->tl){
struct _tuple30*_tmp5EF=(struct _tuple30*)des->hd;struct _tuple30*_tmp5F0=_tmp5EF;struct Cyc_List_List*_tmp60B;void*_tmp60A;_LL1: _tmp60B=_tmp5F0->f1;_tmp60A=_tmp5F0->f2;_LL2:;
if(_tmp60B == 0){
# 3484
struct Cyc_List_List*_tmp5F1=fields;
for(0;_tmp5F1 != 0;_tmp5F1=_tmp5F1->tl){
if(!(*((struct _tuple29*)_tmp5F1->hd)).f2){
(*((struct _tuple29*)_tmp5F1->hd)).f2=1;
({struct Cyc_List_List*_tmp8DD=({struct Cyc_List_List*_tmp5F3=_cycalloc(sizeof(*_tmp5F3));({void*_tmp8DC=(void*)({struct Cyc_Absyn_FieldName_Absyn_Designator_struct*_tmp5F2=_cycalloc(sizeof(*_tmp5F2));_tmp5F2->tag=1U,_tmp5F2->f1=((*((struct _tuple29*)_tmp5F1->hd)).f1)->name;_tmp5F2;});_tmp5F3->hd=_tmp8DC;}),_tmp5F3->tl=0;_tmp5F3;});(*((struct _tuple30*)des->hd)).f1=_tmp8DD;});
ans=({struct Cyc_List_List*_tmp5F5=_region_malloc(rgn,sizeof(*_tmp5F5));({struct _tuple31*_tmp8DE=({struct _tuple31*_tmp5F4=_region_malloc(rgn,sizeof(*_tmp5F4));_tmp5F4->f1=(*((struct _tuple29*)_tmp5F1->hd)).f1,_tmp5F4->f2=_tmp60A;_tmp5F4;});_tmp5F5->hd=_tmp8DE;}),_tmp5F5->tl=ans;_tmp5F5;});
break;}}
# 3492
if(_tmp5F1 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp5F8=({struct Cyc_String_pa_PrintArg_struct _tmp75B;_tmp75B.tag=0U,_tmp75B.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp75B;});void*_tmp5F6[1U];_tmp5F6[0]=& _tmp5F8;({unsigned _tmp8E0=loc;struct _fat_ptr _tmp8DF=({const char*_tmp5F7="too many arguments to %s";_tag_fat(_tmp5F7,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp8E0,_tmp8DF,_tag_fat(_tmp5F6,sizeof(void*),1U));});});}else{
if(_tmp60B->tl != 0)
# 3496
({void*_tmp5F9=0U;({unsigned _tmp8E2=loc;struct _fat_ptr _tmp8E1=({const char*_tmp5FA="multiple designators are not yet supported";_tag_fat(_tmp5FA,sizeof(char),43U);});Cyc_Tcutil_terr(_tmp8E2,_tmp8E1,_tag_fat(_tmp5F9,sizeof(void*),0U));});});else{
# 3499
void*_tmp5FB=(void*)_tmp60B->hd;void*_tmp5FC=_tmp5FB;struct _fat_ptr*_tmp609;if(((struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct*)_tmp5FC)->tag == 0U){_LL4: _LL5:
# 3501
({struct Cyc_String_pa_PrintArg_struct _tmp5FF=({struct Cyc_String_pa_PrintArg_struct _tmp75C;_tmp75C.tag=0U,_tmp75C.f1=(struct _fat_ptr)((struct _fat_ptr)aggr_str);_tmp75C;});void*_tmp5FD[1U];_tmp5FD[0]=& _tmp5FF;({unsigned _tmp8E4=loc;struct _fat_ptr _tmp8E3=({const char*_tmp5FE="array designator used in argument to %s";_tag_fat(_tmp5FE,sizeof(char),40U);});Cyc_Tcutil_terr(_tmp8E4,_tmp8E3,_tag_fat(_tmp5FD,sizeof(void*),1U));});});
goto _LL3;}else{_LL6: _tmp609=((struct Cyc_Absyn_FieldName_Absyn_Designator_struct*)_tmp5FC)->f1;_LL7: {
# 3504
struct Cyc_List_List*_tmp600=fields;
for(0;_tmp600 != 0;_tmp600=_tmp600->tl){
if(Cyc_strptrcmp(_tmp609,((*((struct _tuple29*)_tmp600->hd)).f1)->name)== 0){
if((*((struct _tuple29*)_tmp600->hd)).f2)
({struct Cyc_String_pa_PrintArg_struct _tmp603=({struct Cyc_String_pa_PrintArg_struct _tmp75D;_tmp75D.tag=0U,_tmp75D.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp609);_tmp75D;});void*_tmp601[1U];_tmp601[0]=& _tmp603;({unsigned _tmp8E6=loc;struct _fat_ptr _tmp8E5=({const char*_tmp602="member %s has already been used as an argument";_tag_fat(_tmp602,sizeof(char),47U);});Cyc_Tcutil_terr(_tmp8E6,_tmp8E5,_tag_fat(_tmp601,sizeof(void*),1U));});});
(*((struct _tuple29*)_tmp600->hd)).f2=1;
ans=({struct Cyc_List_List*_tmp605=_region_malloc(rgn,sizeof(*_tmp605));({struct _tuple31*_tmp8E7=({struct _tuple31*_tmp604=_region_malloc(rgn,sizeof(*_tmp604));_tmp604->f1=(*((struct _tuple29*)_tmp600->hd)).f1,_tmp604->f2=_tmp60A;_tmp604;});_tmp605->hd=_tmp8E7;}),_tmp605->tl=ans;_tmp605;});
break;}}
# 3513
if(_tmp600 == 0)
({struct Cyc_String_pa_PrintArg_struct _tmp608=({struct Cyc_String_pa_PrintArg_struct _tmp75E;_tmp75E.tag=0U,_tmp75E.f1=(struct _fat_ptr)((struct _fat_ptr)*_tmp609);_tmp75E;});void*_tmp606[1U];_tmp606[0]=& _tmp608;({unsigned _tmp8E9=loc;struct _fat_ptr _tmp8E8=({const char*_tmp607="bad field designator %s";_tag_fat(_tmp607,sizeof(char),24U);});Cyc_Tcutil_terr(_tmp8E9,_tmp8E8,_tag_fat(_tmp606,sizeof(void*),1U));});});
goto _LL3;}}_LL3:;}}}
# 3518
if((int)aggr_kind == (int)0U)
# 3520
for(0;fields != 0;fields=fields->tl){
if(!(*((struct _tuple29*)fields->hd)).f2){
({void*_tmp60C=0U;({unsigned _tmp8EB=loc;struct _fat_ptr _tmp8EA=({const char*_tmp60D="too few arguments to struct";_tag_fat(_tmp60D,sizeof(char),28U);});Cyc_Tcutil_terr(_tmp8EB,_tmp8EA,_tag_fat(_tmp60C,sizeof(void*),0U));});});
break;}}else{
# 3527
int found=0;
for(0;fields != 0;fields=fields->tl){
if((*((struct _tuple29*)fields->hd)).f2){
if(found)({void*_tmp60E=0U;({unsigned _tmp8ED=loc;struct _fat_ptr _tmp8EC=({const char*_tmp60F="only one member of a union is allowed";_tag_fat(_tmp60F,sizeof(char),38U);});Cyc_Tcutil_terr(_tmp8ED,_tmp8EC,_tag_fat(_tmp60E,sizeof(void*),0U));});});
found=1;}}
# 3533
if(!found)({void*_tmp610=0U;({unsigned _tmp8EF=loc;struct _fat_ptr _tmp8EE=({const char*_tmp611="missing member for union";_tag_fat(_tmp611,sizeof(char),25U);});Cyc_Tcutil_terr(_tmp8EF,_tmp8EE,_tag_fat(_tmp610,sizeof(void*),0U));});});}{
# 3536
struct Cyc_List_List*_tmp612=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(ans);_npop_handler(0U);return _tmp612;};};}
# 3469
;_pop_region(temp);}
# 3542
int Cyc_Tcutil_is_zero_ptr_deref(struct Cyc_Absyn_Exp*e1,void**ptr_type,int*is_fat,void**elt_type){
# 3544
void*_tmp615=e1->r;void*_tmp616=_tmp615;struct Cyc_Absyn_Exp*_tmp628;struct Cyc_Absyn_Exp*_tmp627;struct Cyc_Absyn_Exp*_tmp626;struct Cyc_Absyn_Exp*_tmp625;struct Cyc_Absyn_Exp*_tmp624;struct Cyc_Absyn_Exp*_tmp623;switch(*((int*)_tmp616)){case 14U: _LL1: _LL2:
({struct Cyc_String_pa_PrintArg_struct _tmp619=({struct Cyc_String_pa_PrintArg_struct _tmp75F;_tmp75F.tag=0U,({struct _fat_ptr _tmp8F0=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp75F.f1=_tmp8F0;});_tmp75F;});void*_tmp617[1U];_tmp617[0]=& _tmp619;({struct _fat_ptr _tmp8F1=({const char*_tmp618="we have a cast in a lhs:  %s";_tag_fat(_tmp618,sizeof(char),29U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp8F1,_tag_fat(_tmp617,sizeof(void*),1U));});});case 20U: _LL3: _tmp623=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp616)->f1;_LL4:
 _tmp624=_tmp623;goto _LL6;case 23U: _LL5: _tmp624=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp616)->f1;_LL6:
# 3548
 return Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp624->topt),ptr_type,is_fat,elt_type);case 22U: _LL7: _tmp625=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp616)->f1;_LL8:
 _tmp626=_tmp625;goto _LLA;case 21U: _LL9: _tmp626=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp616)->f1;_LLA:
# 3552
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp626->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp61C=({struct Cyc_String_pa_PrintArg_struct _tmp760;_tmp760.tag=0U,({
struct _fat_ptr _tmp8F2=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp760.f1=_tmp8F2;});_tmp760;});void*_tmp61A[1U];_tmp61A[0]=& _tmp61C;({struct _fat_ptr _tmp8F3=({const char*_tmp61B="found zero pointer aggregate member assignment: %s";_tag_fat(_tmp61B,sizeof(char),51U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp8F3,_tag_fat(_tmp61A,sizeof(void*),1U));});});
return 0;case 13U: _LLB: _tmp627=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp616)->f1;_LLC:
 _tmp628=_tmp627;goto _LLE;case 12U: _LLD: _tmp628=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp616)->f1;_LLE:
# 3558
 if(Cyc_Tcutil_is_zero_ptr_type((void*)_check_null(_tmp628->topt),ptr_type,is_fat,elt_type))
({struct Cyc_String_pa_PrintArg_struct _tmp61F=({struct Cyc_String_pa_PrintArg_struct _tmp761;_tmp761.tag=0U,({
struct _fat_ptr _tmp8F4=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp761.f1=_tmp8F4;});_tmp761;});void*_tmp61D[1U];_tmp61D[0]=& _tmp61F;({struct _fat_ptr _tmp8F5=({const char*_tmp61E="found zero pointer instantiate/noinstantiate: %s";_tag_fat(_tmp61E,sizeof(char),49U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp8F5,_tag_fat(_tmp61D,sizeof(void*),1U));});});
return 0;case 1U: _LLF: _LL10:
 return 0;default: _LL11: _LL12:
({struct Cyc_String_pa_PrintArg_struct _tmp622=({struct Cyc_String_pa_PrintArg_struct _tmp762;_tmp762.tag=0U,({struct _fat_ptr _tmp8F6=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_exp2string(e1));_tmp762.f1=_tmp8F6;});_tmp762;});void*_tmp620[1U];_tmp620[0]=& _tmp622;({struct _fat_ptr _tmp8F7=({const char*_tmp621="found bad lhs in is_zero_ptr_deref: %s";_tag_fat(_tmp621,sizeof(char),39U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp8F7,_tag_fat(_tmp620,sizeof(void*),1U));});});}_LL0:;}
# 3573
int Cyc_Tcutil_is_noalias_region(void*r,int must_be_unique){
# 3576
void*_tmp629=Cyc_Tcutil_compress(r);void*_tmp62A=_tmp629;struct Cyc_Absyn_Tvar*_tmp636;enum Cyc_Absyn_KindQual _tmp635;enum Cyc_Absyn_AliasQual _tmp634;switch(*((int*)_tmp62A)){case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp62A)->f1)){case 7U: _LL1: _LL2:
 return !must_be_unique;case 6U: _LL3: _LL4:
 return 1;default: goto _LL9;}case 8U: if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp62A)->f3 != 0){if(((struct Cyc_Absyn_Typedefdecl*)((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp62A)->f3)->kind != 0){if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp62A)->f4 == 0){_LL5: _tmp635=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp62A)->f3)->kind)->v)->kind;_tmp634=((struct Cyc_Absyn_Kind*)((((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp62A)->f3)->kind)->v)->aliasqual;_LL6:
# 3580
 return(int)_tmp635 == (int)3U &&((int)_tmp634 == (int)1U ||(int)_tmp634 == (int)2U && !must_be_unique);}else{goto _LL9;}}else{goto _LL9;}}else{goto _LL9;}case 2U: _LL7: _tmp636=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp62A)->f1;_LL8: {
# 3584
struct Cyc_Absyn_Kind*_tmp62B=Cyc_Tcutil_tvar_kind(_tmp636,& Cyc_Tcutil_rk);struct Cyc_Absyn_Kind*_tmp62C=_tmp62B;enum Cyc_Absyn_KindQual _tmp633;enum Cyc_Absyn_AliasQual _tmp632;_LLC: _tmp633=_tmp62C->kind;_tmp632=_tmp62C->aliasqual;_LLD:;
if((int)_tmp633 == (int)3U &&((int)_tmp632 == (int)1U ||(int)_tmp632 == (int)2U && !must_be_unique)){
void*_tmp62D=Cyc_Absyn_compress_kb(_tmp636->kind);void*_tmp62E=_tmp62D;struct Cyc_Core_Opt**_tmp631;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp62E)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp62E)->f2)->kind == Cyc_Absyn_RgnKind){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp62E)->f2)->aliasqual == Cyc_Absyn_Top){_LLF: _tmp631=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp62E)->f1;_LL10:
# 3588
({struct Cyc_Core_Opt*_tmp8F9=({struct Cyc_Core_Opt*_tmp630=_cycalloc(sizeof(*_tmp630));({void*_tmp8F8=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp62F=_cycalloc(sizeof(*_tmp62F));_tmp62F->tag=2U,_tmp62F->f1=0,_tmp62F->f2=& Cyc_Tcutil_rk;_tmp62F;});_tmp630->v=_tmp8F8;});_tmp630;});*_tmp631=_tmp8F9;});
return 0;}else{goto _LL11;}}else{goto _LL11;}}else{_LL11: _LL12:
 return 1;}_LLE:;}
# 3593
return 0;}default: _LL9: _LLA:
 return 0;}_LL0:;}
# 3600
int Cyc_Tcutil_is_noalias_pointer(void*t,int must_be_unique){
void*_tmp637=Cyc_Tcutil_compress(t);void*_tmp638=_tmp637;struct Cyc_Absyn_Tvar*_tmp646;void*_tmp645;switch(*((int*)_tmp638)){case 3U: _LL1: _tmp645=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp638)->f1).ptr_atts).rgn;_LL2:
# 3603
 return Cyc_Tcutil_is_noalias_region(_tmp645,must_be_unique);case 2U: _LL3: _tmp646=((struct Cyc_Absyn_VarType_Absyn_Type_struct*)_tmp638)->f1;_LL4: {
# 3605
struct Cyc_Absyn_Kind*_tmp639=Cyc_Tcutil_tvar_kind(_tmp646,& Cyc_Tcutil_bk);struct Cyc_Absyn_Kind*_tmp63A=_tmp639;enum Cyc_Absyn_KindQual _tmp644;enum Cyc_Absyn_AliasQual _tmp643;_LL8: _tmp644=_tmp63A->kind;_tmp643=_tmp63A->aliasqual;_LL9:;{
enum Cyc_Absyn_KindQual _tmp63B=_tmp644;switch(_tmp63B){case Cyc_Absyn_BoxKind: _LLB: _LLC:
 goto _LLE;case Cyc_Absyn_AnyKind: _LLD: _LLE: goto _LL10;case Cyc_Absyn_MemKind: _LLF: _LL10:
 if((int)_tmp643 == (int)1U ||(int)_tmp643 == (int)2U && !must_be_unique){
void*_tmp63C=Cyc_Absyn_compress_kb(_tmp646->kind);void*_tmp63D=_tmp63C;struct Cyc_Core_Opt**_tmp642;enum Cyc_Absyn_KindQual _tmp641;if(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp63D)->tag == 2U){if(((struct Cyc_Absyn_Kind*)((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp63D)->f2)->aliasqual == Cyc_Absyn_Top){_LL14: _tmp642=(struct Cyc_Core_Opt**)&((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp63D)->f1;_tmp641=(((struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*)_tmp63D)->f2)->kind;_LL15:
# 3611
({struct Cyc_Core_Opt*_tmp8FC=({struct Cyc_Core_Opt*_tmp640=_cycalloc(sizeof(*_tmp640));({void*_tmp8FB=(void*)({struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct*_tmp63F=_cycalloc(sizeof(*_tmp63F));_tmp63F->tag=2U,_tmp63F->f1=0,({struct Cyc_Absyn_Kind*_tmp8FA=({struct Cyc_Absyn_Kind*_tmp63E=_cycalloc(sizeof(*_tmp63E));_tmp63E->kind=_tmp641,_tmp63E->aliasqual=Cyc_Absyn_Aliasable;_tmp63E;});_tmp63F->f2=_tmp8FA;});_tmp63F;});_tmp640->v=_tmp8FB;});_tmp640;});*_tmp642=_tmp8FC;});
return 0;}else{goto _LL16;}}else{_LL16: _LL17:
# 3615
 return 1;}_LL13:;}
# 3618
return 0;default: _LL11: _LL12:
 return 0;}_LLA:;};}default: _LL5: _LL6:
# 3621
 return 0;}_LL0:;}
# 3624
int Cyc_Tcutil_is_noalias_pointer_or_aggr(void*t){
void*_tmp647=Cyc_Tcutil_compress(t);
if(Cyc_Tcutil_is_noalias_pointer(_tmp647,0))return 1;{
void*_tmp648=_tmp647;struct Cyc_List_List*_tmp65C;union Cyc_Absyn_DatatypeFieldInfo _tmp65B;struct Cyc_List_List*_tmp65A;union Cyc_Absyn_DatatypeInfo _tmp659;struct Cyc_List_List*_tmp658;struct Cyc_Absyn_Aggrdecl**_tmp657;struct Cyc_List_List*_tmp656;struct Cyc_List_List*_tmp655;switch(*((int*)_tmp648)){case 6U: _LL1: _tmp655=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp648)->f1;_LL2:
# 3629
 while(_tmp655 != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr((*((struct _tuple11*)_tmp655->hd)).f2))return 1;
_tmp655=_tmp655->tl;}
# 3633
return 0;case 0U: switch(*((int*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp648)->f1)){case 20U: if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp648)->f1)->f1).KnownAggr).tag == 2){_LL3: _tmp657=((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp648)->f1)->f1).KnownAggr).val;_tmp656=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp648)->f2;_LL4:
# 3635
 if((*_tmp657)->impl == 0)return 0;else{
# 3637
struct Cyc_List_List*_tmp649=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)((*_tmp657)->tvs,_tmp656);
struct Cyc_List_List*_tmp64A=((struct Cyc_Absyn_AggrdeclImpl*)_check_null((*_tmp657)->impl))->fields;
void*t;
while(_tmp64A != 0){
t=_tmp649 == 0?((struct Cyc_Absyn_Aggrfield*)_tmp64A->hd)->type: Cyc_Tcutil_substitute(_tmp649,((struct Cyc_Absyn_Aggrfield*)_tmp64A->hd)->type);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(t))return 1;
_tmp64A=_tmp64A->tl;}
# 3645
return 0;}}else{_LL7: _LL8:
# 3655
 return 0;}case 18U: _LL9: _tmp659=((struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp648)->f1)->f1;_tmp658=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp648)->f2;_LLA: {
# 3657
union Cyc_Absyn_DatatypeInfo _tmp64B=_tmp659;struct Cyc_List_List*_tmp64F;struct Cyc_Core_Opt*_tmp64E;struct _tuple1*_tmp64D;int _tmp64C;if((_tmp64B.UnknownDatatype).tag == 1){_LL10: _tmp64D=((_tmp64B.UnknownDatatype).val).name;_tmp64C=((_tmp64B.UnknownDatatype).val).is_extensible;_LL11:
# 3660
 return 0;}else{_LL12: _tmp64F=(*(_tmp64B.KnownDatatype).val)->tvs;_tmp64E=(*(_tmp64B.KnownDatatype).val)->fields;_LL13:
# 3663
 return 0;}_LLF:;}case 19U: _LLB: _tmp65B=((struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp648)->f1)->f1;_tmp65A=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp648)->f2;_LLC: {
# 3666
union Cyc_Absyn_DatatypeFieldInfo _tmp650=_tmp65B;struct Cyc_Absyn_Datatypedecl*_tmp654;struct Cyc_Absyn_Datatypefield*_tmp653;if((_tmp650.UnknownDatatypefield).tag == 1){_LL15: _LL16:
# 3669
 return 0;}else{_LL17: _tmp654=((_tmp650.KnownDatatypefield).val).f1;_tmp653=((_tmp650.KnownDatatypefield).val).f2;_LL18: {
# 3671
struct Cyc_List_List*_tmp651=((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_zip)(_tmp654->tvs,_tmp65A);
struct Cyc_List_List*_tmp652=_tmp653->typs;
while(_tmp652 != 0){
_tmp647=_tmp651 == 0?(*((struct _tuple11*)_tmp652->hd)).f2: Cyc_Tcutil_substitute(_tmp651,(*((struct _tuple11*)_tmp652->hd)).f2);
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(_tmp647))return 1;
_tmp652=_tmp652->tl;}
# 3678
return 0;}}_LL14:;}default: goto _LLD;}case 7U: _LL5: _tmp65C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp648)->f2;_LL6:
# 3648
 while(_tmp65C != 0){
if(Cyc_Tcutil_is_noalias_pointer_or_aggr(((struct Cyc_Absyn_Aggrfield*)_tmp65C->hd)->type))return 1;
_tmp65C=_tmp65C->tl;}
# 3652
return 0;default: _LLD: _LLE:
# 3680
 return 0;}_LL0:;};}
# 3687
int Cyc_Tcutil_is_noalias_path(struct Cyc_Absyn_Exp*e){
void*_tmp65D=e->r;void*_tmp65E=_tmp65D;struct Cyc_Absyn_Stmt*_tmp674;struct Cyc_Absyn_Exp*_tmp673;struct Cyc_Absyn_Exp*_tmp672;struct Cyc_Absyn_Exp*_tmp671;struct Cyc_Absyn_Exp*_tmp670;struct Cyc_Absyn_Exp*_tmp66F;struct Cyc_Absyn_Exp*_tmp66E;struct Cyc_Absyn_Exp*_tmp66D;struct _fat_ptr*_tmp66C;struct Cyc_Absyn_Exp*_tmp66B;struct Cyc_Absyn_Exp*_tmp66A;switch(*((int*)_tmp65E)){case 1U: if(((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp65E)->f1)->tag == 1U){_LL1: _LL2:
 return 0;}else{goto _LL13;}case 22U: _LL3: _tmp66A=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp65E)->f1;_LL4:
 _tmp66B=_tmp66A;goto _LL6;case 20U: _LL5: _tmp66B=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp65E)->f1;_LL6:
# 3692
 return Cyc_Tcutil_is_noalias_pointer((void*)_check_null(_tmp66B->topt),1)&& Cyc_Tcutil_is_noalias_path(_tmp66B);case 21U: _LL7: _tmp66D=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp65E)->f1;_tmp66C=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp65E)->f2;_LL8:
 return Cyc_Tcutil_is_noalias_path(_tmp66D);case 23U: _LL9: _tmp66F=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp65E)->f1;_tmp66E=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp65E)->f2;_LLA: {
# 3695
void*_tmp65F=Cyc_Tcutil_compress((void*)_check_null(_tmp66F->topt));void*_tmp660=_tmp65F;if(((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp660)->tag == 6U){_LL16: _LL17:
 return Cyc_Tcutil_is_noalias_path(_tmp66F);}else{_LL18: _LL19:
 return 0;}_LL15:;}case 6U: _LLB: _tmp671=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp65E)->f2;_tmp670=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp65E)->f3;_LLC:
# 3700
 return Cyc_Tcutil_is_noalias_path(_tmp671)&& Cyc_Tcutil_is_noalias_path(_tmp670);case 9U: _LLD: _tmp672=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp65E)->f2;_LLE:
 _tmp673=_tmp672;goto _LL10;case 14U: _LLF: _tmp673=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp65E)->f2;_LL10:
 return Cyc_Tcutil_is_noalias_path(_tmp673);case 37U: _LL11: _tmp674=((struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct*)_tmp65E)->f1;_LL12:
# 3704
 while(1){
void*_tmp661=_tmp674->r;void*_tmp662=_tmp661;struct Cyc_Absyn_Exp*_tmp669;struct Cyc_Absyn_Decl*_tmp668;struct Cyc_Absyn_Stmt*_tmp667;struct Cyc_Absyn_Stmt*_tmp666;struct Cyc_Absyn_Stmt*_tmp665;switch(*((int*)_tmp662)){case 2U: _LL1B: _tmp666=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp662)->f1;_tmp665=((struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct*)_tmp662)->f2;_LL1C:
 _tmp674=_tmp665;goto _LL1A;case 12U: _LL1D: _tmp668=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp662)->f1;_tmp667=((struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct*)_tmp662)->f2;_LL1E:
 _tmp674=_tmp667;goto _LL1A;case 1U: _LL1F: _tmp669=((struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct*)_tmp662)->f1;_LL20:
 return Cyc_Tcutil_is_noalias_path(_tmp669);default: _LL21: _LL22:
({void*_tmp663=0U;({struct _fat_ptr _tmp8FD=({const char*_tmp664="is_noalias_stmt_exp: ill-formed StmtExp";_tag_fat(_tmp664,sizeof(char),40U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp8FD,_tag_fat(_tmp663,sizeof(void*),0U));});});}_LL1A:;}default: _LL13: _LL14:
# 3712
 return 1;}_LL0:;}
# 3729 "tcutil.cyc"
struct _tuple13 Cyc_Tcutil_addressof_props(struct Cyc_Tcenv_Tenv*te,struct Cyc_Absyn_Exp*e){
# 3731
struct _tuple13 bogus_ans=({struct _tuple13 _tmp76F;_tmp76F.f1=0,_tmp76F.f2=Cyc_Absyn_heap_rgn_type;_tmp76F;});
void*_tmp675=e->r;void*_tmp676=_tmp675;struct Cyc_Absyn_Exp*_tmp6AF;struct Cyc_Absyn_Exp*_tmp6AE;struct Cyc_Absyn_Exp*_tmp6AD;struct Cyc_Absyn_Exp*_tmp6AC;struct _fat_ptr*_tmp6AB;int _tmp6AA;struct Cyc_Absyn_Exp*_tmp6A9;struct _fat_ptr*_tmp6A8;int _tmp6A7;void*_tmp6A6;switch(*((int*)_tmp676)){case 1U: _LL1: _tmp6A6=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp676)->f1;_LL2: {
# 3735
void*_tmp677=_tmp6A6;struct Cyc_Absyn_Vardecl*_tmp67F;struct Cyc_Absyn_Vardecl*_tmp67E;struct Cyc_Absyn_Vardecl*_tmp67D;struct Cyc_Absyn_Vardecl*_tmp67C;switch(*((int*)_tmp677)){case 0U: _LLE: _LLF:
 goto _LL11;case 2U: _LL10: _LL11:
 return bogus_ans;case 1U: _LL12: _tmp67C=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp677)->f1;_LL13: {
# 3739
void*_tmp678=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp679=_tmp678;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp679)->tag == 4U){_LL1B: _LL1C:
# 3741
 return({struct _tuple13 _tmp763;_tmp763.f1=1,_tmp763.f2=Cyc_Absyn_heap_rgn_type;_tmp763;});}else{_LL1D: _LL1E:
 return({struct _tuple13 _tmp764;_tmp764.f1=(_tmp67C->tq).real_const,_tmp764.f2=Cyc_Absyn_heap_rgn_type;_tmp764;});}_LL1A:;}case 4U: _LL14: _tmp67D=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp677)->f1;_LL15: {
# 3745
void*_tmp67A=Cyc_Tcutil_compress((void*)_check_null(e->topt));void*_tmp67B=_tmp67A;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp67B)->tag == 4U){_LL20: _LL21:
 return({struct _tuple13 _tmp765;_tmp765.f1=1,_tmp765.f2=(void*)_check_null(_tmp67D->rgn);_tmp765;});}else{_LL22: _LL23:
# 3748
 _tmp67D->escapes=1;
return({struct _tuple13 _tmp766;_tmp766.f1=(_tmp67D->tq).real_const,_tmp766.f2=(void*)_check_null(_tmp67D->rgn);_tmp766;});}_LL1F:;}case 5U: _LL16: _tmp67E=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp677)->f1;_LL17:
# 3751
 _tmp67F=_tmp67E;goto _LL19;default: _LL18: _tmp67F=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp677)->f1;_LL19:
# 3753
 _tmp67F->escapes=1;
return({struct _tuple13 _tmp767;_tmp767.f1=(_tmp67F->tq).real_const,_tmp767.f2=(void*)_check_null(_tmp67F->rgn);_tmp767;});}_LLD:;}case 21U: _LL3: _tmp6A9=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp676)->f1;_tmp6A8=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp676)->f2;_tmp6A7=((struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct*)_tmp676)->f3;_LL4:
# 3758
 if(_tmp6A7)return bogus_ans;{
# 3761
void*_tmp680=Cyc_Tcutil_compress((void*)_check_null(_tmp6A9->topt));void*_tmp681=_tmp680;struct Cyc_Absyn_Aggrdecl*_tmp68D;struct Cyc_List_List*_tmp68C;switch(*((int*)_tmp681)){case 7U: _LL25: _tmp68C=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp681)->f2;_LL26: {
# 3763
struct Cyc_Absyn_Aggrfield*_tmp682=Cyc_Absyn_lookup_field(_tmp68C,_tmp6A8);
if(_tmp682 != 0 && _tmp682->width == 0){
struct _tuple13 _tmp683=Cyc_Tcutil_addressof_props(te,_tmp6A9);struct _tuple13 _tmp684=_tmp683;int _tmp686;void*_tmp685;_LL2C: _tmp686=_tmp684.f1;_tmp685=_tmp684.f2;_LL2D:;
return({struct _tuple13 _tmp768;_tmp768.f1=(_tmp682->tq).real_const || _tmp686,_tmp768.f2=_tmp685;_tmp768;});}
# 3768
return bogus_ans;}case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp681)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp681)->f1)->f1).KnownAggr).tag == 2){_LL27: _tmp68D=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp681)->f1)->f1).KnownAggr).val;_LL28: {
# 3770
struct Cyc_Absyn_Aggrfield*_tmp687=Cyc_Absyn_lookup_decl_field(_tmp68D,_tmp6A8);
if(_tmp687 != 0 && _tmp687->width == 0){
struct _tuple13 _tmp688=Cyc_Tcutil_addressof_props(te,_tmp6A9);struct _tuple13 _tmp689=_tmp688;int _tmp68B;void*_tmp68A;_LL2F: _tmp68B=_tmp689.f1;_tmp68A=_tmp689.f2;_LL30:;
return({struct _tuple13 _tmp769;_tmp769.f1=(_tmp687->tq).real_const || _tmp68B,_tmp769.f2=_tmp68A;_tmp769;});}
# 3775
return bogus_ans;}}else{goto _LL29;}}else{goto _LL29;}default: _LL29: _LL2A:
 return bogus_ans;}_LL24:;};case 22U: _LL5: _tmp6AC=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp676)->f1;_tmp6AB=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp676)->f2;_tmp6AA=((struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct*)_tmp676)->f3;_LL6:
# 3780
 if(_tmp6AA)return bogus_ans;{
# 3783
void*_tmp68E=Cyc_Tcutil_compress((void*)_check_null(_tmp6AC->topt));void*_tmp68F=_tmp68E;void*_tmp695;void*_tmp694;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68F)->tag == 3U){_LL32: _tmp695=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68F)->f1).elt_type;_tmp694=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp68F)->f1).ptr_atts).rgn;_LL33: {
# 3785
struct Cyc_Absyn_Aggrfield*finfo;
{void*_tmp690=Cyc_Tcutil_compress(_tmp695);void*_tmp691=_tmp690;struct Cyc_Absyn_Aggrdecl*_tmp693;struct Cyc_List_List*_tmp692;switch(*((int*)_tmp691)){case 7U: _LL37: _tmp692=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp691)->f2;_LL38:
# 3788
 finfo=Cyc_Absyn_lookup_field(_tmp692,_tmp6AB);goto _LL36;case 0U: if(((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp691)->f1)->tag == 20U){if(((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp691)->f1)->f1).KnownAggr).tag == 2){_LL39: _tmp693=*((((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp691)->f1)->f1).KnownAggr).val;_LL3A:
# 3790
 finfo=Cyc_Absyn_lookup_decl_field(_tmp693,_tmp6AB);goto _LL36;}else{goto _LL3B;}}else{goto _LL3B;}default: _LL3B: _LL3C:
 return bogus_ans;}_LL36:;}
# 3793
if(finfo != 0 && finfo->width == 0)
return({struct _tuple13 _tmp76A;_tmp76A.f1=(finfo->tq).real_const,_tmp76A.f2=_tmp694;_tmp76A;});
return bogus_ans;}}else{_LL34: _LL35:
 return bogus_ans;}_LL31:;};case 20U: _LL7: _tmp6AD=((struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct*)_tmp676)->f1;_LL8: {
# 3800
void*_tmp696=Cyc_Tcutil_compress((void*)_check_null(_tmp6AD->topt));void*_tmp697=_tmp696;struct Cyc_Absyn_Tqual _tmp699;void*_tmp698;if(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp697)->tag == 3U){_LL3E: _tmp699=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp697)->f1).elt_tq;_tmp698=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp697)->f1).ptr_atts).rgn;_LL3F:
# 3802
 return({struct _tuple13 _tmp76B;_tmp76B.f1=_tmp699.real_const,_tmp76B.f2=_tmp698;_tmp76B;});}else{_LL40: _LL41:
 return bogus_ans;}_LL3D:;}case 23U: _LL9: _tmp6AF=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp676)->f1;_tmp6AE=((struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct*)_tmp676)->f2;_LLA: {
# 3808
void*t=Cyc_Tcutil_compress((void*)_check_null(_tmp6AF->topt));
void*_tmp69A=t;struct Cyc_Absyn_Tqual _tmp6A3;struct Cyc_Absyn_Tqual _tmp6A2;void*_tmp6A1;struct Cyc_List_List*_tmp6A0;switch(*((int*)_tmp69A)){case 6U: _LL43: _tmp6A0=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp69A)->f1;_LL44: {
# 3812
struct _tuple12 _tmp69B=Cyc_Evexp_eval_const_uint_exp(_tmp6AE);struct _tuple12 _tmp69C=_tmp69B;unsigned _tmp69F;int _tmp69E;_LL4C: _tmp69F=_tmp69C.f1;_tmp69E=_tmp69C.f2;_LL4D:;
if(!_tmp69E)
return bogus_ans;{
struct _tuple11*_tmp69D=Cyc_Absyn_lookup_tuple_field(_tmp6A0,(int)_tmp69F);
if(_tmp69D != 0)
return({struct _tuple13 _tmp76C;_tmp76C.f1=((*_tmp69D).f1).real_const,({void*_tmp8FE=(Cyc_Tcutil_addressof_props(te,_tmp6AF)).f2;_tmp76C.f2=_tmp8FE;});_tmp76C;});
return bogus_ans;};}case 3U: _LL45: _tmp6A2=(((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69A)->f1).elt_tq;_tmp6A1=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp69A)->f1).ptr_atts).rgn;_LL46:
# 3820
 return({struct _tuple13 _tmp76D;_tmp76D.f1=_tmp6A2.real_const,_tmp76D.f2=_tmp6A1;_tmp76D;});case 4U: _LL47: _tmp6A3=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp69A)->f1).tq;_LL48:
# 3826
 return({struct _tuple13 _tmp76E;_tmp76E.f1=_tmp6A3.real_const,({void*_tmp8FF=(Cyc_Tcutil_addressof_props(te,_tmp6AF)).f2;_tmp76E.f2=_tmp8FF;});_tmp76E;});default: _LL49: _LL4A:
 return bogus_ans;}_LL42:;}default: _LLB: _LLC:
# 3830
({void*_tmp6A4=0U;({unsigned _tmp901=e->loc;struct _fat_ptr _tmp900=({const char*_tmp6A5="unary & applied to non-lvalue";_tag_fat(_tmp6A5,sizeof(char),30U);});Cyc_Tcutil_terr(_tmp901,_tmp900,_tag_fat(_tmp6A4,sizeof(void*),0U));});});
return bogus_ans;}_LL0:;}
# 3837
void Cyc_Tcutil_check_bound(unsigned loc,unsigned i,void*b,int do_warn){
struct Cyc_Absyn_Exp*_tmp6B0=({void*_tmp902=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp902,b);});
if(_tmp6B0 == 0)return;{
struct Cyc_Absyn_Exp*_tmp6B1=_tmp6B0;
struct _tuple12 _tmp6B2=Cyc_Evexp_eval_const_uint_exp(_tmp6B1);struct _tuple12 _tmp6B3=_tmp6B2;unsigned _tmp6BD;int _tmp6BC;_LL1: _tmp6BD=_tmp6B3.f1;_tmp6BC=_tmp6B3.f2;_LL2:;
if(_tmp6BC && _tmp6BD <= i){
if(do_warn)
({struct Cyc_Int_pa_PrintArg_struct _tmp6B6=({struct Cyc_Int_pa_PrintArg_struct _tmp771;_tmp771.tag=1U,_tmp771.f1=(unsigned long)((int)_tmp6BD);_tmp771;});struct Cyc_Int_pa_PrintArg_struct _tmp6B7=({struct Cyc_Int_pa_PrintArg_struct _tmp770;_tmp770.tag=1U,_tmp770.f1=(unsigned long)((int)i);_tmp770;});void*_tmp6B4[2U];_tmp6B4[0]=& _tmp6B6,_tmp6B4[1]=& _tmp6B7;({unsigned _tmp904=loc;struct _fat_ptr _tmp903=({const char*_tmp6B5="a dereference will be out of bounds: %d <= %d";_tag_fat(_tmp6B5,sizeof(char),46U);});Cyc_Tcutil_warn(_tmp904,_tmp903,_tag_fat(_tmp6B4,sizeof(void*),2U));});});else{
# 3846
({struct Cyc_Int_pa_PrintArg_struct _tmp6BA=({struct Cyc_Int_pa_PrintArg_struct _tmp773;_tmp773.tag=1U,_tmp773.f1=(unsigned long)((int)_tmp6BD);_tmp773;});struct Cyc_Int_pa_PrintArg_struct _tmp6BB=({struct Cyc_Int_pa_PrintArg_struct _tmp772;_tmp772.tag=1U,_tmp772.f1=(unsigned long)((int)i);_tmp772;});void*_tmp6B8[2U];_tmp6B8[0]=& _tmp6BA,_tmp6B8[1]=& _tmp6BB;({unsigned _tmp906=loc;struct _fat_ptr _tmp905=({const char*_tmp6B9="dereference is out of bounds: %d <= %d";_tag_fat(_tmp6B9,sizeof(char),39U);});Cyc_Tcutil_terr(_tmp906,_tmp905,_tag_fat(_tmp6B8,sizeof(void*),2U));});});}}
return;};}
# 3850
void Cyc_Tcutil_check_nonzero_bound(unsigned loc,void*b){
Cyc_Tcutil_check_bound(loc,0U,b,0);}
# 3859
static int Cyc_Tcutil_cnst_exp(int var_okay,struct Cyc_Absyn_Exp*e){
void*_tmp6BE=e->r;void*_tmp6BF=_tmp6BE;struct Cyc_List_List*_tmp6DE;struct Cyc_List_List*_tmp6DD;enum Cyc_Absyn_Primop _tmp6DC;struct Cyc_List_List*_tmp6DB;struct Cyc_List_List*_tmp6DA;struct Cyc_List_List*_tmp6D9;struct Cyc_List_List*_tmp6D8;struct Cyc_Absyn_Exp*_tmp6D7;struct Cyc_Absyn_Exp*_tmp6D6;struct Cyc_Absyn_Exp*_tmp6D5;struct Cyc_Absyn_Exp*_tmp6D4;void*_tmp6D3;struct Cyc_Absyn_Exp*_tmp6D2;void*_tmp6D1;struct Cyc_Absyn_Exp*_tmp6D0;struct Cyc_Absyn_Exp*_tmp6CF;struct Cyc_Absyn_Exp*_tmp6CE;struct Cyc_Absyn_Exp*_tmp6CD;struct Cyc_Absyn_Exp*_tmp6CC;struct Cyc_Absyn_Exp*_tmp6CB;struct Cyc_Absyn_Exp*_tmp6CA;struct Cyc_Absyn_Exp*_tmp6C9;struct Cyc_Absyn_Exp*_tmp6C8;void*_tmp6C7;switch(*((int*)_tmp6BF)){case 0U: _LL1: _LL2:
 goto _LL4;case 2U: _LL3: _LL4:
 goto _LL6;case 17U: _LL5: _LL6:
 goto _LL8;case 18U: _LL7: _LL8:
 goto _LLA;case 19U: _LL9: _LLA:
 goto _LLC;case 32U: _LLB: _LLC:
 goto _LLE;case 33U: _LLD: _LLE:
 return 1;case 1U: _LLF: _tmp6C7=(void*)((struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL10: {
# 3871
void*_tmp6C0=_tmp6C7;struct Cyc_Absyn_Vardecl*_tmp6C6;struct Cyc_Absyn_Vardecl*_tmp6C5;switch(*((int*)_tmp6C0)){case 2U: _LL34: _LL35:
 return 1;case 1U: _LL36: _tmp6C5=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp6C0)->f1;_LL37: {
# 3874
void*_tmp6C1=Cyc_Tcutil_compress(_tmp6C5->type);void*_tmp6C2=_tmp6C1;switch(*((int*)_tmp6C2)){case 4U: _LL3F: _LL40:
 goto _LL42;case 5U: _LL41: _LL42:
 return 1;default: _LL43: _LL44:
 return var_okay;}_LL3E:;}case 4U: _LL38: _tmp6C6=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp6C0)->f1;_LL39:
# 3881
 if((int)_tmp6C6->sc == (int)Cyc_Absyn_Static){
void*_tmp6C3=Cyc_Tcutil_compress(_tmp6C6->type);void*_tmp6C4=_tmp6C3;switch(*((int*)_tmp6C4)){case 4U: _LL46: _LL47:
 goto _LL49;case 5U: _LL48: _LL49:
 return 1;default: _LL4A: _LL4B:
 return var_okay;}_LL45:;}else{
# 3888
return var_okay;}case 0U: _LL3A: _LL3B:
 return 0;default: _LL3C: _LL3D:
 return var_okay;}_LL33:;}case 6U: _LL11: _tmp6CA=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_tmp6C9=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6BF)->f2;_tmp6C8=((struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct*)_tmp6BF)->f3;_LL12:
# 3894
 return(Cyc_Tcutil_cnst_exp(0,_tmp6CA)&&
 Cyc_Tcutil_cnst_exp(0,_tmp6C9))&&
 Cyc_Tcutil_cnst_exp(0,_tmp6C8);case 9U: _LL13: _tmp6CC=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_tmp6CB=((struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct*)_tmp6BF)->f2;_LL14:
# 3898
 return Cyc_Tcutil_cnst_exp(0,_tmp6CC)&& Cyc_Tcutil_cnst_exp(0,_tmp6CB);case 41U: _LL15: _tmp6CD=((struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL16:
 _tmp6CE=_tmp6CD;goto _LL18;case 12U: _LL17: _tmp6CE=((struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL18:
 _tmp6CF=_tmp6CE;goto _LL1A;case 13U: _LL19: _tmp6CF=((struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL1A:
# 3902
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp6CF);case 14U: if(((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BF)->f4 == Cyc_Absyn_No_coercion){_LL1B: _tmp6D1=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_tmp6D0=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BF)->f2;_LL1C:
# 3904
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp6D0);}else{_LL1D: _tmp6D3=(void*)((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_tmp6D2=((struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct*)_tmp6BF)->f2;_LL1E:
# 3907
 return Cyc_Tcutil_cnst_exp(var_okay,_tmp6D2);}case 15U: _LL1F: _tmp6D4=((struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL20:
# 3909
 return Cyc_Tcutil_cnst_exp(1,_tmp6D4);case 27U: _LL21: _tmp6D6=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp6BF)->f2;_tmp6D5=((struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct*)_tmp6BF)->f3;_LL22:
# 3911
 return Cyc_Tcutil_cnst_exp(0,_tmp6D6)&& Cyc_Tcutil_cnst_exp(0,_tmp6D5);case 28U: _LL23: _tmp6D7=((struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL24:
# 3913
 return Cyc_Tcutil_cnst_exp(0,_tmp6D7);case 26U: _LL25: _tmp6D8=((struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL26:
 _tmp6D9=_tmp6D8;goto _LL28;case 30U: _LL27: _tmp6D9=((struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct*)_tmp6BF)->f2;_LL28:
 _tmp6DA=_tmp6D9;goto _LL2A;case 29U: _LL29: _tmp6DA=((struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct*)_tmp6BF)->f3;_LL2A:
# 3917
 for(0;_tmp6DA != 0;_tmp6DA=_tmp6DA->tl){
if(!Cyc_Tcutil_cnst_exp(0,(*((struct _tuple15*)_tmp6DA->hd)).f2))
return 0;}
return 1;case 3U: _LL2B: _tmp6DC=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_tmp6DB=((struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct*)_tmp6BF)->f2;_LL2C:
# 3922
 _tmp6DD=_tmp6DB;goto _LL2E;case 24U: _LL2D: _tmp6DD=((struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL2E:
 _tmp6DE=_tmp6DD;goto _LL30;case 31U: _LL2F: _tmp6DE=((struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct*)_tmp6BF)->f1;_LL30:
# 3925
 for(0;_tmp6DE != 0;_tmp6DE=_tmp6DE->tl){
if(!Cyc_Tcutil_cnst_exp(0,(struct Cyc_Absyn_Exp*)_tmp6DE->hd))
return 0;}
return 1;default: _LL31: _LL32:
 return 0;}_LL0:;}
# 3932
int Cyc_Tcutil_is_const_exp(struct Cyc_Absyn_Exp*e){
return Cyc_Tcutil_cnst_exp(0,e);}
# 3936
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
int Cyc_Tcutil_zeroable_type(void*t){
void*_tmp6DF=Cyc_Tcutil_compress(t);void*_tmp6E0=_tmp6DF;struct Cyc_List_List*_tmp6F0;struct Cyc_List_List*_tmp6EF;void*_tmp6EE;void*_tmp6ED;void*_tmp6EC;struct Cyc_List_List*_tmp6EB;switch(*((int*)_tmp6E0)){case 0U: _LL1: _tmp6EC=(void*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E0)->f1;_tmp6EB=((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp6E0)->f2;_LL2: {
# 3940
void*_tmp6E1=_tmp6EC;union Cyc_Absyn_AggrInfo _tmp6EA;struct Cyc_List_List*_tmp6E9;struct Cyc_Absyn_Enumdecl*_tmp6E8;switch(*((int*)_tmp6E1)){case 0U: _LLE: _LLF:
 goto _LL11;case 1U: _LL10: _LL11:
 goto _LL13;case 2U: _LL12: _LL13:
 return 1;case 15U: _LL14: _tmp6E8=((struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct*)_tmp6E1)->f2;_LL15:
# 3945
 if(_tmp6E8 == 0 || _tmp6E8->fields == 0)
return 0;
_tmp6E9=(struct Cyc_List_List*)((struct Cyc_Core_Opt*)_check_null(_tmp6E8->fields))->v;goto _LL17;case 16U: _LL16: _tmp6E9=((struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct*)_tmp6E1)->f1;_LL17:
# 3949
{struct Cyc_List_List*_tmp6E2=_tmp6E9;for(0;_tmp6E2 != 0;_tmp6E2=_tmp6E2->tl){
if(((struct Cyc_Absyn_Enumfield*)_tmp6E2->hd)->tag == 0)
return _tmp6E2 == _tmp6E9;{
struct _tuple12 _tmp6E3=Cyc_Evexp_eval_const_uint_exp((struct Cyc_Absyn_Exp*)_check_null(((struct Cyc_Absyn_Enumfield*)_tmp6E2->hd)->tag));struct _tuple12 _tmp6E4=_tmp6E3;unsigned _tmp6E6;int _tmp6E5;_LL1D: _tmp6E6=_tmp6E4.f1;_tmp6E5=_tmp6E4.f2;_LL1E:;
if(_tmp6E5 && _tmp6E6 == (unsigned)0)
return 1;};}}
# 3956
return 0;case 20U: _LL18: _tmp6EA=((struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct*)_tmp6E1)->f1;_LL19: {
# 3959
struct Cyc_Absyn_Aggrdecl*_tmp6E7=Cyc_Absyn_get_known_aggrdecl(_tmp6EA);
if(_tmp6E7->impl == 0)return 0;
if(((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6E7->impl))->exist_vars != 0)return 0;
if((int)_tmp6E7->kind == (int)Cyc_Absyn_UnionA &&((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6E7->impl))->tagged)return 0;
return Cyc_Tcutil_fields_zeroable(_tmp6E7->tvs,_tmp6EB,((struct Cyc_Absyn_AggrdeclImpl*)_check_null(_tmp6E7->impl))->fields);}default: _LL1A: _LL1B:
 return 0;}_LLD:;}case 3U: _LL3: _tmp6ED=((((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp6E0)->f1).ptr_atts).nullable;_LL4:
# 3967
 return Cyc_Tcutil_force_type2bool(1,_tmp6ED);case 4U: _LL5: _tmp6EE=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp6E0)->f1).elt_type;_LL6:
 return Cyc_Tcutil_zeroable_type(_tmp6EE);case 6U: _LL7: _tmp6EF=((struct Cyc_Absyn_TupleType_Absyn_Type_struct*)_tmp6E0)->f1;_LL8:
# 3970
 for(0;_tmp6EF != 0;_tmp6EF=_tmp6EF->tl){
if(!Cyc_Tcutil_zeroable_type((*((struct _tuple11*)_tmp6EF->hd)).f2))return 0;}
return 1;case 7U: _LL9: _tmp6F0=((struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct*)_tmp6E0)->f2;_LLA:
 return Cyc_Tcutil_fields_zeroable(0,0,_tmp6F0);default: _LLB: _LLC:
 return 0;}_LL0:;}
# 3977
static int Cyc_Tcutil_fields_zeroable(struct Cyc_List_List*tvs,struct Cyc_List_List*ts,struct Cyc_List_List*fs){
# 3979
struct _RegionHandle _tmp6F1=_new_region("rgn");struct _RegionHandle*rgn=& _tmp6F1;_push_region(rgn);
{struct Cyc_List_List*_tmp6F2=((struct Cyc_List_List*(*)(struct _RegionHandle*r1,struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_rzip)(rgn,rgn,tvs,ts);
for(0;fs != 0;fs=fs->tl){
void*t=((struct Cyc_Absyn_Aggrfield*)fs->hd)->type;
if(Cyc_Tcutil_zeroable_type(t))continue;
t=Cyc_Tcutil_rsubstitute(rgn,_tmp6F2,((struct Cyc_Absyn_Aggrfield*)fs->hd)->type);
if(!Cyc_Tcutil_zeroable_type(t)){int _tmp6F3=0;_npop_handler(0U);return _tmp6F3;}}{
# 3987
int _tmp6F4=1;_npop_handler(0U);return _tmp6F4;};}
# 3980
;_pop_region(rgn);}
# 3991
void Cyc_Tcutil_check_no_qual(unsigned loc,void*t){
void*_tmp6F5=t;struct Cyc_Absyn_Typedefdecl*_tmp6FA;if(((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6F5)->tag == 8U){_LL1: _tmp6FA=((struct Cyc_Absyn_TypedefType_Absyn_Type_struct*)_tmp6F5)->f3;_LL2:
# 3994
 if(_tmp6FA != 0){
struct Cyc_Absyn_Tqual _tmp6F6=_tmp6FA->tq;
if(((_tmp6F6.print_const || _tmp6F6.q_volatile)|| _tmp6F6.q_restrict)|| _tmp6F6.real_const)
({struct Cyc_String_pa_PrintArg_struct _tmp6F9=({struct Cyc_String_pa_PrintArg_struct _tmp774;_tmp774.tag=0U,({struct _fat_ptr _tmp907=(struct _fat_ptr)((struct _fat_ptr)Cyc_Absynpp_typ2string(t));_tmp774.f1=_tmp907;});_tmp774;});void*_tmp6F7[1U];_tmp6F7[0]=& _tmp6F9;({unsigned _tmp909=loc;struct _fat_ptr _tmp908=({const char*_tmp6F8="qualifier within typedef type %s is ignored";_tag_fat(_tmp6F8,sizeof(char),44U);});Cyc_Tcutil_warn(_tmp909,_tmp908,_tag_fat(_tmp6F7,sizeof(void*),1U));});});}
# 3999
goto _LL0;}else{_LL3: _LL4:
 goto _LL0;}_LL0:;}
# 4006
struct Cyc_List_List*Cyc_Tcutil_transfer_fn_type_atts(void*t,struct Cyc_List_List*atts){
void*_tmp6FB=Cyc_Tcutil_compress(t);void*_tmp6FC=_tmp6FB;struct Cyc_List_List**_tmp702;if(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6FC)->tag == 5U){_LL1: _tmp702=(struct Cyc_List_List**)&(((struct Cyc_Absyn_FnType_Absyn_Type_struct*)_tmp6FC)->f1).attributes;_LL2: {
# 4009
struct Cyc_List_List*_tmp6FD=0;
for(0;atts != 0;atts=atts->tl){
if(Cyc_Absyn_fntype_att((void*)atts->hd)){
if(!((int(*)(int(*compare)(void*,void*),struct Cyc_List_List*l,void*x))Cyc_List_mem)(Cyc_Absyn_attribute_cmp,*_tmp702,(void*)atts->hd))
({struct Cyc_List_List*_tmp90A=({struct Cyc_List_List*_tmp6FE=_cycalloc(sizeof(*_tmp6FE));_tmp6FE->hd=(void*)atts->hd,_tmp6FE->tl=*_tmp702;_tmp6FE;});*_tmp702=_tmp90A;});}else{
# 4016
_tmp6FD=({struct Cyc_List_List*_tmp6FF=_cycalloc(sizeof(*_tmp6FF));_tmp6FF->hd=(void*)atts->hd,_tmp6FF->tl=_tmp6FD;_tmp6FF;});}}
return _tmp6FD;}}else{_LL3: _LL4:
({void*_tmp700=0U;({struct _fat_ptr _tmp90B=({const char*_tmp701="transfer_fn_type_atts";_tag_fat(_tmp701,sizeof(char),22U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp90B,_tag_fat(_tmp700,sizeof(void*),0U));});});}_LL0:;}
# 4023
struct Cyc_Absyn_Exp*Cyc_Tcutil_get_type_bound(void*t){
void*_tmp703=Cyc_Tcutil_compress(t);void*_tmp704=_tmp703;struct Cyc_Absyn_Exp*_tmp706;struct Cyc_Absyn_PtrInfo _tmp705;switch(*((int*)_tmp704)){case 3U: _LL1: _tmp705=((struct Cyc_Absyn_PointerType_Absyn_Type_struct*)_tmp704)->f1;_LL2:
 return({void*_tmp90C=Cyc_Absyn_bounds_one();Cyc_Tcutil_get_bounds_exp(_tmp90C,(_tmp705.ptr_atts).bounds);});case 4U: _LL3: _tmp706=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp704)->f1).num_elts;_LL4:
 return _tmp706;default: _LL5: _LL6:
 return 0;}_LL0:;}
# 4033
struct Cyc_Absyn_Vardecl*Cyc_Tcutil_nonesc_vardecl(void*b){
void*_tmp707=b;struct Cyc_Absyn_Vardecl*_tmp70B;struct Cyc_Absyn_Vardecl*_tmp70A;struct Cyc_Absyn_Vardecl*_tmp709;struct Cyc_Absyn_Vardecl*_tmp708;switch(*((int*)_tmp707)){case 5U: _LL1: _tmp708=((struct Cyc_Absyn_Pat_b_Absyn_Binding_struct*)_tmp707)->f1;_LL2:
 _tmp709=_tmp708;goto _LL4;case 4U: _LL3: _tmp709=((struct Cyc_Absyn_Local_b_Absyn_Binding_struct*)_tmp707)->f1;_LL4:
 _tmp70A=_tmp709;goto _LL6;case 3U: _LL5: _tmp70A=((struct Cyc_Absyn_Param_b_Absyn_Binding_struct*)_tmp707)->f1;_LL6:
 _tmp70B=_tmp70A;goto _LL8;case 1U: _LL7: _tmp70B=((struct Cyc_Absyn_Global_b_Absyn_Binding_struct*)_tmp707)->f1;_LL8:
# 4039
 if(!_tmp70B->escapes)
return _tmp70B;
return 0;default: _LL9: _LLA:
 return 0;}_LL0:;}
# 4047
struct Cyc_List_List*Cyc_Tcutil_filter_nulls(struct Cyc_List_List*l){
struct Cyc_List_List*_tmp70C=0;
{struct Cyc_List_List*x=l;for(0;x != 0;x=x->tl){
if((void**)x->hd != 0)_tmp70C=({struct Cyc_List_List*_tmp70D=_cycalloc(sizeof(*_tmp70D));_tmp70D->hd=*((void**)_check_null((void**)x->hd)),_tmp70D->tl=_tmp70C;_tmp70D;});}}
return _tmp70C;}
# 4054
void*Cyc_Tcutil_promote_array(void*t,void*rgn,int convert_tag){
void*_tmp70E=Cyc_Tcutil_compress(t);void*_tmp70F=_tmp70E;void*_tmp719;struct Cyc_Absyn_Tqual _tmp718;struct Cyc_Absyn_Exp*_tmp717;void*_tmp716;unsigned _tmp715;if(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70F)->tag == 4U){_LL1: _tmp719=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70F)->f1).elt_type;_tmp718=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70F)->f1).tq;_tmp717=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70F)->f1).num_elts;_tmp716=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70F)->f1).zero_term;_tmp715=(((struct Cyc_Absyn_ArrayType_Absyn_Type_struct*)_tmp70F)->f1).zt_loc;_LL2: {
# 4057
void*b;
if(_tmp717 == 0)
b=Cyc_Absyn_fat_bound_type;else{
# 4061
if(convert_tag){
if(_tmp717->topt == 0)
({void*_tmp710=0U;({struct _fat_ptr _tmp90D=({const char*_tmp711="cannot convert tag without type!";_tag_fat(_tmp711,sizeof(char),33U);});((int(*)(struct _fat_ptr fmt,struct _fat_ptr ap))Cyc_Tcutil_impos)(_tmp90D,_tag_fat(_tmp710,sizeof(void*),0U));});});{
void*_tmp712=Cyc_Tcutil_compress((void*)_check_null(_tmp717->topt));void*_tmp713=_tmp712;void*_tmp714;if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp713)->tag == 0U){if(((struct Cyc_Absyn_TagCon_Absyn_TyCon_struct*)((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp713)->f1)->tag == 4U){if(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp713)->f2 != 0){_LL6: _tmp714=(void*)(((struct Cyc_Absyn_AppType_Absyn_Type_struct*)_tmp713)->f2)->hd;_LL7:
# 4067
 b=Cyc_Absyn_thin_bounds_exp(Cyc_Absyn_valueof_exp(_tmp714,0U));
goto _LL5;}else{goto _LL8;}}else{goto _LL8;}}else{_LL8: _LL9:
# 4070
 b=Cyc_Tcutil_is_const_exp(_tmp717)?Cyc_Absyn_thin_bounds_exp(_tmp717): Cyc_Absyn_fat_bound_type;}_LL5:;};}else{
# 4074
b=Cyc_Absyn_thin_bounds_exp(_tmp717);}}
# 4076
return Cyc_Absyn_atb_type(_tmp719,rgn,_tmp718,b,_tmp716);}}else{_LL3: _LL4:
 return t;}_LL0:;}
