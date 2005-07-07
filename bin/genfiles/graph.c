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

# 121 "core.h"
 int Cyc_Core_intcmp(int,int);struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);struct Cyc___cycFILE;
# 100 "cycboot.h"
extern int Cyc_fprintf(struct Cyc___cycFILE*,struct _fat_ptr,struct _fat_ptr);struct Cyc_Set_Set;
# 51 "set.h"
extern struct Cyc_Set_Set*Cyc_Set_empty(int(*)(void*,void*));
# 63
extern struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*,void*);
# 75
extern struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 82
extern struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*,struct Cyc_Set_Set*);
# 85
extern struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*,void*);
# 97
extern int Cyc_Set_is_empty(struct Cyc_Set_Set*);
# 100
extern int Cyc_Set_member(struct Cyc_Set_Set*,void*);
# 137
extern void*Cyc_Set_choose(struct Cyc_Set_Set*);struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};
# 62 "dict.h"
extern struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 83
extern int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
extern struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 110
extern void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 122 "dict.h"
extern void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict,void*);
# 233
extern struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict);
# 270
extern struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict,void*);
# 27 "graph.cyc"
struct Cyc_Dict_Dict Cyc_Graph_empty(int(*cmp)(void*,void*)){
return Cyc_Dict_empty(cmp);}
# 34
struct Cyc_Set_Set*Cyc_Graph_get_targets(struct Cyc_Dict_Dict g,void*source){
struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**(*_Tmp1)(struct Cyc_Dict_Dict,void*)=(struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup_opt;_Tmp1;})(g,source);void*_Tmp1;if(_Tmp0==0)
return Cyc_Set_empty(g.rel);else{_Tmp1=*_Tmp0;{struct Cyc_Set_Set*targets=_Tmp1;
return targets;}};}
# 45
struct Cyc_Dict_Dict Cyc_Graph_add_node(struct Cyc_Dict_Dict g,void*source){
if(Cyc_Dict_member(g,source))return g;else{
struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert;_Tmp1;});struct Cyc_Dict_Dict _Tmp1=g;void*_Tmp2=source;return _Tmp0(_Tmp1,_Tmp2,Cyc_Set_empty(g.rel));}}
# 53
struct Cyc_Dict_Dict Cyc_Graph_add_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*sourceTargets=Cyc_Graph_get_targets(g,source);
struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert;_Tmp1;});struct Cyc_Dict_Dict _Tmp1=g;void*_Tmp2=source;return _Tmp0(_Tmp1,_Tmp2,Cyc_Set_insert(sourceTargets,target));}
# 61
struct Cyc_Dict_Dict Cyc_Graph_add_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*sourceTargets=Cyc_Graph_get_targets(g,source);
struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert;_Tmp1;});struct Cyc_Dict_Dict _Tmp1=g;void*_Tmp2=source;return _Tmp0(_Tmp1,_Tmp2,Cyc_Set_union_two(sourceTargets,targets));}
# 69
struct Cyc_Dict_Dict Cyc_Graph_remove_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*sourceTargets=Cyc_Graph_get_targets(g,source);
struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert;_Tmp1;});struct Cyc_Dict_Dict _Tmp1=g;void*_Tmp2=source;return _Tmp0(_Tmp1,_Tmp2,Cyc_Set_delete(sourceTargets,target));}
# 77
struct Cyc_Dict_Dict Cyc_Graph_remove_edges(struct Cyc_Dict_Dict g,void*source,struct Cyc_Set_Set*targets){
struct Cyc_Set_Set*sourceTargets=Cyc_Graph_get_targets(g,source);
struct Cyc_Dict_Dict(*_Tmp0)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert;_Tmp1;});struct Cyc_Dict_Dict _Tmp1=g;void*_Tmp2=source;return _Tmp0(_Tmp1,_Tmp2,Cyc_Set_diff(sourceTargets,targets));}
# 86
static struct Cyc_List_List*Cyc_Graph_to_list(struct Cyc_Set_Set*ts){
# 88
struct Cyc_List_List*result=0;
while(!Cyc_Set_is_empty(ts)){
{void*z=Cyc_Set_choose(ts);
ts=Cyc_Set_delete(ts,z);
result=({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));_Tmp0->hd=z,_Tmp0->tl=result;_Tmp0;});}
# 90
1U;}
# 94
return result;}
# 100
int Cyc_Graph_is_edge(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**(*_Tmp1)(struct Cyc_Dict_Dict,void*)=(struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup_opt;_Tmp1;})(g,source);void*_Tmp1;if(_Tmp0==0)
# 103
return 0;else{_Tmp1=*_Tmp0;{struct Cyc_Set_Set*sourceTargets=_Tmp1;
# 105
return Cyc_Set_member(sourceTargets,target);}};}struct _tuple0{void*f0;struct Cyc_Set_Set*f1;};
# 112
void Cyc_Graph_print(struct Cyc___cycFILE*f,struct Cyc_Dict_Dict g,void(*nodeprint)(struct Cyc___cycFILE*,void*)){
Cyc_fprintf(f,_tag_fat("digraph {\n",sizeof(char),11U),_tag_fat(0U,sizeof(void*),0));{
struct Cyc_List_List*edges=Cyc_Dict_to_list(g);
for(1;(unsigned)edges;edges=edges->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)edges->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(void*)_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*s=_Tmp2;struct Cyc_Set_Set*ts=_Tmp1;
struct Cyc_List_List*tslist=Cyc_Graph_to_list(ts);
if((unsigned)tslist)
for(1;(unsigned)tslist;tslist=tslist->tl){
nodeprint(f,s);
Cyc_fprintf(f,_tag_fat(" -> ",sizeof(char),5U),_tag_fat(0U,sizeof(void*),0));
nodeprint(f,tslist->hd);
Cyc_fprintf(f,_tag_fat(";\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));}else{
# 126
nodeprint(f,s);
Cyc_fprintf(f,_tag_fat("; // no targets\n",sizeof(char),17U),_tag_fat(0U,sizeof(void*),0));}}}
# 130
Cyc_fprintf(f,_tag_fat("}\n",sizeof(char),3U),_tag_fat(0U,sizeof(void*),0));}}
# 140 "graph.cyc"
static struct Cyc_List_List*Cyc_Graph_sourcesOf(struct Cyc_Dict_Dict g,void*node){
struct Cyc_List_List*result=0;
# 143
{struct Cyc_List_List*edges=Cyc_Dict_to_list(g);for(0;(unsigned)edges;edges=edges->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)edges->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(void*)_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*s=_Tmp2;struct Cyc_Set_Set*ts=_Tmp1;
if(Cyc_Set_member(ts,node))result=({struct Cyc_List_List*_Tmp3=_cycalloc(sizeof(struct Cyc_List_List));_Tmp3->hd=s,_Tmp3->tl=result;_Tmp3;});}}}
# 147
return result;}struct _tuple1{struct Cyc_Set_Set*f0;struct Cyc_Dict_Dict f1;};
# 156
static struct _tuple1 Cyc_Graph_divideGraph(struct Cyc_Dict_Dict g,void*source){
# 159
struct Cyc_Set_Set**_Tmp0=({struct Cyc_Set_Set**(*_Tmp1)(struct Cyc_Dict_Dict,void*)=(struct Cyc_Set_Set**(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup_opt;_Tmp1;})(g,source);void*_Tmp1;if(_Tmp0==0){
struct _tuple1 _Tmp2;({struct Cyc_Set_Set*_Tmp3=Cyc_Set_empty(g.rel);_Tmp2.f0=_Tmp3;}),_Tmp2.f1=g;return _Tmp2;}else{_Tmp1=*_Tmp0;{struct Cyc_Set_Set*sourceTargets=_Tmp1;
struct _tuple1 _Tmp2;_Tmp2.f0=sourceTargets,({struct Cyc_Dict_Dict _Tmp3=Cyc_Dict_delete(g,source);_Tmp2.f1=_Tmp3;});return _Tmp2;}};}
# 168
static struct Cyc_Dict_Dict Cyc_Graph_add_edgeTc(struct Cyc_Dict_Dict g,void*source,void*target){
struct Cyc_Set_Set*targetTargets=Cyc_Graph_get_targets(g,target);
struct _tuple1 _Tmp0=Cyc_Graph_divideGraph(g,source);struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;_Tmp2=_Tmp0.f0;_Tmp1=_Tmp0.f1;{struct Cyc_Set_Set*sourceTargets=_Tmp2;struct Cyc_Dict_Dict graphWithoutSource=_Tmp1;
struct Cyc_List_List*sourceSources=Cyc_Graph_sourcesOf(graphWithoutSource,source);
struct Cyc_Set_Set*newSourceTargets=({struct Cyc_Set_Set*_Tmp3=Cyc_Set_union_two(sourceTargets,targetTargets);Cyc_Set_insert(_Tmp3,target);});
struct Cyc_Dict_Dict result=({struct Cyc_Dict_Dict(*_Tmp3)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Set_Set*))Cyc_Dict_insert;_Tmp3;})(graphWithoutSource,source,newSourceTargets);
{struct Cyc_List_List*s=sourceSources;for(0;(unsigned)s;s=s->tl){
result=Cyc_Graph_add_edges(result,s->hd,newSourceTargets);}}
# 177
return result;}}
# 179
struct Cyc_Dict_Dict Cyc_Graph_tc_slow(struct Cyc_Dict_Dict g){
struct Cyc_Dict_Dict result=Cyc_Graph_empty(g.rel);
struct Cyc_List_List*edges=Cyc_Dict_to_list(g);
for(1;(unsigned)edges;edges=edges->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)edges->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(void*)_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*source=_Tmp2;struct Cyc_Set_Set*targets=_Tmp1;
struct Cyc_List_List*tslist=Cyc_Graph_to_list(targets);
if((unsigned)tslist){
struct Cyc_List_List*t=tslist;for(0;(unsigned)t;t=t->tl){
result=Cyc_Graph_add_edgeTc(result,source,t->hd);}}else{
result=Cyc_Graph_add_node(result,source);}}}
# 190
return result;}
# 197
struct Cyc_Dict_Dict Cyc_Graph_tkernel(struct Cyc_Dict_Dict g){
int(*cmp)(void*,void*)=g.rel;
struct Cyc_Dict_Dict closure=Cyc_Graph_empty(cmp);
struct Cyc_Dict_Dict result=Cyc_Graph_empty(cmp);
{struct Cyc_List_List*edges=Cyc_Dict_to_list(g);for(0;(unsigned)edges;edges=edges->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)edges->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(void*)_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*source=_Tmp2;struct Cyc_Set_Set*targets=_Tmp1;
while(!Cyc_Set_is_empty(targets)){
{void*target=Cyc_Set_choose(targets);
targets=Cyc_Set_delete(targets,target);
if(Cyc_Graph_is_edge(closure,source,target))continue;
closure=Cyc_Graph_add_edgeTc(closure,source,target);
result=Cyc_Graph_add_edge(result,source,target);}
# 204
1U;}}}}
# 211
return result;}struct Cyc_Graph_nodestate{void*root;int C;int visitindex;};struct Cyc_Graph_componentstate{struct Cyc_Set_Set*Succ;struct Cyc_Set_Set*nodes;};struct Cyc_Graph_tcstate{struct Cyc_Set_Set*visited;int visitindex;struct Cyc_Dict_Dict ns;struct Cyc_Dict_Dict cs;int Cindex;struct Cyc_List_List*nstack;struct Cyc_List_List*cstack;};
# 237
static void Cyc_Graph_comp_tc(struct Cyc_Graph_tcstate*ts,struct Cyc_Dict_Dict g,void*v){
# 239
int(*cmp)(void*,void*)=g.rel;
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(ts->visited,v);ts->visited=_Tmp0;});{
struct Cyc_Graph_nodestate*nsv;nsv=_cycalloc(sizeof(struct Cyc_Graph_nodestate)),nsv->root=v,nsv->C=0,nsv->visitindex=ts->visitindex ++;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,struct Cyc_Graph_nodestate*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,struct Cyc_Graph_nodestate*))Cyc_Dict_insert;_Tmp1;})(ts->ns,v,nsv);ts->ns=_Tmp0;});
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=v,_Tmp1->tl=ts->nstack;_Tmp1;});ts->nstack=_Tmp0;});{
int hsaved=Cyc_List_length(ts->cstack);
struct Cyc_Set_Set*targets=Cyc_Graph_get_targets(g,v);
while(!Cyc_Set_is_empty(targets)){
{void*w=Cyc_Set_choose(targets);
targets=Cyc_Set_delete(targets,w);
if(cmp(v,w)==0)continue;{
int is_forward_edge;
struct Cyc_Graph_nodestate*nsw;
if(Cyc_Set_member(ts->visited,w)){
nsw=({struct Cyc_Graph_nodestate*(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(ts->ns,w);
is_forward_edge=nsw->visitindex < nsv->visitindex?0: 1;}else{
# 257
is_forward_edge=0;
Cyc_Graph_comp_tc(ts,g,w);
nsw=({struct Cyc_Graph_nodestate*(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(ts->ns,w);}{
# 261
int Cw=nsw->C;
if(Cw==0){
struct Cyc_Graph_nodestate*nsrootv=({struct Cyc_Graph_nodestate*(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(ts->ns,nsv->root);
struct Cyc_Graph_nodestate*nsrootw=({struct Cyc_Graph_nodestate*(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(ts->ns,nsw->root);
# 266
if(nsrootv->visitindex > nsrootw->visitindex)
nsv->root=nsw->root;}else{
# 270
if(!is_forward_edge)
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=(void*)Cw,_Tmp1->tl=ts->cstack;_Tmp1;});ts->cstack=_Tmp0;});}}}}
# 247
1U;}
# 273
if(cmp(nsv->root,v)!=0)
return;{
# 276
int Cnew=ts->Cindex ++;
struct Cyc_Graph_componentstate*csCnew;csCnew=_cycalloc(sizeof(struct Cyc_Graph_componentstate)),({struct Cyc_Set_Set*_Tmp0=({struct Cyc_Set_Set*(*_Tmp1)(int(*)(int,int))=(struct Cyc_Set_Set*(*)(int(*)(int,int)))Cyc_Set_empty;_Tmp1;})(Cyc_Core_intcmp);csCnew->Succ=_Tmp0;}),({
struct Cyc_Set_Set*_Tmp0=Cyc_Set_empty(cmp);csCnew->nodes=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,int,struct Cyc_Graph_componentstate*)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,int,struct Cyc_Graph_componentstate*))Cyc_Dict_insert;_Tmp1;})(ts->cs,Cnew,csCnew);ts->cs=_Tmp0;});
{struct Cyc_List_List*_Tmp0=ts->nstack;void*_Tmp1;if(_Tmp0==0)
goto _LL0;else{_Tmp1=(void*)_Tmp0->hd;{void*top=_Tmp1;
# 283
if(cmp(top,v)!=0 ||
 Cyc_Graph_is_edge(g,v,v))
({struct Cyc_Set_Set*_Tmp2=({struct Cyc_Set_Set*(*_Tmp3)(struct Cyc_Set_Set*,int)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,int))Cyc_Set_insert;_Tmp3;})(csCnew->Succ,Cnew);csCnew->Succ=_Tmp2;});
# 287
goto _LL0;}}_LL0:;}
# 289
{int hnow=Cyc_List_length(ts->cstack);for(0;hnow > hsaved;-- hnow){
int X=(int)ts->cstack->hd;
ts->cstack=ts->cstack->tl;
if(!({int(*_Tmp0)(struct Cyc_Set_Set*,int)=(int(*)(struct Cyc_Set_Set*,int))Cyc_Set_member;_Tmp0;})(csCnew->Succ,X)){
struct Cyc_Set_Set*s=({struct Cyc_Set_Set*(*_Tmp0)(struct Cyc_Set_Set*,int)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,int))Cyc_Set_insert;_Tmp0;})(csCnew->Succ,X);
s=({struct Cyc_Set_Set*_Tmp0=s;Cyc_Set_union_two(_Tmp0,({struct Cyc_Graph_componentstate*(*_Tmp1)(struct Cyc_Dict_Dict,int)=(struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict,int))Cyc_Dict_lookup;_Tmp1;})(ts->cs,X)->Succ);});
csCnew->Succ=s;}}}
# 298
while((unsigned)ts->nstack){
{void*w=ts->nstack->hd;
ts->nstack=ts->nstack->tl;{
struct Cyc_Graph_nodestate*nsw=({struct Cyc_Graph_nodestate*(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(struct Cyc_Graph_nodestate*(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(ts->ns,w);
nsw->C=Cnew;
({struct Cyc_Set_Set*_Tmp0=Cyc_Set_insert(csCnew->nodes,w);csCnew->nodes=_Tmp0;});
if(cmp(w,v)==0)break;}}
# 299
1U;}}}}}
# 307
struct Cyc_Dict_Dict Cyc_Graph_tc(struct Cyc_Dict_Dict g){
int(*cmp)(void*,void*)=g.rel;
struct Cyc_Graph_tcstate*ts;
ts=_cycalloc(sizeof(struct Cyc_Graph_tcstate)),({
struct Cyc_Set_Set*_Tmp0=Cyc_Set_empty(cmp);ts->visited=_Tmp0;}),ts->visitindex=1,({
# 313
struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_empty(cmp);ts->ns=_Tmp0;}),({
struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(int(*)(int,int))=(struct Cyc_Dict_Dict(*)(int(*)(int,int)))Cyc_Dict_empty;_Tmp1;})(Cyc_Core_intcmp);ts->cs=_Tmp0;}),ts->Cindex=1,ts->nstack=0,ts->cstack=0;
# 319
{struct Cyc_List_List*edges=Cyc_Dict_to_list(g);for(0;(unsigned)edges;edges=edges->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)edges->hd;void*_Tmp1;void*_Tmp2;_Tmp2=(void*)_Tmp0->f0;_Tmp1=_Tmp0->f1;{void*source=_Tmp2;struct Cyc_Set_Set*targets=_Tmp1;
if(!Cyc_Set_member(ts->visited,source))Cyc_Graph_comp_tc(ts,g,source);}}}{
# 324
struct Cyc_Dict_Dict result=Cyc_Graph_empty(cmp);
{int C=1;for(0;C < ts->Cindex;++ C){
struct Cyc_Graph_componentstate*cs=({struct Cyc_Graph_componentstate*(*_Tmp0)(struct Cyc_Dict_Dict,int)=(struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict,int))Cyc_Dict_lookup;_Tmp0;})(ts->cs,C);
struct Cyc_Set_Set*targets=Cyc_Set_empty(cmp);
struct Cyc_Set_Set*Succ=cs->Succ;
while(!Cyc_Set_is_empty(Succ)){
{int C2=({int(*_Tmp0)(struct Cyc_Set_Set*)=(int(*)(struct Cyc_Set_Set*))Cyc_Set_choose;_Tmp0;})(Succ);
Succ=({struct Cyc_Set_Set*(*_Tmp0)(struct Cyc_Set_Set*,int)=(struct Cyc_Set_Set*(*)(struct Cyc_Set_Set*,int))Cyc_Set_delete;_Tmp0;})(Succ,C2);
targets=({struct Cyc_Set_Set*_Tmp0=targets;Cyc_Set_union_two(_Tmp0,
({struct Cyc_Graph_componentstate*(*_Tmp1)(struct Cyc_Dict_Dict,int)=(struct Cyc_Graph_componentstate*(*)(struct Cyc_Dict_Dict,int))Cyc_Dict_lookup;_Tmp1;})(ts->cs,C2)->nodes);});}
# 330
1U;}{
# 335
struct Cyc_Set_Set*nodes=cs->nodes;
while(!Cyc_Set_is_empty(nodes)){
{void*v=Cyc_Set_choose(nodes);
nodes=Cyc_Set_delete(nodes,v);
result=Cyc_Graph_add_edges(result,v,targets);}
# 337
1U;}}}}
# 342
return result;}}
# 355 "graph.cyc"
static void Cyc_Graph_traverse(struct Cyc_Dict_Dict input,struct Cyc_Dict_Dict*output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned k){
# 359
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=*S;_Tmp1;});*S=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,unsigned)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert;_Tmp1;})(*N,x,k);*N=_Tmp0;});
{struct Cyc_Set_Set*targets=Cyc_Graph_get_targets(input,x);for(0;!Cyc_Set_is_empty(targets);1){
void*y=Cyc_Set_choose(targets);
targets=Cyc_Set_delete(targets,y);
if(({unsigned(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(*N,y)==0U)
Cyc_Graph_traverse(input,output,N,S,y,k + 1U);{
unsigned Ny=({unsigned(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(*N,y);
if(({unsigned _Tmp0=Ny;_Tmp0 < ({unsigned(*_Tmp1)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp1;})(*N,x);}))
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,unsigned)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert;_Tmp1;})(*N,x,Ny);*N=_Tmp0;});}}}
# 370
if(({unsigned _Tmp0=({unsigned(*_Tmp1)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp1;})(*N,x);_Tmp0==k;})){
int(*cmp)(void*,void*)=(*output).rel;
struct Cyc_Set_Set*component=Cyc_Set_empty(cmp);
{struct Cyc_List_List*s=*S;for(0;(unsigned)s;s=s->tl){
void*top=s->hd;
component=Cyc_Set_insert(component,top);
if(cmp(top,x)==0)break;}}
# 378
for(1;(unsigned)*S;*S=(*S)->tl){
void*top=(*S)->hd;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,unsigned)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert;_Tmp1;})(*N,top,4294967295U);*N=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=Cyc_Graph_add_node(*output,top);*output=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=Cyc_Graph_add_edges(*output,top,component);*output=_Tmp0;});
if(cmp(top,x)==0){
*S=(*S)->tl;
break;}}}}
# 390
struct Cyc_Dict_Dict Cyc_Graph_scc(struct Cyc_Dict_Dict input){
int(*cmp)(void*,void*)=input.rel;
struct Cyc_List_List*edges=Cyc_Dict_to_list(input);
struct Cyc_Dict_Dict output=Cyc_Graph_empty(cmp);
struct Cyc_List_List*S=0;
struct Cyc_Dict_Dict N=Cyc_Dict_empty(cmp);
{struct Cyc_List_List*e=edges;for(0;(unsigned)e;e=e->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)e->hd;void*_Tmp1;_Tmp1=(void*)_Tmp0->f0;{void*x=_Tmp1;
N=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,void*,unsigned)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert;_Tmp2;})(N,x,0U);}}}
# 400
{struct Cyc_List_List*e=edges;for(0;(unsigned)e;e=e->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)e->hd;void*_Tmp1;_Tmp1=(void*)_Tmp0->f0;{void*x=_Tmp1;
if(({unsigned(*_Tmp2)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp2;})(N,x)==0U)
Cyc_Graph_traverse(input,& output,& N,& S,x,1U);}}}
# 405
return output;}
# 409
static void Cyc_Graph_tsort0(struct Cyc_Dict_Dict input,struct Cyc_List_List**output,struct Cyc_Dict_Dict*N,struct Cyc_List_List**S,void*x,unsigned k){
# 413
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=x,_Tmp1->tl=*S;_Tmp1;});*S=_Tmp0;});
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,unsigned)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert;_Tmp1;})(*N,x,k);*N=_Tmp0;});
{struct Cyc_Set_Set*targets=Cyc_Graph_get_targets(input,x);for(0;!Cyc_Set_is_empty(targets);1){
void*y=Cyc_Set_choose(targets);
targets=Cyc_Set_delete(targets,y);
if(({unsigned(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(*N,y)==0U)
Cyc_Graph_tsort0(input,output,N,S,y,k + 1U);{
unsigned Ny=({unsigned(*_Tmp0)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp0;})(*N,y);
if(({unsigned _Tmp0=Ny;_Tmp0 < ({unsigned(*_Tmp1)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp1;})(*N,x);}))
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,unsigned)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert;_Tmp1;})(*N,x,Ny);*N=_Tmp0;});}}}
# 424
if(({unsigned _Tmp0=({unsigned(*_Tmp1)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp1;})(*N,x);_Tmp0==k;})){
int(*cmp)(void*,void*)=input.rel;
struct Cyc_Set_Set*component=Cyc_Set_empty(cmp);
{struct Cyc_List_List*s=*S;for(0;(unsigned)s;s=s->tl){
void*top=s->hd;
component=Cyc_Set_insert(component,top);
if(cmp(top,x)==0)break;}}
# 432
for(1;(unsigned)*S;*S=(*S)->tl){
void*top=(*S)->hd;
({struct Cyc_Dict_Dict _Tmp0=({struct Cyc_Dict_Dict(*_Tmp1)(struct Cyc_Dict_Dict,void*,unsigned)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert;_Tmp1;})(*N,top,4294967295U);*N=_Tmp0;});
({struct Cyc_List_List*_Tmp0=({struct Cyc_List_List*_Tmp1=_cycalloc(sizeof(struct Cyc_List_List));_Tmp1->hd=top,_Tmp1->tl=*output;_Tmp1;});*output=_Tmp0;});
if(cmp(top,x)==0){
*S=(*S)->tl;
break;}}}}
# 443
struct Cyc_List_List*Cyc_Graph_tsort(struct Cyc_Dict_Dict input){
int(*cmp)(void*,void*)=input.rel;
struct Cyc_List_List*edges=Cyc_Dict_to_list(input);
struct Cyc_List_List*output=0;
struct Cyc_List_List*S=0;
struct Cyc_Dict_Dict N=Cyc_Dict_empty(cmp);
{struct Cyc_List_List*e=edges;for(0;(unsigned)e;e=e->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)e->hd;void*_Tmp1;_Tmp1=(void*)_Tmp0->f0;{void*x=_Tmp1;
N=({struct Cyc_Dict_Dict(*_Tmp2)(struct Cyc_Dict_Dict,void*,unsigned)=(struct Cyc_Dict_Dict(*)(struct Cyc_Dict_Dict,void*,unsigned))Cyc_Dict_insert;_Tmp2;})(N,x,0U);}}}
# 453
{struct Cyc_List_List*e=edges;for(0;(unsigned)e;e=e->tl){
struct _tuple0*_Tmp0=(struct _tuple0*)e->hd;void*_Tmp1;_Tmp1=(void*)_Tmp0->f0;{void*x=_Tmp1;
if(({unsigned(*_Tmp2)(struct Cyc_Dict_Dict,void*)=(unsigned(*)(struct Cyc_Dict_Dict,void*))Cyc_Dict_lookup;_Tmp2;})(N,x)==0U)
Cyc_Graph_tsort0(input,& output,& N,& S,x,1U);}}}
# 458
return output;}
