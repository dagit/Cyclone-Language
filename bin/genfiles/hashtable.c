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
struct _RegionHandle {
  struct _RuntimeStack s;
  struct _RegionPage *curr;
#if(defined(__linux__) && defined(__KERNEL__))
  struct _RegionPage *vpage;
#endif 
  char               *offset;
  char               *last_plus_one;
  struct _DynRegionHandle *sub_regions;
  struct _pool *released_ptrs;
#ifdef CYC_REGION_PROFILE
  const char         *name;
#else
  unsigned used_bytes;
  unsigned wasted_bytes;
#endif
};
struct _DynRegionFrame {
  struct _RuntimeStack s;
  struct _DynRegionHandle *x;
};
// A dynamic region is just a region handle.  The wrapper struct is for type
// abstraction.
struct Cyc_Core_DynamicRegion {
  struct _RegionHandle h;
};

struct _RegionHandle _new_region(const char*);
void* _region_malloc(struct _RegionHandle*, unsigned);
void* _region_calloc(struct _RegionHandle*, unsigned t, unsigned n);
void* _region_vmalloc(struct _RegionHandle*, unsigned);
void _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void _pop_dynregion();

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

#define _CYC_MAX_REGION_CONST 2
#define _CYC_MIN_ALIGNMENT (sizeof(double))

#ifdef CYC_REGION_PROFILE
extern int rgn_total_bytes;
#endif

static inline void*_fast_region_malloc(struct _RegionHandle*r, unsigned orig_s) {  
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
  return _region_malloc(r,orig_s); 
}

#ifdef CYC_REGION_PROFILE
/* see macros below for usage. defined in runtime_memory.c */
void* _profile_GC_malloc(int,const char*,const char*,int);
void* _profile_GC_malloc_atomic(int,const char*,const char*,int);
void* _profile_GC_calloc(unsigned,unsigned,const char*,const char*,int);
void* _profile_GC_calloc_atomic(unsigned,unsigned,const char*,const char*,int);
void* _profile_region_malloc(struct _RegionHandle*,unsigned,const char*,const char*,int);
void* _profile_region_calloc(struct _RegionHandle*,unsigned,unsigned,const char *,const char*,int);
struct _RegionHandle _profile_new_region(const char*,const char*,const char*,int);
void _profile_free_region(struct _RegionHandle*,const char*,const char*,int);
#ifndef RUNTIME_CYC
#define _new_region(n) _profile_new_region(n,__FILE__,__FUNCTION__,__LINE__)
#define _free_region(r) _profile_free_region(r,__FILE__,__FUNCTION__,__LINE__)
#define _region_malloc(rh,n) _profile_region_malloc(rh,n,__FILE__,__FUNCTION__,__LINE__)
#define _region_calloc(rh,n,t) _profile_region_calloc(rh,n,t,__FILE__,__FUNCTION__,__LINE__)
#  endif
#define _cycalloc(n) _profile_GC_malloc(n,__FILE__,__FUNCTION__,__LINE__)
#define _cycalloc_atomic(n) _profile_GC_malloc_atomic(n,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc(n,s) _profile_GC_calloc(n,s,__FILE__,__FUNCTION__,__LINE__)
#define _cyccalloc_atomic(n,s) _profile_GC_calloc_atomic(n,s,__FILE__,__FUNCTION__,__LINE__)
#endif
#endif

# 168 "core.h"
 extern struct _RegionHandle*Cyc_Core_heap_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);
# 336
extern void*Cyc_List_assoc_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);struct Cyc_Hashtable_Table;
# 72 "hashtable.h"
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*);struct Cyc_Int_pa_PrintArg_struct{int tag;unsigned long f1;};
# 157 "cycboot.h"
extern int Cyc_printf(struct _fat_ptr,struct _fat_ptr);struct Cyc_Hashtable_Table{struct _RegionHandle*r;int(*cmp)(void*,void*);int(*hash)(void*);int max_len;struct _fat_ptr tab;};
# 38 "hashtable.cyc"
struct Cyc_Hashtable_Table*Cyc_Hashtable_rcreate(struct _RegionHandle*r,int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
# 40
return({struct Cyc_Hashtable_Table*_Tmp0=_region_malloc(r,sizeof(struct Cyc_Hashtable_Table));_Tmp0->r=r,_Tmp0->cmp=cmp,_Tmp0->hash=hash,_Tmp0->max_len=3,_Tmp0->tab=({unsigned _Tmp1=sz;_tag_fat(_region_calloc(r,sizeof(struct Cyc_List_List*),_Tmp1),sizeof(struct Cyc_List_List*),_Tmp1);});_Tmp0;});}
# 43
struct Cyc_Hashtable_Table*Cyc_Hashtable_create(int sz,int(*cmp)(void*,void*),int(*hash)(void*)){
return Cyc_Hashtable_rcreate(Cyc_Core_heap_region,sz,cmp,hash);}struct _tuple0{void*f1;void*f2;};
# 47
void Cyc_Hashtable_insert(struct Cyc_Hashtable_Table*t,void*key,void*val){
struct _fat_ptr tab=t->tab;
int bucket=(int)({unsigned _Tmp0=(unsigned)t->hash(key);_Tmp0 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_region_malloc(t->r,sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_region_malloc(t->r,sizeof(struct _tuple0));_Tmp3->f1=key,_Tmp3->f2=val;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=((struct Cyc_List_List**)tab.curr)[bucket];_Tmp1;});((struct Cyc_List_List**)tab.curr)[bucket]=_Tmp0;});
if(({int _Tmp0=Cyc_List_length(((struct Cyc_List_List**)tab.curr)[bucket]);_Tmp0 > t->max_len;}))
Cyc_Hashtable_resize(t);}
# 55
void*Cyc_Hashtable_lookup(struct Cyc_Hashtable_Table*t,void*key){
struct _fat_ptr tab=t->tab;
struct Cyc_List_List*l=*((struct Cyc_List_List**)({typeof(tab )_Tmp0=tab;_check_fat_subscript(_Tmp0,sizeof(struct Cyc_List_List*),(int)({unsigned _Tmp1=(unsigned)t->hash(key);_Tmp1 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));}));}));
return Cyc_List_assoc_cmp(t->cmp,l,key);}
# 61
void**Cyc_Hashtable_lookup_opt(struct Cyc_Hashtable_Table*t,void*key){
struct _fat_ptr tab=t->tab;
struct Cyc_List_List*l=*((struct Cyc_List_List**)({typeof(tab )_Tmp0=tab;_check_fat_subscript(_Tmp0,sizeof(struct Cyc_List_List*),(int)({unsigned _Tmp1=(unsigned)t->hash(key);_Tmp1 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));}));}));
int(*cmp)(void*,void*)=t->cmp;
for(1;l!=0;l=l->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)l->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=(void**)& _Tmp0->f2;{void*k=_Tmp2;void**v=(void**)_Tmp1;
if(cmp(key,k)==0)return v;}}
# 69
return 0;}
# 76
void**Cyc_Hashtable_lookup_other_opt(struct Cyc_Hashtable_Table*t,void*key,int(*cmp)(void*,void*),int(*hash)(void*)){
# 78
struct _fat_ptr tab=t->tab;
struct Cyc_List_List*l=*((struct Cyc_List_List**)({typeof(tab )_Tmp0=tab;_check_fat_subscript(_Tmp0,sizeof(struct Cyc_List_List*),(int)({unsigned _Tmp1=(unsigned)hash(key);_Tmp1 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));}));}));
for(1;l!=0;l=l->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)l->hd;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=(void**)& _Tmp0->f2;{void*k=_Tmp2;void**v=(void**)_Tmp1;
if(cmp(key,k)==0)return v;}}
# 84
return 0;}
# 87
int Cyc_Hashtable_try_lookup(struct Cyc_Hashtable_Table*t,void*key,void**data){
struct _fat_ptr tab=t->tab;
struct Cyc_List_List*l=*((struct Cyc_List_List**)({typeof(tab )_Tmp0=tab;_check_fat_subscript(_Tmp0,sizeof(struct Cyc_List_List*),(int)({unsigned _Tmp1=(unsigned)t->hash(key);_Tmp1 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));}));}));
int(*cmp)(void*,void*)=t->cmp;
for(1;l!=0;l=l->tl){
struct _tuple0 _Tmp0=*((struct _tuple0*)l->hd);void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f1;_Tmp1=_Tmp0.f2;{void*k=_Tmp2;void*v=_Tmp1;
if(cmp(key,k)==0){
*data=v;
return 1;}}}
# 98
return 0;}
# 101
void Cyc_Hashtable_remove(struct Cyc_Hashtable_Table*t,void*key){
# 104
struct _fat_ptr tab=t->tab;
int(*cmp)(void*,void*)=t->cmp;
int bucket=(int)({unsigned _Tmp0=(unsigned)t->hash(key);_Tmp0 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
struct Cyc_List_List*l=((struct Cyc_List_List**)tab.curr)[bucket];
if(l==0)return;
if(cmp(key,((struct _tuple0*)l->hd)[0].f1)==0){
((struct Cyc_List_List**)tab.curr)[bucket]=l->tl;
return;}{
# 113
struct Cyc_List_List*next=l->tl;for(0;_check_null(l)->tl!=0;(l=l->tl,next=next->tl)){
# 115
if(cmp(key,((struct _tuple0*)_check_null(next)->hd)[0].f1)==0){
l->tl=next->tl;
return;}}}}
# 121
int Cyc_Hashtable_hash_string(struct _fat_ptr s){
# 123
int ans=0;
int sz=(int)_get_fat_size(s,sizeof(char));
int shift=0;
{int i=0;for(0;i < sz;++ i){
ans=ans ^ (int)*((const char*)_check_fat_subscript(s,sizeof(char),i))<< shift;
shift +=8;
if(shift==32)
shift=0;}}
# 132
return ans;}
# 135
int Cyc_Hashtable_hash_stringptr(struct _fat_ptr*s){
return Cyc_Hashtable_hash_string(*s);}
# 140
void Cyc_Hashtable_insert_bucket(struct _RegionHandle*r,struct _fat_ptr tab,int(*hash)(void*),struct Cyc_List_List*elems){
# 143
if(elems==0)return;
Cyc_Hashtable_insert_bucket(r,tab,hash,elems->tl);{
void*key=((struct _tuple0*)elems->hd)[0].f1;
void*val=((struct _tuple0*)elems->hd)[0].f2;
int nidx=(int)({unsigned _Tmp0=(unsigned)hash(key);_Tmp0 % _get_fat_size(tab,sizeof(struct Cyc_List_List*));});
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_region_malloc(r,sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp2=({struct _tuple0*_Tmp3=_region_malloc(r,sizeof(struct _tuple0));_Tmp3->f1=key,_Tmp3->f2=val;_Tmp3;});_Tmp1->hd=_Tmp2;}),_Tmp1->tl=((struct Cyc_List_List**)tab.curr)[nidx];_Tmp1;});((struct Cyc_List_List**)tab.curr)[nidx]=_Tmp0;});}}
# 151
void Cyc_Hashtable_resize(struct Cyc_Hashtable_Table*t){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int nsize=2 * osize + 1;
struct _fat_ptr ndata=({unsigned _Tmp0=nsize;_tag_fat(_region_calloc(t->r,sizeof(struct Cyc_List_List*),_Tmp0),sizeof(struct Cyc_List_List*),_Tmp0);});
{int i=0;for(0;i < osize;++ i){
Cyc_Hashtable_insert_bucket(t->r,ndata,t->hash,*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i)));}}
t->tab=ndata;
t->max_len=2 * t->max_len;}
# 163
void Cyc_Hashtable_iter(void(*f)(void*,void*),struct Cyc_Hashtable_Table*t){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter!=0;iter=iter->tl){
f(((struct _tuple0*)iter->hd)[0].f1,((struct _tuple0*)iter->hd)[0].f2);}}}
# 171
void Cyc_Hashtable_iter_c(void(*f)(void*,void*,void*),struct Cyc_Hashtable_Table*t,void*env){
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter!=0;iter=iter->tl){
f(((struct _tuple0*)iter->hd)[0].f1,((struct _tuple0*)iter->hd)[0].f2,env);}}}
# 180
void Cyc_Hashtable_print_table_map(struct Cyc_Hashtable_Table*t,void(*prn_key)(void*),void(*prn_val)(void*)){
# 182
struct _fat_ptr odata=t->tab;
int osize=(int)_get_fat_size(odata,sizeof(struct Cyc_List_List*));
int i=0;for(0;i < osize;++ i){
({struct Cyc_Int_pa_PrintArg_struct _Tmp0=({struct Cyc_Int_pa_PrintArg_struct _Tmp1;_Tmp1.tag=1,_Tmp1.f1=(unsigned long)i;_Tmp1;});void*_Tmp1[1];_Tmp1[0]=& _Tmp0;Cyc_printf(({const char*_Tmp2="%d: ";_tag_fat(_Tmp2,sizeof(char),5U);}),_tag_fat(_Tmp1,sizeof(void*),1));});
{struct Cyc_List_List*iter=*((struct Cyc_List_List**)_check_fat_subscript(odata,sizeof(struct Cyc_List_List*),i));for(0;iter!=0;iter=iter->tl){
Cyc_printf(({const char*_Tmp0="(";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
prn_key(((struct _tuple0*)iter->hd)[0].f1);
Cyc_printf(({const char*_Tmp0=",";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));
prn_val(((struct _tuple0*)iter->hd)[0].f2);
Cyc_printf(({const char*_Tmp0=") ";_tag_fat(_Tmp0,sizeof(char),3U);}),_tag_fat(0U,sizeof(void*),0));}}
# 193
Cyc_printf(({const char*_Tmp0="\n";_tag_fat(_Tmp0,sizeof(char),2U);}),_tag_fat(0U,sizeof(void*),0));}}
