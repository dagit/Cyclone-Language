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

# 95 "core.h"
 struct _fat_ptr Cyc_Core_new_string(unsigned);
# 100
struct _fat_ptr Cyc_Core_rnew_string(struct _RegionHandle*,unsigned);
# 173
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc___cycFILE;
# 276 "cycboot.h"
extern int Cyc_file_string_read(struct Cyc___cycFILE*,struct _fat_ptr,int,int);struct Cyc_Lexing_lexbuf{void(*refill_buff)(struct Cyc_Lexing_lexbuf*);void*refill_state;struct _fat_ptr lex_buffer;int lex_buffer_len;int lex_abs_pos;int lex_start_pos;int lex_curr_pos;int lex_last_pos;int lex_last_action;int lex_eof_reached;};struct Cyc_Lexing_function_lexbuf_state{int(*read_fun)(struct _fat_ptr,int,void*);void*read_fun_state;};
# 73 "string.h"
extern struct _fat_ptr Cyc_zstrncpy(struct _fat_ptr,struct _fat_ptr,unsigned long);
# 105 "string.h"
extern struct _fat_ptr Cyc_strdup(struct _fat_ptr);char Cyc_Lexing_Error[6U]="Error";
# 73 "lexing.cyc"
static char Cyc_Lexing_aux_buffer_v[1U]={'\000'};
static struct _fat_ptr Cyc_Lexing_aux_buffer={(void*)Cyc_Lexing_aux_buffer_v,(void*)Cyc_Lexing_aux_buffer_v,(void*)(Cyc_Lexing_aux_buffer_v + 1U)};
# 76
void Cyc_Lexing_lex_refill(struct Cyc_Lexing_lexbuf*lexbuf){struct _fat_ptr _T0;unsigned _T1;struct Cyc_Lexing_lexbuf*_T2;void*_T3;struct Cyc_Lexing_function_lexbuf_state*_T4;int(*_T5)(struct _fat_ptr,int,void*);struct _fat_ptr _T6;struct _fat_ptr _T7;unsigned _T8;unsigned _T9;int _TA;struct Cyc_Lexing_lexbuf*_TB;void*_TC;struct Cyc_Lexing_function_lexbuf_state*_TD;void*_TE;int _TF;struct Cyc_Lexing_lexbuf*_T10;struct Cyc_Lexing_lexbuf*_T11;int _T12;int _T13;struct Cyc_Lexing_lexbuf*_T14;int _T15;unsigned _T16;struct _fat_ptr _T17;struct _fat_ptr _T18;int _T19;struct _fat_ptr _T1A;struct Cyc_Lexing_lexbuf*_T1B;struct _fat_ptr _T1C;int _T1D;unsigned long _T1E;struct Cyc_Lexing_lexbuf*_T1F;struct Cyc_Lexing_lexbuf*_T20;struct Cyc_Lexing_lexbuf*_T21;struct Cyc_Lexing_lexbuf*_T22;int _T23;int _T24;struct Cyc_Lexing_lexbuf*_T25;struct Cyc_Lexing_lexbuf*_T26;int _T27;int _T28;struct Cyc_Lexing_lexbuf*_T29;struct Cyc_Lexing_lexbuf*_T2A;int _T2B;int _T2C;struct Cyc_Lexing_lexbuf*_T2D;struct Cyc_Lexing_lexbuf*_T2E;int _T2F;int _T30;struct Cyc_Lexing_lexbuf*_T31;struct _fat_ptr _T32;struct _fat_ptr _T33;struct Cyc_Lexing_lexbuf*_T34;struct _fat_ptr _T35;int _T36;struct _fat_ptr _T37;struct Cyc_Lexing_lexbuf*_T38;int _T39;int _T3A;int _T3B;unsigned long _T3C;struct Cyc_Lexing_lexbuf*_T3D;struct _fat_ptr _T3E;struct Cyc_Lexing_lexbuf*_T3F;int _T40;int _T41;int _T42;struct _fat_ptr _T43;struct _fat_ptr _T44;struct _fat_ptr _T45;int _T46;unsigned long _T47;struct Cyc_Lexing_lexbuf*_T48;struct Cyc_Lexing_lexbuf*_T49;int _T4A;int _T4B;struct Cyc_Lexing_lexbuf*_T4C;struct Cyc_Lexing_lexbuf*_T4D;int _T4E;int _T4F;struct Cyc_Lexing_lexbuf*_T50;struct Cyc_Lexing_lexbuf*_T51;int _T52;int _T53;struct Cyc_Lexing_lexbuf*_T54;struct Cyc_Lexing_lexbuf*_T55;int _T56;int _T57;_T0=Cyc_Lexing_aux_buffer;_T1=
_get_fat_size(_T0,sizeof(char));if(_T1!=1U)goto _TL0;Cyc_Lexing_aux_buffer=Cyc_Core_new_string(4097U);goto _TL1;_TL0: _TL1: _T2=lexbuf;_T3=_T2->refill_state;_T4=(struct Cyc_Lexing_function_lexbuf_state*)_T3;_T5=_T4->read_fun;_T6=Cyc_Lexing_aux_buffer;_T7=Cyc_Lexing_aux_buffer;_T8=
# 80
_get_fat_size(_T7,sizeof(char));_T9=_T8 - 1U;_TA=(int)_T9;_TB=lexbuf;_TC=_TB->refill_state;_TD=(struct Cyc_Lexing_function_lexbuf_state*)_TC;_TE=_TD->read_fun_state;{
# 78
int read=
_T5(_T6,_TA,_TE);
# 82
if(read <= 0)goto _TL2;_TF=read;goto _TL3;_TL2: _T10=lexbuf;_T10->lex_eof_reached=1;_TF=0;_TL3: {int n=_TF;_T11=lexbuf;_T12=_T11->lex_start_pos;_T13=n;
if(_T12 >= _T13)goto _TL4;_T14=lexbuf;{
int oldlen=_T14->lex_buffer_len;
int newlen=oldlen * 2;_T15=newlen + 1;_T16=(unsigned)_T15;{
# 87
struct _fat_ptr newbuf=Cyc_Core_new_string(_T16);_T17=newbuf;_T18=
_fat_ptr_decrease_size(_T17,sizeof(char),1U);_T19=oldlen;_T1A=_fat_ptr_plus(_T18,sizeof(char),_T19);_T1B=lexbuf;_T1C=_T1B->lex_buffer;_T1D=oldlen;_T1E=(unsigned long)_T1D;Cyc_zstrncpy(_T1A,_T1C,_T1E);_T1F=lexbuf;
_T1F->lex_buffer=newbuf;_T20=lexbuf;
_T20->lex_buffer_len=newlen;_T21=lexbuf;_T22=lexbuf;_T23=_T22->lex_abs_pos;_T24=oldlen;
_T21->lex_abs_pos=_T23 - _T24;_T25=lexbuf;_T26=lexbuf;_T27=_T26->lex_curr_pos;_T28=oldlen;
_T25->lex_curr_pos=_T27 + _T28;_T29=lexbuf;_T2A=lexbuf;_T2B=_T2A->lex_start_pos;_T2C=oldlen;
_T29->lex_start_pos=_T2B + _T2C;_T2D=lexbuf;_T2E=lexbuf;_T2F=_T2E->lex_last_pos;_T30=oldlen;
_T2D->lex_last_pos=_T2F + _T30;}}goto _TL5;_TL4: _TL5: _T31=lexbuf;_T32=_T31->lex_buffer;_T33=
# 96
_fat_ptr_decrease_size(_T32,sizeof(char),1U);_T34=lexbuf;_T35=_T34->lex_buffer;_T36=n;_T37=
_fat_ptr_plus(_T35,sizeof(char),_T36);_T38=lexbuf;_T39=_T38->lex_buffer_len;_T3A=n;_T3B=_T39 - _T3A;_T3C=(unsigned long)_T3B;
# 96
Cyc_zstrncpy(_T33,_T37,_T3C);_T3D=lexbuf;_T3E=_T3D->lex_buffer;_T3F=lexbuf;_T40=_T3F->lex_buffer_len;_T41=n;_T42=_T40 - _T41;_T43=
# 99
_fat_ptr_plus(_T3E,sizeof(char),_T42);_T44=_fat_ptr_decrease_size(_T43,sizeof(char),1U);_T45=Cyc_Lexing_aux_buffer;_T46=n;_T47=(unsigned long)_T46;Cyc_zstrncpy(_T44,_T45,_T47);_T48=lexbuf;_T49=lexbuf;_T4A=_T49->lex_abs_pos;_T4B=n;
# 101
_T48->lex_abs_pos=_T4A + _T4B;_T4C=lexbuf;_T4D=lexbuf;_T4E=_T4D->lex_curr_pos;_T4F=n;
_T4C->lex_curr_pos=_T4E - _T4F;_T50=lexbuf;_T51=lexbuf;_T52=_T51->lex_start_pos;_T53=n;
_T50->lex_start_pos=_T52 - _T53;_T54=lexbuf;_T55=lexbuf;_T56=_T55->lex_last_pos;_T57=n;
_T54->lex_last_pos=_T56 - _T57;}}}
# 107
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_function(int(*read_fun)(struct _fat_ptr,int,void*),void*read_fun_state){struct Cyc_Lexing_lexbuf*_T0;struct Cyc_Lexing_function_lexbuf_state*_T1;{struct Cyc_Lexing_lexbuf*_T2=_cycalloc(sizeof(struct Cyc_Lexing_lexbuf));
# 109
_T2->refill_buff=Cyc_Lexing_lex_refill;{struct Cyc_Lexing_function_lexbuf_state*_T3=_cycalloc(sizeof(struct Cyc_Lexing_function_lexbuf_state));
_T3->read_fun=read_fun;_T3->read_fun_state=read_fun_state;_T1=(struct Cyc_Lexing_function_lexbuf_state*)_T3;}_T2->refill_state=_T1;
_T2->lex_buffer=Cyc_Core_new_string(8193U);
_T2->lex_buffer_len=8192;_T2->lex_abs_pos=- 8192;
_T2->lex_start_pos=8192;_T2->lex_curr_pos=8192;_T2->lex_last_pos=8192;
_T2->lex_last_action=0;
_T2->lex_eof_reached=0;_T0=(struct Cyc_Lexing_lexbuf*)_T2;}
# 109
return _T0;}
# 118
static int Cyc_Lexing_read_from_file(struct _fat_ptr aux,int n,struct Cyc___cycFILE*f){int _T0;_T0=
Cyc_file_string_read(f,aux,0,n);return _T0;}
# 123
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_file(struct Cyc___cycFILE*f){struct Cyc_Lexing_lexbuf*(*_T0)(int(*)(struct _fat_ptr,int,struct Cyc___cycFILE*),struct Cyc___cycFILE*);struct Cyc___cycFILE*_T1;struct Cyc_Lexing_lexbuf*_T2;{
struct Cyc_Lexing_lexbuf*(*_T3)(int(*)(struct _fat_ptr,int,struct Cyc___cycFILE*),struct Cyc___cycFILE*)=(struct Cyc_Lexing_lexbuf*(*)(int(*)(struct _fat_ptr,int,struct Cyc___cycFILE*),struct Cyc___cycFILE*))Cyc_Lexing_from_function;_T0=_T3;}_T1=f;_T2=_T0(Cyc_Lexing_read_from_file,_T1);return _T2;}
# 127
static void Cyc_Lexing_set_eof(struct Cyc_Lexing_lexbuf*lbuf){struct Cyc_Lexing_lexbuf*_T0;_T0=lbuf;
_T0->lex_eof_reached=1;}
# 131
struct Cyc_Lexing_lexbuf*Cyc_Lexing_from_string(struct _fat_ptr s){struct Cyc_Lexing_lexbuf*_T0;struct _fat_ptr _T1;unsigned _T2;{struct Cyc_Lexing_lexbuf*_T3=_cycalloc(sizeof(struct Cyc_Lexing_lexbuf));
_T3->refill_buff=Cyc_Lexing_set_eof;
_T3->refill_state=(void*)0;
_T3->lex_buffer=Cyc_strdup(s);_T1=s;_T2=
_get_fat_size(_T1,sizeof(char));_T3->lex_buffer_len=(int)_T2;
_T3->lex_abs_pos=0;_T3->lex_start_pos=0;_T3->lex_curr_pos=0;_T3->lex_last_pos=0;_T3->lex_last_action=0;
_T3->lex_eof_reached=1;_T0=(struct Cyc_Lexing_lexbuf*)_T3;}
# 132
return _T0;}
# 140
struct _fat_ptr Cyc_Lexing_rlexeme(struct _RegionHandle*r,struct Cyc_Lexing_lexbuf*lbuf){struct Cyc_Lexing_lexbuf*_T0;int _T1;struct Cyc_Lexing_lexbuf*_T2;int _T3;struct _RegionHandle*_T4;int _T5;unsigned _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct Cyc_Lexing_lexbuf*_T9;struct _fat_ptr _TA;struct Cyc_Lexing_lexbuf*_TB;int _TC;struct _fat_ptr _TD;int _TE;unsigned long _TF;struct _fat_ptr _T10;int _T11;unsigned char*_T12;char*_T13;unsigned _T14;unsigned char*_T15;char*_T16;struct _fat_ptr _T17;_T0=lbuf;_T1=_T0->lex_curr_pos;_T2=lbuf;_T3=_T2->lex_start_pos;{
int len=_T1 - _T3;_T4=r;_T5=len + 1;_T6=(unsigned)_T5;{
struct _fat_ptr s=Cyc_Core_rnew_string(_T4,_T6);_T7=s;_T8=
_fat_ptr_decrease_size(_T7,sizeof(char),1U);_T9=lbuf;_TA=_T9->lex_buffer;_TB=lbuf;_TC=_TB->lex_start_pos;_TD=_fat_ptr_plus(_TA,sizeof(char),_TC);_TE=len;_TF=(unsigned long)_TE;Cyc_zstrncpy(_T8,_TD,_TF);_T10=s;_T11=len;{struct _fat_ptr _T18=_fat_ptr_plus(_T10,sizeof(char),_T11);_T12=_check_fat_subscript(_T18,sizeof(char),0U);_T13=(char*)_T12;{char _T19=*_T13;char _T1A='\000';_T14=_get_fat_size(_T18,sizeof(char));if(_T14!=1U)goto _TL6;if(_T19!=0)goto _TL6;if(_T1A==0)goto _TL6;_throw_arraybounds();goto _TL7;_TL6: _TL7: _T15=_T18.curr;_T16=(char*)_T15;*_T16=_T1A;}}_T17=s;
# 145
return _T17;}}}
# 148
struct _fat_ptr Cyc_Lexing_lexeme(struct Cyc_Lexing_lexbuf*lbuf){struct _fat_ptr _T0;_T0=
Cyc_Lexing_rlexeme(Cyc_Core_heap_region,lbuf);return _T0;}
# 151
char Cyc_Lexing_lexeme_char(struct Cyc_Lexing_lexbuf*lbuf,int i){struct Cyc_Lexing_lexbuf*_T0;struct _fat_ptr _T1;struct Cyc_Lexing_lexbuf*_T2;int _T3;int _T4;int _T5;unsigned char*_T6;char*_T7;char _T8;_T0=lbuf;_T1=_T0->lex_buffer;_T2=lbuf;_T3=_T2->lex_start_pos;_T4=i;_T5=_T3 + _T4;_T6=_check_fat_subscript(_T1,sizeof(char),_T5);_T7=(char*)_T6;_T8=*_T7;
return _T8;}
# 154
int Cyc_Lexing_lexeme_start(struct Cyc_Lexing_lexbuf*lbuf){struct Cyc_Lexing_lexbuf*_T0;int _T1;struct Cyc_Lexing_lexbuf*_T2;int _T3;int _T4;_T0=lbuf;_T1=_T0->lex_abs_pos;_T2=lbuf;_T3=_T2->lex_start_pos;_T4=_T1 + _T3;
return _T4;}
# 157
int Cyc_Lexing_lexeme_end(struct Cyc_Lexing_lexbuf*lbuf){struct Cyc_Lexing_lexbuf*_T0;int _T1;struct Cyc_Lexing_lexbuf*_T2;int _T3;int _T4;_T0=lbuf;_T1=_T0->lex_abs_pos;_T2=lbuf;_T3=_T2->lex_curr_pos;_T4=_T1 + _T3;
return _T4;}
