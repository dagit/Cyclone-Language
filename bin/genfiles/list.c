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
 struct Cyc_Core_Opt{void*v;};extern char Cyc_Core_Invalid_argument[17U];struct Cyc_Core_Invalid_argument_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Failure[8U];struct Cyc_Core_Failure_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Impossible[11U];struct Cyc_Core_Impossible_exn_struct{char*tag;struct _fat_ptr f1;};extern char Cyc_Core_Not_found[10U];struct Cyc_Core_Not_found_exn_struct{char*tag;};
# 149 "core.h"
extern struct Cyc_Core_Not_found_exn_struct Cyc_Core_Not_found_val;extern char Cyc_Core_Unreachable[12U];struct Cyc_Core_Unreachable_exn_struct{char*tag;struct _fat_ptr f1;};
# 168
extern struct _RegionHandle*Cyc_Core_heap_region;
# 171
extern struct _RegionHandle*Cyc_Core_unique_region;struct Cyc_Core_DynamicRegion;struct Cyc_Core_NewDynamicRegion{struct Cyc_Core_DynamicRegion*key;};struct Cyc_Core_ThinRes{void*arr;unsigned nelts;};struct Cyc_List_List{void*hd;struct Cyc_List_List*tl;};
# 61 "list.h"
int Cyc_List_length(struct Cyc_List_List*);
# 72
struct Cyc_List_List*Cyc_List_rcopy(struct _RegionHandle*,struct Cyc_List_List*);
# 79
struct Cyc_List_List*Cyc_List_rmap(struct _RegionHandle*,void*(*)(void*),struct Cyc_List_List*);
# 86
struct Cyc_List_List*Cyc_List_rmap_c(struct _RegionHandle*,void*(*)(void*,void*),void*,struct Cyc_List_List*);extern char Cyc_List_List_mismatch[14U];struct Cyc_List_List_mismatch_exn_struct{char*tag;};
# 100
struct Cyc_List_List*Cyc_List_rmap2(struct _RegionHandle*,void*(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 112
struct Cyc_List_List*Cyc_List_rmap3(struct _RegionHandle*,void*(*)(void*,void*,void*),struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);
# 153
void*Cyc_List_fold_right(void*(*)(void*,void*),struct Cyc_List_List*,void*);
# 157
void*Cyc_List_fold_right_c(void*(*)(void*,void*,void*),void*,struct Cyc_List_List*,void*);
# 167
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 175
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*,struct Cyc_List_List*);
# 178
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*);
# 190
struct Cyc_List_List*Cyc_List_rappend(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 205
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*,struct Cyc_List_List*);
# 214
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*,int(*)(void*,void*),struct Cyc_List_List*);
# 220
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*)(void*,void*),struct Cyc_List_List*);
# 230
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*,int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);
# 234
struct Cyc_List_List*Cyc_List_imp_merge(int(*)(void*,void*),struct Cyc_List_List*,struct Cyc_List_List*);extern char Cyc_List_Nth[4U];struct Cyc_List_Nth_exn_struct{char*tag;};
# 246
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*,int);
# 276
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*);
# 288
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*,struct Cyc_List_List*);struct _tuple0{struct Cyc_List_List*f1;struct Cyc_List_List*f2;};
# 303
struct _tuple0 Cyc_List_rsplit(struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*);struct _tuple1{struct Cyc_List_List*f1;struct Cyc_List_List*f2;struct Cyc_List_List*f3;};
# 310
struct _tuple1 Cyc_List_rsplit3(struct _RegionHandle*,struct _RegionHandle*,struct _RegionHandle*,struct Cyc_List_List*);
# 354
struct Cyc_List_List*Cyc_List_delete_cmp(int(*)(void*,void*),struct Cyc_List_List*,void*);
# 367
struct _fat_ptr Cyc_List_rto_array(struct _RegionHandle*,struct Cyc_List_List*);
# 371
struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr);
# 374
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*,struct _fat_ptr);
# 380
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*,int,void*(*)(int));
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*,int,void*(*)(void*,int),void*);
# 397
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*,int(*)(void*),struct Cyc_List_List*);
# 400
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*,int(*)(void*,void*),void*,struct Cyc_List_List*);
# 30 "list.cyc"
int Cyc_List_length(struct Cyc_List_List*x){
int i=0;
for(1;x != 0;x=x->tl){
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
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp0=_region_malloc(r,sizeof(*_tmp0));_tmp0->hd=x->hd,_tmp0->tl=0;_tmp0;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmp1=_region_malloc(r,sizeof(*_tmp1));_tmp1->hd=x->hd,_tmp1->tl=0;_tmp1;});
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
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp2=_region_malloc(r,sizeof(*_tmp2));({void*_tmp37=f(x->hd);_tmp2->hd=_tmp37;}),_tmp2->tl=0;_tmp2;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmp3=_region_malloc(r,sizeof(*_tmp3));({void*_tmp38=f(x->hd);_tmp3->hd=_tmp38;}),_tmp3->tl=0;_tmp3;});
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
if(x == 0)return 0;
result=({struct Cyc_List_List*_tmp4=_region_malloc(r,sizeof(*_tmp4));({void*_tmp39=f(env,x->hd);_tmp4->hd=_tmp39;}),_tmp4->tl=0;_tmp4;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*e=({struct Cyc_List_List*_tmp5=_region_malloc(r,sizeof(*_tmp5));({void*_tmp3A=f(env,x->hd);_tmp5->hd=_tmp3A;}),_tmp5->tl=0;_tmp5;});
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
if(x == 0 && y == 0)return 0;
if(x == 0 || y == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 127
result=({struct Cyc_List_List*_tmp6=_region_malloc(r,sizeof(*_tmp6));({void*_tmp3B=f(x->hd,y->hd);_tmp6->hd=_tmp3B;}),_tmp6->tl=0;_tmp6;});
prev=result;
x=x->tl;
y=y->tl;
# 132
while(x != 0 && y != 0){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmp7=_region_malloc(r,sizeof(*_tmp7));({void*_tmp3C=f(x->hd,y->hd);_tmp7->hd=_tmp3C;}),_tmp7->tl=0;_tmp7;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;}
# 139
if(x != 0 || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 143
struct Cyc_List_List*Cyc_List_map2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rmap2(Cyc_Core_heap_region,f,x,y);}
# 148
struct Cyc_List_List*Cyc_List_rmap3(struct _RegionHandle*r,void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 150
struct Cyc_List_List*result;struct Cyc_List_List*prev;
# 152
if((x == 0 && y == 0)&& z == 0)return 0;
if((x == 0 || y == 0)|| z == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 155
result=({struct Cyc_List_List*_tmp8=_region_malloc(r,sizeof(*_tmp8));({void*_tmp3D=f(x->hd,y->hd,z->hd);_tmp8->hd=_tmp3D;}),_tmp8->tl=0;_tmp8;});
prev=result;
# 158
x=x->tl;
y=y->tl;
z=z->tl;
# 162
while((x != 0 && y != 0)&& z != 0){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmp9=_region_malloc(r,sizeof(*_tmp9));({void*_tmp3E=f(x->hd,y->hd,z->hd);_tmp9->hd=_tmp3E;}),_tmp9->tl=0;_tmp9;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;
z=z->tl;}
# 170
if((x != 0 || y != 0)|| z != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 174
struct Cyc_List_List*Cyc_List_map3(void*(*f)(void*,void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
return Cyc_List_rmap3(Cyc_Core_heap_region,f,x,y,z);}
# 181
void Cyc_List_app(void*(*f)(void*),struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
f(x->hd);}}
# 186
void Cyc_List_app_c(void*(*f)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
f(env,x->hd);}}
# 194
void Cyc_List_iter(void(*f)(void*),struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
f(x->hd);}}
# 199
void Cyc_List_iter_c(void(*f)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
f(env,x->hd);}}
# 206
void Cyc_List_app2(void*(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
for(1;x != 0 && y != 0;(x=x->tl,y=y->tl)){
f(x->hd,y->hd);}
if(x != 0 || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 212
void Cyc_List_app2_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
for(1;x != 0 && y != 0;(x=x->tl,y=y->tl)){
f(env,x->hd,y->hd);}
if(x != 0 || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 220
void Cyc_List_iter2(void(*f)(void*,void*),struct Cyc_List_List*x,struct Cyc_List_List*y){
for(1;x != 0 && y != 0;(x=x->tl,y=y->tl)){
f(x->hd,y->hd);}
if(x != 0 || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 225
void Cyc_List_iter2_c(void(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,struct Cyc_List_List*y){
for(1;x != 0 && y != 0;(x=x->tl,y=y->tl)){
f(env,x->hd,y->hd);}
if(x != 0 || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);}
# 234
void*Cyc_List_fold_left(void*(*f)(void*,void*),void*accum,struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
accum=f(accum,x->hd);}
return accum;}
# 240
void*Cyc_List_fold_left_c(void*(*f)(void*,void*,void*),void*env,void*accum,struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
accum=f(env,accum,x->hd);}
return accum;}
# 249
void*Cyc_List_fold_right(void*(*f)(void*,void*),struct Cyc_List_List*x,void*accum){
if(x == 0)return accum;
return({void*(*_tmp40)(void*,void*)=f;void*_tmp3F=x->hd;_tmp40(_tmp3F,Cyc_List_fold_right(f,x->tl,accum));});}
# 253
void*Cyc_List_fold_right_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_List_List*x,void*accum){
if(x == 0)return accum;
return({void*(*_tmp43)(void*,void*,void*)=f;void*_tmp42=env;void*_tmp41=x->hd;_tmp43(_tmp42,_tmp41,Cyc_List_fold_right_c(f,env,x->tl,accum));});}
# 260
struct Cyc_List_List*Cyc_List_rrevappend(struct _RegionHandle*r2,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 262
for(1;x != 0;x=x->tl){
y=({struct Cyc_List_List*_tmpA=_region_malloc(r2,sizeof(*_tmpA));_tmpA->hd=x->hd,_tmpA->tl=y;_tmpA;});}
return y;}
# 267
struct Cyc_List_List*Cyc_List_revappend(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rrevappend(Cyc_Core_heap_region,x,y);}
# 272
struct Cyc_List_List*Cyc_List_rrev(struct _RegionHandle*r2,struct Cyc_List_List*x){
return x == 0?0: Cyc_List_rrevappend(r2,x,0);}
# 276
struct Cyc_List_List*Cyc_List_rev(struct Cyc_List_List*x){
return Cyc_List_rrev(Cyc_Core_heap_region,x);}
# 281
struct Cyc_List_List*Cyc_List_imp_rev(struct Cyc_List_List*x){
if(x == 0)return x;{
struct Cyc_List_List*first=x;
struct Cyc_List_List*second=x->tl;
x->tl=0;
while(second != 0){
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
if(x == 0)return y;
if(y == 0)return Cyc_List_rcopy(r2,x);
result=({struct Cyc_List_List*_tmpB=_region_malloc(r2,sizeof(*_tmpB));_tmpB->hd=x->hd,_tmpB->tl=0;_tmpB;});
prev=result;
for(x=x->tl;x != 0;x=x->tl){
struct Cyc_List_List*temp=({struct Cyc_List_List*_tmpC=_region_malloc(r2,sizeof(*_tmpC));_tmpC->hd=x->hd,_tmpC->tl=0;_tmpC;});
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
if(x == 0)return y;
if(y == 0)return x;
for(z=x;((struct Cyc_List_List*)_check_null(z))->tl != 0;z=z->tl){
;}
z->tl=y;
return x;}
# 330
struct Cyc_List_List*Cyc_List_rflatten(struct _RegionHandle*r,struct Cyc_List_List*x){
return((struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_List_List*,struct Cyc_List_List*))Cyc_List_fold_right_c)(Cyc_List_rappend,r,x,0);}
# 334
struct Cyc_List_List*Cyc_List_flatten(struct Cyc_List_List*x){
return Cyc_List_rflatten(Cyc_Core_heap_region,x);}
# 339
struct Cyc_List_List*Cyc_List_imp_merge(int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 342
if(a == 0)return b;
if(b == 0)return a;
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
while(a != 0 && b != 0){
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
c->tl=a == 0?b: a;
return d;}
# 376
struct Cyc_List_List*Cyc_List_rimp_merge_sort(int(*less_eq)(void*,void*),struct Cyc_List_List*x){
if(x == 0)return x;{
struct Cyc_List_List*temp=x->tl;
if(temp == 0)return x;{
# 382
struct Cyc_List_List*a=x;struct Cyc_List_List*aptr=a;
struct Cyc_List_List*b=temp;struct Cyc_List_List*bptr=b;
x=b->tl;
while(x != 0){
aptr->tl=x;
aptr=x;
x=x->tl;
if(x != 0){
bptr->tl=x;
bptr=x;
x=x->tl;}}
# 395
aptr->tl=0;
bptr->tl=0;
return({int(*_tmp45)(void*,void*)=less_eq;struct Cyc_List_List*_tmp44=
Cyc_List_rimp_merge_sort(less_eq,a);
# 397
Cyc_List_imp_merge(_tmp45,_tmp44,
Cyc_List_rimp_merge_sort(less_eq,b));});}}}
# 405
struct Cyc_List_List*Cyc_List_rmerge_sort(struct _RegionHandle*r,int(*less_eq)(void*,void*),struct Cyc_List_List*x){
return({int(*_tmp46)(void*,void*)=less_eq;Cyc_List_rimp_merge_sort(_tmp46,Cyc_List_rcopy(r,x));});}
# 410
struct Cyc_List_List*Cyc_List_rmerge(struct _RegionHandle*r,int(*less_eq)(void*,void*),struct Cyc_List_List*a,struct Cyc_List_List*b){
# 412
struct Cyc_List_List*c;struct Cyc_List_List*d;
# 415
if(a == 0)return Cyc_List_rcopy(r,b);
if(b == 0)return Cyc_List_rcopy(r,a);
# 422
if(less_eq(a->hd,b->hd)<= 0){
c=({struct Cyc_List_List*_tmpD=_region_malloc(r,sizeof(*_tmpD));_tmpD->hd=a->hd,_tmpD->tl=0;_tmpD;});
a=a->tl;}else{
# 426
c=({struct Cyc_List_List*_tmpE=_region_malloc(r,sizeof(*_tmpE));_tmpE->hd=b->hd,_tmpE->tl=0;_tmpE;});
b=b->tl;}
# 429
d=c;
# 431
while(a != 0 && b != 0){
# 433
struct Cyc_List_List*temp;
if(less_eq(a->hd,b->hd)<= 0){
temp=({struct Cyc_List_List*_tmpF=_region_malloc(r,sizeof(*_tmpF));_tmpF->hd=a->hd,_tmpF->tl=0;_tmpF;});
c->tl=temp;
c=temp;
a=a->tl;}else{
# 440
temp=({struct Cyc_List_List*_tmp10=_region_malloc(r,sizeof(*_tmp10));_tmp10->hd=b->hd,_tmp10->tl=0;_tmp10;});
c->tl=temp;
c=temp;
b=b->tl;}}
# 447
({struct Cyc_List_List*_tmp47=a == 0?Cyc_List_rcopy(r,b): Cyc_List_rcopy(r,a);c->tl=_tmp47;});
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
return((struct Cyc_List_List*)_check_null(Cyc_List_nth_tail(x,i)))->hd;}
# 472
struct Cyc_List_List*Cyc_List_nth_tail(struct Cyc_List_List*x,int i){
if(i < 0)(int)_throw((void*)& Cyc_List_Nth_val);
for(1;i > 0;(-- i,x=x->tl)){
if(x == 0)
(int)_throw((void*)& Cyc_List_Nth_val);}
return x;}
# 482
int Cyc_List_forall(int(*pred)(void*),struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
if(!pred(x->hd))
return 0;}
return 1;}
# 488
int Cyc_List_forall_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
if(!pred(env,x->hd))
return 0;}
return 1;}
# 497
int Cyc_List_exists(int(*pred)(void*),struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
if(pred(x->hd))
return 1;}
return 0;}
# 503
int Cyc_List_exists_c(int(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
if(pred(env,x->hd))
return 1;}
return 0;}
# 510
void*Cyc_List_find_c(void*(*pred)(void*,void*),void*env,struct Cyc_List_List*x){
for(1;x != 0;x=x->tl){
void*_tmp11=pred(env,x->hd);void*v=_tmp11;
if((unsigned)v)return v;}
# 515
return 0;}struct _tuple2{void*f1;void*f2;};
# 520
struct Cyc_List_List*Cyc_List_rzip(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y){
# 522
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 524
if(x == 0 && y == 0)return 0;
if(x == 0 || y == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 527
result=({struct Cyc_List_List*_tmp13=_region_malloc(r3,sizeof(*_tmp13));({struct _tuple2*_tmp48=({struct _tuple2*_tmp12=_region_malloc(r4,sizeof(*_tmp12));_tmp12->f1=x->hd,_tmp12->f2=y->hd;_tmp12;});_tmp13->hd=_tmp48;}),_tmp13->tl=0;_tmp13;});
prev=result;
# 530
x=x->tl;
y=y->tl;
# 533
while(x != 0 && y != 0){
temp=({struct Cyc_List_List*_tmp15=_region_malloc(r3,sizeof(*_tmp15));({struct _tuple2*_tmp49=({struct _tuple2*_tmp14=_region_malloc(r4,sizeof(*_tmp14));_tmp14->f1=x->hd,_tmp14->f2=y->hd;_tmp14;});_tmp15->hd=_tmp49;}),_tmp15->tl=0;_tmp15;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;}
# 540
if(x != 0 || y != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
return result;}
# 544
struct Cyc_List_List*Cyc_List_zip(struct Cyc_List_List*x,struct Cyc_List_List*y){
return Cyc_List_rzip(Cyc_Core_heap_region,Cyc_Core_heap_region,x,y);}struct _tuple3{void*f1;void*f2;void*f3;};
# 548
struct Cyc_List_List*Cyc_List_rzip3(struct _RegionHandle*r3,struct _RegionHandle*r4,struct Cyc_List_List*x,struct Cyc_List_List*y,struct Cyc_List_List*z){
# 550
struct Cyc_List_List*result;struct Cyc_List_List*prev;struct Cyc_List_List*temp;
# 552
if((x == 0 && y == 0)&& z == 0)return 0;
if((x == 0 || y == 0)|| z == 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
# 555
result=({struct Cyc_List_List*_tmp17=_region_malloc(r3,sizeof(*_tmp17));({struct _tuple3*_tmp4A=({struct _tuple3*_tmp16=_region_malloc(r4,sizeof(*_tmp16));_tmp16->f1=x->hd,_tmp16->f2=y->hd,_tmp16->f3=z->hd;_tmp16;});_tmp17->hd=_tmp4A;}),_tmp17->tl=0;_tmp17;});
prev=result;
# 558
x=x->tl;
y=y->tl;
z=z->tl;
# 562
while((x != 0 && y != 0)&& z != 0){
temp=({struct Cyc_List_List*_tmp19=_region_malloc(r3,sizeof(*_tmp19));({struct _tuple3*_tmp4B=({struct _tuple3*_tmp18=_region_malloc(r4,sizeof(*_tmp18));_tmp18->f1=x->hd,_tmp18->f2=y->hd,_tmp18->f3=z->hd;_tmp18;});_tmp19->hd=_tmp4B;}),_tmp19->tl=0;_tmp19;});
prev->tl=temp;
prev=temp;
x=x->tl;
y=y->tl;
z=z->tl;}
# 570
if((x != 0 || y != 0)|| z != 0)(int)_throw((void*)& Cyc_List_List_mismatch_val);
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
if(x == 0)return({struct _tuple0 _tmp32;_tmp32.f1=0,_tmp32.f2=0;_tmp32;});
# 587
prev1=(result1=({struct Cyc_List_List*_tmp1A=_region_malloc(r3,sizeof(*_tmp1A));_tmp1A->hd=(((struct _tuple2*)x->hd)[0]).f1,_tmp1A->tl=0;_tmp1A;}));
prev2=(result2=({struct Cyc_List_List*_tmp1B=_region_malloc(r4,sizeof(*_tmp1B));_tmp1B->hd=(((struct _tuple2*)x->hd)[0]).f2,_tmp1B->tl=0;_tmp1B;}));
# 590
for(x=x->tl;x != 0;x=x->tl){
temp1=({struct Cyc_List_List*_tmp1C=_region_malloc(r3,sizeof(*_tmp1C));_tmp1C->hd=(((struct _tuple2*)x->hd)[0]).f1,_tmp1C->tl=0;_tmp1C;});
temp2=({struct Cyc_List_List*_tmp1D=_region_malloc(r4,sizeof(*_tmp1D));_tmp1D->hd=(((struct _tuple2*)x->hd)[0]).f2,_tmp1D->tl=0;_tmp1D;});
prev1->tl=temp1;
prev2->tl=temp2;
prev1=temp1;
prev2=temp2;}
# 598
return({struct _tuple0 _tmp33;_tmp33.f1=result1,_tmp33.f2=result2;_tmp33;});}
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
if(x == 0)return({struct _tuple1 _tmp34;_tmp34.f1=0,_tmp34.f2=0,_tmp34.f3=0;_tmp34;});
# 616
prev1=(result1=({struct Cyc_List_List*_tmp1E=_region_malloc(r3,sizeof(*_tmp1E));_tmp1E->hd=(((struct _tuple3*)x->hd)[0]).f1,_tmp1E->tl=0;_tmp1E;}));
prev2=(result2=({struct Cyc_List_List*_tmp1F=_region_malloc(r4,sizeof(*_tmp1F));_tmp1F->hd=(((struct _tuple3*)x->hd)[0]).f2,_tmp1F->tl=0;_tmp1F;}));
prev3=(result3=({struct Cyc_List_List*_tmp20=_region_malloc(r5,sizeof(*_tmp20));_tmp20->hd=(((struct _tuple3*)x->hd)[0]).f3,_tmp20->tl=0;_tmp20;}));
# 620
for(x=x->tl;x != 0;x=x->tl){
temp1=({struct Cyc_List_List*_tmp21=_region_malloc(r3,sizeof(*_tmp21));_tmp21->hd=(((struct _tuple3*)x->hd)[0]).f1,_tmp21->tl=0;_tmp21;});
temp2=({struct Cyc_List_List*_tmp22=_region_malloc(r4,sizeof(*_tmp22));_tmp22->hd=(((struct _tuple3*)x->hd)[0]).f2,_tmp22->tl=0;_tmp22;});
temp3=({struct Cyc_List_List*_tmp23=_region_malloc(r5,sizeof(*_tmp23));_tmp23->hd=(((struct _tuple3*)x->hd)[0]).f3,_tmp23->tl=0;_tmp23;});
prev1->tl=temp1;
prev2->tl=temp2;
prev3->tl=temp3;
prev1=temp1;
prev2=temp2;
prev3=temp3;}
# 631
return({struct _tuple1 _tmp35;_tmp35.f1=result1,_tmp35.f2=result2,_tmp35.f3=result3;_tmp35;});}
# 634
struct _tuple1 Cyc_List_split3(struct Cyc_List_List*x){
return Cyc_List_rsplit3(Cyc_Core_heap_region,Cyc_Core_heap_region,Cyc_Core_heap_region,x);}
# 640
int Cyc_List_memq(struct Cyc_List_List*l,void*x){
for(1;l != 0;l=l->tl){
if(l->hd == x)
return 1;}
return 0;}
# 647
int Cyc_List_mem(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
for(1;l != 0;l=l->tl){
if(cmp(l->hd,x)== 0)
return 1;}
return 0;}
# 657
void*Cyc_List_assoc(struct Cyc_List_List*l,void*x){
for(1;l != 0;l=l->tl){
if((((struct _tuple2*)l->hd)[0]).f1 == x)
return(((struct _tuple2*)l->hd)[0]).f2;}
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 664
void*Cyc_List_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
for(1;l != 0;l=l->tl){
if(cmp((*((struct _tuple2*)l->hd)).f1,x)== 0)
return(*((struct _tuple2*)l->hd)).f2;}
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 675
struct Cyc_List_List*Cyc_List_delete_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
struct Cyc_List_List*prev=0;
struct Cyc_List_List*_tmp24=l;struct Cyc_List_List*iter=_tmp24;
while(iter != 0){
if(cmp(iter->hd,x)== 0){
if(prev == 0)
return((struct Cyc_List_List*)_check_null(l))->tl;
prev->tl=iter->tl;
return l;}
# 685
prev=iter;
iter=iter->tl;}
# 688
(int)_throw((void*)& Cyc_Core_Not_found_val);}
# 691
static int Cyc_List_ptrequal(void*x,void*y){
return !(x == y);}
# 698
struct Cyc_List_List*Cyc_List_delete(struct Cyc_List_List*l,void*x){
return Cyc_List_delete_cmp(Cyc_List_ptrequal,l,x);}
# 704
int Cyc_List_mem_assoc(struct Cyc_List_List*l,void*x){
for(1;l != 0;l=l->tl){
if((((struct _tuple2*)l->hd)[0]).f1 == x)
return 1;}
return 0;}
# 713
int Cyc_List_mem_assoc_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l,void*x){
for(1;l != 0;l=l->tl){
if(cmp((((struct _tuple2*)l->hd)[0]).f1,x)== 0)
return 1;}
return 0;}
# 722
struct Cyc_Core_Opt*Cyc_List_check_unique(int(*cmp)(void*,void*),struct Cyc_List_List*x){
while(x != 0){
void*_tmp25=x->hd;void*hd=_tmp25;
x=x->tl;
if(x != 0 && cmp(hd,x->hd)== 0)
return({struct Cyc_Core_Opt*_tmp26=_cycalloc(sizeof(*_tmp26));_tmp26->v=hd;_tmp26;});}
# 729
return 0;}
# 733
struct _fat_ptr Cyc_List_rto_array(struct _RegionHandle*r2,struct Cyc_List_List*x){
int s=Cyc_List_length(x);
return({unsigned _tmp29=(unsigned)s;void**_tmp28=({struct _RegionHandle*_tmp4C=r2;_region_malloc(_tmp4C,_check_times(_tmp29,sizeof(void*)));});({{unsigned _tmp36=(unsigned)s;unsigned i;for(i=0;i < _tmp36;++ i){({void*_tmp4D=({void*_tmp27=((struct Cyc_List_List*)_check_null(x))->hd;void*v=_tmp27;x=x->tl;v;});_tmp28[i]=_tmp4D;});}}0;});_tag_fat(_tmp28,sizeof(void*),_tmp29);});}
# 738
struct _fat_ptr Cyc_List_to_array(struct Cyc_List_List*x){
return Cyc_List_rto_array(Cyc_Core_heap_region,x);}
# 743
struct Cyc_List_List*Cyc_List_rfrom_array(struct _RegionHandle*r2,struct _fat_ptr arr){
struct Cyc_List_List*ans=0;
{int i=(int)(_get_fat_size(arr,sizeof(void*))- 1U);for(0;i >= 0;-- i){
ans=({struct Cyc_List_List*_tmp2A=_region_malloc(r2,sizeof(*_tmp2A));_tmp2A->hd=*((void**)_check_fat_subscript(arr,sizeof(void*),i)),_tmp2A->tl=ans;_tmp2A;});}}
return ans;}
# 750
struct Cyc_List_List*Cyc_List_from_array(struct _fat_ptr arr){
return Cyc_List_rfrom_array(Cyc_Core_heap_region,arr);}
# 754
struct Cyc_List_List*Cyc_List_rtabulate(struct _RegionHandle*r,int n,void*(*f)(int)){
struct Cyc_List_List*res=0;
{int i=0;for(0;i < n;++ i){
res=({struct Cyc_List_List*_tmp2B=_region_malloc(r,sizeof(*_tmp2B));({void*_tmp4E=f(i);_tmp2B->hd=_tmp4E;}),_tmp2B->tl=res;_tmp2B;});}}
return Cyc_List_imp_rev(res);}
# 761
struct Cyc_List_List*Cyc_List_tabulate(int n,void*(*f)(int)){
return Cyc_List_rtabulate(Cyc_Core_heap_region,n,f);}
# 765
struct Cyc_List_List*Cyc_List_rtabulate_c(struct _RegionHandle*r,int n,void*(*f)(void*,int),void*env){
struct Cyc_List_List*res=0;
{int i=0;for(0;i < n;++ i){
res=({struct Cyc_List_List*_tmp2C=_region_malloc(r,sizeof(*_tmp2C));({void*_tmp4F=f(env,i);_tmp2C->hd=_tmp4F;}),_tmp2C->tl=res;_tmp2C;});}}
return Cyc_List_imp_rev(res);}
# 772
struct Cyc_List_List*Cyc_List_tabulate_c(int n,void*(*f)(void*,int),void*env){
return Cyc_List_rtabulate_c(Cyc_Core_heap_region,n,f,env);}
# 776
int Cyc_List_list_cmp(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
for(1;l1 != 0 && l2 != 0;(l1=l1->tl,l2=l2->tl)){
# 779
if((unsigned)l1 == (unsigned)l2)
return 0;{
int _tmp2D=cmp(l1->hd,l2->hd);int i=_tmp2D;
if(i != 0)
return i;}}
# 785
if(l1 != 0)
return 1;
if(l2 != 0)
return -1;
return 0;}
# 793
int Cyc_List_list_prefix(int(*cmp)(void*,void*),struct Cyc_List_List*l1,struct Cyc_List_List*l2){
for(1;l1 != 0 && l2 != 0;(l1=l1->tl,l2=l2->tl)){
# 796
if((unsigned)l1 == (unsigned)l2)
return 1;
if(cmp(l1->hd,l2->hd)!= 0)
return 0;}
# 801
return l1 == 0;}
# 804
struct Cyc_List_List*Cyc_List_rfilter_c(struct _RegionHandle*r,int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
if(l == 0)
return 0;{
# 808
struct Cyc_List_List*result=({struct Cyc_List_List*_tmp2F=_region_malloc(r,sizeof(*_tmp2F));_tmp2F->hd=l->hd,_tmp2F->tl=0;_tmp2F;});
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
for(1;l != 0;l=l->tl){
if(f(env,l->hd)){
temp=({struct Cyc_List_List*_tmp2E=_region_malloc(r,sizeof(*_tmp2E));_tmp2E->hd=l->hd,_tmp2E->tl=0;_tmp2E;});
end->tl=temp;
end=temp;}}
# 817
return result->tl;}}
# 820
struct Cyc_List_List*Cyc_List_filter_c(int(*f)(void*,void*),void*env,struct Cyc_List_List*l){
return Cyc_List_rfilter_c(Cyc_Core_heap_region,f,env,l);}
# 824
struct Cyc_List_List*Cyc_List_rfilter(struct _RegionHandle*r,int(*f)(void*),struct Cyc_List_List*l){
if(l == 0)
return 0;{
# 828
struct Cyc_List_List*result=({struct Cyc_List_List*_tmp31=_region_malloc(r,sizeof(*_tmp31));_tmp31->hd=l->hd,_tmp31->tl=0;_tmp31;});
struct Cyc_List_List*end=result;
struct Cyc_List_List*temp;
for(1;l != 0;l=l->tl){
if(f(l->hd)){
temp=({struct Cyc_List_List*_tmp30=_region_malloc(r,sizeof(*_tmp30));_tmp30->hd=l->hd,_tmp30->tl=0;_tmp30;});
end->tl=temp;
end=temp;}}
# 837
return result->tl;}}
# 840
struct Cyc_List_List*Cyc_List_filter(int(*f)(void*),struct Cyc_List_List*l){
return Cyc_List_rfilter(Cyc_Core_heap_region,f,l);}
