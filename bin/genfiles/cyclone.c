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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};
# 275 "core.h"
extern void Cyc_Core_rethrow(void*);
# 279
extern const char*Cyc_Core_get_exn_name(void*);
# 281
extern const char*Cyc_Core_get_exn_filename (void);
# 288
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
extern void exit(int);
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
# 1165 "absyn.h"
extern int Cyc_Absyn_porting_c_code;
# 1167
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
void Cyc_Binding_resolve_all(struct Cyc_List_List*tds);struct Cyc_Binding_Namespace_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_Using_Binding_NSDirective_struct{int tag;struct Cyc_List_List*f1;};struct Cyc_Binding_NSCtxt{struct Cyc_List_List*curr_ns;struct Cyc_List_List*availables;struct Cyc_Dict_Dict ns_data;};struct _union_RelnOp_RConst{int tag;unsigned val;};struct _union_RelnOp_RVar{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RNumelts{int tag;struct Cyc_Absyn_Vardecl*val;};struct _union_RelnOp_RType{int tag;void*val;};struct _union_RelnOp_RParam{int tag;unsigned val;};struct _union_RelnOp_RParamNumelts{int tag;unsigned val;};struct _union_RelnOp_RReturn{int tag;unsigned val;};union Cyc_Relations_RelnOp{struct _union_RelnOp_RConst RConst;struct _union_RelnOp_RVar RVar;struct _union_RelnOp_RNumelts RNumelts;struct _union_RelnOp_RType RType;struct _union_RelnOp_RParam RParam;struct _union_RelnOp_RParamNumelts RParamNumelts;struct _union_RelnOp_RReturn RReturn;};
# 50 "relations-ap.h"
enum Cyc_Relations_Relation{Cyc_Relations_Req =0U,Cyc_Relations_Rneq =1U,Cyc_Relations_Rlte =2U,Cyc_Relations_Rlt =3U};struct Cyc_Relations_Reln{union Cyc_Relations_RelnOp rop1;enum Cyc_Relations_Relation relation;union Cyc_Relations_RelnOp rop2;};struct Cyc_RgnOrder_RgnPO;
# 130 "tcutil.h"
extern int Cyc_Tcutil_warn_alias_coerce;
# 133
extern int Cyc_Tcutil_warn_region_coerce;extern char Cyc_Tcenv_Env_error[10U];struct Cyc_Tcenv_Env_error_exn_struct{char*tag;};struct Cyc_Tcenv_Genv{struct Cyc_Dict_Dict aggrdecls;struct Cyc_Dict_Dict datatypedecls;struct Cyc_Dict_Dict enumdecls;struct Cyc_Dict_Dict typedefs;struct Cyc_Dict_Dict ordinaries;};struct Cyc_Tcenv_Fenv;struct Cyc_Tcenv_Tenv{struct Cyc_List_List*ns;struct Cyc_Tcenv_Genv*ae;struct Cyc_Tcenv_Fenv*le;int allow_valueof: 1;int in_extern_c_include: 1;int in_tempest: 1;int tempest_generalize: 1;int in_extern_c_inc_repeat: 1;};
# 69 "tcenv.h"
struct Cyc_Tcenv_Tenv*Cyc_Tcenv_tc_init (void);
# 89
enum Cyc_Tcenv_NewStatus{Cyc_Tcenv_NoneNew =0U,Cyc_Tcenv_InNew =1U,Cyc_Tcenv_InNewAggr =2U};
# 31 "tc.h"
extern int Cyc_Tc_aggressive_warn;
# 33
extern void Cyc_Tc_tc(struct Cyc_Tcenv_Tenv*te,int var_default_init,struct Cyc_List_List*ds);
# 37
extern struct Cyc_List_List*Cyc_Tc_treeshake(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*);struct Cyc_Hashtable_Table;
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
# 41 "cyclone.cyc"
extern void Cyc_Port_port(struct Cyc_List_List*);
# 44
extern int Cyc_ParseErrors_print_state_and_token;
# 47
extern int Cyc_Lex_compile_for_boot_r;
extern void Cyc_Lex_pos_init (void);
extern void Cyc_Lex_lex_init(int use_cyclone_keywords);
# 58
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
# 92
static struct Cyc_List_List*Cyc_ccargs=0;
static void Cyc_add_ccarg(struct _fat_ptr s){
Cyc_ccargs=({struct Cyc_List_List*_tmp1=_cycalloc(sizeof(*_tmp1));({struct _fat_ptr*_tmp2BC=({struct _fat_ptr*_tmp0=_cycalloc(sizeof(*_tmp0));*_tmp0=s;_tmp0;});_tmp1->hd=_tmp2BC;}),_tmp1->tl=Cyc_ccargs;_tmp1;});}
# 100
void Cyc_set_c_compiler(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2BD=(struct _fat_ptr)s;Cyc_strcmp(_tmp2BD,({const char*_tmp2="vc";_tag_fat(_tmp2,sizeof(char),3U);}));})== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Vc_c;
Cyc_add_ccarg(({const char*_tmp3="-DVC_C";_tag_fat(_tmp3,sizeof(char),7U);}));}else{
# 105
if(({struct _fat_ptr _tmp2BE=(struct _fat_ptr)s;Cyc_strcmp(_tmp2BE,({const char*_tmp4="gcc";_tag_fat(_tmp4,sizeof(char),4U);}));})== 0){
Cyc_Cyclone_c_compiler=Cyc_Cyclone_Gcc_c;
Cyc_add_ccarg(({const char*_tmp5="-DGCC_C";_tag_fat(_tmp5,sizeof(char),8U);}));}}}
# 112
void Cyc_set_port_c_code (void){
Cyc_Absyn_porting_c_code=1;
Cyc_Position_max_errors=65535;
Cyc_save_c_r=1;
Cyc_parseonly_r=1;}
# 119
static struct _fat_ptr*Cyc_output_file=0;
static void Cyc_set_output_file(struct _fat_ptr s){
Cyc_output_file=({struct _fat_ptr*_tmp6=_cycalloc(sizeof(*_tmp6));*_tmp6=s;_tmp6;});}
# 124
static struct _fat_ptr Cyc_specified_interface={(void*)0,(void*)0,(void*)(0 + 0)};
static void Cyc_set_specified_interface(struct _fat_ptr s){
Cyc_specified_interface=s;}
# 134
extern char*Cdef_lib_path;
extern char*Carch;
extern char*Cversion;static char _tmp7[1U]="";
# 139
static struct _fat_ptr Cyc_cpp={_tmp7,_tmp7,_tmp7 + 1U};
static void Cyc_set_cpp(struct _fat_ptr s){
Cyc_cpp=s;}
# 144
static struct Cyc_List_List*Cyc_cppargs=0;
static void Cyc_add_cpparg(struct _fat_ptr s){
Cyc_cppargs=({struct Cyc_List_List*_tmp9=_cycalloc(sizeof(*_tmp9));({struct _fat_ptr*_tmp2BF=({struct _fat_ptr*_tmp8=_cycalloc(sizeof(*_tmp8));*_tmp8=s;_tmp8;});_tmp9->hd=_tmp2BF;}),_tmp9->tl=Cyc_cppargs;_tmp9;});}
# 149
static void Cyc_add_cpp_and_ccarg(struct _fat_ptr s){
Cyc_add_cpparg(s);
Cyc_add_ccarg(s);}
# 154
static void Cyc_add_iprefix(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpC=({struct Cyc_String_pa_PrintArg_struct _tmp289;_tmp289.tag=0U,_tmp289.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp289;});void*_tmpA[1U];_tmpA[0]=& _tmpC;({struct _fat_ptr _tmp2C0=({const char*_tmpB="-iprefix %s";_tag_fat(_tmpB,sizeof(char),12U);});Cyc_aprintf(_tmp2C0,_tag_fat(_tmpA,sizeof(void*),1U));});}));}
# 157
static void Cyc_add_iwithprefix(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpF=({struct Cyc_String_pa_PrintArg_struct _tmp28A;_tmp28A.tag=0U,_tmp28A.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp28A;});void*_tmpD[1U];_tmpD[0]=& _tmpF;({struct _fat_ptr _tmp2C1=({const char*_tmpE="-iwithprefix %s";_tag_fat(_tmpE,sizeof(char),16U);});Cyc_aprintf(_tmp2C1,_tag_fat(_tmpD,sizeof(void*),1U));});}));}
# 160
static void Cyc_add_iwithprefixbefore(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp12=({struct Cyc_String_pa_PrintArg_struct _tmp28B;_tmp28B.tag=0U,_tmp28B.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp28B;});void*_tmp10[1U];_tmp10[0]=& _tmp12;({struct _fat_ptr _tmp2C2=({const char*_tmp11="-iwithprefixbefore %s";_tag_fat(_tmp11,sizeof(char),22U);});Cyc_aprintf(_tmp2C2,_tag_fat(_tmp10,sizeof(void*),1U));});}));}
# 163
static void Cyc_add_isystem(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp15=({struct Cyc_String_pa_PrintArg_struct _tmp28C;_tmp28C.tag=0U,_tmp28C.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp28C;});void*_tmp13[1U];_tmp13[0]=& _tmp15;({struct _fat_ptr _tmp2C3=({const char*_tmp14="-isystem %s";_tag_fat(_tmp14,sizeof(char),12U);});Cyc_aprintf(_tmp2C3,_tag_fat(_tmp13,sizeof(void*),1U));});}));}
# 166
static void Cyc_add_idirafter(struct _fat_ptr s){
Cyc_add_cpparg((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp28D;_tmp28D.tag=0U,_tmp28D.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp28D;});void*_tmp16[1U];_tmp16[0]=& _tmp18;({struct _fat_ptr _tmp2C4=({const char*_tmp17="-idirafter %s";_tag_fat(_tmp17,sizeof(char),14U);});Cyc_aprintf(_tmp2C4,_tag_fat(_tmp16,sizeof(void*),1U));});}));}
# 170
static void Cyc_set_many_errors (void){
Cyc_Position_max_errors=65535;}static char _tmp19[20U]="_see_cycspecs_file_";
# 174
static struct _fat_ptr Cyc_def_inc_path={_tmp19,_tmp19,_tmp19 + 20U};
# 177
static void Cyc_print_version (void){
({struct Cyc_String_pa_PrintArg_struct _tmp1C=({struct Cyc_String_pa_PrintArg_struct _tmp28E;_tmp28E.tag=0U,({struct _fat_ptr _tmp2C5=(struct _fat_ptr)({char*_tmp1D=Cversion;_tag_fat(_tmp1D,sizeof(char),_get_zero_arr_size_char((void*)_tmp1D,1U));});_tmp28E.f1=_tmp2C5;});_tmp28E;});void*_tmp1A[1U];_tmp1A[0]=& _tmp1C;({struct _fat_ptr _tmp2C6=({const char*_tmp1B="The Cyclone compiler, version %s\n";_tag_fat(_tmp1B,sizeof(char),34U);});Cyc_printf(_tmp2C6,_tag_fat(_tmp1A,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp20=({struct Cyc_String_pa_PrintArg_struct _tmp28F;_tmp28F.tag=0U,({struct _fat_ptr _tmp2C7=(struct _fat_ptr)({char*_tmp21=Carch;_tag_fat(_tmp21,sizeof(char),_get_zero_arr_size_char((void*)_tmp21,1U));});_tmp28F.f1=_tmp2C7;});_tmp28F;});void*_tmp1E[1U];_tmp1E[0]=& _tmp20;({struct _fat_ptr _tmp2C8=({const char*_tmp1F="Compiled for architecture: %s\n";_tag_fat(_tmp1F,sizeof(char),31U);});Cyc_printf(_tmp2C8,_tag_fat(_tmp1E,sizeof(void*),1U));});});
({struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp290;_tmp290.tag=0U,({struct _fat_ptr _tmp2C9=(struct _fat_ptr)({char*_tmp25=Cdef_lib_path;_tag_fat(_tmp25,sizeof(char),_get_zero_arr_size_char((void*)_tmp25,1U));});_tmp290.f1=_tmp2C9;});_tmp290;});void*_tmp22[1U];_tmp22[0]=& _tmp24;({struct _fat_ptr _tmp2CA=({const char*_tmp23="Standard library directory: %s\n";_tag_fat(_tmp23,sizeof(char),32U);});Cyc_printf(_tmp2CA,_tag_fat(_tmp22,sizeof(void*),1U));});});
# 182
({struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp291;_tmp291.tag=0U,_tmp291.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_def_inc_path);_tmp291;});void*_tmp26[1U];_tmp26[0]=& _tmp28;({struct _fat_ptr _tmp2CB=({const char*_tmp27="Standard include directory: %s\n";_tag_fat(_tmp27,sizeof(char),32U);});Cyc_printf(_tmp2CB,_tag_fat(_tmp26,sizeof(void*),1U));});});
exit(0);}
# 186
static int Cyc_is_cyclone_sourcefile(struct _fat_ptr s){
unsigned long _tmp29=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp29;
if(len <= (unsigned long)4)return 0;else{
return({struct _fat_ptr _tmp2CC=(struct _fat_ptr)_fat_ptr_plus(s,sizeof(char),(int)(len - (unsigned long)4));Cyc_strcmp(_tmp2CC,({const char*_tmp2A=".cyc";_tag_fat(_tmp2A,sizeof(char),5U);}));})== 0;}}
# 202 "cyclone.cyc"
static struct Cyc_List_List*Cyc_libargs=0;
static void Cyc_add_libarg(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2CD=(struct _fat_ptr)s;Cyc_strcmp(_tmp2CD,({const char*_tmp2B="-lxml";_tag_fat(_tmp2B,sizeof(char),6U);}));})== 0){
if(!Cyc_pa_r)
Cyc_add_ccarg(s);else{
# 208
Cyc_add_ccarg(({const char*_tmp2C="-lxml_a";_tag_fat(_tmp2C,sizeof(char),8U);}));}}else{
if(({struct _fat_ptr _tmp2CE=(struct _fat_ptr)s;Cyc_strncmp(_tmp2CE,({const char*_tmp2D="-lcyc";_tag_fat(_tmp2D,sizeof(char),6U);}),5U);})== 0)
Cyc_add_ccarg(s);else{
# 212
if(({struct _fat_ptr _tmp2CF=(struct _fat_ptr)s;Cyc_strncmp(_tmp2CF,({const char*_tmp2E="-lpthread";_tag_fat(_tmp2E,sizeof(char),10U);}),9U);})== 0)
Cyc_pthread_r=1;
Cyc_libargs=({struct Cyc_List_List*_tmp30=_cycalloc(sizeof(*_tmp30));({struct _fat_ptr*_tmp2D0=({struct _fat_ptr*_tmp2F=_cycalloc(sizeof(*_tmp2F));*_tmp2F=s;_tmp2F;});_tmp30->hd=_tmp2D0;}),_tmp30->tl=Cyc_libargs;_tmp30;});}}}
# 218
static void Cyc_add_marg(struct _fat_ptr s){
Cyc_add_ccarg(s);}
# 222
static void Cyc_set_save_temps (void){
Cyc_save_temps_r=1;
Cyc_add_ccarg(({const char*_tmp31="-save-temps";_tag_fat(_tmp31,sizeof(char),12U);}));}
# 227
static int Cyc_produce_dependencies=0;
static void Cyc_set_produce_dependencies (void){
Cyc_stop_after_cpp_r=1;
Cyc_produce_dependencies=1;
Cyc_add_cpparg(({const char*_tmp32="-M";_tag_fat(_tmp32,sizeof(char),3U);}));}
# 234
static struct _fat_ptr*Cyc_dependencies_target=0;
static void Cyc_set_dependencies_target(struct _fat_ptr s){
Cyc_dependencies_target=({struct _fat_ptr*_tmp33=_cycalloc(sizeof(*_tmp33));*_tmp33=s;_tmp33;});}
# 239
static void Cyc_set_stop_after_objectfile (void){
Cyc_stop_after_objectfile_r=1;
Cyc_add_ccarg(({const char*_tmp34="-c";_tag_fat(_tmp34,sizeof(char),3U);}));}
# 244
static void Cyc_set_nocppprecomp (void){
Cyc_add_cpp_and_ccarg(({const char*_tmp35="-no-cpp-precomp";_tag_fat(_tmp35,sizeof(char),16U);}));}
# 248
static void Cyc_clear_lineno (void){
Cyc_generate_line_directives_r=0;
Cyc_set_save_temps();}
# 252
static void Cyc_set_inline_functions (void){
Cyc_inline_functions_r=1;}
# 255
static void Cyc_set_elim_se (void){
Cyc_elim_se_r=1;
Cyc_set_inline_functions();}
# 259
static void Cyc_set_tovc (void){
Cyc_Cyclone_tovc_r=1;
Cyc_add_ccarg(({const char*_tmp36="-DCYC_ANSI_OUTPUT";_tag_fat(_tmp36,sizeof(char),18U);}));
Cyc_set_elim_se();}
# 264
static void Cyc_set_notoseqc (void){
Cyc_toseqc_r=0;}
# 267
static void Cyc_set_noboundschecks (void){
Cyc_add_ccarg(({const char*_tmp37="-DNO_CYC_BOUNDS_CHECKS";_tag_fat(_tmp37,sizeof(char),23U);}));}
# 270
static void Cyc_set_nonullchecks (void){
Cyc_add_ccarg(({const char*_tmp38="-DNO_CYC_NULL_CHECKS";_tag_fat(_tmp38,sizeof(char),21U);}));}
# 273
static void Cyc_set_nochecks (void){
Cyc_set_noboundschecks();
Cyc_set_nonullchecks();
Cyc_nocheck_r=1;}
# 279
static void Cyc_set_nocyc (void){
Cyc_add_cyc_namespace_r=0;
Cyc_add_ccarg(({const char*_tmp39="-DNO_CYC_PREFIX";_tag_fat(_tmp39,sizeof(char),16U);}));}
# 284
static void Cyc_set_pa (void){
Cyc_pa_r=1;
Cyc_add_ccarg(({const char*_tmp3A="-DCYC_REGION_PROFILE";_tag_fat(_tmp3A,sizeof(char),21U);}));
Cyc_add_cpparg(({const char*_tmp3B="-DCYC_REGION_PROFILE";_tag_fat(_tmp3B,sizeof(char),21U);}));}
# 290
static void Cyc_set_pg (void){
Cyc_pg_r=1;
Cyc_add_ccarg(({const char*_tmp3C="-pg";_tag_fat(_tmp3C,sizeof(char),4U);}));}
# 295
static void Cyc_set_stop_after_asmfile (void){
Cyc_stop_after_asmfile_r=1;
Cyc_add_ccarg(({const char*_tmp3D="-S";_tag_fat(_tmp3D,sizeof(char),3U);}));}
# 300
static void Cyc_set_all_warnings (void){
Cyc_InsertChecks_warn_bounds_checks=1;
Cyc_InsertChecks_warn_all_null_deref=1;
Cyc_NewControlFlow_warn_lose_unique=1;
Cyc_Tcutil_warn_alias_coerce=1;
Cyc_Tcutil_warn_region_coerce=1;
Cyc_Tc_aggressive_warn=1;
Cyc_Binding_warn_override=1;}
# 311
enum Cyc_inputtype{Cyc_DEFAULTINPUT =0U,Cyc_CYCLONEFILE =1U};
# 316
static enum Cyc_inputtype Cyc_intype=Cyc_DEFAULTINPUT;
static void Cyc_set_inputtype(struct _fat_ptr s){
if(({struct _fat_ptr _tmp2D1=(struct _fat_ptr)s;Cyc_strcmp(_tmp2D1,({const char*_tmp3E="cyc";_tag_fat(_tmp3E,sizeof(char),4U);}));})== 0)
Cyc_intype=Cyc_CYCLONEFILE;else{
if(({struct _fat_ptr _tmp2D2=(struct _fat_ptr)s;Cyc_strcmp(_tmp2D2,({const char*_tmp3F="none";_tag_fat(_tmp3F,sizeof(char),5U);}));})== 0)
Cyc_intype=Cyc_DEFAULTINPUT;else{
# 323
({struct Cyc_String_pa_PrintArg_struct _tmp42=({struct Cyc_String_pa_PrintArg_struct _tmp292;_tmp292.tag=0U,_tmp292.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp292;});void*_tmp40[1U];_tmp40[0]=& _tmp42;({struct Cyc___cycFILE*_tmp2D4=Cyc_stderr;struct _fat_ptr _tmp2D3=({const char*_tmp41="Input type '%s' not supported\n";_tag_fat(_tmp41,sizeof(char),31U);});Cyc_fprintf(_tmp2D4,_tmp2D3,_tag_fat(_tmp40,sizeof(void*),1U));});});}}}
# 326
struct _fat_ptr Cyc_make_base_filename(struct _fat_ptr s,struct _fat_ptr*output_file){
struct _fat_ptr _tmp43=Cyc_Filename_dirname(output_file == 0?s:*output_file);struct _fat_ptr outdir=_tmp43;
struct _fat_ptr _tmp44=Cyc_Filename_chop_extension((struct _fat_ptr)Cyc_Filename_basename(s));struct _fat_ptr base=_tmp44;
# 332
struct _fat_ptr _tmp45=Cyc_strlen((struct _fat_ptr)outdir)> (unsigned long)0?Cyc_Filename_concat((struct _fat_ptr)outdir,(struct _fat_ptr)base): base;struct _fat_ptr basename=_tmp45;
return(struct _fat_ptr)basename;}
# 338
static struct Cyc_List_List*Cyc_cyclone_files=0;
# 340
static int Cyc_gcc_optarg=0;
static void Cyc_add_other(struct _fat_ptr s){
# 343
if(Cyc_gcc_optarg > 0){
Cyc_add_ccarg(s);
-- Cyc_gcc_optarg;}else{
# 347
if((int)Cyc_intype == (int)Cyc_CYCLONEFILE || Cyc_is_cyclone_sourcefile(s)){
# 349
Cyc_cyclone_files=({struct Cyc_List_List*_tmp47=_cycalloc(sizeof(*_tmp47));({struct _fat_ptr*_tmp2D5=({struct _fat_ptr*_tmp46=_cycalloc(sizeof(*_tmp46));*_tmp46=s;_tmp46;});_tmp47->hd=_tmp2D5;}),_tmp47->tl=Cyc_cyclone_files;_tmp47;});{
# 355
struct _fat_ptr _tmp48=Cyc_make_base_filename(s,0);struct _fat_ptr basename=_tmp48;
struct _fat_ptr _tmp49=({struct _fat_ptr _tmp2D6=(struct _fat_ptr)basename;Cyc_strconcat(_tmp2D6,({const char*_tmp4A=".c";_tag_fat(_tmp4A,sizeof(char),3U);}));});struct _fat_ptr cfile=_tmp49;
Cyc_add_ccarg((struct _fat_ptr)cfile);}}else{
# 359
Cyc_add_ccarg(s);}}}
# 364
static int Cyc_assume_gcc_flag=1;struct _tuple26{struct _fat_ptr flag;int optargs;};static char _tmp4B[3U]="-s";static char _tmp4C[3U]="-O";static char _tmp4D[4U]="-O0";static char _tmp4E[4U]="-O2";static char _tmp4F[4U]="-O3";static char _tmp50[21U]="-fomit-frame-pointer";static char _tmp51[13U]="-fno-builtin";static char _tmp52[3U]="-g";static char _tmp53[3U]="-p";static char _tmp54[8U]="-static";
static int Cyc_add_other_flag(struct _fat_ptr s){
static struct _tuple26 known_gcc_flags[10U]={{.flag={_tmp4B,_tmp4B,_tmp4B + 3U},.optargs=0},{.flag={_tmp4C,_tmp4C,_tmp4C + 3U},.optargs=0},{.flag={_tmp4D,_tmp4D,_tmp4D + 4U},.optargs=0},{.flag={_tmp4E,_tmp4E,_tmp4E + 4U},.optargs=0},{.flag={_tmp4F,_tmp4F,_tmp4F + 4U},.optargs=0},{.flag={_tmp50,_tmp50,_tmp50 + 21U},.optargs=0},{.flag={_tmp51,_tmp51,_tmp51 + 13U},.optargs=0},{.flag={_tmp52,_tmp52,_tmp52 + 3U},.optargs=0},{.flag={_tmp53,_tmp53,_tmp53 + 3U},.optargs=0},{.flag={_tmp54,_tmp54,_tmp54 + 8U},.optargs=0}};
# 378
if(Cyc_assume_gcc_flag)
Cyc_add_ccarg(s);else{
# 381
{int i=0;for(0;(unsigned)i < 10U;++ i){
if(!Cyc_strcmp((struct _fat_ptr)(*((struct _tuple26*)_check_known_subscript_notnull(known_gcc_flags,10U,sizeof(struct _tuple26),i))).flag,(struct _fat_ptr)s)){
Cyc_add_ccarg(s);
Cyc_gcc_optarg=(known_gcc_flags[i]).optargs;
break;}}}
# 388
return 0;}
# 390
return 1;}
# 393
static void Cyc_noassume_gcc_flag (void){
Cyc_assume_gcc_flag=0;}
# 397
static void Cyc_remove_file(struct _fat_ptr s){
if(Cyc_save_temps_r)return;else{
remove((const char*)_check_null(_untag_fat_ptr(s,sizeof(char),1U)));}}
# 404
int Cyc_compile_failure=0;
# 406
struct Cyc___cycFILE*Cyc_try_file_open(struct _fat_ptr filename,struct _fat_ptr mode,struct _fat_ptr msg_part){
struct _handler_cons _tmp55;_push_handler(& _tmp55);{int _tmp57=0;if(setjmp(_tmp55.handler))_tmp57=1;if(!_tmp57){{struct Cyc___cycFILE*_tmp58=Cyc_file_open(filename,mode);_npop_handler(0U);return _tmp58;};_pop_handler();}else{void*_tmp56=(void*)Cyc_Core_get_exn_thrown();void*_tmp59=_tmp56;_LL1: _LL2:
# 410
 Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp5C=({struct Cyc_String_pa_PrintArg_struct _tmp294;_tmp294.tag=0U,_tmp294.f1=(struct _fat_ptr)((struct _fat_ptr)msg_part);_tmp294;});struct Cyc_String_pa_PrintArg_struct _tmp5D=({struct Cyc_String_pa_PrintArg_struct _tmp293;_tmp293.tag=0U,_tmp293.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_tmp293;});void*_tmp5A[2U];_tmp5A[0]=& _tmp5C,_tmp5A[1]=& _tmp5D;({struct Cyc___cycFILE*_tmp2D8=Cyc_stderr;struct _fat_ptr _tmp2D7=({const char*_tmp5B="\nError: couldn't open %s %s\n";_tag_fat(_tmp5B,sizeof(char),29U);});Cyc_fprintf(_tmp2D8,_tmp2D7,_tag_fat(_tmp5A,sizeof(void*),2U));});});
Cyc_fflush(Cyc_stderr);
return 0;_LL0:;}}}
# 417
extern void CYCALLOCPROFILE_mark(const char*s);
# 419
void*Cyc_do_stage(struct _fat_ptr stage_name,struct Cyc_List_List*tds,void*(*f)(void*,struct Cyc_List_List*),void*env,void(*on_fail)(void*),void*failenv){
# 422
CYCALLOCPROFILE_mark((const char*)_untag_fat_ptr(stage_name,sizeof(char),1U));{
# 424
void*ans;
{struct _handler_cons _tmp5E;_push_handler(& _tmp5E);{int _tmp60=0;if(setjmp(_tmp5E.handler))_tmp60=1;if(!_tmp60){ans=f(env,tds);;_pop_handler();}else{void*_tmp5F=(void*)Cyc_Core_get_exn_thrown();void*_tmp61=_tmp5F;void*_tmp62;_LL1: _tmp62=_tmp61;_LL2: {void*x=_tmp62;
# 429
if(Cyc_Position_error_p()){
Cyc_compile_failure=1;
on_fail(failenv);
({void*_tmp63=0U;({struct Cyc___cycFILE*_tmp2DA=Cyc_stderr;struct _fat_ptr _tmp2D9=({const char*_tmp64="\nCOMPILATION FAILED!\n";_tag_fat(_tmp64,sizeof(char),22U);});Cyc_fprintf(_tmp2DA,_tmp2D9,_tag_fat(_tmp63,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);}else{
# 436
({struct Cyc_String_pa_PrintArg_struct _tmp67=({struct Cyc_String_pa_PrintArg_struct _tmp295;_tmp295.tag=0U,_tmp295.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_tmp295;});void*_tmp65[1U];_tmp65[0]=& _tmp67;({struct Cyc___cycFILE*_tmp2DC=Cyc_stderr;struct _fat_ptr _tmp2DB=({const char*_tmp66="COMPILER STAGE %s\n";_tag_fat(_tmp66,sizeof(char),19U);});Cyc_fprintf(_tmp2DC,_tmp2DB,_tag_fat(_tmp65,sizeof(void*),1U));});});
on_fail(failenv);}
# 439
Cyc_Core_rethrow(x);
goto _LL0;}_LL0:;}}}
# 442
if(Cyc_Position_error_p())
Cyc_compile_failure=1;
if(Cyc_compile_failure){
on_fail(failenv);
({void*_tmp68=0U;({struct Cyc___cycFILE*_tmp2DE=Cyc_stderr;struct _fat_ptr _tmp2DD=({const char*_tmp69="\nCOMPILATION FAILED!\n";_tag_fat(_tmp69,sizeof(char),22U);});Cyc_fprintf(_tmp2DE,_tmp2DD,_tag_fat(_tmp68,sizeof(void*),0U));});});
Cyc_fflush(Cyc_stderr);
return ans;}else{
# 450
if(Cyc_v_r){
({struct Cyc_String_pa_PrintArg_struct _tmp6C=({struct Cyc_String_pa_PrintArg_struct _tmp296;_tmp296.tag=0U,_tmp296.f1=(struct _fat_ptr)((struct _fat_ptr)stage_name);_tmp296;});void*_tmp6A[1U];_tmp6A[0]=& _tmp6C;({struct Cyc___cycFILE*_tmp2E0=Cyc_stderr;struct _fat_ptr _tmp2DF=({const char*_tmp6B="%s completed.\n";_tag_fat(_tmp6B,sizeof(char),15U);});Cyc_fprintf(_tmp2E0,_tmp2DF,_tag_fat(_tmp6A,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return ans;}}
# 455
return ans;}}
# 458
static void Cyc_ignore(void*x){;}
static void Cyc_remove_fileptr(struct _fat_ptr*s){Cyc_remove_file((struct _fat_ptr)*s);}
# 461
struct Cyc_List_List*Cyc_do_parse(struct Cyc___cycFILE*f,struct Cyc_List_List*ignore){
Cyc_Lex_lex_init(1);
Cyc_Lex_pos_init();{
struct Cyc_List_List*ans=0;
{struct _handler_cons _tmp6D;_push_handler(& _tmp6D);{int _tmp6F=0;if(setjmp(_tmp6D.handler))_tmp6F=1;if(!_tmp6F){
ans=Cyc_Parse_parse_file(f);
Cyc_file_close(f);
# 466
;_pop_handler();}else{void*_tmp6E=(void*)Cyc_Core_get_exn_thrown();void*_tmp70=_tmp6E;void*_tmp71;if(((struct Cyc_Parse_Exit_exn_struct*)_tmp70)->tag == Cyc_Parse_Exit){_LL1: _LL2:
# 469
 Cyc_file_close(f);Cyc_compile_failure=1;goto _LL0;}else{_LL3: _tmp71=_tmp70;_LL4: {void*e=_tmp71;
Cyc_file_close(f);Cyc_Core_rethrow(e);}}_LL0:;}}}
# 472
Cyc_Lex_lex_init(1);
return ans;}}
# 476
int Cyc_do_binding(int ignore,struct Cyc_List_List*tds){
Cyc_Binding_resolve_all(tds);
return 1;}
# 481
struct Cyc_List_List*Cyc_do_typecheck(struct Cyc_Tcenv_Tenv*te,struct Cyc_List_List*tds){
# 483
Cyc_Tc_tc(te,1,tds);
if(!Cyc_noshake_r)
tds=Cyc_Tc_treeshake(te,tds);
return tds;}
# 489
struct Cyc_JumpAnalysis_Jump_Anal_Result*Cyc_do_jumpanalysis(int ignore,struct Cyc_List_List*tds){
# 491
return Cyc_JumpAnalysis_jump_analysis(tds);}
# 494
struct Cyc_List_List*Cyc_do_cfcheck(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 496
Cyc_NewControlFlow_cf_check(tables,tds);
return tds;}
# 500
int Cyc_do_vcgen(struct Cyc_JumpAnalysis_Jump_Anal_Result*tables,struct Cyc_List_List*tds){
# 502
Cyc_Vcgen_vcgen_topdecls(tables,tds);
return 1;}
# 506
int Cyc_do_insert_checks(int ignore,struct Cyc_List_List*tds){
Cyc_InsertChecks_insert_checks(tds);
return 1;}struct _tuple27{struct Cyc_Tcenv_Tenv*f1;struct Cyc___cycFILE*f2;struct Cyc___cycFILE*f3;};
# 513
struct Cyc_List_List*Cyc_do_interface(struct _tuple27*params,struct Cyc_List_List*tds){
# 515
struct _tuple27*_tmp72=params;struct Cyc___cycFILE*_tmp75;struct Cyc___cycFILE*_tmp74;struct Cyc_Tcenv_Tenv*_tmp73;_LL1: _tmp73=_tmp72->f1;_tmp74=_tmp72->f2;_tmp75=_tmp72->f3;_LL2: {struct Cyc_Tcenv_Tenv*te=_tmp73;struct Cyc___cycFILE*fi=_tmp74;struct Cyc___cycFILE*fo=_tmp75;
struct Cyc_Interface_I*_tmp76=Cyc_Interface_extract(te->ae,tds);struct Cyc_Interface_I*i1=_tmp76;
if(fi == 0)
Cyc_Interface_save(i1,fo);else{
# 520
struct Cyc_Interface_I*_tmp77=Cyc_Interface_parse(fi);struct Cyc_Interface_I*i0=_tmp77;
if(!({struct Cyc_Interface_I*_tmp2E4=i0;struct Cyc_Interface_I*_tmp2E3=i1;Cyc_Interface_is_subinterface(_tmp2E4,_tmp2E3,({struct _tuple25*_tmp7A=_cycalloc(sizeof(*_tmp7A));({struct _fat_ptr _tmp2E2=({const char*_tmp78="written interface";_tag_fat(_tmp78,sizeof(char),18U);});_tmp7A->f1=_tmp2E2;}),({struct _fat_ptr _tmp2E1=({const char*_tmp79="maximal interface";_tag_fat(_tmp79,sizeof(char),18U);});_tmp7A->f2=_tmp2E1;});_tmp7A;}));}))
Cyc_compile_failure=1;else{
# 524
Cyc_Interface_save(i0,fo);}}
# 526
return tds;}}
# 528
void Cyc_interface_fail(struct _tuple27*params){
struct _tuple27*_tmp7B=params;struct Cyc___cycFILE*_tmp7D;struct Cyc___cycFILE*_tmp7C;_LL1: _tmp7C=_tmp7B->f2;_tmp7D=_tmp7B->f3;_LL2: {struct Cyc___cycFILE*fi=_tmp7C;struct Cyc___cycFILE*fo=_tmp7D;
if(fi != 0)
Cyc_file_close(fi);
Cyc_file_close(fo);}}
# 535
struct Cyc_List_List*Cyc_do_translate(struct Cyc_Hashtable_Table*pops,struct Cyc_List_List*tds){
# 538
return Cyc_Toc_toc(pops,tds);}
# 540
struct Cyc_List_List*Cyc_do_removeaggrs(int ignore,struct Cyc_List_List*tds){
return Cyc_RemoveAggrs_remove_aggrs(tds);}
# 544
struct Cyc_List_List*Cyc_do_toseqc(int ignore,struct Cyc_List_List*tds){
return Cyc_Toseqc_toseqc(tds);}
# 547
struct Cyc_List_List*Cyc_do_tovc(int ignore,struct Cyc_List_List*tds){
Cyc_Tovc_elim_array_initializers=Cyc_Cyclone_tovc_r;
return Cyc_Tovc_tovc(tds);}
# 552
static struct _fat_ptr Cyc_cyc_setjmp={(void*)0,(void*)0,(void*)(0 + 0)};
# 554
static struct _fat_ptr Cyc_cyc_include={(void*)0,(void*)0,(void*)(0 + 0)};
# 556
static void Cyc_set_cyc_include(struct _fat_ptr f){
Cyc_cyc_include=f;}
# 561
int Cyc_copy_internal_file(struct _fat_ptr file,struct Cyc___cycFILE*out_file){
# 564
if(({char*_tmp2E5=(char*)file.curr;_tmp2E5 == (char*)(_tag_fat(0,0,0)).curr;})){
({void*_tmp7E=0U;({struct Cyc___cycFILE*_tmp2E7=Cyc_stderr;struct _fat_ptr _tmp2E6=({const char*_tmp7F="Internal error: copy_internal_file called with NULL\n";_tag_fat(_tmp7F,sizeof(char),53U);});Cyc_fprintf(_tmp2E7,_tmp2E6,_tag_fat(_tmp7E,sizeof(void*),0U));});});
return 1;}{
# 568
struct Cyc___cycFILE*_tmp80=({struct _fat_ptr _tmp2E9=file;struct _fat_ptr _tmp2E8=({const char*_tmp87="r";_tag_fat(_tmp87,sizeof(char),2U);});Cyc_try_file_open(_tmp2E9,_tmp2E8,({const char*_tmp88="internal compiler file";_tag_fat(_tmp88,sizeof(char),23U);}));});struct Cyc___cycFILE*file_f=_tmp80;
if(file_f == 0)return 1;{
unsigned long n_read=1024U;
unsigned long n_written;
char buf[1024U];({{unsigned _tmp299=1024U;unsigned i;for(i=0;i < _tmp299;++ i){buf[i]='\000';}}0;});
while(n_read == (unsigned long)1024){
n_read=({struct _fat_ptr _tmp2EA=_tag_fat(buf,sizeof(char),1024U);Cyc_fread(_tmp2EA,1U,1024U,file_f);});
if(n_read != (unsigned long)1024 && !Cyc_feof(file_f)){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp83=({struct Cyc_String_pa_PrintArg_struct _tmp297;_tmp297.tag=0U,_tmp297.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp297;});void*_tmp81[1U];_tmp81[0]=& _tmp83;({struct Cyc___cycFILE*_tmp2EC=Cyc_stderr;struct _fat_ptr _tmp2EB=({const char*_tmp82="\nError: problem copying %s\n";_tag_fat(_tmp82,sizeof(char),28U);});Cyc_fprintf(_tmp2EC,_tmp2EB,_tag_fat(_tmp81,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return 1;}
# 581
n_written=({struct _fat_ptr _tmp2EE=_tag_fat(buf,sizeof(char),1024U);unsigned long _tmp2ED=n_read;Cyc_fwrite(_tmp2EE,1U,_tmp2ED,out_file);});
if(n_read != n_written){
Cyc_compile_failure=1;
({struct Cyc_String_pa_PrintArg_struct _tmp86=({struct Cyc_String_pa_PrintArg_struct _tmp298;_tmp298.tag=0U,_tmp298.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp298;});void*_tmp84[1U];_tmp84[0]=& _tmp86;({struct Cyc___cycFILE*_tmp2F0=Cyc_stderr;struct _fat_ptr _tmp2EF=({const char*_tmp85="\nError: problem copying %s\n";_tag_fat(_tmp85,sizeof(char),28U);});Cyc_fprintf(_tmp2F0,_tmp2EF,_tag_fat(_tmp84,sizeof(void*),1U));});});
Cyc_fflush(Cyc_stderr);
return 1;}}
# 589
Cyc_fclose(file_f);
return 0;}}}
# 595
static struct Cyc_List_List*Cyc_cfiles=0;
static void Cyc_remove_cfiles (void){
if(!Cyc_save_c_r)
for(0;Cyc_cfiles != 0;Cyc_cfiles=((struct Cyc_List_List*)_check_null(Cyc_cfiles))->tl){
Cyc_remove_file((struct _fat_ptr)*((struct _fat_ptr*)((struct Cyc_List_List*)_check_null(Cyc_cfiles))->hd));}}
# 601
static void Cyc_find_fail(struct _fat_ptr file){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmp8C=_cycalloc(sizeof(*_tmp8C));_tmp8C->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp2F2=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp8B=({struct Cyc_String_pa_PrintArg_struct _tmp29A;_tmp29A.tag=0U,_tmp29A.f1=(struct _fat_ptr)((struct _fat_ptr)file);_tmp29A;});void*_tmp89[1U];_tmp89[0]=& _tmp8B;({struct _fat_ptr _tmp2F1=({const char*_tmp8A="Error: can't find internal compiler file %s\n";_tag_fat(_tmp8A,sizeof(char),45U);});Cyc_aprintf(_tmp2F1,_tag_fat(_tmp89,sizeof(void*),1U));});});_tmp8C->f1=_tmp2F2;});_tmp8C;}));}
# 606
static struct _fat_ptr Cyc_find_in_arch_path(struct _fat_ptr s){
struct _fat_ptr _tmp8D=Cyc_Specsfile_find_in_arch_path(s);struct _fat_ptr r=_tmp8D;
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}
# 611
static struct _fat_ptr Cyc_find_in_exec_path(struct _fat_ptr s){
struct _fat_ptr _tmp8E=Cyc_Specsfile_find_in_exec_path(s);struct _fat_ptr r=_tmp8E;
if(!((unsigned)r.curr))Cyc_find_fail(s);
return r;}struct _tuple28{struct Cyc___cycFILE*f1;struct _fat_ptr f2;};
# 617
struct Cyc_List_List*Cyc_do_print(struct _tuple28*env,struct Cyc_List_List*tds){
# 619
struct _tuple28*_tmp8F=env;struct _fat_ptr _tmp91;struct Cyc___cycFILE*_tmp90;_LL1: _tmp90=_tmp8F->f1;_tmp91=_tmp8F->f2;_LL2: {struct Cyc___cycFILE*out_file=_tmp90;struct _fat_ptr cfile=_tmp91;
struct Cyc_Absynpp_Params _tmp92=Cyc_tc_r?Cyc_Absynpp_cyc_params_r: Cyc_Absynpp_c_params_r;struct Cyc_Absynpp_Params params_r=_tmp92;
params_r.expand_typedefs=!Cyc_noexpand_r;
params_r.to_VC=Cyc_Cyclone_tovc_r;
params_r.add_cyc_prefix=Cyc_add_cyc_namespace_r;
params_r.generate_line_directives=Cyc_generate_line_directives_r;
params_r.print_full_evars=Cyc_print_full_evars_r;
params_r.print_all_tvars=Cyc_print_all_tvars_r;
params_r.print_all_kinds=Cyc_print_all_kinds_r;
params_r.print_all_effects=Cyc_print_all_effects_r;
# 630
if(Cyc_inline_functions_r)
({void*_tmp93=0U;({struct Cyc___cycFILE*_tmp2F4=out_file;struct _fat_ptr _tmp2F3=({const char*_tmp94="#define _INLINE_FUNCTIONS\n";_tag_fat(_tmp94,sizeof(char),27U);});Cyc_fprintf(_tmp2F4,_tmp2F3,_tag_fat(_tmp93,sizeof(void*),0U));});});
# 634
if((!Cyc_parseonly_r && !Cyc_tc_r)&& !Cyc_cf_r){
if(!Cyc_nocyc_setjmp_r){
if(Cyc_Lex_compile_for_boot_r)
({void*_tmp95=0U;({struct Cyc___cycFILE*_tmp2F6=out_file;struct _fat_ptr _tmp2F5=({const char*_tmp96="#include <setjmp.h>\n";_tag_fat(_tmp96,sizeof(char),21U);});Cyc_fprintf(_tmp2F6,_tmp2F5,_tag_fat(_tmp95,sizeof(void*),0U));});});else{
if(Cyc_copy_internal_file(Cyc_cyc_setjmp,out_file))return tds;}}
# 640
if(Cyc_copy_internal_file(Cyc_cyc_include,out_file))return tds;}
# 642
if(Cyc_pp_r){
Cyc_Absynpp_set_params(& params_r);
Cyc_Absynpp_decllist2file(tds,out_file);}else{
# 646
Cyc_Absyndump_set_params(& params_r);
Cyc_Absyndump_dumpdecllist2file(tds,out_file);}
# 649
Cyc_fflush(out_file);
Cyc_file_close(out_file);
Cyc_cfiles=({struct Cyc_List_List*_tmp98=_cycalloc(sizeof(*_tmp98));({struct _fat_ptr*_tmp2F7=({struct _fat_ptr*_tmp97=_cycalloc(sizeof(*_tmp97));*_tmp97=cfile;_tmp97;});_tmp98->hd=_tmp2F7;}),_tmp98->tl=Cyc_cfiles;_tmp98;});
return tds;}}
# 654
void Cyc_print_fail(struct _tuple28*env){
struct _tuple28*_tmp99=env;struct _fat_ptr _tmp9B;struct Cyc___cycFILE*_tmp9A;_LL1: _tmp9A=_tmp99->f1;_tmp9B=_tmp99->f2;_LL2: {struct Cyc___cycFILE*out_file=_tmp9A;struct _fat_ptr cfile=_tmp9B;
Cyc_file_close(out_file);
Cyc_cfiles=({struct Cyc_List_List*_tmp9D=_cycalloc(sizeof(*_tmp9D));({struct _fat_ptr*_tmp2F9=({struct _fat_ptr*_tmp9C=_cycalloc(sizeof(*_tmp9C));({struct _fat_ptr _tmp2F8=Cyc_strdup((struct _fat_ptr)cfile);*_tmp9C=_tmp2F8;});_tmp9C;});_tmp9D->hd=_tmp2F9;}),_tmp9D->tl=Cyc_cfiles;_tmp9D;});}}
# 661
static struct Cyc_List_List*Cyc_split_by_char(struct _fat_ptr s,char c){
if(({char*_tmp2FA=(char*)s.curr;_tmp2FA == (char*)(_tag_fat(0,0,0)).curr;}))return 0;{
struct Cyc_List_List*_tmp9E=0;struct Cyc_List_List*result=_tmp9E;
unsigned long _tmp9F=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp9F;
# 666
while(len > (unsigned long)0){
struct _fat_ptr _tmpA0=Cyc_strchr(s,c);struct _fat_ptr end=_tmpA0;
if(({char*_tmp2FB=(char*)end.curr;_tmp2FB == (char*)(_tag_fat(0,0,0)).curr;})){
result=({struct Cyc_List_List*_tmpA2=_cycalloc(sizeof(*_tmpA2));({struct _fat_ptr*_tmp2FC=({struct _fat_ptr*_tmpA1=_cycalloc(sizeof(*_tmpA1));*_tmpA1=s;_tmpA1;});_tmpA2->hd=_tmp2FC;}),_tmpA2->tl=result;_tmpA2;});
break;}else{
# 673
result=({struct Cyc_List_List*_tmpA4=_cycalloc(sizeof(*_tmpA4));({struct _fat_ptr*_tmp2FE=({struct _fat_ptr*_tmpA3=_cycalloc(sizeof(*_tmpA3));({struct _fat_ptr _tmp2FD=(struct _fat_ptr)Cyc_substring((struct _fat_ptr)s,0,(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char)));*_tmpA3=_tmp2FD;});_tmpA3;});_tmpA4->hd=_tmp2FE;}),_tmpA4->tl=result;_tmpA4;});
len -=(unsigned long)((((struct _fat_ptr)end).curr - s.curr)/ sizeof(char));
s=_fat_ptr_plus(end,sizeof(char),1);}}
# 678
return((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(result);}}
# 683
static struct Cyc_List_List*Cyc_add_subdir(struct Cyc_List_List*dirs,struct _fat_ptr subdir){
# 685
struct Cyc_List_List*_tmpA5=0;struct Cyc_List_List*l=_tmpA5;
for(0;dirs != 0;dirs=dirs->tl){
l=({struct Cyc_List_List*_tmpA7=_cycalloc(sizeof(*_tmpA7));({struct _fat_ptr*_tmp300=({struct _fat_ptr*_tmpA6=_cycalloc(sizeof(*_tmpA6));({struct _fat_ptr _tmp2FF=(struct _fat_ptr)Cyc_Filename_concat(*((struct _fat_ptr*)dirs->hd),subdir);*_tmpA6=_tmp2FF;});_tmpA6;});_tmpA7->hd=_tmp300;}),_tmpA7->tl=l;_tmpA7;});}
# 689
l=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_imp_rev)(l);
return l;}
# 694
static int Cyc_is_other_special(char c){
char _tmpA8=c;switch(_tmpA8){case 92U: _LL1: _LL2:
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
# 709
 goto _LL18;case 9U: _LL17: _LL18:
 return 1;default: _LL19: _LL1A:
 return 0;}_LL0:;}
# 716
static struct _fat_ptr Cyc_sh_escape_string(struct _fat_ptr s){
unsigned long _tmpA9=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmpA9;
# 720
int _tmpAA=0;int single_quotes=_tmpAA;
int _tmpAB=0;int other_special=_tmpAB;
{int i=0;for(0;(unsigned long)i < len;++ i){
char _tmpAC=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmpAC;
if((int)c == (int)'\'')++ single_quotes;else{
if(Cyc_is_other_special(c))++ other_special;}}}
# 729
if(single_quotes == 0 && other_special == 0)
return s;
# 733
if(single_quotes == 0)
return(struct _fat_ptr)Cyc_strconcat_l(({struct _fat_ptr*_tmpAD[3U];({struct _fat_ptr*_tmp305=({struct _fat_ptr*_tmpAF=_cycalloc(sizeof(*_tmpAF));({struct _fat_ptr _tmp304=({const char*_tmpAE="'";_tag_fat(_tmpAE,sizeof(char),2U);});*_tmpAF=_tmp304;});_tmpAF;});_tmpAD[0]=_tmp305;}),({struct _fat_ptr*_tmp303=({struct _fat_ptr*_tmpB0=_cycalloc(sizeof(*_tmpB0));*_tmpB0=(struct _fat_ptr)s;_tmpB0;});_tmpAD[1]=_tmp303;}),({struct _fat_ptr*_tmp302=({struct _fat_ptr*_tmpB2=_cycalloc(sizeof(*_tmpB2));({struct _fat_ptr _tmp301=({const char*_tmpB1="'";_tag_fat(_tmpB1,sizeof(char),2U);});*_tmpB2=_tmp301;});_tmpB2;});_tmpAD[2]=_tmp302;});((struct Cyc_List_List*(*)(struct _fat_ptr))Cyc_List_list)(_tag_fat(_tmpAD,sizeof(struct _fat_ptr*),3U));}));{
# 737
unsigned long _tmpB3=(len + (unsigned long)single_quotes)+ (unsigned long)other_special;unsigned long len2=_tmpB3;
struct _fat_ptr s2=({unsigned _tmpBE=(len2 + (unsigned long)1)+ 1U;char*_tmpBD=_cycalloc_atomic(_check_times(_tmpBE,sizeof(char)));({{unsigned _tmp29B=len2 + (unsigned long)1;unsigned i;for(i=0;i < _tmp29B;++ i){_tmpBD[i]='\000';}_tmpBD[_tmp29B]=0;}0;});_tag_fat(_tmpBD,sizeof(char),_tmpBE);});
int _tmpB4=0;int i=_tmpB4;
int _tmpB5=0;int j=_tmpB5;
for(0;(unsigned long)i < len;++ i){
char _tmpB6=*((const char*)_check_fat_subscript(s,sizeof(char),i));char c=_tmpB6;
if((int)c == (int)'\'' || Cyc_is_other_special(c))
({struct _fat_ptr _tmpB7=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmpB8=*((char*)_check_fat_subscript(_tmpB7,sizeof(char),0U));char _tmpB9='\\';if(_get_fat_size(_tmpB7,sizeof(char))== 1U &&(_tmpB8 == 0 && _tmpB9 != 0))_throw_arraybounds();*((char*)_tmpB7.curr)=_tmpB9;});
({struct _fat_ptr _tmpBA=_fat_ptr_plus(s2,sizeof(char),j ++);char _tmpBB=*((char*)_check_fat_subscript(_tmpBA,sizeof(char),0U));char _tmpBC=c;if(_get_fat_size(_tmpBA,sizeof(char))== 1U &&(_tmpBB == 0 && _tmpBC != 0))_throw_arraybounds();*((char*)_tmpBA.curr)=_tmpBC;});}
# 747
return(struct _fat_ptr)s2;}}
# 749
static struct _fat_ptr*Cyc_sh_escape_stringptr(struct _fat_ptr*sp){
return({struct _fat_ptr*_tmpBF=_cycalloc(sizeof(*_tmpBF));({struct _fat_ptr _tmp306=Cyc_sh_escape_string(*sp);*_tmpBF=_tmp306;});_tmpBF;});}
# 753
static void Cyc_process_file(struct _fat_ptr filename){
# 755
struct _fat_ptr _tmpC0=Cyc_make_base_filename(filename,Cyc_output_file);struct _fat_ptr basename=_tmpC0;
struct _fat_ptr _tmpC1=({struct _fat_ptr _tmp307=(struct _fat_ptr)basename;Cyc_strconcat(_tmp307,({const char*_tmp116=".cyp";_tag_fat(_tmp116,sizeof(char),5U);}));});struct _fat_ptr preprocfile=_tmpC1;
struct _fat_ptr _tmpC2=({char*_tmp309=(char*)Cyc_specified_interface.curr;_tmp309 != (char*)(_tag_fat(0,0,0)).curr;})?Cyc_specified_interface:(struct _fat_ptr)({
struct _fat_ptr _tmp308=(struct _fat_ptr)basename;Cyc_strconcat(_tmp308,({const char*_tmp115=".cyci";_tag_fat(_tmp115,sizeof(char),6U);}));});
# 757
struct _fat_ptr interfacefile=_tmpC2;
# 759
struct _fat_ptr _tmpC3=({struct _fat_ptr _tmp30A=(struct _fat_ptr)basename;Cyc_strconcat(_tmp30A,({const char*_tmp114=".cycio";_tag_fat(_tmp114,sizeof(char),7U);}));});struct _fat_ptr interfaceobjfile=_tmpC3;
# 763
struct _fat_ptr _tmpC4=({struct _fat_ptr _tmp30B=(struct _fat_ptr)Cyc_make_base_filename(filename,0);Cyc_strconcat(_tmp30B,({const char*_tmp113=".c";_tag_fat(_tmp113,sizeof(char),3U);}));});struct _fat_ptr cfile=_tmpC4;
struct _fat_ptr*_tmpC5=({struct _fat_ptr*_tmp112=_cycalloc(sizeof(*_tmp112));*_tmp112=preprocfile;_tmp112;});struct _fat_ptr*preprocfileptr=_tmpC5;
# 766
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpC8=({struct Cyc_String_pa_PrintArg_struct _tmp29C;_tmp29C.tag=0U,_tmp29C.f1=(struct _fat_ptr)((struct _fat_ptr)filename);_tmp29C;});void*_tmpC6[1U];_tmpC6[0]=& _tmpC8;({struct Cyc___cycFILE*_tmp30D=Cyc_stderr;struct _fat_ptr _tmp30C=({const char*_tmpC7="Compiling %s\n";_tag_fat(_tmpC7,sizeof(char),14U);});Cyc_fprintf(_tmp30D,_tmp30C,_tag_fat(_tmpC6,sizeof(void*),1U));});});{
# 769
struct Cyc___cycFILE*f0=({struct _fat_ptr _tmp30F=filename;struct _fat_ptr _tmp30E=({const char*_tmp110="r";_tag_fat(_tmp110,sizeof(char),2U);});Cyc_try_file_open(_tmp30F,_tmp30E,({const char*_tmp111="input file";_tag_fat(_tmp111,sizeof(char),11U);}));});
if(Cyc_compile_failure || !((unsigned)f0))
return;
Cyc_fclose(f0);{
# 776
struct _fat_ptr _tmpC9=({
struct Cyc_List_List*_tmp313=({struct Cyc_List_List*_tmp10E=_cycalloc(sizeof(*_tmp10E));({struct _fat_ptr*_tmp312=({struct _fat_ptr*_tmp10D=_cycalloc(sizeof(*_tmp10D));({struct _fat_ptr _tmp311=(struct _fat_ptr)({const char*_tmp10C="";_tag_fat(_tmp10C,sizeof(char),1U);});*_tmp10D=_tmp311;});_tmp10D;});_tmp10E->hd=_tmp312;}),({
struct Cyc_List_List*_tmp310=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cppargs));_tmp10E->tl=_tmp310;});_tmp10E;});
# 777
Cyc_str_sepstr(_tmp313,({const char*_tmp10F=" ";_tag_fat(_tmp10F,sizeof(char),2U);}));});
# 776
struct _fat_ptr cppargs_string=_tmpC9;
# 786
struct Cyc_List_List*_tmpCA=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);struct Cyc_List_List*stdinc_dirs=_tmpCA;
stdinc_dirs=({struct Cyc_List_List*_tmp314=stdinc_dirs;Cyc_add_subdir(_tmp314,({const char*_tmpCB="include";_tag_fat(_tmpCB,sizeof(char),8U);}));});
if(Cyc_strlen((struct _fat_ptr)Cyc_def_inc_path)> (unsigned long)0)
stdinc_dirs=({struct Cyc_List_List*_tmpCD=_cycalloc(sizeof(*_tmpCD));({struct _fat_ptr*_tmp315=({struct _fat_ptr*_tmpCC=_cycalloc(sizeof(*_tmpCC));*_tmpCC=Cyc_def_inc_path;_tmpCC;});_tmpCD->hd=_tmp315;}),_tmpCD->tl=stdinc_dirs;_tmpCD;});{
# 791
char*_tmpCE=getenv("CYCLONE_INCLUDE_PATH");char*cyclone_include_path=_tmpCE;
if(cyclone_include_path != 0)
stdinc_dirs=({struct Cyc_List_List*_tmp316=Cyc_split_by_char(({char*_tmpCF=cyclone_include_path;_tag_fat(_tmpCF,sizeof(char),_get_zero_arr_size_char((void*)_tmpCF,1U));}),':');((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp316,stdinc_dirs);});{
# 796
struct _fat_ptr stdinc_string=(struct _fat_ptr)({
struct Cyc_List_List*_tmp31A=({struct Cyc_List_List*_tmp10A=_cycalloc(sizeof(*_tmp10A));({struct _fat_ptr*_tmp319=({struct _fat_ptr*_tmp109=_cycalloc(sizeof(*_tmp109));({struct _fat_ptr _tmp318=(struct _fat_ptr)({const char*_tmp108="";_tag_fat(_tmp108,sizeof(char),1U);});*_tmp109=_tmp318;});_tmp109;});_tmp10A->hd=_tmp319;}),({
struct Cyc_List_List*_tmp317=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,stdinc_dirs);_tmp10A->tl=_tmp317;});_tmp10A;});
# 797
Cyc_str_sepstr(_tmp31A,({const char*_tmp10B=" -I";_tag_fat(_tmp10B,sizeof(char),4U);}));});
# 800
struct _fat_ptr ofile_string;
if(Cyc_stop_after_cpp_r){
if(Cyc_output_file != 0)
ofile_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD2=({struct Cyc_String_pa_PrintArg_struct _tmp29D;_tmp29D.tag=0U,_tmp29D.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_output_file)));_tmp29D;});void*_tmpD0[1U];_tmpD0[0]=& _tmpD2;({struct _fat_ptr _tmp31B=({const char*_tmpD1=" > %s";_tag_fat(_tmpD1,sizeof(char),6U);});Cyc_aprintf(_tmp31B,_tag_fat(_tmpD0,sizeof(void*),1U));});});else{
# 805
ofile_string=({const char*_tmpD3="";_tag_fat(_tmpD3,sizeof(char),1U);});}}else{
# 807
ofile_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpD6=({struct Cyc_String_pa_PrintArg_struct _tmp29E;_tmp29E.tag=0U,({struct _fat_ptr _tmp31C=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string((struct _fat_ptr)preprocfile));_tmp29E.f1=_tmp31C;});_tmp29E;});void*_tmpD4[1U];_tmpD4[0]=& _tmpD6;({struct _fat_ptr _tmp31D=({const char*_tmpD5=" > %s";_tag_fat(_tmpD5,sizeof(char),6U);});Cyc_aprintf(_tmp31D,_tag_fat(_tmpD4,sizeof(void*),1U));});});}{
# 809
struct _fat_ptr fixup_string;
if(Cyc_produce_dependencies){
# 813
if(Cyc_dependencies_target == 0)
# 817
fixup_string=({const char*_tmpD7=" | sed 's/^\\(.*\\)\\.cyc\\.o:/\\1.o:/'";_tag_fat(_tmpD7,sizeof(char),35U);});else{
# 822
fixup_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmpDA=({struct Cyc_String_pa_PrintArg_struct _tmp29F;_tmp29F.tag=0U,_tmp29F.f1=(struct _fat_ptr)((struct _fat_ptr)*((struct _fat_ptr*)_check_null(Cyc_dependencies_target)));_tmp29F;});void*_tmpD8[1U];_tmpD8[0]=& _tmpDA;({struct _fat_ptr _tmp31E=({const char*_tmpD9=" | sed 's/^.*\\.cyc\\.o:/%s:/'";_tag_fat(_tmpD9,sizeof(char),29U);});Cyc_aprintf(_tmp31E,_tag_fat(_tmpD8,sizeof(void*),1U));});});}}else{
# 826
fixup_string=({const char*_tmpDB="";_tag_fat(_tmpDB,sizeof(char),1U);});}{
# 828
struct _fat_ptr _tmpDC=({struct Cyc_String_pa_PrintArg_struct _tmp102=({struct Cyc_String_pa_PrintArg_struct _tmp2A7;_tmp2A7.tag=0U,_tmp2A7.f1=(struct _fat_ptr)((struct _fat_ptr)Cyc_cpp);_tmp2A7;});struct Cyc_String_pa_PrintArg_struct _tmp103=({struct Cyc_String_pa_PrintArg_struct _tmp2A6;_tmp2A6.tag=0U,_tmp2A6.f1=(struct _fat_ptr)((struct _fat_ptr)cppargs_string);_tmp2A6;});struct Cyc_String_pa_PrintArg_struct _tmp104=({struct Cyc_String_pa_PrintArg_struct _tmp2A5;_tmp2A5.tag=0U,_tmp2A5.f1=(struct _fat_ptr)((struct _fat_ptr)stdinc_string);_tmp2A5;});struct Cyc_String_pa_PrintArg_struct _tmp105=({struct Cyc_String_pa_PrintArg_struct _tmp2A4;_tmp2A4.tag=0U,({
# 831
struct _fat_ptr _tmp31F=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(filename));_tmp2A4.f1=_tmp31F;});_tmp2A4;});struct Cyc_String_pa_PrintArg_struct _tmp106=({struct Cyc_String_pa_PrintArg_struct _tmp2A3;_tmp2A3.tag=0U,_tmp2A3.f1=(struct _fat_ptr)((struct _fat_ptr)fixup_string);_tmp2A3;});struct Cyc_String_pa_PrintArg_struct _tmp107=({struct Cyc_String_pa_PrintArg_struct _tmp2A2;_tmp2A2.tag=0U,_tmp2A2.f1=(struct _fat_ptr)((struct _fat_ptr)ofile_string);_tmp2A2;});void*_tmp100[6U];_tmp100[0]=& _tmp102,_tmp100[1]=& _tmp103,_tmp100[2]=& _tmp104,_tmp100[3]=& _tmp105,_tmp100[4]=& _tmp106,_tmp100[5]=& _tmp107;({struct _fat_ptr _tmp320=({const char*_tmp101="%s %s%s %s%s%s";_tag_fat(_tmp101,sizeof(char),15U);});Cyc_aprintf(_tmp320,_tag_fat(_tmp100,sizeof(void*),6U));});});
# 828
struct _fat_ptr cmd=_tmpDC;
# 834
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmpDF=({struct Cyc_String_pa_PrintArg_struct _tmp2A0;_tmp2A0.tag=0U,_tmp2A0.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp2A0;});void*_tmpDD[1U];_tmpDD[0]=& _tmpDF;({struct Cyc___cycFILE*_tmp322=Cyc_stderr;struct _fat_ptr _tmp321=({const char*_tmpDE="%s\n";_tag_fat(_tmpDE,sizeof(char),4U);});Cyc_fprintf(_tmp322,_tmp321,_tag_fat(_tmpDD,sizeof(void*),1U));});});
if(system((const char*)_check_null(_untag_fat_ptr(cmd,sizeof(char),1U)))!= 0){
Cyc_compile_failure=1;
({void*_tmpE0=0U;({struct Cyc___cycFILE*_tmp324=Cyc_stderr;struct _fat_ptr _tmp323=({const char*_tmpE1="\nError: preprocessing\n";_tag_fat(_tmpE1,sizeof(char),23U);});Cyc_fprintf(_tmp324,_tmp323,_tag_fat(_tmpE0,sizeof(void*),0U));});});
if(!Cyc_stop_after_cpp_r)Cyc_remove_file((struct _fat_ptr)preprocfile);
return;}
# 841
if(Cyc_stop_after_cpp_r)return;
# 844
Cyc_Position_reset_position((struct _fat_ptr)preprocfile);{
struct Cyc___cycFILE*in_file=({struct _fat_ptr _tmp326=(struct _fat_ptr)preprocfile;struct _fat_ptr _tmp325=({const char*_tmpFE="r";_tag_fat(_tmpFE,sizeof(char),2U);});Cyc_try_file_open(_tmp326,_tmp325,({const char*_tmpFF="file";_tag_fat(_tmpFF,sizeof(char),5U);}));});
if(Cyc_compile_failure)return;{
# 848
struct Cyc_List_List*tds=0;
# 851
tds=({struct _fat_ptr _tmp329=({const char*_tmpE2="parsing";_tag_fat(_tmpE2,sizeof(char),8U);});struct Cyc_List_List*_tmp328=tds;struct Cyc___cycFILE*_tmp327=(struct Cyc___cycFILE*)_check_null(in_file);((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc___cycFILE*,struct Cyc_List_List*),struct Cyc___cycFILE*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp329,_tmp328,Cyc_do_parse,_tmp327,Cyc_remove_fileptr,preprocfileptr);});
# 853
if(Cyc_compile_failure)return;
# 855
if(Cyc_Absyn_porting_c_code){
Cyc_Port_port(tds);
return;}
# 860
({struct _fat_ptr _tmp32B=({const char*_tmpE3="binding";_tag_fat(_tmpE3,sizeof(char),8U);});struct Cyc_List_List*_tmp32A=tds;((int(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp32B,_tmp32A,Cyc_do_binding,1,Cyc_remove_fileptr,preprocfileptr);});
if(Cyc_compile_failure)return;{
# 863
struct Cyc_JumpAnalysis_Jump_Anal_Result*jump_tables;
{struct Cyc_Tcenv_Tenv*_tmpE4=Cyc_Tcenv_tc_init();struct Cyc_Tcenv_Tenv*te=_tmpE4;
# 866
if(Cyc_parseonly_r)goto PRINTC;
# 869
tds=({struct _fat_ptr _tmp32E=({const char*_tmpE5="type checking";_tag_fat(_tmpE5,sizeof(char),14U);});struct Cyc_List_List*_tmp32D=tds;struct Cyc_Tcenv_Tenv*_tmp32C=te;((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Tcenv_Tenv*,struct Cyc_List_List*),struct Cyc_Tcenv_Tenv*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp32E,_tmp32D,Cyc_do_typecheck,_tmp32C,Cyc_remove_fileptr,preprocfileptr);});
# 871
if(Cyc_compile_failure)return;
# 874
jump_tables=({struct _fat_ptr _tmp330=({const char*_tmpE6="jump checking";_tag_fat(_tmpE6,sizeof(char),14U);});struct Cyc_List_List*_tmp32F=tds;((struct Cyc_JumpAnalysis_Jump_Anal_Result*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_JumpAnalysis_Jump_Anal_Result*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp330,_tmp32F,Cyc_do_jumpanalysis,1,Cyc_remove_fileptr,preprocfileptr);});
# 876
if(Cyc_compile_failure)return;
# 879
if(Cyc_tc_r)goto PRINTC;
({struct _fat_ptr _tmp333=({const char*_tmpE7="control-flow checking";_tag_fat(_tmpE7,sizeof(char),22U);});struct Cyc_List_List*_tmp332=tds;struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp331=jump_tables;((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp333,_tmp332,Cyc_do_cfcheck,_tmp331,Cyc_remove_fileptr,preprocfileptr);});
# 882
if(Cyc_compile_failure)return;
# 884
if(Cyc_vcgen_r){
({struct _fat_ptr _tmp336=({const char*_tmpE8="verification condition generation";_tag_fat(_tmpE8,sizeof(char),34U);});struct Cyc_List_List*_tmp335=tds;struct Cyc_JumpAnalysis_Jump_Anal_Result*_tmp334=jump_tables;((int(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,int(*f)(struct Cyc_JumpAnalysis_Jump_Anal_Result*,struct Cyc_List_List*),struct Cyc_JumpAnalysis_Jump_Anal_Result*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp336,_tmp335,Cyc_do_vcgen,_tmp334,Cyc_remove_fileptr,preprocfileptr);});
# 887
if(Cyc_compile_failure)return;}
# 889
({struct _fat_ptr _tmp338=({const char*_tmpE9="check insertion";_tag_fat(_tmpE9,sizeof(char),16U);});struct Cyc_List_List*_tmp337=tds;((int(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,int(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp338,_tmp337,Cyc_do_insert_checks,1,Cyc_remove_fileptr,preprocfileptr);});
# 891
if(Cyc_compile_failure)return;
# 894
if(Cyc_generate_interface_r){
struct Cyc___cycFILE*inter_file=({struct _fat_ptr _tmp33A=({const char*_tmpEA=(const char*)_check_null(_untag_fat_ptr(interfacefile,sizeof(char),1U));_tag_fat(_tmpEA,sizeof(char),_get_zero_arr_size_char((void*)_tmpEA,1U));});struct _fat_ptr _tmp339=({const char*_tmpEB="w";_tag_fat(_tmpEB,sizeof(char),2U);});Cyc_try_file_open(_tmp33A,_tmp339,({const char*_tmpEC="interface file";_tag_fat(_tmpEC,sizeof(char),15U);}));});
if(inter_file == 0){
Cyc_compile_failure=1;
return;}
# 900
Cyc_Absyndump_set_params(& Cyc_Absynpp_cyci_params_r);
Cyc_Absyndump_dump_interface(tds,inter_file);
Cyc_fclose(inter_file);
Cyc_Absynpp_set_params(& Cyc_Absynpp_tc_params_r);}
# 907
if(Cyc_ic_r){
struct Cyc___cycFILE*inter_file=Cyc_fopen((const char*)_check_null(_untag_fat_ptr(interfacefile,sizeof(char),1U)),"r");
struct Cyc___cycFILE*inter_objfile=({struct _fat_ptr _tmp33C=(struct _fat_ptr)interfaceobjfile;struct _fat_ptr _tmp33B=({const char*_tmpEF="w";_tag_fat(_tmpEF,sizeof(char),2U);});Cyc_try_file_open(_tmp33C,_tmp33B,({const char*_tmpF0="interface object file";_tag_fat(_tmpF0,sizeof(char),22U);}));});
if(inter_objfile == 0){
Cyc_compile_failure=1;
return;}
# 914
Cyc_Position_reset_position(interfacefile);{
struct _tuple27 _tmpED=({struct _tuple27 _tmp2A1;_tmp2A1.f1=te,_tmp2A1.f2=inter_file,_tmp2A1.f3=inter_objfile;_tmp2A1;});struct _tuple27 int_env=_tmpED;
tds=({struct _fat_ptr _tmp33D=({const char*_tmpEE="interface checking";_tag_fat(_tmpEE,sizeof(char),19U);});((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple27*,struct Cyc_List_List*),struct _tuple27*env,void(*on_fail)(struct _tuple27*),struct _tuple27*failenv))Cyc_do_stage)(_tmp33D,tds,Cyc_do_interface,& int_env,Cyc_interface_fail,& int_env);});}}}
# 920
if(Cyc_cf_r)goto PRINTC;
# 923
tds=({struct _fat_ptr _tmp340=({const char*_tmpF1="translation to C";_tag_fat(_tmpF1,sizeof(char),17U);});struct Cyc_List_List*_tmp33F=tds;struct Cyc_Hashtable_Table*_tmp33E=jump_tables->pop_tables;((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct Cyc_Hashtable_Table*,struct Cyc_List_List*),struct Cyc_Hashtable_Table*env,void(*on_fail)(struct _fat_ptr*),struct _fat_ptr*failenv))Cyc_do_stage)(_tmp340,_tmp33F,Cyc_do_translate,_tmp33E,Cyc_remove_fileptr,preprocfileptr);});
# 926
tds=({struct _fat_ptr _tmp341=({const char*_tmpF2="aggregate removal";_tag_fat(_tmpF2,sizeof(char),18U);});((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp341,tds,Cyc_do_removeaggrs,1,(void(*)(int x))Cyc_ignore,1);});
if(Cyc_compile_failure)return;
# 930
if(Cyc_toseqc_r)
tds=({struct _fat_ptr _tmp342=({const char*_tmpF3="post-pass to L-to-R evaluation order";_tag_fat(_tmpF3,sizeof(char),37U);});((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp342,tds,Cyc_do_toseqc,1,(void(*)(int x))Cyc_ignore,1);});
# 934
Cyc_Toc_finish();
# 936
if(Cyc_compile_failure)return;
# 939
if(Cyc_Cyclone_tovc_r || Cyc_elim_se_r)
tds=({struct _fat_ptr _tmp343=({const char*_tmpF4="post-pass to VC";_tag_fat(_tmpF4,sizeof(char),16U);});((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(int,struct Cyc_List_List*),int env,void(*on_fail)(int),int failenv))Cyc_do_stage)(_tmp343,tds,Cyc_do_tovc,1,(void(*)(int x))Cyc_ignore,1);});
if(Cyc_compile_failure)return;
# 943
Cyc_Warn_flush_warnings();
Cyc_remove_file((struct _fat_ptr)preprocfile);
# 947
PRINTC: {
struct Cyc___cycFILE*out_file;
if((Cyc_parseonly_r || Cyc_tc_r)|| Cyc_cf_r){
if(Cyc_output_file != 0)
out_file=({struct _fat_ptr _tmp345=*((struct _fat_ptr*)_check_null(Cyc_output_file));struct _fat_ptr _tmp344=({const char*_tmpF5="w";_tag_fat(_tmpF5,sizeof(char),2U);});Cyc_try_file_open(_tmp345,_tmp344,({const char*_tmpF6="output file";_tag_fat(_tmpF6,sizeof(char),12U);}));});else{
# 953
out_file=Cyc_stdout;}}else{
if(Cyc_toc_r && Cyc_output_file != 0)
out_file=({struct _fat_ptr _tmp347=*((struct _fat_ptr*)_check_null(Cyc_output_file));struct _fat_ptr _tmp346=({const char*_tmpF7="w";_tag_fat(_tmpF7,sizeof(char),2U);});Cyc_try_file_open(_tmp347,_tmp346,({const char*_tmpF8="output file";_tag_fat(_tmpF8,sizeof(char),12U);}));});else{
# 957
out_file=({struct _fat_ptr _tmp349=(struct _fat_ptr)cfile;struct _fat_ptr _tmp348=({const char*_tmpF9="w";_tag_fat(_tmpF9,sizeof(char),2U);});Cyc_try_file_open(_tmp349,_tmp348,({const char*_tmpFA="output file";_tag_fat(_tmpFA,sizeof(char),12U);}));});}}
# 959
if(Cyc_compile_failure || !((unsigned)out_file))return;
# 961
if(!Cyc_noprint_r){
struct _tuple28*_tmpFB=({struct _tuple28*_tmpFD=_cycalloc(sizeof(*_tmpFD));_tmpFD->f1=out_file,_tmpFD->f2=cfile;_tmpFD;});struct _tuple28*env=_tmpFB;
tds=({struct _fat_ptr _tmp34C=({const char*_tmpFC="printing";_tag_fat(_tmpFC,sizeof(char),9U);});struct Cyc_List_List*_tmp34B=tds;struct _tuple28*_tmp34A=env;((struct Cyc_List_List*(*)(struct _fat_ptr stage_name,struct Cyc_List_List*tds,struct Cyc_List_List*(*f)(struct _tuple28*,struct Cyc_List_List*),struct _tuple28*env,void(*on_fail)(struct _tuple28*),struct _tuple28*failenv))Cyc_do_stage)(_tmp34C,_tmp34B,Cyc_do_print,_tmp34A,Cyc_print_fail,env);});}}}}}}}}}}}}static char _tmp117[8U]="<final>";
# 968
static struct _fat_ptr Cyc_final_str={_tmp117,_tmp117,_tmp117 + 8U};
static struct _fat_ptr*Cyc_final_strptr=& Cyc_final_str;
# 971
static struct Cyc_Interface_I*Cyc_read_cycio(struct _fat_ptr*n){
if(n == Cyc_final_strptr)
return Cyc_Interface_final();{
# 975
struct _fat_ptr basename;
{struct _handler_cons _tmp118;_push_handler(& _tmp118);{int _tmp11A=0;if(setjmp(_tmp118.handler))_tmp11A=1;if(!_tmp11A){basename=(struct _fat_ptr)Cyc_Filename_chop_extension(*n);;_pop_handler();}else{void*_tmp119=(void*)Cyc_Core_get_exn_thrown();void*_tmp11B=_tmp119;void*_tmp11C;if(((struct Cyc_Core_Invalid_argument_exn_struct*)_tmp11B)->tag == Cyc_Core_Invalid_argument){_LL1: _LL2:
 basename=*n;goto _LL0;}else{_LL3: _tmp11C=_tmp11B;_LL4: {void*exn=_tmp11C;(int)_rethrow(exn);}}_LL0:;}}}{
# 979
struct _fat_ptr _tmp11D=({struct _fat_ptr _tmp34D=(struct _fat_ptr)basename;Cyc_strconcat(_tmp34D,({const char*_tmp122=".cycio";_tag_fat(_tmp122,sizeof(char),7U);}));});struct _fat_ptr nf=_tmp11D;
struct Cyc___cycFILE*_tmp11E=({struct _fat_ptr _tmp34F=(struct _fat_ptr)nf;struct _fat_ptr _tmp34E=({const char*_tmp120="rb";_tag_fat(_tmp120,sizeof(char),3U);});Cyc_try_file_open(_tmp34F,_tmp34E,({const char*_tmp121="interface object file";_tag_fat(_tmp121,sizeof(char),22U);}));});struct Cyc___cycFILE*f=_tmp11E;
if(f == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();
exit(1);}
# 986
Cyc_Position_reset_position((struct _fat_ptr)nf);{
# 988
struct Cyc_Interface_I*_tmp11F=Cyc_Interface_load(f);struct Cyc_Interface_I*i=_tmp11F;
Cyc_file_close(f);
return i;}}}}
# 993
static int Cyc_is_cfile(struct _fat_ptr*n){
return(int)*((const char*)_check_fat_subscript(*n,sizeof(char),0))!= (int)'-';}
# 997
extern void GC_blacklist_warn_clear (void);struct _tuple29{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
# 1007
void Cyc_print_options (void);
# 1014
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt1_spec={3U,& Cyc_v_r};static char _tmp123[3U]="-v";static char _tmp124[1U]="";static char _tmp125[35U]="print compilation stages verbosely";struct _tuple29 Cyc_opt1_tuple={{_tmp123,_tmp123,_tmp123 + 3U},0,{_tmp124,_tmp124,_tmp124 + 1U},(void*)& Cyc_opt1_spec,{_tmp125,_tmp125,_tmp125 + 35U}};struct Cyc_List_List Cyc_opt1={(void*)& Cyc_opt1_tuple,0};
# 1016
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt2_spec={0U,Cyc_print_version};static char _tmp126[10U]="--version";static char _tmp127[1U]="";static char _tmp128[35U]="Print version information and exit";struct _tuple29 Cyc_opt2_tuple={{_tmp126,_tmp126,_tmp126 + 10U},0,{_tmp127,_tmp127,_tmp127 + 1U},(void*)& Cyc_opt2_spec,{_tmp128,_tmp128,_tmp128 + 35U}};struct Cyc_List_List Cyc_opt2={(void*)& Cyc_opt2_tuple,0};
# 1019
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt3_spec={5U,Cyc_set_output_file};static char _tmp129[3U]="-o";static char _tmp12A[8U]=" <file>";static char _tmp12B[35U]="Set the output file name to <file>";struct _tuple29 Cyc_opt3_tuple={{_tmp129,_tmp129,_tmp129 + 3U},0,{_tmp12A,_tmp12A,_tmp12A + 8U},(void*)& Cyc_opt3_spec,{_tmp12B,_tmp12B,_tmp12B + 35U}};struct Cyc_List_List Cyc_opt3={(void*)& Cyc_opt3_tuple,0};
# 1022
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt4_spec={1U,Cyc_add_cpparg};static char _tmp12C[3U]="-D";static char _tmp12D[17U]="<name>[=<value>]";static char _tmp12E[32U]="Pass definition to preprocessor";struct _tuple29 Cyc_opt4_tuple={{_tmp12C,_tmp12C,_tmp12C + 3U},1,{_tmp12D,_tmp12D,_tmp12D + 17U},(void*)& Cyc_opt4_spec,{_tmp12E,_tmp12E,_tmp12E + 32U}};struct Cyc_List_List Cyc_opt4={(void*)& Cyc_opt4_tuple,0};
# 1025
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt5_spec={1U,Cyc_Specsfile_add_cyclone_exec_path};static char _tmp12F[3U]="-B";static char _tmp130[7U]="<file>";static char _tmp131[60U]="Add to the list of directories to search for compiler files";struct _tuple29 Cyc_opt5_tuple={{_tmp12F,_tmp12F,_tmp12F + 3U},1,{_tmp130,_tmp130,_tmp130 + 7U},(void*)& Cyc_opt5_spec,{_tmp131,_tmp131,_tmp131 + 60U}};struct Cyc_List_List Cyc_opt5={(void*)& Cyc_opt5_tuple,0};
# 1028
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt6_spec={1U,Cyc_add_cpparg};static char _tmp132[3U]="-I";static char _tmp133[6U]="<dir>";static char _tmp134[59U]="Add to the list of directories to search for include files";struct _tuple29 Cyc_opt6_tuple={{_tmp132,_tmp132,_tmp132 + 3U},1,{_tmp133,_tmp133,_tmp133 + 6U},(void*)& Cyc_opt6_spec,{_tmp134,_tmp134,_tmp134 + 59U}};struct Cyc_List_List Cyc_opt6={(void*)& Cyc_opt6_tuple,0};
# 1031
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt7_spec={1U,Cyc_add_ccarg};static char _tmp135[3U]="-L";static char _tmp136[6U]="<dir>";static char _tmp137[38U]="Add to the list of directories for -l";struct _tuple29 Cyc_opt7_tuple={{_tmp135,_tmp135,_tmp135 + 3U},1,{_tmp136,_tmp136,_tmp136 + 6U},(void*)& Cyc_opt7_spec,{_tmp137,_tmp137,_tmp137 + 38U}};struct Cyc_List_List Cyc_opt7={(void*)& Cyc_opt7_tuple,0};
# 1034
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt8_spec={1U,Cyc_add_libarg};static char _tmp138[3U]="-l";static char _tmp139[10U]="<libname>";static char _tmp13A[13U]="Library file";struct _tuple29 Cyc_opt8_tuple={{_tmp138,_tmp138,_tmp138 + 3U},1,{_tmp139,_tmp139,_tmp139 + 10U},(void*)& Cyc_opt8_spec,{_tmp13A,_tmp13A,_tmp13A + 13U}};struct Cyc_List_List Cyc_opt8={(void*)& Cyc_opt8_tuple,0};
# 1037
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt9_spec={1U,Cyc_add_marg};static char _tmp13B[3U]="-m";static char _tmp13C[9U]="<option>";static char _tmp13D[52U]="GCC specific: pass machine-dependent flag on to gcc";struct _tuple29 Cyc_opt9_tuple={{_tmp13B,_tmp13B,_tmp13B + 3U},1,{_tmp13C,_tmp13C,_tmp13C + 9U},(void*)& Cyc_opt9_spec,{_tmp13D,_tmp13D,_tmp13D + 52U}};struct Cyc_List_List Cyc_opt9={(void*)& Cyc_opt9_tuple,0};
# 1040
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt10_spec={0U,Cyc_set_stop_after_objectfile};static char _tmp13E[3U]="-c";static char _tmp13F[1U]="";static char _tmp140[61U]="Produce an object file instead of an executable; do not link";struct _tuple29 Cyc_opt10_tuple={{_tmp13E,_tmp13E,_tmp13E + 3U},0,{_tmp13F,_tmp13F,_tmp13F + 1U},(void*)& Cyc_opt10_spec,{_tmp140,_tmp140,_tmp140 + 61U}};struct Cyc_List_List Cyc_opt10={(void*)& Cyc_opt10_tuple,0};
# 1043
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt11_spec={5U,Cyc_set_inputtype};static char _tmp141[3U]="-x";static char _tmp142[12U]=" <language>";static char _tmp143[49U]="Specify <language> for the following input files";struct _tuple29 Cyc_opt11_tuple={{_tmp141,_tmp141,_tmp141 + 3U},0,{_tmp142,_tmp142,_tmp142 + 12U},(void*)& Cyc_opt11_spec,{_tmp143,_tmp143,_tmp143 + 49U}};struct Cyc_List_List Cyc_opt11={(void*)& Cyc_opt11_tuple,0};
# 1046
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt12_spec={0U,Cyc_set_pa};static char _tmp144[4U]="-pa";static char _tmp145[1U]="";static char _tmp146[33U]="Compile for profiling with aprof";struct _tuple29 Cyc_opt12_tuple={{_tmp144,_tmp144,_tmp144 + 4U},0,{_tmp145,_tmp145,_tmp145 + 1U},(void*)& Cyc_opt12_spec,{_tmp146,_tmp146,_tmp146 + 33U}};struct Cyc_List_List Cyc_opt12={(void*)& Cyc_opt12_tuple,0};
# 1049
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt13_spec={0U,Cyc_set_stop_after_asmfile};static char _tmp147[3U]="-S";static char _tmp148[1U]="";static char _tmp149[35U]="Stop after producing assembly code";struct _tuple29 Cyc_opt13_tuple={{_tmp147,_tmp147,_tmp147 + 3U},0,{_tmp148,_tmp148,_tmp148 + 1U},(void*)& Cyc_opt13_spec,{_tmp149,_tmp149,_tmp149 + 35U}};struct Cyc_List_List Cyc_opt13={(void*)& Cyc_opt13_tuple,0};
# 1052
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt14_spec={0U,Cyc_set_produce_dependencies};static char _tmp14A[3U]="-M";static char _tmp14B[1U]="";static char _tmp14C[21U]="Produce dependencies";struct _tuple29 Cyc_opt14_tuple={{_tmp14A,_tmp14A,_tmp14A + 3U},0,{_tmp14B,_tmp14B,_tmp14B + 1U},(void*)& Cyc_opt14_spec,{_tmp14C,_tmp14C,_tmp14C + 21U}};struct Cyc_List_List Cyc_opt14={(void*)& Cyc_opt14_tuple,0};
# 1055
struct Cyc_Arg_Flag_spec_Arg_Spec_struct Cyc_opt15_spec={1U,Cyc_add_cpparg};static char _tmp14D[4U]="-MG";static char _tmp14E[1U]="";static char _tmp14F[68U]="When producing dependencies assume that missing files are generated";struct _tuple29 Cyc_opt15_tuple={{_tmp14D,_tmp14D,_tmp14D + 4U},0,{_tmp14E,_tmp14E,_tmp14E + 1U},(void*)& Cyc_opt15_spec,{_tmp14F,_tmp14F,_tmp14F + 68U}};struct Cyc_List_List Cyc_opt15={(void*)& Cyc_opt15_tuple,0};
# 1059
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt16_spec={5U,Cyc_set_dependencies_target};static char _tmp150[4U]="-MT";static char _tmp151[10U]=" <target>";static char _tmp152[29U]="Give target for dependencies";struct _tuple29 Cyc_opt16_tuple={{_tmp150,_tmp150,_tmp150 + 4U},0,{_tmp151,_tmp151,_tmp151 + 10U},(void*)& Cyc_opt16_spec,{_tmp152,_tmp152,_tmp152 + 29U}};struct Cyc_List_List Cyc_opt16={(void*)& Cyc_opt16_tuple,0};
# 1062
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt17_spec={5U,Cyc_Specsfile_set_target_arch};static char _tmp153[3U]="-b";static char _tmp154[10U]="<machine>";static char _tmp155[19U]="Set target machine";struct _tuple29 Cyc_opt17_tuple={{_tmp153,_tmp153,_tmp153 + 3U},0,{_tmp154,_tmp154,_tmp154 + 10U},(void*)& Cyc_opt17_spec,{_tmp155,_tmp155,_tmp155 + 19U}};struct Cyc_List_List Cyc_opt17={(void*)& Cyc_opt17_tuple,0};
# 1065
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt18_spec={3U,& Cyc_NewControlFlow_warn_lose_unique};static char _tmp156[14U]="-Wlose-unique";static char _tmp157[1U]="";static char _tmp158[49U]="Try to warn when a unique pointer might get lost";struct _tuple29 Cyc_opt18_tuple={{_tmp156,_tmp156,_tmp156 + 14U},0,{_tmp157,_tmp157,_tmp157 + 1U},(void*)& Cyc_opt18_spec,{_tmp158,_tmp158,_tmp158 + 49U}};struct Cyc_List_List Cyc_opt18={(void*)& Cyc_opt18_tuple,0};
# 1068
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt19_spec={3U,& Cyc_Binding_warn_override};static char _tmp159[11U]="-Woverride";static char _tmp15A[1U]="";static char _tmp15B[58U]="Warn when a local variable overrides an existing variable";struct _tuple29 Cyc_opt19_tuple={{_tmp159,_tmp159,_tmp159 + 11U},0,{_tmp15A,_tmp15A,_tmp15A + 1U},(void*)& Cyc_opt19_spec,{_tmp15B,_tmp15B,_tmp15B + 58U}};struct Cyc_List_List Cyc_opt19={(void*)& Cyc_opt19_tuple,0};
# 1071
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt20_spec={0U,Cyc_set_all_warnings};static char _tmp15C[6U]="-Wall";static char _tmp15D[1U]="";static char _tmp15E[21U]="Turn on all warnings";struct _tuple29 Cyc_opt20_tuple={{_tmp15C,_tmp15C,_tmp15C + 6U},0,{_tmp15D,_tmp15D,_tmp15D + 1U},(void*)& Cyc_opt20_spec,{_tmp15E,_tmp15E,_tmp15E + 21U}};struct Cyc_List_List Cyc_opt20={(void*)& Cyc_opt20_tuple,0};
# 1074
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt21_spec={3U,& Cyc_stop_after_cpp_r};static char _tmp15F[3U]="-E";static char _tmp160[1U]="";static char _tmp161[25U]="Stop after preprocessing";struct _tuple29 Cyc_opt21_tuple={{_tmp15F,_tmp15F,_tmp15F + 3U},0,{_tmp160,_tmp160,_tmp160 + 1U},(void*)& Cyc_opt21_spec,{_tmp161,_tmp161,_tmp161 + 25U}};struct Cyc_List_List Cyc_opt21={(void*)& Cyc_opt21_tuple,0};
# 1077
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt22_spec={3U,& Cyc_parseonly_r};static char _tmp162[17U]="-stopafter-parse";static char _tmp163[1U]="";static char _tmp164[19U]="Stop after parsing";struct _tuple29 Cyc_opt22_tuple={{_tmp162,_tmp162,_tmp162 + 17U},0,{_tmp163,_tmp163,_tmp163 + 1U},(void*)& Cyc_opt22_spec,{_tmp164,_tmp164,_tmp164 + 19U}};struct Cyc_List_List Cyc_opt22={(void*)& Cyc_opt22_tuple,0};
# 1080
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt23_spec={3U,& Cyc_tc_r};static char _tmp165[14U]="-stopafter-tc";static char _tmp166[1U]="";static char _tmp167[25U]="Stop after type checking";struct _tuple29 Cyc_opt23_tuple={{_tmp165,_tmp165,_tmp165 + 14U},0,{_tmp166,_tmp166,_tmp166 + 1U},(void*)& Cyc_opt23_spec,{_tmp167,_tmp167,_tmp167 + 25U}};struct Cyc_List_List Cyc_opt23={(void*)& Cyc_opt23_tuple,0};
# 1083
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt24_spec={3U,& Cyc_noprint_r};static char _tmp168[9U]="-noprint";static char _tmp169[1U]="";static char _tmp16A[42U]="Do not print output (when stopping early)";struct _tuple29 Cyc_opt24_tuple={{_tmp168,_tmp168,_tmp168 + 9U},0,{_tmp169,_tmp169,_tmp169 + 1U},(void*)& Cyc_opt24_spec,{_tmp16A,_tmp16A,_tmp16A + 42U}};struct Cyc_List_List Cyc_opt24={(void*)& Cyc_opt24_tuple,0};
# 1086
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt25_spec={3U,& Cyc_cf_r};static char _tmp16B[14U]="-stopafter-cf";static char _tmp16C[1U]="";static char _tmp16D[33U]="Stop after control-flow checking";struct _tuple29 Cyc_opt25_tuple={{_tmp16B,_tmp16B,_tmp16B + 14U},0,{_tmp16C,_tmp16C,_tmp16C + 1U},(void*)& Cyc_opt25_spec,{_tmp16D,_tmp16D,_tmp16D + 33U}};struct Cyc_List_List Cyc_opt25={(void*)& Cyc_opt25_tuple,0};
# 1089
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt26_spec={3U,& Cyc_toc_r};static char _tmp16E[15U]="-stopafter-toc";static char _tmp16F[1U]="";static char _tmp170[28U]="Stop after translation to C";struct _tuple29 Cyc_opt26_tuple={{_tmp16E,_tmp16E,_tmp16E + 15U},0,{_tmp16F,_tmp16F,_tmp16F + 1U},(void*)& Cyc_opt26_spec,{_tmp170,_tmp170,_tmp170 + 28U}};struct Cyc_List_List Cyc_opt26={(void*)& Cyc_opt26_tuple,0};
# 1092
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt27_spec={3U,& Cyc_ic_r};static char _tmp171[4U]="-ic";static char _tmp172[1U]="";static char _tmp173[26U]="Activate the link-checker";struct _tuple29 Cyc_opt27_tuple={{_tmp171,_tmp171,_tmp171 + 4U},0,{_tmp172,_tmp172,_tmp172 + 1U},(void*)& Cyc_opt27_spec,{_tmp173,_tmp173,_tmp173 + 26U}};struct Cyc_List_List Cyc_opt27={(void*)& Cyc_opt27_tuple,0};
# 1095
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt28_spec={3U,& Cyc_pp_r};static char _tmp174[4U]="-pp";static char _tmp175[1U]="";static char _tmp176[47U]="Pretty print the C code that Cyclone generates";struct _tuple29 Cyc_opt28_tuple={{_tmp174,_tmp174,_tmp174 + 4U},0,{_tmp175,_tmp175,_tmp175 + 1U},(void*)& Cyc_opt28_spec,{_tmp176,_tmp176,_tmp176 + 47U}};struct Cyc_List_List Cyc_opt28={(void*)& Cyc_opt28_tuple,0};
# 1098
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt29_spec={4U,& Cyc_pp_r};static char _tmp177[4U]="-up";static char _tmp178[1U]="";static char _tmp179[55U]="Ugly print the C code that Cyclone generates (default)";struct _tuple29 Cyc_opt29_tuple={{_tmp177,_tmp177,_tmp177 + 4U},0,{_tmp178,_tmp178,_tmp178 + 1U},(void*)& Cyc_opt29_spec,{_tmp179,_tmp179,_tmp179 + 55U}};struct Cyc_List_List Cyc_opt29={(void*)& Cyc_opt29_tuple,0};
# 1101
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt30_spec={3U,& Cyc_elim_se_r};static char _tmp17A[28U]="-elim-statement-expressions";static char _tmp17B[1U]="";static char _tmp17C[66U]="Avoid statement expressions in C output (implies --inline-checks)";struct _tuple29 Cyc_opt30_tuple={{_tmp17A,_tmp17A,_tmp17A + 28U},0,{_tmp17B,_tmp17B,_tmp17B + 1U},(void*)& Cyc_opt30_spec,{_tmp17C,_tmp17C,_tmp17C + 66U}};struct Cyc_List_List Cyc_opt30={(void*)& Cyc_opt30_tuple,0};
# 1105
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt31_spec={4U,& Cyc_elim_se_r};static char _tmp17D[31U]="-no-elim-statement-expressions";static char _tmp17E[1U]="";static char _tmp17F[47U]="Do not avoid statement expressions in C output";struct _tuple29 Cyc_opt31_tuple={{_tmp17D,_tmp17D,_tmp17D + 31U},0,{_tmp17E,_tmp17E,_tmp17E + 1U},(void*)& Cyc_opt31_spec,{_tmp17F,_tmp17F,_tmp17F + 47U}};struct Cyc_List_List Cyc_opt31={(void*)& Cyc_opt31_tuple,0};
# 1108
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt32_spec={0U,Cyc_set_tovc};static char _tmp180[8U]="-un-gcc";static char _tmp181[1U]="";static char _tmp182[57U]="Avoid gcc extensions in C output (except for attributes)";struct _tuple29 Cyc_opt32_tuple={{_tmp180,_tmp180,_tmp180 + 8U},0,{_tmp181,_tmp181,_tmp181 + 1U},(void*)& Cyc_opt32_spec,{_tmp182,_tmp182,_tmp182 + 57U}};struct Cyc_List_List Cyc_opt32={(void*)& Cyc_opt32_tuple,0};
# 1111
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt33_spec={5U,Cyc_set_c_compiler};static char _tmp183[8U]="-c-comp";static char _tmp184[12U]=" <compiler>";static char _tmp185[40U]="Produce C output for the given compiler";struct _tuple29 Cyc_opt33_tuple={{_tmp183,_tmp183,_tmp183 + 8U},0,{_tmp184,_tmp184,_tmp184 + 12U},(void*)& Cyc_opt33_spec,{_tmp185,_tmp185,_tmp185 + 40U}};struct Cyc_List_List Cyc_opt33={(void*)& Cyc_opt33_tuple,0};
# 1114
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt34_spec={0U,Cyc_set_save_temps};static char _tmp186[12U]="-save-temps";static char _tmp187[1U]="";static char _tmp188[29U]="Don't delete temporary files";struct _tuple29 Cyc_opt34_tuple={{_tmp186,_tmp186,_tmp186 + 12U},0,{_tmp187,_tmp187,_tmp187 + 1U},(void*)& Cyc_opt34_spec,{_tmp188,_tmp188,_tmp188 + 29U}};struct Cyc_List_List Cyc_opt34={(void*)& Cyc_opt34_tuple,0};
# 1117
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt35_spec={3U,& Cyc_save_c_r};static char _tmp189[8U]="-save-c";static char _tmp18A[1U]="";static char _tmp18B[31U]="Don't delete temporary C files";struct _tuple29 Cyc_opt35_tuple={{_tmp189,_tmp189,_tmp189 + 8U},0,{_tmp18A,_tmp18A,_tmp18A + 1U},(void*)& Cyc_opt35_spec,{_tmp18B,_tmp18B,_tmp18B + 31U}};struct Cyc_List_List Cyc_opt35={(void*)& Cyc_opt35_tuple,0};
# 1120
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt36_spec={0U,Cyc_clear_lineno};static char _tmp18C[11U]="--nolineno";static char _tmp18D[1U]="";static char _tmp18E[63U]="Don't generate line numbers for debugging (automatic with -pp)";struct _tuple29 Cyc_opt36_tuple={{_tmp18C,_tmp18C,_tmp18C + 11U},0,{_tmp18D,_tmp18D,_tmp18D + 1U},(void*)& Cyc_opt36_spec,{_tmp18E,_tmp18E,_tmp18E + 63U}};struct Cyc_List_List Cyc_opt36={(void*)& Cyc_opt36_tuple,0};
# 1123
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt37_spec={0U,Cyc_set_nochecks};static char _tmp18F[11U]="--nochecks";static char _tmp190[1U]="";static char _tmp191[27U]="Disable bounds/null checks";struct _tuple29 Cyc_opt37_tuple={{_tmp18F,_tmp18F,_tmp18F + 11U},0,{_tmp190,_tmp190,_tmp190 + 1U},(void*)& Cyc_opt37_spec,{_tmp191,_tmp191,_tmp191 + 27U}};struct Cyc_List_List Cyc_opt37={(void*)& Cyc_opt37_tuple,0};
# 1126
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt38_spec={0U,Cyc_set_nonullchecks};static char _tmp192[15U]="--nonullchecks";static char _tmp193[1U]="";static char _tmp194[20U]="Disable null checks";struct _tuple29 Cyc_opt38_tuple={{_tmp192,_tmp192,_tmp192 + 15U},0,{_tmp193,_tmp193,_tmp193 + 1U},(void*)& Cyc_opt38_spec,{_tmp194,_tmp194,_tmp194 + 20U}};struct Cyc_List_List Cyc_opt38={(void*)& Cyc_opt38_tuple,0};
# 1129
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt39_spec={0U,Cyc_set_noboundschecks};static char _tmp195[17U]="--noboundschecks";static char _tmp196[1U]="";static char _tmp197[22U]="Disable bounds checks";struct _tuple29 Cyc_opt39_tuple={{_tmp195,_tmp195,_tmp195 + 17U},0,{_tmp196,_tmp196,_tmp196 + 1U},(void*)& Cyc_opt39_spec,{_tmp197,_tmp197,_tmp197 + 22U}};struct Cyc_List_List Cyc_opt39={(void*)& Cyc_opt39_tuple,0};
# 1132
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt40_spec={0U,Cyc_set_inline_functions};static char _tmp198[16U]="--inline-checks";static char _tmp199[1U]="";static char _tmp19A[40U]="Inline bounds checks instead of #define";struct _tuple29 Cyc_opt40_tuple={{_tmp198,_tmp198,_tmp198 + 16U},0,{_tmp199,_tmp199,_tmp199 + 1U},(void*)& Cyc_opt40_spec,{_tmp19A,_tmp19A,_tmp19A + 40U}};struct Cyc_List_List Cyc_opt40={(void*)& Cyc_opt40_tuple,0};
# 1135
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt41_spec={5U,Cyc_set_cpp};static char _tmp19B[9U]="-use-cpp";static char _tmp19C[7U]="<path>";static char _tmp19D[35U]="Indicate which preprocessor to use";struct _tuple29 Cyc_opt41_tuple={{_tmp19B,_tmp19B,_tmp19B + 9U},0,{_tmp19C,_tmp19C,_tmp19C + 7U},(void*)& Cyc_opt41_spec,{_tmp19D,_tmp19D,_tmp19D + 35U}};struct Cyc_List_List Cyc_opt41={(void*)& Cyc_opt41_tuple,0};
# 1138
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt42_spec={0U,Cyc_set_nocppprecomp};static char _tmp19E[16U]="-no-cpp-precomp";static char _tmp19F[1U]="";static char _tmp1A0[40U]="Don't do smart preprocessing (mac only)";struct _tuple29 Cyc_opt42_tuple={{_tmp19E,_tmp19E,_tmp19E + 16U},0,{_tmp19F,_tmp19F,_tmp19F + 1U},(void*)& Cyc_opt42_spec,{_tmp1A0,_tmp1A0,_tmp1A0 + 40U}};struct Cyc_List_List Cyc_opt42={(void*)& Cyc_opt42_tuple,0};
# 1141
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt43_spec={0U,Cyc_set_nocyc};static char _tmp1A1[7U]="-nocyc";static char _tmp1A2[1U]="";static char _tmp1A3[33U]="Don't add implicit namespace Cyc";struct _tuple29 Cyc_opt43_tuple={{_tmp1A1,_tmp1A1,_tmp1A1 + 7U},0,{_tmp1A2,_tmp1A2,_tmp1A2 + 1U},(void*)& Cyc_opt43_spec,{_tmp1A3,_tmp1A3,_tmp1A3 + 33U}};struct Cyc_List_List Cyc_opt43={(void*)& Cyc_opt43_tuple,0};
# 1144
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt44_spec={3U,& Cyc_nogc_r};static char _tmp1A4[6U]="-nogc";static char _tmp1A5[1U]="";static char _tmp1A6[36U]="Don't link in the garbage collector";struct _tuple29 Cyc_opt44_tuple={{_tmp1A4,_tmp1A4,_tmp1A4 + 6U},0,{_tmp1A5,_tmp1A5,_tmp1A5 + 1U},(void*)& Cyc_opt44_spec,{_tmp1A6,_tmp1A6,_tmp1A6 + 36U}};struct Cyc_List_List Cyc_opt44={(void*)& Cyc_opt44_tuple,0};
# 1147
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt45_spec={3U,& Cyc_noshake_r};static char _tmp1A7[16U]="-noremoveunused";static char _tmp1A8[1U]="";static char _tmp1A9[49U]="Don't remove externed variables that aren't used";struct _tuple29 Cyc_opt45_tuple={{_tmp1A7,_tmp1A7,_tmp1A7 + 16U},0,{_tmp1A8,_tmp1A8,_tmp1A8 + 1U},(void*)& Cyc_opt45_spec,{_tmp1A9,_tmp1A9,_tmp1A9 + 49U}};struct Cyc_List_List Cyc_opt45={(void*)& Cyc_opt45_tuple,0};
# 1150
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt46_spec={3U,& Cyc_noexpand_r};static char _tmp1AA[18U]="-noexpandtypedefs";static char _tmp1AB[1U]="";static char _tmp1AC[41U]="Don't expand typedefs in pretty printing";struct _tuple29 Cyc_opt46_tuple={{_tmp1AA,_tmp1AA,_tmp1AA + 18U},0,{_tmp1AB,_tmp1AB,_tmp1AB + 1U},(void*)& Cyc_opt46_spec,{_tmp1AC,_tmp1AC,_tmp1AC + 41U}};struct Cyc_List_List Cyc_opt46={(void*)& Cyc_opt46_tuple,0};
# 1153
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt47_spec={3U,& Cyc_print_all_tvars_r};static char _tmp1AD[15U]="-printalltvars";static char _tmp1AE[1U]="";static char _tmp1AF[57U]="Print all type variables (even implicit default effects)";struct _tuple29 Cyc_opt47_tuple={{_tmp1AD,_tmp1AD,_tmp1AD + 15U},0,{_tmp1AE,_tmp1AE,_tmp1AE + 1U},(void*)& Cyc_opt47_spec,{_tmp1AF,_tmp1AF,_tmp1AF + 57U}};struct Cyc_List_List Cyc_opt47={(void*)& Cyc_opt47_tuple,0};
# 1156
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt48_spec={3U,& Cyc_print_all_kinds_r};static char _tmp1B0[15U]="-printallkinds";static char _tmp1B1[1U]="";static char _tmp1B2[37U]="Always print kinds of type variables";struct _tuple29 Cyc_opt48_tuple={{_tmp1B0,_tmp1B0,_tmp1B0 + 15U},0,{_tmp1B1,_tmp1B1,_tmp1B1 + 1U},(void*)& Cyc_opt48_spec,{_tmp1B2,_tmp1B2,_tmp1B2 + 37U}};struct Cyc_List_List Cyc_opt48={(void*)& Cyc_opt48_tuple,0};
# 1159
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt49_spec={3U,& Cyc_print_full_evars_r};static char _tmp1B3[16U]="-printfullevars";static char _tmp1B4[1U]="";static char _tmp1B5[50U]="Print full information for evars (type debugging)";struct _tuple29 Cyc_opt49_tuple={{_tmp1B3,_tmp1B3,_tmp1B3 + 16U},0,{_tmp1B4,_tmp1B4,_tmp1B4 + 1U},(void*)& Cyc_opt49_spec,{_tmp1B5,_tmp1B5,_tmp1B5 + 50U}};struct Cyc_List_List Cyc_opt49={(void*)& Cyc_opt49_tuple,0};
# 1162
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt50_spec={3U,& Cyc_print_all_effects_r};static char _tmp1B6[17U]="-printalleffects";static char _tmp1B7[1U]="";static char _tmp1B8[45U]="Print effects for functions (type debugging)";struct _tuple29 Cyc_opt50_tuple={{_tmp1B6,_tmp1B6,_tmp1B6 + 17U},0,{_tmp1B7,_tmp1B7,_tmp1B7 + 1U},(void*)& Cyc_opt50_spec,{_tmp1B8,_tmp1B8,_tmp1B8 + 45U}};struct Cyc_List_List Cyc_opt50={(void*)& Cyc_opt50_tuple,0};
# 1165
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt51_spec={3U,& Cyc_nocyc_setjmp_r};static char _tmp1B9[14U]="-nocyc_setjmp";static char _tmp1BA[1U]="";static char _tmp1BB[46U]="Do not use compiler special file cyc_setjmp.h";struct _tuple29 Cyc_opt51_tuple={{_tmp1B9,_tmp1B9,_tmp1B9 + 14U},0,{_tmp1BA,_tmp1BA,_tmp1BA + 1U},(void*)& Cyc_opt51_spec,{_tmp1BB,_tmp1BB,_tmp1BB + 46U}};struct Cyc_List_List Cyc_opt51={(void*)& Cyc_opt51_tuple,0};
# 1168
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt52_spec={3U,& Cyc_Lex_compile_for_boot_r};static char _tmp1BC[18U]="-compile-for-boot";static char _tmp1BD[1U]="";static char _tmp1BE[71U]="Compile using the special boot library instead of the standard library";struct _tuple29 Cyc_opt52_tuple={{_tmp1BC,_tmp1BC,_tmp1BC + 18U},0,{_tmp1BD,_tmp1BD,_tmp1BD + 1U},(void*)& Cyc_opt52_spec,{_tmp1BE,_tmp1BE,_tmp1BE + 71U}};struct Cyc_List_List Cyc_opt52={(void*)& Cyc_opt52_tuple,0};
# 1171
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt53_spec={5U,Cyc_set_cyc_include};static char _tmp1BF[4U]="-CI";static char _tmp1C0[8U]=" <file>";static char _tmp1C1[31U]="Set cyc_include.h to be <file>";struct _tuple29 Cyc_opt53_tuple={{_tmp1BF,_tmp1BF,_tmp1BF + 4U},0,{_tmp1C0,_tmp1C0,_tmp1C0 + 8U},(void*)& Cyc_opt53_spec,{_tmp1C1,_tmp1C1,_tmp1C1 + 31U}};struct Cyc_List_List Cyc_opt53={(void*)& Cyc_opt53_tuple,0};
# 1174
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt54_spec={3U,& Cyc_InsertChecks_warn_bounds_checks};static char _tmp1C2[18U]="-warnboundschecks";static char _tmp1C3[1U]="";static char _tmp1C4[44U]="Warn when bounds checks can't be eliminated";struct _tuple29 Cyc_opt54_tuple={{_tmp1C2,_tmp1C2,_tmp1C2 + 18U},0,{_tmp1C3,_tmp1C3,_tmp1C3 + 1U},(void*)& Cyc_opt54_spec,{_tmp1C4,_tmp1C4,_tmp1C4 + 44U}};struct Cyc_List_List Cyc_opt54={(void*)& Cyc_opt54_tuple,0};
# 1177
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt55_spec={3U,& Cyc_InsertChecks_warn_all_null_deref};static char _tmp1C5[16U]="-warnnullchecks";static char _tmp1C6[1U]="";static char _tmp1C7[45U]="Warn when any null check can't be eliminated";struct _tuple29 Cyc_opt55_tuple={{_tmp1C5,_tmp1C5,_tmp1C5 + 16U},0,{_tmp1C6,_tmp1C6,_tmp1C6 + 1U},(void*)& Cyc_opt55_spec,{_tmp1C7,_tmp1C7,_tmp1C7 + 45U}};struct Cyc_List_List Cyc_opt55={(void*)& Cyc_opt55_tuple,0};
# 1180
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt56_spec={3U,& Cyc_Tcutil_warn_alias_coerce};static char _tmp1C8[19U]="-warnaliascoercion";static char _tmp1C9[1U]="";static char _tmp1CA[41U]="Warn when any alias coercion is inserted";struct _tuple29 Cyc_opt56_tuple={{_tmp1C8,_tmp1C8,_tmp1C8 + 19U},0,{_tmp1C9,_tmp1C9,_tmp1C9 + 1U},(void*)& Cyc_opt56_spec,{_tmp1CA,_tmp1CA,_tmp1CA + 41U}};struct Cyc_List_List Cyc_opt56={(void*)& Cyc_opt56_tuple,0};
# 1183
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt57_spec={3U,& Cyc_Tcutil_warn_region_coerce};static char _tmp1CB[18U]="-warnregioncoerce";static char _tmp1CC[1U]="";static char _tmp1CD[42U]="Warn when any region coercion is inserted";struct _tuple29 Cyc_opt57_tuple={{_tmp1CB,_tmp1CB,_tmp1CB + 18U},0,{_tmp1CC,_tmp1CC,_tmp1CC + 1U},(void*)& Cyc_opt57_spec,{_tmp1CD,_tmp1CD,_tmp1CD + 42U}};struct Cyc_List_List Cyc_opt57={(void*)& Cyc_opt57_tuple,0};
# 1186
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt58_spec={3U,& Cyc_Parse_no_register};static char _tmp1CE[12U]="-noregister";static char _tmp1CF[1U]="";static char _tmp1D0[39U]="Treat register storage class as public";struct _tuple29 Cyc_opt58_tuple={{_tmp1CE,_tmp1CE,_tmp1CE + 12U},0,{_tmp1CF,_tmp1CF,_tmp1CF + 1U},(void*)& Cyc_opt58_spec,{_tmp1D0,_tmp1D0,_tmp1D0 + 39U}};struct Cyc_List_List Cyc_opt58={(void*)& Cyc_opt58_tuple,0};
# 1189
struct Cyc_Arg_Clear_spec_Arg_Spec_struct Cyc_opt59_spec={4U,& Cyc_Position_use_gcc_style_location};static char _tmp1D1[18U]="-detailedlocation";static char _tmp1D2[1U]="";static char _tmp1D3[58U]="Try to give more detailed location information for errors";struct _tuple29 Cyc_opt59_tuple={{_tmp1D1,_tmp1D1,_tmp1D1 + 18U},0,{_tmp1D2,_tmp1D2,_tmp1D2 + 1U},(void*)& Cyc_opt59_spec,{_tmp1D3,_tmp1D3,_tmp1D3 + 58U}};struct Cyc_List_List Cyc_opt59={(void*)& Cyc_opt59_tuple,0};
# 1192
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt60_spec={0U,Cyc_set_port_c_code};static char _tmp1D4[6U]="-port";static char _tmp1D5[1U]="";static char _tmp1D6[38U]="Suggest how to port C code to Cyclone";struct _tuple29 Cyc_opt60_tuple={{_tmp1D4,_tmp1D4,_tmp1D4 + 6U},0,{_tmp1D5,_tmp1D5,_tmp1D5 + 1U},(void*)& Cyc_opt60_spec,{_tmp1D6,_tmp1D6,_tmp1D6 + 38U}};struct Cyc_List_List Cyc_opt60={(void*)& Cyc_opt60_tuple,0};
# 1195
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt61_spec={3U,& Cyc_Absyn_no_regions};static char _tmp1D7[11U]="-noregions";static char _tmp1D8[1U]="";static char _tmp1D9[39U]="Generate code that doesn't use regions";struct _tuple29 Cyc_opt61_tuple={{_tmp1D7,_tmp1D7,_tmp1D7 + 11U},0,{_tmp1D8,_tmp1D8,_tmp1D8 + 1U},(void*)& Cyc_opt61_spec,{_tmp1D9,_tmp1D9,_tmp1D9 + 39U}};struct Cyc_List_List Cyc_opt61={(void*)& Cyc_opt61_tuple,0};
# 1198
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt62_spec={5U,Cyc_add_iprefix};static char _tmp1DA[9U]="-iprefix";static char _tmp1DB[9U]="<prefix>";static char _tmp1DC[67U]="Specify <prefix> as the prefix for subsequent -iwithprefix options";struct _tuple29 Cyc_opt62_tuple={{_tmp1DA,_tmp1DA,_tmp1DA + 9U},0,{_tmp1DB,_tmp1DB,_tmp1DB + 9U},(void*)& Cyc_opt62_spec,{_tmp1DC,_tmp1DC,_tmp1DC + 67U}};struct Cyc_List_List Cyc_opt62={(void*)& Cyc_opt62_tuple,0};
# 1201
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt63_spec={5U,Cyc_add_iwithprefix};static char _tmp1DD[13U]="-iwithprefix";static char _tmp1DE[6U]="<dir>";static char _tmp1DF[47U]="Add <prefix>/<dir> to the second include path.";struct _tuple29 Cyc_opt63_tuple={{_tmp1DD,_tmp1DD,_tmp1DD + 13U},0,{_tmp1DE,_tmp1DE,_tmp1DE + 6U},(void*)& Cyc_opt63_spec,{_tmp1DF,_tmp1DF,_tmp1DF + 47U}};struct Cyc_List_List Cyc_opt63={(void*)& Cyc_opt63_tuple,0};
# 1204
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt64_spec={5U,Cyc_add_iwithprefixbefore};static char _tmp1E0[19U]="-iwithprefixbefore";static char _tmp1E1[6U]="<dir>";static char _tmp1E2[45U]="Add <prefix>/<dir> to the main include path.";struct _tuple29 Cyc_opt64_tuple={{_tmp1E0,_tmp1E0,_tmp1E0 + 19U},0,{_tmp1E1,_tmp1E1,_tmp1E1 + 6U},(void*)& Cyc_opt64_spec,{_tmp1E2,_tmp1E2,_tmp1E2 + 45U}};struct Cyc_List_List Cyc_opt64={(void*)& Cyc_opt64_tuple,0};
# 1207
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt65_spec={5U,Cyc_add_isystem};static char _tmp1E3[9U]="-isystem";static char _tmp1E4[6U]="<dir>";static char _tmp1E5[90U]="Add <dir> to the beginning of the second include path and treat it as a\nsystem directory.";struct _tuple29 Cyc_opt65_tuple={{_tmp1E3,_tmp1E3,_tmp1E3 + 9U},0,{_tmp1E4,_tmp1E4,_tmp1E4 + 6U},(void*)& Cyc_opt65_spec,{_tmp1E5,_tmp1E5,_tmp1E5 + 90U}};struct Cyc_List_List Cyc_opt65={(void*)& Cyc_opt65_tuple,0};
# 1210
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt66_spec={5U,Cyc_add_idirafter};static char _tmp1E6[11U]="-idirafter";static char _tmp1E7[6U]="<dir>";static char _tmp1E8[46U]="Add the directory to the second include path.";struct _tuple29 Cyc_opt66_tuple={{_tmp1E6,_tmp1E6,_tmp1E6 + 11U},0,{_tmp1E7,_tmp1E7,_tmp1E7 + 6U},(void*)& Cyc_opt66_spec,{_tmp1E8,_tmp1E8,_tmp1E8 + 46U}};struct Cyc_List_List Cyc_opt66={(void*)& Cyc_opt66_tuple,0};
# 1213
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt67_spec={3U,& Cyc_generate_interface_r};static char _tmp1E9[15U]="--geninterface";static char _tmp1EA[1U]="";static char _tmp1EB[25U]="Generate interface files";struct _tuple29 Cyc_opt67_tuple={{_tmp1E9,_tmp1E9,_tmp1E9 + 15U},0,{_tmp1EA,_tmp1EA,_tmp1EA + 1U},(void*)& Cyc_opt67_spec,{_tmp1EB,_tmp1EB,_tmp1EB + 25U}};struct Cyc_List_List Cyc_opt67={(void*)& Cyc_opt67_tuple,0};
# 1216
struct Cyc_Arg_String_spec_Arg_Spec_struct Cyc_opt68_spec={5U,Cyc_set_specified_interface};static char _tmp1EC[12U]="--interface";static char _tmp1ED[8U]=" <file>";static char _tmp1EE[37U]="Set the interface file to be <file>.";struct _tuple29 Cyc_opt68_tuple={{_tmp1EC,_tmp1EC,_tmp1EC + 12U},0,{_tmp1ED,_tmp1ED,_tmp1ED + 8U},(void*)& Cyc_opt68_spec,{_tmp1EE,_tmp1EE,_tmp1EE + 37U}};struct Cyc_List_List Cyc_opt68={(void*)& Cyc_opt68_tuple,0};
# 1219
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt69_spec={0U,Cyc_set_many_errors};static char _tmp1EF[13U]="--manyerrors";static char _tmp1F0[1U]="";static char _tmp1F1[36U]="don't stop after only a few errors.";struct _tuple29 Cyc_opt69_tuple={{_tmp1EF,_tmp1EF,_tmp1EF + 13U},0,{_tmp1F0,_tmp1F0,_tmp1F0 + 1U},(void*)& Cyc_opt69_spec,{_tmp1F1,_tmp1F1,_tmp1F1 + 36U}};struct Cyc_List_List Cyc_opt69={(void*)& Cyc_opt69_tuple,0};
# 1222
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt70_spec={3U,& Cyc_ParseErrors_print_state_and_token};static char _tmp1F2[13U]="--parsestate";static char _tmp1F3[1U]="";static char _tmp1F4[50U]="print the parse state and token on a syntax error";struct _tuple29 Cyc_opt70_tuple={{_tmp1F2,_tmp1F2,_tmp1F2 + 13U},0,{_tmp1F3,_tmp1F3,_tmp1F3 + 1U},(void*)& Cyc_opt70_spec,{_tmp1F4,_tmp1F4,_tmp1F4 + 50U}};struct Cyc_List_List Cyc_opt70={(void*)& Cyc_opt70_tuple,0};
# 1225
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt71_spec={0U,Cyc_noassume_gcc_flag};static char _tmp1F5[22U]="-known-gcc-flags-only";static char _tmp1F6[1U]="";static char _tmp1F7[57U]="do not assume that unknown flags should be passed to gcc";struct _tuple29 Cyc_opt71_tuple={{_tmp1F5,_tmp1F5,_tmp1F5 + 22U},0,{_tmp1F6,_tmp1F6,_tmp1F6 + 1U},(void*)& Cyc_opt71_spec,{_tmp1F7,_tmp1F7,_tmp1F7 + 57U}};struct Cyc_List_List Cyc_opt71={(void*)& Cyc_opt71_tuple,0};
# 1228
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt72_spec={0U,Cyc_print_options};static char _tmp1F8[6U]="-help";static char _tmp1F9[1U]="";static char _tmp1FA[32U]="print out the available options";struct _tuple29 Cyc_opt72_tuple={{_tmp1F8,_tmp1F8,_tmp1F8 + 6U},0,{_tmp1F9,_tmp1F9,_tmp1F9 + 1U},(void*)& Cyc_opt72_spec,{_tmp1FA,_tmp1FA,_tmp1FA + 32U}};struct Cyc_List_List Cyc_opt72={(void*)& Cyc_opt72_tuple,0};
# 1231
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt73_spec={0U,Cyc_print_options};static char _tmp1FB[7U]="-usage";static char _tmp1FC[1U]="";static char _tmp1FD[32U]="print out the available options";struct _tuple29 Cyc_opt73_tuple={{_tmp1FB,_tmp1FB,_tmp1FB + 7U},0,{_tmp1FC,_tmp1FC,_tmp1FC + 1U},(void*)& Cyc_opt73_spec,{_tmp1FD,_tmp1FD,_tmp1FD + 32U}};struct Cyc_List_List Cyc_opt73={(void*)& Cyc_opt73_tuple,0};
# 1234
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt74_spec={0U,Cyc_set_notoseqc};static char _tmp1FE[10U]="-no-seq-c";static char _tmp1FF[1U]="";static char _tmp200[71U]="Do not force left-to-right evaluation order of generated code (unsafe)";struct _tuple29 Cyc_opt74_tuple={{_tmp1FE,_tmp1FE,_tmp1FE + 10U},0,{_tmp1FF,_tmp1FF,_tmp1FF + 1U},(void*)& Cyc_opt74_spec,{_tmp200,_tmp200,_tmp200 + 71U}};struct Cyc_List_List Cyc_opt74={(void*)& Cyc_opt74_tuple,0};
# 1237
struct Cyc_Arg_Unit_spec_Arg_Spec_struct Cyc_opt75_spec={0U,Cyc_set_pg};static char _tmp201[4U]="-pg";static char _tmp202[1U]="";static char _tmp203[33U]="Compile for profiling with gprof";struct _tuple29 Cyc_opt75_tuple={{_tmp201,_tmp201,_tmp201 + 4U},0,{_tmp202,_tmp202,_tmp202 + 1U},(void*)& Cyc_opt75_spec,{_tmp203,_tmp203,_tmp203 + 33U}};struct Cyc_List_List Cyc_opt75={(void*)& Cyc_opt75_tuple,0};
# 1240
struct Cyc_Arg_Set_spec_Arg_Spec_struct Cyc_opt76_spec={3U,& Cyc_vcgen_r};static char _tmp204[8U]="--vcgen";static char _tmp205[1U]="";static char _tmp206[32U]="compute verification-conditions";struct _tuple29 Cyc_opt76_tuple={{_tmp204,_tmp204,_tmp204 + 8U},0,{_tmp205,_tmp205,_tmp205 + 1U},(void*)& Cyc_opt76_spec,{_tmp206,_tmp206,_tmp206 + 32U}};struct Cyc_List_List Cyc_opt76={(void*)& Cyc_opt76_tuple,0};
# 1244
struct Cyc_List_List*Cyc_global_options[76U]={& Cyc_opt1,& Cyc_opt2,& Cyc_opt3,& Cyc_opt4,& Cyc_opt5,& Cyc_opt6,& Cyc_opt7,& Cyc_opt8,& Cyc_opt9,& Cyc_opt10,& Cyc_opt11,& Cyc_opt12,& Cyc_opt13,& Cyc_opt14,& Cyc_opt15,& Cyc_opt16,& Cyc_opt17,& Cyc_opt18,& Cyc_opt19,& Cyc_opt20,& Cyc_opt21,& Cyc_opt22,& Cyc_opt23,& Cyc_opt24,& Cyc_opt25,& Cyc_opt26,& Cyc_opt27,& Cyc_opt28,& Cyc_opt29,& Cyc_opt30,& Cyc_opt31,& Cyc_opt32,& Cyc_opt33,& Cyc_opt34,& Cyc_opt35,& Cyc_opt36,& Cyc_opt37,& Cyc_opt38,& Cyc_opt39,& Cyc_opt40,& Cyc_opt41,& Cyc_opt42,& Cyc_opt43,& Cyc_opt44,& Cyc_opt45,& Cyc_opt46,& Cyc_opt47,& Cyc_opt48,& Cyc_opt49,& Cyc_opt50,& Cyc_opt51,& Cyc_opt52,& Cyc_opt53,& Cyc_opt54,& Cyc_opt55,& Cyc_opt56,& Cyc_opt57,& Cyc_opt58,& Cyc_opt59,& Cyc_opt60,& Cyc_opt61,& Cyc_opt62,& Cyc_opt63,& Cyc_opt64,& Cyc_opt65,& Cyc_opt66,& Cyc_opt67,& Cyc_opt68,& Cyc_opt69,& Cyc_opt70,& Cyc_opt71,& Cyc_opt72,& Cyc_opt73,& Cyc_opt74,& Cyc_opt75,& Cyc_opt76};
# 1257
void Cyc_print_options (void){
({struct Cyc_List_List*_tmp350=Cyc_global_options[0];Cyc_Arg_usage(_tmp350,({const char*_tmp207="<program.cyc>";_tag_fat(_tmp207,sizeof(char),14U);}));});}
# 1262
int Cyc_main(int argc,struct _fat_ptr argv){
# 1266
GC_blacklist_warn_clear();{
# 1269
struct _fat_ptr optstring=({const char*_tmp288="Options:";_tag_fat(_tmp288,sizeof(char),9U);});
# 1271
{int i=1;for(0;(unsigned)i < 76U;++ i){
(*((struct Cyc_List_List**)_check_known_subscript_notnull(Cyc_global_options,76U,sizeof(struct Cyc_List_List*),i - 1)))->tl=Cyc_global_options[i];}}{
# 1274
struct Cyc_List_List*options=Cyc_global_options[0];
# 1276
struct _fat_ptr _tmp208=Cyc_Specsfile_parse_b(options,Cyc_add_other,Cyc_add_other_flag,optstring,argv);struct _fat_ptr otherargs=_tmp208;
# 1282
struct _fat_ptr _tmp209=Cyc_find_in_arch_path(({const char*_tmp287="cycspecs";_tag_fat(_tmp287,sizeof(char),9U);}));struct _fat_ptr specs_file=_tmp209;
if(Cyc_v_r)({struct Cyc_String_pa_PrintArg_struct _tmp20C=({struct Cyc_String_pa_PrintArg_struct _tmp2A8;_tmp2A8.tag=0U,_tmp2A8.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp2A8;});void*_tmp20A[1U];_tmp20A[0]=& _tmp20C;({struct Cyc___cycFILE*_tmp352=Cyc_stderr;struct _fat_ptr _tmp351=({const char*_tmp20B="Reading from specs file %s\n";_tag_fat(_tmp20B,sizeof(char),28U);});Cyc_fprintf(_tmp352,_tmp351,_tag_fat(_tmp20A,sizeof(void*),1U));});});{
struct Cyc_List_List*_tmp20D=Cyc_Specsfile_read_specs(specs_file);struct Cyc_List_List*specs=_tmp20D;
# 1286
struct _fat_ptr _tmp20E=Cyc_Specsfile_split_specs(({struct Cyc_List_List*_tmp353=specs;Cyc_Specsfile_get_spec(_tmp353,({const char*_tmp286="cyclone";_tag_fat(_tmp286,sizeof(char),8U);}));}));struct _fat_ptr args_from_specs_file=_tmp20E;
if(({struct _fat_ptr*_tmp354=(struct _fat_ptr*)args_from_specs_file.curr;_tmp354 != (struct _fat_ptr*)(_tag_fat(0,0,0)).curr;})){
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,args_from_specs_file);}{
# 1291
struct _fat_ptr _tmp20F=({struct Cyc_List_List*_tmp355=specs;Cyc_Specsfile_get_spec(_tmp355,({const char*_tmp285="cyclone_target_cflags";_tag_fat(_tmp285,sizeof(char),22U);}));});struct _fat_ptr target_cflags=_tmp20F;
struct _fat_ptr _tmp210=({struct Cyc_List_List*_tmp356=specs;Cyc_Specsfile_get_spec(_tmp356,({const char*_tmp284="cyclone_cc";_tag_fat(_tmp284,sizeof(char),11U);}));});struct _fat_ptr cyclone_cc=_tmp210;
if(!((unsigned)cyclone_cc.curr))cyclone_cc=({const char*_tmp211="gcc";_tag_fat(_tmp211,sizeof(char),4U);});
Cyc_def_inc_path=({struct Cyc_List_List*_tmp357=specs;Cyc_Specsfile_get_spec(_tmp357,({const char*_tmp212="cyclone_inc_path";_tag_fat(_tmp212,sizeof(char),17U);}));});
# 1297
Cyc_Arg_current=0;
Cyc_Arg_parse(options,Cyc_add_other,Cyc_add_other_flag,optstring,otherargs);
if(({struct _fat_ptr _tmp358=(struct _fat_ptr)Cyc_cpp;Cyc_strcmp(_tmp358,({const char*_tmp213="";_tag_fat(_tmp213,sizeof(char),1U);}));})== 0){
# 1315 "cyclone.cyc"
const char*_tmp214=Cyc_produce_dependencies?"":" -E";const char*dash_E=_tmp214;
Cyc_set_cpp((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp217=({struct Cyc_String_pa_PrintArg_struct _tmp2AC;_tmp2AC.tag=0U,_tmp2AC.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_tmp2AC;});struct Cyc_String_pa_PrintArg_struct _tmp218=({struct Cyc_String_pa_PrintArg_struct _tmp2AB;_tmp2AB.tag=0U,_tmp2AB.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_tmp2AB;});struct Cyc_String_pa_PrintArg_struct _tmp219=({struct Cyc_String_pa_PrintArg_struct _tmp2AA;_tmp2AA.tag=0U,({
# 1318
struct _fat_ptr _tmp359=(struct _fat_ptr)({const char*_tmp21B=dash_E;_tag_fat(_tmp21B,sizeof(char),_get_zero_arr_size_char((void*)_tmp21B,1U));});_tmp2AA.f1=_tmp359;});_tmp2AA;});struct Cyc_String_pa_PrintArg_struct _tmp21A=({struct Cyc_String_pa_PrintArg_struct _tmp2A9;_tmp2A9.tag=0U,_tmp2A9.f1=(struct _fat_ptr)((struct _fat_ptr)specs_file);_tmp2A9;});void*_tmp215[4U];_tmp215[0]=& _tmp217,_tmp215[1]=& _tmp218,_tmp215[2]=& _tmp219,_tmp215[3]=& _tmp21A;({struct _fat_ptr _tmp35A=({const char*_tmp216="%s %s -w -x c%s -specs %s";_tag_fat(_tmp216,sizeof(char),26U);});Cyc_aprintf(_tmp35A,_tag_fat(_tmp215,sizeof(void*),4U));});}));}
# 1320
if(Cyc_cyclone_files == 0 && Cyc_ccargs == 0){
({void*_tmp21C=0U;({struct Cyc___cycFILE*_tmp35C=Cyc_stderr;struct _fat_ptr _tmp35B=({const char*_tmp21D="missing file\n";_tag_fat(_tmp21D,sizeof(char),14U);});Cyc_fprintf(_tmp35C,_tmp35B,_tag_fat(_tmp21C,sizeof(void*),0U));});});
exit(1);}
# 1329
if(!Cyc_stop_after_cpp_r && !Cyc_nocyc_setjmp_r)
Cyc_cyc_setjmp=Cyc_find_in_arch_path(({const char*_tmp21E="cyc_setjmp.h";_tag_fat(_tmp21E,sizeof(char),13U);}));
if(!Cyc_stop_after_cpp_r && Cyc_strlen((struct _fat_ptr)Cyc_cyc_include)== (unsigned long)0)
Cyc_cyc_include=Cyc_find_in_exec_path(({const char*_tmp21F="cyc_include.h";_tag_fat(_tmp21F,sizeof(char),14U);}));
# 1335
{struct _handler_cons _tmp220;_push_handler(& _tmp220);{int _tmp222=0;if(setjmp(_tmp220.handler))_tmp222=1;if(!_tmp222){
{struct Cyc_List_List*_tmp223=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_cyclone_files);struct Cyc_List_List*l=_tmp223;for(0;l != 0;l=l->tl){
Cyc_process_file(*((struct _fat_ptr*)l->hd));
if(Cyc_compile_failure){int _tmp224=1;_npop_handler(0U);return _tmp224;}}}
# 1336
;_pop_handler();}else{void*_tmp221=(void*)Cyc_Core_get_exn_thrown();void*_tmp225=_tmp221;void*_tmp226;_LL1: _tmp226=_tmp225;_LL2: {void*x=_tmp226;
# 1342
if(Cyc_compile_failure)return 1;else{
Cyc_Core_rethrow(x);}}_LL0:;}}}
# 1346
if(((Cyc_stop_after_cpp_r || Cyc_parseonly_r)|| Cyc_tc_r)|| Cyc_toc_r)return 0;
# 1352
if(Cyc_ccargs == 0)return 0;
# 1355
if((unsigned)Cyc_Specsfile_target_arch.curr &&(unsigned)Cyc_Specsfile_cyclone_exec_path)
Cyc_add_ccarg((struct _fat_ptr)({struct Cyc_List_List*_tmp360=({struct Cyc_List_List*_tmp229=_cycalloc(sizeof(*_tmp229));({struct _fat_ptr*_tmp35F=({struct _fat_ptr*_tmp228=_cycalloc(sizeof(*_tmp228));({struct _fat_ptr _tmp35E=({const char*_tmp227="";_tag_fat(_tmp227,sizeof(char),1U);});*_tmp228=_tmp35E;});_tmp228;});_tmp229->hd=_tmp35F;}),({
struct Cyc_List_List*_tmp35D=Cyc_add_subdir(Cyc_Specsfile_cyclone_exec_path,Cyc_Specsfile_target_arch);_tmp229->tl=_tmp35D;});_tmp229;});
# 1356
Cyc_str_sepstr(_tmp360,({const char*_tmp22A=" -L";_tag_fat(_tmp22A,sizeof(char),4U);}));}));
# 1359
Cyc_add_ccarg((struct _fat_ptr)({struct _fat_ptr _tmp361=({const char*_tmp22B="-L";_tag_fat(_tmp22B,sizeof(char),3U);});Cyc_strconcat(_tmp361,(struct _fat_ptr)Cyc_Specsfile_def_lib_path);}));
# 1361
Cyc_ccargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_ccargs);{
struct _fat_ptr _tmp22C=({struct Cyc_List_List*_tmp362=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_ccargs);Cyc_str_sepstr(_tmp362,({const char*_tmp283=" ";_tag_fat(_tmp283,sizeof(char),2U);}));});struct _fat_ptr ccargs_string=_tmp22C;
Cyc_libargs=((struct Cyc_List_List*(*)(struct Cyc_List_List*x))Cyc_List_rev)(Cyc_libargs);{
struct _fat_ptr _tmp22D=({struct Cyc_List_List*_tmp366=({struct Cyc_List_List*_tmp281=_cycalloc(sizeof(*_tmp281));({struct _fat_ptr*_tmp365=({struct _fat_ptr*_tmp280=_cycalloc(sizeof(*_tmp280));({struct _fat_ptr _tmp364=({const char*_tmp27F="";_tag_fat(_tmp27F,sizeof(char),1U);});*_tmp280=_tmp364;});_tmp280;});_tmp281->hd=_tmp365;}),({
struct Cyc_List_List*_tmp363=((struct Cyc_List_List*(*)(struct _fat_ptr*(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_map)(Cyc_sh_escape_stringptr,Cyc_libargs);_tmp281->tl=_tmp363;});_tmp281;});
# 1364
Cyc_str_sepstr(_tmp366,({const char*_tmp282=" ";_tag_fat(_tmp282,sizeof(char),2U);}));});struct _fat_ptr libargs_string=_tmp22D;
# 1367
struct Cyc_List_List*stdlib;
struct _fat_ptr stdlib_string;
int _tmp22E=
((Cyc_stop_after_asmfile_r || Cyc_stop_after_objectfile_r)||
# 1372
 Cyc_output_file != 0 &&({struct _fat_ptr _tmp368=*((struct _fat_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp368,({const char*_tmp27D=".a";_tag_fat(_tmp27D,sizeof(char),3U);}));}))||
 Cyc_output_file != 0 &&({struct _fat_ptr _tmp367=*((struct _fat_ptr*)_check_null(Cyc_output_file));Cyc_Filename_check_suffix(_tmp367,({const char*_tmp27E=".lib";_tag_fat(_tmp27E,sizeof(char),5U);}));});
# 1369
int is_not_executable=_tmp22E;
# 1374
if(is_not_executable){
stdlib=0;
stdlib_string=(struct _fat_ptr)({const char*_tmp22F="";_tag_fat(_tmp22F,sizeof(char),1U);});}else{
# 1379
struct _fat_ptr libcyc_flag;
struct _fat_ptr nogc_filename;
struct _fat_ptr runtime_filename;
struct _fat_ptr gc_filename;
if(Cyc_pa_r){
libcyc_flag=({const char*_tmp230="-lcyc_a";_tag_fat(_tmp230,sizeof(char),8U);});
nogc_filename=({const char*_tmp231="nogc_a.a";_tag_fat(_tmp231,sizeof(char),9U);});
runtime_filename=({const char*_tmp232="runtime_cyc_a.a";_tag_fat(_tmp232,sizeof(char),16U);});}else{
if(Cyc_nocheck_r){
libcyc_flag=({const char*_tmp233="-lcyc_nocheck";_tag_fat(_tmp233,sizeof(char),14U);});
nogc_filename=({const char*_tmp234="nogc.a";_tag_fat(_tmp234,sizeof(char),7U);});
runtime_filename=({const char*_tmp235="runtime_cyc.a";_tag_fat(_tmp235,sizeof(char),14U);});}else{
if(Cyc_pg_r){
libcyc_flag=({const char*_tmp236="-lcyc_pg";_tag_fat(_tmp236,sizeof(char),9U);});
runtime_filename=({const char*_tmp237="runtime_cyc_pg.a";_tag_fat(_tmp237,sizeof(char),17U);});
nogc_filename=({const char*_tmp238="nogc_pg.a";_tag_fat(_tmp238,sizeof(char),10U);});}else{
if(Cyc_Lex_compile_for_boot_r){
# 1397
libcyc_flag=({const char*_tmp239="-lcycboot";_tag_fat(_tmp239,sizeof(char),10U);});
nogc_filename=({const char*_tmp23A="nogc.a";_tag_fat(_tmp23A,sizeof(char),7U);});
runtime_filename=({const char*_tmp23B="runtime_cyc.a";_tag_fat(_tmp23B,sizeof(char),14U);});}else{
if(Cyc_pthread_r){
libcyc_flag=({const char*_tmp23C="-lcyc";_tag_fat(_tmp23C,sizeof(char),6U);});
nogc_filename=({const char*_tmp23D="nogc_pthread.a";_tag_fat(_tmp23D,sizeof(char),15U);});
runtime_filename=({const char*_tmp23E="runtime_cyc_pthread.a";_tag_fat(_tmp23E,sizeof(char),22U);});}else{
# 1405
libcyc_flag=({const char*_tmp23F="-lcyc";_tag_fat(_tmp23F,sizeof(char),6U);});
nogc_filename=({const char*_tmp240="nogc.a";_tag_fat(_tmp240,sizeof(char),7U);});
runtime_filename=({const char*_tmp241="runtime_cyc.a";_tag_fat(_tmp241,sizeof(char),14U);});}}}}}
# 1409
if(!Cyc_pthread_r)
gc_filename=({const char*_tmp242="gc.a";_tag_fat(_tmp242,sizeof(char),5U);});else{
# 1412
gc_filename=({const char*_tmp243="gc_pthread.a";_tag_fat(_tmp243,sizeof(char),13U);});}{
# 1414
struct _fat_ptr _tmp244=Cyc_nogc_r?Cyc_find_in_arch_path(nogc_filename): Cyc_find_in_arch_path(gc_filename);struct _fat_ptr gc=_tmp244;
struct _fat_ptr _tmp245=Cyc_find_in_arch_path(runtime_filename);struct _fat_ptr runtime=_tmp245;
# 1418
stdlib=0;
stdlib_string=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp248=({struct Cyc_String_pa_PrintArg_struct _tmp2AF;_tmp2AF.tag=0U,_tmp2AF.f1=(struct _fat_ptr)((struct _fat_ptr)libcyc_flag);_tmp2AF;});struct Cyc_String_pa_PrintArg_struct _tmp249=({struct Cyc_String_pa_PrintArg_struct _tmp2AE;_tmp2AE.tag=0U,_tmp2AE.f1=(struct _fat_ptr)((struct _fat_ptr)runtime);_tmp2AE;});struct Cyc_String_pa_PrintArg_struct _tmp24A=({struct Cyc_String_pa_PrintArg_struct _tmp2AD;_tmp2AD.tag=0U,_tmp2AD.f1=(struct _fat_ptr)((struct _fat_ptr)gc);_tmp2AD;});void*_tmp246[3U];_tmp246[0]=& _tmp248,_tmp246[1]=& _tmp249,_tmp246[2]=& _tmp24A;({struct _fat_ptr _tmp369=({const char*_tmp247=" %s %s %s ";_tag_fat(_tmp247,sizeof(char),11U);});Cyc_aprintf(_tmp369,_tag_fat(_tmp246,sizeof(void*),3U));});});}}
# 1423
if(Cyc_ic_r){struct _handler_cons _tmp24B;_push_handler(& _tmp24B);{int _tmp24D=0;if(setjmp(_tmp24B.handler))_tmp24D=1;if(!_tmp24D){
Cyc_ccargs=((struct Cyc_List_List*(*)(int(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_ccargs);
Cyc_libargs=((struct Cyc_List_List*(*)(int(*f)(struct _fat_ptr*),struct Cyc_List_List*x))Cyc_List_filter)(Cyc_is_cfile,Cyc_libargs);{
struct Cyc_List_List*_tmp24E=({struct Cyc_List_List*_tmp36A=stdlib;((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(_tmp36A,((struct Cyc_List_List*(*)(struct Cyc_List_List*x,struct Cyc_List_List*y))Cyc_List_append)(Cyc_ccargs,Cyc_libargs));});struct Cyc_List_List*lf=_tmp24E;
if(!is_not_executable)
lf=({struct Cyc_List_List*_tmp24F=_cycalloc(sizeof(*_tmp24F));_tmp24F->hd=Cyc_final_strptr,_tmp24F->tl=lf;_tmp24F;});{
# 1431
struct Cyc_Interface_I*_tmp250=((struct Cyc_Interface_I*(*)(struct Cyc_Interface_I*(*get)(struct _fat_ptr*),struct Cyc_List_List*la,struct Cyc_List_List*linfo))Cyc_Interface_get_and_merge_list)(Cyc_read_cycio,lf,lf);struct Cyc_Interface_I*i=_tmp250;
if(i == 0){
({void*_tmp251=0U;({struct Cyc___cycFILE*_tmp36C=Cyc_stderr;struct _fat_ptr _tmp36B=({const char*_tmp252="Error: interfaces incompatible\n";_tag_fat(_tmp252,sizeof(char),32U);});Cyc_fprintf(_tmp36C,_tmp36B,_tag_fat(_tmp251,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp253=1;_npop_handler(0U);return _tmp253;}}
# 1438
if(is_not_executable){
if(Cyc_output_file != 0){
struct _fat_ptr _tmp254=({struct Cyc_String_pa_PrintArg_struct _tmp25B=({struct Cyc_String_pa_PrintArg_struct _tmp2B0;_tmp2B0.tag=0U,({struct _fat_ptr _tmp36D=(struct _fat_ptr)((struct _fat_ptr)Cyc_Filename_chop_extension(*((struct _fat_ptr*)_check_null(Cyc_output_file))));_tmp2B0.f1=_tmp36D;});_tmp2B0;});void*_tmp259[1U];_tmp259[0]=& _tmp25B;({struct _fat_ptr _tmp36E=({const char*_tmp25A="%s.cycio";_tag_fat(_tmp25A,sizeof(char),9U);});Cyc_aprintf(_tmp36E,_tag_fat(_tmp259,sizeof(void*),1U));});});struct _fat_ptr output_file_io=_tmp254;
struct Cyc___cycFILE*_tmp255=({struct _fat_ptr _tmp370=(struct _fat_ptr)output_file_io;struct _fat_ptr _tmp36F=({const char*_tmp257="wb";_tag_fat(_tmp257,sizeof(char),3U);});Cyc_try_file_open(_tmp370,_tmp36F,({const char*_tmp258="interface object file";_tag_fat(_tmp258,sizeof(char),22U);}));});struct Cyc___cycFILE*f=_tmp255;
if(f == 0){
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp256=1;_npop_handler(0U);return _tmp256;}}
# 1447
Cyc_Interface_save(i,f);
Cyc_file_close(f);}}else{
# 1451
if(!({struct Cyc_Interface_I*_tmp374=Cyc_Interface_empty();struct Cyc_Interface_I*_tmp373=i;Cyc_Interface_is_subinterface(_tmp374,_tmp373,({struct _tuple25*_tmp25E=_cycalloc(sizeof(*_tmp25E));
({struct _fat_ptr _tmp372=({const char*_tmp25C="empty interface";_tag_fat(_tmp25C,sizeof(char),16U);});_tmp25E->f1=_tmp372;}),({struct _fat_ptr _tmp371=({const char*_tmp25D="global interface";_tag_fat(_tmp25D,sizeof(char),17U);});_tmp25E->f2=_tmp371;});_tmp25E;}));})){
({void*_tmp25F=0U;({struct Cyc___cycFILE*_tmp376=Cyc_stderr;struct _fat_ptr _tmp375=({const char*_tmp260="Error: some objects are still undefined\n";_tag_fat(_tmp260,sizeof(char),41U);});Cyc_fprintf(_tmp376,_tmp375,_tag_fat(_tmp25F,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();{
int _tmp261=1;_npop_handler(0U);return _tmp261;}}}}}
# 1424
;_pop_handler();}else{void*_tmp24C=(void*)Cyc_Core_get_exn_thrown();void*_tmp262=_tmp24C;void*_tmp263;_LL6: _tmp263=_tmp262;_LL7: {void*e=_tmp263;
# 1461
({struct Cyc_String_pa_PrintArg_struct _tmp266=({struct Cyc_String_pa_PrintArg_struct _tmp2B3;_tmp2B3.tag=0U,({struct _fat_ptr _tmp377=(struct _fat_ptr)({const char*_tmp26A=Cyc_Core_get_exn_name(e);_tag_fat(_tmp26A,sizeof(char),_get_zero_arr_size_char((void*)_tmp26A,1U));});_tmp2B3.f1=_tmp377;});_tmp2B3;});struct Cyc_String_pa_PrintArg_struct _tmp267=({struct Cyc_String_pa_PrintArg_struct _tmp2B2;_tmp2B2.tag=0U,({struct _fat_ptr _tmp378=(struct _fat_ptr)({const char*_tmp269=Cyc_Core_get_exn_filename();_tag_fat(_tmp269,sizeof(char),_get_zero_arr_size_char((void*)_tmp269,1U));});_tmp2B2.f1=_tmp378;});_tmp2B2;});struct Cyc_Int_pa_PrintArg_struct _tmp268=({struct Cyc_Int_pa_PrintArg_struct _tmp2B1;_tmp2B1.tag=1U,({unsigned long _tmp379=(unsigned long)Cyc_Core_get_exn_lineno();_tmp2B1.f1=_tmp379;});_tmp2B1;});void*_tmp264[3U];_tmp264[0]=& _tmp266,_tmp264[1]=& _tmp267,_tmp264[2]=& _tmp268;({struct Cyc___cycFILE*_tmp37B=Cyc_stderr;struct _fat_ptr _tmp37A=({const char*_tmp265="INTERNAL COMPILER FAILURE:  exception %s from around %s:%d thrown.\n  Please send bug report to cyclone-bugs-l@lists.cs.cornell.edu";_tag_fat(_tmp265,sizeof(char),131U);});Cyc_fprintf(_tmp37B,_tmp37A,_tag_fat(_tmp264,sizeof(void*),3U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}_LL5:;}}}{
# 1467
struct _fat_ptr outfile_str=({const char*_tmp27C="";_tag_fat(_tmp27C,sizeof(char),1U);});
if(Cyc_output_file != 0)
outfile_str=(struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp26D=({struct Cyc_String_pa_PrintArg_struct _tmp2B4;_tmp2B4.tag=0U,({struct _fat_ptr _tmp37C=(struct _fat_ptr)((struct _fat_ptr)Cyc_sh_escape_string(*((struct _fat_ptr*)_check_null(Cyc_output_file))));_tmp2B4.f1=_tmp37C;});_tmp2B4;});void*_tmp26B[1U];_tmp26B[0]=& _tmp26D;({struct _fat_ptr _tmp37D=({const char*_tmp26C=" -o %s";_tag_fat(_tmp26C,sizeof(char),7U);});Cyc_aprintf(_tmp37D,_tag_fat(_tmp26B,sizeof(void*),1U));});});{
# 1471
struct _fat_ptr _tmp26E=({struct Cyc_String_pa_PrintArg_struct _tmp276=({struct Cyc_String_pa_PrintArg_struct _tmp2BB;_tmp2BB.tag=0U,_tmp2BB.f1=(struct _fat_ptr)((struct _fat_ptr)cyclone_cc);_tmp2BB;});struct Cyc_String_pa_PrintArg_struct _tmp277=({struct Cyc_String_pa_PrintArg_struct _tmp2BA;_tmp2BA.tag=0U,_tmp2BA.f1=(struct _fat_ptr)((struct _fat_ptr)target_cflags);_tmp2BA;});struct Cyc_String_pa_PrintArg_struct _tmp278=({struct Cyc_String_pa_PrintArg_struct _tmp2B9;_tmp2B9.tag=0U,_tmp2B9.f1=(struct _fat_ptr)((struct _fat_ptr)ccargs_string);_tmp2B9;});struct Cyc_String_pa_PrintArg_struct _tmp279=({struct Cyc_String_pa_PrintArg_struct _tmp2B8;_tmp2B8.tag=0U,_tmp2B8.f1=(struct _fat_ptr)((struct _fat_ptr)outfile_str);_tmp2B8;});struct Cyc_String_pa_PrintArg_struct _tmp27A=({struct Cyc_String_pa_PrintArg_struct _tmp2B7;_tmp2B7.tag=0U,_tmp2B7.f1=(struct _fat_ptr)((struct _fat_ptr)stdlib_string);_tmp2B7;});struct Cyc_String_pa_PrintArg_struct _tmp27B=({struct Cyc_String_pa_PrintArg_struct _tmp2B6;_tmp2B6.tag=0U,_tmp2B6.f1=(struct _fat_ptr)((struct _fat_ptr)libargs_string);_tmp2B6;});void*_tmp274[6U];_tmp274[0]=& _tmp276,_tmp274[1]=& _tmp277,_tmp274[2]=& _tmp278,_tmp274[3]=& _tmp279,_tmp274[4]=& _tmp27A,_tmp274[5]=& _tmp27B;({struct _fat_ptr _tmp37E=({const char*_tmp275="%s %s %s%s %s%s";_tag_fat(_tmp275,sizeof(char),16U);});Cyc_aprintf(_tmp37E,_tag_fat(_tmp274,sizeof(void*),6U));});});struct _fat_ptr cccmd=_tmp26E;
# 1476
if(Cyc_v_r){({struct Cyc_String_pa_PrintArg_struct _tmp271=({struct Cyc_String_pa_PrintArg_struct _tmp2B5;_tmp2B5.tag=0U,_tmp2B5.f1=(struct _fat_ptr)((struct _fat_ptr)cccmd);_tmp2B5;});void*_tmp26F[1U];_tmp26F[0]=& _tmp271;({struct Cyc___cycFILE*_tmp380=Cyc_stderr;struct _fat_ptr _tmp37F=({const char*_tmp270="%s\n";_tag_fat(_tmp270,sizeof(char),4U);});Cyc_fprintf(_tmp380,_tmp37F,_tag_fat(_tmp26F,sizeof(void*),1U));});});Cyc_fflush(Cyc_stderr);}
if(system((const char*)_check_null(_untag_fat_ptr(cccmd,sizeof(char),1U)))!= 0){
({void*_tmp272=0U;({struct Cyc___cycFILE*_tmp382=Cyc_stderr;struct _fat_ptr _tmp381=({const char*_tmp273="Error: C compiler failed\n";_tag_fat(_tmp273,sizeof(char),26U);});Cyc_fprintf(_tmp382,_tmp381,_tag_fat(_tmp272,sizeof(void*),0U));});});
Cyc_compile_failure=1;
Cyc_remove_cfiles();
return 1;}
# 1483
Cyc_remove_cfiles();
# 1485
return Cyc_compile_failure?1: 0;}}}}}}}}}
