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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 280 "core.h"
extern void Cyc_Core_rethrow(void*);
# 284
extern const char*Cyc_Core_get_exn_name(void*);
# 286
extern const char*Cyc_Core_get_exn_filename (void);
# 293
extern int Cyc_Core_get_exn_lineno (void);struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 54 "list.h"
extern struct Cyc_List_List*Cyc_List_list(struct _fat_ptr);
# 76
extern struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 172
extern struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x);
# 178
extern struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x);
# 184
extern struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 391
extern struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*x);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr,struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
# 66 "arg.h"
extern void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 69
extern int Cyc_Arg_current;
# 71
extern void Cyc_Arg_parse(struct Cyc_List_List*specs,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr args);struct Cyc___cycFILE;
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
extern int gettimeofday(struct Cyc_timeval*fp,void*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
# 51
extern int Cyc_strncmp(struct _fat_ptr s1,struct _fat_ptr s2,unsigned long len);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);
# 64
extern struct _fat_ptr Cyc_strconcat_l(struct Cyc_List_List*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);
# 104 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr src);
# 109
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int ofs,unsigned long n);
# 120
extern struct _fat_ptr Cyc_strchr(struct _fat_ptr s,char c);
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
# 158 "absyn.h"
enum Cyc_Absyn_Scope{Cyc_Absyn_Static =0U,Cyc_Absyn_Abstract =1U,Cyc_Absyn_Public =2U,Cyc_Absyn_Extern =3U,Cyc_Absyn_ExternC =4U,Cyc_Absyn_Register =5U};struct Cyc_Absyn_Tqual{int print_const: 1;int q_volatile: 1;int q_restrict: 1;int real_const: 1;unsigned loc;};
# 179
enum Cyc_Absyn_Size_of{Cyc_Absyn_Char_sz =0U,Cyc_Absyn_Short_sz =1U,Cyc_Absyn_Int_sz =2U,Cyc_Absyn_Long_sz =3U,Cyc_Absyn_LongLong_sz =4U};
enum Cyc_Absyn_Sign{Cyc_Absyn_Signed =0U,Cyc_Absyn_Unsigned =1U,Cyc_Absyn_None =2U};
enum Cyc_Absyn_AggrKind{Cyc_Absyn_StructA =0U,Cyc_Absyn_UnionA =1U};
# 184
enum Cyc_Absyn_AliasQual{Cyc_Absyn_Aliasable =0U,Cyc_Absyn_Unique =1U,Cyc_Absyn_Top =2U};
# 189
enum Cyc_Absyn_KindQual{Cyc_Absyn_AnyKind =0U,Cyc_Absyn_MemKind =1U,Cyc_Absyn_BoxKind =2U,Cyc_Absyn_RgnKind =3U,Cyc_Absyn_EffKind =4U,Cyc_Absyn_IntKind =5U,Cyc_Absyn_BoolKind =6U,Cyc_Absyn_PtrBndKind =7U};struct Cyc_Absyn_Kind{enum Cyc_Absyn_KindQual kind;enum Cyc_Absyn_AliasQual aliasqual;};struct Cyc_Absyn_Eq_kb_Absyn_KindBound_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Absyn_Unknown_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;};struct Cyc_Absyn_Less_kb_Absyn_KindBound_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_Tvar{struct _fat_ptr*name;int identity;void*kind;};struct Cyc_Absyn_PtrLoc{unsigned ptr_loc;unsigned rgn_loc;unsigned zt_loc;};struct Cyc_Absyn_PtrAtts{void*rgn;void*nullable;void*bounds;void*zero_term;struct Cyc_Absyn_PtrLoc*ptrloc;};struct Cyc_Absyn_PtrInfo{void*elt_type;struct Cyc_Absyn_Tqual elt_tq;struct Cyc_Absyn_PtrAtts ptr_atts;};struct Cyc_Absyn_VarargInfo{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;int inject;};struct Cyc_Absyn_FnInfo{struct Cyc_List_List*tvars;void*effect;struct Cyc_Absyn_Tqual ret_tqual;void*ret_type;struct Cyc_List_List*args;int c_varargs;struct Cyc_Absyn_VarargInfo*cyc_varargs;struct Cyc_List_List*rgn_po;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;struct Cyc_List_List*requires_relns;struct Cyc_Absyn_Exp*ensures_clause;struct Cyc_List_List*ensures_relns;struct Cyc_Absyn_Vardecl*return_value;};struct Cyc_Absyn_UnknownDatatypeInfo{struct _tuple0*name;int is_extensible;};struct _union_DatatypeInfo_UnknownDatatype{int tag;struct Cyc_Absyn_UnknownDatatypeInfo val;};struct _union_DatatypeInfo_KnownDatatype{int tag;struct Cyc_Absyn_Datatypedecl**val;};union Cyc_Absyn_DatatypeInfo{struct _union_DatatypeInfo_UnknownDatatype UnknownDatatype;struct _union_DatatypeInfo_KnownDatatype KnownDatatype;};struct Cyc_Absyn_UnknownDatatypeFieldInfo{struct _tuple0*datatype_name;struct _tuple0*field_name;int is_extensible;};struct _union_DatatypeFieldInfo_UnknownDatatypefield{int tag;struct Cyc_Absyn_UnknownDatatypeFieldInfo val;};struct _tuple1{struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;};struct _union_DatatypeFieldInfo_KnownDatatypefield{int tag;struct _tuple1 val;};union Cyc_Absyn_DatatypeFieldInfo{struct _union_DatatypeFieldInfo_UnknownDatatypefield UnknownDatatypefield;struct _union_DatatypeFieldInfo_KnownDatatypefield KnownDatatypefield;};struct _tuple2{enum Cyc_Absyn_AggrKind f1;struct _tuple0*f2;struct Cyc_Core_Opt*f3;};struct _union_AggrInfo_UnknownAggr{int tag;struct _tuple2 val;};struct _union_AggrInfo_KnownAggr{int tag;struct Cyc_Absyn_Aggrdecl**val;};union Cyc_Absyn_AggrInfo{struct _union_AggrInfo_UnknownAggr UnknownAggr;struct _union_AggrInfo_KnownAggr KnownAggr;};struct Cyc_Absyn_ArrayInfo{void*elt_type;struct Cyc_Absyn_Tqual tq;struct Cyc_Absyn_Exp*num_elts;void*zero_term;unsigned zt_loc;};struct Cyc_Absyn_Aggr_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Enum_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Datatype_td_Absyn_Raw_typedecl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_TypeDecl{void*r;unsigned loc;};struct Cyc_Absyn_VoidCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_IntCon_Absyn_TyCon_struct{int tag;enum Cyc_Absyn_Sign f1;enum Cyc_Absyn_Size_of f2;};struct Cyc_Absyn_FloatCon_Absyn_TyCon_struct{int tag;int f1;};struct Cyc_Absyn_RgnHandleCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TagCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_HeapCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_UniqueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RefCntCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_AccessCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_JoinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_RgnsCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_TrueCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FalseCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_ThinCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_FatCon_Absyn_TyCon_struct{int tag;};struct Cyc_Absyn_EnumCon_Absyn_TyCon_struct{int tag;struct _tuple0*f1;struct Cyc_Absyn_Enumdecl*f2;};struct Cyc_Absyn_AnonEnumCon_Absyn_TyCon_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_BuiltinCon_Absyn_TyCon_struct{int tag;struct _fat_ptr f1;struct Cyc_Absyn_Kind*f2;};struct Cyc_Absyn_DatatypeCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeInfo f1;};struct Cyc_Absyn_DatatypeFieldCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_DatatypeFieldInfo f1;};struct Cyc_Absyn_AggrCon_Absyn_TyCon_struct{int tag;union Cyc_Absyn_AggrInfo f1;};struct Cyc_Absyn_AppType_Absyn_Type_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Evar_Absyn_Type_struct{int tag;struct Cyc_Core_Opt*f1;void*f2;int f3;struct Cyc_Core_Opt*f4;};struct Cyc_Absyn_VarType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Absyn_PointerType_Absyn_Type_struct{int tag;struct Cyc_Absyn_PtrInfo f1;};struct Cyc_Absyn_ArrayType_Absyn_Type_struct{int tag;struct Cyc_Absyn_ArrayInfo f1;};struct Cyc_Absyn_FnType_Absyn_Type_struct{int tag;struct Cyc_Absyn_FnInfo f1;};struct Cyc_Absyn_TupleType_Absyn_Type_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_AnonAggrType_Absyn_Type_struct{int tag;enum Cyc_Absyn_AggrKind f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_TypedefType_Absyn_Type_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_Typedefdecl*f3;void*f4;};struct Cyc_Absyn_ValueofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_TypeDeclType_Absyn_Type_struct{int tag;struct Cyc_Absyn_TypeDecl*f1;void**f2;};struct Cyc_Absyn_TypeofType_Absyn_Type_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_NoTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;unsigned f2;};struct Cyc_Absyn_WithTypes_Absyn_Funcparams_struct{int tag;struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;struct Cyc_Absyn_Exp*f6;struct Cyc_Absyn_Exp*f7;};
# 414 "absyn.h"
enum Cyc_Absyn_Format_Type{Cyc_Absyn_Printf_ft =0U,Cyc_Absyn_Scanf_ft =1U};struct Cyc_Absyn_Regparm_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Stdcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Cdecl_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Fastcall_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Noreturn_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Const_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Aligned_att_Absyn_Attribute_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Packed_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Section_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Nocommon_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Shared_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Unused_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Weak_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllimport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Dllexport_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_instrument_function_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Constructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Destructor_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_No_check_memory_usage_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Format_att_Absyn_Attribute_struct{int tag;enum Cyc_Absyn_Format_Type f1;int f2;int f3;};struct Cyc_Absyn_Initializes_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Noliveunique_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Consume_att_Absyn_Attribute_struct{int tag;int f1;};struct Cyc_Absyn_Pure_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Mode_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Alias_att_Absyn_Attribute_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Always_inline_att_Absyn_Attribute_struct{int tag;};struct Cyc_Absyn_Carray_mod_Absyn_Type_modifier_struct{int tag;void*f1;unsigned f2;};struct Cyc_Absyn_ConstArray_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;unsigned f3;};struct Cyc_Absyn_Pointer_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_Absyn_PtrAtts f1;struct Cyc_Absyn_Tqual f2;};struct Cyc_Absyn_Function_mod_Absyn_Type_modifier_struct{int tag;void*f1;};struct Cyc_Absyn_TypeParams_mod_Absyn_Type_modifier_struct{int tag;struct Cyc_List_List*f1;unsigned f2;int f3;};struct Cyc_Absyn_Attributes_mod_Absyn_Type_modifier_struct{int tag;unsigned f1;struct Cyc_List_List*f2;};struct _union_Cnst_Null_c{int tag;int val;};struct _tuple3{enum Cyc_Absyn_Sign f1;char f2;};struct _union_Cnst_Char_c{int tag;struct _tuple3 val;};struct _union_Cnst_Wchar_c{int tag;struct _fat_ptr val;};struct _tuple4{enum Cyc_Absyn_Sign f1;short f2;};struct _union_Cnst_Short_c{int tag;struct _tuple4 val;};struct _tuple5{enum Cyc_Absyn_Sign f1;int f2;};struct _union_Cnst_Int_c{int tag;struct _tuple5 val;};struct _tuple6{enum Cyc_Absyn_Sign f1;long long f2;};struct _union_Cnst_LongLong_c{int tag;struct _tuple6 val;};struct _tuple7{struct _fat_ptr f1;int f2;};struct _union_Cnst_Float_c{int tag;struct _tuple7 val;};struct _union_Cnst_String_c{int tag;struct _fat_ptr val;};struct _union_Cnst_Wstring_c{int tag;struct _fat_ptr val;};union Cyc_Absyn_Cnst{struct _union_Cnst_Null_c Null_c;struct _union_Cnst_Char_c Char_c;struct _union_Cnst_Wchar_c Wchar_c;struct _union_Cnst_Short_c Short_c;struct _union_Cnst_Int_c Int_c;struct _union_Cnst_LongLong_c LongLong_c;struct _union_Cnst_Float_c Float_c;struct _union_Cnst_String_c String_c;struct _union_Cnst_Wstring_c Wstring_c;};
# 503
enum Cyc_Absyn_Primop{Cyc_Absyn_Plus =0U,Cyc_Absyn_Times =1U,Cyc_Absyn_Minus =2U,Cyc_Absyn_Div =3U,Cyc_Absyn_Mod =4U,Cyc_Absyn_Eq =5U,Cyc_Absyn_Neq =6U,Cyc_Absyn_Gt =7U,Cyc_Absyn_Lt =8U,Cyc_Absyn_Gte =9U,Cyc_Absyn_Lte =10U,Cyc_Absyn_Not =11U,Cyc_Absyn_Bitnot =12U,Cyc_Absyn_Bitand =13U,Cyc_Absyn_Bitor =14U,Cyc_Absyn_Bitxor =15U,Cyc_Absyn_Bitlshift =16U,Cyc_Absyn_Bitlrshift =17U,Cyc_Absyn_Numelts =18U};
# 510
enum Cyc_Absyn_Incrementor{Cyc_Absyn_PreInc =0U,Cyc_Absyn_PostInc =1U,Cyc_Absyn_PreDec =2U,Cyc_Absyn_PostDec =3U};struct Cyc_Absyn_VarargCallInfo{int num_varargs;struct Cyc_List_List*injectors;struct Cyc_Absyn_VarargInfo*vai;};struct Cyc_Absyn_StructField_Absyn_OffsetofField_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_TupleIndex_Absyn_OffsetofField_struct{int tag;unsigned f1;};
# 528
enum Cyc_Absyn_Coercion{Cyc_Absyn_Unknown_coercion =0U,Cyc_Absyn_No_coercion =1U,Cyc_Absyn_Null_to_NonNull =2U,Cyc_Absyn_Other_coercion =3U};struct Cyc_Absyn_MallocInfo{int is_calloc;struct Cyc_Absyn_Exp*rgn;void**elt_type;struct Cyc_Absyn_Exp*num_elts;int fat_result;int inline_call;};struct Cyc_Absyn_Const_e_Absyn_Raw_exp_struct{int tag;union Cyc_Absyn_Cnst f1;};struct Cyc_Absyn_Var_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Pragma_e_Absyn_Raw_exp_struct{int tag;struct _fat_ptr f1;};struct Cyc_Absyn_Primop_e_Absyn_Raw_exp_struct{int tag;enum Cyc_Absyn_Primop f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_AssignOp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Increment_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;enum Cyc_Absyn_Incrementor f2;};struct Cyc_Absyn_Conditional_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_And_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Or_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_SeqExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_FnCall_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;struct Cyc_Absyn_VarargCallInfo*f3;int f4;};struct Cyc_Absyn_Throw_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;};struct Cyc_Absyn_NoInstantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Instantiate_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Cast_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Exp*f2;int f3;enum Cyc_Absyn_Coercion f4;};struct Cyc_Absyn_Address_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_New_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Sizeoftype_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Sizeofexp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Offsetof_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Deref_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_AggrMember_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_AggrArrow_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;int f3;int f4;};struct Cyc_Absyn_Subscript_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_Tuple_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct _tuple8{struct _fat_ptr*f1;struct Cyc_Absyn_Tqual f2;void*f3;};struct Cyc_Absyn_CompoundLit_e_Absyn_Raw_exp_struct{int tag;struct _tuple8*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Array_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Comprehension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;int f4;};struct Cyc_Absyn_ComprehensionNoinit_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;int f3;};struct Cyc_Absyn_Aggregate_e_Absyn_Raw_exp_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct Cyc_Absyn_Aggrdecl*f4;};struct Cyc_Absyn_AnonStruct_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Datatype_e_Absyn_Raw_exp_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Datatypedecl*f2;struct Cyc_Absyn_Datatypefield*f3;};struct Cyc_Absyn_Enum_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_e_Absyn_Raw_exp_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_Malloc_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_MallocInfo f1;};struct Cyc_Absyn_Swap_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Exp*f2;};struct Cyc_Absyn_UnresolvedMem_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Core_Opt*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_StmtExp_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Absyn_Tagcheck_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _fat_ptr*f2;};struct Cyc_Absyn_Valueof_e_Absyn_Raw_exp_struct{int tag;void*f1;};struct Cyc_Absyn_Asm_e_Absyn_Raw_exp_struct{int tag;int f1;struct _fat_ptr f2;struct Cyc_List_List*f3;struct Cyc_List_List*f4;struct Cyc_List_List*f5;};struct Cyc_Absyn_Extension_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Assert_e_Absyn_Raw_exp_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Exp{void*topt;void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Skip_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Exp_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Seq_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Return_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_IfThenElse_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;struct Cyc_Absyn_Stmt*f3;};struct _tuple9{struct Cyc_Absyn_Exp*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_While_s_Absyn_Raw_stmt_struct{int tag;struct _tuple9 f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Break_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Continue_s_Absyn_Raw_stmt_struct{int tag;};struct Cyc_Absyn_Goto_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;};struct Cyc_Absyn_For_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct _tuple9 f2;struct _tuple9 f3;struct Cyc_Absyn_Stmt*f4;};struct Cyc_Absyn_Switch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Exp*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Fallthru_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_List_List*f1;struct Cyc_Absyn_Switch_clause**f2;};struct Cyc_Absyn_Decl_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Decl*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Label_s_Absyn_Raw_stmt_struct{int tag;struct _fat_ptr*f1;struct Cyc_Absyn_Stmt*f2;};struct Cyc_Absyn_Do_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct _tuple9 f2;};struct Cyc_Absyn_TryCatch_s_Absyn_Raw_stmt_struct{int tag;struct Cyc_Absyn_Stmt*f1;struct Cyc_List_List*f2;void*f3;};struct Cyc_Absyn_Stmt{void*r;unsigned loc;void*annot;};struct Cyc_Absyn_Wild_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Var_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_AliasVar_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Reference_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Vardecl*f1;struct Cyc_Absyn_Pat*f2;};struct Cyc_Absyn_TagInt_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;};struct Cyc_Absyn_Tuple_p_Absyn_Raw_pat_struct{int tag;struct Cyc_List_List*f1;int f2;};struct Cyc_Absyn_Pointer_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Pat*f1;};struct Cyc_Absyn_Aggr_p_Absyn_Raw_pat_struct{int tag;union Cyc_Absyn_AggrInfo*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Datatype_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;struct Cyc_Absyn_Datatypefield*f2;struct Cyc_List_List*f3;int f4;};struct Cyc_Absyn_Null_p_Absyn_Raw_pat_struct{int tag;};struct Cyc_Absyn_Int_p_Absyn_Raw_pat_struct{int tag;enum Cyc_Absyn_Sign f1;int f2;};struct Cyc_Absyn_Char_p_Absyn_Raw_pat_struct{int tag;char f1;};struct Cyc_Absyn_Float_p_Absyn_Raw_pat_struct{int tag;struct _fat_ptr f1;int f2;};struct Cyc_Absyn_Enum_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_AnonEnum_p_Absyn_Raw_pat_struct{int tag;void*f1;struct Cyc_Absyn_Enumfield*f2;};struct Cyc_Absyn_UnknownId_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_UnknownCall_p_Absyn_Raw_pat_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;int f3;};struct Cyc_Absyn_Exp_p_Absyn_Raw_pat_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_Pat{void*r;void*topt;unsigned loc;};struct Cyc_Absyn_Switch_clause{struct Cyc_Absyn_Pat*pattern;struct Cyc_Core_Opt*pat_vars;struct Cyc_Absyn_Exp*where_clause;struct Cyc_Absyn_Stmt*body;unsigned loc;};struct Cyc_Absyn_Unresolved_b_Absyn_Binding_struct{int tag;struct _tuple0*f1;};struct Cyc_Absyn_Global_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Funname_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Param_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Local_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Pat_b_Absyn_Binding_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Vardecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;unsigned varloc;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*initializer;void*rgn;struct Cyc_List_List*attributes;int escapes;int is_proto;};struct Cyc_Absyn_Fndecl{enum Cyc_Absyn_Scope sc;int is_inline;struct _tuple0*name;struct Cyc_Absyn_Stmt*body;struct Cyc_Absyn_FnInfo i;void*cached_type;struct Cyc_Core_Opt*param_vardecls;struct Cyc_Absyn_Vardecl*fn_vardecl;enum Cyc_Absyn_Scope orig_scope;};struct Cyc_Absyn_Aggrfield{struct _fat_ptr*name;struct Cyc_Absyn_Tqual tq;void*type;struct Cyc_Absyn_Exp*width;struct Cyc_List_List*attributes;struct Cyc_Absyn_Exp*requires_clause;};struct Cyc_Absyn_AggrdeclImpl{struct Cyc_List_List*exist_vars;struct Cyc_List_List*rgn_po;struct Cyc_List_List*fields;int tagged;};struct Cyc_Absyn_Aggrdecl{enum Cyc_Absyn_AggrKind kind;enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Absyn_AggrdeclImpl*impl;struct Cyc_List_List*attributes;int expected_mem_kind;};struct Cyc_Absyn_Datatypefield{struct _tuple0*name;struct Cyc_List_List*typs;unsigned loc;enum Cyc_Absyn_Scope sc;};struct Cyc_Absyn_Datatypedecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*fields;int is_extensible;};struct Cyc_Absyn_Enumfield{struct _tuple0*name;struct Cyc_Absyn_Exp*tag;unsigned loc;};struct Cyc_Absyn_Enumdecl{enum Cyc_Absyn_Scope sc;struct _tuple0*name;struct Cyc_Core_Opt*fields;};struct Cyc_Absyn_Typedefdecl{struct _tuple0*name;struct Cyc_Absyn_Tqual tq;struct Cyc_List_List*tvs;struct Cyc_Core_Opt*kind;void*defn;struct Cyc_List_List*atts;int extern_c;};struct Cyc_Absyn_Var_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Absyn_Fn_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Fndecl*f1;};struct Cyc_Absyn_Let_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Pat*f1;struct Cyc_Core_Opt*f2;struct Cyc_Absyn_Exp*f3;void*f4;};struct Cyc_Absyn_Letv_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Absyn_Region_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Tvar*f1;struct Cyc_Absyn_Vardecl*f2;struct Cyc_Absyn_Exp*f3;};struct Cyc_Absyn_Aggr_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Absyn_Datatype_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Datatypedecl*f1;};struct Cyc_Absyn_Enum_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Enumdecl*f1;};struct Cyc_Absyn_Typedef_d_Absyn_Raw_decl_struct{int tag;struct Cyc_Absyn_Typedefdecl*f1;};struct Cyc_Absyn_Namespace_d_Absyn_Raw_decl_struct{int tag;struct _fat_ptr*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_Using_d_Absyn_Raw_decl_struct{int tag;struct _tuple0*f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternC_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;};struct _tuple10{unsigned f1;struct Cyc_List_List*f2;};struct Cyc_Absyn_ExternCinclude_d_Absyn_Raw_decl_struct{int tag;struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;struct _tuple10*f4;};struct Cyc_Absyn_Porton_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Portoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempeston_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Tempestoff_d_Absyn_Raw_decl_struct{int tag;};struct Cyc_Absyn_Decl{void*r;unsigned loc;};struct Cyc_Absyn_ArrayElement_Absyn_Designator_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Absyn_FieldName_Absyn_Designator_struct{int tag;struct _fat_ptr*f1;};extern char Cyc_Absyn_EmptyAnnot[11U];struct Cyc_Absyn_EmptyAnnot_Absyn_AbsynAnnot_struct{char*tag;};
# 1163 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1165
extern int Cyc_Absyn_no_regions;extern char Cyc_Lexing_Error[6U];struct Cyc_Lexing_Error_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};struct Cyc_Lexing_lex_tables{struct _fat_ptr lex_base;struct _fat_ptr lex_backtrk;struct _fat_ptr lex_default;struct _fat_ptr lex_trans;struct _fat_ptr lex_check;};
# 28 "parse.h"
extern struct Cyc_List_List*Cyc_Parse_parse_file(struct Cyc___cycFILE*f);
extern int Cyc_Parse_no_register;extern char Cyc_Parse_Exit[5U];struct Cyc_Parse_Exit_exn_struct{char*tag;};struct Cyc_FlatList{struct Cyc_FlatList*tl;void*hd[0U] __attribute__((aligned )) ;};struct Cyc_Type_specifier{int Signed_spec: 1;int Unsigned_spec: 1;int Short_spec: 1;int Long_spec: 1;int Long_Long_spec: 1;int Valid_type_spec: 1;void*Type_spec;unsigned loc;};struct Cyc_Declarator{struct _tuple0*id;struct Cyc_List_List*tms;};struct _tuple12{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;};struct _tuple11{struct _tuple11*tl;struct _tuple12 hd  __attribute__((aligned )) ;};
# 52
enum Cyc_Storage_class{Cyc_Typedef_sc =0U,Cyc_Extern_sc =1U,Cyc_ExternC_sc =2U,Cyc_Static_sc =3U,Cyc_Auto_sc =4U,Cyc_Register_sc =5U,Cyc_Abstract_sc =6U};struct Cyc_Declaration_spec{enum Cyc_Storage_class*sc;struct Cyc_Absyn_Tqual tq;struct Cyc_Type_specifier type_specs;int is_inline;struct Cyc_List_List*attributes;};struct Cyc_Abstractdeclarator{struct Cyc_List_List*tms;};struct Cyc_Numelts_ptrqual_Pointer_qual_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Region_ptrqual_Pointer_qual_struct{int tag;void*f1;};struct Cyc_Thin_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Fat_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Zeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nozeroterm_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Notnull_ptrqual_Pointer_qual_struct{int tag;};struct Cyc_Nullable_ptrqual_Pointer_qual_struct{int tag;};struct _union_YYSTYPE_Int_tok{int tag;union Cyc_Absyn_Cnst val;};struct _union_YYSTYPE_Char_tok{int tag;char val;};struct _union_YYSTYPE_String_tok{int tag;struct _fat_ptr val;};struct _union_YYSTYPE_Stringopt_tok{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_QualId_tok{int tag;struct _tuple0*val;};struct _tuple13{int f1;struct _fat_ptr f2;};struct _union_YYSTYPE_Asm_tok{int tag;struct _tuple13 val;};struct _union_YYSTYPE_Exp_tok{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_Stmt_tok{int tag;struct Cyc_Absyn_Stmt*val;};struct _tuple14{unsigned f1;void*f2;void*f3;};struct _union_YYSTYPE_YY1{int tag;struct _tuple14*val;};struct _union_YYSTYPE_YY2{int tag;void*val;};struct _union_YYSTYPE_YY3{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY4{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY5{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY6{int tag;enum Cyc_Absyn_Primop val;};struct _union_YYSTYPE_YY7{int tag;struct Cyc_Core_Opt*val;};struct _union_YYSTYPE_YY8{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY9{int tag;struct Cyc_Absyn_Pat*val;};struct _tuple15{struct Cyc_List_List*f1;int f2;};struct _union_YYSTYPE_YY10{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY11{int tag;struct Cyc_List_List*val;};struct _tuple16{struct Cyc_List_List*f1;struct Cyc_Absyn_Pat*f2;};struct _union_YYSTYPE_YY12{int tag;struct _tuple16*val;};struct _union_YYSTYPE_YY13{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY14{int tag;struct _tuple15*val;};struct _union_YYSTYPE_YY15{int tag;struct Cyc_Absyn_Fndecl*val;};struct _union_YYSTYPE_YY16{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY17{int tag;struct Cyc_Declaration_spec val;};struct _union_YYSTYPE_YY18{int tag;struct _tuple12 val;};struct _union_YYSTYPE_YY19{int tag;struct _tuple11*val;};struct _union_YYSTYPE_YY20{int tag;enum Cyc_Storage_class*val;};struct _union_YYSTYPE_YY21{int tag;struct Cyc_Type_specifier val;};struct _union_YYSTYPE_YY22{int tag;enum Cyc_Absyn_AggrKind val;};struct _union_YYSTYPE_YY23{int tag;struct Cyc_Absyn_Tqual val;};struct _union_YYSTYPE_YY24{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY25{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY26{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY27{int tag;struct Cyc_Declarator val;};struct _tuple17{struct Cyc_Declarator f1;struct Cyc_Absyn_Exp*f2;struct Cyc_Absyn_Exp*f3;};struct _union_YYSTYPE_YY28{int tag;struct _tuple17*val;};struct _union_YYSTYPE_YY29{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY30{int tag;struct Cyc_Abstractdeclarator val;};struct _union_YYSTYPE_YY31{int tag;int val;};struct _union_YYSTYPE_YY32{int tag;enum Cyc_Absyn_Scope val;};struct _union_YYSTYPE_YY33{int tag;struct Cyc_Absyn_Datatypefield*val;};struct _union_YYSTYPE_YY34{int tag;struct Cyc_List_List*val;};struct _tuple18{struct Cyc_Absyn_Tqual f1;struct Cyc_Type_specifier f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY35{int tag;struct _tuple18 val;};struct _union_YYSTYPE_YY36{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY37{int tag;struct _tuple8*val;};struct _union_YYSTYPE_YY38{int tag;struct Cyc_List_List*val;};struct _tuple19{struct Cyc_List_List*f1;int f2;struct Cyc_Absyn_VarargInfo*f3;void*f4;struct Cyc_List_List*f5;};struct _union_YYSTYPE_YY39{int tag;struct _tuple19*val;};struct _union_YYSTYPE_YY40{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY41{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY42{int tag;void*val;};struct _union_YYSTYPE_YY43{int tag;struct Cyc_Absyn_Kind*val;};struct _union_YYSTYPE_YY44{int tag;void*val;};struct _union_YYSTYPE_YY45{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY46{int tag;void*val;};struct _union_YYSTYPE_YY47{int tag;struct Cyc_Absyn_Enumfield*val;};struct _union_YYSTYPE_YY48{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY49{int tag;void*val;};struct _union_YYSTYPE_YY50{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY51{int tag;void*val;};struct _union_YYSTYPE_YY52{int tag;struct Cyc_List_List*val;};struct _tuple20{struct Cyc_List_List*f1;unsigned f2;};struct _union_YYSTYPE_YY53{int tag;struct _tuple20*val;};struct _union_YYSTYPE_YY54{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY55{int tag;void*val;};struct _union_YYSTYPE_YY56{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY57{int tag;struct Cyc_Absyn_Exp*val;};struct _union_YYSTYPE_YY58{int tag;void*val;};struct _tuple21{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};struct _union_YYSTYPE_YY59{int tag;struct _tuple21*val;};struct _tuple22{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};struct _union_YYSTYPE_YY60{int tag;struct _tuple22*val;};struct _union_YYSTYPE_YY61{int tag;struct Cyc_List_List*val;};struct _union_YYSTYPE_YY62{int tag;struct Cyc_List_List*val;};struct _tuple23{struct _fat_ptr f1;struct Cyc_Absyn_Exp*f2;};struct _union_YYSTYPE_YY63{int tag;struct _tuple23*val;};struct _union_YYSTYPE_YYINITIALSVAL{int tag;int val;};union Cyc_YYSTYPE{struct _union_YYSTYPE_Int_tok Int_tok;struct _union_YYSTYPE_Char_tok Char_tok;struct _union_YYSTYPE_String_tok String_tok;struct _union_YYSTYPE_Stringopt_tok Stringopt_tok;struct _union_YYSTYPE_QualId_tok QualId_tok;struct _union_YYSTYPE_Asm_tok Asm_tok;struct _union_YYSTYPE_Exp_tok Exp_tok;struct _union_YYSTYPE_Stmt_tok Stmt_tok;struct _union_YYSTYPE_YY1 YY1;struct _union_YYSTYPE_YY2 YY2;struct _union_YYSTYPE_YY3 YY3;struct _union_YYSTYPE_YY4 YY4;struct _union_YYSTYPE_YY5 YY5;struct _union_YYSTYPE_YY6 YY6;struct _union_YYSTYPE_YY7 YY7;struct _union_YYSTYPE_YY8 YY8;struct _union_YYSTYPE_YY9 YY9;struct _union_YYSTYPE_YY10 YY10;struct _union_YYSTYPE_YY11 YY11;struct _union_YYSTYPE_YY12 YY12;struct _union_YYSTYPE_YY13 YY13;struct _union_YYSTYPE_YY14 YY14;struct _union_YYSTYPE_YY15 YY15;struct _union_YYSTYPE_YY16 YY16;struct _union_YYSTYPE_YY17 YY17;struct _union_YYSTYPE_YY18 YY18;struct _union_YYSTYPE_YY19 YY19;struct _union_YYSTYPE_YY20 YY20;struct _union_YYSTYPE_YY21 YY21;struct _union_YYSTYPE_YY22 YY22;struct _union_YYSTYPE_YY23 YY23;struct _union_YYSTYPE_YY24 YY24;struct _union_YYSTYPE_YY25 YY25;struct _union_YYSTYPE_YY26 YY26;struct _union_YYSTYPE_YY27 YY27;struct _union_YYSTYPE_YY28 YY28;struct _union_YYSTYPE_YY29 YY29;struct _union_YYSTYPE_YY30 YY30;struct _union_YYSTYPE_YY31 YY31;struct _union_YYSTYPE_YY32 YY32;struct _union_YYSTYPE_YY33 YY33;struct _union_YYSTYPE_YY34 YY34;struct _union_YYSTYPE_YY35 YY35;struct _union_YYSTYPE_YY36 YY36;struct _union_YYSTYPE_YY37 YY37;struct _union_YYSTYPE_YY38 YY38;struct _union_YYSTYPE_YY39 YY39;struct _union_YYSTYPE_YY40 YY40;struct _union_YYSTYPE_YY41 YY41;struct _union_YYSTYPE_YY42 YY42;struct _union_YYSTYPE_YY43 YY43;struct _union_YYSTYPE_YY44 YY44;struct _union_YYSTYPE_YY45 YY45;struct _union_YYSTYPE_YY46 YY46;struct _union_YYSTYPE_YY47 YY47;struct _union_YYSTYPE_YY48 YY48;struct _union_YYSTYPE_YY49 YY49;struct _union_YYSTYPE_YY50 YY50;struct _union_YYSTYPE_YY51 YY51;struct _union_YYSTYPE_YY52 YY52;struct _union_YYSTYPE_YY53 YY53;struct _union_YYSTYPE_YY54 YY54;struct _union_YYSTYPE_YY55 YY55;struct _union_YYSTYPE_YY56 YY56;struct _union_YYSTYPE_YY57 YY57;struct _union_YYSTYPE_YY58 YY58;struct _union_YYSTYPE_YY59 YY59;struct _union_YYSTYPE_YY60 YY60;struct _union_YYSTYPE_YY61 YY61;struct _union_YYSTYPE_YY62 YY62;struct _union_YYSTYPE_YY63 YY63;struct _union_YYSTYPE_YYINITIALSVAL YYINITIALSVAL;};struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Absynpp_Params{int expand_typedefs;int qvar_to_Cids;int add_cyc_prefix;int to_VC;int decls_first;int rewrite_temp_tvars;int print_all_tvars;int print_all_kinds;int print_all_effects;int print_using_stmts;int print_externC_stmts;int print_full_evars;int print_zeroterm;int generate_line_directives;int use_curr_namespace;struct Cyc_List_List*curr_namespace;};
# 54 "absynpp.h"
void Cyc_Absynpp_set_params(struct Cyc_Absynpp_Params*fs);
# 56
extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyc_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_cyci_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_c_params_r;extern struct Cyc_Absynpp_Params Cyc_Absynpp_tc_params_r;
# 58
void Cyc_Absynpp_decllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
# 26 "absyndump.h"
extern void Cyc_Absyndump_set_params(struct Cyc_Absynpp_Params*fs);
extern void Cyc_Absyndump_dumpdecllist2file(struct Cyc_List_List*tdl,struct Cyc___cycFILE*f);
extern void Cyc_Absyndump_dump_interface(struct Cyc_List_List*ds,struct Cyc___cycFILE*f);struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 29 "binding.h"
extern int Cyc_Binding_warn_override;
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};struct Cyc_RgnOrder_RgnPO;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 29 "currgn.h"
struct _fat_ptr Cyc_CurRgn_curr_rgn_name;
# 37
void Cyc_CurRgn_adjust_all(struct Cyc_List_List*tds);struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};
# 128 "tcutil.h"
extern int Cyc_Tcutil_warn_alias_coerce;
# 131
extern int Cyc_Tcutil_warn_region_coerce;
# 29 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 31
void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 35
struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);struct Cyc_Hashtable_Table;
# 35 "toc.h"
extern struct Cyc_List_List*Cyc_Toc_toc(struct Cyc_Hashtable_Table*pop_tables,struct Cyc_List_List*ds);extern char Cyc_Toc_Dest[5U];struct Cyc_Toc_Dest_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 48
void Cyc_Toc_finish (void);
# 26 "remove_aggregates.h"
extern struct Cyc_List_List*Cyc_RemoveAggrs_remove_aggrs(struct Cyc_List_List*decls);
# 27 "toseqc.h"
extern struct Cyc_List_List*Cyc_Toseqc_toseqc(struct Cyc_List_List*decls);
# 27 "tovc.h"
extern int Cyc_Tovc_elim_array_initializers;
extern struct Cyc_List_List*Cyc_Tovc_tovc(struct Cyc_List_List*decls);struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_JumpAnalysis_Jump_Anal_Result{struct Cyc_Hashtable_Table*pop_tables;struct Cyc_Hashtable_Table*succ_tables;struct Cyc_Hashtable_Table*pat_pop_tables;};
# 46 "jump_analysis.h"
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_JumpAnalysis_jump_analysis(struct Cyc_List_List*tds);
# 41 "cf_flowinfo.h"
int Cyc_CfFlowInfo_anal_error;struct Cyc_CfFlowInfo_VarRoot_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_CfFlowInfo_MallocPt_CfFlowInfo_Root_struct{int tag;struct Cyc_Absyn_Exp*f1;void*f2;};struct Cyc_CfFlowInfo_InitParam_CfFlowInfo_Root_struct{int tag;int f1;void*f2;};struct Cyc_CfFlowInfo_Dot_CfFlowInfo_PathCon_struct{int tag;int f1;};struct Cyc_CfFlowInfo_Star_CfFlowInfo_PathCon_struct{int tag;};struct Cyc_CfFlowInfo_Place{void*root;struct Cyc_List_List*path;};
# 74
enum Cyc_CfFlowInfo_InitLevel{Cyc_CfFlowInfo_NoneIL =0U,Cyc_CfFlowInfo_AllIL =1U};extern char Cyc_CfFlowInfo_IsZero[7U];struct Cyc_CfFlowInfo_IsZero_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_CfFlowInfo_NotZero[8U];struct Cyc_CfFlowInfo_NotZero_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};extern char Cyc_CfFlowInfo_UnknownZ[9U];struct Cyc_CfFlowInfo_UnknownZ_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_List_List*f1;};struct _union_AbsLVal_PlaceL{int tag;struct Cyc_CfFlowInfo_Place*val;};struct _union_AbsLVal_UnknownL{int tag;int val;};union Cyc_CfFlowInfo_AbsLVal{struct _union_AbsLVal_PlaceL PlaceL;struct _union_AbsLVal_UnknownL UnknownL;};struct Cyc_CfFlowInfo_UnionRInfo{int is_union;int fieldnum;};struct Cyc_CfFlowInfo_Zero_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_NotZeroAll_CfFlowInfo_AbsRVal_struct{int tag;};struct Cyc_CfFlowInfo_UnknownR_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_Esc_CfFlowInfo_AbsRVal_struct{int tag;enum Cyc_CfFlowInfo_InitLevel f1;};struct Cyc_CfFlowInfo_AddressOf_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_Place*f1;};struct Cyc_CfFlowInfo_UniquePtr_CfFlowInfo_AbsRVal_struct{int tag;void*f1;};struct Cyc_CfFlowInfo_Aggregate_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_CfFlowInfo_UnionRInfo f1;struct _fat_ptr f2;};struct Cyc_CfFlowInfo_Consumed_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Exp*f1;int f2;void*f3;};struct Cyc_CfFlowInfo_NamedLocation_CfFlowInfo_AbsRVal_struct{int tag;struct Cyc_Absyn_Vardecl*f1;void*f2;};struct _union_FlowInfo_BottomFL{int tag;int val;};struct _tuple24{struct Cyc_Dict_Dict f1;struct Cyc_List_List*f2;};struct _union_FlowInfo_ReachableFL{int tag;struct _tuple24 val;};union Cyc_CfFlowInfo_FlowInfo{struct _union_FlowInfo_BottomFL BottomFL;struct _union_FlowInfo_ReachableFL ReachableFL;};struct Cyc_CfFlowInfo_FlowEnv{void*zero;void*notzeroall;void*unknown_none;void*unknown_all;void*esc_none;void*esc_all;struct Cyc_Dict_Dict mt_flowdict;struct Cyc_CfFlowInfo_Place*dummy_place;};
# 32 "new_control_flow.h"
extern void Cyc_NewControlFlow_cf_check(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);
# 37
extern int Cyc_NewControlFlow_warn_lose_unique;
# 27 "vcgen.h"
extern void Cyc_Vcgen_vcgen_topdecls(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*ds);extern char Cyc_InsertChecks_FatBound[9U];struct Cyc_InsertChecks_FatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NoCheck[8U];struct Cyc_InsertChecks_NoCheck_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndFatBound[16U];struct Cyc_InsertChecks_NullAndFatBound_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_NullAndThinBound[17U];struct Cyc_InsertChecks_NullAndThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};extern char Cyc_InsertChecks_NullOnly[9U];struct Cyc_InsertChecks_NullOnly_Absyn_AbsynAnnot_struct{char*tag;};extern char Cyc_InsertChecks_ThinBound[10U];struct Cyc_InsertChecks_ThinBound_Absyn_AbsynAnnot_struct{char*tag;struct Cyc_Absyn_Exp*f1;};
# 39 "insert_checks.h"
extern int Cyc_InsertChecks_warn_bounds_checks;
extern int Cyc_InsertChecks_warn_all_null_deref;
# 42
void Cyc_InsertChecks_insert_checks(struct Cyc_List_List*);struct Cyc_Interface_I;
# 36 "interface.h"
extern struct Cyc_Interface_I*Cyc_Interface_empty (void);
# 45 "interface.h"
extern struct Cyc_Interface_I*Cyc_Interface_final (void);
# 49
extern struct Cyc_Interface_I*Cyc_Interface_extract(struct Cyc_Tcenv_Genv*,struct Cyc_List_List*);struct _tuple25{struct _fat_ptr f1;struct _fat_ptr f2;};
# 57
extern int Cyc_Interface_is_subinterface(struct Cyc_Interface_I*i1,struct Cyc_Interface_I*i2,struct _tuple25*info);
# 72 "interface.h"
extern struct Cyc_Interface_I*Cyc_Interface_get_and_merge_list(struct Cyc_Interface_I*(*get)(void*),struct Cyc_List_List*la,struct Cyc_List_List*linfo);
# 78
extern struct Cyc_Interface_I*Cyc_Interface_parse(struct Cyc___cycFILE*);
# 81
extern void Cyc_Interface_save(struct Cyc_Interface_I*,struct Cyc___cycFILE*);
# 84
extern struct Cyc_Interface_I*Cyc_Interface_load(struct Cyc___cycFILE*);
# 31 "warn.h"
void Cyc_Warn_flush_warnings (void);struct Cyc_Warn_String_Warn_Warg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Warn_Qvar_Warn_Warg_struct{int tag;struct _tuple0*f1;};struct Cyc_Warn_Typ_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_TypOpt_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Exp_Warn_Warg_struct{int tag;struct Cyc_Absyn_Exp*f1;};struct Cyc_Warn_Stmt_Warn_Warg_struct{int tag;struct Cyc_Absyn_Stmt*f1;};struct Cyc_Warn_Aggrdecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Aggrdecl*f1;};struct Cyc_Warn_Tvar_Warn_Warg_struct{int tag;struct Cyc_Absyn_Tvar*f1;};struct Cyc_Warn_KindBound_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Kind_Warn_Warg_struct{int tag;struct Cyc_Absyn_Kind*f1;};struct Cyc_Warn_Attribute_Warn_Warg_struct{int tag;void*f1;};struct Cyc_Warn_Vardecl_Warn_Warg_struct{int tag;struct Cyc_Absyn_Vardecl*f1;};struct Cyc_Warn_Int_Warn_Warg_struct{int tag;int f1;};
# 29 "specsfile.h"
struct _fat_ptr Cyc_Specsfile_target_arch;
void Cyc_Specsfile_set_target_arch(struct _fat_ptr s);
struct Cyc_List_List*Cyc_Specsfile_cyclone_exec_path;
void Cyc_Specsfile_add_cyclone_exec_path(struct _fat_ptr s);
# 34
struct Cyc_List_List*Cyc_Specsfile_read_specs(struct _fat_ptr file);
struct _fat_ptr Cyc_Specsfile_split_specs(struct _fat_ptr cmdline);
struct _fat_ptr Cyc_Specsfile_get_spec(struct Cyc_List_List*specs,struct _fat_ptr spec_name);
struct Cyc_List_List*Cyc_Specsfile_cyclone_arch_path;
struct _fat_ptr Cyc_Specsfile_def_lib_path;
struct _fat_ptr Cyc_Specsfile_parse_b(struct Cyc_List_List*specs,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr argv);
# 44
struct _fat_ptr Cyc_Specsfile_find_in_arch_path(struct _fat_ptr s);
struct _fat_ptr Cyc_Specsfile_find_in_exec_path(struct _fat_ptr s);
# 24 "cyclone.h"
extern int Cyc_Cyclone_tovc_r;
# 26
enum Cyc_Cyclone_C_Compilers{Cyc_Cyclone_Gcc_c =0U,Cyc_Cyclone_Vc_c =1U};
# 32
extern enum Cyc_Cyclone_C_Compilers Cyc_Cyclone_c_compiler;
# 42 "cyclone.cyc"
extern void Cyc_Port_port(struct Cyc_List_List*);
# 45
extern int Cyc_ParseErrors_print_state_and_token;
# 48
extern int Cyc_Lex_compile_for_boot_r;
extern void Cyc_Lex_pos_init (void);
extern void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 59
static int Cyc_pp_r=0;
int Cyc_noexpand_r=0;
static int Cyc_noshake_r=0;
static int Cyc_stop_after_cpp_r=0;
static int Cyc_parseonly_r=0;
static int Cyc_tc_r=0;
static int Cyc_cf_r=0;
static int Cyc_noprint_r=0;
static int Cyc_ic_r=0;
static int Cyc_toc_r=0;
static int Cyc_stop_after_objectfile_r=0;
static int Cyc_stop_after_asmfile_r=0;
static int Cyc_inline_functions_r=0;
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
static int Cyc_vcgen_r=0;
# 93
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _fat_ptr s){
Cyc_ccargs=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));({struct _fat_ptr*_tmp2CD=({struct _fat_ptr*_tmp0=_cycalloc(sizeof(*_tmp0));*_tmp0=s;_tmp0;});_tmp1->hd=_tmp2CD;}),_tmp1->tl=Cyc_ccargs;_tmp1;});}
# 101
void Cyc_set_c_compiler(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2CE=(struct _fat_ptr)s;Cyc_strcmp(_tmp2CE,({const char*_tmp2="vc";_tag_fat(_tmp2,sizeof(char),3U);}));})== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;
Cyc_add_ccarg(({const char*_tmp3="-DVC_C";_tag_fat(_tmp3,sizeof(char),7U);}));}else{
# 106
if(({struct _fat_ptr _tmp2CF=(struct _fat_ptr)s;Cyc_strcmp(_tmp2CF,({const char*_tmp4="gcc";_tag_fat(_tmp4,sizeof(char),4U);}));})== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";_tag_fat(_tmp5,sizeof(char),8U);}));}}}
# 113
void Cyc_set_port_c_code (void){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 120
static struct _fat_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _fat_ptr s){
Cyc_output_file=({struct _fat_ptr*_tmp6=_cycalloc(sizeof(*_tmp6));*_tmp6=s;_tmp6;});}
# 125
static struct _fat_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _fat_ptr s){
Cyc_specified_interface=s;}
# 135
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Cversion;static char _tmp7[1U]="";
# 140
static struct _fat_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1U};
static void Cyc_set_cpp(struct _fat_ptr s){
Cyc_cpp=s;}
# 145
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));({struct _fat_ptr*_tmp2D0=({struct _fat_ptr*_tmp8=_cycalloc(sizeof(*_tmp8));*_tmp8=s;_tmp8;});_tmp9->hd=_tmp2D0;}),_tmp9->tl=Cyc_cppargs;_tmp9;});}
# 150
static void Cyc_add_cpp_and_ccarg(struct _fat_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 155
static void Cyc_add_iprefix(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp297;_tmp297.tag=0U,_tmp297.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp297;});void*_tmpA[1U];_tmpA[0]=& _tmpC;({struct _fat_ptr _tmp2D1=({const char*_tmpB="-iprefix %s";_tag_fat(_tmpB,sizeof(char),12U);});Cyc_aprintf(_tmp2D1,_tag_fat(_tmpA,sizeof(void*),1U));});}));}
# 158
static void Cyc_add_iwithprefix(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmp298;_tmp298.tag=0U,_tmp298.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp298;});void*_tmpD[1U];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp2D2=({const char*_tmpE="-iwithprefix %s";_tag_fat(_tmpE,sizeof(char),16U);});Cyc_aprintf(_tmp2D2,_tag_fat(_tmpD,sizeof(void*),1U));});}));}
# 161
static void Cyc_add_iwithprefixbefore(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp299;_tmp299.tag=0U,_tmp299.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp299;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp2D3=({const char*_tmp11="-iwithprefixbefore %s";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp2D3,_tag_fat(_tmp10,sizeof(void*),1U));});}));}
# 164
static void Cyc_add_isystem(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp29A;_tmp29A.tag=0U,_tmp29A.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp29A;});void*_tmp13[1U];_tmp13[0]=& _tmp15;({struct _fat_ptr _tmp2D4=({const char*_tmp14="-isystem %s";_tag_fat(_tmp14,sizeof(char),12U);});Cyc_aprintf(_tmp2D4,_tag_fat(_tmp13,sizeof(void*),1U));});}));}
# 167
static void Cyc_add_idirafter(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp29B;_tmp29B.tag=0U,_tmp29B.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp29B;});void*_tmp16[1U];_tmp16[0]=& _tmp18;({struct _fat_ptr _tmp2D5=({const char*_tmp17="-idirafter %s";_tag_fat(_tmp17,sizeof(char),14U);});Cyc_aprintf(_tmp2D5,_tag_fat(_tmp16,sizeof(void*),1U));});}));}
# 171
static void Cyc_set_many_errors (void){
Cyc_Position_max_errors=65535;}static char _tmp19[20U]="_see_cycspecs_file_";
# 175
static struct _fat_ptr Cyc_def_inc_path={_tmp19,_tmp19,_tmp19 + 20U};
# 178
static void Cyc_print_version (void){
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp29C;_tmp29C.tag=0U,({struct _fat_ptr _tmp2D6=(struct _fat_ptr)({char*_tmp1D=Cversion;_tag_fat(_tmp1D,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D,1U));});_tmp29C.f1=_tmp2D6;});_tmp29C;});void*_tmp1A[1U];_tmp1A[0]=& _tmp1C;({struct _fat_ptr _tmp2D7=({const char*_tmp1B="The Cyclone compiler, version %s\n";_tag_fat(_tmp1B,sizeof(char),34U);});Cyc_printf(_tmp2D7,_tag_fat(_tmp1A,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0U,({struct _fat_ptr _tmp2D8=(struct _fat_ptr)({char*_tmp21=Carch;_tag_fat(_tmp21,sizeof(char),_get_zero_arr_size_char((void*)_tmp21,1U));});_tmp29D.f1=_tmp2D8;});_tmp29D;});void*_tmp1E[1U];_tmp1E[0]=& _tmp20;({struct _fat_ptr _tmp2D9=({const char*_tmp1F="Compiled for architecture: %s\n";_tag_fat(_tmp1F,sizeof(char),31U);});Cyc_printf(_tmp2D9,_tag_fat(_tmp1E,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0U,({struct _fat_ptr _tmp2DA=(struct _fat_ptr)({char*_tmp25=Cdef_lib_path;_tag_fat(_tmp25,sizeof(char),_get_zero_arr_size_char((void*)_tmp25,1U));});_tmp29E.f1=_tmp2DA;});_tmp29E;});void*_tmp22[1U];_tmp22[0]=& _tmp24;({struct _fat_ptr _tmp2DB=({const char*_tmp23="Standard library directory: %s\n";_tag_fat(_tmp23,sizeof(char),32U);});Cyc_printf(_tmp2DB,_tag_fat(_tmp22,sizeof(void*),1U));});});
# 183
({struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp29F;_tmp29F.tag=0U,_tmp29F.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_def_inc_path);_tmp29F;});void*_tmp26[1U];_tmp26[0]=& _tmp28;({struct _fat_ptr _tmp2DC=({const char*_tmp27="Standard include directory: %s\n";_tag_fat(_tmp27,sizeof(char),32U);});Cyc_printf(_tmp2DC,_tag_fat(_tmp26,sizeof(void*),1U));});});
exit(0);}
# 187
static int Cyc_is_cyclone_sourcefile(struct _fat_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp29;
if(len <= (unsigned long)4)return 0;else{
return({struct _fat_ptr _tmp2DD=(struct _fat_ptr)_fat_ptr_plus(s,sizeof(char),(int)(len - (unsigned long)4));Cyc_strcmp(_tmp2DD,({const char*_tmp2A=".cyc";_tag_fat(_tmp2A,sizeof(char),5U);}));})== 0;}}
# 203 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2DE=(struct _fat_ptr)s;Cyc_strcmp(_tmp2DE,({const char*_tmp2B="-lxml";_tag_fat(_tmp2B,sizeof(char),6U);}));})== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 209
Cyc_add_ccarg(({const char*_tmp2C="-lxml_a";_tag_fat(_tmp2C,sizeof(char),8U);}));}}else{
if(({struct _fat_ptr _tmp2DF=(struct _fat_ptr)s;Cyc_strncmp(_tmp2DF,({const char*_tmp2D="-lcyc";_tag_fat(_tmp2D,sizeof(char),6U);}),5U);})== 0)
Cyc_add_ccarg(s);else{
# 213
if(({struct _fat_ptr _tmp2E0=(struct _fat_ptr)s;Cyc_strncmp(_tmp2E0,({const char*_tmp2E="-lpthread";_tag_fat(_tmp2E,sizeof(char),10U);}),9U);})== 0)
Cyc_pthread_r=1;
Cyc_libargs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr*_tmp2E1=({struct _fat_ptr*_tmp2F=_cycalloc(sizeof(*_tmp2F));*_tmp2F=s;_tmp2F;});_tmp30->hd=_tmp2E1;}),_tmp30->tl=Cyc_libargs;_tmp30;});}}}
# 219
static void Cyc_add_marg(struct _fat_ptr s){
Cyc_add_ccarg(s);}
# 223
static void Cyc_set_save_temps (void){
Cyc_save_temps_r=1;
Cyc_add_ccarg(({const char*_tmp31="-save-temps";_tag_fat(_tmp31,sizeof(char),12U);}));}
# 228
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies (void){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(({const char*_tmp32="-M";_tag_fat(_tmp32,sizeof(char),3U);}));}
# 235
static struct _fat_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _fat_ptr s){
Cyc_dependencies_target=({struct _fat_ptr*_tmp33=_cycalloc(sizeof(*_tmp33));*_tmp33=s;_tmp33;});}
# 240
static void Cyc_set_stop_after_objectfile (void){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(({const char*_tmp34="-c";_tag_fat(_tmp34,sizeof(char),3U);}));}
# 245
static void Cyc_set_nocppprecomp (void){
Cyc_add_cpp_and_ccarg(({const char*_tmp35="-no-cpp-precomp";_tag_fat(_tmp35,sizeof(char),16U);}));}
# 249
static void Cyc_clear_lineno (void){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 253
static void Cyc_set_inline_functions (void){
Cyc_inline_functions_r=1;}
# 256
static void Cyc_set_elim_se (void){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 260
static void Cyc_set_tovc (void){
Cyc_Cyclone_tovc_r=1;
Cyc_add_ccarg(({const char*_tmp36="-DCYC_ANSI_OUTPUT";_tag_fat(_tmp36,sizeof(char),18U);}));
Cyc_set_elim_se();}
# 265
static void Cyc_set_notoseqc (void){
Cyc_toseqc_r=0;}
# 268
static void Cyc_set_noboundschecks (void){
Cyc_add_ccarg(({const char*_tmp37="-DNO_CYC_BOUNDS_CHECKS";_tag_fat(_tmp37,sizeof(char),23U);}));}
# 271
static void Cyc_set_nonullchecks (void){
Cyc_add_ccarg(({const char*_tmp38="-DNO_CYC_NULL_CHECKS";_tag_fat(_tmp38,sizeof(char),21U);}));}
# 274
static void Cyc_set_nochecks (void){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 280
static void Cyc_set_nocyc (void){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(({const char*_tmp39="-DNO_CYC_PREFIX";_tag_fat(_tmp39,sizeof(char),16U);}));}
# 285
static void Cyc_set_pa (void){
Cyc_pa_r=1;
Cyc_add_ccarg(({const char*_tmp3A="-DCYC_REGION_PROFILE";_tag_fat(_tmp3A,sizeof(char),21U);}));
Cyc_add_cpparg(({const char*_tmp3B="-DCYC_REGION_PROFILE";_tag_fat(_tmp3B,sizeof(char),21U);}));}
# 291
static void Cyc_set_pg (void){
Cyc_pg_r=1;
Cyc_add_ccarg(({const char*_tmp3C="-pg";_tag_fat(_tmp3C,sizeof(char),4U);}));}
# 296
static void Cyc_set_stop_after_asmfile (void){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(({const char*_tmp3D="-S";_tag_fat(_tmp3D,sizeof(char),3U);}));}
# 301
static void Cyc_set_all_warnings (void){
Cyc_InsertChecks_warn_bounds_checks=1;
Cyc_InsertChecks_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;
Cyc_Tc_aggressive_warn=1;
Cyc_Binding_warn_override=1;}
# 312
enum Cyc_inputtype{Cyc_DEFAULTINPUT =0U,Cyc_CYCLONEFILE =1U};
# 317
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2E2=(struct _fat_ptr)s;Cyc_strcmp(_tmp2E2,({const char*_tmp3E="cyc";_tag_fat(_tmp3E,sizeof(char),4U);}));})== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
if(({struct _fat_ptr _tmp2E3=(struct _fat_ptr)s;Cyc_strcmp(_tmp2E3,({const char*_tmp3F="none";_tag_fat(_tmp3F,sizeof(char),5U);}));})== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 324
({struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0U,_tmp2A0.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp2A0;});void*_tmp40[1U];_tmp40[0]=& _tmp42;({struct Cyc___cycFILE*_tmp2E5=Cyc_stderr;struct _fat_ptr _tmp2E4=({const char*_tmp41="Input type '%s' not supported\n";_tag_fat(_tmp41,sizeof(char),31U);});Cyc_fprintf(_tmp2E5,_tmp2E4,_tag_fat(_tmp40,sizeof(void*),1U));});});}}}
# 327
struct _fat_ptr Cyc_make_base_filename(struct _fat_ptr s,struct _fat_ptr*output_file){
struct _fat_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);struct _fat_ptr outdir=_tmp43;
struct _fat_ptr _tmp44=Cyc_Filename_chop_extension((struct _fat_ptr)Cyc_Filename_basename(s));struct _fat_ptr base=_tmp44;
# 333
struct _fat_ptr _tmp45=Cyc_strlen((struct _fat_ptr)outdir)> (unsigned long)0?Cyc_Filename_concat((struct _fat_ptr)outdir,(struct _fat_ptr)base): base;struct _fat_ptr basename=_tmp45;
return(struct _fat_ptr)basename;}
# 339
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 341
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _fat_ptr s){
# 344
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
# 348
if((int)Cyc_intype == (int)Cyc_CYCLONEFILE || Cyc_is_cyclone_sourcefile(s)){
# 350
Cyc_cyclone_files=({struct Cyc_List_List*_tmp47=_cycalloc(sizeof(*_tmp47));({struct _fat_ptr*_tmp2E6=({struct _fat_ptr*_tmp46=_cycalloc(sizeof(*_tmp46));*_tmp46=s;_tmp46;});_tmp47->hd=_tmp2E6;}),_tmp47->tl=Cyc_cyclone_files;_tmp47;});{
# 356
struct _fat_ptr _tmp48=Cyc_make_base_filename(s,0);struct _fat_ptr basename=_tmp48;
struct _fat_ptr _tmp49=({struct _fat_ptr _tmp2E7=(struct _fat_ptr)basename;Cyc_strconcat(_tmp2E7,({const char*_tmp4A=".c";_tag_fat(_tmp4A,sizeof(char),3U);}));});struct _fat_ptr cfile=_tmp49;
Cyc_add_ccarg((struct _fat_ptr)cfile);}}else{
# 360
Cyc_add_ccarg(s);}}}
# 365
static int Cyc_assume_gcc_flag=1;struct _tuple26{struct _fat_ptr flag;int optargs;};static char _tmp4B[3U]="-s";static char _tmp4C[3U]="-O";static char _tmp4D[4U]="-O0";static char _tmp4E[4U]="-O2";static char _tmp4F[4U]="-O3";static char _tmp50[21U]="-fomit-frame-pointer";static char _tmp51[13U]="-fno-builtin";static char _tmp52[3U]="-g";static char _tmp53[3U]="-p";static char _tmp54[8U]="-static";
static int Cyc_add_other_flag(struct _fat_ptr s){
static struct _tuple26 known_gcc_flags[10U]={{.flag={_tmp4B,_tmp4B,_tmp4B + 3U},.optargs=0},{.flag={_tmp4C,_tmp4C,_tmp4C + 3U},.optargs=0},{.flag={_tmp4D,_tmp4D,_tmp4D + 4U},.optargs=0},{.flag={_tmp4E,_tmp4E,_tmp4E + 4U},.optargs=0},{.flag={_tmp4F,_tmp4F,_tmp4F + 4U},.optargs=0},{.flag={_tmp50,_tmp50,_tmp50 + 21U},.optargs=0},{.flag={_tmp51,_tmp51,_tmp51 + 13U},.optargs=0},{.flag={_tmp52,_tmp52,_tmp52 + 3U},.optargs=0},{.flag={_tmp53,_tmp53,_tmp53 + 3U},.optargs=0},{.flag={_tmp54,_tmp54,_tmp54 + 8U},.optargs=0}};
# 379
if(Cyc_assume_gcc_flag)
Cyc_add_ccarg(s);else{
# 382
{int i=0;for(0;(unsigned)i < 10U;++ i){
if(!Cyc_strcmp((struct _fat_ptr)(*((struct _tuple26*)_check_known_subscript_notnull(known_gcc_flags,10U,sizeof(struct _tuple26),i))).flag,(struct _fat_ptr)s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(known_gcc_flags[i]).optargs;
break;}}}
# 389
return 0;}
# 391
return 1;}
# 394
static void Cyc_noassume_gcc_flag (void){
Cyc_assume_gcc_flag=0;}
# 398
static void Cyc_remove_file(struct _fat_ptr s){
if(Cyc_save_temps_r)return;else{
remove((const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)));}}
# 405
int Cyc_compile_failure=0;
# 407
struct Cyc___cycFILE*Cyc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){{struct Cyc___cycFILE*_tmp58=Cyc_file_open(filename,mode);_npop_handler(0U);return _tmp58;};_pop_handler();}else{void*_tmp56=(void*)Cyc_Core_get_exn_thrown();void*_tmp59=_tmp56;_LL1: _LL2:
# 411
 Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0U,_tmp2A2.f1=(struct _fat_ptr)((struct _fat_ptr)msg_part);_tmp2A2;});struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp2A1;_tmp2A1.tag=0U,_tmp2A1.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_tmp2A1;});void*_tmp5A[2U];_tmp5A[0]=& _tmp5C,_tmp5A[1]=& _tmp5D;({struct Cyc___cycFILE*_tmp2E9=Cyc_stderr;struct _fat_ptr _tmp2E8=({const char*_tmp5B="\nError: couldn't open %s %s\n";_tag_fat(_tmp5B,sizeof(char),29U);});Cyc_fprintf(_tmp2E9,_tmp2E8,_tag_fat(_tmp5A,sizeof(void*),2U));});});
Cyc_fflush(Cyc_stderr);
return 0;_LL0:;}}}
# 418
void Cyc_endtime(struct Cyc_timeval*start){
struct Cyc_timeval end;
gettimeofday(& end,0);
end.tv_sec -=((struct Cyc_timeval*)_check_null(start))->tv_sec;
end.tv_usec -=start->tv_usec;
if(end.tv_usec < 0)
end.tv_usec +=1000000;
if(Cyc_v_r)({struct Cyc_Int_pa_PrintArg_struct _tmp60=({struct Cyc_Int_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=1U,_tmp2A4.f1=(unsigned long)end.tv_sec;_tmp2A4;});struct Cyc_Int_pa_PrintArg_struct _tmp61=({struct Cyc_Int_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=1U,_tmp2A3.f1=(unsigned long)end.tv_usec;_tmp2A3;});void*_tmp5E[2U];_tmp5E[0]=& _tmp60,_tmp5E[1]=& _tmp61;({struct Cyc___cycFILE*_tmp2EB=Cyc_stderr;struct _fat_ptr _tmp2EA=({const char*_tmp5F="(%d.%06d secs)\n";_tag_fat(_tmp5F,sizeof(char),16U);});Cyc_fprintf(_tmp2EB,_tmp2EA,_tag_fat(_tmp5E,sizeof(void*),2U));});});}
# 428
extern void CYCALLOCPROFILE_mark(const char*s);
# 430
void*Cyc_do_stage(struct _fat_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void(*on_fail)(void*),void*failenv){
# 433
CYCALLOCPROFILE_mark((const char*)_untag_fat_ptr(stage_name,sizeof(char),1U));{
struct Cyc_timeval start;
gettimeofday(& start,0);{
# 437
void*ans;
{struct _handler_cons _tmp62;_push_handler(& _tmp62);{int _tmp64=0;if(setjmp(_tmp62.handler))_tmp64=1;if(!_tmp64){ans=f(env,tds);;_pop_handler();}else{void*_tmp63=(void*)Cyc_Core_get_exn_thrown();void*_tmp65=_tmp63;void*_tmp66;_LL1: _tmp66=_tmp65;_LL2: {void*x=_tmp66;
# 442
if(Cyc_Position_error_p()){
Cyc_compile_failure=1;
on_fail(failenv);
({void*_tmp67=0U;({struct Cyc___cycFILE*_tmp2ED=Cyc_stderr;struct _fat_ptr _tmp2EC=({const char*_tmp68="\nCOMPILATION FAILED!\n";_tag_fat(_tmp68,sizeof(char),22U);});Cyc_fprintf(_tmp2ED,_tmp2EC,_tag_fat(_tmp67,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);}else{
# 449
({struct Cyc_String_pa_PrintArg_struct _tmp6B=({struct Cyc_String_pa_PrintArg_struct _tmp2A5;_tmp2A5.tag=0U,_tmp2A5.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_tmp2A5;});void*_tmp69[1U];_tmp69[0]=& _tmp6B;({struct Cyc___cycFILE*_tmp2EF=Cyc_stderr;struct _fat_ptr _tmp2EE=({const char*_tmp6A="COMPILER STAGE %s\n";_tag_fat(_tmp6A,sizeof(char),19U);});Cyc_fprintf(_tmp2EF,_tmp2EE,_tag_fat(_tmp69,sizeof(void*),1U));});});
on_fail(failenv);}
# 452
Cyc_endtime(& start);
Cyc_Core_rethrow(x);
goto _LL0;}_LL0:;}}}
# 456
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
on_fail(failenv);
({void*_tmp6C=0U;({struct Cyc___cycFILE*_tmp2F1=Cyc_stderr;struct _fat_ptr _tmp2F0=({const char*_tmp6D="\nCOMPILATION FAILED!\n";_tag_fat(_tmp6D,sizeof(char),22U);});Cyc_fprintf(_tmp2F1,_tmp2F0,_tag_fat(_tmp6C,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);
Cyc_endtime(& start);
return ans;}else{
# 465
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _tmp70=({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0U,_tmp2A6.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_tmp2A6;});void*_tmp6E[1U];_tmp6E[0]=& _tmp70;({struct Cyc___cycFILE*_tmp2F3=Cyc_stderr;struct _fat_ptr _tmp2F2=({const char*_tmp6F="%s completed ";_tag_fat(_tmp6F,sizeof(char),14U);});Cyc_fprintf(_tmp2F3,_tmp2F2,_tag_fat(_tmp6E,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
Cyc_endtime(& start);
return ans;}}
# 471
Cyc_endtime(& start);
return ans;}}}
# 475
static void Cyc_ignore(void*x){;}
static void Cyc_remove_fileptr(struct _fat_ptr*s){Cyc_remove_file((struct _fat_ptr)*s);}
# 478
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp71;_push_handler(& _tmp71);{int _tmp73=0;if(setjmp(_tmp71.handler))_tmp73=1;if(!_tmp73){
ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);
# 483
;_pop_handler();}else{void*_tmp72=(void*)Cyc_Core_get_exn_thrown();void*_tmp74=_tmp72;void*_tmp75;if(((struct Cyc_Parse_Exit_exn_struct*)_tmp74)->tag == Cyc_Parse_Exit){_LL1: _LL2:
# 486
 Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;}else{_LL3: _tmp75=_tmp74;_LL4: {void*e=_tmp75;
Cyc_file_close(f);Cyc_Core_rethrow(e);}}_LL0:;}}}
# 489
Cyc_Lex_lex_init(1);
return ans;}}
# 493
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 498
int Cyc_do_adjust(int ignore,struct Cyc_List_List*tds){
Cyc_CurRgn_adjust_all(tds);
return 1;}
# 503
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){
# 505
Cyc_Tc_tc(te,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(te,tds);
return tds;}
# 511
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 513
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 516
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 518
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}
# 522
int Cyc_do_vcgen(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 524
Cyc_Vcgen_vcgen_topdecls(tables,tds);
return 1;}
# 528
int Cyc_do_insert_checks(int ignore,struct Cyc_List_List*tds){
Cyc_InsertChecks_insert_checks(tds);
return 1;}struct _tuple27{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 535
struct Cyc_List_List*Cyc_do_interface(struct _tuple27*params,struct Cyc_List_List*tds){
# 537
struct _tuple27*_tmp76=params;struct Cyc___cycFILE*_tmp79;struct Cyc___cycFILE*_tmp78;struct Cyc_Tcenv_Tenv*_tmp77;_LL1: _tmp77=_tmp76->f1;_tmp78=_tmp76->f2;_tmp79=_tmp76->f3;_LL2: {struct Cyc_Tcenv_Tenv*te=_tmp77;struct Cyc___cycFILE*fi=_tmp78;struct Cyc___cycFILE*fo=_tmp79;
struct Cyc_Interface_I*_tmp7A=Cyc_Interface_extract(te->ae,tds);struct Cyc_Interface_I*i1=_tmp7A;
if(fi == 0)
Cyc_Interface_save(i1,fo);else{
# 542
struct Cyc_Interface_I*_tmp7B=Cyc_Interface_parse(fi);struct Cyc_Interface_I*i0=_tmp7B;
if(!({struct Cyc_Interface_I*_tmp2F7=i0;struct Cyc_Interface_I*_tmp2F6=i1;Cyc_Interface_is_subinterface(_tmp2F7,_tmp2F6,({struct _tuple25*_tmp7E=_cycalloc(sizeof(*_tmp7E));({struct _fat_ptr _tmp2F5=({const char*_tmp7C="written interface";_tag_fat(_tmp7C,sizeof(char),18U);});_tmp7E->f1=_tmp2F5;}),({struct _fat_ptr _tmp2F4=({const char*_tmp7D="maximal interface";_tag_fat(_tmp7D,sizeof(char),18U);});_tmp7E->f2=_tmp2F4;});_tmp7E;}));}))
Cyc_compile_failure=1;else{
# 546
Cyc_Interface_save(i0,fo);}}
# 548
return tds;}}
# 550
void Cyc_interface_fail(struct _tuple27*params){
struct _tuple27*_tmp7F=params;struct Cyc___cycFILE*_tmp81;struct Cyc___cycFILE*_tmp80;_LL1: _tmp80=_tmp7F->f2;_tmp81=_tmp7F->f3;_LL2: {struct Cyc___cycFILE*fi=_tmp80;struct Cyc___cycFILE*fo=_tmp81;
if(fi != 0)
Cyc_file_close(fi);
Cyc_file_close(fo);}}
# 557
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 560
return Cyc_Toc_toc(pops,tds);}
# 562
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveAggrs_remove_aggrs(tds);}
# 566
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){
return Cyc_Toseqc_toseqc(tds);}
# 569
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 574
static struct _fat_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 576
static struct _fat_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 578
static void Cyc_set_cyc_include(struct _fat_ptr f){
Cyc_cyc_include=f;}
# 583
int Cyc_copy_internal_file(struct _fat_ptr file,struct Cyc___cycFILE*out_file){
# 586
if(({char*_tmp2F8=(char*)file.curr;_tmp2F8 == (char*)(_tag_fat(0,0,0)).curr;})){
({void*_tmp82=0U;({struct Cyc___cycFILE*_tmp2FA=Cyc_stderr;struct _fat_ptr _tmp2F9=({const char*_tmp83="Internal error: copy_internal_file called with NULL\n";_tag_fat(_tmp83,sizeof(char),53U);});Cyc_fprintf(_tmp2FA,_tmp2F9,_tag_fat(_tmp82,sizeof(void*),0U));});});
return 1;}{
# 590
struct Cyc___cycFILE*_tmp84=({struct _fat_ptr _tmp2FC=file;struct _fat_ptr _tmp2FB=({const char*_tmp8B="r";_tag_fat(_tmp8B,sizeof(char),2U);});Cyc_try_file_open(_tmp2FC,_tmp2FB,({const char*_tmp8C="internal compiler file";_tag_fat(_tmp8C,sizeof(char),23U);}));});struct Cyc___cycFILE*file_f=_tmp84;
if(file_f == 0)return 1;{
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];({{unsigned _tmp2A9=1024U;unsigned i;for(i=0;i < _tmp2A9;++ i){buf[i]='\000';}}0;});
while(n_read == (unsigned long)1024){
n_read=({struct _fat_ptr _tmp2FD=_tag_fat(buf,sizeof(char),1024U);Cyc_fread(_tmp2FD,1U,1024U,file_f);});
if(n_read != (unsigned long)1024 && !Cyc_feof(file_f)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp87=({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0U,_tmp2A7.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2A7;});void*_tmp85[1U];_tmp85[0]=& _tmp87;({struct Cyc___cycFILE*_tmp2FF=Cyc_stderr;struct _fat_ptr _tmp2FE=({const char*_tmp86="\nError: problem copying %s\n";_tag_fat(_tmp86,sizeof(char),28U);});Cyc_fprintf(_tmp2FF,_tmp2FE,_tag_fat(_tmp85,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return 1;}
# 603
n_written=({struct _fat_ptr _tmp301=_tag_fat(buf,sizeof(char),1024U);unsigned long _tmp300=n_read;Cyc_fwrite(_tmp301,1U,_tmp300,out_file);});
if(n_read != n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp8A=({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0U,_tmp2A8.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2A8;});void*_tmp88[1U];_tmp88[0]=& _tmp8A;({struct Cyc___cycFILE*_tmp303=Cyc_stderr;struct _fat_ptr _tmp302=({const char*_tmp89="\nError: problem copying %s\n";_tag_fat(_tmp89,sizeof(char),28U);});Cyc_fprintf(_tmp303,_tmp302,_tag_fat(_tmp88,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 611
Cyc_fclose(file_f);
return 0;}}}
# 617
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles (void){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file((struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 623
static void Cyc_find_fail(struct _fat_ptr file){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp90=_cycalloc(sizeof(*_tmp90));_tmp90->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp305=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8F=({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0U,_tmp2AA.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp2AA;});void*_tmp8D[1U];_tmp8D[0]=& _tmp8F;({struct _fat_ptr _tmp304=({const char*_tmp8E="Error: can't find internal compiler file %s\n";_tag_fat(_tmp8E,sizeof(char),45U);});Cyc_aprintf(_tmp304,_tag_fat(_tmp8D,sizeof(void*),1U));});});_tmp90->f1=_tmp305;});_tmp90;}));}
# 628
static struct _fat_ptr Cyc_find_in_arch_path(struct _fat_ptr s){
struct _fat_ptr _tmp91=Cyc_Specsfile_find_in_arch_path(s);struct _fat_ptr r=_tmp91;
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}
# 633
static struct _fat_ptr Cyc_find_in_exec_path(struct _fat_ptr s){
struct _fat_ptr _tmp92=Cyc_Specsfile_find_in_exec_path(s);struct _fat_ptr r=_tmp92;
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}struct _tuple28{struct Cyc___cycFILE*f1;struct _fat_ptr f2;};
# 639
struct Cyc_List_List*Cyc_do_print(struct _tuple28*env,struct Cyc_List_List*tds){
# 641
struct _tuple28*_tmp93=env;struct _fat_ptr _tmp95;struct Cyc___cycFILE*_tmp94;_LL1: _tmp94=_tmp93->f1;_tmp95=_tmp93->f2;_LL2: {struct Cyc___cycFILE*out_file=_tmp94;struct _fat_ptr cfile=_tmp95;
struct Cyc_Absynpp_Params _tmp96=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;struct Cyc_Absynpp_Params params_r=_tmp96;
params_r.expand_typedefs=!Cyc_noexpand_r;
params_r.to_VC=Cyc_Cyclone_tovc_r;
params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;
params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;
params_r.print_all_effects=Cyc_print_all_effects_r;
# 652
if(Cyc_inline_functions_r)
({void*_tmp97=0U;({struct Cyc___cycFILE*_tmp307=out_file;struct _fat_ptr _tmp306=({const char*_tmp98="#define _INLINE_FUNCTIONS\n";_tag_fat(_tmp98,sizeof(char),27U);});Cyc_fprintf(_tmp307,_tmp306,_tag_fat(_tmp97,sizeof(void*),0U));});});
# 656
if((!Cyc_parseonly_r && !Cyc_tc_r)&& !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r)
({void*_tmp99=0U;({struct Cyc___cycFILE*_tmp309=out_file;struct _fat_ptr _tmp308=({const char*_tmp9A="#include <setjmp.h>\n";_tag_fat(_tmp9A,sizeof(char),21U);});Cyc_fprintf(_tmp309,_tmp308,_tag_fat(_tmp99,sizeof(void*),0U));});});else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 662
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 664
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& params_r);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 668
Cyc_Absyndump_set_params(& params_r);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 671
Cyc_fflush(out_file);
Cyc_file_close(out_file);
Cyc_cfiles=({struct Cyc_List_List*_tmp9C=_cycalloc(sizeof(*_tmp9C));({struct _fat_ptr*_tmp30A=({struct _fat_ptr*_tmp9B=_cycalloc(sizeof(*_tmp9B));*_tmp9B=cfile;_tmp9B;});_tmp9C->hd=_tmp30A;}),_tmp9C->tl=Cyc_cfiles;_tmp9C;});
return tds;}}
# 676
void Cyc_print_fail(struct _tuple28*env){
struct _tuple28*_tmp9D=env;struct _fat_ptr _tmp9F;struct Cyc___cycFILE*_tmp9E;_LL1: _tmp9E=_tmp9D->f1;_tmp9F=_tmp9D->f2;_LL2: {struct Cyc___cycFILE*out_file=_tmp9E;struct _fat_ptr cfile=_tmp9F;
Cyc_file_close(out_file);
Cyc_cfiles=({struct Cyc_List_List*_tmpA1=_cycalloc(sizeof(*_tmpA1));({struct _fat_ptr*_tmp30C=({struct _fat_ptr*_tmpA0=_cycalloc(sizeof(*_tmpA0));({struct _fat_ptr _tmp30B=Cyc_strdup((struct _fat_ptr)cfile);*_tmpA0=_tmp30B;});_tmpA0;});_tmpA1->hd=_tmp30C;}),_tmpA1->tl=Cyc_cfiles;_tmpA1;});}}
# 683
static struct Cyc_List_List*Cyc_split_by_char(struct _fat_ptr s,char c){
if(({char*_tmp30D=(char*)s.curr;_tmp30D == (char*)(_tag_fat(0,0,0)).curr;}))return 0;{
struct Cyc_List_List*_tmpA2=0;struct Cyc_List_List*result=_tmpA2;
unsigned long _tmpA3=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmpA3;
# 688
while(len > (unsigned long)0){
struct _fat_ptr _tmpA4=Cyc_strchr(s,c);struct _fat_ptr end=_tmpA4;
if(({char*_tmp30E=(char*)end.curr;_tmp30E == (char*)(_tag_fat(0,0,0)).curr;})){
result=({struct Cyc_List_List*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct _fat_ptr*_tmp30F=({struct _fat_ptr*_tmpA5=_cycalloc(sizeof(*_tmpA5));*_tmpA5=s;_tmpA5;});_tmpA6->hd=_tmp30F;}),_tmpA6->tl=result;_tmpA6;});
break;}else{
# 695
result=({struct Cyc_List_List*_tmpA8=_cycalloc(sizeof(*_tmpA8));({struct _fat_ptr*_tmp311=({struct _fat_ptr*_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct _fat_ptr _tmp310=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char)));*_tmpA7=_tmp310;});_tmpA7;});_tmpA8->hd=_tmp311;}),_tmpA8->tl=result;_tmpA8;});
len -=(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char));
s=_fat_ptr_plus(end,sizeof(char),1);}}
# 700
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(result);}}
# 705
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){
# 707
struct Cyc_List_List*_tmpA9=0;struct Cyc_List_List*l=_tmpA9;
for(0;dirs != 0;dirs=dirs->tl){
l=({struct Cyc_List_List*_tmpAB=_cycalloc(sizeof(*_tmpAB));({struct _fat_ptr*_tmp313=({struct _fat_ptr*_tmpAA=_cycalloc(sizeof(*_tmpAA));({struct _fat_ptr _tmp312=(struct _fat_ptr)Cyc_Filename_concat(*((struct _fat_ptr*)dirs->hd),subdir);*_tmpAA=_tmp312;});_tmpAA;});_tmpAB->hd=_tmp313;}),_tmpAB->tl=l;_tmpAB;});}
# 711
l=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(l);
return l;}
# 716
static int Cyc_is_other_special(char c){
char _tmpAC=c;switch(_tmpAC){case 92U: _LL1: _LL2:
 goto _LL4;case 34U: _LL3: _LL4:
 goto _LL6;case 59U: _LL5: _LL6:
 goto _LL8;case 38U: _LL7: _LL8:
 goto _LLA;case 40U: _LL9: _LLA:
 goto _LLC;case 41U: _LLB: _LLC:
 goto _LLE;case 124U: _LLD: _LLE:
 goto _LL10;case 94U: _LLF: _LL10:
 goto _LL12;case 60U: _LL11: _LL12:
 goto _LL14;case 62U: _LL13: _LL14:
 goto _LL16;case 10U: _LL15: _LL16:
# 731
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 738
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmpAD=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmpAD;
# 742
int _tmpAE=0;int single_quotes=_tmpAE;
int _tmpAF=0;int other_special=_tmpAF;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmpB0=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmpB0;
if((int)c == (int)'\'')++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 751
if(single_quotes == 0 && other_special == 0)
return s;
# 755
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmpB1[3U];({struct _fat_ptr*_tmp318=({struct _fat_ptr*_tmpB3=_cycalloc(sizeof(*_tmpB3));({struct _fat_ptr _tmp317=({const char*_tmpB2="'";_tag_fat(_tmpB2,sizeof(char),2U);});*_tmpB3=_tmp317;});_tmpB3;});_tmpB1[0]=_tmp318;}),({struct _fat_ptr*_tmp316=({struct _fat_ptr*_tmpB4=_cycalloc(sizeof(*_tmpB4));*_tmpB4=(struct _fat_ptr)s;_tmpB4;});_tmpB1[1]=_tmp316;}),({struct _fat_ptr*_tmp315=({struct _fat_ptr*_tmpB6=_cycalloc(sizeof(*_tmpB6));({struct _fat_ptr _tmp314=({const char*_tmpB5="'";_tag_fat(_tmpB5,sizeof(char),2U);});*_tmpB6=_tmp314;});_tmpB6;});_tmpB1[2]=_tmp315;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpB1,sizeof(struct _fat_ptr*),3U));}));{
# 759
unsigned long _tmpB7=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmpB7;
struct _fat_ptr s2=({unsigned _tmpC2=(len2 + (unsigned long)1)+ 1U;char*_tmpC1=_cycalloc_atomic(_check_times(_tmpC2,sizeof(char)));({{unsigned _tmp2AB=len2 + (unsigned long)1;unsigned i;for(i=0;i < _tmp2AB;++ i){_tmpC1[i]='\000';}_tmpC1[_tmp2AB]=0;}0;});_tag_fat(_tmpC1,sizeof(char),_tmpC2);});
int _tmpB8=0;int i=_tmpB8;
int _tmpB9=0;int j=_tmpB9;
for(0;(unsigned long)i < len;++ i){
char _tmpBA=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmpBA;
if((int)c == (int)'\'' || Cyc_is_other_special(c))
({struct _fat_ptr _tmpBB=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmpBC=*((char*)_check_fat_subscript(_tmpBB,sizeof(char),0U));char _tmpBD='\\';if(_get_fat_size(_tmpBB,sizeof(char))== 1U &&(_tmpBC == 0 && _tmpBD != 0))_throw_arraybounds();*((char*)_tmpBB.curr)=_tmpBD;});
({struct _fat_ptr _tmpBE=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmpBF=*((char*)_check_fat_subscript(_tmpBE,sizeof(char),0U));char _tmpC0=c;if(_get_fat_size(_tmpBE,sizeof(char))== 1U &&(_tmpBF == 0 && _tmpC0 != 0))_throw_arraybounds();*((char*)_tmpBE.curr)=_tmpC0;});}
# 769
return(struct _fat_ptr)s2;}}
# 771
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmpC3=_cycalloc(sizeof(*_tmpC3));({struct _fat_ptr _tmp319=Cyc_sh_escape_string(*sp);*_tmpC3=_tmp319;});_tmpC3;});}
# 775
static void Cyc_process_file(struct _fat_ptr filename){
# 777
struct _fat_ptr _tmpC4=Cyc_make_base_filename(filename,Cyc_output_file);struct _fat_ptr basename=_tmpC4;
struct _fat_ptr _tmpC5=({struct _fat_ptr _tmp31A=(struct _fat_ptr)basename;Cyc_strconcat(_tmp31A,({const char*_tmp124=".cyp";_tag_fat(_tmp124,sizeof(char),5U);}));});struct _fat_ptr preprocfile=_tmpC5;
struct _fat_ptr _tmpC6=({char*_tmp31C=(char*)Cyc_specified_interface.curr;_tmp31C != (char*)(_tag_fat(0,0,0)).curr;})?Cyc_specified_interface:(struct _fat_ptr)({
struct _fat_ptr _tmp31B=(struct _fat_ptr)basename;Cyc_strconcat(_tmp31B,({const char*_tmp123=".cyci";_tag_fat(_tmp123,sizeof(char),6U);}));});
# 779
struct _fat_ptr interfacefile=_tmpC6;
# 781
struct _fat_ptr _tmpC7=({struct _fat_ptr _tmp31D=(struct _fat_ptr)basename;Cyc_strconcat(_tmp31D,({const char*_tmp122=".cycio";_tag_fat(_tmp122,sizeof(char),7U);}));});struct _fat_ptr interfaceobjfile=_tmpC7;
# 785
struct _fat_ptr _tmpC8=({struct _fat_ptr _tmp31E=(struct _fat_ptr)Cyc_make_base_filename(filename,0);Cyc_strconcat(_tmp31E,({const char*_tmp121=".c";_tag_fat(_tmp121,sizeof(char),3U);}));});struct _fat_ptr cfile=_tmpC8;
struct _fat_ptr*_tmpC9=({struct _fat_ptr*_tmp120=_cycalloc(sizeof(*_tmp120));*_tmp120=preprocfile;_tmp120;});struct _fat_ptr*preprocfileptr=_tmpC9;
# 788
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpCC=({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0U,_tmp2AC.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_tmp2AC;});void*_tmpCA[1U];_tmpCA[0]=& _tmpCC;({struct Cyc___cycFILE*_tmp320=Cyc_stderr;struct _fat_ptr _tmp31F=({const char*_tmpCB="Compiling %s\n";_tag_fat(_tmpCB,sizeof(char),14U);});Cyc_fprintf(_tmp320,_tmp31F,_tag_fat(_tmpCA,sizeof(void*),1U));});});{
# 791
struct Cyc___cycFILE*f0=({struct _fat_ptr _tmp322=filename;struct _fat_ptr _tmp321=({const char*_tmp11E="r";_tag_fat(_tmp11E,sizeof(char),2U);});Cyc_try_file_open(_tmp322,_tmp321,({const char*_tmp11F="input file";_tag_fat(_tmp11F,sizeof(char),11U);}));});
if(Cyc_compile_failure || !((unsigned)f0))
return;
Cyc_fclose(f0);{
# 798
struct _fat_ptr _tmpCD=({
struct Cyc_List_List*_tmp326=({struct Cyc_List_List*_tmp11C=_cycalloc(sizeof(*_tmp11C));({struct _fat_ptr*_tmp325=({struct _fat_ptr*_tmp11B=_cycalloc(sizeof(*_tmp11B));({struct _fat_ptr _tmp324=(struct _fat_ptr)({const char*_tmp11A="";_tag_fat(_tmp11A,sizeof(char),1U);});*_tmp11B=_tmp324;});_tmp11B;});_tmp11C->hd=_tmp325;}),({
struct Cyc_List_List*_tmp323=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp11C->tl=_tmp323;});_tmp11C;});
# 799
Cyc_str_sepstr(_tmp326,({const char*_tmp11D=" ";_tag_fat(_tmp11D,sizeof(char),2U);}));});
# 798
struct _fat_ptr cppargs_string=_tmpCD;
# 808
struct Cyc_List_List*_tmpCE=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);struct Cyc_List_List*stdinc_dirs=_tmpCE;
stdinc_dirs=({struct Cyc_List_List*_tmp327=stdinc_dirs;Cyc_add_subdir(_tmp327,({const char*_tmpCF="include";_tag_fat(_tmpCF,sizeof(char),8U);}));});
if(Cyc_strlen((struct _fat_ptr)Cyc_def_inc_path)> (unsigned long)0)
stdinc_dirs=({struct Cyc_List_List*_tmpD1=_cycalloc(sizeof(*_tmpD1));({struct _fat_ptr*_tmp328=({struct _fat_ptr*_tmpD0=_cycalloc(sizeof(*_tmpD0));*_tmpD0=Cyc_def_inc_path;_tmpD0;});_tmpD1->hd=_tmp328;}),_tmpD1->tl=stdinc_dirs;_tmpD1;});{
# 813
char*_tmpD2=getenv("CYCLONE_INCLUDE_PATH");char*cyclone_include_path=_tmpD2;
if(cyclone_include_path != 0)
stdinc_dirs=({struct Cyc_List_List*_tmp329=Cyc_split_by_char(({char*_tmpD3=cyclone_include_path;_tag_fat(_tmpD3,sizeof(char),_get_zero_arr_size_char((void*)_tmpD3,1U));}),':');((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp329,stdinc_dirs);});{
# 818
struct _fat_ptr stdinc_string=(struct _fat_ptr)({
struct Cyc_List_List*_tmp32D=({struct Cyc_List_List*_tmp118=_cycalloc(sizeof(*_tmp118));({struct _fat_ptr*_tmp32C=({struct _fat_ptr*_tmp117=_cycalloc(sizeof(*_tmp117));({struct _fat_ptr _tmp32B=(struct _fat_ptr)({const char*_tmp116="";_tag_fat(_tmp116,sizeof(char),1U);});*_tmp117=_tmp32B;});_tmp117;});_tmp118->hd=_tmp32C;}),({
struct Cyc_List_List*_tmp32A=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,stdinc_dirs);_tmp118->tl=_tmp32A;});_tmp118;});
# 819
Cyc_str_sepstr(_tmp32D,({const char*_tmp119=" -I";_tag_fat(_tmp119,sizeof(char),4U);}));});
# 822
struct _fat_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0)
ofile_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0U,_tmp2AD.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_output_file)));_tmp2AD;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({struct _fat_ptr _tmp32E=({const char*_tmpD5=" > %s";_tag_fat(_tmpD5,sizeof(char),6U);});Cyc_aprintf(_tmp32E,_tag_fat(_tmpD4,sizeof(void*),1U));});});else{
# 827
ofile_string=({const char*_tmpD7="";_tag_fat(_tmpD7,sizeof(char),1U);});}}else{
# 829
ofile_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpDA=({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0U,({struct _fat_ptr _tmp32F=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string((struct _fat_ptr)preprocfile));_tmp2AE.f1=_tmp32F;});_tmp2AE;});void*_tmpD8[1U];_tmpD8[0]=& _tmpDA;({struct _fat_ptr _tmp330=({const char*_tmpD9=" > %s";_tag_fat(_tmpD9,sizeof(char),6U);});Cyc_aprintf(_tmp330,_tag_fat(_tmpD8,sizeof(void*),1U));});});}{
# 831
struct _fat_ptr fixup_string;
if(Cyc_produce_dependencies){
# 835
if(Cyc_dependencies_target == 0)
# 839
fixup_string=({const char*_tmpDB=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_fat(_tmpDB,sizeof(char),35U);});else{
# 844
fixup_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpDE=({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0U,_tmp2AF.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_dependencies_target)));_tmp2AF;});void*_tmpDC[1U];_tmpDC[0]=& _tmpDE;({struct _fat_ptr _tmp331=({const char*_tmpDD=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_fat(_tmpDD,sizeof(char),29U);});Cyc_aprintf(_tmp331,_tag_fat(_tmpDC,sizeof(void*),1U));});});}}else{
# 848
fixup_string=({const char*_tmpDF="";_tag_fat(_tmpDF,sizeof(char),1U);});}{
# 850
struct _fat_ptr _tmpE0=({struct Cyc_String_pa_PrintArg_struct _tmp110=({struct Cyc_String_pa_PrintArg_struct _tmp2B8;_tmp2B8.tag=0U,_tmp2B8.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cpp);_tmp2B8;});struct Cyc_String_pa_PrintArg_struct _tmp111=({struct Cyc_String_pa_PrintArg_struct _tmp2B7;_tmp2B7.tag=0U,_tmp2B7.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp2B7;});struct Cyc_String_pa_PrintArg_struct _tmp112=({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0U,_tmp2B6.f1=(struct _fat_ptr)((struct _fat_ptr)stdinc_string);_tmp2B6;});struct Cyc_String_pa_PrintArg_struct _tmp113=({struct Cyc_String_pa_PrintArg_struct _tmp2B5;_tmp2B5.tag=0U,({
# 853
struct _fat_ptr _tmp332=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(filename));_tmp2B5.f1=_tmp332;});_tmp2B5;});struct Cyc_String_pa_PrintArg_struct _tmp114=({struct Cyc_String_pa_PrintArg_struct _tmp2B4;_tmp2B4.tag=0U,_tmp2B4.f1=(struct _fat_ptr)((struct _fat_ptr)fixup_string);_tmp2B4;});struct Cyc_String_pa_PrintArg_struct _tmp115=({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0U,_tmp2B3.f1=(struct _fat_ptr)((struct _fat_ptr)ofile_string);_tmp2B3;});void*_tmp10E[6U];_tmp10E[0]=& _tmp110,_tmp10E[1]=& _tmp111,_tmp10E[2]=& _tmp112,_tmp10E[3]=& _tmp113,_tmp10E[4]=& _tmp114,_tmp10E[5]=& _tmp115;({struct _fat_ptr _tmp333=({const char*_tmp10F="%s %s%s %s%s%s";_tag_fat(_tmp10F,sizeof(char),15U);});Cyc_aprintf(_tmp333,_tag_fat(_tmp10E,sizeof(void*),6U));});});
# 850
struct _fat_ptr cmd=_tmpE0;
# 856
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpE3=({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0U,_tmp2B0.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp2B0;});void*_tmpE1[1U];_tmpE1[0]=& _tmpE3;({struct Cyc___cycFILE*_tmp335=Cyc_stderr;struct _fat_ptr _tmp334=({const char*_tmpE2="%s\n";_tag_fat(_tmpE2,sizeof(char),4U);});Cyc_fprintf(_tmp335,_tmp334,_tag_fat(_tmpE1,sizeof(void*),1U));});});
if(system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)))!= 0){
Cyc_compile_failure=1;
({void*_tmpE4=0U;({struct Cyc___cycFILE*_tmp337=Cyc_stderr;struct _fat_ptr _tmp336=({const char*_tmpE5="\nError: preprocessing\n";_tag_fat(_tmpE5,sizeof(char),23U);});Cyc_fprintf(_tmp337,_tmp336,_tag_fat(_tmpE4,sizeof(void*),0U));});});
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _fat_ptr)preprocfile);
return;}
# 863
if(Cyc_stop_after_cpp_r)return;
# 866
Cyc_Position_reset_position((struct _fat_ptr)preprocfile);{
struct Cyc___cycFILE*in_file=({struct _fat_ptr _tmp339=(struct _fat_ptr)preprocfile;struct _fat_ptr _tmp338=({const char*_tmp10C="r";_tag_fat(_tmp10C,sizeof(char),2U);});Cyc_try_file_open(_tmp339,_tmp338,({const char*_tmp10D="file";_tag_fat(_tmp10D,sizeof(char),5U);}));});
if(Cyc_compile_failure)return;{
# 870
struct Cyc_List_List*tds=0;
# 873
tds=({struct _fat_ptr _tmp33C=({const char*_tmpE6="parsing";_tag_fat(_tmpE6,sizeof(char),8U);});struct Cyc_List_List*_tmp33B=tds;struct Cyc___cycFILE*_tmp33A=(struct Cyc___cycFILE*)_check_null(in_file);((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp33C,_tmp33B,Cyc_do_parse,_tmp33A,Cyc_remove_fileptr,preprocfileptr);});
# 875
if(Cyc_compile_failure)return;
# 877
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}
# 882
({struct _fat_ptr _tmp33E=({const char*_tmpE7="binding";_tag_fat(_tmpE7,sizeof(char),8U);});struct Cyc_List_List*_tmp33D=tds;((int(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp33E,_tmp33D,Cyc_do_binding,1,Cyc_remove_fileptr,preprocfileptr);});
if(Cyc_compile_failure)return;
# 886
{struct _handler_cons _tmpE8;_push_handler(& _tmpE8);{int _tmpEA=0;if(setjmp(_tmpE8.handler))_tmpEA=1;if(!_tmpEA){
({struct _fat_ptr _tmp340=({const char*_tmpEB="current region resolution";_tag_fat(_tmpEB,sizeof(char),26U);});struct Cyc_List_List*_tmp33F=tds;((int(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp340,_tmp33F,Cyc_do_adjust,1,Cyc_remove_fileptr,preprocfileptr);});
# 889
if(Cyc_compile_failure){_npop_handler(0U);return;}
# 887
;_pop_handler();}else{void*_tmpE9=(void*)Cyc_Core_get_exn_thrown();void*_tmpEC=_tmpE9;void*_tmpED;struct _fat_ptr _tmpEE;if(((struct Cyc_Core_Failure_exn_struct*)_tmpEC)->tag == Cyc_Core_Failure){_LL1: _tmpEE=((struct Cyc_Core_Failure_exn_struct*)_tmpEC)->f1;_LL2: {struct _fat_ptr s=_tmpEE;
# 892
({struct Cyc_String_pa_PrintArg_struct _tmpF1=({struct Cyc_String_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=0U,_tmp2B1.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp2B1;});void*_tmpEF[1U];_tmpEF[0]=& _tmpF1;({struct Cyc___cycFILE*_tmp342=Cyc_stderr;struct _fat_ptr _tmp341=({const char*_tmpF0="Got exception Failure(%s)\n";_tag_fat(_tmpF0,sizeof(char),27U);});Cyc_fprintf(_tmp342,_tmp341,_tag_fat(_tmpEF,sizeof(void*),1U));});});
return;}}else{_LL3: _tmpED=_tmpEC;_LL4: {void*e=_tmpED;
Cyc_Core_rethrow(e);}}_LL0:;}}}{
# 897
struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables;
{struct Cyc_Tcenv_Tenv*_tmpF2=Cyc_Tcenv_tc_init();struct Cyc_Tcenv_Tenv*te=_tmpF2;
# 900
if(Cyc_parseonly_r)goto PRINTC;
# 903
tds=({struct _fat_ptr _tmp345=({const char*_tmpF3="type checking";_tag_fat(_tmpF3,sizeof(char),14U);});struct Cyc_List_List*_tmp344=tds;struct Cyc_Tcenv_Tenv*_tmp343=te;((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp345,_tmp344,Cyc_do_typecheck,_tmp343,Cyc_remove_fileptr,preprocfileptr);});
# 905
if(Cyc_compile_failure)return;
# 908
jump_tables=({struct _fat_ptr _tmp347=({const char*_tmpF4="jump checking";_tag_fat(_tmpF4,sizeof(char),14U);});struct Cyc_List_List*_tmp346=tds;((struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp347,_tmp346,Cyc_do_jumpanalysis,1,Cyc_remove_fileptr,preprocfileptr);});
# 910
if(Cyc_compile_failure)return;
# 913
if(Cyc_tc_r)goto PRINTC;
({struct _fat_ptr _tmp34A=({const char*_tmpF5="control-flow checking";_tag_fat(_tmpF5,sizeof(char),22U);});struct Cyc_List_List*_tmp349=tds;struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp348=jump_tables;((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp34A,_tmp349,Cyc_do_cfcheck,_tmp348,Cyc_remove_fileptr,preprocfileptr);});
# 916
if(Cyc_compile_failure)return;
# 918
if(Cyc_vcgen_r){
({struct _fat_ptr _tmp34D=({const char*_tmpF6="verification condition generation";_tag_fat(_tmpF6,sizeof(char),34U);});struct Cyc_List_List*_tmp34C=tds;struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp34B=jump_tables;((int(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,int(*f)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp34D,_tmp34C,Cyc_do_vcgen,_tmp34B,Cyc_remove_fileptr,preprocfileptr);});
# 921
if(Cyc_compile_failure)return;}
# 923
({struct _fat_ptr _tmp34F=({const char*_tmpF7="check insertion";_tag_fat(_tmpF7,sizeof(char),16U);});struct Cyc_List_List*_tmp34E=tds;((int(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp34F,_tmp34E,Cyc_do_insert_checks,1,Cyc_remove_fileptr,preprocfileptr);});
# 925
if(Cyc_compile_failure)return;
# 928
if(Cyc_generate_interface_r){
struct Cyc___cycFILE*inter_file=({struct _fat_ptr _tmp351=({const char*_tmpF8=(const char*)_check_null(_untag_fat_ptr(interfacefile,sizeof(char),1U));_tag_fat(_tmpF8,sizeof(char),_get_zero_arr_size_char((void*)_tmpF8,1U));});struct _fat_ptr _tmp350=({const char*_tmpF9="w";_tag_fat(_tmpF9,sizeof(char),2U);});Cyc_try_file_open(_tmp351,_tmp350,({const char*_tmpFA="interface file";_tag_fat(_tmpFA,sizeof(char),15U);}));});
if(inter_file == 0){
Cyc_compile_failure=1;
return;}
# 934
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 941
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(interfacefile,sizeof(char),1U)),"r");
struct Cyc___cycFILE*inter_objfile=({struct _fat_ptr _tmp353=(struct _fat_ptr)interfaceobjfile;struct _fat_ptr _tmp352=({const char*_tmpFD="w";_tag_fat(_tmpFD,sizeof(char),2U);});Cyc_try_file_open(_tmp353,_tmp352,({const char*_tmpFE="interface object file";_tag_fat(_tmpFE,sizeof(char),22U);}));});
if(inter_objfile == 0){
Cyc_compile_failure=1;
return;}
# 948
Cyc_Position_reset_position(interfacefile);{
struct _tuple27 _tmpFB=({struct _tuple27 _tmp2B2;_tmp2B2.f1=te,_tmp2B2.f2=inter_file,_tmp2B2.f3=inter_objfile;_tmp2B2;});struct _tuple27 int_env=_tmpFB;
tds=({struct _fat_ptr _tmp354=({const char*_tmpFC="interface checking";_tag_fat(_tmpFC,sizeof(char),19U);});((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple27*,struct Cyc_List_List*),struct _tuple27*env,void(*on_fail)(struct _tuple27*),struct _tuple27*failenv))Cyc_do_stage)(_tmp354,tds,Cyc_do_interface,& int_env,Cyc_interface_fail,& int_env);});}}}
# 954
if(Cyc_cf_r)goto PRINTC;
# 957
tds=({struct _fat_ptr _tmp357=({const char*_tmpFF="translation to C";_tag_fat(_tmpFF,sizeof(char),17U);});struct Cyc_List_List*_tmp356=tds;struct Cyc_Hashtable_Table*_tmp355=jump_tables->pop_tables;((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp357,_tmp356,Cyc_do_translate,_tmp355,Cyc_remove_fileptr,preprocfileptr);});
# 960
tds=({struct _fat_ptr _tmp358=({const char*_tmp100="aggregate removal";_tag_fat(_tmp100,sizeof(char),18U);});((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp358,tds,Cyc_do_removeaggrs,1,(void(*)(int x))Cyc_ignore,1);});
if(Cyc_compile_failure)return;
# 964
if(Cyc_toseqc_r)
tds=({struct _fat_ptr _tmp359=({const char*_tmp101="post-pass to L-to-R evaluation order";_tag_fat(_tmp101,sizeof(char),37U);});((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp359,tds,Cyc_do_toseqc,1,(void(*)(int x))Cyc_ignore,1);});
# 968
Cyc_Toc_finish();
# 970
if(Cyc_compile_failure)return;
# 973
if(Cyc_Cyclone_tovc_r || Cyc_elim_se_r)
tds=({struct _fat_ptr _tmp35A=({const char*_tmp102="post-pass to VC";_tag_fat(_tmp102,sizeof(char),16U);});((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp35A,tds,Cyc_do_tovc,1,(void(*)(int x))Cyc_ignore,1);});
if(Cyc_compile_failure)return;
# 977
Cyc_Warn_flush_warnings();
Cyc_remove_file((struct _fat_ptr)preprocfile);
# 981
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r || Cyc_tc_r)|| Cyc_cf_r){
if(Cyc_output_file != 0)
out_file=({struct _fat_ptr _tmp35C=*((struct _fat_ptr*)_check_null(Cyc_output_file));struct _fat_ptr _tmp35B=({const char*_tmp103="w";_tag_fat(_tmp103,sizeof(char),2U);});Cyc_try_file_open(_tmp35C,_tmp35B,({const char*_tmp104="output file";_tag_fat(_tmp104,sizeof(char),12U);}));});else{
# 987
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r && Cyc_output_file != 0)
out_file=({struct _fat_ptr _tmp35E=*((struct _fat_ptr*)_check_null(Cyc_output_file));struct _fat_ptr _tmp35D=({const char*_tmp105="w";_tag_fat(_tmp105,sizeof(char),2U);});Cyc_try_file_open(_tmp35E,_tmp35D,({const char*_tmp106="output file";_tag_fat(_tmp106,sizeof(char),12U);}));});else{
# 991
out_file=({struct _fat_ptr _tmp360=(struct _fat_ptr)cfile;struct _fat_ptr _tmp35F=({const char*_tmp107="w";_tag_fat(_tmp107,sizeof(char),2U);});Cyc_try_file_open(_tmp360,_tmp35F,({const char*_tmp108="output file";_tag_fat(_tmp108,sizeof(char),12U);}));});}}
# 993
if(Cyc_compile_failure || !((unsigned)out_file))return;
# 995
if(!Cyc_noprint_r){
struct _tuple28*_tmp109=({struct _tuple28*_tmp10B=_cycalloc(sizeof(*_tmp10B));_tmp10B->f1=out_file,_tmp10B->f2=cfile;_tmp10B;});struct _tuple28*env=_tmp109;
tds=({struct _fat_ptr _tmp363=({const char*_tmp10A="printing";_tag_fat(_tmp10A,sizeof(char),9U);});struct Cyc_List_List*_tmp362=tds;struct _tuple28*_tmp361=env;((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple28*,struct Cyc_List_List*),struct _tuple28*env,void(*on_fail)(struct _tuple28*),struct _tuple28*failenv))Cyc_do_stage)(_tmp363,_tmp362,Cyc_do_print,_tmp361,Cyc_print_fail,env);});}}}}}}}}}}}}static char _tmp125[8U]="<final>";
# 1002
static struct _fat_ptr Cyc_final_str={_tmp125,_tmp125,_tmp125 + 8U};
static struct _fat_ptr*Cyc_final_strptr=& Cyc_final_str;
# 1005
static struct Cyc_Interface_I*Cyc_read_cycio(struct _fat_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 1009
struct _fat_ptr basename;
{struct _handler_cons _tmp126;_push_handler(& _tmp126);{int _tmp128=0;if(setjmp(_tmp126.handler))_tmp128=1;if(!_tmp128){basename=(struct _fat_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp127=(void*)Cyc_Core_get_exn_thrown();void*_tmp129=_tmp127;void*_tmp12A;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp129)->tag == Cyc_Core_Invalid_argument){_LL1: _LL2:
 basename=*n;goto _LL0;}else{_LL3: _tmp12A=_tmp129;_LL4: {void*exn=_tmp12A;(int)_rethrow(exn);}}_LL0:;}}}{
# 1013
struct _fat_ptr _tmp12B=({struct _fat_ptr _tmp364=(struct _fat_ptr)basename;Cyc_strconcat(_tmp364,({const char*_tmp130=".cycio";_tag_fat(_tmp130,sizeof(char),7U);}));});struct _fat_ptr nf=_tmp12B;
struct Cyc___cycFILE*_tmp12C=({struct _fat_ptr _tmp366=(struct _fat_ptr)nf;struct _fat_ptr _tmp365=({const char*_tmp12E="rb";_tag_fat(_tmp12E,sizeof(char),3U);});Cyc_try_file_open(_tmp366,_tmp365,({const char*_tmp12F="interface object file";_tag_fat(_tmp12F,sizeof(char),22U);}));});struct Cyc___cycFILE*f=_tmp12C;
if(f == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 1020
Cyc_Position_reset_position((struct _fat_ptr)nf);{
# 1022
struct Cyc_Interface_I*_tmp12D=Cyc_Interface_load(f);struct Cyc_Interface_I*i=_tmp12D;
Cyc_file_close(f);
return i;}}}}
# 1027
static int Cyc_is_cfile(struct _fat_ptr*n){
return(int)*((const char*)_check_fat_subscript(*n,sizeof(char),0))!= (int)'-';}
# 1031
extern void GC_blacklist_warn_clear (void);struct _tuple29{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
# 1041
void Cyc_print_options (void);
# 1048
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3U,& Cyc_v_r};static char _tmp131[3U]="-v";static char _tmp132[1U]="";static char _tmp133[35U]="print compilation stages verbosely";struct _tuple29 Cyc_opt1_tuple={{_tmp131,_tmp131,_tmp131 + 3U},0,{_tmp132,_tmp132,_tmp132 + 1U},(void*)& Cyc_opt1_spec,{_tmp133,_tmp133,_tmp133 + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 1050
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0U,Cyc_print_version};static char _tmp134[10U]="--version";static char _tmp135[1U]="";static char _tmp136[35U]="Print version information and exit";struct _tuple29 Cyc_opt2_tuple={{_tmp134,_tmp134,_tmp134 + 10U},0,{_tmp135,_tmp135,_tmp135 + 1U},(void*)& Cyc_opt2_spec,{_tmp136,_tmp136,_tmp136 + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 1053
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5U,Cyc_set_output_file};static char _tmp137[3U]="-o";static char _tmp138[8U]=" <file>";static char _tmp139[35U]="Set the output file name to <file>";struct _tuple29 Cyc_opt3_tuple={{_tmp137,_tmp137,_tmp137 + 3U},0,{_tmp138,_tmp138,_tmp138 + 8U},(void*)& Cyc_opt3_spec,{_tmp139,_tmp139,_tmp139 + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 1056
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1U,Cyc_add_cpparg};static char _tmp13A[3U]="-D";static char _tmp13B[17U]="<name>[=<value>]";static char _tmp13C[32U]="Pass definition to preprocessor";struct _tuple29 Cyc_opt4_tuple={{_tmp13A,_tmp13A,_tmp13A + 3U},1,{_tmp13B,_tmp13B,_tmp13B + 17U},(void*)& Cyc_opt4_spec,{_tmp13C,_tmp13C,_tmp13C + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 1059
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1U,Cyc_Specsfile_add_cyclone_exec_path};static char _tmp13D[3U]="-B";static char _tmp13E[7U]="<file>";static char _tmp13F[60U]="Add to the list of directories to search for compiler files";struct _tuple29 Cyc_opt5_tuple={{_tmp13D,_tmp13D,_tmp13D + 3U},1,{_tmp13E,_tmp13E,_tmp13E + 7U},(void*)& Cyc_opt5_spec,{_tmp13F,_tmp13F,_tmp13F + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 1062
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1U,Cyc_add_cpparg};static char _tmp140[3U]="-I";static char _tmp141[6U]="<dir>";static char _tmp142[59U]="Add to the list of directories to search for include files";struct _tuple29 Cyc_opt6_tuple={{_tmp140,_tmp140,_tmp140 + 3U},1,{_tmp141,_tmp141,_tmp141 + 6U},(void*)& Cyc_opt6_spec,{_tmp142,_tmp142,_tmp142 + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 1065
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1U,Cyc_add_ccarg};static char _tmp143[3U]="-L";static char _tmp144[6U]="<dir>";static char _tmp145[38U]="Add to the list of directories for -l";struct _tuple29 Cyc_opt7_tuple={{_tmp143,_tmp143,_tmp143 + 3U},1,{_tmp144,_tmp144,_tmp144 + 6U},(void*)& Cyc_opt7_spec,{_tmp145,_tmp145,_tmp145 + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 1068
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1U,Cyc_add_libarg};static char _tmp146[3U]="-l";static char _tmp147[10U]="<libname>";static char _tmp148[13U]="Library file";struct _tuple29 Cyc_opt8_tuple={{_tmp146,_tmp146,_tmp146 + 3U},1,{_tmp147,_tmp147,_tmp147 + 10U},(void*)& Cyc_opt8_spec,{_tmp148,_tmp148,_tmp148 + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 1071
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1U,Cyc_add_marg};static char _tmp149[3U]="-m";static char _tmp14A[9U]="<option>";static char _tmp14B[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple29 Cyc_opt9_tuple={{_tmp149,_tmp149,_tmp149 + 3U},1,{_tmp14A,_tmp14A,_tmp14A + 9U},(void*)& Cyc_opt9_spec,{_tmp14B,_tmp14B,_tmp14B + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 1074
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0U,Cyc_set_stop_after_objectfile};static char _tmp14C[3U]="-c";static char _tmp14D[1U]="";static char _tmp14E[61U]="Produce an object file instead of an executable; do not link";struct _tuple29 Cyc_opt10_tuple={{_tmp14C,_tmp14C,_tmp14C + 3U},0,{_tmp14D,_tmp14D,_tmp14D + 1U},(void*)& Cyc_opt10_spec,{_tmp14E,_tmp14E,_tmp14E + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 1077
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5U,Cyc_set_inputtype};static char _tmp14F[3U]="-x";static char _tmp150[12U]=" <language>";static char _tmp151[49U]="Specify <language> for the following input files";struct _tuple29 Cyc_opt11_tuple={{_tmp14F,_tmp14F,_tmp14F + 3U},0,{_tmp150,_tmp150,_tmp150 + 12U},(void*)& Cyc_opt11_spec,{_tmp151,_tmp151,_tmp151 + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 1080
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0U,Cyc_set_pa};static char _tmp152[4U]="-pa";static char _tmp153[1U]="";static char _tmp154[33U]="Compile for profiling with aprof";struct _tuple29 Cyc_opt12_tuple={{_tmp152,_tmp152,_tmp152 + 4U},0,{_tmp153,_tmp153,_tmp153 + 1U},(void*)& Cyc_opt12_spec,{_tmp154,_tmp154,_tmp154 + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 1083
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0U,Cyc_set_stop_after_asmfile};static char _tmp155[3U]="-S";static char _tmp156[1U]="";static char _tmp157[35U]="Stop after producing assembly code";struct _tuple29 Cyc_opt13_tuple={{_tmp155,_tmp155,_tmp155 + 3U},0,{_tmp156,_tmp156,_tmp156 + 1U},(void*)& Cyc_opt13_spec,{_tmp157,_tmp157,_tmp157 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1086
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0U,Cyc_set_produce_dependencies};static char _tmp158[3U]="-M";static char _tmp159[1U]="";static char _tmp15A[21U]="Produce dependencies";struct _tuple29 Cyc_opt14_tuple={{_tmp158,_tmp158,_tmp158 + 3U},0,{_tmp159,_tmp159,_tmp159 + 1U},(void*)& Cyc_opt14_spec,{_tmp15A,_tmp15A,_tmp15A + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1089
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1U,Cyc_add_cpparg};static char _tmp15B[4U]="-MG";static char _tmp15C[1U]="";static char _tmp15D[68U]="When producing dependencies assume that missing files are generated";struct _tuple29 Cyc_opt15_tuple={{_tmp15B,_tmp15B,_tmp15B + 4U},0,{_tmp15C,_tmp15C,_tmp15C + 1U},(void*)& Cyc_opt15_spec,{_tmp15D,_tmp15D,_tmp15D + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1093
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5U,Cyc_set_dependencies_target};static char _tmp15E[4U]="-MT";static char _tmp15F[10U]=" <target>";static char _tmp160[29U]="Give target for dependencies";struct _tuple29 Cyc_opt16_tuple={{_tmp15E,_tmp15E,_tmp15E + 4U},0,{_tmp15F,_tmp15F,_tmp15F + 10U},(void*)& Cyc_opt16_spec,{_tmp160,_tmp160,_tmp160 + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1096
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5U,Cyc_Specsfile_set_target_arch};static char _tmp161[3U]="-b";static char _tmp162[10U]="<machine>";static char _tmp163[19U]="Set target machine";struct _tuple29 Cyc_opt17_tuple={{_tmp161,_tmp161,_tmp161 + 3U},0,{_tmp162,_tmp162,_tmp162 + 10U},(void*)& Cyc_opt17_spec,{_tmp163,_tmp163,_tmp163 + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1099
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3U,& Cyc_NewControlFlow_warn_lose_unique};static char _tmp164[14U]="-Wlose-unique";static char _tmp165[1U]="";static char _tmp166[49U]="Try to warn when a unique pointer might get lost";struct _tuple29 Cyc_opt18_tuple={{_tmp164,_tmp164,_tmp164 + 14U},0,{_tmp165,_tmp165,_tmp165 + 1U},(void*)& Cyc_opt18_spec,{_tmp166,_tmp166,_tmp166 + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1102
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3U,& Cyc_Binding_warn_override};static char _tmp167[11U]="-Woverride";static char _tmp168[1U]="";static char _tmp169[58U]="Warn when a local variable overrides an existing variable";struct _tuple29 Cyc_opt19_tuple={{_tmp167,_tmp167,_tmp167 + 11U},0,{_tmp168,_tmp168,_tmp168 + 1U},(void*)& Cyc_opt19_spec,{_tmp169,_tmp169,_tmp169 + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1105
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0U,Cyc_set_all_warnings};static char _tmp16A[6U]="-Wall";static char _tmp16B[1U]="";static char _tmp16C[21U]="Turn on all warnings";struct _tuple29 Cyc_opt20_tuple={{_tmp16A,_tmp16A,_tmp16A + 6U},0,{_tmp16B,_tmp16B,_tmp16B + 1U},(void*)& Cyc_opt20_spec,{_tmp16C,_tmp16C,_tmp16C + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1108
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt21_spec={3U,& Cyc_stop_after_cpp_r};static char _tmp16D[3U]="-E";static char _tmp16E[1U]="";static char _tmp16F[25U]="Stop after preprocessing";struct _tuple29 Cyc_opt21_tuple={{_tmp16D,_tmp16D,_tmp16D + 3U},0,{_tmp16E,_tmp16E,_tmp16E + 1U},(void*)& Cyc_opt21_spec,{_tmp16F,_tmp16F,_tmp16F + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1111
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt22_spec={3U,& Cyc_parseonly_r};static char _tmp170[17U]="-stopafter-parse";static char _tmp171[1U]="";static char _tmp172[19U]="Stop after parsing";struct _tuple29 Cyc_opt22_tuple={{_tmp170,_tmp170,_tmp170 + 17U},0,{_tmp171,_tmp171,_tmp171 + 1U},(void*)& Cyc_opt22_spec,{_tmp172,_tmp172,_tmp172 + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1114
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt23_spec={3U,& Cyc_tc_r};static char _tmp173[14U]="-stopafter-tc";static char _tmp174[1U]="";static char _tmp175[25U]="Stop after type checking";struct _tuple29 Cyc_opt23_tuple={{_tmp173,_tmp173,_tmp173 + 14U},0,{_tmp174,_tmp174,_tmp174 + 1U},(void*)& Cyc_opt23_spec,{_tmp175,_tmp175,_tmp175 + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1117
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3U,& Cyc_noprint_r};static char _tmp176[9U]="-noprint";static char _tmp177[1U]="";static char _tmp178[42U]="Do not print output (when stopping early)";struct _tuple29 Cyc_opt24_tuple={{_tmp176,_tmp176,_tmp176 + 9U},0,{_tmp177,_tmp177,_tmp177 + 1U},(void*)& Cyc_opt24_spec,{_tmp178,_tmp178,_tmp178 + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1120
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt25_spec={3U,& Cyc_cf_r};static char _tmp179[14U]="-stopafter-cf";static char _tmp17A[1U]="";static char _tmp17B[33U]="Stop after control-flow checking";struct _tuple29 Cyc_opt25_tuple={{_tmp179,_tmp179,_tmp179 + 14U},0,{_tmp17A,_tmp17A,_tmp17A + 1U},(void*)& Cyc_opt25_spec,{_tmp17B,_tmp17B,_tmp17B + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1123
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt26_spec={3U,& Cyc_toc_r};static char _tmp17C[15U]="-stopafter-toc";static char _tmp17D[1U]="";static char _tmp17E[28U]="Stop after translation to C";struct _tuple29 Cyc_opt26_tuple={{_tmp17C,_tmp17C,_tmp17C + 15U},0,{_tmp17D,_tmp17D,_tmp17D + 1U},(void*)& Cyc_opt26_spec,{_tmp17E,_tmp17E,_tmp17E + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1126
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3U,& Cyc_ic_r};static char _tmp17F[4U]="-ic";static char _tmp180[1U]="";static char _tmp181[26U]="Activate the link-checker";struct _tuple29 Cyc_opt27_tuple={{_tmp17F,_tmp17F,_tmp17F + 4U},0,{_tmp180,_tmp180,_tmp180 + 1U},(void*)& Cyc_opt27_spec,{_tmp181,_tmp181,_tmp181 + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1129
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3U,& Cyc_pp_r};static char _tmp182[4U]="-pp";static char _tmp183[1U]="";static char _tmp184[47U]="Pretty print the C code that Cyclone generates";struct _tuple29 Cyc_opt28_tuple={{_tmp182,_tmp182,_tmp182 + 4U},0,{_tmp183,_tmp183,_tmp183 + 1U},(void*)& Cyc_opt28_spec,{_tmp184,_tmp184,_tmp184 + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1132
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4U,& Cyc_pp_r};static char _tmp185[4U]="-up";static char _tmp186[1U]="";static char _tmp187[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple29 Cyc_opt29_tuple={{_tmp185,_tmp185,_tmp185 + 4U},0,{_tmp186,_tmp186,_tmp186 + 1U},(void*)& Cyc_opt29_spec,{_tmp187,_tmp187,_tmp187 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1135
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3U,& Cyc_elim_se_r};static char _tmp188[28U]="-elim-statement-expressions";static char _tmp189[1U]="";static char _tmp18A[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple29 Cyc_opt30_tuple={{_tmp188,_tmp188,_tmp188 + 28U},0,{_tmp189,_tmp189,_tmp189 + 1U},(void*)& Cyc_opt30_spec,{_tmp18A,_tmp18A,_tmp18A + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1139
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4U,& Cyc_elim_se_r};static char _tmp18B[31U]="-no-elim-statement-expressions";static char _tmp18C[1U]="";static char _tmp18D[47U]="Do not avoid statement expressions in C output";struct _tuple29 Cyc_opt31_tuple={{_tmp18B,_tmp18B,_tmp18B + 31U},0,{_tmp18C,_tmp18C,_tmp18C + 1U},(void*)& Cyc_opt31_spec,{_tmp18D,_tmp18D,_tmp18D + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1142
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0U,Cyc_set_tovc};static char _tmp18E[8U]="-un-gcc";static char _tmp18F[1U]="";static char _tmp190[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple29 Cyc_opt32_tuple={{_tmp18E,_tmp18E,_tmp18E + 8U},0,{_tmp18F,_tmp18F,_tmp18F + 1U},(void*)& Cyc_opt32_spec,{_tmp190,_tmp190,_tmp190 + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1145
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5U,Cyc_set_c_compiler};static char _tmp191[8U]="-c-comp";static char _tmp192[12U]=" <compiler>";static char _tmp193[40U]="Produce C output for the given compiler";struct _tuple29 Cyc_opt33_tuple={{_tmp191,_tmp191,_tmp191 + 8U},0,{_tmp192,_tmp192,_tmp192 + 12U},(void*)& Cyc_opt33_spec,{_tmp193,_tmp193,_tmp193 + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1148
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0U,Cyc_set_save_temps};static char _tmp194[12U]="-save-temps";static char _tmp195[1U]="";static char _tmp196[29U]="Don't delete temporary files";struct _tuple29 Cyc_opt34_tuple={{_tmp194,_tmp194,_tmp194 + 12U},0,{_tmp195,_tmp195,_tmp195 + 1U},(void*)& Cyc_opt34_spec,{_tmp196,_tmp196,_tmp196 + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1151
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3U,& Cyc_save_c_r};static char _tmp197[8U]="-save-c";static char _tmp198[1U]="";static char _tmp199[31U]="Don't delete temporary C files";struct _tuple29 Cyc_opt35_tuple={{_tmp197,_tmp197,_tmp197 + 8U},0,{_tmp198,_tmp198,_tmp198 + 1U},(void*)& Cyc_opt35_spec,{_tmp199,_tmp199,_tmp199 + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1154
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0U,Cyc_clear_lineno};static char _tmp19A[11U]="--nolineno";static char _tmp19B[1U]="";static char _tmp19C[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple29 Cyc_opt36_tuple={{_tmp19A,_tmp19A,_tmp19A + 11U},0,{_tmp19B,_tmp19B,_tmp19B + 1U},(void*)& Cyc_opt36_spec,{_tmp19C,_tmp19C,_tmp19C + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1157
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0U,Cyc_set_nochecks};static char _tmp19D[11U]="--nochecks";static char _tmp19E[1U]="";static char _tmp19F[27U]="Disable bounds/null checks";struct _tuple29 Cyc_opt37_tuple={{_tmp19D,_tmp19D,_tmp19D + 11U},0,{_tmp19E,_tmp19E,_tmp19E + 1U},(void*)& Cyc_opt37_spec,{_tmp19F,_tmp19F,_tmp19F + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1160
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0U,Cyc_set_nonullchecks};static char _tmp1A0[15U]="--nonullchecks";static char _tmp1A1[1U]="";static char _tmp1A2[20U]="Disable null checks";struct _tuple29 Cyc_opt38_tuple={{_tmp1A0,_tmp1A0,_tmp1A0 + 15U},0,{_tmp1A1,_tmp1A1,_tmp1A1 + 1U},(void*)& Cyc_opt38_spec,{_tmp1A2,_tmp1A2,_tmp1A2 + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1163
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0U,Cyc_set_noboundschecks};static char _tmp1A3[17U]="--noboundschecks";static char _tmp1A4[1U]="";static char _tmp1A5[22U]="Disable bounds checks";struct _tuple29 Cyc_opt39_tuple={{_tmp1A3,_tmp1A3,_tmp1A3 + 17U},0,{_tmp1A4,_tmp1A4,_tmp1A4 + 1U},(void*)& Cyc_opt39_spec,{_tmp1A5,_tmp1A5,_tmp1A5 + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1166
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt40_spec={0U,Cyc_set_inline_functions};static char _tmp1A6[16U]="--inline-checks";static char _tmp1A7[1U]="";static char _tmp1A8[40U]="Inline bounds checks instead of #define";struct _tuple29 Cyc_opt40_tuple={{_tmp1A6,_tmp1A6,_tmp1A6 + 16U},0,{_tmp1A7,_tmp1A7,_tmp1A7 + 1U},(void*)& Cyc_opt40_spec,{_tmp1A8,_tmp1A8,_tmp1A8 + 40U}};struct Cyc_List_List Cyc_opt40={(void*)& Cyc_opt40_tuple,0};
# 1169
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5U,Cyc_set_cpp};static char _tmp1A9[9U]="-use-cpp";static char _tmp1AA[7U]="<path>";static char _tmp1AB[35U]="Indicate which preprocessor to use";struct _tuple29 Cyc_opt41_tuple={{_tmp1A9,_tmp1A9,_tmp1A9 + 9U},0,{_tmp1AA,_tmp1AA,_tmp1AA + 7U},(void*)& Cyc_opt41_spec,{_tmp1AB,_tmp1AB,_tmp1AB + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1172
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0U,Cyc_set_nocppprecomp};static char _tmp1AC[16U]="-no-cpp-precomp";static char _tmp1AD[1U]="";static char _tmp1AE[40U]="Don't do smart preprocessing (mac only)";struct _tuple29 Cyc_opt42_tuple={{_tmp1AC,_tmp1AC,_tmp1AC + 16U},0,{_tmp1AD,_tmp1AD,_tmp1AD + 1U},(void*)& Cyc_opt42_spec,{_tmp1AE,_tmp1AE,_tmp1AE + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1175
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0U,Cyc_set_nocyc};static char _tmp1AF[7U]="-nocyc";static char _tmp1B0[1U]="";static char _tmp1B1[33U]="Don't add implicit namespace Cyc";struct _tuple29 Cyc_opt43_tuple={{_tmp1AF,_tmp1AF,_tmp1AF + 7U},0,{_tmp1B0,_tmp1B0,_tmp1B0 + 1U},(void*)& Cyc_opt43_spec,{_tmp1B1,_tmp1B1,_tmp1B1 + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1178
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3U,& Cyc_nogc_r};static char _tmp1B2[6U]="-nogc";static char _tmp1B3[1U]="";static char _tmp1B4[36U]="Don't link in the garbage collector";struct _tuple29 Cyc_opt44_tuple={{_tmp1B2,_tmp1B2,_tmp1B2 + 6U},0,{_tmp1B3,_tmp1B3,_tmp1B3 + 1U},(void*)& Cyc_opt44_spec,{_tmp1B4,_tmp1B4,_tmp1B4 + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1181
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3U,& Cyc_noshake_r};static char _tmp1B5[16U]="-noremoveunused";static char _tmp1B6[1U]="";static char _tmp1B7[49U]="Don't remove externed variables that aren't used";struct _tuple29 Cyc_opt45_tuple={{_tmp1B5,_tmp1B5,_tmp1B5 + 16U},0,{_tmp1B6,_tmp1B6,_tmp1B6 + 1U},(void*)& Cyc_opt45_spec,{_tmp1B7,_tmp1B7,_tmp1B7 + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1184
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3U,& Cyc_noexpand_r};static char _tmp1B8[18U]="-noexpandtypedefs";static char _tmp1B9[1U]="";static char _tmp1BA[41U]="Don't expand typedefs in pretty printing";struct _tuple29 Cyc_opt46_tuple={{_tmp1B8,_tmp1B8,_tmp1B8 + 18U},0,{_tmp1B9,_tmp1B9,_tmp1B9 + 1U},(void*)& Cyc_opt46_spec,{_tmp1BA,_tmp1BA,_tmp1BA + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1187
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3U,& Cyc_print_all_tvars_r};static char _tmp1BB[15U]="-printalltvars";static char _tmp1BC[1U]="";static char _tmp1BD[57U]="Print all type variables (even implicit default effects)";struct _tuple29 Cyc_opt47_tuple={{_tmp1BB,_tmp1BB,_tmp1BB + 15U},0,{_tmp1BC,_tmp1BC,_tmp1BC + 1U},(void*)& Cyc_opt47_spec,{_tmp1BD,_tmp1BD,_tmp1BD + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1190
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3U,& Cyc_print_all_kinds_r};static char _tmp1BE[15U]="-printallkinds";static char _tmp1BF[1U]="";static char _tmp1C0[37U]="Always print kinds of type variables";struct _tuple29 Cyc_opt48_tuple={{_tmp1BE,_tmp1BE,_tmp1BE + 15U},0,{_tmp1BF,_tmp1BF,_tmp1BF + 1U},(void*)& Cyc_opt48_spec,{_tmp1C0,_tmp1C0,_tmp1C0 + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1193
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3U,& Cyc_print_full_evars_r};static char _tmp1C1[16U]="-printfullevars";static char _tmp1C2[1U]="";static char _tmp1C3[50U]="Print full information for evars (type debugging)";struct _tuple29 Cyc_opt49_tuple={{_tmp1C1,_tmp1C1,_tmp1C1 + 16U},0,{_tmp1C2,_tmp1C2,_tmp1C2 + 1U},(void*)& Cyc_opt49_spec,{_tmp1C3,_tmp1C3,_tmp1C3 + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1196
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3U,& Cyc_print_all_effects_r};static char _tmp1C4[17U]="-printalleffects";static char _tmp1C5[1U]="";static char _tmp1C6[45U]="Print effects for functions (type debugging)";struct _tuple29 Cyc_opt50_tuple={{_tmp1C4,_tmp1C4,_tmp1C4 + 17U},0,{_tmp1C5,_tmp1C5,_tmp1C5 + 1U},(void*)& Cyc_opt50_spec,{_tmp1C6,_tmp1C6,_tmp1C6 + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1199
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3U,& Cyc_nocyc_setjmp_r};static char _tmp1C7[14U]="-nocyc_setjmp";static char _tmp1C8[1U]="";static char _tmp1C9[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple29 Cyc_opt51_tuple={{_tmp1C7,_tmp1C7,_tmp1C7 + 14U},0,{_tmp1C8,_tmp1C8,_tmp1C8 + 1U},(void*)& Cyc_opt51_spec,{_tmp1C9,_tmp1C9,_tmp1C9 + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1202
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3U,& Cyc_Lex_compile_for_boot_r};static char _tmp1CA[18U]="-compile-for-boot";static char _tmp1CB[1U]="";static char _tmp1CC[71U]="Compile using the special boot library instead of the standard library";struct _tuple29 Cyc_opt52_tuple={{_tmp1CA,_tmp1CA,_tmp1CA + 18U},0,{_tmp1CB,_tmp1CB,_tmp1CB + 1U},(void*)& Cyc_opt52_spec,{_tmp1CC,_tmp1CC,_tmp1CC + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1205
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5U,Cyc_set_cyc_include};static char _tmp1CD[4U]="-CI";static char _tmp1CE[8U]=" <file>";static char _tmp1CF[31U]="Set cyc_include.h to be <file>";struct _tuple29 Cyc_opt53_tuple={{_tmp1CD,_tmp1CD,_tmp1CD + 4U},0,{_tmp1CE,_tmp1CE,_tmp1CE + 8U},(void*)& Cyc_opt53_spec,{_tmp1CF,_tmp1CF,_tmp1CF + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1208
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3U,& Cyc_InsertChecks_warn_bounds_checks};static char _tmp1D0[18U]="-warnboundschecks";static char _tmp1D1[1U]="";static char _tmp1D2[44U]="Warn when bounds checks can't be eliminated";struct _tuple29 Cyc_opt54_tuple={{_tmp1D0,_tmp1D0,_tmp1D0 + 18U},0,{_tmp1D1,_tmp1D1,_tmp1D1 + 1U},(void*)& Cyc_opt54_spec,{_tmp1D2,_tmp1D2,_tmp1D2 + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1211
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3U,& Cyc_InsertChecks_warn_all_null_deref};static char _tmp1D3[16U]="-warnnullchecks";static char _tmp1D4[1U]="";static char _tmp1D5[45U]="Warn when any null check can't be eliminated";struct _tuple29 Cyc_opt55_tuple={{_tmp1D3,_tmp1D3,_tmp1D3 + 16U},0,{_tmp1D4,_tmp1D4,_tmp1D4 + 1U},(void*)& Cyc_opt55_spec,{_tmp1D5,_tmp1D5,_tmp1D5 + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1214
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3U,& Cyc_Tcutil_warn_alias_coerce};static char _tmp1D6[19U]="-warnaliascoercion";static char _tmp1D7[1U]="";static char _tmp1D8[41U]="Warn when any alias coercion is inserted";struct _tuple29 Cyc_opt56_tuple={{_tmp1D6,_tmp1D6,_tmp1D6 + 19U},0,{_tmp1D7,_tmp1D7,_tmp1D7 + 1U},(void*)& Cyc_opt56_spec,{_tmp1D8,_tmp1D8,_tmp1D8 + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1217
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3U,& Cyc_Tcutil_warn_region_coerce};static char _tmp1D9[18U]="-warnregioncoerce";static char _tmp1DA[1U]="";static char _tmp1DB[42U]="Warn when any region coercion is inserted";struct _tuple29 Cyc_opt57_tuple={{_tmp1D9,_tmp1D9,_tmp1D9 + 18U},0,{_tmp1DA,_tmp1DA,_tmp1DA + 1U},(void*)& Cyc_opt57_spec,{_tmp1DB,_tmp1DB,_tmp1DB + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1220
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3U,& Cyc_Parse_no_register};static char _tmp1DC[12U]="-noregister";static char _tmp1DD[1U]="";static char _tmp1DE[39U]="Treat register storage class as public";struct _tuple29 Cyc_opt58_tuple={{_tmp1DC,_tmp1DC,_tmp1DC + 12U},0,{_tmp1DD,_tmp1DD,_tmp1DD + 1U},(void*)& Cyc_opt58_spec,{_tmp1DE,_tmp1DE,_tmp1DE + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1223
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4U,& Cyc_Position_use_gcc_style_location};static char _tmp1DF[18U]="-detailedlocation";static char _tmp1E0[1U]="";static char _tmp1E1[58U]="Try to give more detailed location information for errors";struct _tuple29 Cyc_opt59_tuple={{_tmp1DF,_tmp1DF,_tmp1DF + 18U},0,{_tmp1E0,_tmp1E0,_tmp1E0 + 1U},(void*)& Cyc_opt59_spec,{_tmp1E1,_tmp1E1,_tmp1E1 + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1226
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0U,Cyc_set_port_c_code};static char _tmp1E2[6U]="-port";static char _tmp1E3[1U]="";static char _tmp1E4[38U]="Suggest how to port C code to Cyclone";struct _tuple29 Cyc_opt60_tuple={{_tmp1E2,_tmp1E2,_tmp1E2 + 6U},0,{_tmp1E3,_tmp1E3,_tmp1E3 + 1U},(void*)& Cyc_opt60_spec,{_tmp1E4,_tmp1E4,_tmp1E4 + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1229
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3U,& Cyc_Absyn_no_regions};static char _tmp1E5[11U]="-noregions";static char _tmp1E6[1U]="";static char _tmp1E7[39U]="Generate code that doesn't use regions";struct _tuple29 Cyc_opt61_tuple={{_tmp1E5,_tmp1E5,_tmp1E5 + 11U},0,{_tmp1E6,_tmp1E6,_tmp1E6 + 1U},(void*)& Cyc_opt61_spec,{_tmp1E7,_tmp1E7,_tmp1E7 + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1232
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5U,Cyc_add_iprefix};static char _tmp1E8[9U]="-iprefix";static char _tmp1E9[9U]="<prefix>";static char _tmp1EA[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple29 Cyc_opt62_tuple={{_tmp1E8,_tmp1E8,_tmp1E8 + 9U},0,{_tmp1E9,_tmp1E9,_tmp1E9 + 9U},(void*)& Cyc_opt62_spec,{_tmp1EA,_tmp1EA,_tmp1EA + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1235
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5U,Cyc_add_iwithprefix};static char _tmp1EB[13U]="-iwithprefix";static char _tmp1EC[6U]="<dir>";static char _tmp1ED[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple29 Cyc_opt63_tuple={{_tmp1EB,_tmp1EB,_tmp1EB + 13U},0,{_tmp1EC,_tmp1EC,_tmp1EC + 6U},(void*)& Cyc_opt63_spec,{_tmp1ED,_tmp1ED,_tmp1ED + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1238
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5U,Cyc_add_iwithprefixbefore};static char _tmp1EE[19U]="-iwithprefixbefore";static char _tmp1EF[6U]="<dir>";static char _tmp1F0[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple29 Cyc_opt64_tuple={{_tmp1EE,_tmp1EE,_tmp1EE + 19U},0,{_tmp1EF,_tmp1EF,_tmp1EF + 6U},(void*)& Cyc_opt64_spec,{_tmp1F0,_tmp1F0,_tmp1F0 + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1241
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5U,Cyc_add_isystem};static char _tmp1F1[9U]="-isystem";static char _tmp1F2[6U]="<dir>";static char _tmp1F3[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple29 Cyc_opt65_tuple={{_tmp1F1,_tmp1F1,_tmp1F1 + 9U},0,{_tmp1F2,_tmp1F2,_tmp1F2 + 6U},(void*)& Cyc_opt65_spec,{_tmp1F3,_tmp1F3,_tmp1F3 + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1244
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5U,Cyc_add_idirafter};static char _tmp1F4[11U]="-idirafter";static char _tmp1F5[6U]="<dir>";static char _tmp1F6[46U]="Add the directory to the second include path.";struct _tuple29 Cyc_opt66_tuple={{_tmp1F4,_tmp1F4,_tmp1F4 + 11U},0,{_tmp1F5,_tmp1F5,_tmp1F5 + 6U},(void*)& Cyc_opt66_spec,{_tmp1F6,_tmp1F6,_tmp1F6 + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1247
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3U,& Cyc_generate_interface_r};static char _tmp1F7[15U]="--geninterface";static char _tmp1F8[1U]="";static char _tmp1F9[25U]="Generate interface files";struct _tuple29 Cyc_opt67_tuple={{_tmp1F7,_tmp1F7,_tmp1F7 + 15U},0,{_tmp1F8,_tmp1F8,_tmp1F8 + 1U},(void*)& Cyc_opt67_spec,{_tmp1F9,_tmp1F9,_tmp1F9 + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1250
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5U,Cyc_set_specified_interface};static char _tmp1FA[12U]="--interface";static char _tmp1FB[8U]=" <file>";static char _tmp1FC[37U]="Set the interface file to be <file>.";struct _tuple29 Cyc_opt68_tuple={{_tmp1FA,_tmp1FA,_tmp1FA + 12U},0,{_tmp1FB,_tmp1FB,_tmp1FB + 8U},(void*)& Cyc_opt68_spec,{_tmp1FC,_tmp1FC,_tmp1FC + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1253
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0U,Cyc_set_many_errors};static char _tmp1FD[13U]="--manyerrors";static char _tmp1FE[1U]="";static char _tmp1FF[36U]="don't stop after only a few errors.";struct _tuple29 Cyc_opt69_tuple={{_tmp1FD,_tmp1FD,_tmp1FD + 13U},0,{_tmp1FE,_tmp1FE,_tmp1FE + 1U},(void*)& Cyc_opt69_spec,{_tmp1FF,_tmp1FF,_tmp1FF + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1256
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3U,& Cyc_ParseErrors_print_state_and_token};static char _tmp200[13U]="--parsestate";static char _tmp201[1U]="";static char _tmp202[50U]="print the parse state and token on a syntax error";struct _tuple29 Cyc_opt70_tuple={{_tmp200,_tmp200,_tmp200 + 13U},0,{_tmp201,_tmp201,_tmp201 + 1U},(void*)& Cyc_opt70_spec,{_tmp202,_tmp202,_tmp202 + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1259
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0U,Cyc_noassume_gcc_flag};static char _tmp203[22U]="-known-gcc-flags-only";static char _tmp204[1U]="";static char _tmp205[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple29 Cyc_opt71_tuple={{_tmp203,_tmp203,_tmp203 + 22U},0,{_tmp204,_tmp204,_tmp204 + 1U},(void*)& Cyc_opt71_spec,{_tmp205,_tmp205,_tmp205 + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1262
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0U,Cyc_print_options};static char _tmp206[6U]="-help";static char _tmp207[1U]="";static char _tmp208[32U]="print out the available options";struct _tuple29 Cyc_opt72_tuple={{_tmp206,_tmp206,_tmp206 + 6U},0,{_tmp207,_tmp207,_tmp207 + 1U},(void*)& Cyc_opt72_spec,{_tmp208,_tmp208,_tmp208 + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1265
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0U,Cyc_print_options};static char _tmp209[7U]="-usage";static char _tmp20A[1U]="";static char _tmp20B[32U]="print out the available options";struct _tuple29 Cyc_opt73_tuple={{_tmp209,_tmp209,_tmp209 + 7U},0,{_tmp20A,_tmp20A,_tmp20A + 1U},(void*)& Cyc_opt73_spec,{_tmp20B,_tmp20B,_tmp20B + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1268
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0U,Cyc_set_notoseqc};static char _tmp20C[10U]="-no-seq-c";static char _tmp20D[1U]="";static char _tmp20E[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple29 Cyc_opt74_tuple={{_tmp20C,_tmp20C,_tmp20C + 10U},0,{_tmp20D,_tmp20D,_tmp20D + 1U},(void*)& Cyc_opt74_spec,{_tmp20E,_tmp20E,_tmp20E + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1271
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt75_spec={0U,Cyc_set_pg};static char _tmp20F[4U]="-pg";static char _tmp210[1U]="";static char _tmp211[33U]="Compile for profiling with gprof";struct _tuple29 Cyc_opt75_tuple={{_tmp20F,_tmp20F,_tmp20F + 4U},0,{_tmp210,_tmp210,_tmp210 + 1U},(void*)& Cyc_opt75_spec,{_tmp211,_tmp211,_tmp211 + 33U}};struct Cyc_List_List Cyc_opt75={(void*)& Cyc_opt75_tuple,0};
# 1274
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt76_spec={3U,& Cyc_vcgen_r};static char _tmp212[8U]="--vcgen";static char _tmp213[1U]="";static char _tmp214[32U]="compute verification-conditions";struct _tuple29 Cyc_opt76_tuple={{_tmp212,_tmp212,_tmp212 + 8U},0,{_tmp213,_tmp213,_tmp213 + 1U},(void*)& Cyc_opt76_spec,{_tmp214,_tmp214,_tmp214 + 32U}};struct Cyc_List_List Cyc_opt76={(void*)& Cyc_opt76_tuple,0};
# 1278
struct Cyc_List_List*Cyc_global_options[76U]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt40,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73,& Cyc_opt74,& Cyc_opt75,& Cyc_opt76};
# 1291
void Cyc_print_options (void){
({struct Cyc_List_List*_tmp367=Cyc_global_options[0];Cyc_Arg_usage(_tmp367,({const char*_tmp215="<program.cyc>";_tag_fat(_tmp215,sizeof(char),14U);}));});}
# 1296
int Cyc_main(int argc,struct _fat_ptr argv){
# 1300
GC_blacklist_warn_clear();{
# 1303
struct _fat_ptr optstring=({const char*_tmp296="Options:";_tag_fat(_tmp296,sizeof(char),9U);});
# 1305
{int i=1;for(0;(unsigned)i < 76U;++ i){
(*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,76U,sizeof(struct Cyc_List_List*),i - 1)))->tl=Cyc_global_options[i];}}{
# 1308
struct Cyc_List_List*options=Cyc_global_options[0];
# 1310
struct _fat_ptr _tmp216=Cyc_Specsfile_parse_b(options,Cyc_add_other,Cyc_add_other_flag,optstring,argv);struct _fat_ptr otherargs=_tmp216;
# 1316
struct _fat_ptr _tmp217=Cyc_find_in_arch_path(({const char*_tmp295="cycspecs";_tag_fat(_tmp295,sizeof(char),9U);}));struct _fat_ptr specs_file=_tmp217;
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp21A=({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0U,_tmp2B9.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp2B9;});void*_tmp218[1U];_tmp218[0]=& _tmp21A;({struct Cyc___cycFILE*_tmp369=Cyc_stderr;struct _fat_ptr _tmp368=({const char*_tmp219="Reading from specs file %s\n";_tag_fat(_tmp219,sizeof(char),28U);});Cyc_fprintf(_tmp369,_tmp368,_tag_fat(_tmp218,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp21B=Cyc_Specsfile_read_specs(specs_file);struct Cyc_List_List*specs=_tmp21B;
# 1320
struct _fat_ptr _tmp21C=Cyc_Specsfile_split_specs(({struct Cyc_List_List*_tmp36A=specs;Cyc_Specsfile_get_spec(_tmp36A,({const char*_tmp294="cyclone";_tag_fat(_tmp294,sizeof(char),8U);}));}));struct _fat_ptr args_from_specs_file=_tmp21C;
if(({struct _fat_ptr*_tmp36B=(struct _fat_ptr*)args_from_specs_file.curr;_tmp36B != (struct _fat_ptr*)(_tag_fat(0,0,0)).curr;})){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,args_from_specs_file);}{
# 1325
struct _fat_ptr _tmp21D=({struct Cyc_List_List*_tmp36C=specs;Cyc_Specsfile_get_spec(_tmp36C,({const char*_tmp293="cyclone_target_cflags";_tag_fat(_tmp293,sizeof(char),22U);}));});struct _fat_ptr target_cflags=_tmp21D;
struct _fat_ptr _tmp21E=({struct Cyc_List_List*_tmp36D=specs;Cyc_Specsfile_get_spec(_tmp36D,({const char*_tmp292="cyclone_cc";_tag_fat(_tmp292,sizeof(char),11U);}));});struct _fat_ptr cyclone_cc=_tmp21E;
if(!((unsigned)cyclone_cc.curr))cyclone_cc=({const char*_tmp21F="gcc";_tag_fat(_tmp21F,sizeof(char),4U);});
Cyc_def_inc_path=({struct Cyc_List_List*_tmp36E=specs;Cyc_Specsfile_get_spec(_tmp36E,({const char*_tmp220="cyclone_inc_path";_tag_fat(_tmp220,sizeof(char),17U);}));});
# 1331
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,otherargs);
if(({struct _fat_ptr _tmp36F=(struct _fat_ptr)Cyc_cpp;Cyc_strcmp(_tmp36F,({const char*_tmp221="";_tag_fat(_tmp221,sizeof(char),1U);}));})== 0){
# 1349 "cyclone.cyc"
const char*_tmp222=Cyc_produce_dependencies?"":" -E";const char*dash_E=_tmp222;
Cyc_set_cpp((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp225=({struct Cyc_String_pa_PrintArg_struct _tmp2BD;_tmp2BD.tag=0U,_tmp2BD.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_tmp2BD;});struct Cyc_String_pa_PrintArg_struct _tmp226=({struct Cyc_String_pa_PrintArg_struct _tmp2BC;_tmp2BC.tag=0U,_tmp2BC.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_tmp2BC;});struct Cyc_String_pa_PrintArg_struct _tmp227=({struct Cyc_String_pa_PrintArg_struct _tmp2BB;_tmp2BB.tag=0U,({
# 1352
struct _fat_ptr _tmp370=(struct _fat_ptr)({const char*_tmp229=dash_E;_tag_fat(_tmp229,sizeof(char),_get_zero_arr_size_char((void*)_tmp229,1U));});_tmp2BB.f1=_tmp370;});_tmp2BB;});struct Cyc_String_pa_PrintArg_struct _tmp228=({struct Cyc_String_pa_PrintArg_struct _tmp2BA;_tmp2BA.tag=0U,_tmp2BA.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp2BA;});void*_tmp223[4U];_tmp223[0]=& _tmp225,_tmp223[1]=& _tmp226,_tmp223[2]=& _tmp227,_tmp223[3]=& _tmp228;({struct _fat_ptr _tmp371=({const char*_tmp224="%s %s -w -x c%s -specs %s";_tag_fat(_tmp224,sizeof(char),26U);});Cyc_aprintf(_tmp371,_tag_fat(_tmp223,sizeof(void*),4U));});}));}
# 1354
if(Cyc_cyclone_files == 0 && Cyc_ccargs == 0){
({void*_tmp22A=0U;({struct Cyc___cycFILE*_tmp373=Cyc_stderr;struct _fat_ptr _tmp372=({const char*_tmp22B="missing file\n";_tag_fat(_tmp22B,sizeof(char),14U);});Cyc_fprintf(_tmp373,_tmp372,_tag_fat(_tmp22A,sizeof(void*),0U));});});
exit(1);}
# 1363
if(!Cyc_stop_after_cpp_r && !Cyc_nocyc_setjmp_r)
Cyc_cyc_setjmp=Cyc_find_in_arch_path(({const char*_tmp22C="cyc_setjmp.h";_tag_fat(_tmp22C,sizeof(char),13U);}));
if(!Cyc_stop_after_cpp_r && Cyc_strlen((struct _fat_ptr)Cyc_cyc_include)== (unsigned long)0)
Cyc_cyc_include=Cyc_find_in_exec_path(({const char*_tmp22D="cyc_include.h";_tag_fat(_tmp22D,sizeof(char),14U);}));
# 1369
{struct _handler_cons _tmp22E;_push_handler(& _tmp22E);{int _tmp230=0;if(setjmp(_tmp22E.handler))_tmp230=1;if(!_tmp230){
{struct Cyc_List_List*_tmp231=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);struct Cyc_List_List*l=_tmp231;for(0;l != 0;l=l->tl){
Cyc_process_file(*((struct _fat_ptr*)l->hd));
if(Cyc_compile_failure){int _tmp232=1;_npop_handler(0U);return _tmp232;}}}
# 1370
;_pop_handler();}else{void*_tmp22F=(void*)Cyc_Core_get_exn_thrown();void*_tmp233=_tmp22F;void*_tmp234;_LL1: _tmp234=_tmp233;_LL2: {void*x=_tmp234;
# 1376
if(Cyc_compile_failure)return 1;else{
Cyc_Core_rethrow(x);}}_LL0:;}}}
# 1380
if(((Cyc_stop_after_cpp_r || Cyc_parseonly_r)|| Cyc_tc_r)|| Cyc_toc_r)return 0;
# 1386
if(Cyc_ccargs == 0)return 0;
# 1389
if((unsigned)Cyc_Specsfile_target_arch.curr &&(unsigned)Cyc_Specsfile_cyclone_exec_path)
Cyc_add_ccarg((struct _fat_ptr)({struct Cyc_List_List*_tmp377=({struct Cyc_List_List*_tmp237=_cycalloc(sizeof(*_tmp237));({struct _fat_ptr*_tmp376=({struct _fat_ptr*_tmp236=_cycalloc(sizeof(*_tmp236));({struct _fat_ptr _tmp375=({const char*_tmp235="";_tag_fat(_tmp235,sizeof(char),1U);});*_tmp236=_tmp375;});_tmp236;});_tmp237->hd=_tmp376;}),({
struct Cyc_List_List*_tmp374=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_tmp237->tl=_tmp374;});_tmp237;});
# 1390
Cyc_str_sepstr(_tmp377,({const char*_tmp238=" -L";_tag_fat(_tmp238,sizeof(char),4U);}));}));
# 1393
Cyc_add_ccarg((struct _fat_ptr)({struct _fat_ptr _tmp378=({const char*_tmp239="-L";_tag_fat(_tmp239,sizeof(char),3U);});Cyc_strconcat(_tmp378,(struct _fat_ptr)Cyc_Specsfile_def_lib_path);}));
# 1395
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
struct _fat_ptr _tmp23A=({struct Cyc_List_List*_tmp379=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs);Cyc_str_sepstr(_tmp379,({const char*_tmp291=" ";_tag_fat(_tmp291,sizeof(char),2U);}));});struct _fat_ptr ccargs_string=_tmp23A;
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
struct _fat_ptr _tmp23B=({struct Cyc_List_List*_tmp37D=({struct Cyc_List_List*_tmp28F=_cycalloc(sizeof(*_tmp28F));({struct _fat_ptr*_tmp37C=({struct _fat_ptr*_tmp28E=_cycalloc(sizeof(*_tmp28E));({struct _fat_ptr _tmp37B=({const char*_tmp28D="";_tag_fat(_tmp28D,sizeof(char),1U);});*_tmp28E=_tmp37B;});_tmp28E;});_tmp28F->hd=_tmp37C;}),({
struct Cyc_List_List*_tmp37A=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp28F->tl=_tmp37A;});_tmp28F;});
# 1398
Cyc_str_sepstr(_tmp37D,({const char*_tmp290=" ";_tag_fat(_tmp290,sizeof(char),2U);}));});struct _fat_ptr libargs_string=_tmp23B;
# 1401
struct Cyc_List_List*stdlib;
struct _fat_ptr stdlib_string;
int _tmp23C=
((Cyc_stop_after_asmfile_r || Cyc_stop_after_objectfile_r)||
# 1406
 Cyc_output_file != 0 &&({struct _fat_ptr _tmp37F=*((struct _fat_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp37F,({const char*_tmp28B=".a";_tag_fat(_tmp28B,sizeof(char),3U);}));}))||
 Cyc_output_file != 0 &&({struct _fat_ptr _tmp37E=*((struct _fat_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp37E,({const char*_tmp28C=".lib";_tag_fat(_tmp28C,sizeof(char),5U);}));});
# 1403
int is_not_executable=_tmp23C;
# 1408
if(is_not_executable){
stdlib=0;
stdlib_string=(struct _fat_ptr)({const char*_tmp23D="";_tag_fat(_tmp23D,sizeof(char),1U);});}else{
# 1413
struct _fat_ptr libcyc_flag;
struct _fat_ptr nogc_filename;
struct _fat_ptr runtime_filename;
struct _fat_ptr gc_filename;
if(Cyc_pa_r){
libcyc_flag=({const char*_tmp23E="-lcyc_a";_tag_fat(_tmp23E,sizeof(char),8U);});
nogc_filename=({const char*_tmp23F="nogc_a.a";_tag_fat(_tmp23F,sizeof(char),9U);});
runtime_filename=({const char*_tmp240="runtime_cyc_a.a";_tag_fat(_tmp240,sizeof(char),16U);});}else{
if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp241="-lcyc_nocheck";_tag_fat(_tmp241,sizeof(char),14U);});
nogc_filename=({const char*_tmp242="nogc.a";_tag_fat(_tmp242,sizeof(char),7U);});
runtime_filename=({const char*_tmp243="runtime_cyc.a";_tag_fat(_tmp243,sizeof(char),14U);});}else{
if(Cyc_pg_r){
libcyc_flag=({const char*_tmp244="-lcyc_pg";_tag_fat(_tmp244,sizeof(char),9U);});
runtime_filename=({const char*_tmp245="runtime_cyc_pg.a";_tag_fat(_tmp245,sizeof(char),17U);});
nogc_filename=({const char*_tmp246="nogc_pg.a";_tag_fat(_tmp246,sizeof(char),10U);});}else{
if(Cyc_Lex_compile_for_boot_r){
# 1431
libcyc_flag=({const char*_tmp247="-lcycboot";_tag_fat(_tmp247,sizeof(char),10U);});
nogc_filename=({const char*_tmp248="nogc.a";_tag_fat(_tmp248,sizeof(char),7U);});
runtime_filename=({const char*_tmp249="runtime_cyc.a";_tag_fat(_tmp249,sizeof(char),14U);});}else{
if(Cyc_pthread_r){
libcyc_flag=({const char*_tmp24A="-lcyc";_tag_fat(_tmp24A,sizeof(char),6U);});
nogc_filename=({const char*_tmp24B="nogc_pthread.a";_tag_fat(_tmp24B,sizeof(char),15U);});
runtime_filename=({const char*_tmp24C="runtime_cyc_pthread.a";_tag_fat(_tmp24C,sizeof(char),22U);});}else{
# 1439
libcyc_flag=({const char*_tmp24D="-lcyc";_tag_fat(_tmp24D,sizeof(char),6U);});
nogc_filename=({const char*_tmp24E="nogc.a";_tag_fat(_tmp24E,sizeof(char),7U);});
runtime_filename=({const char*_tmp24F="runtime_cyc.a";_tag_fat(_tmp24F,sizeof(char),14U);});}}}}}
# 1443
if(!Cyc_pthread_r)
gc_filename=({const char*_tmp250="gc.a";_tag_fat(_tmp250,sizeof(char),5U);});else{
# 1446
gc_filename=({const char*_tmp251="gc_pthread.a";_tag_fat(_tmp251,sizeof(char),13U);});}{
# 1448
struct _fat_ptr _tmp252=Cyc_nogc_r?Cyc_find_in_arch_path(nogc_filename): Cyc_find_in_arch_path(gc_filename);struct _fat_ptr gc=_tmp252;
struct _fat_ptr _tmp253=Cyc_find_in_arch_path(runtime_filename);struct _fat_ptr runtime=_tmp253;
# 1452
stdlib=0;
stdlib_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp256=({struct Cyc_String_pa_PrintArg_struct _tmp2C0;_tmp2C0.tag=0U,_tmp2C0.f1=(struct _fat_ptr)((struct _fat_ptr)libcyc_flag);_tmp2C0;});struct Cyc_String_pa_PrintArg_struct _tmp257=({struct Cyc_String_pa_PrintArg_struct _tmp2BF;_tmp2BF.tag=0U,_tmp2BF.f1=(struct _fat_ptr)((struct _fat_ptr)runtime);_tmp2BF;});struct Cyc_String_pa_PrintArg_struct _tmp258=({struct Cyc_String_pa_PrintArg_struct _tmp2BE;_tmp2BE.tag=0U,_tmp2BE.f1=(struct _fat_ptr)((struct _fat_ptr)gc);_tmp2BE;});void*_tmp254[3U];_tmp254[0]=& _tmp256,_tmp254[1]=& _tmp257,_tmp254[2]=& _tmp258;({struct _fat_ptr _tmp380=({const char*_tmp255=" %s %s %s ";_tag_fat(_tmp255,sizeof(char),11U);});Cyc_aprintf(_tmp380,_tag_fat(_tmp254,sizeof(void*),3U));});});}}
# 1457
if(Cyc_ic_r){struct _handler_cons _tmp259;_push_handler(& _tmp259);{int _tmp25B=0;if(setjmp(_tmp259.handler))_tmp25B=1;if(!_tmp25B){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp25C=({struct Cyc_List_List*_tmp381=stdlib;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp381,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));});struct Cyc_List_List*lf=_tmp25C;
if(!is_not_executable)
lf=({struct Cyc_List_List*_tmp25D=_cycalloc(sizeof(*_tmp25D));_tmp25D->hd=Cyc_final_strptr,_tmp25D->tl=lf;_tmp25D;});{
# 1465
struct Cyc_Interface_I*_tmp25E=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _fat_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,lf,lf);struct Cyc_Interface_I*i=_tmp25E;
if(i == 0){
({void*_tmp25F=0U;({struct Cyc___cycFILE*_tmp383=Cyc_stderr;struct _fat_ptr _tmp382=({const char*_tmp260="Error: interfaces incompatible\n";_tag_fat(_tmp260,sizeof(char),32U);});Cyc_fprintf(_tmp383,_tmp382,_tag_fat(_tmp25F,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp261=1;_npop_handler(0U);return _tmp261;}}
# 1472
if(is_not_executable){
if(Cyc_output_file != 0){
struct _fat_ptr _tmp262=({struct Cyc_String_pa_PrintArg_struct _tmp269=({struct Cyc_String_pa_PrintArg_struct _tmp2C1;_tmp2C1.tag=0U,({struct _fat_ptr _tmp384=(struct _fat_ptr)((struct _fat_ptr)Cyc_Filename_chop_extension(*((struct _fat_ptr*)_check_null(Cyc_output_file))));_tmp2C1.f1=_tmp384;});_tmp2C1;});void*_tmp267[1U];_tmp267[0]=& _tmp269;({struct _fat_ptr _tmp385=({const char*_tmp268="%s.cycio";_tag_fat(_tmp268,sizeof(char),9U);});Cyc_aprintf(_tmp385,_tag_fat(_tmp267,sizeof(void*),1U));});});struct _fat_ptr output_file_io=_tmp262;
struct Cyc___cycFILE*_tmp263=({struct _fat_ptr _tmp387=(struct _fat_ptr)output_file_io;struct _fat_ptr _tmp386=({const char*_tmp265="wb";_tag_fat(_tmp265,sizeof(char),3U);});Cyc_try_file_open(_tmp387,_tmp386,({const char*_tmp266="interface object file";_tag_fat(_tmp266,sizeof(char),22U);}));});struct Cyc___cycFILE*f=_tmp263;
if(f == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp264=1;_npop_handler(0U);return _tmp264;}}
# 1481
Cyc_Interface_save(i,f);
Cyc_file_close(f);}}else{
# 1485
if(!({struct Cyc_Interface_I*_tmp38B=Cyc_Interface_empty();struct Cyc_Interface_I*_tmp38A=i;Cyc_Interface_is_subinterface(_tmp38B,_tmp38A,({struct _tuple25*_tmp26C=_cycalloc(sizeof(*_tmp26C));
({struct _fat_ptr _tmp389=({const char*_tmp26A="empty interface";_tag_fat(_tmp26A,sizeof(char),16U);});_tmp26C->f1=_tmp389;}),({struct _fat_ptr _tmp388=({const char*_tmp26B="global interface";_tag_fat(_tmp26B,sizeof(char),17U);});_tmp26C->f2=_tmp388;});_tmp26C;}));})){
({void*_tmp26D=0U;({struct Cyc___cycFILE*_tmp38D=Cyc_stderr;struct _fat_ptr _tmp38C=({const char*_tmp26E="Error: some objects are still undefined\n";_tag_fat(_tmp26E,sizeof(char),41U);});Cyc_fprintf(_tmp38D,_tmp38C,_tag_fat(_tmp26D,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp26F=1;_npop_handler(0U);return _tmp26F;}}}}}
# 1458
;_pop_handler();}else{void*_tmp25A=(void*)Cyc_Core_get_exn_thrown();void*_tmp270=_tmp25A;void*_tmp271;_LL6: _tmp271=_tmp270;_LL7: {void*e=_tmp271;
# 1495
({struct Cyc_String_pa_PrintArg_struct _tmp274=({struct Cyc_String_pa_PrintArg_struct _tmp2C4;_tmp2C4.tag=0U,({struct _fat_ptr _tmp38E=(struct _fat_ptr)({const char*_tmp278=Cyc_Core_get_exn_name(e);_tag_fat(_tmp278,sizeof(char),_get_zero_arr_size_char((void*)_tmp278,1U));});_tmp2C4.f1=_tmp38E;});_tmp2C4;});struct Cyc_String_pa_PrintArg_struct _tmp275=({struct Cyc_String_pa_PrintArg_struct _tmp2C3;_tmp2C3.tag=0U,({struct _fat_ptr _tmp38F=(struct _fat_ptr)({const char*_tmp277=Cyc_Core_get_exn_filename();_tag_fat(_tmp277,sizeof(char),_get_zero_arr_size_char((void*)_tmp277,1U));});_tmp2C3.f1=_tmp38F;});_tmp2C3;});struct Cyc_Int_pa_PrintArg_struct _tmp276=({struct Cyc_Int_pa_PrintArg_struct _tmp2C2;_tmp2C2.tag=1U,({unsigned long _tmp390=(unsigned long)Cyc_Core_get_exn_lineno();_tmp2C2.f1=_tmp390;});_tmp2C2;});void*_tmp272[3U];_tmp272[0]=& _tmp274,_tmp272[1]=& _tmp275,_tmp272[2]=& _tmp276;({struct Cyc___cycFILE*_tmp392=Cyc_stderr;struct _fat_ptr _tmp391=({const char*_tmp273="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu";_tag_fat(_tmp273,sizeof(char),131U);});Cyc_fprintf(_tmp392,_tmp391,_tag_fat(_tmp272,sizeof(void*),3U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}_LL5:;}}}{
# 1501
struct _fat_ptr outfile_str=({const char*_tmp28A="";_tag_fat(_tmp28A,sizeof(char),1U);});
if(Cyc_output_file != 0)
outfile_str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp27B=({struct Cyc_String_pa_PrintArg_struct _tmp2C5;_tmp2C5.tag=0U,({struct _fat_ptr _tmp393=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(*((struct _fat_ptr*)_check_null(Cyc_output_file))));_tmp2C5.f1=_tmp393;});_tmp2C5;});void*_tmp279[1U];_tmp279[0]=& _tmp27B;({struct _fat_ptr _tmp394=({const char*_tmp27A=" -o %s";_tag_fat(_tmp27A,sizeof(char),7U);});Cyc_aprintf(_tmp394,_tag_fat(_tmp279,sizeof(void*),1U));});});{
# 1505
struct _fat_ptr _tmp27C=({struct Cyc_String_pa_PrintArg_struct _tmp284=({struct Cyc_String_pa_PrintArg_struct _tmp2CC;_tmp2CC.tag=0U,_tmp2CC.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_tmp2CC;});struct Cyc_String_pa_PrintArg_struct _tmp285=({struct Cyc_String_pa_PrintArg_struct _tmp2CB;_tmp2CB.tag=0U,_tmp2CB.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_tmp2CB;});struct Cyc_String_pa_PrintArg_struct _tmp286=({struct Cyc_String_pa_PrintArg_struct _tmp2CA;_tmp2CA.tag=0U,_tmp2CA.f1=(struct _fat_ptr)((struct _fat_ptr)ccargs_string);_tmp2CA;});struct Cyc_String_pa_PrintArg_struct _tmp287=({struct Cyc_String_pa_PrintArg_struct _tmp2C9;_tmp2C9.tag=0U,_tmp2C9.f1=(struct _fat_ptr)((struct _fat_ptr)outfile_str);_tmp2C9;});struct Cyc_String_pa_PrintArg_struct _tmp288=({struct Cyc_String_pa_PrintArg_struct _tmp2C8;_tmp2C8.tag=0U,_tmp2C8.f1=(struct _fat_ptr)((struct _fat_ptr)stdlib_string);_tmp2C8;});struct Cyc_String_pa_PrintArg_struct _tmp289=({struct Cyc_String_pa_PrintArg_struct _tmp2C7;_tmp2C7.tag=0U,_tmp2C7.f1=(struct _fat_ptr)((struct _fat_ptr)libargs_string);_tmp2C7;});void*_tmp282[6U];_tmp282[0]=& _tmp284,_tmp282[1]=& _tmp285,_tmp282[2]=& _tmp286,_tmp282[3]=& _tmp287,_tmp282[4]=& _tmp288,_tmp282[5]=& _tmp289;({struct _fat_ptr _tmp395=({const char*_tmp283="%s %s %s%s %s%s";_tag_fat(_tmp283,sizeof(char),16U);});Cyc_aprintf(_tmp395,_tag_fat(_tmp282,sizeof(void*),6U));});});struct _fat_ptr cccmd=_tmp27C;
# 1510
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _tmp27F=({struct Cyc_String_pa_PrintArg_struct _tmp2C6;_tmp2C6.tag=0U,_tmp2C6.f1=(struct _fat_ptr)((struct _fat_ptr)cccmd);_tmp2C6;});void*_tmp27D[1U];_tmp27D[0]=& _tmp27F;({struct Cyc___cycFILE*_tmp397=Cyc_stderr;struct _fat_ptr _tmp396=({const char*_tmp27E="%s\n";_tag_fat(_tmp27E,sizeof(char),4U);});Cyc_fprintf(_tmp397,_tmp396,_tag_fat(_tmp27D,sizeof(void*),1U));});});Cyc_fflush(Cyc_stderr);}
if(system((const char*)_check_null(_untag_fat_ptr(cccmd,sizeof(char),1U)))!= 0){
({void*_tmp280=0U;({struct Cyc___cycFILE*_tmp399=Cyc_stderr;struct _fat_ptr _tmp398=({const char*_tmp281="Error: C compiler failed\n";_tag_fat(_tmp281,sizeof(char),26U);});Cyc_fprintf(_tmp399,_tmp398,_tag_fat(_tmp280,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}
# 1517
Cyc_remove_cfiles();
# 1519
return Cyc_compile_failure?1: 0;}}}}}}}}}
