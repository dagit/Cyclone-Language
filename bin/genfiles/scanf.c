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

void* Cyc_Core_get_exn_thrown();
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
#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ typeof(ptr) _cks_null = (ptr); \
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

static inline unsigned int _check_times(unsigned x, unsigned y) {
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc___cycFILE;
# 52 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdin;
# 90
extern int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 142 "cycboot.h"
extern int Cyc_getc(struct Cyc___cycFILE*);
# 222 "cycboot.h"
extern int Cyc_ungetc(int,struct Cyc___cycFILE*);
# 247
int Cyc_vsscanf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 300 "cycboot.h"
extern int isspace(int);
# 302
extern int isupper(int);
# 314
extern double atof(const char*);
extern long strtol(char*,char**,int);
# 317
extern unsigned long strtoul(char*,char**,int);
# 126 "scanf.cyc"
static struct _fat_ptr Cyc___sccl(char*,struct _fat_ptr);
# 139
static short*Cyc_va_arg_short_ptr(void*a){
void*_tmp0=a;void*_tmp1;switch(*((int*)_tmp0)){case 0: _tmp1=((struct Cyc_ShortPtr_sa_ScanfArg_struct*)_tmp0)->f1;{short*p=_tmp1;
return p;}case 1: _tmp1=((struct Cyc_UShortPtr_sa_ScanfArg_struct*)_tmp0)->f1;{unsigned short*p=_tmp1;
return(short*)p;}default:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp33=({const char*_tmp2="scan expects short pointer";_tag_fat(_tmp2,sizeof(char),27U);});_tmp3->f1=_tmp33;});_tmp3;}));};}
# 147
static int*Cyc_va_arg_int_ptr(void*a){
void*_tmp4=a;void*_tmp5;switch(*((int*)_tmp4)){case 2: _tmp5=((struct Cyc_IntPtr_sa_ScanfArg_struct*)_tmp4)->f1;{int*p=_tmp5;
return p;}case 3: _tmp5=((struct Cyc_UIntPtr_sa_ScanfArg_struct*)_tmp4)->f1;{unsigned*p=_tmp5;
return(int*)p;}default:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp7=_cycalloc(sizeof(*_tmp7));_tmp7->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp34=({const char*_tmp6="scan expects int pointer";_tag_fat(_tmp6,sizeof(char),25U);});_tmp7->f1=_tmp34;});_tmp7;}));};}
# 155
static struct _fat_ptr Cyc_va_arg_string_ptr(void*a){
void*_tmp8=a;struct _fat_ptr _tmp9;switch(*((int*)_tmp8)){case 4: _tmp9=((struct Cyc_StringPtr_sa_ScanfArg_struct*)_tmp8)->f1;{struct _fat_ptr p=_tmp9;
return _fat_ptr_decrease_size(p,sizeof(char),1U);}case 7: _tmp9=((struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmp8)->f1;{struct _fat_ptr p=_tmp9;
return p;}default:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpB=_cycalloc(sizeof(*_tmpB));_tmpB->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp35=({const char*_tmpA="scan expects char pointer";_tag_fat(_tmpA,sizeof(char),26U);});_tmpB->f1=_tmp35;});_tmpB;}));};}
# 163
static double*Cyc_va_arg_double_ptr(void*a){
void*_tmpC=a;void*_tmpD;if(*((int*)_tmpC)== 5){_tmpD=((struct Cyc_DoublePtr_sa_ScanfArg_struct*)_tmpC)->f1;{double*p=_tmpD;
return p;}}else{
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp36=({const char*_tmpE="scan expects double pointer";_tag_fat(_tmpE,sizeof(char),28U);});_tmpF->f1=_tmp36;});_tmpF;}));};}
# 170
static float*Cyc_va_arg_float_ptr(void*a){
void*_tmp10=a;void*_tmp11;if(*((int*)_tmp10)== 6){_tmp11=((struct Cyc_FloatPtr_sa_ScanfArg_struct*)_tmp10)->f1;{float*p=_tmp11;
return p;}}else{
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp37=({const char*_tmp12="scan expects float pointer";_tag_fat(_tmp12,sizeof(char),27U);});_tmp13->f1=_tmp37;});_tmp13;}));};}
# 177
static struct _fat_ptr Cyc_va_arg_char_ptr(void*a){
void*_tmp14=a;struct _fat_ptr _tmp15;switch(*((int*)_tmp14)){case 7: _tmp15=((struct Cyc_CharPtr_sa_ScanfArg_struct*)_tmp14)->f1;{struct _fat_ptr p=_tmp15;
return p;}case 4: _tmp15=((struct Cyc_StringPtr_sa_ScanfArg_struct*)_tmp14)->f1;{struct _fat_ptr p=_tmp15;
return _fat_ptr_decrease_size(p,sizeof(char),1U);}default:
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp38=({const char*_tmp16="scan expects char pointer";_tag_fat(_tmp16,sizeof(char),26U);});_tmp17->f1=_tmp38;});_tmp17;}));};}
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
char buf[351U];({{unsigned _tmp32=350U;unsigned i;for(i=0;i < _tmp32;++ i){buf[i]='0';}buf[_tmp32]=0;}0;});{
# 217
int seen_eof=0;
# 220
static short basefix[17U]={10,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
# 223
nassigned=0;
nread=0;
for(1;1;1){
c=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
if(c == 0)
goto done;
if(isspace(c)){
for(1;1;1){
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
if(c != 37)
goto literal;
width=0;
flags=0;
# 252
again: c=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
{int _tmp18=c;switch((int)_tmp18){case 37:
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
continue;case 42:
# 267
 if(flags)goto control_failure;
flags=8;
goto again;case 108:
# 271
 if(flags & 4294967223U)goto control_failure;
flags |=1;
goto again;case 76:
# 275
 if(flags & 4294967223U)goto control_failure;
flags |=2;
goto again;case 104:
# 279
 if(flags & 4294967223U)goto control_failure;
flags |=4;
goto again;case 48:
# 283
 goto _LLE;case 49: _LLE: goto _LL10;case 50: _LL10: goto _LL12;case 51: _LL12: goto _LL14;case 52: _LL14: goto _LL16;case 53: _LL16:
 goto _LL18;case 54: _LL18: goto _LL1A;case 55: _LL1A: goto _LL1C;case 56: _LL1C: goto _LL1E;case 57: _LL1E:
 if(flags & 4294967223U)goto control_failure;
flags |=64;
width=(width * 10 + (long long)c)- 48;
goto again;case 68:
# 298 "scanf.cyc"
 flags |=1;
goto _LL22;case 100: _LL22:
# 301
 c=3;
use_strtoul=0;
base=10;
goto _LL0;case 105:
# 307
 c=3;
use_strtoul=0;
base=0;
goto _LL0;case 79:
# 313
 flags |=1;
goto _LL28;case 111: _LL28:
# 316
 c=3;
use_strtoul=1;
base=8;
goto _LL0;case 117:
# 322
 c=3;
use_strtoul=1;
base=10;
goto _LL0;case 88:
# 327
 goto _LL2E;case 120: _LL2E:
 flags |=256;
c=3;
use_strtoul=1;
base=16;
goto _LL0;case 69:
# 334
 goto _LL32;case 70: _LL32: goto _LL34;case 101: _LL34: goto _LL36;case 102: _LL36: goto _LL38;case 103: _LL38:
 c=4;
goto _LL0;case 115:
# 339
 c=2;
goto _LL0;case 91:
# 343
 fmt=Cyc___sccl(ccltab,fmt);
flags |=32;
c=1;
goto _LL0;case 99:
# 349
 flags |=32;
c=0;
goto _LL0;case 112:
# 354
 flags |=272U;
c=3;
use_strtoul=1;
base=16;
goto _LL0;case 110:
# 361
 if(flags & 8)
continue;
if(flags & 4)
({short _tmp39=(short)nread;*Cyc_va_arg_short_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp39;});else{
if(flags & 1)
({long _tmp3A=nread;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp3A;});else{
# 368
({int _tmp3B=nread;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp3B;});}}
_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
continue;case 0:
# 376
 nassigned=-1;
goto done;default:
# 380
 if(isupper(c))
flags |=1;
c=3;
use_strtoul=0;
base=10;
goto _LL0;}_LL0:;}
# 391
if(_IO_peekc(fp)== -1)
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
int _tmp19=c;switch((int)_tmp19){case 0:
# 421
 if(width == 0)
width=1;
if(flags & 8){
long long sum=0;
for(1;width > 0;1){
n=_IO_getc(fp);
if(n == -1 && width != 0)
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
long long sum=0;
struct _fat_ptr _tmp1A=Cyc_va_arg_char_ptr(*((void**)ap.curr));struct _fat_ptr z=_tmp1A;_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
for(1;width > 0;1){
n=_IO_getc(fp);
if(n == -1 && width != 0)
goto eof_failure;else{
if(n == -1){
++ seen_eof;
break;}}
# 448
*((char*)z.curr)=(char)n;
_fat_ptr_inplace_plus(& z,sizeof(char),1);
++ sum;
-- width;}
# 453
nread +=sum;
++ nassigned;}
# 456
goto _LL47;case 1:
# 460
 if(width == 0)
width=-1;
# 463
if(flags & 8){
n=0;{
int c=_IO_peekc(fp);
while((int)*((char*)_check_known_subscript_notnull(ccltab,256U,sizeof(char),(int)((char)c)))){
++ n;
_IO_getc(fp);
if(-- width == 0)
break;
if((c=_IO_peekc(fp))== -1){
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
if(_get_fat_size(p5,sizeof(char))== 0U)goto eof_failure;
*((char*)p5.curr)=(char)c;
_fat_ptr_inplace_plus(& p5,sizeof(char),1);
_IO_getc(fp);
if(-- width == 0)
break;
if((c=_IO_peekc(fp))== -1){
if((char*)p5.curr == (char*)p0.curr)
goto eof_failure;
++ seen_eof;
break;}}
# 498
n=(p5.curr - p4.curr)/ sizeof(char);
if(n == 0)
goto match_failure;
if(_get_fat_size(p5,sizeof(char))== 0U)goto eof_failure;
*((char*)p5.curr)='\000';
++ nassigned;}}
# 505
nread +=n;
goto _LL47;case 2:
# 510
 if(width == 0)
width=-1;
if(flags & 8){
n=0;{
int c=_IO_peekc(fp);
while(!isspace((int)((unsigned char)c))){
++ n;
_IO_getc(fp);
if(-- width == 0)
break;
if((c=_IO_peekc(fp))== -1){
++ seen_eof;
break;}}
# 525
nread +=n;}}else{
# 527
struct _fat_ptr _tmp1B=Cyc_va_arg_string_ptr(*((void**)ap.curr));struct _fat_ptr p2=_tmp1B;_fat_ptr_inplace_plus(& ap,sizeof(void*),1);{
struct _fat_ptr _tmp1C=p2;struct _fat_ptr p3=_tmp1C;
int c=_IO_peekc(fp);
while(!isspace((int)((unsigned char)c))){
c=_IO_getc(fp);
if(_get_fat_size(p3,sizeof(char))== 0U)goto eof_failure;
*((char*)p3.curr)=(char)c;
_fat_ptr_inplace_plus(& p3,sizeof(char),1);
if(-- width == 0)
break;
if((c=_IO_peekc(fp))== -1){
++ seen_eof;
break;}}
# 542
if(_get_fat_size(p3,sizeof(char))== 0U)goto eof_failure;
*((char*)p3.curr)='\000';
nread +=(p3.curr - p2.curr)/ sizeof(char);
++ nassigned;}}
# 547
continue;case 3:
# 551
 if(width == 0 || width > (long long)(sizeof(buf)- 1U))
width=(long long)(sizeof(buf)- 1U);
flags |=704U;
for(p=({char*_tmp1D=buf;_tag_fat(_tmp1D,sizeof(char),351U);});width != 0;-- width){
c=(int)((unsigned char)_IO_peekc(fp));
# 560
{int _tmp1E=c;switch((int)_tmp1E){case 48:
# 575 "scanf.cyc"
 if(base == 0){
base=8;
flags |=256;}
# 579
if(flags & 512)
flags &=4294966591U;else{
# 582
flags &=4294966847U;}
goto ok;case 49:
# 586
 goto _LL5A;case 50: _LL5A: goto _LL5C;case 51: _LL5C: goto _LL5E;case 52: _LL5E: goto _LL60;case 53: _LL60:
 goto _LL62;case 54: _LL62: goto _LL64;case 55: _LL64:
 base=(int)*((short*)_check_known_subscript_notnull(basefix,17U,sizeof(short),base));
flags &=4294966847U;
goto ok;case 56:
# 593
 goto _LL68;case 57: _LL68:
 base=(int)*((short*)_check_known_subscript_notnull(basefix,17U,sizeof(short),base));
if(base <= 8)
goto _LL54;
flags &=4294966847U;
goto ok;case 65:
# 601
 goto _LL6C;case 66: _LL6C: goto _LL6E;case 67: _LL6E: goto _LL70;case 68: _LL70: goto _LL72;case 69: _LL72:
 goto _LL74;case 70: _LL74: goto _LL76;case 97: _LL76: goto _LL78;case 98: _LL78: goto _LL7A;case 99: _LL7A:
 goto _LL7C;case 100: _LL7C: goto _LL7E;case 101: _LL7E: goto _LL80;case 102: _LL80:
# 605
 if(base <= 10)
goto _LL54;
flags &=4294966847U;
goto ok;case 43:
# 611
 goto _LL84;case 45: _LL84:
 if(flags & 64){
flags &=4294967231U;
goto ok;}
# 616
goto _LL54;case 120:
# 619
 goto _LL88;case 88: _LL88:
 if(flags & 256 &&({char*_tmp3C=(char*)(_fat_ptr_plus(p,sizeof(char),- 1)).curr;_tmp3C == buf;})){
base=16;
flags &=4294967039U;
goto ok;}
# 625
goto _LL54;default:
# 627
 goto _LL54;}_LL54:;}
# 635
break;
ok:
# 640
({struct _fat_ptr _tmp1F=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp20=*((char*)_check_fat_subscript(_tmp1F,sizeof(char),0U));char _tmp21=(char)c;if(_get_fat_size(_tmp1F,sizeof(char))== 1U &&(_tmp20 == 0 && _tmp21 != 0))_throw_arraybounds();*((char*)_tmp1F.curr)=_tmp21;});
_IO_getc(fp);
if(_IO_peekc(fp)== -1){
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
if(c == 120 || c == 88){
_fat_ptr_inplace_plus(& p,sizeof(char),-1);
_IO_ungetc(c,fp);}
# 665
if((flags & 8)== 0){
unsigned long res;
# 668
({struct _fat_ptr _tmp22=p;char _tmp23=*((char*)_check_fat_subscript(_tmp22,sizeof(char),0U));char _tmp24='\000';if(_get_fat_size(_tmp22,sizeof(char))== 1U &&(_tmp23 == 0 && _tmp24 != 0))_throw_arraybounds();*((char*)_tmp22.curr)=_tmp24;});
if(use_strtoul)
res=strtoul(buf,0,base);else{
# 672
res=(unsigned long)strtol(buf,0,base);}
if(flags & 16)
({int _tmp3D=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp3D;});else{
if(flags & 4)
({short _tmp3E=(short)res;*Cyc_va_arg_short_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp3E;});else{
if(flags & 1)
({int _tmp3F=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp3F;});else{
# 680
({int _tmp40=(int)res;*Cyc_va_arg_int_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp40;});}}}
_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
++ nassigned;}
# 684
({int _tmp42=({unsigned char*_tmp41=p.curr;_tmp41 - ({char*_tmp25=buf;_tag_fat(_tmp25,sizeof(char),351U);}).curr;})/ sizeof(char);nread +=_tmp42;});
goto _LL47;case 4:
# 689
 if(width == 0 || width > (long long)(sizeof(buf)- 1U))
width=(long long)(sizeof(buf)- 1U);
flags |=960U;
for(p=({char*_tmp26=buf;_tag_fat(_tmp26,sizeof(char),351U);});width != 0;-- width){
c=_IO_peekc(fp);
# 698
{int _tmp27=c;switch((int)_tmp27){case 48:
# 700
 goto _LL8F;case 49: _LL8F: goto _LL91;case 50: _LL91: goto _LL93;case 51: _LL93: goto _LL95;case 52: _LL95:
 goto _LL97;case 53: _LL97: goto _LL99;case 54: _LL99: goto _LL9B;case 55: _LL9B: goto _LL9D;case 56: _LL9D:
 goto _LL9F;case 57: _LL9F:
 flags &=4294967103U;
goto fok;case 43:
# 706
 goto _LLA3;case 45: _LLA3:
 if(flags & 64){
flags &=4294967231U;
goto fok;}
# 711
goto _LL8B;case 46:
# 713
 if(flags & 256){
flags &=4294966975U;
goto fok;}
# 717
goto _LL8B;case 101:
 goto _LLA9;case 69: _LLA9:
# 720
 if((flags & 640U)== 512){
flags=(flags & 4294966527U | 64)| 128;
# 724
goto fok;}
# 726
goto _LL8B;default:
# 728
 goto _LL8B;}_LL8B:;}
# 730
break;
fok:
({struct _fat_ptr _tmp28=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _tmp29=*((char*)_check_fat_subscript(_tmp28,sizeof(char),0U));char _tmp2A=(char)c;if(_get_fat_size(_tmp28,sizeof(char))== 1U &&(_tmp29 == 0 && _tmp2A != 0))_throw_arraybounds();*((char*)_tmp28.curr)=_tmp2A;});
_IO_getc(fp);
if(_IO_peekc(fp)== -1){
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
if(c != 101 && c != 69){
_IO_ungetc(c,fp);
_fat_ptr_inplace_plus(& p,sizeof(char),-1);
c=(int)*((char*)_check_fat_subscript(p,sizeof(char),0U));}
# 761
_IO_ungetc(c,fp);}
# 763
if((flags & 8)== 0){
double res;
({struct _fat_ptr _tmp2B=p;char _tmp2C=*((char*)_check_fat_subscript(_tmp2B,sizeof(char),0U));char _tmp2D='\000';if(_get_fat_size(_tmp2B,sizeof(char))== 1U &&(_tmp2C == 0 && _tmp2D != 0))_throw_arraybounds();*((char*)_tmp2B.curr)=_tmp2D;});
res=atof((const char*)buf);
if(flags & 1)
({double _tmp43=res;*Cyc_va_arg_double_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp43;});else{
# 770
({float _tmp44=(float)res;*Cyc_va_arg_float_ptr(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)))=_tmp44;});}
_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
++ nassigned;}
# 774
({int _tmp46=({unsigned char*_tmp45=p.curr;_tmp45 - ({char*_tmp2E=buf;_tag_fat(_tmp2E,sizeof(char),351U);}).curr;})/ sizeof(char);nread +=_tmp46;});
goto _LL47;default:
# 777
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp30=_cycalloc(sizeof(*_tmp30));_tmp30->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp47=({const char*_tmp2F="scanf3";_tag_fat(_tmp2F,sizeof(char),7U);});_tmp30->f1=_tmp47;});_tmp30;}));}_LL47:;}}
# 781
eof_failure:
 ++ seen_eof;

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
if(c == 94){
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
for(1;1;1){
*((char*)_check_known_subscript_notnull(tab,256U,sizeof(char),c))=(char)v;
doswitch:
 n=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));{
int _tmp31=n;switch((int)_tmp31){case 0:
# 837
 return _fat_ptr_plus(fmt,sizeof(char),- 1);case 45:
# 858 "scanf.cyc"
 n=(int)*((const char*)fmt.curr);
if(n == 93 || n < c){
c=45;
goto _LL0;}
# 863
_fat_ptr_inplace_plus(& fmt,sizeof(char),1);
do{
tab[++ c]=(char)v;}while(c < n);
# 873
goto doswitch;
# 881
goto _LL0;case 93:
# 884
 return fmt;default:
# 887
 c=n;
goto _LL0;}_LL0:;}}}
# 896
static int Cyc_string_getc(struct _fat_ptr*sptr){
char c;
struct _fat_ptr s=*sptr;
if(((char*)s.curr == (char*)(_tag_fat(0,0,0)).curr || _get_fat_size(s,sizeof(char))== 0U)||(int)(c=((const char*)s.curr)[0])== 0)return -1;
({struct _fat_ptr _tmp48=_fat_ptr_plus(s,sizeof(char),1);*sptr=_tmp48;});
return(int)c;}
# 904
static int Cyc_string_ungetc(int ignore,struct _fat_ptr*sptr){
({struct _fat_ptr _tmp49=_fat_ptr_plus(*sptr,sizeof(char),- 1);*sptr=_tmp49;});
# 907
return 0;}
# 910
static int Cyc_string_peekc(struct _fat_ptr*sptr){
char c;
struct _fat_ptr s=*sptr;
if(((char*)s.curr == (char*)(_tag_fat(0,0,0)).curr || _get_fat_size(s,sizeof(char))== 0U)||(int)(c=((const char*)s.curr)[0])== 0)return -1;
return(int)c;}
# 917
int Cyc_vsscanf(struct _fat_ptr src1,struct _fat_ptr fmt,struct _fat_ptr ap){
# 920
struct _fat_ptr src=(struct _fat_ptr)src1;
int err=0;
return({(int(*)(int(*)(struct _fat_ptr*),int(*)(int,struct _fat_ptr*),int(*)(struct _fat_ptr*),struct _fat_ptr*,struct _fat_ptr,struct _fat_ptr,int*))Cyc__IO_vfscanf;})(Cyc_string_getc,Cyc_string_ungetc,Cyc_string_peekc,& src,fmt,ap,& err);}
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
return({(int(*)(int(*)(struct Cyc___cycFILE*),int(*)(int,struct Cyc___cycFILE*),int(*)(struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr,int*))Cyc__IO_vfscanf;})(Cyc_getc,Cyc_ungetc,Cyc_peekc,stream,fmt,ap,& err);}
# 946
int Cyc_fscanf(struct Cyc___cycFILE*stream,struct _fat_ptr fmt,struct _fat_ptr ap){
# 949
return Cyc_vfscanf(stream,fmt,ap);}
# 952
int Cyc_scanf(struct _fat_ptr fmt,struct _fat_ptr ap){
# 955
return Cyc_vfscanf(Cyc_stdin,fmt,ap);}
