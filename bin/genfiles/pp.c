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
 struct Cyc_Core_Opt{void*v;};
# 95 "core.h"
struct _fat_ptr Cyc_Core_new_string(unsigned);
# 119
void*Cyc_Core_identity(void*);
# 121
int Cyc_Core_intcmp(int,int);extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};struct Cyc___cycFILE;struct Cyc_String_pa_PrintArg_struct{int tag;struct _fat_ptr f1;};
# 100 "cycboot.h"
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 38 "string.h"
extern unsigned long Cyc_strlen(struct _fat_ptr);
# 54 "string.h"
extern int Cyc_zstrptrcmp(struct _fat_ptr*,struct _fat_ptr*);
# 66
extern struct _fat_ptr Cyc_str_sepstr(struct Cyc_List_List*,struct _fat_ptr);struct Cyc_Hashtable_Table;
# 39 "hashtable.h"
extern struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int,int(*)(void*,void*),int(*)(void*));
# 50
extern void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*,void*,void*);
# 52
extern void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*,void*);
# 86
extern int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*);struct Cyc_Fn_Function{void*(*f)(void*,void*);void*env;};
# 46 "fn.h"
extern struct Cyc_Fn_Function*Cyc_Fn_make_fn(void*(*)(void*,void*),void*);
# 49
extern struct Cyc_Fn_Function*Cyc_Fn_fp2fn(void*(*)(void*));
# 52
extern void*Cyc_Fn_apply(struct Cyc_Fn_Function*,void*);
# 39 "pp.h"
extern int Cyc_PP_tex_output;struct Cyc_PP_Ppstate;struct Cyc_PP_Out;struct Cyc_PP_Doc;struct Cyc_Xarray_Xarray{struct _fat_ptr elmts;int num_elmts;};
# 40 "xarray.h"
extern int Cyc_Xarray_length(struct Cyc_Xarray_Xarray*);
# 42
extern void*Cyc_Xarray_get(struct Cyc_Xarray_Xarray*,int);
# 45
extern void Cyc_Xarray_set(struct Cyc_Xarray_Xarray*,int,void*);
# 48
extern struct Cyc_Xarray_Xarray*Cyc_Xarray_create(int,void*);
# 66
extern void Cyc_Xarray_add(struct Cyc_Xarray_Xarray*,void*);
# 121
extern void Cyc_Xarray_reuse(struct Cyc_Xarray_Xarray*);struct Cyc_PP_Empty_PP_Alist_struct{int tag;int f1;};struct Cyc_PP_Single_PP_Alist_struct{int tag;void*f1;};struct Cyc_PP_Append_PP_Alist_struct{int tag;void*f1;void*f2;};
# 51 "pp.cyc"
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_stringptr={0,0};
struct Cyc_PP_Empty_PP_Alist_struct Cyc_PP_Empty_link={0,0};struct _tuple0{void*f0;void*f1;};
# 54
void*Cyc_PP_append(void*a1,void*a2){struct _tuple0 _T0;void*_T1;int*_T2;int _T3;void*_T4;void*_T5;int*_T6;int _T7;void*_T8;struct Cyc_PP_Append_PP_Alist_struct*_T9;void*_TA;{struct _tuple0 _TB;
_TB.f0=a1;_TB.f1=a2;_T0=_TB;}{struct _tuple0 _TB=_T0;_T1=_TB.f0;_T2=(int*)_T1;_T3=*_T2;if(_T3!=0)goto _TL0;_T4=a2;
return _T4;_TL0: _T5=_TB.f1;_T6=(int*)_T5;_T7=*_T6;if(_T7!=0)goto _TL2;_T8=a1;
return _T8;_TL2:{struct Cyc_PP_Append_PP_Alist_struct*_TC=_cycalloc(sizeof(struct Cyc_PP_Append_PP_Alist_struct));_TC->tag=2;
_TC->f1=a1;_TC->f2=a2;_T9=(struct Cyc_PP_Append_PP_Alist_struct*)_TC;}_TA=(void*)_T9;return _TA;;}}
# 62
struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct Cyc_List_List*l){void*_T0;int*_T1;unsigned _T2;struct Cyc_List_List*_T3;void*_T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;void*_T8;void*_T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;void*_TD;void*_TE;_T0=y;_T1=(int*)_T0;_T2=*_T1;switch(_T2){case 0: _T3=l;
# 64
return _T3;case 1: _T4=y;{struct Cyc_PP_Single_PP_Alist_struct*_TF=(struct Cyc_PP_Single_PP_Alist_struct*)_T4;_T5=_TF->f1;_TE=(void*)_T5;}{void*z=_TE;{struct Cyc_List_List*_TF=_cycalloc(sizeof(struct Cyc_List_List));
_TF->hd=z;_TF->tl=l;_T6=(struct Cyc_List_List*)_TF;}return _T6;}default: _T7=y;{struct Cyc_PP_Append_PP_Alist_struct*_TF=(struct Cyc_PP_Append_PP_Alist_struct*)_T7;_T8=_TF->f1;_TE=(void*)_T8;_T9=_TF->f2;_TD=(void*)_T9;}{void*a1=_TE;void*a2=_TD;_TA=a1;_TB=
Cyc_PP_list_of_alist_f(a2,l);_TC=Cyc_PP_list_of_alist_f(_TA,_TB);return _TC;}};}
# 69
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){struct Cyc_List_List*_T0;_T0=
Cyc_PP_list_of_alist_f(x,0);return _T0;}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*f;};
# 98
static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){int*_T0;unsigned _T1;struct _fat_ptr*_T2;struct Cyc_String_pa_PrintArg_struct _T3;struct _fat_ptr*_T4;struct Cyc___cycFILE*_T5;struct _fat_ptr _T6;struct _fat_ptr _T7;void*_T8;void*_T9;int _TA;int _TB;int _TC;struct Cyc_Xarray_Xarray*_TD;int _TE;void*_TF;
struct Cyc_Xarray_Xarray*xarr=Cyc_Xarray_create(16,al);
Cyc_Xarray_add(xarr,al);{
int last=0;
_TL5: if(last >= 0)goto _TL6;else{goto _TL7;}
_TL6:{void*_T10=Cyc_Xarray_get(xarr,last);void*_T11;void*_T12;_T0=(int*)_T10;_T1=*_T0;switch(_T1){case 0:
 last=last + -1;goto _LL0;case 1:{struct Cyc_PP_Single_PP_Alist_struct*_T13=(struct Cyc_PP_Single_PP_Alist_struct*)_T10;_T2=_T13->f1;_T12=(struct _fat_ptr*)_T2;}{struct _fat_ptr*s=_T12;
last=last + -1;{struct Cyc_String_pa_PrintArg_struct _T13;_T13.tag=0;_T4=s;_T13.f1=*_T4;_T3=_T13;}{struct Cyc_String_pa_PrintArg_struct _T13=_T3;void*_T14[1];_T14[0]=& _T13;_T5=f;_T6=_tag_fat("%s",sizeof(char),3U);_T7=_tag_fat(_T14,sizeof(void*),1);Cyc_fprintf(_T5,_T6,_T7);}goto _LL0;}default:{struct Cyc_PP_Append_PP_Alist_struct*_T13=(struct Cyc_PP_Append_PP_Alist_struct*)_T10;_T8=_T13->f1;_T12=(void*)_T8;_T9=_T13->f2;_T11=(void*)_T9;}{void*a1=_T12;void*a2=_T11;
# 107
Cyc_Xarray_set(xarr,last,a2);_TA=last;_TB=
Cyc_Xarray_length(xarr);_TC=_TB - 1;if(_TA!=_TC)goto _TL9;
Cyc_Xarray_add(xarr,a1);goto _TLA;
# 111
_TL9: _TD=xarr;_TE=last + 1;_TF=a1;Cyc_Xarray_set(_TD,_TE,_TF);_TLA:
 last=last + 1;goto _LL0;}}_LL0:;}goto _TL5;_TL7:
# 116
 Cyc_Xarray_reuse(xarr);}}
# 120
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){struct Cyc_PP_Out*(*_T0)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T1)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T2;struct Cyc_Fn_Function*_T3;struct Cyc_PP_Ppstate*_T4;struct Cyc___cycFILE*_T5;struct Cyc_PP_Out*_T6;void*_T7;_T1=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_T8)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T1;_T0=_T8;}_T2=d;_T3=_T2->f;{struct Cyc_PP_Ppstate*_T8=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_T8->ci=0;_T8->cc=0;_T8->cl=1;_T8->pw=w;_T8->epw=w;_T4=(struct Cyc_PP_Ppstate*)_T8;}{struct Cyc_PP_Out*o=_T0(_T3,_T4);_T5=f;_T6=o;_T7=_T6->ppout;
Cyc_PP_dump_out(_T5,_T7);}}
# 126
struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Out*(*_T0)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T1)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T2;struct Cyc_Fn_Function*_T3;struct Cyc_PP_Ppstate*_T4;struct Cyc_PP_Out*_T5;void*_T6;struct Cyc_List_List*_T7;struct _fat_ptr _T8;struct _fat_ptr _T9;_T1=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_TA)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T1;_T0=_TA;}_T2=d;_T3=_T2->f;{struct Cyc_PP_Ppstate*_TA=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_TA->ci=0;_TA->cc=0;_TA->cl=1;_TA->pw=w;_TA->epw=w;_T4=(struct Cyc_PP_Ppstate*)_TA;}{struct Cyc_PP_Out*o=_T0(_T3,_T4);_T5=o;_T6=_T5->ppout;_T7=
Cyc_PP_list_of_alist(_T6);_T8=_tag_fat("",sizeof(char),1U);_T9=Cyc_str_sepstr(_T7,_T8);return _T9;}}struct _tuple1{struct _fat_ptr f0;struct Cyc_List_List*f1;};
# 133
struct _tuple1*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){struct Cyc_PP_Out*(*_T0)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T1)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T2;struct Cyc_Fn_Function*_T3;struct Cyc_PP_Ppstate*_T4;struct _tuple1*_T5;struct Cyc_PP_Out*_T6;void*_T7;struct Cyc_List_List*_T8;struct _fat_ptr _T9;struct _fat_ptr _TA;struct Cyc_PP_Out*_TB;void*_TC;_T1=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_TD)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T1;_T0=_TD;}_T2=d;_T3=_T2->f;{struct Cyc_PP_Ppstate*_TD=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_TD->ci=0;_TD->cc=0;_TD->cl=1;_TD->pw=w;_TD->epw=w;_T4=(struct Cyc_PP_Ppstate*)_TD;}{struct Cyc_PP_Out*o=_T0(_T3,_T4);{struct _tuple1*_TD=_cycalloc(sizeof(struct _tuple1));_T6=o;_T7=_T6->ppout;_T8=
# 136
Cyc_PP_list_of_alist(_T7);_T9=_tag_fat("",sizeof(char),1U);_TA=Cyc_str_sepstr(_T8,_T9);
# 135
_TD->f0=_TA;_TB=o;_TC=_TB->links;
# 137
_TD->f1=Cyc_PP_list_of_alist(_TC);_T5=(struct _tuple1*)_TD;}
# 135
return _T5;}}
# 140
static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;
# 142
int Cyc_PP_tex_output=0;
# 144
struct _fat_ptr Cyc_PP_nlblanks(int i){struct Cyc_Core_Opt*_T0;struct Cyc_Hashtable_Table*(*_T1)(int,int(*)(int,int),int(*)(int));struct Cyc_Hashtable_Table*(*_T2)(int,int(*)(void*,void*),int(*)(void*));int(*_T3)(int,int);int(*_T4)(int);void*(*_T5)(void*);struct Cyc_Core_Failure_exn_struct*_T6;void*_T7;struct _handler_cons*_T8;int _T9;struct _fat_ptr*(*_TA)(struct Cyc_Hashtable_Table*,int);void*(*_TB)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Core_Opt*_TC;void*_TD;struct Cyc_Hashtable_Table*_TE;int _TF;struct _fat_ptr*_T10;void*_T11;struct Cyc_Core_Not_found_exn_struct*_T12;char*_T13;char*_T14;int _T15;int _T16;int _T17;int _T18;unsigned _T19;struct _fat_ptr _T1A;unsigned char*_T1B;char*_T1C;unsigned _T1D;unsigned char*_T1E;char*_T1F;struct _fat_ptr _T20;int _T21;unsigned char*_T22;char*_T23;unsigned _T24;unsigned char*_T25;char*_T26;struct _fat_ptr _T27;int _T28;int _T29;int _T2A;unsigned char*_T2B;char*_T2C;unsigned _T2D;unsigned char*_T2E;char*_T2F;void(*_T30)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*);void(*_T31)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Core_Opt*_T32;void*_T33;struct Cyc_Hashtable_Table*_T34;int _T35;struct _fat_ptr*_T36;struct _fat_ptr*_T37;unsigned _T38;struct _fat_ptr _T39;struct _fat_ptr _T3A;int _T3B;unsigned _T3C;struct _fat_ptr _T3D;unsigned char*_T3E;char*_T3F;char*_T40;unsigned _T41;unsigned char*_T42;char*_T43;struct _fat_ptr _T44;unsigned char*_T45;char*_T46;unsigned _T47;unsigned char*_T48;char*_T49;struct _fat_ptr _T4A;unsigned char*_T4B;char*_T4C;unsigned _T4D;unsigned char*_T4E;char*_T4F;struct _fat_ptr _T50;int _T51;unsigned char*_T52;char*_T53;unsigned _T54;unsigned char*_T55;char*_T56;void(*_T57)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*);void(*_T58)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Core_Opt*_T59;void*_T5A;struct Cyc_Hashtable_Table*_T5B;int _T5C;struct _fat_ptr*_T5D;struct _fat_ptr*_T5E;unsigned _T5F;struct _fat_ptr _T60;struct _fat_ptr _T61;
if(Cyc_PP_bhashtbl!=0)goto _TLB;{struct Cyc_Core_Opt*_T62=_cycalloc(sizeof(struct Cyc_Core_Opt));_T2=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T63)(int,int(*)(int,int),int(*)(int))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(int,int),int(*)(int)))_T2;_T1=_T63;}_T3=Cyc_Core_intcmp;_T5=Cyc_Core_identity;{int(*_T63)(int)=(int(*)(int))_T5;_T4=_T63;}_T62->v=_T1(61,_T3,_T4);_T0=(struct Cyc_Core_Opt*)_T62;}Cyc_PP_bhashtbl=_T0;goto _TLC;_TLB: _TLC:
 if(i >= 0)goto _TLD;{struct Cyc_Core_Failure_exn_struct*_T62=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_T62->tag=Cyc_Core_Failure;_T62->f1=_tag_fat("nlblanks",sizeof(char),9U);_T6=(struct Cyc_Core_Failure_exn_struct*)_T62;}_T7=(void*)_T6;_throw(_T7);goto _TLE;_TLD: _TLE: {struct _handler_cons _T62;_T8=& _T62;_push_handler(_T8);{int _T63=0;_T9=setjmp(_T62.handler);if(!_T9)goto _TLF;_T63=1;goto _TL10;_TLF: _TL10: if(_T63)goto _TL11;else{goto _TL13;}_TL13: _TB=Cyc_Hashtable_lookup;{
# 149
struct _fat_ptr*(*_T64)(struct Cyc_Hashtable_Table*,int)=(struct _fat_ptr*(*)(struct Cyc_Hashtable_Table*,int))_TB;_TA=_T64;}_TC=Cyc_PP_bhashtbl;_TD=_TC->v;_TE=(struct Cyc_Hashtable_Table*)_TD;_TF=i;_T10=_TA(_TE,_TF);{struct _fat_ptr _T64=*_T10;_npop_handler(0);return _T64;}_pop_handler();goto _TL12;_TL11: _T11=Cyc_Core_get_exn_thrown();{void*_T64=(void*)_T11;void*_T65;_T12=(struct Cyc_Core_Not_found_exn_struct*)_T64;_T13=_T12->tag;_T14=Cyc_Core_Not_found;if(_T13!=_T14)goto _TL14;_T15=Cyc_PP_tex_output;
# 152
if(_T15)goto _TL16;else{goto _TL18;}
_TL18:{int num_tabs=i / 8;
int num_spaces=i % 8;_T16=2 + num_tabs;_T17=num_spaces;{
int total=_T16 + _T17;_T18=total;_T19=(unsigned)_T18;{
struct _fat_ptr nlb=Cyc_Core_new_string(_T19);_T1A=nlb;{struct _fat_ptr _T66=_fat_ptr_plus(_T1A,sizeof(char),0);_T1B=_check_fat_subscript(_T66,sizeof(char),0U);_T1C=(char*)_T1B;{char _T67=*_T1C;char _T68='\n';_T1D=_get_fat_size(_T66,sizeof(char));if(_T1D!=1U)goto _TL19;if(_T67!=0)goto _TL19;if(_T68==0)goto _TL19;_throw_arraybounds();goto _TL1A;_TL19: _TL1A: _T1E=_T66.curr;_T1F=(char*)_T1E;*_T1F=_T68;}}{
# 158
int j=0;_TL1E: if(j < num_tabs)goto _TL1C;else{goto _TL1D;}
_TL1C: _T20=nlb;_T21=j + 1;{struct _fat_ptr _T66=_fat_ptr_plus(_T20,sizeof(char),_T21);_T22=_check_fat_subscript(_T66,sizeof(char),0U);_T23=(char*)_T22;{char _T67=*_T23;char _T68='\t';_T24=_get_fat_size(_T66,sizeof(char));if(_T24!=1U)goto _TL1F;if(_T67!=0)goto _TL1F;if(_T68==0)goto _TL1F;_throw_arraybounds();goto _TL20;_TL1F: _TL20: _T25=_T66.curr;_T26=(char*)_T25;*_T26=_T68;}}
# 158
j=j + 1;goto _TL1E;_TL1D:;}{
# 160
int j=0;_TL24: if(j < num_spaces)goto _TL22;else{goto _TL23;}
_TL22: _T27=nlb;_T28=j + 1;_T29=num_tabs;_T2A=_T28 + _T29;{struct _fat_ptr _T66=_fat_ptr_plus(_T27,sizeof(char),_T2A);_T2B=_check_fat_subscript(_T66,sizeof(char),0U);_T2C=(char*)_T2B;{char _T67=*_T2C;char _T68=' ';_T2D=_get_fat_size(_T66,sizeof(char));if(_T2D!=1U)goto _TL25;if(_T67!=0)goto _TL25;if(_T68==0)goto _TL25;_throw_arraybounds();goto _TL26;_TL25: _TL26: _T2E=_T66.curr;_T2F=(char*)_T2E;*_T2F=_T68;}}
# 160
j=j + 1;goto _TL24;_TL23:;}_T31=Cyc_Hashtable_insert;{
# 162
void(*_T66)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*)=(void(*)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*))_T31;_T30=_T66;}_T32=_check_null(Cyc_PP_bhashtbl);_T33=_T32->v;_T34=(struct Cyc_Hashtable_Table*)_T33;_T35=i;{unsigned _T66=1;_T38=_check_times(_T66,sizeof(struct _fat_ptr));{struct _fat_ptr*_T67=_cycalloc(_T38);_T39=nlb;_T67[0]=_T39;_T37=(struct _fat_ptr*)_T67;}_T36=_T37;}_T30(_T34,_T35,_T36);_T3A=nlb;
return _T3A;}}}
# 168
_TL16:{int total=3 + i;_T3B=total + 1;_T3C=(unsigned)_T3B;{
struct _fat_ptr nlb=Cyc_Core_new_string(_T3C);_T3D=nlb;{struct _fat_ptr _T66=_fat_ptr_plus(_T3D,sizeof(char),0);_T3E=_T66.curr;_T3F=(char*)_T3E;_T40=_check_null(_T3F);{char _T67=*_T40;char _T68='\\';_T41=_get_fat_size(_T66,sizeof(char));if(_T41!=1U)goto _TL27;if(_T67!=0)goto _TL27;if(_T68==0)goto _TL27;_throw_arraybounds();goto _TL28;_TL27: _TL28: _T42=_T66.curr;_T43=(char*)_T42;*_T43=_T68;}}_T44=nlb;{struct _fat_ptr _T66=_fat_ptr_plus(_T44,sizeof(char),1);_T45=_T66.curr;_T46=(char*)_T45;{char _T67=*_T46;char _T68='\\';_T47=_get_fat_size(_T66,sizeof(char));if(_T47!=1U)goto _TL29;if(_T67!=0)goto _TL29;if(_T68==0)goto _TL29;_throw_arraybounds();goto _TL2A;_TL29: _TL2A: _T48=_T66.curr;_T49=(char*)_T48;*_T49=_T68;}}_T4A=nlb;{struct _fat_ptr _T66=_fat_ptr_plus(_T4A,sizeof(char),2);_T4B=_T66.curr;_T4C=(char*)_T4B;{char _T67=*_T4C;char _T68='\n';_T4D=_get_fat_size(_T66,sizeof(char));if(_T4D!=1U)goto _TL2B;if(_T67!=0)goto _TL2B;if(_T68==0)goto _TL2B;_throw_arraybounds();goto _TL2C;_TL2B: _TL2C: _T4E=_T66.curr;_T4F=(char*)_T4E;*_T4F=_T68;}}{
# 173
int j=3;_TL30: if(j < total)goto _TL2E;else{goto _TL2F;}
_TL2E: _T50=nlb;_T51=j;{struct _fat_ptr _T66=_fat_ptr_plus(_T50,sizeof(char),_T51);_T52=_check_fat_subscript(_T66,sizeof(char),0U);_T53=(char*)_T52;{char _T67=*_T53;char _T68='~';_T54=_get_fat_size(_T66,sizeof(char));if(_T54!=1U)goto _TL31;if(_T67!=0)goto _TL31;if(_T68==0)goto _TL31;_throw_arraybounds();goto _TL32;_TL31: _TL32: _T55=_T66.curr;_T56=(char*)_T55;*_T56=_T68;}}
# 173
j=j + 1;goto _TL30;_TL2F:;}_T58=Cyc_Hashtable_insert;{
# 175
void(*_T66)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*)=(void(*)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*))_T58;_T57=_T66;}_T59=_check_null(Cyc_PP_bhashtbl);_T5A=_T59->v;_T5B=(struct Cyc_Hashtable_Table*)_T5A;_T5C=i;{unsigned _T66=1;_T5F=_check_times(_T66,sizeof(struct _fat_ptr));{struct _fat_ptr*_T67=_cycalloc(_T5F);_T60=nlb;_T67[0]=_T60;_T5E=(struct _fat_ptr*)_T67;}_T5D=_T5E;}_T57(_T5B,_T5C,_T5D);_T61=nlb;
return _T61;}}_TL14: _T65=_T64;{void*exn=_T65;_rethrow(exn);};}_TL12:;}}}
# 182
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;
# 184
int Cyc_PP_infinity=9999999;struct _tuple2{int f0;struct _fat_ptr f1;};
# 186
static struct Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*st){struct _tuple2*_T0;struct Cyc_PP_Out*_T1;struct Cyc_PP_Ppstate*_T2;int _T3;int _T4;struct Cyc_PP_Ppstate*_T5;struct Cyc_PP_Single_PP_Alist_struct*_T6;struct _fat_ptr*_T7;struct _fat_ptr*_T8;unsigned _T9;struct Cyc_PP_Empty_PP_Alist_struct*_TA;struct Cyc_PP_Empty_PP_Alist_struct*_TB;struct _fat_ptr _TC;int _TD;_T0=clo;{struct _tuple2 _TE=*_T0;_TD=_TE.f0;_TC=_TE.f1;}{int slen=_TD;struct _fat_ptr s=_TC;{struct Cyc_PP_Out*_TE=_cycalloc(sizeof(struct Cyc_PP_Out));_T2=st;_T3=_T2->cc;_T4=slen;
# 188
_TE->newcc=_T3 + _T4;_T5=st;
_TE->newcl=_T5->cl;{struct Cyc_PP_Single_PP_Alist_struct*_TF=_cycalloc(sizeof(struct Cyc_PP_Single_PP_Alist_struct));_TF->tag=1;{unsigned _T10=1;_T9=_check_times(_T10,sizeof(struct _fat_ptr));{struct _fat_ptr*_T11=_cycalloc(_T9);
_T11[0]=s;_T8=(struct _fat_ptr*)_T11;}_T7=_T8;}_TF->f1=_T7;_T6=(struct Cyc_PP_Single_PP_Alist_struct*)_TF;}_TE->ppout=(void*)_T6;_TA=& Cyc_PP_Empty_link;_TB=(struct Cyc_PP_Empty_PP_Alist_struct*)_TA;
_TE->links=(void*)_TB;_T1=(struct Cyc_PP_Out*)_TE;}
# 188
return _T1;}}
# 193
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _fat_ptr s){unsigned long _T0;struct Cyc_PP_Doc*_T1;struct Cyc_Fn_Function*(*_T2)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*);struct Cyc_Fn_Function*(*_T3)(void*(*)(void*,void*),void*);struct _tuple2*_T4;_T0=
Cyc_strlen(s);{int slen=(int)_T0;{struct Cyc_PP_Doc*_T5=_cycalloc(sizeof(struct Cyc_PP_Doc));
# 196
_T5->mwo=slen;
_T5->mw=Cyc_PP_infinity;_T3=Cyc_Fn_make_fn;{
struct Cyc_Fn_Function*(*_T6)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*))_T3;_T2=_T6;}{struct _tuple2*_T6=_cycalloc(sizeof(struct _tuple2));
# 200
_T6->f0=slen;_T6->f1=s;_T4=(struct _tuple2*)_T6;}
# 198
_T5->f=_T2(Cyc_PP_text_doc_f,_T4);_T1=(struct Cyc_PP_Doc*)_T5;}
# 195
return _T1;}}
# 202
struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr s){struct Cyc_Hashtable_Table*(*_T0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T1)(int,int(*)(void*,void*),int(*)(void*));int(*_T2)(struct _fat_ptr*,struct _fat_ptr*);int(*_T3)(struct _fat_ptr*);struct Cyc_Core_Opt*_T4;struct Cyc_Core_Opt*_T5;void*_T6;struct _handler_cons*_T7;int _T8;struct Cyc_PP_Doc*(*_T9)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_TA)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TB;struct _fat_ptr*_TC;void*_TD;struct Cyc_Core_Not_found_exn_struct*_TE;char*_TF;char*_T10;void(*_T11)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*);void(*_T12)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T13;struct _fat_ptr*_T14;struct Cyc_PP_Doc*_T15;struct Cyc_PP_Doc*_T16;
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl!=0)goto _TL33;_T1=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T17)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T1;_T0=_T17;}_T2=Cyc_zstrptrcmp;_T3=Cyc_Hashtable_hash_stringptr;t=_T0(101,_T2,_T3);{struct Cyc_Core_Opt*_T17=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T17->v=t;_T4=(struct Cyc_Core_Opt*)_T17;}Cyc_PP_str_hashtbl=_T4;goto _TL34;
# 208
_TL33: _T5=Cyc_PP_str_hashtbl;_T6=_T5->v;t=(struct Cyc_Hashtable_Table*)_T6;_TL34: {struct _handler_cons _T17;_T7=& _T17;_push_handler(_T7);{int _T18=0;_T8=setjmp(_T17.handler);if(!_T8)goto _TL35;_T18=1;goto _TL36;_TL35: _TL36: if(_T18)goto _TL37;else{goto _TL39;}_TL39: _TA=Cyc_Hashtable_lookup;{
# 211
struct Cyc_PP_Doc*(*_T19)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_TA;_T9=_T19;}_TB=t;{struct _fat_ptr*_T19=_cycalloc(sizeof(struct _fat_ptr));*_T19=s;_TC=(struct _fat_ptr*)_T19;}{struct Cyc_PP_Doc*_T19=_T9(_TB,_TC);_npop_handler(0);return _T19;}_pop_handler();goto _TL38;_TL37: _TD=Cyc_Core_get_exn_thrown();{void*_T19=(void*)_TD;void*_T1A;_TE=(struct Cyc_Core_Not_found_exn_struct*)_T19;_TF=_TE->tag;_T10=Cyc_Core_Not_found;if(_TF!=_T10)goto _TL3A;{
# 214
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);_T12=Cyc_Hashtable_insert;{
void(*_T1B)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*))_T12;_T11=_T1B;}_T13=t;{struct _fat_ptr*_T1B=_cycalloc(sizeof(struct _fat_ptr));*_T1B=s;_T14=(struct _fat_ptr*)_T1B;}_T15=d;_T11(_T13,_T14,_T15);_T16=d;
return _T16;}_TL3A: _T1A=_T19;{void*exn=_T1A;_rethrow(exn);};}_TL38:;}}}
# 219
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _fat_ptr*s){struct _fat_ptr*_T0;struct _fat_ptr _T1;struct Cyc_PP_Doc*_T2;_T0=s;_T1=*_T0;_T2=Cyc_PP_text(_T1);return _T2;}
# 224
static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _fat_ptr s,int slen){struct Cyc_PP_Doc*_T0;struct Cyc_Fn_Function*(*_T1)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*);struct Cyc_Fn_Function*(*_T2)(void*(*)(void*,void*),void*);struct _tuple2*_T3;{struct Cyc_PP_Doc*_T4=_cycalloc(sizeof(struct Cyc_PP_Doc));
# 226
_T4->mwo=slen;
_T4->mw=Cyc_PP_infinity;_T2=Cyc_Fn_make_fn;{
struct Cyc_Fn_Function*(*_T5)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*))_T2;_T1=_T5;}{struct _tuple2*_T5=_cycalloc(sizeof(struct _tuple2));
# 230
_T5->f0=slen;_T5->f1=s;_T3=(struct _tuple2*)_T5;}
# 228
_T4->f=_T1(Cyc_PP_text_doc_f,_T3);_T0=(struct Cyc_PP_Doc*)_T4;}
# 225
return _T0;}
# 232
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _fat_ptr s,int slen){struct Cyc_Hashtable_Table*(*_T0)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*));struct Cyc_Hashtable_Table*(*_T1)(int,int(*)(void*,void*),int(*)(void*));int(*_T2)(struct _fat_ptr*,struct _fat_ptr*);int(*_T3)(struct _fat_ptr*);struct Cyc_Core_Opt*_T4;struct Cyc_Core_Opt*_T5;void*_T6;struct _handler_cons*_T7;int _T8;struct Cyc_PP_Doc*(*_T9)(struct Cyc_Hashtable_Table*,struct _fat_ptr*);void*(*_TA)(struct Cyc_Hashtable_Table*,void*);struct Cyc_Hashtable_Table*_TB;struct _fat_ptr*_TC;void*_TD;struct Cyc_Core_Not_found_exn_struct*_TE;char*_TF;char*_T10;void(*_T11)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*);void(*_T12)(struct Cyc_Hashtable_Table*,void*,void*);struct Cyc_Hashtable_Table*_T13;struct _fat_ptr*_T14;struct Cyc_PP_Doc*_T15;struct Cyc_PP_Doc*_T16;
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl2!=0)goto _TL3C;_T1=Cyc_Hashtable_create;{
struct Cyc_Hashtable_Table*(*_T17)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*))=(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))_T1;_T0=_T17;}_T2=Cyc_zstrptrcmp;_T3=Cyc_Hashtable_hash_stringptr;t=_T0(101,_T2,_T3);{struct Cyc_Core_Opt*_T17=_cycalloc(sizeof(struct Cyc_Core_Opt));
_T17->v=t;_T4=(struct Cyc_Core_Opt*)_T17;}Cyc_PP_str_hashtbl2=_T4;goto _TL3D;
# 239
_TL3C: _T5=Cyc_PP_str_hashtbl2;_T6=_T5->v;t=(struct Cyc_Hashtable_Table*)_T6;_TL3D: {struct _handler_cons _T17;_T7=& _T17;_push_handler(_T7);{int _T18=0;_T8=setjmp(_T17.handler);if(!_T8)goto _TL3E;_T18=1;goto _TL3F;_TL3E: _TL3F: if(_T18)goto _TL40;else{goto _TL42;}_TL42: _TA=Cyc_Hashtable_lookup;{
# 242
struct Cyc_PP_Doc*(*_T19)(struct Cyc_Hashtable_Table*,struct _fat_ptr*)=(struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))_TA;_T9=_T19;}_TB=t;{struct _fat_ptr*_T19=_cycalloc(sizeof(struct _fat_ptr));*_T19=s;_TC=(struct _fat_ptr*)_T19;}{struct Cyc_PP_Doc*_T19=_T9(_TB,_TC);_npop_handler(0);return _T19;}_pop_handler();goto _TL41;_TL40: _TD=Cyc_Core_get_exn_thrown();{void*_T19=(void*)_TD;void*_T1A;_TE=(struct Cyc_Core_Not_found_exn_struct*)_T19;_TF=_TE->tag;_T10=Cyc_Core_Not_found;if(_TF!=_T10)goto _TL43;{
# 245
struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(s,slen);_T12=Cyc_Hashtable_insert;{
void(*_T1B)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*)=(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*))_T12;_T11=_T1B;}_T13=t;{struct _fat_ptr*_T1B=_cycalloc(sizeof(struct _fat_ptr));*_T1B=s;_T14=(struct _fat_ptr*)_T1B;}_T15=d;_T11(_T13,_T14,_T15);_T16=d;
return _T16;}_TL43: _T1A=_T19;{void*exn=_T1A;_rethrow(exn);};}_TL41:;}}}
# 251
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;
# 257
struct Cyc_PP_Doc*Cyc_PP_nil_doc (void){struct Cyc_Core_Opt*_T0;struct _fat_ptr _T1;struct Cyc_Core_Opt*_T2;void*_T3;struct Cyc_PP_Doc*_T4;
if(Cyc_PP_nil_doc_opt!=0)goto _TL45;{struct Cyc_Core_Opt*_T5=_cycalloc(sizeof(struct Cyc_Core_Opt));_T1=
_tag_fat("",sizeof(char),1U);_T5->v=Cyc_PP_text(_T1);_T0=(struct Cyc_Core_Opt*)_T5;}Cyc_PP_nil_doc_opt=_T0;goto _TL46;_TL45: _TL46: _T2=Cyc_PP_nil_doc_opt;_T3=_T2->v;_T4=(struct Cyc_PP_Doc*)_T3;
return _T4;}
# 264
struct Cyc_PP_Doc*Cyc_PP_blank_doc (void){struct Cyc_Core_Opt*_T0;struct _fat_ptr _T1;struct Cyc_Core_Opt*_T2;void*_T3;struct Cyc_PP_Doc*_T4;
if(Cyc_PP_blank_doc_opt!=0)goto _TL47;{struct Cyc_Core_Opt*_T5=_cycalloc(sizeof(struct Cyc_Core_Opt));_T1=
_tag_fat(" ",sizeof(char),2U);_T5->v=Cyc_PP_text(_T1);_T0=(struct Cyc_Core_Opt*)_T5;}Cyc_PP_blank_doc_opt=_T0;goto _TL48;_TL47: _TL48: _T2=Cyc_PP_blank_doc_opt;_T3=_T2->v;_T4=(struct Cyc_PP_Doc*)_T3;
return _T4;}struct _tuple3{int f0;struct _fat_ptr f1;struct _fat_ptr f2;};struct _tuple4{int f0;int f1;int f2;struct _fat_ptr f3;};
# 270
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple3*clo,struct Cyc_PP_Ppstate*st){struct _tuple3*_T0;struct Cyc_PP_Out*_T1;struct Cyc_PP_Ppstate*_T2;int _T3;int _T4;struct Cyc_PP_Ppstate*_T5;struct Cyc_PP_Single_PP_Alist_struct*_T6;struct _fat_ptr*_T7;struct _fat_ptr*_T8;unsigned _T9;struct Cyc_PP_Single_PP_Alist_struct*_TA;struct _tuple4*_TB;struct Cyc_PP_Ppstate*_TC;struct Cyc_PP_Ppstate*_TD;struct _fat_ptr _TE;struct _fat_ptr _TF;int _T10;_T0=clo;{struct _tuple3 _T11=*_T0;_T10=_T11.f0;_TF=_T11.f1;_TE=_T11.f2;}{int slen=_T10;struct _fat_ptr shrt=_TF;struct _fat_ptr full=_TE;{struct Cyc_PP_Out*_T11=_cycalloc(sizeof(struct Cyc_PP_Out));_T2=st;_T3=_T2->cc;_T4=slen;
# 272
_T11->newcc=_T3 + _T4;_T5=st;
_T11->newcl=_T5->cl;{struct Cyc_PP_Single_PP_Alist_struct*_T12=_cycalloc(sizeof(struct Cyc_PP_Single_PP_Alist_struct));_T12->tag=1;{unsigned _T13=1;_T9=_check_times(_T13,sizeof(struct _fat_ptr));{struct _fat_ptr*_T14=_cycalloc(_T9);
_T14[0]=shrt;_T8=(struct _fat_ptr*)_T14;}_T7=_T8;}_T12->f1=_T7;_T6=(struct Cyc_PP_Single_PP_Alist_struct*)_T12;}_T11->ppout=(void*)_T6;{struct Cyc_PP_Single_PP_Alist_struct*_T12=_cycalloc(sizeof(struct Cyc_PP_Single_PP_Alist_struct));_T12->tag=1;{struct _tuple4*_T13=_cycalloc(sizeof(struct _tuple4));_TC=st;
_T13->f0=_TC->cl;_TD=st;_T13->f1=_TD->cc;_T13->f2=slen;_T13->f3=full;_TB=(struct _tuple4*)_T13;}_T12->f1=_TB;_TA=(struct Cyc_PP_Single_PP_Alist_struct*)_T12;}_T11->links=(void*)_TA;_T1=(struct Cyc_PP_Out*)_T11;}
# 272
return _T1;}}
# 277
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _fat_ptr shrt,struct _fat_ptr full){unsigned long _T0;struct Cyc_PP_Doc*_T1;struct Cyc_Fn_Function*(*_T2)(struct Cyc_PP_Out*(*)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*);struct Cyc_Fn_Function*(*_T3)(void*(*)(void*,void*),void*);struct _tuple3*_T4;_T0=
Cyc_strlen(shrt);{int slen=(int)_T0;{struct Cyc_PP_Doc*_T5=_cycalloc(sizeof(struct Cyc_PP_Doc));
_T5->mwo=slen;
_T5->mw=Cyc_PP_infinity;_T3=Cyc_Fn_make_fn;{
struct Cyc_Fn_Function*(*_T6)(struct Cyc_PP_Out*(*)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*))_T3;_T2=_T6;}{struct _tuple3*_T6=_cycalloc(sizeof(struct _tuple3));_T6->f0=slen;_T6->f1=shrt;_T6->f2=full;_T4=(struct _tuple3*)_T6;}_T5->f=_T2(Cyc_PP_hyperlink_f,_T4);_T1=(struct Cyc_PP_Doc*)_T5;}
# 279
return _T1;}}
# 285
static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){struct Cyc_PP_Out*_T0;struct Cyc_PP_Ppstate*_T1;struct Cyc_PP_Ppstate*_T2;int _T3;struct Cyc_PP_Single_PP_Alist_struct*_T4;struct _fat_ptr*_T5;struct _fat_ptr*_T6;unsigned _T7;struct Cyc_PP_Ppstate*_T8;int _T9;struct Cyc_PP_Empty_PP_Alist_struct*_TA;struct Cyc_PP_Empty_PP_Alist_struct*_TB;{struct Cyc_PP_Out*_TC=_cycalloc(sizeof(struct Cyc_PP_Out));_T1=st;
_TC->newcc=_T1->ci;_T2=st;_T3=_T2->cl;
_TC->newcl=_T3 + 1;{struct Cyc_PP_Single_PP_Alist_struct*_TD=_cycalloc(sizeof(struct Cyc_PP_Single_PP_Alist_struct));_TD->tag=1;{unsigned _TE=1;_T7=_check_times(_TE,sizeof(struct _fat_ptr));{struct _fat_ptr*_TF=_cycalloc(_T7);_T8=st;_T9=_T8->ci;
_TF[0]=Cyc_PP_nlblanks(_T9);_T6=(struct _fat_ptr*)_TF;}_T5=_T6;}_TD->f1=_T5;_T4=(struct Cyc_PP_Single_PP_Alist_struct*)_TD;}_TC->ppout=(void*)_T4;_TA=& Cyc_PP_Empty_link;_TB=(struct Cyc_PP_Empty_PP_Alist_struct*)_TA;
_TC->links=(void*)_TB;_T0=(struct Cyc_PP_Out*)_TC;}
# 286
return _T0;}
# 291
struct Cyc_PP_Doc*Cyc_PP_line_doc (void){struct Cyc_Core_Opt*_T0;struct Cyc_PP_Doc*_T1;struct Cyc_Fn_Function*(*_T2)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Ppstate*));struct Cyc_Fn_Function*(*_T3)(void*(*)(void*));struct Cyc_Core_Opt*_T4;void*_T5;struct Cyc_PP_Doc*_T6;
if(Cyc_PP_line_doc_opt!=0)goto _TL49;{struct Cyc_Core_Opt*_T7=_cycalloc(sizeof(struct Cyc_Core_Opt));{struct Cyc_PP_Doc*_T8=_cycalloc(sizeof(struct Cyc_PP_Doc));
_T8->mwo=0;_T8->mw=0;_T3=Cyc_Fn_fp2fn;{struct Cyc_Fn_Function*(*_T9)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Ppstate*))=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Ppstate*)))_T3;_T2=_T9;}_T8->f=_T2(Cyc_PP_line_f);_T1=(struct Cyc_PP_Doc*)_T8;}_T7->v=_T1;_T0=(struct Cyc_Core_Opt*)_T7;}Cyc_PP_line_doc_opt=_T0;goto _TL4A;_TL49: _TL4A: _T4=Cyc_PP_line_doc_opt;_T5=_T4->v;_T6=(struct Cyc_PP_Doc*)_T5;
return _T6;}struct _tuple5{int f0;struct Cyc_PP_Doc*f1;};
# 297
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*st){struct _tuple5*_T0;struct Cyc_PP_Ppstate*_T1;struct Cyc_PP_Ppstate*_T2;int _T3;int _T4;struct Cyc_PP_Ppstate*_T5;struct Cyc_PP_Ppstate*_T6;struct Cyc_PP_Ppstate*_T7;struct Cyc_PP_Ppstate*_T8;struct Cyc_PP_Ppstate*_T9;struct Cyc_PP_Ppstate*_TA;struct Cyc_PP_Ppstate*_TB;struct Cyc_PP_Ppstate*_TC;struct Cyc_PP_Out*(*_TD)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_TE)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_TF;struct Cyc_Fn_Function*_T10;struct Cyc_PP_Ppstate*_T11;struct Cyc_PP_Out*_T12;struct Cyc_PP_Doc*_T13;int _T14;_T0=clo;{struct _tuple5 _T15=*_T0;_T14=_T15.f0;_T13=_T15.f1;}{int k=_T14;struct Cyc_PP_Doc*d=_T13;
# 299
struct Cyc_PP_Ppstate*st2;st2=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_T1=st2;_T2=st;_T3=_T2->ci;_T4=k;
_T1->ci=_T3 + _T4;_T5=st2;_T6=st;
_T5->cc=_T6->cc;_T7=st2;_T8=st;
_T7->cl=_T8->cl;_T9=st2;_TA=st;
_T9->pw=_TA->pw;_TB=st2;_TC=st;
_TB->epw=_TC->epw;_TE=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_T15)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_TE;_TD=_T15;}_TF=d;_T10=_TF->f;_T11=st2;_T12=_TD(_T10,_T11);return _T12;}}
# 307
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){struct Cyc_PP_Doc*_T0;struct Cyc_PP_Doc*_T1;struct Cyc_PP_Doc*_T2;struct Cyc_Fn_Function*(*_T3)(struct Cyc_PP_Out*(*)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*);struct Cyc_Fn_Function*(*_T4)(void*(*)(void*,void*),void*);struct _tuple5*_T5;{struct Cyc_PP_Doc*_T6=_cycalloc(sizeof(struct Cyc_PP_Doc));_T1=d;
_T6->mwo=_T1->mwo;_T2=d;
_T6->mw=_T2->mw;_T4=Cyc_Fn_make_fn;{
struct Cyc_Fn_Function*(*_T7)(struct Cyc_PP_Out*(*)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*))_T4;_T3=_T7;}{struct _tuple5*_T7=_cycalloc(sizeof(struct _tuple5));
# 312
_T7->f0=k;_T7->f1=d;_T5=(struct _tuple5*)_T7;}
# 310
_T6->f=_T3(Cyc_PP_nest_f,_T5);_T0=(struct Cyc_PP_Doc*)_T6;}
# 308
return _T0;}
# 315
int Cyc_PP_min(int x,int y){int _T0;int _T1;
if(x > y)goto _TL4B;_T0=x;return _T0;
_TL4B: _T1=y;return _T1;}
# 320
int Cyc_PP_max(int x,int y){int _T0;int _T1;
if(x < y)goto _TL4D;_T0=x;return _T0;
_TL4D: _T1=y;return _T1;}struct _tuple6{struct Cyc_PP_Doc*f0;struct Cyc_PP_Doc*f1;};
# 325
static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){struct _tuple6*_T0;struct Cyc_PP_Ppstate*_T1;int _T2;struct Cyc_PP_Doc*_T3;int _T4;int _T5;struct Cyc_PP_Ppstate*_T6;int _T7;struct Cyc_PP_Doc*_T8;int _T9;int _TA;struct Cyc_PP_Ppstate*_TB;struct Cyc_PP_Ppstate*_TC;struct Cyc_PP_Ppstate*_TD;struct Cyc_PP_Ppstate*_TE;struct Cyc_PP_Ppstate*_TF;struct Cyc_PP_Ppstate*_T10;struct Cyc_PP_Ppstate*_T11;struct Cyc_PP_Ppstate*_T12;struct Cyc_PP_Ppstate*_T13;struct Cyc_PP_Out*(*_T14)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T15)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T16;struct Cyc_Fn_Function*_T17;struct Cyc_PP_Ppstate*_T18;struct Cyc_PP_Ppstate*_T19;struct Cyc_PP_Ppstate*_T1A;struct Cyc_PP_Ppstate*_T1B;struct Cyc_PP_Out*_T1C;struct Cyc_PP_Ppstate*_T1D;struct Cyc_PP_Out*_T1E;struct Cyc_PP_Ppstate*_T1F;struct Cyc_PP_Ppstate*_T20;struct Cyc_PP_Ppstate*_T21;struct Cyc_PP_Out*(*_T22)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T23)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T24;struct Cyc_Fn_Function*_T25;struct Cyc_PP_Ppstate*_T26;struct Cyc_PP_Out*_T27;struct Cyc_PP_Out*_T28;struct Cyc_PP_Out*_T29;struct Cyc_PP_Out*_T2A;struct Cyc_PP_Out*_T2B;struct Cyc_PP_Out*_T2C;void*_T2D;struct Cyc_PP_Out*_T2E;void*_T2F;struct Cyc_PP_Out*_T30;struct Cyc_PP_Out*_T31;void*_T32;struct Cyc_PP_Out*_T33;void*_T34;struct Cyc_PP_Out*_T35;struct Cyc_PP_Doc*_T36;struct Cyc_PP_Doc*_T37;_T0=clo;{struct _tuple6 _T38=*_T0;_T37=_T38.f0;_T36=_T38.f1;}{struct Cyc_PP_Doc*d1=_T37;struct Cyc_PP_Doc*d2=_T36;_T1=st;_T2=_T1->pw;_T3=d2;_T4=_T3->mw;_T5=_T2 - _T4;_T6=st;_T7=_T6->epw;_T8=d1;_T9=_T8->mwo;_TA=_T7 - _T9;{
# 327
int epw2=Cyc_PP_max(_T5,_TA);
struct Cyc_PP_Ppstate*st2;st2=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_TB=st2;_TC=st;_TB->ci=_TC->ci;_TD=st2;_TE=st;_TD->cc=_TE->cc;_TF=st2;_T10=st;_TF->cl=_T10->cl;_T11=st2;_T12=st;_T11->pw=_T12->pw;_T13=st2;_T13->epw=epw2;_T15=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_T38)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T15;_T14=_T38;}_T16=d1;_T17=_T16->f;_T18=st2;{struct Cyc_PP_Out*o1=_T14(_T17,_T18);
struct Cyc_PP_Ppstate*st3;st3=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_T19=st3;_T1A=st;_T19->ci=_T1A->ci;_T1B=st3;_T1C=o1;_T1B->cc=_T1C->newcc;_T1D=st3;_T1E=o1;_T1D->cl=_T1E->newcl;_T1F=st3;_T20=st;_T1F->pw=_T20->pw;_T21=st3;_T21->epw=epw2;_T23=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_T38)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T23;_T22=_T38;}_T24=d2;_T25=_T24->f;_T26=st3;{struct Cyc_PP_Out*o2=_T22(_T25,_T26);
struct Cyc_PP_Out*o3;o3=_cycalloc(sizeof(struct Cyc_PP_Out));_T27=o3;_T28=o2;_T27->newcc=_T28->newcc;_T29=o3;_T2A=o2;
_T29->newcl=_T2A->newcl;_T2B=o3;_T2C=o1;_T2D=_T2C->ppout;_T2E=o2;_T2F=_T2E->ppout;
_T2B->ppout=Cyc_PP_append(_T2D,_T2F);_T30=o3;_T31=o1;_T32=_T31->links;_T33=o2;_T34=_T33->links;
_T30->links=Cyc_PP_append(_T32,_T34);_T35=o3;
return _T35;}}}}}
# 338
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){struct Cyc_PP_Doc*_T0;struct Cyc_PP_Doc*_T1;int _T2;struct Cyc_PP_Doc*_T3;int _T4;struct Cyc_PP_Doc*_T5;int _T6;int _T7;struct Cyc_PP_Doc*_T8;int _T9;struct Cyc_PP_Doc*_TA;int _TB;struct Cyc_PP_Doc*_TC;int _TD;int _TE;struct Cyc_Fn_Function*(*_TF)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*);struct Cyc_Fn_Function*(*_T10)(void*(*)(void*,void*),void*);struct _tuple6*_T11;{struct Cyc_PP_Doc*_T12=_cycalloc(sizeof(struct Cyc_PP_Doc));_T1=d1;_T2=_T1->mw;_T3=d1;_T4=_T3->mwo;_T5=d2;_T6=_T5->mwo;_T7=_T4 + _T6;
_T12->mwo=Cyc_PP_min(_T2,_T7);_T8=d1;_T9=_T8->mw;_TA=d1;_TB=_TA->mwo;_TC=d2;_TD=_TC->mw;_TE=_TB + _TD;
_T12->mw=Cyc_PP_min(_T9,_TE);_T10=Cyc_Fn_make_fn;{
struct Cyc_Fn_Function*(*_T13)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*))_T10;_TF=_T13;}{struct _tuple6*_T13=_cycalloc(sizeof(struct _tuple6));_T13->f0=d1;_T13->f1=d2;_T11=(struct _tuple6*)_T13;}_T12->f=_TF(Cyc_PP_concat_f,_T11);_T0=(struct Cyc_PP_Doc*)_T12;}
# 339
return _T0;}
# 343
struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr l){struct _fat_ptr _T0;unsigned _T1;unsigned _T2;struct _fat_ptr _T3;int _T4;unsigned char*_T5;struct Cyc_PP_Doc**_T6;struct Cyc_PP_Doc*_T7;struct Cyc_PP_Doc*_T8;struct Cyc_PP_Doc*_T9;
struct Cyc_PP_Doc*d=Cyc_PP_nil_doc();_T0=l;_T1=
_get_fat_size(_T0,sizeof(struct Cyc_PP_Doc*));_T2=_T1 - 1U;{int i=(int)_T2;_TL52: if(i >= 0)goto _TL50;else{goto _TL51;}
_TL50: _T3=l;_T4=i;_T5=_check_fat_subscript(_T3,sizeof(struct Cyc_PP_Doc*),_T4);_T6=(struct Cyc_PP_Doc**)_T5;_T7=*_T6;_T8=d;d=Cyc_PP_concat(_T7,_T8);
# 345
i=i + -1;goto _TL52;_TL51:;}_T9=d;
# 348
return _T9;}
# 353
static struct Cyc_PP_Out*Cyc_PP_long_cats_f(struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){struct Cyc_List_List*_T0;void*_T1;struct Cyc_PP_Out*(*_T2)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T3)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T4;struct Cyc_Fn_Function*_T5;struct Cyc_PP_Ppstate*_T6;struct Cyc_PP_Ppstate*_T7;struct Cyc_PP_Ppstate*_T8;struct Cyc_PP_Out*_T9;struct Cyc_PP_Out*_TA;struct Cyc_PP_Ppstate*_TB;struct Cyc_PP_Ppstate*_TC;int _TD;struct Cyc_PP_Doc*_TE;int _TF;struct Cyc_List_List*_T10;struct Cyc_List_List*_T11;struct Cyc_List_List*_T12;void*_T13;struct Cyc_PP_Out*_T14;struct Cyc_List_List*_T15;void*_T16;struct Cyc_PP_Out*_T17;struct Cyc_PP_Empty_PP_Alist_struct*_T18;struct Cyc_PP_Empty_PP_Alist_struct*_T19;struct Cyc_PP_Empty_PP_Alist_struct*_T1A;struct Cyc_PP_Empty_PP_Alist_struct*_T1B;struct Cyc_List_List*_T1C;void*_T1D;struct Cyc_PP_Out*_T1E;void*_T1F;void*_T20;struct Cyc_List_List*_T21;void*_T22;struct Cyc_PP_Out*_T23;void*_T24;void*_T25;struct Cyc_List_List*_T26;struct Cyc_PP_Out*_T27;
struct Cyc_List_List*os=0;{
struct Cyc_List_List*ds=ds0;_TL56: if(ds!=0)goto _TL54;else{goto _TL55;}
_TL54: _T0=ds;_T1=_T0->hd;{struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)_T1;_T3=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_T28)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T3;_T2=_T28;}_T4=d;_T5=_T4->f;_T6=st;{struct Cyc_PP_Out*o=_T2(_T5,_T6);{struct Cyc_PP_Ppstate*_T28=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_T8=st;
_T28->ci=_T8->ci;_T9=o;_T28->cc=_T9->newcc;_TA=o;_T28->cl=_TA->newcl;_TB=st;_T28->pw=_TB->pw;_TC=st;_TD=_TC->epw;_TE=d;_TF=_TE->mwo;_T28->epw=_TD - _TF;_T7=(struct Cyc_PP_Ppstate*)_T28;}st=_T7;{struct Cyc_List_List*_T28=_cycalloc(sizeof(struct Cyc_List_List));
_T28->hd=o;_T28->tl=os;_T10=(struct Cyc_List_List*)_T28;}os=_T10;}}_T11=ds;
# 355
ds=_T11->tl;goto _TL56;_TL55:;}_T12=
# 361
_check_null(os);_T13=_T12->hd;_T14=(struct Cyc_PP_Out*)_T13;{int newcc=_T14->newcc;_T15=os;_T16=_T15->hd;_T17=(struct Cyc_PP_Out*)_T16;{
int newcl=_T17->newcl;_T18=& Cyc_PP_Empty_stringptr;_T19=(struct Cyc_PP_Empty_PP_Alist_struct*)_T18;{
void*s=(void*)_T19;_T1A=& Cyc_PP_Empty_link;_T1B=(struct Cyc_PP_Empty_PP_Alist_struct*)_T1A;{
void*links=(void*)_T1B;
_TL5A: if(os!=0)goto _TL58;else{goto _TL59;}
_TL58: _T1C=os;_T1D=_T1C->hd;_T1E=(struct Cyc_PP_Out*)_T1D;_T1F=_T1E->ppout;_T20=s;s=Cyc_PP_append(_T1F,_T20);_T21=os;_T22=_T21->hd;_T23=(struct Cyc_PP_Out*)_T22;_T24=_T23->links;_T25=links;
links=Cyc_PP_append(_T24,_T25);_T26=os;
# 365
os=_T26->tl;goto _TL5A;_TL59:{struct Cyc_PP_Out*_T28=_cycalloc(sizeof(struct Cyc_PP_Out));
# 369
_T28->newcc=newcc;_T28->newcl=newcl;_T28->ppout=s;_T28->links=links;_T27=(struct Cyc_PP_Out*)_T28;}return _T27;}}}}}
# 371
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){struct Cyc_List_List*_T0;void*_T1;struct Cyc_List_List*_T2;struct Cyc_PP_Doc*_T3;struct Cyc_PP_Doc*_T4;int _T5;int _T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_PP_Doc*_T9;int _TA;int _TB;int _TC;int _TD;struct Cyc_List_List*_TE;void*_TF;struct Cyc_PP_Doc*_T10;int _T11;int _T12;struct Cyc_List_List*_T13;struct Cyc_PP_Doc*_T14;struct Cyc_Fn_Function*(*_T15)(struct Cyc_PP_Out*(*)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*);struct Cyc_Fn_Function*(*_T16)(void*(*)(void*,void*),void*);struct Cyc_List_List*_T17;
# 375
struct Cyc_List_List*orig=doclist;_T0=
_check_null(doclist);_T1=_T0->hd;{struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)_T1;_T2=doclist;
doclist=_T2->tl;_T3=d;{
int mw=_T3->mw;_T4=d;{
int mwo=_T4->mw;{
# 381
struct Cyc_List_List*ds=doclist;_TL5E: if(ds!=0)goto _TL5C;else{goto _TL5D;}
_TL5C: _T5=mw;_T6=mwo;_T7=ds;_T8=_T7->hd;_T9=(struct Cyc_PP_Doc*)_T8;_TA=_T9->mwo;_TB=_T6 + _TA;{int mw2=Cyc_PP_min(_T5,_TB);_TC=mw;_TD=mwo;_TE=ds;_TF=_TE->hd;_T10=(struct Cyc_PP_Doc*)_TF;_T11=_T10->mw;_T12=_TD + _T11;{
int mwo2=Cyc_PP_min(_TC,_T12);
mw=mw2;
mwo=mwo2;}}_T13=ds;
# 381
ds=_T13->tl;goto _TL5E;_TL5D:;}{struct Cyc_PP_Doc*_T18=_cycalloc(sizeof(struct Cyc_PP_Doc));
# 387
_T18->mwo=mw;_T18->mw=mwo;_T16=Cyc_Fn_make_fn;{struct Cyc_Fn_Function*(*_T19)(struct Cyc_PP_Out*(*)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*))_T16;_T15=_T19;}_T17=orig;_T18->f=_T15(Cyc_PP_long_cats_f,_T17);_T14=(struct Cyc_PP_Doc*)_T18;}return _T14;}}}}
# 390
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){struct Cyc_PP_Doc*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_PP_Doc*_T5;int _T6;struct Cyc_PP_Doc*_T7;struct Cyc_List_List*_T8;void*_T9;struct Cyc_PP_Doc*_TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct Cyc_PP_Doc*_TD;struct Cyc_PP_Doc*_TE;
if(doclist!=0)goto _TL5F;_T0=Cyc_PP_nil_doc();return _T0;
_TL5F: _T1=doclist;_T2=_T1->tl;if(_T2!=0)goto _TL61;_T3=doclist;_T4=_T3->hd;_T5=(struct Cyc_PP_Doc*)_T4;return _T5;
# 394
_TL61: _T6=Cyc_List_length(doclist);if(_T6 <= 30)goto _TL63;_T7=Cyc_PP_long_cats(doclist);return _T7;
_TL63: _T8=doclist;_T9=_T8->hd;_TA=(struct Cyc_PP_Doc*)_T9;_TB=doclist;_TC=_TB->tl;_TD=Cyc_PP_cats(_TC);_TE=Cyc_PP_concat(_TA,_TD);return _TE;}
# 398
static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _fat_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){struct _fat_ptr*_T0;struct _fat_ptr _T1;unsigned _T2;struct _fat_ptr _T3;int _T4;unsigned char*_T5;struct Cyc_PP_Doc**_T6;struct Cyc_PP_Out*(*_T7)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T8)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T9;struct Cyc_Fn_Function*_TA;struct Cyc_PP_Ppstate*_TB;struct Cyc_PP_Ppstate*_TC;struct Cyc_PP_Ppstate*_TD;struct Cyc_PP_Out*_TE;struct Cyc_PP_Out*_TF;struct Cyc_PP_Ppstate*_T10;struct Cyc_PP_Ppstate*_T11;int _T12;struct Cyc_PP_Doc*_T13;int _T14;struct Cyc_List_List*_T15;struct Cyc_List_List*_T16;void*_T17;struct Cyc_PP_Out*_T18;struct Cyc_List_List*_T19;void*_T1A;struct Cyc_PP_Out*_T1B;struct Cyc_PP_Empty_PP_Alist_struct*_T1C;struct Cyc_PP_Empty_PP_Alist_struct*_T1D;struct Cyc_PP_Empty_PP_Alist_struct*_T1E;struct Cyc_PP_Empty_PP_Alist_struct*_T1F;struct Cyc_List_List*_T20;void*_T21;struct Cyc_PP_Out*_T22;void*_T23;void*_T24;struct Cyc_List_List*_T25;void*_T26;struct Cyc_PP_Out*_T27;void*_T28;void*_T29;struct Cyc_List_List*_T2A;struct Cyc_PP_Out*_T2B;
struct Cyc_List_List*os=0;_T0=docs_ptr;{
struct _fat_ptr docs=*_T0;_T1=docs;_T2=
_get_fat_size(_T1,sizeof(struct Cyc_PP_Doc*));{int sz=(int)_T2;{
int i=0;_TL68: if(i < sz)goto _TL66;else{goto _TL67;}
_TL66: _T3=docs;_T4=i;_T5=_check_fat_subscript(_T3,sizeof(struct Cyc_PP_Doc*),_T4);_T6=(struct Cyc_PP_Doc**)_T5;{struct Cyc_PP_Doc*d=*_T6;_T8=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_T2C)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T8;_T7=_T2C;}_T9=d;_TA=_T9->f;_TB=st;{struct Cyc_PP_Out*o=_T7(_TA,_TB);{struct Cyc_PP_Ppstate*_T2C=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_TD=st;
_T2C->ci=_TD->ci;_TE=o;_T2C->cc=_TE->newcc;_TF=o;_T2C->cl=_TF->newcl;_T10=st;_T2C->pw=_T10->pw;_T11=st;_T12=_T11->epw;_T13=d;_T14=_T13->mwo;_T2C->epw=_T12 - _T14;_TC=(struct Cyc_PP_Ppstate*)_T2C;}st=_TC;{struct Cyc_List_List*_T2C=_cycalloc(sizeof(struct Cyc_List_List));
_T2C->hd=o;_T2C->tl=os;_T15=(struct Cyc_List_List*)_T2C;}os=_T15;}}
# 402
i=i + 1;goto _TL68;_TL67:;}_T16=
# 408
_check_null(os);_T17=_T16->hd;_T18=(struct Cyc_PP_Out*)_T17;{int newcc=_T18->newcc;_T19=os;_T1A=_T19->hd;_T1B=(struct Cyc_PP_Out*)_T1A;{
int newcl=_T1B->newcl;_T1C=& Cyc_PP_Empty_stringptr;_T1D=(struct Cyc_PP_Empty_PP_Alist_struct*)_T1C;{
void*s=(void*)_T1D;_T1E=& Cyc_PP_Empty_link;_T1F=(struct Cyc_PP_Empty_PP_Alist_struct*)_T1E;{
void*links=(void*)_T1F;
_TL6C: if(os!=0)goto _TL6A;else{goto _TL6B;}
_TL6A: _T20=os;_T21=_T20->hd;_T22=(struct Cyc_PP_Out*)_T21;_T23=_T22->ppout;_T24=s;s=Cyc_PP_append(_T23,_T24);_T25=os;_T26=_T25->hd;_T27=(struct Cyc_PP_Out*)_T26;_T28=_T27->links;_T29=links;
links=Cyc_PP_append(_T28,_T29);_T2A=os;
# 412
os=_T2A->tl;goto _TL6C;_TL6B:{struct Cyc_PP_Out*_T2C=_cycalloc(sizeof(struct Cyc_PP_Out));
# 416
_T2C->newcc=newcc;_T2C->newcl=newcl;_T2C->ppout=s;_T2C->links=links;_T2B=(struct Cyc_PP_Out*)_T2C;}return _T2B;}}}}}}}
# 419
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _fat_ptr docs){struct _fat_ptr _T0;unsigned _T1;struct Cyc_Core_Failure_exn_struct*_T2;void*_T3;struct _fat_ptr _T4;unsigned char*_T5;struct Cyc_PP_Doc**_T6;struct Cyc_PP_Doc**_T7;struct Cyc_PP_Doc*_T8;struct Cyc_PP_Doc*_T9;int _TA;int _TB;struct _fat_ptr _TC;int _TD;unsigned char*_TE;struct Cyc_PP_Doc**_TF;struct Cyc_PP_Doc*_T10;int _T11;int _T12;int _T13;int _T14;struct _fat_ptr _T15;unsigned char*_T16;struct Cyc_PP_Doc**_T17;int _T18;struct Cyc_PP_Doc*_T19;int _T1A;int _T1B;struct Cyc_PP_Doc*_T1C;struct Cyc_Fn_Function*(*_T1D)(struct Cyc_PP_Out*(*)(struct _fat_ptr*,struct Cyc_PP_Ppstate*),struct _fat_ptr*);struct Cyc_Fn_Function*(*_T1E)(void*(*)(void*,void*),void*);struct _fat_ptr*_T1F;struct _fat_ptr*_T20;unsigned _T21;_T0=docs;_T1=
_get_fat_size(_T0,sizeof(struct Cyc_PP_Doc*));{int sz=(int)_T1;
if(sz!=0)goto _TL6D;{struct Cyc_Core_Failure_exn_struct*_T22=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_T22->tag=Cyc_Core_Failure;
_T22->f1=_tag_fat("cats_arr -- size zero array",sizeof(char),28U);_T2=(struct Cyc_Core_Failure_exn_struct*)_T22;}_T3=(void*)_T2;_throw(_T3);goto _TL6E;_TL6D: _TL6E: _T4=docs;_T5=_T4.curr;_T6=(struct Cyc_PP_Doc**)_T5;_T7=
_check_null(_T6);{struct Cyc_PP_Doc*d=_T7[0];_T8=d;{
int mw=_T8->mw;_T9=d;{
int mwo=_T9->mw;{
int i=1;_TL72: if(i < sz)goto _TL70;else{goto _TL71;}
_TL70: _TA=mw;_TB=mwo;_TC=docs;_TD=i;_TE=_check_fat_subscript(_TC,sizeof(struct Cyc_PP_Doc*),_TD);_TF=(struct Cyc_PP_Doc**)_TE;_T10=*_TF;_T11=_T10->mwo;_T12=_TB + _T11;{int mw2=Cyc_PP_min(_TA,_T12);_T13=mw;_T14=mwo;_T15=docs;_T16=_T15.curr;_T17=(struct Cyc_PP_Doc**)_T16;_T18=i;_T19=_T17[_T18];_T1A=_T19->mw;_T1B=_T14 + _T1A;{
int mwo2=Cyc_PP_min(_T13,_T1B);
mw=mw2;
mwo=mwo2;}}
# 426
i=i + 1;goto _TL72;_TL71:;}{struct Cyc_PP_Doc*_T22=_cycalloc(sizeof(struct Cyc_PP_Doc));
# 432
_T22->mwo=mw;_T22->mw=mwo;_T1E=Cyc_Fn_make_fn;{struct Cyc_Fn_Function*(*_T23)(struct Cyc_PP_Out*(*)(struct _fat_ptr*,struct Cyc_PP_Ppstate*),struct _fat_ptr*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _fat_ptr*,struct Cyc_PP_Ppstate*),struct _fat_ptr*))_T1E;_T1D=_T23;}{unsigned _T23=1;_T21=_check_times(_T23,sizeof(struct _fat_ptr));{struct _fat_ptr*_T24=_cycalloc(_T21);_T24[0]=docs;_T20=(struct _fat_ptr*)_T24;}_T1F=_T20;}_T22->f=_T1D(Cyc_PP_cats_arr_f,_T1F);_T1C=(struct Cyc_PP_Doc*)_T22;}return _T1C;}}}}}
# 435
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){struct _tuple6*_T0;int _T1;struct Cyc_PP_Ppstate*_T2;int _T3;struct Cyc_PP_Doc*_T4;int _T5;int _T6;struct Cyc_PP_Ppstate*_T7;int _T8;struct Cyc_PP_Ppstate*_T9;int _TA;struct Cyc_PP_Doc*_TB;int _TC;int _TD;struct Cyc_PP_Ppstate*_TE;int _TF;int _T10;struct Cyc_PP_Out*(*_T11)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T12)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T13;struct Cyc_Fn_Function*_T14;struct Cyc_PP_Ppstate*_T15;struct Cyc_PP_Out*_T16;struct Cyc_PP_Out*(*_T17)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_T18)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_T19;struct Cyc_Fn_Function*_T1A;struct Cyc_PP_Ppstate*_T1B;struct Cyc_PP_Out*_T1C;struct Cyc_PP_Doc*_T1D;struct Cyc_PP_Doc*_T1E;_T0=clo;{struct _tuple6 _T1F=*_T0;_T1E=_T1F.f0;_T1D=_T1F.f1;}{struct Cyc_PP_Doc*d=_T1E;struct Cyc_PP_Doc*d2=_T1D;_T2=st;_T3=_T2->cc;_T4=d;_T5=_T4->mwo;_T6=_T3 + _T5;_T7=st;_T8=_T7->epw;
# 437
if(_T6 > _T8)goto _TL73;_T1=1;goto _TL74;_TL73: _T9=st;_TA=_T9->cc;_TB=d;_TC=_TB->mw;_TD=_TA + _TC;_TE=st;_TF=_TE->pw;_T1=_TD <= _TF;_TL74: {int dfits=_T1;_T10=dfits;
# 439
if(!_T10)goto _TL75;_T12=Cyc_Fn_apply;{struct Cyc_PP_Out*(*_T1F)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T12;_T11=_T1F;}_T13=d;_T14=_T13->f;_T15=st;_T16=_T11(_T14,_T15);return _T16;
_TL75: _T18=Cyc_Fn_apply;{struct Cyc_PP_Out*(*_T1F)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_T18;_T17=_T1F;}_T19=d2;_T1A=_T19->f;_T1B=st;_T1C=_T17(_T1A,_T1B);return _T1C;}}}
# 442
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){struct Cyc_PP_Doc*_T0;struct Cyc_PP_Doc*_T1;int _T2;struct Cyc_PP_Doc*_T3;int _T4;struct Cyc_PP_Doc*_T5;int _T6;struct Cyc_PP_Doc*_T7;int _T8;struct Cyc_Fn_Function*(*_T9)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*);struct Cyc_Fn_Function*(*_TA)(void*(*)(void*,void*),void*);struct _tuple6*_TB;{struct Cyc_PP_Doc*_TC=_cycalloc(sizeof(struct Cyc_PP_Doc));_T1=d;_T2=_T1->mwo;_T3=d2;_T4=_T3->mwo;
_TC->mwo=Cyc_PP_min(_T2,_T4);_T5=d;_T6=_T5->mw;_T7=d2;_T8=_T7->mw;
_TC->mw=Cyc_PP_min(_T6,_T8);_TA=Cyc_Fn_make_fn;{
struct Cyc_Fn_Function*(*_TD)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*))_TA;_T9=_TD;}{struct _tuple6*_TD=_cycalloc(sizeof(struct _tuple6));_TD->f0=d;_TD->f1=d2;_TB=(struct _tuple6*)_TD;}_TC->f=_T9(Cyc_PP_doc_union_f,_TB);_T0=(struct Cyc_PP_Doc*)_TC;}
# 443
return _T0;}
# 449
struct Cyc_PP_Doc*Cyc_PP_oline_doc (void){struct Cyc_PP_Doc*_T0;struct Cyc_PP_Doc*_T1;struct Cyc_PP_Doc*_T2;_T0=
Cyc_PP_nil_doc();_T1=Cyc_PP_line_doc();_T2=Cyc_PP_doc_union(_T0,_T1);return _T2;}
# 453
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){struct Cyc_PP_Ppstate*_T0;struct Cyc_PP_Ppstate*_T1;struct Cyc_PP_Ppstate*_T2;struct Cyc_PP_Ppstate*_T3;struct Cyc_PP_Ppstate*_T4;struct Cyc_PP_Ppstate*_T5;struct Cyc_PP_Ppstate*_T6;struct Cyc_PP_Ppstate*_T7;struct Cyc_PP_Ppstate*_T8;struct Cyc_PP_Ppstate*_T9;struct Cyc_PP_Out*(*_TA)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*);void*(*_TB)(struct Cyc_Fn_Function*,void*);struct Cyc_PP_Doc*_TC;struct Cyc_Fn_Function*_TD;struct Cyc_PP_Ppstate*_TE;struct Cyc_PP_Out*_TF;
struct Cyc_PP_Ppstate*st2;st2=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_T0=st2;_T1=st;_T0->ci=_T1->cc;_T2=st2;_T3=st;_T2->cc=_T3->cc;_T4=st2;_T5=st;_T4->cl=_T5->cl;_T6=st2;_T7=st;_T6->pw=_T7->pw;_T8=st2;_T9=st;_T8->epw=_T9->epw;_TB=Cyc_Fn_apply;{
struct Cyc_PP_Out*(*_T10)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*)=(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))_TB;_TA=_T10;}_TC=d;_TD=_TC->f;_TE=st2;_TF=_TA(_TD,_TE);return _TF;}
# 457
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){struct Cyc_PP_Doc*_T0;struct Cyc_PP_Doc*_T1;struct Cyc_PP_Doc*_T2;struct Cyc_PP_Doc*_T3;struct Cyc_PP_Doc*_T4;struct Cyc_Fn_Function*(*_T5)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*);struct Cyc_Fn_Function*(*_T6)(void*(*)(void*,void*),void*);struct Cyc_PP_Doc*_T7;struct Cyc_PP_Doc*_T8;
struct Cyc_PP_Doc*d2;d2=_cycalloc(sizeof(struct Cyc_PP_Doc));_T0=d2;_T1=d;
_T0->mwo=_T1->mwo;_T2=d2;_T3=d;
_T2->mw=_T3->mw;_T4=d2;_T6=Cyc_Fn_make_fn;{
struct Cyc_Fn_Function*(*_T9)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*)=(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*))_T6;_T5=_T9;}_T7=d;_T4->f=_T5(Cyc_PP_tab_f,_T7);_T8=d2;
return _T8;}
# 467
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){struct Cyc_PP_Doc*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_PP_Doc*_T5;struct Cyc_PP_Doc*_T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_PP_Doc*(*_T9)(void*);struct _fat_ptr _TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct _fat_ptr _TD;
if(l!=0)goto _TL77;_T0=Cyc_PP_nil_doc();return _T0;
_TL77: _T1=l;_T2=_T1->tl;if(_T2!=0)goto _TL79;_T3=l;_T4=_T3->hd;_T5=pp(_T4);return _T5;
_TL79:{struct Cyc_PP_Doc*_TE[4];_T7=l;_T8=_T7->hd;_TE[0]=pp(_T8);_TE[1]=Cyc_PP_text(sep);_TE[2]=Cyc_PP_oline_doc();_T9=pp;_TA=sep;_TB=l;_TC=_TB->tl;_TE[3]=Cyc_PP_ppseq_f(_T9,_TA,_TC);_TD=_tag_fat(_TE,sizeof(struct Cyc_PP_Doc*),4);_T6=Cyc_PP_cat(_TD);}return _T6;}
# 473
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){struct Cyc_PP_Doc*(*_T0)(void*);struct _fat_ptr _T1;struct Cyc_List_List*_T2;struct Cyc_PP_Doc*_T3;struct Cyc_PP_Doc*_T4;_T0=pp;_T1=sep;_T2=l;_T3=
Cyc_PP_ppseq_f(_T0,_T1,_T2);_T4=Cyc_PP_tab(_T3);return _T4;}
# 477
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _fat_ptr sep,struct Cyc_List_List*l){struct Cyc_PP_Doc*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_PP_Doc*_T5;struct Cyc_List_List*_T6;struct Cyc_List_List*_T7;struct Cyc_List_List*_T8;struct Cyc_List_List*_T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct Cyc_PP_Doc*_TC;
if(l!=0)goto _TL7B;_T0=Cyc_PP_nil_doc();return _T0;
_TL7B: _T1=l;_T2=_T1->tl;if(_T2!=0)goto _TL7D;_T3=l;_T4=_T3->hd;_T5=(struct Cyc_PP_Doc*)_T4;return _T5;
# 481
_TL7D: {struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);
struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*x=l;
_TL7F: _T6=_check_null(x);_T7=_T6->tl;if(_T7!=0)goto _TL80;else{goto _TL81;}
_TL80: _T8=x;{struct Cyc_List_List*temp=_T8->tl;_T9=x;{struct Cyc_List_List*_TD=_cycalloc(sizeof(struct Cyc_List_List));
_TD->hd=sep2;{struct Cyc_List_List*_TE=_cycalloc(sizeof(struct Cyc_List_List));_TE->hd=oline;_TE->tl=temp;_TB=(struct Cyc_List_List*)_TE;}_TD->tl=_TB;_TA=(struct Cyc_List_List*)_TD;}_T9->tl=_TA;
x=temp;}goto _TL7F;_TL81: _TC=
# 489
Cyc_PP_cats(l);return _TC;}}
# 497
struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr sep,struct Cyc_List_List*l){struct Cyc_PP_Doc*_T0;struct Cyc_PP_Doc*_T1;_T0=
Cyc_PP_seq_f(sep,l);_T1=Cyc_PP_tab(_T0);return _T1;}
# 502
struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){struct Cyc_PP_Doc*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_PP_Doc*_T5;struct Cyc_PP_Doc*_T6;struct Cyc_List_List*_T7;void*_T8;struct Cyc_PP_Doc*(*_T9)(void*);struct _fat_ptr _TA;struct Cyc_List_List*_TB;struct Cyc_List_List*_TC;struct _fat_ptr _TD;
if(l!=0)goto _TL82;_T0=Cyc_PP_nil_doc();return _T0;
_TL82: _T1=l;_T2=_T1->tl;if(_T2!=0)goto _TL84;_T3=l;_T4=_T3->hd;_T5=pp(_T4);return _T5;
_TL84:{struct Cyc_PP_Doc*_TE[4];_T7=l;_T8=_T7->hd;_TE[0]=pp(_T8);_TE[1]=Cyc_PP_text(sep);_TE[2]=Cyc_PP_line_doc();_T9=pp;_TA=sep;_TB=l;_TC=_TB->tl;_TE[3]=Cyc_PP_ppseql_f(_T9,_TA,_TC);_TD=_tag_fat(_TE,sizeof(struct Cyc_PP_Doc*),4);_T6=Cyc_PP_cat(_TD);}return _T6;}
# 507
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){struct Cyc_PP_Doc*(*_T0)(void*);struct _fat_ptr _T1;struct Cyc_List_List*_T2;struct Cyc_PP_Doc*_T3;struct Cyc_PP_Doc*_T4;_T0=pp;_T1=sep;_T2=l;_T3=
Cyc_PP_ppseql_f(_T0,_T1,_T2);_T4=Cyc_PP_tab(_T3);return _T4;}
# 511
static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _fat_ptr sep,struct Cyc_List_List*l){struct Cyc_PP_Doc*_T0;struct Cyc_List_List*_T1;struct Cyc_List_List*_T2;struct Cyc_List_List*_T3;void*_T4;struct Cyc_PP_Doc*_T5;struct Cyc_PP_Doc*_T6;struct Cyc_List_List*_T7;void*_T8;struct _fat_ptr _T9;struct Cyc_List_List*_TA;struct Cyc_List_List*_TB;struct _fat_ptr _TC;
if(l!=0)goto _TL86;_T0=Cyc_PP_nil_doc();return _T0;
_TL86: _T1=l;_T2=_T1->tl;if(_T2!=0)goto _TL88;_T3=l;_T4=_T3->hd;_T5=(struct Cyc_PP_Doc*)_T4;return _T5;
_TL88:{struct Cyc_PP_Doc*_TD[4];_T7=l;_T8=_T7->hd;_TD[0]=(struct Cyc_PP_Doc*)_T8;_TD[1]=Cyc_PP_text(sep);_TD[2]=Cyc_PP_line_doc();_T9=sep;_TA=l;_TB=_TA->tl;_TD[3]=Cyc_PP_seql_f(_T9,_TB);_TC=_tag_fat(_TD,sizeof(struct Cyc_PP_Doc*),4);_T6=Cyc_PP_cat(_TC);}return _T6;}
# 516
struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr sep,struct Cyc_List_List*l0){struct Cyc_PP_Doc*_T0;struct Cyc_PP_Doc*_T1;_T0=
Cyc_PP_seql_f(sep,l0);_T1=Cyc_PP_tab(_T0);return _T1;}
# 521
struct Cyc_PP_Doc*Cyc_PP_group(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_T0;struct _fat_ptr _T1;{struct Cyc_PP_Doc*_T2[3];
_T2[0]=Cyc_PP_text(start);
_T2[1]=Cyc_PP_seq(sep,ss);
_T2[2]=Cyc_PP_text(stop);_T1=_tag_fat(_T2,sizeof(struct Cyc_PP_Doc*),3);_T0=Cyc_PP_cat(_T1);}
# 522
return _T0;}
# 528
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_T0;struct Cyc_PP_Doc*_T1;struct _fat_ptr _T2;
if(ss!=0)goto _TL8A;_T0=Cyc_PP_nil_doc();return _T0;
_TL8A:{struct Cyc_PP_Doc*_T3[3];_T3[0]=Cyc_PP_text(start);
_T3[1]=Cyc_PP_seq(sep,ss);
_T3[2]=Cyc_PP_text(stop);_T2=_tag_fat(_T3,sizeof(struct Cyc_PP_Doc*),3);_T1=Cyc_PP_cat(_T2);}
# 530
return _T1;}
# 535
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){struct Cyc_PP_Doc*_T0;struct _fat_ptr _T1;{struct Cyc_PP_Doc*_T2[3];
_T2[0]=Cyc_PP_text(start);
_T2[1]=Cyc_PP_seql(sep,ss);
_T2[2]=Cyc_PP_text(stop);_T1=_tag_fat(_T2,sizeof(struct Cyc_PP_Doc*),3);_T0=Cyc_PP_cat(_T1);}
# 536
return _T0;}
