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
 extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc___cycFILE;extern char Cyc_FileCloseError[15U];struct Cyc_FileCloseError_exn_struct{char*tag;};extern char Cyc_FileOpenError[14U];struct Cyc_FileOpenError_exn_struct{char*tag;struct _fat_ptr f1;};
# 28 "boot_cycstubs.cyc"
extern int open_without_mode(const char*,int);
extern int open_with_mode(const char*,int,unsigned short);
# 31
int Cyc_open(const char*s,int i,struct _fat_ptr ms){struct _fat_ptr _T0;unsigned _T1;const char*_T2;int _T3;struct _fat_ptr _T4;unsigned char*_T5;unsigned short*_T6;unsigned short _T7;int _T8;int _T9;_T0=ms;_T1=
# 34
_get_fat_size(_T0,sizeof(unsigned short));if(_T1 < 1U)goto _TL0;_T2=s;_T3=i;_T4=ms;_T5=_T4.curr;_T6=(unsigned short*)_T5;_T7=_T6[0];_T8=
open_with_mode(_T2,_T3,_T7);return _T8;
# 37
_TL0: _T9=open_without_mode(s,i);return _T9;}struct __abstractFILE;struct Cyc___cycFILE{struct __abstractFILE*file;};
# 49
extern int fclose(struct __abstractFILE*);
# 51
int Cyc_fclose(struct Cyc___cycFILE*f){struct Cyc___cycFILE*_T0;struct __abstractFILE*_T1;int _T2;_T0=f;_T1=_T0->file;_T2=
fclose(_T1);return _T2;}
# 56
extern int feof(struct __abstractFILE*);
# 58
int Cyc_feof(struct Cyc___cycFILE*f){struct Cyc___cycFILE*_T0;struct __abstractFILE*_T1;int _T2;_T0=f;_T1=_T0->file;_T2=
feof(_T1);return _T2;}
# 63
extern int fflush(struct __abstractFILE*);
# 65
int Cyc_fflush(struct Cyc___cycFILE*f){struct Cyc___cycFILE*_T0;unsigned _T1;struct Cyc___cycFILE*_T2;struct __abstractFILE*_T3;int _T4;int _T5;_T0=f;_T1=(unsigned)_T0;
if(!_T1)goto _TL2;_T2=f;_T3=_T2->file;_T4=
fflush(_T3);return _T4;
_TL2: _T5=fflush(0);return _T5;}
# 72
extern int fgetc(struct __abstractFILE*);
# 74
int Cyc_fgetc(struct Cyc___cycFILE*f){struct Cyc___cycFILE*_T0;struct __abstractFILE*_T1;int _T2;_T0=f;_T1=_T0->file;_T2=
fgetc(_T1);return _T2;}
# 79
extern struct __abstractFILE*fopen(const char*,const char*);
# 81
struct Cyc___cycFILE*Cyc_fopen(const char*name,const char*type){struct Cyc___cycFILE*_T0;struct __abstractFILE*_T1;unsigned _T2;struct Cyc___cycFILE*_T3;
struct __abstractFILE*cf=fopen(name,type);_T1=cf;_T2=(unsigned)_T1;
# 84
if(!_T2)goto _TL4;{struct Cyc___cycFILE*_T4=_cycalloc(sizeof(struct Cyc___cycFILE));_T4->file=cf;_T3=(struct Cyc___cycFILE*)_T4;}_T0=_T3;goto _TL5;_TL4: _T0=0;_TL5: return _T0;}
# 88
extern int fputc(int,struct __abstractFILE*);
# 90
int Cyc_fputc(int x,struct Cyc___cycFILE*f){int _T0;struct Cyc___cycFILE*_T1;struct __abstractFILE*_T2;int _T3;_T0=x;_T1=f;_T2=_T1->file;_T3=
fputc(_T0,_T2);return _T3;}
# 95
extern int fputs(const char*,struct __abstractFILE*);
# 97
int Cyc_fputs(const char*x,struct Cyc___cycFILE*f){const char*_T0;struct Cyc___cycFILE*_T1;struct __abstractFILE*_T2;int _T3;_T0=x;_T1=f;_T2=_T1->file;_T3=
fputs(_T0,_T2);return _T3;}
# 102
extern unsigned long fread(char*,unsigned long,unsigned long,struct __abstractFILE*);static char _TmpG0[27U]="fread: buffer insufficient";
# 104
static struct Cyc_Core_Failure_exn_struct Cyc___fread_failure={Cyc_Core_Failure,{(unsigned char*)_TmpG0,(unsigned char*)_TmpG0,(unsigned char*)_TmpG0 + 27U}};
# 106
unsigned long Cyc_fread(struct _fat_ptr ptr,unsigned long size,unsigned long nmemb,struct Cyc___cycFILE*f){unsigned long _T0;struct _fat_ptr _T1;unsigned _T2;struct Cyc_Core_Failure_exn_struct*_T3;struct Cyc_Core_Failure_exn_struct*_T4;struct _fat_ptr _T5;unsigned char*_T6;char*_T7;unsigned long _T8;unsigned long _T9;struct Cyc___cycFILE*_TA;struct __abstractFILE*_TB;unsigned long _TC;_T0=size * nmemb;_T1=ptr;_T2=
_get_fat_size(_T1,sizeof(char));if(_T0 <= _T2)goto _TL6;_T3=& Cyc___fread_failure;_T4=(struct Cyc_Core_Failure_exn_struct*)_T3;_throw(_T4);goto _TL7;_TL6: _TL7: _T5=ptr;_T6=_untag_fat_ptr_check_bound(_T5,sizeof(char),1U);_T7=(char*)_T6;_T8=size;_T9=nmemb;_TA=f;_TB=_TA->file;_TC=
fread(_T7,_T8,_T9,_TB);return _TC;}
# 112
extern unsigned long fwrite(const char*,unsigned long,unsigned long,struct __abstractFILE*);static char _TmpG1[31U]="fwrite called with NULL string";
# 114
static struct Cyc_Core_Failure_exn_struct Cyc___fwrite_failure_1={Cyc_Core_Failure,{(unsigned char*)_TmpG1,(unsigned char*)_TmpG1,(unsigned char*)_TmpG1 + 31U}};static char _TmpG2[28U]="fwrite: buffer insufficient";
# 116
static struct Cyc_Core_Failure_exn_struct Cyc___fwrite_failure_2={Cyc_Core_Failure,{(unsigned char*)_TmpG2,(unsigned char*)_TmpG2,(unsigned char*)_TmpG2 + 28U}};
# 118
unsigned long Cyc_fwrite(struct _fat_ptr ptr,unsigned long size,unsigned long nmemb,struct Cyc___cycFILE*f){struct _fat_ptr _T0;unsigned char*_T1;unsigned _T2;struct Cyc_Core_Failure_exn_struct*_T3;struct Cyc_Core_Failure_exn_struct*_T4;unsigned long _T5;struct _fat_ptr _T6;unsigned _T7;struct Cyc_Core_Failure_exn_struct*_T8;struct Cyc_Core_Failure_exn_struct*_T9;struct _fat_ptr _TA;unsigned char*_TB;const char*_TC;unsigned long _TD;unsigned long _TE;struct Cyc___cycFILE*_TF;struct __abstractFILE*_T10;unsigned long _T11;_T0=ptr;_T1=_T0.curr;_T2=(unsigned)_T1;
if(_T2)goto _TL8;else{goto _TLA;}_TLA: _T3=& Cyc___fwrite_failure_1;_T4=(struct Cyc_Core_Failure_exn_struct*)_T3;_throw(_T4);goto _TL9;
# 121
_TL8: _T5=size * nmemb;_T6=ptr;_T7=_get_fat_size(_T6,sizeof(char));if(_T5 <= _T7)goto _TLB;_T8=& Cyc___fwrite_failure_2;_T9=(struct Cyc_Core_Failure_exn_struct*)_T8;_throw(_T9);goto _TLC;_TLB: _TLC: _TA=ptr;_TB=_untag_fat_ptr_check_bound(_TA,sizeof(char),1U);_TC=(const char*)_TB;_TD=size;_TE=nmemb;_TF=f;_T10=_TF->file;_T11=
fwrite(_TC,_TD,_TE,_T10);return _T11;_TL9:;}
# 127
extern int getc(struct __abstractFILE*);
# 129
int Cyc_getc(struct Cyc___cycFILE*f){struct Cyc___cycFILE*_T0;struct __abstractFILE*_T1;int _T2;_T0=f;_T1=_T0->file;_T2=
getc(_T1);return _T2;}
# 134
extern int putc(int,struct __abstractFILE*);
# 136
int Cyc_putc(int x,struct Cyc___cycFILE*f){int _T0;struct Cyc___cycFILE*_T1;struct __abstractFILE*_T2;int _T3;_T0=x;_T1=f;_T2=_T1->file;_T3=
putc(_T0,_T2);return _T3;}
# 141
extern int ungetc(int,struct __abstractFILE*);
# 143
int Cyc_ungetc(int x,struct Cyc___cycFILE*f){int _T0;struct Cyc___cycFILE*_T1;struct __abstractFILE*_T2;int _T3;_T0=x;_T1=f;_T2=_T1->file;_T3=
ungetc(_T0,_T2);return _T3;}
# 148
extern int getw(struct __abstractFILE*);
# 150
int Cyc_getw(struct Cyc___cycFILE*f){struct Cyc___cycFILE*_T0;struct __abstractFILE*_T1;int _T2;_T0=f;_T1=_T0->file;_T2=
getw(_T1);return _T2;}
# 155
extern int putw(int,struct __abstractFILE*);
# 157
int Cyc_putw(int x,struct Cyc___cycFILE*f){int _T0;struct Cyc___cycFILE*_T1;struct __abstractFILE*_T2;int _T3;_T0=x;_T1=f;_T2=_T1->file;_T3=
putw(_T0,_T2);return _T3;}char Cyc_FileCloseError[15U]="FileCloseError";char Cyc_FileOpenError[14U]="FileOpenError";
# 166
struct Cyc___cycFILE*Cyc_file_open(struct _fat_ptr fname,struct _fat_ptr mode){struct _fat_ptr _T0;unsigned char*_T1;unsigned char*_T2;const char*_T3;struct _fat_ptr _T4;unsigned char*_T5;unsigned char*_T6;const char*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;unsigned _TA;char*_TB;unsigned _TC;unsigned _TD;struct _fat_ptr _TE;unsigned char*_TF;const char*_T10;unsigned _T11;int _T12;struct Cyc_FileOpenError_exn_struct*_T13;void*_T14;struct Cyc___cycFILE*_T15;_T0=fname;_T1=_untag_fat_ptr_check_bound(_T0,sizeof(char),1U);_T2=_check_null(_T1);_T3=(const char*)_T2;_T4=mode;_T5=_untag_fat_ptr_check_bound(_T4,sizeof(char),1U);_T6=_check_null(_T5);_T7=(const char*)_T6;{
struct Cyc___cycFILE*f=Cyc_fopen(_T3,_T7);
if(f!=0)goto _TLD;_T9=fname;_TA=
_get_fat_size(_T9,sizeof(char));{unsigned _T16=_TA + 1U;_TC=_check_times(_T16,sizeof(char));{char*_T17=_cycalloc_atomic(_TC);{unsigned _T18=_T16;unsigned i;i=0;_TL12: if(i < _T18)goto _TL10;else{goto _TL11;}_TL10: _TD=i;_TE=fname;_TF=_TE.curr;_T10=(const char*)_TF;_T11=i;_T12=(int)_T11;_T17[_TD]=_T10[_T12];i=i + 1;goto _TL12;_TL11: _T17[_T18]=0;}_TB=(char*)_T17;}_T8=_tag_fat(_TB,sizeof(char),_T16);}{struct _fat_ptr fn=_T8;{struct Cyc_FileOpenError_exn_struct*_T16=_cycalloc(sizeof(struct Cyc_FileOpenError_exn_struct));_T16->tag=Cyc_FileOpenError;
_T16->f1=fn;_T13=(struct Cyc_FileOpenError_exn_struct*)_T16;}_T14=(void*)_T13;_throw(_T14);}goto _TLE;_TLD: _TLE: _T15=f;
# 172
return _T15;}}
# 175
struct Cyc_FileCloseError_exn_struct Cyc_FileCloseError_val={Cyc_FileCloseError};
void Cyc_file_close(struct Cyc___cycFILE*f){int _T0;struct Cyc_FileCloseError_exn_struct*_T1;struct Cyc_FileCloseError_exn_struct*_T2;_T0=
Cyc_fclose(f);if(_T0==0)goto _TL13;_T1=& Cyc_FileCloseError_val;_T2=(struct Cyc_FileCloseError_exn_struct*)_T1;_throw(_T2);goto _TL14;_TL13: _TL14:;}
# 183
extern char*getcwd(char*,unsigned long);static char _TmpG3[29U]="getcwd: invalid buf argument";
# 185
static struct Cyc_Core_Failure_exn_struct Cyc___getcwd_failure={Cyc_Core_Failure,{(unsigned char*)_TmpG3,(unsigned char*)_TmpG3,(unsigned char*)_TmpG3 + 29U}};
# 187
struct _fat_ptr Cyc_getcwd(struct _fat_ptr buf,unsigned long size){struct _fat_ptr _T0;unsigned _T1;unsigned long _T2;struct Cyc_Core_Failure_exn_struct*_T3;struct Cyc_Core_Failure_exn_struct*_T4;struct _fat_ptr _T5;unsigned char*_T6;char*_T7;unsigned long _T8;struct _fat_ptr _T9;char*_TA;unsigned _TB;_T0=buf;_T1=
_get_fat_size(_T0,sizeof(char));_T2=size;if(_T1 >= _T2)goto _TL15;_T3=& Cyc___getcwd_failure;_T4=(struct Cyc_Core_Failure_exn_struct*)_T3;_throw(_T4);goto _TL16;_TL15: _TL16: _T5=buf;_T6=_untag_fat_ptr_check_bound(_T5,sizeof(char),1U);_T7=(char*)_T6;_T8=size;{
char*response=getcwd(_T7,_T8);_TA=response;_TB=(unsigned)_TA;
if(!_TB)goto _TL17;_T9=buf;goto _TL18;_TL17: _T9=_tag_fat(0,0,0);_TL18: return _T9;}}
# 194
int Cyc_Execinfo_bt (void){
return 1;}
