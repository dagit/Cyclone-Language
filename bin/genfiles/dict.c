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
return({struct Cyc_Dict_Dict _tmpBA;_tmpBA.rel=comp,_tmpBA.r=r,_tmpBA.t=0;_tmpBA;});}
# 43
struct Cyc_Dict_Dict Cyc_Dict_empty(int(*comp)(void*,void*)){
return Cyc_Dict_rempty(Cyc_Core_heap_region,comp);}
# 46
struct Cyc_Dict_Dict Cyc_Dict_rsingleton(struct _RegionHandle*r,int(*comp)(void*,void*),void*key,void*data){
# 48
return({struct Cyc_Dict_Dict _tmpBB;_tmpBB.rel=comp,_tmpBB.r=r,({const struct Cyc_Dict_T*_tmpD5=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp0=_region_malloc(r,sizeof(*_tmp0));_tmp0->color=1U,_tmp0->left=0,_tmp0->right=0,(_tmp0->key_val).f1=key,(_tmp0->key_val).f2=data;_tmp0;});_tmpBB.t=_tmpD5;});_tmpBB;});}
# 50
struct Cyc_Dict_Dict Cyc_Dict_singleton(int(*comp)(void*,void*),void*key,void*data){
return Cyc_Dict_rsingleton(Cyc_Core_heap_region,comp,key,data);}
# 54
struct Cyc_Dict_Dict Cyc_Dict_rshare(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*t=d.t;
const struct Cyc_Dict_T*t2=t;
return({struct Cyc_Dict_Dict _tmpBC;_tmpBC.rel=d.rel,_tmpBC.r=r,_tmpBC.t=t2;_tmpBC;});}
# 60
int Cyc_Dict_is_empty(struct Cyc_Dict_Dict d){
return d.t == (const struct Cyc_Dict_T*)0;}
# 64
int Cyc_Dict_member(struct Cyc_Dict_Dict d,void*key){
int(*_tmp1)(void*,void*)=d.rel;int(*rel)(void*,void*)=_tmp1;
const struct Cyc_Dict_T*_tmp2=d.t;const struct Cyc_Dict_T*t=_tmp2;
while(t != (const struct Cyc_Dict_T*)0){
int _tmp3=rel(key,(t->key_val).f1);int i=_tmp3;
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return 1;}}}
# 73
return 0;}
# 76
void*Cyc_Dict_lookup(struct Cyc_Dict_Dict d,void*key){
int(*_tmp4)(void*,void*)=d.rel;int(*rel)(void*,void*)=_tmp4;
const struct Cyc_Dict_T*_tmp5=d.t;const struct Cyc_Dict_T*t=_tmp5;
while(t != (const struct Cyc_Dict_T*)0){
int _tmp6=rel(key,(t->key_val).f1);int i=_tmp6;
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return(t->key_val).f2;}}}
# 85
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 88
void*Cyc_Dict_lookup_other(struct Cyc_Dict_Dict d,int(*cmp)(void*,void*),void*key){
const struct Cyc_Dict_T*_tmp7=d.t;const struct Cyc_Dict_T*t=_tmp7;
while(t != (const struct Cyc_Dict_T*)0){
int _tmp8=cmp(key,(t->key_val).f1);int i=_tmp8;
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return(t->key_val).f2;}}}
# 96
(int)_throw((void*)& Cyc_Dict_Absent_val);}
# 99
void**Cyc_Dict_lookup_opt(struct Cyc_Dict_Dict d,void*key){
int(*_tmp9)(void*,void*)=d.rel;int(*rel)(void*,void*)=_tmp9;
const struct Cyc_Dict_T*_tmpA=d.t;const struct Cyc_Dict_T*t=_tmpA;
while(t != (const struct Cyc_Dict_T*)0){
int _tmpB=rel(key,(t->key_val).f1);int i=_tmpB;
if(i < 0)t=t->left;else{
if(i > 0)t=t->right;else{
return&(t->key_val).f2;}}}
# 108
return 0;}
# 111
int Cyc_Dict_lookup_bool(struct Cyc_Dict_Dict d,void*key,void**ans_place){
int(*_tmpC)(void*,void*)=d.rel;int(*rel)(void*,void*)=_tmpC;
const struct Cyc_Dict_T*_tmpD=d.t;const struct Cyc_Dict_T*t=_tmpD;
while(t != (const struct Cyc_Dict_T*)0){
int _tmpE=rel(key,(t->key_val).f1);int i=_tmpE;
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
struct _tuple1 _tmpF=quad;enum Cyc_Dict_Color _tmp10;struct _tuple0 _tmp17;const struct Cyc_Dict_T*_tmp15;struct _tuple0 _tmp16;const struct Cyc_Dict_T*_tmp14;struct _tuple0 _tmp13;const struct Cyc_Dict_T*_tmp12;const struct Cyc_Dict_T*_tmp11;if(_tmpF.f1 == Cyc_Dict_B){if(_tmpF.f2 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f2)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f2)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->left)->color == Cyc_Dict_R){_tmp11=((_tmpF.f2)->left)->left;_tmp12=((_tmpF.f2)->left)->right;_tmp13=((_tmpF.f2)->left)->key_val;_tmp14=(_tmpF.f2)->right;_tmp16=(_tmpF.f2)->key_val;_tmp15=_tmpF.f3;_tmp17=_tmpF.f4;{const struct Cyc_Dict_T*a=_tmp11;const struct Cyc_Dict_T*b=_tmp12;struct _tuple0 x=_tmp13;const struct Cyc_Dict_T*c=_tmp14;struct _tuple0 y=_tmp16;const struct Cyc_Dict_T*d=_tmp15;struct _tuple0 z=_tmp17;
# 131
return({struct Cyc_Dict_T*_tmp1A=_region_malloc(r,sizeof(*_tmp1A));_tmp1A->color=0U,({const struct Cyc_Dict_T*_tmpD7=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp18=_region_malloc(r,sizeof(*_tmp18));_tmp18->color=1U,_tmp18->left=a,_tmp18->right=b,_tmp18->key_val=x;_tmp18;});_tmp1A->left=_tmpD7;}),({const struct Cyc_Dict_T*_tmpD6=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp19=_region_malloc(r,sizeof(*_tmp19));_tmp19->color=1U,_tmp19->left=c,_tmp19->right=d,_tmp19->key_val=z;_tmp19;});_tmp1A->right=_tmpD6;}),_tmp1A->key_val=y;_tmp1A;});}}else{if(((const struct Cyc_Dict_T*)_tmpF.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->right)->color == Cyc_Dict_R)goto _LL3;else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f2)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f2)->right)->color == Cyc_Dict_R){_LL3: _tmp11=(_tmpF.f2)->left;_tmp12=((_tmpF.f2)->right)->left;_tmp14=((_tmpF.f2)->right)->right;_tmp13=((_tmpF.f2)->right)->key_val;_tmp16=(_tmpF.f2)->key_val;_tmp15=_tmpF.f3;_tmp17=_tmpF.f4;{const struct Cyc_Dict_T*a=_tmp11;const struct Cyc_Dict_T*b=_tmp12;const struct Cyc_Dict_T*c=_tmp14;struct _tuple0 y=_tmp13;struct _tuple0 x=_tmp16;const struct Cyc_Dict_T*d=_tmp15;struct _tuple0 z=_tmp17;
# 133
return({struct Cyc_Dict_T*_tmp1D=_region_malloc(r,sizeof(*_tmp1D));_tmp1D->color=0U,({const struct Cyc_Dict_T*_tmpD9=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1B=_region_malloc(r,sizeof(*_tmp1B));_tmp1B->color=1U,_tmp1B->left=a,_tmp1B->right=b,_tmp1B->key_val=x;_tmp1B;});_tmp1D->left=_tmpD9;}),({const struct Cyc_Dict_T*_tmpD8=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1C=_region_malloc(r,sizeof(*_tmp1C));_tmp1C->color=1U,_tmp1C->left=c,_tmp1C->right=d,_tmp1C->key_val=z;_tmp1C;});_tmp1D->right=_tmpD8;}),_tmp1D->key_val=y;_tmp1D;});}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R)goto _LL5;else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{if(_tmpF.f3 != 0){if(((const struct Cyc_Dict_T*)_tmpF.f3)->color == Cyc_Dict_R){if(((const struct Cyc_Dict_T*)_tmpF.f3)->left != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->left)->color == Cyc_Dict_R){_LL5: _tmp11=_tmpF.f2;_tmp12=((_tmpF.f3)->left)->left;_tmp14=((_tmpF.f3)->left)->right;_tmp13=((_tmpF.f3)->left)->key_val;_tmp15=(_tmpF.f3)->right;_tmp16=(_tmpF.f3)->key_val;_tmp17=_tmpF.f4;{const struct Cyc_Dict_T*a=_tmp11;const struct Cyc_Dict_T*b=_tmp12;const struct Cyc_Dict_T*c=_tmp14;struct _tuple0 y=_tmp13;const struct Cyc_Dict_T*d=_tmp15;struct _tuple0 z=_tmp16;struct _tuple0 x=_tmp17;
# 135
return({struct Cyc_Dict_T*_tmp20=_region_malloc(r,sizeof(*_tmp20));_tmp20->color=0U,({const struct Cyc_Dict_T*_tmpDB=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1E=_region_malloc(r,sizeof(*_tmp1E));_tmp1E->color=1U,_tmp1E->left=a,_tmp1E->right=b,_tmp1E->key_val=x;_tmp1E;});_tmp20->left=_tmpDB;}),({const struct Cyc_Dict_T*_tmpDA=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp1F=_region_malloc(r,sizeof(*_tmp1F));_tmp1F->color=1U,_tmp1F->left=c,_tmp1F->right=d,_tmp1F->key_val=z;_tmp1F;});_tmp20->right=_tmpDA;}),_tmp20->key_val=y;_tmp20;});}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R)goto _LL7;else{goto _LL9;}}else{goto _LL9;}}}else{if(((const struct Cyc_Dict_T*)_tmpF.f3)->right != 0){if(((const struct Cyc_Dict_T*)((const struct Cyc_Dict_T*)_tmpF.f3)->right)->color == Cyc_Dict_R){_LL7: _tmp11=_tmpF.f2;_tmp12=(_tmpF.f3)->left;_tmp14=((_tmpF.f3)->right)->left;_tmp15=((_tmpF.f3)->right)->right;_tmp13=((_tmpF.f3)->right)->key_val;_tmp16=(_tmpF.f3)->key_val;_tmp17=_tmpF.f4;{const struct Cyc_Dict_T*a=_tmp11;const struct Cyc_Dict_T*b=_tmp12;const struct Cyc_Dict_T*c=_tmp14;const struct Cyc_Dict_T*d=_tmp15;struct _tuple0 z=_tmp13;struct _tuple0 y=_tmp16;struct _tuple0 x=_tmp17;
# 137
return({struct Cyc_Dict_T*_tmp23=_region_malloc(r,sizeof(*_tmp23));_tmp23->color=0U,({const struct Cyc_Dict_T*_tmpDD=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp21=_region_malloc(r,sizeof(*_tmp21));_tmp21->color=1U,_tmp21->left=a,_tmp21->right=b,_tmp21->key_val=x;_tmp21;});_tmp23->left=_tmpDD;}),({const struct Cyc_Dict_T*_tmpDC=(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp22=_region_malloc(r,sizeof(*_tmp22));_tmp22->color=1U,_tmp22->left=c,_tmp22->right=d,_tmp22->key_val=z;_tmp22;});_tmp23->right=_tmpDC;}),_tmp23->key_val=y;_tmp23;});}}else{goto _LL9;}}else{goto _LL9;}}}else{goto _LL9;}}else{goto _LL9;}}}else{_LL9: _tmp10=_tmpF.f1;_tmp11=_tmpF.f2;_tmp12=_tmpF.f3;_tmp13=_tmpF.f4;{enum Cyc_Dict_Color a=_tmp10;const struct Cyc_Dict_T*b=_tmp11;const struct Cyc_Dict_T*c=_tmp12;struct _tuple0 d=_tmp13;
# 139
return({struct Cyc_Dict_T*_tmp24=_region_malloc(r,sizeof(*_tmp24));_tmp24->color=a,_tmp24->left=b,_tmp24->right=c,_tmp24->key_val=d;_tmp24;});}};}
# 143
static struct Cyc_Dict_T*Cyc_Dict_ins(struct _RegionHandle*r,int(*rel)(void*,void*),struct _tuple0 key_val,const struct Cyc_Dict_T*t){
# 145
const struct Cyc_Dict_T*_tmp25=t;struct _tuple0 _tmp29;const struct Cyc_Dict_T*_tmp28;const struct Cyc_Dict_T*_tmp27;enum Cyc_Dict_Color _tmp26;if(_tmp25 == 0)
return({struct Cyc_Dict_T*_tmp2A=_region_malloc(r,sizeof(*_tmp2A));_tmp2A->color=0U,_tmp2A->left=0,_tmp2A->right=0,_tmp2A->key_val=key_val;_tmp2A;});else{_tmp26=_tmp25->color;_tmp27=_tmp25->left;_tmp28=_tmp25->right;_tmp29=_tmp25->key_val;{enum Cyc_Dict_Color color=_tmp26;const struct Cyc_Dict_T*a=_tmp27;const struct Cyc_Dict_T*b=_tmp28;struct _tuple0 y=_tmp29;
# 148
int _tmp2B=rel(key_val.f1,y.f1);int i=_tmp2B;
if(i < 0)return({struct _RegionHandle*_tmpDF=r;Cyc_Dict_balance(_tmpDF,({struct _tuple1 _tmpBD;_tmpBD.f1=color,({const struct Cyc_Dict_T*_tmpDE=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,a);_tmpBD.f2=_tmpDE;}),_tmpBD.f3=b,_tmpBD.f4=y;_tmpBD;}));});
if(i > 0)return({struct _RegionHandle*_tmpE1=r;Cyc_Dict_balance(_tmpE1,({struct _tuple1 _tmpBE;_tmpBE.f1=color,_tmpBE.f2=a,({const struct Cyc_Dict_T*_tmpE0=(const struct Cyc_Dict_T*)Cyc_Dict_ins(r,rel,key_val,b);_tmpBE.f3=_tmpE0;}),_tmpBE.f4=y;_tmpBE;}));});
return({struct Cyc_Dict_T*_tmp2C=_region_malloc(r,sizeof(*_tmp2C));_tmp2C->color=color,_tmp2C->left=a,_tmp2C->right=b,_tmp2C->key_val=key_val;_tmp2C;});}};}
# 155
struct Cyc_Dict_Dict Cyc_Dict_insert(struct Cyc_Dict_Dict d,void*key,void*data){
struct Cyc_Dict_T*_tmp2D=({struct _RegionHandle*_tmpE4=d.r;int(*_tmpE3)(void*,void*)=d.rel;struct _tuple0 _tmpE2=({struct _tuple0 _tmpC0;_tmpC0.f1=key,_tmpC0.f2=data;_tmpC0;});Cyc_Dict_ins(_tmpE4,_tmpE3,_tmpE2,d.t);});struct Cyc_Dict_T*ans=_tmp2D;
((struct Cyc_Dict_T*)_check_null(ans))->color=1U;
return({struct Cyc_Dict_Dict _tmpBF;_tmpBF.rel=d.rel,_tmpBF.r=d.r,_tmpBF.t=(const struct Cyc_Dict_T*)ans;_tmpBF;});}
# 162
struct Cyc_Dict_Dict Cyc_Dict_insert_new(struct Cyc_Dict_Dict d,void*key,void*data){
if(Cyc_Dict_member(d,key))
(int)_throw((void*)& Cyc_Dict_Absent_val);
return Cyc_Dict_insert(d,key,data);}
# 168
struct Cyc_Dict_Dict Cyc_Dict_inserts(struct Cyc_Dict_Dict d,struct Cyc_List_List*kds){
# 170
for(1;kds != 0;kds=kds->tl){
d=Cyc_Dict_insert(d,(((struct _tuple0*)kds->hd)[0]).f1,(((struct _tuple0*)kds->hd)[0]).f2);}
return d;}
# 175
static void*Cyc_Dict_fold_tree(void*(*f)(void*,void*,void*),const struct Cyc_Dict_T*t,void*accum){
const struct Cyc_Dict_T*_tmp2E=t;void*_tmp32;void*_tmp31;const struct Cyc_Dict_T*_tmp30;const struct Cyc_Dict_T*_tmp2F;_tmp2F=_tmp2E->left;_tmp30=_tmp2E->right;_tmp31=(_tmp2E->key_val).f1;_tmp32=(_tmp2E->key_val).f2;{const struct Cyc_Dict_T*left=_tmp2F;const struct Cyc_Dict_T*right=_tmp30;void*key=_tmp31;void*val=_tmp32;
if(left != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,left,accum);
accum=f(key,val,accum);
if(right != (const struct Cyc_Dict_T*)0)accum=Cyc_Dict_fold_tree(f,right,accum);
return accum;}}
# 183
void*Cyc_Dict_fold(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*_tmp33=d.t;const struct Cyc_Dict_T*t=_tmp33;
if(t == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree(f,t,accum);}
# 190
static void*Cyc_Dict_fold_tree_c(void*(*f)(void*,void*,void*,void*),void*env,const struct Cyc_Dict_T*t,void*accum){
# 192
const struct Cyc_Dict_T*_tmp34=t;void*_tmp38;void*_tmp37;const struct Cyc_Dict_T*_tmp36;const struct Cyc_Dict_T*_tmp35;_tmp35=_tmp34->left;_tmp36=_tmp34->right;_tmp37=(_tmp34->key_val).f1;_tmp38=(_tmp34->key_val).f2;{const struct Cyc_Dict_T*left=_tmp35;const struct Cyc_Dict_T*right=_tmp36;void*key=_tmp37;void*val=_tmp38;
if(left != (const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,left,accum);
accum=f(env,key,val,accum);
if(right != (const struct Cyc_Dict_T*)0)
accum=Cyc_Dict_fold_tree_c(f,env,right,accum);
return accum;}}
# 201
void*Cyc_Dict_fold_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d,void*accum){
const struct Cyc_Dict_T*_tmp39=d.t;const struct Cyc_Dict_T*t=_tmp39;
if(t == (const struct Cyc_Dict_T*)0)
return accum;
return Cyc_Dict_fold_tree_c(f,env,t,accum);}
# 208
static void Cyc_Dict_app_tree(void*(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp3A=t;void*_tmp3E;void*_tmp3D;const struct Cyc_Dict_T*_tmp3C;const struct Cyc_Dict_T*_tmp3B;_tmp3B=_tmp3A->left;_tmp3C=_tmp3A->right;_tmp3D=(_tmp3A->key_val).f1;_tmp3E=(_tmp3A->key_val).f2;{const struct Cyc_Dict_T*left=_tmp3B;const struct Cyc_Dict_T*right=_tmp3C;void*key=_tmp3D;void*val=_tmp3E;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,left);
f(key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree(f,right);}}
# 215
void Cyc_Dict_app(void*(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp3F=d.t;const struct Cyc_Dict_T*t=_tmp3F;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree(f,t);}
# 221
static void Cyc_Dict_app_tree_c(void*(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp40=t;void*_tmp44;void*_tmp43;const struct Cyc_Dict_T*_tmp42;const struct Cyc_Dict_T*_tmp41;_tmp41=_tmp40->left;_tmp42=_tmp40->right;_tmp43=(_tmp40->key_val).f1;_tmp44=(_tmp40->key_val).f2;{const struct Cyc_Dict_T*left=_tmp41;const struct Cyc_Dict_T*right=_tmp42;void*key=_tmp43;void*val=_tmp44;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,left);
f(env,key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_app_tree_c(f,env,right);}}
# 228
void Cyc_Dict_app_c(void*(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp45=d.t;const struct Cyc_Dict_T*t=_tmp45;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_app_tree_c(f,env,t);}
# 234
static void Cyc_Dict_iter_tree(void(*f)(void*,void*),const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp46=t;void*_tmp4A;void*_tmp49;const struct Cyc_Dict_T*_tmp48;const struct Cyc_Dict_T*_tmp47;_tmp47=_tmp46->left;_tmp48=_tmp46->right;_tmp49=(_tmp46->key_val).f1;_tmp4A=(_tmp46->key_val).f2;{const struct Cyc_Dict_T*left=_tmp47;const struct Cyc_Dict_T*right=_tmp48;void*key=_tmp49;void*val=_tmp4A;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,left);
f(key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree(f,right);}}
# 241
void Cyc_Dict_iter(void(*f)(void*,void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp4B=d.t;const struct Cyc_Dict_T*t=_tmp4B;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree(f,t);}
# 247
static void Cyc_Dict_iter_tree_c(void(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp4C=t;void*_tmp50;void*_tmp4F;const struct Cyc_Dict_T*_tmp4E;const struct Cyc_Dict_T*_tmp4D;_tmp4D=_tmp4C->left;_tmp4E=_tmp4C->right;_tmp4F=(_tmp4C->key_val).f1;_tmp50=(_tmp4C->key_val).f2;{const struct Cyc_Dict_T*left=_tmp4D;const struct Cyc_Dict_T*right=_tmp4E;void*key=_tmp4F;void*val=_tmp50;
if(left != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,left);
f(env,key,val);
if(right != (const struct Cyc_Dict_T*)0)Cyc_Dict_iter_tree_c(f,env,right);}}
# 254
void Cyc_Dict_iter_c(void(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp51=d.t;const struct Cyc_Dict_T*t=_tmp51;
if(t != (const struct Cyc_Dict_T*)0)
Cyc_Dict_iter_tree_c(f,env,t);}
# 260
static void Cyc_Dict_count_elem(int*cnt,void*a,void*b){
*cnt=*cnt + 1;}
# 263
int Cyc_Dict_cardinality(struct Cyc_Dict_Dict d){
int num=0;
({(void(*)(void(*)(int*,void*,void*),int*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_Dict_count_elem,& num,d);
return num;}struct _tuple2{void(*f1)(void*,void*);struct Cyc_Dict_Dict f2;};
# 269
static void Cyc_Dict_iter2_f(struct _tuple2*env,void*a,void*b1){
# 271
struct _tuple2*_tmp52=env;struct Cyc_Dict_Dict _tmp54;void*_tmp53;_tmp53=_tmp52->f1;_tmp54=_tmp52->f2;{void(*f)(void*,void*)=_tmp53;struct Cyc_Dict_Dict d2=_tmp54;
({void(*_tmpE6)(void*,void*)=f;void*_tmpE5=b1;_tmpE6(_tmpE5,Cyc_Dict_lookup(d2,a));});}}
# 275
void Cyc_Dict_iter2(void(*f)(void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 278
struct _tuple2 _tmp55=({struct _tuple2 _tmpC1;_tmpC1.f1=f,_tmpC1.f2=d2;_tmpC1;});struct _tuple2 env=_tmp55;
({(void(*)(void(*)(struct _tuple2*,void*,void*),struct _tuple2*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_Dict_iter2_f,& env,d1);}struct _tuple3{void(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 282
static void Cyc_Dict_iter2_c_f(struct _tuple3*env,void*a,void*b1){
# 284
struct _tuple3*_tmp56=env;void*_tmp59;struct Cyc_Dict_Dict _tmp58;void*_tmp57;_tmp57=_tmp56->f1;_tmp58=_tmp56->f2;_tmp59=_tmp56->f3;{void(*f)(void*,void*,void*)=_tmp57;struct Cyc_Dict_Dict d2=_tmp58;void*inner_env=_tmp59;
({void(*_tmpE9)(void*,void*,void*)=f;void*_tmpE8=inner_env;void*_tmpE7=b1;_tmpE9(_tmpE8,_tmpE7,Cyc_Dict_lookup(d2,a));});}}
# 288
void Cyc_Dict_iter2_c(void(*f)(void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 291
struct _tuple3 _tmp5A=({struct _tuple3 _tmpC2;_tmpC2.f1=f,_tmpC2.f2=d2,_tmpC2.f3=inner_env;_tmpC2;});struct _tuple3 env=_tmp5A;
({(void(*)(void(*)(struct _tuple3*,void*,void*),struct _tuple3*,struct Cyc_Dict_Dict))Cyc_Dict_iter_c;})(Cyc_Dict_iter2_c_f,& env,d1);}struct _tuple4{void*(*f1)(void*,void*,void*,void*,void*);struct Cyc_Dict_Dict f2;void*f3;};
# 295
static void*Cyc_Dict_fold2_c_f(struct _tuple4*env,void*a,void*b1,void*accum){
# 299
struct _tuple4*_tmp5B=env;void*_tmp5E;struct Cyc_Dict_Dict _tmp5D;void*_tmp5C;_tmp5C=_tmp5B->f1;_tmp5D=_tmp5B->f2;_tmp5E=_tmp5B->f3;{void*(*f)(void*,void*,void*,void*,void*)=_tmp5C;struct Cyc_Dict_Dict d2=_tmp5D;void*inner_env=_tmp5E;
return({void*(*_tmpEE)(void*,void*,void*,void*,void*)=f;void*_tmpED=inner_env;void*_tmpEC=a;void*_tmpEB=b1;void*_tmpEA=Cyc_Dict_lookup(d2,a);_tmpEE(_tmpED,_tmpEC,_tmpEB,_tmpEA,accum);});}}
# 303
void*Cyc_Dict_fold2_c(void*(*f)(void*,void*,void*,void*,void*),void*inner_env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2,void*accum){
# 307
struct _tuple4 _tmp5F=({struct _tuple4 _tmpC3;_tmpC3.f1=f,_tmpC3.f2=d2,_tmpC3.f3=inner_env;_tmpC3;});struct _tuple4 env=_tmp5F;
return({(void*(*)(void*(*)(struct _tuple4*,void*,void*,void*),struct _tuple4*,struct Cyc_Dict_Dict,void*))Cyc_Dict_fold_c;})(Cyc_Dict_fold2_c_f,& env,d1,accum);}
# 311
static const struct Cyc_Dict_T*Cyc_Dict_copy_tree(struct _RegionHandle*r2,const struct Cyc_Dict_T*t){
if(t == (const struct Cyc_Dict_T*)0)return 0;else{
# 314
struct Cyc_Dict_T _tmp60=*t;struct Cyc_Dict_T _stmttmp0=_tmp60;struct Cyc_Dict_T _tmp61=_stmttmp0;struct _tuple0 _tmp65;const struct Cyc_Dict_T*_tmp64;const struct Cyc_Dict_T*_tmp63;enum Cyc_Dict_Color _tmp62;_tmp62=_tmp61.color;_tmp63=_tmp61.left;_tmp64=_tmp61.right;_tmp65=_tmp61.key_val;{enum Cyc_Dict_Color c=_tmp62;const struct Cyc_Dict_T*left=_tmp63;const struct Cyc_Dict_T*right=_tmp64;struct _tuple0 pr=_tmp65;
const struct Cyc_Dict_T*_tmp66=Cyc_Dict_copy_tree(r2,left);const struct Cyc_Dict_T*new_left=_tmp66;
const struct Cyc_Dict_T*_tmp67=Cyc_Dict_copy_tree(r2,right);const struct Cyc_Dict_T*new_right=_tmp67;
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp68=_region_malloc(r2,sizeof(*_tmp68));_tmp68->color=c,_tmp68->left=new_left,_tmp68->right=new_right,_tmp68->key_val=pr;_tmp68;});}}}
# 321
struct Cyc_Dict_Dict Cyc_Dict_rcopy(struct _RegionHandle*r2,struct Cyc_Dict_Dict d){
return({struct Cyc_Dict_Dict _tmpC4;_tmpC4.rel=d.rel,_tmpC4.r=r2,({const struct Cyc_Dict_T*_tmpEF=Cyc_Dict_copy_tree(r2,d.t);_tmpC4.t=_tmpEF;});_tmpC4;});}
# 325
struct Cyc_Dict_Dict Cyc_Dict_copy(struct Cyc_Dict_Dict d){
return Cyc_Dict_rcopy(Cyc_Core_heap_region,d);}
# 329
static const struct Cyc_Dict_T*Cyc_Dict_map_tree(struct _RegionHandle*r,void*(*f)(void*),const struct Cyc_Dict_T*t){
# 331
const struct Cyc_Dict_T*_tmp69=t;void*_tmp6E;void*_tmp6D;const struct Cyc_Dict_T*_tmp6C;const struct Cyc_Dict_T*_tmp6B;enum Cyc_Dict_Color _tmp6A;_tmp6A=_tmp69->color;_tmp6B=_tmp69->left;_tmp6C=_tmp69->right;_tmp6D=(_tmp69->key_val).f1;_tmp6E=(_tmp69->key_val).f2;{enum Cyc_Dict_Color c=_tmp6A;const struct Cyc_Dict_T*left=_tmp6B;const struct Cyc_Dict_T*right=_tmp6C;void*key=_tmp6D;void*val=_tmp6E;
const struct Cyc_Dict_T*_tmp6F=left == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,left);const struct Cyc_Dict_T*new_left=_tmp6F;
void*_tmp70=f(val);void*new_val=_tmp70;
const struct Cyc_Dict_T*_tmp71=right == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree(r,f,right);const struct Cyc_Dict_T*new_right=_tmp71;
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp72=_region_malloc(r,sizeof(*_tmp72));_tmp72->color=c,_tmp72->left=new_left,_tmp72->right=new_right,(_tmp72->key_val).f1=key,(_tmp72->key_val).f2=new_val;_tmp72;});}}
# 338
struct Cyc_Dict_Dict Cyc_Dict_rmap(struct _RegionHandle*r,void*(*f)(void*),struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp73=d.t;const struct Cyc_Dict_T*t=_tmp73;
if(t == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _tmpC5;_tmpC5.rel=d.rel,_tmpC5.r=r,_tmpC5.t=0;_tmpC5;});
return({struct Cyc_Dict_Dict _tmpC6;_tmpC6.rel=d.rel,_tmpC6.r=r,({const struct Cyc_Dict_T*_tmpF0=Cyc_Dict_map_tree(r,f,t);_tmpC6.t=_tmpF0;});_tmpC6;});}
# 345
struct Cyc_Dict_Dict Cyc_Dict_map(void*(*f)(void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap(Cyc_Core_heap_region,f,d);}
# 349
static const struct Cyc_Dict_T*Cyc_Dict_map_tree_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,const struct Cyc_Dict_T*t){
# 351
const struct Cyc_Dict_T*_tmp74=t;void*_tmp79;void*_tmp78;const struct Cyc_Dict_T*_tmp77;const struct Cyc_Dict_T*_tmp76;enum Cyc_Dict_Color _tmp75;_tmp75=_tmp74->color;_tmp76=_tmp74->left;_tmp77=_tmp74->right;_tmp78=(_tmp74->key_val).f1;_tmp79=(_tmp74->key_val).f2;{enum Cyc_Dict_Color c=_tmp75;const struct Cyc_Dict_T*left=_tmp76;const struct Cyc_Dict_T*right=_tmp77;void*key=_tmp78;void*val=_tmp79;
const struct Cyc_Dict_T*_tmp7A=
left == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,left);
# 352
const struct Cyc_Dict_T*new_left=_tmp7A;
# 354
void*_tmp7B=f(env,val);void*new_val=_tmp7B;
const struct Cyc_Dict_T*_tmp7C=
right == (const struct Cyc_Dict_T*)0?0: Cyc_Dict_map_tree_c(r,f,env,right);
# 355
const struct Cyc_Dict_T*new_right=_tmp7C;
# 357
return(const struct Cyc_Dict_T*)({struct Cyc_Dict_T*_tmp7D=_region_malloc(r,sizeof(*_tmp7D));_tmp7D->color=c,_tmp7D->left=new_left,_tmp7D->right=new_right,(_tmp7D->key_val).f1=key,(_tmp7D->key_val).f2=new_val;_tmp7D;});}}
# 360
struct Cyc_Dict_Dict Cyc_Dict_rmap_c(struct _RegionHandle*r,void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
# 362
const struct Cyc_Dict_T*_tmp7E=d.t;const struct Cyc_Dict_T*t=_tmp7E;
if(t == (const struct Cyc_Dict_T*)0)
return({struct Cyc_Dict_Dict _tmpC7;_tmpC7.rel=d.rel,_tmpC7.r=r,_tmpC7.t=0;_tmpC7;});
return({struct Cyc_Dict_Dict _tmpC8;_tmpC8.rel=d.rel,_tmpC8.r=r,({const struct Cyc_Dict_T*_tmpF1=Cyc_Dict_map_tree_c(r,f,env,t);_tmpC8.t=_tmpF1;});_tmpC8;});}
# 368
struct Cyc_Dict_Dict Cyc_Dict_map_c(void*(*f)(void*,void*),void*env,struct Cyc_Dict_Dict d){
return Cyc_Dict_rmap_c(Cyc_Core_heap_region,f,env,d);}
# 372
struct _tuple0*Cyc_Dict_rchoose(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
if(d.t == (const struct Cyc_Dict_T*)0)
(int)_throw((void*)& Cyc_Dict_Absent_val);
return({struct _tuple0*_tmp7F=_region_malloc(r,sizeof(*_tmp7F));_tmp7F->f1=((d.t)->key_val).f1,_tmp7F->f2=((d.t)->key_val).f2;_tmp7F;});}
# 378
static int Cyc_Dict_forall_tree_c(int(*f)(void*,void*,void*),void*env,const struct Cyc_Dict_T*t){
const struct Cyc_Dict_T*_tmp80=t;void*_tmp84;void*_tmp83;const struct Cyc_Dict_T*_tmp82;const struct Cyc_Dict_T*_tmp81;_tmp81=_tmp80->left;_tmp82=_tmp80->right;_tmp83=(_tmp80->key_val).f1;_tmp84=(_tmp80->key_val).f2;{const struct Cyc_Dict_T*left=_tmp81;const struct Cyc_Dict_T*right=_tmp82;void*key=_tmp83;void*val=_tmp84;
return
((left == (const struct Cyc_Dict_T*)0 || Cyc_Dict_forall_tree_c(f,env,left))&&
 f(env,key,val))&&(
right == (const struct Cyc_Dict_T*)0 || Cyc_Dict_forall_tree_c(f,env,right));}}
# 386
int Cyc_Dict_forall_c(int(*f)(void*,void*,void*),void*env,struct Cyc_Dict_Dict d){
const struct Cyc_Dict_T*_tmp85=d.t;const struct Cyc_Dict_T*t=_tmp85;
if(t == (const struct Cyc_Dict_T*)0)
return 1;
return Cyc_Dict_forall_tree_c(f,env,t);}struct _tuple5{int(*f1)(void*,void*,void*);struct Cyc_Dict_Dict f2;};
# 393
static int Cyc_Dict_forall_intersect_f(struct _tuple5*env,void*a,void*b){
# 395
struct _tuple5*_tmp86=env;struct Cyc_Dict_Dict _tmp88;void*_tmp87;_tmp87=_tmp86->f1;_tmp88=_tmp86->f2;{int(*f)(void*,void*,void*)=_tmp87;struct Cyc_Dict_Dict d2=_tmp88;
if(Cyc_Dict_member(d2,a))
return({int(*_tmpF4)(void*,void*,void*)=f;void*_tmpF3=a;void*_tmpF2=b;_tmpF4(_tmpF3,_tmpF2,Cyc_Dict_lookup(d2,a));});
return 1;}}
# 402
int Cyc_Dict_forall_intersect(int(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
struct _tuple5 _tmp89=({struct _tuple5 _tmpC9;_tmpC9.f1=f,_tmpC9.f2=d2;_tmpC9;});struct _tuple5 env=_tmp89;
return({(int(*)(int(*)(struct _tuple5*,void*,void*),struct _tuple5*,struct Cyc_Dict_Dict))Cyc_Dict_forall_c;})(Cyc_Dict_forall_intersect_f,& env,d1);}struct _tuple6{void*(*f1)(void*,void*,void*,void*);void*f2;};
# 408
static struct Cyc_Dict_Dict*Cyc_Dict_union_f(struct _tuple6*env,void*a,void*b,struct Cyc_Dict_Dict*d1){
# 412
if(Cyc_Dict_member(*d1,a)){
# 414
void*_tmp8A=Cyc_Dict_lookup(*d1,a);void*old_val=_tmp8A;
void*_tmp8B=(((*env).f1))((*env).f2,a,old_val,b);void*new_val=_tmp8B;
if(new_val != old_val)
({struct Cyc_Dict_Dict _tmpF5=Cyc_Dict_insert(*d1,a,new_val);*d1=_tmpF5;});
return d1;}
# 420
({struct Cyc_Dict_Dict _tmpF6=Cyc_Dict_insert(*d1,a,b);*d1=_tmpF6;});
return d1;}
# 424
struct Cyc_Dict_Dict Cyc_Dict_union_two_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 427
if((int)d1.t == (int)d2.t)return d1;
if(d1.t == (const struct Cyc_Dict_T*)0)return d2;
if(d2.t == (const struct Cyc_Dict_T*)0)return d1;{
struct _tuple6 _tmp8C=({struct _tuple6 _tmpCA;_tmpCA.f1=f,_tmpCA.f2=env;_tmpCA;});struct _tuple6 fenvpair=_tmp8C;
({(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple6*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple6*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;})(Cyc_Dict_union_f,& fenvpair,d2,& d1);
return d1;}}
# 435
struct Cyc_Dict_Dict Cyc_Dict_intersect_c(void*(*f)(void*,void*,void*,void*),void*env,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 438
const struct Cyc_Dict_T*_tmp8D=d2.t;const struct Cyc_Dict_T*t2=_tmp8D;
if(t2 == (const struct Cyc_Dict_T*)0)return d2;
if((int)d1.t == (int)t2)return d2;{
const struct Cyc_Dict_T*ans_tree=0;
struct _RegionHandle _tmp8E=_new_region("temp");struct _RegionHandle*temp=& _tmp8E;_push_region(temp);{
# 444
struct _fat_ptr queue=_tag_fat(({unsigned _tmp98=16U;const struct Cyc_Dict_T**_tmp97=({struct _RegionHandle*_tmpF7=temp;_region_malloc(_tmpF7,_check_times(_tmp98,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _tmpCD=16U;unsigned i;for(i=0;i < _tmpCD;++ i){_tmp97[i]=t2;}}0;});_tmp97;}),sizeof(const struct Cyc_Dict_T*),16U);
int ind=0;
while(ind != -1){
const struct Cyc_Dict_T*_tmp8F=((const struct Cyc_Dict_T**)queue.curr)[ind --];const struct Cyc_Dict_T*_stmttmp1=_tmp8F;const struct Cyc_Dict_T*_tmp90=_stmttmp1;void*_tmp94;void*_tmp93;const struct Cyc_Dict_T*_tmp92;const struct Cyc_Dict_T*_tmp91;_tmp91=_tmp90->left;_tmp92=_tmp90->right;_tmp93=(_tmp90->key_val).f1;_tmp94=(_tmp90->key_val).f2;{const struct Cyc_Dict_T*l=_tmp91;const struct Cyc_Dict_T*r=_tmp92;void*k=_tmp93;void*v=_tmp94;
if((unsigned)(ind + 2)>= _get_fat_size(queue,sizeof(const struct Cyc_Dict_T*)))
queue=({unsigned _tmp96=_get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))* 2U;const struct Cyc_Dict_T**_tmp95=({struct _RegionHandle*_tmpF8=temp;_region_malloc(_tmpF8,_check_times(_tmp96,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _tmpCB=_get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))* 2U;unsigned i;for(i=0;i < _tmpCB;++ i){
i < _get_fat_size(queue,sizeof(const struct Cyc_Dict_T*))?_tmp95[i]=((const struct Cyc_Dict_T**)queue.curr)[(int)i]:(_tmp95[i]=t2);}}0;});_tag_fat(_tmp95,sizeof(const struct Cyc_Dict_T*),_tmp96);});
if(l != (const struct Cyc_Dict_T*)0)((const struct Cyc_Dict_T**)queue.curr)[++ ind]=l;
if(r != (const struct Cyc_Dict_T*)0)((const struct Cyc_Dict_T**)queue.curr)[++ ind]=r;
if(Cyc_Dict_member(d1,k))
ans_tree=(const struct Cyc_Dict_T*)({struct _RegionHandle*_tmp100=d2.r;int(*_tmpFF)(void*,void*)=d2.rel;struct _tuple0 _tmpFE=({struct _tuple0 _tmpCC;_tmpCC.f1=k,({void*_tmpFD=({void*(*_tmpFC)(void*,void*,void*,void*)=f;void*_tmpFB=env;void*_tmpFA=k;void*_tmpF9=Cyc_Dict_lookup(d1,k);_tmpFC(_tmpFB,_tmpFA,_tmpF9,v);});_tmpCC.f2=_tmpFD;});_tmpCC;});Cyc_Dict_ins(_tmp100,_tmpFF,_tmpFE,ans_tree);});}}}{
# 457
struct Cyc_Dict_Dict _tmp99=({struct Cyc_Dict_Dict _tmpCE;_tmpCE.rel=d2.rel,_tmpCE.r=d2.r,_tmpCE.t=ans_tree;_tmpCE;});_npop_handler(0);return _tmp99;}
# 442
;_pop_region();}}
# 460
static void*Cyc_Dict_intersect_f(void*(*f)(void*,void*,void*),void*a,void*b1,void*b2){
return f(a,b1,b2);}
# 464
struct Cyc_Dict_Dict Cyc_Dict_intersect(void*(*f)(void*,void*,void*),struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 467
return({(struct Cyc_Dict_Dict(*)(void*(*)(void*(*)(void*,void*,void*),void*,void*,void*),void*(*)(void*,void*,void*),struct Cyc_Dict_Dict,struct Cyc_Dict_Dict))Cyc_Dict_intersect_c;})(Cyc_Dict_intersect_f,f,d1,d2);}
# 470
static struct Cyc_List_List*Cyc_Dict_to_list_f(struct _RegionHandle*r,void*k,void*v,struct Cyc_List_List*accum){
# 472
return({struct Cyc_List_List*_tmp9B=_region_malloc(r,sizeof(*_tmp9B));({struct _tuple0*_tmp101=({struct _tuple0*_tmp9A=_region_malloc(r,sizeof(*_tmp9A));_tmp9A->f1=k,_tmp9A->f2=v;_tmp9A;});_tmp9B->hd=_tmp101;}),_tmp9B->tl=accum;_tmp9B;});}
# 475
struct Cyc_List_List*Cyc_Dict_rto_list(struct _RegionHandle*r,struct Cyc_Dict_Dict d){
return({(struct Cyc_List_List*(*)(struct Cyc_List_List*(*)(struct _RegionHandle*,void*,void*,struct Cyc_List_List*),struct _RegionHandle*,struct Cyc_Dict_Dict,struct Cyc_List_List*))Cyc_Dict_fold_c;})(Cyc_Dict_to_list_f,r,d,0);}
# 478
struct Cyc_List_List*Cyc_Dict_to_list(struct Cyc_Dict_Dict d){
return Cyc_Dict_rto_list(Cyc_Core_heap_region,d);}struct _tuple7{int(*f1)(void*,void*);struct _RegionHandle*f2;};
# 482
static struct Cyc_Dict_Dict*Cyc_Dict_filter_f(struct _tuple7*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 484
struct _tuple7*_tmp9C=env;void*_tmp9E;void*_tmp9D;_tmp9D=_tmp9C->f1;_tmp9E=_tmp9C->f2;{int(*f)(void*,void*)=_tmp9D;struct _RegionHandle*r=_tmp9E;
if(f(x,y))
({struct Cyc_Dict_Dict _tmp102=Cyc_Dict_insert(*acc,x,y);*acc=_tmp102;});
# 488
return acc;}}
# 491
struct Cyc_Dict_Dict Cyc_Dict_rfilter(struct _RegionHandle*r2,int(*f)(void*,void*),struct Cyc_Dict_Dict d){
struct _tuple7 _tmp9F=({struct _tuple7 _tmpCF;_tmpCF.f1=f,_tmpCF.f2=r2;_tmpCF;});struct _tuple7 env=_tmp9F;
struct Cyc_Dict_Dict _tmpA0=Cyc_Dict_rempty(r2,d.rel);struct Cyc_Dict_Dict res=_tmpA0;
return*({(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple7*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple7*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;})(Cyc_Dict_filter_f,& env,d,& res);}
# 502
struct Cyc_Dict_Dict Cyc_Dict_filter(int(*f)(void*,void*),struct Cyc_Dict_Dict d){
return Cyc_Dict_rfilter(Cyc_Core_heap_region,f,d);}struct _tuple8{int(*f1)(void*,void*,void*);void*f2;struct _RegionHandle*f3;};
# 507
static struct Cyc_Dict_Dict*Cyc_Dict_filter_c_f(struct _tuple8*env,void*x,void*y,struct Cyc_Dict_Dict*acc){
# 510
struct _tuple8*_tmpA1=env;void*_tmpA4;void*_tmpA3;void*_tmpA2;_tmpA2=_tmpA1->f1;_tmpA3=_tmpA1->f2;_tmpA4=_tmpA1->f3;{int(*f)(void*,void*,void*)=_tmpA2;void*f_env=_tmpA3;struct _RegionHandle*r2=_tmpA4;
if(f(f_env,x,y))
({struct Cyc_Dict_Dict _tmp103=Cyc_Dict_insert(*acc,x,y);*acc=_tmp103;});
return acc;}}
# 516
struct Cyc_Dict_Dict Cyc_Dict_rfilter_c(struct _RegionHandle*r2,int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 518
struct _tuple8 _tmpA5=({struct _tuple8 _tmpD0;_tmpD0.f1=f,_tmpD0.f2=f_env,_tmpD0.f3=r2;_tmpD0;});struct _tuple8 env=_tmpA5;
struct Cyc_Dict_Dict _tmpA6=Cyc_Dict_rempty(r2,d.rel);struct Cyc_Dict_Dict res=_tmpA6;
return*({(struct Cyc_Dict_Dict*(*)(struct Cyc_Dict_Dict*(*)(struct _tuple8*,void*,void*,struct Cyc_Dict_Dict*),struct _tuple8*,struct Cyc_Dict_Dict,struct Cyc_Dict_Dict*))Cyc_Dict_fold_c;})(Cyc_Dict_filter_c_f,& env,d,& res);}
# 522
struct Cyc_Dict_Dict Cyc_Dict_filter_c(int(*f)(void*,void*,void*),void*f_env,struct Cyc_Dict_Dict d){
# 524
return Cyc_Dict_rfilter_c(Cyc_Core_heap_region,f,f_env,d);}
# 528
static int Cyc_Dict_difference_f(struct Cyc_Dict_Dict*d,void*x,void*y){
return !Cyc_Dict_member(*d,x);}
# 532
struct Cyc_Dict_Dict Cyc_Dict_rdifference(struct _RegionHandle*r2,struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
# 534
return({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct Cyc_Dict_Dict*,void*,void*),struct Cyc_Dict_Dict*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;})(r2,Cyc_Dict_difference_f,& d2,d1);}
# 536
struct Cyc_Dict_Dict Cyc_Dict_difference(struct Cyc_Dict_Dict d1,struct Cyc_Dict_Dict d2){
return Cyc_Dict_rdifference(Cyc_Core_heap_region,d1,d2);}struct _tuple9{int(*f1)(void*,void*);void*f2;};
# 540
static int Cyc_Dict_delete_f(struct _tuple9*env,void*x,void*y){
struct _tuple9*_tmpA7=env;void*_tmpA9;void*_tmpA8;_tmpA8=_tmpA7->f1;_tmpA9=_tmpA7->f2;{int(*rel)(void*,void*)=_tmpA8;void*x0=_tmpA9;
return rel(x0,x)!= 0;}}
# 545
struct Cyc_Dict_Dict Cyc_Dict_rdelete(struct _RegionHandle*r2,struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return Cyc_Dict_rcopy(r2,d);{
struct _tuple9 _tmpAA=({struct _tuple9 _tmpD1;_tmpD1.f1=d.rel,_tmpD1.f2=x;_tmpD1;});struct _tuple9 env=_tmpAA;
return({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;})(r2,Cyc_Dict_delete_f,& env,d);}}
# 551
struct Cyc_Dict_Dict Cyc_Dict_rdelete_same(struct Cyc_Dict_Dict d,void*x){
if(!Cyc_Dict_member(d,x))return d;{
struct _tuple9 _tmpAB=({struct _tuple9 _tmpD2;_tmpD2.f1=d.rel,_tmpD2.f2=x;_tmpD2;});struct _tuple9 env=_tmpAB;
return({(struct Cyc_Dict_Dict(*)(struct _RegionHandle*,int(*)(struct _tuple9*,void*,void*),struct _tuple9*,struct Cyc_Dict_Dict))Cyc_Dict_rfilter_c;})(d.r,Cyc_Dict_delete_f,& env,d);}}
# 557
struct Cyc_Dict_Dict Cyc_Dict_delete(struct Cyc_Dict_Dict d,void*x){
return Cyc_Dict_rdelete(Cyc_Core_heap_region,d,x);}struct _tuple10{struct _fat_ptr f1;int f2;};
# 561
int Cyc_Dict_iter_f(struct _tuple10*stk,struct _tuple0*dest){
# 564
struct _tuple10*_tmpAC=stk;void*_tmpAE;struct _fat_ptr _tmpAD;_tmpAD=_tmpAC->f1;_tmpAE=(int*)& _tmpAC->f2;{struct _fat_ptr stack=_tmpAD;int*indp=(int*)_tmpAE;
int _tmpAF=*indp;int ind=_tmpAF;
if(ind == -1)
return 0;{
const struct Cyc_Dict_T*_tmpB0=((const struct Cyc_Dict_T**)stack.curr)[ind];const struct Cyc_Dict_T*t=_tmpB0;
*dest=((const struct Cyc_Dict_T*)_check_null(t))->key_val;
-- ind;
if((unsigned)t->left)
((const struct Cyc_Dict_T**)stack.curr)[++ ind]=t->left;
if((unsigned)t->right)
((const struct Cyc_Dict_T**)stack.curr)[++ ind]=t->right;
*indp=ind;
return 1;}}}
# 579
struct Cyc_Iter_Iter Cyc_Dict_make_iter(struct _RegionHandle*rgn,struct Cyc_Dict_Dict d){
# 584
int half_max_size=1;
const struct Cyc_Dict_T*_tmpB1=d.t;const struct Cyc_Dict_T*t=_tmpB1;
while(t != (const struct Cyc_Dict_T*)0){
t=t->left;
++ half_max_size;}
# 590
t=d.t;{
struct _fat_ptr _tmpB2=({unsigned _tmpB5=(unsigned)(2 * half_max_size);const struct Cyc_Dict_T**_tmpB4=({struct _RegionHandle*_tmp104=rgn;_region_malloc(_tmp104,_check_times(_tmpB5,sizeof(const struct Cyc_Dict_T*)));});({{unsigned _tmpD4=(unsigned)(2 * half_max_size);unsigned i;for(i=0;i < _tmpD4;++ i){_tmpB4[i]=t;}}0;});_tag_fat(_tmpB4,sizeof(const struct Cyc_Dict_T*),_tmpB5);});struct _fat_ptr stack=_tmpB2;
return({struct Cyc_Iter_Iter _tmpD3;({struct _tuple10*_tmp105=({struct _tuple10*_tmpB3=_region_malloc(rgn,sizeof(*_tmpB3));_tmpB3->f1=stack,(unsigned)t?_tmpB3->f2=0:(_tmpB3->f2=-1);_tmpB3;});_tmpD3.env=_tmp105;}),_tmpD3.next=Cyc_Dict_iter_f;_tmpD3;});}}
# 600
void*Cyc_Dict_marshal(struct _RegionHandle*rgn,void*env,void*(*write_key)(void*,struct Cyc___cycFILE*,void*),void*(*write_val)(void*,struct Cyc___cycFILE*,void*),struct Cyc___cycFILE*fp,struct Cyc_Dict_Dict dict){
# 607
struct Cyc_List_List*dict_list=Cyc_Dict_rto_list(rgn,dict);
int len=({(int(*)(struct Cyc_List_List*))Cyc_List_length;})(dict_list);
# 611
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpB7=_cycalloc(sizeof(*_tmpB7));_tmpB7->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp106=({const char*_tmpB6="Dict::marshal: Write failure";_tag_fat(_tmpB6,sizeof(char),29U);});_tmpB7->f1=_tmp106;});_tmpB7;}));
# 613
while(dict_list != 0){
env=({(void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_key;})(env,fp,(struct _tuple0*)dict_list->hd);
env=({(void*(*)(void*,struct Cyc___cycFILE*,struct _tuple0*))write_val;})(env,fp,(struct _tuple0*)dict_list->hd);
dict_list=dict_list->tl;}
# 618
return env;}
# 621
struct Cyc_Dict_Dict Cyc_Dict_unmarshal(struct _RegionHandle*rgn,void*env,int(*cmp)(void*,void*),void*(*read_key)(void*,struct Cyc___cycFILE*),void*(*read_val)(void*,struct Cyc___cycFILE*),struct Cyc___cycFILE*fp){
# 628
struct Cyc_Dict_Dict dict=Cyc_Dict_empty(cmp);
int len=Cyc_getw(fp);
if(len == -1)
(int)_throw((void*)({struct Cyc_Core_Failure_exn_struct*_tmpB9=_cycalloc(sizeof(*_tmpB9));_tmpB9->tag=Cyc_Core_Failure,({struct _fat_ptr _tmp107=({const char*_tmpB8="Dict::unmarshal: list length is -1";_tag_fat(_tmpB8,sizeof(char),35U);});_tmpB9->f1=_tmp107;});_tmpB9;}));
{int i=0;for(0;i < len;++ i){
void*key=read_key(env,fp);
void*val=read_val(env,fp);
dict=Cyc_Dict_insert(dict,key,val);}}
# 637
return dict;}
