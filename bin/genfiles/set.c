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
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*x);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 117
extern void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x);
# 133
extern void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x);
# 135
extern void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x);
# 210
extern struct Cyc_List_List*Cyc_List_merge_sort(int(*cmp)(void*,void*),struct Cyc_List_List*x);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};struct Cyc_Iter_Iter{void*env;int(*next)(void*env,void*dest);};struct Cyc_Set_Set;
# 100 "set.h"
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt);
# 106
int Cyc_Set_setcmp(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2);extern char Cyc_Set_Absent[7U];struct Cyc_Set_Absent_exn_struct{char*tag;};struct Cyc_Set_Set{int(*cmp)(void*,void*);int cardinality;struct Cyc_List_List*nodes;};
# 37 "set.cyc"
struct Cyc_Set_Set*Cyc_Set_empty(int(*comp)(void*,void*)){
return({struct Cyc_Set_Set*_tmp0=_cycalloc(sizeof(*_tmp0));_tmp0->cmp=comp,_tmp0->cardinality=0,_tmp0->nodes=0;_tmp0;});}
# 40
struct Cyc_Set_Set*Cyc_Set_rempty(struct _RegionHandle*rgn,int(*comp)(void*,void*)){
return({struct Cyc_Set_Set*_tmp1=_region_malloc(rgn,sizeof(*_tmp1));_tmp1->cmp=comp,_tmp1->cardinality=0,_tmp1->nodes=0;_tmp1;});}
# 44
struct Cyc_Set_Set*Cyc_Set_singleton(int(*comp)(void*,void*),void*x){
return({struct Cyc_Set_Set*_tmp3=_cycalloc(sizeof(*_tmp3));_tmp3->cmp=comp,_tmp3->cardinality=1,({struct Cyc_List_List*_tmp1D=({struct Cyc_List_List*_tmp2=_cycalloc(sizeof(*_tmp2));_tmp2->hd=x,_tmp2->tl=0;_tmp2;});_tmp3->nodes=_tmp1D;});_tmp3;});}
# 48
int Cyc_Set_cardinality(struct Cyc_Set_Set*s){
return s->cardinality;}
# 53
int Cyc_Set_is_empty(struct Cyc_Set_Set*s){
return s->cardinality == 0;}
# 58
static int Cyc_Set_member_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
for(0;n != 0;n=n->tl){
int i=cmp(elt,n->hd);
if(i == 0)return 1;
if(i < 0)return 0;}
# 64
return 0;}
# 67
int Cyc_Set_member(struct Cyc_Set_Set*s,void*elt){
return Cyc_Set_member_b(s->cmp,s->nodes,elt);}
# 74
static struct Cyc_List_List*Cyc_Set_insert_b(struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
# 77
if(n == 0)
return({struct Cyc_List_List*_tmp4=_region_malloc(rgn,sizeof(*_tmp4));_tmp4->hd=elt,_tmp4->tl=0;_tmp4;});{
int i=cmp(elt,n->hd);
if(i < 0)
return({struct Cyc_List_List*_tmp5=_region_malloc(rgn,sizeof(*_tmp5));_tmp5->hd=elt,_tmp5->tl=n;_tmp5;});{
struct Cyc_List_List*result=({struct Cyc_List_List*_tmp8=_region_malloc(rgn,sizeof(*_tmp8));_tmp8->hd=n->hd,_tmp8->tl=0;_tmp8;});
struct Cyc_List_List*prev=result;
n=n->tl;
while(n != 0 &&(i=cmp(n->hd,elt))< 0){
({struct Cyc_List_List*_tmp1E=({struct Cyc_List_List*_tmp6=_region_malloc(rgn,sizeof(*_tmp6));_tmp6->hd=n->hd,_tmp6->tl=0;_tmp6;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp1E;});
prev=prev->tl;
n=n->tl;}
# 90
({struct Cyc_List_List*_tmp1F=({struct Cyc_List_List*_tmp7=_region_malloc(rgn,sizeof(*_tmp7));_tmp7->hd=elt,_tmp7->tl=n;_tmp7;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp1F;});
return result;}}}
# 95
struct Cyc_Set_Set*Cyc_Set_insert(struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return s;
return({struct Cyc_Set_Set*_tmp9=_cycalloc(sizeof(*_tmp9));_tmp9->cmp=s->cmp,_tmp9->cardinality=s->cardinality + 1,({
struct Cyc_List_List*_tmp20=Cyc_Set_insert_b(Cyc_Core_heap_region,s->cmp,s->nodes,elt);_tmp9->nodes=_tmp20;});_tmp9;});}
# 100
struct Cyc_Set_Set*Cyc_Set_rinsert(struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))return s;
return({struct Cyc_Set_Set*_tmpA=_region_malloc(rgn,sizeof(*_tmpA));_tmpA->cmp=s->cmp,_tmpA->cardinality=s->cardinality + 1,({
struct Cyc_List_List*_tmp21=Cyc_Set_insert_b(rgn,s->cmp,s->nodes,elt);_tmpA->nodes=_tmp21;});_tmpA;});}
# 108
static struct Cyc_List_List*Cyc_Set_imp_insert_b(struct _RegionHandle*rgn,int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
# 111
if(n == 0)
return({struct Cyc_List_List*_tmpB=_region_malloc(rgn,sizeof(*_tmpB));_tmpB->hd=elt,_tmpB->tl=0;_tmpB;});{
int i=cmp(elt,n->hd);
if(i < 0)
return({struct Cyc_List_List*_tmpC=_region_malloc(rgn,sizeof(*_tmpC));_tmpC->hd=elt,_tmpC->tl=n;_tmpC;});{
struct Cyc_List_List*prev=n;struct Cyc_List_List*res=n;
n=n->tl;
while(n != 0 &&(i=cmp(n->hd,elt))< 0){
prev=((struct Cyc_List_List*)_check_null(prev))->tl;
n=n->tl;}
# 122
({struct Cyc_List_List*_tmp22=({struct Cyc_List_List*_tmpD=_region_malloc(rgn,sizeof(*_tmpD));_tmpD->hd=elt,_tmpD->tl=n;_tmpD;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp22;});
return res;}}}
# 127
void Cyc_Set_imp_insert(struct Cyc_Set_Set*s,void*elt){
if(!Cyc_Set_member(s,elt))
({struct Cyc_List_List*_tmp23=Cyc_Set_imp_insert_b(Cyc_Core_heap_region,s->cmp,s->nodes,elt);s->nodes=_tmp23;});}
# 131
void Cyc_Set_imp_rinsert(struct _RegionHandle*rgn,struct Cyc_Set_Set*s,void*elt){
if(!Cyc_Set_member(s,elt))
({struct Cyc_List_List*_tmp24=Cyc_Set_imp_insert_b(rgn,s->cmp,s->nodes,elt);s->nodes=_tmp24;});}
# 137
struct Cyc_Set_Set*Cyc_Set_union_two(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
if(s1 == s2)
return s1;
if(s1->cardinality == 0)
return s2;
if(s2->cardinality == 0)
return s1;{
# 145
struct Cyc_List_List*nodes=0;
int cardinality=0;
int(*comp)(void*,void*)=s1->cmp;
# 149
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
struct Cyc_List_List*curr=0;
# 153
while(x1 != 0 && x2 != 0){
int i=comp(x1->hd,x2->hd);
if(i == 0)
# 157
x2=x2->tl;else{
if(i < 0){
# 160
if(curr == 0){
nodes=({struct Cyc_List_List*_tmpE=_cycalloc(sizeof(*_tmpE));_tmpE->hd=x1->hd,_tmpE->tl=0;_tmpE;});
curr=nodes;}else{
# 164
({struct Cyc_List_List*_tmp25=({struct Cyc_List_List*_tmpF=_cycalloc(sizeof(*_tmpF));_tmpF->hd=x1->hd,_tmpF->tl=0;_tmpF;});curr->tl=_tmp25;});
curr=curr->tl;}
# 167
x1=x1->tl;
++ cardinality;}else{
# 171
if(curr == 0){
nodes=({struct Cyc_List_List*_tmp10=_cycalloc(sizeof(*_tmp10));_tmp10->hd=x2->hd,_tmp10->tl=0;_tmp10;});
curr=nodes;}else{
# 175
({struct Cyc_List_List*_tmp26=({struct Cyc_List_List*_tmp11=_cycalloc(sizeof(*_tmp11));_tmp11->hd=x2->hd,_tmp11->tl=0;_tmp11;});curr->tl=_tmp26;});
curr=curr->tl;}
# 178
x2=x2->tl;
++ cardinality;}}}
# 182
if(x1 != 0){
# 184
if(curr == 0)
nodes=x1;else{
# 187
curr->tl=x1;}
({int _tmp27=Cyc_List_length(x1);cardinality +=_tmp27;});}else{
if(x2 != 0){
# 191
if(curr == 0)
nodes=x2;else{
# 194
curr->tl=x2;}
({int _tmp28=Cyc_List_length(x2);cardinality +=_tmp28;});}}
# 197
return({struct Cyc_Set_Set*_tmp12=_cycalloc(sizeof(*_tmp12));_tmp12->cmp=comp,_tmp12->cardinality=cardinality,_tmp12->nodes=nodes;_tmp12;});}}
# 202
static struct Cyc_List_List*Cyc_Set_delete_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt){
if(cmp(((struct Cyc_List_List*)_check_null(n))->hd,elt)== 0)return n->tl;{
# 205
struct Cyc_List_List*result=({struct Cyc_List_List*_tmp14=_cycalloc(sizeof(*_tmp14));_tmp14->hd=n->hd,_tmp14->tl=0;_tmp14;});
struct Cyc_List_List*prev=result;
n=n->tl;
while(n != 0 && cmp(n->hd,elt)!= 0){
({struct Cyc_List_List*_tmp29=({struct Cyc_List_List*_tmp13=_cycalloc(sizeof(*_tmp13));_tmp13->hd=n->hd,_tmp13->tl=0;_tmp13;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp29;});
prev=prev->tl;
n=n->tl;}
# 213
({struct Cyc_List_List*_tmp2A=((struct Cyc_List_List*)_check_null(n))->tl;((struct Cyc_List_List*)_check_null(prev))->tl=_tmp2A;});
return result;}}
# 218
struct Cyc_Set_Set*Cyc_Set_delete(struct Cyc_Set_Set*s,void*elt){
if(Cyc_Set_member(s,elt))
return({struct Cyc_Set_Set*_tmp15=_cycalloc(sizeof(*_tmp15));_tmp15->cmp=s->cmp,_tmp15->cardinality=s->cardinality - 1,({
struct Cyc_List_List*_tmp2B=Cyc_Set_delete_b(s->cmp,s->nodes,elt);_tmp15->nodes=_tmp2B;});_tmp15;});
return s;}
# 228
static struct Cyc_List_List*Cyc_Set_imp_delete_b(int(*cmp)(void*,void*),struct Cyc_List_List*n,void*elt,void**ret){
if(cmp(((struct Cyc_List_List*)_check_null(n))->hd,elt)== 0)return n->tl;{
# 231
struct Cyc_List_List*prev=n;struct Cyc_List_List*res=n;
n=n->tl;
while(n != 0 && cmp(n->hd,elt)!= 0){
prev=((struct Cyc_List_List*)_check_null(prev))->tl;
n=n->tl;}
# 237
({struct Cyc_List_List*_tmp2C=((struct Cyc_List_List*)_check_null(n))->tl;((struct Cyc_List_List*)_check_null(prev))->tl=_tmp2C;});
*ret=n->hd;
return res;}}
# 243
void*Cyc_Set_imp_delete(struct Cyc_Set_Set*s,void*elt){
void*ret=elt;
if(Cyc_Set_member(s,elt))
({struct Cyc_List_List*_tmp2D=Cyc_Set_imp_delete_b(s->cmp,s->nodes,elt,& ret);s->nodes=_tmp2D;});
return ret;}
# 251
void*Cyc_Set_fold(void*(*f)(void*,void*),struct Cyc_Set_Set*s,void*accum){
{struct Cyc_List_List*n=s->nodes;for(0;n != 0;n=n->tl){
accum=f(n->hd,accum);}}
return accum;}
# 256
void*Cyc_Set_fold_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Set_Set*s,void*accum){
{struct Cyc_List_List*n=s->nodes;for(0;n != 0;n=n->tl){
accum=f(env,n->hd,accum);}}
return accum;}
# 263
void Cyc_Set_app(void*(*f)(void*),struct Cyc_Set_Set*s){
Cyc_List_app(f,s->nodes);}
# 266
void Cyc_Set_iter(void(*f)(void*),struct Cyc_Set_Set*s){
Cyc_List_iter(f,s->nodes);}
# 269
void Cyc_Set_iter_c(void(*f)(void*,void*),void*env,struct Cyc_Set_Set*s){
Cyc_List_iter_c(f,env,s->nodes);}
# 276
struct Cyc_Set_Set*Cyc_Set_intersect(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
if(s1 == s2)
return s1;{
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
struct Cyc_List_List*result=0;struct Cyc_List_List*prev=0;
int card=0;
if(x1 == 0)
return s1;
if(x2 == 0)
return s2;
# 289
while(x1 != 0 && x2 != 0){
int i=comp(x1->hd,x2->hd);
if(i == 0){
if(result == 0){
result=({struct Cyc_List_List*_tmp16=_cycalloc(sizeof(*_tmp16));_tmp16->hd=x1->hd,_tmp16->tl=0;_tmp16;});
prev=result;}else{
# 296
({struct Cyc_List_List*_tmp2E=({struct Cyc_List_List*_tmp17=_cycalloc(sizeof(*_tmp17));_tmp17->hd=x1->hd,_tmp17->tl=0;_tmp17;});((struct Cyc_List_List*)_check_null(prev))->tl=_tmp2E;});
prev=prev->tl;}
# 299
++ card;
x1=x1->tl;
x2=x2->tl;}else{
if(i < 0)
x1=x1->tl;else{
# 305
x2=x2->tl;}}}
# 308
return({struct Cyc_Set_Set*_tmp18=_cycalloc(sizeof(*_tmp18));_tmp18->cmp=comp,_tmp18->cardinality=card,_tmp18->nodes=result;_tmp18;});}}
# 311
struct Cyc_Set_Set*Cyc_Set_from_list(int(*comp)(void*,void*),struct Cyc_List_List*x){
struct Cyc_List_List*z=Cyc_List_merge_sort(comp,x);
# 314
{struct Cyc_List_List*y=z;for(0;y != 0;y=y->tl){
if(y->tl != 0 && comp(y->hd,((struct Cyc_List_List*)_check_null(y->tl))->hd)== 0)
y->tl=((struct Cyc_List_List*)_check_null(y->tl))->tl;}}
return({struct Cyc_Set_Set*_tmp19=_cycalloc(sizeof(*_tmp19));_tmp19->cmp=comp,({int _tmp2F=Cyc_List_length(z);_tmp19->cardinality=_tmp2F;}),_tmp19->nodes=z;_tmp19;});}
# 320
int Cyc_Set_subset(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
# 325
while(1){
if(x1 == 0)return 1;
if(x2 == 0)return 0;{
int i=comp(x1->hd,x2->hd);
if(i < 0)
return 0;
if(i == 0)
x1=x1->tl;
x2=x2->tl;}}
# 335
return 1;}
# 338
struct Cyc_Set_Set*Cyc_Set_diff(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
int(*comp)(void*,void*)=s1->cmp;
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
int card=s1->cardinality;
# 344
if(x2 == 0)return s1;
# 346
for(0;x2 != 0;x2=x2->tl){
void*elt=x2->hd;
if(Cyc_Set_member_b(comp,x1,elt)){
-- card;
x1=Cyc_Set_delete_b(comp,x1,elt);}}
# 353
return({struct Cyc_Set_Set*_tmp1A=_cycalloc(sizeof(*_tmp1A));_tmp1A->cmp=comp,_tmp1A->cardinality=card,_tmp1A->nodes=x1;_tmp1A;});}
# 356
int Cyc_Set_setcmp(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
if((unsigned)s1 == (unsigned)s2)
return 0;
if(s1->cardinality != s2->cardinality)
return s1->cardinality - s2->cardinality;{
# 362
struct Cyc_List_List*x1=s1->nodes;
struct Cyc_List_List*x2=s2->nodes;
int(*cmp)(void*,void*)=s1->cmp;
while(x1 != 0){
int diff=cmp(x1->hd,((struct Cyc_List_List*)_check_null(x2))->hd);
if(diff != 0)return diff;
x1=x1->tl;
x2=x2->tl;}
# 371
return 0;}}
# 374
int Cyc_Set_equals(struct Cyc_Set_Set*s1,struct Cyc_Set_Set*s2){
return Cyc_Set_setcmp(s1,s2)== 0;}char Cyc_Set_Absent[7U]="Absent";
# 379
struct Cyc_Set_Absent_exn_struct Cyc_Set_Absent_val={Cyc_Set_Absent};
# 383
void*Cyc_Set_choose(struct Cyc_Set_Set*s){
if(s->nodes == 0)(int)_throw((void*)& Cyc_Set_Absent_val);
return((struct Cyc_List_List*)_check_null(s->nodes))->hd;}
# 388
int Cyc_Set_iter_f(struct Cyc_List_List**elts_left,void**dest){
if(!((unsigned)*elts_left))
return 0;
*dest=((struct Cyc_List_List*)_check_null(*elts_left))->hd;
*elts_left=((struct Cyc_List_List*)_check_null(*elts_left))->tl;
return 1;}
# 395
struct Cyc_Iter_Iter Cyc_Set_make_iter(struct _RegionHandle*rgn,struct Cyc_Set_Set*s){
# 398
return({struct Cyc_Iter_Iter _tmp1C;({struct Cyc_List_List**_tmp30=({struct Cyc_List_List**_tmp1B=_region_malloc(rgn,sizeof(*_tmp1B));*_tmp1B=s->nodes;_tmp1B;});_tmp1C.env=_tmp30;}),_tmp1C.next=Cyc_Set_iter_f;_tmp1C;});}
