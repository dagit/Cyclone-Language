#include <setjmp.h>
/* This is a C header used by the output of the Cyclone to
   C translator.  Corresponding definitions are in file lib/runtime_*.c */
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
/* should be size_t but int is fine */
#define offsetof(t,n) ((int)(&(((t*)0)->n)))
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
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons*);
void _push_region(struct _RegionHandle*);
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
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
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
      _curr != (unsigned char*)0) \
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
void cyc_vfree(void*);
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
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;
# 178
void Cyc_Core_ufree(void*);struct Cyc_Buffer_t;
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 72 "string.h"
extern struct _fat_ptr Cyc_strncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
extern struct _fat_ptr Cyc_zstrncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 109 "string.h"
extern struct _fat_ptr Cyc_substring(struct _fat_ptr,int,unsigned long);struct Cyc_Buffer_t{struct _fat_ptr buffer;unsigned position;unsigned length;struct _fat_ptr initial_buffer;};
# 48 "buffer.cyc"
struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned n){
if(n > 0U){
struct _fat_ptr s=({unsigned _Tmp0=n + 1U;_tag_fat(_region_calloc(Cyc_Core_unique_region,sizeof(char),_Tmp0),sizeof(char),_Tmp0);});
return({struct Cyc_Buffer_t*_Tmp0=_cycalloc(sizeof(struct Cyc_Buffer_t));_Tmp0->buffer=s,_Tmp0->position=0U,_Tmp0->length=n,_Tmp0->initial_buffer=_tag_fat(0,0,0);_Tmp0;});}else{
# 53
return({struct Cyc_Buffer_t*_Tmp0=_cycalloc(sizeof(struct Cyc_Buffer_t));_Tmp0->buffer=_tag_fat(0,0,0),_Tmp0->position=0U,_Tmp0->length=0U,_Tmp0->initial_buffer=_tag_fat(0,0,0);_Tmp0;});}}
# 57
struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*b){
struct _fat_ptr buf=_tag_fat(0,0,0);
({struct _fat_ptr _Tmp0=buf;struct _fat_ptr _Tmp1=b->buffer;buf=_Tmp1;b->buffer=_Tmp0;});
if((char*)buf.curr==(char*)_tag_fat(0,0,0).curr)
return _tag_fat(0,0,0);{
struct _fat_ptr _Tmp0=({struct _fat_ptr _Tmp1;_Tmp1=buf;({struct _fat_ptr __aliasvar0=_Tmp1;Cyc_substring(__aliasvar0,0,b->position);});});struct _fat_ptr res=_Tmp0;
({struct _fat_ptr _Tmp1=buf;struct _fat_ptr _Tmp2=b->buffer;buf=_Tmp2;b->buffer=_Tmp1;});
return res;}}
# 67
struct _fat_ptr Cyc_Buffer_extract(struct Cyc_Buffer_t*b){
struct _fat_ptr buf=_tag_fat(0,0,0);
({struct _fat_ptr _Tmp0=buf;struct _fat_ptr _Tmp1=b->buffer;buf=_Tmp1;b->buffer=_Tmp0;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(buf,sizeof(char),(int)b->position);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
b->position=0U;
b->length=0U;
return buf;}
# 76
int Cyc_Buffer_restore(struct Cyc_Buffer_t*b,struct _fat_ptr buf){
int len=(int)({struct _fat_ptr _Tmp0;_Tmp0=buf;({struct _fat_ptr __aliasvar1=_Tmp0;Cyc_strlen(__aliasvar1);});});
({struct _fat_ptr _Tmp0=buf;struct _fat_ptr _Tmp1=b->buffer;buf=_Tmp1;b->buffer=_Tmp0;});
if((char*)buf.curr!=(char*)_tag_fat(0,0,0).curr){
({struct _fat_ptr _Tmp0=buf;struct _fat_ptr _Tmp1=b->buffer;buf=_Tmp1;b->buffer=_Tmp0;});
({(void(*)(char*))Cyc_Core_ufree;})((char*)_untag_fat_ptr(buf,sizeof(char),1U + 1U));
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
if((char*)b->initial_buffer.curr!=(char*)_tag_fat(0,0,0).curr){
struct _fat_ptr buf=_tag_fat(0,0,0);
({struct _fat_ptr _Tmp0=b->initial_buffer;struct _fat_ptr _Tmp1=buf;b->initial_buffer=_Tmp1;buf=_Tmp0;});
b->length=_get_fat_size(buf,sizeof(char))- 1U;
({struct _fat_ptr _Tmp0=b->buffer;struct _fat_ptr _Tmp1=buf;b->buffer=_Tmp1;buf=_Tmp0;});
({(void(*)(char*))Cyc_Core_ufree;})((char*)_untag_fat_ptr(buf,sizeof(char),1U + 1U));}
# 106
return;}
# 109
static void Cyc_Buffer_resize(struct Cyc_Buffer_t*b,unsigned more){
unsigned long len=b->length;
unsigned long new_len=len==0U?1U: len;
struct _fat_ptr new_buffer;
while(b->position + more > new_len){
new_len=2U * new_len;}
# 116
new_buffer=({unsigned _Tmp0=new_len + 1U;_tag_fat(_region_calloc(Cyc_Core_unique_region,sizeof(char),_Tmp0),sizeof(char),_Tmp0);});
if(b->length!=0U){
struct _fat_ptr _Tmp0;_Tmp0=new_buffer;{struct _fat_ptr x=_Tmp0;
({struct _fat_ptr _Tmp1=_fat_ptr_decrease_size(x,sizeof(char),1U);struct _fat_ptr _Tmp2=b->buffer;Cyc_strncpy(_Tmp1,_Tmp2,b->position);});}}
# 121
if((char*)b->initial_buffer.curr==(char*)_tag_fat(0,0,0).curr)
({struct _fat_ptr _Tmp0=b->initial_buffer;struct _fat_ptr _Tmp1=b->buffer;b->initial_buffer=_Tmp1;b->buffer=_Tmp0;});
({struct _fat_ptr _Tmp0=b->buffer;struct _fat_ptr _Tmp1=new_buffer;b->buffer=_Tmp1;new_buffer=_Tmp0;});
({(void(*)(char*))Cyc_Core_ufree;})((char*)_untag_fat_ptr(new_buffer,sizeof(char),1U + 1U));
b->length=new_len;
return;}
# 129
void Cyc_Buffer_add_char(struct Cyc_Buffer_t*b,char c){
int pos=(int)b->position;
if((unsigned)pos >= b->length)Cyc_Buffer_resize(b,1U);
({struct _fat_ptr _Tmp0=_fat_ptr_plus(b->buffer,sizeof(char),pos);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=c;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
b->position=(unsigned)(pos + 1);
return;}
# 140
void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*b,struct _fat_ptr s,int offset,int len){
if((offset < 0 || len < 0)||(unsigned)(offset + len)> _get_fat_size(s,sizeof(char)))
(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=({const char*_Tmp1="Buffer::add_substring";_tag_fat(_Tmp1,sizeof(char),22U);});_Tmp0;}));{
int new_position=(int)(b->position + (unsigned)len);
if((unsigned)new_position > b->length)Cyc_Buffer_resize(b,(unsigned)len);{
struct _fat_ptr buf=_tag_fat(0,0,0);
({struct _fat_ptr _Tmp0=buf;struct _fat_ptr _Tmp1=b->buffer;buf=_Tmp1;b->buffer=_Tmp0;});
{struct _fat_ptr _Tmp0;_Tmp0=buf;{struct _fat_ptr x=_Tmp0;
({struct _fat_ptr _Tmp1=_fat_ptr_decrease_size(_fat_ptr_plus(x,sizeof(char),(int)b->position),sizeof(char),1U);struct _fat_ptr _Tmp2=_fat_ptr_plus(s,sizeof(char),offset);Cyc_zstrncpy(_Tmp1,_Tmp2,(unsigned long)len);});}}
# 150
({struct _fat_ptr _Tmp0=buf;struct _fat_ptr _Tmp1=b->buffer;buf=_Tmp1;b->buffer=_Tmp0;});
b->position=(unsigned)new_position;
return;}}}
# 156
void Cyc_Buffer_add_string(struct Cyc_Buffer_t*b,struct _fat_ptr s){
int len=(int)Cyc_strlen(s);
int new_position=(int)(b->position + (unsigned)len);
if((unsigned)new_position > b->length)Cyc_Buffer_resize(b,(unsigned)len);{
struct _fat_ptr buf=_tag_fat(0,0,0);
({struct _fat_ptr _Tmp0=buf;struct _fat_ptr _Tmp1=b->buffer;buf=_Tmp1;b->buffer=_Tmp0;});
{struct _fat_ptr _Tmp0;_Tmp0=buf;{struct _fat_ptr x=_Tmp0;
({struct _fat_ptr _Tmp1=({struct _fat_ptr _Tmp2=_fat_ptr_decrease_size(x,sizeof(char),1U);_fat_ptr_plus(_Tmp2,sizeof(char),(int)b->position);});struct _fat_ptr _Tmp2=s;Cyc_strncpy(_Tmp1,_Tmp2,(unsigned long)len);});}}
# 165
({struct _fat_ptr _Tmp0=buf;struct _fat_ptr _Tmp1=b->buffer;buf=_Tmp1;b->buffer=_Tmp0;});
b->position=(unsigned)new_position;
return;}}
# 170
void Cyc_Buffer_add_buffer(struct Cyc_Buffer_t*b,struct Cyc_Buffer_t*bs){
Cyc_Buffer_add_substring(b,bs->buffer,0,(int)bs->position);
return;}
