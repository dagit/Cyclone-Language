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
int Cyc__IO_vfscanf(int(*_IO_getc)(void*),int(*_IO_ungetc)(int,void*),int(*_IO_peekc)(void*),void*fp,struct _fat_ptr fmt0,struct _fat_ptr ap,int*errp){char*_T0;unsigned _T1;char*_T2;struct _fat_ptr*_T3;struct _fat_ptr _T4;char*_T5;const char*_T6;char _T7;int _T8;int _T9;int _TA;int _TB;struct _fat_ptr*_TC;struct _fat_ptr _TD;char*_TE;const char*_TF;char _T10;int _T11;int _T12;int _T13;int _T14;int _T15;int _T16;int _T17;int _T18;int _T19;int _T1A;int _T1B;int _T1C;int _T1D;int _T1E;int _T1F;int _T20;int _T21;long long _T22;int _T23;long long _T24;long long _T25;int _T26;struct _fat_ptr _T27;char*_T28;void**_T29;void*_T2A;short*_T2B;int _T2C;struct _fat_ptr _T2D;char*_T2E;void**_T2F;void*_T30;int*_T31;struct _fat_ptr _T32;char*_T33;void**_T34;void*_T35;int*_T36;struct _fat_ptr*_T37;int _T38;int _T39;int _T3A;int _T3B;int _T3C;int _T3D;int _T3E;int _T3F;int _T40;int _T41;int _T42;int _T43;int _T44;long long _T45;struct _fat_ptr _T46;char*_T47;void**_T48;void*_T49;struct _fat_ptr*_T4A;int _T4B;int _T4C;int _T4D;int _T4E;struct _fat_ptr _T4F;char*_T50;char*_T51;int _T52;struct _fat_ptr*_T53;long long _T54;char*_T55;int _T56;char _T57;int _T58;char*_T59;char*_T5A;char _T5B;int _T5C;long long _T5D;int _T5E;int _T5F;struct _fat_ptr _T60;char*_T61;void**_T62;void*_T63;struct _fat_ptr _T64;struct _fat_ptr*_T65;char*_T66;int _T67;char _T68;int _T69;char*_T6A;char*_T6B;char _T6C;int _T6D;struct _fat_ptr _T6E;unsigned _T6F;struct _fat_ptr _T70;unsigned char*_T71;char*_T72;int _T73;struct _fat_ptr*_T74;long long _T75;int _T76;int _T77;struct _fat_ptr _T78;unsigned char*_T79;char*_T7A;struct _fat_ptr _T7B;unsigned char*_T7C;char*_T7D;struct _fat_ptr _T7E;unsigned char*_T7F;struct _fat_ptr _T80;unsigned char*_T81;int _T82;struct _fat_ptr _T83;unsigned _T84;struct _fat_ptr _T85;unsigned char*_T86;char*_T87;int _T88;int _T89;unsigned char _T8A;int _T8B;int _T8C;long long _T8D;int _T8E;int _T8F;int _T90;struct _fat_ptr _T91;char*_T92;void**_T93;void*_T94;struct _fat_ptr*_T95;int _T96;unsigned char _T97;int _T98;int _T99;struct _fat_ptr _T9A;unsigned _T9B;struct _fat_ptr _T9C;unsigned char*_T9D;char*_T9E;int _T9F;struct _fat_ptr*_TA0;long long _TA1;int _TA2;int _TA3;struct _fat_ptr _TA4;unsigned _TA5;struct _fat_ptr _TA6;unsigned char*_TA7;char*_TA8;struct _fat_ptr _TA9;unsigned char*_TAA;struct _fat_ptr _TAB;unsigned char*_TAC;int _TAD;int _TAE;long long _TAF;unsigned _TB0;unsigned _TB1;long long _TB2;unsigned _TB3;unsigned _TB4;int _TB5;int _TB6;char*_TB7;int _TB8;unsigned char _TB9;int _TBA;int _TBB;int _TBC;int _TBD;int _TBE;int _TBF;int _TC0;int _TC1;short*_TC2;int _TC3;char*_TC4;short*_TC5;short _TC6;int _TC7;int _TC8;int _TC9;short*_TCA;int _TCB;char*_TCC;short*_TCD;short _TCE;int _TCF;int _TD0;int _TD1;int _TD2;int _TD3;int _TD4;int _TD5;struct _fat_ptr _TD6;int _TD7;struct _fat_ptr _TD8;unsigned char*_TD9;char*_TDA;char*_TDB;int _TDC;struct _fat_ptr*_TDD;char*_TDE;char*_TDF;int _TE0;unsigned _TE1;unsigned char*_TE2;char*_TE3;int _TE4;int _TE5;struct _fat_ptr _TE6;unsigned char*_TE7;char*_TE8;char*_TE9;struct _fat_ptr*_TEA;struct _fat_ptr _TEB;char*_TEC;char*_TED;char _TEE;int _TEF;void*_TF0;struct _fat_ptr _TF1;int _TF2;char*_TF3;char*_TF4;char _TF5;struct _fat_ptr*_TF6;int _TF7;unsigned char*_TF8;char*_TF9;unsigned _TFA;unsigned char*_TFB;char*_TFC;int _TFD;long _TFE;struct _fat_ptr _TFF;unsigned char*_T100;void**_T101;void*_T102;int*_T103;unsigned long _T104;struct _fat_ptr _T105;unsigned char*_T106;void**_T107;void*_T108;short*_T109;unsigned long _T10A;struct _fat_ptr _T10B;unsigned char*_T10C;void**_T10D;void*_T10E;int*_T10F;unsigned long _T110;struct _fat_ptr _T111;unsigned char*_T112;void**_T113;void*_T114;int*_T115;unsigned long _T116;struct _fat_ptr*_T117;struct _fat_ptr _T118;unsigned char*_T119;char*_T11A;struct _fat_ptr _T11B;unsigned char*_T11C;int _T11D;int _T11E;long long _T11F;unsigned _T120;unsigned _T121;long long _T122;unsigned _T123;unsigned _T124;int _T125;int _T126;int _T127;char*_T128;int _T129;int _T12A;int _T12B;int _T12C;int _T12D;int _T12E;int _T12F;int _T130;int _T131;int _T132;int _T133;int _T134;int _T135;int _T136;int _T137;struct _fat_ptr*_T138;char*_T139;char*_T13A;int _T13B;unsigned _T13C;unsigned char*_T13D;char*_T13E;int _T13F;int _T140;struct _fat_ptr _T141;unsigned char*_T142;char*_T143;char*_T144;struct _fat_ptr*_T145;struct _fat_ptr _T146;char*_T147;char*_T148;char _T149;int _T14A;void*_T14B;struct _fat_ptr*_T14C;struct _fat_ptr _T14D;char*_T14E;char*_T14F;char _T150;struct _fat_ptr*_T151;struct _fat_ptr _T152;char*_T153;char*_T154;char _T155;int _T156;char*_T157;char*_T158;unsigned _T159;unsigned char*_T15A;char*_T15B;char*_T15C;const char*_T15D;struct _fat_ptr _T15E;char*_T15F;void**_T160;void*_T161;double*_T162;struct _fat_ptr _T163;char*_T164;void**_T165;void*_T166;float*_T167;double _T168;struct _fat_ptr*_T169;struct _fat_ptr _T16A;unsigned char*_T16B;char*_T16C;struct _fat_ptr _T16D;unsigned char*_T16E;int _T16F;int _T170;struct Cyc_Core_Impossible_exn_struct*_T171;void*_T172;int*_T173;unsigned _T174;int*_T175;int*_T176;unsigned _T177;int _T178;int*_T179;int _T17A;
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
char buf[351U];{unsigned _T17B=350U;unsigned i;i=0;_TLB: if(i < _T17B)goto _TL9;else{goto _TLA;}_TL9: _T0=buf;_T1=i;_T0[_T1]='0';i=i + 1;goto _TLB;_TLA: _T2=buf;_T2[_T17B]=0;}{
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
_TL64: _T6E=p5;_T6F=_get_fat_size(_T6E,sizeof(char));if(_T6F!=0U)goto _TL66;goto eof_failure;_TL66: _T70=p5;_T71=_T70.curr;_T72=(char*)_T71;_T73=c;
*_T72=(char)_T73;_T74=& p5;
_fat_ptr_inplace_plus(_T74,sizeof(char),1);
_IO_getc(fp);
width=width + -1;_T75=width;if(_T75!=0)goto _TL68;goto _TL65;_TL68:
# 491
 c=_IO_peekc(fp);_T76=c;_T77=- 1;if(_T76!=_T77)goto _TL6A;_T78=p5;_T79=_T78.curr;_T7A=(char*)_T79;_T7B=p0;_T7C=_T7B.curr;_T7D=(char*)_T7C;
if(_T7A!=_T7D)goto _TL6C;goto eof_failure;_TL6C:
# 494
 seen_eof=seen_eof + 1;goto _TL65;_TL6A: goto _TL63;_TL65: _T7E=p5;_T7F=_T7E.curr;_T80=p4;_T81=_T80.curr;_T82=_T7F - _T81;
# 498
n=_T82 / sizeof(char);
if(n!=0)goto _TL6E;goto match_failure;_TL6E: _T83=p5;_T84=
# 501
_get_fat_size(_T83,sizeof(char));if(_T84!=0U)goto _TL70;goto eof_failure;_TL70: _T85=p5;_T86=_T85.curr;_T87=(char*)_T86;
*_T87='\000';
nassigned=nassigned + 1;}}_TL57: _T88=n;
# 505
nread=nread + _T88;goto _LL47;case 2:
# 510
 if(width!=0)goto _TL72;
width=-1;goto _TL73;_TL72: _TL73:
 if(!(flags & 8))goto _TL74;
n=0;{
int c=_IO_peekc(fp);
_TL76: _T89=c;_T8A=(unsigned char)_T89;_T8B=(int)_T8A;_T8C=isspace(_T8B);if(_T8C)goto _TL78;else{goto _TL77;}
_TL77: n=n + 1;
_IO_getc(fp);
width=width + -1;_T8D=width;if(_T8D!=0)goto _TL79;goto _TL78;_TL79:
# 520
 c=_IO_peekc(fp);_T8E=c;_T8F=- 1;if(_T8E!=_T8F)goto _TL7B;
seen_eof=seen_eof + 1;goto _TL78;_TL7B: goto _TL76;_TL78: _T90=n;
# 525
nread=nread + _T90;}goto _TL75;
# 527
_TL74: _T91=ap;_T92=_check_fat_subscript(_T91,sizeof(void*),0U);_T93=(void**)_T92;_T94=*_T93;{struct _fat_ptr p2=Cyc_va_arg_string_ptr(_T94);_T95=& ap;_fat_ptr_inplace_plus(_T95,sizeof(void*),1);{
struct _fat_ptr p3=p2;
int c=_IO_peekc(fp);
_TL7D: _T96=c;_T97=(unsigned char)_T96;_T98=(int)_T97;_T99=isspace(_T98);if(_T99)goto _TL7F;else{goto _TL7E;}
_TL7E: c=_IO_getc(fp);_T9A=p3;_T9B=
_get_fat_size(_T9A,sizeof(char));if(_T9B!=0U)goto _TL80;goto eof_failure;_TL80: _T9C=p3;_T9D=_T9C.curr;_T9E=(char*)_T9D;_T9F=c;
*_T9E=(char)_T9F;_TA0=& p3;
_fat_ptr_inplace_plus(_TA0,sizeof(char),1);
width=width + -1;_TA1=width;if(_TA1!=0)goto _TL82;goto _TL7F;_TL82:
# 537
 c=_IO_peekc(fp);_TA2=c;_TA3=- 1;if(_TA2!=_TA3)goto _TL84;
seen_eof=seen_eof + 1;goto _TL7F;_TL84: goto _TL7D;_TL7F: _TA4=p3;_TA5=
# 542
_get_fat_size(_TA4,sizeof(char));if(_TA5!=0U)goto _TL86;goto eof_failure;_TL86: _TA6=p3;_TA7=_TA6.curr;_TA8=(char*)_TA7;
*_TA8='\000';_TA9=p3;_TAA=_TA9.curr;_TAB=p2;_TAC=_TAB.curr;_TAD=_TAA - _TAC;_TAE=_TAD / sizeof(char);
nread=nread + _TAE;
nassigned=nassigned + 1;}}_TL75: goto _TLC;case 3:
# 551
 if(width==0)goto _TL8A;else{goto _TL8B;}_TL8B: _TAF=width;_TB0=sizeof(buf);_TB1=_TB0 - 1U;_TB2=(long long)_TB1;if(_TAF > _TB2)goto _TL8A;else{goto _TL88;}
_TL8A: _TB3=sizeof(buf);_TB4=_TB3 - 1U;width=(long long)_TB4;goto _TL89;_TL88: _TL89: _TB5=64 | 128;_TB6=_TB5 | 512;
flags=flags | _TB6;_TB7=buf;
p=_tag_fat(_TB7,sizeof(char),351U);_TL8F: if(width!=0)goto _TL8D;else{goto _TL8E;}
_TL8D: _TB8=_IO_peekc(fp);_TB9=(unsigned char)_TB8;c=(int)_TB9;_TBA=c;_TBB=(int)_TBA;switch(_TBB){case 48:
# 575 "scanf.cyc"
 if(base!=0)goto _TL91;
base=8;
flags=flags | 256;goto _TL92;_TL91: _TL92:
# 579
 if(!(flags & 512))goto _TL93;_TBC=64 | 512;_TBD=_TBC | 128;_TBE=~ _TBD;
flags=flags & _TBE;goto _TL94;
# 582
_TL93: _TBF=64 | 256;_TC0=_TBF | 128;_TC1=~ _TC0;flags=flags & _TC1;_TL94: goto ok;case 49: goto _LL5A;case 50: _LL5A: goto _LL5C;case 51: _LL5C: goto _LL5E;case 52: _LL5E: goto _LL60;case 53: _LL60: goto _LL62;case 54: _LL62: goto _LL64;case 55: _LL64: _TC2=basefix;_TC3=base;_TC4=_check_known_subscript_notnull(_TC2,17U,sizeof(short),_TC3);_TC5=(short*)_TC4;_TC6=*_TC5;
# 588
base=(int)_TC6;_TC7=64 | 256;_TC8=_TC7 | 128;_TC9=~ _TC8;
flags=flags & _TC9;goto ok;case 56: goto _LL68;case 57: _LL68: _TCA=basefix;_TCB=base;_TCC=_check_known_subscript_notnull(_TCA,17U,sizeof(short),_TCB);_TCD=(short*)_TCC;_TCE=*_TCD;
# 594
base=(int)_TCE;
if(base > 8)goto _TL95;goto _LL54;_TL95: _TCF=64 | 256;_TD0=_TCF | 128;_TD1=~ _TD0;
# 597
flags=flags & _TD1;goto ok;case 65: goto _LL6C;case 66: _LL6C: goto _LL6E;case 67: _LL6E: goto _LL70;case 68: _LL70: goto _LL72;case 69: _LL72: goto _LL74;case 70: _LL74: goto _LL76;case 97: _LL76: goto _LL78;case 98: _LL78: goto _LL7A;case 99: _LL7A: goto _LL7C;case 100: _LL7C: goto _LL7E;case 101: _LL7E: goto _LL80;case 102: _LL80:
# 605
 if(base > 10)goto _TL97;goto _LL54;_TL97: _TD2=64 | 256;_TD3=_TD2 | 128;_TD4=~ _TD3;
# 607
flags=flags & _TD4;goto ok;case 43: goto _LL84;case 45: _LL84:
# 612
 if(!(flags & 64))goto _TL99;_TD5=~ 64;
flags=flags & _TD5;goto ok;_TL99: goto _LL54;case 120: goto _LL88;case 88: _LL88:
# 620
 if(!(flags & 256))goto _TL9B;_TD6=p;_TD7=- 1;_TD8=_fat_ptr_plus(_TD6,sizeof(char),_TD7);_TD9=_TD8.curr;_TDA=(char*)_TD9;_TDB=buf;if(_TDA!=_TDB)goto _TL9B;
base=16;_TDC=~ 256;
flags=flags & _TDC;goto ok;_TL9B: goto _LL54;default: goto _LL54;}_LL54: goto _TL8E;
# 636
ok: _TDD=& p;{struct _fat_ptr _T17B=
# 640
_fat_ptr_inplace_plus_post(_TDD,sizeof(char),1);_TDE=_check_fat_subscript(_T17B,sizeof(char),0U);_TDF=(char*)_TDE;{char _T17C=*_TDF;_TE0=c;{char _T17D=(char)_TE0;_TE1=_get_fat_size(_T17B,sizeof(char));if(_TE1!=1U)goto _TL9D;if(_T17C!=0)goto _TL9D;if(_T17D==0)goto _TL9D;_throw_arraybounds();goto _TL9E;_TL9D: _TL9E: _TE2=_T17B.curr;_TE3=(char*)_TE2;*_TE3=_T17D;}}}
_IO_getc(fp);_TE4=
_IO_peekc(fp);_TE5=- 1;if(_TE4!=_TE5)goto _TL9F;
seen_eof=seen_eof + 1;goto _TL8E;_TL9F:
# 554 "scanf.cyc"
 width=width + -1;goto _TL8F;_TL8E:
# 653 "scanf.cyc"
 if(!(flags & 128))goto _TLA1;_TE6=p;_TE7=_TE6.curr;_TE8=(char*)_TE7;_TE9=buf;
if(_TE8 <= _TE9)goto _TLA3;_TEA=& p;
_fat_ptr_inplace_plus(_TEA,sizeof(char),-1);_TEB=p;_TEC=_check_fat_subscript(_TEB,sizeof(char),0U);_TED=(char*)_TEC;_TEE=*_TED;_TEF=(int)_TEE;_TF0=fp;
_IO_ungetc(_TEF,_TF0);goto _TLA4;_TLA3: _TLA4: goto match_failure;_TLA1: _TF1=p;_TF2=- 1;_TF3=_check_fat_subscript(_TF1,sizeof(char),_TF2);_TF4=(char*)_TF3;_TF5=*_TF4;
# 660
c=(int)_TF5;
if(c==120)goto _TLA7;else{goto _TLA8;}_TLA8: if(c==88)goto _TLA7;else{goto _TLA5;}
_TLA7: _TF6=& p;_fat_ptr_inplace_plus(_TF6,sizeof(char),-1);
_IO_ungetc(c,fp);goto _TLA6;_TLA5: _TLA6: _TF7=flags & 8;
# 665
if(_TF7!=0)goto _TLA9;{
unsigned long res;{struct _fat_ptr _T17B=p;_TF8=_T17B.curr;_TF9=(char*)_TF8;{char _T17C=*_TF9;char _T17D='\000';_TFA=_get_fat_size(_T17B,sizeof(char));if(_TFA!=1U)goto _TLAB;if(_T17C!=0)goto _TLAB;if(_T17D==0)goto _TLAB;_throw_arraybounds();goto _TLAC;_TLAB: _TLAC: _TFB=_T17B.curr;_TFC=(char*)_TFB;*_TFC=_T17D;}}_TFD=use_strtoul;
# 669
if(!_TFD)goto _TLAD;
res=strtoul(buf,0,base);goto _TLAE;
# 672
_TLAD: _TFE=strtol(buf,0,base);res=(unsigned long)_TFE;_TLAE:
 if(!(flags & 16))goto _TLAF;_TFF=ap;_T100=_TFF.curr;_T101=(void**)_T100;_T102=*_T101;_T103=
Cyc_va_arg_int_ptr(_T102);_T104=res;*_T103=(int)_T104;goto _TLB0;
_TLAF: if(!(flags & 4))goto _TLB1;_T105=ap;_T106=_T105.curr;_T107=(void**)_T106;_T108=*_T107;_T109=
Cyc_va_arg_short_ptr(_T108);_T10A=res;*_T109=(short)_T10A;goto _TLB2;
_TLB1: if(!(flags & 1))goto _TLB3;_T10B=ap;_T10C=_T10B.curr;_T10D=(void**)_T10C;_T10E=*_T10D;_T10F=
Cyc_va_arg_int_ptr(_T10E);_T110=res;*_T10F=(int)_T110;goto _TLB4;
# 680
_TLB3: _T111=ap;_T112=_T111.curr;_T113=(void**)_T112;_T114=*_T113;_T115=Cyc_va_arg_int_ptr(_T114);_T116=res;*_T115=(int)_T116;_TLB4: _TLB2: _TLB0: _T117=& ap;
_fat_ptr_inplace_plus(_T117,sizeof(void*),1);
nassigned=nassigned + 1;}goto _TLAA;_TLA9: _TLAA: _T118=p;_T119=_T118.curr;_T11A=buf;_T11B=
# 684
_tag_fat(_T11A,sizeof(char),351U);_T11C=_T11B.curr;_T11D=_T119 - _T11C;_T11E=_T11D / sizeof(char);nread=nread + _T11E;goto _LL47;case 4:
# 689
 if(width==0)goto _TLB7;else{goto _TLB8;}_TLB8: _T11F=width;_T120=sizeof(buf);_T121=_T120 - 1U;_T122=(long long)_T121;if(_T11F > _T122)goto _TLB7;else{goto _TLB5;}
_TLB7: _T123=sizeof(buf);_T124=_T123 - 1U;width=(long long)_T124;goto _TLB6;_TLB5: _TLB6: _T125=64 | 128;_T126=_T125 | 256;_T127=_T126 | 512;
flags=flags | _T127;_T128=buf;
p=_tag_fat(_T128,sizeof(char),351U);_TLBC: if(width!=0)goto _TLBA;else{goto _TLBB;}
_TLBA: c=_IO_peekc(fp);_T129=c;_T12A=(int)_T129;switch(_T12A){case 48: goto _LL8F;case 49: _LL8F: goto _LL91;case 50: _LL91: goto _LL93;case 51: _LL93: goto _LL95;case 52: _LL95: goto _LL97;case 53: _LL97: goto _LL99;case 54: _LL99: goto _LL9B;case 55: _LL9B: goto _LL9D;case 56: _LL9D: goto _LL9F;case 57: _LL9F: _T12B=64 | 128;_T12C=~ _T12B;
# 703
flags=flags & _T12C;goto fok;case 43: goto _LLA3;case 45: _LLA3:
# 707
 if(!(flags & 64))goto _TLBE;_T12D=~ 64;
flags=flags & _T12D;goto fok;_TLBE: goto _LL8B;case 46:
# 713
 if(!(flags & 256))goto _TLC0;_T12E=64 | 256;_T12F=~ _T12E;
flags=flags & _T12F;goto fok;_TLC0: goto _LL8B;case 101: goto _LLA9;case 69: _LLA9: _T130=flags;_T131=128 | 512;_T132=_T130 & _T131;
# 720
if(_T132!=512)goto _TLC2;_T133=flags;_T134=512 | 256;_T135=~ _T134;_T136=_T133 & _T135;_T137=_T136 | 64;
flags=_T137 | 128;goto fok;_TLC2: goto _LL8B;default: goto _LL8B;}_LL8B: goto _TLBB;
# 731
fok: _T138=& p;{struct _fat_ptr _T17B=
_fat_ptr_inplace_plus_post(_T138,sizeof(char),1);_T139=_check_fat_subscript(_T17B,sizeof(char),0U);_T13A=(char*)_T139;{char _T17C=*_T13A;_T13B=c;{char _T17D=(char)_T13B;_T13C=_get_fat_size(_T17B,sizeof(char));if(_T13C!=1U)goto _TLC4;if(_T17C!=0)goto _TLC4;if(_T17D==0)goto _TLC4;_throw_arraybounds();goto _TLC5;_TLC4: _TLC5: _T13D=_T17B.curr;_T13E=(char*)_T13D;*_T13E=_T17D;}}}
_IO_getc(fp);_T13F=
_IO_peekc(fp);_T140=- 1;if(_T13F!=_T140)goto _TLC6;
seen_eof=seen_eof + 1;goto _TLBB;_TLC6:
# 692
 width=width + -1;goto _TLBC;_TLBB:
# 744
 if(!(flags & 128))goto _TLC8;
if(!(flags & 512))goto _TLCA;
# 747
_TLCC: _T141=p;_T142=_T141.curr;_T143=(char*)_T142;_T144=buf;if(_T143 > _T144)goto _TLCD;else{goto _TLCE;}
_TLCD: _T145=& p;_fat_ptr_inplace_plus(_T145,sizeof(char),-1);_T146=p;_T147=_check_fat_subscript(_T146,sizeof(char),0U);_T148=(char*)_T147;_T149=*_T148;_T14A=(int)_T149;_T14B=fp;
_IO_ungetc(_T14A,_T14B);goto _TLCC;_TLCE: goto match_failure;_TLCA: _T14C=& p;
# 754
_fat_ptr_inplace_plus(_T14C,sizeof(char),-1);_T14D=p;_T14E=_check_fat_subscript(_T14D,sizeof(char),0U);_T14F=(char*)_T14E;_T150=*_T14F;
c=(int)_T150;
if(c==101)goto _TLCF;if(c==69)goto _TLCF;
_IO_ungetc(c,fp);_T151=& p;
_fat_ptr_inplace_plus(_T151,sizeof(char),-1);_T152=p;_T153=_check_fat_subscript(_T152,sizeof(char),0U);_T154=(char*)_T153;_T155=*_T154;
c=(int)_T155;goto _TLD0;_TLCF: _TLD0:
# 761
 _IO_ungetc(c,fp);goto _TLC9;_TLC8: _TLC9: _T156=flags & 8;
# 763
if(_T156!=0)goto _TLD1;{
double res;{struct _fat_ptr _T17B=p;_T157=_check_fat_subscript(_T17B,sizeof(char),0U);_T158=(char*)_T157;{char _T17C=*_T158;char _T17D='\000';_T159=_get_fat_size(_T17B,sizeof(char));if(_T159!=1U)goto _TLD3;if(_T17C!=0)goto _TLD3;if(_T17D==0)goto _TLD3;_throw_arraybounds();goto _TLD4;_TLD3: _TLD4: _T15A=_T17B.curr;_T15B=(char*)_T15A;*_T15B=_T17D;}}_T15C=buf;_T15D=(const char*)_T15C;
# 766
res=atof(_T15D);
if(!(flags & 1))goto _TLD5;_T15E=ap;_T15F=_check_fat_subscript(_T15E,sizeof(void*),0U);_T160=(void**)_T15F;_T161=*_T160;_T162=
Cyc_va_arg_double_ptr(_T161);*_T162=res;goto _TLD6;
# 770
_TLD5: _T163=ap;_T164=_check_fat_subscript(_T163,sizeof(void*),0U);_T165=(void**)_T164;_T166=*_T165;_T167=Cyc_va_arg_float_ptr(_T166);_T168=res;*_T167=(float)_T168;_TLD6: _T169=& ap;
_fat_ptr_inplace_plus(_T169,sizeof(void*),1);
nassigned=nassigned + 1;}goto _TLD2;_TLD1: _TLD2: _T16A=p;_T16B=_T16A.curr;_T16C=buf;_T16D=
# 774
_tag_fat(_T16C,sizeof(char),351U);_T16E=_T16D.curr;_T16F=_T16B - _T16E;_T170=_T16F / sizeof(char);nread=nread + _T170;goto _LL47;default:{struct Cyc_Core_Impossible_exn_struct*_T17B=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T17B->tag=Cyc_Core_Impossible;
# 777
_T17B->f1=_tag_fat("scanf3",sizeof(char),7U);_T171=(struct Cyc_Core_Impossible_exn_struct*)_T17B;}_T172=(void*)_T171;_throw(_T172);}_LL47: _TLC: goto _TLF;_TLE:
# 781
 eof_failure:
 seen_eof=seen_eof + 1;
# 784
if(nassigned!=0)goto _TLD7;
nassigned=- 1;goto _TLD8;_TLD7: _TLD8:
 control_failure:
 match_failure: _T173=errp;_T174=(unsigned)_T173;
if(!_T174)goto _TLD9;_T175=errp;
*_T175=*_T175 | 2;goto _TLDA;_TLD9: _TLDA:
 done: _T176=errp;_T177=(unsigned)_T176;
if(!_T177)goto _TLDB;_T178=seen_eof;if(!_T178)goto _TLDB;_T179=errp;
*_T179=*_T179 | 1;goto _TLDC;_TLDB: _TLDC: _T17A=nassigned;
return _T17A;}}
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
