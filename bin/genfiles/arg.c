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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 197 "cycboot.h"
extern int Cyc_sscanf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 300 "cycboot.h"
extern int isspace(int);struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr s1,struct _fat_ptr s2);
# 51
extern int Cyc_strncmp(struct _fat_ptr s1,struct _fat_ptr s2,unsigned long len);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Arg_Error[6U];struct Cyc_Arg_Error_exn_struct{char*tag;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr,struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
# 66 "arg.h"
void Cyc_Arg_usage(struct Cyc_List_List*,struct _fat_ptr);
# 69
extern int Cyc_Arg_current;struct Cyc_Buffer_t;
# 50 "buffer.h"
extern struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned n);
# 58
extern struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 83
extern void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _fat_ptr,int offset,int len);
# 92 "buffer.h"
extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _fat_ptr);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line);char Cyc_Arg_Bad[4U]="Bad";char Cyc_Arg_Error[6U]="Error";struct Cyc_Arg_Prefix_Arg_Flag_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Exact_Arg_Flag_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Unknown_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Missing_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Message_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Wrong_Arg_error_struct{int tag;struct _fat_ptr f1;struct _fat_ptr f2;struct _fat_ptr f3;};struct _tuple0{struct _fat_ptr f1;int f2;struct _fat_ptr f3;void*f4;struct _fat_ptr f5;};
# 68 "arg.cyc"
static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _fat_ptr x){
while(l != 0){
struct _fat_ptr _tmp0=(*((struct _tuple0*)l->hd)).f1;struct _fat_ptr flag=_tmp0;
unsigned long _tmp1=Cyc_strlen((struct _fat_ptr)flag);unsigned long len=_tmp1;
if(len > (unsigned long)0 &&(*((struct _tuple0*)l->hd)).f2){
if(Cyc_strncmp((struct _fat_ptr)x,(struct _fat_ptr)(*((struct _tuple0*)l->hd)).f1,len)== 0)
return(*((struct _tuple0*)l->hd)).f4;}else{
# 76
if(Cyc_strcmp((struct _fat_ptr)x,(struct _fat_ptr)(*((struct _tuple0*)l->hd)).f1)== 0)
return(*((struct _tuple0*)l->hd)).f4;}
l=l->tl;}
# 80
(int)_throw((void*)({struct Cyc_Core_Not_found_exn_struct*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->tag=Cyc_Core_Not_found;_tmp2;}));}
# 89
static struct _fat_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct _fat_ptr s){
if(({char*_tmp67=(char*)s.curr;_tmp67 == (char*)(_tag_fat(0,0,0)).curr;}))return _tag_fat(0,0,0);
if(howmuch < 0)howmuch=0;{
unsigned long _tmp3=Cyc_strlen((struct _fat_ptr)s);unsigned long len=_tmp3;
len <= _get_fat_size(s,sizeof(char))?0:({struct _fat_ptr _tmp68=({const char*_tmp4="len <= numelts(s)";_tag_fat(_tmp4,sizeof(char),18U);});((int(*)(struct _fat_ptr assertion,struct _fat_ptr file,unsigned line))Cyc___assert_fail)(_tmp68,({const char*_tmp5="arg.cyc";_tag_fat(_tmp5,sizeof(char),8U);}),93U);});
if((unsigned long)howmuch > len){
Cyc_Buffer_add_string(b,s);
return _tag_fat(0,0,0);}{
# 100
int i;
for(i=howmuch - 1;i >= 0 && !isspace((int)*((const char*)_check_fat_subscript(s,sizeof(char),i)));-- i){
;}
# 105
if(i < 0)
for(i=howmuch?howmuch - 1: 0;((unsigned long)i < len &&(int)((const char*)s.curr)[i])&& !isspace((int)((const char*)s.curr)[i]);++ i){
;}
# 111
Cyc_Buffer_add_substring(b,s,0,i);{
# 114
struct _fat_ptr whatsleft=_tag_fat(0,0,0);
# 116
for(0;((unsigned long)i < len &&(int)((const char*)s.curr)[i])&& isspace((int)((const char*)s.curr)[i]);++ i){
;}
if((unsigned long)i < len &&(int)((const char*)s.curr)[i])whatsleft=_fat_ptr_plus(s,sizeof(char),i);
return whatsleft;}}}}
# 127
void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _fat_ptr item,struct _fat_ptr desc){
# 129
if(({char*_tmp69=(char*)item.curr;_tmp69 != (char*)(_tag_fat(0,0,0)).curr;}))Cyc_Buffer_add_string(b,item);
if(({char*_tmp6A=(char*)desc.curr;_tmp6A == (char*)(_tag_fat(0,0,0)).curr;}))return;
if(indent < 0)indent=0;
if(margin < 0)margin=0;{
# 135
struct _fat_ptr indentstr=({unsigned _tmp11=(unsigned)(indent + 2)+ 1U;char*_tmp10=_cycalloc_atomic(_check_times(_tmp11,sizeof(char)));({{unsigned _tmp59=(unsigned)(indent + 2);unsigned i;for(i=0;i < _tmp59;++ i){_tmp10[i]='\000';}_tmp10[_tmp59]=0;}0;});_tag_fat(_tmp10,sizeof(char),_tmp11);});
{unsigned i=0U;for(0;i < (unsigned)(indent + 1);++ i){
({struct _fat_ptr _tmp6=_fat_ptr_plus(indentstr,sizeof(char),(int)i);char _tmp7=*((char*)_check_fat_subscript(_tmp6,sizeof(char),0U));char _tmp8=i == (unsigned)0?'\n':' ';if(_get_fat_size(_tmp6,sizeof(char))== 1U &&(_tmp7 == 0 && _tmp8 != 0))_throw_arraybounds();*((char*)_tmp6.curr)=_tmp8;});}}{
unsigned long _tmp9=Cyc_strlen((struct _fat_ptr)item);unsigned long itemlen=_tmp9;
struct _fat_ptr itemsep;
if(Cyc_strlen((struct _fat_ptr)desc)> (unsigned long)0){
if(itemlen + (unsigned long)1 > (unsigned long)indent)
itemsep=indentstr;else{
# 144
struct _fat_ptr temp=({unsigned _tmpE=(((unsigned long)indent - itemlen)+ (unsigned long)1)+ 1U;char*_tmpD=_cycalloc_atomic(_check_times(_tmpE,sizeof(char)));({{unsigned _tmp58=((unsigned long)indent - itemlen)+ (unsigned long)1;unsigned i;for(i=0;i < _tmp58;++ i){_tmpD[i]='\000';}_tmpD[_tmp58]=0;}0;});_tag_fat(_tmpD,sizeof(char),_tmpE);});
{unsigned i=0U;for(0;i < (unsigned long)indent - itemlen;++ i){({struct _fat_ptr _tmpA=_fat_ptr_plus(temp,sizeof(char),(int)i);char _tmpB=*((char*)_check_fat_subscript(_tmpA,sizeof(char),0U));char _tmpC=' ';if(_get_fat_size(_tmpA,sizeof(char))== 1U &&(_tmpB == 0 && _tmpC != 0))_throw_arraybounds();*((char*)_tmpA.curr)=_tmpC;});}}
itemsep=temp;}}else{
# 149
return;}
# 151
Cyc_Buffer_add_string(b,(struct _fat_ptr)itemsep);
# 153
while(({char*_tmp6B=(char*)desc.curr;_tmp6B != (char*)(_tag_fat(0,0,0)).curr;})){
desc=Cyc_Arg_Justify_break_line(b,margin - indent,desc);
if(({char*_tmp6C=(char*)desc.curr;_tmp6C != (char*)(_tag_fat(0,0,0)).curr;}))
Cyc_Buffer_add_string(b,(struct _fat_ptr)indentstr);else{
# 158
({struct Cyc_Buffer_t*_tmp6D=b;Cyc_Buffer_add_string(_tmp6D,({const char*_tmpF="\n";_tag_fat(_tmpF,sizeof(char),2U);}));});}}
# 160
return;}}}
# 164
void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct _fat_ptr errmsg){
# 166
({struct Cyc_String_pa_PrintArg_struct _tmp14=({struct Cyc_String_pa_PrintArg_struct _tmp5A;_tmp5A.tag=0U,_tmp5A.f1=(struct _fat_ptr)((struct _fat_ptr)errmsg);_tmp5A;});void*_tmp12[1U];_tmp12[0]=& _tmp14;({struct Cyc___cycFILE*_tmp6F=Cyc_stderr;struct _fat_ptr _tmp6E=({const char*_tmp13="%s\n";_tag_fat(_tmp13,sizeof(char),4U);});Cyc_fprintf(_tmp6F,_tmp6E,_tag_fat(_tmp12,sizeof(void*),1U));});});{
struct Cyc_Buffer_t*_tmp15=Cyc_Buffer_create(1024U);struct Cyc_Buffer_t*b=_tmp15;
while(speclist != 0){
({struct Cyc_Buffer_t*_tmp71=b;struct _fat_ptr _tmp70=(struct _fat_ptr)
Cyc_strconcat((struct _fat_ptr)(*((struct _tuple0*)speclist->hd)).f1,(struct _fat_ptr)(*((struct _tuple0*)speclist->hd)).f3);
# 169
Cyc_Arg_Justify_justify_b(_tmp71,12,72,_tmp70,(*((struct _tuple0*)speclist->hd)).f5);});
# 172
speclist=speclist->tl;}
# 174
({struct Cyc_String_pa_PrintArg_struct _tmp18=({struct Cyc_String_pa_PrintArg_struct _tmp5B;_tmp5B.tag=0U,({struct _fat_ptr _tmp72=(struct _fat_ptr)((struct _fat_ptr)Cyc_Buffer_contents(b));_tmp5B.f1=_tmp72;});_tmp5B;});void*_tmp16[1U];_tmp16[0]=& _tmp18;({struct Cyc___cycFILE*_tmp74=Cyc_stderr;struct _fat_ptr _tmp73=({const char*_tmp17="%s";_tag_fat(_tmp17,sizeof(char),3U);});Cyc_fprintf(_tmp74,_tmp73,_tag_fat(_tmp16,sizeof(void*),1U));});});}}
# 177
int Cyc_Arg_current=0;
static struct _fat_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};
# 180
static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct _fat_ptr errmsg){
# 183
struct _fat_ptr progname=(unsigned)prog_pos < _get_fat_size(Cyc_Arg_args,sizeof(struct _fat_ptr))?*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),prog_pos)):({const char*_tmp33="(?)";_tag_fat(_tmp33,sizeof(char),4U);});
{void*_tmp19=e;struct _fat_ptr _tmp1A;struct _fat_ptr _tmp1D;struct _fat_ptr _tmp1C;struct _fat_ptr _tmp1B;struct _fat_ptr _tmp1E;struct _fat_ptr _tmp1F;switch(*((int*)_tmp19)){case 0U: _LL1: _tmp1F=((struct Cyc_Arg_Unknown_Arg_error_struct*)_tmp19)->f1;_LL2: {struct _fat_ptr s=_tmp1F;
# 186
if(({struct _fat_ptr _tmp75=(struct _fat_ptr)s;Cyc_strcmp(_tmp75,({const char*_tmp20="-help";_tag_fat(_tmp20,sizeof(char),6U);}));})!= 0)
({struct Cyc_String_pa_PrintArg_struct _tmp23=({struct Cyc_String_pa_PrintArg_struct _tmp5D;_tmp5D.tag=0U,_tmp5D.f1=(struct _fat_ptr)((struct _fat_ptr)progname);_tmp5D;});struct Cyc_String_pa_PrintArg_struct _tmp24=({struct Cyc_String_pa_PrintArg_struct _tmp5C;_tmp5C.tag=0U,_tmp5C.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5C;});void*_tmp21[2U];_tmp21[0]=& _tmp23,_tmp21[1]=& _tmp24;({struct Cyc___cycFILE*_tmp77=Cyc_stderr;struct _fat_ptr _tmp76=({const char*_tmp22="%s: unknown option `%s'.\n";_tag_fat(_tmp22,sizeof(char),26U);});Cyc_fprintf(_tmp77,_tmp76,_tag_fat(_tmp21,sizeof(void*),2U));});});
goto _LL0;}case 1U: _LL3: _tmp1E=((struct Cyc_Arg_Missing_Arg_error_struct*)_tmp19)->f1;_LL4: {struct _fat_ptr s=_tmp1E;
# 190
({struct Cyc_String_pa_PrintArg_struct _tmp27=({struct Cyc_String_pa_PrintArg_struct _tmp5F;_tmp5F.tag=0U,_tmp5F.f1=(struct _fat_ptr)((struct _fat_ptr)progname);_tmp5F;});struct Cyc_String_pa_PrintArg_struct _tmp28=({struct Cyc_String_pa_PrintArg_struct _tmp5E;_tmp5E.tag=0U,_tmp5E.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp5E;});void*_tmp25[2U];_tmp25[0]=& _tmp27,_tmp25[1]=& _tmp28;({struct Cyc___cycFILE*_tmp79=Cyc_stderr;struct _fat_ptr _tmp78=({const char*_tmp26="%s: option `%s' needs an argument.\n";_tag_fat(_tmp26,sizeof(char),36U);});Cyc_fprintf(_tmp79,_tmp78,_tag_fat(_tmp25,sizeof(void*),2U));});});
goto _LL0;}case 3U: _LL5: _tmp1B=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f1;_tmp1C=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f2;_tmp1D=((struct Cyc_Arg_Wrong_Arg_error_struct*)_tmp19)->f3;_LL6: {struct _fat_ptr flag=_tmp1B;struct _fat_ptr cmd=_tmp1C;struct _fat_ptr t=_tmp1D;
# 193
({struct Cyc_String_pa_PrintArg_struct _tmp2B=({struct Cyc_String_pa_PrintArg_struct _tmp63;_tmp63.tag=0U,_tmp63.f1=(struct _fat_ptr)((struct _fat_ptr)progname);_tmp63;});struct Cyc_String_pa_PrintArg_struct _tmp2C=({struct Cyc_String_pa_PrintArg_struct _tmp62;_tmp62.tag=0U,_tmp62.f1=(struct _fat_ptr)((struct _fat_ptr)cmd);_tmp62;});struct Cyc_String_pa_PrintArg_struct _tmp2D=({struct Cyc_String_pa_PrintArg_struct _tmp61;_tmp61.tag=0U,_tmp61.f1=(struct _fat_ptr)((struct _fat_ptr)flag);_tmp61;});struct Cyc_String_pa_PrintArg_struct _tmp2E=({struct Cyc_String_pa_PrintArg_struct _tmp60;_tmp60.tag=0U,_tmp60.f1=(struct _fat_ptr)((struct _fat_ptr)t);_tmp60;});void*_tmp29[4U];_tmp29[0]=& _tmp2B,_tmp29[1]=& _tmp2C,_tmp29[2]=& _tmp2D,_tmp29[3]=& _tmp2E;({struct Cyc___cycFILE*_tmp7B=Cyc_stderr;struct _fat_ptr _tmp7A=({const char*_tmp2A="%s: wrong argument `%s'; option `%s' expects %s.\n";_tag_fat(_tmp2A,sizeof(char),50U);});Cyc_fprintf(_tmp7B,_tmp7A,_tag_fat(_tmp29,sizeof(void*),4U));});});
# 195
goto _LL0;}default: _LL7: _tmp1A=((struct Cyc_Arg_Message_Arg_error_struct*)_tmp19)->f1;_LL8: {struct _fat_ptr s=_tmp1A;
# 197
({struct Cyc_String_pa_PrintArg_struct _tmp31=({struct Cyc_String_pa_PrintArg_struct _tmp65;_tmp65.tag=0U,_tmp65.f1=(struct _fat_ptr)((struct _fat_ptr)progname);_tmp65;});struct Cyc_String_pa_PrintArg_struct _tmp32=({struct Cyc_String_pa_PrintArg_struct _tmp64;_tmp64.tag=0U,_tmp64.f1=(struct _fat_ptr)((struct _fat_ptr)s);_tmp64;});void*_tmp2F[2U];_tmp2F[0]=& _tmp31,_tmp2F[1]=& _tmp32;({struct Cyc___cycFILE*_tmp7D=Cyc_stderr;struct _fat_ptr _tmp7C=({const char*_tmp30="%s: %s.\n";_tag_fat(_tmp30,sizeof(char),9U);});Cyc_fprintf(_tmp7D,_tmp7C,_tag_fat(_tmp2F,sizeof(void*),2U));});});
goto _LL0;}}_LL0:;}
# 200
Cyc_Arg_usage(speclist,errmsg);
Cyc_Arg_current=(int)_get_fat_size(Cyc_Arg_args,sizeof(struct _fat_ptr));}
# 204
void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr orig_args){
# 209
Cyc_Arg_args=orig_args;{
# 211
int initpos=Cyc_Arg_current;
unsigned l=_get_fat_size(Cyc_Arg_args,sizeof(struct _fat_ptr));
# 214
if(({char*_tmp7E=(char*)((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),(int)(l - (unsigned)1)))->curr;_tmp7E == (char*)(_tag_fat(0,0,0)).curr;}))l=l - (unsigned)1;
++ Cyc_Arg_current;
while((unsigned)Cyc_Arg_current < l){
struct _fat_ptr s=*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current));
if((({char*_tmp7F=(char*)s.curr;_tmp7F != (char*)(_tag_fat(0,0,0)).curr;})&& _get_fat_size(s,sizeof(char))>= (unsigned)1)&&(int)((const char*)s.curr)[0]== (int)'-'){
void*action;
{struct _handler_cons _tmp34;_push_handler(& _tmp34);{int _tmp36=0;if(setjmp(_tmp34.handler))_tmp36=1;if(!_tmp36){action=Cyc_Arg_lookup(speclist,s);;_pop_handler();}else{void*_tmp35=(void*)Cyc_Core_get_exn_thrown();void*_tmp37=_tmp35;void*_tmp38;if(((struct Cyc_Core_Not_found_exn_struct*)_tmp37)->tag == Cyc_Core_Not_found){_LL1: _LL2:
# 224
 if(!anonflagfun(s)){
({int _tmp82=initpos;void*_tmp81=(void*)({struct Cyc_Arg_Unknown_Arg_error_struct*_tmp39=_cycalloc(sizeof(*_tmp39));_tmp39->tag=0U,_tmp39->f1=s;_tmp39;});struct Cyc_List_List*_tmp80=speclist;Cyc_Arg_stop(_tmp82,_tmp81,_tmp80,errmsg);});
return;}else{
# 229
++ Cyc_Arg_current;
continue;}}else{_LL3: _tmp38=_tmp37;_LL4: {void*exn=_tmp38;(int)_rethrow(exn);}}_LL0:;}}}
# 233
{struct _handler_cons _tmp3A;_push_handler(& _tmp3A);{int _tmp3C=0;if(setjmp(_tmp3A.handler))_tmp3C=1;if(!_tmp3C){
{void*_tmp3D=action;void(*_tmp3E)(struct _fat_ptr);void(*_tmp3F)(int);void(*_tmp40)(struct _fat_ptr);void(*_tmp41)(struct _fat_ptr,struct _fat_ptr);int*_tmp42;int*_tmp43;void(*_tmp44)(struct _fat_ptr);void(*_tmp45)(void);switch(*((int*)_tmp3D)){case 0U: _LL6: _tmp45=((struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL7: {void(*f)(void)=_tmp45;
f();goto _LL5;}case 1U: _LL8: _tmp44=((struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL9: {void(*f)(struct _fat_ptr)=_tmp44;
f(s);goto _LL5;}case 3U: _LLA: _tmp43=((struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_tmp3D)->f1;_LLB: {int*r=_tmp43;
*r=1;goto _LL5;}case 4U: _LLC: _tmp42=((struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)_tmp3D)->f1;_LLD: {int*r=_tmp42;
*r=0;goto _LL5;}case 2U: _LLE: _tmp41=((struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*)_tmp3D)->f1;_LLF: {void(*f)(struct _fat_ptr,struct _fat_ptr)=_tmp41;
# 240
if((unsigned)(Cyc_Arg_current + 1)< l){
f(s,*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 244
({int _tmp85=initpos;void*_tmp84=(void*)({struct Cyc_Arg_Missing_Arg_error_struct*_tmp46=_cycalloc(sizeof(*_tmp46));_tmp46->tag=1U,_tmp46->f1=s;_tmp46;});struct Cyc_List_List*_tmp83=speclist;Cyc_Arg_stop(_tmp85,_tmp84,_tmp83,errmsg);});}
goto _LL5;}case 5U: _LL10: _tmp40=((struct Cyc_Arg_String_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL11: {void(*f)(struct _fat_ptr)=_tmp40;
# 247
if((unsigned)(Cyc_Arg_current + 1)< l){
f(*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}else{
# 251
({int _tmp88=initpos;void*_tmp87=(void*)({struct Cyc_Arg_Missing_Arg_error_struct*_tmp47=_cycalloc(sizeof(*_tmp47));_tmp47->tag=1U,_tmp47->f1=s;_tmp47;});struct Cyc_List_List*_tmp86=speclist;Cyc_Arg_stop(_tmp88,_tmp87,_tmp86,errmsg);});}
goto _LL5;}case 6U: _LL12: _tmp3F=((struct Cyc_Arg_Int_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL13: {void(*f)(int)=_tmp3F;
# 254
struct _fat_ptr arg=*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current + 1));
int n=0;
if(({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp4A=({struct Cyc_IntPtr_sa_ScanfArg_struct _tmp66;_tmp66.tag=2U,_tmp66.f1=& n;_tmp66;});void*_tmp48[1U];_tmp48[0]=& _tmp4A;({struct _fat_ptr _tmp8A=arg;struct _fat_ptr _tmp89=({const char*_tmp49="%d";_tag_fat(_tmp49,sizeof(char),3U);});Cyc_sscanf(_tmp8A,_tmp89,_tag_fat(_tmp48,sizeof(void*),1U));});})!= 1){
({int _tmp8E=initpos;void*_tmp8D=(void*)({struct Cyc_Arg_Wrong_Arg_error_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->tag=3U,_tmp4C->f1=s,_tmp4C->f2=arg,({struct _fat_ptr _tmp8B=({const char*_tmp4B="an integer";_tag_fat(_tmp4B,sizeof(char),11U);});_tmp4C->f3=_tmp8B;});_tmp4C;});struct Cyc_List_List*_tmp8C=speclist;Cyc_Arg_stop(_tmp8E,_tmp8D,_tmp8C,errmsg);});
_npop_handler(0U);return;}
# 260
f(n);
++ Cyc_Arg_current;
goto _LL5;}default: _LL14: _tmp3E=((struct Cyc_Arg_Rest_spec_Arg_Spec_struct*)_tmp3D)->f1;_LL15: {void(*f)(struct _fat_ptr)=_tmp3E;
# 264
while((unsigned)Cyc_Arg_current < l - (unsigned)1){
f(*((struct _fat_ptr*)_check_fat_subscript(Cyc_Arg_args,sizeof(struct _fat_ptr),Cyc_Arg_current + 1)));
++ Cyc_Arg_current;}
# 268
goto _LL5;}}_LL5:;}
# 234
;_pop_handler();}else{void*_tmp3B=(void*)Cyc_Core_get_exn_thrown();void*_tmp4D=_tmp3B;void*_tmp4E;struct _fat_ptr _tmp4F;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp4D)->tag == Cyc_Arg_Bad){_LL17: _tmp4F=((struct Cyc_Arg_Bad_exn_struct*)_tmp4D)->f1;_LL18: {struct _fat_ptr s2=_tmp4F;
# 271
({int _tmp91=initpos;void*_tmp90=(void*)({struct Cyc_Arg_Message_Arg_error_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=2U,_tmp50->f1=s2;_tmp50;});struct Cyc_List_List*_tmp8F=speclist;Cyc_Arg_stop(_tmp91,_tmp90,_tmp8F,errmsg);});goto _LL16;}}else{_LL19: _tmp4E=_tmp4D;_LL1A: {void*exn=_tmp4E;(int)_rethrow(exn);}}_LL16:;}}}
# 273
++ Cyc_Arg_current;}else{
# 276
{struct _handler_cons _tmp51;_push_handler(& _tmp51);{int _tmp53=0;if(setjmp(_tmp51.handler))_tmp53=1;if(!_tmp53){anonfun(s);;_pop_handler();}else{void*_tmp52=(void*)Cyc_Core_get_exn_thrown();void*_tmp54=_tmp52;void*_tmp55;struct _fat_ptr _tmp56;if(((struct Cyc_Arg_Bad_exn_struct*)_tmp54)->tag == Cyc_Arg_Bad){_LL1C: _tmp56=((struct Cyc_Arg_Bad_exn_struct*)_tmp54)->f1;_LL1D: {struct _fat_ptr s2=_tmp56;
# 278
({int _tmp94=initpos;void*_tmp93=(void*)({struct Cyc_Arg_Message_Arg_error_struct*_tmp57=_cycalloc(sizeof(*_tmp57));_tmp57->tag=2U,_tmp57->f1=s2;_tmp57;});struct Cyc_List_List*_tmp92=speclist;Cyc_Arg_stop(_tmp94,_tmp93,_tmp92,errmsg);});goto _LL1B;}}else{_LL1E: _tmp55=_tmp54;_LL1F: {void*exn=_tmp55;(int)_rethrow(exn);}}_LL1B:;}}}
# 280
++ Cyc_Arg_current;}}}}
