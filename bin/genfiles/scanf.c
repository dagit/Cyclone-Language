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
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;
# 52 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdin;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 90
int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 142 "cycboot.h"
int Cyc_getc(struct Cyc___cycFILE*);
# 222 "cycboot.h"
int Cyc_ungetc(int,struct Cyc___cycFILE*);
# 247
int Cyc_vsscanf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 300 "cycboot.h"
int isspace(int);
# 302
int isupper(int);
# 314
double atof(const char*);
long strtol(char*,char**,int);
# 317
unsigned long strtoul(char*,char**,int);
# 126 "scanf.cyc"
static struct _fat_ptr Cyc___sccl(char*tab,struct _fat_ptr fmt);
# 139
static short*Cyc_va_arg_short_ptr(void*a){
void*_tmp0=a;unsigned short*_tmp1;short*_tmp2;switch(*((int*)_tmp0)){case 0U: _LL1: _tmp2=((struct Cyc_ShortPtr_sa_ScanfArg_struct*)_tmp0)->f1;_LL2: {short*p=_tmp2;
return p;}case 1U: _LL3: _tmp1=((struct Cyc_UShortPtr_sa_ScanfArg_struct*)_tmp0)->f1;_LL4: {unsigned short*p=_tmp1;
return(short*)p;}default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp4=_cycalloc(sizeof(*_tmp4));_tmp4->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp37=({const char*_tmp3="scan expects short pointer";_tag_fat(_tmp3,sizeof(char),27U);});_tmp4->f1=_tmp37;});_tmp4;}));}_LL0:;}
# 147
static int*Cyc_va_arg_int_ptr(void*a){
void*_tmp5=a;unsigned*_tmp6;int*_tmp7;switch(*((int*)_tmp5)){case 2U: _LL1: _tmp7=((struct Cyc_IntPtr_sa_ScanfArg_struct*)_tmp5)->f1;_LL2: {int*p=_tmp7;
return p;}case 3U: _LL3: _tmp6=((struct Cyc_UIntPtr_sa_ScanfArg_struct*)_tmp5)->f1;_LL4: {unsigned*p=_tmp6;
return(int*)p;}default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp38=({const char*_tmp8="scan expects int pointer";_tag_fat(_tmp8,sizeof(char),25U);});_tmp9->f1=_tmp38;});_tmp9;}));}_LL0:;}
# 155
static struct _fat_ptr Cyc_va_arg_string_ptr(void*a){
void*_tmpA=a;struct _fat_ptr _tmpB;struct _fat_ptr _tmpC;switch(*((int*)_tmpA)){case 4U: _LL1: _tmpC=((struct Cyc_StringPtr_sa_ScanfArg_struct*)_tmpA)->f1;_LL2: {struct _fat_ptr p=_tmpC;
return _fat_ptr_decrease_size(p,sizeof(char),1U);}case 7U: _LL3: _tmpB=((struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmpA)->f1;_LL4: {struct _fat_ptr p=_tmpB;
return p;}default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp39=({const char*_tmpD="scan expects char pointer";_tag_fat(_tmpD,sizeof(char),26U);});_tmpE->f1=_tmp39;});_tmpE;}));}_LL0:;}
# 163
static double*Cyc_va_arg_double_ptr(void*a){
void*_tmpF=a;double*_tmp10;if(((struct Cyc_DoublePtr_sa_ScanfArg_struct*)_tmpF)->tag == 5U){_LL1: _tmp10=((struct Cyc_DoublePtr_sa_ScanfArg_struct*)_tmpF)->f1;_LL2: {double*p=_tmp10;
return p;}}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp3A=({const char*_tmp11="scan expects double pointer";_tag_fat(_tmp11,sizeof(char),28U);});_tmp12->f1=_tmp3A;});_tmp12;}));}_LL0:;}
# 170
static float*Cyc_va_arg_float_ptr(void*a){
void*_tmp13=a;float*_tmp14;if(((struct Cyc_FloatPtr_sa_ScanfArg_struct*)_tmp13)->tag == 6U){_LL1: _tmp14=((struct Cyc_FloatPtr_sa_ScanfArg_struct*)_tmp13)->f1;_LL2: {float*p=_tmp14;
return p;}}else{_LL3: _LL4:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp3B=({const char*_tmp15="scan expects float pointer";_tag_fat(_tmp15,sizeof(char),27U);});_tmp16->f1=_tmp3B;});_tmp16;}));}_LL0:;}
# 177
static struct _fat_ptr Cyc_va_arg_char_ptr(void*a){
void*_tmp17=a;struct _fat_ptr _tmp18;struct _fat_ptr _tmp19;switch(*((int*)_tmp17)){case 7U: _LL1: _tmp19=((struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmp17)->f1;_LL2: {struct _fat_ptr p=_tmp19;
return p;}case 4U: _LL3: _tmp18=((struct Cyc_StringPtr_sa_ScanfArg_struct*)_tmp17)->f1;_LL4: {struct _fat_ptr p=_tmp18;
return _fat_ptr_decrease_size(p,sizeof(char),1U);}default: _LL5: _LL6:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp3C=({const char*_tmp1A="scan expects char pointer";_tag_fat(_tmp1A,sizeof(char),26U);});_tmp1B->f1=_tmp3C;});_tmp1B;}));}_LL0:;}
# 188
int Cyc__IO_vfscanf(int(*_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*_IO_peekc)(void*),void*fp,struct _fat_ptr fmt0,struct _fat_ptr ap,int*errp){
# 198
struct _fat_ptr fmt=fmt0;
int c;
long long width;
struct _fat_ptr p=_tag_fat(0,0,0);
int n;
int flags=0;
# 205
struct _fat_ptr p0=_tag_fat(0,0,0);
int nassigned;
int nread;
# 209
int base=0;
int use_strtoul=0;
# 213
char ccltab[256U];
# 215
char buf[351U];({{unsigned _tmp36=350U;unsigned i;for(i=0;i < _tmp36;++ i){buf[i]='0';}buf[_tmp36]=0;}0;});{
# 217
int seen_eof=0;
# 220
static short basefix[17U]={10,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
# 223
nassigned=0;
nread=0;
for(0;1;0){
c=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
if(c == 0)
goto done;
if(isspace(c)){
for(0;1;0){
c=_IO_getc(fp);
if(c == -1){
++ seen_eof;
break;}
# 236
if(!isspace(c)){
_IO_ungetc(c,fp);
break;}
# 240
++ nread;}
# 242
continue;}
# 244
if(c != (int)'%')
goto literal;
width=(long long)0;
flags=0;
# 252
again: c=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
{int _tmp1C=c;switch(_tmp1C){case 37U: _LL1: _LL2:
# 255
 literal:
 n=_IO_getc(fp);
if(n == -1)
goto eof_failure;
if(n != c){
_IO_ungetc(n,fp);
goto match_failure;}
# 263
++ nread;
continue;case 42U: _LL3: _LL4:
# 267
 if(flags)goto control_failure;
flags=8;
goto again;case 108U: _LL5: _LL6:
# 271
 if(flags & ~(8 | 64))goto control_failure;
flags |=1;
goto again;case 76U: _LL7: _LL8:
# 275
 if(flags & ~(8 | 64))goto control_failure;
flags |=2;
goto again;case 104U: _LL9: _LLA:
# 279
 if(flags & ~(8 | 64))goto control_failure;
flags |=4;
goto again;case 48U: _LLB: _LLC:
# 283
 goto _LLE;case 49U: _LLD: _LLE: goto _LL10;case 50U: _LLF: _LL10: goto _LL12;case 51U: _LL11: _LL12: goto _LL14;case 52U: _LL13: _LL14: goto _LL16;case 53U: _LL15: _LL16:
 goto _LL18;case 54U: _LL17: _LL18: goto _LL1A;case 55U: _LL19: _LL1A: goto _LL1C;case 56U: _LL1B: _LL1C: goto _LL1E;case 57U: _LL1D: _LL1E:
 if(flags & ~(8 | 64))goto control_failure;
flags |=64;
width=(width * (long long)10 + (long long)c)- (long long)'0';
goto again;case 68U: _LL1F: _LL20:
# 298 "scanf.cyc"
 flags |=1;
goto _LL22;case 100U: _LL21: _LL22:
# 301
 c=3;
use_strtoul=0;
base=10;
goto _LL0;case 105U: _LL23: _LL24:
# 307
 c=3;
use_strtoul=0;
base=0;
goto _LL0;case 79U: _LL25: _LL26:
# 313
 flags |=1;
goto _LL28;case 111U: _LL27: _LL28:
# 316
 c=3;
use_strtoul=1;
base=8;
goto _LL0;case 117U: _LL29: _LL2A:
# 322
 c=3;
use_strtoul=1;
base=10;
goto _LL0;case 88U: _LL2B: _LL2C:
# 327
 goto _LL2E;case 120U: _LL2D: _LL2E:
 flags |=256;
c=3;
use_strtoul=1;
base=16;
goto _LL0;case 69U: _LL2F: _LL30:
# 334
 goto _LL32;case 70U: _LL31: _LL32: goto _LL34;case 101U: _LL33: _LL34: goto _LL36;case 102U: _LL35: _LL36: goto _LL38;case 103U: _LL37: _LL38:
 c=4;
goto _LL0;case 115U: _LL39: _LL3A:
# 339
 c=2;
goto _LL0;case 91U: _LL3B: _LL3C:
# 343
 fmt=Cyc___sccl(ccltab,fmt);
flags |=32;
c=1;
goto _LL0;case 99U: _LL3D: _LL3E:
# 349
 flags |=32;
c=0;
goto _LL0;case 112U: _LL3F: _LL40:
# 354
 flags |=16 | 256;
c=3;
use_strtoul=1;
base=16;
goto _LL0;case 110U: _LL41: _LL42:
# 361
 if(flags & 8)
continue;
if(flags & 4)
({short _tmp3D=(short)nread;*Cyc_va_arg_short_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp3D;});else{
if(flags & 1)
({long _tmp3E=nread;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp3E;});else{
# 368
({int _tmp3F=nread;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp3F;});}}
_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
continue;case 0U: _LL43: _LL44:
# 376
 nassigned=-1;
goto done;default: _LL45: _LL46:
# 380
 if(isupper(c))
flags |=1;
c=3;
use_strtoul=0;
base=10;
goto _LL0;}_LL0:;}
# 391
if(_IO_peekc(fp)== - 1)
goto eof_failure;
# 398
if((flags & 32)== 0){
n=_IO_peekc(fp);
while(isspace(n)){
n=_IO_getc(fp);
++ nread;
n=_IO_peekc(fp);
if(n == -1)
goto eof_failure;}}{
# 415
int _tmp1D=c;switch(_tmp1D){case 0U: _LL48: _LL49:
# 421
 if(width == (long long)0)
width=(long long)1;
if(flags & 8){
long long sum=(long long)0;
for(0;width > (long long)0;0){
n=_IO_getc(fp);
if(n == -1 && width != (long long)0)
goto eof_failure;else{
if(n == -1){
++ seen_eof;
break;}}
# 433
++ sum;
-- width;}
# 436
nread +=sum;}else{
# 438
long long sum=(long long)0;
struct _fat_ptr _tmp1E=Cyc_va_arg_char_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)));struct _fat_ptr z=_tmp1E;_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
for(0;width > (long long)0;0){
n=_IO_getc(fp);
if(n == -1 && width != (long long)0)
goto eof_failure;else{
if(n == -1){
++ seen_eof;
break;}}
# 448
*((char*)_check_fat_subscript(z,sizeof(char),0U))=(char)n;
_fat_ptr_inplace_plus(& z,sizeof(char),1);
++ sum;
-- width;}
# 453
nread +=sum;
++ nassigned;}
# 456
goto _LL47;case 1U: _LL4A: _LL4B:
# 460
 if(width == (long long)0)
width=(long long)4294967295U;
# 463
if(flags & 8){
n=0;{
int c=_IO_peekc(fp);
while((int)*((char*)_check_known_subscript_notnull(ccltab,256U,sizeof(char),(int)((char)c)))){
++ n;
_IO_getc(fp);
if(-- width == (long long)0)
break;
if((c=_IO_peekc(fp))== - 1){
if(n == 0)
goto eof_failure;
++ seen_eof;
break;}}
# 478
if(n == 0)
goto match_failure;}}else{
# 481
struct _fat_ptr p4=(struct _fat_ptr)Cyc_va_arg_string_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);{
struct _fat_ptr p5=p4;
int c=_IO_peekc(fp);
while((int)*((char*)_check_known_subscript_notnull(ccltab,256U,sizeof(char),(int)((char)c)))){
if(_get_fat_size(p5,sizeof(char))== (unsigned)0)goto eof_failure;
*((char*)_check_fat_subscript(p5,sizeof(char),0U))=(char)c;
_fat_ptr_inplace_plus(& p5,sizeof(char),1);
_IO_getc(fp);
if(-- width == (long long)0)
break;
if((c=_IO_peekc(fp))== - 1){
if((char*)p5.curr == (char*)p0.curr)
goto eof_failure;
++ seen_eof;
break;}}
# 498
n=(p5.curr - p4.curr)/ sizeof(char);
if(n == 0)
goto match_failure;
if(_get_fat_size(p5,sizeof(char))== (unsigned)0)goto eof_failure;
*((char*)_check_fat_subscript(p5,sizeof(char),0U))='\000';
++ nassigned;}}
# 505
nread +=n;
goto _LL47;case 2U: _LL4C: _LL4D:
# 510
 if(width == (long long)0)
width=(long long)4294967295U;
if(flags & 8){
n=0;{
int c=_IO_peekc(fp);
while(!isspace((int)((unsigned char)c))){
++ n;
_IO_getc(fp);
if(-- width == (long long)0)
break;
if((c=_IO_peekc(fp))== - 1){
++ seen_eof;
break;}}
# 525
nread +=n;}}else{
# 527
struct _fat_ptr _tmp1F=Cyc_va_arg_string_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)));struct _fat_ptr p2=_tmp1F;_fat_ptr_inplace_plus(& ap,sizeof(void*),1);{
struct _fat_ptr _tmp20=p2;struct _fat_ptr p3=_tmp20;
int c=_IO_peekc(fp);
while(!isspace((int)((unsigned char)c))){
c=_IO_getc(fp);
if(_get_fat_size(p3,sizeof(char))== (unsigned)0)goto eof_failure;
*((char*)_check_fat_subscript(p3,sizeof(char),0U))=(char)c;
_fat_ptr_inplace_plus(& p3,sizeof(char),1);
if(-- width == (long long)0)
break;
if((c=_IO_peekc(fp))== - 1){
++ seen_eof;
break;}}
# 542
if(_get_fat_size(p3,sizeof(char))== (unsigned)0)goto eof_failure;
*((char*)_check_fat_subscript(p3,sizeof(char),0U))='\000';
nread +=(p3.curr - p2.curr)/ sizeof(char);
++ nassigned;}}
# 547
continue;case 3U: _LL4E: _LL4F:
# 551
 if(width == (long long)0 || width > (long long)(sizeof(buf)- (unsigned)1))
width=(long long)(sizeof(buf)- 1U);
flags |=(64 | 128)| 512;
for(p=({char*_tmp21=buf;_tag_fat(_tmp21,sizeof(char),351U);});width != (long long)0;-- width){
c=(int)((unsigned char)_IO_peekc(fp));
# 560
{int _tmp22=c;switch(_tmp22){case 48U: _LL55: _LL56:
# 575 "scanf.cyc"
 if(base == 0){
base=8;
flags |=256;}
# 579
if(flags & 512)
flags &=~((64 | 512)| 128);else{
# 582
flags &=~((64 | 256)| 128);}
goto ok;case 49U: _LL57: _LL58:
# 586
 goto _LL5A;case 50U: _LL59: _LL5A: goto _LL5C;case 51U: _LL5B: _LL5C: goto _LL5E;case 52U: _LL5D: _LL5E: goto _LL60;case 53U: _LL5F: _LL60:
 goto _LL62;case 54U: _LL61: _LL62: goto _LL64;case 55U: _LL63: _LL64:
 base=(int)*((short*)_check_known_subscript_notnull(basefix,17U,sizeof(short),base));
flags &=~((64 | 256)| 128);
goto ok;case 56U: _LL65: _LL66:
# 593
 goto _LL68;case 57U: _LL67: _LL68:
 base=(int)*((short*)_check_known_subscript_notnull(basefix,17U,sizeof(short),base));
if(base <= 8)
goto _LL54;
flags &=~((64 | 256)| 128);
goto ok;case 65U: _LL69: _LL6A:
# 601
 goto _LL6C;case 66U: _LL6B: _LL6C: goto _LL6E;case 67U: _LL6D: _LL6E: goto _LL70;case 68U: _LL6F: _LL70: goto _LL72;case 69U: _LL71: _LL72:
 goto _LL74;case 70U: _LL73: _LL74: goto _LL76;case 97U: _LL75: _LL76: goto _LL78;case 98U: _LL77: _LL78: goto _LL7A;case 99U: _LL79: _LL7A:
 goto _LL7C;case 100U: _LL7B: _LL7C: goto _LL7E;case 101U: _LL7D: _LL7E: goto _LL80;case 102U: _LL7F: _LL80:
# 605
 if(base <= 10)
goto _LL54;
flags &=~((64 | 256)| 128);
goto ok;case 43U: _LL81: _LL82:
# 611
 goto _LL84;case 45U: _LL83: _LL84:
 if(flags & 64){
flags &=~ 64;
goto ok;}
# 616
goto _LL54;case 120U: _LL85: _LL86:
# 619
 goto _LL88;case 88U: _LL87: _LL88:
 if(flags & 256 &&({char*_tmp40=(char*)(_fat_ptr_plus(p,sizeof(char),- 1)).curr;_tmp40 == buf;})){
base=16;
flags &=~ 256;
goto ok;}
# 625
goto _LL54;default: _LL89: _LL8A:
# 627
 goto _LL54;}_LL54:;}
# 635
break;
ok:
# 640
({struct _fat_ptr _tmp23=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp24=*((char*)_check_fat_subscript(_tmp23,sizeof(char),0U));char _tmp25=(char)c;if(_get_fat_size(_tmp23,sizeof(char))== 1U &&(_tmp24 == 0 && _tmp25 != 0))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});
_IO_getc(fp);
if(_IO_peekc(fp)== - 1){
++ seen_eof;
break;}}
# 653
if(flags & 128){
if((char*)p.curr > buf){
_fat_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc((int)*((char*)_check_fat_subscript(p,sizeof(char),0U)),fp);}
# 658
goto match_failure;}
# 660
c=(int)*((char*)_check_fat_subscript(p,sizeof(char),-1));
if(c == (int)'x' || c == (int)'X'){
_fat_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc(c,fp);}
# 665
if((flags & 8)== 0){
unsigned long res;
# 668
({struct _fat_ptr _tmp26=p;char _tmp27=*((char*)_check_fat_subscript(_tmp26,sizeof(char),0U));char _tmp28='\000';if(_get_fat_size(_tmp26,sizeof(char))== 1U &&(_tmp27 == 0 && _tmp28 != 0))_throw_arraybounds();*((char*)_tmp26.curr)=_tmp28;});
if(use_strtoul)
res=strtoul(buf,0,base);else{
# 672
res=(unsigned long)strtol(buf,0,base);}
if(flags & 16)
({int _tmp41=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp41;});else{
if(flags & 4)
({short _tmp42=(short)res;*Cyc_va_arg_short_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp42;});else{
if(flags & 1)
({int _tmp43=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp43;});else{
# 680
({int _tmp44=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp44;});}}}
_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
++ nassigned;}
# 684
({int _tmp46=({unsigned char*_tmp45=p.curr;_tmp45 - ({char*_tmp29=buf;_tag_fat(_tmp29,sizeof(char),351U);}).curr;})/ sizeof(char);nread +=_tmp46;});
goto _LL47;case 4U: _LL50: _LL51:
# 689
 if(width == (long long)0 || width > (long long)(sizeof(buf)- (unsigned)1))
width=(long long)(sizeof(buf)- 1U);
flags |=((64 | 128)| 256)| 512;
for(p=({char*_tmp2A=buf;_tag_fat(_tmp2A,sizeof(char),351U);});width != (long long)0;-- width){
c=_IO_peekc(fp);
# 698
{int _tmp2B=c;switch(_tmp2B){case 48U: _LL8C: _LL8D:
# 700
 goto _LL8F;case 49U: _LL8E: _LL8F: goto _LL91;case 50U: _LL90: _LL91: goto _LL93;case 51U: _LL92: _LL93: goto _LL95;case 52U: _LL94: _LL95:
 goto _LL97;case 53U: _LL96: _LL97: goto _LL99;case 54U: _LL98: _LL99: goto _LL9B;case 55U: _LL9A: _LL9B: goto _LL9D;case 56U: _LL9C: _LL9D:
 goto _LL9F;case 57U: _LL9E: _LL9F:
 flags &=~(64 | 128);
goto fok;case 43U: _LLA0: _LLA1:
# 706
 goto _LLA3;case 45U: _LLA2: _LLA3:
 if(flags & 64){
flags &=~ 64;
goto fok;}
# 711
goto _LL8B;case 46U: _LLA4: _LLA5:
# 713
 if(flags & 256){
flags &=~(64 | 256);
goto fok;}
# 717
goto _LL8B;case 101U: _LLA6: _LLA7:
 goto _LLA9;case 69U: _LLA8: _LLA9:
# 720
 if((flags & (128 | 512))== 512){
flags=(flags & ~(512 | 256)| 64)| 128;
# 724
goto fok;}
# 726
goto _LL8B;default: _LLAA: _LLAB:
# 728
 goto _LL8B;}_LL8B:;}
# 730
break;
fok:
({struct _fat_ptr _tmp2C=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp2D=*((char*)_check_fat_subscript(_tmp2C,sizeof(char),0U));char _tmp2E=(char)c;if(_get_fat_size(_tmp2C,sizeof(char))== 1U &&(_tmp2D == 0 && _tmp2E != 0))_throw_arraybounds();*((char*)_tmp2C.curr)=_tmp2E;});
_IO_getc(fp);
if(_IO_peekc(fp)== - 1){
++ seen_eof;
break;}}
# 744
if(flags & 128){
if(flags & 512){
# 747
while((char*)p.curr > buf){
_fat_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc((int)*((char*)_check_fat_subscript(p,sizeof(char),0U)),fp);}
# 751
goto match_failure;}
# 754
_fat_ptr_inplace_plus(& p,sizeof(char),-1);
c=(int)*((char*)_check_fat_subscript(p,sizeof(char),0U));
if(c != (int)'e' && c != (int)'E'){
_IO_ungetc(c,fp);
_fat_ptr_inplace_plus(& p,sizeof(char),-1);
c=(int)*((char*)_check_fat_subscript(p,sizeof(char),0U));}
# 761
_IO_ungetc(c,fp);}
# 763
if((flags & 8)== 0){
double res;
({struct _fat_ptr _tmp2F=p;char _tmp30=*((char*)_check_fat_subscript(_tmp2F,sizeof(char),0U));char _tmp31='\000';if(_get_fat_size(_tmp2F,sizeof(char))== 1U &&(_tmp30 == 0 && _tmp31 != 0))_throw_arraybounds();*((char*)_tmp2F.curr)=_tmp31;});
res=atof((const char*)buf);
if(flags & 1)
({double _tmp47=res;*Cyc_va_arg_double_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp47;});else{
# 770
({float _tmp48=(float)res;*Cyc_va_arg_float_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp48;});}
_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
++ nassigned;}
# 774
({int _tmp4A=({unsigned char*_tmp49=p.curr;_tmp49 - ({char*_tmp32=buf;_tag_fat(_tmp32,sizeof(char),351U);}).curr;})/ sizeof(char);nread +=_tmp4A;});
goto _LL47;default: _LL52: _LL53:
# 777
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp4B=({const char*_tmp33="scanf3";_tag_fat(_tmp33,sizeof(char),7U);});_tmp34->f1=_tmp4B;});_tmp34;}));}_LL47:;}}
# 781
eof_failure:
 ++ seen_eof;
input_failure:
 if(nassigned == 0)
nassigned=-1;
control_failure:
 match_failure:
 if((unsigned)errp)
*errp |=2;
done:
 if((unsigned)errp && seen_eof)
*errp |=1;
return nassigned;}}
# 802
static struct _fat_ptr Cyc___sccl(char*tab,struct _fat_ptr fmt){
# 807
int c;int n;int v;
# 810
c=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
if(c == (int)'^'){
v=1;
c=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));}else{
# 815
v=0;}
# 817
for(n=0;n < 256;++ n){
*((char*)_check_known_subscript_notnull(tab,256U,sizeof(char),n))=(char)v;}
if(c == 0)
return _fat_ptr_plus(fmt,sizeof(char),- 1);
# 829 "scanf.cyc"
v=1 - v;
for(0;1;0){
*((char*)_check_known_subscript_notnull(tab,256U,sizeof(char),c))=(char)v;
doswitch:
 n=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));{
int _tmp35=n;switch(_tmp35){case 0U: _LL1: _LL2:
# 837
 return _fat_ptr_plus(fmt,sizeof(char),- 1);case 45U: _LL3: _LL4:
# 858 "scanf.cyc"
 n=(int)*((const char*)_check_fat_subscript(fmt,sizeof(char),0U));
if(n == (int)']' || n < c){
c=(int)'-';
goto _LL0;}
# 863
_fat_ptr_inplace_plus(& fmt,sizeof(char),1);
do{
*((char*)_check_known_subscript_notnull(tab,256U,sizeof(char),++ c))=(char)v;}while(c < n);
# 873
goto doswitch;
# 881
goto _LL0;case 93U: _LL5: _LL6:
# 884
 return fmt;default: _LL7: _LL8:
# 887
 c=n;
goto _LL0;}_LL0:;}}}
# 896
static int Cyc_string_getc(struct _fat_ptr*sptr){
char c;
struct _fat_ptr s=*sptr;
if((({char*_tmp4C=(char*)s.curr;_tmp4C == (char*)(_tag_fat(0,0,0)).curr;})|| _get_fat_size(s,sizeof(char))== (unsigned)0)||(int)(c=*((const char*)_check_fat_subscript(s,sizeof(char),0)))== (int)'\000')return - 1;
({struct _fat_ptr _tmp4D=_fat_ptr_plus(s,sizeof(char),1);*sptr=_tmp4D;});
return(int)c;}
# 904
static int Cyc_string_ungetc(int ignore,struct _fat_ptr*sptr){
({struct _fat_ptr _tmp4E=_fat_ptr_plus(*sptr,sizeof(char),- 1);*sptr=_tmp4E;});
# 907
return 0;}
# 910
static int Cyc_string_peekc(struct _fat_ptr*sptr){
char c;
struct _fat_ptr s=*sptr;
if((({char*_tmp4F=(char*)s.curr;_tmp4F == (char*)(_tag_fat(0,0,0)).curr;})|| _get_fat_size(s,sizeof(char))== (unsigned)0)||(int)(c=*((const char*)_check_fat_subscript(s,sizeof(char),0)))== (int)'\000')return - 1;
return(int)c;}
# 917
int Cyc_vsscanf(struct _fat_ptr src1,struct _fat_ptr fmt,struct _fat_ptr ap){
# 920
struct _fat_ptr src=(struct _fat_ptr)src1;
int err=0;
return((int(*)(int(*_IO_getc)(struct _fat_ptr*),int(*_IO_ungetc)(int,struct _fat_ptr*),int(*_IO_peekc)(struct _fat_ptr*),struct _fat_ptr*fp,struct _fat_ptr fmt0,struct _fat_ptr ap,int*errp))Cyc__IO_vfscanf)(Cyc_string_getc,Cyc_string_ungetc,Cyc_string_peekc,& src,fmt,ap,& err);}
# 926
int Cyc_sscanf(struct _fat_ptr src,struct _fat_ptr fmt,struct _fat_ptr ap){
# 929
return Cyc_vsscanf(src,fmt,ap);}
# 933
int Cyc_peekc(struct Cyc___cycFILE*stream){
int c=Cyc_fgetc(stream);
Cyc_ungetc(c,stream);
return c;}
# 939
int Cyc_vfscanf(struct Cyc___cycFILE*stream,struct _fat_ptr fmt,struct _fat_ptr ap){
# 942
int err=0;
return((int(*)(int(*_IO_getc)(struct Cyc___cycFILE*),int(*_IO_ungetc)(int,struct Cyc___cycFILE*),int(*_IO_peekc)(struct Cyc___cycFILE*),struct Cyc___cycFILE*fp,struct _fat_ptr fmt0,struct _fat_ptr ap,int*errp))Cyc__IO_vfscanf)(Cyc_getc,Cyc_ungetc,Cyc_peekc,stream,fmt,ap,& err);}
# 946
int Cyc_fscanf(struct Cyc___cycFILE*stream,struct _fat_ptr fmt,struct _fat_ptr ap){
# 949
return Cyc_vfscanf(stream,fmt,ap);}
# 952
int Cyc_scanf(struct _fat_ptr fmt,struct _fat_ptr ap){
# 955
return Cyc_vfscanf(Cyc_stdin,fmt,ap);}
