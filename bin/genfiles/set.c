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

# 173 "core.h"
 extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 117
extern void Cyc_List_app(void*(*)(void*),struct Cyc_List_List*);
# 133
extern void Cyc_List_iter(void(*)(void*),struct Cyc_List_List*);
# 135
extern void Cyc_List_iter_c(void(*)(void*,void*),void*,struct Cyc_List_List*);
# 210
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Set_Set;extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Set_Set{int(*cmp)(void*,void*);int cardinality;struct Cyc_List_List*nodes;};
# 37 "set.cyc"
struct Cyc_Set_Set*Cyc_Set_empty(int(*comp)(void*,void*)){
struct Cyc_Set_Set*_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set));_Tmp0->cmp=comp,_Tmp0->cardinality=0,_Tmp0->nodes=0;return _Tmp0;}
# 40
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*rgn,int(*comp)(void*,void*)){
struct Cyc_Set_Set*_Tmp0=_region_malloc(rgn,0U,sizeof(struct Cyc_Set_Set));_Tmp0->cmp=comp,_Tmp0->cardinality=0,_Tmp0->nodes=0;return _Tmp0;}
# 44
struct Cyc_Set_Set*Cyc_Set_singleton(int(*comp)(void*,void*),void*x){
struct Cyc_Set_Set*_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set));_Tmp0->cmp=comp,_Tmp0->cardinality=1,({struct Cyc_List_List*_Tmp1=({struct Cyc_List_List*_Tmp2=_cycalloc(sizeof(struct Cyc_List_List));_Tmp2->hd=x,_Tmp2->tl=0;_Tmp2;});_Tmp0->nodes=_Tmp1;});return _Tmp0;}
# 48
int Cyc_Set_cardinality(struct Cyc_Set_Set*s){
return s->cardinality;}
# 53
int Cyc_Set_is_empty(struct Cyc_Set_Set*s){
return s->cardinality==0;}
# 58
static int Cyc_Set_member_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
for(1;n!=0;n=n->tl){
int i=cmp(elt,n->hd);
if(i==0)return 1;
if(i < 0)return 0;}
# 64
return 0;}
# 67
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt){
return Cyc_Set_member_b(s->cmp,s->nodes,elt);}
# 74
static struct Cyc_List_List*Cyc_Set_insert_b(struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
# 77
if(n==0){
struct Cyc_List_List*_Tmp0=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=elt,_Tmp0->tl=0;return _Tmp0;}{
int i=cmp(elt,n->hd);
if(i < 0){
struct Cyc_List_List*_Tmp0=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=elt,_Tmp0->tl=n;return _Tmp0;}{
struct Cyc_List_List*result;result=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List)),result->hd=n->hd,result->tl=0;{
struct Cyc_List_List*prev=result;
n=n->tl;
while(n!=0 &&(i=cmp(n->hd,elt))< 0){
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=n->hd,_Tmp1->tl=0;_Tmp1;});prev->tl=_Tmp0;});
prev=prev->tl;
n=n->tl;
# 86
1U;}
# 90
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=elt,_Tmp1->tl=n;_Tmp1;});prev->tl=_Tmp0;});
return result;}}}}
# 95
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return s;{
struct Cyc_Set_Set*_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set));_Tmp0->cmp=s->cmp,_Tmp0->cardinality=s->cardinality + 1,({
struct Cyc_List_List*_Tmp1=Cyc_Set_insert_b(Cyc_Core_heap_region,s->cmp,s->nodes,elt);_Tmp0->nodes=_Tmp1;});return _Tmp0;}}
# 100
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return s;{
struct Cyc_Set_Set*_Tmp0=_region_malloc(rgn,0U,sizeof(struct Cyc_Set_Set));_Tmp0->cmp=s->cmp,_Tmp0->cardinality=s->cardinality + 1,({
struct Cyc_List_List*_Tmp1=Cyc_Set_insert_b(rgn,s->cmp,s->nodes,elt);_Tmp0->nodes=_Tmp1;});return _Tmp0;}}
# 108
static struct Cyc_List_List*Cyc_Set_imp_insert_b(struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
# 111
if(n==0){
struct Cyc_List_List*_Tmp0=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=elt,_Tmp0->tl=0;return _Tmp0;}{
int i=cmp(elt,n->hd);
if(i < 0){
struct Cyc_List_List*_Tmp0=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=elt,_Tmp0->tl=n;return _Tmp0;}{
struct Cyc_List_List*prev=n;struct Cyc_List_List*res=n;
n=n->tl;
while(n!=0 &&(i=cmp(n->hd,elt))< 0){
prev=prev->tl;
n=n->tl;
# 119
1U;}
# 122
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List));_Tmp1->hd=elt,_Tmp1->tl=n;_Tmp1;});prev->tl=_Tmp0;});
return res;}}}
# 127
void Cyc_Set_imp_insert(struct Cyc_Set_Set*s,void*elt){
if(!Cyc_Set_member(s,elt)){
({struct Cyc_List_List*_Tmp0=Cyc_Set_imp_insert_b(Cyc_Core_heap_region,s->cmp,s->nodes,elt);s->nodes=_Tmp0;});
++ s->cardinality;}}
# 133
void Cyc_Set_imp_rinsert(struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){
if(!Cyc_Set_member(s,elt)){
({struct Cyc_List_List*_Tmp0=Cyc_Set_imp_insert_b(rgn,s->cmp,s->nodes,elt);s->nodes=_Tmp0;});
++ s->cardinality;}}
# 141
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
if(s1==s2)
return s1;
if(s1->cardinality==0)
return s2;
if(s2->cardinality==0)
return s1;{
# 149
struct Cyc_List_List*nodes=0;
int cardinality=0;
int(*comp)(void*,void*)=s1->cmp;
# 153
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
struct Cyc_List_List*curr=0;
# 157
while(x1!=0 && x2!=0){
{int i=comp(x1->hd,x2->hd);
if(i==0)
# 161
x2=x2->tl;else{
if(i < 0){
# 164
if(curr==0){
nodes=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x1->hd,_Tmp0->tl=0;_Tmp0;});
curr=nodes;}else{
# 168
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x1->hd,_Tmp1->tl=0;_Tmp1;});curr->tl=_Tmp0;});
curr=curr->tl;}
# 171
x1=x1->tl;
++ cardinality;}else{
# 175
if(curr==0){
nodes=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x2->hd,_Tmp0->tl=0;_Tmp0;});
curr=nodes;}else{
# 179
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x2->hd,_Tmp1->tl=0;_Tmp1;});curr->tl=_Tmp0;});
curr=curr->tl;}
# 182
x2=x2->tl;
++ cardinality;}}}
# 158
1U;}
# 186
if(x1!=0){
# 188
if(curr==0)
nodes=x1;else{
# 191
curr->tl=x1;}
({int _Tmp0=Cyc_List_length(x1);cardinality +=_Tmp0;});}else{
if(x2!=0){
# 195
if(curr==0)
nodes=x2;else{
# 198
curr->tl=x2;}
({int _Tmp0=Cyc_List_length(x2);cardinality +=_Tmp0;});}}{
# 201
struct Cyc_Set_Set*_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set));_Tmp0->cmp=comp,_Tmp0->cardinality=cardinality,_Tmp0->nodes=nodes;return _Tmp0;}}}
# 206
static struct Cyc_List_List*Cyc_Set_delete_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
if(cmp(n->hd,elt)==0)return n->tl;{
# 209
struct Cyc_List_List*result;result=_cycalloc(sizeof(struct Cyc_List_List)),result->hd=n->hd,result->tl=0;{
struct Cyc_List_List*prev=result;
n=n->tl;
while(n!=0 && cmp(n->hd,elt)!=0){
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=n->hd,_Tmp1->tl=0;_Tmp1;});prev->tl=_Tmp0;});
prev=prev->tl;
n=n->tl;
# 213
1U;}
# 217
prev->tl=n->tl;
return result;}}}
# 222
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt)){
struct Cyc_Set_Set*_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set));_Tmp0->cmp=s->cmp,_Tmp0->cardinality=s->cardinality - 1,({
struct Cyc_List_List*_Tmp1=Cyc_Set_delete_b(s->cmp,s->nodes,elt);_Tmp0->nodes=_Tmp1;});return _Tmp0;}
return s;}
# 232
static struct Cyc_List_List*Cyc_Set_imp_delete_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt,void**ret){
if(cmp(n->hd,elt)==0)return n->tl;{
# 235
struct Cyc_List_List*prev=n;struct Cyc_List_List*res=n;
n=n->tl;
while(n!=0 && cmp(n->hd,elt)!=0){
prev=prev->tl;
n=n->tl;
# 238
1U;}
# 241
prev->tl=n->tl;
*ret=n->hd;
return res;}}
# 247
void*Cyc_Set_imp_delete(struct Cyc_Set_Set*s,void*elt){
void*ret=elt;
if(Cyc_Set_member(s,elt)){
({struct Cyc_List_List*_Tmp0=Cyc_Set_imp_delete_b(s->cmp,s->nodes,elt,& ret);s->nodes=_Tmp0;});
-- s->cardinality;}
# 253
return ret;}
# 257
void*Cyc_Set_fold(void*(*f)(void*,void*),struct Cyc_Set_Set*s,void*accum){
{struct Cyc_List_List*n=s->nodes;for(0;n!=0;n=n->tl){
accum=f(n->hd,accum);}}
return accum;}
# 262
void*Cyc_Set_fold_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Set_Set*s,void*accum){
{struct Cyc_List_List*n=s->nodes;for(0;n!=0;n=n->tl){
accum=f(env,n->hd,accum);}}
return accum;}
# 269
void Cyc_Set_app(void*(*f)(void*),struct Cyc_Set_Set*s){
Cyc_List_app(f,s->nodes);}
# 272
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s){
Cyc_List_iter(f,s->nodes);}
# 275
void Cyc_Set_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Set_Set*s){
Cyc_List_iter_c(f,env,s->nodes);}
# 282
struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
if(s1==s2)
return s1;{
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
struct Cyc_List_List*result=0;struct Cyc_List_List*prev=0;
int card=0;
if(x1==0)
return s1;
if(x2==0)
return s2;
# 295
while(x1!=0 && x2!=0){
{int i=comp(x1->hd,x2->hd);
if(i==0){
if(result==0){
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=x1->hd,_Tmp0->tl=0;_Tmp0;});
prev=result;}else{
# 302
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x1->hd,_Tmp1->tl=0;_Tmp1;});_check_null(prev)->tl=_Tmp0;});
prev=prev->tl;}
# 305
++ card;
x1=x1->tl;
x2=x2->tl;}else{
if(i < 0)
x1=x1->tl;else{
# 311
x2=x2->tl;}}}
# 296
1U;}{
# 314
struct Cyc_Set_Set*_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set));_Tmp0->cmp=comp,_Tmp0->cardinality=card,_Tmp0->nodes=result;return _Tmp0;}}}
# 317
struct Cyc_Set_Set*Cyc_Set_from_list(int(*comp)(void*,void*),struct Cyc_List_List*x){
struct Cyc_List_List*z=Cyc_List_merge_sort(comp,x);
# 320
{struct Cyc_List_List*y=z;for(0;y!=0;y=y->tl){
if(y->tl!=0 && comp(y->hd,y->tl->hd)==0)
y->tl=_check_null(y->tl)->tl;}}{
struct Cyc_Set_Set*_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set));_Tmp0->cmp=comp,({int _Tmp1=Cyc_List_length(z);_Tmp0->cardinality=_Tmp1;}),_Tmp0->nodes=z;return _Tmp0;}}
# 326
struct Cyc_List_List*Cyc_Set_to_list(struct Cyc_Set_Set*s){
return s->nodes;}
# 330
int Cyc_Set_subset(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
# 335
while(1){
if(x1==0)return 1;
if(x2==0)return 0;{
int i=comp(x1->hd,x2->hd);
if(i < 0)
return 0;
if(i==0)
x1=x1->tl;
x2=x2->tl;}
# 336
1U;}
# 345
return 1;}
# 348
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
int card=s1->cardinality;
# 354
if(x2==0)return s1;
# 356
for(1;x2!=0;x2=x2->tl){
void*elt=x2->hd;
if(Cyc_Set_member_b(comp,x1,elt)){
-- card;
x1=Cyc_Set_delete_b(comp,x1,elt);}}{
# 363
struct Cyc_Set_Set*_Tmp0=_cycalloc(sizeof(struct Cyc_Set_Set));_Tmp0->cmp=comp,_Tmp0->cardinality=card,_Tmp0->nodes=x1;return _Tmp0;}}
# 366
int Cyc_Set_setcmp(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
if((unsigned)s1==(unsigned)s2)
return 0;
if(s1->cardinality!=s2->cardinality)
return s1->cardinality - s2->cardinality;{
# 372
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
int(*cmp)(void*,void*)=s1->cmp;
while(x1!=0){
{int diff=cmp(x1->hd,_check_null(x2)->hd);
if(diff!=0)return diff;
x1=x1->tl;
x2=x2->tl;}
# 376
1U;}
# 381
return 0;}}
# 384
int Cyc_Set_equals(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
return Cyc_Set_setcmp(s1,s2)==0;}char Cyc_Set_Absent[7U]="Absent";
# 389
struct Cyc_Set_Absent_exn_struct Cyc_Set_Absent_val={Cyc_Set_Absent};
# 393
void*Cyc_Set_choose(struct Cyc_Set_Set*s){
if(s->nodes==0)_throw(& Cyc_Set_Absent_val);
return s->nodes->hd;}
# 398
int Cyc_Set_iter_f(struct Cyc_List_List**elts_left,void**dest){
if(!((unsigned)*elts_left))
return 0;
*dest=(*elts_left)->hd;
*elts_left=_check_null(*elts_left)->tl;
return 1;}
# 405
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s){
struct Cyc_Iter_Iter _Tmp0;({struct Cyc_List_List**_Tmp1=({struct Cyc_List_List**_Tmp2=_region_malloc(rgn,0U,sizeof(struct Cyc_List_List*));*_Tmp2=s->nodes;_Tmp2;});_Tmp0.env=_Tmp1;}),_Tmp0.next=Cyc_Set_iter_f;return _Tmp0;}
