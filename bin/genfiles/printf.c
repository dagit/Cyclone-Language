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
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};
# 173 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 41 "cycboot.h"
extern double modf(double,double*);struct Cyc___cycFILE;
# 51
extern struct Cyc___cycFILE*Cyc_stdout;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};struct Cyc_Double_pa_PrintArg_struct{int tag;double f1;};struct Cyc_LongDouble_pa_PrintArg_struct{int tag;long double f1;};struct Cyc_ShortPtr_pa_PrintArg_struct{int tag;short*f1;};struct Cyc_IntPtr_pa_PrintArg_struct{int tag;unsigned long*f1;};
# 73
struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);
# 161 "cycboot.h"
extern int Cyc_putc(int,struct Cyc___cycFILE*);
# 105 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);
# 87 "printf.cyc"
static struct _fat_ptr Cyc_parg2string(void*x){
switch(*((int*)x)){case 0:
 return _tag_fat("string",sizeof(char),7U);case 1:
 return _tag_fat("int",sizeof(char),4U);case 2:
# 92
 return _tag_fat("double",sizeof(char),7U);case 3:
 return _tag_fat("long double",sizeof(char),12U);case 4:
 return _tag_fat("short *",sizeof(char),8U);default:
 return _tag_fat("unsigned long *",sizeof(char),16U);};}
# 99
static void*Cyc_badarg(struct _fat_ptr s){
return(void*)_throw((void*)({struct Cyc_Core_Invalid_argument_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_Tmp0->tag=Cyc_Core_Invalid_argument,_Tmp0->f1=s;_Tmp0;}));}
# 104
static int Cyc_va_arg_int(struct _fat_ptr ap){
void*_Tmp0=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));unsigned long _Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_Int_pa_PrintArg_struct*)_Tmp0)->f1;{unsigned long i=_Tmp1;
return(int)i;}}else{
return({int(*_Tmp2)(struct _fat_ptr)=(int(*)(struct _fat_ptr))Cyc_badarg;_Tmp2;})(_tag_fat("printf expected int",sizeof(char),20U));};}
# 111
static long Cyc_va_arg_long(struct _fat_ptr ap){
void*_Tmp0=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));unsigned long _Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_Int_pa_PrintArg_struct*)_Tmp0)->f1;{unsigned long i=_Tmp1;
return(long)i;}}else{
return({long(*_Tmp2)(struct _fat_ptr)=(long(*)(struct _fat_ptr))Cyc_badarg;_Tmp2;})(_tag_fat("printf expected int",sizeof(char),20U));};}
# 118
static unsigned long Cyc_va_arg_ulong(struct _fat_ptr ap){
void*_Tmp0=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));unsigned long _Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_Int_pa_PrintArg_struct*)_Tmp0)->f1;{unsigned long i=_Tmp1;
return i;}}else{
return({unsigned long(*_Tmp2)(struct _fat_ptr)=(unsigned long(*)(struct _fat_ptr))Cyc_badarg;_Tmp2;})(_tag_fat("printf expected int",sizeof(char),20U));};}
# 125
static unsigned long Cyc_va_arg_uint(struct _fat_ptr ap){
void*_Tmp0=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));unsigned long _Tmp1;if(*((int*)_Tmp0)==1){_Tmp1=((struct Cyc_Int_pa_PrintArg_struct*)_Tmp0)->f1;{unsigned long i=_Tmp1;
return i;}}else{
return({unsigned long(*_Tmp2)(struct _fat_ptr)=(unsigned long(*)(struct _fat_ptr))Cyc_badarg;_Tmp2;})(_tag_fat("printf expected int",sizeof(char),20U));};}
# 133
static double Cyc_va_arg_double(struct _fat_ptr ap){
void*_Tmp0=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));long double _Tmp1;double _Tmp2;switch(*((int*)_Tmp0)){case 2: _Tmp2=((struct Cyc_Double_pa_PrintArg_struct*)_Tmp0)->f1;{double d=_Tmp2;
return d;}case 3: _Tmp1=((struct Cyc_LongDouble_pa_PrintArg_struct*)_Tmp0)->f1;{long double ld=_Tmp1;
return(double)ld;}default:
# 138
 _throw(Cyc_badarg(({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,({struct _fat_ptr _Tmp5=Cyc_parg2string(*((void**)_check_fat_subscript(ap,sizeof(void*),0U)));_Tmp4.f1=_Tmp5;});_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_aprintf(_tag_fat("printf expected double but found %s",sizeof(char),36U),_tag_fat(_Tmp4,sizeof(void*),1));})));};}
# 144
static short*Cyc_va_arg_short_ptr(struct _fat_ptr ap){
void*_Tmp0=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_Tmp1;if(*((int*)_Tmp0)==4){_Tmp1=((struct Cyc_ShortPtr_pa_PrintArg_struct*)_Tmp0)->f1;{short*p=_Tmp1;
return p;}}else{
_throw(Cyc_badarg(_tag_fat("printf expected short pointer",sizeof(char),30U)));};}
# 152
static unsigned long*Cyc_va_arg_int_ptr(struct _fat_ptr ap){
void*_Tmp0=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));void*_Tmp1;if(*((int*)_Tmp0)==5){_Tmp1=((struct Cyc_IntPtr_pa_PrintArg_struct*)_Tmp0)->f1;{unsigned long*p=_Tmp1;
return p;}}else{
_throw(Cyc_badarg(_tag_fat("printf expected int pointer",sizeof(char),28U)));};}
# 160
static const struct _fat_ptr Cyc_va_arg_string(struct _fat_ptr ap){
void*_Tmp0=*((void**)_check_fat_subscript(ap,sizeof(void*),0U));struct _fat_ptr _Tmp1;if(*((int*)_Tmp0)==0){_Tmp1=((struct Cyc_String_pa_PrintArg_struct*)_Tmp0)->f1;{struct _fat_ptr s=_Tmp1;
return s;}}else{
_throw(Cyc_badarg(_tag_fat("printf expected string",sizeof(char),23U)));};}
# 177 "printf.cyc"
int Cyc___cvt_double(double,int,int,int*,int,struct _fat_ptr,struct _fat_ptr);
# 206 "printf.cyc"
enum Cyc_BASE{Cyc_OCT =0U,Cyc_DEC =1U,Cyc_HEX =2U};
# 212
inline static int Cyc__IO_sputn(int(*ioputc)(int,void*),void*ioputc_env,struct _fat_ptr s,int howmany){
# 214
int i=0;
while(i < howmany){
if(ioputc((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U)),ioputc_env)==-1)return i;
_fat_ptr_inplace_plus(& s,sizeof(char),1);
++ i;
# 216
1U;}
# 220
return i;}
# 223
static int Cyc__IO_nzsputn(int(*ioputc)(int,void*),void*ioputc_env,struct _fat_ptr s,int howmany){
# 225
int i=0;
while(i < howmany){
{char c;
if((int)(c=*((const char*)_check_fat_subscript(s,sizeof(char),0U)))==0 || ioputc((int)c,ioputc_env)==-1)return i;
_fat_ptr_inplace_plus(& s,sizeof(char),1);
++ i;}
# 227
1U;}
# 232
return i;}
# 238
static int Cyc__IO_padn(int(*ioputc)(int,void*),void*ioputc_env,char c,int howmany){
# 240
int i=0;
while(i < howmany){
if(ioputc((int)c,ioputc_env)==-1)return i;
++ i;
# 242
1U;}
# 245
return i;}
# 249
static struct _fat_ptr Cyc_my_memchr(struct _fat_ptr s,char c,int n){
int sz=(int)_get_fat_size(s,sizeof(char));
n=n < sz?n: sz;
for(1;n!=0;(n --,_fat_ptr_inplace_plus_post(& s,sizeof(char),1))){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U))==(int)c)return s;}
# 255
return _tag_fat(0,0,0);}
# 258
static struct _fat_ptr Cyc_my_nzmemchr(struct _fat_ptr s,char c,int n){
int sz=(int)_get_fat_size(s,sizeof(char));
n=n < sz?n: sz;
for(1;n!=0;(n --,_fat_ptr_inplace_plus_post(& s,sizeof(char),1))){
if((int)*((const char*)_check_fat_subscript(s,sizeof(char),0U))==(int)c)return s;}
# 264
return _tag_fat(0,0,0);}
# 267
inline static const unsigned long Cyc_my_strlen(struct _fat_ptr s){
unsigned sz=_get_fat_size(s,sizeof(char));
unsigned i=0U;
while(i < sz &&(int)((const char*)s.curr)[(int)i]!=0){++ i;1U;}
return i;}
# 278
int Cyc__IO_vfprintf(int(*ioputc)(int,void*),void*ioputc_env,struct _fat_ptr fmt0,struct _fat_ptr ap){
# 280
struct _fat_ptr fmt;
register int ch;
register int n;
# 284
struct _fat_ptr cp=_tag_fat(0,0,0);
# 287
struct _fat_ptr cp2=_tag_fat(0,0,0);
# 290
struct _fat_ptr cp3=_tag_fat(0,0,0);
# 293
int which_cp;
struct _fat_ptr fmark;
register int flags;
int ret;
int width;
int prec;
char sign;
# 301
char sign_string[2U];sign_string[0]='\000',sign_string[1]='\000';{
int softsign=0;
double _double;
int fpprec;
unsigned long _ulong;
int dprec;
int dpad;
int fieldsz;
# 312
int size=0;
# 314
char buf[349U];({{unsigned _Tmp0=348U;unsigned i;for(i=0;i < _Tmp0;++ i){buf[i]='\000';}buf[_Tmp0]=0;}0;});{
char ox[2U];ox[0]='\000',ox[1]='\000';{
enum Cyc_BASE base;
# 340 "printf.cyc"
fmt=fmt0;
ret=0;
# 346
for(1;1;1){
# 349
fmark=fmt;{
unsigned fmt_sz=_get_fat_size(fmt,sizeof(char));
for(n=0;((unsigned)n < fmt_sz &&(ch=(int)*((const char*)_check_fat_subscript(fmt,sizeof(char),n)))!=0)&& ch!=37;++ n){
;}
fmt=_fat_ptr_plus(fmt,sizeof(char),n);
# 355
if((n=(fmt.curr - fmark.curr)/ sizeof(char))!=0){
do{if(({int _Tmp0=Cyc__IO_sputn(ioputc,ioputc_env,(struct _fat_ptr)fmark,n);_Tmp0!=n;}))goto error;}while(0);
ret +=n;}
# 359
if(ch==0)
goto done;
_fat_ptr_inplace_plus(& fmt,sizeof(char),1);
# 363
flags=0;
dprec=0;
fpprec=0;
width=0;
prec=-1;
sign='\000';
# 370
rflag: ch=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
reswitch: which_cp=0;
switch((int)ch){case 32:
# 379
 if(!((int)sign))
sign=' ';
goto rflag;case 35:
# 383
 flags |=8;
goto rflag;case 42:
# 392
 width=Cyc_va_arg_int(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
if(width >= 0)
goto rflag;
width=- width;
goto _LL8;case 45: _LL8:
# 398
 flags |=16;
flags &=4294967263U;
goto rflag;case 43:
# 402
 sign='+';
goto rflag;case 46:
# 405
 if((ch=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U)))==42){
n=Cyc_va_arg_int(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
prec=n < 0?-1: n;
goto rflag;}
# 410
n=0;
while((unsigned)(ch - 48)<= 9U){
n=10 * n + (ch - 48);
ch=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));
# 412
1U;}
# 415
prec=n < 0?-1: n;
goto reswitch;case 48:
# 423
 if(!(flags & 16))
flags |=32;
goto rflag;case 49:
 goto _LL12;case 50: _LL12: goto _LL14;case 51: _LL14: goto _LL16;case 52: _LL16: goto _LL18;case 53: _LL18:
 goto _LL1A;case 54: _LL1A: goto _LL1C;case 55: _LL1C: goto _LL1E;case 56: _LL1E: goto _LL20;case 57: _LL20:
 n=0;
do{
n=10 * n + (ch - 48);
ch=(int)*((const char*)_check_fat_subscript(_fat_ptr_inplace_plus_post(& fmt,sizeof(char),1),sizeof(char),0U));}while((unsigned)(ch - 48)<= 9U);
# 433
width=n;
goto reswitch;case 76:
# 436
 flags |=2;
goto rflag;case 104:
# 439
 flags |=4;
goto rflag;case 108:
# 442
 flags |=1;
goto rflag;case 99:
# 445
 cp=_tag_fat(buf,sizeof(char),349U);
({struct _fat_ptr _Tmp0=cp;char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)Cyc_va_arg_int(ap);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
size=1;
sign='\000';
goto _LL0;case 68:
# 451
 flags |=1;
goto _LL2C;case 100: _LL2C:
 goto _LL2E;case 105: _LL2E:
 _ulong=(unsigned long)(flags & 1?Cyc_va_arg_long(ap):(flags & 4?(long)((short)Cyc_va_arg_int(ap)): Cyc_va_arg_int(ap)));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
if((long)_ulong < 0){
_ulong=- _ulong;
sign='-';}
# 459
base=1U;
goto number;case 101:
 goto _LL32;case 69: _LL32: goto _LL34;case 102: _LL34: goto _LL36;case 70: _LL36: goto _LL38;case 103: _LL38:
 goto _LL3A;case 71: _LL3A:
 _double=Cyc_va_arg_double(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
# 468
if(prec > 39){
if(ch!=103 && ch!=71 || flags & 8)
fpprec=prec - 39;
prec=39;}else{
if(prec==-1)
prec=6;}
# 480
cp=_tag_fat(buf,sizeof(char),349U);
({struct _fat_ptr _Tmp0=cp;char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='\000';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
size=({double _Tmp0=_double;int _Tmp1=prec;int _Tmp2=flags;int _Tmp3=ch;struct _fat_ptr _Tmp4=cp;Cyc___cvt_double(_Tmp0,_Tmp1,_Tmp2,& softsign,_Tmp3,_Tmp4,
# 484
_fat_ptr_plus(_tag_fat(buf,sizeof(char),349U),sizeof(char),(int)(sizeof(buf)- 1U)));});
if(softsign)
sign='-';
if((int)*((char*)cp.curr)==0)
_fat_ptr_inplace_plus(& cp,sizeof(char),1);
goto _LL0;case 110:
# 491
 if(flags & 1)
({unsigned long _Tmp0=(unsigned long)ret;*Cyc_va_arg_int_ptr(ap)=_Tmp0;});else{
if(flags & 4)
({short _Tmp0=(short)ret;*Cyc_va_arg_short_ptr(ap)=_Tmp0;});else{
# 496
({unsigned long _Tmp0=(unsigned long)ret;*Cyc_va_arg_int_ptr(ap)=_Tmp0;});}}
_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
continue;case 79:
# 500
 flags |=1;
goto _LL40;case 111: _LL40:
# 503
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
base=0U;
goto nosign;case 112:
# 515 "printf.cyc"
 _ulong=(unsigned long)Cyc_va_arg_long(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
base=2U;
flags |=64;
ch=120;
goto nosign;case 115:  {
# 521
struct _fat_ptr b=Cyc_va_arg_string(ap);_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
which_cp=3;cp3=b;
if(prec >= 0){
struct _fat_ptr p=Cyc_my_nzmemchr(cp3,'\000',prec);
if((char*)p.curr!=(char*)_tag_fat(0,0,0).curr){
size=(p.curr - cp3.curr)/ sizeof(char);
if(size > prec)
size=prec;}else{
# 530
size=prec;}}else{
# 532
size=(int)Cyc_my_strlen(cp3);}
sign='\000';
goto _LL0;}case 85:
# 536
 flags |=1;
goto _LL48;case 117: _LL48:
# 539
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
base=1U;
goto nosign;case 88:
 goto _LL4C;case 120: _LL4C:
 _ulong=flags & 1?Cyc_va_arg_ulong(ap):(flags & 4?(unsigned long)((unsigned short)Cyc_va_arg_int(ap)): Cyc_va_arg_uint(ap));_fat_ptr_inplace_plus(& ap,sizeof(void*),1);
base=2U;
# 546
if(flags & 8 && _ulong!=0U)
flags |=64;
# 550
nosign: sign='\000';
# 556
number: if((dprec=prec)>= 0)
flags &=4294967263U;
# 564
cp=_fat_ptr_plus(_tag_fat(buf,sizeof(char),349U),sizeof(char),348);
if(_ulong!=0U || prec!=0){
struct _fat_ptr xdigs;
# 572
switch((int)base){case Cyc_OCT:
# 574
 do{
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((_ulong & 7U)+ 48U);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
_ulong >>=3U;}while((int)_ulong);
# 579
if(flags & 8 &&(int)*((char*)_check_fat_subscript(cp,sizeof(char),0U))!=48)
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
goto _LL4F;case Cyc_DEC:
# 585
 while(_ulong >= 10U){
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(_ulong % 10U + 48U);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
_ulong /=10U;
# 586
1U;}
# 589
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(_ulong + 48U);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
goto _LL4F;case Cyc_HEX:
# 593
 if(ch==88)
xdigs=_tag_fat("0123456789ABCDEF",sizeof(char),17U);else{
# 596
xdigs=_tag_fat("0123456789abcdef",sizeof(char),17U);}
do{
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus(& cp,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=*((const char*)_check_fat_subscript(xdigs,sizeof(char),(int)(_ulong & 15U)));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
_ulong >>=4U;}while((int)_ulong);
# 601
goto _LL4F;default:
# 603
 cp=Cyc_strdup(_tag_fat("bug in vform: bad base",sizeof(char),23U));
goto skipsize;}_LL4F:;}
# 607
size=({unsigned char*_Tmp0=_fat_ptr_plus(_tag_fat(buf,sizeof(char),349U),sizeof(char),348).curr;_Tmp0 - cp.curr;})/ sizeof(char);
skipsize:
 goto _LL0;default:
# 611
 if(ch==0)
goto done;
# 614
cp=_tag_fat(buf,sizeof(char),349U);
({struct _fat_ptr _Tmp0=cp;char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)ch;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
size=1;
sign='\000';
goto _LL0;}_LL0:;
# 643 "printf.cyc"
fieldsz=size + fpprec;
dpad=dprec - size;
if(dpad < 0)
dpad=0;
# 648
if((int)sign)
++ fieldsz;else{
if(flags & 64)
fieldsz +=2;}
fieldsz +=dpad;
# 655
if((flags & 48U)==0){
if(({int _Tmp0=Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz);_Tmp0 < width - fieldsz;}))goto error;}
# 659
if((int)sign){
({struct _fat_ptr _Tmp0=_fat_ptr_plus(_tag_fat(sign_string,sizeof(char),2U),sizeof(char),0);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=sign;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
do{if(Cyc__IO_sputn(ioputc,ioputc_env,_tag_fat(sign_string,sizeof(char),2U),1)!=1)goto error;}while(0);}else{
if(flags & 64){
ox[0]='0';
ox[1]=(char)ch;
do{if(Cyc__IO_nzsputn(ioputc,ioputc_env,_tag_fat(ox,sizeof(char),2U),2)!=2)goto error;}while(0);}}
# 669
if((flags & 48U)==32){
if(({int _Tmp0=Cyc__IO_padn(ioputc,ioputc_env,'0',width - fieldsz);_Tmp0 < width - fieldsz;}))goto error;}
# 673
if(({int _Tmp0=Cyc__IO_padn(ioputc,ioputc_env,'0',dpad);_Tmp0 < dpad;}))goto error;
# 676
if(which_cp==0)
do{if(({int _Tmp0=Cyc__IO_sputn(ioputc,ioputc_env,(struct _fat_ptr)cp,size);_Tmp0!=size;}))goto error;}while(0);else{
if(which_cp==2)
do{if(({int _Tmp0=Cyc__IO_sputn(ioputc,ioputc_env,(struct _fat_ptr)cp2,size);_Tmp0!=size;}))goto error;}while(0);else{
if(which_cp==3)
do{if(({int _Tmp0=Cyc__IO_nzsputn(ioputc,ioputc_env,(struct _fat_ptr)cp3,size);_Tmp0!=size;}))goto error;}while(0);}}
# 684
if(({int _Tmp0=Cyc__IO_padn(ioputc,ioputc_env,'0',fpprec);_Tmp0 < fpprec;}))goto error;
# 687
if(flags & 16){
if(({int _Tmp0=Cyc__IO_padn(ioputc,ioputc_env,' ',width - fieldsz);_Tmp0 < width - fieldsz;}))goto error;}
# 691
ret +=width > fieldsz?width: fieldsz;}}
# 694
done:
 return ret;
error:
 return -1;}}}}
# 701
static struct _fat_ptr Cyc_exponent(struct _fat_ptr p,int exp,int fmtch){
# 703
struct _fat_ptr t;
char expbuffer[309U];({{unsigned _Tmp0=308U;unsigned i;for(i=0;i < _Tmp0;++ i){expbuffer[i]='0';}expbuffer[_Tmp0]=0;}0;});{
struct _fat_ptr expbuf=_tag_fat(expbuffer,sizeof(char),309U);
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)fmtch;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
if(exp < 0){
exp=- exp;
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='-';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}else{
# 712
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='+';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
t=_fat_ptr_plus(expbuf,sizeof(char),308);
if(exp > 9){
do{
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus(& t,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(exp % 10 + 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}while((exp /=10)> 9);
# 718
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus(& t,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(exp + 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
for(1;({char*_Tmp0=(char*)t.curr;_Tmp0 < (char*)_fat_ptr_plus(expbuf,sizeof(char),308).curr;});({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=*((char*)_fat_ptr_inplace_plus_post(& t,sizeof(char),1).curr);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;})){;}}else{
# 722
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& p,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)(exp + 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 725
return p;}}
# 728
static struct _fat_ptr Cyc_sround(double fract,int*exp,struct _fat_ptr start,struct _fat_ptr end,char ch,int*signp){
# 732
double tmp=0.0;
# 734
if(fract!=0.0)
modf(fract * (double)10,& tmp);else{
# 737
tmp=(double)((int)ch - 48);}
if(tmp > (double)4)
for(1;1;_fat_ptr_inplace_plus(& end,sizeof(char),-1)){
if((int)*((char*)_check_fat_subscript(end,sizeof(char),0U))==46)
_fat_ptr_inplace_plus(& end,sizeof(char),-1);
if((int)({struct _fat_ptr _Tmp0=end;char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=_Tmp1 + 1;if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;})<= 57)
break;
({struct _fat_ptr _Tmp0=end;char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
if((char*)end.curr==(char*)start.curr){
if((unsigned)exp){
({struct _fat_ptr _Tmp0=end;char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='1';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
++(*exp);}else{
# 751
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus(& end,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='1';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
_fat_ptr_inplace_plus(& start,sizeof(char),-1);}
# 754
break;}}else{
# 758
if(*signp==45)
for(1;1;_fat_ptr_inplace_plus(& end,sizeof(char),-1)){
if((int)*((char*)_check_fat_subscript(end,sizeof(char),0U))==46)
_fat_ptr_inplace_plus(& end,sizeof(char),-1);
if((int)*((char*)_check_fat_subscript(end,sizeof(char),0U))!=48)
break;
if((char*)end.curr==(char*)start.curr)
*signp=0;}}
# 767
return start;}
# 770
int Cyc___cvt_double(double number,int prec,int flags,int*signp,int fmtch,struct _fat_ptr startp,struct _fat_ptr endp){
# 773
struct _fat_ptr p;struct _fat_ptr t;
register double fract;
int dotrim=0;int expcnt;int gformat=0;
double integer=0.0;double tmp=0.0;
# 778
expcnt=0;
if(number < (double)0){
number=- number;
*signp=45;}else{
# 783
*signp=0;}
# 785
fract=modf(number,& integer);
# 788
t=_fat_ptr_inplace_plus(& startp,sizeof(char),1);
# 794
for(p=_fat_ptr_plus(endp,sizeof(char),- 1);(char*)p.curr >= (char*)startp.curr && integer!=0.0;++ expcnt){
tmp=modf(integer / (double)10,& integer);
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& p,sizeof(char),-1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)((tmp + .01)* (double)10)+ 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 798
switch((int)fmtch){case 102:
 goto _LL4;case 70: _LL4:
# 801
 if(expcnt)
for(1;({char*_Tmp0=(char*)_fat_ptr_inplace_plus(& p,sizeof(char),1).curr;_Tmp0 < (char*)endp.curr;});({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=*((char*)_check_fat_subscript(p,sizeof(char),0U));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;})){;}else{
# 804
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 809
if(prec || flags & 8)
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='.';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
# 812
if(fract!=0.0){
if(prec)
do{
fract=modf(fract * (double)10,& tmp);
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)tmp + 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}while(
-- prec && fract!=0.0);
if(fract!=0.0)
startp=({double _Tmp0=fract;struct _fat_ptr _Tmp1=startp;struct _fat_ptr _Tmp2=
_fat_ptr_plus(t,sizeof(char),- 1);
# 819
Cyc_sround(_Tmp0,0,_Tmp1,_Tmp2,'\000',signp);});}
# 822
for(1;prec --;({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;})){;}
goto _LL0;case 101:
 goto _LL8;case 69: _LL8:
 eformat: if(expcnt){
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=*((char*)_check_fat_subscript(_fat_ptr_inplace_plus(& p,sizeof(char),1),sizeof(char),0U));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
if(prec || flags & 8)
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='.';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
# 830
for(1;prec &&({char*_Tmp0=(char*)_fat_ptr_inplace_plus(& p,sizeof(char),1).curr;_Tmp0 < (char*)endp.curr;});-- prec){
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=*((char*)_check_fat_subscript(p,sizeof(char),0U));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 837
if(!prec &&({char*_Tmp0=(char*)_fat_ptr_inplace_plus(& p,sizeof(char),1).curr;_Tmp0 < (char*)endp.curr;})){
fract=(double)0;
startp=({struct _fat_ptr _Tmp0=startp;struct _fat_ptr _Tmp1=
_fat_ptr_plus(t,sizeof(char),- 1);
# 839
char _Tmp2=*((char*)_check_fat_subscript(p,sizeof(char),0U));Cyc_sround((double)0,& expcnt,_Tmp0,_Tmp1,_Tmp2,signp);});}
# 843
-- expcnt;}else{
# 846
if(fract!=0.0){
# 848
for(expcnt=-1;1;-- expcnt){
fract=modf(fract * (double)10,& tmp);
if(tmp!=0.0)
break;}
# 853
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)tmp + 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
if(prec || flags & 8)
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='.';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}else{
# 858
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
if(prec || flags & 8)
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='.';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}}
# 863
if(fract!=0.0){
if(prec)
do{
fract=modf(fract * (double)10,& tmp);
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)tmp + 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}while(
-- prec && fract!=0.0);
if(fract!=0.0)
startp=({double _Tmp0=fract;struct _fat_ptr _Tmp1=startp;struct _fat_ptr _Tmp2=
_fat_ptr_plus(t,sizeof(char),- 1);
# 870
Cyc_sround(_Tmp0,& expcnt,_Tmp1,_Tmp2,'\000',signp);});}
# 874
for(1;prec --;({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;})){;}
# 877
if(gformat && !(flags & 8)){
while((char*)t.curr > (char*)startp.curr &&(int)*((char*)_check_fat_subscript(_fat_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0U))==48){1U;}
if((int)*((char*)_check_fat_subscript(t,sizeof(char),0U))==46)
_fat_ptr_inplace_plus(& t,sizeof(char),-1);
_fat_ptr_inplace_plus(& t,sizeof(char),1);}
# 883
t=Cyc_exponent(t,expcnt,fmtch);
goto _LL0;case 103:
 goto _LLC;case 71: _LLC:
# 887
 if(!prec)
++ prec;
# 895
if(expcnt > prec ||(!expcnt && fract!=0.0)&& fract < .0001){
# 903
-- prec;
fmtch -=2;
gformat=1;
goto eformat;}
# 912
if(expcnt)
for(1;({char*_Tmp0=(char*)_fat_ptr_inplace_plus(& p,sizeof(char),1).curr;_Tmp0 < (char*)endp.curr;});(({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=*((char*)_check_fat_subscript(p,sizeof(char),0U));if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;}),-- prec)){;}else{
# 915
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}
# 920
if(prec || flags & 8){
dotrim=1;
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='.';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}else{
# 925
dotrim=0;}
# 927
if(fract!=0.0){
if(prec){
# 931
do{
fract=modf(fract * (double)10,& tmp);
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)tmp + 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});}while(
tmp==0.0 && !expcnt);
while(-- prec && fract!=0.0){
fract=modf(fract * (double)10,& tmp);
({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2=(char)((int)tmp + 48);if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;});
# 936
1U;}}
# 940
if(fract!=0.0)
startp=({double _Tmp0=fract;struct _fat_ptr _Tmp1=startp;struct _fat_ptr _Tmp2=
_fat_ptr_plus(t,sizeof(char),- 1);
# 941
Cyc_sround(_Tmp0,0,_Tmp1,_Tmp2,'\000',signp);});}
# 945
if(flags & 8)
for(1;prec --;({struct _fat_ptr _Tmp0=_fat_ptr_inplace_plus_post(& t,sizeof(char),1);char _Tmp1=*((char*)_check_fat_subscript(_Tmp0,sizeof(char),0U));char _Tmp2='0';if(_get_fat_size(_Tmp0,sizeof(char))==1U &&(_Tmp1==0 && _Tmp2!=0))_throw_arraybounds();*((char*)_Tmp0.curr)=_Tmp2;})){;}else{
if(dotrim){
while((char*)t.curr > (char*)startp.curr &&(int)*((char*)_check_fat_subscript(_fat_ptr_inplace_plus(& t,sizeof(char),-1),sizeof(char),0U))==48){1U;}
if((int)*((char*)_check_fat_subscript(t,sizeof(char),0U))!=46)
_fat_ptr_inplace_plus(& t,sizeof(char),1);}}
# 952
goto _LL0;default:
 _throw((void*)({struct Cyc_Core_Impossible_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_Tmp0->tag=Cyc_Core_Impossible,_Tmp0->f1=_tag_fat("__cvt_double",sizeof(char),13U);_Tmp0;}));}_LL0:;
# 955
return(t.curr - startp.curr)/ sizeof(char);}
# 959
int Cyc_vfprintf(struct Cyc___cycFILE*f,struct _fat_ptr fmt,struct _fat_ptr ap){
# 962
int ans;
ans=({int(*_Tmp0)(int(*)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr)=(int(*)(int(*)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr))Cyc__IO_vfprintf;_Tmp0;})(Cyc_putc,f,fmt,ap);
return ans;}
# 967
int Cyc_fprintf(struct Cyc___cycFILE*f,struct _fat_ptr fmt,struct _fat_ptr ap){
# 971
return Cyc_vfprintf(f,fmt,ap);}
# 974
int Cyc_vprintf(struct _fat_ptr fmt,struct _fat_ptr ap){
# 977
int ans;
ans=({int(*_Tmp0)(int(*)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr)=(int(*)(int(*)(int,struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr))Cyc__IO_vfprintf;_Tmp0;})(Cyc_putc,Cyc_stdout,fmt,ap);
return ans;}
# 982
int Cyc_printf(struct _fat_ptr fmt,struct _fat_ptr ap){
# 985
int ans;
ans=Cyc_vprintf(fmt,ap);
return ans;}struct _tuple0{struct _fat_ptr*f0;unsigned long*f1;};
# 990
static int Cyc_putc_string(int c,struct _tuple0*sptr_n){
void*_Tmp0;void*_Tmp1;_Tmp1=sptr_n->f0;_Tmp0=sptr_n->f1;{struct _fat_ptr*sptr=_Tmp1;unsigned long*nptr=_Tmp0;
struct _fat_ptr s=*sptr;
unsigned long n=*nptr;
if(n==0U)return -1;
*((char*)_check_fat_subscript(s,sizeof(char),0U))=(char)c;
_fat_ptr_inplace_plus(sptr,sizeof(char),1);
*nptr=n - 1U;
return 1;}}
# 1001
int Cyc_vsnprintf(struct _fat_ptr s,unsigned long n,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1004
int ans;
struct _fat_ptr sptr=s;
unsigned long nptr=n;
struct _tuple0 sptr_n=({struct _tuple0 _Tmp0;_Tmp0.f0=& sptr,_Tmp0.f1=& nptr;_Tmp0;});
ans=({int(*_Tmp0)(int(*)(int,struct _tuple0*),struct _tuple0*,struct _fat_ptr,struct _fat_ptr)=(int(*)(int(*)(int,struct _tuple0*),struct _tuple0*,struct _fat_ptr,struct _fat_ptr))Cyc__IO_vfprintf;_Tmp0;})(Cyc_putc_string,& sptr_n,fmt,ap);
if(0 <= ans)
*((char*)_check_fat_subscript(s,sizeof(char),ans))='\000';
return ans;}
# 1014
int Cyc_snprintf(struct _fat_ptr s,unsigned long n,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1017
return Cyc_vsnprintf(s,n,fmt,ap);}
# 1020
int Cyc_vsprintf(struct _fat_ptr s,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1023
return Cyc_vsnprintf(s,_get_fat_size(s,sizeof(char)),fmt,ap);}
# 1026
int Cyc_sprintf(struct _fat_ptr s,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1029
return Cyc_vsnprintf(s,_get_fat_size(s,sizeof(char)),fmt,ap);}
# 1032
static int Cyc_putc_void(int c,int dummy){
return 1;}
# 1036
struct _fat_ptr Cyc_vrprintf(struct _RegionHandle*r1,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1040
int size=({int(*_Tmp0)(int(*)(int,int),int,struct _fat_ptr,struct _fat_ptr)=(int(*)(int(*)(int,int),int,struct _fat_ptr,struct _fat_ptr))Cyc__IO_vfprintf;_Tmp0;})(Cyc_putc_void,0,fmt,ap)+ 1;
struct _fat_ptr s=({unsigned _Tmp0=size + 1;_tag_fat(_region_calloc(r1,0U,sizeof(char),_Tmp0),sizeof(char),_Tmp0);});
({struct _fat_ptr _Tmp0=_fat_ptr_decrease_size(s,sizeof(char),1U);struct _fat_ptr _Tmp1=fmt;Cyc_vsprintf(_Tmp0,_Tmp1,ap);});
return s;}
# 1046
struct _fat_ptr Cyc_rprintf(struct _RegionHandle*r1,struct _fat_ptr fmt,struct _fat_ptr ap){
# 1049
return Cyc_vrprintf(r1,fmt,ap);}
# 1052
struct _fat_ptr Cyc_aprintf(struct _fat_ptr fmt,struct _fat_ptr ap){
# 1055
return Cyc_vrprintf(Cyc_Core_heap_region,fmt,ap);}
