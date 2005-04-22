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
 struct Cyc_Core_Opt{void*v;};struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 151 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;
# 170
extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 246 "list.h"
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*,int);struct _tuple0{struct Cyc_List_List*f0;struct Cyc_List_List*f1;};struct _tuple1{struct Cyc_List_List*f0;struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr);
# 374
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*,struct _fat_ptr);
# 30 "list.cyc"
int Cyc_List_length(struct Cyc_List_List*x){
int i=0;
for(1;x!=0;x=x->tl){
++ i;}
return i;}
# 38
void*Cyc_List_hd(struct Cyc_List_List*x){
return x->hd;}
# 43
struct Cyc_List_List*Cyc_List_tl(struct Cyc_List_List*x){
return x->tl;}
# 49
struct Cyc_List_List*Cyc_List_rlist(struct _RegionHandle*r,struct _fat_ptr argv){
return Cyc_List_rfrom_array(r,argv);}
# 53
struct Cyc_List_List*Cyc_List_list(struct _fat_ptr argv){
return Cyc_List_from_array(argv);}
# 58
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*r,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 61
if(x==0)return 0;
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=x->hd,_Tmp0->tl=0;_Tmp0;});
prev=result;
for(x=x->tl;x!=0;x=x->tl){
struct Cyc_List_List*temp;temp=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),temp->hd=x->hd,temp->tl=0;
prev->tl=temp;
prev=temp;}
# 69
return result;}
# 72
struct Cyc_List_List*Cyc_List_copy(struct Cyc_List_List*x){
return Cyc_List_rcopy(Cyc_Core_heap_region,x);}
# 77
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 80
if(x==0)return 0;
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=f(x->hd);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
prev=result;
for(x=x->tl;x!=0;x=x->tl){
struct Cyc_List_List*temp;temp=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),({void*_Tmp0=f(x->hd);temp->hd=_Tmp0;}),temp->tl=0;
prev->tl=temp;
prev=temp;}
# 88
return result;}
# 91
struct Cyc_List_List*Cyc_List_map(void*(*f)(void*),struct Cyc_List_List*x){
return Cyc_List_rmap(Cyc_Core_heap_region,f,x);}
# 96
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 99
if(x==0)return 0;
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=f(env,x->hd);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
prev=result;
for(x=x->tl;x!=0;x=x->tl){
struct Cyc_List_List*e;e=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),({void*_Tmp0=f(env,x->hd);e->hd=_Tmp0;}),e->tl=0;
prev->tl=e;
prev=e;}
# 107
return result;}
# 110
struct Cyc_List_List*Cyc_List_map_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
return Cyc_List_rmap_c(Cyc_Core_heap_region,f,env,x);}char Cyc_List_List_mismatch[14U]="List_mismatch";
# 116
struct Cyc_List_List_mismatch_exn_struct Cyc_List_List_mismatch_val={Cyc_List_List_mismatch};
# 121
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*r,void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 124
if(x==0 && y==0)return 0;
if(x==0 || y==0)_throw(& Cyc_List_List_mismatch_val);
# 127
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=f(x->hd,y->hd);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
prev=result;
x=x->tl;
y=y->tl;
# 132
while(x!=0 && y!=0){
struct Cyc_List_List*temp;temp=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),({void*_Tmp0=f(x->hd,y->hd);temp->hd=_Tmp0;}),temp->tl=0;
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;}
# 139
if(x!=0 || y!=0)_throw(& Cyc_List_List_mismatch_val);
return result;}
# 143
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rmap2(Cyc_Core_heap_region,f,x,y);}
# 148
struct Cyc_List_List*Cyc_List_rmap3(struct _RegionHandle*r,void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 150
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 152
if((x==0 && y==0)&& z==0)return 0;
if((x==0 || y==0)|| z==0)_throw(& Cyc_List_List_mismatch_val);
# 155
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=f(x->hd,y->hd,z->hd);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
prev=result;
# 158
x=x->tl;
y=y->tl;
z=z->tl;
# 162
while((x!=0 && y!=0)&& z!=0){
struct Cyc_List_List*temp;temp=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),({void*_Tmp0=f(x->hd,y->hd,z->hd);temp->hd=_Tmp0;}),temp->tl=0;
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;
z=z->tl;}
# 170
if((x!=0 || y!=0)|| z!=0)_throw(& Cyc_List_List_mismatch_val);
return result;}
# 174
struct Cyc_List_List*Cyc_List_map3(void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
return Cyc_List_rmap3(Cyc_Core_heap_region,f,x,y,z);}
# 181
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
f(x->hd);}}
# 186
void Cyc_List_app_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
f(env,x->hd);}}
# 194
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
f(x->hd);}}
# 199
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
f(env,x->hd);}}
# 206
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
for(1;x!=0 && y!=0;(x=x->tl,y=y->tl)){
f(x->hd,y->hd);}
if(x!=0 || y!=0)_throw(& Cyc_List_List_mismatch_val);}
# 212
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
for(1;x!=0 && y!=0;(x=x->tl,y=y->tl)){
f(env,x->hd,y->hd);}
if(x!=0 || y!=0)_throw(& Cyc_List_List_mismatch_val);}
# 220
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
for(1;x!=0 && y!=0;(x=x->tl,y=y->tl)){
f(x->hd,y->hd);}
if(x!=0 || y!=0)_throw(& Cyc_List_List_mismatch_val);}
# 225
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
for(1;x!=0 && y!=0;(x=x->tl,y=y->tl)){
f(env,x->hd,y->hd);}
if(x!=0 || y!=0)_throw(& Cyc_List_List_mismatch_val);}
# 234
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
accum=f(accum,x->hd);}
return accum;}
# 240
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*env,void*accum,struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
accum=f(env,accum,x->hd);}
return accum;}
# 249
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum){
if(x==0)return accum;
return({void*(*_Tmp0)(void*,void*)=f;void*_Tmp1=x->hd;_Tmp0(_Tmp1,Cyc_List_fold_right(f,x->tl,accum));});}
# 253
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,void*accum){
if(x==0)return accum;
return({void*(*_Tmp0)(void*,void*,void*)=f;void*_Tmp1=env;void*_Tmp2=x->hd;_Tmp0(_Tmp1,_Tmp2,Cyc_List_fold_right_c(f,env,x->tl,accum));});}
# 260
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 262
for(1;x!=0;x=x->tl){
y=({struct Cyc_List_List*_Tmp0=_region_malloc(r2,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=x->hd,_Tmp0->tl=y;_Tmp0;});}
return y;}
# 267
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rrevappend(Cyc_Core_heap_region,x,y);}
# 272
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*r2,struct Cyc_List_List*x){
return x==0?0: Cyc_List_rrevappend(r2,x,0);}
# 276
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x){
return Cyc_List_rrev(Cyc_Core_heap_region,x);}
# 281
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x){
if(x==0)return x;{
struct Cyc_List_List*first=x;
struct Cyc_List_List*second=x->tl;
x->tl=0;
while(second!=0){
struct Cyc_List_List*temp=second->tl;
second->tl=first;
first=second;
second=temp;}
# 292
return first;}}
# 296
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 299
if(x==0)return y;
if(y==0)return Cyc_List_rcopy(r2,x);
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r2,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=x->hd,_Tmp0->tl=0;_Tmp0;});
prev=result;
for(x=x->tl;x!=0;x=x->tl){
struct Cyc_List_List*temp;temp=_region_malloc(r2,0U,sizeof(struct Cyc_List_List)),temp->hd=x->hd,temp->tl=0;
prev->tl=temp;
prev=temp;}
# 308
prev->tl=y;
return result;}
# 312
struct Cyc_List_List*Cyc_List_append(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rappend(Cyc_Core_heap_region,x,y);}
# 318
struct Cyc_List_List*Cyc_List_imp_append(struct Cyc_List_List*x,struct Cyc_List_List*y){
struct Cyc_List_List*z;
# 321
if(x==0)return y;
if(y==0)return x;
for(z=x;_check_null(z)->tl!=0;z=z->tl){
;}
z->tl=y;
return x;}
# 330
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*r,struct Cyc_List_List*x){
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_right_c;})(Cyc_List_rappend,r,x,0);}
# 334
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x){
return Cyc_List_rflatten(Cyc_Core_heap_region,x);}
# 339
struct Cyc_List_List*Cyc_List_imp_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 342
if(a==0)return b;
if(b==0)return a;
# 349
if(less_eq(a->hd,b->hd)<= 0){
c=a;
a=a->tl;}else{
# 353
c=b;
b=b->tl;}
# 356
d=c;
# 358
while(a!=0 && b!=0){
# 360
if(less_eq(a->hd,b->hd)<= 0){
c->tl=a;
c=a;
a=a->tl;}else{
# 365
c->tl=b;
c=b;
b=b->tl;}}
# 371
c->tl=a==0?b: a;
return d;}
# 376
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
if(x==0)return x;{
struct Cyc_List_List*temp=x->tl;
if(temp==0)return x;{
# 382
struct Cyc_List_List*a=x;struct Cyc_List_List*aptr=a;
struct Cyc_List_List*b=temp;struct Cyc_List_List*bptr=b;
x=b->tl;
while(x!=0){
aptr->tl=x;
aptr=x;
x=x->tl;
if(x!=0){
bptr->tl=x;
bptr=x;
x=x->tl;}}
# 395
aptr->tl=0;
bptr->tl=0;
return({int(*_Tmp0)(void*,void*)=less_eq;struct Cyc_List_List*_Tmp1=
Cyc_List_rimp_merge_sort(less_eq,a);
# 397
Cyc_List_imp_merge(_Tmp0,_Tmp1,
Cyc_List_rimp_merge_sort(less_eq,b));});}}}
# 405
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*r,int(*less_eq)(void*,void*),struct Cyc_List_List*x){
return({int(*_Tmp0)(void*,void*)=less_eq;Cyc_List_rimp_merge_sort(_Tmp0,Cyc_List_rcopy(r,x));});}
# 410
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*r,int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
# 412
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 415
if(a==0)return Cyc_List_rcopy(r,b);
if(b==0)return Cyc_List_rcopy(r,a);
# 422
if(less_eq(a->hd,b->hd)<= 0){
c=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=a->hd,_Tmp0->tl=0;_Tmp0;});
a=a->tl;}else{
# 426
c=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=b->hd,_Tmp0->tl=0;_Tmp0;});
b=b->tl;}
# 429
d=c;
# 431
while(a!=0 && b!=0){
# 433
struct Cyc_List_List*temp;
if(less_eq(a->hd,b->hd)<= 0){
temp=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=a->hd,_Tmp0->tl=0;_Tmp0;});
c->tl=temp;
c=temp;
a=a->tl;}else{
# 440
temp=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=b->hd,_Tmp0->tl=0;_Tmp0;});
c->tl=temp;
c=temp;
b=b->tl;}}
# 447
({struct Cyc_List_List*_Tmp0=a==0?Cyc_List_rcopy(r,b): Cyc_List_rcopy(r,a);c->tl=_Tmp0;});
return d;}
# 451
struct Cyc_List_List*Cyc_List_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
return Cyc_List_rmerge_sort(Cyc_Core_heap_region,less_eq,x);}
# 455
struct Cyc_List_List*Cyc_List_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
return Cyc_List_rmerge(Cyc_Core_heap_region,less_eq,a,b);}char Cyc_List_Nth[4U]="Nth";
# 461
struct Cyc_List_Nth_exn_struct Cyc_List_Nth_val={Cyc_List_Nth};
# 466
void*Cyc_List_nth(struct Cyc_List_List*x,int i){
return _check_null(Cyc_List_nth_tail(x,i))->hd;}
# 472
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i){
if(i < 0)_throw(& Cyc_List_Nth_val);
for(1;i > 0;(-- i,x=x->tl)){
if(x==0)
_throw(& Cyc_List_Nth_val);}
return x;}
# 482
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
if(!pred(x->hd))
return 0;}
return 1;}
# 488
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
if(!pred(env,x->hd))
return 0;}
return 1;}
# 497
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
if(pred(x->hd))
return 1;}
return 0;}
# 503
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
if(pred(env,x->hd))
return 1;}
return 0;}
# 510
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x!=0;x=x->tl){
void*v=pred(env,x->hd);
if((unsigned)v)return v;}
# 515
return 0;}struct _tuple2{void*f0;void*f1;};
# 520
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 522
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 524
if(x==0 && y==0)return 0;
if(x==0 || y==0)_throw(& Cyc_List_List_mismatch_val);
# 527
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r3,0U,sizeof(struct Cyc_List_List));({struct _tuple2*_Tmp1=({struct _tuple2*_Tmp2=_region_malloc(r4,0U,sizeof(struct _tuple2));_Tmp2->f0=x->hd,_Tmp2->f1=y->hd;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
prev=result;
# 530
x=x->tl;
y=y->tl;
# 533
while(x!=0 && y!=0){
temp=({struct Cyc_List_List*_Tmp0=_region_malloc(r3,0U,sizeof(struct Cyc_List_List));({struct _tuple2*_Tmp1=({struct _tuple2*_Tmp2=_region_malloc(r4,0U,sizeof(struct _tuple2));_Tmp2->f0=x->hd,_Tmp2->f1=y->hd;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;}
# 540
if(x!=0 || y!=0)_throw(& Cyc_List_List_mismatch_val);
return result;}
# 544
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rzip(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y);}struct _tuple3{void*f0;void*f1;void*f2;};
# 548
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 550
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 552
if((x==0 && y==0)&& z==0)return 0;
if((x==0 || y==0)|| z==0)_throw(& Cyc_List_List_mismatch_val);
# 555
result=({struct Cyc_List_List*_Tmp0=_region_malloc(r3,0U,sizeof(struct Cyc_List_List));({struct _tuple3*_Tmp1=({struct _tuple3*_Tmp2=_region_malloc(r4,0U,sizeof(struct _tuple3));_Tmp2->f0=x->hd,_Tmp2->f1=y->hd,_Tmp2->f2=z->hd;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
prev=result;
# 558
x=x->tl;
y=y->tl;
z=z->tl;
# 562
while((x!=0 && y!=0)&& z!=0){
temp=({struct Cyc_List_List*_Tmp0=_region_malloc(r3,0U,sizeof(struct Cyc_List_List));({struct _tuple3*_Tmp1=({struct _tuple3*_Tmp2=_region_malloc(r4,0U,sizeof(struct _tuple3));_Tmp2->f0=x->hd,_Tmp2->f1=y->hd,_Tmp2->f2=z->hd;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=0;_Tmp0;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;
z=z->tl;}
# 570
if((x!=0 || y!=0)|| z!=0)_throw(& Cyc_List_List_mismatch_val);
return result;}
# 574
struct Cyc_List_List*Cyc_List_zip3(struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
return Cyc_List_rzip3(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y,z);}
# 580
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x){
# 582
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
# 585
if(x==0)return({struct _tuple0 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=0;_Tmp0;});
# 587
prev1=(result1=({struct Cyc_List_List*_Tmp0=_region_malloc(r3,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple2*)x->hd)[0].f0,_Tmp0->tl=0;_Tmp0;}));
prev2=(result2=({struct Cyc_List_List*_Tmp0=_region_malloc(r4,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple2*)x->hd)[0].f1,_Tmp0->tl=0;_Tmp0;}));
# 590
for(x=x->tl;x!=0;x=x->tl){
temp1=({struct Cyc_List_List*_Tmp0=_region_malloc(r3,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple2*)x->hd)[0].f0,_Tmp0->tl=0;_Tmp0;});
temp2=({struct Cyc_List_List*_Tmp0=_region_malloc(r4,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple2*)x->hd)[0].f1,_Tmp0->tl=0;_Tmp0;});
prev1->tl=temp1;
prev2->tl=temp2;
prev1=temp1;
prev2=temp2;}
# 598
return({struct _tuple0 _Tmp0;_Tmp0.f0=result1,_Tmp0.f1=result2;_Tmp0;});}
# 601
struct _tuple0 Cyc_List_split(struct Cyc_List_List*x){
return Cyc_List_rsplit(Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 607
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct _RegionHandle*r5,struct Cyc_List_List*x){
# 610
struct Cyc_List_List*result1;struct Cyc_List_List*prev1;struct Cyc_List_List*temp1;
struct Cyc_List_List*result2;struct Cyc_List_List*prev2;struct Cyc_List_List*temp2;
struct Cyc_List_List*result3;struct Cyc_List_List*prev3;struct Cyc_List_List*temp3;
# 614
if(x==0)return({struct _tuple1 _Tmp0;_Tmp0.f0=0,_Tmp0.f1=0,_Tmp0.f2=0;_Tmp0;});
# 616
prev1=(result1=({struct Cyc_List_List*_Tmp0=_region_malloc(r3,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple3*)x->hd)[0].f0,_Tmp0->tl=0;_Tmp0;}));
prev2=(result2=({struct Cyc_List_List*_Tmp0=_region_malloc(r4,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple3*)x->hd)[0].f1,_Tmp0->tl=0;_Tmp0;}));
prev3=(result3=({struct Cyc_List_List*_Tmp0=_region_malloc(r5,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple3*)x->hd)[0].f2,_Tmp0->tl=0;_Tmp0;}));
# 620
for(x=x->tl;x!=0;x=x->tl){
temp1=({struct Cyc_List_List*_Tmp0=_region_malloc(r3,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple3*)x->hd)[0].f0,_Tmp0->tl=0;_Tmp0;});
temp2=({struct Cyc_List_List*_Tmp0=_region_malloc(r4,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple3*)x->hd)[0].f1,_Tmp0->tl=0;_Tmp0;});
temp3=({struct Cyc_List_List*_Tmp0=_region_malloc(r5,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=((struct _tuple3*)x->hd)[0].f2,_Tmp0->tl=0;_Tmp0;});
prev1->tl=temp1;
prev2->tl=temp2;
prev3->tl=temp3;
prev1=temp1;
prev2=temp2;
prev3=temp3;}
# 631
return({struct _tuple1 _Tmp0;_Tmp0.f0=result1,_Tmp0.f1=result2,_Tmp0.f2=result3;_Tmp0;});}
# 634
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x){
return Cyc_List_rsplit3(Cyc_Core_heap_region,Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 640
int Cyc_List_memq(struct Cyc_List_List*l,void*x){
for(1;l!=0;l=l->tl){
if(l->hd==x)
return 1;}
return 0;}
# 647
int Cyc_List_mem(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
for(1;l!=0;l=l->tl){
if(cmp(l->hd,x)==0)
return 1;}
return 0;}
# 657
void*Cyc_List_assoc(struct Cyc_List_List*l,void*x){
for(1;l!=0;l=l->tl){
if(((struct _tuple2*)l->hd)[0].f0==x)
return((struct _tuple2*)l->hd)[0].f1;}
_throw(& Cyc_Core_Not_found_val);}
# 664
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
for(1;l!=0;l=l->tl){
if(cmp((*((struct _tuple2*)l->hd)).f0,x)==0)
return(*((struct _tuple2*)l->hd)).f1;}
_throw(& Cyc_Core_Not_found_val);}
# 675
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
struct Cyc_List_List*prev=0;
struct Cyc_List_List*iter=l;
while(iter!=0){
if(cmp(iter->hd,x)==0){
if(prev==0)
return _check_null(l)->tl;
prev->tl=iter->tl;
return l;}
# 685
prev=iter;
iter=iter->tl;}
# 688
_throw(& Cyc_Core_Not_found_val);}
# 691
static int Cyc_List_ptrequal(void*x,void*y){
return !(x==y);}
# 698
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x){
return Cyc_List_delete_cmp(Cyc_List_ptrequal,l,x);}
# 704
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x){
for(1;l!=0;l=l->tl){
if(((struct _tuple2*)l->hd)[0].f0==x)
return 1;}
return 0;}
# 713
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
for(1;l!=0;l=l->tl){
if(cmp(((struct _tuple2*)l->hd)[0].f0,x)==0)
return 1;}
return 0;}
# 722
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x){
while(x!=0){
void*hd=x->hd;
x=x->tl;
if(x!=0 && cmp(hd,x->hd)==0)
return({struct Cyc_Core_Opt*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp0->v=hd;_Tmp0;});}
# 729
return 0;}
# 733
struct _fat_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*x){
int s=Cyc_List_length(x);
return({unsigned _Tmp0=(unsigned)s;_tag_fat(({void**_Tmp1=({struct _RegionHandle*_Tmp2=r2;_region_malloc(_Tmp2,0U,_check_times(_Tmp0,sizeof(void*)));});({{unsigned _Tmp2=(unsigned)s;unsigned i;for(i=0;i < _Tmp2;++ i){({void*_Tmp3=({void*v=_check_null(x)->hd;x=x->tl;v;});_Tmp1[i]=_Tmp3;});}}0;});_Tmp1;}),sizeof(void*),_Tmp0);});}
# 738
struct _fat_ptr Cyc_List_to_array(struct Cyc_List_List*x){
return Cyc_List_rto_array(Cyc_Core_heap_region,x);}
# 743
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _fat_ptr arr){
struct Cyc_List_List*ans=0;
{int i=(int)(_get_fat_size(arr,sizeof(void*))- 1U);for(0;i >= 0;-- i){
ans=({struct Cyc_List_List*_Tmp0=_region_malloc(r2,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=*((void**)_check_fat_subscript(arr,sizeof(void*),i)),_Tmp0->tl=ans;_Tmp0;});}}
return ans;}
# 750
struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr arr){
return Cyc_List_rfrom_array(Cyc_Core_heap_region,arr);}
# 754
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int)){
struct Cyc_List_List*res=0;
{int i=0;for(0;i < n;++ i){
res=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=f(i);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=res;_Tmp0;});}}
return Cyc_List_imp_rev(res);}
# 761
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int)){
return Cyc_List_rtabulate(Cyc_Core_heap_region,n,f);}
# 765
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env){
struct Cyc_List_List*res=0;
{int i=0;for(0;i < n;++ i){
res=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));({void*_Tmp1=f(env,i);_Tmp0->hd=_Tmp1;}),_Tmp0->tl=res;_Tmp0;});}}
return Cyc_List_imp_rev(res);}
# 772
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env){
return Cyc_List_rtabulate_c(Cyc_Core_heap_region,n,f,env);}
# 776
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
for(1;l1!=0 && l2!=0;(l1=l1->tl,l2=l2->tl)){
# 779
if((unsigned)l1==(unsigned)l2)
return 0;{
int i=cmp(l1->hd,l2->hd);
if(i!=0)
return i;}}
# 785
if(l1!=0)
return 1;
if(l2!=0)
return -1;
return 0;}
# 793
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
for(1;l1!=0 && l2!=0;(l1=l1->tl,l2=l2->tl)){
# 796
if((unsigned)l1==(unsigned)l2)
return 1;
if(cmp(l1->hd,l2->hd)!=0)
return 0;}
# 801
return l1==0;}
# 804
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
if(l==0)
return 0;{
# 808
struct Cyc_List_List*result;result=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),result->hd=l->hd,result->tl=0;{
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
for(1;l!=0;l=l->tl){
if(f(env,l->hd)){
temp=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=l->hd,_Tmp0->tl=0;_Tmp0;});
end->tl=temp;
end=temp;}}
# 817
return result->tl;}}}
# 820
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
return Cyc_List_rfilter_c(Cyc_Core_heap_region,f,env,l);}
# 824
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*l){
if(l==0)
return 0;{
# 828
struct Cyc_List_List*result;result=_region_malloc(r,0U,sizeof(struct Cyc_List_List)),result->hd=l->hd,result->tl=0;{
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
for(1;l!=0;l=l->tl){
if(f(l->hd)){
temp=({struct Cyc_List_List*_Tmp0=_region_malloc(r,0U,sizeof(struct Cyc_List_List));_Tmp0->hd=l->hd,_Tmp0->tl=0;_Tmp0;});
end->tl=temp;
end=temp;}}
# 837
return result->tl;}}}
# 840
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*l){
return Cyc_List_rfilter(Cyc_Core_heap_region,f,l);}
