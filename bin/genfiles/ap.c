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
 extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];
# 300 "cycboot.h"
extern int isspace(int);
# 339
extern const long Cyc_long_max;extern const long Cyc_long_min;
# 22 "ctype.h"
extern int isspace(int);extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];
# 81 "string.h"
extern struct _fat_ptr Cyc__memcpy(struct _fat_ptr,struct _fat_ptr,unsigned long,unsigned);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);
# 7 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_zero;
extern struct Cyc_AP_T*Cyc_AP_one;
struct Cyc_AP_T*Cyc_AP_new(long);
struct Cyc_AP_T*Cyc_AP_fromint(long);
# 16
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*,struct Cyc_AP_T*);
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*,struct Cyc_AP_T*);
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*,struct Cyc_AP_T*);
struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*,struct Cyc_AP_T*);
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*,struct Cyc_AP_T*);
struct Cyc_AP_T*Cyc_AP_pow(struct Cyc_AP_T*,struct Cyc_AP_T*,struct Cyc_AP_T*);
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*,long);
# 28
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*,int);
struct Cyc_AP_T*Cyc_AP_and(struct Cyc_AP_T*,struct Cyc_AP_T*);
struct Cyc_AP_T*Cyc_AP_or(struct Cyc_AP_T*,struct Cyc_AP_T*);
struct Cyc_AP_T*Cyc_AP_xor(struct Cyc_AP_T*,struct Cyc_AP_T*);
int Cyc_AP_cmp(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 34
struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 8 "xp.h"
extern int Cyc_XP_add(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,int);
extern int Cyc_XP_sub(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr,int);
extern int Cyc_XP_mul(struct _fat_ptr,int,struct _fat_ptr,int,struct _fat_ptr);
extern int Cyc_XP_div(int,struct _fat_ptr,struct _fat_ptr,int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
extern int Cyc_XP_sum(int,struct _fat_ptr,struct _fat_ptr,int);
extern int Cyc_XP_diff(int,struct _fat_ptr,struct _fat_ptr,int);
# 17
extern int Cyc_XP_cmp(int,struct _fat_ptr,struct _fat_ptr);
extern void Cyc_XP_lshift(int,struct _fat_ptr,int,struct _fat_ptr,int,int);
# 20
extern void Cyc_XP_rshift(int,struct _fat_ptr,int,struct _fat_ptr,int,int);
# 22
extern void Cyc_XP_and(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
extern void Cyc_XP_or(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
extern void Cyc_XP_xor(int,struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
extern int Cyc_XP_length(int,struct _fat_ptr);
extern unsigned long Cyc_XP_fromint(int,struct _fat_ptr,unsigned long);
# 28
extern unsigned long Cyc_XP_toint(int,struct _fat_ptr);
extern int Cyc_XP_fromstr(int,struct _fat_ptr,const char*,int);
# 31
extern struct _fat_ptr Cyc_XP_tostr(struct _fat_ptr,int,int,int,struct _fat_ptr);struct Cyc_AP_T{int sign;int ndigits;int size;struct _fat_ptr digits;};
# 21 "ap.cyc"
struct Cyc_AP_T*Cyc_AP_zero;
struct Cyc_AP_T*Cyc_AP_one;
int Cyc_init=0;
# 25
static struct Cyc_AP_T*Cyc_normalize(struct Cyc_AP_T*,int);
static int Cyc_cmp(struct Cyc_AP_T*,struct Cyc_AP_T*);
static void Cyc_AP_init (void){
Cyc_init=1;
Cyc_AP_zero=Cyc_AP_fromint(0);
Cyc_AP_one=Cyc_AP_fromint(1);}
# 32
static struct Cyc_AP_T*Cyc_mk(int size){
struct Cyc_AP_T*z;
if(!Cyc_init)Cyc_AP_init();
z=_cycalloc(sizeof(struct Cyc_AP_T));
({struct _fat_ptr _tmp4E=({unsigned _tmp0=size;_tag_fat(_cyccalloc_atomic(sizeof(unsigned char),_tmp0),sizeof(unsigned char),_tmp0);});z->digits=_tmp4E;});
size > 0?0:({int(*_tmp50)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp4F=({const char*_tmp1="size > 0";_tag_fat(_tmp1,sizeof(char),9U);});_tmp50(_tmp4F,({const char*_tmp2="ap.cyc";_tag_fat(_tmp2,sizeof(char),7U);}),37U);});
z->sign=1;
z->size=size;
z->ndigits=1;
return z;}
# 43
static struct Cyc_AP_T*Cyc_set(struct Cyc_AP_T*z,long n){
if(n == Cyc_long_min)
Cyc_XP_fromint(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,(unsigned)Cyc_long_max + 1U);else{
if(n < 0)
Cyc_XP_fromint(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,(unsigned long)(- n));else{
# 49
Cyc_XP_fromint(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,(unsigned long)n);}}
z->sign=n < 0?-1: 1;
return Cyc_normalize(z,z->size);}
# 53
static struct Cyc_AP_T*Cyc_normalize(struct Cyc_AP_T*z,int n){
({int _tmp51=Cyc_XP_length(n,((struct Cyc_AP_T*)_check_null(z))->digits);z->ndigits=_tmp51;});
return z;}
# 57
static struct Cyc_AP_T*Cyc_add(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int n=((struct Cyc_AP_T*)_check_null(y))->ndigits;
if(((struct Cyc_AP_T*)_check_null(x))->ndigits < n)
return Cyc_add(z,y,x);else{
if(x->ndigits > n){
int carry=Cyc_XP_add(n,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits,0);
# 64
({unsigned char _tmp55=(unsigned char)({int _tmp54=x->ndigits - n;struct _fat_ptr _tmp53=
_fat_ptr_plus(z->digits,sizeof(unsigned char),n);
# 64
struct _fat_ptr _tmp52=
_fat_ptr_plus(x->digits,sizeof(unsigned char),n);
# 64
Cyc_XP_sum(_tmp54,_tmp53,_tmp52,carry);});*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),z->size - 1))=_tmp55;});}else{
# 67
({unsigned char _tmp56=(unsigned char)Cyc_XP_add(n,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits,0);*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),n))=_tmp56;});}}
# 69
return Cyc_normalize(z,z->size);}
# 71
static struct Cyc_AP_T*Cyc_sub(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int borrow;int n=((struct Cyc_AP_T*)_check_null(y))->ndigits;
borrow=({int _tmp59=n;struct _fat_ptr _tmp58=((struct Cyc_AP_T*)_check_null(z))->digits;struct _fat_ptr _tmp57=((struct Cyc_AP_T*)_check_null(x))->digits;Cyc_XP_sub(_tmp59,_tmp58,_tmp57,y->digits,0);});
# 75
if(x->ndigits > n)
borrow=({int _tmp5C=x->ndigits - n;struct _fat_ptr _tmp5B=_fat_ptr_plus(z->digits,sizeof(unsigned char),n);struct _fat_ptr _tmp5A=
_fat_ptr_plus(x->digits,sizeof(unsigned char),n);
# 76
Cyc_XP_diff(_tmp5C,_tmp5B,_tmp5A,borrow);});
# 78
borrow == 0?0:({int(*_tmp5E)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp5D=({const char*_tmp3="borrow == 0";_tag_fat(_tmp3,sizeof(char),12U);});_tmp5E(_tmp5D,({const char*_tmp4="ap.cyc";_tag_fat(_tmp4,sizeof(char),7U);}),78U);});
return Cyc_normalize(z,z->size);}
# 81
static struct Cyc_AP_T*Cyc_mulmod(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){
struct Cyc_AP_T*z;struct Cyc_AP_T*xy=Cyc_AP_mul(x,y);
z=Cyc_AP_mod(xy,p);
return z;}
# 86
static int Cyc_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
if(({int _tmp5F=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmp5F != ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return x->ndigits - y->ndigits;else{
# 90
return Cyc_XP_cmp(x->ndigits,x->digits,y->digits);}}
# 92
struct Cyc_AP_T*Cyc_AP_new(long n){
return({struct Cyc_AP_T*_tmp60=Cyc_mk((int)sizeof(long));Cyc_set(_tmp60,n);});}
# 95
struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_tmp62)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp61=({const char*_tmp5="x";_tag_fat(_tmp5,sizeof(char),2U);});_tmp62(_tmp61,({const char*_tmp6="ap.cyc";_tag_fat(_tmp6,sizeof(char),7U);}),97U);});
z=Cyc_mk(x->ndigits);
({(struct _fat_ptr(*)(struct _fat_ptr,struct _fat_ptr,unsigned long,unsigned))Cyc__memcpy;})(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _fat_ptr)x->digits,(unsigned)x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (unsigned)((unsigned)x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0U?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=z->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))== 0?1: - x->sign;
return z;}
# 104
struct Cyc_AP_T*Cyc_AP_abs(struct Cyc_AP_T*x){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_tmp64)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp63=({const char*_tmp7="x";_tag_fat(_tmp7,sizeof(char),2U);});_tmp64(_tmp63,({const char*_tmp8="ap.cyc";_tag_fat(_tmp8,sizeof(char),7U);}),106U);});
z=Cyc_mk(x->ndigits);
({(struct _fat_ptr(*)(struct _fat_ptr,struct _fat_ptr,unsigned long,unsigned))Cyc__memcpy;})(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _fat_ptr)x->digits,(unsigned)x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (unsigned)((unsigned)x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0U?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=1;
return z;}
# 113
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_tmp66)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp65=({const char*_tmp9="x";_tag_fat(_tmp9,sizeof(char),2U);});_tmp66(_tmp65,({const char*_tmpA="ap.cyc";_tag_fat(_tmpA,sizeof(char),7U);}),115U);});
(unsigned)y?0:({int(*_tmp68)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp67=({const char*_tmpB="y";_tag_fat(_tmpB,sizeof(char),2U);});_tmp68(_tmp67,({const char*_tmpC="ap.cyc";_tag_fat(_tmpC,sizeof(char),7U);}),116U);});
z=Cyc_mk(x->ndigits + y->ndigits);
Cyc_XP_mul(((struct Cyc_AP_T*)_check_null(z))->digits,x->ndigits,x->digits,y->ndigits,y->digits);
# 120
Cyc_normalize(z,z->size);
z->sign=(z->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))== 0 ||(x->sign ^ y->sign)== 0)?1: -1;
# 123
return z;}
# 125
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_tmp6A)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp69=({const char*_tmpD="x";_tag_fat(_tmpD,sizeof(char),2U);});_tmp6A(_tmp69,({const char*_tmpE="ap.cyc";_tag_fat(_tmpE,sizeof(char),7U);}),127U);});
(unsigned)y?0:({int(*_tmp6C)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp6B=({const char*_tmpF="y";_tag_fat(_tmpF,sizeof(char),2U);});_tmp6C(_tmp6B,({const char*_tmp10="ap.cyc";_tag_fat(_tmp10,sizeof(char),7U);}),128U);});
if((x->sign ^ y->sign)== 0){
z=({struct Cyc_AP_T*_tmp6E=Cyc_mk((x->ndigits > y->ndigits?x->ndigits: y->ndigits)+ 1);struct Cyc_AP_T*_tmp6D=x;Cyc_add(_tmp6E,_tmp6D,y);});
({int _tmp6F=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp6F;});}else{
# 133
if(Cyc_cmp(x,y)> 0){
z=({struct Cyc_AP_T*_tmp71=Cyc_mk(x->ndigits);struct Cyc_AP_T*_tmp70=x;Cyc_sub(_tmp71,_tmp70,y);});
z->sign=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1 &&(int)((unsigned char*)(z->digits).curr)[0]== 0?1: x->sign;}else{
# 138
z=({struct Cyc_AP_T*_tmp73=Cyc_mk(y->ndigits);struct Cyc_AP_T*_tmp72=y;Cyc_sub(_tmp73,_tmp72,x);});
z->sign=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1 &&(int)((unsigned char*)(z->digits).curr)[0]== 0?1: - x->sign;}}
# 141
return z;}
# 143
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_tmp75)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp74=({const char*_tmp11="x";_tag_fat(_tmp11,sizeof(char),2U);});_tmp75(_tmp74,({const char*_tmp12="ap.cyc";_tag_fat(_tmp12,sizeof(char),7U);}),145U);});
(unsigned)y?0:({int(*_tmp77)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp76=({const char*_tmp13="y";_tag_fat(_tmp13,sizeof(char),2U);});_tmp77(_tmp76,({const char*_tmp14="ap.cyc";_tag_fat(_tmp14,sizeof(char),7U);}),146U);});
if(!((x->sign ^ y->sign)== 0)){
z=({struct Cyc_AP_T*_tmp79=Cyc_mk((x->ndigits > y->ndigits?x->ndigits: y->ndigits)+ 1);struct Cyc_AP_T*_tmp78=x;Cyc_add(_tmp79,_tmp78,y);});
z->sign=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1 &&(int)((unsigned char*)(z->digits).curr)[0]== 0?1: x->sign;}else{
# 151
if(Cyc_cmp(x,y)> 0){
z=({struct Cyc_AP_T*_tmp7B=Cyc_mk(x->ndigits);struct Cyc_AP_T*_tmp7A=x;Cyc_sub(_tmp7B,_tmp7A,y);});
({int _tmp7C=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;z->sign=_tmp7C;});}else{
# 155
z=({struct Cyc_AP_T*_tmp7E=Cyc_mk(y->ndigits);struct Cyc_AP_T*_tmp7D=y;Cyc_sub(_tmp7E,_tmp7D,x);});
({int _tmp7F=((struct Cyc_AP_T*)_check_null(z))->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))== 0?1: - x->sign;z->sign=_tmp7F;});}}
# 158
return z;}
# 160
struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*q;struct Cyc_AP_T*r;
(unsigned)x?0:({int(*_tmp81)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp80=({const char*_tmp15="x";_tag_fat(_tmp15,sizeof(char),2U);});_tmp81(_tmp80,({const char*_tmp16="ap.cyc";_tag_fat(_tmp16,sizeof(char),7U);}),162U);});
(unsigned)y?0:({int(*_tmp83)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp82=({const char*_tmp17="y";_tag_fat(_tmp17,sizeof(char),2U);});_tmp83(_tmp82,({const char*_tmp18="ap.cyc";_tag_fat(_tmp18,sizeof(char),7U);}),163U);});
!(y->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))== 0)?0:({int(*_tmp85)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp84=({const char*_tmp19="!((y)->ndigits==1 && (y)->digits[0]==0)";_tag_fat(_tmp19,sizeof(char),40U);});_tmp85(_tmp84,({const char*_tmp1A="ap.cyc";_tag_fat(_tmp1A,sizeof(char),7U);}),164U);});
q=Cyc_mk(x->ndigits);
r=Cyc_mk(y->ndigits);
{
struct _fat_ptr tmp=({unsigned _tmp1B=(unsigned)((x->ndigits + y->ndigits)+ 2)* sizeof(unsigned char);_tag_fat(_cycalloc_atomic(_tmp1B),1U,_tmp1B);});
({int _tmp8B=x->ndigits;struct _fat_ptr _tmp8A=((struct Cyc_AP_T*)_check_null(q))->digits;struct _fat_ptr _tmp89=x->digits;int _tmp88=y->ndigits;struct _fat_ptr _tmp87=y->digits;struct _fat_ptr _tmp86=((struct Cyc_AP_T*)_check_null(r))->digits;Cyc_XP_div(_tmp8B,_tmp8A,_tmp89,_tmp88,_tmp87,_tmp86,tmp);});}
# 172
Cyc_normalize(q,q->size);
Cyc_normalize(r,r->size);
q->sign=(q->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(q->digits,sizeof(unsigned char),0))== 0 ||(x->sign ^ y->sign)== 0)?1: -1;
# 176
return q;}
# 178
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*q;struct Cyc_AP_T*r;
(unsigned)x?0:({int(*_tmp8D)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp8C=({const char*_tmp1C="x";_tag_fat(_tmp1C,sizeof(char),2U);});_tmp8D(_tmp8C,({const char*_tmp1D="ap.cyc";_tag_fat(_tmp1D,sizeof(char),7U);}),180U);});
(unsigned)y?0:({int(*_tmp8F)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp8E=({const char*_tmp1E="y";_tag_fat(_tmp1E,sizeof(char),2U);});_tmp8F(_tmp8E,({const char*_tmp1F="ap.cyc";_tag_fat(_tmp1F,sizeof(char),7U);}),181U);});
!(y->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))== 0)?0:({int(*_tmp91)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp90=({const char*_tmp20="!((y)->ndigits==1 && (y)->digits[0]==0)";_tag_fat(_tmp20,sizeof(char),40U);});_tmp91(_tmp90,({const char*_tmp21="ap.cyc";_tag_fat(_tmp21,sizeof(char),7U);}),182U);});
q=Cyc_mk(x->ndigits);
r=Cyc_mk(y->ndigits);
{
struct _fat_ptr tmp=({unsigned _tmp22=(unsigned)((x->ndigits + y->ndigits)+ 2)* sizeof(unsigned char);_tag_fat(_cycalloc_atomic(_tmp22),1U,_tmp22);});
({int _tmp97=x->ndigits;struct _fat_ptr _tmp96=((struct Cyc_AP_T*)_check_null(q))->digits;struct _fat_ptr _tmp95=x->digits;int _tmp94=y->ndigits;struct _fat_ptr _tmp93=y->digits;struct _fat_ptr _tmp92=((struct Cyc_AP_T*)_check_null(r))->digits;Cyc_XP_div(_tmp97,_tmp96,_tmp95,_tmp94,_tmp93,_tmp92,tmp);});}
# 190
Cyc_normalize(q,q->size);
Cyc_normalize(r,r->size);
r->sign=r->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(r->digits,sizeof(unsigned char),0))== 0?1: x->sign;
return r;}
# 195
struct Cyc_AP_T*Cyc_AP_pow(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_tmp99)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp98=({const char*_tmp23="x";_tag_fat(_tmp23,sizeof(char),2U);});_tmp99(_tmp98,({const char*_tmp24="ap.cyc";_tag_fat(_tmp24,sizeof(char),7U);}),197U);});
(unsigned)y?0:({int(*_tmp9B)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp9A=({const char*_tmp25="y";_tag_fat(_tmp25,sizeof(char),2U);});_tmp9B(_tmp9A,({const char*_tmp26="ap.cyc";_tag_fat(_tmp26,sizeof(char),7U);}),198U);});
y->sign == 1?0:({int(*_tmp9D)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp9C=({const char*_tmp27="y->sign == 1";_tag_fat(_tmp27,sizeof(char),13U);});_tmp9D(_tmp9C,({const char*_tmp28="ap.cyc";_tag_fat(_tmp28,sizeof(char),7U);}),199U);});
(!((unsigned)p)||(p->sign == 1 && !(p->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(p->digits,sizeof(unsigned char),0))== 0))&& !(p->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(p->digits,sizeof(unsigned char),0))== 1))?0:({int(*_tmp9F)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmp9E=({const char*_tmp29="!p || p->sign==1 && !((p)->ndigits==1 && (p)->digits[0]==0) && !((p)->ndigits==1 && (p)->digits[0]==1)";_tag_fat(_tmp29,sizeof(char),103U);});_tmp9F(_tmp9E,({const char*_tmp2A="ap.cyc";_tag_fat(_tmp2A,sizeof(char),7U);}),200U);});
if(x->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(x->digits,sizeof(unsigned char),0))== 0)
return Cyc_AP_new(0);
if(y->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))== 0)
return Cyc_AP_new(1);
if(x->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(x->digits,sizeof(unsigned char),0))== 1)
return Cyc_AP_new(((int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0?1: x->sign);
if((unsigned)p){
if(y->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))== 1)
z=Cyc_AP_mod(x,p);else{
# 211
struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,p);
z=Cyc_mulmod(t,t,p);
if(!(((int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0))
z=({struct Cyc_AP_T*_tmpA1=y2=Cyc_AP_mod(x,p);struct Cyc_AP_T*_tmpA0=t=z;Cyc_mulmod(_tmpA1,_tmpA0,p);});}}else{
# 218
if(y->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))== 1)
z=Cyc_AP_addi(x,0);else{
# 221
struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,0);
z=Cyc_AP_mul(t,t);
if(!(((int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))& 1)== 0))
z=({struct Cyc_AP_T*_tmpA2=x;Cyc_AP_mul(_tmpA2,(t=z));});}}
# 227
return z;}
# 229
int Cyc_AP_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
(unsigned)x?0:({int(*_tmpA4)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpA3=({const char*_tmp2B="x";_tag_fat(_tmp2B,sizeof(char),2U);});_tmpA4(_tmpA3,({const char*_tmp2C="ap.cyc";_tag_fat(_tmp2C,sizeof(char),7U);}),230U);});
(unsigned)y?0:({int(*_tmpA6)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpA5=({const char*_tmp2D="y";_tag_fat(_tmp2D,sizeof(char),2U);});_tmpA6(_tmpA5,({const char*_tmp2E="ap.cyc";_tag_fat(_tmp2E,sizeof(char),7U);}),231U);});
if(!((x->sign ^ y->sign)== 0))
return x->sign;else{
if(x->sign == 1)
return Cyc_cmp(x,y);else{
# 237
return Cyc_cmp(y,x);}}}
# 239
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpA7=x;Cyc_AP_add(_tmpA7,Cyc_set(t,y));});}
# 243
struct Cyc_AP_T*Cyc_AP_subi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpA8=x;Cyc_AP_sub(_tmpA8,Cyc_set(t,y));});}
# 247
struct Cyc_AP_T*Cyc_AP_muli(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpA9=x;Cyc_AP_mul(_tmpA9,Cyc_set(t,y));});}
# 251
struct Cyc_AP_T*Cyc_AP_divi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpAA=x;Cyc_AP_div(_tmpAA,Cyc_set(t,y));});}
# 255
int Cyc_AP_cmpi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return({struct Cyc_AP_T*_tmpAB=x;Cyc_AP_cmp(_tmpAB,Cyc_set(t,y));});}
# 259
long Cyc_AP_modi(struct Cyc_AP_T*x,long y){
long rem;
struct Cyc_AP_T*r;
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
r=({struct Cyc_AP_T*_tmpAC=x;Cyc_AP_mod(_tmpAC,Cyc_set(t,y));});
rem=(long)Cyc_XP_toint(((struct Cyc_AP_T*)_check_null(r))->ndigits,r->digits);
return rem;}
# 267
struct Cyc_AP_T*Cyc_AP_lshift(struct Cyc_AP_T*x,int s){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_tmpAE)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpAD=({const char*_tmp2F="x";_tag_fat(_tmp2F,sizeof(char),2U);});_tmpAE(_tmpAD,({const char*_tmp30="ap.cyc";_tag_fat(_tmp30,sizeof(char),7U);}),269U);});
s >= 0?0:({int(*_tmpB0)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpAF=({const char*_tmp31="s >= 0";_tag_fat(_tmp31,sizeof(char),7U);});_tmpB0(_tmpAF,({const char*_tmp32="ap.cyc";_tag_fat(_tmp32,sizeof(char),7U);}),270U);});
z=Cyc_mk(x->ndigits + (s + 7 & 4294967288U)/ 8);
Cyc_XP_lshift(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,x->ndigits,x->digits,s,0);
# 274
z->sign=x->sign;
return Cyc_normalize(z,z->size);}
# 277
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*x,int s){
(unsigned)x?0:({int(*_tmpB2)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpB1=({const char*_tmp33="x";_tag_fat(_tmp33,sizeof(char),2U);});_tmpB2(_tmpB1,({const char*_tmp34="ap.cyc";_tag_fat(_tmp34,sizeof(char),7U);}),278U);});
s >= 0?0:({int(*_tmpB4)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpB3=({const char*_tmp35="s >= 0";_tag_fat(_tmp35,sizeof(char),7U);});_tmpB4(_tmpB3,({const char*_tmp36="ap.cyc";_tag_fat(_tmp36,sizeof(char),7U);}),279U);});
if(s >= 8 * x->ndigits)
return Cyc_AP_new(0);else{
# 283
struct Cyc_AP_T*z=Cyc_mk(x->ndigits - s / 8);
Cyc_XP_rshift(((struct Cyc_AP_T*)_check_null(z))->size,z->digits,x->ndigits,x->digits,s,0);
# 286
Cyc_normalize(z,z->size);
z->sign=z->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))== 0?1: x->sign;
return z;}}
# 291
struct Cyc_AP_T*Cyc_AP_and(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _tmpB5=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpB5 < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_and(y,x);
z=Cyc_mk(y->ndigits);
Cyc_XP_and(y->ndigits,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits);
Cyc_normalize(z,z->size);
z->sign=1;
return z;}
# 302
struct Cyc_AP_T*Cyc_AP_or(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _tmpB6=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpB6 < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_or(y,x);
z=Cyc_mk(x->ndigits);
Cyc_XP_or(y->ndigits,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits);
for(i=y->ndigits;i < x->ndigits;++ i){
Cyc_normalize(z,z->size);}
z->sign=1;
return z;}
# 314
struct Cyc_AP_T*Cyc_AP_xor(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _tmpB7=((struct Cyc_AP_T*)_check_null(x))->ndigits;_tmpB7 < ((struct Cyc_AP_T*)_check_null(y))->ndigits;}))
return Cyc_AP_xor(y,x);
z=Cyc_mk(x->ndigits);
Cyc_XP_xor(y->ndigits,((struct Cyc_AP_T*)_check_null(z))->digits,x->digits,y->digits);
for(i=y->ndigits;i < x->ndigits;++ i){
Cyc_normalize(z,z->size);}
z->sign=1;
return z;}
# 326
struct Cyc_AP_T*Cyc_AP_fromint(long x){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return Cyc_set(t,x);}
# 330
long Cyc_AP_toint(struct Cyc_AP_T*x){
unsigned long u;
(unsigned)x?0:({int(*_tmpB9)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpB8=({const char*_tmp37="x";_tag_fat(_tmp37,sizeof(char),2U);});_tmpB9(_tmpB8,({const char*_tmp38="ap.cyc";_tag_fat(_tmp38,sizeof(char),7U);}),332U);});
u=({unsigned long _tmpBA=Cyc_XP_toint(x->ndigits,x->digits);_tmpBA % ((unsigned)Cyc_long_max + 1U);});
if(x->sign == -1)
return -(long)u;else{
# 337
return(long)u;}}
# 339
struct Cyc_AP_T*Cyc_AP_fromstr(const char*str,int base){
struct Cyc_AP_T*z;
struct _fat_ptr p=({const char*_tmp40=str;_tag_fat(_tmp40,sizeof(char),_get_zero_arr_size_char((void*)_tmp40,1U));});
char sign='\000';
int carry;
(unsigned)p.curr?0:({int(*_tmpBC)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpBB=({const char*_tmp39="p";_tag_fat(_tmp39,sizeof(char),2U);});_tmpBC(_tmpBB,({const char*_tmp3A="ap.cyc";_tag_fat(_tmp3A,sizeof(char),7U);}),344U);});
base >= 2 && base <= 36?0:({int(*_tmpBE)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpBD=({const char*_tmp3B="base >= 2 && base <= 36";_tag_fat(_tmp3B,sizeof(char),24U);});_tmpBE(_tmpBD,({const char*_tmp3C="ap.cyc";_tag_fat(_tmp3C,sizeof(char),7U);}),345U);});
while((int)*((const char*)_check_fat_subscript(p,sizeof(char),0U))&& isspace((int)*((const char*)p.curr))){
_fat_ptr_inplace_plus(& p,sizeof(char),1);}
if((int)*((const char*)p.curr)== 45 ||(int)*((const char*)p.curr)== 43)
sign=*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& p,sizeof(char),1),sizeof(char),0U));
{
const char*start;
int k;int n=0;
for(1;(int)*((const char*)_check_fat_subscript(p,sizeof(char),0U))== 48 &&(int)*((const char*)_check_fat_subscript(p,sizeof(char),1))== 48;_fat_ptr_inplace_plus(& p,sizeof(char),1)){
;}
start=(const char*)_untag_fat_ptr(p,sizeof(char),1U);
for(1;((48 <= (int)*((const char*)_check_fat_subscript(p,sizeof(char),0U))&&(int)*((const char*)p.curr)<= 57)&&(int)*((const char*)p.curr)< 48 + base ||
(97 <= (int)*((const char*)p.curr)&&(int)*((const char*)p.curr)<= 122)&&(int)*((const char*)p.curr)< (97 + base)- 10)||
(65 <= (int)*((const char*)p.curr)&&(int)*((const char*)p.curr)<= 90)&&(int)*((const char*)p.curr)< (65 + base)- 10;_fat_ptr_inplace_plus(& p,sizeof(char),1)){
++ n;}
for(k=1;1 << k < base;++ k){
;}
z=Cyc_mk((k * n + 7 & 4294967288U)/ 8);
p=({const char*_tmp3D=start;_tag_fat(_tmp3D,sizeof(char),_get_zero_arr_size_char((void*)_tmp3D,1U));});}
# 365
carry=({int _tmpC1=((struct Cyc_AP_T*)_check_null(z))->size;struct _fat_ptr _tmpC0=z->digits;const char*_tmpBF=(const char*)_untag_fat_ptr(p,sizeof(char),1U);Cyc_XP_fromstr(_tmpC1,_tmpC0,_tmpBF,base);});
# 367
carry == 0?0:({int(*_tmpC3)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpC2=({const char*_tmp3E="carry == 0";_tag_fat(_tmp3E,sizeof(char),11U);});_tmpC3(_tmpC2,({const char*_tmp3F="ap.cyc";_tag_fat(_tmp3F,sizeof(char),7U);}),367U);});
Cyc_normalize(z,z->size);
z->sign=(z->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))== 0 ||(int)sign != 45)?1: -1;
return z;}
# 372
char*Cyc_AP_tostr(struct Cyc_AP_T*x,int base){
struct _fat_ptr q;
struct _fat_ptr str;
int size;int k;
(unsigned)x?0:({int(*_tmpC5)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpC4=({const char*_tmp41="x";_tag_fat(_tmp41,sizeof(char),2U);});_tmpC5(_tmpC4,({const char*_tmp42="ap.cyc";_tag_fat(_tmp42,sizeof(char),7U);}),376U);});
base >= 2 && base <= 36?0:({int(*_tmpC7)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpC6=({const char*_tmp43="base >= 2 && base <= 36";_tag_fat(_tmp43,sizeof(char),24U);});_tmpC7(_tmpC6,({const char*_tmp44="ap.cyc";_tag_fat(_tmp44,sizeof(char),7U);}),377U);});
for(k=5;1 << k > base;-- k){
;}
size=((8 * x->ndigits)/ k + 1)+ 1;
if(x->sign == -1)
++ size;
str=({unsigned _tmp45=size;_tag_fat(_cyccalloc_atomic(sizeof(char),_tmp45),sizeof(char),_tmp45);});
q=({unsigned _tmp46=(unsigned)x->ndigits * sizeof(unsigned char);_tag_fat(_cycalloc_atomic(_tmp46),1U,_tmp46);});
({(struct _fat_ptr(*)(struct _fat_ptr,struct _fat_ptr,unsigned long,unsigned))Cyc__memcpy;})(q,(struct _fat_ptr)x->digits,(unsigned)x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (unsigned)((unsigned)x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0U?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
if(x->sign == -1){
({struct _fat_ptr _tmp47=_fat_ptr_plus(str,sizeof(char),0);char _tmp48=*((char*)_check_fat_subscript(_tmp47,sizeof(char),0U));char _tmp49='-';if(_get_fat_size(_tmp47,sizeof(char))== 1U &&(_tmp48 == 0 && _tmp49 != 0))_throw_arraybounds();*((char*)_tmp47.curr)=_tmp49;});
({struct _fat_ptr _tmpCB=_fat_ptr_plus(str,sizeof(char),1);int _tmpCA=size - 1;int _tmpC9=base;int _tmpC8=x->ndigits;Cyc_XP_tostr(_tmpCB,_tmpCA,_tmpC9,_tmpC8,q);});}else{
# 390
Cyc_XP_tostr(str,size,base,x->ndigits,q);}
return(char*)_untag_fat_ptr(str,sizeof(char),1U);}
# 393
struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;struct Cyc_AP_T*tmp;
(unsigned)x?0:({int(*_tmpCD)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpCC=({const char*_tmp4A="x";_tag_fat(_tmp4A,sizeof(char),2U);});_tmpCD(_tmpCC,({const char*_tmp4B="ap.cyc";_tag_fat(_tmp4B,sizeof(char),7U);}),395U);});
(unsigned)y?0:({int(*_tmpCF)(struct _fat_ptr,struct _fat_ptr,unsigned)=({(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;});struct _fat_ptr _tmpCE=({const char*_tmp4C="y";_tag_fat(_tmp4C,sizeof(char),2U);});_tmpCF(_tmpCE,({const char*_tmp4D="ap.cyc";_tag_fat(_tmp4D,sizeof(char),7U);}),396U);});
while(!(((struct Cyc_AP_T*)_check_null(y))->ndigits == 1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))== 0)){
tmp=Cyc_AP_mod(x,y);
x=y;
y=tmp;}
# 402
z=Cyc_mk(x->ndigits);
({(struct _fat_ptr(*)(struct _fat_ptr,struct _fat_ptr,unsigned long,unsigned))Cyc__memcpy;})(((struct Cyc_AP_T*)_check_null(z))->digits,(struct _fat_ptr)x->digits,(unsigned)x->ndigits / sizeof(*((unsigned char*)(x->digits).curr))+ (unsigned)((unsigned)x->ndigits % sizeof(*((unsigned char*)(x->digits).curr))== 0U?0: 1),sizeof(*((unsigned char*)(x->digits).curr)));
z->ndigits=x->ndigits;
z->sign=x->sign;
return z;}
# 408
struct Cyc_AP_T*Cyc_AP_lcm(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
return({struct Cyc_AP_T*_tmpD1=x;Cyc_AP_mul(_tmpD1,({struct Cyc_AP_T*_tmpD0=y;Cyc_AP_div(_tmpD0,Cyc_AP_gcd(x,y));}));});}
