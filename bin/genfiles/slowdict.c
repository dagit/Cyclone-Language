#include <setjmp.h>
/* This is a C header file to be used by the output of the Cyclone to
   C translator.  The corresponding definitions are in file lib/runtime_*.c */
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

/* Discriminated Unions */
struct _xtunion_struct { char *tag; };

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

struct _xtunion_struct* Cyc_Core_get_exn_thrown();
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
#ifdef CYC_ANSI_OUTPUT
#define _INLINE  
#else
#define _INLINE inline
#endif

#ifdef NO_CYC_NULL_CHECKS
#define _check_null(ptr) (ptr)
#else
#define _check_null(ptr) \
  ({ void*_cks_null = (void*)(ptr); \
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

/* Allocation */
void* GC_malloc(int);
void* GC_malloc_atomic(int);
void* GC_calloc(unsigned,unsigned);
void* GC_calloc_atomic(unsigned,unsigned);
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

static _INLINE unsigned int _check_times(unsigned x, unsigned y) {
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

static _INLINE void *_fast_region_malloc(struct _RegionHandle *r, unsigned orig_s) {  
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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 171 "core.h"
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Splay_node;struct Cyc_Splay_noderef{struct Cyc_Splay_node*v;};struct Cyc_Splay_Leaf_Splay_tree_struct{int tag;int f1;};struct Cyc_Splay_Node_Splay_tree_struct{int tag;struct Cyc_Splay_noderef*f1;};struct Cyc_Splay_node{void*key;void*data;void*left;void*right;};
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
void*t=(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp1=_cycalloc(sizeof(*_tmp1));_tmp1->tag=0,_tmp1->f1=0;_tmp1;});
return({struct Cyc_SlowDict_Dict*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->reln=comp,_tmp0->tree=t;_tmp0;});}
# 49
int Cyc_SlowDict_is_empty(struct Cyc_SlowDict_Dict*d){
void*_tmp2=d->tree;void*_stmttmp0=_tmp2;void*_tmp3=_stmttmp0;if(*((int*)_tmp3)== 0)
return 1;else{
return 0;};}
# 56
int Cyc_SlowDict_member(struct Cyc_SlowDict_Dict*d,void*key){
return Cyc_Splay_splay(d->reln,key,d->tree);}
# 60
struct Cyc_SlowDict_Dict*Cyc_SlowDict_insert(struct Cyc_SlowDict_Dict*d,void*key,void*data){
void*leaf=(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->tag=0,_tmp15->f1=0;_tmp15;});
void*newleft=leaf;void*newright=leaf;
if(Cyc_Splay_splay(d->reln,key,d->tree)){
# 65
void*_tmp4=d->tree;void*_stmttmp1=_tmp4;void*_tmp5=_stmttmp1;struct Cyc_Splay_noderef*_tmp6;if(*((int*)_tmp5)== 1){_tmp6=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp5)->f1;{struct Cyc_Splay_noderef*n=_tmp6;
# 67
newleft=(n->v)->left;
newright=(n->v)->right;
goto _LL0;}}else{
# 71
goto _LL0;}_LL0:;}else{
# 75
void*_tmp7=d->tree;void*_stmttmp2=_tmp7;void*_tmp8=_stmttmp2;struct Cyc_Splay_noderef*_tmp9;if(*((int*)_tmp8)== 1){_tmp9=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp8)->f1;{struct Cyc_Splay_noderef*nr=_tmp9;
# 77
struct Cyc_Splay_node*_tmpA=nr->v;struct Cyc_Splay_node*n=_tmpA;
if(((d->reln))(key,n->key)< 0){
newleft=n->left;
newright=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmpD=_cycalloc(sizeof(*_tmpD));_tmpD->tag=1,({struct Cyc_Splay_noderef*_tmp5C=({struct Cyc_Splay_noderef*_tmpC=_cycalloc(sizeof(*_tmpC));({struct Cyc_Splay_node*_tmp5B=({struct Cyc_Splay_node*_tmpB=_cycalloc(sizeof(*_tmpB));
_tmpB->key=n->key,_tmpB->data=n->data,_tmpB->left=leaf,_tmpB->right=n->right;_tmpB;});
# 80
_tmpC->v=_tmp5B;});_tmpC;});_tmpD->f1=_tmp5C;});_tmpD;});}else{
# 84
newleft=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->tag=1,({struct Cyc_Splay_noderef*_tmp5E=({struct Cyc_Splay_noderef*_tmpF=_cycalloc(sizeof(*_tmpF));({struct Cyc_Splay_node*_tmp5D=({struct Cyc_Splay_node*_tmpE=_cycalloc(sizeof(*_tmpE));
_tmpE->key=n->key,_tmpE->data=n->data,_tmpE->left=n->left,_tmpE->right=leaf;_tmpE;});
# 84
_tmpF->v=_tmp5D;});_tmpF;});_tmp10->f1=_tmp5E;});_tmp10;});
# 86
newright=n->right;}
# 88
goto _LL5;}}else{
# 90
goto _LL5;}_LL5:;}
# 93
return({struct Cyc_SlowDict_Dict*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->reln=d->reln,({void*_tmp61=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->tag=1,({struct Cyc_Splay_noderef*_tmp60=({struct Cyc_Splay_noderef*_tmp12=_cycalloc(sizeof(*_tmp12));({
struct Cyc_Splay_node*_tmp5F=({struct Cyc_Splay_node*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->key=key,_tmp11->data=data,_tmp11->left=newleft,_tmp11->right=newright;_tmp11;});_tmp12->v=_tmp5F;});_tmp12;});
# 93
_tmp13->f1=_tmp60;});_tmp13;});_tmp14->tree=_tmp61;});_tmp14;});}
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
struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp16=({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp1B=_cycalloc(sizeof(*_tmp1B));_tmp1B->tag=0,_tmp1B->f1=0;_tmp1B;});struct Cyc_Splay_Leaf_Splay_tree_struct*leaf=_tmp16;
return({struct Cyc_SlowDict_Dict*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->reln=comp,({void*_tmp64=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->tag=1,({struct Cyc_Splay_noderef*_tmp63=({struct Cyc_Splay_noderef*_tmp18=_cycalloc(sizeof(*_tmp18));({struct Cyc_Splay_node*_tmp62=({struct Cyc_Splay_node*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->key=key,_tmp17->data=data,_tmp17->left=(void*)leaf,_tmp17->right=(void*)leaf;_tmp17;});_tmp18->v=_tmp62;});_tmp18;});_tmp19->f1=_tmp63;});_tmp19;});_tmp1A->tree=_tmp64;});_tmp1A;});}
# 117
void*Cyc_SlowDict_lookup(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp1C=d->tree;void*_stmttmp3=_tmp1C;void*_tmp1D=_stmttmp3;struct Cyc_Splay_noderef*_tmp1E;if(*((int*)_tmp1D)== 1){_tmp1E=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp1D)->f1;{struct Cyc_Splay_noderef*nr=_tmp1E;
return(nr->v)->data;}}else{
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp20=_cycalloc(sizeof(*_tmp20));_tmp20->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp65=({const char*_tmp1F="Dict::lookup";_tag_fat(_tmp1F,sizeof(char),13U);});_tmp20->f1=_tmp65;});_tmp20;}));};}
# 123
(int)_throw((void*)& Cyc_SlowDict_Absent_val);}
# 126
struct Cyc_Core_Opt*Cyc_SlowDict_lookup_opt(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp21=d->tree;void*_stmttmp4=_tmp21;void*_tmp22=_stmttmp4;struct Cyc_Splay_noderef*_tmp23;if(*((int*)_tmp22)== 1){_tmp23=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp22)->f1;{struct Cyc_Splay_noderef*nr=_tmp23;
return({struct Cyc_Core_Opt*_tmp24=_cycalloc(sizeof(*_tmp24));_tmp24->v=(nr->v)->data;_tmp24;});}}else{
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp66=({const char*_tmp25="Dict::lookup";_tag_fat(_tmp25,sizeof(char),13U);});_tmp26->f1=_tmp66;});_tmp26;}));};}
# 132
return 0;}
# 135
static int Cyc_SlowDict_get_largest(void*x,void*y){return 1;}
# 137
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete(struct Cyc_SlowDict_Dict*d,void*key){
if(Cyc_Splay_splay(d->reln,key,d->tree)){
void*_tmp27=d->tree;void*_stmttmp5=_tmp27;void*_tmp28=_stmttmp5;struct Cyc_Splay_noderef*_tmp29;if(*((int*)_tmp28)== 0)
(int)_throw((void*)({struct Cyc_Core_Impossible_exn_struct*_tmp2B=_cycalloc(sizeof(*_tmp2B));_tmp2B->tag=Cyc_Core_Impossible,({struct _fat_ptr _tmp67=({const char*_tmp2A="Dict::lookup";_tag_fat(_tmp2A,sizeof(char),13U);});_tmp2B->f1=_tmp67;});_tmp2B;}));else{_tmp29=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp28)->f1;{struct Cyc_Splay_noderef*nr=_tmp29;
# 142
struct Cyc_Splay_node*n=nr->v;
void*_tmp2C=n->left;void*_stmttmp6=_tmp2C;void*_tmp2D=_stmttmp6;struct Cyc_Splay_noderef*_tmp2E;if(*((int*)_tmp2D)== 0)
return({struct Cyc_SlowDict_Dict*_tmp2F=_cycalloc(sizeof(*_tmp2F));_tmp2F->reln=d->reln,_tmp2F->tree=n->right;_tmp2F;});else{_tmp2E=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp2D)->f1;{struct Cyc_Splay_noderef*noderef_left=_tmp2E;
# 146
void*_tmp30=n->right;void*_stmttmp7=_tmp30;void*_tmp31=_stmttmp7;struct Cyc_Splay_noderef*_tmp32;if(*((int*)_tmp31)== 0)
return({struct Cyc_SlowDict_Dict*_tmp33=_cycalloc(sizeof(*_tmp33));_tmp33->reln=d->reln,_tmp33->tree=n->left;_tmp33;});else{_tmp32=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp31)->f1;{struct Cyc_Splay_noderef*node_ref_right=_tmp32;
# 149
Cyc_Splay_splay(Cyc_SlowDict_get_largest,key,n->left);{
struct Cyc_Splay_node*newtop=noderef_left->v;
return({struct Cyc_SlowDict_Dict*_tmp37=_cycalloc(sizeof(*_tmp37));_tmp37->reln=d->reln,({
void*_tmp6A=(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp36=_cycalloc(sizeof(*_tmp36));_tmp36->tag=1,({struct Cyc_Splay_noderef*_tmp69=({struct Cyc_Splay_noderef*_tmp35=_cycalloc(sizeof(*_tmp35));({struct Cyc_Splay_node*_tmp68=({struct Cyc_Splay_node*_tmp34=_cycalloc(sizeof(*_tmp34));_tmp34->key=newtop->key,_tmp34->data=newtop->data,_tmp34->left=newtop->left,_tmp34->right=n->right;_tmp34;});_tmp35->v=_tmp68;});_tmp35;});_tmp36->f1=_tmp69;});_tmp36;});_tmp37->tree=_tmp6A;});_tmp37;});}}};}};}};}else{
# 158
return d;}}
# 161
struct Cyc_SlowDict_Dict*Cyc_SlowDict_delete_present(struct Cyc_SlowDict_Dict*d,void*key){
struct Cyc_SlowDict_Dict*_tmp38=Cyc_SlowDict_delete(d,key);struct Cyc_SlowDict_Dict*d2=_tmp38;
if(d == d2)
(int)_throw((void*)& Cyc_SlowDict_Absent_val);
return d2;}
# 168
static void*Cyc_SlowDict_fold_tree(void*(*f)(void*,void*,void*),void*t,void*accum){
void*_tmp39=t;struct Cyc_Splay_noderef*_tmp3A;if(*((int*)_tmp39)== 0)
# 171
return accum;else{_tmp3A=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp39)->f1;{struct Cyc_Splay_noderef*nr=_tmp3A;
# 173
struct Cyc_Splay_node*n=nr->v;
return({void*(*_tmp6F)(void*,void*,void*)=f;void*_tmp6E=n->key;void*_tmp6D=n->data;_tmp6F(_tmp6E,_tmp6D,({void*(*_tmp6C)(void*,void*,void*)=f;void*_tmp6B=n->left;Cyc_SlowDict_fold_tree(_tmp6C,_tmp6B,
Cyc_SlowDict_fold_tree(f,n->right,accum));}));});}};}
# 178
void*Cyc_SlowDict_fold(void*(*f)(void*,void*,void*),struct Cyc_SlowDict_Dict*d,void*accum){
return Cyc_SlowDict_fold_tree(f,d->tree,accum);}
# 182
static void*Cyc_SlowDict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,void*t,void*accum){
void*_tmp3B=t;struct Cyc_Splay_noderef*_tmp3C;if(*((int*)_tmp3B)== 0)
# 185
return accum;else{_tmp3C=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp3B)->f1;{struct Cyc_Splay_noderef*nr=_tmp3C;
# 187
struct Cyc_Splay_node*n=nr->v;
return({void*(*_tmp76)(void*,void*,void*,void*)=f;void*_tmp75=env;void*_tmp74=n->key;void*_tmp73=n->data;_tmp76(_tmp75,_tmp74,_tmp73,({
void*(*_tmp72)(void*,void*,void*,void*)=f;void*_tmp71=env;void*_tmp70=n->left;Cyc_SlowDict_fold_tree_c(_tmp72,_tmp71,_tmp70,Cyc_SlowDict_fold_tree_c(f,env,n->right,accum));}));});}};}
# 192
void*Cyc_SlowDict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_SlowDict_Dict*dict,void*accum){
return Cyc_SlowDict_fold_tree_c(f,env,dict->tree,accum);}
# 196
static void Cyc_SlowDict_app_tree(void*(*f)(void*,void*),void*t){
void*_tmp3D=t;struct Cyc_Splay_noderef*_tmp3E;if(*((int*)_tmp3D)== 0)
goto _LL0;else{_tmp3E=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp3D)->f1;{struct Cyc_Splay_noderef*nr=_tmp3E;
# 200
struct Cyc_Splay_node*_tmp3F=nr->v;struct Cyc_Splay_node*n=_tmp3F;
Cyc_SlowDict_app_tree(f,n->left);
f(n->key,n->data);
Cyc_SlowDict_app_tree(f,n->right);
goto _LL0;}}_LL0:;}
# 207
void Cyc_SlowDict_app(void*(*f)(void*,void*),struct Cyc_SlowDict_Dict*d){
Cyc_SlowDict_app_tree(f,d->tree);}
# 211
static void Cyc_SlowDict_iter_tree(void(*f)(void*,void*),void*t){
void*_tmp40=t;struct Cyc_Splay_noderef*_tmp41;if(*((int*)_tmp40)== 0)
goto _LL0;else{_tmp41=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp40)->f1;{struct Cyc_Splay_noderef*nr=_tmp41;
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
void*_tmp42=t;struct Cyc_Splay_noderef*_tmp43;if(*((int*)_tmp42)== 0)
goto _LL0;else{_tmp43=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp42)->f1;{struct Cyc_Splay_noderef*nr=_tmp43;
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
void*_tmp44=t;struct Cyc_Splay_noderef*_tmp45;if(*((int*)_tmp44)== 0)
goto _LL0;else{_tmp45=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp44)->f1;{struct Cyc_Splay_noderef*nr=_tmp45;
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
void*_tmp46=t;struct Cyc_Splay_noderef*_tmp47;if(*((int*)_tmp46)== 0)
return(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp48=_cycalloc(sizeof(*_tmp48));_tmp48->tag=0,_tmp48->f1=0;_tmp48;});else{_tmp47=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp46)->f1;{struct Cyc_Splay_noderef*nr=_tmp47;
# 260
struct Cyc_Splay_node*_tmp49=nr->v;struct Cyc_Splay_node*n=_tmp49;
return(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp4C=_cycalloc(sizeof(*_tmp4C));_tmp4C->tag=1,({struct Cyc_Splay_noderef*_tmp7B=({struct Cyc_Splay_noderef*_tmp4B=_cycalloc(sizeof(*_tmp4B));({struct Cyc_Splay_node*_tmp7A=({struct Cyc_Splay_node*_tmp4A=_cycalloc(sizeof(*_tmp4A));_tmp4A->key=n->key,({
void*_tmp79=f(n->data);_tmp4A->data=_tmp79;}),({
void*_tmp78=Cyc_SlowDict_map_tree(f,n->left);_tmp4A->left=_tmp78;}),({
void*_tmp77=Cyc_SlowDict_map_tree(f,n->right);_tmp4A->right=_tmp77;});_tmp4A;});
# 261
_tmp4B->v=_tmp7A;});_tmp4B;});_tmp4C->f1=_tmp7B;});_tmp4C;});}};}
# 267
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map(void*(*f)(void*),struct Cyc_SlowDict_Dict*d){
return({struct Cyc_SlowDict_Dict*_tmp4D=_cycalloc(sizeof(*_tmp4D));_tmp4D->reln=d->reln,({void*_tmp7C=Cyc_SlowDict_map_tree(f,d->tree);_tmp4D->tree=_tmp7C;});_tmp4D;});}
# 271
static void*Cyc_SlowDict_map_tree_c(void*(*f)(void*,void*),void*env,void*t){
void*_tmp4E=t;struct Cyc_Splay_noderef*_tmp4F;if(*((int*)_tmp4E)== 0)
return(void*)({struct Cyc_Splay_Leaf_Splay_tree_struct*_tmp50=_cycalloc(sizeof(*_tmp50));_tmp50->tag=0,_tmp50->f1=0;_tmp50;});else{_tmp4F=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp4E)->f1;{struct Cyc_Splay_noderef*nr=_tmp4F;
# 275
struct Cyc_Splay_node*n=nr->v;
return(void*)({struct Cyc_Splay_Node_Splay_tree_struct*_tmp53=_cycalloc(sizeof(*_tmp53));_tmp53->tag=1,({struct Cyc_Splay_noderef*_tmp81=({struct Cyc_Splay_noderef*_tmp52=_cycalloc(sizeof(*_tmp52));({struct Cyc_Splay_node*_tmp80=({struct Cyc_Splay_node*_tmp51=_cycalloc(sizeof(*_tmp51));_tmp51->key=n->key,({void*_tmp7F=f(env,n->data);_tmp51->data=_tmp7F;}),({
void*_tmp7E=Cyc_SlowDict_map_tree_c(f,env,n->left);_tmp51->left=_tmp7E;}),({
void*_tmp7D=Cyc_SlowDict_map_tree_c(f,env,n->right);_tmp51->right=_tmp7D;});_tmp51;});
# 276
_tmp52->v=_tmp80;});_tmp52;});_tmp53->f1=_tmp81;});_tmp53;});}};}
# 281
struct Cyc_SlowDict_Dict*Cyc_SlowDict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_SlowDict_Dict*d){
return({struct Cyc_SlowDict_Dict*_tmp54=_cycalloc(sizeof(*_tmp54));_tmp54->reln=d->reln,({void*_tmp82=Cyc_SlowDict_map_tree_c(f,env,d->tree);_tmp54->tree=_tmp82;});_tmp54;});}
# 285
struct _tuple0*Cyc_SlowDict_choose(struct Cyc_SlowDict_Dict*d){
void*_tmp55=d->tree;void*_stmttmp8=_tmp55;void*_tmp56=_stmttmp8;struct Cyc_Splay_noderef*_tmp57;if(*((int*)_tmp56)== 0)
(int)_throw((void*)& Cyc_SlowDict_Absent_val);else{_tmp57=((struct Cyc_Splay_Node_Splay_tree_struct*)_tmp56)->f1;{struct Cyc_Splay_noderef*nr=_tmp57;
return({struct _tuple0*_tmp58=_cycalloc(sizeof(*_tmp58));_tmp58->f1=(nr->v)->key,_tmp58->f2=(nr->v)->data;_tmp58;});}};}
# 292
struct Cyc_List_List*Cyc_SlowDict_to_list_f(void*k,void*v,struct Cyc_List_List*accum){
return({struct Cyc_List_List*_tmp5A=_cycalloc(sizeof(*_tmp5A));({struct _tuple0*_tmp83=({struct _tuple0*_tmp59=_cycalloc(sizeof(*_tmp59));_tmp59->f1=k,_tmp59->f2=v;_tmp59;});_tmp5A->hd=_tmp83;}),_tmp5A->tl=accum;_tmp5A;});}
# 296
struct Cyc_List_List*Cyc_SlowDict_to_list(struct Cyc_SlowDict_Dict*d){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(void*,void*,struct Cyc_List_List*),struct Cyc_SlowDict_Dict*,struct Cyc_List_List*))Cyc_SlowDict_fold)(Cyc_SlowDict_to_list_f,d,0);}
