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
{ 
#ifdef CYC_REGION_PROFILE
  unsigned total_bytes;
  unsigned free_bytes;
#endif
  struct _RegionPage *next;
  char data[1];
};

struct _pool;
struct bget_region_key;
struct _RegionAllocFunctions;

struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  struct _RegionAllocFunctions *fcns;
  char               *offset;
  char               *last_plus_one;
  struct _pool *released_ptrs;
  struct bget_region_key *key;
#ifdef CYC_REGION_PROFILE
  const char *name;
#endif
  unsigned used_bytes;
  unsigned wasted_bytes;
};


// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

/* Alias qualifier stuff */
typedef unsigned int _AliasQualHandle_t; // must match aqualt_type() in toc.cyc

struct _RegionHandle _new_region(unsigned int, const char*);
void* _region_malloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned);
void* _region_calloc(struct _RegionHandle*, _AliasQualHandle_t, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void * _aqual_malloc(_AliasQualHandle_t aq, unsigned int s);
void * _aqual_calloc(_AliasQualHandle_t aq, unsigned int n, unsigned int t);
void _free_region(struct _RegionHandle*);

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
void* _throw_assert_fn(const char *,unsigned);
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw_assert() (_throw_assert_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
struct Cyc_Assert_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];
extern char Cyc_Assert[];

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
#define _zero_arr_plus_char_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_other_fn(t_sz,orig_x,orig_sz,orig_i,f,l)((orig_x)+(orig_i))
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
void* _zero_arr_plus_other_fn(unsigned,void*,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
unsigned _get_zero_arr_size_char(const char*,unsigned);
unsigned _get_zero_arr_size_other(unsigned,const void*,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
// note: must cast result in toc.cyc
void* _zero_arr_inplace_plus_other_fn(unsigned,void**,int,const char*,unsigned);
void* _zero_arr_inplace_plus_post_other_fn(unsigned,void**,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char**)(x),(i),__FILE__,__LINE__)
#define _zero_arr_plus_other(t,x,s,i) \
  (_zero_arr_plus_other_fn(t,x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_other(t,x,i) \
  _zero_arr_inplace_plus_other_fn(t,(void**)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_other(t,x,i) \
  _zero_arr_inplace_plus_post_other_fn(t,(void**)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ((arr).curr)
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
#define _untag_fat_ptr_check_bound(arr,elt_sz,num_elts) ({ \
  struct _fat_ptr _arr = (arr); \
  unsigned char *_curr = _arr.curr; \
  if ((_curr < _arr.base || _curr + (elt_sz) * (num_elts) > _arr.last_plus_one) &&\
      _curr != (unsigned char*)0) \
    _throw_arraybounds(); \
  _curr; })
#define _check_fat_at_base(arr) ({ \
  struct _fat_ptr _arr = (arr); \
  if (_arr.base != _arr.curr) _throw_arraybounds(); \
  _arr; })
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

#define _CYC_MAX_REGION_CONST 0
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, _AliasQualHandle_t aq, unsigned orig_s) {  
  if (r > (struct _RegionHandle*)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
  return _region_malloc(r,aq,orig_s); 
}

//doesn't make sense to fast malloc with reaps
#ifndef DISABLE_REAPS
#define _fast_region_malloc _region_malloc
#endif

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,_AliasQualHandle_t,unsigned,unsigned,const char *,const char*,int);
void * _profile_aqual_malloc(_AliasQualHandle_t aq, unsigned int s,const char *file, const char *func, int lineno);
void * _profile_aqual_calloc(_AliasQualHandle_t aq, unsigned int t1,unsigned int t2,const char *file, const char *func, int lineno);
struct _RegionHandle _profile_new_region(unsigned int i, const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(i,n) _profile_new_region(i,n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,aq,n) _profile_region_malloc(rh,aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,aq,n,t) _profile_region_calloc(rh,aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_malloc(aq,n) _profile_aqual_malloc(aq,n,__FILE__,__FUNCTION__,__LINE__)
#define _aqual_calloc(aq,n,t) _profile_aqual_calloc(aq,n,t,__FILE__,__FUNCTION__,__LINE__)
#endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif //CYC_REGION_PROFILE
#endif //_CYC_INCLUDE_H

# 300 "cycboot.h"
 extern int isspace(int);
# 339
extern const long Cyc_long_max;extern const long Cyc_long_min;
# 82 "string.h"
extern struct _fat_ptr Cyc__memcpy(struct _fat_ptr,struct _fat_ptr,unsigned long,unsigned);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);
# 7 "ap.h"
extern struct Cyc_AP_T*Cyc_AP_zero;
extern struct Cyc_AP_T*Cyc_AP_one;
# 10
struct Cyc_AP_T*Cyc_AP_fromint(long);
# 18
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 20
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*,struct Cyc_AP_T*);
# 22
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*,long);
# 28
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*,int);
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
# 18 "ap.cyc"
struct Cyc_AP_T*Cyc_AP_zero;
struct Cyc_AP_T*Cyc_AP_one;
int Cyc_init=0;
# 22
static struct Cyc_AP_T*Cyc_normalize(struct Cyc_AP_T*,int);
# 24
static void Cyc_AP_init (void){
Cyc_init=1;
Cyc_AP_zero=Cyc_AP_fromint(0);
Cyc_AP_one=Cyc_AP_fromint(1);}
# 29
static struct Cyc_AP_T*Cyc_mk(int size){
struct Cyc_AP_T*z;
if(!Cyc_init)Cyc_AP_init();
z=(struct Cyc_AP_T*)_cycalloc(sizeof(struct Cyc_AP_T));
z->digits=({unsigned _Tmp0=size;_tag_fat(_cyccalloc_atomic(sizeof(unsigned char),_Tmp0),sizeof(unsigned char),_Tmp0);});
size > 0?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("size > 0",sizeof(char),9U),_tag_fat("ap.cyc",sizeof(char),7U),34U);
z->sign=1;
z->size=size;
z->ndigits=1;
return z;}
# 40
static struct Cyc_AP_T*Cyc_set(struct Cyc_AP_T*z,long n){
if(n==Cyc_long_min)
Cyc_XP_fromint(_check_null(z)->size,z->digits,(unsigned)Cyc_long_max + 1U);else{
if(n < 0)
Cyc_XP_fromint(_check_null(z)->size,z->digits,(unsigned long)(- n));else{
# 46
Cyc_XP_fromint(_check_null(z)->size,z->digits,(unsigned long)n);}}
z->sign=n < 0?-1: 1;
return Cyc_normalize(z,z->size);}
# 50
static struct Cyc_AP_T*Cyc_normalize(struct Cyc_AP_T*z,int n){
({int _Tmp0=Cyc_XP_length(n,_check_null(z)->digits);z->ndigits=_Tmp0;});
return z;}
# 54
static struct Cyc_AP_T*Cyc_add(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int n=_check_null(y)->ndigits;
if(_check_null(x)->ndigits < n)
return Cyc_add(z,y,x);else{
if(x->ndigits > n){
int carry=Cyc_XP_add(n,_check_null(z)->digits,x->digits,y->digits,0);
# 61
({unsigned char _Tmp0=(unsigned char)({int _Tmp1=x->ndigits - n;struct _fat_ptr _Tmp2=
_fat_ptr_plus(z->digits,sizeof(unsigned char),n);
# 61
struct _fat_ptr _Tmp3=
_fat_ptr_plus(x->digits,sizeof(unsigned char),n);
# 61
Cyc_XP_sum(_Tmp1,_Tmp2,_Tmp3,carry);});*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),z->size - 1))=_Tmp0;});}else{
# 64
({unsigned char _Tmp0=(unsigned char)Cyc_XP_add(n,_check_null(z)->digits,x->digits,y->digits,0);*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),n))=_Tmp0;});}}
# 66
return Cyc_normalize(z,z->size);}
# 68
static struct Cyc_AP_T*Cyc_sub(struct Cyc_AP_T*z,struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int borrow;int n=_check_null(y)->ndigits;
borrow=({int _Tmp0=n;struct _fat_ptr _Tmp1=_check_null(z)->digits;struct _fat_ptr _Tmp2=_check_null(x)->digits;Cyc_XP_sub(_Tmp0,_Tmp1,_Tmp2,y->digits,0);});
# 72
if(x->ndigits > n)
borrow=({int _Tmp0=x->ndigits - n;struct _fat_ptr _Tmp1=_fat_ptr_plus(z->digits,sizeof(unsigned char),n);struct _fat_ptr _Tmp2=
_fat_ptr_plus(x->digits,sizeof(unsigned char),n);
# 73
Cyc_XP_diff(_Tmp0,_Tmp1,_Tmp2,borrow);});
# 75
borrow==0?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("borrow == 0",sizeof(char),12U),_tag_fat("ap.cyc",sizeof(char),7U),75U);
return Cyc_normalize(z,z->size);}
# 78
static struct Cyc_AP_T*Cyc_mulmod(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){
struct Cyc_AP_T*z;struct Cyc_AP_T*xy=Cyc_AP_mul(x,y);
z=Cyc_AP_mod(xy,p);
return z;}
# 83
static int Cyc_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
if(({int _Tmp0=_check_null(x)->ndigits;_Tmp0!=_check_null(y)->ndigits;}))
return x->ndigits - y->ndigits;else{
# 87
return Cyc_XP_cmp(x->ndigits,x->digits,y->digits);}}
# 89
struct Cyc_AP_T*Cyc_AP_new(long n){
struct Cyc_AP_T*_Tmp0=Cyc_mk((int)sizeof(long));return Cyc_set(_Tmp0,n);}
# 92
struct Cyc_AP_T*Cyc_AP_neg(struct Cyc_AP_T*x){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),94U);
z=Cyc_mk(x->ndigits);
Cyc__memcpy(z->digits,x->digits,(unsigned)x->ndigits / sizeof(*((unsigned char*)x->digits.curr))+ (unsigned)((unsigned)x->ndigits % sizeof(*((unsigned char*)x->digits.curr))==0U?0: 1),sizeof(*((unsigned char*)x->digits.curr)));
z->ndigits=x->ndigits;
z->sign=z->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0?1: - x->sign;
return z;}
# 101
struct Cyc_AP_T*Cyc_AP_abs(struct Cyc_AP_T*x){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),103U);
z=Cyc_mk(x->ndigits);
Cyc__memcpy(z->digits,x->digits,(unsigned)x->ndigits / sizeof(*((unsigned char*)x->digits.curr))+ (unsigned)((unsigned)x->ndigits % sizeof(*((unsigned char*)x->digits.curr))==0U?0: 1),sizeof(*((unsigned char*)x->digits.curr)));
z->ndigits=x->ndigits;
z->sign=1;
return z;}
# 110
struct Cyc_AP_T*Cyc_AP_mul(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),112U);
(unsigned)y?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),113U);
z=Cyc_mk(x->ndigits + y->ndigits);
Cyc_XP_mul(z->digits,x->ndigits,x->digits,y->ndigits,y->digits);
# 117
Cyc_normalize(z,z->size);
z->sign=(z->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0 ||(x->sign ^ y->sign)==0)?1: -1;
# 120
return z;}
# 122
struct Cyc_AP_T*Cyc_AP_add(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),124U);
(unsigned)y?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),125U);
if((x->sign ^ y->sign)==0){
z=({struct Cyc_AP_T*_Tmp0=Cyc_mk((x->ndigits > y->ndigits?x->ndigits: y->ndigits)+ 1);struct Cyc_AP_T*_Tmp1=x;Cyc_add(_Tmp0,_Tmp1,y);});
({int _Tmp0=_check_null(z)->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0?1: x->sign;z->sign=_Tmp0;});}else{
# 130
if(Cyc_cmp(x,y)> 0){
z=({struct Cyc_AP_T*_Tmp0=Cyc_mk(x->ndigits);struct Cyc_AP_T*_Tmp1=x;Cyc_sub(_Tmp0,_Tmp1,y);});
z->sign=z->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0?1: x->sign;}else{
# 135
z=({struct Cyc_AP_T*_Tmp0=Cyc_mk(y->ndigits);struct Cyc_AP_T*_Tmp1=y;Cyc_sub(_Tmp0,_Tmp1,x);});
z->sign=z->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0?1: - x->sign;}}
# 138
return z;}
# 140
struct Cyc_AP_T*Cyc_AP_sub(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),142U);
(unsigned)y?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),143U);
if(!((x->sign ^ y->sign)==0)){
z=({struct Cyc_AP_T*_Tmp0=Cyc_mk((x->ndigits > y->ndigits?x->ndigits: y->ndigits)+ 1);struct Cyc_AP_T*_Tmp1=x;Cyc_add(_Tmp0,_Tmp1,y);});
({int _Tmp0=_check_null(z)->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0?1: x->sign;z->sign=_Tmp0;});}else{
# 148
if(Cyc_cmp(x,y)> 0){
z=({struct Cyc_AP_T*_Tmp0=Cyc_mk(x->ndigits);struct Cyc_AP_T*_Tmp1=x;Cyc_sub(_Tmp0,_Tmp1,y);});
z->sign=z->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0?1: x->sign;}else{
# 152
z=({struct Cyc_AP_T*_Tmp0=Cyc_mk(y->ndigits);struct Cyc_AP_T*_Tmp1=y;Cyc_sub(_Tmp0,_Tmp1,x);});
z->sign=z->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0?1: - x->sign;}}
# 155
return z;}
# 157
struct Cyc_AP_T*Cyc_AP_div(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*q;struct Cyc_AP_T*r;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),159U);
(unsigned)y?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),160U);
!(y->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))==0)?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("!((y)->ndigits==1 && (y)->digits[0]==0)",sizeof(char),40U),_tag_fat("ap.cyc",sizeof(char),7U),161U);
q=Cyc_mk(x->ndigits);
r=Cyc_mk(y->ndigits);
{
struct _fat_ptr tmp=({unsigned _Tmp0=(unsigned)((x->ndigits + y->ndigits)+ 2)* sizeof(unsigned char);_tag_fat(_cycalloc_atomic(_Tmp0),1U,_Tmp0);});
Cyc_XP_div(x->ndigits,q->digits,x->digits,y->ndigits,y->digits,r->digits,tmp);}
# 169
Cyc_normalize(q,q->size);
Cyc_normalize(r,r->size);
q->sign=(q->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(q->digits,sizeof(unsigned char),0))==0 ||(x->sign ^ y->sign)==0)?1: -1;
# 173
return q;}
# 175
struct Cyc_AP_T*Cyc_AP_mod(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*q;struct Cyc_AP_T*r;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),177U);
(unsigned)y?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),178U);
!(y->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))==0)?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("!((y)->ndigits==1 && (y)->digits[0]==0)",sizeof(char),40U),_tag_fat("ap.cyc",sizeof(char),7U),179U);
q=Cyc_mk(x->ndigits);
r=Cyc_mk(y->ndigits);
{
struct _fat_ptr tmp=({unsigned _Tmp0=(unsigned)((x->ndigits + y->ndigits)+ 2)* sizeof(unsigned char);_tag_fat(_cycalloc_atomic(_Tmp0),1U,_Tmp0);});
Cyc_XP_div(x->ndigits,q->digits,x->digits,y->ndigits,y->digits,r->digits,tmp);}
# 187
Cyc_normalize(q,q->size);
Cyc_normalize(r,r->size);
r->sign=r->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(r->digits,sizeof(unsigned char),0))==0?1: x->sign;
return r;}
# 192
struct Cyc_AP_T*Cyc_AP_pow(struct Cyc_AP_T*x,struct Cyc_AP_T*y,struct Cyc_AP_T*p){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),194U);
(unsigned)y?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),195U);
y->sign==1?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y->sign == 1",sizeof(char),13U),_tag_fat("ap.cyc",sizeof(char),7U),196U);
(!((unsigned)p)||(p->sign==1 && !(p->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(p->digits,sizeof(unsigned char),0))==0))&& !(p->ndigits==1 &&(int)((unsigned char*)p->digits.curr)[0]==1))?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("!p || p->sign==1 && !((p)->ndigits==1 && (p)->digits[0]==0) && !((p)->ndigits==1 && (p)->digits[0]==1)",sizeof(char),103U),_tag_fat("ap.cyc",sizeof(char),7U),197U);
if(x->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(x->digits,sizeof(unsigned char),0))==0)
return Cyc_AP_new(0);
if(y->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))==0)
return Cyc_AP_new(1);
if(x->ndigits==1 &&(int)((unsigned char*)x->digits.curr)[0]==1)
return Cyc_AP_new(((int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))& 1)==0?1: x->sign);
if((unsigned)p){
if(y->ndigits==1 &&(int)((unsigned char*)y->digits.curr)[0]==1)
z=Cyc_AP_mod(x,p);else{
# 208
struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,p);
z=Cyc_mulmod(t,t,p);
if(!(((int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))& 1)==0))
z=({struct Cyc_AP_T*_Tmp0=y2=Cyc_AP_mod(x,p);struct Cyc_AP_T*_Tmp1=t=z;Cyc_mulmod(_Tmp0,_Tmp1,p);});}}else{
# 215
if(y->ndigits==1 &&(int)((unsigned char*)y->digits.curr)[0]==1)
z=Cyc_AP_addi(x,0);else{
# 218
struct Cyc_AP_T*y2=Cyc_AP_rshift(y,1);struct Cyc_AP_T*t=Cyc_AP_pow(x,y2,0);
z=Cyc_AP_mul(t,t);
if(!(((int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))& 1)==0))
z=({struct Cyc_AP_T*_Tmp0=x;Cyc_AP_mul(_Tmp0,(t=z));});}}
# 224
return z;}
# 226
int Cyc_AP_cmp(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),227U);
(unsigned)y?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),228U);
if(!((x->sign ^ y->sign)==0))
return x->sign;else{
if(x->sign==1)
return Cyc_cmp(x,y);else{
# 234
return Cyc_cmp(y,x);}}}
# 236
struct Cyc_AP_T*Cyc_AP_addi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
struct Cyc_AP_T*_Tmp0=x;return Cyc_AP_add(_Tmp0,Cyc_set(t,y));}
# 240
struct Cyc_AP_T*Cyc_AP_subi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
struct Cyc_AP_T*_Tmp0=x;return Cyc_AP_sub(_Tmp0,Cyc_set(t,y));}
# 244
struct Cyc_AP_T*Cyc_AP_muli(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
struct Cyc_AP_T*_Tmp0=x;return Cyc_AP_mul(_Tmp0,Cyc_set(t,y));}
# 248
struct Cyc_AP_T*Cyc_AP_divi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
struct Cyc_AP_T*_Tmp0=x;return Cyc_AP_div(_Tmp0,Cyc_set(t,y));}
# 252
int Cyc_AP_cmpi(struct Cyc_AP_T*x,long y){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
struct Cyc_AP_T*_Tmp0=x;return Cyc_AP_cmp(_Tmp0,Cyc_set(t,y));}
# 256
long Cyc_AP_modi(struct Cyc_AP_T*x,long y){
long rem;
struct Cyc_AP_T*r;
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
r=({struct Cyc_AP_T*_Tmp0=x;Cyc_AP_mod(_Tmp0,Cyc_set(t,y));});
rem=(long)Cyc_XP_toint(r->ndigits,r->digits);
return rem;}
# 264
struct Cyc_AP_T*Cyc_AP_lshift(struct Cyc_AP_T*x,int s){
struct Cyc_AP_T*z;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),266U);
s >= 0?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("s >= 0",sizeof(char),7U),_tag_fat("ap.cyc",sizeof(char),7U),267U);
z=Cyc_mk(x->ndigits + (s + 7 & 4294967288U)/ 8);
Cyc_XP_lshift(z->size,z->digits,x->ndigits,x->digits,s,0);
# 271
z->sign=x->sign;
return Cyc_normalize(z,z->size);}
# 274
struct Cyc_AP_T*Cyc_AP_rshift(struct Cyc_AP_T*x,int s){
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),275U);
s >= 0?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("s >= 0",sizeof(char),7U),_tag_fat("ap.cyc",sizeof(char),7U),276U);
if(s >= 8 * x->ndigits)
return Cyc_AP_new(0);else{
# 280
struct Cyc_AP_T*z=Cyc_mk(x->ndigits - s / 8);
Cyc_XP_rshift(z->size,z->digits,x->ndigits,x->digits,s,0);
# 283
Cyc_normalize(z,z->size);
z->sign=z->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0?1: x->sign;
return z;}}
# 288
struct Cyc_AP_T*Cyc_AP_and(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _Tmp0=_check_null(x)->ndigits;_Tmp0 < _check_null(y)->ndigits;}))
return Cyc_AP_and(y,x);
z=Cyc_mk(y->ndigits);
Cyc_XP_and(y->ndigits,z->digits,x->digits,y->digits);
Cyc_normalize(z,z->size);
z->sign=1;
return z;}
# 299
struct Cyc_AP_T*Cyc_AP_or(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _Tmp0=_check_null(x)->ndigits;_Tmp0 < _check_null(y)->ndigits;}))
return Cyc_AP_or(y,x);
z=Cyc_mk(x->ndigits);
Cyc_XP_or(y->ndigits,z->digits,x->digits,y->digits);
for(i=y->ndigits;i < x->ndigits;++ i){
Cyc_normalize(z,z->size);}
z->sign=1;
return z;}
# 311
struct Cyc_AP_T*Cyc_AP_xor(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
int i;
struct Cyc_AP_T*z;
if(({int _Tmp0=_check_null(x)->ndigits;_Tmp0 < _check_null(y)->ndigits;}))
return Cyc_AP_xor(y,x);
z=Cyc_mk(x->ndigits);
Cyc_XP_xor(y->ndigits,z->digits,x->digits,y->digits);
for(i=y->ndigits;i < x->ndigits;++ i){
Cyc_normalize(z,z->size);}
z->sign=1;
return z;}
# 323
struct Cyc_AP_T*Cyc_AP_fromint(long x){
struct Cyc_AP_T*t=Cyc_mk((int)(sizeof(unsigned long)/ sizeof(unsigned char)));
return Cyc_set(t,x);}
# 327
long Cyc_AP_toint(struct Cyc_AP_T*x){
unsigned long u;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),329U);
u=({unsigned long _Tmp0=Cyc_XP_toint(x->ndigits,x->digits);_Tmp0 % ((unsigned)Cyc_long_max + 1U);});
if(x->sign==-1)
return -(long)u;else{
# 334
return(long)u;}}
# 336
struct Cyc_AP_T*Cyc_AP_fromstr(const char*str,int base){
struct Cyc_AP_T*z;
struct _fat_ptr p=({const char*_Tmp0=str;_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));});
char sign='\000';
int carry;
(unsigned)p.curr?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("p",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),341U);
base >= 2 && base <= 36?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("base >= 2 && base <= 36",sizeof(char),24U),_tag_fat("ap.cyc",sizeof(char),7U),342U);
while((int)*((const char*)_check_fat_subscript(p,sizeof(char),0U))&& isspace((int)*((const char*)p.curr))){
_fat_ptr_inplace_plus(& p,sizeof(char),1);1U;}
if((int)*((const char*)p.curr)==45 ||(int)*((const char*)p.curr)==43)
sign=*((const char*)_fat_ptr_inplace_plus_post(& p,sizeof(char),1).curr);
{
const char*start;
int k;int n=0;
for(1;(int)*((const char*)_check_fat_subscript(p,sizeof(char),0U))==48 &&(int)*((const char*)_check_fat_subscript(p,sizeof(char),1))==48;_fat_ptr_inplace_plus(& p,sizeof(char),1)){
;}
start=(const char*)_untag_fat_ptr(p,sizeof(char),1U);
for(1;((48 <= (int)*((const char*)_check_fat_subscript(p,sizeof(char),0U))&&(int)*((const char*)p.curr)<= 57)&&(int)*((const char*)p.curr)< 48 + base ||
(97 <= (int)*((const char*)p.curr)&&(int)*((const char*)p.curr)<= 122)&&(int)*((const char*)p.curr)< (97 + base)- 10)||
(65 <= (int)*((const char*)p.curr)&&(int)*((const char*)p.curr)<= 90)&&(int)*((const char*)p.curr)< (65 + base)- 10;_fat_ptr_inplace_plus(& p,sizeof(char),1)){
++ n;}
for(k=1;1 << k < base;++ k){
;}
z=Cyc_mk((k * n + 7 & 4294967288U)/ 8);
p=({const char*_Tmp0=start;_tag_fat((void*)_Tmp0,sizeof(char),_get_zero_arr_size_char((void*)_Tmp0,1U));});}
# 362
carry=Cyc_XP_fromstr(z->size,z->digits,(const char*)_untag_fat_ptr(p,sizeof(char),1U),base);
# 364
carry==0?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("carry == 0",sizeof(char),11U),_tag_fat("ap.cyc",sizeof(char),7U),364U);
Cyc_normalize(z,z->size);
z->sign=(z->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(z->digits,sizeof(unsigned char),0))==0 ||(int)sign!=45)?1: -1;
return z;}
# 369
char*Cyc_AP_tostr(struct Cyc_AP_T*x,int base){
struct _fat_ptr q;
struct _fat_ptr str;
int size;int k;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),373U);
base >= 2 && base <= 36?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("base >= 2 && base <= 36",sizeof(char),24U),_tag_fat("ap.cyc",sizeof(char),7U),374U);
for(k=5;1 << k > base;-- k){
;}
size=((8 * x->ndigits)/ k + 1)+ 1;
if(x->sign==-1)
++ size;
str=({unsigned _Tmp0=size;_tag_fat(_cyccalloc_atomic(sizeof(char),_Tmp0),sizeof(char),_Tmp0);});
q=({unsigned _Tmp0=(unsigned)x->ndigits * sizeof(unsigned char);_tag_fat(_cycalloc_atomic(_Tmp0),1U,_Tmp0);});
Cyc__memcpy(q,x->digits,(unsigned)x->ndigits / sizeof(*((unsigned char*)x->digits.curr))+ (unsigned)((unsigned)x->ndigits % sizeof(*((unsigned char*)x->digits.curr))==0U?0: 1),sizeof(*((unsigned char*)x->digits.curr)));
if(x->sign==-1){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(str,sizeof(char),0);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='-';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_plus(str,sizeof(char),1);int _Tmp1=size - 1;int _Tmp2=base;int _Tmp3=x->ndigits;Cyc_XP_tostr(_Tmp0,_Tmp1,_Tmp2,_Tmp3,q);});}else{
# 387
Cyc_XP_tostr(str,size,base,x->ndigits,q);}
return(char*)_untag_fat_ptr_check_bound(str,sizeof(char),1U);}
# 390
struct Cyc_AP_T*Cyc_AP_gcd(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*z;struct Cyc_AP_T*tmp;
(unsigned)x?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("x",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),392U);
(unsigned)y?0:({int(*_Tmp0)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))Cyc___assert_fail;_Tmp0;})(_tag_fat("y",sizeof(char),2U),_tag_fat("ap.cyc",sizeof(char),7U),393U);
while(!(_check_null(y)->ndigits==1 &&(int)*((unsigned char*)_check_fat_subscript(y->digits,sizeof(unsigned char),0))==0)){
tmp=Cyc_AP_mod(x,y);
x=y;
y=tmp;
# 395
1U;}
# 399
z=Cyc_mk(x->ndigits);
Cyc__memcpy(z->digits,x->digits,(unsigned)x->ndigits / sizeof(*((unsigned char*)x->digits.curr))+ (unsigned)((unsigned)x->ndigits % sizeof(*((unsigned char*)x->digits.curr))==0U?0: 1),sizeof(*((unsigned char*)x->digits.curr)));
z->ndigits=x->ndigits;
z->sign=x->sign;
return z;}
# 405
struct Cyc_AP_T*Cyc_AP_lcm(struct Cyc_AP_T*x,struct Cyc_AP_T*y){
struct Cyc_AP_T*_Tmp0=x;return Cyc_AP_mul(_Tmp0,({struct Cyc_AP_T*_Tmp1=y;Cyc_AP_div(_Tmp1,Cyc_AP_gcd(x,y));}));}
