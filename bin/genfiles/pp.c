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
#ifdef CYC_REGION_PROFILE
{ unsigned total_bytes;
  unsigned free_bytes;
  struct _RegionPage *next;
  char data[1];
}
#endif
; // abstract -- defined in runtime_memory.c
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
typedef unsigned char _AliasQualHandle_t;

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
void* _throw_fn(void*,const char*,unsigned);
void* _rethrow(void*);
#define _throw_null() (_throw_null_fn(__FILE__,__LINE__))
#define _throw_arraybounds() (_throw_arraybounds_fn(__FILE__,__LINE__))
#define _throw_badalloc() (_throw_badalloc_fn(__FILE__,__LINE__))
#define _throw_match() (_throw_match_fn(__FILE__,__LINE__))
#define _throw(e) (_throw_fn((e),__FILE__,__LINE__))
#endif

void* Cyc_Core_get_exn_thrown();
/* Built-in Exceptions */
struct Cyc_Null_Exception_exn_struct { char *tag; };
struct Cyc_Array_bounds_exn_struct { char *tag; };
struct Cyc_Match_Exception_exn_struct { char *tag; };
struct Cyc_Bad_alloc_exn_struct { char *tag; };
extern char Cyc_Null_Exception[];
extern char Cyc_Array_bounds[];
extern char Cyc_Match_Exception[];
extern char Cyc_Bad_alloc[];

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
#define _check_fat_at_base(arr) (arr)
#else
#define _check_fat_subscript(arr,elt_sz,index) ({ \
  struct _fat_ptr _cus_arr = (arr); \
  unsigned char *_cus_ans = _cus_arr.curr + (elt_sz) * (index); \
  /* JGM: not needed! if (!_cus_arr.base) _throw_null();*/ \
  if (_cus_ans < _cus_arr.base || _cus_ans >= _cus_arr.last_plus_one) \
    _throw_arraybounds(); \
  _cus_ans; })
#define _untag_fat_ptr(arr,elt_sz,num_elts) ({ \
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
# 116
void*Cyc_Core_identity(void*);
# 118
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
void*Cyc_PP_append(void*a1,void*a2){
struct _tuple0 _Tmp0=({struct _tuple0 _Tmp1;_Tmp1.f0=a1,_Tmp1.f1=a2;_Tmp1;});if(*((int*)_Tmp0.f0)==0)
return a2;else{if(*((int*)_Tmp0.f1)==0)
return a1;else{
return(void*)({struct Cyc_PP_Append_PP_Alist_struct*_Tmp1=_cycalloc(sizeof(struct Cyc_PP_Append_PP_Alist_struct));_Tmp1->tag=2,_Tmp1->f1=a1,_Tmp1->f2=a2;_Tmp1;});}};}
# 62
struct Cyc_List_List*Cyc_PP_list_of_alist_f(void*y,struct Cyc_List_List*l){
void*_Tmp0;void*_Tmp1;switch(*((int*)y)){case 0:
 return l;case 1: _Tmp1=(void*)((struct Cyc_PP_Single_PP_Alist_struct*)y)->f1;{void*z=_Tmp1;
return({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=z,_Tmp2->tl=l;_Tmp2;});}default: _Tmp1=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)y)->f1;_Tmp0=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)y)->f2;{void*a1=_Tmp1;void*a2=_Tmp0;
return({void*_Tmp2=a1;Cyc_PP_list_of_alist_f(_Tmp2,Cyc_PP_list_of_alist_f(a2,l));});}};}
# 69
struct Cyc_List_List*Cyc_PP_list_of_alist(void*x){
return Cyc_PP_list_of_alist_f(x,0);}struct Cyc_PP_Ppstate{int ci;int cc;int cl;int pw;int epw;};struct Cyc_PP_Out{int newcc;int newcl;void*ppout;void*links;};struct Cyc_PP_Doc{int mwo;int mw;struct Cyc_Fn_Function*f;};
# 98
static void Cyc_PP_dump_out(struct Cyc___cycFILE*f,void*al){
struct Cyc_Xarray_Xarray*xarr=Cyc_Xarray_create(16,al);
Cyc_Xarray_add(xarr,al);{
int last=0;
while(last >= 0){
void*_Tmp0=Cyc_Xarray_get(xarr,last);void*_Tmp1;void*_Tmp2;switch(*((int*)_Tmp0)){case 0:
 -- last;goto _LL0;case 1: _Tmp2=(struct _fat_ptr*)((struct Cyc_PP_Single_PP_Alist_struct*)_Tmp0)->f1;{struct _fat_ptr*s=_Tmp2;
-- last;({struct Cyc_String_pa_PrintArg_struct _Tmp3=({struct Cyc_String_pa_PrintArg_struct _Tmp4;_Tmp4.tag=0,_Tmp4.f1=(struct _fat_ptr)((struct _fat_ptr)*s);_Tmp4;});void*_Tmp4[1];_Tmp4[0]=& _Tmp3;Cyc_fprintf(f,_tag_fat("%s",sizeof(char),3U),_tag_fat(_Tmp4,sizeof(void*),1));});goto _LL0;}default: _Tmp2=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_Tmp0)->f1;_Tmp1=(void*)((struct Cyc_PP_Append_PP_Alist_struct*)_Tmp0)->f2;{void*a1=_Tmp2;void*a2=_Tmp1;
# 107
Cyc_Xarray_set(xarr,last,a2);
if(({int _Tmp3=last;_Tmp3==Cyc_Xarray_length(xarr)- 1;}))
Cyc_Xarray_add(xarr,a1);else{
# 111
Cyc_Xarray_set(xarr,last + 1,a1);}
++ last;
goto _LL0;}}_LL0:;}
# 116
Cyc_Xarray_reuse(xarr);}}
# 120
void Cyc_PP_file_of_doc(struct Cyc_PP_Doc*d,int w,struct Cyc___cycFILE*f){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_Tmp0=d->f;({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(_Tmp0,({struct Cyc_PP_Ppstate*_Tmp1=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_Tmp1->ci=0,_Tmp1->cc=0,_Tmp1->cl=1,_Tmp1->pw=w,_Tmp1->epw=w;_Tmp1;}));});
Cyc_PP_dump_out(f,o->ppout);}
# 126
struct _fat_ptr Cyc_PP_string_of_doc(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_Tmp0=d->f;({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(_Tmp0,({struct Cyc_PP_Ppstate*_Tmp1=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_Tmp1->ci=0,_Tmp1->cc=0,_Tmp1->cl=1,_Tmp1->pw=w,_Tmp1->epw=w;_Tmp1;}));});
return({struct Cyc_List_List*_Tmp0=Cyc_PP_list_of_alist(o->ppout);Cyc_str_sepstr(_Tmp0,_tag_fat("",sizeof(char),1U));});}struct _tuple1{struct _fat_ptr f0;struct Cyc_List_List*f1;};
# 133
struct _tuple1*Cyc_PP_string_and_links(struct Cyc_PP_Doc*d,int w){
struct Cyc_PP_Out*o=({struct Cyc_Fn_Function*_Tmp0=d->f;({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(_Tmp0,({struct Cyc_PP_Ppstate*_Tmp1=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_Tmp1->ci=0,_Tmp1->cc=0,_Tmp1->cl=1,_Tmp1->pw=w,_Tmp1->epw=w;_Tmp1;}));});
return({struct _tuple1*_Tmp0=_cycalloc(sizeof(struct _tuple1));({struct _fat_ptr _Tmp1=(struct _fat_ptr)({
struct Cyc_List_List*_Tmp2=Cyc_PP_list_of_alist(o->ppout);Cyc_str_sepstr(_Tmp2,_tag_fat("",sizeof(char),1U));});
# 135
_Tmp0->f0=_Tmp1;}),({
# 137
struct Cyc_List_List*_Tmp1=Cyc_PP_list_of_alist(o->links);_Tmp0->f1=_Tmp1;});_Tmp0;});}
# 140
static struct Cyc_Core_Opt*Cyc_PP_bhashtbl=0;
# 142
int Cyc_PP_tex_output=0;
# 144
struct _fat_ptr Cyc_PP_nlblanks(int i){
if(Cyc_PP_bhashtbl==0)
Cyc_PP_bhashtbl=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_Hashtable_Table*_Tmp1=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(int,int),int(*)(int)))Cyc_Hashtable_create;})(61,Cyc_Core_intcmp,({(int(*)(int))Cyc_Core_identity;}));_Tmp0->v=_Tmp1;});_Tmp0;});
if(i < 0)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("nlblanks",sizeof(char),9U);_Tmp0;}));{
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct _fat_ptr _Tmp2=*({(struct _fat_ptr*(*)(struct Cyc_Hashtable_Table*,int))Cyc_Hashtable_lookup;})((struct Cyc_Hashtable_Table*)_check_null(Cyc_PP_bhashtbl)->v,i);_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 152
if(!Cyc_PP_tex_output){
int num_tabs=i / 8;
int num_spaces=i % 8;
int total=(2 + num_tabs)+ num_spaces;
struct _fat_ptr nlb=Cyc_Core_new_string((unsigned)total);
({struct _fat_ptr _Tmp4=_fat_ptr_plus(nlb,sizeof(char),0);char _Tmp5=*((char*)_check_fat_subscript(_Tmp4,sizeof(char),0U));char _Tmp6='\n';if(_get_fat_size(_Tmp4,sizeof(char))==1U &&(_Tmp5==0 && _Tmp6!=0))_throw_arraybounds();*((char*)_Tmp4.curr)=_Tmp6;});
{int j=0;for(0;j < num_tabs;++ j){
({struct _fat_ptr _Tmp4=_fat_ptr_plus(nlb,sizeof(char),j + 1);char _Tmp5=*((char*)_check_fat_subscript(_Tmp4,sizeof(char),0U));char _Tmp6='\t';if(_get_fat_size(_Tmp4,sizeof(char))==1U &&(_Tmp5==0 && _Tmp6!=0))_throw_arraybounds();*((char*)_Tmp4.curr)=_Tmp6;});}}
{int j=0;for(0;j < num_spaces;++ j){
({struct _fat_ptr _Tmp4=_fat_ptr_plus(nlb,sizeof(char),(j + 1)+ num_tabs);char _Tmp5=*((char*)_check_fat_subscript(_Tmp4,sizeof(char),0U));char _Tmp6=' ';if(_get_fat_size(_Tmp4,sizeof(char))==1U &&(_Tmp5==0 && _Tmp6!=0))_throw_arraybounds();*((char*)_Tmp4.curr)=_Tmp6;});}}
({struct Cyc_Hashtable_Table*_Tmp4=(struct Cyc_Hashtable_Table*)_check_null(Cyc_PP_bhashtbl)->v;int _Tmp5=i;({(void(*)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*))Cyc_Hashtable_insert;})(_Tmp4,_Tmp5,({unsigned _Tmp6=1;({struct _fat_ptr*_Tmp7=_cycalloc(_check_times(_Tmp6,sizeof(struct _fat_ptr)));_Tmp7[0]=nlb;_Tmp7;});}));});
return nlb;}else{
# 168
int total=3 + i;
struct _fat_ptr nlb=Cyc_Core_new_string((unsigned)(total + 1));
({struct _fat_ptr _Tmp4=_fat_ptr_plus(nlb,sizeof(char),0);char _Tmp5=*((char*)_check_fat_subscript(_Tmp4,sizeof(char),0U));char _Tmp6='\\';if(_get_fat_size(_Tmp4,sizeof(char))==1U &&(_Tmp5==0 && _Tmp6!=0))_throw_arraybounds();*((char*)_Tmp4.curr)=_Tmp6;});
({struct _fat_ptr _Tmp4=_fat_ptr_plus(nlb,sizeof(char),1);char _Tmp5=*((char*)_check_fat_subscript(_Tmp4,sizeof(char),0U));char _Tmp6='\\';if(_get_fat_size(_Tmp4,sizeof(char))==1U &&(_Tmp5==0 && _Tmp6!=0))_throw_arraybounds();*((char*)_Tmp4.curr)=_Tmp6;});
({struct _fat_ptr _Tmp4=_fat_ptr_plus(nlb,sizeof(char),2);char _Tmp5=*((char*)_check_fat_subscript(_Tmp4,sizeof(char),0U));char _Tmp6='\n';if(_get_fat_size(_Tmp4,sizeof(char))==1U &&(_Tmp5==0 && _Tmp6!=0))_throw_arraybounds();*((char*)_Tmp4.curr)=_Tmp6;});
{int j=3;for(0;j < total;++ j){
({struct _fat_ptr _Tmp4=_fat_ptr_plus(nlb,sizeof(char),j);char _Tmp5=*((char*)_check_fat_subscript(_Tmp4,sizeof(char),0U));char _Tmp6='~';if(_get_fat_size(_Tmp4,sizeof(char))==1U &&(_Tmp5==0 && _Tmp6!=0))_throw_arraybounds();*((char*)_Tmp4.curr)=_Tmp6;});}}
({struct Cyc_Hashtable_Table*_Tmp4=(struct Cyc_Hashtable_Table*)_check_null(Cyc_PP_bhashtbl)->v;int _Tmp5=i;({(void(*)(struct Cyc_Hashtable_Table*,int,struct _fat_ptr*))Cyc_Hashtable_insert;})(_Tmp4,_Tmp5,({unsigned _Tmp6=1;({struct _fat_ptr*_Tmp7=_cycalloc(_check_times(_Tmp6,sizeof(struct _fat_ptr)));_Tmp7[0]=nlb;_Tmp7;});}));});
return nlb;}}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}}
# 182
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl=0;
# 184
int Cyc_PP_infinity=9999999;struct _tuple2{int f0;struct _fat_ptr f1;};
# 186
static struct Cyc_PP_Out*Cyc_PP_text_doc_f(struct _tuple2*clo,struct Cyc_PP_Ppstate*st){
struct _fat_ptr _Tmp0;int _Tmp1;_Tmp1=clo->f0;_Tmp0=clo->f1;{int slen=_Tmp1;struct _fat_ptr s=_Tmp0;
return({struct Cyc_PP_Out*_Tmp2=_cycalloc(sizeof(struct Cyc_PP_Out));_Tmp2->newcc=st->cc + slen,_Tmp2->newcl=st->cl,({
# 190
void*_Tmp3=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_Tmp4=_cycalloc(sizeof(struct Cyc_PP_Single_PP_Alist_struct));_Tmp4->tag=1,({struct _fat_ptr*_Tmp5=({unsigned _Tmp6=1;({struct _fat_ptr*_Tmp7=_cycalloc(_check_times(_Tmp6,sizeof(struct _fat_ptr)));_Tmp7[0]=s;_Tmp7;});});_Tmp4->f1=_Tmp5;});_Tmp4;});_Tmp2->ppout=_Tmp3;}),_Tmp2->links=(void*)& Cyc_PP_Empty_link;_Tmp2;});}}
# 193
static struct Cyc_PP_Doc*Cyc_PP_text_doc(struct _fat_ptr s){
int slen=(int)Cyc_strlen(s);
return({struct Cyc_PP_Doc*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Doc));
_Tmp0->mwo=slen,_Tmp0->mw=Cyc_PP_infinity,({
# 198
struct Cyc_Fn_Function*_Tmp1=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*))Cyc_Fn_make_fn;})(Cyc_PP_text_doc_f,({struct _tuple2*_Tmp2=_cycalloc(sizeof(struct _tuple2));
# 200
_Tmp2->f0=slen,_Tmp2->f1=s;_Tmp2;}));
# 198
_Tmp0->f=_Tmp1;});_Tmp0;});}
# 202
struct Cyc_PP_Doc*Cyc_PP_text(struct _fat_ptr s){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl==0){
t=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;})(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=t;_Tmp0;});}else{
# 208
t=(struct Cyc_Hashtable_Table*)Cyc_PP_str_hashtbl->v;}{
# 210
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_PP_Doc*_Tmp2=({struct Cyc_Hashtable_Table*_Tmp3=t;({(struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;})(_Tmp3,({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));*_Tmp4=s;_Tmp4;}));});_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 214
struct Cyc_PP_Doc*d=Cyc_PP_text_doc(s);
({struct Cyc_Hashtable_Table*_Tmp4=t;struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));*_Tmp6=s;_Tmp6;});({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*))Cyc_Hashtable_insert;})(_Tmp4,_Tmp5,d);});
return d;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}}
# 219
struct Cyc_PP_Doc*Cyc_PP_textptr(struct _fat_ptr*s){return Cyc_PP_text(*s);}
# 224
static struct Cyc_PP_Doc*Cyc_PP_text_width_doc(struct _fat_ptr s,int slen){
return({struct Cyc_PP_Doc*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Doc));
_Tmp0->mwo=slen,_Tmp0->mw=Cyc_PP_infinity,({
# 228
struct Cyc_Fn_Function*_Tmp1=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple2*,struct Cyc_PP_Ppstate*),struct _tuple2*))Cyc_Fn_make_fn;})(Cyc_PP_text_doc_f,({struct _tuple2*_Tmp2=_cycalloc(sizeof(struct _tuple2));
# 230
_Tmp2->f0=slen,_Tmp2->f1=s;_Tmp2;}));
# 228
_Tmp0->f=_Tmp1;});_Tmp0;});}
# 232
static struct Cyc_Core_Opt*Cyc_PP_str_hashtbl2=0;
struct Cyc_PP_Doc*Cyc_PP_text_width(struct _fat_ptr s,int slen){
struct Cyc_Hashtable_Table*t;
if(Cyc_PP_str_hashtbl2==0){
t=({(struct Cyc_Hashtable_Table*(*)(int,int(*)(struct _fat_ptr*,struct _fat_ptr*),int(*)(struct _fat_ptr*)))Cyc_Hashtable_create;})(101,Cyc_zstrptrcmp,Cyc_Hashtable_hash_stringptr);
Cyc_PP_str_hashtbl2=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=t;_Tmp0;});}else{
# 239
t=(struct Cyc_Hashtable_Table*)Cyc_PP_str_hashtbl2->v;}{
# 241
struct _handler_cons _Tmp0;_push_handler(& _Tmp0);{int _Tmp1=0;if(setjmp(_Tmp0.handler))_Tmp1=1;if(!_Tmp1){
{struct Cyc_PP_Doc*_Tmp2=({struct Cyc_Hashtable_Table*_Tmp3=t;({(struct Cyc_PP_Doc*(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*))Cyc_Hashtable_lookup;})(_Tmp3,({struct _fat_ptr*_Tmp4=_cycalloc(sizeof(struct _fat_ptr));*_Tmp4=s;_Tmp4;}));});_npop_handler(0);return _Tmp2;};_pop_handler();}else{void*_Tmp2=(void*)Cyc_Core_get_exn_thrown();void*_Tmp3;if(((struct Cyc_Core_Not_found_exn_struct*)_Tmp2)->tag==Cyc_Core_Not_found){
# 245
struct Cyc_PP_Doc*d=Cyc_PP_text_width_doc(s,slen);
({struct Cyc_Hashtable_Table*_Tmp4=t;struct _fat_ptr*_Tmp5=({struct _fat_ptr*_Tmp6=_cycalloc(sizeof(struct _fat_ptr));*_Tmp6=s;_Tmp6;});({(void(*)(struct Cyc_Hashtable_Table*,struct _fat_ptr*,struct Cyc_PP_Doc*))Cyc_Hashtable_insert;})(_Tmp4,_Tmp5,d);});
return d;}else{_Tmp3=_Tmp2;{void*exn=_Tmp3;_rethrow(exn);}};}}}}
# 251
struct Cyc_Core_Opt*Cyc_PP_nil_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_blank_doc_opt=0;
struct Cyc_Core_Opt*Cyc_PP_line_doc_opt=0;
# 257
struct Cyc_PP_Doc*Cyc_PP_nil_doc (void){
if(Cyc_PP_nil_doc_opt==0)
Cyc_PP_nil_doc_opt=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("",sizeof(char),1U));_Tmp0->v=_Tmp1;});_Tmp0;});
return(struct Cyc_PP_Doc*)_check_null(Cyc_PP_nil_doc_opt)->v;}
# 264
struct Cyc_PP_Doc*Cyc_PP_blank_doc (void){
if(Cyc_PP_blank_doc_opt==0)
Cyc_PP_blank_doc_opt=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(_tag_fat("",sizeof(char),1U));_Tmp0->v=_Tmp1;});_Tmp0;});
return(struct Cyc_PP_Doc*)_check_null(Cyc_PP_blank_doc_opt)->v;}struct _tuple3{int f0;struct _fat_ptr f1;struct _fat_ptr f2;};struct _tuple4{int f0;int f1;int f2;struct _fat_ptr f3;};
# 270
static struct Cyc_PP_Out*Cyc_PP_hyperlink_f(struct _tuple3*clo,struct Cyc_PP_Ppstate*st){
struct _fat_ptr _Tmp0;struct _fat_ptr _Tmp1;int _Tmp2;_Tmp2=clo->f0;_Tmp1=clo->f1;_Tmp0=clo->f2;{int slen=_Tmp2;struct _fat_ptr shrt=_Tmp1;struct _fat_ptr full=_Tmp0;
return({struct Cyc_PP_Out*_Tmp3=_cycalloc(sizeof(struct Cyc_PP_Out));_Tmp3->newcc=st->cc + slen,_Tmp3->newcl=st->cl,({
# 274
void*_Tmp4=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_PP_Single_PP_Alist_struct));_Tmp5->tag=1,({struct _fat_ptr*_Tmp6=({unsigned _Tmp7=1;({struct _fat_ptr*_Tmp8=_cycalloc(_check_times(_Tmp7,sizeof(struct _fat_ptr)));_Tmp8[0]=shrt;_Tmp8;});});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->ppout=_Tmp4;}),({
void*_Tmp4=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_Tmp5=_cycalloc(sizeof(struct Cyc_PP_Single_PP_Alist_struct));_Tmp5->tag=1,({struct _tuple4*_Tmp6=({struct _tuple4*_Tmp7=_cycalloc(sizeof(struct _tuple4));_Tmp7->f0=st->cl,_Tmp7->f1=st->cc,_Tmp7->f2=slen,_Tmp7->f3=full;_Tmp7;});_Tmp5->f1=_Tmp6;});_Tmp5;});_Tmp3->links=_Tmp4;});_Tmp3;});}}
# 277
struct Cyc_PP_Doc*Cyc_PP_hyperlink(struct _fat_ptr shrt,struct _fat_ptr full){
int slen=(int)Cyc_strlen(shrt);
return({struct Cyc_PP_Doc*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Doc));_Tmp0->mwo=slen,_Tmp0->mw=Cyc_PP_infinity,({
# 281
struct Cyc_Fn_Function*_Tmp1=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple3*,struct Cyc_PP_Ppstate*),struct _tuple3*))Cyc_Fn_make_fn;})(Cyc_PP_hyperlink_f,({struct _tuple3*_Tmp2=_cycalloc(sizeof(struct _tuple3));_Tmp2->f0=slen,_Tmp2->f1=shrt,_Tmp2->f2=full;_Tmp2;}));_Tmp0->f=_Tmp1;});_Tmp0;});}
# 285
static struct Cyc_PP_Out*Cyc_PP_line_f(struct Cyc_PP_Ppstate*st){
return({struct Cyc_PP_Out*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Out));_Tmp0->newcc=st->ci,_Tmp0->newcl=st->cl + 1,({
# 288
void*_Tmp1=(void*)({struct Cyc_PP_Single_PP_Alist_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_PP_Single_PP_Alist_struct));_Tmp2->tag=1,({struct _fat_ptr*_Tmp3=({unsigned _Tmp4=1;({struct _fat_ptr*_Tmp5=_cycalloc(_check_times(_Tmp4,sizeof(struct _fat_ptr)));({struct _fat_ptr _Tmp6=Cyc_PP_nlblanks(st->ci);_Tmp5[0]=_Tmp6;});_Tmp5;});});_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->ppout=_Tmp1;}),_Tmp0->links=(void*)& Cyc_PP_Empty_link;_Tmp0;});}
# 291
struct Cyc_PP_Doc*Cyc_PP_line_doc (void){
if(Cyc_PP_line_doc_opt==0)
Cyc_PP_line_doc_opt=({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));({struct Cyc_PP_Doc*_Tmp1=({struct Cyc_PP_Doc*_Tmp2=_cycalloc(sizeof(struct Cyc_PP_Doc));_Tmp2->mwo=0,_Tmp2->mw=0,({struct Cyc_Fn_Function*_Tmp3=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Ppstate*)))Cyc_Fn_fp2fn;})(Cyc_PP_line_f);_Tmp2->f=_Tmp3;});_Tmp2;});_Tmp0->v=_Tmp1;});_Tmp0;});
return(struct Cyc_PP_Doc*)_check_null(Cyc_PP_line_doc_opt)->v;}struct _tuple5{int f0;struct Cyc_PP_Doc*f1;};
# 297
static struct Cyc_PP_Out*Cyc_PP_nest_f(struct _tuple5*clo,struct Cyc_PP_Ppstate*st){
void*_Tmp0;int _Tmp1;_Tmp1=clo->f0;_Tmp0=clo->f1;{int k=_Tmp1;struct Cyc_PP_Doc*d=_Tmp0;
struct Cyc_PP_Ppstate*st2;
st2=_cycalloc(sizeof(struct Cyc_PP_Ppstate)),st2->ci=st->ci + k,st2->cc=st->cc,st2->cl=st->cl,st2->pw=st->pw,st2->epw=st->epw;
# 305
return({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st2);}}
# 307
struct Cyc_PP_Doc*Cyc_PP_nest(int k,struct Cyc_PP_Doc*d){
return({struct Cyc_PP_Doc*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Doc));_Tmp0->mwo=d->mwo,_Tmp0->mw=d->mw,({
# 310
struct Cyc_Fn_Function*_Tmp1=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple5*,struct Cyc_PP_Ppstate*),struct _tuple5*))Cyc_Fn_make_fn;})(Cyc_PP_nest_f,({struct _tuple5*_Tmp2=_cycalloc(sizeof(struct _tuple5));
# 312
_Tmp2->f0=k,_Tmp2->f1=d;_Tmp2;}));
# 310
_Tmp0->f=_Tmp1;});_Tmp0;});}
# 315
int Cyc_PP_min(int x,int y){
if(x <= y)return x;else{
return y;}}
# 320
int Cyc_PP_max(int x,int y){
if(x >= y)return x;else{
return y;}}struct _tuple6{struct Cyc_PP_Doc*f0;struct Cyc_PP_Doc*f1;};
# 325
static struct Cyc_PP_Out*Cyc_PP_concat_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
void*_Tmp0;void*_Tmp1;_Tmp1=clo->f0;_Tmp0=clo->f1;{struct Cyc_PP_Doc*d1=_Tmp1;struct Cyc_PP_Doc*d2=_Tmp0;
int epw2=Cyc_PP_max(st->pw - d2->mw,st->epw - d1->mwo);
struct Cyc_PP_Ppstate*st2;st2=_cycalloc(sizeof(struct Cyc_PP_Ppstate)),st2->ci=st->ci,st2->cc=st->cc,st2->cl=st->cl,st2->pw=st->pw,st2->epw=epw2;{
struct Cyc_PP_Out*o1=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d1->f,st2);
struct Cyc_PP_Ppstate*st3;st3=_cycalloc(sizeof(struct Cyc_PP_Ppstate)),st3->ci=st->ci,st3->cc=o1->newcc,st3->cl=o1->newcl,st3->pw=st->pw,st3->epw=epw2;{
struct Cyc_PP_Out*o2=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d2->f,st3);
struct Cyc_PP_Out*o3;o3=_cycalloc(sizeof(struct Cyc_PP_Out)),o3->newcc=o2->newcc,o3->newcl=o2->newcl,({
# 334
void*_Tmp2=Cyc_PP_append(o1->ppout,o2->ppout);o3->ppout=_Tmp2;}),({
void*_Tmp2=Cyc_PP_append(o1->links,o2->links);o3->links=_Tmp2;});
return o3;}}}}
# 338
static struct Cyc_PP_Doc*Cyc_PP_concat(struct Cyc_PP_Doc*d1,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Doc));({int _Tmp1=Cyc_PP_min(d1->mw,d1->mwo + d2->mwo);_Tmp0->mwo=_Tmp1;}),({
int _Tmp1=Cyc_PP_min(d1->mw,d1->mwo + d2->mw);_Tmp0->mw=_Tmp1;}),({
struct Cyc_Fn_Function*_Tmp1=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*))Cyc_Fn_make_fn;})(Cyc_PP_concat_f,({struct _tuple6*_Tmp2=_cycalloc(sizeof(struct _tuple6));_Tmp2->f0=d1,_Tmp2->f1=d2;_Tmp2;}));_Tmp0->f=_Tmp1;});_Tmp0;});}
# 343
struct Cyc_PP_Doc*Cyc_PP_cat(struct _fat_ptr l){
struct Cyc_PP_Doc*d=Cyc_PP_nil_doc();
{int i=(int)(_get_fat_size(l,sizeof(struct Cyc_PP_Doc*))- 1U);for(0;i >= 0;-- i){
d=Cyc_PP_concat(*((struct Cyc_PP_Doc**)_check_fat_subscript(l,sizeof(struct Cyc_PP_Doc*),i)),d);}}
# 348
return d;}
# 353
static struct Cyc_PP_Out*Cyc_PP_long_cats_f(struct Cyc_List_List*ds0,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
{struct Cyc_List_List*ds=ds0;for(0;ds!=0;ds=ds->tl){
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)ds->hd;
struct Cyc_PP_Out*o=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st);
st=({struct Cyc_PP_Ppstate*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_Tmp0->ci=st->ci,_Tmp0->cc=o->newcc,_Tmp0->cl=o->newcl,_Tmp0->pw=st->pw,_Tmp0->epw=st->epw - d->mwo;_Tmp0;});
os=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=o,_Tmp0->tl=os;_Tmp0;});}}{
# 361
int newcc=((struct Cyc_PP_Out*)_check_null(os)->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(1;os!=0;os=os->tl){
s=Cyc_PP_append(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=Cyc_PP_append(((struct Cyc_PP_Out*)os->hd)->links,links);}
# 369
return({struct Cyc_PP_Out*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Out));_Tmp0->newcc=newcc,_Tmp0->newcl=newcl,_Tmp0->ppout=s,_Tmp0->links=links;_Tmp0;});}}
# 371
static struct Cyc_PP_Doc*Cyc_PP_long_cats(struct Cyc_List_List*doclist){
# 375
struct Cyc_List_List*orig=doclist;
struct Cyc_PP_Doc*d=(struct Cyc_PP_Doc*)_check_null(doclist)->hd;
doclist=doclist->tl;{
int mw=d->mw;
int mwo=d->mw;
# 381
{struct Cyc_List_List*ds=doclist;for(0;ds!=0;ds=ds->tl){
int mw2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)ds->hd)->mwo);
int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc*)ds->hd)->mw);
mw=mw2;
mwo=mwo2;}}
# 387
return({struct Cyc_PP_Doc*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Doc));_Tmp0->mwo=mw,_Tmp0->mw=mwo,({struct Cyc_Fn_Function*_Tmp1=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_List_List*,struct Cyc_PP_Ppstate*),struct Cyc_List_List*))Cyc_Fn_make_fn;})(Cyc_PP_long_cats_f,orig);_Tmp0->f=_Tmp1;});_Tmp0;});}}
# 390
struct Cyc_PP_Doc*Cyc_PP_cats(struct Cyc_List_List*doclist){
if(doclist==0)return Cyc_PP_nil_doc();else{
if(doclist->tl==0)return(struct Cyc_PP_Doc*)doclist->hd;else{
# 394
if(Cyc_List_length(doclist)> 30)return Cyc_PP_long_cats(doclist);else{
return({struct Cyc_PP_Doc*_Tmp0=(struct Cyc_PP_Doc*)doclist->hd;Cyc_PP_concat(_Tmp0,Cyc_PP_cats(doclist->tl));});}}}}
# 398
static struct Cyc_PP_Out*Cyc_PP_cats_arr_f(struct _fat_ptr*docs_ptr,struct Cyc_PP_Ppstate*st){
struct Cyc_List_List*os=0;
struct _fat_ptr docs=*docs_ptr;
int sz=(int)_get_fat_size(docs,sizeof(struct Cyc_PP_Doc*));
{int i=0;for(0;i < sz;++ i){
struct Cyc_PP_Doc*d=*((struct Cyc_PP_Doc**)_check_fat_subscript(docs,sizeof(struct Cyc_PP_Doc*),i));
struct Cyc_PP_Out*o=({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st);
st=({struct Cyc_PP_Ppstate*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Ppstate));_Tmp0->ci=st->ci,_Tmp0->cc=o->newcc,_Tmp0->cl=o->newcl,_Tmp0->pw=st->pw,_Tmp0->epw=st->epw - d->mwo;_Tmp0;});
os=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=o,_Tmp0->tl=os;_Tmp0;});}}{
# 408
int newcc=((struct Cyc_PP_Out*)_check_null(os)->hd)->newcc;
int newcl=((struct Cyc_PP_Out*)os->hd)->newcl;
void*s=(void*)& Cyc_PP_Empty_stringptr;
void*links=(void*)& Cyc_PP_Empty_link;
for(1;os!=0;os=os->tl){
s=Cyc_PP_append(((struct Cyc_PP_Out*)os->hd)->ppout,s);
links=Cyc_PP_append(((struct Cyc_PP_Out*)os->hd)->links,links);}
# 416
return({struct Cyc_PP_Out*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Out));_Tmp0->newcc=newcc,_Tmp0->newcl=newcl,_Tmp0->ppout=s,_Tmp0->links=links;_Tmp0;});}}
# 419
struct Cyc_PP_Doc*Cyc_PP_cats_arr(struct _fat_ptr docs){
int sz=(int)_get_fat_size(docs,sizeof(struct Cyc_PP_Doc*));
if(sz==0)
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("cats_arr -- size zero array",sizeof(char),28U);_Tmp0;}));{
struct Cyc_PP_Doc*d=((struct Cyc_PP_Doc**)docs.curr)[0];
int mw=d->mw;
int mwo=d->mw;
{int i=1;for(0;i < sz;++ i){
int mw2=Cyc_PP_min(mw,mwo + (*((struct Cyc_PP_Doc**)_check_fat_subscript(docs,sizeof(struct Cyc_PP_Doc*),i)))->mwo);
int mwo2=Cyc_PP_min(mw,mwo + ((struct Cyc_PP_Doc**)docs.curr)[i]->mw);
mw=mw2;
mwo=mwo2;}}
# 432
return({struct Cyc_PP_Doc*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Doc));_Tmp0->mwo=mw,_Tmp0->mw=mwo,({struct Cyc_Fn_Function*_Tmp1=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _fat_ptr*,struct Cyc_PP_Ppstate*),struct _fat_ptr*))Cyc_Fn_make_fn;})(Cyc_PP_cats_arr_f,({unsigned _Tmp2=1;({struct _fat_ptr*_Tmp3=_cycalloc(_check_times(_Tmp2,sizeof(struct _fat_ptr)));_Tmp3[0]=docs;_Tmp3;});}));_Tmp0->f=_Tmp1;});_Tmp0;});}}
# 435
static struct Cyc_PP_Out*Cyc_PP_doc_union_f(struct _tuple6*clo,struct Cyc_PP_Ppstate*st){
void*_Tmp0;void*_Tmp1;_Tmp1=clo->f0;_Tmp0=clo->f1;{struct Cyc_PP_Doc*d=_Tmp1;struct Cyc_PP_Doc*d2=_Tmp0;
int dfits=st->cc + d->mwo <= st->epw || st->cc + d->mw <= st->pw;
# 439
if(dfits)return({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st);else{
return({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d2->f,st);}}}
# 442
struct Cyc_PP_Doc*Cyc_PP_doc_union(struct Cyc_PP_Doc*d,struct Cyc_PP_Doc*d2){
return({struct Cyc_PP_Doc*_Tmp0=_cycalloc(sizeof(struct Cyc_PP_Doc));({int _Tmp1=Cyc_PP_min(d->mwo,d2->mwo);_Tmp0->mwo=_Tmp1;}),({
int _Tmp1=Cyc_PP_min(d->mw,d2->mw);_Tmp0->mw=_Tmp1;}),({
struct Cyc_Fn_Function*_Tmp1=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct _tuple6*,struct Cyc_PP_Ppstate*),struct _tuple6*))Cyc_Fn_make_fn;})(Cyc_PP_doc_union_f,({struct _tuple6*_Tmp2=_cycalloc(sizeof(struct _tuple6));_Tmp2->f0=d,_Tmp2->f1=d2;_Tmp2;}));_Tmp0->f=_Tmp1;});_Tmp0;});}
# 449
struct Cyc_PP_Doc*Cyc_PP_oline_doc (void){
return({struct Cyc_PP_Doc*_Tmp0=Cyc_PP_nil_doc();Cyc_PP_doc_union(_Tmp0,Cyc_PP_line_doc());});}
# 453
static struct Cyc_PP_Out*Cyc_PP_tab_f(struct Cyc_PP_Doc*d,struct Cyc_PP_Ppstate*st){
struct Cyc_PP_Ppstate*st2;st2=_cycalloc(sizeof(struct Cyc_PP_Ppstate)),st2->ci=st->cc,st2->cc=st->cc,st2->cl=st->cl,st2->pw=st->pw,st2->epw=st->epw;
return({(struct Cyc_PP_Out*(*)(struct Cyc_Fn_Function*,struct Cyc_PP_Ppstate*))Cyc_Fn_apply;})(d->f,st2);}
# 457
struct Cyc_PP_Doc*Cyc_PP_tab(struct Cyc_PP_Doc*d){
struct Cyc_PP_Doc*d2;
d2=_cycalloc(sizeof(struct Cyc_PP_Doc)),d2->mwo=d->mwo,d2->mw=d->mw,({
# 461
struct Cyc_Fn_Function*_Tmp0=({(struct Cyc_Fn_Function*(*)(struct Cyc_PP_Out*(*)(struct Cyc_PP_Doc*,struct Cyc_PP_Ppstate*),struct Cyc_PP_Doc*))Cyc_Fn_make_fn;})(Cyc_PP_tab_f,d);d2->f=_Tmp0;});
return d2;}
# 467
static struct Cyc_PP_Doc*Cyc_PP_ppseq_f(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){
if(l==0)return Cyc_PP_nil_doc();else{
if(l->tl==0)return pp(l->hd);else{
return({struct Cyc_PP_Doc*_Tmp0[4];({struct Cyc_PP_Doc*_Tmp1=pp(l->hd);_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(sep);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_oline_doc();_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_ppseq_f(pp,sep,l->tl);_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});}}}
# 473
struct Cyc_PP_Doc*Cyc_PP_ppseq(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseq_f(pp,sep,l));}
# 477
struct Cyc_PP_Doc*Cyc_PP_seq_f(struct _fat_ptr sep,struct Cyc_List_List*l){
if(l==0)return Cyc_PP_nil_doc();else{
if(l->tl==0)return(struct Cyc_PP_Doc*)l->hd;else{
# 481
struct Cyc_PP_Doc*sep2=Cyc_PP_text(sep);
struct Cyc_PP_Doc*oline=Cyc_PP_oline_doc();
struct Cyc_List_List*x=l;
while(_check_null(x)->tl!=0){
struct Cyc_List_List*temp=x->tl;
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=sep2,({struct Cyc_List_List*_Tmp2=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=oline,_Tmp3->tl=temp;_Tmp3;});_Tmp1->tl=_Tmp2;});_Tmp1;});x->tl=_Tmp0;});
x=temp;}
# 489
return Cyc_PP_cats(l);}}}
# 497
struct Cyc_PP_Doc*Cyc_PP_seq(struct _fat_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_seq_f(sep,l));}
# 502
struct Cyc_PP_Doc*Cyc_PP_ppseql_f(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){
if(l==0)return Cyc_PP_nil_doc();else{
if(l->tl==0)return pp(l->hd);else{
return({struct Cyc_PP_Doc*_Tmp0[4];({struct Cyc_PP_Doc*_Tmp1=pp(l->hd);_Tmp0[0]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(sep);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_ppseql_f(pp,sep,l->tl);_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});}}}
# 507
struct Cyc_PP_Doc*Cyc_PP_ppseql(struct Cyc_PP_Doc*(*pp)(void*),struct _fat_ptr sep,struct Cyc_List_List*l){
return Cyc_PP_tab(Cyc_PP_ppseql_f(pp,sep,l));}
# 511
static struct Cyc_PP_Doc*Cyc_PP_seql_f(struct _fat_ptr sep,struct Cyc_List_List*l){
if(l==0)return Cyc_PP_nil_doc();else{
if(l->tl==0)return(struct Cyc_PP_Doc*)l->hd;else{
return({struct Cyc_PP_Doc*_Tmp0[4];_Tmp0[0]=(struct Cyc_PP_Doc*)l->hd,({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(sep);_Tmp0[1]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_line_doc();_Tmp0[2]=_Tmp1;}),({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_seql_f(sep,l->tl);_Tmp0[3]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),4));});}}}
# 516
struct Cyc_PP_Doc*Cyc_PP_seql(struct _fat_ptr sep,struct Cyc_List_List*l0){
return Cyc_PP_tab(Cyc_PP_seql_f(sep,l0));}
# 521
struct Cyc_PP_Doc*Cyc_PP_group(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(start);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_seq(sep,ss);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(stop);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
# 528
struct Cyc_PP_Doc*Cyc_PP_egroup(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){
if(ss==0)return Cyc_PP_nil_doc();else{
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(start);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_seq(sep,ss);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(stop);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}}
# 535
struct Cyc_PP_Doc*Cyc_PP_groupl(struct _fat_ptr start,struct _fat_ptr stop,struct _fat_ptr sep,struct Cyc_List_List*ss){
return({struct Cyc_PP_Doc*_Tmp0[3];({struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(start);_Tmp0[0]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_seql(sep,ss);_Tmp0[1]=_Tmp1;}),({
struct Cyc_PP_Doc*_Tmp1=Cyc_PP_text(stop);_Tmp0[2]=_Tmp1;});Cyc_PP_cat(_tag_fat(_Tmp0,sizeof(struct Cyc_PP_Doc*),3));});}
