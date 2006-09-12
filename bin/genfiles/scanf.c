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
int Cyc__IO_vfscanf(int(*_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*_IO_peekc)(void*),void*fp,struct _fat_ptr fmt0,struct _fat_ptr ap,int*errp){char*_T0;unsigned _T1;char*_T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;unsigned char*_T5;const char*_T6;char _T7;int _T8;int _T9;int _TA;int _TB;struct _fat_ptr*_TC;struct _fat_ptr _TD;unsigned char*_TE;const char*_TF;char _T10;int _T11;int _T12;int _T13;int _T14;int _T15;int _T16;int _T17;int _T18;int _T19;int _T1A;int _T1B;int _T1C;int _T1D;int _T1E;int _T1F;int _T20;int _T21;long long _T22;int _T23;long long _T24;long long _T25;int _T26;struct _fat_ptr _T27;unsigned char*_T28;void**_T29;void*_T2A;short*_T2B;int _T2C;struct _fat_ptr _T2D;unsigned char*_T2E;void**_T2F;void*_T30;int*_T31;struct _fat_ptr _T32;unsigned char*_T33;void**_T34;void*_T35;int*_T36;struct _fat_ptr*_T37;int _T38;int _T39;int _T3A;int _T3B;int _T3C;int _T3D;int _T3E;int _T3F;int _T40;int _T41;int _T42;int _T43;int _T44;long long _T45;struct _fat_ptr _T46;unsigned char*_T47;void**_T48;void*_T49;struct _fat_ptr*_T4A;int _T4B;int _T4C;int _T4D;int _T4E;struct _fat_ptr _T4F;unsigned char*_T50;char*_T51;int _T52;struct _fat_ptr*_T53;long long _T54;char*_T55;int _T56;char _T57;int _T58;char*_T59;char*_T5A;char _T5B;int _T5C;long long _T5D;int _T5E;int _T5F;struct _fat_ptr _T60;unsigned char*_T61;void**_T62;void*_T63;struct _fat_ptr _T64;struct _fat_ptr*_T65;char*_T66;int _T67;char _T68;int _T69;char*_T6A;char*_T6B;char _T6C;int _T6D;struct _fat_ptr _T6E;unsigned _T6F;struct _fat_ptr _T70;unsigned char*_T71;char*_T72;char*_T73;int _T74;struct _fat_ptr*_T75;long long _T76;int _T77;int _T78;struct _fat_ptr _T79;unsigned char*_T7A;char*_T7B;struct _fat_ptr _T7C;unsigned char*_T7D;char*_T7E;struct _fat_ptr _T7F;unsigned char*_T80;struct _fat_ptr _T81;unsigned char*_T82;int _T83;struct _fat_ptr _T84;unsigned _T85;struct _fat_ptr _T86;unsigned char*_T87;char*_T88;char*_T89;int _T8A;int _T8B;unsigned char _T8C;int _T8D;int _T8E;long long _T8F;int _T90;int _T91;int _T92;struct _fat_ptr _T93;unsigned char*_T94;void**_T95;void*_T96;struct _fat_ptr*_T97;int _T98;unsigned char _T99;int _T9A;int _T9B;struct _fat_ptr _T9C;unsigned _T9D;struct _fat_ptr _T9E;unsigned char*_T9F;char*_TA0;char*_TA1;int _TA2;struct _fat_ptr*_TA3;long long _TA4;int _TA5;int _TA6;struct _fat_ptr _TA7;unsigned _TA8;struct _fat_ptr _TA9;unsigned char*_TAA;char*_TAB;char*_TAC;struct _fat_ptr _TAD;unsigned char*_TAE;struct _fat_ptr _TAF;unsigned char*_TB0;int _TB1;int _TB2;long long _TB3;unsigned _TB4;unsigned _TB5;long long _TB6;unsigned _TB7;unsigned _TB8;int _TB9;int _TBA;char*_TBB;int _TBC;unsigned char _TBD;int _TBE;int _TBF;int _TC0;int _TC1;int _TC2;int _TC3;int _TC4;int _TC5;short*_TC6;int _TC7;char*_TC8;short*_TC9;short _TCA;int _TCB;int _TCC;int _TCD;short*_TCE;int _TCF;char*_TD0;short*_TD1;short _TD2;int _TD3;int _TD4;int _TD5;int _TD6;int _TD7;int _TD8;int _TD9;struct _fat_ptr _TDA;int _TDB;struct _fat_ptr _TDC;unsigned char*_TDD;char*_TDE;char*_TDF;int _TE0;struct _fat_ptr*_TE1;unsigned char*_TE2;char*_TE3;int _TE4;unsigned _TE5;unsigned char*_TE6;char*_TE7;int _TE8;int _TE9;struct _fat_ptr _TEA;unsigned char*_TEB;char*_TEC;char*_TED;struct _fat_ptr*_TEE;struct _fat_ptr _TEF;unsigned char*_TF0;char*_TF1;char _TF2;int _TF3;void*_TF4;struct _fat_ptr _TF5;int _TF6;unsigned char*_TF7;char*_TF8;char _TF9;struct _fat_ptr*_TFA;int _TFB;unsigned char*_TFC;char*_TFD;unsigned _TFE;unsigned char*_TFF;char*_T100;int _T101;long _T102;struct _fat_ptr _T103;unsigned char*_T104;void**_T105;void*_T106;int*_T107;unsigned long _T108;struct _fat_ptr _T109;unsigned char*_T10A;void**_T10B;void*_T10C;short*_T10D;unsigned long _T10E;struct _fat_ptr _T10F;unsigned char*_T110;void**_T111;void*_T112;int*_T113;unsigned long _T114;struct _fat_ptr _T115;unsigned char*_T116;void**_T117;void*_T118;int*_T119;unsigned long _T11A;struct _fat_ptr*_T11B;struct _fat_ptr _T11C;unsigned char*_T11D;char*_T11E;struct _fat_ptr _T11F;unsigned char*_T120;int _T121;int _T122;long long _T123;unsigned _T124;unsigned _T125;long long _T126;unsigned _T127;unsigned _T128;int _T129;int _T12A;int _T12B;char*_T12C;int _T12D;int _T12E;int _T12F;int _T130;int _T131;int _T132;int _T133;int _T134;int _T135;int _T136;int _T137;int _T138;int _T139;int _T13A;int _T13B;struct _fat_ptr*_T13C;unsigned char*_T13D;char*_T13E;int _T13F;unsigned _T140;unsigned char*_T141;char*_T142;int _T143;int _T144;struct _fat_ptr _T145;unsigned char*_T146;char*_T147;char*_T148;struct _fat_ptr*_T149;struct _fat_ptr _T14A;unsigned char*_T14B;char*_T14C;char _T14D;int _T14E;void*_T14F;struct _fat_ptr*_T150;struct _fat_ptr _T151;unsigned char*_T152;char*_T153;char _T154;struct _fat_ptr*_T155;struct _fat_ptr _T156;unsigned char*_T157;char*_T158;char _T159;int _T15A;unsigned char*_T15B;char*_T15C;unsigned _T15D;unsigned char*_T15E;char*_T15F;char*_T160;const char*_T161;struct _fat_ptr _T162;unsigned char*_T163;void**_T164;void*_T165;double*_T166;struct _fat_ptr _T167;unsigned char*_T168;void**_T169;void*_T16A;float*_T16B;double _T16C;struct _fat_ptr*_T16D;struct _fat_ptr _T16E;unsigned char*_T16F;char*_T170;struct _fat_ptr _T171;unsigned char*_T172;int _T173;int _T174;struct Cyc_Core_Impossible_exn_struct*_T175;void*_T176;int*_T177;unsigned _T178;int*_T179;int*_T17A;unsigned _T17B;int _T17C;int*_T17D;int _T17E;
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
char buf[351U];{unsigned _T17F=350U;unsigned i;i=0;_TLB: if(i < _T17F)goto _TL9;else{goto _TLA;}_TL9: _T0=buf;_T1=i;_T0[_T1]='0';i=i + 1;goto _TLB;_TLA: _T2=buf;_T2[_T17F]=0;}{
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
_TL15: c=_IO_getc(fp);_T9=c;_TA=- 1;
if(_T9!=_TA)goto _TL18;
seen_eof=seen_eof + 1;goto _TL16;_TL18: _TB=
# 236
isspace(c);if(_TB)goto _TL1A;else{goto _TL1C;}
_TL1C: _IO_ungetc(c,fp);goto _TL16;_TL1A:
# 240
 nread=nread + 1;goto _TL17;_TL16: goto _TLC;_TL12:
# 244
 if(c==37)goto _TL1D;goto literal;_TL1D:
# 246
 width=0;
flags=0;
# 252
again: _TC=& fmt;_TD=_fat_ptr_inplace_plus_post(_TC,sizeof(char),1);_TE=_check_fat_subscript(_TD,sizeof(char),0U);_TF=(const char*)_TE;_T10=*_TF;c=(int)_T10;_T11=c;_T12=(int)_T11;switch(_T12){case 37:
# 255
 literal:
 n=_IO_getc(fp);_T13=n;_T14=- 1;
if(_T13!=_T14)goto _TL20;goto eof_failure;_TL20:
# 259
 if(n==c)goto _TL22;
_IO_ungetc(n,fp);goto match_failure;_TL22:
# 263
 nread=nread + 1;goto _TLC;case 42: _T15=flags;
# 267
if(!_T15)goto _TL24;goto control_failure;_TL24:
 flags=8;goto again;case 108: _T16=flags;_T17=8 | 64;_T18=~ _T17;
# 271
if(!(_T16 & _T18))goto _TL26;goto control_failure;_TL26:
 flags=flags | 1;goto again;case 76: _T19=flags;_T1A=8 | 64;_T1B=~ _T1A;
# 275
if(!(_T19 & _T1B))goto _TL28;goto control_failure;_TL28:
 flags=flags | 2;goto again;case 104: _T1C=flags;_T1D=8 | 64;_T1E=~ _T1D;
# 279
if(!(_T1C & _T1E))goto _TL2A;goto control_failure;_TL2A:
 flags=flags | 4;goto again;case 48: goto _LLE;case 49: _LLE: goto _LL10;case 50: _LL10: goto _LL12;case 51: _LL12: goto _LL14;case 52: _LL14: goto _LL16;case 53: _LL16: goto _LL18;case 54: _LL18: goto _LL1A;case 55: _LL1A: goto _LL1C;case 56: _LL1C: goto _LL1E;case 57: _LL1E: _T1F=flags;_T20=8 | 64;_T21=~ _T20;
# 285
if(!(_T1F & _T21))goto _TL2C;goto control_failure;_TL2C:
 flags=flags | 64;_T22=width * 10;_T23=c;_T24=(long long)_T23;_T25=_T22 + _T24;
width=_T25 - 48;goto again;case 68:
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
c=0;goto _LL0;case 112: _T26=16 | 256;
# 354
flags=flags | _T26;
c=3;
use_strtoul=1;
base=16;goto _LL0;case 110:
# 361
 if(!(flags & 8))goto _TL2E;goto _TLC;_TL2E:
# 363
 if(!(flags & 4))goto _TL30;_T27=ap;_T28=_check_fat_subscript(_T27,sizeof(void*),0U);_T29=(void**)_T28;_T2A=*_T29;_T2B=
Cyc_va_arg_short_ptr(_T2A);_T2C=nread;*_T2B=(short)_T2C;goto _TL31;
_TL30: if(!(flags & 1))goto _TL32;_T2D=ap;_T2E=_check_fat_subscript(_T2D,sizeof(void*),0U);_T2F=(void**)_T2E;_T30=*_T2F;_T31=
Cyc_va_arg_int_ptr(_T30);*_T31=nread;goto _TL33;
# 368
_TL32: _T32=ap;_T33=_check_fat_subscript(_T32,sizeof(void*),0U);_T34=(void**)_T33;_T35=*_T34;_T36=Cyc_va_arg_int_ptr(_T35);*_T36=nread;_TL33: _TL31: _T37=& ap;
_fat_ptr_inplace_plus(_T37,sizeof(void*),1);goto _TLC;case 0:
# 376
 nassigned=- 1;goto done;default: _T38=
# 380
isupper(c);if(!_T38)goto _TL34;
flags=flags | 1;goto _TL35;_TL34: _TL35:
 c=3;
use_strtoul=0;
base=10;goto _LL0;}_LL0: _T39=
# 391
_IO_peekc(fp);_T3A=- 1;if(_T39!=_T3A)goto _TL36;goto eof_failure;_TL36: _T3B=flags & 32;
# 398
if(_T3B!=0)goto _TL38;
n=_IO_peekc(fp);
_TL3A: _T3C=isspace(n);if(_T3C)goto _TL3B;else{goto _TL3C;}
_TL3B: n=_IO_getc(fp);
nread=nread + 1;
n=_IO_peekc(fp);_T3D=n;_T3E=- 1;
if(_T3D!=_T3E)goto _TL3D;goto eof_failure;_TL3D: goto _TL3A;_TL3C: goto _TL39;_TL38: _TL39: _T3F=c;_T40=(int)_T3F;switch(_T40){case 0:
# 421
 if(width!=0)goto _TL40;
width=1;goto _TL41;_TL40: _TL41:
 if(!(flags & 8))goto _TL42;{
long long sum=0;
_TL47: if(width > 0)goto _TL45;else{goto _TL46;}
_TL45: n=_IO_getc(fp);_T41=n;_T42=- 1;
if(_T41!=_T42)goto _TL48;if(width==0)goto _TL48;goto eof_failure;
# 429
_TL48: _T43=n;_T44=- 1;if(_T43!=_T44)goto _TL4A;
seen_eof=seen_eof + 1;goto _TL46;_TL4A:
# 433
 sum=sum + 1;
width=width + -1;goto _TL47;_TL46: _T45=sum;
# 436
nread=nread + _T45;}goto _TL43;
# 438
_TL42:{long long sum=0;_T46=ap;_T47=_check_fat_subscript(_T46,sizeof(void*),0U);_T48=(void**)_T47;_T49=*_T48;{
struct _fat_ptr z=Cyc_va_arg_char_ptr(_T49);_T4A=& ap;_fat_ptr_inplace_plus(_T4A,sizeof(void*),1);
_TL4F: if(width > 0)goto _TL4D;else{goto _TL4E;}
_TL4D: n=_IO_getc(fp);_T4B=n;_T4C=- 1;
if(_T4B!=_T4C)goto _TL50;if(width==0)goto _TL50;goto eof_failure;
# 444
_TL50: _T4D=n;_T4E=- 1;if(_T4D!=_T4E)goto _TL52;
seen_eof=seen_eof + 1;goto _TL4E;_TL52: _T4F=z;_T50=_check_fat_subscript(_T4F,sizeof(char),0U);_T51=(char*)_T50;_T52=n;
# 448
*_T51=(char)_T52;_T53=& z;
_fat_ptr_inplace_plus(_T53,sizeof(char),1);
sum=sum + 1;
width=width + -1;goto _TL4F;_TL4E: _T54=sum;
# 453
nread=nread + _T54;
nassigned=nassigned + 1;}}_TL43: goto _LL47;case 1:
# 460
 if(width!=0)goto _TL54;
width=-1;goto _TL55;_TL54: _TL55:
# 463
 if(!(flags & 8))goto _TL56;
n=0;{
int c=_IO_peekc(fp);
_TL58: _T55=ccltab;_T56=c;_T57=(char)_T56;_T58=(int)_T57;_T59=_check_known_subscript_notnull(_T55,256U,sizeof(char),_T58);_T5A=(char*)_T59;_T5B=*_T5A;_T5C=(int)_T5B;if(_T5C)goto _TL59;else{goto _TL5A;}
_TL59: n=n + 1;
_IO_getc(fp);
width=width + -1;_T5D=width;if(_T5D!=0)goto _TL5B;goto _TL5A;_TL5B:
# 471
 c=_IO_peekc(fp);_T5E=c;_T5F=- 1;if(_T5E!=_T5F)goto _TL5D;
if(n!=0)goto _TL5F;goto eof_failure;_TL5F:
# 474
 seen_eof=seen_eof + 1;goto _TL5A;_TL5D: goto _TL58;_TL5A:
# 478
 if(n!=0)goto _TL61;goto match_failure;_TL61:;}goto _TL57;
# 481
_TL56: _T60=ap;_T61=_check_fat_subscript(_T60,sizeof(void*),0U);_T62=(void**)_T61;_T63=*_T62;_T64=Cyc_va_arg_string_ptr(_T63);{struct _fat_ptr p4=_T64;_T65=& ap;_fat_ptr_inplace_plus(_T65,sizeof(void*),1);{
struct _fat_ptr p5=p4;
int c=_IO_peekc(fp);
_TL63: _T66=ccltab;_T67=c;_T68=(char)_T67;_T69=(int)_T68;_T6A=_check_known_subscript_notnull(_T66,256U,sizeof(char),_T69);_T6B=(char*)_T6A;_T6C=*_T6B;_T6D=(int)_T6C;if(_T6D)goto _TL64;else{goto _TL65;}
_TL64: _T6E=p5;_T6F=_get_fat_size(_T6E,sizeof(char));if(_T6F!=0U)goto _TL66;goto eof_failure;_TL66: _T70=p5;_T71=_T70.curr;_T72=(char*)_T71;_T73=
_check_null(_T72);_T74=c;*_T73=(char)_T74;_T75=& p5;
_fat_ptr_inplace_plus(_T75,sizeof(char),1);
_IO_getc(fp);
width=width + -1;_T76=width;if(_T76!=0)goto _TL68;goto _TL65;_TL68:
# 491
 c=_IO_peekc(fp);_T77=c;_T78=- 1;if(_T77!=_T78)goto _TL6A;_T79=p5;_T7A=_T79.curr;_T7B=(char*)_T7A;_T7C=p0;_T7D=_T7C.curr;_T7E=(char*)_T7D;
if(_T7B!=_T7E)goto _TL6C;goto eof_failure;_TL6C:
# 494
 seen_eof=seen_eof + 1;goto _TL65;_TL6A: goto _TL63;_TL65: _T7F=p5;_T80=_T7F.curr;_T81=p4;_T82=_T81.curr;_T83=_T80 - _T82;
# 498
n=_T83 / sizeof(char);
if(n!=0)goto _TL6E;goto match_failure;_TL6E: _T84=p5;_T85=
# 501
_get_fat_size(_T84,sizeof(char));if(_T85!=0U)goto _TL70;goto eof_failure;_TL70: _T86=p5;_T87=_T86.curr;_T88=(char*)_T87;_T89=
_check_null(_T88);*_T89='\000';
nassigned=nassigned + 1;}}_TL57: _T8A=n;
# 505
nread=nread + _T8A;goto _LL47;case 2:
# 510
 if(width!=0)goto _TL72;
width=-1;goto _TL73;_TL72: _TL73:
 if(!(flags & 8))goto _TL74;
n=0;{
int c=_IO_peekc(fp);
_TL76: _T8B=c;_T8C=(unsigned char)_T8B;_T8D=(int)_T8C;_T8E=isspace(_T8D);if(_T8E)goto _TL78;else{goto _TL77;}
_TL77: n=n + 1;
_IO_getc(fp);
width=width + -1;_T8F=width;if(_T8F!=0)goto _TL79;goto _TL78;_TL79:
# 520
 c=_IO_peekc(fp);_T90=c;_T91=- 1;if(_T90!=_T91)goto _TL7B;
seen_eof=seen_eof + 1;goto _TL78;_TL7B: goto _TL76;_TL78: _T92=n;
# 525
nread=nread + _T92;}goto _TL75;
# 527
_TL74: _T93=ap;_T94=_check_fat_subscript(_T93,sizeof(void*),0U);_T95=(void**)_T94;_T96=*_T95;{struct _fat_ptr p2=Cyc_va_arg_string_ptr(_T96);_T97=& ap;_fat_ptr_inplace_plus(_T97,sizeof(void*),1);{
struct _fat_ptr p3=p2;
int c=_IO_peekc(fp);
_TL7D: _T98=c;_T99=(unsigned char)_T98;_T9A=(int)_T99;_T9B=isspace(_T9A);if(_T9B)goto _TL7F;else{goto _TL7E;}
_TL7E: c=_IO_getc(fp);_T9C=p3;_T9D=
_get_fat_size(_T9C,sizeof(char));if(_T9D!=0U)goto _TL80;goto eof_failure;_TL80: _T9E=p3;_T9F=_T9E.curr;_TA0=(char*)_T9F;_TA1=
_check_null(_TA0);_TA2=c;*_TA1=(char)_TA2;_TA3=& p3;
_fat_ptr_inplace_plus(_TA3,sizeof(char),1);
width=width + -1;_TA4=width;if(_TA4!=0)goto _TL82;goto _TL7F;_TL82:
# 537
 c=_IO_peekc(fp);_TA5=c;_TA6=- 1;if(_TA5!=_TA6)goto _TL84;
seen_eof=seen_eof + 1;goto _TL7F;_TL84: goto _TL7D;_TL7F: _TA7=p3;_TA8=
# 542
_get_fat_size(_TA7,sizeof(char));if(_TA8!=0U)goto _TL86;goto eof_failure;_TL86: _TA9=p3;_TAA=_TA9.curr;_TAB=(char*)_TAA;_TAC=
_check_null(_TAB);*_TAC='\000';_TAD=p3;_TAE=_TAD.curr;_TAF=p2;_TB0=_TAF.curr;_TB1=_TAE - _TB0;_TB2=_TB1 / sizeof(char);
nread=nread + _TB2;
nassigned=nassigned + 1;}}_TL75: goto _TLC;case 3:
# 551
 if(width==0)goto _TL8A;else{goto _TL8B;}_TL8B: _TB3=width;_TB4=sizeof(buf);_TB5=_TB4 - 1U;_TB6=(long long)_TB5;if(_TB3 > _TB6)goto _TL8A;else{goto _TL88;}
_TL8A: _TB7=sizeof(buf);_TB8=_TB7 - 1U;width=(long long)_TB8;goto _TL89;_TL88: _TL89: _TB9=64 | 128;_TBA=_TB9 | 512;
flags=flags | _TBA;_TBB=buf;
p=_tag_fat(_TBB,sizeof(char),351U);_TL8F: if(width!=0)goto _TL8D;else{goto _TL8E;}
_TL8D: _TBC=_IO_peekc(fp);_TBD=(unsigned char)_TBC;c=(int)_TBD;_TBE=c;_TBF=(int)_TBE;switch(_TBF){case 48:
# 575 "scanf.cyc"
 if(base!=0)goto _TL91;
base=8;
flags=flags | 256;goto _TL92;_TL91: _TL92:
# 579
 if(!(flags & 512))goto _TL93;_TC0=64 | 512;_TC1=_TC0 | 128;_TC2=~ _TC1;
flags=flags & _TC2;goto _TL94;
# 582
_TL93: _TC3=64 | 256;_TC4=_TC3 | 128;_TC5=~ _TC4;flags=flags & _TC5;_TL94: goto ok;case 49: goto _LL5A;case 50: _LL5A: goto _LL5C;case 51: _LL5C: goto _LL5E;case 52: _LL5E: goto _LL60;case 53: _LL60: goto _LL62;case 54: _LL62: goto _LL64;case 55: _LL64: _TC6=basefix;_TC7=base;_TC8=_check_known_subscript_notnull(_TC6,17U,sizeof(short),_TC7);_TC9=(short*)_TC8;_TCA=*_TC9;
# 588
base=(int)_TCA;_TCB=64 | 256;_TCC=_TCB | 128;_TCD=~ _TCC;
flags=flags & _TCD;goto ok;case 56: goto _LL68;case 57: _LL68: _TCE=basefix;_TCF=base;_TD0=_check_known_subscript_notnull(_TCE,17U,sizeof(short),_TCF);_TD1=(short*)_TD0;_TD2=*_TD1;
# 594
base=(int)_TD2;
if(base > 8)goto _TL95;goto _LL54;_TL95: _TD3=64 | 256;_TD4=_TD3 | 128;_TD5=~ _TD4;
# 597
flags=flags & _TD5;goto ok;case 65: goto _LL6C;case 66: _LL6C: goto _LL6E;case 67: _LL6E: goto _LL70;case 68: _LL70: goto _LL72;case 69: _LL72: goto _LL74;case 70: _LL74: goto _LL76;case 97: _LL76: goto _LL78;case 98: _LL78: goto _LL7A;case 99: _LL7A: goto _LL7C;case 100: _LL7C: goto _LL7E;case 101: _LL7E: goto _LL80;case 102: _LL80:
# 605
 if(base > 10)goto _TL97;goto _LL54;_TL97: _TD6=64 | 256;_TD7=_TD6 | 128;_TD8=~ _TD7;
# 607
flags=flags & _TD8;goto ok;case 43: goto _LL84;case 45: _LL84:
# 612
 if(!(flags & 64))goto _TL99;_TD9=~ 64;
flags=flags & _TD9;goto ok;_TL99: goto _LL54;case 120: goto _LL88;case 88: _LL88:
# 620
 if(!(flags & 256))goto _TL9B;_TDA=p;_TDB=- 1;_TDC=_fat_ptr_plus(_TDA,sizeof(char),_TDB);_TDD=_TDC.curr;_TDE=(char*)_TDD;_TDF=buf;if(_TDE!=_TDF)goto _TL9B;
base=16;_TE0=~ 256;
flags=flags & _TE0;goto ok;_TL9B: goto _LL54;default: goto _LL54;}_LL54: goto _TL8E;
# 636
ok: _TE1=& p;{struct _fat_ptr _T17F=
# 640
_fat_ptr_inplace_plus_post(_TE1,sizeof(char),1);_TE2=_check_fat_subscript(_T17F,sizeof(char),0U);_TE3=(char*)_TE2;{char _T180=*_TE3;_TE4=c;{char _T181=(char)_TE4;_TE5=_get_fat_size(_T17F,sizeof(char));if(_TE5!=1U)goto _TL9D;if(_T180!=0)goto _TL9D;if(_T181==0)goto _TL9D;_throw_arraybounds();goto _TL9E;_TL9D: _TL9E: _TE6=_T17F.curr;_TE7=(char*)_TE6;*_TE7=_T181;}}}
_IO_getc(fp);_TE8=
_IO_peekc(fp);_TE9=- 1;if(_TE8!=_TE9)goto _TL9F;
seen_eof=seen_eof + 1;goto _TL8E;_TL9F:
# 554 "scanf.cyc"
 width=width + -1;goto _TL8F;_TL8E:
# 653 "scanf.cyc"
 if(!(flags & 128))goto _TLA1;_TEA=p;_TEB=_TEA.curr;_TEC=(char*)_TEB;_TED=buf;
if(_TEC <= _TED)goto _TLA3;_TEE=& p;
_fat_ptr_inplace_plus(_TEE,sizeof(char),-1);_TEF=p;_TF0=_check_fat_subscript(_TEF,sizeof(char),0U);_TF1=(char*)_TF0;_TF2=*_TF1;_TF3=(int)_TF2;_TF4=fp;
_IO_ungetc(_TF3,_TF4);goto _TLA4;_TLA3: _TLA4: goto match_failure;_TLA1: _TF5=p;_TF6=- 1;_TF7=_check_fat_subscript(_TF5,sizeof(char),_TF6);_TF8=(char*)_TF7;_TF9=*_TF8;
# 660
c=(int)_TF9;
if(c==120)goto _TLA7;else{goto _TLA8;}_TLA8: if(c==88)goto _TLA7;else{goto _TLA5;}
_TLA7: _TFA=& p;_fat_ptr_inplace_plus(_TFA,sizeof(char),-1);
_IO_ungetc(c,fp);goto _TLA6;_TLA5: _TLA6: _TFB=flags & 8;
# 665
if(_TFB!=0)goto _TLA9;{
unsigned long res;{struct _fat_ptr _T17F=p;_TFC=_T17F.curr;_TFD=(char*)_TFC;{char _T180=*_TFD;char _T181='\000';_TFE=_get_fat_size(_T17F,sizeof(char));if(_TFE!=1U)goto _TLAB;if(_T180!=0)goto _TLAB;if(_T181==0)goto _TLAB;_throw_arraybounds();goto _TLAC;_TLAB: _TLAC: _TFF=_T17F.curr;_T100=(char*)_TFF;*_T100=_T181;}}_T101=use_strtoul;
# 669
if(!_T101)goto _TLAD;
res=strtoul(buf,0,base);goto _TLAE;
# 672
_TLAD: _T102=strtol(buf,0,base);res=(unsigned long)_T102;_TLAE:
 if(!(flags & 16))goto _TLAF;_T103=ap;_T104=_T103.curr;_T105=(void**)_T104;_T106=*_T105;_T107=
Cyc_va_arg_int_ptr(_T106);_T108=res;*_T107=(int)_T108;goto _TLB0;
_TLAF: if(!(flags & 4))goto _TLB1;_T109=ap;_T10A=_T109.curr;_T10B=(void**)_T10A;_T10C=*_T10B;_T10D=
Cyc_va_arg_short_ptr(_T10C);_T10E=res;*_T10D=(short)_T10E;goto _TLB2;
_TLB1: if(!(flags & 1))goto _TLB3;_T10F=ap;_T110=_T10F.curr;_T111=(void**)_T110;_T112=*_T111;_T113=
Cyc_va_arg_int_ptr(_T112);_T114=res;*_T113=(int)_T114;goto _TLB4;
# 680
_TLB3: _T115=ap;_T116=_T115.curr;_T117=(void**)_T116;_T118=*_T117;_T119=Cyc_va_arg_int_ptr(_T118);_T11A=res;*_T119=(int)_T11A;_TLB4: _TLB2: _TLB0: _T11B=& ap;
_fat_ptr_inplace_plus(_T11B,sizeof(void*),1);
nassigned=nassigned + 1;}goto _TLAA;_TLA9: _TLAA: _T11C=p;_T11D=_T11C.curr;_T11E=buf;_T11F=
# 684
_tag_fat(_T11E,sizeof(char),351U);_T120=_T11F.curr;_T121=_T11D - _T120;_T122=_T121 / sizeof(char);nread=nread + _T122;goto _LL47;case 4:
# 689
 if(width==0)goto _TLB7;else{goto _TLB8;}_TLB8: _T123=width;_T124=sizeof(buf);_T125=_T124 - 1U;_T126=(long long)_T125;if(_T123 > _T126)goto _TLB7;else{goto _TLB5;}
_TLB7: _T127=sizeof(buf);_T128=_T127 - 1U;width=(long long)_T128;goto _TLB6;_TLB5: _TLB6: _T129=64 | 128;_T12A=_T129 | 256;_T12B=_T12A | 512;
flags=flags | _T12B;_T12C=buf;
p=_tag_fat(_T12C,sizeof(char),351U);_TLBC: if(width!=0)goto _TLBA;else{goto _TLBB;}
_TLBA: c=_IO_peekc(fp);_T12D=c;_T12E=(int)_T12D;switch(_T12E){case 48: goto _LL8F;case 49: _LL8F: goto _LL91;case 50: _LL91: goto _LL93;case 51: _LL93: goto _LL95;case 52: _LL95: goto _LL97;case 53: _LL97: goto _LL99;case 54: _LL99: goto _LL9B;case 55: _LL9B: goto _LL9D;case 56: _LL9D: goto _LL9F;case 57: _LL9F: _T12F=64 | 128;_T130=~ _T12F;
# 703
flags=flags & _T130;goto fok;case 43: goto _LLA3;case 45: _LLA3:
# 707
 if(!(flags & 64))goto _TLBE;_T131=~ 64;
flags=flags & _T131;goto fok;_TLBE: goto _LL8B;case 46:
# 713
 if(!(flags & 256))goto _TLC0;_T132=64 | 256;_T133=~ _T132;
flags=flags & _T133;goto fok;_TLC0: goto _LL8B;case 101: goto _LLA9;case 69: _LLA9: _T134=flags;_T135=128 | 512;_T136=_T134 & _T135;
# 720
if(_T136!=512)goto _TLC2;_T137=flags;_T138=512 | 256;_T139=~ _T138;_T13A=_T137 & _T139;_T13B=_T13A | 64;
flags=_T13B | 128;goto fok;_TLC2: goto _LL8B;default: goto _LL8B;}_LL8B: goto _TLBB;
# 731
fok: _T13C=& p;{struct _fat_ptr _T17F=
_fat_ptr_inplace_plus_post(_T13C,sizeof(char),1);_T13D=_check_fat_subscript(_T17F,sizeof(char),0U);_T13E=(char*)_T13D;{char _T180=*_T13E;_T13F=c;{char _T181=(char)_T13F;_T140=_get_fat_size(_T17F,sizeof(char));if(_T140!=1U)goto _TLC4;if(_T180!=0)goto _TLC4;if(_T181==0)goto _TLC4;_throw_arraybounds();goto _TLC5;_TLC4: _TLC5: _T141=_T17F.curr;_T142=(char*)_T141;*_T142=_T181;}}}
_IO_getc(fp);_T143=
_IO_peekc(fp);_T144=- 1;if(_T143!=_T144)goto _TLC6;
seen_eof=seen_eof + 1;goto _TLBB;_TLC6:
# 692
 width=width + -1;goto _TLBC;_TLBB:
# 744
 if(!(flags & 128))goto _TLC8;
if(!(flags & 512))goto _TLCA;
# 747
_TLCC: _T145=p;_T146=_T145.curr;_T147=(char*)_T146;_T148=buf;if(_T147 > _T148)goto _TLCD;else{goto _TLCE;}
_TLCD: _T149=& p;_fat_ptr_inplace_plus(_T149,sizeof(char),-1);_T14A=p;_T14B=_check_fat_subscript(_T14A,sizeof(char),0U);_T14C=(char*)_T14B;_T14D=*_T14C;_T14E=(int)_T14D;_T14F=fp;
_IO_ungetc(_T14E,_T14F);goto _TLCC;_TLCE: goto match_failure;_TLCA: _T150=& p;
# 754
_fat_ptr_inplace_plus(_T150,sizeof(char),-1);_T151=p;_T152=_check_fat_subscript(_T151,sizeof(char),0U);_T153=(char*)_T152;_T154=*_T153;
c=(int)_T154;
if(c==101)goto _TLCF;if(c==69)goto _TLCF;
_IO_ungetc(c,fp);_T155=& p;
_fat_ptr_inplace_plus(_T155,sizeof(char),-1);_T156=p;_T157=_check_fat_subscript(_T156,sizeof(char),0U);_T158=(char*)_T157;_T159=*_T158;
c=(int)_T159;goto _TLD0;_TLCF: _TLD0:
# 761
 _IO_ungetc(c,fp);goto _TLC9;_TLC8: _TLC9: _T15A=flags & 8;
# 763
if(_T15A!=0)goto _TLD1;{
double res;{struct _fat_ptr _T17F=p;_T15B=_check_fat_subscript(_T17F,sizeof(char),0U);_T15C=(char*)_T15B;{char _T180=*_T15C;char _T181='\000';_T15D=_get_fat_size(_T17F,sizeof(char));if(_T15D!=1U)goto _TLD3;if(_T180!=0)goto _TLD3;if(_T181==0)goto _TLD3;_throw_arraybounds();goto _TLD4;_TLD3: _TLD4: _T15E=_T17F.curr;_T15F=(char*)_T15E;*_T15F=_T181;}}_T160=buf;_T161=(const char*)_T160;
# 766
res=atof(_T161);
if(!(flags & 1))goto _TLD5;_T162=ap;_T163=_check_fat_subscript(_T162,sizeof(void*),0U);_T164=(void**)_T163;_T165=*_T164;_T166=
Cyc_va_arg_double_ptr(_T165);*_T166=res;goto _TLD6;
# 770
_TLD5: _T167=ap;_T168=_check_fat_subscript(_T167,sizeof(void*),0U);_T169=(void**)_T168;_T16A=*_T169;_T16B=Cyc_va_arg_float_ptr(_T16A);_T16C=res;*_T16B=(float)_T16C;_TLD6: _T16D=& ap;
_fat_ptr_inplace_plus(_T16D,sizeof(void*),1);
nassigned=nassigned + 1;}goto _TLD2;_TLD1: _TLD2: _T16E=p;_T16F=_T16E.curr;_T170=buf;_T171=
# 774
_tag_fat(_T170,sizeof(char),351U);_T172=_T171.curr;_T173=_T16F - _T172;_T174=_T173 / sizeof(char);nread=nread + _T174;goto _LL47;default:{struct Cyc_Core_Impossible_exn_struct*_T17F=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T17F->tag=Cyc_Core_Impossible;
# 777
_T17F->f1=_tag_fat("scanf3",sizeof(char),7U);_T175=(struct Cyc_Core_Impossible_exn_struct*)_T17F;}_T176=(void*)_T175;_throw(_T176);}_LL47: _TLC: goto _TLF;_TLE:
# 781
 eof_failure:
 seen_eof=seen_eof + 1;
# 784
if(nassigned!=0)goto _TLD7;
nassigned=- 1;goto _TLD8;_TLD7: _TLD8:
 control_failure:
 match_failure: _T177=errp;_T178=(unsigned)_T177;
if(!_T178)goto _TLD9;_T179=errp;
*_T179=*_T179 | 2;goto _TLDA;_TLD9: _TLDA:
 done: _T17A=errp;_T17B=(unsigned)_T17A;
if(!_T17B)goto _TLDB;_T17C=seen_eof;if(!_T17C)goto _TLDB;_T17D=errp;
*_T17D=*_T17D | 1;goto _TLDC;_TLDB: _TLDC: _T17E=nassigned;
return _T17E;}}
# 802
static struct _fat_ptr Cyc___sccl(char*tab,struct _fat_ptr fmt){struct _fat_ptr*_T0;struct _fat_ptr _T1;unsigned char*_T2;const char*_T3;char _T4;struct _fat_ptr*_T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;char _T9;char*_TA;int _TB;char*_TC;char*_TD;int _TE;struct _fat_ptr _TF;int _T10;struct _fat_ptr _T11;char*_T12;int _T13;char*_T14;char*_T15;int _T16;struct _fat_ptr*_T17;struct _fat_ptr _T18;unsigned char*_T19;const char*_T1A;char _T1B;int _T1C;int _T1D;struct _fat_ptr _T1E;int _T1F;struct _fat_ptr _T20;struct _fat_ptr _T21;unsigned char*_T22;const char*_T23;char _T24;struct _fat_ptr*_T25;char*_T26;int _T27;char*_T28;char*_T29;int _T2A;struct _fat_ptr _T2B;
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
static int Cyc_string_getc(struct _fat_ptr*sptr){struct _fat_ptr*_T0;struct _fat_ptr _T1;unsigned char*_T2;char*_T3;struct _fat_ptr _T4;unsigned _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;char _T9;int _TA;int _TB;struct _fat_ptr*_TC;struct _fat_ptr _TD;char _TE;int _TF;
char c;_T0=sptr;{
struct _fat_ptr s=*_T0;_T1=s;_T2=_T1.curr;_T3=(char*)_T2;
if(_T3==0)goto _TLF2;else{goto _TLF4;}_TLF4: _T4=s;_T5=_get_fat_size(_T4,sizeof(char));if(_T5==0U)goto _TLF2;else{goto _TLF3;}_TLF3: _T6=s;_T7=_T6.curr;_T8=(const char*)_T7;c=_T8[0];_T9=c;_TA=(int)_T9;if(_TA==0)goto _TLF2;else{goto _TLF0;}_TLF2: _TB=- 1;return _TB;_TLF0: _TC=sptr;_TD=s;
*_TC=_fat_ptr_plus(_TD,sizeof(char),1);_TE=c;_TF=(int)_TE;
return _TF;}}
# 904
static int Cyc_string_ungetc(int ignore,struct _fat_ptr*sptr){struct _fat_ptr*_T0;struct _fat_ptr*_T1;struct _fat_ptr _T2;int _T3;_T0=sptr;_T1=sptr;_T2=*_T1;_T3=- 1;
*_T0=_fat_ptr_plus(_T2,sizeof(char),_T3);
# 907
return 0;}
# 910
static int Cyc_string_peekc(struct _fat_ptr*sptr){struct _fat_ptr*_T0;struct _fat_ptr _T1;unsigned char*_T2;char*_T3;struct _fat_ptr _T4;unsigned _T5;struct _fat_ptr _T6;unsigned char*_T7;const char*_T8;char _T9;int _TA;int _TB;char _TC;int _TD;
char c;_T0=sptr;{
struct _fat_ptr s=*_T0;_T1=s;_T2=_T1.curr;_T3=(char*)_T2;
if(_T3==0)goto _TLF7;else{goto _TLF9;}_TLF9: _T4=s;_T5=_get_fat_size(_T4,sizeof(char));if(_T5==0U)goto _TLF7;else{goto _TLF8;}_TLF8: _T6=s;_T7=_T6.curr;_T8=(const char*)_T7;c=_T8[0];_T9=c;_TA=(int)_T9;if(_TA==0)goto _TLF7;else{goto _TLF5;}_TLF7: _TB=- 1;return _TB;_TLF5: _TC=c;_TD=(int)_TC;
return _TD;}}
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
