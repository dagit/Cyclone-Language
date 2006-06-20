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
 struct _fat_ptr Cyc_Core_new_string(unsigned);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 73 "cycboot.h"
extern struct _fat_ptr Cyc_aprintf(struct _fat_ptr,struct _fat_ptr);struct Cyc_Lineno_Pos{struct _fat_ptr logical_file;struct _fat_ptr line;int line_no;int col;};
# 31 "lineno.h"
struct Cyc_Lineno_Pos*Cyc_Lineno_pos_of_abs(struct _fat_ptr,int);struct Cyc_Position_Error;
# 48 "position.h"
extern int Cyc_Position_use_gcc_style_location;struct _tuple0{struct _fat_ptr f0;unsigned f1;};
# 26 "position.cyc"
static struct _tuple0 Cyc_Position_seg_to_loc(unsigned);
# 31
int Cyc_Position_use_gcc_style_location=1;static char _TmpG0[1U]="";
# 34
static struct _fat_ptr Cyc_Position_source={_TmpG0,_TmpG0,_TmpG0 + 1U};
# 36
unsigned Cyc_Position_segment_join(unsigned s1,unsigned s2){unsigned _T0;unsigned _T1;unsigned _T2;
if(s1!=0U)goto _TL0;_T0=s2;return _T0;_TL0:
 if(s2!=0U)goto _TL2;_T1=s1;return _T1;_TL2: _T2=s1;
return _T2;}
# 41
int Cyc_Position_segment_equals(unsigned s1,unsigned s2){int _T0;_T0=s1==s2;
return _T0;}
# 45
struct _fat_ptr Cyc_Position_string_of_loc(unsigned seg){struct _fat_ptr _T0;unsigned _T1;int _T2;int _T3;struct _fat_ptr _T4;struct Cyc_String_pa_PrintArg_struct _T5;struct Cyc_Lineno_Pos*_T6;struct Cyc_Int_pa_PrintArg_struct _T7;struct Cyc_Lineno_Pos*_T8;int _T9;struct _fat_ptr _TA;struct _fat_ptr _TB;struct _fat_ptr _TC;struct Cyc_String_pa_PrintArg_struct _TD;struct Cyc_Lineno_Pos*_TE;struct Cyc_Int_pa_PrintArg_struct _TF;struct Cyc_Lineno_Pos*_T10;int _T11;struct Cyc_Int_pa_PrintArg_struct _T12;struct Cyc_Lineno_Pos*_T13;int _T14;struct _fat_ptr _T15;struct _fat_ptr _T16;
struct _tuple0 _T17=Cyc_Position_seg_to_loc(seg);unsigned _T18;struct _fat_ptr _T19;_T19=_T17.f0;_T18=_T17.f1;{struct _fat_ptr source=_T19;unsigned loc=_T18;_T0=source;_T1=loc;_T2=(int)_T1;{
struct Cyc_Lineno_Pos*pos=Cyc_Lineno_pos_of_abs(_T0,_T2);_T3=Cyc_Position_use_gcc_style_location;
if(!_T3)goto _TL4;{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;_T6=pos;
_T1A.f1=_T6->logical_file;_T5=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_T5;{struct Cyc_Int_pa_PrintArg_struct _T1B;_T1B.tag=1;_T8=pos;_T9=_T8->line_no;_T1B.f1=(unsigned long)_T9;_T7=_T1B;}{struct Cyc_Int_pa_PrintArg_struct _T1B=_T7;void*_T1C[2];_T1C[0]=& _T1A;_T1C[1]=& _T1B;_TA=_tag_fat("%s:%d",sizeof(char),6U);_TB=_tag_fat(_T1C,sizeof(void*),2);_T4=Cyc_aprintf(_TA,_TB);}}return _T4;_TL4:{struct Cyc_String_pa_PrintArg_struct _T1A;_T1A.tag=0;_TE=pos;
_T1A.f1=_TE->logical_file;_TD=_T1A;}{struct Cyc_String_pa_PrintArg_struct _T1A=_TD;{struct Cyc_Int_pa_PrintArg_struct _T1B;_T1B.tag=1;_T10=pos;_T11=_T10->line_no;_T1B.f1=(unsigned long)_T11;_TF=_T1B;}{struct Cyc_Int_pa_PrintArg_struct _T1B=_TF;{struct Cyc_Int_pa_PrintArg_struct _T1C;_T1C.tag=1;_T13=pos;_T14=_T13->col;_T1C.f1=(unsigned long)_T14;_T12=_T1C;}{struct Cyc_Int_pa_PrintArg_struct _T1C=_T12;void*_T1D[3];_T1D[0]=& _T1A;_T1D[1]=& _T1B;_T1D[2]=& _T1C;_T15=_tag_fat("%s:(%d:%d)",sizeof(char),11U);_T16=_tag_fat(_T1D,sizeof(void*),3);_TC=Cyc_aprintf(_T15,_T16);}}}return _TC;}}}
# 53
static struct _fat_ptr Cyc_Position_string_of_pos_pr(struct Cyc_Lineno_Pos*pos){int _T0;struct _fat_ptr _T1;struct Cyc_String_pa_PrintArg_struct _T2;struct Cyc_Lineno_Pos*_T3;struct Cyc_Int_pa_PrintArg_struct _T4;struct Cyc_Lineno_Pos*_T5;int _T6;struct _fat_ptr _T7;struct _fat_ptr _T8;struct _fat_ptr _T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc_Lineno_Pos*_TB;struct Cyc_Int_pa_PrintArg_struct _TC;struct Cyc_Lineno_Pos*_TD;int _TE;struct Cyc_Int_pa_PrintArg_struct _TF;struct Cyc_Lineno_Pos*_T10;int _T11;struct _fat_ptr _T12;struct _fat_ptr _T13;_T0=Cyc_Position_use_gcc_style_location;
if(!_T0)goto _TL6;{struct Cyc_String_pa_PrintArg_struct _T14;_T14.tag=0;_T3=pos;
_T14.f1=_T3->logical_file;_T2=_T14;}{struct Cyc_String_pa_PrintArg_struct _T14=_T2;{struct Cyc_Int_pa_PrintArg_struct _T15;_T15.tag=1;_T5=pos;_T6=_T5->line_no;_T15.f1=(unsigned long)_T6;_T4=_T15;}{struct Cyc_Int_pa_PrintArg_struct _T15=_T4;void*_T16[2];_T16[0]=& _T14;_T16[1]=& _T15;_T7=_tag_fat("%s:%d",sizeof(char),6U);_T8=_tag_fat(_T16,sizeof(void*),2);_T1=Cyc_aprintf(_T7,_T8);}}return _T1;_TL6:{struct Cyc_String_pa_PrintArg_struct _T14;_T14.tag=0;_TB=pos;
_T14.f1=_TB->logical_file;_TA=_T14;}{struct Cyc_String_pa_PrintArg_struct _T14=_TA;{struct Cyc_Int_pa_PrintArg_struct _T15;_T15.tag=1;_TD=pos;_TE=_TD->line_no;_T15.f1=(unsigned long)_TE;_TC=_T15;}{struct Cyc_Int_pa_PrintArg_struct _T15=_TC;{struct Cyc_Int_pa_PrintArg_struct _T16;_T16.tag=1;_T10=pos;_T11=_T10->col;_T16.f1=(unsigned long)_T11;_TF=_T16;}{struct Cyc_Int_pa_PrintArg_struct _T16=_TF;void*_T17[3];_T17[0]=& _T14;_T17[1]=& _T15;_T17[2]=& _T16;_T12=_tag_fat("%s:(%d:%d)",sizeof(char),11U);_T13=_tag_fat(_T17,sizeof(void*),3);_T9=Cyc_aprintf(_T12,_T13);}}}return _T9;}
# 59
struct _fat_ptr Cyc_Position_string_of_segment(unsigned s){struct _fat_ptr _T0;_T0=
Cyc_Position_string_of_loc(s);return _T0;}
# 63
static struct Cyc_Lineno_Pos*Cyc_Position_new_pos (void){struct Cyc_Lineno_Pos*_T0;{struct Cyc_Lineno_Pos*_T1=_cycalloc(sizeof(struct Cyc_Lineno_Pos));
_T1->logical_file=_tag_fat("",sizeof(char),1U);_T1->line=Cyc_Core_new_string(0U);_T1->line_no=0;_T1->col=0;_T0=(struct Cyc_Lineno_Pos*)_T1;}return _T0;}
# 67
struct Cyc_List_List*Cyc_Position_strings_of_segments(struct Cyc_List_List*segs){struct Cyc_List_List*_T0;struct _fat_ptr*_T1;struct Cyc_List_List*_T2;void*_T3;unsigned _T4;struct Cyc_List_List*_T5;struct Cyc_List_List*_T6;
# 69
struct Cyc_List_List*ans=0;
_TLB: if(segs!=0)goto _TL9;else{goto _TLA;}
_TL9:{struct Cyc_List_List*_T7=_cycalloc(sizeof(struct Cyc_List_List));{struct _fat_ptr*_T8=_cycalloc(sizeof(struct _fat_ptr));_T2=segs;_T3=_T2->hd;_T4=(unsigned)_T3;*_T8=Cyc_Position_string_of_segment(_T4);_T1=(struct _fat_ptr*)_T8;}_T7->hd=_T1;_T7->tl=ans;_T0=(struct Cyc_List_List*)_T7;}ans=_T0;_T5=segs;
# 70
segs=_T5->tl;goto _TLB;_TLA: _T6=ans;
# 72
return _T6;}struct Cyc_Position_Error{struct _fat_ptr source;unsigned seg;struct _fat_ptr desc;};
# 95 "position.cyc"
struct Cyc_Position_Error*Cyc_Position_mk_err(unsigned l,struct _fat_ptr desc){struct Cyc_Position_Error*_T0;
struct _tuple0 _T1=Cyc_Position_seg_to_loc(l);unsigned _T2;struct _fat_ptr _T3;_T3=_T1.f0;_T2=_T1.f1;{struct _fat_ptr source=_T3;unsigned loc=_T2;{struct Cyc_Position_Error*_T4=_cycalloc(sizeof(struct Cyc_Position_Error));
_T4->source=source;_T4->seg=l;_T4->desc=desc;_T0=(struct Cyc_Position_Error*)_T4;}return _T0;}}
# 99
unsigned Cyc_Position_get_seg(struct Cyc_Position_Error*e){struct Cyc_Position_Error*_T0;unsigned _T1;_T0=e;_T1=_T0->seg;
return _T1;}
# 102
struct _fat_ptr Cyc_Position_get_desc(struct Cyc_Position_Error*e){struct Cyc_Position_Error*_T0;struct _fat_ptr _T1;_T0=e;_T1=_T0->desc;
return _T1;}
# 107
void Cyc_Position_reset_position(struct _fat_ptr s){Cyc_Position_source=s;}
void Cyc_Position_set_position_file(struct _fat_ptr s){Cyc_Position_source=s;}
struct _fat_ptr Cyc_Position_get_position_file (void){struct _fat_ptr _T0;_T0=Cyc_Position_source;return _T0;}
# 111
struct _fat_ptr Cyc_Position_get_line_directive(unsigned s){struct _fat_ptr _T0;unsigned char*_T1;unsigned _T2;struct _fat_ptr _T3;unsigned _T4;int _T5;struct _fat_ptr _T6;struct Cyc_Int_pa_PrintArg_struct _T7;struct Cyc_Lineno_Pos*_T8;int _T9;struct Cyc_String_pa_PrintArg_struct _TA;struct Cyc_Lineno_Pos*_TB;struct _fat_ptr _TC;struct _fat_ptr _TD;struct _fat_ptr _TE;
struct _tuple0 _TF=Cyc_Position_seg_to_loc(s);unsigned _T10;struct _fat_ptr _T11;_T11=_TF.f0;_T10=_TF.f1;{struct _fat_ptr source=_T11;unsigned loc=_T10;_T0=source;_T1=_T0.curr;_T2=(unsigned)_T1;
if(!_T2)goto _TLC;_T3=source;_T4=loc;_T5=(int)_T4;{
struct Cyc_Lineno_Pos*pos_s=Cyc_Lineno_pos_of_abs(_T3,_T5);
if(pos_s==0)goto _TLE;{struct Cyc_Int_pa_PrintArg_struct _T12;_T12.tag=1;_T8=pos_s;_T9=_T8->line_no;
_T12.f1=(unsigned long)_T9;_T7=_T12;}{struct Cyc_Int_pa_PrintArg_struct _T12=_T7;{struct Cyc_String_pa_PrintArg_struct _T13;_T13.tag=0;_TB=pos_s;_T13.f1=_TB->logical_file;_TA=_T13;}{struct Cyc_String_pa_PrintArg_struct _T13=_TA;void*_T14[2];_T14[0]=& _T12;_T14[1]=& _T13;_TC=_tag_fat("\n#line %d \"%s\"\n",sizeof(char),16U);_TD=_tag_fat(_T14,sizeof(void*),2);_T6=Cyc_aprintf(_TC,_TD);}}return _T6;_TLE:;}goto _TLD;_TLC: _TLD: _TE=
# 118
_tag_fat(0,0,0);return _TE;}}
# 121
unsigned Cyc_Position_loc_to_seg(unsigned loc){unsigned _T0;_T0=loc;
return _T0;}
# 125
static struct _tuple0 Cyc_Position_seg_to_loc(unsigned seg){struct _tuple0 _T0;{struct _tuple0 _T1;
_T1.f0=Cyc_Position_source;_T1.f1=seg;_T0=_T1;}return _T0;}
