#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator  The corresponding definitions are in file lib/runtime_*.c */
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
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
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
void* _region_vmalloc(struct _RegionHandle*, unsigned);
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

#ifdef CYC_GC_PTHREAD_REDIRECTS
# define pthread_create GC_pthread_create
# define pthread_sigmask GC_pthread_sigmask
# define pthread_join GC_pthread_join
# define pthread_detach GC_pthread_detach
# define dlopen GC_dlopen
#endif
/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);

#if(defined(__linux__) && defined(__KERNEL__))
void *cyc_vmalloc(unsigned);
void cyc_vfree(void *);
#endif
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
 struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);
# 319 "cycboot.h"
extern void exit(int);
# 35 "warn.h"
void Cyc_Warn_err(unsigned,struct _fat_ptr,struct _fat_ptr);
# 127 "flags.h"
extern int Cyc_Flags_print_parser_state_and_token;
# 27 "parse.h"
struct _fat_ptr Cyc_token2string(int);struct Cyc_Yyltype{int timestamp;int first_line;int first_column;int last_line;int last_column;};
# 92 "parse_tab.h"
extern struct Cyc_Yyltype Cyc_yylloc;struct Cyc_ParseErrors_ParseState{int state;int token;struct _fat_ptr msg;};static char _TmpG0[34U]="undeclared type or missing comma ";static char _TmpG1[28U]="type has not been declared ";static char _TmpG2[13U]="missing ';' ";static char _TmpG3[31U]="missing ')' on parameter list ";static char _TmpG4[29U]="expecting IDENTIFIER or '{' ";static char _TmpG5[29U]="expecting IDENTIFIER or '{' ";static char _TmpG6[19U]="expecting PATTERN ";static char _TmpG7[22U]="expecting IDENTIFIER ";static char _TmpG8[22U]="expecting IDENTIFIER ";static char _TmpG9[30U]="expecting '(' EXPRESSION ')' ";static char _TmpGA[19U]="expecting 'union' ";static char _TmpGB[22U]="expecting 'datatype' ";static char _TmpGC[36U]="expecting '((' ATTRIBUTE_LIST '))' ";static char _TmpGD[34U]="expecting '(' PARAMETER_LIST ')' ";static char _TmpGE[16U]="expecting KIND ";static char _TmpGF[16U]="expecting KIND ";static char _TmpG10[22U]="expecting ';' or '{' ";static char _TmpG11[22U]="expecting ';' or '{' ";static char _TmpG12[29U]="expecting '<' or IDENTIFIER ";static char _TmpG13[14U]="missing '};' ";static char _TmpG14[22U]="expecting IDENTIFIER ";static char _TmpG15[15U]="expecting '(' ";static char _TmpG16[22U]="expecting IDENTIFIER ";static char _TmpG17[19U]="expecting PATTERN ";static char _TmpG18[35U]="expecting IDENTIFIER or extra ',' ";static char _TmpG19[22U]="expecting EXPRESSION ";static char _TmpG1A[13U]="missing '}' ";static char _TmpG1B[13U]="missing '}' ";static char _TmpG1C[13U]="missing ';' ";static char _TmpG1D[22U]="expecting EXPRESSION ";static char _TmpG1E[23U]="expecting DECLARATION ";static char _TmpG1F[13U]="missing '}' ";static char _TmpG20[29U]="expecting ']' or EXPRESSION ";static char _TmpG21[22U]="expecting EXPRESSION ";static char _TmpG22[16U]="expecting TYPE ";static char _TmpG23[13U]="missing ')' ";static char _TmpG24[22U]="expecting EXPRESSION ";static char _TmpG25[13U]="missing ')' ";static char _TmpG26[16U]="expecting TYPE ";static char _TmpG27[22U]="expecting EXPRESSION ";static char _TmpG28[13U]="missing '}' ";static char _TmpG29[32U]="expecting TYPE_VARIABLE or '_' ";static char _TmpG2A[45U]="expecting '(' EXPRESSION ')' or '( TYPE ')' ";static char _TmpG2B[13U]="missing ')' ";static char _TmpG2C[22U]="expecting EXPRESSION ";static char _TmpG2D[37U]="expecting INITIALIZER or EXPRESSION ";static char _TmpG2E[29U]="expecting '(' EXPRESSION ') ";static char _TmpG2F[25U]="expecting EXPRESSION ') ";static char _TmpG30[12U]="missing ') ";static char _TmpG31[44U]="expecting '(' EXPRESSION ',' EXPRESSION ') ";static char _TmpG32[40U]="expecting EXPRESSION ',' EXPRESSION ') ";static char _TmpG33[29U]="expecting ',' EXPRESSION ') ";static char _TmpG34[13U]="missing ')' ";static char _TmpG35[15U]="expecting '(' ";static char _TmpG36[22U]="expecting EXPRESSION ";static char _TmpG37[15U]="expecting ',' ";static char _TmpG38[30U]="expecting 'sizeof(' TYPE ')' ";static char _TmpG39[16U]="expecting TYPE ";static char _TmpG3A[13U]="missing ')' ";static char _TmpG3B[15U]="expecting '(' ";static char _TmpG3C[22U]="expecting EXPRESSION ";static char _TmpG3D[15U]="expecting ',' ";static char _TmpG3E[22U]="expecting EXPRESSION ";static char _TmpG3F[15U]="expecting ',' ";static char _TmpG40[20U]="expecting 'sizeof' ";static char _TmpG41[15U]="expecting '(' ";static char _TmpG42[16U]="expecting TYPE ";static char _TmpG43[13U]="missing ')' ";static char _TmpG44[37U]="expecting '(' REGION_EXPRESSION ')' ";static char _TmpG45[13U]="missing ')' ";static char _TmpG46[37U]="expecting INITIALIZER or EXPRESSION ";static char _TmpG47[30U]="expecting '(' EXPRESSION ')' ";static char _TmpG48[26U]="expecting EXPRESSION ')' ";static char _TmpG49[13U]="missing ')' ";static char _TmpG4A[24U]="expecting '(' TYPE ')' ";static char _TmpG4B[20U]="expecting TYPE ')' ";static char _TmpG4C[13U]="missing ')' ";static char _TmpG4D[15U]="expecting '(' ";static char _TmpG4E[22U]="expecting EXPRESSION ";static char _TmpG4F[41U]="expecting '.' IDENTIFIER or missing ')' ";static char _TmpG50[22U]="expecting IDENTIFIER ";static char _TmpG51[46U]="expecting TYPE_LIST, EXPRESSION_LIST, or ')' ";static char _TmpG52[15U]="expecting ')' ";static char _TmpG53[24U]="missing ':' EXPRESSION ";static char _TmpG54[33U]="missing EXPRESSION or extra '?' ";static char _TmpG55[33U]="missing EXPRESSION or extra '+' ";static char _TmpG56[33U]="missing EXPRESSION or extra '-' ";static char _TmpG57[33U]="missing EXPRESSION or extra '*' ";static char _TmpG58[33U]="missing EXPRESSION or extra '/' ";static char _TmpG59[33U]="missing EXPRESSION or extra '|' ";static char _TmpG5A[33U]="missing EXPRESSION or extra '&' ";static char _TmpG5B[33U]="missing EXPRESSION or extra '^' ";static char _TmpG5C[33U]="missing EXPRESSION or extra '&' ";static char _TmpG5D[34U]="missing EXPRESSION or extra '==' ";static char _TmpG5E[34U]="missing EXPRESSION or extra '!=' ";static char _TmpG5F[33U]="missing EXPRESSION or extra '<' ";static char _TmpG60[34U]="missing EXPRESSION or extra '<=' ";static char _TmpG61[33U]="missing EXPRESSION or extra '>' ";static char _TmpG62[34U]="missing EXPRESSION or extra '>=' ";static char _TmpG63[34U]="missing EXPRESSION or extra '>>' ";static char _TmpG64[34U]="missing EXPRESSION or extra '<<' ";static char _TmpG65[33U]="missing EXPRESSION or extra '%' ";static char _TmpG66[22U]="expecting EXPRESSION ";static char _TmpG67[22U]="expecting EXPRESSION ";static char _TmpG68[22U]="expecting EXPRESSION ";static char _TmpG69[31U]="extra ',' or missing argument ";static char _TmpG6A[13U]="missing ')' ";static char _TmpG6B[22U]="expecting IDENTIFIER ";static char _TmpG6C[22U]="expecting IDENTIFIER ";static char _TmpG6D[48U]="extra space not allowed in empty instantiation ";static char _TmpG6E[37U]="expecting '<>' or '<' TYPE_LIST '>' ";
# 11 "parse_errors.cyc"
static struct Cyc_ParseErrors_ParseState Cyc_ParseErrors_msg_table[111U]={{130,363,{_TmpG0,_TmpG0,_TmpG0 + 34U}},{69,363,{_TmpG1,_TmpG1,_TmpG1 + 28U}},{489,- 1,{_TmpG2,_TmpG2,_TmpG2 + 13U}},{317,- 1,{_TmpG3,_TmpG3,_TmpG3 + 31U}},{66,- 1,{_TmpG4,_TmpG4,_TmpG4 + 29U}},{22,- 1,{_TmpG5,_TmpG5,_TmpG5 + 29U}},{26,- 1,{_TmpG6,_TmpG6,_TmpG6 + 19U}},{28,- 1,{_TmpG7,_TmpG7,_TmpG7 + 22U}},{29,- 1,{_TmpG8,_TmpG8,_TmpG8 + 22U}},{36,- 1,{_TmpG9,_TmpG9,_TmpG9 + 30U}},{37,- 1,{_TmpGA,_TmpGA,_TmpGA + 19U}},{38,- 1,{_TmpGB,_TmpGB,_TmpGB + 22U}},{44,- 1,{_TmpGC,_TmpGC,_TmpGC + 36U}},{47,- 1,{_TmpGD,_TmpGD,_TmpGD + 34U}},{114,- 1,{_TmpGE,_TmpGE,_TmpGE + 16U}},{118,- 1,{_TmpGF,_TmpGF,_TmpGF + 16U}},{55,- 1,{_TmpG10,_TmpG10,_TmpG10 + 22U}},{56,- 1,{_TmpG11,_TmpG11,_TmpG11 + 22U}},{33,- 1,{_TmpG12,_TmpG12,_TmpG12 + 29U}},{859,- 1,{_TmpG13,_TmpG13,_TmpG13 + 14U}},{68,- 1,{_TmpG14,_TmpG14,_TmpG14 + 22U}},{95,- 1,{_TmpG15,_TmpG15,_TmpG15 + 15U}},{96,- 1,{_TmpG16,_TmpG16,_TmpG16 + 22U}},{97,- 1,{_TmpG17,_TmpG17,_TmpG17 + 19U}},{230,- 1,{_TmpG18,_TmpG18,_TmpG18 + 35U}},{120,- 1,{_TmpG19,_TmpG19,_TmpG19 + 22U}},{126,- 1,{_TmpG1A,_TmpG1A,_TmpG1A + 13U}},{266,- 1,{_TmpG1B,_TmpG1B,_TmpG1B + 13U}},{130,- 1,{_TmpG1C,_TmpG1C,_TmpG1C + 13U}},{269,- 1,{_TmpG1D,_TmpG1D,_TmpG1D + 22U}},{268,- 1,{_TmpG1E,_TmpG1E,_TmpG1E + 23U}},{143,0,{_TmpG1F,_TmpG1F,_TmpG1F + 13U}},{150,- 1,{_TmpG20,_TmpG20,_TmpG20 + 29U}},{445,- 1,{_TmpG21,_TmpG21,_TmpG21 + 22U}},{324,- 1,{_TmpG22,_TmpG22,_TmpG22 + 16U}},{542,- 1,{_TmpG23,_TmpG23,_TmpG23 + 13U}},{323,- 1,{_TmpG24,_TmpG24,_TmpG24 + 22U}},{541,- 1,{_TmpG25,_TmpG25,_TmpG25 + 13U}},{164,- 1,{_TmpG26,_TmpG26,_TmpG26 + 16U}},{332,- 1,{_TmpG27,_TmpG27,_TmpG27 + 22U}},{170,- 1,{_TmpG28,_TmpG28,_TmpG28 + 13U}},{222,- 1,{_TmpG29,_TmpG29,_TmpG29 + 32U}},{171,- 1,{_TmpG2A,_TmpG2A,_TmpG2A + 45U}},{358,- 1,{_TmpG2B,_TmpG2B,_TmpG2B + 13U}},{174,- 1,{_TmpG2C,_TmpG2C,_TmpG2C + 22U}},{175,- 1,{_TmpG2D,_TmpG2D,_TmpG2D + 37U}},{176,- 1,{_TmpG2E,_TmpG2E,_TmpG2E + 29U}},{344,- 1,{_TmpG2F,_TmpG2F,_TmpG2F + 25U}},{559,- 1,{_TmpG30,_TmpG30,_TmpG30 + 12U}},{177,- 1,{_TmpG31,_TmpG31,_TmpG31 + 44U}},{345,- 1,{_TmpG32,_TmpG32,_TmpG32 + 40U}},{560,- 1,{_TmpG33,_TmpG33,_TmpG33 + 29U}},{717,- 1,{_TmpG34,_TmpG34,_TmpG34 + 13U}},{179,- 1,{_TmpG35,_TmpG35,_TmpG35 + 15U}},{347,- 1,{_TmpG36,_TmpG36,_TmpG36 + 22U}},{562,- 1,{_TmpG37,_TmpG37,_TmpG37 + 15U}},{719,- 1,{_TmpG38,_TmpG38,_TmpG38 + 30U}},{915,- 1,{_TmpG39,_TmpG39,_TmpG39 + 16U}},{1045,- 1,{_TmpG3A,_TmpG3A,_TmpG3A + 13U}},{180,- 1,{_TmpG3B,_TmpG3B,_TmpG3B + 15U}},{348,- 1,{_TmpG3C,_TmpG3C,_TmpG3C + 22U}},{563,- 1,{_TmpG3D,_TmpG3D,_TmpG3D + 15U}},{720,- 1,{_TmpG3E,_TmpG3E,_TmpG3E + 22U}},{821,- 1,{_TmpG3F,_TmpG3F,_TmpG3F + 15U}},{916,- 1,{_TmpG40,_TmpG40,_TmpG40 + 20U}},{989,- 1,{_TmpG41,_TmpG41,_TmpG41 + 15U}},{1046,- 1,{_TmpG42,_TmpG42,_TmpG42 + 16U}},{1065,- 1,{_TmpG43,_TmpG43,_TmpG43 + 13U}},{181,- 1,{_TmpG44,_TmpG44,_TmpG44 + 37U}},{564,- 1,{_TmpG45,_TmpG45,_TmpG45 + 13U}},{721,- 1,{_TmpG46,_TmpG46,_TmpG46 + 37U}},{183,- 1,{_TmpG47,_TmpG47,_TmpG47 + 30U}},{351,- 1,{_TmpG48,_TmpG48,_TmpG48 + 26U}},{566,- 1,{_TmpG49,_TmpG49,_TmpG49 + 13U}},{184,- 1,{_TmpG4A,_TmpG4A,_TmpG4A + 24U}},{352,- 1,{_TmpG4B,_TmpG4B,_TmpG4B + 20U}},{567,- 1,{_TmpG4C,_TmpG4C,_TmpG4C + 13U}},{185,- 1,{_TmpG4D,_TmpG4D,_TmpG4D + 15U}},{353,- 1,{_TmpG4E,_TmpG4E,_TmpG4E + 22U}},{568,- 1,{_TmpG4F,_TmpG4F,_TmpG4F + 41U}},{726,- 1,{_TmpG50,_TmpG50,_TmpG50 + 22U}},{514,- 1,{_TmpG51,_TmpG51,_TmpG51 + 46U}},{573,- 1,{_TmpG52,_TmpG52,_TmpG52 + 15U}},{577,- 1,{_TmpG53,_TmpG53,_TmpG53 + 24U}},{366,- 1,{_TmpG54,_TmpG54,_TmpG54 + 33U}},{379,- 1,{_TmpG55,_TmpG55,_TmpG55 + 33U}},{380,- 1,{_TmpG56,_TmpG56,_TmpG56 + 33U}},{381,- 1,{_TmpG57,_TmpG57,_TmpG57 + 33U}},{382,- 1,{_TmpG58,_TmpG58,_TmpG58 + 33U}},{368,- 1,{_TmpG59,_TmpG59,_TmpG59 + 33U}},{370,- 1,{_TmpG5A,_TmpG5A,_TmpG5A + 33U}},{369,- 1,{_TmpG5B,_TmpG5B,_TmpG5B + 33U}},{195,- 1,{_TmpG5C,_TmpG5C,_TmpG5C + 33U}},{371,- 1,{_TmpG5D,_TmpG5D,_TmpG5D + 34U}},{372,- 1,{_TmpG5E,_TmpG5E,_TmpG5E + 34U}},{375,- 1,{_TmpG5F,_TmpG5F,_TmpG5F + 33U}},{373,- 1,{_TmpG60,_TmpG60,_TmpG60 + 34U}},{376,- 1,{_TmpG61,_TmpG61,_TmpG61 + 33U}},{374,- 1,{_TmpG62,_TmpG62,_TmpG62 + 34U}},{378,- 1,{_TmpG63,_TmpG63,_TmpG63 + 34U}},{377,- 1,{_TmpG64,_TmpG64,_TmpG64 + 34U}},{383,- 1,{_TmpG65,_TmpG65,_TmpG65 + 33U}},{396,- 1,{_TmpG66,_TmpG66,_TmpG66 + 22U}},{186,- 1,{_TmpG67,_TmpG67,_TmpG67 + 22U}},{403,- 1,{_TmpG68,_TmpG68,_TmpG68 + 22U}},{731,41,{_TmpG69,_TmpG69,_TmpG69 + 31U}},{599,59,{_TmpG6A,_TmpG6A,_TmpG6A + 13U}},{402,- 1,{_TmpG6B,_TmpG6B,_TmpG6B + 22U}},{398,- 1,{_TmpG6C,_TmpG6C,_TmpG6C + 22U}},{602,62,{_TmpG6D,_TmpG6D,_TmpG6D + 48U}},{405,- 1,{_TmpG6E,_TmpG6E,_TmpG6E + 37U}}};
# 19 "parse_errors.cyc"
void Cyc_yyerror(struct _fat_ptr s,int state,int token){
if(Cyc_Flags_print_parser_state_and_token){
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)state;_Tmp1;});struct Cyc_Int_pa_PrintArg_struct _Tmp1=({struct Cyc_Int_pa_PrintArg_struct _Tmp2;_Tmp2.tag=1,_Tmp2.f1=(unsigned long)token;_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_printf(({const char*_Tmp3="parse error: state [%d], token [%d]\n";_tag_fat(_Tmp3,sizeof(char),37U);}),_tag_fat(_Tmp2,sizeof(void*),2));});
exit(1);}
# 27
{unsigned i=0U;for(0;i < 111U;++ i){
if((Cyc_ParseErrors_msg_table[(int)i]).state == state &&(Cyc_ParseErrors_msg_table[(int)i]).token == token){
# 30
s=(Cyc_ParseErrors_msg_table[(int)i]).msg;
break;}else{
if((Cyc_ParseErrors_msg_table[(int)i]).state == state)
s=(Cyc_ParseErrors_msg_table[(int)i]).msg;}}}{
# 35
struct _fat_ptr ts=Cyc_token2string(token);
if((char*)ts.curr != (char*)(_tag_fat(0,0,0)).curr)
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});struct Cyc_String_pa_PrintArg_struct _Tmp1=({struct Cyc_String_pa_PrintArg_struct _Tmp2;_Tmp2.tag=0,({struct _fat_ptr _Tmp3=Cyc_token2string(token);_Tmp2.f1=_Tmp3;});_Tmp2;});void*_Tmp2[2];_Tmp2[0]=& _Tmp0,_Tmp2[1]=& _Tmp1;Cyc_Warn_err((unsigned)Cyc_yylloc.first_line,({const char*_Tmp3="%s (found %s instead)";_tag_fat(_Tmp3,sizeof(char),22U);}),_tag_fat(_Tmp2,sizeof(void*),2));});else{
# 39
({struct Cyc_String_pa_PrintArg_struct _Tmp0=({struct Cyc_String_pa_PrintArg_struct _Tmp1;_Tmp1.tag=0,_Tmp1.f1=s;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_Warn_err((unsigned)Cyc_yylloc.first_line,({const char*_Tmp2="%s ";_tag_fat(_Tmp2,sizeof(char),4U);}),_tag_fat(_Tmp1,sizeof(void*),1));});}}}
