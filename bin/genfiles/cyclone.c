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
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
struct _pool;
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 292 "core.h"
void Cyc_Core_rethrow(void*);
# 296
const char*Cyc_Core_get_exn_name(void*);
# 298
const char*Cyc_Core_get_exn_filename (void);
# 305
int Cyc_Core_get_exn_lineno (void);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*)(void*),struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*)(void*),struct Cyc_List_List*);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr,struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
# 66 "arg.h"
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 69
extern int Cyc_Arg_current;
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);struct Cyc___cycFILE;
# 51 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdout;
# 53
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 79
extern int Cyc_fclose(struct Cyc___cycFILE*);
# 84
extern int Cyc_feof(struct Cyc___cycFILE*);
# 88
extern int Cyc_fflush(struct Cyc___cycFILE*);
# 98
extern struct Cyc___cycFILE*Cyc_fopen(const char*,const char*);
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 108
extern unsigned long Cyc_fread(struct _fat_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 140 "cycboot.h"
extern unsigned long Cyc_fwrite(struct _fat_ptr,unsigned long,unsigned long,struct Cyc___cycFILE*);
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 167
extern int remove(const char*);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 271 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_file_open(struct _fat_ptr,struct _fat_ptr);
extern void Cyc_file_close(struct Cyc___cycFILE*);
# 313
extern char*getenv(const char*);
# 318
extern int system(const char*);
extern void exit(int);struct Cyc_timeval{long tv_sec;long tv_usec;};
# 328
extern int gettimeofday(struct Cyc_timeval*,void*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 51
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 104 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 109
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);
# 120
extern struct _fat_ptr Cyc_strchr(struct _fat_ptr,char);
# 30 "filename.h"
extern struct _fat_ptr Cyc_Filename_concat(struct _fat_ptr,struct _fat_ptr);
# 34
extern struct _fat_ptr Cyc_Filename_chop_extension(struct _fat_ptr);
# 40
extern struct _fat_ptr Cyc_Filename_dirname(struct _fat_ptr);
# 43
extern struct _fat_ptr Cyc_Filename_basename(struct _fat_ptr);
# 46
extern int Cyc_Filename_check_suffix(struct _fat_ptr,struct _fat_ptr);
# 28 "position.h"
extern void Cyc_Position_reset_position(struct _fat_ptr);struct Cyc_Position_Error;
# 46
extern int Cyc_Position_use_gcc_style_location;
# 48
extern int Cyc_Position_max_errors;
# 50
extern int Cyc_Position_error_p (void);struct Cyc_Relations_Reln;struct _union_Nmspace_Rel_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Abs_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_C_n{int tag;struct Cyc_List_List*val;};struct _union_Nmspace_Loc_n{int tag;int val;};union Cyc_Absyn_Nmspace{struct _union_Nmspace_Rel_n Rel_n;struct _union_Nmspace_Abs_n Abs_n;struct _union_Nmspace_C_n C_n;struct _union_Nmspace_Loc_n Loc_n;};struct _tuple0{union Cyc_Absyn_Nmspace f1;struct _fat_ptr*f2;};
# 149 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 170
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 175
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 180
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;void*released;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};
# 392 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_throw_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 465
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 472
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 490
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 24 "parse.h"
struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*);extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
# 26 "absyndump.h"
void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*);
void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*,struct Cyc___cycFILE*);
void Cyc_Absyndump_dump_interface(struct Cyc_List_List*,struct Cyc___cycFILE*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 29 "binding.h"
void Cyc_Binding_resolve_all(struct Cyc_List_List*);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 38
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*);
# 29 "tc.h"
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*,int,struct Cyc_List_List*);
# 33
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 35 "toc.h"
struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*,struct Cyc_List_List*);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 48
void Cyc_Toc_finish (void);
# 26 "remove_aggregates.h"
struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*);
# 25 "remove_labels.h"
struct Cyc_List_List*Cyc_RemoveLabels_remove_unused_labels(struct Cyc_List_List*);
# 27 "toseqc.h"
extern struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;int f4;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple11{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple11 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 32 "new_control_flow.h"
void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*);
# 31 "warn.h"
void Cyc_Warn_flush_warnings (void);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};struct Cyc_AssnDef_Const_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_AssnDef_Var_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_AssnDef_LogicVar_AssnDef_Term_struct{int tag;struct Cyc_Absyn_Vardecl*f1;int f2;void*f3;};struct Cyc_AssnDef_Primop_AssnDef_Term_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_AssnDef_Cast_AssnDef_Term_struct{int tag;void*f1;void*f2;};
# 79 "assndef.h"
enum Cyc_AssnDef_Primreln{Cyc_AssnDef_Eq =0U,Cyc_AssnDef_Neq =1U,Cyc_AssnDef_SLt =2U,Cyc_AssnDef_SLte =3U,Cyc_AssnDef_ULt =4U,Cyc_AssnDef_ULte =5U};struct Cyc_AssnDef_True_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_False_AssnDef_Assn_struct{int tag;};struct Cyc_AssnDef_Prim_AssnDef_Assn_struct{int tag;void*f1;enum Cyc_AssnDef_Primreln f2;void*f3;};struct Cyc_AssnDef_And_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Or_AssnDef_Assn_struct{int tag;void*f1;void*f2;};struct Cyc_AssnDef_Subst_AssnDef_Assn_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;void*f3;};struct Cyc_AssnDef_Kill_AssnDef_Assn_struct{int tag;void*f1;};extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 40 "insert_checks.h"
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*,int);struct Cyc_Interface_I;
# 36 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_empty (void);
# 45 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_final (void);
# 49
struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple12{struct _fat_ptr f1;struct _fat_ptr f2;};
# 57
int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*,struct Cyc_Interface_I*,struct _tuple12*);
# 72 "interface.h"
struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*)(void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 78
struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 26 "flags.h"
extern int Cyc_Flags_porting_c_code;
# 30
extern int Cyc_Flags_no_regions;
# 34
extern int Cyc_Flags_no_register;
# 37
extern int Cyc_Flags_warn_override;
# 47
extern int Cyc_Flags_warn_alias_coerce;
# 51
extern int Cyc_Flags_warn_region_coerce;
# 54
extern int Cyc_Flags_warn_lose_unique;
# 58
extern int Cyc_Flags_warn_bounds_checks;
# 62
extern int Cyc_Flags_warn_all_null_deref;
# 66
extern int Cyc_Flags_noexpand_r;
# 72
extern int Cyc_Flags_tovc_r;
enum Cyc_Flags_C_Compilers{Cyc_Flags_Gcc_c =0U,Cyc_Flags_Vc_c =1U};
# 77
extern enum Cyc_Flags_C_Compilers Cyc_Flags_c_compiler;
# 87 "flags.h"
enum Cyc_Flags_Cyclone_Passes{Cyc_Flags_Cpp =0U,Cyc_Flags_Parsing =1U,Cyc_Flags_Binding =2U,Cyc_Flags_CurrentRegion =3U,Cyc_Flags_TypeChecking =4U,Cyc_Flags_Jumps =5U,Cyc_Flags_FlowAnalysis =6U,Cyc_Flags_VCGen =7U,Cyc_Flags_CheckInsertion =8U,Cyc_Flags_Toc =9U,Cyc_Flags_AggregateRemoval =10U,Cyc_Flags_LabelRemoval =11U,Cyc_Flags_EvalOrder =12U,Cyc_Flags_CCompiler =13U,Cyc_Flags_AllPasses =14U};
# 104
extern enum Cyc_Flags_Cyclone_Passes Cyc_Flags_stop_after_pass;
# 106
void Cyc_Flags_set_cpponly (void);
void Cyc_Flags_set_parseonly (void);
void Cyc_Flags_set_crgnonly (void);
void Cyc_Flags_set_tconly (void);
void Cyc_Flags_set_cfonly (void);
void Cyc_Flags_set_toconly (void);
# 117
extern int Cyc_Flags_compile_for_boot;
# 121
extern int Cyc_Flags_print_parser_state_and_token;
# 124
void Cyc_Flags_set_all_warnings (void);
# 29 "specsfile.h"
struct _fat_ptr Cyc_Specsfile_target_arch;
void Cyc_Specsfile_set_target_arch(struct _fat_ptr);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr);
# 34
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_split_specs(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*,struct _fat_ptr);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _fat_ptr Cyc_Specsfile_def_lib_path;
struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*,void(*)(struct _fat_ptr),int(*)(struct _fat_ptr),struct _fat_ptr,struct _fat_ptr);
# 44
struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr);
struct _fat_ptr Cyc_Specsfile_find_in_exec_path(struct _fat_ptr);
# 38 "cifc.h"
void Cyc_Cifc_set_inst_tvar (void);
# 44 "cyclone.cyc"
void Cyc_Port_port(struct Cyc_List_List*);
# 47
void Cyc_Lex_pos_init (void);
void Cyc_Lex_lex_init(int);
# 55
static int Cyc_pp_r=0;
static int Cyc_noshake_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_elim_se_r=0;
static int Cyc_v_r=0;
static int Cyc_save_temps_r=0;
static int Cyc_save_c_r=0;
static int Cyc_nogc_r=0;
static int Cyc_pa_r=0;
static int Cyc_pg_r=0;
static int Cyc_nocheck_r=0;
static int Cyc_add_cyc_namespace_r=1;
static int Cyc_generate_line_directives_r=1;
static int Cyc_print_full_evars_r=0;
static int Cyc_print_all_tvars_r=0;
static int Cyc_print_all_kinds_r=0;
static int Cyc_print_all_effects_r=0;
static int Cyc_nocyc_setjmp_r=0;
static int Cyc_generate_interface_r=0;
static int Cyc_toseqc_r=1;
static int Cyc_pthread_r=0;
static int Cyc_vcgen_r=1;
# 82
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _fat_ptr s){
Cyc_ccargs=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));({struct _fat_ptr*_tmp2CB=({struct _fat_ptr*_tmp0=_cycalloc(sizeof(*_tmp0));*_tmp0=s;_tmp0;});_tmp1->hd=_tmp2CB;}),_tmp1->tl=Cyc_ccargs;_tmp1;});}
# 90
void Cyc_set_c_compiler(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2CC=(struct _fat_ptr)s;Cyc_strcmp(_tmp2CC,({const char*_tmp2="vc";_tag_fat(_tmp2,sizeof(char),3U);}));})== 0){
Cyc_Flags_c_compiler=1U;
Cyc_add_ccarg(({const char*_tmp3="-DVC_C";_tag_fat(_tmp3,sizeof(char),7U);}));}else{
# 95
if(({struct _fat_ptr _tmp2CD=(struct _fat_ptr)s;Cyc_strcmp(_tmp2CD,({const char*_tmp4="gcc";_tag_fat(_tmp4,sizeof(char),4U);}));})== 0){
Cyc_Flags_c_compiler=0U;
Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";_tag_fat(_tmp5,sizeof(char),8U);}));}}}
# 102
void Cyc_set_port_c_code (void){
Cyc_Flags_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_Flags_set_parseonly();}
# 109
static struct _fat_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _fat_ptr s){
Cyc_output_file=({struct _fat_ptr*_tmp6=_cycalloc(sizeof(*_tmp6));*_tmp6=s;_tmp6;});}
# 114
static struct _fat_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _fat_ptr s){
Cyc_specified_interface=s;}
# 124
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Cversion;static char _tmp7[1U]="";
# 129
static struct _fat_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1U};
static void Cyc_set_cpp(struct _fat_ptr s){
Cyc_cpp=s;}
# 134
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));({struct _fat_ptr*_tmp2CE=({struct _fat_ptr*_tmp8=_cycalloc(sizeof(*_tmp8));*_tmp8=s;_tmp8;});_tmp9->hd=_tmp2CE;}),_tmp9->tl=Cyc_cppargs;_tmp9;});}
# 139
static void Cyc_add_cpp_and_ccarg(struct _fat_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 144
static void Cyc_add_iprefix(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp293;_tmp293.tag=0,_tmp293.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp293;});void*_tmpA[1];_tmpA[0]=& _tmpC;({struct _fat_ptr _tmp2CF=({const char*_tmpB="-iprefix %s";_tag_fat(_tmpB,sizeof(char),12U);});Cyc_aprintf(_tmp2CF,_tag_fat(_tmpA,sizeof(void*),1));});}));}
# 147
static void Cyc_add_iwithprefix(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmp294;_tmp294.tag=0,_tmp294.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp294;});void*_tmpD[1];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp2D0=({const char*_tmpE="-iwithprefix %s";_tag_fat(_tmpE,sizeof(char),16U);});Cyc_aprintf(_tmp2D0,_tag_fat(_tmpD,sizeof(void*),1));});}));}
# 150
static void Cyc_add_iwithprefixbefore(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp295;_tmp295.tag=0,_tmp295.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp295;});void*_tmp10[1];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp2D1=({const char*_tmp11="-iwithprefixbefore %s";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp2D1,_tag_fat(_tmp10,sizeof(void*),1));});}));}
# 153
static void Cyc_add_isystem(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp296;_tmp296.tag=0,_tmp296.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp296;});void*_tmp13[1];_tmp13[0]=& _tmp15;({struct _fat_ptr _tmp2D2=({const char*_tmp14="-isystem %s";_tag_fat(_tmp14,sizeof(char),12U);});Cyc_aprintf(_tmp2D2,_tag_fat(_tmp13,sizeof(void*),1));});}));}
# 156
static void Cyc_add_idirafter(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp297;_tmp297.tag=0,_tmp297.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp297;});void*_tmp16[1];_tmp16[0]=& _tmp18;({struct _fat_ptr _tmp2D3=({const char*_tmp17="-idirafter %s";_tag_fat(_tmp17,sizeof(char),14U);});Cyc_aprintf(_tmp2D3,_tag_fat(_tmp16,sizeof(void*),1));});}));}
# 160
static void Cyc_set_many_errors (void){
Cyc_Position_max_errors=65535;}static char _tmp19[20U]="_see_cycspecs_file_";
# 164
static struct _fat_ptr Cyc_def_inc_path={_tmp19,_tmp19,_tmp19 + 20U};
# 167
static void Cyc_print_version (void){
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp298;_tmp298.tag=0,({struct _fat_ptr _tmp2D4=(struct _fat_ptr)({char*_tmp1D=Cversion;_tag_fat(_tmp1D,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D,1U));});_tmp298.f1=_tmp2D4;});_tmp298;});void*_tmp1A[1];_tmp1A[0]=& _tmp1C;({struct _fat_ptr _tmp2D5=({const char*_tmp1B="The Cyclone compiler, version %s\n";_tag_fat(_tmp1B,sizeof(char),34U);});Cyc_printf(_tmp2D5,_tag_fat(_tmp1A,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp299;_tmp299.tag=0,({struct _fat_ptr _tmp2D6=(struct _fat_ptr)({char*_tmp21=Carch;_tag_fat(_tmp21,sizeof(char),_get_zero_arr_size_char((void*)_tmp21,1U));});_tmp299.f1=_tmp2D6;});_tmp299;});void*_tmp1E[1];_tmp1E[0]=& _tmp20;({struct _fat_ptr _tmp2D7=({const char*_tmp1F="Compiled for architecture: %s\n";_tag_fat(_tmp1F,sizeof(char),31U);});Cyc_printf(_tmp2D7,_tag_fat(_tmp1E,sizeof(void*),1));});});
({struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp29A;_tmp29A.tag=0,({struct _fat_ptr _tmp2D8=(struct _fat_ptr)({char*_tmp25=Cdef_lib_path;_tag_fat(_tmp25,sizeof(char),_get_zero_arr_size_char((void*)_tmp25,1U));});_tmp29A.f1=_tmp2D8;});_tmp29A;});void*_tmp22[1];_tmp22[0]=& _tmp24;({struct _fat_ptr _tmp2D9=({const char*_tmp23="Standard library directory: %s\n";_tag_fat(_tmp23,sizeof(char),32U);});Cyc_printf(_tmp2D9,_tag_fat(_tmp22,sizeof(void*),1));});});
# 172
({struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp29B;_tmp29B.tag=0,_tmp29B.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_def_inc_path);_tmp29B;});void*_tmp26[1];_tmp26[0]=& _tmp28;({struct _fat_ptr _tmp2DA=({const char*_tmp27="Standard include directory: %s\n";_tag_fat(_tmp27,sizeof(char),32U);});Cyc_printf(_tmp2DA,_tag_fat(_tmp26,sizeof(void*),1));});});
exit(0);}
# 176
static int Cyc_is_cyclone_sourcefile(struct _fat_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp29;
if(len <= (unsigned long)4)return 0;
return({struct _fat_ptr _tmp2DB=(struct _fat_ptr)_fat_ptr_plus(s,sizeof(char),(int)(len - (unsigned long)4));Cyc_strcmp(_tmp2DB,({const char*_tmp2A=".cyc";_tag_fat(_tmp2A,sizeof(char),5U);}));})== 0;}
# 192 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2DC=(struct _fat_ptr)s;Cyc_strcmp(_tmp2DC,({const char*_tmp2B="-lxml";_tag_fat(_tmp2B,sizeof(char),6U);}));})== 0)
Cyc_add_ccarg(Cyc_pa_r?({const char*_tmp2C="-lxml_a";_tag_fat(_tmp2C,sizeof(char),8U);}): s);else{
if(({struct _fat_ptr _tmp2DD=(struct _fat_ptr)s;Cyc_strncmp(_tmp2DD,({const char*_tmp2D="-lcyc";_tag_fat(_tmp2D,sizeof(char),6U);}),5U);})== 0)
Cyc_add_ccarg(s);else{
# 199
if(({struct _fat_ptr _tmp2DE=(struct _fat_ptr)s;Cyc_strncmp(_tmp2DE,({const char*_tmp2E="-lpthread";_tag_fat(_tmp2E,sizeof(char),10U);}),9U);})== 0)
Cyc_pthread_r=1;
Cyc_libargs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr*_tmp2DF=({struct _fat_ptr*_tmp2F=_cycalloc(sizeof(*_tmp2F));*_tmp2F=s;_tmp2F;});_tmp30->hd=_tmp2DF;}),_tmp30->tl=Cyc_libargs;_tmp30;});}}}
# 205
static void Cyc_add_marg(struct _fat_ptr s){
Cyc_add_ccarg(s);}
# 209
static void Cyc_set_save_temps (void){
Cyc_save_temps_r=1;
Cyc_add_ccarg(({const char*_tmp31="-save-temps";_tag_fat(_tmp31,sizeof(char),12U);}));}
# 214
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies (void){
Cyc_Flags_stop_after_pass=Cyc_Flags_Cpp;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(({const char*_tmp32="-M";_tag_fat(_tmp32,sizeof(char),3U);}));}
# 221
static struct _fat_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _fat_ptr s){
Cyc_dependencies_target=({struct _fat_ptr*_tmp33=_cycalloc(sizeof(*_tmp33));*_tmp33=s;_tmp33;});}
# 226
static void Cyc_set_stop_after_objectfile (void){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(({const char*_tmp34="-c";_tag_fat(_tmp34,sizeof(char),3U);}));}
# 231
static void Cyc_set_nocppprecomp (void){
Cyc_add_cpp_and_ccarg(({const char*_tmp35="-no-cpp-precomp";_tag_fat(_tmp35,sizeof(char),16U);}));}
# 235
static void Cyc_clear_lineno (void){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 239
static void Cyc_set_elim_se (void){
Cyc_elim_se_r=1;}
# 242
static void Cyc_set_tovc (void){
Cyc_Flags_tovc_r=1;
Cyc_add_ccarg(({const char*_tmp36="-DCYC_ANSI_OUTPUT";_tag_fat(_tmp36,sizeof(char),18U);}));
Cyc_set_elim_se();}
# 247
static void Cyc_set_notoseqc (void){
Cyc_toseqc_r=0;}
# 250
static void Cyc_set_noboundschecks (void){
Cyc_add_ccarg(({const char*_tmp37="-DNO_CYC_BOUNDS_CHECKS";_tag_fat(_tmp37,sizeof(char),23U);}));}
# 253
static void Cyc_set_nonullchecks (void){
Cyc_add_ccarg(({const char*_tmp38="-DNO_CYC_NULL_CHECKS";_tag_fat(_tmp38,sizeof(char),21U);}));}
# 256
static void Cyc_set_nochecks (void){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 262
static void Cyc_set_nocyc (void){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(({const char*_tmp39="-DNO_CYC_PREFIX";_tag_fat(_tmp39,sizeof(char),16U);}));}
# 267
static void Cyc_set_pa (void){
Cyc_pa_r=1;
Cyc_add_ccarg(({const char*_tmp3A="-DCYC_REGION_PROFILE";_tag_fat(_tmp3A,sizeof(char),21U);}));
Cyc_add_cpparg(({const char*_tmp3B="-DCYC_REGION_PROFILE";_tag_fat(_tmp3B,sizeof(char),21U);}));}
# 273
static void Cyc_set_pg (void){
Cyc_pg_r=1;
Cyc_add_ccarg(({const char*_tmp3C="-pg";_tag_fat(_tmp3C,sizeof(char),4U);}));}
# 278
static void Cyc_set_stop_after_asmfile (void){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(({const char*_tmp3D="-S";_tag_fat(_tmp3D,sizeof(char),3U);}));}
# 284
enum Cyc_inputtype{Cyc_DEFAULTINPUT =0U,Cyc_CYCLONEFILE =1U};
# 288
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2E0=(struct _fat_ptr)s;Cyc_strcmp(_tmp2E0,({const char*_tmp3E="cyc";_tag_fat(_tmp3E,sizeof(char),4U);}));})== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
if(({struct _fat_ptr _tmp2E1=(struct _fat_ptr)s;Cyc_strcmp(_tmp2E1,({const char*_tmp3F="none";_tag_fat(_tmp3F,sizeof(char),5U);}));})== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 295
({struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp29C;_tmp29C.tag=0,_tmp29C.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp29C;});void*_tmp40[1];_tmp40[0]=& _tmp42;({struct Cyc___cycFILE*_tmp2E3=Cyc_stderr;struct _fat_ptr _tmp2E2=({const char*_tmp41="Input type '%s' not supported\n";_tag_fat(_tmp41,sizeof(char),31U);});Cyc_fprintf(_tmp2E3,_tmp2E2,_tag_fat(_tmp40,sizeof(void*),1));});});}}}
# 298
struct _fat_ptr Cyc_make_base_filename(struct _fat_ptr s,struct _fat_ptr*output_file){
struct _fat_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);struct _fat_ptr outdir=_tmp43;
struct _fat_ptr _tmp44=Cyc_Filename_chop_extension((struct _fat_ptr)Cyc_Filename_basename(s));struct _fat_ptr base=_tmp44;
# 304
struct _fat_ptr _tmp45=Cyc_strlen((struct _fat_ptr)outdir)> (unsigned long)0?Cyc_Filename_concat((struct _fat_ptr)outdir,(struct _fat_ptr)base): base;struct _fat_ptr basename=_tmp45;
return(struct _fat_ptr)basename;}
# 310
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 312
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _fat_ptr s){
# 315
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
# 319
if((int)Cyc_intype == (int)Cyc_CYCLONEFILE || Cyc_is_cyclone_sourcefile(s)){
# 321
Cyc_cyclone_files=({struct Cyc_List_List*_tmp47=_cycalloc(sizeof(*_tmp47));({struct _fat_ptr*_tmp2E4=({struct _fat_ptr*_tmp46=_cycalloc(sizeof(*_tmp46));*_tmp46=s;_tmp46;});_tmp47->hd=_tmp2E4;}),_tmp47->tl=Cyc_cyclone_files;_tmp47;});{
# 327
struct _fat_ptr _tmp48=Cyc_make_base_filename(s,0);struct _fat_ptr basename=_tmp48;
struct _fat_ptr _tmp49=({struct _fat_ptr _tmp2E5=(struct _fat_ptr)basename;Cyc_strconcat(_tmp2E5,({const char*_tmp4A=".c";_tag_fat(_tmp4A,sizeof(char),3U);}));});struct _fat_ptr cfile=_tmp49;
Cyc_add_ccarg((struct _fat_ptr)cfile);}}else{
# 331
Cyc_add_ccarg(s);}}}
# 336
static int Cyc_assume_gcc_flag=1;struct _tuple13{struct _fat_ptr flag;int optargs;};static char _tmp4B[3U]="-s";static char _tmp4C[3U]="-O";static char _tmp4D[4U]="-O0";static char _tmp4E[4U]="-O2";static char _tmp4F[4U]="-O3";static char _tmp50[21U]="-fomit-frame-pointer";static char _tmp51[13U]="-fno-builtin";static char _tmp52[3U]="-g";static char _tmp53[3U]="-p";static char _tmp54[8U]="-static";
static int Cyc_add_other_flag(struct _fat_ptr s){
static struct _tuple13 known_gcc_flags[10U]={{.flag={_tmp4B,_tmp4B,_tmp4B + 3U},.optargs=0},{.flag={_tmp4C,_tmp4C,_tmp4C + 3U},.optargs=0},{.flag={_tmp4D,_tmp4D,_tmp4D + 4U},.optargs=0},{.flag={_tmp4E,_tmp4E,_tmp4E + 4U},.optargs=0},{.flag={_tmp4F,_tmp4F,_tmp4F + 4U},.optargs=0},{.flag={_tmp50,_tmp50,_tmp50 + 21U},.optargs=0},{.flag={_tmp51,_tmp51,_tmp51 + 13U},.optargs=0},{.flag={_tmp52,_tmp52,_tmp52 + 3U},.optargs=0},{.flag={_tmp53,_tmp53,_tmp53 + 3U},.optargs=0},{.flag={_tmp54,_tmp54,_tmp54 + 8U},.optargs=0}};
# 350
if(Cyc_assume_gcc_flag)
Cyc_add_ccarg(s);else{
# 353
{int i=0;for(0;(unsigned)i < 10U;++ i){
if(!Cyc_strcmp((struct _fat_ptr)(*((struct _tuple13*)_check_known_subscript_notnull(known_gcc_flags,10U,sizeof(struct _tuple13),i))).flag,(struct _fat_ptr)s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(known_gcc_flags[i]).optargs;
break;}}}
# 360
return 0;}
# 362
return 1;}
# 365
static void Cyc_noassume_gcc_flag (void){
Cyc_assume_gcc_flag=0;}
# 369
static void Cyc_remove_file(struct _fat_ptr s){
if(!Cyc_save_temps_r)
remove((const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)));}
# 376
int Cyc_compile_failure=0;
# 378
struct Cyc___cycFILE*Cyc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){{struct Cyc___cycFILE*_tmp58=Cyc_file_open(filename,mode);_npop_handler(0);return _tmp58;};_pop_handler();}else{void*_tmp56=(void*)Cyc_Core_get_exn_thrown();void*_tmp59=_tmp56;
# 381
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0,_tmp29E.f1=(struct _fat_ptr)((struct _fat_ptr)msg_part);_tmp29E;});struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0,_tmp29D.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_tmp29D;});void*_tmp5A[2];_tmp5A[0]=& _tmp5C,_tmp5A[1]=& _tmp5D;({struct Cyc___cycFILE*_tmp2E7=Cyc_stderr;struct _fat_ptr _tmp2E6=({const char*_tmp5B="\nError: couldn't open %s %s\n";_tag_fat(_tmp5B,sizeof(char),29U);});Cyc_fprintf(_tmp2E7,_tmp2E6,_tag_fat(_tmp5A,sizeof(void*),2));});});
Cyc_fflush(Cyc_stderr);
return 0;;}}}struct _tuple14{void*f1;const char*f2;int f3;};
# 388
struct _tuple14*Cyc_uncaught_exn=0;
# 390
extern void CYCALLOCPROFILE_mark(const char*);
# 392
void*Cyc_do_stage(struct _fat_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void*onfail){
# 394
CYCALLOCPROFILE_mark((const char*)_untag_fat_ptr(stage_name,sizeof(char),1U));{
struct Cyc_timeval start;struct Cyc_timeval end;
if(Cyc_v_r)
gettimeofday(& start,0);{
# 399
void*ans;
{struct _handler_cons _tmp5E;_push_handler(& _tmp5E);{int _tmp60=0;if(setjmp(_tmp5E.handler))_tmp60=1;if(!_tmp60){ans=f(env,tds);;_pop_handler();}else{void*_tmp5F=(void*)Cyc_Core_get_exn_thrown();void*_tmp61=_tmp5F;void*_tmp62;_tmp62=_tmp61;{void*x=_tmp62;
# 402
ans=onfail;
if(Cyc_Position_error_p())
goto _LL0;
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp65=({struct Cyc_String_pa_PrintArg_struct _tmp29F;_tmp29F.tag=0,_tmp29F.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_tmp29F;});void*_tmp63[1];_tmp63[0]=& _tmp65;({struct Cyc___cycFILE*_tmp2E9=Cyc_stderr;struct _fat_ptr _tmp2E8=({const char*_tmp64="COMPILER STAGE %s (unexpected exception) \n";_tag_fat(_tmp64,sizeof(char),43U);});Cyc_fprintf(_tmp2E9,_tmp2E8,_tag_fat(_tmp63,sizeof(void*),1));});});
Cyc_uncaught_exn=({struct _tuple14*_tmp66=_cycalloc(sizeof(*_tmp66));_tmp66->f1=x,({const char*_tmp2EB=Cyc_Core_get_exn_filename();_tmp66->f2=_tmp2EB;}),({int _tmp2EA=Cyc_Core_get_exn_lineno();_tmp66->f3=_tmp2EA;});_tmp66;});
goto _LL0;}_LL0:;}}}
# 410
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
({void*_tmp67=0U;({struct Cyc___cycFILE*_tmp2ED=Cyc_stderr;struct _fat_ptr _tmp2EC=({const char*_tmp68="\nCOMPILATION FAILED!\n";_tag_fat(_tmp68,sizeof(char),22U);});Cyc_fprintf(_tmp2ED,_tmp2EC,_tag_fat(_tmp67,sizeof(void*),0));});});
Cyc_fflush(Cyc_stderr);}else{
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0,_tmp2A0.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_tmp2A0;});void*_tmp69[1];_tmp69[0]=& _tmp6B;({struct Cyc___cycFILE*_tmp2EF=Cyc_stderr;struct _fat_ptr _tmp2EE=({const char*_tmp6A="%s completed ";_tag_fat(_tmp6A,sizeof(char),14U);});Cyc_fprintf(_tmp2EF,_tmp2EE,_tag_fat(_tmp69,sizeof(void*),1));});});
gettimeofday(& end,0);
end.tv_sec -=start.tv_sec;
end.tv_usec -=start.tv_usec;
if(end.tv_usec < 0){
end.tv_usec +=1000000;
-- end.tv_sec;}
# 424
({struct Cyc_Int_pa_PrintArg_struct _tmp6E=({struct Cyc_Int_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=1,_tmp2A2.f1=(unsigned long)end.tv_sec;_tmp2A2;});struct Cyc_Int_pa_PrintArg_struct _tmp6F=({struct Cyc_Int_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=1,_tmp2A1.f1=(unsigned long)end.tv_usec;_tmp2A1;});void*_tmp6C[2];_tmp6C[0]=& _tmp6E,_tmp6C[1]=& _tmp6F;({struct Cyc___cycFILE*_tmp2F1=Cyc_stderr;struct _fat_ptr _tmp2F0=({const char*_tmp6D="(%d.%06d secs)\n";_tag_fat(_tmp6D,sizeof(char),16U);});Cyc_fprintf(_tmp2F1,_tmp2F0,_tag_fat(_tmp6C,sizeof(void*),2));});});
Cyc_fflush(Cyc_stderr);}}
# 427
return ans;}}}
# 430
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp70;_push_handler(& _tmp70);{int _tmp72=0;if(setjmp(_tmp70.handler))_tmp72=1;if(!_tmp72){
ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);
# 435
;_pop_handler();}else{void*_tmp71=(void*)Cyc_Core_get_exn_thrown();void*_tmp73=_tmp71;void*_tmp74;if(((struct Cyc_Parse_Exit_exn_struct*)_tmp73)->tag == Cyc_Parse_Exit){
# 438
Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;}else{_tmp74=_tmp73;{void*e=_tmp74;
Cyc_file_close(f);Cyc_Core_rethrow(e);}}_LL0:;}}}
# 441
Cyc_Lex_lex_init(1);
return ans;}}
# 445
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 450
int Cyc_do_adjust(int ignore,struct Cyc_List_List*tds){
Cyc_CurRgn_adjust_all(tds);
return 1;}
# 455
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){
# 457
Cyc_Tc_tc(te,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(te,tds);
return tds;}
# 463
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 465
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 468
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 470
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}
# 474
int Cyc_do_insert_checks(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
Cyc_InsertChecks_insert_checks(tds,tables,Cyc_vcgen_r);
return 1;}struct _tuple15{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 481
struct Cyc_List_List*Cyc_do_interface(struct _tuple15*params,struct Cyc_List_List*tds){
# 483
struct _tuple15*_tmp75=params;void*_tmp78;void*_tmp77;void*_tmp76;_tmp76=_tmp75->f1;_tmp77=_tmp75->f2;_tmp78=_tmp75->f3;{struct Cyc_Tcenv_Tenv*te=_tmp76;struct Cyc___cycFILE*fi=_tmp77;struct Cyc___cycFILE*fo=_tmp78;
struct Cyc_Interface_I*_tmp79=Cyc_Interface_extract(te->ae,tds);struct Cyc_Interface_I*i1=_tmp79;
if(fi == 0)
Cyc_Interface_save(i1,fo);else{
# 488
struct Cyc_Interface_I*_tmp7A=Cyc_Interface_parse(fi);struct Cyc_Interface_I*i0=_tmp7A;
if(!({struct Cyc_Interface_I*_tmp2F5=i0;struct Cyc_Interface_I*_tmp2F4=i1;Cyc_Interface_is_subinterface(_tmp2F5,_tmp2F4,({struct _tuple12*_tmp7D=_cycalloc(sizeof(*_tmp7D));({struct _fat_ptr _tmp2F3=({const char*_tmp7B="written interface";_tag_fat(_tmp7B,sizeof(char),18U);});_tmp7D->f1=_tmp2F3;}),({struct _fat_ptr _tmp2F2=({const char*_tmp7C="maximal interface";_tag_fat(_tmp7C,sizeof(char),18U);});_tmp7D->f2=_tmp2F2;});_tmp7D;}));}))
Cyc_compile_failure=1;else{
# 492
Cyc_Interface_save(i0,fo);}}
# 494
return tds;}}
# 497
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 500
return Cyc_Toc_toc(pops,tds);}
# 502
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveAggrs_remove_aggrs(tds);}
# 505
struct Cyc_List_List*Cyc_do_removelabs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveLabels_remove_unused_labels(tds);}
# 509
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){
return Cyc_Toseqc_toseqc(tds);}
# 512
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Flags_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 517
static struct _fat_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 519
static struct _fat_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 521
static void Cyc_set_cyc_include(struct _fat_ptr f){
Cyc_cyc_include=f;}
# 526
int Cyc_copy_internal_file(struct _fat_ptr file,struct Cyc___cycFILE*out_file){
# 529
if(({char*_tmp2F6=(char*)file.curr;_tmp2F6 == (char*)(_tag_fat(0,0,0)).curr;})){
({void*_tmp7E=0U;({struct Cyc___cycFILE*_tmp2F8=Cyc_stderr;struct _fat_ptr _tmp2F7=({const char*_tmp7F="Internal error: copy_internal_file called with NULL\n";_tag_fat(_tmp7F,sizeof(char),53U);});Cyc_fprintf(_tmp2F8,_tmp2F7,_tag_fat(_tmp7E,sizeof(void*),0));});});
return 1;}{
# 533
struct Cyc___cycFILE*_tmp80=({struct _fat_ptr _tmp2FA=file;struct _fat_ptr _tmp2F9=({const char*_tmp87="r";_tag_fat(_tmp87,sizeof(char),2U);});Cyc_try_file_open(_tmp2FA,_tmp2F9,({const char*_tmp88="internal compiler file";_tag_fat(_tmp88,sizeof(char),23U);}));});struct Cyc___cycFILE*file_f=_tmp80;
if(file_f == 0)return 1;{
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];({{unsigned _tmp2A5=1024U;unsigned i;for(i=0;i < _tmp2A5;++ i){buf[i]='\000';}}0;});
while(n_read == 1024U){
n_read=({struct _fat_ptr _tmp2FB=_tag_fat(buf,sizeof(char),1024U);Cyc_fread(_tmp2FB,1U,1024U,file_f);});
if(n_read != 1024U && !Cyc_feof(file_f)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=0,_tmp2A3.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2A3;});void*_tmp81[1];_tmp81[0]=& _tmp83;({struct Cyc___cycFILE*_tmp2FD=Cyc_stderr;struct _fat_ptr _tmp2FC=({const char*_tmp82="\nError: problem copying %s\n";_tag_fat(_tmp82,sizeof(char),28U);});Cyc_fprintf(_tmp2FD,_tmp2FC,_tag_fat(_tmp81,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
return 1;}
# 546
n_written=({struct _fat_ptr _tmp2FF=_tag_fat(buf,sizeof(char),1024U);unsigned long _tmp2FE=n_read;Cyc_fwrite(_tmp2FF,1U,_tmp2FE,out_file);});
if(n_read != n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp86=({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0,_tmp2A4.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2A4;});void*_tmp84[1];_tmp84[0]=& _tmp86;({struct Cyc___cycFILE*_tmp301=Cyc_stderr;struct _fat_ptr _tmp300=({const char*_tmp85="\nError: problem copying %s\n";_tag_fat(_tmp85,sizeof(char),28U);});Cyc_fprintf(_tmp301,_tmp300,_tag_fat(_tmp84,sizeof(void*),1));});});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 554
Cyc_fclose(file_f);
return 0;}}}
# 560
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles (void){
if(!Cyc_save_c_r)
for(1;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file((struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 566
static void Cyc_find_fail(struct _fat_ptr file){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp303=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0,_tmp2A6.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2A6;});void*_tmp89[1];_tmp89[0]=& _tmp8B;({struct _fat_ptr _tmp302=({const char*_tmp8A="Error: can't find internal compiler file %s\n";_tag_fat(_tmp8A,sizeof(char),45U);});Cyc_aprintf(_tmp302,_tag_fat(_tmp89,sizeof(void*),1));});});_tmp8C->f1=_tmp303;});_tmp8C;}));}
# 571
static struct _fat_ptr Cyc_find_in_arch_path(struct _fat_ptr s){
struct _fat_ptr _tmp8D=Cyc_Specsfile_find_in_arch_path(s);struct _fat_ptr r=_tmp8D;
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}
# 576
static struct _fat_ptr Cyc_find_in_exec_path(struct _fat_ptr s){
struct _fat_ptr _tmp8E=Cyc_Specsfile_find_in_exec_path(s);struct _fat_ptr r=_tmp8E;
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}struct _tuple16{struct Cyc___cycFILE*f1;struct _fat_ptr f2;};
# 582
struct Cyc_List_List*Cyc_do_print(struct _tuple16*env,struct Cyc_List_List*tds){
# 584
struct _tuple16*_tmp8F=env;struct _fat_ptr _tmp91;void*_tmp90;_tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;{struct Cyc___cycFILE*out_file=_tmp90;struct _fat_ptr cfile=_tmp91;
struct Cyc_Absynpp_Params _tmp92=(int)Cyc_Flags_stop_after_pass < (int)Cyc_Flags_Toc?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;struct Cyc_Absynpp_Params params_r=_tmp92;
params_r.expand_typedefs=!Cyc_Flags_noexpand_r;
params_r.to_VC=Cyc_Flags_tovc_r;
params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;
params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;
params_r.print_all_effects=Cyc_print_all_effects_r;
# 596
if((int)Cyc_Flags_stop_after_pass > 6){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Flags_compile_for_boot)
({void*_tmp93=0U;({struct Cyc___cycFILE*_tmp305=out_file;struct _fat_ptr _tmp304=({const char*_tmp94="#include <setjmp.h>\n";_tag_fat(_tmp94,sizeof(char),21U);});Cyc_fprintf(_tmp305,_tmp304,_tag_fat(_tmp93,sizeof(void*),0));});});else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 602
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 604
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& params_r);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 608
Cyc_Absyndump_set_params(& params_r);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 611
Cyc_fflush(out_file);
return tds;}}
# 616
static struct Cyc_List_List*Cyc_split_by_char(struct _fat_ptr s,char c){
if(({char*_tmp306=(char*)s.curr;_tmp306 == (char*)(_tag_fat(0,0,0)).curr;}))return 0;{
struct Cyc_List_List*_tmp95=0;struct Cyc_List_List*result=_tmp95;
unsigned long _tmp96=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp96;
# 621
while(len > 0U){
struct _fat_ptr _tmp97=Cyc_strchr(s,c);struct _fat_ptr end=_tmp97;
if(({char*_tmp307=(char*)end.curr;_tmp307 == (char*)(_tag_fat(0,0,0)).curr;})){
result=({struct Cyc_List_List*_tmp99=_cycalloc(sizeof(*_tmp99));({struct _fat_ptr*_tmp308=({struct _fat_ptr*_tmp98=_cycalloc(sizeof(*_tmp98));*_tmp98=s;_tmp98;});_tmp99->hd=_tmp308;}),_tmp99->tl=result;_tmp99;});
break;}else{
# 627
result=({struct Cyc_List_List*_tmp9B=_cycalloc(sizeof(*_tmp9B));({struct _fat_ptr*_tmp30A=({struct _fat_ptr*_tmp9A=_cycalloc(sizeof(*_tmp9A));({struct _fat_ptr _tmp309=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char)));*_tmp9A=_tmp309;});_tmp9A;});_tmp9B->hd=_tmp30A;}),_tmp9B->tl=result;_tmp9B;});
len -=(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char));
s=_fat_ptr_plus(end,sizeof(char),1);}}
# 632
return((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(result);}}
# 637
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){
# 639
struct Cyc_List_List*_tmp9C=0;struct Cyc_List_List*l=_tmp9C;
for(1;dirs != 0;dirs=dirs->tl){
l=({struct Cyc_List_List*_tmp9E=_cycalloc(sizeof(*_tmp9E));({struct _fat_ptr*_tmp30C=({struct _fat_ptr*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct _fat_ptr _tmp30B=(struct _fat_ptr)Cyc_Filename_concat(*((struct _fat_ptr*)dirs->hd),subdir);*_tmp9D=_tmp30B;});_tmp9D;});_tmp9E->hd=_tmp30C;}),_tmp9E->tl=l;_tmp9E;});}
l=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_imp_rev)(l);
return l;}
# 647
static int Cyc_is_other_special(char c){
char _tmp9F=c;switch((int)_tmp9F){case 92:
 goto _LL4;case 34: _LL4:
 goto _LL6;case 59: _LL6:
 goto _LL8;case 38: _LL8:
 goto _LLA;case 40: _LLA:
 goto _LLC;case 41: _LLC:
 goto _LLE;case 124: _LLE:
 goto _LL10;case 94: _LL10:
 goto _LL12;case 60: _LL12:
 goto _LL14;case 62: _LL14:
 goto _LL16;case 10: _LL16:
# 662
 goto _LL18;case 9: _LL18:
 return 1;default:
 return 0;};}
# 669
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmpA0=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmpA0;
# 673
int _tmpA1=0;int single_quotes=_tmpA1;
int _tmpA2=0;int other_special=_tmpA2;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmpA3=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmpA3;
if((int)c == (int)'\'')++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 682
if(single_quotes == 0 && other_special == 0)
return s;
# 686
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmpA4[3];({struct _fat_ptr*_tmp311=({struct _fat_ptr*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct _fat_ptr _tmp310=({const char*_tmpA5="'";_tag_fat(_tmpA5,sizeof(char),2U);});*_tmpA6=_tmp310;});_tmpA6;});_tmpA4[0]=_tmp311;}),({struct _fat_ptr*_tmp30F=({struct _fat_ptr*_tmpA7=_cycalloc(sizeof(*_tmpA7));*_tmpA7=(struct _fat_ptr)s;_tmpA7;});_tmpA4[1]=_tmp30F;}),({struct _fat_ptr*_tmp30E=({struct _fat_ptr*_tmpA9=_cycalloc(sizeof(*_tmpA9));({struct _fat_ptr _tmp30D=({const char*_tmpA8="'";_tag_fat(_tmpA8,sizeof(char),2U);});*_tmpA9=_tmp30D;});_tmpA9;});_tmpA4[2]=_tmp30E;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpA4,sizeof(struct _fat_ptr*),3));}));{
# 690
unsigned long _tmpAA=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmpAA;
struct _fat_ptr s2=({unsigned _tmpB5=(len2 + (unsigned long)1)+ 1U;char*_tmpB4=_cycalloc_atomic(_check_times(_tmpB5,sizeof(char)));({{unsigned _tmp2A7=len2 + (unsigned long)1;unsigned i;for(i=0;i < _tmp2A7;++ i){_tmpB4[i]='\000';}_tmpB4[_tmp2A7]=0;}0;});_tag_fat(_tmpB4,sizeof(char),_tmpB5);});
int _tmpAB=0;int i=_tmpAB;
int _tmpAC=0;int j=_tmpAC;
for(1;(unsigned long)i < len;++ i){
char _tmpAD=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmpAD;
if((int)c == 39 || Cyc_is_other_special(c))
({struct _fat_ptr _tmpAE=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmpAF=*((char*)_check_fat_subscript(_tmpAE,sizeof(char),0U));char _tmpB0='\\';if(_get_fat_size(_tmpAE,sizeof(char))== 1U &&(_tmpAF == 0 && _tmpB0 != 0))_throw_arraybounds();*((char*)_tmpAE.curr)=_tmpB0;});
({struct _fat_ptr _tmpB1=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmpB2=*((char*)_check_fat_subscript(_tmpB1,sizeof(char),0U));char _tmpB3=c;if(_get_fat_size(_tmpB1,sizeof(char))== 1U &&(_tmpB2 == 0 && _tmpB3 != 0))_throw_arraybounds();*((char*)_tmpB1.curr)=_tmpB3;});}
# 700
return(struct _fat_ptr)s2;}}
# 702
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct _fat_ptr _tmp312=Cyc_sh_escape_string(*sp);*_tmpB6=_tmp312;});_tmpB6;});}
# 706
static void Cyc_process_file(struct _fat_ptr filename){
# 708
struct _fat_ptr _tmpB7=Cyc_make_base_filename(filename,Cyc_output_file);struct _fat_ptr basename=_tmpB7;
struct _fat_ptr _tmpB8=({struct _fat_ptr _tmp313=(struct _fat_ptr)basename;Cyc_strconcat(_tmp313,({const char*_tmp11D=".cyp";_tag_fat(_tmp11D,sizeof(char),5U);}));});struct _fat_ptr preprocfile=_tmpB8;
struct _fat_ptr _tmpB9=({char*_tmp315=(char*)Cyc_specified_interface.curr;_tmp315 != (char*)(_tag_fat(0,0,0)).curr;})?Cyc_specified_interface:(struct _fat_ptr)({
struct _fat_ptr _tmp314=(struct _fat_ptr)basename;Cyc_strconcat(_tmp314,({const char*_tmp11C=".cyci";_tag_fat(_tmp11C,sizeof(char),6U);}));});
# 710
struct _fat_ptr interfacefile=_tmpB9;
# 712
struct _fat_ptr _tmpBA=({struct _fat_ptr _tmp316=(struct _fat_ptr)basename;Cyc_strconcat(_tmp316,({const char*_tmp11B=".cycio";_tag_fat(_tmp11B,sizeof(char),7U);}));});struct _fat_ptr interfaceobjfile=_tmpBA;
# 716
struct _fat_ptr _tmpBB=({struct _fat_ptr _tmp317=(struct _fat_ptr)Cyc_make_base_filename(filename,0);Cyc_strconcat(_tmp317,({const char*_tmp11A=".c";_tag_fat(_tmp11A,sizeof(char),3U);}));});struct _fat_ptr cfile=_tmpBB;
struct _fat_ptr*_tmpBC=({struct _fat_ptr*_tmp119=_cycalloc(sizeof(*_tmp119));*_tmp119=preprocfile;_tmp119;});struct _fat_ptr*preprocfileptr=_tmpBC;
# 719
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpBF=({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0,_tmp2A8.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_tmp2A8;});void*_tmpBD[1];_tmpBD[0]=& _tmpBF;({struct Cyc___cycFILE*_tmp319=Cyc_stderr;struct _fat_ptr _tmp318=({const char*_tmpBE="Compiling %s\n";_tag_fat(_tmpBE,sizeof(char),14U);});Cyc_fprintf(_tmp319,_tmp318,_tag_fat(_tmpBD,sizeof(void*),1));});});{
# 722
struct Cyc___cycFILE*f0=({struct _fat_ptr _tmp31B=filename;struct _fat_ptr _tmp31A=({const char*_tmp117="r";_tag_fat(_tmp117,sizeof(char),2U);});Cyc_try_file_open(_tmp31B,_tmp31A,({const char*_tmp118="input file";_tag_fat(_tmp118,sizeof(char),11U);}));});
if(Cyc_compile_failure || !((unsigned)f0))
return;
Cyc_fclose(f0);{
# 729
struct _fat_ptr _tmpC0=({
struct Cyc_List_List*_tmp31F=({struct Cyc_List_List*_tmp115=_cycalloc(sizeof(*_tmp115));({struct _fat_ptr*_tmp31E=({struct _fat_ptr*_tmp114=_cycalloc(sizeof(*_tmp114));({struct _fat_ptr _tmp31D=(struct _fat_ptr)({const char*_tmp113="";_tag_fat(_tmp113,sizeof(char),1U);});*_tmp114=_tmp31D;});_tmp114;});_tmp115->hd=_tmp31E;}),({
struct Cyc_List_List*_tmp31C=((struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_cppargs));_tmp115->tl=_tmp31C;});_tmp115;});
# 730
Cyc_str_sepstr(_tmp31F,({const char*_tmp116=" ";_tag_fat(_tmp116,sizeof(char),2U);}));});
# 729
struct _fat_ptr cppargs_string=_tmpC0;
# 739
struct Cyc_List_List*_tmpC1=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);struct Cyc_List_List*stdinc_dirs=_tmpC1;
stdinc_dirs=({struct Cyc_List_List*_tmp320=stdinc_dirs;Cyc_add_subdir(_tmp320,({const char*_tmpC2="include";_tag_fat(_tmpC2,sizeof(char),8U);}));});
if(Cyc_strlen((struct _fat_ptr)Cyc_def_inc_path)> (unsigned long)0)
stdinc_dirs=({struct Cyc_List_List*_tmpC4=_cycalloc(sizeof(*_tmpC4));({struct _fat_ptr*_tmp321=({struct _fat_ptr*_tmpC3=_cycalloc(sizeof(*_tmpC3));*_tmpC3=Cyc_def_inc_path;_tmpC3;});_tmpC4->hd=_tmp321;}),_tmpC4->tl=stdinc_dirs;_tmpC4;});{
char*_tmpC5=getenv("CYCLONE_INCLUDE_PATH");char*cyclone_include_path=_tmpC5;
if(cyclone_include_path != 0)
stdinc_dirs=({struct Cyc_List_List*_tmp322=Cyc_split_by_char(({char*_tmpC6=cyclone_include_path;_tag_fat(_tmpC6,sizeof(char),_get_zero_arr_size_char((void*)_tmpC6,1U));}),':');((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp322,stdinc_dirs);});{
struct _fat_ptr stdinc_string=(struct _fat_ptr)({
struct Cyc_List_List*_tmp326=({struct Cyc_List_List*_tmp111=_cycalloc(sizeof(*_tmp111));({struct _fat_ptr*_tmp325=({struct _fat_ptr*_tmp110=_cycalloc(sizeof(*_tmp110));({struct _fat_ptr _tmp324=(struct _fat_ptr)({const char*_tmp10F="";_tag_fat(_tmp10F,sizeof(char),1U);});*_tmp110=_tmp324;});_tmp110;});_tmp111->hd=_tmp325;}),({
struct Cyc_List_List*_tmp323=((struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map)(Cyc_sh_escape_stringptr,stdinc_dirs);_tmp111->tl=_tmp323;});_tmp111;});
# 747
Cyc_str_sepstr(_tmp326,({const char*_tmp112=" -I";_tag_fat(_tmp112,sizeof(char),4U);}));});
# 750
struct _fat_ptr ofile_string;
if((int)Cyc_Flags_stop_after_pass <= (int)Cyc_Flags_Cpp)
ofile_string=Cyc_output_file == 0?({const char*_tmpC7="";_tag_fat(_tmpC7,sizeof(char),1U);}):(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpCA=({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0,_tmp2A9.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_output_file)));_tmp2A9;});void*_tmpC8[1];_tmpC8[0]=& _tmpCA;({struct _fat_ptr _tmp327=({const char*_tmpC9=" > %s";_tag_fat(_tmpC9,sizeof(char),6U);});Cyc_aprintf(_tmp327,_tag_fat(_tmpC8,sizeof(void*),1));});});else{
# 754
ofile_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpCD=({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0,({struct _fat_ptr _tmp328=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string((struct _fat_ptr)preprocfile));_tmp2AA.f1=_tmp328;});_tmp2AA;});void*_tmpCB[1];_tmpCB[0]=& _tmpCD;({struct _fat_ptr _tmp329=({const char*_tmpCC=" > %s";_tag_fat(_tmpCC,sizeof(char),6U);});Cyc_aprintf(_tmp329,_tag_fat(_tmpCB,sizeof(void*),1));});});}{
# 756
struct _fat_ptr fixup_string;
if(Cyc_produce_dependencies){
# 760
if(Cyc_dependencies_target == 0)
# 764
fixup_string=({const char*_tmpCE=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_fat(_tmpCE,sizeof(char),35U);});else{
# 769
fixup_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD1=({struct Cyc_String_pa_PrintArg_struct _tmp2AB;_tmp2AB.tag=0,_tmp2AB.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_dependencies_target)));_tmp2AB;});void*_tmpCF[1];_tmpCF[0]=& _tmpD1;({struct _fat_ptr _tmp32A=({const char*_tmpD0=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_fat(_tmpD0,sizeof(char),29U);});Cyc_aprintf(_tmp32A,_tag_fat(_tmpCF,sizeof(void*),1));});});}}else{
# 772
fixup_string=({const char*_tmpD2="";_tag_fat(_tmpD2,sizeof(char),1U);});}{
# 774
struct _fat_ptr _tmpD3=({struct Cyc_String_pa_PrintArg_struct _tmp109=({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0,_tmp2B6.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cpp);_tmp2B6;});struct Cyc_String_pa_PrintArg_struct _tmp10A=({struct Cyc_String_pa_PrintArg_struct _tmp2B5;_tmp2B5.tag=0,_tmp2B5.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp2B5;});struct Cyc_String_pa_PrintArg_struct _tmp10B=({struct Cyc_String_pa_PrintArg_struct _tmp2B4;_tmp2B4.tag=0,_tmp2B4.f1=(struct _fat_ptr)((struct _fat_ptr)stdinc_string);_tmp2B4;});struct Cyc_String_pa_PrintArg_struct _tmp10C=({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0,({
# 776
struct _fat_ptr _tmp32B=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(filename));_tmp2B3.f1=_tmp32B;});_tmp2B3;});struct Cyc_String_pa_PrintArg_struct _tmp10D=({struct Cyc_String_pa_PrintArg_struct _tmp2B2;_tmp2B2.tag=0,_tmp2B2.f1=(struct _fat_ptr)((struct _fat_ptr)fixup_string);_tmp2B2;});struct Cyc_String_pa_PrintArg_struct _tmp10E=({struct Cyc_String_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=0,_tmp2B1.f1=(struct _fat_ptr)((struct _fat_ptr)ofile_string);_tmp2B1;});void*_tmp107[6];_tmp107[0]=& _tmp109,_tmp107[1]=& _tmp10A,_tmp107[2]=& _tmp10B,_tmp107[3]=& _tmp10C,_tmp107[4]=& _tmp10D,_tmp107[5]=& _tmp10E;({struct _fat_ptr _tmp32C=({const char*_tmp108="%s %s%s %s%s%s";_tag_fat(_tmp108,sizeof(char),15U);});Cyc_aprintf(_tmp32C,_tag_fat(_tmp107,sizeof(void*),6));});});
# 774
struct _fat_ptr cmd=_tmpD3;
# 778
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0,_tmp2AC.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp2AC;});void*_tmpD4[1];_tmpD4[0]=& _tmpD6;({struct Cyc___cycFILE*_tmp32E=Cyc_stderr;struct _fat_ptr _tmp32D=({const char*_tmpD5="%s\n";_tag_fat(_tmpD5,sizeof(char),4U);});Cyc_fprintf(_tmp32E,_tmp32D,_tag_fat(_tmpD4,sizeof(void*),1));});});
if(system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)))!= 0){
Cyc_compile_failure=1;
({void*_tmpD7=0U;({struct Cyc___cycFILE*_tmp330=Cyc_stderr;struct _fat_ptr _tmp32F=({const char*_tmpD8="\nError: preprocessing\n";_tag_fat(_tmpD8,sizeof(char),23U);});Cyc_fprintf(_tmp330,_tmp32F,_tag_fat(_tmpD7,sizeof(void*),0));});});
if((int)Cyc_Flags_stop_after_pass > (int)Cyc_Flags_Cpp)Cyc_remove_file((struct _fat_ptr)preprocfile);
return;}
# 785
if((int)Cyc_Flags_stop_after_pass <= 0)return;
# 788
Cyc_Position_reset_position((struct _fat_ptr)preprocfile);{
struct Cyc___cycFILE*in_file=({struct _fat_ptr _tmp332=(struct _fat_ptr)preprocfile;struct _fat_ptr _tmp331=({const char*_tmp105="r";_tag_fat(_tmp105,sizeof(char),2U);});Cyc_try_file_open(_tmp332,_tmp331,({const char*_tmp106="file";_tag_fat(_tmp106,sizeof(char),5U);}));});
# 791
struct Cyc_List_List*tds=0;
struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmpD9=0;struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables=_tmpD9;
struct Cyc_Tcenv_Tenv*_tmpDA=Cyc_Tcenv_tc_init();struct Cyc_Tcenv_Tenv*te=_tmpDA;
# 796
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= (int)Cyc_Flags_Parsing)
tds=({struct _fat_ptr _tmp335=({const char*_tmpDB="parsing";_tag_fat(_tmpDB,sizeof(char),8U);});struct Cyc_List_List*_tmp334=tds;struct Cyc___cycFILE*_tmp333=(struct Cyc___cycFILE*)_check_null(in_file);((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*,struct Cyc_List_List*))Cyc_do_stage)(_tmp335,_tmp334,Cyc_do_parse,_tmp333,tds);});
# 800
if(!Cyc_compile_failure && Cyc_Flags_porting_c_code){
Cyc_Port_port(tds);
Cyc_remove_file((struct _fat_ptr)preprocfile);
return;}
# 807
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= (int)Cyc_Flags_Binding)
({struct _fat_ptr _tmp336=({const char*_tmpDC="binding";_tag_fat(_tmpDC,sizeof(char),8U);});((int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage)(_tmp336,tds,Cyc_do_binding,1,1);});
# 811
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 3)
({struct _fat_ptr _tmp337=({const char*_tmpDD="current region resolution";_tag_fat(_tmpDD,sizeof(char),26U);});((int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(int,struct Cyc_List_List*),int,int))Cyc_do_stage)(_tmp337,tds,Cyc_do_adjust,1,1);});
# 815
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= (int)Cyc_Flags_TypeChecking)
tds=({struct _fat_ptr _tmp33A=({const char*_tmpDE="type checking";_tag_fat(_tmpDE,sizeof(char),14U);});struct Cyc_List_List*_tmp339=tds;struct Cyc_Tcenv_Tenv*_tmp338=te;((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*))Cyc_do_stage)(_tmp33A,_tmp339,Cyc_do_typecheck,_tmp338,tds);});
# 819
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 5)
jump_tables=({struct _fat_ptr _tmp33B=({const char*_tmpDF="jump checking";_tag_fat(_tmpDF,sizeof(char),14U);});((struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(int,struct Cyc_List_List*),int,struct Cyc_JumpAnalysis_Jump_Anal_Result*))Cyc_do_stage)(_tmp33B,tds,Cyc_do_jumpanalysis,1,0);});
# 823
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 6)
tds=({struct _fat_ptr _tmp33E=({const char*_tmpE0="control-flow checking";_tag_fat(_tmpE0,sizeof(char),22U);});struct Cyc_List_List*_tmp33D=tds;struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp33C=(struct Cyc_JumpAnalysis_Jump_Anal_Result*)_check_null(jump_tables);((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*))Cyc_do_stage)(_tmp33E,_tmp33D,Cyc_do_cfcheck,_tmp33C,tds);});
# 827
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 8)
({struct _fat_ptr _tmp340=({const char*_tmpE1="check insertion";_tag_fat(_tmpE1,sizeof(char),16U);});struct Cyc_List_List*_tmp33F=tds;((int(*)(struct _fat_ptr,struct Cyc_List_List*,int(*)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*,int))Cyc_do_stage)(_tmp340,_tmp33F,Cyc_do_insert_checks,(struct Cyc_JumpAnalysis_Jump_Anal_Result*)_check_null(jump_tables),1);});
# 831
if((!Cyc_compile_failure && Cyc_generate_interface_r)&&(int)Cyc_Flags_stop_after_pass >= 9){
# 833
struct Cyc___cycFILE*inter_file=({struct _fat_ptr _tmp342=({const char*_tmpE6=(const char*)_check_null(_untag_fat_ptr(interfacefile,sizeof(char),1U));_tag_fat(_tmpE6,sizeof(char),_get_zero_arr_size_char((void*)_tmpE6,1U));});struct _fat_ptr _tmp341=({const char*_tmpE7="w";_tag_fat(_tmpE7,sizeof(char),2U);});Cyc_try_file_open(_tmp342,_tmp341,({const char*_tmpE8="interface file";_tag_fat(_tmpE8,sizeof(char),15U);}));});
if(inter_file == 0)
Cyc_compile_failure=1;else{
# 837
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 844
if(!Cyc_compile_failure && Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_untag_fat_ptr(interfacefile,sizeof(char),1U),"r");
struct Cyc___cycFILE*inter_objfile=({struct _fat_ptr _tmp344=(struct _fat_ptr)interfaceobjfile;struct _fat_ptr _tmp343=({const char*_tmpE4="w";_tag_fat(_tmpE4,sizeof(char),2U);});Cyc_try_file_open(_tmp344,_tmp343,({const char*_tmpE5="interface object file";_tag_fat(_tmpE5,sizeof(char),22U);}));});
if(inter_objfile == 0)
Cyc_compile_failure=1;else{
# 850
Cyc_Position_reset_position(interfacefile);{
struct _tuple15 _tmpE2=({struct _tuple15 _tmp2AD;_tmp2AD.f1=te,_tmp2AD.f2=inter_file,_tmp2AD.f3=inter_objfile;_tmp2AD;});struct _tuple15 int_env=_tmpE2;
tds=({struct _fat_ptr _tmp346=({const char*_tmpE3="interface checking";_tag_fat(_tmpE3,sizeof(char),19U);});struct Cyc_List_List*_tmp345=tds;((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple15*,struct Cyc_List_List*),struct _tuple15*,struct Cyc_List_List*))Cyc_do_stage)(_tmp346,_tmp345,Cyc_do_interface,& int_env,tds);});
if(inter_file != 0)
Cyc_file_close(inter_file);
Cyc_file_close(inter_objfile);}}}}
# 861
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
tds=({struct _fat_ptr _tmp349=({const char*_tmpE9="translation to C";_tag_fat(_tmpE9,sizeof(char),17U);});struct Cyc_List_List*_tmp348=tds;struct Cyc_Hashtable_Table*_tmp347=((struct Cyc_JumpAnalysis_Jump_Anal_Result*)_check_null(jump_tables))->pop_tables;((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*,struct Cyc_List_List*))Cyc_do_stage)(_tmp349,_tmp348,Cyc_do_translate,_tmp347,tds);});
# 865
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 10)
tds=({struct _fat_ptr _tmp34B=({const char*_tmpEA="aggregate removal";_tag_fat(_tmpEA,sizeof(char),18U);});struct Cyc_List_List*_tmp34A=tds;((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage)(_tmp34B,_tmp34A,Cyc_do_removeaggrs,1,tds);});
# 868
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= (int)Cyc_Flags_LabelRemoval)
tds=({struct _fat_ptr _tmp34D=({const char*_tmpEB="unused label removal";_tag_fat(_tmpEB,sizeof(char),21U);});struct Cyc_List_List*_tmp34C=tds;((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage)(_tmp34D,_tmp34C,Cyc_do_removelabs,1,tds);});
# 871
if((!Cyc_compile_failure && Cyc_toseqc_r)&&(int)Cyc_Flags_stop_after_pass >= (int)Cyc_Flags_EvalOrder)
tds=({struct _fat_ptr _tmp34F=({const char*_tmpEC="post-pass to L-to-R evaluation order";_tag_fat(_tmpEC,sizeof(char),37U);});struct Cyc_List_List*_tmp34E=tds;((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage)(_tmp34F,_tmp34E,Cyc_do_toseqc,1,tds);});
# 875
if(!Cyc_compile_failure &&(int)Cyc_Flags_stop_after_pass >= 9)
Cyc_Toc_finish();
# 879
if(!Cyc_compile_failure &&(Cyc_Flags_tovc_r || Cyc_elim_se_r))
tds=({struct _fat_ptr _tmp351=({const char*_tmpED="post-pass to VC";_tag_fat(_tmpED,sizeof(char),16U);});struct Cyc_List_List*_tmp350=tds;((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(int,struct Cyc_List_List*),int,struct Cyc_List_List*))Cyc_do_stage)(_tmp351,_tmp350,Cyc_do_tovc,1,tds);});
# 882
if(!Cyc_compile_failure)
Cyc_Warn_flush_warnings();
Cyc_remove_file((struct _fat_ptr)preprocfile);
if(Cyc_uncaught_exn != 0){
struct _tuple14*_tmpEE=(struct _tuple14*)_check_null(Cyc_uncaught_exn);struct _tuple14*_stmttmp0=_tmpEE;struct _tuple14*_tmpEF=_stmttmp0;int _tmpF2;const char*_tmpF1;void*_tmpF0;_tmpF0=_tmpEF->f1;_tmpF1=_tmpEF->f2;_tmpF2=_tmpEF->f3;{void*e=_tmpF0;const char*fn=_tmpF1;int line=_tmpF2;
({struct Cyc_String_pa_PrintArg_struct _tmpF5=({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0,({
struct _fat_ptr _tmp352=(struct _fat_ptr)({const char*_tmpF9=Cyc_Core_get_exn_name(e);_tag_fat(_tmpF9,sizeof(char),_get_zero_arr_size_char((void*)_tmpF9,1U));});_tmp2B0.f1=_tmp352;});_tmp2B0;});struct Cyc_String_pa_PrintArg_struct _tmpF6=({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0,({struct _fat_ptr _tmp353=(struct _fat_ptr)({const char*_tmpF8=fn;_tag_fat(_tmpF8,sizeof(char),_get_zero_arr_size_char((void*)_tmpF8,1U));});_tmp2AF.f1=_tmp353;});_tmp2AF;});struct Cyc_Int_pa_PrintArg_struct _tmpF7=({struct Cyc_Int_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=1,_tmp2AE.f1=(unsigned long)line;_tmp2AE;});void*_tmpF3[3];_tmpF3[0]=& _tmpF5,_tmpF3[1]=& _tmpF6,_tmpF3[2]=& _tmpF7;({struct Cyc___cycFILE*_tmp355=Cyc_stderr;struct _fat_ptr _tmp354=({const char*_tmpF4="uncaught exception: %s from %s, line %d\n";_tag_fat(_tmpF4,sizeof(char),41U);});Cyc_fprintf(_tmp355,_tmp354,_tag_fat(_tmpF3,sizeof(void*),3));});});
Cyc_fflush(Cyc_stderr);
return;}}{
# 894
struct Cyc___cycFILE*out_file;
if((int)Cyc_Flags_stop_after_pass <= 6){
if(Cyc_output_file != 0)
out_file=({struct _fat_ptr _tmp357=*((struct _fat_ptr*)_check_null(Cyc_output_file));struct _fat_ptr _tmp356=({const char*_tmpFA="w";_tag_fat(_tmpFA,sizeof(char),2U);});Cyc_try_file_open(_tmp357,_tmp356,({const char*_tmpFB="output file";_tag_fat(_tmpFB,sizeof(char),12U);}));});else{
# 899
out_file=Cyc_stdout;}}else{
if((int)Cyc_Flags_stop_after_pass < (int)Cyc_Flags_CCompiler && Cyc_output_file != 0)
out_file=({struct _fat_ptr _tmp359=*((struct _fat_ptr*)_check_null(Cyc_output_file));struct _fat_ptr _tmp358=({const char*_tmpFC="w";_tag_fat(_tmpFC,sizeof(char),2U);});Cyc_try_file_open(_tmp359,_tmp358,({const char*_tmpFD="output file";_tag_fat(_tmpFD,sizeof(char),12U);}));});else{
# 903
out_file=({struct _fat_ptr _tmp35B=(struct _fat_ptr)cfile;struct _fat_ptr _tmp35A=({const char*_tmpFE="w";_tag_fat(_tmpFE,sizeof(char),2U);});Cyc_try_file_open(_tmp35B,_tmp35A,({const char*_tmpFF="output file";_tag_fat(_tmpFF,sizeof(char),12U);}));});}}
# 905
if(Cyc_compile_failure || !((unsigned)out_file))return;
# 907
if(!Cyc_noprint_r){
struct _tuple16*_tmp100=({struct _tuple16*_tmp104=_cycalloc(sizeof(*_tmp104));_tmp104->f1=out_file,_tmp104->f2=cfile;_tmp104;});struct _tuple16*env=_tmp100;
tds=({struct _fat_ptr _tmp35E=({const char*_tmp101="printing";_tag_fat(_tmp101,sizeof(char),9U);});struct Cyc_List_List*_tmp35D=tds;struct _tuple16*_tmp35C=env;((struct Cyc_List_List*(*)(struct _fat_ptr,struct Cyc_List_List*,struct Cyc_List_List*(*)(struct _tuple16*,struct Cyc_List_List*),struct _tuple16*,struct Cyc_List_List*))Cyc_do_stage)(_tmp35E,_tmp35D,Cyc_do_print,_tmp35C,tds);});
Cyc_file_close(out_file);
Cyc_cfiles=({struct Cyc_List_List*_tmp103=_cycalloc(sizeof(*_tmp103));({struct _fat_ptr*_tmp360=({struct _fat_ptr*_tmp102=_cycalloc(sizeof(*_tmp102));({struct _fat_ptr _tmp35F=Cyc_strdup((struct _fat_ptr)cfile);*_tmp102=_tmp35F;});_tmp102;});_tmp103->hd=_tmp360;}),_tmp103->tl=Cyc_cfiles;_tmp103;});}}}}}}}}}}static char _tmp11E[8U]="<final>";
# 915
static struct _fat_ptr Cyc_final_str={_tmp11E,_tmp11E,_tmp11E + 8U};
static struct _fat_ptr*Cyc_final_strptr=& Cyc_final_str;
# 918
static struct Cyc_Interface_I*Cyc_read_cycio(struct _fat_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
struct _fat_ptr basename;
{struct _handler_cons _tmp11F;_push_handler(& _tmp11F);{int _tmp121=0;if(setjmp(_tmp11F.handler))_tmp121=1;if(!_tmp121){basename=(struct _fat_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp120=(void*)Cyc_Core_get_exn_thrown();void*_tmp122=_tmp120;void*_tmp123;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp122)->tag == Cyc_Core_Invalid_argument){
basename=*n;goto _LL0;}else{_tmp123=_tmp122;{void*exn=_tmp123;(int)_rethrow(exn);}}_LL0:;}}}{
# 925
struct _fat_ptr _tmp124=({struct _fat_ptr _tmp361=(struct _fat_ptr)basename;Cyc_strconcat(_tmp361,({const char*_tmp129=".cycio";_tag_fat(_tmp129,sizeof(char),7U);}));});struct _fat_ptr nf=_tmp124;
struct Cyc___cycFILE*_tmp125=({struct _fat_ptr _tmp363=(struct _fat_ptr)nf;struct _fat_ptr _tmp362=({const char*_tmp127="rb";_tag_fat(_tmp127,sizeof(char),3U);});Cyc_try_file_open(_tmp363,_tmp362,({const char*_tmp128="interface object file";_tag_fat(_tmp128,sizeof(char),22U);}));});struct Cyc___cycFILE*f=_tmp125;
if(f == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 932
Cyc_Position_reset_position((struct _fat_ptr)nf);{
struct Cyc_Interface_I*_tmp126=Cyc_Interface_load(f);struct Cyc_Interface_I*i=_tmp126;
Cyc_file_close(f);
return i;}}}}
# 938
static int Cyc_is_cfile(struct _fat_ptr*n){
return(int)*((const char*)_check_fat_subscript(*n,sizeof(char),0))!= (int)'-';}
# 942
extern void GC_blacklist_warn_clear (void);struct _tuple17{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
# 952
void Cyc_print_options (void);
# 960
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3,& Cyc_v_r};static char _tmp12A[3U]="-v";static char _tmp12B[1U]="";static char _tmp12C[35U]="print compilation stages verbosely";struct _tuple17 Cyc_opt1_tuple={{_tmp12A,_tmp12A,_tmp12A + 3U},0,{_tmp12B,_tmp12B,_tmp12B + 1U},(void*)& Cyc_opt1_spec,{_tmp12C,_tmp12C,_tmp12C + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 962
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0,Cyc_print_version};static char _tmp12D[10U]="--version";static char _tmp12E[1U]="";static char _tmp12F[35U]="Print version information and exit";struct _tuple17 Cyc_opt2_tuple={{_tmp12D,_tmp12D,_tmp12D + 10U},0,{_tmp12E,_tmp12E,_tmp12E + 1U},(void*)& Cyc_opt2_spec,{_tmp12F,_tmp12F,_tmp12F + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 965
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5,Cyc_set_output_file};static char _tmp130[3U]="-o";static char _tmp131[8U]=" <file>";static char _tmp132[35U]="Set the output file name to <file>";struct _tuple17 Cyc_opt3_tuple={{_tmp130,_tmp130,_tmp130 + 3U},0,{_tmp131,_tmp131,_tmp131 + 8U},(void*)& Cyc_opt3_spec,{_tmp132,_tmp132,_tmp132 + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 968
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1,Cyc_add_cpparg};static char _tmp133[3U]="-D";static char _tmp134[17U]="<name>[=<value>]";static char _tmp135[32U]="Pass definition to preprocessor";struct _tuple17 Cyc_opt4_tuple={{_tmp133,_tmp133,_tmp133 + 3U},1,{_tmp134,_tmp134,_tmp134 + 17U},(void*)& Cyc_opt4_spec,{_tmp135,_tmp135,_tmp135 + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 971
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1,Cyc_Specsfile_add_cyclone_exec_path};static char _tmp136[3U]="-B";static char _tmp137[7U]="<file>";static char _tmp138[60U]="Add to the list of directories to search for compiler files";struct _tuple17 Cyc_opt5_tuple={{_tmp136,_tmp136,_tmp136 + 3U},1,{_tmp137,_tmp137,_tmp137 + 7U},(void*)& Cyc_opt5_spec,{_tmp138,_tmp138,_tmp138 + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 974
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1,Cyc_add_cpparg};static char _tmp139[3U]="-I";static char _tmp13A[6U]="<dir>";static char _tmp13B[59U]="Add to the list of directories to search for include files";struct _tuple17 Cyc_opt6_tuple={{_tmp139,_tmp139,_tmp139 + 3U},1,{_tmp13A,_tmp13A,_tmp13A + 6U},(void*)& Cyc_opt6_spec,{_tmp13B,_tmp13B,_tmp13B + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 977
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1,Cyc_add_ccarg};static char _tmp13C[3U]="-L";static char _tmp13D[6U]="<dir>";static char _tmp13E[38U]="Add to the list of directories for -l";struct _tuple17 Cyc_opt7_tuple={{_tmp13C,_tmp13C,_tmp13C + 3U},1,{_tmp13D,_tmp13D,_tmp13D + 6U},(void*)& Cyc_opt7_spec,{_tmp13E,_tmp13E,_tmp13E + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 980
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1,Cyc_add_libarg};static char _tmp13F[3U]="-l";static char _tmp140[10U]="<libname>";static char _tmp141[13U]="Library file";struct _tuple17 Cyc_opt8_tuple={{_tmp13F,_tmp13F,_tmp13F + 3U},1,{_tmp140,_tmp140,_tmp140 + 10U},(void*)& Cyc_opt8_spec,{_tmp141,_tmp141,_tmp141 + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 983
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1,Cyc_add_marg};static char _tmp142[3U]="-m";static char _tmp143[9U]="<option>";static char _tmp144[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple17 Cyc_opt9_tuple={{_tmp142,_tmp142,_tmp142 + 3U},1,{_tmp143,_tmp143,_tmp143 + 9U},(void*)& Cyc_opt9_spec,{_tmp144,_tmp144,_tmp144 + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 986
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0,Cyc_set_stop_after_objectfile};static char _tmp145[3U]="-c";static char _tmp146[1U]="";static char _tmp147[61U]="Produce an object file instead of an executable; do not link";struct _tuple17 Cyc_opt10_tuple={{_tmp145,_tmp145,_tmp145 + 3U},0,{_tmp146,_tmp146,_tmp146 + 1U},(void*)& Cyc_opt10_spec,{_tmp147,_tmp147,_tmp147 + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 989
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5,Cyc_set_inputtype};static char _tmp148[3U]="-x";static char _tmp149[12U]=" <language>";static char _tmp14A[49U]="Specify <language> for the following input files";struct _tuple17 Cyc_opt11_tuple={{_tmp148,_tmp148,_tmp148 + 3U},0,{_tmp149,_tmp149,_tmp149 + 12U},(void*)& Cyc_opt11_spec,{_tmp14A,_tmp14A,_tmp14A + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 992
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0,Cyc_set_pa};static char _tmp14B[4U]="-pa";static char _tmp14C[1U]="";static char _tmp14D[33U]="Compile for profiling with aprof";struct _tuple17 Cyc_opt12_tuple={{_tmp14B,_tmp14B,_tmp14B + 4U},0,{_tmp14C,_tmp14C,_tmp14C + 1U},(void*)& Cyc_opt12_spec,{_tmp14D,_tmp14D,_tmp14D + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 995
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0,Cyc_set_stop_after_asmfile};static char _tmp14E[3U]="-S";static char _tmp14F[1U]="";static char _tmp150[35U]="Stop after producing assembly code";struct _tuple17 Cyc_opt13_tuple={{_tmp14E,_tmp14E,_tmp14E + 3U},0,{_tmp14F,_tmp14F,_tmp14F + 1U},(void*)& Cyc_opt13_spec,{_tmp150,_tmp150,_tmp150 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 998
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0,Cyc_set_produce_dependencies};static char _tmp151[3U]="-M";static char _tmp152[1U]="";static char _tmp153[21U]="Produce dependencies";struct _tuple17 Cyc_opt14_tuple={{_tmp151,_tmp151,_tmp151 + 3U},0,{_tmp152,_tmp152,_tmp152 + 1U},(void*)& Cyc_opt14_spec,{_tmp153,_tmp153,_tmp153 + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1001
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1,Cyc_add_cpparg};static char _tmp154[4U]="-MG";static char _tmp155[1U]="";static char _tmp156[68U]="When producing dependencies assume that missing files are generated";struct _tuple17 Cyc_opt15_tuple={{_tmp154,_tmp154,_tmp154 + 4U},0,{_tmp155,_tmp155,_tmp155 + 1U},(void*)& Cyc_opt15_spec,{_tmp156,_tmp156,_tmp156 + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1005
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5,Cyc_set_dependencies_target};static char _tmp157[4U]="-MT";static char _tmp158[10U]=" <target>";static char _tmp159[29U]="Give target for dependencies";struct _tuple17 Cyc_opt16_tuple={{_tmp157,_tmp157,_tmp157 + 4U},0,{_tmp158,_tmp158,_tmp158 + 10U},(void*)& Cyc_opt16_spec,{_tmp159,_tmp159,_tmp159 + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1008
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5,Cyc_Specsfile_set_target_arch};static char _tmp15A[3U]="-b";static char _tmp15B[10U]="<machine>";static char _tmp15C[19U]="Set target machine";struct _tuple17 Cyc_opt17_tuple={{_tmp15A,_tmp15A,_tmp15A + 3U},0,{_tmp15B,_tmp15B,_tmp15B + 10U},(void*)& Cyc_opt17_spec,{_tmp15C,_tmp15C,_tmp15C + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1011
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3,& Cyc_Flags_warn_lose_unique};static char _tmp15D[14U]="-Wlose-unique";static char _tmp15E[1U]="";static char _tmp15F[49U]="Try to warn when a unique pointer might get lost";struct _tuple17 Cyc_opt18_tuple={{_tmp15D,_tmp15D,_tmp15D + 14U},0,{_tmp15E,_tmp15E,_tmp15E + 1U},(void*)& Cyc_opt18_spec,{_tmp15F,_tmp15F,_tmp15F + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1014
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3,& Cyc_Flags_warn_override};static char _tmp160[11U]="-Woverride";static char _tmp161[1U]="";static char _tmp162[58U]="Warn when a local variable overrides an existing variable";struct _tuple17 Cyc_opt19_tuple={{_tmp160,_tmp160,_tmp160 + 11U},0,{_tmp161,_tmp161,_tmp161 + 1U},(void*)& Cyc_opt19_spec,{_tmp162,_tmp162,_tmp162 + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1017
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0,Cyc_Flags_set_all_warnings};static char _tmp163[6U]="-Wall";static char _tmp164[1U]="";static char _tmp165[21U]="Turn on all warnings";struct _tuple17 Cyc_opt20_tuple={{_tmp163,_tmp163,_tmp163 + 6U},0,{_tmp164,_tmp164,_tmp164 + 1U},(void*)& Cyc_opt20_spec,{_tmp165,_tmp165,_tmp165 + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1020
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt21_spec={0,Cyc_Flags_set_cpponly};static char _tmp166[3U]="-E";static char _tmp167[1U]="";static char _tmp168[25U]="Stop after preprocessing";struct _tuple17 Cyc_opt21_tuple={{_tmp166,_tmp166,_tmp166 + 3U},0,{_tmp167,_tmp167,_tmp167 + 1U},(void*)& Cyc_opt21_spec,{_tmp168,_tmp168,_tmp168 + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1023
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt22_spec={0,Cyc_Flags_set_parseonly};static char _tmp169[17U]="-stopafter-parse";static char _tmp16A[1U]="";static char _tmp16B[19U]="Stop after parsing";struct _tuple17 Cyc_opt22_tuple={{_tmp169,_tmp169,_tmp169 + 17U},0,{_tmp16A,_tmp16A,_tmp16A + 1U},(void*)& Cyc_opt22_spec,{_tmp16B,_tmp16B,_tmp16B + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1026
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt23_spec={0,Cyc_Flags_set_tconly};static char _tmp16C[14U]="-stopafter-tc";static char _tmp16D[1U]="";static char _tmp16E[25U]="Stop after type checking";struct _tuple17 Cyc_opt23_tuple={{_tmp16C,_tmp16C,_tmp16C + 14U},0,{_tmp16D,_tmp16D,_tmp16D + 1U},(void*)& Cyc_opt23_spec,{_tmp16E,_tmp16E,_tmp16E + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1029
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3,& Cyc_noprint_r};static char _tmp16F[9U]="-noprint";static char _tmp170[1U]="";static char _tmp171[42U]="Do not print output (when stopping early)";struct _tuple17 Cyc_opt24_tuple={{_tmp16F,_tmp16F,_tmp16F + 9U},0,{_tmp170,_tmp170,_tmp170 + 1U},(void*)& Cyc_opt24_spec,{_tmp171,_tmp171,_tmp171 + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1032
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt25_spec={0,Cyc_Flags_set_cfonly};static char _tmp172[14U]="-stopafter-cf";static char _tmp173[1U]="";static char _tmp174[33U]="Stop after control-flow checking";struct _tuple17 Cyc_opt25_tuple={{_tmp172,_tmp172,_tmp172 + 14U},0,{_tmp173,_tmp173,_tmp173 + 1U},(void*)& Cyc_opt25_spec,{_tmp174,_tmp174,_tmp174 + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1035
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt26_spec={0,Cyc_Flags_set_toconly};static char _tmp175[15U]="-stopafter-toc";static char _tmp176[1U]="";static char _tmp177[28U]="Stop after translation to C";struct _tuple17 Cyc_opt26_tuple={{_tmp175,_tmp175,_tmp175 + 15U},0,{_tmp176,_tmp176,_tmp176 + 1U},(void*)& Cyc_opt26_spec,{_tmp177,_tmp177,_tmp177 + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1038
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3,& Cyc_ic_r};static char _tmp178[4U]="-ic";static char _tmp179[1U]="";static char _tmp17A[26U]="Activate the link-checker";struct _tuple17 Cyc_opt27_tuple={{_tmp178,_tmp178,_tmp178 + 4U},0,{_tmp179,_tmp179,_tmp179 + 1U},(void*)& Cyc_opt27_spec,{_tmp17A,_tmp17A,_tmp17A + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1041
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3,& Cyc_pp_r};static char _tmp17B[4U]="-pp";static char _tmp17C[1U]="";static char _tmp17D[47U]="Pretty print the C code that Cyclone generates";struct _tuple17 Cyc_opt28_tuple={{_tmp17B,_tmp17B,_tmp17B + 4U},0,{_tmp17C,_tmp17C,_tmp17C + 1U},(void*)& Cyc_opt28_spec,{_tmp17D,_tmp17D,_tmp17D + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1044
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4,& Cyc_pp_r};static char _tmp17E[4U]="-up";static char _tmp17F[1U]="";static char _tmp180[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple17 Cyc_opt29_tuple={{_tmp17E,_tmp17E,_tmp17E + 4U},0,{_tmp17F,_tmp17F,_tmp17F + 1U},(void*)& Cyc_opt29_spec,{_tmp180,_tmp180,_tmp180 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1047
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3,& Cyc_elim_se_r};static char _tmp181[28U]="-elim-statement-expressions";static char _tmp182[1U]="";static char _tmp183[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple17 Cyc_opt30_tuple={{_tmp181,_tmp181,_tmp181 + 28U},0,{_tmp182,_tmp182,_tmp182 + 1U},(void*)& Cyc_opt30_spec,{_tmp183,_tmp183,_tmp183 + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1051
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4,& Cyc_elim_se_r};static char _tmp184[31U]="-no-elim-statement-expressions";static char _tmp185[1U]="";static char _tmp186[47U]="Do not avoid statement expressions in C output";struct _tuple17 Cyc_opt31_tuple={{_tmp184,_tmp184,_tmp184 + 31U},0,{_tmp185,_tmp185,_tmp185 + 1U},(void*)& Cyc_opt31_spec,{_tmp186,_tmp186,_tmp186 + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1054
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0,Cyc_set_tovc};static char _tmp187[8U]="-un-gcc";static char _tmp188[1U]="";static char _tmp189[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple17 Cyc_opt32_tuple={{_tmp187,_tmp187,_tmp187 + 8U},0,{_tmp188,_tmp188,_tmp188 + 1U},(void*)& Cyc_opt32_spec,{_tmp189,_tmp189,_tmp189 + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1057
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5,Cyc_set_c_compiler};static char _tmp18A[8U]="-c-comp";static char _tmp18B[12U]=" <compiler>";static char _tmp18C[40U]="Produce C output for the given compiler";struct _tuple17 Cyc_opt33_tuple={{_tmp18A,_tmp18A,_tmp18A + 8U},0,{_tmp18B,_tmp18B,_tmp18B + 12U},(void*)& Cyc_opt33_spec,{_tmp18C,_tmp18C,_tmp18C + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1060
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0,Cyc_set_save_temps};static char _tmp18D[12U]="-save-temps";static char _tmp18E[1U]="";static char _tmp18F[29U]="Don't delete temporary files";struct _tuple17 Cyc_opt34_tuple={{_tmp18D,_tmp18D,_tmp18D + 12U},0,{_tmp18E,_tmp18E,_tmp18E + 1U},(void*)& Cyc_opt34_spec,{_tmp18F,_tmp18F,_tmp18F + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1063
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3,& Cyc_save_c_r};static char _tmp190[8U]="-save-c";static char _tmp191[1U]="";static char _tmp192[31U]="Don't delete temporary C files";struct _tuple17 Cyc_opt35_tuple={{_tmp190,_tmp190,_tmp190 + 8U},0,{_tmp191,_tmp191,_tmp191 + 1U},(void*)& Cyc_opt35_spec,{_tmp192,_tmp192,_tmp192 + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1066
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0,Cyc_clear_lineno};static char _tmp193[11U]="--nolineno";static char _tmp194[1U]="";static char _tmp195[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple17 Cyc_opt36_tuple={{_tmp193,_tmp193,_tmp193 + 11U},0,{_tmp194,_tmp194,_tmp194 + 1U},(void*)& Cyc_opt36_spec,{_tmp195,_tmp195,_tmp195 + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1069
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0,Cyc_set_nochecks};static char _tmp196[11U]="--nochecks";static char _tmp197[1U]="";static char _tmp198[27U]="Disable bounds/null checks";struct _tuple17 Cyc_opt37_tuple={{_tmp196,_tmp196,_tmp196 + 11U},0,{_tmp197,_tmp197,_tmp197 + 1U},(void*)& Cyc_opt37_spec,{_tmp198,_tmp198,_tmp198 + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1072
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0,Cyc_set_nonullchecks};static char _tmp199[15U]="--nonullchecks";static char _tmp19A[1U]="";static char _tmp19B[20U]="Disable null checks";struct _tuple17 Cyc_opt38_tuple={{_tmp199,_tmp199,_tmp199 + 15U},0,{_tmp19A,_tmp19A,_tmp19A + 1U},(void*)& Cyc_opt38_spec,{_tmp19B,_tmp19B,_tmp19B + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1075
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0,Cyc_set_noboundschecks};static char _tmp19C[17U]="--noboundschecks";static char _tmp19D[1U]="";static char _tmp19E[22U]="Disable bounds checks";struct _tuple17 Cyc_opt39_tuple={{_tmp19C,_tmp19C,_tmp19C + 17U},0,{_tmp19D,_tmp19D,_tmp19D + 1U},(void*)& Cyc_opt39_spec,{_tmp19E,_tmp19E,_tmp19E + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1078
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5,Cyc_set_cpp};static char _tmp19F[9U]="-use-cpp";static char _tmp1A0[7U]="<path>";static char _tmp1A1[35U]="Indicate which preprocessor to use";struct _tuple17 Cyc_opt41_tuple={{_tmp19F,_tmp19F,_tmp19F + 9U},0,{_tmp1A0,_tmp1A0,_tmp1A0 + 7U},(void*)& Cyc_opt41_spec,{_tmp1A1,_tmp1A1,_tmp1A1 + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1081
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0,Cyc_set_nocppprecomp};static char _tmp1A2[16U]="-no-cpp-precomp";static char _tmp1A3[1U]="";static char _tmp1A4[40U]="Don't do smart preprocessing (mac only)";struct _tuple17 Cyc_opt42_tuple={{_tmp1A2,_tmp1A2,_tmp1A2 + 16U},0,{_tmp1A3,_tmp1A3,_tmp1A3 + 1U},(void*)& Cyc_opt42_spec,{_tmp1A4,_tmp1A4,_tmp1A4 + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1084
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0,Cyc_set_nocyc};static char _tmp1A5[7U]="-nocyc";static char _tmp1A6[1U]="";static char _tmp1A7[33U]="Don't add implicit namespace Cyc";struct _tuple17 Cyc_opt43_tuple={{_tmp1A5,_tmp1A5,_tmp1A5 + 7U},0,{_tmp1A6,_tmp1A6,_tmp1A6 + 1U},(void*)& Cyc_opt43_spec,{_tmp1A7,_tmp1A7,_tmp1A7 + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1087
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3,& Cyc_nogc_r};static char _tmp1A8[6U]="-nogc";static char _tmp1A9[1U]="";static char _tmp1AA[36U]="Don't link in the garbage collector";struct _tuple17 Cyc_opt44_tuple={{_tmp1A8,_tmp1A8,_tmp1A8 + 6U},0,{_tmp1A9,_tmp1A9,_tmp1A9 + 1U},(void*)& Cyc_opt44_spec,{_tmp1AA,_tmp1AA,_tmp1AA + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1090
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3,& Cyc_noshake_r};static char _tmp1AB[16U]="-noremoveunused";static char _tmp1AC[1U]="";static char _tmp1AD[49U]="Don't remove externed variables that aren't used";struct _tuple17 Cyc_opt45_tuple={{_tmp1AB,_tmp1AB,_tmp1AB + 16U},0,{_tmp1AC,_tmp1AC,_tmp1AC + 1U},(void*)& Cyc_opt45_spec,{_tmp1AD,_tmp1AD,_tmp1AD + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1093
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3,& Cyc_Flags_noexpand_r};static char _tmp1AE[18U]="-noexpandtypedefs";static char _tmp1AF[1U]="";static char _tmp1B0[41U]="Don't expand typedefs in pretty printing";struct _tuple17 Cyc_opt46_tuple={{_tmp1AE,_tmp1AE,_tmp1AE + 18U},0,{_tmp1AF,_tmp1AF,_tmp1AF + 1U},(void*)& Cyc_opt46_spec,{_tmp1B0,_tmp1B0,_tmp1B0 + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1096
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3,& Cyc_print_all_tvars_r};static char _tmp1B1[15U]="-printalltvars";static char _tmp1B2[1U]="";static char _tmp1B3[57U]="Print all type variables (even implicit default effects)";struct _tuple17 Cyc_opt47_tuple={{_tmp1B1,_tmp1B1,_tmp1B1 + 15U},0,{_tmp1B2,_tmp1B2,_tmp1B2 + 1U},(void*)& Cyc_opt47_spec,{_tmp1B3,_tmp1B3,_tmp1B3 + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1099
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3,& Cyc_print_all_kinds_r};static char _tmp1B4[15U]="-printallkinds";static char _tmp1B5[1U]="";static char _tmp1B6[37U]="Always print kinds of type variables";struct _tuple17 Cyc_opt48_tuple={{_tmp1B4,_tmp1B4,_tmp1B4 + 15U},0,{_tmp1B5,_tmp1B5,_tmp1B5 + 1U},(void*)& Cyc_opt48_spec,{_tmp1B6,_tmp1B6,_tmp1B6 + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1102
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3,& Cyc_print_full_evars_r};static char _tmp1B7[16U]="-printfullevars";static char _tmp1B8[1U]="";static char _tmp1B9[50U]="Print full information for evars (type debugging)";struct _tuple17 Cyc_opt49_tuple={{_tmp1B7,_tmp1B7,_tmp1B7 + 16U},0,{_tmp1B8,_tmp1B8,_tmp1B8 + 1U},(void*)& Cyc_opt49_spec,{_tmp1B9,_tmp1B9,_tmp1B9 + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1105
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3,& Cyc_print_all_effects_r};static char _tmp1BA[17U]="-printalleffects";static char _tmp1BB[1U]="";static char _tmp1BC[45U]="Print effects for functions (type debugging)";struct _tuple17 Cyc_opt50_tuple={{_tmp1BA,_tmp1BA,_tmp1BA + 17U},0,{_tmp1BB,_tmp1BB,_tmp1BB + 1U},(void*)& Cyc_opt50_spec,{_tmp1BC,_tmp1BC,_tmp1BC + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1108
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3,& Cyc_nocyc_setjmp_r};static char _tmp1BD[14U]="-nocyc_setjmp";static char _tmp1BE[1U]="";static char _tmp1BF[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple17 Cyc_opt51_tuple={{_tmp1BD,_tmp1BD,_tmp1BD + 14U},0,{_tmp1BE,_tmp1BE,_tmp1BE + 1U},(void*)& Cyc_opt51_spec,{_tmp1BF,_tmp1BF,_tmp1BF + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1111
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3,& Cyc_Flags_compile_for_boot};static char _tmp1C0[18U]="-compile-for-boot";static char _tmp1C1[1U]="";static char _tmp1C2[71U]="Compile using the special boot library instead of the standard library";struct _tuple17 Cyc_opt52_tuple={{_tmp1C0,_tmp1C0,_tmp1C0 + 18U},0,{_tmp1C1,_tmp1C1,_tmp1C1 + 1U},(void*)& Cyc_opt52_spec,{_tmp1C2,_tmp1C2,_tmp1C2 + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1114
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5,Cyc_set_cyc_include};static char _tmp1C3[4U]="-CI";static char _tmp1C4[8U]=" <file>";static char _tmp1C5[31U]="Set cyc_include.h to be <file>";struct _tuple17 Cyc_opt53_tuple={{_tmp1C3,_tmp1C3,_tmp1C3 + 4U},0,{_tmp1C4,_tmp1C4,_tmp1C4 + 8U},(void*)& Cyc_opt53_spec,{_tmp1C5,_tmp1C5,_tmp1C5 + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1117
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3,& Cyc_Flags_warn_bounds_checks};static char _tmp1C6[18U]="-warnboundschecks";static char _tmp1C7[1U]="";static char _tmp1C8[44U]="Warn when bounds checks can't be eliminated";struct _tuple17 Cyc_opt54_tuple={{_tmp1C6,_tmp1C6,_tmp1C6 + 18U},0,{_tmp1C7,_tmp1C7,_tmp1C7 + 1U},(void*)& Cyc_opt54_spec,{_tmp1C8,_tmp1C8,_tmp1C8 + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1120
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3,& Cyc_Flags_warn_all_null_deref};static char _tmp1C9[16U]="-warnnullchecks";static char _tmp1CA[1U]="";static char _tmp1CB[45U]="Warn when any null check can't be eliminated";struct _tuple17 Cyc_opt55_tuple={{_tmp1C9,_tmp1C9,_tmp1C9 + 16U},0,{_tmp1CA,_tmp1CA,_tmp1CA + 1U},(void*)& Cyc_opt55_spec,{_tmp1CB,_tmp1CB,_tmp1CB + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1123
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3,& Cyc_Flags_warn_alias_coerce};static char _tmp1CC[19U]="-warnaliascoercion";static char _tmp1CD[1U]="";static char _tmp1CE[41U]="Warn when any alias coercion is inserted";struct _tuple17 Cyc_opt56_tuple={{_tmp1CC,_tmp1CC,_tmp1CC + 19U},0,{_tmp1CD,_tmp1CD,_tmp1CD + 1U},(void*)& Cyc_opt56_spec,{_tmp1CE,_tmp1CE,_tmp1CE + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1126
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3,& Cyc_Flags_warn_region_coerce};static char _tmp1CF[18U]="-warnregioncoerce";static char _tmp1D0[1U]="";static char _tmp1D1[42U]="Warn when any region coercion is inserted";struct _tuple17 Cyc_opt57_tuple={{_tmp1CF,_tmp1CF,_tmp1CF + 18U},0,{_tmp1D0,_tmp1D0,_tmp1D0 + 1U},(void*)& Cyc_opt57_spec,{_tmp1D1,_tmp1D1,_tmp1D1 + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1129
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3,& Cyc_Flags_no_register};static char _tmp1D2[12U]="-noregister";static char _tmp1D3[1U]="";static char _tmp1D4[39U]="Treat register storage class as public";struct _tuple17 Cyc_opt58_tuple={{_tmp1D2,_tmp1D2,_tmp1D2 + 12U},0,{_tmp1D3,_tmp1D3,_tmp1D3 + 1U},(void*)& Cyc_opt58_spec,{_tmp1D4,_tmp1D4,_tmp1D4 + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1132
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4,& Cyc_Position_use_gcc_style_location};static char _tmp1D5[18U]="-detailedlocation";static char _tmp1D6[1U]="";static char _tmp1D7[58U]="Try to give more detailed location information for errors";struct _tuple17 Cyc_opt59_tuple={{_tmp1D5,_tmp1D5,_tmp1D5 + 18U},0,{_tmp1D6,_tmp1D6,_tmp1D6 + 1U},(void*)& Cyc_opt59_spec,{_tmp1D7,_tmp1D7,_tmp1D7 + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1135
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0,Cyc_set_port_c_code};static char _tmp1D8[6U]="-port";static char _tmp1D9[1U]="";static char _tmp1DA[38U]="Suggest how to port C code to Cyclone";struct _tuple17 Cyc_opt60_tuple={{_tmp1D8,_tmp1D8,_tmp1D8 + 6U},0,{_tmp1D9,_tmp1D9,_tmp1D9 + 1U},(void*)& Cyc_opt60_spec,{_tmp1DA,_tmp1DA,_tmp1DA + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1138
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3,& Cyc_Flags_no_regions};static char _tmp1DB[11U]="-noregions";static char _tmp1DC[1U]="";static char _tmp1DD[39U]="Generate code that doesn't use regions";struct _tuple17 Cyc_opt61_tuple={{_tmp1DB,_tmp1DB,_tmp1DB + 11U},0,{_tmp1DC,_tmp1DC,_tmp1DC + 1U},(void*)& Cyc_opt61_spec,{_tmp1DD,_tmp1DD,_tmp1DD + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1141
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5,Cyc_add_iprefix};static char _tmp1DE[9U]="-iprefix";static char _tmp1DF[9U]="<prefix>";static char _tmp1E0[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple17 Cyc_opt62_tuple={{_tmp1DE,_tmp1DE,_tmp1DE + 9U},0,{_tmp1DF,_tmp1DF,_tmp1DF + 9U},(void*)& Cyc_opt62_spec,{_tmp1E0,_tmp1E0,_tmp1E0 + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1144
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5,Cyc_add_iwithprefix};static char _tmp1E1[13U]="-iwithprefix";static char _tmp1E2[6U]="<dir>";static char _tmp1E3[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple17 Cyc_opt63_tuple={{_tmp1E1,_tmp1E1,_tmp1E1 + 13U},0,{_tmp1E2,_tmp1E2,_tmp1E2 + 6U},(void*)& Cyc_opt63_spec,{_tmp1E3,_tmp1E3,_tmp1E3 + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1147
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5,Cyc_add_iwithprefixbefore};static char _tmp1E4[19U]="-iwithprefixbefore";static char _tmp1E5[6U]="<dir>";static char _tmp1E6[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple17 Cyc_opt64_tuple={{_tmp1E4,_tmp1E4,_tmp1E4 + 19U},0,{_tmp1E5,_tmp1E5,_tmp1E5 + 6U},(void*)& Cyc_opt64_spec,{_tmp1E6,_tmp1E6,_tmp1E6 + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1150
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5,Cyc_add_isystem};static char _tmp1E7[9U]="-isystem";static char _tmp1E8[6U]="<dir>";static char _tmp1E9[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple17 Cyc_opt65_tuple={{_tmp1E7,_tmp1E7,_tmp1E7 + 9U},0,{_tmp1E8,_tmp1E8,_tmp1E8 + 6U},(void*)& Cyc_opt65_spec,{_tmp1E9,_tmp1E9,_tmp1E9 + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1153
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5,Cyc_add_idirafter};static char _tmp1EA[11U]="-idirafter";static char _tmp1EB[6U]="<dir>";static char _tmp1EC[46U]="Add the directory to the second include path.";struct _tuple17 Cyc_opt66_tuple={{_tmp1EA,_tmp1EA,_tmp1EA + 11U},0,{_tmp1EB,_tmp1EB,_tmp1EB + 6U},(void*)& Cyc_opt66_spec,{_tmp1EC,_tmp1EC,_tmp1EC + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1156
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3,& Cyc_generate_interface_r};static char _tmp1ED[15U]="--geninterface";static char _tmp1EE[1U]="";static char _tmp1EF[25U]="Generate interface files";struct _tuple17 Cyc_opt67_tuple={{_tmp1ED,_tmp1ED,_tmp1ED + 15U},0,{_tmp1EE,_tmp1EE,_tmp1EE + 1U},(void*)& Cyc_opt67_spec,{_tmp1EF,_tmp1EF,_tmp1EF + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1159
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5,Cyc_set_specified_interface};static char _tmp1F0[12U]="--interface";static char _tmp1F1[8U]=" <file>";static char _tmp1F2[37U]="Set the interface file to be <file>.";struct _tuple17 Cyc_opt68_tuple={{_tmp1F0,_tmp1F0,_tmp1F0 + 12U},0,{_tmp1F1,_tmp1F1,_tmp1F1 + 8U},(void*)& Cyc_opt68_spec,{_tmp1F2,_tmp1F2,_tmp1F2 + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1162
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0,Cyc_set_many_errors};static char _tmp1F3[13U]="--manyerrors";static char _tmp1F4[1U]="";static char _tmp1F5[36U]="don't stop after only a few errors.";struct _tuple17 Cyc_opt69_tuple={{_tmp1F3,_tmp1F3,_tmp1F3 + 13U},0,{_tmp1F4,_tmp1F4,_tmp1F4 + 1U},(void*)& Cyc_opt69_spec,{_tmp1F5,_tmp1F5,_tmp1F5 + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1165
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3,& Cyc_Flags_print_parser_state_and_token};static char _tmp1F6[13U]="--parsestate";static char _tmp1F7[1U]="";static char _tmp1F8[50U]="print the parse state and token on a syntax error";struct _tuple17 Cyc_opt70_tuple={{_tmp1F6,_tmp1F6,_tmp1F6 + 13U},0,{_tmp1F7,_tmp1F7,_tmp1F7 + 1U},(void*)& Cyc_opt70_spec,{_tmp1F8,_tmp1F8,_tmp1F8 + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1168
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0,Cyc_noassume_gcc_flag};static char _tmp1F9[22U]="-known-gcc-flags-only";static char _tmp1FA[1U]="";static char _tmp1FB[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple17 Cyc_opt71_tuple={{_tmp1F9,_tmp1F9,_tmp1F9 + 22U},0,{_tmp1FA,_tmp1FA,_tmp1FA + 1U},(void*)& Cyc_opt71_spec,{_tmp1FB,_tmp1FB,_tmp1FB + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1171
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0,Cyc_print_options};static char _tmp1FC[6U]="-help";static char _tmp1FD[1U]="";static char _tmp1FE[32U]="print out the available options";struct _tuple17 Cyc_opt72_tuple={{_tmp1FC,_tmp1FC,_tmp1FC + 6U},0,{_tmp1FD,_tmp1FD,_tmp1FD + 1U},(void*)& Cyc_opt72_spec,{_tmp1FE,_tmp1FE,_tmp1FE + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1174
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0,Cyc_print_options};static char _tmp1FF[7U]="-usage";static char _tmp200[1U]="";static char _tmp201[32U]="print out the available options";struct _tuple17 Cyc_opt73_tuple={{_tmp1FF,_tmp1FF,_tmp1FF + 7U},0,{_tmp200,_tmp200,_tmp200 + 1U},(void*)& Cyc_opt73_spec,{_tmp201,_tmp201,_tmp201 + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1177
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0,Cyc_set_notoseqc};static char _tmp202[10U]="-no-seq-c";static char _tmp203[1U]="";static char _tmp204[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple17 Cyc_opt74_tuple={{_tmp202,_tmp202,_tmp202 + 10U},0,{_tmp203,_tmp203,_tmp203 + 1U},(void*)& Cyc_opt74_spec,{_tmp204,_tmp204,_tmp204 + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1180
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt75_spec={0,Cyc_set_pg};static char _tmp205[4U]="-pg";static char _tmp206[1U]="";static char _tmp207[33U]="Compile for profiling with gprof";struct _tuple17 Cyc_opt75_tuple={{_tmp205,_tmp205,_tmp205 + 4U},0,{_tmp206,_tmp206,_tmp206 + 1U},(void*)& Cyc_opt75_spec,{_tmp207,_tmp207,_tmp207 + 33U}};struct Cyc_List_List Cyc_opt75={(void*)& Cyc_opt75_tuple,0};
# 1183
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt76_spec={4,& Cyc_vcgen_r};static char _tmp208[10U]="--novcgen";static char _tmp209[1U]="";static char _tmp20A[39U]="do not compute verification-conditions";struct _tuple17 Cyc_opt76_tuple={{_tmp208,_tmp208,_tmp208 + 10U},0,{_tmp209,_tmp209,_tmp209 + 1U},(void*)& Cyc_opt76_spec,{_tmp20A,_tmp20A,_tmp20A + 39U}};struct Cyc_List_List Cyc_opt76={(void*)& Cyc_opt76_tuple,0};
# 1186
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt77_spec={0,Cyc_Cifc_set_inst_tvar};static char _tmp20B[17U]="--cifc-inst-tvar";static char _tmp20C[1U]="";static char _tmp20D[59U]="instantiate type variables instead of introducing new ones";struct _tuple17 Cyc_opt77_tuple={{_tmp20B,_tmp20B,_tmp20B + 17U},0,{_tmp20C,_tmp20C,_tmp20C + 1U},(void*)& Cyc_opt77_spec,{_tmp20D,_tmp20D,_tmp20D + 59U}};struct Cyc_List_List Cyc_opt77={(void*)& Cyc_opt77_tuple,0};
# 1189
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt78_spec={0,Cyc_Flags_set_crgnonly};static char _tmp20E[16U]="-stopafter-crgn";static char _tmp20F[1U]="";static char _tmp210[19U]="Stop after currrgn";struct _tuple17 Cyc_opt78_tuple={{_tmp20E,_tmp20E,_tmp20E + 16U},0,{_tmp20F,_tmp20F,_tmp20F + 1U},(void*)& Cyc_opt78_spec,{_tmp210,_tmp210,_tmp210 + 19U}};struct Cyc_List_List Cyc_opt78={(void*)& Cyc_opt78_tuple,0};
# 1193
struct Cyc_List_List*Cyc_global_options[77U]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73,& Cyc_opt74,& Cyc_opt75,& Cyc_opt76,& Cyc_opt77,& Cyc_opt78};
# 1206
void Cyc_print_options (void){
({struct Cyc_List_List*_tmp364=Cyc_global_options[0];Cyc_Arg_usage(_tmp364,({const char*_tmp211="<program.cyc>";_tag_fat(_tmp211,sizeof(char),14U);}));});}
# 1211
int Cyc_main(int argc,struct _fat_ptr argv){
# 1214
GC_blacklist_warn_clear();{
# 1217
struct _fat_ptr optstring=({const char*_tmp292="Options:";_tag_fat(_tmp292,sizeof(char),9U);});
# 1219
{int i=1;for(0;(unsigned)i < 77U;++ i){
({struct Cyc_List_List*_tmp365=*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,77U,sizeof(struct Cyc_List_List*),i));(*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,77U,sizeof(struct Cyc_List_List*),i - 1)))->tl=_tmp365;});}}{
struct Cyc_List_List*options=Cyc_global_options[0];
# 1223
struct _fat_ptr _tmp212=Cyc_Specsfile_parse_b(options,Cyc_add_other,Cyc_add_other_flag,optstring,argv);struct _fat_ptr otherargs=_tmp212;
# 1229
struct _fat_ptr _tmp213=Cyc_find_in_arch_path(({const char*_tmp291="cycspecs";_tag_fat(_tmp291,sizeof(char),9U);}));struct _fat_ptr specs_file=_tmp213;
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp216=({struct Cyc_String_pa_PrintArg_struct _tmp2B7;_tmp2B7.tag=0,_tmp2B7.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp2B7;});void*_tmp214[1];_tmp214[0]=& _tmp216;({struct Cyc___cycFILE*_tmp367=Cyc_stderr;struct _fat_ptr _tmp366=({const char*_tmp215="Reading from specs file %s\n";_tag_fat(_tmp215,sizeof(char),28U);});Cyc_fprintf(_tmp367,_tmp366,_tag_fat(_tmp214,sizeof(void*),1));});});{
struct Cyc_List_List*_tmp217=Cyc_Specsfile_read_specs(specs_file);struct Cyc_List_List*specs=_tmp217;
# 1233
struct _fat_ptr _tmp218=Cyc_Specsfile_split_specs(({struct Cyc_List_List*_tmp368=specs;Cyc_Specsfile_get_spec(_tmp368,({const char*_tmp290="cyclone";_tag_fat(_tmp290,sizeof(char),8U);}));}));struct _fat_ptr args_from_specs_file=_tmp218;
if(({struct _fat_ptr*_tmp369=(struct _fat_ptr*)args_from_specs_file.curr;_tmp369 != (struct _fat_ptr*)(_tag_fat(0,0,0)).curr;})){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,args_from_specs_file);}{
# 1238
struct _fat_ptr _tmp219=({struct Cyc_List_List*_tmp36A=specs;Cyc_Specsfile_get_spec(_tmp36A,({const char*_tmp28F="cyclone_target_cflags";_tag_fat(_tmp28F,sizeof(char),22U);}));});struct _fat_ptr target_cflags=_tmp219;
struct _fat_ptr _tmp21A=({struct Cyc_List_List*_tmp36B=specs;Cyc_Specsfile_get_spec(_tmp36B,({const char*_tmp28E="cyclone_cc";_tag_fat(_tmp28E,sizeof(char),11U);}));});struct _fat_ptr cyclone_cc=_tmp21A;
if(!((unsigned)cyclone_cc.curr))cyclone_cc=({const char*_tmp21B="gcc";_tag_fat(_tmp21B,sizeof(char),4U);});
Cyc_def_inc_path=({struct Cyc_List_List*_tmp36C=specs;Cyc_Specsfile_get_spec(_tmp36C,({const char*_tmp21C="cyclone_inc_path";_tag_fat(_tmp21C,sizeof(char),17U);}));});
# 1244
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,otherargs);
if(({struct _fat_ptr _tmp36D=(struct _fat_ptr)Cyc_cpp;Cyc_strcmp(_tmp36D,({const char*_tmp21D="";_tag_fat(_tmp21D,sizeof(char),1U);}));})== 0){
# 1262 "cyclone.cyc"
const char*_tmp21E=Cyc_produce_dependencies?"":" -E";const char*dash_E=_tmp21E;
Cyc_set_cpp((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp221=({struct Cyc_String_pa_PrintArg_struct _tmp2BB;_tmp2BB.tag=0,_tmp2BB.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_tmp2BB;});struct Cyc_String_pa_PrintArg_struct _tmp222=({struct Cyc_String_pa_PrintArg_struct _tmp2BA;_tmp2BA.tag=0,_tmp2BA.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_tmp2BA;});struct Cyc_String_pa_PrintArg_struct _tmp223=({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0,({
struct _fat_ptr _tmp36E=(struct _fat_ptr)({const char*_tmp225=dash_E;_tag_fat(_tmp225,sizeof(char),_get_zero_arr_size_char((void*)_tmp225,1U));});_tmp2B9.f1=_tmp36E;});_tmp2B9;});struct Cyc_String_pa_PrintArg_struct _tmp224=({struct Cyc_String_pa_PrintArg_struct _tmp2B8;_tmp2B8.tag=0,_tmp2B8.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp2B8;});void*_tmp21F[4];_tmp21F[0]=& _tmp221,_tmp21F[1]=& _tmp222,_tmp21F[2]=& _tmp223,_tmp21F[3]=& _tmp224;({struct _fat_ptr _tmp36F=({const char*_tmp220="%s %s -w -x c%s -specs %s";_tag_fat(_tmp220,sizeof(char),26U);});Cyc_aprintf(_tmp36F,_tag_fat(_tmp21F,sizeof(void*),4));});}));}
# 1266
if(Cyc_cyclone_files == 0 && Cyc_ccargs == 0){
({void*_tmp226=0U;({struct Cyc___cycFILE*_tmp371=Cyc_stderr;struct _fat_ptr _tmp370=({const char*_tmp227="missing file\n";_tag_fat(_tmp227,sizeof(char),14U);});Cyc_fprintf(_tmp371,_tmp370,_tag_fat(_tmp226,sizeof(void*),0));});});
exit(1);}
# 1275
if((int)Cyc_Flags_stop_after_pass > (int)Cyc_Flags_Cpp && !Cyc_nocyc_setjmp_r)
Cyc_cyc_setjmp=Cyc_find_in_arch_path(({const char*_tmp228="cyc_setjmp.h";_tag_fat(_tmp228,sizeof(char),13U);}));
if((int)Cyc_Flags_stop_after_pass > (int)Cyc_Flags_Cpp && Cyc_strlen((struct _fat_ptr)Cyc_cyc_include)== (unsigned long)0)
Cyc_cyc_include=Cyc_find_in_exec_path(({const char*_tmp229="cyc_include.h";_tag_fat(_tmp229,sizeof(char),14U);}));
# 1281
{struct _handler_cons _tmp22A;_push_handler(& _tmp22A);{int _tmp22C=0;if(setjmp(_tmp22A.handler))_tmp22C=1;if(!_tmp22C){
{struct Cyc_List_List*_tmp22D=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_cyclone_files);struct Cyc_List_List*l=_tmp22D;for(0;l != 0;l=l->tl){
Cyc_process_file(*((struct _fat_ptr*)l->hd));
if(Cyc_compile_failure){int _tmp22E=1;_npop_handler(0);return _tmp22E;}}}
# 1282
;_pop_handler();}else{void*_tmp22B=(void*)Cyc_Core_get_exn_thrown();void*_tmp22F=_tmp22B;void*_tmp230;_tmp230=_tmp22F;{void*x=_tmp230;
# 1287
if(Cyc_compile_failure)return 1;
Cyc_Core_rethrow(x);};}}}
# 1291
if((int)Cyc_Flags_stop_after_pass < 13)return 0;
# 1297
if(Cyc_ccargs == 0)return 0;
# 1300
if((unsigned)Cyc_Specsfile_target_arch.curr &&(unsigned)Cyc_Specsfile_cyclone_exec_path)
Cyc_add_ccarg((struct _fat_ptr)({struct Cyc_List_List*_tmp375=({struct Cyc_List_List*_tmp233=_cycalloc(sizeof(*_tmp233));({struct _fat_ptr*_tmp374=({struct _fat_ptr*_tmp232=_cycalloc(sizeof(*_tmp232));({struct _fat_ptr _tmp373=({const char*_tmp231="";_tag_fat(_tmp231,sizeof(char),1U);});*_tmp232=_tmp373;});_tmp232;});_tmp233->hd=_tmp374;}),({
struct Cyc_List_List*_tmp372=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_tmp233->tl=_tmp372;});_tmp233;});
# 1301
Cyc_str_sepstr(_tmp375,({const char*_tmp234=" -L";_tag_fat(_tmp234,sizeof(char),4U);}));}));
# 1304
Cyc_add_ccarg((struct _fat_ptr)({struct _fat_ptr _tmp376=({const char*_tmp235="-L";_tag_fat(_tmp235,sizeof(char),3U);});Cyc_strconcat(_tmp376,(struct _fat_ptr)Cyc_Specsfile_def_lib_path);}));
# 1306
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_ccargs);{
struct _fat_ptr _tmp236=({struct Cyc_List_List*_tmp377=((struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs);Cyc_str_sepstr(_tmp377,({const char*_tmp28D=" ";_tag_fat(_tmp28D,sizeof(char),2U);}));});struct _fat_ptr ccargs_string=_tmp236;
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*))Cyc_List_rev)(Cyc_libargs);{
struct _fat_ptr _tmp237=({struct Cyc_List_List*_tmp37B=({struct Cyc_List_List*_tmp28B=_cycalloc(sizeof(*_tmp28B));({struct _fat_ptr*_tmp37A=({struct _fat_ptr*_tmp28A=_cycalloc(sizeof(*_tmp28A));({struct _fat_ptr _tmp379=({const char*_tmp289="";_tag_fat(_tmp289,sizeof(char),1U);});*_tmp28A=_tmp379;});_tmp28A;});_tmp28B->hd=_tmp37A;}),({
struct Cyc_List_List*_tmp378=((struct Cyc_List_List*(*)(struct _fat_ptr*(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp28B->tl=_tmp378;});_tmp28B;});
# 1309
Cyc_str_sepstr(_tmp37B,({const char*_tmp28C=" ";_tag_fat(_tmp28C,sizeof(char),2U);}));});struct _fat_ptr libargs_string=_tmp237;
# 1312
struct Cyc_List_List*stdlib;
struct _fat_ptr stdlib_string;
int _tmp238=
((Cyc_stop_after_asmfile_r || Cyc_stop_after_objectfile_r)||
# 1317
 Cyc_output_file != 0 &&({struct _fat_ptr _tmp37D=*((struct _fat_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp37D,({const char*_tmp287=".a";_tag_fat(_tmp287,sizeof(char),3U);}));}))||
 Cyc_output_file != 0 &&({struct _fat_ptr _tmp37C=*((struct _fat_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp37C,({const char*_tmp288=".lib";_tag_fat(_tmp288,sizeof(char),5U);}));});
# 1314
int is_not_executable=_tmp238;
# 1319
if(is_not_executable){
stdlib=0;
stdlib_string=(struct _fat_ptr)({const char*_tmp239="";_tag_fat(_tmp239,sizeof(char),1U);});}else{
# 1324
struct _fat_ptr libcyc_flag;
struct _fat_ptr nogc_filename;
struct _fat_ptr runtime_filename;
struct _fat_ptr gc_filename;
if(Cyc_pa_r){
libcyc_flag=({const char*_tmp23A="-lcyc_a";_tag_fat(_tmp23A,sizeof(char),8U);});
nogc_filename=({const char*_tmp23B="nogc_a.a";_tag_fat(_tmp23B,sizeof(char),9U);});
runtime_filename=({const char*_tmp23C="runtime_cyc_a.a";_tag_fat(_tmp23C,sizeof(char),16U);});}else{
if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp23D="-lcyc_nocheck";_tag_fat(_tmp23D,sizeof(char),14U);});
nogc_filename=({const char*_tmp23E="nogc.a";_tag_fat(_tmp23E,sizeof(char),7U);});
runtime_filename=({const char*_tmp23F="runtime_cyc.a";_tag_fat(_tmp23F,sizeof(char),14U);});}else{
if(Cyc_pg_r){
libcyc_flag=({const char*_tmp240="-lcyc_pg";_tag_fat(_tmp240,sizeof(char),9U);});
runtime_filename=({const char*_tmp241="runtime_cyc_pg.a";_tag_fat(_tmp241,sizeof(char),17U);});
nogc_filename=({const char*_tmp242="nogc_pg.a";_tag_fat(_tmp242,sizeof(char),10U);});}else{
if(Cyc_Flags_compile_for_boot){
# 1342
libcyc_flag=({const char*_tmp243="-lcycboot";_tag_fat(_tmp243,sizeof(char),10U);});
nogc_filename=({const char*_tmp244="nogc.a";_tag_fat(_tmp244,sizeof(char),7U);});
runtime_filename=({const char*_tmp245="runtime_cyc.a";_tag_fat(_tmp245,sizeof(char),14U);});}else{
if(Cyc_pthread_r){
libcyc_flag=({const char*_tmp246="-lcyc";_tag_fat(_tmp246,sizeof(char),6U);});
nogc_filename=({const char*_tmp247="nogc_pthread.a";_tag_fat(_tmp247,sizeof(char),15U);});
runtime_filename=({const char*_tmp248="runtime_cyc_pthread.a";_tag_fat(_tmp248,sizeof(char),22U);});}else{
# 1350
libcyc_flag=({const char*_tmp249="-lcyc";_tag_fat(_tmp249,sizeof(char),6U);});
nogc_filename=({const char*_tmp24A="nogc.a";_tag_fat(_tmp24A,sizeof(char),7U);});
runtime_filename=({const char*_tmp24B="runtime_cyc.a";_tag_fat(_tmp24B,sizeof(char),14U);});}}}}}
# 1354
if(!Cyc_pthread_r)
gc_filename=({const char*_tmp24C="gc.a";_tag_fat(_tmp24C,sizeof(char),5U);});else{
# 1357
gc_filename=({const char*_tmp24D="gc_pthread.a";_tag_fat(_tmp24D,sizeof(char),13U);});}{
# 1359
struct _fat_ptr _tmp24E=Cyc_nogc_r?Cyc_find_in_arch_path(nogc_filename): Cyc_find_in_arch_path(gc_filename);struct _fat_ptr gc=_tmp24E;
struct _fat_ptr _tmp24F=Cyc_find_in_arch_path(runtime_filename);struct _fat_ptr runtime=_tmp24F;
# 1363
stdlib=0;
stdlib_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp252=({struct Cyc_String_pa_PrintArg_struct _tmp2BE;_tmp2BE.tag=0,_tmp2BE.f1=(struct _fat_ptr)((struct _fat_ptr)libcyc_flag);_tmp2BE;});struct Cyc_String_pa_PrintArg_struct _tmp253=({struct Cyc_String_pa_PrintArg_struct _tmp2BD;_tmp2BD.tag=0,_tmp2BD.f1=(struct _fat_ptr)((struct _fat_ptr)runtime);_tmp2BD;});struct Cyc_String_pa_PrintArg_struct _tmp254=({struct Cyc_String_pa_PrintArg_struct _tmp2BC;_tmp2BC.tag=0,_tmp2BC.f1=(struct _fat_ptr)((struct _fat_ptr)gc);_tmp2BC;});void*_tmp250[3];_tmp250[0]=& _tmp252,_tmp250[1]=& _tmp253,_tmp250[2]=& _tmp254;({struct _fat_ptr _tmp37E=({const char*_tmp251=" %s %s %s ";_tag_fat(_tmp251,sizeof(char),11U);});Cyc_aprintf(_tmp37E,_tag_fat(_tmp250,sizeof(void*),3));});});}}
# 1367
if(Cyc_ic_r){struct _handler_cons _tmp255;_push_handler(& _tmp255);{int _tmp257=0;if(setjmp(_tmp255.handler))_tmp257=1;if(!_tmp257){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*)(struct _fat_ptr*),struct Cyc_List_List*))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp258=({struct Cyc_List_List*_tmp37F=stdlib;((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(_tmp37F,((struct Cyc_List_List*(*)(struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));});struct Cyc_List_List*lf=_tmp258;
if(!is_not_executable)
lf=({struct Cyc_List_List*_tmp259=_cycalloc(sizeof(*_tmp259));_tmp259->hd=Cyc_final_strptr,_tmp259->tl=lf;_tmp259;});{
# 1374
struct Cyc_Interface_I*_tmp25A=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*)(struct _fat_ptr*),struct Cyc_List_List*,struct Cyc_List_List*))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,lf,lf);struct Cyc_Interface_I*i=_tmp25A;
if(i == 0){
({void*_tmp25B=0U;({struct Cyc___cycFILE*_tmp381=Cyc_stderr;struct _fat_ptr _tmp380=({const char*_tmp25C="Error: interfaces incompatible\n";_tag_fat(_tmp25C,sizeof(char),32U);});Cyc_fprintf(_tmp381,_tmp380,_tag_fat(_tmp25B,sizeof(void*),0));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp25D=1;_npop_handler(0);return _tmp25D;}}
# 1381
if(is_not_executable){
if(Cyc_output_file != 0){
struct _fat_ptr _tmp25E=({struct Cyc_String_pa_PrintArg_struct _tmp265=({struct Cyc_String_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=0,({struct _fat_ptr _tmp382=(struct _fat_ptr)((struct _fat_ptr)Cyc_Filename_chop_extension(*((struct _fat_ptr*)_check_null(Cyc_output_file))));_tmp2BF.f1=_tmp382;});_tmp2BF;});void*_tmp263[1];_tmp263[0]=& _tmp265;({struct _fat_ptr _tmp383=({const char*_tmp264="%s.cycio";_tag_fat(_tmp264,sizeof(char),9U);});Cyc_aprintf(_tmp383,_tag_fat(_tmp263,sizeof(void*),1));});});struct _fat_ptr output_file_io=_tmp25E;
struct Cyc___cycFILE*_tmp25F=({struct _fat_ptr _tmp385=(struct _fat_ptr)output_file_io;struct _fat_ptr _tmp384=({const char*_tmp261="wb";_tag_fat(_tmp261,sizeof(char),3U);});Cyc_try_file_open(_tmp385,_tmp384,({const char*_tmp262="interface object file";_tag_fat(_tmp262,sizeof(char),22U);}));});struct Cyc___cycFILE*f=_tmp25F;
if(f == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp260=1;_npop_handler(0);return _tmp260;}}
# 1390
Cyc_Interface_save(i,f);
Cyc_file_close(f);}}else{
# 1394
if(!({struct Cyc_Interface_I*_tmp389=Cyc_Interface_empty();struct Cyc_Interface_I*_tmp388=i;Cyc_Interface_is_subinterface(_tmp389,_tmp388,({struct _tuple12*_tmp268=_cycalloc(sizeof(*_tmp268));
({struct _fat_ptr _tmp387=({const char*_tmp266="empty interface";_tag_fat(_tmp266,sizeof(char),16U);});_tmp268->f1=_tmp387;}),({struct _fat_ptr _tmp386=({const char*_tmp267="global interface";_tag_fat(_tmp267,sizeof(char),17U);});_tmp268->f2=_tmp386;});_tmp268;}));})){
({void*_tmp269=0U;({struct Cyc___cycFILE*_tmp38B=Cyc_stderr;struct _fat_ptr _tmp38A=({const char*_tmp26A="Error: some objects are still undefined\n";_tag_fat(_tmp26A,sizeof(char),41U);});Cyc_fprintf(_tmp38B,_tmp38A,_tag_fat(_tmp269,sizeof(void*),0));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp26B=1;_npop_handler(0);return _tmp26B;}}}}}
# 1368
;_pop_handler();}else{void*_tmp256=(void*)Cyc_Core_get_exn_thrown();void*_tmp26C=_tmp256;void*_tmp26D;_tmp26D=_tmp26C;{void*e=_tmp26D;
# 1404
({struct Cyc_String_pa_PrintArg_struct _tmp270=({struct Cyc_String_pa_PrintArg_struct _tmp2C2;_tmp2C2.tag=0,({struct _fat_ptr _tmp38C=(struct _fat_ptr)({const char*_tmp274=Cyc_Core_get_exn_name(e);_tag_fat(_tmp274,sizeof(char),_get_zero_arr_size_char((void*)_tmp274,1U));});_tmp2C2.f1=_tmp38C;});_tmp2C2;});struct Cyc_String_pa_PrintArg_struct _tmp271=({struct Cyc_String_pa_PrintArg_struct _tmp2C1;_tmp2C1.tag=0,({struct _fat_ptr _tmp38D=(struct _fat_ptr)({const char*_tmp273=Cyc_Core_get_exn_filename();_tag_fat(_tmp273,sizeof(char),_get_zero_arr_size_char((void*)_tmp273,1U));});_tmp2C1.f1=_tmp38D;});_tmp2C1;});struct Cyc_Int_pa_PrintArg_struct _tmp272=({struct Cyc_Int_pa_PrintArg_struct _tmp2C0;_tmp2C0.tag=1,({unsigned long _tmp38E=(unsigned long)Cyc_Core_get_exn_lineno();_tmp2C0.f1=_tmp38E;});_tmp2C0;});void*_tmp26E[3];_tmp26E[0]=& _tmp270,_tmp26E[1]=& _tmp271,_tmp26E[2]=& _tmp272;({struct Cyc___cycFILE*_tmp390=Cyc_stderr;struct _fat_ptr _tmp38F=({const char*_tmp26F="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu";_tag_fat(_tmp26F,sizeof(char),131U);});Cyc_fprintf(_tmp390,_tmp38F,_tag_fat(_tmp26E,sizeof(void*),3));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;};}}}{
# 1410
struct _fat_ptr outfile_str=({const char*_tmp286="";_tag_fat(_tmp286,sizeof(char),1U);});
if(Cyc_output_file != 0)
outfile_str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp277=({struct Cyc_String_pa_PrintArg_struct _tmp2C3;_tmp2C3.tag=0,({struct _fat_ptr _tmp391=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(*((struct _fat_ptr*)_check_null(Cyc_output_file))));_tmp2C3.f1=_tmp391;});_tmp2C3;});void*_tmp275[1];_tmp275[0]=& _tmp277;({struct _fat_ptr _tmp392=({const char*_tmp276=" -o %s";_tag_fat(_tmp276,sizeof(char),7U);});Cyc_aprintf(_tmp392,_tag_fat(_tmp275,sizeof(void*),1));});});{
# 1414
struct _fat_ptr _tmp278=({struct Cyc_String_pa_PrintArg_struct _tmp280=({struct Cyc_String_pa_PrintArg_struct _tmp2CA;_tmp2CA.tag=0,_tmp2CA.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_tmp2CA;});struct Cyc_String_pa_PrintArg_struct _tmp281=({struct Cyc_String_pa_PrintArg_struct _tmp2C9;_tmp2C9.tag=0,_tmp2C9.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_tmp2C9;});struct Cyc_String_pa_PrintArg_struct _tmp282=({struct Cyc_String_pa_PrintArg_struct _tmp2C8;_tmp2C8.tag=0,_tmp2C8.f1=(struct _fat_ptr)((struct _fat_ptr)ccargs_string);_tmp2C8;});struct Cyc_String_pa_PrintArg_struct _tmp283=({struct Cyc_String_pa_PrintArg_struct _tmp2C7;_tmp2C7.tag=0,_tmp2C7.f1=(struct _fat_ptr)((struct _fat_ptr)outfile_str);_tmp2C7;});struct Cyc_String_pa_PrintArg_struct _tmp284=({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0,_tmp2C6.f1=(struct _fat_ptr)((struct _fat_ptr)stdlib_string);_tmp2C6;});struct Cyc_String_pa_PrintArg_struct _tmp285=({struct Cyc_String_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=0,_tmp2C5.f1=(struct _fat_ptr)((struct _fat_ptr)libargs_string);_tmp2C5;});void*_tmp27E[6];_tmp27E[0]=& _tmp280,_tmp27E[1]=& _tmp281,_tmp27E[2]=& _tmp282,_tmp27E[3]=& _tmp283,_tmp27E[4]=& _tmp284,_tmp27E[5]=& _tmp285;({struct _fat_ptr _tmp393=({const char*_tmp27F="%s %s %s%s %s%s";_tag_fat(_tmp27F,sizeof(char),16U);});Cyc_aprintf(_tmp393,_tag_fat(_tmp27E,sizeof(void*),6));});});struct _fat_ptr cccmd=_tmp278;
# 1419
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _tmp27B=({struct Cyc_String_pa_PrintArg_struct _tmp2C4;_tmp2C4.tag=0,_tmp2C4.f1=(struct _fat_ptr)((struct _fat_ptr)cccmd);_tmp2C4;});void*_tmp279[1];_tmp279[0]=& _tmp27B;({struct Cyc___cycFILE*_tmp395=Cyc_stderr;struct _fat_ptr _tmp394=({const char*_tmp27A="%s\n";_tag_fat(_tmp27A,sizeof(char),4U);});Cyc_fprintf(_tmp395,_tmp394,_tag_fat(_tmp279,sizeof(void*),1));});});Cyc_fflush(Cyc_stderr);}
if(system((const char*)_check_null(_untag_fat_ptr(cccmd,sizeof(char),1U)))!= 0){
({void*_tmp27C=0U;({struct Cyc___cycFILE*_tmp397=Cyc_stderr;struct _fat_ptr _tmp396=({const char*_tmp27D="Error: C compiler failed\n";_tag_fat(_tmp27D,sizeof(char),26U);});Cyc_fprintf(_tmp397,_tmp396,_tag_fat(_tmp27C,sizeof(void*),0));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}
# 1426
Cyc_remove_cfiles();
# 1428
return Cyc_compile_failure?1: 0;}}}}}}}}}
