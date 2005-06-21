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
 extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};
# 170 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);struct Cyc___cycFILE;
# 252 "cycboot.h"
extern int Cyc_getw(struct Cyc___cycFILE*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};char Cyc_Dict_Absent[7U]="Absent";char Cyc_Dict_Present[8U]="Present";
# 27 "dict.cyc"
struct Cyc_Dict_Absent_exn_struct Cyc_Dict_Absent_val={Cyc_Dict_Absent};
struct Cyc_Dict_Present_exn_struct Cyc_Dict_Present_val={Cyc_Dict_Present};
# 30
enum Cyc_Dict_Color{Cyc_Dict_R =0U,Cyc_Dict_B =1U};struct _tuple0{void*f0;void*f1;};struct Cyc_Dict_T{enum Cyc_Dict_Color color;const struct Cyc_Dict_T*left;const struct Cyc_Dict_T*right;struct _tuple0 key_val;};
# 40
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*r,int(*comp)(void*,void*)){
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=comp,_Tmp0.r=r,_Tmp0.t=0;_Tmp0;});}
# 43
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){
return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}
# 46
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,void*),void*key,void*data){
# 48
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=comp,_Tmp0.r=r,({const struct Cyc_Dict_T*_Tmp1=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_Tmp2=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_Tmp2->color=Cyc_Dict_B,_Tmp2->left=0,_Tmp2->right=0,_Tmp2->key_val.f0=key,_Tmp2->key_val.f1=data;_Tmp2;});_Tmp0.t=_Tmp1;});_Tmp0;});}
# 50
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*data){
return Cyc_Dict_rsingleton(Cyc_Core_heap_region,comp,key,data);}
# 54
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
const struct Cyc_Dict_T*t2=t;
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,_Tmp0.t=t2;_Tmp0;});}
# 60
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d){
return d.t==(const struct Cyc_Dict_T*)0;}
# 64
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*key){
int(*rel)(void*,void*)=d.rel;
const struct Cyc_Dict_T*t=d.t;
while(t!=(const struct Cyc_Dict_T*)0){
int i=rel(key,t->key_val.f0);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return 1;}}}
# 73
return 0;}
# 76
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*key){
int(*rel)(void*,void*)=d.rel;
const struct Cyc_Dict_T*t=d.t;
while(t!=(const struct Cyc_Dict_T*)0){
int i=rel(key,t->key_val.f0);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return t->key_val.f1;}}}
# 85
_throw(& Cyc_Dict_Absent_val);}
# 88
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*key){
const struct Cyc_Dict_T*t=d.t;
while(t!=(const struct Cyc_Dict_T*)0){
int i=cmp(key,t->key_val.f0);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return t->key_val.f1;}}}
# 96
_throw(& Cyc_Dict_Absent_val);}
# 99
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*key){
int(*rel)(void*,void*)=d.rel;
const struct Cyc_Dict_T*t=d.t;
while(t!=(const struct Cyc_Dict_T*)0){
int i=rel(key,t->key_val.f0);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return& t->key_val.f1;}}}
# 108
return 0;}
# 111
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*key,void**ans_place){
int(*rel)(void*,void*)=d.rel;
const struct Cyc_Dict_T*t=d.t;
while(t!=(const struct Cyc_Dict_T*)0){
int i=rel(key,t->key_val.f0);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
# 119
*ans_place=t->key_val.f1;
return 1;}}}
# 123
return 0;}struct _tuple1{enum Cyc_Dict_Color f0;const struct Cyc_Dict_T*f1;const struct Cyc_Dict_T*f2;struct _tuple0 f3;};
# 126
static struct Cyc_Dict_T*Cyc_Dict_balance(struct _RegionHandle*r,struct _tuple1 quad){
# 129
enum Cyc_Dict_Color _Tmp0;struct _tuple0 _Tmp1;const struct Cyc_Dict_T*_Tmp2;struct _tuple0 _Tmp3;const struct Cyc_Dict_T*_Tmp4;struct _tuple0 _Tmp5;const struct Cyc_Dict_T*_Tmp6;const struct Cyc_Dict_T*_Tmp7;if(quad.f0==Cyc_Dict_B){if(quad.f1!=0){if(((const struct Cyc_Dict_T*)quad.f1)->color==Cyc_Dict_R){if(((const struct Cyc_Dict_T*)quad.f1)->left!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f1)->left)->color==Cyc_Dict_R){_Tmp7=quad.f1->left->left;_Tmp6=quad.f1->left->right;_Tmp5=quad.f1->left->key_val;_Tmp4=quad.f1->right;_Tmp3=quad.f1->key_val;_Tmp2=quad.f2;_Tmp1=quad.f3;{const struct Cyc_Dict_T*a=_Tmp7;const struct Cyc_Dict_T*b=_Tmp6;struct _tuple0 x=_Tmp5;const struct Cyc_Dict_T*c=_Tmp4;struct _tuple0 y=_Tmp3;const struct Cyc_Dict_T*d=_Tmp2;struct _tuple0 z=_Tmp1;
_Tmp7=a;_Tmp6=b;_Tmp4=c;_Tmp5=y;_Tmp3=x;_Tmp2=d;_Tmp1=z;goto _LL4;}}else{if(((const struct Cyc_Dict_T*)quad.f1)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f1)->right)->color==Cyc_Dict_R)goto _LL3;else{if(quad.f2!=0){if(((const struct Cyc_Dict_T*)quad.f2)->color==Cyc_Dict_R){if(((const struct Cyc_Dict_T*)quad.f2)->left!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->left)->color==Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(quad.f2!=0){if(((const struct Cyc_Dict_T*)quad.f2)->color==Cyc_Dict_R){if(((const struct Cyc_Dict_T*)quad.f2)->left!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->left)->color==Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(((const struct Cyc_Dict_T*)quad.f1)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f1)->right)->color==Cyc_Dict_R){_LL3: _Tmp7=quad.f1->left;_Tmp6=quad.f1->right->left;_Tmp4=quad.f1->right->right;_Tmp5=quad.f1->right->key_val;_Tmp3=quad.f1->key_val;_Tmp2=quad.f2;_Tmp1=quad.f3;_LL4: {const struct Cyc_Dict_T*a=_Tmp7;const struct Cyc_Dict_T*b=_Tmp6;const struct Cyc_Dict_T*c=_Tmp4;struct _tuple0 y=_Tmp5;struct _tuple0 x=_Tmp3;const struct Cyc_Dict_T*d=_Tmp2;struct _tuple0 z=_Tmp1;
_Tmp7=a;_Tmp6=b;_Tmp4=c;_Tmp5=y;_Tmp2=d;_Tmp3=z;_Tmp1=x;goto _LL6;}}else{if(quad.f2!=0){if(((const struct Cyc_Dict_T*)quad.f2)->color==Cyc_Dict_R){if(((const struct Cyc_Dict_T*)quad.f2)->left!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->left)->color==Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(quad.f2!=0){if(((const struct Cyc_Dict_T*)quad.f2)->color==Cyc_Dict_R){if(((const struct Cyc_Dict_T*)quad.f2)->left!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->left)->color==Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(quad.f2!=0){if(((const struct Cyc_Dict_T*)quad.f2)->color==Cyc_Dict_R){if(((const struct Cyc_Dict_T*)quad.f2)->left!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->left)->color==Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(quad.f2!=0){if(((const struct Cyc_Dict_T*)quad.f2)->color==Cyc_Dict_R){if(((const struct Cyc_Dict_T*)quad.f2)->left!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->left)->color==Cyc_Dict_R){_LL5: _Tmp7=quad.f1;_Tmp6=quad.f2->left->left;_Tmp4=quad.f2->left->right;_Tmp5=quad.f2->left->key_val;_Tmp2=quad.f2->right;_Tmp3=quad.f2->key_val;_Tmp1=quad.f3;_LL6: {const struct Cyc_Dict_T*a=_Tmp7;const struct Cyc_Dict_T*b=_Tmp6;const struct Cyc_Dict_T*c=_Tmp4;struct _tuple0 y=_Tmp5;const struct Cyc_Dict_T*d=_Tmp2;struct _tuple0 z=_Tmp3;struct _tuple0 x=_Tmp1;
_Tmp7=a;_Tmp6=b;_Tmp4=c;_Tmp2=d;_Tmp5=z;_Tmp3=y;_Tmp1=x;goto _LL8;}}else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)quad.f2)->right!=0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)quad.f2)->right)->color==Cyc_Dict_R){_LL7: _Tmp7=quad.f1;_Tmp6=quad.f2->left;_Tmp4=quad.f2->right->left;_Tmp2=quad.f2->right->right;_Tmp5=quad.f2->right->key_val;_Tmp3=quad.f2->key_val;_Tmp1=quad.f3;_LL8: {const struct Cyc_Dict_T*a=_Tmp7;const struct Cyc_Dict_T*b=_Tmp6;const struct Cyc_Dict_T*c=_Tmp4;const struct Cyc_Dict_T*d=_Tmp2;struct _tuple0 z=_Tmp5;struct _tuple0 y=_Tmp3;struct _tuple0 x=_Tmp1;
# 134
return({struct Cyc_Dict_T*_Tmp8=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_Tmp8->color=Cyc_Dict_R,({const struct Cyc_Dict_T*_Tmp9=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_TmpA=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_TmpA->color=Cyc_Dict_B,_TmpA->left=a,_TmpA->right=b,_TmpA->key_val=x;_TmpA;});_Tmp8->left=_Tmp9;}),({const struct Cyc_Dict_T*_Tmp9=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_TmpA=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_TmpA->color=Cyc_Dict_B,_TmpA->left=c,_TmpA->right=d,_TmpA->key_val=z;_TmpA;});_Tmp8->right=_Tmp9;}),_Tmp8->key_val=y;_Tmp8;});}}else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{_LL9: _Tmp0=quad.f0;_Tmp7=quad.f1;_Tmp6=quad.f2;_Tmp5=quad.f3;{enum Cyc_Dict_Color a=_Tmp0;const struct Cyc_Dict_T*b=_Tmp7;const struct Cyc_Dict_T*c=_Tmp6;struct _tuple0 d=_Tmp5;
# 136
return({struct Cyc_Dict_T*_Tmp8=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_Tmp8->color=a,_Tmp8->left=b,_Tmp8->right=c,_Tmp8->key_val=d;_Tmp8;});}};}
# 140
static struct Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,const struct Cyc_Dict_T*t){
# 142
struct _tuple0 _Tmp0;const struct Cyc_Dict_T*_Tmp1;const struct Cyc_Dict_T*_Tmp2;enum Cyc_Dict_Color _Tmp3;if(t==0)
return({struct Cyc_Dict_T*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_Tmp4->color=Cyc_Dict_R,_Tmp4->left=0,_Tmp4->right=0,_Tmp4->key_val=key_val;_Tmp4;});else{_Tmp3=t->color;_Tmp2=t->left;_Tmp1=t->right;_Tmp0=t->key_val;{enum Cyc_Dict_Color color=_Tmp3;const struct Cyc_Dict_T*a=_Tmp2;const struct Cyc_Dict_T*b=_Tmp1;struct _tuple0 y=_Tmp0;
# 145
int i=rel(key_val.f0,y.f0);
if(i < 0)return({struct _RegionHandle*_Tmp4=r;Cyc_Dict_balance(_Tmp4,({struct _tuple1 _Tmp5;_Tmp5.f0=color,({const struct Cyc_Dict_T*_Tmp6=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,a);_Tmp5.f1=_Tmp6;}),_Tmp5.f2=b,_Tmp5.f3=y;_Tmp5;}));});
if(i > 0)return({struct _RegionHandle*_Tmp4=r;Cyc_Dict_balance(_Tmp4,({struct _tuple1 _Tmp5;_Tmp5.f0=color,_Tmp5.f1=a,({const struct Cyc_Dict_T*_Tmp6=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,b);_Tmp5.f2=_Tmp6;}),_Tmp5.f3=y;_Tmp5;}));});
return({struct Cyc_Dict_T*_Tmp4=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_Tmp4->color=color,_Tmp4->left=a,_Tmp4->right=b,_Tmp4->key_val=key_val;_Tmp4;});}};}
# 152
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){
struct Cyc_Dict_T*ans=({struct _RegionHandle*_Tmp0=d.r;int(*_Tmp1)(void*,void*)=d.rel;struct _tuple0 _Tmp2=({struct _tuple0 _Tmp3;_Tmp3.f0=key,_Tmp3.f1=data;_Tmp3;});Cyc_Dict_ins(_Tmp0,_Tmp1,_Tmp2,d.t);});
ans->color=1U;
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=d.r,_Tmp0.t=(const struct Cyc_Dict_T*)ans;_Tmp0;});}
# 159
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*key,void*data){
if(Cyc_Dict_member(d,key))
_throw(& Cyc_Dict_Absent_val);
return Cyc_Dict_insert(d,key,data);}
# 165
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*kds){
# 167
for(1;kds!=0;kds=kds->tl){
d=Cyc_Dict_insert(d,((struct _tuple0*)kds->hd)[0].f0,((struct _tuple0*)kds->hd)[0].f1);}
return d;}
# 172
static void*Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),const struct Cyc_Dict_T*t,void*accum){
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
if(left!=(const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,left,accum);
accum=f(key,val,accum);
if(right!=(const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,right,accum);
return accum;}}
# 180
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*t=d.t;
if(t==(const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree(f,t,accum);}
# 187
static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,const struct Cyc_Dict_T*t,void*accum){
# 189
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
if(left!=(const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,left,accum);
accum=f(env,key,val,accum);
if(right!=(const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,right,accum);
return accum;}}
# 198
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*t=d.t;
if(t==(const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree_c(f,env,t,accum);}
# 205
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),const struct Cyc_Dict_T*t){
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
if(left!=(const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,left);
f(key,val);
if(right!=(const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,right);}}
# 212
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t!=(const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree(f,t);}
# 218
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
if(left!=(const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,left);
f(env,key,val);
if(right!=(const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,right);}}
# 225
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t!=(const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree_c(f,env,t);}
# 231
static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*t){
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
if(left!=(const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,left);
f(key,val);
if(right!=(const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,right);}}
# 238
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t!=(const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree(f,t);}
# 244
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
if(left!=(const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,left);
f(env,key,val);
if(right!=(const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,right);}}
# 251
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t!=(const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree_c(f,env,t);}
# 257
static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){
*cnt=*cnt + 1;}
# 260
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){
int num=0;
({void(*_Tmp0)(void(*)(int*,void*,void*),int*,struct Cyc_Dict_Dict)=(void(*)(void(*)(int*,void*,void*),int*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp0;})(Cyc_Dict_count_elem,& num,d);
return num;}struct _tuple2{void(*f0)(void*,void*);struct Cyc_Dict_Dict f1;};
# 266
static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*b1){
# 268
struct Cyc_Dict_Dict _Tmp0;void*_Tmp1;_Tmp1=env->f0;_Tmp0=env->f1;{void(*f)(void*,void*)=_Tmp1;struct Cyc_Dict_Dict d2=_Tmp0;
({void(*_Tmp2)(void*,void*)=f;void*_Tmp3=b1;_Tmp2(_Tmp3,Cyc_Dict_lookup(d2,a));});}}
# 272
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 275
struct _tuple2 env=({struct _tuple2 _Tmp0;_Tmp0.f0=f,_Tmp0.f1=d2;_Tmp0;});
({void(*_Tmp0)(void(*)(struct _tuple2*,void*,void*),struct _tuple2*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple2*,void*,void*),struct _tuple2*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp0;})(Cyc_Dict_iter2_f,& env,d1);}struct _tuple3{void(*f0)(void*,void*,void*);struct Cyc_Dict_Dict f1;void*f2;};
# 279
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){
# 281
void*_Tmp0;struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;_Tmp2=env->f0;_Tmp1=env->f1;_Tmp0=(void*)env->f2;{void(*f)(void*,void*,void*)=_Tmp2;struct Cyc_Dict_Dict d2=_Tmp1;void*inner_env=_Tmp0;
({void(*_Tmp3)(void*,void*,void*)=f;void*_Tmp4=inner_env;void*_Tmp5=b1;_Tmp3(_Tmp4,_Tmp5,Cyc_Dict_lookup(d2,a));});}}
# 285
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 288
struct _tuple3 env=({struct _tuple3 _Tmp0;_Tmp0.f0=f,_Tmp0.f1=d2,_Tmp0.f2=inner_env;_Tmp0;});
({void(*_Tmp0)(void(*)(struct _tuple3*,void*,void*),struct _tuple3*,struct Cyc_Dict_Dict)=(void(*)(void(*)(struct _tuple3*,void*,void*),struct _tuple3*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;_Tmp0;})(Cyc_Dict_iter2_c_f,& env,d1);}struct _tuple4{void*(*f0)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict f1;void*f2;};
# 292
static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*accum){
# 296
void*_Tmp0;struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;_Tmp2=env->f0;_Tmp1=env->f1;_Tmp0=(void*)env->f2;{void*(*f)(void*,void*,void*,void*,void*)=_Tmp2;struct Cyc_Dict_Dict d2=_Tmp1;void*inner_env=_Tmp0;
return({void*(*_Tmp3)(void*,void*,void*,void*,void*)=f;void*_Tmp4=inner_env;void*_Tmp5=a;void*_Tmp6=b1;void*_Tmp7=Cyc_Dict_lookup(d2,a);_Tmp3(_Tmp4,_Tmp5,_Tmp6,_Tmp7,accum);});}}
# 300
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){
# 304
struct _tuple4 env=({struct _tuple4 _Tmp0;_Tmp0.f0=f,_Tmp0.f1=d2,_Tmp0.f2=inner_env;_Tmp0;});
return({void*(*_Tmp0)(void*(*)(struct _tuple4*,void*,void*,void*),struct _tuple4*,struct Cyc_Dict_Dict,void*)=(void*(*)(void*(*)(struct _tuple4*,void*,void*,void*),struct _tuple4*,struct Cyc_Dict_Dict,void*))Cyc_Dict_fold_c;_Tmp0;})(Cyc_Dict_fold2_c_f,& env,d1,accum);}
# 308
static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,const struct Cyc_Dict_T*t){
if(t==(const struct Cyc_Dict_T*)0)return 0;else{
# 311
struct Cyc_Dict_T _Tmp0=*t;struct _tuple0 _Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;enum Cyc_Dict_Color _Tmp4;_Tmp4=_Tmp0.color;_Tmp3=_Tmp0.left;_Tmp2=_Tmp0.right;_Tmp1=_Tmp0.key_val;{enum Cyc_Dict_Color c=_Tmp4;const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;struct _tuple0 pr=_Tmp1;
const struct Cyc_Dict_T*new_left=Cyc_Dict_copy_tree(r2,left);
const struct Cyc_Dict_T*new_right=Cyc_Dict_copy_tree(r2,right);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_Tmp5=_region_malloc(r2,0U,sizeof(struct Cyc_Dict_T));_Tmp5->color=c,_Tmp5->left=new_left,_Tmp5->right=new_right,_Tmp5->key_val=pr;_Tmp5;});}}}
# 318
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r2,({const struct Cyc_Dict_T*_Tmp1=Cyc_Dict_copy_tree(r2,d.t);_Tmp0.t=_Tmp1;});_Tmp0;});}
# 322
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){
return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}
# 326
static const struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),const struct Cyc_Dict_T*t){
# 328
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;enum Cyc_Dict_Color _Tmp4;_Tmp4=t->color;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{enum Cyc_Dict_Color c=_Tmp4;const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
const struct Cyc_Dict_T*new_left=left==(const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,left);
void*new_val=f(val);
const struct Cyc_Dict_T*new_right=right==(const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,right);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_Tmp5->color=c,_Tmp5->left=new_left,_Tmp5->right=new_right,_Tmp5->key_val.f0=key,_Tmp5->key_val.f1=new_val;_Tmp5;});}}
# 335
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t==(const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,_Tmp0.t=0;_Tmp0;});
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,({const struct Cyc_Dict_T*_Tmp1=Cyc_Dict_map_tree(r,f,t);_Tmp0.t=_Tmp1;});_Tmp0;});}
# 342
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}
# 346
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Dict_T*t){
# 348
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;enum Cyc_Dict_Color _Tmp4;_Tmp4=t->color;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{enum Cyc_Dict_Color c=_Tmp4;const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
const struct Cyc_Dict_T*new_left=
left==(const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,left);
void*new_val=f(env,val);
const struct Cyc_Dict_T*new_right=
right==(const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,right);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_Tmp5=_region_malloc(r,0U,sizeof(struct Cyc_Dict_T));_Tmp5->color=c,_Tmp5->left=new_left,_Tmp5->right=new_right,_Tmp5->key_val.f0=key,_Tmp5->key_val.f1=new_val;_Tmp5;});}}
# 357
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
# 359
const struct Cyc_Dict_T*t=d.t;
if(t==(const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,_Tmp0.t=0;_Tmp0;});
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,({const struct Cyc_Dict_T*_Tmp1=Cyc_Dict_map_tree_c(r,f,env,t);_Tmp0.t=_Tmp1;});_Tmp0;});}
# 365
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap_c(Cyc_Core_heap_region,f,env,d);}
# 369
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
if(d.t==(const struct Cyc_Dict_T*)0)
_throw(& Cyc_Dict_Absent_val);
return({struct _tuple0*_Tmp0=_region_malloc(r,0U,sizeof(struct _tuple0));_Tmp0->f0=d.t->key_val.f0,_Tmp0->f1=d.t->key_val.f1;_Tmp0;});}
# 375
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
void*_Tmp0;void*_Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;_Tmp3=t->left;_Tmp2=t->right;_Tmp1=(void*)t->key_val.f0;_Tmp0=(void*)t->key_val.f1;{const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;void*key=_Tmp1;void*val=_Tmp0;
return
((left==(const struct Cyc_Dict_T*)0 || Cyc_Dict_forall_tree_c(f,env,left))&&
 f(env,key,val))&&(
right==(const struct Cyc_Dict_T*)0 || Cyc_Dict_forall_tree_c(f,env,right));}}
# 383
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t==(const struct Cyc_Dict_T*)0)
return 1;
return Cyc_Dict_forall_tree_c(f,env,t);}struct _tuple5{int(*f0)(void*,void*,void*);struct Cyc_Dict_Dict f1;};
# 390
static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
# 392
struct Cyc_Dict_Dict _Tmp0;void*_Tmp1;_Tmp1=env->f0;_Tmp0=env->f1;{int(*f)(void*,void*,void*)=_Tmp1;struct Cyc_Dict_Dict d2=_Tmp0;
if(Cyc_Dict_member(d2,a))
return({int(*_Tmp2)(void*,void*,void*)=f;void*_Tmp3=a;void*_Tmp4=b;_Tmp2(_Tmp3,_Tmp4,Cyc_Dict_lookup(d2,a));});
return 1;}}
# 399
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
struct _tuple5 env=({struct _tuple5 _Tmp0;_Tmp0.f0=f,_Tmp0.f1=d2;_Tmp0;});
return({int(*_Tmp0)(int(*)(struct _tuple5*,void*,void*),struct _tuple5*,struct Cyc_Dict_Dict)=(int(*)(int(*)(struct _tuple5*,void*,void*),struct _tuple5*,struct Cyc_Dict_Dict))Cyc_Dict_forall_c;_Tmp0;})(Cyc_Dict_forall_intersect_f,& env,d1);}struct _tuple6{void*(*f0)(void*,void*,void*,void*);void*f1;};
# 405
static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){
# 409
if(Cyc_Dict_member(*d1,a)){
# 411
void*old_val=Cyc_Dict_lookup(*d1,a);
void*new_val=(*env).f0((*env).f1,a,old_val,b);
if(new_val!=old_val)
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_insert(*d1,a,new_val);*d1=_Tmp0;});
return d1;}
# 417
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_insert(*d1,a,b);*d1=_Tmp0;});
return d1;}
# 421
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 424
if((int)d1.t==(int)d2.t)return d1;
if(d1.t==(const struct Cyc_Dict_T*)0)return d2;
if(d2.t==(const struct Cyc_Dict_T*)0)return d1;{
struct _tuple6 fenvpair=({struct _tuple6 _Tmp0;_Tmp0.f0=f,_Tmp0.f1=env;_Tmp0;});
({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict*(*)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_Tmp0;})(Cyc_Dict_union_f,& fenvpair,d2,& d1);
return d1;}}
# 432
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 435
const struct Cyc_Dict_T*t2=d2.t;
if(t2==(const struct Cyc_Dict_T*)0)return d2;
if((int)d1.t==(int)t2)return d2;{
const struct Cyc_Dict_T*ans_tree=0;
struct _RegionHandle _Tmp0=_new_region(0U,"temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);{
# 441
struct _fat_ptr queue=_tag_fat(({unsigned _Tmp1=16U;({const struct Cyc_Dict_T**_Tmp2=({struct _RegionHandle*_Tmp3=temp;_region_malloc(_Tmp3,0U,_check_times(_Tmp1,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _Tmp3=16U;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]=t2;}}0;});_Tmp2;});}),sizeof(const struct Cyc_Dict_T*),16U);
int ind=0;
while(ind!=-1){
const struct Cyc_Dict_T*_Tmp1=*((const struct Cyc_Dict_T**)_check_fat_subscript(queue,sizeof(const struct Cyc_Dict_T*),ind --));void*_Tmp2;void*_Tmp3;const struct Cyc_Dict_T*_Tmp4;const struct Cyc_Dict_T*_Tmp5;_Tmp5=_Tmp1->left;_Tmp4=_Tmp1->right;_Tmp3=(void*)_Tmp1->key_val.f0;_Tmp2=(void*)_Tmp1->key_val.f1;{const struct Cyc_Dict_T*l=_Tmp5;const struct Cyc_Dict_T*r=_Tmp4;void*k=_Tmp3;void*v=_Tmp2;
if((unsigned)(ind + 2)>= _get_fat_size(queue,sizeof(const struct Cyc_Dict_T*)))
queue=({unsigned _Tmp6=_get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))* (unsigned)2;_tag_fat(({const struct Cyc_Dict_T**_Tmp7=({struct _RegionHandle*_Tmp8=temp;_region_malloc(_Tmp8,0U,_check_times(_Tmp6,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _Tmp8=_get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))* (unsigned)2;unsigned i;for(i=0;i < _Tmp8;++ i){i < _get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))?_Tmp7[i]=*((const struct Cyc_Dict_T**)_check_fat_subscript(queue,sizeof(const struct Cyc_Dict_T*),(int)i)):(_Tmp7[i]=t2);}}0;});_Tmp7;}),sizeof(const struct Cyc_Dict_T*),_Tmp6);});
# 448
if(l!=(const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)_check_fat_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=l;
if(r!=(const struct Cyc_Dict_T*)0)*((const struct Cyc_Dict_T**)_check_fat_subscript(queue,sizeof(const struct Cyc_Dict_T*),++ ind))=r;
if(Cyc_Dict_member(d1,k))
ans_tree=(const struct Cyc_Dict_T*)({struct _RegionHandle*_Tmp6=d2.r;int(*_Tmp7)(void*,void*)=d2.rel;struct _tuple0 _Tmp8=({struct _tuple0 _Tmp9;_Tmp9.f0=k,({void*_TmpA=({void*(*_TmpB)(void*,void*,void*,void*)=f;void*_TmpC=env;void*_TmpD=k;void*_TmpE=Cyc_Dict_lookup(d1,k);_TmpB(_TmpC,_TmpD,_TmpE,v);});_Tmp9.f1=_TmpA;});_Tmp9;});Cyc_Dict_ins(_Tmp6,_Tmp7,_Tmp8,ans_tree);});}}}{
# 454
struct Cyc_Dict_Dict _Tmp1=({struct Cyc_Dict_Dict _Tmp2;_Tmp2.rel=d2.rel,_Tmp2.r=d2.r,_Tmp2.t=ans_tree;_Tmp2;});_npop_handler(0);return _Tmp1;}
# 439
;_pop_region();}}
# 457
static void*Cyc_Dict_intersect_f(void*(*f)(void*,void*,void*),void*a,void*b1,void*b2){
return f(a,b1,b2);}
# 461
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 464
return({struct Cyc_Dict_Dict(*_Tmp0)(void*(*)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(void*(*)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;_Tmp0;})(Cyc_Dict_intersect_f,f,d1,d2);}
# 467
static struct Cyc_List_List*Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
# 469
return({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_region_malloc(r,0U,sizeof(struct _tuple0));_Tmp2->f0=k,_Tmp2->f1=v;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=accum;_Tmp0;});}
# 472
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
return({struct Cyc_List_List*(*_Tmp0)(struct Cyc_List_List*(*)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold_c;_Tmp0;})(Cyc_Dict_to_list_f,r,d,0);}
# 475
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d){
return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f0)(void*,void*);struct _RegionHandle*f1;};
# 479
static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 481
void*_Tmp0;void*_Tmp1;_Tmp1=env->f0;_Tmp0=env->f1;{int(*f)(void*,void*)=_Tmp1;struct _RegionHandle*r=_Tmp0;
if(f(x,y))
({struct Cyc_Dict_Dict _Tmp2=Cyc_Dict_insert(*acc,x,y);*acc=_Tmp2;});
# 485
return acc;}}
# 488
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict d){
struct _tuple7 env=({struct _tuple7 _Tmp0;_Tmp0.f0=f,_Tmp0.f1=r2;_Tmp0;});
struct Cyc_Dict_Dict res=Cyc_Dict_rempty(r2,d.rel);
return*({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict*(*)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_Tmp0;})(Cyc_Dict_filter_f,& env,d,& res);}
# 499
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f0)(void*,void*,void*);void*f1;struct _RegionHandle*f2;};
# 504
static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 507
void*_Tmp0;void*_Tmp1;void*_Tmp2;_Tmp2=env->f0;_Tmp1=(void*)env->f1;_Tmp0=env->f2;{int(*f)(void*,void*,void*)=_Tmp2;void*f_env=_Tmp1;struct _RegionHandle*r2=_Tmp0;
if(f(f_env,x,y))
({struct Cyc_Dict_Dict _Tmp3=Cyc_Dict_insert(*acc,x,y);*acc=_Tmp3;});
return acc;}}
# 513
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 515
struct _tuple8 env=({struct _tuple8 _Tmp0;_Tmp0.f0=f,_Tmp0.f1=f_env,_Tmp0.f2=r2;_Tmp0;});
struct Cyc_Dict_Dict res=Cyc_Dict_rempty(r2,d.rel);
return*({struct Cyc_Dict_Dict*(*_Tmp0)(struct Cyc_Dict_Dict*(*)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*)=(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;_Tmp0;})(Cyc_Dict_filter_c_f,& env,d,& res);}
# 519
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 521
return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,f,f_env,d);}
# 525
static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
return !Cyc_Dict_member(*d,x);}
# 529
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*r2,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 531
return({struct Cyc_Dict_Dict(*_Tmp0)(struct _RegionHandle*,int(*)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;_Tmp0;})(r2,Cyc_Dict_difference_f,& d2,d1);}
# 533
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*f0)(void*,void*);void*f1;};
# 537
static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,void*y){
void*_Tmp0;void*_Tmp1;_Tmp1=env->f0;_Tmp0=(void*)env->f1;{int(*rel)(void*,void*)=_Tmp1;void*x0=_Tmp0;
return rel(x0,x)!=0;}}
# 542
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(r2,d);{
struct _tuple9 env=({struct _tuple9 _Tmp0;_Tmp0.f0=d.rel,_Tmp0.f1=x;_Tmp0;});
return({struct Cyc_Dict_Dict(*_Tmp0)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;_Tmp0;})(r2,Cyc_Dict_delete_f,& env,d);}}
# 548
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return d;{
struct _tuple9 env=({struct _tuple9 _Tmp0;_Tmp0.f0=d.rel,_Tmp0.f1=x;_Tmp0;});
return({struct Cyc_Dict_Dict(*_Tmp0)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict)=(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;_Tmp0;})(d.r,Cyc_Dict_delete_f,& env,d);}}
# 554
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct _fat_ptr f0;int f1;};
# 558
int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){
# 561
void*_Tmp0;struct _fat_ptr _Tmp1;_Tmp1=stk->f0;_Tmp0=(int*)& stk->f1;{struct _fat_ptr stack=_Tmp1;int*indp=(int*)_Tmp0;
int ind=*indp;
if(ind==-1)
return 0;{
const struct Cyc_Dict_T*t=*((const struct Cyc_Dict_T**)_check_fat_subscript(stack,sizeof(const struct Cyc_Dict_T*),ind));
*dest=_check_null(t)->key_val;
-- ind;
if((unsigned)t->left)
((const struct Cyc_Dict_T**)stack.curr)[++ ind]=t->left;
if((unsigned)t->right)
*((const struct Cyc_Dict_T**)_check_fat_subscript(stack,sizeof(const struct Cyc_Dict_T*),++ ind))=t->right;
*indp=ind;
return 1;}}}
# 578
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d){
# 581
int half_max_size=1;
const struct Cyc_Dict_T*t=d.t;
while(t!=(const struct Cyc_Dict_T*)0){
t=t->left;
++ half_max_size;}
# 587
t=d.t;{
struct _fat_ptr stack=({unsigned _Tmp0=(unsigned)(2 * half_max_size);_tag_fat(({const struct Cyc_Dict_T**_Tmp1=({struct _RegionHandle*_Tmp2=rgn;_region_malloc(_Tmp2,0U,_check_times(_Tmp0,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _Tmp2=(unsigned)(2 * half_max_size);unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=t;}}0;});_Tmp1;}),sizeof(const struct Cyc_Dict_T*),_Tmp0);});
return({struct Cyc_Iter_Iter _Tmp0;({struct _tuple10*_Tmp1=({struct _tuple10*_Tmp2=_region_malloc(rgn,0U,sizeof(struct _tuple10));_Tmp2->f0=stack,(unsigned)t?_Tmp2->f1=0:(_Tmp2->f1=- 1);_Tmp2;});_Tmp0.env=_Tmp1;}),_Tmp0.next=Cyc_Dict_iter_f;_Tmp0;});}}
# 597
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){
# 604
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);
int len=Cyc_List_length(dict_list);
# 608
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("Dict::marshal: Write failure",sizeof(char),29U);_Tmp0;}));
# 610
while(dict_list!=0){
env=({void*(*_Tmp0)(void*,struct Cyc___cycFILE*,struct _tuple0*)=(void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_key;_Tmp0;})(env,fp,(struct _tuple0*)dict_list->hd);
env=({void*(*_Tmp0)(void*,struct Cyc___cycFILE*,struct _tuple0*)=(void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_val;_Tmp0;})(env,fp,(struct _tuple0*)dict_list->hd);
dict_list=dict_list->tl;}
# 615
return env;}
# 618
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp){
# 625
struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);
int len=Cyc_getw(fp);
if(len==-1)
_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=_tag_fat("Dict::unmarshal: list length is -1",sizeof(char),35U);_Tmp0;}));
{int i=0;for(0;i < len;++ i){
void*key=read_key(env,fp);
void*val=read_val(env,fp);
dict=Cyc_Dict_insert(dict,key,val);}}
# 634
return dict;}
