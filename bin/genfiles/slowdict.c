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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};struct Cyc_Splay_Leaf_Splay_tree_struct{int tag;int f1;};struct Cyc_Splay_Node_Splay_tree_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*left;void*right;};
# 49 "splay.h"
extern int Cyc_Splay_splay(int(*)(void*,void*),void*,void*);struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[8U];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7U];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};char Cyc_SlowDict_Absent[7U]="Absent";char Cyc_SlowDict_Present[8U]="Present";
# 32 "slowdict.cyc"
struct Cyc_SlowDict_Absent_exn_struct Cyc_SlowDict_Absent_val={Cyc_SlowDict_Absent};
struct Cyc_SlowDict_Present_exn_struct Cyc_SlowDict_Present_val={Cyc_SlowDict_Present};struct Cyc_SlowDict_Dict{int(*reln)(void*,void*);void*tree;};
# 41
struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*comp)(void*,void*)){struct Cyc_Splay_Leaf_Splay_tree_struct*_T0;struct Cyc_SlowDict_Dict*_T1;{struct Cyc_Splay_Leaf_Splay_tree_struct*_T2=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_T2->tag=0;
_T2->f1=0;_T0=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_T2;}{void*t=(void*)_T0;{struct Cyc_SlowDict_Dict*_T2=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));
_T2->reln=comp;_T2->tree=t;_T1=(struct Cyc_SlowDict_Dict*)_T2;}return _T1;}}
# 49
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d){struct Cyc_SlowDict_Dict*_T0;int*_T1;int _T2;_T0=d;{
void*_T3=_T0->tree;_T1=(int*)_T3;_T2=*_T1;if(_T2!=0)goto _TL0;
return 1;_TL0:
 return 0;;}}
# 56
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*key){struct Cyc_SlowDict_Dict*_T0;int(*_T1)(void*,void*);void*_T2;struct Cyc_SlowDict_Dict*_T3;void*_T4;int _T5;_T0=d;_T1=_T0->reln;_T2=key;_T3=d;_T4=_T3->tree;_T5=
Cyc_Splay_splay(_T1,_T2,_T4);return _T5;}
# 60
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*key,void*data){struct Cyc_Splay_Leaf_Splay_tree_struct*_T0;struct Cyc_SlowDict_Dict*_T1;int(*_T2)(void*,void*);void*_T3;struct Cyc_SlowDict_Dict*_T4;void*_T5;int _T6;struct Cyc_SlowDict_Dict*_T7;int*_T8;int _T9;struct Cyc_Splay_noderef*_TA;struct Cyc_Splay_node*_TB;struct Cyc_Splay_noderef*_TC;struct Cyc_Splay_node*_TD;struct Cyc_SlowDict_Dict*_TE;int*_TF;int _T10;struct Cyc_Splay_noderef*_T11;struct Cyc_SlowDict_Dict*_T12;int(*_T13)(void*,void*);void*_T14;struct Cyc_Splay_node*_T15;void*_T16;int _T17;struct Cyc_Splay_node*_T18;struct Cyc_Splay_Node_Splay_tree_struct*_T19;struct Cyc_Splay_noderef*_T1A;struct Cyc_Splay_node*_T1B;struct Cyc_Splay_node*_T1C;struct Cyc_Splay_node*_T1D;struct Cyc_Splay_node*_T1E;struct Cyc_Splay_Node_Splay_tree_struct*_T1F;struct Cyc_Splay_noderef*_T20;struct Cyc_Splay_node*_T21;struct Cyc_Splay_node*_T22;struct Cyc_Splay_node*_T23;struct Cyc_Splay_node*_T24;struct Cyc_Splay_node*_T25;struct Cyc_SlowDict_Dict*_T26;struct Cyc_SlowDict_Dict*_T27;struct Cyc_Splay_Node_Splay_tree_struct*_T28;struct Cyc_Splay_noderef*_T29;struct Cyc_Splay_node*_T2A;{struct Cyc_Splay_Leaf_Splay_tree_struct*_T2B=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_T2B->tag=0;
_T2B->f1=0;_T0=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_T2B;}{void*leaf=(void*)_T0;
void*newleft=leaf;void*newright=leaf;_T1=d;_T2=_T1->reln;_T3=key;_T4=d;_T5=_T4->tree;_T6=
Cyc_Splay_splay(_T2,_T3,_T5);if(!_T6)goto _TL2;_T7=d;{
# 65
void*_T2B=_T7->tree;struct Cyc_Splay_noderef*_T2C;_T8=(int*)_T2B;_T9=*_T8;if(_T9!=1)goto _TL4;{struct Cyc_Splay_Node_Splay_tree_struct*_T2D=(struct Cyc_Splay_Node_Splay_tree_struct*)_T2B;_T2C=_T2D->f1;}{struct Cyc_Splay_noderef*n=_T2C;_TA=n;_TB=_TA->v;
# 67
newleft=_TB->left;_TC=n;_TD=_TC->v;
newright=_TD->right;goto _LL0;}_TL4: goto _LL0;_LL0:;}goto _TL3;
# 75
_TL2: _TE=d;{void*_T2B=_TE->tree;struct Cyc_Splay_noderef*_T2C;_TF=(int*)_T2B;_T10=*_TF;if(_T10!=1)goto _TL6;{struct Cyc_Splay_Node_Splay_tree_struct*_T2D=(struct Cyc_Splay_Node_Splay_tree_struct*)_T2B;_T2C=_T2D->f1;}{struct Cyc_Splay_noderef*nr=_T2C;_T11=nr;{
# 77
struct Cyc_Splay_node*n=_T11->v;_T12=d;_T13=_T12->reln;_T14=key;_T15=n;_T16=_T15->key;_T17=
_T13(_T14,_T16);if(_T17 >= 0)goto _TL8;_T18=n;
newleft=_T18->left;{struct Cyc_Splay_Node_Splay_tree_struct*_T2D=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_T2D->tag=1;{struct Cyc_Splay_noderef*_T2E=_cycalloc(sizeof(struct Cyc_Splay_noderef));{struct Cyc_Splay_node*_T2F=_cycalloc(sizeof(struct Cyc_Splay_node));_T1C=n;
# 81
_T2F->key=_T1C->key;_T1D=n;_T2F->data=_T1D->data;
_T2F->left=leaf;_T1E=n;_T2F->right=_T1E->right;_T1B=(struct Cyc_Splay_node*)_T2F;}
# 80
_T2E->v=_T1B;_T1A=(struct Cyc_Splay_noderef*)_T2E;}_T2D->f1=_T1A;_T19=(struct Cyc_Splay_Node_Splay_tree_struct*)_T2D;}newright=(void*)_T19;goto _TL9;
# 84
_TL8:{struct Cyc_Splay_Node_Splay_tree_struct*_T2D=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_T2D->tag=1;{struct Cyc_Splay_noderef*_T2E=_cycalloc(sizeof(struct Cyc_Splay_noderef));{struct Cyc_Splay_node*_T2F=_cycalloc(sizeof(struct Cyc_Splay_node));_T22=n;
_T2F->key=_T22->key;_T23=n;_T2F->data=_T23->data;_T24=n;_T2F->left=_T24->left;_T2F->right=leaf;_T21=(struct Cyc_Splay_node*)_T2F;}
# 84
_T2E->v=_T21;_T20=(struct Cyc_Splay_noderef*)_T2E;}_T2D->f1=_T20;_T1F=(struct Cyc_Splay_Node_Splay_tree_struct*)_T2D;}newleft=(void*)_T1F;_T25=n;
# 86
newright=_T25->right;_TL9: goto _LL5;}}_TL6: goto _LL5;_LL5:;}_TL3:{struct Cyc_SlowDict_Dict*_T2B=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_T27=d;
# 93
_T2B->reln=_T27->reln;{struct Cyc_Splay_Node_Splay_tree_struct*_T2C=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_T2C->tag=1;{struct Cyc_Splay_noderef*_T2D=_cycalloc(sizeof(struct Cyc_Splay_noderef));{struct Cyc_Splay_node*_T2E=_cycalloc(sizeof(struct Cyc_Splay_node));
_T2E->key=key;_T2E->data=data;
_T2E->left=newleft;_T2E->right=newright;_T2A=(struct Cyc_Splay_node*)_T2E;}
# 94
_T2D->v=_T2A;_T29=(struct Cyc_Splay_noderef*)_T2D;}
# 93
_T2C->f1=_T29;_T28=(struct Cyc_Splay_Node_Splay_tree_struct*)_T2C;}_T2B->tree=(void*)_T28;_T26=(struct Cyc_SlowDict_Dict*)_T2B;}return _T26;}}
# 98
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*key,void*data){struct Cyc_SlowDict_Dict*_T0;int(*_T1)(void*,void*);void*_T2;struct Cyc_SlowDict_Dict*_T3;void*_T4;int _T5;struct Cyc_SlowDict_Present_exn_struct*_T6;struct Cyc_SlowDict_Present_exn_struct*_T7;struct Cyc_SlowDict_Dict*_T8;_T0=d;_T1=_T0->reln;_T2=key;_T3=d;_T4=_T3->tree;_T5=
# 100
Cyc_Splay_splay(_T1,_T2,_T4);if(!_T5)goto _TLA;_T6=& Cyc_SlowDict_Present_val;_T7=(struct Cyc_SlowDict_Present_exn_struct*)_T6;_throw(_T7);goto _TLB;_TLA: _TLB: _T8=
# 102
Cyc_SlowDict_insert(d,key,data);return _T8;}struct _tuple0{void*f0;void*f1;};
# 105
struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*kds){struct Cyc_SlowDict_Dict*_T0;struct Cyc_List_List*_T1;void*_T2;struct _tuple0*_T3;struct _tuple0 _T4;void*_T5;struct Cyc_List_List*_T6;void*_T7;struct _tuple0*_T8;struct _tuple0 _T9;void*_TA;struct Cyc_List_List*_TB;struct Cyc_SlowDict_Dict*_TC;
# 107
_TLF: if(kds!=0)goto _TLD;else{goto _TLE;}
_TLD: _T0=d;_T1=kds;_T2=_T1->hd;_T3=(struct _tuple0*)_T2;_T4=*_T3;_T5=_T4.f0;_T6=kds;_T7=_T6->hd;_T8=(struct _tuple0*)_T7;_T9=*_T8;_TA=_T9.f1;d=Cyc_SlowDict_insert(_T0,_T5,_TA);_TB=kds;
# 107
kds=_TB->tl;goto _TLF;_TLE: _TC=d;
# 109
return _TC;}
# 112
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*comp)(void*,void*),void*key,void*data){struct Cyc_Splay_Leaf_Splay_tree_struct*_T0;struct Cyc_Splay_Leaf_Splay_tree_struct*_T1;struct Cyc_SlowDict_Dict*_T2;struct Cyc_Splay_Node_Splay_tree_struct*_T3;struct Cyc_Splay_noderef*_T4;struct Cyc_Splay_node*_T5;struct Cyc_Splay_Leaf_Splay_tree_struct*_T6;struct Cyc_Splay_Leaf_Splay_tree_struct*_T7;
struct Cyc_Splay_Leaf_Splay_tree_struct*leaf;leaf=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_T0=leaf;_T0->tag=0;_T1=leaf;_T1->f1=0;{struct Cyc_SlowDict_Dict*_T8=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));
_T8->reln=comp;{struct Cyc_Splay_Node_Splay_tree_struct*_T9=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_T9->tag=1;{struct Cyc_Splay_noderef*_TA=_cycalloc(sizeof(struct Cyc_Splay_noderef));{struct Cyc_Splay_node*_TB=_cycalloc(sizeof(struct Cyc_Splay_node));_TB->key=key;_TB->data=data;_T6=leaf;_TB->left=(void*)_T6;_T7=leaf;_TB->right=(void*)_T7;_T5=(struct Cyc_Splay_node*)_TB;}_TA->v=_T5;_T4=(struct Cyc_Splay_noderef*)_TA;}_T9->f1=_T4;_T3=(struct Cyc_Splay_Node_Splay_tree_struct*)_T9;}_T8->tree=(void*)_T3;_T2=(struct Cyc_SlowDict_Dict*)_T8;}return _T2;}
# 117
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*key){struct Cyc_SlowDict_Dict*_T0;int(*_T1)(void*,void*);void*_T2;struct Cyc_SlowDict_Dict*_T3;void*_T4;int _T5;struct Cyc_SlowDict_Dict*_T6;int*_T7;int _T8;struct Cyc_Splay_noderef*_T9;struct Cyc_Splay_node*_TA;void*_TB;struct Cyc_Core_Impossible_exn_struct*_TC;void*_TD;struct Cyc_SlowDict_Absent_exn_struct*_TE;struct Cyc_SlowDict_Absent_exn_struct*_TF;_T0=d;_T1=_T0->reln;_T2=key;_T3=d;_T4=_T3->tree;_T5=
Cyc_Splay_splay(_T1,_T2,_T4);if(!_T5)goto _TL10;_T6=d;{
void*_T10=_T6->tree;struct Cyc_Splay_noderef*_T11;_T7=(int*)_T10;_T8=*_T7;if(_T8!=1)goto _TL12;{struct Cyc_Splay_Node_Splay_tree_struct*_T12=(struct Cyc_Splay_Node_Splay_tree_struct*)_T10;_T11=_T12->f1;}{struct Cyc_Splay_noderef*nr=_T11;_T9=nr;_TA=_T9->v;_TB=_TA->data;
return _TB;}_TL12:{struct Cyc_Core_Impossible_exn_struct*_T12=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T12->tag=Cyc_Core_Impossible;
_T12->f1=_tag_fat("Dict::lookup",sizeof(char),13U);_TC=(struct Cyc_Core_Impossible_exn_struct*)_T12;}_TD=(void*)_TC;_throw(_TD);;}goto _TL11;_TL10: _TL11: _TE=& Cyc_SlowDict_Absent_val;_TF=(struct Cyc_SlowDict_Absent_exn_struct*)_TE;_throw(_TF);}
# 126
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*key){struct Cyc_SlowDict_Dict*_T0;int(*_T1)(void*,void*);void*_T2;struct Cyc_SlowDict_Dict*_T3;void*_T4;int _T5;struct Cyc_SlowDict_Dict*_T6;int*_T7;int _T8;struct Cyc_Core_Opt*_T9;struct Cyc_Splay_noderef*_TA;struct Cyc_Splay_node*_TB;struct Cyc_Core_Impossible_exn_struct*_TC;void*_TD;_T0=d;_T1=_T0->reln;_T2=key;_T3=d;_T4=_T3->tree;_T5=
Cyc_Splay_splay(_T1,_T2,_T4);if(!_T5)goto _TL14;_T6=d;{
void*_TE=_T6->tree;struct Cyc_Splay_noderef*_TF;_T7=(int*)_TE;_T8=*_T7;if(_T8!=1)goto _TL16;{struct Cyc_Splay_Node_Splay_tree_struct*_T10=(struct Cyc_Splay_Node_Splay_tree_struct*)_TE;_TF=_T10->f1;}{struct Cyc_Splay_noderef*nr=_TF;{struct Cyc_Core_Opt*_T10=_cycalloc(sizeof(struct Cyc_Core_Opt));_TA=nr;_TB=_TA->v;
_T10->v=_TB->data;_T9=(struct Cyc_Core_Opt*)_T10;}return _T9;}_TL16:{struct Cyc_Core_Impossible_exn_struct*_T10=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T10->tag=Cyc_Core_Impossible;
_T10->f1=_tag_fat("Dict::lookup",sizeof(char),13U);_TC=(struct Cyc_Core_Impossible_exn_struct*)_T10;}_TD=(void*)_TC;_throw(_TD);;}goto _TL15;_TL14: _TL15:
# 132
 return 0;}
# 135
static int Cyc_SlowDict_get_largest(void*x,void*y){return 1;}
# 137
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*key){struct Cyc_SlowDict_Dict*_T0;int(*_T1)(void*,void*);void*_T2;struct Cyc_SlowDict_Dict*_T3;void*_T4;int _T5;struct Cyc_SlowDict_Dict*_T6;int*_T7;int _T8;struct Cyc_Core_Impossible_exn_struct*_T9;void*_TA;struct Cyc_Splay_noderef*_TB;struct Cyc_Splay_node*_TC;int*_TD;int _TE;struct Cyc_SlowDict_Dict*_TF;struct Cyc_SlowDict_Dict*_T10;struct Cyc_Splay_node*_T11;struct Cyc_Splay_node*_T12;int*_T13;int _T14;struct Cyc_SlowDict_Dict*_T15;struct Cyc_SlowDict_Dict*_T16;struct Cyc_Splay_node*_T17;void*_T18;struct Cyc_Splay_node*_T19;void*_T1A;struct Cyc_Splay_noderef*_T1B;struct Cyc_SlowDict_Dict*_T1C;struct Cyc_SlowDict_Dict*_T1D;struct Cyc_Splay_Node_Splay_tree_struct*_T1E;struct Cyc_Splay_noderef*_T1F;struct Cyc_Splay_node*_T20;struct Cyc_Splay_node*_T21;struct Cyc_Splay_node*_T22;struct Cyc_Splay_node*_T23;struct Cyc_Splay_node*_T24;struct Cyc_SlowDict_Dict*_T25;_T0=d;_T1=_T0->reln;_T2=key;_T3=d;_T4=_T3->tree;_T5=
Cyc_Splay_splay(_T1,_T2,_T4);if(!_T5)goto _TL18;_T6=d;{
void*_T26=_T6->tree;struct Cyc_Splay_noderef*_T27;_T7=(int*)_T26;_T8=*_T7;if(_T8!=0)goto _TL1A;{struct Cyc_Core_Impossible_exn_struct*_T28=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_T28->tag=Cyc_Core_Impossible;
_T28->f1=_tag_fat("Dict::lookup",sizeof(char),13U);_T9=(struct Cyc_Core_Impossible_exn_struct*)_T28;}_TA=(void*)_T9;_throw(_TA);goto _TL1B;_TL1A:{struct Cyc_Splay_Node_Splay_tree_struct*_T28=(struct Cyc_Splay_Node_Splay_tree_struct*)_T26;_T27=_T28->f1;}{struct Cyc_Splay_noderef*nr=_T27;_TB=nr;{
# 142
struct Cyc_Splay_node*n=_TB->v;_TC=n;{
void*_T28=_TC->left;struct Cyc_Splay_noderef*_T29;_TD=(int*)_T28;_TE=*_TD;if(_TE!=0)goto _TL1C;{struct Cyc_SlowDict_Dict*_T2A=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_T10=d;
_T2A->reln=_T10->reln;_T11=n;_T2A->tree=_T11->right;_TF=(struct Cyc_SlowDict_Dict*)_T2A;}return _TF;_TL1C:{struct Cyc_Splay_Node_Splay_tree_struct*_T2A=(struct Cyc_Splay_Node_Splay_tree_struct*)_T28;_T29=_T2A->f1;}{struct Cyc_Splay_noderef*noderef_left=_T29;_T12=n;{
# 146
void*_T2A=_T12->right;struct Cyc_Splay_noderef*_T2B;_T13=(int*)_T2A;_T14=*_T13;if(_T14!=0)goto _TL1E;{struct Cyc_SlowDict_Dict*_T2C=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_T16=d;
_T2C->reln=_T16->reln;_T17=n;_T2C->tree=_T17->left;_T15=(struct Cyc_SlowDict_Dict*)_T2C;}return _T15;_TL1E:{struct Cyc_Splay_Node_Splay_tree_struct*_T2C=(struct Cyc_Splay_Node_Splay_tree_struct*)_T2A;_T2B=_T2C->f1;}{struct Cyc_Splay_noderef*node_ref_right=_T2B;_T18=key;_T19=n;_T1A=_T19->left;
# 149
Cyc_Splay_splay(Cyc_SlowDict_get_largest,_T18,_T1A);_T1B=noderef_left;{
struct Cyc_Splay_node*newtop=_T1B->v;{struct Cyc_SlowDict_Dict*_T2C=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_T1D=d;
_T2C->reln=_T1D->reln;{struct Cyc_Splay_Node_Splay_tree_struct*_T2D=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_T2D->tag=1;{struct Cyc_Splay_noderef*_T2E=_cycalloc(sizeof(struct Cyc_Splay_noderef));{struct Cyc_Splay_node*_T2F=_cycalloc(sizeof(struct Cyc_Splay_node));_T21=newtop;
_T2F->key=_T21->key;_T22=newtop;_T2F->data=_T22->data;_T23=newtop;
_T2F->left=_T23->left;_T24=n;_T2F->right=_T24->right;_T20=(struct Cyc_Splay_node*)_T2F;}
# 152
_T2E->v=_T20;_T1F=(struct Cyc_Splay_noderef*)_T2E;}_T2D->f1=_T1F;_T1E=(struct Cyc_Splay_Node_Splay_tree_struct*)_T2D;}_T2C->tree=(void*)_T1E;_T1C=(struct Cyc_SlowDict_Dict*)_T2C;}
# 151
return _T1C;}};}};}}}_TL1B:;}goto _TL19;
# 158
_TL18: _T25=d;return _T25;_TL19:;}
# 161
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*key){struct Cyc_SlowDict_Absent_exn_struct*_T0;struct Cyc_SlowDict_Absent_exn_struct*_T1;struct Cyc_SlowDict_Dict*_T2;
struct Cyc_SlowDict_Dict*d2=Cyc_SlowDict_delete(d,key);
if(d!=d2)goto _TL20;_T0=& Cyc_SlowDict_Absent_val;_T1=(struct Cyc_SlowDict_Absent_exn_struct*)_T0;_throw(_T1);goto _TL21;_TL20: _TL21: _T2=d2;
# 165
return _T2;}
# 168
static void*Cyc_SlowDict_fold_tree(void*(*f)(void*,void*,void*),void*t,void*accum){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Splay_noderef*_T5;struct Cyc_Splay_node*_T6;void*_T7;struct Cyc_Splay_node*_T8;void*_T9;void*(*_TA)(void*,void*,void*);struct Cyc_Splay_node*_TB;void*_TC;void*(*_TD)(void*,void*,void*);struct Cyc_Splay_node*_TE;void*_TF;void*_T10;void*_T11;void*_T12;void*_T13;struct Cyc_Splay_noderef*_T14;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL22;_T3=accum;
# 171
return _T3;_TL22: _T4=t;{struct Cyc_Splay_Node_Splay_tree_struct*_T15=(struct Cyc_Splay_Node_Splay_tree_struct*)_T4;_T14=_T15->f1;}{struct Cyc_Splay_noderef*nr=_T14;_T5=nr;{
# 173
struct Cyc_Splay_node*n=_T5->v;_T6=n;_T7=_T6->key;_T8=n;_T9=_T8->data;_TA=f;_TB=n;_TC=_TB->left;_TD=f;_TE=n;_TF=_TE->right;_T10=accum;_T11=
# 175
Cyc_SlowDict_fold_tree(_TD,_TF,_T10);_T12=
# 174
Cyc_SlowDict_fold_tree(_TA,_TC,_T11);_T13=f(_T7,_T9,_T12);return _T13;}};}
# 178
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum){void*(*_T0)(void*,void*,void*);struct Cyc_SlowDict_Dict*_T1;void*_T2;void*_T3;void*_T4;_T0=f;_T1=d;_T2=_T1->tree;_T3=accum;_T4=
Cyc_SlowDict_fold_tree(_T0,_T2,_T3);return _T4;}
# 182
static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,void*t,void*accum){void*_T0;int*_T1;int _T2;void*_T3;void*_T4;struct Cyc_Splay_noderef*_T5;void*_T6;struct Cyc_Splay_node*_T7;void*_T8;struct Cyc_Splay_node*_T9;void*_TA;void*(*_TB)(void*,void*,void*,void*);void*_TC;struct Cyc_Splay_node*_TD;void*_TE;void*(*_TF)(void*,void*,void*,void*);void*_T10;struct Cyc_Splay_node*_T11;void*_T12;void*_T13;void*_T14;void*_T15;void*_T16;struct Cyc_Splay_noderef*_T17;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL24;_T3=accum;
# 185
return _T3;_TL24: _T4=t;{struct Cyc_Splay_Node_Splay_tree_struct*_T18=(struct Cyc_Splay_Node_Splay_tree_struct*)_T4;_T17=_T18->f1;}{struct Cyc_Splay_noderef*nr=_T17;_T5=nr;{
# 187
struct Cyc_Splay_node*n=_T5->v;_T6=env;_T7=n;_T8=_T7->key;_T9=n;_TA=_T9->data;_TB=f;_TC=env;_TD=n;_TE=_TD->left;_TF=f;_T10=env;_T11=n;_T12=_T11->right;_T13=accum;_T14=
# 189
Cyc_SlowDict_fold_tree_c(_TF,_T10,_T12,_T13);_T15=Cyc_SlowDict_fold_tree_c(_TB,_TC,_TE,_T14);_T16=
# 188
f(_T6,_T8,_TA,_T15);return _T16;}};}
# 192
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*dict,void*accum){void*(*_T0)(void*,void*,void*,void*);void*_T1;struct Cyc_SlowDict_Dict*_T2;void*_T3;void*_T4;void*_T5;_T0=f;_T1=env;_T2=dict;_T3=_T2->tree;_T4=accum;_T5=
Cyc_SlowDict_fold_tree_c(_T0,_T1,_T3,_T4);return _T5;}
# 196
static void Cyc_SlowDict_app_tree(void*(*f)(void*,void*),void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Splay_noderef*_T4;void*(*_T5)(void*,void*);struct Cyc_Splay_node*_T6;void*_T7;struct Cyc_Splay_node*_T8;void*_T9;struct Cyc_Splay_node*_TA;void*_TB;void*(*_TC)(void*,void*);struct Cyc_Splay_node*_TD;void*_TE;struct Cyc_Splay_noderef*_TF;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL26;goto _LL0;_TL26: _T3=t;{struct Cyc_Splay_Node_Splay_tree_struct*_T10=(struct Cyc_Splay_Node_Splay_tree_struct*)_T3;_TF=_T10->f1;}{struct Cyc_Splay_noderef*nr=_TF;_T4=nr;{
# 200
struct Cyc_Splay_node*n=_T4->v;_T5=f;_T6=n;_T7=_T6->left;
Cyc_SlowDict_app_tree(_T5,_T7);_T8=n;_T9=_T8->key;_TA=n;_TB=_TA->data;
f(_T9,_TB);_TC=f;_TD=n;_TE=_TD->right;
Cyc_SlowDict_app_tree(_TC,_TE);goto _LL0;}}_LL0:;}
# 207
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){void*(*_T0)(void*,void*);struct Cyc_SlowDict_Dict*_T1;void*_T2;_T0=f;_T1=d;_T2=_T1->tree;
Cyc_SlowDict_app_tree(_T0,_T2);}
# 211
static void Cyc_SlowDict_iter_tree(void(*f)(void*,void*),void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Splay_noderef*_T4;void(*_T5)(void*,void*);struct Cyc_Splay_node*_T6;void*_T7;struct Cyc_Splay_node*_T8;void*_T9;struct Cyc_Splay_node*_TA;void*_TB;void(*_TC)(void*,void*);struct Cyc_Splay_node*_TD;void*_TE;struct Cyc_Splay_noderef*_TF;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL28;goto _LL0;_TL28: _T3=t;{struct Cyc_Splay_Node_Splay_tree_struct*_T10=(struct Cyc_Splay_Node_Splay_tree_struct*)_T3;_TF=_T10->f1;}{struct Cyc_Splay_noderef*nr=_TF;_T4=nr;{
# 215
struct Cyc_Splay_node*n=_T4->v;_T5=f;_T6=n;_T7=_T6->left;
Cyc_SlowDict_iter_tree(_T5,_T7);_T8=n;_T9=_T8->key;_TA=n;_TB=_TA->data;
f(_T9,_TB);_TC=f;_TD=n;_TE=_TD->right;
Cyc_SlowDict_iter_tree(_TC,_TE);goto _LL0;}}_LL0:;}
# 222
void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){void(*_T0)(void*,void*);struct Cyc_SlowDict_Dict*_T1;void*_T2;_T0=f;_T1=d;_T2=_T1->tree;
Cyc_SlowDict_iter_tree(_T0,_T2);}
# 226
static void Cyc_SlowDict_app_tree_c(void*(*f)(void*,void*,void*),void*env,void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Splay_noderef*_T4;void*(*_T5)(void*,void*,void*);void*_T6;struct Cyc_Splay_node*_T7;void*_T8;void*_T9;struct Cyc_Splay_node*_TA;void*_TB;struct Cyc_Splay_node*_TC;void*_TD;void*(*_TE)(void*,void*,void*);void*_TF;struct Cyc_Splay_node*_T10;void*_T11;struct Cyc_Splay_noderef*_T12;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL2A;goto _LL0;_TL2A: _T3=t;{struct Cyc_Splay_Node_Splay_tree_struct*_T13=(struct Cyc_Splay_Node_Splay_tree_struct*)_T3;_T12=_T13->f1;}{struct Cyc_Splay_noderef*nr=_T12;_T4=nr;{
# 230
struct Cyc_Splay_node*n=_T4->v;_T5=f;_T6=env;_T7=n;_T8=_T7->left;
Cyc_SlowDict_app_tree_c(_T5,_T6,_T8);_T9=env;_TA=n;_TB=_TA->key;_TC=n;_TD=_TC->data;
f(_T9,_TB,_TD);_TE=f;_TF=env;_T10=n;_T11=_T10->right;
Cyc_SlowDict_app_tree_c(_TE,_TF,_T11);goto _LL0;}}_LL0:;}
# 237
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){void*(*_T0)(void*,void*,void*);void*_T1;struct Cyc_SlowDict_Dict*_T2;void*_T3;_T0=f;_T1=env;_T2=d;_T3=_T2->tree;
Cyc_SlowDict_app_tree_c(_T0,_T1,_T3);}
# 241
static void Cyc_SlowDict_iter_tree_c(void(*f)(void*,void*,void*),void*env,void*t){void*_T0;int*_T1;int _T2;void*_T3;struct Cyc_Splay_noderef*_T4;void(*_T5)(void*,void*,void*);void*_T6;struct Cyc_Splay_node*_T7;void*_T8;void*_T9;struct Cyc_Splay_node*_TA;void*_TB;struct Cyc_Splay_node*_TC;void*_TD;void(*_TE)(void*,void*,void*);void*_TF;struct Cyc_Splay_node*_T10;void*_T11;struct Cyc_Splay_noderef*_T12;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL2C;goto _LL0;_TL2C: _T3=t;{struct Cyc_Splay_Node_Splay_tree_struct*_T13=(struct Cyc_Splay_Node_Splay_tree_struct*)_T3;_T12=_T13->f1;}{struct Cyc_Splay_noderef*nr=_T12;_T4=nr;{
# 245
struct Cyc_Splay_node*n=_T4->v;_T5=f;_T6=env;_T7=n;_T8=_T7->left;
Cyc_SlowDict_iter_tree_c(_T5,_T6,_T8);_T9=env;_TA=n;_TB=_TA->key;_TC=n;_TD=_TC->data;
f(_T9,_TB,_TD);_TE=f;_TF=env;_T10=n;_T11=_T10->right;
Cyc_SlowDict_iter_tree_c(_TE,_TF,_T11);goto _LL0;}}_LL0:;}
# 252
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){void(*_T0)(void*,void*,void*);void*_T1;struct Cyc_SlowDict_Dict*_T2;void*_T3;_T0=f;_T1=env;_T2=d;_T3=_T2->tree;
Cyc_SlowDict_iter_tree_c(_T0,_T1,_T3);}
# 256
static void*Cyc_SlowDict_map_tree(void*(*f)(void*),void*t){void*_T0;int*_T1;int _T2;struct Cyc_Splay_Leaf_Splay_tree_struct*_T3;void*_T4;void*_T5;struct Cyc_Splay_noderef*_T6;struct Cyc_Splay_Node_Splay_tree_struct*_T7;struct Cyc_Splay_noderef*_T8;struct Cyc_Splay_node*_T9;struct Cyc_Splay_node*_TA;struct Cyc_Splay_node*_TB;void*_TC;void*(*_TD)(void*);struct Cyc_Splay_node*_TE;void*_TF;void*(*_T10)(void*);struct Cyc_Splay_node*_T11;void*_T12;void*_T13;struct Cyc_Splay_noderef*_T14;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL2E;{struct Cyc_Splay_Leaf_Splay_tree_struct*_T15=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_T15->tag=0;
# 258
_T15->f1=0;_T3=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_T15;}_T4=(void*)_T3;return _T4;_TL2E: _T5=t;{struct Cyc_Splay_Node_Splay_tree_struct*_T15=(struct Cyc_Splay_Node_Splay_tree_struct*)_T5;_T14=_T15->f1;}{struct Cyc_Splay_noderef*nr=_T14;_T6=nr;{
# 260
struct Cyc_Splay_node*n=_T6->v;{struct Cyc_Splay_Node_Splay_tree_struct*_T15=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_T15->tag=1;{struct Cyc_Splay_noderef*_T16=_cycalloc(sizeof(struct Cyc_Splay_noderef));{struct Cyc_Splay_node*_T17=_cycalloc(sizeof(struct Cyc_Splay_node));_TA=n;
_T17->key=_TA->key;_TB=n;_TC=_TB->data;
_T17->data=f(_TC);_TD=f;_TE=n;_TF=_TE->left;
_T17->left=Cyc_SlowDict_map_tree(_TD,_TF);_T10=f;_T11=n;_T12=_T11->right;
_T17->right=Cyc_SlowDict_map_tree(_T10,_T12);_T9=(struct Cyc_Splay_node*)_T17;}
# 261
_T16->v=_T9;_T8=(struct Cyc_Splay_noderef*)_T16;}_T15->f1=_T8;_T7=(struct Cyc_Splay_Node_Splay_tree_struct*)_T15;}_T13=(void*)_T7;return _T13;}};}
# 267
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d){struct Cyc_SlowDict_Dict*_T0;struct Cyc_SlowDict_Dict*_T1;void*(*_T2)(void*);struct Cyc_SlowDict_Dict*_T3;void*_T4;{struct Cyc_SlowDict_Dict*_T5=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_T1=d;
_T5->reln=_T1->reln;_T2=f;_T3=d;_T4=_T3->tree;_T5->tree=Cyc_SlowDict_map_tree(_T2,_T4);_T0=(struct Cyc_SlowDict_Dict*)_T5;}return _T0;}
# 271
static void*Cyc_SlowDict_map_tree_c(void*(*f)(void*,void*),void*env,void*t){void*_T0;int*_T1;int _T2;struct Cyc_Splay_Leaf_Splay_tree_struct*_T3;void*_T4;void*_T5;struct Cyc_Splay_noderef*_T6;struct Cyc_Splay_Node_Splay_tree_struct*_T7;struct Cyc_Splay_noderef*_T8;struct Cyc_Splay_node*_T9;struct Cyc_Splay_node*_TA;void*_TB;struct Cyc_Splay_node*_TC;void*_TD;void*(*_TE)(void*,void*);void*_TF;struct Cyc_Splay_node*_T10;void*_T11;void*(*_T12)(void*,void*);void*_T13;struct Cyc_Splay_node*_T14;void*_T15;void*_T16;struct Cyc_Splay_noderef*_T17;_T0=t;_T1=(int*)_T0;_T2=*_T1;if(_T2!=0)goto _TL30;{struct Cyc_Splay_Leaf_Splay_tree_struct*_T18=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_T18->tag=0;
# 273
_T18->f1=0;_T3=(struct Cyc_Splay_Leaf_Splay_tree_struct*)_T18;}_T4=(void*)_T3;return _T4;_TL30: _T5=t;{struct Cyc_Splay_Node_Splay_tree_struct*_T18=(struct Cyc_Splay_Node_Splay_tree_struct*)_T5;_T17=_T18->f1;}{struct Cyc_Splay_noderef*nr=_T17;_T6=nr;{
# 275
struct Cyc_Splay_node*n=_T6->v;{struct Cyc_Splay_Node_Splay_tree_struct*_T18=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_T18->tag=1;{struct Cyc_Splay_noderef*_T19=_cycalloc(sizeof(struct Cyc_Splay_noderef));{struct Cyc_Splay_node*_T1A=_cycalloc(sizeof(struct Cyc_Splay_node));_TA=n;
_T1A->key=_TA->key;_TB=env;_TC=n;_TD=_TC->data;_T1A->data=f(_TB,_TD);_TE=f;_TF=env;_T10=n;_T11=_T10->left;
_T1A->left=Cyc_SlowDict_map_tree_c(_TE,_TF,_T11);_T12=f;_T13=env;_T14=n;_T15=_T14->right;
_T1A->right=Cyc_SlowDict_map_tree_c(_T12,_T13,_T15);_T9=(struct Cyc_Splay_node*)_T1A;}
# 276
_T19->v=_T9;_T8=(struct Cyc_Splay_noderef*)_T19;}_T18->f1=_T8;_T7=(struct Cyc_Splay_Node_Splay_tree_struct*)_T18;}_T16=(void*)_T7;return _T16;}};}
# 281
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d){struct Cyc_SlowDict_Dict*_T0;struct Cyc_SlowDict_Dict*_T1;void*(*_T2)(void*,void*);void*_T3;struct Cyc_SlowDict_Dict*_T4;void*_T5;{struct Cyc_SlowDict_Dict*_T6=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_T1=d;
_T6->reln=_T1->reln;_T2=f;_T3=env;_T4=d;_T5=_T4->tree;_T6->tree=Cyc_SlowDict_map_tree_c(_T2,_T3,_T5);_T0=(struct Cyc_SlowDict_Dict*)_T6;}return _T0;}
# 285
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d){struct Cyc_SlowDict_Dict*_T0;int*_T1;int _T2;struct Cyc_SlowDict_Absent_exn_struct*_T3;struct Cyc_SlowDict_Absent_exn_struct*_T4;struct _tuple0*_T5;struct Cyc_Splay_noderef*_T6;struct Cyc_Splay_node*_T7;struct Cyc_Splay_noderef*_T8;struct Cyc_Splay_node*_T9;_T0=d;{
void*_TA=_T0->tree;struct Cyc_Splay_noderef*_TB;_T1=(int*)_TA;_T2=*_T1;if(_T2!=0)goto _TL32;_T3=& Cyc_SlowDict_Absent_val;_T4=(struct Cyc_SlowDict_Absent_exn_struct*)_T3;_throw(_T4);goto _TL33;_TL32:{struct Cyc_Splay_Node_Splay_tree_struct*_TC=(struct Cyc_Splay_Node_Splay_tree_struct*)_TA;_TB=_TC->f1;}{struct Cyc_Splay_noderef*nr=_TB;{struct _tuple0*_TC=_cycalloc(sizeof(struct _tuple0));_T6=nr;_T7=_T6->v;
# 288
_TC->f0=_T7->key;_T8=nr;_T9=_T8->v;_TC->f1=_T9->data;_T5=(struct _tuple0*)_TC;}return _T5;}_TL33:;}}
# 292
struct Cyc_List_List*Cyc_SlowDict_to_list_f(void*k,void*v,struct Cyc_List_List*accum){struct Cyc_List_List*_T0;struct _tuple0*_T1;{struct Cyc_List_List*_T2=_cycalloc(sizeof(struct Cyc_List_List));{struct _tuple0*_T3=_cycalloc(sizeof(struct _tuple0));
_T3->f0=k;_T3->f1=v;_T1=(struct _tuple0*)_T3;}_T2->hd=_T1;_T2->tl=accum;_T0=(struct Cyc_List_List*)_T2;}return _T0;}
# 296
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d){struct Cyc_List_List*(*_T0)(struct Cyc_List_List*(*)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*,struct Cyc_List_List*);struct Cyc_SlowDict_Dict*_T1;struct Cyc_List_List*_T2;{
struct Cyc_List_List*(*_T3)(struct Cyc_List_List*(*)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*,struct Cyc_List_List*)=(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*,struct Cyc_List_List*))Cyc_SlowDict_fold;_T0=_T3;}_T1=d;_T2=_T0(Cyc_SlowDict_to_list_f,_T1,0);return _T2;}
