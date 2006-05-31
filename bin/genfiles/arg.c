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
 struct Cyc___cycFILE;
# 53 "cycboot.h"
extern struct Cyc___cycFILE*Cyc_stderr;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_IntPtr_sa_ScanfArg_struct{int tag;int*f1;};
# 197 "cycboot.h"
extern int Cyc_sscanf(struct _fat_ptr,struct _fat_ptr,struct _fat_ptr);
# 300 "cycboot.h"
extern int isspace(int);extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 49 "string.h"
extern int Cyc_strcmp(struct _fat_ptr,struct _fat_ptr);
# 51
extern int Cyc_strncmp(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 62
extern struct _fat_ptr Cyc_strconcat(struct _fat_ptr,struct _fat_ptr);extern char Cyc_Arg_Bad[4U];struct Cyc_Arg_Bad_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_Arg_Unit_spec_Arg_Spec_struct{int tag;void(*f1)(void);};struct Cyc_Arg_Flag_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_FlagString_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr,struct _fat_ptr);};struct Cyc_Arg_Set_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_Clear_spec_Arg_Spec_struct{int tag;int*f1;};struct Cyc_Arg_String_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};struct Cyc_Arg_Int_spec_Arg_Spec_struct{int tag;void(*f1)(int);};struct Cyc_Arg_Rest_spec_Arg_Spec_struct{int tag;void(*f1)(struct _fat_ptr);};
# 69 "arg.h"
extern int Cyc_Arg_current;struct Cyc_Buffer_t;
# 50 "buffer.h"
extern struct Cyc_Buffer_t*Cyc_Buffer_create(unsigned);
# 58
extern struct _fat_ptr Cyc_Buffer_contents(struct Cyc_Buffer_t*);
# 83
extern void Cyc_Buffer_add_substring(struct Cyc_Buffer_t*,struct _fat_ptr,int,int);
# 92 "buffer.h"
extern void Cyc_Buffer_add_string(struct Cyc_Buffer_t*,struct _fat_ptr);
# 29 "assert.h"
extern void*Cyc___assert_fail(struct _fat_ptr,struct _fat_ptr,unsigned);char Cyc_Arg_Bad[4U]="Bad";char Cyc_Arg_Error[6U]="Error";struct Cyc_Arg_Unknown_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Missing_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Message_Arg_error_struct{int tag;struct _fat_ptr f1;};struct Cyc_Arg_Wrong_Arg_error_struct{int tag;struct _fat_ptr f1;struct _fat_ptr f2;struct _fat_ptr f3;};struct _tuple0{struct _fat_ptr f0;int f1;struct _fat_ptr f2;void*f3;struct _fat_ptr f4;};
# 68 "arg.cyc"
static void*Cyc_Arg_lookup(struct Cyc_List_List*l,struct _fat_ptr x){struct Cyc_List_List*_T0;void*_T1;struct _tuple0*_T2;struct _tuple0 _T3;struct Cyc_List_List*_T4;void*_T5;struct _tuple0*_T6;struct _tuple0 _T7;int _T8;struct _fat_ptr _T9;struct Cyc_List_List*_TA;void*_TB;struct _tuple0*_TC;struct _tuple0 _TD;struct _fat_ptr _TE;unsigned long _TF;int _T10;struct Cyc_List_List*_T11;void*_T12;struct _tuple0*_T13;struct _tuple0 _T14;void*_T15;struct _fat_ptr _T16;struct Cyc_List_List*_T17;void*_T18;struct _tuple0*_T19;struct _tuple0 _T1A;struct _fat_ptr _T1B;int _T1C;struct Cyc_List_List*_T1D;void*_T1E;struct _tuple0*_T1F;struct _tuple0 _T20;void*_T21;struct Cyc_List_List*_T22;struct Cyc_Core_Not_found_exn_struct*_T23;void*_T24;
_TL0: if(l!=0)goto _TL1;else{goto _TL2;}
_TL1: _T0=l;_T1=_T0->hd;_T2=(struct _tuple0*)_T1;_T3=*_T2;{struct _fat_ptr flag=_T3.f0;
unsigned long len=Cyc_strlen(flag);
if(len <= 0U)goto _TL3;_T4=l;_T5=_T4->hd;_T6=(struct _tuple0*)_T5;_T7=*_T6;_T8=_T7.f1;if(!_T8)goto _TL3;_T9=x;_TA=l;_TB=_TA->hd;_TC=(struct _tuple0*)_TB;_TD=*_TC;_TE=_TD.f0;_TF=len;_T10=
Cyc_strncmp(_T9,_TE,_TF);if(_T10!=0)goto _TL5;_T11=l;_T12=_T11->hd;_T13=(struct _tuple0*)_T12;_T14=*_T13;_T15=_T14.f3;
return _T15;_TL5: goto _TL4;
# 76
_TL3: _T16=x;_T17=l;_T18=_T17->hd;_T19=(struct _tuple0*)_T18;_T1A=*_T19;_T1B=_T1A.f0;_T1C=Cyc_strcmp(_T16,_T1B);if(_T1C!=0)goto _TL7;_T1D=l;_T1E=_T1D->hd;_T1F=(struct _tuple0*)_T1E;_T20=*_T1F;_T21=_T20.f3;
return _T21;_TL7: _TL4: _T22=l;
l=_T22->tl;}goto _TL0;_TL2:{struct Cyc_Core_Not_found_exn_struct*_T25=_cycalloc(sizeof(struct Cyc_Core_Not_found_exn_struct));
# 80
_T25->tag=Cyc_Core_Not_found;_T23=(struct Cyc_Core_Not_found_exn_struct*)_T25;}_T24=(void*)_T23;_throw(_T24);}
# 89
static struct _fat_ptr Cyc_Arg_Justify_break_line(struct Cyc_Buffer_t*b,int howmuch,struct _fat_ptr s){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct _fat_ptr _T3;unsigned long _T4;struct _fat_ptr _T5;unsigned _T6;int(*_T7)(struct _fat_ptr,struct _fat_ptr,unsigned);void*(*_T8)(struct _fat_ptr,struct _fat_ptr,unsigned);struct _fat_ptr _T9;struct _fat_ptr _TA;int _TB;unsigned long _TC;unsigned long _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;int _T10;char*_T11;const char*_T12;char _T13;int _T14;int _T15;int _T16;int _T17;int _T18;unsigned long _T19;unsigned long _T1A;struct _fat_ptr _T1B;unsigned char*_T1C;const char*_T1D;int _T1E;char _T1F;int _T20;struct _fat_ptr _T21;unsigned char*_T22;const char*_T23;int _T24;char _T25;int _T26;int _T27;int _T28;unsigned long _T29;unsigned long _T2A;struct _fat_ptr _T2B;unsigned char*_T2C;const char*_T2D;int _T2E;char _T2F;int _T30;struct _fat_ptr _T31;unsigned char*_T32;const char*_T33;int _T34;char _T35;int _T36;int _T37;int _T38;unsigned long _T39;unsigned long _T3A;struct _fat_ptr _T3B;unsigned char*_T3C;const char*_T3D;int _T3E;char _T3F;int _T40;struct _fat_ptr _T41;int _T42;struct _fat_ptr _T43;_T0=s;_T1=_T0.curr;_T2=(char*)_T1;
if(_T2!=0)goto _TL9;_T3=_tag_fat(0,0,0);return _T3;_TL9:
 if(howmuch >= 0)goto _TLB;howmuch=0;goto _TLC;_TLB: _TLC: {
unsigned long len=Cyc_strlen(s);_T4=len;_T5=s;_T6=
_get_fat_size(_T5,sizeof(char));if(_T4 > _T6)goto _TLD;goto _TLE;_TLD: _T8=Cyc___assert_fail;{int(*_T44)(struct _fat_ptr,struct _fat_ptr,unsigned)=(int(*)(struct _fat_ptr,struct _fat_ptr,unsigned))_T8;_T7=_T44;}_T9=_tag_fat("len <= numelts(s)",sizeof(char),18U);_TA=_tag_fat("arg.cyc",sizeof(char),8U);_T7(_T9,_TA,93U);_TLE: _TB=howmuch;_TC=(unsigned long)_TB;_TD=len;
if(_TC <= _TD)goto _TLF;
Cyc_Buffer_add_string(b,s);_TE=
_tag_fat(0,0,0);return _TE;_TLF: {
# 100
int i;
i=howmuch - 1;_TL14: if(i >= 0)goto _TL15;else{goto _TL13;}_TL15: _TF=s;_T10=i;_T11=_check_fat_subscript(_TF,sizeof(char),_T10);_T12=(const char*)_T11;_T13=*_T12;_T14=(int)_T13;_T15=isspace(_T14);if(_T15)goto _TL13;else{goto _TL12;}
_TL12:
# 101
 i=i + -1;goto _TL14;_TL13:
# 105
 if(i >= 0)goto _TL16;_T17=howmuch;
if(!_T17)goto _TL18;_T16=howmuch - 1;goto _TL19;_TL18: _T16=0;_TL19: i=_T16;_TL1D: _T18=i;_T19=(unsigned long)_T18;_T1A=len;if(_T19 < _T1A)goto _TL1F;else{goto _TL1C;}_TL1F: _T1B=s;_T1C=_T1B.curr;_T1D=(const char*)_T1C;_T1E=i;_T1F=_T1D[_T1E];_T20=(int)_T1F;if(_T20)goto _TL1E;else{goto _TL1C;}_TL1E: _T21=s;_T22=_T21.curr;_T23=(const char*)_T22;_T24=i;_T25=_T23[_T24];_T26=(int)_T25;_T27=isspace(_T26);if(_T27)goto _TL1C;else{goto _TL1B;}
_TL1B:
# 106
 i=i + 1;goto _TL1D;_TL1C: goto _TL17;_TL16: _TL17:
# 111
 Cyc_Buffer_add_substring(b,s,0,i);{
# 114
struct _fat_ptr whatsleft=_tag_fat(0,0,0);
# 116
_TL23: _T28=i;_T29=(unsigned long)_T28;_T2A=len;if(_T29 < _T2A)goto _TL25;else{goto _TL22;}_TL25: _T2B=s;_T2C=_T2B.curr;_T2D=(const char*)_T2C;_T2E=i;_T2F=_T2D[_T2E];_T30=(int)_T2F;if(_T30)goto _TL24;else{goto _TL22;}_TL24: _T31=s;_T32=_T31.curr;_T33=(const char*)_T32;_T34=i;_T35=_T33[_T34];_T36=(int)_T35;_T37=isspace(_T36);if(_T37)goto _TL21;else{goto _TL22;}
_TL21:
# 116
 i=i + 1;goto _TL23;_TL22: _T38=i;_T39=(unsigned long)_T38;_T3A=len;
# 118
if(_T39 >= _T3A)goto _TL26;_T3B=s;_T3C=_T3B.curr;_T3D=(const char*)_T3C;_T3E=i;_T3F=_T3D[_T3E];_T40=(int)_T3F;if(!_T40)goto _TL26;_T41=s;_T42=i;whatsleft=_fat_ptr_plus(_T41,sizeof(char),_T42);goto _TL27;_TL26: _TL27: _T43=whatsleft;
return _T43;}}}}
# 127
void Cyc_Arg_Justify_justify_b(struct Cyc_Buffer_t*b,int indent,int margin,struct _fat_ptr item,struct _fat_ptr desc){struct _fat_ptr _T0;unsigned char*_T1;char*_T2;struct _fat_ptr _T3;unsigned char*_T4;char*_T5;struct _fat_ptr _T6;int _T7;unsigned _T8;char*_T9;unsigned _TA;int _TB;unsigned _TC;unsigned _TD;int _TE;unsigned _TF;struct _fat_ptr _T10;unsigned _T11;int _T12;char*_T13;char*_T14;char _T15;unsigned _T16;unsigned char*_T17;char*_T18;unsigned long _T19;unsigned long _T1A;int _T1B;unsigned long _T1C;struct _fat_ptr _T1D;int _T1E;unsigned long _T1F;unsigned long _T20;unsigned long _T21;unsigned long _T22;char*_T23;unsigned _T24;int _T25;unsigned long _T26;unsigned long _T27;unsigned long _T28;unsigned _T29;unsigned _T2A;int _T2B;unsigned long _T2C;unsigned long _T2D;unsigned long _T2E;struct _fat_ptr _T2F;unsigned _T30;int _T31;char*_T32;char*_T33;unsigned _T34;unsigned char*_T35;char*_T36;struct _fat_ptr _T37;unsigned char*_T38;char*_T39;struct Cyc_Buffer_t*_T3A;int _T3B;struct _fat_ptr _T3C;struct _fat_ptr _T3D;unsigned char*_T3E;char*_T3F;struct Cyc_Buffer_t*_T40;struct _fat_ptr _T41;_T0=item;_T1=_T0.curr;_T2=(char*)_T1;
# 129
if(_T2==0)goto _TL28;Cyc_Buffer_add_string(b,item);goto _TL29;_TL28: _TL29: _T3=desc;_T4=_T3.curr;_T5=(char*)_T4;
if(_T5!=0)goto _TL2A;return;_TL2A:
 if(indent >= 0)goto _TL2C;indent=0;goto _TL2D;_TL2C: _TL2D:
 if(margin >= 0)goto _TL2E;margin=0;goto _TL2F;_TL2E: _TL2F: _T7=indent + 2;_T8=(unsigned)_T7;{unsigned _T42=_T8 + 1U;_TA=_check_times(_T42,sizeof(char));{char*_T43=_cycalloc_atomic(_TA);_TB=indent + 2;{unsigned _T44=(unsigned)_TB;unsigned i;i=0;_TL33: if(i < _T44)goto _TL31;else{goto _TL32;}_TL31: _TC=i;
# 135
_T43[_TC]='\000';i=i + 1;goto _TL33;_TL32: _T43[_T44]=0;}_T9=(char*)_T43;}_T6=_tag_fat(_T9,sizeof(char),_T42);}{struct _fat_ptr indentstr=_T6;{
unsigned i=0U;_TL37: _TD=i;_TE=indent + 1;_TF=(unsigned)_TE;if(_TD < _TF)goto _TL35;else{goto _TL36;}
_TL35: _T10=indentstr;_T11=i;_T12=(int)_T11;{struct _fat_ptr _T42=_fat_ptr_plus(_T10,sizeof(char),_T12);_T13=_check_fat_subscript(_T42,sizeof(char),0U);_T14=(char*)_T13;{char _T43=*_T14;if(i!=0U)goto _TL38;_T15='\n';goto _TL39;_TL38: _T15=' ';_TL39: {char _T44=_T15;_T16=_get_fat_size(_T42,sizeof(char));if(_T16!=1U)goto _TL3A;if(_T43!=0)goto _TL3A;if(_T44==0)goto _TL3A;_throw_arraybounds();goto _TL3B;_TL3A: _TL3B: _T17=_T42.curr;_T18=(char*)_T17;*_T18=_T44;}}}
# 136
i=i + 1;goto _TL37;_TL36:;}{
# 138
unsigned long itemlen=Cyc_strlen(item);
struct _fat_ptr itemsep;_T19=
Cyc_strlen(desc);if(_T19 <= 0U)goto _TL3C;_T1A=itemlen + 1U;_T1B=indent;_T1C=(unsigned long)_T1B;
if(_T1A <= _T1C)goto _TL3E;
itemsep=indentstr;goto _TL3F;
# 144
_TL3E: _T1E=indent;_T1F=(unsigned long)_T1E;_T20=itemlen;_T21=_T1F - _T20;_T22=_T21 + 1U;{unsigned _T42=_T22 + 1U;_T24=_check_times(_T42,sizeof(char));{char*_T43=_cycalloc_atomic(_T24);_T25=indent;_T26=(unsigned long)_T25;_T27=itemlen;_T28=_T26 - _T27;{unsigned _T44=_T28 + 1U;unsigned i;i=0;_TL43: if(i < _T44)goto _TL41;else{goto _TL42;}_TL41: _T29=i;_T43[_T29]='\000';i=i + 1;goto _TL43;_TL42: _T43[_T44]=0;}_T23=(char*)_T43;}_T1D=_tag_fat(_T23,sizeof(char),_T42);}{struct _fat_ptr temp=_T1D;{
unsigned i=0U;_TL47: _T2A=i;_T2B=indent;_T2C=(unsigned long)_T2B;_T2D=itemlen;_T2E=_T2C - _T2D;if(_T2A < _T2E)goto _TL45;else{goto _TL46;}_TL45: _T2F=temp;_T30=i;_T31=(int)_T30;{struct _fat_ptr _T42=_fat_ptr_plus(_T2F,sizeof(char),_T31);_T32=_check_fat_subscript(_T42,sizeof(char),0U);_T33=(char*)_T32;{char _T43=*_T33;char _T44=' ';_T34=_get_fat_size(_T42,sizeof(char));if(_T34!=1U)goto _TL48;if(_T43!=0)goto _TL48;if(_T44==0)goto _TL48;_throw_arraybounds();goto _TL49;_TL48: _TL49: _T35=_T42.curr;_T36=(char*)_T35;*_T36=_T44;}}i=i + 1;goto _TL47;_TL46:;}
itemsep=temp;}_TL3F: goto _TL3D;
# 149
_TL3C: return;_TL3D:
# 151
 Cyc_Buffer_add_string(b,itemsep);
# 153
_TL4A: _T37=desc;_T38=_T37.curr;_T39=(char*)_T38;if(_T39!=0)goto _TL4B;else{goto _TL4C;}
_TL4B: _T3A=b;_T3B=margin - indent;_T3C=desc;desc=Cyc_Arg_Justify_break_line(_T3A,_T3B,_T3C);_T3D=desc;_T3E=_T3D.curr;_T3F=(char*)_T3E;
if(_T3F==0)goto _TL4D;
Cyc_Buffer_add_string(b,indentstr);goto _TL4E;
# 158
_TL4D: _T40=b;_T41=_tag_fat("\n",sizeof(char),2U);Cyc_Buffer_add_string(_T40,_T41);_TL4E: goto _TL4A;_TL4C:
# 160
 return;}}}
# 164
void Cyc_Arg_usage(struct Cyc_List_List*speclist,struct _fat_ptr errmsg){struct Cyc_String_pa_PrintArg_struct _T0;struct Cyc___cycFILE*_T1;struct _fat_ptr _T2;struct _fat_ptr _T3;struct Cyc_Buffer_t*_T4;struct Cyc_List_List*_T5;void*_T6;struct _tuple0*_T7;struct _tuple0 _T8;struct _fat_ptr _T9;struct Cyc_List_List*_TA;void*_TB;struct _tuple0*_TC;struct _tuple0 _TD;struct _fat_ptr _TE;struct _fat_ptr _TF;struct Cyc_List_List*_T10;void*_T11;struct _tuple0*_T12;struct _tuple0 _T13;struct _fat_ptr _T14;struct Cyc_List_List*_T15;struct Cyc_String_pa_PrintArg_struct _T16;struct Cyc___cycFILE*_T17;struct _fat_ptr _T18;struct _fat_ptr _T19;{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;
# 166
_T1A.f1=errmsg;_T0=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_T0;void*_T1B[1];_T1B[0]=& _T1A;_T1=Cyc_stderr;_T2=_tag_fat("%s\n",sizeof(char),4U);_T3=_tag_fat(_T1B,sizeof(void*),1);Cyc_fprintf(_T1,_T2,_T3);}{
struct Cyc_Buffer_t*b=Cyc_Buffer_create(1024U);
_TL4F: if(speclist!=0)goto _TL50;else{goto _TL51;}
_TL50: _T4=b;_T5=speclist;_T6=_T5->hd;_T7=(struct _tuple0*)_T6;_T8=*_T7;_T9=_T8.f0;_TA=speclist;_TB=_TA->hd;_TC=(struct _tuple0*)_TB;_TD=*_TC;_TE=_TD.f2;_TF=
Cyc_strconcat(_T9,_TE);_T10=speclist;_T11=_T10->hd;_T12=(struct _tuple0*)_T11;_T13=*_T12;_T14=_T13.f4;
# 169
Cyc_Arg_Justify_justify_b(_T4,12,72,_TF,_T14);_T15=speclist;
# 172
speclist=_T15->tl;goto _TL4F;_TL51:{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;
# 174
_T1A.f1=Cyc_Buffer_contents(b);_T16=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_T16;void*_T1B[1];_T1B[0]=& _T1A;_T17=Cyc_stderr;_T18=_tag_fat("%s",sizeof(char),3U);_T19=_tag_fat(_T1B,sizeof(void*),1);Cyc_fprintf(_T17,_T18,_T19);}}}
# 177
int Cyc_Arg_current=0;
static struct _fat_ptr Cyc_Arg_args={(void*)0,(void*)0,(void*)(0 + 0)};
# 180
static void Cyc_Arg_stop(int prog_pos,void*e,struct Cyc_List_List*speclist,struct _fat_ptr errmsg){struct _fat_ptr _T0;int _T1;unsigned _T2;struct _fat_ptr _T3;unsigned _T4;struct _fat_ptr _T5;unsigned char*_T6;struct _fat_ptr*_T7;int _T8;void*_T9;int*_TA;unsigned _TB;void*_TC;struct _fat_ptr _TD;struct _fat_ptr _TE;int _TF;struct Cyc_String_pa_PrintArg_struct _T10;struct Cyc_String_pa_PrintArg_struct _T11;struct Cyc___cycFILE*_T12;struct _fat_ptr _T13;struct _fat_ptr _T14;void*_T15;struct Cyc_String_pa_PrintArg_struct _T16;struct Cyc_String_pa_PrintArg_struct _T17;struct Cyc___cycFILE*_T18;struct _fat_ptr _T19;struct _fat_ptr _T1A;void*_T1B;struct Cyc_String_pa_PrintArg_struct _T1C;struct Cyc_String_pa_PrintArg_struct _T1D;struct Cyc_String_pa_PrintArg_struct _T1E;struct Cyc_String_pa_PrintArg_struct _T1F;struct Cyc___cycFILE*_T20;struct _fat_ptr _T21;struct _fat_ptr _T22;void*_T23;struct Cyc_String_pa_PrintArg_struct _T24;struct Cyc_String_pa_PrintArg_struct _T25;struct Cyc___cycFILE*_T26;struct _fat_ptr _T27;struct _fat_ptr _T28;struct _fat_ptr _T29;unsigned _T2A;_T1=prog_pos;_T2=(unsigned)_T1;_T3=Cyc_Arg_args;_T4=
# 183
_get_fat_size(_T3,sizeof(struct _fat_ptr));if(_T2 >= _T4)goto _TL52;_T5=Cyc_Arg_args;_T6=_T5.curr;_T7=(struct _fat_ptr*)_T6;_T8=prog_pos;_T0=_T7[_T8];goto _TL53;_TL52: _T0=_tag_fat("(?)",sizeof(char),4U);_TL53: {struct _fat_ptr progname=_T0;{struct _fat_ptr _T2B;struct _fat_ptr _T2C;struct _fat_ptr _T2D;_T9=e;_TA=(int*)_T9;_TB=*_TA;switch(_TB){case 0: _TC=e;{struct Cyc_Arg_Unknown_Arg_error_struct*_T2E=(struct Cyc_Arg_Unknown_Arg_error_struct*)_TC;_T2D=_T2E->f1;}{struct _fat_ptr s=_T2D;_TD=s;_TE=
# 186
_tag_fat("-help",sizeof(char),6U);_TF=Cyc_strcmp(_TD,_TE);if(_TF==0)goto _TL55;{struct Cyc_String_pa_PrintArg_struct _T2E;_T2E.tag=0;
_T2E.f1=progname;_T10=_T2E;}{struct Cyc_String_pa_PrintArg_struct _T2E=_T10;{struct Cyc_String_pa_PrintArg_struct _T2F;_T2F.tag=0;_T2F.f1=s;_T11=_T2F;}{struct Cyc_String_pa_PrintArg_struct _T2F=_T11;void*_T30[2];_T30[0]=& _T2E;_T30[1]=& _T2F;_T12=Cyc_stderr;_T13=_tag_fat("%s: unknown option `%s'.\n",sizeof(char),26U);_T14=_tag_fat(_T30,sizeof(void*),2);Cyc_fprintf(_T12,_T13,_T14);}}goto _TL56;_TL55: _TL56: goto _LL0;}case 1: _T15=e;{struct Cyc_Arg_Missing_Arg_error_struct*_T2E=(struct Cyc_Arg_Missing_Arg_error_struct*)_T15;_T2D=_T2E->f1;}{struct _fat_ptr s=_T2D;{struct Cyc_String_pa_PrintArg_struct _T2E;_T2E.tag=0;
# 190
_T2E.f1=progname;_T16=_T2E;}{struct Cyc_String_pa_PrintArg_struct _T2E=_T16;{struct Cyc_String_pa_PrintArg_struct _T2F;_T2F.tag=0;_T2F.f1=s;_T17=_T2F;}{struct Cyc_String_pa_PrintArg_struct _T2F=_T17;void*_T30[2];_T30[0]=& _T2E;_T30[1]=& _T2F;_T18=Cyc_stderr;_T19=_tag_fat("%s: option `%s' needs an argument.\n",sizeof(char),36U);_T1A=_tag_fat(_T30,sizeof(void*),2);Cyc_fprintf(_T18,_T19,_T1A);}}goto _LL0;}case 3: _T1B=e;{struct Cyc_Arg_Wrong_Arg_error_struct*_T2E=(struct Cyc_Arg_Wrong_Arg_error_struct*)_T1B;_T2D=_T2E->f1;_T2C=_T2E->f2;_T2B=_T2E->f3;}{struct _fat_ptr flag=_T2D;struct _fat_ptr cmd=_T2C;struct _fat_ptr t=_T2B;{struct Cyc_String_pa_PrintArg_struct _T2E;_T2E.tag=0;
# 194
_T2E.f1=progname;_T1C=_T2E;}{struct Cyc_String_pa_PrintArg_struct _T2E=_T1C;{struct Cyc_String_pa_PrintArg_struct _T2F;_T2F.tag=0;_T2F.f1=cmd;_T1D=_T2F;}{struct Cyc_String_pa_PrintArg_struct _T2F=_T1D;{struct Cyc_String_pa_PrintArg_struct _T30;_T30.tag=0;_T30.f1=flag;_T1E=_T30;}{struct Cyc_String_pa_PrintArg_struct _T30=_T1E;{struct Cyc_String_pa_PrintArg_struct _T31;_T31.tag=0;_T31.f1=t;_T1F=_T31;}{struct Cyc_String_pa_PrintArg_struct _T31=_T1F;void*_T32[4];_T32[0]=& _T2E;_T32[1]=& _T2F;_T32[2]=& _T30;_T32[3]=& _T31;_T20=Cyc_stderr;_T21=
# 193
_tag_fat("%s: wrong argument `%s'; option `%s' expects %s.\n",sizeof(char),50U);_T22=_tag_fat(_T32,sizeof(void*),4);Cyc_fprintf(_T20,_T21,_T22);}}}}goto _LL0;}default: _T23=e;{struct Cyc_Arg_Message_Arg_error_struct*_T2E=(struct Cyc_Arg_Message_Arg_error_struct*)_T23;_T2D=_T2E->f1;}{struct _fat_ptr s=_T2D;{struct Cyc_String_pa_PrintArg_struct _T2E;_T2E.tag=0;
# 197
_T2E.f1=progname;_T24=_T2E;}{struct Cyc_String_pa_PrintArg_struct _T2E=_T24;{struct Cyc_String_pa_PrintArg_struct _T2F;_T2F.tag=0;_T2F.f1=s;_T25=_T2F;}{struct Cyc_String_pa_PrintArg_struct _T2F=_T25;void*_T30[2];_T30[0]=& _T2E;_T30[1]=& _T2F;_T26=Cyc_stderr;_T27=_tag_fat("%s: %s.\n",sizeof(char),9U);_T28=_tag_fat(_T30,sizeof(void*),2);Cyc_fprintf(_T26,_T27,_T28);}}goto _LL0;}}_LL0:;}
# 200
Cyc_Arg_usage(speclist,errmsg);_T29=Cyc_Arg_args;_T2A=
_get_fat_size(_T29,sizeof(struct _fat_ptr));Cyc_Arg_current=(int)_T2A;}}
# 204
void Cyc_Arg_parse(struct Cyc_List_List*speclist,void(*anonfun)(struct _fat_ptr),int(*anonflagfun)(struct _fat_ptr),struct _fat_ptr errmsg,struct _fat_ptr orig_args){struct _fat_ptr _T0;struct _fat_ptr _T1;unsigned _T2;int _T3;char*_T4;struct _fat_ptr*_T5;unsigned char*_T6;char*_T7;int _T8;unsigned _T9;unsigned _TA;struct _fat_ptr _TB;int _TC;char*_TD;struct _fat_ptr*_TE;struct _fat_ptr _TF;unsigned char*_T10;char*_T11;struct _fat_ptr _T12;unsigned _T13;struct _fat_ptr _T14;unsigned char*_T15;const char*_T16;char _T17;int _T18;struct _handler_cons*_T19;int*_T1A;int _T1B;void*_T1C;struct Cyc_Core_Not_found_exn_struct*_T1D;char*_T1E;char*_T1F;int _T20;int _T21;struct Cyc_Arg_Unknown_Arg_error_struct*_T22;void*_T23;struct Cyc_List_List*_T24;struct _fat_ptr _T25;struct _handler_cons*_T26;int*_T27;int _T28;void*_T29;int*_T2A;unsigned _T2B;void*_T2C;void*_T2D;void*_T2E;int*_T2F;void*_T30;int*_T31;void*_T32;int _T33;unsigned _T34;unsigned _T35;struct _fat_ptr _T36;struct _fat_ptr _T37;int _T38;char*_T39;struct _fat_ptr*_T3A;struct _fat_ptr _T3B;int _T3C;struct Cyc_Arg_Missing_Arg_error_struct*_T3D;void*_T3E;struct Cyc_List_List*_T3F;struct _fat_ptr _T40;void*_T41;int _T42;unsigned _T43;unsigned _T44;struct _fat_ptr _T45;int _T46;char*_T47;struct _fat_ptr*_T48;struct _fat_ptr _T49;int _T4A;struct Cyc_Arg_Missing_Arg_error_struct*_T4B;void*_T4C;struct Cyc_List_List*_T4D;struct _fat_ptr _T4E;void*_T4F;struct _fat_ptr _T50;int _T51;char*_T52;struct _fat_ptr*_T53;int _T54;struct Cyc_IntPtr_sa_ScanfArg_struct _T55;struct _fat_ptr _T56;struct _fat_ptr _T57;struct _fat_ptr _T58;int _T59;struct Cyc_Arg_Wrong_Arg_error_struct*_T5A;void*_T5B;struct Cyc_List_List*_T5C;struct _fat_ptr _T5D;void*_T5E;int _T5F;unsigned _T60;unsigned _T61;struct _fat_ptr _T62;int _T63;char*_T64;struct _fat_ptr*_T65;struct _fat_ptr _T66;void*_T67;struct Cyc_Arg_Bad_exn_struct*_T68;char*_T69;char*_T6A;int _T6B;struct Cyc_Arg_Message_Arg_error_struct*_T6C;void*_T6D;struct Cyc_List_List*_T6E;struct _fat_ptr _T6F;struct _handler_cons*_T70;int*_T71;int _T72;void*_T73;struct Cyc_Arg_Bad_exn_struct*_T74;char*_T75;char*_T76;int _T77;struct Cyc_Arg_Message_Arg_error_struct*_T78;void*_T79;struct Cyc_List_List*_T7A;struct _fat_ptr _T7B;
# 209
Cyc_Arg_args=orig_args;{
# 211
int initpos=Cyc_Arg_current;_T0=Cyc_Arg_args;{
unsigned l=_get_fat_size(_T0,sizeof(struct _fat_ptr));_T1=Cyc_Arg_args;_T2=l - 1U;_T3=(int)_T2;_T4=_check_fat_subscript(_T1,sizeof(struct _fat_ptr),_T3);_T5=(struct _fat_ptr*)_T4;_T6=_T5->curr;_T7=(char*)_T6;
# 214
if(_T7!=0)goto _TL57;l=l - 1U;goto _TL58;_TL57: _TL58:
 Cyc_Arg_current=Cyc_Arg_current + 1;
_TL59: _T8=Cyc_Arg_current;_T9=(unsigned)_T8;_TA=l;if(_T9 < _TA)goto _TL5A;else{goto _TL5B;}
_TL5A: _TB=Cyc_Arg_args;_TC=Cyc_Arg_current;_TD=_check_fat_subscript(_TB,sizeof(struct _fat_ptr),_TC);_TE=(struct _fat_ptr*)_TD;{struct _fat_ptr s=*_TE;_TF=s;_T10=_TF.curr;_T11=(char*)_T10;
if(_T11==0)goto _TL5C;_T12=s;_T13=_get_fat_size(_T12,sizeof(char));if(_T13 < 1U)goto _TL5C;_T14=s;_T15=_T14.curr;_T16=(const char*)_T15;_T17=_T16[0];_T18=(int)_T17;if(_T18!=45)goto _TL5C;{
void*action;{struct _handler_cons _T7C;_T19=& _T7C;_push_handler(_T19);{int _T7D=0;_T1A=_T7C.handler;_T1B=setjmp(_T1A);if(!_T1B)goto _TL5E;_T7D=1;goto _TL5F;_TL5E: _TL5F: if(_T7D)goto _TL60;else{goto _TL62;}_TL62:
 action=Cyc_Arg_lookup(speclist,s);_pop_handler();goto _TL61;_TL60: _T1C=Cyc_Core_get_exn_thrown();{void*_T7E=(void*)_T1C;void*_T7F;_T1D=(struct Cyc_Core_Not_found_exn_struct*)_T7E;_T1E=_T1D->tag;_T1F=Cyc_Core_Not_found;if(_T1E!=_T1F)goto _TL63;_T20=
# 224
anonflagfun(s);if(_T20)goto _TL65;else{goto _TL67;}
_TL67: _T21=initpos;{struct Cyc_Arg_Unknown_Arg_error_struct*_T80=_cycalloc(sizeof(struct Cyc_Arg_Unknown_Arg_error_struct));_T80->tag=0;_T80->f1=s;_T22=(struct Cyc_Arg_Unknown_Arg_error_struct*)_T80;}_T23=(void*)_T22;_T24=speclist;_T25=errmsg;Cyc_Arg_stop(_T21,_T23,_T24,_T25);
return;
# 229
_TL65: Cyc_Arg_current=Cyc_Arg_current + 1;goto _TL59;_TL63: _T7F=_T7E;{void*exn=_T7F;_rethrow(exn);};}_TL61:;}}{struct _handler_cons _T7C;_T26=& _T7C;_push_handler(_T26);{int _T7D=0;_T27=_T7C.handler;_T28=setjmp(_T27);if(!_T28)goto _TL68;_T7D=1;goto _TL69;_TL68: _TL69: if(_T7D)goto _TL6A;else{goto _TL6C;}_TL6C:{void(*_T7E)(int);void(*_T7F)(struct _fat_ptr,struct _fat_ptr);int*_T80;void(*_T81)(struct _fat_ptr);void(*_T82)(void);_T29=action;_T2A=(int*)_T29;_T2B=*_T2A;switch(_T2B){case 0: _T2C=action;{struct Cyc_Arg_Unit_spec_Arg_Spec_struct*_T83=(struct Cyc_Arg_Unit_spec_Arg_Spec_struct*)_T2C;_T82=_T83->f1;}{void(*f)(void)=_T82;
# 235
f();goto _LL5;}case 1: _T2D=action;{struct Cyc_Arg_Flag_spec_Arg_Spec_struct*_T83=(struct Cyc_Arg_Flag_spec_Arg_Spec_struct*)_T2D;_T81=_T83->f1;}{void(*f)(struct _fat_ptr)=_T81;
f(s);goto _LL5;}case 3: _T2E=action;{struct Cyc_Arg_Set_spec_Arg_Spec_struct*_T83=(struct Cyc_Arg_Set_spec_Arg_Spec_struct*)_T2E;_T80=_T83->f1;}{int*r=_T80;_T2F=r;
*_T2F=1;goto _LL5;}case 4: _T30=action;{struct Cyc_Arg_Clear_spec_Arg_Spec_struct*_T83=(struct Cyc_Arg_Clear_spec_Arg_Spec_struct*)_T30;_T80=_T83->f1;}{int*r=_T80;_T31=r;
*_T31=0;goto _LL5;}case 2: _T32=action;{struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*_T83=(struct Cyc_Arg_FlagString_spec_Arg_Spec_struct*)_T32;_T7F=_T83->f1;}{void(*f)(struct _fat_ptr,struct _fat_ptr)=_T7F;_T33=Cyc_Arg_current + 1;_T34=(unsigned)_T33;_T35=l;
# 240
if(_T34 >= _T35)goto _TL6E;_T36=s;_T37=Cyc_Arg_args;_T38=Cyc_Arg_current + 1;_T39=_check_fat_subscript(_T37,sizeof(struct _fat_ptr),_T38);_T3A=(struct _fat_ptr*)_T39;_T3B=*_T3A;
f(_T36,_T3B);
Cyc_Arg_current=Cyc_Arg_current + 1;goto _TL6F;
# 244
_TL6E: _T3C=initpos;{struct Cyc_Arg_Missing_Arg_error_struct*_T83=_cycalloc(sizeof(struct Cyc_Arg_Missing_Arg_error_struct));_T83->tag=1;_T83->f1=s;_T3D=(struct Cyc_Arg_Missing_Arg_error_struct*)_T83;}_T3E=(void*)_T3D;_T3F=speclist;_T40=errmsg;Cyc_Arg_stop(_T3C,_T3E,_T3F,_T40);_TL6F: goto _LL5;}case 5: _T41=action;{struct Cyc_Arg_String_spec_Arg_Spec_struct*_T83=(struct Cyc_Arg_String_spec_Arg_Spec_struct*)_T41;_T81=_T83->f1;}{void(*f)(struct _fat_ptr)=_T81;_T42=Cyc_Arg_current + 1;_T43=(unsigned)_T42;_T44=l;
# 247
if(_T43 >= _T44)goto _TL70;_T45=Cyc_Arg_args;_T46=Cyc_Arg_current + 1;_T47=_check_fat_subscript(_T45,sizeof(struct _fat_ptr),_T46);_T48=(struct _fat_ptr*)_T47;_T49=*_T48;
f(_T49);
Cyc_Arg_current=Cyc_Arg_current + 1;goto _TL71;
# 251
_TL70: _T4A=initpos;{struct Cyc_Arg_Missing_Arg_error_struct*_T83=_cycalloc(sizeof(struct Cyc_Arg_Missing_Arg_error_struct));_T83->tag=1;_T83->f1=s;_T4B=(struct Cyc_Arg_Missing_Arg_error_struct*)_T83;}_T4C=(void*)_T4B;_T4D=speclist;_T4E=errmsg;Cyc_Arg_stop(_T4A,_T4C,_T4D,_T4E);_TL71: goto _LL5;}case 6: _T4F=action;{struct Cyc_Arg_Int_spec_Arg_Spec_struct*_T83=(struct Cyc_Arg_Int_spec_Arg_Spec_struct*)_T4F;_T7E=_T83->f1;}{void(*f)(int)=_T7E;_T50=Cyc_Arg_args;_T51=Cyc_Arg_current + 1;_T52=_check_fat_subscript(_T50,sizeof(struct _fat_ptr),_T51);_T53=(struct _fat_ptr*)_T52;{
# 254
struct _fat_ptr arg=*_T53;
int n=0;{struct Cyc_IntPtr_sa_ScanfArg_struct _T83;_T83.tag=2;
_T83.f1=& n;_T55=_T83;}{struct Cyc_IntPtr_sa_ScanfArg_struct _T83=_T55;void*_T84[1];_T84[0]=& _T83;_T56=arg;_T57=_tag_fat("%d",sizeof(char),3U);_T58=_tag_fat(_T84,sizeof(void*),1);_T54=Cyc_sscanf(_T56,_T57,_T58);}if(_T54==1)goto _TL72;_T59=initpos;{struct Cyc_Arg_Wrong_Arg_error_struct*_T83=_cycalloc(sizeof(struct Cyc_Arg_Wrong_Arg_error_struct));_T83->tag=3;
_T83->f1=s;_T83->f2=arg;_T83->f3=_tag_fat("an integer",sizeof(char),11U);_T5A=(struct Cyc_Arg_Wrong_Arg_error_struct*)_T83;}_T5B=(void*)_T5A;_T5C=speclist;_T5D=errmsg;Cyc_Arg_stop(_T59,_T5B,_T5C,_T5D);_npop_handler(0);return;_TL72:
# 260
 f(n);
Cyc_Arg_current=Cyc_Arg_current + 1;goto _LL5;}}default: _T5E=action;{struct Cyc_Arg_Rest_spec_Arg_Spec_struct*_T83=(struct Cyc_Arg_Rest_spec_Arg_Spec_struct*)_T5E;_T81=_T83->f1;}{void(*f)(struct _fat_ptr)=_T81;
# 264
_TL74: _T5F=Cyc_Arg_current;_T60=(unsigned)_T5F;_T61=l - 1U;if(_T60 < _T61)goto _TL75;else{goto _TL76;}
_TL75: _T62=Cyc_Arg_args;_T63=Cyc_Arg_current + 1;_T64=_check_fat_subscript(_T62,sizeof(struct _fat_ptr),_T63);_T65=(struct _fat_ptr*)_T64;_T66=*_T65;f(_T66);
Cyc_Arg_current=Cyc_Arg_current + 1;goto _TL74;_TL76: goto _LL5;}}_LL5:;}_pop_handler();goto _TL6B;_TL6A: _T67=Cyc_Core_get_exn_thrown();{void*_T7E=(void*)_T67;void*_T7F;struct _fat_ptr _T80;_T68=(struct Cyc_Arg_Bad_exn_struct*)_T7E;_T69=_T68->tag;_T6A=Cyc_Arg_Bad;if(_T69!=_T6A)goto _TL77;{struct Cyc_Arg_Bad_exn_struct*_T81=(struct Cyc_Arg_Bad_exn_struct*)_T7E;_T80=_T81->f1;}{struct _fat_ptr s2=_T80;_T6B=initpos;{struct Cyc_Arg_Message_Arg_error_struct*_T81=_cycalloc(sizeof(struct Cyc_Arg_Message_Arg_error_struct));_T81->tag=2;
# 271
_T81->f1=s2;_T6C=(struct Cyc_Arg_Message_Arg_error_struct*)_T81;}_T6D=(void*)_T6C;_T6E=speclist;_T6F=errmsg;Cyc_Arg_stop(_T6B,_T6D,_T6E,_T6F);goto _LL16;}_TL77: _T7F=_T7E;{void*exn=_T7F;_rethrow(exn);}_LL16:;}_TL6B:;}}
# 273
Cyc_Arg_current=Cyc_Arg_current + 1;}goto _TL5D;
# 276
_TL5C:{struct _handler_cons _T7C;_T70=& _T7C;_push_handler(_T70);{int _T7D=0;_T71=_T7C.handler;_T72=setjmp(_T71);if(!_T72)goto _TL79;_T7D=1;goto _TL7A;_TL79: _TL7A: if(_T7D)goto _TL7B;else{goto _TL7D;}_TL7D: anonfun(s);_pop_handler();goto _TL7C;_TL7B: _T73=Cyc_Core_get_exn_thrown();{void*_T7E=(void*)_T73;void*_T7F;struct _fat_ptr _T80;_T74=(struct Cyc_Arg_Bad_exn_struct*)_T7E;_T75=_T74->tag;_T76=Cyc_Arg_Bad;if(_T75!=_T76)goto _TL7E;{struct Cyc_Arg_Bad_exn_struct*_T81=(struct Cyc_Arg_Bad_exn_struct*)_T7E;_T80=_T81->f1;}{struct _fat_ptr s2=_T80;_T77=initpos;{struct Cyc_Arg_Message_Arg_error_struct*_T81=_cycalloc(sizeof(struct Cyc_Arg_Message_Arg_error_struct));_T81->tag=2;
# 278
_T81->f1=s2;_T78=(struct Cyc_Arg_Message_Arg_error_struct*)_T81;}_T79=(void*)_T78;_T7A=speclist;_T7B=errmsg;Cyc_Arg_stop(_T77,_T79,_T7A,_T7B);goto _LL1B;}_TL7E: _T7F=_T7E;{void*exn=_T7F;_rethrow(exn);}_LL1B:;}_TL7C:;}}
# 280
Cyc_Arg_current=Cyc_Arg_current + 1;_TL5D:;}goto _TL59;_TL5B:;}}}
