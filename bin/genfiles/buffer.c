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
extern struct _RegionHandle*Cyc_Core_unique_region;
# 178
void Cyc_Core_ufree(void*ptr);struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_timeval{long tv_sec;long tv_usec;};struct Cyc_Buffer_t;
# 83 "buffer.h"
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _fat_ptr,int offset,int len);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr s);
# 72 "string.h"
extern struct _fat_ptr Cyc_strncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
extern struct _fat_ptr Cyc_zstrncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 109 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int ofs,unsigned long n);struct Cyc_Buffer_t{struct _fat_ptr buffer;unsigned position;unsigned length;struct _fat_ptr initial_buffer;};
# 48 "buffer.cyc"
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned n){
if(n > (unsigned)0){
struct _fat_ptr s=({unsigned _tmp1=n + (unsigned)1;_tag_fat(_region_calloc(Cyc_Core_unique_region,sizeof(char),_tmp1),sizeof(char),_tmp1);});
return({struct Cyc_Buffer_t*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->buffer=s,_tmp0->position=0U,_tmp0->length=n,({struct _fat_ptr _tmp36=_tag_fat(0,0,0);_tmp0->initial_buffer=_tmp36;});_tmp0;});}else{
# 53
return({struct Cyc_Buffer_t*_tmp2=_cycalloc(sizeof(*_tmp2));({struct _fat_ptr _tmp38=_tag_fat(0,0,0);_tmp2->buffer=_tmp38;}),_tmp2->position=0U,_tmp2->length=0U,({struct _fat_ptr _tmp37=_tag_fat(0,0,0);_tmp2->initial_buffer=_tmp37;});_tmp2;});}}
# 57
struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*b){
struct _fat_ptr _tmp3=_tag_fat(0,0,0);struct _fat_ptr buf=_tmp3;
({struct _fat_ptr _tmp4=buf;struct _fat_ptr _tmp5=b->buffer;buf=_tmp5;b->buffer=_tmp4;});
if(({char*_tmp39=(char*)buf.curr;_tmp39 == (char*)(_tag_fat(0,0,0)).curr;}))
return _tag_fat(0,0,0);{
struct _fat_ptr _tmp6=({struct _fat_ptr _tmp9=buf;struct _fat_ptr _tmpA;_LL1: _tmpA=_tmp9;_LL2: ({struct _fat_ptr __aliasvar0=_tmpA;Cyc_substring(__aliasvar0,0,b->position);});});struct _fat_ptr res=_tmp6;
({struct _fat_ptr _tmp7=buf;struct _fat_ptr _tmp8=b->buffer;buf=_tmp8;b->buffer=_tmp7;});
return res;}}
# 67
struct _fat_ptr Cyc_Buffer_extract(struct Cyc_Buffer_t*b){
struct _fat_ptr _tmpB=_tag_fat(0,0,0);struct _fat_ptr buf=_tmpB;
({struct _fat_ptr _tmpC=buf;struct _fat_ptr _tmpD=b->buffer;buf=_tmpD;b->buffer=_tmpC;});
({struct _fat_ptr _tmpE=_fat_ptr_plus(buf,sizeof(char),(int)b->position);char _tmpF=*((char*)_check_fat_subscript(_tmpE,sizeof(char),0U));char _tmp10='\000';if(_get_fat_size(_tmpE,sizeof(char))== 1U &&(_tmpF == 0 && _tmp10 != 0))_throw_arraybounds();*((char*)_tmpE.curr)=_tmp10;});
b->position=0U;
b->length=0U;
return buf;}
# 76
int Cyc_Buffer_restore(struct Cyc_Buffer_t*b,struct _fat_ptr buf){
int len=(int)({struct _fat_ptr _tmp15=buf;struct _fat_ptr _tmp16;_LL1: _tmp16=_tmp15;_LL2: ({struct _fat_ptr __aliasvar1=_tmp16;Cyc_strlen(__aliasvar1);});});
({struct _fat_ptr _tmp11=buf;struct _fat_ptr _tmp12=b->buffer;buf=_tmp12;b->buffer=_tmp11;});
if(({char*_tmp3A=(char*)buf.curr;_tmp3A != (char*)(_tag_fat(0,0,0)).curr;})){
({struct _fat_ptr _tmp13=buf;struct _fat_ptr _tmp14=b->buffer;buf=_tmp14;b->buffer=_tmp13;});
((void(*)(char*ptr))Cyc_Core_ufree)((char*)_untag_fat_ptr(buf,sizeof(char),1U + 1U));
return 0;}
# 84
b->position=0U;
b->length=(unsigned)len;
return 1;}
# 89
unsigned long Cyc_Buffer_length(struct Cyc_Buffer_t*b){
return b->position;}
# 93
void Cyc_Buffer_clear(struct Cyc_Buffer_t*b){
b->position=0U;}
# 97
void Cyc_Buffer_reset(struct Cyc_Buffer_t*b){
b->position=0U;
if(({char*_tmp3B=(char*)(b->initial_buffer).curr;_tmp3B != (char*)(_tag_fat(0,0,0)).curr;})){
struct _fat_ptr _tmp17=_tag_fat(0,0,0);struct _fat_ptr buf=_tmp17;
({struct _fat_ptr _tmp18=b->initial_buffer;struct _fat_ptr _tmp19=buf;b->initial_buffer=_tmp19;buf=_tmp18;});
b->length=_get_fat_size(buf,sizeof(char))- (unsigned)1;
({struct _fat_ptr _tmp1A=b->buffer;struct _fat_ptr _tmp1B=buf;b->buffer=_tmp1B;buf=_tmp1A;});
((void(*)(char*ptr))Cyc_Core_ufree)((char*)_untag_fat_ptr(buf,sizeof(char),1U + 1U));}
# 106
return;}
# 109
static void Cyc_Buffer_resize(struct Cyc_Buffer_t*b,unsigned more){
unsigned long len=b->length;
unsigned long new_len=len == (unsigned long)0?1U: len;
struct _fat_ptr new_buffer;
while(b->position + more > new_len){
new_len=(unsigned long)2 * new_len;}
# 116
new_buffer=({unsigned _tmp1C=new_len + (unsigned long)1;_tag_fat(_region_calloc(Cyc_Core_unique_region,sizeof(char),_tmp1C),sizeof(char),_tmp1C);});
if(b->length != (unsigned)0){
struct _fat_ptr _tmp1D=new_buffer;struct _fat_ptr _tmp1E;_LL1: _tmp1E=_tmp1D;_LL2: {struct _fat_ptr x=_tmp1E;
({struct _fat_ptr _tmp3D=_fat_ptr_decrease_size(x,sizeof(char),1U);struct _fat_ptr _tmp3C=(struct _fat_ptr)b->buffer;Cyc_strncpy(_tmp3D,_tmp3C,b->position);});}}
# 121
if(({char*_tmp3E=(char*)(b->initial_buffer).curr;_tmp3E == (char*)(_tag_fat(0,0,0)).curr;}))
({struct _fat_ptr _tmp1F=b->initial_buffer;struct _fat_ptr _tmp20=b->buffer;b->initial_buffer=_tmp20;b->buffer=_tmp1F;});
({struct _fat_ptr _tmp21=b->buffer;struct _fat_ptr _tmp22=new_buffer;b->buffer=_tmp22;new_buffer=_tmp21;});
((void(*)(char*ptr))Cyc_Core_ufree)((char*)_untag_fat_ptr(new_buffer,sizeof(char),1U + 1U));
b->length=new_len;
return;}
# 129
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*b,char c){
int pos=(int)b->position;
if((unsigned)pos >= b->length)Cyc_Buffer_resize(b,1U);
({struct _fat_ptr _tmp23=_fat_ptr_plus(b->buffer,sizeof(char),pos);char _tmp24=*((char*)_check_fat_subscript(_tmp23,sizeof(char),0U));char _tmp25=c;if(_get_fat_size(_tmp23,sizeof(char))== 1U &&(_tmp24 == 0 && _tmp25 != 0))_throw_arraybounds();*((char*)_tmp23.curr)=_tmp25;});
b->position=(unsigned)(pos + 1);
return;}
# 140
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*b,struct _fat_ptr s,int offset,int len){
if((offset < 0 || len < 0)||(unsigned)(offset + len)> _get_fat_size(s,sizeof(char)))
(int)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_tmp27=_cycalloc(sizeof(*_tmp27));_tmp27->tag=Cyc_Core_Invalid_argument,({struct _fat_ptr _tmp3F=({const char*_tmp26="Buffer::add_substring";_tag_fat(_tmp26,sizeof(char),22U);});_tmp27->f1=_tmp3F;});_tmp27;}));{
int new_position=(int)(b->position + (unsigned)len);
if((unsigned)new_position > b->length)Cyc_Buffer_resize(b,(unsigned)len);{
struct _fat_ptr _tmp28=_tag_fat(0,0,0);struct _fat_ptr buf=_tmp28;
({struct _fat_ptr _tmp29=buf;struct _fat_ptr _tmp2A=b->buffer;buf=_tmp2A;b->buffer=_tmp29;});
{struct _fat_ptr _tmp2B=buf;struct _fat_ptr _tmp2C;_LL1: _tmp2C=_tmp2B;_LL2: {struct _fat_ptr x=_tmp2C;
({struct _fat_ptr _tmp41=_fat_ptr_decrease_size(_fat_ptr_plus(x,sizeof(char),(int)b->position),sizeof(char),1U);struct _fat_ptr _tmp40=(struct _fat_ptr)_fat_ptr_plus(s,sizeof(char),offset);Cyc_zstrncpy(_tmp41,_tmp40,(unsigned long)len);});}}
# 150
({struct _fat_ptr _tmp2D=buf;struct _fat_ptr _tmp2E=b->buffer;buf=_tmp2E;b->buffer=_tmp2D;});
b->position=(unsigned)new_position;
return;}}}
# 156
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*b,struct _fat_ptr s){
int len=(int)Cyc_strlen((struct _fat_ptr)s);
int new_position=(int)(b->position + (unsigned)len);
if((unsigned)new_position > b->length)Cyc_Buffer_resize(b,(unsigned)len);{
struct _fat_ptr _tmp2F=_tag_fat(0,0,0);struct _fat_ptr buf=_tmp2F;
({struct _fat_ptr _tmp30=buf;struct _fat_ptr _tmp31=b->buffer;buf=_tmp31;b->buffer=_tmp30;});
{struct _fat_ptr _tmp32=buf;struct _fat_ptr _tmp33;_LL1: _tmp33=_tmp32;_LL2: {struct _fat_ptr x=_tmp33;
({struct _fat_ptr _tmp44=({struct _fat_ptr _tmp42=_fat_ptr_decrease_size(x,sizeof(char),1U);_fat_ptr_plus(_tmp42,sizeof(char),(int)b->position);});struct _fat_ptr _tmp43=(struct _fat_ptr)s;Cyc_strncpy(_tmp44,_tmp43,(unsigned long)len);});}}
# 165
({struct _fat_ptr _tmp34=buf;struct _fat_ptr _tmp35=b->buffer;buf=_tmp35;b->buffer=_tmp34;});
b->position=(unsigned)new_position;
return;}}
# 170
void Cyc_Buffer_add_buffer(struct Cyc_Buffer_t*b,struct Cyc_Buffer_t*bs){
Cyc_Buffer_add_substring(b,(struct _fat_ptr)bs->buffer,0,(int)bs->position);
return;}
