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
 extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc___cycFILE;
# 52 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stdin;
# 90
extern int Cyc_fgetc(struct Cyc___cycFILE*);struct Cyc_ShortPtr_sa_ScanfArg_struct{int tag;short*f1;};struct Cyc_UShortPtr_sa_ScanfArg_struct{int tag;unsigned short*f1;};struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};struct Cyc_UIntPtr_sa_ScanfArg_struct{int tag;unsigned*f1;};struct Cyc_StringPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_DoublePtr_sa_ScanfArg_struct{int tag;double*f1;};struct Cyc_FloatPtr_sa_ScanfArg_struct{int tag;float*f1;};struct Cyc_CharPtr_sa_ScanfArg_struct{int tag;struct _fat_ptr f1;};
# 142 "cycboot.h"
extern int Cyc_getc(struct Cyc___cycFILE*);
# 222 "cycboot.h"
extern int Cyc_ungetc(int,struct Cyc___cycFILE*);
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
static short*Cyc_va_arg_short_ptr(void*a){void*_T0;int*_T1;unsigned _T2;void*_T3;short*_T4;void*_T5;unsigned short*_T6;short*_T7;struct Cyc_Core_Invalid_argument_exn_struct*_T8;void*_T9;unsigned short*_TA;short*_TB;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=a;{struct Cyc_ShortPtr_sa_ScanfArg_struct*_TC=(struct Cyc_ShortPtr_sa_ScanfArg_struct*)_T3;_TB=_TC->f1;}{short*p=_TB;_T4=p;
# 141
return _T4;}case 1: _T5=a;{struct Cyc_UShortPtr_sa_ScanfArg_struct*_TC=(struct Cyc_UShortPtr_sa_ScanfArg_struct*)_T5;_TA=_TC->f1;}{unsigned short*p=_TA;_T6=p;_T7=(short*)_T6;
return _T7;}default:{struct Cyc_Core_Invalid_argument_exn_struct*_TC=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_TC->tag=Cyc_Core_Invalid_argument;
_TC->f1=_tag_fat("scan expects short pointer",sizeof(char),27U);_T8=(struct Cyc_Core_Invalid_argument_exn_struct*)_TC;}_T9=(void*)_T8;_throw(_T9);};}
# 147
static int*Cyc_va_arg_int_ptr(void*a){void*_T0;int*_T1;unsigned _T2;void*_T3;int*_T4;void*_T5;unsigned*_T6;int*_T7;struct Cyc_Core_Invalid_argument_exn_struct*_T8;void*_T9;unsigned*_TA;int*_TB;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 2: _T3=a;{struct Cyc_IntPtr_sa_ScanfArg_struct*_TC=(struct Cyc_IntPtr_sa_ScanfArg_struct*)_T3;_TB=_TC->f1;}{int*p=_TB;_T4=p;
# 149
return _T4;}case 3: _T5=a;{struct Cyc_UIntPtr_sa_ScanfArg_struct*_TC=(struct Cyc_UIntPtr_sa_ScanfArg_struct*)_T5;_TA=_TC->f1;}{unsigned*p=_TA;_T6=p;_T7=(int*)_T6;
return _T7;}default:{struct Cyc_Core_Invalid_argument_exn_struct*_TC=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_TC->tag=Cyc_Core_Invalid_argument;
_TC->f1=_tag_fat("scan expects int pointer",sizeof(char),25U);_T8=(struct Cyc_Core_Invalid_argument_exn_struct*)_TC;}_T9=(void*)_T8;_throw(_T9);};}
# 155
static struct _fat_ptr Cyc_va_arg_string_ptr(void*a){void*_T0;int*_T1;unsigned _T2;void*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;void*_T6;struct _fat_ptr _T7;struct Cyc_Core_Invalid_argument_exn_struct*_T8;void*_T9;struct _fat_ptr _TA;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 4: _T3=a;{struct Cyc_StringPtr_sa_ScanfArg_struct*_TB=(struct Cyc_StringPtr_sa_ScanfArg_struct*)_T3;_TA=_TB->f1;}{struct _fat_ptr p=_TA;_T4=p;_T5=
# 157
_fat_ptr_decrease_size(_T4,sizeof(char),1U);return _T5;}case 7: _T6=a;{struct Cyc_CharPtr_sa_ScanfArg_struct*_TB=(struct Cyc_CharPtr_sa_ScanfArg_struct*)_T6;_TA=_TB->f1;}{struct _fat_ptr p=_TA;_T7=p;
return _T7;}default:{struct Cyc_Core_Invalid_argument_exn_struct*_TB=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_TB->tag=Cyc_Core_Invalid_argument;
_TB->f1=_tag_fat("scan expects char pointer",sizeof(char),26U);_T8=(struct Cyc_Core_Invalid_argument_exn_struct*)_TB;}_T9=(void*)_T8;_throw(_T9);};}
# 163
static double*Cyc_va_arg_double_ptr(void*a){void*_T0;int*_T1;int _T2;void*_T3;double*_T4;struct Cyc_Core_Invalid_argument_exn_struct*_T5;void*_T6;double*_T7;_T0=a;_T1=(int*)_T0;_T2=*_T1;if(_T2!=5)goto _TL3;_T3=a;{struct Cyc_DoublePtr_sa_ScanfArg_struct*_T8=(struct Cyc_DoublePtr_sa_ScanfArg_struct*)_T3;_T7=_T8->f1;}{double*p=_T7;_T4=p;
# 165
return _T4;}_TL3:{struct Cyc_Core_Invalid_argument_exn_struct*_T8=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_T8->tag=Cyc_Core_Invalid_argument;
_T8->f1=_tag_fat("scan expects double pointer",sizeof(char),28U);_T5=(struct Cyc_Core_Invalid_argument_exn_struct*)_T8;}_T6=(void*)_T5;_throw(_T6);;}
# 170
static float*Cyc_va_arg_float_ptr(void*a){void*_T0;int*_T1;int _T2;void*_T3;float*_T4;struct Cyc_Core_Invalid_argument_exn_struct*_T5;void*_T6;float*_T7;_T0=a;_T1=(int*)_T0;_T2=*_T1;if(_T2!=6)goto _TL5;_T3=a;{struct Cyc_FloatPtr_sa_ScanfArg_struct*_T8=(struct Cyc_FloatPtr_sa_ScanfArg_struct*)_T3;_T7=_T8->f1;}{float*p=_T7;_T4=p;
# 172
return _T4;}_TL5:{struct Cyc_Core_Invalid_argument_exn_struct*_T8=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_T8->tag=Cyc_Core_Invalid_argument;
_T8->f1=_tag_fat("scan expects float pointer",sizeof(char),27U);_T5=(struct Cyc_Core_Invalid_argument_exn_struct*)_T8;}_T6=(void*)_T5;_throw(_T6);;}
# 177
static struct _fat_ptr Cyc_va_arg_char_ptr(void*a){void*_T0;int*_T1;unsigned _T2;void*_T3;struct _fat_ptr _T4;void*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;struct Cyc_Core_Invalid_argument_exn_struct*_T8;void*_T9;struct _fat_ptr _TA;_T0=a;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 7: _T3=a;{struct Cyc_CharPtr_sa_ScanfArg_struct*_TB=(struct Cyc_CharPtr_sa_ScanfArg_struct*)_T3;_TA=_TB->f1;}{struct _fat_ptr p=_TA;_T4=p;
# 179
return _T4;}case 4: _T5=a;{struct Cyc_StringPtr_sa_ScanfArg_struct*_TB=(struct Cyc_StringPtr_sa_ScanfArg_struct*)_T5;_TA=_TB->f1;}{struct _fat_ptr p=_TA;_T6=p;_T7=
_fat_ptr_decrease_size(_T6,sizeof(char),1U);return _T7;}default:{struct Cyc_Core_Invalid_argument_exn_struct*_TB=_cycalloc(sizeof(struct Cyc_Core_Invalid_argument_exn_struct));_TB->tag=Cyc_Core_Invalid_argument;
_TB->f1=_tag_fat("scan expects char pointer",sizeof(char),26U);_T8=(struct Cyc_Core_Invalid_argument_exn_struct*)_TB;}_T9=(void*)_T8;_throw(_T9);};}
# 188
int Cyc__IO_vfscanf(int(*_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*_IO_peekc)(void*),void*fp,struct _fat_ptr fmt0,struct _fat_ptr ap,int*errp){char*_T0;unsigned _T1;char*_T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;char*_T5;const char*_T6;char _T7;int _T8;int _T9;struct _fat_ptr*_TA;struct _fat_ptr _TB;char*_TC;const char*_TD;char _TE;int _TF;int _T10;int _T11;long long _T12;int _T13;long long _T14;long long _T15;struct _fat_ptr _T16;char*_T17;void**_T18;void*_T19;short*_T1A;int _T1B;struct _fat_ptr _T1C;char*_T1D;void**_T1E;void*_T1F;int*_T20;struct _fat_ptr _T21;char*_T22;void**_T23;void*_T24;int*_T25;struct _fat_ptr*_T26;int _T27;int _T28;int _T29;int _T2A;int _T2B;int _T2C;long long _T2D;struct _fat_ptr _T2E;char*_T2F;void**_T30;void*_T31;struct _fat_ptr*_T32;struct _fat_ptr _T33;char*_T34;char*_T35;int _T36;struct _fat_ptr*_T37;long long _T38;char*_T39;int _T3A;char _T3B;int _T3C;char*_T3D;char*_T3E;char _T3F;int _T40;long long _T41;int _T42;struct _fat_ptr _T43;char*_T44;void**_T45;void*_T46;struct _fat_ptr _T47;struct _fat_ptr*_T48;char*_T49;int _T4A;char _T4B;int _T4C;char*_T4D;char*_T4E;char _T4F;int _T50;struct _fat_ptr _T51;unsigned _T52;struct _fat_ptr _T53;unsigned char*_T54;char*_T55;int _T56;struct _fat_ptr*_T57;long long _T58;int _T59;struct _fat_ptr _T5A;unsigned char*_T5B;char*_T5C;struct _fat_ptr _T5D;unsigned char*_T5E;char*_T5F;struct _fat_ptr _T60;unsigned char*_T61;struct _fat_ptr _T62;unsigned char*_T63;int _T64;struct _fat_ptr _T65;unsigned _T66;struct _fat_ptr _T67;unsigned char*_T68;char*_T69;int _T6A;int _T6B;unsigned char _T6C;int _T6D;int _T6E;long long _T6F;int _T70;int _T71;struct _fat_ptr _T72;char*_T73;void**_T74;void*_T75;struct _fat_ptr*_T76;int _T77;unsigned char _T78;int _T79;int _T7A;struct _fat_ptr _T7B;unsigned _T7C;struct _fat_ptr _T7D;unsigned char*_T7E;char*_T7F;int _T80;struct _fat_ptr*_T81;long long _T82;int _T83;struct _fat_ptr _T84;unsigned _T85;struct _fat_ptr _T86;unsigned char*_T87;char*_T88;struct _fat_ptr _T89;unsigned char*_T8A;struct _fat_ptr _T8B;unsigned char*_T8C;int _T8D;int _T8E;long long _T8F;unsigned _T90;unsigned _T91;long long _T92;unsigned _T93;unsigned _T94;char*_T95;int _T96;unsigned char _T97;int _T98;int _T99;short*_T9A;int _T9B;char*_T9C;short*_T9D;short _T9E;short*_T9F;int _TA0;char*_TA1;short*_TA2;short _TA3;struct _fat_ptr _TA4;int _TA5;struct _fat_ptr _TA6;unsigned char*_TA7;char*_TA8;char*_TA9;struct _fat_ptr*_TAA;char*_TAB;char*_TAC;int _TAD;unsigned _TAE;unsigned char*_TAF;char*_TB0;int _TB1;struct _fat_ptr _TB2;unsigned char*_TB3;char*_TB4;char*_TB5;struct _fat_ptr*_TB6;struct _fat_ptr _TB7;char*_TB8;char*_TB9;char _TBA;int _TBB;void*_TBC;struct _fat_ptr _TBD;char*_TBE;char*_TBF;char _TC0;struct _fat_ptr*_TC1;int _TC2;unsigned char*_TC3;char*_TC4;unsigned _TC5;unsigned char*_TC6;char*_TC7;int _TC8;long _TC9;struct _fat_ptr _TCA;unsigned char*_TCB;void**_TCC;void*_TCD;int*_TCE;unsigned long _TCF;struct _fat_ptr _TD0;unsigned char*_TD1;void**_TD2;void*_TD3;short*_TD4;unsigned long _TD5;struct _fat_ptr _TD6;unsigned char*_TD7;void**_TD8;void*_TD9;int*_TDA;unsigned long _TDB;struct _fat_ptr _TDC;unsigned char*_TDD;void**_TDE;void*_TDF;int*_TE0;unsigned long _TE1;struct _fat_ptr*_TE2;struct _fat_ptr _TE3;unsigned char*_TE4;char*_TE5;struct _fat_ptr _TE6;unsigned char*_TE7;int _TE8;int _TE9;long long _TEA;unsigned _TEB;unsigned _TEC;long long _TED;unsigned _TEE;unsigned _TEF;char*_TF0;int _TF1;int _TF2;int _TF3;int _TF4;int _TF5;struct _fat_ptr*_TF6;char*_TF7;char*_TF8;int _TF9;unsigned _TFA;unsigned char*_TFB;char*_TFC;int _TFD;struct _fat_ptr _TFE;unsigned char*_TFF;char*_T100;char*_T101;struct _fat_ptr*_T102;struct _fat_ptr _T103;char*_T104;char*_T105;char _T106;int _T107;void*_T108;struct _fat_ptr*_T109;struct _fat_ptr _T10A;char*_T10B;char*_T10C;char _T10D;struct _fat_ptr*_T10E;struct _fat_ptr _T10F;char*_T110;char*_T111;char _T112;int _T113;char*_T114;char*_T115;unsigned _T116;unsigned char*_T117;char*_T118;char*_T119;const char*_T11A;struct _fat_ptr _T11B;char*_T11C;void**_T11D;void*_T11E;double*_T11F;struct _fat_ptr _T120;char*_T121;void**_T122;void*_T123;float*_T124;double _T125;struct _fat_ptr*_T126;struct _fat_ptr _T127;unsigned char*_T128;char*_T129;struct _fat_ptr _T12A;unsigned char*_T12B;int _T12C;int _T12D;struct Cyc_Core_Impossible_exn_struct*_T12E;void*_T12F;int*_T130;unsigned _T131;int*_T132;int*_T133;unsigned _T134;int _T135;int*_T136;int _T137;
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
char buf[351U];{unsigned _T138=350U;unsigned i;i=0;_TLB: if(i < _T138)goto _TL9;else{goto _TLA;}_TL9: _T0=buf;_T1=i;_T0[_T1]='0';i=i + 1;goto _TLB;_TLA: _T2=buf;_T2[_T138]=0;}{
# 217
int seen_eof=0;
# 220
static short basefix[17U]={10,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
# 223
nassigned=0;
nread=0;_TLF:
 if(1)goto _TLD;else{goto _TLE;}
_TLD: _T3=& fmt;_T4=_fat_ptr_inplace_plus_post(_T3,sizeof(char),1);_T5=_check_fat_subscript(_T4,sizeof(char),0U);_T6=(const char*)_T5;_T7=*_T6;c=(int)_T7;
if(c!=0)goto _TL10;goto done;_TL10: _T8=
# 229
isspace(c);if(!_T8)goto _TL12;_TL17:
 if(1)goto _TL15;else{goto _TL16;}
_TL15: c=_IO_getc(fp);
if(c!=-1)goto _TL18;
seen_eof=seen_eof + 1;goto _TL16;_TL18: _T9=
# 236
isspace(c);if(_T9)goto _TL1A;else{goto _TL1C;}
_TL1C: _IO_ungetc(c,fp);goto _TL16;_TL1A:
# 240
 nread=nread + 1;goto _TL17;_TL16: goto _TLC;_TL12:
# 244
 if(c==37)goto _TL1D;goto literal;_TL1D:
# 246
 width=0;
flags=0;
# 252
again: _TA=& fmt;_TB=_fat_ptr_inplace_plus_post(_TA,sizeof(char),1);_TC=_check_fat_subscript(_TB,sizeof(char),0U);_TD=(const char*)_TC;_TE=*_TD;c=(int)_TE;_TF=c;_T10=(int)_TF;switch(_T10){case 37:
# 255
 literal:
 n=_IO_getc(fp);
if(n!=-1)goto _TL20;goto eof_failure;_TL20:
# 259
 if(n==c)goto _TL22;
_IO_ungetc(n,fp);goto match_failure;_TL22:
# 263
 nread=nread + 1;goto _TLC;case 42: _T11=flags;
# 267
if(!_T11)goto _TL24;goto control_failure;_TL24:
 flags=8;goto again;case 108:
# 271
 if(!(flags & 4294967223U))goto _TL26;goto control_failure;_TL26:
 flags=flags | 1;goto again;case 76:
# 275
 if(!(flags & 4294967223U))goto _TL28;goto control_failure;_TL28:
 flags=flags | 2;goto again;case 104:
# 279
 if(!(flags & 4294967223U))goto _TL2A;goto control_failure;_TL2A:
 flags=flags | 4;goto again;case 48: goto _LLE;case 49: _LLE: goto _LL10;case 50: _LL10: goto _LL12;case 51: _LL12: goto _LL14;case 52: _LL14: goto _LL16;case 53: _LL16: goto _LL18;case 54: _LL18: goto _LL1A;case 55: _LL1A: goto _LL1C;case 56: _LL1C: goto _LL1E;case 57: _LL1E:
# 285
 if(!(flags & 4294967223U))goto _TL2C;goto control_failure;_TL2C:
 flags=flags | 64;_T12=width * 10;_T13=c;_T14=(long long)_T13;_T15=_T12 + _T14;
width=_T15 - 48;goto again;case 68:
# 298 "scanf.cyc"
 flags=flags | 1;goto _LL22;case 100: _LL22:
# 301
 c=3;
use_strtoul=0;
base=10;goto _LL0;case 105:
# 307
 c=3;
use_strtoul=0;
base=0;goto _LL0;case 79:
# 313
 flags=flags | 1;goto _LL28;case 111: _LL28:
# 316
 c=3;
use_strtoul=1;
base=8;goto _LL0;case 117:
# 322
 c=3;
use_strtoul=1;
base=10;goto _LL0;case 88: goto _LL2E;case 120: _LL2E:
# 328
 flags=flags | 256;
c=3;
use_strtoul=1;
base=16;goto _LL0;case 69: goto _LL32;case 70: _LL32: goto _LL34;case 101: _LL34: goto _LL36;case 102: _LL36: goto _LL38;case 103: _LL38:
# 335
 c=4;goto _LL0;case 115:
# 339
 c=2;goto _LL0;case 91:
# 343
 fmt=Cyc___sccl(ccltab,fmt);
flags=flags | 32;
c=1;goto _LL0;case 99:
# 349
 flags=flags | 32;
c=0;goto _LL0;case 112:
# 354
 flags=flags | 272U;
c=3;
use_strtoul=1;
base=16;goto _LL0;case 110:
# 361
 if(!(flags & 8))goto _TL2E;goto _TLC;_TL2E:
# 363
 if(!(flags & 4))goto _TL30;_T16=ap;_T17=_check_fat_subscript(_T16,sizeof(void*),0U);_T18=(void**)_T17;_T19=*_T18;_T1A=
Cyc_va_arg_short_ptr(_T19);_T1B=nread;*_T1A=(short)_T1B;goto _TL31;
_TL30: if(!(flags & 1))goto _TL32;_T1C=ap;_T1D=_check_fat_subscript(_T1C,sizeof(void*),0U);_T1E=(void**)_T1D;_T1F=*_T1E;_T20=
Cyc_va_arg_int_ptr(_T1F);*_T20=nread;goto _TL33;
# 368
_TL32: _T21=ap;_T22=_check_fat_subscript(_T21,sizeof(void*),0U);_T23=(void**)_T22;_T24=*_T23;_T25=Cyc_va_arg_int_ptr(_T24);*_T25=nread;_TL33: _TL31: _T26=& ap;
_fat_ptr_inplace_plus(_T26,sizeof(void*),1);goto _TLC;case 0:
# 376
 nassigned=-1;goto done;default: _T27=
# 380
isupper(c);if(!_T27)goto _TL34;
flags=flags | 1;goto _TL35;_TL34: _TL35:
 c=3;
use_strtoul=0;
base=10;goto _LL0;}_LL0: _T28=
# 391
_IO_peekc(fp);if(_T28!=-1)goto _TL36;goto eof_failure;_TL36: _T29=flags & 32;
# 398
if(_T29!=0)goto _TL38;
n=_IO_peekc(fp);
_TL3A: _T2A=isspace(n);if(_T2A)goto _TL3B;else{goto _TL3C;}
_TL3B: n=_IO_getc(fp);
nread=nread + 1;
n=_IO_peekc(fp);
if(n!=-1)goto _TL3D;goto eof_failure;_TL3D: goto _TL3A;_TL3C: goto _TL39;_TL38: _TL39: _T2B=c;_T2C=(int)_T2B;switch(_T2C){case 0:
# 421
 if(width!=0)goto _TL40;
width=1;goto _TL41;_TL40: _TL41:
 if(!(flags & 8))goto _TL42;{
long long sum=0;
_TL47: if(width > 0)goto _TL45;else{goto _TL46;}
_TL45: n=_IO_getc(fp);
if(n!=-1)goto _TL48;if(width==0)goto _TL48;goto eof_failure;
# 429
_TL48: if(n!=-1)goto _TL4A;
seen_eof=seen_eof + 1;goto _TL46;_TL4A:
# 433
 sum=sum + 1;
width=width + -1;goto _TL47;_TL46: _T2D=sum;
# 436
nread=nread + _T2D;}goto _TL43;
# 438
_TL42:{long long sum=0;_T2E=ap;_T2F=_check_fat_subscript(_T2E,sizeof(void*),0U);_T30=(void**)_T2F;_T31=*_T30;{
struct _fat_ptr z=Cyc_va_arg_char_ptr(_T31);_T32=& ap;_fat_ptr_inplace_plus(_T32,sizeof(void*),1);
_TL4F: if(width > 0)goto _TL4D;else{goto _TL4E;}
_TL4D: n=_IO_getc(fp);
if(n!=-1)goto _TL50;if(width==0)goto _TL50;goto eof_failure;
# 444
_TL50: if(n!=-1)goto _TL52;
seen_eof=seen_eof + 1;goto _TL4E;_TL52: _T33=z;_T34=_check_fat_subscript(_T33,sizeof(char),0U);_T35=(char*)_T34;_T36=n;
# 448
*_T35=(char)_T36;_T37=& z;
_fat_ptr_inplace_plus(_T37,sizeof(char),1);
sum=sum + 1;
width=width + -1;goto _TL4F;_TL4E: _T38=sum;
# 453
nread=nread + _T38;
nassigned=nassigned + 1;}}_TL43: goto _LL47;case 1:
# 460
 if(width!=0)goto _TL54;
width=-1;goto _TL55;_TL54: _TL55:
# 463
 if(!(flags & 8))goto _TL56;
n=0;{
int c=_IO_peekc(fp);
_TL58: _T39=ccltab;_T3A=c;_T3B=(char)_T3A;_T3C=(int)_T3B;_T3D=_check_known_subscript_notnull(_T39,256U,sizeof(char),_T3C);_T3E=(char*)_T3D;_T3F=*_T3E;_T40=(int)_T3F;if(_T40)goto _TL59;else{goto _TL5A;}
_TL59: n=n + 1;
_IO_getc(fp);
width=width + -1;_T41=width;if(_T41!=0)goto _TL5B;goto _TL5A;_TL5B:
# 471
 c=_IO_peekc(fp);_T42=c;if(_T42!=-1)goto _TL5D;
if(n!=0)goto _TL5F;goto eof_failure;_TL5F:
# 474
 seen_eof=seen_eof + 1;goto _TL5A;_TL5D: goto _TL58;_TL5A:
# 478
 if(n!=0)goto _TL61;goto match_failure;_TL61:;}goto _TL57;
# 481
_TL56: _T43=ap;_T44=_check_fat_subscript(_T43,sizeof(void*),0U);_T45=(void**)_T44;_T46=*_T45;_T47=Cyc_va_arg_string_ptr(_T46);{struct _fat_ptr p4=_T47;_T48=& ap;_fat_ptr_inplace_plus(_T48,sizeof(void*),1);{
struct _fat_ptr p5=p4;
int c=_IO_peekc(fp);
_TL63: _T49=ccltab;_T4A=c;_T4B=(char)_T4A;_T4C=(int)_T4B;_T4D=_check_known_subscript_notnull(_T49,256U,sizeof(char),_T4C);_T4E=(char*)_T4D;_T4F=*_T4E;_T50=(int)_T4F;if(_T50)goto _TL64;else{goto _TL65;}
_TL64: _T51=p5;_T52=_get_fat_size(_T51,sizeof(char));if(_T52!=0U)goto _TL66;goto eof_failure;_TL66: _T53=p5;_T54=_T53.curr;_T55=(char*)_T54;_T56=c;
*_T55=(char)_T56;_T57=& p5;
_fat_ptr_inplace_plus(_T57,sizeof(char),1);
_IO_getc(fp);
width=width + -1;_T58=width;if(_T58!=0)goto _TL68;goto _TL65;_TL68:
# 491
 c=_IO_peekc(fp);_T59=c;if(_T59!=-1)goto _TL6A;_T5A=p5;_T5B=_T5A.curr;_T5C=(char*)_T5B;_T5D=p0;_T5E=_T5D.curr;_T5F=(char*)_T5E;
if(_T5C!=_T5F)goto _TL6C;goto eof_failure;_TL6C:
# 494
 seen_eof=seen_eof + 1;goto _TL65;_TL6A: goto _TL63;_TL65: _T60=p5;_T61=_T60.curr;_T62=p4;_T63=_T62.curr;_T64=_T61 - _T63;
# 498
n=_T64 / sizeof(char);
if(n!=0)goto _TL6E;goto match_failure;_TL6E: _T65=p5;_T66=
# 501
_get_fat_size(_T65,sizeof(char));if(_T66!=0U)goto _TL70;goto eof_failure;_TL70: _T67=p5;_T68=_T67.curr;_T69=(char*)_T68;
*_T69='\000';
nassigned=nassigned + 1;}}_TL57: _T6A=n;
# 505
nread=nread + _T6A;goto _LL47;case 2:
# 510
 if(width!=0)goto _TL72;
width=-1;goto _TL73;_TL72: _TL73:
 if(!(flags & 8))goto _TL74;
n=0;{
int c=_IO_peekc(fp);
_TL76: _T6B=c;_T6C=(unsigned char)_T6B;_T6D=(int)_T6C;_T6E=isspace(_T6D);if(_T6E)goto _TL78;else{goto _TL77;}
_TL77: n=n + 1;
_IO_getc(fp);
width=width + -1;_T6F=width;if(_T6F!=0)goto _TL79;goto _TL78;_TL79:
# 520
 c=_IO_peekc(fp);_T70=c;if(_T70!=-1)goto _TL7B;
seen_eof=seen_eof + 1;goto _TL78;_TL7B: goto _TL76;_TL78: _T71=n;
# 525
nread=nread + _T71;}goto _TL75;
# 527
_TL74: _T72=ap;_T73=_check_fat_subscript(_T72,sizeof(void*),0U);_T74=(void**)_T73;_T75=*_T74;{struct _fat_ptr p2=Cyc_va_arg_string_ptr(_T75);_T76=& ap;_fat_ptr_inplace_plus(_T76,sizeof(void*),1);{
struct _fat_ptr p3=p2;
int c=_IO_peekc(fp);
_TL7D: _T77=c;_T78=(unsigned char)_T77;_T79=(int)_T78;_T7A=isspace(_T79);if(_T7A)goto _TL7F;else{goto _TL7E;}
_TL7E: c=_IO_getc(fp);_T7B=p3;_T7C=
_get_fat_size(_T7B,sizeof(char));if(_T7C!=0U)goto _TL80;goto eof_failure;_TL80: _T7D=p3;_T7E=_T7D.curr;_T7F=(char*)_T7E;_T80=c;
*_T7F=(char)_T80;_T81=& p3;
_fat_ptr_inplace_plus(_T81,sizeof(char),1);
width=width + -1;_T82=width;if(_T82!=0)goto _TL82;goto _TL7F;_TL82:
# 537
 c=_IO_peekc(fp);_T83=c;if(_T83!=-1)goto _TL84;
seen_eof=seen_eof + 1;goto _TL7F;_TL84: goto _TL7D;_TL7F: _T84=p3;_T85=
# 542
_get_fat_size(_T84,sizeof(char));if(_T85!=0U)goto _TL86;goto eof_failure;_TL86: _T86=p3;_T87=_T86.curr;_T88=(char*)_T87;
*_T88='\000';_T89=p3;_T8A=_T89.curr;_T8B=p2;_T8C=_T8B.curr;_T8D=_T8A - _T8C;_T8E=_T8D / sizeof(char);
nread=nread + _T8E;
nassigned=nassigned + 1;}}_TL75: goto _TLC;case 3:
# 551
 if(width==0)goto _TL8A;else{goto _TL8B;}_TL8B: _T8F=width;_T90=sizeof(buf);_T91=_T90 - 1U;_T92=(long long)_T91;if(_T8F > _T92)goto _TL8A;else{goto _TL88;}
_TL8A: _T93=sizeof(buf);_T94=_T93 - 1U;width=(long long)_T94;goto _TL89;_TL88: _TL89:
 flags=flags | 704U;_T95=buf;
p=_tag_fat(_T95,sizeof(char),351U);_TL8F: if(width!=0)goto _TL8D;else{goto _TL8E;}
_TL8D: _T96=_IO_peekc(fp);_T97=(unsigned char)_T96;c=(int)_T97;_T98=c;_T99=(int)_T98;switch(_T99){case 48:
# 575 "scanf.cyc"
 if(base!=0)goto _TL91;
base=8;
flags=flags | 256;goto _TL92;_TL91: _TL92:
# 579
 if(!(flags & 512))goto _TL93;
flags=flags & 4294966591U;goto _TL94;
# 582
_TL93: flags=flags & 4294966847U;_TL94: goto ok;case 49: goto _LL5A;case 50: _LL5A: goto _LL5C;case 51: _LL5C: goto _LL5E;case 52: _LL5E: goto _LL60;case 53: _LL60: goto _LL62;case 54: _LL62: goto _LL64;case 55: _LL64: _T9A=basefix;_T9B=base;_T9C=_check_known_subscript_notnull(_T9A,17U,sizeof(short),_T9B);_T9D=(short*)_T9C;_T9E=*_T9D;
# 588
base=(int)_T9E;
flags=flags & 4294966847U;goto ok;case 56: goto _LL68;case 57: _LL68: _T9F=basefix;_TA0=base;_TA1=_check_known_subscript_notnull(_T9F,17U,sizeof(short),_TA0);_TA2=(short*)_TA1;_TA3=*_TA2;
# 594
base=(int)_TA3;
if(base > 8)goto _TL95;goto _LL54;_TL95:
# 597
 flags=flags & 4294966847U;goto ok;case 65: goto _LL6C;case 66: _LL6C: goto _LL6E;case 67: _LL6E: goto _LL70;case 68: _LL70: goto _LL72;case 69: _LL72: goto _LL74;case 70: _LL74: goto _LL76;case 97: _LL76: goto _LL78;case 98: _LL78: goto _LL7A;case 99: _LL7A: goto _LL7C;case 100: _LL7C: goto _LL7E;case 101: _LL7E: goto _LL80;case 102: _LL80:
# 605
 if(base > 10)goto _TL97;goto _LL54;_TL97:
# 607
 flags=flags & 4294966847U;goto ok;case 43: goto _LL84;case 45: _LL84:
# 612
 if(!(flags & 64))goto _TL99;
flags=flags & 4294967231U;goto ok;_TL99: goto _LL54;case 120: goto _LL88;case 88: _LL88:
# 620
 if(!(flags & 256))goto _TL9B;_TA4=p;_TA5=- 1;_TA6=_fat_ptr_plus(_TA4,sizeof(char),_TA5);_TA7=_TA6.curr;_TA8=(char*)_TA7;_TA9=buf;if(_TA8!=_TA9)goto _TL9B;
base=16;
flags=flags & 4294967039U;goto ok;_TL9B: goto _LL54;default: goto _LL54;}_LL54: goto _TL8E;
# 636
ok: _TAA=& p;{struct _fat_ptr _T138=
# 640
_fat_ptr_inplace_plus_post(_TAA,sizeof(char),1);_TAB=_check_fat_subscript(_T138,sizeof(char),0U);_TAC=(char*)_TAB;{char _T139=*_TAC;_TAD=c;{char _T13A=(char)_TAD;_TAE=_get_fat_size(_T138,sizeof(char));if(_TAE!=1U)goto _TL9D;if(_T139!=0)goto _TL9D;if(_T13A==0)goto _TL9D;_throw_arraybounds();goto _TL9E;_TL9D: _TL9E: _TAF=_T138.curr;_TB0=(char*)_TAF;*_TB0=_T13A;}}}
_IO_getc(fp);_TB1=
_IO_peekc(fp);if(_TB1!=-1)goto _TL9F;
seen_eof=seen_eof + 1;goto _TL8E;_TL9F:
# 554 "scanf.cyc"
 width=width + -1;goto _TL8F;_TL8E:
# 653 "scanf.cyc"
 if(!(flags & 128))goto _TLA1;_TB2=p;_TB3=_TB2.curr;_TB4=(char*)_TB3;_TB5=buf;
if(_TB4 <= _TB5)goto _TLA3;_TB6=& p;
_fat_ptr_inplace_plus(_TB6,sizeof(char),-1);_TB7=p;_TB8=_check_fat_subscript(_TB7,sizeof(char),0U);_TB9=(char*)_TB8;_TBA=*_TB9;_TBB=(int)_TBA;_TBC=fp;
_IO_ungetc(_TBB,_TBC);goto _TLA4;_TLA3: _TLA4: goto match_failure;_TLA1: _TBD=p;_TBE=_check_fat_subscript(_TBD,sizeof(char),-1);_TBF=(char*)_TBE;_TC0=*_TBF;
# 660
c=(int)_TC0;
if(c==120)goto _TLA7;else{goto _TLA8;}_TLA8: if(c==88)goto _TLA7;else{goto _TLA5;}
_TLA7: _TC1=& p;_fat_ptr_inplace_plus(_TC1,sizeof(char),-1);
_IO_ungetc(c,fp);goto _TLA6;_TLA5: _TLA6: _TC2=flags & 8;
# 665
if(_TC2!=0)goto _TLA9;{
unsigned long res;{struct _fat_ptr _T138=p;_TC3=_T138.curr;_TC4=(char*)_TC3;{char _T139=*_TC4;char _T13A='\000';_TC5=_get_fat_size(_T138,sizeof(char));if(_TC5!=1U)goto _TLAB;if(_T139!=0)goto _TLAB;if(_T13A==0)goto _TLAB;_throw_arraybounds();goto _TLAC;_TLAB: _TLAC: _TC6=_T138.curr;_TC7=(char*)_TC6;*_TC7=_T13A;}}_TC8=use_strtoul;
# 669
if(!_TC8)goto _TLAD;
res=strtoul(buf,0,base);goto _TLAE;
# 672
_TLAD: _TC9=strtol(buf,0,base);res=(unsigned long)_TC9;_TLAE:
 if(!(flags & 16))goto _TLAF;_TCA=ap;_TCB=_TCA.curr;_TCC=(void**)_TCB;_TCD=*_TCC;_TCE=
Cyc_va_arg_int_ptr(_TCD);_TCF=res;*_TCE=(int)_TCF;goto _TLB0;
_TLAF: if(!(flags & 4))goto _TLB1;_TD0=ap;_TD1=_TD0.curr;_TD2=(void**)_TD1;_TD3=*_TD2;_TD4=
Cyc_va_arg_short_ptr(_TD3);_TD5=res;*_TD4=(short)_TD5;goto _TLB2;
_TLB1: if(!(flags & 1))goto _TLB3;_TD6=ap;_TD7=_TD6.curr;_TD8=(void**)_TD7;_TD9=*_TD8;_TDA=
Cyc_va_arg_int_ptr(_TD9);_TDB=res;*_TDA=(int)_TDB;goto _TLB4;
# 680
_TLB3: _TDC=ap;_TDD=_TDC.curr;_TDE=(void**)_TDD;_TDF=*_TDE;_TE0=Cyc_va_arg_int_ptr(_TDF);_TE1=res;*_TE0=(int)_TE1;_TLB4: _TLB2: _TLB0: _TE2=& ap;
_fat_ptr_inplace_plus(_TE2,sizeof(void*),1);
nassigned=nassigned + 1;}goto _TLAA;_TLA9: _TLAA: _TE3=p;_TE4=_TE3.curr;_TE5=buf;_TE6=
# 684
_tag_fat(_TE5,sizeof(char),351U);_TE7=_TE6.curr;_TE8=_TE4 - _TE7;_TE9=_TE8 / sizeof(char);nread=nread + _TE9;goto _LL47;case 4:
# 689
 if(width==0)goto _TLB7;else{goto _TLB8;}_TLB8: _TEA=width;_TEB=sizeof(buf);_TEC=_TEB - 1U;_TED=(long long)_TEC;if(_TEA > _TED)goto _TLB7;else{goto _TLB5;}
_TLB7: _TEE=sizeof(buf);_TEF=_TEE - 1U;width=(long long)_TEF;goto _TLB6;_TLB5: _TLB6:
 flags=flags | 960U;_TF0=buf;
p=_tag_fat(_TF0,sizeof(char),351U);_TLBC: if(width!=0)goto _TLBA;else{goto _TLBB;}
_TLBA: c=_IO_peekc(fp);_TF1=c;_TF2=(int)_TF1;switch(_TF2){case 48: goto _LL8F;case 49: _LL8F: goto _LL91;case 50: _LL91: goto _LL93;case 51: _LL93: goto _LL95;case 52: _LL95: goto _LL97;case 53: _LL97: goto _LL99;case 54: _LL99: goto _LL9B;case 55: _LL9B: goto _LL9D;case 56: _LL9D: goto _LL9F;case 57: _LL9F:
# 703
 flags=flags & 4294967103U;goto fok;case 43: goto _LLA3;case 45: _LLA3:
# 707
 if(!(flags & 64))goto _TLBE;
flags=flags & 4294967231U;goto fok;_TLBE: goto _LL8B;case 46:
# 713
 if(!(flags & 256))goto _TLC0;
flags=flags & 4294966975U;goto fok;_TLC0: goto _LL8B;case 101: goto _LLA9;case 69: _LLA9: _TF3=flags & 640U;
# 720
if(_TF3!=512)goto _TLC2;_TF4=flags & 4294966527U;_TF5=_TF4 | 64;
flags=_TF5 | 128;goto fok;_TLC2: goto _LL8B;default: goto _LL8B;}_LL8B: goto _TLBB;
# 731
fok: _TF6=& p;{struct _fat_ptr _T138=
_fat_ptr_inplace_plus_post(_TF6,sizeof(char),1);_TF7=_check_fat_subscript(_T138,sizeof(char),0U);_TF8=(char*)_TF7;{char _T139=*_TF8;_TF9=c;{char _T13A=(char)_TF9;_TFA=_get_fat_size(_T138,sizeof(char));if(_TFA!=1U)goto _TLC4;if(_T139!=0)goto _TLC4;if(_T13A==0)goto _TLC4;_throw_arraybounds();goto _TLC5;_TLC4: _TLC5: _TFB=_T138.curr;_TFC=(char*)_TFB;*_TFC=_T13A;}}}
_IO_getc(fp);_TFD=
_IO_peekc(fp);if(_TFD!=-1)goto _TLC6;
seen_eof=seen_eof + 1;goto _TLBB;_TLC6:
# 692
 width=width + -1;goto _TLBC;_TLBB:
# 744
 if(!(flags & 128))goto _TLC8;
if(!(flags & 512))goto _TLCA;
# 747
_TLCC: _TFE=p;_TFF=_TFE.curr;_T100=(char*)_TFF;_T101=buf;if(_T100 > _T101)goto _TLCD;else{goto _TLCE;}
_TLCD: _T102=& p;_fat_ptr_inplace_plus(_T102,sizeof(char),-1);_T103=p;_T104=_check_fat_subscript(_T103,sizeof(char),0U);_T105=(char*)_T104;_T106=*_T105;_T107=(int)_T106;_T108=fp;
_IO_ungetc(_T107,_T108);goto _TLCC;_TLCE: goto match_failure;_TLCA: _T109=& p;
# 754
_fat_ptr_inplace_plus(_T109,sizeof(char),-1);_T10A=p;_T10B=_check_fat_subscript(_T10A,sizeof(char),0U);_T10C=(char*)_T10B;_T10D=*_T10C;
c=(int)_T10D;
if(c==101)goto _TLCF;if(c==69)goto _TLCF;
_IO_ungetc(c,fp);_T10E=& p;
_fat_ptr_inplace_plus(_T10E,sizeof(char),-1);_T10F=p;_T110=_check_fat_subscript(_T10F,sizeof(char),0U);_T111=(char*)_T110;_T112=*_T111;
c=(int)_T112;goto _TLD0;_TLCF: _TLD0:
# 761
 _IO_ungetc(c,fp);goto _TLC9;_TLC8: _TLC9: _T113=flags & 8;
# 763
if(_T113!=0)goto _TLD1;{
double res;{struct _fat_ptr _T138=p;_T114=_check_fat_subscript(_T138,sizeof(char),0U);_T115=(char*)_T114;{char _T139=*_T115;char _T13A='\000';_T116=_get_fat_size(_T138,sizeof(char));if(_T116!=1U)goto _TLD3;if(_T139!=0)goto _TLD3;if(_T13A==0)goto _TLD3;_throw_arraybounds();goto _TLD4;_TLD3: _TLD4: _T117=_T138.curr;_T118=(char*)_T117;*_T118=_T13A;}}_T119=buf;_T11A=(const char*)_T119;
# 766
res=atof(_T11A);
if(!(flags & 1))goto _TLD5;_T11B=ap;_T11C=_check_fat_subscript(_T11B,sizeof(void*),0U);_T11D=(void**)_T11C;_T11E=*_T11D;_T11F=
Cyc_va_arg_double_ptr(_T11E);*_T11F=res;goto _TLD6;
# 770
_TLD5: _T120=ap;_T121=_check_fat_subscript(_T120,sizeof(void*),0U);_T122=(void**)_T121;_T123=*_T122;_T124=Cyc_va_arg_float_ptr(_T123);_T125=res;*_T124=(float)_T125;_TLD6: _T126=& ap;
_fat_ptr_inplace_plus(_T126,sizeof(void*),1);
nassigned=nassigned + 1;}goto _TLD2;_TLD1: _TLD2: _T127=p;_T128=_T127.curr;_T129=buf;_T12A=
# 774
_tag_fat(_T129,sizeof(char),351U);_T12B=_T12A.curr;_T12C=_T128 - _T12B;_T12D=_T12C / sizeof(char);nread=nread + _T12D;goto _LL47;default:{struct Cyc_Core_Impossible_exn_struct*_T138=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T138->tag=Cyc_Core_Impossible;
# 777
_T138->f1=_tag_fat("scanf3",sizeof(char),7U);_T12E=(struct Cyc_Core_Impossible_exn_struct*)_T138;}_T12F=(void*)_T12E;_throw(_T12F);}_LL47: _TLC: goto _TLF;_TLE:
# 781
 eof_failure:
 seen_eof=seen_eof + 1;
# 784
if(nassigned!=0)goto _TLD7;
nassigned=-1;goto _TLD8;_TLD7: _TLD8:
 control_failure:
 match_failure: _T130=errp;_T131=(unsigned)_T130;
if(!_T131)goto _TLD9;_T132=errp;
*_T132=*_T132 | 2;goto _TLDA;_TLD9: _TLDA:
 done: _T133=errp;_T134=(unsigned)_T133;
if(!_T134)goto _TLDB;_T135=seen_eof;if(!_T135)goto _TLDB;_T136=errp;
*_T136=*_T136 | 1;goto _TLDC;_TLDB: _TLDC: _T137=nassigned;
return _T137;}}
# 802
static struct _fat_ptr Cyc___sccl(char*tab,struct _fat_ptr fmt){struct _fat_ptr*_T0;struct _fat_ptr _T1;char*_T2;const char*_T3;char _T4;struct _fat_ptr*_T5;struct _fat_ptr _T6;char*_T7;const char*_T8;char _T9;char*_TA;int _TB;char*_TC;char*_TD;int _TE;struct _fat_ptr _TF;int _T10;struct _fat_ptr _T11;char*_T12;int _T13;char*_T14;char*_T15;int _T16;struct _fat_ptr*_T17;struct _fat_ptr _T18;char*_T19;const char*_T1A;char _T1B;int _T1C;int _T1D;struct _fat_ptr _T1E;int _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;char*_T22;const char*_T23;char _T24;struct _fat_ptr*_T25;char*_T26;int _T27;char*_T28;char*_T29;int _T2A;struct _fat_ptr _T2B;
# 807
int c;int n;int v;_T0=& fmt;_T1=
# 810
_fat_ptr_inplace_plus_post(_T0,sizeof(char),1);_T2=_check_fat_subscript(_T1,sizeof(char),0U);_T3=(const char*)_T2;_T4=*_T3;c=(int)_T4;
if(c!=94)goto _TLDD;
v=1;_T5=& fmt;_T6=
_fat_ptr_inplace_plus_post(_T5,sizeof(char),1);_T7=_check_fat_subscript(_T6,sizeof(char),0U);_T8=(const char*)_T7;_T9=*_T8;c=(int)_T9;goto _TLDE;
# 815
_TLDD: v=0;_TLDE:
# 817
 n=0;_TLE2: if(n < 256)goto _TLE0;else{goto _TLE1;}
_TLE0: _TA=tab;_TB=n;_TC=_check_known_subscript_notnull(_TA,256U,sizeof(char),_TB);_TD=(char*)_TC;_TE=v;*_TD=(char)_TE;
# 817
n=n + 1;goto _TLE2;_TLE1:
# 819
 if(c!=0)goto _TLE3;_TF=fmt;_T10=- 1;_T11=
_fat_ptr_plus(_TF,sizeof(char),_T10);return _T11;_TLE3:
# 829 "scanf.cyc"
 v=1 - v;_TLE8:
 if(1)goto _TLE6;else{goto _TLE7;}
_TLE6: _T12=tab;_T13=c;_T14=_check_known_subscript_notnull(_T12,256U,sizeof(char),_T13);_T15=(char*)_T14;_T16=v;*_T15=(char)_T16;
doswitch: _T17=& fmt;_T18=
_fat_ptr_inplace_plus_post(_T17,sizeof(char),1);_T19=_check_fat_subscript(_T18,sizeof(char),0U);_T1A=(const char*)_T19;_T1B=*_T1A;n=(int)_T1B;_T1C=n;_T1D=(int)_T1C;switch(_T1D){case 0: _T1E=fmt;_T1F=- 1;_T20=
# 837
_fat_ptr_plus(_T1E,sizeof(char),_T1F);return _T20;case 45: _T21=fmt;_T22=_check_fat_subscript(_T21,sizeof(char),0U);_T23=(const char*)_T22;_T24=*_T23;
# 858 "scanf.cyc"
n=(int)_T24;
if(n==93)goto _TLEC;else{goto _TLED;}_TLED: if(n < c)goto _TLEC;else{goto _TLEA;}
_TLEC: c=45;goto _LL0;_TLEA: _T25=& fmt;
# 863
_fat_ptr_inplace_plus(_T25,sizeof(char),1);
# 865
_TLEE: _T26=tab;c=c + 1;_T27=c;_T28=_check_known_subscript_notnull(_T26,256U,sizeof(char),_T27);_T29=(char*)_T28;_T2A=v;*_T29=(char)_T2A;
# 864
if(c < n)goto _TLEE;else{goto _TLEF;}_TLEF: goto doswitch;goto _LL0;case 93: _T2B=fmt;
# 884
return _T2B;default:
# 887
 c=n;goto _LL0;}_LL0: goto _TLE8;_TLE7:;}
# 896
static int Cyc_string_getc(struct _fat_ptr*sptr){struct _fat_ptr*_T0;struct _fat_ptr _T1;unsigned char*_T2;char*_T3;struct _fat_ptr _T4;unsigned _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;char _T9;int _TA;struct _fat_ptr*_TB;struct _fat_ptr _TC;char _TD;int _TE;
char c;_T0=sptr;{
struct _fat_ptr s=*_T0;_T1=s;_T2=_T1.curr;_T3=(char*)_T2;
if(_T3==0)goto _TLF2;else{goto _TLF4;}_TLF4: _T4=s;_T5=_get_fat_size(_T4,sizeof(char));if(_T5==0U)goto _TLF2;else{goto _TLF3;}_TLF3: _T6=s;_T7=_T6.curr;_T8=(const char*)_T7;c=_T8[0];_T9=c;_TA=(int)_T9;if(_TA==0)goto _TLF2;else{goto _TLF0;}_TLF2: return -1;_TLF0: _TB=sptr;_TC=s;
*_TB=_fat_ptr_plus(_TC,sizeof(char),1);_TD=c;_TE=(int)_TD;
return _TE;}}
# 904
static int Cyc_string_ungetc(int ignore,struct _fat_ptr*sptr){struct _fat_ptr*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;int _T3;_T0=sptr;_T1=sptr;_T2=*_T1;_T3=- 1;
*_T0=_fat_ptr_plus(_T2,sizeof(char),_T3);
# 907
return 0;}
# 910
static int Cyc_string_peekc(struct _fat_ptr*sptr){struct _fat_ptr*_T0;struct _fat_ptr _T1;unsigned char*_T2;char*_T3;struct _fat_ptr _T4;unsigned _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;char _T9;int _TA;char _TB;int _TC;
char c;_T0=sptr;{
struct _fat_ptr s=*_T0;_T1=s;_T2=_T1.curr;_T3=(char*)_T2;
if(_T3==0)goto _TLF7;else{goto _TLF9;}_TLF9: _T4=s;_T5=_get_fat_size(_T4,sizeof(char));if(_T5==0U)goto _TLF7;else{goto _TLF8;}_TLF8: _T6=s;_T7=_T6.curr;_T8=(const char*)_T7;c=_T8[0];_T9=c;_TA=(int)_T9;if(_TA==0)goto _TLF7;else{goto _TLF5;}_TLF7: return -1;_TLF5: _TB=c;_TC=(int)_TB;
return _TC;}}
# 917
int Cyc_vsscanf(struct _fat_ptr src1,struct _fat_ptr fmt,struct _fat_ptr ap){struct _fat_ptr _T0;int(*_T1)(int(*)(struct _fat_ptr*),int(*)(int,struct _fat_ptr*),int(*)(struct _fat_ptr*),struct _fat_ptr*,struct _fat_ptr,struct _fat_ptr,int*);struct _fat_ptr*_T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;int*_T6;int _T7;_T0=src1;{
# 920
struct _fat_ptr src=_T0;
int err=0;{
int(*_T8)(int(*)(struct _fat_ptr*),int(*)(int,struct _fat_ptr*),int(*)(struct _fat_ptr*),struct _fat_ptr*,struct _fat_ptr,struct _fat_ptr,int*)=(int(*)(int(*)(struct _fat_ptr*),int(*)(int,struct _fat_ptr*),int(*)(struct _fat_ptr*),struct _fat_ptr*,struct _fat_ptr,struct _fat_ptr,int*))Cyc__IO_vfscanf;_T1=_T8;}_T2=& src;_T3=(struct _fat_ptr*)_T2;_T4=fmt;_T5=ap;_T6=& err;_T7=_T1(Cyc_string_getc,Cyc_string_ungetc,Cyc_string_peekc,_T3,_T4,_T5,_T6);return _T7;}}
# 926
int Cyc_sscanf(struct _fat_ptr src,struct _fat_ptr fmt,struct _fat_ptr ap){int _T0;_T0=
# 929
Cyc_vsscanf(src,fmt,ap);return _T0;}
# 933
int Cyc_peekc(struct Cyc___cycFILE*stream){int _T0;
int c=Cyc_fgetc(stream);
Cyc_ungetc(c,stream);_T0=c;
return _T0;}
# 939
int Cyc_vfscanf(struct Cyc___cycFILE*stream,struct _fat_ptr fmt,struct _fat_ptr ap){int(*_T0)(int(*)(struct Cyc___cycFILE*),int(*)(int,struct Cyc___cycFILE*),int(*)(struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr,int*);int(*_T1)(struct Cyc___cycFILE*);int(*_T2)(int,struct Cyc___cycFILE*);struct Cyc___cycFILE*_T3;struct _fat_ptr _T4;struct _fat_ptr _T5;int*_T6;int _T7;
# 942
int err=0;{
int(*_T8)(int(*)(struct Cyc___cycFILE*),int(*)(int,struct Cyc___cycFILE*),int(*)(struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr,int*)=(int(*)(int(*)(struct Cyc___cycFILE*),int(*)(int,struct Cyc___cycFILE*),int(*)(struct Cyc___cycFILE*),struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr,int*))Cyc__IO_vfscanf;_T0=_T8;}_T1=Cyc_getc;_T2=Cyc_ungetc;_T3=stream;_T4=fmt;_T5=ap;_T6=& err;_T7=_T0(_T1,_T2,Cyc_peekc,_T3,_T4,_T5,_T6);return _T7;}
# 946
int Cyc_fscanf(struct Cyc___cycFILE*stream,struct _fat_ptr fmt,struct _fat_ptr ap){int _T0;_T0=
# 949
Cyc_vfscanf(stream,fmt,ap);return _T0;}
# 952
int Cyc_scanf(struct _fat_ptr fmt,struct _fat_ptr ap){int _T0;_T0=
# 955
Cyc_vfscanf(Cyc_stdin,fmt,ap);return _T0;}
