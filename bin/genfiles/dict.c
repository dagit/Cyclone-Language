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
 extern char Cyc_Core_Invalid_argument[17U];extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];extern char Cyc_Core_Not_found[10U];extern char Cyc_Core_Unreachable[12U];
# 168 "core.h"
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
extern int Cyc_List_length(struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];extern char Cyc_List_Nth[4U];struct Cyc___cycFILE;
# 252 "cycboot.h"
extern int Cyc_getw(struct Cyc___cycFILE*);extern char Cyc_FileCloseError[15U];extern char Cyc_FileOpenError[14U];struct Cyc_Iter_Iter{void*env;int(*next)(void*,void*);};struct Cyc_Dict_T;struct Cyc_Dict_Dict{int(*rel)(void*,void*);struct _RegionHandle*r;const struct Cyc_Dict_T*t;};extern char Cyc_Dict_Present[8U];struct Cyc_Dict_Present_exn_struct{char*tag;};extern char Cyc_Dict_Absent[7U];struct Cyc_Dict_Absent_exn_struct{char*tag;};
# 62 "dict.h"
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*)(void*,void*));
# 68
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*,int(*)(void*,void*));
# 83
int Cyc_Dict_member(struct Cyc_Dict_Dict,void*);
# 87
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict,void*,void*);
# 104
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*,int(*)(void*,void*),void*,void*);
# 110
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict,void*);
# 135 "dict.h"
void*Cyc_Dict_fold_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,void*);
# 149
void Cyc_Dict_iter_c(void(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);
# 170
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 181
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*,void*(*)(void*),struct Cyc_Dict_Dict);
# 187
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_Dict_Dict);
# 212
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*)(void*,void*,void*,void*),void*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 218
int Cyc_Dict_forall_c(int(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);
# 236
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*,struct Cyc_Dict_Dict);
# 244
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*,int(*)(void*,void*),struct Cyc_Dict_Dict);
# 250
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*,int(*)(void*,void*,void*),void*,struct Cyc_Dict_Dict);
# 263
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict);
# 274
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*,struct Cyc_Dict_Dict,void*);char Cyc_Dict_Absent[7U]="Absent";char Cyc_Dict_Present[8U]="Present";
# 27 "dict.cyc"
struct Cyc_Dict_Absent_exn_struct Cyc_Dict_Absent_val={Cyc_Dict_Absent};
struct Cyc_Dict_Present_exn_struct Cyc_Dict_Present_val={Cyc_Dict_Present};
# 30
enum Cyc_Dict_Color{Cyc_Dict_R =0U,Cyc_Dict_B =1U};struct _tuple0{void*f1;void*f2;};struct Cyc_Dict_T{enum Cyc_Dict_Color color;const struct Cyc_Dict_T*left;const struct Cyc_Dict_T*right;struct _tuple0 key_val;};
# 40
struct Cyc_Dict_Dict Cyc_Dict_rempty(struct _RegionHandle*r,int(*comp)(void*,void*)){
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=comp,_Tmp0.r=r,_Tmp0.t=0;_Tmp0;});}
# 43
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){
return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}
# 46
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,void*),void*key,void*data){
# 48
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=comp,_Tmp0.r=r,({const struct Cyc_Dict_T*_Tmp1=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_Tmp2=_region_malloc(r,sizeof(struct Cyc_Dict_T));_Tmp2->color=1U,_Tmp2->left=0,_Tmp2->right=0,(_Tmp2->key_val).f1=key,(_Tmp2->key_val).f2=data;_Tmp2;});_Tmp0.t=_Tmp1;});_Tmp0;});}
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
return d.t == (const struct Cyc_Dict_T*)0;}
# 64
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*key){
int(*rel)(void*,void*)=d.rel;
const struct Cyc_Dict_T*t=d.t;
while(t != (const struct Cyc_Dict_T*)0){
int i=rel(key,(t->key_val).f1);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return 1;}}}
# 73
return 0;}
# 76
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*key){
int(*rel)(void*,void*)=d.rel;
const struct Cyc_Dict_T*t=d.t;
while(t != (const struct Cyc_Dict_T*)0){
int i=rel(key,(t->key_val).f1);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return(t->key_val).f2;}}}
# 85
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 88
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*key){
const struct Cyc_Dict_T*t=d.t;
while(t != (const struct Cyc_Dict_T*)0){
int i=cmp(key,(t->key_val).f1);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return(t->key_val).f2;}}}
# 96
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 99
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*key){
int(*rel)(void*,void*)=d.rel;
const struct Cyc_Dict_T*t=d.t;
while(t != (const struct Cyc_Dict_T*)0){
int i=rel(key,(t->key_val).f1);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return&(t->key_val).f2;}}}
# 108
return 0;}
# 111
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*key,void**ans_place){
int(*rel)(void*,void*)=d.rel;
const struct Cyc_Dict_T*t=d.t;
while(t != (const struct Cyc_Dict_T*)0){
int i=rel(key,(t->key_val).f1);
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
# 119
*ans_place=(t->key_val).f2;
return 1;}}}
# 123
return 0;}struct _tuple1{enum Cyc_Dict_Color f1;const struct Cyc_Dict_T*f2;const struct Cyc_Dict_T*f3;struct _tuple0 f4;};
# 126
static struct Cyc_Dict_T*Cyc_Dict_balance(struct _RegionHandle*r,struct _tuple1 quad){
# 129
struct _tuple1 _Tmp0=quad;enum Cyc_Dict_Color _Tmp1;struct _tuple0 _Tmp2;const struct Cyc_Dict_T*_Tmp3;struct _tuple0 _Tmp4;const struct Cyc_Dict_T*_Tmp5;struct _tuple0 _Tmp6;const struct Cyc_Dict_T*_Tmp7;const struct Cyc_Dict_T*_Tmp8;if(_Tmp0.f1 == Cyc_Dict_B){if(_Tmp0.f2 != 0){if(((const struct Cyc_Dict_T*)_Tmp0.f2)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_Tmp0.f2)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f2)->left)->color == Cyc_Dict_R){_Tmp8=((_Tmp0.f2)->left)->left;_Tmp7=((_Tmp0.f2)->left)->right;_Tmp6=((_Tmp0.f2)->left)->key_val;_Tmp5=(_Tmp0.f2)->right;_Tmp4=(_Tmp0.f2)->key_val;_Tmp3=_Tmp0.f3;_Tmp2=_Tmp0.f4;{const struct Cyc_Dict_T*a=_Tmp8;const struct Cyc_Dict_T*b=_Tmp7;struct _tuple0 x=_Tmp6;const struct Cyc_Dict_T*c=_Tmp5;struct _tuple0 y=_Tmp4;const struct Cyc_Dict_T*d=_Tmp3;struct _tuple0 z=_Tmp2;
_Tmp8=a;_Tmp7=b;_Tmp5=c;_Tmp6=y;_Tmp4=x;_Tmp3=d;_Tmp2=z;goto _LL4;}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f2)->right)->color == Cyc_Dict_R)goto _LL3;else{if(_Tmp0.f3 != 0){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_Tmp0.f3 != 0){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f2)->right)->color == Cyc_Dict_R){_LL3: _Tmp8=(_Tmp0.f2)->left;_Tmp7=((_Tmp0.f2)->right)->left;_Tmp5=((_Tmp0.f2)->right)->right;_Tmp6=((_Tmp0.f2)->right)->key_val;_Tmp4=(_Tmp0.f2)->key_val;_Tmp3=_Tmp0.f3;_Tmp2=_Tmp0.f4;_LL4: {const struct Cyc_Dict_T*a=_Tmp8;const struct Cyc_Dict_T*b=_Tmp7;const struct Cyc_Dict_T*c=_Tmp5;struct _tuple0 y=_Tmp6;struct _tuple0 x=_Tmp4;const struct Cyc_Dict_T*d=_Tmp3;struct _tuple0 z=_Tmp2;
_Tmp8=a;_Tmp7=b;_Tmp5=c;_Tmp6=y;_Tmp3=d;_Tmp4=z;_Tmp2=x;goto _LL6;}}else{if(_Tmp0.f3 != 0){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_Tmp0.f3 != 0){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(_Tmp0.f3 != 0){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_Tmp0.f3 != 0){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_Tmp0.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->left)->color == Cyc_Dict_R){_LL5: _Tmp8=_Tmp0.f2;_Tmp7=((_Tmp0.f3)->left)->left;_Tmp5=((_Tmp0.f3)->left)->right;_Tmp6=((_Tmp0.f3)->left)->key_val;_Tmp3=(_Tmp0.f3)->right;_Tmp4=(_Tmp0.f3)->key_val;_Tmp2=_Tmp0.f4;_LL6: {const struct Cyc_Dict_T*a=_Tmp8;const struct Cyc_Dict_T*b=_Tmp7;const struct Cyc_Dict_T*c=_Tmp5;struct _tuple0 y=_Tmp6;const struct Cyc_Dict_T*d=_Tmp3;struct _tuple0 z=_Tmp4;struct _tuple0 x=_Tmp2;
_Tmp8=a;_Tmp7=b;_Tmp5=c;_Tmp3=d;_Tmp6=z;_Tmp4=y;_Tmp2=x;goto _LL8;}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_Tmp0.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_Tmp0.f3)->right)->color == Cyc_Dict_R){_LL7: _Tmp8=_Tmp0.f2;_Tmp7=(_Tmp0.f3)->left;_Tmp5=((_Tmp0.f3)->right)->left;_Tmp3=((_Tmp0.f3)->right)->right;_Tmp6=((_Tmp0.f3)->right)->key_val;_Tmp4=(_Tmp0.f3)->key_val;_Tmp2=_Tmp0.f4;_LL8: {const struct Cyc_Dict_T*a=_Tmp8;const struct Cyc_Dict_T*b=_Tmp7;const struct Cyc_Dict_T*c=_Tmp5;const struct Cyc_Dict_T*d=_Tmp3;struct _tuple0 z=_Tmp6;struct _tuple0 y=_Tmp4;struct _tuple0 x=_Tmp2;
# 134
return({struct Cyc_Dict_T*_Tmp9=_region_malloc(r,sizeof(struct Cyc_Dict_T));_Tmp9->color=0U,({const struct Cyc_Dict_T*_TmpA=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_TmpB=_region_malloc(r,sizeof(struct Cyc_Dict_T));_TmpB->color=1U,_TmpB->left=a,_TmpB->right=b,_TmpB->key_val=x;_TmpB;});_Tmp9->left=_TmpA;}),({const struct Cyc_Dict_T*_TmpA=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_TmpB=_region_malloc(r,sizeof(struct Cyc_Dict_T));_TmpB->color=1U,_TmpB->left=c,_TmpB->right=d,_TmpB->key_val=z;_TmpB;});_Tmp9->right=_TmpA;}),_Tmp9->key_val=y;_Tmp9;});}}else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{_LL9: _Tmp1=_Tmp0.f1;_Tmp8=_Tmp0.f2;_Tmp7=_Tmp0.f3;_Tmp6=_Tmp0.f4;{enum Cyc_Dict_Color a=_Tmp1;const struct Cyc_Dict_T*b=_Tmp8;const struct Cyc_Dict_T*c=_Tmp7;struct _tuple0 d=_Tmp6;
# 136
return({struct Cyc_Dict_T*_Tmp9=_region_malloc(r,sizeof(struct Cyc_Dict_T));_Tmp9->color=a,_Tmp9->left=b,_Tmp9->right=c,_Tmp9->key_val=d;_Tmp9;});}};}
# 140
static struct Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,const struct Cyc_Dict_T*t){
# 142
const struct Cyc_Dict_T*_Tmp0=t;struct _tuple0 _Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;enum Cyc_Dict_Color _Tmp4;if(_Tmp0 == 0)
return({struct Cyc_Dict_T*_Tmp5=_region_malloc(r,sizeof(struct Cyc_Dict_T));_Tmp5->color=0U,_Tmp5->left=0,_Tmp5->right=0,_Tmp5->key_val=key_val;_Tmp5;});else{_Tmp4=_Tmp0->color;_Tmp3=_Tmp0->left;_Tmp2=_Tmp0->right;_Tmp1=_Tmp0->key_val;{enum Cyc_Dict_Color color=_Tmp4;const struct Cyc_Dict_T*a=_Tmp3;const struct Cyc_Dict_T*b=_Tmp2;struct _tuple0 y=_Tmp1;
# 145
int i=rel(key_val.f1,y.f1);
if(i < 0)return({struct _RegionHandle*_Tmp5=r;Cyc_Dict_balance(_Tmp5,({struct _tuple1 _Tmp6;_Tmp6.f1=color,({const struct Cyc_Dict_T*_Tmp7=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,a);_Tmp6.f2=_Tmp7;}),_Tmp6.f3=b,_Tmp6.f4=y;_Tmp6;}));});
if(i > 0)return({struct _RegionHandle*_Tmp5=r;Cyc_Dict_balance(_Tmp5,({struct _tuple1 _Tmp6;_Tmp6.f1=color,_Tmp6.f2=a,({const struct Cyc_Dict_T*_Tmp7=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,b);_Tmp6.f3=_Tmp7;}),_Tmp6.f4=y;_Tmp6;}));});
return({struct Cyc_Dict_T*_Tmp5=_region_malloc(r,sizeof(struct Cyc_Dict_T));_Tmp5->color=color,_Tmp5->left=a,_Tmp5->right=b,_Tmp5->key_val=key_val;_Tmp5;});}};}
# 152
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){
struct Cyc_Dict_T*ans=({struct _RegionHandle*_Tmp0=d.r;int(*_Tmp1)(void*,void*)=d.rel;struct _tuple0 _Tmp2=({struct _tuple0 _Tmp3;_Tmp3.f1=key,_Tmp3.f2=data;_Tmp3;});Cyc_Dict_ins(_Tmp0,_Tmp1,_Tmp2,d.t);});
(_check_null(ans))->color=1U;
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=d.r,_Tmp0.t=(const struct Cyc_Dict_T*)ans;_Tmp0;});}
# 159
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*key,void*data){
if(Cyc_Dict_member(d,key))
(int)_throw((void*)& Cyc_Dict_Absent_val);
return Cyc_Dict_insert(d,key,data);}
# 165
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*kds){
# 167
for(1;kds != 0;kds=kds->tl){
d=Cyc_Dict_insert(d,(((struct _tuple0*)kds->hd)[0]).f1,(((struct _tuple0*)kds->hd)[0]).f2);}
return d;}
# 172
static void*Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),const struct Cyc_Dict_T*t,void*accum){
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
if(left != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,left,accum);
accum=f(key,val,accum);
if(right != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,right,accum);
return accum;}}
# 180
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*t=d.t;
if(t == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree(f,t,accum);}
# 187
static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,const struct Cyc_Dict_T*t,void*accum){
# 189
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
if(left != (const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,left,accum);
accum=f(env,key,val,accum);
if(right != (const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,right,accum);
return accum;}}
# 198
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*t=d.t;
if(t == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree_c(f,env,t,accum);}
# 205
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,left);
f(key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,right);}}
# 212
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree(f,t);}
# 218
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,left);
f(env,key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,right);}}
# 225
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree_c(f,env,t);}
# 231
static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,left);
f(key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,right);}}
# 238
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree(f,t);}
# 244
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,left);
f(env,key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,right);}}
# 251
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree_c(f,env,t);}
# 257
static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){
*cnt=*cnt + 1;}
# 260
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){
int num=0;
({(void(*)(void(*)(int*,void*,void*),int*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_Dict_count_elem,& num,d);
return num;}struct _tuple2{void(*f1)(void*,void*);struct Cyc_Dict_Dict f2;};
# 266
static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*b1){
# 268
struct _tuple2*_Tmp0=env;struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{void(*f)(void*,void*)=_Tmp2;struct Cyc_Dict_Dict d2=_Tmp1;
({void(*_Tmp3)(void*,void*)=f;void*_Tmp4=b1;_Tmp3(_Tmp4,Cyc_Dict_lookup(d2,a));});}}
# 272
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 275
struct _tuple2 env=({struct _tuple2 _Tmp0;_Tmp0.f1=f,_Tmp0.f2=d2;_Tmp0;});
({(void(*)(void(*)(struct _tuple2*,void*,void*),struct _tuple2*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_Dict_iter2_f,& env,d1);}struct _tuple3{void(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 279
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){
# 281
struct _tuple3*_Tmp0=env;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{void(*f)(void*,void*,void*)=_Tmp3;struct Cyc_Dict_Dict d2=_Tmp2;void*inner_env=_Tmp1;
({void(*_Tmp4)(void*,void*,void*)=f;void*_Tmp5=inner_env;void*_Tmp6=b1;_Tmp4(_Tmp5,_Tmp6,Cyc_Dict_lookup(d2,a));});}}
# 285
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 288
struct _tuple3 env=({struct _tuple3 _Tmp0;_Tmp0.f1=f,_Tmp0.f2=d2,_Tmp0.f3=inner_env;_Tmp0;});
({(void(*)(void(*)(struct _tuple3*,void*,void*),struct _tuple3*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_Dict_iter2_c_f,& env,d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 292
static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*accum){
# 296
struct _tuple4*_Tmp0=env;void*_Tmp1;struct Cyc_Dict_Dict _Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{void*(*f)(void*,void*,void*,void*,void*)=_Tmp3;struct Cyc_Dict_Dict d2=_Tmp2;void*inner_env=_Tmp1;
return({void*(*_Tmp4)(void*,void*,void*,void*,void*)=f;void*_Tmp5=inner_env;void*_Tmp6=a;void*_Tmp7=b1;void*_Tmp8=Cyc_Dict_lookup(d2,a);_Tmp4(_Tmp5,_Tmp6,_Tmp7,_Tmp8,accum);});}}
# 300
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){
# 304
struct _tuple4 env=({struct _tuple4 _Tmp0;_Tmp0.f1=f,_Tmp0.f2=d2,_Tmp0.f3=inner_env;_Tmp0;});
return({(void*(*)(void*(*)(struct _tuple4*,void*,void*,void*),struct _tuple4*,struct Cyc_Dict_Dict,void*))Cyc_Dict_fold_c;})(Cyc_Dict_fold2_c_f,& env,d1,accum);}
# 308
static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,const struct Cyc_Dict_T*t){
if(t == (const struct Cyc_Dict_T*)0)return 0;else{
# 311
struct Cyc_Dict_T _stmttmp0=*t;struct Cyc_Dict_T _Tmp0=_stmttmp0;struct _tuple0 _Tmp1;const struct Cyc_Dict_T*_Tmp2;const struct Cyc_Dict_T*_Tmp3;enum Cyc_Dict_Color _Tmp4;_Tmp4=_Tmp0.color;_Tmp3=_Tmp0.left;_Tmp2=_Tmp0.right;_Tmp1=_Tmp0.key_val;{enum Cyc_Dict_Color c=_Tmp4;const struct Cyc_Dict_T*left=_Tmp3;const struct Cyc_Dict_T*right=_Tmp2;struct _tuple0 pr=_Tmp1;
const struct Cyc_Dict_T*new_left=Cyc_Dict_copy_tree(r2,left);
const struct Cyc_Dict_T*new_right=Cyc_Dict_copy_tree(r2,right);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_Tmp5=_region_malloc(r2,sizeof(struct Cyc_Dict_T));_Tmp5->color=c,_Tmp5->left=new_left,_Tmp5->right=new_right,_Tmp5->key_val=pr;_Tmp5;});}}}
# 318
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r2,({const struct Cyc_Dict_T*_Tmp1=Cyc_Dict_copy_tree(r2,d.t);_Tmp0.t=_Tmp1;});_Tmp0;});}
# 322
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){
return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}
# 326
static const struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),const struct Cyc_Dict_T*t){
# 328
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;enum Cyc_Dict_Color _Tmp5;_Tmp5=_Tmp0->color;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{enum Cyc_Dict_Color c=_Tmp5;const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
const struct Cyc_Dict_T*new_left=left == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,left);
void*new_val=f(val);
const struct Cyc_Dict_T*new_right=right == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,right);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Dict_T));_Tmp6->color=c,_Tmp6->left=new_left,_Tmp6->right=new_right,(_Tmp6->key_val).f1=key,(_Tmp6->key_val).f2=new_val;_Tmp6;});}}
# 335
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,_Tmp0.t=0;_Tmp0;});
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,({const struct Cyc_Dict_T*_Tmp1=Cyc_Dict_map_tree(r,f,t);_Tmp0.t=_Tmp1;});_Tmp0;});}
# 342
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}
# 346
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Dict_T*t){
# 348
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;enum Cyc_Dict_Color _Tmp5;_Tmp5=_Tmp0->color;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{enum Cyc_Dict_Color c=_Tmp5;const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
const struct Cyc_Dict_T*new_left=
left == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,left);
void*new_val=f(env,val);
const struct Cyc_Dict_T*new_right=
right == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,right);
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_Tmp6=_region_malloc(r,sizeof(struct Cyc_Dict_T));_Tmp6->color=c,_Tmp6->left=new_left,_Tmp6->right=new_right,(_Tmp6->key_val).f1=key,(_Tmp6->key_val).f2=new_val;_Tmp6;});}}
# 357
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
# 359
const struct Cyc_Dict_T*t=d.t;
if(t == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,_Tmp0.t=0;_Tmp0;});
return({struct Cyc_Dict_Dict _Tmp0;_Tmp0.rel=d.rel,_Tmp0.r=r,({const struct Cyc_Dict_T*_Tmp1=Cyc_Dict_map_tree_c(r,f,env,t);_Tmp0.t=_Tmp1;});_Tmp0;});}
# 365
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap_c(Cyc_Core_heap_region,f,env,d);}
# 369
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
if(d.t == (const struct Cyc_Dict_T*)0)
(int)_throw((void*)& Cyc_Dict_Absent_val);
return({struct _tuple0*_Tmp0=_region_malloc(r,sizeof(struct _tuple0));_Tmp0->f1=((d.t)->key_val).f1,_Tmp0->f2=((d.t)->key_val).f2;_Tmp0;});}
# 375
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_Tmp0=t;void*_Tmp1;void*_Tmp2;const struct Cyc_Dict_T*_Tmp3;const struct Cyc_Dict_T*_Tmp4;_Tmp4=_Tmp0->left;_Tmp3=_Tmp0->right;_Tmp2=(_Tmp0->key_val).f1;_Tmp1=(_Tmp0->key_val).f2;{const struct Cyc_Dict_T*left=_Tmp4;const struct Cyc_Dict_T*right=_Tmp3;void*key=_Tmp2;void*val=_Tmp1;
return
((left == (const struct Cyc_Dict_T*)0 || Cyc_Dict_forall_tree_c(f,env,left))&&
 f(env,key,val))&&(
right == (const struct Cyc_Dict_T*)0 || Cyc_Dict_forall_tree_c(f,env,right));}}
# 383
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
if(t == (const struct Cyc_Dict_T*)0)
return 1;
return Cyc_Dict_forall_tree_c(f,env,t);}struct _tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;};
# 390
static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
# 392
struct _tuple5*_Tmp0=env;struct Cyc_Dict_Dict _Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{int(*f)(void*,void*,void*)=_Tmp2;struct Cyc_Dict_Dict d2=_Tmp1;
if(Cyc_Dict_member(d2,a))
return({int(*_Tmp3)(void*,void*,void*)=f;void*_Tmp4=a;void*_Tmp5=b;_Tmp3(_Tmp4,_Tmp5,Cyc_Dict_lookup(d2,a));});
return 1;}}
# 399
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
struct _tuple5 env=({struct _tuple5 _Tmp0;_Tmp0.f1=f,_Tmp0.f2=d2;_Tmp0;});
return({(int(*)(int(*)(struct _tuple5*,void*,void*),struct _tuple5*,struct Cyc_Dict_Dict))Cyc_Dict_forall_c;})(Cyc_Dict_forall_intersect_f,& env,d1);}struct _tuple6{void*(*f1)(void*,void*,void*,void*);void*f2;};
# 405
static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){
# 409
if(Cyc_Dict_member(*d1,a)){
# 411
void*old_val=Cyc_Dict_lookup(*d1,a);
void*new_val=(((*env).f1))((*env).f2,a,old_val,b);
if(new_val != old_val)
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_insert(*d1,a,new_val);*d1=_Tmp0;});
return d1;}
# 417
({struct Cyc_Dict_Dict _Tmp0=Cyc_Dict_insert(*d1,a,b);*d1=_Tmp0;});
return d1;}
# 421
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 424
if((int)d1.t == (int)d2.t)return d1;
if(d1.t == (const struct Cyc_Dict_T*)0)return d2;
if(d2.t == (const struct Cyc_Dict_T*)0)return d1;{
struct _tuple6 fenvpair=({struct _tuple6 _Tmp0;_Tmp0.f1=f,_Tmp0.f2=env;_Tmp0;});
({(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;})(Cyc_Dict_union_f,& fenvpair,d2,& d1);
return d1;}}
# 432
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 435
const struct Cyc_Dict_T*t2=d2.t;
if(t2 == (const struct Cyc_Dict_T*)0)return d2;
if((int)d1.t == (int)t2)return d2;{
const struct Cyc_Dict_T*ans_tree=0;
struct _RegionHandle _Tmp0=_new_region("temp");struct _RegionHandle*temp=& _Tmp0;_push_region(temp);{
# 441
struct _fat_ptr queue=_tag_fat(({unsigned _Tmp1=16U;({const struct Cyc_Dict_T**_Tmp2=({struct _RegionHandle*_Tmp3=temp;_region_malloc(_Tmp3,_check_times(_Tmp1,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _Tmp3=16U;unsigned i;for(i=0;i < _Tmp3;++ i){_Tmp2[i]=t2;}}0;});_Tmp2;});}),sizeof(const struct Cyc_Dict_T*),16U);
int ind=0;
while(ind != -1){
const struct Cyc_Dict_T*_stmttmp1=((const struct Cyc_Dict_T**)queue.curr)[ind --];const struct Cyc_Dict_T*_Tmp1=_stmttmp1;void*_Tmp2;void*_Tmp3;const struct Cyc_Dict_T*_Tmp4;const struct Cyc_Dict_T*_Tmp5;_Tmp5=_Tmp1->left;_Tmp4=_Tmp1->right;_Tmp3=(_Tmp1->key_val).f1;_Tmp2=(_Tmp1->key_val).f2;{const struct Cyc_Dict_T*l=_Tmp5;const struct Cyc_Dict_T*r=_Tmp4;void*k=_Tmp3;void*v=_Tmp2;
if((unsigned)(ind + 2)>= _get_fat_size(queue,sizeof(const struct Cyc_Dict_T*)))
queue=({unsigned _Tmp6=_get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))* 2U;_tag_fat(({const struct Cyc_Dict_T**_Tmp7=({struct _RegionHandle*_Tmp8=temp;_region_malloc(_Tmp8,_check_times(_Tmp6,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _Tmp8=_get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))* 2U;unsigned i;for(i=0;i < _Tmp8;++ i){i < _get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))?_Tmp7[i]=((const struct Cyc_Dict_T**)queue.curr)[(int)i]:(_Tmp7[i]=t2);}}0;});_Tmp7;}),sizeof(const struct Cyc_Dict_T*),_Tmp6);});
# 448
if(l != (const struct Cyc_Dict_T*)0)((const struct Cyc_Dict_T**)queue.curr)[++ ind]=l;
if(r != (const struct Cyc_Dict_T*)0)((const struct Cyc_Dict_T**)queue.curr)[++ ind]=r;
if(Cyc_Dict_member(d1,k))
ans_tree=(const struct Cyc_Dict_T*)({struct _RegionHandle*_Tmp6=d2.r;int(*_Tmp7)(void*,void*)=d2.rel;struct _tuple0 _Tmp8=({struct _tuple0 _Tmp9;_Tmp9.f1=k,({void*_TmpA=({void*(*_TmpB)(void*,void*,void*,void*)=f;void*_TmpC=env;void*_TmpD=k;void*_TmpE=Cyc_Dict_lookup(d1,k);_TmpB(_TmpC,_TmpD,_TmpE,v);});_Tmp9.f2=_TmpA;});_Tmp9;});Cyc_Dict_ins(_Tmp6,_Tmp7,_Tmp8,ans_tree);});}}}{
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
return({(struct Cyc_Dict_Dict(*)(void*(*)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;})(Cyc_Dict_intersect_f,f,d1,d2);}
# 467
static struct Cyc_List_List*Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
# 469
return({struct Cyc_List_List*_Tmp0=_region_malloc(r,sizeof(struct Cyc_List_List));({struct _tuple0*_Tmp1=({struct _tuple0*_Tmp2=_region_malloc(r,sizeof(struct _tuple0));_Tmp2->f1=k,_Tmp2->f2=v;_Tmp2;});_Tmp0->hd=_Tmp1;}),_Tmp0->tl=accum;_Tmp0;});}
# 472
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold_c;})(Cyc_Dict_to_list_f,r,d,0);}
# 475
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d){
return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,void*);struct _RegionHandle*f2;};
# 479
static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 481
struct _tuple7*_Tmp0=env;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{int(*f)(void*,void*)=_Tmp2;struct _RegionHandle*r=_Tmp1;
if(f(x,y))
({struct Cyc_Dict_Dict _Tmp3=Cyc_Dict_insert(*acc,x,y);*acc=_Tmp3;});
# 485
return acc;}}
# 488
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict d){
struct _tuple7 env=({struct _tuple7 _Tmp0;_Tmp0.f1=f,_Tmp0.f2=r2;_Tmp0;});
struct Cyc_Dict_Dict res=Cyc_Dict_rempty(r2,d.rel);
return*({(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;})(Cyc_Dict_filter_f,& env,d,& res);}
# 499
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,void*,void*);void*f2;struct _RegionHandle*f3;};
# 504
static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 507
struct _tuple8*_Tmp0=env;void*_Tmp1;void*_Tmp2;void*_Tmp3;_Tmp3=_Tmp0->f1;_Tmp2=_Tmp0->f2;_Tmp1=_Tmp0->f3;{int(*f)(void*,void*,void*)=_Tmp3;void*f_env=_Tmp2;struct _RegionHandle*r2=_Tmp1;
if(f(f_env,x,y))
({struct Cyc_Dict_Dict _Tmp4=Cyc_Dict_insert(*acc,x,y);*acc=_Tmp4;});
return acc;}}
# 513
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 515
struct _tuple8 env=({struct _tuple8 _Tmp0;_Tmp0.f1=f,_Tmp0.f2=f_env,_Tmp0.f3=r2;_Tmp0;});
struct Cyc_Dict_Dict res=Cyc_Dict_rempty(r2,d.rel);
return*({(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;})(Cyc_Dict_filter_c_f,& env,d,& res);}
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
return({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;})(r2,Cyc_Dict_difference_f,& d2,d1);}
# 533
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*f1)(void*,void*);void*f2;};
# 537
static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,void*y){
struct _tuple9*_Tmp0=env;void*_Tmp1;void*_Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=_Tmp0->f2;{int(*rel)(void*,void*)=_Tmp2;void*x0=_Tmp1;
return rel(x0,x)!= 0;}}
# 542
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(r2,d);{
struct _tuple9 env=({struct _tuple9 _Tmp0;_Tmp0.f1=d.rel,_Tmp0.f2=x;_Tmp0;});
return({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;})(r2,Cyc_Dict_delete_f,& env,d);}}
# 548
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return d;{
struct _tuple9 env=({struct _tuple9 _Tmp0;_Tmp0.f1=d.rel,_Tmp0.f2=x;_Tmp0;});
return({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;})(d.r,Cyc_Dict_delete_f,& env,d);}}
# 554
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct _fat_ptr f1;int f2;};
# 558
int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){
# 561
struct _tuple10*_Tmp0=stk;void*_Tmp1;struct _fat_ptr _Tmp2;_Tmp2=_Tmp0->f1;_Tmp1=(int*)& _Tmp0->f2;{struct _fat_ptr stack=_Tmp2;int*indp=(int*)_Tmp1;
int ind=*indp;
if(ind == -1)
return 0;{
const struct Cyc_Dict_T*t=((const struct Cyc_Dict_T**)stack.curr)[ind];
*dest=(_check_null(t))->key_val;
-- ind;
if((unsigned)t->left)
((const struct Cyc_Dict_T**)stack.curr)[++ ind]=t->left;
if((unsigned)t->right)
((const struct Cyc_Dict_T**)stack.curr)[++ ind]=t->right;
*indp=ind;
return 1;}}}
# 576
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d){
# 581
int half_max_size=1;
const struct Cyc_Dict_T*t=d.t;
while(t != (const struct Cyc_Dict_T*)0){
t=t->left;
++ half_max_size;}
# 587
t=d.t;{
struct _fat_ptr stack=({unsigned _Tmp0=(unsigned)(2 * half_max_size);_tag_fat(({const struct Cyc_Dict_T**_Tmp1=({struct _RegionHandle*_Tmp2=rgn;_region_malloc(_Tmp2,_check_times(_Tmp0,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _Tmp2=(unsigned)(2 * half_max_size);unsigned i;for(i=0;i < _Tmp2;++ i){_Tmp1[i]=t;}}0;});_Tmp1;}),sizeof(const struct Cyc_Dict_T*),_Tmp0);});
return({struct Cyc_Iter_Iter _Tmp0;({struct _tuple10*_Tmp1=({struct _tuple10*_Tmp2=_region_malloc(rgn,sizeof(struct _tuple10));_Tmp2->f1=stack,(unsigned)t?_Tmp2->f2=0:(_Tmp2->f2=-1);_Tmp2;});_Tmp0.env=_Tmp1;}),_Tmp0.next=Cyc_Dict_iter_f;_Tmp0;});}}
# 597
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){
# 604
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);
int len=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(dict_list);
# 608
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=({const char*_Tmp1="Dict::marshal: Write failure";_tag_fat(_Tmp1,sizeof(char),29U);});_Tmp0;}));
# 610
while(dict_list != 0){
env=({(void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_key;})(env,fp,(struct _tuple0*)dict_list->hd);
env=({(void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_val;})(env,fp,(struct _tuple0*)dict_list->hd);
dict_list=dict_list->tl;}
# 615
return env;}
# 618
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp){
# 625
struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);
int len=Cyc_getw(fp);
if(len == -1)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_Tmp0=_cycalloc(sizeof(struct Cyc_Core_Failure_exn_struct));_Tmp0->tag=Cyc_Core_Failure,_Tmp0->f1=({const char*_Tmp1="Dict::unmarshal: list length is -1";_tag_fat(_Tmp1,sizeof(char),35U);});_Tmp0;}));
{int i=0;for(0;i < len;++ i){
void*key=read_key(env,fp);
void*val=read_val(env,fp);
dict=Cyc_Dict_insert(dict,key,val);}}
# 634
return dict;}
