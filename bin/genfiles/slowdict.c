#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator  The corresponding definitions are in file lib/runtime_*.c */
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
/* should be size_t, but int is fine. */
#define offsetof(t,n) ((int)(&(((t *)0)->n)))
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
void   _free_region(struct _RegionHandle*);
struct _RegionHandle*_open_dynregion(struct _DynRegionFrame*,struct _DynRegionHandle*);
void   _pop_dynregion();

/* Exceptions */
struct _handler_cons {
  struct _RuntimeStack s;
  jmp_buf handler;
};
void _push_handler(struct _handler_cons *);
void _push_region(struct _RegionHandle *);
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
#define _zero_arr_plus_fn(orig_x,orig_sz,orig_i,f,l) ((orig_x)+(orig_i))
#define _zero_arr_plus_char_fn _zero_arr_plus_fn
#define _zero_arr_plus_short_fn _zero_arr_plus_fn
#define _zero_arr_plus_int_fn _zero_arr_plus_fn
#define _zero_arr_plus_float_fn _zero_arr_plus_fn
#define _zero_arr_plus_double_fn _zero_arr_plus_fn
#define _zero_arr_plus_longdouble_fn _zero_arr_plus_fn
#define _zero_arr_plus_voidstar_fn _zero_arr_plus_fn
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
short* _zero_arr_plus_short_fn(short*,unsigned,int,const char*,unsigned);
int* _zero_arr_plus_int_fn(int*,unsigned,int,const char*,unsigned);
float* _zero_arr_plus_float_fn(float*,unsigned,int,const char*,unsigned);
double* _zero_arr_plus_double_fn(double*,unsigned,int,const char*,unsigned);
long double* _zero_arr_plus_longdouble_fn(long double*,unsigned,int,const char*, unsigned);
void** _zero_arr_plus_voidstar_fn(void**,unsigned,int,const char*,unsigned);
#endif

/* _get_zero_arr_size_*(x,sz) returns the number of elements in a
   zero-terminated array that is NULL or has at least sz elements */
int _get_zero_arr_size_char(const char*,unsigned);
int _get_zero_arr_size_short(const short*,unsigned);
int _get_zero_arr_size_int(const int*,unsigned);
int _get_zero_arr_size_float(const float*,unsigned);
int _get_zero_arr_size_double(const double*,unsigned);
int _get_zero_arr_size_longdouble(const long double*,unsigned);
int _get_zero_arr_size_voidstar(const void**,unsigned);

/* _zero_arr_inplace_plus_*_fn(x,i,filename,lineno) sets
   zero-terminated pointer *x to *x + i */
char* _zero_arr_inplace_plus_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_short_fn(short**,int,const char*,unsigned);
int* _zero_arr_inplace_plus_int(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_longdouble_fn(long double**,int,const char*,unsigned);
void** _zero_arr_inplace_plus_voidstar_fn(void***,int,const char*,unsigned);
/* like the previous functions, but does post-addition (as in e++) */
char* _zero_arr_inplace_plus_post_char_fn(char**,int,const char*,unsigned);
short* _zero_arr_inplace_plus_post_short_fn(short**x,int,const char*,unsigned);
int* _zero_arr_inplace_plus_post_int_fn(int**,int,const char*,unsigned);
float* _zero_arr_inplace_plus_post_float_fn(float**,int,const char*,unsigned);
double* _zero_arr_inplace_plus_post_double_fn(double**,int,const char*,unsigned);
long double* _zero_arr_inplace_plus_post_longdouble_fn(long double**,int,const char *,unsigned);
void** _zero_arr_inplace_plus_post_voidstar_fn(void***,int,const char*,unsigned);
#define _zero_arr_plus_char(x,s,i) \
  (_zero_arr_plus_char_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_short(x,s,i) \
  (_zero_arr_plus_short_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_int(x,s,i) \
  (_zero_arr_plus_int_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_float(x,s,i) \
  (_zero_arr_plus_float_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_double(x,s,i) \
  (_zero_arr_plus_double_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_longdouble(x,s,i) \
  (_zero_arr_plus_longdouble_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_plus_voidstar(x,s,i) \
  (_zero_arr_plus_voidstar_fn(x,s,i,__FILE__,__LINE__))
#define _zero_arr_inplace_plus_char(x,i) \
  _zero_arr_inplace_plus_char_fn((char **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_short(x,i) \
  _zero_arr_inplace_plus_short_fn((short **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_int(x,i) \
  _zero_arr_inplace_plus_int_fn((int **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_float(x,i) \
  _zero_arr_inplace_plus_float_fn((float **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_double(x,i) \
  _zero_arr_inplace_plus_double_fn((double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_longdouble(x,i) \
  _zero_arr_inplace_plus_longdouble_fn((long double **)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_voidstar(x,i) \
  _zero_arr_inplace_plus_voidstar_fn((void ***)(x),i,__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_char(x,i) \
  _zero_arr_inplace_plus_post_char_fn((char **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_short(x,i) \
  _zero_arr_inplace_plus_post_short_fn((short **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_int(x,i) \
  _zero_arr_inplace_plus_post_int_fn((int **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_float(x,i) \
  _zero_arr_inplace_plus_post_float_fn((float **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_double(x,i) \
  _zero_arr_inplace_plus_post_double_fn((double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_longdouble(x,i) \
  _zero_arr_inplace_plus_post_longdouble_fn((long double **)(x),(i),__FILE__,__LINE__)
#define _zero_arr_inplace_plus_post_voidstar(x,i) \
  _zero_arr_inplace_plus_post_voidstar_fn((void***)(x),(i),__FILE__,__LINE__)

#ifdef NO_CYC_BOUNDS_CHECKS
#define _check_fat_subscript(arr,elt_sz,index) ((arr).curr + (elt_sz) * (index))
#define _untag_fat_ptr(arr,elt_sz,num_elts) ((arr).curr)
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
      _curr != (unsigned char *)0) \
    _throw_arraybounds(); \
  _curr; })
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
void cyc_vfree(void *);
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

static inline void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
  if (r > (struct _RegionHandle *)_CYC_MAX_REGION_CONST && r->curr != 0) { 
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};struct Cyc_Splay_Leaf_Splay_tree_struct{int tag;int f1;};struct Cyc_Splay_Node_Splay_tree_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*left;void*right;};
# 49 "splay.h"
extern int Cyc_Splay_splay(int(*)(void*,void*),void*,void*);struct Cyc_SlowDict_Dict;extern char Cyc_SlowDict_Present[8U];struct Cyc_SlowDict_Present_exn_struct{char*tag;};extern char Cyc_SlowDict_Absent[7U];struct Cyc_SlowDict_Absent_exn_struct{char*tag;};
# 64 "slowdict.h"
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*,void*,void*);
# 89
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*,void*);
# 98
void*Cyc_SlowDict_fold(void*(*)(void*,void*,void*),struct Cyc_SlowDict_Dict*,void*);char Cyc_SlowDict_Absent[7U]="Absent";char Cyc_SlowDict_Present[8U]="Present";
# 32 "slowdict.cyc"
struct Cyc_SlowDict_Absent_exn_struct Cyc_SlowDict_Absent_val={Cyc_SlowDict_Absent};
struct Cyc_SlowDict_Present_exn_struct Cyc_SlowDict_Present_val={Cyc_SlowDict_Present};struct Cyc_SlowDict_Dict{int(*reln)(void*,void*);void*tree;};
# 41
struct Cyc_SlowDict_Dict*Cyc_SlowDict_empty(int(*comp)(void*,void*)){
void*t=(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_Tmp0->tag=0,_Tmp0->f1=0;_Tmp0;});
return({struct Cyc_SlowDict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_Tmp0->reln=comp,_Tmp0->tree=t;_Tmp0;});}
# 49
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d){
void*_stmttmp0=d->tree;void*_Tmp0=_stmttmp0;if(*((int*)_Tmp0)== 0)
return 1;else{
return 0;};}
# 56
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*key){
return Cyc_Splay_splay(d->reln,key,d->tree);}
# 60
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*key,void*data){
void*leaf=(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_Tmp0->tag=0,_Tmp0->f1=0;_Tmp0;});
void*newleft=leaf;void*newright=leaf;
if(Cyc_Splay_splay(d->reln,key,d->tree)){
# 65
void*_stmttmp1=d->tree;void*_Tmp0=_stmttmp1;void*_Tmp1;if(*((int*)_Tmp0)== 1){_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*n=_Tmp1;
# 67
newleft=(n->v)->left;
newright=(n->v)->right;
goto _LL0;}}else{
# 71
goto _LL0;}_LL0:;}else{
# 75
void*_stmttmp2=d->tree;void*_Tmp0=_stmttmp2;void*_Tmp1;if(*((int*)_Tmp0)== 1){_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 77
struct Cyc_Splay_node*n=nr->v;
if(((d->reln))(key,n->key)< 0){
newleft=n->left;
newright=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_Tmp2->tag=1,({struct Cyc_Splay_noderef*_Tmp3=({struct Cyc_Splay_noderef*_Tmp4=_cycalloc(sizeof(struct Cyc_Splay_noderef));({struct Cyc_Splay_node*_Tmp5=({struct Cyc_Splay_node*_Tmp6=_cycalloc(sizeof(struct Cyc_Splay_node));
_Tmp6->key=n->key,_Tmp6->data=n->data,_Tmp6->left=leaf,_Tmp6->right=n->right;_Tmp6;});
# 80
_Tmp4->v=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});}else{
# 84
newleft=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_Tmp2->tag=1,({struct Cyc_Splay_noderef*_Tmp3=({struct Cyc_Splay_noderef*_Tmp4=_cycalloc(sizeof(struct Cyc_Splay_noderef));({struct Cyc_Splay_node*_Tmp5=({struct Cyc_Splay_node*_Tmp6=_cycalloc(sizeof(struct Cyc_Splay_node));
_Tmp6->key=n->key,_Tmp6->data=n->data,_Tmp6->left=n->left,_Tmp6->right=leaf;_Tmp6;});
# 84
_Tmp4->v=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});
# 86
newright=n->right;}
# 88
goto _LL5;}}else{
# 90
goto _LL5;}_LL5:;}
# 93
return({struct Cyc_SlowDict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_Tmp0->reln=d->reln,({void*_Tmp1=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_Tmp2->tag=1,({struct Cyc_Splay_noderef*_Tmp3=({struct Cyc_Splay_noderef*_Tmp4=_cycalloc(sizeof(struct Cyc_Splay_noderef));({
struct Cyc_Splay_node*_Tmp5=({struct Cyc_Splay_node*_Tmp6=_cycalloc(sizeof(struct Cyc_Splay_node));_Tmp6->key=key,_Tmp6->data=data,_Tmp6->left=newleft,_Tmp6->right=newright;_Tmp6;});_Tmp4->v=_Tmp5;});_Tmp4;});
# 93
_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->tree=_Tmp1;});_Tmp0;});}
# 98
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert_new(struct Cyc_SlowDict_Dict*d,void*key,void*data){
# 100
if(Cyc_Splay_splay(d->reln,key,d->tree))
(int)_throw((void*)& Cyc_SlowDict_Present_val);
return Cyc_SlowDict_insert(d,key,data);}struct _tuple0{void*f1;void*f2;};
# 105
struct Cyc_SlowDict_Dict*Cyc_SlowDict_inserts(struct Cyc_SlowDict_Dict*d,struct Cyc_List_List*kds){
# 107
for(1;kds != 0;kds=kds->tl){
d=Cyc_SlowDict_insert(d,(*((struct _tuple0*)kds->hd)).f1,(*((struct _tuple0*)kds->hd)).f2);}
return d;}
# 112
struct Cyc_SlowDict_Dict*Cyc_SlowDict_singleton(int(*comp)(void*,void*),void*key,void*data){
struct Cyc_Splay_Leaf_Splay_tree_struct*leaf=({struct Cyc_Splay_Leaf_Splay_tree_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_Tmp0->tag=0,_Tmp0->f1=0;_Tmp0;});
return({struct Cyc_SlowDict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_Tmp0->reln=comp,({void*_Tmp1=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_Tmp2->tag=1,({struct Cyc_Splay_noderef*_Tmp3=({struct Cyc_Splay_noderef*_Tmp4=_cycalloc(sizeof(struct Cyc_Splay_noderef));({struct Cyc_Splay_node*_Tmp5=({struct Cyc_Splay_node*_Tmp6=_cycalloc(sizeof(struct Cyc_Splay_node));_Tmp6->key=key,_Tmp6->data=data,_Tmp6->left=(void*)leaf,_Tmp6->right=(void*)leaf;_Tmp6;});_Tmp4->v=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});_Tmp0->tree=_Tmp1;});_Tmp0;});}
# 117
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_stmttmp3=d->tree;void*_Tmp0=_stmttmp3;void*_Tmp1;if(*((int*)_Tmp0)== 1){_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
return(nr->v)->data;}}else{
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_Tmp2->tag=Cyc_Core_Impossible,_Tmp2->f1=({const char*_Tmp3="Dict::lookup";_tag_fat(_Tmp3,sizeof(char),13U);});_Tmp2;}));};}
# 123
(int)_throw((void*)& Cyc_SlowDict_Absent_val);}
# 126
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_stmttmp4=d->tree;void*_Tmp0=_stmttmp4;void*_Tmp1;if(*((int*)_Tmp0)== 1){_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
return({struct Cyc_Core_Opt*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Opt));_Tmp2->v=(nr->v)->data;_Tmp2;});}}else{
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_Tmp2->tag=Cyc_Core_Impossible,_Tmp2->f1=({const char*_Tmp3="Dict::lookup";_tag_fat(_Tmp3,sizeof(char),13U);});_Tmp2;}));};}
# 132
return 0;}
# 135
static int Cyc_SlowDict_get_largest(void*x,void*y){return 1;}
# 137
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_stmttmp5=d->tree;void*_Tmp0=_stmttmp5;void*_Tmp1;if(*((int*)_Tmp0)== 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Core_Impossible_exn_struct));_Tmp2->tag=Cyc_Core_Impossible,_Tmp2->f1=({const char*_Tmp3="Dict::lookup";_tag_fat(_Tmp3,sizeof(char),13U);});_Tmp2;}));else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 142
struct Cyc_Splay_node*n=nr->v;
void*_stmttmp6=n->left;void*_Tmp2=_stmttmp6;void*_Tmp3;if(*((int*)_Tmp2)== 0)
return({struct Cyc_SlowDict_Dict*_Tmp4=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_Tmp4->reln=d->reln,_Tmp4->tree=n->right;_Tmp4;});else{_Tmp3=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp2)->f1;{struct Cyc_Splay_noderef*noderef_left=_Tmp3;
# 146
void*_stmttmp7=n->right;void*_Tmp4=_stmttmp7;void*_Tmp5;if(*((int*)_Tmp4)== 0)
return({struct Cyc_SlowDict_Dict*_Tmp6=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_Tmp6->reln=d->reln,_Tmp6->tree=n->left;_Tmp6;});else{_Tmp5=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp4)->f1;{struct Cyc_Splay_noderef*node_ref_right=_Tmp5;
# 149
Cyc_Splay_splay(Cyc_SlowDict_get_largest,key,n->left);{
struct Cyc_Splay_node*newtop=noderef_left->v;
return({struct Cyc_SlowDict_Dict*_Tmp6=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_Tmp6->reln=d->reln,({
void*_Tmp7=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_Tmp8=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_Tmp8->tag=1,({struct Cyc_Splay_noderef*_Tmp9=({struct Cyc_Splay_noderef*_TmpA=_cycalloc(sizeof(struct Cyc_Splay_noderef));({struct Cyc_Splay_node*_TmpB=({struct Cyc_Splay_node*_TmpC=_cycalloc(sizeof(struct Cyc_Splay_node));_TmpC->key=newtop->key,_TmpC->data=newtop->data,_TmpC->left=newtop->left,_TmpC->right=n->right;_TmpC;});_TmpA->v=_TmpB;});_TmpA;});_Tmp8->f1=_Tmp9;});_Tmp8;});_Tmp6->tree=_Tmp7;});_Tmp6;});}}};}};}};}else{
# 158
return d;}}
# 161
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*key){
struct Cyc_SlowDict_Dict*d2=Cyc_SlowDict_delete(d,key);
if(d == d2)
(int)_throw((void*)& Cyc_SlowDict_Absent_val);
return d2;}
# 168
static void*Cyc_SlowDict_fold_tree(void*(*f)(void*,void*,void*),void*t,void*accum){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 0)
# 171
return accum;else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 173
struct Cyc_Splay_node*n=nr->v;
return({void*(*_Tmp2)(void*,void*,void*)=f;void*_Tmp3=n->key;void*_Tmp4=n->data;_Tmp2(_Tmp3,_Tmp4,({void*(*_Tmp5)(void*,void*,void*)=f;void*_Tmp6=n->left;Cyc_SlowDict_fold_tree(_Tmp5,_Tmp6,
Cyc_SlowDict_fold_tree(f,n->right,accum));}));});}};}
# 178
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum){
return Cyc_SlowDict_fold_tree(f,d->tree,accum);}
# 182
static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,void*t,void*accum){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 0)
# 185
return accum;else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 187
struct Cyc_Splay_node*n=nr->v;
return({void*(*_Tmp2)(void*,void*,void*,void*)=f;void*_Tmp3=env;void*_Tmp4=n->key;void*_Tmp5=n->data;_Tmp2(_Tmp3,_Tmp4,_Tmp5,({
void*(*_Tmp6)(void*,void*,void*,void*)=f;void*_Tmp7=env;void*_Tmp8=n->left;Cyc_SlowDict_fold_tree_c(_Tmp6,_Tmp7,_Tmp8,Cyc_SlowDict_fold_tree_c(f,env,n->right,accum));}));});}};}
# 192
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*dict,void*accum){
return Cyc_SlowDict_fold_tree_c(f,env,dict->tree,accum);}
# 196
static void Cyc_SlowDict_app_tree(void*(*f)(void*,void*),void*t){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 0)
goto _LL0;else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 200
struct Cyc_Splay_node*n=nr->v;
Cyc_SlowDict_app_tree(f,n->left);
f(n->key,n->data);
Cyc_SlowDict_app_tree(f,n->right);
goto _LL0;}}_LL0:;}
# 207
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree(f,d->tree);}
# 211
static void Cyc_SlowDict_iter_tree(void(*f)(void*,void*),void*t){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 0)
goto _LL0;else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 215
struct Cyc_Splay_node*n=nr->v;
Cyc_SlowDict_iter_tree(f,n->left);
f(n->key,n->data);
Cyc_SlowDict_iter_tree(f,n->right);
goto _LL0;}}_LL0:;}
# 222
void Cyc_SlowDict_iter(void(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree(f,d->tree);}
# 226
static void Cyc_SlowDict_app_tree_c(void*(*f)(void*,void*,void*),void*env,void*t){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 0)
goto _LL0;else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 230
struct Cyc_Splay_node*n=nr->v;
Cyc_SlowDict_app_tree_c(f,env,n->left);
f(env,n->key,n->data);
Cyc_SlowDict_app_tree_c(f,env,n->right);
goto _LL0;}}_LL0:;}
# 237
void Cyc_SlowDict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree_c(f,env,d->tree);}
# 241
static void Cyc_SlowDict_iter_tree_c(void(*f)(void*,void*,void*),void*env,void*t){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 0)
goto _LL0;else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 245
struct Cyc_Splay_node*n=nr->v;
Cyc_SlowDict_iter_tree_c(f,env,n->left);
f(env,n->key,n->data);
Cyc_SlowDict_iter_tree_c(f,env,n->right);
goto _LL0;}}_LL0:;}
# 252
void Cyc_SlowDict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_iter_tree_c(f,env,d->tree);}
# 256
static void*Cyc_SlowDict_map_tree(void*(*f)(void*),void*t){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 0)
return(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_Tmp2->tag=0,_Tmp2->f1=0;_Tmp2;});else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 260
struct Cyc_Splay_node*n=nr->v;
return(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_Tmp2->tag=1,({struct Cyc_Splay_noderef*_Tmp3=({struct Cyc_Splay_noderef*_Tmp4=_cycalloc(sizeof(struct Cyc_Splay_noderef));({struct Cyc_Splay_node*_Tmp5=({struct Cyc_Splay_node*_Tmp6=_cycalloc(sizeof(struct Cyc_Splay_node));_Tmp6->key=n->key,({
void*_Tmp7=f(n->data);_Tmp6->data=_Tmp7;}),({
void*_Tmp7=Cyc_SlowDict_map_tree(f,n->left);_Tmp6->left=_Tmp7;}),({
void*_Tmp7=Cyc_SlowDict_map_tree(f,n->right);_Tmp6->right=_Tmp7;});_Tmp6;});
# 261
_Tmp4->v=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});}};}
# 267
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d){
return({struct Cyc_SlowDict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_Tmp0->reln=d->reln,({void*_Tmp1=Cyc_SlowDict_map_tree(f,d->tree);_Tmp0->tree=_Tmp1;});_Tmp0;});}
# 271
static void*Cyc_SlowDict_map_tree_c(void*(*f)(void*,void*),void*env,void*t){
void*_Tmp0=t;void*_Tmp1;if(*((int*)_Tmp0)== 0)
return(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Splay_Leaf_Splay_tree_struct));_Tmp2->tag=0,_Tmp2->f1=0;_Tmp2;});else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
# 275
struct Cyc_Splay_node*n=nr->v;
return(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_Tmp2=_cycalloc(sizeof(struct Cyc_Splay_Node_Splay_tree_struct));_Tmp2->tag=1,({struct Cyc_Splay_noderef*_Tmp3=({struct Cyc_Splay_noderef*_Tmp4=_cycalloc(sizeof(struct Cyc_Splay_noderef));({struct Cyc_Splay_node*_Tmp5=({struct Cyc_Splay_node*_Tmp6=_cycalloc(sizeof(struct Cyc_Splay_node));_Tmp6->key=n->key,({void*_Tmp7=f(env,n->data);_Tmp6->data=_Tmp7;}),({
void*_Tmp7=Cyc_SlowDict_map_tree_c(f,env,n->left);_Tmp6->left=_Tmp7;}),({
void*_Tmp7=Cyc_SlowDict_map_tree_c(f,env,n->right);_Tmp6->right=_Tmp7;});_Tmp6;});
# 276
_Tmp4->v=_Tmp5;});_Tmp4;});_Tmp2->f1=_Tmp3;});_Tmp2;});}};}
# 281
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
return({struct Cyc_SlowDict_Dict*_Tmp0=_cycalloc(sizeof(struct Cyc_SlowDict_Dict));_Tmp0->reln=d->reln,({void*_Tmp1=Cyc_SlowDict_map_tree_c(f,env,d->tree);_Tmp0->tree=_Tmp1;});_Tmp0;});}
# 285
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d){
void*_stmttmp8=d->tree;void*_Tmp0=_stmttmp8;void*_Tmp1;if(*((int*)_Tmp0)== 0)
(int)_throw((void*)& Cyc_SlowDict_Absent_val);else{_Tmp1=((struct Cyc_Splay_Node_Splay_tree_struct*)_Tmp0)->f1;{struct Cyc_Splay_noderef*nr=_Tmp1;
return({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));_Tmp2->f1=(nr->v)->key,_Tmp2->f2=(nr->v)->data;_Tmp2;});}};}
# 292
struct Cyc_List_List*Cyc_SlowDict_to_list_f(void*k,void*v,struct Cyc_List_List*accum){
return({struct Cyc_List_List*_Tmp0=_cycalloc(sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_cycalloc(sizeof(struct _tuple0));_Tmp2->f1=k,_Tmp2->f2=v;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=accum;_Tmp0;});}
# 296
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d){
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*,struct Cyc_List_List*))Cyc_SlowDict_fold;})(Cyc_SlowDict_to_list_f,d,0);}
