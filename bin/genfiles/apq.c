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
 struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 7 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_zero;
extern struct Cyc_AP_T*Cyc_AP_one;
# 10
extern struct Cyc_AP_T*Cyc_AP_fromint(long x);
extern struct Cyc_AP_T*Cyc_AP_fromstr(const char*str,int base);
# 13
extern char*Cyc_AP_tostr(struct Cyc_AP_T*x,int base);
extern struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x);
extern struct Cyc_AP_T*Cyc_AP_abs(struct Cyc_AP_T*x);
extern struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
extern struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
extern struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
extern struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
# 32
extern int Cyc_AP_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
# 34
extern struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
extern struct Cyc_AP_T*Cyc_AP_lcm(struct Cyc_AP_T*x,struct Cyc_AP_T*y);
# 7 "apq.h"
struct Cyc_APQ_T*Cyc_APQ_fromAP(struct Cyc_AP_T*n,struct Cyc_AP_T*d);struct Cyc_APQ_T{struct Cyc_AP_T*n;struct Cyc_AP_T*d;};char Cyc_Invalid_argument[17U]="Invalid_argument";struct Cyc_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 13 "apq.cyc"
struct Cyc_APQ_T*Cyc_reduce(struct Cyc_APQ_T*q){
if(Cyc_AP_cmp(((struct Cyc_APQ_T*)_check_null(q))->d,Cyc_AP_zero)< 0){
({struct Cyc_AP_T*_tmp1A=Cyc_AP_neg(q->d);q->d=_tmp1A;});
({struct Cyc_AP_T*_tmp1B=Cyc_AP_neg(q->n);q->n=_tmp1B;});}{
# 18
struct Cyc_AP_T*gcd=({struct Cyc_AP_T*_tmp1C=Cyc_AP_abs(q->n);Cyc_AP_gcd(_tmp1C,q->d);});
if(Cyc_AP_cmp(gcd,Cyc_AP_one)== 0)
return q;{
struct Cyc_APQ_T*newq=_cycalloc(sizeof(struct Cyc_APQ_T));
({struct Cyc_AP_T*_tmp1D=Cyc_AP_div(q->n,gcd);newq->n=_tmp1D;});
({struct Cyc_AP_T*_tmp1E=Cyc_AP_div(q->d,gcd);newq->d=_tmp1E;});
return newq;}}}
# 27
struct Cyc_APQ_T*Cyc_APQ_fromint(int i){
struct Cyc_APQ_T*q=_cycalloc(sizeof(struct Cyc_APQ_T));
({struct Cyc_AP_T*_tmp1F=Cyc_AP_fromint(i);q->n=_tmp1F;});
q->d=Cyc_AP_one;
return q;}
# 34
struct Cyc_APQ_T*Cyc_APQ_fromAP(struct Cyc_AP_T*n,struct Cyc_AP_T*d){
if(Cyc_AP_cmp(d,Cyc_AP_zero)== 0)(int)_throw((void*)({struct Cyc_Invalid_argument_exn_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=Cyc_Invalid_argument,({struct _fat_ptr _tmp20=({const char*_tmp0="APQ_fromAP: divide by zero";_tag_fat(_tmp0,sizeof(char),27U);});_tmp1->f1=_tmp20;});_tmp1;}));{
struct Cyc_APQ_T*q=_cycalloc(sizeof(struct Cyc_APQ_T));
q->n=n;
q->d=d;
return Cyc_reduce(q);}}
# 42
struct Cyc_APQ_T*Cyc_APQ_fromstr(struct _fat_ptr str,int base){
struct Cyc_APQ_T*q=_cycalloc(sizeof(struct Cyc_APQ_T));
struct _fat_ptr _tmp2=str;struct _fat_ptr s=_tmp2;
while((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U))&&(int)*((const char*)_check_fat_subscript(s,sizeof(char),0U))!= (int)'/'){_fat_ptr_inplace_plus(& s,sizeof(char),1);}
({struct Cyc_AP_T*_tmp21=Cyc_AP_fromstr((const char*)_untag_fat_ptr(str,sizeof(char),1U),base);q->n=_tmp21;});
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U))){
struct Cyc_AP_T*_tmp3=Cyc_AP_fromstr((const char*)_untag_fat_ptr(_fat_ptr_plus(s,sizeof(char),1),sizeof(char),1U),base);struct Cyc_AP_T*d=_tmp3;
if(Cyc_AP_cmp(d,Cyc_AP_zero))
q->d=d;else{
(int)_throw((void*)({struct Cyc_Invalid_argument_exn_struct*_tmp5=_cycalloc(sizeof(*_tmp5));_tmp5->tag=Cyc_Invalid_argument,({struct _fat_ptr _tmp22=({const char*_tmp4="APQ_fromstr: malformed string";_tag_fat(_tmp4,sizeof(char),30U);});_tmp5->f1=_tmp22;});_tmp5;}));}}else{
# 54
q->d=Cyc_AP_one;}
return Cyc_reduce(q);}
# 58
struct _fat_ptr Cyc_APQ_tostr(struct Cyc_APQ_T*q,int base){
if(Cyc_AP_cmp(((struct Cyc_APQ_T*)_check_null(q))->d,Cyc_AP_one)== 0)
return({char*_tmp6=Cyc_AP_tostr(q->n,base);_tag_fat(_tmp6,sizeof(char),_get_zero_arr_size_char((void*)_tmp6,1U));});
return({struct Cyc_String_pa_PrintArg_struct _tmp9=({struct Cyc_String_pa_PrintArg_struct _tmp19;_tmp19.tag=0U,({struct _fat_ptr _tmp23=(struct _fat_ptr)({char*_tmpC=Cyc_AP_tostr(q->n,base);_tag_fat(_tmpC,sizeof(char),_get_zero_arr_size_char((void*)_tmpC,1U));});_tmp19.f1=_tmp23;});_tmp19;});struct Cyc_String_pa_PrintArg_struct _tmpA=({struct Cyc_String_pa_PrintArg_struct _tmp18;_tmp18.tag=0U,({struct _fat_ptr _tmp24=(struct _fat_ptr)({char*_tmpB=Cyc_AP_tostr(q->d,base);_tag_fat(_tmpB,sizeof(char),_get_zero_arr_size_char((void*)_tmpB,1U));});_tmp18.f1=_tmp24;});_tmp18;});void*_tmp7[2U];_tmp7[0]=& _tmp9,_tmp7[1]=& _tmpA;({struct _fat_ptr _tmp25=({const char*_tmp8="%s/%s";_tag_fat(_tmp8,sizeof(char),6U);});Cyc_aprintf(_tmp25,_tag_fat(_tmp7,sizeof(void*),2U));});});}
# 64
struct Cyc_APQ_T*Cyc_APQ_neg(struct Cyc_APQ_T*q){
return({struct Cyc_AP_T*_tmp26=Cyc_AP_neg(((struct Cyc_APQ_T*)_check_null(q))->n);Cyc_APQ_fromAP(_tmp26,q->d);});}
# 68
struct Cyc_APQ_T*Cyc_APQ_abs(struct Cyc_APQ_T*q){
return({struct Cyc_AP_T*_tmp27=Cyc_AP_abs(((struct Cyc_APQ_T*)_check_null(q))->n);Cyc_APQ_fromAP(_tmp27,q->d);});}
# 72
struct Cyc_APQ_T*Cyc_APQ_add(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){
struct Cyc_AP_T*_tmpD=({struct Cyc_AP_T*_tmp28=((struct Cyc_APQ_T*)_check_null(p))->d;Cyc_AP_lcm(_tmp28,((struct Cyc_APQ_T*)_check_null(q))->d);});struct Cyc_AP_T*d=_tmpD;
struct Cyc_AP_T*_tmpE=Cyc_AP_div(d,p->d);struct Cyc_AP_T*px=_tmpE;
struct Cyc_AP_T*_tmpF=Cyc_AP_div(d,q->d);struct Cyc_AP_T*qx=_tmpF;
return({struct Cyc_AP_T*_tmp2A=({struct Cyc_AP_T*_tmp29=Cyc_AP_mul(p->n,px);Cyc_AP_add(_tmp29,Cyc_AP_mul(q->n,qx));});Cyc_APQ_fromAP(_tmp2A,d);});}
# 79
struct Cyc_APQ_T*Cyc_APQ_sub(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){
struct Cyc_AP_T*_tmp10=({struct Cyc_AP_T*_tmp2B=((struct Cyc_APQ_T*)_check_null(p))->d;Cyc_AP_lcm(_tmp2B,((struct Cyc_APQ_T*)_check_null(q))->d);});struct Cyc_AP_T*d=_tmp10;
struct Cyc_AP_T*_tmp11=Cyc_AP_div(d,p->d);struct Cyc_AP_T*px=_tmp11;
struct Cyc_AP_T*_tmp12=Cyc_AP_div(d,q->d);struct Cyc_AP_T*qx=_tmp12;
return({struct Cyc_AP_T*_tmp2D=({struct Cyc_AP_T*_tmp2C=Cyc_AP_mul(p->n,px);Cyc_AP_sub(_tmp2C,Cyc_AP_mul(q->n,qx));});Cyc_APQ_fromAP(_tmp2D,d);});}
# 86
struct Cyc_APQ_T*Cyc_APQ_mul(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){
return({struct Cyc_AP_T*_tmp2F=({struct Cyc_AP_T*_tmp2E=((struct Cyc_APQ_T*)_check_null(p))->n;Cyc_AP_mul(_tmp2E,((struct Cyc_APQ_T*)_check_null(q))->n);});Cyc_APQ_fromAP(_tmp2F,Cyc_AP_mul(p->d,q->d));});}
# 90
struct Cyc_APQ_T*Cyc_APQ_div(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){
if(Cyc_AP_cmp(((struct Cyc_APQ_T*)_check_null(q))->n,Cyc_AP_zero)== 0)(int)_throw((void*)({struct Cyc_Invalid_argument_exn_struct*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->tag=Cyc_Invalid_argument,({struct _fat_ptr _tmp30=({const char*_tmp13="APQ_div: divide by zero";_tag_fat(_tmp13,sizeof(char),24U);});_tmp14->f1=_tmp30;});_tmp14;}));
return({struct Cyc_AP_T*_tmp31=Cyc_AP_mul(((struct Cyc_APQ_T*)_check_null(p))->n,q->d);Cyc_APQ_fromAP(_tmp31,Cyc_AP_mul(p->d,q->n));});}
# 95
int Cyc_APQ_cmp(struct Cyc_APQ_T*p,struct Cyc_APQ_T*q){
struct Cyc_AP_T*_tmp15=({struct Cyc_AP_T*_tmp32=((struct Cyc_APQ_T*)_check_null(p))->d;Cyc_AP_lcm(_tmp32,((struct Cyc_APQ_T*)_check_null(q))->d);});struct Cyc_AP_T*d=_tmp15;
struct Cyc_AP_T*_tmp16=Cyc_AP_div(d,p->d);struct Cyc_AP_T*px=_tmp16;
struct Cyc_AP_T*_tmp17=Cyc_AP_div(d,q->d);struct Cyc_AP_T*qx=_tmp17;
return({struct Cyc_AP_T*_tmp33=Cyc_AP_mul(p->n,px);Cyc_AP_cmp(_tmp33,Cyc_AP_mul(q->n,qx));});}
