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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};
# 41 "cycboot.h"
extern double modf(double,double*);struct Cyc___cycFILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 161 "cycboot.h"
extern int Cyc_putc(int,struct Cyc___cycFILE*);
# 224 "cycboot.h"
int Cyc_vfprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);
# 228
int Cyc_vprintf(struct _fat_ptr,struct _fat_ptr);
# 232
struct _fat_ptr Cyc_vrprintf(struct _RegionHandle*,struct _fat_ptr,struct _fat_ptr);
# 239
int Cyc_vsnprintf(struct _fat_ptr,unsigned long,struct _fat_ptr,struct _fat_ptr);
# 243
int Cyc_vsprintf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 104 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 87 "printf.cyc"
static struct _fat_ptr Cyc_parg2string(void*x){
void*_tmp0=x;switch(*((int*)_tmp0)){case 0:
 return({const char*_tmp1="string";_tag_fat(_tmp1,sizeof(char),7U);});case 1:
 return({const char*_tmp2="int";_tag_fat(_tmp2,sizeof(char),4U);});case 2:
# 92
 return({const char*_tmp3="double";_tag_fat(_tmp3,sizeof(char),7U);});case 3:
 return({const char*_tmp4="long double";_tag_fat(_tmp4,sizeof(char),12U);});case 4:
 return({const char*_tmp5="short *";_tag_fat(_tmp5,sizeof(char),8U);});default:
 return({const char*_tmp6="unsigned long *";_tag_fat(_tmp6,sizeof(char),16U);});};}
# 99
static void*Cyc_badarg(struct _fat_ptr s){
return(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=Cyc_Core_Invalid_argument,_tmp7->f1=s;_tmp7;}));}
# 104
static int Cyc_va_arg_int(struct _fat_ptr ap){
void*_tmp8=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_stmttmp0=_tmp8;void*_tmp9=_stmttmp0;unsigned long _tmpA;if(*((int*)_tmp9)== 1){_tmpA=((struct Cyc_Int_pa_PrintArg_struct*)_tmp9)->f1;{unsigned long i=_tmpA;
return(int)i;}}else{
return((int(*)(struct _fat_ptr))Cyc_badarg)(({const char*_tmpB="printf expected int";_tag_fat(_tmpB,sizeof(char),20U);}));};}
# 111
static long Cyc_va_arg_long(struct _fat_ptr ap){
void*_tmpC=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_stmttmp1=_tmpC;void*_tmpD=_stmttmp1;unsigned long _tmpE;if(*((int*)_tmpD)== 1){_tmpE=((struct Cyc_Int_pa_PrintArg_struct*)_tmpD)->f1;{unsigned long i=_tmpE;
return(long)i;}}else{
return((long(*)(struct _fat_ptr))Cyc_badarg)(({const char*_tmpF="printf expected int";_tag_fat(_tmpF,sizeof(char),20U);}));};}
# 118
static unsigned long Cyc_va_arg_ulong(struct _fat_ptr ap){
void*_tmp10=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_stmttmp2=_tmp10;void*_tmp11=_stmttmp2;unsigned long _tmp12;if(*((int*)_tmp11)== 1){_tmp12=((struct Cyc_Int_pa_PrintArg_struct*)_tmp11)->f1;{unsigned long i=_tmp12;
return i;}}else{
return((unsigned long(*)(struct _fat_ptr))Cyc_badarg)(({const char*_tmp13="printf expected int";_tag_fat(_tmp13,sizeof(char),20U);}));};}
# 125
static unsigned long Cyc_va_arg_uint(struct _fat_ptr ap){
void*_tmp14=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_stmttmp3=_tmp14;void*_tmp15=_stmttmp3;unsigned long _tmp16;if(*((int*)_tmp15)== 1){_tmp16=((struct Cyc_Int_pa_PrintArg_struct*)_tmp15)->f1;{unsigned long i=_tmp16;
return i;}}else{
return((unsigned long(*)(struct _fat_ptr))Cyc_badarg)(({const char*_tmp17="printf expected int";_tag_fat(_tmp17,sizeof(char),20U);}));};}
# 133
static double Cyc_va_arg_double(struct _fat_ptr ap){
void*_tmp18=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_stmttmp4=_tmp18;void*_tmp19=_stmttmp4;long double _tmp1A;double _tmp1B;switch(*((int*)_tmp19)){case 2: _tmp1B=((struct Cyc_Double_pa_PrintArg_struct*)_tmp19)->f1;{double d=_tmp1B;
return d;}case 3: _tmp1A=((struct Cyc_LongDouble_pa_PrintArg_struct*)_tmp19)->f1;{long double ld=_tmp1A;
return(double)ld;}default:
# 138
(int)_throw(((void*(*)(struct _fat_ptr))Cyc_badarg)((struct _fat_ptr)({struct Cyc_String_pa_PrintArg_struct _tmp1E=({struct Cyc_String_pa_PrintArg_struct _tmpC1;_tmpC1.tag=0,({struct _fat_ptr _tmpC5=(struct _fat_ptr)((struct _fat_ptr)Cyc_parg2string(*((void**)ap.curr)));_tmpC1.f1=_tmpC5;});_tmpC1;});void*_tmp1C[1];_tmp1C[0]=& _tmp1E;({struct _fat_ptr _tmpC6=({const char*_tmp1D="printf expected double but found %s";_tag_fat(_tmp1D,sizeof(char),36U);});Cyc_aprintf(_tmpC6,_tag_fat(_tmp1C,sizeof(void*),1));});})));};}
# 144
static short*Cyc_va_arg_short_ptr(struct _fat_ptr ap){
void*_tmp1F=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_stmttmp5=_tmp1F;void*_tmp20=_stmttmp5;void*_tmp21;if(*((int*)_tmp20)== 4){_tmp21=((struct Cyc_ShortPtr_pa_PrintArg_struct*)_tmp20)->f1;{short*p=_tmp21;
return p;}}else{
(int)_throw(((void*(*)(struct _fat_ptr))Cyc_badarg)(({const char*_tmp22="printf expected short pointer";_tag_fat(_tmp22,sizeof(char),30U);})));};}
# 152
static unsigned long*Cyc_va_arg_int_ptr(struct _fat_ptr ap){
void*_tmp23=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_stmttmp6=_tmp23;void*_tmp24=_stmttmp6;void*_tmp25;if(*((int*)_tmp24)== 5){_tmp25=((struct Cyc_IntPtr_pa_PrintArg_struct*)_tmp24)->f1;{unsigned long*p=_tmp25;
return p;}}else{
(int)_throw(((void*(*)(struct _fat_ptr))Cyc_badarg)(({const char*_tmp26="printf expected int pointer";_tag_fat(_tmp26,sizeof(char),28U);})));};}
# 160
static const struct _fat_ptr Cyc_va_arg_string(struct _fat_ptr ap){
void*_tmp27=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_stmttmp7=_tmp27;void*_tmp28=_stmttmp7;struct _fat_ptr _tmp29;if(*((int*)_tmp28)== 0){_tmp29=((struct Cyc_String_pa_PrintArg_struct*)_tmp28)->f1;{struct _fat_ptr s=_tmp29;
return s;}}else{
(int)_throw(((void*(*)(struct _fat_ptr))Cyc_badarg)(({const char*_tmp2A="printf expected string";_tag_fat(_tmp2A,sizeof(char),23U);})));};}
# 177 "printf.cyc"
int Cyc___cvt_double(double,int,int,int*,int,struct _fat_ptr,struct _fat_ptr);
# 206 "printf.cyc"
enum Cyc_BASE{Cyc_OCT =0U,Cyc_DEC =1U,Cyc_HEX =2U};
# 212
inline static int Cyc__IO_sputn(int(*ioputc)(int,void*),void*ioputc_env,struct _fat_ptr s,int howmany){
# 214
int i=0;
while(i < howmany){
if(ioputc((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U)),ioputc_env)== - 1)return i;
_fat_ptr_inplace_plus(& s,sizeof(char),1);
++ i;}
# 220
return i;}
# 223
static int Cyc__IO_nzsputn(int(*ioputc)(int,void*),void*ioputc_env,struct _fat_ptr s,int howmany){
# 225
int i=0;
while(i < howmany){
char c;
if((int)(c=*((const char*)_check_fat_subscript(s,sizeof(char),0U)))== 0 || ioputc((int)c,ioputc_env)== - 1)return i;
_fat_ptr_inplace_plus(& s,sizeof(char),1);
++ i;}
# 232
return i;}
# 238
static int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int howmany){
# 240
int i=0;
while(i < howmany){
if(ioputc((int)c,ioputc_env)== - 1)return i;
++ i;}
# 245
return i;}
# 249
static struct _fat_ptr Cyc_my_memchr(struct _fat_ptr s,char c,int n){
int sz=(int)_get_fat_size(s,sizeof(char));
n=n < sz?n: sz;
for(1;n != 0;(n --,_fat_ptr_inplace_plus_post(& s,sizeof(char),1))){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U))== (int)c)return s;}
# 255
return _tag_fat(0,0,0);}
# 258
static struct _fat_ptr Cyc_my_nzmemchr(struct _fat_ptr s,char c,int n){
int sz=(int)_get_fat_size(s,sizeof(char));
n=n < sz?n: sz;
for(1;n != 0;(n --,_fat_ptr_inplace_plus_post(& s,sizeof(char),1))){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U))== (int)c)return s;}
# 264
return _tag_fat(0,0,0);}
# 267
inline static const unsigned long Cyc_my_strlen(struct _fat_ptr s){
unsigned sz=_get_fat_size(s,sizeof(char));
unsigned i=0U;
while(i < sz &&(int)((const char*)s.curr)[(int)i]!= 0){++ i;}
return i;}
# 278
int Cyc__IO_vfprintf(int(*ioputc)(int,void*),void*ioputc_env,struct _fat_ptr fmt0,struct _fat_ptr ap){
# 282
struct _fat_ptr fmt;
register int ch;
register int n;
# 286
struct _fat_ptr cp=_tag_fat(0,0,0);
# 289
struct _fat_ptr cp2=_tag_fat(0,0,0);
# 292
struct _fat_ptr cp3=_tag_fat(0,0,0);
# 295
int which_cp;
struct _fat_ptr fmark;
register int flags;
int ret;
int width;
int prec;
char sign;
# 303
char sign_string[2U];sign_string[0]='\000',sign_string[1]='\000';{
int softsign=0;
double _double;
int fpprec;
unsigned long _ulong;
int dprec;
int dpad;
int fieldsz;
# 314
int size=0;
# 316
char buf[349U];({{unsigned _tmpC2=348U;unsigned i;for(i=0;i < _tmpC2;++ i){buf[i]='\000';}buf[_tmpC2]=0;}0;});{
char ox[2U];ox[0]='\000',ox[1]='\000';{
enum Cyc_BASE base;
# 342 "printf.cyc"
fmt=fmt0;
ret=0;
# 348
for(1;1;1){
# 351
fmark=fmt;{
unsigned fmt_sz=_get_fat_size(fmt,sizeof(char));
for(n=0;((unsigned)n < fmt_sz &&(ch=(int)((const char*)fmt.curr)[n])!= 0)&& ch != 37;++ n){
;}
fmt=_fat_ptr_plus(fmt,sizeof(char),n);
# 357
if((n=(fmt.curr - fmark.curr)/ sizeof(char))!= 0){
do{if(({int _tmpC7=Cyc__IO_sputn(ioputc,ioputc_env,(struct _fat_ptr)fmark,n);_tmpC7 != n;}))goto error;}while(0);
ret +=n;}
# 361
if(ch == 0)
goto done;
_fat_ptr_inplace_plus(& fmt,sizeof(char),1);
# 365
flags=0;
dprec=0;
fpprec=0;
width=0;
prec=-1;
sign='\000';
# 372
rflag: ch=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
reswitch: which_cp=0;
{int _tmp2B=ch;switch((int)_tmp2B){case 32:
# 381
 if(!((int)sign))
sign=' ';
goto rflag;case 35:
# 385
 flags |=8;
goto rflag;case 42:
# 394
 width=Cyc_va_arg_int(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
if(width >= 0)
goto rflag;
width=- width;
goto _LL8;case 45: _LL8:
# 400
 flags |=16;
flags &=~ 32;
goto rflag;case 43:
# 404
 sign='+';
goto rflag;case 46:
# 407
 if((ch=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U)))== 42){
n=Cyc_va_arg_int(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
prec=n < 0?- 1: n;
goto rflag;}
# 412
n=0;
while((unsigned)(ch - (int)'0')<= 9U){
n=10 * n + (ch - (int)'0');
ch=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));}
# 417
prec=n < 0?- 1: n;
goto reswitch;case 48:
# 425
 if(!(flags & 16))
flags |=32;
goto rflag;case 49:
 goto _LL12;case 50: _LL12: goto _LL14;case 51: _LL14: goto _LL16;case 52: _LL16: goto _LL18;case 53: _LL18:
 goto _LL1A;case 54: _LL1A: goto _LL1C;case 55: _LL1C: goto _LL1E;case 56: _LL1E: goto _LL20;case 57: _LL20:
 n=0;
do{
n=10 * n + (ch - (int)'0');
ch=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));}while((unsigned)(ch - (int)'0')<= (unsigned)9);
# 435
width=n;
goto reswitch;case 76:
# 438
 flags |=2;
goto rflag;case 104:
# 441
 flags |=4;
goto rflag;case 108:
# 444
 flags |=1;
goto rflag;case 99:
# 447
 cp=({char*_tmp2C=buf;_tag_fat(_tmp2C,sizeof(char),349U);});
({struct _fat_ptr _tmp2D=cp;char _tmp2E=*((char*)_check_fat_subscript(_tmp2D,sizeof(char),0U));char _tmp2F=(char)Cyc_va_arg_int(ap);if(_get_fat_size(_tmp2D,sizeof(char))== 1U &&(_tmp2E == 0 && _tmp2F != 0))_throw_arraybounds();*((char*)_tmp2D.curr)=_tmp2F;});_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
size=1;
sign='\000';
goto _LL0;case 68:
# 453
 flags |=1;
goto _LL2C;case 100: _LL2C:
 goto _LL2E;case 105: _LL2E:
 _ulong=(unsigned long)(flags & 1?Cyc_va_arg_long(ap):(flags & 4?(long)((short)Cyc_va_arg_int(ap)): Cyc_va_arg_int(ap)));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
if((long)_ulong < 0){
_ulong=- _ulong;
sign='-';}
# 461
base=1U;
goto number;case 101:
 goto _LL32;case 69: _LL32: goto _LL34;case 102: _LL34: goto _LL36;case 70: _LL36: goto _LL38;case 103: _LL38:
 goto _LL3A;case 71: _LL3A:
 _double=Cyc_va_arg_double(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
# 470
if(prec > 39){
if(ch != 103 && ch != 71 || flags & 8)
fpprec=prec - 39;
prec=39;}else{
if(prec == -1)
prec=6;}
# 482
cp=({char*_tmp30=buf;_tag_fat(_tmp30,sizeof(char),349U);});
({struct _fat_ptr _tmp31=cp;char _tmp32=*((char*)_check_fat_subscript(_tmp31,sizeof(char),0U));char _tmp33='\000';if(_get_fat_size(_tmp31,sizeof(char))== 1U &&(_tmp32 == 0 && _tmp33 != 0))_throw_arraybounds();*((char*)_tmp31.curr)=_tmp33;});
size=({double _tmpCC=_double;int _tmpCB=prec;int _tmpCA=flags;int _tmpC9=ch;struct _fat_ptr _tmpC8=cp;Cyc___cvt_double(_tmpCC,_tmpCB,_tmpCA,& softsign,_tmpC9,_tmpC8,
# 486
_fat_ptr_plus(({char*_tmp34=buf;_tag_fat(_tmp34,sizeof(char),349U);}),sizeof(char),(int)(sizeof(buf)- (unsigned)1)));});
if(softsign)
sign='-';
if((int)*((char*)cp.curr)== (int)'\000')
_fat_ptr_inplace_plus(& cp,sizeof(char),1);
goto _LL0;case 110:
# 493
 if(flags & 1)
({unsigned long _tmpCD=(unsigned long)ret;*Cyc_va_arg_int_ptr(ap)=_tmpCD;});else{
if(flags & 4)
({short _tmpCE=(short)ret;*Cyc_va_arg_short_ptr(ap)=_tmpCE;});else{
# 498
({unsigned long _tmpCF=(unsigned long)ret;*Cyc_va_arg_int_ptr(ap)=_tmpCF;});}}
_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
continue;case 79:
# 502
 flags |=1;
goto _LL40;case 111: _LL40:
# 505
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
base=0U;
goto nosign;case 112:
# 517 "printf.cyc"
 _ulong=(unsigned long)Cyc_va_arg_long(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
base=2U;
flags |=64;
ch=120;
goto nosign;case 115:  {
# 523
struct _fat_ptr b=Cyc_va_arg_string(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
which_cp=3;cp3=b;
if(prec >= 0){
struct _fat_ptr p=Cyc_my_nzmemchr(cp3,'\000',prec);
if(({char*_tmpD0=(char*)p.curr;_tmpD0 != (char*)(_tag_fat(0,0,0)).curr;})){
size=(p.curr - cp3.curr)/ sizeof(char);
if(size > prec)
size=prec;}else{
# 532
size=prec;}}else{
# 534
size=(int)Cyc_my_strlen(cp3);}
sign='\000';
goto _LL0;}case 85:
# 538
 flags |=1;
goto _LL48;case 117: _LL48:
# 541
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
base=1U;
goto nosign;case 88:
 goto _LL4C;case 120: _LL4C:
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
base=2U;
# 548
if(flags & 8 && _ulong != 0U)
flags |=64;
# 552
nosign: sign='\000';
# 558
number: if((dprec=prec)>= 0)
flags &=~ 32;
# 566
cp=_fat_ptr_plus(({char*_tmp35=buf;_tag_fat(_tmp35,sizeof(char),349U);}),sizeof(char),(308 + 39)+ 1);
if(_ulong != 0U || prec != 0){
struct _fat_ptr xdigs;
# 574
enum Cyc_BASE _tmp36=base;switch((int)_tmp36){case Cyc_OCT:
# 576
 do{
({struct _fat_ptr _tmp37=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp38=*((char*)_check_fat_subscript(_tmp37,sizeof(char),0U));char _tmp39=(char)((_ulong & (unsigned long)7)+ (unsigned long)'0');if(_get_fat_size(_tmp37,sizeof(char))== 1U &&(_tmp38 == 0 && _tmp39 != 0))_throw_arraybounds();*((char*)_tmp37.curr)=_tmp39;});
_ulong >>=3U;}while((int)_ulong);
# 581
if(flags & 8 &&(int)*((char*)_check_fat_subscript(cp,sizeof(char),0U))!= (int)'0')
({struct _fat_ptr _tmp3A=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp3B=*((char*)_check_fat_subscript(_tmp3A,sizeof(char),0U));char _tmp3C='0';if(_get_fat_size(_tmp3A,sizeof(char))== 1U &&(_tmp3B == 0 && _tmp3C != 0))_throw_arraybounds();*((char*)_tmp3A.curr)=_tmp3C;});
goto _LL4F;case Cyc_DEC:
# 587
 while(_ulong >= 10U){
({struct _fat_ptr _tmp3D=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp3E=*((char*)_check_fat_subscript(_tmp3D,sizeof(char),0U));char _tmp3F=(char)(_ulong % (unsigned long)10 + (unsigned long)'0');if(_get_fat_size(_tmp3D,sizeof(char))== 1U &&(_tmp3E == 0 && _tmp3F != 0))_throw_arraybounds();*((char*)_tmp3D.curr)=_tmp3F;});
_ulong /=10U;}
# 591
({struct _fat_ptr _tmp40=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp41=*((char*)_check_fat_subscript(_tmp40,sizeof(char),0U));char _tmp42=(char)(_ulong + (unsigned long)'0');if(_get_fat_size(_tmp40,sizeof(char))== 1U &&(_tmp41 == 0 && _tmp42 != 0))_throw_arraybounds();*((char*)_tmp40.curr)=_tmp42;});
goto _LL4F;case Cyc_HEX:
# 595
 if(ch == 88)
xdigs=({const char*_tmp43="0123456789ABCDEF";_tag_fat(_tmp43,sizeof(char),17U);});else{
# 598
xdigs=({const char*_tmp44="0123456789abcdef";_tag_fat(_tmp44,sizeof(char),17U);});}
do{
({struct _fat_ptr _tmp45=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _tmp46=*((char*)_check_fat_subscript(_tmp45,sizeof(char),0U));char _tmp47=*((const char*)_check_fat_subscript(xdigs,sizeof(char),(int)(_ulong & 15U)));if(_get_fat_size(_tmp45,sizeof(char))== 1U &&(_tmp46 == 0 && _tmp47 != 0))_throw_arraybounds();*((char*)_tmp45.curr)=_tmp47;});
_ulong >>=4U;}while((int)_ulong);
# 603
goto _LL4F;default:
# 605
 cp=(struct _fat_ptr)Cyc_strdup(({const char*_tmp48="bug in vform: bad base";_tag_fat(_tmp48,sizeof(char),23U);}));
goto skipsize;}_LL4F:;}
# 609
size=({unsigned char*_tmpD1=(_fat_ptr_plus(({char*_tmp49=buf;_tag_fat(_tmp49,sizeof(char),349U);}),sizeof(char),348)).curr;_tmpD1 - cp.curr;})/ sizeof(char);
skipsize:
 goto _LL0;default:
# 613
 if(ch == 0)
goto done;
# 616
cp=({char*_tmp4A=buf;_tag_fat(_tmp4A,sizeof(char),349U);});
({struct _fat_ptr _tmp4B=cp;char _tmp4C=*((char*)_check_fat_subscript(_tmp4B,sizeof(char),0U));char _tmp4D=(char)ch;if(_get_fat_size(_tmp4B,sizeof(char))== 1U &&(_tmp4C == 0 && _tmp4D != 0))_throw_arraybounds();*((char*)_tmp4B.curr)=_tmp4D;});
size=1;
sign='\000';
goto _LL0;}_LL0:;}
# 645 "printf.cyc"
fieldsz=size + fpprec;
dpad=dprec - size;
if(dpad < 0)
dpad=0;
# 650
if((int)sign)
++ fieldsz;else{
if(flags & 64)
fieldsz +=2;}
fieldsz +=dpad;
# 657
if((flags & (16 | 32))== 0){
if(({int _tmpD2=Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz);_tmpD2 < width - fieldsz;}))goto error;}
# 661
if((int)sign){
({struct _fat_ptr _tmp4F=_fat_ptr_plus(({char*_tmp4E=sign_string;_tag_fat(_tmp4E,sizeof(char),2U);}),sizeof(char),0);char _tmp50=*((char*)_check_fat_subscript(_tmp4F,sizeof(char),0U));char _tmp51=sign;if(_get_fat_size(_tmp4F,sizeof(char))== 1U &&(_tmp50 == 0 && _tmp51 != 0))_throw_arraybounds();*((char*)_tmp4F.curr)=_tmp51;});
do{if(({int(*_tmpD4)(int,void*)=ioputc;void*_tmpD3=ioputc_env;Cyc__IO_sputn(_tmpD4,_tmpD3,(struct _fat_ptr)({char*_tmp52=sign_string;_tag_fat(_tmp52,sizeof(char),2U);}),1);})!= 1)goto error;}while(0);}else{
if(flags & 64){
ox[0]='0';
ox[1]=(char)ch;
do{if(({int(*_tmpD6)(int,void*)=ioputc;void*_tmpD5=ioputc_env;Cyc__IO_nzsputn(_tmpD6,_tmpD5,_tag_fat(ox,sizeof(char),2U),2);})!= 2)goto error;}while(0);}}
# 671
if((flags & (16 | 32))== 32){
if(({int _tmpD7=Cyc__IO_padn(ioputc,ioputc_env,'0',width - fieldsz);_tmpD7 < width - fieldsz;}))goto error;}
# 675
if(({int _tmpD8=Cyc__IO_padn(ioputc,ioputc_env,'0',dpad);_tmpD8 < dpad;}))goto error;
# 678
if(which_cp == 0)
do{if(({int _tmpD9=Cyc__IO_sputn(ioputc,ioputc_env,(struct _fat_ptr)cp,size);_tmpD9 != size;}))goto error;}while(0);else{
if(which_cp == 2)
do{if(({int _tmpDA=Cyc__IO_sputn(ioputc,ioputc_env,(struct _fat_ptr)cp2,size);_tmpDA != size;}))goto error;}while(0);else{
if(which_cp == 3)
do{if(({int _tmpDB=Cyc__IO_nzsputn(ioputc,ioputc_env,(struct _fat_ptr)cp3,size);_tmpDB != size;}))goto error;}while(0);}}
# 686
if(({int _tmpDC=Cyc__IO_padn(ioputc,ioputc_env,'0',fpprec);_tmpDC < fpprec;}))goto error;
# 689
if(flags & 16){
if(({int _tmpDD=Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz);_tmpDD < width - fieldsz;}))goto error;}
# 693
ret +=width > fieldsz?width: fieldsz;}}
# 696
done:
 return ret;
error:
 return - 1;}}}}
# 703
static struct _fat_ptr Cyc_exponent(struct _fat_ptr p,int exp,int fmtch){
# 705
struct _fat_ptr t;
char expbuffer[309U];({{unsigned _tmpC3=308U;unsigned i;for(i=0;i < _tmpC3;++ i){expbuffer[i]='0';}expbuffer[_tmpC3]=0;}0;});{
struct _fat_ptr expbuf=({char*_tmp6B=expbuffer;_tag_fat(_tmp6B,sizeof(char),309U);});
({struct _fat_ptr _tmp53=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp54=*((char*)_check_fat_subscript(_tmp53,sizeof(char),0U));char _tmp55=(char)fmtch;if(_get_fat_size(_tmp53,sizeof(char))== 1U &&(_tmp54 == 0 && _tmp55 != 0))_throw_arraybounds();*((char*)_tmp53.curr)=_tmp55;});
if(exp < 0){
exp=- exp;
({struct _fat_ptr _tmp56=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp57=*((char*)_check_fat_subscript(_tmp56,sizeof(char),0U));char _tmp58='-';if(_get_fat_size(_tmp56,sizeof(char))== 1U &&(_tmp57 == 0 && _tmp58 != 0))_throw_arraybounds();*((char*)_tmp56.curr)=_tmp58;});}else{
# 714
({struct _fat_ptr _tmp59=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp5A=*((char*)_check_fat_subscript(_tmp59,sizeof(char),0U));char _tmp5B='+';if(_get_fat_size(_tmp59,sizeof(char))== 1U &&(_tmp5A == 0 && _tmp5B != 0))_throw_arraybounds();*((char*)_tmp59.curr)=_tmp5B;});}
t=_fat_ptr_plus(expbuf,sizeof(char),308);
if(exp > 9){
do{
({struct _fat_ptr _tmp5C=_fat_ptr_inplace_plus(& t,sizeof(char),-1);char _tmp5D=*((char*)_check_fat_subscript(_tmp5C,sizeof(char),0U));char _tmp5E=(char)(exp % 10 + (int)'0');if(_get_fat_size(_tmp5C,sizeof(char))== 1U &&(_tmp5D == 0 && _tmp5E != 0))_throw_arraybounds();*((char*)_tmp5C.curr)=_tmp5E;});}while((exp /=10)> 9);
# 720
({struct _fat_ptr _tmp5F=_fat_ptr_inplace_plus(& t,sizeof(char),-1);char _tmp60=*((char*)_check_fat_subscript(_tmp5F,sizeof(char),0U));char _tmp61=(char)(exp + (int)'0');if(_get_fat_size(_tmp5F,sizeof(char))== 1U &&(_tmp60 == 0 && _tmp61 != 0))_throw_arraybounds();*((char*)_tmp5F.curr)=_tmp61;});
for(1;({char*_tmpDE=(char*)t.curr;_tmpDE < (char*)(_fat_ptr_plus(expbuf,sizeof(char),308)).curr;});({struct _fat_ptr _tmp62=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp63=*((char*)_check_fat_subscript(_tmp62,sizeof(char),0U));char _tmp64=*((char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& t,sizeof(char),1),sizeof(char),0U));if(_get_fat_size(_tmp62,sizeof(char))== 1U &&(_tmp63 == 0 && _tmp64 != 0))_throw_arraybounds();*((char*)_tmp62.curr)=_tmp64;})){;}}else{
# 724
({struct _fat_ptr _tmp65=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp66=*((char*)_check_fat_subscript(_tmp65,sizeof(char),0U));char _tmp67='0';if(_get_fat_size(_tmp65,sizeof(char))== 1U &&(_tmp66 == 0 && _tmp67 != 0))_throw_arraybounds();*((char*)_tmp65.curr)=_tmp67;});
({struct _fat_ptr _tmp68=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp69=*((char*)_check_fat_subscript(_tmp68,sizeof(char),0U));char _tmp6A=(char)(exp + (int)'0');if(_get_fat_size(_tmp68,sizeof(char))== 1U &&(_tmp69 == 0 && _tmp6A != 0))_throw_arraybounds();*((char*)_tmp68.curr)=_tmp6A;});}
# 727
return p;}}
# 730
static struct _fat_ptr Cyc_sround(double fract,int*exp,struct _fat_ptr start,struct _fat_ptr end,char ch,int*signp){
# 734
double tmp=0.0;
# 736
if(fract != 0.0)
modf(fract * (double)10,& tmp);else{
# 739
tmp=(double)((int)ch - (int)'0');}
if(tmp > (double)4)
for(1;1;_fat_ptr_inplace_plus(& end,sizeof(char),-1)){
if((int)*((char*)_check_fat_subscript(end,sizeof(char),0U))== (int)'.')
_fat_ptr_inplace_plus(& end,sizeof(char),-1);
if((int)({struct _fat_ptr _tmp6C=end;char _tmp6D=*((char*)_check_fat_subscript(_tmp6C,sizeof(char),0U));char _tmp6E=_tmp6D + (int)'\001';if(_get_fat_size(_tmp6C,sizeof(char))== 1U &&(_tmp6D == 0 && _tmp6E != 0))_throw_arraybounds();*((char*)_tmp6C.curr)=_tmp6E;})<= (int)'9')
break;
({struct _fat_ptr _tmp6F=end;char _tmp70=*((char*)_check_fat_subscript(_tmp6F,sizeof(char),0U));char _tmp71='0';if(_get_fat_size(_tmp6F,sizeof(char))== 1U &&(_tmp70 == 0 && _tmp71 != 0))_throw_arraybounds();*((char*)_tmp6F.curr)=_tmp71;});
if((char*)end.curr == (char*)start.curr){
if((unsigned)exp){
({struct _fat_ptr _tmp72=end;char _tmp73=*((char*)_check_fat_subscript(_tmp72,sizeof(char),0U));char _tmp74='1';if(_get_fat_size(_tmp72,sizeof(char))== 1U &&(_tmp73 == 0 && _tmp74 != 0))_throw_arraybounds();*((char*)_tmp72.curr)=_tmp74;});
++(*exp);}else{
# 753
({struct _fat_ptr _tmp75=_fat_ptr_inplace_plus(& end,sizeof(char),-1);char _tmp76=*((char*)_check_fat_subscript(_tmp75,sizeof(char),0U));char _tmp77='1';if(_get_fat_size(_tmp75,sizeof(char))== 1U &&(_tmp76 == 0 && _tmp77 != 0))_throw_arraybounds();*((char*)_tmp75.curr)=_tmp77;});
_fat_ptr_inplace_plus(& start,sizeof(char),-1);}
# 756
break;}}else{
# 760
if(*signp == (int)'-')
for(1;1;_fat_ptr_inplace_plus(& end,sizeof(char),-1)){
if((int)*((char*)_check_fat_subscript(end,sizeof(char),0U))== (int)'.')
_fat_ptr_inplace_plus(& end,sizeof(char),-1);
if((int)*((char*)_check_fat_subscript(end,sizeof(char),0U))!= (int)'0')
break;
if((char*)end.curr == (char*)start.curr)
*signp=0;}}
# 769
return start;}
# 772
int Cyc___cvt_double(double number,int prec,int flags,int*signp,int fmtch,struct _fat_ptr startp,struct _fat_ptr endp){
# 775
struct _fat_ptr p;struct _fat_ptr t;
register double fract;
int dotrim=0;int expcnt;int gformat=0;
double integer=0.0;double tmp=0.0;
# 780
expcnt=0;
if(number < (double)0){
number=- number;
*signp=(int)'-';}else{
# 785
*signp=0;}
# 787
fract=modf(number,& integer);
# 790
t=_fat_ptr_inplace_plus(& startp,sizeof(char),1);
# 796
for(p=_fat_ptr_plus(endp,sizeof(char),- 1);(char*)p.curr >= (char*)startp.curr && integer != 0.0;++ expcnt){
tmp=modf(integer / (double)10,& integer);
({struct _fat_ptr _tmp78=_fat_ptr_inplace_plus_post(& p,sizeof(char),-1);char _tmp79=*((char*)_check_fat_subscript(_tmp78,sizeof(char),0U));char _tmp7A=(char)((int)((tmp + .01)* (double)10)+ (int)'0');if(_get_fat_size(_tmp78,sizeof(char))== 1U &&(_tmp79 == 0 && _tmp7A != 0))_throw_arraybounds();*((char*)_tmp78.curr)=_tmp7A;});}
# 800
{int _tmp7B=fmtch;switch((int)_tmp7B){case 102:
 goto _LL4;case 70: _LL4:
# 803
 if(expcnt)
for(1;({char*_tmpDF=(char*)(_fat_ptr_inplace_plus(& p,sizeof(char),1)).curr;_tmpDF < (char*)endp.curr;});({struct _fat_ptr _tmp7C=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp7D=*((char*)_check_fat_subscript(_tmp7C,sizeof(char),0U));char _tmp7E=*((char*)_check_fat_subscript(p,sizeof(char),0U));if(_get_fat_size(_tmp7C,sizeof(char))== 1U &&(_tmp7D == 0 && _tmp7E != 0))_throw_arraybounds();*((char*)_tmp7C.curr)=_tmp7E;})){;}else{
# 806
({struct _fat_ptr _tmp7F=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp80=*((char*)_check_fat_subscript(_tmp7F,sizeof(char),0U));char _tmp81='0';if(_get_fat_size(_tmp7F,sizeof(char))== 1U &&(_tmp80 == 0 && _tmp81 != 0))_throw_arraybounds();*((char*)_tmp7F.curr)=_tmp81;});}
# 811
if(prec || flags & 8)
({struct _fat_ptr _tmp82=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp83=*((char*)_check_fat_subscript(_tmp82,sizeof(char),0U));char _tmp84='.';if(_get_fat_size(_tmp82,sizeof(char))== 1U &&(_tmp83 == 0 && _tmp84 != 0))_throw_arraybounds();*((char*)_tmp82.curr)=_tmp84;});
# 814
if(fract != 0.0){
if(prec)
do{
fract=modf(fract * (double)10,& tmp);
({struct _fat_ptr _tmp85=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp86=*((char*)_check_fat_subscript(_tmp85,sizeof(char),0U));char _tmp87=(char)((int)tmp + (int)'0');if(_get_fat_size(_tmp85,sizeof(char))== 1U &&(_tmp86 == 0 && _tmp87 != 0))_throw_arraybounds();*((char*)_tmp85.curr)=_tmp87;});}while(
-- prec && fract != 0.0);
if(fract != 0.0)
startp=({double _tmpE2=fract;struct _fat_ptr _tmpE1=startp;struct _fat_ptr _tmpE0=
_fat_ptr_plus(t,sizeof(char),- 1);
# 821
Cyc_sround(_tmpE2,0,_tmpE1,_tmpE0,'\000',signp);});}
# 824
for(1;prec --;({struct _fat_ptr _tmp88=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp89=*((char*)_check_fat_subscript(_tmp88,sizeof(char),0U));char _tmp8A='0';if(_get_fat_size(_tmp88,sizeof(char))== 1U &&(_tmp89 == 0 && _tmp8A != 0))_throw_arraybounds();*((char*)_tmp88.curr)=_tmp8A;})){;}
goto _LL0;case 101:
 goto _LL8;case 69: _LL8:
 eformat: if(expcnt){
({struct _fat_ptr _tmp8B=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp8C=*((char*)_check_fat_subscript(_tmp8B,sizeof(char),0U));char _tmp8D=*((char*)_check_fat_subscript(_fat_ptr_inplace_plus(& p,sizeof(char),1),sizeof(char),0U));if(_get_fat_size(_tmp8B,sizeof(char))== 1U &&(_tmp8C == 0 && _tmp8D != 0))_throw_arraybounds();*((char*)_tmp8B.curr)=_tmp8D;});
if(prec || flags & 8)
({struct _fat_ptr _tmp8E=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp8F=*((char*)_check_fat_subscript(_tmp8E,sizeof(char),0U));char _tmp90='.';if(_get_fat_size(_tmp8E,sizeof(char))== 1U &&(_tmp8F == 0 && _tmp90 != 0))_throw_arraybounds();*((char*)_tmp8E.curr)=_tmp90;});
# 832
for(1;prec &&({char*_tmpE3=(char*)(_fat_ptr_inplace_plus(& p,sizeof(char),1)).curr;_tmpE3 < (char*)endp.curr;});-- prec){
({struct _fat_ptr _tmp91=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp92=*((char*)_check_fat_subscript(_tmp91,sizeof(char),0U));char _tmp93=*((char*)_check_fat_subscript(p,sizeof(char),0U));if(_get_fat_size(_tmp91,sizeof(char))== 1U &&(_tmp92 == 0 && _tmp93 != 0))_throw_arraybounds();*((char*)_tmp91.curr)=_tmp93;});}
# 839
if(!prec &&({char*_tmpE4=(char*)(_fat_ptr_inplace_plus(& p,sizeof(char),1)).curr;_tmpE4 < (char*)endp.curr;})){
fract=(double)0;
startp=({struct _fat_ptr _tmpE7=startp;struct _fat_ptr _tmpE6=
_fat_ptr_plus(t,sizeof(char),- 1);
# 841
char _tmpE5=*((char*)_check_fat_subscript(p,sizeof(char),0U));Cyc_sround((double)0,& expcnt,_tmpE7,_tmpE6,_tmpE5,signp);});}
# 845
-- expcnt;}else{
# 848
if(fract != 0.0){
# 850
for(expcnt=- 1;1;-- expcnt){
fract=modf(fract * (double)10,& tmp);
if(tmp != 0.0)
break;}
# 855
({struct _fat_ptr _tmp94=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp95=*((char*)_check_fat_subscript(_tmp94,sizeof(char),0U));char _tmp96=(char)((int)tmp + (int)'0');if(_get_fat_size(_tmp94,sizeof(char))== 1U &&(_tmp95 == 0 && _tmp96 != 0))_throw_arraybounds();*((char*)_tmp94.curr)=_tmp96;});
if(prec || flags & 8)
({struct _fat_ptr _tmp97=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp98=*((char*)_check_fat_subscript(_tmp97,sizeof(char),0U));char _tmp99='.';if(_get_fat_size(_tmp97,sizeof(char))== 1U &&(_tmp98 == 0 && _tmp99 != 0))_throw_arraybounds();*((char*)_tmp97.curr)=_tmp99;});}else{
# 860
({struct _fat_ptr _tmp9A=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp9B=*((char*)_check_fat_subscript(_tmp9A,sizeof(char),0U));char _tmp9C='0';if(_get_fat_size(_tmp9A,sizeof(char))== 1U &&(_tmp9B == 0 && _tmp9C != 0))_throw_arraybounds();*((char*)_tmp9A.curr)=_tmp9C;});
if(prec || flags & 8)
({struct _fat_ptr _tmp9D=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmp9E=*((char*)_check_fat_subscript(_tmp9D,sizeof(char),0U));char _tmp9F='.';if(_get_fat_size(_tmp9D,sizeof(char))== 1U &&(_tmp9E == 0 && _tmp9F != 0))_throw_arraybounds();*((char*)_tmp9D.curr)=_tmp9F;});}}
# 865
if(fract != 0.0){
if(prec)
do{
fract=modf(fract * (double)10,& tmp);
({struct _fat_ptr _tmpA0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpA1=*((char*)_check_fat_subscript(_tmpA0,sizeof(char),0U));char _tmpA2=(char)((int)tmp + (int)'0');if(_get_fat_size(_tmpA0,sizeof(char))== 1U &&(_tmpA1 == 0 && _tmpA2 != 0))_throw_arraybounds();*((char*)_tmpA0.curr)=_tmpA2;});}while(
-- prec && fract != 0.0);
if(fract != 0.0)
startp=({double _tmpEA=fract;struct _fat_ptr _tmpE9=startp;struct _fat_ptr _tmpE8=
_fat_ptr_plus(t,sizeof(char),- 1);
# 872
Cyc_sround(_tmpEA,& expcnt,_tmpE9,_tmpE8,'\000',signp);});}
# 876
for(1;prec --;({struct _fat_ptr _tmpA3=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpA4=*((char*)_check_fat_subscript(_tmpA3,sizeof(char),0U));char _tmpA5='0';if(_get_fat_size(_tmpA3,sizeof(char))== 1U &&(_tmpA4 == 0 && _tmpA5 != 0))_throw_arraybounds();*((char*)_tmpA3.curr)=_tmpA5;})){;}
# 879
if(gformat && !(flags & 8)){
while((char*)t.curr > (char*)startp.curr &&(int)*((char*)_check_fat_subscript(_fat_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0U))== (int)'0'){;}
if((int)*((char*)_check_fat_subscript(t,sizeof(char),0U))== (int)'.')
_fat_ptr_inplace_plus(& t,sizeof(char),-1);
_fat_ptr_inplace_plus(& t,sizeof(char),1);}
# 885
t=Cyc_exponent(t,expcnt,fmtch);
goto _LL0;case 103:
 goto _LLC;case 71: _LLC:
# 889
 if(!prec)
++ prec;
# 897
if(expcnt > prec ||(!expcnt && fract != 0.0)&& fract < .0001){
# 905
-- prec;
fmtch -=2;
gformat=1;
goto eformat;}
# 914
if(expcnt)
for(1;({char*_tmpEB=(char*)(_fat_ptr_inplace_plus(& p,sizeof(char),1)).curr;_tmpEB < (char*)endp.curr;});(({struct _fat_ptr _tmpA6=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpA7=*((char*)_check_fat_subscript(_tmpA6,sizeof(char),0U));char _tmpA8=*((char*)_check_fat_subscript(p,sizeof(char),0U));if(_get_fat_size(_tmpA6,sizeof(char))== 1U &&(_tmpA7 == 0 && _tmpA8 != 0))_throw_arraybounds();*((char*)_tmpA6.curr)=_tmpA8;}),-- prec)){;}else{
# 917
({struct _fat_ptr _tmpA9=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpAA=*((char*)_check_fat_subscript(_tmpA9,sizeof(char),0U));char _tmpAB='0';if(_get_fat_size(_tmpA9,sizeof(char))== 1U &&(_tmpAA == 0 && _tmpAB != 0))_throw_arraybounds();*((char*)_tmpA9.curr)=_tmpAB;});}
# 922
if(prec || flags & 8){
dotrim=1;
({struct _fat_ptr _tmpAC=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpAD=*((char*)_check_fat_subscript(_tmpAC,sizeof(char),0U));char _tmpAE='.';if(_get_fat_size(_tmpAC,sizeof(char))== 1U &&(_tmpAD == 0 && _tmpAE != 0))_throw_arraybounds();*((char*)_tmpAC.curr)=_tmpAE;});}else{
# 927
dotrim=0;}
# 929
if(fract != 0.0){
if(prec){
# 933
do{
fract=modf(fract * (double)10,& tmp);
({struct _fat_ptr _tmpAF=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpB0=*((char*)_check_fat_subscript(_tmpAF,sizeof(char),0U));char _tmpB1=(char)((int)tmp + (int)'0');if(_get_fat_size(_tmpAF,sizeof(char))== 1U &&(_tmpB0 == 0 && _tmpB1 != 0))_throw_arraybounds();*((char*)_tmpAF.curr)=_tmpB1;});}while(
tmp == 0.0 && !expcnt);
while(-- prec && fract != 0.0){
fract=modf(fract * (double)10,& tmp);
({struct _fat_ptr _tmpB2=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpB3=*((char*)_check_fat_subscript(_tmpB2,sizeof(char),0U));char _tmpB4=(char)((int)tmp + (int)'0');if(_get_fat_size(_tmpB2,sizeof(char))== 1U &&(_tmpB3 == 0 && _tmpB4 != 0))_throw_arraybounds();*((char*)_tmpB2.curr)=_tmpB4;});}}
# 942
if(fract != 0.0)
startp=({double _tmpEE=fract;struct _fat_ptr _tmpED=startp;struct _fat_ptr _tmpEC=
_fat_ptr_plus(t,sizeof(char),- 1);
# 943
Cyc_sround(_tmpEE,0,_tmpED,_tmpEC,'\000',signp);});}
# 947
if(flags & 8)
for(1;prec --;({struct _fat_ptr _tmpB5=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _tmpB6=*((char*)_check_fat_subscript(_tmpB5,sizeof(char),0U));char _tmpB7='0';if(_get_fat_size(_tmpB5,sizeof(char))== 1U &&(_tmpB6 == 0 && _tmpB7 != 0))_throw_arraybounds();*((char*)_tmpB5.curr)=_tmpB7;})){;}else{
if(dotrim){
while((char*)t.curr > (char*)startp.curr &&(int)*((char*)_check_fat_subscript(_fat_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0U))== (int)'0'){;}
if((int)*((char*)_check_fat_subscript(t,sizeof(char),0U))!= (int)'.')
_fat_ptr_inplace_plus(& t,sizeof(char),1);}}
# 954
goto _LL0;default:
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmpEF=({const char*_tmpB8="__cvt_double";_tag_fat(_tmpB8,sizeof(char),13U);});_tmpB9->f1=_tmpEF;});_tmpB9;}));}_LL0:;}
# 957
return(t.curr - startp.curr)/ sizeof(char);}
# 961
int Cyc_vfprintf(struct Cyc___cycFILE*f,struct _fat_ptr fmt,struct _fat_ptr ap){
# 964
int ans;
ans=((int(*)(int(*)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr))Cyc__IO_vfprintf)(Cyc_putc,f,fmt,ap);
return ans;}
# 969
int Cyc_fprintf(struct Cyc___cycFILE*f,struct _fat_ptr fmt,struct _fat_ptr ap){
# 973
return Cyc_vfprintf(f,fmt,ap);}
# 976
int Cyc_vprintf(struct _fat_ptr fmt,struct _fat_ptr ap){
# 979
int ans;
ans=((int(*)(int(*)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr))Cyc__IO_vfprintf)(Cyc_putc,Cyc_stdout,fmt,ap);
return ans;}
# 984
int Cyc_printf(struct _fat_ptr fmt,struct _fat_ptr ap){
# 987
int ans;
ans=Cyc_vprintf(fmt,ap);
return ans;}struct _tuple0{struct _fat_ptr*f1;unsigned long*f2;};
# 992
static int Cyc_putc_string(int c,struct _tuple0*sptr_n){
struct _tuple0*_tmpBA=sptr_n;void*_tmpBC;void*_tmpBB;_tmpBB=_tmpBA->f1;_tmpBC=_tmpBA->f2;{struct _fat_ptr*sptr=_tmpBB;unsigned long*nptr=_tmpBC;
struct _fat_ptr s=*sptr;
unsigned long n=*nptr;
if(n == (unsigned long)0)return - 1;
*((char*)_check_fat_subscript(s,sizeof(char),0U))=(char)c;
_fat_ptr_inplace_plus(sptr,sizeof(char),1);
*nptr=n - (unsigned long)1;
return 1;}}
# 1003
int Cyc_vsnprintf(struct _fat_ptr s,unsigned long n,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1006
int ans;
struct _fat_ptr _tmpBD=s;struct _fat_ptr sptr=_tmpBD;
unsigned long _tmpBE=n;unsigned long nptr=_tmpBE;
struct _tuple0 _tmpBF=({struct _tuple0 _tmpC4;_tmpC4.f1=& sptr,_tmpC4.f2=& nptr;_tmpC4;});struct _tuple0 sptr_n=_tmpBF;
ans=((int(*)(int(*)(int,struct _tuple0*),struct _tuple0*,struct _fat_ptr,struct _fat_ptr))Cyc__IO_vfprintf)(Cyc_putc_string,& sptr_n,fmt,ap);
if(0 <= ans)
*((char*)_check_fat_subscript(s,sizeof(char),ans))='\000';
return ans;}
# 1016
int Cyc_snprintf(struct _fat_ptr s,unsigned long n,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1019
return Cyc_vsnprintf(s,n,fmt,ap);}
# 1022
int Cyc_vsprintf(struct _fat_ptr s,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1025
return Cyc_vsnprintf(s,_get_fat_size(s,sizeof(char)),fmt,ap);}
# 1028
int Cyc_sprintf(struct _fat_ptr s,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1031
return Cyc_vsnprintf(s,_get_fat_size(s,sizeof(char)),fmt,ap);}
# 1034
static int Cyc_putc_void(int c,int dummy){
return 1;}
# 1038
struct _fat_ptr Cyc_vrprintf(struct _RegionHandle*r1,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1042
int size=((int(*)(int(*)(int,int),int,struct _fat_ptr,struct _fat_ptr))Cyc__IO_vfprintf)(Cyc_putc_void,0,fmt,ap)+ 1;
struct _fat_ptr s=(struct _fat_ptr)({unsigned _tmpC0=size + 1;_tag_fat(_cyccalloc_atomic(sizeof(char),_tmpC0),sizeof(char),_tmpC0);});
({struct _fat_ptr _tmpF1=_fat_ptr_decrease_size(s,sizeof(char),1U);struct _fat_ptr _tmpF0=fmt;Cyc_vsprintf(_tmpF1,_tmpF0,ap);});
return s;}
# 1048
struct _fat_ptr Cyc_rprintf(struct _RegionHandle*r1,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1051
return Cyc_vrprintf(r1,fmt,ap);}
# 1054
struct _fat_ptr Cyc_aprintf(struct _fat_ptr fmt,struct _fat_ptr ap){
# 1057
return Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);}
